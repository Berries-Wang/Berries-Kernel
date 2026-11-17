.. SPDX-License-Identifier: GPL-2.0

======
ARCnet
======

(ARCnet)

:Author: Avery Pennarun <apenwarr@worldvisions.ca>

(作者: Avery Pennarun <apenwarr@worldvisions.ca>)

.. note::

   See also arcnet-hardware.txt in this directory for jumper-setting
   and cabling information if you're like many of us and didn't happen to get a
   manual with your ARCnet card.
   (如果你也和我们中的许多人一样，没随 ARCnet 卡一起拿到手册，请参阅本
   目录中的 arcnet-hardware.txt 以了解跳线设置和布线信息。)

Since no one seems to listen to me otherwise, perhaps a poem will get your
attention::
(既然似乎没人听我说话，那么也许一首诗能引起你的注意：)

		This driver's getting fat and beefy,
		But my cat is still named Fifi.

Hmm, I think I'm allowed to call that a poem, even though it's only two
lines.  Hey, I'm in Computer Science, not English.  Give me a break.
(嗯，我想我可以把它称作一首诗，尽管只有两行。嘿，我是学计算机的，不是
学英语的。放过我吧。)

The point is:  I REALLY REALLY REALLY REALLY REALLY want to hear from you if
you test this and get it working.  Or if you don't.  Or anything.
(重点是：如果你测试了这个驱动并让它跑起来了，我真的真的真的真的真的
非常想收到你的消息。就算没跑起来也想听你说说。什么都行。)

ARCnet 0.32 ALPHA first made it into the Linux kernel 1.1.80 - this was
nice, but after that even FEWER people started writing to me because they
didn't even have to install the patch.  <sigh>
(ARCnet 0.32 ALPHA 首次进入 Linux 内核 1.1.80——这很好，但从那以后给我
写信的人反而更少了，因为他们连补丁都不用装了。<叹气>)

Come on, be a sport!  Send me a success report!
(来吧，帮个忙！给我发一份成功报告！)

(hey, that was even better than my original poem... this is getting bad!)
((嘿，这比我原来那首诗还要顺口……情况越来越糟了！))

----

These are the ARCnet drivers for Linux.
(这些是 Linux 的 ARCnet 驱动。)

This new release (2.91) has been put together by David Woodhouse
<dwmw2@infradead.org>, in an attempt to tidy up the driver after adding support
for yet another chipset. Now the generic support has been separated from the
individual chipset drivers, and the source files aren't quite so packed with
#ifdefs! I've changed this file a bit, but kept it in the first person from
Avery, because I didn't want to completely rewrite it.
(这个新版本 (2.91) 由 David Woodhouse <dwmw2@infradead.org> 整合，目的是
在加入又一种芯片组的支持之后整理该驱动。现在通用支持已从各个芯片组驱动
中分离出来，源文件也不再那么充斥着 #ifdef 了！我对本文件做了一些修改，
但保留了 Avery 的第一人称叙述，因为我不想完全重写它。)

The previous release resulted from many months of on-and-off effort from me
(Avery Pennarun), many bug reports/fixes and suggestions from others, and in
particular a lot of input and coding from Tomasz Motylewski.  Starting with
ARCnet 2.10 ALPHA, Tomasz's all-new-and-improved RFC1051 support has been
included and seems to be working fine!
(上一个版本源自我 (Avery Pennarun) 断断续续数月的努力、其他人提交的大量
缺陷报告/修复与建议，特别是 Tomasz Motylewski 的大量投入和代码编写。从
ARCnet 2.10 ALPHA 开始，Tomasz 全新改进的 RFC1051 支持已被包含进来，而且
看起来工作正常！)


.. _arcnet-netdev:

Where do I discuss these drivers?(我在哪里讨论这些驱动？)
---------------------------------------------------------

ARCnet discussions take place on netdev. Simply send your email to
netdev@vger.kernel.org and make sure to Cc: maintainer listed in
"ARCNET NETWORK LAYER" heading of Documentation/process/maintainers.rst.
(ARCnet 的讨论在 netdev 上进行。只需把邮件发送到 netdev@vger.kernel.org，
并确保抄送 Documentation/process/maintainers.rst 中 "ARCNET NETWORK LAYER"
标题下所列的维护者。)

Other Drivers and Info(其他驱动与信息)
--------------------------------------

You can try my ARCNET page on the World Wide Web at:
(可以在万维网上访问我的 ARCNET 页面：)

	http://www.qis.net/~jschmitz/arcnet/

Also, SMC (one of the companies that makes ARCnet cards) has a WWW site you
might be interested in, which includes several drivers for various cards
including ARCnet.  Try:
(另外，SMC (ARCnet 卡的制造商之一) 有一个你可能感兴趣的网站，其中包含
包括 ARCnet 在内的多种卡片的若干驱动。试试：)

	http://www.smc.com/

Performance Technologies makes various network software that supports
ARCnet:
(Performance Technologies 开发多种支持 ARCnet 的网络软件：)

	http://www.perftech.com/ or ftp to ftp.perftech.com.

Novell makes a networking stack for DOS which includes ARCnet drivers.  Try
FTPing to ftp.novell.com.
(Novell 为 DOS 提供一个包含 ARCnet 驱动的网络协议栈。可以 FTP 到
ftp.novell.com。)

You can get the Crynwr packet driver collection (including arcether.com, the
one you'll want to use with ARCnet cards) from
oak.oakland.edu:/simtel/msdos/pktdrvr. It won't work perfectly on a 386+
without patches, though, and also doesn't like several cards.  Fixed
versions are available on my WWW page, or via e-mail if you don't have WWW
access.
(你可以从 oak.oakland.edu:/simtel/msdos/pktdrvr 获取 Crynwr 数据包驱动
程序集合 (包括 arcether.com，也就是你想配合 ARCnet 卡使用的那个)。不过，
不加补丁的话它在 386+ 上不能完美工作，而且对几张卡也不友好。修复版可以
在我的网页上获取；如果你不能访问万维网，也可以通过电子邮件获取。)


Installing the Driver(安装驱动)
-------------------------------

All you will need to do in order to install the driver is::
(要安装这个驱动，你所需要做的就是：)

	make config
		(be sure to choose ARCnet in the network devices
		and at least one chipset driver.)
	make clean
	make zImage

If you obtained this ARCnet package as an upgrade to the ARCnet driver in
your current kernel, you will need to first copy arcnet.c over the one in
the linux/drivers/net directory.
(如果你获得这个 ARCnet 软件包是为了升级当前内核中的 ARCnet 驱动，则需要
先把 arcnet.c 覆盖到 linux/drivers/net 目录中的那个文件。)

You will know the driver is installed properly if you get some ARCnet
messages when you reboot into the new Linux kernel.
(如果重启进入新的 Linux 内核时看到一些 ARCnet 消息，就说明驱动已正确
安装。)

There are four chipset options:
(有四种芯片组选项：)

 1. Standard ARCnet COM90xx chipset.
    (标准 ARCnet COM90xx 芯片组。)

This is the normal ARCnet card, which you've probably got. This is the only
chipset driver which will autoprobe if not told where the card is.
It following options on the command line::
(这是普通的 ARCnet 卡，你手上那张很可能就是。这是唯一一个在没有被告知
卡的位置时会自动探测的芯片组驱动。它支持下列命令行选项：)

 com90xx=[<io>[,<irq>[,<shmem>]]][,<name>] | <name>

If you load the chipset support as a module, the options are::
(如果以模块方式加载芯片组支持，则选项为：)

 io=<io> irq=<irq> shmem=<shmem> device=<name>

To disable the autoprobe, just specify "com90xx=" on the kernel command line.
To specify the name alone, but allow autoprobe, just put "com90xx=<name>"
(要禁用自动探测，只需在内核命令行上指定 "com90xx="。要只指定名称但允许
自动探测，只需写 "com90xx=<name>"。)

 2. ARCnet COM20020 chipset.
    (ARCnet COM20020 芯片组。)

This is the new chipset from SMC with support for promiscuous mode (packet
sniffing), extra diagnostic information, etc. Unfortunately, there is no
sensible method of autoprobing for these cards. You must specify the I/O
address on the kernel command line.
(这是来自 SMC 的新芯片组，支持混杂模式 (数据包嗅探)、额外的诊断信息等。
遗憾的是，对这种卡没有合理的自动探测方法。你必须在内核命令行上指定
I/O 地址。)

The command line options are::
(命令行选项为：)

 com20020=<io>[,<irq>[,<node_ID>[,backplane[,CKP[,timeout]]]]][,name]

If you load the chipset support as a module, the options are::
(如果以模块方式加载芯片组支持，则选项为：)

 io=<io> irq=<irq> node=<node_ID> backplane=<backplane> clock=<CKP>
 timeout=<timeout> device=<name>

The COM20020 chipset allows you to set the node ID in software, overriding the
default which is still set in DIP switches on the card. If you don't have the
COM20020 data sheets, and you don't know what the other three options refer
to, then they won't interest you - forget them.
(COM20020 芯片组允许你在软件中设置节点 ID，覆盖卡上 DIP 开关仍然设置的
默认值。如果你没有 COM20020 的数据手册，也不知道其他三个选项指的是什么，
那么它们不会引起你的兴趣——忘掉它们吧。)

 3. ARCnet COM90xx chipset in IO-mapped mode.
    (IO 映射模式下的 ARCnet COM90xx 芯片组。)

This will also work with the normal ARCnet cards, but doesn't use the shared
memory. It performs less well than the above driver, but is provided in case
you have a card which doesn't support shared memory, or (strangely) in case
you have so many ARCnet cards in your machine that you run out of shmem slots.
If you don't give the IO address on the kernel command line, then the driver
will not find the card.
(它也能用于普通的 ARCnet 卡，但不使用共享内存。它的性能不如上面的驱动，
提供它是为了应对你的卡不支持共享内存的情况，或者 (说来奇怪) 你的机器里
插了太多 ARCnet 卡以致 shmem 插槽用光的情况。如果你不在内核命令行上给出
IO 地址，驱动将找不到卡。)

The command line options are::
(命令行选项为：)

 com90io=<io>[,<irq>][,<name>]

If you load the chipset support as a module, the options are:
 io=<io> irq=<irq> device=<name>
 (如果以模块方式加载芯片组支持，则选项为：)

 4. ARCnet RIM I cards.
    (ARCnet RIM I 卡。)

These are COM90xx chips which are _completely_ memory mapped. The support for
these is not tested. If you have one, please mail the author with a success
report. All options must be specified, except the device name.
Command line options::
(这些是完全内存映射的 COM90xx 芯片。对它们的支持未经测试。如果你有这样
一块卡，请给作者发邮件报告成功消息。除设备名外，所有选项都必须指定。
命令行选项：)

 arcrimi=<shmem>,<irq>,<node_ID>[,<name>]

If you load the chipset support as a module, the options are::
(如果以模块方式加载芯片组支持，则选项为：)

 shmem=<shmem> irq=<irq> node=<node_ID> device=<name>


Loadable Module Support(可加载模块支持)
---------------------------------------

Configure and rebuild Linux.  When asked, answer 'm' to "Generic ARCnet
support" and to support for your ARCnet chipset if you want to use the
loadable module. You can also say 'y' to "Generic ARCnet support" and 'm'
to the chipset support if you wish.
(配置并重新构建 Linux。在提问时，如果想使用可加载模块，对 "Generic
ARCnet support" 以及你的 ARCnet 芯片组支持回答 'm'。如果愿意，你也可以
对 "Generic ARCnet support" 回答 'y'、对芯片组支持回答 'm'。)

::

	make config
	make clean
	make zImage
	make modules

If you're using a loadable module, you need to use insmod to load it, and
you can specify various characteristics of your card on the command
line.  (In recent versions of the driver, autoprobing is much more reliable
and works as a module, so most of this is now unnecessary.)
(如果使用可加载模块，需要用 insmod 加载它，并且可以在命令行上指定卡的
多种特性。(在驱动的近期版本中，自动探测可靠得多，而且可以作为模块工作，
所以其中大部分现在已经没有必要了。))

For example::
(例如：)

	cd /usr/src/linux/modules
	insmod arcnet.o
	insmod com90xx.o
	insmod com20020.o io=0x2e0 device=eth1


Using the Driver(使用驱动)
--------------------------

If you build your kernel with ARCnet COM90xx support included, it should
probe for your card automatically when you boot. If you use a different
chipset driver complied into the kernel, you must give the necessary options
on the kernel command line, as detailed above.
(如果你的内核编译时包含了 ARCnet COM90xx 支持，那么启动时它应当会自动
探测你的卡。如果编译进内核的是其他芯片组驱动，则必须按上面详述的那样在
内核命令行上给出必要的选项。)

Go read the NET-2-HOWTO and ETHERNET-HOWTO for Linux; they should be
available where you picked up this driver.  Think of your ARCnet as a
souped-up (or down, as the case may be) Ethernet card.
(去阅读 Linux 的 NET-2-HOWTO 和 ETHERNET-HOWTO；它们应当能在你获取本
驱动的地方找到。把你的 ARCnet 想象成一块加强版 (或者按实际情况，降级版)
的以太网卡。)

By the way, be sure to change all references from "eth0" to "arc0" in the
HOWTOs.  Remember that ARCnet isn't a "true" Ethernet, and the device name
is DIFFERENT.
(顺便说一句，务必把 HOWTO 中所有 "eth0" 的引用改为 "arc0"。记住，ARCnet
不是"真正的"以太网，设备名称是不一样的。)


Multiple Cards in One Computer(一台计算机中的多块卡)
----------------------------------------------------

Linux has pretty good support for this now, but since I've been busy, the
ARCnet driver has somewhat suffered in this respect. COM90xx support, if
compiled into the kernel, will (try to) autodetect all the installed cards.
(Linux 现在对这一点已有相当好的支持，但由于我一直很忙，ARCnet 驱动在
这方面有所欠缺。COM90xx 支持如果编译进内核，会 (尝试) 自动检测所有已
安装的卡。)

If you have other cards, with support compiled into the kernel, then you can
just repeat the options on the kernel command line, e.g.::
(如果你的其他卡的支持编译进了内核，那么只需在内核命令行上重复这些选项，
例如：)

	LILO: linux com20020=0x2e0 com20020=0x380 com90io=0x260

If you have the chipset support built as a loadable module, then you need to
do something like this::
(如果芯片组支持被构建为可加载模块，那么你需要像这样做：)

	insmod -o arc0 com90xx
	insmod -o arc1 com20020 io=0x2e0
	insmod -o arc2 com90xx

The ARCnet drivers will now sort out their names automatically.
(ARCnet 驱动现在会自动理清它们的名称。)


How do I get it to work with...?(如何让它与……协同工作？)
--------------------------------------------------------

NFS:
(NFS：)

	Should be fine linux->linux, just pretend you're using Ethernet cards.
	oak.oakland.edu:/simtel/msdos/nfs has some nice DOS clients.  There
	is also a DOS-based NFS server called SOSS.  It doesn't multitask
	quite the way Linux does (actually, it doesn't multitask AT ALL) but
	you never know what you might need.
	(linux 到 linux 应该没问题，就假装你在用以太网卡好了。
	oak.oakland.edu:/simtel/msdos/nfs 有一些不错的 DOS 客户端。还有一个
	叫 SOSS 的基于 DOS 的 NFS 服务器。它的多任务方式与 Linux 不太一样
	(实际上，它根本不多任务)，但你永远不知道自己会需要什么。)

	With AmiTCP (and possibly others), you may need to set the following
	options in your Amiga nfstab:  MD 1024 MR 1024 MW 1024
	(Thanks to Christian Gottschling <ferksy@indigo.tng.oche.de>
	for this.)
	(使用 AmiTCP (可能还有其他软件) 时，可能需要在 Amiga 的 nfstab 中
	设置以下选项：MD 1024 MR 1024 MW 1024 (感谢 Christian Gottschling
	<ferksy@indigo.tng.oche.de> 提供此信息)。)

	Probably these refer to maximum NFS data/read/write block sizes.  I
	don't know why the defaults on the Amiga didn't work; write to me if
	you know more.
	(这些可能指的是 NFS 数据/读/写块的最大尺寸。我不知道为什么 Amiga 上
	的默认值不管用；如果你了解更多，请写信告诉我。)

DOS:
(DOS：)

	If you're using the freeware arcether.com, you might want to install
	the driver patch from my web page.  It helps with PC/TCP, and also
	can get arcether to load if it timed out too quickly during
	initialization.  In fact, if you use it on a 386+ you REALLY need
	the patch, really.
	(如果你使用免费的 arcether.com，可能需要安装我网页上的驱动补丁。它对
	PC/TCP 有帮助，还能让 arcether 在初始化期间超时太快的情况下加载起来。
	实际上，如果你在 386+ 上使用它，你真的非常需要这个补丁，真的。)

Windows:
(Windows：)

	See DOS :)  Trumpet Winsock works fine with either the Novell or
	Arcether client, assuming you remember to load winpkt of course.
	(参见 DOS 部分。Trumpet Winsock 配合 Novell 或 Arcether 客户端都能
	正常工作，当然前提是你记得加载 winpkt。)

LAN Manager and Windows for Workgroups:
(LAN Manager 与 Windows for Workgroups：)

	These programs use protocols that
	are incompatible with the Internet standard.  They try to pretend
	the cards are Ethernet, and confuse everyone else on the network.
	(这些程序使用的协议与互联网标准不兼容。它们试图假装网卡是以太网卡，
	从而让网络上的其他所有人感到困惑。)

	However, v2.00 and higher of the Linux ARCnet driver supports this
	protocol via the 'arc0e' device.  See the section on "Multiprotocol
	Support" for more information.
	(不过，v2.00 及更高版本的 Linux ARCnet 驱动通过 'arc0e' 设备支持这种
	协议。更多信息参见 "Multiprotocol Support" 一节。)

	Using the freeware Samba server and clients for Linux, you can now
	interface quite nicely with TCP/IP-based WfWg or Lan Manager
	networks.
	(使用 Linux 的免费 Samba 服务器和客户端，你现在可以与基于 TCP/IP 的
	WfWg 或 Lan Manager 网络相当好地互操作。)

Windows 95:
(Windows 95：)

	Tools are included with Win95 that let you use either the LANMAN
	style network drivers (NDIS) or Novell drivers (ODI) to handle your
	ARCnet packets.  If you use ODI, you'll need to use the 'arc0'
	device with Linux.  If you use NDIS, then try the 'arc0e' device.
	See the "Multiprotocol Support" section below if you need arc0e,
	you're completely insane, and/or you need to build some kind of
	hybrid network that uses both encapsulation types.
	(Win95 自带一些工具，让你可以使用 LANMAN 风格的网络驱动 (NDIS) 或
	Novell 驱动 (ODI) 来处理 ARCnet 数据包。如果使用 ODI，在 Linux 中
	你需要使用 'arc0' 设备。如果使用 NDIS，那么试试 'arc0e' 设备。如果
	你需要 arc0e、你彻底疯了、和/或你需要构建某种同时使用两种封装类型的
	混合网络，请参见下文的 "Multiprotocol Support" 一节。)

OS/2:
(OS/2：)

	I've been told it works under Warp Connect with an ARCnet driver from
	SMC.  You need to use the 'arc0e' interface for this.  If you get
	the SMC driver to work with the TCP/IP stuff included in the
	"normal" Warp Bonus Pack, let me know.
	(有人告诉我，使用 SMC 的 ARCnet 驱动时它在 Warp Connect 下可以工作。
	为此你需要使用 'arc0e' 接口。如果你让 SMC 驱动与 "普通" Warp Bonus
	Pack 中附带的 TCP/IP 组件协同工作，请告诉我。)

	ftp.microsoft.com also has a freeware "Lan Manager for OS/2" client
	which should use the same protocol as WfWg does.  I had no luck
	installing it under Warp, however.  Please mail me with any results.
	(ftp.microsoft.com 上也有一个免费的 "Lan Manager for OS/2" 客户端，
	它应当使用与 WfWg 相同的协议。不过我在 Warp 下安装它没有成功。无论
	结果如何都请写信告诉我。)

NetBSD/AmiTCP:
(NetBSD/AmiTCP：)

	These use an old version of the Internet standard ARCnet
	protocol (RFC1051) which is compatible with the Linux driver v2.10
	ALPHA and above using the arc0s device. (See "Multiprotocol ARCnet"
	below.)  ** Newer versions of NetBSD apparently support RFC1201.
	(它们使用互联网标准 ARCnet 协议的一个旧版本 (RFC1051)，它与 Linux
	驱动 v2.10 ALPHA 及以上版本配合 arc0s 设备时兼容。(参见下文的
	"Multiprotocol ARCnet"。) 较新版本的 NetBSD 显然支持 RFC1201。)


Using Multiprotocol ARCnet(使用多协议 ARCnet)
---------------------------------------------

The ARCnet driver v2.10 ALPHA supports three protocols, each on its own
"virtual network device":
(ARCnet 驱动 v2.10 ALPHA 支持三种协议，每种协议都在自己的 "虚拟网络设备"
上：)

	======  ===============================================================
	arc0	RFC1201 protocol, the official Internet standard which just
		happens to be 100% compatible with Novell's TRXNET driver.
		Version 1.00 of the ARCnet driver supported _only_ this
		protocol.  arc0 is the fastest of the three protocols (for
		whatever reason), and allows larger packets to be used
		because it supports RFC1201 "packet splitting" operations.
		Unless you have a specific need to use a different protocol,
		I strongly suggest that you stick with this one.
		(RFC1201 协议，官方互联网标准，恰好与 Novell 的 TRXNET 驱动
		100% 兼容。ARCnet 驱动 1.00 版只支持这一种协议。arc0 是三种
		协议中最快的 (不管什么原因)，并且由于支持 RFC1201 的 "packet
		splitting" (数据包拆分) 操作而允许使用更大的数据包。除非你有
		使用其他协议的特定需求，否则我强烈建议使用这一种。)

	arc0e	"Ethernet-Encapsulation" which sends packets over ARCnet
		that are actually a lot like Ethernet packets, including the
		6-byte hardware addresses.  This protocol is compatible with
		Microsoft's NDIS ARCnet driver, like the one in WfWg and
		LANMAN.  Because the MTU of 493 is actually smaller than the
		one "required" by TCP/IP (576), there is a chance that some
		network operations will not function properly.  The Linux
		TCP/IP layer can compensate in most cases, however, by
		automatically fragmenting the TCP/IP packets to make them
		fit.  arc0e also works slightly more slowly than arc0, for
		reasons yet to be determined.  (Probably it's the smaller
		MTU that does it.)
		("Ethernet-Encapsulation" (以太网封装)，它通过 ARCnet 发送的
		数据包实际上非常类似于以太网数据包，包括 6 字节硬件地址。该
		协议与 Microsoft 的 NDIS ARCnet 驱动兼容，例如 WfWg 和 LANMAN
		中的那个。由于 493 的 MTU 实际上小于 TCP/IP "要求" 的值 (576)，
		某些网络操作有可能无法正常工作。不过在大多数情况下，Linux 的
		TCP/IP 层可以通过自动分片 TCP/IP 数据包使其适应来加以弥补。
		arc0e 的工作速度也略慢于 arc0，原因尚待确定。(大概就是较小的
		MTU 所致。))

	arc0s	The "[s]imple" RFC1051 protocol is the "previous" Internet
		standard that is completely incompatible with the new
		standard.  Some software today, however, continues to
		support the old standard (and only the old standard)
		including NetBSD and AmiTCP.  RFC1051 also does not support
		RFC1201's packet splitting, and the MTU of 507 is still
		smaller than the Internet "requirement," so it's quite
		possible that you may run into problems.  It's also slower
		than RFC1201 by about 25%, for the same reason as arc0e.
		("[s]imple" RFC1051 协议是 "以前的" 互联网标准，与新标准完全
		不兼容。不过如今一些软件仍在支持旧标准 (而且只支持旧标准)，
		包括 NetBSD 和 AmiTCP。RFC1051 也不支持 RFC1201 的数据包拆分，
		且 507 的 MTU 仍小于互联网的 "要求"，因此你很可能遇到问题。
		出于与 arc0e 相同的原因，它也比 RFC1201 慢约 25%。)

		The arc0s support was contributed by Tomasz Motylewski
		and modified somewhat by me.  Bugs are probably my fault.
		(arc0s 的支持由 Tomasz Motylewski 贡献，并由我稍作修改。缺陷
		大概是我的错。)
	======  ===============================================================

You can choose not to compile arc0e and arc0s into the driver if you want -
this will save you a bit of memory and avoid confusion when eg. trying to
use the "NFS-root" stuff in recent Linux kernels.
(如果愿意，你可以选择不把 arc0e 和 arc0s 编译进驱动——这会为你节省一些
内存，并避免在使用较新 Linux 内核的 "NFS-root" 功能之类时产生困惑。)

The arc0e and arc0s devices are created automatically when you first
ifconfig the arc0 device.  To actually use them, though, you need to also
ifconfig the other virtual devices you need.  There are a number of ways you
can set up your network then:
(当你第一次对 arc0 设备执行 ifconfig 时，arc0e 和 arc0s 设备会被自动
创建。不过要真正使用它们，你还需要对你需要的其他虚拟设备执行 ifconfig。
此后有多种方式可以搭建你的网络：)


1. Single Protocol.
   (单协议。)

   This is the simplest way to configure your network: use just one of the
   two available protocols.  As mentioned above, it's a good idea to use
   only arc0 unless you have a good reason (like some other software, ie.
   WfWg, that only works with arc0e).
   (这是配置网络最简单的方式：只使用两个可用协议之一。如上所述，除非你
   有充分的理由 (比如某些其他软件，如 WfWg，只能与 arc0e 配合工作)，否则
   最好只用 arc0。)

   If you need only arc0, then the following commands should get you going::
   (如果你只需要 arc0，那么下面的命令应该就能让你跑起来：)

	ifconfig arc0 MY.IP.ADD.RESS
	route add MY.IP.ADD.RESS arc0
	route add -net SUB.NET.ADD.RESS arc0
	[add other local routes here]

   If you need arc0e (and only arc0e), it's a little different::
   (如果你需要 arc0e (而且只要 arc0e)，则稍有不同：)

	ifconfig arc0 MY.IP.ADD.RESS
	ifconfig arc0e MY.IP.ADD.RESS
	route add MY.IP.ADD.RESS arc0e
	route add -net SUB.NET.ADD.RESS arc0e

   arc0s works much the same way as arc0e.
   (arc0s 的工作方式与 arc0e 大致相同。)


2. More than one protocol on the same wire.
   (同一根线上有多个协议。)

   Now things start getting confusing.  To even try it, you may need to be
   partly crazy.  Here's what *I* did. :) Note that I don't include arc0s in
   my home network; I don't have any NetBSD or AmiTCP computers, so I only
   use arc0s during limited testing.
   (现在事情开始变得混乱了。要尝试它，你可能得有点疯。下面就是我的做法。
   :) 注意，我的家庭网络中不包括 arc0s；我没有 NetBSD 或 AmiTCP 计算机，
   所以我只在有限的测试中使用 arc0s。)

   I have three computers on my home network; two Linux boxes (which prefer
   RFC1201 protocol, for reasons listed above), and one XT that can't run
   Linux but runs the free Microsoft LANMAN Client instead.
   (我的家庭网络中有三台计算机；两台 Linux 主机 (出于上面列出的原因，它们
   偏好 RFC1201 协议)，以及一台不能运行 Linux、转而运行免费的 Microsoft
   LANMAN Client 的 XT。)

   Worse, one of the Linux computers (freedom) also has a modem and acts as
   a router to my Internet provider.  The other Linux box (insight) also has
   its own IP address and needs to use freedom as its default gateway.  The
   XT (patience), however, does not have its own Internet IP address and so
   I assigned it one on a "private subnet" (as defined by RFC1597).
   (更麻烦的是，其中一台 Linux 计算机 (freedom) 还有一个调制解调器，并
   充当到我的互联网提供商的路由器。另一台 Linux 主机 (insight) 也有自己的
   IP 地址，需要使用 freedom 作为其默认网关。而那台 XT (patience) 没有自己
   的互联网 IP 地址，所以我给它分配了一个 "私有子网" (由 RFC1597 定义)
   上的地址。)

   To start with, take a simple network with just insight and freedom.
   Insight needs to:
   (先从只有 insight 和 freedom 的简单网络开始。insight 需要：)

	- talk to freedom via RFC1201 (arc0) protocol, because I like it
	  more and it's faster.
	  (通过 RFC1201 (arc0) 协议与 freedom 通信，因为我更喜欢它，而且
	  它更快。)
	- use freedom as its Internet gateway.
	  (使用 freedom 作为其互联网网关。)

   That's pretty easy to do.  Set up insight like this::
   (这很容易做到。像这样设置 insight：)

	ifconfig arc0 insight
	route add insight arc0
	route add freedom arc0	/* I would use the subnet here (like I said
					to in "single protocol" above),
					but the rest of the subnet
					unfortunately lies across the PPP
					link on freedom, which confuses
					things. */
	route add default gw freedom

   And freedom gets configured like so::
   (而 freedom 则像这样配置：)

	ifconfig arc0 freedom
	route add freedom arc0
	route add insight arc0
	/* and default gateway is configured by pppd */

   Great, now insight talks to freedom directly on arc0, and sends packets
   to the Internet through freedom.  If you didn't know how to do the above,
   you should probably stop reading this section now because it only gets
   worse.
   (很好，现在 insight 通过 arc0 直接与 freedom 通信，并经由 freedom 向
   互联网发送数据包。如果你不知道如何完成上面的操作，那么现在可能就该
   停止阅读本节了，因为后面只会更糟。)

   Now, how do I add patience into the network?  It will be using LANMAN
   Client, which means I need the arc0e device.  It needs to be able to talk
   to both insight and freedom, and also use freedom as a gateway to the
   Internet.  (Recall that patience has a "private IP address" which won't
   work on the Internet; that's okay, I configured Linux IP masquerading on
   freedom for this subnet).
   (现在，如何把 patience 加入网络？它将使用 LANMAN Client，这意味着我
   需要 arc0e 设备。它需要能够与 insight 和 freedom 都通信，并且也使用
   freedom 作为到互联网的网关。(回想一下，patience 有一个在互联网上无法
   使用的 "私有 IP 地址"；没关系，我在 freedom 上为这个子网配置了 Linux
   IP 伪装)。)

   So patience (necessarily; I don't have another IP number from my
   provider) has an IP address on a different subnet than freedom and
   insight, but needs to use freedom as an Internet gateway.  Worse, most
   DOS networking programs, including LANMAN, have braindead networking
   schemes that rely completely on the netmask and a 'default gateway' to
   determine how to route packets.  This means that to get to freedom or
   insight, patience WILL send through its default gateway, regardless of
   the fact that both freedom and insight (courtesy of the arc0e device)
   could understand a direct transmission.
   (于是 patience (必然如此；我从提供商那里没有别的 IP 号可用) 在与
   freedom 和 insight 不同的子网上拥有 IP 地址，但需要使用 freedom 作为
   互联网网关。更糟的是，包括 LANMAN 在内的大多数 DOS 网络程序都有愚蠢
   的网络方案，完全依赖网络掩码和 "默认网关" 来决定如何路由数据包。这
   意味着要到达 freedom 或 insight，patience 将会通过其默认网关发送，
   而不顾 freedom 和 insight (多亏 arc0e 设备) 本可以直接理解直接传输
   这一事实。)

   I compensate by giving freedom an extra IP address - aliased 'gatekeeper' -
   that is on my private subnet, the same subnet that patience is on.  I
   then define gatekeeper to be the default gateway for patience.
   (我通过给 freedom 一个额外的 IP 地址来弥补——别名 'gatekeeper'——
   它位于我的私有子网上，也就是 patience 所在的子网。然后我把 gatekeeper
   定义为 patience 的默认网关。)

   To configure freedom (in addition to the commands above)::
   (要配置 freedom (在上述命令之外还要执行)：)

	ifconfig arc0e gatekeeper
	route add gatekeeper arc0e
	route add patience arc0e

   This way, freedom will send all packets for patience through arc0e,
   giving its IP address as gatekeeper (on the private subnet).  When it
   talks to insight or the Internet, it will use its "freedom" Internet IP
   address.
   (这样，freedom 发往 patience 的所有数据包都会通过 arc0e，并以
   gatekeeper (私有子网上) 的 IP 地址作为源地址。当它与 insight 或互联网
   通信时，将使用它的 "freedom" 互联网 IP 地址。)

   You will notice that we haven't configured the arc0e device on insight.
   This would work, but is not really necessary, and would require me to
   assign insight another special IP number from my private subnet.  Since
   both insight and patience are using freedom as their default gateway, the
   two can already talk to each other.
   (你会注意到，我们没有在 insight 上配置 arc0e 设备。这样做也可以，但
   并非真正必要，而且需要我从私有子网中再给 insight 分配一个特殊的 IP 号。
   由于 insight 和 patience 都使用 freedom 作为默认网关，二者其实已经
   可以互相通信。)

   It's quite fortunate that I set things up like this the first time (cough
   cough) because it's really handy when I boot insight into DOS.  There, it
   runs the Novell ODI protocol stack, which only works with RFC1201 ARCnet.
   In this mode it would be impossible for insight to communicate directly
   with patience, since the Novell stack is incompatible with Microsoft's
   Ethernet-Encap.  Without changing any settings on freedom or patience, I
   simply set freedom as the default gateway for insight (now in DOS,
   remember) and all the forwarding happens "automagically" between the two
   hosts that would normally not be able to communicate at all.
   (幸运的是，我第一次就 (咳咳) 像这样把事情设置好了，因为当我把 insight
   启动到 DOS 时这真的非常方便。在那里，它运行 Novell ODI 协议栈，而该
   协议栈只能与 RFC1201 ARCnet 配合工作。在这种模式下，insight 不可能
   直接与 patience 通信，因为 Novell 协议栈与 Microsoft 的以太网封装
   不兼容。无需更改 freedom 或 patience 上的任何设置，我只需把 freedom
   设为 insight 的默认网关 (记住，此时 insight 在 DOS 下)，于是在这两个
   原本完全无法通信的主机之间，所有转发都会 "自动地" 魔术般发生。)

   For those who like diagrams, I have created two "virtual subnets" on the
   same physical ARCnet wire.  You can picture it like this::
   (对于喜欢图示的人：我在同一条物理 ARCnet 线上创建了两个 "虚拟子网"。
   你可以这样想象它：)


	  [RFC1201 NETWORK]                   [ETHER-ENCAP NETWORK]
      (registered Internet subnet)           (RFC1597 private subnet)

			     (IP Masquerade)
	  /---------------\         *            /---------------\
	  |               |         *            |               |
	  |               +-Freedom-*-Gatekeeper-+               |
	  |               |    |    *            |               |
	  \-------+-------/    |    *            \-------+-------/
		  |            |                         |
	       Insight         |                      Patience
			   (Internet)



It works: what now?(它能工作了：接下来做什么？)
-----------------------------------------------

Send mail following :ref:`arcnet-netdev`. Describe your setup, preferably
including driver version, kernel version, ARCnet card model, CPU type, number
of systems on your network, and list of software in use.
(按照 :ref:`arcnet-netdev` 发送邮件。描述你的配置环境，最好包括驱动版本、
内核版本、ARCnet 卡型号、CPU 类型、网络上的系统数量以及所使用软件的
清单。)

It doesn't work: what now?(它不工作：接下来做什么？)
----------------------------------------------------

Do the same as above, but also include the output of the ifconfig and route
commands, as well as any pertinent log entries (ie. anything that starts
with "arcnet:" and has shown up since the last reboot) in your mail.
(执行与上面相同的操作，但在邮件中还要附上 ifconfig 和 route 命令的输出，
以及任何相关的日志条目 (即任何以 "arcnet:" 开头且自上次重启以来出现的
内容)。)

If you want to try fixing it yourself (I strongly recommend that you mail me
about the problem first, since it might already have been solved) you may
want to try some of the debug levels available.  For heavy testing on
D_DURING or more, it would be a REALLY good idea to kill your klogd daemon
first!  D_DURING displays 4-5 lines for each packet sent or received.  D_TX,
D_RX, and D_SKB actually DISPLAY each packet as it is sent or received,
which is obviously quite big.
(如果你想尝试自己修复 (我强烈建议你先就这个问题给我发邮件，因为它可能
已经被解决了)，你可以试试一些可用的调试级别。对于 D_DURING 或更高级别
的大量测试，先杀掉你的 klogd 守护进程会是个非常非常好的主意！D_DURING
会为每个发送或接收的数据包显示 4-5 行信息。D_TX、D_RX 和 D_SKB 则会
实际显示每个发送或接收的数据包，这显然相当庞大。)

Starting with v2.40 ALPHA, the autoprobe routines have changed
significantly.  In particular, they won't tell you why the card was not
found unless you turn on the D_INIT_REASONS debugging flag.
(从 v2.40 ALPHA 开始，自动探测例程发生了重大变化。特别是，除非你打开
D_INIT_REASONS 调试标志，否则它们不会告诉你为什么没有找到卡。)

Once the driver is running, you can run the arcdump shell script (available
from me or in the full ARCnet package, if you have it) as root to list the
contents of the arcnet buffers at any time.  To make any sense at all out of
this, you should grab the pertinent RFCs. (some are listed near the top of
arcnet.c).  arcdump assumes your card is at 0xD0000.  If it isn't, edit the
script.
(驱动运行起来之后，你可以以 root 身份运行 arcdump shell 脚本 (可向我
索取，如果你有完整的 ARCnet 软件包，也可以从中获取)，随时列出 arcnet
缓冲区的内容。要真正看懂这些内容，你应该去获取相关的 RFC。(有些 RFC
列在 arcnet.c 靠近顶部的位置)。arcdump 假定你的卡位于 0xD0000。如果不是，
请编辑该脚本。)

Buffers 0 and 1 are used for receiving, and Buffers 2 and 3 are for sending.
Ping-pong buffers are implemented both ways.
(缓冲区 0 和 1 用于接收，缓冲区 2 和 3 用于发送。两个方向都实现了乒乓
缓冲。)

If your debug level includes D_DURING and you did NOT define SLOW_XMIT_COPY,
the buffers are cleared to a constant value of 0x42 every time the card is
reset (which should only happen when you do an ifconfig up, or when Linux
decides that the driver is broken).  During a transmit, unused parts of the
buffer will be cleared to 0x42 as well.  This is to make it easier to figure
out which bytes are being used by a packet.
(如果你的调试级别包含 D_DURING 且你没有定义 SLOW_XMIT_COPY，那么每次
卡被复位时缓冲区都会被清成常量值 0x42 (这应当只在你执行 ifconfig up 时，
或 Linux 判定驱动损坏时发生)。在发送期间，缓冲区中未使用的部分也会被
清成 0x42。这是为了让弄清哪些字节被某个数据包使用变得更容易。)

You can change the debug level without recompiling the kernel by typing::
(无需重新编译内核，只要输入以下命令就可以改变调试级别：)

	ifconfig arc0 down metric 1xxx
	/etc/rc.d/rc.inet1

where "xxx" is the debug level you want.  For example, "metric 1015" would put
you at debug level 15.  Debug level 7 is currently the default.
(其中 "xxx" 是你想要的调试级别。例如，"metric 1015" 会把调试级别设为
15。调试级别 7 目前是默认值。)

Note that the debug level is (starting with v1.90 ALPHA) a binary
combination of different debug flags; so debug level 7 is really 1+2+4 or
D_NORMAL+D_EXTRA+D_INIT.  To include D_DURING, you would add 16 to this,
resulting in debug level 23.
(注意，调试级别 (从 v1.90 ALPHA 开始) 是不同调试标志的二进制组合；所以
调试级别 7 实际上是 1+2+4，即 D_NORMAL+D_EXTRA+D_INIT。要包含
D_DURING，需要在此基础上加 16，得到调试级别 23。)

If you don't understand that, you probably don't want to know anyway.
E-mail me about your problem.
(如果你不理解这些，那么反正你可能也不想弄明白。就你的问题给我发邮件吧。)


I want to send money: what now?(我想寄钱：接下来做什么？)
---------------------------------------------------------

Go take a nap or something.  You'll feel better in the morning.
(去打个盹或做点别的什么吧。早上你会感觉好一些。)
