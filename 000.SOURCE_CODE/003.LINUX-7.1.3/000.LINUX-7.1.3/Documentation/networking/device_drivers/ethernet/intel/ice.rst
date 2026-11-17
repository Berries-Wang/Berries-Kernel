.. SPDX-License-Identifier: GPL-2.0+

======================================================================================================================
Linux Base Driver for the Intel(R) Ethernet Controller 800 Series(Intel(R) 以太网控制器 800 系列的 Linux 基础驱动程序)
======================================================================================================================

Intel ice Linux driver.
(Intel ice Linux 驱动程序。)
Copyright(c) 2018-2021 Intel Corporation.

Contents(目录)
==============

- Overview
  (概述)
- Identifying Your Adapter
  (识别你的适配器)
- Important Notes
  (重要说明)
- Additional Features & Configurations
  (附加功能与配置)
- Performance Optimization
  (性能优化)


The associated Virtual Function (VF) driver for this driver is iavf.
(与此驱动程序对应的虚拟功能 (VF) 驱动程序是 iavf。)

Driver information can be obtained using ethtool and lspci.
(可以使用 ethtool 和 lspci 获取驱动程序信息。)

For questions related to hardware requirements, refer to the documentation
supplied with your Intel adapter. All hardware requirements listed apply to use
with Linux.
(有关硬件要求的相关问题，请参阅 Intel 适配器随附的文档。所列出的所有硬件要求均适用于
Linux。)

This driver supports XDP (Express Data Path) and AF_XDP zero-copy. Note that
XDP is blocked for frame sizes larger than 3KB.
(此驱动程序支持 XDP (Express Data Path) 和 AF_XDP 零拷贝。注意，帧大小超过 3KB 时 XDP
将被禁用。)


Identifying Your Adapter(识别你的适配器)
========================================
For information on how to identify your adapter, and for the latest Intel
network drivers, refer to the Intel Support website:
https://www.intel.com/support
(有关如何识别你的适配器的信息以及最新的 Intel 网络驱动程序，请参阅 Intel 支持网站：
https://www.intel.com/support)


Important Notes(重要说明)
=========================

Packet drops may occur under receive stress(接收压力下可能出现丢包)
-------------------------------------------------------------------
Devices based on the Intel(R) Ethernet Controller 800 Series are designed to
tolerate a limited amount of system latency during PCIe and DMA transactions.
If these transactions take longer than the tolerated latency, it can impact the
length of time the packets are buffered in the device and associated memory,
which may result in dropped packets. These packets drops typically do not have
a noticeable impact on throughput and performance under standard workloads.
(基于 Intel(R) 以太网控制器 800 系列的设备在设计上可以容忍 PCIe 和 DMA 事务期间有限的
系统延迟。如果这些事务耗时超过可容忍的延迟，就会影响数据包在设备及关联内存中被缓冲的
时间长度，从而可能导致丢包。在标准工作负载下，这些丢包通常不会对吞吐量和性能产生明显
影响。)

If these packet drops appear to affect your workload, the following may improve
the situation:
(如果这些丢包似乎影响了你的工作负载，以下措施或许可以改善这种情况：)

1) Make sure that your system's physical memory is in a high-performance
   configuration, as recommended by the platform vendor. A common
   recommendation is for all channels to be populated with a single DIMM
   module.
   (确保系统物理内存处于平台供应商建议的高性能配置。一个常见的建议是所有内存通道都只
   插入一条 DIMM 内存条。)
2) In your system's BIOS/UEFI settings, select the "Performance" profile.
   (在系统的 BIOS/UEFI 设置中，选择 "Performance" 配置档。)
3) Your distribution may provide tools like "tuned," which can help tweak
   kernel settings to achieve better standard settings for different workloads.
   (你的发行版可能提供 "tuned" 之类的工具，它们有助于调整内核设置，以便针对不同的工作
   负载获得更好的标准设置。)


Configuring SR-IOV for improved network security(配置 SR-IOV 以增强网络安全性)
------------------------------------------------------------------------------
In a virtualized environment, on Intel(R) Ethernet Network Adapters that
support SR-IOV, the virtual function (VF) may be subject to malicious behavior.
Software-generated layer two frames, like IEEE 802.3x (link flow control), IEEE
802.1Qbb (priority based flow-control), and others of this type, are not
expected and can throttle traffic between the host and the virtual switch,
reducing performance. To resolve this issue, and to ensure isolation from
unintended traffic streams, configure all SR-IOV enabled ports for VLAN tagging
from the administrative interface on the PF. This configuration allows
unexpected, and potentially malicious, frames to be dropped.
(在虚拟化环境中，对于支持 SR-IOV 的 Intel(R) 以太网网络适配器，其虚拟功能 (VF) 可能会
遭受恶意行为。软件生成的二层帧，例如 IEEE 802.3x (链路流控制)、IEEE 802.1Qbb (基于优
先级的流控制) 以及其他类似类型的帧，是不被预期的，它们可能抑制主机与虚拟交换机之间的
流量，降低性能。为解决此问题并确保与意外流量流的隔离，请在 PF 的管理接口上为所有启用
了 SR-IOV 的端口配置 VLAN 打标。此配置允许丢弃意外的以及潜在恶意的帧。)

See "Configuring VLAN Tagging on SR-IOV Enabled Adapter Ports" later in this
README for configuration instructions.
(配置说明请参阅本 README 后文的 "Configuring VLAN Tagging on SR-IOV Enabled Adapter
Ports" 一节。)


Do not unload port driver if VF with active VM is bound to it(若有活动虚拟机的 VF 绑定在端口上，请勿卸载端口驱动程序)
---------------------------------------------------------------------------------------------------------------------
Do not unload a port's driver if a Virtual Function (VF) with an active Virtual
Machine (VM) is bound to it. Doing so will cause the port to appear to hang.
Once the VM shuts down, or otherwise releases the VF, the command will
complete.
(如果绑定到端口的是带有运行中虚拟机 (VM) 的虚拟功能 (VF)，请勿卸载该端口的驱动程序。
否则会导致端口看起来像是挂起了。一旦虚拟机关闭或以其他方式释放该 VF，该命令才会完
成。)


Additional Features and Configurations(附加功能与配置)
======================================================

ethtool(ethtool)
----------------
The driver utilizes the ethtool interface for driver configuration and
diagnostics, as well as displaying statistical information. The latest ethtool
version is required for this functionality. Download it at:
https://kernel.org/pub/software/network/ethtool/
(此驱动程序利用 ethtool 接口进行驱动程序配置与诊断，以及显示统计信息。该功能需要最新
版本的 ethtool。可从以下网址下载：https://kernel.org/pub/software/network/ethtool/)

NOTE: The rx_bytes value of ethtool does not match the rx_bytes value of
Netdev, due to the 4-byte CRC being stripped by the device. The difference
between the two rx_bytes values will be 4 x the number of Rx packets. For
example, if Rx packets are 10 and Netdev (software statistics) displays
rx_bytes as "X", then ethtool (hardware statistics) will display rx_bytes as
"X+40" (4 bytes CRC x 10 packets).
(注意：由于设备会剥离 4 字节的 CRC，ethtool 的 rx_bytes 值与 Netdev 的 rx_bytes 值不
一致。两个 rx_bytes 值之差将是 4 x 接收数据包数。例如，如果接收数据包为 10 个，且
Netdev (软件统计) 显示的 rx_bytes 为 "X"，则 ethtool (硬件统计) 显示的 rx_bytes 将为
"X+40" (4 字节 CRC x 10 个数据包)。)

ethtool reset(ethtool 复位)
---------------------------
The driver supports 3 types of resets:
(此驱动程序支持 3 种复位类型：)

- PF reset - resets only components associated with the given PF, does not
  impact other PFs
  (PF reset - 仅复位与给定 PF 关联的组件，不影响其他 PF)

- CORE reset - whole adapter is affected, reset all PFs
  (CORE reset - 整个适配器受影响，复位所有 PF)

- GLOBAL reset - same as CORE but mac and phy components are also reinitialized
  (GLOBAL reset - 与 CORE 相同，但 mac 和 phy 组件也会重新初始化)

These are mapped to ethtool reset flags as follow:
(它们与 ethtool 复位标志的对应关系如下：)

- PF reset:
  (PF 复位：)

  # ethtool --reset <ethX> irq dma filter offload

- CORE reset:
  (CORE 复位：)

  # ethtool --reset <ethX> irq-shared dma-shared filter-shared offload-shared \
  ram-shared

- GLOBAL reset:
  (GLOBAL 复位：)

  # ethtool --reset <ethX> irq-shared dma-shared filter-shared offload-shared \
  mac-shared phy-shared ram-shared

In switchdev mode you can reset a VF using port representor:
(在 switchdev 模式下，可以使用端口 representor 复位 VF：)

  # ethtool --reset <repr> irq dma filter offload


Viewing Link Messages(查看链路消息)
-----------------------------------
Link messages will not be displayed to the console if the distribution is
restricting system messages. In order to see network driver link messages on
your console, set dmesg to eight by entering the following::
(如果发行版限制了系统消息，链路消息将不会显示到控制台。要在控制台上查看网络驱动程序
的链路消息，请输入以下命令将 dmesg 设为 8：)

  # dmesg -n 8

NOTE: This setting is not saved across reboots.
(注意：此设置在重启后不会保存。)


Dynamic Device Personalization(动态设备个性化)
----------------------------------------------
Dynamic Device Personalization (DDP) allows you to change the packet processing
pipeline of a device by applying a profile package to the device at runtime.
Profiles can be used to, for example, add support for new protocols, change
existing protocols, or change default settings. DDP profiles can also be rolled
back without rebooting the system.
(动态设备个性化 (DDP) 允许你在运行时向设备应用配置包 (profile package)，从而更改设备
的数据包处理管线。配置包可用于例如添加对新协议的支持、修改现有协议或更改默认设置。DDP
配置包还可以在不重启系统的情况下回滚。)

The DDP package loads during device initialization. The driver looks for
``intel/ice/ddp/ice.pkg`` in your firmware root (typically ``/lib/firmware/``
or ``/lib/firmware/updates/``) and checks that it contains a valid DDP package
file.
(DDP 包在设备初始化期间加载。驱动程序会在固件根目录 (通常是 ``/lib/firmware/`` 或
``/lib/firmware/updates/``) 中查找 ``intel/ice/ddp/ice.pkg``，并检查其中是否包含有效
的 DDP 包文件。)

NOTE: Your distribution should likely have provided the latest DDP file, but if
ice.pkg is missing, you can find it in the linux-firmware repository or from
intel.com.
(注意：你的发行版应该已经提供了最新的 DDP 文件，但如果缺少 ice.pkg，可以在
linux-firmware 仓库或 intel.com 上找到。)

If the driver is unable to load the DDP package, the device will enter Safe
Mode. Safe Mode disables advanced and performance features and supports only
basic traffic and minimal functionality, such as updating the NVM or
downloading a new driver or DDP package. Safe Mode only applies to the affected
physical function and does not impact any other PFs. See the "Intel(R) Ethernet
Adapters and Devices User Guide" for more details on DDP and Safe Mode.
(如果驱动程序无法加载 DDP 包，设备将进入安全模式 (Safe Mode)。安全模式会禁用高级和性
能特性，仅支持基本流量和最基本的功能，例如更新 NVM 或下载新的驱动程序或 DDP 包。安全
模式仅适用于受影响的物理功能，不会影响任何其他 PF。有关 DDP 和安全模式的更多细节，请
参阅 "Intel(R) Ethernet Adapters and Devices User Guide"。)

NOTES:
(注意：)

- If you encounter issues with the DDP package file, you may need to download
  an updated driver or DDP package file. See the log messages for more
  information.
  (如果遇到 DDP 包文件的问题，可能需要下载更新的驱动程序或 DDP 包文件。更多信息请查
  看日志消息。)

- The ice.pkg file is a symbolic link to the default DDP package file.
  (ice.pkg 文件是指向默认 DDP 包文件的符号链接。)

- You cannot update the DDP package if any PF drivers are already loaded. To
  overwrite a package, unload all PFs and then reload the driver with the new
  package.
  (如果任何 PF 驱动程序已经加载，则无法更新 DDP 包。要覆盖一个包，请先卸载所有 PF，
  然后使用新的包重新加载驱动程序。)

- Only the first loaded PF per device can download a package for that device.
  (每个设备只有第一个加载的 PF 可以为该设备下载包。)

You can install specific DDP package files for different physical devices in
the same system. To install a specific DDP package file:
(可以在同一系统中为不同的物理设备安装特定的 DDP 包文件。要安装特定的 DDP 包文件：)

1. Download the DDP package file you want for your device.
   (下载你的设备所需的 DDP 包文件。)

2. Rename the file ice-xxxxxxxxxxxxxxxx.pkg, where 'xxxxxxxxxxxxxxxx' is the
   unique 64-bit PCI Express device serial number (in hex) of the device you
   want the package downloaded on. The filename must include the complete
   serial number (including leading zeros) and be all lowercase. For example,
   if the 64-bit serial number is b887a3ffffca0568, then the file name would be
   ice-b887a3ffffca0568.pkg.
   (将文件重命名为 ice-xxxxxxxxxxxxxxxx.pkg，其中 'xxxxxxxxxxxxxxxx' 是你希望下载该
   包的设备的唯一 64 位 PCI Express 设备序列号 (十六进制)。文件名必须包含完整的序列
   号 (包括前导零) 且全部为小写。例如，如果 64 位序列号为 b887a3ffffca0568，则文件名
   应为 ice-b887a3ffffca0568.pkg。)

   To find the serial number from the PCI bus address, you can use the
   following command::
   (要根据 PCI 总线地址查找序列号，可以使用以下命令：)

     # lspci -vv -s af:00.0 | grep -i Serial
     Capabilities: [150 v1] Device Serial Number b8-87-a3-ff-ff-ca-05-68

   You can use the following command to format the serial number without the
   dashes::
   (可以使用以下命令去掉序列号中的连字符：)

     # lspci -vv -s af:00.0 | grep -i Serial | awk '{print $7}' | sed s/-//g
     b887a3ffffca0568

3. Copy the renamed DDP package file to
   ``/lib/firmware/updates/intel/ice/ddp/``. If the directory does not yet
   exist, create it before copying the file.
   (将重命名后的 DDP 包文件复制到 ``/lib/firmware/updates/intel/ice/ddp/``。如果该
   目录尚不存在，请先创建目录再复制文件。)

4. Unload all of the PFs on the device.
   (卸载设备上的所有 PF。)

5. Reload the driver with the new package.
   (使用新的包重新加载驱动程序。)

NOTE: The presence of a device-specific DDP package file overrides the loading
of the default DDP package file (ice.pkg).
(注意：设备专用 DDP 包文件的存在会覆盖默认 DDP 包文件 (ice.pkg) 的加载。)


Intel(R) Ethernet Flow Director(Intel(R) 以太网 Flow Director)
--------------------------------------------------------------
The Intel Ethernet Flow Director performs the following tasks:
(Intel Ethernet Flow Director 执行以下任务：)

- Directs receive packets according to their flows to different queues
  (根据流将接收的数据包引导到不同的队列)
- Enables tight control on routing a flow in the platform
  (对平台中流的路由提供严格控制)
- Matches flows and CPU cores for flow affinity
  (将流与 CPU 核心匹配以实现流亲和性)

NOTE: This driver supports the following flow types:
(注意：此驱动程序支持以下流类型：)

- IPv4
- TCPv4
- UDPv4
- SCTPv4
- IPv6
- TCPv6
- UDPv6
- SCTPv6

Each flow type supports valid combinations of IP addresses (source or
destination) and UDP/TCP/SCTP ports (source and destination). You can supply
only a source IP address, a source IP address and a destination port, or any
combination of one or more of these four parameters.
(每种流类型都支持 IP 地址 (源或目的) 与 UDP/TCP/SCTP 端口 (源和目的) 的有效组合。你
可以只提供源 IP 地址、源 IP 地址加目的端口，或者这四个参数中一个或多个的任意组合。)

NOTE: This driver allows you to filter traffic based on a user-defined flexible
two-byte pattern and offset by using the ethtool user-def and mask fields. Only
L3 and L4 flow types are supported for user-defined flexible filters. For a
given flow type, you must clear all Intel Ethernet Flow Director filters before
changing the input set (for that flow type).
(注意：此驱动程序允许你使用 ethtool 的 user-def 和 mask 字段，基于用户定义的灵活二字
节模式和偏移量来过滤流量。用户定义的灵活过滤器仅支持 L3 和 L4 流类型。对于给定的流类
型，在更改输入集 (针对该流类型) 之前，必须清除所有 Intel Ethernet Flow Director 过滤
器。)


Flow Director Filters(Flow Director 过滤器)
-------------------------------------------
Flow Director filters are used to direct traffic that matches specified
characteristics. They are enabled through ethtool's ntuple interface. To enable
or disable the Intel Ethernet Flow Director and these filters::
(Flow Director 过滤器用于引导匹配指定特征的流量。它们通过 ethtool 的 ntuple 接口启
用。要启用或禁用 Intel Ethernet Flow Director 及这些过滤器：)

  # ethtool -K <ethX> ntuple <off|on>

NOTE: When you disable ntuple filters, all the user programmed filters are
flushed from the driver cache and hardware. All needed filters must be re-added
when ntuple is re-enabled.
(注意：禁用 ntuple 过滤器时，所有用户编程的过滤器都会从驱动程序缓存和硬件中清除。重
新启用 ntuple 时，必须重新添加所有需要的过滤器。)

To display all of the active filters::
(要显示所有活动的过滤器：)

  # ethtool -u <ethX>

To add a new filter::
(要添加新过滤器：)

  # ethtool -U <ethX> flow-type <type> src-ip <ip> [m <ip_mask>] dst-ip <ip>
  [m <ip_mask>] src-port <port> [m <port_mask>] dst-port <port> [m <port_mask>]
  action <queue>

  Where:
    <ethX> - the Ethernet device to program
    <type> - can be ip4, tcp4, udp4, sctp4, ip6, tcp6, udp6, sctp6
    <ip> - the IP address to match on
    <ip_mask> - the IPv4 address to mask on
              NOTE: These filters use inverted masks.
    <port> - the port number to match on
    <port_mask> - the 16-bit integer for masking
              NOTE: These filters use inverted masks.
    <queue> - the queue to direct traffic toward (-1 discards the
              matched traffic)

To delete a filter::
(要删除过滤器：)

  # ethtool -U <ethX> delete <N>

  Where <N> is the filter ID displayed when printing all the active filters,
  and may also have been specified using "loc <N>" when adding the filter.

EXAMPLES:
(示例：)

To add a filter that directs packet to queue 2::
(要添加一个将数据包引导到队列 2 的过滤器：)

  # ethtool -U <ethX> flow-type tcp4 src-ip 192.168.10.1 dst-ip \
  192.168.10.2 src-port 2000 dst-port 2001 action 2 [loc 1]

To set a filter using only the source and destination IP address::
(要仅使用源和目的 IP 地址设置过滤器：)

  # ethtool -U <ethX> flow-type tcp4 src-ip 192.168.10.1 dst-ip \
  192.168.10.2 action 2 [loc 1]

To set a filter based on a user-defined pattern and offset::
(要基于用户定义的模式和偏移量设置过滤器：)

  # ethtool -U <ethX> flow-type tcp4 src-ip 192.168.10.1 dst-ip \
  192.168.10.2 user-def 0x4FFFF action 2 [loc 1]

  where the value of the user-def field contains the offset (4 bytes) and
  the pattern (0xffff).

To match TCP traffic sent from 192.168.0.1, port 5300, directed to 192.168.0.5,
port 80, and then send it to queue 7::
(要匹配从 192.168.0.1 端口 5300 发往 192.168.0.5 端口 80 的 TCP 流量，并将其发送到队
列 7：)

  # ethtool -U enp130s0 flow-type tcp4 src-ip 192.168.0.1 dst-ip 192.168.0.5
  src-port 5300 dst-port 80 action 7

To add a TCPv4 filter with a partial mask for a source IP subnet::
(要为源 IP 子网添加一个带部分掩码的 TCPv4 过滤器：)

  # ethtool -U <ethX> flow-type tcp4 src-ip 192.168.0.0 m 0.255.255.255 dst-ip
  192.168.5.12 src-port 12600 dst-port 31 action 12

NOTES:
(注意：)

For each flow-type, the programmed filters must all have the same matching
input set. For example, issuing the following two commands is acceptable::
(对于每种流类型，已编程的过滤器必须都使用相同的匹配输入集。例如，发出以下两条命令是
可以接受的：)

  # ethtool -U enp130s0 flow-type ip4 src-ip 192.168.0.1 src-port 5300 action 7
  # ethtool -U enp130s0 flow-type ip4 src-ip 192.168.0.5 src-port 55 action 10

Issuing the next two commands, however, is not acceptable, since the first
specifies src-ip and the second specifies dst-ip::
(然而，发出接下来的两条命令是不可接受的，因为第一条指定了 src-ip，而第二条指定了
dst-ip：)

  # ethtool -U enp130s0 flow-type ip4 src-ip 192.168.0.1 src-port 5300 action 7
  # ethtool -U enp130s0 flow-type ip4 dst-ip 192.168.0.5 src-port 55 action 10

The second command will fail with an error. You may program multiple filters
with the same fields, using different values, but, on one device, you may not
program two tcp4 filters with different matching fields.
(第二条命令将报错失败。你可以使用相同字段、不同取值来编程多个过滤器，但在一个设备
上，不能编程两个匹配字段不同的 tcp4 过滤器。)

The ice driver does not support matching on a subportion of a field, thus
partial mask fields are not supported.
(ice 驱动程序不支持匹配字段的子部分，因此不支持部分掩码字段。)


Flex Byte Flow Director Filters(Flex Byte Flow Director 过滤器)
---------------------------------------------------------------
The driver also supports matching user-defined data within the packet payload.
This flexible data is specified using the "user-def" field of the ethtool
command in the following way:
(此驱动程序还支持匹配数据包载荷中用户定义的数据。此灵活数据通过 ethtool 命令的
"user-def" 字段按如下方式指定：)

.. table::

    ============================== ============================
    ``31    28    24    20    16`` ``15    12    8    4    0``
    ``offset into packet payload`` ``2 bytes of flexible data``
    ============================== ============================

For example,
(例如，)

::

  ... user-def 0x4FFFF ...

tells the filter to look 4 bytes into the payload and match that value against
0xFFFF. The offset is based on the beginning of the payload, and not the
beginning of the packet. Thus
(告诉过滤器深入载荷 4 字节，并将该值与 0xFFFF 进行匹配。偏移量基于载荷的开头，而不是
数据包的开头。因此)

::

  flow-type tcp4 ... user-def 0x8BEAF ...

would match TCP/IPv4 packets which have the value 0xBEAF 8 bytes into the
TCP/IPv4 payload.
(将匹配在 TCP/IPv4 载荷第 8 字节处具有值 0xBEAF 的 TCP/IPv4 数据包。)

Note that ICMP headers are parsed as 4 bytes of header and 4 bytes of payload.
Thus to match the first byte of the payload, you must actually add 4 bytes to
the offset. Also note that ip4 filters match both ICMP frames as well as raw
(unknown) ip4 frames, where the payload will be the L3 payload of the IP4
frame.
(注意，ICMP 头被解析为 4 字节头加 4 字节载荷。因此要匹配载荷的第一个字节，实际上必须
在偏移量上加 4。还要注意，ip4 过滤器既匹配 ICMP 帧，也匹配原始 (未知) 的 ip4 帧，此
时载荷将是 IP4 帧的 L3 载荷。)

The maximum offset is 64. The hardware will only read up to 64 bytes of data
from the payload. The offset must be even because the flexible data is 2 bytes
long and must be aligned to byte 0 of the packet payload.
(最大偏移量为 64。硬件最多只从载荷中读取 64 字节的数据。偏移量必须是偶数，因为灵活数
据的长度为 2 字节，必须与数据包载荷的字节 0 对齐。)

The user-defined flexible offset is also considered part of the input set and
cannot be programmed separately for multiple filters of the same type. However,
the flexible data is not part of the input set and multiple filters may use the
same offset but match against different data.
(用户定义的灵活偏移量也被视为输入集的一部分，同一类型的多个过滤器不能对其分别编程。
不过，灵活数据不属于输入集，多个过滤器可以使用相同的偏移量，但匹配不同的数据。)


RSS Hash Flow(RSS 哈希流)
-------------------------
Allows you to set the hash bytes per flow type and any combination of one or
more options for Receive Side Scaling (RSS) hash byte configuration.
(允许你为每种流类型设置哈希字节，以及为接收侧扩展 (RSS) 哈希字节配置设置一个或多个选
项的任意组合。)

::

  # ethtool -N <ethX> rx-flow-hash <type> <option>

  Where <type> is:
    tcp4    signifying TCP over IPv4
    udp4    signifying UDP over IPv4
    gtpc4   signifying GTP-C over IPv4
    gtpc4t  signifying GTP-C (include TEID) over IPv4
    gtpu4   signifying GTP-U over IPV4
    gtpu4e  signifying GTP-U and Extension Header over IPV4
    gtpu4u  signifying GTP-U PSC Uplink over IPV4
    gtpu4d  signifying GTP-U PSC Downlink over IPV4
    tcp6    signifying TCP over IPv6
    udp6    signifying UDP over IPv6
    gtpc6   signifying GTP-C over IPv6
    gtpc6t  signifying GTP-C (include TEID) over IPv6
    gtpu6   signifying GTP-U over IPV6
    gtpu6e  signifying GTP-U and Extension Header over IPV6
    gtpu6u  signifying GTP-U PSC Uplink over IPV6
    gtpu6d  signifying GTP-U PSC Downlink over IPV6
  And <option> is one or more of:
    s     Hash on the IP source address of the Rx packet.
    d     Hash on the IP destination address of the Rx packet.
    f     Hash on bytes 0 and 1 of the Layer 4 header of the Rx packet.
    n     Hash on bytes 2 and 3 of the Layer 4 header of the Rx packet.
    e     Hash on GTP Packet on TEID (4bytes) of the Rx packet.


Accelerated Receive Flow Steering (aRFS)(加速接收流导向 (aRFS))
---------------------------------------------------------------
Devices based on the Intel(R) Ethernet Controller 800 Series support
Accelerated Receive Flow Steering (aRFS) on the PF. aRFS is a load-balancing
mechanism that allows you to direct packets to the same CPU where an
application is running or consuming the packets in that flow.
(基于 Intel(R) 以太网控制器 800 系列的设备在 PF 上支持加速接收流导向 (aRFS)。aRFS 是
一种负载均衡机制，允许你将数据包引导到应用程序正在运行或正在消费该流中数据包的同一个
CPU 上。)

NOTES:
(注意：)

- aRFS requires that ntuple filtering is enabled via ethtool.
  (aRFS 要求通过 ethtool 启用 ntuple 过滤。)
- aRFS support is limited to the following packet types:
  (aRFS 仅支持以下数据包类型：)

    - TCP over IPv4 and IPv6
      (基于 IPv4 和 IPv6 的 TCP)
    - UDP over IPv4 and IPv6
      (基于 IPv4 和 IPv6 的 UDP)
    - Nonfragmented packets
      (未分片的数据包)

- aRFS only supports Flow Director filters, which consist of the
  source/destination IP addresses and source/destination ports.
  (aRFS 仅支持 Flow Director 过滤器，即由源/目的 IP 地址和源/目的端口构成的过滤器。)
- aRFS and ethtool's ntuple interface both use the device's Flow Director. aRFS
  and ntuple features can coexist, but you may encounter unexpected results if
  there's a conflict between aRFS and ntuple requests. See "Intel(R) Ethernet
  Flow Director" for additional information.
  (aRFS 和 ethtool 的 ntuple 接口都使用设备的 Flow Director。aRFS 与 ntuple 特性可以
  共存，但如果 aRFS 请求与 ntuple 请求之间存在冲突，可能会遇到意外的结果。更多信息参
  见 "Intel(R) Ethernet Flow Director" 一节。)

To set up aRFS:
(设置 aRFS 的步骤：)

1. Enable the Intel Ethernet Flow Director and ntuple filters using ethtool.
   (使用 ethtool 启用 Intel Ethernet Flow Director 和 ntuple 过滤器。)

::

   # ethtool -K <ethX> ntuple on

2. Set up the number of entries in the global flow table. For example:
   (设置全局流表的表项数。例如：)

::

   # NUM_RPS_ENTRIES=16384
   # echo $NUM_RPS_ENTRIES > /proc/sys/net/core/rps_sock_flow_entries

3. Set up the number of entries in the per-queue flow table. For example:
   (设置每队列流表的表项数。例如：)

::

   # NUM_RX_QUEUES=64
   # for file in /sys/class/net/$IFACE/queues/rx-*/rps_flow_cnt; do
   # echo $(($NUM_RPS_ENTRIES/$NUM_RX_QUEUES)) > $file;
   # done

4. Disable the IRQ balance daemon (this is only a temporary stop of the service
   until the next reboot).
   (禁用 IRQ balance 守护进程 (这只是暂时停止该服务，直到下次重启为止)。)

::

   # systemctl stop irqbalance

5. Configure the interrupt affinity.
   (配置中断亲和性。)

   See ``/Documentation/core-api/irq/irq-affinity.rst``
   (参见 ``/Documentation/core-api/irq/irq-affinity.rst``)


To disable aRFS using ethtool::
(要使用 ethtool 禁用 aRFS：)

  # ethtool -K <ethX> ntuple off

NOTE: This command will disable ntuple filters and clear any aRFS filters in
software and hardware.
(注意：此命令将禁用 ntuple 过滤器，并清除软件和硬件中的所有 aRFS 过滤器。)

Example Use Case:
(使用示例：)

1. Set the server application on the desired CPU (e.g., CPU 4).
   (将服务器应用程序设置到所需的 CPU 上 (例如 CPU 4)。)

::

   # taskset -c 4 netserver

2. Use netperf to route traffic from the client to CPU 4 on the server with
   aRFS configured. This example uses TCP over IPv4.
   (在配置了 aRFS 的情况下，使用 netperf 将客户端的流量引导到服务器上的 CPU 4。本例
   使用基于 IPv4 的 TCP。)

::

   # netperf -H <Host IPv4 Address> -t TCP_STREAM


Enabling Virtual Functions (VFs)(启用虚拟功能 (VF))
---------------------------------------------------
Use sysfs to enable virtual functions (VF).
(使用 sysfs 启用虚拟功能 (VF)。)

For example, you can create 4 VFs as follows::
(例如，可以按如下方式创建 4 个 VF：)

  # echo 4 > /sys/class/net/<ethX>/device/sriov_numvfs

To disable VFs, write 0 to the same file::
(要禁用 VF，向同一文件写入 0：)

  # echo 0 > /sys/class/net/<ethX>/device/sriov_numvfs

The maximum number of VFs for the ice driver is 256 total (all ports). To check
how many VFs each PF supports, use the following command::
(ice 驱动程序支持的 VF 最大总数为 256 个 (所有端口)。要检查每个 PF 支持多少个 VF，可以
使用以下命令：)

  # cat /sys/class/net/<ethX>/device/sriov_totalvfs

Note: You cannot use SR-IOV when link aggregation (LAG)/bonding is active, and
vice versa. To enforce this, the driver checks for this mutual exclusion.
(注意：当链路聚合 (LAG)/绑定 (bonding) 处于活动状态时，不能使用 SR-IOV，反之亦然。为
了强制实施这一点，驱动程序会检查这种互斥关系。)


Displaying VF Statistics on the PF(在 PF 上显示 VF 统计信息)
------------------------------------------------------------
Use the following command to display the statistics for the PF and its VFs::
(使用以下命令显示 PF 及其 VF 的统计信息：)

  # ip -s link show dev <ethX>

NOTE: The output of this command can be very large due to the maximum number of
possible VFs.
(注意：由于可能的 VF 数量很大，此命令的输出可能非常庞大。)

The PF driver will display a subset of the statistics for the PF and for all
VFs that are configured. The PF will always print a statistics block for each
of the possible VFs, and it will show zero for all unconfigured VFs.
(PF 驱动程序将显示 PF 以及所有已配置 VF 的统计信息子集。PF 总会为每个可能的 VF 打印一
个统计信息块，并对所有未配置的 VF 显示零。)


Configuring VLAN Tagging on SR-IOV Enabled Adapter Ports(在启用了 SR-IOV 的适配器端口上配置 VLAN 打标)
------------------------------------------------------------------------------------------------------
To configure VLAN tagging for the ports on an SR-IOV enabled adapter, use the
following command. The VLAN configuration should be done before the VF driver
is loaded or the VM is booted. The VF is not aware of the VLAN tag being
inserted on transmit and removed on received frames (sometimes called "port
VLAN" mode).
(要为启用了 SR-IOV 的适配器上的端口配置 VLAN 打标，请使用以下命令。VLAN 配置应在加载
VF 驱动程序或引导 VM 之前完成。VF 不会感知到在发送帧时插入、在接收帧时移除的 VLAN 标
签 (有时称为 "port VLAN" 模式)。)

::

  # ip link set dev <ethX> vf <id> vlan <vlan id>

For example, the following will configure PF eth0 and the first VF on VLAN 10::
(例如，以下命令将把 PF eth0 和第一个 VF 配置到 VLAN 10 上：)

  # ip link set dev eth0 vf 0 vlan 10


Enabling a VF link if the port is disconnected(端口断开时启用 VF 链路)
----------------------------------------------------------------------
If the physical function (PF) link is down, you can force link up (from the
host PF) on any virtual functions (VF) bound to the PF.
(如果物理功能 (PF) 的链路断开，你可以 (从主机的 PF 侧) 强制将绑定到该 PF 的任何虚拟
功能 (VF) 的链路置为 up。)

For example, to force link up on VF 0 bound to PF eth0::
(例如，强制将绑定到 PF eth0 的 VF 0 的链路置为 up：)

  # ip link set eth0 vf 0 state enable

Note: If the command does not work, it may not be supported by your system.
(注意：如果该命令不起作用，可能是你的系统不支持此功能。)


Setting the MAC Address for a VF(为 VF 设置 MAC 地址)
-----------------------------------------------------
To change the MAC address for the specified VF::
(要更改指定 VF 的 MAC 地址：)

  # ip link set <ethX> vf 0 mac <address>

For example::
(例如：)

  # ip link set <ethX> vf 0 mac 00:01:02:03:04:05

This setting lasts until the PF is reloaded.
(此设置会一直保持到 PF 重新加载为止。)

NOTE: Assigning a MAC address for a VF from the host will disable any
subsequent requests to change the MAC address from within the VM. This is a
security feature. The VM is not aware of this restriction, so if this is
attempted in the VM, it will trigger MDD events.
(注意：从主机为 VF 分配 MAC 地址将禁用之后从 VM 内部更改 MAC 地址的任何请求。这是一
项安全特性。VM 并不知道这一限制，因此如果在 VM 中尝试这样做，将触发 MDD 事件。)


Trusted VFs and VF Promiscuous Mode(受信任的 VF 与 VF 混杂模式)
---------------------------------------------------------------
This feature allows you to designate a particular VF as trusted and allows that
trusted VF to request selective promiscuous mode on the Physical Function (PF).
(此特性允许你将特定的 VF 指定为受信任的，并允许该受信任的 VF 在物理功能 (PF) 上请求
选择性混杂模式。)

To set a VF as trusted or untrusted, enter the following command in the
Hypervisor::
(要将 VF 设置为受信任或不受信任，请在 Hypervisor 中输入以下命令：)

  # ip link set dev <ethX> vf 1 trust [on|off]

NOTE: It's important to set the VF to trusted before setting promiscuous mode.
If the VM is not trusted, the PF will ignore promiscuous mode requests from the
VF. If the VM becomes trusted after the VF driver is loaded, you must make a
new request to set the VF to promiscuous.
(注意：在设置混杂模式之前，先将 VF 设置为受信任非常重要。如果 VM 不受信任，PF 将忽略
来自 VF 的混杂模式请求。如果 VM 是在 VF 驱动程序加载之后才变为受信任的，你必须重新
发起请求将 VF 设置为混杂模式。)

Once the VF is designated as trusted, use the following commands in the VM to
set the VF to promiscuous mode.
(将 VF 指定为受信任后，在 VM 中使用以下命令将 VF 设置为混杂模式。)

For promiscuous all::
(对于全混杂模式：)

  # ip link set <ethX> promisc on
  Where <ethX> is a VF interface in the VM

For promiscuous Multicast::
(对于组播混杂模式：)

  # ip link set <ethX> allmulticast on
  Where <ethX> is a VF interface in the VM

NOTE: By default, the ethtool private flag vf-true-promisc-support is set to
"off," meaning that promiscuous mode for the VF will be limited. To set the
promiscuous mode for the VF to true promiscuous and allow the VF to see all
ingress traffic, use the following command::
(注意：默认情况下，ethtool 私有标志 vf-true-promisc-support 被设置为 "off"，这意味着
VF 的混杂模式将是受限的。要将 VF 的混杂模式设置为真混杂 (true promiscuous) 并允许 VF
看到所有入站流量，请使用以下命令：)

  # ethtool --set-priv-flags <ethX> vf-true-promisc-support on

The vf-true-promisc-support private flag does not enable promiscuous mode;
rather, it designates which type of promiscuous mode (limited or true) you will
get when you enable promiscuous mode using the ip link commands above. Note
that this is a global setting that affects the entire device. However, the
vf-true-promisc-support private flag is only exposed to the first PF of the
device. The PF remains in limited promiscuous mode regardless of the
vf-true-promisc-support setting.
(vf-true-promisc-support 私有标志并不会启用混杂模式；它只是指定当你使用上面的 ip link
命令启用混杂模式时，将得到哪种类型的混杂模式 (受限的或真的)。注意，这是一个影响整个
设备的全局设置。不过，vf-true-promisc-support 私有标志仅向设备的第一个 PF 暴露。无论
vf-true-promisc-support 设置为何值，PF 自身始终保持受限混杂模式。)

Next, add a VLAN interface on the VF interface. For example::
(接下来，在 VF 接口上添加一个 VLAN 接口。例如：)

  # ip link add link eth2 name eth2.100 type vlan id 100

Note that the order in which you set the VF to promiscuous mode and add the
VLAN interface does not matter (you can do either first). The result in this
example is that the VF will get all traffic that is tagged with VLAN 100.
(注意，设置 VF 为混杂模式和添加 VLAN 接口的先后顺序无关紧要 (可以先做任何一个)。在本
例中，结果将是 VF 会接收到所有打有 VLAN 100 标签的流量。)


Malicious Driver Detection (MDD) for VFs(VF 的恶意驱动检测 (MDD))
-----------------------------------------------------------------
Some Intel Ethernet devices use Malicious Driver Detection (MDD) to detect
malicious traffic from the VF and disable Tx/Rx queues or drop the offending
packet until a VF driver reset occurs. You can view MDD messages in the PF's
system log using the dmesg command.
(一些 Intel 以太网设备使用恶意驱动检测 (MDD) 来检测来自 VF 的恶意流量，并禁用 Tx/Rx
队列或丢弃违规数据包，直到 VF 驱动程序复位发生为止。可以使用 dmesg 命令在 PF 的系统
日志中查看 MDD 消息。)

- If the PF driver logs MDD events from the VF, confirm that the correct VF
  driver is installed.
  (如果 PF 驱动程序记录了来自 VF 的 MDD 事件，请确认已安装正确的 VF 驱动程序。)
- To restore functionality, you can manually reload the VF or VM or enable
  automatic VF resets.
  (要恢复功能，可以手动重新加载 VF 或 VM，或者启用自动 VF 复位。)
- When automatic VF resets are enabled, the PF driver will immediately reset
  the VF and reenable queues when it detects MDD events on the receive path.
  (启用自动 VF 复位后，当 PF 驱动程序在接收路径上检测到 MDD 事件时，会立即复位 VF 并
  重新启用队列。)
- If automatic VF resets are disabled, the PF will not automatically reset the
  VF when it detects MDD events.
  (如果禁用了自动 VF 复位，PF 在检测到 MDD 事件时不会自动复位 VF。)

To enable or disable automatic VF resets, use the following command::
(要启用或禁用自动 VF 复位，请使用以下命令：)

  # ethtool --set-priv-flags <ethX> mdd-auto-reset-vf on|off


MAC and VLAN Anti-Spoofing Feature for VFs(VF 的 MAC 和 VLAN 防欺骗特性)
------------------------------------------------------------------------
When a malicious driver on a Virtual Function (VF) interface attempts to send a
spoofed packet, it is dropped by the hardware and not transmitted.
(当虚拟功能 (VF) 接口上的恶意驱动程序试图发送欺骗性数据包时，该数据包会被硬件丢弃而不
被发送。)

NOTE: This feature can be disabled for a specific VF::
(注意：可以为特定的 VF 禁用此特性：)

  # ip link set <ethX> vf <vf id> spoofchk {off|on}


Jumbo Frames(巨型帧)
--------------------
Jumbo Frames support is enabled by changing the Maximum Transmission Unit (MTU)
to a value larger than the default value of 1500.
(通过将最大传输单元 (MTU) 更改为大于默认值 1500 的数值，即可启用巨型帧支持。)

Use the ifconfig command to increase the MTU size. For example, enter the
following where <ethX> is the interface number::
(使用 ifconfig 命令增大 MTU 大小。例如，在 <ethX> 为接口编号的情况下输入以下命令：)

  # ifconfig <ethX> mtu 9000 up

Alternatively, you can use the ip command as follows::
(或者，可以按如下方式使用 ip 命令：)

  # ip link set mtu 9000 dev <ethX>
  # ip link set up dev <ethX>

This setting is not saved across reboots.
(此设置在重启后不会保存。)


NOTE: The maximum MTU setting for jumbo frames is 9702. This corresponds to the
maximum jumbo frame size of 9728 bytes.
(注意：巨型帧的最大 MTU 设置为 9702。这对应于 9728 字节的最大巨型帧大小。)

NOTE: This driver will attempt to use multiple page sized buffers to receive
each jumbo packet. This should help to avoid buffer starvation issues when
allocating receive packets.
(注意：此驱动程序会尝试使用多个页大小的缓冲区来接收每个巨型数据包。这应有助于避免在
分配接收数据包时出现缓冲区耗尽的问题。)

NOTE: Packet loss may have a greater impact on throughput when you use jumbo
frames. If you observe a drop in performance after enabling jumbo frames,
enabling flow control may mitigate the issue.
(注意：使用巨型帧时，丢包可能对吞吐量产生更大的影响。如果在启用巨型帧后观察到性能下
降，启用流控制可能会缓解此问题。)


Speed and Duplex Configuration(速度和双工配置)
----------------------------------------------
In addressing speed and duplex configuration issues, you need to distinguish
between copper-based adapters and fiber-based adapters.
(在处理速度和双工配置问题时，需要区分基于铜缆的适配器和基于光纤的适配器。)

In the default mode, an Intel(R) Ethernet Network Adapter using copper
connections will attempt to auto-negotiate with its link partner to determine
the best setting. If the adapter cannot establish link with the link partner
using auto-negotiation, you may need to manually configure the adapter and link
partner to identical settings to establish link and pass packets. This should
only be needed when attempting to link with an older switch that does not
support auto-negotiation or one that has been forced to a specific speed or
duplex mode. Your link partner must match the setting you choose. 1 Gbps speeds
and higher cannot be forced. Use the autonegotiation advertising setting to
manually set devices for 1 Gbps and higher.
(在默认模式下，使用铜缆连接的 Intel(R) 以太网网络适配器会尝试与其链路伙伴进行自动协商
以确定最佳设置。如果适配器无法通过自动协商与链路伙伴建立链路，则可能需要手动将适配器
和链路伙伴配置为相同的设置才能建立链路并传输数据包。只有在尝试与不支持自动协商的旧式
交换机、或被强制为特定速度或双工模式的交换机建立链路时，才需要这样做。链路伙伴必须与
你选择的设置相匹配。1 Gbps 及更高的速度不能强制设置。请使用自动协商通告 (autonegotia
tion advertising) 设置来手动配置 1 Gbps 及更高速度的设备。)

Speed, duplex, and autonegotiation advertising are configured through the
ethtool utility. For the latest version, download and install ethtool from the
following website:
(速度、双工和自动协商通告通过 ethtool 工具进行配置。要获取最新版本，请从以下网站下载
并安装 ethtool：)

   https://kernel.org/pub/software/network/ethtool/

To see the speed configurations your device supports, run the following::
(要查看你的设备支持的速度配置，请运行以下命令：)

  # ethtool <ethX>

Caution: Only experienced network administrators should force speed and duplex
or change autonegotiation advertising manually. The settings at the switch must
always match the adapter settings. Adapter performance may suffer or your
adapter may not operate if you configure the adapter differently from your
switch.
(警告：只有经验丰富的网络管理员才应强制设置速度和双工，或手动更改自动协商通告。交换
机上的设置必须始终与适配器设置相匹配。如果适配器的配置与交换机不一致，适配器性能可能
下降，甚至可能无法工作。)


Data Center Bridging (DCB)(数据中心桥接 (DCB))
----------------------------------------------
NOTE: The kernel assumes that TC0 is available, and will disable Priority Flow
Control (PFC) on the device if TC0 is not available. To fix this, ensure TC0 is
enabled when setting up DCB on your switch.
(注意：内核假定 TC0 可用，如果 TC0 不可用，将会在设备上禁用优先级流控制 (PFC)。要修
复此问题，请确保在交换机上设置 DCB 时启用 TC0。)

DCB is a configuration Quality of Service implementation in hardware. It uses
the VLAN priority tag (802.1p) to filter traffic. That means that there are 8
different priorities that traffic can be filtered into. It also enables
priority flow control (802.1Qbb) which can limit or eliminate the number of
dropped packets during network stress. Bandwidth can be allocated to each of
these priorities, which is enforced at the hardware level (802.1Qaz).
(DCB 是在硬件中实现的配置化服务质量。它使用 VLAN 优先级标签 (802.1p) 来过滤流量，这
意味着流量可以被过滤进 8 个不同的优先级。它还启用优先级流控制 (802.1Qbb)，可以在网络
压力较大时限制或消除丢包数量。可以为每个优先级分配带宽，这由硬件层面强制实施
(802.1Qaz)。)

DCB is normally configured on the network using the DCBX protocol (802.1Qaz), a
specialization of LLDP (802.1AB). The ice driver supports the following
mutually exclusive variants of DCBX support:
(在网络中通常使用 DCBX 协议 (802.1Qaz)——LLDP (802.1AB) 的一个特化版本——来配置 DCB。
ice 驱动程序支持以下两种互斥的 DCBX 支持变体：)

1) Firmware-based LLDP Agent
   (基于固件的 LLDP 代理)
2) Software-based LLDP Agent
   (基于软件的 LLDP 代理)

In firmware-based mode, firmware intercepts all LLDP traffic and handles DCBX
negotiation transparently for the user. In this mode, the adapter operates in
"willing" DCBX mode, receiving DCB settings from the link partner (typically a
switch). The local user can only query the negotiated DCB configuration. For
information on configuring DCBX parameters on a switch, please consult the
switch manufacturer's documentation.
(在基于固件的模式下，固件会拦截所有 LLDP 流量，并为用户透明地处理 DCBX 协商。在此模
式下，适配器以 "willing" DCBX 模式运行，从链路伙伴 (通常是交换机) 接收 DCB 设置。本
地用户只能查询协商得到的 DCB 配置。有关在交换机上配置 DCBX 参数的信息，请查阅交换机
制造商的文档。)

In software-based mode, LLDP traffic is forwarded to the network stack and user
space, where a software agent can handle it. In this mode, the adapter can
operate in either "willing" or "nonwilling" DCBX mode and DCB configuration can
be both queried and set locally. This mode requires the FW-based LLDP Agent to
be disabled.
(在基于软件的模式下，LLDP 流量被转发到网络协议栈和用户空间，由软件代理进行处理。在此
模式下，适配器可以以 "willing" 或 "nonwilling" DCBX 模式运行，并且 DCB 配置既可以在
本地查询也可以在本地设置。此模式要求禁用基于固件的 LLDP 代理。)

NOTE:
(注意：)

- You can enable and disable the firmware-based LLDP Agent using an ethtool
  private flag. Refer to the "FW-LLDP (Firmware Link Layer Discovery Protocol)"
  section in this README for more information.
  (可以使用 ethtool 私有标志启用和禁用基于固件的 LLDP 代理。更多信息请参阅本 README
  中的 "FW-LLDP (Firmware Link Layer Discovery Protocol)" 一节。)
- In software-based DCBX mode, you can configure DCB parameters using software
  LLDP/DCBX agents that interface with the Linux kernel's DCB Netlink API. We
  recommend using OpenLLDP as the DCBX agent when running in software mode. For
  more information, see the OpenLLDP man pages and
  https://github.com/intel/openlldp.
  (在基于软件的 DCBX 模式下，可以使用与 Linux 内核 DCB Netlink API 交互的软件
  LLDP/DCBX 代理来配置 DCB 参数。在软件模式下运行时，我们建议使用 OpenLLDP 作为 DCBX
  代理。更多信息请参阅 OpenLLDP 的 man 手册页以及 https://github.com/intel/openlldp。)
- The driver implements the DCB netlink interface layer to allow the user space
  to communicate with the driver and query DCB configuration for the port.
  (驱动程序实现了 DCB netlink 接口层，允许用户空间与驱动程序通信并查询端口的 DCB 配
  置。)
- iSCSI with DCB is not supported.
  (不支持将 iSCSI 与 DCB 配合使用。)


FW-LLDP (Firmware Link Layer Discovery Protocol)(FW-LLDP (固件链路层发现协议))
------------------------------------------------------------------------------
Use ethtool to change FW-LLDP settings. The FW-LLDP setting is per port and
persists across boots.
(使用 ethtool 更改 FW-LLDP 设置。FW-LLDP 设置是按端口生效的，并在重启后保持。)

To enable LLDP::
(要启用 LLDP：)

  # ethtool --set-priv-flags <ethX> fw-lldp-agent on

To disable LLDP::
(要禁用 LLDP：)

  # ethtool --set-priv-flags <ethX> fw-lldp-agent off

To check the current LLDP setting::
(要查看当前的 LLDP 设置：)

  # ethtool --show-priv-flags <ethX>

NOTE: You must enable the UEFI HII "LLDP Agent" attribute for this setting to
take effect. If "LLDP AGENT" is set to disabled, you cannot enable it from the
OS.
(注意：必须启用 UEFI HII 的 "LLDP Agent" 属性，此设置才能生效。如果 "LLDP AGENT" 被
设置为禁用，则无法从操作系统启用它。)


Flow Control(流控制)
--------------------
Ethernet Flow Control (IEEE 802.3x) can be configured with ethtool to enable
receiving and transmitting pause frames for ice. When transmit is enabled,
pause frames are generated when the receive packet buffer crosses a predefined
threshold. When receive is enabled, the transmit unit will halt for the time
delay specified when a pause frame is received.
(可以使用 ethtool 配置以太网流控制 (IEEE 802.3x)，以启用 ice 的暂停帧收发。启用发送
时，当接收数据包缓冲区超过预定义的阈值，就会生成暂停帧。启用接收时，收到暂停帧后，发
送单元将按指定的时间延迟暂停。)

NOTE: You must have a flow control capable link partner.
(注意：你必须拥有一个支持流控制的链路伙伴。)

Flow Control is disabled by default.
(流控制默认是禁用的。)

Use ethtool to change the flow control settings.
(使用 ethtool 更改流控制设置。)

To enable or disable Rx or Tx Flow Control::
(要启用或禁用 Rx 或 Tx 流控制：)

  # ethtool -A <ethX> rx <on|off> tx <on|off>

Note: This command only enables or disables Flow Control if auto-negotiation is
disabled. If auto-negotiation is enabled, this command changes the parameters
used for auto-negotiation with the link partner.
(注意：只有在自动协商被禁用的情况下，此命令才会启用或禁用流控制。如果启用了自动协
商，此命令会更改与链路伙伴进行自动协商时所使用的参数。)

Note: Flow Control auto-negotiation is part of link auto-negotiation. Depending
on your device, you may not be able to change the auto-negotiation setting.
(注意：流控制自动协商是链路自动协商的一部分。根据你的设备情况，可能无法更改自动协商
设置。)

NOTE:
(注意：)

- The ice driver requires flow control on both the port and link partner. If
  flow control is disabled on one of the sides, the port may appear to hang on
  heavy traffic.
  (ice 驱动程序要求端口和链路伙伴双方都启用流控制。如果其中一侧禁用了流控制，端口在
  大流量下可能看起来像是挂起了。)
- You may encounter issues with link-level flow control (LFC) after disabling
  DCB. The LFC status may show as enabled but traffic is not paused. To resolve
  this issue, disable and reenable LFC using ethtool::
  (禁用 DCB 后，你可能会遇到链路级流控制 (LFC) 的问题。LFC 状态可能显示为已启用，但
  流量并未被暂停。要解决此问题，请使用 ethtool 先禁用再重新启用 LFC：)

   # ethtool -A <ethX> rx off tx off
   # ethtool -A <ethX> rx on tx on


NAPI(NAPI)
----------

This driver supports NAPI (Rx polling mode).
(此驱动程序支持 NAPI (接收轮询模式)。)

See :ref:`Documentation/networking/napi.rst <napi>` for more information.
(更多信息参见 :ref:`Documentation/networking/napi.rst <napi>`。)

MACVLAN(MACVLAN)
----------------
This driver supports MACVLAN. Kernel support for MACVLAN can be tested by
checking if the MACVLAN driver is loaded. You can run 'lsmod | grep macvlan' to
see if the MACVLAN driver is loaded or run 'modprobe macvlan' to try to load
the MACVLAN driver.
(此驱动程序支持 MACVLAN。可以通过检查 MACVLAN 驱动程序是否已加载来测试内核是否支持
MACVLAN。可以运行 'lsmod | grep macvlan' 查看 MACVLAN 驱动程序是否已加载，或运行
'modprobe macvlan' 尝试加载 MACVLAN 驱动程序。)

NOTE:
(注意：)

- In passthru mode, you can only set up one MACVLAN device. It will inherit the
  MAC address of the underlying PF (Physical Function) device.
  (在 passthru 模式下，只能设置一个 MACVLAN 设备。它将继承底层 PF (物理功能) 设备的
  MAC 地址。)


IEEE 802.1ad (QinQ) Support(IEEE 802.1ad (QinQ) 支持)
-----------------------------------------------------
The IEEE 802.1ad standard, informally known as QinQ, allows for multiple VLAN
IDs within a single Ethernet frame. VLAN IDs are sometimes referred to as
"tags," and multiple VLAN IDs are thus referred to as a "tag stack." Tag stacks
allow L2 tunneling and the ability to segregate traffic within a particular
VLAN ID, among other uses.
(IEEE 802.1ad 标准，俗称 QinQ，允许在单个以太网帧中包含多个 VLAN ID。VLAN ID 有时被
称为 "标签 (tag)"，多个 VLAN ID 因此被称为 "标签栈 (tag stack)"。标签栈可用于 L2 隧
道以及在特定 VLAN ID 内隔离流量等用途。)

NOTES:
(注意：)

- Receive checksum offloads and VLAN acceleration are not supported for 802.1ad
  (QinQ) packets.
  (802.1ad (QinQ) 数据包不支持接收校验和卸载和 VLAN 加速。)

- 0x88A8 traffic will not be received unless VLAN stripping is disabled with
  the following command::
  (除非使用以下命令禁用 VLAN 剥离，否则将不会接收 0x88A8 流量：)

    # ethtool -K <ethX> rxvlan off

- 0x88A8/0x8100 double VLANs cannot be used with 0x8100 or 0x8100/0x8100 VLANS
  configured on the same port. 0x88a8/0x8100 traffic will not be received if
  0x8100 VLANs are configured.
  (0x88A8/0x8100 双 VLAN 不能与配置在同一端口上的 0x8100 或 0x8100/0x8100 VLAN 共
  用。如果配置了 0x8100 VLAN，将不会接收 0x88a8/0x8100 流量。)

- The VF can only transmit 0x88A8/0x8100 (i.e., 802.1ad/802.1Q) traffic if:
  (仅当满足以下条件时，VF 才能发送 0x88A8/0x8100 (即 802.1ad/802.1Q) 流量：)

    1) The VF is not assigned a port VLAN.
       (VF 未被分配端口 VLAN。)
    2) spoofchk is disabled from the PF. If you enable spoofchk, the VF will
       not transmit 0x88A8/0x8100 traffic.
       (已在 PF 上禁用 spoofchk。如果启用 spoofchk，VF 将不会发送 0x88A8/0x8100
       流量。)

- The VF may not receive all network traffic based on the Inner VLAN header
  when VF true promiscuous mode (vf-true-promisc-support) and double VLANs are
  enabled in SR-IOV mode.
  (在 SR-IOV 模式下启用 VF 真混杂模式 (vf-true-promisc-support) 和双 VLAN 时，VF 可
  能无法基于内部 VLAN 头接收所有网络流量。)

The following are examples of how to configure 802.1ad (QinQ)::
(以下是如何配置 802.1ad (QinQ) 的示例：)

  # ip link add link eth0 eth0.24 type vlan proto 802.1ad id 24
  # ip link add link eth0.24 eth0.24.371 type vlan proto 802.1Q id 371

  Where "24" and "371" are example VLAN IDs.


Tunnel/Overlay Stateless Offloads(隧道/覆盖层无状态卸载)
--------------------------------------------------------
Supported tunnels and overlays include VXLAN, GENEVE, and others depending on
hardware and software configuration. Stateless offloads are enabled by default.
(支持的隧道和覆盖层包括 VXLAN、GENEVE 等，具体取决于硬件和软件配置。无状态卸载默认启
用。)

To view the current state of all offloads::
(要查看所有卸载的当前状态：)

  # ethtool -k <ethX>


UDP Segmentation Offload(UDP 分段卸载)
--------------------------------------
Allows the adapter to offload transmit segmentation of UDP packets with
payloads up to 64K into valid Ethernet frames. Because the adapter hardware is
able to complete data segmentation much faster than operating system software,
this feature may improve transmission performance.
In addition, the adapter may use fewer CPU resources.
(允许适配器将载荷高达 64K 的 UDP 数据包的发送分段卸载为有效的以太网帧。由于适配器硬
件完成数据分段的速度远快于操作系统软件，此特性可能会提升传输性能。此外，适配器还可以
占用更少的 CPU 资源。)

NOTE:
(注意：)

- The application sending UDP packets must support UDP segmentation offload.
  (发送 UDP 数据包的应用程序必须支持 UDP 分段卸载。)

To enable/disable UDP Segmentation Offload, issue the following command::
(要启用/禁用 UDP 分段卸载，请执行以下命令：)

  # ethtool -K <ethX> tx-udp-segmentation [off|on]

PTP pin interface(PTP 引脚接口)
-------------------------------
All adapters support standard PTP pin interface. SDPs (Software Definable Pin)
are single ended pins with both periodic output and external timestamp
supported. There are also specific differential input/output pins (TIME_SYNC,
1PPS) with only one of the functions supported.
(所有适配器都支持标准 PTP 引脚接口。SDP (Software Definable Pin，软件可定义引脚) 是
单端引脚，同时支持周期性输出和外部时间戳。还有一些特定的差分输入/输出引脚 (TIME_SYNC、
1PPS)，它们只支持其中一种功能。)

There are adapters with DPLL, where pins are connected to the DPLL instead of
being exposed on the board. You have to be aware that in those configurations,
only SDP pins are exposed and each pin has its own fixed direction.
To see input signal on those PTP pins, you need to configure DPLL properly.
Output signal is only visible on DPLL and to send it to the board SMA/U.FL pins,
DPLL output pins have to be manually configured.
(有些适配器带有 DPLL，此时引脚连接到 DPLL 而不是暴露在板卡上。需要注意的是，在这种配
置下，只有 SDP 引脚暴露出来，并且每个引脚都有各自固定的方向。要查看这些 PTP 引脚上的
输入信号，需要正确配置 DPLL。输出信号只在 DPLL 上可见，要将其送到板卡的 SMA/U.FL 引
脚，必须手动配置 DPLL 输出引脚。)

GNSS module(GNSS 模块)
----------------------
Requires kernel compiled with CONFIG_GNSS=y or CONFIG_GNSS=m.
Allows user to read messages from the GNSS hardware module and write supported
commands. If the module is physically present, a GNSS device is spawned:
``/dev/gnss<id>``.
The protocol of write command is dependent on the GNSS hardware module as the
driver writes raw bytes by the GNSS object to the receiver through i2c. Please
refer to the hardware GNSS module documentation for configuration details.
(需要内核以 CONFIG_GNSS=y 或 CONFIG_GNSS=m 编译。允许用户读取来自 GNSS 硬件模块的消
息并写入受支持的命令。如果该模块物理存在，则会创建一个 GNSS 设备：``/dev/gnss<id>``。
写入命令的协议取决于 GNSS 硬件模块，因为驱动程序通过 GNSS 对象经 i2c 向接收器写入原
始字节。配置细节请参阅硬件 GNSS 模块的文档。)


Firmware (FW) logging(固件 (FW) 日志)
-------------------------------------
The driver supports FW logging via the debugfs interface on PF 0 only. The FW
running on the NIC must support FW logging; if the FW doesn't support FW logging
the 'fwlog' file will not get created in the ice debugfs directory.
(此驱动程序仅通过 PF 0 上的 debugfs 接口支持 FW 日志。NIC 上运行的 FW 必须支持 FW 日
志；如果 FW 不支持 FW 日志，将不会在 ice 的 debugfs 目录中创建 'fwlog' 文件。)

Module configuration(模块配置)
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
Firmware logging is configured on a per module basis. Each module can be set to
a value independent of the other modules (unless the module 'all' is specified).
The modules will be instantiated under the 'fwlog/modules' directory.
(固件日志按模块进行配置。每个模块都可以独立于其他模块进行设置 (除非指定了模块
'all')。这些模块将在 'fwlog/modules' 目录下实例化。)

The user can set the log level for a module by writing to the module file like
this::
(用户可以通过向模块文件写入来设置模块的日志级别，如下所示：)

  # echo <log_level> > /sys/kernel/debug/ice/0000\:18\:00.0/fwlog/modules/<module>

where
(其中)

* log_level is a name as described below. Each level includes the
  messages from the previous/lower level
  (log_level 是如下所述的名称。每个级别都包含前一个/更低级别的消息)

      *	none
      *	error
      *	warning
      *	normal
      *	verbose

* module is a name that represents the module to receive events for. The
  module names are
  (module 是表示要接收其事件的模块的名称。模块名称如下：)

      *	general
      *	ctrl
      *	link
      *	link_topo
      *	dnl
      *	i2c
      *	sdp
      *	mdio
      *	adminq
      *	hdma
      *	lldp
      *	dcbx
      *	dcb
      *	xlr
      *	nvm
      *	auth
      *	vpd
      *	iosf
      *	parser
      *	sw
      *	scheduler
      *	txq
      *	rsvd
      *	post
      *	watchdog
      *	task_dispatch
      *	mng
      *	synce
      *	health
      *	tsdrv
      *	pfreg
      *	mdlver
      *	all

The name 'all' is special and allows the user to set all of the modules to the
specified log_level or to read the log_level of all of the modules.
(名称 'all' 比较特殊，它允许用户将所有模块设置为指定的 log_level，或读取所有模块的
log_level。)

Example usage to configure the modules(配置模块的示例用法)
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

To set a single module to 'verbose'::
(要将单个模块设置为 'verbose'：)

  # echo verbose > /sys/kernel/debug/ice/0000\:18\:00.0/fwlog/modules/link

To set multiple modules then issue the command multiple times::
(要设置多个模块，则多次执行该命令：)

  # echo verbose > /sys/kernel/debug/ice/0000\:18\:00.0/fwlog/modules/link
  # echo warning > /sys/kernel/debug/ice/0000\:18\:00.0/fwlog/modules/ctrl
  # echo none > /sys/kernel/debug/ice/0000\:18\:00.0/fwlog/modules/dcb

To set all the modules to the same value::
(要将所有模块设置为相同的值：)

  # echo normal > /sys/kernel/debug/ice/0000\:18\:00.0/fwlog/modules/all

To read the log_level of a specific module (e.g. module 'general')::
(要读取特定模块 (例如模块 'general') 的 log_level：)

  # cat /sys/kernel/debug/ice/0000\:18\:00.0/fwlog/modules/general

To read the log_level of all the modules::
(要读取所有模块的 log_level：)

  # cat /sys/kernel/debug/ice/0000\:18\:00.0/fwlog/modules/all

Enabling FW log(启用 FW 日志)
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
Configuring the modules indicates to the FW that the configured modules should
generate events that the driver is interested in, but it **does not** send the
events to the driver until the enable message is sent to the FW. To do this
the user can write a 1 (enable) or 0 (disable) to 'fwlog/enable'. An example
is::
(配置这些模块是告诉 FW，已配置的模块应生成驱动程序感兴趣的事件，但在向 FW 发送启用消
息之前，它 **不会** 将这些事件发送给驱动程序。为此，用户可以向 'fwlog/enable' 写入 1
(启用) 或 0 (禁用)。示例如下：)

  # echo 1 > /sys/kernel/debug/ice/0000\:18\:00.0/fwlog/enable

Retrieving FW log data(获取 FW 日志数据)
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
The FW log data can be retrieved by reading from 'fwlog/data'. The user can
write any value to 'fwlog/data' to clear the data. The data can only be cleared
when FW logging is disabled. The FW log data is a binary file that is sent to
Intel and used to help debug user issues.
(可以通过读取 'fwlog/data' 来获取 FW 日志数据。用户可以向 'fwlog/data' 写入任意值来清
除数据。只有在 FW 日志被禁用时才能清除数据。FW 日志数据是一个二进制文件，会被发送给
Intel，用于帮助调试用户的问题。)

An example to read the data is::
(读取数据的示例如下：)

  # cat /sys/kernel/debug/ice/0000\:18\:00.0/fwlog/data > fwlog.bin

An example to clear the data is::
(清除数据的示例如下：)

  # echo 0 > /sys/kernel/debug/ice/0000\:18\:00.0/fwlog/data

Changing how often the log events are sent to the driver(更改日志事件发送到驱动程序的频率)
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
The driver receives FW log data from the Admin Receive Queue (ARQ). The
frequency that the FW sends the ARQ events can be configured by writing to
'fwlog/nr_messages'. The range is 1-128 (1 means push every log message, 128
means push only when the max AQ command buffer is full). The suggested value is
10. The user can see what the value is configured to by reading
'fwlog/nr_messages'. An example to set the value is::
(驱动程序从管理接收队列 (ARQ) 接收 FW 日志数据。FW 发送 ARQ 事件的频率可以通过写入
'fwlog/nr_messages' 来配置。范围为 1-128 (1 表示每条日志消息都推送，128 表示仅在最大
AQ 命令缓冲区满时才推送)。建议值为 10。用户可以通过读取 'fwlog/nr_messages' 查看当前
配置的值。设置该值的示例如下：)

  # echo 50 > /sys/kernel/debug/ice/0000\:18\:00.0/fwlog/nr_messages

Configuring the amount of memory used to store FW log data(配置用于存储 FW 日志数据的内存量)
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
The driver stores FW log data within the driver. The default size of the memory
used to store the data is 1MB. Some use cases may require more or less data so
the user can change the amount of memory that is allocated for FW log data.
To change the amount of memory then write to 'fwlog/log_size'. The value must be
one of: 128K, 256K, 512K, 1M, or 2M. FW logging must be disabled to change the
value. An example of changing the value is::
(驱动程序将 FW 日志数据存储在驱动程序内部。用于存储数据的内存默认大小为 1MB。某些使
用场景可能需要更多或更少的数据，因此用户可以更改分配给 FW 日志数据的内存量。要更改内
存量，请写入 'fwlog/log_size'。该值必须是 128K、256K、512K、1M 或 2M 之一。必须先禁
用 FW 日志才能更改该值。更改该值的示例如下：)

  # echo 128K > /sys/kernel/debug/ice/0000\:18\:00.0/fwlog/log_size


Performance Optimization(性能优化)
==================================
Driver defaults are meant to fit a wide variety of workloads, but if further
optimization is required, we recommend experimenting with the following
settings.
(驱动程序默认设置旨在适应各种工作负载，但如果需要进一步优化，我们建议对以下设置进行
试验。)


Rx Descriptor Ring Size(Rx 描述符环形队列大小)
----------------------------------------------
To reduce the number of Rx packet discards, increase the number of Rx
descriptors for each Rx ring using ethtool.
(要减少接收数据包的丢弃数量，可以使用 ethtool 增加每个 Rx 环的 Rx 描述符数量。)

  Check if the interface is dropping Rx packets due to buffers being full
  (rx_dropped.nic can mean that there is no PCIe bandwidth)::
  (检查接口是否因缓冲区满而丢弃 Rx 数据包 (rx_dropped.nic 可能意味着 PCIe 带宽不
  足)：)

    # ethtool -S <ethX> | grep "rx_dropped"

  If the previous command shows drops on queues, it may help to increase
  the number of descriptors using 'ethtool -G'::
  (如果上一条命令显示队列上有丢包，使用 'ethtool -G' 增加描述符数量可能会有所帮助：)

    # ethtool -G <ethX> rx <N>
    Where <N> is the desired number of ring entries/descriptors

  This can provide temporary buffering for issues that create latency while
  the CPUs process descriptors.
  (这可以为 CPU 处理描述符时产生延迟的问题提供临时缓冲。)


Interrupt Rate Limiting(中断速率限制)
-------------------------------------
This driver supports an adaptive interrupt throttle rate (ITR) mechanism that
is tuned for general workloads. The user can customize the interrupt rate
control for specific workloads, via ethtool, adjusting the number of
microseconds between interrupts.
(此驱动程序支持一种针对常规工作负载调优的自适应中断节流率 (ITR) 机制。用户可以通过
ethtool 针对特定工作负载自定义中断速率控制，调整中断之间的微秒数。)

To set the interrupt rate manually, you must disable adaptive mode::
(要手动设置中断速率，必须禁用自适应模式：)

  # ethtool -C <ethX> adaptive-rx off adaptive-tx off

For lower CPU utilization:
(若要降低 CPU 占用率：)

  Disable adaptive ITR and lower Rx and Tx interrupts. The examples below
  affect every queue of the specified interface.
  (禁用自适应 ITR 并降低 Rx 和 Tx 中断。以下示例会影响指定接口的每个队列。)

  Setting rx-usecs and tx-usecs to 80 will limit interrupts to about
  12,500 interrupts per second per queue::
  (将 rx-usecs 和 tx-usecs 设为 80 会将中断限制为每队列每秒约 12,500 次中断：)

    # ethtool -C <ethX> adaptive-rx off adaptive-tx off rx-usecs 80 tx-usecs 80

For reduced latency:
(若要降低延迟：)

  Disable adaptive ITR and ITR by setting rx-usecs and tx-usecs to 0
  using ethtool::
  (使用 ethtool 将 rx-usecs 和 tx-usecs 设为 0 来禁用自适应 ITR 和 ITR：)

    # ethtool -C <ethX> adaptive-rx off adaptive-tx off rx-usecs 0 tx-usecs 0

Per-queue interrupt rate settings:
(每队列中断速率设置：)

  The following examples are for queues 1 and 3, but you can adjust other
  queues.
  (以下示例针对队列 1 和 3，但你也可以调整其他队列。)

  To disable Rx adaptive ITR and set static Rx ITR to 10 microseconds or
  about 100,000 interrupts/second, for queues 1 and 3::
  (要为队列 1 和 3 禁用 Rx 自适应 ITR，并将静态 Rx ITR 设为 10 微秒 (约 100,000 次
  中断/秒)：)

    # ethtool --per-queue <ethX> queue_mask 0xa --coalesce adaptive-rx off
    rx-usecs 10

  To show the current coalesce settings for queues 1 and 3::
  (要显示队列 1 和 3 当前的合并 (coalesce) 设置：)

    # ethtool --per-queue <ethX> queue_mask 0xa --show-coalesce

Bounding interrupt rates using rx-usecs-high:
(使用 rx-usecs-high 限制中断速率上限：)

  :Valid Range: 0-236 (0=no limit)
  (有效范围： 0-236 (0=无限制))

   The range of 0-236 microseconds provides an effective range of 4,237 to
   250,000 interrupts per second. The value of rx-usecs-high can be set
   independently of rx-usecs and tx-usecs in the same ethtool command, and is
   also independent of the adaptive interrupt moderation algorithm. The
   underlying hardware supports granularity in 4-microsecond intervals, so
   adjacent values may result in the same interrupt rate.
   (0-236 微秒的范围提供了每秒 4,237 到 250,000 次中断的有效范围。rx-usecs-high 的值
   可以在同一条 ethtool 命令中独立于 rx-usecs 和 tx-usecs 进行设置，并且也独立于自适
   应中断缓和算法。底层硬件以 4 微秒为间隔支持粒度，因此相邻的值可能产生相同的中断速
   率。)

  The following command would disable adaptive interrupt moderation, and allow
  a maximum of 5 microseconds before indicating a receive or transmit was
  complete. However, instead of resulting in as many as 200,000 interrupts per
  second, it limits total interrupts per second to 50,000 via the rx-usecs-high
  parameter.
  (以下命令将禁用自适应中断缓和，并允许在指示接收或发送完成之前最多经过 5 微秒。不
  过，它不会产生高达每秒 200,000 次的中断，而是通过 rx-usecs-high 参数将每秒总中断次
  数限制为 50,000 次。)

  ::

    # ethtool -C <ethX> adaptive-rx off adaptive-tx off rx-usecs-high 20
    rx-usecs 5 tx-usecs 5


Virtualized Environments(虚拟化环境)
------------------------------------
In addition to the other suggestions in this section, the following may be
helpful to optimize performance in VMs.
(除了本节中的其他建议之外，以下措施也可能有助于优化虚拟机中的性能。)

  Using the appropriate mechanism (vcpupin) in the VM, pin the CPUs to
  individual LCPUs, making sure to use a set of CPUs included in the
  device's local_cpulist: ``/sys/class/net/<ethX>/device/local_cpulist``.
  (在虚拟机中使用适当的机制 (vcpupin) 将 CPU 绑定到各个逻辑 CPU，确保使用设备
  local_cpulist 中所包含的一组 CPU：``/sys/class/net/<ethX>/device/local_cpulist``。)

  Configure as many Rx/Tx queues in the VM as available. (See the iavf driver
  documentation for the number of queues supported.) For example::
  (在虚拟机中尽可能多地配置可用的 Rx/Tx 队列。(支持的队列数请参阅 iavf 驱动程序文
  档。) 例如：)

    # ethtool -L <virt_interface> rx <max> tx <max>


Support(支持)
=============
For general information, go to the Intel support website at:
https://www.intel.com/support/
(一般信息请访问 Intel 支持网站：https://www.intel.com/support/)

If an issue is identified with the released source code on a supported kernel
with a supported adapter, email the specific information related to the issue
to intel-wired-lan@lists.osuosl.org.
(如果在受支持的内核上使用受支持的适配器时发现已发布源代码存在问题，请将与该问题相关
的具体信息通过电子邮件发送至 intel-wired-lan@lists.osuosl.org。)


Trademarks(商标)
================
Intel is a trademark or registered trademark of Intel Corporation or its
subsidiaries in the United States and/or other countries.
(Intel 是 Intel Corporation 或其子公司在美国和/或其他国家/地区的商标或注册商标。)

* Other names and brands may be claimed as the property of others.
  (其他名称和品牌可能归其各自所有者所有。)
