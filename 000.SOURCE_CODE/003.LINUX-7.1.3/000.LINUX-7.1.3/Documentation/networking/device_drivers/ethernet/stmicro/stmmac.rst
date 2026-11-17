.. SPDX-License-Identifier: GPL-2.0+

==============================================================
Linux Driver for the Synopsys(R) Ethernet Controllers "stmmac"
==============================================================

(Synopsys(R) 以太网控制器 "stmmac" 的 Linux 驱动)

Authors: Giuseppe Cavallaro <peppe.cavallaro@st.com>,
Alexandre Torgue <alexandre.torgue@st.com>, Jose Abreu <joabreu@synopsys.com>
(作者：Giuseppe Cavallaro <peppe.cavallaro@st.com>、Alexandre Torgue
<alexandre.torgue@st.com>、Jose Abreu <joabreu@synopsys.com>)

Contents
(目录)
========

- In This Release
  (本版本简介)
- Feature List
  (特性列表)
- Kernel Configuration
  (内核配置)
- Command Line Parameters
  (命令行参数)
- Driver Information and Notes
  (驱动信息与说明)
- Debug Information
  (调试信息)
- Support
  (支持)

In This Release
(本版本简介)
===============

This file describes the stmmac Linux Driver for all the Synopsys(R) Ethernet
Controllers.
(本文件描述适用于所有 Synopsys(R) 以太网控制器的 stmmac Linux 驱动。)

Currently, this network device driver is for all STi embedded MAC/GMAC
(i.e. 7xxx/5xxx SoCs), SPEAr (arm), Loongson1B (mips) and XILINX XC2V3000
FF1152AMT0221 D1215994A VIRTEX FPGA board. The Synopsys Ethernet QoS 5.0 IPK
is also supported.
(目前，该网络设备驱动适用于所有 STi 嵌入式 MAC/GMAC (即 7xxx/5xxx SoC)、SPEAr
(arm)、Loongson1B (mips) 以及 XILINX XC2V3000 FF1152AMT0221 D1215994A VIRTEX
FPGA 板。此外也支持 Synopsys Ethernet QoS 5.0 IPK。)

DesignWare(R) Cores Ethernet MAC 10/100/1000 Universal version 3.70a
(and older) and DesignWare(R) Cores Ethernet Quality-of-Service version 4.0
(and upper) have been used for developing this driver as well as
DesignWare(R) Cores XGMAC - 10G Ethernet MAC and DesignWare(R) Cores
Enterprise MAC - 100G Ethernet MAC.
(本驱动的开发基于 DesignWare(R) Cores Ethernet MAC 10/100/1000 Universal 3.70a
(及更早) 版本和 DesignWare(R) Cores Ethernet Quality-of-Service 4.0 (及更高)
版本，同时也基于 DesignWare(R) Cores XGMAC - 10G Ethernet MAC 与
DesignWare(R) Cores Enterprise MAC - 100G Ethernet MAC。)

This driver supports both the platform bus and PCI.
(此驱动同时支持平台总线和 PCI。)

This driver includes support for the following Synopsys(R) DesignWare(R)
Cores Ethernet Controllers and corresponding minimum and maximum versions:
(此驱动支持以下 Synopsys(R) DesignWare(R) Cores 以太网控制器及对应的最低和最高
版本：)

+-------------------------------+--------------+--------------+--------------+
| Controller Name               | Min. Version | Max. Version | Abbrev. Name |
| (控制器名称)                  | (最低版本)   | (最高版本)   | (缩写名)     |
+===============================+==============+==============+==============+
| Ethernet MAC Universal        | N/A          | 3.73a        | GMAC         |
| (通用以太网 MAC)              |              |              |              |
+-------------------------------+--------------+--------------+--------------+
| Ethernet Quality-of-Service   | 4.00a        | N/A          | GMAC4+       |
| (以太网服务质量)              |              |              |              |
+-------------------------------+--------------+--------------+--------------+
| XGMAC - 10G Ethernet MAC      | 2.10a        | N/A          | XGMAC2+      |
| (XGMAC - 10G 以太网 MAC)      |              |              |              |
+-------------------------------+--------------+--------------+--------------+
| XLGMAC - 100G Ethernet MAC    | 2.00a        | N/A          | XLGMAC2+     |
| (XLGMAC - 100G 以太网 MAC)    |              |              |              |
+-------------------------------+--------------+--------------+--------------+

For questions related to hardware requirements, refer to the documentation
supplied with your Ethernet adapter. All hardware requirements listed apply
to use with Linux.
(有关硬件要求的问题，请参考随你的以太网适配器提供的文档。所列的所有硬件要求均
适用于 Linux。)

Feature List
(特性列表)
============

The following features are available in this driver:
(此驱动提供以下特性：)

 - GMII/MII/RGMII/SGMII/RMII/XGMII/XLGMII Interface
   (GMII/MII/RGMII/SGMII/RMII/XGMII/XLGMII 接口)
 - Half-Duplex / Full-Duplex Operation
   (半双工 / 全双工操作)
 - Energy Efficient Ethernet (EEE)
   (节能以太网 (EEE))
 - IEEE 802.3x PAUSE Packets (Flow Control)
   (IEEE 802.3x PAUSE 数据包 (流控制))
 - RMON/MIB Counters
   (RMON/MIB 计数器)
 - IEEE 1588 Timestamping (PTP)
   (IEEE 1588 时间戳 (PTP))
 - Pulse-Per-Second Output (PPS)
   (每秒脉冲输出 (PPS))
 - MDIO Clause 22 / Clause 45 Interface
   (MDIO Clause 22 / Clause 45 接口)
 - MAC Loopback
   (MAC 回环)
 - ARP Offloading
   (ARP 卸载)
 - Automatic CRC / PAD Insertion and Checking
   (自动 CRC / PAD 插入与检查)
 - Checksum Offload for Received and Transmitted Packets
   (接收与发送数据包的校验和卸载)
 - Standard or Jumbo Ethernet Packets
   (标准或巨型以太网数据包)
 - Source Address Insertion / Replacement
   (源地址插入 / 替换)
 - VLAN TAG Insertion / Replacement / Deletion / Filtering (HASH and PERFECT)
   (VLAN TAG 插入 / 替换 / 删除 / 过滤 (HASH 和 PERFECT))
 - Programmable TX and RX Watchdog and Coalesce Settings
   (可编程的 TX 和 RX 看门狗与合并 (coalesce) 设置)
 - Destination Address Filtering (PERFECT)
   (目的地址过滤 (PERFECT))
 - HASH Filtering (Multicast)
   (HASH 过滤 (组播))
 - Layer 3 / Layer 4 Filtering
   (第 3 层 / 第 4 层过滤)
 - Remote Wake-Up Detection
   (远程唤醒检测)
 - Receive Side Scaling (RSS)
   (接收侧扩展 (RSS))
 - Frame Preemption for TX and RX
   (TX 和 RX 帧抢占)
 - Programmable Burst Length, Threshold, Queue Size
   (可编程的突发长度、阈值、队列大小)
 - Multiple Queues (up to 8)
   (多队列 (最多 8 个))
 - Multiple Scheduling Algorithms (TX: WRR, DWRR, WFQ, SP, CBS, EST, TBS;
   RX: WRR, SP)
   (多种调度算法 (TX: WRR、DWRR、WFQ、SP、CBS、EST、TBS；RX: WRR、SP))
 - Flexible RX Parser
   (灵活的 RX 解析器)
 - TCP / UDP Segmentation Offload (TSO, USO)
   (TCP / UDP 分段卸载 (TSO、USO))
 - Split Header (SPH)
   (分离头部 (SPH))
 - Safety Features (ECC Protection, Data Parity Protection)
   (安全特性 (ECC 保护、数据奇偶校验保护))
 - Selftests using Ethtool
   (使用 Ethtool 的自测)

Kernel Configuration
(内核配置)
====================

The kernel configuration option is ``CONFIG_STMMAC_ETH``:
(内核配置选项是 ``CONFIG_STMMAC_ETH`` ：)

 - ``CONFIG_STMMAC_PLATFORM``: is to enable the platform driver.
   (``CONFIG_STMMAC_PLATFORM`` ：用于启用平台驱动。)
 - ``CONFIG_STMMAC_PCI``: is to enable the pci driver.
   (``CONFIG_STMMAC_PCI`` ：用于启用 PCI 驱动。)

Command Line Parameters
(命令行参数)
=======================

If the driver is built as a module the following optional parameters are used
by entering them on the command line with the modprobe command using this
syntax (e.g. for PCI module)::
(如果驱动被构建为模块，可使用以下可选参数：在命令行中用 modprobe 命令按此语法输入
它们 (以 PCI 模块为例)：)

    modprobe stmmac_pci [<option>=<VAL1>,<VAL2>,...]

Driver parameters can be also passed in command line by using::
(驱动参数也可以在命令行中传入，方式如下：)

    stmmaceth=watchdog:100,chain_mode=1

The default value for each parameter is generally the recommended setting,
unless otherwise noted.
(每个参数的默认值通常就是推荐设置，除非另有说明。)

watchdog
(watchdog (看门狗))
--------
:Valid Range: 5000-None
:Default Value: 5000
(有效范围：5000-无限制；默认值：5000)

This parameter overrides the transmit timeout in milliseconds.
(此参数覆盖以毫秒计的发送超时。)

debug
(debug (调试))
-----
:Valid Range: 0-16 (0=none,...,16=all)
:Default Value: 0
(有效范围：0-16 (0=无，...，16=全部)；默认值：0)

This parameter adjusts the level of debug messages displayed in the system
logs.
(此参数调整系统日志中显示的调试消息级别。)

phyaddr
(phyaddr (PHY 地址))
-------
:Valid Range: 0-31
:Default Value: -1
(有效范围：0-31；默认值：-1)

This parameter overrides the physical address of the PHY device.
(此参数覆盖 PHY 设备的物理地址。)

flow_ctrl
(flow_ctrl (流控制))
---------
:Valid Range: 0-3 (0=off,1=rx,2=tx,3=rx/tx)
:Default Value: 3
(有效范围：0-3 (0=关，1=rx，2=tx，3=rx/tx)；默认值：3)

This parameter changes the default Flow Control ability.
(此参数更改默认的流控制能力。)

pause
(pause (暂停时间))
-----
:Valid Range: 0-65535
:Default Value: 65535
(有效范围：0-65535；默认值：65535)

This parameter changes the default Flow Control Pause time.
(此参数更改默认的流控制暂停时间。)

tc
(tc (阈值控制))
--
:Valid Range: 64-256
:Default Value: 64
(有效范围：64-256；默认值：64)

This parameter changes the default HW FIFO Threshold control value.
(此参数更改默认的硬件 FIFO 阈值控制值。)

buf_sz
(buf_sz (缓冲区大小))
------
:Valid Range: 1536-16384
:Default Value: 1536
(有效范围：1536-16384；默认值：1536)

This parameter changes the default RX DMA packet buffer size.
(此参数更改默认的 RX DMA 数据包缓冲区大小。)

eee_timer
(eee_timer (EEE 定时器))
---------
:Valid Range: 0-None
:Default Value: 1000
(有效范围：0-无限制；默认值：1000)

This parameter changes the default LPI TX Expiration time in milliseconds.
(此参数更改默认的 LPI TX 过期时间 (以毫秒计)。)

chain_mode
(chain_mode (链模式))
----------
:Valid Range: 0-1 (0=off,1=on)
:Default Value: 0
(有效范围：0-1 (0=关，1=开)；默认值：0)

This parameter changes the default mode of operation from Ring Mode to
Chain Mode.
(此参数将默认的工作模式从环 (Ring) 模式更改为链 (Chain) 模式。)

Driver Information and Notes
(驱动信息与说明)
============================

Transmit Process
(发送流程)
----------------

The xmit method is invoked when the kernel needs to transmit a packet; it sets
the descriptors in the ring and informs the DMA engine that there is a packet
ready to be transmitted.
(当内核需要发送一个数据包时，会调用 xmit 方法；它在环中设置描述符，并通知 DMA 引擎
有一个数据包已准备好发送。)

By default, the driver sets the ``NETIF_F_SG`` bit in the features field of
the ``net_device`` structure, enabling the scatter-gather feature. This is
true on chips and configurations where the checksum can be done in hardware.
(默认情况下，驱动会在 ``net_device`` 结构的 features 字段中设置 ``NETIF_F_SG``
位，从而启用分散-聚集 (scatter-gather) 特性。在校验和可以由硬件完成的芯片和配置
上，情况正是如此。)

Once the controller has finished transmitting the packet, timer will be
scheduled to release the transmit resources.
(控制器完成数据包发送后，会调度一个定时器来释放发送资源。)

Receive Process
(接收流程)
---------------

When one or more packets are received, an interrupt happens. The interrupts
are not queued, so the driver has to scan all the descriptors in the ring
during the receive process.
(当收到一个或多个数据包时会产生中断。中断不会排队，因此驱动必须在接收过程中扫描环
中的所有描述符。)

This is based on NAPI, so the interrupt handler signals only if there is work
to be done, and it exits. Then the poll method will be scheduled at some
future point.
(这基于 NAPI 实现，因此只有当有待处理的工作时，中断处理程序才会发出信号然后退出。
随后 poll 方法会在未来的某个时间点被调度执行。)

The incoming packets are stored, by the DMA, in a list of pre-allocated socket
buffers in order to avoid the memcpy (zero-copy).
(为了避免 memcpy (零拷贝)，DMA 会将到达的数据包存储在预分配的套接字缓冲区列表中。)

Interrupt Mitigation
(中断缓解)
--------------------

The driver is able to mitigate the number of its DMA interrupts using NAPI for
the reception on chips older than the 3.50. New chips have an HW RX Watchdog
used for this mitigation.
(对于 3.50 之前的芯片，驱动能够在接收方向上使用 NAPI 来减少 DMA 中断次数。新芯片
则有一个用于此缓解目的的硬件 RX 看门狗。)

Mitigation parameters can be tuned by ethtool.
(缓解参数可以通过 ethtool 调整。)

WoL
(WoL (网络唤醒))
---

Wake up on Lan feature through Magic and Unicast frames are supported for the
GMAC, GMAC4/5 and XGMAC core.
(GMAC、GMAC4/5 和 XGMAC 核心支持通过 Magic 帧和单播帧实现的网络唤醒 (Wake up on
Lan) 特性。)

DMA Descriptors
(DMA 描述符)
---------------

Driver handles both normal and alternate descriptors. The latter has been only
tested on DesignWare(R) Cores Ethernet MAC Universal version 3.41a and later.
(驱动同时处理普通描述符和备用 (alternate) 描述符。后者仅在 DesignWare(R) Cores
Ethernet MAC Universal 3.41a 及更高版本上经过测试。)

stmmac supports DMA descriptor to operate both in dual buffer (RING) and
linked-list(CHAINED) mode. In RING each descriptor points to two data buffer
pointers whereas in CHAINED mode they point to only one data buffer pointer.
RING mode is the default.
(stmmac 的 DMA 描述符既可以在双缓冲 (RING) 模式下工作，也可以在链表 (CHAINED) 模式
下工作。在 RING 模式中，每个描述符指向两个数据缓冲区指针；而在 CHAINED 模式中，它们
只指向一个数据缓冲区指针。默认为 RING 模式。)

In CHAINED mode each descriptor will have pointer to next descriptor in the
list, hence creating the explicit chaining in the descriptor itself, whereas
such explicit chaining is not possible in RING mode.
(在 CHAINED 模式中，每个描述符都带有指向链表中下一个描述符的指针，从而在描述符自身
中形成显式链接；而这种显式链接在 RING 模式中是无法实现的。)

Extended Descriptors
(扩展描述符)
--------------------

The extended descriptors give us information about the Ethernet payload when
it is carrying PTP packets or TCP/UDP/ICMP over IP. These are not available on
GMAC Synopsys(R) chips older than the 3.50. At probe time the driver will
decide if these can be actually used. This support also is mandatory for PTPv2
because the extra descriptors are used for saving the hardware timestamps and
Extended Status.
(扩展描述符提供以太网载荷承载 PTP 数据包或 IP 之上的 TCP/UDP/ICMP 时的相关信息。
3.50 之前的 GMAC Synopsys(R) 芯片不提供这些描述符。在探测 (probe) 时，驱动会判断
它们是否真的可用。PTPv2 也必须依赖此支持，因为这些额外的描述符用于保存硬件时间戳和
扩展状态 (Extended Status)。)

Ethtool Support
(Ethtool 支持)
---------------

Ethtool is supported. For example, driver statistics (including RMON),
internal errors can be taken using::
(Ethtool 受支持。例如，可以使用以下命令获取驱动统计信息 (包括 RMON) 和内部
错误：)

    ethtool -S ethX

Ethtool selftests are also supported. This allows to do some early sanity
checks to the HW using MAC and PHY loopback mechanisms::
(Ethtool 自测同样受支持。它允许使用 MAC 和 PHY 回环机制对硬件做一些早期的完整性
检查：)

    ethtool -t ethX

Jumbo and Segmentation Offloading
(Jumbo 与分段卸载)
---------------------------------

Jumbo frames are supported and tested for the GMAC. The GSO has been also
added but it's performed in software. LRO is not supported.
(GMAC 支持并已测试巨型帧 (Jumbo frames)。此外还加入了 GSO，但它由软件执行。不支持
LRO。)

TSO Support
(TSO 支持)
-----------

TSO (TCP Segmentation Offload) feature is supported by GMAC > 4.x and XGMAC
chip family. When a packet is sent through TCP protocol, the TCP stack ensures
that the SKB provided to the low level driver (stmmac in our case) matches
with the maximum frame len (IP header + TCP header + payload <= 1500 bytes
(for MTU set to 1500)). It means that if an application using TCP want to send
a packet which will have a length (after adding headers) > 1514 the packet
will be split in several TCP packets: The data payload is split and headers
(TCP/IP ..) are added. It is done by software.
(GMAC 4.x 以上和 XGMAC 芯片家族支持 TSO (TCP 分段卸载) 特性。当通过 TCP 协议发送
数据包时，TCP 栈会确保提供给底层驱动 (在我们这里是 stmmac) 的 SKB 符合最大帧长度
(IP 头 + TCP 头 + 载荷 <= 1500 字节 (MTU 设置为 1500 时))。这意味着，如果使用 TCP
的应用程序要发送一个 (加上头部后) 长度 > 1514 的数据包，该数据包将被拆分为多个 TCP
数据包：数据载荷被拆分，并添加 (TCP/IP 等) 头部。这是由软件完成的。)

When TSO is enabled, the TCP stack doesn't care about the maximum frame length
and provide SKB packet to stmmac as it is. The GMAC IP will have to perform
the segmentation by it self to match with maximum frame length.
(启用 TSO 后，TCP 栈不再关心最大帧长度，而是将 SKB 数据包原样提供给 stmmac。GMAC
IP 必须自行执行分段，以符合最大帧长度。)

This feature can be enabled in device tree through ``snps,tso`` entry.
(此特性可以在设备树中通过 ``snps,tso`` 条目启用。)

Energy Efficient Ethernet
(节能以太网 (EEE))
-------------------------

Energy Efficient Ethernet (EEE) enables IEEE 802.3 MAC sublayer along with a
family of Physical layer to operate in the Low Power Idle (LPI) mode. The EEE
mode supports the IEEE 802.3 MAC operation at 100Mbps, 1000Mbps and 1Gbps.
(节能以太网 (EEE) 使 IEEE 802.3 MAC 子层与一系列物理层能够在低功耗空闲 (LPI) 模式
下运行。EEE 模式支持 100Mbps、1000Mbps 和 1Gbps 下的 IEEE 802.3 MAC 操作。)

The LPI mode allows power saving by switching off parts of the communication
device functionality when there is no data to be transmitted & received.
The system on both the side of the link can disable some functionalities and
save power during the period of low-link utilization. The MAC controls whether
the system should enter or exit the LPI mode and communicate this to PHY.
(LPI 模式通过在没有数据需要发送和接收时关闭通信设备的部分功能来实现节能。链路两侧
的系统都可以在链路利用率较低期间禁用某些功能以节省电力。MAC 控制系统应进入还是退出
LPI 模式，并将此情况告知 PHY。)

As soon as the interface is opened, the driver verifies if the EEE can be
supported. This is done by looking at both the DMA HW capability register and
the PHY devices MCD registers.
(接口一打开，驱动就会检查是否可以支持 EEE。这是通过同时查看 DMA 硬件能力寄存器和
PHY 设备的 MCD 寄存器来完成的。)

To enter in TX LPI mode the driver needs to have a software timer that enable
and disable the LPI mode when there is nothing to be transmitted.
(要进入 TX LPI 模式，驱动需要一个软件定时器，在没有数据要发送时启用和禁用 LPI
模式。)

Precision Time Protocol (PTP)
(精确时间协议 (PTP))
-----------------------------

The driver supports the IEEE 1588-2002, Precision Time Protocol (PTP), which
enables precise synchronization of clocks in measurement and control systems
implemented with technologies such as network communication.
(驱动支持 IEEE 1588-2002 精确时间协议 (PTP)，它可以为采用网络通信等技术实现的测量
与控制系统提供精确的时钟同步。)

In addition to the basic timestamp features mentioned in IEEE 1588-2002
Timestamps, new GMAC cores support the advanced timestamp features.
IEEE 1588-2008 can be enabled when configuring the Kernel.
(除了 IEEE 1588-2002 时间戳中提到的基本时间戳特性之外，新的 GMAC 核心还支持高级
时间戳特性。IEEE 1588-2008 可以在配置内核时启用。)

SGMII/RGMII Support
(SGMII/RGMII 支持)
-------------------

New GMAC devices provide own way to manage RGMII/SGMII. This information is
available at run-time by looking at the HW capability register. This means
that the stmmac can manage auto-negotiation and link status w/o using the
PHYLIB stuff. In fact, the HW provides a subset of extended registers to
restart the ANE, verify Full/Half duplex mode and Speed. Thanks to these
registers, it is possible to look at the Auto-negotiated Link Parter Ability.
(新的 GMAC 设备提供了自己的方式来管理 RGMII/SGMII。该信息可在运行时通过查看硬件
能力寄存器获得。这意味着 stmmac 无需借助 PHYLIB 就能管理自协商和链路状态。实际上，
硬件提供了一组扩展寄存器，用于重启 ANE、校验全/半双工模式和速率。借助这些寄存器，
可以查看自协商的链路伙伴能力 (Auto-negotiated Link Parter Ability)。)

Physical
(物理层)
--------

The driver is compatible with Physical Abstraction Layer to be connected with
PHY and GPHY devices.
(该驱动与物理抽象层 (Physical Abstraction Layer) 兼容，可以连接 PHY 和 GPHY
设备。)

Platform Information
(平台信息)
--------------------

Several information can be passed through the platform and device-tree.
(可以通过 platform 和设备树传递多种信息。)

::

    struct plat_stmmacenet_data {

1) Bus identifier::
(总线标识符：)

        int bus_id;

2) PHY Physical Address. If set to -1 the driver will pick the first PHY it
finds::
(PHY 物理地址。如果设置为 -1，驱动将选择它找到的第一个 PHY：)

        int phy_addr;

3) PHY Device Interface::
(PHY 设备接口：)

        int interface;

4) Specific platform fields for the MDIO bus::
(MDIO 总线的特定平台字段：)

        struct stmmac_mdio_bus_data *mdio_bus_data;

5) Internal DMA parameters::
(内部 DMA 参数：)

        struct stmmac_dma_cfg *dma_cfg;

6) Fixed CSR Clock Range selection::
(固定 CSR 时钟范围选择：)

        int clk_csr;

7) HW uses the GMAC core::
(硬件使用 GMAC 核心：)

        int has_gmac;

8) If set the MAC will use Enhanced Descriptors::
(如果设置，MAC 将使用增强描述符：)

        int enh_desc;

9) Core is able to perform TX Checksum and/or RX Checksum in HW::
(核心能够在硬件中执行 TX 校验和和/或 RX 校验和：)

        int tx_coe;
        int rx_coe;

11) Some HWs are not able to perform the csum in HW for over-sized frames due
to limited buffer sizes. Setting this flag the csum will be done in SW on
JUMBO frames::
(由于缓冲区大小有限，一些硬件无法在硬件中对超长帧执行 csum。设置此标志后，JUMBO
帧的 csum 将在软件中完成：)

        int bugged_jumbo;

12) Core has the embedded power module::
(核心具有嵌入式电源模块：)

        int pmt;

13) Force DMA to use the Store and Forward mode or Threshold mode::
(强制 DMA 使用存储转发 (Store and Forward) 模式或阈值 (Threshold) 模式：)

        int force_sf_dma_mode;
        int force_thresh_dma_mode;

15) Force to disable the RX Watchdog feature and switch to NAPI mode::
(强制禁用 RX 看门狗特性并切换到 NAPI 模式：)

        int riwt_off;

16) Limit the maximum operating speed and MTU::
(限制最大运行速率和 MTU：)

        int max_speed;
        int maxmtu;

18) Number of Multicast/Unicast filters::
(组播/单播过滤器数量：)

        int multicast_filter_bins;
        int unicast_filter_entries;

20) Limit the maximum TX and RX FIFO size::
(限制最大 TX 和 RX FIFO 大小：)

        int tx_fifo_size;
        int rx_fifo_size;

21) Use the specified number of TX and RX Queues::
(使用指定数量的 TX 和 RX 队列：)

        u32 rx_queues_to_use;
        u32 tx_queues_to_use;

22) Use the specified TX and RX scheduling algorithm::
(使用指定的 TX 和 RX 调度算法：)

        u8 rx_sched_algorithm;
        u8 tx_sched_algorithm;

23) Internal TX and RX Queue parameters::
(内部 TX 和 RX 队列参数：)

        struct stmmac_rxq_cfg rx_queues_cfg[MTL_MAX_RX_QUEUES];
        struct stmmac_txq_cfg tx_queues_cfg[MTL_MAX_TX_QUEUES];

24) This callback is used for modifying some syscfg registers (on ST SoCs)
according to the link speed negotiated by the physical layer::
(此回调用于根据物理层协商的链路速率修改某些 syscfg 寄存器 (在 ST SoC 上)：)

        void (*fix_mac_speed)(void *priv, unsigned int speed);

25) Callbacks used for calling a custom initialization; This is sometimes
necessary on some platforms (e.g. ST boxes) where the HW needs to have set
some PIO lines or system cfg registers. init/exit callbacks should not use
or modify platform data::
(用于调用自定义初始化的回调；在某些平台 (例如 ST 盒子) 上有时需要这样做，因为
硬件需要预先设置某些 PIO 线路或系统配置寄存器。init/exit 回调不应使用或修改
平台数据：)

        int (*init)(struct platform_device *pdev, void *priv);
        void (*exit)(struct platform_device *pdev, void *priv);

26) Perform HW setup of the bus. For example, on some ST platforms this field
is used to configure the AMBA bridge to generate more efficient STBus traffic::
(执行总线的硬件设置。例如，在一些 ST 平台上，此字段用于配置 AMBA 桥以生成更
高效的 STBus 流量：)

        struct mac_device_info *(*setup)(void *priv);
        void *bsp_priv;

27) Internal clocks and rates::
(内部时钟与频率：)

        struct clk *stmmac_clk;
        struct clk *pclk;
        struct clk *clk_ptp_ref;
        unsigned int clk_ptp_rate;
        unsigned int clk_ref_rate;
        s32 ptp_max_adj;

28) Main reset::
(主复位：)

        struct reset_control *stmmac_rst;

29) AXI Internal Parameters::
(AXI 内部参数：)

        struct stmmac_axi *axi;

30) HW uses GMAC>4 cores::
(硬件使用 GMAC>4 核心：)

        int has_gmac4;

31) HW is sun8i based::
(硬件基于 sun8i：)

        bool has_sun8i;

32) Enables TSO feature::
(启用 TSO 特性：)

        bool tso_en;

33) Enables Receive Side Scaling (RSS) feature::
(启用接收侧扩展 (RSS) 特性：)

        int rss_en;

34) MAC Port selection::
(MAC 端口选择：)

        int mac_port_sel_speed;

35) Enables TX LPI Clock Gating::
(启用 TX LPI 时钟门控：)

        bool en_tx_lpi_clockgating;

36) HW uses XGMAC>2.10 cores::
(硬件使用 XGMAC>2.10 核心：)

        int has_xgmac;

::

    }

For MDIO bus data, we have:
(对于 MDIO 总线数据，我们有：)

::

    struct stmmac_mdio_bus_data {

1) PHY mask passed when MDIO bus is registered::
(注册 MDIO 总线时传入的 PHY 掩码：)

        unsigned int phy_mask;

2) List of IRQs, one per PHY::
(IRQ 列表，每个 PHY 一个：)

        int *irqs;

3) If IRQs is NULL, use this for probed PHY::
(如果 IRQs 为 NULL，则对探测到的 PHY 使用此项：)

        int probed_phy_irq;

4) Set to true if PHY needs reset::
(如果 PHY 需要复位则设为 true：)

        bool needs_reset;

::

    }

For DMA engine configuration, we have:
(对于 DMA 引擎配置，我们有：)

::

    struct stmmac_dma_cfg {

1) Programmable Burst Length (TX and RX)::
(可编程突发长度 (TX 和 RX)：)

        int pbl;

2) If set, DMA TX / RX will use this value rather than pbl::
(如果设置，DMA TX / RX 将使用此值而非 pbl：)

        int txpbl;
        int rxpbl;

3) Enable 8xPBL::
(启用 8xPBL：)

        bool pblx8;

4) Enable Fixed or Mixed burst::
(启用固定或混合突发：)

        int fixed_burst;
        int mixed_burst;

5) Enable Address Aligned Beats::
(启用地址对齐节拍：)

        bool aal;

6) Enable Enhanced Addressing (> 32 bits)::
(启用增强寻址 (> 32 位)：)

        bool eame;

::

    }

For DMA AXI parameters, we have:
(对于 DMA AXI 参数，我们有：)

::

    struct stmmac_axi {

1) Enable AXI LPI::
(启用 AXI LPI：)

        bool axi_lpi_en;
        bool axi_xit_frm;

2) Set AXI Write / Read maximum outstanding requests::
(设置 AXI 写 / 读最大未完成请求数：)

        u32 axi_wr_osr_lmt;
        u32 axi_rd_osr_lmt;

3) Set AXI 4KB bursts::
(设置 AXI 4KB 突发：)

        bool axi_kbbe;

4) Set AXI maximum burst length map::
(设置 AXI 最大突发长度映射：)

        u32 axi_blen[AXI_BLEN];

5) Set AXI Fixed burst / mixed burst::
(设置 AXI 固定突发 / 混合突发：)

        bool axi_fb;
        bool axi_mb;

6) Set AXI rebuild incrx mode::
(设置 AXI 重建 incrx 模式：)

        bool axi_rb;

::

    }

For the RX Queues configuration, we have:
(对于 RX 队列配置，我们有：)

::

    struct stmmac_rxq_cfg {

1) Mode to use (DCB or AVB)::
(要使用的模式 (DCB 或 AVB)：)

        u8 mode_to_use;

2) DMA channel to use::
(要使用的 DMA 通道：)

        u32 chan;

3) Packet routing, if applicable::
(数据包路由 (如适用)：)

        u8 pkt_route;

4) Use priority routing, and priority to route::
(使用优先级路由，以及要路由的优先级：)

        bool use_prio;
        u32 prio;

::

    }

For the TX Queues configuration, we have:
(对于 TX 队列配置，我们有：)

::

    struct stmmac_txq_cfg {

1) Queue weight in scheduler::
(调度器中的队列权重：)

        u32 weight;

2) Mode to use (DCB or AVB)::
(要使用的模式 (DCB 或 AVB)：)

        u8 mode_to_use;

3) Credit Base Shaper Parameters::
(基于信用的整形器 (shaper) 参数：)

        u32 send_slope;
        u32 idle_slope;
        u32 high_credit;
        u32 low_credit;

4) Use priority scheduling, and priority::
(使用优先级调度，以及优先级：)

        bool use_prio;
        u32 prio;

::

    }

Device Tree Information
(设备树信息)
-----------------------

Please refer to the following document:
Documentation/devicetree/bindings/net/snps,dwmac.yaml
(请参考以下文档：Documentation/devicetree/bindings/net/snps,dwmac.yaml)

HW Capabilities
(HW 能力)
---------------

Note that, starting from new chips, where it is available the HW capability
register, many configurations are discovered at run-time for example to
understand if EEE, HW csum, PTP, enhanced descriptor etc are actually
available. As strategy adopted in this driver, the information from the HW
capability register can replace what has been passed from the platform.
(注意，从提供硬件能力寄存器的新芯片开始，许多配置都是在运行时发现的，例如判断
EEE、硬件校验和、PTP、增强描述符等是否真正可用。按照本驱动采用的策略，来自硬件
能力寄存器的信息可以取代从平台传递的信息。)

Debug Information
(调试信息)
=================

The driver exports many information i.e. internal statistics, debug
information, MAC and DMA registers etc.
(驱动导出许多信息，例如内部统计信息、调试信息、MAC 和 DMA 寄存器等。)

These can be read in several ways depending on the type of the information
actually needed.
(可以根据实际需要的信息类型，通过多种方式读取这些信息。)

For example a user can be use the ethtool support to get statistics: e.g.
using: ``ethtool -S ethX`` (that shows the Management counters (MMC) if
supported) or sees the MAC/DMA registers: e.g. using: ``ethtool -d ethX``
(例如，用户可以使用 ethtool 支持来获取统计信息：如使用 ``ethtool -S ethX`` (如果
受支持，将显示管理计数器 (MMC))；或查看 MAC/DMA 寄存器：如使用 ``ethtool -d
ethX`` 。)

Compiling the Kernel with ``CONFIG_DEBUG_FS`` the driver will export the
following debugfs entries:
(使用 ``CONFIG_DEBUG_FS`` 编译内核后，驱动将导出以下 debugfs 条目：)

 - ``descriptors_status``: To show the DMA TX/RX descriptor rings
   (``descriptors_status`` ：显示 DMA TX/RX 描述符环)
 - ``dma_cap``: To show the HW Capabilities
   (``dma_cap`` ：显示硬件能力)

Developer can also use the ``debug`` module parameter to get further debug
information (please see: NETIF Msg Level).
(开发者还可以使用 ``debug`` 模块参数获取进一步的调试信息 (请参见：NETIF Msg
Level)。)

Support
(支持)
=======

If an issue is identified with the released source code on a supported kernel
with a supported adapter, email the specific information related to the
issue to netdev@vger.kernel.org
(如果在受支持的内核上配合受支持的适配器使用已发布的源代码时发现了问题，请将与该
问题相关的具体信息通过电子邮件发送至 netdev@vger.kernel.org)
