#include <windows.h>
#include <stdio.h>

int main(void)
{
    HMODULE h1 = LoadLibraryA("user32.dll");

    if (h1 == NULL) {
        printf("LoadLibrary failed: %lu\n", GetLastError());
        return 1;
    }

    HMODULE h2 = GetModuleHandleA("user32.dll");

    printf("LoadLibrary handle:      %p\n", (void *)h1);
    printf("GetModuleHandle handle:   %p\n", (void *)h2);

    FreeLibrary(h1);

    return 0;
}