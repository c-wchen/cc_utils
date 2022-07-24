cmake_minimum_required(VERSION 3.17)
project(wchen_cc_utils)
set(CMAKE_CXX_STANDARD 14)

## 本模块项目
include_directories(include)
include_directories(third_party/uthash/src)
add_subdirectory(src)
add_subdirectory(testcase)

##　gtest编译
set(GOOGLETEST_VERSION 1.11.0)
set(GTEST_SOURCE_DIR third_party/googletest)
# 全局可见头文件
include_directories(${GTEST_SOURCE_DIR}/googletest/include)
include_directories(${GTEST_SOURCE_DIR}/googlemock/include)
add_subdirectory(${GTEST_SOURCE_DIR}/googletest)

set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -Wall -fexec-charset=GBK")