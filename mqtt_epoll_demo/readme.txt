gcc -o epoll_broker epoll_broker.c
./epoll_broker
mosquitto_sub -h 127.0.0.1 -p 1883 -t "test"
