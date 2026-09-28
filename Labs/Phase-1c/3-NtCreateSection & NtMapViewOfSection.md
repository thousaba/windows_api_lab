# NtCreateSection / NtMapViewOfSection — Native Section Mapping Lab

## Objective

The objective of this lab was to understand the Native API equivalents of the previously tested `CreateFileMapping` and `MapViewOfFile` functionality.

The lab demonstrated how Windows creates a section object with `NtCreateSection()` and maps that section into a process's virtual address space with `NtMapViewOfSection()`.

---

## APIs

### NtCreateSection

`NtCreateSection` is a Native API exported by `ntdll.dll` that creates a **section object**.

In this lab, the section was:

* 4096 bytes in size
* read/write
* pagefile-backed rather than backed by an actual file
* created with `SEC_COMMIT`

The resulting handle was stored in `hSection`.

Conceptually:

```text
NtCreateSection()
        ↓
Section Object
        ↓
hSection
```

The handle itself is **not a memory address**. It is a reference that the process can use to perform operations on the section.

---

### NtMapViewOfSection

`NtMapViewOfSection` maps a view of an existing section into a specified process's virtual address space.

In this lab:

```c
NtMapViewOfSection(
    hSection,
    GetCurrentProcess(),
    &baseAddress,
    ...
);
```

was used to map the section into the current process.

Windows returned a virtual address through `baseAddress`:

```text
0x0000025761410000
```

This address represented the beginning of the mapped view.

The test string:

```text
HELLO NATIVE SECTION!
```

was then written to this address.

---

## Test Flow

The complete execution flow was:

```text
NtCreateSection()
        ↓
Create 4096-byte section
        ↓
Section handle returned
        ↓
NtMapViewOfSection()
        ↓
Section mapped into current process
        ↓
0x0000025761410000
        ↓
Write "HELLO NATIVE SECTION!"
        ↓
Verify with x64dbg
```

x64dbg confirmed the mapping by displaying the written data at the returned address:

```text
0000025761410000
48 45 4C 4C 4F 20 4E 41 54 49 56 45 20 53 45 43
54 49 4F 4E 21
```

which corresponds to:

```text
HELLO NATIVE SECTION!
```

---

## Relationship to the Win32 APIs

The previous lab used the higher-level Win32 APIs:

```text
CreateFileMapping()
        ↓
MapViewOfFile()
```

This lab demonstrated the Native API layer:

```text
NtCreateSection()
        ↓
NtMapViewOfSection()
```

Together with the previous `NtUnmapViewOfSection()` experiment, the basic section lifecycle can be represented as:

```text
NtCreateSection()
        ↓
Section Object
        ↓
NtMapViewOfSection()
        ↓
Mapped View
        ↓
Process Virtual Address
        ↓
NtUnmapViewOfSection()
        ↓
View Removed
```

---

## Key Findings

* `NtCreateSection()` creates the section object.
* The returned section handle is not the same thing as a virtual memory address.
* `NtMapViewOfSection()` maps the section into a process's virtual address space.
* The mapped view receives a process-specific virtual address.
* The mapped memory can then be accessed through that address according to its protection.
* `NtUnmapViewOfSection()` can later remove the view from the process.
* The Native API provides lower-level access to the same fundamental section/mapping mechanism previously observed through `CreateFileMapping` and `MapViewOfFile`.

## Lab Summary

The experiment established the fundamental relationship:

```text
Section Object
      │
      │ NtMapViewOfSection
      ▼
Process Virtual Address Space
      │
      ▼
Mapped View
      │
      ▼
Accessible Memory
```

This provides the foundation for understanding **section-based memory operations and PE image mapping**, which will be relevant when studying image base relocation and Process Hollowing.
