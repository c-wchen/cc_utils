project(wblog)

set(SRC_LIST
    binlog.c
)
add_library(${PROJECT_NAME} SHARED ${SRC_LIST})

add_executable(binlog_example binlog_example.c)
target_link_libraries(binlog_example ${PROJECT_NAME} pthread)


add_executable(binlog_parser binlog_parser.c)
target_link_libraries(binlog_parser)