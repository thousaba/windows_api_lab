#include <stdio.h>
#include <windows.h>

#define SHM_NAME "Local\\WinAPI_SharedMemory" 
#define MEM_SIZE 1024                       

int main() {
    HANDLE hMapFile;
    char* pBuf;

    // 1. Step: Create a named Section Object in RAM using CreateFileMapping().
    // We use INVALID_HANDLE_VALUE because we don't want a disk file; we'll use RAM backed by the Page File.
    hMapFile = CreateFileMappingA(
        INVALID_HANDLE_VALUE,   // No disk file, use RAM
        NULL,                   // Default security attributes
        PAGE_READWRITE,         // Read and write access
        0,                      // High-order DWORD of size
        MEM_SIZE,               // Low-order DWORD of size - 1024 bytes
        SHM_NAME                // Unique name for the mapping
    );

    if (hMapFile == NULL) {
        printf("[-] CreateFileMapping error! Code: %lu\n", GetLastError());
        return 1;
    }
    printf("[+] Section object created (Handle: 0x%p)\n", hMapFile);

    // 2. Step: Map this object into our own virtual memory address space using MapViewOfFile().
    pBuf = (char*) MapViewOfFile(
        hMapFile,               // The mapping handle we created
        FILE_MAP_ALL_ACCESS,    // Full access (Read/Write)
        0, 0,                   // Offset (start from the beginning)
        MEM_SIZE                // Size to map
    );

    if (pBuf == NULL) {
        printf("[-] MapViewOfFile error! Code: %lu\n", GetLastError());
        CloseHandle(hMapFile);
        return 1;
    }
    printf("[+] Memory address (Pointer) obtained: 0x%p\n", (void*)pBuf);

    // 3. Step: Write directly into RAM through the pointer (NO ReadFile/WriteFile!)
    char secretMessage[] = "HELLO READER.EXE this is WRITER.EXE! IPC is Working.";
    CopyMemory((PVOID)pBuf, secretMessage, strlen(secretMessage) + 1);

    printf("[+] Message written to shared memory: \"%s\"\n", pBuf);
    printf("[*] Waiting for Reader program to run and read it... (Press ENTER to exit)\n");

    getchar(); // Keep the process open until Reader reads it, otherwise the handle closes!

    // Cleanup
    UnmapViewOfFile(pBuf);
    CloseHandle(hMapFile);

    return 0;
}