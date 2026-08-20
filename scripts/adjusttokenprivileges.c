#include <windows.h>
#include <stdio.h>

int main() {
    printf("AdjustTokenPrivileges REAL PROOF LAB\n");
    printf("===================================\n");

    DWORD targetPid = 0;
    printf("Enter target LSASS PID: ");
    if (scanf("%lu", &targetPid) != 1) {
        printf("Invalid PID.\n");
        return 1;
    }

    printf("\n--- STAGE 1: OpenProcess WITHOUT SeDebugPrivilege ---\n");

    HANDLE hFail = OpenProcess(PROCESS_QUERY_INFORMATION | PROCESS_VM_READ, FALSE, targetPid);

    if (hFail == NULL) {
        DWORD err = GetLastError();
        printf("[-] FAILED AS EXPECTED! Error Code: %lu\n", err);
        if (err == 5) { // ERROR_ACCESS_DENIED
            printf("[!] REASON: ERROR_ACCESS_DENIED (5). Kernel blocked us because SeDebugPrivilege is NOT enabled!\n");
        }
    } else {
        printf("[+] Unexpected Success! Handle: %p\n", hFail);
        CloseHandle(hFail);
    }

    printf("\n--- STAGE 2: Enabling SeDebugPrivilege via AdjustTokenPrivileges ---\n");

    HANDLE hToken = NULL;
    if (!OpenProcessToken(GetCurrentProcess(), TOKEN_ADJUST_PRIVILEGES | TOKEN_QUERY, &hToken)) {
        printf("[-] OpenProcessToken failed. Error: %lu\n", GetLastError());
        return 1;
    }

    LUID luid;
    if (!LookupPrivilegeValueA(NULL, "SeDebugPrivilege", &luid)) {
        printf("[-] LookupPrivilegeValueA failed. Error: %lu\n", GetLastError());
        CloseHandle(hToken);
        return 1;
    }

    TOKEN_PRIVILEGES tp;
    tp.PrivilegeCount = 1;
    tp.Privileges[0].Luid = luid;
    tp.Privileges[0].Attributes = SE_PRIVILEGE_ENABLED;

    if (!AdjustTokenPrivileges(hToken, FALSE, &tp, sizeof(TOKEN_PRIVILEGES), NULL, NULL) || GetLastError() == ERROR_NOT_ALL_ASSIGNED) {
        printf("[-] AdjustTokenPrivileges failed! Run terminal as Administrator.\n");
        CloseHandle(hToken);
        return 1;
    }

    printf("[+] SUCCESS: SeDebugPrivilege is now ACTIVE in process token!\n");
    CloseHandle(hToken);

    printf("\n--- STAGE 3: OpenProcess WITH SeDebugPrivilege ENABLED ---\n");

    HANDLE hSuccess = OpenProcess(PROCESS_QUERY_INFORMATION | PROCESS_VM_READ, FALSE, targetPid);

    if (hSuccess == NULL) {
        printf("[-] Still Failed! Error Code: %lu\n", GetLastError());
    } else {
        printf("[+] VICTORY! OpenProcess SUCCESSFUL against LSASS.EXE!\n");
        printf("[+] Granted Process Handle: %p\n", hSuccess);
        CloseHandle(hSuccess);
    }

    return 0;
}