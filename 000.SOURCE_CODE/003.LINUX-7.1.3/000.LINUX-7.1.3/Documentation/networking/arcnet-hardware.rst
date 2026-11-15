.. SPDX-License-Identifier: GPL-2.0

===============
ARCnet Hardware
===============

(ARCnet 硬件)

:Author: Avery Pennarun <apenwarr@worldvisions.ca>
(作者: Avery Pennarun <apenwarr@worldvisions.ca>)

.. note::

   1) This file is a supplement to arcnet.rst.  Please read that for general
      driver configuration help.
   2) This file is no longer Linux-specific.  It should probably be moved out
      of the kernel sources.  Ideas?
      (注意：1) 本文件是 arcnet.rst 的补充材料，驱动的一般配置帮助请阅读该文件。
      2) 本文件已不再局限于 Linux，或许应该移出内核源码树。有何好主意？)

Because so many people (myself included) seem to have obtained ARCnet cards
without manuals, this file contains a quick introduction to ARCnet hardware,
some cabling tips, and a listing of all jumper settings I can find. If you
have any settings for your particular card, and/or any other information you
have, do not hesitate to :ref:`email to netdev <arcnet-netdev>`.
(由于太多人(包括我自己在内)拿到 ARCnet 卡时都没有手册，本文件提供了 ARCnet 硬件的
快速入门、一些布线技巧，以及我能找到的全部跳线设置清单。如果你掌握自己那张卡的
设置和/或任何其他信息，请随时 :ref:`email to netdev <arcnet-netdev>`。)


Introduction to ARCnet
(ARCnet 简介)
======================

ARCnet is a network type which works in a way similar to popular Ethernet
networks but which is also different in some very important ways.
(ARCnet 是一种工作方式与常见以太网相似、但在一些非常重要的方面又有所不同的网络。)

First of all, you can get ARCnet cards in at least two speeds: 2.5 Mbps
(slower than Ethernet) and 100 Mbps (faster than normal Ethernet).  In fact,
there are others as well, but these are less common.  The different hardware
types, as far as I'm aware, are not compatible and so you cannot wire a
100 Mbps card to a 2.5 Mbps card, and so on.  From what I hear, my driver does
work with 100 Mbps cards, but I haven't been able to verify this myself,
since I only have the 2.5 Mbps variety.  It is probably not going to saturate
your 100 Mbps card.  Stop complaining. :)
(首先，ARCnet 卡至少有两种速度：2.5 Mbps(比以太网慢)和 100 Mbps(比普通以太网快)。
实际上还有其他速度，只是不太常见。据我所知，这些不同类型的硬件互不兼容，因此你
不能把 100 Mbps 的卡和 2.5 Mbps 的卡接在一起，等等。听说我的驱动在 100 Mbps 卡上
也能工作，但我自己无法验证，因为我只有 2.5 Mbps 的卡。它多半也跑不满你的 100 Mbps
卡。别再抱怨了。:))

You also cannot connect an ARCnet card to any kind of Ethernet card and
expect it to work.
(你同样不能把 ARCnet 卡接到任何一种以太网卡上并指望它能工作。)

There are two "types" of ARCnet - STAR topology and BUS topology.  This
refers to how the cards are meant to be wired together.  According to most
available documentation, you can only connect STAR cards to STAR cards and
BUS cards to BUS cards.  That makes sense, right?  Well, it's not quite
true; see below under "Cabling."
(ARCnet 有两种"类型"——STAR(星型)拓扑和 BUS(总线)拓扑。这指的是各卡之间应当如何
连接。按照大多数现有文档的说法，STAR 卡只能连 STAR 卡，BUS 卡只能连 BUS 卡。
听起来合理，对吧？嗯，其实并不完全如此；参见下文 "Cabling"(布线)一节。)

Once you get past these little stumbling blocks, ARCnet is actually quite a
well-designed standard.  It uses something called "modified token passing"
which makes it completely incompatible with so-called "Token Ring" cards,
but which makes transfers much more reliable than Ethernet does.  In fact,
ARCnet will guarantee that a packet arrives safely at the destination, and
even if it can't possibly be delivered properly (ie. because of a cable
break, or because the destination computer does not exist) it will at least
tell the sender about it.
(越过这些小绊脚石之后，ARCnet 实际上是一个设计得相当不错的标准。它使用一种叫做
"改进型令牌传递"(modified token passing)的机制，这使它与所谓的 "Token Ring"
(令牌环)卡完全不兼容，却让传输比以太网可靠得多。事实上，ARCnet 保证数据包安全
到达目的地，即使数据包根本无法正确送达(例如因为电缆断开，或目标计算机不存在)，
它至少也会把这一情况告知发送方。)

Because of the carefully defined action of the "token", it will always make
a pass around the "ring" within a maximum length of time.  This makes it
useful for realtime networks.
(由于"令牌"的行为被仔细定义，它总会在一段最长时间之内绕"环"一周。这使得 ARCnet
适合用于实时网络。)

In addition, all known ARCnet cards have an (almost) identical programming
interface.  This means that with one ARCnet driver you can support any
card, whereas with Ethernet each manufacturer uses what is sometimes a
completely different programming interface, leading to a lot of different,
sometimes very similar, Ethernet drivers.  Of course, always using the same
programming interface also means that when high-performance hardware
facilities like PCI bus mastering DMA appear, it's hard to take advantage of
them.  Let's not go into that.
(此外，所有已知的 ARCnet 卡都拥有(几乎)完全相同的编程接口。这意味着一个 ARCnet
驱动就能支持任何一种卡；而在以太网方面，各家厂商有时使用截然不同的编程接口，导致
出现了大量彼此不同、有时又极其相似的以太网驱动。当然，总是使用同一种编程接口也
意味着：当 PCI 总线主控 DMA 这类高性能硬件特性出现时，很难加以利用。这个话题就
不展开了。)

One thing that makes ARCnet cards difficult to program for, however, is the
limit on their packet sizes; standard ARCnet can only send packets that are
up to 508 bytes in length.  This is smaller than the Internet "bare minimum"
of 576 bytes, let alone the Ethernet MTU of 1500.  To compensate, an extra
level of encapsulation is defined by RFC1201, which I call "packet
splitting," that allows "virtual packets" to grow as large as 64K each,
although they are generally kept down to the Ethernet-style 1500 bytes.
(不过，让 ARCnet 卡难以编程的一点是它的数据包长度限制：标准 ARCnet 只能发送长度
不超过 508 字节的数据包。这比互联网的"最小"576 字节还小，更不用说以太网 1500 的
MTU 了。作为补偿，RFC1201 定义了一个额外的封装层，我称之为"分包"(packet
splitting)，它允许"虚拟数据包"大到每个 64K，不过实际中一般保持在以太网式的
1500 字节。)

For more information on ARCnet networks, visit the "ARCNET Resource Center"
WWW page at:
(想了解关于 ARCnet 网络的更多信息，请访问 "ARCNET Resource Center" 网页：)

	https://www.arcnet.cc


Cabling ARCnet Networks
(ARCnet 网络布线)
=======================

This section was rewritten by
(本节由以下人士重写：)

	Vojtech Pavlik     <vojtech@suse.cz>

using information from several people, including:
(并采用了多位人士提供的信息，包括：)

	- Avery Pennraun     <apenwarr@worldvisions.ca>
	- Stephen A. Wood    <saw@hallc1.cebaf.gov>
	- John Paul Morrison <jmorriso@bogomips.ee.ubc.ca>
	- Joachim Koenig     <jojo@repas.de>

and Avery touched it up a bit, at Vojtech's request.
(应 Vojtech 的要求，Avery 稍微润色了一下。)

ARCnet (the classic 2.5 Mbps version) can be connected by two different
types of cabling: coax and twisted pair.  The other ARCnet-type networks
(100 Mbps TCNS and 320 kbps - 32 Mbps ARCnet Plus) use different types of
cabling (Type1, Fiber, C1, C4, C5).
(ARCnet(经典的 2.5 Mbps 版本)可以用两种不同的线缆连接：同轴电缆和双绞线。其他
ARCnet 类型的网络(100 Mbps TCNS 和 320 kbps - 32 Mbps ARCnet Plus)则使用不同类型
的线缆(Type1、Fiber、C1、C4、C5)。)

For a coax network, you "should" use 93 Ohm RG-62 cable.  But other cables
also work fine, because ARCnet is a very stable network. I personally use 75
Ohm TV antenna cable.
(对于同轴电缆网络，你"应该"使用 93 Ohm RG-62 电缆。但其他电缆也能很好地工作，因为
ARCnet 是一种非常稳定的网络。我个人用的就是 75 Ohm 电视天线电缆。)

Cards for coax cabling are shipped in two different variants: for BUS and
STAR network topologies.  They are mostly the same.  The only difference
lies in the hybrid chip installed.  BUS cards use high impedance output,
while STAR use low impedance.  Low impedance card (STAR) is electrically
equal to a high impedance one with a terminator installed.
(用于同轴布线的卡有两种出厂变体：BUS 和 STAR 网络拓扑。它们大体相同，唯一区别
在于所装的混合芯片(hybrid chip)。BUS 卡使用高阻抗输出，而 STAR 卡使用低阻抗。
低阻抗卡(STAR)在电气上等效于装了端接器的高阻抗卡。)

Usually, the ARCnet networks are built up from STAR cards and hubs.  There
are two types of hubs - active and passive.  Passive hubs are small boxes
with four BNC connectors containing four 47 Ohm resistors::
(通常，ARCnet 网络由 STAR 卡和集线器搭建而成。集线器有两种——有源和无源。无源
集线器是带有四个 BNC 插头、内含四个 47 Ohm 电阻的小盒子：)

	   |         | wires
	   R         + junction
	-R-+-R-      R 47 Ohm resistors
	   R
	   |

The shielding is connected together.  Active hubs are much more complicated;
they are powered and contain electronics to amplify the signal and send it
to other segments of the net.  They usually have eight connectors.  Active
hubs come in two variants - dumb and smart.  The dumb variant just
amplifies, but the smart one decodes to digital and encodes back all packets
coming through.  This is much better if you have several hubs in the net,
since many dumb active hubs may worsen the signal quality.
(屏蔽层是连接在一起的。有源集线器复杂得多；它们需要供电，内部电路会对信号进行
放大并发送到网络的其他网段。它们通常有八个插口。有源集线器又分两种——"哑"的和
"智能"的。哑的只是放大信号，而智能的会把所有经过的数据包解码成数字再重新编码。
如果网络中有多个集线器，智能型要好得多，因为许多哑有源集线器串在一起可能会使
信号质量变差。)

And now to the cabling.  What you can connect together:
(下面谈布线。可以连接在一起的组合有：)

1. A card to a card.  This is the simplest way of creating a 2-computer
   network.
   (卡对卡。这是组建双机网络最简单的方式。)

2. A card to a passive hub.  Remember that all unused connectors on the hub
   must be properly terminated with 93 Ohm (or something else if you don't
   have the right ones) terminators.
   (卡对无源集线器。记住，集线器上所有未使用的插口都必须用 93 Ohm(如果没有合适的，
   用别的也行)端接器正确端接。)

	(Avery's note: oops, I didn't know that.  Mine (TV cable) works
	anyway, though.)
	(Avery 的注记：哎呀，我原来不知道这一点。不过我的(电视电缆)反正也能用。)

3. A card to an active hub.  Here is no need to terminate the unused
   connectors except some kind of aesthetic feeling.  But, there may not be
   more than eleven active hubs between any two computers.  That of course
   doesn't limit the number of active hubs on the network.
   (卡对有源集线器。这里无需端接未使用的插口，除非出于某种美观上的考虑。但是，
   任意两台计算机之间有源集线器不得超过十一个。当然，这并不限制网络上有源集线器
   的总数。)

4. An active hub to another.
   (有源集线器对有源集线器。)

5. An active hub to passive hub.
   (有源集线器对无源集线器。)

Remember that you cannot connect two passive hubs together.  The power loss
implied by such a connection is too high for the net to operate reliably.
(记住，两个无源集线器不能连在一起。这种连接带来的功率损耗太大，网络无法可靠
工作。)

An example of a typical ARCnet network::
(一个典型 ARCnet 网络的例子：)

	   R                     S - STAR type card
    S------H--------A-------S    R - Terminator
	   |        |            H - Hub
	   |        |            A - Active hub
	   |   S----H----S
	   S        |
		    |
		    S

The BUS topology is very similar to the one used by Ethernet.  The only
difference is in cable and terminators: they should be 93 Ohm.  Ethernet
uses 50 Ohm impedance. You use T connectors to put the computers on a single
line of cable, the bus. You have to put terminators at both ends of the
cable. A typical BUS ARCnet network looks like::
(BUS 拓扑与以太网所用的非常相似。区别只在电缆和端接器：应为 93 Ohm，而以太网使用
50 Ohm 阻抗。用 T 形连接器把各台计算机接到同一条电缆线上，即总线。必须在电缆
两端安装端接器。典型的 BUS 型 ARCnet 网络如下：)

    RT----T------T------T------T------TR
     B    B      B      B      B      B

  B - BUS type card
  R - Terminator
  T - T connector

But that is not all! The two types can be connected together.  According to
the official documentation the only way of connecting them is using an active
hub::
(但这还不是全部！两种类型可以连接在一起。按照官方文档，唯一的连接方式是使用
有源集线器：)

	 A------T------T------TR
	 |      B      B      B
     S---H---S
	 |
	 S

The official docs also state that you can use STAR cards at the ends of
BUS network in place of a BUS card and a terminator::
(官方文档还说，可以把 STAR 卡用在 BUS 网络的两端，代替 BUS 卡加端接器：)

     S------T------T------S
	    B      B

But, according to my own experiments, you can simply hang a BUS type card
anywhere in middle of a cable in a STAR topology network.  And more - you
can use the bus card in place of any star card if you use a terminator. Then
you can build very complicated networks fulfilling all your needs!  An
example::
(不过，根据我自己的实验，你可以把 BUS 型卡直接挂到 STAR 拓扑网络中一段电缆的中间
任何位置。而且——只要使用端接器，还可以用总线卡代替任何星型卡。这样你就能搭建
各种满足自己需要的复杂网络！一个例子：)

				  S
				  |
	   RT------T-------T------H------S
	    B      B       B      |
				  |       R
    S------A------T-------T-------A-------H------TR
	   |      B       B       |       |      B
	   |   S                 BT       |
	   |   |                  |  S----A-----S
    S------H---A----S             |       |
	   |   |      S------T----H---S   |
	   S   S             B    R       S

A basically different cabling scheme is used with Twisted Pair cabling. Each
of the TP cards has two RJ (phone-cord style) connectors.  The cards are
then daisy-chained together using a cable connecting every two neighboring
cards.  The ends are terminated with RJ 93 Ohm terminators which plug into
the empty connectors of cards on the ends of the chain.  An example::
(双绞线(Twisted Pair)布线采用一种截然不同的方案。每张 TP 卡有两个 RJ(电话线式)
插口。然后用电缆把相邻两张卡依次串接(daisy-chain)起来。链条两端用 RJ 93 Ohm
端接器端接，插在链条两端卡片的空余插口上。一个例子：)

	  ___________   ___________
      _R_|_         _|_|_         _|_R_
     |     |       |     |       |     |
     |Card |       |Card |       |Card |
     |_____|       |_____|       |_____|


There are also hubs for the TP topology.  There is nothing difficult
involved in using them; you just connect a TP chain to a hub on any end or
even at both.  This way you can create almost any network configuration.
The maximum of 11 hubs between any two computers on the net applies here as
well.  An example::
(TP 拓扑也有集线器。使用它们毫无难度；只要把 TP 链条的任意一端、甚至两端接到
集线器上即可。这样几乎可以创建任何网络配置。"任意两台计算机之间最多 11 个集线器"
的规则在这里同样适用。一个例子：)

    RP-------P--------P--------H-----P------P-----PR
			       |
      RP-----H--------P--------H-----P------PR
	     |                 |
	     PR                PR

    R - RJ Terminator
    P - TP Card
    H - TP Hub

Like any network, ARCnet has a limited cable length.  These are the maximum
cable lengths between two active ends (an active end being an active hub or
a STAR card).
(和任何网络一样，ARCnet 的电缆长度也是有限的。下面是两个有源端(有源端指有源集线器
或 STAR 卡)之间的最大电缆长度。)

		========== ======= ===========
		RG-62       93 Ohm up to 650 m (最长 650 米)
		RG-59/U     75 Ohm up to 457 m (最长 457 米)
		RG-11/U     75 Ohm up to 533 m (最长 533 米)
		IBM Type 1 150 Ohm up to 200 m (最长 200 米)
		IBM Type 3 100 Ohm up to 100 m (最长 100 米)
		========== ======= ===========

The maximum length of all cables connected to a passive hub is limited to 65
meters for RG-62 cabling; less for others.  You can see that using passive
hubs in a large network is a bad idea. The maximum length of a single "BUS
Trunk" is about 300 meters for RG-62. The maximum distance between the two
most distant points of the net is limited to 3000 meters. The maximum length
of a TP cable between two cards/hubs is 650 meters.
(对于 RG-62 布线，接到无源集线器上的所有电缆总长限制在 65 米以内；其他线缆则更短。
可见在大型网络中使用无源集线器不是好主意。对 RG-62 而言，单段 "BUS Trunk"(总线
干线)的最大长度约为 300 米。网络中相距最远的两点之间最大距离限制为 3000 米。
两块卡/集线器之间 TP 电缆的最大长度为 650 米。)


Setting the Jumpers
(跳线设置)
===================

All ARCnet cards should have a total of four or five different settings:
(所有 ARCnet 卡总共应当有四到五种不同的设置：)

  - the I/O address:  this is the "port" your ARCnet card is on.  Probed
    values in the Linux ARCnet driver are only from 0x200 through 0x3F0. (If
    your card has additional ones, which is possible, please tell me.) This
    should not be the same as any other device on your system.  According to
    a doc I got from Novell, MS Windows prefers values of 0x300 or more,
    eating net connections on my system (at least) otherwise.  My guess is
    this may be because, if your card is at 0x2E0, probing for a serial port
    at 0x2E8 will reset the card and probably mess things up royally.
    (I/O 地址：即你的 ARCnet 卡所在的"端口"。Linux ARCnet 驱动探测的取值只有
    0x200 到 0x3F0。(如果你的卡还有其他取值——这是可能的——请告诉我。)它不应与
    系统中任何其他设备的地址相同。据我从 Novell 得到的一份文档，MS Windows 更
    喜欢 0x300 或更高的值，否则(至少在我的系统上)会吃掉网络连接。我猜这可能是
    因为：如果你的卡在 0x2E0，对 0x2E8 串口的探测将会复位这张卡，多半会把事情
    搞得一团糟。)

	- Avery's favourite: 0x300.
	  (Avery 的最爱：0x300。)

  - the IRQ: on  8-bit cards, it might be 2 (9), 3, 4, 5, or 7.
	     on 16-bit cards, it might be 2 (9), 3, 4, 5, 7, or 10-15.
    (IRQ：8 位卡可能是 2 (9)、3、4、5 或 7；16 位卡可能是 2 (9)、3、4、5、7 或
    10-15。)

    Make sure this is different from any other card on your system.  Note
    that IRQ2 is the same as IRQ9, as far as Linux is concerned.  You can
    "cat /proc/interrupts" for a somewhat complete list of which ones are in
    use at any given time.  Here is a list of common usages from Vojtech
    Pavlik <vojtech@suse.cz>:
    (请确保它与系统中任何其他卡都不同。注意，就 Linux 而言 IRQ2 与 IRQ9 是相同
    的。你可以用 "cat /proc/interrupts" 查看一份相当完整的、任一时刻哪些中断正在
    使用的列表。下面是 Vojtech Pavlik <vojtech@suse.cz> 提供的常见用法列表：)

	("Not on bus" means there is no way for a card to generate this
	interrupt)
	(("Not on bus"(不在总线上)表示卡无法产生该中断))

	======   =========================================================
	IRQ  0   Timer 0 (Not on bus) (定时器 0，不在总线上)
	IRQ  1   Keyboard (Not on bus) (键盘，不在总线上)
	IRQ  2   IRQ Controller 2 (Not on bus, nor does interrupt the CPU) (中断控制器 2，不在总线上，也不会向 CPU 发中断)
	IRQ  3   COM2 (COM2 串口)
	IRQ  4   COM1 (COM1 串口)
	IRQ  5   FREE (LPT2 if you have it; sometimes COM3; maybe PLIP) (空闲，如有 LPT2 则为其所用；有时为 COM3；也可能是 PLIP)
	IRQ  6   Floppy disk controller (软盘控制器)
	IRQ  7   FREE (LPT1 if you don't use the polling driver; PLIP) (空闲，若未使用轮询驱动则为 LPT1；PLIP)
	IRQ  8   Realtime Clock Interrupt (Not on bus) (实时时钟中断，不在总线上)
	IRQ  9   FREE (VGA vertical sync interrupt if enabled) (空闲，若已启用则为 VGA 垂直同步中断)
	IRQ 10   FREE (空闲)
	IRQ 11   FREE (空闲)
	IRQ 12   FREE (空闲)
	IRQ 13   Numeric Coprocessor (Not on bus) (数字协处理器，不在总线上)
	IRQ 14   Fixed Disk Controller (硬盘控制器)
	IRQ 15   FREE (Fixed Disk Controller 2 if you have it) (空闲，如有第二块硬盘控制器则为其所用)
	======   =========================================================


	.. note::

	   IRQ 9 is used on some video cards for the "vertical retrace"
	   interrupt.  This interrupt would have been handy for things like
	   video games, as it occurs exactly once per screen refresh, but
	   unfortunately IBM cancelled this feature starting with the original
	   VGA and thus many VGA/SVGA cards do not support it.  For this
	   reason, no modern software uses this interrupt and it can almost
	   always be safely disabled, if your video card supports it at all.
	   (注意：某些显卡将 IRQ 9 用于"垂直回扫"(vertical retrace)中断。这个中断
	   本来对电子游戏之类的程序很有用，因为它在每次屏幕刷新时恰好发生一次；但
	   遗憾的是 IBM 从最初的 VGA 开始就取消了这一特性，因此许多 VGA/SVGA 卡都
	   不支持它。正因如此，没有现代软件使用这个中断，只要你的显卡支持，几乎总是
	   可以放心地将其禁用。)

	If your card for some reason CANNOT disable this IRQ (usually there
	is a jumper), one solution would be to clip the printed circuit
	contact on the board: it's the fourth contact from the left on the
	back side.  I take no responsibility if you try this.
	(如果你的卡由于某种原因无法禁用这个 IRQ(通常会有一个跳线)，一个办法是剪断
	板上印刷电路的触点：它是背面从左数第四个触点。若你尝试此法，我不承担任何
	责任。)

	- Avery's favourite: IRQ2 (actually IRQ9).  Watch that VGA, though.
	  (Avery 的最爱：IRQ2(实际上是 IRQ9)。不过要当心你的 VGA。)

  - the memory address:  Unlike most cards, ARCnets use "shared memory" for
    copying buffers around.  Make SURE it doesn't conflict with any other
    used memory in your system!
    (内存地址：与大多数卡不同，ARCnet 使用"共享内存"来搬运缓冲区。务必确保它不
    与系统中任何其他已用内存冲突！)

    ::

	A0000		- VGA graphics memory (ok if you don't have VGA)
	B0000		- Monochrome text mode
	C0000		\  One of these is your VGA BIOS - usually C0000.
	E0000		/
	F0000		- System BIOS

    Anything less than 0xA0000 is, well, a BAD idea since it isn't above
    640k.
    (低于 0xA0000 的地址嘛，是个糟糕透顶的主意，因为它没有高于 640k。)

	- Avery's favourite: 0xD0000
	  (Avery 的最爱：0xD0000)

  - the station address:  Every ARCnet card has its own "unique" network
    address from 0 to 255.  Unlike Ethernet, you can set this address
    yourself with a jumper or switch (or on some cards, with special
    software).  Since it's only 8 bits, you can only have 254 ARCnet cards
    on a network.  DON'T use 0 or 255, since these are reserved (although
    neat stuff will probably happen if you DO use them).  By the way, if you
    haven't already guessed, don't set this the same as any other ARCnet on
    your network!
    (站地址：每张 ARCnet 卡都有从 0 到 255 的自己的"唯一"网络地址。与以太网不同，
    这个地址可以由你自己用跳线或拨码开关设置(有些卡可用专门的软件设置)。因为它
    只有 8 位，一个网络上最多只能有 254 张 ARCnet 卡。不要用 0 或 255，它们是
    保留的(不过如果你真用了，多半会发生些有趣的事)。顺便说一句，如果你还没猜到：
    不要把它设置成与网络上任何其他 ARCnet 卡相同！)

	- Avery's favourite:  3 and 4.  Not that it matters.
	  (Avery 的最爱：3 和 4。倒不是说这有多重要。)

  - There may be ETS1 and ETS2 settings.  These may or may not make a
    difference on your card (many manuals call them "reserved"), but are
    used to change the delays used when powering up a computer on the
    network.  This is only necessary when wiring VERY long range ARCnet
    networks, on the order of 4km or so; in any case, the only real
    requirement here is that all cards on the network with ETS1 and ETS2
    jumpers have them in the same position.  Chris Hindy <chrish@io.org>
    sent in a chart with actual values for this:
    (有些卡可能有 ETS1 和 ETS2 设置。它们对你的卡也许有影响，也许没有(许多手册
    称其为"保留")，它们用于改变网络上给计算机上电时所用的延迟。只有在搭建非常
    长距离的 ARCnet 网络(约 4km 量级)时才需要关心它；无论如何，这里唯一真正的
    要求就是：网络上所有带 ETS1 和 ETS2 跳线的卡都把它们置于相同的位置。Chris
    Hindy <chrish@io.org> 提供了一张列有实际数值的表：)

	======= ======= =============== ====================
	ET1	ET2	Response Time	Reconfiguration Time (重配置时间)
	======= ======= =============== ====================
	open	open	74.7us		840us
	open	closed	283.4us		1680us
	closed	open	561.8us		1680us
	closed	closed	1118.6us	1680us
	======= ======= =============== ====================

    Make sure you set ETS1 and ETS2 to the SAME VALUE for all cards on your
    network.
    (务必把网络上所有卡的 ETS1 和 ETS2 设置成相同的值。)

Also, on many cards (not mine, though) there are red and green LED's.
Vojtech Pavlik <vojtech@suse.cz> tells me this is what they mean:
(另外，许多卡上(我的卡倒没有)有红色和绿色 LED。Vojtech Pavlik
<vojtech@suse.cz> 告诉我它们的含义如下：)

	=============== =============== =====================================
	GREEN           RED             Status (状态)
	=============== =============== =====================================
	OFF             OFF             Power off (电源关闭)
	OFF             Short flashes   Cabling problems (broken cable or not
					terminated) (布线问题：电缆断开或未端接)
	OFF (short)     ON              Card init (卡正在初始化)
	ON              ON              Normal state - everything OK, nothing
					happens (正常状态，一切正常，无事发生)
	ON              Long flashes    Data transfer (正在传输数据)
	ON              OFF             Never happens (maybe when wrong ID) (从不出现(也许是 ID 错误时))
	=============== =============== =====================================


The following is all the specific information people have sent me about
their own particular ARCnet cards.  It is officially a mess, and contains
huge amounts of duplicated information.  I have no time to fix it.  If you
want to, PLEASE DO!  Just send me a 'diff -u' of all your changes.
(以下是大家就自己那张 ARCnet 卡发给我的全部具体信息。坦白说这是一团乱麻，包含
大量重复内容。我没有时间整理它。如果你愿意，请务必动手整理！只要把你的全部修改
用 'diff -u' 发给我即可。)

The model # is listed right above specifics for that card, so you should be
able to use your text viewer's "search" function to find the entry you want.
If you don't KNOW what kind of card you have, try looking through the
various diagrams to see if you can tell.
(型号就列在该卡具体说明的正上方，所以你可以用文本查看器的"搜索"功能找到想要的
条目。如果你并不知道自己拥有的是哪种卡，试着把各种图翻一遍，看看能不能认出来。)

If your model isn't listed and/or has different settings, PLEASE PLEASE
tell me.  I had to figure mine out without the manual, and it WASN'T FUN!
(如果你的型号没有列出和/或设置不同，请千万务必告诉我。我当年不得不在没有手册的
情况下摸索出自己的卡，那可一点儿都不好玩！)

Even if your ARCnet model isn't listed, but has the same jumpers as another
model that is, please e-mail me to say so.
(即使你的 ARCnet 型号没有列出，但它的跳线与某个已列出的型号相同，也请发邮件告诉
我一声。)

Cards Listed in this file (in this order, mostly):
(本文件中列出的卡(大体按此顺序)：)

	=============== ======================= ====
	Manufacturer	Model #			Bits
	=============== ======================= ====
	SMC		PC100			8
	SMC		PC110			8
	SMC		PC120			8
	SMC		PC130			8
	SMC		PC270E			8
	SMC		PC500			16
	SMC		PC500Longboard		16
	SMC		PC550Longboard		16
	SMC		PC600			16
	SMC		PC710			8
	SMC?		LCS-8830(-T)		8/16
	Puredata	PDI507			8
	CNet Tech	CN120-Series		8
	CNet Tech	CN160-Series		16
	Lantech?	UM9065L chipset		8
	Acer		5210-003		8
	Datapoint?	LAN-ARC-8		8
	Topware		TA-ARC/10		8
	Thomas-Conrad	500-6242-0097 REV A	8
	Waterloo?	(C)1985 Waterloo Micro. 8
	No Name		--			8/16
	No Name		Taiwan R.O.C?		8
	No Name		Model 9058		8
	Tiara		Tiara Lancard?		8
	=============== ======================= ====


* SMC = Standard Microsystems Corp.
(SMC = Standard Microsystems Corp.，即标准微系统公司)
* CNet Tech = CNet Technology, Inc.
(CNet Tech = CNet Technology, Inc.，即 CNet 技术公司)

Unclassified Stuff
(未分类内容)
==================

  - Please send any other information you can find.
    (请把你找到的任何其他信息都发给我。)

  - And some other stuff (more info is welcome!)::
    (还有一些其他内容(欢迎补充更多信息！)：)

     From: root@ultraworld.xs4all.nl (Timo Hilbrink)
     To: apenwarr@foxnet.net (Avery Pennarun)
     Date: Wed, 26 Oct 1994 02:10:32 +0000 (GMT)
     Reply-To: timoh@xs4all.nl

     [...parts deleted...]

     About the jumpers: On my PC130 there is one more jumper, located near the
     cable-connector and it's for changing to star or bus topology;
     closed: star - open: bus
     On the PC500 are some more jumper-pins, one block labeled with RX,PDN,TXI
     and another with ALE,LA17,LA18,LA19 these are undocumented..

     [...more parts deleted...]

     --- CUT ---

Standard Microsystems Corp (SMC)
(SMC，标准微系统公司)
================================

PC100, PC110, PC120, PC130 (8-bit cards) and PC500, PC600 (16-bit cards)
(PC100、PC110、PC120、PC130(8 位卡)与 PC500、PC600(16 位卡))
------------------------------------------------------------------------

  - mainly from Avery Pennarun <apenwarr@worldvisions.ca>.  Values depicted
    are from Avery's setup.
    (主要来自 Avery Pennarun <apenwarr@worldvisions.ca>。所列取值来自 Avery 的
    配置。)
  - special thanks to Timo Hilbrink <timoh@xs4all.nl> for noting that PC120,
    130, 500, and 600 all have the same switches as Avery's PC100.
    PC500/600 have several extra, undocumented pins though. (?)
    (特别感谢 Timo Hilbrink <timoh@xs4all.nl> 指出 PC120、130、500 和 600 的开关
    都与 Avery 的 PC100 相同。不过 PC500/600 还有几个额外的、未见于文档的引脚。)
  - PC110 settings were verified by Stephen A. Wood <saw@cebaf.gov>
    (PC110 的设置已由 Stephen A. Wood <saw@cebaf.gov> 验证)
  - Also, the JP- and S-numbers probably don't match your card exactly.  Try
    to find jumpers/switches with the same number of settings - it's
    probably more reliable.
    (另外，JP 和 S 的编号多半与你的卡不完全对应。请设法找到设置档位数目相同的
    跳线/开关——这样更可靠。)

::

	     JP5		       [|]    :    :    :    :
	(IRQ Setting)		      IRQ2  IRQ3 IRQ4 IRQ5 IRQ7
			Put exactly one jumper on exactly one set of pins.


				  1  2   3  4  5  6   7  8  9 10
	     S1                /----------------------------------\
	(I/O and Memory        |  1  1 * 0  0  0  0 * 1  1  0  1  |
	 addresses)            \----------------------------------/
				  |--|   |--------|   |--------|
				  (a)       (b)           (m)

			WARNING.  It's very important when setting these which way
			you're holding the card, and which way you think is '1'!

			If you suspect that your settings are not being made
			correctly, try reversing the direction or inverting the
			switch positions.

			a: The first digit of the I/O address.
				Setting		Value
				-------		-----
				00		0
				01		1
				10		2
				11		3

			b: The second digit of the I/O address.
				Setting		Value
				-------		-----
				0000		0
				0001		1
				0010		2
				...		...
				1110		E
				1111		F

			The I/O address is in the form ab0.  For example, if
			a is 0x2 and b is 0xE, the address will be 0x2E0.

			DO NOT SET THIS LESS THAN 0x200!!!!!


			m: The first digit of the memory address.
				Setting		Value
				-------		-----
				0000		0
				0001		1
				0010		2
				...		...
				1110		E
				1111		F

			The memory address is in the form m0000.  For example, if
			m is D, the address will be 0xD0000.

			DO NOT SET THIS TO C0000, F0000, OR LESS THAN A0000!

				  1  2  3  4  5  6  7  8
	     S2                /--------------------------\
	(Station Address)      |  1  1  0  0  0  0  0  0  |
			       \--------------------------/

				Setting		Value
				-------		-----
				00000000	00
				10000000	01
				01000000	02
				...
				01111111	FE
				11111111	FF

			Note that this is binary with the digits reversed!

			DO NOT SET THIS TO 0 OR 255 (0xFF)!


PC130E/PC270E (8-bit cards)
(PC130E/PC270E(8 位卡))
---------------------------

  - from Juergen Seifert <seifert@htwm.de>
    (来自 Juergen Seifert <seifert@htwm.de>)

This description has been written by Juergen Seifert <seifert@htwm.de>
using information from the following Original SMC Manual
(以下说明由 Juergen Seifert <seifert@htwm.de> 依据下列 SMC 原版手册编写)

	     "Configuration Guide for ARCNET(R)-PC130E/PC270 Network
	     Controller Boards Pub. # 900.044A June, 1989"
	     (《ARCNET(R)-PC130E/PC270 网络控制器板卡配置指南》，Pub. # 900.044A，
	     1989 年 6 月)

ARCNET is a registered trademark of the Datapoint Corporation
SMC is a registered trademark of the Standard Microsystems Corporation
(ARCNET 是 Datapoint Corporation 的注册商标。SMC 是 Standard Microsystems
Corporation 的注册商标。)

The PC130E is an enhanced version of the PC130 board, is equipped with a
standard BNC female connector for connection to RG-62/U coax cable.
Since this board is designed both for point-to-point connection in star
networks and for connection to bus networks, it is downwardly compatible
with all the other standard boards designed for coax networks (that is,
the PC120, PC110 and PC100 star topology boards and the PC220, PC210 and
PC200 bus topology boards).
(PC130E 是 PC130 板的增强版，带一个标准 BNC 母头连接器，用于连接 RG-62/U 同轴
电缆。由于这块板既设计用于星型网络中的点对点连接，也可用于连接总线网络，因此
它向下兼容所有其他为同轴网络设计的标准板卡(即 PC120、PC110 和 PC100 星型拓扑
板，以及 PC220、PC210 和 PC200 总线拓扑板)。)

The PC270E is an enhanced version of the PC260 board, is equipped with two
modular RJ11-type jacks for connection to twisted pair wiring.
It can be used in a star or a daisy-chained network.
(PC270E 是 PC260 板的增强版，带两个模块化 RJ11 型插口，用于连接双绞线。它可以
用在星型或菊花链式网络中。)

::

	 8 7 6 5 4 3 2 1
    ________________________________________________________________
   |   |       S1        |                                          |
   |   |_________________|                                          |
   |    Offs|Base |I/O Addr                                         |
   |     RAM Addr |                                              ___|
   |         ___  ___                                       CR3 |___|
   |        |   \/   |                                      CR4 |___|
   |        |  PROM  |                                           ___|
   |        |        |                                        N |   | 8
   |        | SOCKET |                                        o |   | 7
   |        |________|                                        d |   | 6
   |                   ___________________                    e |   | 5
   |                  |                   |                   A | S | 4
   |       |oo| EXT2  |                   |                   d | 2 | 3
   |       |oo| EXT1  |       SMC         |                   d |   | 2
   |       |oo| ROM   |      90C63        |                   r |___| 1
   |       |oo| IRQ7  |                   |               |o|  _____|
   |       |oo| IRQ5  |                   |               |o| | J1  |
   |       |oo| IRQ4  |                   |              STAR |_____|
   |       |oo| IRQ3  |                   |                   | J2  |
   |       |oo| IRQ2  |___________________|                   |_____|
   |___                                               ______________|
       |                                             |
       |_____________________________________________|

Legend::
(图例：)

  SMC 90C63	ARCNET Controller / Transceiver /Logic
  S1	1-3:	I/O Base Address Select
	4-6:	Memory Base Address Select
	7-8:	RAM Offset Select
  S2	1-8:	Node ID Select
  EXT		Extended Timeout Select
  ROM		ROM Enable Select
  STAR		Selected - Star Topology	(PC130E only)
		Deselected - Bus Topology	(PC130E only)
  CR3/CR4	Diagnostic LEDs
  J1		BNC RG62/U Connector		(PC130E only)
  J1		6-position Telephone Jack	(PC270E only)
  J2		6-position Telephone Jack	(PC270E only)

Setting one of the switches to Off/Open means "1", On/Closed means "0".
(把某个开关拨到 Off/Open(断开)表示 "1"，拨到 On/Closed(接通)表示 "0"。)


Setting the Node ID
(设置节点 ID)
^^^^^^^^^^^^^^^^^^^

The eight switches in group S2 are used to set the node ID.
These switches work in a way similar to the PC100-series cards; see that
entry for more information.
(S2 组的八个开关用于设置节点 ID。这些开关的工作方式与 PC100 系列卡类似；更多
信息参见该条目。)


Setting the I/O Base Address
(设置 I/O 基址)
^^^^^^^^^^^^^^^^^^^^^^^^^^^^

The first three switches in switch group S1 are used to select one
of eight possible I/O Base addresses using the following table::
(S1 开关组的前三个开关用于按下表从八个可能的 I/O 基址中选择一个：)


   Switch | Hex I/O
   1 2 3  | Address
   -------|--------
   0 0 0  |  260
   0 0 1  |  290
   0 1 0  |  2E0  (Manufacturer's default)
   0 1 1  |  2F0
   1 0 0  |  300
   1 0 1  |  350
   1 1 0  |  380
   1 1 1  |  3E0


Setting the Base Memory (RAM) buffer Address
(设置内存(RAM)缓冲区基址)
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

The memory buffer requires 2K of a 16K block of RAM. The base of this
16K block can be located in any of eight positions.
Switches 4-6 of switch group S1 select the Base of the 16K block.
Within that 16K address space, the buffer may be assigned any one of four
positions, determined by the offset, switches 7 and 8 of group S1.
(内存缓冲区需要 16K RAM 块中的 2K。这个 16K 块的基址可以位于八个位置中的任意
一个。S1 开关组的开关 4-6 选择该 16K 块的基址。在这 16K 地址空间内，缓冲区由
S1 组的开关 7 和 8 决定的偏移量，可指派到四个位置中的任意一个。)

::

   Switch     | Hex RAM | Hex ROM
   4 5 6  7 8 | Address | Address *)
   -----------|---------|-----------
   0 0 0  0 0 |  C0000  |  C2000
   0 0 0  0 1 |  C0800  |  C2000
   0 0 0  1 0 |  C1000  |  C2000
   0 0 0  1 1 |  C1800  |  C2000
	      |         |
   0 0 1  0 0 |  C4000  |  C6000
   0 0 1  0 1 |  C4800  |  C6000
   0 0 1  1 0 |  C5000  |  C6000
   0 0 1  1 1 |  C5800  |  C6000
	      |         |
   0 1 0  0 0 |  CC000  |  CE000
   0 1 0  0 1 |  CC800  |  CE000
   0 1 0  1 0 |  CD000  |  CE000
   0 1 0  1 1 |  CD800  |  CE000
	      |         |
   0 1 1  0 0 |  D0000  |  D2000  (Manufacturer's default)
   0 1 1  0 1 |  D0800  |  D2000
   0 1 1  1 0 |  D1000  |  D2000
   0 1 1  1 1 |  D1800  |  D2000
	      |         |
   1 0 0  0 0 |  D4000  |  D6000
   1 0 0  0 1 |  D4800  |  D6000
   1 0 0  1 0 |  D5000  |  D6000
   1 0 0  1 1 |  D5800  |  D6000
	      |         |
   1 0 1  0 0 |  D8000  |  DA000
   1 0 1  0 1 |  D8800  |  DA000
   1 0 1  1 0 |  D9000  |  DA000
   1 0 1  1 1 |  D9800  |  DA000
	      |         |
   1 1 0  0 0 |  DC000  |  DE000
   1 1 0  0 1 |  DC800  |  DE000
   1 1 0  1 0 |  DD000  |  DE000
   1 1 0  1 1 |  DD800  |  DE000
	      |         |
   1 1 1  0 0 |  E0000  |  E2000
   1 1 1  0 1 |  E0800  |  E2000
   1 1 1  1 0 |  E1000  |  E2000
   1 1 1  1 1 |  E1800  |  E2000

  *) To enable the 8K Boot PROM install the jumper ROM.
     The default is jumper ROM not installed.
  (要启用 8K Boot PROM，请安装标有 ROM 的跳线。默认情况下未安装跳线。)


Setting the Timeouts and Interrupt
(设置超时与中断)
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

The jumpers labeled EXT1 and EXT2 are used to determine the timeout
parameters. These two jumpers are normally left open.
(标有 EXT1 和 EXT2 的跳线用于确定超时参数。这两个跳线通常保持断开。)

To select a hardware interrupt level set one (only one!) of the jumpers
IRQ2, IRQ3, IRQ4, IRQ5, IRQ7. The Manufacturer's default is IRQ2.
(要选择硬件中断级别，请设置 IRQ2、IRQ3、IRQ4、IRQ5、IRQ7 跳线中的一个(仅限
一个！)。厂商默认值为 IRQ2。)


Configuring the PC130E for Star or Bus Topology
(把 PC130E 配置为星型或总线拓扑)
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

The single jumper labeled STAR is used to configure the PC130E board for
star or bus topology.
When the jumper is installed, the board may be used in a star network, when
it is removed, the board can be used in a bus topology.
(标有 STAR 的单个跳线用于把 PC130E 板配置成星型或总线拓扑。装上跳线时，板可用于
星型网络；取下跳线时，板可用于总线拓扑。)


Diagnostic LEDs
(诊断 LED)
^^^^^^^^^^^^^^^

Two diagnostic LEDs are visible on the rear bracket of the board.
The green LED monitors the network activity: the red one shows the
board activity::
(板上后挡板处可以看到两个诊断 LED。绿色 LED 监视网络活动，红色 LED 显示板卡
活动：)

 Green  | Status               Red      | Status
 -------|-------------------   ---------|-------------------
  on    | normal activity      flash/on | data transfer
  blink | reconfiguration      off      | no data transfer;
  off   | defective board or            | incorrect memory or
	| node ID is zero               | I/O address


PC500/PC550 Longboard (16-bit cards)
(PC500/PC550 长卡(16 位卡))
------------------------------------

  - from Juergen Seifert <seifert@htwm.de>
    (来自 Juergen Seifert <seifert@htwm.de>)


  .. note::

      There is another Version of the PC500 called Short Version, which
      is different in hard- and software! The most important differences
      are:

      - The long board has no Shared memory.
      - On the long board the selection of the interrupt is done by binary
	coded switch, on the short board directly by jumper.
      (注意：还有另一个版本的 PC500，称为短卡(Short Version)，它在硬件和软件上
      都与长卡不同！最重要的区别是：长卡没有共享内存；长卡上中断的选择由二进制
      编码的拨码开关完成，短卡上则直接由跳线完成。)

[Avery's note: pay special attention to that: the long board HAS NO SHARED
MEMORY.  This means the current Linux-ARCnet driver can't use these cards.
I have obtained a PC500Longboard and will be doing some experiments on it in
the future, but don't hold your breath.  Thanks again to Juergen Seifert for
his advice about this!]
(Avery 的注记：请特别注意这一点：长卡没有共享内存。这意味着当前的 Linux-ARCnet
驱动无法使用这些卡。我已经弄到了一块 PC500Longboard，将来会在它上面做一些实验，
不过别太指望。再次感谢 Juergen Seifert 就此提供的建议！)

This description has been written by Juergen Seifert <seifert@htwm.de>
using information from the following Original SMC Manual
(以下说明由 Juergen Seifert <seifert@htwm.de> 依据下列 SMC 原版手册编写)

	 "Configuration Guide for SMC ARCNET-PC500/PC550
	 Series Network Controller Boards Pub. # 900.033 Rev. A
	 November, 1989"
	 (《SMC ARCNET-PC500/PC550 系列网络控制器板卡配置指南》，Pub. # 900.033
	 Rev. A，1989 年 11 月)

ARCNET is a registered trademark of the Datapoint Corporation
SMC is a registered trademark of the Standard Microsystems Corporation
(ARCNET 是 Datapoint Corporation 的注册商标。SMC 是 Standard Microsystems
Corporation 的注册商标。)

The PC500 is equipped with a standard BNC female connector for connection
to RG-62/U coax cable.
The board is designed both for point-to-point connection in star networks
and for connection to bus networks.
(PC500 带一个标准 BNC 母头连接器，用于连接 RG-62/U 同轴电缆。该板既设计用于
星型网络中的点对点连接，也可用于连接总线网络。)

The PC550 is equipped with two modular RJ11-type jacks for connection
to twisted pair wiring.
It can be used in a star or a daisy-chained (BUS) network.
(PC550 带两个模块化 RJ11 型插口，用于连接双绞线。它可以用在星型或菊花链式
(BUS)网络中。)

::

       1
       0 9 8 7 6 5 4 3 2 1     6 5 4 3 2 1
    ____________________________________________________________________
   < |         SW1         | |     SW2     |                            |
   > |_____________________| |_____________|                            |
   <   IRQ    |I/O Addr                                                 |
   >                                                                 ___|
   <                                                            CR4 |___|
   >                                                            CR3 |___|
   <                                                                 ___|
   >                                                              N |   | 8
   <                                                              o |   | 7
   >                                                              d | S | 6
   <                                                              e | W | 5
   >                                                              A | 3 | 4
   <                                                              d |   | 3
   >                                                              d |   | 2
   <                                                              r |___| 1
   >                                                        |o|    _____|
   <                                                        |o|   | J1  |
   >  3 1                                                   JP6   |_____|
   < |o|o| JP2                                                    | J2  |
   > |o|o|                                                        |_____|
   <  4 2__                                               ______________|
   >    |  |                                             |
   <____|  |_____________________________________________|

Legend::
(图例：)

  SW1	1-6:	I/O Base Address Select
	7-10:	Interrupt Select
  SW2	1-6:	Reserved for Future Use
  SW3	1-8:	Node ID Select
  JP2	1-4:	Extended Timeout Select
  JP6		Selected - Star Topology	(PC500 only)
		Deselected - Bus Topology	(PC500 only)
  CR3	Green	Monitors Network Activity
  CR4	Red	Monitors Board Activity
  J1		BNC RG62/U Connector		(PC500 only)
  J1		6-position Telephone Jack	(PC550 only)
  J2		6-position Telephone Jack	(PC550 only)

Setting one of the switches to Off/Open means "1", On/Closed means "0".
(把某个开关拨到 Off/Open 表示 "1"，拨到 On/Closed 表示 "0"。)


Setting the Node ID
(设置节点 ID)
^^^^^^^^^^^^^^^^^^^

The eight switches in group SW3 are used to set the node ID. Each node
attached to the network must have an unique node ID which must be
different from 0.
Switch 1 serves as the least significant bit (LSB).
(SW3 组的八个开关用于设置节点 ID。接入网络的每个节点都必须拥有一个唯一且不为
0 的节点 ID。开关 1 作为最低有效位(LSB)。)

The node ID is the sum of the values of all switches set to "1"
These values are::
(节点 ID 是所有拨到 "1" 的开关取值之和。这些取值为：)

    Switch | Value
    -------|-------
      1    |   1
      2    |   2
      3    |   4
      4    |   8
      5    |  16
      6    |  32
      7    |  64
      8    | 128

Some Examples::
(一些例子：)

    Switch         | Hex     | Decimal
   8 7 6 5 4 3 2 1 | Node ID | Node ID
   ----------------|---------|---------
   0 0 0 0 0 0 0 0 |    not allowed
   0 0 0 0 0 0 0 1 |    1    |    1
   0 0 0 0 0 0 1 0 |    2    |    2
   0 0 0 0 0 0 1 1 |    3    |    3
       . . .       |         |
   0 1 0 1 0 1 0 1 |   55    |   85
       . . .       |         |
   1 0 1 0 1 0 1 0 |   AA    |  170
       . . .       |         |
   1 1 1 1 1 1 0 1 |   FD    |  253
   1 1 1 1 1 1 1 0 |   FE    |  254
   1 1 1 1 1 1 1 1 |   FF    |  255


Setting the I/O Base Address
(设置 I/O 基址)
^^^^^^^^^^^^^^^^^^^^^^^^^^^^

The first six switches in switch group SW1 are used to select one
of 32 possible I/O Base addresses using the following table::
(SW1 开关组的前六个开关用于按下表从 32 个可能的 I/O 基址中选择一个：)

   Switch       | Hex I/O
   6 5  4 3 2 1 | Address
   -------------|--------
   0 1  0 0 0 0 |  200
   0 1  0 0 0 1 |  210
   0 1  0 0 1 0 |  220
   0 1  0 0 1 1 |  230
   0 1  0 1 0 0 |  240
   0 1  0 1 0 1 |  250
   0 1  0 1 1 0 |  260
   0 1  0 1 1 1 |  270
   0 1  1 0 0 0 |  280
   0 1  1 0 0 1 |  290
   0 1  1 0 1 0 |  2A0
   0 1  1 0 1 1 |  2B0
   0 1  1 1 0 0 |  2C0
   0 1  1 1 0 1 |  2D0
   0 1  1 1 1 0 |  2E0 (Manufacturer's default)
   0 1  1 1 1 1 |  2F0
   1 1  0 0 0 0 |  300
   1 1  0 0 0 1 |  310
   1 1  0 0 1 0 |  320
   1 1  0 0 1 1 |  330
   1 1  0 1 0 0 |  340
   1 1  0 1 0 1 |  350
   1 1  0 1 1 0 |  360
   1 1  0 1 1 1 |  370
   1 1  1 0 0 0 |  380
   1 1  1 0 0 1 |  390
   1 1  1 0 1 0 |  3A0
   1 1  1 0 1 1 |  3B0
   1 1  1 1 0 0 |  3C0
   1 1  1 1 0 1 |  3D0
   1 1  1 1 1 0 |  3E0
   1 1  1 1 1 1 |  3F0


Setting the Interrupt
(设置中断)
^^^^^^^^^^^^^^^^^^^^^

Switches seven through ten of switch group SW1 are used to select the
interrupt level. The interrupt level is binary coded, so selections
from 0 to 15 would be possible, but only the following eight values will
be supported: 3, 4, 5, 7, 9, 10, 11, 12.
(SW1 开关组的第七到第十个开关用于选择中断级别。中断级别按二进制编码，因此理论
上可以选择 0 到 15，但只支持以下八个取值：3、4、5、7、9、10、11、12。)

::

   Switch   | IRQ
   10 9 8 7 |
   ---------|--------
    0 0 1 1 |  3
    0 1 0 0 |  4
    0 1 0 1 |  5
    0 1 1 1 |  7
    1 0 0 1 |  9 (=2) (default)
    1 0 1 0 | 10
    1 0 1 1 | 11
    1 1 0 0 | 12


Setting the Timeouts
(设置超时)
^^^^^^^^^^^^^^^^^^^^

The two jumpers JP2 (1-4) are used to determine the timeout parameters.
These two jumpers are normally left open.
Refer to the COM9026 Data Sheet for alternate configurations.
(两个 JP2 跳线(1-4)用于确定超时参数。这两个跳线通常保持断开。其他配置请参阅
COM9026 数据手册。)


Configuring the PC500 for Star or Bus Topology
(把 PC500 配置为星型或总线拓扑)
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

The single jumper labeled JP6 is used to configure the PC500 board for
star or bus topology.
When the jumper is installed, the board may be used in a star network, when
it is removed, the board can be used in a bus topology.
(标有 JP6 的单个跳线用于把 PC500 板配置成星型或总线拓扑。装上跳线时，板可用于
星型网络；取下跳线时，板可用于总线拓扑。)


Diagnostic LEDs
(诊断 LED)
^^^^^^^^^^^^^^^

Two diagnostic LEDs are visible on the rear bracket of the board.
The green LED monitors the network activity: the red one shows the
board activity::
(板上后挡板处可以看到两个诊断 LED。绿色 LED 监视网络活动，红色 LED 显示板卡
活动：)

 Green  | Status               Red      | Status
 -------|-------------------   ---------|-------------------
  on    | normal activity      flash/on | data transfer
  blink | reconfiguration      off      | no data transfer;
  off   | defective board or            | incorrect memory or
	| node ID is zero               | I/O address


PC710 (8-bit card)
(PC710(8 位卡))
------------------

  - from J.S. van Oosten <jvoosten@compiler.tdcnet.nl>
    (来自 J.S. van Oosten <jvoosten@compiler.tdcnet.nl>)

Note: this data is gathered by experimenting and looking at info of other
cards. However, I'm sure I got 99% of the settings right.
(注意：这些数据是通过实验并参考其他卡的资料收集而来的。不过我确信 99% 的设置
都是正确的。)

The SMC710 card resembles the PC270 card, but is much more basic (i.e. no
LEDs, RJ11 jacks, etc.) and 8 bit. Here's a little drawing::
(SMC710 卡与 PC270 卡相似，但要简陋得多(即没有 LED、RJ11 插口等)，而且是 8 位的。
下面是一张小图：)

    _______________________________________
   | +---------+  +---------+              |____
   | |   S2    |  |   S1    |              |
   | +---------+  +---------+              |
   |                                       |
   |  +===+    __                          |
   |  | R |   |  | X-tal                 ###___
   |  | O |   |__|                      ####__'|
   |  | M |    ||                        ###
   |  +===+                                |
   |                                       |
   |   .. JP1   +----------+               |
   |   ..       | big chip |               |
   |   ..       |  90C63   |               |
   |   ..       |          |               |
   |   ..       +----------+               |
    -------                     -----------
	   |||||||||||||||||||||

The row of jumpers at JP1 actually consists of 8 jumpers, (sometimes
labelled) the same as on the PC270, from top to bottom: EXT2, EXT1, ROM,
IRQ7, IRQ5, IRQ4, IRQ3, IRQ2 (gee, wonder what they would do? :-) )
(JP1 处的一排跳线实际由 8 个跳线组成，(有时标有标签)与 PC270 上的相同，从上到下
依次为：EXT2、EXT1、ROM、IRQ7、IRQ5、IRQ4、IRQ3、IRQ2(咦，猜猜它们是干什么
用的？ :-) ))

S1 and S2 perform the same function as on the PC270, only their numbers
are swapped (S1 is the nodeaddress, S2 sets IO- and RAM-address).
(S1 和 S2 的功能与 PC270 上的相同，只是编号互换了(S1 是节点地址，S2 设置 IO 和
RAM 地址)。)

I know it works when connected to a PC110 type ARCnet board.
(我知道它在连接到 PC110 型 ARCnet 板时可以工作。)


*****************************************************************************

Possibly SMC
(疑为 SMC)
============

LCS-8830(-T) (8 and 16-bit cards)
(LCS-8830(-T)(8 位和 16 位卡))
---------------------------------

  - from Mathias Katzer <mkatzer@HRZ.Uni-Bielefeld.DE>
    (来自 Mathias Katzer <mkatzer@HRZ.Uni-Bielefeld.DE>)
  - Marek Michalkiewicz <marekm@i17linuxb.ists.pwr.wroc.pl> says the
    LCS-8830 is slightly different from LCS-8830-T.  These are 8 bit, BUS
    only (the JP0 jumper is hardwired), and BNC only.
    (Marek Michalkiewicz <marekm@i17linuxb.ists.pwr.wroc.pl> 说 LCS-8830 与
    LCS-8830-T 略有不同。它们是 8 位、仅支持 BUS(JP0 跳线被硬连)、且只有 BNC
    接口。)

This is a LCS-8830-T made by SMC, I think ('SMC' only appears on one PLCC,
nowhere else, not even on the few Xeroxed sheets from the manual).
(我认为这是 SMC 制造的 LCS-8830-T('SMC' 只出现在一颗 PLCC 芯片上，其他任何地方
都没有，甚至连手册的几页复印件上也没有)。)

SMC ARCnet Board Type LCS-8830-T::
(由 SMC 制造的 LCS-8830-T 型 ARCnet 板：)

     ------------------------------------
    |                                    |
    |              JP3 88  8 JP2         |
    |       #####      | \               |
    |       #####    ET1 ET2          ###|
    |                              8  ###|
    |  U3   SW 1                  JP0 ###|  Phone Jacks
    |  --                             ###|
    | |  |                               |
    | |  |   SW2                         |
    | |  |                               |
    | |  |  #####                        |
    |  --   #####                       ####  BNC Connector
    |                                   ####
    |   888888 JP1                       |
    |   234567                           |
     --                           -------
       |||||||||||||||||||||||||||
	--------------------------


  SW1: DIP-Switches for Station Address
  (SW1：用于站点地址的 DIP 拨码开关)
  SW2: DIP-Switches for Memory Base and I/O Base addresses
  (SW2：用于内存基址和 I/O 基址的 DIP 拨码开关)

  JP0: If closed, internal termination on (default open)
  (JP0：闭合时启用内部端接(默认断开))
  JP1: IRQ Jumpers
  (JP1：IRQ 跳线)
  JP2: Boot-ROM enabled if closed
  (JP2：闭合时启用 Boot-ROM)
  JP3: Jumpers for response timeout
  (JP3：响应超时跳线)

  U3: Boot-ROM Socket
  (U3：Boot-ROM 插座)


  ET1 ET2     Response Time     Idle Time    Reconfiguration Time
  (ET1、ET2 与响应时间、空闲时间、重配置时间的对应关系：)

		 78                86               840
   X            285               316              1680
       X        563               624              1680
   X   X       1130              1237              1680

  (X means closed jumper)
  (X 表示跳线闭合)

  (DIP-Switch downwards means "0")
  (DIP 开关拨向下表示 "0")

The station address is binary-coded with SW1.
(站点地址由 SW1 按二进制编码。)

The I/O base address is coded with DIP-Switches 6,7 and 8 of SW2:
(I/O 基址由 SW2 的 DIP 开关 6、7 和 8 编码：)

========	========
Switches        Base
678             Address (地址)
========	========
000		260-26f
100		290-29f
010		2e0-2ef
110		2f0-2ff
001		300-30f
101		350-35f
011		380-38f
111 		3e0-3ef
========	========


DIP Switches 1-5 of SW2 encode the RAM and ROM Address Range:
(SW2 的 DIP 开关 1-5 编码 RAM 和 ROM 的地址范围：)

========        ============= ================
Switches        RAM           ROM
12345           Address Range  Address Range (地址范围)
========        ============= ================
00000		C:0000-C:07ff	C:2000-C:3fff
10000		C:0800-C:0fff
01000		C:1000-C:17ff
11000		C:1800-C:1fff
00100		C:4000-C:47ff	C:6000-C:7fff
10100		C:4800-C:4fff
01100		C:5000-C:57ff
11100		C:5800-C:5fff
00010		C:C000-C:C7ff	C:E000-C:ffff
10010		C:C800-C:Cfff
01010		C:D000-C:D7ff
11010		C:D800-C:Dfff
00110		D:0000-D:07ff	D:2000-D:3fff
10110		D:0800-D:0fff
01110		D:1000-D:17ff
11110		D:1800-D:1fff
00001		D:4000-D:47ff	D:6000-D:7fff
10001		D:4800-D:4fff
01001		D:5000-D:57ff
11001		D:5800-D:5fff
00101		D:8000-D:87ff	D:A000-D:bfff
10101		D:8800-D:8fff
01101		D:9000-D:97ff
11101		D:9800-D:9fff
00011		D:C000-D:c7ff	D:E000-D:ffff
10011		D:C800-D:cfff
01011		D:D000-D:d7ff
11011		D:D800-D:dfff
00111		E:0000-E:07ff	E:2000-E:3fff
10111		E:0800-E:0fff
01111		E:1000-E:17ff
11111		E:1800-E:1fff
========        ============= ================


PureData Corp
(PureData 公司)
=============

PDI507 (8-bit card)
(PDI507(8 位卡))
--------------------

  - from Mark Rejhon <mdrejhon@magi.com> (slight modifications by Avery)
    (来自 Mark Rejhon <mdrejhon@magi.com>(Avery 略作修改))
  - Avery's note: I think PDI508 cards (but definitely NOT PDI508Plus cards)
    are mostly the same as this.  PDI508Plus cards appear to be mainly
    software-configured.
    (Avery 的注记：我认为 PDI508 卡(但绝对不包括 PDI508Plus 卡)与这张卡大体
    相同。PDI508Plus 卡看起来主要通过软件配置。)

Jumpers:
(跳线：)

	There is a jumper array at the bottom of the card, near the edge
	connector.  This array is labelled J1.  They control the IRQs and
	something else.  Put only one jumper on the IRQ pins.
	(卡底部靠近边缘连接器处有一组跳线阵列。该阵列标为 J1。它们控制 IRQ 和其他
	一些东西。IRQ 引脚上只能插一个跳线帽。)

	ETS1, ETS2 are for timing on very long distance networks.  See the
	more general information near the top of this file.
	(ETS1、ETS2 用于超长距离网络的时序。更一般的信息见本文件开头附近。)

	There is a J2 jumper on two pins.  A jumper should be put on them,
	since it was already there when I got the card.  I don't know what
	this jumper is for though.
	(有一个 J2 跳线跨在两个引脚上。应该把跳线帽插上，因为我拿到卡时它就已经
	插在那里了。不过我不知道这个跳线是干什么用的。)

	There is a two-jumper array for J3.  I don't know what it is for,
	but there were already two jumpers on it when I got the card.  It's
	a six pin grid in a two-by-three fashion.  The jumpers were
	configured as follows::
	(J3 是一个两跳线阵列。我不知道它是干什么用的，但我拿到卡时上面已经插着
	两个跳线帽。它是一个两行三列的六引脚阵列。跳线的配置如下：)

	   .-------.
	 o | o   o |
	   :-------:    ------> Accessible end of card with connectors
	 o | o   o |             in this direction ------->
	   `-------'

Carl de Billy <CARL@carainfo.com> explains J3 and J4:
(Carl de Billy <CARL@carainfo.com> 对 J3 和 J4 的解释如下：)

   J3 Diagram::
   (J3 示意图：)

	   .-------.
	 o | o   o |
	   :-------:    TWIST Technology
	 o | o   o |
	   `-------'
	   .-------.
	   | o   o | o
	   :-------:    COAX Technology
	   | o   o | o
	   `-------'

  - If using coax cable in a bus topology the J4 jumper must be removed;
    place it on one pin.
    (如果在总线拓扑中使用同轴电缆，必须取下 J4 跳线，把它插到一个引脚上收好。)

  - If using bus topology with twisted pair wiring move the J3
    jumpers so they connect the middle pin and the pins closest to the RJ11
    Connectors.  Also the J4 jumper must be removed; place it on one pin of
    J4 jumper for storage.
    (如果在双绞线布线中使用总线拓扑，移动 J3 跳线，使其连接中间引脚和最靠近
    RJ11 连接器的引脚。同时必须取下 J4 跳线，把它插到 J4 的一个引脚上收好。)

  - If using  star topology with twisted pair wiring move the J3
    jumpers so they connect the middle pin and the pins closest to the RJ11
    connectors.
    (如果在双绞线布线中使用星型拓扑，移动 J3 跳线，使其连接中间引脚和最靠近
    RJ11 连接器的引脚。)


DIP Switches:
(DIP 拨码开关：)

	The DIP switches accessible on the accessible end of the card while
	it is installed, is used to set the ARCnet address.  There are 8
	switches.  Use an address from 1 to 254
	(卡安装好之后，在可触及一端可以操作的 DIP 开关用于设置 ARCnet 地址。共有
	8 个开关。请使用 1 到 254 之间的地址)

	==========      =========================
	Switch No.	ARCnet address (ARCnet 地址)
	12345678
	==========      =========================
	00000000	FF  	(Don't use this!) (请勿使用！)
	00000001	FE
	00000010	FD
	...
	11111101	2
	11111110	1
	11111111	0	(Don't use this!) (请勿使用！)
	==========      =========================

	There is another array of eight DIP switches at the top of the
	card.  There are five labelled MS0-MS4 which seem to control the
	memory address, and another three labelled IO0-IO2 which seem to
	control the base I/O address of the card.
	(卡的顶部还有一组八个的 DIP 开关阵列。其中五个标为 MS0-MS4，似乎控制内存
	地址；另外三个标为 IO0-IO2，似乎控制卡的 I/O 基址。)

	This was difficult to test by trial and error, and the I/O addresses
	are in a weird order.  This was tested by setting the DIP switches,
	rebooting the computer, and attempting to load ARCETHER at various
	addresses (mostly between 0x200 and 0x400).  The address that caused
	the red transmit LED to blink, is the one that I thought works.
	(这很难通过反复试错来测试，而且 I/O 地址的排列顺序很怪。测试方法是：设置
	DIP 开关，重启计算机，然后尝试在各个地址(大多在 0x200 到 0x400 之间)加载
	ARCETHER。能让红色发送 LED 闪烁的那个地址，就是我认为可用的地址。)

	Also, the address 0x3D0 seem to have a special meaning, since the
	ARCETHER packet driver loaded fine, but without the red LED
	blinking.  I don't know what 0x3D0 is for though.  I recommend using
	an address of 0x300 since Windows may not like addresses below
	0x300.
	(另外，地址 0x3D0 似乎有特殊含义，因为 ARCETHER 包驱动加载成功了，但红色
	LED 没有闪烁。不过我不知道 0x3D0 是干什么用的。我建议使用 0x300，因为
	Windows 可能不喜欢低于 0x300 的地址。)

	=============   ===========
	IO Switch No.   I/O address (I/O 地址)
	210
	=============   ===========
	111             0x260
	110             0x290
	101             0x2E0
	100             0x2F0
	011             0x300
	010             0x350
	001             0x380
	000             0x3E0
	=============   ===========

	The memory switches set a reserved address space of 0x1000 bytes
	(0x100 segment units, or 4k).  For example if I set an address of
	0xD000, it will use up addresses 0xD000 to 0xD100.
	(内存开关预留一段 0x1000 字节(0x100 个段单位，即 4k)的地址空间。例如，如果
	我设置地址 0xD000，它将占用 0xD000 到 0xD100 的地址。)

	The memory switches were tested by booting using QEMM386 stealth,
	and using LOADHI to see what address automatically became excluded
	from the upper memory regions, and then attempting to load ARCETHER
	using these addresses.
	(内存开关的测试方法是：用 QEMM386 stealth 模式引导，用 LOADHI 观察哪个地址
	自动被排除在上位内存区之外，然后尝试用这些地址加载 ARCETHER。)

	I recommend using an ARCnet memory address of 0xD000, and putting
	the EMS page frame at 0xC000 while using QEMM stealth mode.  That
	way, you get contiguous high memory from 0xD100 almost all the way
	the end of the megabyte.
	(我建议使用 0xD000 作为 ARCnet 内存地址，并在使用 QEMM stealth 模式时把 EMS
	页框放在 0xC000。这样，你就能获得从 0xD100 直到几乎 1MB 末端的连续高端
	内存。)

	Memory Switch 0 (MS0) didn't seem to work properly when set to OFF
	on my card.  It could be malfunctioning on my card.  Experiment with
	it ON first, and if it doesn't work, set it to OFF.  (It may be a
	modifier for the 0x200 bit?)
	(在我的卡上，内存开关 0(MS0)设为 OFF 时似乎不能正常工作。可能是我的卡上
	这个开关出了故障。先试着把它设为 ON，如果不行，再设为 OFF。(它也许是 0x200
	位的修饰开关？))

	=============   ============================================
	MS Switch No.
	43210           Memory address (内存地址)
	=============   ============================================
	00001           0xE100  (guessed - was not detected by QEMM) (猜测值，QEMM 未检测到)
	00011           0xE000  (guessed - was not detected by QEMM) (猜测值，QEMM 未检测到)
	00101           0xDD00
	00111           0xDC00
	01001           0xD900
	01011           0xD800
	01101           0xD500
	01111           0xD400
	10001           0xD100
	10011           0xD000
	10101           0xCD00
	10111           0xCC00
	11001           0xC900 (guessed - crashes tested system) (猜测值，会使被测系统崩溃)
	11011           0xC800 (guessed - crashes tested system) (猜测值，会使被测系统崩溃)
	11101           0xC500 (guessed - crashes tested system) (猜测值，会使被测系统崩溃)
	11111           0xC400 (guessed - crashes tested system) (猜测值，会使被测系统崩溃)
	=============   ============================================

CNet Technology Inc. (8-bit cards)
(CNet Technology Inc.(8 位卡))
==================================

120 Series (8-bit cards)
(120 系列(8 位卡))
------------------------

  - from Juergen Seifert <seifert@htwm.de>
    (来自 Juergen Seifert <seifert@htwm.de>)

This description has been written by Juergen Seifert <seifert@htwm.de>
using information from the following Original CNet Manual
(以下说明由 Juergen Seifert <seifert@htwm.de> 依据下列 CNet 原版手册编写)

	      "ARCNET USER'S MANUAL for
	      CN120A
	      CN120AB
	      CN120TP
	      CN120ST
	      CN120SBT
	      P/N:12-01-0007
	      Revision 3.00"
	      (《ARCNET 用户手册：CN120A、CN120AB、CN120TP、CN120ST、CN120SBT》，
	      P/N:12-01-0007，修订版 3.00)

ARCNET is a registered trademark of the Datapoint Corporation
(ARCNET 是 Datapoint Corporation 的注册商标。)

- P/N 120A   ARCNET 8 bit XT/AT Star
  (P/N 120A，ARCNET 8 位 XT/AT 星型)
- P/N 120AB  ARCNET 8 bit XT/AT Bus
  (P/N 120AB，ARCNET 8 位 XT/AT 总线)
- P/N 120TP  ARCNET 8 bit XT/AT Twisted Pair
  (P/N 120TP，ARCNET 8 位 XT/AT 双绞线)
- P/N 120ST  ARCNET 8 bit XT/AT Star, Twisted Pair
  (P/N 120ST，ARCNET 8 位 XT/AT 星型、双绞线)
- P/N 120SBT ARCNET 8 bit XT/AT Star, Bus, Twisted Pair
  (P/N 120SBT，ARCNET 8 位 XT/AT 星型、总线、双绞线)

::

    __________________________________________________________________
   |                                                                  |
   |                                                               ___|
   |                                                          LED |___|
   |                                                               ___|
   |                                                            N |   | ID7
   |                                                            o |   | ID6
   |                                                            d | S | ID5
   |                                                            e | W | ID4
   |                     ___________________                    A | 2 | ID3
   |                    |                   |                   d |   | ID2
   |                    |                   |  1 2 3 4 5 6 7 8  d |   | ID1
   |                    |                   | _________________ r |___| ID0
   |                    |      90C65        ||       SW1       |  ____|
   |  JP 8 7            |                   ||_________________| |    |
   |    |o|o|  JP1      |                   |                    | J2 |
   |    |o|o|  |oo|     |                   |         JP 1 1 1   |    |
   |   ______________   |                   |            0 1 2   |____|
   |  |  PROM        |  |___________________|           |o|o|o|  _____|
   |  >  SOCKET      |  JP 6 5 4 3 2                    |o|o|o| | J1  |
   |  |______________|    |o|o|o|o|o|                   |o|o|o| |_____|
   |_____                 |o|o|o|o|o|                   ______________|
	 |                                             |
	 |_____________________________________________|

Legend::
(图例：)

  90C65       ARCNET Probe
  S1  1-5:    Base Memory Address Select
      6-8:    Base I/O Address Select
  S2  1-8:    Node ID Select (ID0-ID7)
  JP1     ROM Enable Select
  JP2     IRQ2
  JP3     IRQ3
  JP4     IRQ4
  JP5     IRQ5
  JP6     IRQ7
  JP7/JP8     ET1, ET2 Timeout Parameters
  JP10/JP11   Coax / Twisted Pair Select  (CN120ST/SBT only)
  JP12        Terminator Select       (CN120AB/ST/SBT only)
  J1      BNC RG62/U Connector        (all except CN120TP)
  J2      Two 6-position Telephone Jack   (CN120TP/ST/SBT only)

Setting one of the switches to Off means "1", On means "0".
(把某个开关拨到 Off 表示 "1"，拨到 On 表示 "0"。)


Setting the Node ID
(设置节点 ID)
^^^^^^^^^^^^^^^^^^^

The eight switches in SW2 are used to set the node ID. Each node attached
to the network must have an unique node ID which must be different from 0.
Switch 1 (ID0) serves as the least significant bit (LSB).
(SW2 的八个开关用于设置节点 ID。接入网络的每个节点都必须拥有一个唯一且不为 0
的节点 ID。开关 1(ID0)作为最低有效位(LSB)。)

The node ID is the sum of the values of all switches set to "1"
These values are:
(节点 ID 是所有拨到 "1" 的开关取值之和。这些取值为：)

   =======  ======  =====
   Switch   Label   Value
   =======  ======  =====
     1      ID0       1
     2      ID1       2
     3      ID2       4
     4      ID3       8
     5      ID4      16
     6      ID5      32
     7      ID6      64
     8      ID7     128
   =======  ======  =====

Some Examples::
(一些例子：)

    Switch         | Hex     | Decimal
   8 7 6 5 4 3 2 1 | Node ID | Node ID
   ----------------|---------|---------
   0 0 0 0 0 0 0 0 |    not allowed
   0 0 0 0 0 0 0 1 |    1    |    1
   0 0 0 0 0 0 1 0 |    2    |    2
   0 0 0 0 0 0 1 1 |    3    |    3
       . . .       |         |
   0 1 0 1 0 1 0 1 |   55    |   85
       . . .       |         |
   1 0 1 0 1 0 1 0 |   AA    |  170
       . . .       |         |
   1 1 1 1 1 1 0 1 |   FD    |  253
   1 1 1 1 1 1 1 0 |   FE    |  254
   1 1 1 1 1 1 1 1 |   FF    |  255


Setting the I/O Base Address
(设置 I/O 基址)
^^^^^^^^^^^^^^^^^^^^^^^^^^^^

The last three switches in switch block SW1 are used to select one
of eight possible I/O Base addresses using the following table::
(SW1 开关组的最后三个开关用于按下表从八个可能的 I/O 基址中选择一个：)


   Switch      | Hex I/O
    6   7   8  | Address
   ------------|--------
   ON  ON  ON  |  260
   OFF ON  ON  |  290
   ON  OFF ON  |  2E0  (Manufacturer's default)
   OFF OFF ON  |  2F0
   ON  ON  OFF |  300
   OFF ON  OFF |  350
   ON  OFF OFF |  380
   OFF OFF OFF |  3E0


Setting the Base Memory (RAM) buffer Address
(设置内存(RAM)缓冲区基址)
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

The memory buffer (RAM) requires 2K. The base of this buffer can be
located in any of eight positions. The address of the Boot Prom is
memory base + 8K or memory base + 0x2000.
Switches 1-5 of switch block SW1 select the Memory Base address.
(内存缓冲区(RAM)需要 2K。缓冲区基址可位于八个位置中的任意一个。Boot Prom 的
地址为内存基址 + 8K，即内存基址 + 0x2000。SW1 开关组的开关 1-5 选择内存基址。)

::

   Switch              | Hex RAM | Hex ROM
    1   2   3   4   5  | Address | Address *)
   --------------------|---------|-----------
   ON  ON  ON  ON  ON  |  C0000  |  C2000
   ON  ON  OFF ON  ON  |  C4000  |  C6000
   ON  ON  ON  OFF ON  |  CC000  |  CE000
   ON  ON  OFF OFF ON  |  D0000  |  D2000  (Manufacturer's default)
   ON  ON  ON  ON  OFF |  D4000  |  D6000
   ON  ON  OFF ON  OFF |  D8000  |  DA000
   ON  ON  ON  OFF OFF |  DC000  |  DE000
   ON  ON  OFF OFF OFF |  E0000  |  E2000

  *) To enable the Boot ROM install the jumper JP1
  (要启用 Boot ROM，请安装跳线 JP1)

.. note::

      Since the switches 1 and 2 are always set to ON it may be possible
      that they can be used to add an offset of 2K, 4K or 6K to the base
      address, but this feature is not documented in the manual and I
      haven't tested it yet.
      (注意：由于开关 1 和 2 总是设为 ON，它们或许可以用来给基址加上 2K、4K 或
      6K 的偏移量，但手册没有记载这一特性，我也还没测试过。)


Setting the Interrupt Line
(设置中断线)
^^^^^^^^^^^^^^^^^^^^^^^^^^

To select a hardware interrupt level install one (only one!) of the jumpers
JP2, JP3, JP4, JP5, JP6. JP2 is the default::
(要选择硬件中断级别，请安装 JP2、JP3、JP4、JP5、JP6 跳线中的一个(仅限一个！)。
JP2 为默认值：)

   Jumper | IRQ
   -------|-----
     2    |  2
     3    |  3
     4    |  4
     5    |  5
     6    |  7


Setting the Internal Terminator on CN120AB/TP/SBT
(设置 CN120AB/TP/SBT 的内部端接器)
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

The jumper JP12 is used to enable the internal terminator::
(跳线 JP12 用于启用内部端接器：)

			 -----
       0                |  0  |
     -----   ON         |     |  ON
    |  0  |             |  0  |
    |     |  OFF         -----   OFF
    |  0  |                0
     -----
   Terminator          Terminator
    disabled            enabled


Selecting the Connector Type on CN120ST/SBT
(在 CN120ST/SBT 上选择连接器类型)
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

::

     JP10    JP11        JP10    JP11
			 -----   -----
       0       0        |  0  | |  0  |
     -----   -----      |     | |     |
    |  0  | |  0  |     |  0  | |  0  |
    |     | |     |      -----   -----
    |  0  | |  0  |        0       0
     -----   -----
     Coaxial Cable       Twisted Pair Cable
       (Default)


Setting the Timeout Parameters
(设置超时参数)
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

The jumpers labeled EXT1 and EXT2 are used to determine the timeout
parameters. These two jumpers are normally left open.
(标有 EXT1 和 EXT2 的跳线用于确定超时参数。这两个跳线通常保持断开。)


CNet Technology Inc. (16-bit cards)
(CNet Technology Inc.(16 位卡))
===================================

160 Series (16-bit cards)
(160 系列(16 位卡))
-------------------------

  - from Juergen Seifert <seifert@htwm.de>
    (来自 Juergen Seifert <seifert@htwm.de>)

This description has been written by Juergen Seifert <seifert@htwm.de>
using information from the following Original CNet Manual
(以下说明由 Juergen Seifert <seifert@htwm.de> 依据下列 CNet 原版手册编写)

	      "ARCNET USER'S MANUAL for
	      CN160A CN160AB CN160TP
	      P/N:12-01-0006 Revision 3.00"
	      (《ARCNET 用户手册：CN160A、CN160AB、CN160TP》，P/N:12-01-0006，
	      修订版 3.00)

ARCNET is a registered trademark of the Datapoint Corporation
(ARCNET 是 Datapoint Corporation 的注册商标。)

- P/N 160A   ARCNET 16 bit XT/AT Star
  (P/N 160A，ARCNET 16 位 XT/AT 星型)
- P/N 160AB  ARCNET 16 bit XT/AT Bus
  (P/N 160AB，ARCNET 16 位 XT/AT 总线)
- P/N 160TP  ARCNET 16 bit XT/AT Twisted Pair
  (P/N 160TP，ARCNET 16 位 XT/AT 双绞线)

::

   ___________________________________________________________________
  <                             _________________________          ___|
  >               |oo| JP2     |                         |    LED |___|
  <               |oo| JP1     |        9026             |    LED |___|
  >                            |_________________________|         ___|
  <                                                             N |   | ID7
  >                                                      1      o |   | ID6
  <                                    1 2 3 4 5 6 7 8 9 0      d | S | ID5
  >         _______________           _____________________     e | W | ID4
  <        |     PROM      |         |         SW1         |    A | 2 | ID3
  >        >    SOCKET     |         |_____________________|    d |   | ID2
  <        |_______________|          | IO-Base   | MEM   |     d |   | ID1
  >                                                             r |___| ID0
  <                                                               ____|
  >                                                              |    |
  <                                                              | J1 |
  >                                                              |    |
  <                                                              |____|
  >                            1 1 1 1                                |
  <  3 4 5 6 7      JP     8 9 0 1 2 3                                |
  > |o|o|o|o|o|           |o|o|o|o|o|o|                               |
  < |o|o|o|o|o| __        |o|o|o|o|o|o|                    ___________|
  >            |  |                                       |
  <____________|  |_______________________________________|

Legend::
(图例：)

  9026            ARCNET Probe
  SW1 1-6:    Base I/O Address Select
      7-10:   Base Memory Address Select
  SW2 1-8:    Node ID Select (ID0-ID7)
  JP1/JP2     ET1, ET2 Timeout Parameters
  JP3-JP13    Interrupt Select
  J1      BNC RG62/U Connector        (CN160A/AB only)
  J1      Two 6-position Telephone Jack   (CN160TP only)
  LED

Setting one of the switches to Off means "1", On means "0".
(把某个开关拨到 Off 表示 "1"，拨到 On 表示 "0"。)


Setting the Node ID
(设置节点 ID)
^^^^^^^^^^^^^^^^^^^

The eight switches in SW2 are used to set the node ID. Each node attached
to the network must have an unique node ID which must be different from 0.
Switch 1 (ID0) serves as the least significant bit (LSB).
(SW2 的八个开关用于设置节点 ID。接入网络的每个节点都必须拥有一个唯一且不为 0
的节点 ID。开关 1(ID0)作为最低有效位(LSB)。)

The node ID is the sum of the values of all switches set to "1"
These values are::
(节点 ID 是所有拨到 "1" 的开关取值之和。这些取值为：)

   Switch | Label | Value
   -------|-------|-------
     1    | ID0   |   1
     2    | ID1   |   2
     3    | ID2   |   4
     4    | ID3   |   8
     5    | ID4   |  16
     6    | ID5   |  32
     7    | ID6   |  64
     8    | ID7   | 128

Some Examples::
(一些例子：)

    Switch         | Hex     | Decimal
   8 7 6 5 4 3 2 1 | Node ID | Node ID
   ----------------|---------|---------
   0 0 0 0 0 0 0 0 |    not allowed
   0 0 0 0 0 0 0 1 |    1    |    1
   0 0 0 0 0 0 1 0 |    2    |    2
   0 0 0 0 0 0 1 1 |    3    |    3
       . . .       |         |
   0 1 0 1 0 1 0 1 |   55    |   85
       . . .       |         |
   1 0 1 0 1 0 1 0 |   AA    |  170
       . . .       |         |
   1 1 1 1 1 1 0 1 |   FD    |  253
   1 1 1 1 1 1 1 0 |   FE    |  254
   1 1 1 1 1 1 1 1 |   FF    |  255


Setting the I/O Base Address
(设置 I/O 基址)
^^^^^^^^^^^^^^^^^^^^^^^^^^^^

The first six switches in switch block SW1 are used to select the I/O Base
address using the following table::
(SW1 开关组的前六个开关用于按下表选择 I/O 基址：)

	     Switch        | Hex I/O
    1   2   3   4   5   6  | Address
   ------------------------|--------
   OFF ON  ON  OFF OFF ON  |  260
   OFF ON  OFF ON  ON  OFF |  290
   OFF ON  OFF OFF OFF ON  |  2E0  (Manufacturer's default)
   OFF ON  OFF OFF OFF OFF |  2F0
   OFF OFF ON  ON  ON  ON  |  300
   OFF OFF ON  OFF ON  OFF |  350
   OFF OFF OFF ON  ON  ON  |  380
   OFF OFF OFF OFF OFF ON  |  3E0

Note: Other IO-Base addresses seem to be selectable, but only the above
      combinations are documented.
(注意：其他 IO 基址似乎也可以选择，但只有上述组合在文档中有记载。)


Setting the Base Memory (RAM) buffer Address
(设置内存(RAM)缓冲区基址)
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

The switches 7-10 of switch block SW1 are used to select the Memory
Base address of the RAM (2K) and the PROM::
(SW1 开关组的开关 7-10 用于选择 RAM(2K)和 PROM 的内存基址：)

   Switch          | Hex RAM | Hex ROM
    7   8   9  10  | Address | Address
   ----------------|---------|-----------
   OFF OFF ON  ON  |  C0000  |  C8000
   OFF OFF ON  OFF |  D0000  |  D8000 (Default)
   OFF OFF OFF ON  |  E0000  |  E8000

.. note::

      Other MEM-Base addresses seem to be selectable, but only the above
      combinations are documented.
      (注意：其他 MEM 基址似乎也可以选择，但只有上述组合在文档中有记载。)


Setting the Interrupt Line
(设置中断线)
^^^^^^^^^^^^^^^^^^^^^^^^^^

To select a hardware interrupt level install one (only one!) of the jumpers
JP3 through JP13 using the following table::
(要选择硬件中断级别，请按下表安装 JP3 到 JP13 跳线中的一个(仅限一个！)：)

   Jumper | IRQ
   -------|-----------------
     3    |  14
     4    |  15
     5    |  12
     6    |  11
     7    |  10
     8    |   3
     9    |   4
    10    |   5
    11    |   6
    12    |   7
    13    |   2 (=9) Default!

.. note::

       - Do not use JP11=IRQ6, it may conflict with your Floppy Disk
	 Controller
       - Use JP3=IRQ14 only, if you don't have an IDE-, MFM-, or RLL-
	 Hard Disk, it may conflict with their controllers
       (注意：不要使用 JP11=IRQ6，它可能与你的软盘控制器冲突；只有在你没有
       IDE、MFM 或 RLL 硬盘时才使用 JP3=IRQ14，否则它可能与这些硬盘的控制器
       冲突)


Setting the Timeout Parameters
(设置超时参数)
-------------------------------

The jumpers labeled JP1 and JP2 are used to determine the timeout
parameters. These two jumpers are normally left open.
(标有 JP1 和 JP2 的跳线用于确定超时参数。这两个跳线通常保持断开。)


Lantech
(Lantech 公司)
=======

8-bit card, unknown model
(8 位卡，型号未知)
-------------------------

  - from Vlad Lungu <vlungu@ugal.ro> - his e-mail address seemed broken at
    the time I tried to reach him.  Sorry Vlad, if you didn't get my reply.
    (来自 Vlad Lungu <vlungu@ugal.ro> —— 我尝试联系他时，他的电子邮件地址似乎是
    坏的。Vlad，如果你没收到我的回复，那就抱歉了。)

::

   ________________________________________________________________
   |   1         8                                                 |
   |   ___________                                               __|
   |   |   SW1    |                                         LED |__|
   |   |__________|                                                |
   |                                                            ___|
   |                _____________________                       |S | 8
   |                |                   |                       |W |
   |                |                   |                       |2 |
   |                |                   |                       |__| 1
   |                |      UM9065L      |     |o|  JP4         ____|____
   |                |                   |     |o|              |  CN    |
   |                |                   |                      |________|
   |                |                   |                          |
   |                |___________________|                          |
   |                                                               |
   |                                                               |
   |      _____________                                            |
   |      |            |                                           |
   |      |    PROM    |        |ooooo|  JP6                       |
   |      |____________|        |ooooo|                            |
   |_____________                                             _   _|
		|____________________________________________| |__|


UM9065L : ARCnet Controller
(UM9065L：ARCnet 控制器)

SW 1    : Shared Memory Address and I/O Base
(SW 1：共享内存地址与 I/O 基址)

::

	ON=0

	12345|Memory Address
	-----|--------------
	00001|  D4000
	00010|  CC000
	00110|  D0000
	01110|  D1000
	01101|  D9000
	10010|  CC800
	10011|  DC800
	11110|  D1800

It seems that the bits are considered in reverse order.  Also, you must
observe that some of those addresses are unusual and I didn't probe them; I
used a memory dump in DOS to identify them.  For the 00000 configuration and
some others that I didn't write here the card seems to conflict with the
video card (an S3 GENDAC). I leave the full decoding of those addresses to
you.
(看起来这些位是按相反顺序考虑的。另外，你应当注意其中一些地址并不常见，我没有
探测过它们；我是用 DOS 下的内存转储来识别它们的。对于 00000 配置以及一些我没有
写在这里的其他配置，卡似乎与显卡(一块 S3 GENDAC)冲突。这些地址的完整解码就留给
你自己去弄了。)

::

	678| I/O Address
	---|------------
	000|    260
	001|    failed probe
	010|    2E0
	011|    380
	100|    290
	101|    350
	110|    failed probe
	111|    3E0

  SW 2  : Node ID (binary coded)
  (SW 2：节点 ID(按二进制编码))

  JP 4  : Boot PROM enable   CLOSE - enabled
			     OPEN  - disabled
  (JP 4：Boot PROM 启用。CLOSE(闭合)—— 启用；OPEN(断开)—— 禁用)

  JP 6  : IRQ set (ONLY ONE jumper on 1-5 for IRQ 2-6)
  (JP 6：IRQ 设置(1-5 上只能插一个跳线帽，对应 IRQ 2-6))


Acer
(Acer 宏碁)
====

8-bit card, Model 5210-003
(8 位卡，型号 5210-003)
--------------------------

  - from Vojtech Pavlik <vojtech@suse.cz> using portions of the existing
    arcnet-hardware file.
    (来自 Vojtech Pavlik <vojtech@suse.cz>，使用了现有 arcnet-hardware 文件的
    部分内容。)

This is a 90C26 based card.  Its configuration seems similar to the SMC
PC100, but has some additional jumpers I don't know the meaning of.
(这是一张基于 90C26 的卡。它的配置看起来与 SMC PC100 相似，但有一些我不知道
含义的额外跳线。)

::

	       __
	      |  |
   ___________|__|_________________________
  |         |      |                       |
  |         | BNC  |                       |
  |         |______|                    ___|
  |  _____________________             |___
  | |                     |                |
  | | Hybrid IC           |                |
  | |                     |       o|o J1   |
  | |_____________________|       8|8      |
  |                               8|8 J5   |
  |                               o|o      |
  |                               8|8      |
  |__                             8|8      |
 (|__| LED                        o|o      |
  |                               8|8      |
  |                               8|8 J15  |
  |                                        |
  |                    _____               |
  |                   |     |   _____      |
  |                   |     |  |     |  ___|
  |                   |     |  |     | |
  |  _____            | ROM |  | UFS | |
  | |     |           |     |  |     | |
  | |     |     ___   |     |  |     | |
  | |     |    |   |  |__.__|  |__.__| |
  | | NCR |    |XTL|   _____    _____  |
  | |     |    |___|  |     |  |     | |
  | |90C26|           |     |  |     | |
  | |     |           | RAM |  | UFS | |
  | |     | J17 o|o   |     |  |     | |
  | |     | J16 o|o   |     |  |     | |
  | |__.__|           |__.__|  |__.__| |
  |  ___                               |
  | |   |8                             |
  | |SW2|                              |
  | |   |                              |
  | |___|1                             |
  |  ___                               |
  | |   |10           J18 o|o          |
  | |   |                 o|o          |
  | |SW1|                 o|o          |
  | |   |             J21 o|o          |
  | |___|1                             |
  |                                    |
  |____________________________________|


Legend::
(图例：)

  90C26       ARCNET Chip
  XTL         20 MHz Crystal
  SW1 1-6     Base I/O Address Select
      7-10    Memory Address Select
  SW2 1-8     Node ID Select (ID0-ID7)
  J1-J5       IRQ Select
  J6-J21      Unknown (Probably extra timeouts & ROM enable ...)
  LED1        Activity LED
  BNC         Coax connector (STAR ARCnet)
  RAM         2k of SRAM
  ROM         Boot ROM socket
  UFS         Unidentified Flying Sockets


Setting the Node ID
(设置节点 ID)
^^^^^^^^^^^^^^^^^^^

The eight switches in SW2 are used to set the node ID. Each node attached
to the network must have an unique node ID which must not be 0.
Switch 1 (ID0) serves as the least significant bit (LSB).
(SW2 的八个开关用于设置节点 ID。接入网络的每个节点都必须拥有一个唯一且不得为
0 的节点 ID。开关 1(ID0)作为最低有效位(LSB)。)

Setting one of the switches to OFF means "1", ON means "0".
(把某个开关拨到 OFF 表示 "1"，拨到 ON 表示 "0"。)

The node ID is the sum of the values of all switches set to "1"
These values are::
(节点 ID 是所有拨到 "1" 的开关取值之和。这些取值为：)

   Switch | Value
   -------|-------
     1    |   1
     2    |   2
     3    |   4
     4    |   8
     5    |  16
     6    |  32
     7    |  64
     8    | 128

Don't set this to 0 or 255; these values are reserved.
(不要把它设为 0 或 255；这些值是保留的。)


Setting the I/O Base Address
(设置 I/O 基址)
^^^^^^^^^^^^^^^^^^^^^^^^^^^^

The switches 1 to 6 of switch block SW1 are used to select one
of 32 possible I/O Base addresses using the following tables::
(SW1 开关组的开关 1 到 6 用于按下表从 32 个可能的 I/O 基址中选择一个：)

	  | Hex
   Switch | Value
   -------|-------
     1    | 200
     2    | 100
     3    |  80
     4    |  40
     5    |  20
     6    |  10

The I/O address is sum of all switches set to "1". Remember that
the I/O address space below 0x200 is RESERVED for mainboard, so
switch 1 should be ALWAYS SET TO OFF.
(I/O 地址是所有拨到 "1" 的开关取值之和。记住，低于 0x200 的 I/O 地址空间为主板
保留，因此开关 1 应当始终设为 OFF。)


Setting the Base Memory (RAM) buffer Address
(设置内存(RAM)缓冲区基址)
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

The memory buffer (RAM) requires 2K. The base of this buffer can be
located in any of sixteen positions. However, the addresses below
A0000 are likely to cause system hang because there's main RAM.
(内存缓冲区(RAM)需要 2K。缓冲区基址可位于十六个位置中的任意一个。不过，低于
A0000 的地址很可能导致系统挂起，因为那里是主内存。)

Jumpers 7-10 of switch block SW1 select the Memory Base address::
(SW1 开关组的跳线 7-10 选择内存基址：)

   Switch          | Hex RAM
    7   8   9  10  | Address
   ----------------|---------
   OFF OFF OFF OFF |  F0000 (conflicts with main BIOS)
   OFF OFF OFF ON  |  E0000
   OFF OFF ON  OFF |  D0000
   OFF OFF ON  ON  |  C0000 (conflicts with video BIOS)
   OFF ON  OFF OFF |  B0000 (conflicts with mono video)
   OFF ON  OFF ON  |  A0000 (conflicts with graphics)


Setting the Interrupt Line
(设置中断线)
^^^^^^^^^^^^^^^^^^^^^^^^^^

Jumpers 1-5 of the jumper block J1 control the IRQ level. ON means
shorted, OFF means open::
(跳线块 J1 的跳线 1-5 控制 IRQ 级别。ON 表示短接，OFF 表示断开：)

    Jumper              |  IRQ
    1   2   3   4   5   |
   ----------------------------
    ON  OFF OFF OFF OFF |  7
    OFF ON  OFF OFF OFF |  5
    OFF OFF ON  OFF OFF |  4
    OFF OFF OFF ON  OFF |  3
    OFF OFF OFF OFF ON  |  2


Unknown jumpers & sockets
(未知的跳线与插座)
^^^^^^^^^^^^^^^^^^^^^^^^^

I know nothing about these. I just guess that J16&J17 are timeout
jumpers and maybe one of J18-J21 selects ROM. Also J6-J10 and
J11-J15 are connecting IRQ2-7 to some pins on the UFSs. I can't
guess the purpose.
(我对这些一无所知。我只是猜测 J16 和 J17 是超时跳线，而 J18-J21 之一可能用于
选择 ROM。另外 J6-J10 和 J11-J15 把 IRQ2-7 连接到 UFS 的一些引脚上。我猜不出
它们的用途。)

Datapoint?
(可能是 Datapoint？)
==========

LAN-ARC-8, an 8-bit card
(LAN-ARC-8，8 位卡)
------------------------

  - from Vojtech Pavlik <vojtech@suse.cz>
    (来自 Vojtech Pavlik <vojtech@suse.cz>)

This is another SMC 90C65-based ARCnet card. I couldn't identify the
manufacturer, but it might be DataPoint, because the card has the
original arcNet logo in its upper right corner.
(这是又一张基于 SMC 90C65 的 ARCnet 卡。我无法确认制造商，但可能是 DataPoint，
因为卡的右上角有原始的 arcNet 标志。)

::

	  _______________________________________________________
	 |                         _________                     |
	 |                        |   SW2   | ON      arcNet     |
	 |                        |_________| OFF             ___|
	 |  _____________         1 ______  8                |   | 8
	 | |             | SW1     | XTAL | ____________     | S |
	 | > RAM (2k)    |         |______||            |    | W |
	 | |_____________|                 |      H     |    | 3 |
	 |                        _________|_____ y     |    |___| 1
	 |  _________            |         |     |b     |        |
	 | |_________|           |         |     |r     |        |
	 |                       |     SMC |     |i     |        |
	 |                       |    90C65|     |d     |        |
	 |  _________            |         |     |      |        |
	 | |   SW1   | ON        |         |     |I     |        |
	 | |_________| OFF       |_________|_____/C     |   _____|
	 |  1       8                      |            |  |     |___
	 |  ______________                 |            |  | BNC |___|
	 | |              |                |____________|  |_____|
	 | > EPROM SOCKET |              _____________           |
	 | |______________|             |_____________|          |
	 |                                         ______________|
	 |                                        |
	 |________________________________________|

Legend::
(图例：)

  90C65       ARCNET Chip
  SW1 1-5:    Base Memory Address Select
      6-8:    Base I/O Address Select
  SW2 1-8:    Node ID Select
  SW3 1-5:    IRQ Select
      6-7:    Extra Timeout
      8  :    ROM Enable
  BNC         Coax connector
  XTAL        20 MHz Crystal


Setting the Node ID
(设置节点 ID)
^^^^^^^^^^^^^^^^^^^

The eight switches in SW3 are used to set the node ID. Each node attached
to the network must have an unique node ID which must not be 0.
Switch 1 serves as the least significant bit (LSB).
(SW3 的八个开关用于设置节点 ID。接入网络的每个节点都必须拥有一个唯一且不得为
0 的节点 ID。开关 1 作为最低有效位(LSB)。)

Setting one of the switches to Off means "1", On means "0".
(把某个开关拨到 Off 表示 "1"，拨到 On 表示 "0"。)

The node ID is the sum of the values of all switches set to "1"
These values are::
(节点 ID 是所有拨到 "1" 的开关取值之和。这些取值为：)

   Switch | Value
   -------|-------
     1    |   1
     2    |   2
     3    |   4
     4    |   8
     5    |  16
     6    |  32
     7    |  64
     8    | 128


Setting the I/O Base Address
(设置 I/O 基址)
^^^^^^^^^^^^^^^^^^^^^^^^^^^^

The last three switches in switch block SW1 are used to select one
of eight possible I/O Base addresses using the following table::
(SW1 开关组的最后三个开关用于按下表从八个可能的 I/O 基址中选择一个：)


   Switch      | Hex I/O
    6   7   8  | Address
   ------------|--------
   ON  ON  ON  |  260
   OFF ON  ON  |  290
   ON  OFF ON  |  2E0  (Manufacturer's default)
   OFF OFF ON  |  2F0
   ON  ON  OFF |  300
   OFF ON  OFF |  350
   ON  OFF OFF |  380
   OFF OFF OFF |  3E0


Setting the Base Memory (RAM) buffer Address
(设置内存(RAM)缓冲区基址)
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

The memory buffer (RAM) requires 2K. The base of this buffer can be
located in any of eight positions. The address of the Boot Prom is
memory base + 0x2000.
(内存缓冲区(RAM)需要 2K。缓冲区基址可位于八个位置中的任意一个。Boot Prom 的
地址为内存基址 + 0x2000。)

Jumpers 3-5 of switch block SW1 select the Memory Base address.
(SW1 开关组的跳线 3-5 选择内存基址。)

::

   Switch              | Hex RAM | Hex ROM
    1   2   3   4   5  | Address | Address *)
   --------------------|---------|-----------
   ON  ON  ON  ON  ON  |  C0000  |  C2000
   ON  ON  OFF ON  ON  |  C4000  |  C6000
   ON  ON  ON  OFF ON  |  CC000  |  CE000
   ON  ON  OFF OFF ON  |  D0000  |  D2000  (Manufacturer's default)
   ON  ON  ON  ON  OFF |  D4000  |  D6000
   ON  ON  OFF ON  OFF |  D8000  |  DA000
   ON  ON  ON  OFF OFF |  DC000  |  DE000
   ON  ON  OFF OFF OFF |  E0000  |  E2000

  *) To enable the Boot ROM set the switch 8 of switch block SW3 to position ON.
  (要启用 Boot ROM，请把 SW3 开关组的开关 8 拨到 ON 位置。)

The switches 1 and 2 probably add 0x0800 and 0x1000 to RAM base address.
(开关 1 和 2 可能分别给 RAM 基址加上 0x0800 和 0x1000。)


Setting the Interrupt Line
(设置中断线)
^^^^^^^^^^^^^^^^^^^^^^^^^^

Switches 1-5 of the switch block SW3 control the IRQ level::
(SW3 开关组的开关 1-5 控制 IRQ 级别：)

    Jumper              |  IRQ
    1   2   3   4   5   |
   ----------------------------
    ON  OFF OFF OFF OFF |  3
    OFF ON  OFF OFF OFF |  4
    OFF OFF ON  OFF OFF |  5
    OFF OFF OFF ON  OFF |  7
    OFF OFF OFF OFF ON  |  2


Setting the Timeout Parameters
(设置超时参数)
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

The switches 6-7 of the switch block SW3 are used to determine the timeout
parameters.  These two switches are normally left in the OFF position.
(SW3 开关组的开关 6-7 用于确定超时参数。这两个开关通常保持在 OFF 位置。)


Topware
(Topware 公司)
=======

8-bit card, TA-ARC/10
(8 位卡，TA-ARC/10)
---------------------

  - from Vojtech Pavlik <vojtech@suse.cz>
    (来自 Vojtech Pavlik <vojtech@suse.cz>)

This is another very similar 90C65 card. Most of the switches and jumpers
are the same as on other clones.
(这是又一张非常相似的 90C65 卡。大多数开关和跳线与其他兼容卡相同。)

::

   _____________________________________________________________________
  |  ___________   |                         |            ______        |
  | |SW2 NODE ID|  |                         |           | XTAL |       |
  | |___________|  |  Hybrid IC              |           |______|       |
  |  ___________   |                         |                        __|
  | |SW1 MEM+I/O|  |_________________________|                   LED1|__|)
  | |___________|           1 2                                         |
  |                     J3 |o|o| TIMEOUT                          ______|
  |     ______________     |o|o|                                 |      |
  |    |              |  ___________________                     | RJ   |
  |    > EPROM SOCKET | |                   \                    |------|
  |J2  |______________| |                    |                   |      |
  ||o|                  |                    |                   |______|
  ||o| ROM ENABLE       |        SMC         |    _________             |
  |     _____________   |       90C65        |   |_________|       _____|
  |    |             |  |                    |                    |     |___
  |    > RAM (2k)    |  |                    |                    | BNC |___|
  |    |_____________|  |                    |                    |_____|
  |                     |____________________|                          |
  | ________ IRQ 2 3 4 5 7                  ___________                 |
  ||________|   |o|o|o|o|o|                |___________|                |
  |________   J1|o|o|o|o|o|                               ______________|
	   |                                             |
	   |_____________________________________________|

Legend::
(图例：)

  90C65       ARCNET Chip
  XTAL        20 MHz Crystal
  SW1 1-5     Base Memory Address Select
      6-8     Base I/O Address Select
  SW2 1-8     Node ID Select (ID0-ID7)
  J1          IRQ Select
  J2          ROM Enable
  J3          Extra Timeout
  LED1        Activity LED
  BNC         Coax connector (BUS ARCnet)
  RJ          Twisted Pair Connector (daisy chain)


Setting the Node ID
(设置节点 ID)
^^^^^^^^^^^^^^^^^^^

The eight switches in SW2 are used to set the node ID. Each node attached to
the network must have an unique node ID which must not be 0.  Switch 1 (ID0)
serves as the least significant bit (LSB).
(SW2 的八个开关用于设置节点 ID。接入网络的每个节点都必须拥有一个唯一且不得为
0 的节点 ID。开关 1(ID0)作为最低有效位(LSB)。)

Setting one of the switches to Off means "1", On means "0".
(把某个开关拨到 Off 表示 "1"，拨到 On 表示 "0"。)

The node ID is the sum of the values of all switches set to "1"
These values are::
(节点 ID 是所有拨到 "1" 的开关取值之和。这些取值为：)

   Switch | Label | Value
   -------|-------|-------
     1    | ID0   |   1
     2    | ID1   |   2
     3    | ID2   |   4
     4    | ID3   |   8
     5    | ID4   |  16
     6    | ID5   |  32
     7    | ID6   |  64
     8    | ID7   | 128

Setting the I/O Base Address
(设置 I/O 基址)
^^^^^^^^^^^^^^^^^^^^^^^^^^^^

The last three switches in switch block SW1 are used to select one
of eight possible I/O Base addresses using the following table::
(SW1 开关组的最后三个开关用于按下表从八个可能的 I/O 基址中选择一个：)


   Switch      | Hex I/O
    6   7   8  | Address
   ------------|--------
   ON  ON  ON  |  260  (Manufacturer's default)
   OFF ON  ON  |  290
   ON  OFF ON  |  2E0
   OFF OFF ON  |  2F0
   ON  ON  OFF |  300
   OFF ON  OFF |  350
   ON  OFF OFF |  380
   OFF OFF OFF |  3E0


Setting the Base Memory (RAM) buffer Address
(设置内存(RAM)缓冲区基址)
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

The memory buffer (RAM) requires 2K. The base of this buffer can be
located in any of eight positions. The address of the Boot Prom is
memory base + 0x2000.
(内存缓冲区(RAM)需要 2K。缓冲区基址可位于八个位置中的任意一个。Boot Prom 的
地址为内存基址 + 0x2000。)

Jumpers 3-5 of switch block SW1 select the Memory Base address.
(SW1 开关组的跳线 3-5 选择内存基址。)

::

   Switch              | Hex RAM | Hex ROM
    1   2   3   4   5  | Address | Address *)
   --------------------|---------|-----------
   ON  ON  ON  ON  ON  |  C0000  |  C2000
   ON  ON  OFF ON  ON  |  C4000  |  C6000  (Manufacturer's default)
   ON  ON  ON  OFF ON  |  CC000  |  CE000
   ON  ON  OFF OFF ON  |  D0000  |  D2000
   ON  ON  ON  ON  OFF |  D4000  |  D6000
   ON  ON  OFF ON  OFF |  D8000  |  DA000
   ON  ON  ON  OFF OFF |  DC000  |  DE000
   ON  ON  OFF OFF OFF |  E0000  |  E2000

   *) To enable the Boot ROM short the jumper J2.

The jumpers 1 and 2 probably add 0x0800 and 0x1000 to RAM address.
(跳线 1 和 2 可能分别给 RAM 地址加上 0x0800 和 0x1000。)


Setting the Interrupt Line
(设置中断线)
^^^^^^^^^^^^^^^^^^^^^^^^^^

Jumpers 1-5 of the jumper block J1 control the IRQ level.  ON means
shorted, OFF means open::
(跳线块 J1 的跳线 1-5 控制 IRQ 级别。ON 表示短接，OFF 表示断开：)

    Jumper              |  IRQ
    1   2   3   4   5   |
   ----------------------------
    ON  OFF OFF OFF OFF |  2
    OFF ON  OFF OFF OFF |  3
    OFF OFF ON  OFF OFF |  4
    OFF OFF OFF ON  OFF |  5
    OFF OFF OFF OFF ON  |  7


Setting the Timeout Parameters
(设置超时参数)
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

The jumpers J3 are used to set the timeout parameters. These two
jumpers are normally left open.
(跳线 J3 用于设置超时参数。这两个跳线通常保持断开。)


Thomas-Conrad
(Thomas-Conrad 公司)
=============

Model #500-6242-0097 REV A (8-bit card)
(型号 #500-6242-0097 REV A(8 位卡))
----------------------------------------

  - from Lars Karlsson <100617.3473@compuserve.com>
    (来自 Lars Karlsson <100617.3473@compuserve.com>)

::

     ________________________________________________________
   |          ________   ________                           |_____
   |         |........| |........|                            |
   |         |________| |________|                         ___|
   |            SW 3       SW 1                           |   |
   |         Base I/O   Base Addr.                Station |   |
   |                                              address |   |
   |    ______                                    switch  |   |
   |   |      |                                           |   |
   |   |      |                                           |___|
   |   |      |                                 ______        |___._
   |   |______|                                |______|         ____| BNC
   |                                            Jumper-        _____| Connector
   |   Main chip                                block  _    __|   '
   |                                                  | |  |    RJ Connector
   |                                                  |_|  |    with 110 Ohm
   |                                                       |__  Terminator
   |    ___________                                         __|
   |   |...........|                                       |    RJ-jack
   |   |...........|    _____                              |    (unused)
   |   |___________|   |_____|                             |__
   |  Boot PROM socket IRQ-jumpers                            |_  Diagnostic
   |________                                       __          _| LED (red)
	    | | | | | | | | | | | | | | | | | | | |  |        |
	    | | | | | | | | | | | | | | | | | | | |  |________|
							      |
							      |

And here are the settings for some of the switches and jumpers on the cards.
(下面是卡上部分开关和跳线的设置。)

::

	    I/O

	   1 2 3 4 5 6 7 8

  2E0----- 0 0 0 1 0 0 0 1
  2F0----- 0 0 0 1 0 0 0 0
  300----- 0 0 0 0 1 1 1 1
  350----- 0 0 0 0 1 1 1 0

"0" in the above example means switch is off "1" means that it is on.
(上例中的 "0" 表示开关断开，"1" 表示开关接通。)

::

      ShMem address.

	1 2 3 4 5 6 7 8

  CX00--0 0 1 1 | |   |
  DX00--0 0 1 0       |
  X000--------- 1 1   |
  X400--------- 1 0   |
  X800--------- 0 1   |
  XC00--------- 0 0
  ENHANCED----------- 1
  COMPATIBLE--------- 0

::

	 IRQ


     3 4 5 7 2
     . . . . .
     . . . . .


There is a DIP-switch with 8 switches, used to set the shared memory address
to be used. The first 6 switches set the address, the 7th doesn't have any
function, and the 8th switch is used to select "compatible" or "enhanced".
When I got my two cards, one of them had this switch set to "enhanced". That
card didn't work at all, it wasn't even recognized by the driver. The other
card had this switch set to "compatible" and it behaved absolutely normally. I
guess that the switch on one of the cards, must have been changed accidentally
when the card was taken out of its former host. The question remains
unanswered, what is the purpose of the "enhanced" position?
(有一个带 8 个开关的 DIP 拨码开关，用于设置要使用的共享内存地址。前 6 个开关
设置地址，第 7 个没有任何功能，第 8 个开关用于选择 "compatible"(兼容)还是
"enhanced"(增强)。我拿到两张卡时，其中一张的这个开关设成了 "enhanced"，那张卡
完全不能工作，驱动甚至无法识别它。另一张卡的开关设为 "compatible"，工作完全
正常。我猜其中一张卡上的开关，一定是在它被从原先主机上拔出时被意外拨动了。
"enhanced" 位置究竟有什么用途？这个问题仍无答案。)

[Avery's note: "enhanced" probably either disables shared memory (use IO
ports instead) or disables IO ports (use memory addresses instead).  This
varies by the type of card involved.  I fail to see how either of these
enhance anything.  Send me more detailed information about this mode, or
just use "compatible" mode instead.]
(Avery 的注记："enhanced" 可能有两种含义：要么禁用共享内存(改用 IO 端口)，要么
禁用 IO 端口(改用内存地址)。具体取决于卡的类型。我看不出这两者究竟"增强"了
什么。请给我发来关于这种模式的更详细信息，或者干脆改用 "compatible" 模式。)

Waterloo Microsystems Inc. ??
(Waterloo 微系统公司？？)
=============================

8-bit card (C) 1985
(8 位卡 (C) 1985)
-------------------

  - from Robert Michael Best <rmb117@cs.usask.ca>
    (来自 Robert Michael Best <rmb117@cs.usask.ca>)

[Avery's note: these don't work with my driver for some reason.  These cards
SEEM to have settings similar to the PDI508Plus, which is
software-configured and doesn't work with my driver either.  The "Waterloo
chip" is a boot PROM, probably designed specifically for the University of
Waterloo.  If you have any further information about this card, please
e-mail me.]
(Avery 的注记：由于某种原因，这些卡在我的驱动下无法工作。这些卡的设置看起来与
PDI508Plus 相似，后者通过软件配置，同样不能在我的驱动下工作。"Waterloo 芯片"
是一个引导 PROM，可能是专为滑铁卢大学(University of Waterloo)设计的。如果你有
关于这张卡的更多信息，请给我发邮件。)

The probe has not been able to detect the card on any of the J2 settings,
and I tried them again with the "Waterloo" chip removed.
(无论 J2 处于哪种设置，探测都无法发现这张卡；我取下 "Waterloo" 芯片后又把各种
设置试了一遍，仍然不行。)

::

   _____________________________________________________________________
  | \/  \/              ___  __ __                                      |
  | C4  C4     |^|     | M ||  ^  ||^|                                  |
  | --  --     |_|     | 5 ||     || | C3                               |
  | \/  \/      C10    |___||     ||_|                                  |
  | C4  C4             _  _ |     |                 ??                  |
  | --  --            | \/ ||     |                                     |
  |                   |    ||     |                                     |
  |                   |    ||  C1 |                                     |
  |                   |    ||     |  \/                            _____|
  |                   | C6 ||     |  C9                           |     |___
  |                   |    ||     |  --                           | BNC |___|
  |                   |    ||     |          >C7|                 |_____|
  |                   |    ||     |                                     |
  | __ __             |____||_____|       1 2 3     6                   |
  ||  ^  |     >C4|                      |o|o|o|o|o|o| J2    >C4|       |
  ||     |                               |o|o|o|o|o|o|                  |
  || C2  |     >C4|                                          >C4|       |
  ||     |                                   >C8|                       |
  ||     |       2 3 4 5 6 7  IRQ                            >C4|       |
  ||_____|      |o|o|o|o|o|o| J3                                        |
  |_______      |o|o|o|o|o|o|                            _______________|
	  |                                             |
	  |_____________________________________________|

  C1 -- "COM9026
	 SMC 8638"
	In a chip socket.

  C2 -- "@Copyright
	 Waterloo Microsystems Inc.
	 1985"
	In a chip Socket with info printed on a label covering a round window
	showing the circuit inside. (The window indicates it is an EPROM chip.)

  C3 -- "COM9032
	 SMC 8643"
	In a chip socket.

  C4 -- "74LS"
	9 total no sockets.

  M5 -- "50006-136
	 20.000000 MHZ
	 MTQ-T1-S3
	 0 M-TRON 86-40"
	Metallic case with 4 pins, no socket.

  C6 -- "MOSTEK@TC8643
	 MK6116N-20
	 MALAYSIA"
	No socket.

  C7 -- No stamp or label but in a 20 pin chip socket.

  C8 -- "PAL10L8CN
	 8623"
	In a 20 pin socket.

  C9 -- "PAl16R4A-2CN
	 8641"
	In a 20 pin socket.

  C10 -- "M8640
	    NMC
	  9306N"
	 In an 8 pin socket.

  ?? -- Some components on a smaller board and attached with 20 pins all
	along the side closest to the BNC connector.  The are coated in a dark
	resin.

On the board there are two jumper banks labeled J2 and J3. The
manufacturer didn't put a J1 on the board. The two boards I have both
came with a jumper box for each bank.
(板上有两个分别标为 J2 和 J3 的跳线组。厂商没有在板上设置 J1。我手里的两块板
都随附了每个跳线组一个的跳线盒。)

::

  J2 -- Numbered 1 2 3 4 5 6.
	4 and 5 are not stamped due to solder points.

  J3 -- IRQ 2 3 4 5 6 7

The board itself has a maple leaf stamped just above the irq jumpers
and "-2 46-86" beside C2. Between C1 and C6 "ASS 'Y 300163" and "@1986
CORMAN CUSTOM ELECTRONICS CORP." stamped just below the BNC connector.
Below that "MADE IN CANADA"
(板子本身在 irq 跳线正上方印有一片枫叶，C2 旁边印有 "-2 46-86"。BNC 连接器正
下方印有 "ASS 'Y 300163" 和 "@1986 CORMAN CUSTOM ELECTRONICS CORP."。再往下
是 "MADE IN CANADA"。)

No Name
(无名牌)
=======

8-bit cards, 16-bit cards
(8 位卡，16 位卡)
-------------------------

  - from Juergen Seifert <seifert@htwm.de>
    (来自 Juergen Seifert <seifert@htwm.de>)

I have named this ARCnet card "NONAME", since there is no name of any
manufacturer on the Installation manual nor on the shipping box. The only
hint to the existence of a manufacturer at all is written in copper,
it is "Made in Taiwan"
(我把这张 ARCnet 卡命名为 "NONAME"，因为安装手册和包装盒上都没有任何制造商的
名字。唯一能表明制造商存在的线索是用铜箔写成的："Made in Taiwan"(台湾制造))

This description has been written by Juergen Seifert <seifert@htwm.de>
using information from the Original
(以下说明由 Juergen Seifert <seifert@htwm.de> 依据下列原版手册编写)

		    "ARCnet Installation Manual"
		    (《ARCnet 安装手册》)

::

    ________________________________________________________________
   | |STAR| BUS| T/P|                                               |
   | |____|____|____|                                               |
   |                            _____________________               |
   |                           |                     |              |
   |                           |                     |              |
   |                           |                     |              |
   |                           |        SMC          |              |
   |                           |                     |              |
   |                           |       COM90C65      |              |
   |                           |                     |              |
   |                           |                     |              |
   |                           |__________-__________|              |
   |                                                           _____|
   |      _______________                                     |  CN |
   |     | PROM          |                                    |_____|
   |     > SOCKET        |                                          |
   |     |_______________|         1 2 3 4 5 6 7 8  1 2 3 4 5 6 7 8 |
   |                               _______________  _______________ |
   |           |o|o|o|o|o|o|o|o|  |      SW1      ||      SW2      ||
   |           |o|o|o|o|o|o|o|o|  |_______________||_______________||
   |___         2 3 4 5 7 E E R        Node ID       IOB__|__MEM____|
       |        \ IRQ   / T T O                      |
       |__________________1_2_M______________________|

Legend::
(图例：)

  COM90C65:       ARCnet Probe
  S1  1-8:    Node ID Select
  S2  1-3:    I/O Base Address Select
      4-6:    Memory Base Address Select
      7-8:    RAM Offset Select
  ET1, ET2    Extended Timeout Select
  ROM     ROM Enable Select
  CN              RG62 Coax Connector
  STAR| BUS | T/P Three fields for placing a sign (colored circle)
		  indicating the topology of the card

Setting one of the switches to Off means "1", On means "0".
(把某个开关拨到 Off 表示 "1"，拨到 On 表示 "0"。)


Setting the Node ID
(设置节点 ID)
^^^^^^^^^^^^^^^^^^^

The eight switches in group SW1 are used to set the node ID.
Each node attached to the network must have an unique node ID which
must be different from 0.
Switch 8 serves as the least significant bit (LSB).
(SW1 组的八个开关用于设置节点 ID。接入网络的每个节点都必须拥有一个唯一且不为
0 的节点 ID。开关 8 作为最低有效位(LSB)。)

The node ID is the sum of the values of all switches set to "1"
These values are::
(节点 ID 是所有拨到 "1" 的开关取值之和。这些取值为：)

    Switch | Value
    -------|-------
      8    |   1
      7    |   2
      6    |   4
      5    |   8
      4    |  16
      3    |  32
      2    |  64
      1    | 128

Some Examples::
(一些例子：)

    Switch         | Hex     | Decimal
   1 2 3 4 5 6 7 8 | Node ID | Node ID
   ----------------|---------|---------
   0 0 0 0 0 0 0 0 |    not allowed
   0 0 0 0 0 0 0 1 |    1    |    1
   0 0 0 0 0 0 1 0 |    2    |    2
   0 0 0 0 0 0 1 1 |    3    |    3
       . . .       |         |
   0 1 0 1 0 1 0 1 |   55    |   85
       . . .       |         |
   1 0 1 0 1 0 1 0 |   AA    |  170
       . . .       |         |
   1 1 1 1 1 1 0 1 |   FD    |  253
   1 1 1 1 1 1 1 0 |   FE    |  254
   1 1 1 1 1 1 1 1 |   FF    |  255


Setting the I/O Base Address
(设置 I/O 基址)
^^^^^^^^^^^^^^^^^^^^^^^^^^^^

The first three switches in switch group SW2 are used to select one
of eight possible I/O Base addresses using the following table::
(SW2 开关组的前三个开关用于按下表从八个可能的 I/O 基址中选择一个：)

   Switch      | Hex I/O
    1   2   3  | Address
   ------------|--------
   ON  ON  ON  |  260
   ON  ON  OFF |  290
   ON  OFF ON  |  2E0  (Manufacturer's default)
   ON  OFF OFF |  2F0
   OFF ON  ON  |  300
   OFF ON  OFF |  350
   OFF OFF ON  |  380
   OFF OFF OFF |  3E0


Setting the Base Memory (RAM) buffer Address
(设置内存(RAM)缓冲区基址)
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

The memory buffer requires 2K of a 16K block of RAM. The base of this
16K block can be located in any of eight positions.
Switches 4-6 of switch group SW2 select the Base of the 16K block.
Within that 16K address space, the buffer may be assigned any one of four
positions, determined by the offset, switches 7 and 8 of group SW2.
(内存缓冲区需要 16K RAM 块中的 2K。这个 16K 块的基址可以位于八个位置中的任意
一个。SW2 开关组的开关 4-6 选择该 16K 块的基址。在这 16K 地址空间内，缓冲区由
SW2 组的开关 7 和 8 决定的偏移量，可指派到四个位置中的任意一个。)

::

   Switch     | Hex RAM | Hex ROM
   4 5 6  7 8 | Address | Address *)
   -----------|---------|-----------
   0 0 0  0 0 |  C0000  |  C2000
   0 0 0  0 1 |  C0800  |  C2000
   0 0 0  1 0 |  C1000  |  C2000
   0 0 0  1 1 |  C1800  |  C2000
	      |         |
   0 0 1  0 0 |  C4000  |  C6000
   0 0 1  0 1 |  C4800  |  C6000
   0 0 1  1 0 |  C5000  |  C6000
   0 0 1  1 1 |  C5800  |  C6000
	      |         |
   0 1 0  0 0 |  CC000  |  CE000
   0 1 0  0 1 |  CC800  |  CE000
   0 1 0  1 0 |  CD000  |  CE000
   0 1 0  1 1 |  CD800  |  CE000
	      |         |
   0 1 1  0 0 |  D0000  |  D2000  (Manufacturer's default)
   0 1 1  0 1 |  D0800  |  D2000
   0 1 1  1 0 |  D1000  |  D2000
   0 1 1  1 1 |  D1800  |  D2000
	      |         |
   1 0 0  0 0 |  D4000  |  D6000
   1 0 0  0 1 |  D4800  |  D6000
   1 0 0  1 0 |  D5000  |  D6000
   1 0 0  1 1 |  D5800  |  D6000
	      |         |
   1 0 1  0 0 |  D8000  |  DA000
   1 0 1  0 1 |  D8800  |  DA000
   1 0 1  1 0 |  D9000  |  DA000
   1 0 1  1 1 |  D9800  |  DA000
	      |         |
   1 1 0  0 0 |  DC000  |  DE000
   1 1 0  0 1 |  DC800  |  DE000
   1 1 0  1 0 |  DD000  |  DE000
   1 1 0  1 1 |  DD800  |  DE000
	      |         |
   1 1 1  0 0 |  E0000  |  E2000
   1 1 1  0 1 |  E0800  |  E2000
   1 1 1  1 0 |  E1000  |  E2000
   1 1 1  1 1 |  E1800  |  E2000

   *) To enable the 8K Boot PROM install the jumper ROM.
      The default is jumper ROM not installed.


Setting Interrupt Request Lines (IRQ)
(设置中断请求线(IRQ))
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

To select a hardware interrupt level set one (only one!) of the jumpers
IRQ2, IRQ3, IRQ4, IRQ5 or IRQ7. The manufacturer's default is IRQ2.
(要选择硬件中断级别，请设置 IRQ2、IRQ3、IRQ4、IRQ5 或 IRQ7 跳线中的一个
(仅限一个！)。厂商默认值为 IRQ2。)


Setting the Timeouts
(设置超时)
^^^^^^^^^^^^^^^^^^^^

The two jumpers labeled ET1 and ET2 are used to determine the timeout
parameters (response and reconfiguration time). Every node in a network
must be set to the same timeout values.
(标有 ET1 和 ET2 的两个跳线用于确定超时参数(响应时间和重配置时间)。网络中的
每个节点都必须设置为相同的超时值。)

::

   ET1 ET2 | Response Time (us) | Reconfiguration Time (ms)
   --------|--------------------|--------------------------
   Off Off |        78          |          840   (Default)
   Off On  |       285          |         1680
   On  Off |       563          |         1680
   On  On  |      1130          |         1680

On means jumper installed, Off means jumper not installed
(On 表示已安装跳线，Off 表示未安装跳线)


16-BIT ARCNET
(16 位 ARCNET)
-------------

The manual of my 8-Bit NONAME ARCnet Card contains another description
of a 16-Bit Coax / Twisted Pair Card. This description is incomplete,
because there are missing two pages in the manual booklet. (The table
of contents reports pages ... 2-9, 2-11, 2-12, 3-1, ... but inside
the booklet there is a different way of counting ... 2-9, 2-10, A-1,
(empty page), 3-1, ..., 3-18, A-1 (again), A-2)
Also the picture of the board layout is not as good as the picture of
8-Bit card, because there isn't any letter like "SW1" written to the
picture.
(我的 8 位 NONAME ARCnet 卡的手册里还包含对一种 16 位同轴/双绞线卡的另一段
说明。这段说明不完整，因为手册小册子里缺了两页。(目录列出的页码是 ... 2-9、
2-11、2-12、3-1、...，但册子里面实际的编号方式不同：... 2-9、2-10、A-1、
(空白页)、3-1、...、3-18、A-1(又一次)、A-2。)此外，板卡布局图也不如 8 位卡的
图清晰，因为图上没有标注任何 "SW1" 之类的字样。)

Should somebody have such a board, please feel free to complete this
description or to send a mail to me!
(如果有人有这样一块板，请尽管补全这段说明，或给我发邮件！)

This description has been written by Juergen Seifert <seifert@htwm.de>
using information from the Original
(以下说明由 Juergen Seifert <seifert@htwm.de> 依据下列原版手册编写)

		    "ARCnet Installation Manual"
		    (《ARCnet 安装手册》)

::

   ___________________________________________________________________
  <                    _________________  _________________           |
  >                   |       SW?       ||      SW?        |          |
  <                   |_________________||_________________|          |
  >                       ____________________                        |
  <                      |                    |                       |
  >                      |                    |                       |
  <                      |                    |                       |
  >                      |                    |                       |
  <                      |                    |                       |
  >                      |                    |                       |
  <                      |                    |                       |
  >                      |____________________|                       |
  <                                                               ____|
  >                       ____________________                   |    |
  <                      |                    |                  | J1 |
  >                      |                    <                  |    |
  <                      |____________________|  ? ? ? ? ? ?     |____|
  >                                             |o|o|o|o|o|o|         |
  <                                             |o|o|o|o|o|o|         |
  >                                                                   |
  <             __                                         ___________|
  >            |  |                                       |
  <____________|  |_______________________________________|


Setting one of the switches to Off means "1", On means "0".
(把某个开关拨到 Off 表示 "1"，拨到 On 表示 "0"。)


Setting the Node ID
(设置节点 ID)
^^^^^^^^^^^^^^^^^^^

The eight switches in group SW2 are used to set the node ID.
Each node attached to the network must have an unique node ID which
must be different from 0.
Switch 8 serves as the least significant bit (LSB).
(SW2 组的八个开关用于设置节点 ID。接入网络的每个节点都必须拥有一个唯一且不为
0 的节点 ID。开关 8 作为最低有效位(LSB)。)

The node ID is the sum of the values of all switches set to "1"
These values are::
(节点 ID 是所有拨到 "1" 的开关取值之和。这些取值为：)

    Switch | Value
    -------|-------
      8    |   1
      7    |   2
      6    |   4
      5    |   8
      4    |  16
      3    |  32
      2    |  64
      1    | 128

Some Examples::
(一些例子：)

    Switch         | Hex     | Decimal
   1 2 3 4 5 6 7 8 | Node ID | Node ID
   ----------------|---------|---------
   0 0 0 0 0 0 0 0 |    not allowed
   0 0 0 0 0 0 0 1 |    1    |    1
   0 0 0 0 0 0 1 0 |    2    |    2
   0 0 0 0 0 0 1 1 |    3    |    3
       . . .       |         |
   0 1 0 1 0 1 0 1 |   55    |   85
       . . .       |         |
   1 0 1 0 1 0 1 0 |   AA    |  170
       . . .       |         |
   1 1 1 1 1 1 0 1 |   FD    |  253
   1 1 1 1 1 1 1 0 |   FE    |  254
   1 1 1 1 1 1 1 1 |   FF    |  255


Setting the I/O Base Address
(设置 I/O 基址)
^^^^^^^^^^^^^^^^^^^^^^^^^^^^

The first three switches in switch group SW1 are used to select one
of eight possible I/O Base addresses using the following table::
(SW1 开关组的前三个开关用于按下表从八个可能的 I/O 基址中选择一个：)

   Switch      | Hex I/O
    3   2   1  | Address
   ------------|--------
   ON  ON  ON  |  260
   ON  ON  OFF |  290
   ON  OFF ON  |  2E0  (Manufacturer's default)
   ON  OFF OFF |  2F0
   OFF ON  ON  |  300
   OFF ON  OFF |  350
   OFF OFF ON  |  380
   OFF OFF OFF |  3E0


Setting the Base Memory (RAM) buffer Address
(设置内存(RAM)缓冲区基址)
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

The memory buffer requires 2K of a 16K block of RAM. The base of this
16K block can be located in any of eight positions.
Switches 6-8 of switch group SW1 select the Base of the 16K block.
Within that 16K address space, the buffer may be assigned any one of four
positions, determined by the offset, switches 4 and 5 of group SW1::
(内存缓冲区需要 16K RAM 块中的 2K。这个 16K 块的基址可以位于八个位置中的任意
一个。SW1 开关组的开关 6-8 选择该 16K 块的基址。在这 16K 地址空间内，缓冲区由
SW1 组的开关 4 和 5 决定的偏移量，可指派到四个位置中的任意一个。)

   Switch     | Hex RAM | Hex ROM
   8 7 6  5 4 | Address | Address
   -----------|---------|-----------
   0 0 0  0 0 |  C0000  |  C2000
   0 0 0  0 1 |  C0800  |  C2000
   0 0 0  1 0 |  C1000  |  C2000
   0 0 0  1 1 |  C1800  |  C2000
	      |         |
   0 0 1  0 0 |  C4000  |  C6000
   0 0 1  0 1 |  C4800  |  C6000
   0 0 1  1 0 |  C5000  |  C6000
   0 0 1  1 1 |  C5800  |  C6000
	      |         |
   0 1 0  0 0 |  CC000  |  CE000
   0 1 0  0 1 |  CC800  |  CE000
   0 1 0  1 0 |  CD000  |  CE000
   0 1 0  1 1 |  CD800  |  CE000
	      |         |
   0 1 1  0 0 |  D0000  |  D2000  (Manufacturer's default)
   0 1 1  0 1 |  D0800  |  D2000
   0 1 1  1 0 |  D1000  |  D2000
   0 1 1  1 1 |  D1800  |  D2000
	      |         |
   1 0 0  0 0 |  D4000  |  D6000
   1 0 0  0 1 |  D4800  |  D6000
   1 0 0  1 0 |  D5000  |  D6000
   1 0 0  1 1 |  D5800  |  D6000
	      |         |
   1 0 1  0 0 |  D8000  |  DA000
   1 0 1  0 1 |  D8800  |  DA000
   1 0 1  1 0 |  D9000  |  DA000
   1 0 1  1 1 |  D9800  |  DA000
	      |         |
   1 1 0  0 0 |  DC000  |  DE000
   1 1 0  0 1 |  DC800  |  DE000
   1 1 0  1 0 |  DD000  |  DE000
   1 1 0  1 1 |  DD800  |  DE000
	      |         |
   1 1 1  0 0 |  E0000  |  E2000
   1 1 1  0 1 |  E0800  |  E2000
   1 1 1  1 0 |  E1000  |  E2000
   1 1 1  1 1 |  E1800  |  E2000


Setting Interrupt Request Lines (IRQ)
(设置中断请求线(IRQ))
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

??????????????????????????????????????


Setting the Timeouts
(设置超时)
^^^^^^^^^^^^^^^^^^^^

??????????????????????????????????????


8-bit cards ("Made in Taiwan R.O.C.")
(8 位卡("Made in Taiwan R.O.C."))
-------------------------------------

  - from Vojtech Pavlik <vojtech@suse.cz>
    (来自 Vojtech Pavlik <vojtech@suse.cz>)

I have named this ARCnet card "NONAME", since I got only the card with
no manual at all and the only text identifying the manufacturer is
"MADE IN TAIWAN R.O.C" printed on the card.
(我把这张 ARCnet 卡命名为 "NONAME"，因为我只拿到了卡本身，完全没有手册，卡上
唯一能表明制造商的文字就是印刷的 "MADE IN TAIWAN R.O.C"。)

::

	  ____________________________________________________________
	 |                 1 2 3 4 5 6 7 8                            |
	 | |o|o| JP1       o|o|o|o|o|o|o|o| ON                        |
	 |  +              o|o|o|o|o|o|o|o|                        ___|
	 |  _____________  o|o|o|o|o|o|o|o| OFF         _____     |   | ID7
	 | |             | SW1                         |     |    |   | ID6
	 | > RAM (2k)    |        ____________________ |  H  |    | S | ID5
	 | |_____________|       |                    ||  y  |    | W | ID4
	 |                       |                    ||  b  |    | 2 | ID3
	 |                       |                    ||  r  |    |   | ID2
	 |                       |                    ||  i  |    |   | ID1
	 |                       |       90C65        ||  d  |    |___| ID0
	 |      SW3              |                    ||     |        |
	 | |o|o|o|o|o|o|o|o| ON  |                    ||  I  |        |
	 | |o|o|o|o|o|o|o|o|     |                    ||  C  |        |
	 | |o|o|o|o|o|o|o|o| OFF |____________________||     |   _____|
	 |  1 2 3 4 5 6 7 8                            |     |  |     |___
	 |  ______________                             |     |  | BNC |___|
	 | |              |                            |_____|  |_____|
	 | > EPROM SOCKET |                                           |
	 | |______________|                                           |
	 |                                              ______________|
	 |                                             |
	 |_____________________________________________|

Legend::
(图例：)

  90C65       ARCNET Chip
  SW1 1-5:    Base Memory Address Select
      6-8:    Base I/O Address Select
  SW2 1-8:    Node ID Select (ID0-ID7)
  SW3 1-5:    IRQ Select
      6-7:    Extra Timeout
      8  :    ROM Enable
  JP1         Led connector
  BNC         Coax connector

Although the jumpers SW1 and SW3 are marked SW, not JP, they are jumpers, not
switches.
(尽管 SW1 和 SW3 标记为 SW 而非 JP，它们是跳线而不是拨码开关。)

Setting the jumpers to ON means connecting the upper two pins, off the bottom
two - or - in case of IRQ setting, connecting none of them at all.
(把跳线设为 ON 表示连接上面两个引脚，OFF 表示连接下面两个引脚——或者——在设置
IRQ 的情况下，一个都不连。)

Setting the Node ID
(设置节点 ID)
^^^^^^^^^^^^^^^^^^^

The eight switches in SW2 are used to set the node ID. Each node attached
to the network must have an unique node ID which must not be 0.
Switch 1 (ID0) serves as the least significant bit (LSB).
(SW2 的八个开关用于设置节点 ID。接入网络的每个节点都必须拥有一个唯一且不得为
0 的节点 ID。开关 1(ID0)作为最低有效位(LSB)。)

Setting one of the switches to Off means "1", On means "0".
(把某个开关拨到 Off 表示 "1"，拨到 On 表示 "0"。)

The node ID is the sum of the values of all switches set to "1"
These values are::
(节点 ID 是所有拨到 "1" 的开关取值之和。这些取值为：)

   Switch | Label | Value
   -------|-------|-------
     1    | ID0   |   1
     2    | ID1   |   2
     3    | ID2   |   4
     4    | ID3   |   8
     5    | ID4   |  16
     6    | ID5   |  32
     7    | ID6   |  64
     8    | ID7   | 128

Some Examples::
(一些例子：)

    Switch         | Hex     | Decimal
   8 7 6 5 4 3 2 1 | Node ID | Node ID
   ----------------|---------|---------
   0 0 0 0 0 0 0 0 |    not allowed
   0 0 0 0 0 0 0 1 |    1    |    1
   0 0 0 0 0 0 1 0 |    2    |    2
   0 0 0 0 0 0 1 1 |    3    |    3
       . . .       |         |
   0 1 0 1 0 1 0 1 |   55    |   85
       . . .       |         |
   1 0 1 0 1 0 1 0 |   AA    |  170
       . . .       |         |
   1 1 1 1 1 1 0 1 |   FD    |  253
   1 1 1 1 1 1 1 0 |   FE    |  254
   1 1 1 1 1 1 1 1 |   FF    |  255


Setting the I/O Base Address
(设置 I/O 基址)
^^^^^^^^^^^^^^^^^^^^^^^^^^^^

The last three switches in switch block SW1 are used to select one
of eight possible I/O Base addresses using the following table::
(SW1 开关组的最后三个开关用于按下表从八个可能的 I/O 基址中选择一个：)


   Switch      | Hex I/O
    6   7   8  | Address
   ------------|--------
   ON  ON  ON  |  260
   OFF ON  ON  |  290
   ON  OFF ON  |  2E0  (Manufacturer's default)
   OFF OFF ON  |  2F0
   ON  ON  OFF |  300
   OFF ON  OFF |  350
   ON  OFF OFF |  380
   OFF OFF OFF |  3E0


Setting the Base Memory (RAM) buffer Address
(设置内存(RAM)缓冲区基址)
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

The memory buffer (RAM) requires 2K. The base of this buffer can be
located in any of eight positions. The address of the Boot Prom is
memory base + 0x2000.
(内存缓冲区(RAM)需要 2K。缓冲区基址可位于八个位置中的任意一个。Boot Prom 的
地址为内存基址 + 0x2000。)

Jumpers 3-5 of jumper block SW1 select the Memory Base address.
(跳线块 SW1 的跳线 3-5 选择内存基址。)

::

   Switch              | Hex RAM | Hex ROM
    1   2   3   4   5  | Address | Address *)
   --------------------|---------|-----------
   ON  ON  ON  ON  ON  |  C0000  |  C2000
   ON  ON  OFF ON  ON  |  C4000  |  C6000
   ON  ON  ON  OFF ON  |  CC000  |  CE000
   ON  ON  OFF OFF ON  |  D0000  |  D2000  (Manufacturer's default)
   ON  ON  ON  ON  OFF |  D4000  |  D6000
   ON  ON  OFF ON  OFF |  D8000  |  DA000
   ON  ON  ON  OFF OFF |  DC000  |  DE000
   ON  ON  OFF OFF OFF |  E0000  |  E2000

  *) To enable the Boot ROM set the jumper 8 of jumper block SW3 to position ON.
  (要启用 Boot ROM，请把跳线块 SW3 的跳线 8 设为 ON 位置。)

The jumpers 1 and 2 probably add 0x0800, 0x1000 and 0x1800 to RAM adders.
(跳线 1 和 2 可能分别给 RAM 地址加上 0x0800、0x1000 和 0x1800。)

Setting the Interrupt Line
(设置中断线)
^^^^^^^^^^^^^^^^^^^^^^^^^^

Jumpers 1-5 of the jumper block SW3 control the IRQ level::
(跳线块 SW3 的跳线 1-5 控制 IRQ 级别：)

    Jumper              |  IRQ
    1   2   3   4   5   |
   ----------------------------
    ON  OFF OFF OFF OFF |  2
    OFF ON  OFF OFF OFF |  3
    OFF OFF ON  OFF OFF |  4
    OFF OFF OFF ON  OFF |  5
    OFF OFF OFF OFF ON  |  7


Setting the Timeout Parameters
(设置超时参数)
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

The jumpers 6-7 of the jumper block SW3 are used to determine the timeout
parameters. These two jumpers are normally left in the OFF position.
(跳线块 SW3 的跳线 6-7 用于确定超时参数。这两个跳线通常保持在 OFF 位置。)



(Generic Model 9058)
(通用型号 9058)
--------------------

  - from Andrew J. Kroll <ag784@freenet.buffalo.edu>
    (来自 Andrew J. Kroll <ag784@freenet.buffalo.edu>)
  - Sorry this sat in my to-do box for so long, Andrew! (yikes - over a
    year!)
    (抱歉，这份材料在我的待办箱里放了这么久，Andrew！(天哪——超过一年！))

::

								      _____
								     |    <
								     | .---'
    ________________________________________________________________ | |
   |                           |     SW2     |                      |  |
   |   ___________             |_____________|                      |  |
   |  |           |              1 2 3 4 5 6                     ___|  |
   |  >  6116 RAM |         _________                         8 |   |  |
   |  |___________|        |20MHzXtal|                        7 |   |  |
   |                       |_________|       __________       6 | S |  |
   |    74LS373                             |          |-     5 | W |  |
   |   _________                            |      E   |-     4 |   |  |
   |   >_______|              ______________|..... P   |-     3 | 3 |  |
   |                         |              |    : O   |-     2 |   |  |
   |                         |              |    : X   |-     1 |___|  |
   |   ________________      |              |    : Y   |-           |  |
   |  |      SW1       |     |      SL90C65 |    :     |-           |  |
   |  |________________|     |              |    : B   |-           |  |
   |    1 2 3 4 5 6 7 8      |              |    : O   |-           |  |
   |                         |_________o____|..../ A   |-    _______|  |
   |    ____________________                |      R   |-   |       |------,
   |   |                    |               |      D   |-   |  BNC  |   #  |
   |   > 2764 PROM SOCKET   |               |__________|-   |_______|------'
   |   |____________________|              _________                |  |
   |                                       >________| <- 74LS245    |  |
   |                                                                |  |
   |___                                               ______________|  |
       |H H H H H H H H H H H H H H H H H H H H H H H|               | |
       |U_U_U_U_U_U_U_U_U_U_U_U_U_U_U_U_U_U_U_U_U_U_U|               | |
								      \|

Legend::
(图例：)

  SL90C65 	ARCNET Controller / Transceiver /Logic
  SW1	1-5:	IRQ Select
	  6:	ET1
	  7:	ET2
	  8:	ROM ENABLE
  SW2	1-3:    Memory Buffer/PROM Address
	3-6:	I/O Address Map
  SW3	1-8:	Node ID Select
  BNC		BNC RG62/U Connection
		*I* have had success using RG59B/U with *NO* terminators!
		What gives?!

SW1: Timeouts, Interrupt and ROM
(SW1：超时、中断与 ROM)
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

To select a hardware interrupt level set one (only one!) of the dip switches
up (on) SW1...(switches 1-5)
IRQ3, IRQ4, IRQ5, IRQ7, IRQ2. The Manufacturer's default is IRQ2.
(要选择硬件中断级别，请把 SW1 上的一个(仅限一个！)拨码开关拨到上方(接通)……
(开关 1-5)：IRQ3、IRQ4、IRQ5、IRQ7、IRQ2。厂商默认值为 IRQ2。)

The switches on SW1 labeled EXT1 (switch 6) and EXT2 (switch 7)
are used to determine the timeout parameters. These two dip switches
are normally left off (down).
(SW1 上标为 EXT1(开关 6)和 EXT2(开关 7)的开关用于确定超时参数。这两个拨码
开关通常保持在 off(下方)位置。)

   To enable the 8K Boot PROM position SW1 switch 8 on (UP) labeled ROM.
   The default is jumper ROM not installed.
   (要启用 8K Boot PROM，请把标为 ROM 的 SW1 开关 8 拨到接通(上方)位置。默认
   情况下未安装 ROM。)


Setting the I/O Base Address
(设置 I/O 基址)
^^^^^^^^^^^^^^^^^^^^^^^^^^^^

The last three switches in switch group SW2 are used to select one
of eight possible I/O Base addresses using the following table::
(SW2 开关组的最后三个开关用于按下表从八个可能的 I/O 基址中选择一个：)


   Switch | Hex I/O
   4 5 6  | Address
   -------|--------
   0 0 0  |  260
   0 0 1  |  290
   0 1 0  |  2E0  (Manufacturer's default)
   0 1 1  |  2F0
   1 0 0  |  300
   1 0 1  |  350
   1 1 0  |  380
   1 1 1  |  3E0


Setting the Base Memory Address (RAM & ROM)
(设置内存基址(RAM 与 ROM))
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

The memory buffer requires 2K of a 16K block of RAM. The base of this
16K block can be located in any of eight positions.
Switches 1-3 of switch group SW2 select the Base of the 16K block.
(0 = DOWN, 1 = UP)
I could, however, only verify two settings...
(内存缓冲区需要 16K RAM 块中的 2K。这个 16K 块的基址可以位于八个位置中的任意
一个。SW2 开关组的开关 1-3 选择该 16K 块的基址。(0 = 下方(DOWN)，1 = 上方
(UP)) 不过，我只能验证其中两种设置……)


::

   Switch| Hex RAM | Hex ROM
   1 2 3 | Address | Address
   ------|---------|-----------
   0 0 0 |  E0000  |  E2000
   0 0 1 |  D0000  |  D2000  (Manufacturer's default)
   0 1 0 |  ?????  |  ?????
   0 1 1 |  ?????  |  ?????
   1 0 0 |  ?????  |  ?????
   1 0 1 |  ?????  |  ?????
   1 1 0 |  ?????  |  ?????
   1 1 1 |  ?????  |  ?????


Setting the Node ID
(设置节点 ID)
^^^^^^^^^^^^^^^^^^^

The eight switches in group SW3 are used to set the node ID.
Each node attached to the network must have an unique node ID which
must be different from 0.
Switch 1 serves as the least significant bit (LSB).
switches in the DOWN position are OFF (0) and in the UP position are ON (1)
(SW3 组的八个开关用于设置节点 ID。接入网络的每个节点都必须拥有一个唯一且不为
0 的节点 ID。开关 1 作为最低有效位(LSB)。开关处于下方(DOWN)位置为 OFF(0)，
处于上方(UP)位置为 ON(1)。)

The node ID is the sum of the values of all switches set to "1"
These values are::
(节点 ID 是所有拨到 "1" 的开关取值之和。这些取值为：)

    Switch | Value
    -------|-------
      1    |   1
      2    |   2
      3    |   4
      4    |   8
      5    |  16
      6    |  32
      7    |  64
      8    | 128

Some Examples::
(一些例子：)

      Switch#     |   Hex   | Decimal
  8 7 6 5 4 3 2 1 | Node ID | Node ID
  ----------------|---------|---------
  0 0 0 0 0 0 0 0 |    not allowed  <-.
  0 0 0 0 0 0 0 1 |    1    |    1    |
  0 0 0 0 0 0 1 0 |    2    |    2    |
  0 0 0 0 0 0 1 1 |    3    |    3    |
      . . .       |         |         |
  0 1 0 1 0 1 0 1 |   55    |   85    |
      . . .       |         |         + Don't use 0 or 255!
  1 0 1 0 1 0 1 0 |   AA    |  170    |
      . . .       |         |         |
  1 1 1 1 1 1 0 1 |   FD    |  253    |
  1 1 1 1 1 1 1 0 |   FE    |  254    |
  1 1 1 1 1 1 1 1 |   FF    |  255  <-'


Tiara
(Tiara 公司)
=====

(model unknown)
(型号未知)
---------------

  - from Christoph Lameter <cl@gentwo.org>
    (来自 Christoph Lameter <cl@gentwo.org>)


Here is information about my card as far as I could figure it out::
(以下是我尽力弄清的关于我这张卡的信息：)


  ----------------------------------------------- tiara
  Tiara LanCard of Tiara Computer Systems.

  +----------------------------------------------+
  !           ! Transmitter Unit !               !
  !           +------------------+             -------
  !          MEM                              Coax Connector
  !  ROM    7654321 <- I/O                     -------
  !  :  :   +--------+                           !
  !  :  :   ! 90C66LJ!                         +++
  !  :  :   !        !                         !D  Switch to set
  !  :  :   !        !                         !I  the Nodenumber
  !  :  :   +--------+                         !P
  !                                            !++
  !         234567 <- IRQ                      !
  +------------!!!!!!!!!!!!!!!!!!!!!!!!--------+
	       !!!!!!!!!!!!!!!!!!!!!!!!

- 0 = Jumper Installed
  (0 = 已安装跳线)
- 1 = Open
  (1 = 断开)

Top Jumper line Bit 7 = ROM Enable 654=Memory location 321=I/O
(顶部跳线行：位 7 = ROM 启用，654 = 内存位置，321 = I/O)

Settings for Memory Location (Top Jumper Line)
(内存位置设置(顶部跳线行))

===     ================
456     Address selected (所选地址)
===     ================
000	C0000
001     C4000
010     CC000
011     D0000
100     D4000
101     D8000
110     DC000
111     E0000
===     ================

Settings for I/O Address (Top Jumper Line)
(I/O 地址设置(顶部跳线行))

===     ====
123     Port
===     ====
000	260
001	290
010	2E0
011	2F0
100	300
101	350
110	380
111	3E0
===     ====

Settings for IRQ Selection (Lower Jumper Line)
(IRQ 选择设置(下方跳线行))

====== =====
234567
====== =====
011111 IRQ 2
101111 IRQ 3
110111 IRQ 4
111011 IRQ 5
111110 IRQ 7
====== =====

Other Cards
(其他卡)
===========

I have no information on other models of ARCnet cards at the moment.
(目前我没有关于其他型号 ARCnet 卡的信息。)

Thanks.
(谢谢。)
