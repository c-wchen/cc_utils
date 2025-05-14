#ifndef CLI_H
#define CLI_H

#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif
#define COMMAND_MAX_LEN 32

ssize_t safe_read(int fd, void *buf, size_t count);
static int safe_write(int fd, const void *buf, signed int len);
void msg_encode(char *buf, char *sub_command, int32_t argc, char **argv);


#ifdef __cplusplus /* CLI_H */
}
#endif
#endif