#ifndef CLIENT_H
#define CLIENT_H

#include <queue>
#include <thread>
#include <arpa/inet.h>
#include "moves_generated.h"

class Client {
private:
    int sockfd;
    struct sockaddr_in myAddr;
    struct sockaddr_in serverAddr;
    char buffer[1024];
    flatbuffers::FlatBufferBuilder builder;
    std::queue<std::array<int, 3>> recvQueue;
    std::array<int, 3> data;
    std::mutex qMtx;
    std::thread recvThread;
    void recv();

public:
    Client(int PORT);
    ~Client();
    void send(int pid, int dx, int dy);
    bool isEmptyRecv();
    std::array<int, 3> popRecv();
};

#endif