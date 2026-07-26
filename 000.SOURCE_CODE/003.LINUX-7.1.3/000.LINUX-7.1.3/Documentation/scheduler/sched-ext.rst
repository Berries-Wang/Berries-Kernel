.. _sched-ext:

==========================
Extensible Scheduler Class
==========================
可扩展调度器类

sched_ext is a scheduler class whose behavior can be defined by a set of BPF
programs - the BPF scheduler.
sched_ext 是一个调度器类，其行为可由一组 BPF 程序（即 BPF 调度器）定义。

* sched_ext exports a full scheduling interface so that any scheduling
  algorithm can be implemented on top.
* sched_ext 导出了完整的调度接口，因此任何调度算法都可以在其上实现。

* The BPF scheduler can group CPUs however it sees fit and schedule them
  together, as tasks aren't tied to specific CPUs at the time of wakeup.
* BPF 调度器可以按需对 CPU 进行分组并一起调度，因为任务在唤醒时并不绑定到特定 CPU。

* The BPF scheduler can be turned on and off dynamically anytime.
* BPF 调度器可以随时动态地开启和关闭。

* The system integrity is maintained no matter what the BPF scheduler does.
  The default scheduling behavior is restored anytime an error is detected,
  a runnable task stalls, or on invoking the SysRq key sequence
  ``SysRq-S``.
* 无论 BPF 调度器做什么，系统完整性都得到维护。每当检测到错误、可运行任务停滞或调用 SysRq 按键序列 ``SysRq-S`` 时，都会恢复默认的调度行为。

* When the BPF scheduler triggers an error, debug information is dumped to
  aid debugging. The debug dump is passed to and printed out by the
  scheduler binary. The debug dump can also be accessed through the
  ``sched_ext_dump`` tracepoint. The SysRq key sequence ``SysRq-D``
  triggers a debug dump. This doesn't terminate the BPF scheduler and can
  only be read through the tracepoint.
* 当 BPF 调度器触发错误时，会转储调试信息以辅助调试。调试转储会传递给调度器二进制程序并打印输出。调试转储也可以通过 ``sched_ext_dump`` 跟踪点访问。SysRq 按键序列 ``SysRq-D`` 会触发调试转储。这不会终止 BPF 调度器，并且只能通过跟踪点读取。

Switching to and from sched_ext
===============================
切换到 sched_ext 和从 sched_ext 切换

``CONFIG_SCHED_CLASS_EXT`` is the config option to enable sched_ext and
``tools/sched_ext`` contains the example schedulers. The following config
options should be enabled to use sched_ext:
``CONFIG_SCHED_CLASS_EXT`` 是启用 sched_ext 的配置选项，``tools/sched_ext`` 包含示例调度器。使用 sched_ext 需要启用以下配置选项：

.. code-block:: none

    CONFIG_BPF=y
    CONFIG_SCHED_CLASS_EXT=y
    CONFIG_BPF_SYSCALL=y
    CONFIG_BPF_JIT=y
    CONFIG_DEBUG_INFO_BTF=y
    CONFIG_BPF_JIT_ALWAYS_ON=y
    CONFIG_BPF_JIT_DEFAULT_ON=y

sched_ext is used only when the BPF scheduler is loaded and running.
sched_ext 仅在 BPF 调度器加载并运行时才被使用。

If a task explicitly sets its scheduling policy to ``SCHED_EXT``, it will be
treated as ``SCHED_NORMAL`` and scheduled by the fair-class scheduler until the
BPF scheduler is loaded.
如果任务显式将其调度策略设置为 ``SCHED_EXT``，在 BPF 调度器加载之前，它将被视为 ``SCHED_NORMAL`` 并由公平调度器调度。

When the BPF scheduler is loaded and ``SCX_OPS_SWITCH_PARTIAL`` is not set
in ``ops->flags``, all ``SCHED_NORMAL``, ``SCHED_BATCH``, ``SCHED_IDLE``, and
``SCHED_EXT`` tasks are scheduled by sched_ext.
当 BPF 调度器加载且 ``ops->flags`` 中未设置 ``SCX_OPS_SWITCH_PARTIAL`` 时，所有 ``SCHED_NORMAL``、``SCHED_BATCH``、``SCHED_IDLE`` 和 ``SCHED_EXT`` 任务都由 sched_ext 调度。

However, when the BPF scheduler is loaded and ``SCX_OPS_SWITCH_PARTIAL`` is
set in ``ops->flags``, only tasks with the ``SCHED_EXT`` policy are scheduled
by sched_ext, while tasks with ``SCHED_NORMAL``, ``SCHED_BATCH`` and
``SCHED_IDLE`` policies are scheduled by the fair-class scheduler which has
higher sched_class precedence than ``SCHED_EXT``.
然而，当 BPF 调度器加载且 ``ops->flags`` 中设置了 ``SCX_OPS_SWITCH_PARTIAL`` 时，只有具有 ``SCHED_EXT`` 策略的任务由 sched_ext 调度，而具有 ``SCHED_NORMAL``、``SCHED_BATCH`` 和 ``SCHED_IDLE`` 策略的任务则由具有比 ``SCHED_EXT`` 更高调度类优先级的公平调度器调度。

Terminating the sched_ext scheduler program, triggering ``SysRq-S``, or
detection of any internal error including stalled runnable tasks aborts the
BPF scheduler and reverts all tasks back to the fair-class scheduler.
终止 sched_ext 调度器程序、触发 ``SysRq-S`` 或检测到任何内部错误（包括可运行任务停滞）会中止 BPF 调度器，并将所有任务回退到公平调度器。

.. code-block:: none

    # make -j16 -C tools/sched_ext
    # tools/sched_ext/build/bin/scx_simple
    local=0 global=3
    local=5 global=24
    local=9 global=44
    local=13 global=56
    local=17 global=72
    ^CEXIT: BPF scheduler unregistered

The current status of the BPF scheduler can be determined as follows:
BPF 调度器的当前状态可以通过以下方式确定：

.. code-block:: none

    # cat /sys/kernel/sched_ext/state
    enabled
    # cat /sys/kernel/sched_ext/root/ops
    simple

You can check if any BPF scheduler has ever been loaded since boot by examining
this monotonically incrementing counter (a value of zero indicates that no BPF
scheduler has been loaded):
你可以通过检查这个单调递增的计数器来确认自启动以来是否有 BPF 调度器曾被加载过（值为零表示从未加载过 BPF 调度器）：

.. code-block:: none

    # cat /sys/kernel/sched_ext/enable_seq
    1

Each running scheduler also exposes a per-scheduler ``events`` file under
``/sys/kernel/sched_ext/<scheduler-name>/events`` that tracks diagnostic
counters. Each counter occupies one ``name value`` line:
每个正在运行的调度器还在 ``/sys/kernel/sched_ext/<scheduler-name>/events`` 下暴露了一个按调度器划分的 ``events`` 文件，用于跟踪诊断计数器。每个计数器占据一行 ``name value``：

.. code-block:: none

    # cat /sys/kernel/sched_ext/simple/events
    SCX_EV_SELECT_CPU_FALLBACK 0
    SCX_EV_DISPATCH_LOCAL_DSQ_OFFLINE 0
    SCX_EV_DISPATCH_KEEP_LAST 123
    SCX_EV_ENQ_SKIP_EXITING 0
    SCX_EV_ENQ_SKIP_MIGRATION_DISABLED 0
    SCX_EV_REENQ_IMMED 0
    SCX_EV_REENQ_LOCAL_REPEAT 0
    SCX_EV_REFILL_SLICE_DFL 456789
    SCX_EV_BYPASS_DURATION 0
    SCX_EV_BYPASS_DISPATCH 0
    SCX_EV_BYPASS_ACTIVATE 0
    SCX_EV_INSERT_NOT_OWNED 0
    SCX_EV_SUB_BYPASS_DISPATCH 0

The counters are described in ``kernel/sched/ext_internal.h``; briefly:
这些计数器在 ``kernel/sched/ext_internal.h`` 中有描述；简要说明如下：

* ``SCX_EV_SELECT_CPU_FALLBACK``: ops.select_cpu() returned a CPU unusable by
  the task and the core scheduler silently picked a fallback CPU.
* ``SCX_EV_SELECT_CPU_FALLBACK``：ops.select_cpu() 返回了一个任务无法使用的 CPU，核心调度器静默地选择了一个备用 CPU。

* ``SCX_EV_DISPATCH_LOCAL_DSQ_OFFLINE``: a local-DSQ dispatch was redirected
  to the global DSQ because the target CPU went offline.
* ``SCX_EV_DISPATCH_LOCAL_DSQ_OFFLINE``：由于目标 CPU 下线，本地 DSQ 的调度被重定向到全局 DSQ。

* ``SCX_EV_DISPATCH_KEEP_LAST``: a task continued running because no other
  task was available (only when ``SCX_OPS_ENQ_LAST`` is not set).
* ``SCX_EV_DISPATCH_KEEP_LAST``：因为没有其他任务可用，一个任务继续运行（仅在未设置 ``SCX_OPS_ENQ_LAST`` 时）。

* ``SCX_EV_ENQ_SKIP_EXITING``: an exiting task was dispatched to the local DSQ
  directly, bypassing ops.enqueue() (only when ``SCX_OPS_ENQ_EXITING`` is not set).
* ``SCX_EV_ENQ_SKIP_EXITING``：一个正在退出的任务被直接调度到本地 DSQ，跳过了 ops.enqueue()（仅在未设置 ``SCX_OPS_ENQ_EXITING`` 时）。

* ``SCX_EV_ENQ_SKIP_MIGRATION_DISABLED``: a migration-disabled task was
  dispatched to its local DSQ directly (only when
  ``SCX_OPS_ENQ_MIGRATION_DISABLED`` is not set).
* ``SCX_EV_ENQ_SKIP_MIGRATION_DISABLED``：一个禁用了迁移的任务被直接调度到其本地 DSQ（仅在未设置 ``SCX_OPS_ENQ_MIGRATION_DISABLED`` 时）。

* ``SCX_EV_REENQ_IMMED``: a task dispatched with ``SCX_ENQ_IMMED`` was
  re-enqueued because the target CPU was not available for immediate execution.
* ``SCX_EV_REENQ_IMMED``：一个使用 ``SCX_ENQ_IMMED`` 调度的任务被重新入队，因为目标 CPU 无法立即执行。

* ``SCX_EV_REENQ_LOCAL_REPEAT``: a reenqueue of the local DSQ triggered
  another reenqueue; recurring counts indicate incorrect ``SCX_ENQ_REENQ``
  handling in the BPF scheduler.
* ``SCX_EV_REENQ_LOCAL_REPEAT``：本地 DSQ 的重新入队触发了另一次重新入队；重复计数表明 BPF 调度器中 ``SCX_ENQ_REENQ`` 处理不正确。

* ``SCX_EV_REFILL_SLICE_DFL``: a task's time slice was refilled with the
  default value (``SCX_SLICE_DFL``).
* ``SCX_EV_REFILL_SLICE_DFL``：任务的时间片被填充为默认值（``SCX_SLICE_DFL``）。

* ``SCX_EV_BYPASS_DURATION``: total nanoseconds spent in bypass mode.
* ``SCX_EV_BYPASS_DURATION``：在旁路模式下花费的总纳秒数。

* ``SCX_EV_BYPASS_DISPATCH``: number of tasks dispatched while in bypass mode.
* ``SCX_EV_BYPASS_DISPATCH``：在旁路模式下调度的任务数。

* ``SCX_EV_BYPASS_ACTIVATE``: number of times bypass mode was activated.
* ``SCX_EV_BYPASS_ACTIVATE``：旁路模式被激活的次数。

* ``SCX_EV_INSERT_NOT_OWNED``: attempted to insert a task not owned by this
  scheduler into a DSQ; such attempts are silently ignored.
* ``SCX_EV_INSERT_NOT_OWNED``：尝试将不属于此调度器的任务插入到 DSQ 中；此类尝试会被静默忽略。

* ``SCX_EV_SUB_BYPASS_DISPATCH``: tasks dispatched from sub-scheduler bypass
  DSQs (only relevant with ``CONFIG_EXT_SUB_SCHED``).
* ``SCX_EV_SUB_BYPASS_DISPATCH``：从子调度器旁路 DSQ 调度的任务（仅在 ``CONFIG_EXT_SUB_SCHED`` 下相关）。

``tools/sched_ext/scx_show_state.py`` is a drgn script which shows more
detailed information:
``tools/sched_ext/scx_show_state.py`` 是一个 drgn 脚本，显示更详细的信息：

.. code-block:: none

    # tools/sched_ext/scx_show_state.py
    ops           : simple
    enabled       : 1
    switching_all : 1
    switched_all  : 1
    enable_state  : enabled (2)
    bypass_depth  : 0
    nr_rejected   : 0
    enable_seq    : 1

Whether a given task is on sched_ext can be determined as follows:
给定任务是否在 sched_ext 上可以通过以下方式确定：

.. code-block:: none

    # grep ext /proc/self/sched
    ext.enabled                                  :                    1

The Basics
==========
基础知识

Userspace can implement an arbitrary BPF scheduler by loading a set of BPF
programs that implement ``struct sched_ext_ops``. The only mandatory field
is ``ops.name`` which must be a valid BPF object name. All operations are
optional. The following modified excerpt is from
``tools/sched_ext/scx_simple.bpf.c`` showing a minimal global FIFO scheduler.
用户空间可以通过加载一组实现 ``struct sched_ext_ops`` 的 BPF 程序来实现任意 BPF 调度器。唯一的必填字段是 ``ops.name``，它必须是有效的 BPF 对象名称。所有操作都是可选的。以下修改后的摘录来自 ``tools/sched_ext/scx_simple.bpf.c``，展示了一个最小的全局 FIFO 调度器。

.. code-block:: c

    /*
     * Decide which CPU a task should be migrated to before being
     * enqueued (either at wakeup, fork time, or exec time). If an
     * idle core is found by the default ops.select_cpu() implementation,
     * then insert the task directly into SCX_DSQ_LOCAL and skip the
     * ops.enqueue() callback.
     *
     * Note that this implementation has exactly the same behavior as the
     * default ops.select_cpu implementation. The behavior of the scheduler
     * would be exactly same if the implementation just didn't define the
     * simple_select_cpu() struct_ops prog.
     */
    s32 BPF_STRUCT_OPS(simple_select_cpu, struct task_struct *p,
                       s32 prev_cpu, u64 wake_flags)
    {
            s32 cpu;
            /* Need to initialize or the BPF verifier will reject the program */
            bool direct = false;

            cpu = scx_bpf_select_cpu_dfl(p, prev_cpu, wake_flags, &direct);

            if (direct)
                    scx_bpf_dsq_insert(p, SCX_DSQ_LOCAL, SCX_SLICE_DFL, 0);

            return cpu;
    }

    /*
     * Do a direct insertion of a task to the global DSQ. This ops.enqueue()
     * callback will only be invoked if we failed to find a core to insert
     * into in ops.select_cpu() above.
     *
     * Note that this implementation has exactly the same behavior as the
     * default ops.enqueue implementation, which just dispatches the task
     * to SCX_DSQ_GLOBAL. The behavior of the scheduler would be exactly same
     * if the implementation just didn't define the simple_enqueue struct_ops
     * prog.
     */
    void BPF_STRUCT_OPS(simple_enqueue, struct task_struct *p, u64 enq_flags)
    {
            scx_bpf_dsq_insert(p, SCX_DSQ_GLOBAL, SCX_SLICE_DFL, enq_flags);
    }

    s32 BPF_STRUCT_OPS_SLEEPABLE(simple_init)
    {
            /*
             * By default, all SCHED_EXT, SCHED_OTHER, SCHED_IDLE, and
             * SCHED_BATCH tasks should use sched_ext.
             */
            return 0;
    }

    void BPF_STRUCT_OPS(simple_exit, struct scx_exit_info *ei)
    {
            exit_type = ei->type;
    }

    SEC(".struct_ops")
    struct sched_ext_ops simple_ops = {
            .select_cpu             = (void *)simple_select_cpu,
            .enqueue                = (void *)simple_enqueue,
            .init                   = (void *)simple_init,
            .exit                   = (void *)simple_exit,
            .name                   = "simple",
    };

Dispatch Queues
---------------
调度队列

To match the impedance between the scheduler core and the BPF scheduler,
sched_ext uses DSQs (dispatch queues) which can operate as both a FIFO and a
priority queue. By default, there is one global FIFO (``SCX_DSQ_GLOBAL``),
and one local DSQ per CPU (``SCX_DSQ_LOCAL``). The BPF scheduler can manage
an arbitrary number of DSQs using ``scx_bpf_create_dsq()`` and
``scx_bpf_destroy_dsq()``.
为了匹配调度器核心和 BPF 调度器之间的阻抗，sched_ext 使用 DSQ（调度队列），它可以同时作为 FIFO 和优先级队列运行。默认情况下，有一个全局 FIFO（``SCX_DSQ_GLOBAL``）和每个 CPU 一个本地 DSQ（``SCX_DSQ_LOCAL``）。BPF 调度器可以使用 ``scx_bpf_create_dsq()`` 和 ``scx_bpf_destroy_dsq()`` 管理任意数量的 DSQ。

A CPU always executes a task from its local DSQ. A task is "inserted" into a
DSQ. A task in a non-local DSQ is "move"d into the target CPU's local DSQ.
CPU 总是从其本地 DSQ 执行任务。任务被"插入"到 DSQ 中。非本地 DSQ 中的任务被"移动"到目标 CPU 的本地 DSQ。

When a CPU is looking for the next task to run, if the local DSQ is not
empty, the first task is picked. Otherwise, the CPU tries to move a task
from the global DSQ. If that doesn't yield a runnable task either,
``ops.dispatch()`` is invoked.
当 CPU 寻找下一个要运行的任务时，如果本地 DSQ 不为空，则选取第一个任务。否则，CPU 尝试从全局 DSQ 移动一个任务。如果仍然没有可运行的任务，则调用 ``ops.dispatch()``。

Scheduling Cycle
----------------
调度周期

The following briefly shows how a waking task is scheduled and executed.
以下简要展示了一个正在唤醒的任务是如何被调度和执行的。

1. When a task is waking up, ``ops.select_cpu()`` is the first operation
   invoked. This serves two purposes. First, CPU selection optimization
   hint. Second, waking up the selected CPU if idle.
1. 当任务正在唤醒时，``ops.select_cpu()`` 是第一个被调用的操作。这有两个目的。第一，CPU 选择优化提示。第二，如果选中的 CPU 处于空闲状态则唤醒它。

   The CPU selected by ``ops.select_cpu()`` is an optimization hint and not
   binding. The actual decision is made at the last step of scheduling.
   However, there is a small performance gain if the CPU
   ``ops.select_cpu()`` returns matches the CPU the task eventually runs on.
   ``ops.select_cpu()`` 选择的 CPU 是一个优化提示，并非绑定。实际决策在调度的最后一步做出。然而，如果 ``ops.select_cpu()`` 返回的 CPU 与任务最终运行的 CPU 匹配，会有微小的性能提升。

   A side-effect of selecting a CPU is waking it up from idle. While a BPF
   scheduler can wake up any cpu using the ``scx_bpf_kick_cpu()`` helper,
   using ``ops.select_cpu()`` judiciously can be simpler and more efficient.
   选择 CPU 的一个副作用是将其从空闲状态唤醒。虽然 BPF 调度器可以使用 ``scx_bpf_kick_cpu()`` 辅助函数唤醒任何 CPU，但明智地使用 ``ops.select_cpu()`` 可以更简单、更高效。

   Note that the scheduler core will ignore an invalid CPU selection, for
   example, if it's outside the allowed cpumask of the task.
   注意，调度器核心将忽略无效的 CPU 选择，例如，如果它在任务允许的 cpumask 之外。

   A task can be immediately inserted into a DSQ from ``ops.select_cpu()``
   by calling ``scx_bpf_dsq_insert()`` or ``scx_bpf_dsq_insert_vtime()``.
   任务可以通过调用 ``scx_bpf_dsq_insert()`` 或 ``scx_bpf_dsq_insert_vtime()`` 从 ``ops.select_cpu()`` 立即插入到 DSQ 中。

   If the task is inserted into ``SCX_DSQ_LOCAL`` from
   ``ops.select_cpu()``, it will be added to the local DSQ of whichever CPU
   is returned from ``ops.select_cpu()``. Additionally, inserting directly
   from ``ops.select_cpu()`` will cause the ``ops.enqueue()`` callback to
   be skipped.
   如果任务从 ``ops.select_cpu()`` 插入到 ``SCX_DSQ_LOCAL``，它将被添加到 ``ops.select_cpu()`` 返回的任意 CPU 的本地 DSQ。此外，从 ``ops.select_cpu()`` 直接插入将导致跳过 ``ops.enqueue()`` 回调。

   Any other attempt to store a task in BPF-internal data structures from
   ``ops.select_cpu()`` does not prevent ``ops.enqueue()`` from being
   invoked. This is discouraged, as it can introduce racy behavior or
   inconsistent state.
   从 ``ops.select_cpu()`` 将任务存储在 BPF 内部数据结构中的任何其他尝试都不会阻止 ``ops.enqueue()`` 被调用。不鼓励这样做，因为它可能引入竞态行为或不一致的状态。

2. Once the target CPU is selected, ``ops.enqueue()`` is invoked (unless the
   task was inserted directly from ``ops.select_cpu()``). ``ops.enqueue()``
   can make one of the following decisions:
2. 一旦选择了目标 CPU，就会调用 ``ops.enqueue()``（除非任务是从 ``ops.select_cpu()`` 直接插入的）。``ops.enqueue()`` 可以做出以下决策之一：

   * Immediately insert the task into either the global or a local DSQ by
     calling ``scx_bpf_dsq_insert()`` with one of the following options:
     ``SCX_DSQ_GLOBAL``, ``SCX_DSQ_LOCAL``, or ``SCX_DSQ_LOCAL_ON | cpu``.
   * 通过调用 ``scx_bpf_dsq_insert()`` 并传入以下选项之一，立即将任务插入到全局或本地 DSQ：``SCX_DSQ_GLOBAL``、``SCX_DSQ_LOCAL`` 或 ``SCX_DSQ_LOCAL_ON | cpu``。

   * Immediately insert the task into a custom DSQ by calling
     ``scx_bpf_dsq_insert()`` with a DSQ ID which is smaller than 2^63.
   * 通过调用 ``scx_bpf_dsq_insert()`` 并传入小于 2^63 的 DSQ ID，立即将任务插入到自定义 DSQ。

   * Queue the task on the BPF side.
   * 在 BPF 侧将任务排队。

   **Task State Tracking and ops.dequeue() Semantics**
   **任务状态跟踪和 ops.dequeue() 语义**

   A task is in the "BPF scheduler's custody" when the BPF scheduler is
   responsible for managing its lifecycle. A task enters custody when it is
   dispatched to a user DSQ or stored in the BPF scheduler's internal data
   structures. Custody is entered only from ``ops.enqueue()`` for those
   operations. The only exception is dispatching to a user DSQ from
   ``ops.select_cpu()``: although the task is not yet technically in BPF
   scheduler custody at that point, the dispatch has the same semantic
   effect as dispatching from ``ops.enqueue()`` for custody-related
   purposes.
   当 BPF 调度器负责管理任务的生命周期时，该任务处于"BPF 调度器的监管"之下。任务在被调度到用户 DSQ 或存储在 BPF 调度器的内部数据结构中时进入监管。对于这些操作，监管仅从 ``ops.enqueue()`` 进入。唯一的例外是从 ``ops.select_cpu()`` 调度到用户 DSQ：虽然此时任务在技术上尚未处于 BPF 调度器监管之下，但该调度在监管相关目的上与从 ``ops.enqueue()`` 调度具有相同的语义效果。

   Once ``ops.enqueue()`` is called, the task may or may not enter custody
   depending on what the scheduler does:
   一旦调用了 ``ops.enqueue()``，任务是否进入监管取决于调度器的行为：

   * **Directly dispatched to terminal DSQs** (``SCX_DSQ_LOCAL``,
     ``SCX_DSQ_LOCAL_ON | cpu``, or ``SCX_DSQ_GLOBAL``): the BPF scheduler
     is done with the task - it either goes straight to a CPU's local run
     queue or to the global DSQ as a fallback. The task never enters (or
     exits) BPF custody, and ``ops.dequeue()`` will not be called.
   * **直接调度到终端 DSQ**（``SCX_DSQ_LOCAL``、``SCX_DSQ_LOCAL_ON | cpu`` 或 ``SCX_DSQ_GLOBAL``）：BPF 调度器对该任务的处理完成——它要么直接进入 CPU 的本地运行队列，要么作为回退进入全局 DSQ。该任务从不进入（或退出）BPF 监管，且 ``ops.dequeue()`` 不会被调用。

   * **Dispatch to user-created DSQs** (custom DSQs): the task enters the
     BPF scheduler's custody. When the task later leaves BPF custody
     (dispatched to a terminal DSQ, picked by core-sched, or dequeued for
     sleep/property changes), ``ops.dequeue()`` will be called exactly
     once.
   * **调度到用户创建的 DSQ**（自定义 DSQ）：任务进入 BPF 调度器的监管。当任务随后离开 BPF 监管（调度到终端 DSQ、被核心调度选中、或因睡眠/属性变更而出队）时，``ops.dequeue()`` 将被恰好调用一次。

   * **Stored in BPF data structures** (e.g., internal BPF queues): the
     task is in BPF custody. ``ops.dequeue()`` will be called when it
     leaves (e.g., when ``ops.dispatch()`` moves it to a terminal DSQ, or
     on property change / sleep).
   * **存储在 BPF 数据结构中**（例如内部 BPF 队列）：任务处于 BPF 监管中。``ops.dequeue()`` 将在其离开时被调用（例如，当 ``ops.dispatch()`` 将其移动到终端 DSQ，或属性变更/睡眠时）。

   When a task leaves BPF scheduler custody, ``ops.dequeue()`` is invoked.
   The dequeue can happen for different reasons, distinguished by flags:
   当任务离开 BPF 调度器监管时，``ops.dequeue()`` 被调用。出队可能因不同原因发生，通过标志区分：

   1. **Regular dispatch**: when a task in BPF custody is dispatched to a
      terminal DSQ from ``ops.dispatch()`` (leaving BPF custody for
      execution), ``ops.dequeue()`` is triggered without any special flags.
   1. **常规调度**：当 BPF 监管中的任务从 ``ops.dispatch()`` 被调度到终端 DSQ（离开 BPF 监管以执行）时，``ops.dequeue()`` 被触发，不带任何特殊标志。

   2. **Core scheduling pick**: when ``CONFIG_SCHED_CORE`` is enabled and
      core scheduling picks a task for execution while it's still in BPF
      custody, ``ops.dequeue()`` is called with the
      ``SCX_DEQ_CORE_SCHED_EXEC`` flag.
   2. **核心调度选中**：当启用了 ``CONFIG_SCHED_CORE`` 且核心调度在任务仍处于 BPF 监管时选择其执行，``ops.dequeue()`` 会被调用，并带有 ``SCX_DEQ_CORE_SCHED_EXEC`` 标志。

   3. **Scheduling property change**: when a task property changes (via
      operations like ``sched_setaffinity()``, ``sched_setscheduler()``,
      priority changes, CPU migrations, etc.) while the task is still in
      BPF custody, ``ops.dequeue()`` is called with the
      ``SCX_DEQ_SCHED_CHANGE`` flag set in ``deq_flags``.
   3. **调度属性变更**：当任务属性变更（通过 ``sched_setaffinity()``、``sched_setscheduler()``、优先级变更、CPU 迁移等操作）且任务仍处于 BPF 监管中时，``ops.dequeue()`` 会被调用，并在 ``deq_flags`` 中设置 ``SCX_DEQ_SCHED_CHANGE`` 标志。

   **Important**: Once a task has left BPF custody (e.g., after being
   dispatched to a terminal DSQ), property changes will not trigger
   ``ops.dequeue()``, since the task is no longer managed by the BPF
   scheduler.
   **重要**：一旦任务离开了 BPF 监管（例如，被调度到终端 DSQ 后），属性变更将不会触发 ``ops.dequeue()``，因为该任务不再由 BPF 调度器管理。

3. When a CPU is ready to schedule, it first looks at its local DSQ. If
   empty, it then looks at the global DSQ. If there still isn't a task to
   run, ``ops.dispatch()`` is invoked which can use the following two
   functions to populate the local DSQ.
3. 当 CPU 准备进行调度时，它首先查看其本地 DSQ。如果为空，则查看全局 DSQ。如果仍然没有任务可运行，则调用 ``ops.dispatch()``，它可以使用以下两个函数来填充本地 DSQ。

   * ``scx_bpf_dsq_insert()`` inserts a task to a DSQ. Any target DSQ can be
     used - ``SCX_DSQ_LOCAL``, ``SCX_DSQ_LOCAL_ON | cpu``,
     ``SCX_DSQ_GLOBAL`` or a custom DSQ. While ``scx_bpf_dsq_insert()``
     currently can't be called with BPF locks held, this is being worked on
     and will be supported. ``scx_bpf_dsq_insert()`` schedules insertion
     rather than performing them immediately. There can be up to
     ``ops.dispatch_max_batch`` pending tasks.
   * ``scx_bpf_dsq_insert()`` 将任务插入到 DSQ。可以使用任何目标 DSQ——``SCX_DSQ_LOCAL``、``SCX_DSQ_LOCAL_ON | cpu``、``SCX_DSQ_GLOBAL`` 或自定义 DSQ。虽然 ``scx_bpf_dsq_insert()`` 当前不能在持有 BPF 锁的情况下调用，但这正在开发中并将得到支持。``scx_bpf_dsq_insert()`` 安排插入操作而非立即执行。最多可以有 ``ops.dispatch_max_batch`` 个待处理任务。

   * ``scx_bpf_dsq_move_to_local()`` moves a task from the specified non-local
     DSQ to the dispatching DSQ. This function cannot be called with any BPF
     locks held. ``scx_bpf_dsq_move_to_local()`` flushes the pending insertions
     tasks before trying to move from the specified DSQ.
   * ``scx_bpf_dsq_move_to_local()`` 将任务从指定的非本地 DSQ 移动到正在调度的 DSQ。此函数不能在持有任何 BPF 锁的情况下调用。``scx_bpf_dsq_move_to_local()`` 在尝试从指定 DSQ 移动之前会刷新待处理的插入任务。

4. After ``ops.dispatch()`` returns, if there are tasks in the local DSQ,
   the CPU runs the first one. If empty, the following steps are taken:
4. 在 ``ops.dispatch()`` 返回后，如果本地 DSQ 中有任务，CPU 运行第一个任务。如果为空，则执行以下步骤：

   * Try to move from the global DSQ. If successful, run the task.
   * 尝试从全局 DSQ 移动。如果成功，运行该任务。

   * If ``ops.dispatch()`` has dispatched any tasks, retry #3.
   * 如果 ``ops.dispatch()`` 已调度了任何任务，重试第 3 步。

   * If the previous task is an SCX task and still runnable, keep executing
     it (see ``SCX_OPS_ENQ_LAST``).
   * 如果前一个任务是 SCX 任务且仍然可运行，继续执行它（参见 ``SCX_OPS_ENQ_LAST``）。

   * Go idle.
   * 进入空闲状态。

Note that the BPF scheduler can always choose to dispatch tasks immediately
in ``ops.enqueue()`` as illustrated in the above simple example. If only the
built-in DSQs are used, there is no need to implement ``ops.dispatch()`` as
a task is never queued on the BPF scheduler and both the local and global
DSQs are executed automatically.
注意，BPF 调度器始终可以选择在 ``ops.enqueue()`` 中立即调度任务，如上文的简单示例所示。如果只使用内置 DSQ，则无需实现 ``ops.dispatch()``，因为任务永远不会在 BPF 调度器上排队，本地和全局 DSQ 都会自动执行。

``scx_bpf_dsq_insert()`` inserts the task on the FIFO of the target DSQ. Use
``scx_bpf_dsq_insert_vtime()`` for the priority queue. Internal DSQs such as
``SCX_DSQ_LOCAL`` and ``SCX_DSQ_GLOBAL`` do not support priority-queue
dispatching, and must be dispatched to with ``scx_bpf_dsq_insert()``. See
the function documentation and usage in ``tools/sched_ext/scx_simple.bpf.c``
for more information.
``scx_bpf_dsq_insert()`` 将任务插入到目标 DSQ 的 FIFO 中。对于优先级队列，使用 ``scx_bpf_dsq_insert_vtime()``。诸如 ``SCX_DSQ_LOCAL`` 和 ``SCX_DSQ_GLOBAL`` 之类的内部 DSQ 不支持优先级队列调度，必须使用 ``scx_bpf_dsq_insert()`` 进行调度。更多信息请参见函数文档和 ``tools/sched_ext/scx_simple.bpf.c`` 中的用法。

Task Lifecycle
--------------
任务生命周期

The following pseudo-code presents a rough overview of the entire lifecycle
of a task managed by a sched_ext scheduler:
以下伪代码粗略概述了由 sched_ext 调度器管理的任务的整个生命周期：

.. code-block:: c

    ops.init_task();            /* A new task is created */
    ops.enable();               /* Enable BPF scheduling for the task */

    while (task in SCHED_EXT) {
        if (task can migrate)
            ops.select_cpu();   /* Called on wakeup (optimization) */

        ops.runnable();         /* Task becomes ready to run */

        while (task_is_runnable(task)) {
            if (task is not in a DSQ || task->scx.slice == 0) {
                ops.enqueue();  /* Task can be added to a DSQ */

                /* Task property change (i.e., affinity, nice, etc.)? */
                if (sched_change(task)) {
                    ops.dequeue(); /* Exiting BPF scheduler custody */
                    ops.quiescent();

                    /* Property change callback, e.g. ops.set_weight() */

                    ops.runnable();
                    continue;
                }

                /* Any usable CPU becomes available */

                ops.dispatch();     /* Task is moved to a local DSQ */
                ops.dequeue();      /* Exiting BPF scheduler custody */
            }

            ops.running();      /* Task starts running on its assigned CPU */

            while (task_is_runnable(task) && task->scx.slice > 0) {
                ops.tick();     /* Called every 1/HZ seconds */

                if (task->scx.slice == 0)
                    ops.dispatch(); /* task->scx.slice can be refilled */
            }

            ops.stopping();     /* Task stops running (time slice expires or wait) */
        }

        ops.quiescent();        /* Task releases its assigned CPU (wait) */
    }

    ops.disable();              /* Disable BPF scheduling for the task */
    ops.exit_task();            /* Task is destroyed */

Note that the above pseudo-code does not cover all possible state transitions
and edge cases, to name a few examples:
注意，上述伪代码并未涵盖所有可能的状态转换和边界情况，仅举几例：

* ``ops.dispatch()`` may fail to move the task to a local DSQ due to a racing
  property change on that task, in which case ``ops.dispatch()`` will be
  retried.
* ``ops.dispatch()`` 可能由于该任务的竞态属性变更而无法将任务移动到本地 DSQ，在这种情况下 ``ops.dispatch()`` 将被重试。

* The task may be direct-dispatched to a local DSQ from ``ops.enqueue()``,
  in which case ``ops.dispatch()`` and ``ops.dequeue()`` are skipped and we go
  straight to ``ops.running()``.
* 任务可能从 ``ops.enqueue()`` 被直接调度到本地 DSQ，在这种情况下 ``ops.dispatch()`` 和 ``ops.dequeue()`` 被跳过，直接进入 ``ops.running()``。

* Property changes may occur at virtually any point during the task's lifecycle,
  not just when the task is queued and waiting to be dispatched. For example,
  changing a property of a running task will lead to the callback sequence
  ``ops.stopping()`` -> ``ops.quiescent()`` -> (property change callback) ->
  ``ops.runnable()`` -> ``ops.running()``.
* 属性变更实际上可能在任务生命周期的任何时刻发生，而不仅仅是当任务排队等待调度时。例如，更改正在运行的任务的属性将导致回调序列 ``ops.stopping()`` -> ``ops.quiescent()`` ->（属性变更回调）-> ``ops.runnable()`` -> ``ops.running()``。

* A sched_ext task can be preempted by a task from a higher-priority scheduling
  class, in which case it will exit the tick-dispatch loop even though it is runnable
  and has a non-zero slice.
* sched_ext 任务可以被来自更高优先级调度类的任务抢占，在这种情况下，即使它是可运行的且具有非零时间片，它也会退出 tick-dispatch 循环。

See the "Scheduling Cycle" section for a more detailed description of how
a freshly woken up task gets on a CPU.
有关刚唤醒的任务如何获取 CPU 的更详细描述，请参见"调度周期"一节。

Where to Look
=============
参考位置

* ``include/linux/sched/ext.h`` defines the core data structures, ops table
  and constants.
* ``include/linux/sched/ext.h`` 定义了核心数据结构、ops 表和常量。

* ``kernel/sched/ext.c`` contains sched_ext core implementation and helpers.
  The functions prefixed with ``scx_bpf_`` can be called from the BPF
  scheduler.
* ``kernel/sched/ext.c`` 包含 sched_ext 核心实现和辅助函数。以 ``scx_bpf_`` 为前缀的函数可以从 BPF 调度器调用。

* ``kernel/sched/ext_idle.c`` contains the built-in idle CPU selection policy.
* ``kernel/sched/ext_idle.c`` 包含内置的空闲 CPU 选择策略。

* ``tools/sched_ext/`` hosts example BPF scheduler implementations.
* ``tools/sched_ext/`` 存放示例 BPF 调度器实现。

  * ``scx_simple[.bpf].c``: Minimal global FIFO scheduler example using a
    custom DSQ.
  * ``scx_simple[.bpf].c``：使用自定义 DSQ 的最小全局 FIFO 调度器示例。

  * ``scx_qmap[.bpf].c``: A multi-level FIFO scheduler supporting five
    levels of priority implemented with ``BPF_MAP_TYPE_QUEUE``.
  * ``scx_qmap[.bpf].c``：一个多级 FIFO 调度器，支持使用 ``BPF_MAP_TYPE_QUEUE`` 实现的五级优先级。

  * ``scx_central[.bpf].c``: A central FIFO scheduler where all scheduling
    decisions are made on one CPU, demonstrating ``LOCAL_ON`` dispatching,
    tickless operation, and kthread preemption.
  * ``scx_central[.bpf].c``：一个中央 FIFO 调度器，所有调度决策都在一个 CPU 上做出，演示了 ``LOCAL_ON`` 调度、无 tick 操作和内核线程抢占。

  * ``scx_cpu0[.bpf].c``: A scheduler that queues all tasks to a shared DSQ
    and only dispatches them on CPU0 in FIFO order. Useful for testing bypass
    behavior.
  * ``scx_cpu0[.bpf].c``：一个将所有任务排队到共享 DSQ 并仅在 CPU0 上按 FIFO 顺序调度的调度器。用于测试旁路行为。

  * ``scx_flatcg[.bpf].c``: A flattened cgroup hierarchy scheduler
    implementing hierarchical weight-based cgroup CPU control by compounding
    each cgroup's share at every level into a single flat scheduling layer.
  * ``scx_flatcg[.bpf].c``：一个扁平化 cgroup 层级调度器，通过将每个 cgroup 在每一级的份额合并到单个扁平调度层中，实现基于权重的层级 cgroup CPU 控制。

  * ``scx_pair[.bpf].c``: A core-scheduling example that always makes
    sibling CPU pairs execute tasks from the same CPU cgroup.
  * ``scx_pair[.bpf].c``：一个核心调度示例，始终让兄弟 CPU 对执行来自同一 CPU cgroup 的任务。

  * ``scx_sdt[.bpf].c``: A variation of ``scx_simple`` demonstrating BPF
    arena memory management for per-task data.
  * ``scx_sdt[.bpf].c``：``scx_simple`` 的一个变体，演示了用于每任务数据的 BPF arena 内存管理。

  * ``scx_userland[.bpf].c``: A minimal scheduler demonstrating user space
    scheduling. Tasks with CPU affinity are direct-dispatched in FIFO order;
    all others are scheduled in user space by a simple vruntime scheduler.
  * ``scx_userland[.bpf].c``：一个演示用户空间调度的最小调度器。具有 CPU 亲和性的任务按 FIFO 顺序直接调度；所有其他任务在用户空间由简单的虚拟运行时调度器调度。

Module Parameters
=================
模块参数

sched_ext exposes two module parameters under the ``sched_ext.`` prefix that
control bypass-mode behaviour. These knobs are primarily for debugging; there
is usually no reason to change them during normal operation. They can be read
and written at runtime (mode 0600) via
``/sys/module/sched_ext/parameters/``.
sched_ext 在 ``sched_ext.`` 前缀下暴露了两个控制旁路模式行为的模块参数。这些旋钮主要用于调试；在正常操作期间通常没有理由更改它们。可以通过 ``/sys/module/sched_ext/parameters/`` 在运行时（权限 0600）读取和写入。

``sched_ext.slice_bypass_us`` (default: 5000 µs)
    The time slice assigned to all tasks when the scheduler is in bypass mode,
    i.e. during BPF scheduler load, unload, and error recovery. Valid range is
    100 µs to 100 ms.
    ``sched_ext.slice_bypass_us``（默认值：5000 µs）
    当调度器处于旁路模式时（即 BPF 调度器加载、卸载和错误恢复期间）分配给所有任务的时间片。有效范围为 100 µs 到 100 ms。

``sched_ext.bypass_lb_intv_us`` (default: 500000 µs)
    The interval at which the bypass-mode load balancer redistributes tasks
    across CPUs. Set to 0 to disable load balancing during bypass mode. Valid
    range is 0 to 10 s.
    ``sched_ext.bypass_lb_intv_us``（默认值：500000 µs）
    旁路模式负载均衡器在 CPU 间重新分配任务的时间间隔。设置为 0 可禁用旁路模式期间的负载均衡。有效范围为 0 到 10 秒。

ABI Instability
===============
ABI 不稳定性

The APIs provided by sched_ext to BPF schedulers programs have no stability
guarantees. This includes the ops table callbacks and constants defined in
``include/linux/sched/ext.h``, as well as the ``scx_bpf_`` kfuncs defined in
``kernel/sched/ext.c`` and ``kernel/sched/ext_idle.c``.
sched_ext 提供给 BPF 调度器程序的 API 没有稳定性保证。这包括 ``include/linux/sched/ext.h`` 中定义的 ops 表回调和常量，以及 ``kernel/sched/ext.c`` 和 ``kernel/sched/ext_idle.c`` 中定义的 ``scx_bpf_`` kfunc。

While we will attempt to provide a relatively stable API surface when
possible, they are subject to change without warning between kernel
versions.
虽然我们将尽可能提供相对稳定的 API 表面，但它们在不同内核版本之间可能会在没有警告的情况下发生变化。

sched_ext 嵌入内核的调用链路
=============================

sched_ext 作为一个新的调度类（sched class），通过以下机制嵌入 Linux 内核调度框架。

优先级层级与链接器排布
----------------------

所有调度类通过链接器脚本按优先级排列，形成从高到低的顺序：

``include/asm-generic/vmlinux.lds.h`` (155-162行) ::

    __sched_class_highest = .;
    *(__stop_sched_class)
    *(__dl_sched_class)
    *(__rt_sched_class)
    *(__fair_sched_class)
    *(__ext_sched_class)        <-- sched_ext 排在 fair 之后、idle 之前
    *(__idle_sched_class)
    __sched_class_lowest = .;

``kernel/sched/sched.h`` (2728-2731行) 中 ``DEFINE_SCHED_CLASS`` 宏将每个调度类实例放入对应 section::

    #define DEFINE_SCHED_CLASS(name)                                     \\
        const struct sched_class name##_sched_class                       \\
            __aligned(__alignof__(struct sched_class))                    \\
            __section("__" #name "_sched_class")

``kernel/sched/ext.c`` (4546-4579行) 通过此宏定义 ``ext_sched_class``::

    DEFINE_SCHED_CLASS(ext) = {
        .enqueue_task       = enqueue_task_scx,
        .dequeue_task       = dequeue_task_scx,
        .pick_task          = pick_task_scx,
        .put_prev_task      = put_prev_task_scx,
        .set_next_task      = set_next_task_scx,
        .select_task_rq     = select_task_rq_scx,
        .task_tick          = task_tick_scx,
        .switching_to       = switching_to_scx,
        .switched_from      = switched_from_scx,
        .switched_to        = switched_to_scx,
        ...
    };

优先级校验与初始化
------------------

``kernel/sched/core.c`` (8907-8915行) 在 ``sched_init()`` 中校验各调度类的优先级顺序::

    BUG_ON(!sched_class_above(&stop_sched_class, &dl_sched_class));
    BUG_ON(!sched_class_above(&dl_sched_class, &rt_sched_class));
    BUG_ON(!sched_class_above(&rt_sched_class, &fair_sched_class));
    BUG_ON(!sched_class_above(&fair_sched_class, &ext_sched_class));
    BUG_ON(!sched_class_above(&ext_sched_class, &idle_sched_class));

``kernel/sched/core.c`` (9093行) 在 ``sched_init()`` 末尾调用 ``init_sched_ext_class()``::

    init_sched_ext_class();

``kernel/sched/ext.c`` (8030-8076行) 中 ``init_sched_ext_class()`` 完成以下初始化:

* 初始化每个 CPU 的本地 DSQ 及 per-CPU 状态
* 分配 kick/preempt/wait 等 cpumask
* 注册 SysRq 键 ``S``（重置）和 ``D``（调试转储）
* 初始化 watchdog 延迟工作队列

调度策略到调度类的映射
----------------------

``kernel/sched/core.c`` (7557-7571行)::

    const struct sched_class *__setscheduler_class(int policy, int prio)
    {
        if (dl_prio(prio))        return &dl_sched_class;
        if (rt_prio(prio))        return &rt_sched_class;
    #ifdef CONFIG_SCHED_CLASS_EXT
        if (task_should_scx(policy))  return &ext_sched_class;
    #endif
        return &fair_sched_class;
    }

``kernel/sched/core.c`` (4734-4740行) 在 ``__sched_fork()`` 中根据策略设置任务调度类::

    p->sched_class = &rt_sched_class;
    #ifdef CONFIG_SCHED_CLASS_EXT
        p->sched_class = &ext_sched_class;
    #endif
        p->sched_class = &fair_sched_class;

``kernel/sched/syscalls.c`` (1482行, 1510行) 在 ``__sched_setscheduler()`` 中识别 ``SCHED_EXT`` 策略。

任务是否在 sched_ext 上的判断
-----------------------------

``kernel/sched/ext.h`` (33-36行)::

    static inline bool task_on_scx(const struct task_struct *p)
    {
        return scx_enabled() && p->sched_class == &ext_sched_class;
    }

``kernel/sched/ext.h`` (35行) 通过 ``scx_enabled()`` 检查 BPF 调度器是否已加载。

调度类的遍历与激活
------------------

``kernel/sched/sched.h`` (2747-2757行) 定义了 ``next_active_class()``，在遍历调度类时动态跳过::

    static inline const struct sched_class *next_active_class(const struct sched_class *class)
    {
        class++;
    #ifdef CONFIG_SCHED_CLASS_EXT
        if (scx_switched_all() && class == &fair_sched_class)
            class++;        /* 当 sched_ext 接管所有 fair 任务时，跳过 fair */
        if (!scx_enabled() && class == &ext_sched_class)
            class++;        /* 当 sched_ext 未启用时，跳过 ext */
    #endif
        return class;
    }

``kernel/sched/core.c`` (5995行) 的 ``pick_next_task()`` 通过 ``for_active_class_range()`` 遍历所有活跃调度类，依次调用 ``pick_task()``。

sched_ext 与其他调度类的交互点
------------------------------

各调度类文件中通过 ``#ifdef CONFIG_SCHED_CLASS_EXT`` 嵌入专用钩子:

+----------------------+---------------------+---------------------------------------------+
| 文件                 | 行号                | 钩子作用                                     |
+======================+=====================+=============================================+
| ``kernel/sched/idle.c`` | 561              | 空闲 CPU 选择时考虑 sched_ext 状态            |
| ``kernel/sched/deadline.c`` | 114, 1861, 3242, 3256 | DL 调度器与 ext 的交互，如 dl_server 选择 SCX 任务 |
| ``kernel/sched/topology.c`` | 511             | CPU 拓扑变化时通知 sched_ext                  |
| ``kernel/sched/debug.c`` | 448, 480, 514, 546, 571, 592, 632, 1368 | 调度调试信息中包含 sched_ext 状态     |
| ``kernel/sched/core.c`` | 262, 4487, 4735, 7565, 8834, 8913, 9093 | 核心调度器中的多个集成点             |
| ``kernel/sched/cpufreq_schedutil.c`` | （通过 scx_cpuperf_target() 调用） | CPU 频率调整考虑 sched_ext 性能目标 |

调用链路全景
------------

::

    sched_init()                              [core.c:8902]
      ├─ BUG_ON 校验调度类优先级顺序            [core.c:8907-8915]
      └─ init_sched_ext_class()               [core.c:9093]
           └─ init_sched_ext_class()          [ext.c:8030]
                ├─ scx_idle_init_masks()      初始化 idle masks
                ├─ per-CPU 初始化: DSQ、cpumask、irq_work
                ├─ register_sysrq_key('S')    注册 SysRq-S 重置
                ├─ register_sysrq_key('D')    注册 SysRq-D 调试转储
                └─ INIT_DELAYED_WORK(watchdog) 初始化 watchdog

    __sched_fork() / sched_setscheduler()     [core.c:4734-4740]
      └─ p->sched_class = &ext_sched_class    (当 policy == SCHED_EXT)

    __setscheduler_class(policy, prio)         [core.c:7557]
      └─ task_should_scx(policy) → &ext_sched_class

    pick_next_task()                           [core.c:5995]
      └─ for_active_class_range(class, ...)   遍历调度类
           ├─ [scx_switched_all()] → 跳过 fair_sched_class
           ├─ [!scx_enabled()]     → 跳过 ext_sched_class
           └─ class->pick_task()              调用各调度类的 pick_task

    BPF 调度器加载 (用户空间)                  [tools/sched_ext/scx_simple]
      └─ struct_ops 注册 → 内核加载 BPF 程序
           └─ __scx_enabled 设为 true
                └─ 任务开始由 ext_sched_class 调度
                     ├─ enqueue_task_scx()     入队
                     ├─ dequeue_task_scx()     出队
                     ├─ select_task_rq_scx()   选择 CPU
                     ├─ pick_task_scx()        选择下一个任务
                     ├─ put_prev_task_scx()    放回前一个任务
                     ├─ set_next_task_scx()    设置下一个任务
                     ├─ task_tick_scx()        时钟 tick
                     └─ switching_to/from_scx() 调度类切换

    BPF 调度器卸载
      └─ __scx_enabled 设为 false
           └─ 所有任务回退到 fair_sched_class


