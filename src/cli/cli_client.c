#define _GNU_SOURCE 
#include <stdio.h>
#include <pthread.h>
#include <stdlib.h>
#include <stdbool.h>
#include <unistd.h>
#include <string.h>
#include <errno.h>
#include <assert.h>

#include<sys/types.h>
#include <sys/un.h>
#include <sys/socket.h>
#include <sys/epoll.h>
#include <sys/stat.h>
#include <poll.h>
#include<dirent.h>

#include "cli_internal.h"


#define PRINTLN(fmt, ...) printf(fmt "\n", ##__VA_ARGS__)
#define PRINT(fmt, ...) printf(fmt "\n", ##__VA_ARGS__)

int is_directory(const char *path) {
    struct stat statbuf;
    
    if (lstat(path, &statbuf) != 0) {
        return 0;
    }
    
    return S_ISDIR(statbuf.st_mode);
}

bool pid_exist(const char* pid) {
    char *e;
    uint64_t v = strtol(pid, &e, 10);
    if ('\0' != *e) {
        return false;
    } else {
        char proc_path[64];
        snprintf(proc_path, 64, "/proc/%d", v);
        if (is_directory(proc_path)) {
            return true;
        } else {
            return false;
        }
        
    }
}

int main(int argc, char **argv)
{

    if (argc < 3) {
        PRINTLN("invalid parameters.");
        return -EINVAL;
    }
    if (argc - 3 > COMMAND_MAX_ARGC) {
        PRINTLN("too many parameters.");
        return -EINVAL;
    }

    for (int i = 0; i < argc; i++) {
        if (strlen(argv[i]) + 1 > COMMAND_MAX_LEN) {
            PRINTLN("(%s) parameters to long.", argv[i]);
            return -EINVAL;
        }
    }

    const char *proc_name = argv[1];
    DIR *cmd_dir = opendir(SOCKET_PATH_DIR);
    struct dirent *entry;
    struct stat file_stat;
    bool find = false;
    char socket_path[SOCKET_PATH_LEN];
    while (entry = readdir(cmd_dir)) {
        int32_t offset = snprintf(socket_path, SOCKET_PATH_LEN, SOCKET_PATH_DIR "/%s", entry->d_name);
        if (offset + 1 >= SOCKET_PATH_LEN) {
            continue;
        }
        if (stat(socket_path, &file_stat) == -1) {
            continue;
        }
        if (S_ISSOCK(file_stat.st_mode)) {
            if (strncmp(entry->d_name, proc_name, strlen(proc_name)) == 0) {
                PRINTLN("entry: %s cmd: %s %d", entry->d_name, proc_name, strlen(proc_name));
                if (strlen(entry->d_name) > strlen(proc_name) + 2 &&
                    pid_exist(entry->d_name + strlen(proc_name) + 1)) { /* format: name.pid */
                    find = true;
                    break;
                }
            }
        }
    }
    closedir(cmd_dir);

    if (!find) {
        PRINTLN("not found name %s", proc_name);
        return -ENOENT;
    }

    struct sockaddr_un addr;
    memset(&addr, 0, sizeof(addr));
    addr.sun_family = AF_UNIX;
    strncpy(addr.sun_path, socket_path, sizeof(addr.sun_path) - 1);


    int32_t sock_fd = socket(AF_UNIX, SOCK_STREAM, 0);
    if (sock_fd == -1) {
        PRINTLN("socket create faild (%s)", strerror(errno));
        return errno;
    }

    if (connect(sock_fd, (struct sockaddr *)&addr, sizeof(addr)) == -1) {
        PRINTLN("socket connect faild (%s)", strerror(errno));
    }


    char buffer[1024];
    uint32_t msg_len = msg_encode(buffer, argv[2], argc - 3, argv + 3);
    if (safe_write(sock_fd, buffer, msg_len) == -1) {
        PRINTLN("socket message send faild (%s)", strerror(errno));
        close(sock_fd);
        return 1;
    }
    PRINTLN("reply mesage:");

    struct timeval timeout = {.tv_sec = COMMAND_TIMEOUT, .tv_usec = 0};
    fd_set rfds;
    FD_ZERO(&rfds);
    FD_SET(sock_fd, &rfds);
    
    int ready = select(sock_fd + 1, &rfds, NULL, NULL, &timeout);

    if (ready == -1) {
        PRINTLN("start select faild.");
        close(sock_fd);
        return 1;
    } else if (ready == 0) {
        PRINTLN("command wait timeout.");
        close(sock_fd);
        return 1;
    }
    int32_t pos = 0;

    while (1) {
        int ret = safe_read(sock_fd, &buffer[pos], 1);
        if (ret <= 0) {
            if (ret < 0) {
                PRINTLN("error reading request code: %s", strerror(ret));
            }
            if (pos) {
                buffer[pos] = '\0';
                PRINT("%s", buffer);
                break;
            }
        }
        if (++pos >= sizeof(buffer)) {
            buffer[pos] = '\0';
            PRINT("%s", buffer);
            pos = 0;
        }
    }

    close(sock_fd);
    return 0;
}
