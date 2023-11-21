#include "mock.h"
#include "gtest/gtest.h"
#include <stdint.h>

static void test_mock_fn(void)
{
    printf("%s\n", __func__);
}

static void test_fn(void)
{
    printf("%s\n", __func__);
}

void test_void_fn()
{

}

TEST(TEST_STUB, test_custom_mock)
{
    mock_s *m;
    m = mock_init(MOCK_LEN);

    int ret = mock_set(m, (void *)test_fn, (void *)test_mock_fn);
    EXPECT_EQ(ret, 0);
    test_fn();
    mock_reset(m);
    test_fn();
    mock_finalize(m);
}
