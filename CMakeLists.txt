cmake_minimum_required(VERSION 3.16)
project(proxy)

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED YES)

add_subdirectory(lib/sockpp)
add_subdirectory(lib/logpp)
add_subdirectory(src)
