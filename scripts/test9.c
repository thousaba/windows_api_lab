#include <windows.h>
#include <stdio.h>

int main() {

    printf("Remote ReadProcessMemory Lab\n");
    printf("============================\n");

    DWORD targetPid = 0;

    printf("Enter the target process ID: ");

    if (scanf("%lu", &targetPid) != 1) {
        printf("Invalid PID.\n");
        return 1;
    }

    HANDLE hProcess = OpenProcess(
        PROCESS_QUERY_INFORMATION | PROCESS_VM_READ,
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

    MEMORY_BASIC_INFORMATION mbi;
    ZeroMemory(&mbi, sizeof(mbi));

    PBYTE address = NULL;

    while (VirtualQueryEx(
        hProcess,
        address,
        &mbi,
        sizeof(mbi)
    ) != 0) {

        if (mbi.State == MEM_COMMIT &&
            (mbi.Protect & PAGE_READONLY ||
             mbi.Protect & PAGE_READWRITE ||
             mbi.Protect & PAGE_EXECUTE_READ ||
             mbi.Protect & PAGE_EXECUTE_READWRITE)) {

            printf("[+] Readable region found\n");
            printf("    Base   : %p\n", mbi.BaseAddress);
            printf("    Size   : %zu bytes\n", mbi.RegionSize);
            printf("    Protect: 0x%lx\n", mbi.Protect);

            BYTE buffer[16] = {0};
            SIZE_T bytesRead = 0;

            BOOL result = ReadProcessMemory(
                hProcess,
                mbi.BaseAddress,
                buffer,
                sizeof(buffer),
                &bytesRead
            );

            if (result) {

                printf("[+] ReadProcessMemory succeeded\n");
                printf("[+] Bytes read: %zu\n", bytesRead);

                printf("[+] Data:\n");

                for (SIZE_T i = 0; i < bytesRead; i++) {
                    printf("%02X ", buffer[i]);
                }

                printf("\n");

                break;
            }
        }

        address =
            (PBYTE)mbi.BaseAddress + mbi.RegionSize;
    }

    CloseHandle(hProcess);

    printf("[+] Process handle closed successfully.\n");

    return 0;
}