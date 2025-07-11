#include <iostream>
#include <string>
#include <windows.h>

std::string GetOverlayPath() {
    HKEY hKey;
    const char* subKey = "SOFTWARE\\PandaDex\\TeamSpeakOverlay";
    const char* valueName = "installDir";
    char value[512];
    DWORD value_length = sizeof(value);

    if (RegOpenKeyExA(HKEY_CURRENT_USER, subKey, 0, KEY_READ, &hKey) != ERROR_SUCCESS) {
        return "";
    }

    if (RegQueryValueExA(hKey, valueName, NULL, NULL, (LPBYTE)&value, &value_length) != ERROR_SUCCESS) {
        RegCloseKey(hKey);
        return "";
    }

    RegCloseKey(hKey);
    return std::string(value);
}

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow) {
    OutputDebugStringA("Starting teamspeak executable\n");

    STARTUPINFO si = {sizeof(STARTUPINFO)};
    PROCESS_INFORMATION pi;
    CreateProcess(NULL, (LPSTR) "TeamSpeak.exe", NULL, NULL, FALSE, 0, NULL, NULL, &si, &pi);
    CloseHandle(pi.hThread);
    CloseHandle(pi.hProcess);

    Sleep(5000);

    std::string overlayProgramPath = GetOverlayPath();
    if (overlayProgramPath.empty()) {
        MessageBoxA(NULL, "Error: Could not read overlay path from registry", "Error", MB_OK);
        return 1;
    }

    OutputDebugStringA(("Starting overlay executable: " + overlayProgramPath + "\n").c_str());

    STARTUPINFO si2 = {sizeof(STARTUPINFO)};
    PROCESS_INFORMATION pi2;
    CreateProcess(NULL, (LPSTR) overlayProgramPath.c_str(), NULL, NULL, FALSE, 0, NULL, NULL, &si2, &pi2);
    CloseHandle(pi2.hThread);
    CloseHandle(pi2.hProcess);

    OutputDebugStringA("Done.\n");
    return 0;
}
