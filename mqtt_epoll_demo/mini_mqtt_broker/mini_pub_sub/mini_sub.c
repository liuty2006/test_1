#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <fcntl.h>
#include <time.h>

#define BUF_SIZE 1024

void send_connect(int sock, const char *client_id, int keepalive) {
    unsigned char pkt[BUF_SIZE];
    int pos = 0;
    pkt[pos++] = 0x10; // CONNECT
    int remaining_len = 10 + 2 + strlen(client_id); // fixed part + client_id length
    pkt[pos++] = remaining_len;

    // Protocol Name "MQTT"
    pkt[pos++] = 0x00; pkt[pos++] = 0x04;
    pkt[pos++] = 'M'; pkt[pos++] = 'Q'; pkt[pos++] = 'T'; pkt[pos++] = 'T';
    pkt[pos++] = 0x04;        // Protocol Level 4
    pkt[pos++] = 0x02;        // Connect Flags: CleanSession
    pkt[pos++] = keepalive >> 8;
    pkt[pos++] = keepalive & 0xFF;

    pkt[pos++] = 0x00; pkt[pos++] = strlen(client_id);
    memcpy(&pkt[pos], client_id, strlen(client_id));
    pos += strlen(client_id);

    send(sock, pkt, pos, 0);
}

void send_subscribe(int sock, const char *topic) {
    unsigned char pkt[BUF_SIZE];
    int pos = 0;
    pkt[pos++] = 0x82; // SUBSCRIBE
    int pkt_id = 1;
    int topic_len = strlen(topic);
    int remaining_len = 2 + 2 + topic_len + 1; // pkt_id + topic len + topic + QoS
    pkt[pos++] = remaining_len;

    pkt[pos++] = pkt_id >> 8;
    pkt[pos++] = pkt_id & 0xFF;

    pkt[pos++] = topic_len >> 8;
    pkt[pos++] = topic_len & 0xFF;
    memcpy(&pkt[pos], topic, topic_len); pos += topic_len;
    pkt[pos++] = 0x00; // QoS 0

    send(sock, pkt, pos, 0);
}

void send_ping(int sock) {
    unsigned char pkt[2] = {0xC0, 0x00}; // PINGREQ
    send(sock, pkt, 2, 0);
}

int main(int argc, char *argv[]) {
    if (argc != 2) {
        fprintf(stderr, "用法: %s <topic>\n", argv[0]);
        exit(1);
    }

    const char *topic = argv[1];

    int sock = socket(AF_INET, SOCK_STREAM, 0);
    struct sockaddr_in serv = {
        .sin_family = AF_INET,
        .sin_port = htons(1883),
        .sin_addr.s_addr = inet_addr("127.0.0.1")
    };
    connect(sock, (struct sockaddr*)&serv, sizeof(serv));

    send_connect(sock, "sub-client", 10);
    send_subscribe(sock, topic);

    printf("已订阅主题 [%s]，等待消息中...\n", topic);
    time_t last_ping = time(NULL);

    while (1) {
        fd_set fds;
        FD_ZERO(&fds);
        FD_SET(sock, &fds);
        struct timeval tv = {1, 0};
        int ret = select(sock + 1, &fds, NULL, NULL, &tv);

        if (ret > 0 && FD_ISSET(sock, &fds)) {
            unsigned char buf[BUF_SIZE];
            int len = recv(sock, buf, BUF_SIZE, 0);
            if (len <= 0) break;

            if ((buf[0] & 0xF0) == 0x30) { // PUBLISH
                int topic_len = (buf[2] << 8) | buf[3];
                char topic[64] = {0};
                memcpy(topic, &buf[4], topic_len);
                char *payload = (char*)&buf[4 + topic_len];
                int payload_len = len - 4 - topic_len;
                printf("收到 [%s]: %.*s\n", topic, payload_len, payload);
            }
        }

        if (time(NULL) - last_ping >= 5) {
            send_ping(sock);
            last_ping = time(NULL);
        }
    }

    close(sock);
    return 0;
}

