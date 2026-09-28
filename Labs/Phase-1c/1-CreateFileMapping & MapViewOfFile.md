# CreateFileMapping / MapViewOfFile — Shared Memory IPC Lab

## Objective

The objective of this lab was to understand how Windows processes can share memory using `CreateFileMapping` and `MapViewOfFile`, and to observe this mechanism through a simple Writer/Reader IPC scenario.

The goal was not to read one process's private memory directly, but to create a shared memory region that both processes can access.

---

## APIs

### CreateFileMapping

`CreateFileMapping` creates a **file-mapping object**, also commonly referred to as a **section object**.

The object represents a region of memory that can later be mapped into one or more processes.

In this lab, `writer.exe` created a named mapping object. Because it was named, `reader.exe` could later locate and open the same object.

Conceptually:

```text
CreateFileMapping()
        ↓
Section / File-Mapping Object
        ↓
Shared memory can be mapped into processes
```

Importantly, creating the mapping object does not by itself give the process a normal pointer that can be used to access the data. The process needs to map a view of the object.

---

### MapViewOfFile

`MapViewOfFile` maps a view of an existing file-mapping/section object into the calling process's virtual address space.

This gives the process a normal virtual address that can be used to read or write the mapped memory.

In this lab:

```text
CreateFileMapping
        ↓
Section object
        ↓
MapViewOfFile
        ↓
Pointer to mapped memory
```

The returned pointer is valid in the virtual address space of the process that called `MapViewOfFile`.

---

## Test Scenario

Two separate programs were used:

```text
writer.exe
reader.exe
```

[writer.c](../../scripts/writer.c)

[reader.c](../../scripts/reader.c)


### 1. Writer creates the shared memory

`writer.exe` called `CreateFileMapping` and created the section object.

Output:

```text
[+] Section object created
    Handle: 0x00000000000000B8
```

The returned handle represents the section object inside the writer process.

---

### 2. Writer maps the section

`writer.exe` then called `MapViewOfFile`.

Output:

```text
[+] Memory address (Pointer) obtained:
    0x0000028682420000
```

This address is the writer's virtual address for its view of the shared memory.

The writer then wrote:

```text
HELLO READER.EXE this is WRITER.EXE! IPC is Working.
```

into that memory.

---

### 3. Reader opens the same section

`reader.exe` started separately and opened the existing named section object.

Output:

```text
[+] Connected to existing Section object!
    Handle: 0x00000000000000B4
```

Notice that the handle is different from the writer's handle.

That is expected because each process has its own handle table.

The reader then called `MapViewOfFile` and received its own virtual address:

```text
[+] Reader memory address (Pointer) obtained:
    0x0000028AFEF90000
```

---

## Why Are the Addresses Different?

The writer and reader received different virtual addresses:

```text
WRITER.EXE
0x0000028682420000

READER.EXE
0x0000028AFEF90000
```

This does **not** mean that they are accessing different copies of the data.

The important distinction is:

```text
             Section Object
                  │
          Shared backing memory
             /           \
            /             \
           ↓               ↓
     Writer View       Reader View
           │               │
  0x286842420000    0x28AFEF90000
```

Each process has its own virtual address space, so the same shared memory can appear at different virtual addresses.

Both views ultimately refer to the same underlying mapped memory.

---

## Result

The reader successfully received the exact message written by the writer:

```text
[--->] DATA READ:
HELLO READER.EXE this is WRITER.EXE! IPC is Working.
```

This confirmed that the two independent processes were successfully communicating through shared memory.

The experiment demonstrated the basic relationship:

```text
CreateFileMapping
        ↓
Create/Open Section Object
        ↓
MapViewOfFile
        ↓
Obtain process-local virtual address
        ↓
Read / Write shared memory
```

---

## Key Findings

* `CreateFileMapping` creates the file-mapping/section object that represents the shared memory region.
* `MapViewOfFile` maps that object into a process's virtual address space.
* The returned pointer is process-specific.
* Different processes can have different virtual addresses for the same shared memory.
* The writer does not directly read or modify the reader's private memory.
* The reader does not directly read the writer's private memory.
* Instead, both processes access a common shared memory region.
* This mechanism is a Windows IPC technique and is useful when processes need to exchange data efficiently through shared memory.

### Lab Summary

```text
WRITER.EXE                         READER.EXE
     │                                  │
     │ CreateFileMapping()              │
     │                                  │
     ├────── Section Object ────────────┤
     │                                  │
     │ MapViewOfFile()                  │ MapViewOfFile()
     │                                  │
     ↓                                  ↓
Writer View                        Reader View
0x286842420000                     0x28AFEF90000
     │                                  │
     └──────── Shared Memory ───────────┘
```

The lab therefore established the basic Windows **section object → mapped view → shared memory → IPC** model.
