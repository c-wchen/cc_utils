mock工具的作用是指定函数的行为（模拟函数的行为）。可以对入参进行校验，对出参进行设定，还可以指定函数的返回值。

## 模板


```c
TEST(TEST_MOCKCPP, test_detail)
{
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
}
```

## 约束

### 调用次数约束

.stub()：指定函数调用行为，不检验调用次数

.expact(once())/.expact(never()) ：需要校验次数

```c
MOCKER(add).expects(atMost(2)).will(invoke(add_stub));
add(5, 4);
add(6, 5);
add(7, 6);
MOCKER(add).stubs();

// output ==>
func addStub
unknown file: Failure
C++ exception with description "Expected at most 2 times, but you are trying to invoke more than that.
method(add)
     .expects(atMost(2))
     .invoked(2)
     .will(invoke(0x430dd2));" thrown in the test body.
```

|方法|描述|
|---|----|
|onece()|只调用一次|
|never()|绝不调用|
|atMost(num)|调用最多num次|
|atLeast(num)|调用最少num次|
|exactly(num)|调用精确num次|

### 调用者选择器

caller

### 调用次序约束

| 方法         | 描述                               |
| ------------ | ---------------------------------- |
| id(name)     | 标识符，定义之后before/after才有效 |
| before(name) | 在标识符name之前运行               |
| after(name)  | 在标识符name之后运行               |

### 调用参数约束

.with(eq(val), any(), gt(val),...)

```c
MOCKER(add).stubs()
    .with(eq(2), any())
    .will(invoke(add_stub));
printf("===%d\n", add(2, 2));
printf("===%d\n", add(2, 2));
```

| 方法                 | 描述           |
| -------------------- | -------------- |
| eq(val)              | 相等           |
| neq(val)             | 不相等         |
| gt(val)              | 大于           |
| lt(val)              | 小于           |
| mirroc(arress, size) | 内存匹配       |
| outBound(obj)        | 引用传值       |
| any()                | 占位符         |
| startWith(str)       | 字符串开始匹配 |
| endWith(str)         | 字符串尾部匹配 |
| contains(str)        | 字符串包含匹配 |

### 函数调用行为 —— will()/then()

will和then结合表示调用返回次序

```c
 MOCKER(add).stubs()
     .will(returnValue(1))
     .then(increase(20));
add(2, 3);
add(2, 3);
```

| 方法                              | 描述                |
| --------------------------------- | ------------------- |
| returnValue(val)                  | 返回确定值          |
| returnObjectList(o1, o2, o3,...)  | 返回列表            |
| ignoreReturnValue()               | 忽略返回值          |
| invoke(stubFunc)                  | 打桩函数            |
| repeact(val, times)               | 重复返回val times次 |
| increase(from, to)/increase(from) | 返回递增值          |


## 支持动态库

mockcpp会根据函数地址进行动态插装，在使用动态库，函数地址指向当前目标文件中的plt项地址，并非指向动态库加载后的函数地址，为了解决该问题，通过dlysm进行查找函数的真实地址。

动态库函数查找: plt->(got)->函数真实地址

```c
// dlsysm修正动态库的函数地址
#define DFUNC(func) （(typeof(func) *)dlsym(RTLD_NEXT, #func))

MOCKER(DFUNC(func))
```

## 参考

[MockCpp手册（中文）_Tony_Wong的博客-CSDN博客_mockcpp](https://blog.csdn.net/Tony_Wong/article/details/38752355)