project(wsrc)
AUX_SOURCE_DIRECTORY(. SRC_LIST)
add_library(${PROJECT_NAME} SHARED ${SRC_LIST})
add_library(${PROJECT_NAME}_static STATIC ${SRC_LIST})


set(CMAKE_C_FLAGS "-std=c11")
message("LCOV is enabled: ${ENABLE_COVERAGE}")
if (ENABLE_COVERAGE)
    SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fprofile-arcs -ftest-coverage")
    SET(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -fprofile-arcs -ftest-coverage")
endif ()

if (ENABLE_DEBUG)
    target_compile_options(${PROJECT_NAME} PRIVATE -g -O1)
endif()