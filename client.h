#ifndef CLIENT_H
#define CLIENT_H

#include <queue>
#include <thread>
#include <arpa/inet.h>
#include "states_generated.h"

class Client {
private:
    int sockfd;
    struct sockaddr_in myAddr;
    struct sockaddr_in serverAddr;
    char buffer[1024];
    flatbuffers::FlatBufferBuilder builder;
    std::queue<std::array<float, 5>> recvQueue;
    std::array<float, 5> data;
    std::mutex qMtx;
    std::thread recvThread;
    void recv();

public:
    Client(int PORT);
    ~Client();
    void send(int pid, float x, float y, float vx, float vy);
    bool isEmptyRecv();
    std::array<float, 5> popRecv();
};

#endif