#ifndef NETWORK_UTILS_H
#define NETWORK_UTILS_H

#include <winsock2.h>
#include <string>

void initializeWinSock();
SOCKET createSocket();
void connectToServer(SOCKET& sock);
void sendData(SOCKET& sock, const std::string& data);
void cleanup(SOCKET& sock);
void handleConnection(SOCKET& sock);

#endif
