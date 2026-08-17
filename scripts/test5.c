#include <windows.h>
#include <stdio.h>

int main() {
    printf("Virtual Memory Query Lab\n");

    DWORD targetPid = 0;
    printf("Enter the target process ID: ");
    if (scanf("%lu", &targetPid) != 1) {
        printf("Invalid input. Please enter a valid process ID.\n");
        return 1;
    }

    HANDLE hProcess = OpenProcess(PROCESS_QUERY_INFORMATION | PROCESS_VM_READ, FALSE, targetPid);
    if (hProcess == NULL) {
        DWORD error = GetLastError();
        printf("Failed to open process with ID %lu. Error: %lu\n", targetPid, error);
        return 1;
    }

    printf("[+] Target PID     : %lu\n", targetPid);
    printf("[+] Process Handle : %p\n", hProcess);

    MEMORY_BASIC_INFORMATION mbi;
    ZeroMemory(&mbi, sizeof(mbi));

    LPCVOID lpaddress = NULL;

    SIZE_T bytesReturned = VirtualQueryEx(hProcess, lpaddress, &mbi, sizeof(mbi));

    if (bytesReturned == 0) {
        DWORD error = GetLastError();
        printf("VirtualQueryEx failed. Error: %lu\n", error);
        CloseHandle(hProcess);
        return 1;
    }

    printf("Memory Region Information:\n");
    printf("Base Address: %p\n", mbi.BaseAddress);
    printf("Allocation Base: %p\n", mbi.AllocationBase);
    printf("Region Size: %zu bytes\n", mbi.RegionSize);
    printf("State: %lu\n", mbi.State);
    printf("Protect: %lu\n", mbi.Protect);
    printf("Type: %lu\n", mbi.Type);

    CloseHandle(hProcess);
    printf("Process handle closed successfully.\n");

    return 0;
}