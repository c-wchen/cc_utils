#ifndef CLI_INTERNAL_H
#define CLI_INTERNAL_H

#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

#define COMMAND_MAX_ARGC 32
#define COMMAND_MAX_LEN 32
#define COMMAND_TIMEOUT 60
#define SOCKET_PATH_LEN 108
#define SOCKET_PATH_DIR "/var/tmp/"

#define DF_SOCKET_DIR "/var/tmp/%s.%d"


ssize_t safe_read(int fd, void *buf, size_t count);
int safe_write(int fd, const void *buf, signed int len);
uint32_t msg_encode(char *buf, char *sub_command, int32_t argc, char **argv);


void cdp_print(void *cdp, const char *fmt, ...);

#define CMD_PRINT(cdp, fmt, ...) cdp_print(cdp, fmt, ##__VA_ARGS__)
#define CMD_PRINTLN(cdp, fmt, ...) cdp_print(cdp, fmt "\n", ##__VA_ARGS__)

#ifdef __cplusplus /* CLI_INTERNAL_H */
}
#endif
#endif