#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <limits.h>
#include "w_utils.h"

int str_to_int(const char *buf, int length) {
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