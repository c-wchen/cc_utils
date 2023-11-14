#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <limits.h>
#include "num_convert.h"

int str_to_int(const char *buf, int length)
{
    if (strlen(buf) != length) {
        printf("str length is mismatch (%ld, %d)\n", strlen(buf), length);
        return INT_MIN;
    }
    for (int i = 0; i < length; i++) {
        if (buf[i] < '0' || buf[i] > '9') {
            printf("valid str buf[%d] = %c\n", i, buf[i]);
            return INT_MIN;
        }
    }
    // pre check
    int res = strtol(buf, NULL, 10);
    int checkVal = res;
    int remainder;
    for (int i = length - 1; i >= 0; i--) {
        remainder = checkVal % 10;
        checkVal /= 10;
        if ('0' + remainder != buf[i]) {
            printf("value overflow\n");
            return INT_MIN;
        }
    }
    return res;
}

/**
 * @param val          待转换的float类型值
 * @param preserve     保留位数
 * @param output       输出缓冲区
 * @param outputSize   输出的最大值
 * @return
 */
int double_to_str(double val, int preserve, char *output, int outputSize)
{
    if (preserve > 7 || preserve <= 0) {
        printf("preserve is err num\n");
    }
    long long_val = 0;
    long preserve_val = 1;
    for (int i = 0; i < preserve; ++i) {
        preserve_val *= 10;
    }
    long_val = (long) (val * preserve_val);
    if (val * preserve_val - long_val > 0.5) {
        long_val++;
    }
    int i = 0;
    long tmp_val = long_val;
    while (tmp_val > 0) {
        if (i == preserve) {
            output[i++] = '.';
        }
        output[i++] = tmp_val % 10 + '0';
        tmp_val /= 10;
    }
    if (long_val < preserve_val) {
        output[i++] = '.';
        output[i++] = '0';
    }
    int len = strlen(output);
    for (int i = 0; i < len / 2; ++i) {
        char ch = output[i];
        output[i] = output[len - 1 - i];
        output[len - 1 - i] = ch;
    }
}