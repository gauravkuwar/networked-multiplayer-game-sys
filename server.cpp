#include <iostream>
#include <arpa/inet.h>
#include <unistd.h>
#include <map>
#include <cstring>
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

        memset(&serverAddr, 0, sizeof(serverAddr));
        serverAddr.sin_family = AF_INET;
        serverAddr.sin_addr.s_addr = INADDR_ANY;
        serverAddr.sin_port = htons(PORT);  // Replace with the port to listen on

        bind(sockfd, (struct sockaddr*)&serverAddr, sizeof(serverAddr));
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
            sendto(sockfd, bufferData, bufferSize, 0, (struct sockaddr*)&i.second, sizeof(i.second));
        }
    }

    void recv() {
        // Receive a message
        // std::cout << "waiting to recv... " << std::endl;
        std::cout << "buffer:" << buffer[0] << std::endl;
        ssize_t bytesReceived = recvfrom(sockfd, buffer, sizeof(buffer), 0, nullptr, nullptr);

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
