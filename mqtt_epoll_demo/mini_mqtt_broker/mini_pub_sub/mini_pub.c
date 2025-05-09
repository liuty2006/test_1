#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <time.h>

#define BUF_SIZE 1024

void send_connect(int sock, const char *client_id, int keepalive) {
    unsigned char pkt[BUF_SIZE];
    int pos = 0;
    pkt[pos++] = 0x10; // CONNECT
    int remaining_len = 10 + 2 + strlen(client_id);
    pkt[pos++] = remaining_len;

    pkt[pos++] = 0x00; pkt[pos++] = 0x04;
    pkt[pos++] = 'M'; pkt[pos++] = 'Q'; pkt[pos++] = 'T'; pkt[pos++] = 'T';
    pkt[pos++] = 0x04; // Protocol level
    pkt[pos++] = 0x02; // Clean session
    pkt[pos++] = keepalive >> 8;
    pkt[pos++] = keepalive & 0xFF;

    pkt[pos++] = 0x00; pkt[pos++] = strlen(client_id);
    memcpy(&pkt[pos], client_id, strlen(client_id));
    pos += strlen(client_id);

    send(sock, pkt, pos, 0);
}

void send_publish(int sock, const char *topic, const char *msg) {
    unsigned char pkt[BUF_SIZE];
    int pos = 0;
    int topic_len = strlen(topic);
    int msg_len = strlen(msg);

    pkt[pos++] = 0x30; // PUBLISH QoS 0
    pkt[pos++] = 2 + topic_len + msg_len;

    pkt[pos++] = topic_len >> 8;
    pkt[pos++] = topic_len & 0xFF;
    memcpy(&pkt[pos], topic, topic_len); pos += topic_len;
    memcpy(&pkt[pos], msg, msg_len); pos += msg_len;

    send(sock, pkt, pos, 0);
}

void send_ping(int sock) {
    unsigned char pkt[2] = {0xC0, 0x00};
    send(sock, pkt, 2, 0);
}

int main(int argc, char *argv[]) {
    if (argc != 3) {
        fprintf(stderr, "用法: %s <topic> <message>\n", argv[0]);
        return 1;
    }

    const char *topic = argv[1];
    const char *message = argv[2];

    int sock = socket(AF_INET, SOCK_STREAM, 0);
    struct sockaddr_in serv = {
        .sin_family = AF_INET,
        .sin_port = htons(1883),
        .sin_addr.s_addr = inet_addr("127.0.0.1")
    };

    if (connect(sock, (struct sockaddr *)&serv, sizeof(serv)) != 0) {
        perror("connect");
        return 1;
    }

    send_connect(sock, "pub-client", 10);
    sleep(1); // 等待 CONNACK（可省略或改为 epoll/recv 检查）
    send_publish(sock, topic, message);

    printf("已发送消息 [%s]: %s\n", topic, message);

    // 可选：发送一次 PINGREQ 并保持连接一小段时间
    send_ping(sock);
    sleep(1);

    close(sock);
    return 0;
}

