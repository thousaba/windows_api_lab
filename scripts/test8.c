#include <windows.h>
#include <stdio.h>

int main() {

    printf("ReadProcessMemory Lab\n");
    printf("=====================\n");

    char secret[] = "HELLO_MEMORY";

    printf("[+] Buffer address : %p\n", (void*)secret);
    printf("[+] Buffer content : %s\n", secret);

    HANDLE hProcess = GetCurrentProcess();

    char buffer[32] = {0};
    SIZE_T bytesRead = 0;

    BOOL result = ReadProcessMemory(
        hProcess,
        (LPCVOID)secret,
        buffer,
        sizeof(secret),
        &bytesRead
    );

    if (!result) {
        printf(
            "[-] ReadProcessMemory failed. Error: %lu\n",
            GetLastError()
        );
        return 1;
    }

    printf("[+] Read successful!\n");
    printf("[+] Bytes read : %zu\n", bytesRead);
    printf("[+] Data read  : %s\n", buffer);

    printf("[+] Hex dump:\n");

    for (SIZE_T i = 0; i < bytesRead; i++) {
        printf("%02X ", (unsigned char)buffer[i]);
    }

    printf("\n");

    return 0;
}