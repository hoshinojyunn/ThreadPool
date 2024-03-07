cmake_minimum_required(VERSION 3.21)
set(CMAKE_CXX_STANDARD 20)
project(ThreadPool)
set(EXECUTABLE_OUTPUT_PATH ${CMAKE_BINARY_DIR}/bin)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
set(CMAKE_CXX_COMPILER /usr/bin/clang++)
set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -O0 -ggdb -fsanitize=address -fno-omit-frame-pointer -fno-optimize-sibling-calls")
# include your headers
include_directories(${CMAKE_SOURCE_DIR}/includes)
set(SOURCES src/thread_pool.cpp src/main.cpp)
add_executable(${CMAKE_PROJECT_NAME} ${SOURCES})
target_compile_options(${CMAKE_PROJECT_NAME} PRIVATE -g)

