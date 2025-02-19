// client.cpp
#include <zmq.hpp>
#include <iostream>
#include <string>

int main() {
    // 初始化 ZeroMQ 上下文
    zmq::context_t context(1);

    // 创建一个 REQ (请求) 套接字
    zmq::socket_t socket(context, ZMQ_REQ);

    // 连接到服务器端
    socket.connect("tcp://localhost:5555");

    // 发送请求
    std::string request = "Hello, server!";
    zmq::message_t request_message(request.size());
    memcpy(request_message.data(), request.data(), request.size());
    socket.send(request_message);

    // 接收服务器的响应
    zmq::message_t response;
    socket.recv(&response);

    // 打印服务器的响应
    std::string response_message(static_cast<char*>(response.data()), response.size());
    std::cout << "Received response: " << response_message << std::endl;

    return 0;
}

