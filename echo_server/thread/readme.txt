gcc -o echo_server_threaded echo_server_threaded.c -pthread
./echo_server_threaded
telnet 127.0.0.1 8888
