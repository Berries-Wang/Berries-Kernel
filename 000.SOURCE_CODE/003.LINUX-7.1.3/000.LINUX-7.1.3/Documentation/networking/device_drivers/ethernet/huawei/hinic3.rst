.. SPDX-License-Identifier: GPL-2.0

========================================================================================================================
Linux kernel driver for Huawei Ethernet Device Driver (hinic3) family(华为以太网设备驱动 (hinic3) 家族的 Linux 内核驱动)
========================================================================================================================

Overview(概述)
==============

The hinic3 is a network interface card (NIC) for Data Center. It supports
a range of link-speed devices (10GE, 25GE, 100GE, etc.). The hinic3
devices can have multiple physical forms: LOM (Lan on Motherboard) NIC,
PCIe standard NIC, OCP (Open Compute Project) NIC, etc.
(hinic3 是一款面向数据中心的网络接口卡 (NIC)。它支持一系列链路速率的设备
(10GE、25GE、100GE 等)。hinic3 设备可以有多种物理形态：LOM (Lan on
Motherboard，板载网卡) NIC、PCIe 标准网卡、OCP (Open Compute Project) NIC
等。)

The hinic3 driver supports the following features:
(hinic3 驱动支持以下特性：)

- IPv4/IPv6 TCP/UDP checksum offload
  (IPv4/IPv6 TCP/UDP 校验和卸载)
- TSO (TCP Segmentation Offload), LRO (Large Receive Offload)
  (TSO (TCP 分段卸载)、LRO (大接收卸载))
- RSS (Receive Side Scaling)
  (RSS (接收侧扩展))
- MSI-X interrupt aggregation configuration and interrupt adaptation.
  (MSI-X 中断聚合配置与中断自适应。)
- SR-IOV (Single Root I/O Virtualization).
  (SR-IOV (单根 I/O 虚拟化)。)

Content(目录)
=============

- Supported PCI vendor ID/device IDs
  (支持的 PCI 厂商 ID/设备 ID)
- Source Code Structure of Hinic3 Driver
  (Hinic3 驱动源码结构)
- Management Interface
  (管理接口)

Supported PCI vendor ID/device IDs(支持的 PCI 厂商 ID/设备 ID)
==============================================================

19e5:0222 - hinic3 PF/PPF
19e5:375F - hinic3 VF
(19e5:0222 对应 hinic3 PF/PPF，19e5:375F 对应 hinic3 VF。)

Prime Physical Function (PPF) is responsible for the management of the
whole NIC card. For example, clock synchronization between the NIC and
the host. Any PF may serve as a PPF. The PPF is selected dynamically.
(主物理功能 (Prime Physical Function，PPF) 负责管理整块网卡。例如，网卡
与主机之间的时钟同步。任何 PF 都可以充当 PPF。PPF 是动态选择的。)

Source Code Structure of Hinic3 Driver(Hinic3 驱动源码结构)
===========================================================

========================  ================================================
hinic3_pci_id_tbl.h       Supported device IDs (支持的设备 ID)
hinic3_hw_intf.h          Interface between HW and driver (硬件与驱动之间的接口)
hinic3_queue_common.[ch]  Common structures and methods for NIC queues (网卡队列的通用结构与方法)
hinic3_common.[ch]        Encapsulation of memory operations in Linux (Linux 内存操作的封装)
hinic3_csr.h              Register definitions in the BAR (BAR 中的寄存器定义)
hinic3_hwif.[ch]          Interface for BAR (BAR 接口)
hinic3_eqs.[ch]           Interface for AEQs and CEQs (AEQ 与 CEQ 接口)
hinic3_mbox.[ch]          Interface for mailbox (邮箱接口)
hinic3_mgmt.[ch]          Management interface based on mailbox and AEQ (基于邮箱和 AEQ 的管理接口)
hinic3_wq.[ch]            Work queue data structures and interface (工作队列数据结构与接口)
hinic3_cmdq.[ch]          Command queue is used to post command to HW (命令队列，用于向硬件提交命令)
hinic3_hwdev.[ch]         HW structures and methods abstractions (硬件结构与方法的抽象)
hinic3_lld.[ch]           Auxiliary driver adaptation layer (辅助驱动适配层)
hinic3_hw_comm.[ch]       Interface for common HW operations (通用硬件操作接口)
hinic3_mgmt_interface.h   Interface between firmware and driver (固件与驱动之间的接口)
hinic3_hw_cfg.[ch]        Interface for HW configuration (硬件配置接口)
hinic3_irq.c              Interrupt request (中断请求)
hinic3_netdev_ops.c       Operations registered to Linux kernel stack (注册到 Linux 内核协议栈的操作)
hinic3_nic_dev.h          NIC structures and methods abstractions (网卡结构与方法的抽象)
hinic3_main.c             Main Linux kernel driver (Linux 内核主驱动)
hinic3_nic_cfg.[ch]       NIC service configuration (网卡服务配置)
hinic3_nic_io.[ch]        Management plane interface for TX and RX (发送与接收的管理面接口)
hinic3_rss.[ch]           Interface for Receive Side Scaling (RSS) (接收侧扩展 (RSS) 接口)
hinic3_rx.[ch]            Interface for transmit (发送接口)
hinic3_tx.[ch]            Interface for receive (接收接口)
hinic3_ethtool.c          Interface for ethtool operations (ops) (ethtool 操作 (ops) 接口)
hinic3_filter.c           Interface for MAC address (MAC 地址接口)
========================  ================================================

Management Interface(管理接口)
==============================

Asynchronous Event Queue (AEQ)(异步事件队列 (AEQ))
--------------------------------------------------

AEQ receives high priority events from the HW over a descriptor queue.
Every descriptor is a fixed size of 64 bytes. AEQ can receive solicited or
unsolicited events. Every device, VF or PF, can have up to 4 AEQs.
Every AEQ is associated to a dedicated IRQ. AEQ can receive multiple types
of events, but in practice the hinic3 driver ignores all events except for
2 mailbox related events.
(AEQ 通过描述符队列接收来自硬件的高优先级事件。每个描述符的固定大小为
64 字节。AEQ 可以接收主动请求或非主动请求的事件。每个设备 (无论是 VF 还是
PF) 最多可以拥有 4 个 AEQ。每个 AEQ 都关联一个专用 IRQ。AEQ 可以接收多种
类型的事件，但实际上 hinic3 驱动会忽略除 2 个邮箱相关事件之外的所有事件。)

Mailbox(邮箱)
-------------

Mailbox is a communication mechanism between the hinic3 driver and the HW.
Each device has an independent mailbox. Driver can use the mailbox to send
requests to management. Driver receives mailbox messages, such as responses
to requests, over the AEQ (using event HINIC3_AEQ_FOR_MBOX). Due to the
limited size of mailbox data register, mailbox messages are sent
segment-by-segment.
(邮箱是 hinic3 驱动与硬件之间的通信机制。每个设备都有独立的邮箱。驱动可以
使用邮箱向管理模块发送请求。驱动通过 AEQ (使用事件 HINIC3_AEQ_FOR_MBOX)
接收邮箱消息，例如对请求的响应。由于邮箱数据寄存器的大小有限，邮箱消息是
按分段发送的。)

Every device can use its mailbox to post request to firmware. The mailbox
can also be used to post requests and responses between the PF and its VFs.
(每个设备都可以使用自己的邮箱向固件提交请求。邮箱也可以用于在 PF 与其 VF
之间传递请求和响应。)

Completion Event Queue (CEQ)(完成事件队列 (CEQ))
------------------------------------------------

The implementation of CEQ is the same as AEQ. It receives completion events
from HW over a fixed size descriptor of 32 bits. Every device can have up
to 32 CEQs. Every CEQ has a dedicated IRQ. CEQ only receives solicited
events that are responses to requests from the driver. CEQ can receive
multiple types of events, but in practice the hinic3 driver ignores all
events except for HINIC3_CMDQ that represents completion of previously
posted commands on a cmdq.
(CEQ 的实现与 AEQ 相同。它通过固定大小为 32 位的描述符接收来自硬件的完成
事件。每个设备最多可以拥有 32 个 CEQ。每个 CEQ 都有一个专用 IRQ。CEQ 只
接收作为对驱动请求之响应的主动请求事件。CEQ 可以接收多种类型的事件，但
实际上 hinic3 驱动会忽略除 HINIC3_CMDQ (表示 cmdq 上先前提交的命令已完成)
之外的所有事件。)

Command Queue (cmdq)(命令队列 (cmdq))
-------------------------------------

Every cmdq has a dedicated work queue on which commands are posted.
Commands on the work queue are fixed size descriptor of size 64 bytes.
Completion of a command will be indicated using ctrl bits in the
descriptor that carried the command. Notification of command completions
will also be provided via event on CEQ. Every device has 4 command queues
that are initialized as a set (called cmdqs), each with its own type.
Hinic3 driver only uses type HINIC3_CMDQ_SYNC.
(每个 cmdq 都有一个专用工作队列，命令提交到该队列上。工作队列上的命令是
固定大小为 64 字节的描述符。命令的完成将通过携带该命令的描述符中的控制位
来指示。命令完成的通知也会通过 CEQ 上的事件提供。每个设备都有 4 个命令
队列，它们作为一个集合被初始化 (称为 cmdqs)，各自有自己的类型。Hinic3
驱动只使用 HINIC3_CMDQ_SYNC 类型。)

Work Queues(WQ)(工作队列 (WQ))
------------------------------

Work queues are logical arrays of fixed size WQEs. The array may be spread
over multiple non-contiguous pages using indirection table. Work queues are
used by I/O queues and command queues.
(工作队列是固定大小 WQE 的逻辑数组。该数组可以使用间接表分布在多个不连续
的页面上。I/O 队列和命令队列都会使用工作队列。)

Global function ID(全局功能 ID)
-------------------------------

Every function, PF or VF, has a unique ordinal identification within the device.
Many management commands (mbox or cmdq) contain this ID so HW can apply the
command effect to the right function.
(每个功能 (无论是 PF 还是 VF) 在设备内都有唯一的序号标识。许多管理命令
(mbox 或 cmdq) 都包含此 ID，以便硬件能把命令的作用施加到正确的功能上。)

PF is allowed to post management commands to a subordinate VF by specifying the
VFs ID. A VF must provide its own ID. Anti-spoofing in the HW will cause
command from a VF to fail if it contains the wrong ID.
(允许 PF 通过指定 VF 的 ID 向其下属的 VF 提交管理命令。VF 必须提供自己的
ID。如果 VF 发出的命令包含错误的 ID，硬件中的防欺骗机制会使其失败。)
