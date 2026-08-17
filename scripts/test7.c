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

    DWORD targetPid;

    printf("Target PID: ");
    scanf("%lu", &targetPid);

    HANDLE hProcess = OpenProcess(
        PROCESS_QUERY_LIMITED_INFORMATION,
        FALSE,
        targetPid
    );

    if (hProcess == NULL) {
        printf("[-] OpenProcess failed. Error: %lu\n", GetLastError());
        return 1;
    }

    printf("[+] OpenProcess succeeded: %p\n", hProcess);

    CloseHandle(hProcess);

    return 0;
}