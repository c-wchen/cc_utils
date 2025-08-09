
#include <fcntl.h>
#include <unistd.h>
#include <time.h>
#include <string.h>
#include <stdlib.h>
#include <stdarg.h>
#include <errno.h>
#include <stdint.h>
#include <stdio.h>
#include <assert.h>
#include <pthread.h>

#include "binlog/binlog.h"
#include "binlog_internal.h"

__thread tsd_binlog_t tbinlog;

#define FUNC_INT      print_int
#define FUNC_LONG     print_long
#define FUNC_DOUBLE   print_double
#define FUNC_STRING   print_string
#define FUNC_POINTER  print_pointer

#define MAX(a, b) (a) > (b) ? (a) : (b)

static inline void binlog_maybe_realloc(int32_t size)
{
    if (tbinlog.offset + size > tbinlog.size) {
        if (tbinlog.size == BINLOG_STACK_SIZE) {
            void *buf = tbinlog.buf;
            int32_t alloc_size = MAX(2 * BINLOG_STACK_SIZE, tbinlog.offset + size + BINLOG_STACK_SIZE - 1 / BINLOG_STACK_SIZE);
            tbinlog.size = alloc_size;
            tbinlog.buf = malloc(alloc_size);
            memcpy(tbinlog.buf, buf, BINLOG_STACK_SIZE);
        } else {
            int32_t alloc_size = MAX(2 * tbinlog.size, tbinlog.offset + size + BINLOG_STACK_SIZE - 1 / BINLOG_STACK_SIZE);
            tbinlog.buf = realloc(tbinlog.buf, alloc_size);
            tbinlog.size = alloc_size;
        }
    }
    return;
}

static inline void binlog_maybe_free(void)
{
    if (tbinlog.size > BINLOG_STACK_SIZE) {
        free(tbinlog.buf);
    }
    tbinlog.size = 0;
    tbinlog.buf = NULL;
    tbinlog.offset = 0;
}

static inline int print_int(int32_t val)
{
    int result = 1 + sizeof(int32_t);
    binlog_maybe_realloc(result);
    *(uint8_t *)(tbinlog.buf + tbinlog.offset) = BLOG_INT;
    tbinlog.offset += 1;
    *(int32_t *)(tbinlog.buf + tbinlog.offset) = val;
    tbinlog.offset += 4;
    return result;
}

static inline int print_long(int64_t val)
{
    int result = 1 + sizeof(int64_t);
    binlog_maybe_realloc(result);
    *(uint8_t *)(tbinlog.buf + tbinlog.offset) = BLOG_LONG;
    tbinlog.offset += 1;
    *(int64_t *)(tbinlog.buf + tbinlog.offset) = val;
    tbinlog.offset += 8;
    return result;
}

static inline int print_double(double val)
{
    int result = 1 + sizeof(double);
    binlog_maybe_realloc(result);
    *(uint8_t *)(tbinlog.buf + tbinlog.offset) = BLOG_DOUBLE;
    tbinlog.offset += 1;
    *(double *)(tbinlog.buf + tbinlog.offset) = val;
    tbinlog.offset += 8;
    return result;
}

static inline int print_pointer(void *val)
{
    int result = 1 + sizeof(void *);
    binlog_maybe_realloc(result);
    *(uint8_t *)(tbinlog.buf + tbinlog.offset) = BLOG_POINTER;
    tbinlog.offset += 1;
    *(uint64_t *)(tbinlog.buf + tbinlog.offset) = (uint64_t)val;
    tbinlog.offset += 8;
    return result;
}

static inline int print_string(char *val)
{
    int result = strlen(val) + 1 + 1;
    int len = strlen(val);
    binlog_maybe_realloc(result);
    *(uint8_t *)(tbinlog.buf + tbinlog.offset) = BLOG_STRING;
    tbinlog.offset += 1;
    memcpy(tbinlog.buf + tbinlog.offset, val, len + 1);
    tbinlog.offset += len + 1;
    return result;
}

static inline int print_begin(uint8_t *val, uint32_t len)
{
    *(uint8_t *)(tbinlog.buf + tbinlog.offset) = BLOG_BEGIN;
    tbinlog.offset += 1;
    memcpy(tbinlog.buf + tbinlog.offset, val, len);
    tbinlog.offset += len;
    return len + 1;
}

static inline int print_end(int32_t fd, int32_t valog_len)
{
    binlog_maybe_realloc(sizeof(uint8_t));
    *(int32_t *)(tbinlog.buf + sizeof(uint8_t)) = valog_len;
    *(uint8_t *)(tbinlog.buf + tbinlog.offset) = BLOG_END;
    tbinlog.offset += 1;

    write(fd, tbinlog.buf, tbinlog.offset);

    binlog_maybe_free();
    return sizeof(uint8_t);
}

#define PRINT_BINTYPE(BTYPE)                                        \
    do {                                                            \
        TYPE_BLOG_##BTYPE value = va_arg (ap, TYPE_BLOG_##BTYPE);   \
        int result = FUNC_##BTYPE(value);                           \
        ptr++;                                                      \
        total_printed += result;                                    \
    } while (0)

int do_valog(const char *format, va_list ap)
{
    const char *ptr = format;
    int total_printed = 0;

    while (*ptr != '\0') {
        if (*ptr != '%') { /* While we have regular characters, print them.  */
            ptr++;
        } else { /* We got a format specifier! */
            int wide_width = 0, short_width = 0;
            ptr++;
            while (strchr("-+ #0", *ptr)) { /* Move past flags.  */
                ptr++;
            }

            if (*ptr == '*') {
                PRINT_BINTYPE(INT);
            } else
                while (ISDIGIT(*ptr)) { /* Handle explicit numeric value.  */
                    ptr++;
                }

            if (*ptr == '.') {
                ptr++; /* Copy and go past the period.  */
                if (*ptr == '*') {
                    PRINT_BINTYPE(INT);
                    ptr++;
                } else
                    while (ISDIGIT(*ptr)) { /* Handle explicit numeric value.  */
                        ptr++;
                    }
            }
            while (strchr("hlL", *ptr)) {
                switch (*ptr) {
                    case 'h':
                        short_width = 1;
                        break;
                    case 'l':
                        wide_width++;
                        break;
                    case 'L':
                        wide_width = 2;
                        break;
                    default:
                        abort();
                }
                ptr++;
            }

            switch (*ptr) {
                case 'd':
                case 'i':
                case 'o':
                case 'u':
                case 'x':
                case 'X':
                case 'c': {
                    /* Short values are promoted to int, so just copy it
                               as an int and trust the C library printf to cast it
                               to the right width.  */
                    if (short_width) {
                        PRINT_BINTYPE(INT);
                    } else {
                        switch (wide_width) {
                            case 0:
                                PRINT_BINTYPE(INT);
                                break;
                            case 1:
                                PRINT_BINTYPE(LONG);
                                break;
                            case 2:
                            default:
                                PRINT_BINTYPE(LONG);
                                break;
                        } /* End of switch (wide_width) */
                    } /* End of else statement */
                    } /* End of integer case */
                break;
                case 'f':
                case 'e':
                case 'E':
                case 'g':
                case 'G': {
                    if (wide_width == 0) {
                        PRINT_BINTYPE(DOUBLE);
                    } else {
                        PRINT_BINTYPE(DOUBLE); /* Fake it and hope for the best.  */
                    }
                }
                break;
                case 's':
                    PRINT_BINTYPE(STRING);
                    break;
                case 'p':
                    PRINT_BINTYPE(POINTER);
                    break;
                case '%':
                    break;
                default:
                    abort();
            } /* End of switch (*ptr) */
        } /* End of else statement */
    }

    return total_printed;
}

void *binlog_create(const char *name)
{
    binlog_handle_t *handle = (binlog_handle_t *)malloc(sizeof(binlog_handle_t));
    if (handle == NULL) {
        fprintf(stderr, "creatre handle faild(%d).", -ENOMEM);
        return NULL;
    }
    int32_t off = snprintf(handle->name, sizeof(handle->name), "/var/log/tmp/%s", name);

    if (off < 0 || off >= sizeof(handle->name)) {
        fprintf(stderr, "name too long(%d).", -ENAMETOOLONG);
        free(handle);
        return NULL;
    }

    handle->fd = open(handle->name, O_WRONLY | O_CREAT | O_APPEND, 0644);

    if (handle->fd < 0) {
        fprintf(stderr, "open file %s faild(%d).", handle->name, -errno);
        free(handle);
        return NULL;
    }
    return handle;
}

void binlog_destroy(void *handle)
{
    binlog_handle_t *internal_handle = (binlog_handle_t *)handle;
    close(internal_handle->fd);
    free(internal_handle);
    return;
}

void binlog_print(void *handle, int32_t level, const char *func, int32_t line, const char *format, ...)
{
    if (handle == NULL) {
        return;
    }
    char buf[BINLOG_STACK_SIZE];
    tbinlog.buf = buf;
    tbinlog.size = BINLOG_STACK_SIZE;
    tbinlog.offset = 0;

    binlog_handle_t *internal_handle = (binlog_handle_t *)handle;

    struct timespec ts;
    clock_gettime(CLOCK_MONOTONIC_COARSE, &ts);
    time_t now = ts.tv_sec * (time_t)(1e6) + ts.tv_nsec / (time_t)(1e3);

    const int32_t size = sizeof(int32_t) /* length */ + 1 /* level */ + strlen(func) + 1 /* func */  +
                  sizeof(line) /* line */ + sizeof(pthread_t) /* thread */ + sizeof(time_t) /* time */;

    uint8_t prefix[size];
    size_t offset = 0;

    /* length */
    *(int32_t *)(prefix + offset) = 0;
    offset += sizeof(int32_t);

    /* level */
    *(uint8_t *)(prefix + offset) = (uint8_t)level;
    offset += sizeof(uint8_t);

    /* function */
    memcpy(prefix + offset, func, strlen(func) + 1);
    offset += strlen(func) + 1;

    /* line */
    *(int32_t *)(prefix + offset) = line;
    offset += sizeof(line);

    /* thread */
    *(uint64_t *)(prefix + offset) = pthread_self();
    offset += sizeof(uint64_t);

    /* timestamp */
    *(time_t *)(prefix + offset) = now;
    offset += sizeof(time_t);

    assert(size == offset);
    /* begin */
    print_begin(prefix, size);
    va_list ap;
    va_start(ap, format);
    int32_t va_length = do_valog(format, ap);
    va_end(ap);

    /* end */
    print_end(internal_handle->fd, va_length + size + sizeof(uint8_t) /* begin type */  + sizeof(uint8_t) /* end type */);
    return;
}