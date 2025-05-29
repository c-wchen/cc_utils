#ifndef BINLOG_H
#define BINLOG_H

#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

void *binlog_create(const char *name);

void binlog_destroy(void *handle);

void binlog_print(void *handle, int32_t level, const char *func, int32_t line, const char *format, ...);

void binlog_flush(void *handle);

#define LOG_ERROR 1
#define LOG_WARN  2
#define LOG_INFO  3
#define LOG_DEBUG 4
#define LOG_TRACE 5

#define BLOG_ERROR(handle, format, ...)  binlog_print(handle, LOG_ERROR, __func__, __LINE__, format, ##__VA_ARGS__)
#define BLOG_INFO(handle, format, ...)   binlog_print(handle, LOG_INFO, __func__, __LINE__, format, ##__VA_ARGS__)
#define BLOG_WARN(handle, format, ...)   binlog_print(handle, LOG_WARN, __func__, __LINE__, format, ##__VA_ARGS__)
#define BLOG_DEBUG(handle, format, ...)  binlog_print(handle, LOG_DEBUG, __func__, __LINE__, format, ##__VA_ARGS__)
#define BLOG_TRACE(handle, format, ...)  binlog_print(handle, LOG_TRACE, __func__, __LINE__, format, ##__VA_ARGS__)

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