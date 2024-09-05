#include "system_info.h"
#include <iostream>
#include <winsock2.h>
#include <windows.h>
#include <ws2tcpip.h>
#include <Lm.h>
#include <strsafe.h>

std::string getDomainName() {
	LPWKSTA_INFO_100 pBuf = NULL;
	std::string domain = "Unknow";
	if (NetWkstaGetInfo(NULL, 100, (LPBYTE*)&pBuf) == NERR_Success && pBuf != NULL) {
		wchar_t domainName[UNLEN + 1];
		HRESULT hr = StringCchCopy(domainName, UNLEN + 1, pBuf->wki100_langroup);
		if (SUCCEEDED(hr)) {
			int bufferSize = WideCharToMultiByte(CP_UTF8, 0, domainName, -1, NULL, 0, NULL, NULL);
			domain.resize(bufferSize - 1);
			WideCharToMultiByte(CP_UTF8, 0, domainName, -1, &domain[0], bufferSize, NULL, NULL);
		}
		NetApiBufferFree(pBuf);
	}
	return domain;
}

std::string getIpAddress() {
	char hostname[UNLEN];
	if (gethostname(hostname, sizeof(hostname)) == SOCKET_ERROR) {
		return "Unknow";
	}

	addrinfo hints = { 0 }, * res = NULL;
	hints.ai_family = AF_INET;
	hints.ai_socktype = SOCK_STREAM;
	if (getaddrinfo(hostname, NULL, &hints, &res) != 0) {
		return "Unknow";
	}

	char ip[INET_ADDRSTRLEN];
	sockaddr_in* addr = (sockaddr_in*)res->ai_addr;
	inet_ntop(AF_INET, &(addr->sin_addr), ip, INET_ADDRSTRLEN);
	freeaddrinfo(res);

	return std::string(ip);
}

std::string getUserInfo() {
    char username[UNLEN + 1];
    DWORD username_len = UNLEN + 1;
    GetUserNameA(username, &username_len);

    char computerName[UNLEN + 1];
    DWORD computerName_len = UNLEN + 1;
    GetComputerNameA(computerName, &computerName_len);

    std::string domainName = getDomainName();
    std::string ipAddress = getIpAddress();

    return "INFO:Domain: " + domainName + ", Machine: " + std::string(computerName) + ", IP: " + ipAddress + ", User: " + std::string(username);
}
