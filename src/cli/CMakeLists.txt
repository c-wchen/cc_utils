project(wcli)
set(SRC_LIST
    cli_client.c
    cli_server.c
)
add_library(${PROJECT_NAME} SHARED ${SRC_LIST})
add_library(${PROJECT_NAME}_static STATIC ${SRC_LIST})