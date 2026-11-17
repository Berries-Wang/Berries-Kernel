.. SPDX-License-Identifier: GPL-2.0

==================================================================
DSA switch configuration from userspace(从用户空间配置 DSA 交换机)
==================================================================

The DSA switch configuration is not integrated into the main userspace
network configuration suites by now and has to be performed manually.
(到目前为止，DSA 交换机的配置尚未集成到主流的用户空间网络配置套件中，需要手动
完成。)

.. _dsa-config-showcases:

Configuration showcases(配置示例)
---------------------------------

To configure a DSA switch a couple of commands need to be executed. In this
documentation some common configuration scenarios are handled as showcases:
(要配置一台 DSA 交换机，需要执行若干条命令。本文档以示例的形式讲解几种常见的
配置场景：)

*single port*
(单端口)

  Every switch port acts as a different configurable Ethernet port
  (每个交换机端口都各自作为一个独立的、可配置的以太网端口)

*bridge*
(网桥)

  Every switch port is part of one configurable Ethernet bridge
  (每个交换机端口都属于同一个可配置的以太网网桥)

*gateway*
(网关)

  Every switch port except one upstream port is part of a configurable
  Ethernet bridge.
  The upstream port acts as different configurable Ethernet port.
  (除一个上行端口外，其余所有交换机端口都属于一个可配置的以太网网桥。
  上行端口则各自作为一个独立的、可配置的以太网端口。)

All configurations are performed with tools from iproute2, which is available
at https://www.kernel.org/pub/linux/utils/net/iproute2/
(所有配置均使用 iproute2 提供的工具完成，该工具集可在
https://www.kernel.org/pub/linux/utils/net/iproute2/ 获取。)

Through DSA every port of a switch is handled like a normal linux Ethernet
interface. The CPU port is the switch port connected to an Ethernet MAC chip.
The corresponding linux Ethernet interface is called the conduit interface.
All other corresponding linux interfaces are called user interfaces.
(通过 DSA，交换机的每个端口都被当作一个普通的 Linux 以太网接口来处理。CPU 端口
是交换机上连接到以太网 MAC 芯片的那个端口。与之对应的 Linux 以太网接口称为
conduit 接口（管理接口），其余对应的 Linux 接口称为用户接口。)

The user interfaces depend on the conduit interface being up in order for them
to send or receive traffic. Prior to kernel v5.12, the state of the conduit
interface had to be managed explicitly by the user. Starting with kernel v5.12,
the behavior is as follows:
(用户接口需要 conduit 接口处于 up 状态才能收发流量。在内核 v5.12 之前，conduit
接口的状态必须由用户显式管理。从内核 v5.12 开始，其行为如下：)

- when a DSA user interface is brought up, the conduit interface is
  automatically brought up.
  (当某个 DSA 用户接口被启用时，conduit 接口会被自动启用。)
- when the conduit interface is brought down, all DSA user interfaces are
  automatically brought down.
  (当 conduit 接口被停用时，所有 DSA 用户接口都会被自动停用。)

In this documentation the following Ethernet interfaces are used:
(本文档使用以下以太网接口：)

*eth0*
(eth0)

  the conduit interface
  (conduit 接口)

*eth1*
(eth1)

  another conduit interface
  (另一个 conduit 接口)

*lan1*
(lan1)

  a user interface
  (一个用户接口)

*lan2*
(lan2)

  another user interface
  (另一个用户接口)

*lan3*
(lan3)

  a third user interface
  (第三个用户接口)

*wan*
(wan)

  A user interface dedicated for upstream traffic
  (专用于上行流量的用户接口)

Further Ethernet interfaces can be configured similar.
The configured IPs and networks are:
(其余以太网接口可用类似方式配置。所配置的 IP 地址与网络如下：)

*single port*
(单端口)

  * lan1: 192.0.2.1/30 (192.0.2.0 - 192.0.2.3)
  * lan2: 192.0.2.5/30 (192.0.2.4 - 192.0.2.7)
  * lan3: 192.0.2.9/30 (192.0.2.8 - 192.0.2.11)

*bridge*
(网桥)

  * br0: 192.0.2.129/25 (192.0.2.128 - 192.0.2.255)

*gateway*
(网关)

  * br0: 192.0.2.129/25 (192.0.2.128 - 192.0.2.255)
  * wan: 192.0.2.1/30 (192.0.2.0 - 192.0.2.3)

.. _dsa-tagged-configuration:

Configuration with tagging support(基于打标签支持的配置)
--------------------------------------------------------

The tagging based configuration is desired and supported by the majority of
DSA switches. These switches are capable to tag incoming and outgoing traffic
without using a VLAN based configuration.
(基于打标签的配置方式是首选方案，并为大多数 DSA 交换机所支持。这类交换机无需
借助基于 VLAN 的配置，即可对出入流量打标签。)

*single port*
(单端口)

  .. code-block:: sh

    # configure each interface
    ip addr add 192.0.2.1/30 dev lan1
    ip addr add 192.0.2.5/30 dev lan2
    ip addr add 192.0.2.9/30 dev lan3

    # For kernels earlier than v5.12, the conduit interface needs to be
    # brought up manually before the user ports.
    ip link set eth0 up

    # bring up the user interfaces
    ip link set lan1 up
    ip link set lan2 up
    ip link set lan3 up

*bridge*
(网桥)

  .. code-block:: sh

    # For kernels earlier than v5.12, the conduit interface needs to be
    # brought up manually before the user ports.
    ip link set eth0 up

    # bring up the user interfaces
    ip link set lan1 up
    ip link set lan2 up
    ip link set lan3 up

    # create bridge
    ip link add name br0 type bridge

    # add ports to bridge
    ip link set dev lan1 master br0
    ip link set dev lan2 master br0
    ip link set dev lan3 master br0

    # configure the bridge
    ip addr add 192.0.2.129/25 dev br0

    # bring up the bridge
    ip link set dev br0 up

*gateway*
(网关)

  .. code-block:: sh

    # For kernels earlier than v5.12, the conduit interface needs to be
    # brought up manually before the user ports.
    ip link set eth0 up

    # bring up the user interfaces
    ip link set wan up
    ip link set lan1 up
    ip link set lan2 up

    # configure the upstream port
    ip addr add 192.0.2.1/30 dev wan

    # create bridge
    ip link add name br0 type bridge

    # add ports to bridge
    ip link set dev lan1 master br0
    ip link set dev lan2 master br0

    # configure the bridge
    ip addr add 192.0.2.129/25 dev br0

    # bring up the bridge
    ip link set dev br0 up

.. _dsa-vlan-configuration:

Configuration without tagging support(无打标签支持时的配置)
-----------------------------------------------------------

A minority of switches are not capable to use a taging protocol
(DSA_TAG_PROTO_NONE). These switches can be configured by a VLAN based
configuration.
(少数交换机无法使用打标签协议 (DSA_TAG_PROTO_NONE)。这类交换机可以采用基于 VLAN
的配置方式。)

*single port*
(单端口)

  The configuration can only be set up via VLAN tagging and bridge setup.
  (这种配置只能通过 VLAN 打标签和网桥设置来完成。)

  .. code-block:: sh

    # tag traffic on CPU port
    ip link add link eth0 name eth0.1 type vlan id 1
    ip link add link eth0 name eth0.2 type vlan id 2
    ip link add link eth0 name eth0.3 type vlan id 3

    # For kernels earlier than v5.12, the conduit interface needs to be
    # brought up manually before the user ports.
    ip link set eth0 up
    ip link set eth0.1 up
    ip link set eth0.2 up
    ip link set eth0.3 up

    # bring up the user interfaces
    ip link set lan1 up
    ip link set lan2 up
    ip link set lan3 up

    # create bridge
    ip link add name br0 type bridge

    # activate VLAN filtering
    ip link set dev br0 type bridge vlan_filtering 1

    # add ports to bridges
    ip link set dev lan1 master br0
    ip link set dev lan2 master br0
    ip link set dev lan3 master br0

    # tag traffic on ports
    bridge vlan add dev lan1 vid 1 pvid untagged
    bridge vlan add dev lan2 vid 2 pvid untagged
    bridge vlan add dev lan3 vid 3 pvid untagged

    # configure the VLANs
    ip addr add 192.0.2.1/30 dev eth0.1
    ip addr add 192.0.2.5/30 dev eth0.2
    ip addr add 192.0.2.9/30 dev eth0.3

    # bring up the bridge devices
    ip link set br0 up


*bridge*
(网桥)

  .. code-block:: sh

    # tag traffic on CPU port
    ip link add link eth0 name eth0.1 type vlan id 1

    # For kernels earlier than v5.12, the conduit interface needs to be
    # brought up manually before the user ports.
    ip link set eth0 up
    ip link set eth0.1 up

    # bring up the user interfaces
    ip link set lan1 up
    ip link set lan2 up
    ip link set lan3 up

    # create bridge
    ip link add name br0 type bridge

    # activate VLAN filtering
    ip link set dev br0 type bridge vlan_filtering 1

    # add ports to bridge
    ip link set dev lan1 master br0
    ip link set dev lan2 master br0
    ip link set dev lan3 master br0
    ip link set eth0.1 master br0

    # tag traffic on ports
    bridge vlan add dev lan1 vid 1 pvid untagged
    bridge vlan add dev lan2 vid 1 pvid untagged
    bridge vlan add dev lan3 vid 1 pvid untagged

    # configure the bridge
    ip addr add 192.0.2.129/25 dev br0

    # bring up the bridge
    ip link set dev br0 up

*gateway*
(网关)

  .. code-block:: sh

    # tag traffic on CPU port
    ip link add link eth0 name eth0.1 type vlan id 1
    ip link add link eth0 name eth0.2 type vlan id 2

    # For kernels earlier than v5.12, the conduit interface needs to be
    # brought up manually before the user ports.
    ip link set eth0 up
    ip link set eth0.1 up
    ip link set eth0.2 up

    # bring up the user interfaces
    ip link set wan up
    ip link set lan1 up
    ip link set lan2 up

    # create bridge
    ip link add name br0 type bridge

    # activate VLAN filtering
    ip link set dev br0 type bridge vlan_filtering 1

    # add ports to bridges
    ip link set dev wan master br0
    ip link set eth0.1 master br0
    ip link set dev lan1 master br0
    ip link set dev lan2 master br0

    # tag traffic on ports
    bridge vlan add dev lan1 vid 1 pvid untagged
    bridge vlan add dev lan2 vid 1 pvid untagged
    bridge vlan add dev wan vid 2 pvid untagged

    # configure the VLANs
    ip addr add 192.0.2.1/30 dev eth0.2
    ip addr add 192.0.2.129/25 dev br0

    # bring up the bridge devices
    ip link set br0 up

Forwarding database (FDB) management(转发数据库 (FDB) 管理)
-----------------------------------------------------------

The existing DSA switches do not have the necessary hardware support to keep
the software FDB of the bridge in sync with the hardware tables, so the two
tables are managed separately (``bridge fdb show`` queries both, and depending
on whether the ``self`` or ``master`` flags are being used, a ``bridge fdb
add`` or ``bridge fdb del`` command acts upon entries from one or both tables).
(现有的 DSA 交换机不具备让网桥的软件 FDB 与硬件表保持同步所需的硬件支持，因此这
两张表是分开管理的（``bridge fdb show`` 会同时查询两者；而根据使用的是 ``self``
还是 ``master`` 标志，``bridge fdb add`` 或 ``bridge fdb del`` 命令会作用于其中
一张或两张表中的表项）。)

Up until kernel v4.14, DSA only supported user space management of bridge FDB
entries using the bridge bypass operations (which do not update the software
FDB, just the hardware one) using the ``self`` flag (which is optional and can
be omitted).
(直到内核 v4.14 为止，DSA 仅支持用户空间借助网桥旁路（bridge bypass）操作并使用
``self`` 标志（该标志是可选的，可以省略）来管理网桥 FDB 表项；这些旁路操作不更新
软件 FDB，只更新硬件表。)

  .. code-block:: sh

    bridge fdb add dev swp0 00:01:02:03:04:05 self static
    # or shorthand
    bridge fdb add dev swp0 00:01:02:03:04:05 static

Due to a bug, the bridge bypass FDB implementation provided by DSA did not
distinguish between ``static`` and ``local`` FDB entries (``static`` are meant
to be forwarded, while ``local`` are meant to be locally terminated, i.e. sent
to the host port). Instead, all FDB entries with the ``self`` flag (implicit or
explicit) are treated by DSA as ``static`` even if they are ``local``.
(由于一个 bug，DSA 提供的网桥旁路 FDB 实现无法区分 ``static`` 与 ``local`` 这两种
FDB 表项（``static`` 表示应当被转发，而 ``local`` 表示应当本地终结，即发往主机
端口）。恰恰相反，所有带 ``self`` 标志（无论显式还是隐式）的 FDB 表项都会被 DSA
当作 ``static`` 处理，即使它们其实是 ``local``。)

  .. code-block:: sh

    # This command:
    bridge fdb add dev swp0 00:01:02:03:04:05 static
    # behaves the same for DSA as this command:
    bridge fdb add dev swp0 00:01:02:03:04:05 local
    # or shorthand, because the 'local' flag is implicit if 'static' is not
    # specified, it also behaves the same as:
    bridge fdb add dev swp0 00:01:02:03:04:05

The last command is an incorrect way of adding a static bridge FDB entry to a
DSA switch using the bridge bypass operations, and works by mistake. Other
drivers will treat an FDB entry added by the same command as ``local`` and as
such, will not forward it, as opposed to DSA.
(最后那条命令是通过网桥旁路操作向 DSA 交换机添加静态网桥 FDB 表项的一种错误
方式，它只是碰巧能用。与 DSA 不同，其他驱动会把用同一命令添加的 FDB 表项当作
``local``，因而不转发它。)

Between kernel v4.14 and v5.14, DSA has supported in parallel two modes of
adding a bridge FDB entry to the switch: the bridge bypass discussed above, as
well as a new mode using the ``master`` flag which installs FDB entries in the
software bridge too.
(在内核 v4.14 到 v5.14 之间，DSA 并行支持两种向交换机添加网桥 FDB 表项的方式：
上文讨论的网桥旁路方式，以及使用 ``master`` 标志的新方式——后者同时会把 FDB
表项安装到软件网桥中。)

  .. code-block:: sh

    bridge fdb add dev swp0 00:01:02:03:04:05 master static

Since kernel v5.14, DSA has gained stronger integration with the bridge's
software FDB, and the support for its bridge bypass FDB implementation (using
the ``self`` flag) has been removed. This results in the following changes:
(自内核 v5.14 起，DSA 与网桥软件 FDB 的集成更加紧密，同时移除了对其网桥旁路 FDB
实现（使用 ``self`` 标志）的支持。由此带来以下变化：)

  .. code-block:: sh

    # This is the only valid way of adding an FDB entry that is supported,
    # compatible with v4.14 kernels and later:
    bridge fdb add dev swp0 00:01:02:03:04:05 master static
    # This command is no longer buggy and the entry is properly treated as
    # 'local' instead of being forwarded:
    bridge fdb add dev swp0 00:01:02:03:04:05
    # This command no longer installs a static FDB entry to hardware:
    bridge fdb add dev swp0 00:01:02:03:04:05 static

Script writers are therefore encouraged to use the ``master static`` set of
flags when working with bridge FDB entries on DSA switch interfaces.
(因此，建议脚本编写者在处理 DSA 交换机接口上的网桥 FDB 表项时，使用
``master static`` 这组标志。)

Affinity of user ports to CPU ports(用户端口与 CPU 端口的亲和性)
----------------------------------------------------------------

Typically, DSA switches are attached to the host via a single Ethernet
interface, but in cases where the switch chip is discrete, the hardware design
may permit the use of 2 or more ports connected to the host, for an increase in
termination throughput.
(通常，DSA 交换机通过单个以太网接口连接到主机；但在交换芯片为独立芯片的情况下，
硬件设计可能允许使用 2 个或更多连接到主机的端口，以提升终结流量的吞吐量。)

DSA can make use of multiple CPU ports in two ways. First, it is possible to
statically assign the termination traffic associated with a certain user port
to be processed by a certain CPU port. This way, user space can implement
custom policies of static load balancing between user ports, by spreading the
affinities according to the available CPU ports.
(DSA 可以通过两种方式利用多个 CPU 端口。第一种方式是：可以把与某个用户端口相关
联的终结流量静态地指定由某个 CPU 端口来处理。这样，用户空间便可根据可用的 CPU
端口来分配亲和性，从而在用户端口之间实现自定义的静态负载均衡策略。)

Secondly, it is possible to perform load balancing between CPU ports on a per
packet basis, rather than statically assigning user ports to CPU ports.
This can be achieved by placing the DSA conduits under a LAG interface (bonding
or team). DSA monitors this operation and creates a mirror of this software LAG
on the CPU ports facing the physical DSA conduits that constitute the LAG slave
devices.
(第二种方式是：可以在逐数据包的粒度上于 CPU 端口之间进行负载均衡，而不是把用户
端口静态地指派给 CPU 端口。做法是把各个 DSA conduit 置于一个 LAG 接口（bonding
或 team）之下。DSA 会监视这一操作，并在面向构成该 LAG slave 设备的各物理 DSA
conduit 的 CPU 端口上，创建这个软件 LAG 的镜像。)

To make use of multiple CPU ports, the firmware (device tree) description of
the switch must mark all the links between CPU ports and their DSA conduits
using the ``ethernet`` reference/phandle. At startup, only a single CPU port
and DSA conduit will be used - the numerically first port from the firmware
description which has an ``ethernet`` property. It is up to the user to
configure the system for the switch to use other conduits.
(要使用多个 CPU 端口，交换机的固件（设备树）描述必须使用 ``ethernet``
reference/phandle 标记 CPU 端口与其 DSA conduit 之间的所有链路。启动时只会使用
一个 CPU 端口和一个 DSA conduit——即固件描述中编号最小、带有 ``ethernet`` 属性
的那个端口。若要让交换机使用其他 conduit，需要由用户自行配置系统。)

DSA uses the ``rtnl_link_ops`` mechanism (with a "dsa" ``kind``) to allow
changing the DSA conduit of a user port. The ``IFLA_DSA_CONDUIT`` u32 netlink
attribute contains the ifindex of the conduit device that handles each user
device. The DSA conduit must be a valid candidate based on firmware node
information, or a LAG interface which contains only slaves which are valid
candidates.
(DSA 使用 ``rtnl_link_ops`` 机制（``kind`` 为 "dsa"）来允许更改用户端口的 DSA
conduit。``IFLA_DSA_CONDUIT`` u32 netlink 属性包含负责处理每个用户设备的
conduit 设备的 ifindex。DSA conduit 必须是根据固件节点信息判定为有效候选的
设备，或者是一个仅包含有效候选 slave 的 LAG 接口。)

Using iproute2, the following manipulations are possible:
(借助 iproute2，可以进行如下操作：)

  .. code-block:: sh

    # See the DSA conduit in current use
    ip -d link show dev swp0
        (...)
        dsa master eth0

    # Static CPU port distribution
    ip link set swp0 type dsa master eth1
    ip link set swp1 type dsa master eth0
    ip link set swp2 type dsa master eth1
    ip link set swp3 type dsa master eth0

    # CPU ports in LAG, using explicit assignment of the DSA conduit
    ip link add bond0 type bond mode balance-xor && ip link set bond0 up
    ip link set eth1 down && ip link set eth1 master bond0
    ip link set swp0 type dsa master bond0
    ip link set swp1 type dsa master bond0
    ip link set swp2 type dsa master bond0
    ip link set swp3 type dsa master bond0
    ip link set eth0 down && ip link set eth0 master bond0
    ip -d link show dev swp0
        (...)
        dsa master bond0

    # CPU ports in LAG, relying on implicit migration of the DSA conduit
    ip link add bond0 type bond mode balance-xor && ip link set bond0 up
    ip link set eth0 down && ip link set eth0 master bond0
    ip link set eth1 down && ip link set eth1 master bond0
    ip -d link show dev swp0
        (...)
        dsa master bond0

Notice that in the case of CPU ports under a LAG, the use of the
``IFLA_DSA_CONDUIT`` netlink attribute is not strictly needed, but rather, DSA
reacts to the ``IFLA_MASTER`` attribute change of its present conduit (``eth0``)
and migrates all user ports to the new upper of ``eth0``, ``bond0``. Similarly,
when ``bond0`` is destroyed using ``RTM_DELLINK``, DSA migrates the user ports
that were assigned to this interface to the first physical DSA conduit which is
eligible, based on the firmware description (it effectively reverts to the
startup configuration).
(注意，对于位于 LAG 之下的 CPU 端口，并不严格需要使用 ``IFLA_DSA_CONDUIT``
netlink 属性；DSA 会响应其当前 conduit（``eth0``）的 ``IFLA_MASTER`` 属性变化，
把所有用户端口迁移到 ``eth0`` 的新上层设备 ``bond0`` 上。类似地，当 ``bond0``
通过 ``RTM_DELLINK`` 被删除时，DSA 会把原先指派给该接口的用户端口迁移到固件描述
所确定的第一个符合条件的物理 DSA conduit 上（这实际上就是恢复到启动时的配置）。)

In a setup with more than 2 physical CPU ports, it is therefore possible to mix
static user to CPU port assignment with LAG between DSA conduits. It is not
possible to statically assign a user port towards a DSA conduit that has any
upper interfaces (this includes LAG devices - the conduit must always be the LAG
in this case).
(因此，在拥有多于 2 个物理 CPU 端口的系统中，可以把静态的用户端口到 CPU 端口
指派与 DSA conduit 之间的 LAG 混合使用。不能把用户端口静态指派给任何拥有上层
接口的 DSA conduit（这包括 LAG 设备——在这种情况下 conduit 必须始终是该 LAG
本身）。)

Live changing of the DSA conduit (and thus CPU port) affinity of a user port is
permitted, in order to allow dynamic redistribution in response to traffic.
(允许动态更改用户端口的 DSA conduit（因而也就是 CPU 端口）亲和性，以便根据流量
情况进行动态重分配。)

Physical DSA conduits are allowed to join and leave at any time a LAG interface
used as a DSA conduit; however, DSA will reject a LAG interface as a valid
candidate for being a DSA conduit unless it has at least one physical DSA conduit
as a slave device.
(允许物理 DSA conduit 随时加入或退出用作 DSA conduit 的 LAG 接口；但是，除非该
LAG 接口至少拥有一个物理 DSA conduit 作为 slave 设备，否则 DSA 会拒绝将其视为
有效的 DSA conduit 候选。)
