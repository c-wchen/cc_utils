cmake_minimum_required(VERSION 3.17)
project(wtest)
set(CMAKE_VERBOSE_MAKEFILEON ON)

option(ENABLE_ASAN "ENABLE_ASAN: " ON)
option(ENABLE_COVERAGE "ENABLE_COVERAGE: " ON)
option(ENABLE_DEBUG "ENABLE_COVERAGE: " ON)

# asan
set(CMAKE_CXX_FLAGS "-g -std=c++11 -fno-omit-frame-pointer")
if (ENABLE_ASAN)
	add_definitions("-fsanitize=address")
endif()
#lcov
if (ENABLE_COVERAGE)
    SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fprofile-arcs -ftest-coverage")
    SET(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -fprofile-arcs -ftest-coverage")
endif ()

link_directories(${CMAKE_SOURCE_DIR}/lib)

include_directories(CMAKE_SOURCE_DIR/include)
aux_source_directory(. TEST_SRC_LIST)
list(REMOVE_ITEM TEST_SRC_LIST "./test_glib.cpp")
list(REMOVE_ITEM TEST_SRC_LIST "./test_gtest.cpp")

add_executable(${PROJECT_NAME} ${TEST_SRC_LIST})
set(THIRD_PARTY_PATH ../third_party)
set(TOP_DIR ..)
set(BIN_DIRECTORY_DIR binary_dir)

# glib-2.0
# find_package(PkgConfig REQUIRED)
# pkg_check_modules(deps REQUIRED IMPORTED_TARGET glib-2.0)
# target_link_libraries(${PROJECT_NAME} PkgConfig::deps)

## 第三方头文件
include_directories(${THIRD_PARTY_PATH}/uthash/src)
include_directories(${THIRD_PARTY_PATH}/googletest/googletest/include)
include_directories(${THIRD_PARTY_PATH}/mockcpp/3rdparty)
include_directories(${THIRD_PARTY_PATH}/mockcpp/include)
include_directories(${TOP_DIR}/include)


##　gtest编译
set(GOOGLETEST_VERSION 1.11.0)

if (NOT EXISTS ${CMAKE_SOURCE_DIR}/lib/libgtest.a)
    add_subdirectory(${THIRD_PARTY_PATH}/googletest/googletest gtest_${BIN_DIRECTORY_DIR})
endif()

if (NOT EXISTS ${CMAKE_SOURCE_DIR}/lib/libmockcpp.a)
    add_subdirectory(${THIRD_PARTY_PATH}/mockcpp mockcpp_${BIN_DIRECTORY_DIR})
endif()

add_subdirectory(${TOP_DIR} wsrc_${BIN_DIRECTORY_DIR})

target_link_libraries(${PROJECT_NAME} wsrc_static gtest mockcpp pthread asan dl)
target_compile_options(${PROJECT_NAME} PRIVATE -g -O1)