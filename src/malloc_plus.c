//
// Created by 忘尘 on 2022/7/23.
//
#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>

void *malloc_plus(int size, const char *func, uint32_t lineNum) {
    if (size <= 0) {
        return NULL;
    }
    void *ptr = malloc(size);
    memset(ptr, 0, size);
    printf("DBG[%s:%u] malloc size (%d)\n", func, lineNum, size);
    return ptr;
}

#define MALLOC_PLUS(size) malloc_plus(size, __FUNCTION__, __LINE__)

void free_plus(void **ptr) {
    if (*ptr != NULL) {
        free(*ptr);
    }
    *ptr = NULL;
}

#define FREE_PLUS(ptr) free_plus(&(ptr))
