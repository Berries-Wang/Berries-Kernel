.. SPDX-License-Identifier: GPL-2.0

==========
batman-adv
==========

(batman-adv)

Batman advanced is a new approach to wireless networking which does no longer
operate on the IP basis. Unlike the batman daemon, which exchanges information
using UDP packets and sets routing tables, batman-advanced operates on ISO/OSI
Layer 2 only and uses and routes (or better: bridges) Ethernet Frames. It
emulates a virtual network switch of all nodes participating. Therefore all
nodes appear to be link local, thus all higher operating protocols won't be
affected by any changes within the network. You can run almost any protocol
above batman advanced, prominent examples are: IPv4, IPv6, DHCP, IPX.
(Batman advanced 是一种不再基于 IP 运作的无线网络新方案。与使用 UDP 数据包交换信息并设置路由表的
batman 守护进程不同，batman-advanced 仅工作在 ISO/OSI 第 2 层，使用并路由(或者更确切地说：桥接)
以太网帧。它模拟了一个由所有参与节点构成的虚拟网络交换机。因此所有节点都呈现为链路本地节点，网络内部
的任何变化都不会影响更高层运行的协议。几乎所有协议都可以运行在 batman advanced 之上，典型的例子
有：IPv4、IPv6、DHCP、IPX。)

Batman advanced was implemented as a Linux kernel driver to reduce the overhead
to a minimum. It does not depend on any (other) network driver, and can be used
on wifi as well as ethernet lan, vpn, etc ... (anything with ethernet-style
layer 2).
(Batman advanced 被实现为一个 Linux 内核驱动程序，以将开销降到最低。它不依赖任何(其他)网络驱动
程序，既可用于 wifi，也可用于以太网局域网、vpn 等(任何具有以太网风格第 2 层的环境)。)


Configuration(配置)
===================

Load the batman-adv module into your kernel::
(把 batman-adv 模块加载到内核中：)

  $ insmod batman-adv.ko

The module is now waiting for activation. You must add some interfaces on which
batman-adv can operate. The batman-adv mesh-interface can be created using the
iproute2 tool ``ip``::
(模块现在处于等待激活状态。你必须添加一些可供 batman-adv 操作的接口。可以使用 iproute2 工具
``ip`` 来创建 batman-adv mesh 接口：)

  $ ip link add name bat0 type batadv

To activate a given interface simply attach it to the ``bat0`` interface::
(要激活某个给定的接口，只需将它挂接到 ``bat0`` 接口：)

  $ ip link set dev eth0 master bat0

Repeat this step for all interfaces you wish to add. Now batman-adv starts
using/broadcasting on this/these interface(s).
(对你想加入的所有接口重复这一步骤。此时 batman-adv 就会开始在该(这些)接口上使用/广播。)

To deactivate an interface you have to detach it from the "bat0" interface::
(要停用一个接口，必须将它从 "bat0" 接口上分离：)

  $ ip link set dev eth0 nomaster

The same can also be done using the batctl interface subcommand::
(同样的操作也可以通过 batctl 的 interface 子命令完成：)

  batctl -m bat0 interface create
  batctl -m bat0 interface add -M eth0

To detach eth0 and destroy bat0::
(要分离 eth0 并销毁 bat0：)

  batctl -m bat0 interface del -M eth0
  batctl -m bat0 interface destroy

There are additional settings for each batadv mesh interface, vlan and hardif
which can be modified using batctl. Detailed information about this can be found
in its manual.
(每个 batadv mesh 接口、vlan 和 hardif 还有其他一些设置，可以使用 batctl 进行修改。详细信息
请参阅其手册。)

For instance, you can check the current originator interval (value
in milliseconds which determines how often batman-adv sends its broadcast
packets)::
(例如，可以查看当前的 originator interval(以毫秒为单位的值，决定 batman-adv 发送其广播数据包
的频率)：)

  $ batctl -M bat0 orig_interval
  1000

and also change its value::
(也可以修改它的值：)

  $ batctl -M bat0 orig_interval 3000

In very mobile scenarios, you might want to adjust the originator interval to a
lower value. This will make the mesh more responsive to topology changes, but
will also increase the overhead.
(在高度移动的场景中，你可能希望把 originator interval 调整为一个较低的值。这会让 mesh 对拓扑
变化响应更灵敏，但也会增加开销。)

Information about the current state can be accessed via the batadv generic
netlink family. batctl provides a human readable version via its debug tables
subcommands.
(当前状态的信息可以通过 batadv generic netlink family 访问。batctl 通过其 debug tables 子
命令提供了人类可读的版本。)


Usage(使用方法)
===============

To make use of your newly created mesh, batman advanced provides a new
interface "bat0" which you should use from this point on. All interfaces added
to batman advanced are not relevant any longer because batman handles them for
you. Basically, one "hands over" the data by using the batman interface and
batman will make sure it reaches its destination.
(要使用你新建的 mesh，batman advanced 提供了一个新接口 "bat0"，从现在起你应该使用它。所有加入
batman advanced 的接口都不再需要你直接操心了，因为 batman 会替你处理它们。基本上，就是通过
batman 接口"移交"数据，batman 会确保数据到达目的地。)

The "bat0" interface can be used like any other regular interface. It needs an
IP address which can be either statically configured or dynamically (by using
DHCP or similar services)::
("bat0" 接口可以像其他任何普通接口一样使用。它需要一个 IP 地址，既可以静态配置，也可以动态获取
(通过 DHCP 或类似服务)：)

  NodeA: ip link set up dev bat0
  NodeA: ip addr add 192.168.0.1/24 dev bat0

  NodeB: ip link set up dev bat0
  NodeB: ip addr add 192.168.0.2/24 dev bat0
  NodeB: ping 192.168.0.1

Note: In order to avoid problems remove all IP addresses previously assigned to
interfaces now used by batman advanced, e.g.::
(注意：为避免出现问题，请删除之前分配给那些现已由 batman advanced 使用的接口上的所有 IP 地址，
例如：)

  $ ip addr flush dev eth0


Logging/Debugging(日志/调试)
============================

All error messages, warnings and information messages are sent to the kernel
log. Depending on your operating system distribution this can be read in one of
a number of ways. Try using the commands: ``dmesg``, ``logread``, or looking in
the files ``/var/log/kern.log`` or ``/var/log/syslog``. All batman-adv messages
are prefixed with "batman-adv:" So to see just these messages try::
(所有错误消息、警告和信息消息都会发送到内核日志。根据你所用的操作系统发行版，可以通过多种方式之一
读取它。可以尝试使用 ``dmesg``、``logread`` 命令，或者查看文件 ``/var/log/kern.log`` 和
``/var/log/syslog``。所有 batman-adv 消息都带有 "batman-adv:" 前缀，因此要只查看这些消息，
可以尝试：)

  $ dmesg | grep batman-adv

When investigating problems with your mesh network, it is sometimes necessary to
see more detailed debug messages. This must be enabled when compiling the
batman-adv module. When building batman-adv as part of the kernel, use "make
menuconfig" and enable the option ``B.A.T.M.A.N. debugging``
(``CONFIG_BATMAN_ADV_DEBUG=y``).
(排查 mesh 网络问题时，有时需要看到更详细的调试消息。这必须在编译 batman-adv 模块时启用。把
batman-adv 作为内核的一部分构建时，请使用 "make menuconfig" 并启用 ``B.A.T.M.A.N. debugging``
选项(``CONFIG_BATMAN_ADV_DEBUG=y``)。)

Those additional debug messages can be accessed using the perf infrastructure::
(这些额外的调试消息可以通过 perf 基础设施访问：)

  $ trace-cmd stream -e batadv:batadv_dbg

The additional debug output is by default disabled. It can be enabled during
run time::
(额外的调试输出默认是禁用的。可以在运行时启用：)

  $ batctl -m bat0 loglevel routes tt

will enable debug messages for when routes and translation table entries change.
(这将启用针对路由和转换表条目变化的调试消息。)

Counters for different types of packets entering and leaving the batman-adv
module are available through ethtool::
(进入和离开 batman-adv 模块的不同类型数据包的计数器可以通过 ethtool 查看：)

  $ ethtool --statistics bat0


batctl(batctl)
==============

As batman advanced operates on layer 2, all hosts participating in the virtual
switch are completely transparent for all protocols above layer 2. Therefore
the common diagnosis tools do not work as expected. To overcome these problems,
batctl was created. At the moment the batctl contains ping, traceroute, tcpdump
and interfaces to the kernel module settings.
(由于 batman advanced 工作在第 2 层，参与虚拟交换机的所有主机对第 2 层之上的所有协议都是完全
透明的。因此常用的诊断工具无法按预期工作。为了解决这些问题，batctl 便应运而生。目前 batctl
包含 ping、traceroute、tcpdump 以及访问内核模块设置的接口。)

For more information, please see the manpage (``man batctl``).
(更多信息请参阅手册页(``man batctl``)。)

batctl is available on https://www.open-mesh.org/
(batctl 可以在 https://www.open-mesh.org/ 获取。)


Contact(联系方式)
=================

Please send us comments, experiences, questions, anything :)
(欢迎向我们发送评论、经验、问题，任何内容都可以 :) )

IRC:
(IRC)

  #batadv on ircs://irc.hackint.org/

Mailing-list:
(邮件列表)

  b.a.t.m.a.n@lists.open-mesh.org (optional subscription at
  https://lists.open-mesh.org/mailman3/postorius/lists/b.a.t.m.a.n.lists.open-mesh.org/)

You can also contact the Authors:
(你也可以联系作者：)

* Marek Lindner <marek.lindner@mailbox.org>
* Simon Wunderlich <sw@simonwunderlich.de>
