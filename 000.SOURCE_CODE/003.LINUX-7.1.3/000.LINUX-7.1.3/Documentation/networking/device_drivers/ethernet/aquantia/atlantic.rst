.. SPDX-License-Identifier: GPL-2.0
.. include:: <isonum.txt>

===============================
Marvell(Aquantia) AQtion Driver
===============================

(Marvell(Aquantia) AQtion 驱动程序)

For the aQuantia Multi-Gigabit PCI Express Family of Ethernet Adapters
(适用于 aQuantia Multi-Gigabit PCI Express 系列以太网适配器)

.. Contents

    - Identifying Your Adapter
    - Configuration
    - Supported ethtool options
    - Command Line Parameters
    - Config file parameters
    - Support
    - License

Identifying Your Adapter
(识别你的适配器)
========================

The driver in this release is compatible with AQC-100, AQC-107, AQC-108
based ethernet adapters.
(本版本中的驱动程序兼容基于 AQC-100、AQC-107、AQC-108 的以太网适配器。)


SFP+ Devices (for AQC-100 based adapters)
(SFP+ 设备 (适用于基于 AQC-100 的适配器))
-----------------------------------------

This release tested with passive Direct Attach Cables (DAC) and SFP+/LC
Optical Transceiver.
(本版本已使用无源直连电缆 (DAC) 和 SFP+/LC 光收发器完成测试。)

Configuration
(配置)
=============

Viewing Link Messages
(查看链路消息)
---------------------

  Link messages will not be displayed to the console if the distribution is
  restricting system messages. In order to see network driver link messages on
  your console, set dmesg to eight by entering the following::
  (如果发行版限制了系统消息，链路消息将不会显示在控制台上。要在控制台上查看网络驱动程序的链路消息，请输入以下命令将 dmesg 设置为 8：)

       dmesg -n 8

  .. note::

     This setting is not saved across reboots.
     (此设置在重启后不会保留。)

Jumbo Frames
------------
(巨型帧)

  The driver supports Jumbo Frames for all adapters. Jumbo Frames support is
  enabled by changing the MTU to a value larger than the default of 1500.
  The maximum value for the MTU is 16000.  Use the `ip` command to
  increase the MTU size.  For example::
  (驱动程序为所有适配器支持巨型帧。将 MTU 更改为大于默认值 1500 的数值即可启用巨型帧支持。MTU 的最大值为 16000。请使用 `ip` 命令增大 MTU 大小。例如：)

	ip link set mtu 16000 dev enp1s0

ethtool
-------
(ethtool 工具)

  The driver utilizes the ethtool interface for driver configuration and
  diagnostics, as well as displaying statistical information. The latest
  ethtool version is required for this functionality.
  (驱动程序利用 ethtool 接口进行驱动程序配置和诊断，以及显示统计信息。此功能需要最新版本的 ethtool。)

NAPI
----
(NAPI)

  NAPI (Rx polling mode) is supported in the atlantic driver.
  (atlantic 驱动程序支持 NAPI (Rx 轮询模式)。)

Supported ethtool options
(支持的 ethtool 选项)
=========================

Viewing adapter settings
(查看适配器设置)
------------------------

 ::

    ethtool <ethX>

 Output example::
 (输出示例：)

  Settings for enp1s0:
    Supported ports: [ TP ]
    Supported link modes:   100baseT/Full
			    1000baseT/Full
			    10000baseT/Full
			    2500baseT/Full
			    5000baseT/Full
    Supported pause frame use: Symmetric
    Supports auto-negotiation: Yes
    Supported FEC modes: Not reported
    Advertised link modes:  100baseT/Full
			    1000baseT/Full
			    10000baseT/Full
			    2500baseT/Full
			    5000baseT/Full
    Advertised pause frame use: Symmetric
    Advertised auto-negotiation: Yes
    Advertised FEC modes: Not reported
    Speed: 10000Mb/s
    Duplex: Full
    Port: Twisted Pair
    PHYAD: 0
    Transceiver: internal
    Auto-negotiation: on
    MDI-X: Unknown
    Supports Wake-on: g
    Wake-on: d
    Link detected: yes


 .. note::

    AQrate speeds (2.5/5 Gb/s) will be displayed only with linux kernels > 4.10.
    But you can still use these speeds::
    (只有 linux 内核版本高于 4.10 时才会显示 AQrate 速率 (2.5/5 Gb/s)。但你仍然可以使用这些速率：)

	ethtool -s eth0 autoneg off speed 2500

Viewing adapter information
(查看适配器信息)
---------------------------

 ::

  ethtool -i <ethX>

 Output example::
 (输出示例：)

  driver: atlantic
  version: 5.2.0-050200rc5-generic-kern
  firmware-version: 3.1.78
  expansion-rom-version:
  bus-info: 0000:01:00.0
  supports-statistics: yes
  supports-test: no
  supports-eeprom-access: no
  supports-register-dump: yes
  supports-priv-flags: no


Viewing Ethernet adapter statistics
(查看以太网适配器统计信息)
-----------------------------------

 ::

    ethtool -S <ethX>

 Output example::
 (输出示例：)

  NIC statistics:
     InPackets: 13238607
     InUCast: 13293852
     InMCast: 52
     InBCast: 3
     InErrors: 0
     OutPackets: 23703019
     OutUCast: 23704941
     OutMCast: 67
     OutBCast: 11
     InUCastOctects: 213182760
     OutUCastOctects: 22698443
     InMCastOctects: 6600
     OutMCastOctects: 8776
     InBCastOctects: 192
     OutBCastOctects: 704
     InOctects: 2131839552
     OutOctects: 226938073
     InPacketsDma: 95532300
     OutPacketsDma: 59503397
     InOctetsDma: 1137102462
     OutOctetsDma: 2394339518
     InDroppedDma: 0
     Queue[0] InPackets: 23567131
     Queue[0] OutPackets: 20070028
     Queue[0] InJumboPackets: 0
     Queue[0] InLroPackets: 0
     Queue[0] InErrors: 0
     Queue[1] InPackets: 45428967
     Queue[1] OutPackets: 11306178
     Queue[1] InJumboPackets: 0
     Queue[1] InLroPackets: 0
     Queue[1] InErrors: 0
     Queue[2] InPackets: 3187011
     Queue[2] OutPackets: 13080381
     Queue[2] InJumboPackets: 0
     Queue[2] InLroPackets: 0
     Queue[2] InErrors: 0
     Queue[3] InPackets: 23349136
     Queue[3] OutPackets: 15046810
     Queue[3] InJumboPackets: 0
     Queue[3] InLroPackets: 0
     Queue[3] InErrors: 0

Interrupt coalescing support
(中断合并支持)
----------------------------

 ITR mode, TX/RX coalescing timings could be viewed with::
 (可以使用以下命令查看 ITR 模式以及 TX/RX 合并时序：)

    ethtool -c <ethX>

 and changed with::
 (并可以使用以下命令进行修改：)

    ethtool -C <ethX> tx-usecs <usecs> rx-usecs <usecs>

 To disable coalescing::
 (要禁用合并：)

    ethtool -C <ethX> tx-usecs 0 rx-usecs 0 tx-max-frames 1 tx-max-frames 1

Wake on LAN support
(局域网唤醒 (WOL) 支持)
-------------------

 WOL support by magic packet::
 (通过魔术数据包 (magic packet) 支持 WOL：)

    ethtool -s <ethX> wol g

 To disable WOL::
 (要禁用 WOL：)

    ethtool -s <ethX> wol d

Set and check the driver message level
(设置和检查驱动程序消息级别)
--------------------------------------

 Set message level
 (设置消息级别)

 ::

    ethtool -s <ethX> msglvl <level>

 Level values:
 (级别值：)

 ======   =============================
 0x0001   general driver status. (通用驱动程序状态。)
 0x0002   hardware probing. (硬件探测。)
 0x0004   link state. (链路状态。)
 0x0008   periodic status check. (周期性状态检查。)
 0x0010   interface being brought down. (接口正在被关闭。)
 0x0020   interface being brought up. (接口正在被启动。)
 0x0040   receive error. (接收错误。)
 0x0080   transmit error. (发送错误。)
 0x0200   interrupt handling. (中断处理。)
 0x0400   transmit completion. (发送完成。)
 0x0800   receive completion. (接收完成。)
 0x1000   packet contents. (数据包内容。)
 0x2000   hardware status. (硬件状态。)
 0x4000   Wake-on-LAN status. (局域网唤醒状态。)
 ======   =============================

 By default, the level of debugging messages is set 0x0001(general driver status).
 (默认情况下，调试消息的级别设置为 0x0001 (通用驱动程序状态)。)

 Check message level
 (检查消息级别)

 ::

    ethtool <ethX> | grep "Current message level"

 If you want to disable the output of messages::
 (如果要禁用消息输出：)

    ethtool -s <ethX> msglvl 0

RX flow rules (ntuple filters)
(RX 流规则 (ntuple 过滤器))
------------------------------

 There are separate rules supported, that applies in that order:
 (支持多种相互独立的规则，按以下顺序应用：)

 1. 16 VLAN ID rules
    (1. 16 条 VLAN ID 规则)
 2. 16 L2 EtherType rules
    (2. 16 条 L2 EtherType 规则)
 3. 8 L3/L4 5-Tuple rules
    (3. 8 条 L3/L4 五元组规则)


 The driver utilizes the ethtool interface for configuring ntuple filters,
 via ``ethtool -N <device> <filter>``.
 (驱动程序利用 ethtool 接口，通过 ``ethtool -N <device> <filter>`` 来配置 ntuple 过滤器。)

 To enable or disable the RX flow rules::
 (要启用或禁用 RX 流规则：)

    ethtool -K ethX ntuple <on|off>

 When disabling ntuple filters, all the user programmed filters are
 flushed from the driver cache and hardware. All needed filters must
 be re-added when ntuple is re-enabled.
 (禁用 ntuple 过滤器时，所有用户编程的过滤器都会从驱动程序缓存和硬件中刷新掉。重新启用 ntuple 时，必须重新添加所有需要的过滤器。)

 Because of the fixed order of the rules, the location of filters is also fixed:
 (由于规则的顺序是固定的，过滤器的位置也是固定的：)

 - Locations 0 - 15 for VLAN ID filters
   (位置 0 - 15 用于 VLAN ID 过滤器)
 - Locations 16 - 31 for L2 EtherType filters
   (位置 16 - 31 用于 L2 EtherType 过滤器)
 - Locations 32 - 39 for L3/L4 5-tuple filters (locations 32, 36 for IPv6)
   (位置 32 - 39 用于 L3/L4 五元组过滤器 (IPv6 使用位置 32 和 36))

 The L3/L4 5-tuple (protocol, source and destination IP address, source and
 destination TCP/UDP/SCTP port) is compared against 8 filters. For IPv4, up to
 8 source and destination addresses can be matched. For IPv6, up to 2 pairs of
 addresses can be supported. Source and destination ports are only compared for
 TCP/UDP/SCTP packets.
 (L3/L4 五元组 (协议、源和目的 IP 地址、源和目的 TCP/UDP/SCTP 端口) 会与 8 个过滤器进行比较。对于 IPv4，最多可匹配 8 对源和目的地址。对于 IPv6，最多可支持 2 对地址。源和目的端口仅对 TCP/UDP/SCTP 数据包进行比较。)

 To add a filter that directs packet to queue 5, use
 ``<-N|-U|--config-nfc|--config-ntuple>`` switch::
 (要添加将数据包引导到队列 5 的过滤器，请使用 ``<-N|-U|--config-nfc|--config-ntuple>`` 开关：)

    ethtool -N <ethX> flow-type udp4 src-ip 10.0.0.1 dst-ip 10.0.0.2 src-port 2000 dst-port 2001 action 5 <loc 32>

 - action is the queue number.
   (action 是队列编号。)
 - loc is the rule number.
   (loc 是规则编号。)

 For ``flow-type ip4|udp4|tcp4|sctp4|ip6|udp6|tcp6|sctp6`` you must set the loc
 number within 32 - 39.
 (对于 ``flow-type ip4|udp4|tcp4|sctp4|ip6|udp6|tcp6|sctp6`` 类型，你必须将 loc 编号设置在 32 - 39 范围内。)
 For ``flow-type ip4|udp4|tcp4|sctp4|ip6|udp6|tcp6|sctp6`` you can set 8 rules
 for traffic IPv4 or you can set 2 rules for traffic IPv6. Loc number traffic
 IPv6 is 32 and 36.
 At the moment you can not use IPv4 and IPv6 filters at the same time.
 (对于 ``flow-type ip4|udp4|tcp4|sctp4|ip6|udp6|tcp6|sctp6`` 类型，你可以为 IPv4 流量设置 8 条规则，或者为 IPv6 流量设置 2 条规则。IPv6 流量的 loc 编号为 32 和 36。目前无法同时使用 IPv4 和 IPv6 过滤器。)

 Example filter for IPv6 filter traffic::
 (IPv6 过滤流量的过滤器示例：)

    sudo ethtool -N <ethX> flow-type tcp6 src-ip 2001:db8:0:f101::1 dst-ip 2001:db8:0:f101::2 action 1 loc 32
    sudo ethtool -N <ethX> flow-type ip6 src-ip 2001:db8:0:f101::2 dst-ip 2001:db8:0:f101::5 action -1 loc 36

 Example filter for IPv4 filter traffic::
 (IPv4 过滤流量的过滤器示例：)

    sudo ethtool -N <ethX> flow-type udp4 src-ip 10.0.0.4 dst-ip 10.0.0.7 src-port 2000 dst-port 2001 loc 32
    sudo ethtool -N <ethX> flow-type tcp4 src-ip 10.0.0.3 dst-ip 10.0.0.9 src-port 2000 dst-port 2001 loc 33
    sudo ethtool -N <ethX> flow-type ip4 src-ip 10.0.0.6 dst-ip 10.0.0.4 loc 34

 If you set action -1, then all traffic corresponding to the filter will be discarded.
 (如果将 action 设置为 -1，则与该过滤器匹配的所有流量都将被丢弃。)

 The maximum value action is 31.
 (action 的最大值为 31。)


 The VLAN filter (VLAN id) is compared against 16 filters.
 VLAN id must be accompanied by mask 0xF000. That is to distinguish VLAN filter
 from L2 Ethertype filter with UserPriority since both User Priority and VLAN ID
 are passed in the same 'vlan' parameter.
 (VLAN 过滤器 (VLAN id) 会与 16 个过滤器进行比较。VLAN id 必须附带掩码 0xF000。这样做是为了将 VLAN 过滤器与带 UserPriority 的 L2 Ethertype 过滤器区分开，因为 User Priority 和 VLAN ID 是通过同一个 'vlan' 参数传入的。)

 To add a filter that directs packets from VLAN 2001 to queue 5::
 (要添加将来自 VLAN 2001 的数据包引导到队列 5 的过滤器：)

    ethtool -N <ethX> flow-type ip4 vlan 2001 m 0xF000 action 1 loc 0


 L2 EtherType filters allows filter packet by EtherType field or both EtherType
 and User Priority (PCP) field of 802.1Q.
 UserPriority (vlan) parameter must be accompanied by mask 0x1FFF. That is to
 distinguish VLAN filter from L2 Ethertype filter with UserPriority since both
 User Priority and VLAN ID are passed in the same 'vlan' parameter.
 (L2 EtherType 过滤器允许按 EtherType 字段过滤数据包，或者同时按 802.1Q 的 EtherType 和 User Priority (PCP) 字段过滤数据包。UserPriority (vlan) 参数必须附带掩码 0x1FFF。这样做是为了将 VLAN 过滤器与带 UserPriority 的 L2 Ethertype 过滤器区分开，因为 User Priority 和 VLAN ID 是通过同一个 'vlan' 参数传入的。)

 To add a filter that directs IP4 packess of priority 3 to queue 3::
 (要添加将优先级为 3 的 IP4 数据包引导到队列 3 的过滤器：)

    ethtool -N <ethX> flow-type ether proto 0x800 vlan 0x600 m 0x1FFF action 3 loc 16

 To see the list of filters currently present::
 (要查看当前存在的过滤器列表：)

    ethtool <-u|-n|--show-nfc|--show-ntuple> <ethX>

 Rules may be deleted from the table itself. This is done using::
 (可以直接从表中删除规则。这通过以下命令完成：)

    sudo ethtool <-N|-U|--config-nfc|--config-ntuple> <ethX> delete <loc>

 - loc is the rule number to be deleted.
   (loc 是要删除的规则编号。)

 Rx filters is an interface to load the filter table that funnels all flow
 into queue 0 unless an alternative queue is specified using "action". In that
 case, any flow that matches the filter criteria will be directed to the
 appropriate queue. RX filters is supported on all kernels 2.6.30 and later.
 (Rx 过滤器是加载过滤器表的接口，该表将所有流引导到队列 0，除非使用 "action" 指定了其他队列。在这种情况下，任何匹配过滤条件的流都会被引导到相应的队列。所有 2.6.30 及更高版本的内核都支持 RX 过滤器。)

RSS for UDP
(UDP 的 RSS)
-----------

 Currently, NIC does not support RSS for fragmented IP packets, which leads to
 incorrect working of RSS for fragmented UDP traffic. To disable RSS for UDP the
 RX Flow L3/L4 rule may be used.
 (目前，网卡不支持针对分片 IP 数据包的 RSS，这导致分片 UDP 流量的 RSS 无法正常工作。可以使用 RX Flow L3/L4 规则来禁用 UDP 的 RSS。)

 Example::
 (示例：)

    ethtool -N eth0 flow-type udp4 action 0 loc 32

UDP GSO hardware offload
(UDP GSO 硬件卸载)
------------------------

 UDP GSO allows to boost UDP tx rates by offloading UDP headers allocation
 into hardware. A special userspace socket option is required for this,
 could be validated with /kernel/tools/testing/selftests/net/::
 (UDP GSO 通过将 UDP 头的分配卸载到硬件来提升 UDP 发送速率。这需要一个特殊的用户空间套接字选项，可使用 /kernel/tools/testing/selftests/net/ 下的测试进行验证：)

    udpgso_bench_tx -u -4 -D 10.0.1.1 -s 6300 -S 100

 Will cause sending out of 100 byte sized UDP packets formed from single
 6300 bytes user buffer.
 (这会将从单个 6300 字节用户缓冲区生成的 100 个字节大小的 UDP 数据包发送出去。)

 UDP GSO is configured by::
 (UDP GSO 通过以下命令配置：)

    ethtool -K eth0 tx-udp-segmentation on

Private flags (testing)
(私有标志 (测试))
-----------------------

 Atlantic driver supports private flags for hardware custom features::
 (Atlantic 驱动程序支持用于硬件自定义功能的私有标志：)

	$ ethtool --show-priv-flags ethX

	Private flags for ethX:
	DMASystemLoopback  : off
	PKTSystemLoopback  : off
	DMANetworkLoopback : off
	PHYInternalLoopback: off
	PHYExternalLoopback: off

 Example::
 (示例：)

	$ ethtool --set-priv-flags ethX DMASystemLoopback on

 DMASystemLoopback:   DMA Host loopback.
 (DMASystemLoopback:   DMA 主机环回。)
 PKTSystemLoopback:   Packet buffer host loopback.
 (PKTSystemLoopback:   数据包缓冲区主机环回。)
 DMANetworkLoopback:  Network side loopback on DMA block.
 (DMANetworkLoopback:  DMA 块上的网络侧环回。)
 PHYInternalLoopback: Internal loopback on Phy.
 (PHYInternalLoopback: PHY 内部环回。)
 PHYExternalLoopback: External loopback on Phy (with loopback ethernet cable).
 (PHYExternalLoopback: PHY 外部环回 (使用环回以太网电缆)。)


Command Line Parameters
(命令行参数)
=======================

The following command line parameters are available on atlantic driver:
(atlantic 驱动程序提供以下命令行参数：)

aq_itr -Interrupt throttling mode
(aq_itr - 中断节流模式)
---------------------------------

Accepted values: 0, 1, 0xFFFF
(取值：0、1、0xFFFF)

Default value: 0xFFFF
(默认值：0xFFFF)

======   ==============================================================
0        Disable interrupt throttling. (禁用中断节流。)
1        Enable interrupt throttling and use specified tx and rx rates. (启用中断节流并使用指定的 tx 和 rx 速率。)
0xFFFF   Auto throttling mode. Driver will choose the best RX and TX
	 interrupt throttling settings based on link speed. (自动节流模式。驱动程序将根据链路速率选择最佳的 RX 和 TX 中断节流设置。)
======   ==============================================================

aq_itr_tx - TX interrupt throttle rate
(aq_itr_tx - TX 中断节流速率)
--------------------------------------

Accepted values: 0 - 0x1FF
(取值：0 - 0x1FF)

Default value: 0
(默认值：0)

TX side throttling in microseconds. Adapter will setup maximum interrupt delay
to this value. Minimum interrupt delay will be a half of this value
(TX 侧节流，单位为微秒。适配器会将最大中断延迟设置为此值。最小中断延迟为该值的一半)

aq_itr_rx - RX interrupt throttle rate
(aq_itr_rx - RX 中断节流速率)
--------------------------------------

Accepted values: 0 - 0x1FF
(取值：0 - 0x1FF)

Default value: 0
(默认值：0)

RX side throttling in microseconds. Adapter will setup maximum interrupt delay
to this value. Minimum interrupt delay will be a half of this value
(RX 侧节流，单位为微秒。适配器会将最大中断延迟设置为此值。最小中断延迟为该值的一半)

.. note::

   ITR settings could be changed in runtime by ethtool -c means (see below)
   (ITR 设置可以在运行时通过 ethtool -c 进行更改 (见下文))

Config file parameters
(配置文件参数)
======================

For some fine tuning and performance optimizations,
some parameters can be changed in the {source_dir}/aq_cfg.h file.
(要进行一些微调和性能优化，可以在 {source_dir}/aq_cfg.h 文件中更改某些参数。)

AQ_CFG_RX_PAGEORDER
(AQ_CFG_RX_PAGEORDER)
-------------------

Default value: 0
(默认值：0)

RX page order override. That's a power of 2 number of RX pages allocated for
each descriptor. Received descriptor size is still limited by
AQ_CFG_RX_FRAME_MAX.
(RX 页阶 (page order) 覆盖。这是为每个描述符分配的 RX 页数的 2 的幂。接收描述符的大小仍受 AQ_CFG_RX_FRAME_MAX 限制。)

Increasing pageorder makes page reuse better (actual on iommu enabled systems).
(增大 pageorder 可以改善页复用 (在启用 iommu 的系统上尤为实用)。)

AQ_CFG_RX_REFILL_THRES
(AQ_CFG_RX_REFILL_THRES)
----------------------

Default value: 32
(默认值：32)

RX refill threshold. RX path will not refill freed descriptors until the
specified number of free descriptors is observed. Larger values may help
better page reuse but may lead to packet drops as well.
(RX 补充阈值。在观察到指定数量的空闲描述符之前，RX 路径不会补充已释放的描述符。较大的值可能有助于更好的页复用，但也可能导致丢包。)

AQ_CFG_VECS_DEF
(AQ_CFG_VECS_DEF)
---------------

Number of queues
(队列数量)

Valid Range: 0 - 8 (up to AQ_CFG_VECS_MAX)
(有效范围：0 - 8 (最大至 AQ_CFG_VECS_MAX))

Default value: 8
(默认值：8)

Notice this value will be capped by the number of cores available on the system.
(注意，此值将受系统上可用核心数的限制。)

AQ_CFG_IS_RSS_DEF
(AQ_CFG_IS_RSS_DEF)
-----------------

Enable/disable Receive Side Scaling
(启用/禁用接收侧扩展 (Receive Side Scaling))

This feature allows the adapter to distribute receive processing
across multiple CPU-cores and to prevent from overloading a single CPU core.
(此功能允许适配器将接收处理分散到多个 CPU 核心上，并防止单个 CPU 核心过载。)

Valid values
(有效值)

==  ========
0   disabled (禁用)
1   enabled (启用)
==  ========

Default value: 1
(默认值：1)

AQ_CFG_NUM_RSS_QUEUES_DEF
(AQ_CFG_NUM_RSS_QUEUES_DEF)
-------------------------

Number of queues for Receive Side Scaling
(用于接收侧扩展的队列数量)

Valid Range: 0 - 8 (up to AQ_CFG_VECS_DEF)
(有效范围：0 - 8 (最大至 AQ_CFG_VECS_DEF))

Default value: AQ_CFG_VECS_DEF
(默认值：AQ_CFG_VECS_DEF)

AQ_CFG_IS_LRO_DEF
(AQ_CFG_IS_LRO_DEF)
-----------------

Enable/disable Large Receive Offload
(启用/禁用大接收卸载 (Large Receive Offload))

This offload enables the adapter to coalesce multiple TCP segments and indicate
them as a single coalesced unit to the OS networking subsystem.
(此卸载功能使适配器能够将多个 TCP 报文段合并起来，并作为一个合并后的单元提交给操作系统的网络子系统。)

The system consumes less energy but it also introduces more latency in packets
processing.
(系统消耗的能量更少，但也会在数据包处理中引入更多延迟。)

Valid values
(有效值)

==  ========
0   disabled (禁用)
1   enabled (启用)
==  ========

Default value: 1
(默认值：1)

AQ_CFG_TX_CLEAN_BUDGET
(AQ_CFG_TX_CLEAN_BUDGET)
----------------------

Maximum descriptors to cleanup on TX at once.
(一次在 TX 上清理的最大描述符数量。)

Default value: 256
(默认值：256)

After the aq_cfg.h file changed the driver must be rebuilt to take effect.
(修改 aq_cfg.h 文件后，必须重新构建驱动程序才能生效。)

Support
(支持)
=======

If an issue is identified with the released source code on the supported
kernel with a supported adapter, email the specific information related
to the issue to aqn_support@marvell.com
(如果在受支持的内核上使用受支持的适配器时发现已发布源代码的问题，请将与该问题相关的具体信息通过电子邮件发送至 aqn_support@marvell.com)

License
(许可证)
=======

aQuantia Corporation Network Driver
(aQuantia 公司网络驱动程序)

Copyright |copy| 2014 - 2019 aQuantia Corporation.
(版权所有 |copy| 2014 - 2019 aQuantia Corporation。)

This program is free software; you can redistribute it and/or modify it
under the terms and conditions of the GNU General Public License,
version 2, as published by the Free Software Foundation.
(本程序是自由软件；你可以根据自由软件基金会发布的 GNU 通用公共许可证第 2 版的条款和条件对其进行再分发和/或修改。)
