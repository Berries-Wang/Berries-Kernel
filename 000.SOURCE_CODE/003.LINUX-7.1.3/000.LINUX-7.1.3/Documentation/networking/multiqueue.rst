.. SPDX-License-Identifier: GPL-2.0

===========================================
HOWTO for multiqueue network device support
===========================================

(多队列网络设备支持 HOWTO)

Section 1: Base driver requirements for implementing multiqueue support
(第 1 节：实现多队列支持的底层驱动要求)
=======================================================================

Intro: Kernel support for multiqueue devices
(简介：针对多队列设备的内核支持)
---------------------------------------------------------

Kernel support for multiqueue devices is always present.
(对多队列设备的内核支持始终存在。)

Base drivers are required to use the new alloc_etherdev_mq() or
alloc_netdev_mq() functions to allocate the subqueues for the device.  The
underlying kernel API will take care of the allocation and deallocation of
the subqueue memory, as well as netdev configuration of where the queues
exist in memory.
(底层驱动需要使用新的 alloc_etherdev_mq() 或 alloc_netdev_mq() 函数来为设备分配子队列。底层内核 API 会负责子队列内存的分配与释放，以及队列在内存中位置的 netdev 配置。)

The base driver will also need to manage the queues as it does the global
netdev->queue_lock today.  Therefore base drivers should use the
netif_{start|stop|wake}_subqueue() functions to manage each queue while the
device is still operational.  netdev->queue_lock is still used when the device
comes online or when it's completely shut down (unregister_netdev(), etc.).
(底层驱动还需要像今天管理全局 netdev->queue_lock 那样来管理这些队列。因此，底层驱动应使用 netif_{start|stop|wake}_subqueue() 函数在设备仍在运行时管理每个队列。而在设备上线或完全关闭(unregister_netdev() 等)时，仍会使用 netdev->queue_lock。)


Section 2: Qdisc support for multiqueue devices
(第 2 节：针对多队列设备的排队规则支持)
===============================================

Currently two qdiscs are optimized for multiqueue devices.  The first is the
default pfifo_fast qdisc.  This qdisc supports one qdisc per hardware queue.
A new round-robin qdisc, sch_multiq also supports multiple hardware queues. The
qdisc is responsible for classifying the skb's and then directing the skb's to
bands and queues based on the value in skb->queue_mapping.  Use this field in
the base driver to determine which queue to send the skb to.
(目前有两种针对多队列设备优化的排队规则 (qdisc)。第一种是默认的 pfifo_fast qdisc，它支持每个硬件队列一个 qdisc。新的轮询 (round-robin) qdisc sch_multiq 也支持多个硬件队列。qdisc 负责对 skb 进行分类，然后根据 skb->queue_mapping 中的值将 skb 引导到相应的波段 (band) 和队列。底层驱动可以利用该字段决定把 skb 发送到哪个队列。)

sch_multiq has been added for hardware that wishes to avoid head-of-line
blocking.  It will cycle though the bands and verify that the hardware queue
associated with the band is not stopped prior to dequeuing a packet.
(sch_multiq 是为希望避免队头阻塞 (head-of-line blocking) 的硬件而新增的。它在出队数据包之前会轮询各波段，并验证与该波段关联的硬件队列未处于停止状态。)

On qdisc load, the number of bands is based on the number of queues on the
hardware.  Once the association is made, any skb with skb->queue_mapping set,
will be queued to the band associated with the hardware queue.
(在 qdisc 加载时，波段数量取决于硬件上的队列数量。一旦建立了关联，任何设置了 skb->queue_mapping 的 skb 都会被排入与该硬件队列关联的波段。)


Section 3: Brief howto using MULTIQ for multiqueue devices
(第 3 节：使用 MULTIQ 操作多队列设备的简要指南)
==========================================================

The userspace command 'tc,' part of the iproute2 package, is used to configure
qdiscs.  To add the MULTIQ qdisc to your network device, assuming the device
is called eth0, run the following command::
(用户空间命令 tc(iproute2 软件包的一部分)用于配置 qdisc。要把 MULTIQ qdisc 添加到你的网络设备上(假设设备名为 eth0)，运行以下命令：)

    # tc qdisc add dev eth0 root handle 1: multiq

The qdisc will allocate the number of bands to equal the number of queues that
the device reports, and bring the qdisc online.  Assuming eth0 has 4 Tx
queues, the band mapping would look like::
(该 qdisc 会将波段数量分配为与设备报告的队列数量相等，并使 qdisc 上线。假设 eth0 有 4 个 Tx 队列，波段映射如下所示：)

    band 0 => queue 0
    band 1 => queue 1
    band 2 => queue 2
    band 3 => queue 3

Traffic will begin flowing through each queue based on either the simple_tx_hash
function or based on netdev->select_queue() if you have it defined.
(流量将开始流经各个队列，依据是 simple_tx_hash 函数，或者在你定义了 netdev->select_queue() 的情况下依据它。)

The behavior of tc filters remains the same.  However a new tc action,
skbedit, has been added.  Assuming you wanted to route all traffic to a
specific host, for example 192.168.0.3, through a specific queue you could use
this action and establish a filter such as::
(tc 过滤器的行为保持不变。不过，新增了一个 tc action，即 skbedit。假设你想把发往特定主机(例如 192.168.0.3)的所有流量都经由特定队列发送，可以使用这个 action 并建立如下过滤器：)

    tc filter add dev eth0 parent 1: protocol ip prio 1 u32 \
	    match ip dst 192.168.0.3 \
	    action skbedit queue_mapping 3

:Author: Alexander Duyck <alexander.h.duyck@intel.com>
(作者: Alexander Duyck <alexander.h.duyck@intel.com>)
:Original Author: Peter P. Waskiewicz Jr. <peter.p.waskiewicz.jr@intel.com>
(原作者: Peter P. Waskiewicz Jr. <peter.p.waskiewicz.jr@intel.com>)
