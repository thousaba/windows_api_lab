# 2. VirtualQueryEx

## What was tested

The `VirtualQueryEx()` API was used to inspect the virtual memory
layout of a target process.

The program opened the target process with:

```c
HANDLE hProcess = OpenProcess(
    PROCESS_QUERY_INFORMATION | PROCESS_VM_READ,
    FALSE,
    targetPid
);
```
MEMORY_BASIC_INFORMATION structure was then passed to:

```
VirtualQueryEx()
```

The first query started at:

```
LPCVOID lpaddress = NULL;
```

and returned information about the memory region containing that
address.

## C File

[VirtualQueryEx](../../scripts/test5.c)

## Observed output

```
Virtual Memory Query Lab
Enter the target process ID: 5800
[+] Target PID     : 5800
[+] Process Handle : 00000000000000D0
Memory Region Information:
Base Address: 0000000000000000
Allocation Base: 0000000000000000
Region Size: 2147352576 bytes
State: 65536
Protect: 1
Type: 0
Process handle closed successfully.
```

![VirtualQueryEx](../../screenshots/virtualqueryex.png)

## What was learned

VirtualQueryEx() does not read the contents of process memory.

Instead, it retrieves information about a region of virtual memory.

The returned MEMORY_BASIC_INFORMATION structure contains information
such as:

```
BaseAddress
AllocationBase
RegionSize
State
Protect
Type
```

The important distinction is:

```
VirtualQueryEx()
      |
      v
Memory region metadata
```

rather than:

```
VirtualQueryEx()
      |
      v
Actual memory contents
```

This distinction becomes important when moving from memory layout
inspection to APIs such as ReadProcessMemory().

## Important observation

The first query began at address NULL:

```
LPCVOID lpaddress = NULL;
```

Therefore the result describes the virtual memory region containing
that starting address.

A complete memory map requires repeatedly calling VirtualQueryEx()
with the address of the next region:

```
lpaddress =
    (PBYTE)mbi.BaseAddress +
    mbi.RegionSize;
```

That was explored in a later version of the lab, where hundreds of
memory regions were enumerated.

The key concept is:

```
Process Virtual Address Space
        |
        +-- Region
        +-- Region
        +-- Region
        +-- Region
        +-- ...
```

A process's virtual address space is therefore composed of many
individual regions rather than being one continuous memory block.

## 2. --- Enumerating Virtual Memory Regions

### What was tested

The previous test queried a single virtual memory region.

In this test, `VirtualQueryEx()` was called repeatedly to enumerate the
target process's virtual address space region by region.

The enumeration started at:

```c
PBYTE lpaddress = NULL;
```
After each successful query, the address was advanced to the beginning
of the next region:

```
lpaddress =
    (PBYTE)mbi.BaseAddress + mbi.RegionSize;
```

Only committed regions were displayed:

```
if (mbi.State == MEM_COMMIT)
```

## C File

[VirtualQueryEx Enumeration](../../scripts/test6.c)

Output:

![VirtualQueryEnum](../../screenshots/virtualqueryenum.png)

## What was learned

VirtualQueryEx() can be used repeatedly to walk through the virtual
address space of another process.

Each successful query describes one memory region through the
MEMORY_BASIC_INFORMATION structure.

The important values used in this lab were:

```
BaseAddress
RegionSize
State
Protect
```


This allows the program to move from one region to the next.

Conceptually:

```
Virtual Address Space

Region 0
    |
    | BaseAddress + RegionSize
    v
Region 1
    |
    | BaseAddress + RegionSize
    v
Region 2
    |
    | BaseAddress + RegionSize
    v
Region 3
    |
    v
...
```

The experiment demonstrated that a process's virtual address space is
composed of many separate memory regions rather than one continuous
memory block.

The experiment demonstrated that a process's virtual address space is
composed of many separate memory regions rather than one continuous
memory block.
