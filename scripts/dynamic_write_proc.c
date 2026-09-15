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
    pfnCreateRemoteThread myCreateRemoteThread = (pfnCreateRemoteThread)GetProcAddress(hKernel32, "CreateRemoteThread");

    if (!myOpenProcess || !myVirtualAllocEx || !myWriteProcessMemory || !myCreateRemoteThread) {
        printf("[-] API adresleri cozulemedi!\n");
        return -1;
    }

    printf("[+] API addresses found :\n");
    printf("    -> OpenProcess        : 0x%p\n", (void*)myOpenProcess);
    printf("    -> VirtualAllocEx     : 0x%p\n", (void*)myVirtualAllocEx);
    printf("    -> WriteProcessMemory : 0x%p\n", (void*)myWriteProcessMemory);
    printf("    -> CreateRemoteThread : 0x%p\n", (void*)myCreateRemoteThread);

    HANDLE hProcess = myOpenProcess(PROCESS_ALL_ACCESS, FALSE, targetPID);
    if (!hProcess) {
        printf("[-] Error Code : %lu\n", GetLastError());
        return -1;
    }
    printf("[+] Successful! Handle: 0x%p\n", hProcess);

    char payload[] = "hello this is thousaba";
    SIZE_T payloadSize = sizeof(payload);

    LPVOID pRemoteMem = myVirtualAllocEx(hProcess, NULL, payloadSize, MEM_COMMIT | MEM_RESERVE, PAGE_READWRITE);
    if (!pRemoteMem) {
        printf("[-] Error Code : %lu\n", GetLastError());
        CloseHandle(hProcess);
        return -1;
    }
    printf("[+] Remote memory established : 0x%p\n", pRemoteMem);

    SIZE_T bytesWritten = 0;
    BOOL bWritten = myWriteProcessMemory(hProcess, pRemoteMem, payload, payloadSize, &bytesWritten);
    if (bWritten) {
        printf("[+] Payload written successfully! Bytes: %zu.\n", bytesWritten);
    } else {
        printf("[-] Error Code : %lu\n", GetLastError());
    }

    CloseHandle(hProcess);
    return 0;
}