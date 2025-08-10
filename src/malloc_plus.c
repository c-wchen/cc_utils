//
// Created by 忘尘 on 2022/7/23.
//
#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include "malloc_plus.h"

void *malloc_plus(int size, const char *func, uint32_t line_num)
{
    if (size <= 0) {
        return NULL;
    }
    void *ptr = malloc(size);
    memset(ptr, 0, size);
    printf("DBG[%s:%u] malloc size (%d)\n", func, line_num, size);
    return ptr;
}

void free_plus(void **ptr, const char *func, uint32_t line_num)
{
    if (*ptr != NULL) {
        free(*ptr);
    }
    printf("DBG[%s:%u] free successful\n", func, line_num);
    *ptr = NULL;
}
