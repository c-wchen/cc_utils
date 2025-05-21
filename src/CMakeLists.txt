project(wsrc)
AUX_SOURCE_DIRECTORY(. SRC_LIST)
add_library(${PROJECT_NAME} SHARED ${SRC_LIST})
add_library(${PROJECT_NAME}_static STATIC ${SRC_LIST})


set(CMAKE_C_FLAGS "-std=gnu99")
if (ENABLE_COVERAGE)
    message("ENABLE COVERAGE ON")
    SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fprofile-arcs -ftest-coverage")
    SET(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -fprofile-arcs -ftest-coverage")
endif ()

if (ENABLE_DEBUG)
    message("ENABLE_DEBUG ON")
    add_compile_options(-g -O0)
endif()

if (ENABLE_ASAN)
    message("ENABLE_ASAN ON")
    add_link_options("-fsanitize=address") # prepare: dnf|yum install libasan
endif()

add_subdirectory(cli)