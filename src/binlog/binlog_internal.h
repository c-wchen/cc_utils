#ifndef BINLOG_INTERNAL_H
#define BINLOG_INTERNAL_H

#include <stdint.h>
#include <stdatomic.h>
#include <unistd.h>
#include "spin.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef enum {
    BLOG_INT,
    BLOG_LONG,
    BLOG_DOUBLE,
    BLOG_STRING,
    BLOG_POINTER,
    BLOG_BEGIN = 0xfd,
    BLOG_END = 0xff,
} binlog_enum;

typedef int     TYPE_BLOG_INT;
typedef long    TYPE_BLOG_LONG;
typedef double  TYPE_BLOG_DOUBLE;
typedef char   *TYPE_BLOG_STRING;
typedef void   *TYPE_BLOG_POINTER;

typedef struct {
    void *buf;
    int32_t size;
    int32_t offset;
} tsd_binlog_t;

#define BINLOG_STACK_SIZE  (1024)

typedef struct {
    int32_t fd;
    char name[128];
} binlog_handle_t;

#define ISDIGIT(x) ({__typeof(x) __is_x = (x); __is_x >= '0' && __is_x <= '9'; })

#ifdef __cplusplus /* BINLOG_INTERNAL_H */
}
#endif
#endif