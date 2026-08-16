#include <stdio.h>
#include <windows.h>

int main() {

    printf("OpenProcess -> NtOpenProcess Lab\n");
    printf("================================\n");

    printf("OpenProcess address: %p\n", OpenProcess);

    HMODULE hNtdll = GetModuleHandleA("ntdll.dll");

    if (hNtdll == NULL) {
        printf("[-] ntdll.dll bulunamadi. Error: %lu\n", GetLastError());
        return 1;
    }

    FARPROC ntOpenProcess = GetProcAddress(
        hNtdll,
        "NtOpenProcess"
    );

    if (ntOpenProcess == NULL) {
        printf("[-] NtOpenProcess bulunamadi. Error: %lu\n", GetLastError());
        return 1;
    }

    printf("ntdll.dll base     : %p\n", hNtdll);
    printf("NtOpenProcess addr : %p\n", ntOpenProcess);

    return 0;
}