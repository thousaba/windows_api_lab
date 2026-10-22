
#include <windows.h>
#include <stdio.h>
#include <string.h>

int main(void)
{
    printf("Cross-Process Memory Inspection Lab\n");
    printf("===================================\n");

    SIZE_T size = 4096;

    LPVOID memory = VirtualAlloc(
        NULL,
        size,
        MEM_RESERVE | MEM_COMMIT,
        PAGE_READWRITE
    );

    if (memory == NULL) {
        printf("VirtualAlloc failed: %lu\n", GetLastError());
        return 1;
    }

    strcpy((char *)memory, "Private memory lab");

    MEMORY_BASIC_INFORMATION mbi;
    if (VirtualQuery(memory, &mbi, sizeof(mbi)) == 0) {
        printf("VirtualQuery failed: %lu\n", GetLastError());
        VirtualFree(memory, 0, MEM_RELEASE);
        return 1;
    }

    printf("[+] PID: %lu\n", GetCurrentProcessId());
    printf("[+] Allocation address: %p\n", memory);
    printf("[+] Region size: %zu bytes\n", mbi.RegionSize);
    printf("[+] State: 0x%lx\n", (unsigned long)mbi.State);
    printf("[+] Protect: 0x%lx\n", (unsigned long)mbi.Protect);
    printf("[+] Type: 0x%lx\n", (unsigned long)mbi.Type);

    printf("\nKeep this process running for inspection.\n");
    printf("Press Enter to release memory and exit...\n");
    getchar();

    VirtualFree(memory, 0, MEM_RELEASE);
    printf("[+] Memory released. Exiting.\n");

    return 0;
}
