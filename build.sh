
g++ -c dht22.cpp
g++ -c threads.cpp
g++ -lpthread -lwiringPi *.o main.cpp -o dht22
