//
// Created by 忘尘 on 2022/7/31.
//
#include "gtest/gtest.h"
#include "mockcpp/mockcpp.hpp"

int add(int a, int b) {
    printf("func add");
    return a + b;
}
int add_stub(int a, int b) {
    printf("func addStub");
    return a + b;
}
TEST(TEST_MOCKCPP, test_add_stub) {
    MOCKER(add).stubs().will(invoke(add_stub));
    add(3, 2);
    MOCKER(add).stubs();
}