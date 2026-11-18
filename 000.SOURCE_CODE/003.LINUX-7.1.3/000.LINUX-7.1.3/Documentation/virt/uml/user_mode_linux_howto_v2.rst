.. SPDX-License-Identifier: GPL-2.0

#######################
UML HowTo(UML 使用指南)
#######################

.. contents:: :local:

******************
Introduction(引言)
******************

Welcome to User Mode Linux(欢迎来到 User Mode Linux。)

User Mode Linux is the first Open Source virtualization platform (first
release date 1991) and second virtualization platform for an x86 PC.
(User Mode Linux 是第一个开源虚拟化平台（首次发布于 1991 年），也是 x86 PC 上的第二个虚拟化平台。)

How is UML Different from a VM using Virtualization package X?(与使用虚拟化软件包 X 的虚拟机相比，UML 有何不同？)
=================================================================================================================

We have come to assume that virtualization also means some level of
hardware emulation. In fact, it does not. As long as a virtualization
package provides the OS with devices which the OS can recognize and
has a driver for, the devices do not need to emulate real hardware.
Most OSes today have built-in support for a number of "fake"
devices used only under virtualization.
User Mode Linux takes this concept to the ultimate extreme - there
is not a single real device in sight. It is 100% artificial or if
we use the correct term 100% paravirtual. All UML devices are abstract
concepts which map onto something provided by the host - files, sockets,
pipes, etc.
(我们习惯于认为虚拟化还意味着某种程度的硬件模拟。事实上并非如此。只要虚拟化软件包向操作系统提供了操作系统能够识别、并且已有驱动程序的设备，这些设备就不需要模拟真实硬件。如今大多数操作系统都内置支持多种仅在虚拟化环境下使用的"假"设备。
User Mode Linux 把这一概念发挥到了极致——视野中找不到任何一台真实设备。它是 100% 人造的，或者用准确的术语说，是 100% 半虚拟化（paravirtual）的。所有 UML 设备都是抽象概念，映射到宿主机提供的某种东西——文件、套接字、管道等。)

The other major difference between UML and various virtualization
packages is that there is a distinct difference between the way the UML
kernel and the UML programs operate.
The UML kernel is just a process running on Linux - same as any other
program. It can be run by an unprivileged user and it does not require
anything in terms of special CPU features.
The UML userspace, however, is a bit different. The Linux kernel on the
host machine assists UML in intercepting everything the program running
on a UML instance is trying to do and making the UML kernel handle all
of its requests.
This is different from other virtualization packages which do not make any
difference between the guest kernel and guest programs. This difference
results in a number of advantages and disadvantages of UML over let's say
QEMU which we will cover later in this document.
(UML 与各种虚拟化软件包之间的另一个主要区别是：UML 内核与 UML 程序的运行方式有着明显差异。
UML 内核只是一个运行在 Linux 上的进程，与任何其他程序无异。它可以由非特权用户运行，并且不需要任何特殊 CPU 特性。
然而 UML 的用户空间则略有不同。宿主机上的 Linux 内核会协助 UML 截获运行在 UML 实例上的程序试图执行的一切操作，并让 UML 内核处理它的全部请求。
这与其他虚拟化软件包不同，后者并不区分客户机内核和客户机程序。这一差异使 UML 相比例如 QEMU 具有一系列优缺点，我们将在本文后面介绍。)


Why Would I Want User Mode Linux?(为什么需要 User Mode Linux？)
===============================================================


* If User Mode Linux kernel crashes, your host kernel is still fine. It
  is not accelerated in any way (vhost, kvm, etc) and it is not trying to
  access any devices directly.  It is, in fact, a process like any other.
  (如果 User Mode Linux 内核崩溃，你的宿主机内核仍然安然无恙。它没有以任何方式加速（vhost、kvm 等），也不会试图直接访问任何设备。实际上，它就是一个与其他进程无异的普通进程。)

* You can run a usermode kernel as a non-root user (you may need to
  arrange appropriate permissions for some devices).
  (可以以非 root 用户身份运行用户态内核（可能需要为某些设备安排适当的权限）。)

* You can run a very small VM with a minimal footprint for a specific
  task (for example 32M or less).
  (可以针对特定任务运行一个占用资源极小的非常小的虚拟机（例如 32M 或更少）。)

* You can get extremely high performance for anything which is a "kernel
  specific task" such as forwarding, firewalling, etc while still being
  isolated from the host kernel.
  (对于任何"内核特定任务"（例如转发、防火墙等），都可以获得极高的性能，同时仍与宿主机内核保持隔离。)

* You can play with kernel concepts without breaking things.
  (可以在不把东西搞坏的情况下尝试各种内核概念。)

* You are not bound by "emulating" hardware, so you can try weird and
  wonderful concepts which are very difficult to support when emulating
  real hardware such as time travel and making your system clock
  dependent on what UML does (very useful for things like tests).
  (不受"模拟"硬件的束缚，因此可以尝试各种奇特而精妙的想法——这些想法在模拟真实硬件时非常难以支持，例如时间旅行（time travel），以及让系统时钟取决于 UML 的行为（对测试之类的事情非常有用）。)

* It's fun.
  (它很有趣。)

Why not to run UML(为什么不运行 UML)
====================================

* The syscall interception technique used by UML makes it inherently
  slower for any userspace applications. While it can do kernel tasks
  on par with most other virtualization packages, its userspace is
  **slow**. The root cause is that UML has a very high cost of creating
  new processes and threads (something most Unix/Linux applications
  take for granted).
  (UML 所采用的系统调用截获技术，使它在运行任何用户态应用程序时都天生偏慢。虽然它执行内核任务的能力不亚于大多数其他虚拟化软件包，但它的用户空间却非常慢。根本原因是 UML 创建新进程和线程的代价极高（而大多数 Unix/Linux 应用程序把这一点视为理所当然）。)

* UML is strictly uniprocessor at present. If you want to run an
  application which needs many CPUs to function, it is clearly the
  wrong choice.
  (UML 目前严格来说只支持单处理器。如果你想运行需要多个 CPU 才能工作的应用程序，它显然是错误的选择。)

**************************************
Building a UML instance(构建 UML 实例)
**************************************

There is no UML installer in any distribution. While you can use off
the shelf install media to install into a blank VM using a virtualization
package, there is no UML equivalent. You have to use appropriate tools on
your host to build a viable filesystem image.
(任何发行版中都没有 UML 安装器。虽然使用虚拟化软件包时，可以用现成的安装介质安装到空白虚拟机中，但 UML 没有对应的东西。你必须使用宿主机上的适当工具来构建一个可用的文件系统镜像。)

This is extremely easy on Debian - you can do it using debootstrap. It is
also easy on OpenWRT - the build process can build UML images. All other
distros - YMMV.
(这在 Debian 上极其简单——你可以用 debootstrap 来完成。在 OpenWRT 上也很容易——其构建过程可以构建 UML 镜像。至于其他发行版——请自行斟酌（YMMV）。)

Creating an image(创建镜像)
===========================

Create a sparse raw disk image::

   # dd if=/dev/zero of=disk_image_name bs=1 count=1 seek=16G

(创建一个稀疏的裸磁盘镜像，命令如下。)

This will create a 16G disk image. The OS will initially allocate only one
block and will allocate more as they are written by UML. As of kernel
version 4.19 UML fully supports TRIM (as usually used by flash drives).
Using TRIM inside the UML image by specifying discard as a mount option
or by running ``tune2fs -o discard /dev/ubdXX`` will request UML to
return any unused blocks to the OS.
(这会创建一个 16G 的磁盘镜像。操作系统最初只分配一个块，之后随着 UML 的写入而分配更多。从内核版本 4.19 开始，UML 完全支持 TRIM（通常用于闪存设备的那种）。在 UML 镜像内使用 TRIM——通过将 discard 指定为挂载选项，或运行 tune2fs -o discard /dev/ubdXX——会让 UML 把所有未使用的块归还给操作系统。)

Create a filesystem on the disk image and mount it::

   # mkfs.ext4 ./disk_image_name && mount ./disk_image_name /mnt

(在磁盘镜像上创建文件系统并挂载它。)

This example uses ext4, any other filesystem such as ext3, btrfs, xfs,
jfs, etc will work too.
(本例使用 ext4，任何其他文件系统（如 ext3、btrfs、xfs、jfs 等）也同样可行。)

Create a minimal OS installation on the mounted filesystem::

   # debootstrap buster /mnt http://deb.debian.org/debian

(在已挂载的文件系统上创建一个最小的操作系统安装。)

debootstrap does not set up the root password, fstab, hostname or
anything related to networking. It is up to the user to do that.
(debootstrap 不会设置 root 密码、fstab、主机名或任何与网络相关的内容。这些都要靠用户自己完成。)

Set the root password - the easiest way to do that is to chroot into the
mounted image::

   # chroot /mnt
   # passwd
   # exit

(设置 root 密码——最简单的方法是 chroot 进入已挂载的镜像。)

Edit key system files(编辑关键系统文件)
=======================================

UML block devices are called ubds. The fstab created by debootstrap
will be empty and it needs an entry for the root file system::

   /dev/ubd0   ext4    discard,errors=remount-ro  0       1

(UML 块设备称为 ubd。debootstrap 创建的 fstab 是空的，需要为根文件系统添加一个条目。)

The image hostname will be set to the same as the host on which you
are creating its image. It is a good idea to change that to avoid
"Oh, bummer, I rebooted the wrong machine".
(镜像的主机名会被设置为与你创建镜像时所在宿主机相同的主机名。建议把它改掉，以免发生"糟糕，我重启错机器了"的情况。)

UML supports vector I/O high performance network devices which have
support for some standard virtual network encapsulations like
Ethernet over GRE and Ethernet over L2TPv3. These are called vecX.
(UML 支持 vector I/O 高性能网络设备，它们支持一些标准的虚拟网络封装，例如 Ethernet over GRE 和 Ethernet over L2TPv3。这些设备称为 vecX。)

When vector network devices are in use, ``/etc/network/interfaces``
will need entries like::

   # vector UML network devices
   auto vec0
   iface vec0 inet dhcp

(使用 vector 网络设备时，/etc/network/interfaces 中需要类似如下的条目。)

We now have a UML image which is nearly ready to run, all we need is a
UML kernel and modules for it.

Most distributions have a UML package. Even if you intend to use your own
kernel, testing the image with a stock one is always a good start. These
packages come with a set of modules which should be copied to the target
filesystem. The location is distribution dependent. For Debian these
reside under /usr/lib/uml/modules. Copy recursively the content of this
directory to the mounted UML filesystem::

   # cp -rax /usr/lib/uml/modules /mnt/lib/modules

(现在我们已经有了一个几乎可以运行的 UML 镜像，还差 UML 内核及其模块。
大多数发行版都有 UML 软件包。即使你打算使用自己的内核，先用发行版自带的内核测试镜像也总是一个好的开始。这些软件包附带一组模块，应当把它们复制到目标文件系统中。具体位置因发行版而异。在 Debian 上它们位于 /usr/lib/uml/modules 下。请把这个目录的内容递归复制到已挂载的 UML 文件系统。)

If you have compiled your own kernel, you need to use the usual "install
modules to a location" procedure by running::

  # make INSTALL_MOD_PATH=/mnt/lib/modules modules_install

(如果你编译了自己的内核，需要按常规的"把模块安装到某个位置"流程操作，即运行以下命令。)

This will install modules into /mnt/lib/modules/$(KERNELRELEASE).
To specify the full module installation path, use::

  # make MODLIB=/mnt/lib/modules modules_install

(这会把模块安装到 /mnt/lib/modules/$(KERNELRELEASE)。要指定完整的模块安装路径，请使用以下命令。)

At this point the image is ready to be brought up.
(到这里，镜像已经可以启动了。)

****************************************
Setting Up UML Networking(设置 UML 网络)
****************************************

UML networking is designed to emulate an Ethernet connection. This
connection may be either point-to-point (similar to a connection
between machines using a back-to-back cable) or a connection to a
switch. UML supports a wide variety of means to build these
connections to all of: local machine, remote machine(s), local and
remote UML and other VM instances.
(UML 网络的设计目标是模拟一条以太网连接。这条连接既可以是点对点的（类似于用交叉网线直连两台机器），也可以是到交换机的连接。UML 支持多种多样的手段来建立这些连接，对象涵盖：本地机器、远程机器、本地与远程的 UML 实例以及其他虚拟机实例。)


+-----------+--------+------------------------------------+------------+
| Transport |  Type  |        Capabilities                | Throughput |
|  (传输)   | (类型) |            (能力)                  |  (吞吐量)  |
+===========+========+====================================+============+
| tap       | vector | checksum, tso                      | > 8Gbit    |
+-----------+--------+------------------------------------+------------+
| hybrid    | vector | checksum, tso, multipacket rx      | > 6GBit    |
+-----------+--------+------------------------------------+------------+
| raw       | vector | checksum, tso, multipacket rx, tx" | > 6GBit    |
+-----------+--------+------------------------------------+------------+
| EoGRE     | vector | multipacket rx, tx                 | > 3Gbit    |
+-----------+--------+------------------------------------+------------+
| Eol2tpv3  | vector | multipacket rx, tx                 | > 3Gbit    |
+-----------+--------+------------------------------------+------------+
| bess      | vector | multipacket rx, tx                 | > 3Gbit    |
+-----------+--------+------------------------------------+------------+
| fd        | vector | dependent on fd type               | varies     |
+-----------+--------+------------------------------------+------------+
| vde       | vector | dep. on VDE VPN: Virt.Net Locator  | varies     |
+-----------+--------+------------------------------------+------------+

* All transports which have tso and checksum offloads can deliver speeds
  approaching 10G on TCP streams.
  (所有支持 tso 和校验和卸载的传输，在 TCP 流上都能提供接近 10G 的速度。)

* All transports which have multi-packet rx and/or tx can deliver pps
  rates of up to 1Mps or more.
  (所有支持多包 rx 和/或 tx 的传输，都能提供高达 1Mps 或更高的 pps 速率。)

* GRE and L2TPv3 allow connections to all of: local machine, remote
  machines, remote network devices and remote UML instances.
  (GRE 和 L2TPv3 允许连接到：本地机器、远程机器、远程网络设备以及远程 UML 实例。)


Network configuration privileges(网络配置权限)
==============================================

The majority of the supported networking modes need ``root`` privileges.
For example, for vector transports, ``root`` privilege is required to fire
an ioctl to setup the tun interface and/or use raw sockets where needed.
(大多数受支持的网络模式都需要 root 权限。例如，对于 vector 传输，需要 root 权限来发起 ioctl 以设置 tun 接口，和/或在需要时使用 raw socket。)

This can be achieved by granting the user a particular capability instead
of running UML as root.  In case of vector transport, a user can add the
capability ``CAP_NET_ADMIN`` or ``CAP_NET_RAW`` to the uml binary.
Thenceforth, UML can be run with normal user privilges, along with
full networking.
(可以通过授予用户特定的 capability 来实现这一点，而不必以 root 身份运行 UML。对于 vector 传输，用户可以给 uml 二进制文件添加 CAP_NET_ADMIN 或 CAP_NET_RAW capability。此后，UML 就能以普通用户权限运行，并拥有完整的网络功能。)

For example::

   # sudo setcap cap_net_raw,cap_net_admin+ep linux

(例如执行以下命令。)

Configuring vector transports(配置 vector 传输)
===============================================

All vector transports support a similar syntax:
(所有 vector 传输都支持类似的语法：)

If X is the interface number as in vec0, vec1, vec2, etc, the general
syntax for options is::

   vecX:transport="Transport Name",option=value,option=value,...,option=value

(如果 X 是接口编号，如 vec0、vec1、vec2 等，则选项的一般语法如下。)

Common options(通用选项)
------------------------

These options are common for all transports:
(以下选项对所有传输都通用：)

* ``depth=int`` - sets the queue depth for vector IO. This is the
  amount of packets UML will attempt to read or write in a single
  system call. The default number is 64 and is generally sufficient
  for most applications that need throughput in the 2-4 Gbit range.
  Higher speeds may require larger values.
  (depth=int——设置 vector IO 的队列深度，即 UML 在单次系统调用中尝试读写的包数量。默认值为 64，对于大多数需要 2-4 Gbit 吞吐量的应用来说通常已经足够。更高的速度可能需要更大的值。)

* ``mac=XX:XX:XX:XX:XX`` - sets the interface MAC address value.
  (mac=XX:XX:XX:XX:XX——设置接口的 MAC 地址值。)

* ``gro=[0,1]`` - sets GRO off or on. Enables receive/transmit offloads.
  The effect of this option depends on the host side support in the transport
  which is being configured. In most cases it will enable TCP segmentation and
  RX/TX checksumming offloads. The setting must be identical on the host side
  and the UML side. The UML kernel will produce warnings if it is not.
  For example, GRO is enabled by default on local machine interfaces
  (e.g. veth pairs, bridge, etc), so it should be enabled in UML in the
  corresponding UML transports (raw, tap, hybrid) in order for networking to
  operate correctly.
  (gro=[0,1]——设置 GRO 关闭或开启，启用接收/发送卸载。此选项的效果取决于所配置传输在宿主机一侧的支持情况。在大多数情况下，它会启用 TCP 分段以及 RX/TX 校验和卸载。宿主机一侧与 UML 一侧的设置必须一致，否则 UML 内核会产生警告。例如，本地机器接口（如 veth 对、网桥等）默认启用 GRO，因此要让网络正常工作，UML 中对应的传输（raw、tap、hybrid）也应启用 GRO。)

* ``mtu=int`` - sets the interface MTU
  (mtu=int——设置接口的 MTU。)

* ``headroom=int`` - adjusts the default headroom (32 bytes) reserved
  if a packet will need to be re-encapsulated into for instance VXLAN.
  (headroom=int——调整默认保留的头部空间（32 字节），以备数据包需要重新封装（例如封装进 VXLAN）时使用。)

* ``vec=0`` - disable multipacket IO and fall back to packet at a
  time mode
  (vec=0——禁用多包 IO，退回到逐包模式。)

Shared Options(共享选项)
------------------------

* ``ifname=str`` Transports which bind to a local network interface
  have a shared option - the name of the interface to bind to.
  (ifname=str——绑定到本地网络接口的传输有一个共享选项：要绑定的接口名称。)

* ``src, dst, src_port, dst_port`` - all transports which use sockets
  which have the notion of source and destination and/or source port
  and destination port use these to specify them.
  (src, dst, src_port, dst_port——所有使用具有源/目的（和/或源端口/目的端口）概念的套接字的传输，都用这些选项来指定它们。)

* ``v6=[0,1]`` to specify if a v6 connection is desired for all
  transports which operate over IP. Additionally, for transports that
  have some differences in the way they operate over v4 and v6 (for example
  EoL2TPv3), sets the correct mode of operation. In the absence of this
  option, the socket type is determined based on what do the src and dst
  arguments resolve/parse to.
  (v6=[0,1]——对所有基于 IP 运行的传输，指定是否需要 v6 连接。此外，对于那些在 v4 和 v6 上的运行方式有所差异的传输（例如 EoL2TPv3），它会设置正确的运行模式。如果没有此选项，套接字类型将根据 src 和 dst 参数解析出来的结果来确定。)

tap transport(tap 传输)
-----------------------

Example::

   vecX:transport=tap,ifname=tap0,depth=128,gro=1

(示例。)

This will connect vec0 to tap0 on the host. Tap0 must already exist (for example
created using tunctl) and UP.
(这会把 vec0 连接到宿主机上的 tap0。tap0 必须已经存在（例如用 tunctl 创建）并且处于 UP 状态。)

tap0 can be configured as a point-to-point interface and given an IP
address so that UML can talk to the host. Alternatively, it is possible
to connect UML to a tap interface which is connected to a bridge.
(可以把 tap0 配置为点对点接口并赋予 IP 地址，使 UML 能与宿主机通信。另一种做法是把 UML 连接到一个已接入网桥的 tap 接口。)

While tap relies on the vector infrastructure, it is not a true vector
transport at this point, because Linux does not support multi-packet
IO on tap file descriptors for normal userspace apps like UML. This
is a privilege which is offered only to something which can hook up
to it at kernel level via specialized interfaces like vhost-net. A
vhost-net like helper for UML is planned at some point in the future.
(虽然 tap 依赖 vector 基础设施，但就目前而言它并不是真正的 vector 传输，因为 Linux 不支持像 UML 这样的普通用户态应用在 tap 文件描述符上进行多包 IO。这一特权只提供给能够通过 vhost-net 之类的专用接口在内核层面挂接的对象。为 UML 提供一个类似 vhost-net 的辅助程序已列入未来计划。)

Privileges required: tap transport requires either:
(所需权限：tap 传输需要满足以下条件之一：)

* tap interface to exist and be created persistent and owned by the
  UML user using tunctl. Example ``tunctl -u uml-user -t tap0``
  (存在一个 tap 接口，用 tunctl 以持久方式创建并归 UML 用户所有。示例：tunctl -u uml-user -t tap0。)

* binary to have ``CAP_NET_ADMIN`` privilege
  (二进制文件具有 CAP_NET_ADMIN 权限。)

hybrid transport(hybrid 传输)
-----------------------------

Example::

   vecX:transport=hybrid,ifname=tap0,depth=128,gro=1

(示例。)

This is an experimental/demo transport which couples tap for transmit
and a raw socket for receive. The raw socket allows multi-packet
receive resulting in significantly higher packet rates than normal tap.
(这是一个实验性/演示性质的传输：发送使用 tap，接收使用 raw socket。raw socket 支持多包接收，因此包速率显著高于普通 tap。)

Privileges required: hybrid requires ``CAP_NET_RAW`` capability by
the UML user as well as the requirements for the tap transport.
(所需权限：hybrid 要求 UML 用户具有 CAP_NET_RAW capability，同时还要满足 tap 传输的各项要求。)

raw socket transport(raw socket 传输)
-------------------------------------

Example::

   vecX:transport=raw,ifname=p-veth0,depth=128,gro=1

(示例。)


This transport uses vector IO on raw sockets. While you can bind to any
interface including a physical one, the most common use it to bind to
the "peer" side of a veth pair with the other side configured on the
host.
(此传输在 raw socket 上使用 vector IO。虽然你可以绑定到包括物理接口在内的任何接口，但最常见的用法是绑定到 veth 对的"对端"一侧，而另一侧配置在宿主机上。)

Example host configuration for Debian:
(Debian 宿主机配置示例：)

**/etc/network/interfaces**::

   auto veth0
   iface veth0 inet static
	address 192.168.4.1
	netmask 255.255.255.252
	broadcast 192.168.4.3
	pre-up ip link add veth0 type veth peer name p-veth0 && \
          ifconfig p-veth0 up

(/etc/network/interfaces 的配置示例。)

UML can now bind to p-veth0 like this::

   vec0:transport=raw,ifname=p-veth0,depth=128,gro=1

(现在 UML 可以像下面这样绑定到 p-veth0。)


If the UML guest is configured with 192.168.4.2 and netmask 255.255.255.0
it can talk to the host on 192.168.4.1
(如果 UML 客户机配置为 192.168.4.2、子网掩码 255.255.255.0，它就可以与 192.168.4.1 上的宿主机通信。)

The raw transport also provides some support for offloading some of the
filtering to the host. The two options to control it are:
(raw 传输还提供了一些把部分过滤工作卸载到宿主机的支持。控制它的两个选项是：)

* ``bpffile=str`` filename of raw bpf code to be loaded as a socket filter
  (bpffile=str——作为套接字过滤器加载的原始 bpf 代码的文件名。)

* ``bpfflash=int`` 0/1 allow loading of bpf from inside User Mode Linux.
  This option allows the use of the ethtool load firmware command to
  load bpf code.
  (bpfflash=int——0/1，允许从 User Mode Linux 内部加载 bpf。此选项允许使用 ethtool 的加载固件命令来加载 bpf 代码。)

In either case the bpf code is loaded into the host kernel. While this is
presently limited to legacy bpf syntax (not ebpf), it is still a security
risk. It is not recommended to allow this unless the User Mode Linux
instance is considered trusted.
(无论哪种情况，bpf 代码都会被加载进宿主机内核。虽然目前仅限于旧式 bpf 语法（不是 ebpf），但这仍然是一个安全风险。除非认为该 User Mode Linux 实例是可信的，否则不建议允许这种操作。)

Privileges required: raw socket transport requires `CAP_NET_RAW`
capability.
(所需权限：raw socket 传输需要 CAP_NET_RAW capability。)

GRE socket transport(GRE socket 传输)
-------------------------------------

Example::

   vecX:transport=gre,src=$src_host,dst=$dst_host

(示例。)


This will configure an Ethernet over ``GRE`` (aka ``GRETAP`` or
``GREIRB``) tunnel which will connect the UML instance to a ``GRE``
endpoint at host dst_host. ``GRE`` supports the following additional
options:
(这会配置一条 Ethernet over GRE 隧道（又称 GRETAP 或 GREIRB），把 UML 实例连接到 dst_host 主机上的一个 GRE 端点。GRE 支持以下附加选项：)

* ``rx_key=int`` - GRE 32-bit integer key for rx packets, if set,
  ``txkey`` must be set too
  (rx_key=int——rx 数据包的 GRE 32 位整数密钥；如果设置，txkey 也必须设置。)

* ``tx_key=int`` - GRE 32-bit integer key for tx packets, if set
  ``rx_key`` must be set too
  (tx_key=int——tx 数据包的 GRE 32 位整数密钥；如果设置，rx_key 也必须设置。)

* ``sequence=[0,1]`` - enable GRE sequence
  (sequence=[0,1]——启用 GRE 序列号。)

* ``pin_sequence=[0,1]`` - pretend that the sequence is always reset
  on each packet (needed to interoperate with some really broken
  implementations)
  (pin_sequence=[0,1]——假装序列号在每个包上都会重置（与某些非常糟糕的实现互操作时需要）。)

* ``v6=[0,1]`` - force IPv4 or IPv6 sockets respectively
  (v6=[0,1]——分别强制使用 IPv4 或 IPv6 套接字。)

* GRE checksum is not presently supported
  (目前不支持 GRE 校验和。)

GRE has a number of caveats:
(GRE 有以下一些注意事项：)

* You can use only one GRE connection per IP address. There is no way to
  multiplex connections as each GRE tunnel is terminated directly on
  the UML instance.
  (每个 IP 地址只能使用一条 GRE 连接。由于每条 GRE 隧道都直接终结在 UML 实例上，因此无法对连接进行多路复用。)

* The key is not really a security feature. While it was intended as such
  its "security" is laughable. It is, however, a useful feature to
  ensure that the tunnel is not misconfigured.
  (密钥并不是真正的安全特性。虽然设计初衷如此，但它的"安全性"令人啼笑皆非。不过，它对于防止隧道被错误配置还是很有用的。)

An example configuration for a Linux host with a local address of
192.168.128.1 to connect to a UML instance at 192.168.129.1
(一个配置示例：本地地址为 192.168.128.1 的 Linux 主机，连接到位于 192.168.129.1 的 UML 实例。)

**/etc/network/interfaces**::

   auto gt0
   iface gt0 inet static
    address 10.0.0.1
    netmask 255.255.255.0
    broadcast 10.0.0.255
    mtu 1500
    pre-up ip link add gt0 type gretap local 192.168.128.1 \
           remote 192.168.129.1 || true
    down ip link del gt0 || true

(/etc/network/interfaces 的配置示例。)

Additionally, GRE has been tested versus a variety of network equipment.
(此外，GRE 已针对各种网络设备进行过测试。)

Privileges required: GRE requires ``CAP_NET_RAW``
(所需权限：GRE 需要 CAP_NET_RAW。)

l2tpv3 socket transport(l2tpv3 socket 传输)
-------------------------------------------

_Warning_. L2TPv3 has a "bug". It is the "bug" known as "has more
options than GNU ls". While it has some advantages, there are usually
easier (and less verbose) ways to connect a UML instance to something.
For example, most devices which support L2TPv3 also support GRE.
(警告。L2TPv3 有一个"毛病"——这个"毛病"就是"选项比 GNU ls 还多"。虽然它有一些优点，但把 UML 实例连接到别的东西通常有更简单（也没那么啰嗦）的办法。例如，大多数支持 L2TPv3 的设备也支持 GRE。)

Example::

    vec0:transport=l2tpv3,udp=1,src=$src_host,dst=$dst_host,srcport=$src_port,dstport=$dst_port,depth=128,rx_session=0xffffffff,tx_session=0xffff

(示例。)

This will configure an Ethernet over L2TPv3 fixed tunnel which will
connect the UML instance to a L2TPv3 endpoint at host $dst_host using
the L2TPv3 UDP flavour and UDP destination port $dst_port.
(这会配置一条 Ethernet over L2TPv3 固定隧道，使用 L2TPv3 的 UDP 变体和 UDP 目的端口 $dst_port，把 UML 实例连接到主机 $dst_host 上的一个 L2TPv3 端点。)

L2TPv3 always requires the following additional options:
(L2TPv3 总是需要以下附加选项：)

* ``rx_session=int`` - l2tpv3 32-bit integer session for rx packets
  (rx_session=int——rx 数据包的 l2tpv3 32 位整数会话号。)

* ``tx_session=int`` - l2tpv3 32-bit integer session for tx packets
  (tx_session=int——tx 数据包的 l2tpv3 32 位整数会话号。)

As the tunnel is fixed these are not negotiated and they are
preconfigured on both ends.
(由于隧道是固定式的，这些参数不经协商，需要预先在两端配置好。)

Additionally, L2TPv3 supports the following optional parameters.
(此外，L2TPv3 还支持以下可选参数。)

* ``rx_cookie=int`` - l2tpv3 32-bit integer cookie for rx packets - same
  functionality as GRE key, more to prevent misconfiguration than provide
  actual security
  (rx_cookie=int——rx 数据包的 l2tpv3 32 位整数 cookie，功能与 GRE 密钥相同，更多是为了防止配置错误而非提供真正的安全性。)

* ``tx_cookie=int`` - l2tpv3 32-bit integer cookie for tx packets
  (tx_cookie=int——tx 数据包的 l2tpv3 32 位整数 cookie。)

* ``cookie64=[0,1]`` - use 64-bit cookies instead of 32-bit.
  (cookie64=[0,1]——使用 64 位 cookie 而非 32 位。)

* ``counter=[0,1]`` - enable l2tpv3 counter
  (counter=[0,1]——启用 l2tpv3 计数器。)

* ``pin_counter=[0,1]`` - pretend that the counter is always reset on
  each packet (needed to interoperate with some really broken
  implementations)
  (pin_counter=[0,1]——假装计数器在每个包上都会重置（与某些非常糟糕的实现互操作时需要）。)

* ``v6=[0,1]`` - force v6 sockets
  (v6=[0,1]——强制使用 v6 套接字。)

* ``udp=[0,1]`` - use raw sockets (0) or UDP (1) version of the protocol
  (udp=[0,1]——使用协议的 raw socket（0）或 UDP（1）版本。)

L2TPv3 has a number of caveats:
(L2TPv3 有以下一些注意事项：)

* you can use only one connection per IP address in raw mode. There is
  no way to multiplex connections as each L2TPv3 tunnel is terminated
  directly on the UML instance. UDP mode can use different ports for
  this purpose.
  (raw 模式下每个 IP 地址只能使用一条连接。由于每条 L2TPv3 隧道都直接终结在 UML 实例上，因此无法对连接进行多路复用。UDP 模式则可以利用不同的端口来实现这一目的。)

Here is an example of how to configure a Linux host to connect to UML
via L2TPv3:
(下面是一个把 Linux 主机配置为通过 L2TPv3 连接 UML 的示例：)

**/etc/network/interfaces**::

   auto l2tp1
   iface l2tp1 inet static
    address 192.168.126.1
    netmask 255.255.255.0
    broadcast 192.168.126.255
    mtu 1500
    pre-up ip l2tp add tunnel remote 127.0.0.1 \
           local 127.0.0.1 encap udp tunnel_id 2 \
           peer_tunnel_id 2 udp_sport 1706 udp_dport 1707 && \
           ip l2tp add session name l2tp1 tunnel_id 2 \
           session_id 0xffffffff peer_session_id 0xffffffff
    down ip l2tp del session tunnel_id 2 session_id 0xffffffff && \
           ip l2tp del tunnel tunnel_id 2

(/etc/network/interfaces 的配置示例。)


Privileges required: L2TPv3 requires ``CAP_NET_RAW`` for raw IP mode and
no special privileges for the UDP mode.
(所需权限：L2TPv3 的 raw IP 模式需要 CAP_NET_RAW，UDP 模式则不需要特殊权限。)

BESS socket transport(BESS socket 传输)
---------------------------------------

BESS is a high performance modular network switch.
(BESS 是一个高性能的模块化网络交换机。)

https://github.com/NetSys/bess

It has support for a simple sequential packet socket mode which in the
more recent versions is using vector IO for high performance.
(它支持一种简单的顺序包套接字模式，在较新的版本中该模式使用 vector IO 来获得高性能。)

Example::

   vecX:transport=bess,src=$unix_src,dst=$unix_dst

(示例。)

This will configure a BESS transport using the unix_src Unix domain
socket address as source and unix_dst socket address as destination.
(这会配置一个 BESS 传输，以 unix_src Unix 域套接字地址作为源，以 unix_dst 套接字地址作为目的地。)

For BESS configuration and how to allocate a BESS Unix domain socket port
please see the BESS documentation.
(关于 BESS 的配置以及如何分配 BESS Unix 域套接字端口，请参阅 BESS 文档。)

https://github.com/NetSys/bess/wiki/Built-In-Modules-and-Ports

BESS transport does not require any special privileges.
(BESS 传输不需要任何特殊权限。)

VDE vector transport(VDE vector 传输)
-------------------------------------

Virtual Distributed Ethernet (VDE) is a project whose main goal is to provide a
highly flexible support for virtual networking.
(Virtual Distributed Ethernet（VDE）是一个以提供高度灵活的虚拟网络支持为主要目标的项目。)

http://wiki.virtualsquare.org/#/tutorials/vdebasics

Common usages of VDE include fast prototyping and teaching.
(VDE 的常见用途包括快速原型开发和教学。)

Examples:
(示例：)

   ``vecX:transport=vde,vnl=tap://tap0``

use tap0
(使用 tap0。)

   ``vecX:transport=vde,vnl=slirp://``

use slirp
(使用 slirp。)

   ``vec0:transport=vde,vnl=vde:///tmp/switch``

connect to a vde switch
(连接到一个 vde 交换机。)

   ``vecX:transport=\"vde,vnl=cmd://ssh remote.host //tmp/sshlirp\"``

connect to a remote slirp (instant VPN: convert ssh to VPN, it uses sshlirp)
(连接到远程 slirp（即时 VPN：把 ssh 变成 VPN，它使用 sshlirp）。)
https://github.com/virtualsquare/sshlirp

   ``vec0:transport=vde,vnl=vxvde://234.0.0.1``

connect to a local area cloud (all the UML nodes using the same
multicast address running on hosts in the same multicast domain (LAN)
will be automagically connected together to a virtual LAN.
(连接到一个局域云：在同一组播域（LAN）内的主机上、使用相同组播地址运行的所有 UML 节点，会被自动连接成一个虚拟局域网。)

*********************
Running UML(运行 UML)
*********************

This section assumes that either the user-mode-linux package from the
distribution or a custom built kernel has been installed on the host.
(本节假定宿主机上已经安装了发行版提供的 user-mode-linux 软件包，或者是自行构建的内核。)

These add an executable called linux to the system. This is the UML
kernel. It can be run just like any other executable.
It will take most normal linux kernel arguments as command line
arguments.  Additionally, it will need some UML-specific arguments
in order to do something useful.
(它们会向系统添加一个名为 linux 的可执行文件。这就是 UML 内核。它可以像任何其他可执行文件一样运行。它接受大多数普通的 Linux 内核命令行参数。此外，要让它做些有用的事情，还需要一些 UML 特有的参数。)

Arguments(参数)
===============

Mandatory Arguments:(必选参数)
------------------------------

* ``mem=int[K,M,G]`` - amount of memory. By default in bytes. It will
  also accept K, M or G qualifiers.
  (mem=int[K,M,G]——内存大小，默认单位为字节，也接受 K、M 或 G 修饰符。)

* ``ubdX[s,d,c,t]=`` virtual disk specification. This is not really
  mandatory, but it is likely to be needed in nearly all cases so we can
  specify a root file system.
  The simplest possible image specification is the name of the image
  file for the filesystem (created using one of the methods described
  in `Creating an image(创建镜像)`_).
  (ubdX[s,d,c,t]=——虚拟磁盘规格。这并不严格算是必选，但几乎所有情况下都需要它来指定根文件系统。最简单的镜像规格就是文件系统的镜像文件名（用 `Creating an image(创建镜像)`_ 中描述的方法之一创建）。)

  * UBD devices support copy on write (COW). The changes are kept in
    a separate file which can be discarded allowing a rollback to the
    original pristine image.  If COW is desired, the UBD image is
    specified as: ``cow_file,master_image``.
    Example:``ubd0=Filesystem.cow,Filesystem.img``
    (UBD 设备支持写时复制（COW）。改动保存在一个单独的文件中，丢弃该文件即可回滚到原始的干净镜像。如果需要 COW，UBD 镜像按如下方式指定：cow_file,master_image，例如 ubd0=Filesystem.cow,Filesystem.img。)

  * UBD devices can be set to use synchronous IO. Any writes are
    immediately flushed to disk. This is done by adding ``s`` after
    the ``ubdX`` specification.
    (UBD 设备可以设置为使用同步 IO，任何写入都会立即刷盘。做法是在 ubdX 规格后面加上 s。)

  * UBD performs some heuristics on devices specified as a single
    filename to make sure that a COW file has not been specified as
    the image. To turn them off, use the ``d`` flag after ``ubdX``.
    (对于仅以单个文件名指定的设备，UBD 会做一些启发式检查，以确保没有把 COW 文件误当作镜像。要关闭这些检查，可在 ubdX 后面使用 d 标志。)

  * UBD supports TRIM - asking the Host OS to reclaim any unused
    blocks in the image. To turn it off, specify the ``t`` flag after
    ``ubdX``.
    (UBD 支持 TRIM——请求宿主操作系统回收镜像中所有未使用的块。要关闭它，可在 ubdX 后面指定 t 标志。)

* ``root=`` root device - most likely ``/dev/ubd0`` (this is a Linux
  filesystem image)
  (root=——根设备，最可能是 /dev/ubd0（这是一个 Linux 文件系统镜像）。)

Important Optional Arguments(重要可选参数)
------------------------------------------

If UML is run as "linux" with no extra arguments, it will try to start an
xterm for every console configured inside the image (up to 6 in most
Linux distributions). Each console is started inside an
xterm. This makes it nice and easy to use UML on a host with a GUI. It is,
however, the wrong approach if UML is to be used as a testing harness or run
in a text-only environment.
(如果不带任何额外参数以 "linux" 运行 UML，它会为镜像内配置的每个控制台尝试启动一个 xterm（在大多数 Linux 发行版中最多 6 个）。每个控制台都在一个 xterm 内启动。这使得在带 GUI 的宿主机上使用 UML 既方便又轻松。然而，如果要把 UML 用作测试工具或在纯文本环境中运行，这就是错误的方式。)

In order to change this behaviour we need to specify an alternative console
and wire it to one of the supported "line" channels. For this we need to map a
console to use something different from the default xterm.
(要改变这一行为，我们需要指定一个替代控制台，并把它接到支持的某种"线路"通道上。为此，我们需要把某个控制台映射为默认 xterm 之外的东西。)

Example which will divert console number 1 to stdin/stdout::

   con1=fd:0,fd:1

(一个把 1 号控制台改接到标准输入/标准输出的示例。)

UML supports a wide variety of serial line channels which are specified using
the following syntax

   conX=channel_type:options[,channel_type:options]

(UML 支持多种多样的串行线路通道，使用以下语法指定。)


If the channel specification contains two parts separated by comma, the first
one is input, the second one output.
(如果通道规格包含由逗号分隔的两部分，第一部分是输入，第二部分是输出。)

* The null channel - Discard all input or output. Example ``con=null`` will set
  all consoles to null by default.
  (null 通道——丢弃所有输入或输出。例如 con=null 会默认把所有控制台设为 null。)

* The fd channel - use file descriptor numbers for input/output. Example:
  ``con1=fd:0,fd:1.``
  (fd 通道——用文件描述符编号作为输入/输出。示例：con1=fd:0,fd:1。)

* The port channel - start a telnet server on TCP port number. Example:
  ``con1=port:4321``.  The host must have /usr/sbin/in.telnetd (usually part of
  a telnetd package) and the port-helper from the UML utilities (see the
  information for the xterm channel below).  UML will not boot until a client
  connects.
  (port 通道——在给定的 TCP 端口号上启动一个 telnet 服务器。示例：con1=port:4321。宿主机必须有 /usr/sbin/in.telnetd（通常是 telnetd 软件包的一部分）以及 UML 工具中的 port-helper（参见下文 xterm 通道的说明）。在客户端连接之前 UML 不会启动。)

* The pty and pts channels - use system pty/pts.
  (pty 和 pts 通道——使用系统的 pty/pts。)

* The tty channel - bind to an existing system tty. Example: ``con1=/dev/tty8``
  will make UML use the host 8th console (usually unused).
  (tty 通道——绑定到一个已有的系统 tty。示例：con1=/dev/tty8 会让 UML 使用宿主机的第 8 个控制台（通常闲置未用）。)

* The xterm channel - this is the default - bring up an xterm on this channel
  and direct IO to it. Note that in order for xterm to work, the host must
  have the UML distribution package installed. This usually contains the
  port-helper and other utilities needed for UML to communicate with the xterm.
  Alternatively, these need to be complied and installed from source. All
  options applicable to consoles also apply to UML serial lines which are
  presented as ttyS inside UML.
  (xterm 通道——这是默认通道——在此通道上弹出一个 xterm 并把 IO 定向到它。注意，要让 xterm 正常工作，宿主机必须安装 UML 发行版软件包，其中通常包含 port-helper 以及 UML 与 xterm 通信所需的其他工具；或者需要从源码编译并安装这些工具。所有适用于控制台的选项也适用于 UML 串行线路，后者在 UML 内部表现为 ttyS。)

Starting UML(启动 UML)
======================

We can now run UML.
::

   # linux mem=2048M umid=TEST \
    ubd0=Filesystem.img \
    vec0:transport=tap,ifname=tap0,depth=128,gro=1 \
    root=/dev/ubda con=null con0=null,fd:2 con1=fd:0,fd:1

(现在我们可以运行 UML 了。)

This will run an instance with ``2048M RAM`` and try to use the image file
called ``Filesystem.img`` as root. It will connect to the host using tap0.
All consoles except ``con1`` will be disabled and console 1 will
use standard input/output making it appear in the same terminal it was started.
(这会运行一个具有 2048M RAM 的实例，并尝试使用名为 Filesystem.img 的镜像文件作为根文件系统。它会通过 tap0 连接到宿主机。除 con1 之外的所有控制台都将被禁用，1 号控制台将使用标准输入/输出，使其显示在启动它的同一个终端里。)

Logging in(登录)
================

If you have not set up a password when generating the image, you will have to
shut down the UML instance, mount the image, chroot into it and set it - as
described in the Generating an Image section.  If the password is already set,
you can just log in.
(如果在生成镜像时没有设置密码，你必须关闭 UML 实例、挂载镜像、chroot 进去并设置密码——如"生成镜像"（Generating an Image）一节所述。如果密码已经设置好，直接登录即可。)

The UML Management Console(UML 管理控制台)
==========================================

In addition to managing the image from "the inside" using normal sysadmin tools,
it is possible to perform a number of low-level operations using the UML
management console. The UML management console is a low-level interface to the
kernel on a running UML instance, somewhat like the i386 SysRq interface. Since
there is a full-blown operating system under UML, there is much greater
flexibility possible than with the SysRq mechanism.
(除了用常规系统管理工具从"内部"管理镜像之外，还可以使用 UML 管理控制台执行许多低层操作。UML 管理控制台是面向运行中的 UML 实例内核的低层接口，有点类似于 i386 的 SysRq 接口。由于 UML 之下是一个完整的操作系统，因此它比 SysRq 机制灵活得多。)

There are a number of things you can do with the mconsole interface:
(使用 mconsole 接口可以做很多事情：)

* get the kernel version
  (获取内核版本。)
* add and remove devices
  (添加和移除设备。)
* halt or reboot the machine
  (关机或重启机器。)
* Send SysRq commands
  (发送 SysRq 命令。)
* Pause and resume the UML
  (暂停和恢复 UML。)
* Inspect processes running inside UML
  (查看 UML 内运行的进程。)
* Inspect UML internal /proc state
  (查看 UML 内部的 /proc 状态。)

You need the mconsole client (uml\_mconsole) which is a part of the UML
tools package available in most Linux distritions.
(你需要 mconsole 客户端（uml\_mconsole），它是 UML 工具包的一部分，在大多数 Linux 发行版中都可用。)

You also need ``CONFIG_MCONSOLE`` (under 'General Setup') enabled in the UML
kernel.  When you boot UML, you'll see a line like::

   mconsole initialized on /home/jdike/.uml/umlNJ32yL/mconsole

(还需要在 UML 内核中启用 CONFIG_MCONSOLE（位于 'General Setup' 之下）。启动 UML 时，你会看到类似这样的一行。)

If you specify a unique machine id on the UML command line, i.e.
``umid=debian``, you'll see this::

   mconsole initialized on /home/jdike/.uml/debian/mconsole

(如果在 UML 命令行上指定了唯一的机器 ID，即 umid=debian，你会看到如下内容。)


That file is the socket that uml_mconsole will use to communicate with
UML.  Run it with either the umid or the full path as its argument::

   # uml_mconsole debian

(该文件是 uml_mconsole 用来与 UML 通信的套接字。运行它时以 umid 或完整路径作为参数。)

or

   # uml_mconsole /home/jdike/.uml/debian/mconsole


You'll get a prompt, at which you can run one of these commands:
(你会得到一个提示符，在它那里可以运行以下命令之一：)

* version
* help
* halt
* reboot
* config
* remove
* sysrq
* help
* cad
* stop
* go
* proc
* stack

version(version 命令)
---------------------

This command takes no arguments.  It prints the UML version::

   (mconsole)  version
   OK Linux OpenWrt 4.14.106 #0 Tue Mar 19 08:19:41 2019 x86_64

(此命令不接受任何参数，它会打印 UML 版本。)


There are a couple actual uses for this.  It's a simple no-op which
can be used to check that a UML is running.  It's also a way of
sending a device interrupt to the UML. UML mconsole is treated internally as
a UML device.
(它有几个实际用途：它是一个简单的无操作命令，可以用来检查某个 UML 是否在运行；它也是一种向 UML 发送设备中断的方式。UML mconsole 在内部被当作一个 UML 设备对待。)

help(help 命令)
---------------

This command takes no arguments. It prints a short help screen with the
supported mconsole commands.
(此命令不接受任何参数，它会打印一个简短的帮助屏，列出受支持的 mconsole 命令。)


halt and reboot(halt 与 reboot 命令)
------------------------------------

These commands take no arguments.  They shut the machine down immediately, with
no syncing of disks and no clean shutdown of userspace.  So, they are
pretty close to crashing the machine::

   (mconsole)  halt
   OK

(这些命令不接受任何参数。它们会立即把机器关掉，不同步磁盘，也不干净地关闭用户空间。所以，它们的效果非常接近让机器直接崩溃。)

config(config 命令)
-------------------

"config" adds a new device to the virtual machine. This is supported
by most UML device drivers. It takes one argument, which is the
device to add, with the same syntax as the kernel command line::

   (mconsole) config ubd3=/home/jdike/incoming/roots/root_fs_debian22

("config" 向虚拟机添加一个新设备。大多数 UML 设备驱动都支持此操作。它接受一个参数，即要添加的设备，语法与内核命令行相同。)

remove(remove 命令)
-------------------

"remove" deletes a device from the system.  Its argument is just the
name of the device to be removed. The device must be idle in whatever
sense the driver considers necessary.  In the case of the ubd driver,
the removed block device must not be mounted, swapped on, or otherwise
open, and in the case of the network driver, the device must be down::

   (mconsole)  remove ubd3

("remove" 从系统中删除一个设备。它的参数就是要删除的设备名。该设备必须在驱动所要求的意义上处于空闲状态。对于 ubd 驱动，被删除的块设备不能已挂载、已启用交换或以其他方式打开；对于网络驱动，设备必须处于 down 状态。)

sysrq(sysrq 命令)
-----------------

This command takes one argument, which is a single letter.  It calls the
generic kernel's SysRq driver, which does whatever is called for by
that argument.  See the SysRq documentation in
Documentation/admin-guide/sysrq.rst in your favorite kernel tree to
see what letters are valid and what they do.
(此命令接受一个参数，即单个字母。它会调用通用内核的 SysRq 驱动，由该驱动执行参数所要求的操作。想了解哪些字母有效以及它们的作用，请在你喜欢的内核树中查阅 Documentation/admin-guide/sysrq.rst 的 SysRq 文档。)

cad(cad 命令)
-------------

This invokes the ``Ctl-Alt-Del`` action in the running image.  What exactly
this ends up doing is up to init, systemd, etc.  Normally, it reboots the
machine.
(这会在运行中的镜像里触发 Ctl-Alt-Del 动作。最终具体做什么取决于 init、systemd 等。通常它会重启机器。)

stop(stop 命令)
---------------

This puts the UML in a loop reading mconsole requests until a 'go'
mconsole command is received. This is very useful as a
debugging/snapshotting tool.
(这会让 UML 进入一个循环，不断读取 mconsole 请求，直到收到 'go' mconsole 命令。作为调试/快照工具，这非常有用。)

go(go 命令)
-----------

This resumes a UML after being paused by a 'stop' command. Note that
when the UML has resumed, TCP connections may have timed out and if
the UML is paused for a long period of time, crond might go a little
crazy, running all the jobs it didn't do earlier.
(这会让被 'stop' 命令暂停的 UML 恢复运行。注意，UML 恢复后 TCP 连接可能已经超时；而且如果 UML 被暂停了很长时间，crond 可能会有点发狂，把之前没做的所有任务一股脑跑完。)

proc(proc 命令)
---------------

This takes one argument - the name of a file in /proc which is printed
to the mconsole standard output
(它接受一个参数——/proc 中某个文件的名字，该文件的内容会打印到 mconsole 的标准输出。)

stack(stack 命令)
-----------------

This takes one argument - the pid number of a process. Its stack is
printed to a standard output.
(它接受一个参数——某个进程的 pid 号，该进程的栈会打印到标准输出。)

*********************************
Advanced UML Topics(UML 高级主题)
*********************************

Sharing Filesystems between Virtual Machines(在虚拟机之间共享文件系统)
======================================================================

Don't attempt to share filesystems simply by booting two UMLs from the
same file.  That's the same thing as booting two physical machines
from a shared disk.  It will result in filesystem corruption.
(不要试图用同一个文件启动两个 UML 来简单地共享文件系统。这相当于让两台物理机器从一块共享磁盘启动，会导致文件系统损坏。)

Using layered block devices(使用分层块设备)
-------------------------------------------

The way to share a filesystem between two virtual machines is to use
the copy-on-write (COW) layering capability of the ubd block driver.
Any changed blocks are stored in the private COW file, while reads come
from either device - the private one if the requested block is valid in
it, the shared one if not.  Using this scheme, the majority of data
which is unchanged is shared between an arbitrary number of virtual
machines, each of which has a much smaller file containing the changes
that it has made.  With a large number of UMLs booting from a large root
filesystem, this leads to a huge disk space saving.
(在两台虚拟机之间共享文件系统的办法，是利用 ubd 块驱动的写时复制（COW）分层能力。所有发生改动的块都保存在私有的 COW 文件中，而读取则来自两者之一——如果所请求的块在私有文件中有效，就来自私有文件，否则来自共享文件。采用这种方案，大部分未发生变化的数据可以在任意数量的虚拟机之间共享，每个虚拟机只需一个相对小得多的文件来保存它所做的改动。当大量 UML 从一个很大的根文件系统启动时，这能节省大量磁盘空间。)

Sharing file system data will also help performance, since the host will
be able to cache the shared data using a much smaller amount of memory,
so UML disk requests will be served from the host's memory rather than
its disks.  There is a major caveat in doing this on multisocket NUMA
machines.  On such hardware, running many UML instances with a shared
master image and COW changes may cause issues like NMIs from excess of
inter-socket traffic.
(共享文件系统数据还有助于提升性能，因为宿主机可以用少得多的内存缓存共享数据，UML 的磁盘请求将由宿主机的内存而非磁盘来满足。在多插槽（multisocket）NUMA 机器上这样做有一个重大警告：在这类硬件上，让许多 UML 实例共享同一主镜像和各自的 COW 改动，可能因套接字间流量过大而引发 NMIs 之类的问题。)

If you are running UML on high-end hardware like this, make sure to
bind UML to a set of logical CPUs residing on the same socket using the
``taskset`` command or have a look at the "tuning" section.
(如果你在这样的高端硬件上运行 UML，请务必用 taskset 命令把 UML 绑定到位于同一插槽上的一组逻辑 CPU，或者参考"调优"一节。)

To add a copy-on-write layer to an existing block device file, simply
add the name of the COW file to the appropriate ubd switch::

   ubd0=root_fs_cow,root_fs_debian_22

(要给一个已有的块设备文件加上写时复制层，只需把 COW 文件名添加到相应的 ubd 开关上。)

where ``root_fs_cow`` is the private COW file and ``root_fs_debian_22`` is
the existing shared filesystem.  The COW file need not exist.  If it
doesn't, the driver will create and initialize it.
(其中 root_fs_cow 是私有的 COW 文件，root_fs_debian_22 是已有的共享文件系统。COW 文件不必预先存在；如果不存在，驱动会创建并初始化它。)

Disk Usage(磁盘用量)
--------------------

UML has TRIM support which will release any unused space in its disk
image files to the underlying OS. It is important to use either ls -ls
or du to verify the actual file size.
(UML 支持 TRIM，会把磁盘镜像文件中所有未使用的空间释放给底层操作系统。务必使用 ls -ls 或 du 来核实文件的实际大小。)

COW validity.(COW 有效性)
-------------------------

Any changes to the master image will invalidate all COW files. If this
happens, UML will *NOT* automatically delete any of the COW files and
will refuse to boot. In this case the only solution is to either
restore the old image (including its last modified timestamp) or remove
all COW files which will result in their recreation. Any changes in
the COW files will be lost.
(对主镜像的任何改动都会使所有 COW 文件失效。一旦发生这种情况，UML 绝不会自动删除任何 COW 文件，而是会拒绝启动。此时唯一的解决办法，要么是恢复旧镜像（包括其最后修改时间戳），要么删除所有 COW 文件（它们会被重新创建）。COW 文件中的改动将全部丢失。)

Cows can moo - uml_moo : Merging a COW file with its backing file(Cows can moo - uml_moo：把 COW 文件合并回其后备文件)
----------------------------------------------------------------------------------------------------------------------

Depending on how you use UML and COW devices, it may be advisable to
merge the changes in the COW file into the backing file every once in
a while.
(根据你对 UML 和 COW 设备的使用方式，最好每隔一段时间就把 COW 文件中的改动合并回后备文件。)

The utility that does this is uml_moo.  Its usage is::

   uml_moo COW_file new_backing_file

(完成这项工作的工具是 uml_moo，其用法如下。)


There's no need to specify the backing file since that information is
already in the COW file header.  If you're paranoid, boot the new
merged file, and if you're happy with it, move it over the old backing
file.
(无需指定后备文件，因为该信息已经在 COW 文件头里了。如果你比较谨慎，可以先启动新合并出来的文件，满意之后再把它替换掉旧的后备文件。)

``uml_moo`` creates a new backing file by default as a safety measure.
It also has a destructive merge option which will merge the COW file
directly into its current backing file.  This is really only usable
when the backing file only has one COW file associated with it.  If
there are multiple COWs associated with a backing file, a -d merge of
one of them will invalidate all of the others.  However, it is
convenient if you're short of disk space, and it should also be
noticeably faster than a non-destructive merge.
(出于安全考虑，``uml_moo`` 默认会创建一个新的后备文件。它还有一个破坏性合并选项，会把 COW 文件直接合并进其当前的后备文件。只有当后备文件只关联了一个 COW 文件时，这个选项才真正可用。如果一个后备文件关联了多个 COW 文件，对其中的任何一个做 -d 合并都会使其余的 COW 全部失效。不过，在磁盘空间紧张时它很方便，而且速度也应明显快于非破坏性合并。)

``uml_moo`` is installed with the UML distribution packages and is
available as a part of UML utilities.
(``uml_moo`` 随 UML 发行版软件包一起安装，也是 UML 工具包的一部分。)

Host file access(访问宿主机文件)
================================

If you want to access files on the host machine from inside UML, you
can treat it as a separate machine and either nfs mount directories
from the host or copy files into the virtual machine with scp.
However, since UML is running on the host, it can access those
files just like any other process and make them available inside the
virtual machine without the need to use the network.
This is possible with the hostfs virtual filesystem.  With it, you
can mount a host directory into the UML filesystem and access the
files contained in it just as you would on the host.
(如果想从 UML 内部访问宿主机上的文件，你可以把宿主机当作另一台机器，用 nfs 挂载宿主机的目录，或者用 scp 把文件拷进虚拟机。然而，由于 UML 就运行在宿主机上，它可以像任何其他进程一样访问这些文件，无需借助网络就能让它们在虚拟机内可用。这可以通过 hostfs 虚拟文件系统实现。借助它，你可以把宿主机的某个目录挂载进 UML 文件系统，然后像在宿主机上一样访问其中的文件。)

*SECURITY WARNING*
(安全警告)

Hostfs without any parameters to the UML Image will allow the image
to mount any part of the host filesystem and write to it. Always
confine hostfs to a specific "harmless" directory (for example ``/var/tmp``)
if running UML. This is especially important if UML is being run as root.
(不给 UML 镜像传任何参数的 hostfs，会允许该镜像挂载宿主机文件系统的任意部分并写入。运行 UML 时，务必把 hostfs 限制在某个特定的"无害"目录（例如 /var/tmp）内。如果 UML 以 root 身份运行，这一点尤其重要。)

Using hostfs(使用 hostfs)
-------------------------

To begin with, make sure that hostfs is available inside the virtual
machine with::

   # cat /proc/filesystems

(首先，用以下命令确认虚拟机内部可以使用 hostfs。)

``hostfs`` should be listed.  If it's not, either rebuild the kernel
with hostfs configured into it or make sure that hostfs is built as a
module and available inside the virtual machine, and insmod it.
(``hostfs`` 应当出现在列表中。如果没有，要么重新编译内核并把 hostfs 编译进去，要么确保 hostfs 被构建为模块且在虚拟机内部可用，然后 insmod 它。)


Now all you need to do is run mount::

   # mount none /mnt/host -t hostfs

(现在你要做的就是运行 mount。)

will mount the host's ``/`` on the virtual machine's ``/mnt/host``.
If you don't want to mount the host root directory, then you can
specify a subdirectory to mount with the -o switch to mount::

   # mount none /mnt/home -t hostfs -o /home

(这会把宿主机的 ``/`` 挂载到虚拟机的 ``/mnt/host``。如果不想挂载宿主机的根目录，可以用 mount 的 -o 开关指定要挂载的子目录。)

will mount the host's /home on the virtual machine's /mnt/home.
(这会把宿主机的 /home 挂载到虚拟机的 /mnt/home。)

hostfs as the root filesystem(hostfs 作为根文件系统)
----------------------------------------------------

It's possible to boot from a directory hierarchy on the host using
hostfs rather than using the standard filesystem in a file.
To start, you need that hierarchy.  The easiest way is to loop mount
an existing root_fs file::

   #  mount root_fs uml_root_dir -o loop

(可以不用标准的文件镜像，而是用 hostfs 直接从宿主机上的某个目录层级启动。首先，你需要那个目录层级。最简单的办法是把一个已有的 root_fs 文件 loop 挂载出来。)


You need to change the filesystem type of ``/`` in ``etc/fstab`` to be
'hostfs', so that line looks like this::

   /dev/ubd/0       /        hostfs      defaults          1   1

(你需要把 ``etc/fstab`` 中 ``/`` 的文件系统类型改为 'hostfs'，让那一行看起来像下面这样。)

Then you need to chown to yourself all the files in that directory
that are owned by root.  This worked for me::

   #  find . -uid 0 -exec chown jdike {} \;

(然后，你需要把该目录中所有属于 root 的文件 chown 给自己。下面这招对我有效。)

Next, make sure that your UML kernel has hostfs compiled in, not as a
module.  Then run UML with the boot device pointing at that directory::

   ubd0=/path/to/uml/root/directory

(接下来，确保你的 UML 内核把 hostfs 编译进内核而不是作为模块。然后以指向该目录的启动设备运行 UML。)

UML should then boot as it does normally.
(之后 UML 应当能像往常一样启动。)

Hostfs Caveats(hostfs 注意事项)
-------------------------------

Hostfs does not support keeping track of host filesystem changes on the
host (outside UML). As a result, if a file is changed without UML's
knowledge, UML will not know about it and its own in-memory cache of
the file may be corrupt. While it is possible to fix this, it is not
something which is being worked on at present.
(hostfs 不支持跟踪宿主机一侧（UML 之外）的文件系统变化。因此，如果一个文件在 UML 不知情的情况下被修改，UML 不会察觉，它自己对文件的内存缓存可能已经损坏。虽然这在技术上可以修复，但目前并没有人在做这项工作。)

Tuning UML(调优 UML)
====================

UML at present is strictly uniprocessor. It will, however spin up a
number of threads to handle various functions.
(UML 目前严格来说只支持单处理器。不过，它会启动若干线程来处理各种功能。)

The UBD driver, SIGIO and the MMU emulation do that. If the system is
idle, these threads will be migrated to other processors on a SMP host.
This, unfortunately, will usually result in LOWER performance because of
all of the cache/memory synchronization traffic between cores. As a
result, UML will usually benefit from being pinned on a single CPU,
especially on a large system. This can result in performance differences
of 5 times or higher on some benchmarks.
(UBD 驱动、SIGIO 和 MMU 模拟都会这样做。如果系统空闲，这些线程在 SMP 宿主机上会被迁移到其他处理器。不幸的是，由于核心之间存在大量缓存/内存同步流量，这通常会导致性能更差。因此，把 UML 钉在单个 CPU 上通常会更有利，尤其是在大型系统上。在某些基准测试中，这可能造成 5 倍甚至更高的性能差距。)

Similarly, on large multi-node NUMA systems UML will benefit if all of
its memory is allocated from the same NUMA node it will run on. The
OS will *NOT* do that by default. In order to do that, the sysadmin
needs to create a suitable tmpfs ramdisk bound to a particular node
and use that as the source for UML RAM allocation by specifying it
in the TMP or TEMP environment variables. UML will look at the values
of ``TMPDIR``, ``TMP`` or ``TEMP`` for that. If that fails, it will
look for shmfs mounted under ``/dev/shm``. If everything else fails use
``/tmp/`` regardless of the filesystem type used for it::

   mount -t tmpfs -ompol=bind:X none /mnt/tmpfs-nodeX
   TEMP=/mnt/tmpfs-nodeX taskset -cX linux options options options..

(类似地，在大型多节点 NUMA 系统上，如果 UML 的全部内存都从它将要运行的同一个 NUMA 节点分配，它会从中受益。操作系统默认绝不会这样做。要做到这一点，系统管理员需要创建一个绑定到特定节点的合适 tmpfs 内存盘，并通过在 TMP 或 TEMP 环境变量中指定它，让它成为 UML 内存分配的来源。UML 会为此查看 ``TMPDIR``、``TMP`` 或 ``TEMP`` 的值。如果都失败，它会寻找挂载在 ``/dev/shm`` 下的 shmfs。如果一切都不行，就使用 ``/tmp/``，不管它用的是什么文件系统类型。)

***************************************************************************
Contributing to UML and Developing with UML(为 UML 做贡献以及基于 UML 开发)
***************************************************************************

UML is an excellent platform to develop new Linux kernel concepts -
filesystems, devices, virtualization, etc. It provides unrivalled
opportunities to create and test them without being constrained to
emulating specific hardware.
(UML 是开发新 Linux 内核概念——文件系统、设备、虚拟化等——的绝佳平台。它提供了无与伦比的机会来创建和测试这些概念，而不必受限于模拟特定硬件。)

Example - want to try how Linux will work with 4096 "proper" network
devices?
(举个例子——想试试 Linux 配上 4096 个"正经"网络设备会是什么样吗？)

Not an issue with UML. At the same time, this is something which
is difficult with other virtualization packages - they are
constrained by the number of devices allowed on the hardware bus
they are trying to emulate (for example 16 on a PCI bus in qemu).
(在 UML 里这不成问题。与此同时，这在其他虚拟化软件包中却很困难——它们受限于所模拟的硬件总线上允许的设备数量（例如 qemu 的 PCI 总线上只有 16 个）。)

If you have something to contribute such as a patch, a bugfix, a
new feature, please send it to ``linux-um@lists.infradead.org``.
(如果你有东西要贡献，例如补丁、缺陷修复或新功能，请发送到 ``linux-um@lists.infradead.org``。)

Please follow all standard Linux patch guidelines such as cc-ing
relevant maintainers and run ``./scripts/checkpatch.pl`` on your patch.
For more details see ``Documentation/process/submitting-patches.rst``
(请遵循所有标准的 Linux 补丁准则，例如抄送相关维护者，并在你的补丁上运行 ``./scripts/checkpatch.pl``。更多细节请参阅 ``Documentation/process/submitting-patches.rst``。)

Note - the list does not accept HTML or attachments, all emails must
be formatted as plain text.
(注意——邮件列表不接受 HTML 或附件，所有邮件都必须是纯文本格式。)

Developing always goes hand in hand with debugging. First of all,
you can always run UML under gdb and there will be a whole section
later on on how to do that. That, however, is not the only way to
debug a Linux kernel. Quite often adding tracing statements and/or
using UML specific approaches such as ptracing the UML kernel process
are significantly more informative.
(开发总是与调试形影不离。首先，你随时可以在 gdb 下运行 UML，本文后面会有一整节专门讲怎么做。不过，这并不是调试 Linux 内核的唯一方法。很多时候，添加跟踪语句，或者使用 UML 特有的手段（例如对 UML 内核进程做 ptrace），能提供的信息要多得多。)

Tracing UML(跟踪 UML)
=====================

When running, UML consists of a main kernel thread and a number of
helper threads. The ones of interest for tracing are NOT the ones
that are already ptraced by UML as a part of its MMU emulation.
(UML 运行时由一个主内核线程和若干辅助线程组成。对跟踪而言值得关注的，不是那些已经因 MMU 模拟而被 UML ptrace 的线程。)

These are usually the first three threads visible in a ps display.
The one with the lowest PID number and using most CPU is usually the
kernel thread. The other threads are the disk
(ubd) device helper thread and the SIGIO helper thread.
Running ptrace on this thread usually results in the following picture::

   host$ strace -p 16566
   --- SIGIO {si_signo=SIGIO, si_code=POLL_IN, si_band=65} ---
   epoll_wait(4, [{EPOLLIN, {u32=3721159424, u64=3721159424}}], 64, 0) = 1
   epoll_wait(4, [], 64, 0)                = 0
   rt_sigreturn({mask=[PIPE]})             = 16967
   ptrace(PTRACE_GETREGS, 16967, NULL, 0xd5f34f38) = 0
   ptrace(PTRACE_GETREGSET, 16967, NT_X86_XSTATE, [{iov_base=0xd5f35010, iov_len=832}]) = 0
   ptrace(PTRACE_GETSIGINFO, 16967, NULL, {si_signo=SIGTRAP, si_code=0x85, si_pid=16967, si_uid=0}) = 0
   ptrace(PTRACE_SETREGS, 16967, NULL, 0xd5f34f38) = 0
   ptrace(PTRACE_SETREGSET, 16967, NT_X86_XSTATE, [{iov_base=0xd5f35010, iov_len=2696}]) = 0
   ptrace(PTRACE_SYSEMU, 16967, NULL, 0)   = 0
   --- SIGCHLD {si_signo=SIGCHLD, si_code=CLD_TRAPPED, si_pid=16967, si_uid=0, si_status=SIGTRAP, si_utime=65, si_stime=89} ---
   wait4(16967, [{WIFSTOPPED(s) && WSTOPSIG(s) == SIGTRAP | 0x80}], WSTOPPED|__WALL, NULL) = 16967
   ptrace(PTRACE_GETREGS, 16967, NULL, 0xd5f34f38) = 0
   ptrace(PTRACE_GETREGSET, 16967, NT_X86_XSTATE, [{iov_base=0xd5f35010, iov_len=832}]) = 0
   ptrace(PTRACE_GETSIGINFO, 16967, NULL, {si_signo=SIGTRAP, si_code=0x85, si_pid=16967, si_uid=0}) = 0
   timer_settime(0, 0, {it_interval={tv_sec=0, tv_nsec=0}, it_value={tv_sec=0, tv_nsec=2830912}}, NULL) = 0
   getpid()                                = 16566
   clock_nanosleep(CLOCK_MONOTONIC, 0, {tv_sec=1, tv_nsec=0}, NULL) = ? ERESTART_RESTARTBLOCK (Interrupted by signal)
   --- SIGALRM {si_signo=SIGALRM, si_code=SI_TIMER, si_timerid=0, si_overrun=0, si_value={int=1631716592, ptr=0x614204f0}} ---
   rt_sigreturn({mask=[PIPE]})             = -1 EINTR (Interrupted system call)

(它们通常是 ps 显示中最早看到的三个线程。PID 号最小、占用 CPU 最多的那个通常是内核线程。其他线程是磁盘（ubd）设备辅助线程和 SIGIO 辅助线程。对内核线程运行 ptrace，通常得到下面这样的画面。)

This is a typical picture from a mostly idle UML instance.
(这是一个大部分时间处于空闲状态的 UML 实例的典型画面。)

* UML interrupt controller uses epoll - this is UML waiting for IO
  interrupts:
  (UML 的中断控制器使用 epoll——这是 UML 在等待 IO 中断：)

   epoll_wait(4, [{EPOLLIN, {u32=3721159424, u64=3721159424}}], 64, 0) = 1

* The sequence of ptrace calls is part of MMU emulation and running the
  UML userspace.
  (这一系列 ptrace 调用是 MMU 模拟以及运行 UML 用户空间的一部分。)
* ``timer_settime`` is part of the UML high res timer subsystem mapping
  timer requests from inside UML onto the host high resolution timers.
  (``timer_settime`` 是 UML 高分辨率定时器子系统的一部分，它把 UML 内部的定时器请求映射到宿主机的高分辨率定时器上。)
* ``clock_nanosleep`` is UML going into idle (similar to the way a PC
  will execute an ACPI idle).
  (``clock_nanosleep`` 是 UML 进入空闲状态（类似于 PC 执行 ACPI idle 的方式）。)

As you can see UML will generate quite a bit of output even in idle. The output
can be very informative when observing IO. It shows the actual IO calls, their
arguments and returns values.
(可以看出，即使处于空闲状态，UML 也会产生相当多的输出。在观察 IO 时，这些输出可能非常有用，它会展示实际的 IO 调用、它们的参数和返回值。)

Kernel debugging(内核调试)
==========================

You can run UML under gdb now, though it will not necessarily agree to
be started under it. If you are trying to track a runtime bug, it is
much better to attach gdb to a running UML instance and let UML run.
(现在你可以在 gdb 下运行 UML，不过它未必乐意被这样启动。如果你要追查一个运行期缺陷，更好的做法是把 gdb 附着到一个正在运行的 UML 实例上，让 UML 继续跑。)

Assuming the same PID number as in the previous example, this would be::

   # gdb -p 16566

(沿用前例中的 PID 号，命令如下。)

This will STOP the UML instance, so you must enter `cont` at the GDB
command line to request it to continue. It may be a good idea to make
this into a gdb script and pass it to gdb as an argument.
(这会 STOP（暂停）那个 UML 实例，所以你必须在 GDB 命令行输入 `cont` 来请求它继续运行。把这一步做成 gdb 脚本并作为参数传给 gdb，可能是个好主意。)

Developing Device Drivers(开发设备驱动)
=======================================

Nearly all UML drivers are monolithic. While it is possible to build a
UML driver as a kernel module, that limits the possible functionality
to in-kernel only and non-UML specific.  The reason for this is that
in order to really leverage UML, one needs to write a piece of
userspace code which maps driver concepts onto actual userspace host
calls.
(几乎所有 UML 驱动都是内建的。虽然可以把 UML 驱动构建成内核模块，但这会把可用功能限制在内核内且非 UML 特有的范围。原因在于，要真正发挥 UML 的威力，需要编写一段用户态代码，把驱动概念映射到真实的用户态宿主机调用上。)

This forms the so-called "user" portion of the driver. While it can
reuse a lot of kernel concepts, it is generally just another piece of
userspace code. This portion needs some matching "kernel" code which
resides inside the UML image and which implements the Linux kernel part.
(这就构成了驱动中所谓的 "user" 部分。虽然它可以复用许多内核概念，但它通常只是又一段用户态代码。这一部分需要一些与之匹配的 "kernel" 代码，后者驻留在 UML 镜像内部，实现 Linux 内核侧的部分。)

*Note: There are very few limitations in the way "kernel" and "user" interact*.
(注意：在 "kernel" 与 "user" 的交互方式上，限制非常少。)

UML does not have a strictly defined kernel-to-host API. It does not
try to emulate a specific architecture or bus. UML's "kernel" and
"user" can share memory, code and interact as needed to implement
whatever design the software developer has in mind. The only
limitations are purely technical. Due to a lot of functions and
variables having the same names, the developer should be careful
which includes and libraries they are trying to refer to.
(UML 没有严格定义的内核到宿主机 API。它不试图模拟特定的体系结构或总线。UML 的 "kernel" 和 "user" 可以共享内存与代码，并按需交互，以实现软件开发者构想的任何设计。唯一的限制纯粹是技术性的。由于大量函数和变量重名，开发者在引用头文件和库时需要小心区分。)

As a result a lot of userspace code consists of simple wrappers.
E.g. ``os_close_file()`` is just a wrapper around ``close()``
which ensures that the userspace function close does not clash
with similarly named function(s) in the kernel part.
(因此，很多用户态代码都由简单的包装构成。例如 ``os_close_file()`` 只是 ``close()`` 的一个包装，用于确保用户态的 close 函数不会与内核部分中同名的函数发生冲突。)

Using UML as a Test Platform(把 UML 用作测试平台)
=================================================

UML is an excellent test platform for device driver development. As
with most things UML, "some user assembly may be required". It is
up to the user to build their emulation environment. UML at present
provides only the kernel infrastructure.
(UML 是设备驱动开发的绝佳测试平台。与 UML 的大多数事情一样，"可能需要用户自行组装"。构建什么样的模拟环境由用户自己决定，UML 目前只提供内核基础设施。)

Part of this infrastructure is the ability to load and parse fdt
device tree blobs as used in Arm or Open Firmware platforms. These
are supplied as an optional extra argument to the kernel command
line::

    dtb=filename

(这个基础设施的一部分，是加载并解析 fdt 设备树 blob 的能力，即 Arm 或 Open Firmware 平台上使用的那种。它们作为可选的额外参数提供给内核命令行。)

The device tree is loaded and parsed at boottime and is accessible by
drivers which query it. At this moment in time this facility is
intended solely for development purposes. UML's own devices do not
query the device tree.
(设备树在启动时被加载并解析，查询它的驱动可以访问它。目前这一功能仅用于开发目的。UML 自己的设备不会查询设备树。)

Security Considerations(安全考量)
---------------------------------

Drivers or any new functionality should default to not
accepting arbitrary filename, bpf code or other parameters
which can affect the host from inside the UML instance.
For example, specifying the socket used for IPC communication
between a driver and the host at the UML command line is OK
security-wise. Allowing it as a loadable module parameter
isn't.
(驱动或任何新功能，默认都不应接受任意文件名、bpf 代码或其他能从 UML 实例内部影响宿主机的参数。例如，在 UML 命令行上指定驱动与宿主机之间 IPC 通信所用的套接字，从安全角度看是可以的；但把它做成可加载模块参数则不行。)

If such functionality is desirable for a particular application
(e.g. loading BPF "firmware" for raw socket network transports),
it should be off by default and should be explicitly turned on
as a command line parameter at startup.
(如果某类应用确实需要这种功能（例如为 raw socket 网络传输加载 BPF "固件"），它应当默认关闭，并应在启动时作为命令行参数显式打开。)

Even with this in mind, the level of isolation between UML
and the host is relatively weak. If the UML userspace is
allowed to load arbitrary kernel drivers, an attacker can
use this to break out of UML. Thus, if UML is used in
a production application, it is recommended that all modules
are loaded at boot and kernel module loading is disabled
afterwards.
(即便如此，UML 与宿主机之间的隔离程度也相对较弱。如果允许 UML 用户空间加载任意内核驱动，攻击者可以利用这一点逃出 UML。因此，如果 UML 用在生产应用中，建议在启动时加载全部模块，之后禁用内核模块加载。)
