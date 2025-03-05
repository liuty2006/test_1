cmake_minimum_required(VERSION 3.10)

project(test_1)

if (MSVC)
    add_compile_options(/utf-8)
endif()

set(CMAKE_CXX_STANDARD 11)

add_executable(test_1 main.cpp)

