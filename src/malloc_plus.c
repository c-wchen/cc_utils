//
// Created by 忘尘 on 2022/7/23.
//
#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include "malloc_plus.h"

void *malloc_plus(int size, const char *func, uint32_t lineNum) {
    if (size <= 0) {
        return NULL;
    }
    void *ptr = malloc(size);
    memset(ptr, 0, size);
    printf("DBG[%s:%u] malloc size (%d)\n", func, lineNum, size);
    return ptr;
}

void free_plus(void **ptr, const char *func, uint32_t lineNum) {
    if (*ptr != NULL) {
        free(*ptr);
    }
    printf("DBG[%s:%u] free successful\n", func, lineNum);
    *ptr = NULL;
}
