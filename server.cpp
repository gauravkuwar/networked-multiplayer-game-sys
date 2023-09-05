#include <iostream>
#include <arpa/inet.h>
#include <unistd.h>
#include <map>
#include <cstring>
#include <cerrno> 
#include <thread>
#include <chrono>
#include <queue>
#include "states_generated.h"

int LAG = 200; // ms 

class Server {
private:
    flatbuffers::FlatBufferBuilder builder;

    int sockfd;
    char buffer[1024];
    struct sockaddr_in serverAddr;
    std::map<int, struct sockaddr_in> clients = {
        {1, {
            .sin_family = AF_INET,
            .sin_port = htons(12346), // Port number
            .sin_addr.s_addr = inet_addr("127.0.0.1") // IP address
        }},
        {2, {
            .sin_family = AF_INET,
            .sin_port = htons(12347), // Port number
            .sin_addr.s_addr = inet_addr("127.0.0.1") // IP address
        }}
    };

    int tid = 0;
    long long initTime;
    std::chrono::system_clock::time_point now;
    std::queue<std::array<float, 6>> broadcastQueue;
    std::array<float, 6> data;

public:
    Server(int PORT) {
        sockfd = socket(AF_INET, SOCK_DGRAM, 0);
        if (sockfd == -1)
            std::cerr << "Error creating socket: " << strerror(errno) << std::endl;

        // memset(&serverAddr, 0, sizeof(serverAddr));
        serverAddr.sin_family = AF_INET;
        serverAddr.sin_addr.s_addr = INADDR_ANY;
        serverAddr.sin_port = htons(PORT);  // Replace with the port to listen on

        if (bind(sockfd, (struct sockaddr*)&serverAddr, sizeof(serverAddr)) == -1)
            std::cerr << "Error binding socket: " << strerror(errno) << std::endl;

        now = std::chrono::system_clock::now();
        auto duration = now.time_since_epoch();
        initTime = std::chrono::duration_cast<std::chrono::milliseconds>(duration).count();
    }

    ~Server() {
        close(sockfd);
    }

    void broadcast(int pid, float x, float y, float vx, float vy) {
        auto states = MyNamespace::CreateStates(builder, pid, x, y, vy, vx);
        builder.Finish(states);

        // serialized data
        const uint8_t* bufferData = builder.GetBufferPointer();
        size_t bufferSize = builder.GetSize();
        
        // send to all clients
        for (auto i : clients) {
            if (sendto(sockfd, bufferData, bufferSize, 0, (struct sockaddr*)&i.second, sizeof(i.second)) == -1)
                std::cerr << "Error sending data: " << strerror(errno) << std::endl;
        }

        builder.Clear();
    }

    void recv() {
        // Receive a message
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

        std::cout << tid << ": Recieved " << pid << ": pos(" << x << ", " << y;
        std::cout << "), vel(" << vx << ", " << vy << ")" << std::endl;
        tid += 1;

        // HERE CODE FOR VERFIYING ACTIONS


        // add artificial lag by using a queue
        // push send items to queue to be broadcasted after a certain lag
        now = std::chrono::system_clock::now();
        auto duration = now.time_since_epoch();
        long long timeNow = std::chrono::duration_cast<std::chrono::milliseconds>(duration).count();
        int timeDiff = timeNow - initTime;
        int broadcastTime = timeDiff + LAG;

        broadcastQueue.push({static_cast<float>(broadcastTime), static_cast<float>(pid), x, y, vx, vy});
        while (static_cast<int>(broadcastQueue.front()[0]) <= timeDiff) {
            data = broadcastQueue.front();
            broadcastQueue.pop();
            this->broadcast(static_cast<int>(data[1]), data[2], data[3], data[4], data[5]);
        }
    }

    void run() {
        while (true) {
            this->recv();
        }
    }
};

int main() {
    Server server(12345);
    server.run();
    return 0;
}
