project(wtest)
set(CMAKE_VERBOSE_MAKEFILEON ON)
# add asan
set(CMAKE_CXX_FLAGS "-g -fsanitize=address -fno-omit-frame-pointer")
aux_source_directory(. TEST_SRC_LIST)
add_executable(${PROJECT_NAME} ${TEST_SRC_LIST})
target_link_libraries(${PROJECT_NAME} gtest wsrc)