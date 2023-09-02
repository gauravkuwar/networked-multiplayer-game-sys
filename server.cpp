#include <iostream>
#include <arpa/inet.h>
#include <unistd.h>
#include <map>
#include <cstring>
#include <cerrno> 
#include <thread>
#include <chrono>
#include "moves_generated.h"

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
    }

    ~Server() {
        close(sockfd);
    }

    void broadcast(int pid, int dx, int dy) {
        auto mv = MyNamespace::CreateMoves(builder, pid, dx, dy);
        builder.Finish(mv);

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
        auto mv = MyNamespace::GetMoves(buffer);
        auto pid = mv->pid();
        auto dx = mv->dx();
        auto dy = mv->dy();

        std::cout << tid << ": Recieved " << pid << " : " << "(" << dx << ", " << dy << ")" << std::endl;
        tid += 1;
        this->broadcast(pid, dx, dy);    
    }
};

int main() {
    Server server(12345);
    while (true) {
        server.recv();
    }
    return 0;
}
