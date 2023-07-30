project(wtest)
set(CMAKE_VERBOSE_MAKEFILEON ON)
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

include_directories(CMAKE_SOURCE_DIR/include)
aux_source_directory(. TEST_SRC_LIST)
list(REMOVE_ITEM TEST_SRC_LIST "./test_glib.cpp")
list(REMOVE_ITEM TEST_SRC_LIST "./test_gtest.cpp")

add_executable(${PROJECT_NAME} ${TEST_SRC_LIST})
target_link_libraries(${PROJECT_NAME} wsrc gtest mockcpp)

# glib-2.0
# find_package(PkgConfig REQUIRED)
# pkg_check_modules(deps REQUIRED IMPORTED_TARGET glib-2.0)
# target_link_libraries(${PROJECT_NAME} PkgConfig::deps)

