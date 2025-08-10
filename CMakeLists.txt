cmake_minimum_required(VERSION 3.17)
project(cc_utils)
set(CMAKE_CXX_STANDARD 14)

set(TOP_PROJECT_DIR ${PROJECT_SOURCE_DIR})

# 第三方模块
add_subdirectory(${TOP_PROJECT_DIR}/third_party/cJSON cJSON_binary_dir)
include_directories(${TOP_PROJECT_DIR}/third_party/cJSON)

## 本模块项目
include_directories(include)
add_subdirectory(src)