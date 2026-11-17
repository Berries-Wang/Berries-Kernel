.. SPDX-License-Identifier: GPL-2.0
.. include:: <isonum.txt>
.. _switchdev:

===============================================
Ethernet switch device driver model (switchdev)
===============================================

(以太网交换机设备驱动模型 (switchdev))

Copyright |copy| 2014 Jiri Pirko <jiri@resnulli.us>
(版权所有 |copy| 2014 Jiri Pirko <jiri@resnulli.us>)

Copyright |copy| 2014-2015 Scott Feldman <sfeldma@gmail.com>
(版权所有 |copy| 2014-2015 Scott Feldman <sfeldma@gmail.com>)


The Ethernet switch device driver model (switchdev) is an in-kernel driver
model for switch devices which offload the forwarding (data) plane from the
kernel.
(以太网交换机设备驱动模型 (switchdev) 是一种内核内驱动模型，面向把转发
(数据) 面从内核卸载出去的交换设备。)

Figure 1 is a block diagram showing the components of the switchdev model for
an example setup using a data-center-class switch ASIC chip.  Other setups
with SR-IOV or soft switches, such as OVS, are possible.
(图 1 是一个框图，展示了在使用数据中心级交换 ASIC 芯片的示例环境中
switchdev 模型的组成部分。使用 SR-IOV 或软交换机 (如 OVS) 的其他环境也是
可能的。)

::


			     User-space tools

       user space                   |
      +-------------------------------------------------------------------+
       kernel                       | Netlink
				    |
		     +--------------+-------------------------------+
		     |         Network stack                        |
		     |           (Linux)                            |
		     |                                              |
		     +----------------------------------------------+

			   sw1p2     sw1p4     sw1p6
		      sw1p1  +  sw1p3  +  sw1p5  +          eth1
			+    |    +    |    +    |            +
			|    |    |    |    |    |            |
		     +--+----+----+----+----+----+---+  +-----+-----+
		     |         Switch driver         |  |    mgmt   |
		     |        (this document)        |  |   driver  |
		     |                               |  |           |
		     +--------------+----------------+  +-----------+
				    |
       kernel                       | HW bus (eg PCI)
      +-------------------------------------------------------------------+
       hardware                     |
		     +--------------+----------------+
		     |         Switch device (sw1)   |
		     |  +----+                       +--------+
		     |  |    v offloaded data path   | mgmt port
		     |  |    |                       |
		     +--|----|----+----+----+----+---+
			|    |    |    |    |    |
			+    +    +    +    +    +
		       p1   p2   p3   p4   p5   p6

			     front-panel ports


				    Fig 1.


Include Files
(包含文件)
-------------

::

    #include <linux/netdevice.h>
    #include <net/switchdev.h>


Configuration
(配置)
-------------

Use "depends NET_SWITCHDEV" in driver's Kconfig to ensure switchdev model
support is built for driver.
(在驱动的 Kconfig 中使用 "depends NET_SWITCHDEV"，以确保为该驱动构建
switchdev 模型支持。)


Switch Ports
(交换机端口)
------------

On switchdev driver initialization, the driver will allocate and register a
struct net_device (using register_netdev()) for each enumerated physical switch
port, called the port netdev.  A port netdev is the software representation of
the physical port and provides a conduit for control traffic to/from the
controller (the kernel) and the network, as well as an anchor point for higher
level constructs such as bridges, bonds, VLANs, tunnels, and L3 routers.  Using
standard netdev tools (iproute2, ethtool, etc), the port netdev can also
provide to the user access to the physical properties of the switch port such
as PHY link state and I/O statistics.
(在 switchdev 驱动初始化时，驱动会为每个枚举到的物理交换机端口分配并注册
一个 struct net_device (使用 register_netdev())，称为端口 netdev。端口
netdev 是物理端口的软件表示，提供控制器 (内核) 与网络之间控制流量的通道，
也是更高层结构 (如网桥、bond、VLAN、隧道和 L3 路由器) 的锚点。通过标准
netdev 工具 (iproute2、ethtool 等)，端口 netdev 还可以向用户提供对交换机
端口物理属性 (如 PHY 链路状态和 I/O 统计信息) 的访问。)

There is (currently) no higher-level kernel object for the switch beyond the
port netdevs.  All of the switchdev driver ops are netdev ops or switchdev ops.
(除端口 netdev 之外，(目前) 没有更高层的内核对象来表示交换机。switchdev
驱动的所有操作都是 netdev 操作或 switchdev 操作。)

A switch management port is outside the scope of the switchdev driver model.
Typically, the management port is not participating in offloaded data plane and
is loaded with a different driver, such as a NIC driver, on the management port
device.
(交换机管理端口不在 switchdev 驱动模型的范围之内。通常，管理端口不参与
已卸载的数据平面，并且管理端口设备上加载的是不同的驱动，例如网卡驱动。)

Switch ID
(交换机 ID)
^^^^^^^^^

The switchdev driver must implement the net_device operation
ndo_get_port_parent_id for each port netdev, returning the same physical ID for
each port of a switch. The ID must be unique between switches on the same
system. The ID does not need to be unique between switches on different
systems.
(switchdev 驱动必须为每个端口 netdev 实现 net_device 操作
ndo_get_port_parent_id，为交换机的每个端口返回相同的物理 ID。该 ID 在
同一系统内的各交换机之间必须唯一；在不同系统的交换机之间则无需唯一。)

The switch ID is used to locate ports on a switch and to know if aggregated
ports belong to the same switch.
(交换机 ID 用于定位交换机上的端口，并判断聚合端口是否属于同一台交换机。)

Port Netdev Naming
(端口 netdev 命名)
^^^^^^^^^^^^^^^^^^

Udev rules should be used for port netdev naming, using some unique attribute
of the port as a key, for example the port MAC address or the port PHYS name.
Hard-coding of kernel netdev names within the driver is discouraged; let the
kernel pick the default netdev name, and let udev set the final name based on a
port attribute.
(端口 netdev 的命名应使用 udev 规则，以端口的某个唯一属性作为键，例如
端口 MAC 地址或端口 PHYS 名称。不建议在驱动中硬编码内核 netdev 名称；
应让内核选定默认的 netdev 名称，并让 udev 根据端口属性设置最终名称。)

Using port PHYS name (ndo_get_phys_port_name) for the key is particularly
useful for dynamically-named ports where the device names its ports based on
external configuration.  For example, if a physical 40G port is split logically
into 4 10G ports, resulting in 4 port netdevs, the device can give a unique
name for each port using port PHYS name.  The udev rule would be::
(使用端口 PHYS 名称 (ndo_get_phys_port_name) 作为键对动态命名的端口
尤其有用，这类设备根据外部配置为端口命名。例如，如果一个物理 40G 端口
被逻辑拆分为 4 个 10G 端口、从而产生 4 个端口 netdev，设备可以使用端口
PHYS 名称给每个端口一个唯一的名称。udev 规则如下：)

    SUBSYSTEM=="net", ACTION=="add", ATTR{phys_switch_id}=="<phys_switch_id>", \
	    ATTR{phys_port_name}!="", NAME="swX$attr{phys_port_name}"

Suggested naming convention is "swXpYsZ", where X is the switch name or ID, Y
is the port name or ID, and Z is the sub-port name or ID.  For example, sw1p1s0
would be sub-port 0 on port 1 on switch 1.
(建议的命名约定是 "swXpYsZ"，其中 X 是交换机名称或 ID，Y 是端口名称或
ID，Z 是子端口名称或 ID。例如，sw1p1s0 表示交换机 1 的端口 1 上的子端口
0。)

Port Features
(端口特性)
^^^^^^^^^^^^^

dev->netns_immutable

If the switchdev driver (and device) only supports offloading of the default
network namespace (netns), the driver should set this private flag to prevent
the port netdev from being moved out of the default netns.  A netns-aware
driver/device would not set this flag and be responsible for partitioning
hardware to preserve netns containment.  This means hardware cannot forward
traffic from a port in one namespace to another port in another namespace.
(如果 switchdev 驱动 (和设备) 只支持对默认网络命名空间 (netns) 的卸载，
驱动应设置这个私有标志，以防止端口 netdev 被移出默认 netns。具备 netns
感知能力的驱动/设备不会设置此标志，而是负责对硬件进行分区以保持 netns
的隔离。这意味着硬件不能把流量从一个命名空间中的端口转发到另一个命名
空间中的端口。)

Port Topology
(端口拓扑)
^^^^^^^^^^^^^

The port netdevs representing the physical switch ports can be organized into
higher-level switching constructs.  The default construct is a standalone
router port, used to offload L3 forwarding.  Two or more ports can be bonded
together to form a LAG.  Two or more ports (or LAGs) can be bridged to bridge
L2 networks.  VLANs can be applied to sub-divide L2 networks.  L2-over-L3
tunnels can be built on ports.  These constructs are built using standard Linux
tools such as the bridge driver, the bonding/team drivers, and netlink-based
tools such as iproute2.
(代表物理交换机端口的端口 netdev 可以被组织成更高层的交换结构。默认
结构是独立路由端口，用于卸载 L3 转发。两个或更多端口可以绑定在一起形成
LAG。两个或更多端口 (或 LAG) 可以桥接以桥接 L2 网络。VLAN 可用于进一步
划分 L2 网络。端口上可以构建 L2-over-L3 隧道。这些结构使用标准 Linux
工具构建，例如 bridge 驱动、bonding/team 驱动，以及 iproute2 等基于
netlink 的工具。)

The switchdev driver can know a particular port's position in the topology by
monitoring NETDEV_CHANGEUPPER notifications.  For example, a port moved into a
bond will see its upper master change.  If that bond is moved into a bridge,
the bond's upper master will change.  And so on.  The driver will track such
movements to know what position a port is in in the overall topology by
registering for netdevice events and acting on NETDEV_CHANGEUPPER.
(switchdev 驱动可以通过监视 NETDEV_CHANGEUPPER 通知来了解特定端口在
拓扑中的位置。例如，被移入 bond 的端口会看到其上层 master 发生变化。
如果该 bond 被移入网桥，bond 的上层 master 会变化，依此类推。驱动通过
注册 netdevice 事件并对 NETDEV_CHANGEUPPER 作出响应，来跟踪这类变动，
从而了解端口在整个拓扑中的位置。)

L2 Forwarding Offload
(L2 转发卸载)
---------------------

The idea is to offload the L2 data forwarding (switching) path from the kernel
to the switchdev device by mirroring bridge FDB entries down to the device.  An
FDB entry is the {port, MAC, VLAN} tuple forwarding destination.
(其思路是通过把网桥 FDB 表项镜像下发到设备，把 L2 数据转发 (交换) 路径
从内核卸载到 switchdev 设备。FDB 表项是 {port, MAC, VLAN} 元组形式的
转发目的地。)

To offloading L2 bridging, the switchdev driver/device should support:
(要卸载 L2 桥接，switchdev 驱动/设备应支持：)

	- Static FDB entries installed on a bridge port
	  (安装在网桥端口上的静态 FDB 表项)
	- Notification of learned/forgotten src mac/vlans from device
	  (设备对学习/遗忘的源 mac/vlan 的通知)
	- STP state changes on the port
	  (端口上 STP 状态的变化)
	- VLAN flooding of multicast/broadcast and unknown unicast packets
	  (对组播/广播和未知单播数据包的 VLAN 泛洪)

Static FDB Entries
(静态 FDB 表项)
^^^^^^^^^^^^^^^^^^

A driver which implements the ``ndo_fdb_add``, ``ndo_fdb_del`` and
``ndo_fdb_dump`` operations is able to support the command below, which adds a
static bridge FDB entry::
(实现了 ``ndo_fdb_add``、``ndo_fdb_del`` 和 ``ndo_fdb_dump`` 操作的驱动
可以支持下方的命令，该命令添加一条静态网桥 FDB 表项：)

        bridge fdb add dev DEV ADDRESS [vlan VID] [self] static

(the "static" keyword is non-optional: if not specified, the entry defaults to
being "local", which means that it should not be forwarded)
(("static" 关键字并非可选：如果不指定，该表项默认为 "local"，即不应被
转发。))

The "self" keyword (optional because it is implicit) has the role of
instructing the kernel to fulfill the operation through the ``ndo_fdb_add``
implementation of the ``DEV`` device itself. If ``DEV`` is a bridge port, this
will bypass the bridge and therefore leave the software database out of sync
with the hardware one.
("self" 关键字 (可选，因为它是隐式的) 的作用是指示内核通过 ``DEV`` 设备
自身的 ``ndo_fdb_add`` 实现来完成该操作。如果 ``DEV`` 是一个网桥端口，
这将绕过网桥，从而使软件数据库与硬件数据库失去同步。)

To avoid this, the "master" keyword can be used::
(为避免这种情况，可以使用 "master" 关键字：)

        bridge fdb add dev DEV ADDRESS [vlan VID] master static

The above command instructs the kernel to search for a master interface of
``DEV`` and fulfill the operation through the ``ndo_fdb_add`` method of that.
This time, the bridge generates a ``SWITCHDEV_FDB_ADD_TO_DEVICE`` notification
which the port driver can handle and use it to program its hardware table. This
way, the software and the hardware database will both contain this static FDB
entry.
(上述命令指示内核搜索 ``DEV`` 的 master 接口，并通过它的 ``ndo_fdb_add``
方法完成该操作。这一次，网桥会生成 ``SWITCHDEV_FDB_ADD_TO_DEVICE`` 通知，
端口驱动可以处理该通知并用它来编程自己的硬件表。这样，软件数据库和硬件
数据库都会包含这条静态 FDB 表项。)

Note: for new switchdev drivers that offload the Linux bridge, implementing the
``ndo_fdb_add`` and ``ndo_fdb_del`` bridge bypass methods is strongly
discouraged: all static FDB entries should be added on a bridge port using the
"master" flag. The ``ndo_fdb_dump`` is an exception and can be implemented to
visualize the hardware tables, if the device does not have an interrupt for
notifying the operating system of newly learned/forgotten dynamic FDB
addresses. In that case, the hardware FDB might end up having entries that the
software FDB does not, and implementing ``ndo_fdb_dump`` is the only way to see
them.
(注意：对于卸载 Linux 网桥的新型 switchdev 驱动，强烈不建议实现
``ndo_fdb_add`` 和 ``ndo_fdb_del`` 这两个网桥旁路方法：所有静态 FDB 表项
都应使用 "master" 标志添加到网桥端口上。``ndo_fdb_dump`` 是个例外：如果
设备没有用于向操作系统通知新学习/遗忘的动态 FDB 地址的中断，可以实现它
来查看硬件表。在这种情况下，硬件 FDB 最终可能包含软件 FDB 所没有的
表项，而实现 ``ndo_fdb_dump`` 是看到它们的唯一途径。)

Note: by default, the bridge does not filter on VLAN and only bridges untagged
traffic.  To enable VLAN support, turn on VLAN filtering::
(注意：默认情况下，网桥不按 VLAN 过滤，只桥接未打标签的流量。要启用
VLAN 支持，请打开 VLAN 过滤：)

	echo 1 >/sys/class/net/<bridge>/bridge/vlan_filtering

Notification of Learned/Forgotten Source MAC/VLANs
(学习/遗忘源 MAC/VLAN 的通知)
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

The switch device will learn/forget source MAC address/VLAN on ingress packets
and notify the switch driver of the mac/vlan/port tuples.  The switch driver,
in turn, will notify the bridge driver using the switchdev notifier call::
(交换设备会在入方向数据包上学习/遗忘源 MAC 地址/VLAN，并把 mac/vlan/
port 元组通知交换驱动。交换驱动随后会使用 switchdev 通知器调用通知网桥
驱动：)

	err = call_switchdev_notifiers(val, dev, info, extack);

Where val is SWITCHDEV_FDB_ADD when learning and SWITCHDEV_FDB_DEL when
forgetting, and info points to a struct switchdev_notifier_fdb_info.  On
SWITCHDEV_FDB_ADD, the bridge driver will install the FDB entry into the
bridge's FDB and mark the entry as NTF_EXT_LEARNED.  The iproute2 bridge
command will label these entries "offload"::
(其中 val 在学习时为 SWITCHDEV_FDB_ADD，在遗忘时为 SWITCHDEV_FDB_DEL，
info 指向 struct switchdev_notifier_fdb_info。收到 SWITCHDEV_FDB_ADD 时，
网桥驱动会把该 FDB 表项安装到网桥的 FDB 中，并把该表项标记为
NTF_EXT_LEARNED。iproute2 的 bridge 命令会把这些表项标注为 "offload"：)

	$ bridge fdb
	52:54:00:12:35:01 dev sw1p1 master br0 permanent
	00:02:00:00:02:00 dev sw1p1 master br0 offload
	00:02:00:00:02:00 dev sw1p1 self
	52:54:00:12:35:02 dev sw1p2 master br0 permanent
	00:02:00:00:03:00 dev sw1p2 master br0 offload
	00:02:00:00:03:00 dev sw1p2 self
	33:33:00:00:00:01 dev eth0 self permanent
	01:00:5e:00:00:01 dev eth0 self permanent
	33:33:ff:00:00:00 dev eth0 self permanent
	01:80:c2:00:00:0e dev eth0 self permanent
	33:33:00:00:00:01 dev br0 self permanent
	01:00:5e:00:00:01 dev br0 self permanent
	33:33:ff:12:35:01 dev br0 self permanent

Learning on the port should be disabled on the bridge using the bridge command::
(应使用 bridge 命令在网桥上禁用该端口的学习：)

	bridge link set dev DEV learning off

Learning on the device port should be enabled, as well as learning_sync::
(设备端口上的学习应当启用，learning_sync 也是如此：)

	bridge link set dev DEV learning on self
	bridge link set dev DEV learning_sync on self

Learning_sync attribute enables syncing of the learned/forgotten FDB entry to
the bridge's FDB.  It's possible, but not optimal, to enable learning on the
device port and on the bridge port, and disable learning_sync.
(learning_sync 属性用于启用把学习/遗忘的 FDB 表项同步到网桥 FDB 的
功能。也可以在设备端口和网桥端口上都启用学习并禁用 learning_sync，但这
并非最优。)

To support learning, the driver implements switchdev op
switchdev_port_attr_set for SWITCHDEV_ATTR_PORT_ID_{PRE}_BRIDGE_FLAGS.
(为支持学习，驱动要实现 switchdev 操作 switchdev_port_attr_set，对应
SWITCHDEV_ATTR_PORT_ID_{PRE}_BRIDGE_FLAGS。)

FDB Ageing
(FDB 老化)
^^^^^^^^^^

The bridge will skip ageing FDB entries marked with NTF_EXT_LEARNED and it is
the responsibility of the port driver/device to age out these entries.  If the
port device supports ageing, when the FDB entry expires, it will notify the
driver which in turn will notify the bridge with SWITCHDEV_FDB_DEL.  If the
device does not support ageing, the driver can simulate ageing using a
garbage collection timer to monitor FDB entries.  Expired entries will be
notified to the bridge using SWITCHDEV_FDB_DEL.  See rocker driver for
example of driver running ageing timer.
(网桥会跳过对标记为 NTF_EXT_LEARNED 的 FDB 表项的老化，把这些表项老化
掉是端口驱动/设备的责任。如果端口设备支持老化，当 FDB 表项过期时，它会
通知驱动，驱动随后用 SWITCHDEV_FDB_DEL 通知网桥。如果设备不支持老化，
驱动可以使用垃圾回收定时器监视 FDB 表项来模拟老化。过期表项将通过
SWITCHDEV_FDB_DEL 通知给网桥。驱动运行老化定时器的示例参见 rocker
驱动。)

To keep an NTF_EXT_LEARNED entry "alive", the driver should refresh the FDB
entry by calling call_switchdev_notifiers(SWITCHDEV_FDB_ADD, ...).  The
notification will reset the FDB entry's last-used time to now.  The driver
should rate limit refresh notifications, for example, no more than once a
second.  (The last-used time is visible using the bridge -s fdb option).
(要让 NTF_EXT_LEARNED 表项保持 "存活"，驱动应通过调用
call_switchdev_notifiers(SWITCHDEV_FDB_ADD, ...) 来刷新该 FDB 表项。该
通知会把 FDB 表项的最近使用时间重置为当前时间。驱动应限制刷新通知的
速率，例如每秒不超过一次。(最近使用时间可通过 bridge -s fdb 选项
查看)。)

STP State Change on Port
(端口上的 STP 状态变化)
^^^^^^^^^^^^^^^^^^^^^^^^

Internally or with a third-party STP protocol implementation (e.g. mstpd), the
bridge driver maintains the STP state for ports, and will notify the switch
driver of STP state change on a port using the switchdev op
switchdev_attr_port_set for SWITCHDEV_ATTR_PORT_ID_STP_UPDATE.
(在内部或借助第三方 STP 协议实现 (例如 mstpd) 时，网桥驱动会维护端口的
STP 状态，并使用 switchdev 操作 switchdev_attr_port_set (对应
SWITCHDEV_ATTR_PORT_ID_STP_UPDATE) 把端口上 STP 状态的变化通知交换
驱动。)

State is one of BR_STATE_*.  The switch driver can use STP state updates to
update ingress packet filter list for the port.  For example, if port is
DISABLED, no packets should pass, but if port moves to BLOCKED, then STP BPDUs
and other IEEE 01:80:c2:xx:xx:xx link-local multicast packets can pass.
(状态是 BR_STATE_* 之一。交换驱动可以利用 STP 状态更新来更新端口的入
方向数据包过滤列表。例如，端口处于 DISABLED 时任何数据包都不应通过，
而当端口转入 BLOCKED 时，STP BPDU 和其他 IEEE 01:80:c2:xx:xx:xx 链路
本地组播数据包可以通过。)

Note that STP BDPUs are untagged and STP state applies to all VLANs on the port
so packet filters should be applied consistently across untagged and tagged
VLANs on the port.
(注意，STP BPDU 是未打标签的，且 STP 状态适用于端口上的所有 VLAN，因此
数据包过滤器应在端口上未打标签和已打标签的 VLAN 之间一致地应用。)

Flooding L2 domain
(L2 域泛洪)
^^^^^^^^^^^^^^^^^^

For a given L2 VLAN domain, the switch device should flood multicast/broadcast
and unknown unicast packets to all ports in domain, if allowed by port's
current STP state.  The switch driver, knowing which ports are within which
vlan L2 domain, can program the switch device for flooding.  The packet may
be sent to the port netdev for processing by the bridge driver.  The
bridge should not reflood the packet to the same ports the device flooded,
otherwise there will be duplicate packets on the wire.
(对于给定的 L2 VLAN 域，如果端口当前的 STP 状态允许，交换设备应把组播/
广播和未知单播数据包泛洪到该域中的所有端口。交换驱动知道哪些端口属于
哪个 vlan L2 域，可以对交换设备编程以执行泛洪。数据包可以被送到端口
netdev，由网桥驱动处理。网桥不应把数据包重新泛洪到设备已泛洪过的相同
端口，否则线上会出现重复数据包。)

To avoid duplicate packets, the switch driver should mark a packet as already
forwarded by setting the skb->offload_fwd_mark bit. The bridge driver will mark
the skb using the ingress bridge port's mark and prevent it from being forwarded
through any bridge port with the same mark.
(为避免重复数据包，交换驱动应通过设置 skb->offload_fwd_mark 位把数据包
标记为已转发。网桥驱动会用入方向网桥端口的标记来标记 skb，并阻止它通过
任何具有相同标记的网桥端口转发。)

It is possible for the switch device to not handle flooding and push the
packets up to the bridge driver for flooding.  This is not ideal as the number
of ports scale in the L2 domain as the device is much more efficient at
flooding packets that software.
(交换设备也可以不处理泛洪，而是把数据包上交网桥驱动来泛洪。这并不理想，
因为随着 L2 域中端口数量的扩展，设备在泛洪数据包方面比软件高效得多。)

If supported by the device, flood control can be offloaded to it, preventing
certain netdevs from flooding unicast traffic for which there is no FDB entry.
(如果设备支持，泛洪控制可以卸载到设备上，防止某些 netdev 泛洪没有对应
FDB 表项的单播流量。)

IGMP Snooping
(IGMP 侦听)
^^^^^^^^^^^^^

In order to support IGMP snooping, the port netdevs should trap to the bridge
driver all IGMP join and leave messages.
The bridge multicast module will notify port netdevs on every multicast group
changed whether it is static configured or dynamically joined/leave.
The hardware implementation should be forwarding all registered multicast
traffic groups only to the configured ports.
(为了支持 IGMP 侦听，端口 netdev 应把所有 IGMP 加入和离开消息上送
(trap) 给网桥驱动。网桥组播模块会在每个组播组发生变化时通知端口
netdev，无论该变化是静态配置的还是动态加入/离开。硬件实现应当把所有已
注册的组播流量组只转发到已配置的端口。)

L3 Routing Offload
(L3 路由卸载)
------------------

Offloading L3 routing requires that device be programmed with FIB entries from
the kernel, with the device doing the FIB lookup and forwarding.  The device
does a longest prefix match (LPM) on FIB entries matching route prefix and
forwards the packet to the matching FIB entry's nexthop(s) egress ports.
(卸载 L3 路由要求用内核的 FIB 表项对设备编程，由设备执行 FIB 查找和
转发。设备对 FIB 表项做最长前缀匹配 (LPM) 以匹配路由前缀，并把数据包
转发到匹配的 FIB 表项的下一跳的出端口。)

To program the device, the driver has to register a FIB notifier handler
using register_fib_notifier. The following events are available:
(要对设备编程，驱动必须使用 register_fib_notifier 注册一个 FIB 通知器
处理函数。可用的事件如下：)

===================  ===================================================
FIB_EVENT_ENTRY_ADD  used for both adding a new FIB entry to the device,
		     or modifying an existing entry on the device.
		     (用于向设备添加新的 FIB 表项，或修改设备上已有的
		     表项。)
FIB_EVENT_ENTRY_DEL  used for removing a FIB entry
		     (用于删除 FIB 表项)
FIB_EVENT_RULE_ADD,
FIB_EVENT_RULE_DEL   used to propagate FIB rule changes
		     (用于传播 FIB 规则的变化)
===================  ===================================================

FIB_EVENT_ENTRY_ADD and FIB_EVENT_ENTRY_DEL events pass::
(FIB_EVENT_ENTRY_ADD 和 FIB_EVENT_ENTRY_DEL 事件会传递：)

	struct fib_entry_notifier_info {
		struct fib_notifier_info info; /* must be first */
		u32 dst;
		int dst_len;
		struct fib_info *fi;
		u8 tos;
		u8 type;
		u32 tb_id;
		u32 nlflags;
	};

to add/modify/delete IPv4 dst/dest_len prefix on table tb_id.  The ``*fi``
structure holds details on the route and route's nexthops.  ``*dev`` is one
of the port netdevs mentioned in the route's next hop list.
(用于在表 tb_id 上添加/修改/删除 IPv4 dst/dest_len 前缀。``*fi`` 结构
保存路由及路由下一跳的详细信息。``*dev`` 是路由下一跳列表中提到的端口
netdev 之一。)

Routes offloaded to the device are labeled with "offload" in the ip route
listing::
(已卸载到设备的路由在 ip route 列表中会标注 "offload"：)

	$ ip route show
	default via 192.168.0.2 dev eth0
	11.0.0.0/30 dev sw1p1  proto kernel  scope link  src 11.0.0.2 offload
	11.0.0.4/30 via 11.0.0.1 dev sw1p1  proto zebra  metric 20 offload
	11.0.0.8/30 dev sw1p2  proto kernel  scope link  src 11.0.0.10 offload
	11.0.0.12/30 via 11.0.0.9 dev sw1p2  proto zebra  metric 20 offload
	12.0.0.2  proto zebra  metric 30 offload
		nexthop via 11.0.0.1  dev sw1p1 weight 1
		nexthop via 11.0.0.9  dev sw1p2 weight 1
	12.0.0.3 via 11.0.0.1 dev sw1p1  proto zebra  metric 20 offload
	12.0.0.4 via 11.0.0.9 dev sw1p2  proto zebra  metric 20 offload
	192.168.0.0/24 dev eth0  proto kernel  scope link  src 192.168.0.15

The "offload" flag is set in case at least one device offloads the FIB entry.
(只要至少有一个设备卸载了该 FIB 表项，就会设置 "offload" 标志。)

XXX: add/mod/del IPv6 FIB API
(XXX: 待补充 IPv6 FIB 的增/改/删 API)

Nexthop Resolution
(下一跳解析)
^^^^^^^^^^^^^^^^^^

The FIB entry's nexthop list contains the nexthop tuple (gateway, dev), but for
the switch device to forward the packet with the correct dst mac address, the
nexthop gateways must be resolved to the neighbor's mac address.  Neighbor mac
address discovery comes via the ARP (or ND) process and is available via the
arp_tbl neighbor table.  To resolve the routes nexthop gateways, the driver
should trigger the kernel's neighbor resolution process.  See the rocker
driver's rocker_port_ipv4_resolve() for an example.
(FIB 表项的下一跳列表包含下一跳元组 (gateway, dev)，但要使交换设备能够
用正确的目的 MAC 地址转发数据包，下一跳网关必须解析为邻居的 MAC 地址。
邻居 MAC 地址的发现通过 ARP (或 ND) 过程完成，并可从 arp_tbl 邻居表
获得。为了解析路由的下一跳网关，驱动应触发内核的邻居解析过程。示例
参见 rocker 驱动的 rocker_port_ipv4_resolve()。)

The driver can monitor for updates to arp_tbl using the netevent notifier
NETEVENT_NEIGH_UPDATE.  The device can be programmed with resolved nexthops
for the routes as arp_tbl updates.  The driver implements ndo_neigh_destroy
to know when arp_tbl neighbor entries are purged from the port.
(驱动可以使用 netevent 通知器 NETEVENT_NEIGH_UPDATE 来监视 arp_tbl 的
更新。随着 arp_tbl 的更新，设备可以被编程为使用已解析的下一跳。驱动
实现 ndo_neigh_destroy 以了解 arp_tbl 邻居表项何时从端口被清除。)

Device driver expected behavior
(设备驱动预期行为)
-------------------------------

Below is a set of defined behavior that switchdev enabled network devices must
adhere to.
(下面是一组已定义的行为，启用 switchdev 的网络设备必须遵守。)

Configuration-less state
(免配置状态)
^^^^^^^^^^^^^^^^^^^^^^^^

Upon driver bring up, the network devices must be fully operational, and the
backing driver must configure the network device such that it is possible to
send and receive traffic to this network device and it is properly separated
from other network devices/ports (e.g.: as is frequent with a switch ASIC). How
this is achieved is heavily hardware dependent, but a simple solution can be to
use per-port VLAN identifiers unless a better mechanism is available
(proprietary metadata for each network port for instance).
(在驱动启动时，网络设备必须完全可运作，支撑驱动必须对网络设备进行配置，
使其能够收发流量，并与其他网络设备/端口正确隔离 (例如：交换 ASIC 中常见
的情况)。具体如何实现很大程度上取决于硬件，但一个简单的解决办法是使用
每端口 VLAN 标识符，除非有更好的机制可用 (例如每个网络端口专有的
元数据)。)

The network device must be capable of running a full IP protocol stack
including multicast, DHCP, IPv4/6, etc. If necessary, it should program the
appropriate filters for VLAN, multicast, unicast etc. The underlying device
driver must effectively be configured in a similar fashion to what it would do
when IGMP snooping is enabled for IP multicast over these switchdev network
devices and unsolicited multicast must be filtered as early as possible in
the hardware.
(网络设备必须能够运行完整的 IP 协议栈，包括组播、DHCP、IPv4/6 等。
必要时，它应针对 VLAN、组播、单播等编程相应的过滤器。底层设备驱动必须
以与在这些 switchdev 网络设备上为 IP 组播启用 IGMP 侦听时类似的方式进行
有效配置，且非请求的组播必须在硬件中尽早被过滤。)

When configuring VLANs on top of the network device, all VLANs must be working,
irrespective of the state of other network devices (e.g.: other ports being part
of a VLAN-aware bridge doing ingress VID checking). See below for details.
(在网络设备之上配置 VLAN 时，所有 VLAN 都必须能工作，而不受其他网络
设备状态的影响 (例如：其他端口属于开启了入方向 VID 检查的 VLAN 感知
网桥)。详情见下文。)

If the device implements e.g.: VLAN filtering, putting the interface in
promiscuous mode should allow the reception of all VLAN tags (including those
not present in the filter(s)).
(如果设备实现了例如 VLAN 过滤，那么把接口置于混杂模式后应当能够接收
所有 VLAN 标签 (包括过滤器中不存在的那些)。)

Bridged switch ports
(桥接的交换机端口)
^^^^^^^^^^^^^^^^^^^^

When a switchdev enabled network device is added as a bridge member, it should
not disrupt any functionality of non-bridged network devices and they
should continue to behave as normal network devices. Depending on the bridge
configuration knobs below, the expected behavior is documented.
(当启用 switchdev 的网络设备被添加为网桥成员时，它不应破坏非桥接网络
设备的任何功能，这些设备应继续表现得像普通网络设备一样。根据下文的网桥
配置开关，预期行为已有说明。)

Bridge VLAN filtering
(网桥 VLAN 过滤)
^^^^^^^^^^^^^^^^^^^^^

The Linux bridge allows the configuration of a VLAN filtering mode (statically,
at device creation time, and dynamically, during run time) which must be
observed by the underlying switchdev network device/hardware:
(Linux 网桥允许配置 VLAN 过滤模式 (创建设备时静态配置，运行时动态
配置)，底层的 switchdev 网络设备/硬件必须遵守：)

- with VLAN filtering turned off: the bridge is strictly VLAN unaware and its
  data path will process all Ethernet frames as if they are VLAN-untagged.
  The bridge VLAN database can still be modified, but the modifications should
  have no effect while VLAN filtering is turned off. Frames ingressing the
  device with a VID that is not programmed into the bridge/switch's VLAN table
  must be forwarded and may be processed using a VLAN device (see below).
  (关闭 VLAN 过滤时：网桥严格地不感知 VLAN，其数据路径会像处理未打
  VLAN 标签的帧一样处理所有以太网帧。网桥 VLAN 数据库仍可修改，但在
  VLAN 过滤关闭期间这些修改不应产生任何效果。从设备进入的、VID 未被
  编程到网桥/交换机 VLAN 表中的帧必须被转发，并且可以用 VLAN 设备处理
  (见下文)。)

- with VLAN filtering turned on: the bridge is VLAN-aware and frames ingressing
  the device with a VID that is not programmed into the bridges/switch's VLAN
  table must be dropped (strict VID checking).
  (打开 VLAN 过滤时：网桥感知 VLAN，从设备进入的、VID 未被编程到网桥/
  交换机 VLAN 表中的帧必须被丢弃 (严格的 VID 检查)。)

When there is a VLAN device (e.g: sw0p1.100) configured on top of a switchdev
network device which is a bridge port member, the behavior of the software
network stack must be preserved, or the configuration must be refused if that
is not possible.
(当在作为网桥端口成员的 switchdev 网络设备之上配置了 VLAN 设备 (例如
sw0p1.100) 时，软件网络栈的行为必须得到保持；如果无法做到，则必须拒绝
该配置。)

- with VLAN filtering turned off, the bridge will process all ingress traffic
  for the port, except for the traffic tagged with a VLAN ID destined for a
  VLAN upper. The VLAN upper interface (which consumes the VLAN tag) can even
  be added to a second bridge, which includes other switch ports or software
  interfaces. Some approaches to ensure that the forwarding domain for traffic
  belonging to the VLAN upper interfaces are managed properly:
  (关闭 VLAN 过滤时，网桥会处理该端口的所有入方向流量，但目的为 VLAN
  上层设备、带有 VLAN ID 标签的流量除外。VLAN 上层接口 (消费 VLAN 标签
  的接口) 甚至可以被加入第二个网桥，该网桥可以包含其他交换机端口或软件
  接口。确保属于 VLAN 上层接口的流量的转发域得到正确管理的一些方法：)

    * If forwarding destinations can be managed per VLAN, the hardware could be
      configured to map all traffic, except the packets tagged with a VID
      belonging to a VLAN upper interface, to an internal VID corresponding to
      untagged packets. This internal VID spans all ports of the VLAN-unaware
      bridge. The VID corresponding to the VLAN upper interface spans the
      physical port of that VLAN interface, as well as the other ports that
      might be bridged with it.
      (如果可以按 VLAN 管理转发目的地，硬件可以被配置为把所有流量——
      除了带有属于 VLAN 上层接口的 VID 标签的数据包——映射到与未打标签
      数据包对应的内部 VID。该内部 VID 覆盖 VLAN 不感知网桥的所有端口。
      与 VLAN 上层接口对应的 VID 覆盖该 VLAN 接口的物理端口，以及可能
      与它桥接在一起的其他端口。)
    * Treat bridge ports with VLAN upper interfaces as standalone, and let
      forwarding be handled in the software data path.
      (把带有 VLAN 上层接口的网桥端口当作独立端口对待，让转发在软件
      数据路径中处理。)

- with VLAN filtering turned on, these VLAN devices can be created as long as
  the bridge does not have an existing VLAN entry with the same VID on any
  bridge port. These VLAN devices cannot be enslaved into the bridge since they
  duplicate functionality/use case with the bridge's VLAN data path processing.
  (打开 VLAN 过滤时，只要网桥的任何网桥端口上都没有已存在的同 VID VLAN
  表项，就可以创建这些 VLAN 设备。这些 VLAN 设备不能被加入网桥之中，
  因为它们与网桥的 VLAN 数据路径处理在功能/用例上重复。)

Non-bridged network ports of the same switch fabric must not be disturbed in any
way by the enabling of VLAN filtering on the bridge device(s). If the VLAN
filtering setting is global to the entire chip, then the standalone ports
should indicate to the network stack that VLAN filtering is required by setting
'rx-vlan-filter: on [fixed]' in the ethtool features.
(同一交换机矩阵中非桥接的网络端口绝不能因网桥设备上启用 VLAN 过滤而
受到任何干扰。如果 VLAN 过滤设置对整个芯片是全局的，那么独立端口应
通过在 ethtool 特性中设置 'rx-vlan-filter: on [fixed]' 来向网络栈表明
需要 VLAN 过滤。)

Because VLAN filtering can be turned on/off at runtime, the switchdev driver
must be able to reconfigure the underlying hardware on the fly to honor the
toggling of that option and behave appropriately. If that is not possible, the
switchdev driver can also refuse to support dynamic toggling of the VLAN
filtering knob at runtime and require a destruction of the bridge device(s) and
creation of new bridge device(s) with a different VLAN filtering value to
ensure VLAN awareness is pushed down to the hardware.
(由于 VLAN 过滤可以在运行时打开/关闭，switchdev 驱动必须能够即时重新
配置底层硬件，以遵从该选项的切换并恰当行事。如果做不到，switchdev 驱动
也可以拒绝在运行时支持 VLAN 过滤开关的动态切换，并要求销毁网桥设备、
以不同的 VLAN 过滤值创建新的网桥设备，以确保 VLAN 感知被下推到硬件。)

Even when VLAN filtering in the bridge is turned off, the underlying switch
hardware and driver may still configure itself in a VLAN-aware mode provided
that the behavior described above is observed.
(即使网桥中的 VLAN 过滤已关闭，只要遵守上述行为要求，底层交换硬件和
驱动仍可以把自己配置为 VLAN 感知模式。)

The VLAN protocol of the bridge plays a role in deciding whether a packet is
treated as tagged or not: a bridge using the 802.1ad protocol must treat both
VLAN-untagged packets, as well as packets tagged with 802.1Q headers, as
untagged.
(网桥的 VLAN 协议在决定数据包是否被视为已打标签方面起作用：使用
802.1ad 协议的网桥必须把未打 VLAN 标签的数据包以及带 802.1Q 头标签的
数据包都视为未打标签。)

The 802.1p (VID 0) tagged packets must be treated in the same way by the device
as untagged packets, since the bridge device does not allow the manipulation of
VID 0 in its database.
(由于网桥设备不允许在其数据库中操作 VID 0，设备必须以与未打标签数据包
相同的方式对待 802.1p (VID 0) 打标签的数据包。)

When the bridge has VLAN filtering enabled and a PVID is not configured on the
ingress port, untagged and 802.1p tagged packets must be dropped. When the bridge
has VLAN filtering enabled and a PVID exists on the ingress port, untagged and
priority-tagged packets must be accepted and forwarded according to the
bridge's port membership of the PVID VLAN. When the bridge has VLAN filtering
disabled, the presence/lack of a PVID should not influence the packet
forwarding decision.
(当网桥启用了 VLAN 过滤且入方向端口未配置 PVID 时，未打标签和 802.1p
打标签的数据包必须被丢弃。当网桥启用了 VLAN 过滤且入方向端口存在 PVID
时，未打标签和优先级标签的数据包必须被接受，并根据网桥在 PVID VLAN 中
的端口成员关系进行转发。当网桥禁用了 VLAN 过滤时，PVID 的有无不应影响
数据包转发决定。)

Bridge IGMP snooping
(网桥 IGMP 侦听)
^^^^^^^^^^^^^^^^^^^^

The Linux bridge allows the configuration of IGMP snooping (statically, at
interface creation time, or dynamically, during runtime) which must be observed
by the underlying switchdev network device/hardware in the following way:
(Linux 网桥允许配置 IGMP 侦听 (创建接口时静态配置，或运行时动态配置)，
底层的 switchdev 网络设备/硬件必须按以下方式遵守：)

- when IGMP snooping is turned off, multicast traffic must be flooded to all
  ports within the same bridge that have mcast_flood=true. The CPU/management
  port should ideally not be flooded (unless the ingress interface has
  IFF_ALLMULTI or IFF_PROMISC) and continue to learn multicast traffic through
  the network stack notifications. If the hardware is not capable of doing that
  then the CPU/management port must also be flooded and multicast filtering
  happens in software.
  (关闭 IGMP 侦听时，组播流量必须泛洪到同一网桥内所有 mcast_flood=true
  的端口。CPU/管理端口理想情况下不应被泛洪 (除非入方向接口设置了
  IFF_ALLMULTI 或 IFF_PROMISC)，并应继续通过网络栈通知来学习组播流量。
  如果硬件无法做到这一点，那么 CPU/管理端口也必须被泛洪，组播过滤在
  软件中进行。)

- when IGMP snooping is turned on, multicast traffic must selectively flow
  to the appropriate network ports (including CPU/management port). Flooding of
  unknown multicast should be only towards the ports connected to a multicast
  router (the local device may also act as a multicast router).
  (打开 IGMP 侦听时，组播流量必须有选择地流向适当的网络端口 (包括
  CPU/管理端口)。未知组播的泛洪应只面向连接到组播路由器的端口 (本地
  设备也可以充当组播路由器)。)

The switch must adhere to RFC 4541 and flood multicast traffic accordingly
since that is what the Linux bridge implementation does.
(交换机必须遵守 RFC 4541 并相应地泛洪组播流量，因为 Linux 网桥实现
就是这样做的。)

Because IGMP snooping can be turned on/off at runtime, the switchdev driver
must be able to reconfigure the underlying hardware on the fly to honor the
toggling of that option and behave appropriately.
(由于 IGMP 侦听可以在运行时打开/关闭，switchdev 驱动必须能够即时重新
配置底层硬件，以遵从该选项的切换并恰当行事。)

A switchdev driver can also refuse to support dynamic toggling of the multicast
snooping knob at runtime and require the destruction of the bridge device(s)
and creation of a new bridge device(s) with a different multicast snooping
value.
(switchdev 驱动也可以拒绝在运行时支持组播侦听开关的动态切换，并要求
销毁网桥设备、以不同的组播侦听值创建新的网桥设备。)
