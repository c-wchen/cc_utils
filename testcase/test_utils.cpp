#include "gtest/gtest.h"
#include "w_utils.h"
#include <limits.h>

TEST(TEST_UTILS, test_str_to_int) {
    EXPECT_NE(str_to_int("1234", 4), INT_MIN);
    EXPECT_EQ(str_to_int("test", 4), INT_MIN);
    EXPECT_EQ(str_to_int("1111111111111111", 16), INT_MIN);
}