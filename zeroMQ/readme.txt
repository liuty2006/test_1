sudo apt-get update
sudo apt-get install libzmq3-dev

g++ server.cpp -o server -lzmq
g++ client.cpp -o client -lzmq

