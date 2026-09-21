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

    HANDLE hThread = OpenThread(THREAD_SUSPEND_RESUME, FALSE, targetTid);
    if (hThread == NULL) {
        printf("[-] OpenThread failed. Error: %lu\n", GetLastError());
        return 1;
    }
    printf("[+] OpenThread Successful! Handle: 0x%p\n", hThread);

    DWORD suspendCount = SuspendThread(hThread);
    if (suspendCount == (DWORD)-1) {
        printf("[-] SuspendThread failed. Error: %lu\n", GetLastError());
        CloseHandle(hThread);
        return 1;
    }
    printf("[+] Thread SUSPENDED. Previous suspend count: %lu\n", suspendCount);

    printf("[*] Check System Informer now - thread state should show SUSPENDED.\n");
    printf("[*] Press ENTER to RESUME the thread...\n");
    getchar();

    DWORD resumeCount = ResumeThread(hThread);
    if (resumeCount == (DWORD)-1) {
        printf("[-] ResumeThread failed. Error: %lu\n", GetLastError());
    } else {
        printf("[+] Thread RESUMED. Suspend count before resume: %lu\n", resumeCount);
    }

    CloseHandle(hThread);
    printf("[*] Press ENTER to exit...");
    getchar();
    return 0;
}