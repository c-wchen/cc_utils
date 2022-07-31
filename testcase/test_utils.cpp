#include "gtest/gtest.h"
#include "num_convert.h"
#include "malloc_plus.h"
#include <limits.h>

TEST(TEST_UTILS, test_str_to_int) {
    EXPECT_NE(str_to_int("1234", 4), INT_MIN);
    EXPECT_EQ(str_to_int("test", 4), INT_MIN);
    EXPECT_EQ(str_to_int("1111111111111111", 16), INT_MIN);
}

TEST(TEST_UTILS, test_float_to_str) {
    char output[100] = {'\0'};
    float_to_str(1234.12121, 2, output, 100);
    EXPECT_STREQ(output, "1234.12");
    memset(output, 0, 100);
    float_to_str(1234.12121, 3, output, 100);
    EXPECT_STREQ(output, "1234.121");
    memset(output, 0, 100);
    float_to_str(1234.567, 2, output, 100);
    EXPECT_STREQ(output, "1234.57");
    memset(output, 0, 100);
    float_to_str(0.479, 2, output, 100);
    EXPECT_STREQ(output, "0.48");
}

TEST(TEST_UTILS, test_malloc_plus) {
    int *a = (int *)MALLOC_PLUS(10 * sizeof(int));
    a[0] = 11;
    for (int i = 0; i < 10; ++i) {
        printf("index %d value %d\n", i, *(a + i));
    }
    FREE_PLUS(a);
}