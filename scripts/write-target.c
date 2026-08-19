#include <windows.h>
#include <stdio.h>

volatile char sharedData[] = "BEFORE_WRITE";

int main() {
    printf("WriteProcessMemory Target Lab\n");
    printf("==============================\n");

    printf("[+] PID     : %lu\n", GetCurrentProcessId());
    printf("[+] Address : %p\n", (void *)sharedData);
    printf("[+] Data    : %s\n", sharedData);

    printf("\nPress ENTER to exit...\n");
    getchar();

    printf("[+] Final Data: %s\n", sharedData);

    return 0;
}