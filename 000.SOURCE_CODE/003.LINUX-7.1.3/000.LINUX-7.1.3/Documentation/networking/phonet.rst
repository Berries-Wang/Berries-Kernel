.. SPDX-License-Identifier: GPL-2.0
.. include:: <isonum.txt>

============================
Linux Phonet protocol family
============================

(Linux Phonet 协议族)

Introduction
(简介)
------------

Phonet is a packet protocol used by Nokia cellular modems for both IPC
and RPC. With the Linux Phonet socket family, Linux host processes can
receive and send messages from/to the modem, or any other external
device attached to the modem. The modem takes care of routing.
(Phonet 是 Nokia 蜂窝调制解调器使用的分组协议，同时用于进程间通信 (IPC) 和远程过程调用 (RPC)。借助 Linux Phonet 套接字族，Linux 主机上的进程可以向调制解调器或连接到调制解调器的任何其他外部设备发送消息以及从中接收消息。路由由调制解调器负责。)

Phonet packets can be exchanged through various hardware connections
depending on the device, such as:
(Phonet 数据包可以通过多种硬件连接进行交换，具体取决于设备，例如：)

  - USB with the CDC Phonet interface,
    (带 CDC Phonet 接口的 USB，)
  - infrared,
    (红外，)
  - Bluetooth,
    (蓝牙，)
  - an RS232 serial port (with a dedicated "FBUS" line discipline),
    (RS232 串口（使用专用的 "FBUS" 行规程），)
  - the SSI bus with some TI OMAP processors.
    (某些 TI OMAP 处理器上的 SSI 总线。)


Packets format
(数据包格式)
--------------

Phonet packets have a common header as follows::
(Phonet 数据包具有如下所示的公共报头：)

  struct phonethdr {
    uint8_t  pn_media;  /* Media type (link-layer identifier) */
    uint8_t  pn_rdev;   /* Receiver device ID */
    uint8_t  pn_sdev;   /* Sender device ID */
    uint8_t  pn_res;    /* Resource ID or function */
    uint16_t pn_length; /* Big-endian message byte length (minus 6) */
    uint8_t  pn_robj;   /* Receiver object ID */
    uint8_t  pn_sobj;   /* Sender object ID */
  };

On Linux, the link-layer header includes the pn_media byte (see below).
The next 7 bytes are part of the network-layer header.
(在 Linux 上，链路层报头包含 pn_media 字节（见下文）。接下来的 7 个字节是网络层报头的一部分。)

The device ID is split: the 6 higher-order bits constitute the device
address, while the 2 lower-order bits are used for multiplexing, as are
the 8-bit object identifiers. As such, Phonet can be considered as a
network layer with 6 bits of address space and 10 bits for transport
protocol (much like port numbers in IP world).
(设备 ID 是拆分的：高 6 位构成设备地址，低 2 位用于多路复用，8 位对象标识符同样如此。因此，可以把 Phonet 看作一个具有 6 位地址空间和 10 位传输协议标识（很类似于 IP 世界中的端口号）的网络层。)

The modem always has address number zero. All other device have a their
own 6-bit address.
(调制解调器的地址编号始终为零。所有其他设备都拥有自己的 6 位地址。)


Link layer
(链路层)
----------

Phonet links are always point-to-point links. The link layer header
consists of a single Phonet media type byte. It uniquely identifies the
link through which the packet is transmitted, from the modem's
perspective. Each Phonet network device shall prepend and set the media
type byte as appropriate. For convenience, a common phonet_header_ops
link-layer header operations structure is provided. It sets the
media type according to the network device hardware address.
(Phonet 链路始终是点对点链路。链路层报头由单个 Phonet 媒体类型字节组成。从调制解调器的角度看，它唯一地标识数据包传输所经过的链路。每个 Phonet 网络设备都应在前面加上媒体类型字节并适当地设置它。为方便起见，提供了公共的 phonet_header_ops 链路层报头操作结构。它根据网络设备的硬件地址设置媒体类型。)

Linux Phonet network interfaces support a dedicated link layer packets
type (ETH_P_PHONET) which is out of the Ethernet type range. They can
only send and receive Phonet packets.
(Linux Phonet 网络接口支持一种专用的链路层包类型 (ETH_P_PHONET)，它超出以太网类型的取值范围。它们只能发送和接收 Phonet 数据包。)

The virtual TUN tunnel device driver can also be used for Phonet. This
requires IFF_TUN mode, _without_ the IFF_NO_PI flag. In this case,
there is no link-layer header, so there is no Phonet media type byte.
(虚拟 TUN 隧道设备驱动也可用于 Phonet。这需要 IFF_TUN 模式，但不带 IFF_NO_PI 标志。在这种情况下，没有链路层报头，因此也没有 Phonet 媒体类型字节。)

Note that Phonet interfaces are not allowed to re-order packets, so
only the (default) Linux FIFO qdisc should be used with them.
(注意，不允许 Phonet 接口对数据包重新排序，因此只能对它们使用（默认的）Linux FIFO qdisc。)


Network layer
(网络层)
-------------

The Phonet socket address family maps the Phonet packet header::
(Phonet 套接字地址族对 Phonet 数据包报头进行映射：)

  struct sockaddr_pn {
    sa_family_t spn_family;    /* AF_PHONET */
    uint8_t     spn_obj;       /* Object ID */
    uint8_t     spn_dev;       /* Device ID */
    uint8_t     spn_resource;  /* Resource or function */
    uint8_t     spn_zero[...]; /* Padding */
  };

The resource field is only used when sending and receiving;
It is ignored by bind() and getsockname().
(resource 字段仅在发送和接收时使用；bind() 和 getsockname() 会忽略它。)


Low-level datagram protocol
(低层数据报协议)
---------------------------

Applications can send Phonet messages using the Phonet datagram socket
protocol from the PF_PHONET family. Each socket is bound to one of the
2^10 object IDs available, and can send and receive packets with any
other peer.
(应用程序可以使用 PF_PHONET 族中的 Phonet 数据报套接字协议发送 Phonet 消息。每个套接字绑定到可用的 2^10 个对象 ID 之一，并且可以与任何其他对端相互发送和接收数据包。)

::

  struct sockaddr_pn addr = { .spn_family = AF_PHONET, };
  ssize_t len;
  socklen_t addrlen = sizeof(addr);
  int fd;

  fd = socket(PF_PHONET, SOCK_DGRAM, 0);
  bind(fd, (struct sockaddr *)&addr, sizeof(addr));
  /* ... */

  sendto(fd, msg, msglen, 0, (struct sockaddr *)&addr, sizeof(addr));
  len = recvfrom(fd, buf, sizeof(buf), 0,
		 (struct sockaddr *)&addr, &addrlen);

This protocol follows the SOCK_DGRAM connection-less semantics.
However, connect() and getpeername() are not supported, as they did
not seem useful with Phonet usages (could be added easily).
(此协议遵循 SOCK_DGRAM 无连接语义。不过，connect() 和 getpeername() 不受支持，因为它们对 Phonet 的用途似乎没什么用处（很容易添加）。)


Resource subscription
(资源订阅)
---------------------

A Phonet datagram socket can be subscribed to any number of 8-bits
Phonet resources, as follow::
(一个 Phonet 数据报套接字可以订阅任意数量的 8 位 Phonet 资源，方法如下：)

  uint32_t res = 0xXX;
  ioctl(fd, SIOCPNADDRESOURCE, &res);

Subscription is similarly cancelled using the SIOCPNDELRESOURCE I/O
control request, or when the socket is closed.
(类似地，可以使用 SIOCPNDELRESOURCE I/O 控制请求取消订阅，或者在套接字关闭时取消。)

Note that no more than one socket can be subscribed to any given
resource at a time. If not, ioctl() will return EBUSY.
(注意，任何给定的资源在同一时间最多只能被一个套接字订阅。否则 ioctl() 将返回 EBUSY。)


Phonet Pipe protocol
(Phonet 管道协议)
--------------------

The Phonet Pipe protocol is a simple sequenced packets protocol
with end-to-end congestion control. It uses the passive listening
socket paradigm. The listening socket is bound to an unique free object
ID. Each listening socket can handle up to 255 simultaneous
connections, one per accept()'d socket.
(Phonet 管道协议是一种简单的顺序分组协议，具有端到端拥塞控制。它使用被动监听套接字范式。监听套接字绑定到一个唯一的空闲对象 ID。每个监听套接字最多可以处理 255 个并发连接，每个 accept() 出的套接字对应一个连接。)

::

  int lfd, cfd;

  lfd = socket(PF_PHONET, SOCK_SEQPACKET, PN_PROTO_PIPE);
  listen (lfd, INT_MAX);

  /* ... */
  cfd = accept(lfd, NULL, NULL);
  for (;;)
  {
    char buf[...];
    ssize_t len = read(cfd, buf, sizeof(buf));

    /* ... */

    write(cfd, msg, msglen);
  }

Connections are traditionally established between two endpoints by a
"third party" application. This means that both endpoints are passive.
(传统上，连接由"第三方"应用在两个端点之间建立。这意味着两个端点都是被动的。)


As of Linux kernel version 2.6.39, it is also possible to connect
two endpoints directly, using connect() on the active side. This is
intended to support the newer Nokia Wireless Modem API, as found in
e.g. the Nokia Slim Modem in the ST-Ericsson U8500 platform::
(从 Linux 内核 2.6.39 起，也可以直接连接两个端点，即在主动一侧使用 connect()。这是为了支持较新的 Nokia Wireless Modem API，例如 ST-Ericsson U8500 平台中 Nokia Slim Modem 所提供的那一套：)

  struct sockaddr_spn spn;
  int fd;

  fd = socket(PF_PHONET, SOCK_SEQPACKET, PN_PROTO_PIPE);
  memset(&spn, 0, sizeof(spn));
  spn.spn_family = AF_PHONET;
  spn.spn_obj = ...;
  spn.spn_dev = ...;
  spn.spn_resource = 0xD9;
  connect(fd, (struct sockaddr *)&spn, sizeof(spn));
  /* normal I/O here ... */
  close(fd);


.. Warning:

   When polling a connected pipe socket for writability, there is an
   intrinsic race condition whereby writability might be lost between the
   polling and the writing system calls. In this case, the socket will
   block until write becomes possible again, unless non-blocking mode
   is enabled.
   (在轮询已连接管道套接字的可写性时，存在一种内在的竞态条件：在轮询与写入这两个系统调用之间，可写性可能会丢失。在这种情况下，除非启用了非阻塞模式，否则套接字将阻塞，直到再次可以写入。)


The pipe protocol provides two socket options at the SOL_PNPIPE level:
(管道协议在 SOL_PNPIPE 层级提供两个套接字选项：)

  PNPIPE_ENCAP accepts one integer value (int) of:
  (PNPIPE_ENCAP 接受一个整数值 (int)，取值如下：)

    PNPIPE_ENCAP_NONE:
    (PNPIPE_ENCAP_NONE：)

      The socket operates normally (default).
      (套接字正常工作（默认）。)

    PNPIPE_ENCAP_IP:
    (PNPIPE_ENCAP_IP：)

      The socket is used as a backend for a virtual IP
      interface. This requires CAP_NET_ADMIN capability. GPRS data
      support on Nokia modems can use this. Note that the socket cannot
      be reliably poll()'d or read() from while in this mode.
      (套接字用作虚拟 IP 接口的后端。这需要 CAP_NET_ADMIN 能力。Nokia 调制解调器上的 GPRS 数据支持可以使用此模式。注意，在此模式下无法可靠地对套接字进行 poll() 或 read()。)

  PNPIPE_IFINDEX
  (PNPIPE_IFINDEX：)

      is a read-only integer value. It contains the
      interface index of the network interface created by PNPIPE_ENCAP,
      or zero if encapsulation is off.
      (是一个只读整数值。它包含由 PNPIPE_ENCAP 创建的网络接口的接口索引，如果封装未开启则为零。)

  PNPIPE_HANDLE
  (PNPIPE_HANDLE：)

      is a read-only integer value. It contains the underlying
      identifier ("pipe handle") of the pipe. This is only defined for
      socket descriptors that are already connected or being connected.
      (是一个只读整数值。它包含管道的底层标识符（"pipe handle"）。它只对已经连接或正在连接的套接字描述符才有定义。)


Authors
(作者)
-------

Linux Phonet was initially written by Sakari Ailus.
(Linux Phonet 最初由 Sakari Ailus 编写。)

Other contributors include Mikä Liljeberg, Andras Domokos,
Carlos Chinea and Rémi Denis-Courmont.
(其他贡献者包括 Mikä Liljeberg、Andras Domokos、Carlos Chinea 和 Rémi Denis-Courmont。)

Copyright |copy| 2008 Nokia Corporation.
(版权所有 |copy| 2008 Nokia Corporation。)
