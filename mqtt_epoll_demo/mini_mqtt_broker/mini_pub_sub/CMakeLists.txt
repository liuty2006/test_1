cmake_minimum_required(VERSION 3.10)
project(MiniMQTTClients C)

set(CMAKE_C_STANDARD 99)

# 编译 mini_pub
add_executable(mini_pub mini_pub.c)

# 编译 mini_sub
add_executable(mini_sub mini_sub.c)

