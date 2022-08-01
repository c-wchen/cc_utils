

### 调用次数约束

.stub()：指定函数调用行为，不检验调用次数

.expact(once())/.expact(never()) ：需要校验次数



|方法|描述|
|---|----|
|onece()|只调用一次|
|never()||
|atMost(num)||
|atLeast(num)||
|exactly(num)||

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
|                      |                |

### 函数调用行为 —— will()/then()

```bash
MOCKER(func)
.stubs()
.will(returnValue(10))
.then(repeat(20,2))
.then(returnValue(5));
```

| 方法                              | 描述                |
| --------------------------------- | ------------------- |
| returnVal(val)                    | 返回确定值          |
| returnObjectList(o1, o2, o3,...)  | 返回列表            |
| ignoreReturnVal()                 | 忽略返回值          |
| invoke(stubFunc)                  | 打桩函数            |
| repeact(val, times)               | 重复返回val times次 |
| increase(from, to)/increase(from) | 返回递增值          |