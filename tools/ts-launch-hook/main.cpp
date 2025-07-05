#include <iostream>
#include <fstream>
#include <string>
#include <thread>
#include <windows.h>

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow) {
    OutputDebugStringA("Starting teamspeak executable\n");

    STARTUPINFO si = {sizeof(STARTUPINFO)};
    PROCESS_INFORMATION pi;
    CreateProcess(NULL, (LPSTR) "TeamSpeak.exe", NULL, NULL, FALSE, 0, NULL, NULL, &si, &pi);
    CloseHandle(pi.hThread);
    CloseHandle(pi.hProcess);

    Sleep(5000);

    std::ifstream file("overlay_path.txt");
    if (!file) {
        MessageBoxA(NULL, "Error: Could not open overlay_path.txt", "Error", MB_OK);
        return 1;
    }

    std::string overlayProgramPath;
    std::getline(file, overlayProgramPath);
    file.close();

    if (overlayProgramPath.empty()) {
        MessageBoxA(NULL, "Error: overlay_path.txt is empty", "Error", MB_OK);
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
