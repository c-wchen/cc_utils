#include <stdint.h>
#include <stdlib.h>
#include "point_array.h"

#define MAX_LEN_LIMIT 8000

void *create_point_array(int size) {
    uint32_t maxColNum = MAX_LEN_LIMIT / sizeof(uint64_t *);
    uint32_t rowNum = (size + maxColNum - 1) / maxColNum;
    uint64_t **rowPtr = (uint64_t **) malloc(rowNum * sizeof(uint64_t **));
    for (int32_t i = 0; i < rowNum; i++) {
        uint32_t colNum = (i == rowNum - 1) ? (size - maxColNum * i) : maxColNum;
        rowPtr[i] = (uint64_t *) malloc(colNum * sizeof(uint64_t *));
    }
    return (void *) rowPtr;
}

void free_point_array(void *ptr, int size) {
    uint32_t maxColNum = MAX_LEN_LIMIT / sizeof(uint64_t *);
    uint32_t rowNum = (size + maxColNum - 1) / maxColNum;
    for (int32_t i = 0; i < rowNum; i++) {
        uint32_t colNum = (i == rowNum - 1) ? (size - maxColNum * i) : maxColNum;
        free(((uint64_t **)ptr) + i);
    }
    free(ptr);
}

void set_point_value(void *rowPtr, uint32_t index, void *value) {
    uint32_t maxColNum = MAX_LEN_LIMIT / sizeof(uint64_t *);
    ((uint64_t **) rowPtr)[index / maxColNum][index % maxColNum] = (uint64_t) value;
}

void *get_point_value(void *rowPtr, uint32_t index) {
    uint32_t maxColNum = MAX_LEN_LIMIT / sizeof(uint64_t *);
    return (void *) ((uint64_t **) rowPtr)[index / maxColNum][index % maxColNum];
}
