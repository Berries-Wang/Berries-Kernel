.. SPDX-License-Identifier: GPL-2.0

=====================
TLAN driver for Linux
=====================

(Linux 的 TLAN 驱动)

:Version: 1.14a

(版本: 1.14a)

(C) 1997-1998 Caldera, Inc.
(版权所有 (C) 1997-1998 Caldera, Inc.)

(C) 1998 James Banks
(版权所有 (C) 1998 James Banks)

(C) 1999-2001 Torben Mathiasen <tmm@image.dk, torben.mathiasen@compaq.com>
(版权所有 (C) 1999-2001 Torben Mathiasen <tmm@image.dk, torben.mathiasen@compaq.com>)

For driver information/updates visit http://www.compaq.com
(如需驱动信息或更新，请访问 http://www.compaq.com)




I. Supported Devices
(I. 支持的设备)
====================

    Only PCI devices will work with this driver.
    (只有 PCI 设备才能与本驱动协同工作。)

    Supported:
    (支持的设备：)

    =========	=========	===========================================
    Vendor ID	Device ID	Name (名称)
    =========	=========	===========================================
    0e11	ae32		Compaq Netelligent 10/100 TX PCI UTP
    0e11	ae34		Compaq Netelligent 10 T PCI UTP
    0e11	ae35		Compaq Integrated NetFlex 3/P
    0e11	ae40		Compaq Netelligent Dual 10/100 TX PCI UTP
    0e11	ae43		Compaq Netelligent Integrated 10/100 TX UTP
    0e11	b011		Compaq Netelligent 10/100 TX Embedded UTP
    0e11	b012		Compaq Netelligent 10 T/2 PCI UTP/Coax
    0e11	b030		Compaq Netelligent 10/100 TX UTP
    0e11	f130		Compaq NetFlex 3/P
    0e11	f150		Compaq NetFlex 3/P
    108d	0012		Olicom OC-2325
    108d	0013		Olicom OC-2183
    108d	0014		Olicom OC-2326
    =========	=========	===========================================


    Caveats:
    (注意事项：)

    I am not sure if 100BaseTX daughterboards (for those cards which
    support such things) will work.  I haven't had any solid evidence
    either way.
    (我不确定 100BaseTX 子板 (对于支持此类子板的那些网卡) 是否能正常工作。
    目前还没有任何确凿的证据能证明能或不能。)

    However, if a card supports 100BaseTx without requiring an add
    on daughterboard, it should work with 100BaseTx.
    (不过，如果网卡无需附加子板即可支持 100BaseTx，那么它应当可以在
    100BaseTx 下正常工作。)

    The "Netelligent 10 T/2 PCI UTP/Coax" (b012) device is untested,
    but I do not expect any problems.
    ("Netelligent 10 T/2 PCI UTP/Coax" (b012) 设备未经测试，但我预计不会有
    什么问题。)


II. Driver Options
(II. 驱动选项)
==================

	1. You can append debug=x to the end of the insmod line to get
	   debug messages, where x is a bit field where the bits mean
	   the following:
	   (可以在 insmod 命令行末尾附加 debug=x 来获取调试消息，其中 x 是
	   一个位域，各个位的含义如下：)

	   ====		=====================================
	   0x01		Turn on general debugging messages. (开启一般调试消息。)
	   0x02		Turn on receive debugging messages. (开启接收调试消息。)
	   0x04		Turn on transmit debugging messages. (开启发送调试消息。)
	   0x08		Turn on list debugging messages. (开启链表调试消息。)
	   ====		=====================================

	2. You can append aui=1 to the end of the insmod line to cause
	   the adapter to use the AUI interface instead of the 10 Base T
	   interface.  This is also what to do if you want to use the BNC
	   connector on a TLAN based device.  (Setting this option on a
	   device that does not have an AUI/BNC connector will probably
	   cause it to not function correctly.)
	   (可以在 insmod 命令行末尾附加 aui=1，使适配器使用 AUI 接口而不是
	   10 Base T 接口。如果想使用基于 TLAN 的设备上的 BNC 连接器，也应这样
	   设置。(在没有 AUI/BNC 连接器的设备上设置此选项，很可能导致其无法
	   正常工作。))

	3. You can set duplex=1 to force half duplex, and duplex=2 to
	   force full duplex.
	   (可以设置 duplex=1 强制半双工，设置 duplex=2 强制全双工。)

	4. You can set speed=10 to force 10Mbs operation, and speed=100
	   to force 100Mbs operation. (I'm not sure what will happen
	   if a card which only supports 10Mbs is forced into 100Mbs
	   mode.)
	   (可以设置 speed=10 强制 10Mbit/s 速率，设置 speed=100 强制
	   100Mbit/s 速率。(我不确定把只支持 10Mbit/s 的网卡强制到 100Mbit/s
	   模式下会发生什么。))

	5. You have to use speed=X duplex=Y together now. If you just
	   do "insmod tlan.o speed=100" the driver will do Auto-Neg.
	   To force a 10Mbps Half-Duplex link do "insmod tlan.o speed=10
	   duplex=1".
	   (现在必须把 speed=X 与 duplex=Y 配合使用。如果只执行
	   "insmod tlan.o speed=100"，驱动会进行自动协商。要强制 10Mbps 半双工
	   链路，请执行 "insmod tlan.o speed=10 duplex=1"。)

	6. If the driver is built into the kernel, you can use the 3rd
	   and 4th parameters to set aui and debug respectively.  For
	   example::
	   (如果驱动编译进内核，可以使用第 3 和第 4 个参数分别设置 aui 和
	   debug。例如：)

		ether=0,0,0x1,0x7,eth0

	   This sets aui to 0x1 and debug to 0x7, assuming eth0 is a
	   supported TLAN device.
	   (这会把 aui 设为 0x1、debug 设为 0x7，前提是 eth0 是受支持的 TLAN
	   设备。)

	   The bits in the third byte are assigned as follows:
	   (第三个字节中各个位的含义如下：)

		====   ===============
		0x01   aui (AUI 接口)
		0x02   use half duplex (使用半双工)
		0x04   use full duplex (使用全双工)
		0x08   use 10BaseT (使用 10BaseT)
		0x10   use 100BaseTx (使用 100BaseTx)
		====   ===============

	   You also need to set both speed and duplex settings when forcing
	   speeds with kernel-parameters.
	   ether=0,0,0x12,0,eth0 will force link to 100Mbps Half-Duplex.
	   (用内核参数强制速率时，同样需要同时设置 speed 和 duplex。
	   ether=0,0,0x12,0,eth0 会强制链路为 100Mbps 半双工。)

	7. If you have more than one tlan adapter in your system, you can
	   use the above options on a per adapter basis. To force a 100Mbit/HD
	   link with your eth1 adapter use::
	   (如果系统中有多个 tlan 适配器，可以按适配器分别使用上述选项。要为
	   eth1 适配器强制 100Mbit/半双工链路，可使用：)

		insmod tlan speed=0,100 duplex=0,1

	   Now eth0 will use auto-neg and eth1 will be forced to 100Mbit/HD.
	   Note that the tlan driver supports a maximum of 8 adapters.
	   (这样 eth0 将使用自动协商，而 eth1 将被强制为 100Mbit/半双工。注意
	   tlan 驱动最多支持 8 个适配器。)


III. Things to try if you have problems
(III. 遇到问题时可以尝试的方法)
=======================================

	1. Make sure your card's PCI id is among those listed in
	   section I, above.
	   (确保你网卡的 PCI ID 位于上文第 I 节所列的清单之中。)
	2. Make sure routing is correct.
	   (确保路由设置正确。)
	3. Try forcing different speed/duplex settings
	   (尝试强制使用不同的速率/双工设置)


There is also a tlan mailing list which you can join by sending "subscribe tlan"
in the body of an email to majordomo@vuser.vu.union.edu.
(还有一个 tlan 邮件列表，向 majordomo@vuser.vu.union.edu 发送一封正文为
"subscribe tlan" 的邮件即可加入。)

There is also a tlan website at http://www.compaq.com
(还有一个 tlan 网站，位于 http://www.compaq.com)
