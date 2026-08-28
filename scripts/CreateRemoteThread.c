// 8-CreateRemoteThreadLab.c
#include <windows.h>
#include <stdio.h>

int main() {
    DWORD targetPid = 0;
    printf("CreateRemoteThread & Access Rights Lab\n");
    printf("======================================\n");
    printf("Enter target process ID (e.g., notepad.exe): ");
    if (scanf("%d", &targetPid) != 1) {
        printf("[-] Invalid PID!\n");
        return 1;
    }

    // STEP 1: Process Access Rights Validation
    // CreateRemoteThread requires PROCESS_CREATE_THREAD (0x0002),
    // PROCESS_VM_OPERATION (0x0008), and PROCESS_VM_WRITE (0x0020).
    DWORD desiredAccess = PROCESS_CREATE_THREAD | PROCESS_VM_OPERATION | PROCESS_VM_WRITE | PROCESS_QUERY_INFORMATION;
    
    printf("\n[+] Requesting Access Rights: 0x%04X\n", desiredAccess);
    HANDLE hProcess = OpenProcess(desiredAccess, FALSE, targetPid);

    if (hProcess == NULL) {
        printf("[-] OpenProcess failed! Error Code: %d\n", GetLastError());
        return 1;
    }
    printf("[+] Target PID     : %d\n", targetPid);
    printf("[+] Process Handle : 0x%p\n", hProcess);

    // STEP 2: Allocate Remote Memory
    SIZE_T payloadSize = 4096; // 1 Page
    LPVOID remoteAddress = VirtualAllocEx(
        hProcess,
        NULL,
        payloadSize,
        MEM_RESERVE | MEM_COMMIT,
        PAGE_READWRITE
    );

    if (remoteAddress == NULL) {
        printf("[-] VirtualAllocEx failed! Error Code: %d\n", GetLastError());
        CloseHandle(hProcess);
        return 1;
    }
    printf("[+] VirtualAllocEx Successful! Address: 0x%p\n", remoteAddress);

    // STEP 3: Shellcode / Dummy Payload Injection
    // Simple x64 Infinite Loop Stub (EB FE) or NOPs + RET (90 90 90 C3)
    // C3 = RET instruction (safely exits thread without crashing process)
    unsigned char payload[] = { 0x90, 0x90, 0x90, 0xC3 }; 
    SIZE_T bytesWritten = 0;

    BOOL writeResult = WriteProcessMemory(
        hProcess,
        remoteAddress,
        payload,
        sizeof(payload),
        &bytesWritten
    );

    if (!writeResult) {
        printf("[-] WriteProcessMemory failed! Error Code: %d\n", GetLastError());
        VirtualFreeEx(hProcess, remoteAddress, 0, MEM_RELEASE);
        CloseHandle(hProcess);
        return 1;
    }
    printf("[+] WriteProcessMemory Successful! Written: %zu bytes\n", bytesWritten);

    // STEP 4: Memory Protection Transition (PAGE_READWRITE -> PAGE_EXECUTE_READ)
    DWORD oldProtect = 0;
    BOOL protectResult = VirtualProtectEx(
        hProcess,
        remoteAddress,
        payloadSize,
        PAGE_EXECUTE_READ,
        &oldProtect
    );

    if (!protectResult) {
        printf("[-] VirtualProtectEx failed! Error Code: %d\n", GetLastError());
        CloseHandle(hProcess);
        return 1;
    }
    printf("[+] VirtualProtectEx Successful! RX Protection Applied (Old: 0x%X)\n", oldProtect);

    getchar();
    // STEP 5: Trigger Execution via CreateRemoteThread
    printf("\n[*] Triggering CreateRemoteThread...\n");
    DWORD threadId = 0;
    HANDLE hThread = CreateRemoteThread(
        hProcess,
        NULL,
        0,
        (LPTHREAD_START_ROUTINE)remoteAddress,
        NULL,
        0,
        &threadId
    );

    if (hThread == NULL) {
        printf("[-] CreateRemoteThread failed! Error Code: %d\n", GetLastError());
    } else {
        printf("[+] SUCCESS! Remote Thread Created!\n");
        printf("[+] Thread Handle : 0x%p\n", hThread);
        printf("[+] Thread ID     : %d\n", threadId);
        getchar();
        // Wait for thread execution to complete safely
        WaitForSingleObject(hThread, 2000);
        CloseHandle(hThread);
    }

    // Cleanup
    CloseHandle(hProcess);
    printf("[+] Process handle closed successfully.\n");
    return 0;
}