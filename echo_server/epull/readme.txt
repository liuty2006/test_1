1.compile
gcc -o epoll_echo_server epoll_echo_server.c

2 run
./epoll_echo_server

3 
telnet 127.0.0.1 8888
# 或者
nc 127.0.0.1 8888

