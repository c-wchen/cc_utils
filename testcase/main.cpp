#include <iostream>
#include "gtest/gtest.h"
using namespace std;

TEST(TestCase, test2) {
    EXPECT_EQ(3, 2);
}

int main(int argc, char** argv) {
    testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
