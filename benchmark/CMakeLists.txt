cmake_minimum_required(VERSION 3.17)
project(wbenmark)
set(CMAKE_VERBOSE_MAKEFILEON ON)

link_directories(${CMAKE_SOURCE_DIR}/lib)

include_directories(CMAKE_SOURCE_DIR/include)
aux_source_directory(. BENCH_SRC_LIST)

add_executable(${PROJECT_NAME} ${BENCH_SRC_LIST})
set(THIRD_PARTY_PATH ../third_party)
set(TOP_DIR ..)
set(BIN_DIRECTORY_DIR binary_dir)

add_definitions(-DBENCHMARK_ENABLE_GTEST_TESTS=OFF -DBENCHMARK_DOWNLOAD_DEPENDENCIES=ON -DCMAKE_BUILD_TYPE=Release)

include_directories(${THIRD_PARTY_PATH}/benchmark/include)

target_link_libraries(${PROJECT_NAME} pthread benchmark)
target_compile_options(${PROJECT_NAME} PRIVATE -g)