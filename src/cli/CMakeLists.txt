project(wcli)
set(SERVER_LIST
    cli_server.c
)
SET(CLIENT_LIST
    cli_client.c
)
add_library(${PROJECT_NAME} SHARED ${SERVER_LIST})


add_executable(wcmd ${CLIENT_LIST})
target_link_libraries(wcmd wcli)


add_executable(wcli_example cli_server_example.c)
target_link_libraries(wcli_example wcli)