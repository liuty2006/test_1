#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <errno.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/poll.h>
#include <netinet/in.h>
#include <arpa/inet.h>

#define PORT 8888
#define BACKLOG 10
#define BUF_SIZE 1024
#define MAX_CLIENTS 1024  // 最大客户端数量

int main() {
    int listenfd, connfd, pollfd_count;
    struct sockaddr_in server_addr, client_addr;
    socklen_t client_len;
    struct pollfd fds[MAX_CLIENTS];  // 用于存储文件描述符
    char buf[BUF_SIZE];

    // 创建监听 socket
    listenfd = socket(AF_INET, SOCK_STREAM, 0);
    if (listenfd < 0) {
        perror("socket");
        exit(EXIT_FAILURE);
    }

    // 设置地址复用
    int opt = 1;
    setsockopt(listenfd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));

    // 绑定地址和端口
    memset(&server_addr, 0, sizeof(server_addr));
    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = INADDR_ANY;
    server_addr.sin_port = htons(PORT);
    if (bind(listenfd, (struct sockaddr*)&server_addr, sizeof(server_addr)) < 0) {
        perror("bind");
        close(listenfd);
        exit(EXIT_FAILURE);
    }

    // 设置监听
    if (listen(listenfd, BACKLOG) < 0) {
        perror("listen");
        close(listenfd);
        exit(EXIT_FAILURE);
    }

    // 初始化 pollfd 数组
    fds[0].fd = listenfd;
    fds[0].events = POLLIN;  // 监听是否有新连接
    pollfd_count = 1;

    printf("Echo server running on port %d...\n", PORT);

    // 主循环
    while (1) {
        int ret = poll(fds, pollfd_count, -1);  // 等待事件发生
        if (ret < 0) {
            perror("poll");
            break;
        }

        // 检查监听 socket 是否有新连接
        if (fds[0].revents & POLLIN) {
            client_len = sizeof(client_addr);
            connfd = accept(listenfd, (struct sockaddr*)&client_addr, &client_len);
            if (connfd < 0) {
                perror("accept");
                continue;
            }

            printf("Accepted connection from %s:%d\n",
                   inet_ntoa(client_addr.sin_addr),
                   ntohs(client_addr.sin_port));

            // 将新连接的文件描述符添加到 pollfd 数组
            fds[pollfd_count].fd = connfd;
            fds[pollfd_count].events = POLLIN;  // 监听是否有数据可读
            pollfd_count++;

            if (pollfd_count >= MAX_CLIENTS) {
                printf("Maximum client limit reached, cannot accept more clients.\n");
                continue;
            }
        }

        // 检查每个客户端的文件描述符是否有数据可读
        for (int i = 1; i < pollfd_count; ++i) {
            if (fds[i].revents & POLLIN) {
                int n = read(fds[i].fd, buf, BUF_SIZE);
                if (n <= 0) {
                    if (n == 0) {
                        printf("Client disconnected (fd=%d)\n", fds[i].fd);
                    } else {
                        perror("read");
                    }
                    close(fds[i].fd);
                    // 从 pollfd 数组中移除该客户端
                    fds[i] = fds[pollfd_count - 1];
                    pollfd_count--;
                } else {
                    // 回显数据
                    write(fds[i].fd, buf, n);
                }
            }
        }
    }

    close(listenfd);
    return 0;
}

