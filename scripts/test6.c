#include <windows.h>
#include <stdio.h>

int main() {
    printf("Virtual Memory Enumeration Lab\n");
    printf("==============================\n");

    DWORD targetPid = 0;

    printf("Enter the target process ID: ");
    if (scanf("%lu", &targetPid) != 1) {
        printf("Invalid input. Please enter a valid process ID.\n");
        return 1;
    }

    HANDLE hProcess = OpenProcess(
        PROCESS_QUERY_INFORMATION | PROCESS_VM_READ,
        FALSE,
        targetPid
    );

    if (hProcess == NULL) {
        DWORD error = GetLastError();
        printf(
            "Failed to open process with ID %lu. Error: %lu\n",
            targetPid,
            error
        );
        return 1;
    }

    printf("[+] Target PID     : %lu\n", targetPid);
    printf("[+] Process Handle : %p\n", hProcess);
    printf("[+] Enumerating committed memory regions...\n");
    printf("===============================================\n");

    MEMORY_BASIC_INFORMATION mbi;
    ZeroMemory(&mbi, sizeof(mbi));

    PBYTE lpaddress = NULL;
    int count = 0;

    while (VirtualQueryEx(
        hProcess,
        lpaddress,
        &mbi,
        sizeof(mbi)
    ) != 0) {

        if (mbi.State == MEM_COMMIT) {

            printf(
                "[%d] Base: %p | Size: %zu KB | Protect: 0x%lx\n",
                count,
                mbi.BaseAddress,
                mbi.RegionSize / 1024,
                mbi.Protect
            );

            count++;
        }

        lpaddress =
            (PBYTE)mbi.BaseAddress + mbi.RegionSize;
    }

    printf("===============================================\n");
    printf("[+] Committed regions found: %d\n", count);

    CloseHandle(hProcess);

    printf("[+] Process handle closed successfully.\n");

    return 0;
}