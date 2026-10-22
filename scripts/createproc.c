#include <windows.h>
#include <stdio.h>

int main(void)
{
    STARTUPINFOA si = {0};
    PROCESS_INFORMATION pi = {0};

    si.cb = sizeof(si);

    BOOL result = CreateProcessA(
        "C:\\Windows\\System32\\notepad.exe",
        NULL,
        NULL,
        NULL,
        FALSE,
        0,
        NULL,
        NULL,
        &si,
        &pi
    );

    if (!result) {
        printf("CreateProcess failed: %lu\n", GetLastError());
        return 1;
    }

    printf("Process created!\n");
    printf("PID: %lu\n", pi.dwProcessId);
    printf("TID: %lu\n", pi.dwThreadId);

    CloseHandle(pi.hThread);
    CloseHandle(pi.hProcess);

    return 0;
}