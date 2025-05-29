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

typedef int32_t TYPE_BLOG_INT;
typedef long    TYPE_BLOG_LONG;
typedef double  TYPE_BLOG_DOUBLE;
typedef char   *TYPE_BLOG_STRING;
typedef void   *TYPE_BLOG_POINTER;

typedef struct {
    void *buf;
    int32_t buf_size;
    int32_t begoff;
    int32_t endoff;
    int32_t curoff;
} tsd_binlog_t;


#define BINLOG_MEM_DEFAULT_SIZE  (1UL << 22)

typedef struct {
    uint8_t mem[BINLOG_MEM_DEFAULT_SIZE];
    size_t alloc;
    size_t free;
    spinlock spin;
} binlog_mem_t;


typedef struct {
    int32_t fd;
    char name[128];
    binlog_mem_t bmem;
} binlog_handle_t;


static inline void* binlog_mem_reserve(binlog_mem_t *bmem, size_t size)
{
    spinlock_acquire(&bmem->spin);
    while (bmem->alloc + size - bmem->free > BINLOG_MEM_DEFAULT_SIZE) {
        spinlock_release(&bmem->spin);
        usleep(1);
        spinlock_acquire(&bmem->spin);
    }
    uint8_t *ptr = bmem->mem + ((bmem->alloc) & (BINLOG_MEM_DEFAULT_SIZE - 1));
    bmem->alloc += size;
    spinlock_release(&bmem->spin);

    *ptr = 0x00;

    return ptr;
}

static inline void* binlog_mem_put(binlog_mem_t *bmem, size_t size, void *buf)
{
    spinlock_acquire(&bmem->spin);
    while (bmem->alloc + size - bmem->free > BINLOG_MEM_DEFAULT_SIZE) {
        spinlock_release(&bmem->spin);
        usleep(1);
        spinlock_acquire(&bmem->spin);
    }
    uint8_t *ptr = bmem->mem + ((bmem->alloc) & (BINLOG_MEM_DEFAULT_SIZE - 1));
    bmem->alloc += size;
    spinlock_release(&bmem->spin);

    memcpy(ptr, buf, size);

    return ptr;
}


#define ISDIGIT(x) ({__typeof(x) __is_x = (x); __is_x >= '0' && __is_x <= '9'; })

#ifdef __cplusplus /* BINLOG_INTERNAL_H */
}
#endif
#endif