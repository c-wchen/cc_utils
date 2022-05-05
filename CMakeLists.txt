cmake_minimum_required(VERSION 3.20)
project(wchen_cc_utils)
set(CMAKE_CXX_STANDARD 14)
#set (LIBRARY_OUTPUT_PATH ${PROJECT_SOURCE_DIR}/lib)



## 本模块项目
include_directories(include)
add_subdirectory(src)
add_subdirectory(testcase)

##　gtest编译
set(GOOGLETEST_VERSION 1.11.0)
# 全局可见头文件
include_directories(third_party/googletest-release-1.11.0/googletest/include)
include_directories(third_party/googletest-release-1.11.0/googlemock/include)
add_subdirectory(third_party/googletest-release-1.11.0/googletest)
#add_subdirectory(third_party/googletest-release-1.11.0/googlemock)

#定义需要暴露的头文件
#target_include_directories(${PROJECT_NAME} PUBLIC ${PROJECT_SOURCE_DIR})
