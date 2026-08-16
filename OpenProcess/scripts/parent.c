#include <stdio.h>
#include <windows.h>

int main() {
    printf("[PARENT] Lab 1H: Handle Inheritance Test\n");
    printf("========================================\n");

    DWORD targetPid = 0;
    printf("[PARENT] Target Process PID : ");
    if (scanf("%lu", &targetPid) != 1) return 1;

    HANDLE hProcess = OpenProcess(PROCESS_QUERY_LIMITED_INFORMATION, TRUE, /*FALSE*/ targetPid);
    if (hProcess == NULL) {
        printf("[PARENT] OpenProcess failed: %lu\n", GetLastError());
        return 1;
    }

    printf("[+] [PARENT] Opened Handle: %p (PID: %lu)\n", hProcess, targetPid);

    char cmdLine[256];
    sprintf(cmdLine, "child.exe %p", hProcess);

    STARTUPINFOA si;
    PROCESS_INFORMATION pi;
    ZeroMemory(&si, sizeof(si));
    si.cb = sizeof(si);
    ZeroMemory(&pi, sizeof(pi));

    printf("[PARENT] Child process is starting...\n");

    BOOL success = CreateProcessA(
        NULL,           // Application Name
        cmdLine,        // Command Line
        NULL,           // Process Attributes
        NULL,           // Thread Attributes
        TRUE, //FALSE   // bInheritHandles 
        0,              // Creation Flags
        NULL,           // Environment
        NULL,           // Current Directory
        &si,            // Startup Info
        &pi             // Process Info
    );

    if (!success) {
        printf("[PARENT] CreateProcess failed: %lu\n", GetLastError());
        CloseHandle(hProcess);
        return 1;
    }

    WaitForSingleObject(pi.hProcess, INFINITE);

    CloseHandle(pi.hProcess);
    CloseHandle(pi.hThread);
    CloseHandle(hProcess);

    printf("\n[PARENT] Completed. Shutting down.\n");
    return 0;
}