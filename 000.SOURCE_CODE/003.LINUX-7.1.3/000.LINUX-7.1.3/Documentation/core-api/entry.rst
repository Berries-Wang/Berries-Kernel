============================================================================================================
Entry/exit handling for exceptions, interrupts, syscalls and KVM(异常、中断、系统调用和 KVM 的进入/退出处理)
============================================================================================================

All transitions between execution domains require state updates which are
subject to strict ordering constraints. State updates are required for the
following:
(所有执行域之间的转换都需要进行状态更新，而这些更新受到严格的顺序约束。需要更新
状态的项目如下：)

  * Lockdep
    (Lockdep(锁依赖检查)。)
  * RCU / Context tracking
    (RCU / 上下文跟踪。)
  * Preemption counter
    (抢占计数器。)
  * Tracing
    (跟踪。)
  * Time accounting
    (时间统计。)

The update order depends on the transition type and is explained below in
the transition type sections: `Syscalls(系统调用)`_, `KVM(KVM 虚拟化)`_,
`Interrupts and regular exceptions(中断与常规异常)`_,
`NMI and NMI-like exceptions(NMI 与类 NMI 异常)`_.
(更新顺序取决于转换类型，将在下文相应的转换类型小节中说明：`Syscalls(系统调用)`_、
`KVM(KVM 虚拟化)`_、`Interrupts and regular exceptions(中断与常规异常)`_、
`NMI and NMI-like exceptions(NMI 与类 NMI 异常)`_。)

Non-instrumentable code - noinstr(不可插桩代码 - noinstr)
---------------------------------------------------------

Most instrumentation facilities depend on RCU, so instrumentation is prohibited
for entry code before RCU starts watching and exit code after RCU stops
watching. In addition, many architectures must save and restore register state,
which means that (for example) a breakpoint in the breakpoint entry code would
overwrite the debug registers of the initial breakpoint.
(大多数插桩设施都依赖 RCU，因此在 RCU 开始关注(watch)之前的进入代码，以及 RCU
停止关注之后的退出代码中，禁止插桩。此外，许多体系结构必须保存和恢复寄存器状态，
这意味着(例如)断点进入代码中的一个断点会覆盖最初断点的调试寄存器。)

Such code must be marked with the 'noinstr' attribute, placing that code into a
special section inaccessible to instrumentation and debug facilities. Some
functions are partially instrumentable, which is handled by marking them
noinstr and using instrumentation_begin() and instrumentation_end() to flag the
instrumentable ranges of code:
(这类代码必须标记 'noinstr' 属性，从而被放入插桩和调试设施无法触及的特殊代码段。
有些函数是部分可插桩的，处理办法是给它们标记 noinstr，并使用
instrumentation_begin() 和 instrumentation_end() 来标注其中可插桩的代码范围：)

.. code-block:: c

  noinstr void entry(void)
  {
  	handle_entry();     // <-- must be 'noinstr' or '__always_inline'
	...

	instrumentation_begin();
	handle_context();   // <-- instrumentable code
	instrumentation_end();

	...
	handle_exit();      // <-- must be 'noinstr' or '__always_inline'
  }

This allows verification of the 'noinstr' restrictions via objtool on
supported architectures.
(在受支持的体系结构上，可以通过 objtool 来验证 'noinstr' 约束。)

Invoking non-instrumentable functions from instrumentable context has no
restrictions and is useful to protect e.g. state switching which would
cause malfunction if instrumented.
(从可插桩上下文中调用不可插桩函数没有任何限制，这可以用来保护例如状态切换之类的
操作——如果被插桩，它们就会出现故障。)

All non-instrumentable entry/exit code sections before and after the RCU
state transitions must run with interrupts disabled.
(RCU 状态转换前后的所有不可插桩进入/退出代码段，都必须在禁用中断的情况下运行。)

Syscalls(系统调用)
------------------

Syscall-entry code starts in assembly code and calls out into low-level C code
after establishing low-level architecture-specific state and stack frames. This
low-level C code must not be instrumented. A typical syscall handling function
invoked from low-level assembly code looks like this:
(系统调用入口代码从汇编代码开始，在建立底层体系结构特定的状态和栈帧之后，再调用
底层的 C 代码。这段底层 C 代码不得插桩。由底层汇编代码调用的典型系统调用处理函数
如下所示：)

.. code-block:: c

  noinstr void syscall(struct pt_regs *regs, int nr)
  {
	arch_syscall_enter(regs);
	nr = syscall_enter_from_user_mode(regs, nr);

	instrumentation_begin();
	if (!invoke_syscall(regs, nr) && nr != -1)
	 	result_reg(regs) = __sys_ni_syscall(regs);
	instrumentation_end();

	syscall_exit_to_user_mode(regs);
  }

syscall_enter_from_user_mode() first invokes enter_from_user_mode() which
establishes state in the following order:
(syscall_enter_from_user_mode() 首先调用 enter_from_user_mode()，它按以下顺序
建立状态：)

  * Lockdep
    (Lockdep(锁依赖检查)。)
  * RCU / Context tracking
    (RCU / 上下文跟踪。)
  * Tracing
    (跟踪。)

and then invokes the various entry work functions like ptrace, seccomp, audit,
syscall tracing, etc. After all that is done, the instrumentable invoke_syscall
function can be invoked. The instrumentable code section then ends, after which
syscall_exit_to_user_mode() is invoked.
(随后调用各种入口工作函数，如 ptrace、seccomp、audit、系统调用跟踪等。这一切完成
之后，才能调用可插桩的 invoke_syscall 函数。然后可插桩代码段结束，接着调用
syscall_exit_to_user_mode()。)

syscall_exit_to_user_mode() handles all work which needs to be done before
returning to user space like tracing, audit, signals, task work etc. After
that it invokes exit_to_user_mode() which again handles the state
transition in the reverse order:
(syscall_exit_to_user_mode() 处理返回用户空间之前需要完成的所有工作，如跟踪、
audit、信号、task work 等。之后它调用 exit_to_user_mode()，后者按相反的顺序再次
处理状态转换：)

  * Tracing
    (跟踪。)
  * RCU / Context tracking
    (RCU / 上下文跟踪。)
  * Lockdep
    (Lockdep(锁依赖检查)。)

syscall_enter_from_user_mode() and syscall_exit_to_user_mode() are also
available as fine grained subfunctions in cases where the architecture code
has to do extra work between the various steps. In such cases it has to
ensure that enter_from_user_mode() is called first on entry and
exit_to_user_mode() is called last on exit.
(当体系结构代码需要在各个步骤之间做额外工作时，syscall_enter_from_user_mode()
和 syscall_exit_to_user_mode() 也以细粒度子函数的形式提供。在这种情况下，必须
确保进入时最先调用 enter_from_user_mode()，退出时最后调用
exit_to_user_mode()。)

Do not nest syscalls. Nested syscalls will cause RCU and/or context tracking
to print a warning.
(不要嵌套系统调用。嵌套的系统调用会导致 RCU 和/或上下文跟踪打印警告。)

KVM(KVM 虚拟化)
---------------

Entering or exiting guest mode is very similar to syscalls. From the host
kernel point of view the CPU goes off into user space when entering the
guest and returns to the kernel on exit.
(进入或退出客户机模式与系统调用非常相似。从宿主内核的角度看，CPU 进入客户机时
就像是去了用户空间，退出时则回到内核。)

guest_state_enter_irqoff() is a KVM-specific variant of exit_to_user_mode()
and guest_state_exit_irqoff() is the KVM variant of enter_from_user_mode().
The state operations have the same ordering.
(guest_state_enter_irqoff() 是 exit_to_user_mode() 的 KVM 专用变体，
guest_state_exit_irqoff() 则是 enter_from_user_mode() 的 KVM 变体。两者的状态
操作顺序相同。)

Task work handling is done separately for guest at the boundary of the
vcpu_run() loop via xfer_to_guest_mode_handle_work() which is a subset of
the work handled on return to user space.
(针对客户机的 task work 处理是在 vcpu_run() 循环边界上通过
xfer_to_guest_mode_handle_work() 单独完成的，它是返回用户空间时所处理工作的
一个子集。)

Do not nest KVM entry/exit transitions because doing so is nonsensical.
(不要嵌套 KVM 的进入/退出转换，因为这样做毫无意义。)

Interrupts and regular exceptions(中断与常规异常)
-------------------------------------------------

Interrupts entry and exit handling is slightly more complex than syscalls
and KVM transitions.
(中断的进入与退出处理比系统调用和 KVM 转换稍微复杂一些。)

If an interrupt is raised while the CPU executes in user space, the entry
and exit handling is exactly the same as for syscalls.
(如果中断在 CPU 于用户空间执行时发出，其进入和退出处理与系统调用完全相同。)

If the interrupt is raised while the CPU executes in kernel space the entry and
exit handling is slightly different. RCU state is only updated when the
interrupt is raised in the context of the CPU's idle task. Otherwise, RCU will
already be watching. Lockdep and tracing have to be updated unconditionally.
(如果中断在 CPU 于内核空间执行时发出，其进入和退出处理则稍有不同。只有当中断在
CPU 的 idle 任务上下文中发出时才更新 RCU 状态；否则 RCU 已经处于 watching 状态。
Lockdep 和跟踪则必须无条件更新。)

irqentry_enter() and irqentry_exit() provide the implementation for this.
(irqentry_enter() 和 irqentry_exit() 提供了这一机制的实现。)

The architecture-specific part looks similar to syscall handling:
(体系结构相关的部分与系统调用处理类似：)

.. code-block:: c

  noinstr void interrupt(struct pt_regs *regs, int nr)
  {
	arch_interrupt_enter(regs);
	state = irqentry_enter(regs);

	instrumentation_begin();

	irq_enter_rcu();
	invoke_irq_handler(regs, nr);
	irq_exit_rcu();

	instrumentation_end();

	irqentry_exit(regs, state);
  }

Note that the invocation of the actual interrupt handler is within a
irq_enter_rcu() and irq_exit_rcu() pair.
(注意，实际中断处理程序的调用位于 irq_enter_rcu() 与 irq_exit_rcu() 这一对调用
之间。)

irq_enter_rcu() updates the preemption count which makes in_hardirq()
return true, handles NOHZ tick state and interrupt time accounting. This
means that up to the point where irq_enter_rcu() is invoked in_hardirq()
returns false.
(irq_enter_rcu() 更新抢占计数使 in_hardirq() 返回 true，并处理 NOHZ tick 状态和
中断时间统计。这意味着在 irq_enter_rcu() 被调用之前，in_hardirq() 一直返回
false。)

irq_exit_rcu() handles interrupt time accounting, undoes the preemption
count update and eventually handles soft interrupts and NOHZ tick state.
(irq_exit_rcu() 处理中断时间统计，撤销抢占计数的更新，并最终处理软中断和 NOHZ
tick 状态。)

In theory, the preemption count could be updated in irqentry_enter(). In
practice, deferring this update to irq_enter_rcu() allows the preemption-count
code to be traced, while also maintaining symmetry with irq_exit_rcu() and
irqentry_exit(), which are described in the next paragraph. The only downside
is that the early entry code up to irq_enter_rcu() must be aware that the
preemption count has not yet been updated with the HARDIRQ_OFFSET state.
(理论上，抢占计数可以在 irqentry_enter() 中更新。但实践中，把这个更新推迟到
irq_enter_rcu() 可以让抢占计数相关代码被跟踪，同时与下一段要描述的
irq_exit_rcu() 和 irqentry_exit() 保持对称。唯一的缺点是，直到
irq_enter_rcu() 之前的早期进入代码必须知道抢占计数尚未加上 HARDIRQ_OFFSET
状态。)

Note that irq_exit_rcu() must remove HARDIRQ_OFFSET from the preemption count
before it handles soft interrupts, whose handlers must run in BH context rather
than irq-disabled context. In addition, irqentry_exit() might schedule, which
also requires that HARDIRQ_OFFSET has been removed from the preemption count.
(注意，irq_exit_rcu() 在处理软中断之前必须从抢占计数中移除 HARDIRQ_OFFSET，
因为软中断的处理程序必须在 BH 上下文中运行，而不是在禁用中断的上下文中。此外，
irqentry_exit() 可能会进行调度，这也要求 HARDIRQ_OFFSET 已经从抢占计数中移除。)

Even though interrupt handlers are expected to run with local interrupts
disabled, interrupt nesting is common from an entry/exit perspective. For
example, softirq handling happens within an irqentry_{enter,exit}() block with
local interrupts enabled. Also, although uncommon, nothing prevents an
interrupt handler from re-enabling interrupts.
(尽管中断处理程序预期在禁用本地中断的情况下运行，但从进入/退出处理的角度看，
中断嵌套很常见。例如，softirq 处理发生在 irqentry_{enter,exit}() 块内，此时本地
中断是启用的。另外，虽然不常见，但没有什么能阻止中断处理程序重新启用中断。)

Interrupt entry/exit code doesn't strictly need to handle reentrancy, since it
runs with local interrupts disabled. But NMIs can happen anytime, and a lot of
the entry code is shared between the two.
(中断进入/退出代码并非严格要求处理重入，因为它在禁用本地中断的情况下运行。但
NMI 可能在任何时刻发生，而这两者之间共享了大量进入代码。)

NMI and NMI-like exceptions(NMI 与类 NMI 异常)
----------------------------------------------

NMIs and NMI-like exceptions (machine checks, double faults, debug
interrupts, etc.) can hit any context and must be extra careful with
the state.
(NMI 和类 NMI 异常(机器检查、双重故障、调试中断等)可能发生在任何上下文中，因此
必须格外小心地处理状态。)

State changes for debug exceptions and machine-check exceptions depend on
whether these exceptions happened in user-space (breakpoints or watchpoints) or
in kernel mode (code patching). From user-space, they are treated like
interrupts, while from kernel mode they are treated like NMIs.
(调试异常和机器检查异常的状态变更取决于这些异常是发生在用户空间(断点或观察点)
还是内核模式(代码打补丁)。来自用户空间时，它们按中断对待；来自内核模式时，则按
NMI 对待。)

NMIs and other NMI-like exceptions handle state transitions without
distinguishing between user-mode and kernel-mode origin.
(NMI 和其他类 NMI 异常处理状态转换时不区分来自用户模式还是内核模式。)

The state update on entry is handled in irqentry_nmi_enter() which updates
state in the following order:
(进入时的状态更新由 irqentry_nmi_enter() 处理，它按以下顺序更新状态：)

  * Preemption counter
    (抢占计数器。)
  * Lockdep
    (Lockdep(锁依赖检查)。)
  * RCU / Context tracking
    (RCU / 上下文跟踪。)
  * Tracing
    (跟踪。)

The exit counterpart irqentry_nmi_exit() does the reverse operation in the
reverse order.
(退出时对应的 irqentry_nmi_exit() 以相反的顺序执行相反的操作。)

Note that the update of the preemption counter has to be the first
operation on enter and the last operation on exit. The reason is that both
lockdep and RCU rely on in_nmi() returning true in this case. The
preemption count modification in the NMI entry/exit case must not be
traced.
(注意，抢占计数的更新必须是进入时的第一个操作、退出时的最后一个操作。原因是
lockdep 和 RCU 都依赖 in_nmi() 在这种情况下返回 true。NMI 进入/退出情形下的
抢占计数修改不得被跟踪。)

Architecture-specific code looks like this:
(体系结构相关代码如下所示：)

.. code-block:: c

  noinstr void nmi(struct pt_regs *regs)
  {
	arch_nmi_enter(regs);
	state = irqentry_nmi_enter(regs);

	instrumentation_begin();
	nmi_handler(regs);
	instrumentation_end();

	irqentry_nmi_exit(regs);
  }

and for e.g. a debug exception it can look like this:
(例如，对于调试异常，它可能如下所示：)

.. code-block:: c

  noinstr void debug(struct pt_regs *regs)
  {
	arch_nmi_enter(regs);

	debug_regs = save_debug_regs();

	if (user_mode(regs)) {
		state = irqentry_enter(regs);

		instrumentation_begin();
		user_mode_debug_handler(regs, debug_regs);
		instrumentation_end();

		irqentry_exit(regs, state);
  	} else {
  		state = irqentry_nmi_enter(regs);

		instrumentation_begin();
		kernel_mode_debug_handler(regs, debug_regs);
		instrumentation_end();

		irqentry_nmi_exit(regs, state);
	}
  }

There is no combined irqentry_nmi_if_kernel() function available as the
above cannot be handled in an exception-agnostic way.
(并不存在一个合并的 irqentry_nmi_if_kernel() 函数，因为上述逻辑无法以与异常类型
无关的方式统一处理。)

NMIs can happen in any context. For example, an NMI-like exception triggered
while handling an NMI. So NMI entry code has to be reentrant and state updates
need to handle nesting.
(NMI 可能发生在任何上下文中。例如，在处理一个 NMI 的过程中又触发了类 NMI
异常。因此 NMI 进入代码必须支持重入，状态更新也需要处理嵌套。)
