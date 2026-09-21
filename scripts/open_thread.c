#include <stdio.h>
#include <windows.h>

int main(void) {
    DWORD targetTid = 0;
    printf("[*] Enter target TID: ");
    if (scanf("%lu", &targetTid) != 1) {
        printf("[-] Invalid input.\n");
        return 1;
    }

    while (getchar() != '\n'); 

    HANDLE hThread = OpenThread(
        THREAD_ALL_ACCESS,
        FALSE,
        targetTid
    );

    if (hThread == NULL) {
        printf("[-] OpenThread failed. Error: %lu\n", GetLastError());
        return 1;
    }

    printf("[+] OpenThread Successful! Handle: 0x%p\n", hThread);

    DWORD ownerPid = GetProcessIdOfThread(hThread);
    printf("[+] This thread belongs to PID: %lu\n", ownerPid);

    printf("[*] PAUSED - check System Informer now, then press ENTER to close the handle...\n");
    getchar();

    CloseHandle(hThread);
    printf("[*] Press ENTER to exit...");
    getchar();
    return 0;
}