#include <stdio.h>
#include <windows.h>

int main() {
    printf("Windows Internals Lab-1\n");
    
    DWORD currentPid = GetCurrentProcessId();
    printf("Current Process ID: %lu\n", currentPid);

    return 0;
}