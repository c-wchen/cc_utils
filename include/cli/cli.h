#ifndef CLI_H
#define CLI_H

#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif
#define COMMAND_MAX_LEN 32

int32_t cli_create(const char *name);
int32_t cli_destroy();

int32_t cli_register(const char *sub_command, const char *help, void (*execute)(void *cdp, int32_t argc, char **argv));

void cdp_print(void *cdp, const char *fmt, ...);

#define CMD_PRINT(cdp, fmt, ...) cdp_print(cdp, fmt, ##__VA_ARGS__)
#define CMD_PRINTLN(cdp, fmt, ...) cdp_print(cdp, fmt "\n", ##__VA_ARGS__)

#ifdef __cplusplus /* CLI_H */
}
#endif
#endif