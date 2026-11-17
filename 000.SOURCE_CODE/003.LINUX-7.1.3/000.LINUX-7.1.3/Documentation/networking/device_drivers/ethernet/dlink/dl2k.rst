.. SPDX-License-Identifier: GPL-2.0

=========================================================
D-Link DL2000-based Gigabit Ethernet Adapter Installation
=========================================================

(基于 D-Link DL2000 的千兆以太网适配器安装指南)

May 23, 2002
(2002 年 5 月 23 日)

.. Contents

 - Compatibility List
 - Quick Install
 - Compiling the Driver
 - Installing the Driver
 - Option parameter
 - Configuration Script Sample
 - Troubleshooting


Compatibility List
(兼容性列表)
==================

Adapter Support:
(支持的适配器：)

- D-Link DGE-550T Gigabit Ethernet Adapter.
  (D-Link DGE-550T 千兆以太网适配器。)
- D-Link DGE-550SX Gigabit Ethernet Adapter.
  (D-Link DGE-550SX 千兆以太网适配器。)
- D-Link DL2000-based Gigabit Ethernet Adapter.
  (基于 D-Link DL2000 的千兆以太网适配器。)


The driver support Linux kernel 2.4.7 later. We had tested it
on the environments below.
(该驱动程序支持 Linux 2.4.7 及以后的内核。我们已在以下环境中进行过
测试。)

 . Red Hat v6.2 (update kernel to 2.4.7)
 . Red Hat v7.0 (update kernel to 2.4.7)
 . Red Hat v7.1 (kernel 2.4.7)
 . Red Hat v7.2 (kernel 2.4.7-10)


Quick Install
(快速安装)
=============
Install linux driver as following command::
(按以下命令安装 Linux 驱动程序：)

    1. make all
    2. insmod dl2k.ko
    3. ifconfig eth0 up 10.xxx.xxx.xxx netmask 255.0.0.0
			^^^^^^^^^^^^^^^\	    ^^^^^^^^\
					IP		     NETMASK

Now eth0 should active, you can test it by "ping" or get more information by
"ifconfig". If tested ok, continue the next step.
(现在 eth0 应该已经激活，您可以用 "ping" 测试它，或用 "ifconfig" 获取
更多信息。如果测试通过，请继续下一步。)

4. ``cp dl2k.ko /lib/modules/`uname -r`/kernel/drivers/net``
   (4. ``cp dl2k.ko /lib/modules/`uname -r`/kernel/drivers/net``)
5. Add the following line to /etc/modprobe.d/dl2k.conf::
   (在 /etc/modprobe.d/dl2k.conf 中加入以下一行：)

	alias eth0 dl2k

6. Run ``depmod`` to updated module indexes.
   (6. 运行 ``depmod`` 更新模块索引。)
7. Run ``netconfig`` or ``netconf`` to create configuration script ifcfg-eth0
   located at /etc/sysconfig/network-scripts or create it manually.
   (7. 运行 ``netconfig`` 或 ``netconf``，在 /etc/sysconfig/network-scripts
   下创建配置脚本 ifcfg-eth0，或者手动创建。)

   [see - Configuration Script Sample]
8. Driver will automatically load and configure at next boot time.
   (8. 下次启动时驱动程序将自动加载并完成配置。)

Compiling the Driver
(编译驱动程序)
====================
In Linux, NIC drivers are most commonly configured as loadable modules.
The approach of building a monolithic kernel has become obsolete. The driver
can be compiled as part of a monolithic kernel, but is strongly discouraged.
The remainder of this section assumes the driver is built as a loadable module.
In the Linux environment, it is a good idea to rebuild the driver from the
source instead of relying on a precompiled version. This approach provides
better reliability since a precompiled driver might depend on libraries or
kernel features that are not present in a given Linux installation.
(在 Linux 中，网卡驱动程序最常被配置为可加载模块。构建单一内核（monolithic
kernel）的做法已经过时。驱动程序虽可编入单一内核，但强烈不建议这样做。
本节余下内容均假定驱动程序以可加载模块的形式构建。在 Linux 环境下，最好
从源码重新构建驱动程序，而不是依赖预编译版本。这种做法可靠性更好，因为
预编译驱动程序可能依赖给定 Linux 安装中并不存在的库或内核特性。)

The 3 files necessary to build Linux device driver are dl2k.c, dl2k.h and
Makefile. To compile, the Linux installation must include the gcc compiler,
the kernel source, and the kernel headers. The Linux driver supports Linux
Kernels 2.4.7. Copy the files to a directory and enter the following command
to compile and link the driver:
(构建 Linux 设备驱动程序所需的 3 个文件是 dl2k.c、dl2k.h 和 Makefile。
要编译，Linux 安装必须包含 gcc 编译器、内核源码和内核头文件。该 Linux
驱动程序支持 Linux 2.4.7 内核。将这些文件复制到一个目录，然后输入以下
命令来编译并链接驱动程序：)

CD-ROM drive
(CD-ROM 光驱)
------------

::

    [root@XXX /] mkdir cdrom
    [root@XXX /] mount -r -t iso9660 -o conv=auto /dev/cdrom /cdrom
    [root@XXX /] cd root
    [root@XXX /root] mkdir dl2k
    [root@XXX /root] cd dl2k
    [root@XXX dl2k] cp /cdrom/linux/dl2k.tgz /root/dl2k
    [root@XXX dl2k] tar xfvz dl2k.tgz
    [root@XXX dl2k] make all

Floppy disc drive
(软盘驱动器)
-----------------

::

    [root@XXX /] cd root
    [root@XXX /root] mkdir dl2k
    [root@XXX /root] cd dl2k
    [root@XXX dl2k] mcopy a:/linux/dl2k.tgz /root/dl2k
    [root@XXX dl2k] tar xfvz dl2k.tgz
    [root@XXX dl2k] make all

Installing the Driver
(安装驱动程序)
=====================

Manual Installation
(手动安装)
-------------------

  Once the driver has been compiled, it must be loaded, enabled, and bound
  to a protocol stack in order to establish network connectivity. To load a
  module enter the command::
  (驱动程序编译完成后，必须将其加载、启用并绑定到协议栈，才能建立网络
  连接。要加载模块，请输入命令：)

    insmod dl2k.o

  or::
  (或：)

    insmod dl2k.o <optional parameter>	; add parameter
    (insmod dl2k.o <可选参数>	；添加参数)

---------------------------------------------------------

  example::
  (示例：)

    insmod dl2k.o media=100mbps_hd

   or::
   (或：)

    insmod dl2k.o media=3

   or::
   (或：)

    insmod dl2k.o media=3,2	; for 2 cards

---------------------------------------------------------

  Please reference the list of the command line parameters supported by
  the Linux device driver below.
  (请参阅下文 Linux 设备驱动程序所支持的命令行参数列表。)

  The insmod command only loads the driver and gives it a name of the form
  eth0, eth1, etc. To bring the NIC into an operational state,
  it is necessary to issue the following command::
  (insmod 命令只加载驱动程序，并为其赋予形如 eth0、eth1 等的名称。要使
  网卡进入工作状态，还必须执行以下命令：)

    ifconfig eth0 up

  Finally, to bind the driver to the active protocol (e.g., TCP/IP with
  Linux), enter the following command::
  (最后，要将驱动程序绑定到活动协议（例如 Linux 下的 TCP/IP），请输入
  以下命令：)

    ifup eth0

  Note that this is meaningful only if the system can find a configuration
  script that contains the necessary network information. A sample will be
  given in the next paragraph.
  (注意：只有当系统能找到包含必要网络信息的配置脚本时，该命令才有意义。
  下文将给出一个示例。)

  The commands to unload a driver are as follows::
  (卸载驱动程序的命令如下：)

    ifdown eth0
    ifconfig eth0 down
    rmmod dl2k.o

  The following are the commands to list the currently loaded modules and
  to see the current network configuration::
  (以下是列出当前已加载模块以及查看当前网络配置的命令：)

    lsmod
    ifconfig


Automated Installation
(自动安装)
----------------------

  This section describes how to install the driver such that it is
  automatically loaded and configured at boot time. The following description
  is based on a Red Hat 6.0/7.0 distribution, but it can easily be ported to
  other distributions as well.
  (本节描述如何安装驱动程序，使其在启动时自动加载并完成配置。以下描述
  基于 Red Hat 6.0/7.0 发行版，但同样可以轻松移植到其他发行版。)

Red Hat v6.x/v7.x
(Red Hat v6.x/v7.x)
-----------------

  1. Copy dl2k.o to the network modules directory, typically
     /lib/modules/2.x.x-xx/net or /lib/modules/2.x.x/kernel/drivers/net.
     (1. 将 dl2k.o 复制到网络模块目录，通常为
     /lib/modules/2.x.x-xx/net 或 /lib/modules/2.x.x/kernel/drivers/net。)
  2. Locate the boot module configuration file, most commonly in the
     /etc/modprobe.d/ directory. Add the following lines::
     (2. 找到启动时的模块配置文件，最常见于 /etc/modprobe.d/ 目录。加入
     以下几行：)

	alias ethx dl2k
	options dl2k <optional parameters>

     where ethx will be eth0 if the NIC is the only ethernet adapter, eth1 if
     one other ethernet adapter is installed, etc. Refer to the table in the
     previous section for the list of optional parameters.
     (其中，如果该网卡是唯一的以太网适配器，ethx 将是 eth0；如果还安装了
     另一个以太网适配器，则为 eth1，依此类推。可选参数列表请参阅上一节的
     表格。)
  3. Locate the network configuration scripts, normally the
     /etc/sysconfig/network-scripts directory, and create a configuration
     script named ifcfg-ethx that contains network information.
     (3. 找到网络配置脚本所在位置（通常是 /etc/sysconfig/network-scripts
     目录），并创建一个包含网络信息的配置脚本 ifcfg-ethx。)
  4. Note that for most Linux distributions, Red Hat included, a configuration
     utility with a graphical user interface is provided to perform steps 2
     and 3 above.
     (4. 注意，对大多数 Linux 发行版（包括 Red Hat）而言，系统都提供了
     带图形用户界面的配置工具来执行上述第 2、3 步。)


Parameter Description
=====================
You can install this driver without any additional parameter. However, if you
are going to have extensive functions then it is necessary to set extra
parameter. Below is a list of the command line parameters supported by the
Linux device
driver.
(您可以不带任何额外参数安装此驱动程序。但如果要使用其全部功能，则有必要
设置额外参数。下面是 Linux 设备驱动程序所支持的命令行参数列表。)


===============================   ==============================================
mtu=packet_size			  Specifies the maximum packet size. default
				  is 1500. (指定最大数据包大小。默认为 1500。)

media=media_type		  Specifies the media type the NIC operates at.
				  autosense	Autosensing active media. (指定网卡工作的介质类型。autosense：自动侦测活动介质。)

				  ===========	=========================
				  10mbps_hd	10Mbps half duplex. (10Mbps 半双工)
				  10mbps_fd	10Mbps full duplex. (10Mbps 全双工)
				  100mbps_hd	100Mbps half duplex. (100Mbps 半双工)
				  100mbps_fd	100Mbps full duplex. (100Mbps 全双工)
				  1000mbps_fd	1000Mbps full duplex. (1000Mbps 全双工)
				  1000mbps_hd	1000Mbps half duplex. (1000Mbps 半双工)
				  0		Autosensing active media. (自动侦测活动介质)
				  1		10Mbps half duplex. (10Mbps 半双工)
				  2		10Mbps full duplex. (10Mbps 全双工)
				  3		100Mbps half duplex. (100Mbps 半双工)
				  4		100Mbps full duplex. (100Mbps 全双工)
				  5          	1000Mbps half duplex. (1000Mbps 半双工)
				  6          	1000Mbps full duplex. (1000Mbps 全双工)
				  ===========	=========================

				  By default, the NIC operates at autosense.
				  1000mbps_fd and 1000mbps_hd types are only
				  available for fiber adapter. (默认情况下，网卡以自动侦测方式工作。1000mbps_fd 与 1000mbps_hd 类型仅适用于光纤适配器。)

vlan=n				  Specifies the VLAN ID. If vlan=0, the
				  Virtual Local Area Network (VLAN) function is
				  disable. (指定 VLAN ID。若 vlan=0，则禁用虚拟局域网（VLAN）功能。)

jumbo=[0|1]			  Specifies the jumbo frame support. If jumbo=1,
				  the NIC accept jumbo frames. By default, this
				  function is disabled.
				  Jumbo frame usually improve the performance
				  int gigabit.
				  This feature need jumbo frame compatible
				  remote. (指定巨型帧支持。若 jumbo=1，网卡将接受巨型帧。默认情况下该功能被禁用。巨型帧通常能提升千兆速率下的性能。该特性需要远端兼容巨型帧。)

rx_coalesce=m			  Number of rx frame handled each interrupt. (每次中断处理的接收帧数。)
rx_timeout=n			  Rx DMA wait time for an interrupt.
				  If set rx_coalesce > 0, hardware only assert
				  an interrupt for m frames. Hardware won't
				  assert rx interrupt until m frames received or
				  reach timeout of n * 640 nano seconds.
				  Set proper rx_coalesce and rx_timeout can
				  reduce congestion collapse and overload which
				  has been a bottleneck for high speed network. (每次中断的 Rx DMA 等待时间。若设置 rx_coalesce > 0，硬件只会对 m 帧断言一次中断；在收到 m 帧之前或到达 n * 640 纳秒超时之前，硬件不会断言 rx 中断。合理设置 rx_coalesce 与 rx_timeout 可减少拥塞崩溃与过载，这一问题一直是高速网络的瓶颈。)

				  For example, rx_coalesce=10 rx_timeout=800.
				  that is, hardware assert only 1 interrupt
				  for 10 frames received or timeout of 512 us. (例如 rx_coalesce=10 rx_timeout=800：即对收到的 10 帧或 512 微秒超时只断言 1 次中断。)

tx_coalesce=n			  Number of tx frame handled each interrupt.
				  Set n > 1 can reduce the interrupts
				  congestion usually lower performance of
				  high speed network card. Default is 16. (每次中断处理的发送帧数。设置 n > 1 可减少中断拥塞——拥塞通常会降低高速网卡的性能。默认为 16。)

tx_flow=[1|0]			  Specifies the Tx flow control. If tx_flow=0,
				  the Tx flow control disable else driver
				  autodetect. (指定发送流控。若 tx_flow=0，则禁用发送流控，否则由驱动程序自动侦测。)
rx_flow=[1|0]			  Specifies the Rx flow control. If rx_flow=0,
				  the Rx flow control enable else driver
				  autodetect. (指定接收流控。若 rx_flow=0，则启用接收流控，否则由驱动程序自动侦测。)
===============================   ==============================================


Configuration Script Sample
(配置脚本示例)
===========================
Here is a sample of a simple configuration script::
(下面是一个简单配置脚本的示例：)

    DEVICE=eth0
    USERCTL=no
    ONBOOT=yes
    POOTPROTO=none
    BROADCAST=207.200.5.255
    NETWORK=207.200.5.0
    NETMASK=255.255.255.0
    IPADDR=207.200.5.2


Troubleshooting
(故障排除)
===============
Q1. Source files contain ^ M behind every line.
(Q1. 源文件中每行末尾都有 ^ M。)

    Make sure all files are Unix file format (no LF). Try the following
    shell command to convert files::
    (请确保所有文件都是 Unix 文件格式（无 LF）。尝试使用以下 shell 命令
    转换文件：)

	cat dl2k.c | col -b > dl2k.tmp
	mv dl2k.tmp dl2k.c

    OR::
    (或者：)

	cat dl2k.c | tr -d "\r" > dl2k.tmp
	mv dl2k.tmp dl2k.c

Q2: Could not find header files (``*.h``)?
(Q2：找不到头文件（``*.h``）？)

    To compile the driver, you need kernel header files. After
    installing the kernel source, the header files are usually located in
    /usr/src/linux/include, which is the default include directory configured
    in Makefile. For some distributions, there is a copy of header files in
    /usr/src/include/linux and /usr/src/include/asm, that you can change the
    INCLUDEDIR in Makefile to /usr/include without installing kernel source.
    (要编译驱动程序，您需要内核头文件。安装内核源码后，头文件通常位于
    /usr/src/linux/include，这是 Makefile 中配置的默认包含目录。对于某些
    发行版，/usr/src/include/linux 和 /usr/src/include/asm 中有头文件的
    副本，此时无需安装内核源码，直接把 Makefile 中的 INCLUDEDIR 改为
    /usr/include 即可。)

    Note that RH 7.0 didn't provide correct header files in /usr/include,
    including those files will make a wrong version driver.
    (注意：RH 7.0 在 /usr/include 中提供的头文件并不正确，包含这些文件会
    编译出错误版本的驱动程序。)

