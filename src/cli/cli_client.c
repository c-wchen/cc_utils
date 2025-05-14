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


#define PRINT(fmt, ...) printf(fmt "\n", ##__VA_ARGS__)

int main(int argc, char **argv)
{

    if (argc < 3 && argv[0] == NULL) {
        PRINT("invalid parameters.");
        return 0;
    }
    if (argc - 3 > 32) {
        PRINT("too many parameters.");
        return 0;
    }

    for (int i = 0; i < argc; i++) {
        if (strlen(argv[i]) + 1 > 32) {
            PRINT("(%s) parameters to long.", argv[i]);
            return 0;
        }
    }

    const char *cmd_handler_name = argv[1];
    DIR *cmd_dir = opendir("/var/tmp");
    struct dirent *entry;
    struct stat file_stat;
    bool find = false;
    char socket_path[1024];
    while (entry = readdir(cmd_dir)) {
        snprintf(socket_path, 1024, "/var/tmp/%s", entry->d_name);
        if (stat(socket_path, &file_stat) == -1) {
            continue;
        }
        
        if (S_ISSOCK(file_stat.st_mode)) {
            PRINT("entry: %s/%s", entry->d_name, cmd_handler_name);
            if (strncmp(entry->d_name, cmd_handler_name, strlen(cmd_handler_name)) == 0) {
                find = true;
                break;
            }
        }
    }
    if (find) {
        PRINT("find socket path: %s", socket_path);
    } else {
        return -1;
    }
    

    snprintf(socket_path, 128, "/var/tmp/%s", entry->d_name);

    struct sockaddr_un addr;
    memset(&addr, 0, sizeof(addr));
    addr.sun_family = AF_UNIX;
    strncpy(addr.sun_path, socket_path, sizeof(addr.sun_path) - 1);


    int32_t sock_fd = socket(AF_UNIX, SOCK_STREAM, 0);
    if (sock_fd == -1) {
        PRINT("socket create faild (%s)", strerror(errno));
        return errno;
    }

    if (connect(sock_fd, (struct sockaddr *)&addr, sizeof(addr)) == -1) {
        PRINT("socket connect faild (%s)", strerror(errno));
    }


    char buffer[1024];
    msg_encode(buffer, argv[1], argc - 2, argv + 2);
    if (write(sock_fd, buffer, sizeof(buffer)) == -1) {
        PRINT("socket message send faild (%s)", strerror(errno));
        close(sock_fd);
        return 1;
    }

    struct timeval timeout = {.tv_sec = 60, .tv_usec = 0};
    fd_set readfds;
    FD_ZERO(&readfds);
    FD_SET(sock_fd, &readfds);
    
    int ready = select(sock_fd + 1, &readfds, NULL, NULL, &timeout);

    if (ready == -1) {
        PRINT("select faild.");
        close(sock_fd);
        return 1;
    } else if (ready == 0) {
        PRINT("timeout.");
        close(sock_fd);
        return 1;
    }

    memset(buffer, 0, 1024);
    int cnt = safe_read(sock_fd, buffer, 1024);
    if (cnt > 0) {
        PRINT("%s", buffer);
    }

    close(sock_fd);
}
