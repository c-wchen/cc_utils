project(wtest)
set(CMAKE_VERBOSE_MAKEFILEON ON)
# add asan
set(CMAKE_CXX_FLAGS "-g -fsanitize=address -fno-omit-frame-pointer")
aux_source_directory(. TEST_SRC_LIST)
add_executable(${PROJECT_NAME} ${TEST_SRC_LIST})
target_link_libraries(${PROJECT_NAME} gtest wsrc)

set(CMAKE_C_FLAGS "-std=c11")
if (ENABLE_COVERAGE)
    SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fprofile-arcs -ftest-coverage")
    SET(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -fprofile-arcs -ftest-coverage")
endif ()
