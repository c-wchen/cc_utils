project(wtest)
add_executable(${PROJECT_NAME} main.cpp wchen_point_array_testcase.cpp)
target_link_libraries(${PROJECT_NAME} gtest wsrc)