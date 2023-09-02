#include <iostream>
#include <cstring>
#include <arpa/inet.h>
#include <unistd.h>
#include <queue>
#include <thread>
#include <chrono>
#include "moves_generated.h"
#include "client.h"

void Client::recv() {
    while (true) {
        // Receive data
        ssize_t bytesReceived = recvfrom(sockfd, buffer, sizeof(buffer), 0, nullptr, nullptr);

        // deserialized data
        auto mv = MyNamespace::GetMoves(buffer);
        auto pid = mv->pid();
        auto dx = mv->dx();
        auto dy = mv->dy();

        // push to shared queue
        {
            std::lock_guard<std::mutex> lock(qMtx);
            recvQueue.push({pid, dx, dy});
        }
    }
}

Client::Client(int PORT) {
    sockfd = socket(AF_INET, SOCK_DGRAM, 0);

    // client
    memset(&serverAddr, 0, sizeof(serverAddr));
    myAddr.sin_family = AF_INET;
    myAddr.sin_port = htons(PORT);  // my port
    myAddr.sin_addr.s_addr = INADDR_ANY;
    
    // server
    serverAddr.sin_family = AF_INET;
    serverAddr.sin_port = htons(12345);  // server port
    inet_pton(AF_INET, "127.0.0.1", &serverAddr.sin_addr); // server IP address

    bind(sockfd, (struct sockaddr*)&myAddr, sizeof(myAddr));
    recvThread = std::thread(&Client::recv, this);
}

Client::~Client() {
    if (recvThread.joinable()) {
        recvThread.join();
    }
    close(sockfd);
}

void Client::send(int pid, int dx, int dy) {
    // serialized data
    auto mv = MyNamespace::CreateMoves(builder, pid, dx, dy);
    builder.Finish(mv);
    const uint8_t* bufferData = builder.GetBufferPointer();
    size_t bufferSize = builder.GetSize();

    // Send the message to the server
    ssize_t bytesSent = sendto(sockfd, bufferData, bufferSize, 0,
                            (struct sockaddr*)&serverAddr, sizeof(serverAddr));
}

bool Client::isEmptyRecv() {
    return recvQueue.empty();
}

std::array<int, 3> Client::popRecv() {
    std::lock_guard<std::mutex> lock(qMtx);
    data = recvQueue.front();
    recvQueue.pop();
    return data;
}


// int main() {
//     Client client(12346);
//     // client.send(1,5,0);
//     // std::this_thread::sleep_for(std::chrono::seconds(2));
//     // client.getQueue();

//     int pid, dx, dy;
//     while (true) {
//         std::cout << "Input: ";
//         std::cin >> pid;
//         std::cin >> dx;
//         std::cin >> dy;

//         if (pid == 10) {
    
//             // Retrieve and print all items from the queue until it's empty
//             while (!client.isEmptyRecv()) {
//                 std::array<int, 3> data = client.popRecv();
//                 std::cout << "{" << data[0] << "," << data[1] << "," << data[2] << "}, ";
//             }

//             std::cout << std::endl;
//         } else {
//             // std::cout << "You entered: " << pid << ", " << dx << ", " << dy << std::endl;
//             client.send(pid, dx, dy);
//         }
//     }

//     return 0;
// }
