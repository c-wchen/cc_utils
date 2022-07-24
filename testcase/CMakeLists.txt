project(wtest)
set(CMAKE_VERBOSE_MAKEFILEON ON)
# asan
set(CMAKE_CXX_FLAGS "-g -fsanitize=address -fno-omit-frame-pointer")
aux_source_directory(. TEST_SRC_LIST)
add_executable(${PROJECT_NAME} ${TEST_SRC_LIST})
target_link_libraries(${PROJECT_NAME} gtest wsrc)
# glib-2.0
find_package(PkgConfig REQUIRED)
pkg_check_modules(deps REQUIRED IMPORTED_TARGET glib-2.0)
target_link_libraries(${PROJECT_NAME} PkgConfig::deps)
# lcov
set(CMAKE_C_FLAGS "-std=c11")
if (ENABLE_COVERAGE)
    SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fprofile-arcs -ftest-coverage")
    SET(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -fprofile-arcs -ftest-coverage")
endif ()
