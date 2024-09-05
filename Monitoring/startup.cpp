#include "startup.h"
#include <iostream>
#include <windows.h>
#include <string>

void addToStartup() {
    HKEY hKey;
    wchar_t czExePath[MAX_PATH];
    GetModuleFileNameW(NULL, czExePath, MAX_PATH);

    if (RegOpenKeyExW(HKEY_CURRENT_USER, L"Software\\Microsoft\\Windows\\CurrentVersion\\Run", 0, KEY_READ | KEY_SET_VALUE, &hKey) == ERROR_SUCCESS) {
        wchar_t existingValue[MAX_PATH];
        DWORD size = sizeof(existingValue);
        LONG result = RegQueryValueExW(hKey, L"Monitoring", NULL, NULL, (BYTE*)existingValue, &size);

        if (result == ERROR_SUCCESS) {
            if (wcscmp(existingValue, czExePath) == 0) {
                RegCloseKey(hKey);
                return;
            }
        }

        DWORD pathLength = (wcslen(czExePath) + 1) * sizeof(wchar_t);
        RegSetValueExW(hKey, L"Monitoring", 0, REG_SZ, (const BYTE*)czExePath, pathLength);
        RegCloseKey(hKey);
    }
}
