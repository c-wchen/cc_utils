#include <stdio.h>
#include <stdbool.h>
#include <unistd.h>
#include <assert.h>

#include "binlog/binlog.h"

static void binlog_test1(void *log)
{

    for (int i = 0; i < 1024 * 1024; i++) {
        binlog_print(log, LOG_DEBUG, __func__, __LINE__, "start test1 %s <%d %d %d %u %llu %f %lf>", "name", i, i + 1, i + 2,
                     i + 3, i + 4, i * 1.1F, i * 1.212);
    }
    return;
}

int main()
{
    void *log = binlog_create("20250525.log");
    assert(log != NULL);

    binlog_test1(log);

    binlog_destroy(log);
    return 0;
}