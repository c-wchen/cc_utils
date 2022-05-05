## 添加gtest子模块
```git submodule add git@github.com:google/googletest.git third_party/googletest```

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

### 注意项
如果是C语言和C++混合编译在头文件需要添加```__cplusplus```
```c
#ifndef __XXX__
#define __XXX__

#ifdef __cplusplus // 防止被C++重载
extern "C" {
#endif
...
#ifdef __cplusplus
}
#endif
#endif
```

###cmake使用

1. add_execute
2. target_link_library
3. ...