#include <windows.h>
#include <stdio.h>

int main() {

    printf("GetTokenInformation Lab\n");
    printf("=======================\n");

    HANDLE hToken = NULL;

    if (!OpenProcessToken(GetCurrentProcess(), TOKEN_QUERY, &hToken)) {
        printf("[-] OpenProcessToken failed. Error: %lu\n", GetLastError());
        return 1;
    }

    printf("[+] OpenProcessToken successful!\n");

    DWORD tokenInfoLength = 0;

    GetTokenInformation(
        hToken,
        TokenPrivileges,
        NULL,
        0,
        &tokenInfoLength
    );

    if (tokenInfoLength == 0) {
        printf("[-] Failed to query token information length.\n");
        CloseHandle(hToken);
        return 1;
    }

    PTOKEN_PRIVILEGES pPrivileges = (PTOKEN_PRIVILEGES)malloc(tokenInfoLength);

    if (!GetTokenInformation(
            hToken,
            TokenPrivileges,
            pPrivileges,
            tokenInfoLength,
            &tokenInfoLength)) 
    {
        printf("[-] GetTokenInformation failed. Error: %lu\n", GetLastError());
        free(pPrivileges);
        CloseHandle(hToken);
        return 1;
    }

    printf("[+] Total Privileges Count: %lu\n\n", pPrivileges->PrivilegeCount);
    printf("%-35s | %-10s\n", "Privilege Name", "State");
    printf("----------------------------------------------------\n");

    for (DWORD i = 0; i < pPrivileges->PrivilegeCount; i++) {
        char privilegeName[256];
        DWORD nameSize = sizeof(privilegeName);

        if (LookupPrivilegeNameA(NULL, &pPrivileges->Privileges[i].Luid, privilegeName, &nameSize)) {
            
            BOOL isEnabled = (pPrivileges->Privileges[i].Attributes & SE_PRIVILEGE_ENABLED);
            
            printf("%-35s | %-10s\n", 
                privilegeName, 
                isEnabled ? "ENABLED" : "DISABLED");
        }
    }

    free(pPrivileges);
    CloseHandle(hToken);
    printf("\n[+] Token handle closed successfully.\n");

    return 0;
}