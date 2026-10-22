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

**How this roadmap works:** each phase ends with a required deliverable (a lab writeup, a tool, or a detection query) before moving on. Checklist items inside a phase do not all need to be exhaustively completed first — go deep only where the deliverable needs it, and come back to the rest later if a real gap shows up.

---

# windows_api_lab — Roadmap

## Phase 1 — Process & Handle APIs

- [x] `OpenProcess()`
- [x] `GetProcessId()`
- [x] `CloseHandle()`
- [x] Access masks
- [x] Handle inheritance
- [x] `OpenProcessToken()`
- [x] `ReadProcessMemory()` / `WriteProcessMemory()`
- [x] `VirtualQueryEx()`
- [x] `VirtualAllocEx()` / `VirtualProtectEx()`
- [x] `CreateRemoteThread()`

**Goal:** Understand process handles, access rights, cross-process operations, and their observable behavior.

---

## Phase 1b — Thread APIs

- [x] `OpenThread()`
- [x] `GetThreadId()` / `GetCurrentThreadId()`
- [x] `SuspendThread()` / `ResumeThread()`
- [x] `GetThreadContext()` / `SetThreadContext()`
- [x] `QueueUserAPC()`
- [x] `SleepEx()` / `WaitForSingleObjectEx()`

**Goal:** Understand thread objects, thread state, context manipulation, APCs, and remote execution primitives.

---

## Phase 1c — Section & Mapping APIs

- [x] `CreateFileMapping()`
- [x] `MapViewOfFile()`
- [x] `NtCreateSection()`
- [x] `NtMapViewOfSection()`
- [x] `NtUnmapViewOfSection()`
- [x] Section Mapping with `RtlCreateUserThread()` and `QueueUserAPC()`


**Goal:** Understand sections, shared memory, mapped views, and how section objects differ from ordinary private allocations.

---

## Phase 1d — DLL & Module APIs

- [x] `LoadLibrary()`
- [x] `GetModuleHandle()`
- [x] `GetProcAddress()`
- [x] `DllMain`
- [x] DLL lifecycle
- [x] Dynamic API resolution
- [x] API hashing

**Goal:** Understand how Windows loads modules and resolves APIs.

---

# Phase 2 — Virtual Memory APIs & Internals

- [x] `VirtualQuery()` / `VirtualQueryEx()`
- [x] `VirtualAlloc()` / `VirtualAllocEx()`
- [x] `VirtualFree()` / `VirtualFreeEx()`
- [x] `MEMORY_BASIC_INFORMATION`
- [x] `MEM_COMMIT` / `MEM_RESERVE` / `MEM_FREE`
- [x] `MEM_IMAGE` / `MEM_PRIVATE` / `MEM_MAPPED`
- [x] Page protections

**Goal:** Connect the memory APIs already used in Phase 1 to what Windows actually creates in a process address space.

---

# Phase 3 — Process Creation APIs

- [x] `CreateProcess()`
- [ ] `CreateProcessW()`
- [ ] `CreateProcessAsUser()`
- [ ] `CreateProcessWithTokenW()`
- [ ] `STARTUPINFO` / `PROCESS_INFORMATION`
- [ ] Parent/child process relationships
- [ ] Process creation flags
- [ ] Environment and command-line handling

**Lab:** Create processes with different flags and inspect the resulting process/thread objects and telemetry.

---

# Phase 4 — File & I/O APIs

- [ ] `CreateFile()`
- [ ] `ReadFile()`
- [ ] `WriteFile()`
- [ ] `SetFilePointer()` / `SetFilePointerEx()`
- [ ] `GetFileSizeEx()`
- [ ] `DeleteFile()`
- [ ] `MoveFile()` / `MoveFileEx()`
- [ ] File HANDLEs and access/share modes
- [ ] Basic asynchronous I/O concept

**Lab:** Build a small file-inspection utility and correlate operations with Windows/Sysmon telemetry.

---

# Phase 5 — Registry APIs

- [ ] `RegOpenKeyEx()`
- [ ] `RegCreateKeyEx()`
- [ ] `RegQueryValueEx()`
- [ ] `RegSetValueEx()`
- [ ] `RegDeleteValue()`
- [ ] `RegCloseKey()`
- [ ] Registry HANDLEs and access masks

**Lab:** Read/write controlled test keys and observe Sysmon EID 12/13/14.

---

# Phase 6 — Service & SCM APIs

- [ ] `OpenSCManager()`
- [ ] `OpenService()`
- [ ] `CreateService()`
- [ ] `StartService()`
- [ ] `ControlService()`
- [ ] `QueryServiceStatusEx()`
- [ ] `DeleteService()`
- [ ] Service HANDLEs and access rights

**Lab:** Create and manage a harmless test service and examine SCM/service telemetry.

---

# Phase 7 — Token & Security APIs

Build on `OpenProcessToken()` from Phase 1.

- [ ] `GetTokenInformation()`
- [ ] `LookupPrivilegeValue()`
- [ ] `AdjustTokenPrivileges()`
- [ ] `DuplicateTokenEx()`
- [ ] Token types and impersonation levels
- [ ] Integrity levels
- [ ] Privileges vs access rights
- [ ] Security descriptors / ACLs — fundamentals only

**Goal:** Understand how Windows represents process identity and authorization.

---

# Phase 8 — Native API Layer

Only after the Win32 API foundation is established.

- [ ] Win32 → `ntdll` mapping
- [ ] `NtOpenProcess()`
- [ ] `NtAllocateVirtualMemory()`
- [ ] `NtReadVirtualMemory()` / `NtWriteVirtualMemory()`
- [ ] `NtProtectVirtualMemory()`
- [ ] `NtCreateThreadEx()`
- [ ] Compare equivalent Win32 and Native API operations
- [ ] Syscall transition — conceptual overview

**Goal:** Take APIs already understood at the Win32 layer and follow them one layer deeper.

---

# Why This Project Exists

Understanding Windows internals is important for detection engineering. A useful detection asks which process accessed which target, with which rights, what happened afterward, and what evidence supports that conclusion.

This project studies Windows mechanisms first, observes their behavior in controlled labs, and develops detection logic from verified evidence. Detection notes and telemetry limitations accompany each lab, with the final report in Phase 6 bringing those results together.