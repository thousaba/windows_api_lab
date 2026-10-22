#include <windows.h>
#include <stdio.h>
#include <stdlib.h>

int main(void)
{
    DWORD pid;

    printf("=== VirtualAllocEx Lab ===\n");
    printf("Target PID: ");
    if (scanf("%lu", &pid) != 1 || pid == 0) {
        printf("Invalid PID.\n");
        return 1;
    }

    HANDLE hProcess = OpenProcess(
        PROCESS_VM_OPERATION | PROCESS_QUERY_INFORMATION,
        FALSE,
        pid
    );

    if (hProcess == NULL) {
        printf("OpenProcess failed: %lu\n",
               (unsigned long)GetLastError());
        return 1;
    }

    SIZE_T size = 4096;

    LPVOID remoteMemory = VirtualAllocEx(
        hProcess,
        NULL,
        size,
        MEM_RESERVE | MEM_COMMIT,
        PAGE_READWRITE
    );

    if (remoteMemory == NULL) {
        printf("VirtualAllocEx failed: %lu\n",
               (unsigned long)GetLastError());
        CloseHandle(hProcess);
        return 1;
    }

    printf("\n[+] Allocation succeeded\n");
    printf("[+] Target PID: %lu\n", (unsigned long)pid);
    printf("[+] Remote address: %p\n", remoteMemory);

    MEMORY_BASIC_INFORMATION mbi;

    SIZE_T result = VirtualQueryEx(
        hProcess,
        remoteMemory,
        &mbi,
        sizeof(mbi)
    );

    if (result == 0) {
        printf("VirtualQueryEx failed: %lu\n",
               (unsigned long)GetLastError());
    } else {
        printf("\n=== Observed Region ===\n");
        printf("BaseAddress:      %p\n", mbi.BaseAddress);
        printf("RegionSize:       %llu bytes\n",
               (unsigned long long)mbi.RegionSize);
        printf("State:            0x%lx\n",
               (unsigned long)mbi.State);
        printf("Protect:          0x%lx\n",
               (unsigned long)mbi.Protect);
        printf("AllocationProtect: 0x%lx\n",
               (unsigned long)mbi.AllocationProtect);
        printf("Type:             0x%lx\n",
               (unsigned long)mbi.Type);
    }

    printf("\nPress Enter to free the remote allocation...\n");
    getchar(); 
    getchar();  

    if (VirtualFreeEx(hProcess, remoteMemory, 0, MEM_RELEASE)) {
        printf("[+] Remote allocation released.\n");
    } else {
        printf("VirtualFreeEx failed: %lu\n",
               (unsigned long)GetLastError());
    }

    CloseHandle(hProcess);
    return 0;
}