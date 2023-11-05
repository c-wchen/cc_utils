//
// Created by 忘尘 on 2022/7/17.
//
#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>
#include <stdarg.h>
#include "check_param.h"

#define MAX_BUF_SIZE 256

bool CheckParamFromAddr(const char *func, uint32_t lineNum, int32_t num, ...) {
    va_list valist;
    va_start(valist, num);
    char buf[MAX_BUF_SIZE] = {'\0'};
    int32_t offset = 0;
    offset += snprintf(buf + offset, MAX_BUF_SIZE - offset, "Check Param[%s:%d] ", func, lineNum);
    for (int i = 0; i < num; ++i) {
        uint64_t addr = va_arg(valist, uint64_t);
        if (addr == 0) {
            if (i == 0) {
                offset += snprintf(buf + offset, MAX_BUF_SIZE - offset, "param%d is INVALID", i);
            } else {
                offset += snprintf(buf + offset, MAX_BUF_SIZE - offset, ", param%d is INVALID", i);
            }
        }
    }
    buf[MAX_BUF_SIZE - 1] = '\0';
    printf("%s\n", buf);
    va_end(valist);
    return true;
}