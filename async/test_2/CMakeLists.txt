cmake_minimum_required(VERSION 3.10)

project(c4_1)

#add_compile_options(/utf-8)

set(CMAKE_CXX_STANDARD 11)

link_libraries(pthread)
add_executable(test main.cpp)
