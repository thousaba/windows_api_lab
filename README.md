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
- [x] OpenProcess()
- [x] Process `HANDLE
- [x] GetProcessId()
- [x] CloseHandle()
- [x] bInheritHandle
- [x] Handle inheritance
- [x] VirtualQueryEx()
- [x] Virtual memory region enumeration
- [x] ReadProcessMemory()
- [x] WriteProcessMemory()
- [x] VirtualAllocEx()
- [x] VirtualProtectEx()
- [x] OpenProcessToken()
- [x] CreateRemoteThread()
- [x] Static API-CreateRemoteThread()
- [x] Dynamic API-CreateRemoteThread()
- [x] API Hashing-CreateRemoteThread()

---

# Phase 1b — Thread Fundamentals
 
- [x] Thread HANDLE
- [x] OpenThread()
- [x] GetThreadId() / GetCurrentThreadId()
- [x] SuspendThread() / ResumeThread()
- [x] GetThreadContext() / SetThreadContext()
- [x] CONTEXT structure (Rip/Eip, register set)
- [ ] QueueUserAPC()
- [ ] Alertable state / alertable wait
- [ ] SleepEx() / WaitForSingleObjectEx()

---

# Phase 1c — Process Creation & Section Fundamentals

- [ ] CreateProcess() / CreateProcessA/W
- [ ] CREATE_SUSPENDED flag
- [ ] PROCESS_INFORMATION / STARTUPINFO structures
- [ ] CreateFileMapping() / MapViewOfFile()
- [ ] NtUnmapViewOfSection()
- [ ] NtCreateSection() / NtMapViewOfSection()
- [ ] Image base relocation concept

---

# Phase 1d — Transacted File / NTFS TxF Fundamentals

- [ ] CreateFileTransacted()
- [ ] CreateTransaction()
- [ ] RollbackTransaction() / CommitTransaction()
- [ ] NtCreateProcessEx()

---

# Phase 1e — DLL / Module Loading Fundamentals

- [ ] LoadLibrary() / LoadLibraryA/W
- [ ] GetModuleHandle()
- [ ] GetProcAddress()
- [ ] DLL_PROCESS_ATTACH ve DllMain flow
- [ ] Reflective loading concept

---

# Phase 2 — Windows Memory Internals

- [ ] Virtual address space
- [ ] MEMORY_BASIC_INFORMATION
- [ ] Memory protection
- [ ] MEM_COMMIT / MEM_RESERVE / MEM_FREE
- [ ] Image / Private / Mapped memory
- [ ] PE image mappings
- [ ] VAD concepts
- [ ] Process memory scanning

---

# Phase 3 — Native API & System Calls

- [ ] Win32 API → Native API relationship
- [ ] NtOpenProcess
- [ ] NtAllocateVirtualMemory
- [ ] NtWriteVirtualMemory
- [ ] NtProtectVirtualMemory
- [ ] NtCreateThreadEx
- [ ] Syscall stubs
- [ ] x64 calling convention
- [ ] System Service Numbers

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

---

# Why This Project Exists

Understanding Windows internals is important for detection engineering.

A detection rule should not simply ask:

> "Did a process call OpenProcess?"

It should ask:

> "Which process accessed which target, with which rights, and what happened afterward?"

The same principle applies to memory allocation, memory modification, thread creation, and other process manipulation primitives.

This project therefore focuses on understanding the underlying Windows mechanisms first, and detection logic second.
