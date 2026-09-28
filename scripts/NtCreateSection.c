#include <windows.h>
#include <stdio.h>

typedef LONG NTSTATUS;

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

#define STATUS_SUCCESS ((NTSTATUS)0x00000000L)

int main(void)
{
    HMODULE hNtdll = GetModuleHandleA("ntdll.dll");

    if (!hNtdll) {
        printf("[-] ntdll.dll not found!\n");
        return 1;
    }

    pNtCreateSection NtCreateSection =
        (pNtCreateSection)GetProcAddress(
            hNtdll,
            "NtCreateSection"
        );

    pNtMapViewOfSection NtMapViewOfSection =
        (pNtMapViewOfSection)GetProcAddress(
            hNtdll,
            "NtMapViewOfSection"
        );

    if (!NtCreateSection || !NtMapViewOfSection) {
        printf("[-] Native API not found!\n");
        return 1;
    }

    printf("[+] NtCreateSection   : %p\n", (void*)NtCreateSection);
    printf("[+] NtMapViewOfSection: %p\n", (void*)NtMapViewOfSection);

    HANDLE hSection = NULL;

    LARGE_INTEGER sectionSize;
    sectionSize.QuadPart = 4096;

    NTSTATUS status = NtCreateSection(
        &hSection,
        SECTION_ALL_ACCESS,
        NULL,
        &sectionSize,
        PAGE_READWRITE,
        SEC_COMMIT,
        NULL
    );

    if (status != STATUS_SUCCESS) {
        printf("[-] NtCreateSection failed: 0x%08X\n",
               (unsigned int)status);
        return 1;
    }

    printf("[+] Section created! Handle: %p\n",
           (void*)hSection);

    PVOID baseAddress = NULL;
    SIZE_T viewSize = 0;

    status = NtMapViewOfSection(
        hSection,
        GetCurrentProcess(),
        &baseAddress,
        0,
        0,
        NULL,
        &viewSize,
        2,              // ViewUnmap
        0,
        PAGE_READWRITE
    );

    if (status != STATUS_SUCCESS) {
        printf("[-] NtMapViewOfSection failed: 0x%08X\n",
               (unsigned int)status);

        CloseHandle(hSection);
        return 1;
    }

    printf("[+] Section mapped!\n");
    printf("[+] Base address: %p\n", baseAddress);
    printf("[+] View size   : %zu\n", viewSize);

    strcpy_s(
        (char*)baseAddress,
        4096,
        "HELLO NATIVE SECTION!"
    );

    printf("[+] Data written: %s\n", (char*)baseAddress);

    printf("\n============================================\n");
    printf("[*] Attach x64dbg and inspect the address.\n");
    printf("[*] Press ENTER to exit...\n");
    printf("============================================\n");

    getchar();

    CloseHandle(hSection);

    return 0;
}