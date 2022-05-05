project(wsrc)
add_executable(${PROJECT_NAME}_run point_array.c)
add_library(${PROJECT_NAME} point_array.c)

#target_include_directories(${PROJECT_NAME} PUBLIC ${PROJECT_SOURCE_DIR})