========================================================================================================
CPU Scheduler implementation hints for architecture specific code(面向架构特定代码的 CPU 调度器实现提示)
========================================================================================================

	Nick Piggin, 2005
	(Nick Piggin，2005 年。)

Context switch(上下文切换)
==========================
1. Runqueue locking
By default, the switch_to arch function is called with the runqueue
locked. This is usually not a problem unless switch_to may need to
take the runqueue lock. This is usually due to a wake up operation in
the context switch.
(1. 运行队列锁。默认情况下，switch_to 架构函数在运行队列已加锁的情况
下被调用。除非 switch_to 可能需要获取运行队列锁，否则这通常不是问
题；而这通常是由上下文切换中的唤醒操作引起的。)

To request the scheduler call switch_to with the runqueue unlocked,
you must `#define __ARCH_WANT_UNLOCKED_CTXSW` in a header file
(typically the one where switch_to is defined).
(要请求调度器在运行队列未加锁的情况下调用 switch_to，必须在某个头文
件 (通常是定义 switch_to 的那个) 中 ``#define __ARCH_WANT_UNLOCKED_CTXSW``
。)

Unlocked context switches introduce only a very minor performance
penalty to the core scheduler implementation in the CONFIG_SMP case.
(在 CONFIG_SMP 情形下，未加锁的上下文切换只会给核心调度器实现带来非
常轻微的性能损失。)

CPU idle(CPU 空闲)
==================
Your cpu_idle routines need to obey the following rules:
(你的 cpu_idle 例程需要遵守以下规则：)

1. Preempt should now disabled over idle routines. Should only
   be enabled to call schedule() then disabled again.
   (1. 现在应在空闲例程期间禁用抢占。只应为了调用 schedule() 而启用，
   然后再次禁用。)

2. need_resched/TIF_NEED_RESCHED is only ever set, and will never
   be cleared until the running task has called schedule(). Idle
   threads need only ever query need_resched, and may never set or
   clear it.
   (2. need_resched/TIF_NEED_RESCHED 只会被置位，在正在运行的任务调用
   schedule() 之前永远不会被清除。空闲线程只需查询 need_resched，绝
   不能设置或清除它。)

3. When cpu_idle finds (need_resched() == 'true'), it should call
   schedule(). It should not call schedule() otherwise.
   (3. 当 cpu_idle 发现 (need_resched() == 'true') 时，应调用
   schedule()。除此之外不应调用 schedule()。)

4. The only time interrupts need to be disabled when checking
   need_resched is if we are about to sleep the processor until
   the next interrupt (this doesn't provide any protection of
   need_resched, it prevents losing an interrupt):
   (4. 在检查 need_resched 时唯一需要禁用中断的情形，是我们准备让处
   理器睡眠直到下一个中断之时 (这并不会对 need_resched 提供任何保
   护，而是为了防止丢失一个中断)：)

	4a. Common problem with this type of sleep appears to be::

	        local_irq_disable();
	        if (!need_resched()) {
	                local_irq_enable();
	                *** resched interrupt arrives here ***
	                __asm__("sleep until next interrupt");
	        }

	(4a. 这类睡眠的一个常见问题似乎是：)

5. TIF_POLLING_NRFLAG can be set by idle routines that do not
   need an interrupt to wake them up when need_resched goes high.
   In other words, they must be periodically polling need_resched,
   although it may be reasonable to do some background work or enter
   a low CPU priority.
   (5. 不需要中断来在 need_resched 变高时唤醒自己的空闲例程可以设置
   TIF_POLLING_NRFLAG。换句话说，它们必须周期性地轮询 need_resched，
   尽管做一些后台工作或进入较低的 CPU 优先级可能是合理的。)

      - 5a. If TIF_POLLING_NRFLAG is set, and we do decide to enter
	an interrupt sleep, it needs to be cleared then a memory
	barrier issued (followed by a test of need_resched with
	interrupts disabled, as explained in 3).
        (5a. 如果设置了 TIF_POLLING_NRFLAG，而我们又决定进入中断睡
        眠，则需要先清除它，然后发出一个内存屏障 (接着在禁用中断的情
        况下测试 need_resched，如第 3 点所述)。)

arch/x86/kernel/process.c has examples of both polling and
sleeping idle functions.
(arch/x86/kernel/process.c 中有轮询式与睡眠式空闲函数的示例。)


Possible arch/ problems(可能的 arch/ 问题)
==========================================

Possible arch problems I found (and either tried to fix or didn't):
(我发现的可能的架构问题 (有的尝试修复了，有的没有)：)

sparc - IRQs on at this point(?), change local_irq_save to _disable.
      - TODO: needs secondary CPUs to disable preempt (See #1)
(sparc——此时 IRQ 处于开启状态 (?)，把 local_irq_save 改为 _disable；
TODO: 需要辅助 CPU 禁用抢占 (见第 1 点)。)
