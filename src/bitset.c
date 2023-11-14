//
// Created by 忘尘 on 2022/10/12.
//
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>


#define BIT_SUCCESS 1
#define BIT_FAIL -1

#define DIVIDE_UP(val, x) ((val + x - 1) / x)

typedef struct {
    uint8_t *data;
    int32_t len;
} Bits;

Bits *BitAlloc(int32_t len)
{
    Bits *bits = (Bits *) malloc(sizeof(Bits));
    if (bits == NULL) {
        return NULL;
    }
    bits->data = (uint8_t *) malloc(DIVIDE_UP(len, 8));
    if (bits->data == NULL) {
        free(bits);
        return NULL;
    }
    bits->len = len;
    memset(bits->data, 0, DIVIDE_UP(len, 8));
    return bits;
}

void BitFree(Bits *bits)
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
int32_t BitSet(Bits *bits, int32_t offset, uint8_t value)
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

int32_t BitGet(Bits *bits, int32_t offset)
{
    if (offset >= bits->len) {
        return BIT_FAIL;
    }
    return (uint32_t)((bits->data[offset / 8] & (0x01 << (offset % 8))) > 0) ? 1 : 0;
}

int32_t BitPrint(Bits *bits)
{
    int32_t i;
#define MAX_SEP_LEN 16
    for (i = 0; i < bits->len; i++) {
        printf("%u", BitGet(bits, i));
        if (i % MAX_SEP_LEN) {
            printf("\n");
        }
    }
    printf("\n");
}

//int main(void) {
//    Bits *bits = BitAlloc(200);
//    BitSet(bits, 14, 1);
//    BitSet(bits, 1, 1);
//    BitSet(bits, 1, 0);
//    BitSet(bits, 2, 1);
//    BitPrint(bits);
//    BitFree(bits);
//    bits = NULL;
//    return 0;
//}