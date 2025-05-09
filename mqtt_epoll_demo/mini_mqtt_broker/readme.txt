gcc -o mini_mqtt_broker mini_mqtt_broker.c
./mini_mqtt_broker
mosquitto_sub -t test
mosquitto_pub -t test -m "Hello MQTT!"
