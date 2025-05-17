#ifndef CLI_H
#define CLI_H

#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif
#define COMMAND_MAX_LEN 32

int32_t cli_create(const char *name);
int32_t cli_destroy();

int32_t cli_register(const char *sub_command, const char *help, void (*handler)(void *cdp, int32_t argc, char** argv));


#ifdef __cplusplus /* CLI_H */
}
#endif
#endif