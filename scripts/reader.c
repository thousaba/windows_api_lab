#include <stdio.h>
#include <windows.h>

#define SHM_NAME "Local\\WinAPI_SharedMemory"
#define MEM_SIZE 1024

int main() {
    HANDLE hMapFile;
    char* pBuf;

    // 1. Step: Connect to the existing memory object opened by Writer using OpenFileMapping
    hMapFile = OpenFileMappingA(
        FILE_MAP_READ,          // We only want read access
        FALSE,                  // Handle should not be inherited
        SHM_NAME                // Same name defined by Writer!
    );

    if (hMapFile == NULL) {
        printf("[-] OpenFileMapping error! Is Writer running? Code: %lu\n", GetLastError());
        return 1;
    }
    printf("[+] Connected to existing Section object! (Handle: 0x%p)\n", hMapFile);

    // 2. Step: Map it into our own memory address space using MapViewOfFile
    pBuf = (char*) MapViewOfFile(
        hMapFile,
        FILE_MAP_READ,          // Read-only access
        0, 0,
        MEM_SIZE
    );

    if (pBuf == NULL) {
        printf("[-] MapViewOfFile error! Code: %lu\n", GetLastError());
        CloseHandle(hMapFile);
        return 1;
    }
    printf("[+] Reader memory address (Pointer) obtained: 0x%p\n", (void*)pBuf);

    // 3. Step: Read the data directly from the address!
    printf("\n==========================================");
    printf("\n[--->] DATA READ: %s\n", pBuf);
    printf("==========================================\n\n");

    // Cleanup
    UnmapViewOfFile(pBuf);
    CloseHandle(hMapFile);

    return 0;
}