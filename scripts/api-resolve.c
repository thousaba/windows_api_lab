#include <stdio.h>
#include <windows.h>

DWORD HashString(const char* str) {
    DWORD hash = 5381;
    int c;
    while ((c = *str++)) {
        hash = ((hash << 5) + hash) + c; // hash * 33 + c
    }
    return hash;
}

int main() {
    printf("#define HASH_OPENPROCESS        0x%08X\n", HashString("OpenProcess"));
    printf("#define HASH_VIRTUALALLOCEX     0x%08X\n", HashString("VirtualAllocEx"));
    printf("#define HASH_WRITEPROCESSMEMORY 0x%08X\n", HashString("WriteProcessMemory"));
    printf("#define HASH_CREATEREMOTETHREAD 0x%08X\n", HashString("CreateRemoteThread"));
    return 0;
}