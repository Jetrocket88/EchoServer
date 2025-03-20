#include <WinSock2.h>
#include <iostream>
#include <fstream>


#define PORT 8080
#define BUFFER_SIZE 1024

int main() {
    // Initialize Winsock
    WSADATA wsaData;
    SOCKET serverSocket, clientSocket;
    struct sockaddr_in serverAddress, clientAddress;
    char buffer[BUFFER_SIZE] = {};

    if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0) {
        std::cerr << "WSAStartup failed!" << std::endl;
        return 1;
    }

    // Create socket
    serverSocket = socket(AF_INET, SOCK_STREAM, 0);
    if (serverSocket == INVALID_SOCKET) {
        std::cerr << "Socket creation failed: " << WSAGetLastError() << std::endl;
        WSACleanup();
        return 1;
    }

    // Bind address
    serverAddress.sin_family = AF_INET;
    serverAddress.sin_addr.s_addr = INADDR_ANY;
    serverAddress.sin_port = htons(8080);

    if (bind(serverSocket, (struct sockaddr*)&serverAddress, sizeof(serverAddress)) == SOCKET_ERROR) {
        std::cerr << "Bind failed: " << WSAGetLastError() << std::endl;
        closesocket(serverSocket);
        WSACleanup();
        return 1;
    }

    // Listen for connections
    if (listen(serverSocket, 5) == SOCKET_ERROR) {
        std::cerr << "Listen failed: " << WSAGetLastError() << std::endl;
        closesocket(serverSocket);
        WSACleanup();
        return 1;
    }

    std::cout << "Waiting for client..." << std::endl;


    int clientAddressSize = sizeof(clientAddress);
    clientSocket = accept(serverSocket, (struct sockaddr*)&clientAddress, &clientAddressSize);
    if (clientSocket == INVALID_SOCKET) {
        std::cerr << "Accept failed: " << WSAGetLastError() << std::endl;
        closesocket(serverSocket);
        WSACleanup();
        return 1;
    }

    std::ofstream messageFile("../messages.txt", std::ios::app);
    while (true) {
            memset(buffer, 0, BUFFER_SIZE); //clear the buffer
            // Read message from client
            int bytes_read = recv(clientSocket, buffer, BUFFER_SIZE, 0);
            if (bytes_read <= 0) {
                std::cout << "Client disconnected.\n";
                break;
            }

            // Echo message back to client
            send(clientSocket, buffer, bytes_read, 0);
            std::cout << "Received: " << buffer << std::endl;
            std::cout << "Sucessfully echoed" << std::endl;

            messageFile << buffer << std::endl;
    }
    messageFile.close();

    // Cleanup
    closesocket(clientSocket);
    closesocket(serverSocket);
    WSACleanup();

    std::cout << "Press Enter to exit...\n";
    std::cin.get(); // Keep server open

    return 0;
}
