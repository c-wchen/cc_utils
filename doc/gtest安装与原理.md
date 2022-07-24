## 添加gtest子模块
```bash
git submodule add git@github.com:google/googletest.git third_party/googletest
```

### cmake编译gtest
```cmake
##　gtest编译
set(GOOGLETEST_VERSION 1.11.0)
set(GTEST_SOURCE_DIR third_party/googletest)
# 全局可见头文件
include_directories(${GTEST_SOURCE_DIR}/googletest/include)
include_directories(${GTEST_SOURCE_DIR}/googlemock/include)
add_subdirectory(${GTEST_SOURCE_DIR}/googletest)
#add_subdirectory(third_party/googletest-release-1.11.0/googlemock)
```

## Gtest原理

创建用例

```c
TEST(TEST_DEMO_SUIT, TEST_DEMO_CASE) {
    // ... 测试代码
}
```

预处理之后代码

> 1. 定义继承testing::Test的TEST_DEMO_SUIT_TEST_DEMO_CASE_Test类
> 2. 初始化MakeAndRegisterTestInfo
> 3. TestBody： 实现为测试代码

```c
static_assert(sizeof("TEST_DEMO_SUIT") > 1, "test_suite_name must not be empty");
static_assert(sizeof("TEST_DEMO_CASE") > 1, "test_name must not be empty");
class TEST_DEMO_SUIT_TEST_DEMO_CASE_Test : public::testing::Test {
    public:
    TEST_DEMO_SUIT_TEST_DEMO_CASE_Test()= default;
    ~TEST_DEMO_SUIT_TEST_DEMO_CASE_Test() override = default;
    TEST_DEMO_SUIT_TEST_DEMO_CASE_Test(const TEST_DEMO_SUIT_TEST_DEMO_CASE_Test &) = delete;
    TEST_DEMO_SUIT_TEST_DEMO_CASE_Test &operator = (const TEST_DEMO_SUIT_TEST_DEMO_CASE_Test&) = delete;
    TEST_DEMO_SUIT_TEST_DEMO_CASE_Test(TEST_DEMO_SUIT_TEST_DEMO_CASE_Test &&) noexcept = delete;
    TEST_DEMO_SUIT_TEST_DEMO_CASE_Test& operator = (TEST_DEMO_SUIT_TEST_DEMO_CASE_Test&& ) noexcept = delete;
    private:
    void TestBody()override; static ::testing::TestInfo *const test_info_ __attribute__((unused));
};
::testing::TestInfo *const TEST_DEMO_SUIT_TEST_DEMO_CASE_Test ::test_info_ = ::testing::internal::MakeAndRegisterTestInfo(
        "TEST_DEMO_SUIT",
        "TEST_DEMO_CASE", nullptr, nullptr,
        ::testing::internal::CodeLocation("...", 5),
        (::testing::internal::GetTestTypeId()),
        ::testing::internal::SuiteApiResolver<::testing::Test>::GetSetUpCaseOrSuite("...", 5),
        ::testing::internal::SuiteApiResolver<::testing::Test>::GetTearDownCaseOrSuite("...", 5),
        new::testing::internal::TestFactoryImpl <
        TEST_DEMO_SUIT_TEST_DEMO_CASE_Test>
);

void TEST_DEMO_SUIT_TEST_DEMO_CASE_Test::TestBody() {
    // ... 测试代码
}
```

## 源码流程

TEST->GTEST_TEST->GTEST_TEST_

> 类的静态方法可以在函数外直接赋值，加载时会自动执行MakeAndRegisterTestInfo

```c
#define GTEST_TEST_(test_suite_name, test_name, parent_class, parent_id)       \
  static_assert(sizeof(GTEST_STRINGIFY_(test_suite_name)) > 1,                 \
                "test_suite_name must not be empty");                          \
  static_assert(sizeof(GTEST_STRINGIFY_(test_name)) > 1,                       \
                "test_name must not be empty");                                \
  class GTEST_TEST_CLASS_NAME_(test_suite_name, test_name)                     \
      : public parent_class {                                                  \
   public:                                                                     \
    GTEST_TEST_CLASS_NAME_(test_suite_name, test_name)() = default;            \
    ~GTEST_TEST_CLASS_NAME_(test_suite_name, test_name)() override = default;  \
    GTEST_TEST_CLASS_NAME_(test_suite_name, test_name)                         \
    (const GTEST_TEST_CLASS_NAME_(test_suite_name, test_name) &) = delete;     \
    GTEST_TEST_CLASS_NAME_(test_suite_name, test_name) & operator=(            \
        const GTEST_TEST_CLASS_NAME_(test_suite_name,                          \
                                     test_name) &) = delete; /* NOLINT */      \
    GTEST_TEST_CLASS_NAME_(test_suite_name, test_name)                         \
    (GTEST_TEST_CLASS_NAME_(test_suite_name, test_name) &&) noexcept = delete; \
    GTEST_TEST_CLASS_NAME_(test_suite_name, test_name) & operator=(            \
        GTEST_TEST_CLASS_NAME_(test_suite_name,                                \
                               test_name) &&) noexcept = delete; /* NOLINT */  \
                                                                               \
   private:                                                                    \
    void TestBody() override;                                                  \
    static ::testing::TestInfo* const test_info_ GTEST_ATTRIBUTE_UNUSED_;      \
  };                                                                           \
                                                                               \
  ::testing::TestInfo* const GTEST_TEST_CLASS_NAME_(test_suite_name,           \
                                                    test_name)::test_info_ =   \
      ::testing::internal::MakeAndRegisterTestInfo(                            \
          #test_suite_name, #test_name, nullptr, nullptr,                      \
          ::testing::internal::CodeLocation(__FILE__, __LINE__), (parent_id),  \
          ::testing::internal::SuiteApiResolver<                               \
              parent_class>::GetSetUpCaseOrSuite(__FILE__, __LINE__),          \
          ::testing::internal::SuiteApiResolver<                               \
              parent_class>::GetTearDownCaseOrSuite(__FILE__, __LINE__),       \
          new ::testing::internal::TestFactoryImpl<GTEST_TEST_CLASS_NAME_(     \
              test_suite_name, test_name)>);                                   \
  void GTEST_TEST_CLASS_NAME_(test_suite_name, test_name)::TestBody()
```

->MakeAndRegisterTestInfo

其中形参的意义如下：

- test_case_name：测试套名称，即TEST宏中的第一个形参。
- name：测试案例名称。
- type_param：测试套的附加信息。默认为无
- value_param：测试案例的附加信息。默认为无
- fixture_class_id：test fixture类的id
- set_up_tc ：函数指针，指向函数SetUpTestCaseFunc
- tear_down_tc：函数指针，指向函数TearDownTestCaseFunc
- factory：指向工厂对象的指针，该工厂对象创建上面TEST宏生成的测试类的对象

> 1. 将TestFactoryImpl <TEST_DEMO_SUIT_TEST_DEMO_CASE_Test>的factory包装为TestInfo
>
> 2. 将TestInfo添加到GetUnitTestImpl(): GetUnitTestImpl()获取单例UnitTest

```c
TestInfo* MakeAndRegisterTestInfo(
    const char* test_suite_name, const char* name, const char* type_param,
    const char* value_param, CodeLocation code_location,
    TypeId fixture_class_id, SetUpTestSuiteFunc set_up_tc,
    TearDownTestSuiteFunc tear_down_tc, TestFactoryBase* factory) {
  TestInfo* const test_info =
      new TestInfo(test_suite_name, name, type_param, value_param,
                   code_location, fixture_class_id, factory);
  GetUnitTestImpl()->AddTestInfo(set_up_tc, tear_down_tc, test_info);
  return test_info;
}
```

-> TestFactoryBase

> 工厂类通过CreateTest创建泛型对象（C++的花里胡哨操作）

```c
template <class TestClass>
class TestFactoryImpl : public TestFactoryBase {
 public:
  Test* CreateTest() override { return new TestClass; }
};
```



最后总结一下

1. 每调用一次宏TEST(test_suite_name, test_name)都会调用MakeAndRegisterTestInfo对当前用例初始化
2. 注册用例会将自身的对象信息放入工厂中，通过工厂运行该用例
3. GetUnitTestImpl()返回的是容器的单例包装类，只初始化一次

