// epoll_threadpool_echo_server.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <pthread.h>
#include <fcntl.h>
#include <errno.h>
#include <arpa/inet.h>
#include <sys/epoll.h>
#include <sys/socket.h>
#include <netinet/in.h>

#define PORT 8888
#define MAX_EVENTS 1024
#define BUF_SIZE 1024
#define THREAD_POOL_SIZE 4

// --- 任务队列节点 ---
typedef struct task {
    int client_fd;
    struct task *next;
} task_t;

task_t *task_head = NULL, *task_tail = NULL;
pthread_mutex_t task_mutex = PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t task_cond = PTHREAD_COND_INITIALIZER;

// --- 把 socket 设置为非阻塞 ---
void set_non_blocking(int fd) {
    fcntl(fd, F_SETFL, fcntl(fd, F_GETFL, 0) | O_NONBLOCK);
}

// --- 添加任务到队列 ---
void add_task(int client_fd) {
    task_t *new_task = malloc(sizeof(task_t));
    new_task->client_fd = client_fd;
    new_task->next = NULL;

    pthread_mutex_lock(&task_mutex);
    if (task_tail) {
        task_tail->next = new_task;
        task_tail = new_task;
    } else {
        task_head = task_tail = new_task;
    }
    pthread_cond_signal(&task_cond);
    pthread_mutex_unlock(&task_mutex);
}

// --- 工作线程主函数 ---
void *worker_thread(void *arg) {
    char buffer[BUF_SIZE];
    while (1) {
        pthread_mutex_lock(&task_mutex);
        while (task_head == NULL) {
            pthread_cond_wait(&task_cond, &task_mutex);
        }

        task_t *task = task_head;
        task_head = task_head->next;
        if (task_head == NULL) task_tail = NULL;
        pthread_mutex_unlock(&task_mutex);

        int client_fd = task->client_fd;
        free(task);

        ssize_t n = read(client_fd, buffer, BUF_SIZE);
        if (n > 0) {
            write(client_fd, buffer, n);  // 回显
        } else if (n <= 0) {
            close(client_fd);  // 断开或错误
        }
    }
    return NULL;
}

int main() {
    int listen_fd, epoll_fd;
    struct sockaddr_in server_addr;

    // 1. 创建监听 socket
    listen_fd = socket(AF_INET, SOCK_STREAM, 0);
    set_non_blocking(listen_fd);

    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = INADDR_ANY;
    server_addr.sin_port = htons(PORT);
    bind(listen_fd, (struct sockaddr *)&server_addr, sizeof(server_addr));
    listen(listen_fd, 128);

    // 2. 创建 epoll 实例
    epoll_fd = epoll_create1(0);
    struct epoll_event ev, events[MAX_EVENTS];
    ev.events = EPOLLIN;
    ev.data.fd = listen_fd;
    epoll_ctl(epoll_fd, EPOLL_CTL_ADD, listen_fd, &ev);

    // 3. 创建线程池
    pthread_t threads[THREAD_POOL_SIZE];
    for (int i = 0; i < THREAD_POOL_SIZE; ++i) {
        pthread_create(&threads[i], NULL, worker_thread, NULL);
    }

    printf("Epoll + ThreadPool Echo Server started on port %d...\n", PORT);

    // 4. 事件循环
    while (1) {
        int nfds = epoll_wait(epoll_fd, events, MAX_EVENTS, -1);
        for (int i = 0; i < nfds; ++i) {
            if (events[i].data.fd == listen_fd) {
                // 新连接
                int client_fd = accept(listen_fd, NULL, NULL);
                set_non_blocking(client_fd);
                ev.events = EPOLLIN | EPOLLET;  // 边沿触发
                ev.data.fd = client_fd;
                epoll_ctl(epoll_fd, EPOLL_CTL_ADD, client_fd, &ev);
            } else {
                // 有数据可读的连接
                add_task(events[i].data.fd);  // 放入任务队列
            }
        }
    }

    close(listen_fd);
    close(epoll_fd);
    return 0;
}

