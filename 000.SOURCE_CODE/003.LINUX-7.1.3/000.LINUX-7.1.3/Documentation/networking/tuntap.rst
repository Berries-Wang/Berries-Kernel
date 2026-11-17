.. SPDX-License-Identifier: GPL-2.0
.. include:: <isonum.txt>

======================================================
Universal TUN/TAP device driver(通用 TUN/TAP 设备驱动)
======================================================

Copyright |copy| 1999-2000 Maxim Krasnyansky <max_mk@yahoo.com>
(版权所有 |copy| 1999-2000 Maxim Krasnyansky <max_mk@yahoo.com>)

  Linux, Solaris drivers
  Copyright |copy| 1999-2000 Maxim Krasnyansky <max_mk@yahoo.com>
  (Linux、Solaris 驱动，版权所有 |copy| 1999-2000 Maxim Krasnyansky <max_mk@yahoo.com>)

  FreeBSD TAP driver
  Copyright |copy| 1999-2000 Maksim Yevmenkin <m_evmenkin@yahoo.com>
  (FreeBSD TAP 驱动，版权所有 |copy| 1999-2000 Maksim Yevmenkin <m_evmenkin@yahoo.com>)

  Revision of this document 2002 by Florian Thiel <florian.thiel@gmx.net>
  (本文档 2002 年由 Florian Thiel <florian.thiel@gmx.net> 修订)

1. Description(1. 描述)
=======================

  TUN/TAP provides packet reception and transmission for user space programs.
  It can be seen as a simple Point-to-Point or Ethernet device, which,
  instead of receiving packets from physical media, receives them from
  user space program and instead of sending packets via physical media
  writes them to the user space program.
  (TUN/TAP 为用户态程序提供数据包的接收与发送。它可以看作一个简单的点对点或以太网设备：它不从物理介质接收数据包，而是从用户态程序接收；也不通过物理介质发送数据包，而是把数据包写入用户态程序。)

  In order to use the driver a program has to open /dev/net/tun and issue a
  corresponding ioctl() to register a network device with the kernel. A network
  device will appear as tunXX or tapXX, depending on the options chosen. When
  the program closes the file descriptor, the network device and all
  corresponding routes will disappear.
  (要使用该驱动，程序必须打开 /dev/net/tun 并发出相应的 ioctl() 向内核注册一个网络设备。网络设备将显示为 tunXX 或 tapXX，取决于所选的选项。当程序关闭文件描述符时，该网络设备及所有相应的路由都会消失。)

  Depending on the type of device chosen the userspace program has to read/write
  IP packets (with tun) or ethernet frames (with tap). Which one is being used
  depends on the flags given with the ioctl().
  (根据所选的设备类型，用户态程序必须读/写 IP 数据包（tun）或以太网帧（tap）。使用哪一种取决于 ioctl() 所给的标志。)

  The package from http://vtun.sourceforge.net/tun contains two simple examples
  for how to use tun and tap devices. Both programs work like a bridge between
  two network interfaces.
  br_select.c - bridge based on select system call.
  br_sigio.c  - bridge based on async io and SIGIO signal.
  However, the best example is VTun http://vtun.sourceforge.net :))
  (来自 http://vtun.sourceforge.net/tun 的软件包包含两个如何使用 tun 和 tap 设备的简单示例。两个程序都像两个网络接口之间的网桥一样工作。
  br_select.c —— 基于 select 系统调用的网桥。
  br_sigio.c —— 基于异步 I/O 和 SIGIO 信号的网桥。
  不过，最好的示例是 VTun http://vtun.sourceforge.net 。)

2. Configuration(2. 配置)
=========================

  Create device node::
  (创建设备节点：)

     mkdir /dev/net (if it doesn't exist already)
     mknod /dev/net/tun c 10 200

  Set permissions::
  (设置权限：)

     e.g. chmod 0666 /dev/net/tun

  There's no harm in allowing the device to be accessible by non-root users,
  since CAP_NET_ADMIN is required for creating network devices or for
  connecting to network devices which aren't owned by the user in question.
  If you want to create persistent devices and give ownership of them to
  unprivileged users, then you need the /dev/net/tun device to be usable by
  those users.
  (允许非 root 用户访问该设备并无害处，因为创建网络设备、或连接到不属于该用户的网络设备都需要 CAP_NET_ADMIN。如果你想创建持久设备并把它们的所有权交给无特权的用户，那么需要让 /dev/net/tun 设备可供这些用户使用。)

  Driver module autoloading
  (驱动模块自动加载)

     Make sure that "Kernel module loader" - module auto-loading
     support is enabled in your kernel.  The kernel should load it on
     first access.
     (确保内核中启用了 "内核模块加载器" (Kernel module loader)——模块自动加载支持。内核应在首次访问时自动加载该模块。)

  Manual loading
  (手动加载)

     insert the module by hand::
     (手工插入该模块：)

	modprobe tun

  If you do it the latter way, you have to load the module every time you
  need it, if you do it the other way it will be automatically loaded when
  /dev/net/tun is being opened.
  (如果采用后一种方式，每次需要使用时都必须手动加载该模块；如果采用前一种方式，当 /dev/net/tun 被打开时模块会自动加载。)

3. Program interface(3. 程序接口)
=================================

3.1 Network device allocation(3.1 网络设备的分配)
-------------------------------------------------

``char *dev`` should be the name of the device with a format string (e.g.
"tun%d"), but (as far as I can see) this can be any valid network device name.
Note that the character pointer becomes overwritten with the real device name
(e.g. "tun0")::
(``char *dev`` 应为带格式字符串的设备名（例如 "tun%d"），但（据我所见）它也可以是任何有效的网络设备名。注意，该字符指针会被真实设备名（例如 "tun0"）覆写：)

  #include <linux/if.h>
  #include <linux/if_tun.h>

  int tun_alloc(char *dev)
  {
      struct ifreq ifr;
      int fd, err;

      if( (fd = open("/dev/net/tun", O_RDWR)) < 0 )
	 return tun_alloc_old(dev);

      memset(&ifr, 0, sizeof(ifr));

      /* Flags: IFF_TUN   - TUN device (no Ethernet headers)
       *        IFF_TAP   - TAP device
       *
       *        IFF_NO_PI - Do not provide packet information
       */
      ifr.ifr_flags = IFF_TUN;
      if( *dev )
	 strscpy_pad(ifr.ifr_name, dev, IFNAMSIZ);

      if( (err = ioctl(fd, TUNSETIFF, (void *) &ifr)) < 0 ){
	 close(fd);
	 return err;
      }
      strcpy(dev, ifr.ifr_name);
      return fd;
  }

3.2 Frame format(3.2 帧格式)
----------------------------

If flag IFF_NO_PI is not set each frame format is::
(如果未设置 IFF_NO_PI 标志，则每帧的格式如下：)

     Flags [2 bytes]
     Proto [2 bytes]
     Raw protocol(IP, IPv6, etc) frame.

3.3 Multiqueue tuntap interface(3.3 多队列 tuntap 接口)
-------------------------------------------------------

From version 3.8, Linux supports multiqueue tuntap which can uses multiple
file descriptors (queues) to parallelize packets sending or receiving. The
device allocation is the same as before, and if user wants to create multiple
queues, TUNSETIFF with the same device name must be called many times with
IFF_MULTI_QUEUE flag.
(自 3.8 版本起，Linux 支持多队列 tuntap，它可以使用多个文件描述符（队列）来并行发送或接收数据包。设备分配与之前相同；如果用户想创建多个队列，必须多次调用 TUNSETIFF 并带 IFF_MULTI_QUEUE 标志，且使用相同的设备名。)

``char *dev`` should be the name of the device, queues is the number of queues
to be created, fds is used to store and return the file descriptors (queues)
created to the caller. Each file descriptor were served as the interface of a
queue which could be accessed by userspace.
(``char *dev`` 应为设备名，queues 是要创建的队列数，fds 用于存储并向调用者返回所创建的文件描述符（队列）。每个文件描述符都充当一个队列的接口，可由用户态访问。)

::

  #include <linux/if.h>
  #include <linux/if_tun.h>

  int tun_alloc_mq(char *dev, int queues, int *fds)
  {
      struct ifreq ifr;
      int fd, err, i;

      if (!dev)
	  return -1;

      memset(&ifr, 0, sizeof(ifr));
      /* Flags: IFF_TUN   - TUN device (no Ethernet headers)
       *        IFF_TAP   - TAP device
       *
       *        IFF_NO_PI - Do not provide packet information
       *        IFF_MULTI_QUEUE - Create a queue of multiqueue device
       */
      ifr.ifr_flags = IFF_TAP | IFF_NO_PI | IFF_MULTI_QUEUE;
      strcpy(ifr.ifr_name, dev);

      for (i = 0; i < queues; i++) {
	  if ((fd = open("/dev/net/tun", O_RDWR)) < 0)
	     goto err;
	  err = ioctl(fd, TUNSETIFF, (void *)&ifr);
	  if (err) {
	     close(fd);
	     goto err;
	  }
	  fds[i] = fd;
      }

      return 0;
  err:
      for (--i; i >= 0; i--)
	  close(fds[i]);
      return err;
  }

A new ioctl(TUNSETQUEUE) were introduced to enable or disable a queue. When
calling it with IFF_DETACH_QUEUE flag, the queue were disabled. And when
calling it with IFF_ATTACH_QUEUE flag, the queue were enabled. The queue were
enabled by default after it was created through TUNSETIFF.
(引入了新的 ioctl(TUNSETQUEUE) 用于启用或禁用一个队列。以 IFF_DETACH_QUEUE 标志调用它时，队列被禁用；以 IFF_ATTACH_QUEUE 标志调用时，队列被启用。通过 TUNSETIFF 创建的队列默认是启用的。)

fd is the file descriptor (queue) that we want to enable or disable, when
enable is true we enable it, otherwise we disable it::
(fd 是我们想要启用或禁用的文件描述符（队列）；enable 为 true 时启用它，否则禁用它：)

  #include <linux/if.h>
  #include <linux/if_tun.h>

  int tun_set_queue(int fd, int enable)
  {
      struct ifreq ifr;

      memset(&ifr, 0, sizeof(ifr));

      if (enable)
	 ifr.ifr_flags = IFF_ATTACH_QUEUE;
      else
	 ifr.ifr_flags = IFF_DETACH_QUEUE;

      return ioctl(fd, TUNSETQUEUE, (void *)&ifr);
  }

Universal TUN/TAP device driver Frequently Asked Question(通用 TUN/TAP 设备驱动常见问题解答)
============================================================================================

1. What platforms are supported by TUN/TAP driver ?
(1. TUN/TAP 驱动支持哪些平台？)

Currently driver has been written for 3 Unices:
(目前该驱动已为 3 种 Unix 编写：)

  - Linux kernels 2.2.x, 2.4.x
    (Linux 内核 2.2.x、2.4.x)
  - FreeBSD 3.x, 4.x, 5.x
    (FreeBSD 3.x、4.x、5.x)
  - Solaris 2.6, 7.0, 8.0
    (Solaris 2.6、7.0、8.0)

2. What is TUN/TAP driver used for?
(2. TUN/TAP 驱动的用途是什么？)

As mentioned above, main purpose of TUN/TAP driver is tunneling.
It is used by VTun (http://vtun.sourceforge.net).
(如上所述，TUN/TAP 驱动的主要用途是隧道。VTun (http://vtun.sourceforge.net) 使用了它。)

Another interesting application using TUN/TAP is pipsecd
(http://perso.enst.fr/~beyssac/pipsec/), a userspace IPSec
implementation that can use complete kernel routing (unlike FreeS/WAN).
(另一个使用 TUN/TAP 的有趣应用是 pipsecd (http://perso.enst.fr/~beyssac/pipsec/)，它是一个用户态 IPSec 实现，可以使用完整的内核路由（与 FreeS/WAN 不同）。)

3. How does Virtual network device actually work ?
(3. 虚拟网络设备实际是如何工作的？)

Virtual network device can be viewed as a simple Point-to-Point or
Ethernet device, which instead of receiving packets from a physical
media, receives them from user space program and instead of sending
packets via physical media sends them to the user space program.
(虚拟网络设备可以看作一个简单的点对点或以太网设备：它不从物理介质接收数据包，而是从用户态程序接收；不通过物理介质发送数据包，而是发送给用户态程序。)

Let's say that you configured IPv6 on the tap0, then whenever
the kernel sends an IPv6 packet to tap0, it is passed to the application
(VTun for example). The application encrypts, compresses and sends it to
the other side over TCP or UDP. The application on the other side decompresses
and decrypts the data received and writes the packet to the TAP device,
the kernel handles the packet like it came from real physical device.
(假设你在 tap0 上配置了 IPv6，那么每当内核向 tap0 发送一个 IPv6 数据包时，它都会被传递给应用程序（例如 VTun）。应用程序将其加密、压缩并通过 TCP 或 UDP 发送到另一端。另一端的应用程序把收到的数据解压、解密，并把数据包写入 TAP 设备，内核像处理来自真实物理设备的数据包一样处理它。)

4. What is the difference between TUN driver and TAP driver?
(4. TUN 驱动与 TAP 驱动有什么区别？)

TUN works with IP frames. TAP works with Ethernet frames.
(TUN 处理 IP 帧。TAP 处理以太网帧。)

This means that you have to read/write IP packets when you are using tun and
ethernet frames when using tap.
(这意味着使用 tun 时必须读/写 IP 数据包，而使用 tap 时读/写以太网帧。)

5. What is the difference between BPF and TUN/TAP driver?
(5. BPF 与 TUN/TAP 驱动有什么区别？)

BPF is an advanced packet filter. It can be attached to existing
network interface. It does not provide a virtual network interface.
A TUN/TAP driver does provide a virtual network interface and it is possible
to attach BPF to this interface.
(BPF 是一种高级数据包过滤器。它可以挂接到现有的网络接口上。它不提供虚拟网络接口。TUN/TAP 驱动则提供虚拟网络接口，并且可以把 BPF 挂接到该接口上。)

6. Does TAP driver support kernel Ethernet bridging?
(6. TAP 驱动支持内核以太网桥接吗？)

Yes. Linux and FreeBSD drivers support Ethernet bridging.
(支持。Linux 和 FreeBSD 驱动都支持以太网桥接。)
