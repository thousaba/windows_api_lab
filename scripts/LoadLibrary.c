
#include <windows.h>
#include <stdio.h>

typedef int (WINAPI *MessageBoxA_t)(
    HWND,
    LPCSTR,
    LPCSTR,
    UINT
);

int main(void)
{
    HMODULE hUser32 = LoadLibraryA("user32.dll");

    if (hUser32 == NULL) {
        printf("LoadLibraryA failed: %lu\n", GetLastError());
        return 1;
    }

    printf("user32.dll handle: %p\n", (void *)hUser32);

    FARPROC proc = GetProcAddress(hUser32, "MessageBoxA");

    if (proc == NULL) {
        printf("GetProcAddress failed: %lu\n", GetLastError());
        FreeLibrary(hUser32);
        return 1;
    }

    printf("MessageBoxA address: %p\n", (void *)proc);

    MessageBoxA_t MessageBoxDynamic = (MessageBoxA_t)proc;

    MessageBoxDynamic(
        NULL,
        "Dynamic API resolution works!",
        "Windows API Lab",
        MB_OK | MB_ICONINFORMATION
    );

    FreeLibrary(hUser32);

    return 0;
}
