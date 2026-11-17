.. SPDX-License-Identifier: GPL-2.0

=====================================================
The QorIQ DPAA Ethernet Driver(QorIQ DPAA 以太网驱动)
=====================================================

Authors:
(作者：)
- Madalin Bucur <madalin.bucur@nxp.com>
- Camelia Groza <camelia.groza@nxp.com>

.. Contents

	- DPAA Ethernet Overview
	  (DPAA 以太网概述)
	- DPAA Ethernet Supported SoCs
	  (DPAA 以太网支持的 SoC)
	- Configuring DPAA Ethernet in your kernel
	  (在内核中配置 DPAA 以太网)
	- DPAA Ethernet Frame Processing
	  (DPAA 以太网帧处理)
	- DPAA Ethernet Features
	  (DPAA 以太网特性)
	- DPAA IRQ Affinity and Receive Side Scaling
	  (DPAA 中断亲和性与接收侧扩展 (RSS))
	- Debugging
	  (调试)

DPAA Ethernet Overview(DPAA 以太网概述)
=======================================

DPAA stands for Data Path Acceleration Architecture and it is a
set of networking acceleration IPs that are available on several
generations of SoCs, both on PowerPC and ARM64.
(DPAA 代表数据路径加速架构 (Data Path Acceleration Architecture)，它是一组网络加速 IP，存在于多代 SoC 之上，PowerPC 和 ARM64 平台均有。)

The Freescale DPAA architecture consists of a series of hardware blocks
that support Ethernet connectivity. The Ethernet driver depends upon the
following drivers in the Linux kernel:
(Freescale DPAA 架构由一系列支持以太网连接的硬件模块组成。以太网驱动依赖于 Linux 内核中的以下驱动：)

 - Peripheral Access Memory Unit (PAMU) (* needed only for PPC platforms)
    drivers/iommu/fsl_*
   (外设访问存储单元 (PAMU)，* 仅 PPC 平台需要)
 - Frame Manager (FMan)
    drivers/net/ethernet/freescale/fman
   (帧管理器 (FMan))
 - Queue Manager (QMan), Buffer Manager (BMan)
    drivers/soc/fsl/qbman
   (队列管理器 (QMan)、缓冲区管理器 (BMan))

A simplified view of the dpaa_eth interfaces mapped to FMan MACs::

  dpaa_eth       /eth0\     ...       /ethN\
  driver        |      |             |      |
  -------------   ----   -----------   ----   -------------
       -Ports  / Tx  Rx \    ...    / Tx  Rx \
  FMan        |          |         |          |
       -MACs  |   MAC0   |         |   MACN   |
	     /   dtsec0   \  ...  /   dtsecN   \ (or tgec)
	    /              \     /              \(or memac)
  ---------  --------------  ---  --------------  ---------
      FMan, FMan Port, FMan SP, FMan MURAM drivers
  ---------------------------------------------------------
      FMan HW blocks: MURAM, MACs, Ports, SP
  ---------------------------------------------------------

(dpaa_eth 接口与 FMan MAC 映射关系的简化视图：)

The dpaa_eth relation to the QMan, BMan and FMan::

	      ________________________________
  dpaa_eth   /            eth0                \
  driver    /                                  \
  ---------   -^-   -^-   -^-   ---    ---------
  QMan driver / \   / \   / \  \   /  | BMan    |
	     |Rx | |Rx | |Tx | |Tx |  | driver  |
  ---------  |Dfl| |Err| |Cnf| |FQs|  |         |
  QMan HW    |FQ | |FQ | |FQs| |   |  |         |
	     /   \ /   \ /   \  \ /   |         |
  ---------   ---   ---   ---   -v-    ---------
	    |        FMan QMI         |         |
	    | FMan HW       FMan BMI  | BMan HW |
	      -----------------------   --------

(dpaa_eth 与 QMan、BMan 和 FMan 的关系：)

where the acronyms used above (and in the code) are:
(上文（及代码中）所用缩写的含义如下：)

=============== ===========================================================
DPAA 		Data Path Acceleration Architecture (数据路径加速架构)
FMan 		DPAA Frame Manager (DPAA 帧管理器)
QMan 		DPAA Queue Manager (DPAA 队列管理器)
BMan 		DPAA Buffers Manager (DPAA 缓冲区管理器)
QMI 		QMan interface in FMan (FMan 中的 QMan 接口)
BMI 		BMan interface in FMan (FMan 中的 BMan 接口)
FMan SP 	FMan Storage Profiles (FMan 存储配置文件)
MURAM 		Multi-user RAM in FMan (FMan 中的多用户 RAM)
FQ 		QMan Frame Queue (QMan 帧队列)
Rx Dfl FQ 	default reception FQ (默认接收 FQ)
Rx Err FQ 	Rx error frames FQ (接收错误帧 FQ)
Tx Cnf FQ 	Tx confirmation FQs (发送确认 FQ)
Tx FQs 		transmission frame queues (发送帧队列)
dtsec 		datapath three speed Ethernet controller (10/100/1000 Mbps) (数据路径三速以太网控制器（10/100/1000 Mbps）)
tgec 		ten gigabit Ethernet controller (10 Gbps) (万兆以太网控制器（10 Gbps）)
memac 		multirate Ethernet MAC (10/100/1000/10000) (多速率以太网 MAC（10/100/1000/10000）)
=============== ===========================================================

DPAA Ethernet Supported SoCs(DPAA 以太网支持的 SoC)
===================================================

The DPAA drivers enable the Ethernet controllers present on the following SoCs:
(DPAA 驱动为以下 SoC 上存在的以太网控制器提供支持：)

PPC
(PPC)
- P1023
- P2041
- P3041
- P4080
- P5020
- P5040
- T1023
- T1024
- T1040
- T1042
- T2080
- T4240
- B4860

ARM
(ARM)
- LS1043A
- LS1046A

Configuring DPAA Ethernet in your kernel(在内核中配置 DPAA 以太网)
==================================================================

To enable the DPAA Ethernet driver, the following Kconfig options are required::
(要启用 DPAA 以太网驱动，需要以下 Kconfig 选项：)

  # common for arch/arm64 and arch/powerpc platforms
  CONFIG_FSL_DPAA=y
  CONFIG_FSL_FMAN=y
  CONFIG_FSL_DPAA_ETH=y
  CONFIG_FSL_XGMAC_MDIO=y

  # for arch/powerpc only
  CONFIG_FSL_PAMU=y

  # common options needed for the PHYs used on the RDBs
  CONFIG_VITESSE_PHY=y
  CONFIG_REALTEK_PHY=y
  CONFIG_AQUANTIA_PHY=y

DPAA Ethernet Frame Processing(DPAA 以太网帧处理)
=================================================

On Rx, buffers for the incoming frames are retrieved from the buffers found
in the dedicated interface buffer pool. The driver initializes and seeds these
with one page buffers.
(在接收方向上，用于到达帧的缓冲区从专用接口缓冲池中的缓冲区获取。驱动会用单页缓冲区来初始化并填充这些缓冲池。)

On Tx, all transmitted frames are returned to the driver through Tx
confirmation frame queues. The driver is then responsible for freeing the
buffers. In order to do this properly, a backpointer is added to the buffer
before transmission that points to the skb. When the buffer returns to the
driver on a confirmation FQ, the skb can be correctly consumed.
(在发送方向上，所有已发送的帧都会通过发送确认帧队列返回给驱动。驱动随后负责释放缓冲区。为了正确做到这一点，在发送之前会向缓冲区添加一个指向 skb 的反向指针。当缓冲区通过确认 FQ 返回驱动时，就可以正确地消费该 skb。)

DPAA Ethernet Features(DPAA 以太网特性)
=======================================

Currently the DPAA Ethernet driver enables the basic features required for
a Linux Ethernet driver. The support for advanced features will be added
gradually.
(目前，DPAA 以太网驱动启用了 Linux 以太网驱动所需的基本特性。高级特性的支持将逐步加入。)

The driver has Rx and Tx checksum offloading for UDP and TCP. Currently the Rx
checksum offload feature is enabled by default and cannot be controlled through
ethtool. Also, rx-flow-hash and rx-hashing was added. The addition of RSS
provides a big performance boost for the forwarding scenarios, allowing
different traffic flows received by one interface to be processed by different
CPUs in parallel.
(驱动对 UDP 和 TCP 提供接收和发送校验和卸载。目前接收校验和卸载特性默认启用，且无法通过 ethtool 控制。此外，还加入了 rx-flow-hash 和 rx-hashing。RSS 的加入为转发场景带来了很大的性能提升，使一个接口收到的不同流量流可以由不同 CPU 并行处理。)

The driver has support for multiple prioritized Tx traffic classes. Priorities
range from 0 (lowest) to 3 (highest). These are mapped to HW workqueues with
strict priority levels. Each traffic class contains NR_CPU TX queues. By
default, only one traffic class is enabled and the lowest priority Tx queues
are used. Higher priority traffic classes can be enabled with the mqprio
qdisc. For example, all four traffic classes are enabled on an interface with
the following command. Furthermore, skb priority levels are mapped to traffic
classes as follows:
(驱动支持多个按优先级区分的发送流量类别。优先级范围从 0（最低）到 3（最高）。它们被映射到具有严格优先级的硬件工作队列。每个流量类别包含 NR_CPU 个发送队列。默认情况下只启用一个流量类别，并使用最低优先级的发送队列。可以使用 mqprio qdisc 启用更高优先级的流量类别。例如，使用以下命令即可在一个接口上启用全部四个流量类别。此外，skb 优先级到流量类别的映射如下：)

	* priorities 0 to 3 - traffic class 0 (low priority)
	  (优先级 0 到 3 —— 流量类别 0（低优先级）)
	* priorities 4 to 7 - traffic class 1 (medium-low priority)
	  (优先级 4 到 7 —— 流量类别 1（中低优先级）)
	* priorities 8 to 11 - traffic class 2 (medium-high priority)
	  (优先级 8 到 11 —— 流量类别 2（中高优先级）)
	* priorities 12 to 15 - traffic class 3 (high priority)
	  (优先级 12 到 15 —— 流量类别 3（高优先级）)

::

  tc qdisc add dev <int> root handle 1: \
	 mqprio num_tc 4 map 0 0 0 0 1 1 1 1 2 2 2 2 3 3 3 3 hw 1

DPAA IRQ Affinity and Receive Side Scaling(DPAA 中断亲和性与接收侧扩展)
=======================================================================

Traffic coming on the DPAA Rx queues or on the DPAA Tx confirmation
queues is seen by the CPU as ingress traffic on a certain portal.
The DPAA QMan portal interrupts are affined each to a certain CPU.
The same portal interrupt services all the QMan portal consumers.
(经由 DPAA 接收队列或 DPAA 发送确认队列到达的流量，会被 CPU 看作来自某个 portal 的入站流量。DPAA QMan portal 中断各自被绑定到特定 CPU。同一个 portal 中断为所有 QMan portal 使用者提供服务。)

By default the DPAA Ethernet driver enables RSS, making use of the
DPAA FMan Parser and Keygen blocks to distribute traffic on 128
hardware frame queues using a hash on IP v4/v6 source and destination
and L4 source and destination ports, in present in the received frame.
When RSS is disabled, all traffic received by a certain interface is
received on the default Rx frame queue. The default DPAA Rx frame
queues are configured to put the received traffic into a pool channel
that allows any available CPU portal to dequeue the ingress traffic.
The default frame queues have the HOLDACTIVE option set, ensuring that
traffic bursts from a certain queue are serviced by the same CPU.
This ensures a very low rate of frame reordering. A drawback of this
is that only one CPU at a time can service the traffic received by a
certain interface when RSS is not enabled.
(默认情况下，DPAA 以太网驱动启用 RSS，利用 DPAA FMan 解析器 (Parser) 和密钥生成器 (Keygen) 模块，基于接收帧中存在的 IP v4/v6 源地址和目的地址以及 L4 源端口和目的端口做哈希，把流量分布到 128 个硬件帧队列上。当 RSS 被禁用时，某个接口收到的所有流量都会在默认接收帧队列上接收。默认的 DPAA 接收帧队列被配置为把收到的流量放入一个池通道 (pool channel)，允许任何可用的 CPU portal 将入站流量出队。默认帧队列设置了 HOLDACTIVE 选项，确保来自某个队列的流量突发由同一个 CPU 处理。这确保了极低的帧重排率。其缺点是在未启用 RSS 时，同一时间只能有一个 CPU 处理某个接口收到的流量。)

To implement RSS, the DPAA Ethernet driver allocates an extra set of
128 Rx frame queues that are configured to dedicated channels, in a
round-robin manner. The mapping of the frame queues to CPUs is now
hardcoded, there is no indirection table to move traffic for a certain
FQ (hash result) to another CPU. The ingress traffic arriving on one
of these frame queues will arrive at the same portal and will always
be processed by the same CPU. This ensures intra-flow order preservation
and workload distribution for multiple traffic flows.
(为了实现 RSS，DPAA 以太网驱动额外分配了一组 128 个接收帧队列，并以轮询 (round-robin) 方式把它们配置到专用通道。帧队列到 CPU 的映射现在是硬编码的，没有间接表可以把某个 FQ（哈希结果）的流量转移到另一个 CPU。到达这些帧队列之一的入站流量会到达同一 portal，并始终由同一个 CPU 处理。这确保了流内顺序保持以及多条流量流之间的负载分配。)

RSS can be turned off for a certain interface using ethtool, i.e.::
(可以使用 ethtool 针对某个接口关闭 RSS，即：)

	# ethtool -N fm1-mac9 rx-flow-hash tcp4 ""

To turn it back on, one needs to set rx-flow-hash for tcp4/6 or udp4/6::
(要重新打开，需要为 tcp4/6 或 udp4/6 设置 rx-flow-hash：)

	# ethtool -N fm1-mac9 rx-flow-hash udp4 sfdn

There is no independent control for individual protocols, any command
run for one of tcp4|udp4|ah4|esp4|sctp4|tcp6|udp6|ah6|esp6|sctp6 is
going to control the rx-flow-hashing for all protocols on that interface.
(对单个协议没有独立的控制，针对 tcp4|udp4|ah4|esp4|sctp4|tcp6|udp6|ah6|esp6|sctp6 中任何一个协议运行的命令，都会控制该接口上所有协议的 rx-flow-hash。)

Besides using the FMan Keygen computed hash for spreading traffic on the
128 Rx FQs, the DPAA Ethernet driver also sets the skb hash value when
the NETIF_F_RXHASH feature is on (active by default). This can be turned
on or off through ethtool, i.e.::
(除了使用 FMan Keygen 计算出的哈希把流量分散到 128 个接收 FQ 之外，当 NETIF_F_RXHASH 特性开启（默认激活）时，DPAA 以太网驱动还会设置 skb 哈希值。可以通过 ethtool 开关该特性，即：)

	# ethtool -K fm1-mac9 rx-hashing off
	# ethtool -k fm1-mac9 | grep hash
	receive-hashing: off
	# ethtool -K fm1-mac9 rx-hashing on
	Actual changes:
	receive-hashing: on
	# ethtool -k fm1-mac9 | grep hash
	receive-hashing: on

Please note that Rx hashing depends upon the rx-flow-hashing being on
for that interface - turning off rx-flow-hashing will also disable the
rx-hashing (without ethtool reporting it as off as that depends on the
NETIF_F_RXHASH feature flag).
(请注意，接收哈希依赖于该接口的 rx-flow-hash 处于开启状态——关闭 rx-flow-hash 也会禁用 rx-hashing（ethtool 不会将其报告为 off，因为那取决于 NETIF_F_RXHASH 特性标志）。)

Debugging(调试)
===============

The following statistics are exported for each interface through ethtool:
(以下统计信息通过 ethtool 针对每个接口导出：)

	- interrupt count per CPU
	  (每 CPU 中断计数)
	- Rx packets count per CPU
	  (每 CPU 接收包计数)
	- Tx packets count per CPU
	  (每 CPU 发送包计数)
	- Tx confirmed packets count per CPU
	  (每 CPU 发送确认包计数)
	- Tx S/G frames count per CPU
	  (每 CPU 发送 S/G 帧计数)
	- Tx error count per CPU
	  (每 CPU 发送错误计数)
	- Rx error count per CPU
	  (每 CPU 接收错误计数)
	- Rx error count per type
	  (按类型统计的接收错误计数)
	- congestion related statistics:
	  (拥塞相关统计信息：)

		- congestion status
		  (拥塞状态)
		- time spent in congestion
		  (处于拥塞状态的时长)
		- number of time the device entered congestion
		  (设备进入拥塞状态的次数)
		- dropped packets count per cause
		  (按原因统计的丢包数)

The driver also exports the following information in sysfs:
(驱动还在 sysfs 中导出以下信息：)

	- the FQ IDs for each FQ type
	  /sys/devices/platform/soc/<addr>.fman/<addr>.ethernet/dpaa-ethernet.<id>/net/fm<nr>-mac<nr>/fqids
	  (每种 FQ 类型的 FQ ID)

	- the ID of the buffer pool in use
	  /sys/devices/platform/soc/<addr>.fman/<addr>.ethernet/dpaa-ethernet.<id>/net/fm<nr>-mac<nr>/bpids
	  (正在使用的缓冲池的 ID)
