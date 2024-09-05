#include "screenshot.h"
#include "network_utils.h"
#include <iostream>
#include <windows.h>
#include <vector>
#include <string>

void captureScreenshotAndSend(SOCKET& sock) {
	RECT desktop;
	const HWND hDesktop = GetDesktopWindow();
	GetWindowRect(hDesktop, &desktop);
	int width = desktop.right;
	int height = desktop.bottom;

	HDC hScreenDC = GetDC(NULL);
	HDC hMemoryDC = CreateCompatibleDC(hScreenDC);

	HBITMAP hBitmap = CreateCompatibleBitmap(hScreenDC, width, height);
	HBITMAP hOldBitmap = (HBITMAP)SelectObject(hMemoryDC, hBitmap);

	BitBlt(hMemoryDC, 0, 0, width, height, hScreenDC, 0, 0, SRCCOPY);

	BITMAPINFO bitmapInfo;
	ZeroMemory(&bitmapInfo, sizeof(BITMAPINFO));
	bitmapInfo.bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
	bitmapInfo.bmiHeader.biWidth = width;
	bitmapInfo.bmiHeader.biHeight = -height;
	bitmapInfo.bmiHeader.biPlanes = 1;
	bitmapInfo.bmiHeader.biBitCount = 24;
	bitmapInfo.bmiHeader.biCompression = BI_RGB;

	std::vector<BYTE> bitmapData(width * height * 4);
	GetDIBits(hMemoryDC, hBitmap, 0, height, bitmapData.data(), &bitmapInfo, DIB_RGB_COLORS);

	std::string dimensions = std::to_string(width) + "," + std::to_string(height);
	sendData(sock, dimensions);

	if (send(sock, reinterpret_cast<char*>(bitmapData.data()), bitmapData.size(), 0) == SOCKET_ERROR) {
		std::cerr << "Send failed: " << WSAGetLastError() << std::endl;
	}
	std::cout << "data send" << std::endl;

	SelectObject(hMemoryDC, hOldBitmap);
	DeleteObject(hBitmap);
	DeleteDC(hMemoryDC);
	ReleaseDC(NULL, hScreenDC);
}
