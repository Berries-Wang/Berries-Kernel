============================================================
Long running workloads and compute(长时运行的工作负载与计算)
============================================================

Long running workloads (compute) are workloads that will not complete in 10
seconds. (The time let the user wait before he reaches for the power button).
This means that other techniques need to be used to manage those workloads,
that cannot use fences.
(长时运行的工作负载 (计算负载) 是指无法在 10 秒内完成的工作负载。(也就是
用户在伸手去按电源按钮之前愿意等待的时间)。这意味着需要用其他技术来管理
这些无法使用 fence 的工作负载。)

Some hardware may schedule compute jobs, and have no way to pre-empt them, or
have their memory swapped out from them. Or they simply want their workload
not to be preempted or swapped out at all.
(有些硬件可以调度计算作业，但没有办法抢占它们，或把它们占用的内存换出。
也有些硬件只是希望其工作负载完全不被抢占或换出。)

This means that it differs from what is described in driver-api/dma-buf.rst.
(这意味着它与 driver-api/dma-buf.rst 中描述的情况有所不同。)

As with normal compute jobs, dma-fence may not be used at all. In this case,
not even to force preemption. The driver with is simply forced to unmap a BO
from the long compute job's address space on unbind immediately, not even
waiting for the workload to complete. Effectively this terminates the workload
when there is no hardware support to recover.
(与普通计算作业一样，dma-fence 可能完全无法使用，甚至连强制抢占也不行。
驱动只能在解绑 (unbind) 时立即把 BO 从长时计算作业的地址空间解除映射，
甚至不等工作负载完成。在缺乏硬件恢复支持的情况下，这实际上会终止工作
负载。)

Since this is undesirable, there need to be mitigations to prevent a workload
from being terminated. There are several possible approach, all with their
advantages and drawbacks.
(由于这是不可取的，因此需要一些缓解措施来防止工作负载被终止。有几种可能的
方案，各有优缺点。)

The first approach you will likely try is to pin all buffers used by compute.
This guarantees that the job will run uninterrupted, but also allows a very
denial of service attack by pinning as much memory as possible, hogging the
all GPU memory, and possibly a huge chunk of CPU memory.
(你可能首先会尝试的方案是固定 (pin) 计算使用的所有缓冲区。这能保证作业不
间断运行，但也允许一种非常恶劣的拒绝服务攻击：尽可能多地固定内存，霸占全部
GPU 内存，可能还霸占一大块 CPU 内存。)

A second approach that will work slightly better on its own is adding an option
not to evict when creating a new job (any kind). If all of userspace opts in
to this flag, it would prevent cooperating userspace from forced terminating
older compute jobs to start a new one.
(第二种单独效果稍好的方案是增加一个选项：创建新作业 (任何类型) 时不进行
逐出 (evict)。如果所有用户态程序都选择使用此标志，就可以防止配合良好的
用户态程序被迫终止较旧的计算作业来启动新作业。)

If job preemption and recoverable pagefaults are not available, those are the
only approaches possible. So even with those, you want a separate way of
controlling resources. The standard kernel way of doing so is cgroups.
(如果不具备作业抢占和可恢复缺页，这些就是仅有的可行方案。因此即便有了这些，
你也需要一种独立的资源控制方式。内核中这样做的标准方式是 cgroups。)

This creates a third option, using cgroups to prevent eviction. Both GPU and
driver-allocated CPU memory would be accounted to the correct cgroup, and
eviction would be made cgroup aware. This allows the GPU to be partitioned
into cgroups, that will allow jobs to run next to each other without
interference.
(这就产生了第三种方案：使用 cgroups 防止逐出。GPU 内存和驱动分配的 CPU
内存都会被记账到正确的 cgroup，逐出机制也将感知 cgroup。这允许把 GPU 按
cgroup 划分，使各作业可以在互不干扰的情况下并排运行。)

The interface to the cgroup would be similar to the current CPU memory
interface, with similar semantics for min/low/high/max, if eviction can
be made cgroup aware.
(如果逐出机制可以做到感知 cgroup，与 cgroup 的接口将类似于当前的 CPU 内存
接口，min/low/high/max 的语义也类似。)

What should be noted is that each memory region (tiled memory for example)
should have its own accounting.
(需要注意的是，每个内存区域 (例如分块内存 tiled memory) 都应有自己的
记账。)

The key is set to the regionid set by the driver, for example "tile0".
For the value of $card, we use drmGetUnique().
(键设置为驱动设定的 regionid，例如 "tile0"。$card 的值使用 drmGetUnique()。)
