project(wsrc)
AUX_SOURCE_DIRECTORY(. SRC_LIST)
add_executable(${PROJECT_NAME}_run ${SRC_LIST})
add_library(${PROJECT_NAME} ${SRC_LIST})

target_include_directories(${PROJECT_NAME} PUBLIC ${PROJECT_SOURCE_DIR})