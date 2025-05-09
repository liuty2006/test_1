#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <errno.h>
#include <fcntl.h>
#include <netinet/in.h>
#include <sys/epoll.h>
#include <sys/socket.h>
#include <time.h>

//#define PORT 1883
#define PORT 1884
#define MAX_EVENTS 64
#define BUF_SIZE 1024
#define MAX_CLIENTS 128
#define KEEPALIVE_GRACE 5

typedef struct {
    int fd;
    time_t last_seen;
    int keepalive;
    char client_id[64];
    char topic[64];
    int connected;
    char outbox[BUF_SIZE];
    int outbox_len;
} client_t;

client_t *clients[MAX_CLIENTS];

int make_socket_non_blocking(int fd) {
    int flags = fcntl(fd, F_GETFL, 0);
    return fcntl(fd, F_SETFL, flags | O_NONBLOCK);
}

int create_and_bind() {
    int s = socket(AF_INET, SOCK_STREAM, 0);
    setsockopt(s, SOL_SOCKET, SO_REUSEADDR, &(int){1}, sizeof(int));

    struct sockaddr_in addr = {
        .sin_family = AF_INET,
        .sin_port = htons(PORT),
        .sin_addr.s_addr = INADDR_ANY
    };
    bind(s, (struct sockaddr *)&addr, sizeof(addr));
    return s;
}

client_t* add_client(int fd) {
    client_t *c = calloc(1, sizeof(client_t));
    c->fd = fd;
    c->last_seen = time(NULL);
    clients[fd] = c;
    return c;
}

void remove_client(int fd, int epoll_fd) {
    if (clients[fd]) {
        printf("Client disconnected: %s (fd=%d)\n", clients[fd]->client_id, fd);
        epoll_ctl(epoll_fd, EPOLL_CTL_DEL, fd, NULL);
        close(fd);
        free(clients[fd]);
        clients[fd] = NULL;
    }
}

// MQTT packet types
#define MQTT_CONN 0x10
#define MQTT_CONNACK 0x20
#define MQTT_PUBLISH 0x30
#define MQTT_SUBSCRIBE 0x82
#define MQTT_SUBACK 0x90
#define MQTT_PINGREQ 0xC0
#define MQTT_PINGRESP 0xD0

void send_packet(int fd, const void *buf, int len) {
    int n = send(fd, buf, len, 0);
    if (n <= 0) perror("send");
}

void handle_connect(client_t *c, const unsigned char *buf, int len) {
    if (len < 10) return;
    int keepalive = (buf[len - 3] << 8) | buf[len - 2];
    int client_id_len = (buf[12] << 8) | buf[13];
    if (client_id_len >= sizeof(c->client_id)) client_id_len = sizeof(c->client_id) - 1;
    memcpy(c->client_id, &buf[14], client_id_len);
    c->client_id[client_id_len] = 0;
    c->keepalive = keepalive;
    c->connected = 1;
    c->last_seen = time(NULL);
    printf("Client connected: %s (keepalive=%ds)\n", c->client_id, keepalive);

    // send CONNACK
    unsigned char resp[4] = {0x20, 0x02, 0x00, 0x00};
    send_packet(c->fd, resp, 4);
}

void handle_subscribe(client_t *c, const unsigned char *buf, int len) {
    int topic_len = (buf[5] << 8) | buf[6];
    if (topic_len >= sizeof(c->topic)) topic_len = sizeof(c->topic) - 1;
    memcpy(c->topic, &buf[7], topic_len);
    c->topic[topic_len] = 0;
    printf("%s subscribed to topic: %s\n", c->client_id, c->topic);

    // SUBACK
    unsigned char resp[5] = {0x90, 0x03, buf[2], buf[3], 0x00}; // Packet ID + QoS 0
    send_packet(c->fd, resp, 5);
}

void publish_to_subscribers(client_t *sender, const char *topic, const char *msg, int msglen) {
    unsigned char pkt[BUF_SIZE];
    int topic_len = strlen(topic);
    int pos = 0;

    pkt[pos++] = 0x30; // PUBLISH, QoS 0
    pkt[pos++] = 2 + topic_len + msglen; // remaining length
    pkt[pos++] = topic_len >> 8;
    pkt[pos++] = topic_len & 0xFF;
    memcpy(&pkt[pos], topic, topic_len); pos += topic_len;
    memcpy(&pkt[pos], msg, msglen); pos += msglen;

    for (int i = 0; i < MAX_CLIENTS; i++) {
        client_t *c = clients[i];
        if (c && c != sender && strcmp(c->topic, topic) == 0) {
            memcpy(c->outbox, pkt, pos);
            c->outbox_len = pos;
            struct epoll_event ev = { .events = EPOLLIN | EPOLLOUT, .data.fd = c->fd };
            epoll_ctl(epoll_create1(0), EPOLL_CTL_MOD, c->fd, &ev); // for demo: create new epoll, better: pass epoll_fd
        }
    }
}

void handle_publish(client_t *c, const unsigned char *buf, int len) {
    int topic_len = (buf[2] << 8) | buf[3];
    char topic[64] = {0};
    memcpy(topic, &buf[4], topic_len);
    const char *msg = (const char *)&buf[4 + topic_len];
    int msglen = len - 4 - topic_len;

    printf("PUBLISH from %s: [%s] %.*s\n", c->client_id, topic, msglen, msg);
    publish_to_subscribers(c, topic, msg, msglen);
}

void handle_packet(client_t *c, unsigned char *buf, int len) {
    c->last_seen = time(NULL);
    unsigned char type = buf[0] & 0xF0;
    switch (type) {
        case MQTT_CONN: handle_connect(c, buf, len); break;
        case MQTT_SUBSCRIBE: handle_subscribe(c, buf, len); break;
        case MQTT_PUBLISH: handle_publish(c, buf, len); break;
        case MQTT_PINGREQ: {
            unsigned char resp[2] = {MQTT_PINGRESP, 0};
            send_packet(c->fd, resp, 2);
            break;
        }
        default: printf("Unknown packet: 0x%X\n", type); break;
    }
}

int main() {
    int listen_fd = create_and_bind();
    make_socket_non_blocking(listen_fd);
    listen(listen_fd, SOMAXCONN);

    int epoll_fd = epoll_create1(0);
    struct epoll_event ev = { .events = EPOLLIN, .data.fd = listen_fd };
    epoll_ctl(epoll_fd, EPOLL_CTL_ADD, listen_fd, &ev);

    struct epoll_event events[MAX_EVENTS];

    printf("Mini MQTT broker listening on port %d...\n", PORT);

    while (1) {
        int n = epoll_wait(epoll_fd, events, MAX_EVENTS, 1000);
        time_t now = time(NULL);

        for (int i = 0; i < n; i++) {
            int fd = events[i].data.fd;

            if (fd == listen_fd) {
                struct sockaddr_in caddr; socklen_t clen = sizeof(caddr);
                int client_fd = accept(listen_fd, (struct sockaddr *)&caddr, &clen);
                make_socket_non_blocking(client_fd);
                ev.events = EPOLLIN; ev.data.fd = client_fd;
                epoll_ctl(epoll_fd, EPOLL_CTL_ADD, client_fd, &ev);
                add_client(client_fd);
            } else {
                client_t *c = clients[fd];
                if (events[i].events & EPOLLIN) {
                    unsigned char buf[BUF_SIZE];
                    int count = recv(fd, buf, BUF_SIZE, 0);
                    if (count <= 0) {
                        remove_client(fd, epoll_fd);
                    } else {
                        handle_packet(c, buf, count);
                    }
                }
                if (events[i].events & EPOLLOUT && c->outbox_len > 0) {
                    send_packet(fd, c->outbox, c->outbox_len);
                    c->outbox_len = 0;
                    ev.events = EPOLLIN;
                    ev.data.fd = fd;
                    epoll_ctl(epoll_fd, EPOLL_CTL_MOD, fd, &ev);
                }
            }
        }

        // 心跳检测
        for (int i = 0; i < MAX_CLIENTS; i++) {
            client_t *c = clients[i];
            if (c && c->connected && c->keepalive > 0 &&
                now - c->last_seen > c->keepalive + KEEPALIVE_GRACE) {
                printf("Client timeout: %s\n", c->client_id);
                remove_client(c->fd, epoll_fd);
            }
        }
    }

    close(listen_fd);
    return 0;
}

