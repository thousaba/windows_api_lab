#include <stdio.h>
#include <windows.h>

int main(void) {
    DWORD targetTid = 0;
    printf("[*] Enter target TID: ");
    if (scanf("%lu", &targetTid) != 1) {
        printf("[-] Invalid input.\n");
        return 1;
    }
    while (getchar() != '\n');

    HANDLE hThread = OpenThread(
        THREAD_SUSPEND_RESUME | THREAD_GET_CONTEXT | THREAD_SET_CONTEXT,
        FALSE,
        targetTid
    );
    if (hThread == NULL) {
        printf("[-] OpenThread failed. Error: %lu\n", GetLastError());
        return 1;
    }
    printf("[+] OpenThread Successful! Handle: 0x%p\n", hThread);

    DWORD suspendCount = SuspendThread(hThread);
    if (suspendCount == (DWORD)-1) {
        printf("[-] SuspendThread failed. Error: %lu\n", GetLastError());
        CloseHandle(hThread);
        return 1;
    }
    printf("[+] Thread SUSPENDED.\n");

    CONTEXT ctx;
    ctx.ContextFlags = CONTEXT_FULL; 

    if (!GetThreadContext(hThread, &ctx)) {
        printf("[-] GetThreadContext failed. Error: %lu\n", GetLastError());
        ResumeThread(hThread);
        CloseHandle(hThread);
        return 1;
    }

    printf("\n[+] CONTEXT captured:\n");
    printf("    Rip (Instruction Pointer): 0x%p\n", (void*)ctx.Rip);
    printf("    Rsp (Stack Pointer):       0x%p\n", (void*)ctx.Rsp);
    printf("    Rax:                       0x%016llX\n", ctx.Rax);
    printf("    Rbx:                       0x%016llX\n", ctx.Rbx);
    printf("    Rcx:                       0x%016llX\n", ctx.Rcx);
    printf("    Rdx:                       0x%016llX\n", ctx.Rdx);

    printf("\n[*] Press ENTER to RESUME the thread (no modification made)...\n");
    getchar();

    ResumeThread(hThread);
    printf("[+] Thread RESUMED.\n");

    CloseHandle(hThread);
    printf("[*] Press ENTER to exit...");
    getchar();
    return 0;
}