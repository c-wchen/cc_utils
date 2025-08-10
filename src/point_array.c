#include <stdint.h>
#include <stdlib.h>
#include "point_array.h"

#define MAX_LEN_LIMIT 8000

void *create_point_array(int size)
{
    uint32_t max_col_num = MAX_LEN_LIMIT / sizeof(uint64_t *);
    uint32_t row_num = (size + max_col_num - 1) / max_col_num;
    uint64_t **row_ptr = (uint64_t **) malloc(row_num * sizeof(uint64_t **));
    for (int32_t i = 0; i < row_num; i++) {
        uint32_t colNum = (i == row_num - 1) ? (size - max_col_num * i) : max_col_num;
        row_ptr[i] = (uint64_t *) malloc(colNum * sizeof(uint64_t *));
    }
    return (void *) row_ptr;
}

void free_point_array(void *ptr, int size)
{
    uint32_t max_col_num = MAX_LEN_LIMIT / sizeof(uint64_t *);
    uint32_t row_num = (size + max_col_num - 1) / max_col_num;
    for (int32_t i = 0; i < row_num; i++) {
        free(((uint64_t **)ptr)[i]);
    }
    free(ptr);
}

void set_point_value(void *row_ptr, uint32_t index, void *value)
{
    uint32_t max_col_num = MAX_LEN_LIMIT / sizeof(uint64_t *);
    ((uint64_t **) row_ptr)[index / max_col_num][index % max_col_num] = (uint64_t) value;
}

void *get_point_value(void *row_ptr, uint32_t index)
{
    uint32_t max_col_num = MAX_LEN_LIMIT / sizeof(uint64_t *);
    return (void *)((uint64_t **) row_ptr)[index / max_col_num][index % max_col_num];
}
