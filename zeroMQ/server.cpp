// server.cpp
#include <zmq.hpp>
#include <iostream>
#include <string>

int main() {
    // 初始化 ZeroMQ 上下文
    zmq::context_t context(1);

    // 创建一个 REP (回复) 套接字
    zmq::socket_t socket(context, ZMQ_REP);

    // 绑定到 TCP 端口 5555
    socket.bind("tcp://*:5555");

    while (true) {
        // 接收客户端请求
        zmq::message_t request;
        socket.recv(&request);

        // 打印收到的请求
        std::string received_message(static_cast<char*>(request.data()), request.size());
        std::cout << "Received request: " << received_message << std::endl;

        // 发送响应
        std::string response = "Hello, client!";
        zmq::message_t reply(response.size());
        memcpy(reply.data(), response.data(), response.size());
        socket.send(reply);
    }

    return 0;
}

