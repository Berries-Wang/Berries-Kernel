.. _amdgpu-userq:

=============================
 User Mode Queues(用户态队列)
=============================

Introduction(简介)
==================

Similar to the KFD, GPU engine queues move into userspace.  The idea is to let
user processes manage their submissions to the GPU engines directly, bypassing
IOCTL calls to the driver to submit work.  This reduces overhead and also allows
the GPU to submit work to itself.  Applications can set up work graphs of jobs
across multiple GPU engines without needing trips through the CPU.
(与 KFD 类似，GPU 引擎队列移入了用户空间。其想法是让用户进程直接管理向 GPU 引擎的提交，绕过向驱动提交工作的 IOCTL 调用。这降低了开销，还允许 GPU 向自身提交工作。应用程序可以跨多个 GPU 引擎构建作业的工作图(work graph)，而无需经过 CPU。)

UMDs directly interface with firmware via per application shared memory areas.
The main vehicle for this is queue.  A queue is a ring buffer with a read
pointer (rptr) and a write pointer (wptr).  The UMD writes IP specific packets
into the queue and the firmware processes those packets, kicking off work on the
GPU engines.  The CPU in the application (or another queue or device) updates
the wptr to tell the firmware how far into the ring buffer to process packets
and the rtpr provides feedback to the UMD on how far the firmware has progressed
in executing those packets.  When the wptr and the rptr are equal, the queue is
idle.
(UMD 通过每个应用专有的共享内存区域直接与固件交互。实现这一交互的主要载体是队列(queue)。队列是一个带有读指针(rptr)和写指针(wptr)的环形缓冲区。UMD 把 IP 特定的数据包写入队列，固件处理这些数据包，从而在 GPU 引擎上启动工作。应用程序中的 CPU(或另一个队列/设备)更新 wptr，告知固件要处理环形缓冲区中多远的数据包；而 rptr 则向 UMD 反馈固件执行这些数据包的进度。当 wptr 与 rptr 相等时，队列处于空闲状态。)

Theory of Operation(工作原理)
=============================

The various engines on modern AMD GPUs support multiple queues per engine with a
scheduling firmware which handles dynamically scheduling user queues on the
available hardware queue slots.  When the number of user queues outnumbers the
available hardware queue slots, the scheduling firmware dynamically maps and
unmaps queues based on priority and time quanta.  The state of each user queue
is managed in the kernel driver in an MQD (Memory Queue Descriptor).  This is a
buffer in GPU accessible memory that stores the state of a user queue.  The
scheduling firmware uses the MQD to load the queue state into an HQD (Hardware
Queue Descriptor) when a user queue is mapped.  Each user queue requires a
number of additional buffers which represent the ring buffer and any metadata
needed by the engine for runtime operation.  On most engines this consists of
the ring buffer itself, a rptr buffer (where the firmware will shadow the rptr
to userspace), a wptr buffer (where the application will write the wptr for the
firmware to fetch it), and a doorbell.  A doorbell is a piece of one of the
device's MMIO BARs which can be mapped to specific user queues.  When the
application writes to the doorbell, it will signal the firmware to take some
action. Writing to the doorbell wakes the firmware and causes it to fetch the
wptr and start processing the packets in the queue. Each 4K page of the doorbell
BAR supports specific offset ranges for specific engines.  The doorbell of a
queue must be mapped into the aperture aligned to the IP used by the queue
(e.g., GFX, VCN, SDMA, etc.).  These doorbell apertures are set up via NBIO
registers.  Doorbells are 32 bit or 64 bit (depending on the engine) chunks of
the doorbell BAR.  A 4K doorbell page provides 512 64-bit doorbells for up to
512 user queues.  A subset of each page is reserved for each IP type supported
on the device.  The user can query the doorbell ranges for each IP via the INFO
IOCTL.  See the IOCTL Interfaces section for more information.
(现代 AMD GPU 上的各种引擎支持每引擎多个队列，并由调度固件负责在可用的硬件队列槽位上动态调度用户队列。当用户队列的数量超过可用的硬件队列槽位时，调度固件会根据优先级与时间片动态地映射和取消映射队列。每个用户队列的状态在内核驱动中通过 MQD(内存队列描述符)管理。这是 GPU 可访问内存中的一个缓冲区，用于保存用户队列的状态。当用户队列被映射时，调度固件使用 MQD 把队列状态加载到 HQD(硬件队列描述符)中。每个用户队列还需要若干额外的缓冲区，用于表示环形缓冲区以及引擎运行所需的元数据。在大多数引擎上，这包括环形缓冲区本身、一个 rptr 缓冲区(固件把 rptr 影子复制给用户空间的地方)、一个 wptr 缓冲区(应用程序写入 wptr 供固件读取的地方)，以及一个门铃(doorbell)。门铃是设备某个 MMIO BAR 中的一小块区域，可以映射到特定的用户队列。应用程序写门铃时，会通知固件执行某些动作。写门铃会唤醒固件，使其读取 wptr 并开始处理队列中的数据包。门铃 BAR 的每个 4K 页为特定引擎支持特定的偏移范围。队列的门铃必须映射到与该队列所用 IP(例如 GFX、VCN、SDMA 等)对齐的窗口(aperture)中。这些门铃窗口通过 NBIO 寄存器设置。门铃是门铃 BAR 中 32 位或 64 位(取决于引擎)的块。一个 4K 门铃页提供 512 个 64 位门铃，最多可支持 512 个用户队列。每个页的一个子集被预留给设备支持的每种 IP 类型。用户可以通过 INFO IOCTL 查询每种 IP 的门铃范围。更多信息参见 IOCTL 接口一节。)

When an application wants to create a user queue, it allocates the necessary
buffers for the queue (ring buffer, wptr and rptr, context save areas, etc.).
These can be separate buffers or all part of one larger buffer.  The application
would map the buffer(s) into its GPUVM and use the GPU virtual addresses of for
the areas of memory they want to use for the user queue.  They would also
allocate a doorbell page for the doorbells used by the user queues.  The
application would then populate the MQD in the USERQ IOCTL structure with the
GPU virtual addresses and doorbell index they want to use.  The user can also
specify the attributes for the user queue (priority, whether the queue is secure
for protected content, etc.).  The application would then call the USERQ
CREATE IOCTL to create the queue using the specified MQD details in the IOCTL.
The kernel driver then validates the MQD provided by the application and
translates the MQD into the engine specific MQD format for the IP.  The IP
specific MQD would be allocated and the queue would be added to the run list
maintained by the scheduling firmware.  Once the queue has been created, the
application can write packets directly into the queue, update the wptr, and
write to the doorbell offset to kick off work in the user queue.
(当应用程序想要创建用户队列时，它会为队列分配所需的缓冲区(环形缓冲区、wptr 与 rptr、上下文保存区等)。这些可以是各自独立的缓冲区，也可以都包含在一个更大的缓冲区中。应用程序把这些缓冲区映射到自己的 GPUVM 中，并把想要用于用户队列的内存区域的 GPU 虚拟地址拿来使用。它们还会为用户队列所用的门铃分配一个门铃页。然后，应用程序在 USERQ IOCTL 结构中填入 MQD，写上想要使用的 GPU 虚拟地址与门铃索引。用户还可以指定用户队列的属性(优先级、队列是否对受保护内容安全等)。随后应用程序调用 USERQ CREATE IOCTL，使用 IOCTL 中指定的 MQD 详情来创建队列。内核驱动随后校验应用程序提供的 MQD，并把该 MQD 转换为该 IP 的引擎特定 MQD 格式。接着分配 IP 特定的 MQD，并把队列加入调度固件维护的运行列表。队列创建完成后，应用程序就可以直接把数据包写入队列、更新 wptr，并写门铃偏移来启动用户队列中的工作。)

When the application is done with the user queue, it would call the USERQ
FREE IOCTL to destroy it.  The kernel driver would preempt the queue and
remove it from the scheduling firmware's run list.  Then the IP specific MQD
would be freed and the user queue state would be cleaned up.
(当应用程序用完用户队列后，它会调用 USERQ FREE IOCTL 来销毁该队列。内核驱动会抢占(preempt)该队列，并把它从调度固件的运行列表中移除。然后释放 IP 特定的 MQD，并清理用户队列的状态。)

Some engines may require the aggregated doorbell too if the engine does not
support doorbells from unmapped queues.  The aggregated doorbell is a special
page of doorbell space which wakes the scheduler.  In cases where the engine may
be oversubscribed, some queues may not be mapped.  If the doorbell is rung when
the queue is not mapped, the engine firmware may miss the request.  Some
scheduling firmware may work around this by polling wptr shadows when the
hardware is oversubscribed, other engines may support doorbell updates from
unmapped queues.  In the event that one of these options is not available, the
kernel driver will map a page of aggregated doorbell space into each GPUVM
space.  The UMD will then update the doorbell and wptr as normal and then write
to the aggregated doorbell as well.
(如果引擎不支持来自未映射队列的门铃，某些引擎可能还需要聚合门铃(aggregated doorbell)。聚合门铃是门铃空间中一个特殊的页，用于唤醒调度器。在引擎可能被超额订阅(oversubscribed)的情况下，某些队列可能未被映射。如果在队列未映射时敲响门铃，引擎固件可能会错过该请求。一些调度固件会在硬件被超额订阅时通过轮询 wptr 影子来绕过这一问题；另一些引擎可能支持来自未映射队列的门铃更新。如果这些方案都不可用，内核驱动会向每个 GPUVM 地址空间映射一页聚合门铃空间。之后 UMD 会照常更新门铃与 wptr，并额外写一次聚合门铃。)

Special Packets(特殊数据包)
---------------------------

In order to support legacy implicit synchronization, as well as mixed user and
kernel queues, we need a synchronization mechanism that is secure.  Because
kernel queues or memory management tasks depend on kernel fences, we need a way
for user queues to update memory that the kernel can use for a fence, that can't
be messed with by a bad actor.  To support this, we've added a protected fence
packet.  This packet works by writing a monotonically increasing value to
a memory location that only privileged clients have write access to. User
queues only have read access.  When this packet is executed, the memory location
is updated and other queues (kernel or user) can see the results.  The
user application would submit this packet in their command stream.  The actual
packet format varies from IP to IP (GFX/Compute, SDMA, VCN, etc.), but the
behavior is the same.  The packet submission is handled in userspace.  The
kernel driver sets up the privileged memory used for each user queue when it
sets the queues up when the application creates them.
(为了支持传统的隐式同步，以及用户队列与内核队列的混合使用，我们需要一种安全的同步机制。由于内核队列或内存管理任务依赖内核 fence，我们需要一种途径让用户队列更新内核可用作 fence 的内存，且不会被恶意行为者篡改。为此，我们添加了受保护 fence 数据包。该数据包的工作方式是：向一个只有特权客户端才有写权限的内存位置写入一个单调递增的值。用户队列只有读权限。当该数据包被执行时，该内存位置被更新，其他队列(内核或用户)可以看到结果。用户应用程序会在其命令流中提交该数据包。实际的数据包格式因 IP 而异(GFX/Compute、SDMA、VCN 等)，但行为相同。数据包的提交在用户空间处理。内核驱动在应用程序创建队列、驱动设置队列时，为每个用户队列设置好所用到的特权内存。)


Memory Management(内存管理)
===========================

It is assumed that all buffers mapped into the GPUVM space for the process are
valid when engines on the GPU are running.  The kernel driver will only allow
user queues to run when all buffers are mapped.  If there is a memory event that
requires buffer migration, the kernel driver will preempt the user queues,
migrate buffers to where they need to be, update the GPUVM page tables and
invaldidate the TLB, and then resume the user queues.
(假定当 GPU 上的引擎运行时，映射到该进程 GPUVM 空间的所有缓冲区都是有效的。内核驱动只会在所有缓冲区都已映射时才允许用户队列运行。如果发生需要缓冲区迁移的内存事件，内核驱动会抢占用户队列，把缓冲区迁移到所需位置，更新 GPUVM 页表并使 TLB 失效，然后恢复用户队列。)

Interaction with Kernel Queues(与内核队列的交互)
================================================

Depending on the IP and the scheduling firmware, you can enable kernel queues
and user queues at the same time, however, you are limited by the HQD slots.
Kernel queues are always mapped so any work that goes into kernel queues will
take priority.  This limits the available HQD slots for user queues.
(取决于 IP 与调度固件，可以同时启用内核队列与用户队列，但会受到 HQD 槽位的限制。内核队列总是被映射，因此进入内核队列的任何工作都会优先执行。这限制了用户队列可用的 HQD 槽位。)

Not all IPs will support user queues on all GPUs.  As such, UMDs will need to
support both user queues and kernel queues depending on the IP.  For example, a
GPU may support user queues for GFX, compute, and SDMA, but not for VCN, JPEG,
and VPE.  UMDs need to support both.  The kernel driver provides a way to
determine if user queues and kernel queues are supported on a per IP basis.
UMDs can query this information via the INFO IOCTL and determine whether to use
kernel queues or user queues for each IP.
(并非所有 IP 在所有 GPU 上都支持用户队列。因此，UMD 需要根据 IP 同时支持用户队列与内核队列。例如，某个 GPU 可能对 GFX、compute 和 SDMA 支持用户队列，但对 VCN、JPEG 和 VPE 不支持。UMD 需要两者都支持。内核驱动提供了一种按 IP 判断是否支持用户队列与内核队列的方法。UMD 可以通过 INFO IOCTL 查询这一信息，并据此决定每个 IP 使用内核队列还是用户队列。)

Queue Resets(队列复位)
======================

For most engines, queues can be reset individually.  GFX, compute, and SDMA
queues can be reset individually.  When a hung queue is detected, it can be
reset either via the scheduling firmware or MMIO.  Since there are no kernel
fences for most user queues, they will usually only be detected when some other
event happens; e.g., a memory event which requires migration of buffers.  When
the queues are preempted, if the queue is hung, the preemption will fail.
Driver will then look up the queues that failed to preempt and reset them and
record which queues are hung.
(对大多数引擎来说，队列可以单独复位。GFX、compute 与 SDMA 队列都可以单独复位。当检测到挂起的队列时，可以通过调度固件或 MMIO 来复位它。由于大多数用户队列没有内核 fence，通常只有在发生其他事件时才会检测到问题；例如需要迁移缓冲区的内存事件。当队列被抢占时，如果队列已挂起，抢占会失败。驱动随后会查找抢占失败的队列并复位它们，同时记录哪些队列已挂起。)

On the UMD side, we will add a USERQ QUERY_STATUS IOCTL to query the queue
status.  UMD will provide the queue id in the IOCTL and the kernel driver
will check if it has already recorded the queue as hung (e.g., due to failed
peemption) and report back the status.
(在 UMD 侧，我们将添加 USERQ QUERY_STATUS IOCTL 来查询队列状态。UMD 在 IOCTL 中提供队列 id，内核驱动会检查它是否已把该队列记录为挂起(例如因抢占失败)，并回报状态。)

IOCTL Interfaces(IOCTL 接口)
============================

GPU virtual addresses used for queues and related data (rptrs, wptrs, context
save areas, etc.) should be validated by the kernel mode driver to prevent the
user from specifying invalid GPU virtual addresses.  If the user provides
invalid GPU virtual addresses or doorbell indicies, the IOCTL should return an
error message.  These buffers should also be tracked in the kernel driver so
that if the user attempts to unmap the buffer(s) from the GPUVM, the umap call
would return an error.
(队列及相关数据(rptr、wptr、上下文保存区等)使用的 GPU 虚拟地址应由内核态驱动进行校验，以防止用户指定无效的 GPU 虚拟地址。如果用户提供了无效的 GPU 虚拟地址或门铃索引，IOCTL 应返回错误信息。这些缓冲区还应在内核驱动中加以跟踪，这样当用户试图把缓冲区从 GPUVM 取消映射时，unmap 调用会返回错误。)

INFO(原名)
----------
There are several new INFO queries related to user queues in order to query the
size of user queue meta data needed for a user queue (e.g., context save areas
or shadow buffers), whether kernel or user queues or both are supported
for each IP type, and the offsets for each IP type in each doorbell page.
(新增了若干与用户队列相关的 INFO 查询，用于查询用户队列所需的用户队列元数据大小(例如上下文保存区或影子缓冲区)、每种 IP 类型支持内核队列还是用户队列还是两者都支持，以及每种 IP 类型在每个门铃页中的偏移。)

USERQ(原名)
-----------
The USERQ IOCTL is used for creating, freeing, and querying the status of user
queues.  It supports 3 opcodes:
(USERQ IOCTL 用于创建、释放用户队列以及查询其状态。它支持 3 个操作码：)

1. CREATE - Create a user queue.  The application provides an MQD-like structure
   that defines the type of queue and associated metadata and flags for that
   queue type.  Returns the queue id.
   (CREATE - 创建用户队列。应用程序提供一个类似 MQD 的结构，定义队列类型以及该队列类型相关的元数据与标志。返回队列 id。)
2. FREE - Free a user queue.
   (FREE - 释放一个用户队列。)
3. QUERY_STATUS - Query that status of a queue.  Used to check if the queue is
   healthy or not.  E.g., if the queue has been reset. (WIP)
   (QUERY_STATUS - 查询队列的状态。用于检查队列是否健康，例如队列是否已被复位。(开发中))

USERQ_SIGNAL(原名)
------------------
The USERQ_SIGNAL IOCTL is used to provide a list of sync objects to be signaled.
(USERQ_SIGNAL IOCTL 用于提供要发信号的同步对象列表。)

USERQ_WAIT(原名)
----------------
The USERQ_WAIT IOCTL is used to provide a list of sync object to be waited on.
(USERQ_WAIT IOCTL 用于提供要等待的同步对象列表。)

Kernel and User Queues(内核队列与用户队列)
==========================================

In order to properly validate and test performance, we have a driver option to
select what type of queues are enabled (kernel queues, user queues or both).
The user_queue driver parameter allows you to enable kernel queues only (0),
user queues and kernel queues (1), and user queues only (2).  Enabling user
queues only will free up static queue assignments that would otherwise be used
by kernel queues for use by the scheduling firmware.  Some kernel queues are
required for kernel driver operation and they will always be created.  When the
kernel queues are not enabled, they are not registered with the drm scheduler
and the CS IOCTL will reject any incoming command submissions which target those
queue types.  Kernel queues only mirrors the behavior on all existing GPUs.
Enabling both queues allows for backwards compatibility with old userspace while
still supporting user queues.
(为了正确验证并测试性能，我们提供了一个驱动选项来选择启用哪种队列(内核队列、用户队列或两者)。user_queue 驱动参数允许只启用内核队列(0)、用户队列与内核队列(1)，或只启用用户队列(2)。只启用用户队列可以释放原本供内核队列使用的静态队列分配，供调度固件使用。内核驱动运行需要一些内核队列，它们始终会被创建。当内核队列未启用时，它们不会注册到 drm 调度器，CS IOCTL 将拒绝任何以这些队列类型为目标的命令提交。只启用内核队列则与所有现有 GPU 上的行为相同。同时启用两种队列可以在支持用户队列的同时，向后兼容旧的用户空间。)
