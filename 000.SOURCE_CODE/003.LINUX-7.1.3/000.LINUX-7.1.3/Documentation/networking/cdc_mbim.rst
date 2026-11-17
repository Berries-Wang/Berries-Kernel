.. SPDX-License-Identifier: GPL-2.0

======================================================
cdc_mbim - Driver for CDC MBIM Mobile Broadband modems
======================================================

(cdc_mbim - CDC MBIM 移动宽带调制解调器驱动程序)

The cdc_mbim driver supports USB devices conforming to the "Universal
Serial Bus Communications Class Subclass Specification for Mobile
Broadband Interface Model" [1], which is a further development of
"Universal Serial Bus Communications Class Subclass Specifications for
Network Control Model Devices" [2] optimized for Mobile Broadband
devices, aka "3G/LTE modems".
(cdc_mbim 驱动程序支持符合 "Universal Serial Bus Communications Class Subclass Specification for Mobile Broadband Interface Model" (移动宽带接口模型 USB 通信类子类规范) [1] 的 USB 设备。该规范是 "Universal Serial Bus Communications Class Subclass Specifications for Network Control Model Devices" (网络控制模型设备 USB 通信类子类规范) [2] 的进一步发展，针对移动宽带设备 (即俗称的 "3G/LTE 调制解调器") 进行了优化。)


Command Line Parameters
(命令行参数)
=======================

The cdc_mbim driver has no parameters of its own.  But the probing
behaviour for NCM 1.0 backwards compatible MBIM functions (an
"NCM/MBIM function" as defined in section 3.2 of [1]) is affected
by a cdc_ncm driver parameter:
(cdc_mbim 驱动程序自身没有参数。但是，NCM 1.0 向后兼容的 MBIM 功能 (即 [1] 第 3.2 节定义的 "NCM/MBIM function") 的探测行为会受一个 cdc_ncm 驱动程序参数的影响：)

prefer_mbim
(prefer_mbim 参数)
-----------

:Type:          Boolean
  (类型：Boolean (布尔值))
:Valid Range:   N/Y (0-1)
  (有效范围：N/Y (0-1))
:Default Value: Y (MBIM is preferred)
  (默认值：Y (优先使用 MBIM))

This parameter sets the system policy for NCM/MBIM functions.  Such
functions will be handled by either the cdc_ncm driver or the cdc_mbim
driver depending on the prefer_mbim setting.  Setting prefer_mbim=N
makes the cdc_mbim driver ignore these functions and lets the cdc_ncm
driver handle them instead.
(此参数为 NCM/MBIM 功能设置系统策略。根据 prefer_mbim 的设置，这类功能将由 cdc_ncm 驱动程序或 cdc_mbim 驱动程序处理。设置 prefer_mbim=N 会让 cdc_mbim 驱动程序忽略这些功能，改由 cdc_ncm 驱动程序处理。)

The parameter is writable, and can be changed at any time. A manual
unbind/bind is required to make the change effective for NCM/MBIM
functions bound to the "wrong" driver
(此参数可写，可随时修改。要让修改对已绑定到"错误"驱动程序的 NCM/MBIM 功能生效，需要手动执行一次 unbind/bind)


Basic usage
(基本用法)
===========

MBIM functions are inactive when unmanaged. The cdc_mbim driver only
provides a userspace interface to the MBIM control channel, and will
not participate in the management of the function. This implies that a
userspace MBIM management application always is required to enable a
MBIM function.
(MBIM 功能在被管理时才会活动。cdc_mbim 驱动程序只提供 MBIM 控制通道的用户态接口，不参与功能的管理。这意味着始终需要一个用户态 MBIM 管理应用来启用 MBIM 功能。)

Such userspace applications includes, but are not limited to:
(这类用户态应用包括但不限于：)

 - mbimcli (included with the libmbim [3] library), and
   (mbimcli (随 libmbim [3] 库一同提供)，以及)
 - ModemManager [4]
   (ModemManager [4])

Establishing a MBIM IP session requires at least these actions by the
management application:
(建立 MBIM IP 会话至少需要管理应用执行以下操作：)

 - open the control channel
   (打开控制通道)
 - configure network connection settings
   (配置网络连接设置)
 - connect to network
   (连接到网络)
 - configure IP interface
   (配置 IP 接口)

Management application development
(管理应用开发)
----------------------------------

The driver <-> userspace interfaces are described below.  The MBIM
control channel protocol is described in [1].
(下面介绍驱动程序 <-> 用户态接口。MBIM 控制通道协议在 [1] 中描述。)


MBIM control channel userspace ABI
(MBIM 控制通道用户态 ABI)
==================================

/dev/cdc-wdmX character device
(/dev/cdc-wdmX 字符设备)
------------------------------

The driver creates a two-way pipe to the MBIM function control channel
using the cdc-wdm driver as a subdriver.  The userspace end of the
control channel pipe is a /dev/cdc-wdmX character device.
(驱动程序使用 cdc-wdm 驱动程序作为子驱动程序，创建一条通往 MBIM 功能控制通道的双向管道。该控制通道管道的用户态端是一个 /dev/cdc-wdmX 字符设备。)

The cdc_mbim driver does not process or police messages on the control
channel.  The channel is fully delegated to the userspace management
application.  It is therefore up to this application to ensure that it
complies with all the control channel requirements in [1].
(cdc_mbim 驱动程序不处理、也不管控控制通道上的消息。该通道完全委托给用户态管理应用。因此，确保符合 [1] 中所有控制通道要求是这一应用自己的责任。)

The cdc-wdmX device is created as a child of the MBIM control
interface USB device.  The character device associated with a specific
MBIM function can be looked up using sysfs.  For example::
(cdc-wdmX 设备作为 MBIM 控制接口 USB 设备的子设备创建。与特定 MBIM 功能关联的字符设备可以通过 sysfs 查找。例如：)

 bjorn@nemi:~$ ls /sys/bus/usb/drivers/cdc_mbim/2-4:2.12/usbmisc
 cdc-wdm0

 bjorn@nemi:~$ grep . /sys/bus/usb/drivers/cdc_mbim/2-4:2.12/usbmisc/cdc-wdm0/dev
 180:0


USB configuration descriptors
(USB 配置描述符)
-----------------------------

The wMaxControlMessage field of the CDC MBIM functional descriptor
limits the maximum control message size. The management application is
responsible for negotiating a control message size complying with the
requirements in section 9.3.1 of [1], taking this descriptor field
into consideration.
(CDC MBIM 功能描述符中的 wMaxControlMessage 字段限制了最大控制消息大小。管理应用负责协商一个符合 [1] 第 9.3.1 节要求的控制消息大小，并把该描述符字段考虑在内。)

The userspace application can access the CDC MBIM functional
descriptor of a MBIM function using either of the two USB
configuration descriptor kernel interfaces described in [6] or [7].
(用户态应用可以使用 [6] 或 [7] 中描述的两种 USB 配置描述符内核接口之一，访问 MBIM 功能的 CDC MBIM 功能描述符。)

See also the ioctl documentation below.
(另请参阅下文的 ioctl 文档。)


Fragmentation
(分片)
-------------

The userspace application is responsible for all control message
fragmentation and defragmentaion, as described in section 9.5 of [1].
(用户态应用负责所有控制消息的分片与重组，如 [1] 第 9.5 节所述。)


/dev/cdc-wdmX write()
(/dev/cdc-wdmX write())
---------------------

The MBIM control messages from the management application *must not*
exceed the negotiated control message size.
(来自管理应用的 MBIM 控制消息 *不得超过* 协商好的控制消息大小。)


/dev/cdc-wdmX read()
(/dev/cdc-wdmX read())
--------------------

The management application *must* accept control messages of up the
negotiated control message size.
(管理应用 *必须* 能接受最大达到协商控制消息大小的控制消息。)


/dev/cdc-wdmX ioctl()
(/dev/cdc-wdmX ioctl())
---------------------
IOCTL_WDM_MAX_COMMAND: Get Maximum Command Size
(IOCTL_WDM_MAX_COMMAND：获取最大命令大小)
This ioctl returns the wMaxControlMessage field of the CDC MBIM
functional descriptor for MBIM devices.  This is intended as a
convenience, eliminating the need to parse the USB descriptors from
userspace.
(此 ioctl 返回 MBIM 设备的 CDC MBIM 功能描述符中的 wMaxControlMessage 字段。这是为方便起见而提供的，免去用户态解析 USB 描述符的需要。)

::

	#include <stdio.h>
	#include <fcntl.h>
	#include <sys/ioctl.h>
	#include <linux/types.h>
	#include <linux/usb/cdc-wdm.h>
	int main()
	{
		__u16 max;
		int fd = open("/dev/cdc-wdm0", O_RDWR);
		if (!ioctl(fd, IOCTL_WDM_MAX_COMMAND, &max))
			printf("wMaxControlMessage is %d\n", max);
	}


Custom device services
(自定义设备服务)
----------------------

The MBIM specification allows vendors to freely define additional
services.  This is fully supported by the cdc_mbim driver.
(MBIM 规范允许厂商自由定义额外的服务。cdc_mbim 驱动程序完全支持这一点。)

Support for new MBIM services, including vendor specified services, is
implemented entirely in userspace, like the rest of the MBIM control
protocol
(对新 MBIM 服务 (包括厂商指定服务) 的支持完全在用户态实现，与 MBIM 控制协议的其余部分一样)

New services should be registered in the MBIM Registry [5].
(新服务应在 MBIM Registry [5] 中注册。)



MBIM data channel userspace ABI
(MBIM 数据通道用户态 ABI)
===============================

wwanY network device
(wwanY 网络设备)
--------------------

The cdc_mbim driver represents the MBIM data channel as a single
network device of the "wwan" type. This network device is initially
mapped to MBIM IP session 0.
(cdc_mbim 驱动程序把 MBIM 数据通道表示为一个 "wwan" 类型的单一网络设备。该网络设备最初映射到 MBIM IP 会话 0。)


Multiplexed IP sessions (IPS)
(多路复用 IP 会话 (IPS))
-----------------------------

MBIM allows multiplexing up to 256 IP sessions over a single USB data
channel.  The cdc_mbim driver models such IP sessions as 802.1q VLAN
subdevices of the master wwanY device, mapping MBIM IP session Z to
VLAN ID Z for all values of Z greater than 0.
(MBIM 允许在单一 USB 数据通道上多路复用最多 256 个 IP 会话。cdc_mbim 驱动程序把这些 IP 会话建模为主 wwanY 设备的 802.1q VLAN 子设备，对每个大于 0 的 Z，把 MBIM IP 会话 Z 映射到 VLAN ID Z。)

The device maximum Z is given in the MBIM_DEVICE_CAPS_INFO structure
described in section 10.5.1 of [1].
(设备支持的最大 Z 值由 [1] 第 10.5.1 节描述的 MBIM_DEVICE_CAPS_INFO 结构给出。)

The userspace management application is responsible for adding new
VLAN links prior to establishing MBIM IP sessions where the SessionId
is greater than 0. These links can be added by using the normal VLAN
kernel interfaces, either ioctl or netlink.
(用户态管理应用负责在建立 SessionId 大于 0 的 MBIM IP 会话之前添加新的 VLAN 链路。这些链路可以用普通的 VLAN 内核接口添加，ioctl 或 netlink 皆可。)

For example, adding a link for a MBIM IP session with SessionId 3::
(例如，为 SessionId 为 3 的 MBIM IP 会话添加链路：)

  ip link add link wwan0 name wwan0.3 type vlan id 3

The driver will automatically map the "wwan0.3" network device to MBIM
IP session 3.
(驱动程序会自动把 "wwan0.3" 网络设备映射到 MBIM IP 会话 3。)


Device Service Streams (DSS)
(设备服务流 (DSS))
----------------------------

MBIM also allows up to 256 non-IP data streams to be multiplexed over
the same shared USB data channel.  The cdc_mbim driver models these
sessions as another set of 802.1q VLAN subdevices of the master wwanY
device, mapping MBIM DSS session A to VLAN ID (256 + A) for all values
of A.
(MBIM 还允许在同一个共享 USB 数据通道上多路复用最多 256 个非 IP 数据流。cdc_mbim 驱动程序把这些会话建模为主 wwanY 设备的另一组 802.1q VLAN 子设备，对每个 A 值，把 MBIM DSS 会话 A 映射到 VLAN ID (256 + A)。)

The device maximum A is given in the MBIM_DEVICE_SERVICES_INFO
structure described in section 10.5.29 of [1].
(设备支持的最大 A 值由 [1] 第 10.5.29 节描述的 MBIM_DEVICE_SERVICES_INFO 结构给出。)

The DSS VLAN subdevices are used as a practical interface between the
shared MBIM data channel and a MBIM DSS aware userspace application.
It is not intended to be presented as-is to an end user. The
assumption is that a userspace application initiating a DSS session
also takes care of the necessary framing of the DSS data, presenting
the stream to the end user in an appropriate way for the stream type.
(DSS VLAN 子设备被用作共享 MBIM 数据通道与感知 MBIM DSS 的用户态应用之间的实用接口。它并不打算原样呈现给最终用户。其假定是：发起 DSS 会话的用户态应用也会负责 DSS 数据所需的成帧处理，并按流类型以适当方式把数据流呈现给最终用户。)

The network device ABI requires a dummy ethernet header for every DSS
data frame being transported.  The contents of this header is
arbitrary, with the following exceptions:
(网络设备 ABI 要求每个被传输的 DSS 数据帧都携带一个虚拟以太网头。该头的内容是任意的，但有以下例外：)

 - TX frames using an IP protocol (0x0800 or 0x86dd) will be dropped
   (使用 IP 协议 (0x0800 或 0x86dd) 的 TX 帧将被丢弃)
 - RX frames will have the protocol field set to ETH_P_802_3 (but will
   not be properly formatted 802.3 frames)
   (RX 帧的协议字段会被设为 ETH_P_802_3 (但不是格式正确的 802.3 帧))
 - RX frames will have the destination address set to the hardware
   address of the master device
   (RX 帧的目的地址会被设为主设备的硬件地址)

The DSS supporting userspace management application is responsible for
adding the dummy ethernet header on TX and stripping it on RX.
(支持 DSS 的用户态管理应用负责在 TX 时添加这个虚拟以太网头，并在 RX 时剥除它。)

This is a simple example using tools commonly available, exporting
DssSessionId 5 as a pty character device pointed to by a /dev/nmea
symlink::
(下面是一个使用常用工具的简单示例，把 DssSessionId 5 导出为一个 pty 字符设备，并由 /dev/nmea 符号链接指向它：)

  ip link add link wwan0 name wwan0.dss5 type vlan id 261
  ip link set dev wwan0.dss5 up
  socat INTERFACE:wwan0.dss5,type=2 PTY:,echo=0,link=/dev/nmea

This is only an example, most suitable for testing out a DSS
service. Userspace applications supporting specific MBIM DSS services
are expected to use the tools and programming interfaces required by
that service.
(这只是一个示例，最适合用来试验 DSS 服务。支持特定 MBIM DSS 服务的用户态应用应当使用该服务所要求的工具和编程接口。)

Note that adding VLAN links for DSS sessions is entirely optional.  A
management application may instead choose to bind a packet socket
directly to the master network device, using the received VLAN tags to
map frames to the correct DSS session and adding 18 byte VLAN ethernet
headers with the appropriate tag on TX.  In this case using a socket
filter is recommended, matching only the DSS VLAN subset. This avoid
unnecessary copying of unrelated IP session data to userspace.  For
example::
(注意，为 DSS 会话添加 VLAN 链路是完全可选的。管理应用也可以选择把一个包套接字直接绑定到主网络设备，利用收到的 VLAN 标签把帧映射到正确的 DSS 会话，并在 TX 时添加带有适当标签的 18 字节 VLAN 以太网头。在这种情况下，建议使用套接字过滤器，只匹配 DSS VLAN 子集。这样可以避免把无关的 IP 会话数据复制到用户态。例如：)

  static struct sock_filter dssfilter[] = {
	/* use special negative offsets to get VLAN tag */
	BPF_STMT(BPF_LD|BPF_B|BPF_ABS, SKF_AD_OFF + SKF_AD_VLAN_TAG_PRESENT),
	BPF_JUMP(BPF_JMP|BPF_JEQ|BPF_K, 1, 0, 6), /* true */

	/* verify DSS VLAN range */
	BPF_STMT(BPF_LD|BPF_H|BPF_ABS, SKF_AD_OFF + SKF_AD_VLAN_TAG),
	BPF_JUMP(BPF_JMP|BPF_JGE|BPF_K, 256, 0, 4),	/* 256 is first DSS VLAN */
	BPF_JUMP(BPF_JMP|BPF_JGE|BPF_K, 512, 3, 0),	/* 511 is last DSS VLAN */

	/* verify ethertype */
	BPF_STMT(BPF_LD|BPF_H|BPF_ABS, 2 * ETH_ALEN),
	BPF_JUMP(BPF_JMP|BPF_JEQ|BPF_K, ETH_P_802_3, 0, 1),

	BPF_STMT(BPF_RET|BPF_K, (u_int)-1),	/* accept */
	BPF_STMT(BPF_RET|BPF_K, 0),		/* ignore */
  };



Tagged IP session 0 VLAN
(带标签的 IP 会话 0 VLAN)
------------------------

As described above, MBIM IP session 0 is treated as special by the
driver.  It is initially mapped to untagged frames on the wwanY
network device.
(如上所述，MBIM IP 会话 0 被驱动程序视为特殊会话。它最初映射到 wwanY 网络设备上的无标签帧。)

This mapping implies a few restrictions on multiplexed IPS and DSS
sessions, which may not always be practical:
(这种映射对多路复用的 IPS 和 DSS 会话带来了一些限制，而这些限制并不总是切合实际：)

 - no IPS or DSS session can use a frame size greater than the MTU on
   IP session 0
   (任何 IPS 或 DSS 会话都不能使用大于 IP 会话 0 上 MTU 的帧大小)
 - no IPS or DSS session can be in the up state unless the network
   device representing IP session 0 also is up
   (除非代表 IP 会话 0 的网络设备也处于 up 状态，否则任何 IPS 或 DSS 会话都不能进入 up 状态)

These problems can be avoided by optionally making the driver map IP
session 0 to a VLAN subdevice, similar to all other IP sessions.  This
behaviour is triggered by adding a VLAN link for the magic VLAN ID
4094.  The driver will then immediately start mapping MBIM IP session
0 to this VLAN, and will drop untagged frames on the master wwanY
device.
(这些问题可以通过让驱动程序把 IP 会话 0 也映射到一个 VLAN 子设备 (与其他所有 IP 会话类似) 来避免。只需为神奇的 VLAN ID 4094 添加一条 VLAN 链路即可触发此行为。此后驱动程序会立即开始把 MBIM IP 会话 0 映射到这个 VLAN，并丢弃主 wwanY 设备上的无标签帧。)

Tip: It might be less confusing to the end user to name this VLAN
subdevice after the MBIM SessionID instead of the VLAN ID.  For
example::
(提示：用 MBIM SessionID 而不是 VLAN ID 来命名这个 VLAN 子设备，可能更不容易让最终用户困惑。例如：)

  ip link add link wwan0 name wwan0.0 type vlan id 4094


VLAN mapping
(VLAN 映射)
------------

Summarizing the cdc_mbim driver mapping described above, we have this
relationship between VLAN tags on the wwanY network device and MBIM
sessions on the shared USB data channel::
(总结上述 cdc_mbim 驱动程序的映射：wwanY 网络设备上的 VLAN 标签与共享 USB 数据通道上的 MBIM 会话之间有以下关系。)

  VLAN ID       MBIM type   MBIM SessionID           Notes
  ---------------------------------------------------------
  untagged      IPS         0                        a)
  1 - 255       IPS         1 - 255 <VLANID>
  256 - 511     DSS         0 - 255 <VLANID - 256>
  512 - 4093                                         b)
  4094          IPS         0                        c)

    a) if no VLAN ID 4094 link exists, else dropped
    b) unsupported VLAN range, unconditionally dropped
    c) if a VLAN ID 4094 link exists, else dropped




References
(参考文献)
==========

 1) USB Implementers Forum, Inc. - "Universal Serial Bus
    Communications Class Subclass Specification for Mobile Broadband
    Interface Model", Revision 1.0 (Errata 1), May 1, 2013
    (USB Implementers Forum, Inc. - "移动宽带接口模型 USB 通信类子类规范"，1.0 修订版 (勘误 1)，2013 年 5 月 1 日)

      - http://www.usb.org/developers/docs/devclass_docs/

 2) USB Implementers Forum, Inc. - "Universal Serial Bus
    Communications Class Subclass Specifications for Network Control
    Model Devices", Revision 1.0 (Errata 1), November 24, 2010
    (USB Implementers Forum, Inc. - "网络控制模型设备 USB 通信类子类规范"，1.0 修订版 (勘误 1)，2010 年 11 月 24 日)

      - http://www.usb.org/developers/docs/devclass_docs/

 3) libmbim - "a glib-based library for talking to WWAN modems and
    devices which speak the Mobile Interface Broadband Model (MBIM)
    protocol"
    (libmbim - "一个基于 glib 的库，用于与 WWAN 调制解调器以及使用移动接口宽带模型 (MBIM) 协议的设备通信")

      - http://www.freedesktop.org/wiki/Software/libmbim/

 4) ModemManager - "a DBus-activated daemon which controls mobile
    broadband (2G/3G/4G) devices and connections"
    (ModemManager - "一个由 DBus 激活的守护进程，用于控制移动宽带 (2G/3G/4G) 设备和连接")

      - http://www.freedesktop.org/wiki/Software/ModemManager/

 5) "MBIM (Mobile Broadband Interface Model) Registry"
    ("MBIM (移动宽带接口模型) 注册表")

       - http://compliance.usb.org/mbim/

 6) "/sys/kernel/debug/usb/devices output format"
    ("/sys/kernel/debug/usb/devices 输出格式")

       - Documentation/driver-api/usb/usb.rst

 7) "/sys/bus/usb/devices/.../descriptors"
    ("/sys/bus/usb/devices/.../descriptors")

       - Documentation/ABI/stable/sysfs-bus-usb
