#include <windows.h>
#include <stdio.h>

int main() {

    printf("VirtualAllocEx Lab\n");
    printf("===================\n");

    DWORD targetPid = 0;

    printf("Enter target process ID: ");

    if (scanf("%lu", &targetPid) != 1) {
        printf("Invalid PID.\n");
        return 1;
    }

    HANDLE hProcess = OpenProcess(
        PROCESS_VM_OPERATION,
        FALSE,
        targetPid
    );

    if (hProcess == NULL) {
        printf(
            "[-] OpenProcess failed. Error: %lu\n",
            GetLastError()
        );
        return 1;
    }

    printf("[+] Target PID     : %lu\n", targetPid);
    printf("[+] Process Handle : %p\n", hProcess);

    SIZE_T regionSize = 4096;

    LPVOID allocatedAddress = VirtualAllocEx(
        hProcess,
        NULL,
        regionSize,
        MEM_RESERVE | MEM_COMMIT,
        PAGE_READWRITE
    );

    if (allocatedAddress == NULL) {
        printf(
            "[-] VirtualAllocEx failed. Error: %lu\n",
            GetLastError()
        );

        CloseHandle(hProcess);
        return 1;
    }

    printf("[+] Allocation successful!\n");
    printf("[+] Base Address : %p\n", allocatedAddress);
    printf("[+] Region Size   : %zu bytes\n", regionSize);

    CloseHandle(hProcess);

    printf("[+] Process handle closed successfully.\n");

    return 0;
}