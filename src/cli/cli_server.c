#define _GNU_SOURCE 
#include <stdio.h>
#include <pthread.h>
#include <stdlib.h>
#include <stdbool.h>
#include <unistd.h>
#include <string.h>
#include <errno.h>
#include <assert.h>


#include <sys/un.h>
#include <sys/socket.h>
#include <sys/epoll.h>
#include <poll.h>

 #include <signal.h>

 #include <stdarg.h>

#include "cli/cli.h"
#include "cli_internal.h"

void do_accept(int32_t socket_fd);
void *msg_poll(void *arg);
int32_t bind_and_listen(const char *socket_path, int32_t *fd);

#define LOG_ERROR(fmt, ...)   printf("[ERROR] " fmt " [%s:%d]\n", ##__VA_ARGS__, __func__, __LINE__)
#define LOG_INFO(fmt, ...)    printf("[INFO] " fmt " [%s:%d]\n", ##__VA_ARGS__, __func__, __LINE__)
#define LOG_WARN(fmt, ...)    printf("[WARN] " fmt " [%s:%d]\n", ##__VA_ARGS__, __func__, __LINE__)
#define LOG_DEBUG(fmt, ...)   printf("[DEBUG] " fmt " [%s:%d]\n", ##__VA_ARGS__, __func__, __LINE__)

typedef struct cli_handler {
    char name[32];
    pthread_t cli_thread;
    int socket_fd;
} cli_handler_t;

cli_handler_t *command_handler = NULL;

typedef struct command {
    const char *subcommand;
    const char *help;
    void (*handler)(int32_t argc, char **argv);
} command_t;

typedef struct {
    pthread_mutex_t mutex;
    char *buf;
    int32_t buf_size;
    int32_t pos;
} cmdprint_t;

cmdprint_t cdp = {0};

#define CMD_DEF(s, h, d) {.subcommand = s, .help = h, .handler = d}

void cmd_test(int32_t argc, char **argv)
{
    CMD_PRINTLN("argc: %d", argc);
    for (int32_t i = 0; i < argc; i++) {
        CMD_PRINTLN("argv[%d] = %s", i, argv[i]);
    }
    return;
}

command_t commands[1024] = {
    CMD_DEF("test", "test command", cmd_test)
};


void remove_all_cleanup_files()
{
    LOG_DEBUG("begin clean files");
    pid_t pid = getpid();
    char socket_path[64];
    sprintf(socket_path, DF_SOCKET_DIR, command_handler->name, pid);
    unlink(socket_path);
}

void common_signal(int sig)
{
    exit(-1);
}

int cli_create(const char *name)
{
    if (strlen(name) + 1 > 32) {
        LOG_ERROR("cli name is too long %s.", name);
        return -EINVAL;
    }
    cli_handler_t *handler = (cli_handler_t *) malloc(sizeof(cli_handler_t));
    if (!handler) {
        return -ENOSPC;
    }
    pid_t pid = getpid();
    char socket_path[SOCKET_PATH_LEN];
    sprintf(socket_path, DF_SOCKET_DIR, name, pid);
    int ret = bind_and_listen(socket_path, &handler->socket_fd);
    if (ret < 0) {
        free(handler);
        return ret;
    }
    LOG_DEBUG("bind and listen success.");
    ret = pthread_create(&handler->cli_thread, NULL, msg_poll, handler);

    if (ret < 0) {
        LOG_ERROR("pthread create faild (%s).", strerror(ret));
        close(handler->socket_fd);
        free(handler);
        return ret;
    }

    memcpy(&handler->name, name, strlen(name) + 1);
    command_handler = handler;

    cdp_init();

    atexit(remove_all_cleanup_files);
    signal(SIGINT, common_signal);
    return 0;
}


int cli_destroy()
{
    if (command_handler) {
        cdp_destroy();
        close(command_handler->socket_fd);
        free(command_handler);
        command_handler = NULL;
    }
    return 0;
}

int32_t bind_and_listen(const char *socket_path, int32_t *fd)
{
    struct sockaddr_un address;
    int ret = 0;
    if (strlen(socket_path) > sizeof(address.sun_path) - 1) {
        LOG_ERROR("socket path %s is too long!", socket_path);
        return -EINVAL;
    }
    int socket_fd = socket(PF_UNIX, SOCK_STREAM, 0);

    if (socket_fd < 0) {
        ret = errno;
        LOG_ERROR("failed to create socket: %s", strerror(ret));
        return ret;
    }

    memset(&address, 0, sizeof(struct sockaddr_un));
    address.sun_family = AF_UNIX;
    snprintf(address.sun_path, sizeof(address.sun_path), "%s", socket_path);

    if (bind(socket_fd, (struct sockaddr *)&address, sizeof(struct sockaddr_un)) != 0) {
        ret = errno;
        if (ret == EADDRINUSE) {
            // The old UNIX domain socket must still be there.
            // Let's unlink it and try again.
            if (bind(socket_fd, (struct sockaddr *)&address, sizeof(struct sockaddr_un)) == 0) {
                ret = 0;
            } else {
                ret = errno;
            }
        }
        if (ret != 0) {
            LOG_ERROR("failed to bind the UNIX domain socket to %s, ret: %s", socket_path, strerror(ret));
            close(socket_fd);
            return ret;
        }
    }

    if (listen(socket_fd, 5) != 0) {
        ret = errno;
        LOG_ERROR("failed to listen to socket: %s", strerror(ret));
        close(socket_fd);
        return ret;
    }
    *fd = socket_fd;
    return ret;
}


void *msg_poll(void *arg)
{

    cli_handler_t *handler = (cli_handler_t *)arg;
    while (true) {
        struct pollfd fds[1];
        // FIPS zeroization audit 20191115: this memset is fine.
        memset(fds, 0, sizeof(fds));
        fds[0].fd = handler->socket_fd;
        fds[0].events = POLLIN | POLLRDBAND;

        LOG_DEBUG("start waiting");
        int ret = poll(fds, 1, -1);
        if (ret < 0) {
            if (ret == EINTR) {
                continue;
            }
            LOG_ERROR("poll(2) error: %s", strerror(ret));
            return NULL;
        }
        LOG_DEBUG("end wake");

        if (fds[0].revents & POLLIN) {
            // Send out some data
            do_accept(fds[0].fd);
        }
    }
    return NULL;
}

ssize_t safe_read(int fd, void *buf, size_t count)
{
    size_t cnt = 0;

    while (cnt < count) {
        ssize_t r = read(fd, buf, count - cnt);
        if (r <= 0) {
            if (r == 0) {
                // EOF
                return cnt;
            }
            if (errno == EINTR) {
                continue;
            }
            return -errno;
        }
        cnt += r;
        buf = (char *)buf + r;
    }
    return cnt;
}

int safe_write(int fd, const void *buf, signed int len)
{
    const char *b = (const char*)buf;
    /* Handle EINTR and short writes */
    while (1) {
        int res = write(fd, b, len);
        if (res < 0) {
            int err = errno;
            if (err != EINTR) {
                return err;
            }
        }
        len -= res;
        b += res;
        if (len <= 0)
            return 0;
    }
}

uint32_t msg_encode(char *buf, char *sub_command, int32_t argc, char **argv)
{
    LOG_DEBUG("subcommand: %s, argc: %d", sub_command, argc);
    size_t offset = 0;
    *(uint32_t *)buf = strlen(sub_command) + 1;
    offset += 4;
    offset += sprintf(buf + offset, "%s", sub_command) + 1;
    *(uint32_t *)(buf + offset) = argc;

    offset += 4;
    for (uint32_t i = 0; i < argc; i++) {
        *(uint32_t *)(buf + offset) = strlen(argv[i]) + 1;
        offset += 4;
        offset += sprintf(buf + offset, "%s", argv[i]) + 1;
    }
    *(buf + offset) = '\n'; /* \n terminated string */
    return offset + 1;
}


uint32_t msg_decode(char *buf, char **sub_command, int32_t *argc, char **argv)
{
    size_t offset = 0;
    uint32_t subcommand_len = *(uint32_t *)(buf + offset);
    offset += 4;
    assert(subcommand_len <= COMMAND_MAX_LEN);

    *sub_command = (char *)(buf + offset);
    offset += subcommand_len;

    uint32_t decode_argc = *(uint32_t *)(buf + offset);
    offset += 4;
    assert(decode_argc <= COMMAND_MAX_LEN);

    for (uint32_t i = 0; i < decode_argc; i++) {
        uint32_t arg_len = *(uint32_t *)(buf + offset);
        assert(arg_len <= COMMAND_MAX_LEN);
        offset += 4;
        argv[i] = (char *)(buf + offset);
        offset += arg_len;
    }
    *argc = decode_argc;
    return offset;
}

char *command_output()
{
    return cdp_output();
}


command_t *command_find(const char *subcommand)
{
    for (int32_t i = 0; i < 1024; i++) {
        if (commands[i].subcommand == NULL) {
            return NULL;
        }
        if (strncmp(commands[i].subcommand, subcommand, COMMAND_MAX_LEN) == 0) {
            return &commands[i];
        }
    }
    return NULL;
}

int32_t command_execute(const char*subcommand, int32_t argc, char **argv)
{
    command_t *cmd = command_find(subcommand);
    if (cmd) {
        cmd->handler(argc, argv);
    }
    return 0;
}

void do_accept(int32_t socket_fd)
{
    struct sockaddr_un address;
    socklen_t address_length = sizeof(address);
    LOG_DEBUG("begin accept");
    int connection_fd = accept(socket_fd, (struct sockaddr*) &address, &address_length);
    if (connection_fd < 0) {
        int err = errno;
        LOG_ERROR("do_accept error: %s", strerror(err));
        return;
    }
    LOG_DEBUG("finished accept");

    char cmd[1024];
    unsigned pos = 0;
    while (1) {
        int ret = safe_read(connection_fd, &cmd[pos], 1);
        if (ret <= 0) {
            if (ret < 0) {
                LOG_ERROR("error reading request code: %s", strerror(ret));
            }
            close(connection_fd);
            return;
        }
        // new protocol: \n terminated string
        if (cmd[pos] == '\n') {
            break;
        }
        if (++pos >= sizeof(cmd)) {
            LOG_ERROR("error reading request too long, %d", pos);
            close(connection_fd);
            return;
        }
    }

    char *subcommand;
    int32_t argc;
    char *argv[32] = {0};
    msg_decode(cmd, &subcommand, &argc, argv);

    LOG_DEBUG("begin execute %d", argc);
    
    int rval = command_execute(subcommand, argc, argv);

    LOG_DEBUG("end execute");


    char *output = command_output();
    int ret = safe_write(connection_fd, output, strlen(output));
    if (ret < 0) {
        LOG_ERROR("error writing response length %s", strerror(ret));
    }
    cdp_reinit();
    close(connection_fd);
}




void cdp_init()
{
    pthread_mutex_init(&cdp.mutex, NULL);
    
    cdp.buf = (char *)malloc(sizeof(char) * 4096);
    cdp.buf_size = 4096;
    cdp.pos = 0;
}

void cdp_reinit()
{
    
    if (cdp.buf == NULL) {
        return;
    }
    pthread_mutex_lock(&cdp.mutex);
    if (cdp.buf_size > 10240) {
        free(cdp.buf);
        cdp.buf = (char *)malloc(sizeof(char) * 4096);
        cdp.buf_size = 4096;
    }
    cdp.pos = 0;
    pthread_mutex_unlock(&cdp.mutex);
}

void cdp_destroy()
{
    if (cdp.buf) {
        pthread_mutex_destroy(&cdp.mutex);
        free(cdp.buf);
    }
    cdp.pos = 0;
    cdp.buf_size = 0;
    return;
}

char *cdp_output()
{
    if (cdp.pos > 0) {
        return cdp.buf;
    } else {
        return "success";
    }
}

void cdp_print(const char *fmt, ...)
{
    va_list ap;
    
    char buffer[1024];

    va_start(ap, fmt);
    int off = vsnprintf(buffer, 1024, fmt, ap);
    va_end(ap);

    if (off <= 0 || off + 1 > 1024) {
        LOG_ERROR("onetime print too long, size: %d", off);
        return;
    }

    pthread_mutex_lock(&cdp.mutex);
    if (off + cdp.pos >= cdp.buf_size) {
        cdp.buf = (char *)realloc(cdp.buf, cdp.buf_size * 2);
        cdp.buf_size = cdp.buf_size * 2;
    }

    memcpy(cdp.buf + cdp.pos, buffer, off);
    cdp.pos += off;
    *(cdp.buf + cdp.pos + 1) = '\0';
    pthread_mutex_unlock(&cdp.mutex);
    return;
}