.. SPDX-License-Identifier: (GPL-2.0-only OR BSD-2-Clause)

====================================
Marvell OcteonTx2 RVU Kernel Drivers
====================================

(Marvell OcteonTx2 RVU 内核驱动)

Copyright (c) 2020 Marvell International Ltd.

Contents
(目录)
========

- `Overview`_
- `Drivers`_
- `Basic packet flow`_
- `Devlink health reporters`_
- `Quality of service`_
- `RVU representors`_

Overview
(概述)
========

Resource virtualization unit (RVU) on Marvell's OcteonTX2 SOC maps HW
resources from the network, crypto and other functional blocks into
PCI-compatible physical and virtual functions. Each functional block
again has multiple local functions (LFs) for provisioning to PCI devices.
RVU supports multiple PCIe SRIOV physical functions (PFs) and virtual
functions (VFs). PF0 is called the administrative / admin function (AF)
and has privileges to provision RVU functional block's LFs to each of the
PF/VF.
(Marvell OcteonTX2 SOC 上的资源虚拟化单元 (RVU) 将来自网络、加密及其他功能块的
硬件资源映射为 PCI 兼容的物理功能与虚拟功能。每个功能块又拥有多个本地功能
(LF)，用于分配给 PCI 设备。RVU 支持多个 PCIe SRIOV 物理功能 (PF) 和虚拟功能
(VF)。PF0 被称为管理功能 (AF)，拥有将 RVU 功能块的 LF 分配给各 PF/VF 的特权。)

RVU managed networking functional blocks
(RVU 管理的网络功能块)

 - Network pool or buffer allocator (NPA)
   (网络池或缓冲区分配器 (NPA))
 - Network interface controller (NIX)
   (网络接口控制器 (NIX))
 - Network parser CAM (NPC)
   (网络解析器 CAM (NPC))
 - Schedule/Synchronize/Order unit (SSO)
   (调度/同步/排序单元 (SSO))
 - Loopback interface (LBK)
   (环回接口 (LBK))

RVU managed non-networking functional blocks
(RVU 管理的非网络功能块)

 - Crypto accelerator (CPT)
   (加密加速器 (CPT))
 - Scheduled timers unit (TIM)
   (定时器调度单元 (TIM))
 - Schedule/Synchronize/Order unit (SSO)
   (调度/同步/排序单元 (SSO))
   Used for both networking and non networking usecases
   (同时用于网络与非网络场景。)

Resource provisioning examples
(资源分配示例)

 - A PF/VF with NIX-LF & NPA-LF resources works as a pure network device
   (拥有 NIX-LF 与 NPA-LF 资源的 PF/VF 可作为纯网络设备工作。)
 - A PF/VF with CPT-LF resource works as a pure crypto offload device.
   (拥有 CPT-LF 资源的 PF/VF 可作为纯加密卸载设备工作。)

RVU functional blocks are highly configurable as per software requirements.
(RVU 功能块可根据软件需求进行高度灵活的配置。)

Firmware setups following stuff before kernel boots
(固件在内核启动前完成以下设置)

 - Enables required number of RVU PFs based on number of physical links.
   (根据物理链路数量启用所需数量的 RVU PF。)
 - Number of VFs per PF are either static or configurable at compile time.
   Based on config, firmware assigns VFs to each of the PFs.
   (每个 PF 的 VF 数量既可以是静态的，也可以在编译时配置。固件根据该配置为每个
   PF 分配 VF。)
 - Also assigns MSIX vectors to each of PF and VFs.
   (还为每个 PF 和 VF 分配 MSIX 向量。)
 - These are not changed after kernel boot.
   (这些设置在内核启动后不会再改变。)

Drivers
(驱动)
=======

Linux kernel will have multiple drivers registering to different PF and VFs
of RVU. Wrt networking there will be 3 flavours of drivers.
(Linux 内核会有多个驱动注册到 RVU 的不同 PF 和 VF 上。就网络而言，将有 3 种类型
的驱动。)

Admin Function driver
(管理功能驱动)
---------------------

As mentioned above RVU PF0 is called the admin function (AF), this driver
supports resource provisioning and configuration of functional blocks.
Doesn't handle any I/O. It sets up few basic stuff but most of the
functionality is achieved via configuration requests from PFs and VFs.
(如上所述，RVU PF0 被称为管理功能 (AF)，该驱动负责功能块的资源分配与配置。
它本身不处理任何 I/O，只完成少量基础设置，大部分功能通过来自 PF 和 VF 的
配置请求来实现。)

PF/VFs communicates with AF via a shared memory region (mailbox). Upon
receiving requests AF does resource provisioning and other HW configuration.
AF is always attached to host kernel, but PFs and their VFs may be used by host
kernel itself, or attached to VMs or to userspace applications like
DPDK etc. So AF has to handle provisioning/configuration requests sent
by any device from any domain.
(PF/VF 通过一块共享内存区域 (mailbox，邮箱) 与 AF 通信。AF 在收到请求后进行
资源分配及其他硬件配置。AF 始终挂载在主机内核上，而 PF 及其 VF 可能由主机内核
自身使用，也可能挂载到虚拟机或 DPDK 等用户态应用上。因此 AF 必须能够处理来自
任何域上任何设备的分配/配置请求。)

AF driver also interacts with underlying firmware to
(AF 驱动还与底层固件交互，以便：)

 - Manage physical ethernet links ie CGX LMACs.
   (管理物理以太网链路，即 CGX LMAC。)
 - Retrieve information like speed, duplex, autoneg etc
   (获取速率、双工、自协商等信息。)
 - Retrieve PHY EEPROM and stats.
   (读取 PHY EEPROM 与统计信息。)
 - Configure FEC, PAM modes
   (配置 FEC、PAM 模式。)
 - etc
   (等等。)

From pure networking side AF driver supports following functionality.
(从纯网络角度来看，AF 驱动支持以下功能。)

 - Map a physical link to a RVU PF to which a netdev is registered.
   (将物理链路映射到注册了 netdev 的 RVU PF。)
 - Attach NIX and NPA block LFs to RVU PF/VF which provide buffer pools, RQs, SQs
   for regular networking functionality.
   (将 NIX 与 NPA 块的 LF 附着到 RVU PF/VF，为常规网络功能提供缓冲区池、RQ、
   SQ。)
 - Flow control (pause frames) enable/disable/config.
   (流控 (pause 帧) 的启用/禁用/配置。)
 - HW PTP timestamping related config.
   (硬件 PTP 时间戳相关配置。)
 - NPC parser profile config, basically how to parse pkt and what info to extract.
   (NPC 解析 profile 配置，即如何解析数据包以及提取哪些信息。)
 - NPC extract profile config, what to extract from the pkt to match data in MCAM entries.
   (NPC 提取 profile 配置，即从数据包中提取哪些内容以匹配 MCAM 表项中的数据。)
 - Manage NPC MCAM entries, upon request can frame and install requested packet forwarding rules.
   (管理 NPC MCAM 表项，可按需构造并安装所请求的数据包转发规则。)
 - Defines receive side scaling (RSS) algorithms.
   (定义接收侧扩展 (RSS) 算法。)
 - Defines segmentation offload algorithms (eg TSO)
   (定义分段卸载算法 (例如 TSO)。)
 - VLAN stripping, capture and insertion config.
   (VLAN 剥除、捕获与插入配置。)
 - SSO and TIM blocks config which provide packet scheduling support.
   (提供数据包调度支持的 SSO 与 TIM 块配置。)
 - Debugfs support, to check current resource provising, current status of
   NPA pools, NIX RQ, SQ and CQs, various stats etc which helps in debugging issues.
   (Debugfs 支持，用于查看当前资源分配情况、NPA 池、NIX RQ、SQ 与 CQ 的当前
   状态以及各类统计信息等，有助于排查问题。)
 - And many more.
   (还有更多。)

Physical Function driver
(物理功能驱动)
------------------------

This RVU PF handles IO, is mapped to a physical ethernet link and this
driver registers a netdev. This supports SR-IOV. As said above this driver
communicates with AF with a mailbox. To retrieve information from physical
links this driver talks to AF and AF gets that info from firmware and responds
back ie cannot talk to firmware directly.
(该 RVU PF 负责处理 IO，映射到一条物理以太网链路，此驱动注册一个 netdev。
它支持 SR-IOV。如上所述，该驱动通过 mailbox 与 AF 通信。要从物理链路获取信息，
该驱动会与 AF 通信，由 AF 从固件取得信息并回复，也就是说它不能直接与固件通信。)

Supports ethtool for configuring links, RSS, queue count, queue size,
flow control, ntuple filters, dump PHY EEPROM, config FEC etc.
(支持通过 ethtool 配置链路、RSS、队列数量、队列大小、流控、ntuple 过滤器、
转储 PHY EEPROM、配置 FEC 等。)

Virtual Function driver
(虚拟功能驱动)
-----------------------

There are two types VFs, VFs that share the physical link with their parent
SR-IOV PF and the VFs which work in pairs using internal HW loopback channels (LBK).
(VF 分为两类：一类与其父 SR-IOV PF 共享物理链路；另一类通过内部硬件环回通道
(LBK) 成对工作。)

Type1:
(类型 1：)

 - These VFs and their parent PF share a physical link and used for outside communication.
   (这些 VF 与其父 PF 共享一条物理链路，用于对外通信。)
 - VFs cannot communicate with AF directly, they send mbox message to PF and PF
   forwards that to AF. AF after processing, responds back to PF and PF forwards
   the reply to VF.
   (VF 无法直接与 AF 通信，它们将 mbox 消息发送给 PF，由 PF 转发给 AF。AF 处理
   之后向 PF 回复，再由 PF 将回复转发给 VF。)
 - From functionality point of view there is no difference between PF and VF as same type
   HW resources are attached to both. But user would be able to configure few stuff only
   from PF as PF is treated as owner/admin of the link.
   (从功能角度看，PF 与 VF 没有区别，因为两者附着的是同类硬件资源。但由于 PF
   被视为链路的所有者/管理者，用户只能在 PF 上完成部分配置。)

Type2:
(类型 2：)

 - RVU PF0 ie admin function creates these VFs and maps them to loopback block's channels.
   (这些 VF 由 RVU PF0 即管理功能创建，并映射到环回块的通道。)
 - A set of two VFs (VF0 & VF1, VF2 & VF3 .. so on) works as a pair ie pkts sent out of
   VF0 will be received by VF1 and vice versa.
   (每两个 VF 一组 (VF0 与 VF1、VF2 与 VF3 等) 成对工作，即从 VF0 发出的数据包
   会被 VF1 接收，反之亦然。)
 - These VFs can be used by applications or virtual machines to communicate between them
   without sending traffic outside. There is no switch present in HW, hence the support
   for loopback VFs.
   (这些 VF 可供应用程序或虚拟机在彼此之间通信，而无需将流量发往外部。硬件中
   不存在交换机，因此才有对环回 VF 的支持。)
 - These communicate directly with AF (PF0) via mbox.
   (它们通过 mbox 直接与 AF (PF0) 通信。)

Except for the IO channels or links used for packet reception and transmission there is
no other difference between these VF types. AF driver takes care of IO channel mapping,
hence same VF driver works for both types of devices.
(除了用于数据包收发的 IO 通道或链路之外，这两种 VF 类型没有其他区别。IO 通道
的映射由 AF 驱动负责，因此同一个 VF 驱动可同时适用于这两种类型的设备。)

Basic packet flow
(基本数据包流程)
=================

Ingress
(入方向)
-------

1. CGX LMAC receives packet.
   (CGX LMAC 接收数据包。)
2. Forwards the packet to the NIX block.
   (将数据包转发给 NIX 块。)
3. Then submitted to NPC block for parsing and then MCAM lookup to get the destination RVU device.
   (随后提交给 NPC 块进行解析，再经 MCAM 查找得到目的 RVU 设备。)
4. NIX LF attached to the destination RVU device allocates a buffer from RQ mapped buffer pool of NPA block LF.
   (附着在目的 RVU 设备上的 NIX LF 从 NPA 块 LF 的 RQ 映射缓冲区池中分配一个缓冲区。)
5. RQ may be selected by RSS or by configuring MCAM rule with a RQ number.
   (RQ 可由 RSS 选择，也可以通过配置带 RQ 编号的 MCAM 规则来指定。)
6. Packet is DMA'ed and driver is notified.
   (数据包经 DMA 传输，随后通知驱动。)

Egress
(出方向)
------

1. Driver prepares a send descriptor and submits to SQ for transmission.
   (驱动准备一个发送描述符并提交给 SQ 进行传输。)
2. The SQ is already configured (by AF) to transmit on a specific link/channel.
   (SQ 已由 AF 配置为在特定链路/通道上发送。)
3. The SQ descriptor ring is maintained in buffers allocated from SQ mapped pool of NPA block LF.
   (SQ 描述符环维护在从 NPA 块 LF 的 SQ 映射池中分配的缓冲区内。)
4. NIX block transmits the pkt on the designated channel.
   (NIX 块在指定通道上发送该数据包。)
5. NPC MCAM entries can be installed to divert pkt onto a different channel.
   (可以安装 NPC MCAM 表项，把数据包改道到其他通道。)

Devlink health reporters
(devlink 健康报告器)
========================

NPA Reporters
(NPA 报告器)
-------------

The NPA reporters are responsible for reporting and recovering the following group of errors:
(NPA 报告器负责报告并恢复以下几类错误：)

1. GENERAL events
   (GENERAL 事件)

   - Error due to operation of unmapped PF.
     (因操作未映射的 PF 而产生的错误。)
   - Error due to disabled alloc/free for other HW blocks (NIX, SSO, TIM, DPI and AURA).
     (因其他硬件块 (NIX、SSO、TIM、DPI 和 AURA) 的分配/释放被禁用而产生的错误。)

2. ERROR events
   (ERROR 事件)

   - Fault due to NPA_AQ_INST_S read or NPA_AQ_RES_S write.
     (因读取 NPA_AQ_INST_S 或写入 NPA_AQ_RES_S 而导致的故障。)
   - AQ Doorbell Error.
     (AQ 门铃错误。)

3. RAS events
   (RAS 事件)

   - RAS Error Reporting for NPA_AQ_INST_S/NPA_AQ_RES_S.
     (针对 NPA_AQ_INST_S/NPA_AQ_RES_S 的 RAS 错误报告。)

4. RVU events
   (RVU 事件)

   - Error due to unmapped slot.
     (因未映射的槽位而产生的错误。)

Sample Output::
(输出示例：)

	~# devlink health
	pci/0002:01:00.0:
	  reporter hw_npa_intr
	      state healthy error 2872 recover 2872 last_dump_date 2020-12-10 last_dump_time 09:39:09 grace_period 0 auto_recover true auto_dump true
	  reporter hw_npa_gen
	      state healthy error 2872 recover 2872 last_dump_date 2020-12-11 last_dump_time 04:43:04 grace_period 0 auto_recover true auto_dump true
	  reporter hw_npa_err
	      state healthy error 2871 recover 2871 last_dump_date 2020-12-10 last_dump_time 09:39:17 grace_period 0 auto_recover true auto_dump true
	   reporter hw_npa_ras
	      state healthy error 0 recover 0 last_dump_date 2020-12-10 last_dump_time 09:32:40 grace_period 0 auto_recover true auto_dump true

Each reporter dumps the
(每个报告器都会转储以下内容：)

 - Error Type
   (错误类型)
 - Error Register value
   (错误寄存器值)
 - Reason in words
   (以文字描述的原因)

For example::
(例如：)

	~# devlink health dump show  pci/0002:01:00.0 reporter hw_npa_gen
	 NPA_AF_GENERAL:
	         NPA General Interrupt Reg : 1
	         NIX0: free disabled RX
	~# devlink health dump show  pci/0002:01:00.0 reporter hw_npa_intr
	 NPA_AF_RVU:
	         NPA RVU Interrupt Reg : 1
	         Unmap Slot Error
	~# devlink health dump show  pci/0002:01:00.0 reporter hw_npa_err
	 NPA_AF_ERR:
	        NPA Error Interrupt Reg : 4096
	        AQ Doorbell Error


NIX Reporters
(NIX 报告器)
-------------

The NIX reporters are responsible for reporting and recovering the following group of errors:
(NIX 报告器负责报告并恢复以下几类错误：)

1. GENERAL events
   (GENERAL 事件)

   - Receive mirror/multicast packet drop due to insufficient buffer.
     (因缓冲区不足而丢弃接收到的镜像/组播数据包。)
   - SMQ Flush operation.
     (SMQ 冲刷操作。)

2. ERROR events
   (ERROR 事件)

   - Memory Fault due to WQE read/write from multicast/mirror buffer.
     (因对组播/镜像缓冲区进行 WQE 读写而导致的内存故障。)
   - Receive multicast/mirror replication list error.
     (接收组播/镜像复制列表错误。)
   - Receive packet on an unmapped PF.
     (在未映射的 PF 上收到数据包。)
   - Fault due to NIX_AQ_INST_S read or NIX_AQ_RES_S write.
     (因读取 NIX_AQ_INST_S 或写入 NIX_AQ_RES_S 而导致的故障。)
   - AQ Doorbell Error.
     (AQ 门铃错误。)

3. RAS events
   (RAS 事件)

   - RAS Error Reporting for NIX Receive Multicast/Mirror Entry Structure.
     (针对 NIX 接收组播/镜像表项结构的 RAS 错误报告。)
   - RAS Error Reporting for WQE/Packet Data read from Multicast/Mirror Buffer..
     (针对从组播/镜像缓冲区读取 WQE/数据包数据的 RAS 错误报告。)
   - RAS Error Reporting for NIX_AQ_INST_S/NIX_AQ_RES_S.
     (针对 NIX_AQ_INST_S/NIX_AQ_RES_S 的 RAS 错误报告。)

4. RVU events
   (RVU 事件)

   - Error due to unmapped slot.
     (因未映射的槽位而产生的错误。)

Sample Output::
(输出示例：)

	~# ./devlink health
	pci/0002:01:00.0:
	  reporter hw_npa_intr
	    state healthy error 0 recover 0 grace_period 0 auto_recover true auto_dump true
	  reporter hw_npa_gen
	    state healthy error 0 recover 0 grace_period 0 auto_recover true auto_dump true
	  reporter hw_npa_err
	    state healthy error 0 recover 0 grace_period 0 auto_recover true auto_dump true
	  reporter hw_npa_ras
	    state healthy error 0 recover 0 grace_period 0 auto_recover true auto_dump true
	  reporter hw_nix_intr
	    state healthy error 1121 recover 1121 last_dump_date 2021-01-19 last_dump_time 05:42:26 grace_period 0 auto_recover true auto_dump true
	  reporter hw_nix_gen
	    state healthy error 949 recover 949 last_dump_date 2021-01-19 last_dump_time 05:42:43 grace_period 0 auto_recover true auto_dump true
	  reporter hw_nix_err
	    state healthy error 1147 recover 1147 last_dump_date 2021-01-19 last_dump_time 05:42:59 grace_period 0 auto_recover true auto_dump true
	  reporter hw_nix_ras
	    state healthy error 409 recover 409 last_dump_date 2021-01-19 last_dump_time 05:43:16 grace_period 0 auto_recover true auto_dump true

Each reporter dumps the
(每个报告器都会转储以下内容：)

 - Error Type
   (错误类型)
 - Error Register value
   (错误寄存器值)
 - Reason in words
   (以文字描述的原因)

For example::
(例如：)

	~# devlink health dump show pci/0002:01:00.0 reporter hw_nix_intr
	 NIX_AF_RVU:
	        NIX RVU Interrupt Reg : 1
	        Unmap Slot Error
	~# devlink health dump show pci/0002:01:00.0 reporter hw_nix_gen
	 NIX_AF_GENERAL:
	        NIX General Interrupt Reg : 1
	        Rx multicast pkt drop
	~# devlink health dump show pci/0002:01:00.0 reporter hw_nix_err
	 NIX_AF_ERR:
	        NIX Error Interrupt Reg : 64
	        Rx on unmapped PF_FUNC


Quality of service
(服务质量)
==================


Hardware algorithms used in scheduling
(调度中使用的硬件算法)
--------------------------------------

octeontx2 silicon and CN10K transmit interface consists of five transmit levels
starting from SMQ/MDQ, TL4 to TL1. Each packet will traverse MDQ, TL4 to TL1
levels. Each level contains an array of queues to support scheduling and shaping.
The hardware uses the below algorithms depending on the priority of scheduler queues.
once the usercreates tc classes with different priorities, the driver configures
schedulers allocated to the class with specified priority along with rate-limiting
configuration.
(octeontx2 芯片与 CN10K 的发送接口由五个发送层级组成，从 SMQ/MDQ、TL4 直到
TL1。每个数据包都会经过 MDQ、TL4 到 TL1 各层级。每个层级包含一组队列，用于
支持调度与整形。硬件会根据调度器队列的优先级采用以下算法。当用户创建具有
不同优先级的 tc 类时，驱动会按照指定优先级以及限速配置来设置分配给该类的
调度器。)

1. Strict Priority
   (严格优先级)

      -  Once packets are submitted to MDQ, hardware picks all active MDQs having different priority
         using strict priority.
         (数据包一旦提交到 MDQ，硬件就会使用严格优先级算法挑选所有处于活动状态、
         优先级各不相同的 MDQ。)

2. Round Robin
   (轮询)

      - Active MDQs having the same priority level are chosen using round robin.
        (优先级相同的活动 MDQ 通过轮询方式选取。)


Setup HTB offload
(设置 HTB 卸载)
-----------------

1. Enable HW TC offload on the interface::
   (在接口上启用硬件 TC 卸载：)

        # ethtool -K <interface> hw-tc-offload on

2. Create htb root::
   (创建 htb 根节点：)

        # tc qdisc add dev <interface> clsact
        # tc qdisc replace dev <interface> root handle 1: htb offload

3. Create tc classes with different priorities::
   (创建具有不同优先级的 tc 类：)

        # tc class add dev <interface> parent 1: classid 1:1 htb rate 10Gbit prio 1

        # tc class add dev <interface> parent 1: classid 1:2 htb rate 10Gbit prio 7

4. Create tc classes with same priorities and different quantum::
   (创建优先级相同但 quantum 不同的 tc 类：)

        # tc class add dev <interface> parent 1: classid 1:1 htb rate 10Gbit prio 2 quantum 409600

        # tc class add dev <interface> parent 1: classid 1:2 htb rate 10Gbit prio 2 quantum 188416

        # tc class add dev <interface> parent 1: classid 1:3 htb rate 10Gbit prio 2 quantum 32768


RVU Representors
(RVU 代表设备)
================

RVU representor driver adds support for creation of representor devices for
RVU PFs' VFs in the system. Representor devices are created when user enables
the switchdev mode.
Switchdev mode can be enabled either before or after setting up SRIOV numVFs.
All representor devices share a single NIXLF but each has a dedicated Rx/Tx
queues. RVU PF representor driver registers a separate netdev for each
Rx/Tx queue pair.
(RVU 代表设备驱动新增了对系统中 RVU PF 的 VF 创建代表设备 (representor) 的
支持。当用户启用 switchdev 模式时会创建这些代表设备。switchdev 模式既可以在
设置 SRIOV numVFs 之前启用，也可以在其之后启用。所有代表设备共享一个
NIXLF，但各自拥有专用的 Rx/Tx 队列。RVU PF 代表设备驱动为每个 Rx/Tx 队列对
注册一个独立的 netdev。)

Current HW does not support built-in switch which can do L2 learning and
forwarding packets between representee and representor. Hence, packet path
between representee and it's representor is achieved by setting up appropriate
NPC MCAM filters.
Transmit packets matching these filters will be loopbacked through hardware
loopback channel/interface (i.e, instead of sending them out of MAC interface).
Which will again match the installed filters and will be forwarded.
This way representee => representor and representor => representee packet
path is achieved. These rules get installed when representors are created
and gets active/deactivate based on the representor/representee interface state.
(当前硬件不支持能够在被代表设备 (representee) 与代表设备之间进行 L2 学习并
转发数据包的内置交换机。因此，被代表设备与其代表设备之间的数据包路径是通过
设置合适的 NPC MCAM 过滤器来实现的。匹配这些过滤器的发送数据包会经硬件环回
通道/接口环回 (即不再从 MAC 接口发出)。这些数据包会再次匹配已安装的过滤器
并被转发。这样便实现了被代表设备 => 代表设备以及代表设备 => 被代表设备的
数据包路径。这些规则在代表设备创建时安装，并根据代表设备/被代表设备接口的
状态被激活/停用。)

Usage example:
(用法示例：)

 - Change device to switchdev mode::
   (将设备切换到 switchdev 模式：)

	# devlink dev eswitch set pci/0002:1c:00.0 mode switchdev

 - List of representor devices on the system::
   (列出系统中的代表设备：)

	# ip link show
	Rpf1vf0: <BROADCAST,MULTICAST,UP,LOWER_UP> mtu 1500 qdisc pfifo_fast state DOWN mode DEFAULT group default qlen 1000 link/ether f6:43:83:ee:26:21 brd ff:ff:ff:ff:ff:ff
	Rpf1vf1: <BROADCAST,MULTICAST,UP,LOWER_UP> mtu 1500 qdisc pfifo_fast state DOWN mode DEFAULT group default qlen 1000 link/ether 12:b2:54:0e:24:54 brd ff:ff:ff:ff:ff:ff
	Rpf1vf2: <BROADCAST,MULTICAST,UP,LOWER_UP> mtu 1500 qdisc pfifo_fast state DOWN mode DEFAULT group default qlen 1000 link/ether 4a:12:c4:4c:32:62 brd ff:ff:ff:ff:ff:ff
	Rpf1vf3: <BROADCAST,MULTICAST,UP,LOWER_UP> mtu 1500 qdisc pfifo_fast state DOWN mode DEFAULT group default qlen 1000 link/ether ca:cb:68:0e:e2:6e brd ff:ff:ff:ff:ff:ff
	Rpf2vf0: <BROADCAST,MULTICAST,UP,LOWER_UP> mtu 1500 qdisc pfifo_fast state DOWN mode DEFAULT group default qlen 1000 link/ether 06:cc:ad:b4:f0:93 brd ff:ff:ff:ff:ff:ff


To delete the representors devices from the system. Change the device to legacy mode.
(要从系统中删除这些代表设备，需将设备切换回传统 (legacy) 模式。)

 - Change device to legacy mode::
   (将设备切换到传统模式：)

	# devlink dev eswitch set pci/0002:1c:00.0 mode legacy

RVU representors can be managed using devlink ports
(see :ref:`Documentation/networking/devlink/devlink-port.rst <devlink_port>`) interface.
(RVU 代表设备可以通过 devlink 端口接口进行管理
(参见 :ref:`Documentation/networking/devlink/devlink-port.rst <devlink_port>`)。)

 - Show devlink ports of representors::
   (显示代表设备的 devlink 端口：)

	# devlink port
	pci/0002:1c:00.0/0: type eth netdev Rpf1vf0 flavour physical port 0 splittable false
	pci/0002:1c:00.0/1: type eth netdev Rpf1vf1 flavour pcivf controller 0 pfnum 1 vfnum 1 external false splittable false
	pci/0002:1c:00.0/2: type eth netdev Rpf1vf2 flavour pcivf controller 0 pfnum 1 vfnum 2 external false splittable false
	pci/0002:1c:00.0/3: type eth netdev Rpf1vf3 flavour pcivf controller 0 pfnum 1 vfnum 3 external false splittable false

Function attributes
(功能属性)
===================

The RVU representor support function attributes for representors.
Port function configuration of the representors are supported through devlink eswitch port.
(RVU 代表设备为代表设备提供功能属性支持。代表设备的端口功能配置可通过
devlink eswitch 端口完成。)

MAC address setup
(MAC 地址设置)
-----------------

RVU representor driver support devlink port function attr mechanism to setup MAC
address. (refer to Documentation/networking/devlink/devlink-port.rst)
(RVU 代表设备驱动支持通过 devlink 端口功能属性机制设置 MAC 地址。
(参见 Documentation/networking/devlink/devlink-port.rst))

 - To setup MAC address for port 2::
   (为端口 2 设置 MAC 地址：)

	# devlink port function set pci/0002:1c:00.0/2 hw_addr 5c:a1:1b:5e:43:11
	# devlink port show pci/0002:1c:00.0/2
	pci/0002:1c:00.0/2: type eth netdev Rpf1vf2 flavour pcivf controller 0 pfnum 1 vfnum 2 external false splittable false
	function:
		hw_addr 5c:a1:1b:5e:43:11


TC offload
(TC 卸载)
==========

The rvu representor driver implements support for offloading tc rules using port representors.
(rvu 代表设备驱动实现了通过端口代表设备卸载 tc 规则的支持。)

 - Drop packets with vlan id 3::
   (丢弃 vlan id 为 3 的数据包：)

	# tc filter add dev Rpf1vf0 protocol 802.1Q parent ffff: flower vlan_id 3 vlan_ethtype ipv4 skip_sw action drop

 - Redirect packets with vlan id 5 and IPv4 packets to eth1, after stripping vlan header.::
   (在剥除 vlan 头部之后，将 vlan id 为 5 的数据包和 IPv4 数据包重定向到 eth1：)

	# tc filter add dev Rpf1vf0 ingress protocol 802.1Q flower vlan_id 5 vlan_ethtype ipv4 skip_sw action vlan pop action mirred ingress redirect dev eth1
