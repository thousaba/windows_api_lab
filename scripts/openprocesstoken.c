# include <windows.h>
# include <stdio.h>

int main() {
    printf("OpenProcessToken Lab\n");
    printf("====================\n");

    HANDLE hToken = NULL;

    BOOL result = OpenProcessToken(
        GetCurrentProcess(),
        TOKEN_QUERY | TOKEN_ADJUST_PRIVILEGES,
        &hToken
    );

    if (!result) {
        printf("[-] OpenProcessToken failed. Error: %lu\n", GetLastError());
        return 1;
    }

    printf("[+] OpenProcessToken successful!\n");
    printf("[+] Current Process Handle: %p\n", GetCurrentProcess());
    printf("[+] Token Handle: %p\n", hToken);

    CloseHandle(hToken);
    printf("[+] Token handle closed successfully.\n");

    return 0;
}