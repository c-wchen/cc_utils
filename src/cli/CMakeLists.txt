project(wcmd)
set(SERVER_LIST
    cli_server.c
    options.c
)
SET(CLIENT_LIST
    cli_client.c
)
add_library(${PROJECT_NAME} SHARED ${SERVER_LIST})


add_executable(wcmdc ${CLIENT_LIST})
target_link_libraries(wcmdc wcmd)


add_executable(wcmd_server_example cli_server_example.c)
target_link_libraries(wcmd_server_example ${PROJECT_NAME})