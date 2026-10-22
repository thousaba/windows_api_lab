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
        CREATE_SUSPENDED,
        NULL,
        NULL,
        &si,
        &pi
    );

    if (!result) {
        printf("CreateProcessA failed: %lu\n", GetLastError());
        return 1;
    }

    printf("[+] Process created in suspended state\n");
    printf("[+] PID: %lu\n", pi.dwProcessId);
    printf("[+] Initial TID: %lu\n", pi.dwThreadId);
    printf("[+] Process HANDLE: %p\n", pi.hProcess);
    printf("[+] Thread HANDLE:  %p\n", pi.hThread);

    printf("\nPress Enter to resume the initial thread...\n");
    getchar();

    DWORD previousCount = ResumeThread(pi.hThread);

    if (previousCount == (DWORD)-1) {
        printf("ResumeThread failed: %lu\n", GetLastError());

        CloseHandle(pi.hThread);
        CloseHandle(pi.hProcess);
        return 1;
    }

    printf("[+] ResumeThread succeeded\n");
    printf("[+] Previous suspend count: %lu\n", previousCount);

    CloseHandle(pi.hThread);
    CloseHandle(pi.hProcess);

    return 0;
}