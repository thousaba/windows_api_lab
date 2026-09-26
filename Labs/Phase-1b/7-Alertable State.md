# Alertable State and User-Mode APC Delivery — Lab Report

## 1. Objective

The objective of this lab was to understand how a user-mode APC is delivered to a thread and to determine the role of an **alertable wait** in APC execution.

The experiment used:

* `QueueUserAPC()`
* `SleepEx()`
* A dedicated worker thread
* Two different `SleepEx()` configurations:

  * `SleepEx(INFINITE, TRUE)`
  * `SleepEx(INFINITE, FALSE)`

[Alertable State Source Code](../../scripts/alertable.c)  

The experiment was performed to distinguish **APC queuing** from **APC delivery**.

---

## 2. Experimental Design

The program created a worker thread containing:

```c
SleepEx(INFINITE, TRUE);
```

The main thread then queued an APC:

```c
QueueUserAPC(MyAPC, hThread, 1234);
```

The APC routine printed the value received through `dwData`.

The experiment was then repeated with:

```c
SleepEx(INFINITE, FALSE);
```

The only intentional difference between the two tests was the alertable parameter.

---

## 3. Alertable Wait — TRUE

The first test used:

```c
SleepEx(INFINITE, TRUE);
```

The observed output was:

```text
[Worker] Entering SleepEx...
[Main] Queueing APC...
[APC] APC routine executed! Data = 1234
[Worker] SleepEx returned.
```

### Observation

The worker thread entered `SleepEx()` in an alertable state.

The main thread subsequently executed:

```c
QueueUserAPC(MyAPC, hThread, 1234);
```

The APC was queued to the worker thread.

Because the worker was in an alertable wait, the APC was delivered and the following routine executed:

```c
MyAPC(1234);
```

After the APC routine returned, `SleepEx()` returned and the worker continued execution.

The execution flow was therefore:

```text
Worker Thread
     |
     v
SleepEx(INFINITE, TRUE)
     |
     |  Alertable wait
     |
     | <--- APC queued by Main Thread
     |
     v
MyAPC(1234)
     |
     v
SleepEx() returns
```

---

## 4. Non-Alertable Wait — FALSE

The second test changed only the second argument:

```c
SleepEx(INFINITE, FALSE);
```

The observed output was:

```text
[Worker] Entering SleepEx...
[Main] Queueing APC...
```

The program remained in this state.

The following message was not printed:

```text
[APC] APC routine executed! Data = 1234
```

Likewise, the worker did not print:

```text
[Worker] SleepEx returned.
```

### Observation

The APC was still queued by:

```c
QueueUserAPC(MyAPC, hThread, 1234);
```

However, the worker was in a **non-alertable wait**.

Therefore, the APC was not delivered at that point, and the worker remained blocked inside:

```c
SleepEx(INFINITE, FALSE);
```

The execution flow was:

```text
Worker Thread
     |
     v
SleepEx(INFINITE, FALSE)
     |
     |  Non-alertable wait
     |
     | <--- APC queued
     |
     X
   APC not delivered
```

---

## 5. Key Difference

The experiment demonstrated that two separate operations must be distinguished:

### APC Queuing

```c
QueueUserAPC(...)
```

This places an APC in the target thread's APC queue.

### APC Delivery

The target thread must reach a state in which the queued user-mode APC can be delivered.

The experiment demonstrated this difference directly:

```text
QueueUserAPC()
      |
      v
APC queued
      |
      +-----------------------+
      |                       |
      v                       v
Alertable wait           Non-alertable wait
(TRUE)                   (FALSE)
      |                       |
      v                       X
APC delivered            APC not delivered
      |
      v
MyAPC()
```

---

## 6. Role of `INFINITE`

The experiment also clarified the meaning of `INFINITE`.

For example:

```c
SleepEx(INFINITE, TRUE);
```

does **not** mean:

> "Alertable forever."

Instead:

* `INFINITE` specifies the timeout.
* `TRUE` specifies that the wait is alertable.

Therefore:

```text
INFINITE = how long to wait
TRUE/FALSE = whether the wait is alertable
```

This distinction is important when analyzing Windows synchronization APIs.

---

## 7. Relation to the Previous APC Injection Lab

The previous APC injection experiment used:

```c
QueueUserAPC(...)
ResumeThread(...)
WaitForSingleObject(...)
```

The current experiment was intentionally separated from that scenario.

The previous experiment demonstrated that an APC-based execution path could reach the injected payload.

This experiment instead isolated the **APC delivery mechanism** itself.

In particular, the experiment showed that:

```c
WaitForSingleObject(hThread, INFINITE);
```

in the injector should not be interpreted as the mechanism that makes the target thread alertable.

That wait belongs to the injector's thread. The `SleepEx()` calls in this experiment belong to the worker thread receiving the APC.

---

## 8. Malware Analysis Relevance

From a malware-analysis perspective, understanding alertable waits helps explain why an APC-related API call does not necessarily mean that the APC routine executes immediately.

When analyzing a sample containing:

```c
QueueUserAPC(...)
```

the analyst should distinguish:

1. Which thread receives the APC.
2. When the APC is queued.
3. What the target thread is doing afterward.
4. When the APC can actually be delivered.
5. Where execution transfers when the APC routine runs.

This makes it possible to follow the actual execution flow rather than assuming that `QueueUserAPC()` immediately transfers execution.

---

## 9. Detection Engineering Relevance

From a defensive perspective, the experiment also demonstrates why a single API call does not necessarily describe the complete behavior.

A sequence such as:

```text
VirtualAllocEx
      ↓
WriteProcessMemory
      ↓
QueueUserAPC
      ↓
ResumeThread
```

contains several distinct operations.

`QueueUserAPC()` represents APC queuing, while actual APC execution is a subsequent execution event.

Therefore, behavioral detection can benefit from correlating multiple events instead of treating a single API call as proof of execution.

---

## 10. Conclusion

The experiment successfully demonstrated the difference between **queuing** and **delivering** a user-mode APC.

With:

```c
SleepEx(INFINITE, TRUE);
```

the worker entered an alertable wait, allowing the queued APC to execute:

```text
QueueUserAPC()
      ↓
APC queued
      ↓
Alertable wait
      ↓
MyAPC(1234)
      ↓
SleepEx returns
```

With:

```c
SleepEx(INFINITE, FALSE);
```

the worker entered a non-alertable wait. The APC was queued, but it was not delivered during that wait:

```text
QueueUserAPC()
      ↓
APC queued
      ↓
Non-alertable wait
      ↓
APC not delivered
```

The experiment therefore established the fundamental distinction:

> **`QueueUserAPC()` queues the APC; an alertable execution state provides a condition under which the queued user-mode APC can be delivered.**
