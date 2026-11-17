.. SPDX-License-Identifier: GPL-2.0

=====================================================================
Netfilter's flowtable infrastructure(Netfilter 的 flowtable 基础设施)
=====================================================================

This documentation describes the Netfilter flowtable infrastructure which allows
you to define a fastpath through the flowtable datapath. This infrastructure
also provides hardware offload support. The flowtable supports for the layer 3
IPv4 and IPv6 and the layer 4 TCP and UDP protocols.
(本文档描述 Netfilter 的 flowtable 基础设施，它允许你通过 flowtable 数据路径定义一条快速路径 (fastpath)。该基础设施还提供硬件卸载支持。flowtable 支持三层的 IPv4 与 IPv6 以及四层的 TCP 与 UDP 协议。)

Overview(概述)
--------------

Once the first packet of the flow successfully goes through the IP forwarding
path, from the second packet on, you might decide to offload the flow to the
flowtable through your ruleset. The flowtable infrastructure provides a rule
action that allows you to specify when to add a flow to the flowtable.
(当流的第一个数据包成功通过 IP 转发路径之后，从第二个数据包起，你可以决定通过自己的规则集把该流卸载到 flowtable。flowtable 基础设施提供一条规则动作，允许你指定何时把流加入 flowtable。)

A packet that finds a matching entry in the flowtable (ie. flowtable hit) is
transmitted to the output netdevice via neigh_xmit(), hence, packets bypass the
classic IP forwarding path (the visible effect is that you do not see these
packets from any of the Netfilter hooks coming after ingress). In case that
there is no matching entry in the flowtable (ie. flowtable miss), the packet
follows the classic IP forwarding path.
(在 flowtable 中找到匹配表项的数据包（即 flowtable hit）会经由 neigh_xmit() 发送到输出网络设备，因此这些数据包绕过经典的 IP 转发路径（可见的效果是：你在 ingress 之后的任何 Netfilter 钩子处都看不到这些数据包）。如果 flowtable 中没有匹配表项（即 flowtable miss），数据包则走经典的 IP 转发路径。)

The flowtable uses a resizable hashtable. Lookups are based on the following
n-tuple selectors: layer 2 protocol encapsulation (VLAN and PPPoE), layer 3
source and destination, layer 4 source and destination ports and the input
interface (useful in case there are several conntrack zones in place).
(flowtable 使用可调整大小的哈希表。查找基于以下 n 元组选择器：二层协议封装（VLAN 和 PPPoE）、三层源与目的地址、四层源与目的端口，以及输入接口（在存在多个 conntrack zone 时很有用）。)

The 'flow add' action allows you to populate the flowtable, the user selectively
specifies what flows are placed into the flowtable. Hence, packets follow the
classic IP forwarding path unless the user explicitly instruct flows to use this
new alternative forwarding path via policy.
('flow add' 动作允许你填充 flowtable，由用户有选择地指定哪些流放入 flowtable。因此，除非用户通过策略明确指示流使用这条新的替代转发路径，否则数据包仍走经典的 IP 转发路径。)

The flowtable datapath is represented in Fig.1, which describes the classic IP
forwarding path including the Netfilter hooks and the flowtable fastpath bypass.
(flowtable 数据路径如图 1 所示，图中描述了包含 Netfilter 钩子的经典 IP 转发路径，以及 flowtable 快速路径旁路。)

::

					 userspace process
					  ^              |
					  |              |
				     _____|____     ____\/___
				    /          \   /         \
				    |   input   |  |  output  |
				    \__________/   \_________/
					 ^               |
					 |               |
      _________      __________      ---------     _____\/_____
     /         \    /          \     |Routing |   /            \
  -->  ingress  ---> prerouting ---> |decision|   | postrouting |--> neigh_xmit
     \_________/    \__________/     ----------   \____________/          ^
       |      ^                          |               ^                |
   flowtable  |                     ____\/___            |                |
       |      |                    /         \           |                |
    __\/___   |                    | forward |------------                |
    |-----|   |                    \_________/                            |
    |-----|   |                 'flow offload' rule                       |
    |-----|   |                   adds entry to                           |
    |_____|   |                     flowtable                             |
       |      |                                                           |
      / \     |                                                           |
     /hit\_no_|                                                           |
     \ ? /                                                                |
      \ /                                                                 |
       |__yes_________________fastpath bypass ____________________________|

	       Fig.1 Netfilter hooks and flowtable interactions

The flowtable entry also stores the NAT configuration, so all packets are
mangled according to the NAT policy that is specified from the classic IP
forwarding path. The TTL is decremented before calling neigh_xmit(). Fragmented
traffic is passed up to follow the classic IP forwarding path given that the
transport header is missing, in this case, flowtable lookups are not possible.
TCP RST and FIN packets are also passed up to the classic IP forwarding path to
release the flow gracefully. Packets that exceed the MTU are also passed up to
the classic forwarding path to report packet-too-big ICMP errors to the sender.
(flowtable 表项还保存 NAT 配置，因此所有数据包都会按照经典 IP 转发路径中指定的 NAT 策略进行改写。在调用 neigh_xmit() 之前 TTL 会被递减。分片流量会被向上传递、走经典 IP 转发路径，因为传输层报头缺失，在这种情况下无法进行 flowtable 查找。TCP RST 和 FIN 数据包也会被向上传递到经典 IP 转发路径，以便优雅地释放流。超过 MTU 的数据包同样会被向上传递到经典转发路径，以便向发送方报告 packet-too-big ICMP 错误。)

Example configuration(配置示例)
-------------------------------

Enabling the flowtable bypass is relatively easy, you only need to create a
flowtable and add one rule to your forward chain::
(启用 flowtable 旁路相对简单，你只需要创建一个 flowtable，并在转发链中添加一条规则：)

	table inet x {
		flowtable f {
			hook ingress priority 0; devices = { eth0, eth1 };
		}
		chain y {
			type filter hook forward priority 0; policy accept;
			ip protocol tcp flow add @f
			counter packets 0 bytes 0
		}
	}

This example adds the flowtable 'f' to the ingress hook of the eth0 and eth1
netdevices. You can create as many flowtables as you want in case you need to
perform resource partitioning. The flowtable priority defines the order in which
hooks are run in the pipeline, this is convenient in case you already have a
nftables ingress chain (make sure the flowtable priority is smaller than the
nftables ingress chain hence the flowtable runs before in the pipeline).
(此示例把 flowtable 'f' 添加到 eth0 和 eth1 网络设备的 ingress 钩子上。如果需要进行资源分区，你可以创建任意多个 flowtable。flowtable 优先级定义了钩子在流水线中的运行顺序，如果你已经有 nftables ingress 链，这一点会很方便（确保 flowtable 优先级小于 nftables ingress 链，这样 flowtable 在流水线中先运行）。)

The 'flow offload' action from the forward chain 'y' adds an entry to the
flowtable for the TCP syn-ack packet coming in the reply direction. Once the
flow is offloaded, you will observe that the counter rule in the example above
does not get updated for the packets that are being forwarded through the
forwarding bypass.
(来自转发链 'y' 的 'flow offload' 动作会为来自回复方向的 TCP syn-ack 数据包向 flowtable 添加一个表项。一旦流被卸载，你会观察到上例中的 counter 规则不再对经由转发旁路转发的数据包进行更新。)

You can identify offloaded flows through the [OFFLOAD] tag when listing your
connection tracking table.
(列出连接跟踪表时，你可以通过 [OFFLOAD] 标签识别已卸载的流。)

::

	# conntrack -L
	tcp      6 src=10.141.10.2 dst=192.168.10.2 sport=52728 dport=5201 src=192.168.10.2 dst=192.168.10.1 sport=5201 dport=52728 [OFFLOAD] mark=0 use=2


Layer 2 encapsulation(二层封装)
-------------------------------

Since Linux kernel 5.13, the flowtable infrastructure discovers the real
netdevice behind VLAN and PPPoE netdevices. The flowtable software datapath
parses the VLAN and PPPoE layer 2 headers to extract the ethertype and the
VLAN ID / PPPoE session ID which are used for the flowtable lookups. The
flowtable datapath also deals with layer 2 decapsulation.
(自 Linux 内核 5.13 起，flowtable 基础设施能够发现 VLAN 和 PPPoE 网络设备背后的真实网络设备。flowtable 软件数据路径解析 VLAN 和 PPPoE 的二层报头，提取 ethertype 以及用于 flowtable 查找的 VLAN ID / PPPoE 会话 ID。flowtable 数据路径还负责二层解封装。)

You do not need to add the PPPoE and the VLAN devices to your flowtable,
instead the real device is sufficient for the flowtable to track your flows.
(你无需把 PPPoE 和 VLAN 设备加入你的 flowtable，只要有真实设备就足以让 flowtable 跟踪你的流。)

Bridge and IP forwarding(网桥与 IP 转发)
----------------------------------------

Since Linux kernel 5.13, you can add bridge ports to the flowtable. The
flowtable infrastructure discovers the topology behind the bridge device. This
allows the flowtable to define a fastpath bypass between the bridge ports
(represented as eth1 and eth2 in the example figure below) and the gateway
device (represented as eth0) in your switch/router.
(自 Linux 内核 5.13 起，你可以把网桥端口加入 flowtable。flowtable 基础设施会发现网桥设备背后的拓扑。这使得 flowtable 能够在你的交换机/路由器中的网桥端口（在下图示例中表示为 eth1 和 eth2）与网关设备（表示为 eth0）之间定义一条快速路径旁路。)

::

                      fastpath bypass
               .-------------------------.
              /                           \
              |           IP forwarding   |
              |          /             \ \/
              |       br0               eth0 ..... eth0
              .       / \                          *host B*
               -> eth1  eth2
                   .           *switch/router*
                   .
                   .
                 eth0
               *host A*

The flowtable infrastructure also supports for bridge VLAN filtering actions
such as PVID and untagged. You can also stack a classic VLAN device on top of
your bridge port.
(flowtable 基础设施还支持网桥 VLAN 过滤动作，例如 PVID 和 untagged。你也可以在网桥端口之上再堆叠一个经典 VLAN 设备。)

If you would like that your flowtable defines a fastpath between your bridge
ports and your IP forwarding path, you have to add your bridge ports (as
represented by the real netdevice) to your flowtable definition.
(如果你希望 flowtable 在你的网桥端口与 IP 转发路径之间定义一条快速路径，你必须把网桥端口（以真实网络设备表示）添加到你的 flowtable 定义中。)

Counters(计数器)
----------------

The flowtable can synchronize packet and byte counters with the existing
connection tracking entry by specifying the counter statement in your flowtable
definition, e.g.
(通过在你的 flowtable 定义中指定 counter 语句，flowtable 可以与现有的连接跟踪表项同步数据包和字节计数器，例如)

::

	table inet x {
		flowtable f {
			hook ingress priority 0; devices = { eth0, eth1 };
			counter
		}
	}

Counter support is available since Linux kernel 5.7.
(自 Linux 内核 5.7 起提供计数器支持。)

Hardware offload(硬件卸载)
--------------------------

If your network device provides hardware offload support, you can turn it on by
means of the 'offload' flag in your flowtable definition, e.g.
(如果你的网络设备提供硬件卸载支持，你可以通过 flowtable 定义中的 'offload' 标志将其打开，例如)

::

	table inet x {
		flowtable f {
			hook ingress priority 0; devices = { eth0, eth1 };
			flags offload;
		}
	}

There is a workqueue that adds the flows to the hardware. Note that a few
packets might still run over the flowtable software path until the workqueue has
a chance to offload the flow to the network device.
(有一个工作队列负责把流添加到硬件。注意，在工作队列有机会把流卸载到网络设备之前，可能仍有一些数据包走 flowtable 软件路径。)

You can identify hardware offloaded flows through the [HW_OFFLOAD] tag when
listing your connection tracking table. Please, note that the [OFFLOAD] tag
refers to the software offload mode, so there is a distinction between [OFFLOAD]
which refers to the software flowtable fastpath and [HW_OFFLOAD] which refers
to the hardware offload datapath being used by the flow.
(列出连接跟踪表时，你可以通过 [HW_OFFLOAD] 标签识别硬件卸载的流。请注意，[OFFLOAD] 标签指的是软件卸载模式，因此 [OFFLOAD]（指软件 flowtable 快速路径）与 [HW_OFFLOAD]（指流所使用的硬件卸载数据路径）之间是有区别的。)

The flowtable hardware offload infrastructure also supports for the DSA
(Distributed Switch Architecture).
(flowtable 硬件卸载基础设施还支持 DSA（分布式交换架构）。)

Limitations(限制)
-----------------

The flowtable behaves like a cache. The flowtable entries might get stale if
either the destination MAC address or the egress netdevice that is used for
transmission changes.
(flowtable 的行为类似缓存。如果用于传输的目的 MAC 地址或出口网络设备发生变化，flowtable 表项可能会变得过时。)

This might be a problem if:
(在以下情况下这可能成为问题：)

- You run the flowtable in software mode and you combine bridge and IP
  forwarding in your setup.
  (你在软件模式下运行 flowtable，并且在你的设置中把网桥和 IP 转发组合在了一起。)
- Hardware offload is enabled.
  (启用了硬件卸载。)

More reading(延伸阅读)
----------------------

This documentation is based on the LWN.net articles [1]_\ [2]_. Rafal Milecki
also made a very complete and comprehensive summary called "A state of network
acceleration" that describes how things were before this infrastructure was
mainlined [3]_ and it also makes a rough summary of this work [4]_.
(本文档基于 LWN.net 的文章 [1] 和 [2]。Rafal Milecki 还撰写了一篇非常完整而全面的总结，题为 "A state of network acceleration"，描述了该基础设施进入主线之前的情况 [3]，并对这项工作做了粗略总结 [4]。)

.. [1] https://lwn.net/Articles/738214/
.. [2] https://lwn.net/Articles/742164/
.. [3] http://lists.infradead.org/pipermail/lede-dev/2018-January/010830.html
.. [4] http://lists.infradead.org/pipermail/lede-dev/2018-January/010829.html
