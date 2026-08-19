#include <windows.h>
#include <stdio.h>

int main() {

    printf("VirtualProtectEx Lab\n");
    printf("====================\n");

    DWORD targetPid = 0;

    printf("Enter target process ID: ");
    if (scanf("%lu", &targetPid) != 1) {
        printf("Invalid PID.\n");
        return 1;
    }

    // PROCESS_VM_OPERATION şarttır! VirtualProtectEx bellek yapısını değiştirir.
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

    // 1. Adım: RW olarak bellek ayır
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

    printf("[+] Allocation successful! Base Address: %p (PAGE_READWRITE)\n", allocatedAddress);

    // 2. Adım: Veriyi yaz
    const char data[] = "Hello, VirtualProtectEx!";
    SIZE_T bytesWritten = 0;
    WriteProcessMemory(hProcess, allocatedAddress, data, sizeof(data), &bytesWritten);
    printf("[+] Data written: %zu bytes\n", bytesWritten);

    // 3. Adım: VirtualProtectEx ile izinleri Değiştir (PAGE_EXECUTE_READWRITE / 0x40)
    DWORD oldProtect = 0;
    BOOL protectResult = VirtualProtectEx(
        hProcess,
        allocatedAddress,
        regionSize,
        PAGE_EXECUTE_READWRITE, // RWX yapıyoruz!
        &oldProtect
    );

    if (!protectResult) {
        printf("[-] VirtualProtectEx failed. Error: %lu\n", GetLastError());
    } else {
        printf("[+] VirtualProtectEx successful!\n");
        printf("[+] Old Protection : 0x%lX (PAGE_READWRITE = 0x04)\n", oldProtect);
        printf("[+] New Protection : PAGE_EXECUTE_READWRITE (0x40)\n");
    }

    CloseHandle(hProcess);
    printf("[+] Process handle closed successfully.\n");

    return 0;
}