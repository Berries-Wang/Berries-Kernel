.. SPDX-License-Identifier: GPL-2.0

:orphan:

========================
MSM Preemption(MSM 抢占)
========================

Preemption allows Adreno GPUs to switch to a higher priority ring when work is
pushed to it, reducing latency for high priority submissions.
(抢占允许 Adreno GPU 在有工作推入更高优先级环形队列(ring)时切换到
该 ring，从而降低高优先级提交的延迟。)

When preemption is enabled 4 rings are initialized, corresponding to different
priority levels. Having multiple rings is purely a software concept as the GPU
only has registers to keep track of one graphics ring.
The kernel is able to switch which ring is currently being processed by
requesting preemption. When certain conditions are met, depending on the
priority level, the GPU will save its current state in a series of buffers,
then restores state from a similar set of buffers specified by the kernel. It
then resumes execution and fires an IRQ to let the kernel know the context
switch has completed.
(启用抢占后会初始化 4 个 ring，对应不同的优先级。拥有多个 ring 纯粹
是软件层面的概念，因为 GPU 只有用于跟踪一个图形 ring 的寄存器。内核
可以通过请求抢占来切换当前正在处理的 ring。当满足某些条件时(取决于
优先级)，GPU 会把当前状态保存到一系列缓冲区中，然后从内核指定的
一组类似缓冲区恢复状态。随后它恢复执行，并触发一个 IRQ 通知内核
上下文切换已完成。)

This mechanism can be used by the kernel to switch between rings. Whenever a
submission occurs the kernel finds the highest priority ring which isn't empty
and preempts to it if said ring is not the one being currently executed. This is
also done whenever a submission completes to make sure execution resumes on a
lower priority ring when a higher priority ring is done.
(内核可以利用该机制在 ring 之间切换。每当有提交发生时，内核会找到
非空的最高优先级 ring，如果该 ring 不是当前正在执行的 ring，就抢占到
它。每当一个提交完成时也会这样做，以确保高优先级 ring 完成后，执行
恢复到较低优先级的 ring 上。)

Preemption levels(抢占级别)
---------------------------

Preemption can only occur at certain boundaries. The exact conditions can be
configured by changing the preemption level, this allows to compromise between
latency (ie. the time that passes between when the kernel requests preemption
and when the SQE begins saving state) and overhead (the amount of state that
needs to be saved).
(抢占只能在某些边界处发生。具体条件可以通过更改抢占级别来配置，这
允许在延迟(即从内核请求抢占到 SQE 开始保存状态之间经过的时间)与开销
(需要保存的状态量)之间进行权衡。)

The GPU offers 3 levels:
(GPU 提供 3 个级别：)

Level 0
  Preemption only occurs at the submission level. This requires the least amount
  of state to be saved as the execution of userspace submitted IBs is never
  interrupted, however it offers very little benefit compared to not enabling
  preemption of any kind.
  (抢占只发生在提交级别。这需要保存的状态量最少，因为用户空间提交的
  IB 的执行永远不会被打断；但与完全不启用任何抢占相比，它带来的好处
  微乎其微。)

Level 1
  Preemption occurs at either bin level, if using GMEM rendering, or draw level
  in the sysmem rendering case.
  (如果使用 GMEM 渲染，抢占发生在 bin 级别；在 sysmem 渲染的情况下，
  抢占发生在 draw 级别。)

Level 2
  Preemption occurs at draw level.
  (抢占发生在 draw 级别。)

Level 1 is the mode that is used by the msm driver.
(msm 驱动使用的模式是 Level 1。)

Additionally the GPU allows to specify a `skip_save_restore` option. This
disables the saving and restoring of all registers except those relating to the
operation of the SQE itself, reducing overhead. Saving and restoring is only
skipped when using GMEM with Level 1 preemption. When enabling this userspace is
expected to set the state that isn't preserved whenever preemption occurs which
is done by specifying preamble and postambles. Those are IBs that are executed
before and after preemption.
(此外，GPU 允许指定 skip_save_restore 选项。它会禁用除与 SQE 自身操作
相关的寄存器之外所有寄存器的保存与恢复，从而减少开销。只有在使用
GMEM 且抢占级别为 Level 1 时才会跳过保存与恢复。启用此功能后，每当
抢占发生时，用户空间应设置未获保留的状态，具体通过指定前导
(preamble)和后置(postamble)IB 来完成。这些 IB 在抢占前后执行。)

Preemption buffers(抢占缓冲区)
------------------------------

A series of buffers are necessary to store the state of rings while they are not
being executed. There are different kinds of preemption records and most of
those require one buffer per ring. This is because preemption never occurs
between submissions on the same ring, which always run in sequence when the ring
is active. This means that only one context per ring is effectively active.
(需要一系列缓冲区来存储 ring 未被执行时的状态。抢占记录有多种类型，
其中大多数需要每个 ring 一个缓冲区。这是因为抢占从不会发生在同一
ring 的提交之间——当 ring 处于活动状态时，这些提交总是按顺序执行。
这意味着每个 ring 实际上只有一个活动上下文。)

SMMU_INFO
  This buffer contains info about the current SMMU configuration such as the
  ttbr0 register. The SQE firmware isn't actually able to save this record.
  As a result SMMU info must be saved manually from the CP to a buffer and the
  SMMU record updated with info from said buffer before triggering
  preemption.
  (此缓冲区包含当前 SMMU 配置的信息，例如 ttbr0 寄存器。SQE 固件实际
  上无法保存此记录。因此，必须在触发抢占之前，由 CP 手动把 SMMU 信息
  保存到一个缓冲区，并用该缓冲区中的信息更新 SMMU 记录。)

NON_SECURE
  This is the main preemption record where most state is saved. It is mostly
  opaque to the kernel except for the first few words that must be initialized
  by the kernel.
  (这是主要的抢占记录，大部分状态都保存在这里。对内核来说它基本上是
  不透明的，除了开头几个必须由内核初始化的字。)

SECURE
  This saves state related to the GPU's secure mode.
  (它保存与 GPU 安全模式相关的状态。)

NON_PRIV
  The intended purpose of this record is unknown. The SQE firmware actually
  ignores it and therefore msm doesn't handle it.
  (此记录的预期用途未知。SQE 固件实际上会忽略它，因此 msm 不处理它。)

COUNTER
  This record is used to save and restore performance counters.
  (此记录用于保存和恢复性能计数器。)

Handling the permissions of those buffers is critical for security. All but the
NON_PRIV records need to be inaccessible from userspace, so they must be mapped
in the kernel address space with the MSM_BO_MAP_PRIV flag.
For example, making the NON_SECURE record accessible from userspace would allow
any process to manipulate a saved ring's RPTR which can be used to skip the
execution of some packets in a ring and execute user commands with higher
privileges.
(处理这些缓冲区的权限对安全至关重要。除 NON_PRIV 记录之外，所有记录
都需要对用户空间不可访问，因此必须用 MSM_BO_MAP_PRIV 标志映射到内核
地址空间。例如，让 NON_SECURE 记录可被用户空间访问，将使任何进程都
能操纵已保存 ring 的 RPTR，这可用于跳过 ring 中某些包的执行，并以
更高权限执行用户命令。)
