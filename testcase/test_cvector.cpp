#include "gtest/gtest.h"
extern "C" {
    #include "cvector.h"
}

TEST(TEST_CVECTOR, test_cvector)
{
    cvector(int) vec = NULL;

    for (int i = 0; i < 100; i++) {
        cvector_push_back(vec, i);
    }

    EXPECT_EQ(cvector_size(vec), 100);
    EXPECT_EQ(*cvector_at(vec, 50), 50);
    EXPECT_EQ(*cvector_at(vec, 99), 99);
    
    cvector_free(vec);

}
