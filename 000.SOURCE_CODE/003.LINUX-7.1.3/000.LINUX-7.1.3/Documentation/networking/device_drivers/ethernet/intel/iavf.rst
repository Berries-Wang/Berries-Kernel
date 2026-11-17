.. SPDX-License-Identifier: GPL-2.0+

=================================================================
Linux Base Driver for Intel(R) Ethernet Adaptive Virtual Function
=================================================================

(Intel(R) Ethernet Adaptive Virtual Function 的 Linux 基础驱动程序)

Intel Ethernet Adaptive Virtual Function Linux driver.
Copyright(c) 2013-2018 Intel Corporation.
(Intel Ethernet Adaptive Virtual Function Linux 驱动程序。
版权所有(c) 2013-2018 Intel Corporation。)

Contents
(目录)
========

- Overview
  (概述)
- Identifying Your Adapter
  (识别您的适配器)
- Additional Configurations
  (其他配置)
- Known Issues/Troubleshooting
  (已知问题/故障排除)
- Support
  (支持)

Overview
(概述)
========

This file describes the iavf Linux Base Driver. This driver was formerly
called i40evf.
(本文描述 iavf Linux 基础驱动程序。该驱动程序曾名为 i40evf。)

The iavf driver supports the below mentioned virtual function devices and
can only be activated on kernels running the i40e or newer Physical Function
(PF) driver compiled with CONFIG_PCI_IOV.  The iavf driver requires
CONFIG_PCI_MSI to be enabled.
(iavf 驱动程序支持下文列出的虚拟功能设备，并且只能在运行 i40e 或更新
物理功能（PF）驱动程序、且编译时启用 CONFIG_PCI_IOV 的内核上激活。
iavf 驱动程序要求启用 CONFIG_PCI_MSI。)

The guest OS loading the iavf driver must support MSI-X interrupts.
(加载 iavf 驱动程序的客户操作系统必须支持 MSI-X 中断。)

Identifying Your Adapter
(识别您的适配器)
========================

The driver in this kernel is compatible with devices based on the following:
 * Intel(R) XL710 X710 Virtual Function
 * Intel(R) X722 Virtual Function
 * Intel(R) XXV710 Virtual Function
 * Intel(R) Ethernet Adaptive Virtual Function

(本内核中的驱动程序与基于以下设备的硬件兼容：Intel(R) XL710 X710 虚拟
功能、Intel(R) X722 虚拟功能、Intel(R) XXV710 虚拟功能，以及 Intel(R)
Ethernet Adaptive Virtual Function（以太网自适应虚拟功能）。)

For the best performance, make sure the latest NVM/FW is installed on your
device.
(为获得最佳性能，请确保您的设备上安装了最新的 NVM/FW。)

For information on how to identify your adapter, and for the latest NVM/FW
images and Intel network drivers, refer to the Intel Support website:
https://www.intel.com/support
(有关如何识别您的适配器的信息，以及最新的 NVM/FW 镜像和 Intel 网络驱动
程序，请参阅 Intel 支持网站：https://www.intel.com/support)


Additional Features and Configurations
(其他特性与配置)
======================================

Viewing Link Messages
(查看链路消息)
---------------------
Link messages will not be displayed to the console if the distribution is
restricting system messages. In order to see network driver link messages on
your console, set dmesg to eight by entering the following::
(如果发行版限制了系统消息，链路消息将不会显示到控制台。要在控制台上
看到网络驱动程序的链路消息，请输入以下命令将 dmesg 设为 8：)

    # dmesg -n 8

NOTE:
  This setting is not saved across reboots.
  (注意：该设置在重启后不会保留。)

ethtool
(ethtool 工具)
-------
The driver utilizes the ethtool interface for driver configuration and
diagnostics, as well as displaying statistical information. The latest ethtool
version is required for this functionality. Download it at:
https://www.kernel.org/pub/software/network/ethtool/
(驱动程序利用 ethtool 接口进行驱动配置与诊断，以及显示统计信息。此功能
需要最新版本的 ethtool。可从以下地址下载：
https://www.kernel.org/pub/software/network/ethtool/)

Setting VLAN Tag Stripping
(设置 VLAN 标签剥离)
--------------------------
If you have applications that require Virtual Functions (VFs) to receive
packets with VLAN tags, you can disable VLAN tag stripping for the VF. The
Physical Function (PF) processes requests issued from the VF to enable or
disable VLAN tag stripping. Note that if the PF has assigned a VLAN to a VF,
then requests from that VF to set VLAN tag stripping will be ignored.
(如果您的应用要求虚拟功能（VF）接收带有 VLAN 标签的数据包，可以为该 VF
禁用 VLAN 标签剥离。物理功能（PF）会处理 VF 发出的启用或禁用 VLAN 标签
剥离的请求。注意：如果 PF 已为某个 VF 分配了 VLAN，那么该 VF 发出的设置
VLAN 标签剥离的请求将被忽略。)

To enable/disable VLAN tag stripping for a VF, issue the following command
from inside the VM in which you are running the VF::
(要为某个 VF 启用/禁用 VLAN 标签剥离，请在该 VF 所在的 VM 内部执行以下
命令：)

    # ethtool -K <if_name> rxvlan on/off

or alternatively::
(或者也可以：)

    # ethtool --offload <if_name> rxvlan on/off

Adaptive Virtual Function
(自适应虚拟功能)
-------------------------
Adaptive Virtual Function (AVF) allows the virtual function driver, or VF, to
adapt to changing feature sets of the physical function driver (PF) with which
it is associated. This allows system administrators to update a PF without
having to update all the VFs associated with it. All AVFs have a single common
device ID and branding string.
(自适应虚拟功能（AVF）让虚拟功能驱动程序（即 VF）能够适应与其关联的
物理功能驱动程序（PF）不断变化的特性集。这使系统管理员得以更新 PF，
而不必更新与之关联的所有 VF。所有 AVF 都共用同一个设备 ID 和品牌
字符串。)

AVFs have a minimum set of features known as "base mode," but may provide
additional features depending on what features are available in the PF with
which the AVF is associated. The following are base mode features:
(AVF 具有一组称为"基础模式"（base mode）的最小特性集，但也可能视其所
关联 PF 中可用的特性而提供额外的功能。以下为基础模式的特性：)

- 4 Queue Pairs (QP) and associated Configuration Status Registers (CSRs)
  for Tx/Rx
  (4 个队列对（QP）及用于发送/接收的相关配置状态寄存器（CSR）)
- i40e descriptors and ring format
  (i40e 描述符与环形队列格式)
- Descriptor write-back completion
  (描述符回写完成)
- 1 control queue, with i40e descriptors, CSRs and ring format
  (1 个控制队列，带 i40e 描述符、CSR 与环形队列格式)
- 5 MSI-X interrupt vectors and corresponding i40e CSRs
  (5 个 MSI-X 中断向量及对应的 i40e CSR)
- 1 Interrupt Throttle Rate (ITR) index
  (1 个中断节流率（ITR）索引)
- 1 Virtual Station Interface (VSI) per VF
  (每个 VF 1 个虚拟工作站接口（VSI）)
- 1 Traffic Class (TC), TC0
  (1 个流量类（TC），即 TC0)
- Receive Side Scaling (RSS) with 64 entry indirection table and key,
  configured through the PF
  (接收侧扩展（RSS），带 64 表项的间接表与密钥，通过 PF 配置)
- 1 unicast MAC address reserved per VF
  (每个 VF 保留 1 个单播 MAC 地址)
- 16 MAC address filters for each VF
  (每个 VF 16 个 MAC 地址过滤器)
- Stateless offloads - non-tunneled checksums
  (无状态卸载——非隧道校验和)
- AVF device ID
  (AVF 设备 ID)
- HW mailbox is used for VF to PF communications (including on Windows)
  (硬件邮箱用于 VF 与 PF 之间的通信（包括在 Windows 上）)

IEEE 802.1ad (QinQ) Support
(IEEE 802.1ad（QinQ）支持)
---------------------------
The IEEE 802.1ad standard, informally known as QinQ, allows for multiple VLAN
IDs within a single Ethernet frame. VLAN IDs are sometimes referred to as
"tags," and multiple VLAN IDs are thus referred to as a "tag stack." Tag stacks
allow L2 tunneling and the ability to segregate traffic within a particular
VLAN ID, among other uses.
(IEEE 802.1ad 标准俗称 QinQ，允许在单个以太网帧中携带多个 VLAN ID。
VLAN ID 有时被称为"标签"（tags），多个 VLAN ID 因此被称为"标签栈"
（tag stack）。标签栈可用于 L2 隧道，以及在特定 VLAN ID 内隔离流量等
多种用途。)

The following are examples of how to configure 802.1ad (QinQ)::
(以下为配置 802.1ad（QinQ）的示例：)

    # ip link add link eth0 eth0.24 type vlan proto 802.1ad id 24
    # ip link add link eth0.24 eth0.24.371 type vlan proto 802.1Q id 371

Where "24" and "371" are example VLAN IDs.
(其中 "24" 和 "371" 是示例 VLAN ID。)

NOTES:
  Receive checksum offloads, cloud filters, and VLAN acceleration are not
  supported for 802.1ad (QinQ) packets.
  (注意：802.1ad（QinQ）数据包不支持接收校验和卸载、云过滤器以及 VLAN
  加速。)

Application Device Queues (ADq)
(应用设备队列（ADq）)
-------------------------------
Application Device Queues (ADq) allows you to dedicate one or more queues to a
specific application. This can reduce latency for the specified application,
and allow Tx traffic to be rate limited per application. Follow the steps below
to set ADq.
(应用设备队列（ADq）允许您将一个或多个队列专用于特定应用。这可以降低
指定应用的延迟，并允许按应用对发送流量进行限速。请按照以下步骤设置
ADq。)

Requirements:
(要求：)

- The sch_mqprio, act_mirred and cls_flower modules must be loaded
  (必须加载 sch_mqprio、act_mirred 和 cls_flower 模块)
- The latest version of iproute2
  (需要最新版本的 iproute2)
- If another driver (for example, DPDK) has set cloud filters, you cannot
  enable ADQ
  (如果其他驱动程序（例如 DPDK）已设置了云过滤器，则无法启用 ADQ)
- Depending on the underlying PF device, ADQ cannot be enabled when the
  following features are enabled:
  (取决于底层 PF 设备，当以下特性被启用时将无法启用 ADQ：)

  + Data Center Bridging (DCB)
    (数据中心桥接（DCB）)
  + Multiple Functions per Port (MFP)
    (每端口多功能（MFP）)
  + Sideband Filters
    (边带过滤器)

1. Create traffic classes (TCs). Maximum of 8 TCs can be created per interface.
The shaper bw_rlimit parameter is optional.
(创建流量类（TC）。每个接口最多可创建 8 个 TC。整形器 bw_rlimit 参数为
可选项。)

Example: Sets up two tcs, tc0 and tc1, with 16 queues each and max tx rate set
to 1Gbit for tc0 and 3Gbit for tc1.
(示例：设置两个 TC（tc0 和 tc1），各含 16 个队列，tc0 的最大发送速率设
为 1Gbit，tc1 设为 3Gbit。)

::

    tc qdisc add dev <interface> root mqprio num_tc 2 map 0 0 0 0 1 1 1 1
    queues 16@0 16@16 hw 1 mode channel shaper bw_rlimit min_rate 1Gbit 2Gbit
    max_rate 1Gbit 3Gbit

map: priority mapping for up to 16 priorities to tcs (e.g. map 0 0 0 0 1 1 1 1
sets priorities 0-3 to use tc0 and 4-7 to use tc1)
(map：最多 16 个优先级到各 TC 的映射（例如 map 0 0 0 0 1 1 1 1 表示
优先级 0-3 使用 tc0，优先级 4-7 使用 tc1）)

queues: for each tc, <num queues>@<offset> (e.g. queues 16@0 16@16 assigns
16 queues to tc0 at offset 0 and 16 queues to tc1 at offset 16. Max total
number of queues for all tcs is 64 or number of cores, whichever is lower.)
(queues：对每个 TC 而言为 <num queues>@<offset>（例如 queues 16@0
16@16 表示把 16 个队列分配给偏移 0 处的 tc0，16 个队列分配给偏移 16 处
的 tc1。所有 TC 的队列总数上限为 64 或 CPU 核数，取两者中较小者）。)

hw 1 mode channel: ‘channel’ with ‘hw’ set to 1 is a new new hardware
offload mode in mqprio that makes full use of the mqprio options, the
TCs, the queue configurations, and the QoS parameters.
(hw 1 mode channel："channel" 配合 "hw" 设为 1，是 mqprio 中的一种新
硬件卸载模式，可充分利用 mqprio 选项、TC、队列配置以及 QoS 参数。)

shaper bw_rlimit: for each tc, sets minimum and maximum bandwidth rates.
Totals must be equal or less than port speed.
(shaper bw_rlimit：为每个 TC 设置最小与最大带宽速率。总和必须等于或
小于端口速度。)

For example: min_rate 1Gbit 3Gbit: Verify bandwidth limit using network
monitoring tools such as ``ifstat`` or ``sar -n DEV [interval] [number of samples]``
(例如：min_rate 1Gbit 3Gbit：使用 ``ifstat`` 或
``sar -n DEV [interval] [number of samples]`` 等网络监控工具验证带宽
限制。)

NOTE:
  Setting up channels via ethtool (ethtool -L) is not supported when the
  TCs are configured using mqprio.
  (注意：当 TC 通过 mqprio 配置时，不支持用 ethtool（ethtool -L）设置
  channel。)

2. Enable HW TC offload on interface::
   (在接口上启用硬件 TC 卸载：)

    # ethtool -K <interface> hw-tc-offload on

3. Apply TCs to ingress (RX) flow of interface::
   (将 TC 应用到接口的入站（RX）流：)

    # tc qdisc add dev <interface> ingress

NOTES:
 - Run all tc commands from the iproute2 <pathtoiproute2>/tc/ directory
   (请从 iproute2 的 <pathtoiproute2>/tc/ 目录运行所有 tc 命令)
 - ADq is not compatible with cloud filters
   (ADq 与云过滤器不兼容)
 - Setting up channels via ethtool (ethtool -L) is not supported when the TCs
   are configured using mqprio
   (当 TC 通过 mqprio 配置时，不支持用 ethtool（ethtool -L）设置
   channel)
 - You must have iproute2 latest version
   (必须使用最新版本的 iproute2)
 - NVM version 6.01 or later is required
   (需要 NVM 6.01 或更高版本)
 - ADq cannot be enabled when any the following features are enabled: Data
   Center Bridging (DCB), Multiple Functions per Port (MFP), or Sideband Filters
   (当以下任一特性被启用时无法启用 ADq：数据中心桥接（DCB）、每端口
   多功能（MFP）或边带过滤器)
 - If another driver (for example, DPDK) has set cloud filters, you cannot
   enable ADq
   (如果其他驱动程序（例如 DPDK）已设置了云过滤器，则无法启用 ADq)
 - Tunnel filters are not supported in ADq. If encapsulated packets do arrive
   in non-tunnel mode, filtering will be done on the inner headers.  For example,
   for VXLAN traffic in non-tunnel mode, PCTYPE is identified as a VXLAN
   encapsulated packet, outer headers are ignored. Therefore, inner headers are
   matched.
   (ADq 不支持隧道过滤器。如果封装数据包确实在非隧道模式下到达，将在
   内层报头上进行过滤。例如，对于非隧道模式下的 VXLAN 流量，PCTYPE 被
   识别为 VXLAN 封装数据包，外层报头被忽略，因此匹配的是内层报头。)
 - If a TC filter on a PF matches traffic over a VF (on the PF), that traffic
   will be routed to the appropriate queue of the PF, and will not be passed on
   the VF. Such traffic will end up getting dropped higher up in the TCP/IP
   stack as it does not match PF address data.
   (如果 PF 上的 TC 过滤器匹配了经由（该 PF 上的）VF 的流量，该流量将被
   路由到 PF 的相应队列，而不会传递给 VF。由于与 PF 的地址数据不匹配，
   这类流量最终会在 TCP/IP 栈的上层被丢弃。)
 - If traffic matches multiple TC filters that point to different TCs, that
   traffic will be duplicated and sent to all matching TC queues.  The hardware
   switch mirrors the packet to a VSI list when multiple filters are matched.
   (如果流量匹配了指向不同 TC 的多个 TC 过滤器，该流量将被复制并发送到
   所有匹配的 TC 队列。当匹配了多个过滤器时，硬件交换机将数据包镜像到
   一个 VSI 列表。)


Known Issues/Troubleshooting
(已知问题/故障排除)
============================

Bonding fails with VFs bound to an Intel(R) Ethernet Controller 700 series device
---------------------------------------------------------------------------------
(将 VF 绑定到 Intel(R) 以太网控制器 700 系列设备时绑定失败)

If you bind Virtual Functions (VFs) to an Intel(R) Ethernet Controller 700
series based device, the VF slaves may fail when they become the active slave.
If the MAC address of the VF is set by the PF (Physical Function) of the
device, when you add a slave, or change the active-backup slave, Linux bonding
tries to sync the backup slave's MAC address to the same MAC address as the
active slave. Linux bonding will fail at this point. This issue will not occur
if the VF's MAC address is not set by the PF.
(如果把虚拟功能（VF）绑定到基于 Intel(R) 以太网控制器 700 系列的设备上，
VF 从属接口在成为活动从属接口时可能会失败。如果 VF 的 MAC 地址是由设备的
PF（物理功能）设置的，那么在添加从属接口或更改 active-backup 活动从属
接口时，Linux 绑定会尝试把备份从属接口的 MAC 地址同步为与活动从属接口
相同的 MAC 地址，此时 Linux 绑定会失败。如果 VF 的 MAC 地址不是由 PF
设置的，则不会出现此问题。)

Traffic Is Not Being Passed Between VM and Client
-------------------------------------------------
(虚拟机与客户端之间无法传递流量)

You may not be able to pass traffic between a client system and a
Virtual Machine (VM) running on a separate host if the Virtual Function
(VF, or Virtual NIC) is not in trusted mode and spoof checking is enabled
on the VF. Note that this situation can occur in any combination of client,
host, and guest operating system. For information on how to set the VF to
trusted mode, refer to the section "VLAN Tag Packet Steering" in this
readme document. For information on setting spoof checking, refer to the
section "MAC and VLAN anti-spoofing feature" in this readme document.
(如果虚拟功能（VF，或虚拟网卡）未处于 trusted 模式且 VF 上启用了欺骗
检查（spoof checking），您可能无法在客户端系统与运行在另一台主机上的
虚拟机（VM）之间传递流量。注意：这种情况可能在客户端、主机与客户操作
系统的任意组合中出现。有关如何将 VF 设置为 trusted 模式，请参阅本自述
文档中的 "VLAN Tag Packet Steering" 一节；有关设置欺骗检查，请参阅本
自述文档中的 "MAC and VLAN anti-spoofing feature" 一节。)

Do not unload port driver if VF with active VM is bound to it
-------------------------------------------------------------
(绑定了运行中虚拟机的 VF 时不要卸载端口驱动程序)

Do not unload a port's driver if a Virtual Function (VF) with an active Virtual
Machine (VM) is bound to it. Doing so will cause the port to appear to hang.
Once the VM shuts down, or otherwise releases the VF, the command will complete.
(当绑定在端口上的虚拟功能（VF）带有正在运行的虚拟机（VM）时，请不要
卸载该端口的驱动程序；否则该端口看起来会像挂起一样。只有当 VM 关闭或以
其他方式释放该 VF 后，命令才会完成。)

Using four traffic classes fails
--------------------------------
(使用四个流量类会失败)

Do not try to reserve more than three traffic classes in the iavf driver. Doing
so will fail to set any traffic classes and will cause the driver to write
errors to stdout. Use a maximum of three queues to avoid this issue.
(不要尝试在 iavf 驱动程序中保留三个以上的流量类；否则将无法设置任何
流量类，并导致驱动程序向 stdout 写入错误信息。最多使用三个队列即可避免
此问题。)

Multiple log error messages on iavf driver removal
--------------------------------------------------
(卸载 iavf 驱动程序时出现多条日志错误消息)

If you have several VFs and you remove the iavf driver, several instances of
the following log errors are written to the log::
(如果您拥有多个 VF 并卸载了 iavf 驱动程序，日志中会写入多条如下所示的
日志错误：)

    Unable to send opcode 2 to PF, err I40E_ERR_QUEUE_EMPTY, aq_err ok
    Unable to send the message to VF 2 aq_err 12
    ARQ Overflow Error detected

Virtual machine does not get link
---------------------------------
(虚拟机无法获得链路)

If the virtual machine has more than one virtual port assigned to it, and those
virtual ports are bound to different physical ports, you may not get link on
all of the virtual ports. The following command may work around the issue::
(如果虚拟机被分配了多个虚拟端口，且这些虚拟端口绑定到不同的物理端口，
您可能无法在所有虚拟端口上都获得链路。以下命令或许可以规避该问题：)

    # ethtool -r <PF>

Where <PF> is the PF interface in the host, for example: p5p1. You may need to
run the command more than once to get link on all virtual ports.
(其中 <PF> 是主机中的 PF 接口，例如 p5p1。您可能需要多次运行该命令，
才能在所有虚拟端口上获得链路。)

MAC address of Virtual Function changes unexpectedly
----------------------------------------------------
(虚拟功能的 MAC 地址意外变化)

If a Virtual Function's MAC address is not assigned in the host, then the VF
(virtual function) driver will use a random MAC address. This random MAC
address may change each time the VF driver is reloaded. You can assign a static
MAC address in the host machine. This static MAC address will survive
a VF driver reload.
(如果虚拟功能的 MAC 地址未在主机中分配，VF（虚拟功能）驱动程序将使用
随机 MAC 地址。每次重新加载 VF 驱动程序时，该随机 MAC 地址都可能变化。
您可以在主机中分配一个静态 MAC 地址，这样即使 VF 驱动程序重新加载，该
静态 MAC 地址也会保留下来。)

Driver Buffer Overflow Fix
--------------------------
(驱动程序缓冲区溢出修复)

The fix to resolve CVE-2016-8105, referenced in Intel SA-00069
https://www.intel.com/content/www/us/en/security-center/advisory/intel-sa-00069.html
is included in this and future versions of the driver.
(解决 CVE-2016-8105 的修复（参见 Intel SA-00069：
https://www.intel.com/content/www/us/en/security-center/advisory/intel-sa-00069.html）
已包含在本版本及后续版本的驱动程序中。)

Multiple Interfaces on Same Ethernet Broadcast Network
------------------------------------------------------
(同一以太网广播网络上的多个接口)

Due to the default ARP behavior on Linux, it is not possible to have one system
on two IP networks in the same Ethernet broadcast domain (non-partitioned
switch) behave as expected. All Ethernet interfaces will respond to IP traffic
for any IP address assigned to the system. This results in unbalanced receive
traffic.
(由于 Linux 默认的 ARP 行为，无法让位于同一以太网广播域（未分区交换机）
中两个 IP 网络上的同一个系统按预期工作。所有以太网接口都会响应发往系统
所配置的任何 IP 地址的 IP 流量，从而导致接收流量不均衡。)

If you have multiple interfaces in a server, either turn on ARP filtering by
entering::
(如果服务器中有多个接口，可以输入以下命令开启 ARP 过滤：)

    # echo 1 > /proc/sys/net/ipv4/conf/all/arp_filter

NOTE:
  This setting is not saved across reboots. The configuration change can be
  made permanent by adding the following line to the file /etc/sysctl.conf::
  (注意：该设置在重启后不会保留。将下面一行添加到 /etc/sysctl.conf 文件
  中，可使配置更改永久生效：)

    net.ipv4.conf.all.arp_filter = 1

Another alternative is to install the interfaces in separate broadcast domains
(either in different switches or in a switch partitioned to VLANs).
(另一种做法是把各接口安装在不同的广播域中（使用不同的交换机，或使用
按 VLAN 分区的交换机）。)

Rx Page Allocation Errors
-------------------------
(Rx 页面分配错误)

'Page allocation failure. order:0' errors may occur under stress.
This is caused by the way the Linux kernel reports this stressed condition.
(在压力较大的情况下可能出现 'Page allocation failure. order:0' 错误。
这是由 Linux 内核报告这种高压力状况的方式所致。)


Support
=======
(支持)

For general information, go to the Intel support website at:
https://support.intel.com
(一般信息请访问 Intel 支持网站：https://support.intel.com)

If an issue is identified with the released source code on the supported kernel
with a supported adapter, email the specific information related to the issue
to intel-wired-lan@lists.osuosl.org.
(如果在受支持的内核上配合受支持的适配器使用已发布的源代码时发现了问题，
请将与该问题相关的具体信息发送电子邮件至 intel-wired-lan@lists.osuosl.org。)
