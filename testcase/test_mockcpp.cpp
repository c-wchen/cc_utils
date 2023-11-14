//
// Created by 忘尘 on 2022/7/31.
//
#include "gtest/gtest.h"
#include "mockcpp/mockcpp.hpp"

int add(int a, int b)
{
    printf("func add\n");
    return a + b;
}

int add_stub(int a, int b)
{
    printf("func addStub\n");
    return a + b;
}

TEST(TEST_MOCKCPP, test_add_stub)
{
    MOCKER(add).stubs().will(invoke(add_stub));
    add(3, 2);
    MOCKER(add).stubs();
}

/**
MOCKER(function) / MOCK_METHOD(mocker, method)
    .stubs() / defaults() / expects(never() | once() | exactly(3) | atLeast(3) | atMost(3) )
    [.before("some-mocker-id")]
    [.with( any() | eq(3) | neq(3) | gt(3) | lt(3) | spy(var_out) | check(check_func)
    | outBound(var_out) | outBoundP(var_out_addr, var_size) | mirror(var_in_addr, var_size)
    | smirror(string) | contains(string) | startWith(string) | endWith(string) )]
    [.after("some-mocker-id")]
    .will( returnValue(1) | repeat(1, 20) | returnObjectList(r1, r2)
    | invoke(func_stub) | ignoreReturnValue()
    | increase(from, to) | increase(from) | throws(exception) | die(3))
    [.then(returnValue(2))]
    [.id("some-mocker-id")]
*/

TEST(TEST_MOCKCPP, test_expects)
{
    MOCKER(add).expects(atMost(2)).will(invoke(add_stub));
    add(5, 4);
    add(6, 5);
    add(7, 6);
    MOCKER(add).stubs();
}

TEST(TEST_MOCKCPP, test_will_and_then)
{
    // MOCKER(add).stubs()
    //         .with(eq(2), any())
    //         .will(returnValue(1))
    //         .then(increase(20));
    // add(2, 2);
    // add(2, 2);
    // add(2, 2);
    // add(2, 2);
    // MOCKER(add).stubs();
}

TEST(TEST_MOCKCPP, test_with)
{
    MOCKER(add).stubs()
    .with(eq(2), any())
    .will(invoke(add_stub));
    MOCKER(add).stubs();
}