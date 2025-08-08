
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
#include <sys/stat.h>

#include "binlog/binlog.h"
#include "binlog_internal.h"

typedef struct {
    int32_t fd;
    FILE *out;
    void *buf;
    int32_t size;
    int32_t read_size;
    int32_t offset;

    int32_t file_offset;
    int32_t file_size;
} binlog_parser_t;

typedef struct {
    int32_t len;
    int32_t off;
    void *buf;
} binlog_va_list;


static int32_t binlog_file_size(const char *filename)
{
    struct stat st;
    if (stat(filename, &st) == -1) {
        return -1;
    }
    return st.st_size;
}

static void binlog_parse_read(binlog_parser_t *parser)
{
    int read_size;
    if (parser->file_offset == 0) {
        read_size = read(parser->fd, parser->buf, parser->size);
        assert(read_size > 0);
        parser->file_offset += read_size;
        parser->read_size = read_size;
    } else {
        memcpy(parser->buf, parser->buf + parser->offset, parser->read_size - parser->offset);
        read_size = read(parser->fd, parser->buf + (parser->read_size - parser->offset),
                         parser->size - (parser->read_size - parser->offset));

        assert(read_size > 0);
        parser->file_offset += read_size;
        parser->read_size = read_size + (parser->read_size - parser->offset);
    }
    parser->offset = 0;
    return;
}

void binlog_parser_create(binlog_parser_t *parser, const char *name, const char *output)
{
    char path_name[128];
    int32_t off = snprintf(path_name, sizeof(path_name), "/var/log/tmp/%s", name);

    if (off < 0 || off >= sizeof(path_name)) {
        fprintf(stderr, "name too long(%d).", -ENAMETOOLONG);
        return;
    }

    parser->fd = open(path_name, O_RDONLY, 0644);

    if (parser->fd < 0) {
        fprintf(stderr, "open file %s faild(%d).", path_name, -errno);
        return;
    }

    char output_path_name[128];
    off = snprintf(output_path_name, sizeof(output_path_name), "/var/log/tmp/%s", output);
    parser->out = fopen(output_path_name, "a");

    if (parser->out == NULL) {
        close(parser->fd);
        fprintf(stderr, "open  output file %s faild (%d).", output_path_name, -errno);
        return;
    }

    parser->buf = malloc(1 << 22);
    parser->offset = 0;
    parser->size = 1 << 22;

    parser->file_size = binlog_file_size(path_name);
    if (parser->file_size < 0) {
        fclose(parser->out);
        close(parser->fd);
        fprintf(stderr, "file size get faild (%d).", path_name, parser->file_size);
        return;
    }
    parser->file_offset = 0;

    binlog_parse_read(parser);

    return;
}

void binlog_parser_destroy(binlog_parser_t *bparser)
{
    close(bparser->fd);
    if (bparser->buf != NULL) {
        free(bparser->buf);
    }
    return;
}

static inline int32_t get_int(binlog_va_list *vlist)
{
    if (vlist->off + 1 + sizeof(int32_t) > vlist->len) {
        assert(0);
    }
    assert(*(uint8_t *)(vlist->buf + vlist->off) == BLOG_INT);
    vlist->off += 1;
    int32_t ret = *(int32_t *)(vlist->buf + vlist->off);
    vlist->off += sizeof(int32_t);
    return ret;
}

static inline int64_t get_long(binlog_va_list *vlist)
{
    if (vlist->off + 1 + sizeof(int64_t) > vlist->len) {
        assert(0);
    }
    assert(*(uint8_t *)(vlist->buf + vlist->off) == BLOG_LONG);
    vlist->off += 1;
    int64_t ret = *(int64_t *)(vlist->buf + vlist->off);
    vlist->off += sizeof(int64_t);
    return ret;
}

static inline double get_double(binlog_va_list *vlist)
{
    if (vlist->off + 1 + sizeof(double) > vlist->len) {
        assert(0);
    }
    assert(*(uint8_t *)(vlist->buf + vlist->off) == BLOG_DOUBLE);
    vlist->off += 1;
    double ret = *(double *)(vlist->buf + vlist->off);
    vlist->off += sizeof(double);
    return ret;
}

static inline void *get_pointer(binlog_va_list *vlist)
{
    if (vlist->off + 1 + sizeof(void *) > vlist->len) {
        assert(0);
    }
    assert(*(uint8_t *)(vlist->buf + vlist->off) == BLOG_POINTER);
    vlist->off += 1;
    void *ret = (void *)(*(int64_t *)(vlist->buf + vlist->off));
    vlist->off += sizeof(void *);
    return ret;
}

static inline char *get_string(binlog_va_list *vlist)
{
    int32_t len = strlen(vlist->buf + vlist->off + 1);
    if (vlist->off + 1 + len + 1 > vlist->len) {
        assert(0);
    }
    assert(*(uint8_t *)(vlist->buf + vlist->off) == BLOG_STRING);
    vlist->off += 1;
    char *ret = (char *)(vlist->buf + vlist->off);
    vlist->off += len + 1;
    return ret;
}

#define PARSE_FUNC_INT      get_int
#define PARSE_FUNC_LONG     get_long
#define PARSE_FUNC_DOUBLE   get_double
#define PARSE_FUNC_STRING   get_string
#define PARSE_FUNC_POINTER  get_pointer

#define binlog_va_arg(BTYPE) ({                                \
    TYPE_BLOG_##BTYPE __arg = PARSE_FUNC_##BTYPE(&(va_list));  \
    __arg;                                                     \
})

#define COPY_VA_INT                                      \
    do {                                                 \
        const int value = abs (binlog_va_arg (INT));     \
        char buf[32];                                    \
        ptr++; /* Go past the asterisk.  */              \
        *sptr = '\0'; /* NULL terminate sptr.  */        \
        sprintf(buf, "%d", value);                       \
        strcat(sptr, buf);                               \
        while (*sptr) sptr++;                            \
    } while (0)

#define PRINT_CHAR(CHAR)                                 \
    do {                                                 \
        putc(CHAR, stream);                              \
        ptr++;                                           \
        total_printed++;                                 \
    } while (0)

#define PRINT_TYPE(BTYPE)                                 \
    do {                                                  \
        int result;                                       \
        TYPE_BLOG_##BTYPE value = binlog_va_arg (BTYPE);  \
        *sptr++ = *ptr++; /* Copy the type specifier.  */ \
        *sptr = '\0'; /* NULL terminate sptr.  */         \
        result = fprintf(stream, specifier, value);       \
        if (result == -1)                                 \
            return -1;                                    \
        else {                                            \
            total_printed += result;                      \
            continue;                                     \
        }                                                 \
    } while (0)


int32_t print_format(char *format, binlog_va_list va_list, FILE *stream)
{
    const char *ptr = format;
    char specifier[128];
    int total_printed = 0;

    while (*ptr != '\0') {
        if (*ptr != '%') { /* While we have regular characters, print them.  */
            PRINT_CHAR(*ptr);
        } else { /* We got a format specifier! */
            char *sptr = specifier;
            int wide_width = 0, short_width = 0;

            *sptr++ = *ptr++; /* Copy the % and move forward.  */

            while (strchr("-+ #0", *ptr)) { /* Move past flags.  */
                *sptr++ = *ptr++;
            }

            if (*ptr == '*') {
                COPY_VA_INT;
            } else
                while (ISDIGIT(*ptr)) { /* Handle explicit numeric value.  */
                    *sptr++ = *ptr++;
                }

            if (*ptr == '.') {
                *sptr++ = *ptr++; /* Copy and go past the period.  */
                if (*ptr == '*') {
                    COPY_VA_INT;
                } else
                    while (ISDIGIT(*ptr)) { /* Handle explicit numeric value.  */
                        *sptr++ = *ptr++;
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
                *sptr++ = *ptr++;
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
                        PRINT_TYPE(INT);
                    } else {
                        switch (wide_width) {
                            case 0:
                                PRINT_TYPE(INT);
                                break;
                            case 1:
                                PRINT_TYPE(LONG);
                                break;
                            case 2:
                            default:

                                PRINT_TYPE(LONG); /* Fake it and hope for the best.  */
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
                        PRINT_TYPE(DOUBLE);
                    } else {

                        PRINT_TYPE(DOUBLE); /* Fake it and hope for the best.  */
                    }
                }
                break;
                case 's':
                    PRINT_TYPE(STRING);
                    break;
                case 'p':
                    PRINT_TYPE(POINTER);
                    break;
                case '%':
                    PRINT_CHAR('%');
                    break;
                default:
                    abort();
            } /* End of switch (*ptr) */
        } /* End of else statement */
    }

    return total_printed;
}

char *foramt_fetch(char *func, int32_t line)
{
    return "start test1 %s <%d %d %d %u %llu %f %lf>";
}

void binlog_prase(binlog_parser_t *parser)
{
    struct timespec monotonic_ts;
    clock_gettime(CLOCK_MONOTONIC_COARSE, &monotonic_ts);
    struct timespec realtime_ts;
    clock_gettime(CLOCK_REALTIME, &realtime_ts);

    time_t diff = realtime_ts.tv_sec * (time_t)(1e6) + realtime_ts.tv_nsec / (time_t)(1e3) -
                  monotonic_ts.tv_sec * (time_t)(1e6) + monotonic_ts.tv_nsec / (time_t)(1e3);
    while (1) {
        if (parser->offset == parser->read_size) {
            binlog_parse_read(parser);
        }
        void *buffer = parser->buf + parser->offset;

        /* begin */
        uint32_t offset = sizeof(uint8_t);

        /* length */
        int len = *(int32_t *)(buffer + offset);
        offset += sizeof(int32_t);
    
        if (len + parser->offset > parser->read_size) {
            binlog_parse_read(parser);
        }
        buffer = parser->buf + parser->offset;

        /* level */
        uint8_t level =  *(uint8_t *)(buffer + offset);
        offset += sizeof(uint8_t);
    
        /* function */
        char *func = (char *)(buffer + offset);
        offset += strlen(func) + 1;

        /* line */
        int32_t line = *(int32_t *)(buffer + offset);
        offset += sizeof(int32_t);

        /* thread */
        pthread_t thread = (pthread_t)(*(uint64_t *)(buffer + offset));
        offset += sizeof(uint64_t);

        /* timestamp */
        time_t ts = *(time_t *)(buffer + offset);
        offset += sizeof(time_t);

        binlog_va_list vlist = {
            .len = len,
            .buf = buffer,
            .off = offset
        };

        time_t real_us = ts + diff;
        time_t real_sec = real_us / (time_t)(1e6);
        struct tm utc_time;
        localtime_r(&real_sec, &utc_time);
        char time_fmt[128];
        strftime(time_fmt, sizeof(time_fmt), "%Y-%m-%d %H:%M:%S", &utc_time);
        snprintf(time_fmt + strlen(time_fmt), sizeof(time_fmt) - strlen(time_fmt), ".%03ld", real_us % 1000);

        /* prefix */
        fprintf(parser->out, "%s %s %#p ", time_fmt, binlog_level_name(level), thread);

        /* va args */
        print_format(foramt_fetch(func, line), vlist, parser->out);

        /* function + line */
        fprintf(parser->out, " [%s:%d]\n", func, line);

        parser->offset += len;
        if (parser->file_offset == parser->file_size && parser->offset == parser->read_size) {
            break;
        }
    }
    return;
}

int main(int argc, char **argv)
{
    char *name = NULL;
    char *output = NULL;
    char opt;
    while ((opt = getopt(argc, argv, "f:o:")) != -1) {
        switch (opt) {
            case 'f': {
                name = optarg;
            }
            case 'o': {
                output = optarg;
            }
        }
    }
    if (name == NULL || output == NULL) {
        exit(-1);
    }
    binlog_parser_t parser;

    binlog_parser_create(&parser, name, output);
    binlog_prase(&parser);
    binlog_parser_destroy(&parser);
}