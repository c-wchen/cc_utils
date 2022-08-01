### 目录介绍
```bash
├─CI                 #　集成模块，项目运行和集成相关脚本
├─doc
├─include            # src 头文件
├─src                # 案例源码
├─testcase           # 测试用例
└─third_party        # 第三方子模块
```

### 运行
```bash
mkdir build; cd build
cmake .. -DENABLE_COVERAGE=ON
```
### 注意项
1.如果是C语言和C++混合编译在头文件需要添加```__cplusplus```
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
2.libasan和gcc和配套发布的，所以版本需要一致
```bash
[wchen@wchen ~]$ yum list | grep devtoolset-8-gcc*
devtoolset-8-gcc.x86_64                    8.3.1-3.2.el7          @centos-sclo-rh
devtoolset-8-gcc-c++.x86_64                8.3.1-3.2.el7          @centos-sclo-rh
devtoolset-8-gcc-gdb-plugin.x86_64         8.3.1-3.2.el7          @centos-sclo-rh
devtoolset-8-gcc-gfortran.x86_64           8.3.1-3.2.el7          @centos-sclo-rh
devtoolset-8-gcc-plugin-devel.x86_64       8.3.1-3.2.el7          @centos-sclo-rh
[wchen@wchen ~]$ yum list | grep devtoolset-8-libasan*
devtoolset-8-libasan-devel.x86_64          8.3.1-3.2.el7          @centos-sclo-rh
[wchen@wchen ~]$
```