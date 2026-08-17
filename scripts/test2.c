#include <windows.h>
#include <stdio.h>

int main() {
    DWORD currentPid = GetCurrentProcessId();
    printf("Current Process ID: %lu\n", currentPid);

    HANDLE hProcess = GetCurrentProcess();
    printf("Current Process Handle: %p\n", hProcess);

    DWORD resolvedPid = GetProcessId(hProcess);
    printf("Resolved Process ID from Handle: %lu\n", resolvedPid);

    if (!CloseHandle(hProcess)) {
        printf("Failed to close process handle. Error: %lu\n", GetLastError());
    } else {
        printf("Process handle closed successfully.\n");
    }

    return 0;
}