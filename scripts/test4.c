#include <stdio.h>
#include <windows.h>

int main() {
    printf("Access Mask Values:\n");

    printf("single Access Masks:\n");

    printf("PROCESS_QUERY_INFORMATION: 0x%04X\n", PROCESS_QUERY_INFORMATION);
    printf("PROCESS_VM_READ: 0x%04X\n", PROCESS_VM_READ);
    printf("PROCESS_VM_WRITE: 0x%04X\n", PROCESS_VM_WRITE);
    printf("PROCESS_VM_OPERATION: 0x%04X\n", PROCESS_VM_OPERATION);
    printf("PROCESS_CREATE_THREAD: 0x%04X\n", PROCESS_CREATE_THREAD);
    printf("PROCESS_QUERY_LIMITED_INFORMATION: 0x%04X\n", PROCESS_QUERY_LIMITED_INFORMATION);

    printf("Bitwise OR Combination of Access Masks:\n");

    // VM_READ | VM_WRITE
    DWORD combinedAccessMask = PROCESS_VM_READ | PROCESS_VM_WRITE;
    printf("PROCESS_VM_READ | PROCESS_VM_WRITE: 0x%04X\n", combinedAccessMask);

    // VM_READ | VM_WRITE | VM_OPERATION
    DWORD fullAccessMask = PROCESS_VM_READ | PROCESS_VM_WRITE | PROCESS_VM_OPERATION;
    printf("PROCESS_VM_READ | PROCESS_VM_WRITE | PROCESS_VM_OPERATION: 0x%04X\n", fullAccessMask);

    return 0;
}