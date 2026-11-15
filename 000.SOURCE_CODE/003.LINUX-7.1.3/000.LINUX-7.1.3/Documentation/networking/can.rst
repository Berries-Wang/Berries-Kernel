===================================
SocketCAN - Controller Area Network
===================================

(SocketCAN - 控制器局域网)

Overview / What is SocketCAN
(概述 / 什么是 SocketCAN)
============================

The socketcan package is an implementation of CAN protocols
(Controller Area Network) for Linux.  CAN is a networking technology
which has widespread use in automation, embedded devices, and
automotive fields.  While there have been other CAN implementations
for Linux based on character devices, SocketCAN uses the Berkeley
socket API, the Linux network stack and implements the CAN device
drivers as network interfaces.  The CAN socket API has been designed
as similar as possible to the TCP/IP protocols to allow programmers,
familiar with network programming, to easily learn how to use CAN
sockets.
(socketcan 软件包是 Linux 上 CAN 协议（控制器局域网，Controller Area
Network）的一种实现。CAN 是一种在自动化、嵌入式设备和汽车领域广泛使用的
网络技术。虽然此前已有其他基于字符设备的 Linux CAN 实现，但 SocketCAN
使用 Berkeley 套接字 API 和 Linux 网络栈，并将 CAN 设备驱动实现为网络
接口。CAN 套接字 API 在设计上尽可能与 TCP/IP 协议相似，以便熟悉网络编程
的程序员能够轻松学会如何使用 CAN 套接字。)


.. _socketcan-motivation:

Motivation / Why Using the Socket API
(动机 / 为什么使用套接字 API)
=====================================

There have been CAN implementations for Linux before SocketCAN so the
question arises, why we have started another project.  Most existing
implementations come as a device driver for some CAN hardware, they
are based on character devices and provide comparatively little
functionality.  Usually, there is only a hardware-specific device
driver which provides a character device interface to send and
receive raw CAN frames, directly to/from the controller hardware.
Queueing of frames and higher-level transport protocols like ISO-TP
have to be implemented in user space applications.  Also, most
character-device implementations support only one single process to
open the device at a time, similar to a serial interface.  Exchanging
the CAN controller requires employment of another device driver and
often the need for adaption of large parts of the application to the
new driver's API.
(在 SocketCAN 之前，Linux 上已经存在一些 CAN 实现，于是问题来了：为什么
我们要启动另一个项目。大多数现有实现以针对某种 CAN 硬件的设备驱动形式
出现，它们基于字符设备，提供的功能相对有限。通常只有一个硬件专用的设备
驱动，它提供字符设备接口，直接与控制器硬件之间收发原始 CAN 帧。帧的排队
以及 ISO-TP 之类的高层传输协议都必须在用户空间应用程序中实现。此外，大多
数字符设备实现一次只支持一个进程打开设备，类似于串行接口。更换 CAN 控制
器需要启用另一个设备驱动，而且往往需要修改应用程序的大部分代码以适配新
驱动的 API。)

SocketCAN was designed to overcome all of these limitations.  A new
protocol family has been implemented which provides a socket interface
to user space applications and which builds upon the Linux network
layer, enabling use all of the provided queueing functionality.  A device
driver for CAN controller hardware registers itself with the Linux
network layer as a network device, so that CAN frames from the
controller can be passed up to the network layer and on to the CAN
protocol family module and also vice-versa.  Also, the protocol family
module provides an API for transport protocol modules to register, so
that any number of transport protocols can be loaded or unloaded
dynamically.  In fact, the can core module alone does not provide any
protocol and cannot be used without loading at least one additional
protocol module.  Multiple sockets can be opened at the same time,
on different or the same protocol module and they can listen/send
frames on different or the same CAN IDs.  Several sockets listening on
the same interface for frames with the same CAN ID are all passed the
same received matching CAN frames.  An application wishing to
communicate using a specific transport protocol, e.g. ISO-TP, just
selects that protocol when opening the socket, and then can read and
write application data byte streams, without having to deal with
CAN-IDs, frames, etc.
(SocketCAN 的设计目的就是克服所有这些局限。我们实现了一个新的协议族，它为
用户空间应用程序提供套接字接口，并构建在 Linux 网络层之上，从而可以利用
全部已有的排队功能。CAN 控制器硬件的设备驱动以网络设备的形式向 Linux 网
络层注册，这样来自控制器的 CAN 帧可以向上传递到网络层，再传递到 CAN 协议
族模块，反之亦然。此外，协议族模块还为传输协议模块提供了注册 API，因此任
意多个传输协议都可以动态加载或卸载。事实上，can 核心模块本身不提供任何协
议，若不加载至少一个额外的协议模块就无法使用。可以同时打开多个套接字，它
们可以位于不同或相同的协议模块上，并在不同或相同的 CAN ID 上监听/发送帧。
在同一接口上监听相同 CAN ID 帧的多个套接字都会收到相同的、匹配的已接收
CAN 帧。希望使用特定传输协议（例如 ISO-TP）进行通信的应用程序，只需在打
开套接字时选择该协议，然后就可以读写应用数据字节流，而无需处理 CAN-ID、
帧等细节。)

Similar functionality visible from user-space could be provided by a
character device, too, but this would lead to a technically inelegant
solution for a couple of reasons:
(从用户空间来看，类似的功能也可以由字符设备提供，但出于以下几个原因，这会
是一种技术上不优雅的解决方案：)

* **Intricate usage:**  Instead of passing a protocol argument to
  socket(2) and using bind(2) to select a CAN interface and CAN ID, an
  application would have to do all these operations using ioctl(2)s.
  (用法繁杂：与其向 socket(2) 传递协议参数并用 bind(2) 来选择 CAN 接口和
  CAN ID，应用程序将不得不使用 ioctl(2) 来完成所有这些操作。)

* **Code duplication:**  A character device cannot make use of the Linux
  network queueing code, so all that code would have to be duplicated
  for CAN networking.
  (代码重复：字符设备无法利用 Linux 的网络排队代码，因此所有这些代码都得
  在 CAN 网络中重复实现。)

* **Abstraction:**  In most existing character-device implementations, the
  hardware-specific device driver for a CAN controller directly
  provides the character device for the application to work with.
  This is at least very unusual in Unix systems for both, char and
  block devices.  For example you don't have a character device for a
  certain UART of a serial interface, a certain sound chip in your
  computer, a SCSI or IDE controller providing access to your hard
  disk or tape streamer device.  Instead, you have abstraction layers
  which provide a unified character or block device interface to the
  application on the one hand, and a interface for hardware-specific
  device drivers on the other hand.  These abstractions are provided
  by subsystems like the tty layer, the audio subsystem or the SCSI
  and IDE subsystems for the devices mentioned above.
  (抽象缺失：在大多数现有的字符设备实现中，CAN 控制器的硬件专用设备驱动
  直接提供供应用程序使用的字符设备。这在 Unix 系统中无论对字符设备还是块
  设备来说都是非常罕见的。例如，你不会为串行接口的某个 UART、计算机中的
  某个声音芯片、提供硬盘或磁带机访问的某个 SCSI 或 IDE 控制器去单独准备
  一个字符设备。相反，你拥有的是抽象层：一方面向应用程序提供统一的字符或
  块设备接口，另一方面为硬件专用设备驱动提供接口。这些抽象由诸如 tty 层、
  音频子系统或 SCSI 与 IDE 子系统等子系统为上述设备提供。)

  The easiest way to implement a CAN device driver is as a character
  device without such a (complete) abstraction layer, as is done by most
  existing drivers.  The right way, however, would be to add such a
  layer with all the functionality like registering for certain CAN
  IDs, supporting several open file descriptors and (de)multiplexing
  CAN frames between them, (sophisticated) queueing of CAN frames, and
  providing an API for device drivers to register with.  However, then
  it would be no more difficult, or may be even easier, to use the
  networking framework provided by the Linux kernel, and this is what
  SocketCAN does.
  (实现 CAN 设备驱动最简单的方式是像大多数现有驱动那样，把它做成一个没有
  这种（完整）抽象层的字符设备。然而，正确的做法是添加这样一个抽象层，它
  具备注册特定 CAN ID、支持多个打开的文件描述符并在它们之间（分）复用 CAN
  帧、（精细的）CAN 帧排队以及为设备驱动提供注册 API 等全部功能。不过，到
  那时再使用 Linux 内核提供的网络框架并不会更难，甚至可能更容易——而这也
  正是 SocketCAN 所做的。)

The use of the networking framework of the Linux kernel is just the
natural and most appropriate way to implement CAN for Linux.
(使用 Linux 内核的网络框架，恰恰是在 Linux 上实现 CAN 最自然、最合适的
方式。)


.. _socketcan-concept:

SocketCAN Concept
(SocketCAN 概念)
=================

As described in :ref:`socketcan-motivation` the main goal of SocketCAN is to
provide a socket interface to user space applications which builds
upon the Linux network layer. In contrast to the commonly known
TCP/IP and ethernet networking, the CAN bus is a broadcast-only(!)
medium that has no MAC-layer addressing like ethernet. The CAN-identifier
(can_id) is used for arbitration on the CAN-bus. Therefore the CAN-IDs
have to be chosen uniquely on the bus. When designing a CAN-ECU
network the CAN-IDs are mapped to be sent by a specific ECU.
For this reason a CAN-ID can be treated best as a kind of source address.
(如 :ref:`socketcan-motivation` 中所述，SocketCAN 的主要目标是为用户空间
应用程序提供一个构建在 Linux 网络层之上的套接字接口。与广为人知的 TCP/IP
和以太网网络不同，CAN 总线是一种仅支持广播（！）的介质，没有像以太网那样
的 MAC 层寻址。CAN 标识符（can_id）用于 CAN 总线上的仲裁，因此 CAN-ID 必须
在总线上唯一选定。在设计 CAN-ECU 网络时，CAN-ID 会被映射为由特定 ECU 发送。
因此，最好把 CAN-ID 看作一种源地址。)


.. _socketcan-receive-lists:

Receive Lists
(接收列表)
-------------

The network transparent access of multiple applications leads to the
problem that different applications may be interested in the same
CAN-IDs from the same CAN network interface. The SocketCAN core
module - which implements the protocol family CAN - provides several
high efficient receive lists for this reason. If e.g. a user space
application opens a CAN RAW socket, the raw protocol module itself
requests the (range of) CAN-IDs from the SocketCAN core that are
requested by the user. The subscription and unsubscription of
CAN-IDs can be done for specific CAN interfaces or for all(!) known
CAN interfaces with the can_rx_(un)register() functions provided to
CAN protocol modules by the SocketCAN core (see :ref:`socketcan-core-module`).
To optimize the CPU usage at runtime the receive lists are split up
into several specific lists per device that match the requested
filter complexity for a given use-case.
(多应用程序的网络透明访问带来一个问题：不同的应用程序可能对来自同一个 CAN
网络接口的相同 CAN-ID 感兴趣。为此，实现了 CAN 协议族的 SocketCAN 核心模
块提供了若干高效的接收列表。例如，当用户空间应用程序打开一个 CAN RAW 套
接字时，raw 协议模块会自行向 SocketCAN 核心请求用户所需的（范围内的）
CAN-ID。CAN-ID 的订阅与退订既可以针对特定 CAN 接口，也可以针对所有（！）
已知的 CAN 接口，这通过 SocketCAN 核心提供给 CAN 协议模块的
can_rx_(un)register() 函数完成（见 :ref:`socketcan-core-module`）。为了优
化运行时的 CPU 占用，接收列表会按设备拆分为多个特定列表，以匹配给定用例
所需的过滤复杂度。)


.. _socketcan-local-loopback1:

Local Loopback of Sent Frames
(发送帧的本地回环)
-----------------------------

As known from other networking concepts the data exchanging
applications may run on the same or different nodes without any
change (except for the according addressing information):
(正如从其他网络概念中所知的那样，交换数据的应用程序可以运行在相同或不同
的节点上而无需任何改动（除了相应的寻址信息）：)

.. code::

	 ___   ___   ___                   _______   ___
	| _ | | _ | | _ |                 | _   _ | | _ |
	||A|| ||B|| ||C||                 ||A| |B|| ||C||
	|___| |___| |___|                 |_______| |___|
	  |     |     |                       |       |
	-----------------(1)- CAN bus -(2)---------------

To ensure that application A receives the same information in the
example (2) as it would receive in example (1) there is need for
some kind of local loopback of the sent CAN frames on the appropriate
node.
(为了确保示例 (2) 中的应用程序 A 收到的信息与它在示例 (1) 中收到的相同，
需要在相应节点上对已发送的 CAN 帧进行某种本地回环。)

The Linux network devices (by default) just can handle the
transmission and reception of media dependent frames. Due to the
arbitration on the CAN bus the transmission of a low prio CAN-ID
may be delayed by the reception of a high prio CAN frame. To
reflect the correct [#f1]_ traffic on the node the loopback of the sent
data has to be performed right after a successful transmission. If
the CAN network interface is not capable of performing the loopback for
some reason the SocketCAN core can do this task as a fallback solution.
See :ref:`socketcan-local-loopback2` for details (recommended).
(Linux 网络设备（默认情况下）只能处理与介质相关的帧的发送和接收。由于 CAN
总线上的仲裁机制，低优先级 CAN-ID 的发送可能因高优先级 CAN 帧的接收而被
延迟。为了在节点上反映正确的 [#f1]_ 流量，已发送数据的回环必须在成功发送
之后立即执行。如果 CAN 网络接口因某些原因无法执行回环，SocketCAN 核心可以
作为后备方案来完成这项任务。详情（推荐做法）见
:ref:`socketcan-local-loopback2`。)

The loopback functionality is enabled by default to reflect standard
networking behaviour for CAN applications. Due to some requests from
the RT-SocketCAN group the loopback optionally may be disabled for each
separate socket. See sockopts from the CAN RAW sockets in :ref:`socketcan-raw-sockets`.
(回环功能默认启用，以反映 CAN 应用程序的标准网络行为。应 RT-SocketCAN 团
队的一些请求，回环也可以选择针对每个单独的套接字禁用。参见
:ref:`socketcan-raw-sockets` 中 CAN RAW 套接字的 sockopts。)

.. [#f1] you really like to have this when you're running analyser
       tools like 'candump' or 'cansniffer' on the (same) node.
       (当你在（同一个）节点上运行 'candump' 或 'cansniffer' 之类的分析
       工具时，你确实会希望有这个功能。)


.. _socketcan-network-problem-notifications:

Network Problem Notifications
(网络问题通知)
-----------------------------

The use of the CAN bus may lead to several problems on the physical
and media access control layer. Detecting and logging of these lower
layer problems is a vital requirement for CAN users to identify
hardware issues on the physical transceiver layer as well as
arbitration problems and error frames caused by the different
ECUs. The occurrence of detected errors are important for diagnosis
and have to be logged together with the exact timestamp. For this
reason the CAN interface driver can generate so called Error Message
Frames that can optionally be passed to the user application in the
same way as other CAN frames. Whenever an error on the physical layer
or the MAC layer is detected (e.g. by the CAN controller) the driver
creates an appropriate error message frame. Error messages frames can
be requested by the user application using the common CAN filter
mechanisms. Inside this filter definition the (interested) type of
errors may be selected. The reception of error messages is disabled
by default. The format of the CAN error message frame is briefly
described in the Linux header file "include/uapi/linux/can/error.h".
(使用 CAN 总线可能会在物理层和介质访问控制层引发若干问题。检测并记录这些
底层问题，对 CAN 用户来说是识别物理收发器层硬件故障以及不同 ECU 引发的仲
裁问题和错误帧的关键需求。检测到的错误的发生情况对诊断非常重要，必须与精
确的时间戳一起记录。为此，CAN 接口驱动可以生成所谓的错误消息帧（Error
Message Frames），它们可以选择性地以与其他 CAN 帧相同的方式传递给用户应用
程序。每当检测到物理层或 MAC 层上的错误（例如由 CAN 控制器检测到）时，驱
动就会创建相应的错误消息帧。用户应用程序可以使用通用的 CAN 过滤器机制来
请求错误消息帧。在这个过滤器定义中，可以选择（感兴趣的）错误类型。错误消
息的接收默认是禁用的。CAN 错误消息帧的格式在 Linux 头文件
"include/uapi/linux/can/error.h" 中有简要描述。)


How to use SocketCAN
(如何使用 SocketCAN)
====================

Like TCP/IP, you first need to open a socket for communicating over a
CAN network. Since SocketCAN implements a new protocol family, you
need to pass PF_CAN as the first argument to the socket(2) system
call. Currently, there are two CAN protocols to choose from, the raw
socket protocol and the broadcast manager (BCM). So to open a socket,
you would write::
(与 TCP/IP 一样，你首先需要打开一个套接字以便通过 CAN 网络进行通信。由于
SocketCAN 实现了一个新的协议族，你需要将 PF_CAN 作为第一个参数传递给
socket(2) 系统调用。目前有两种 CAN 协议可供选择：raw 套接字协议和广播管
理器（BCM）。要打开套接字，你可以这样写：)

    s = socket(PF_CAN, SOCK_RAW, CAN_RAW);

and::
(或：)

    s = socket(PF_CAN, SOCK_DGRAM, CAN_BCM);

respectively.  After the successful creation of the socket, you would
normally use the bind(2) system call to bind the socket to a CAN
interface (which is different from TCP/IP due to different addressing
- see :ref:`socketcan-concept`). After binding (CAN_RAW) or connecting (CAN_BCM)
the socket, you can read(2) and write(2) from/to the socket or use
send(2), sendto(2), sendmsg(2) and the recv* counterpart operations
on the socket as usual. There are also CAN specific socket options
described below.
(成功创建套接字之后，通常需要使用 bind(2) 系统调用把套接字绑定到一个 CAN
接口上（由于寻址方式不同，这与 TCP/IP 不同——见 :ref:`socketcan-concept`）。
在绑定（CAN_RAW）或连接（CAN_BCM）套接字之后，就可以照常在套接字上进行
read(2)、write(2)，或者使用 send(2)、sendto(2)、sendmsg(2) 以及对应的 recv*
系列操作。此外还有一些 CAN 特有的套接字选项，将在下文描述。)

The Classical CAN frame structure (aka CAN 2.0B), the CAN FD frame structure
and the sockaddr structure are defined in include/linux/can.h:
(经典 CAN 帧结构（又称 CAN 2.0B）、CAN FD 帧结构和 sockaddr 结构都定义在
include/linux/can.h 中：)

.. code-block:: C

    struct can_frame {
            canid_t can_id;  /* 32 bit CAN_ID + EFF/RTR/ERR flags */
            union {
                    /* CAN frame payload length in byte (0 .. CAN_MAX_DLEN)
                     * was previously named can_dlc so we need to carry that
                     * name for legacy support
                     */
                    __u8 len;
                    __u8 can_dlc; /* deprecated */
            };
            __u8    __pad;   /* padding */
            __u8    __res0;  /* reserved / padding */
            __u8    len8_dlc; /* optional DLC for 8 byte payload length (9 .. 15) */
            __u8    data[8] __attribute__((aligned(8)));
    };

Remark: The len element contains the payload length in bytes and should be
used instead of can_dlc. The deprecated can_dlc was misleadingly named as
it always contained the plain payload length in bytes and not the so called
'data length code' (DLC).
(注意：len 元素包含以字节为单位的载荷长度，应当用它来取代 can_dlc。已弃用
的 can_dlc 命名有误导性，因为它始终包含的是以字节为单位的纯载荷长度，而不
是所谓的"数据长度码"（DLC）。)

To pass the raw DLC from/to a Classical CAN network device the len8_dlc
element can contain values 9 .. 15 when the len element is 8 (the real
payload length for all DLC values greater or equal to 8).
(为了在经典 CAN 网络设备之间传递原始 DLC，当 len 元素为 8 时（对所有大于
等于 8 的 DLC 值而言，8 就是真实的载荷长度），len8_dlc 元素可以取 9 .. 15
之间的值。)

The alignment of the (linear) payload data[] to a 64bit boundary
allows the user to define their own structs and unions to easily access
the CAN payload. There is no given byteorder on the CAN bus by
default. A read(2) system call on a CAN_RAW socket transfers a
struct can_frame to the user space.
(（线性）载荷 data[] 按 64 位边界对齐，允许用户定义自己的结构体和联合体来
方便地访问 CAN 载荷。默认情况下，CAN 总线上没有给定的字节序。在 CAN_RAW
套接字上调用 read(2) 系统调用会把一个 struct can_frame 传输到用户空间。)

The sockaddr_can structure has an interface index like the
PF_PACKET socket, that also binds to a specific interface:
(sockaddr_can 结构像 PF_PACKET 套接字一样带有接口索引，同样可以绑定到特
定接口：)

.. code-block:: C

    struct sockaddr_can {
            sa_family_t can_family;
            int         can_ifindex;
            union {
                    /* transport protocol class address info (e.g. ISOTP) */
                    struct { canid_t rx_id, tx_id; } tp;

                    /* J1939 address information */
                    struct {
                            /* 8 byte name when using dynamic addressing */
                            __u64 name;

                            /* pgn:
                             * 8 bit: PS in PDU2 case, else 0
                             * 8 bit: PF
                             * 1 bit: DP
                             * 1 bit: reserved
                             */
                            __u32 pgn;

                            /* 1 byte address */
                            __u8 addr;
                    } j1939;

                    /* reserved for future CAN protocols address information */
            } can_addr;
    };

To determine the interface index an appropriate ioctl() has to
be used (example for CAN_RAW sockets without error checking):
(要确定接口索引，必须使用相应的 ioctl()（以下为 CAN_RAW 套接字的示例，未
做错误检查）：)

.. code-block:: C

    int s;
    struct sockaddr_can addr;
    struct ifreq ifr;

    s = socket(PF_CAN, SOCK_RAW, CAN_RAW);

    strcpy(ifr.ifr_name, "can0" );
    ioctl(s, SIOCGIFINDEX, &ifr);

    addr.can_family = AF_CAN;
    addr.can_ifindex = ifr.ifr_ifindex;

    bind(s, (struct sockaddr *)&addr, sizeof(addr));

    (..)

To bind a socket to all(!) CAN interfaces the interface index must
be 0 (zero). In this case the socket receives CAN frames from every
enabled CAN interface. To determine the originating CAN interface
the system call recvfrom(2) may be used instead of read(2). To send
on a socket that is bound to 'any' interface sendto(2) is needed to
specify the outgoing interface.
(要把套接字绑定到所有（！）CAN 接口，接口索引必须为 0（零）。此时套接字会
从每个已启用的 CAN 接口接收 CAN 帧。要确定 CAN 帧来自哪个接口，可以用系
统调用 recvfrom(2) 代替 read(2)。要在绑定到 'any'（任意）接口的套接字上发
送，需要用 sendto(2) 来指定输出接口。)

Reading CAN frames from a bound CAN_RAW socket (see above) consists
of reading a struct can_frame:
(从已绑定的 CAN_RAW 套接字（见上文）读取 CAN 帧，就是读取一个 struct
can_frame：)

.. code-block:: C

    struct can_frame frame;

    nbytes = read(s, &frame, sizeof(struct can_frame));

    if (nbytes < 0) {
            perror("can raw socket read");
            return 1;
    }

    /* paranoid check ... */
    if (nbytes < sizeof(struct can_frame)) {
            fprintf(stderr, "read: incomplete CAN frame\n");
            return 1;
    }

    /* do something with the received CAN frame */

Writing CAN frames can be done similarly, with the write(2) system call::
(发送 CAN 帧可以用类似的方式完成，即使用 write(2) 系统调用：)

    nbytes = write(s, &frame, sizeof(struct can_frame));

When the CAN interface is bound to 'any' existing CAN interface
(addr.can_ifindex = 0) it is recommended to use recvfrom(2) if the
information about the originating CAN interface is needed:
(当 CAN 接口绑定到任何现存 CAN 接口（addr.can_ifindex = 0）时，如果需要
关于发起 CAN 接口的信息，建议使用 recvfrom(2)：)

.. code-block:: C

    struct sockaddr_can addr;
    struct ifreq ifr;
    socklen_t len = sizeof(addr);
    struct can_frame frame;

    nbytes = recvfrom(s, &frame, sizeof(struct can_frame),
                      0, (struct sockaddr*)&addr, &len);

    /* get interface name of the received CAN frame */
    ifr.ifr_ifindex = addr.can_ifindex;
    ioctl(s, SIOCGIFNAME, &ifr);
    printf("Received a CAN frame from interface %s", ifr.ifr_name);

To write CAN frames on sockets bound to 'any' CAN interface the
outgoing interface has to be defined certainly:
(要在绑定到 'any' CAN 接口的套接字上写入 CAN 帧，必须明确指定输出接口：)

.. code-block:: C

    strcpy(ifr.ifr_name, "can0");
    ioctl(s, SIOCGIFINDEX, &ifr);
    addr.can_ifindex = ifr.ifr_ifindex;
    addr.can_family  = AF_CAN;

    nbytes = sendto(s, &frame, sizeof(struct can_frame),
                    0, (struct sockaddr*)&addr, sizeof(addr));

An accurate timestamp can be obtained with an ioctl(2) call after reading
a message from the socket:
(从套接字读取一条消息之后，可以通过 ioctl(2) 调用获得精确的时间戳：)

.. code-block:: C

    struct timeval tv;
    ioctl(s, SIOCGSTAMP, &tv);

The timestamp has a resolution of one microsecond and is set automatically
at the reception of a CAN frame.
(时间戳的分辨率为一微秒，并在接收到 CAN 帧时自动设置。)

Remark about CAN FD (flexible data rate) support:
(关于 CAN FD（灵活数据速率）支持的说明：)

Generally the handling of CAN FD is very similar to the formerly described
examples. The new CAN FD capable CAN controllers support two different
bitrates for the arbitration phase and the payload phase of the CAN FD frame
and up to 64 bytes of payload. This extended payload length breaks all the
kernel interfaces (ABI) which heavily rely on the CAN frame with fixed eight
bytes of payload (struct can_frame) like the CAN_RAW socket. Therefore e.g.
the CAN_RAW socket supports a new socket option CAN_RAW_FD_FRAMES that
switches the socket into a mode that allows the handling of CAN FD frames
and Classical CAN frames simultaneously (see :ref:`socketcan-rawfd`).
(一般来说，CAN FD 的处理方式与前面描述的示例非常相似。新的支持 CAN FD 的
CAN 控制器为 CAN FD 帧的仲裁阶段和载荷阶段支持两种不同的位速率，且载荷最
多可达 64 字节。这种扩展的载荷长度会破坏所有严重依赖固定 8 字节载荷 CAN 帧
（struct can_frame）的内核接口（ABI），例如 CAN_RAW 套接字。因此，例如
CAN_RAW 套接字支持一个新的套接字选项 CAN_RAW_FD_FRAMES，它把套接字切换到
一种可以同时处理 CAN FD 帧和经典 CAN 帧的模式（见 :ref:`socketcan-rawfd`）。)

The struct canfd_frame is defined in include/linux/can.h:
(struct canfd_frame 定义在 include/linux/can.h 中：)

.. code-block:: C

    struct canfd_frame {
            canid_t can_id;  /* 32 bit CAN_ID + EFF/RTR/ERR flags */
            __u8    len;     /* frame payload length in byte (0 .. 64) */
            __u8    flags;   /* additional flags for CAN FD */
            __u8    __res0;  /* reserved / padding */
            __u8    __res1;  /* reserved / padding */
            __u8    data[64] __attribute__((aligned(8)));
    };

The struct canfd_frame and the existing struct can_frame have the can_id,
the payload length and the payload data at the same offset inside their
structures. This allows to handle the different structures very similar.
When the content of a struct can_frame is copied into a struct canfd_frame
all structure elements can be used as-is - only the data[] becomes extended.
(struct canfd_frame 与现有的 struct can_frame 在各自结构内的相同偏移处保存
can_id、载荷长度和载荷数据。这使得可以非常相似地处理这两种不同的结构。当
把 struct can_frame 的内容复制到 struct canfd_frame 中时，所有结构元素都
可以原样使用——只有 data[] 会被扩展。)

When introducing the struct canfd_frame it turned out that the data length
code (DLC) of the struct can_frame was used as a length information as the
length and the DLC has a 1:1 mapping in the range of 0 .. 8. To preserve
the easy handling of the length information the canfd_frame.len element
contains a plain length value from 0 .. 64. So both canfd_frame.len and
can_frame.len are equal and contain a length information and no DLC.
For details about the distinction of CAN and CAN FD capable devices and
the mapping to the bus-relevant data length code (DLC), see :ref:`socketcan-can-fd-driver`.
(在引入 struct canfd_frame 时发现，struct can_frame 的数据长度码（DLC）被
当作长度信息使用，因为长度与 DLC 在 0 .. 8 范围内是一一对应的。为了保持长
度信息易于处理的特性，canfd_frame.len 元素包含 0 .. 64 的纯长度值。因此
canfd_frame.len 和 can_frame.len 是相同的，都包含长度信息而非 DLC。关于
CAN 与支持 CAN FD 的设备之间的区分以及到总线相关数据长度码（DLC）的映射
的详细信息，见 :ref:`socketcan-can-fd-driver`。)

The length of the two CAN(FD) frame structures define the maximum transfer
unit (MTU) of the CAN(FD) network interface and skbuff data length. Two
definitions are specified for CAN specific MTUs in include/linux/can.h:
(两种 CAN(FD) 帧结构的长度定义了 CAN(FD) 网络接口的最大传输单元（MTU）以
及 skbuff 数据长度。CAN 专用 MTU 在 include/linux/can.h 中有两个定义：)

.. code-block:: C

  #define CAN_MTU   (sizeof(struct can_frame))   == 16  => Classical CAN frame
  #define CANFD_MTU (sizeof(struct canfd_frame)) == 72  => CAN FD frame


Returned Message Flags
(返回的消息标志)
----------------------

When using the system call recvmsg(2) on a RAW or a BCM socket, the
msg->msg_flags field may contain the following flags:
(在 RAW 或 BCM 套接字上使用系统调用 recvmsg(2) 时，msg->msg_flags 字段可能
包含以下标志：)

MSG_DONTROUTE:
(MSG_DONTROUTE:)

	set when the received frame was created on the local host.
	(当接收到的帧是在本地主机上创建时设置。)

MSG_CONFIRM:
(MSG_CONFIRM:)

	set when the frame was sent via the socket it is received on.
	This flag can be interpreted as a 'transmission confirmation' when the
	CAN driver supports the echo of frames on driver level, see
	:ref:`socketcan-local-loopback1` and :ref:`socketcan-local-loopback2`.
	(Note: In order to receive such messages on a RAW socket,
	CAN_RAW_RECV_OWN_MSGS must be set.)
	(当该帧是通过接收它的这个套接字发送出去时设置。当 CAN 驱动支持驱动层的
	帧回显时，此标志可以解释为"传输确认"，见 :ref:`socketcan-local-loopback1`
	和 :ref:`socketcan-local-loopback2`。（注意：要在 RAW 套接字上接收此类
	消息，必须设置 CAN_RAW_RECV_OWN_MSGS。）)


.. _socketcan-raw-sockets:

RAW Protocol Sockets with can_filters (SOCK_RAW)
(带 can_filters 的 RAW 协议套接字（SOCK_RAW）)
------------------------------------------------

Using CAN_RAW sockets is extensively comparable to the commonly
known access to CAN character devices. To meet the new possibilities
provided by the multi user SocketCAN approach, some reasonable
defaults are set at RAW socket binding time:
(使用 CAN_RAW 套接字在很大程度上与人们熟知的 CAN 字符设备访问方式类似。
为了满足多用户 SocketCAN 方式带来的新可能性，在 RAW 套接字绑定时会设置一
些合理的默认值：)

- The filters are set to exactly one filter receiving everything
  (过滤器被设置为恰好一个接收所有内容的过滤器)
- The socket only receives valid data frames (=> no error message frames)
  (套接字只接收有效的数据帧（=> 不接收错误消息帧）)
- The loopback of sent CAN frames is enabled (see :ref:`socketcan-local-loopback2`)
  (已启用已发送 CAN 帧的回环（见 :ref:`socketcan-local-loopback2`）)
- The socket does not receive its own sent frames (in loopback mode)
  (套接字不接收它自己发送的帧（在回环模式下）)

These default settings may be changed before or after binding the socket.
To use the referenced definitions of the socket options for CAN_RAW
sockets, include <linux/can/raw.h>.
(这些默认设置可以在绑定套接字之前或之后更改。要使用 CAN_RAW 套接字选项的
相关定义，请包含 <linux/can/raw.h>。)


.. _socketcan-rawfilter:

RAW socket option CAN_RAW_FILTER
(RAW 套接字选项 CAN_RAW_FILTER)
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

The reception of CAN frames using CAN_RAW sockets can be controlled
by defining 0 .. n filters with the CAN_RAW_FILTER socket option.
(通过用 CAN_RAW_FILTER 套接字选项定义 0 .. n 个过滤器，可以控制 CAN_RAW
套接字对 CAN 帧的接收。)

The CAN filter structure is defined in include/linux/can.h:
(CAN 过滤器结构定义在 include/linux/can.h 中：)

.. code-block:: C

    struct can_filter {
            canid_t can_id;
            canid_t can_mask;
    };

A filter matches, when:
(过滤器在满足以下条件时匹配：)

.. code-block:: C

    <received_can_id> & mask == can_id & mask

which is analogous to known CAN controllers hardware filter semantics.
The filter can be inverted in this semantic, when the CAN_INV_FILTER
bit is set in can_id element of the can_filter structure. In
contrast to CAN controller hardware filters the user may set 0 .. n
receive filters for each open socket separately:
(这与已知的 CAN 控制器硬件过滤器语义类似。当在 can_filter 结构的 can_id 元
素中设置 CAN_INV_FILTER 位时，过滤器可以在这种语义下取反。与 CAN 控制器硬
件过滤器不同，用户可以为每个打开的套接字分别设置 0 .. n 个接收过滤器：)

.. code-block:: C

    struct can_filter rfilter[2];

    rfilter[0].can_id   = 0x123;
    rfilter[0].can_mask = CAN_SFF_MASK;
    rfilter[1].can_id   = 0x200;
    rfilter[1].can_mask = 0x700;

    setsockopt(s, SOL_CAN_RAW, CAN_RAW_FILTER, &rfilter, sizeof(rfilter));

To disable the reception of CAN frames on the selected CAN_RAW socket:
(要在所选的 CAN_RAW 套接字上禁用 CAN 帧的接收：)

.. code-block:: C

    setsockopt(s, SOL_CAN_RAW, CAN_RAW_FILTER, NULL, 0);

To set the filters to zero filters is quite obsolete as to not read
data causes the raw socket to discard the received CAN frames. But
having this 'send only' use-case we may remove the receive list in the
Kernel to save a little (really a very little!) CPU usage.
(把过滤器设置为零个过滤器已经相当多余，因为不读取数据就会导致 raw 套接字
丢弃接收到的 CAN 帧。但对于这种"只发送"的用例，我们可以移除内核中的接收
列表，以节省一点（真的非常少！）CPU 占用。)

CAN Filter Usage Optimisation
(CAN 过滤器使用优化)
.............................

The CAN filters are processed in per-device filter lists at CAN frame
reception time. To reduce the number of checks that need to be performed
while walking through the filter lists the CAN core provides an optimized
filter handling when the filter subscription focuses on a single CAN ID.
(CAN 过滤器在 CAN 帧接收时按每设备的过滤器列表进行处理。为了减少遍历过滤
器列表时需要执行的检查次数，当过滤器订阅针对单个 CAN ID 时，CAN 核心提供
了优化的过滤器处理。)

For the possible 2048 SFF CAN identifiers the identifier is used as an index
to access the corresponding subscription list without any further checks.
For the 2^29 possible EFF CAN identifiers a 10 bit XOR folding is used as
hash function to retrieve the EFF table index.
(对于 2048 个可能的 SFF CAN 标识符，该标识符被用作索引来直接访问相应的订
阅列表，无需任何进一步检查。对于 2^29 个可能的 EFF CAN 标识符，则使用 10
位 XOR 折叠作为哈希函数来获取 EFF 表索引。)

To benefit from the optimized filters for single CAN identifiers the
CAN_SFF_MASK or CAN_EFF_MASK have to be set into can_filter.mask together
with set CAN_EFF_FLAG and CAN_RTR_FLAG bits. A set CAN_EFF_FLAG bit in the
can_filter.mask makes clear that it matters whether a SFF or EFF CAN ID is
subscribed. E.g. in the example from above:
(要让单个 CAN 标识符受益于优化的过滤器，必须将 CAN_SFF_MASK 或 CAN_EFF_MASK
设置到 can_filter.mask 中，并同时设置 CAN_EFF_FLAG 和 CAN_RTR_FLAG 位。
can_filter.mask 中设置的 CAN_EFF_FLAG 位明确表示所订阅的是 SFF 还是 EFF
CAN ID 是有影响的。例如上面的示例中：)

.. code-block:: C

    rfilter[0].can_id   = 0x123;
    rfilter[0].can_mask = CAN_SFF_MASK;

both SFF frames with CAN ID 0x123 and EFF frames with 0xXXXXX123 can pass.
(这样，CAN ID 为 0x123 的 SFF 帧和 CAN ID 为 0xXXXXX123 的 EFF 帧都可以通
过。)

To filter for only 0x123 (SFF) and 0x12345678 (EFF) CAN identifiers the
filter has to be defined in this way to benefit from the optimized filters:
(要只过滤 0x123（SFF）和 0x12345678（EFF）这两个 CAN 标识符，必须按如下方
式定义过滤器才能受益于优化的过滤器：)

.. code-block:: C

    struct can_filter rfilter[2];

    rfilter[0].can_id   = 0x123;
    rfilter[0].can_mask = (CAN_EFF_FLAG | CAN_RTR_FLAG | CAN_SFF_MASK);
    rfilter[1].can_id   = 0x12345678 | CAN_EFF_FLAG;
    rfilter[1].can_mask = (CAN_EFF_FLAG | CAN_RTR_FLAG | CAN_EFF_MASK);

    setsockopt(s, SOL_CAN_RAW, CAN_RAW_FILTER, &rfilter, sizeof(rfilter));


RAW Socket Option CAN_RAW_ERR_FILTER
(RAW 套接字选项 CAN_RAW_ERR_FILTER)
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

As described in :ref:`socketcan-network-problem-notifications` the CAN interface driver can generate so
called Error Message Frames that can optionally be passed to the user
application in the same way as other CAN frames. The possible
errors are divided into different error classes that may be filtered
using the appropriate error mask. To register for every possible
error condition CAN_ERR_MASK can be used as value for the error mask.
The values for the error mask are defined in linux/can/error.h:
(如 :ref:`socketcan-network-problem-notifications` 中所述，CAN 接口驱动可
以生成所谓的错误消息帧（Error Message Frames），它们可以选择性地以与其他
CAN 帧相同的方式传递给用户应用程序。可能的错误被划分为不同的错误类别，可
以使用相应的错误掩码进行过滤。要注册所有可能的错误条件，可以使用
CAN_ERR_MASK 作为错误掩码的值。错误掩码的取值定义在 linux/can/error.h 中：)

.. code-block:: C

    can_err_mask_t err_mask = ( CAN_ERR_TX_TIMEOUT | CAN_ERR_BUSOFF );

    setsockopt(s, SOL_CAN_RAW, CAN_RAW_ERR_FILTER,
               &err_mask, sizeof(err_mask));


RAW Socket Option CAN_RAW_LOOPBACK
(RAW 套接字选项 CAN_RAW_LOOPBACK)
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

To meet multi user needs the local loopback is enabled by default
(see :ref:`socketcan-local-loopback1` for details). But in some embedded use-cases
(e.g. when only one application uses the CAN bus) this loopback
functionality can be disabled (separately for each socket):
(为了满足多用户需求，本地回环默认是启用的（详见
:ref:`socketcan-local-loopback1`）。但在某些嵌入式用例中（例如只有一个应用
程序使用 CAN 总线时），可以为每个套接字单独禁用此回环功能：)

.. code-block:: C

    int loopback = 0; /* 0 = disabled, 1 = enabled (default) */

    setsockopt(s, SOL_CAN_RAW, CAN_RAW_LOOPBACK, &loopback, sizeof(loopback));


RAW socket option CAN_RAW_RECV_OWN_MSGS
(RAW 套接字选项 CAN_RAW_RECV_OWN_MSGS)
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

When the local loopback is enabled, all the sent CAN frames are
looped back to the open CAN sockets that registered for the CAN
frames' CAN-ID on this given interface to meet the multi user
needs. The reception of the CAN frames on the same socket that was
sending the CAN frame is assumed to be unwanted and therefore
disabled by default. This default behaviour may be changed on
demand:
(当本地回环启用时，为了满足多用户需求，所有发送的 CAN 帧都会回环给在该指
定接口上为这些帧的 CAN-ID 注册过的所有已打开的 CAN 套接字。由发送 CAN 帧
的那个套接字本身接收这些 CAN 帧被认为是不需要的，因此默认是禁用的。此默
认行为可以按需更改：)

.. code-block:: C

    int recv_own_msgs = 1; /* 0 = disabled (default), 1 = enabled */

    setsockopt(s, SOL_CAN_RAW, CAN_RAW_RECV_OWN_MSGS,
               &recv_own_msgs, sizeof(recv_own_msgs));

Note that reception of a socket's own CAN frames are subject to the same
filtering as other CAN frames (see :ref:`socketcan-rawfilter`).
(注意，套接字接收其自身的 CAN 帧同样要经过与其他 CAN 帧相同的过滤
（见 :ref:`socketcan-rawfilter`）。)

.. _socketcan-rawfd:

RAW Socket Option CAN_RAW_FD_FRAMES
(RAW 套接字选项 CAN_RAW_FD_FRAMES)
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

CAN FD support in CAN_RAW sockets can be enabled with a new socket option
CAN_RAW_FD_FRAMES which is off by default. When the new socket option is
not supported by the CAN_RAW socket (e.g. on older kernels), switching the
CAN_RAW_FD_FRAMES option returns the error -ENOPROTOOPT.
(CAN_RAW 套接字的 CAN FD 支持可以通过新的套接字选项 CAN_RAW_FD_FRAMES 启
用，该选项默认关闭。如果 CAN_RAW 套接字不支持这个新套接字选项（例如在较旧
的内核上），切换 CAN_RAW_FD_FRAMES 选项会返回错误 -ENOPROTOOPT。)

Once CAN_RAW_FD_FRAMES is enabled the application can send both CAN frames
and CAN FD frames. OTOH the application has to handle CAN and CAN FD frames
when reading from the socket:
(启用 CAN_RAW_FD_FRAMES 后，应用程序既可以发送 CAN 帧也可以发送 CAN FD 帧。
另一方面，应用程序在从套接字读取时必须同时处理 CAN 帧和 CAN FD 帧：)

.. code-block:: C

    CAN_RAW_FD_FRAMES enabled:  CAN_MTU and CANFD_MTU are allowed
    CAN_RAW_FD_FRAMES disabled: only CAN_MTU is allowed (default)

Example:
(示例：)

.. code-block:: C

    [ remember: CANFD_MTU == sizeof(struct canfd_frame) ]

    struct canfd_frame cfd;

    nbytes = read(s, &cfd, CANFD_MTU);

    if (nbytes == CANFD_MTU) {
            printf("got CAN FD frame with length %d\n", cfd.len);
            /* cfd.flags contains valid data */
    } else if (nbytes == CAN_MTU) {
            printf("got Classical CAN frame with length %d\n", cfd.len);
            /* cfd.flags is undefined */
    } else {
            fprintf(stderr, "read: invalid CAN(FD) frame\n");
            return 1;
    }

    /* the content can be handled independently from the received MTU size */

    printf("can_id: %X data length: %d data: ", cfd.can_id, cfd.len);
    for (i = 0; i < cfd.len; i++)
            printf("%02X ", cfd.data[i]);

When reading with size CANFD_MTU only returns CAN_MTU bytes that have
been received from the socket a Classical CAN frame has been read into the
provided CAN FD structure. Note that the canfd_frame.flags data field is
not specified in the struct can_frame and therefore it is only valid in
CANFD_MTU sized CAN FD frames.
(当以 CANFD_MTU 大小读取却只返回从套接字接收到的 CAN_MTU 个字节时，说明读
入所提供的 CAN FD 结构中的是一个经典 CAN 帧。注意，canfd_frame.flags 数据
字段在 struct can_frame 中没有定义，因此它只在 CANFD_MTU 大小的 CAN FD 帧
中有效。)

Implementation hint for new CAN applications:
(针对新 CAN 应用程序的实现提示：)

To build a CAN FD aware application use struct canfd_frame as basic CAN
data structure for CAN_RAW based applications. When the application is
executed on an older Linux kernel and switching the CAN_RAW_FD_FRAMES
socket option returns an error: No problem. You'll get Classical CAN frames
or CAN FD frames and can process them the same way.
(要构建支持 CAN FD 的应用程序，请使用 struct canfd_frame 作为基于 CAN_RAW
的应用程序的基本 CAN 数据结构。当应用程序在较旧的 Linux 内核上运行、切换
CAN_RAW_FD_FRAMES 套接字选项返回错误时：没有问题。你收到的要么是经典 CAN
帧，要么是 CAN FD 帧，可以用相同的方式处理它们。)

When sending to CAN devices make sure that the device is capable to handle
CAN FD frames by checking if the device maximum transfer unit is CANFD_MTU.
The CAN device MTU can be retrieved e.g. with a SIOCGIFMTU ioctl() syscall.
(向 CAN 设备发送时，请通过检查设备的最大传输单元是否为 CANFD_MTU 来确保该
设备能够处理 CAN FD 帧。CAN 设备的 MTU 可以例如通过 SIOCGIFMTU ioctl() 系
统调用获取。)


RAW socket option CAN_RAW_JOIN_FILTERS
(RAW 套接字选项 CAN_RAW_JOIN_FILTERS)
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

The CAN_RAW socket can set multiple CAN identifier specific filters that
lead to multiple filters in the af_can.c filter processing. These filters
are independent from each other which leads to logical OR'ed filters when
applied (see :ref:`socketcan-rawfilter`).
(CAN_RAW 套接字可以设置多个 CAN 标识符特定的过滤器，从而在 af_can.c 的过
滤器处理中形成多个过滤器。这些过滤器彼此独立，应用时表现为逻辑或（OR）的
过滤器（见 :ref:`socketcan-rawfilter`）。)

This socket option joins the given CAN filters in the way that only CAN
frames are passed to user space that matched *all* given CAN filters. The
semantic for the applied filters is therefore changed to a logical AND.
(此套接字选项把给定的 CAN 过滤器以如下方式连接起来：只有匹配 *所有* 给定
CAN 过滤器的 CAN 帧才会传递给用户空间。因此，所应用过滤器的语义变为逻辑
与（AND）。)

This is useful especially when the filterset is a combination of filters
where the CAN_INV_FILTER flag is set in order to notch single CAN IDs or
CAN ID ranges from the incoming traffic.
(当过滤器集是由设置了 CAN_INV_FILTER 标志的过滤器组合而成、用于从传入流量
中剔除单个 CAN ID 或 CAN ID 范围时，此选项尤其有用。)


Broadcast Manager Protocol Sockets (SOCK_DGRAM)
(广播管理器协议套接字（SOCK_DGRAM）)
-----------------------------------------------

The Broadcast Manager protocol provides a command based configuration
interface to filter and send (e.g. cyclic) CAN messages in kernel space.
(广播管理器（Broadcast Manager）协议提供一个基于命令的配置接口，用于在内
核空间中过滤和发送（例如周期性的）CAN 消息。)

Receive filters can be used to down sample frequent messages; detect events
such as message contents changes, packet length changes, and do time-out
monitoring of received messages.
(接收过滤器可用于对频繁出现的消息进行降采样；检测诸如消息内容变化、数据包
长度变化等事件，并对接收到的消息进行超时监视。)

Periodic transmission tasks of CAN frames or a sequence of CAN frames can be
created and modified at runtime; both the message content and the two
possible transmit intervals can be altered.
(CAN 帧或 CAN 帧序列的周期性发送任务可以在运行时创建和修改；消息内容以及
两个可能的发送间隔都可以更改。)

A BCM socket is not intended for sending individual CAN frames using the
struct can_frame as known from the CAN_RAW socket. Instead a special BCM
configuration message is defined. The basic BCM configuration message used
to communicate with the broadcast manager and the available operations are
defined in the linux/can/bcm.h include. The BCM message consists of a
message header with a command ('opcode') followed by zero or more CAN frames.
The broadcast manager sends responses to user space in the same form:
(BCM 套接字并不打算像 CAN_RAW 套接字那样使用 struct can_frame 来发送单个
CAN 帧，而是定义了一种特殊的 BCM 配置消息。用于与广播管理器通信的基本 BCM
配置消息以及可用的操作都定义在 linux/can/bcm.h 包含文件中。BCM 消息由一个
带有命令（'opcode'）的消息头加上零个或多个 CAN 帧组成。广播管理器以相同的
形式向用户空间发送响应：)

.. code-block:: C

    struct bcm_msg_head {
            __u32 opcode;                   /* command */
            __u32 flags;                    /* special flags */
            __u32 count;                    /* run 'count' times with ival1 */
            struct timeval ival1, ival2;    /* count and subsequent interval */
            canid_t can_id;                 /* unique can_id for task */
            __u32 nframes;                  /* number of can_frames following */
            struct can_frame frames[];
    };

The aligned payload 'frames' uses the same basic CAN frame structure defined
at the beginning of :ref:`socketcan-rawfd` and in the include/linux/can.h include. All
messages to the broadcast manager from user space have this structure.
(对齐的载荷 'frames' 使用与 :ref:`socketcan-rawfd` 开头处以及
include/linux/can.h 包含文件中定义的相同的基本 CAN 帧结构。从用户空间发给
广播管理器的所有消息都具有这种结构。)

Note a CAN_BCM socket must be connected instead of bound after socket
creation (example without error checking):
(注意，CAN_BCM 套接字在创建之后必须进行连接（connect）而不是绑定（bind）
（以下示例未做错误检查）：)

.. code-block:: C

    int s;
    struct sockaddr_can addr;
    struct ifreq ifr;

    s = socket(PF_CAN, SOCK_DGRAM, CAN_BCM);

    strcpy(ifr.ifr_name, "can0");
    ioctl(s, SIOCGIFINDEX, &ifr);

    addr.can_family = AF_CAN;
    addr.can_ifindex = ifr.ifr_ifindex;

    connect(s, (struct sockaddr *)&addr, sizeof(addr));

    (..)

The broadcast manager socket is able to handle any number of in flight
transmissions or receive filters concurrently. The different RX/TX jobs are
distinguished by the unique can_id in each BCM message. However additional
CAN_BCM sockets are recommended to communicate on multiple CAN interfaces.
When the broadcast manager socket is bound to 'any' CAN interface (=> the
interface index is set to zero) the configured receive filters apply to any
CAN interface unless the sendto() syscall is used to overrule the 'any' CAN
interface index. When using recvfrom() instead of read() to retrieve BCM
socket messages the originating CAN interface is provided in can_ifindex.
(广播管理器套接字能够并发处理任意数量的在途发送任务或接收过滤器。不同的
RX/TX 任务通过每个 BCM 消息中唯一的 can_id 来区分。不过，在多个 CAN 接口
上通信时建议使用额外的 CAN_BCM 套接字。当广播管理器套接字绑定到 'any'
（任意）CAN 接口（=> 接口索引设为零）时，所配置的接收过滤器适用于任何 CAN
接口，除非使用 sendto() 系统调用来覆盖 'any' CAN 接口索引。当使用
recvfrom() 而不是 read() 来获取 BCM 套接字消息时，发起该消息的 CAN 接口会
在 can_ifindex 中提供。)


Broadcast Manager Operations
(广播管理器操作)
~~~~~~~~~~~~~~~~~~~~~~~~~~~~

The opcode defines the operation for the broadcast manager to carry out,
or details the broadcast managers response to several events, including
user requests.
(opcode 定义广播管理器要执行的操作，或描述广播管理器对若干事件（包括用户
请求）的响应。)

Transmit Operations (user space to broadcast manager):
(发送操作（用户空间到广播管理器）：)

TX_SETUP:
(TX_SETUP:)

	Create (cyclic) transmission task.
	(创建（循环）发送任务。)

TX_DELETE:
(TX_DELETE:)

	Remove (cyclic) transmission task, requires only can_id.
	(移除（循环）发送任务，只需要 can_id。)

TX_READ:
(TX_READ:)

	Read properties of (cyclic) transmission task for can_id.
	(读取 can_id 对应的（循环）发送任务属性。)

TX_SEND:
(TX_SEND:)

	Send one CAN frame.
	(发送一个 CAN 帧。)

Transmit Responses (broadcast manager to user space):
(发送响应（广播管理器到用户空间）：)

TX_STATUS:
(TX_STATUS:)

	Reply to TX_READ request (transmission task configuration).
	(对 TX_READ 请求的应答（发送任务配置）。)

TX_EXPIRED:
(TX_EXPIRED:)

	Notification when counter finishes sending at initial interval
	'ival1'. Requires the TX_COUNTEVT flag to be set at TX_SETUP.
	(计数器按初始间隔 'ival1' 完成发送时的通知。要求在 TX_SETUP 时设置
	TX_COUNTEVT 标志。)

Receive Operations (user space to broadcast manager):
(接收操作（用户空间到广播管理器）：)

RX_SETUP:
(RX_SETUP:)

	Create RX content filter subscription.
	(创建 RX 内容过滤器订阅。)

RX_DELETE:
(RX_DELETE:)

	Remove RX content filter subscription, requires only can_id.
	(移除 RX 内容过滤器订阅，只需要 can_id。)

RX_READ:
(RX_READ:)

	Read properties of RX content filter subscription for can_id.
	(读取 can_id 对应的 RX 内容过滤器订阅属性。)

Receive Responses (broadcast manager to user space):
(接收响应（广播管理器到用户空间）：)

RX_STATUS:
(RX_STATUS:)

	Reply to RX_READ request (filter task configuration).
	(对 RX_READ 请求的应答（过滤器任务配置）。)

RX_TIMEOUT:
(RX_TIMEOUT:)

	Cyclic message is detected to be absent (timer ival1 expired).
	(检测到循环消息缺席（定时器 ival1 超时）。)

RX_CHANGED:
(RX_CHANGED:)

	BCM message with updated CAN frame (detected content change).
	Sent on first message received or on receipt of revised CAN messages.
	(带有更新后 CAN 帧的 BCM 消息（检测到内容变化）。在收到第一条消息时或
	收到修订后的 CAN 消息时发送。)


Broadcast Manager Message Flags
(广播管理器消息标志)
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

When sending a message to the broadcast manager the 'flags' element may
contain the following flag definitions which influence the behaviour:
(向广播管理器发送消息时，'flags' 元素可以包含以下影响行为的标志定义：)

SETTIMER:
(SETTIMER:)

	Set the values of ival1, ival2 and count
	(设置 ival1、ival2 和 count 的值)

STARTTIMER:
(STARTTIMER:)

	Start the timer with the actual values of ival1, ival2
	and count. Starting the timer leads simultaneously to emit a CAN frame.
	(以 ival1、ival2 和 count 的当前值启动定时器。启动定时器会同时导致发出
	一个 CAN 帧。)

TX_COUNTEVT:
(TX_COUNTEVT:)

	Create the message TX_EXPIRED when count expires
	(当 count 到期时创建 TX_EXPIRED 消息)

TX_ANNOUNCE:
(TX_ANNOUNCE:)

	A change of data by the process is emitted immediately.
	(进程对数据所做的更改会立即发出。)

TX_CP_CAN_ID:
(TX_CP_CAN_ID:)

	Copies the can_id from the message header to each
	subsequent frame in frames. This is intended as usage simplification. For
	TX tasks the unique can_id from the message header may differ from the
	can_id(s) stored for transmission in the subsequent struct can_frame(s).
	(把消息头中的 can_id 复制到 frames 中后续的每个帧。这是为了简化使用。
	对于 TX 任务，消息头中唯一的 can_id 可能与存储在后续 struct can_frame
	中用于发送的 can_id 不同。)

RX_FILTER_ID:
(RX_FILTER_ID:)

	Filter by can_id alone, no frames required (nframes=0).
	(仅按 can_id 过滤，不需要帧（nframes=0）。)

RX_CHECK_DLC:
(RX_CHECK_DLC:)

	A change of the DLC leads to an RX_CHANGED.
	(DLC 的变化会导致一个 RX_CHANGED。)

RX_NO_AUTOTIMER:
(RX_NO_AUTOTIMER:)

	Prevent automatically starting the timeout monitor.
	(阻止自动启动超时监视器。)

RX_ANNOUNCE_RESUME:
(RX_ANNOUNCE_RESUME:)

	If passed at RX_SETUP and a receive timeout occurred, a
	RX_CHANGED message will be generated when the (cyclic) receive restarts.
	(如果在 RX_SETUP 时传入此标志且发生了接收超时，那么当（循环）接收重新
	开始时会生成一个 RX_CHANGED 消息。)

TX_RESET_MULTI_IDX:
(TX_RESET_MULTI_IDX:)

	Reset the index for the multiple frame transmission.
	(重置多帧发送的索引。)

RX_RTR_FRAME:
(RX_RTR_FRAME:)

	Send reply for RTR-request (placed in op->frames[0]).
	(发送对 RTR 请求的应答（放在 op->frames[0] 中）。)

CAN_FD_FRAME:
(CAN_FD_FRAME:)

	The CAN frames following the bcm_msg_head are struct canfd_frame's
	( bcm_msg_head 之后的 CAN 帧为 struct canfd_frame 类型)

Broadcast Manager Transmission Timers
(广播管理器发送定时器)
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

Periodic transmission configurations may use up to two interval timers.
In this case the BCM sends a number of messages ('count') at an interval
'ival1', then continuing to send at another given interval 'ival2'. When
only one timer is needed 'count' is set to zero and only 'ival2' is used.
When SET_TIMER and START_TIMER flag were set the timers are activated.
The timer values can be altered at runtime when only SET_TIMER is set.
(周期性发送配置最多可以使用两个间隔定时器。在这种情况下，BCM 以间隔
'ival1' 发送若干条消息（'count' 条），然后继续以另一个给定的间隔 'ival2'
发送。当只需要一个定时器时，把 'count' 设为零，只使用 'ival2'。当设置了
SET_TIMER 和 START_TIMER 标志时，定时器被激活。当只设置了 SET_TIMER 时，
定时器的值可以在运行时更改。)


Broadcast Manager message sequence transmission
(广播管理器消息序列发送)
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

Up to 256 CAN frames can be transmitted in a sequence in the case of a cyclic
TX task configuration. The number of CAN frames is provided in the 'nframes'
element of the BCM message head. The defined number of CAN frames are added
as array to the TX_SETUP BCM configuration message:
(在循环 TX 任务配置的情况下，一个序列中最多可以发送 256 个 CAN 帧。CAN 帧
的数量在 BCM 消息头的 'nframes' 元素中提供。定义数量的 CAN 帧作为数组附加
到 TX_SETUP BCM 配置消息中：)

.. code-block:: C

    /* create a struct to set up a sequence of four CAN frames */
    struct {
            struct bcm_msg_head msg_head;
            struct can_frame frame[4];
    } mytxmsg;

    (..)
    mytxmsg.msg_head.nframes = 4;
    (..)

    write(s, &mytxmsg, sizeof(mytxmsg));

With every transmission the index in the array of CAN frames is increased
and set to zero at index overflow.
(每发送一次，CAN 帧数组中的索引就会加一，并在索引溢出时归零。)


Broadcast Manager Receive Filter Timers
(广播管理器接收过滤器定时器)
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

The timer values ival1 or ival2 may be set to non-zero values at RX_SETUP.
When the SET_TIMER flag is set the timers are enabled:
(在 RX_SETUP 时可以将定时器值 ival1 或 ival2 设置为非零值。当设置了
SET_TIMER 标志时，这些定时器被启用：)

ival1:
(ival1:)

	Send RX_TIMEOUT when a received message is not received again within
	the given time. When START_TIMER is set at RX_SETUP the timeout detection
	is activated directly - even without a former CAN frame reception.
	(当已接收的消息在给定时间内没有再次收到时发送 RX_TIMEOUT。如果在
	RX_SETUP 时设置了 START_TIMER，则超时检测被直接激活——即使此前没有
	收到过任何 CAN 帧。)

ival2:
(ival2:)

	Throttle the received message rate down to the value of ival2. This
	is useful to reduce messages for the application when the signal inside the
	CAN frame is stateless as state changes within the ival2 period may get
	lost.
	(把接收消息的速率限制到 ival2 的值。当 CAN 帧内的信号是无状态的时，这
	有助于减少应用程序收到的消息数量，因为 ival2 周期内的状态变化可能会
	丢失。)

Broadcast Manager Multiplex Message Receive Filter
(广播管理器多路复用消息接收过滤器)
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

To filter for content changes in multiplex message sequences an array of more
than one CAN frames can be passed in a RX_SETUP configuration message. The
data bytes of the first CAN frame contain the mask of relevant bits that
have to match in the subsequent CAN frames with the received CAN frame.
If one of the subsequent CAN frames is matching the bits in that frame data
mark the relevant content to be compared with the previous received content.
Up to 257 CAN frames (multiplex filter bit mask CAN frame plus 256 CAN
filters) can be added as array to the TX_SETUP BCM configuration message:
(要过滤多路复用消息序列中的内容变化，可以在一个 RX_SETUP 配置消息中传入
一个包含多个 CAN 帧的数组。第一个 CAN 帧的数据字节包含相关位的掩码，后续
CAN 帧与接收到的 CAN 帧必须在这些位上匹配。如果某个后续 CAN 帧匹配，那么
该帧数据中被标记的位所对应的相关内容将与之前接收到的内容进行比较。最多
257 个 CAN 帧（多路复用过滤器位掩码 CAN 帧加上 256 个 CAN 过滤器）可以作
为数组附加到 TX_SETUP BCM 配置消息中：)

.. code-block:: C

    /* usually used to clear CAN frame data[] - beware of endian problems! */
    #define U64_DATA(p) (*(unsigned long long*)(p)->data)

    struct {
            struct bcm_msg_head msg_head;
            struct can_frame frame[5];
    } msg;

    msg.msg_head.opcode  = RX_SETUP;
    msg.msg_head.can_id  = 0x42;
    msg.msg_head.flags   = 0;
    msg.msg_head.nframes = 5;
    U64_DATA(&msg.frame[0]) = 0xFF00000000000000ULL; /* MUX mask */
    U64_DATA(&msg.frame[1]) = 0x01000000000000FFULL; /* data mask (MUX 0x01) */
    U64_DATA(&msg.frame[2]) = 0x0200FFFF000000FFULL; /* data mask (MUX 0x02) */
    U64_DATA(&msg.frame[3]) = 0x330000FFFFFF0003ULL; /* data mask (MUX 0x33) */
    U64_DATA(&msg.frame[4]) = 0x4F07FC0FF0000000ULL; /* data mask (MUX 0x4F) */

    write(s, &msg, sizeof(msg));


Broadcast Manager CAN FD Support
(广播管理器 CAN FD 支持)
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

The programming API of the CAN_BCM depends on struct can_frame which is
given as array directly behind the bcm_msg_head structure. To follow this
schema for the CAN FD frames a new flag 'CAN_FD_FRAME' in the bcm_msg_head
flags indicates that the concatenated CAN frame structures behind the
bcm_msg_head are defined as struct canfd_frame:
(CAN_BCM 的编程 API 依赖于 struct can_frame，它以数组的形式紧跟在
bcm_msg_head 结构之后。为了让 CAN FD 帧也遵循这一模式，bcm_msg_head 的
flags 中引入了新标志 'CAN_FD_FRAME'，它表示 bcm_msg_head 之后串联的 CAN 帧
结构按 struct canfd_frame 定义：)

.. code-block:: C

    struct {
            struct bcm_msg_head msg_head;
            struct canfd_frame frame[5];
    } msg;

    msg.msg_head.opcode  = RX_SETUP;
    msg.msg_head.can_id  = 0x42;
    msg.msg_head.flags   = CAN_FD_FRAME;
    msg.msg_head.nframes = 5;
    (..)

When using CAN FD frames for multiplex filtering the MUX mask is still
expected in the first 64 bit of the struct canfd_frame data section.
(当使用 CAN FD 帧进行多路复用过滤时，MUX 掩码仍然应位于 struct canfd_frame
数据段的前 64 位中。)


Connected Transport Protocols (SOCK_SEQPACKET)
(面向连接的传输协议（SOCK_SEQPACKET）)
----------------------------------------------

(to be written)
(待编写。)


Unconnected Transport Protocols (SOCK_DGRAM)
(无连接的传输协议（SOCK_DGRAM）)
--------------------------------------------

(to be written)
(待编写。)


.. _socketcan-core-module:

SocketCAN Core Module
(SocketCAN 核心模块)
=====================

The SocketCAN core module implements the protocol family
PF_CAN. CAN protocol modules are loaded by the core module at
runtime. The core module provides an interface for CAN protocol
modules to subscribe needed CAN IDs (see :ref:`socketcan-receive-lists`).
(SocketCAN 核心模块实现协议族 PF_CAN。CAN 协议模块由核心模块在运行时加载。
核心模块为 CAN 协议模块提供了一个订阅所需 CAN ID 的接口
（见 :ref:`socketcan-receive-lists`）。)


can.ko Module Params
(can.ko 模块参数)
--------------------

- **stats_timer**:
  To calculate the SocketCAN core statistics
  (e.g. current/maximum frames per second) this 1 second timer is
  invoked at can.ko module start time by default. This timer can be
  disabled by using stattimer=0 on the module commandline.
  (为了计算 SocketCAN 核心统计信息（例如当前/最大每秒帧数），默认情况下这
  个 1 秒定时器会在 can.ko 模块启动时被调用。可以通过在模块命令行上使用
  stattimer=0 来禁用该定时器。)

- **debug**:
  (removed since SocketCAN SVN r546)
  (自 SocketCAN SVN r546 起已移除。)


procfs content
(procfs 内容)
--------------

As described in :ref:`socketcan-receive-lists` the SocketCAN core uses several filter
lists to deliver received CAN frames to CAN protocol modules. These
receive lists, their filters and the count of filter matches can be
checked in the appropriate receive list. All entries contain the
device and a protocol module identifier::
(如 :ref:`socketcan-receive-lists` 中所述，SocketCAN 核心使用多个过滤器列
表把接收到的 CAN 帧分发给 CAN 协议模块。可以在相应的接收列表中查看这些接
收列表、它们的过滤器以及过滤器匹配计数。所有条目都包含设备名和协议模块标
识符：)

    foo@bar:~$ cat /proc/net/can/rcvlist_all

    receive list 'rx_all':
      (vcan3: no entry)
      (vcan2: no entry)
      (vcan1: no entry)

      device   can_id   can_mask  function  userdata   matches  ident

       vcan0     000    00000000  f88e6370  f6c6f400         0  raw

      (any: no entry)

In this example an application requests any CAN traffic from vcan0::
(在此示例中，应用程序请求来自 vcan0 的任意 CAN 流量：)

    rcvlist_all - list for unfiltered entries (no filter operations)
    rcvlist_eff - list for single extended frame (EFF) entries
    rcvlist_err - list for error message frames masks
    rcvlist_fil - list for mask/value filters
    rcvlist_inv - list for mask/value filters (inverse semantic)
    rcvlist_sff - list for single standard frame (SFF) entries

Additional procfs files in /proc/net/can::
( /proc/net/can 中的其他 procfs 文件：)

    stats       - SocketCAN core statistics (rx/tx frames, match ratios, ...)
    reset_stats - manual statistic reset
    version     - prints SocketCAN core and ABI version (removed in Linux 5.10)


Writing Own CAN Protocol Modules
(编写自己的 CAN 协议模块)
--------------------------------

To implement a new protocol in the protocol family PF_CAN a new
protocol has to be defined in include/linux/can.h .
The prototypes and definitions to use the SocketCAN core can be
accessed by including include/linux/can/core.h .
In addition to functions that register the CAN protocol and the
CAN device notifier chain there are functions to subscribe CAN
frames received by CAN interfaces and to send CAN frames::
(要在协议族 PF_CAN 中实现一个新协议，必须在 include/linux/can.h 中定义一
个新协议。包含 include/linux/can/core.h 即可使用 SocketCAN 核心的原型和定
义。除了注册 CAN 协议和 CAN 设备通知链的函数之外，还有用于订阅 CAN 接口
接收到的 CAN 帧以及发送 CAN 帧的函数：)

    can_rx_register   - subscribe CAN frames from a specific interface
    can_rx_unregister - unsubscribe CAN frames from a specific interface
    can_send          - transmit a CAN frame (optional with local loopback)

For details see the kerneldoc documentation in net/can/af_can.c or
the source code of net/can/raw.c or net/can/bcm.c .
(详情请参阅 net/can/af_can.c 中的 kerneldoc 文档，或 net/can/raw.c 与
net/can/bcm.c 的源代码。)


CAN Network Drivers
(CAN 网络驱动)
===================

Writing a CAN network device driver is much easier than writing a
CAN character device driver. Similar to other known network device
drivers you mainly have to deal with:
(编写 CAN 网络设备驱动比编写 CAN 字符设备驱动容易得多。与其他广为人知的网
络设备驱动类似，你主要需要处理：)

- TX: Put the CAN frame from the socket buffer to the CAN controller.
  (TX：把 CAN 帧从套接字缓冲区送到 CAN 控制器。)
- RX: Put the CAN frame from the CAN controller to the socket buffer.
  (RX：把 CAN 帧从 CAN 控制器送到套接字缓冲区。)

See e.g. at Documentation/networking/netdevices.rst . The differences
for writing CAN network device driver are described below:
(例如参见 Documentation/networking/netdevices.rst 。编写 CAN 网络设备驱动
的不同之处如下所述：)


General Settings
(常规设置)
----------------

CAN network device drivers can use alloc_candev_mqs() and friends instead of
alloc_netdev_mqs(), to automatically take care of CAN-specific setup:
(CAN 网络设备驱动可以使用 alloc_candev_mqs() 及其相关函数来代替
alloc_netdev_mqs()，以自动处理 CAN 特有的设置：)

.. code-block:: C

    dev = alloc_candev_mqs(...);

The struct can_frame or struct canfd_frame is the payload of each socket
buffer (skbuff) in the protocol family PF_CAN.
(struct can_frame 或 struct canfd_frame 是协议族 PF_CAN 中每个套接字缓冲区
（skbuff）的载荷。)


.. _socketcan-local-loopback2:

Local Loopback of Sent Frames
(发送帧的本地回环)
-----------------------------

As described in :ref:`socketcan-local-loopback1` the CAN network device driver should
support a local loopback functionality similar to the local echo
e.g. of tty devices. In this case the driver flag IFF_ECHO has to be
set to prevent the PF_CAN core from locally echoing sent frames
(aka loopback) as fallback solution::
(如 :ref:`socketcan-local-loopback1` 中所述，CAN 网络设备驱动应当支持本地
回环功能，类似于 tty 设备的本地回显。此时必须设置驱动标志 IFF_ECHO，以防
止 PF_CAN 核心将已发送帧在本地回显（又称回环）作为后备方案：)

    dev->flags = (IFF_NOARP | IFF_ECHO);


CAN Controller Hardware Filters
(CAN 控制器硬件过滤器)
-------------------------------

To reduce the interrupt load on deep embedded systems some CAN
controllers support the filtering of CAN IDs or ranges of CAN IDs.
These hardware filter capabilities vary from controller to
controller and have to be identified as not feasible in a multi-user
networking approach. The use of the very controller specific
hardware filters could make sense in a very dedicated use-case, as a
filter on driver level would affect all users in the multi-user
system. The high efficient filter sets inside the PF_CAN core allow
to set different multiple filters for each socket separately.
Therefore the use of hardware filters goes to the category 'handmade
tuning on deep embedded systems'. The author is running a MPC603e
@133MHz with four SJA1000 CAN controllers from 2002 under heavy bus
load without any problems ...
(为了降低深度嵌入式系统上的中断负载，一些 CAN 控制器支持对 CAN ID 或 CAN
ID 范围进行过滤。这些硬件过滤能力因控制器而异，在多用户网络方案中必须认
定其不可行。控制器专用的硬件过滤器只在非常特殊的使用场景下才可能有意义，
因为驱动层的过滤器会影响多用户系统中的所有用户。PF_CAN 核心内部高效的过
滤器集合允许为每个套接字分别设置不同的多个过滤器。因此，硬件过滤器的使用
属于"在深度嵌入式系统上手工调优"的范畴。作者自 2002 年起一直在 133MHz 的
MPC603e 上运行四个 SJA1000 CAN 控制器，即使在沉重的总线负载下也没有出现
任何问题……)


Switchable Termination Resistors
(可切换的终端电阻)
--------------------------------

CAN bus requires a specific impedance across the differential pair,
typically provided by two 120Ohm resistors on the farthest nodes of
the bus. Some CAN controllers support activating / deactivating a
termination resistor(s) to provide the correct impedance.
(CAN 总线要求差分线对之间具有特定阻抗，这通常由总线上相距最远的两个节点上
的两个 120 欧姆电阻提供。一些 CAN 控制器支持激活/停用终端电阻以提供正确的
阻抗。)

Query the available resistances::
(查询可用的电阻值：)

    $ ip -details link show can0
    ...
    termination 120 [ 0, 120 ]

Activate the terminating resistor::
(激活终端电阻：)

    $ ip link set dev can0 type can termination 120

Deactivate the terminating resistor::
(停用终端电阻：)

    $ ip link set dev can0 type can termination 0

To enable termination resistor support to a can-controller, either
implement in the controller's struct can-priv::
(要为 CAN 控制器启用终端电阻支持，可以在控制器的 struct can-priv 中实现
以下成员：)

    termination_const
    termination_const_cnt
    do_set_termination

or add gpio control with the device tree entries from
Documentation/devicetree/bindings/net/can/can-controller.yaml
(或者，使用 Documentation/devicetree/bindings/net/can/can-controller.yaml
中的设备树条目添加 GPIO 控制。)


The Virtual CAN Driver (vcan)
(虚拟 CAN 驱动（vcan）)
-----------------------------

Similar to the network loopback devices, vcan offers a virtual local
CAN interface. A full qualified address on CAN consists of
(与网络回环设备类似，vcan 提供一个虚拟的本地 CAN 接口。CAN 上的一个完整限
定地址由以下部分组成：)

- a unique CAN Identifier (CAN ID)
  (一个唯一的 CAN 标识符（CAN ID）)
- the CAN bus this CAN ID is transmitted on (e.g. can0)
  (该 CAN ID 在其上传输的 CAN 总线（例如 can0）)

so in common use cases more than one virtual CAN interface is needed.
(因此在常见用例中需要一个以上的虚拟 CAN 接口。)

The virtual CAN interfaces allow the transmission and reception of CAN
frames without real CAN controller hardware. Virtual CAN network
devices are usually named 'vcanX', like vcan0 vcan1 vcan2 ...
When compiled as a module the virtual CAN driver module is called vcan.ko
(虚拟 CAN 接口允许在没有真实 CAN 控制器硬件的情况下发送和接收 CAN 帧。虚
拟 CAN 网络设备通常命名为 'vcanX'，如 vcan0、vcan1、vcan2……当编译为模块
时，虚拟 CAN 驱动模块称为 vcan.ko)

Since Linux Kernel version 2.6.24 the vcan driver supports the Kernel
netlink interface to create vcan network devices. The creation and
removal of vcan network devices can be managed with the ip(8) tool::
(自 Linux 内核 2.6.24 版本起，vcan 驱动支持使用内核 netlink 接口创建 vcan
网络设备。vcan 网络设备的创建与删除可以用 ip(8) 工具管理：)

  - Create a virtual CAN network interface:
       $ ip link add type vcan

  - Create a virtual CAN network interface with a specific name 'vcan42':
       $ ip link add dev vcan42 type vcan

  - Remove a (virtual CAN) network interface 'vcan42':
       $ ip link del vcan42


The CAN Network Device Driver Interface
(CAN 网络设备驱动接口)
---------------------------------------

The CAN network device driver interface provides a generic interface
to setup, configure and monitor CAN network devices. The user can then
configure the CAN device, like setting the bit-timing parameters, via
the netlink interface using the program "ip" from the "IPROUTE2"
utility suite. The following chapter describes briefly how to use it.
Furthermore, the interface uses a common data structure and exports a
set of common functions, which all real CAN network device drivers
should use. Please have a look to the SJA1000 or MSCAN driver to
understand how to use them. The name of the module is can-dev.ko.
(CAN 网络设备驱动接口提供了一套通用接口，用于设置、配置和监控 CAN 网络设
备。用户随后可以通过 netlink 接口、使用 "IPROUTE2" 工具套件中的 "ip" 程序
来配置 CAN 设备，例如设置位时序参数。下一章将简要介绍如何使用它。此外，该
接口使用一个公共数据结构并导出一组公共函数，所有实际的 CAN 网络设备驱动
都应使用它们。请参阅 SJA1000 或 MSCAN 驱动来了解如何使用。该模块名为
can-dev.ko。)


Netlink interface to set/get devices properties
(设置/获取设备属性的 netlink 接口)
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

The CAN device must be configured via netlink interface. The supported
netlink message types are defined and briefly described in
"include/linux/can/netlink.h". CAN link support for the program "ip"
of the IPROUTE2 utility suite is available and it can be used as shown
below:
(必须通过 netlink 接口来配置 CAN 设备。受支持的 netlink 消息类型在
"include/linux/can/netlink.h" 中定义并有简要描述。IPROUTE2 工具套件中的
"ip" 程序已提供 CAN 链路支持，用法如下所示：)

Setting CAN device properties::
(设置 CAN 设备属性：)

    $ ip link set can0 type can help
    Usage: ip link set DEVICE type can

        [ bitrate BITRATE [ sample-point SAMPLE-POINT] ] |
        [ tq TQ prop-seg PROP_SEG phase-seg1 PHASE-SEG1

          phase-seg2 PHASE-SEG2 [ sjw SJW ] ]

        [ dbitrate BITRATE [ dsample-point SAMPLE-POINT] ] |
        [ dtq TQ dprop-seg PROP_SEG dphase-seg1 PHASE-SEG1

          dphase-seg2 PHASE-SEG2 [ dsjw SJW ] ]

        [ loopback { on | off } ]
        [ listen-only { on | off } ]
        [ triple-sampling { on | off } ]
        [ one-shot { on | off } ]
        [ berr-reporting { on | off } ]
        [ fd { on | off } ]
        [ fd-non-iso { on | off } ]
        [ presume-ack { on | off } ]
        [ cc-len8-dlc { on | off } ]

        [ restart-ms TIME-MS ]
        [ restart ]

        Where: BITRATE       := { 1..1000000 }
               SAMPLE-POINT  := { 0.000..0.999 }
               TQ            := { NUMBER }
               PROP-SEG      := { 1..8 }
               PHASE-SEG1    := { 1..8 }
               PHASE-SEG2    := { 1..8 }
               SJW           := { 1..4 }
               RESTART-MS    := { 0 | NUMBER }

Display CAN device details and statistics::
(显示 CAN 设备的详细信息与统计信息：)

    $ ip -details -statistics link show can0
    2: can0: <NOARP,UP,LOWER_UP,ECHO> mtu 16 qdisc pfifo_fast state UP qlen 10

      link/can
      can <TRIPLE-SAMPLING> state ERROR-ACTIVE restart-ms 100
      bitrate 125000 sample_point 0.875
      tq 125 prop-seg 6 phase-seg1 7 phase-seg2 2 sjw 1
      sja1000: tseg1 1..16 tseg2 1..8 sjw 1..4 brp 1..64 brp-inc 1
      clock 8000000
      re-started bus-errors arbit-lost error-warn error-pass bus-off
      41         17457      0          41         42         41
      RX: bytes  packets  errors  dropped overrun mcast
      140859     17608    17457   0       0       0
      TX: bytes  packets  errors  dropped carrier collsns
      861        112      0       41      0       0

More info to the above output:
(关于上述输出的更多信息：)

"<TRIPLE-SAMPLING>"
("<TRIPLE-SAMPLING>")

	Shows the list of selected CAN controller modes: LOOPBACK,
	LISTEN-ONLY, or TRIPLE-SAMPLING.
	(显示所选 CAN 控制器模式的列表：LOOPBACK、LISTEN-ONLY 或
	TRIPLE-SAMPLING。)

"state ERROR-ACTIVE"
("state ERROR-ACTIVE")

	The current state of the CAN controller: "ERROR-ACTIVE",
	"ERROR-WARNING", "ERROR-PASSIVE", "BUS-OFF" or "STOPPED"
	(CAN 控制器的当前状态："ERROR-ACTIVE"、"ERROR-WARNING"、
	"ERROR-PASSIVE"、"BUS-OFF" 或 "STOPPED"。)

"restart-ms 100"
("restart-ms 100")

	Automatic restart delay time. If set to a non-zero value, a
	restart of the CAN controller will be triggered automatically
	in case of a bus-off condition after the specified delay time
	in milliseconds. By default it's off.
	(自动重启延迟时间。如果设置为非零值，那么在出现 bus-off 状态时，将在
	指定的延迟时间（以毫秒为单位）之后自动触发 CAN 控制器的重启。默认关闭。)

"bitrate 125000 sample-point 0.875"
("bitrate 125000 sample-point 0.875")

	Shows the real bit-rate in bits/sec and the sample-point in the
	range 0.000..0.999. If the calculation of bit-timing parameters
	is enabled in the kernel (CONFIG_CAN_CALC_BITTIMING=y), the
	bit-timing can be defined by setting the "bitrate" argument.
	Optionally the "sample-point" can be specified. By default it's
	0.000 assuming CIA-recommended sample-points.
	(显示以比特/秒为单位的实际位速率以及 0.000..0.999 范围内的采样点。如
	果内核中启用了位时序参数计算（CONFIG_CAN_CALC_BITTIMING=y），则可以通
	过设置 "bitrate" 参数来定义位时序。还可以选择指定 "sample-point"。默
	认值为 0.000，并假定采用 CIA 推荐的采样点。)

"tq 125 prop-seg 6 phase-seg1 7 phase-seg2 2 sjw 1"
("tq 125 prop-seg 6 phase-seg1 7 phase-seg2 2 sjw 1")

	Shows the time quanta in ns, propagation segment, phase buffer
	segment 1 and 2 and the synchronisation jump width in units of
	tq. They allow to define the CAN bit-timing in a hardware
	independent format as proposed by the Bosch CAN 2.0 spec (see
	chapter 8 of http://www.semiconductors.bosch.de/pdf/can2spec.pdf).
	(显示以 ns 为单位的时间量子、传播段、相位缓冲段 1 和 2 以及以 tq 为单
	位的同步跳转宽度。它们允许按照 Bosch CAN 2.0 规范的建议，以与硬件无关
	的格式定义 CAN 位时序（见
	http://www.semiconductors.bosch.de/pdf/can2spec.pdf 第 8 章）。)

"sja1000: tseg1 1..16 tseg2 1..8 sjw 1..4 brp 1..64 brp-inc 1 clock 8000000"
("sja1000: tseg1 1..16 tseg2 1..8 sjw 1..4 brp 1..64 brp-inc 1 clock 8000000")

	Shows the bit-timing constants of the CAN controller, here the
	"sja1000". The minimum and maximum values of the time segment 1
	and 2, the synchronisation jump width in units of tq, the
	bitrate pre-scaler and the CAN system clock frequency in Hz.
	These constants could be used for user-defined (non-standard)
	bit-timing calculation algorithms in user-space.
	(显示 CAN 控制器（此处为 "sja1000"）的位时序常量：时间段 1 和 2 的最小
	值与最大值、以 tq 为单位的同步跳转宽度、位速率预分频器以及以 Hz 为单位
	的 CAN 系统时钟频率。这些常量可用于用户空间中用户自定义的（非标准的）
	位时序计算算法。)

"re-started bus-errors arbit-lost error-warn error-pass bus-off"
("re-started bus-errors arbit-lost error-warn error-pass bus-off")

	Shows the number of restarts, bus and arbitration lost errors,
	and the state changes to the error-warning, error-passive and
	bus-off state. RX overrun errors are listed in the "overrun"
	field of the standard network statistics.
	(显示重启次数、总线错误和仲裁丢失错误，以及到 error-warning、
	error-passive 和 bus-off 状态的状态变化。RX 溢出错误列在标准网络统计信
	息的 "overrun" 字段中。)

Setting the CAN Bit-Timing
(设置 CAN 位时序)
~~~~~~~~~~~~~~~~~~~~~~~~~~

The CAN bit-timing parameters can always be defined in a hardware
independent format as proposed in the Bosch CAN 2.0 specification
specifying the arguments "tq", "prop_seg", "phase_seg1", "phase_seg2"
and "sjw"::
(CAN 位时序参数始终可以按照 Bosch CAN 2.0 规范建议的、与硬件无关的格式来
定义，即指定参数 "tq"、"prop_seg"、"phase_seg1"、"phase_seg2" 和 "sjw"：)

    $ ip link set canX type can tq 125 prop-seg 6 \
				phase-seg1 7 phase-seg2 2 sjw 1

If the kernel option CONFIG_CAN_CALC_BITTIMING is enabled, CIA
recommended CAN bit-timing parameters will be calculated if the bit-
rate is specified with the argument "bitrate"::
(如果内核选项 CONFIG_CAN_CALC_BITTIMING 已启用，那么当用参数 "bitrate" 指
定位速率时，将自动计算 CIA 推荐的 CAN 位时序参数：)

    $ ip link set canX type can bitrate 125000

Note that this works fine for the most common CAN controllers with
standard bit-rates but may *fail* for exotic bit-rates or CAN system
clock frequencies. Disabling CONFIG_CAN_CALC_BITTIMING saves some
space and allows user-space tools to solely determine and set the
bit-timing parameters. The CAN controller specific bit-timing
constants can be used for that purpose. They are listed by the
following command::
(注意，这对大多数采用标准位速率的常见 CAN 控制器来说工作良好，但对于特殊
的位速率或 CAN 系统时钟频率则可能 *fail*。禁用 CONFIG_CAN_CALC_BITTIMING
可以节省一些空间，并允许用户空间工具独自确定并设置位时序参数。为此可以使
用 CAN 控制器特定的位时序常量。它们由以下命令列出：)

    $ ip -details link show can0
    ...

      sja1000: clock 8000000 tseg1 1..16 tseg2 1..8 sjw 1..4 brp 1..64 brp-inc 1


Starting and Stopping the CAN Network Device
(启动与停止 CAN 网络设备)
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

A CAN network device is started or stopped as usual with the command
"ifconfig canX up/down" or "ip link set canX up/down". Be aware that
you *must* define proper bit-timing parameters for real CAN devices
before you can start it to avoid error-prone default settings::
(CAN 网络设备像往常一样用命令 "ifconfig canX up/down" 或 "ip link set canX
up/down" 来启动或停止。注意，对于真实的 CAN 设备，在启动之前你 *必须* 定
义正确的位时序参数，以避免容易出错的默认设置：)

    $ ip link set canX up type can bitrate 125000

A device may enter the "bus-off" state if too many errors occurred on
the CAN bus. Then no more messages are received or sent. An automatic
bus-off recovery can be enabled by setting the "restart-ms" to a
non-zero value, e.g.::
(如果 CAN 总线上发生了过多错误，设备可能进入 "bus-off" 状态，此时不再接收
或发送任何消息。可以通过把 "restart-ms" 设置为非零值来启用自动 bus-off 恢
复，例如：)

    $ ip link set canX type can restart-ms 100

Alternatively, the application may realize the "bus-off" condition
by monitoring CAN error message frames and do a restart when
appropriate with the command::
(或者，应用程序可以通过监视 CAN 错误消息帧来感知 "bus-off" 状态，并在合适
的时机用以下命令执行重启：)

    $ ip link set canX type can restart

Note that a restart will also create a CAN error message frame (see
also :ref:`socketcan-network-problem-notifications`).
(注意，重启也会产生一个 CAN 错误消息帧（另见
:ref:`socketcan-network-problem-notifications`）。)


.. _socketcan-can-fd-driver:

CAN FD (Flexible Data Rate) Driver Support
(CAN FD（灵活数据速率）驱动支持)
------------------------------------------

CAN FD capable CAN controllers support two different bitrates for the
arbitration phase and the payload phase of the CAN FD frame. Therefore a
second bit timing has to be specified in order to enable the CAN FD bitrate.
(支持 CAN FD 的 CAN 控制器为 CAN FD 帧的仲裁阶段和载荷阶段支持两种不同的
位速率。因此，必须指定第二个位时序才能启用 CAN FD 位速率。)

Additionally CAN FD capable CAN controllers support up to 64 bytes of
payload. The representation of this length in can_frame.len and
canfd_frame.len for userspace applications and inside the Linux network
layer is a plain value from 0 .. 64 instead of the Classical CAN length
which ranges from 0 to 8. The payload length to the bus-relevant DLC mapping
is only performed inside the CAN drivers, preferably with the helper
functions can_fd_dlc2len() and can_fd_len2dlc().
(此外，支持 CAN FD 的 CAN 控制器支持最多 64 字节的载荷。对用户空间应用程
序以及在 Linux 网络层内部而言，can_frame.len 和 canfd_frame.len 中该长度
的表示是 0 .. 64 的普通数值，而不是范围从 0 到 8 的经典 CAN 长度。从载荷长
度到总线相关 DLC 的映射只在 CAN 驱动内部完成，最好借助辅助函数
can_fd_dlc2len() 和 can_fd_len2dlc()。)

The CAN netdevice driver capabilities can be distinguished by the network
devices maximum transfer unit (MTU)::
(CAN netdevice 驱动的能力可以通过网络设备的最大传输单元（MTU）来区分：)

  MTU = 16 (CAN_MTU)   => sizeof(struct can_frame)   => Classical CAN device
  MTU = 72 (CANFD_MTU) => sizeof(struct canfd_frame) => CAN FD capable device

The CAN device MTU can be retrieved e.g. with a SIOCGIFMTU ioctl() syscall.
N.B. CAN FD capable devices can also handle and send Classical CAN frames.
(CAN 设备的 MTU 可以例如通过 SIOCGIFMTU ioctl() 系统调用获取。注意，支持
CAN FD 的设备同样能够处理和发送经典 CAN 帧。)

When configuring CAN FD capable CAN controllers an additional 'data' bitrate
has to be set. This bitrate for the data phase of the CAN FD frame has to be
at least the bitrate which was configured for the arbitration phase. This
second bitrate is specified analogue to the first bitrate but the bitrate
setting keywords for the 'data' bitrate start with 'd' e.g. dbitrate,
dsample-point, dsjw or dtq and similar settings. When a data bitrate is set
within the configuration process the controller option "fd on" can be
specified to enable the CAN FD mode in the CAN controller. This controller
option also switches the device MTU to 72 (CANFD_MTU).
(在配置支持 CAN FD 的 CAN 控制器时，还必须设置一个额外的 'data' 位速率。
这个用于 CAN FD 帧数据阶段的位速率必须至少不低于为仲裁阶段配置的位速率。
第二个位速率的指定方式与第一个位速率类似，但 'data' 位速率的设置关键字以
'd' 开头，例如 dbitrate、dsample-point、dsjw 或 dtq 等类似设置。在配置过
程中设置了数据位速率之后，可以指定控制器选项 "fd on" 来在 CAN 控制器中启
用 CAN FD 模式。该控制器选项还会把设备的 MTU 切换为 72（CANFD_MTU）。)

The first CAN FD specification presented as whitepaper at the International
CAN Conference 2012 needed to be improved for data integrity reasons.
Therefore two CAN FD implementations have to be distinguished today:
(最初作为白皮书在 2012 年国际 CAN 会议上发布的第一份 CAN FD 规范，出于数据
完整性方面的原因需要改进。因此，如今必须区分两种 CAN FD 实现：)

- ISO compliant:     The ISO 11898-1:2015 CAN FD implementation (default)
  (ISO 兼容：符合 ISO 11898-1:2015 的 CAN FD 实现（默认）)
- non-ISO compliant: The CAN FD implementation following the 2012 whitepaper
  (非 ISO 兼容：遵循 2012 年白皮书的 CAN FD 实现)

Finally there are three types of CAN FD controllers:
(最后，共有三种类型的 CAN FD 控制器：)

1. ISO compliant (fixed)
   (ISO 兼容（固定）)
2. non-ISO compliant (fixed, like the M_CAN IP core v3.0.1 in m_can.c)
   (非 ISO 兼容（固定，如 m_can.c 中的 M_CAN IP 核 v3.0.1）)
3. ISO/non-ISO CAN FD controllers (switchable, like the PEAK PCAN-USB FD)
   (ISO/非 ISO 混合型 CAN FD 控制器（可切换，如 PEAK PCAN-USB FD）)

The current ISO/non-ISO mode is announced by the CAN controller driver via
netlink and displayed by the 'ip' tool (controller option FD-NON-ISO).
The ISO/non-ISO-mode can be altered by setting 'fd-non-iso {on|off}' for
switchable CAN FD controllers only.
(当前的 ISO/非 ISO 模式由 CAN 控制器驱动通过 netlink 声明，并由 'ip' 工具
显示（控制器选项 FD-NON-ISO）。只有可切换的 CAN FD 控制器才能通过设置
'fd-non-iso {on|off}' 来更改 ISO/非 ISO 模式。)

Example configuring 500 kbit/s arbitration bitrate and 4 Mbit/s data bitrate::
(示例：配置 500 kbit/s 仲裁位速率和 4 Mbit/s 数据位速率：)

    $ ip link set can0 up type can bitrate 500000 sample-point 0.75 \
                                   dbitrate 4000000 dsample-point 0.8 fd on

    $ ip -details link show can0

    5: can0: <NOARP,UP,LOWER_UP,ECHO> mtu 72 qdisc pfifo_fast state UNKNOWN \
             mode DEFAULT group default qlen 10

    link/can  promiscuity 0

    can <FD> state ERROR-ACTIVE (berr-counter tx 0 rx 0) restart-ms 0

          bitrate 500000 sample-point 0.750
          tq 50 prop-seg 14 phase-seg1 15 phase-seg2 10 sjw 1
          pcan_usb_pro_fd: tseg1 1..64 tseg2 1..16 sjw 1..16 brp 1..1024 \
          brp-inc 1
          dbitrate 4000000 dsample-point 0.800
          dtq 12 dprop-seg 7 dphase-seg1 8 dphase-seg2 4 dsjw 1
          pcan_usb_pro_fd: dtseg1 1..16 dtseg2 1..8 dsjw 1..4 dbrp 1..1024 \
          dbrp-inc 1
          clock 80000000

Example when 'fd-non-iso on' is added on this switchable CAN FD adapter::
(在此可切换 CAN FD 适配器上添加 'fd-non-iso on' 时的示例：)

   can <FD,FD-NON-ISO> state ERROR-ACTIVE (berr-counter tx 0 rx 0) restart-ms 0


Transmitter Delay Compensation
(发送器延迟补偿)
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

At high bit rates, the propagation delay from the TX pin to the RX pin of
the transceiver might become greater than the actual bit time causing
measurement errors: the RX pin would still be measuring the previous bit.
(在高波特率下，收发器从 TX 引脚到 RX 引脚的传播延迟可能会大于实际的位时
间，从而导致测量错误：RX 引脚测量的可能仍是前一个位。)

The Transmitter Delay Compensation (thereafter, TDC) resolves this problem
by introducing a Secondary Sample Point (SSP) equal to the distance, in
minimum time quantum, from the start of the bit time on the TX pin to the
actual measurement on the RX pin. The SSP is calculated as the sum of two
configurable values: the TDC Value (TDCV) and the TDC offset (TDCO).
(发送器延迟补偿（Transmitter Delay Compensation，下称 TDC）通过引入一个次
级采样点（Secondary Sample Point，SSP）来解决这个问题，SSP 等于从 TX 引脚
位时间开始到 RX 引脚实际测量之间的距离（以最小时间量子为单位）。SSP 的计算
方式是两个可配置值之和：TDC 值（TDCV）与 TDC 偏移（TDCO）。)

TDC, if supported by the device, can be configured together with CAN-FD
using the ip tool's "tdc-mode" argument as follow:
(如果设备支持 TDC，可以使用 ip 工具的 "tdc-mode" 参数将它与 CAN-FD 一起配
置，方式如下：)

**omitted**
(省略)

	When no "tdc-mode" option is provided, the kernel will automatically
	decide whether TDC should be turned on, in which case it will
	calculate a default TDCO and use the TDCV as measured by the
	device. This is the recommended method to use TDC.
	(未提供 "tdc-mode" 选项时，内核会自动决定是否应当开启 TDC；若开启，内
	核将计算一个默认的 TDCO，并使用设备测得的 TDCV。这是使用 TDC 的推荐方
	法。)

**"tdc-mode off"**
(**"tdc-mode off"**)

	TDC is explicitly disabled.
	(显式禁用 TDC。)

**"tdc-mode auto"**
(**"tdc-mode auto"**)

	The user must provide the "tdco" argument. The TDCV will be
	automatically calculated by the device. This option is only
	available if the device supports the TDC-AUTO CAN controller mode.
	(用户必须提供 "tdco" 参数。TDCV 将由设备自动计算。此选项仅在设备支持
	TDC-AUTO CAN 控制器模式时可用。)

**"tdc-mode manual"**
(**"tdc-mode manual"**)

	The user must provide both the "tdco" and "tdcv" arguments. This
	option is only available if the device supports the TDC-MANUAL CAN
	controller mode.
	(用户必须同时提供 "tdco" 和 "tdcv" 参数。此选项仅在设备支持 TDC-MANUAL
	CAN 控制器模式时可用。)

Note that some devices may offer an additional parameter: "tdcf" (TDC Filter
window). If supported by your device, this can be added as an optional
argument to either "tdc-mode auto" or "tdc-mode manual".
(注意，某些设备可能提供额外的参数："tdcf"（TDC 过滤窗口）。如果你的设备
支持，它可以作为可选参数添加到 "tdc-mode auto" 或 "tdc-mode manual"。)

Example configuring a 500 kbit/s arbitration bitrate, a 5 Mbit/s data
bitrate, a TDCO of 15 minimum time quantum and a TDCV automatically measured
by the device::
(示例：配置 500 kbit/s 仲裁位速率、5 Mbit/s 数据位速率、TDCO 为 15 个最小
时间量子，且 TDCV 由设备自动测量：)

    $ ip link set can0 up type can bitrate 500000 \
                                   fd on dbitrate 4000000 \
				   tdc-mode auto tdco 15

    $ ip -details link show can0

    5: can0: <NOARP,UP,LOWER_UP,ECHO> mtu 72 qdisc pfifo_fast state UP \
             mode DEFAULT group default qlen 10

        link/can  promiscuity 0 allmulti 0 minmtu 72 maxmtu 72
        can <FD,TDC-AUTO> state ERROR-ACTIVE restart-ms 0

          bitrate 500000 sample-point 0.875
          tq 12 prop-seg 69 phase-seg1 70 phase-seg2 20 sjw 10 brp 1
          ES582.1/ES584.1: tseg1 2..256 tseg2 2..128 sjw 1..128 brp 1..512 \
          brp_inc 1
          dbitrate 4000000 dsample-point 0.750
          dtq 12 dprop-seg 7 dphase-seg1 7 dphase-seg2 5 dsjw 2 dbrp 1
          tdco 15 tdcf 0
          ES582.1/ES584.1: dtseg1 2..32 dtseg2 1..16 dsjw 1..8 dbrp 1..32 \
          dbrp_inc 1
          tdco 0..127 tdcf 0..127
          clock 80000000


Supported CAN Hardware
(支持的 CAN 硬件)
----------------------

Please check the "Kconfig" file in "drivers/net/can" to get an actual
list of the support CAN hardware. On the SocketCAN project website
(see :ref:`socketcan-resources`) there might be further drivers available, also for
older kernel versions.
(请查看 "drivers/net/can" 中的 "Kconfig" 文件，以获取受支持 CAN 硬件的最新
列表。在 SocketCAN 项目网站（见 :ref:`socketcan-resources`）上可能还有更多
驱动可用，也包括面向较旧内核版本的驱动。)


.. _socketcan-resources:

SocketCAN Resources
(SocketCAN 资源)
===================

The Linux CAN / SocketCAN project resources (project site / mailing list)
are referenced in the MAINTAINERS file in the Linux source tree.
Search for CAN NETWORK [LAYERS|DRIVERS].
(Linux CAN / SocketCAN 项目资源（项目网站 / 邮件列表）在 Linux 源码树的
MAINTAINERS 文件中列出。搜索 CAN NETWORK [LAYERS|DRIVERS]。)

Credits
(致谢)
=======

- Oliver Hartkopp (PF_CAN core, filters, drivers, bcm, SJA1000 driver)
  (Oliver Hartkopp（PF_CAN 核心、过滤器、驱动、bcm、SJA1000 驱动）)
- Urs Thuermann (PF_CAN core, kernel integration, socket interfaces, raw, vcan)
  (Urs Thuermann（PF_CAN 核心、内核集成、套接字接口、raw、vcan）)
- Jan Kizka (RT-SocketCAN core, Socket-API reconciliation)
  (Jan Kizka（RT-SocketCAN 核心、套接字 API 调和）)
- Wolfgang Grandegger (RT-SocketCAN core & drivers, Raw Socket-API reviews, CAN device driver interface, MSCAN driver)
  (Wolfgang Grandegger（RT-SocketCAN 核心与驱动、Raw 套接字 API 评审、CAN
  设备驱动接口、MSCAN 驱动）)
- Robert Schwebel (design reviews, PTXdist integration)
  (Robert Schwebel（设计评审、PTXdist 集成）)
- Marc Kleine-Budde (design reviews, Kernel 2.6 cleanups, drivers)
  (Marc Kleine-Budde（设计评审、内核 2.6 清理、驱动）)
- Benedikt Spranger (reviews)
  (Benedikt Spranger（评审）)
- Thomas Gleixner (LKML reviews, coding style, posting hints)
  (Thomas Gleixner（LKML 评审、编码风格、投稿提示）)
- Andrey Volkov (kernel subtree structure, ioctls, MSCAN driver)
  (Andrey Volkov（内核子树结构、ioctl、MSCAN 驱动）)
- Matthias Brukner (first SJA1000 CAN netdevice implementation Q2/2003)
  (Matthias Brukner（2003 年第二季度首个 SJA1000 CAN 网络设备实现）)
- Klaus Hitschler (PEAK driver integration)
  (Klaus Hitschler（PEAK 驱动集成）)
- Uwe Koppe (CAN netdevices with PF_PACKET approach)
  (Uwe Koppe（采用 PF_PACKET 方式的 CAN 网络设备）)
- Michael Schulze (driver layer loopback requirement, RT CAN drivers review)
  (Michael Schulze（驱动层回环需求、RT CAN 驱动评审）)
- Pavel Pisa (Bit-timing calculation)
  (Pavel Pisa（位时序计算）)
- Sascha Hauer (SJA1000 platform driver)
  (Sascha Hauer（SJA1000 平台驱动）)
- Sebastian Haas (SJA1000 EMS PCI driver)
  (Sebastian Haas（SJA1000 EMS PCI 驱动）)
- Markus Plessing (SJA1000 EMS PCI driver)
  (Markus Plessing（SJA1000 EMS PCI 驱动）)
- Per Dalen (SJA1000 Kvaser PCI driver)
  (Per Dalen（SJA1000 Kvaser PCI 驱动）)
- Sam Ravnborg (reviews, coding style, kbuild help)
  (Sam Ravnborg（评审、编码风格、kbuild 帮助）)
