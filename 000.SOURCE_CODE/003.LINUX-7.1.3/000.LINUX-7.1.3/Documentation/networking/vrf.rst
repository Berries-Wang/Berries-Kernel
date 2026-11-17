.. SPDX-License-Identifier: GPL-2.0

====================================
Virtual Routing and Forwarding (VRF)
====================================

(虚拟路由和转发 (VRF))

The VRF Device
(VRF 设备)
==============

The VRF device combined with ip rules provides the ability to create virtual
routing and forwarding domains (aka VRFs, VRF-lite to be specific) in the
Linux network stack. One use case is the multi-tenancy problem where each
tenant has their own unique routing tables and in the very least need
different default gateways.
(VRF 设备与 ip 规则相结合，提供了在 Linux 网络协议栈中创建虚拟路由和转发域
(即 VRF，具体指 VRF-lite) 的能力。一个用例是多租户问题：每个租户拥有自己
独一无二的路由表，并且至少需要不同的默认网关。)

Processes can be "VRF aware" by binding a socket to the VRF device. Packets
through the socket then use the routing table associated with the VRF
device. An important feature of the VRF device implementation is that it
impacts only Layer 3 and above so L2 tools (e.g., LLDP) are not affected
(ie., they do not need to be run in each VRF). The design also allows
the use of higher priority ip rules (Policy Based Routing, PBR) to take
precedence over the VRF device rules directing specific traffic as desired.
(通过将套接字绑定到 VRF 设备，进程可以做到 "感知 VRF"。通过该套接字的
数据包随后会使用与 VRF 设备关联的路由表。VRF 设备实现的一个重要特性是它
只影响第 3 层及以上，因此 L2 工具 (例如 LLDP) 不受影响 (即它们不需要在
每个 VRF 中运行)。该设计还允许使用更高优先级的 ip 规则 (基于策略的路由，
PBR) 来优先于 VRF 设备规则，从而按需要引导特定流量。)

In addition, VRF devices allow VRFs to be nested within namespaces. For
example network namespaces provide separation of network interfaces at the
device layer, VLANs on the interfaces within a namespace provide L2 separation
and then VRF devices provide L3 separation.
(此外，VRF 设备允许将 VRF 嵌套在命名空间之内。例如，网络命名空间在设备层
提供网络接口的隔离，命名空间内接口上的 VLAN 提供 L2 隔离，然后 VRF 设备
提供 L3 隔离。)

Design
(设计)
------

A VRF device is created with an associated route table. Network interfaces
are then enslaved to a VRF device::
(创建 VRF 设备时会关联一个路由表。然后，网络接口被 enslave (捆绑) 到
VRF 设备上：)

	 +-----------------------------+
	 |           vrf-blue          |  ===> route table 10
	 +-----------------------------+
	    |        |            |
	 +------+ +------+     +-------------+
	 | eth1 | | eth2 | ... |    bond1    |
	 +------+ +------+     +-------------+
				  |       |
			      +------+ +------+
			      | eth8 | | eth9 |
			      +------+ +------+

Packets received on an enslaved device and are switched to the VRF device
in the IPv4 and IPv6 processing stacks giving the impression that packets
flow through the VRF device. Similarly on egress routing rules are used to
send packets to the VRF device driver before getting sent out the actual
interface. This allows tcpdump on a VRF device to capture all packets into
and out of the VRF as a whole\ [1]_. Similarly, netfilter\ [2]_ and tc rules
can be applied using the VRF device to specify rules that apply to the VRF
domain as a whole.
(在 enslaved 设备上收到的数据包会在 IPv4 和 IPv6 处理栈中被切换到 VRF
设备，从而给人一种数据包流经 VRF 设备的印象。类似地，在出口方向上，路由
规则会在数据包从实际接口发出之前，将其发送给 VRF 设备驱动程序。这使得
VRF 设备上的 tcpdump 能够捕获整个 VRF 进出的所有数据包\ [1]_ 。类似地，
可以使用 VRF 设备应用 netfilter\ [2]_ 和 tc 规则，来指定适用于整个 VRF
域的规则。)

.. [1] Packets in the forwarded state do not flow through the device, so those
       packets are not seen by tcpdump. Will revisit this limitation in a
       future release.
       (处于转发状态的数据包不流经该设备，因此 tcpdump 看不到这些数据包。
       将在未来的版本中重新审视这一限制。)

.. [2] Iptables on ingress supports PREROUTING with skb->dev set to the real
       ingress device and both INPUT and PREROUTING rules with skb->dev set to
       the VRF device. For egress POSTROUTING and OUTPUT rules can be written
       using either the VRF device or real egress device.
       (入口方向的 Iptables 支持 skb->dev 设置为真实入口设备的 PREROUTING，
       同时支持 skb->dev 设置为 VRF 设备的 INPUT 和 PREROUTING 规则。对于
       出口方向，POSTROUTING 和 OUTPUT 规则可以使用 VRF 设备或真实出口设备
       来编写。)

Setup
(设置)
-----

1. VRF device is created with an association to a FIB table.
   e.g,::
   (1. 创建 VRF 设备时将其与一个 FIB 表关联。
   例如：)

	ip link add vrf-blue type vrf table 10
	ip link set dev vrf-blue up

2. An l3mdev FIB rule directs lookups to the table associated with the device.
   A single l3mdev rule is sufficient for all VRFs. The VRF device adds the
   l3mdev rule for IPv4 and IPv6 when the first device is created with a
   default preference of 1000. Users may delete the rule if desired and add
   with a different priority or install per-VRF rules.
   (2. l3mdev FIB 规则将查找引导到与设备关联的表。一条 l3mdev 规则就足以
   覆盖所有 VRF。VRF 设备在创建第一个设备时，会以默认优先级 1000 为 IPv4
   和 IPv6 添加 l3mdev 规则。用户可以根据需要删除该规则，并以不同的优先级
   重新添加，或者安装针对每个 VRF 的规则。)

   Prior to the v4.8 kernel iif and oif rules are needed for each VRF device::
   (在 v4.8 内核之前，每个 VRF 设备都需要 iif 和 oif 规则：)

       ip ru add oif vrf-blue table 10
       ip ru add iif vrf-blue table 10

3. Set the default route for the table (and hence default route for the VRF)::
   (3. 为该表设置默认路由 (也即 VRF 的默认路由)：)

       ip route add table 10 unreachable default metric 4278198272

   This high metric value ensures that the default unreachable route can
   be overridden by a routing protocol suite.  FRRouting interprets
   kernel metrics as a combined admin distance (upper byte) and priority
   (lower 3 bytes).  Thus the above metric translates to [255/8192].
   (这么高的 metric 值确保默认的 unreachable 路由可以被路由协议套件覆盖。
   FRRouting 将内核 metric 解释为管理距离 (高字节) 和优先级 (低 3 字节)
   的组合。因此上面的 metric 对应 [255/8192]。)

4. Enslave L3 interfaces to a VRF device::
   (4. 将 L3 接口 enslave 到 VRF 设备：)

       ip link set dev eth1 master vrf-blue

   Local and connected routes for enslaved devices are automatically moved to
   the table associated with VRF device. Any additional routes depending on
   the enslaved device are dropped and will need to be reinserted to the VRF
   FIB table following the enslavement.
   (被 enslave 设备的本地路由和直连路由会自动移动到与 VRF 设备关联的表中。
   任何依赖于被 enslave 设备的其他路由都会被丢弃，需要在 enslave 之后重新
   插入到 VRF FIB 表中。)

   The IPv6 sysctl option keep_addr_on_down can be enabled to keep IPv6 global
   addresses as VRF enslavement changes::
   (可以启用 IPv6 sysctl 选项 keep_addr_on_down，以便在 VRF enslave 关系
   变化时保留 IPv6 全局地址：)

       sysctl -w net.ipv6.conf.all.keep_addr_on_down=1

5. Additional VRF routes are added to associated table::
   (5. 其他 VRF 路由被添加到关联的表中：)

       ip route add table 10 ...


Applications
(应用程序)
------------

Applications that are to work within a VRF need to bind their socket to the
VRF device::
(要在 VRF 中工作的应用程序，需要将其套接字绑定到 VRF 设备：)

    setsockopt(sd, SOL_SOCKET, SO_BINDTODEVICE, dev, strlen(dev)+1);

or to specify the output device using cmsg and IP_PKTINFO.
(或者使用 cmsg 和 IP_PKTINFO 来指定输出设备。)

By default the scope of the port bindings for unbound sockets is
limited to the default VRF. That is, it will not be matched by packets
arriving on interfaces enslaved to an l3mdev and processes may bind to
the same port if they bind to an l3mdev.
(默认情况下，未绑定套接字的端口绑定范围仅限于默认 VRF。也就是说，从
enslave 到 l3mdev 的接口到达的数据包不会匹配它，而如果进程绑定到某个
l3mdev，则它们可以绑定到相同的端口。)

TCP & UDP services running in the default VRF context (ie., not bound
to any VRF device) can work across all VRF domains by enabling the
tcp_l3mdev_accept and udp_l3mdev_accept sysctl options::
(在默认 VRF 上下文中运行 (即未绑定到任何 VRF 设备) 的 TCP 和 UDP 服务，
可以通过启用 tcp_l3mdev_accept 和 udp_l3mdev_accept sysctl 选项来跨所有
VRF 域工作：)

    sysctl -w net.ipv4.tcp_l3mdev_accept=1
    sysctl -w net.ipv4.udp_l3mdev_accept=1

These options are disabled by default so that a socket in a VRF is only
selected for packets in that VRF. There is a similar option for RAW
sockets, which is enabled by default for reasons of backwards compatibility.
This is so as to specify the output device with cmsg and IP_PKTINFO, but
using a socket not bound to the corresponding VRF. This allows e.g. older ping
implementations to be run with specifying the device but without executing it
in the VRF. This option can be disabled so that packets received in a VRF
context are only handled by a raw socket bound to the VRF, and packets in the
default VRF are only handled by a socket not bound to any VRF::
(这些选项默认禁用，这样 VRF 中的套接字只会为该 VRF 中的数据包所选用。RAW
套接字有一个类似的选项，出于向后兼容的原因默认启用。这样做的目的是通过
cmsg 和 IP_PKTINFO 指定输出设备，但使用未绑定到相应 VRF 的套接字。这使得
例如较旧的 ping 实现能够在指定设备的情况下运行，而不必在 VRF 中执行。可以
禁用该选项，使在 VRF 上下文中收到的数据包只由绑定到该 VRF 的 RAW 套接字
处理，而默认 VRF 中的数据包只由未绑定到任何 VRF 的套接字处理：)

    sysctl -w net.ipv4.raw_l3mdev_accept=0

netfilter rules on the VRF device can be used to limit access to services
running in the default VRF context as well.
(VRF 设备上的 netfilter 规则也可用于限制对在默认 VRF 上下文中运行的服务的
访问。)

Using VRF-aware applications (applications which simultaneously create sockets
outside and inside VRFs) in conjunction with ``net.ipv4.tcp_l3mdev_accept=1``
is possible but may lead to problems in some situations. With that sysctl
value, it is unspecified which listening socket will be selected to handle
connections for VRF traffic; ie. either a socket bound to the VRF or an unbound
socket may be used to accept new connections from a VRF. This somewhat
unexpected behavior can lead to problems if sockets are configured with extra
options (ex. TCP MD5 keys) with the expectation that VRF traffic will
exclusively be handled by sockets bound to VRFs, as would be the case with
``net.ipv4.tcp_l3mdev_accept=0``. Finally and as a reminder, regardless of
which listening socket is selected, established sockets will be created in the
VRF based on the ingress interface, as documented earlier.
(将感知 VRF 的应用程序 (同时在 VRF 之外和之内创建套接字的应用程序) 与
``net.ipv4.tcp_l3mdev_accept=1`` 结合使用是可能的，但在某些情况下可能会
引发问题。在该 sysctl 值下，将选择哪个监听套接字来处理 VRF 流量的连接是
未定义的；也就是说，绑定到 VRF 的套接字或未绑定的套接字都可能被用来接受
来自 VRF 的新连接。这种有些出乎意料的行为可能会导致问题：如果套接字配置了
额外选项 (例如 TCP MD5 密钥)，并期望 VRF 流量只由绑定到 VRF 的套接字处理，
(就像 ``net.ipv4.tcp_l3mdev_accept=0`` 时那样)，就会出问题。最后提醒一下，
无论选择了哪个监听套接字，已建立的套接字都会根据入口接口在相应 VRF 中
创建，如前文所述。)

--------------------------------------------------------------------------------

Using iproute2 for VRFs
(使用 iproute2 管理 VRF)
=======================

iproute2 supports the vrf keyword as of v4.7. For backwards compatibility this
section lists both commands where appropriate -- with the vrf keyword and the
older form without it.
(iproute2 从 v4.7 起支持 vrf 关键字。为了向后兼容，本节在适当之处同时列出
两种命令 -- 带 vrf 关键字的和不带该关键字的旧形式。)

1. Create a VRF
   (1. 创建 VRF)

   To instantiate a VRF device and associate it with a table::
   (要实例化一个 VRF 设备并将其与一个表关联：)

       $ ip link add dev NAME type vrf table ID

   As of v4.8 the kernel supports the l3mdev FIB rule where a single rule
   covers all VRFs. The l3mdev rule is created for IPv4 and IPv6 on first
   device create.
   (从 v4.8 起，内核支持 l3mdev FIB 规则，一条规则即可覆盖所有 VRF。
   l3mdev 规则在创建第一个设备时为 IPv4 和 IPv6 创建。)

2. List VRFs
   (2. 列出 VRF)

   To list VRFs that have been created::
   (要列出已创建的 VRF：)

       $ ip [-d] link show type vrf
	 NOTE: The -d option is needed to show the table id

   For example::

       $ ip -d link show type vrf
       11: mgmt: <NOARP,MASTER,UP,LOWER_UP> mtu 1500 qdisc pfifo_fast state UP mode DEFAULT group default qlen 1000
	   link/ether 72:b3:ba:91:e2:24 brd ff:ff:ff:ff:ff:ff promiscuity 0
	   vrf table 1 addrgenmode eui64
       12: red: <NOARP,MASTER,UP,LOWER_UP> mtu 1500 qdisc pfifo_fast state UP mode DEFAULT group default qlen 1000
	   link/ether b6:6f:6e:f6:da:73 brd ff:ff:ff:ff:ff:ff promiscuity 0
	   vrf table 10 addrgenmode eui64
       13: blue: <NOARP,MASTER,UP,LOWER_UP> mtu 1500 qdisc pfifo_fast state UP mode DEFAULT group default qlen 1000
	   link/ether 36:62:e8:7d:bb:8c brd ff:ff:ff:ff:ff:ff promiscuity 0
	   vrf table 66 addrgenmode eui64
       14: green: <NOARP,MASTER,UP,LOWER_UP> mtu 1500 qdisc pfifo_fast state UP mode DEFAULT group default qlen 1000
	   link/ether e6:28:b8:63:70:bb brd ff:ff:ff:ff:ff:ff promiscuity 0
	   vrf table 81 addrgenmode eui64


   Or in brief output::
   (或者以简略输出显示：)

       $ ip -br link show type vrf
       mgmt         UP             72:b3:ba:91:e2:24 <NOARP,MASTER,UP,LOWER_UP>
       red          UP             b6:6f:6e:f6:da:73 <NOARP,MASTER,UP,LOWER_UP>
       blue         UP             36:62:e8:7d:bb:8c <NOARP,MASTER,UP,LOWER_UP>
       green        UP             e6:28:b8:63:70:bb <NOARP,MASTER,UP,LOWER_UP>


3. Assign a Network Interface to a VRF
   (3. 将网络接口分配给 VRF)

   Network interfaces are assigned to a VRF by enslaving the netdevice to a
   VRF device::
   (通过将 netdevice enslave 到 VRF 设备，可以把网络接口分配给一个 VRF：)

       $ ip link set dev NAME master NAME

   On enslavement connected and local routes are automatically moved to the
   table associated with the VRF device.
   (在 enslave 时，直连路由和本地路由会自动移动到与 VRF 设备关联的表中。)

   For example::
   (例如：)

       $ ip link set dev eth0 master mgmt


4. Show Devices Assigned to a VRF
   (4. 显示分配给 VRF 的设备)

   To show devices that have been assigned to a specific VRF add the master
   option to the ip command::
   (要显示已分配给特定 VRF 的设备，请在 ip 命令中加上 master 选项：)

       $ ip link show vrf NAME
       $ ip link show master NAME

   For example::

       $ ip link show vrf red
       3: eth1: <BROADCAST,MULTICAST,UP,LOWER_UP> mtu 1500 qdisc pfifo_fast master red state UP mode DEFAULT group default qlen 1000
	   link/ether 02:00:00:00:02:02 brd ff:ff:ff:ff:ff:ff
       4: eth2: <BROADCAST,MULTICAST,UP,LOWER_UP> mtu 1500 qdisc pfifo_fast master red state UP mode DEFAULT group default qlen 1000
	   link/ether 02:00:00:00:02:03 brd ff:ff:ff:ff:ff:ff
       7: eth5: <BROADCAST,MULTICAST> mtu 1500 qdisc noop master red state DOWN mode DEFAULT group default qlen 1000
	   link/ether 02:00:00:00:02:06 brd ff:ff:ff:ff:ff:ff


   Or using the brief output::
   (或者使用简略输出：)

       $ ip -br link show vrf red
       eth1             UP             02:00:00:00:02:02 <BROADCAST,MULTICAST,UP,LOWER_UP>
       eth2             UP             02:00:00:00:02:03 <BROADCAST,MULTICAST,UP,LOWER_UP>
       eth5             DOWN           02:00:00:00:02:06 <BROADCAST,MULTICAST>


5. Show Neighbor Entries for a VRF
   (5. 显示 VRF 的邻居条目)

   To list neighbor entries associated with devices enslaved to a VRF device
   add the master option to the ip command::
   (要列出与 enslave 到 VRF 设备的设备相关联的邻居条目，请在 ip 命令中加上
   master 选项：)

       $ ip [-6] neigh show vrf NAME
       $ ip [-6] neigh show master NAME

   For example::

       $  ip neigh show vrf red
       10.2.1.254 dev eth1 lladdr a6:d9:c7:4f:06:23 REACHABLE
       10.2.2.254 dev eth2 lladdr 5e:54:01:6a:ee:80 REACHABLE

       $ ip -6 neigh show vrf red
       2002:1::64 dev eth1 lladdr a6:d9:c7:4f:06:23 REACHABLE


6. Show Addresses for a VRF
   (6. 显示 VRF 的地址)

   To show addresses for interfaces associated with a VRF add the master
   option to the ip command::
   (要显示与 VRF 关联的接口的地址，请在 ip 命令中加上 master 选项：)

       $ ip addr show vrf NAME
       $ ip addr show master NAME

   For example::

	$ ip addr show vrf red
	3: eth1: <BROADCAST,MULTICAST,UP,LOWER_UP> mtu 1500 qdisc pfifo_fast master red state UP group default qlen 1000
	    link/ether 02:00:00:00:02:02 brd ff:ff:ff:ff:ff:ff
	    inet 10.2.1.2/24 brd 10.2.1.255 scope global eth1
	       valid_lft forever preferred_lft forever
	    inet6 2002:1::2/120 scope global
	       valid_lft forever preferred_lft forever
	    inet6 fe80::ff:fe00:202/64 scope link
	       valid_lft forever preferred_lft forever
	4: eth2: <BROADCAST,MULTICAST,UP,LOWER_UP> mtu 1500 qdisc pfifo_fast master red state UP group default qlen 1000
	    link/ether 02:00:00:00:02:03 brd ff:ff:ff:ff:ff:ff
	    inet 10.2.2.2/24 brd 10.2.2.255 scope global eth2
	       valid_lft forever preferred_lft forever
	    inet6 2002:2::2/120 scope global
	       valid_lft forever preferred_lft forever
	    inet6 fe80::ff:fe00:203/64 scope link
	       valid_lft forever preferred_lft forever
	7: eth5: <BROADCAST,MULTICAST> mtu 1500 qdisc noop master red state DOWN group default qlen 1000
	    link/ether 02:00:00:00:02:06 brd ff:ff:ff:ff:ff:ff

   Or in brief format::
   (或者以简略格式显示：)

	$ ip -br addr show vrf red
	eth1             UP             10.2.1.2/24 2002:1::2/120 fe80::ff:fe00:202/64
	eth2             UP             10.2.2.2/24 2002:2::2/120 fe80::ff:fe00:203/64
	eth5             DOWN


7. Show Routes for a VRF
   (7. 显示 VRF 的路由)

   To show routes for a VRF use the ip command to display the table associated
   with the VRF device::
   (要显示 VRF 的路由，可使用 ip 命令显示与 VRF 设备关联的表：)

       $ ip [-6] route show vrf NAME
       $ ip [-6] route show table ID

   For example::

	$ ip route show vrf red
	unreachable default  metric 4278198272
	broadcast 10.2.1.0 dev eth1  proto kernel  scope link  src 10.2.1.2
	10.2.1.0/24 dev eth1  proto kernel  scope link  src 10.2.1.2
	local 10.2.1.2 dev eth1  proto kernel  scope host  src 10.2.1.2
	broadcast 10.2.1.255 dev eth1  proto kernel  scope link  src 10.2.1.2
	broadcast 10.2.2.0 dev eth2  proto kernel  scope link  src 10.2.2.2
	10.2.2.0/24 dev eth2  proto kernel  scope link  src 10.2.2.2
	local 10.2.2.2 dev eth2  proto kernel  scope host  src 10.2.2.2
	broadcast 10.2.2.255 dev eth2  proto kernel  scope link  src 10.2.2.2

	$ ip -6 route show vrf red
	local 2002:1:: dev lo  proto none  metric 0  pref medium
	local 2002:1::2 dev lo  proto none  metric 0  pref medium
	2002:1::/120 dev eth1  proto kernel  metric 256  pref medium
	local 2002:2:: dev lo  proto none  metric 0  pref medium
	local 2002:2::2 dev lo  proto none  metric 0  pref medium
	2002:2::/120 dev eth2  proto kernel  metric 256  pref medium
	local fe80:: dev lo  proto none  metric 0  pref medium
	local fe80:: dev lo  proto none  metric 0  pref medium
	local fe80::ff:fe00:202 dev lo  proto none  metric 0  pref medium
	local fe80::ff:fe00:203 dev lo  proto none  metric 0  pref medium
	fe80::/64 dev eth1  proto kernel  metric 256  pref medium
	fe80::/64 dev eth2  proto kernel  metric 256  pref medium
	ff00::/8 dev red  metric 256  pref medium
	ff00::/8 dev eth1  metric 256  pref medium
	ff00::/8 dev eth2  metric 256  pref medium
	unreachable default dev lo  metric 4278198272  error -101 pref medium

8. Route Lookup for a VRF
   (8. 为 VRF 做路由查找)

   A test route lookup can be done for a VRF::
   (可以对 VRF 进行测试性的路由查找：)

       $ ip [-6] route get vrf NAME ADDRESS
       $ ip [-6] route get oif NAME ADDRESS

   For example::

	$ ip route get 10.2.1.40 vrf red
	10.2.1.40 dev eth1  table red  src 10.2.1.2
	    cache

	$ ip -6 route get 2002:1::32 vrf red
	2002:1::32 from :: dev eth1  table red  proto kernel  src 2002:1::2  metric 256  pref medium


9. Removing Network Interface from a VRF
   (9. 从 VRF 中移除网络接口)

   Network interfaces are removed from a VRF by breaking the enslavement to
   the VRF device::
   (通过解除与 VRF 设备的 enslave 关系，可以将网络接口从 VRF 中移除：)

       $ ip link set dev NAME nomaster

   Connected routes are moved back to the default table and local entries are
   moved to the local table.
   (直连路由会被移回默认表，本地条目会被移到本地表中。)

   For example::
   (例如：)

    $ ip link set dev eth0 nomaster

--------------------------------------------------------------------------------

Commands used in this example::
(本示例中使用的命令：)

     cat >> /etc/iproute2/rt_tables.d/vrf.conf <<EOF
     1  mgmt
     10 red
     66 blue
     81 green
     EOF

     function vrf_create
     {
	 VRF=$1
	 TBID=$2

	 # create VRF device
	 ip link add ${VRF} type vrf table ${TBID}

	 if [ "${VRF}" != "mgmt" ]; then
	     ip route add table ${TBID} unreachable default metric 4278198272
	 fi
	 ip link set dev ${VRF} up
     }

     vrf_create mgmt 1
     ip link set dev eth0 master mgmt

     vrf_create red 10
     ip link set dev eth1 master red
     ip link set dev eth2 master red
     ip link set dev eth5 master red

     vrf_create blue 66
     ip link set dev eth3 master blue

     vrf_create green 81
     ip link set dev eth4 master green


     Interface addresses from /etc/network/interfaces:
     auto eth0
     iface eth0 inet static
	   address 10.0.0.2
	   netmask 255.255.255.0
	   gateway 10.0.0.254

     iface eth0 inet6 static
	   address 2000:1::2
	   netmask 120

     auto eth1
     iface eth1 inet static
	   address 10.2.1.2
	   netmask 255.255.255.0

     iface eth1 inet6 static
	   address 2002:1::2
	   netmask 120

     auto eth2
     iface eth2 inet static
	   address 10.2.2.2
	   netmask 255.255.255.0

     iface eth2 inet6 static
	   address 2002:2::2
	   netmask 120

     auto eth3
     iface eth3 inet static
	   address 10.2.3.2
	   netmask 255.255.255.0

     iface eth3 inet6 static
	   address 2002:3::2
	   netmask 120

     auto eth4
     iface eth4 inet static
	   address 10.2.4.2
	   netmask 255.255.255.0

     iface eth4 inet6 static
	   address 2002:4::2
	   netmask 120
