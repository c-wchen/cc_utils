project(wsrc)
AUX_SOURCE_DIRECTORY(. SRC_LIST)

# global compile options
add_compile_options(-std=gnu99)
if (ENABLE_COVERAGE)
    message("ENABLE COVERAGE ON")
    add_compile_options(-fprofile-arcs -ftest-coverage)
    link_libraries(gcov)
endif ()

if (ENABLE_DEBUG)
    message("ENABLE_DEBUG ON")
    add_compile_options(-g -O0)
endif()

if (ENABLE_ASAN)
    message("ENABLE_ASAN ON")
    add_compile_options("-fsanitize=address") # prepare: dnf|yum install libasan
    link_libraries(asan)
endif()

add_library(${PROJECT_NAME} SHARED ${SRC_LIST})
add_library(${PROJECT_NAME}_static STATIC ${SRC_LIST})

add_subdirectory(cli)
add_subdirectory(binlog)