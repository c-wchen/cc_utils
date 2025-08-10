#include "bitmap.h"
#include "gtest/gtest.h"

TEST(TEST_BITMAP, test_bitmap)
{
    bitmap *bits = bitmap_alloc(200);

    int rc = bitmap_set(bits, 14, 1);
    EXPECT_EQ(rc, BIT_SUCCESS);

    rc = bitmap_set(bits, 1, 1);
    EXPECT_EQ(rc, BIT_SUCCESS);

    rc = bitmap_set(bits, 1, 0);
    EXPECT_EQ(rc, BIT_SUCCESS);

    rc = bitmap_set(bits, 2, 1);
    EXPECT_EQ(rc, BIT_SUCCESS);

    EXPECT_EQ(bitmap_set(bits, 2000, 1), BIT_FAIL);
    EXPECT_EQ(bitmap_get(bits, 14), 1);
    EXPECT_EQ(bitmap_get(bits, 0), 0);
    EXPECT_EQ(bitmap_get(bits, 2), 1);
    EXPECT_EQ(bitmap_get(bits, 199), 0);

    bitmap_print(bits);
    bitmap_free(bits);
}