#include <stdio.h>
//#include <unistd.h>
#include <stdlib.h>
#include <mosquitto.h>

#ifdef _WIN32
#include <windows.h>
#else
#include <unistd.h>
#endif


const char *MQTT_BROKER_ADDRESS = "localhost";
const int MQTT_BROKER_PORT = 1883;
const char *MQTT_TOPIC = "demo/topic";

struct mosquitto *mosq = NULL;

void on_connect(struct mosquitto *mosq, void *userdata, int result) {
    if (result == 0) {
        printf("Connected to MQTT broker\n");

        // 订阅主题
        mosquitto_subscribe(mosq, NULL, MQTT_TOPIC, 1);
    } else {
        fprintf(stderr, "Connection to MQTT broker failed\n");
        exit(EXIT_FAILURE);
    }
}

void on_message(struct mosquitto *mosq, void *userdata, const struct mosquitto_message *message) {
    printf("Received message: %.*s\n", message->payloadlen, (char *)message->payload);

    // 订阅到消息后，可以在这里处理接收到的数据
}

int main() {
    mosquitto_lib_init();

    mosq = mosquitto_new(NULL, true, NULL);
    if (!mosq) {
        fprintf(stderr, "Error: Unable to initialize Mosquitto library\n");
        return EXIT_FAILURE;
    }

    mosquitto_connect_callback_set(mosq, on_connect);
    mosquitto_message_callback_set(mosq, on_message);

    if (mosquitto_connect(mosq, MQTT_BROKER_ADDRESS, MQTT_BROKER_PORT, 60) != MOSQ_ERR_SUCCESS) {
        fprintf(stderr, "Unable to connect to MQTT broker\n");
        return EXIT_FAILURE;
    }

    // 开始处理 MQTT 事件循环
    mosquitto_loop_start(mosq);

    // 保持订阅者运行
    while (1) {
#ifdef _WIN32
        Sleep(1);
#else
        sleep(1);
#endif

        //sleep(1);
    }

    // 清理资源
    mosquitto_disconnect(mosq);
    mosquitto_destroy(mosq);
    mosquitto_lib_cleanup();

    return 0;
}

