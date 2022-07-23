project(wsrc)
AUX_SOURCE_DIRECTORY(. SRC_LIST)
add_library(${PROJECT_NAME} ${SRC_LIST})

target_include_directories(${PROJECT_NAME} PUBLIC ${PROJECT_SOURCE_DIR})

set(CMAKE_C_FLAGS "-std=c11")
message("LCOV is enabled: ${ENABLE_COVERAGE}")
if (ENABLE_COVERAGE)
    SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fprofile-arcs -ftest-coverage")
    SET(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -fprofile-arcs -ftest-coverage")
endif ()