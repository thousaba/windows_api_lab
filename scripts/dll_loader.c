
#include <windows.h>
#include <stdio.h>

int main(void)
{
    HMODULE hDll = LoadLibraryA("lab_dll.dll");

    if (hDll == NULL)
    {
        printf("LoadLibraryA failed: %lu\n", GetLastError());
        return 1;
    }

    printf("DLL loaded. Base address: %p\n", (void *)hDll);
    getchar();

    FreeLibrary(hDll);
    printf("DLL unloaded.\n");

    return 0;
}
