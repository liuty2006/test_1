cmake_minimum_required(VERSION 3.10)

# 项目名称
project(ZeroMQExample)

# 设置 C++ 标准
set(CMAKE_CXX_STANDARD 11)

# 查找 ZeroMQ 包
find_package(ZeroMQ REQUIRED)

# 指定可执行文件
add_executable(server server.cpp)
add_executable(client client.cpp)

# 链接 ZeroMQ 库
target_link_libraries(server PRIVATE ZeroMQ::ZeroMQ)
target_link_libraries(client PRIVATE ZeroMQ::ZeroMQ)

