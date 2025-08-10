## cmake
1. add_execute
2. target_link_library
3. ...
4. FetchContent
   https://github.com/taoweiji/cpp-cmake-example

## 变量
1.set

变量定义与使用

```cmake
# set(<variable> <value>... [PARENT_SCOPE])
set(PROJECT A1.c A2.c A3.c)
message("${PROJECT}")
```
常用变量

| 环境变量名                                                   | 描述                                                         | 重要性 |
| :----------------------------------------------------------- | :----------------------------------------------------------- | ------ |
| CMAKE_BINARY_DIR, PROJECT_BINARY_DIR, `<projectname>`_BINARY_DIR | cmake命令执行目录                                            |        |
| CMAKE_SOURCE_DIR,                                            | 工程顶层目录，cmake命令执行编译的CMakelists所在目录          | 重要   |
| PROJECT_SOURCE_DIR, `<projectname>`_SOURCE_DIR               | 项目目录，编译目标文件的cmake所在目录，子模块里指代本子模块CMakelists目录 |        |
| CMAKE_CURRENT_SOURCE_DIR                                     | 当前处理的 CMakeLists.txt 所在的路径,比如上面我们提到的 src 子目录。 |        |
| CMAKE_CURRRENT_BINARY_DIR                                    | 如果是 `in-source` 编译,它跟 CMAKE_CURRENT_SOURCE_DIR 一致,如果是 `out-of-source` 编译,他指的是 target 编译目录。 |        |
| EXECUTABLE_OUTPUT_PATH , LIBRARY_OUTPUT_PATH                 | 最终目标文件存放的路径。                                     |        |
| PROJECT_NAME                                                 | 通过 PROJECT 指令定义的项目名称。                            | 重要   |

cmake 编译选项

| 编译控制开关名    | 描述                                                    |
| :---------------- | :------------------------------------------------------ |
| BUILD_SHARED_LIBS | 使用 `ADD_LIBRARY` 时生成动态库                         |
| BUILD_STATIC_LIBS | 使用 `ADD_LIBRARY` 时生成静态库                         |
| CMAKE_C_FLAGS     | 设置 C 编译选项,也可以通过指令 ADD_DEFINITIONS()添加。  |
| CMAKE_CXX_FLAGS   | 设置 C++编译选项,也可以通过指令 ADD_DEFINITIONS()添加。 |

## 常用指令

1.```add_definitions(-DFOO -DBAR ...)```

> 对应gcc -DFOO， 外部添加宏选项

2.```add_dependencies(<target> [<target-dependency>]...)```

> 确保target依赖全部构建完成

3.```aux_source_directory(<dir> <variable>)```

> 将目录下的所有源文件保存到变量中，方便编译时替换

4.```add_subdirectory(source_dir [binary_dir] [EXCLUDE_FROM_ALL])```

> 添加子项目目录，子项目需要CMakeLists.txt才能编译

5.```include_directories([AFTER|BEFORE] [SYSTEM] dir1 [dir2 ...])```\

> 将给定目录添加到编译器用来搜索包含文件的目录中。相对路径被解释为相对于当前源目录

6. ```add_executable(<name> <options>... <sources>...)```

> 添加可执行文件

7. ```add_library(<name> [<type>] [EXCLUDE_FROM_ALL] <sources>...```

> 添加动态库或静态库,type包含STATIC和SHARED

8. ```install(TARGETS <target>... [...])```

> 安装可执行文件、动态库、静态库到指定路径

## 全局操作和target操作

target针对```add_library```或```add_executable```生成的库或执行文件，对目标指定添加编译选项、链接选项等而不影响全局，注意先有target才能对target进行操作。

```cmake
target_include_directories(<target> 
  [SYSTEM] [AFTER|BEFORE]
  <INTERFACE|PUBLIC|PRIVATE> [items1...]
  [<INTERFACE|PUBLIC|PRIVATE> [items2...] ...]
)
```

* target: 执行目标

* SYSTEM: 非必须，将指定的头文件路径标记为 "系统级头文件目录"
* INTERFACE|PUBLIC|PRIVATE: 必须
  * 仅当前目标（`<target>`）编译时需要
  * 仅依赖该目标的**其他目标**编译时需要
  *  当前目标和依赖该目标的其他目标都需要
* item: 添加的链接头文件目录

|global|target|描述|gcc选项|
|--|--|--|--|
|include_directories(...)|target_include_directories(target PRIVATE/PUBLIC/INTERFACE ...)|包含头文件|-I|
|link_directories(...)|target_link_directories(target ...)|链接库目录|-L|
|link_libraries(...)|target_link_libraries(target ...)|链接库|-l|
|add_definitions(...)|                                                              |添加定义宏|-D|
|add_compile_definitions(...)| target_compile_definitions(target...)                        |||
|add_link_options(...)| target_link_options(target...)                               |添加链接选项||
|| target_precompile_headers(target...)                         |添加预编译头，重复头文件只编译一次||
|| target_compile_features(target...)                           |添加语言特性，比如c++17标准，cxx_std_17||
|| target_sources(target...)                                    |添加源文件||
|set(CMAKE_CXX_FLAGS ...)||添加编译选项||

## 参考
[CMake教程之CMake从入门到应用](https://aiden-dong.gitee.io/2019/07/20/CMake%E6%95%99%E7%A8%8B%E4%B9%8BCMake%E4%BB%8E%E5%85%A5%E9%97%A8%E5%88%B0%E5%BA%94%E7%94%A8/
)

[make安装和编译代码](https://blog.csdn.net/Nicholas_Liu2017/article/details/78323391)
