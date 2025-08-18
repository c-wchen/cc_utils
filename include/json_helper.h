#ifndef CJSON_HELPER_H
#define CJSON_HELPER_H

#include <stdint.h>
#include <stdbool.h>

#include "cJSON.h"
#ifdef __cplusplus
extern "C" {
#endif

#define CH_TYPE_INT     1
#define CH_TYPE_FLOAT   2
/* #define CH_TYPE_DOUBLE */
#define CH_TYPE_STRING  3
/* #define CH_TYPE_ARRAY */

typedef struct {
    const char *name;
    uint8_t type;
    uint32_t offset;
    uint32_t length;
    bool must;
} json2struct_t;

// #define offsetof(type, member) ((size_t)&(((type *)0)->member))
#define clength(t, m) ((size_t)sizeof((t *)0)->m)
#define array_size(arr) ((size_t)(sizeof(arr) / sizeof(arr[0])))

#define CH_INT(k, t, m, r)                       \
    {                                            \
        .name = (k),                             \
        .type = CH_TYPE_INT,                     \
        .offset = offsetof(t, m),                \
        .length = clength(t, m),                 \
        .must = (r),                             \
    }

#define CH_STRING(k, t, m, r, l)                 \
    {                                            \
        .name = (k),                             \
        .type = CH_TYPE_STRING,                  \
        .offset = offsetof(t, m),                \
        .length = l,                             \
        .must = (r),                             \
    }

#define CH_FLOAT(k, t, m, r)                     \
    {                                            \
        .name = (k),                             \
        .type = CH_TYPE_FLOAT,                   \
        .offset = offsetof(t, m),                \
        .length = clength(t, m),                 \
        .must = (r),                             \
    }

int json_object_helper(void *ptr, cJSON *obj, json2struct_t *jstruct_arr, uint32_t jstruct_size);

#ifdef __cplusplus
}
#endif
#endif