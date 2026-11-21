=========================================
IRQ-flags state tracing(IRQ 标志状态跟踪)
=========================================

:Author: started by Ingo Molnar <mingo@redhat.com>
(作者: 由 Ingo Molnar <mingo@redhat.com> 发起)

The "irq-flags tracing" feature "traces" hardirq and softirq state, in
that it gives interested subsystems an opportunity to be notified of
every hardirqs-off/hardirqs-on, softirqs-off/softirqs-on event that
happens in the kernel.
("irq-flags tracing" (IRQ 标志跟踪) 功能会"跟踪"硬中断 (hardirq) 与软中断
(softirq) 的状态：它让感兴趣的子系统有机会收到内核中发生的每一个
hardirqs-off/hardirqs-on、softirqs-off/softirqs-on 事件的通知。)

CONFIG_TRACE_IRQFLAGS_SUPPORT is needed for CONFIG_PROVE_SPIN_LOCKING
and CONFIG_PROVE_RW_LOCKING to be offered by the generic lock debugging
code. Otherwise only CONFIG_PROVE_MUTEX_LOCKING and
CONFIG_PROVE_RWSEM_LOCKING will be offered on an architecture - these
are locking APIs that are not used in IRQ context. (the one exception
for rwsems is worked around)
(通用锁调试代码要提供 CONFIG_PROVE_SPIN_LOCKING 和
CONFIG_PROVE_RW_LOCKING，就需要 CONFIG_TRACE_IRQFLAGS_SUPPORT。否则，某个
体系结构上只会提供 CONFIG_PROVE_MUTEX_LOCKING 和
CONFIG_PROVE_RWSEM_LOCKING——这些是用于非 IRQ 上下文的加锁 API。
(rwsem 的那一个例外已被绕过。))

Architecture support for this is certainly not in the "trivial"
category, because lots of lowlevel assembly code deal with irq-flags
state changes. But an architecture can be irq-flags-tracing enabled in a
rather straightforward and risk-free manner.
(这方面的体系结构支持当然不属于"简单"级别，因为大量低层汇编代码都会处理
IRQ 标志的状态变化。不过，启用某体系结构的 IRQ 标志跟踪仍然可以以一种相当
直接且无风险的方式进行。)

Architectures that want to support this need to do a couple of
code-organizational changes first:
(想要支持此功能的体系结构需要先做一些代码组织方面的改动：)

- add and enable TRACE_IRQFLAGS_SUPPORT in their arch level Kconfig file
  (在体系结构级别的 Kconfig 文件中添加并启用 TRACE_IRQFLAGS_SUPPORT。)

and then a couple of functional changes are needed as well to implement
irq-flags-tracing support:
(然后还需要做几处功能性的改动来实现 IRQ 标志跟踪支持：)

- in lowlevel entry code add (build-conditional) calls to the
  trace_hardirqs_off()/trace_hardirqs_on() functions. The lock validator
  closely guards whether the 'real' irq-flags matches the 'virtual'
  irq-flags state, and complains loudly (and turns itself off) if the
  two do not match. Usually most of the time for arch support for
  irq-flags-tracing is spent in this state: look at the lockdep
  complaint, try to figure out the assembly code we did not cover yet,
  fix and repeat. Once the system has booted up and works without a
  lockdep complaint in the irq-flags-tracing functions arch support is
  complete.
  (在低层入口代码中添加 (编译条件化的) 对
  trace_hardirqs_off()/trace_hardirqs_on() 函数的调用。锁验证器会严密监督
  "真实"的 IRQ 标志是否与"虚拟"的 IRQ 标志状态一致，若两者不匹配就会大声
  抱怨 (并把自己关闭)。通常，体系结构支持 IRQ 标志跟踪的大部分时间都花在
  这种状态上：查看 lockdep 抱怨，设法弄清我们尚未覆盖的汇编代码，修复并
  重复。一旦系统能够启动并且在 IRQ 标志跟踪函数中不再出现 lockdep 抱怨，
  体系结构支持即告完成。)
- if the architecture has non-maskable interrupts then those need to be
  excluded from the irq-tracing [and lock validation] mechanism via
  lockdep_off()/lockdep_on().
  (如果体系结构有不可屏蔽中断，则需要通过 lockdep_off()/lockdep_on() 把
  它们排除在 IRQ 跟踪 [和锁验证] 机制之外。)

In general there is no risk from having an incomplete irq-flags-tracing
implementation in an architecture: lockdep will detect that and will
turn itself off. I.e. the lock validator will still be reliable. There
should be no crashes due to irq-tracing bugs. (except if the assembly
changes break other code by modifying conditions or registers that
shouldn't be)
(总体而言，体系结构中存在不完整的 IRQ 标志跟踪实现并无风险：lockdep 会
检测到这一点并自行关闭。也就是说，锁验证器仍然是可靠的。不应出现由 IRQ
跟踪缺陷导致的崩溃。(除非汇编代码的修改改变了不该改变的条件或寄存器，
从而破坏了其他代码。))

