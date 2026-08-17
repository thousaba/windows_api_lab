#include <stdio.h>
#include <windows.h>

int main(int argc, char* argv[]) {
    printf("\n    [CHILD] Child process started!\n");

    if (argc < 2) {
        printf("    [CHILD] Error: Parent did not pass a handle value.\n");
        return 1;
    }

    HANDLE hInherited = (HANDLE)strtoull(argv[1], NULL, 16);
    printf("    [CHILD] Handle inherited from parent: %p\n", hInherited);

    DWORD pid = GetProcessId(hInherited);
    if (pid != 0) {
        printf("    [CHILD] SUCCESS! PID read from inherited handle: %lu\n", pid);
    } else {
        printf("    [CHILD] FAILED! Invalid handle. Error Code: %lu\n", GetLastError());
    }

    return 0;
}