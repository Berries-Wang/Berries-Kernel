==============================================
CPU hotplug in the Kernel(内核中的 CPU 热插拔)
==============================================

:Date: September, 2021
:Author: Sebastian Andrzej Siewior <bigeasy@linutronix.de>,
         Rusty Russell <rusty@rustcorp.com.au>,
         Srivatsa Vaddagiri <vatsa@in.ibm.com>,
         Ashok Raj <ashok.raj@intel.com>,
         Joel Schopp <jschopp@austin.ibm.com>,
	 Thomas Gleixner <tglx@kernel.org>

Introduction(引言)
==================

Modern advances in system architectures have introduced advanced error
reporting and correction capabilities in processors. There are couple OEMS that
support NUMA hardware which are hot pluggable as well, where physical node
insertion and removal require support for CPU hotplug.
(系统架构的现代进步为处理器引入了高级的错误报告与纠正能力。有一些 OEM 支持
同样可热插拔的 NUMA 硬件，其中物理节点的插入与移除需要 CPU 热插拔的支持。)

Such advances require CPUs available to a kernel to be removed either for
provisioning reasons, or for RAS purposes to keep an offending CPU off
system execution path. Hence the need for CPU hotplug support in the
Linux kernel.
(这类进步要求内核可用的 CPU 能够被移除，或者出于供给调配的原因，或者出于 RAS
的目的让肇事的 CPU 远离系统执行路径。因此 Linux 内核需要 CPU 热插拔支持。)

A more novel use of CPU-hotplug support is its use today in suspend resume
support for SMP. Dual-core and HT support makes even a laptop run SMP kernels
which didn't support these methods.
(CPU 热插拔支持的一种较新颖的用法，是如今在 SMP 的挂起/恢复支持中的使用。
双核与 HT 支持使得连笔记本都能运行不支持这些方法的 SMP 内核。)


Command Line Switches(命令行开关)
=================================
``maxcpus=n``
  Restrict boot time CPUs to *n*. Say if you have four CPUs, using
  ``maxcpus=2`` will only boot two. You can choose to bring the
  other CPUs later online.
  (将启动时的 CPU 数量限制为 *n*。比如说，如果你有四个 CPU，使用
  ``maxcpus=2`` 将只启动两个。你可以稍后再选择把其余的 CPU 上线。)

``nr_cpus=n``
  Restrict the total amount of CPUs the kernel will support. If the number
  supplied here is lower than the number of physically available CPUs, then
  those CPUs can not be brought online later.
  (限制内核所支持的 CPU 总数。如果这里给出的数字低于物理可用 CPU 的数量，那么
  那些 CPU 以后将无法上线。)

``possible_cpus=n``
  This option sets ``possible_cpus`` bits in ``cpu_possible_mask``.
  (此选项设置 ``cpu_possible_mask`` 中的 ``possible_cpus`` 位。)

  This option is limited to the X86 and S390 architecture.
  (此选项仅限于 X86 与 S390 架构。)

``cpu0_hotplug``
  Allow to shutdown CPU0.
  (允许关闭 CPU0。)

  This option is limited to the X86 architecture.
  (此选项仅限于 X86 架构。)

CPU maps(CPU 掩码)
==================

``cpu_possible_mask``
  Bitmap of possible CPUs that can ever be available in the
  system. This is used to allocate some boot time memory for per_cpu variables
  that aren't designed to grow/shrink as CPUs are made available or removed.
  Once set during boot time discovery phase, the map is static, i.e no bits
  are added or removed anytime. Trimming it accurately for your system needs
  upfront can save some boot time memory.
  (系统中将来可能可用的 CPU 的位图。它用于为 per_cpu 变量分配一些启动期内存，
  这些变量并不随 CPU 的可用或移除而增缩。在启动时的发现阶段设置之后，该掩码
  就是静态的，即任何时候都不会增删位。提前为你的系统需求精确裁剪它可以节省
  一些启动期内存。)

``cpu_online_mask``
  Bitmap of all CPUs currently online. Its set in ``__cpu_up()``
  after a CPU is available for kernel scheduling and ready to receive
  interrupts from devices. Its cleared when a CPU is brought down using
  ``__cpu_disable()``, before which all OS services including interrupts are
  migrated to another target CPU.
  (当前在线的所有 CPU 的位图。在某个 CPU 可参与内核调度、并准备好接收来自
  设备的中断之后，``__cpu_up()`` 会设置它。在使用 ``__cpu_disable()`` 将某个
  CPU 下线时会清除它，在此之前，包括中断在内的所有 OS 服务都已迁移到另一个
  目标 CPU。)

``cpu_present_mask``
  Bitmap of CPUs currently present in the system. Not all
  of them may be online. When physical hotplug is processed by the relevant
  subsystem (e.g ACPI) can change and new bit either be added or removed
  from the map depending on the event is hot-add/hot-remove. There are currently
  no locking rules as of now. Typical usage is to init topology during boot,
  at which time hotplug is disabled.
  (当前系统中在位的 CPU 的位图。它们未必都在线。当相关子系统（例如 ACPI）处理
  物理热插拔时，该掩码可能改变：根据事件是热添加还是热移除，会加入或移除新的
  位。目前尚无加锁规则。典型用法是在启动期间初始化拓扑，此时热插拔是禁用的。)

You really don't need to manipulate any of the system CPU maps. They should
be read-only for most use. When setting up per-cpu resources almost always use
``cpu_possible_mask`` or ``for_each_possible_cpu()`` to iterate. To macro
``for_each_cpu()`` can be used to iterate over a custom CPU mask.
(你其实不需要操作任何系统 CPU 掩码。对大多数用途而言，它们应当是只读的。设置
per-cpu 资源时，几乎总是应使用 ``cpu_possible_mask`` 或
``for_each_possible_cpu()`` 来遍历。宏 ``for_each_cpu()`` 可用于遍历自定义的
CPU 掩码。)

Never use anything other than ``cpumask_t`` to represent bitmap of CPUs.
(千万不要用 ``cpumask_t`` 以外的任何东西来表示 CPU 的位图。)


Using CPU hotplug(使用 CPU 热插拔)
==================================

The kernel option *CONFIG_HOTPLUG_CPU* needs to be enabled. It is currently
available on multiple architectures including ARM, MIPS, PowerPC and X86. The
configuration is done via the sysfs interface::
(需要启用内核选项 *CONFIG_HOTPLUG_CPU*。它目前可用于包括 ARM、MIPS、PowerPC
与 X86 在内的多种架构。配置通过 sysfs 接口完成：)

 $ ls -lh /sys/devices/system/cpu
 total 0
 drwxr-xr-x  9 root root    0 Dec 21 16:33 cpu0
 drwxr-xr-x  9 root root    0 Dec 21 16:33 cpu1
 drwxr-xr-x  9 root root    0 Dec 21 16:33 cpu2
 drwxr-xr-x  9 root root    0 Dec 21 16:33 cpu3
 drwxr-xr-x  9 root root    0 Dec 21 16:33 cpu4
 drwxr-xr-x  9 root root    0 Dec 21 16:33 cpu5
 drwxr-xr-x  9 root root    0 Dec 21 16:33 cpu6
 drwxr-xr-x  9 root root    0 Dec 21 16:33 cpu7
 drwxr-xr-x  2 root root    0 Dec 21 16:33 hotplug
 -r--r--r--  1 root root 4.0K Dec 21 16:33 offline
 -r--r--r--  1 root root 4.0K Dec 21 16:33 online
 -r--r--r--  1 root root 4.0K Dec 21 16:33 possible
 -r--r--r--  1 root root 4.0K Dec 21 16:33 present

(需要启用内核选项 *CONFIG_HOTPLUG_CPU*。它目前可用于包括 ARM、MIPS、
PowerPC 与 X86 在内的多种架构。配置通过 sysfs 接口完成：)

The files *offline*, *online*, *possible*, *present* represent the CPU masks.
Each CPU folder contains an *online* file which controls the logical on (1) and
off (0) state. To logically shutdown CPU4::

 $ echo 0 > /sys/devices/system/cpu/cpu4/online
  smpboot: CPU 4 is now offline

(文件 *offline*、*online*、*possible*、*present* 表示各 CPU 掩码。每个 CPU
文件夹都包含一个 *online* 文件，控制逻辑上的开启（1）与关闭（0）状态。要把
CPU4 逻辑关闭：)

Once the CPU is shutdown, it will be removed from */proc/interrupts*,
*/proc/cpuinfo* and should also not be shown visible by the *top* command. To
bring CPU4 back online::

 $ echo 1 > /sys/devices/system/cpu/cpu4/online
 smpboot: Booting Node 0 Processor 4 APIC 0x1

(一旦 CPU 被关闭，它就会从 */proc/interrupts* 与 */proc/cpuinfo* 中消失，并且
也不应再被 *top* 命令显示出来。要把 CPU4 重新上线：)

The CPU is usable again. This should work on all CPUs, but CPU0 is often special
and excluded from CPU hotplug.
(CPU 又可用了。这在所有 CPU 上都应该可行，但 CPU0 往往比较特殊，被排除在
CPU 热插拔之外。)

The CPU hotplug coordination(CPU 热插拔的协调)
==============================================

The offline case(下线情形)
--------------------------

Once a CPU has been logically shutdown the teardown callbacks of registered
hotplug states will be invoked, starting with ``CPUHP_ONLINE`` and terminating
at state ``CPUHP_OFFLINE``. This includes:
(一旦某个 CPU 被逻辑关闭，已注册热插拔状态的 teardown 回调就会被调用，从
``CPUHP_ONLINE`` 开始，到状态 ``CPUHP_OFFLINE`` 结束。这包括：)

* If tasks are frozen due to a suspend operation then *cpuhp_tasks_frozen*
  will be set to true.
  (如果任务因挂起操作而被冻结，则 *cpuhp_tasks_frozen* 会被置为 true。)
* All processes are migrated away from this outgoing CPU to new CPUs.
  The new CPU is chosen from each process' current cpuset, which may be
  a subset of all online CPUs.
  (所有进程都会从这个即将离线的 CPU 迁移到新的 CPU。新 CPU 从每个进程当前的
  cpuset 中挑选，该 cpuset 可能是所有在线 CPU 的一个子集。)
* All interrupts targeted to this CPU are migrated to a new CPU
  (所有指向该 CPU 的中断都会迁移到新的 CPU。)
* timers are also migrated to a new CPU
  (定时器也会迁移到新的 CPU。)
* Once all services are migrated, kernel calls an arch specific routine
  ``__cpu_disable()`` to perform arch specific cleanup.
  (所有服务迁移完毕后，内核会调用与架构相关的例程 ``__cpu_disable()`` 执行
  架构特定的清理。)


The CPU hotplug API(CPU 热插拔 API)
===================================

CPU hotplug state machine(CPU 热插拔状态机)
-------------------------------------------

CPU hotplug uses a trivial state machine with a linear state space from
CPUHP_OFFLINE to CPUHP_ONLINE. Each state has a startup and a teardown
callback.
(CPU 热插拔使用一个简单的状态机，其状态空间从 CPUHP_OFFLINE 到
CPUHP_ONLINE 线性排布。每个状态都有一个 startup 回调和一个 teardown 回调。)

When a CPU is onlined, the startup callbacks are invoked sequentially until
the state CPUHP_ONLINE is reached. They can also be invoked when the
callbacks of a state are set up or an instance is added to a multi-instance
state.
(当 CPU 上线时，startup 回调按顺序被调用，直到到达 CPUHP_ONLINE 状态。当某个
状态的回调被建立、或向多实例状态添加实例时，这些回调也会被调用。)

When a CPU is offlined the teardown callbacks are invoked in the reverse
order sequentially until the state CPUHP_OFFLINE is reached. They can also
be invoked when the callbacks of a state are removed or an instance is
removed from a multi-instance state.
(当 CPU 下线时，teardown 回调按相反顺序被调用，直到到达 CPUHP_OFFLINE 状态。
当某个状态的回调被移除、或从多实例状态删除实例时，这些回调也会被调用。)

If a usage site requires only a callback in one direction of the hotplug
operations (CPU online or CPU offline) then the other not-required callback
can be set to NULL when the state is set up.
(如果使用点只需要热插拔操作某一方向（CPU 上线或 CPU 下线）的回调，那么在设置
状态时可以把不需要的另一个回调设为 NULL。)

The state space is divided into three sections:
(状态空间被划分为三个区段：)

* The PREPARE section
  (PREPARE 区段)

  The PREPARE section covers the state space from CPUHP_OFFLINE to
  CPUHP_BRINGUP_CPU.
  (PREPARE 区段覆盖从 CPUHP_OFFLINE 到 CPUHP_BRINGUP_CPU 的状态空间。)

  The startup callbacks in this section are invoked before the CPU is
  started during a CPU online operation. The teardown callbacks are invoked
  after the CPU has become dysfunctional during a CPU offline operation.
  (在 CPU 上线操作中，该区段的 startup 回调在 CPU 被启动之前调用。在 CPU 下线
  操作中，teardown 回调在 CPU 已经失效之后调用。)

  The callbacks are invoked on a control CPU as they can't obviously run on
  the hotplugged CPU which is either not yet started or has become
  dysfunctional already.
  (这些回调在控制 CPU 上调用，因为它们显然无法在热插拔的 CPU 上运行——该 CPU
  要么尚未启动，要么已经失效。)

  The startup callbacks are used to setup resources which are required to
  bring a CPU successfully online. The teardown callbacks are used to free
  resources or to move pending work to an online CPU after the hotplugged
  CPU became dysfunctional.
  (startup 回调用于设置让 CPU 成功上线所需的资源。teardown 回调用于释放资源，
  或者在热插拔的 CPU 失效之后把未完成的工作转移到在线 CPU 上。)

  The startup callbacks are allowed to fail. If a callback fails, the CPU
  online operation is aborted and the CPU is brought down to the previous
  state (usually CPUHP_OFFLINE) again.
  (startup 回调允许失败。如果某个回调失败，CPU 上线操作会被中止，CPU 被拉回
  到前一个状态（通常是 CPUHP_OFFLINE）。)

  The teardown callbacks in this section are not allowed to fail.
  (该区段的 teardown 回调不允许失败。)

* The STARTING section
  (STARTING 区段)

  The STARTING section covers the state space between CPUHP_BRINGUP_CPU + 1
  and CPUHP_AP_ONLINE.
  (STARTING 区段覆盖 CPUHP_BRINGUP_CPU + 1 与 CPUHP_AP_ONLINE 之间的状态
  空间。)

  The startup callbacks in this section are invoked on the hotplugged CPU
  with interrupts disabled during a CPU online operation in the early CPU
  setup code. The teardown callbacks are invoked with interrupts disabled
  on the hotplugged CPU during a CPU offline operation shortly before the
  CPU is completely shut down.
  (在 CPU 上线操作中，该区段的 startup 回调在早期的 CPU 设置代码中于热插拔的
  CPU 上、在中断禁用的情况下调用。在 CPU 下线操作中，teardown 回调在 CPU 被
  彻底关闭之前不久，于热插拔的 CPU 上、在中断禁用的情况下调用。)

  The callbacks in this section are not allowed to fail.
  (该区段的回调不允许失败。)

  The callbacks are used for low level hardware initialization/shutdown and
  for core subsystems.
  (这些回调用于底层硬件的初始化/关闭以及核心子系统。)

* The ONLINE section
  (ONLINE 区段)

  The ONLINE section covers the state space between CPUHP_AP_ONLINE + 1 and
  CPUHP_ONLINE.
  (ONLINE 区段覆盖 CPUHP_AP_ONLINE + 1 与 CPUHP_ONLINE 之间的状态空间。)

  The startup callbacks in this section are invoked on the hotplugged CPU
  during a CPU online operation. The teardown callbacks are invoked on the
  hotplugged CPU during a CPU offline operation.
  (在 CPU 上线操作中，该区段的 startup 回调在热插拔的 CPU 上调用。在 CPU 下线
  操作中，teardown 回调在热插拔的 CPU 上调用。)

  The callbacks are invoked in the context of the per CPU hotplug thread,
  which is pinned on the hotplugged CPU. The callbacks are invoked with
  interrupts and preemption enabled.
  (这些回调在 per CPU 热插拔线程的上下文中调用，该线程被钉在热插拔的 CPU 上。
  回调在中断与抢占开启的情况下调用。)

  The callbacks are allowed to fail. When a callback fails the hotplug
  operation is aborted and the CPU is brought back to the previous state.
  (这些回调允许失败。当某个回调失败时，热插拔操作被中止，CPU 被拉回到前一个
  状态。)

CPU online/offline operations(CPU 上线/下线操作)
------------------------------------------------

A successful online operation looks like this::

  [CPUHP_OFFLINE]
  [CPUHP_OFFLINE + 1]->startup()       -> success
  [CPUHP_OFFLINE + 2]->startup()       -> success
  [CPUHP_OFFLINE + 3]                  -> skipped because startup == NULL
  ...
  [CPUHP_BRINGUP_CPU]->startup()       -> success
  === End of PREPARE section
  [CPUHP_BRINGUP_CPU + 1]->startup()   -> success
  ...
  [CPUHP_AP_ONLINE]->startup()         -> success
  === End of STARTUP section
  [CPUHP_AP_ONLINE + 1]->startup()     -> success
  ...
  [CPUHP_ONLINE - 1]->startup()        -> success
  [CPUHP_ONLINE]

(一次成功的上线操作如下所示：)

A successful offline operation looks like this::

  [CPUHP_ONLINE]
  [CPUHP_ONLINE - 1]->teardown()       -> success
  ...
  [CPUHP_AP_ONLINE + 1]->teardown()    -> success
  === Start of STARTUP section
  [CPUHP_AP_ONLINE]->teardown()        -> success
  ...
  [CPUHP_BRINGUP_ONLINE - 1]->teardown()
  ...
  === Start of PREPARE section
  [CPUHP_BRINGUP_CPU]->teardown()
  [CPUHP_OFFLINE + 3]->teardown()
  [CPUHP_OFFLINE + 2]                  -> skipped because teardown == NULL
  [CPUHP_OFFLINE + 1]->teardown()
  [CPUHP_OFFLINE]

(一次成功的下线操作如下所示：)

A failed online operation looks like this::

  [CPUHP_OFFLINE]

(一次失败的上线操作如下所示：)
  [CPUHP_OFFLINE + 1]->startup()       -> success
  [CPUHP_OFFLINE + 2]->startup()       -> success
  [CPUHP_OFFLINE + 3]                  -> skipped because startup == NULL
  ...
  [CPUHP_BRINGUP_CPU]->startup()       -> success
  === End of PREPARE section
  [CPUHP_BRINGUP_CPU + 1]->startup()   -> success
  ...
  [CPUHP_AP_ONLINE]->startup()         -> success
  === End of STARTUP section
  [CPUHP_AP_ONLINE + 1]->startup()     -> success
  ---
  [CPUHP_AP_ONLINE + N]->startup()     -> fail
  [CPUHP_AP_ONLINE + (N - 1)]->teardown()
  ...
  [CPUHP_AP_ONLINE + 1]->teardown()
  === Start of STARTUP section
  [CPUHP_AP_ONLINE]->teardown()
  ...
  [CPUHP_BRINGUP_ONLINE - 1]->teardown()
  ...
  === Start of PREPARE section
  [CPUHP_BRINGUP_CPU]->teardown()
  [CPUHP_OFFLINE + 3]->teardown()
  [CPUHP_OFFLINE + 2]                  -> skipped because teardown == NULL
  [CPUHP_OFFLINE + 1]->teardown()
  [CPUHP_OFFLINE]

A failed offline operation looks like this::

  [CPUHP_ONLINE]
  [CPUHP_ONLINE - 1]->teardown()       -> success
  ...
  [CPUHP_ONLINE - N]->teardown()       -> fail
  [CPUHP_ONLINE - (N - 1)]->startup()
  ...
  [CPUHP_ONLINE - 1]->startup()
  [CPUHP_ONLINE]

(一次失败的下线操作如下所示：)

Recursive failures cannot be handled sensibly. Look at the following
example of a recursive fail due to a failed offline operation: ::

  [CPUHP_ONLINE]
  [CPUHP_ONLINE - 1]->teardown()       -> success
  ...
  [CPUHP_ONLINE - N]->teardown()       -> fail
  [CPUHP_ONLINE - (N - 1)]->startup()  -> success
  [CPUHP_ONLINE - (N - 2)]->startup()  -> fail

(递归失败无法被合理地处理。请看下面这个由一次失败的下线操作引发的递归失败示例：)

The CPU hotplug state machine stops right here and does not try to go back
down again because that would likely result in an endless loop::

  [CPUHP_ONLINE - (N - 1)]->teardown() -> success
  [CPUHP_ONLINE - N]->teardown()       -> fail

(CPU 热插拔状态机就此停止，不会再试图向下走，因为那很可能导致无限循环：)
  [CPUHP_ONLINE - (N - 1)]->startup()  -> success
  [CPUHP_ONLINE - (N - 2)]->startup()  -> fail
  [CPUHP_ONLINE - (N - 1)]->teardown() -> success
  [CPUHP_ONLINE - N]->teardown()       -> fail

Lather, rinse and repeat. In this case the CPU left in state::

  [CPUHP_ONLINE - (N - 1)]

(如此反复循环。这种情况下，CPU 停留在状态：)

which at least lets the system make progress and gives the user a chance to
debug or even resolve the situation.
(这至少让系统得以继续推进，并给用户调试甚至化解这一局面的机会。)

Allocating a state(分配状态)
----------------------------

There are two ways to allocate a CPU hotplug state:
(分配 CPU 热插拔状态有两种方式：)

* Static allocation
  (静态分配)

  Static allocation has to be used when the subsystem or driver has
  ordering requirements versus other CPU hotplug states. E.g. the PERF core
  startup callback has to be invoked before the PERF driver startup
  callbacks during a CPU online operation. During a CPU offline operation
  the driver teardown callbacks have to be invoked before the core teardown
  callback. The statically allocated states are described by constants in
  the cpuhp_state enum which can be found in include/linux/cpuhotplug.h.
  (当子系统或驱动相对于其他 CPU 热插拔状态有顺序要求时，必须使用静态分配。
  例如在 CPU 上线操作中，PERF 核心的 startup 回调必须先于 PERF 驱动的 startup
  回调被调用；而在 CPU 下线操作中，驱动的 teardown 回调必须先于核心的
  teardown 回调被调用。静态分配的状态由 include/linux/cpuhotplug.h 中的
  cpuhp_state 枚举常量描述。)

  Insert the state into the enum at the proper place so the ordering
  requirements are fulfilled. The state constant has to be used for state
  setup and removal.
  (把状态插入到枚举中合适的位置以满足顺序要求。状态的建立与移除都必须使用该
  状态常量。)

  Static allocation is also required when the state callbacks are not set
  up at runtime and are part of the initializer of the CPU hotplug state
  array in kernel/cpu.c.
  (当状态回调不在运行时建立、而是 kernel/cpu.c 中 CPU 热插拔状态数组初始化器的
  一部分时，也需要静态分配。)

* Dynamic allocation
  (动态分配)

  When there are no ordering requirements for the state callbacks then
  dynamic allocation is the preferred method. The state number is allocated
  by the setup function and returned to the caller on success.
  (当状态回调没有顺序要求时，动态分配是首选方式。状态号由建立函数分配，成功时
  返回给调用者。)

  Only the PREPARE and ONLINE sections provide a dynamic allocation
  range. The STARTING section does not as most of the callbacks in that
  section have explicit ordering requirements.
  (只有 PREPARE 与 ONLINE 区段提供动态分配范围。STARTING 区段不提供，因为该
  区段的大多数回调都有明确的顺序要求。)

Setup of a CPU hotplug state(设置 CPU 热插拔状态)
-------------------------------------------------

The core code provides the following functions to setup a state:
(核心代码提供以下函数来建立状态：)

* cpuhp_setup_state(state, name, startup, teardown)
* cpuhp_setup_state_nocalls(state, name, startup, teardown)
* cpuhp_setup_state_cpuslocked(state, name, startup, teardown)
* cpuhp_setup_state_nocalls_cpuslocked(state, name, startup, teardown)

For cases where a driver or a subsystem has multiple instances and the same
CPU hotplug state callbacks need to be invoked for each instance, the CPU
hotplug core provides multi-instance support. The advantage over driver
specific instance lists is that the instance related functions are fully
serialized against CPU hotplug operations and provide the automatic
invocations of the state callbacks on add and removal. To set up such a
multi-instance state the following function is available:
(当驱动或子系统有多个实例、且需要对每个实例调用相同的 CPU 热插拔状态回调时，
CPU 热插拔核心提供了多实例支持。相对于驱动自有的实例链表，其优势在于实例相关
的函数与 CPU 热插拔操作完全串行化，并在添加和删除时自动调用状态回调。要建立
这样一个多实例状态，可以使用以下函数：)

* cpuhp_setup_state_multi(state, name, startup, teardown)

The @state argument is either a statically allocated state or one of the
constants for dynamically allocated states - CPUHP_BP_PREPARE_DYN,
CPUHP_AP_ONLINE_DYN - depending on the state section (PREPARE, ONLINE) for
which a dynamic state should be allocated.
(@state 参数要么是静态分配的状态，要么是用于动态分配状态的常量之一——
CPUHP_BP_PREPARE_DYN、CPUHP_AP_ONLINE_DYN——取决于要在哪个状态区段
（PREPARE、ONLINE）分配动态状态。)

The @name argument is used for sysfs output and for instrumentation. The
naming convention is "subsys:mode" or "subsys/driver:mode",
e.g. "perf:mode" or "perf/x86:mode". The common mode names are:
(@name 参数用于 sysfs 输出与插桩。命名约定是 "subsys:mode" 或
"subsys/driver:mode"，例如 "perf:mode" 或 "perf/x86:mode"。常用的 mode
名称为：)

======== =======================================================
prepare  For states in the PREPARE section (用于 PREPARE 区段中的状态)

dead     For states in the PREPARE section which do not provide
         a startup callback (用于 PREPARE 区段中不提供启动回调的状态)

starting For states in the STARTING section (用于 STARTING 区段中的状态)

dying    For states in the STARTING section which do not provide
         a startup callback (用于 STARTING 区段中不提供启动回调的状态)

online   For states in the ONLINE section (用于 ONLINE 区段中的状态)

offline  For states in the ONLINE section which do not provide
         a startup callback (用于 ONLINE 区段中不提供启动回调的状态)
======== =======================================================

As the @name argument is only used for sysfs and instrumentation other mode
descriptors can be used as well if they describe the nature of the state
better than the common ones.
(由于 @name 参数只用于 sysfs 与插桩，如果其他 mode 描述符比这些常用名称更能
描述状态的性质，也可以使用它们。)

Examples for @name arguments: "perf/online", "perf/x86:prepare",
"RCU/tree:dying", "sched/waitempty"
(@name 参数示例："perf/online"、"perf/x86:prepare"、"RCU/tree:dying"、
"sched/waitempty"。)

The @startup argument is a function pointer to the callback which should be
invoked during a CPU online operation. If the usage site does not require a
startup callback set the pointer to NULL.
(@startup 参数是函数指针，指向应在 CPU 上线操作期间调用的回调。如果使用点
不需要 startup 回调，则把该指针设为 NULL。)

The @teardown argument is a function pointer to the callback which should
be invoked during a CPU offline operation. If the usage site does not
require a teardown callback set the pointer to NULL.
(@teardown 参数是函数指针，指向应在 CPU 下线操作期间调用的回调。如果使用点
不需要 teardown 回调，则把该指针设为 NULL。)

The functions differ in the way how the installed callbacks are treated:
(这些函数在如何对待所安装的回调上有所区别：)

  * cpuhp_setup_state_nocalls(), cpuhp_setup_state_nocalls_cpuslocked()
    and cpuhp_setup_state_multi() only install the callbacks
    (cpuhp_setup_state_nocalls()、cpuhp_setup_state_nocalls_cpuslocked() 与
    cpuhp_setup_state_multi() 只安装回调。)

  * cpuhp_setup_state() and cpuhp_setup_state_cpuslocked() install the
    callbacks and invoke the @startup callback (if not NULL) for all online
    CPUs which have currently a state greater than the newly installed
    state. Depending on the state section the callback is either invoked on
    the current CPU (PREPARE section) or on each online CPU (ONLINE
    section) in the context of the CPU's hotplug thread.
    (cpuhp_setup_state() 与 cpuhp_setup_state_cpuslocked() 安装回调，并对当前
    状态大于新安装状态的所有在线 CPU 调用 @startup 回调（如果不为 NULL）。取决于
    状态区段，回调或者在当前 CPU 上调用（PREPARE 区段），或者在每个在线 CPU 上、
    于该 CPU 的热插拔线程上下文中调用（ONLINE 区段）。)

    If a callback fails for CPU N then the teardown callback for CPU
    0 .. N-1 is invoked to rollback the operation. The state setup fails,
    the callbacks for the state are not installed and in case of dynamic
    allocation the allocated state is freed.
    (如果回调在 CPU N 上失败，则会对 CPU 0 .. N-1 调用 teardown 回调以回滚该
    操作。状态建立失败，该状态的回调不会被安装；在动态分配的情况下，分配到的
    状态会被释放。)

The state setup and the callback invocations are serialized against CPU
hotplug operations. If the setup function has to be called from a CPU
hotplug read locked region, then the _cpuslocked() variants have to be
used. These functions cannot be used from within CPU hotplug callbacks.
(状态的建立与回调调用相对于 CPU 热插拔操作是串行化的。如果必须在 CPU 热插拔
读锁区域内调用建立函数，则必须使用 _cpuslocked() 变体。这些函数不能在 CPU
热插拔回调内部使用。)

The function return values:
(这些函数的返回值：)

  ======== ===================================================================
  0        Statically allocated state was successfully set up (静态分配的状态已成功设置。)

  >0       Dynamically allocated state was successfully set up.

           The returned number is the state number which was allocated. If
           the state callbacks have to be removed later, e.g. module
           removal, then this number has to be saved by the caller and used
           as @state argument for the state remove function. For
           multi-instance states the dynamically allocated state number is
           also required as @state argument for the instance add/remove
           operations.
           (动态分配的状态已成功设置。返回的数字是分配到的状态号。如果以后
           需要移除状态回调（例如模块卸载），调用者必须保存这个数字，并将其作为
           @state 参数传给状态移除函数。对于多实例状态，动态分配的状态号也需要
           作为 @state 参数用于实例的添加/移除操作。)

  <0	   Operation failed (操作失败)
  ======== ===================================================================

Removal of a CPU hotplug state(移除 CPU 热插拔状态)
---------------------------------------------------

To remove a previously set up state, the following functions are provided:
(要移除之前建立的状态，提供了以下函数：)

* cpuhp_remove_state(state)
* cpuhp_remove_state_nocalls(state)
* cpuhp_remove_state_nocalls_cpuslocked(state)
* cpuhp_remove_multi_state(state)

The @state argument is either a statically allocated state or the state
number which was allocated in the dynamic range by cpuhp_setup_state*(). If
the state is in the dynamic range, then the state number is freed and
available for dynamic allocation again.
(@state 参数要么是静态分配的状态，要么是由 cpuhp_setup_state*() 在动态范围内
分配的状态号。如果状态位于动态范围内，则该状态号会被释放，可供再次动态分配。)

The functions differ in the way how the installed callbacks are treated:
(这些函数在如何对待所安装的回调上有所区别：)

  * cpuhp_remove_state_nocalls(), cpuhp_remove_state_nocalls_cpuslocked()
    and cpuhp_remove_multi_state() only remove the callbacks.
    (cpuhp_remove_state_nocalls()、cpuhp_remove_state_nocalls_cpuslocked() 与
    cpuhp_remove_multi_state() 只移除回调。)

  * cpuhp_remove_state() removes the callbacks and invokes the teardown
    callback (if not NULL) for all online CPUs which have currently a state
    greater than the removed state. Depending on the state section the
    callback is either invoked on the current CPU (PREPARE section) or on
    each online CPU (ONLINE section) in the context of the CPU's hotplug
    thread.
    (cpuhp_remove_state() 移除回调，并对当前状态大于被移除状态的所有在线 CPU
    调用 teardown 回调（如果不为 NULL）。取决于状态区段，回调或者在当前 CPU 上
    调用（PREPARE 区段），或者在每个在线 CPU 上、于该 CPU 的热插拔线程上下文中
    调用（ONLINE 区段）。)

    In order to complete the removal, the teardown callback should not fail.
    (为了完成移除，teardown 回调不应失败。)

The state removal and the callback invocations are serialized against CPU
hotplug operations. If the remove function has to be called from a CPU
hotplug read locked region, then the _cpuslocked() variants have to be
used. These functions cannot be used from within CPU hotplug callbacks.
(状态的移除与回调调用相对于 CPU 热插拔操作是串行化的。如果必须在 CPU 热插拔
读锁区域内调用移除函数，则必须使用 _cpuslocked() 变体。这些函数不能在 CPU
热插拔回调内部使用。)

If a multi-instance state is removed then the caller has to remove all
instances first.
(如果移除的是多实例状态，调用者必须先移除所有实例。)

Multi-Instance state instance management(多实例状态的实例管理)
--------------------------------------------------------------

Once the multi-instance state is set up, instances can be added to the
state:
(多实例状态建立之后，可以向该状态添加实例：)

  * cpuhp_state_add_instance(state, node)
  * cpuhp_state_add_instance_nocalls(state, node)

The @state argument is either a statically allocated state or the state
number which was allocated in the dynamic range by cpuhp_setup_state_multi().
(@state 参数要么是静态分配的状态，要么是由 cpuhp_setup_state_multi() 在动态
范围内分配的状态号。)

The @node argument is a pointer to an hlist_node which is embedded in the
instance's data structure. The pointer is handed to the multi-instance
state callbacks and can be used by the callback to retrieve the instance
via container_of().
(@node 参数是指向 hlist_node 的指针，该节点内嵌在实例的数据结构中。这个指针
会被传给多实例状态回调，回调可以用它通过 container_of() 取回实例。)

The functions differ in the way how the installed callbacks are treated:
(这些函数在如何对待所安装的回调上有所区别：)

  * cpuhp_state_add_instance_nocalls() and only adds the instance to the
    multi-instance state's node list.
    (cpuhp_state_add_instance_nocalls() 只把实例添加到多实例状态的节点列表中。)

  * cpuhp_state_add_instance() adds the instance and invokes the startup
    callback (if not NULL) associated with @state for all online CPUs which
    have currently a state greater than @state. The callback is only
    invoked for the to be added instance. Depending on the state section
    the callback is either invoked on the current CPU (PREPARE section) or
    on each online CPU (ONLINE section) in the context of the CPU's hotplug
    thread.
    (cpuhp_state_add_instance() 添加实例，并对当前状态大于 @state 的所有在线
    CPU 调用与 @state 关联的 startup 回调（如果不为 NULL）。回调只为这个要添加
    的实例调用。取决于状态区段，回调或者在当前 CPU 上调用（PREPARE 区段），或者
    在每个在线 CPU 上、于该 CPU 的热插拔线程上下文中调用（ONLINE 区段）。)

    If a callback fails for CPU N then the teardown callback for CPU
    0 .. N-1 is invoked to rollback the operation, the function fails and
    the instance is not added to the node list of the multi-instance state.
    (如果回调在 CPU N 上失败，则会对 CPU 0 .. N-1 调用 teardown 回调以回滚该
    操作，函数失败，实例不会被加入多实例状态的节点列表。)

To remove an instance from the state's node list these functions are
available:
(要从状态的节点列表中移除实例，可以使用以下函数：)

  * cpuhp_state_remove_instance(state, node)
  * cpuhp_state_remove_instance_nocalls(state, node)

The arguments are the same as for the cpuhp_state_add_instance*()
variants above.
(其参数与上面的 cpuhp_state_add_instance*() 变体相同。)

The functions differ in the way how the installed callbacks are treated:
(这些函数在如何对待所安装的回调上有所区别：)

  * cpuhp_state_remove_instance_nocalls() only removes the instance from the
    state's node list.
    (cpuhp_state_remove_instance_nocalls() 只把实例从状态的节点列表中移除。)

  * cpuhp_state_remove_instance() removes the instance and invokes the
    teardown callback (if not NULL) associated with @state for all online
    CPUs which have currently a state greater than @state.  The callback is
    only invoked for the to be removed instance.  Depending on the state
    section the callback is either invoked on the current CPU (PREPARE
    section) or on each online CPU (ONLINE section) in the context of the
    CPU's hotplug thread.
    (cpuhp_state_remove_instance() 移除实例，并对当前状态大于 @state 的所有
    在线 CPU 调用与 @state 关联的 teardown 回调（如果不为 NULL）。回调只为这个
    要移除的实例调用。取决于状态区段，回调或者在当前 CPU 上调用（PREPARE 区段），
    或者在每个在线 CPU 上、于该 CPU 的热插拔线程上下文中调用（ONLINE 区段）。)

    In order to complete the removal, the teardown callback should not fail.
    (为了完成移除，teardown 回调不应失败。)

The node list add/remove operations and the callback invocations are
serialized against CPU hotplug operations. These functions cannot be used
from within CPU hotplug callbacks and CPU hotplug read locked regions.
(节点列表的添加/移除操作与回调调用相对于 CPU 热插拔操作是串行化的。这些函数
不能在 CPU 热插拔回调内部以及 CPU 热插拔读锁区域内使用。)

Examples(示例)
--------------

Setup and teardown a statically allocated state in the STARTING section for
notifications on online and offline operations::

   ret = cpuhp_setup_state(CPUHP_SUBSYS_STARTING, "subsys:starting", subsys_cpu_starting, subsys_cpu_dying);
   if (ret < 0)
        return ret;
   ....
   cpuhp_remove_state(CPUHP_SUBSYS_STARTING);

(在 STARTING 区段建立并拆除一个静态分配的状态，用于在线与离线操作的通知：)

Setup and teardown a dynamically allocated state in the ONLINE section
for notifications on offline operations::

   state = cpuhp_setup_state(CPUHP_AP_ONLINE_DYN, "subsys:offline", NULL, subsys_cpu_offline);
   if (state < 0)
       return state;
   ....
   cpuhp_remove_state(state);

(在 ONLINE 区段建立并拆除一个动态分配的状态，用于离线操作的通知：)

Setup and teardown a dynamically allocated state in the ONLINE section
for notifications on online operations without invoking the callbacks::

   state = cpuhp_setup_state_nocalls(CPUHP_AP_ONLINE_DYN, "subsys:online", subsys_cpu_online, NULL);
   if (state < 0)
       return state;
   ....
   cpuhp_remove_state_nocalls(state);

(在 ONLINE 区段建立并拆除一个动态分配的状态，用于在线操作的通知，但不调用回调：)

Setup, use and teardown a dynamically allocated multi-instance state in the
ONLINE section for notifications on online and offline operation::

   state = cpuhp_setup_state_multi(CPUHP_AP_ONLINE_DYN, "subsys:online", subsys_cpu_online, subsys_cpu_offline);
   if (state < 0)
       return state;
   ....
   ret = cpuhp_state_add_instance(state, &inst1->node);
   if (ret)
        return ret;
   ....
   ret = cpuhp_state_add_instance(state, &inst2->node);
   if (ret)
        return ret;
   ....
   cpuhp_remove_instance(state, &inst1->node);
   ....
   cpuhp_remove_instance(state, &inst2->node);
   ....
   cpuhp_remove_multi_state(state);

(在 ONLINE 区段建立、使用并拆除一个动态分配的多实例状态，用于在线与离线操作的通知：)


Testing of hotplug states(测试热插拔状态)
=========================================

One way to verify whether a custom state is working as expected or not is to
shutdown a CPU and then put it online again. It is also possible to put the CPU
to certain state (for instance *CPUHP_AP_ONLINE*) and then go back to
*CPUHP_ONLINE*. This would simulate an error one state after *CPUHP_AP_ONLINE*
which would lead to rollback to the online state.
(验证自定义状态是否按预期工作的一种方法，是把某个 CPU 关闭后再重新上线。也可以
把 CPU 置于某个状态（例如 CPUHP_AP_ONLINE）然后再回到 CPUHP_ONLINE。这会
模拟发生在 CPUHP_AP_ONLINE 之后一个状态处的错误，从而导致回滚到在线状态。)

All registered states are enumerated in ``/sys/devices/system/cpu/hotplug/states`` ::

 $ tail /sys/devices/system/cpu/hotplug/states
 138: mm/vmscan:online
 139: mm/vmstat:online
 140: lib/percpu_cnt:online
 141: acpi/cpu-drv:online
 142: base/cacheinfo:online
 143: virtio/net:online
 144: x86/mce:online
 145: printk:online
 168: sched:active
 169: online

(所有已注册的状态都在 ``/sys/devices/system/cpu/hotplug/states`` 中枚举：)

To rollback CPU4 to ``lib/percpu_cnt:online`` and back online just issue::

  $ cat /sys/devices/system/cpu/cpu4/hotplug/state
  169
  $ echo 140 > /sys/devices/system/cpu/cpu4/hotplug/target
  $ cat /sys/devices/system/cpu/cpu4/hotplug/state
  140

(要把 CPU4 回滚到 ``lib/percpu_cnt:online`` 再重新上线，只需执行：)

It is important to note that the teardown callback of state 140 have been
invoked. And now get back online::

  $ echo 169 > /sys/devices/system/cpu/cpu4/hotplug/target
  $ cat /sys/devices/system/cpu/cpu4/hotplug/state
  169

(需要注意，状态 140 的 teardown 回调已被调用过。现在重新上线：)

With trace events enabled, the individual steps are visible, too::

  #  TASK-PID   CPU#    TIMESTAMP  FUNCTION
  #     | |       |        |         |
      bash-394  [001]  22.976: cpuhp_enter: cpu: 0004 target: 140 step: 169 (cpuhp_kick_ap_work)
   cpuhp/4-31   [004]  22.977: cpuhp_enter: cpu: 0004 target: 140 step: 168 (sched_cpu_deactivate)
   cpuhp/4-31   [004]  22.990: cpuhp_exit:  cpu: 0004  state: 168 step: 168 ret: 0
   cpuhp/4-31   [004]  22.991: cpuhp_enter: cpu: 0004 target: 140 step: 144 (mce_cpu_pre_down)
   cpuhp/4-31   [004]  22.992: cpuhp_exit:  cpu: 0004  state: 144 step: 144 ret: 0
   cpuhp/4-31   [004]  22.993: cpuhp_multi_enter: cpu: 0004 target: 140 step: 143 (virtnet_cpu_down_prep)
   cpuhp/4-31   [004]  22.994: cpuhp_exit:  cpu: 0004  state: 143 step: 143 ret: 0
   cpuhp/4-31   [004]  22.995: cpuhp_enter: cpu: 0004 target: 140 step: 142 (cacheinfo_cpu_pre_down)
   cpuhp/4-31   [004]  22.996: cpuhp_exit:  cpu: 0004  state: 142 step: 142 ret: 0
      bash-394  [001]  22.997: cpuhp_exit:  cpu: 0004  state: 140 step: 169 ret: 0
      bash-394  [005]  95.540: cpuhp_enter: cpu: 0004 target: 169 step: 140 (cpuhp_kick_ap_work)
   cpuhp/4-31   [004]  95.541: cpuhp_enter: cpu: 0004 target: 169 step: 141 (acpi_soft_cpu_online)
   cpuhp/4-31   [004]  95.542: cpuhp_exit:  cpu: 0004  state: 141 step: 141 ret: 0
   cpuhp/4-31   [004]  95.543: cpuhp_enter: cpu: 0004 target: 169 step: 142 (cacheinfo_cpu_online)
   cpuhp/4-31   [004]  95.544: cpuhp_exit:  cpu: 0004  state: 142 step: 142 ret: 0
   cpuhp/4-31   [004]  95.545: cpuhp_multi_enter: cpu: 0004 target: 169 step: 143 (virtnet_cpu_online)
   cpuhp/4-31   [004]  95.546: cpuhp_exit:  cpu: 0004  state: 143 step: 143 ret: 0
   cpuhp/4-31   [004]  95.547: cpuhp_enter: cpu: 0004 target: 169 step: 144 (mce_cpu_online)
   cpuhp/4-31   [004]  95.548: cpuhp_exit:  cpu: 0004  state: 144 step: 144 ret: 0
   cpuhp/4-31   [004]  95.549: cpuhp_enter: cpu: 0004 target: 169 step: 145 (console_cpu_notify)
   cpuhp/4-31   [004]  95.550: cpuhp_exit:  cpu: 0004  state: 145 step: 145 ret: 0
   cpuhp/4-31   [004]  95.551: cpuhp_enter: cpu: 0004 target: 169 step: 168 (sched_cpu_activate)
   cpuhp/4-31   [004]  95.552: cpuhp_exit:  cpu: 0004  state: 168 step: 168 ret: 0
      bash-394  [005]  95.553: cpuhp_exit:  cpu: 0004  state: 169 step: 140 ret: 0

(在启用跟踪事件的情况下，各个单独的步骤也是可见的：)

As it an be seen, CPU4 went down until timestamp 22.996 and then back up until
95.552. All invoked callbacks including their return codes are visible in the
trace.
(可以看出，CPU4 一直下线到时间戳 22.996，然后又重新上线直到 95.552。所有被
调用的回调及其返回码都在跟踪记录中可见。)

Architecture's requirements(架构要求)
=====================================

The following functions and configurations are required:
(需要以下函数与配置：)

``CONFIG_HOTPLUG_CPU``
  This entry needs to be enabled in Kconfig
  (此项需要在 Kconfig 中启用。)

``__cpu_up()``
  Arch interface to bring up a CPU
  (启动一个 CPU 的架构接口。)

``__cpu_disable()``
  Arch interface to shutdown a CPU, no more interrupts can be handled by the
  kernel after the routine returns. This includes the shutdown of the timer.
  (关闭一个 CPU 的架构接口。该例程返回之后，内核不能再处理任何中断。这包括
  定时器的关闭。)

``__cpu_die()``
  This actually supposed to ensure death of the CPU. Actually look at some
  example code in other arch that implement CPU hotplug. The processor is taken
  down from the ``idle()`` loop for that specific architecture. ``__cpu_die()``
  typically waits for some per_cpu state to be set, to ensure the processor dead
  routine is called to be sure positively.
  (它实际上应当确保 CPU 死亡。可以看看其他实现了 CPU 热插拔的架构中的一些示例
  代码。处理器会从该特定架构的 ``idle()`` 循环中被摘除。``__cpu_die()`` 通常会
  等待某个 per_cpu 状态被设置，以确保处理器死亡例程确实被调用。)

User Space Notification(用户空间通知)
=====================================

After CPU successfully onlined or offline udev events are sent. A udev rule like::

  SUBSYSTEM=="cpu", DRIVERS=="processor", DEVPATH=="/devices/system/cpu/*", RUN+="the_hotplug_receiver.sh"

(CPU 成功上线或下线之后，会发送 udev 事件。下面这样的一条 udev 规则)

will receive all events. A script like::

  #!/bin/sh

  if [ "${ACTION}" = "offline" ]
  then
      echo "CPU ${DEVPATH##*/} offline"

  elif [ "${ACTION}" = "online" ]
  then
      echo "CPU ${DEVPATH##*/} online"

  fi

(将接收到所有事件。而下面这样的脚本)

can process the event further.
(可以对该事件做进一步处理。)

When changes to the CPUs in the system occur, the sysfs file
/sys/devices/system/cpu/crash_hotplug contains '1' if the kernel
updates the kdump capture kernel list of CPUs itself (via elfcorehdr and
other relevant kexec segment), or '0' if userspace must update the kdump
capture kernel list of CPUs.
(当系统中的 CPU 发生变化时，sysfs 文件 /sys/devices/system/cpu/crash_hotplug
在内核自行更新 kdump 捕获内核的 CPU 列表（通过 elfcorehdr 及其他相关 kexec
segment）时包含 '1'；如果必须由用户态更新 kdump 捕获内核的 CPU 列表，则包含
'0'。)

The availability depends on the CONFIG_HOTPLUG_CPU kernel configuration
option.
(其可用性取决于 CONFIG_HOTPLUG_CPU 内核配置选项。)

To skip userspace processing of CPU hot un/plug events for kdump
(i.e. the unload-then-reload to obtain a current list of CPUs), this sysfs
file can be used in a udev rule as follows:
(为了让 kdump 跳过对 CPU 热插拔事件的用户态处理（即卸载后再重载以获得当前的
CPU 列表），可以在 udev 规则中如下使用这个 sysfs 文件：)

 SUBSYSTEM=="cpu", ATTRS{crash_hotplug}=="1", GOTO="kdump_reload_end"

For a CPU hot un/plug event, if the architecture supports kernel updates
of the elfcorehdr (which contains the list of CPUs) and other relevant
kexec segments, then the rule skips the unload-then-reload of the kdump
capture kernel.
(对于 CPU 热插拔事件，如果架构支持由内核更新 elfcorehdr（其中包含 CPU 列表）
及其他相关 kexec segment，那么该规则就会跳过 kdump 捕获内核的卸载-再重载。)

Kernel Inline Documentations Reference(内核内联文档参考)
========================================================

.. kernel-doc:: include/linux/cpuhotplug.h
