#include <windows.h>
#include <stdio.h>

int main() {

    printf("VirtualAllocEx + WriteProcessMemory Lab\n");
    printf("=======================================\n");

    DWORD targetPid = 0;

    printf("Enter target process ID: ");

    if (scanf("%lu", &targetPid) != 1) {
        printf("Invalid PID.\n");
        return 1;
    }

    // CRITICAL FIX: PROCESS_VM_WRITE yetkisi eklendi
    HANDLE hProcess = OpenProcess(
        PROCESS_VM_OPERATION | PROCESS_VM_WRITE,
        FALSE,
        targetPid
    );

    if (hProcess == NULL) {
        printf("[-] OpenProcess failed. Error: %lu\n", GetLastError());
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
        printf("[-] VirtualAllocEx failed. Error: %lu\n", GetLastError());
        CloseHandle(hProcess);
        return 1;
    }

    printf("[+] Allocation successful!\n");
    printf("[+] Base Address : %p\n", allocatedAddress);
    printf("[+] Region Size   : %zu bytes\n", regionSize);

    // Kodu/Veriyi yazma aşaması
    const char data[] = "Hello, Notepad!";
    SIZE_T bytesWritten = 0;

    BOOL writeResult = WriteProcessMemory(
        hProcess,
        allocatedAddress,
        data,
        sizeof(data), // Null-terminator dahil kopyalamak icin sizeof
        &bytesWritten
    );

    if (!writeResult) {
        printf("[-] WriteProcessMemory failed. Error: %lu\n", GetLastError());
    } else {
        printf("[+] WriteProcessMemory successful!\n");
        printf("[+] Bytes Written : %zu\n", bytesWritten);
    }

    CloseHandle(hProcess);
    printf("[+] Process handle closed successfully.\n");

    return 0;
}