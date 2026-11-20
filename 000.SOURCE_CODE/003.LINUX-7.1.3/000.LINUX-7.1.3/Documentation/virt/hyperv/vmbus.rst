.. SPDX-License-Identifier: GPL-2.0

VMBus(VMBus 总线)
=================
VMBus is a software construct provided by Hyper-V to guest VMs.  It
consists of a control path and common facilities used by synthetic
devices that Hyper-V presents to guest VMs.   The control path is
used to offer synthetic devices to the guest VM and, in some cases,
to rescind those devices.   The common facilities include software
channels for communicating between the device driver in the guest VM
and the synthetic device implementation that is part of Hyper-V, and
signaling primitives to allow Hyper-V and the guest to interrupt
each other.
(VMBus 是 Hyper-V 提供给客户虚拟机的软件构造。它由一个控制路径和
Hyper-V 呈现给客户虚拟机的合成设备所共用的公共设施组成。控制路径用于向
客户虚拟机提供合成设备，以及在某些情况下撤销这些设备。公共设施包括用于
在客户虚拟机中的设备驱动与作为 Hyper-V 一部分的合成设备实现之间通信的
软件通道，以及允许 Hyper-V 与客户互相中断对方的信号原语。)

VMBus is modeled in Linux as a bus, with the expected /sys/bus/vmbus
entry in a running Linux guest.  The VMBus driver (drivers/hv/vmbus_drv.c)
establishes the VMBus control path with the Hyper-V host, then
registers itself as a Linux bus driver.  It implements the standard
bus functions for adding and removing devices to/from the bus.
(VMBus 在 Linux 中被建模为一条总线，在运行中的 Linux 客户机里有预期的
/sys/bus/vmbus 条目。VMBus 驱动 (drivers/hv/vmbus_drv.c) 与 Hyper-V 主
机建立 VMBus 控制路径，然后把自己注册为 Linux 总线驱动。它实现标准的总
线函数，用于向总线添加设备以及从总线移除设备。)

Most synthetic devices offered by Hyper-V have a corresponding Linux
device driver.  These devices include:
(Hyper-V 提供的大多数合成设备都有对应的 Linux 设备驱动。这些设备包
括：)

* SCSI controller
  (- SCSI 控制器)
* NIC
  (- 网卡 (NIC))
* Graphics frame buffer
  (- 图形帧缓冲)
* Keyboard
  (- 键盘)
* Mouse
  (- 鼠标)
* PCI device pass-thru
  (- PCI 设备直通)
* Heartbeat
  (- 心跳)
* Time Sync
  (- 时间同步)
* Shutdown
  (- 关机)
* Memory balloon
  (- 内存气球)
* Key/Value Pair (KVP) exchange with Hyper-V
  (- 与 Hyper-V 的键/值对 (KVP) 交换)
* Hyper-V online backup (a.k.a. VSS)
  (- Hyper-V 在线备份 (又称 VSS))

Guest VMs may have multiple instances of the synthetic SCSI
controller, synthetic NIC, and PCI pass-thru devices.  Other
synthetic devices are limited to a single instance per VM.  Not
listed above are a small number of synthetic devices offered by
Hyper-V that are used only by Windows guests and for which Linux
does not have a driver.
(客户虚拟机可能拥有合成 SCSI 控制器、合成网卡和 PCI 直通设备的多个实
例。其他合成设备在每个虚拟机中限于单个实例。上面未列出的是 Hyper-V 提
供的少数仅由 Windows 客户机使用、Linux 没有驱动的合成设备。)

Hyper-V uses the terms "VSP" and "VSC" in describing synthetic
devices.  "VSP" refers to the Hyper-V code that implements a
particular synthetic device, while "VSC" refers to the driver for
the device in the guest VM.  For example, the Linux driver for the
synthetic NIC is referred to as "netvsc" and the Linux driver for
the synthetic SCSI controller is "storvsc".  These drivers contain
functions with names like "storvsc_connect_to_vsp".
(Hyper-V 在描述合成设备时使用 "VSP" 和 "VSC" 这两个术语。"VSP" 指实现
特定合成设备的 Hyper-V 代码，而 "VSC" 指客户虚拟机中该设备的驱动。例
如，合成网卡的 Linux 驱动被称为 "netvsc"，合成 SCSI 控制器的 Linux 驱
动是 "storvsc"。这些驱动包含诸如 "storvsc_connect_to_vsp" 之类名字的函
数。)

VMBus channels(VMBus 通道)
--------------------------
An instance of a synthetic device uses VMBus channels to communicate
between the VSP and the VSC.  Channels are bi-directional and used
for passing messages.   Most synthetic devices use a single channel,
but the synthetic SCSI controller and synthetic NIC may use multiple
channels to achieve higher performance and greater parallelism.
(合成设备的一个实例使用 VMBus 通道在 VSP 与 VSC 之间通信。通道是双向
的，用于传递消息。大多数合成设备使用单个通道，但合成 SCSI 控制器和合
成网卡可能使用多个通道以获得更高的性能和更大的并行度。)

Each channel consists of two ring buffers.  These are classic ring
buffers from a university data structures textbook.  If the read
and writes pointers are equal, the ring buffer is considered to be
empty, so a full ring buffer always has at least one byte unused.
The "in" ring buffer is for messages from the Hyper-V host to the
guest, and the "out" ring buffer is for messages from the guest to
the Hyper-V host.  In Linux, the "in" and "out" designations are as
viewed by the guest side.  The ring buffers are memory that is
shared between the guest and the host, and they follow the standard
paradigm where the memory is allocated by the guest, with the list
of GPAs that make up the ring buffer communicated to the host.  Each
ring buffer consists of a header page (4 Kbytes) with the read and
write indices and some control flags, followed by the memory for the
actual ring.  The size of the ring is determined by the VSC in the
guest and is specific to each synthetic device.   The list of GPAs
making up the ring is communicated to the Hyper-V host over the
VMBus control path as a GPA Descriptor List (GPADL).  See function
vmbus_establish_gpadl().
(每个通道由两个环形缓冲区组成。它们是大学数据结构教科书中的经典环形缓
冲区。如果读指针和写指针相等，则环形缓冲区被视为空，因此满的环形缓冲区
总是至少有一个字节未使用。"in" 环形缓冲区用于从 Hyper-V 主机到客户机的
消息，"out" 环形缓冲区用于从客户机到 Hyper-V 主机的消息。在 Linux 中，
"in" 和 "out" 的称呼以客户机一侧的视角为准。环形缓冲区是客户机与主机
之间共享的内存，它们遵循标准范式：内存由客户机分配，并把组成环形缓冲区
的 GPA 列表告知主机。每个环形缓冲区由一个头部页 (4 KB) 组成，其中包含
读写索引和一些控制标志，随后是实际环形的内存。环的大小由客户机中的 VSC
决定，并因合成设备而异。组成环的 GPA 列表经由 VMBus 控制路径以 GPA 描
述符列表 (GPADL) 的形式告知 Hyper-V 主机。参见函数
vmbus_establish_gpadl()。)

Each ring buffer is mapped into contiguous Linux kernel virtual
space in three parts:  1) the 4 Kbyte header page, 2) the memory
that makes up the ring itself, and 3) a second mapping of the memory
that makes up the ring itself.  Because (2) and (3) are contiguous
in kernel virtual space, the code that copies data to and from the
ring buffer need not be concerned with ring buffer wrap-around.
Once a copy operation has completed, the read or write index may
need to be reset to point back into the first mapping, but the
actual data copy does not need to be broken into two parts.  This
approach also allows complex data structures to be easily accessed
directly in the ring without handling wrap-around.
(每个环形缓冲区被映射到连续的 Linux 内核虚拟空间中的三个部分：1) 4 KB
的头部页，2) 组成环本身的内存，3) 组成环本身的内存的第二个映射。由于
(2) 和 (3) 在内核虚拟空间中连续，向环形缓冲区拷入和拷出数据的代码无需
关心环形缓冲区的回绕。拷贝操作完成后，读或写索引可能需要重置以指回第一
个映射，但实际的数据拷贝不需要分成两部分。这一方法还允许在环中直接轻松
访问复杂数据结构，而无需处理回绕。)

On arm64 with page sizes > 4 Kbytes, the header page must still be
passed to Hyper-V as a 4 Kbyte area.  But the memory for the actual
ring must be aligned to PAGE_SIZE and have a size that is a multiple
of PAGE_SIZE so that the duplicate mapping trick can be done.  Hence
a portion of the header page is unused and not communicated to
Hyper-V.  This case is handled by vmbus_establish_gpadl().
(在页大小大于 4 KB 的 arm64 上，头部页仍必须作为 4 KB 区域传递给
Hyper-V。但实际环的内存必须按 PAGE_SIZE 对齐，且大小必须是 PAGE_SIZE
的倍数，以便完成重复映射的技巧。因此头部页的一部分未被使用，也不会告知
Hyper-V。这种情况由 vmbus_establish_gpadl() 处理。)

Hyper-V enforces a limit on the aggregate amount of guest memory
that can be shared with the host via GPADLs.  This limit ensures
that a rogue guest can't force the consumption of excessive host
resources.  For Windows Server 2019 and later, this limit is
approximately 1280 Mbytes.  For versions prior to Windows Server
2019, the limit is approximately 384 Mbytes.
(Hyper-V 对可以经由 GPADL 与主机共享的客户机内存总量强制实施一个限
制。该限制确保恶意的客户机无法强迫消耗过多的主机资源。对于 Windows
Server 2019 及更高版本，该限制约为 1280 MB。对于早于 Windows Server
2019 的版本，该限制约为 384 MB。)

VMBus channel messages(VMBus 通道消息)
--------------------------------------
All messages sent in a VMBus channel have a standard header that includes
the message length, the offset of the message payload, some flags, and a
transactionID.  The portion of the message after the header is
unique to each VSP/VSC pair.
(在 VMBus 通道中发送的所有消息都有一个标准头部，包含消息长度、消息有
效载荷的偏移、一些标志以及 transactionID。头部之后的消息部分因 VSP/VSC
对而异。)

Messages follow one of two patterns:
(消息遵循以下两种模式之一：)

* Unidirectional:  Either side sends a message and does not
  expect a response message
  (- 单向：一方发送消息，不期待响应消息)
* Request/response:  One side (usually the guest) sends a message
  and expects a response
  (- 请求/响应：一方 (通常是客户机) 发送消息并期待响应)

The transactionID (a.k.a. "requestID") is for matching requests &
responses.  Some synthetic devices allow multiple requests to be in-
flight simultaneously, so the guest specifies a transactionID when
sending a request.  Hyper-V sends back the same transactionID in the
matching response.
(transactionID (又称 "requestID") 用于匹配请求与响应。一些合成设备允
许多个请求同时在途 (in-flight)，因此客户机在发送请求时指定一个
transactionID。Hyper-V 在匹配的响应中返回相同的 transactionID。)

Messages passed between the VSP and VSC are control messages.  For
example, a message sent from the storvsc driver might be "execute
this SCSI command".   If a message also implies some data transfer
between the guest and the Hyper-V host, the actual data to be
transferred may be embedded with the control message, or it may be
specified as a separate data buffer that the Hyper-V host will
access as a DMA operation.  The former case is used when the size of
the data is small and the cost of copying the data to and from the
ring buffer is minimal.  For example, time sync messages from the
Hyper-V host to the guest contain the actual time value.  When the
data is larger, a separate data buffer is used.  In this case, the
control message contains a list of GPAs that describe the data
buffer.  For example, the storvsc driver uses this approach to
specify the data buffers to/from which disk I/O is done.
(在 VSP 与 VSC 之间传递的消息是控制消息。例如，从 storvsc 驱动发出的
一条消息可能是 "执行这条 SCSI 命令"。如果一条消息还意味着客户机与
Hyper-V 主机之间的一些数据传输，则要传输的实际数据可以嵌入控制消息
中，也可以指定为一个单独的数据缓冲区，由 Hyper-V 主机作为 DMA 操作访
问。前者用于数据较小、把数据拷入拷出环形缓冲区的代价最小的情况。例
如，从 Hyper-V 主机发给客户机的时间同步消息包含实际的时间值。当数据较
大时，使用单独的数据缓冲区。在这种情况下，控制消息包含描述该数据缓冲区
的 GPA 列表。例如，storvsc 驱动使用这种方法指定进行磁盘 I/O 的数据缓
冲区。)

Three functions exist to send VMBus channel messages:
(存在三个用于发送 VMBus 通道消息的函数：)

1. vmbus_sendpacket():  Control-only messages and messages with
   embedded data -- no GPAs
   (1. vmbus_sendpacket()：纯控制消息以及带内嵌数据的消息——没有 GPA)
2. vmbus_sendpacket_pagebuffer(): Message with list of GPAs
   identifying data to transfer.  An offset and length is
   associated with each GPA so that multiple discontinuous areas
   of guest memory can be targeted.
   (2. vmbus_sendpacket_pagebuffer()：带 GPA 列表的消息，标识要传输的
   数据。每个 GPA 都关联一个偏移和长度，从而可以针对客户机内存中多个
   不连续的区域。)
3. vmbus_sendpacket_mpb_desc(): Message with list of GPAs
   identifying data to transfer.  A single offset and length is
   associated with a list of GPAs.  The GPAs must describe a
   single logical area of guest memory to be targeted.
   (3. vmbus_sendpacket_mpb_desc()：带 GPA 列表的消息，标识要传输的
   数据。单个偏移和长度关联于一组 GPA。这些 GPA 必须描述要针对的客户
   机内存中的单一逻辑区域。)

Historically, Linux guests have trusted Hyper-V to send well-formed
and valid messages, and Linux drivers for synthetic devices did not
fully validate messages.  With the introduction of processor
technologies that fully encrypt guest memory and that allow the
guest to not trust the hypervisor (AMD SEV-SNP, Intel TDX), trusting
the Hyper-V host is no longer a valid assumption.  The drivers for
VMBus synthetic devices are being updated to fully validate any
values read from memory that is shared with Hyper-V, which includes
messages from VMBus devices.  To facilitate such validation,
messages read by the guest from the "in" ring buffer are copied to a
temporary buffer that is not shared with Hyper-V.  Validation is
performed in this temporary buffer without the risk of Hyper-V
maliciously modifying the message after it is validated but before
it is used.
(从历史上看，Linux 客户机一直信任 Hyper-V 发送格式良好且有效的消息，
合成设备的 Linux 驱动也没有完全校验消息。随着完全加密客户机内存、并允
许客户机不信任 hypervisor 的处理器技术 (AMD SEV-SNP、Intel TDX) 的出
现，信任 Hyper-V 主机不再是有效的假设。VMBus 合成设备的驱动正在被更新
为完全校验从与 Hyper-V 共享的内存中读取的任何值，其中包括来自 VMBus 设
备的消息。为了便于这种校验，客户机从 "in" 环形缓冲区读取的消息会被复制
到一个不与 Hyper-V 共享的临时缓冲区中。校验在这个临时缓冲区中进行，避
免了 Hyper-V 在消息被校验之后、使用之前恶意修改消息的风险。)

Synthetic Interrupt Controller (synic)(合成中断控制器，synic)
-------------------------------------------------------------
Hyper-V provides each guest CPU with a synthetic interrupt controller
that is used by VMBus for host-guest communication. While each synic
defines 16 synthetic interrupts (SINT), Linux uses only one of the 16
(VMBUS_MESSAGE_SINT). All interrupts related to communication between
the Hyper-V host and a guest CPU use that SINT.
(Hyper-V 为每个客户机 CPU 提供一个合成中断控制器，VMBus 用它进行主机
与客户机之间的通信。虽然每个 synic 定义了 16 个合成中断 (SINT)，但
Linux 只使用其中的一个 (VMBUS_MESSAGE_SINT)。与 Hyper-V 主机和客户机
CPU 之间的通信相关的所有中断都使用该 SINT。)

The SINT is mapped to a single per-CPU architectural interrupt (i.e,
an 8-bit x86/x64 interrupt vector, or an arm64 PPI INTID). Because
each CPU in the guest has a synic and may receive VMBus interrupts,
they are best modeled in Linux as per-CPU interrupts. This model works
well on arm64 where a single per-CPU Linux IRQ is allocated for
VMBUS_MESSAGE_SINT. This IRQ appears in /proc/interrupts as an IRQ labelled
"Hyper-V VMbus". Since x86/x64 lacks support for per-CPU IRQs, an x86
interrupt vector is statically allocated (HYPERVISOR_CALLBACK_VECTOR)
across all CPUs and explicitly coded to call vmbus_isr(). In this case,
there's no Linux IRQ, and the interrupts are visible in aggregate in
/proc/interrupts on the "HYP" line.
(SINT 被映射到单个每 CPU 架构中断 (即 8 位 x86/x64 中断向量，或 arm64
的 PPI INTID)。由于客户机中的每个 CPU 都有 synic 并且可能接收 VMBus
中断，它们在 Linux 中最好建模为每 CPU 中断。这一模型在 arm64 上工作良
好，那里为 VMBUS_MESSAGE_SINT 分配了单个每 CPU 的 Linux IRQ。该 IRQ
在 /proc/interrupts 中显示为一个标为 "Hyper-V VMbus" 的 IRQ。由于
x86/x64 不支持每 CPU IRQ，一个 x86 中断向量 (HYPERVISOR_CALLBACK_VECTOR)
被静态分配给所有 CPU，并被显式编码为调用 vmbus_isr()。在这种情况下，
没有 Linux IRQ，这些中断在 /proc/interrupts 中以 "HYP" 行的聚合形式可
见。)

The synic provides the means to demultiplex the architectural interrupt into
one or more logical interrupts and route the logical interrupt to the proper
VMBus handler in Linux. This demultiplexing is done by vmbus_isr() and
related functions that access synic data structures.
(synic 提供了把架构中断分离 (demultiplex) 为一个或多个逻辑中断、并把
逻辑中断路由到 Linux 中正确的 VMBus 处理程序的手段。这种分离由
vmbus_isr() 及相关函数完成，它们访问 synic 数据结构。)

The synic is not modeled in Linux as an irq chip or irq domain,
and the demultiplexed logical interrupts are not Linux IRQs. As such,
they don't appear in /proc/interrupts or /proc/irq. The CPU
affinity for one of these logical interrupts is controlled via an
entry under /sys/bus/vmbus as described below.
(synic 在 Linux 中没有被建模为 irq chip 或 irq domain，分离出的逻辑中
断也不是 Linux IRQ。因此，它们不出现在 /proc/interrupts 或 /proc/irq
中。这些逻辑中断之一的 CPU 亲和性经由 /sys/bus/vmbus 下的一个条目控
制，如下文所述。)

VMBus interrupts(VMBus 中断)
----------------------------
VMBus provides a mechanism for the guest to interrupt the host when
the guest has queued new messages in a ring buffer.  The host
expects that the guest will send an interrupt only when an "out"
ring buffer transitions from empty to non-empty.  If the guest sends
interrupts at other times, the host deems such interrupts to be
unnecessary.  If a guest sends an excessive number of unnecessary
interrupts, the host may throttle that guest by suspending its
execution for a few seconds to prevent a denial-of-service attack.
(VMBus 提供了一种机制，让客户机在环形缓冲区中排入新消息时中断主机。主
机预期客户机只在 "out" 环形缓冲区从空变为非空时才发送中断。如果客户机
在其他时间发送中断，主机认为这些中断是不必要的。如果客户机发送过多不必
要的中断，主机可能通过暂停其执行几秒钟来对该客户机进行限流，以防止拒绝
服务攻击。)

Similarly, the host will interrupt the guest via the synic when
it sends a new message on the VMBus control path, or when a VMBus
channel "in" ring buffer transitions from empty to non-empty due to
the host inserting a new VMBus channel message. The control message stream
and each VMBus channel "in" ring buffer are separate logical interrupts
that are demultiplexed by vmbus_isr(). It demultiplexes by first checking
for channel interrupts by calling vmbus_chan_sched(), which looks at a synic
bitmap to determine which channels have pending interrupts on this CPU.
If multiple channels have pending interrupts for this CPU, they are
processed sequentially.  When all channel interrupts have been processed,
vmbus_isr() checks for and processes any messages received on the VMBus
control path.
(类似地，当主机在 VMBus 控制路径上发送新消息，或由于主机插入新的
VMBus 通道消息而使某个 VMBus 通道的 "in" 环形缓冲区从空变为非空时，主
机会经由 synic 中断客户机。控制消息流和每个 VMBus 通道的 "in" 环形缓
冲区是独立的逻辑中断，由 vmbus_isr() 分离。它首先调用 vmbus_chan_sched()
检查通道中断，该函数查看 synic 位图以确定哪些通道在此 CPU 上有待处理中
断。如果多个通道在此 CPU 上有待处理中断，它们会被顺序处理。当所有通道
中断都处理完毕后，vmbus_isr() 检查并处理在 VMBus 控制路径上收到的任何
消息。)

The guest CPU that a VMBus channel will interrupt is selected by the
guest when the channel is created, and the host is informed of that
selection.  VMBus devices are broadly grouped into two categories:
(VMBus 通道将中断哪个客户机 CPU 由客户机在创建通道时选择，并告知主
机。VMBus 设备大体分为两类：)

1. "Slow" devices that need only one VMBus channel.  The devices
   (such as keyboard, mouse, heartbeat, and timesync) generate
   relatively few interrupts.  Their VMBus channels are all
   assigned to interrupt the VMBUS_CONNECT_CPU, which is always
   CPU 0.
   (1. "慢速"设备，只需要一个 VMBus 通道。这些设备 (如键盘、鼠标、心
   跳和时间同步) 产生的中断相对较少。它们的 VMBus 通道都被分配为中断
   VMBUS_CONNECT_CPU，后者始终是 CPU 0。)

2. "High speed" devices that may use multiple VMBus channels for
   higher parallelism and performance.  These devices include the
   synthetic SCSI controller and synthetic NIC.  Their VMBus
   channels interrupts are assigned to CPUs that are spread out
   among the available CPUs in the VM so that interrupts on
   multiple channels can be processed in parallel.
   (2. "高速"设备，可能使用多个 VMBus 通道以获得更高的并行度和性能。
   这些设备包括合成 SCSI 控制器和合成网卡。它们的 VMBus 通道中断被分
   配给虚拟机中可用 CPU 里分散开的一些 CPU，以便多个通道上的中断可以
   被并行处理。)

The assignment of VMBus channel interrupts to CPUs is done in the
function init_vp_index().  This assignment is done outside of the
normal Linux interrupt affinity mechanism, so the interrupts are
neither "unmanaged" nor "managed" interrupts.
(VMBus 通道中断到 CPU 的分配在函数 init_vp_index() 中完成。这一分配
在正常的 Linux 中断亲和性机制之外进行，因此这些中断既不是 "unmanaged"
也不是 "managed" 中断。)

The CPU that a VMBus channel will interrupt can be seen in
/sys/bus/vmbus/devices/<deviceGUID>/ channels/<channelRelID>/cpu.
When running on later versions of Hyper-V, the CPU can be changed
by writing a new value to this sysfs entry. Because VMBus channel
interrupts are not Linux IRQs, there are no entries in /proc/interrupts
or /proc/irq corresponding to individual VMBus channel interrupts.
(VMBus 通道将中断的 CPU 可以在
/sys/bus/vmbus/devices/<deviceGUID>/ channels/<channelRelID>/cpu 中
看到。在较新版本的 Hyper-V 上运行时，可以通过向该 sysfs 条目写入新值
来改变 CPU。由于 VMBus 通道中断不是 Linux IRQ，/proc/interrupts 或
/proc/irq 中没有与单个 VMBus 通道中断对应的条目。)

An online CPU in a Linux guest may not be taken offline if it has
VMBus channel interrupts assigned to it. Starting in kernel v6.15,
any such interrupts are automatically reassigned to some other CPU
at the time of offlining. The "other" CPU is chosen by the
implementation and is not load balanced or otherwise intelligently
determined. If the CPU is onlined again, channel interrupts previously
assigned to it are not moved back. As a result, after multiple CPUs
have been offlined, and perhaps onlined again, the interrupt-to-CPU
mapping may be scrambled and non-optimal. In such a case, optimal
assignments must be re-established manually. For kernels v6.14 and
earlier, any conflicting channel interrupts must first be manually
reassigned to another CPU as described above. Then when no channel
interrupts are assigned to the CPU, it can be taken offline.
(Linux 客户机中处于在线状态的 CPU 如果被分配了 VMBus 通道中断，则可
能无法被下线。从内核 v6.15 开始，任何此类中断在下线时会被自动重新分配
给某个其他 CPU。"其他" CPU 由实现选择，不做负载均衡或其他智能决策。
如果该 CPU 再次上线，先前分配给它的通道中断不会被移回。因此，在多个
CPU 被下线、或许又重新上线之后，中断到 CPU 的映射可能变得混乱且非最
优。在这种情况下，必须手动重新建立最优分配。对于 v6.14 及更早的内核，
必须先按上述方式把任何冲突的通道中断手动重新分配给另一个 CPU。然后当
没有通道中断分配给该 CPU 时，才能将其下线。)

The VMBus channel interrupt handling code is designed to work
correctly even if an interrupt is received on a CPU other than the
CPU assigned to the channel.  Specifically, the code does not use
CPU-based exclusion for correctness.  In normal operation, Hyper-V
will interrupt the assigned CPU.  But when the CPU assigned to a
channel is being changed via sysfs, the guest doesn't know exactly
when Hyper-V will make the transition.  The code must work correctly
even if there is a time lag before Hyper-V starts interrupting the
new CPU.  See comments in target_cpu_store().
(VMBus 通道中断处理代码被设计为即使中断在非分配 CPU 上收到也能正确
工作。具体来说，代码不依赖基于 CPU 的互斥来保证正确性。正常情况下，
Hyper-V 会中断所分配的 CPU。但当分配给某通道的 CPU 正在经由 sysfs 更
改时，客户机无法确切知道 Hyper-V 何时完成切换。即使 Hyper-V 开始中断
新 CPU 之前存在时间滞后，代码也必须正确工作。参见 target_cpu_store()
中的注释。)

VMBus device creation/deletion(VMBus 设备的创建与删除)
------------------------------------------------------
Hyper-V and the Linux guest have a separate message-passing path
that is used for synthetic device creation and deletion. This
path does not use a VMBus channel.  See vmbus_post_msg() and
vmbus_on_msg_dpc().
(Hyper-V 与 Linux 客户机之间有一条单独的消息传递路径，用于合成设备
的创建与删除。该路径不使用 VMBus 通道。参见 vmbus_post_msg() 和
vmbus_on_msg_dpc()。)

The first step is for the guest to connect to the generic
Hyper-V VMBus mechanism.  As part of establishing this connection,
the guest and Hyper-V agree on a VMBus protocol version they will
use.  This negotiation allows newer Linux kernels to run on older
Hyper-V versions, and vice versa.
(第一步是客户机连接到通用的 Hyper-V VMBus 机制。作为建立这一连接的
一部分，客户机与 Hyper-V 就将要使用的 VMBus 协议版本达成一致。这种协
商使较新的 Linux 内核可以运行在较旧的 Hyper-V 版本上，反之亦然。)

The guest then tells Hyper-V to "send offers".  Hyper-V sends an
offer message to the guest for each synthetic device that the VM
is configured to have. Each VMBus device type has a fixed GUID
known as the "class ID", and each VMBus device instance is also
identified by a GUID. The offer message from Hyper-V contains
both GUIDs to uniquely (within the VM) identify the device.
There is one offer message for each device instance, so a VM with
two synthetic NICs will get two offers messages with the NIC
class ID. The ordering of offer messages can vary from boot-to-boot
and must not be assumed to be consistent in Linux code. Offer
messages may also arrive long after Linux has initially booted
because Hyper-V supports adding devices, such as synthetic NICs,
to running VMs. A new offer message is processed by
vmbus_process_offer(), which indirectly invokes vmbus_add_channel_work().
(客户机然后告诉 Hyper-V "发送 offer"。Hyper-V 会为虚拟机被配置拥有
的每个合成设备向客户机发送一条 offer 消息。每种 VMBus 设备类型都有
一个固定的 GUID，称为 "class ID"，每个 VMBus 设备实例也由一个 GUID
标识。来自 Hyper-V 的 offer 消息同时包含这两个 GUID，以在虚拟机内唯
一地标识设备。每个设备实例有一条 offer 消息，因此拥有两个合成网卡的
虚拟机会收到两条带网卡 class ID 的 offer 消息。offer 消息的顺序在每
次启动之间可能不同，Linux 代码不得假定其一致。由于 Hyper-V 支持向运
行中的虚拟机添加设备 (例如合成网卡)，offer 消息也可能在 Linux 最初
启动很久之后到达。新的 offer 消息由 vmbus_process_offer() 处理，它
间接调用 vmbus_add_channel_work()。)

Upon receipt of an offer message, the guest identifies the device
type based on the class ID, and invokes the correct driver to set up
the device.  Driver/device matching is performed using the standard
Linux mechanism.
(收到 offer 消息后，客户机根据 class ID 识别设备类型，并调用正确的
驱动来设置该设备。驱动与设备的匹配使用标准的 Linux 机制完成。)

The device driver probe function opens the primary VMBus channel to
the corresponding VSP. It allocates guest memory for the channel
ring buffers and shares the ring buffer with the Hyper-V host by
giving the host a list of GPAs for the ring buffer memory.  See
vmbus_establish_gpadl().
(设备驱动的 probe 函数打开通往相应 VSP 的主 VMBus 通道。它为通道环
形缓冲区分配客户机内存，并通过把环形缓冲区内存的 GPA 列表交给主机来
与 Hyper-V 主机共享该环形缓冲区。参见 vmbus_establish_gpadl()。)

Once the ring buffer is set up, the device driver and VSP exchange
setup messages via the primary channel.  These messages may include
negotiating the device protocol version to be used between the Linux
VSC and the VSP on the Hyper-V host.  The setup messages may also
include creating additional VMBus channels, which are somewhat
mis-named as "sub-channels" since they are functionally
equivalent to the primary channel once they are created.
(环形缓冲区设置好之后，设备驱动与 VSP 经由主通道交换设置消息。这些
消息可能包括协商 Linux VSC 与 Hyper-V 主机上的 VSP 之间要使用的设备
协议版本。设置消息还可能包括创建额外的 VMBus 通道，这些通道被有些名
不副实地称为 "sub-channel" (子通道)，因为它们一旦创建就与主通道在功
能上等价。)

Finally, the device driver may create entries in /dev as with
any device driver.
(最后，设备驱动可以像任何设备驱动那样在 /dev 中创建条目。)

The Hyper-V host can send a "rescind" message to the guest to
remove a device that was previously offered. Linux drivers must
handle such a rescind message at any time. Rescinding a device
invokes the device driver "remove" function to cleanly shut
down the device and remove it. Once a synthetic device is
rescinded, neither Hyper-V nor Linux retains any state about
its previous existence. Such a device might be re-added later,
in which case it is treated as an entirely new device. See
vmbus_onoffer_rescind().
(Hyper-V 主机可以向客户机发送 "rescind" (撤销) 消息，以移除先前提
供的设备。Linux 驱动必须能够在任何时刻处理这样的撤销消息。撤销设备
会调用设备驱动的 "remove" 函数，干净地关闭并移除设备。一旦合成设备
被撤销，Hyper-V 和 Linux 都不再保留关于其先前存在的任何状态。这样
的设备之后可能被重新添加，届时它将被视为一个全新的设备。参见
vmbus_onoffer_rescind()。)

For some devices, such as the KVP device, Hyper-V automatically
sends a rescind message when the primary channel is closed,
likely as a result of unbinding the device from its driver.
The rescind causes Linux to remove the device. But then Hyper-V
immediately reoffers the device to the guest, causing a new
instance of the device to be created in Linux. For other
devices, such as the synthetic SCSI and NIC devices, closing the
primary channel does *not* result in Hyper-V sending a rescind
message. The device continues to exist in Linux on the VMBus,
but with no driver bound to it. The same driver or a new driver
can subsequently be bound to the existing instance of the device.
(对于某些设备 (例如 KVP 设备)，当主通道被关闭时 (很可能是设备从其
驱动解绑的结果)，Hyper-V 会自动发送一条撤销消息。该撤销导致 Linux
移除设备。但随后 Hyper-V 立即把设备重新 offer 给客户机，导致 Linux
中创建该设备的一个新实例。对于其他设备 (例如合成 SCSI 和网卡设备)，
关闭主通道 *不会* 导致 Hyper-V 发送撤销消息。设备继续以无驱动绑定
的状态存在于 Linux 的 VMBus 上。同一个驱动或新的驱动随后可以绑定到
该设备的既有实例上。)
