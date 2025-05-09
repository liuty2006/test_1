#include <stdio.h>
//#include <unistd.h>
#include <stdlib.h>
#include <string.h>
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

        // 发布一条消息
        const char *message = "Hello, MQTT from Publisher!";
        mosquitto_publish(mosq, NULL, MQTT_TOPIC, strlen(message), message, 1, false);
    } else {
        fprintf(stderr, "Connection to MQTT broker failed\n");
        exit(EXIT_FAILURE);
    }
}

int main() {
    mosquitto_lib_init();

    mosq = mosquitto_new(NULL, true, NULL);
    if (!mosq) {
        fprintf(stderr, "Error: Unable to initialize Mosquitto library\n");
        return EXIT_FAILURE;
    }

    mosquitto_connect_callback_set(mosq, on_connect);

    if (mosquitto_connect(mosq, MQTT_BROKER_ADDRESS, MQTT_BROKER_PORT, 60) != MOSQ_ERR_SUCCESS) {
        fprintf(stderr, "Unable to connect to MQTT broker\n");
        return EXIT_FAILURE;
    }

    // 开始处理 MQTT 事件循环
    mosquitto_loop_start(mosq);

    // 等待发布完成

    #ifdef _WIN32
        Sleep(2);
#else
        sleep(2);
#endif
    //sleep(2);

    // 清理资源
    mosquitto_disconnect(mosq);
    mosquitto_destroy(mosq);
    mosquitto_lib_cleanup();

    return 0;
}

