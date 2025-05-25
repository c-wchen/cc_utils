#ifndef BINLOG_H
#define BINLOG_H

#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

void *binlog_create(const char *name);

void binlog_destroy(void *handle);

void binlog_print(void *handle, int32_t level, const char *func, int32_t line, const char *format, ...);

#define LOG_ERROR 1
#define LOG_WARN  2
#define LOG_INFO  3
#define LOG_DEBUG 4
#define LOG_TRACE 4

static inline const char *binlog_level_name(int32_t level)
{
    switch (level) {
        case LOG_ERROR: {
            return "ERROR";
        }
        case LOG_WARN: {
            return "WARN";
        }
        case LOG_INFO: {
            return "INFO";
        }
        case LOG_DEBUG: {
            return "DEBUG";
        }
        default: {
            return "TRACE";
        }
    }
}

#ifdef __cplusplus /* BINLOG_H */
}
#endif
#endif