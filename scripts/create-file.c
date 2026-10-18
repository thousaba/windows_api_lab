
#include <windows.h>
#include <stdio.h>

BOOL WINAPI DllMain(HINSTANCE hinstDLL, DWORD fdwReason,
                    LPVOID lpvReserved)
{
    if (fdwReason == DLL_PROCESS_ATTACH)
    {
        FILE *f = fopen("C:\\temp_test\\pwned.txt", "w");

        if (f != NULL)
        {
            fputs("DLL loaded successfully!\n", f);
            fclose(f);
        }
    }

    return TRUE;
}
