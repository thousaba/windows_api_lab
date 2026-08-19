#include <windows.h>
#include <stdio.h>

int main() {

    printf("Remote WriteProcessMemory Lab\n");
    printf("=============================\n");

    DWORD targetPid = 0;
    unsigned long long addressValue = 0;

    printf("Enter target process ID: ");
    if (scanf("%lu", &targetPid) != 1) {
        printf("Invalid PID.\n");
        return 1;
    }

    printf("Enter target memory address (hex): ");
    if (scanf("%llx", &addressValue) != 1) {
        printf("Invalid address.\n");
        return 1;
    }

    HANDLE hProcess = OpenProcess(
        PROCESS_VM_WRITE | PROCESS_VM_OPERATION,
        FALSE,
        targetPid
    );

    if (hProcess == NULL) {
        printf("[-] OpenProcess failed. Error: %lu\n", GetLastError());
        return 1;
    }

    printf("[+] Target PID     : %lu\n", targetPid);
    printf("[+] Process Handle : %p\n", hProcess);

    const char newData[] = "AFTER_WRITE";
    SIZE_T bytesWritten = 0;

    BOOL result = WriteProcessMemory(
        hProcess,
        (LPVOID)(ULONG_PTR)addressValue,
        newData,
        sizeof(newData),
        &bytesWritten
    );

    if (!result) {
        printf(
            "[-] WriteProcessMemory failed. Error: %lu\n",
            GetLastError()
        );

        CloseHandle(hProcess);
        return 1;
    }

    printf("[+] WriteProcessMemory succeeded!\n");
    printf("[+] Bytes written: %zu\n", bytesWritten);
    printf("[+] Data written : %s\n", newData);

    CloseHandle(hProcess);

    printf("[+] Process handle closed successfully.\n");

    return 0;
}