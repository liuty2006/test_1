cmake_minimum_required(VERSION 3.10)

project(test_2)

#add_compile_options(/utf-8)

set(CMAKE_CXX_STANDARD 11)

if(CMAKE_HOST_SYSTEM_NAME STREQUAL "Linux")
    link_libraries(pthread)
endif()

add_executable(test_2 main.cpp)
