.. SPDX-License-Identifier: GPL-2.0

========================================================================
Porting an architecture to support PREEMPT_RT(移植架构以支持 PREEMPT_RT)
========================================================================

:Author: Sebastian Andrzej Siewior <bigeasy@linutronix.de>

(作者: Sebastian Andrzej Siewior <bigeasy@linutronix.de>)

This list outlines the architecture specific requirements that must be
implemented in order to enable PREEMPT_RT. Once all required features are
implemented, ARCH_SUPPORTS_RT can be selected in architecture’s Kconfig to make
PREEMPT_RT selectable.
Many prerequisites (genirq support for example) are enforced by the common code
and are omitted here.
(此列表列出了为启用 PREEMPT_RT 而必须实现的架构特定要求。一旦所有必需的特性都已实现，就可以在架构的 Kconfig 中选择 ARCH_SUPPORTS_RT，使 PREEMPT_RT 可被选择。许多先决条件（例如 genirq 支持）由公共代码强制执行，此处不再列出。)

The optional features are not strictly required but it is worth to consider
them.
(可选特性并非严格要求，但值得考虑。)

Requirements(必备要求)
----------------------

Forced threaded interrupts
  CONFIG_IRQ_FORCED_THREADING must be selected. Any interrupts that must
  remain in hard-IRQ context must be marked with IRQF_NO_THREAD. This
  requirement applies for instance to clocksource event interrupts,
  perf interrupts and cascading interrupt-controller handlers.
  (必须选择 CONFIG_IRQ_FORCED_THREADING。任何必须保留在硬中断（hard-IRQ）上下文中的中断都必须用 IRQF_NO_THREAD 标记。例如，此要求适用于 clocksource 事件中断、perf 中断和级联中断控制器处理程序。)

PREEMPTION support
  Kernel preemption must be supported and requires that
  CONFIG_ARCH_NO_PREEMPT remain unselected. Scheduling requests, such as those
  issued from an interrupt or other exception handler, must be processed
  immediately.
  (必须支持内核抢占，并且要求 CONFIG_ARCH_NO_PREEMPT 保持未被选择。调度请求（例如从中断或其他异常处理程序发出的请求）必须立即得到处理。)

POSIX CPU timers and KVM
  POSIX CPU timers must expire from thread context rather than directly within
  the timer interrupt. This behavior is enabled by setting the configuration
  option CONFIG_HAVE_POSIX_CPU_TIMERS_TASK_WORK.
  When virtualization support, such as KVM, is enabled,
  CONFIG_VIRT_XFER_TO_GUEST_WORK must also be set to ensure
  that any pending work, such as POSIX timer expiration, is handled before
  transitioning into guest mode.
  (POSIX CPU 定时器必须从线程上下文到期，而不是直接在定时器中断内到期。此行为通过设置配置选项 CONFIG_HAVE_POSIX_CPU_TIMERS_TASK_WORK 启用。当启用了诸如 KVM 之类的虚拟化支持时，还必须设置 CONFIG_VIRT_XFER_TO_GUEST_WORK，以确保在进入客户机模式之前处理任何待处理的工作，例如 POSIX 定时器到期。)

Hard-IRQ and Soft-IRQ stacks
  Soft interrupts are handled in the thread context in which they are raised. If
  a soft interrupt is triggered from hard-IRQ context, its execution is deferred
  to the ksoftirqd thread. Preemption is never disabled during soft interrupt
  handling, which makes soft interrupts preemptible.
  (软中断在引发它们的线程上下文中处理。如果软中断是在硬中断上下文中触发的，其执行会被推迟到 ksoftirqd 线程。软中断处理期间从不禁用抢占，这使软中断是可抢占的。)
  If an architecture provides a custom __do_softirq() implementation that uses a
  separate stack, it must select CONFIG_HAVE_SOFTIRQ_ON_OWN_STACK. The
  functionality should only be enabled when CONFIG_SOFTIRQ_ON_OWN_STACK is set.
  (如果架构提供了使用独立栈的自定义 __do_softirq() 实现，它必须选择 CONFIG_HAVE_SOFTIRQ_ON_OWN_STACK。只有当设置了 CONFIG_SOFTIRQ_ON_OWN_STACK 时才应启用该功能。)

FPU and SIMD access in kernel mode
  FPU and SIMD registers are typically not used in kernel mode and are therefore
  not saved during kernel preemption. As a result, any kernel code that uses
  these registers must be enclosed within a kernel_fpu_begin() and
  kernel_fpu_end() section.
  The kernel_fpu_begin() function usually invokes local_bh_disable() to prevent
  interruptions from softirqs and to disable regular preemption. This allows the
  protected code to run safely in both thread and softirq contexts.
  (FPU 和 SIMD 寄存器通常不在内核模式下使用，因此在内核抢占期间不会被保存。因此，任何使用这些寄存器的内核代码都必须包裹在 kernel_fpu_begin() 和 kernel_fpu_end() 段之内。kernel_fpu_begin() 函数通常会调用 local_bh_disable()，以防止 softirq 的干扰并禁用普通抢占。这使受保护的代码能够在线程和 softirq 上下文中安全运行。)
  On PREEMPT_RT kernels, however, kernel_fpu_begin() must not call
  local_bh_disable(). Instead, it should use preempt_disable(), since softirqs
  are always handled in thread context under PREEMPT_RT. In this case, disabling
  preemption alone is sufficient.
  (但在 PREEMPT_RT 内核上，kernel_fpu_begin() 不得调用 local_bh_disable() 。相反，它应使用 preempt_disable() ，因为在 PREEMPT_RT 下 softirq 总是在线程上下文中处理。在这种情况下，仅禁用抢占就足够了。)
  The crypto subsystem operates on memory pages and requires users to "walk and
  map" these pages while processing a request. This operation must occur outside
  the kernel_fpu_begin()/ kernel_fpu_end() section because it requires preemption
  to be enabled. These preemption points are generally sufficient to avoid
  excessive scheduling latency.
  (crypto 子系统操作内存页，并要求使用者在处理请求时"遍历并映射"这些页。此操作必须发生在 kernel_fpu_begin()/ kernel_fpu_end() 段之外，因为它需要启用抢占。这些抢占点通常足以避免过度的调度延迟。)

Exception handlers
  Exception handlers, such as the page fault handler, typically enable interrupts
  early, before invoking any generic code to process the exception. This is
  necessary because handling a page fault may involve operations that can sleep.
  Enabling interrupts is especially important on PREEMPT_RT, where certain
  locks, such as spinlock_t, become sleepable. For example, handling an
  invalid opcode may result in sending a SIGILL signal to the user task. A
  debug exception will send a SIGTRAP signal.
  (异常处理程序（例如缺页异常处理程序）通常会提前启用中断，然后再调用任何处理该异常的通用代码。这是必要的，因为处理缺页异常可能涉及可以睡眠的操作。在 PREEMPT_RT 上启用中断尤为重要，因为某些锁（例如 spinlock_t）会变为可睡眠的。例如，处理无效操作码可能导致向用户任务发送 SIGILL 信号。调试异常则会发送 SIGTRAP 信号。)
  In both cases, if the exception occurred in user space, it is safe to enable
  interrupts early. Sending a signal requires both interrupts and kernel
  preemption to be enabled.
  (在这两种情况下，如果异常发生在用户空间，提前启用中断是安全的。发送信号需要中断和内核抢占都被启用。)

Optional features(可选特性)
---------------------------

Timer and clocksource
  A high-resolution clocksource and clockevents device are recommended. The
  clockevents device should support the CLOCK_EVT_FEAT_ONESHOT feature for
  optimal timer behavior. In most cases, microsecond-level accuracy is
  sufficient
  (建议使用高分辨率 clocksource 和 clockevents 设备。clockevents 设备应支持 CLOCK_EVT_FEAT_ONESHOT 特性以获得最佳定时器行为。在大多数情况下，微秒级的精度就足够了)

Lazy preemption
  This mechanism allows an in-kernel scheduling request for non-real-time tasks
  to be delayed until the task is about to return to user space. It helps avoid
  preempting a task that holds a sleeping lock at the time of the scheduling
  request.
  With CONFIG_GENERIC_IRQ_ENTRY enabled, supporting this feature requires
  defining a bit for TIF_NEED_RESCHED_LAZY, preferably near TIF_NEED_RESCHED.
  (此机制允许把针对非实时任务的内核内调度请求延迟到任务即将返回用户空间时。它有助于避免抢占在发出调度请求时持有可睡眠锁的任务。在启用 CONFIG_GENERIC_IRQ_ENTRY 的情况下，支持此特性需要为 TIF_NEED_RESCHED_LAZY 定义一个位，最好靠近 TIF_NEED_RESCHED。)

Serial console with NBCON
  With PREEMPT_RT enabled, all console output is handled by a dedicated thread
  rather than directly from the context in which printk() is invoked. This design
  allows printk() to be safely used in atomic contexts.
  However, this also means that if the kernel crashes and cannot switch to the
  printing thread, no output will be visible preventing the system from printing
  its final messages.
  There are exceptions for immediate output, such as during panic() handling. To
  support this, the console driver must implement new-style lock handling. This
  involves setting the CON_NBCON flag in console::flags and providing
  implementations for the write_atomic, write_thread, device_lock, and
  device_unlock callbacks.
  (启用 PREEMPT_RT 后，所有控制台输出都由专用线程处理，而不是直接从调用 printk() 的上下文输出。这一设计使 printk() 可以安全地在原子上下文中使用。然而，这也意味着如果内核崩溃且无法切换到打印线程，将看不到任何输出，导致系统无法打印其最后的消息。对于立即输出存在例外情况，例如在 panic() 处理期间。为支持这一点，控制台驱动程序必须实现新式锁处理。这涉及在 console::flags 中设置 CON_NBCON 标志，并为 write_atomic、write_thread、device_lock 和 device_unlock 回调提供实现。)
