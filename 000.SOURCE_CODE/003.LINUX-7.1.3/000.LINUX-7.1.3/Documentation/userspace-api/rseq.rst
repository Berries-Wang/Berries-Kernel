=================================
Restartable Sequences(可重启序列)
=================================

Restartable Sequences allow to register a per thread userspace memory area
to be used as an ABI between kernel and userspace for three purposes:
(可重启序列 (Restartable Sequences) 允许注册一个每线程的用户态内存区域，作为内核与用户态之间的 ABI，用于三种目的：)

 * userspace restartable sequences
   (用户态可重启序列)

 * quick access to read the current CPU number, node ID from userspace
   (从用户态快速读取当前 CPU 编号与节点 ID)

 * scheduler time slice extensions
   (调度器时间片扩展)

Restartable sequences (per-cpu atomics)(可重启序列 (per-cpu 原子操作))
----------------------------------------------------------------------

Restartable sequences allow userspace to perform update operations on
per-cpu data without requiring heavyweight atomic operations. The actual
ABI is unfortunately only available in the code and selftests.
(可重启序列允许用户态在不依赖重量级原子操作的情况下对 per-cpu 数据执行更新操作。遗憾的是，实际的 ABI 只能在代码与自测中找到。)

Quick access to CPU number, node ID(快速访问 CPU 编号与节点 ID)
---------------------------------------------------------------

Allows to implement per CPU data efficiently. Documentation is in code and
selftests. :(
(可以高效地实现 per-CPU 数据。文档在代码与自测中。:( )

Optimized RSEQ V2(优化的 RSEQ V2)
---------------------------------

On architectures which utilize the generic entry code and generic TIF bits
the kernel supports runtime optimizations for RSEQ, which also enable
enhanced features like scheduler time slice extensions.
(在使用通用入口代码 (generic entry code) 与通用 TIF 位的架构上，内核支持 RSEQ 的运行时优化，这些优化还启用了调度器时间片扩展等增强特性。)

To enable them a task has to register the RSEQ region with at least the
length advertised by getauxval(AT_RSEQ_FEATURE_SIZE).
(要启用这些特性，任务注册 RSEQ 区域时长度至少要达到 getauxval(AT_RSEQ_FEATURE_SIZE) 报告的值。)

If existing binaries register with RSEQ_ORIG_SIZE (32 bytes), the kernel
keeps the legacy low performance mode enabled to fulfil the expectations
of existing users regarding the original RSEQ implementation behaviour.
(如果现有二进制程序以 RSEQ_ORIG_SIZE (32 字节) 注册，内核会保持启用旧式低性能模式，以满足现有用户对原始 RSEQ 实现行为的预期。)

The following table documents the ABI and behavioral guarantees of the
legacy and the optimized V2 mode.
(下表记录了旧模式与优化 V2 模式的 ABI 与行为保证。)

.. list-table:: RSEQ modes
   :header-rows: 1

   * - Nr(编号)
     - What(项目)

     - Legacy(旧模式)
     - Optimized V2(优化的 V2)

   * - 1
     - The cpu_id_start, cpu_id, node_id and mm_cid fields (User mode read
       only)(cpu_id_start、cpu_id、node_id 与 mm_cid 字段，用户态只读)
       .. Legacy
     - Updated by the kernel unconditionally after each context switch and
       before signal delivery(内核在每次上下文切换之后、信号投递之前无条件更新)
       .. Optimized V2
     - Updated by the kernel if and only if they change, i.e. if the task
       is migrated or mm_cid changes(内核当且仅当它们发生变化时更新，即任务被迁移或 mm_cid 变化时)

   * - 2
     - The rseq_cs critical section field(rseq_cs 临界区字段)
       .. Legacy
     - Evaluated and handled unconditionally after each context switch and
       before signal delivery(在每次上下文切换之后、信号投递之前无条件评估并处理)
       .. Optimized V2
     - Evaluated and handled conditionally only when user space was
       interrupted and was scheduled out or before delivering a signal in
       the interrupted context.(仅当用户态被中断且被调度出去时、或在被中断上下文中投递信号之前，才有条件地评估并处理)

   * - 3
     - Read only fields(只读字段)
       .. Legacy
     - No strict enforcement except in debug mode(除调试模式外不做严格强制)
       .. Optimized V2
     - Strict enforcement(严格强制)

   * - 4
     - membarrier(...RSEQ)
       .. Legacy
     - All running threads of the process are interrupted and the ID fields
       are rewritten and eventually active critical sections are aborted
       before they return to user space.  All threads which are scheduled
       out whether voluntary or not are covered by #1/#2 above.
       (进程所有正在运行的线程被中断，ID 字段被重写，活动的临界区最终在这些线程返回用户态之前被中止。无论自愿与否被调度出去的线程都已由上述 #1/#2 覆盖。)
       .. Optimized V2
     - All running threads of the process are interrupted and eventually
       active critical sections are aborted before these threads return to
       user space. The ID fields are only updated if changed as a
       consequence of the interrupt. All threads which are scheduled out
       whether voluntary or not are covered by #1/#2 above.
       (进程所有正在运行的线程被中断，活动的临界区最终在这些线程返回用户态之前被中止。ID 字段仅在因中断而发生变化时才更新。无论自愿与否被调度出去的线程都已由上述 #1/#2 覆盖。)

   * - 5
     - Time slice extensions(时间片扩展)
       .. Legacy
     - Not supported(不支持)
       .. Optimized V2
     - Supported(支持)

The legacy mode is obviously less performant as it does unconditional
updates and critical section checks even if not strictly required by the
ABI contract. That can't be changed anymore as some users depend on that
observed behavior, which in turn enables them to violate the ABI and
overwrite the cpu_id_start field for their own purposes. This is obviously
discouraged as it renders RSEQ incompatible with the intended usage and
breaks the expectation of other libraries in the same application.
(旧模式的性能明显较差，因为它即使 ABI 契约并不严格要求，也会做无条件更新与临界区检查。这一点已经无法改变，因为一些用户依赖这种被观察到的行为，这反过来使他们能够违反 ABI、为自身目的改写 cpu_id_start 字段。这显然不被鼓励，因为它使 RSEQ 与预期用法不兼容，并破坏同一应用中其他库的预期。)

The ABI compliant optimized v2 mode, which respects the read only fields,
does not require unconditional updates and therefore is way more
performant. The kernel validates the read only fields for compliance. If
user space modifies them, the process is killed. Compliant usage allows
multiple libraries in the same application to benefit from the RSEQ
functionality without disturbing each other. The ABI compliant optimized v2
mode also enables extended RSEQ features like time slice extensions.
(遵循 ABI 的优化 v2 模式尊重只读字段，不需要无条件更新，因此性能好得多。内核校验只读字段是否符合规范。如果用户态修改它们，进程会被杀死。合规的使用允许同一应用中的多个库互不干扰地受益于 RSEQ 功能。遵循 ABI 的优化 v2 模式还启用诸如时间片扩展之类的扩展 RSEQ 特性。)


Scheduler time slice extensions(调度器时间片扩展)
--------------------------------------------------

This allows a thread to request a time slice extension when it enters a
critical section to avoid contention on a resource when the thread is
scheduled out inside of the critical section.
(这允许线程在进入临界区时请求时间片扩展，以避免线程在临界区内被调度出去时对某个资源的争用。)

The prerequisites for this functionality are:
(该功能的前提条件是：)

    * Enabled in Kconfig
      (在 Kconfig 中启用)

    * Enabled at boot time (default is enabled)
      (在启动时启用 (默认启用))

    * A rseq userspace pointer has been registered for the thread in
      optimized V2 mode
      (已为该线程以优化 V2 模式注册 rseq 用户态指针)

The thread has to enable the functionality via prctl(2)::

    prctl(PR_RSEQ_SLICE_EXTENSION, PR_RSEQ_SLICE_EXTENSION_SET,
          PR_RSEQ_SLICE_EXT_ENABLE, 0, 0);

(线程必须通过 prctl(2) 启用该功能。)

prctl() returns 0 on success or otherwise with the following error codes:
(成功时 prctl() 返回 0，否则返回以下错误码之一：)

========= ==============================================================
Errorcode Meaning(含义)
========= ==============================================================
EINVAL	  Functionality not available or invalid function arguments.
          Note: arg4 and arg5 must be zero(功能不可用或函数参数无效。
          注意：arg4 与 arg5 必须为零)
ENOTSUPP  Functionality was disabled on the kernel command line(该功能在内核命令行上被禁用)
ENXIO	  Available, but no rseq user struct registered(可用，但未注册 rseq 用户结构体)
========= ==============================================================

The state can be also queried via prctl(2)::

  prctl(PR_RSEQ_SLICE_EXTENSION, PR_RSEQ_SLICE_EXTENSION_GET, 0, 0, 0);

(也可以通过 prctl(2) 查询状态。)

prctl() returns ``PR_RSEQ_SLICE_EXT_ENABLE`` when it is enabled or 0 if
disabled. Otherwise it returns with the following error codes:
(启用时 prctl() 返回 ``PR_RSEQ_SLICE_EXT_ENABLE``，禁用时返回 0。否则它返回以下错误码：)

========= ==============================================================
Errorcode Meaning(含义)
========= ==============================================================
EINVAL	  Functionality not available or invalid function arguments.
          Note: arg3 and arg4 and arg5 must be zero(功能不可用或函数参数无效。
          注意：arg3、arg4 与 arg5 必须为零)
========= ==============================================================

The availability and status is also exposed via the rseq ABI struct flags
field via the ``RSEQ_CS_FLAG_SLICE_EXT_AVAILABLE_BIT`` and the
``RSEQ_CS_FLAG_SLICE_EXT_ENABLED_BIT``. These bits are read-only for user
space and only for informational purposes.
(可用性与状态还通过 rseq ABI 结构体的 flags 字段暴露，即 ``RSEQ_CS_FLAG_SLICE_EXT_AVAILABLE_BIT`` 与
``RSEQ_CS_FLAG_SLICE_EXT_ENABLED_BIT`` 。这些位对用户态只读，仅供提供信息之用。)

If the mechanism was enabled via prctl(), the thread can request a time
slice extension by setting rseq::slice_ctrl::request to 1. If the thread is
interrupted and the interrupt results in a reschedule request in the
kernel, then the kernel can grant a time slice extension and return to
userspace instead of scheduling out. The length of the extension is
determined by debugfs:rseq/slice_ext_nsec. The default value is 5 usec; which
is the minimum value. It can be incremented to 50 usecs, however doing so
can/will affect the minimum scheduling latency.
(如果该机制已通过 prctl() 启用，线程可以把 rseq::slice_ctrl::request 设为 1 来请求时间片扩展。如果线程被中断且该中断在内核中导致重新调度请求，那么内核可以授予时间片扩展并返回用户态，而不是把它调度出去。扩展的时长由 debugfs:rseq/slice_ext_nsec 决定。默认值为 5 微秒，即最小值。可以增加到 50 微秒，但这样做会/将影响最小调度延迟。)

Any proposed changes to this default will have to come with a selftest and
rseq-slice-hist.py output that shows the new value has merrit.
(对该默认值的任何修改提议都必须附带一个自测以及能表明新值确有价值的 rseq-slice-hist.py 输出。)

The kernel indicates the grant by clearing rseq::slice_ctrl::request and
setting rseq::slice_ctrl::granted to 1. If there is a reschedule of the
thread after granting the extension, the kernel clears the granted bit to
indicate that to userspace.
(内核通过清除 rseq::slice_ctrl::request 并把 rseq::slice_ctrl::granted 置为 1 来表明授予。如果在授予扩展之后该线程又被重新调度，内核会清除 granted 位以把这一点告知用户态。)

If the request bit is still set when the leaving the critical section,
userspace can clear it and continue.
(如果离开临界区时 request 位仍然置位，用户态可以清除它并继续。)

If the granted bit is set, then userspace invokes rseq_slice_yield(2) when
leaving the critical section to relinquish the CPU. The kernel enforces
this by arming a timer to prevent misbehaving userspace from abusing this
mechanism.
(如果 granted 位置位，用户态应在离开临界区时调用 rseq_slice_yield(2) 来放弃 CPU。内核通过设定一个定时器来强制这一点，防止行为不端的用户态滥用该机制。)

If both the request bit and the granted bit are false when leaving the
critical section, then this indicates that a grant was revoked and no
further action is required by userspace.
(如果离开临界区时 request 位与 granted 位都为假，则表示授予已被撤销，用户态无需进一步操作。)

The required code flow is as follows::

    rseq->slice_ctrl.request = 1;
    barrier();  // Prevent compiler reordering
    critical_section();
    barrier();  // Prevent compiler reordering
    rseq->slice_ctrl.request = 0;
    if (rseq->slice_ctrl.granted)
        rseq_slice_yield();

(所需的代码流程如上。)

As all of this is strictly CPU local, there are no atomicity requirements.
Checking the granted state is racy, but that cannot be avoided at all::

    if (rseq->slice_ctrl.granted)
      -> Interrupt results in schedule and grant revocation
        rseq_slice_yield();

(由于所有这些都严格限于 CPU 本地，没有原子性要求。检查 granted 状态存在竞态，但这完全无法避免。)

So there is no point in pretending that this might be solved by an atomic
operation.
(所以假装这可能用某个原子操作来解决是毫无意义的。)

If the thread issues a syscall other than rseq_slice_yield(2) within the
granted timeslice extension, the grant is also revoked and the CPU is
relinquished immediately when entering the kernel. This is required as
syscalls might consume arbitrary CPU time until they reach a scheduling
point when the preemption model is either NONE or VOLUNTARY and therefore
might exceed the grant by far.
(如果线程在授予的时间片扩展内发出了 rseq_slice_yield(2) 以外的系统调用，授予也会被撤销，且在进入内核时立即放弃 CPU。这是必需的，因为当抢占模型为 NONE 或 VOLUNTARY 时，系统调用可能消耗任意长的 CPU 时间才到达调度点，因此可能远远超出授予的时间。)

The preferred solution for user space is to use rseq_slice_yield(2) which
is side effect free. The support for arbitrary syscalls is required to
support onion layer architectured applications, where the code handling the
critical section and requesting the time slice extension has no control
over the code within the critical section.
(用户态首选的方案是使用没有副作用的 rseq_slice_yield(2)。支持任意系统调用是为了支持洋葱分层架构的应用，其中处理临界区并请求时间片扩展的代码无法控制临界区内的代码。)

The kernel enforces flag consistency and terminates the thread with SIGSEGV
if it detects a violation.
(内核强制标志一致性，并在检测到违反时以 SIGSEGV 终止该线程。)
