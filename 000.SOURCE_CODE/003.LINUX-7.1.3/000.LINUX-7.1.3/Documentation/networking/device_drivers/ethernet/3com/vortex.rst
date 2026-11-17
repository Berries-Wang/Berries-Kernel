.. SPDX-License-Identifier: GPL-2.0

=========================
3Com Vortex device driver
=========================

(3Com Vortex 设备驱动程序)

Andrew Morton
(作者：Andrew Morton)

30 April 2000
(2000 年 4 月 30 日)


This document describes the usage and errata of the 3Com "Vortex" device
driver for Linux, 3c59x.c.
(本文档描述面向 Linux 的 3Com "Vortex" 设备驱动程序 3c59x.c 的用法和勘误。)

The driver was written by Donald Becker <becker@scyld.com>
(该驱动程序由 Donald Becker <becker@scyld.com> 编写)

Don is no longer the prime maintainer of this version of the driver.
Please report problems to one or more of:
(Don 已不再是本版本驱动程序的主要维护者。有问题请报告给以下一个或多个：)

- Andrew Morton
- Netdev mailing list <netdev@vger.kernel.org>
  (Netdev 邮件列表 <netdev@vger.kernel.org>)
- Linux kernel mailing list <linux-kernel@vger.kernel.org>
  (Linux 内核邮件列表 <linux-kernel@vger.kernel.org>)

Please note the 'Reporting and Diagnosing Problems' section at the end
of this file.
(请注意本文件末尾的 'Reporting and Diagnosing Problems' 一节。)


Since kernel 2.3.99-pre6, this driver incorporates the support for the
3c575-series Cardbus cards which used to be handled by 3c575_cb.c.
(从内核 2.3.99-pre6 起，该驱动程序并入了对 3c575 系列 CardBus 卡的支持，
这些卡以前由 3c575_cb.c 处理。)

This driver supports the following hardware:
(该驱动程序支持以下硬件：)

	- 3c590 Vortex 10Mbps
	- 3c592 EISA 10Mbps Demon/Vortex
	- 3c597 EISA Fast Demon/Vortex
	- 3c595 Vortex 100baseTx
	- 3c595 Vortex 100baseT4
	- 3c595 Vortex 100base-MII
	- 3c900 Boomerang 10baseT
	- 3c900 Boomerang 10Mbps Combo
	- 3c900 Cyclone 10Mbps TPO
	- 3c900 Cyclone 10Mbps Combo
	- 3c900 Cyclone 10Mbps TPC
	- 3c900B-FL Cyclone 10base-FL
	- 3c905 Boomerang 100baseTx
	- 3c905 Boomerang 100baseT4
	- 3c905B Cyclone 100baseTx
	- 3c905B Cyclone 10/100/BNC
	- 3c905B-FX Cyclone 100baseFx
	- 3c905C Tornado
	- 3c920B-EMB-WNM (ATI Radeon 9100 IGP)
	- 3c980 Cyclone
	- 3c980C Python-T
	- 3cSOHO100-TX Hurricane
	- 3c555 Laptop Hurricane
	- 3c556 Laptop Tornado
	- 3c556B Laptop Hurricane
	- 3c575 [Megahertz] 10/100 LAN  CardBus
	- 3c575 Boomerang CardBus
	- 3CCFE575BT Cyclone CardBus
	- 3CCFE575CT Tornado CardBus
	- 3CCFE656 Cyclone CardBus
	- 3CCFEM656B Cyclone+Winmodem CardBus
	- 3CXFEM656C Tornado+Winmodem CardBus
	- 3c450 HomePNA Tornado
	- 3c920 Tornado
	- 3c982 Hydra Dual Port A
	- 3c982 Hydra Dual Port B
	- 3c905B-T4
	- 3c920B-EMB-WNM Tornado

Module parameters
(模块参数)
=================

There are several parameters which may be provided to the driver when
its module is loaded.  These are usually placed in ``/etc/modprobe.d/*.conf``
configuration files.  Example::
(在加载模块时，可以给驱动程序提供若干参数。这些参数通常放在
``/etc/modprobe.d/*.conf`` 配置文件中。例如：)

    options 3c59x debug=3 rx_copybreak=300

If you are using the PCMCIA tools (cardmgr) then the options may be
placed in /etc/pcmcia/config.opts::
(如果你使用 PCMCIA 工具 (cardmgr)，那么这些选项可以放在
/etc/pcmcia/config.opts 中：)

    module "3c59x" opts "debug=3 rx_copybreak=300"


The supported parameters are:
(支持的参数有：)

debug=N
(debug=N)

  Where N is a number from 0 to 7.  Anything above 3 produces a lot
  of output in your system logs.  debug=1 is default.
  (其中 N 是 0 到 7 之间的数字。大于 3 的值会在系统日志中产生大量输出。
  默认值为 debug=1。)

options=N1,N2,N3,...
(options=N1,N2,N3,...)

  Each number in the list provides an option to the corresponding
  network card.  So if you have two 3c905's and you wish to provide
  them with option 0x204 you would use::
  (列表中的每个数字为对应的网卡提供一个选项。例如，如果你有两块 3c905，
  并希望给它们都设置选项 0x204，可以使用：)

    options=0x204,0x204

  The individual options are composed of a number of bitfields which
  have the following meanings:
  (每个选项由若干位域组成，其含义如下：)

  Possible media type settings
  (可选的介质类型设置)

	==	=================================
	0	10baseT
	1	10Mbs AUI
	2	undefined
	3	10base2 (BNC)
	4	100base-TX
	5	100base-FX
	6	MII (Media Independent Interface)
	7	Use default setting from EEPROM
	8       Autonegotiate
	9       External MII
	10      Use default setting from EEPROM
	==	=================================

  When generating a value for the 'options' setting, the above media
  selection values may be OR'ed (or added to) the following:
  (在生成 'options' 设置的值时，上面的介质选择值可以与以下值进行 OR 运算
  (或相加)：)

  ======  =============================================
  0x8000  Set driver debugging level to 7
  0x4000  Set driver debugging level to 2
  0x0400  Enable Wake-on-LAN
  0x0200  Force full duplex mode.
  0x0010  Bus-master enable bit (Old Vortex cards only)
  ======  =============================================

  For example::
  (例如：)

    insmod 3c59x options=0x204

  will force full-duplex 100base-TX, rather than allowing the usual
  autonegotiation.
  (将强制使用全双工 100base-TX，而不是进行常规的自动协商。)

global_options=N
(global_options=N)

  Sets the ``options`` parameter for all 3c59x NICs in the machine.
  Entries in the ``options`` array above will override any setting of
  this.
  (为机器中所有 3c59x 网卡设置 ``options`` 参数。上面 ``options`` 数组中
  的条目会覆盖此项的任何设置。)

full_duplex=N1,N2,N3...
(full_duplex=N1,N2,N3...)

  Similar to bit 9 of 'options'.  Forces the corresponding card into
  full-duplex mode.  Please use this in preference to the ``options``
  parameter.
  (类似于 'options' 的第 9 位。强制相应网卡进入全双工模式。请优先使用
  此参数而不是 ``options`` 参数。)

  In fact, please don't use this at all! You're better off getting
  autonegotiation working properly.
  (实际上，请完全不要使用这个参数！让自动协商正常工作才是更好的选择。)

global_full_duplex=N1
(global_full_duplex=N1)

  Sets full duplex mode for all 3c59x NICs in the machine.  Entries
  in the ``full_duplex`` array above will override any setting of this.
  (为机器中所有 3c59x 网卡设置全双工模式。上面 ``full_duplex`` 数组中的
  条目会覆盖此项的任何设置。)

flow_ctrl=N1,N2,N3...
(flow_ctrl=N1,N2,N3...)

  Use 802.3x MAC-layer flow control.  The 3com cards only support the
  PAUSE command, which means that they will stop sending packets for a
  short period if they receive a PAUSE frame from the link partner.
  (使用 802.3x MAC 层流量控制。3com 网卡只支持 PAUSE 命令，这意味着当它们
  收到来自链路伙伴的 PAUSE 帧时，会在一小段时间内停止发送数据包。)

  The driver only allows flow control on a link which is operating in
  full duplex mode.
  (驱动程序只允许在全双工模式的链路上使用流量控制。)

  This feature does not appear to work on the 3c905 - only 3c905B and
  3c905C have been tested.
  (该特性在 3c905 上似乎不工作 -- 只有 3c905B 和 3c905C 经过测试。)

  The 3com cards appear to only respond to PAUSE frames which are
  sent to the reserved destination address of 01:80:c2:00:00:01.  They
  do not honour PAUSE frames which are sent to the station MAC address.
  (3com 网卡似乎只对发送到保留目标地址 01:80:c2:00:00:01 的 PAUSE 帧做出
  响应。对于发送到工作站 MAC 地址的 PAUSE 帧，它们不予理会。)

rx_copybreak=M
(rx_copybreak=M)

  The driver preallocates 32 full-sized (1536 byte) network buffers
  for receiving.  When a packet arrives, the driver has to decide
  whether to leave the packet in its full-sized buffer, or to allocate
  a smaller buffer and copy the packet across into it.
  (驱动程序为接收预分配了 32 个全尺寸 (1536 字节) 网络缓冲区。当数据包
  到达时，驱动程序必须决定是把它留在全尺寸缓冲区中，还是分配一个更小的
  缓冲区并把数据包复制过去。)

  This is a speed/space tradeoff.
  (这是一种速度/空间的权衡。)

  The value of rx_copybreak is used to decide when to make the copy.
  If the packet size is less than rx_copybreak, the packet is copied.
  The default value for rx_copybreak is 200 bytes.
  (rx_copybreak 的值用于决定何时进行复制。如果数据包大小小于
  rx_copybreak，则复制该数据包。rx_copybreak 的默认值为 200 字节。)

max_interrupt_work=N
(max_interrupt_work=N)

  The driver's interrupt service routine can handle many receive and
  transmit packets in a single invocation.  It does this in a loop.
  The value of max_interrupt_work governs how many times the interrupt
  service routine will loop.  The default value is 32 loops.  If this
  is exceeded the interrupt service routine gives up and generates a
  warning message "eth0: Too much work in interrupt".
  (驱动程序的中断服务例程可以在单次调用中处理多个接收和发送数据包。
  它通过循环来实现。max_interrupt_work 的值决定了中断服务例程循环的次数。
  默认值为 32 次。如果超过该值，中断服务例程会放弃并产生警告消息
  "eth0: Too much work in interrupt"。)

hw_checksums=N1,N2,N3,...
(hw_checksums=N1,N2,N3,...)

  Recent 3com NICs are able to generate IPv4, TCP and UDP checksums
  in hardware.  Linux has used the Rx checksumming for a long time.
  The "zero copy" patch which is planned for the 2.4 kernel series
  allows you to make use of the NIC's DMA scatter/gather and transmit
  checksumming as well.
  (较新的 3com 网卡能够在硬件中生成 IPv4、TCP 和 UDP 校验和。Linux 很久
  以前就已经使用了 Rx 校验和。计划用于 2.4 内核系列的 "zero copy" 补丁
  让你还可以利用网卡的 DMA 分散/聚合 (scatter/gather) 与发送校验和。)

  The driver is set up so that, when the zerocopy patch is applied,
  all Tornado and Cyclone devices will use S/G and Tx checksums.
  (驱动程序的设置是：当应用 zerocopy 补丁后，所有 Tornado 和 Cyclone 设备
  都会使用 S/G 和 Tx 校验和。)

  This module parameter has been provided so you can override this
  decision.  If you think that Tx checksums are causing a problem, you
  may disable the feature with ``hw_checksums=0``.
  (提供这个模块参数是为了让你可以覆盖这一决定。如果你认为 Tx 校验和
  引起了问题，可以用 ``hw_checksums=0`` 禁用该特性。)

  If you think your NIC should be performing Tx checksumming and the
  driver isn't enabling it, you can force the use of hardware Tx
  checksumming with ``hw_checksums=1``.
  (如果你认为你的网卡应该执行 Tx 校验和而驱动程序没有启用它，可以用
  ``hw_checksums=1`` 强制使用硬件 Tx 校验和。)

  The driver drops a message in the logfiles to indicate whether or
  not it is using hardware scatter/gather and hardware Tx checksums.
  (驱动程序会在日志文件中留下一则消息，表明它是否在使用硬件分散/聚合和
  硬件 Tx 校验和。)

  Scatter/gather and hardware checksums provide considerable
  performance improvement for the sendfile() system call, but a small
  decrease in throughput for send().  There is no effect upon receive
  efficiency.
  (分散/聚合和硬件校验和为 sendfile() 系统调用带来相当可观的性能提升，
  但会使 send() 的吞吐量略有下降。对接收效率没有影响。)

compaq_ioaddr=N,
compaq_irq=N,
compaq_device_id=N
(compaq_ioaddr=N、compaq_irq=N、compaq_device_id=N)

  "Variables to work-around the Compaq PCI BIOS32 problem"....
  ("用于规避 Compaq PCI BIOS32 问题的变量"……)

watchdog=N
(watchdog=N)

  Sets the time duration (in milliseconds) after which the kernel
  decides that the transmitter has become stuck and needs to be reset.
  This is mainly for debugging purposes, although it may be advantageous
  to increase this value on LANs which have very high collision rates.
  The default value is 5000 (5.0 seconds).
  (设置一段时间 (以毫秒为单位)，超过这段时间后内核将认定发送器已经卡住并
  需要复位。这主要用于调试目的，尽管在冲突率非常高的局域网上增大该值可能
  有好处。默认值为 5000 (5.0 秒)。)

enable_wol=N1,N2,N3,...
(enable_wol=N1,N2,N3,...)

  Enable Wake-on-LAN support for the relevant interface.  Donald
  Becker's ``ether-wake`` application may be used to wake suspended
  machines.
  (为相应的接口启用 Wake-on-LAN (网络唤醒) 支持。可以使用 Donald Becker
  的 ``ether-wake`` 应用程序来唤醒挂起的机器。)

  Also enables the NIC's power management support.
  (同时还会启用网卡电源管理支持。)

global_enable_wol=N
(global_enable_wol=N)

  Sets enable_wol mode for all 3c59x NICs in the machine.  Entries in
  the ``enable_wol`` array above will override any setting of this.
  (为机器中所有 3c59x 网卡设置 enable_wol 模式。上面 ``enable_wol`` 数组
  中的条目会覆盖此项的任何设置。)

Media selection
(介质选择)
---------------

A number of the older NICs such as the 3c590 and 3c900 series have
10base2 and AUI interfaces.
(一些较旧的网卡，如 3c590 和 3c900 系列，具有 10base2 和 AUI 接口。)

Prior to January, 2001 this driver would autoselect the 10base2 or AUI
port if it didn't detect activity on the 10baseT port.  It would then
get stuck on the 10base2 port and a driver reload was necessary to
switch back to 10baseT.  This behaviour could not be prevented with a
module option override.
(在 2001 年 1 月之前，如果该驱动程序没有在 10baseT 端口上检测到活动，
它会自动选择 10base2 或 AUI 端口。随后它会卡在 10base2 端口上，必须重新
加载驱动程序才能切换回 10baseT。这种行为无法通过模块选项覆盖来阻止。)

Later (current) versions of the driver _do_ support locking of the
media type.  So if you load the driver module with
(较新 (当前) 版本的驱动程序确实支持锁定介质类型。因此，如果你使用以下
命令加载驱动模块：)

	modprobe 3c59x options=0

it will permanently select the 10baseT port.  Automatic selection of
other media types does not occur.
(它将永久性地选择 10baseT 端口。不会再自动选择其他介质类型。)


Transmit error, Tx status register 82
(发送错误，Tx 状态寄存器 82)
-------------------------------------

This is a common error which is almost always caused by another host on
the same network being in full-duplex mode, while this host is in
half-duplex mode.  You need to find that other host and make it run in
half-duplex mode or fix this host to run in full-duplex mode.
(这是一个常见错误，几乎总是由以下原因造成：同一网络上的另一台主机处于
全双工模式，而本机处于半双工模式。你需要找到那台主机并让它以半双工模式
运行，或者把本机改为以全双工模式运行。)

As a last resort, you can force the 3c59x driver into full-duplex mode
with
(作为最后的手段，你可以用以下方式强制 3c59x 驱动程序进入全双工模式：)

	options 3c59x full_duplex=1

but this has to be viewed as a workaround for broken network gear and
should only really be used for equipment which cannot autonegotiate.
(但这只能被视为针对有故障的网络设备的变通办法，实际只应用于无法自动协商
的设备。)


Additional resources
(其他资源)
--------------------

Details of the device driver implementation are at the top of the source file.
(设备驱动程序实现的细节位于源文件的顶部。)

Additional documentation is available at Don Becker's Linux Drivers site:
(更多文档可在 Don Becker 的 Linux 驱动程序网站上找到：)

     http://www.scyld.com/vortex.html

Donald Becker's driver development site:
(Donald Becker 的驱动程序开发网站：)

     http://www.scyld.com/network.html

Donald's vortex-diag program is useful for inspecting the NIC's state:
(Donald 的 vortex-diag 程序可用于检查网卡的状态：)

     http://www.scyld.com/ethercard_diag.html

Donald's mii-diag program may be used for inspecting and manipulating
the NIC's Media Independent Interface subsystem:
(Donald 的 mii-diag 程序可用于检查和操控网卡的介质无关接口 (MII)
子系统：)

     http://www.scyld.com/ethercard_diag.html#mii-diag

Donald's wake-on-LAN page:
(Donald 的 wake-on-LAN 页面：)

     http://www.scyld.com/wakeonlan.html

3Com's DOS-based application for setting up the NICs EEPROMs:
(3Com 基于 DOS 的网卡 EEPROM 设置工具：)

	ftp://ftp.3com.com/pub/nic/3c90x/3c90xx2.exe


Autonegotiation notes
(自动协商说明)
---------------------

  The driver uses a one-minute heartbeat for adapting to changes in
  the external LAN environment if link is up and 5 seconds if link is down.
  This means that when, for example, a machine is unplugged from a hubbed
  10baseT LAN plugged into a  switched 100baseT LAN, the throughput
  will be quite dreadful for up to sixty seconds.  Be patient.
  (当链路已建立时，驱动程序使用一分钟一次的心跳来适应外部局域网环境的
  变化；链路断开时则是 5 秒。这意味着，例如，当一台机器从使用集线器的
  10baseT 局域网上拔下、插到交换式 100baseT 局域网上时，最长六十秒内
  吞吐量都会相当糟糕。请耐心等待。)

  Cisco interoperability note from Walter Wong <wcw+@CMU.EDU>:
  (来自 Walter Wong <wcw+@CMU.EDU> 的 Cisco 互操作性说明：)

  On a side note, adding HAS_NWAY seems to share a problem with the
  Cisco 6509 switch.  Specifically, you need to change the spanning
  tree parameter for the port the machine is plugged into to 'portfast'
  mode.  Otherwise, the negotiation fails.  This has been an issue
  we've noticed for a while but haven't had the time to track down.
  (另外提一句，添加 HAS_NWAY 后似乎与 Cisco 6509 交换机存在一个共同问题。
  具体来说，你需要把机器所插端口的生产树参数改为 'portfast' 模式，否则
  协商会失败。这是我们注意到已有一段时间、但还没来得及追查的问题。)

  Cisco switches    (Jeff Busch <jbusch@deja.com>)
  (Cisco 交换机    (Jeff Busch <jbusch@deja.com>))

    My "standard config" for ports to which PC's/servers connect directly::
    (对于 PC/服务器直接连接的端口，我的 "标准配置" 是：)

	interface FastEthernet0/N
	description machinename
	load-interval 30
	spanning-tree portfast

    If autonegotiation is a problem, you may need to specify "speed
    100" and "duplex full" as well (or "speed 10" and "duplex half").
    (如果自动协商有问题，你可能还需要指定 "speed 100" 和 "duplex full"
    (或者 "speed 10" 和 "duplex half")。)

    WARNING: DO NOT hook up hubs/switches/bridges to these
    specially-configured ports! The switch will become very confused.
    (警告：不要把集线器/交换机/网桥接到这些经过特殊配置的端口上！交换机
    会变得非常混乱。)


Reporting and diagnosing problems
(报告和诊断问题)
---------------------------------

Maintainers find that accurate and complete problem reports are
invaluable in resolving driver problems.  We are frequently not able to
reproduce problems and must rely on your patience and efforts to get to
the bottom of the problem.
(维护者发现，准确而完整的问题报告对于解决驱动程序问题非常宝贵。我们常常
无法重现问题，必须依靠你的耐心和努力来查明问题的根源。)

If you believe you have a driver problem here are some of the
steps you should take:
(如果你认为自己遇到了驱动程序问题，以下是你应该采取的一些步骤：)

- Is it really a driver problem?
  (- 这真的是驱动程序的问题吗？)

   Eliminate some variables: try different cards, different
   computers, different cables, different ports on the switch/hub,
   different versions of the kernel or of the driver, etc.
   (排除一些变量：尝试不同的网卡、不同的计算机、不同的网线、
   交换机/集线器上的不同端口、不同版本的内核或驱动程序等。)

- OK, it's a driver problem.
  (- 好的，这是驱动程序的问题。)

   You need to generate a report.  Typically this is an email to the
   maintainer and/or netdev@vger.kernel.org.  The maintainer's
   email address will be in the driver source or in the MAINTAINERS file.
   (你需要生成一份报告。通常这是一封发给维护者和/或 netdev@vger.kernel.org
   的电子邮件。维护者的电子邮件地址可以在驱动程序源码或 MAINTAINERS 文件
   中找到。)

- The contents of your report will vary a lot depending upon the
  problem.  If it's a kernel crash then you should refer to
  'Documentation/admin-guide/reporting-issues.rst'.
  (- 报告的内容会因问题而有很大差异。如果是内核崩溃，你应当参考
  'Documentation/admin-guide/reporting-issues.rst'。)

  But for most problems it is useful to provide the following:
  (但对于大多数问题，提供以下信息会很有用：)

   - Kernel version, driver version
     (内核版本、驱动程序版本)

   - A copy of the banner message which the driver generates when
     it is initialised.  For example:
     (驱动程序初始化时生成的横幅消息的一份副本。例如：)

     eth0: 3Com PCI 3c905C Tornado at 0xa400,  00:50:da:6a:88:f0, IRQ 19
     8K byte-wide RAM 5:3 Rx:Tx split, autoselect/Autonegotiate interface.
     MII transceiver found at address 24, status 782d.
     Enabling bus-master transmits and whole-frame receives.

     NOTE: You must provide the ``debug=2`` modprobe option to generate
     a full detection message.  Please do this::
     (注意：你必须提供 ``debug=2`` modprobe 选项才能生成完整的检测消息。
     请这样做：)

	modprobe 3c59x debug=2

   - If it is a PCI device, the relevant output from 'lspci -vx', eg::
     (如果是 PCI 设备，提供 'lspci -vx' 的相关输出，例如：)

       00:09.0 Ethernet controller: 3Com Corporation 3c905C-TX [Fast Etherlink] (rev 74)
	       Subsystem: 3Com Corporation: Unknown device 9200
	       Flags: bus master, medium devsel, latency 32, IRQ 19
	       I/O ports at a400 [size=128]
	       Memory at db000000 (32-bit, non-prefetchable) [size=128]
	       Expansion ROM at <unassigned> [disabled] [size=128K]
	       Capabilities: [dc] Power Management version 2
       00: b7 10 00 92 07 00 10 02 74 00 00 02 08 20 00 00
       10: 01 a4 00 00 00 00 00 db 00 00 00 00 00 00 00 00
       20: 00 00 00 00 00 00 00 00 00 00 00 00 b7 10 00 10
       30: 00 00 00 00 dc 00 00 00 00 00 00 00 05 01 0a 0a

   - A description of the environment: 10baseT? 100baseT?
     full/half duplex? switched or hubbed?
     (对环境的描述：10baseT？100baseT？全/半双工？交换式还是集线器式？)

   - Any additional module parameters which you may be providing to the driver.
     (你可能提供给驱动程序的任何其他模块参数。)

   - Any kernel logs which are produced.  The more the merrier.
     If this is a large file and you are sending your report to a
     mailing list, mention that you have the logfile, but don't send
     it.  If you're reporting direct to the maintainer then just send
     it.
     (产生的任何内核日志。多多益善。如果这是一个大文件，而你要把报告发送
     到邮件列表，请说明你有这份日志文件，但不要发送它。如果你直接向维护者
     报告，那就直接发送。)

     To ensure that all kernel logs are available, add the
     following line to /etc/syslog.conf::
     (为确保所有内核日志都可用，请把以下行添加到 /etc/syslog.conf：)

	 kern.* /var/log/messages

     Then restart syslogd with::
     (然后用以下命令重启 syslogd：)

	 /etc/rc.d/init.d/syslog restart

     (The above may vary, depending upon which Linux distribution you use).
     (以上内容可能因你所使用的 Linux 发行版而异。)

    - If your problem is reproducible then that's great.  Try the
      following:
      (- 如果你的问题可以重现，那就太好了。请尝试以下步骤：)

      1) Increase the debug level.  Usually this is done via:
         (1) 提高调试级别。通常通过以下方式完成：)

	 a) modprobe driver debug=7
	    (a) modprobe driver debug=7
	 b) In /etc/modprobe.d/driver.conf:
	    options driver debug=7
	    (b) 在 /etc/modprobe.d/driver.conf 中：
	    options driver debug=7

      2) Recreate the problem with the higher debug level,
	 send all logs to the maintainer.
         (2) 在更高的调试级别下重现问题，
	 把所有日志发给维护者。)

      3) Download you card's diagnostic tool from Donald
	 Becker's website <http://www.scyld.com/ethercard_diag.html>.
	 Download mii-diag.c as well.  Build these.
         (3) 从 Donald Becker 的网站
	 <http://www.scyld.com/ethercard_diag.html> 下载你网卡的诊断工具。
	 同时下载 mii-diag.c。构建它们。)

	 a) Run 'vortex-diag -aaee' and 'mii-diag -v' when the card is
	    working correctly.  Save the output.
	    (a) 在网卡工作正常时运行 'vortex-diag -aaee' 和 'mii-diag -v'。
	    保存输出。

	 b) Run the above commands when the card is malfunctioning.  Send
	    both sets of output.
	    (b) 在网卡工作异常时运行上述命令。把两组输出都发送出去。)

Finally, please be patient and be prepared to do some work.  You may
end up working on this problem for a week or more as the maintainer
asks more questions, asks for more tests, asks for patches to be
applied, etc.  At the end of it all, the problem may even remain
unresolved.
(最后，请保持耐心，并做好做一些工作的准备。随着维护者提出更多问题、要求
更多测试、要求应用补丁等，你可能要在这个问题上花上一周甚至更长时间。最终
问题甚至可能仍未解决。)
