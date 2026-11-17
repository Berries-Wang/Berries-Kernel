.. SPDX-License-Identifier: GPL-2.0
.. include:: <isonum.txt>

=======================================
DPAA2 Ethernet driver(DPAA2 以太网驱动)
=======================================

:Copyright: |copy| 2017-2018 NXP

(版权： 2017-2018 NXP)

This file provides documentation for the Freescale DPAA2 Ethernet driver.
(本文件提供 Freescale DPAA2 以太网驱动的文档。)

Supported Platforms(支持的平台)
===============================

This driver provides networking support for Freescale DPAA2 SoCs, e.g.
LS2080A, LS2088A, LS1088A.
(本驱动为 Freescale DPAA2 SoC 提供网络支持，例如 LS2080A、LS2088A、LS1088A。)


Architecture Overview(架构概览)
===============================

Unlike regular NICs, in the DPAA2 architecture there is no single hardware block
representing network interfaces; instead, several separate hardware resources
concur to provide the networking functionality:
(与常规网卡不同，DPAA2 架构中没有代表网络接口的单一硬件块；而是由多个独立的硬件资源共同提供网络功能：)

- network interfaces
  (网络接口)
- queues, channels
  (队列、通道)
- buffer pools
  (缓冲池)
- MAC/PHY
  (MAC/PHY)

All hardware resources are allocated and configured through the Management
Complex (MC) portals. MC abstracts most of these resources as DPAA2 objects
and exposes ABIs through which they can be configured and controlled. A few
hardware resources, like queues, do not have a corresponding MC object and
are treated as internal resources of other objects.
(所有硬件资源都通过管理复合体 (Management Complex, MC) 门户进行分配和配置。MC 将其中大部分资源抽象为 DPAA2 对象，并公开 ABI 用于配置和控制。少数硬件资源（如队列）没有对应的 MC 对象，被视为其他对象的内部资源。)

For a more detailed description of the DPAA2 architecture and its object
abstractions see
*Documentation/networking/device_drivers/ethernet/freescale/dpaa2/overview.rst*.
(关于 DPAA2 架构及其对象抽象的更详细描述，请参阅
*Documentation/networking/device_drivers/ethernet/freescale/dpaa2/overview.rst*。)

Each Linux net device is built on top of a Datapath Network Interface (DPNI)
object and uses Buffer Pools (DPBPs), I/O Portals (DPIOs) and Concentrators
(DPCONs).
(每个 Linux 网络设备都构建在一个数据路径网络接口 (DPNI) 对象之上，并使用缓冲池 (DPBP)、I/O 门户 (DPIO) 和集中器 (DPCON)。)

Configuration interface::

                 -----------------------
                | DPAA2 Ethernet Driver |
                 -----------------------
                     .      .      .
                     .      .      .
             . . . . .      .      . . . . . .
             .              .                .
             .              .                .
         ----------     ----------      -----------
        | DPBP API |   | DPNI API |    | DPCON API |
         ----------     ----------      -----------
             .              .                .             software
    =======  .  ==========  .  ============  .  ===================
             .              .                .             hardware
         ------------------------------------------
        |            MC hardware portals           |
         ------------------------------------------
             .              .                .
             .              .                .
          ------         ------            -------
         | DPBP |       | DPNI |          | DPCON |
          ------         ------            -------

(配置接口：)

The DPNIs are network interfaces without a direct one-on-one mapping to PHYs.
DPBPs represent hardware buffer pools. Packet I/O is performed in the context
of DPCON objects, using DPIO portals for managing and communicating with the
hardware resources.
(DPNI 是没有与 PHY 直接一一对应关系的网络接口。DPBP 代表硬件缓冲池。数据包 I/O 在 DPCON 对象的上下文中执行，使用 DPIO 门户来管理和与硬件资源通信。)

Datapath (I/O) interface::

         -----------------------------------------------
        |           DPAA2 Ethernet Driver               |
         -----------------------------------------------
          |          ^        ^         |            |
          |          |        |         |            |
   enqueue|   dequeue|   data |  dequeue|       seed |
    (Tx)  | (Rx, TxC)|  avail.|  request|     buffers|
          |          |  notify|         |            |
          |          |        |         |            |
          V          |        |         V            V
         -----------------------------------------------
        |                 DPIO Driver                   |
         -----------------------------------------------
          |          |        |         |            |          software
          |          |        |         |            |  ================
          |          |        |         |            |          hardware
         -----------------------------------------------
        |               I/O hardware portals            |
         -----------------------------------------------
          |          ^        ^         |            |
          |          |        |         |            |
          |          |        |         V            |
          V          |    ================           V
        ----------------------           |      -------------
 queues  ----------------------          |     | Buffer pool |
          ----------------------         |      -------------
                   =======================
                                Channel

(数据路径 (I/O) 接口：)

Datapath I/O (DPIO) portals provide enqueue and dequeue services, data
availability notifications and buffer pool management. DPIOs are shared between
all DPAA2 objects (and implicitly all DPAA2 kernel drivers) that work with data
frames, but must be affine to the CPUs for the purpose of traffic distribution.
(数据路径 I/O (DPIO) 门户提供入队和出队服务、数据可用性通知以及缓冲池管理。DPIO 在所有处理数据帧的 DPAA2 对象（隐含地也包括所有 DPAA2 内核驱动）之间共享，但为了流量分配的目的必须绑定到特定 CPU。)

Frames are transmitted and received through hardware frame queues, which can be
grouped in channels for the purpose of hardware scheduling. The Ethernet driver
enqueues TX frames on egress queues and after transmission is complete a TX
confirmation frame is sent back to the CPU.
(帧通过硬件帧队列发送和接收，为了硬件调度的目的，帧队列可以分组为通道。以太网驱动将 TX 帧入队到出站队列，传输完成后一个 TX 确认帧被送回 CPU。)

When frames are available on ingress queues, a data availability notification
is sent to the CPU; notifications are raised per channel, so even if multiple
queues in the same channel have available frames, only one notification is sent.
After a channel fires a notification, is must be explicitly rearmed.
(当入站队列上有帧可用时，会向 CPU 发送数据可用性通知；通知按通道发出，因此即使同一通道中的多个队列都有可用帧，也只发送一个通知。通道发出通知后，必须显式重新武装 (rearm)。)

Each network interface can have multiple Rx, Tx and confirmation queues affined
to CPUs, and one channel (DPCON) for each CPU that services at least one queue.
DPCONs are used to distribute ingress traffic to different CPUs via the cores'
affine DPIOs.
(每个网络接口可以有多个绑定到 CPU 的 Rx、Tx 和确认队列，并且每个至少服务一个队列的 CPU 有一个通道 (DPCON)。DPCON 用于通过各核心绑定的 DPIO 将入站流量分配到不同 CPU。)

The role of hardware buffer pools is storage of ingress frame data. Each network
interface has a privately owned buffer pool which it seeds with kernel allocated
buffers.
(硬件缓冲池的作用是存储入站帧数据。每个网络接口拥有一个私有的缓冲池，并使用内核分配的缓冲区为其填充。)


DPNIs are decoupled from PHYs; a DPNI can be connected to a PHY through a DPMAC
object or to another DPNI through an internal link, but the connection is
managed by MC and completely transparent to the Ethernet driver.
(DPNI 与 PHY 解耦；DPNI 可以通过 DPMAC 对象连接到 PHY，或通过内部链路连接到另一个 DPNI，但该连接由 MC 管理，对以太网驱动完全透明。)

::

     ---------     ---------     ---------
    | eth if1 |   | eth if2 |   | eth ifn |
     ---------     ---------     ---------
          .           .          .
          .           .          .
          .           .          .
         ---------------------------
        |   DPAA2 Ethernet Driver   |
         ---------------------------
          .           .          .
          .           .          .
          .           .          .
       ------      ------      ------            -------
      | DPNI |    | DPNI |    | DPNI |          | DPMAC |----+
       ------      ------      ------            -------     |
         |           |           |                  |        |
         |           |           |                  |      -----
          ===========             ==================      | PHY |
                                                           -----

Creating a Network Interface(创建网络接口)
==========================================

A net device is created for each DPNI object probed on the MC bus. Each DPNI has
a number of properties which determine the network interface configuration
options and associated hardware resources.
(在 MC 总线上探测到的每个 DPNI 对象都会创建一个网络设备。每个 DPNI 都具有若干属性，这些属性决定了网络接口的配置选项和相关硬件资源。)

DPNI objects (and the other DPAA2 objects needed for a network interface) can be
added to a container on the MC bus in one of two ways: statically, through a
Datapath Layout Binary file (DPL) that is parsed by MC at boot time; or created
dynamically at runtime, via the DPAA2 objects APIs.
(DPNI 对象（以及网络接口所需的其他 DPAA2 对象）可以通过两种方式之一添加到 MC 总线上的容器中：静态方式，通过 MC 在启动时解析的数据路径布局二进制文件 (DPL)；或者动态方式，在运行时通过 DPAA2 对象 API 创建。)


Features & Offloads(特性与卸载)
===============================

Hardware checksum offloading is supported for TCP and UDP over IPv4/6 frames.
The checksum offloads can be independently configured on RX and TX through
ethtool.
(支持对 IPv4/6 帧上的 TCP 和 UDP 进行硬件校验和卸载。校验和卸载可以通过 ethtool 在 RX 和 TX 上独立配置。)

Hardware offload of unicast and multicast MAC filtering is supported on the
ingress path and permanently enabled.
(入站路径支持单播和多播 MAC 过滤的硬件卸载，且始终启用。)

Scatter-gather frames are supported on both RX and TX paths. On TX, SG support
is configurable via ethtool; on RX it is always enabled.
(RX 和 TX 路径都支持分散-聚集 (scatter-gather) 帧。在 TX 方向，SG 支持可通过 ethtool 配置；在 RX 方向则始终启用。)

The DPAA2 hardware can process jumbo Ethernet frames of up to 10K bytes.
(DPAA2 硬件可以处理最大 10K 字节的巨型以太网帧。)

The Ethernet driver defines a static flow hashing scheme that distributes
traffic based on a 5-tuple key: src IP, dst IP, IP proto, L4 src port,
L4 dst port. No user configuration is supported for now.
(以太网驱动定义了一种静态流哈希方案，基于 5 元组键分配流量：源 IP、目的 IP、IP 协议、L4 源端口、L4 目的端口。目前不支持用户配置。)

Hardware specific statistics for the network interface as well as some
non-standard driver stats can be consulted through ethtool -S option.
(网络接口的硬件特定统计信息以及一些非标准驱动统计信息可以通过 ethtool -S 选项查看。)
