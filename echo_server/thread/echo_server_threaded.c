// echo_server_threaded.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <pthread.h>
#include <arpa/inet.h>

#define PORT 8888
#define BUF_SIZE 1024

void *handle_client(void *arg) {
    int client_fd = *(int *)arg;
    free(arg);  // 释放传进来的动态分配内存
    char buffer[BUF_SIZE];
    ssize_t n;

    printf("New thread started for client fd %d\n", client_fd);

    while ((n = read(client_fd, buffer, BUF_SIZE)) > 0) {
        write(client_fd, buffer, n);  // 回显
    }

    printf("Client fd %d disconnected\n", client_fd);
    close(client_fd);
    return NULL;
}

int main() {
    int listen_fd;
    struct sockaddr_in server_addr, client_addr;
    socklen_t client_len;

    // 1. 创建监听 socket
    listen_fd = socket(AF_INET, SOCK_STREAM, 0);
    if (listen_fd < 0) {
        perror("socket");
        exit(EXIT_FAILURE);
    }

    // 2. 设置地址结构
    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = INADDR_ANY;
    server_addr.sin_port = htons(PORT);

    // 3. 绑定
    if (bind(listen_fd, (struct sockaddr *)&server_addr, sizeof(server_addr)) < 0) {
        perror("bind");
        close(listen_fd);
        exit(EXIT_FAILURE);
    }

    // 4. 监听
    if (listen(listen_fd, 10) < 0) {
        perror("listen");
        close(listen_fd);
        exit(EXIT_FAILURE);
    }

    printf("Multithreaded Echo Server started on port %d...\n", PORT);

    while (1) {
        client_len = sizeof(client_addr);
        int *conn_fd = malloc(sizeof(int));
        *conn_fd = accept(listen_fd, (struct sockaddr *)&client_addr, &client_len);
        if (*conn_fd < 0) {
            perror("accept");
            free(conn_fd);
            continue;
        }

        printf("Accepted connection from %s:%d\n",
               inet_ntoa(client_addr.sin_addr), ntohs(client_addr.sin_port));

        // 5. 创建新线程处理客户端
        pthread_t tid;
        if (pthread_create(&tid, NULL, handle_client, conn_fd) != 0) {
            perror("pthread_create");
            close(*conn_fd);
            free(conn_fd);
        } else {
            pthread_detach(tid);  // 自动回收线程资源
        }
    }

    close(listen_fd);
    return 0;
}

