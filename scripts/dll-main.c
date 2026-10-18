#include <windows.h>
#include <stdio.h>

int main(void)
{
    HMODULE hDll = LoadLibraryA("create-file.dll");

    if (hDll == NULL)
    {
        printf("LoadLibraryA failed: %lu\n", GetLastError());
        return 1;
    }

    printf("DLL loaded. Base address: %p\n", (void *)hDll);
    printf("File Created");

    FreeLibrary(hDll);
    printf("DLL unloaded.\n");

    return 0;
}
