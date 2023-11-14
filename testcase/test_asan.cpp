/*
 * gcc asan-test.c -o asan-test -fsanitize=address -g
 */
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <strings.h>
#include "gtest/gtest.h"

/* #include <sanitizer/lsan_interface.h> */

int ga[10] = {1};

int global_buffer_overflow()
{
    return ga[10];
}

void heap_leak()
{
    int *k = (int *) malloc(10 * sizeof(int));
    return;
}

int heap_use_after_free()
{
    int *u = (int *) malloc(10 * sizeof(int));
    u[9] = 10;
    free(u);
    return u[9];
}

int heap_buffer_overflow()
{
    int *h = (int *) malloc(10 * sizeof(int));
    h[0] = 10;
    return h[10];
}

int stack_buffer_overflow()
{
    int s[10];
    s[0] = 10;
    return s[10];
}

int *gp;

void stack_use_after_return()
{
    int r[10];
    r[0] = 10;
    gp = &r[0];
    return;
}

void stack_use_after_scope()
{
    {
        int c = 0;
        gp = &c;
    }
    *gp = 10;
    return;
}

int test(int argc, char *argv[])
{
    int opt = 0;
    while ((opt = getopt(argc, argv, "hbfloprs")) != -1) {
        switch (opt) {
        case 'h':
            printf("Test AddressSanitier\n"
                   "usage: asan-test [-bfloprs]\n\n"
                   "-b   heap buffer overflow\n"
                   "-f   heap use after free\n"
                   "-l   heap memory leak\n"
                   "-o   global buffer overflow\n"
                   "-p   stack use after scope\n"
                   "-r   stack use after return\n"
                   "-s   stack buffer overflow\n");
            break;
        case 'b':
            stack_buffer_overflow();
            break;
        case 'f':
            heap_use_after_free();
            break;
        case 'l':
            heap_leak();
            break;
        case 'o':
            global_buffer_overflow();
            break;
        case 'p':
            stack_use_after_scope();
            break;
        case 'r':
            stack_use_after_return();
            break;
        case 's':
            stack_buffer_overflow();
            break;
        default:
            break;
        }
    }
    return 0;
}

// asan场景验证，注释防止影响程序运行。
//TEST(TEST_ASAN, test_asan) {
//    int argc = 2;
//    int len = 30;
//    char *argv[10];
//    for (int i = 0; i < argc; ++i) {
//        argv[i] = (char *)malloc(len);
//        memset(argv[i], 0, len);
//    }
//    strcpy(argv[0], "asan-test");
//    strcpy(argv[1], "-b");
//    test(argc, argv);
//    for (int i = 0; i < argc; ++i) {
//        free(argv[i]);
//        argv[i] = NULL;
//    }
//}