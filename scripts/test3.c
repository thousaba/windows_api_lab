#include <stdio.h>
#include <windows.h>

int main() {
    printf("Windows Internals Lab-3: Remote Process Handle\n");
    printf("===============================\n");

    DWORD targetPid = 0;
    printf("Enter the target process ID: ");
    if (scanf("%lu", &targetPid) != 1) {
        printf("Invalid input. Please enter a valid process ID.\n");
        return 1;
    }

    HANDLE hProcess = OpenProcess (
        PROCESS_QUERY_LIMITED_INFORMATION, FALSE, targetPid);
    
    
    if (hProcess == NULL) {
        DWORD error = GetLastError();
        printf("Failed to open process with ID %lu. Error: %lu\n", targetPid, error);
        return 1;
    }
    
    printf("[+] Target PID     : %lu\n", targetPid);
    printf("[+] Process Handle : %p\n", hProcess);
    printf("[+] Process Handle Value: %lu\n", GetProcessId(hProcess));
    printf("[+] bInheritHandle      : FALSE\n");
    
    if (!CloseHandle(hProcess)) {
        printf("Failed to close process handle. Error: %lu\n", GetLastError());
    } else {
        printf("Process handle closed successfully.\n");
    }

    return 0;
}