#include <windows.h>
#include <stdio.h>

VOID CALLBACK MyAPC(ULONG_PTR dwData)
{
    printf("[APC] APC routine executed! Data = %llu\n",
           (unsigned long long)dwData);
}

DWORD WINAPI WorkerThread(LPVOID lpParam)
{
    printf("[Worker] Entering SleepEx...\n");

    SleepEx(INFINITE, FALSE); // SleepEx(INFINITE, TRUE)

    printf("[Worker] SleepEx returned.\n");

    return 0;
}

int main(void)
{
    HANDLE hThread = CreateThread(
        NULL,
        0,
        WorkerThread,
        NULL,
        0,
        NULL
    );

    Sleep(1000);

    printf("[Main] Queueing APC...\n");

    QueueUserAPC(
        MyAPC,
        hThread,
        1234
    );

    WaitForSingleObject(hThread, INFINITE);

    CloseHandle(hThread);

    return 0;
}