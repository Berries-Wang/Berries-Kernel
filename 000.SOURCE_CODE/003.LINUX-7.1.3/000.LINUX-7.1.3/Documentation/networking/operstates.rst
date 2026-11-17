.. SPDX-License-Identifier: GPL-2.0

============================
Operational States(运行状态)
============================


1. Introduction(1. 简介)
========================

Linux distinguishes between administrative and operational state of an
interface. Administrative state is the result of "ip link set dev
<dev> up or down" and reflects whether the administrator wants to use
the device for traffic.
(Linux 区分接口的管理状态 (administrative state) 和运行状态 (operational state)。管理状态是 "ip link set dev <dev> up or down" 的结果，反映管理员是否想使用该设备传输流量。)

However, an interface is not usable just because the admin enabled it
- ethernet requires to be plugged into the switch and, depending on
a site's networking policy and configuration, an 802.1X authentication
to be performed before user data can be transferred. Operational state
shows the ability of an interface to transmit this user data.
(然而，接口并非只要管理员启用就可用了 - 以太网需要插入交换机，并且根据站点的网络策略和配置，在用户数据能够传输之前可能还需要进行 802.1X 认证。运行状态显示的是接口传输这些用户数据的能力。)

Thanks to 802.1X, userspace must be granted the possibility to
influence operational state. To accommodate this, operational state is
split into two parts: Two flags that can be set by the driver only, and
a RFC2863 compatible state that is derived from these flags, a policy,
and changeable from userspace under certain rules.
(由于 802.1X 的存在，必须允许用户态影响运行状态。为此，运行状态被拆分为两部分：两个只能由驱动设置的标志，以及一个与 RFC2863 兼容的状态——该状态由这两个标志和一项策略推导得出，并可在一定规则下由用户态修改。)


2. Querying from userspace(2. 从用户态查询)
===========================================

Both admin and operational state can be queried via the netlink
operation RTM_GETLINK. It is also possible to subscribe to RTNLGRP_LINK
to be notified of updates while the interface is admin up. This is
important for setting from userspace.
(管理状态和运行状态都可以通过 netlink 操作 RTM_GETLINK 查询。也可以订阅 RTNLGRP_LINK，在接口处于管理性 up 状态期间获得更新通知。这对从用户态进行设置很重要。)

These values contain interface state:
(以下值包含接口状态：)

ifinfomsg::if_flags & IFF_UP:
(ifinfomsg::if_flags & IFF_UP：)

 Interface is admin up
 (接口处于管理性 up 状态。)

ifinfomsg::if_flags & IFF_RUNNING:
(ifinfomsg::if_flags & IFF_RUNNING：)

 Interface is in RFC2863 operational state UP or UNKNOWN. This is for
 backward compatibility, routing daemons, dhcp clients can use this
 flag to determine whether they should use the interface.
 (接口处于 RFC2863 运行状态 UP 或 UNKNOWN。这是为了向后兼容，路由守护进程、DHCP 客户端可以用这个标志来决定是否使用该接口。)

ifinfomsg::if_flags & IFF_LOWER_UP:
(ifinfomsg::if_flags & IFF_LOWER_UP：)

 Driver has signaled netif_carrier_on()
 (驱动已调用 netif_carrier_on() 发出信号。)

ifinfomsg::if_flags & IFF_DORMANT:
(ifinfomsg::if_flags & IFF_DORMANT：)

 Driver has signaled netif_dormant_on()
 (驱动已调用 netif_dormant_on() 发出信号。)

TLV IFLA_OPERSTATE
------------------

contains RFC2863 state of the interface in numeric representation:
(以数值形式包含接口的 RFC2863 状态：)

IF_OPER_UNKNOWN (0):
(IF_OPER_UNKNOWN (0)：)

 Interface is in unknown state, neither driver nor userspace has set
 operational state. Interface must be considered for user data as
 setting operational state has not been implemented in every driver.
 (接口处于未知状态，驱动和用户态都没有设置运行状态。用户数据传输必须考虑该接口，因为并非每个驱动都实现了运行状态设置。)

IF_OPER_NOTPRESENT (1):
(IF_OPER_NOTPRESENT (1)：)

 Unused in current kernel (notpresent interfaces normally disappear),
 just a numerical placeholder.
 (当前内核未使用（notpresent 的接口通常会直接消失），只是一个数值占位符。)

IF_OPER_DOWN (2):
(IF_OPER_DOWN (2)：)

 Interface is unable to transfer data on L1, f.e. ethernet is not
 plugged or interface is ADMIN down.
 (接口无法在 L1 层传输数据，例如以太网线未插好，或接口处于管理性 down 状态。)

IF_OPER_LOWERLAYERDOWN (3):
(IF_OPER_LOWERLAYERDOWN (3)：)

 Interfaces stacked on an interface that is IF_OPER_DOWN show this
 state (f.e. VLAN).
 (叠加在处于 IF_OPER_DOWN 状态的接口之上的接口会显示此状态（例如 VLAN）。)

IF_OPER_TESTING (4):
(IF_OPER_TESTING (4)：)

 Interface is in testing mode, for example executing driver self-tests
 or media (cable) test. It can't be used for normal traffic until tests
 complete.
 (接口处于测试模式，例如正在执行驱动自检或介质（网线）测试。在测试完成之前不能用于正常流量。)

IF_OPER_DORMANT (5):
(IF_OPER_DORMANT (5)：)

 Interface is L1 up, but waiting for an external event, f.e. for a
 protocol to establish. (802.1X)
 (接口 L1 层已 up，但正在等待某个外部事件，例如等待某个协议建立。(802.1X))

IF_OPER_UP (6):
(IF_OPER_UP (6)：)

 Interface is operational up and can be used.
 (接口运行正常，可以使用。)

This TLV can also be queried via sysfs.
(此 TLV 也可以通过 sysfs 查询。)

TLV IFLA_LINKMODE
-----------------

contains link policy. This is needed for userspace interaction
described below.
(包含链路策略。下面描述的用户态交互需要用到它。)

This TLV can also be queried via sysfs.
(此 TLV 也可以通过 sysfs 查询。)


3. Kernel driver API(3. 内核驱动 API)
=====================================

Kernel drivers have access to two flags that map to IFF_LOWER_UP and
IFF_DORMANT. These flags can be set from everywhere, even from
interrupts. It is guaranteed that only the driver has write access,
however, if different layers of the driver manipulate the same flag,
the driver has to provide the synchronisation needed.
(内核驱动可以访问两个标志，它们分别映射到 IFF_LOWER_UP 和 IFF_DORMANT。这些标志可以在任何地方设置，甚至可以在中断中设置。保证只有驱动拥有写权限；但如果驱动的不同层次操作同一个标志，驱动必须自行提供所需的同步。)

__LINK_STATE_NOCARRIER, maps to !IFF_LOWER_UP:
(__LINK_STATE_NOCARRIER，映射到 !IFF_LOWER_UP：)

The driver uses netif_carrier_on() to clear and netif_carrier_off() to
set this flag. On netif_carrier_off(), the scheduler stops sending
packets. The name 'carrier' and the inversion are historical, think of
it as lower layer.
(驱动使用 netif_carrier_on() 清除、使用 netif_carrier_off() 设置此标志。调用 netif_carrier_off() 时，调度器停止发送数据包。"carrier" 这个名字及其反转是历史遗留，可以把它理解为更底层的意思。)

Note that for certain kind of soft-devices, which are not managing any
real hardware, it is possible to set this bit from userspace.  One
should use TLV IFLA_CARRIER to do so.
(注意，对于不管理任何真实硬件的某类软设备，可以从用户态设置此位。应使用 TLV IFLA_CARRIER 来完成。)

netif_carrier_ok() can be used to query that bit.
(netif_carrier_ok() 可用于查询该位。)

__LINK_STATE_DORMANT, maps to IFF_DORMANT:
(__LINK_STATE_DORMANT，映射到 IFF_DORMANT：)

Set by the driver to express that the device cannot yet be used
because some driver controlled protocol establishment has to
complete. Corresponding functions are netif_dormant_on() to set the
flag, netif_dormant_off() to clear it and netif_dormant() to query.
(由驱动设置，表示设备尚不可用，因为某个由驱动控制的协议建立过程必须先完成。相应的函数是：netif_dormant_on() 设置标志，netif_dormant_off() 清除标志，netif_dormant() 查询。)

On device allocation, both flags __LINK_STATE_NOCARRIER and
__LINK_STATE_DORMANT are cleared, so the effective state is equivalent
to netif_carrier_ok() and !netif_dormant().
(设备分配时，__LINK_STATE_NOCARRIER 和 __LINK_STATE_DORMANT 两个标志都被清除，因此等效状态等价于 netif_carrier_ok() 且 !netif_dormant()。)


Whenever the driver CHANGES one of these flags, a workqueue event is
scheduled to translate the flag combination to IFLA_OPERSTATE as
follows:
(每当驱动更改其中一个标志时，都会调度一个 workqueue 事件，将标志组合转换为 IFLA_OPERSTATE，规则如下：)

!netif_carrier_ok():
(!netif_carrier_ok()：)

 IF_OPER_LOWERLAYERDOWN if the interface is stacked, IF_OPER_DOWN
 otherwise. Kernel can recognise stacked interfaces because their
 ifindex != iflink.
 (若接口是叠加接口则为 IF_OPER_LOWERLAYERDOWN，否则为 IF_OPER_DOWN。内核可以识别叠加接口，因为它们的 ifindex != iflink。)

netif_carrier_ok() && netif_dormant():
(netif_carrier_ok() && netif_dormant()：)

 IF_OPER_DORMANT
 (IF_OPER_DORMANT)

netif_carrier_ok() && !netif_dormant():
(netif_carrier_ok() && !netif_dormant()：)

 IF_OPER_UP if userspace interaction is disabled. Otherwise
 IF_OPER_DORMANT with the possibility for userspace to initiate the
 IF_OPER_UP transition afterwards.
 (若禁用了用户态交互则为 IF_OPER_UP。否则为 IF_OPER_DORMANT，之后用户态可以发起向 IF_OPER_UP 的转换。)


4. Setting from userspace(4. 从用户态设置)
==========================================

Applications have to use the netlink interface to influence the
RFC2863 operational state of an interface. Setting IFLA_LINKMODE to 1
via RTM_SETLINK instructs the kernel that an interface should go to
IF_OPER_DORMANT instead of IF_OPER_UP when the combination
netif_carrier_ok() && !netif_dormant() is set by the
driver. Afterwards, the userspace application can set IFLA_OPERSTATE
to IF_OPER_DORMANT or IF_OPER_UP as long as the driver does not set
netif_carrier_off() or netif_dormant_on(). Changes made by userspace
are multicasted on the netlink group RTNLGRP_LINK.
(应用程序必须使用 netlink 接口来影响接口的 RFC2863 运行状态。通过 RTM_SETLINK 将 IFLA_LINKMODE 设为 1，即可指示内核：当驱动设置了 netif_carrier_ok() && !netif_dormant() 组合时，接口应进入 IF_OPER_DORMANT 而不是 IF_OPER_UP。之后，只要驱动没有设置 netif_carrier_off() 或 netif_dormant_on()，用户态应用程序就可以将 IFLA_OPERSTATE 设为 IF_OPER_DORMANT 或 IF_OPER_UP。用户态所做的更改会通过 netlink 组 RTNLGRP_LINK 组播。)

So basically a 802.1X supplicant interacts with the kernel like this:
(基本上，一个 802.1X 请求者 (supplicant) 与内核的交互方式如下：)

- subscribe to RTNLGRP_LINK
  (订阅 RTNLGRP_LINK)
- set IFLA_LINKMODE to 1 via RTM_SETLINK
  (通过 RTM_SETLINK 将 IFLA_LINKMODE 设为 1)
- query RTM_GETLINK once to get initial state
  (查询一次 RTM_GETLINK 以获得初始状态)
- if initial flags are not (IFF_LOWER_UP && !IFF_DORMANT), wait until
  netlink multicast signals this state
  (如果初始标志不是 (IFF_LOWER_UP && !IFF_DORMANT)，则等待 netlink 组播通知这一状态)
- do 802.1X, eventually abort if flags go down again
  (执行 802.1X；若标志再次 down 则最终中止)
- send RTM_SETLINK to set operstate to IF_OPER_UP if authentication
  succeeds, IF_OPER_DORMANT otherwise
  (发送 RTM_SETLINK：认证成功则将 operstate 设为 IF_OPER_UP，否则设为 IF_OPER_DORMANT)
- see how operstate and IFF_RUNNING is echoed via netlink multicast
  (观察 operstate 和 IFF_RUNNING 如何通过 netlink 组播回传)
- set interface back to IF_OPER_DORMANT if 802.1X reauthentication
  fails
  (如果 802.1X 重认证失败，将接口设回 IF_OPER_DORMANT)
- restart if kernel changes IFF_LOWER_UP or IFF_DORMANT flag
  (如果内核更改了 IFF_LOWER_UP 或 IFF_DORMANT 标志则重新开始)

if supplicant goes down, bring back IFLA_LINKMODE to 0 and
IFLA_OPERSTATE to a sane value.
(如果请求者进程退出，应将 IFLA_LINKMODE 恢复为 0，并将 IFLA_OPERSTATE 恢复为一个合理的值。)

A routing daemon or dhcp client just needs to care for IFF_RUNNING or
waiting for operstate to go IF_OPER_UP/IF_OPER_UNKNOWN before
considering the interface / querying a DHCP address.
(路由守护进程或 DHCP 客户端只需关注 IFF_RUNNING，或等待 operstate 变为 IF_OPER_UP/IF_OPER_UNKNOWN 之后，再考虑使用该接口/查询 DHCP 地址。)


For technical questions and/or comments please e-mail to Stefan Rompf
(stefan at loplof.de).
(如需技术问题和/或意见，请发送电子邮件给 Stefan Rompf (stefan at loplof.de)。)
