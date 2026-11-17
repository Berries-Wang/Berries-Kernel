.. SPDX-License-Identifier: GPL-2.0

================================================
PLIP: The Parallel Line Internet Protocol Device
================================================

(PLIP：并行线路 Internet 协议设备)

Donald Becker (becker@super.org)
I.D.A. Supercomputing Research Center, Bowie MD 20715
(Donald Becker (becker@super.org)，I.D.A. 超级计算研究中心，马里兰州 Bowie 20715)

At some point T. Thorn will probably contribute text,
Tommy Thorn (tthorn@daimi.aau.dk)
(T. Thorn (Tommy Thorn，tthorn@daimi.aau.dk) 日后可能会贡献一些文字。)

PLIP Introduction
(PLIP 简介)
-----------------

This document describes the parallel port packet pusher for Net/LGX.
This device interface allows a point-to-point connection between two
parallel ports to appear as a IP network interface.
(本文档描述 Net/LGX 的并行端口数据包推送器。这种设备接口使两个并行端口之间的点对点连接表现为一个 IP 网络接口。)

What is PLIP?
(什么是 PLIP？)
=============

PLIP is Parallel Line IP, that is, the transportation of IP packages
over a parallel port. In the case of a PC, the obvious choice is the
printer port.  PLIP is a non-standard, but [can use] uses the standard
LapLink null-printer cable [can also work in turbo mode, with a PLIP
cable]. [The protocol used to pack IP packages, is a simple one
initiated by Crynwr.]
(PLIP 即 Parallel Line IP，也就是通过并行端口传输 IP 包。就 PC 而言，显而易见的选择是打印机端口。PLIP 是非标准的，但 [可以] 使用标准的 LapLink 空打印机电缆 [也可以配合 PLIP 电缆在 turbo 模式下工作]。[用于封装 IP 包的协议是一个由 Crynwr 发起的简单协议。])

Advantages of PLIP
(PLIP 的优点)
==================

It's cheap, it's available everywhere, and it's easy.
(它便宜，随处可得，而且简单。)

The PLIP cable is all that's needed to connect two Linux boxes, and it
can be built for very few bucks.
(连接两台 Linux 机器只需要一根 PLIP 电缆，而且花很少的钱就能做出来。)

Connecting two Linux boxes takes only a second's decision and a few
minutes' work, no need to search for a [supported] netcard. This might
even be especially important in the case of notebooks, where netcards
are not easily available.
(把两台 Linux 机器连起来只需要一秒钟的决定和几分钟的工作，无需去寻找 [受支持的] 网卡。在网卡不易获得的场合——例如笔记本电脑——这一点可能尤为重要。)

Not requiring a netcard also means that apart from connecting the
cables, everything else is software configuration [which in principle
could be made very easy.]
(不需要网卡还意味着，除了连接电缆之外，其余一切都是软件配置 [原则上这可以做得非常简单。])

Disadvantages of PLIP
(PLIP 的缺点)
=====================

Doesn't work over a modem, like SLIP and PPP. Limited range, 15 m.
Can only be used to connect three (?) Linux boxes. Doesn't connect to
an existing Ethernet. Isn't standard (not even de facto standard, like
SLIP).
(不能像 SLIP 和 PPP 那样通过调制解调器工作。距离受限，15 米。最多只能用来连接三台 (?) Linux 机器。不能接入现有的以太网。不是标准（甚至不像 SLIP 那样是事实上的标准）。)

Performance
(性能)
===========

PLIP easily outperforms Ethernet cards....(ups, I was dreaming, but
it *is* getting late. EOB)
(PLIP 轻松超越以太网卡……(哎呀，我在做梦呢，不过天色确实已经晚了。EOB))

PLIP driver details
(PLIP 驱动细节)
-------------------

The Linux PLIP driver is an implementation of the original Crynwr protocol,
that uses the parallel port subsystem of the kernel in order to properly
share parallel ports between PLIP and other services.
(Linux PLIP 驱动是原始 Crynwr 协议的一种实现，它使用内核的并行端口子系统，以便在 PLIP 与其他服务之间正确地共享并行端口。)

IRQs and trigger timeouts
(IRQ 与触发超时)
=========================

When a parallel port used for a PLIP driver has an IRQ configured to it, the
PLIP driver is signaled whenever data is sent to it via the cable, such that
when no data is available, the driver isn't being used.
(当用于 PLIP 驱动的并行端口配置了 IRQ 时，每当有数据经由电缆发送给它，PLIP 驱动就会得到通知，从而在没有数据时不会占用驱动。)

However, on some machines it is hard, if not impossible, to configure an IRQ
to a certain parallel port, mainly because it is used by some other device.
On these machines, the PLIP driver can be used in IRQ-less mode, where
the PLIP driver would constantly poll the parallel port for data waiting,
and if such data is available, process it. This mode is less efficient than
the IRQ mode, because the driver has to check the parallel port many times
per second, even when no data at all is sent. Some rough measurements
indicate that there isn't a noticeable performance drop when using IRQ-less
mode as compared to IRQ mode as far as the data transfer speed is involved.
There is a performance drop on the machine hosting the driver.
(然而，在某些机器上，为某个并行端口配置 IRQ 即使并非不可能也十分困难，主要原因是它已被其他设备占用。在这类机器上，PLIP 驱动可以以无 IRQ 模式使用，此时 PLIP 驱动会不断轮询并行端口是否有数据在等待，如有则进行处理。这种模式比 IRQ 模式效率低，因为驱动每秒必须多次检查并行端口，即使根本没有数据在发送。一些粗略的测量表明，就数据传输速度而言，无 IRQ 模式与 IRQ 模式相比没有明显的性能下降。但承载驱动的机器上会有性能下降。)

When the PLIP driver is used in IRQ mode, the timeout used for triggering a
data transfer (the maximal time the PLIP driver would allow the other side
before announcing a timeout, when trying to handshake a transfer of some
data) is, by default, 500usec. As IRQ delivery is more or less immediate,
this timeout is quite sufficient.
(当 PLIP 驱动以 IRQ 模式使用时，用于触发数据传输的超时（即在尝试握手传输某些数据时，PLIP 驱动在宣告超时之前允许对方使用的最长时间）默认为 500 微秒。由于 IRQ 的送达或多或少是即时的，这个超时相当充裕。)

When in IRQ-less mode, the PLIP driver polls the parallel port HZ times
per second (where HZ is typically 100 on most platforms, and 1024 on an
Alpha, as of this writing). Between two such polls, there are 10^6/HZ usecs.
On an i386, for example, 10^6/100 = 10000usec. It is easy to see that it is
quite possible for the trigger timeout to expire between two such polls, as
the timeout is only 500usec long. As a result, it is required to change the
trigger timeout on the *other* side of a PLIP connection, to about
10^6/HZ usecs. If both sides of a PLIP connection are used in IRQ-less mode,
this timeout is required on both sides.
(在无 IRQ 模式下，PLIP 驱动每秒轮询并行端口 HZ 次（就撰写本文而言，HZ 在大多数平台上通常为 100，在 Alpha 上为 1024）。两次这样的轮询之间相隔 10^6/HZ 微秒。例如在 i386 上，10^6/100 = 10000 微秒。不难看出，由于超时只有 500 微秒长，触发超时完全可能在这两次轮询之间到期。因此，需要把 PLIP 连接另一侧的触发超时改为约 10^6/HZ 微秒。如果 PLIP 连接的两端都以无 IRQ 模式使用，则两端都需要设置此超时。)

It appears that in practice, the trigger timeout can be shorter than in the
above calculation. It isn't an important issue, unless the wire is faulty,
in which case a long timeout would stall the machine when, for whatever
reason, bits are dropped.
(实践中的触发超时似乎可以比上面的计算值更短。这并不是什么重要问题，除非线路有故障——那种情况下，无论出于何种原因发生位丢失时，过长的超时都会使机器停顿。)

A utility that can perform this change in Linux is plipconfig, which is part
of the net-tools package (its location can be found in the
Documentation/Changes file). An example command would be
'plipconfig plipX trigger 10000', where plipX is the appropriate
PLIP device.
(在 Linux 中可以执行此项修改的工具是 plipconfig，它是 net-tools 软件包的一部分（其位置可以在 Documentation/Changes 文件中找到）。示例命令为 'plipconfig plipX trigger 10000'，其中 plipX 是相应的 PLIP 设备。)

PLIP hardware interconnection
(PLIP 硬件互连)
-----------------------------

PLIP uses several different data transfer methods.  The first (and the
only one implemented in the early version of the code) uses a standard
printer "null" cable to transfer data four bits at a time using
data bit outputs connected to status bit inputs.
(PLIP 使用几种不同的数据传输方法。第一种（也是代码早期版本中唯一实现的一种）使用标准打印机"空"电缆，通过把数据位输出连接到状态位输入，每次传输 4 位数据。)

The second data transfer method relies on both machines having
bi-directional parallel ports, rather than output-only ``printer``
ports.  This allows byte-wide transfers and avoids reconstructing
nibbles into bytes, leading to much faster transfers.
(第二种数据传输方法依赖于两台机器都拥有双向并行端口，而不是只能输出的 ``printer`` 端口。这样可以按字节宽传输，避免把半字节重组为字节，从而实现快得多的传输。)

Parallel Transfer Mode 0 Cable
(并行传输模式 0 电缆)
==============================

The cable for the first transfer mode is a standard
printer "null" cable which transfers data four bits at a time using
data bit outputs of the first port (machine T) connected to the
status bit inputs of the second port (machine R).  There are five
status inputs, and they are used as four data inputs and a clock (data
strobe) input, arranged so that the data input bits appear as contiguous
bits with standard status register implementation.
(第一种传输模式使用的电缆是标准打印机"空"电缆，它通过把第一个端口（机器 T）的数据位输出连接到第二个端口（机器 R）的状态位输入，每次传输 4 位数据。共有五个状态输入，它们被用作四个数据输入和一个时钟（数据选通）输入，其排列方式使得在标准状态寄存器实现中，数据输入位表现为连续的位。)

A cable that implements this protocol is available commercially as a
"Null Printer" or "Turbo Laplink" cable.  It can be constructed with
two DB-25 male connectors symmetrically connected as follows::
(实现此协议的电缆在市面上作为 "Null Printer" 或 "Turbo Laplink" 电缆出售。它可以由两个对称连接的 DB-25 公头连接器构成，如下所示：)

    STROBE output	1*
    D0->ERROR	2 - 15		15 - 2
    D1->SLCT	3 - 13		13 - 3
    D2->PAPOUT	4 - 12		12 - 4
    D3->ACK	5 - 10		10 - 5
    D4->BUSY	6 - 11		11 - 6
    D5,D6,D7 are   7*, 8*, 9*
    AUTOFD output 14*
    INIT   output 16*
    SLCTIN	17 - 17
    extra grounds are 18*,19*,20*,21*,22*,23*,24*
    GROUND	25 - 25

    * Do not connect these pins on either end

If the cable you are using has a metallic shield it should be
connected to the metallic DB-25 shell at one end only.
(如果你使用的电缆带有金属屏蔽层，它应当只在一端连接到金属 DB-25 外壳。)

Parallel Transfer Mode 1
(并行传输模式 1)
========================

The second data transfer method relies on both machines having
bi-directional parallel ports, rather than output-only ``printer``
ports.  This allows byte-wide transfers, and avoids reconstructing
nibbles into bytes.  This cable should not be used on unidirectional
``printer`` (as opposed to ``parallel``) ports or when the machine
isn't configured for PLIP, as it will result in output driver
conflicts and the (unlikely) possibility of damage.
(第二种数据传输方法依赖于两台机器都拥有双向并行端口，而不是只能输出的 ``printer`` 端口。这样可以按字节宽传输，避免把半字节重组为字节。这种电缆不应用于单向 ``printer`` 端口（而非 ``parallel`` 端口），也不应用于未配置为 PLIP 的机器，否则会导致输出驱动冲突，并（不太可能地）存在造成损坏的可能性。)

The cable for this transfer mode should be constructed as follows::
(这种传输模式的电缆应按如下方式制作：)

    STROBE->BUSY 1 - 11
    D0->D0	2 - 2
    D1->D1	3 - 3
    D2->D2	4 - 4
    D3->D3	5 - 5
    D4->D4	6 - 6
    D5->D5	7 - 7
    D6->D6	8 - 8
    D7->D7	9 - 9
    INIT -> ACK  16 - 10
    AUTOFD->PAPOUT 14 - 12
    SLCT->SLCTIN 13 - 17
    GND->ERROR	18 - 15
    extra grounds are 19*,20*,21*,22*,23*,24*
    GROUND	25 - 25

    * Do not connect these pins on either end

Once again, if the cable you are using has a metallic shield it should
be connected to the metallic DB-25 shell at one end only.
(再次强调，如果你使用的电缆带有金属屏蔽层，它应当只在一端连接到金属 DB-25 外壳。)

PLIP Mode 0 transfer protocol
(PLIP 模式 0 传输协议)
=============================

The PLIP driver is compatible with the "Crynwr" parallel port transfer
standard in Mode 0.  That standard specifies the following protocol::
(PLIP 驱动在模式 0 下与 "Crynwr" 并行端口传输标准兼容。该标准规定了以下协议：)

   send header nibble '0x8'
   count-low octet
   count-high octet
   ... data octets
   checksum octet

Each octet is sent as::
(每个八位组按如下方式发送：)

	<wait for rx. '0x1?'>	<send 0x10+(octet&0x0F)>
	<wait for rx. '0x0?'>	<send 0x00+((octet>>4)&0x0F)>

To start a transfer the transmitting machine outputs a nibble 0x08.
That raises the ACK line, triggering an interrupt in the receiving
machine.  The receiving machine disables interrupts and raises its own ACK
line.
(要开始一次传输，发送方机器输出一个 0x08 半字节。这会拉起 ACK 线，在接收方机器中触发一次中断。接收方机器随后禁用中断，并拉起自己的 ACK 线。)

Restated::
(重述如下：)

  (OUT is bit 0-4, OUT.j is bit j from OUT. IN likewise)
  Send_Byte:
     OUT := low nibble, OUT.4 := 1
     WAIT FOR IN.4 = 1
     OUT := high nibble, OUT.4 := 0
     WAIT FOR IN.4 = 0
