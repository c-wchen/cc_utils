#include <stdio.h>
#include <stdbool.h>
#include <unistd.h>
#include <assert.h>
#include <pthread.h>

#include "binlog/binlog.h"

static void *binlog_test(void *log)
{

    for (int i = 0; i < 1024; i++) {
        binlog_print(log, LOG_DEBUG, __func__, __LINE__, "start test %s <%d %d %d %u %llu %f %lf>", "n_debug", i, i + 1, i + 2,
                     i + 3, i + 4, i * 1.1F, i * 1.212);

        binlog_print(log, LOG_INFO, __func__, __LINE__, "start test %s <%d %d %d %u %llu %f %lf>", "n_info", i, i + 1, i + 2,
                     i + 3, i + 4, i * 1.1F, i * 1.212);

        binlog_print(log, LOG_ERROR, __func__, __LINE__, "start test %s <%d %d %d %u %llu %f %lf>", "n_error", i, i + 1, i + 2,
                     i + 3, i + 4, i * 1.1F, i * 1.212);

        binlog_print(log, LOG_WARN, __func__, __LINE__, "start test %s <%d %d %d %u %llu %f %lf>", "n_warn", i, i + 1, i + 2,
                     i + 3, i + 4, i * 1.1F, i * 1.212);

        binlog_print(log, LOG_TRACE, __func__, __LINE__, "start test %s <%d %d %d %u %llu %f %lf>", "n_trace", i, i + 1, i + 2,
                     i + 3, i + 4, i * 1.1F, i * 1.212);
    }
    return NULL;
}

static void *binlog_test2(void *log)
{
    char str[4096];
    str[4095] = '\0';
    for (int i = 0; i < 4095; i++) {
        str[i] = 'a' + i % 26;
    }
    for (int i = 0; i < 1024; i++) {
        binlog_print(log, LOG_DEBUG, __func__, __LINE__, "start test2 %s <%d %lf>", str, i, i + 1, i * 1.212);
    }
    return NULL;
}

#define THREAD_NUM 8

#define BINLOG_EXEC binlog_test

int main()
{
    void *log = binlog_create("20250525.log");
    assert(log != NULL);
    pthread_t threads[THREAD_NUM];
    for (uint32_t i = 0; i < THREAD_NUM - 1; i++) {
        pthread_create(&threads[i], NULL, BINLOG_EXEC, log);
    }

    BINLOG_EXEC(log);

    for (uint32_t i = 0; i < THREAD_NUM - 1; i++) {
        pthread_join(threads[i], NULL);
    }

    binlog_destroy(log);
    return 0;
}