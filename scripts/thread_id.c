#include <stdio.h>
#include <windows.h>

int main(void){
    DWORD tid = GetCurrentThreadId();
    printf("Thread ID (TID): %lu\n", tid);

    HANDLE hSelfProcess = GetCurrentProcess();
    DWORD pid = GetCurrentProcessId();
    printf("Process Id (PID): %lu\n", pid);

    printf("[*] Press ENTER to exit...");
    getchar();
    return 0;
}