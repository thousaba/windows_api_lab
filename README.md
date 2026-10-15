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

# Phase 1 — Process & Handle Fundamentals

- [x] Access Masks
- [x] OpenProcess()
- [x] Process HANDLE
- [x] GetProcessId()
- [x] CloseHandle()
- [x] bInheritHandle / Handle inheritance
- [x] VirtualQueryEx() / Virtual memory region enumeration
- [x] ReadProcessMemory() / WriteProcessMemory()
- [x] VirtualAllocEx() / VirtualProtectEx()
- [x] OpenProcessToken()
- [x] CreateRemoteThread() — static API, dynamic API, API hashing variants

**Deliverable:** done — `windows_api_lab` repo, Part 1 (static IAT method) committed with x64dbg analysis + Splunk "CHAINED_ACCESS_REFLECTIVE_INJECTION" detection.

---

# Phase 1b — Thread Fundamentals (mostly complete — finish the loop)

- [x] Thread HANDLE
- [x] OpenThread()
- [x] GetThreadId() / GetCurrentThreadId()
- [x] SuspendThread() / ResumeThread()
- [x] GetThreadContext() / SetThreadContext() — **done**: msfvenom shellcode injected into notepad.exe via thread hijacking, got a working shell
- [x] QueueUserAPC()
- [x] Alertable state / alertable wait (SleepEx() / WaitForSingleObjectEx())

**Deliverable:** one short writeup comparing thread hijacking (done) vs APC injection (todo) — same target, same lab notebook format (Teknik/Amaç/API/Mekanizma/Telemetry/Detection). Don't move to 1c until this comparison exists, even if it's short.

---

# Phase 1c — Process Creation & Section Fundamentals

- [x] CreateFileMapping() / MapViewOfFile()
- [x] NtUnmapViewOfSection()
- [x] NtCreateSection() / NtMapViewOfSection()
- [x] Section mapping with RtlCreateUserThread()
- [x] Section mapping with QueueUserAPC

**Deliverable:** a minimal process hollowing PoC against a suspended benign target, with the Sysmon/ETW evidence it produces.

---

# Phase 1d — DLL & Module Loading Fundamentals

- [x] LoadLibrary() / GetModuleHandle() / GetProcAddress()
- [ ] DllMain / DLL_PROCESS_ATTACH lifecycle
- [ ] IAT / EAT, static imports vs dynamic resolution
- [ ] Module-load telemetry (Sysmon EID 7)

**Deliverable:** extend the Phase 1 API-hashing test with a short note on how a reflective loader would evade EID 7, and what would still catch it.

---

# Phase 2 — Windows Virtual Memory Internals (scope this down)

Keep this phase tied to Phase 1c/1d findings — don't study memory internals in the abstract.

- [ ] MEMORY_BASIC_INFORMATION fields you actually use in VirtualQueryEx output
- [ ] MEM_COMMIT / MEM_RESERVE / MEM_FREE, protection transitions
- [ ] Image vs Private vs Mapped memory — enough to label a region correctly
- [ ] VAD — conceptual overview only (System Informer already shows this)

**Deliverable:** none standalone — fold this into the Phase 1c/1d writeups as a "what the memory layout looked like" section, rather than a separate report.

---

# Phase 3 — x64, PE & Debugging Foundations

## 3.1 x64 (scope to what's needed to read x64dbg traces, not full assembly mastery — this overlaps your RE track)

- [ ] Calling convention, argument registers, shadow space
- [ ] Stack frames, prologues/epilogues — enough to recognize them, not to hand-write them

## 3.2 PE and debugging

- [ ] PE headers/sections/imports/exports — as needed per lab, not exhaustively upfront
- [ ] x64dbg workflow for correlating API args → memory → thread state

**Deliverable:** none standalone — this phase is infrastructure for Phase 1c/1d/4/5 labs, not its own report.

---

# Phase 3.5 — Memory-Safety & Code-Reuse Concepts (ROP)

- [ ] DEP/NX, ASLR, stack canaries, CFG/CET — what each mitigation blocks and doesn't
- [ ] Memory corruption / crash triage fundamentals
- [ ] Build a ROP chain against an intentionally vulnerable local lab binary
- [ ] Analyze how the mitigations above change what the chain has to do

**Deliverable:** one writeup — trigger a stack overflow in a vulnerable lab binary, build a working ROP chain that bypasses at least one mitigation (e.g. DEP via ROP), document the chain and what each mitigation would have blocked if enabled.

---

# Phase 4 — Native API & System Call Internals

- [ ] Win32 → Native API mapping for the APIs you've already used (NtOpenProcess, NtAllocateVirtualMemory, NtWriteVirtualMemory, NtProtectVirtualMemory, NtCreateThreadEx)
- [ ] Syscall stub / SSN — inspect one in a disassembler
- [ ] User-mode hooking visibility limits
- [ ] Direct/indirect syscalls — conceptual, tied to a telemetry comparison

**Deliverable:** rewrite one earlier test (e.g. the API-hashing one) to call Nt* directly instead of Win32, and document what telemetry changes vs stays the same.

---

# Phase 5 — Process Behavior Analysis

- [ ] Remote thread injection — already covered, consolidate notes
- [ ] Thread hijacking — already covered (Phase 1b), consolidate notes
- [ ] APC injection — once Phase 1b todo is done
- [ ] Process hollowing — once Phase 1c is done
- [ ] Process Doppelgänging / Herpaderping / Ghosting — conceptual comparison, Sysmon EID 25 coverage

**Deliverable:** one consolidated "injection technique matrix" (technique / APIs / telemetry / detection confidence) — this can likely reuse/extend [[attack-coverage-matrix]] rather than being a new artifact.

---

# Phase 6 — Detection Engineering & Final Report (merged with former Phase 7)

This replaces the old separate "Phase 6: Malware Analysis" and "Phase 7: Final Integrated Project" — they covered the same ground twice.

- [ ] For each behavior studied in Phase 5, write and tune a Splunk detection with stated FP/FN evaluation
- [ ] Map each behavior to MITRE ATT&CK
- [ ] Document Sysmon/ETW sensor coverage and known blind spots per technique
- [ ] Pick 2–3 of the strongest behaviors (not all of them) and write them up as the final portfolio report, linking internals → behavior → detection → confidence

**Deliverable:** the final portfolio report itself. This is the actual output of the whole repo — everything above feeds into it.

---

# Why This Project Exists

Understanding Windows internals is important for detection engineering. A useful detection asks which process accessed which target, with which rights, what happened afterward, and what evidence supports that conclusion.

This project studies Windows mechanisms first, observes their behavior in controlled labs, and develops detection logic from verified evidence. Detection notes and telemetry limitations accompany each lab, with the final report in Phase 6 bringing those results together.