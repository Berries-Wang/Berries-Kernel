.. SPDX-License-Identifier: GPL-2.0
.. include:: <isonum.txt>

=================================================================================
Chelsio N210 10Gb Ethernet Network Controller(Chelsio N210 10Gb 以太网网络控制器)
=================================================================================

Driver Release Notes for Linux

(Linux 驱动发行说明)

Version 2.1.1

(版本 2.1.1)

June 20, 2005

(2005 年 6 月 20 日)

.. Contents

 INTRODUCTION
 FEATURES
 PERFORMANCE
 DRIVER MESSAGES
 KNOWN ISSUES
 SUPPORT


Introduction(简介)
==================

 This document describes the Linux driver for Chelsio 10Gb Ethernet Network
 Controller. This driver supports the Chelsio N210 NIC and is backward
 compatible with the Chelsio N110 model 10Gb NICs.
 (本文档描述 Chelsio 10Gb 以太网网络控制器的 Linux 驱动。该驱动支持 Chelsio
 N210 NIC，并向后兼容 Chelsio N110 型 10Gb NIC。)


Features(特性)
==============

Adaptive Interrupts (adaptive-rx)(自适应中断 (adaptive-rx))
-----------------------------------------------------------

  This feature provides an adaptive algorithm that adjusts the interrupt
  coalescing parameters, allowing the driver to dynamically adapt the latency
  settings to achieve the highest performance during various types of network
  load.
  (该特性提供一种自适应算法来调整中断合并 (interrupt coalescing) 参数，使驱动
  能够动态调整延迟设置，从而在各种类型的网络负载下达到最高性能。)

  The interface used to control this feature is ethtool. Please see the
  ethtool manpage for additional usage information.
  (控制该特性的接口是 ethtool。更多用法信息请参见 ethtool 手册页。)

  By default, adaptive-rx is disabled.
  To enable adaptive-rx::
  (默认情况下，adaptive-rx 处于禁用状态。要启用 adaptive-rx：)

      ethtool -C <interface> adaptive-rx on

  To disable adaptive-rx, use ethtool::
  (要禁用 adaptive-rx，使用 ethtool：)

      ethtool -C <interface> adaptive-rx off

  After disabling adaptive-rx, the timer latency value will be set to 50us.
  You may set the timer latency after disabling adaptive-rx::
  (禁用 adaptive-rx 后，定时器延迟值将被设为 50us。也可以在禁用 adaptive-rx
  之后设置定时器延迟：)

      ethtool -C <interface> rx-usecs <microseconds>

  An example to set the timer latency value to 100us on eth0::
  (在 eth0 上把定时器延迟值设为 100us 的示例：)

      ethtool -C eth0 rx-usecs 100

  You may also provide a timer latency value while disabling adaptive-rx::
  (也可以在禁用 adaptive-rx 的同时提供定时器延迟值：)

      ethtool -C <interface> adaptive-rx off rx-usecs <microseconds>

  If adaptive-rx is disabled and a timer latency value is specified, the timer
  will be set to the specified value until changed by the user or until
  adaptive-rx is enabled.
  (如果禁用了 adaptive-rx 且指定了定时器延迟值，定时器将被设为该指定值，直到
  用户更改它或重新启用 adaptive-rx。)

  To view the status of the adaptive-rx and timer latency values::
  (要查看 adaptive-rx 与定时器延迟值的状态：)

      ethtool -c <interface>


TCP Segmentation Offloading (TSO) Support(TCP 分段卸载 (TSO) 支持)
------------------------------------------------------------------

  This feature, also known as "large send", enables a system's protocol stack
  to offload portions of outbound TCP processing to a network interface card
  thereby reducing system CPU utilization and enhancing performance.
  (该特性也称为 "large send"，它使系统的协议栈能够把部分出站 TCP 处理卸载到
  网卡上，从而降低系统 CPU 利用率并提升性能。)

  The interface used to control this feature is ethtool version 1.8 or higher.
  Please see the ethtool manpage for additional usage information.
  (控制该特性的接口是 ethtool 1.8 或更高版本。更多用法信息请参见 ethtool
  手册页。)

  By default, TSO is enabled.
  To disable TSO::
  (默认情况下，TSO 处于启用状态。要禁用 TSO：)

      ethtool -K <interface> tso off

  To enable TSO::
  (要启用 TSO：)

      ethtool -K <interface> tso on

  To view the status of TSO::
  (要查看 TSO 的状态：)

      ethtool -k <interface>


Performance(性能)
=================

 The following information is provided as an example of how to change system
 parameters for "performance tuning" an what value to use. You may or may not
 want to change these system parameters, depending on your server/workstation
 application. Doing so is not warranted in any way by Chelsio Communications,
 and is done at "YOUR OWN RISK". Chelsio will not be held responsible for loss
 of data or damage to equipment.
 (以下信息仅作为示例，说明如何更改系统参数以进行 "性能调优" 以及应使用什么
 值。是否更改这些系统参数取决于你的服务器/工作站应用。Chelsio Communications
 对此不做任何形式的保证，相关操作的风险由 "你自己承担"。对于数据丢失或设备
 损坏，Chelsio 概不负责。)

 Your distribution may have a different way of doing things, or you may prefer
 a different method. These commands are shown only to provide an example of
 what to do and are by no means definitive.
 (你的发行版可能有不同的做法，或者你可能偏好其他方法。这些命令仅用于提供
 示例，绝非定论。)

 Making any of the following system changes will only last until you reboot
 your system. You may want to write a script that runs at boot-up which
 includes the optimal settings for your system.
 (以下任何一项系统更改都只会在你重启系统之前保持有效。你可能需要编写一个
 开机时运行的脚本，其中包含适合你系统的最优设置。)

  Setting PCI Latency Timer::
  (设置 PCI 延迟定时器：)

      setpci -d 1425::

* 0x0c.l=0x0000F800

  Disabling TCP timestamp::
  (禁用 TCP 时间戳：)

      sysctl -w net.ipv4.tcp_timestamps=0

  Disabling SACK::
  (禁用 SACK：)

      sysctl -w net.ipv4.tcp_sack=0

  Setting large number of incoming connection requests::
  (设置大量传入连接请求：)

      sysctl -w net.ipv4.tcp_max_syn_backlog=3000

  Setting maximum receive socket buffer size::
  (设置最大接收套接字缓冲区大小：)

      sysctl -w net.core.rmem_max=1024000

  Setting maximum send socket buffer size::
  (设置最大发送套接字缓冲区大小：)

      sysctl -w net.core.wmem_max=1024000

  Set smp_affinity (on a multiprocessor system) to a single CPU::
  (把 smp_affinity (在多处理器系统上) 设为单个 CPU：)

      echo 1 > /proc/irq/<interrupt_number>/smp_affinity

  Setting default receive socket buffer size::
  (设置默认接收套接字缓冲区大小：)

      sysctl -w net.core.rmem_default=524287

  Setting default send socket buffer size::
  (设置默认发送套接字缓冲区大小：)

      sysctl -w net.core.wmem_default=524287

  Setting maximum option memory buffers::
  (设置最大选项内存缓冲区：)

      sysctl -w net.core.optmem_max=524287

  Setting maximum backlog (# of unprocessed packets before kernel drops)::
  (设置最大 backlog (内核丢弃数据包之前未处理数据包的数量)：)

      sysctl -w net.core.netdev_max_backlog=300000

  Setting TCP read buffers (min/default/max)::
  (设置 TCP 读缓冲区 (min/default/max)：)

      sysctl -w net.ipv4.tcp_rmem="10000000 10000000 10000000"

  Setting TCP write buffers (min/pressure/max)::
  (设置 TCP 写缓冲区 (min/pressure/max)：)

      sysctl -w net.ipv4.tcp_wmem="10000000 10000000 10000000"

  Setting TCP buffer space (min/pressure/max)::
  (设置 TCP 缓冲区空间 (min/pressure/max)：)

      sysctl -w net.ipv4.tcp_mem="10000000 10000000 10000000"

  TCP window size for single connections:
  (单连接的 TCP 窗口大小：)

   The receive buffer (RX_WINDOW) size must be at least as large as the
   Bandwidth-Delay Product of the communication link between the sender and
   receiver. Due to the variations of RTT, you may want to increase the buffer
   size up to 2 times the Bandwidth-Delay Product. Reference page 289 of
   "TCP/IP Illustrated, Volume 1, The Protocols" by W. Richard Stevens.
   (接收缓冲区 (RX_WINDOW) 大小必须至少与收发双方之间通信链路的带宽时延积
   (Bandwidth-Delay Product) 一样大。由于 RTT 存在变化，你可能需要把缓冲区
   大小增大到带宽时延积的 2 倍。参见 W. Richard Stevens 所著 "TCP/IP
   Illustrated, Volume 1, The Protocols" 一书第 289 页。)

   At 10Gb speeds, use the following formula::
   (在 10Gb 速率下，使用以下公式：)

       RX_WINDOW >= 1.25MBytes * RTT(in milliseconds)
       Example for RTT with 100us: RX_WINDOW = (1,250,000 * 0.1) = 125,000

   RX_WINDOW sizes of 256KB - 512KB should be sufficient.
   (256KB - 512KB 的 RX_WINDOW 大小应当足够。)

   Setting the min, max, and default receive buffer (RX_WINDOW) size::
   (设置最小、最大与默认接收缓冲区 (RX_WINDOW) 大小：)

       sysctl -w net.ipv4.tcp_rmem="<min> <default> <max>"

  TCP window size for multiple connections:
  (多连接的 TCP 窗口大小：)

   The receive buffer (RX_WINDOW) size may be calculated the same as single
   connections, but should be divided by the number of connections. The
   smaller window prevents congestion and facilitates better pacing,
   especially if/when MAC level flow control does not work well or when it is
   not supported on the machine. Experimentation may be necessary to attain
   the correct value. This method is provided as a starting point for the
   correct receive buffer size.
   (接收缓冲区 (RX_WINDOW) 大小可以按与单连接相同的方式计算，但应除以连接
   数量。较小的窗口可防止拥塞并有助于更好的节拍控制 (pacing)，尤其是在 MAC
   层流控工作不佳或机器不支持它的情况下。可能需要通过实验来获得正确的值。
   此方法仅作为确定正确接收缓冲区大小的起点。)

   Setting the min, max, and default receive buffer (RX_WINDOW) size is
   performed in the same manner as single connection.
   (最小、最大与默认接收缓冲区 (RX_WINDOW) 大小的设置方式与单连接情形相同。)


Driver Messages(驱动消息)
=========================

 The following messages are the most common messages logged by syslog. These
 may be found in /var/log/messages.
 (以下消息是 syslog 记录的最常见消息。它们可在 /var/log/messages 中找到。)

  Driver up::
  (驱动已加载：)

     Chelsio Network Driver - version 2.1.1

  NIC detected::
  (检测到 NIC：)

     eth#: Chelsio N210 1x10GBaseX NIC (rev #), PCIX 133MHz/64-bit

  Link up::
  (链路已建立：)

     eth#: link is up at 10 Gbps, full duplex

  Link down::
  (链路已断开：)

     eth#: link is down


Known Issues(已知问题)
======================

 These issues have been identified during testing. The following information
 is provided as a workaround to the problem. In some cases, this problem is
 inherent to Linux or to a particular Linux Distribution and/or hardware
 platform.
 (这些问题是在测试期间发现的。以下信息提供了应对该问题的变通方法。在某些
 情况下，该问题是 Linux 或特定 Linux 发行版和/或硬件平台所固有的。)

  1. Large number of TCP retransmits on a multiprocessor (SMP) system.
     (在多处理器 (SMP) 系统上出现大量 TCP 重传。)

      On a system with multiple CPUs, the interrupt (IRQ) for the network
      controller may be bound to more than one CPU. This will cause TCP
      retransmits if the packet data were to be split across different CPUs
      and re-assembled in a different order than expected.
      (在具有多个 CPU 的系统上，网络控制器的中断 (IRQ) 可能绑定到多个 CPU。
      如果数据包数据被拆分到不同 CPU 上、并以与预期不同的顺序重新组装，就会
      导致 TCP 重传。)

      To eliminate the TCP retransmits, set smp_affinity on the particular
      interrupt to a single CPU. You can locate the interrupt (IRQ) used on
      the N110/N210 by using ifconfig::
      (要消除 TCP 重传，需把该中断的 smp_affinity 设为单个 CPU。可以使用
      ifconfig 找到 N110/N210 所使用的中断 (IRQ)：)

	  ifconfig <dev_name> | grep Interrupt

      Set the smp_affinity to a single CPU::
      (把 smp_affinity 设为单个 CPU：)

	  echo 1 > /proc/irq/<interrupt_number>/smp_affinity

      It is highly suggested that you do not run the irqbalance daemon on your
      system, as this will change any smp_affinity setting you have applied.
      The irqbalance daemon runs on a 10 second interval and binds interrupts
      to the least loaded CPU determined by the daemon. To disable this daemon::
      (强烈建议不要在系统上运行 irqbalance 守护进程，因为它会更改你已应用的
      任何 smp_affinity 设置。irqbalance 守护进程每 10 秒运行一次，并把中断
      绑定到它确定的负载最轻的 CPU。要禁用该守护进程：)

	  chkconfig --level 2345 irqbalance off

      By default, some Linux distributions enable the kernel feature,
      irqbalance, which performs the same function as the daemon. To disable
      this feature, add the following line to your bootloader::
      (默认情况下，一些 Linux 发行版启用了与该守护进程功能相同的内核特性
      irqbalance。要禁用该特性，请在引导加载器中添加以下行：)

	  noirqbalance

	  Example using the Grub bootloader::
	  (使用 Grub 引导加载器的示例：)

	      title Red Hat Enterprise Linux AS (2.4.21-27.ELsmp)
	      root (hd0,0)
	      kernel /vmlinuz-2.4.21-27.ELsmp ro root=/dev/hda3 noirqbalance
	      initrd /initrd-2.4.21-27.ELsmp.img

  2. After running insmod, the driver is loaded and the incorrect network
     interface is brought up without running ifup.
     (运行 insmod 之后，驱动被加载，并且在未运行 ifup 的情况下错误的网络接口
     被启用。)

      When using 2.4.x kernels, including RHEL kernels, the Linux kernel
      invokes a script named "hotplug". This script is primarily used to
      automatically bring up USB devices when they are plugged in, however,
      the script also attempts to automatically bring up a network interface
      after loading the kernel module. The hotplug script does this by scanning
      the ifcfg-eth# config files in /etc/sysconfig/network-scripts, looking
      for HWADDR=<mac_address>.
      (使用 2.4.x 内核 (包括 RHEL 内核) 时，Linux 内核会调用一个名为
      "hotplug" 的脚本。该脚本主要用于在 USB 设备插入时自动启用它们，但它
      也会在加载内核模块之后尝试自动启用一个网络接口。hotplug 脚本通过扫描
      /etc/sysconfig/network-scripts 中的 ifcfg-eth# 配置文件、查找
      HWADDR=<mac_address> 来做到这一点。)

      If the hotplug script does not find the HWADDRR within any of the
      ifcfg-eth# files, it will bring up the device with the next available
      interface name. If this interface is already configured for a different
      network card, your new interface will have incorrect IP address and
      network settings.
      (如果 hotplug 脚本在任何 ifcfg-eth# 文件中都找不到 HWADDR，它就会用
      下一个可用的接口名启用该设备。如果该接口已经为另一块网卡配置过，你的
      新接口就会得到错误的 IP 地址与网络设置。)

      To solve this issue, you can add the HWADDR=<mac_address> key to the
      interface config file of your network controller.
      (要解决此问题，可以在网络控制器的接口配置文件中添加
      HWADDR=<mac_address> 键。)

      To disable this "hotplug" feature, you may add the driver (module name)
      to the "blacklist" file located in /etc/hotplug. It has been noted that
      this does not work for network devices because the net.agent script
      does not use the blacklist file. Simply remove, or rename, the net.agent
      script located in /etc/hotplug to disable this feature.
      (要禁用这一 "hotplug" 特性，可以把驱动 (模块名) 添加到 /etc/hotplug 中
      的 "blacklist" 文件。但据观察这对网络设备不起作用，因为 net.agent 脚本
      并不使用 blacklist 文件。只需删除或重命名 /etc/hotplug 中的 net.agent
      脚本即可禁用该特性。)

  3. Transport Protocol (TP) hangs when running heavy multi-connection traffic
     on an AMD Opteron system with HyperTransport PCI-X Tunnel chipset.
     (在配备 HyperTransport PCI-X Tunnel 芯片组的 AMD Opteron 系统上运行繁重
     的多连接流量时，传输协议 (TP) 挂起。)

      If your AMD Opteron system uses the AMD-8131 HyperTransport PCI-X Tunnel
      chipset, you may experience the "133-Mhz Mode Split Completion Data
      Corruption" bug identified by AMD while using a 133Mhz PCI-X card on the
      bus PCI-X bus.
      (如果你的 AMD Opteron 系统使用 AMD-8131 HyperTransport PCI-X Tunnel
      芯片组，那么在 PCI-X 总线上使用 133Mhz PCI-X 卡时，你可能会遇到 AMD
      发现的 "133-Mhz Mode Split Completion Data Corruption" 缺陷。)

      AMD states, "Under highly specific conditions, the AMD-8131 PCI-X Tunnel
      can provide stale data via split completion cycles to a PCI-X card that
      is operating at 133 Mhz", causing data corruption.
      (AMD 指出，"在非常特定的条件下，AMD-8131 PCI-X Tunnel 会通过 split
      completion 周期向运行在 133 Mhz 的 PCI-X 卡提供过期数据"，从而导致数据
      损坏。)

      AMD's provides three workarounds for this problem, however, Chelsio
      recommends the first option for best performance with this bug:
      (AMD 针对此问题提供了三种变通方法，不过 Chelsio 建议选择第一项，以便在
      存在此缺陷的情况下获得最佳性能：)

	For 133Mhz secondary bus operation, limit the transaction length and
	the number of outstanding transactions, via BIOS configuration
	programming of the PCI-X card, to the following:
	(对于 133Mhz 二级总线操作，通过 PCI-X 卡的 BIOS 配置编程，把事务长度与
	未完成事务的数量限制为以下值：)

	   Data Length (bytes): 1k
	   (数据长度 (字节)：1k)

	   Total allowed outstanding transactions: 2
	   (允许的未完成事务总数：2)

      Please refer to AMD 8131-HT/PCI-X Errata 26310 Rev 3.08 August 2004,
      section 56, "133-MHz Mode Split Completion Data Corruption" for more
      details with this bug and workarounds suggested by AMD.
      (有关该缺陷及 AMD 建议的变通方法的更多细节，请参阅 AMD 8131-HT/PCI-X
      勘误文档 26310 Rev 3.08 (2004 年 8 月) 第 56 节 "133-MHz Mode Split
      Completion Data Corruption"。)

      It may be possible to work outside AMD's recommended PCI-X settings, try
      increasing the Data Length to 2k bytes for increased performance. If you
      have issues with these settings, please revert to the "safe" settings
      and duplicate the problem before submitting a bug or asking for support.
      (也可以不采用 AMD 推荐的 PCI-X 设置，尝试把数据长度增大到 2k 字节以获得
      更高性能。如果这些设置出现问题，请在提交缺陷报告或寻求支持之前，先恢复
      到 "安全" 设置并重现该问题。)

      .. note::

	    The default setting on most systems is 8 outstanding transactions
	    and 2k bytes data length.
	    (注意：大多数系统上的默认设置为 8 个未完成事务和 2k 字节数据长度。)

  4. On multiprocessor systems, it has been noted that an application which
     is handling 10Gb networking can switch between CPUs causing degraded
     and/or unstable performance.
     (在多处理器系统上，据观察处理 10Gb 网络的应用可能在 CPU 之间切换，导致
     性能下降和/或不稳定。)

      If running on an SMP system and taking performance measurements, it
      is suggested you either run the latest netperf-2.4.0+ or use a binding
      tool such as Tim Hockin's procstate utilities (runon)
      <http://www.hockin.org/~thockin/procstate/>.
      (如果在 SMP 系统上运行并进行性能测量，建议要么运行最新的
      netperf-2.4.0+，要么使用绑定工具，例如 Tim Hockin 的 procstate 工具集
      (runon) <http://www.hockin.org/~thockin/procstate/>。)

      Binding netserver and netperf (or other applications) to particular
      CPUs will have a significant difference in performance measurements.
      You may need to experiment which CPU to bind the application to in
      order to achieve the best performance for your system.
      (把 netserver 与 netperf (或其他应用) 绑定到特定 CPU，会使性能测量结果
      出现显著差异。可能需要通过实验确定把应用绑定到哪个 CPU，才能为你的
      系统取得最佳性能。)

      If you are developing an application designed for 10Gb networking,
      please keep in mind you may want to look at kernel functions
      sched_setaffinity & sched_getaffinity to bind your application.
      (如果你在开发面向 10Gb 网络的应用，请记住你可能需要借助内核函数
      sched_setaffinity 与 sched_getaffinity 来绑定你的应用。)

      If you are just running user-space applications such as ftp, telnet,
      etc., you may want to try the runon tool provided by Tim Hockin's
      procstate utility. You could also try binding the interface to a
      particular CPU: runon 0 ifup eth0
      (如果你只是运行 ftp、telnet 等用户态应用，可以尝试 Tim Hockin 的
      procstate 工具集提供的 runon 工具。也可以尝试把接口绑定到特定 CPU：
      runon 0 ifup eth0)


Support(支持)
=============

 If you have problems with the software or hardware, please contact our
 customer support team via email at support@chelsio.com or check our website
 at http://www.chelsio.com
 (如果你遇到软件或硬件方面的问题，请通过电子邮件 support@chelsio.com 联系
 我们的客户支持团队，或访问我们的网站 http://www.chelsio.com)

-------------------------------------------------------------------------------

::

 Chelsio Communications
 370 San Aleso Ave.
 Suite 100
 Sunnyvale, CA 94085
 http://www.chelsio.com

This program is free software; you can redistribute it and/or modify
it under the terms of the GNU General Public License, version 2, as
published by the Free Software Foundation.
(本程序是自由软件；你可以遵照自由软件基金会发布的 GNU 通用公共许可证第 2 版
的条款对其进行再分发和/或修改。)

You should have received a copy of the GNU General Public License along
with this program; if not, write to the Free Software Foundation, Inc.,
59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.
(你应该已随本程序收到一份 GNU 通用公共许可证的副本；如果没有，请致函
Free Software Foundation, Inc.，地址：59 Temple Place - Suite 330, Boston,
MA 02111-1307, USA。)

THIS SOFTWARE IS PROVIDED ``AS IS`` AND WITHOUT ANY EXPRESS OR IMPLIED
WARRANTIES, INCLUDING, WITHOUT LIMITATION, THE IMPLIED WARRANTIES OF
MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE.
(本软件按 ``AS IS`` 提供，不附带任何明示或默示的保证，包括但不限于对适销性
和特定用途适用性的默示保证。)

Copyright |copy| 2003-2005 Chelsio Communications. All rights reserved.
(Copyright |copy| 2003-2005 Chelsio Communications. 版权所有。)
