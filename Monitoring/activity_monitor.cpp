#include "activity_monitor.h"
#include "network_utils.h"
#include <iostream>
#include <windows.h>
#include <thread>


std::string getLastActivityTime() {
	LASTINPUTINFO lastInputInfo;
	lastInputInfo.cbSize = sizeof(LASTINPUTINFO);

	if (!GetLastInputInfo(&lastInputInfo)) {
		return "Unknown";
	}

	DWORD currentTime = GetTickCount();
	DWORD idleTime = (currentTime - lastInputInfo.dwTime) / 1000;

	return std::to_string(idleTime) + " seconds ago";
}

void monitorActivity(SOCKET& sock) {
	std::string lastSentActivity = getLastActivityTime();

	while (true) { 
		std::string currentActivity = getLastActivityTime();
		if (currentActivity != lastSentActivity) {
			std::string updatedInfo = "ACTIVITY:Last Activity:" + currentActivity;
			sendData(sock, updatedInfo);
			lastSentActivity = currentActivity;
			std::this_thread::sleep_for(std::chrono::seconds(10));
		}
	}
}
