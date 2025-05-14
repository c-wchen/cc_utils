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


#include "cli/cli.h"

void do_accept(int32_t socket_fd);
void *msg_poll(void *arg);
int32_t bind_and_listen(const char *socket_path, int32_t *fd);

#define LOG_ERROR(fmt, ...)   printf("[ERROR] " fmt "[%s:%d]\n", ##__VA_ARGS__, __func__, __LINE__)
#define LOG_INFO(fmt, ...)    printf("[INFO] " fmt "[%s:%d]\n", ##__VA_ARGS__, __func__, __LINE__)
#define LOG_WARN(fmt, ...)    printf("[WARN] " fmt "[%s:%d]\n", ##__VA_ARGS__, __func__, __LINE__)
#define LOG_DEBUG(fmt, ...)   printf("[DEBUG] " fmt "[%s:%d]\n", ##__VA_ARGS__, __func__, __LINE__)

typedef struct cli_handler {
    pthread_t cli_thread;
    int socket_fd;
} cli_handler_t;

cli_handler_t *command_handler = NULL;

typedef struct command {
    const char *subcommand;
    const char *help;
    void (*handler)(int32_t argc, char **argv);
} command_t;

#define CMD_DEF(s, h, d) {.subcommand = s, .help = h, .handler = d}


void cmd_test(int32_t argc, char **argv)
{
    LOG_INFO("argc: %d", argc);
    for (int32_t i = 0; i < argc; i++) {
        LOG_INFO("argv[%d] = %s", i, argv[i]);
    }
    return;
}

command_t commands[1024] = {
    CMD_DEF("test", "test command", cmd_test)
};



int cli_create(const char *name)
{
    cli_handler_t *handler = (cli_handler_t *) malloc(sizeof(cli_handler_t));
    if (!handler) {
        return -ENOSPC;
    }
    pid_t pid = getpid();
    char socket_path[64];
    sprintf(socket_path, "/var/tmp/%s.%d", name, pid);
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
    command_handler = handler;
    return 0;
}

int cli_destroy()
{
    if (command_handler) {
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

        LOG_DEBUG("entry waiting");
        int ret = poll(fds, 1, -1);
        if (ret < 0) {
            if (ret == EINTR) {
                continue;
            }
            LOG_ERROR("poll(2) error: %s", strerror(ret));
            return NULL;
        }
        LOG_DEBUG("entry wake");

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

static int safe_write(int fd, const void *buf, signed int len)
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

void msg_encode(char *buf, char *sub_command, int32_t argc, char **argv)
{
    size_t offset = 0;
    *(uint32_t *)buf = strlen(sub_command) + 1;
    offset += 4;
    offset += sprintf(buf + offset, "%s", sub_command) + 1;
    *(int32_t *)(buf + offset) = argc;

    offset += 4;
    for (int32_t i = 0; i < argc; i++) {
        *(int32_t *)(buf + offset) += strlen(argv[i]) + 1;
        offset += 4;
        offset += sprintf(buf + offset, "%s", argv[i]) + 1;
    }
    return;
}


void msg_decode(char *buf, char *sub_command, int32_t *argc, char **argv)
{
    size_t offset = 0;
    int32_t subcommand_len = *(uint32_t *)(buf + offset);
    offset += 4;
    assert(subcommand_len <= COMMAND_MAX_LEN);

    sub_command = (char *)(buf + offset);
    offset += subcommand_len;

    int32_t decode_argc = *(uint32_t *)(buf + offset);
    offset += 4;
    assert(decode_argc <= COMMAND_MAX_LEN);

    for (int32_t i = 0; i < decode_argc; i++) {
        int32_t arg_len = *(int32_t *)(buf + offset);
        assert(arg_len <= COMMAND_MAX_LEN);
        offset += 4;
        argv[i] = (char *)(buf + offset);
        offset += arg_len;
    }
    
    *argc = decode_argc;
    return;
}

char *comand_output()
{
    return "success";
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
    LOG_DEBUG("calling accept");
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
        
        if (++pos >= sizeof(cmd)) {
            LOG_ERROR("error reading request too long, %d", pos);
            close(connection_fd);
            return;
        }

        if (ret > 0) {
            break;
        }
    }

    char *subcommand;
    int32_t argc;
    char *argv[32] = {0};
    msg_decode(cmd, subcommand, &argc, argv);
    
    int rval = command_execute(subcommand, argc, argv);


    char *output = comand_output();
    int ret = safe_write(connection_fd, output, sizeof(output));
    if (ret < 0) {
        LOG_ERROR("error writing response length %s", strerror(ret));
    }
    close(connection_fd);
}


