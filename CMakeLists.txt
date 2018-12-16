cmake_minimum_required(VERSION 3.10)
project(high_performance_linux_server_programming)

set(CMAKE_CXX_STANDARD 17)

set(SRC_DIR ${PROJECT_SOURCE_DIR}/src)

set(INCLUDE_DIR ${PROJECT_SOURCE_DIR}/include)

add_subdirectory(${SRC_DIR})