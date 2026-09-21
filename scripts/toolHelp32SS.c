#include <stdio.h>
#include <windows.h>
#include <tlhelp32.h>

int main(void) {
    DWORD targetPid = 0;
    printf("[*] Enter target PID: ");
    if (scanf("%lu", &targetPid) != 1) {
        printf("[-] Invalid input.\n");
        return 1;
    }

    HANDLE hSnapshot = CreateToolhelp32Snapshot(TH32CS_SNAPTHREAD, 0);
    if (hSnapshot == INVALID_HANDLE_VALUE) {
        printf("[-] CreateToolhelp32Snapshot failed. Error: %lu\n", GetLastError());
        return 1;
    }

    THREADENTRY32 te;
    te.dwSize = sizeof(THREADENTRY32);

    if (!Thread32First(hSnapshot, &te)) {
        printf("[-] Thread32First failed. Error: %lu\n", GetLastError());
        CloseHandle(hSnapshot);
        return 1;
    }

    int count = 0;
    do {
        if (te.th32OwnerProcessID == targetPid) {
            printf("[+] Thread found -> TID: %-8lu Base Priority: %ld\n",
                   te.th32ThreadID, te.tpBasePri);
            count++;
        }
    } while (Thread32Next(hSnapshot, &te));

    printf("[*] Total threads belonging to PID %lu: %d\n", targetPid, count);

    CloseHandle(hSnapshot);
    printf("[*] Press ENTER to exit...");
    getchar();
    return 0;
}