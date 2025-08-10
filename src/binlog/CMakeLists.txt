project(wblog)

set(SRC_LIST
    binlog.c
)
add_library(${PROJECT_NAME} SHARED ${SRC_LIST})

add_subdirectory(${TOP_PROJECT_DIR}/third_party/cJSON cJSON_binary_dir)


add_executable(binlog_example binlog_example.c)
add_executable(binlog_parser binlog_parser.c)


option(ENABLE_ASAN "ENABLE_ASAN: " ON)
if (ENABLE_ASAN)
    target_compile_options(binlog_parser PRIVATE -fsanitize=address -g -O0)
    target_compile_options(binlog_example PRIVATE -fsanitize=address -g -O0)

    target_link_libraries(binlog_parser PRIVATE asan)
    target_link_libraries(binlog_example PRIVATE asan)
endif()

target_link_libraries(binlog_example PRIVATE pthread ${PROJECT_NAME})
target_link_libraries(binlog_parser PRIVATE cjson)
target_include_directories(binlog_parser PRIVATE ${TOP_PROJECT_DIR}/third_party/cJSON)