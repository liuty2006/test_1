以下是一个完整的最小 MQTT Broker（支持 epoll）框架，实现以下功能：

✅ MQTT CONNECT 报文解析
✅ QoS=0 发布 / 订阅（仅支持一个主题）
✅ epoll EPOLLOUT 写入支持
✅ 心跳检测（keepalive 超时断开）

1.编译
gcc -o mini_mqtt_broker mini_mqtt_broker.c

2.运行
./mini_mqtt_broker

3.测试
mosquitto_sub -t test
mosquitto_pub -t test -m "Hello MQTT!"

4.
https://chatgpt.com/c/681d7288-97e4-800c-a826-f293a190ea77
