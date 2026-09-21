#include <stdio.h>
#include <windows.h>

int main(void) {
    HANDLE hThread = GetCurrentThread();
    printf("[+] GetCurrentThread() pseudo-handle: 0x%p\n", hThread);

    DWORD tid = GetCurrentThreadId();
    printf("[+] Real TID (for comparison): %lu\n", tid);

    printf("[*] Press ENTER to exit...");
    getchar();
    return 0;
}