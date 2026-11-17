.. SPDX-License-Identifier: GPL-2.0

===============================================================
Scaling in the Linux Networking Stack(Linux 网络协议栈中的扩展)
===============================================================


Introduction(引言)
==================

This document describes a set of complementary techniques in the Linux
networking stack to increase parallelism and improve performance for
multi-processor systems.
(本文描述 Linux 网络协议栈中一组相互补充的技术，用于提升多处理器系统的
并行性并改善性能。)

The following technologies are described:
(下文将介绍以下技术：)

- RSS: Receive Side Scaling
  (RSS：接收端扩展)
- RPS: Receive Packet Steering
  (RPS：接收数据包引导)
- RFS: Receive Flow Steering
  (RFS：接收流引导)
- Accelerated Receive Flow Steering
  (加速接收流引导)
- XPS: Transmit Packet Steering
  (XPS：发送数据包引导)


RSS: Receive Side Scaling(RSS：接收端扩展)
==========================================

Contemporary NICs support multiple receive and transmit descriptor queues
(multi-queue). On reception, a NIC can send different packets to different
queues to distribute processing among CPUs. The NIC distributes packets by
applying a filter to each packet that assigns it to one of a small number
of logical flows. Packets for each flow are steered to a separate receive
queue, which in turn can be processed by separate CPUs. This mechanism is
generally known as “Receive-side Scaling” (RSS). The goal of RSS and
the other scaling techniques is to increase performance uniformly.
Multi-queue distribution can also be used for traffic prioritization, but
that is not the focus of these techniques.
(现代网卡支持多个接收和发送描述符队列 (多队列)。在接收时，网卡可以把不同的
数据包发往不同的队列，从而在各 CPU 之间分配处理工作。网卡通过对接收到的每个
数据包应用一个过滤器来分发数据包，该过滤器将数据包归入少数几个逻辑流之一。
每个流的数据包被引导到单独的接收队列，而每个接收队列又可由不同的 CPU 处理。
这种机制通常被称为"接收端扩展" (RSS)。RSS 及其他扩展技术的目标是均匀地提升
性能。多队列分发也可用于流量优先级排序，但这并不是这些技术的关注点。)

The filter used in RSS is typically a hash function over the network
and/or transport layer headers-- for example, a 4-tuple hash over
IP addresses and TCP ports of a packet. The most common hardware
implementation of RSS uses an indirection table where each entry
stores a queue number. The receive queue for a packet is determined
by indexing the indirection table with the low order bits of the
computed hash for the packet (usually a Toeplitz hash).
(RSS 中使用的过滤器通常是对网络层和/或传输层报头计算的哈希函数——例如，
对数据包的 IP 地址和 TCP 端口计算的四元组哈希。最常见的 RSS 硬件实现使用
一个间接表 (indirection table)，其中每个表项存储一个队列编号。数据包的
接收队列由以下方式确定：用为该数据包计算出的哈希 (通常是 Toeplitz 哈希)
的低位比特对间接表进行索引。)

The indirection table helps even out the traffic distribution when queue
count is not a power of two. NICs should provide an indirection table
at least 4 times larger than the queue count. 4x table results in ~16%
imbalance between the queues, which is acceptable for most applications.
(当队列数不是 2 的幂时，间接表有助于使流量分布更加均衡。网卡提供的间接表
应至少为队列数的 4 倍大小。4 倍大小的表会使各队列之间产生约 16% 的不均衡，
这对大多数应用来说是可以接受的。)

Some NICs support symmetric RSS hashing where, if the IP (source address,
destination address) and TCP/UDP (source port, destination port) tuples
are swapped, the computed hash is the same. This is beneficial in some
applications that monitor TCP/IP flows (IDS, firewalls, ...etc) and need
both directions of the flow to land on the same Rx queue (and CPU). The
"Symmetric-XOR" and "Symmetric-OR-XOR" are types of RSS algorithms that
achieve this hash symmetry by XOR/ORing the input source and destination
fields of the IP and/or L4 protocols. This, however, results in reduced
input entropy and could potentially be exploited.
(一些网卡支持对称 RSS 哈希：如果把 IP (源地址, 目的地址) 和 TCP/UDP
(源端口, 目的端口) 二元组互换，计算出的哈希值不变。这对某些需要监控
TCP/IP 流的应用 (IDS、防火墙等) 很有用，它们需要流的两个方向都落在同一个
接收队列 (和 CPU) 上。"Symmetric-XOR" 与 "Symmetric-OR-XOR" 是两类能实现
这种哈希对称性的 RSS 算法，其做法是对 IP 和/或 L4 协议的输入源与目的字段
进行异或 (XOR) 或按位或 (OR) 运算。然而，这会降低输入熵，并可能被利用。)

Specifically, the "Symmetric-XOR" algorithm XORs the input
as follows::
(具体而言，"Symmetric-XOR" 算法按如下方式对输入做异或：)

    # (SRC_IP ^ DST_IP, SRC_IP ^ DST_IP, SRC_PORT ^ DST_PORT, SRC_PORT ^ DST_PORT)

The "Symmetric-OR-XOR" algorithm, on the other hand, transforms the input as
follows::
(而 "Symmetric-OR-XOR" 算法则按如下方式变换输入：)

    # (SRC_IP | DST_IP, SRC_IP ^ DST_IP, SRC_PORT | DST_PORT, SRC_PORT ^ DST_PORT)

The result is then fed to the underlying RSS algorithm.
(计算结果随后被送入底层 RSS 算法。)

Some advanced NICs allow steering packets to queues based on
programmable filters. For example, webserver bound TCP port 80 packets
can be directed to their own receive queue. Such “n-tuple” filters can
be configured from ethtool (--config-ntuple).
(一些高级网卡允许基于可编程过滤器把数据包引导到特定队列。例如，发往
Web 服务器 TCP 端口 80 的数据包可以被定向到自己的接收队列。这类 "n 元组"
(n-tuple) 过滤器可以通过 ethtool 配置 (--config-ntuple)。)


RSS Configuration(RSS 配置)
---------------------------

The driver for a multi-queue capable NIC typically provides a kernel
module parameter for specifying the number of hardware queues to
configure. In the bnx2x driver, for instance, this parameter is called
num_queues. A typical RSS configuration would be to have one receive queue
for each CPU if the device supports enough queues, or otherwise at least
one for each memory domain, where a memory domain is a set of CPUs that
share a particular memory level (L1, L2, NUMA node, etc.).
(支持多队列的网卡的驱动程序通常提供一个内核模块参数，用于指定要配置的硬件
队列数量。例如在 bnx2x 驱动中，该参数名为 num_queues。典型的 RSS 配置是：
如果设备支持足够多的队列，就为每个 CPU 配置一个接收队列；否则至少为每个
内存域配置一个，其中内存域是指共享某一特定内存层级 (L1、L2、NUMA 节点等)
的一组 CPU。)

The indirection table of an RSS device, which resolves a queue by masked
hash, is usually programmed by the driver at initialization. The
default mapping is to distribute the queues evenly in the table, but the
indirection table can be retrieved and modified at runtime using ethtool
commands (--show-rxfh-indir and --set-rxfh-indir). Modifying the
indirection table could be done to give different queues different
relative weights.
(RSS 设备的间接表通过掩码后的哈希值解析出队列，通常由驱动程序在初始化时
写入。默认映射是将各队列均匀分布在表中，但可以在运行时使用 ethtool 命令
(--show-rxfh-indir 和 --set-rxfh-indir) 读取和修改间接表。修改间接表可以
为不同队列赋予不同的相对权重。)


RSS IRQ Configuration(RSS 中断配置)
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

Each receive queue has a separate IRQ associated with it. The NIC triggers
this to notify a CPU when new packets arrive on the given queue. The
signaling path for PCIe devices uses message signaled interrupts (MSI-X),
that can route each interrupt to a particular CPU. The active mapping
of queues to IRQs can be determined from /proc/interrupts. By default,
an IRQ may be handled on any CPU. Because a non-negligible part of packet
processing takes place in receive interrupt handling, it is advantageous
to spread receive interrupts between CPUs. To manually adjust the IRQ
affinity of each interrupt see Documentation/core-api/irq/irq-affinity.rst. Some systems
will be running irqbalance, a daemon that dynamically optimizes IRQ
assignments and as a result may override any manual settings.
(每个接收队列都有一个与之关联的独立 IRQ。当给定队列上有新数据包到达时，
网卡会触发该 IRQ 通知某个 CPU。PCIe 设备的信号通路使用消息信号中断
(MSI-X)，可以把每个中断路由到特定的 CPU。队列到 IRQ 的实际映射可以从
/proc/interrupts 确定。默认情况下，IRQ 可以在任何 CPU 上处理。由于相当
一部分数据包处理工作发生在接收中断处理过程中，因此把接收中断分散到多个
CPU 上是有益的。要手动调整每个中断的 IRQ 亲和性，参见
Documentation/core-api/irq/irq-affinity.rst。一些系统会运行 irqbalance——
这是一个动态优化 IRQ 分配的守护进程，因而可能会覆盖任何手动设置。)


Suggested Configuration(推荐配置)
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

RSS should be enabled when latency is a concern or whenever receive
interrupt processing forms a bottleneck. Spreading load between CPUs
decreases queue length. For low latency networking, the optimal setting
is to allocate as many queues as there are CPUs in the system (or the
NIC maximum, if lower). The most efficient high-rate configuration
is likely the one with the smallest number of receive queues where no
receive queue overflows due to a saturated CPU, because in default
mode with interrupt coalescing enabled, the aggregate number of
interrupts (and thus work) grows with each additional queue.
(在关心延迟时，或每当接收中断处理成为瓶颈时，都应启用 RSS。在多个 CPU
之间分散负载可以缩短队列长度。对于低延迟网络，最优设置是分配与系统中
CPU 数量一样多的队列 (若网卡上限更低则取网卡上限)。对于高速率场景，
最高效的配置很可能是使用最少数量的接收队列、且没有任何接收队列因某个
CPU 饱和而溢出的配置，因为在默认模式下启用中断合并 (interrupt coalescing)
时，中断总数 (也就是工作量) 会随每增加一个队列而增长。)

Per-cpu load can be observed using the mpstat utility, but note that on
processors with hyperthreading (HT), each hyperthread is represented as
a separate CPU. For interrupt handling, HT has shown no benefit in
initial tests, so limit the number of queues to the number of CPU cores
in the system.
(可以使用 mpstat 工具观察每个 CPU 的负载，但注意在支持超线程 (HT) 的
处理器上，每个超线程都表现为一个独立的 CPU。对于中断处理而言，初步测试
显示 HT 没有带来收益，因此应将队列数量限制为系统中的 CPU 核心数。)

Dedicated RSS contexts(专用 RSS 上下文)
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

Modern NICs support creating multiple co-existing RSS configurations
which are selected based on explicit matching rules. This can be very
useful when application wants to constrain the set of queues receiving
traffic for e.g. a particular destination port or IP address.
The example below shows how to direct all traffic to TCP port 22
to queues 0 and 1.
(现代网卡支持创建多个可共存的 RSS 配置，并基于显式匹配规则进行选择。当
应用程序希望限制接收某类流量的队列集合时，例如针对特定目的端口或 IP 地址，
这会非常有用。下面的示例展示了如何把发往 TCP 端口 22 的所有流量引导到
队列 0 和 1。)

To create an additional RSS context use::
(要创建一个额外的 RSS 上下文，可使用：)

  # ethtool -X eth0 hfunc toeplitz context new
  New RSS context is 1

Kernel reports back the ID of the allocated context (the default, always
present RSS context has ID of 0). The new context can be queried and
modified using the same APIs as the default context::
(内核会返回所分配上下文的 ID (默认 RSS 上下文始终存在，其 ID 为 0)。
可以使用与默认上下文相同的 API 来查询和修改新上下文：)

  # ethtool -x eth0 context 1
  RX flow hash indirection table for eth0 with 13 RX ring(s):
    0:      0     1     2     3     4     5     6     7
    8:      8     9    10    11    12     0     1     2
  [...]
  # ethtool -X eth0 equal 2 context 1
  # ethtool -x eth0 context 1
  RX flow hash indirection table for eth0 with 13 RX ring(s):
    0:      0     1     0     1     0     1     0     1
    8:      0     1     0     1     0     1     0     1
  [...]

To make use of the new context direct traffic to it using an n-tuple
filter::
(要使用新上下文，需通过 n 元组过滤器把流量引导过去：)

  # ethtool -N eth0 flow-type tcp6 dst-port 22 context 1
  Added rule with ID 1023

When done, remove the context and the rule::
(完成后，删除该上下文和规则：)

  # ethtool -N eth0 delete 1023
  # ethtool -X eth0 context 1 delete


RPS: Receive Packet Steering(RPS：接收数据包引导)
=================================================

Receive Packet Steering (RPS) is logically a software implementation of
RSS. Being in software, it is necessarily called later in the datapath.
Whereas RSS selects the queue and hence CPU that will run the hardware
interrupt handler, RPS selects the CPU to perform protocol processing
above the interrupt handler. This is accomplished by placing the packet
on the desired CPU’s backlog queue and waking up the CPU for processing.
RPS has some advantages over RSS:
(接收数据包引导 (RPS) 在逻辑上是 RSS 的软件实现。由于它以软件方式实现，
在数据路径中必然被更晚调用。RSS 选择的是队列及其上运行硬件中断处理程序的
CPU，而 RPS 选择的则是在中断处理程序之上执行协议处理的 CPU。其实现方式是
把数据包放到目标 CPU 的积压队列 (backlog queue) 上，并唤醒该 CPU 进行
处理。RPS 相对 RSS 的优点有：)

1) it can be used with any NIC
   (可用于任何网卡)
2) software filters can easily be added to hash over new protocols
   (可以方便地添加软件过滤器，以对新协议进行哈希)
3) it does not increase hardware device interrupt rate (although it does
   introduce inter-processor interrupts (IPIs))
   (不会提高硬件设备的中断率 (尽管会引入处理器间中断 (IPI)))

RPS is called during bottom half of the receive interrupt handler, when
a driver sends a packet up the network stack with netif_rx() or
netif_receive_skb(). These call the get_rps_cpu() function, which
selects the queue that should process a packet.
(RPS 在接收中断处理程序的下半部 (bottom half) 中被调用，此时驱动程序通过
netif_rx() 或 netif_receive_skb() 将数据包送入网络协议栈。这两个函数会
调用 get_rps_cpu()，由它选择应当处理该数据包的队列。)

The first step in determining the target CPU for RPS is to calculate a
flow hash over the packet’s addresses or ports (2-tuple or 4-tuple hash
depending on the protocol). This serves as a consistent hash of the
associated flow of the packet. The hash is either provided by hardware
or will be computed in the stack. Capable hardware can pass the hash in
the receive descriptor for the packet; this would usually be the same
hash used for RSS (e.g. computed Toeplitz hash). The hash is saved in
skb->hash and can be used elsewhere in the stack as a hash of the
packet’s flow.
(确定 RPS 目标 CPU 的第一步是对数据包的地址或端口计算流哈希 (根据协议
不同，为二元组或四元组哈希)。它作为数据包所属流的一个一致性哈希。该哈希
既可以由硬件提供，也可以在协议栈中计算。具备能力的硬件可以在该数据包的
接收描述符中传递哈希；这通常与 RSS 使用的哈希相同 (例如计算得到的
Toeplitz 哈希)。哈希保存在 skb->hash 中，并可在协议栈其他地方用作该
数据包所属流的哈希。)

Each receive hardware queue has an associated list of CPUs to which
RPS may enqueue packets for processing. For each received packet,
an index into the list is computed from the flow hash modulo the size
of the list. The indexed CPU is the target for processing the packet,
and the packet is queued to the tail of that CPU’s backlog queue. At
the end of the bottom half routine, IPIs are sent to any CPUs for which
packets have been queued to their backlog queue. The IPI wakes backlog
processing on the remote CPU, and any queued packets are then processed
up the networking stack.
(每个接收硬件队列都有一个关联的 CPU 列表，RPS 可以把数据包加入这些 CPU
以待处理。对每个收到的数据包，用流哈希对该列表长度取模得到一个列表索引。
被索引到的 CPU 就是处理该数据包的目标，数据包被排到该 CPU 积压队列的
尾部。在下半部例程结束时，会向所有积压队列中已入队数据包的 CPU 发送
IPI。IPI 唤醒远端 CPU 上的积压处理，随后所有排队的数据包沿网络协议栈
向上处理。)


RPS Configuration(RPS 配置)
---------------------------

RPS requires a kernel compiled with the CONFIG_RPS kconfig symbol (on
by default for SMP). Even when compiled in, RPS remains disabled until
explicitly configured. The list of CPUs to which RPS may forward traffic
can be configured for each receive queue using a sysfs file entry::
(RPS 需要内核编译时带有 CONFIG_RPS kconfig 选项 (对 SMP 默认开启)。即使
编译进内核，RPS 在显式配置之前也保持禁用。RPS 可将流量转发到哪些 CPU，
这一列表可针对每个接收队列通过一个 sysfs 文件项进行配置：)

  /sys/class/net/<dev>/queues/rx-<n>/rps_cpus

This file implements a bitmap of CPUs. RPS is disabled when it is zero
(the default), in which case packets are processed on the interrupting
CPU. Documentation/core-api/irq/irq-affinity.rst explains how CPUs are assigned to
the bitmap.
(该文件实现了一个 CPU 位图。当它为零时 (默认值)，RPS 被禁用，此时数据包
在触发中断的 CPU 上处理。Documentation/core-api/irq/irq-affinity.rst 说明了
如何把 CPU 分配到位图中。)


Suggested Configuration(推荐配置)
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

For a single queue device, a typical RPS configuration would be to set
the rps_cpus to the CPUs in the same memory domain of the interrupting
CPU. If NUMA locality is not an issue, this could also be all CPUs in
the system. At high interrupt rate, it might be wise to exclude the
interrupting CPU from the map since that already performs much work.
(对于单队列设备，典型的 RPS 配置是把 rps_cpus 设置为与触发中断的 CPU 处于
同一内存域的 CPU。如果 NUMA 局部性不是问题，也可以设置为系统中的所有
CPU。在中断率很高时，最好把触发中断的 CPU 从映射中排除，因为它已经承担
了大量工作。)

For a multi-queue system, if RSS is configured so that a hardware
receive queue is mapped to each CPU, then RPS is probably redundant
and unnecessary. If there are fewer hardware queues than CPUs, then
RPS might be beneficial if the rps_cpus for each queue are the ones that
share the same memory domain as the interrupting CPU for that queue.
(对于多队列系统，如果 RSS 已配置为每个 CPU 映射一个硬件接收队列，那么
RPS 可能是多余的、没有必要的。如果硬件队列数量少于 CPU 数量，那么当每个
队列的 rps_cpus 是与该队列的中断 CPU 共享同一内存域的那些 CPU 时，RPS
可能会有帮助。)


RPS Flow Limit(RPS 流限制)
--------------------------

RPS scales kernel receive processing across CPUs without introducing
reordering. The trade-off to sending all packets from the same flow
to the same CPU is CPU load imbalance if flows vary in packet rate.
In the extreme case a single flow dominates traffic. Especially on
common server workloads with many concurrent connections, such
behavior indicates a problem such as a misconfiguration or spoofed
source Denial of Service attack.
(RPS 在不引入乱序的前提下，把内核接收处理扩展到多个 CPU 上。把同一流的
所有数据包都送到同一个 CPU 的代价是：如果各流的数据包速率不同，会造成
CPU 负载不均衡。在极端情况下，单个流会主导全部流量。尤其是在有许多并发
连接的常见服务器工作负载下，这种行为意味着存在问题，例如配置错误或伪造
源的拒绝服务攻击。)

Flow Limit is an optional RPS feature that prioritizes small flows
during CPU contention by dropping packets from large flows slightly
ahead of those from small flows. It is active only when an RPS or RFS
destination CPU approaches saturation.  Once a CPU's input packet
queue exceeds half the maximum queue length (as set by sysctl
net.core.netdev_max_backlog), the kernel starts a per-flow packet
count over the last 256 packets. If a flow exceeds a set ratio (by
default, half) of these packets when a new packet arrives, then the
new packet is dropped. Packets from other flows are still only
dropped once the input packet queue reaches netdev_max_backlog.
No packets are dropped when the input packet queue length is below
the threshold, so flow limit does not sever connections outright:
even large flows maintain connectivity.
(流限制 (Flow Limit) 是一个可选的 RPS 特性，它在 CPU 争用时优先照顾小流，
方法是让来自大流的数据包略早于来自小流的数据包被丢弃。只有当 RPS 或 RFS
的目标 CPU 接近饱和时它才会生效。一旦某个 CPU 的输入数据包队列超过最大
队列长度 (由 sysctl net.core.netdev_max_backlog 设定) 的一半，内核就开始
对最近 256 个数据包进行按流的计数。当新数据包到达时，如果某个流在这些
数据包中的占比超过设定比例 (默认为一半)，则丢弃该新数据包。来自其他流的
数据包仍然只有当输入数据包队列达到 netdev_max_backlog 时才会被丢弃。当
输入数据包队列长度低于阈值时不会丢弃任何数据包，因此流限制不会直接切断
连接：即使是大流也保持连通性。)


Interface(接口)
~~~~~~~~~~~~~~~

Flow limit is compiled in by default (CONFIG_NET_FLOW_LIMIT), but not
turned on. It is implemented for each CPU independently (to avoid lock
and cache contention) and toggled per CPU by setting the relevant bit
in sysctl net.core.flow_limit_cpu_bitmap. It exposes the same CPU
bitmap interface as rps_cpus (see above) when called from procfs::
(流限制默认编译进内核 (CONFIG_NET_FLOW_LIMIT)，但默认不开启。它为每个
CPU 独立实现 (以避免锁和缓存争用)，通过设置 sysctl
net.core.flow_limit_cpu_bitmap 中的相应位来按 CPU 开关。当从 procfs 调用
时，它暴露与 rps_cpus 相同的 CPU 位图接口 (见上文)：)

  /proc/sys/net/core/flow_limit_cpu_bitmap

Per-flow rate is calculated by hashing each packet into a hashtable
bucket and incrementing a per-bucket counter. The hash function is
the same that selects a CPU in RPS, but as the number of buckets can
be much larger than the number of CPUs, flow limit has finer-grained
identification of large flows and fewer false positives. The default
table has 4096 buckets. This value can be modified through sysctl::
(每个流的速率是这样计算的：把每个数据包哈希到哈希表的一个桶中，并递增
该桶的计数器。哈希函数与 RPS 中选择 CPU 的函数相同，但由于桶的数量可以
远大于 CPU 的数量，流限制能够更细粒度地识别大流，误报更少。默认表有
4096 个桶。该值可通过 sysctl 修改：)

  net.core.flow_limit_table_len

The value is only consulted when a new table is allocated. Modifying
it does not update active tables.
(该值只在新表分配时被读取。修改它不会更新已处于活动状态的表。)


Suggested Configuration(推荐配置)
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

Flow limit is useful on systems with many concurrent connections,
where a single connection taking up 50% of a CPU indicates a problem.
In such environments, enable the feature on all CPUs that handle
network rx interrupts (as set in /proc/irq/N/smp_affinity).
(流限制适用于有许多并发连接的系统，在这种系统中，单个连接占用某个 CPU
的 50% 即表明存在问题。在这类环境中，应在所有处理网络接收中断的 CPU 上
启用该特性 (即 /proc/irq/N/smp_affinity 中设置的 CPU)。)

The feature depends on the input packet queue length to exceed
the flow limit threshold (50%) + the flow history length (256).
Setting net.core.netdev_max_backlog to either 1000 or 10000
performed well in experiments.
(该特性要求输入数据包队列长度超过流限制阈值 (50%) 加上流历史长度
(256)。实验中，将 net.core.netdev_max_backlog 设置为 1000 或 10000 都
表现良好。)


RFS: Receive Flow Steering(RFS：接收流引导)
===========================================

While RPS steers packets solely based on hash, and thus generally
provides good load distribution, it does not take into account
application locality. This is accomplished by Receive Flow Steering
(RFS). The goal of RFS is to increase datacache hitrate by steering
kernel processing of packets to the CPU where the application thread
consuming the packet is running. RFS relies on the same RPS mechanisms
to enqueue packets onto the backlog of another CPU and to wake up that
CPU.
(RPS 仅基于哈希来引导数据包，因此通常能提供良好的负载分布，但它没有考虑
应用程序的局部性。这正是接收流引导 (RFS) 所要实现的。RFS 的目标是把对
数据包的内核处理引导到正在运行消费该数据包的应用线程的 CPU 上，以提高
数据缓存命中率。RFS 依赖与 RPS 相同的机制把数据包加入另一个 CPU 的积压
队列并唤醒该 CPU。)

In RFS, packets are not forwarded directly by the value of their hash,
but the hash is used as index into a flow lookup table. This table maps
flows to the CPUs where those flows are being processed. The flow hash
(see RPS section above) is used to calculate the index into this table.
The CPU recorded in each entry is the one which last processed the flow.
If an entry does not hold a valid CPU, then packets mapped to that entry
are steered using plain RPS. Multiple table entries may point to the
same CPU. Indeed, with many flows and few CPUs, it is very likely that
a single application thread handles flows with many different flow hashes.
(在 RFS 中，数据包不再直接按其哈希值转发，而是把哈希用作流查找表的索引。
该表把各个流映射到正在处理这些流的 CPU。流哈希 (见上文 RPS 一节) 用于
计算该表的索引。每个表项中记录的 CPU 是最近处理该流的 CPU。如果某表项
没有记录有效的 CPU，那么映射到该表项的数据包就按普通 RPS 引导。多个表项
可以指向同一个 CPU。实际上，当流很多而 CPU 很少时，单个应用线程很可能要
处理具有许多不同流哈希的流。)

rps_sock_flow_table is a global flow table that contains the *desired* CPU
for flows: the CPU that is currently processing the flow in userspace.
Each table value is a CPU index that is updated during calls to recvmsg
and sendmsg (specifically, inet_recvmsg(), inet_sendmsg() and
tcp_splice_read()).
(rps_sock_flow_table 是一个全局流表，其中包含各个流的目标 CPU (desired
CPU)：即当前在用户态处理该流的 CPU。每个表值是一个 CPU 索引，在调用
recvmsg 和 sendmsg 时更新 (具体为 inet_recvmsg()、inet_sendmsg() 和
tcp_splice_read())。)

When the scheduler moves a thread to a new CPU while it has outstanding
receive packets on the old CPU, packets may arrive out of order. To
avoid this, RFS uses a second flow table to track outstanding packets
for each flow: rps_dev_flow_table is a table specific to each hardware
receive queue of each device. Each table value stores a CPU index and a
counter. The CPU index represents the *current* CPU onto which packets
for this flow are enqueued for further kernel processing. Ideally, kernel
and userspace processing occur on the same CPU, and hence the CPU index
in both tables is identical. This is likely false if the scheduler has
recently migrated a userspace thread while the kernel still has packets
enqueued for kernel processing on the old CPU.
(当调度器把一个线程移到新 CPU，而该线程在旧 CPU 上还有未处理完的接收
数据包时，数据包可能乱序到达。为避免这种情况，RFS 使用第二个流表来跟踪
每个流的未处理数据包：rps_dev_flow_table 是针对每个设备每个硬件接收队列
的表。每个表值存储一个 CPU 索引和一个计数器。CPU 索引表示当前接收该流
数据包入队以供进一步内核处理的 CPU (current CPU)。理想情况下，内核处理
与用户态处理发生在同一个 CPU 上，因此两个表中的 CPU 索引相同。如果调度器
刚刚迁移了用户态线程，而内核仍在旧 CPU 上留有为内核处理而入队的数据包，
则这一点很可能不成立。)

The counter in rps_dev_flow_table values records the length of the current
CPU's backlog when a packet in this flow was last enqueued. Each backlog
queue has a head counter that is incremented on dequeue. A tail counter
is computed as head counter + queue length. In other words, the counter
in rps_dev_flow[i] records the last element in flow i that has
been enqueued onto the currently designated CPU for flow i (of course,
entry i is actually selected by hash and multiple flows may hash to the
same entry i).
(rps_dev_flow_table 表值中的计数器记录了该流最近一次数据包入队时当前 CPU
积压队列的长度。每个积压队列都有一个出队时递增的队首计数器 (head
counter)，队尾计数器 (tail counter) 则由队首计数器加队列长度计算得出。
换言之，rps_dev_flow[i] 中的计数器记录了流 i 中已被入队到流 i 当前指定
CPU 的最后一个元素 (当然，表项 i 实际上由哈希选出，多个流可能哈希到同一
个表项 i)。)

And now the trick for avoiding out of order packets: when selecting the
CPU for packet processing (from get_rps_cpu()) the rps_sock_flow table
and the rps_dev_flow table of the queue that the packet was received on
are compared. If the desired CPU for the flow (found in the
rps_sock_flow table) matches the current CPU (found in the rps_dev_flow
table), the packet is enqueued onto that CPU’s backlog. If they differ,
the current CPU is updated to match the desired CPU if one of the
following is true:
(下面就是避免数据包乱序的技巧：在为数据包处理选择 CPU 时 (由
get_rps_cpu() 完成)，会比较 rps_sock_flow 表与接收该数据包的队列的
rps_dev_flow 表。如果该流的目标 CPU (在 rps_sock_flow 表中查得) 与当前
CPU (在 rps_dev_flow 表中查得) 一致，数据包就被加入该 CPU 的积压队列。
如果二者不同，则在满足下列条件之一时，把当前 CPU 更新为目标 CPU：)

  - The current CPU's queue head counter >= the recorded tail counter
    value in rps_dev_flow[i]
    (当前 CPU 的队列队首计数器 >= rps_dev_flow[i] 中记录的队尾计数器值)
  - The current CPU is unset (>= nr_cpu_ids)
    (当前 CPU 未设置 (>= nr_cpu_ids))
  - The current CPU is offline
    (当前 CPU 处于离线状态)

After this check, the packet is sent to the (possibly updated) current
CPU. These rules aim to ensure that a flow only moves to a new CPU when
there are no packets outstanding on the old CPU, as the outstanding
packets could arrive later than those about to be processed on the new
CPU.
(经过这一检查后，数据包被送往 (可能已更新的) 当前 CPU。这些规则旨在确保
只有当旧 CPU 上没有未处理完的数据包时，流才迁移到新 CPU，因为那些未处理
完的数据包可能比即将在新 CPU 上处理的数据包更晚到达。)


RFS Configuration(RFS 配置)
---------------------------

RFS is only available if the kconfig symbol CONFIG_RPS is enabled (on
by default for SMP). The functionality remains disabled until explicitly
configured. The number of entries in the global flow table is set through::
(只有当 kconfig 选项 CONFIG_RPS 开启时 (对 SMP 默认开启) RFS 才可用。
该功能在显式配置之前保持禁用。全局流表的表项数通过以下途径设置：)

  /proc/sys/net/core/rps_sock_flow_entries

The number of entries in the per-queue flow table are set through::
(每队列流表的表项数通过以下途径设置：)

  /sys/class/net/<dev>/queues/rx-<n>/rps_flow_cnt


Suggested Configuration(推荐配置)
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

Both of these need to be set before RFS is enabled for a receive queue.
Values for both are rounded up to the nearest power of two. The
suggested flow count depends on the expected number of active connections
at any given time, which may be significantly less than the number of open
connections. We have found that a value of 65536 for rps_sock_flow_entries
works fairly well on a moderately loaded server. Big servers might
need 1048576 or even higher values.
(这两项都需要在为某个接收队列启用 RFS 之前设置。二者的取值都会向上取整
到最接近的 2 的幂。建议的流计数取决于任意时刻预计的活动连接数，该数值
可能远小于打开的连接数。我们发现，在负载中等的服务器上，
rps_sock_flow_entries 取 65536 效果相当好。大型服务器可能需要 1048576
甚至更大的值。)

On a NUMA host it is advisable to spread rps_sock_flow_entries on all nodes.
(在 NUMA 主机上，建议将 rps_sock_flow_entries 分散到所有节点上。)

numactl --interleave=all bash -c "echo 1048576 >/proc/sys/net/core/rps_sock_flow_entries"

For a single queue device, the rps_flow_cnt value for the single queue
would normally be configured to the same value as rps_sock_flow_entries.
For a multi-queue device, the rps_flow_cnt for each queue might be
configured as rps_sock_flow_entries / N, where N is the number of
queues. So for instance, if rps_sock_flow_entries is set to 131072 and there
are 16 configured receive queues, rps_flow_cnt for each queue might be
configured as 8192.
(对于单队列设备，该队列的 rps_flow_cnt 通常配置为与 rps_sock_flow_entries
相同的值。对于多队列设备，每个队列的 rps_flow_cnt 可以配置为
rps_sock_flow_entries / N，其中 N 为队列数。例如，若 rps_sock_flow_entries
设为 131072 且配置了 16 个接收队列，则每个队列的 rps_flow_cnt 可配置为
8192。)


Accelerated RFS(加速 RFS)
=========================

Accelerated RFS is to RFS what RSS is to RPS: a hardware-accelerated load
balancing mechanism that uses soft state to steer flows based on where
the application thread consuming the packets of each flow is running.
Accelerated RFS should perform better than RFS since packets are sent
directly to a CPU local to the thread consuming the data. The target CPU
will either be the same CPU where the application runs, or at least a CPU
which is local to the application thread’s CPU in the cache hierarchy.
(加速 RFS 之于 RFS，正如 RSS 之于 RPS：它是一种硬件加速的负载均衡机制，
利用软状态 (soft state) 根据消费每个流数据包的应用线程运行在何处来引导
各个流。加速 RFS 的性能应优于 RFS，因为数据包被直接发送到消费该数据的
线程本地的 CPU。目标 CPU 要么是应用程序运行的同一个 CPU，要么至少是在
缓存层次结构中与应用线程 CPU 相邻的 CPU。)

To enable accelerated RFS, the networking stack calls the
ndo_rx_flow_steer driver function to communicate the desired hardware
queue for packets matching a particular flow. The network stack
automatically calls this function every time a flow entry in
rps_dev_flow_table is updated. The driver in turn uses a device specific
method to program the NIC to steer the packets.
(为了启用加速 RFS，网络协议栈会调用驱动程序的 ndo_rx_flow_steer 函数，
告知与特定流匹配的数据包应使用的目标硬件队列。每当 rps_dev_flow_table
中的流表项被更新时，网络协议栈都会自动调用该函数。驱动程序随后使用设备
特定的方法对网卡编程以引导数据包。)

The hardware queue for a flow is derived from the CPU recorded in
rps_dev_flow_table. The stack consults a CPU to hardware queue map which
is maintained by the NIC driver. This is an auto-generated reverse map of
the IRQ affinity table shown by /proc/interrupts. Drivers can use
functions in the cpu_rmap (“CPU affinity reverse map”) kernel library
to populate the map. Alternatively, drivers can delegate the cpu_rmap
management to the Kernel by calling netif_enable_cpu_rmap(). For each CPU,
the corresponding queue in the map is set to be one whose processing CPU is
closest in cache locality.
(每个流的硬件队列由 rps_dev_flow_table 中记录的 CPU 推导得出。协议栈会
查询一个由网卡驱动维护的 CPU 到硬件队列的映射。这是 /proc/interrupts 所
显示的 IRQ 亲和性表的一个自动生成的反向映射。驱动程序可以使用内核库
cpu_rmap ("CPU 亲和性反向映射") 中的函数来填充该映射。另外，驱动程序也
可以通过调用 netif_enable_cpu_rmap() 把 cpu_rmap 的管理委托给内核。映射
中为每个 CPU 设置的对应队列，是处理 CPU 在缓存局部性上与该 CPU 最接近的
队列。)


Accelerated RFS Configuration(加速 RFS 配置)
--------------------------------------------

Accelerated RFS is only available if the kernel is compiled with
CONFIG_RFS_ACCEL and support is provided by the NIC device and driver.
It also requires that ntuple filtering is enabled via ethtool. The map
of CPU to queues is automatically deduced from the IRQ affinities
configured for each receive queue by the driver, so no additional
configuration should be necessary.
(只有当内核编译时启用 CONFIG_RFS_ACCEL，且网卡设备和驱动程序提供支持时，
加速 RFS 才可用。它还要求通过 ethtool 启用 ntuple 过滤。CPU 到队列的映射
会由驱动程序根据为每个接收队列配置的 IRQ 亲和性自动推导，因此通常不需要
额外的配置。)


Suggested Configuration(推荐配置)
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

This technique should be enabled whenever one wants to use RFS and the
NIC supports hardware acceleration.
(只要想使用 RFS 且网卡支持硬件加速，就应启用该技术。)


XPS: Transmit Packet Steering(XPS：发送数据包引导)
==================================================

Transmit Packet Steering is a mechanism for intelligently selecting
which transmit queue to use when transmitting a packet on a multi-queue
device. This can be accomplished by recording two kinds of maps, either
a mapping of CPU to hardware queue(s) or a mapping of receive queue(s)
to hardware transmit queue(s).
(发送数据包引导 (Transmit Packet Steering) 是一种在多队列设备上发送数据
包时智能选择所用发送队列的机制。这可以通过记录两类映射来实现：CPU 到
硬件队列的映射，或接收队列到硬件发送队列的映射。)

1. XPS using CPUs map
   (1. 使用 CPU 映射的 XPS)

The goal of this mapping is usually to assign queues
exclusively to a subset of CPUs, where the transmit completions for
these queues are processed on a CPU within this set. This choice
provides two benefits. First, contention on the device queue lock is
significantly reduced since fewer CPUs contend for the same queue
(contention can be eliminated completely if each CPU has its own
transmit queue). Secondly, cache miss rate on transmit completion is
reduced, in particular for data cache lines that hold the sk_buff
structures.
(这种映射的目的通常是独占地把队列分配给一个 CPU 子集，使这些队列的发送
完成处理也在该集合内的某个 CPU 上进行。这一选择带来两个好处。第一，设备
队列锁上的争用显著减少，因为争用同一队列的 CPU 更少 (如果每个 CPU 拥有
自己的发送队列，争用可以完全消除)。第二，发送完成时的缓存未命中率降低，
尤其是对于存放 sk_buff 结构的数据缓存行。)

2. XPS using receive queues map
   (2. 使用接收队列映射的 XPS)

This mapping is used to pick transmit queue based on the receive
queue(s) map configuration set by the administrator. A set of receive
queues can be mapped to a set of transmit queues (many:many), although
the common use case is a 1:1 mapping. This will enable sending packets
on the same queue associations for transmit and receive. This is useful for
busy polling multi-threaded workloads where there are challenges in
associating a given CPU to a given application thread. The application
threads are not pinned to CPUs and each thread handles packets
received on a single queue. The receive queue number is cached in the
socket for the connection. In this model, sending the packets on the same
transmit queue corresponding to the associated receive queue has benefits
in keeping the CPU overhead low. Transmit completion work is locked into
the same queue-association that a given application is polling on. This
avoids the overhead of triggering an interrupt on another CPU. When the
application cleans up the packets during the busy poll, transmit completion
may be processed along with it in the same thread context and so result in
reduced latency.
(这种映射用于依据管理员设置的接收队列映射配置来挑选发送队列。一组接收
队列可以映射到一组发送队列 (多对多)，但常见用例是 1:1 映射。这使得收发
数据包可以使用相同的队列关联。这对忙轮询的多线程工作负载很有用，因为在
这类场景中把给定 CPU 与给定应用线程关联起来比较困难。应用线程不固定在
特定 CPU 上，且每个线程只处理在单个队列上收到的数据包。接收队列号会被
缓存在该连接的套接字中。在这种模型下，在与关联接收队列相对应的同一发送
队列上发送数据包，有助于保持较低的 CPU 开销。发送完成处理被锁定在该
应用程序正在轮询的同一队列关联上，从而避免在另一个 CPU 上触发中断的
开销。当应用程序在忙轮询期间清理数据包时，发送完成处理可以在同一线程
上下文中一并完成，从而降低延迟。)

XPS is configured per transmit queue by setting a bitmap of
CPUs/receive-queues that may use that queue to transmit. The reverse
mapping, from CPUs to transmit queues or from receive-queues to transmit
queues, is computed and maintained for each network device. When
transmitting the first packet in a flow, the function get_xps_queue() is
called to select a queue. This function uses the ID of the receive queue
for the socket connection for a match in the receive queue-to-transmit queue
lookup table. Alternatively, this function can also use the ID of the
running CPU as a key into the CPU-to-queue lookup table. If the
ID matches a single queue, that is used for transmission. If multiple
queues match, one is selected by using the flow hash to compute an index
into the set. When selecting the transmit queue based on receive queue(s)
map, the transmit device is not validated against the receive device as it
requires expensive lookup operation in the datapath.
(XPS 按发送队列进行配置，方法是设置一个位图，标明哪些 CPU/接收队列可以
使用该队列发送。反向映射 (从 CPU 到发送队列，或从接收队列到发送队列)
会为每个网络设备计算并维护。当发送某个流的第一个数据包时，会调用
get_xps_queue() 函数来选择队列。该函数使用套接字连接的接收队列 ID，在
接收队列到发送队列的查找表中查找匹配。此外，该函数也可以用当前运行 CPU
的 ID 作为键，在 CPU 到队列的查找表中查找。如果 ID 只匹配一个队列，就
使用该队列发送。如果匹配多个队列，则用流哈希在该集合中计算一个索引来
选出其中一个。在基于接收队列映射选择发送队列时，不会对照接收设备校验
发送设备，因为那需要在数据路径中进行代价高昂的查找操作。)

The queue chosen for transmitting a particular flow is saved in the
corresponding socket structure for the flow (e.g. a TCP connection).
This transmit queue is used for subsequent packets sent on the flow to
prevent out of order (ooo) packets. The choice also amortizes the cost
of calling get_xps_queues() over all packets in the flow. To avoid
ooo packets, the queue for a flow can subsequently only be changed if
skb->ooo_okay is set for a packet in the flow. This flag indicates that
there are no outstanding packets in the flow, so the transmit queue can
change without the risk of generating out of order packets. The
transport layer is responsible for setting ooo_okay appropriately. TCP,
for instance, sets the flag when all data for a connection has been
acknowledged.
(为某个特定流选定的发送队列保存在该流对应的套接字结构中 (例如一条 TCP
连接)。该流后续发送的数据包都使用这一发送队列，以避免乱序 (ooo) 数据包。
这一选择也把调用 get_xps_queues() 的成本分摊到流中的所有数据包上。为
避免乱序数据包，此后只有当流中某个数据包设置了 skb->ooo_okay 时，才能
更改该流的队列。该标志表明流中没有未处理完的数据包，因此更改发送队列
不会有产生乱序数据包的风险。传输层负责恰当地设置 ooo_okay。例如，TCP
会在一条连接的所有数据都被确认后设置该标志。)

XPS Configuration(XPS 配置)
---------------------------

XPS is only available if the kconfig symbol CONFIG_XPS is enabled (on by
default for SMP). If compiled in, it is driver dependent whether, and
how, XPS is configured at device init. The mapping of CPUs/receive-queues
to transmit queue can be inspected and configured using sysfs:
(只有当 kconfig 选项 CONFIG_XPS 开启时 (对 SMP 默认开启) XPS 才可用。
如果编译进内核，XPS 在设备初始化时是否配置以及如何配置取决于驱动程序。
CPU/接收队列到发送队列的映射可以使用 sysfs 查看和配置：)

For selection based on CPUs map::
(对于基于 CPU 映射的选择：)

  /sys/class/net/<dev>/queues/tx-<n>/xps_cpus

For selection based on receive-queues map::
(对于基于接收队列映射的选择：)

  /sys/class/net/<dev>/queues/tx-<n>/xps_rxqs


Suggested Configuration(推荐配置)
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

For a network device with a single transmission queue, XPS configuration
has no effect, since there is no choice in this case. In a multi-queue
system, XPS is preferably configured so that each CPU maps onto one queue.
If there are as many queues as there are CPUs in the system, then each
queue can also map onto one CPU, resulting in exclusive pairings that
experience no contention. If there are fewer queues than CPUs, then the
best CPUs to share a given queue are probably those that share the cache
with the CPU that processes transmit completions for that queue
(transmit interrupts).
(对于只有一个发送队列的网络设备，XPS 配置不起作用，因为这种情况下没有
选择余地。在多队列系统中，XPS 最好配置为每个 CPU 映射到一个队列。如果
队列数与系统中的 CPU 数一样多，那么每个队列也可以映射到一个 CPU，形成
没有争用的独占配对。如果队列数少于 CPU 数，那么共享给定队列的最佳 CPU，
很可能是与处理该队列发送完成 (发送中断) 的 CPU 共享缓存的那些 CPU。)

For transmit queue selection based on receive queue(s), XPS has to be
explicitly configured mapping receive-queue(s) to transmit queue(s). If the
user configuration for receive-queue map does not apply, then the transmit
queue is selected based on the CPUs map.
(对于基于接收队列的发送队列选择，必须显式配置 XPS 的接收队列到发送队列
映射。如果用户对接收队列映射的配置不适用，则基于 CPU 映射选择发送队列。)


Per TX Queue rate limitation(每发送队列速率限制)
================================================

These are rate-limitation mechanisms implemented by HW, where currently
a max-rate attribute is supported, by setting a Mbps value to::
(这些是由硬件实现的速率限制机制，目前支持一个 max-rate 属性，通过向
以下文件写入一个 Mbps 值来设置：)

  /sys/class/net/<dev>/queues/tx-<n>/tx_maxrate

A value of zero means disabled, and this is the default.
(值为零表示禁用，这也是默认值。)


Further Information(更多信息)
=============================

RPS and RFS were introduced in kernel 2.6.35. XPS was incorporated into
2.6.38. Original patches were submitted by Tom Herbert
(therbert@google.com)
(RPS 和 RFS 在内核 2.6.35 中引入。XPS 在 2.6.38 中并入。最初的补丁由
Tom Herbert (therbert@google.com) 提交。)

Accelerated RFS was introduced in 2.6.35. Original patches were
submitted by Ben Hutchings (bwh@kernel.org)
(加速 RFS 在 2.6.35 中引入。最初的补丁由 Ben Hutchings (bwh@kernel.org)
提交。)

Authors:
(作者：)

- Tom Herbert (therbert@google.com)
- Willem de Bruijn (willemb@google.com)
