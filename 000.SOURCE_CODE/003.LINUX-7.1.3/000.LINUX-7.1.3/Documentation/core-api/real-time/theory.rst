.. SPDX-License-Identifier: GPL-2.0

=============================
Theory of operation(工作原理)
=============================

:Author: Sebastian Andrzej Siewior <bigeasy@linutronix.de>

(作者: Sebastian Andrzej Siewior <bigeasy@linutronix.de>)

Preface(前言)
=============

PREEMPT_RT transforms the Linux kernel into a real-time kernel. It achieves
this by replacing locking primitives, such as spinlock_t, with a preemptible
and priority-inheritance aware implementation known as rtmutex, and by enforcing
the use of threaded interrupts. As a result, the kernel becomes fully
preemptible, with the exception of a few critical code paths, including entry
code, the scheduler, and low-level interrupt handling routines.
(PREEMPT_RT 把 Linux 内核转变为实时内核。它通过用一种可抢占且具有优先级继承意识的实现（称为 rtmutex）替换诸如 spinlock_t 之类的锁原语，以及强制使用线程化中断来实现这一点。结果是，内核变得完全可抢占，只有少数关键代码路径除外，包括入口代码、调度器和低级中断处理例程。)

This transformation places the majority of kernel execution contexts under the
control of the scheduler and significantly increasing the number of preemption
points. Consequently, it reduces the latency between a high-priority task
becoming runnable and its actual execution on the CPU.
(这种转变使大多数内核执行上下文处于调度器的控制之下，并显著增加了抢占点的数量。因此，它缩短了高优先级任务变为可运行状态与其实际在 CPU 上执行之间的延迟。)

Scheduling(调度)
================

The core principles of Linux scheduling and the associated user-space API are
documented in the man page sched(7)
`sched(7) <https://man7.org/linux/man-pages/man7/sched.7.html>`_.
By default, the Linux kernel uses the SCHED_OTHER scheduling policy. Under
this policy, a task is preempted when the scheduler determines that it has
consumed a fair share of CPU time relative to other runnable tasks. However,
the policy does not guarantee immediate preemption when a new SCHED_OTHER task
becomes runnable. The currently running task may continue executing.
(Linux 调度的核心原则和相关的用户空间 API 记录在手册页 sched(7) `sched(7) <https://man7.org/linux/man-pages/man7/sched.7.html>`_ 中。默认情况下，Linux 内核使用 SCHED_OTHER 调度策略。在该策略下，当调度器判断某个任务相对于其他可运行任务已经消耗了公平份额的 CPU 时间时，该任务会被抢占。然而，当一个新的 SCHED_OTHER 任务变为可运行时，该策略并不保证立即抢占。当前正在运行的任务可能继续执行。)

This behavior differs from that of real-time scheduling policies such as
SCHED_FIFO. When a task with a real-time policy becomes runnable, the
scheduler immediately selects it for execution if it has a higher priority than
the currently running task. The task continues to run until it voluntarily
yields the CPU, typically by blocking on an event.
(这种行为与 SCHED_FIFO 等实时调度策略不同。当一个具有实时策略的任务变为可运行时，如果它比当前运行的任务具有更高的优先级，调度器会立即选择它执行。该任务会持续运行，直到它自愿让出 CPU，通常是通过在某个事件上阻塞。)

Sleeping spin locks(可睡眠自旋锁)
=================================

The various lock types and their behavior under real-time configurations are
described in detail in Documentation/locking/locktypes.rst.
In a non-PREEMPT_RT configuration, a spinlock_t is acquired by first disabling
preemption and then actively spinning until the lock becomes available. Once
the lock is released, preemption is enabled. From a real-time perspective,
this approach is undesirable because disabling preemption prevents the
scheduler from switching to a higher-priority task, potentially increasing
latency.
(各种锁类型及其在实时配置下的行为在 Documentation/locking/locktypes.rst 中有详细描述。在非 PREEMPT_RT 配置中，获取 spinlock_t 时首先禁用抢占，然后主动自旋直到锁可用。一旦锁被释放，抢占被重新启用。从实时角度来看，这种方法是不可取的，因为禁用抢占会阻止调度器切换到更高优先级的任务，从而可能增加延迟。)

To address this, PREEMPT_RT replaces spinning locks with sleeping spin locks
that do not disable preemption. On PREEMPT_RT, spinlock_t is implemented using
rtmutex. Instead of spinning, a task attempting to acquire a contended lock
disables CPU migration, donates its priority to the lock owner (priority
inheritance), and voluntarily schedules out while waiting for the lock to
become available.
(为了解决这个问题，PREEMPT_RT 用不禁用抢占的可睡眠自旋锁替换自旋锁。在 PREEMPT_RT 上，spinlock_t 使用 rtmutex 实现。尝试获取竞争锁的任务不是自旋，而是禁用 CPU 迁移，把它的优先级让给锁拥有者（优先级继承），并在等待锁可用期间自愿调度出去。)

Disabling CPU migration provides the same effect as disabling preemption, while
still allowing preemption and ensuring that the task continues to run on the
same CPU while holding a sleeping lock.
(禁用 CPU 迁移提供与禁用抢占相同的效果，同时仍允许抢占，并确保任务在持有可睡眠锁期间继续在同一 CPU 上运行。)

Priority inheritance(优先级继承)
================================

Lock types such as spinlock_t and mutex_t in a PREEMPT_RT enabled kernel are
implemented on top of rtmutex, which provides support for priority inheritance
(PI). When a task blocks on such a lock, the PI mechanism temporarily
propagates the blocked task’s scheduling parameters to the lock owner.
(在启用 PREEMPT_RT 的内核中，spinlock_t 和 mutex_t 等锁类型构建在 rtmutex 之上，后者提供优先级继承（PI）支持。当任务在此类锁上阻塞时，PI 机制会临时把被阻塞任务的调度参数传播给锁拥有者。)

For example, if a SCHED_FIFO task A blocks on a lock currently held by a
SCHED_OTHER task B, task A’s scheduling policy and priority are temporarily
inherited by task B. After this inheritance, task A is put to sleep while
waiting for the lock, and task B effectively becomes the highest-priority task
in the system. This allows B to continue executing, make progress, and
eventually release the lock.
(例如，如果 SCHED_FIFO 任务 A 阻塞在一个当前由 SCHED_OTHER 任务 B 持有的锁上，任务 A 的调度策略和优先级会被任务 B 临时继承。在这种继承之后，任务 A 在等待锁期间被置为睡眠，而任务 B 实际上成为系统中优先级最高的任务。这使 B 能够继续执行、取得进展并最终释放锁。)

Once B releases the lock, it reverts to its original scheduling parameters, and
task A can resume execution.
(一旦 B 释放锁，它就恢复到原来的调度参数，任务 A 可以继续执行。)

Threaded interrupts(线程化中断)
===============================

Interrupt handlers are another source of code that executes with preemption
disabled and outside the control of the scheduler. To bring interrupt handling
under scheduler control, PREEMPT_RT enforces threaded interrupt handlers.
(中断处理程序是在禁用抢占且不受调度器控制的情况下执行的另一类代码。为了把中断处理纳入调度器控制，PREEMPT_RT 强制使用线程化中断处理程序。)

With forced threading, interrupt handling is split into two stages. The first
stage, the primary handler, is executed in IRQ context with interrupts disabled.
Its sole responsibility is to wake the associated threaded handler. The second
stage, the threaded handler, is the function passed to request_irq() as the
interrupt handler. It runs in process context, scheduled by the kernel.
(通过强制线程化，中断处理被拆分为两个阶段。第一阶段是主处理程序（primary handler），在中断禁用的情况下于 IRQ 上下文中执行。它的唯一职责是唤醒相关的线程化处理程序。第二阶段是线程化处理程序（threaded handler），即作为中断处理程序传递给 request_irq() 的函数。它在进程上下文中运行，由内核调度。)

From waking the interrupt thread until threaded handling is completed, the
interrupt source is masked in the interrupt controller. This ensures that the
device interrupt remains pending but does not retrigger the CPU, allowing the
system to exit IRQ context and handle the interrupt in a scheduled thread.
(从唤醒中断线程到线程化处理完成，中断源在中断控制器中被屏蔽。这确保设备中断保持挂起状态，但不会再次触发 CPU，使系统能够退出 IRQ 上下文并在一个被调度的线程中处理该中断。)

By default, the threaded handler executes with the SCHED_FIFO scheduling policy
and a priority of 50 (MAX_RT_PRIO / 2), which is midway between the minimum and
maximum real-time priorities.
(默认情况下，线程化处理程序以 SCHED_FIFO 调度策略和优先级 50（MAX_RT_PRIO / 2）执行，即介于最小和最大实时优先级之间的中点。)

If the threaded interrupt handler raises any soft interrupts during its
execution, those soft interrupt routines are invoked after the threaded handler
completes, within the same thread. Preemption remains enabled during the
execution of the soft interrupt handler.
(如果线程化中断处理程序在执行期间引发任何软中断，这些软中断例程会在线程化处理程序完成后于同一线程内被调用。在软中断处理程序执行期间，抢占保持启用。)

Summary(总结)
=============

By using sleeping locks and forced-threaded interrupts, PREEMPT_RT
significantly reduces sections of code where interrupts or preemption is
disabled, allowing the scheduler to preempt the current execution context and
switch to a higher-priority task.
(通过使用可睡眠锁和强制线程化中断，PREEMPT_RT 显著减少了禁用中断或抢占的代码段，使调度器能够抢占当前执行上下文并切换到更高优先级的任务。)
