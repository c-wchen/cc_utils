cmake_minimum_required(VERSION 3.17)
project(wchen_cc_utils)
set(CMAKE_CXX_STANDARD 14)

## 第三方头文件
include_directories(third_party/uthash/src)
include_directories(third_party/googletest/googletest/include)
include_directories(third_party/mockcpp/3rdparty)
include_directories(third_party/mockcpp/include)
## 本模块项目
include_directories(include)
add_subdirectory(src)
add_subdirectory(testcase)

##　gtest编译
set(GOOGLETEST_VERSION 1.11.0)
add_subdirectory(third_party/googletest/googletest)
add_subdirectory(third_party/mockcpp)