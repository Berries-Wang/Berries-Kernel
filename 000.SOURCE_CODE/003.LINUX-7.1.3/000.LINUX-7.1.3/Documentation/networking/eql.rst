.. SPDX-License-Identifier: GPL-2.0

==========================================
EQL Driver: Serial IP Load Balancing HOWTO
==========================================
(EQL 驱动：串行 IP 负载均衡 HOWTO)

  Simon "Guru Aleph-Null" Janes, simon@ncm.com
  (作者：Simon "Guru Aleph-Null" Janes，simon@ncm.com)

  v1.1, February 27, 1995
  (v1.1 版，1995 年 2 月 27 日)

  This is the manual for the EQL device driver. EQL is a software device
  that lets you load-balance IP serial links (SLIP or uncompressed PPP)
  to increase your bandwidth. It will not reduce your latency (i.e. ping
  times) except in the case where you already have lots of traffic on
  your link, in which it will help them out. This driver has been tested
  with the 1.1.75 kernel, and is known to have patched cleanly with
  1.1.86.  Some testing with 1.1.92 has been done with the v1.1 patch
  which was only created to patch cleanly in the very latest kernel
  source trees. (Yes, it worked fine.)
  (这是 EQL 设备驱动程序的手册。EQL 是一种软件设备，让你可以对 IP 串行链路 (SLIP 或未压缩的 PPP) 进行负载均衡以提高带宽。它不会降低你的延迟 (即 ping 时间)，除非你的链路上本来就有大量流量，那种情况下它会帮上忙。该驱动程序已在 1.1.75 内核上测试过，已知在 1.1.86 上可以干净地打上补丁。还在 1.1.92 上用 v1.1 补丁做了一些测试，该补丁只是为了能在最新的内核源码树上干净地打上而创建的。(是的，它工作得很好。))

1. Introduction
(1. 简介)
===============

  Which is worse? A huge fee for a 56K leased line or two phone lines?
  It's probably the former.  If you find yourself craving more bandwidth,
  and have a ISP that is flexible, it is now possible to bind modems
  together to work as one point-to-point link to increase your
  bandwidth.  All without having to have a special black box on either
  side.
  (哪个更糟糕？一条 56K 租用线路的高昂费用，还是两条电话线？很可能是前者。如果你渴望更多带宽，而你的 ISP 又比较灵活，那么现在可以把多个调制解调器绑定在一起，作为一条点对点链路工作以提高带宽，而且两端都不需要任何特殊的黑盒子。)


  The eql driver has only been tested with the Livingston PortMaster-2e
  terminal server. I do not know if other terminal servers support load-
  balancing, but I do know that the PortMaster does it, and does it
  almost as well as the eql driver seems to do it (-- Unfortunately, in
  my testing so far, the Livingston PortMaster 2e's load-balancing is a
  good 1 to 2 KB/s slower than the test machine working with a 28.8 Kbps
  and 14.4 Kbps connection.  However, I am not sure that it really is
  the PortMaster, or if it's Linux's TCP drivers. I'm told that Linux's
  TCP implementation is pretty fast though.--)
  (eql 驱动程序仅在 Livingston PortMaster-2e 终端服务器上测试过。我不知道其他终端服务器是否支持负载均衡，但我知道 PortMaster 支持，而且做得几乎和 eql 驱动程序一样好 (-- 不幸的是，就我目前的测试而言，Livingston PortMaster 2e 的负载均衡比测试机使用 28.8 Kbps 和 14.4 Kbps 连接时慢了整整 1 到 2 KB/s。不过，我不确定问题究竟出在 PortMaster 还是 Linux 的 TCP 驱动程序上。有人告诉我 Linux 的 TCP 实现相当快。--))


  I suggest to ISPs out there that it would probably be fair to charge
  a load-balancing client 75% of the cost of the second line and 50% of
  the cost of the third line etc...
  (我建议各大 ISP：向负载均衡客户收取第二条线路费用的 75%、第三条线路费用的 50%，依此类推，可能是比较公平的做法……)


  Hey, we can all dream you know...
  (嘿，我们都可以做做梦，不是吗……)


2. Kernel Configuration
(2. 内核配置)
=======================

  Here I describe the general steps of getting a kernel up and working
  with the eql driver.	From patching, building, to installing.
  (这里我介绍让内核与 eql 驱动程序一起跑起来的大致步骤：从打补丁、构建到安装。)


2.1. Patching The Kernel
(2.1. 给内核打补丁)
------------------------

  If you do not have or cannot get a copy of the kernel with the eql
  driver folded into it, get your copy of the driver from
  ftp://slaughter.ncm.com/pub/Linux/LOAD_BALANCING/eql-1.1.tar.gz.
  Unpack this archive someplace obvious like /usr/local/src/.  It will
  create the following files::
  (如果你没有、也无法获得已合入 eql 驱动程序的内核副本，请从 ftp://slaughter.ncm.com/pub/Linux/LOAD_BALANCING/eql-1.1.tar.gz 获取驱动程序副本。把这个压缩包解压到显眼的地方，比如 /usr/local/src/。它将创建以下文件：)

       -rw-r--r-- guru/ncm	198 Jan 19 18:53 1995 eql-1.1/NO-WARRANTY
       -rw-r--r-- guru/ncm	30620 Feb 27 21:40 1995 eql-1.1/eql-1.1.patch
       -rwxr-xr-x guru/ncm	16111 Jan 12 22:29 1995 eql-1.1/eql_enslave
       -rw-r--r-- guru/ncm	2195 Jan 10 21:48 1995 eql-1.1/eql_enslave.c

  Unpack a recent kernel (something after 1.1.92) someplace convenient
  like say /usr/src/linux-1.1.92.eql. Use symbolic links to point
  /usr/src/linux to this development directory.
  (把一个较新的内核 (1.1.92 之后的版本) 解压到方便的地方，比如说 /usr/src/linux-1.1.92.eql。使用符号链接把 /usr/src/linux 指向这个开发目录。)


  Apply the patch by running the commands::
  (运行以下命令来应用补丁：)

       cd /usr/src
       patch </usr/local/src/eql-1.1/eql-1.1.patch


2.2. Building The Kernel
(2.2. 构建内核)
------------------------

  After patching the kernel, run make config and configure the kernel
  for your hardware.
  (给内核打完补丁后，运行 make config 并针对你的硬件配置内核。)


  After configuration, make and install according to your habit.
  (配置完成后，按照你的习惯进行 make 和安装。)


3. Network Configuration
(3. 网络配置)
========================

  So far, I have only used the eql device with the DSLIP SLIP connection
  manager by Matt Dillon (-- "The man who sold his soul to code so much
  so quickly."--) .  How you configure it for other "connection"
  managers is up to you.  Most other connection managers that I've seen
  don't do a very good job when it comes to handling more than one
  connection.
  (到目前为止，我只把 eql 设备与 Matt Dillon 的 DSLIP SLIP 连接管理器 (-- "那个为了这么快写完这么多代码而把灵魂出卖给代码的人。"-- ) 一起使用过。如何为其他"连接"管理器配置它由你自己决定。据我所见，其他大多数连接管理器在处理多个连接时都做得不太好。)


3.1. /etc/rc.d/rc.inet1
(3.1. /etc/rc.d/rc.inet1)
-----------------------

  In rc.inet1, ifconfig the eql device to the IP address you usually use
  for your machine, and the MTU you prefer for your SLIP lines.	One
  could argue that MTU should be roughly half the usual size for two
  modems, one-third for three, one-fourth for four, etc...  But going
  too far below 296 is probably overkill. Here is an example ifconfig
  command that sets up the eql device::
  (在 rc.inet1 中，用 ifconfig 把 eql 设备配置为你机器常用的 IP 地址，以及你为 SLIP 线路偏好的 MTU。有人可能会说，两条调制解调器时 MTU 应大约取平时的一半，三条时取三分之一，四条时取四分之一，依此类推……但低于 296 太多可能就过头了。下面是设置 eql 设备的 ifconfig 命令示例：)

       ifconfig eql 198.67.33.239 mtu 1006

  Once the eql device is up and running, add a static default route to
  it in the routing table using the cool new route syntax that makes
  life so much easier::
  (eql 设备启动并运行之后，使用让生活轻松得多的超酷新 route 语法，在路由表中为它添加一条静态默认路由：)

       route add default eql


3.2. Enslaving Devices By Hand
(3.2. 手动奴役设备)
------------------------------

  Enslaving devices by hand requires two utility programs: eql_enslave
  and eql_emancipate (-- eql_emancipate hasn't been written because when
  an enslaved device "dies", it is automatically taken out of the queue.
  I haven't found a good reason to write it yet... other than for
  completeness, but that isn't a good motivator is it?--)
  (手动奴役设备需要两个实用程序：eql_enslave 和 eql_emancipate (-- eql_emancipate 还没有编写，因为当一个被奴役的设备"死亡"时，它会自动从队列中移除。我还没找到编写它的充分理由……除了为了完整性之外，但这算不上一个好的动机，对吧？--))


  The syntax for enslaving a device is "eql_enslave <master-name>
  <slave-name> <estimated-bps>".  Here are some example enslavings::
  (奴役设备的语法是 "eql_enslave <master-name> <slave-name> <estimated-bps>"。下面是一些奴役示例：)

       eql_enslave eql sl0 28800
       eql_enslave eql ppp0 14400
       eql_enslave eql sl1 57600

  When you want to free a device from its life of slavery, you can
  either down the device with ifconfig (eql will automatically bury the
  dead slave and remove it from its queue) or use eql_emancipate to free
  it. (-- Or just ifconfig it down, and the eql driver will take it out
  for you.--)::
  (当你想让设备脱离被奴役的生活时，可以用 ifconfig 把设备置为 down (eql 会自动埋葬死亡的从属设备并将其从队列中移除)，或者使用 eql_emancipate 来释放它。(-- 或者直接用 ifconfig 把它置为 down，eql 驱动程序会替你把它移除。--)：)

       eql_emancipate eql sl0
       eql_emancipate eql ppp0
       eql_emancipate eql sl1


3.3. DSLIP Configuration for the eql Device
(3.3. eql 设备的 DSLIP 配置)
-------------------------------------------

  The general idea is to bring up and keep up as many SLIP connections
  as you need, automatically.
  (总体思路是按需自动建立并保持尽可能多的 SLIP 连接。)


3.3.1.  /etc/slip/runslip.conf
(3.3.1. /etc/slip/runslip.conf)
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

  Here is an example runslip.conf::
  (下面是一个 runslip.conf 示例：)

	  name		sl-line-1
	  enabled
	  baud		38400
	  mtu		576
	  ducmd		-e /etc/slip/dialout/cua2-288.xp -t 9
	  command	 eql_enslave eql $interface 28800
	  address	 198.67.33.239
	  line		/dev/cua2

	  name		sl-line-2
	  enabled
	  baud		38400
	  mtu		576
	  ducmd		-e /etc/slip/dialout/cua3-288.xp -t 9
	  command	 eql_enslave eql $interface 28800
	  address	 198.67.33.239
	  line		/dev/cua3


3.4. Using PPP and the eql Device
(3.4. 使用 PPP 与 eql 设备)
---------------------------------

  I have not yet done any load-balancing testing for PPP devices, mainly
  because I don't have a PPP-connection manager like SLIP has with
  DSLIP. I did find a good tip from LinuxNET:Billy for PPP performance:
  make sure you have asyncmap set to something so that control
  characters are not escaped.
  (我还没有对 PPP 设备做过任何负载均衡测试，主要因为我没有像 SLIP 拥有 DSLIP 那样的 PPP 连接管理器。我确实从 LinuxNET:Billy 那里找到一条关于 PPP 性能的好建议：确保把 asyncmap 设置为某个值，使控制字符不被转义。)


  I tried to fix up a PPP script/system for redialing lost PPP
  connections for use with the eql driver the weekend of Feb 25-26 '95
  (Hereafter known as the 8-hour PPP Hate Festival).  Perhaps later this
  year.
  (在 1995 年 2 月 25-26 日的周末，我尝试整理了一套用于重拨丢失 PPP 连接的 PPP 脚本/系统，以配合 eql 驱动程序使用 (此后被称为"8 小时 PPP 讨厌节")。也许今年晚些时候能完成。)


4. About the Slave Scheduler Algorithm
(4. 关于从属设备调度器算法)
======================================

  The slave scheduler probably could be replaced with a dozen other
  things and push traffic much faster.	The formula in the current set
  up of the driver was tuned to handle slaves with wildly different
  bits-per-second "priorities".
  (从属设备调度器大概可以用十几种其他实现来替换，从而更快地推送流量。当前驱动程序设置中的公式是针对处理比特率"优先级"差异巨大的从属设备而调优的。)


  All testing I have done was with two 28.8 V.FC modems, one connecting
  at 28800 bps or slower, and the other connecting at 14400 bps all the
  time.
  (我所做的全部测试都使用两个 28.8 V.FC 调制解调器：一个以 28800 bps 或更低的速率连接，另一个始终以 14400 bps 连接。)


  One version of the scheduler was able to push 5.3 K/s through the
  28800 and 14400 connections, but when the priorities on the links were
  very wide apart (57600 vs. 14400) the "faster" modem received all
  traffic and the "slower" modem starved.
  (调度器的一个版本能够通过 28800 和 14400 的连接推送 5.3 K/s，但当链路上的优先级相差悬殊 (57600 对 14400) 时，"较快"的调制解调器接收了全部流量，而"较慢"的调制解调器则被饿死。)


5. Testers' Reports
(5. 测试者的报告)
===================

  Some people have experimented with the eql device with newer
  kernels (than 1.1.75).  I have since updated the driver to patch
  cleanly in newer kernels because of the removal of the old "slave-
  balancing" driver config option.
  (有些人已经用比 1.1.75 更新的内核对 eql 设备做了试验。由于旧的"从属均衡 (slave-balancing)"驱动程序配置选项被移除，我后来更新了驱动程序，使其能在较新的内核上干净地打上补丁。)


  -  icee from LinuxNET patched 1.1.86 without any rejects and was able
     to boot the kernel and enslave a couple of ISDN PPP links.
     (来自 LinuxNET 的 icee 给 1.1.86 打补丁时没有任何拒绝 (reject)，并能启动内核、奴役几条 ISDN PPP 链路。)

5.1. Randolph Bentson's Test Report
(5.1. Randolph Bentson 的测试报告)
-----------------------------------

  ::

    From bentson@grieg.seaslug.org Wed Feb  8 19:08:09 1995
    Date: Tue, 7 Feb 95 22:57 PST
    From: Randolph Bentson <bentson@grieg.seaslug.org>
    To: guru@ncm.com
    Subject: EQL driver tests


    I have been checking out your eql driver.  (Nice work, that!)
    Although you may already done this performance testing, here
    are some data I've discovered.

    Randolph Bentson
    bentson@grieg.seaslug.org

------------------------------------------------------------------


  A pseudo-device driver, EQL, written by Simon Janes, can be used
  to bundle multiple SLIP connections into what appears to be a
  single connection.  This allows one to improve dial-up network
  connectivity gradually, without having to buy expensive DSU/CSU
  hardware and services.
  (由 Simon Janes 编写的伪设备驱动程序 EQL 可用于把多条 SLIP 连接捆绑成一条表面上的单一连接。这让人们可以逐步改善拨号网络连接，而不必购买昂贵的 DSU/CSU 硬件和服务。)

  I have done some testing of this software, with two goals in
  mind: first, to ensure it actually works as described and
  second, as a method of exercising my device driver.
  (我对这个软件做了一些测试，有两个目的：第一，确认它确实像描述的那样工作；第二，作为锻炼我自己设备驱动程序的一种方法。)

  The following performance measurements were derived from a set
  of SLIP connections run between two Linux systems (1.1.84) using
  a 486DX2/66 with a Cyclom-8Ys and a 486SLC/40 with a Cyclom-16Y.
  (Ports 0,1,2,3 were used.  A later configuration will distribute
  port selection across the different Cirrus chips on the boards.)
  Once a link was established, I timed a binary ftp transfer of
  289284 bytes of data.	If there were no overhead (packet headers,
  inter-character and inter-packet delays, etc.) the transfers
  would take the following times::
  (以下性能测量结果来自在两台 Linux 系统 (1.1.84) 之间运行的一组 SLIP 连接：一台是配备 Cyclom-8Ys 的 486DX2/66，另一台是配备 Cyclom-16Y 的 486SLC/40。(使用了端口 0、1、2、3。后续配置将把端口选择分布到板卡上不同的 Cirrus 芯片上。) 链路建立之后，我对一次 289284 字节的二进制 ftp 传输进行了计时。如果没有任何开销 (数据包头、字符间与包间延迟等)，传输将花费以下时间：)

      bits/sec	seconds
      345600	8.3
      234600	12.3
      172800	16.7
      153600	18.8
      76800	37.6
      57600	50.2
      38400	75.3
      28800	100.4
      19200	150.6
      9600	301.3

  A single line running at the lower speeds and with large packets
  comes to within 2% of this.  Performance is limited for the higher
  speeds (as predicted by the Cirrus databook) to an aggregate of
  about 160 kbits/sec.	The next round of testing will distribute
  the load across two or more Cirrus chips.
  (单条线路在较低速度下使用大数据包时，结果与此相差在 2% 以内。较高速度下的性能受限 (正如 Cirrus 数据手册所预言)，聚合速度约为 160 kbits/sec。下一轮测试将把负载分布到两片或更多 Cirrus 芯片上。)

  The good news is that one gets nearly the full advantage of the
  second, third, and fourth line's bandwidth.  (The bad news is
  that the connection establishment seemed fragile for the higher
  speeds.  Once established, the connection seemed robust enough.)
  (好消息是，人们几乎可以完全利用第二条、第三条和第四条线路的带宽。(坏消息是，在较高速度下，连接的建立似乎很脆弱。不过一旦建立，连接看起来足够稳健。))

  ======  ========	===  ========   ======= ======= ===
  #lines  speed		mtu  seconds	theory  actual  %of
	  kbit/sec	     duration	speed	speed	max
  ======  ========	===  ========   ======= ======= ===
  3	  115200	900	_	345600
  3	  115200	400	18.1	345600  159825  46
  2	  115200	900	_	230400
  2	  115200	600	18.1	230400  159825  69
  2	  115200	400	19.3	230400  149888  65
  4	  57600		900	_	234600
  4	  57600		600	_	234600
  4	  57600		400	_	234600
  3	  57600		600	20.9	172800  138413  80
  3	  57600		900	21.2	172800  136455  78
  3	  115200	600	21.7	345600  133311  38
  3	  57600		400	22.5	172800  128571  74
  4	  38400		900	25.2	153600  114795  74
  4	  38400		600	26.4	153600  109577  71
  4	  38400		400	27.3	153600  105965  68
  2	  57600		900	29.1	115200  99410.3 86
  1	  115200	900	30.7	115200  94229.3 81
  2	  57600		600	30.2	115200  95789.4 83
  3	  38400		900	30.3	115200  95473.3 82
  3	  38400		600	31.2	115200  92719.2 80
  1	  115200	600	31.3	115200  92423	80
  2	  57600		400	32.3	115200  89561.6 77
  1	  115200	400	32.8	115200  88196.3 76
  3	  38400		400	33.5	115200  86353.4 74
  2	  38400		900	43.7	76800	66197.7 86
  2	  38400		600	44	76800	65746.4 85
  2	  38400		400	47.2	76800	61289	79
  4	  19200		900	50.8	76800	56945.7 74
  4	  19200		400	53.2	76800	54376.7 70
  4	  19200		600	53.7	76800	53870.4 70
  1	  57600		900	54.6	57600	52982.4 91
  1	  57600		600	56.2	57600	51474	89
  3	  19200		900	60.5	57600	47815.5 83
  1	  57600		400	60.2	57600	48053.8 83
  3	  19200		600	62	57600	46658.7 81
  3	  19200		400	64.7	57600	44711.6 77
  1	  38400		900	79.4	38400	36433.8 94
  1	  38400		600	82.4	38400	35107.3 91
  2	  19200		900	84.4	38400	34275.4 89
  1	  38400		400	86.8	38400	33327.6 86
  2	  19200		600	87.6	38400	33023.3 85
  2	  19200		400	91.2	38400	31719.7 82
  4	  9600		900	94.7	38400	30547.4 79
  4	  9600		400	106	38400	27290.9 71
  4	  9600		600	110	38400	26298.5 68
  3	  9600		900	118	28800	24515.6 85
  3	  9600		600	120	28800	24107	83
  3	  9600		400	131	28800	22082.7 76
  1	  19200		900	155	19200	18663.5 97
  1	  19200		600	161	19200	17968	93
  1	  19200		400	170	19200	17016.7 88
  2	  9600		600	176	19200	16436.6 85
  2	  9600		900	180	19200	16071.3 83
  2	  9600		400	181	19200	15982.5 83
  1	  9600		900	305	9600	9484.72 98
  1	  9600		600	314	9600	9212.87 95
  1	  9600		400	332	9600	8713.37 90
  ======  ========	===  ========   ======= ======= ===

  (上表各列从左到右依次为：线路数 (#lines)、标称速度 (speed，kbit/sec)、MTU、持续秒数 (seconds duration)、理论速度 (theory speed)、实际速度 (actual speed)，以及实际速度达到理论速度的百分比 (%of max)。表中 "_" 表示该格未测得数据。)

5.2. Anthony Healy's Report
(5.2. Antony Healy 的报告)
---------------------------

  ::

    Date: Mon, 13 Feb 1995 16:17:29 +1100 (EST)
    From: Antony Healey <ahealey@st.nepean.uws.edu.au>
    To: Simon Janes <guru@ncm.com>
    Subject: Re: Load Balancing

    Hi Simon,
	  I've installed your patch and it works great. I have trialed
	  it over twin SL/IP lines, just over null modems, but I was
	  able to data at over 48Kb/s [ISDN link -Simon]. I managed a
	  transfer of up to 7.5 Kbyte/s on one go, but averaged around
	  6.4 Kbyte/s, which I think is pretty cool.  :)
