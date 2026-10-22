#include <windows.h>
#include <stdio.h>

int main(void)
{
    printf("=== Target Memory Process ===\n");
    printf("PID: %lu\n", (unsigned long)GetCurrentProcessId());
    printf("Keep this process running while the other program inspects it.\n");
    printf("Press Enter to exit...\n");

    getchar();
    return 0;
}