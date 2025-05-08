#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <errno.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/select.h>
#include <netinet/in.h>
#include <arpa/inet.h>

#define PORT 8888
#define BACKLOG 10
#define BUF_SIZE 1024
#define MAX_CLIENTS FD_SETSIZE

int main() {
    int listenfd, connfd, maxfd, sockfd;
    int client_fds[MAX_CLIENTS];
    struct sockaddr_in server_addr, client_addr;
    socklen_t client_len;
    char buf[BUF_SIZE];
    fd_set read_fds, all_fds;

    // 初始化客户端 fd 数组
    for (int i = 0; i < MAX_CLIENTS; ++i)
        client_fds[i] = -1;

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

    // 初始化 fd 集合
    FD_ZERO(&all_fds);
    FD_SET(listenfd, &all_fds);
    maxfd = listenfd;

    printf("Echo server running on port %d...\n", PORT);

    // 主循环
    while (1) {
        read_fds = all_fds; // 每次都要复制，因为 select 会修改它
        int nready = select(maxfd + 1, &read_fds, NULL, NULL, NULL);
        if (nready < 0) {
            perror("select");
            break;
        }

        // 新连接
        if (FD_ISSET(listenfd, &read_fds)) {
            client_len = sizeof(client_addr);
            connfd = accept(listenfd, (struct sockaddr*)&client_addr, &client_len);
            if (connfd < 0) {
                perror("accept");
                continue;
            }

            printf("Accepted connection from %s:%d\n",
                   inet_ntoa(client_addr.sin_addr),
                   ntohs(client_addr.sin_port));

            // 添加到客户端列表
            int i;
            for (i = 0; i < MAX_CLIENTS; ++i) {
                if (client_fds[i] < 0) {
                    client_fds[i] = connfd;
                    break;
                }
            }

            if (i == MAX_CLIENTS) {
                fprintf(stderr, "Too many clients\n");
                close(connfd);
                continue;
            }

            FD_SET(connfd, &all_fds);
            if (connfd > maxfd)
                maxfd = connfd;

            if (--nready <= 0)
                continue;
        }

        // 处理客户端数据
        for (int i = 0; i < MAX_CLIENTS; ++i) {
            sockfd = client_fds[i];
            if (sockfd < 0)
                continue;

            if (FD_ISSET(sockfd, &read_fds)) {
                int n = read(sockfd, buf, BUF_SIZE);
                if (n <= 0) {
                    if (n == 0) {
                        printf("Client disconnected (fd=%d)\n", sockfd);
                    } else {
                        perror("read");
                    }
                    close(sockfd);
                    FD_CLR(sockfd, &all_fds);
                    client_fds[i] = -1;
                } else {
                    // 回显
                    write(sockfd, buf, n);
                }

                if (--nready <= 0)
                    break;
            }
        }
    }

    close(listenfd);
    return 0;
}

