#include <stdio.h>
#include <windows.h>

typedef NTSTATUS(NTAPI* pfnNtUnmapViewOfSection)(
    HANDLE ProcessHandle,
    PVOID BaseAddress
);

#define SHM_NAME "Local\\UnmapLab"
#define MEM_SIZE 4096 

int main() {
    HANDLE hMapFile;
    char* pBuf;

    HMODULE hNtdll = GetModuleHandleA("ntdll.dll");
    if (!hNtdll) {
        printf("[-] ntdll.dll not found!\n");
        return 1;
    }

    pfnNtUnmapViewOfSection NtUnmapViewOfSection = 
        (pfnNtUnmapViewOfSection)GetProcAddress(hNtdll, "NtUnmapViewOfSection");

    if (!NtUnmapViewOfSection) {
        printf("[-] NtUnmapViewOfSection address not found!\n");
        return 1;
    }
    printf("[+] NtUnmapViewOfSection error: 0x%p\n", (void*)NtUnmapViewOfSection);

    hMapFile = CreateFileMappingA(
        INVALID_HANDLE_VALUE,
        NULL,
        PAGE_READWRITE,
        0,
        MEM_SIZE,
        SHM_NAME
    );

    if (!hMapFile) {
        printf("[-] CreateFileMapping error: %lu\n", GetLastError());
        return 1;
    }

    pBuf = (char*)MapViewOfFile(
        hMapFile,
        FILE_MAP_ALL_ACCESS,
        0, 0,
        MEM_SIZE
    );

    if (!pBuf) {
        printf("[-] MapViewOfFile error: %lu\n", GetLastError());
        CloseHandle(hMapFile);
        return 1;
    }

    strcpy_s(pBuf, MEM_SIZE, "HELLO - UNMAP LAB");

    printf("\n==================================================\n");
    printf("[+] MAPPED ADDRESS : 0x%p\n", (void*)pBuf);
    printf("[+] DATA   : %s\n", pBuf);
    printf("==================================================\n\n");

    printf("[*] STEP 1: Attach to Debugger / Check memory.\n");
    printf("[*] Click ENTER to continue and call NtUnmapViewOfSection...");
    getchar();

    NTSTATUS status = NtUnmapViewOfSection(GetCurrentProcess(), (PVOID)pBuf);

    if (status == 0) { // STATUS_SUCCESS (0x00000000)
        printf("\n[+] NtUnmapViewOfSection SUCCESSFULL! (Status: 0x%08X)\n", status);
    } else {
        printf("\n[-] NtUnmapViewOfSection ERROR! Status: 0x%08X\n", status);
    }

    printf("\n[*] ADIM 2: check the address (must be UNMAPPED ).\n");
    printf("[*] Click ENTER to exit...");
    getchar();

    CloseHandle(hMapFile);
    return 0;
}