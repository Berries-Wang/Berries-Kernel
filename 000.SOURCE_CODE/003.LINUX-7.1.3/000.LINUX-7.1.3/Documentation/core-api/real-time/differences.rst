.. SPDX-License-Identifier: GPL-2.0

=============================================
How realtime kernels differ(实时内核有何不同)
=============================================

:Author: Sebastian Andrzej Siewior <bigeasy@linutronix.de>(作者：Sebastian Andrzej Siewior)

Preface(前言)
=============

With forced-threaded interrupts and sleeping spin locks, code paths that
previously caused long scheduling latencies have been made preemptible and
moved into process context. This allows the scheduler to manage them more
effectively and respond to higher-priority tasks with reduced latency.
(通过强制线程化中断与可睡眠的自旋锁，原先造成长调度延迟的代码路径已变得可抢占并被移入进程上下文。这使调度器能够更有效地管理它们，并以更低的延迟响应更高优先级的任务。)

The following chapters provide an overview of key differences between a
PREEMPT_RT kernel and a standard, non-PREEMPT_RT kernel.
(以下各章概述 PREEMPT_RT 内核与标准非 PREEMPT_RT 内核之间的主要区别。)

Locking(锁)
===========

Spinning locks such as spinlock_t are used to provide synchronization for data
structures accessed from both interrupt context and process context. For this
reason, locking functions are also available with the _irq() or _irqsave()
suffixes, which disable interrupts before acquiring the lock. This ensures that
the lock can be safely acquired in process context when interrupts are enabled.
(spinlock_t 这类自旋锁用于为既会从中断上下文又会从进程上下文访问的数据结构提供同步。因此，锁函数还提供带 _irq() 或 _irqsave() 后缀的版本，它们在获取锁之前先禁用中断。这确保在开中断的情况下也能安全地在进程上下文中获取锁。)

However, on a PREEMPT_RT system, interrupts are forced-threaded and no longer
run in hard IRQ context. As a result, there is no need to disable interrupts as
part of the locking procedure when using spinlock_t.
(不过在 PREEMPT_RT 系统上，中断被强制线程化，不再在硬 IRQ 上下文中运行。因此使用 spinlock_t 时，加锁过程中无需再禁用中断。)

For low-level core components such as interrupt handling, the scheduler, or the
timer subsystem the kernel uses raw_spinlock_t. This lock type preserves
traditional semantics: it disables preemption and, when used with _irq() or
_irqsave(), also disables interrupts. This ensures proper synchronization in
critical sections that must remain non-preemptible or with interrupts disabled.
(对于中断处理、调度器或定时器子系统等低层核心组件，内核使用 raw_spinlock_t。这种锁类型保留传统语义：它禁用抢占，并且与 _irq() 或 _irqsave() 一起使用时还禁用中断。这确保了必须保持不可抢占或禁用中断的临界区中的正确同步。)

Execution context(执行上下文)
=============================

Interrupt handling in a PREEMPT_RT system is invoked in process context through
the use of threaded interrupts. Other parts of the kernel also shift their
execution into threaded context by different mechanisms. The goal is to keep
execution paths preemptible, allowing the scheduler to interrupt them when a
higher-priority task needs to run.
(PREEMPT_RT 系统中的中断处理通过线程化中断在进程上下文中被调用。内核的其他部分也通过各种机制把执行转移到线程上下文。目标是让执行路径保持可抢占，当更高优先级的任务需要运行时，调度器可以中断它们。)

Below is an overview of the kernel subsystems involved in this transition to
threaded, preemptible execution.
(以下是参与这一向线程化、可抢占执行过渡的内核子系统概览。)

Interrupt handling(中断处理)
----------------------------

All interrupts are forced-threaded in a PREEMPT_RT system. The exceptions are
interrupts that are requested with the IRQF_NO_THREAD, IRQF_PERCPU, or
IRQF_ONESHOT flags.
(在 PREEMPT_RT 系统中，所有中断都被强制线程化。例外是以 IRQF_NO_THREAD、IRQF_PERCPU 或 IRQF_ONESHOT 标志申请的中断。)

The IRQF_ONESHOT flag is used together with threaded interrupts, meaning those
registered using request_threaded_irq() and providing only a threaded handler.
Its purpose is to keep the interrupt line masked until the threaded handler has
completed.
(IRQF_ONESHOT 标志与线程化中断一起使用，即那些通过 request_threaded_irq() 注册且只提供线程处理函数的中断。其用途是在线程处理函数完成之前保持中断线处于屏蔽状态。)

If a primary handler is also provided in this case, it is essential that the
handler does not acquire any sleeping locks, as it will not be threaded. The
handler should be minimal and must avoid introducing delays, such as
busy-waiting on hardware registers.
(这种情况下如果还提供了主处理函数 (primary handler)，必须确保该处理函数不获取任何可睡眠锁，因为它不会被线程化。处理函数应当尽量精简，必须避免引入延迟，例如对硬件寄存器的忙等。)


Soft interrupts, bottom half handling(软中断与下半部处理)
---------------------------------------------------------

Soft interrupts are raised by the interrupt handler and are executed after the
handler returns. Since they run in thread context, they can be preempted by
other threads. Do not assume that softirq context runs with preemption
disabled. This means you must not rely on mechanisms like local_bh_disable() in
process context to protect per-CPU variables. Because softirq handlers are
preemptible under PREEMPT_RT, this approach does not provide reliable
synchronization.
(软中断由中断处理函数发起，并在处理函数返回之后执行。由于它们在线程上下文中运行，可能被其他线程抢占。不要假设 softirq 上下文在抢占关闭的情况下运行。这意味着你不能依赖诸如在进程上下文中使用 local_bh_disable() 之类的机制来保护 per-CPU 变量。因为在 PREEMPT_RT 下 softirq 处理函数是可抢占的，这种做法无法提供可靠的同步。)

If this kind of protection is required for performance reasons, consider using
local_lock_nested_bh(). On non-PREEMPT_RT kernels, this allows lockdep to
verify that bottom halves are disabled. On PREEMPT_RT systems, it adds the
necessary locking to ensure proper protection.
(如果出于性能原因需要这种保护，可以考虑使用 local_lock_nested_bh()。在非 PREEMPT_RT 内核上，它让 lockdep 验证下半部确实被禁用。在 PREEMPT_RT 系统上，它会加上必要的锁以确保正确的保护。)

Using local_lock_nested_bh() also makes the locking scope explicit and easier
for readers and maintainers to understand.
(使用 local_lock_nested_bh() 还使锁的作用范围显式化，更便于读者与维护者理解。)


per-CPU variables(per-CPU 变量)
-------------------------------

Protecting access to per-CPU variables solely by using preempt_disable() should
be avoided, especially if the critical section has unbounded runtime or may
call APIs that can sleep.
(应避免仅靠 preempt_disable() 来保护对 per-CPU 变量的访问，尤其是当临界区运行时间无界或可能调用可睡眠 API 时。)

If using a spinlock_t is considered too costly for performance reasons,
consider using local_lock_t. On non-PREEMPT_RT configurations, this introduces
no runtime overhead when lockdep is disabled. With lockdep enabled, it verifies
that the lock is only acquired in process context and never from softirq or
hard IRQ context.
(如果认为使用 spinlock_t 因性能原因代价过高，可以考虑使用 local_lock_t。在非 PREEMPT_RT 配置上，当 lockdep 被禁用时它不引入任何运行时开销。启用 lockdep 时，它会验证该锁只在进程上下文中获取，而从不在 softirq 或硬 IRQ 上下文中获取。)

On a PREEMPT_RT kernel, local_lock_t is implemented using a per-CPU spinlock_t,
which provides safe local protection for per-CPU data while keeping the system
preemptible.
(在 PREEMPT_RT 内核上，local_lock_t 用 per-CPU spinlock_t 实现，在保持系统可抢占的同时为 per-CPU 数据提供安全的本地保护。)

Because spinlock_t on PREEMPT_RT does not disable preemption, it cannot be used
to protect per-CPU data by relying on implicit preemption disabling. If this
inherited preemption disabling is essential and if local_lock_t cannot be used
due to performance constraints, brevity of the code, or abstraction boundaries
within an API then preempt_disable_nested() may be a suitable alternative. On
non-PREEMPT_RT kernels, it verifies with lockdep that preemption is already
disabled. On PREEMPT_RT, it explicitly disables preemption.
(由于 PREEMPT_RT 上的 spinlock_t 不禁用抢占，它无法通过依赖隐式抢占禁用来保护 per-CPU 数据。如果这种继承而来的抢占禁用是必需的，而由于性能约束、代码简洁性或 API 内的抽象边界等原因无法使用 local_lock_t，那么 preempt_disable_nested() 可能是一个合适的替代方案。在非 PREEMPT_RT 内核上，它用 lockdep 验证抢占已经关闭。在 PREEMPT_RT 上，它显式禁用抢占。)

Timers(定时器)
--------------

By default, an hrtimer is executed in hard interrupt context. The exception is
timers initialized with the HRTIMER_MODE_SOFT flag, which are executed in
softirq context.
(默认情况下，hrtimer 在硬中断上下文中执行。例外是使用 HRTIMER_MODE_SOFT 标志初始化的定时器，它们在 softirq 上下文中执行。)

On a PREEMPT_RT kernel, this behavior is reversed: hrtimers are executed in
softirq context by default, typically within the ktimersd thread. This thread
runs at the lowest real-time priority, ensuring it executes before any
SCHED_OTHER tasks but does not interfere with higher-priority real-time
threads. To explicitly request execution in hard interrupt context on
PREEMPT_RT, the timer must be marked with the HRTIMER_MODE_HARD flag.
(在 PREEMPT_RT 内核上，这一行为是相反的：hrtimer 默认在 softirq 上下文中执行，通常是在 ktimersd 线程内。该线程以最低实时优先级运行，确保它先于任何 SCHED_OTHER 任务执行，但不会干扰更高优先级的实时线程。要在 PREEMPT_RT 上显式请求在硬中断上下文中执行，必须用 HRTIMER_MODE_HARD 标志标记定时器。)

Memory allocation(内存分配)
---------------------------

The memory allocation APIs, such as kmalloc() and alloc_pages(), require a
gfp_t flag to indicate the allocation context. On non-PREEMPT_RT kernels, it is
necessary to use GFP_ATOMIC when allocating memory from interrupt context or
from sections where preemption is disabled. This is because the allocator must
not sleep in these contexts waiting for memory to become available.
(内存分配 API，如 kmalloc() 与 alloc_pages()，需要一个 gfp_t 标志来指明分配上下文。在非 PREEMPT_RT 内核上，从中断上下文或抢占被关闭的代码区分配内存时必须使用 GFP_ATOMIC。这是因为分配器在这些上下文中不能睡眠等待内存可用。)

However, this approach does not work on PREEMPT_RT kernels. The memory
allocator in PREEMPT_RT uses sleeping locks internally, which cannot be
acquired when preemption is disabled. Fortunately, this is generally not a
problem, because PREEMPT_RT moves most contexts that would traditionally run
with preemption or interrupts disabled into threaded context, where sleeping is
allowed.
(然而这种做法在 PREEMPT_RT 内核上行不通。PREEMPT_RT 的内存分配器内部使用可睡眠锁，而在抢占被关闭时无法获取。幸运的是，这通常不成问题，因为 PREEMPT_RT 把大多数传统上在抢占或中断关闭状态下运行的上下文移入了允许睡眠的线程上下文。)

What remains problematic is code that explicitly disables preemption or
interrupts. In such cases, memory allocation must be performed outside the
critical section.
(仍然有问题的是那些显式禁用抢占或中断的代码。这种情况下，内存分配必须在临界区之外进行。)

This restriction also applies to memory deallocation routines such as kfree()
and free_pages(), which may also involve internal locking and must not be
called from non-preemptible contexts.
(这一限制同样适用于 kfree() 与 free_pages() 等内存释放例程，它们可能同样涉及内部锁，不能从不可抢占上下文中调用。)

IRQ work(IRQ work)
------------------

The irq_work API provides a mechanism to schedule a callback in interrupt
context. It is designed for use in contexts where traditional scheduling is not
possible, such as from within NMI handlers or from inside the scheduler, where
using a workqueue would be unsafe.
(irq_work API 提供了一种在中断上下文中调度回调的机制。它设计用于无法进行传统调度的场景，例如在 NMI 处理函数内或调度器内部，此时使用 workqueue 会不安全。)

On non-PREEMPT_RT systems, all irq_work items are executed immediately in
interrupt context. Items marked with IRQ_WORK_LAZY are deferred until the next
timer tick but are still executed in interrupt context.
(在非 PREEMPT_RT 系统上，所有 irq_work 项都立即在中断上下文中执行。标记为 IRQ_WORK_LAZY 的项被推迟到下一次时钟滴答 (timer tick)，但仍在中断上下文中执行。)

On PREEMPT_RT systems, the execution model changes. Because irq_work callbacks
may acquire sleeping locks or have unbounded execution time, they are handled
in thread context by a per-CPU irq_work kernel thread. This thread runs at the
lowest real-time priority, ensuring it executes before any SCHED_OTHER tasks
but does not interfere with higher-priority real-time threads.

The exception are work items marked with IRQ_WORK_HARD_IRQ, which are still
executed in hard interrupt context. Lazy items (IRQ_WORK_LAZY) continue to be
deferred until the next timer tick and are also executed by the irq_work/
thread.
(在 PREEMPT_RT 系统上，执行模型发生了变化。由于 irq_work 回调可能获取可睡眠锁或执行时间无界，它们由一个 per-CPU 的 irq_work 内核线程在线程上下文中处理。该线程以最低实时优先级运行，确保它先于任何 SCHED_OTHER 任务执行，但不会干扰更高优先级的实时线程。)

The exception are work items marked with IRQ_WORK_HARD_IRQ, which are still
executed in hard interrupt context. Lazy items (IRQ_WORK_LAZY) continue to be
deferred until the next timer tick and are also executed by the irq_work/
thread.
(例外是标记为 IRQ_WORK_HARD_IRQ 的工作项，它们仍在硬中断上下文中执行。惰性项 (IRQ_WORK_LAZY) 继续被推迟到下一次时钟滴答，同样由 irq_work/ 线程执行。)

RCU callbacks(RCU 回调)
-----------------------

RCU callbacks are invoked by default in softirq context. Their execution is
important because, depending on the use case, they either free memory or ensure
progress in state transitions. Running these callbacks as part of the softirq
chain can lead to undesired situations, such as contention for CPU resources
with other SCHED_OTHER tasks when executed within ksoftirqd.
(默认情况下，RCU 回调在 softirq 上下文中被调用。它们的执行很重要，因为根据用例的不同，它们要么释放内存，要么保证状态转换取得进展。把这些回调作为 softirq 链的一部分运行可能导致不理想的情况，例如在 ksoftirqd 中执行时与其他 SCHED_OTHER 任务争抢 CPU 资源。)

To avoid running callbacks in softirq context, the RCU subsystem provides a
mechanism to execute them in process context instead. This behavior can be
enabled by setting the boot command-line parameter rcutree.use_softirq=0. This
setting is enforced in kernels configured with PREEMPT_RT.
(为避免在 softirq 上下文中运行回调，RCU 子系统提供了一种改为在进程上下文中执行它们的机制。可以通过设置启动命令行参数 rcutree.use_softirq=0 来启用该行为。在配置了 PREEMPT_RT 的内核中，此设置被强制执行。)

Spin until ready(自旋直至就绪)
==============================

The "spin until ready" pattern involves repeatedly checking (spinning on) the
state of a data structure until it becomes available. This pattern assumes that
preemption, soft interrupts, or interrupts are disabled. If the data structure
is marked busy, it is presumed to be in use by another CPU, and spinning should
eventually succeed as that CPU makes progress.
("自旋直至就绪" (spin until ready) 模式指反复检查 (自旋于) 数据结构的状态直至其可用。该模式假设抢占、软中断或中断已被禁用。如果数据结构被标记为忙，则假定它正被另一个 CPU 使用，随着该 CPU 取得进展，自旋最终应能成功。)

Some examples are hrtimer_cancel() or timer_delete_sync(). These functions
cancel timers that execute with interrupts or soft interrupts disabled. If a
thread attempts to cancel a timer and finds it active, spinning until the
callback completes is safe because the callback can only run on another CPU and
will eventually finish.
(例如 hrtimer_cancel() 或 timer_delete_sync()。这些函数取消在禁用中断或软中断的状态下执行的定时器。如果一个线程试图取消定时器并发现它正在活动，自旋等待回调完成是安全的，因为回调只能在另一个 CPU 上运行且最终会结束。)

On PREEMPT_RT kernels, however, timer callbacks run in thread context. This
introduces a challenge: a higher-priority thread attempting to cancel the timer
may preempt the timer callback thread. Since the scheduler cannot migrate the
callback thread to another CPU due to affinity constraints, spinning can result
in livelock even on multiprocessor systems.
(然而在 PREEMPT_RT 内核上，定时器回调在线程上下文中运行。这带来一个挑战：一个试图取消定时器的更高优先级线程可能抢占定时器回调线程。由于亲和性约束，调度器无法把回调线程迁移到另一个 CPU，即使在多处理器系统上自旋也可能导致活锁 (livelock)。)

To avoid this, both the canceling and callback sides must use a handshake
mechanism that supports priority inheritance. This allows the canceling thread
to suspend until the callback completes, ensuring forward progress without
risking livelock.
(为避免这一点，取消方与回调方都必须使用支持优先级继承的握手机制。这样取消线程可以挂起直至回调完成，在避免活锁风险的同时保证前进。)

In order to solve the problem at the API level, the sequence locks were extended
to allow a proper handover between the spinning reader and the maybe
blocked writer.
(为了在 API 层面解决这一问题，顺序锁被扩展以支持自旋读者与可能被阻塞的写者之间的正确交接。)

Sequence locks(顺序锁)
----------------------

Sequence counters and sequential locks are documented in
Documentation/locking/seqlock.rst.
(序列计数器与顺序锁记录在 Documentation/locking/seqlock.rst 中。)

The interface has been extended to ensure proper preemption states for the
writer and spinning reader contexts. This is achieved by embedding the writer
serialization lock directly into the sequence counter type, resulting in
composite types such as seqcount_spinlock_t or seqcount_mutex_t.
(该接口被扩展以确保写者与自旋读者上下文获得正确的抢占状态。这通过把写者串行化锁直接嵌入序列计数器类型来实现，由此产生 seqcount_spinlock_t 或 seqcount_mutex_t 等复合类型。)

These composite types allow readers to detect an ongoing write and actively
boost the writer’s priority to help it complete its update instead of spinning
and waiting for its completion.
(这些复合类型让读者能够检测到正在进行的写操作，并主动提升写者的优先级以帮助其完成更新，而不是自旋等待其完成。)

If the plain seqcount_t is used, extra care must be taken to synchronize the
reader with the writer during updates. The writer must ensure its update is
serialized and non-preemptible relative to the reader. This cannot be achieved
using a regular spinlock_t because spinlock_t on PREEMPT_RT does not disable
preemption. In such cases, using seqcount_spinlock_t is the preferred solution.
(如果使用普通的 seqcount_t，则在更新期间必须格外小心地同步读者与写者。写者必须确保其更新相对于读者是串行的且不可抢占。这无法用普通 spinlock_t 实现，因为 PREEMPT_RT 上的 spinlock_t 不禁用抢占。这种情况下，首选方案是使用 seqcount_spinlock_t。)

However, if there is no spinning involved i.e., if the reader only needs to
detect whether a write has started and not serialize against it then using
seqcount_t is reasonable.
(不过，如果不涉及自旋，即读者只需检测写操作是否已开始而无需与之串行化，那么使用 seqcount_t 是合理的。)
