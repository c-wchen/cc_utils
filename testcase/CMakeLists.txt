project(wtest)
aux_source_directory(. TEST_SRC_LIST)
add_executable(${PROJECT_NAME} ${TEST_SRC_LIST})
target_link_libraries(${PROJECT_NAME} gtest wsrc)