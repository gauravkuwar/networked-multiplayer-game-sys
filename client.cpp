#include <iostream>
#include <cstring>
#include <arpa/inet.h>
#include <unistd.h>
#include <queue>
#include <thread>
#include <chrono>
#include <cerrno> 
#include "states_generated.h"
#include "client.h"


Client::Client(int PORT) {
    sockfd = socket(AF_INET, SOCK_DGRAM, 0);
    if (sockfd == -1)
        std::cerr << "Error creating socket: " << strerror(errno) << std::endl;

    // client
    // memset(&myAddr, 0, sizeof(myAddr));
    myAddr.sin_family = AF_INET;
    myAddr.sin_port = htons(PORT);  // my port
    myAddr.sin_addr.s_addr = INADDR_ANY;
    
    // server
    serverAddr.sin_family = AF_INET;
    serverAddr.sin_port = htons(12345);  // server port
    inet_pton(AF_INET, "127.0.0.1", &serverAddr.sin_addr); // server IP address

    if (bind(sockfd, (struct sockaddr*)&myAddr, sizeof(myAddr)) == -1)
        std::cerr << "Error binding socket: " << strerror(errno) << std::endl;
    recvThread = std::thread(&Client::recv, this);
}

Client::~Client() {
    if (recvThread.joinable()) {
        recvThread.join();
    }
    close(sockfd);
}

void Client::recv() {
    while (true) {
        // Receive data
        // std::cout << "waiting to recv..." << std::endl;
        ssize_t bytesReceived = recvfrom(sockfd, buffer, sizeof(buffer), 0, nullptr, nullptr);
        if (bytesReceived == -1)
            std::cerr << "Error receiving data: " << strerror(errno) << std::endl;

        // deserialized data
        auto states = MyNamespace::GetStates(buffer);
        int pid = states->pid();
        float x = states->x();
        float y = states->y();
        float vx = states->vx();
        float vy = states->vy();

        // push to shared queue
        {
            std::lock_guard<std::mutex> lock(qMtx);
            recvQueue.push({static_cast<float>(pid), x, y, vx, vy});
        }
    }
}

void Client::send(int pid, float x, float y, float vx, float vy) {
    // serialized data
    auto states = MyNamespace::CreateStates(builder, pid, x, y, vy, vx);
    builder.Finish(states);

    const uint8_t* bufferData = builder.GetBufferPointer();
    size_t bufferSize = builder.GetSize();
    
    // Send the message to the server
    ssize_t bytesSent = sendto(sockfd, bufferData, bufferSize, 0,
                            (struct sockaddr*)&serverAddr, sizeof(serverAddr));

    if (bytesSent == -1)
        std::cerr << "Error sending data: " << strerror(errno) << std::endl;

    builder.Clear();
}

bool Client::isEmptyRecv() {
    return recvQueue.empty();
}

std::array<float, 5> Client::popRecv() {
    std::lock_guard<std::mutex> lock(qMtx);
    data = recvQueue.front();
    recvQueue.pop();
    return data;
}