//
// Created by 忘尘 on 2022/7/24.
//
#include "glib.h"
#include "gtest/gtest.h"

using namespace std;

class MyTestSuit : public testing::Test
{
protected:
    static void TearDownTestSuite()
    {
        cout << "test suit tear down" << endl;
    }

    static void SetUpTestSuite()
    {
        cout << "test suit set up" << endl;
    }

public:
    virtual void SetUp()
    {
        cout << "single testcase set up" << endl;
    }

    virtual void TearDown()
    {
        cout << "single testcase tear down" << endl;
    }
};

TEST_F(MyTestSuit, test_my_test_suit)
{
    cout << "test impl content" << endl;
}

TEST_F(MyTestSuit, test_gtest_expect_mcro)
{
    int a = 12;
    int b = 15;
    EXPECT_EQ(a, 12);
    EXPECT_NE(a, b);
    EXPECT_GT(b, a);
    EXPECT_LT(a, 20);
    EXPECT_TRUE(a == 12);
    EXPECT_FALSE(a == b);
    EXPECT_STREQ("a", "a");
    EXPECT_STRNE("a", "b");
    EXPECT_STRCASEEQ("A", "a");
    EXPECT_STRCASENE("A", "B");
    EXPECT_FLOAT_EQ(1.2, 1.2);
    EXPECT_DOUBLE_EQ(1.2, 1.2);
}

TEST_F(MyTestSuit, test_gtest_assert_mcro)
{
    int a = 12;
    int b = 15;
    ASSERT_EQ(a, 12);
    ASSERT_NE(a, b);
    ASSERT_GT(b, a);
    ASSERT_LT(a, 20);
    ASSERT_TRUE(a == 12);
    ASSERT_FALSE(a == b);
    ASSERT_STREQ("a", "a");
    ASSERT_STRNE("a", "b");
    ASSERT_STRCASEEQ("A", "a");
    ASSERT_STRCASENE("A", "B");
}

class MyParamTestSuit : public ::testing::TestWithParam<int>
{

};

TEST_P(MyParamTestSuit, test_param_input1)
{
    cout << "test param input 1: " << GetParam() << endl;
}

TEST_P(MyParamTestSuit, test_param_input2)
{
    cout << "test param input 2: " << GetParam() << endl;
}

INSTANTIATE_TEST_CASE_P(TrueReturn, MyParamTestSuit, testing::Range(3, 30));

