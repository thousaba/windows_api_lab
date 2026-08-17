# Windows API Lab

> Hands-on Windows internals study focused on process objects, handles, access rights, virtual memory, Native API, and process manipulation.

## Objective

This repository documents a hands-on study of Windows process internals from a defensive and detection-engineering perspective.

The goal is not to memorize APIs. The goal is to understand:

- What each API does
- Why it is needed
- What Windows object or mechanism it interacts with
- Which permissions it requires
- What happens underneath the Win32 API
- What telemetry may expose the operation
- How the same primitives can be used in process manipulation techniques

---

# Project Roadmap

## Phase 1 — Process & Handle Fundamentals

- [x] Access Masks
- [x] `OpenProcess()`
- [x] Process `HANDLE`
- [x] `GetProcessId()`
- [x] `CloseHandle()`
- [x] `bInheritHandle`
- [x] Handle inheritance
- [x] `VirtualQueryEx()`
- [x] Virtual memory region enumeration
- [ ] `ReadProcessMemory()`
- [ ] `WriteProcessMemory()`
- [ ] `VirtualAllocEx()`
- [ ] `VirtualProtectEx()`
- [ ] `OpenProcessToken()`
- [ ] Process access rights
- [ ] Thread access rights
- [ ] `CreateRemoteThread()`

---

# Phase 2 — Windows Memory Internals

- [ ] Virtual address space
- [ ] `MEMORY_BASIC_INFORMATION`
- [ ] Memory protection
- [ ] `MEM_COMMIT` / `MEM_RESERVE` / `MEM_FREE`
- [ ] Image / Private / Mapped memory
- [ ] PE image mappings
- [ ] VAD concepts
- [ ] Process memory scanning

---

# Phase 3 — Native API & System Calls

- [ ] Win32 API → Native API relationship
- [ ] `NtOpenProcess`
- [ ] `NtAllocateVirtualMemory`
- [ ] `NtWriteVirtualMemory`
- [ ] `NtProtectVirtualMemory`
- [ ] `NtCreateThreadEx`
- [ ] Syscall stubs
- [ ] x64 calling convention
- [ ] System Service Numbers

The intended model is:

```text
Win32 API
    ↓
kernel32 / kernelbase
    ↓
ntdll.dll
    ↓
Native API
    ↓
syscall
    ↓
Windows Kernel
```

---

# Phase 4 — Process Manipulation Techniques

The Windows primitives will later be combined to understand common process manipulation techniques.

- [ ] Remote Thread Injection
- [ ] Classic DLL Injection
- [ ] Process Hollowing
- [ ] Section Mapping
- [ ] Thread Hijacking
- [ ] APC Injection
- [ ] Process Doppelgänging
- [ ] Process Herpaderping
- [ ] Early Bird Injection

Each technique will be studied from three perspectives:

```text
Mechanism
    ↓
Windows API / Internals
    ↓
Detection / Telemetry
```

---

# Phase 5 — Detection Engineering

For each technique:

- [ ] Identify observable operations
- [ ] Map relevant Sysmon events
- [ ] Examine ETW visibility
- [ ] Examine EDR-style telemetry
- [ ] Identify blind spots
- [ ] Develop detection logic
- [ ] Test false positives
- [ ] Document limitations

Example process-manipulation chain:

```text
Process A
    |
    | OpenProcess()
    | access rights
    v
Process B
    |
    | VirtualAllocEx()
    v
Process B memory
    |
    | WriteProcessMemory()
    v
Process B memory
    |
    | VirtualProtectEx()
    v
Executable memory
    |
    | CreateRemoteThread()
    v
Execution
```

Not every occurrence of these APIs is malicious. The detection objective is to understand the context surrounding the operation.


---

# Why This Project Exists

Understanding Windows internals is important for detection engineering.

A detection rule should not simply ask:

> "Did a process call OpenProcess?"

It should ask:

> "Which process accessed which target, with which rights, and what happened afterward?"

The same principle applies to memory allocation, memory modification, thread creation, and other process manipulation primitives.

This project therefore focuses on understanding the underlying Windows mechanisms first, and detection logic second.
