//
// Created by 忘尘 on 2022/10/12.
//
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "bitmap.h"

#define ROUND_UP(val, x) ((val + x - 1) / x * x)

bitmap *bitmap_alloc(int32_t len)
{
    bitmap *bits = (bitmap *) malloc(sizeof(bitmap));
    if (bits == NULL) {
        return NULL;
    }
    bits->data = (uint8_t *) malloc(ROUND_UP(len, 8) / 8);
    if (bits->data == NULL) {
        free(bits);
        return NULL;
    }
    bits->len = len;
    memset(bits->data, 0, ROUND_UP(len, 8) / 8);
    return bits;
}

void bitmap_free(bitmap *bits)
{
    if (bits != NULL) {
        if (bits->data) {
            free(bits->data);
            bits->data = NULL;
        }
        free(bits);
    }
}

/**
 * @param bits
 * @param offset : 0 ~ len-1
 * @param value : 0 | 1
 * @return
 */
int32_t bitmap_set(bitmap *bits, int32_t offset, uint8_t value)
{
    if (value != 0 && value != 1) {
        return BIT_FAIL;
    }
    if (offset >= bits->len) {
        return BIT_FAIL;
    }
    if (value == 0) {
        bits->data[offset / 8] &= ~(0x01 << (offset % 8));
    } else {
        bits->data[offset / 8] |= (0x01 << (offset % 8));
    }
    return BIT_SUCCESS;
}

int32_t bitmap_get(bitmap *bits, int32_t offset)
{
    if (offset >= bits->len) {
        return BIT_FAIL;
    }
    return (uint32_t)((bits->data[offset / 8] & (0x01 << (offset % 8))) > 0) ? 1 : 0;
}

int32_t bitmap_print(bitmap *bits)
{
    int32_t i;
#define STR_MAX 64
    char str[STR_MAX + 1] = {0};
    for (i = 0; i < bits->len; i++) {
        if (i > 0 && i % STR_MAX == 0) {
            printf("%s\n", str);
        } else if (i + 1 == bits->len) {
            str[(i + 1) % STR_MAX] = '\0';
            printf("%s\n", str);
        }
        str[i % STR_MAX] = bitmap_get(bits, i) ? '1' : '0';
    }
    printf("\n");
}