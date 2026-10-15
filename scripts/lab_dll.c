
#include <windows.h>

BOOL WINAPI DllMain(HINSTANCE hinstDLL, DWORD fdwReason, LPVOID lpvReserved)
{
    switch (fdwReason)
    {
        case DLL_PROCESS_ATTACH:
            OutputDebugStringA("[lab_dll] DLL_PROCESS_ATTACH\n");
            break;

        case DLL_PROCESS_DETACH:
            OutputDebugStringA("[lab_dll] DLL_PROCESS_DETACH\n");
            break;
    }

    return TRUE;
}
