// epoll_broker.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <unistd.h>
#include <fcntl.h>
#include <netinet/in.h>
#include <sys/epoll.h>
#include <sys/socket.h>

//#define PORT 1883
#define PORT 1884
#define MAX_EVENTS 64
#define MAX_CLIENTS 128
#define BUF_SIZE 1024

int make_socket_non_blocking(int fd) {
    int flags = fcntl(fd, F_GETFL, 0);
    if (flags == -1) return -1;
    return fcntl(fd, F_SETFL, flags | O_NONBLOCK);
}

int create_and_bind() {
    int sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd < 0) { perror("socket"); exit(1); }

    int opt = 1;
    setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));

    struct sockaddr_in addr = {
        .sin_family = AF_INET,
        .sin_port = htons(PORT),
        .sin_addr.s_addr = INADDR_ANY
    };

    if (bind(sockfd, (struct sockaddr*)&addr, sizeof(addr)) < 0) {
        perror("bind");
        exit(1);
    }

    return sockfd;
}

int main() {
    int listen_fd = create_and_bind();
    make_socket_non_blocking(listen_fd);
    listen(listen_fd, SOMAXCONN);

    int epoll_fd = epoll_create1(0);
    if (epoll_fd == -1) { perror("epoll_create1"); exit(1); }

    struct epoll_event ev = {
        .events = EPOLLIN,
        .data.fd = listen_fd
    };
    epoll_ctl(epoll_fd, EPOLL_CTL_ADD, listen_fd, &ev);

    struct epoll_event events[MAX_EVENTS];

    printf("MQTT pseudo broker listening on port %d...\n", PORT);

    while (1) {
        int n = epoll_wait(epoll_fd, events, MAX_EVENTS, -1);
        for (int i = 0; i < n; i++) {
            if (events[i].data.fd == listen_fd) {
                // 新客户端连接
                struct sockaddr_in client_addr;
                socklen_t client_len = sizeof(client_addr);
                int client_fd = accept(listen_fd, (struct sockaddr*)&client_addr, &client_len);
                if (client_fd == -1) continue;

                make_socket_non_blocking(client_fd);
                ev.events = EPOLLIN;
                ev.data.fd = client_fd;
                epoll_ctl(epoll_fd, EPOLL_CTL_ADD, client_fd, &ev);
                printf("New client connected: fd=%d\n", client_fd);
            } else {
                // 客户端数据
                char buf[BUF_SIZE];
                int count = read(events[i].data.fd, buf, BUF_SIZE);
                if (count <= 0) {
                    close(events[i].data.fd);
                    printf("Client disconnected: fd=%d\n", events[i].data.fd);
                } else {
                    printf("Received from fd=%d: %d bytes\n", events[i].data.fd, count);
                    // 实际 MQTT broker 会解析 MQTT 协议包
                }
            }
        }
    }

    close(listen_fd);
    return 0;
}

