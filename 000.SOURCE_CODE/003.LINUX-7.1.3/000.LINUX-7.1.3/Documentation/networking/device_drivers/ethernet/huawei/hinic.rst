.. SPDX-License-Identifier: GPL-2.0

============================================================
Linux Kernel Driver for Huawei Intelligent NIC(HiNIC) family
============================================================

(华为智能网卡 (HiNIC) 家族的 Linux 内核驱动)

Overview:
(概述：)
=========

HiNIC is a network interface card for the Data Center Area.
(HiNIC 是一款面向数据中心领域的网络接口卡。)

The driver supports a range of link-speed devices (10GbE, 25GbE, 40GbE, etc.).
The driver supports also a negotiated and extendable feature set.
(该驱动支持一系列链路速率的设备 (10GbE、25GbE、40GbE 等)。该驱动还支持
可协商、可扩展的特性集。)

Some HiNIC devices support SR-IOV. This driver is used for Physical Function
(PF).
(部分 HiNIC 设备支持 SR-IOV。本驱动用于物理功能 (PF)。)

HiNIC devices support MSI-X interrupt vector for each Tx/Rx queue and
adaptive interrupt moderation.
(HiNIC 设备为每个 Tx/Rx 队列支持 MSI-X 中断向量以及自适应中断调谐。)

HiNIC devices support also various offload features such as checksum offload,
TCP Transmit Segmentation Offload(TSO), Receive-Side Scaling(RSS) and
LRO(Large Receive Offload).
(HiNIC 设备还支持多种卸载特性，例如校验和卸载、TCP 传输分段卸载 (TSO)、
接收侧扩展 (RSS) 以及 LRO (大接收卸载)。)


Supported PCI vendor ID/device IDs:
(支持的 PCI 厂商 ID/设备 ID：)
===================================

19e5:1822 - HiNIC PF
(19e5:1822 对应 HiNIC PF。)


Driver Architecture and Source Code:
(驱动架构与源码：)
====================================

hinic_dev - Implement a Logical Network device that is independent from
specific HW details about HW data structure formats.
(hinic_dev - 实现一个逻辑网络设备，该设备不依赖于关于 HW 数据结构格式的
具体硬件细节。)

hinic_hwdev - Implement the HW details of the device and include the components
for accessing the PCI NIC.
(hinic_hwdev - 实现设备的硬件细节，并包含用于访问 PCI 网卡的组件。)

hinic_hwdev contains the following components:
(hinic_hwdev 包含以下组件：)
===============================================

HW Interface:
(硬件接口：)
=============

The interface for accessing the pci device (DMA memory and PCI BARs).
(hinic_hw_if.c, hinic_hw_if.h)
(用于访问 PCI 设备 (DMA 内存和 PCI BAR) 的接口。
(hinic_hw_if.c, hinic_hw_if.h))

Configuration Status Registers Area that describes the HW Registers on the
configuration and status BAR0. (hinic_hw_csr.h)
(配置状态寄存器区域，描述配置与状态 BAR0 上的硬件寄存器。(hinic_hw_csr.h))

MGMT components:
(MGMT 组件：)
================

Asynchronous Event Queues(AEQs) - The event queues for receiving messages from
the MGMT modules on the cards. (hinic_hw_eqs.c, hinic_hw_eqs.h)
(异步事件队列 (AEQs) - 用于从网卡上的 MGMT 模块接收消息的事件队列。
(hinic_hw_eqs.c, hinic_hw_eqs.h))

Application Programmable Interface commands(API CMD) - Interface for sending
MGMT commands to the card. (hinic_hw_api_cmd.c, hinic_hw_api_cmd.h)
(应用可编程接口命令 (API CMD) - 用于向网卡发送 MGMT 命令的接口。
(hinic_hw_api_cmd.c, hinic_hw_api_cmd.h))

Management (MGMT) - the PF to MGMT channel that uses API CMD for sending MGMT
commands to the card and receives notifications from the MGMT modules on the
card by AEQs. Also set the addresses of the IO CMDQs in HW.
(hinic_hw_mgmt.c, hinic_hw_mgmt.h)
(管理 (MGMT) - PF 到 MGMT 的通道，它使用 API CMD 向网卡发送 MGMT 命令，
并通过 AEQ 从网卡上的 MGMT 模块接收通知。还会在硬件中设置 IO CMDQ 的
地址。(hinic_hw_mgmt.c, hinic_hw_mgmt.h))

IO components:
(IO 组件：)
==============

Completion Event Queues(CEQs) - The completion Event Queues that describe IO
tasks that are finished. (hinic_hw_eqs.c, hinic_hw_eqs.h)
(完成事件队列 (CEQs) - 描述已完成的 IO 任务的完成事件队列。
(hinic_hw_eqs.c, hinic_hw_eqs.h))

Work Queues(WQ) - Contain the memory and operations for use by CMD queues and
the Queue Pairs. The WQ is a Memory Block in a Page. The Block contains
pointers to Memory Areas that are the Memory for the Work Queue Elements(WQEs).
(hinic_hw_wq.c, hinic_hw_wq.h)
(工作队列 (WQ) - 包含供 CMD 队列和队列对使用的内存与操作。WQ 是一个页面
中的内存块。该块包含指向内存区的指针，这些内存区就是工作队列元素 (WQEs)
的内存。(hinic_hw_wq.c, hinic_hw_wq.h))

Command Queues(CMDQ) - The queues for sending commands for IO management and is
used to set the QPs addresses in HW. The commands completion events are
accumulated on the CEQ that is configured to receive the CMDQ completion events.
(hinic_hw_cmdq.c, hinic_hw_cmdq.h)
(命令队列 (CMDQ) - 用于发送 IO 管理命令的队列，也用于在硬件中设置 QP
地址。命令完成事件会在被配置为接收 CMDQ 完成事件的 CEQ 上累积。
(hinic_hw_cmdq.c, hinic_hw_cmdq.h))

Queue Pairs(QPs) - The HW Receive and Send queues for Receiving and Transmitting
Data. (hinic_hw_qp.c, hinic_hw_qp.h, hinic_hw_qp_ctxt.h)
(队列对 (QPs) - 用于接收和发送数据的硬件接收与发送队列。
(hinic_hw_qp.c, hinic_hw_qp.h, hinic_hw_qp_ctxt.h))

IO - de/constructs all the IO components. (hinic_hw_io.c, hinic_hw_io.h)
(IO - 构建/解构所有 IO 组件。(hinic_hw_io.c, hinic_hw_io.h))

HW device:
(硬件设备)
==========

HW device - de/constructs the HW Interface, the MGMT components on the
initialization of the driver and the IO components on the case of Interface
UP/DOWN Events. (hinic_hw_dev.c, hinic_hw_dev.h)
(硬件设备 - 在驱动初始化时构建/解构硬件接口和 MGMT 组件，在接口 UP/DOWN
事件时构建/解构 IO 组件。(hinic_hw_dev.c, hinic_hw_dev.h))


hinic_dev contains the following components:
(hinic_dev 包含以下组件：)
===============================================

PCI ID table - Contains the supported PCI Vendor/Device IDs.
(hinic_pci_tbl.h)
(PCI ID 表 - 包含受支持的 PCI 厂商/设备 ID。(hinic_pci_tbl.h))

Port Commands - Send commands to the HW device for port management
(MAC, Vlan, MTU, ...). (hinic_port.c, hinic_port.h)
(端口命令 - 向硬件设备发送用于端口管理 (MAC、Vlan、MTU 等) 的命令。
(hinic_port.c, hinic_port.h))

Tx Queues - Logical Tx Queues that use the HW Send Queues for transmit.
The Logical Tx queue is not dependent on the format of the HW Send Queue.
(hinic_tx.c, hinic_tx.h)
(Tx 队列 - 使用硬件发送队列进行发送的逻辑 Tx 队列。逻辑 Tx 队列不依赖于
硬件发送队列的格式。(hinic_tx.c, hinic_tx.h))

Rx Queues - Logical Rx Queues that use the HW Receive Queues for receive.
The Logical Rx queue is not dependent on the format of the HW Receive Queue.
(hinic_rx.c, hinic_rx.h)
(Rx 队列 - 使用硬件接收队列进行接收的逻辑 Rx 队列。逻辑 Rx 队列不依赖于
硬件接收队列的格式。(hinic_rx.c, hinic_rx.h))

hinic_dev - de/constructs the Logical Tx and Rx Queues.
(hinic_main.c, hinic_dev.h)
(hinic_dev - 构建/解构逻辑 Tx 和 Rx 队列。(hinic_main.c, hinic_dev.h))


Miscellaneous
(杂项)
=============

Common functions that are used by HW and Logical Device.
(hinic_common.c, hinic_common.h)
(由硬件和逻辑设备共同使用的通用函数。(hinic_common.c, hinic_common.h))


Support
(支持)
=======

If an issue is identified with the released source code on the supported kernel
with a supported adapter, email the specific information related to the issue to
aviad.krawczyk@huawei.com.
(如果在受支持的内核上配合受支持的适配器使用已发布的源代码时发现某问题，
请将与该问题相关的具体信息通过邮件发送到 aviad.krawczyk@huawei.com。)
