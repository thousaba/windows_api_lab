#include <windows.h>
#include <stdio.h>

int main(void)
{
    STARTUPINFOA si = {0};
    PROCESS_INFORMATION pi = {0};

    si.cb = sizeof(si);

    char cmdLine[] =
        "notepad.exe C:\\temp_test\\createprocess-test.txt";

    BOOL result = CreateProcessA(
        "C:\\Windows\\System32\\notepad.exe",  // lpApplicationName
        cmdLine,                               // lpCommandLine
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
        printf("CreateProcessA failed: %lu\n", GetLastError());
        return 1;
    }

    printf("[+] Process created\n");
    printf("[+] PID: %lu\n", pi.dwProcessId);
    printf("[+] TID: %lu\n", pi.dwThreadId);
    printf("[+] Command line: %s\n", cmdLine);

    CloseHandle(pi.hThread);
    CloseHandle(pi.hProcess);

    return 0;
}