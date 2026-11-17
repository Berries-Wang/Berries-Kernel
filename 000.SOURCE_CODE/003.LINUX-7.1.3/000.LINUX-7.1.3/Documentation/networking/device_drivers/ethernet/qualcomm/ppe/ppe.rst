.. SPDX-License-Identifier: GPL-2.0

===============================================
PPE Ethernet Driver for Qualcomm IPQ SoC Family
===============================================

(Qualcomm IPQ SoC 系列的 PPE 以太网驱动)

Copyright (c) Qualcomm Technologies, Inc. and/or its subsidiaries.
(版权所有 (c) Qualcomm Technologies, Inc. 及/或其子公司。)

Author: Lei Wei <quic_leiwei@quicinc.com>
(作者: Lei Wei <quic_leiwei@quicinc.com>)


Contents
(目录)
========

- `PPE Overview`_
  (PPE 概述)
- `PPE Driver Overview`_
  (PPE 驱动概述)
- `PPE Driver Supported SoCs`_
  (PPE 驱动支持的 SoC)
- `Enabling the Driver`_
  (启用该驱动)
- `Debugging`_
  (调试)


PPE Overview
(PPE 概述)
============

IPQ (Qualcomm Internet Processor) SoC (System-on-Chip) series is Qualcomm's series of
networking SoC for Wi-Fi access points. The PPE (Packet Process Engine) is the Ethernet
packet process engine in the IPQ SoC.
(IPQ（Qualcomm Internet Processor）SoC（片上系统）系列是 Qualcomm 面向 Wi-Fi 接入点的网络 SoC 系列。PPE
（Packet Process Engine，数据包处理引擎）是 IPQ SoC 中的以太网数据包处理引擎。)

Below is a simplified hardware diagram of IPQ9574 SoC which includes the PPE engine and
other blocks which are in the SoC but outside the PPE engine. These blocks work together
to enable the Ethernet for the IPQ SoC::
(下面是 IPQ9574 SoC 的简化硬件框图，其中包含 PPE 引擎以及位于 SoC 中但在 PPE 引擎之外的其他模块。这些模块
协同工作，为 IPQ SoC 提供以太网功能：)

               +------+ +------+ +------+ +------+ +------+  +------+ start +-------+
               |netdev| |netdev| |netdev| |netdev| |netdev|  |netdev|<------|PHYLINK|
               +------+ +------+ +------+ +------+ +------+  +------+ stop  +-+-+-+-+
                                             |                                | | ^
 +-------+     +-------------------------+--------+----------------------+    | | |
 | GCC   |     |                         |  EDMA  |                      |    | | |
 +---+---+     |  PPE                    +---+----+                      |    | | |
     | clk     |                             |                           |    | | |
     +-------->| +-----------------------+------+-----+---------------+  |    | | |
               | |   Switch Core         |Port0 |     |Port7(EIP FIFO)|  |    | | |
               | |                       +---+--+     +------+--------+  |    | | |
               | |                           |               |        |  |    | | |
 +-------+     | |                    +------+---------------+----+   |  |    | | |
 |CMN PLL|     | | +---+ +---+ +----+ | +--------+                |   |  |    | | |
 +---+---+     | | |BM | |QM | |SCH | | | L2/L3  |  .......       |   |  |    | | |
 |   |         | | +---+ +---+ +----+ | +--------+                |   |  |    | | |
 |   |         | |                    +------+--------------------+   |  |    | | |
 |   |         | |                           |                        |  |    | | |
 |   v         | | +-----+-+-----+-+-----+-+-+---+--+-----+-+-----+   |  |    | | |
 | +------+    | | |Port1| |Port2| |Port3| |Port4|  |Port5| |Port6|   |  |    | | |
 | |NSSCC |    | | +-----+ +-----+ +-----+ +-----+  +-----+ +-----+   |  | mac| | |
 | +-+-+--+    | | |MAC0 | |MAC1 | |MAC2 | |MAC3 |  |MAC4 | |MAC5 |   |  |<---+ | |
 | ^ | |clk    | | +-----+-+-----+-+-----+-+-----+--+-----+-+-----+   |  | ops  | |
 | | | +------>| +----|------|-------|-------|---------|--------|-----+  |      | |
 | | |         +---------------------------------------------------------+      | |
 | | |                |      |       |       |         |        |               | |
 | | |   MII clk      |      QSGMII               USXGMII   USXGMII             | |
 | | +--------------->|      |       |       |         |        |               | |
 | |                +-------------------------+ +---------+ +---------+         | |
 | |125/312.5MHz clk|       (PCS0)            | | (PCS1)  | | (PCS2)  | pcs ops | |
 | +----------------+       UNIPHY0           | | UNIPHY1 | | UNIPHY2 |<--------+ |
 +----------------->|                         | |         | |         |           |
 | 31.25MHz ref clk +-------------------------+ +---------+ +---------+           |
 |                     |     |      |      |          |          |                |
 |                +-----------------------------------------------------+         |
 |25/50MHz ref clk| +-------------------------+    +------+   +------+  | link    |
 +--------------->| |      QUAD PHY           |    | PHY4 |   | PHY5 |  |---------+
                  | +-------------------------+    +------+   +------+  | change
                  |                                                     |
                  |                       MDIO bus                      |
                  +-----------------------------------------------------+

The CMN (Common) PLL, NSSCC (Networking Sub System Clock Controller) and GCC (Global
Clock Controller) blocks are in the SoC and act as clock providers.
(CMN（公共）PLL、NSSCC（网络子系统时钟控制器）和 GCC（全局时钟控制器）模块位于 SoC 中，充当时钟提供者。)

The UNIPHY block is in the SoC and provides the PCS (Physical Coding Sublayer) and
XPCS (10-Gigabit Physical Coding Sublayer) functions to support different interface
modes between the PPE MAC and the external PHY.
(UNIPHY 模块位于 SoC 中，提供 PCS（物理编码子层）和 XPCS（万兆物理编码子层）功能，以支持 PPE MAC 与外部
PHY 之间不同的接口模式。)

This documentation focuses on the descriptions of PPE engine and the PPE driver.
(本文档重点介绍 PPE 引擎和 PPE 驱动。)

The Ethernet functionality in the PPE (Packet Process Engine) is comprised of three
components: the switch core, port wrapper and Ethernet DMA.
(PPE（数据包处理引擎）中的以太网功能由三个部分组成：交换核心 (switch core)、端口包装器 (port wrapper) 和
以太网 DMA。)

The Switch core in the IPQ9574 PPE has maximum of 6 front panel ports and two FIFO
interfaces. One of the two FIFO interfaces is used for Ethernet port to host CPU
communication using Ethernet DMA. The other one is used to communicate to the EIP
engine which is used for IPsec offload. On the IPQ9574, the PPE includes 6 GMAC/XGMACs
that can be connected with external Ethernet PHY. Switch core also includes BM (Buffer
Management), QM (Queue Management) and SCH (Scheduler) modules for supporting the
packet processing.
(IPQ9574 PPE 中的交换核心最多有 6 个前面板端口和两个 FIFO 接口。两个 FIFO 接口中的一个用于借助以太网 DMA
进行以太网端口与主机 CPU 之间的通信；另一个用于与 EIP 引擎通信，EIP 引擎用于 IPsec 卸载。在 IPQ9574 上，
PPE 包含 6 个可与外部以太网 PHY 连接的 GMAC/XGMAC。交换核心还包含 BM（缓冲管理）、QM（队列管理）和 SCH
（调度器）模块，以支持数据包处理。)

The port wrapper provides connections from the 6 GMAC/XGMACS to UNIPHY (PCS) supporting
various modes such as SGMII/QSGMII/PSGMII/USXGMII/10G-BASER. There are 3 UNIPHY (PCS)
instances supported on the IPQ9574.
(端口包装器提供从 6 个 GMAC/XGMAC 到 UNIPHY（PCS）的连接，支持 SGMII/QSGMII/PSGMII/USXGMII/10G-BASER
等各种模式。IPQ9574 支持 3 个 UNIPHY（PCS）实例。)

Ethernet DMA is used to transmit and receive packets between the Ethernet subsystem
and ARM host CPU.
(以太网 DMA 用于在以太网子系统与 ARM 主机 CPU 之间收发数据包。)

The following lists the main blocks in the PPE engine which will be driven by this
PPE driver:
(下面列出了将由该 PPE 驱动驱动的 PPE 引擎中的主要模块：)

- BM
    BM is the hardware buffer manager for the PPE switch ports.
    (BM 是 PPE 交换机端口的硬件缓冲管理器。)
- QM
    Queue Manager for managing the egress hardware queues of the PPE switch ports.
    (队列管理器，用于管理 PPE 交换机端口的出方向硬件队列。)
- SCH
    The scheduler which manages the hardware traffic scheduling for the PPE switch ports.
    (调度器，负责管理 PPE 交换机端口的硬件流量调度。)
- L2
    The L2 block performs the packet bridging in the switch core. The bridge domain is
    represented by the VSI (Virtual Switch Instance) domain in PPE. FDB learning can be
    enabled based on the VSI domain and bridge forwarding occurs within the VSI domain.
    (L2 模块在交换核心中执行数据包桥接。桥接域在 PPE 中由 VSI（虚拟交换实例）域表示。可以基于 VSI 域启用 FDB
    学习，桥接转发也发生在 VSI 域之内。)
- MAC
    The PPE in the IPQ9574 supports up to six MACs (MAC0 to MAC5) which are corresponding
    to six switch ports (port1 to port6). The MAC block is connected with external PHY
    through the UNIPHY PCS block. Each MAC block includes the GMAC and XGMAC blocks and
    the switch port can select to use GMAC or XMAC through a MUX selection according to
    the external PHY's capability.
    (IPQ9574 的 PPE 最多支持 6 个 MAC（MAC0 到 MAC5），对应 6 个交换端口（port1 到 port6）。MAC 模块通过
    UNIPHY PCS 模块与外部 PHY 连接。每个 MAC 模块包含 GMAC 和 XGMAC 子块，交换端口可根据外部 PHY 的能力，通
    过 MUX 选择使用 GMAC 或 XMAC。)
- EDMA (Ethernet DMA)
    The Ethernet DMA is used to transmit and receive Ethernet packets between the PPE
    ports and the ARM cores.
    (以太网 DMA 用于在 PPE 端口与 ARM 核之间收发以太网数据包。)

The received packet on a PPE MAC port can be forwarded to another PPE MAC port. It can
be also forwarded to internal switch port0 so that the packet can be delivered to the
ARM cores using the Ethernet DMA (EDMA) engine. The Ethernet DMA driver will deliver the
packet to the corresponding 'netdevice' interface.
(在某个 PPE MAC 端口上收到的数据包可以转发到另一个 PPE MAC 端口，也可以转发到内部交换端口 port0，从而通过
以太网 DMA（EDMA）引擎将数据包投递到 ARM 核。以太网 DMA 驱动会把数据包投递到相应的 'netdevice' 接口。)

The software instantiations of the PPE MAC (netdevice), PCS and external PHYs interact
with the Linux PHYLINK framework to manage the connectivity between the PPE ports and
the connected PHYs, and the port link states. This is also illustrated in above diagram.
(PPE MAC（netdevice）、PCS 和外部 PHY 的软件实例与 Linux PHYLINK 框架交互，以管理 PPE 端口与所连接 PHY
之间的连通性以及端口链路状态。上面的框图也展示了这一点。)


PPE Driver Overview
(PPE 驱动概述)
===================
PPE driver is Ethernet driver for the Qualcomm IPQ SoC. It is a single platform driver
which includes the PPE part and Ethernet DMA part. The PPE part initializes and drives the
various blocks in PPE switch core such as BM/QM/L2 blocks and the PPE MACs. The EDMA part
drives the Ethernet DMA for packet transfer between PPE ports and ARM cores, and enables
the netdevice driver for the PPE ports.
(PPE 驱动是 Qualcomm IPQ SoC 的以太网驱动。它是一个单一的平台驱动，包含 PPE 部分和以太网 DMA 部分。PPE 部
分初始化并驱动 PPE 交换核心中的各种模块（如 BM/QM/L2 模块）以及 PPE MAC。EDMA 部分驱动以太网 DMA，负责
PPE 端口与 ARM 核之间的数据包传输，并为 PPE 端口启用 netdevice 驱动。)

The PPE driver files in drivers/net/ethernet/qualcomm/ppe/ are listed as below:
(drivers/net/ethernet/qualcomm/ppe/ 中的 PPE 驱动文件如下所示：)

- Makefile
- ppe.c
- ppe.h
- ppe_config.c
- ppe_config.h
- ppe_debugfs.c
- ppe_debugfs.h
- ppe_regs.h

The ppe.c file contains the main PPE platform driver and undertakes the initialization of
PPE switch core blocks such as QM, BM and L2. The configuration APIs for these hardware
blocks are provided in the ppe_config.c file.
(ppe.c 文件包含主 PPE 平台驱动，承担 QM、BM 和 L2 等 PPE 交换核心模块的初始化。这些硬件模块的配置 API 在
ppe_config.c 文件中提供。)

The ppe.h defines the PPE device data structure which will be used by PPE driver functions.
(ppe.h 定义 PPE 设备数据结构，PPE 驱动函数将使用它。)

The ppe_debugfs.c enables the PPE statistics counters such as PPE port Rx and Tx counters,
CPU code counters and queue counters.
(ppe_debugfs.c 提供 PPE 统计计数器，如 PPE 端口 Rx/Tx 计数器、CPU code 计数器和队列计数器。)


PPE Driver Supported SoCs
(PPE 驱动支持的 SoC)
=========================

The PPE driver supports the following IPQ SoC:
(PPE 驱动支持以下 IPQ SoC：)

- IPQ9574
  (IPQ9574)


Enabling the Driver
(启用该驱动)
===================

The driver is located in the menu structure at::
(该驱动位于如下菜单结构中：)

  -> Device Drivers
    -> Network device support (NETDEVICES [=y])
      -> Ethernet driver support
        -> Qualcomm devices
          -> Qualcomm Technologies, Inc. PPE Ethernet support

If the driver is built as a module, the module will be called qcom-ppe.
(如果驱动被构建为模块，该模块将被称为 qcom-ppe。)

The PPE driver functionally depends on the CMN PLL and NSSCC clock controller drivers.
Please make sure the dependent modules are installed before installing the PPE driver
module.
(PPE 驱动在功能上依赖 CMN PLL 和 NSSCC 时钟控制器驱动。请在安装 PPE 驱动模块之前，确保依赖模块已经安装。)


Debugging
(调试)
=========

The PPE hardware counters can be accessed using debugfs interface from the
``/sys/kernel/debug/ppe/`` directory.
(可以使用 debugfs 接口从 ``/sys/kernel/debug/ppe/`` 目录访问 PPE 硬件计数器。)
