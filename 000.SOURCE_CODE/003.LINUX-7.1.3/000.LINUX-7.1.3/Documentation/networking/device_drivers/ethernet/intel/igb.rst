.. SPDX-License-Identifier: GPL-2.0+

==========================================================
Linux Base Driver for Intel(R) Ethernet Network Connection
==========================================================

(Intel(R) 以太网网络连接的 Linux 基础驱动)

Intel Gigabit Linux driver.
Copyright(c) 1999-2018 Intel Corporation.
(Intel 千兆 Linux 驱动。
版权所有 (c) 1999-2018 Intel Corporation。)

Contents
(目录)
========

- Identifying Your Adapter
  (识别你的适配器)
- Command Line Parameters
  (命令行参数)
- Additional Configurations
  (其他配置)
- Support
  (支持)


Identifying Your Adapter
(识别你的适配器)
========================

For information on how to identify your adapter, and for the latest Intel
network drivers, refer to the Intel Support website:
https://www.intel.com/support
(有关如何识别你的适配器的信息以及最新的 Intel 网络驱动，请参阅 Intel 支持网站：
https://www.intel.com/support)


Command Line Parameters
(命令行参数)
========================

If the driver is built as a module, the following optional parameters are used
by entering them on the command line with the modprobe command using this
syntax::
(如果驱动被构建为模块，则可以使用以下可选参数：在命令行中通过 modprobe 命令按如下语法输入：)

    modprobe igb [<option>=<VAL1>,<VAL2>,...]

There needs to be a <VAL#> for each network port in the system supported by
this driver. The values will be applied to each instance, in function order.
For example::
(此驱动支持的系统中每个网络端口都需要有一个 <VAL#>。这些值将按功能顺序依次应用到每个实例上。例如：)

    modprobe igb max_vfs=2,4

In this case, there are two network ports supported by igb in the system.
(在本例中，系统中有两个网络端口由 igb 支持。)

NOTE: A descriptor describes a data buffer and attributes related to the data
buffer. This information is accessed by the hardware.
(注意：描述符 (descriptor) 描述一个数据缓冲区及与该缓冲区相关的属性。硬件会访问这些信息。)

max_vfs
(max_vfs)
-------
:Valid Range: 0-7
(有效范围: 0-7)

This parameter adds support for SR-IOV. It causes the driver to spawn up to
max_vfs worth of virtual functions.  If the value is greater than 0 it will
also force the VMDq parameter to be 1 or more.
(该参数增加对 SR-IOV 的支持。它使驱动最多生成 max_vfs 个虚拟功能 (VF)。如果该值大于 0，还会强制 VMDq 参数
取 1 或更大的值。)

The parameters for the driver are referenced by position. Thus, if you have a
dual port adapter, or more than one adapter in your system, and want N virtual
functions per port, you must specify a number for each port with each parameter
separated by a comma. For example::
(驱动参数按位置引用。因此，如果你有一个双端口适配器，或系统中有多块适配器，并希望每个端口有 N 个虚拟功能，
就必须为每个端口各指定一个数字，各参数之间用逗号分隔。例如：)

    modprobe igb max_vfs=4

This will spawn 4 VFs on the first port.
(这将在第一个端口上生成 4 个 VF。)

::

    modprobe igb max_vfs=2,4

This will spawn 2 VFs on the first port and 4 VFs on the second port.
(这将在第一个端口上生成 2 个 VF，在第二个端口上生成 4 个 VF。)

NOTE: Caution must be used in loading the driver with these parameters.
Depending on your system configuration, number of slots, etc., it is impossible
to predict in all cases where the positions would be on the command line.
(注意：携带这些参数加载驱动时必须谨慎。取决于你的系统配置、插槽数量等，并非在所有情况下都能预测各参数在命令
行上的位置。)

NOTE: Neither the device nor the driver control how VFs are mapped into config
space. Bus layout will vary by operating system. On operating systems that
support it, you can check sysfs to find the mapping.
(注意：无论是设备还是驱动都不控制 VF 如何映射到配置空间。总线布局因操作系统而异。在支持的操作系统上，可以
通过 sysfs 查看映射关系。)

NOTE: When either SR-IOV mode or VMDq mode is enabled, hardware VLAN filtering
and VLAN tag stripping/insertion will remain enabled. Please remove the old
VLAN filter before the new VLAN filter is added. For example::
(注意：当 SR-IOV 模式或 VMDq 模式任一启用时，硬件 VLAN 过滤以及 VLAN 标签的剥离/插入将保持启用。请在添加新
VLAN 过滤器之前先移除旧的 VLAN 过滤器。例如：)

    ip link set eth0 vf 0 vlan 100	// set vlan 100 for VF 0
    ip link set eth0 vf 0 vlan 0	// Delete vlan 100
    ip link set eth0 vf 0 vlan 200	// set a new vlan 200 for VF 0

Debug
(Debug)
-----
:Valid Range: 0-16 (0=none,...,16=all)
(有效范围: 0-16 (0=无,...,16=全部))
:Default Value: 0
(默认值: 0)

This parameter adjusts the level debug messages displayed in the system logs.
(该参数调整系统日志中显示的调试消息级别。)


Additional Features and Configurations
(其他特性与配置)
======================================

Jumbo Frames
(巨型帧)
------------
Jumbo Frames support is enabled by changing the Maximum Transmission Unit (MTU)
to a value larger than the default value of 1500.
(通过把最大传输单元 (MTU) 更改为大于默认值 1500 的数值来启用巨型帧 (Jumbo Frames) 支持。)

Use the ifconfig command to increase the MTU size. For example, enter the
following where <x> is the interface number::
(使用 ifconfig 命令增大 MTU。例如，其中 <x> 为接口编号，输入以下命令：)

    ifconfig eth<x> mtu 9000 up

Alternatively, you can use the ip command as follows::
(或者，也可以按如下方式使用 ip 命令：)

    ip link set mtu 9000 dev eth<x>
    ip link set up dev eth<x>

This setting is not saved across reboots. The setting change can be made
permanent by adding 'MTU=9000' to the file:
(该设置在重启后不会保留。将 'MTU=9000' 添加到以下文件中可使设置更改永久生效：)

- For RHEL: /etc/sysconfig/network-scripts/ifcfg-eth<x>
  (RHEL: /etc/sysconfig/network-scripts/ifcfg-eth<x>)
- For SLES: /etc/sysconfig/network/<config_file>
  (SLES: /etc/sysconfig/network/<config_file>)

NOTE: The maximum MTU setting for Jumbo Frames is 9216. This value coincides
with the maximum Jumbo Frames size of 9234 bytes.
(注意：巨型帧的最大 MTU 设置为 9216。该值与 9234 字节的最大巨型帧尺寸相对应。)

NOTE: Using Jumbo frames at 10 or 100 Mbps is not supported and may result in
poor performance or loss of link.
(注意：不支持在 10 或 100 Mbps 下使用巨型帧，这可能导致性能不佳或链路丢失。)


ethtool
(ethtool)
-------
The driver utilizes the ethtool interface for driver configuration and
diagnostics, as well as displaying statistical information. The latest ethtool
version is required for this functionality. Download it at:
(该驱动利用 ethtool 接口进行驱动配置与诊断，以及显示统计信息。此功能需要最新版本的 ethtool。下载地址：)

https://www.kernel.org/pub/software/network/ethtool/


Enabling Wake on LAN (WoL)
(启用局域网唤醒 (WoL))
--------------------------
WoL is configured through the ethtool utility.
(WoL 通过 ethtool 工具进行配置。)

WoL will be enabled on the system during the next shut down or reboot. For
this driver version, in order to enable WoL, the igb driver must be loaded
prior to shutting down or suspending the system.
(WoL 将在系统下次关机或重启时启用。对于此驱动版本，要启用 WoL，必须在关机或挂起系统之前加载 igb 驱动。)

NOTE: Wake on LAN is only supported on port A of multi-port devices.  Also
Wake On LAN is not supported for the following device:
- Intel(R) Gigabit VT Quad Port Server Adapter
(注意：局域网唤醒仅在多端口设备的端口 A 上受支持。此外，以下设备不支持局域网唤醒：
- Intel(R) Gigabit VT Quad Port Server Adapter)


Multiqueue
(多队列)
----------
In this mode, a separate MSI-X vector is allocated for each queue and one for
"other" interrupts such as link status change and errors. All interrupts are
throttled via interrupt moderation. Interrupt moderation must be used to avoid
interrupt storms while the driver is processing one interrupt. The moderation
value should be at least as large as the expected time for the driver to
process an interrupt. Multiqueue is off by default.
(在此模式下，每个队列都会分配一个独立的 MSI-X 向量，另有一个用于"其他"中断，如链路状态变化和错误。所有中断
都通过中断调节进行节流。驱动处理某个中断期间，必须使用中断调节以避免中断风暴。调节值至少应与驱动处理一个中
断的预期时间一样大。多队列默认关闭。)

REQUIREMENTS: MSI-X support is required for Multiqueue. If MSI-X is not found,
the system will fallback to MSI or to Legacy interrupts. This driver supports
receive multiqueue on all kernels that support MSI-X.
(要求：多队列需要 MSI-X 支持。如果未找到 MSI-X，系统将回退到 MSI 或传统 (Legacy) 中断。该驱动在所有支持
MSI-X 的内核上都支持接收多队列。)

NOTE: On some kernels a reboot is required to switch between single queue mode
and multiqueue mode or vice-versa.
(注意：在某些内核上，需要在单队列模式与多队列模式之间切换（或反向切换）时重启系统。)


MAC and VLAN anti-spoofing feature
(MAC 与 VLAN 防欺骗特性)
----------------------------------
When a malicious driver attempts to send a spoofed packet, it is dropped by the
hardware and not transmitted.
(当恶意驱动试图发送伪造的数据包时，硬件会将其丢弃，不予发送。)

An interrupt is sent to the PF driver notifying it of the spoof attempt. When a
spoofed packet is detected, the PF driver will send the following message to
the system log (displayed by the "dmesg" command):
Spoof event(s) detected on VF(n), where n = the VF that attempted to do the
spoofing
(会向 PF 驱动发送一个中断，通知它此次欺骗企图。当检测到伪造数据包时，PF 驱动会向系统日志（由 "dmesg" 命令
显示）发送以下消息：
在 VF(n) 上检测到欺骗事件 (Spoof event(s) detected on VF(n))，其中 n = 试图进行欺骗的 VF)


Setting MAC Address, VLAN and Rate Limit Using IProute2 Tool
(使用 IProute2 工具设置 MAC 地址、VLAN 和速率限制)
------------------------------------------------------------
You can set a MAC address of a Virtual Function (VF), a default VLAN and the
rate limit using the IProute2 tool. Download the latest version of the
IProute2 tool from Sourceforge if your version does not have all the features
you require.
(可以使用 IProute2 工具设置虚拟功能 (VF) 的 MAC 地址、默认 VLAN 以及速率限制。如果你所用的版本不具备所需
的全部功能，请从 Sourceforge 下载最新版本的 IProute2 工具。)

Credit Based Shaper (Qav Mode)
(基于信用的整形器 (Qav 模式))
------------------------------
When enabling the CBS qdisc in the hardware offload mode, traffic shaping using
the CBS (described in the IEEE 802.1Q-2018 Section 8.6.8.2 and discussed in the
Annex L) algorithm will run in the i210 controller, so it's more accurate and
uses less CPU.
(当以硬件卸载模式启用 CBS 排队规则时，使用 CBS 算法（见 IEEE 802.1Q-2018 第 8.6.8.2 节及附录 L 中的讨论）
的流量整形将在 i210 控制器中运行，因此更精确且占用更少 CPU。)

When using offloaded CBS, and the traffic rate obeys the configured rate
(doesn't go above it), CBS should have little to no effect in the latency.
(使用卸载的 CBS 且流量速率遵守所配置的速率（不超过该速率）时，CBS 对时延几乎没有影响。)

The offloaded version of the algorithm has some limits, caused by how the idle
slope is expressed in the adapter's registers. It can only represent idle slopes
in 16.38431 kbps units, which means that if a idle slope of 2576kbps is
requested, the controller will be configured to use a idle slope of ~2589 kbps,
because the driver rounds the value up. For more details, see the comments on
:c:func:`igb_config_tx_modes()`.
(该算法的卸载版本存在一些限制，这是由适配器寄存器表示空闲斜率 (idle slope) 的方式导致的。它只能以 16.38431
kbps 为单位表示空闲斜率，这意味着如果请求的空闲斜率为 2576kbps，控制器将被配置为使用约 2589 kbps 的空闲斜
率，因为驱动会对该值向上取整。更多细节参见 :c:func:`igb_config_tx_modes()` 的注释。)

NOTE: This feature is exclusive to i210 models.
(注意：此特性为 i210 型号独有。)


Support
(支持)
=======
For general information, go to the Intel support website at:
https://www.intel.com/support/
(一般信息请访问 Intel 支持网站：
https://www.intel.com/support/)

If an issue is identified with the released source code on a supported kernel
with a supported adapter, email the specific information related to the issue
to intel-wired-lan@lists.osuosl.org.
(如果在受支持的内核上使用受支持的适配器时发现已发布源代码存在问题，请将与该问题相关的具体信息发送电子邮件至
intel-wired-lan@lists.osuosl.org。)
