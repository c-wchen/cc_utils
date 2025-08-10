
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

#include "cJSON.h"
#include "binlog/binlog.h"
#include "binlog_internal.h"

typedef struct {
    int32_t fd;
    FILE *out;
    void *buf;
    cJSON *json_arr;
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

int32_t binlog_read_json(binlog_parser_t *parser, const char *json)
{
    int rc = 0;
    int32_t fd = open(json, O_RDONLY, 0644);
    if (fd < 0) {
        fprintf(stderr, "open file faild(%d).", -errno);
        return -errno;
    }

    int32_t size = binlog_file_size(json);
    if (size <= 0) {
        fprintf(stderr, "open fstat faild.");
        return -1;
    }

    char *json_str = (char *)calloc(1, size + 1);

    int rd_size = read(fd, json_str, size);

    if (rd_size < 0) {
        fprintf(stderr, "read file faild(%d).", -errno);
        rc = -errno;
    } else {
        parser->json_arr = cJSON_Parse(json_str);
    }

    free(json_str);
    json_str = NULL;
    return 0;
}

int binlog_parser_create(binlog_parser_t *parser, const char *json, const char *name, const char *output)
{
    char path_name[128];
    int32_t off = snprintf(path_name, sizeof(path_name), "/var/log/tmp/%s", name);

    if (off < 0 || off >= sizeof(path_name)) {
        fprintf(stderr, "name too long(%d).", -ENAMETOOLONG);
        return -ENAMETOOLONG;
    }

    parser->fd = open(path_name, O_RDONLY, 0644);

    if (parser->fd < 0) {
        fprintf(stderr, "open file %s faild(%d).", path_name, -errno);
        return -errno;
    }

    char output_path_name[128];
    off = snprintf(output_path_name, sizeof(output_path_name), "/var/log/tmp/%s", output);
    parser->out = fopen(output_path_name, "a");

    if (parser->out == NULL) {
        close(parser->fd);
        fprintf(stderr, "open  output file %s faild (%d).", output_path_name, -errno);
        return -errno;
    }

    int rc = binlog_read_json(parser, json);
    if (rc != 0) {
        return rc;
    }

    parser->buf = malloc(1 << 22);
    parser->offset = 0;
    parser->size = 1 << 22;

    parser->file_size = binlog_file_size(path_name);
    if (parser->file_size < 0) {
        fclose(parser->out);
        close(parser->fd);
        cJSON_Delete(parser->json_arr);
        fprintf(stderr, "file size get faild (%d).", path_name, parser->file_size);
        return -errno;
    }
    parser->file_offset = 0;

    binlog_parse_read(parser);

    return 0;
}

void binlog_parser_destroy(binlog_parser_t *bparser)
{
    close(bparser->fd);
    if (bparser->buf != NULL) {
        free(bparser->buf);
    }
    cJSON_Delete(bparser->json_arr);
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

char *format_fetch(binlog_parser_t *parser, char *func, int32_t line)
{
    cJSON *json_arr = parser->json_arr;
    cJSON *item = NULL;
    cJSON_ArrayForEach(item, json_arr) {
        if (strcmp(func, cJSON_GetObjectItem(item, "function")->valuestring) == 0 &&
            line ==  cJSON_GetObjectItem(item, "line")->valueint) {
            return cJSON_GetObjectItem(item, "format")->valuestring;
        }

    }
    return NULL;
}

static void timestamp_fmt(time_t ts, char *time_fmt, uint32_t size)
{
    time_t sec = ts / (time_t)(1e6);
    time_t usec = ts % 1000;
    struct tm utc_time;
    localtime_r(&sec, &utc_time);
    strftime(time_fmt, size, "%Y-%m-%d %H:%M:%S", &utc_time);
    snprintf(time_fmt + strlen(time_fmt), size - strlen(time_fmt), ".%03ld", usec);
    return;
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

        /* time format */
        time_t real_us = ts + diff;
        char time_fmt[128];
        timestamp_fmt(real_us, time_fmt, 128);

        /* prefix */
        fprintf(parser->out, "%s %s %#p ", time_fmt, binlog_level_name(level), thread);

        /* va args */
        char *format = format_fetch(parser, func, line);
        if (format == NULL) {
            fprintf(stderr, "skip: [%s:%d] format unfound", func, line);
        }
        print_format(format, vlist, parser->out);

        /* function + line */
        fprintf(parser->out, " [%s:%d]\n", func, line);

        parser->offset += len;
        if (parser->file_offset == parser->file_size && parser->offset == parser->read_size) {
            break;
        }
    }
    return;
}

static void usage(void)
{
    fprintf(stdout, "Usage: binlog_parser -f [BINLOG_FILE] -o [OUTPUT] -j [FORMAT_JSON]\n"
            "\t -f enter a binary file name\n"
            "\t -o output the filename after parsing\n"
            "\t -j formatting json file\n\n");
    return;
}

int main(int argc, char **argv)
{
    char *name = NULL;
    char *output = NULL;
    char *json = NULL;
    char opt;
    while ((opt = getopt(argc, argv, "f:o:j:h")) != -1) {
        switch (opt) {
            case 'f': {
                name = optarg;
                break;
            }
            case 'o': {
                output = optarg;
                break;
            }
            case 'j': {
                json = optarg;
                break;
            }
            case 'h':
            default: {
                usage();
                exit(-1);
                break;
            }
        }
    }
    if (name == NULL || output == NULL || json == NULL) {
        usage();
        exit(-1);
    }
    binlog_parser_t parser = {0};

    int rc = binlog_parser_create(&parser, json, name, output);
    if (rc != 0) {
        return rc;
    }
    binlog_prase(&parser);
    binlog_parser_destroy(&parser);
}