#include "fault_inject.h"
#include "gtest/gtest.h"

int func(void)
{
    return -1;
}

TEST(TEST_FAULT, test_fault_inject)
{
    int ret = 0;
    fault_inject_register("TEST_FAULT/test_fault_inject");

    fault_inject_t new_fault = {
        .enable = true,
        // .sleep_time = 10,
        .after_times = 4,
        .enable_times = 5,
        .inject_err = -2,
        .hit_times = 0,
    };
    fault_inject_init("TEST_FAULT/test_fault_inject", new_fault);

    for (int i = 0; i < 10; i++) {
        FAULT_INJECT_BEGIN("TEST_FAULT/test_fault_inject", FAULT_STATEMENT_MOD_ERR(ret), 0)
        ret = func();
        FAULT_INJECT_END("TEST_FAULT/test_fault_inject")
        printf("func ret = %d\n", ret);
    }
    
}
