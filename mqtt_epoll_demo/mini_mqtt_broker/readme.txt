gcc -o mini_mqtt_broker mini_mqtt_broker.c
./mini_mqtt_broker
mosquitto_sub -t test
mosquitto_pub -t test -m "Hello MQTT!"
https://chatgpt.com/c/681d7288-97e4-800c-a826-f293a190ea77
