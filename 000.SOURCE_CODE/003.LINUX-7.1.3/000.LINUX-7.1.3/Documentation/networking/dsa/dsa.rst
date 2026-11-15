============
Architecture
============

(架构)

This document describes the **Distributed Switch Architecture (DSA)** subsystem
design principles, limitations, interactions with other subsystems, and how to
develop drivers for this subsystem as well as a TODO for developers interested
in joining the effort.
(本文档描述 **Distributed Switch Architecture (DSA)** (分布式交换机架构) 子系统的设计原则、限制、与其他子系统的交互，以及如何为该子系统开发驱动程序，并为有兴趣加入这项工作的开发者提供了 TODO。)

Design principles
(设计原则)
=================

The Distributed Switch Architecture subsystem was primarily designed to
support Marvell Ethernet switches (MV88E6xxx, a.k.a. Link Street product
line) using Linux, but has since evolved to support other vendors as well.
(分布式交换机架构 (Distributed Switch Architecture) 子系统最初是为在使用 Linux 的情况下支持 Marvell 以太网交换机 (MV88E6xxx，即 Link Street 产品线) 而设计的，但此后已发展为支持其他厂商的产品。)

The original philosophy behind this design was to be able to use unmodified
Linux tools such as bridge, iproute2, ifconfig to work transparently whether
they configured/queried a switch port network device or a regular network
device.
(这一设计背后的最初理念是：能够不加修改地使用 bridge、iproute2、ifconfig 等 Linux 工具，无论它们配置/查询的是交换机端口网络设备还是普通网络设备，都能透明地工作。)

An Ethernet switch typically comprises multiple front-panel ports and one
or more CPU or management ports. The DSA subsystem currently relies on the
presence of a management port connected to an Ethernet controller capable of
receiving Ethernet frames from the switch. This is a very common setup for all
kinds of Ethernet switches found in Small Home and Office products: routers,
gateways, or even top-of-rack switches. This host Ethernet controller will
be later referred to as "conduit" and "cpu" in DSA terminology and code.
(以太网交换机通常包含多个前面板端口和一个或多个 CPU 或管理端口。DSA 子系统目前依赖于一个管理端口的存在，该端口连接到一个能够从交换机接收以太网帧的以太网控制器。对于在小型家庭和办公产品中发现的各种以太网交换机：路由器、网关，甚至机架顶部 (top-of-rack) 交换机，这都是非常常见的配置。这个主机以太网控制器在 DSA 术语和代码中后续将被称为 "conduit" 和 "cpu"。)

The D in DSA stands for Distributed, because the subsystem has been designed
with the ability to configure and manage cascaded switches on top of each other
using upstream and downstream Ethernet links between switches. These specific
ports are referred to as "dsa" ports in DSA terminology and code. A collection
of multiple switches connected to each other is called a "switch tree".
(DSA 中的 D 代表 Distributed (分布式)，因为该子系统的设计能够通过交换机之间的上行和下行以太网链路来配置和管理相互级联的交换机。这些特定端口在 DSA 术语和代码中被称为 "dsa" 端口。相互连接的多个交换机的集合被称为 "switch tree" (交换机树)。)

For each front-panel port, DSA creates specialized network devices which are
used as controlling and data-flowing endpoints for use by the Linux networking
stack. These specialized network interfaces are referred to as "user" network
interfaces in DSA terminology and code.
(对于每个前面板端口，DSA 都会创建专门的网络设备，作为供 Linux 网络栈使用的控制和数据流端点。这些专门的网络接口在 DSA 术语和代码中被称为 "user" 网络接口。)

The ideal case for using DSA is when an Ethernet switch supports a "switch tag"
which is a hardware feature making the switch insert a specific tag for each
Ethernet frame it receives to/from specific ports to help the management
interface figure out:
(使用 DSA 的理想情形是：以太网交换机支持 "switch tag" (交换机标签)——这是一项硬件特性，使交换机为其从/向特定端口接收/发送的每个以太网帧插入一个特定标签，以帮助管理接口弄清：)

- what port is this frame coming from
  (这个帧来自哪个端口)
- what was the reason why this frame got forwarded
  (这个帧被转发的原因是什么)
- how to send CPU originated traffic to specific ports
  (如何把源于 CPU 的流量发送到特定端口)

The subsystem does support switches not capable of inserting/stripping tags, but
the features might be slightly limited in that case (traffic separation relies
on Port-based VLAN IDs).
(该子系统确实也支持不具备插入/剥离标签能力的交换机，但在这种情况下功能可能略有受限 (流量分离依赖于基于端口的 VLAN ID)。)

Note that DSA does not currently create network interfaces for the "cpu" and
"dsa" ports because:
(注意，DSA 目前不为 "cpu" 和 "dsa" 端口创建网络接口，原因是：)

- the "cpu" port is the Ethernet switch facing side of the management
  controller, and as such, would create a duplication of feature, since you
  would get two interfaces for the same conduit: conduit netdev, and "cpu" netdev
  ("cpu" 端口是管理控制器面向以太网交换机的一侧，为它创建网络接口会造成功能重复，因为同一个 conduit 会得到两个接口：conduit netdev 和 "cpu" netdev)

- the "dsa" port(s) are just conduits between two or more switches, and as such
  cannot really be used as proper network interfaces either, only the
  downstream, or the top-most upstream interface makes sense with that model
  ("dsa" 端口只是两个或多个交换机之间的导管，因此也无法真正用作正常的网络接口；在这种模型下，只有下游接口或最顶层的上行接口才有意义)

NB: for the past 15 years, the DSA subsystem had been making use of the terms
"master" (rather than "conduit") and "slave" (rather than "user"). These terms
have been removed from the DSA codebase and phased out of the uAPI.
(注意：在过去 15 年里，DSA 子系统一直使用 "master" (而非 "conduit") 和 "slave" (而非 "user") 这两个术语。这些术语已从 DSA 代码库中移除，并已从用户态 API 中逐步淘汰。)

Switch tagging protocols
(交换机标签协议)
------------------------

DSA supports many vendor-specific tagging protocols, one software-defined
tagging protocol, and a tag-less mode as well (``DSA_TAG_PROTO_NONE``).
(DSA 支持许多厂商特有的标签协议、一种软件定义的标签协议，以及一种无标签模式 (``DSA_TAG_PROTO_NONE``)。)

The exact format of the tag protocol is vendor specific, but in general, they
all contain something which:
(标签协议的确切格式因厂商而异，但一般而言，它们都包含如下内容：)

- identifies which port the Ethernet frame came from/should be sent to
  (标识以太网帧来自哪个端口/应发送到哪个端口)
- provides a reason why this frame was forwarded to the management interface
  (提供这个帧被转发到管理接口的原因)

All tagging protocols are in ``net/dsa/tag_*.c`` files and implement the
methods of the ``struct dsa_device_ops`` structure, which are detailed below.
(所有标签协议都位于 ``net/dsa/tag_*.c`` 文件中，并实现 ``struct dsa_device_ops`` 结构体的方法，详见下文。)

Tagging protocols generally fall in one of three categories:
(标签协议通常分为以下三类：)

1. The switch-specific frame header is located before the Ethernet header,
   shifting to the right (from the perspective of the DSA conduit's frame
   parser) the MAC DA, MAC SA, EtherType and the entire L2 payload.
   (交换机特有的帧头位于以太网头之前，从 DSA conduit 的帧解析器的角度看，它使 MAC DA、MAC SA、EtherType 和整个 L2 载荷向右移动。)
2. The switch-specific frame header is located before the EtherType, keeping
   the MAC DA and MAC SA in place from the DSA conduit's perspective, but
   shifting the 'real' EtherType and L2 payload to the right.
   (交换机特有的帧头位于 EtherType 之前，从 DSA conduit 的角度看 MAC DA 和 MAC SA 保持原位，但把"真正的" EtherType 和 L2 载荷向右移动。)
3. The switch-specific frame header is located at the tail of the packet,
   keeping all frame headers in place and not altering the view of the packet
   that the DSA conduit's frame parser has.
   (交换机特有的帧头位于数据包尾部，保持所有帧头原位不动，不改变 DSA conduit 的帧解析器所看到的数据包视图。)

A tagging protocol may tag all packets with switch tags of the same length, or
the tag length might vary (for example packets with PTP timestamps might
require an extended switch tag, or there might be one tag length on TX and a
different one on RX). Either way, the tagging protocol driver must populate the
``struct dsa_device_ops::needed_headroom`` and/or ``struct dsa_device_ops::needed_tailroom``
with the length in octets of the longest switch frame header/trailer. The DSA
framework will automatically adjust the MTU of the conduit interface to
accommodate for this extra size in order for DSA user ports to support the
standard MTU (L2 payload length) of 1500 octets. The ``needed_headroom`` and
``needed_tailroom`` properties are also used to request from the network stack,
on a best-effort basis, the allocation of packets with enough extra space such
that the act of pushing the switch tag on transmission of a packet does not
cause it to reallocate due to lack of memory.
(标签协议可以用相同长度的交换机标签标记所有数据包，标签长度也可能变化 (例如带 PTP 时间戳的数据包可能需要扩展的交换机标签，或者 TX 使用一种标签长度而 RX 使用另一种)。无论哪种情况，标签协议驱动都必须用最长的交换机帧头/帧尾的字节长度来填充 ``struct dsa_device_ops::needed_headroom`` 和/或 ``struct dsa_device_ops::needed_tailroom`` 。DSA 框架会自动调整 conduit 接口的 MTU 以容纳这一额外大小，使 DSA 用户端口能够支持 1500 字节的标准 MTU (L2 载荷长度)。 ``needed_headroom`` 和 ``needed_tailroom`` 属性还用于以尽力而为的方式向网络栈请求分配带有足够额外空间的数据包，使得在发送数据包时推入交换机标签的动作不会因内存不足而导致重新分配。)

Even though applications are not expected to parse DSA-specific frame headers,
the format on the wire of the tagging protocol represents an Application Binary
Interface exposed by the kernel towards user space, for decoders such as
``libpcap``. The tagging protocol driver must populate the ``proto`` member of
``struct dsa_device_ops`` with a value that uniquely describes the
characteristics of the interaction required between the switch hardware and the
data path driver: the offset of each bit field within the frame header and any
stateful processing required to deal with the frames (as may be required for
PTP timestamping).
(尽管应用程序并不需要解析 DSA 特有的帧头，但标签协议在线路上的格式代表着内核向用户态暴露的应用程序二进制接口 (ABI)，供 ``libpcap`` 之类的解码器使用。标签协议驱动必须用唯一描述交换机硬件与数据路径驱动之间所需交互特性的值来填充 ``struct dsa_device_ops`` 的 ``proto`` 成员：帧头内每个位域的偏移量，以及处理帧所需的任何有状态处理 (如 PTP 时间戳可能要求的)。)

From the perspective of the network stack, all switches within the same DSA
switch tree use the same tagging protocol. In case of a packet transiting a
fabric with more than one switch, the switch-specific frame header is inserted
by the first switch in the fabric that the packet was received on. This header
typically contains information regarding its type (whether it is a control
frame that must be trapped to the CPU, or a data frame to be forwarded).
Control frames should be decapsulated only by the software data path, whereas
data frames might also be autonomously forwarded towards other user ports of
other switches from the same fabric, and in this case, the outermost switch
ports must decapsulate the packet.
(从网络栈的角度看，同一 DSA 交换机树内的所有交换机都使用相同的标签协议。当数据包经过包含多个交换机的 fabric 时，交换机特有的帧头由该数据包在 fabric 中到达的第一个交换机插入。这个帧头通常包含关于其类型的信息 (它是必须被陷阱到 CPU 的控制帧，还是要被转发的数据帧)。控制帧只应由软件数据路径解封装，而数据帧也可以被自主转发到同一 fabric 中其他交换机的其他用户端口，在这种情况下，最外侧的交换机端口必须解封装该数据包。)

Note that in certain cases, it might be the case that the tagging format used
by a leaf switch (not connected directly to the CPU) is not the same as what
the network stack sees. This can be seen with Marvell switch trees, where the
CPU port can be configured to use either the DSA or the Ethertype DSA (EDSA)
format, but the DSA links are configured to use the shorter (without Ethertype)
DSA frame header, in order to reduce the autonomous packet forwarding overhead.
It still remains the case that, if the DSA switch tree is configured for the
EDSA tagging protocol, the operating system sees EDSA-tagged packets from the
leaf switches that tagged them with the shorter DSA header. This can be done
because the Marvell switch connected directly to the CPU is configured to
perform tag translation between DSA and EDSA (which is simply the operation of
adding or removing the ``ETH_P_EDSA`` EtherType and some padding octets).
(注意，在某些情况下，叶子交换机 (未直接连接到 CPU) 使用的标签格式可能与网络栈所看到的不一致。Marvell 交换机树就是一个例子：CPU 端口可以配置为使用 DSA 或 Ethertype DSA (EDSA) 格式，但 DSA 链路被配置为使用更短的 (不含 Ethertype 的) DSA 帧头，以减少自主数据包转发的开销。即便如此，如果 DSA 交换机树被配置为 EDSA 标签协议，操作系统看到的仍是来自叶子交换机的 EDSA 标签数据包，尽管这些叶子交换机是用更短的 DSA 帧头打标签的。之所以能这样做，是因为直接连接到 CPU 的 Marvell 交换机被配置为在 DSA 与 EDSA 之间执行标签转换 (这只是添加或移除 ``ETH_P_EDSA`` EtherType 和一些填充字节的操作)。)

It is possible to construct cascaded setups of DSA switches even if their
tagging protocols are not compatible with one another. In this case, there are
no DSA links in this fabric, and each switch constitutes a disjoint DSA switch
tree. The DSA links are viewed as simply a pair of a DSA conduit (the out-facing
port of the upstream DSA switch) and a CPU port (the in-facing port of the
downstream DSA switch).
(即使各 DSA 交换机的标签协议彼此不兼容，也可以构建级联的 DSA 交换机设置。在这种情况下，该 fabric 中不存在 DSA 链路，每个交换机各自构成一个互不相连的 DSA 交换机树。DSA 链路被简单地视为一对 DSA conduit (上游 DSA 交换机的对外端口) 和 CPU 端口 (下游 DSA 交换机的对内端口)。)

The tagging protocol of the attached DSA switch tree can be viewed through the
``dsa/tagging`` sysfs attribute of the DSA conduit::
(附加的 DSA 交换机树的标签协议可以通过 DSA conduit 的 ``dsa/tagging`` sysfs 属性查看：)

    cat /sys/class/net/eth0/dsa/tagging

If the hardware and driver are capable, the tagging protocol of the DSA switch
tree can be changed at runtime. This is done by writing the new tagging
protocol name to the same sysfs device attribute as above (the DSA conduit and
all attached switch ports must be down while doing this).
(如果硬件和驱动支持，DSA 交换机树的标签协议可以在运行时更改。做法是把新的标签协议名称写入与上面相同的 sysfs 设备属性 (执行此操作时 DSA conduit 和所有已连接的交换机端口必须处于 down 状态)。)

It is desirable that all tagging protocols are testable with the ``dsa_loop``
mockup driver, which can be attached to any network interface. The goal is that
any network interface should be capable of transmitting the same packet in the
same way, and the tagger should decode the same received packet in the same way
regardless of the driver used for the switch control path, and the driver used
for the DSA conduit.
(理想情况下，所有标签协议都应能用 ``dsa_loop`` 模拟驱动进行测试，该驱动可以附加到任何网络接口。目标是：无论交换机控制路径使用何种驱动、DSA conduit 使用何种驱动，任何网络接口都应能以相同的方式发送相同的数据包，并且标签器应以相同的方式解码相同的接收数据包。)

The transmission of a packet goes through the tagger's ``xmit`` function.
The passed ``struct sk_buff *skb`` has ``skb->data`` pointing at
``skb_mac_header(skb)``, i.e. at the destination MAC address, and the passed
``struct net_device *dev`` represents the virtual DSA user network interface
whose hardware counterpart the packet must be steered to (i.e. ``swp0``).
The job of this method is to prepare the skb in a way that the switch will
understand what egress port the packet is for (and not deliver it towards other
ports). Typically this is fulfilled by pushing a frame header. Checking for
insufficient size in the skb headroom or tailroom is unnecessary provided that
the ``needed_headroom`` and ``needed_tailroom`` properties were filled out
properly, because DSA ensures there is enough space before calling this method.
(数据包的发送要经过标签器的 ``xmit`` 函数。传入的 ``struct sk_buff *skb`` 的 ``skb->data`` 指向 ``skb_mac_header(skb)``，即目标 MAC 地址；传入的 ``struct net_device *dev`` 表示虚拟的 DSA 用户网络接口，数据包必须被导向其对应的硬件实体 (即 ``swp0``)。此方法的职责是准备好 skb，使交换机能理解这个数据包针对哪个出端口 (而不是把它投递到其他端口)。这通常通过推入一个帧头来完成。只要 ``needed_headroom`` 和 ``needed_tailroom`` 属性被正确填写，就无需检查 skb 的 headroom 或 tailroom 是否不足，因为 DSA 在调用此方法之前会确保有足够的空间。)

The reception of a packet goes through the tagger's ``rcv`` function. The
passed ``struct sk_buff *skb`` has ``skb->data`` pointing at
``skb_mac_header(skb) + ETH_ALEN`` octets, i.e. to where the first octet after
the EtherType would have been, were this frame not tagged. The role of this
method is to consume the frame header, adjust ``skb->data`` to really point at
the first octet after the EtherType, and to change ``skb->dev`` to point to the
virtual DSA user network interface corresponding to the physical front-facing
switch port that the packet was received on.
(数据包的接收要经过标签器的 ``rcv`` 函数。传入的 ``struct sk_buff *skb`` 的 ``skb->data`` 指向 ``skb_mac_header(skb) + ETH_ALEN`` 字节处，即如果这个帧没有被打标签，EtherType 之后第一个字节所在的位置。此方法的职责是消费掉帧头，把 ``skb->data`` 调整为真正指向 EtherType 之后的第一个字节，并把 ``skb->dev`` 改为指向与收到该数据包的物理前面板交换机端口相对应的虚拟 DSA 用户网络接口。)

Since tagging protocols in category 1 and 2 break software (and most often also
hardware) packet dissection on the DSA conduit, features such as RPS (Receive
Packet Steering) on the DSA conduit would be broken. The DSA framework deals
with this by hooking into the flow dissector and shifting the offset at which
the IP header is to be found in the tagged frame as seen by the DSA conduit.
This behavior is automatic based on the ``overhead`` value of the tagging
protocol. If not all packets are of equal size, the tagger can implement the
``flow_dissect`` method of the ``struct dsa_device_ops`` and override this
default behavior by specifying the correct offset incurred by each individual
RX packet. Tail taggers do not cause issues to the flow dissector.
(由于第 1 类和第 2 类标签协议会破坏 DSA conduit 上的软件 (通常还有硬件) 数据包解析，DSA conduit 上的 RPS (Receive Packet Steering) 等特性也会因此失效。DSA 框架通过挂接流解析器 (flow dissector) 来解决这个问题：移动 DSA conduit 所看到的已打标签帧中 IP 头的偏移位置。该行为基于标签协议的 ``overhead`` 值自动完成。如果并非所有数据包大小相同，标签器可以实现 ``struct dsa_device_ops`` 的 ``flow_dissect`` 方法，通过为每个 RX 数据包指定正确的偏移量来覆盖这一默认行为。尾部标签器不会给流解析器带来问题。)

Checksum offload should work with category 1 and 2 taggers when the DSA conduit
driver declares NETIF_F_HW_CSUM in vlan_features and looks at csum_start and
csum_offset. For those cases, DSA will shift the checksum start and offset by
the tag size. If the DSA conduit driver still uses the legacy NETIF_F_IP_CSUM
or NETIF_F_IPV6_CSUM in vlan_features, the offload might only work if the
offload hardware already expects that specific tag (perhaps due to matching
vendors). DSA user ports inherit those flags from the conduit, and it is up to
the driver to correctly fall back to software checksum when the IP header is not
where the hardware expects. If that check is ineffective, the packets might go
to the network without a proper checksum (the checksum field will have the
pseudo IP header sum). For category 3, when the offload hardware does not
already expect the switch tag in use, the checksum must be calculated before any
tag is inserted (i.e. inside the tagger). Otherwise, the DSA conduit would
include the tail tag in the (software or hardware) checksum calculation. Then,
when the tag gets stripped by the switch during transmission, it will leave an
incorrect IP checksum in place.
(当 DSA conduit 驱动在 vlan_features 中声明了 NETIF_F_HW_CSUM 并使用 csum_start 和 csum_offset 时，校验和卸载应当能与第 1 类和第 2 类标签器配合工作。对于这些情况，DSA 会按标签大小平移校验和的起始位置与偏移量。如果 DSA conduit 驱动在 vlan_features 中仍使用传统的 NETIF_F_IP_CSUM 或 NETIF_F_IPV6_CSUM，那么只有当卸载硬件本身已经预期那种特定标签时 (可能是由于厂商相互匹配)，卸载才可能工作。DSA 用户端口从 conduit 继承这些标志，当 IP 头不在硬件预期的位置时，驱动需要自行正确回退到软件校验和。如果该检查无效，数据包可能会在没有正确校验和的情况下发往网络 (校验和字段中将是伪 IP 头的和)。对于第 3 类，当卸载硬件尚未预期所使用的交换机标签时，必须在插入任何标签之前计算校验和 (即在标签器内部)。否则 DSA conduit 会把尾部标签计入 (软件或硬件) 校验和计算，随后当交换机在传输过程中剥离该标签时，就会留下一个不正确的 IP 校验和。)

Due to various reasons (most common being category 1 taggers being associated
with DSA-unaware conduits, mangling what the conduit perceives as MAC DA), the
tagging protocol may require the DSA conduit to operate in promiscuous mode, to
receive all frames regardless of the value of the MAC DA. This can be done by
setting the ``promisc_on_conduit`` property of the ``struct dsa_device_ops``.
Note that this assumes a DSA-unaware conduit driver, which is the norm.
(由于各种原因 (最常见的是第 1 类标签器与不识别 DSA 的 conduit 相关联，破坏了 conduit 所认为的 MAC DA)，标签协议可能要求 DSA conduit 工作在混杂模式下，以接收所有帧而不管 MAC DA 的值是什么。这可以通过设置 ``struct dsa_device_ops`` 的 ``promisc_on_conduit`` 属性来完成。注意，这假设 conduit 驱动不识别 DSA，而这正是常态。)

Conduit network devices
(Conduit 网络设备)
-----------------------

Conduit network devices are regular, unmodified Linux network device drivers for
the CPU/management Ethernet interface. Such a driver might occasionally need to
know whether DSA is enabled (e.g.: to enable/disable specific offload features),
but the DSA subsystem has been proven to work with industry standard drivers:
``e1000e,`` ``mv643xx_eth`` etc. without having to introduce modifications to these
drivers. Such network devices are also often referred to as conduit network
devices since they act as a pipe between the host processor and the hardware
Ethernet switch.
(Conduit 网络设备是针对 CPU/管理以太网接口的常规、未修改的 Linux 网络设备驱动。这类驱动偶尔可能需要知道 DSA 是否已启用 (例如用于启用/禁用特定的卸载特性)，但 DSA 子系统已被证明可以与工业标准驱动 (如 ``e1000e,`` ``mv643xx_eth`` 等) 配合工作，而无需对这些驱动做任何修改。这类网络设备也常被称为 conduit 网络设备，因为它们充当主机处理器与硬件以太网交换机之间的管道。)

Networking stack hooks
(网络栈挂接点)
----------------------

When a conduit netdev is used with DSA, a small hook is placed in the
networking stack is in order to have the DSA subsystem process the Ethernet
switch specific tagging protocol. DSA accomplishes this by registering a
specific (and fake) Ethernet type (later becoming ``skb->protocol``) with the
networking stack, this is also known as a ``ptype`` or ``packet_type``. A typical
Ethernet Frame receive sequence looks like this:
(当 conduit netdev 与 DSA 一起使用时，会在网络栈中放置一个小的挂接点 (hook)，让 DSA 子系统处理以太网交换机特有的标签协议。DSA 的做法是向网络栈注册一个特定 (且伪造) 的以太网类型 (之后成为 ``skb->protocol``)，这也被称为 ``ptype`` 或 ``packet_type``。典型的以太网帧接收序列如下所示：)

Conduit network device (e.g.: e1000e):
(Conduit 网络设备 (例如 e1000e)：)

1. Receive interrupt fires:
   (1. 接收中断触发：)

        - receive function is invoked
          (接收函数被调用)
        - basic packet processing is done: getting length, status etc.
          (完成基本的数据包处理：获取长度、状态等)
        - packet is prepared to be processed by the Ethernet layer by calling
          ``eth_type_trans``
          (通过调用 ``eth_type_trans`` 准备好数据包，交由以太网层处理)

2. net/ethernet/eth.c::
   (2. net/ethernet/eth.c：)

          eth_type_trans(skb, dev)
                  if (dev->dsa_ptr != NULL)
                          -> skb->protocol = ETH_P_XDSA

3. drivers/net/ethernet/\*::
   (3. drivers/net/ethernet/\*：)

          netif_receive_skb(skb)
                  -> iterate over registered packet_type
                          -> invoke handler for ETH_P_XDSA, calls dsa_switch_rcv()

4. net/dsa/dsa.c::
   (4. net/dsa/dsa.c：)

          -> dsa_switch_rcv()
                  -> invoke switch tag specific protocol handler in 'net/dsa/tag_*.c'

5. net/dsa/tag_*.c:
   (5. net/dsa/tag_*.c：)

        - inspect and strip switch tag protocol to determine originating port
          (检查并剥离交换机标签协议，以确定来源端口)
        - locate per-port network device
          (定位每端口对应的网络设备)
        - invoke ``eth_type_trans()`` with the DSA user network device
          (以 DSA 用户网络设备为参数调用 ``eth_type_trans()``)
        - invoked ``netif_receive_skb()``
          (调用 ``netif_receive_skb()``)

Past this point, the DSA user network devices get delivered regular Ethernet
frames that can be processed by the networking stack.
(过了这一步之后，DSA 用户网络设备收到的就是可供网络栈处理的普通以太网帧了。)

User network devices
(用户网络设备)
--------------------

User network devices created by DSA are stacked on top of their conduit network
device, each of these network interfaces will be responsible for being a
controlling and data-flowing end-point for each front-panel port of the switch.
These interfaces are specialized in order to:
(由 DSA 创建的用户网络设备叠加在其 conduit 网络设备之上，其中每个网络接口都负责作为交换机每个前面板端口的控制和数据流端点。这些接口经过专门定制，以便：)

- insert/remove the switch tag protocol (if it exists) when sending traffic
  to/from specific switch ports
  (在向/从特定交换机端口发送流量时插入/移除交换机标签协议 (如果存在))
- query the switch for ethtool operations: statistics, link state,
  Wake-on-LAN, register dumps...
  (为 ethtool 操作查询交换机：统计信息、链路状态、Wake-on-LAN、寄存器转储等)
- manage external/internal PHY: link, auto-negotiation, etc.
  (管理外部/内部 PHY：链路、自动协商等)

These user network devices have custom net_device_ops and ethtool_ops function
pointers which allow DSA to introduce a level of layering between the networking
stack/ethtool and the switch driver implementation.
(这些用户网络设备拥有自定义的 net_device_ops 和 ethtool_ops 函数指针，使 DSA 能够在网络栈/ethtool 与交换机驱动实现之间引入一层分层。)

Upon frame transmission from these user network devices, DSA will look up which
switch tagging protocol is currently registered with these network devices and
invoke a specific transmit routine which takes care of adding the relevant
switch tag in the Ethernet frames.
(当帧从这些用户网络设备发出时，DSA 会查找这些网络设备当前注册的是哪个交换机标签协议，并调用特定的发送例程，负责在以太网帧中添加相应的交换机标签。)

These frames are then queued for transmission using the conduit network device
``ndo_start_xmit()`` function. Since they contain the appropriate switch tag, the
Ethernet switch will be able to process these incoming frames from the
management interface and deliver them to the physical switch port.
(这些帧随后通过 conduit 网络设备的 ``ndo_start_xmit()`` 函数排队发送。由于它们包含适当的交换机标签，以太网交换机将能够从管理接口处理这些到来的帧，并把它们投递到物理交换机端口。)

When using multiple CPU ports, it is possible to stack a LAG (bonding/team)
device between the DSA user devices and the physical DSA conduits. The LAG
device is thus also a DSA conduit, but the LAG slave devices continue to be DSA
conduits as well (just with no user port assigned to them; this is needed for
recovery in case the LAG DSA conduit disappears). Thus, the data path of the LAG
DSA conduit is used asymmetrically. On RX, the ``ETH_P_XDSA`` handler, which
calls ``dsa_switch_rcv()``, is invoked early (on the physical DSA conduit;
LAG slave). Therefore, the RX data path of the LAG DSA conduit is not used.
On the other hand, TX takes place linearly: ``dsa_user_xmit`` calls
``dsa_enqueue_skb``, which calls ``dev_queue_xmit`` towards the LAG DSA conduit.
The latter calls ``dev_queue_xmit`` towards one physical DSA conduit or the
other, and in both cases, the packet exits the system through a hardware path
towards the switch.
(当使用多个 CPU 端口时，可以在 DSA 用户设备与物理 DSA conduit 之间叠加一个 LAG (bonding/team) 设备。这样，LAG 设备本身也是一个 DSA conduit，但 LAG 的 slave 设备仍然继续作为 DSA conduit (只是没有分配用户端口；这是为了在 LAG DSA conduit 消失时进行恢复所需)。因此，LAG DSA conduit 的数据路径是不对称使用的。在 RX 方向，调用 ``dsa_switch_rcv()`` 的 ``ETH_P_XDSA`` 处理器会被提前调用 (在物理 DSA conduit，即 LAG slave 上)。因此，LAG DSA conduit 的 RX 数据路径不会被使用。而在 TX 方向，处理是线性进行的： ``dsa_user_xmit`` 调用 ``dsa_enqueue_skb`` ，后者朝 LAG DSA conduit 调用 ``dev_queue_xmit`` 。LAG conduit 再向其中一个物理 DSA conduit 调用 ``dev_queue_xmit`` ，无论哪种情况，数据包都通过硬件路径离开系统、前往交换机。)

Graphical representation
(图形化表示)
------------------------

Summarized, this is basically how DSA looks like from a network device
perspective::
(概括地说，从网络设备的角度来看，DSA 大致如下图所示：)

                Unaware application
              opens and binds socket
                       |  ^
                       |  |
           +-----------v--|--------------------+
           |+------+ +------+ +------+ +------+|
           || swp0 | | swp1 | | swp2 | | swp3 ||
           |+------+-+------+-+------+-+------+|
           |          DSA switch driver        |
           +-----------------------------------+
                         |        ^
            Tag added by |        | Tag consumed by
           switch driver |        | switch driver
                         v        |
           +-----------------------------------+
           | Unmodified host interface driver  | Software
   --------+-----------------------------------+------------
           |       Host interface (eth0)       | Hardware
           +-----------------------------------+
                         |        ^
         Tag consumed by |        | Tag added by
         switch hardware |        | switch hardware
                         v        |
           +-----------------------------------+
           |               Switch              |
           |+------+ +------+ +------+ +------+|
           || swp0 | | swp1 | | swp2 | | swp3 ||
           ++------+-+------+-+------+-+------++

User MDIO bus
(用户 MDIO 总线)
-------------

In order to be able to read to/from a switch PHY built into it, DSA creates an
user MDIO bus which allows a specific switch driver to divert and intercept
MDIO reads/writes towards specific PHY addresses. In most MDIO-connected
switches, these functions would utilize direct or indirect PHY addressing mode
to return standard MII registers from the switch builtin PHYs, allowing the PHY
library and/or to return link status, link partner pages, auto-negotiation
results, etc.
(为了能够读/写交换机内建的 PHY，DSA 会创建一条用户 MDIO 总线，允许特定的交换机驱动程序改道并拦截发往特定 PHY 地址的 MDIO 读/写操作。在大多数通过 MDIO 连接的交换机中，这些函数会利用直接或间接 PHY 寻址模式，从交换机内建 PHY 返回标准 MII 寄存器，使 PHY 库能够 (并/或) 返回链路状态、链路伙伴页、自动协商结果等。)

For Ethernet switches which have both external and internal MDIO buses, the
user MII bus can be utilized to mux/demux MDIO reads and writes towards either
internal or external MDIO devices this switch might be connected to: internal
PHYs, external PHYs, or even external switches.
(对于同时具有外部和内部 MDIO 总线的以太网交换机，可以利用用户 MII 总线对 MDIO 读/写进行复用/解复用，将其导向该交换机可能连接的内部或外部 MDIO 设备：内部 PHY、外部 PHY，甚至外部交换机。)

Data structures
(数据结构)
---------------

DSA data structures are defined in ``include/net/dsa.h`` as well as
``net/dsa/dsa_priv.h``:
(DSA 的数据结构定义在 ``include/net/dsa.h`` 以及 ``net/dsa/dsa_priv.h`` 中：)

- ``dsa_chip_data``: platform data configuration for a given switch device,
  this structure describes a switch device's parent device, its address, as
  well as various properties of its ports: names/labels, and finally a routing
  table indication (when cascading switches)
  ( ``dsa_chip_data``：给定交换机设备的平台数据配置。该结构体描述交换机设备的父设备、其地址，以及其端口的各项属性：名称/标签，最后还有路由表指示 (在级联交换机时))

- ``dsa_switch_tree``: structure assigned to the conduit network device under
  ``dsa_ptr``, this structure references a dsa_platform_data structure as well as
  the tagging protocol supported by the switch tree, and which receive/transmit
  function hooks should be invoked, information about the directly attached
  switch is also provided: CPU port. Finally, a collection of dsa_switch are
  referenced to address individual switches in the tree.
  ( ``dsa_switch_tree``：赋给 conduit 网络设备、存放在 ``dsa_ptr`` 下的结构体。该结构体引用一个 dsa_platform_data 结构体，以及交换机树支持的标签协议和应调用的接收/发送函数挂接点；也提供了关于直接连接的交换机的信息：CPU 端口。最后，它引用一组 dsa_switch，用于寻址树中的各个交换机。)

- ``dsa_switch``: structure describing a switch device in the tree, referencing
  a ``dsa_switch_tree`` as a backpointer, user network devices, conduit network
  device, and a reference to the backing``dsa_switch_ops``
  ( ``dsa_switch``：描述树中一个交换机设备的结构体，引用 ``dsa_switch_tree`` 作为反向指针，还包含用户网络设备、conduit 网络设备，以及对底层 ``dsa_switch_ops`` 的引用)

- ``dsa_switch_ops``: structure referencing function pointers, see below for a
  full description.
  ( ``dsa_switch_ops``：引用函数指针的结构体，完整说明见下文。)

Design limitations
(设计限制)
==================

Lack of CPU/DSA network devices
(缺少 CPU/DSA 网络设备)
-------------------------------

DSA does not currently create user network devices for the CPU or DSA ports, as
described before. This might be an issue in the following cases:
(如前所述，DSA 目前不为 CPU 或 DSA 端口创建用户网络设备。这在以下情况下可能成为问题：)

- inability to fetch switch CPU port statistics counters using ethtool, which
  can make it harder to debug MDIO switch connected using xMII interfaces
  (无法使用 ethtool 获取交换机 CPU 端口的统计计数器，这会使调试通过 xMII 接口连接的 MDIO 交换机变得更加困难)

- inability to configure the CPU port link parameters based on the Ethernet
  controller capabilities attached to it: http://patchwork.ozlabs.org/patch/509806/
  (无法根据所连接以太网控制器的能力配置 CPU 端口链路参数： http://patchwork.ozlabs.org/patch/509806/)

- inability to configure specific VLAN IDs / trunking VLANs between switches
  when using a cascaded setup
  (使用级联设置时，无法在交换机之间配置特定的 VLAN ID / trunking VLAN)

Common pitfalls using DSA setups
(使用 DSA 设置的常见陷阱)
--------------------------------

Once a conduit network device is configured to use DSA (dev->dsa_ptr becomes
non-NULL), and the switch behind it expects a tagging protocol, this network
interface can only exclusively be used as a conduit interface. Sending packets
directly through this interface (e.g.: opening a socket using this interface)
will not make us go through the switch tagging protocol transmit function, so
the Ethernet switch on the other end, expecting a tag will typically drop this
frame.
(一旦 conduit 网络设备被配置为使用 DSA (dev->dsa_ptr 变为非 NULL)，且其背后的交换机要求某种标签协议，这个网络接口就只能被独占用作 conduit 接口。直接通过该接口发送数据包 (例如用该接口打开套接字) 不会经过交换机标签协议的发送函数，因此另一端期望标签的以太网交换机通常会丢弃这样的帧。)

Interactions with other subsystems
(与其他子系统的交互)
==================================

DSA currently leverages the following subsystems:
(DSA 目前利用了以下子系统：)

- MDIO/PHY library: ``drivers/net/phy/phy.c``, ``mdio_bus.c``
  (MDIO/PHY 库： ``drivers/net/phy/phy.c`` 、 ``mdio_bus.c``)
- Switchdev:``net/switchdev/*``
  (Switchdev： ``net/switchdev/*``)
- Device Tree for various of_* functions
  (设备树 (Device Tree)，用于各种 of_* 函数)
- Devlink: ``net/core/devlink.c``
  (Devlink： ``net/core/devlink.c``)

MDIO/PHY library
(MDIO/PHY 库)
----------------

User network devices exposed by DSA may or may not be interfacing with PHY
devices (``struct phy_device`` as defined in ``include/linux/phy.h)``, but the DSA
subsystem deals with all possible combinations:
(由 DSA 暴露的用户网络设备可能与 PHY 设备 (``include/linux/phy.h`` 中定义的 ``struct phy_device``) 相接，也可能不相接，但 DSA 子系统会处理所有可能的组合：)

- internal PHY devices, built into the Ethernet switch hardware
  (内建于以太网交换机硬件中的内部 PHY 设备)
- external PHY devices, connected via an internal or external MDIO bus
  (通过内部或外部 MDIO 总线连接的外部 PHY 设备)
- internal PHY devices, connected via an internal MDIO bus
  (通过内部 MDIO 总线连接的内部 PHY 设备)
- special, non-autonegotiated or non MDIO-managed PHY devices: SFPs, MoCA; a.k.a
  fixed PHYs
  (特殊的、非自动协商或非 MDIO 管理的 PHY 设备：SFP、MoCA，即所谓的 fixed PHY)

The PHY configuration is done by the ``dsa_user_phy_setup()`` function and the
logic basically looks like this:
(PHY 配置由 ``dsa_user_phy_setup()`` 函数完成，其逻辑大致如下：)

- if Device Tree is used, the PHY device is looked up using the standard
  "phy-handle" property, if found, this PHY device is created and registered
  using ``of_phy_connect()``
  (如果使用设备树，则使用标准的 "phy-handle" 属性查找 PHY 设备；如果找到，则使用 ``of_phy_connect()`` 创建并注册该 PHY 设备)

- if Device Tree is used and the PHY device is "fixed", that is, conforms to
  the definition of a non-MDIO managed PHY as defined in
  ``Documentation/devicetree/bindings/net/fixed-link.txt``, the PHY is registered
  and connected transparently using the special fixed MDIO bus driver
  (如果使用设备树且 PHY 设备是 "fixed" 的，即符合 ``Documentation/devicetree/bindings/net/fixed-link.txt`` 中定义的非 MDIO 管理 PHY，则会使用特殊的 fixed MDIO 总线驱动透明地注册并连接该 PHY)

- finally, if the PHY is built into the switch, as is very common with
  standalone switch packages, the PHY is probed using the user MII bus created
  by DSA
  (最后，如果 PHY 内建于交换机中 (这在独立交换机芯片中非常常见)，则使用 DSA 创建的用户 MII 总线来探测该 PHY)


SWITCHDEV
(SWITCHDEV)
---------

DSA directly utilizes SWITCHDEV when interfacing with the bridge layer, and
more specifically with its VLAN filtering portion when configuring VLANs on top
of per-port user network devices. As of today, the only SWITCHDEV objects
supported by DSA are the FDB and VLAN objects.
(在与网桥层交互时——尤其是在基于每端口用户网络设备配置 VLAN 时与其 VLAN 过滤部分交互——DSA 直接使用 SWITCHDEV。迄今为止，DSA 支持的 SWITCHDEV 对象只有 FDB 和 VLAN 对象。)

Devlink
(Devlink)
-------

DSA registers one devlink device per physical switch in the fabric.
For each devlink device, every physical port (i.e. user ports, CPU ports, DSA
links or unused ports) is exposed as a devlink port.
(DSA 为 fabric 中的每个物理交换机注册一个 devlink 设备。对于每个 devlink 设备，每个物理端口 (即用户端口、CPU 端口、DSA 链路或未使用端口) 都会作为一个 devlink 端口暴露出来。)

DSA drivers can make use of the following devlink features:
(DSA 驱动可以使用以下 devlink 特性：)

- Regions: debugging feature which allows user space to dump driver-defined
  areas of hardware information in a low-level, binary format. Both global
  regions as well as per-port regions are supported. It is possible to export
  devlink regions even for pieces of data that are already exposed in some way
  to the standard iproute2 user space programs (ip-link, bridge), like address
  tables and VLAN tables. For example, this might be useful if the tables
  contain additional hardware-specific details which are not visible through
  the iproute2 abstraction, or it might be useful to inspect these tables on
  the non-user ports too, which are invisible to iproute2 because no network
  interface is registered for them.
  (Regions (区域)：一种调试特性，允许用户态以低层二进制格式转储驱动程序定义的硬件信息区域。全局区域和每端口区域都受支持。即使某些数据已经以某种方式暴露给标准 iproute2 用户态程序 (ip-link、bridge)，例如地址表和 VLAN 表，也可以为它们导出 devlink region。例如，当这些表包含通过 iproute2 抽象不可见的额外硬件特定细节时，这可能会很有用；或者需要在非用户端口上也检查这些表时——由于没有为它们注册网络接口，iproute2 看不到这些端口。)
- Params: a feature which enables user to configure certain low-level tunable
  knobs pertaining to the device. Drivers may implement applicable generic
  devlink params, or may add new device-specific devlink params.
  (Params (参数)：一种让用户能够配置与设备相关的某些低层可调选项的特性。驱动程序可以实现适用的通用 devlink 参数，也可以添加新的设备特定 devlink 参数。)
- Resources: a monitoring feature which enables users to see the degree of
  utilization of certain hardware tables in the device, such as FDB, VLAN, etc.
  (Resources (资源)：一种监视特性，让用户能够查看设备中某些硬件表 (如 FDB、VLAN 等) 的利用程度。)
- Shared buffers: a QoS feature for adjusting and partitioning memory and frame
  reservations per port and per traffic class, in the ingress and egress
  directions, such that low-priority bulk traffic does not impede the
  processing of high-priority critical traffic.
  (Shared buffers (共享缓冲区)：一种 QoS 特性，用于在入口和出口方向按端口和按流量类别调整和划分内存与帧预留，使低优先级的批量流量不会妨碍高优先级关键流量的处理。)

For more details, consult ``Documentation/networking/devlink/``.
(更多细节请查阅 ``Documentation/networking/devlink/`` 。)

Device Tree
(设备树)
-----------

DSA features a standardized binding which is documented in
``Documentation/devicetree/bindings/net/dsa/dsa.txt``. PHY/MDIO library helper
functions such as ``of_get_phy_mode()``, ``of_phy_connect()`` are also used to query
per-port PHY specific details: interface connection, MDIO bus location, etc.
(DSA 提供一个标准化的绑定，记录在 ``Documentation/devicetree/bindings/net/dsa/dsa.txt`` 中。PHY/MDIO 库的辅助函数 (如 ``of_get_phy_mode()`` 、 ``of_phy_connect()``) 也被用于查询每端口的 PHY 特定细节：接口连接方式、MDIO 总线位置等。)

Driver development
(驱动开发)
==================

DSA switch drivers need to implement a ``dsa_switch_ops`` structure which will
contain the various members described below.
(DSA 交换机驱动需要实现一个 ``dsa_switch_ops`` 结构体，其中包含下文描述的各个成员。)

Probing, registration and device lifetime
(探测、注册与设备生命周期)
-----------------------------------------

DSA switches are regular ``device`` structures on buses (be they platform, SPI,
I2C, MDIO or otherwise). The DSA framework is not involved in their probing
with the device core.
(DSA 交换机是总线 (无论是 platform、SPI、I2C、MDIO 还是其他总线) 上常规的 ``device`` 结构体。DSA 框架不参与设备核心对它们的探测。)

Switch registration from the perspective of a driver means passing a valid
``struct dsa_switch`` pointer to ``dsa_register_switch()``, usually from the
switch driver's probing function. The following members must be valid in the
provided structure:
(从驱动的角度来看，注册交换机意味着把一个有效的 ``struct dsa_switch`` 指针传给 ``dsa_register_switch()`` ，通常是在交换机驱动的探测函数中。所提供的结构体中以下成员必须有效：)

- ``ds->dev``: will be used to parse the switch's OF node or platform data.
  ( ``ds->dev``：将用于解析交换机的 OF 节点或平台数据。)

- ``ds->num_ports``: will be used to create the port list for this switch, and
  to validate the port indices provided in the OF node.
  ( ``ds->num_ports``：将用于为该交换机创建端口列表，并验证 OF 节点中提供的端口索引。)

- ``ds->ops``: a pointer to the ``dsa_switch_ops`` structure holding the DSA
  method implementations.
  ( ``ds->ops``：指向 ``dsa_switch_ops`` 结构体的指针，其中包含 DSA 方法的实现。)

- ``ds->priv``: backpointer to a driver-private data structure which can be
  retrieved in all further DSA method callbacks.
  ( ``ds->priv``：指向驱动私有数据结构的反向指针，可在后续所有 DSA 方法回调中获取。)

In addition, the following flags in the ``dsa_switch`` structure may optionally
be configured to obtain driver-specific behavior from the DSA core. Their
behavior when set is documented through comments in ``include/net/dsa.h``.
(此外， ``dsa_switch`` 结构体中的以下标志可以选择性地配置，以便从 DSA 核心获得驱动特定的行为。设置后它们的行为通过 ``include/net/dsa.h`` 中的注释说明。)

- ``ds->vlan_filtering_is_global``
  ( ``ds->vlan_filtering_is_global``)

- ``ds->needs_standalone_vlan_filtering``
  ( ``ds->needs_standalone_vlan_filtering``)

- ``ds->configure_vlan_while_not_filtering``
  ( ``ds->configure_vlan_while_not_filtering``)

- ``ds->untag_bridge_pvid``
  ( ``ds->untag_bridge_pvid``)

- ``ds->assisted_learning_on_cpu_port``
  ( ``ds->assisted_learning_on_cpu_port``)

- ``ds->mtu_enforcement_ingress``
  ( ``ds->mtu_enforcement_ingress``)

- ``ds->fdb_isolation``
  ( ``ds->fdb_isolation``)

Internally, DSA keeps an array of switch trees (group of switches) global to
the kernel, and attaches a ``dsa_switch`` structure to a tree on registration.
The tree ID to which the switch is attached is determined by the first u32
number of the ``dsa,member`` property of the switch's OF node (0 if missing).
The switch ID within the tree is determined by the second u32 number of the
same OF property (0 if missing). Registering multiple switches with the same
switch ID and tree ID is illegal and will cause an error. Using platform data,
a single switch and a single switch tree is permitted.
(在内部，DSA 在内核全局维护一个交换机树 (交换机组) 数组，并在注册时把 ``dsa_switch`` 结构体附加到某棵树上。交换机所附加到的树 ID 由交换机 OF 节点的 ``dsa,member`` 属性的第一个 u32 数值决定 (若缺失则为 0)。树内的交换机 ID 由同一 OF 属性的第二个 u32 数值决定 (若缺失则为 0)。以相同的交换机 ID 和树 ID 注册多个交换机是非法的，会导致错误。使用平台数据时，允许使用单个交换机和单个交换机树。)

In case of a tree with multiple switches, probing takes place asymmetrically.
The first N-1 callers of ``dsa_register_switch()`` only add their ports to the
port list of the tree (``dst->ports``), each port having a backpointer to its
associated switch (``dp->ds``). Then, these switches exit their
``dsa_register_switch()`` call early, because ``dsa_tree_setup_routing_table()``
has determined that the tree is not yet complete (not all ports referenced by
DSA links are present in the tree's port list). The tree becomes complete when
the last switch calls ``dsa_register_switch()``, and this triggers the effective
continuation of initialization (including the call to ``ds->ops->setup()``) for
all switches within that tree, all as part of the calling context of the last
switch's probe function.
(对于包含多个交换机的树，探测是不对称进行的。前 N-1 个调用 ``dsa_register_switch()`` 的调用者只是把自己的端口加入树的端口列表 (``dst->ports``)，每个端口都带有指向其关联交换机的反向指针 (``dp->ds``)。随后，这些交换机提前退出 ``dsa_register_switch()`` 调用，因为 ``dsa_tree_setup_routing_table()`` 已判定该树尚未完整 (DSA 链路引用的所有端口尚未全部出现在树的端口列表中)。当最后一个交换机调用 ``dsa_register_switch()`` 时，树即告完整，这将触发该树内所有交换机初始化的有效延续 (包括对 ``ds->ops->setup()`` 的调用)，而这一切都发生在最后一个交换机探测函数的调用上下文中。)

The opposite of registration takes place when calling ``dsa_unregister_switch()``,
which removes a switch's ports from the port list of the tree. The entire tree
is torn down when the first switch unregisters.
(注册的逆过程在调用 ``dsa_unregister_switch()`` 时发生，它把一个交换机的端口从树的端口列表中移除。当第一个交换机注销时，整棵树都会被拆除。)

It is mandatory for DSA switch drivers to implement the ``shutdown()`` callback
of their respective bus, and call ``dsa_switch_shutdown()`` from it (a minimal
version of the full teardown performed by ``dsa_unregister_switch()``).
The reason is that DSA keeps a reference on the conduit net device, and if the
driver for the conduit device decides to unbind on shutdown, DSA's reference
will block that operation from finalizing.
(DSA 交换机驱动必须实现其所在总线的 ``shutdown()`` 回调，并从中调用 ``dsa_switch_shutdown()`` (这是 ``dsa_unregister_switch()`` 所执行的完整拆除的最小版本)。原因是 DSA 持有 conduit 网络设备的一个引用，如果 conduit 设备的驱动决定在关机时解绑，DSA 的引用会阻止该操作完成。)

Either ``dsa_switch_shutdown()`` or ``dsa_unregister_switch()`` must be called,
but not both, and the device driver model permits the bus' ``remove()`` method
to be called even if ``shutdown()`` was already called. Therefore, drivers are
expected to implement a mutual exclusion method between ``remove()`` and
``shutdown()`` by setting their drvdata to NULL after any of these has run, and
checking whether the drvdata is NULL before proceeding to take any action.
(必须调用 ``dsa_switch_shutdown()`` 或 ``dsa_unregister_switch()`` 之一，但不可两者都调用；而且设备驱动模型允许总线的 ``remove()`` 方法即使在 ``shutdown()`` 已经被调用过的情况下仍被调用。因此，驱动应在 ``remove()`` 与 ``shutdown()`` 之间实现一种互斥机制：在任一函数运行之后把 drvdata 置为 NULL，并在执行任何操作之前检查 drvdata 是否为 NULL。)

After ``dsa_switch_shutdown()`` or ``dsa_unregister_switch()`` was called, no
further callbacks via the provided ``dsa_switch_ops`` may take place, and the
driver may free the data structures associated with the ``dsa_switch``.
(在 ``dsa_switch_shutdown()`` 或 ``dsa_unregister_switch()`` 被调用之后，不得再通过所提供的 ``dsa_switch_ops`` 发生任何回调，驱动可以释放与 ``dsa_switch`` 相关联的数据结构。)

Switch configuration
(交换机配置)
--------------------

- ``get_tag_protocol``: this is to indicate what kind of tagging protocol is
  supported, should be a valid value from the ``dsa_tag_protocol`` enum.
  The returned information does not have to be static; the driver is passed the
  CPU port number, as well as the tagging protocol of a possibly stacked
  upstream switch, in case there are hardware limitations in terms of supported
  tag formats.
  ( ``get_tag_protocol``：用于指示支持何种标签协议，应为 ``dsa_tag_protocol`` 枚举中的有效值。返回的信息不必是静态的；驱动会收到 CPU 端口号，以及可能已级联的上游交换机的标签协议，以防在支持的标签格式方面存在硬件限制。)

- ``change_tag_protocol``: when the default tagging protocol has compatibility
  problems with the conduit or other issues, the driver may support changing it
  at runtime, either through a device tree property or through sysfs. In that
  case, further calls to ``get_tag_protocol`` should report the protocol in
  current use.
  ( ``change_tag_protocol``：当默认标签协议与 conduit 存在兼容性问题或其他问题时，驱动可以支持在运行时更改它，方式可以是设备树属性，也可以是 sysfs。在这种情况下，后续对 ``get_tag_protocol`` 的调用应报告当前使用的协议。)

- ``setup``: setup function for the switch, this function is responsible for setting
  up the ``dsa_switch_ops`` private structure with all it needs: register maps,
  interrupts, mutexes, locks, etc. This function is also expected to properly
  configure the switch to separate all network interfaces from each other, that
  is, they should be isolated by the switch hardware itself, typically by creating
  a Port-based VLAN ID for each port and allowing only the CPU port and the
  specific port to be in the forwarding vector. Ports that are unused by the
  platform should be disabled. Past this function, the switch is expected to be
  fully configured and ready to serve any kind of request. It is recommended
  to issue a software reset of the switch during this setup function in order to
  avoid relying on what a previous software agent such as a bootloader/firmware
  may have previously configured. The method responsible for undoing any
  applicable allocations or operations done here is ``teardown``.
  ( ``setup``：交换机的设置函数，负责为 ``dsa_switch_ops`` 私有结构体准备好所需的一切：寄存器映射、中断、互斥锁、自旋锁等。该函数还应正确配置交换机，使所有网络接口彼此分离，也就是说，它们应由交换机硬件本身隔离，通常做法是为每个端口创建一个基于端口的 VLAN ID，并只允许 CPU 端口和该特定端口处于转发向量中。平台未使用的端口应被禁用。过了这个函数之后，交换机应已完全配置好，可以处理任何类型的请求。建议在此设置函数中对交换机执行一次软件复位，以避免依赖先前的软件代理 (如引导加载程序/固件) 可能已配置的内容。负责撤销此处所做的一切适用分配或操作的方法是 ``teardown`` 。)

- ``port_setup`` and ``port_teardown``: methods for initialization and
  destruction of per-port data structures. It is mandatory for some operations
  such as registering and unregistering devlink port regions to be done from
  these methods, otherwise they are optional. A port will be torn down only if
  it has been previously set up. It is possible for a port to be set up during
  probing only to be torn down immediately afterwards, for example in case its
  PHY cannot be found. In this case, probing of the DSA switch continues
  without that particular port.
  ( ``port_setup`` 与 ``port_teardown``：初始化和销毁每端口数据结构的方法。某些操作 (例如注册和注销 devlink 端口区域) 必须在这两个方法中进行，除此之外它们是可选的。端口只有在先前已被设置过的情况下才会被拆除。端口可能在探测期间被设置、随后立即被拆除，例如找不到其 PHY 的情况下。此时，DSA 交换机的探测会在没有该特定端口的情况下继续进行。)

- ``port_change_conduit``: method through which the affinity (association used
  for traffic termination purposes) between a user port and a CPU port can be
  changed. By default all user ports from a tree are assigned to the first
  available CPU port that makes sense for them (most of the times this means
  the user ports of a tree are all assigned to the same CPU port, except for H
  topologies as described in commit 2c0b03258b8b). The ``port`` argument
  represents the index of the user port, and the ``conduit`` argument represents
  the new DSA conduit ``net_device``. The CPU port associated with the new
  conduit can be retrieved by looking at ``struct dsa_port *cpu_dp =
  conduit->dsa_ptr``. Additionally, the conduit can also be a LAG device where
  all the slave devices are physical DSA conduits. LAG DSA  also have a
  valid ``conduit->dsa_ptr`` pointer, however this is not unique, but rather a
  duplicate of the first physical DSA conduit's (LAG slave) ``dsa_ptr``. In case
  of a LAG DSA conduit, a further call to ``port_lag_join`` will be emitted
  separately for the physical CPU ports associated with the physical DSA
  conduits, requesting them to create a hardware LAG associated with the LAG
  interface.
  ( ``port_change_conduit``：用于更改用户端口与 CPU 端口之间的亲和关系 (为流量终结目的而建立的关联) 的方法。默认情况下，树中的所有用户端口都被分配给对它们有意义的第一个可用 CPU 端口 (大多数情况下，这意味着树的所有用户端口都分配给同一个 CPU 端口，但 commit 2c0b03258b8b 所描述的 H 拓扑除外)。 ``port`` 参数表示用户端口的索引， ``conduit`` 参数表示新的 DSA conduit ``net_device`` 。与新 conduit 关联的 CPU 端口可以通过查看 ``struct dsa_port *cpu_dp = conduit->dsa_ptr`` 获得。此外，conduit 也可以是一个 LAG 设备，其所有 slave 设备都是物理 DSA conduit。LAG DSA 也拥有有效的 ``conduit->dsa_ptr`` 指针，但它并不唯一，而是第一个物理 DSA conduit (LAG slave) ``dsa_ptr`` 的副本。对于 LAG DSA conduit，还会为与物理 DSA conduit 相关联的物理 CPU 端口分别发出对 ``port_lag_join`` 的进一步调用，请求它们创建与该 LAG 接口相关联的硬件 LAG。)

PHY devices and link management
(PHY 设备与链路管理)
-------------------------------

- ``get_phy_flags``: Some switches are interfaced to various kinds of Ethernet PHYs,
  if the PHY library PHY driver needs to know about information it cannot obtain
  on its own (e.g.: coming from switch memory mapped registers), this function
  should return a 32-bit bitmask of "flags" that is private between the switch
  driver and the Ethernet PHY driver in ``drivers/net/phy/\*``.
  ( ``get_phy_flags``：有些交换机与各种类型的以太网 PHY 相接，如果 PHY 库的 PHY 驱动需要了解它自己无法获得的信息 (例如来自交换机内存映射寄存器的信息)，此函数应返回一个 32 位的 "flags" 位掩码，该掩码是交换机驱动与 ``drivers/net/phy/\*`` 中以太网 PHY 驱动之间的私有约定。)

- ``phy_read``: Function invoked by the DSA user MDIO bus when attempting to read
  the switch port MDIO registers. If unavailable, return 0xffff for each read.
  For builtin switch Ethernet PHYs, this function should allow reading the link
  status, auto-negotiation results, link partner pages, etc.
  ( ``phy_read``：DSA 用户 MDIO 总线在尝试读取交换机端口 MDIO 寄存器时调用的函数。如果不可用，每次读取应返回 0xffff。对于交换机内建的以太网 PHY，此函数应允许读取链路状态、自动协商结果、链路伙伴页等。)

- ``phy_write``: Function invoked by the DSA user MDIO bus when attempting to write
  to the switch port MDIO registers. If unavailable return a negative error
  code.
  ( ``phy_write``：DSA 用户 MDIO 总线在尝试写入交换机端口 MDIO 寄存器时调用的函数。如果不可用，返回负的错误码。)

- ``adjust_link``: Function invoked by the PHY library when a user network device
  is attached to a PHY device. This function is responsible for appropriately
  configuring the switch port link parameters: speed, duplex, pause based on
  what the ``phy_device`` is providing.
  ( ``adjust_link``：当用户网络设备连接到 PHY 设备时由 PHY 库调用的函数。此函数负责根据 ``phy_device`` 提供的内容，恰当地配置交换机端口的链路参数：速率、双工、流控 (pause)。)

- ``fixed_link_update``: Function invoked by the PHY library, and specifically by
  the fixed PHY driver asking the switch driver for link parameters that could
  not be auto-negotiated, or obtained by reading the PHY registers through MDIO.
  This is particularly useful for specific kinds of hardware such as QSGMII,
  MoCA or other kinds of non-MDIO managed PHYs where out of band link
  information is obtained
  ( ``fixed_link_update``：由 PHY 库 (具体而言是 fixed PHY 驱动) 调用的函数，向交换机驱动询问无法通过自动协商或通过 MDIO 读取 PHY 寄存器获得的链路参数。对于 QSGMII、MoCA 等通过带外途径获取链路信息的特定类型硬件或其他非 MDIO 管理的 PHY，此函数尤其有用)

Ethtool operations
(Ethtool 操作)
------------------

- ``get_strings``: ethtool function used to query the driver's strings, will
  typically return statistics strings, private flags strings, etc.
  ( ``get_strings``：用于查询驱动字符串的 ethtool 函数，通常返回统计字符串、私有标志字符串等。)

- ``get_ethtool_stats``: ethtool function used to query per-port statistics and
  return their values. DSA overlays user network devices general statistics:
  RX/TX counters from the network device, with switch driver specific statistics
  per port
  ( ``get_ethtool_stats``：用于查询每端口统计信息并返回其值的 ethtool 函数。DSA 把用户网络设备的通用统计信息 (来自网络设备的 RX/TX 计数器) 与交换机驱动特定的每端口统计信息叠加在一起)

- ``get_sset_count``: ethtool function used to query the number of statistics items
  ( ``get_sset_count``：用于查询统计条目数量的 ethtool 函数)

- ``get_wol``: ethtool function used to obtain Wake-on-LAN settings per-port, this
  function may for certain implementations also query the conduit network device
  Wake-on-LAN settings if this interface needs to participate in Wake-on-LAN
  ( ``get_wol``：用于获取每端口 Wake-on-LAN 设置的 ethtool 函数；在某些实现中，如果该接口需要参与 Wake-on-LAN，此函数还会查询 conduit 网络设备的 Wake-on-LAN 设置)

- ``set_wol``: ethtool function used to configure Wake-on-LAN settings per-port,
  direct counterpart to set_wol with similar restrictions
  ( ``set_wol``：用于配置每端口 Wake-on-LAN 设置的 ethtool 函数，是 set_wol 的直接对应操作，具有类似的限制)

- ``set_eee``: ethtool function which is used to configure a switch port EEE (Green
  Ethernet) settings, can optionally invoke the PHY library to enable EEE at the
  PHY level if relevant. This function should enable EEE at the switch port MAC
  controller and data-processing logic
  ( ``set_eee``：用于配置交换机端口 EEE (Green Ethernet) 设置的 ethtool 函数；如相关，可选择调用 PHY 库在 PHY 层级启用 EEE。此函数应在交换机端口的 MAC 控制器和数据处理逻辑中启用 EEE)

- ``get_eee``: ethtool function which is used to query a switch port EEE settings,
  this function should return the EEE state of the switch port MAC controller
  and data-processing logic as well as query the PHY for its currently configured
  EEE settings
  ( ``get_eee``：用于查询交换机端口 EEE 设置的 ethtool 函数；此函数应返回交换机端口 MAC 控制器和数据处理逻辑的 EEE 状态，并向 PHY 查询其当前配置的 EEE 设置)

- ``get_eeprom_len``: ethtool function returning for a given switch the EEPROM
  length/size in bytes
  ( ``get_eeprom_len``：返回给定交换机 EEPROM 长度/大小 (以字节为单位) 的 ethtool 函数)

- ``get_eeprom``: ethtool function returning for a given switch the EEPROM contents
  ( ``get_eeprom``：返回给定交换机 EEPROM 内容的 ethtool 函数)

- ``set_eeprom``: ethtool function writing specified data to a given switch EEPROM
  ( ``set_eeprom``：把指定数据写入给定交换机 EEPROM 的 ethtool 函数)

- ``get_regs_len``: ethtool function returning the register length for a given
  switch
  ( ``get_regs_len``：返回给定交换机寄存器长度的 ethtool 函数)

- ``get_regs``: ethtool function returning the Ethernet switch internal register
  contents. This function might require user-land code in ethtool to
  pretty-print register values and registers
  ( ``get_regs``：返回以太网交换机内部寄存器内容的 ethtool 函数。该功能可能需要 ethtool 中的用户态代码来美化打印寄存器值和寄存器)

Power management
(电源管理)
----------------

- ``suspend``: function invoked by the DSA platform device when the system goes to
  suspend, should quiesce all Ethernet switch activities, but keep ports
  participating in Wake-on-LAN active as well as additional wake-up logic if
  supported
  ( ``suspend``：系统进入挂起时由 DSA 平台设备调用的函数；应停息所有以太网交换机活动，但保持参与 Wake-on-LAN 的端口处于活动状态，若支持的话还要保留额外的唤醒逻辑)

- ``resume``: function invoked by the DSA platform device when the system resumes,
  should resume all Ethernet switch activities and re-configure the switch to be
  in a fully active state
  ( ``resume``：系统恢复时由 DSA 平台设备调用的函数；应恢复所有以太网交换机活动，并把交换机重新配置为完全活动状态)

- ``port_enable``: function invoked by the DSA user network device ndo_open
  function when a port is administratively brought up, this function should
  fully enable a given switch port. DSA takes care of marking the port with
  ``BR_STATE_BLOCKING`` if the port is a bridge member, or ``BR_STATE_FORWARDING`` if it
  was not, and propagating these changes down to the hardware
  ( ``port_enable``：当端口被管理性启用时由 DSA 用户网络设备的 ndo_open 函数调用的函数；此函数应完全启用给定的交换机端口。如果该端口是网桥成员，DSA 会负责用 ``BR_STATE_BLOCKING`` 标记端口，否则用 ``BR_STATE_FORWARDING`` 标记，并把这些更改向下传播到硬件)

- ``port_disable``: function invoked by the DSA user network device ndo_close
  function when a port is administratively brought down, this function should
  fully disable a given switch port. DSA takes care of marking the port with
  ``BR_STATE_DISABLED`` and propagating changes to the hardware if this port is
  disabled while being a bridge member
  ( ``port_disable``：当端口被管理性禁用时由 DSA 用户网络设备的 ndo_close 函数调用的函数；此函数应完全禁用给定的交换机端口。如果该端口在被禁用时是网桥成员，DSA 会负责用 ``BR_STATE_DISABLED`` 标记端口并把更改传播到硬件)

Address databases
(地址数据库)
-----------------

Switching hardware is expected to have a table for FDB entries, however not all
of them are active at the same time. An address database is the subset (partition)
of FDB entries that is active (can be matched by address learning on RX, or FDB
lookup on TX) depending on the state of the port. An address database may
occasionally be called "FID" (Filtering ID) in this document, although the
underlying implementation may choose whatever is available to the hardware.
(交换硬件应有一个用于 FDB 表项的表，但并非所有表项都同时处于活动状态。地址数据库是 FDB 表项的一个子集 (分区)，它根据端口的状态处于活动状态 (可在 RX 时通过地址学习匹配，或在 TX 时通过 FDB 查找匹配)。在本文档中，地址数据库偶尔也被称为 "FID" (Filtering ID)，尽管底层实现可以选择硬件可用的任何机制。)

For example, all ports that belong to a VLAN-unaware bridge (which is
*currently* VLAN-unaware) are expected to learn source addresses in the
database associated by the driver with that bridge (and not with other
VLAN-unaware bridges). During forwarding and FDB lookup, a packet received on a
VLAN-unaware bridge port should be able to find a VLAN-unaware FDB entry having
the same MAC DA as the packet, which is present on another port member of the
same bridge. At the same time, the FDB lookup process must be able to not find
an FDB entry having the same MAC DA as the packet, if that entry points towards
a port which is a member of a different VLAN-unaware bridge (and is therefore
associated with a different address database).
(例如，属于一个不感知 VLAN 的网桥 (即 *当前* 不感知 VLAN 的网桥) 的所有端口，都应在该驱动与该网桥相关联的数据库中学习源地址 (而不是在其他不感知 VLAN 的网桥的数据库中)。在转发和 FDB 查找期间，从不感知 VLAN 的网桥端口收到的数据包应能找到一条与该数据包具有相同 MAC DA 的不感知 VLAN 的 FDB 表项，该表项存在于同一网桥的另一个端口成员上。与此同时，FDB 查找过程必须能够找不到与该数据包具有相同 MAC DA 的 FDB 表项——如果该表项指向属于另一个不感知 VLAN 网桥的端口 (因此关联到不同的地址数据库)。)

Similarly, each VLAN of each offloaded VLAN-aware bridge should have an
associated address database, which is shared by all ports which are members of
that VLAN, but not shared by ports belonging to different bridges that are
members of the same VID.
(类似地，每个被卸载的感知 VLAN 的网桥的每个 VLAN 都应有一个相关联的地址数据库，该数据库由该 VLAN 的所有成员端口共享，但不会被属于同一 VID 的不同网桥的端口共享。)

In this context, a VLAN-unaware database means that all packets are expected to
match on it irrespective of VLAN ID (only MAC address lookup), whereas a
VLAN-aware database means that packets are supposed to match based on the VLAN
ID from the classified 802.1Q header (or the pvid if untagged).
(在此语境下，不感知 VLAN 的数据库意味着所有数据包都应与其匹配而不考虑 VLAN ID (只做 MAC 地址查找)；而感知 VLAN 的数据库意味着数据包应基于已分类的 802.1Q 头中的 VLAN ID (若未打标签则是 pvid) 进行匹配。)

At the bridge layer, VLAN-unaware FDB entries have the special VID value of 0,
whereas VLAN-aware FDB entries have non-zero VID values. Note that a
VLAN-unaware bridge may have VLAN-aware (non-zero VID) FDB entries, and a
VLAN-aware bridge may have VLAN-unaware FDB entries. As in hardware, the
software bridge keeps separate address databases, and offloads to hardware the
FDB entries belonging to these databases, through switchdev, asynchronously
relative to the moment when the databases become active or inactive.
(在网桥层，不感知 VLAN 的 FDB 表项具有特殊的 VID 值 0，而感知 VLAN 的 FDB 表项具有非零 VID 值。注意，不感知 VLAN 的网桥也可能拥有感知 VLAN 的 (非零 VID) FDB 表项，感知 VLAN 的网桥也可能拥有不感知 VLAN 的 FDB 表项。与硬件中一样，软件网桥维护着相互独立的地址数据库，并通过 switchdev 把属于这些数据库的 FDB 表项异步卸载到硬件，异步是相对于数据库变为活动或不活动的时刻而言的。)

When a user port operates in standalone mode, its driver should configure it to
use a separate database called a port private database. This is different from
the databases described above, and should impede operation as standalone port
(packet in, packet out to the CPU port) as little as possible. For example,
on ingress, it should not attempt to learn the MAC SA of ingress traffic, since
learning is a bridging layer service and this is a standalone port, therefore
it would consume useless space. With no address learning, the port private
database should be empty in a naive implementation, and in this case, all
received packets should be trivially flooded to the CPU port.
(当用户端口以独立模式运行时，其驱动应把它配置为使用一个单独的数据库，称为端口私有数据库。它不同于上文描述的数据库，并且应尽可能少地妨碍作为独立端口的运行 (数据包进入，数据包发往 CPU 端口)。例如，在入口方向，它不应尝试学习入口流量的 MAC SA，因为学习是网桥层的服务，而这是一个独立端口，学习只会消耗无用的空间。在没有地址学习的情况下，端口私有数据库在朴素实现中应为空；在这种情况下，所有收到的数据包都应简单地洪泛到 CPU 端口。)

DSA (cascade) and CPU ports are also called "shared" ports because they service
multiple address databases, and the database that a packet should be associated
to is usually embedded in the DSA tag. This means that the CPU port may
simultaneously transport packets coming from a standalone port (which were
classified by hardware in one address database), and from a bridge port (which
were classified to a different address database).
(DSA (级联) 端口和 CPU 端口也被称为"共享"端口，因为它们服务于多个地址数据库，而数据包应关联到的数据库通常嵌入在 DSA 标签中。这意味着 CPU 端口可能同时传输来自独立端口的数据包 (由硬件在一个地址数据库中分类) 和来自网桥端口的数据包 (被分类到另一个地址数据库)。)

Switch drivers which satisfy certain criteria are able to optimize the naive
configuration by removing the CPU port from the flooding domain of the switch,
and just program the hardware with FDB entries pointing towards the CPU port
for which it is known that software is interested in those MAC addresses.
Packets which do not match a known FDB entry will not be delivered to the CPU,
which will save CPU cycles required for creating an skb just to drop it.
(满足特定条件的交换机驱动可以优化这种朴素配置：把 CPU 端口从交换机的洪泛域中移除，只在硬件中编程那些指向 CPU 端口、且已知软件感兴趣的 MAC 地址的 FDB 表项。不匹配任何已知 FDB 表项的数据包将不会投递到 CPU，从而节省仅为丢弃数据包而创建 skb 所需的 CPU 周期。)

DSA is able to perform host address filtering for the following kinds of
addresses:
(DSA 能够对以下几类地址执行主机地址过滤：)

- Primary unicast MAC addresses of ports (``dev->dev_addr``). These are
  associated with the port private database of the respective user port,
  and the driver is notified to install them through ``port_fdb_add`` towards
  the CPU port.
  (端口的初级单播 MAC 地址 (``dev->dev_addr``)。它们与相应用户端口的端口私有数据库相关联，并通过 ``port_fdb_add`` 通知驱动将其安装到 CPU 端口。)

- Secondary unicast and multicast MAC addresses of ports (addresses added
  through ``dev_uc_add()`` and ``dev_mc_add()``). These are also associated
  with the port private database of the respective user port.
  (端口的次级单播和组播 MAC 地址 (通过 ``dev_uc_add()`` 和 ``dev_mc_add()`` 添加的地址)。它们同样与相应用户端口的端口私有数据库相关联。)

- Local/permanent bridge FDB entries (``BR_FDB_LOCAL``). These are the MAC
  addresses of the bridge ports, for which packets must be terminated locally
  and not forwarded. They are associated with the address database for that
  bridge.
  (本地/永久网桥 FDB 表项 (``BR_FDB_LOCAL``)。这些是网桥端口的 MAC 地址，针对它们的数据包必须在本地终结而不能转发。它们与该网桥的地址数据库相关联。)

- Static bridge FDB entries installed towards foreign (non-DSA) interfaces
  present in the same bridge as some DSA switch ports. These are also
  associated with the address database for that bridge.
  (指向外来 (非 DSA) 接口的静态网桥 FDB 表项，这些外来接口与某些 DSA 交换机端口处于同一网桥中。它们同样与该网桥的地址数据库相关联。)

- Dynamically learned FDB entries on foreign interfaces present in the same
  bridge as some DSA switch ports, only if ``ds->assisted_learning_on_cpu_port``
  is set to true by the driver. These are associated with the address database
  for that bridge.
  (外来接口上动态学习的 FDB 表项 (这些外来接口与某些 DSA 交换机端口处于同一网桥中)，前提是驱动把 ``ds->assisted_learning_on_cpu_port`` 设置为 true。这些表项与该网桥的地址数据库相关联。)

For various operations detailed below, DSA provides a ``dsa_db`` structure
which can be of the following types:
(对于下文详述的各种操作，DSA 提供一个 ``dsa_db`` 结构体，它可以是以下类型：)

- ``DSA_DB_PORT``: the FDB (or MDB) entry to be installed or deleted belongs to
  the port private database of user port ``db->dp``.
  ( ``DSA_DB_PORT``：要安装或删除的 FDB (或 MDB) 表项属于用户端口 ``db->dp`` 的端口私有数据库。)
- ``DSA_DB_BRIDGE``: the entry belongs to one of the address databases of bridge
  ``db->bridge``. Separation between the VLAN-unaware database and the per-VID
  databases of this bridge is expected to be done by the driver.
  ( ``DSA_DB_BRIDGE``：表项属于网桥 ``db->bridge`` 的某个地址数据库。该网桥的不感知 VLAN 数据库与各 per-VID 数据库之间的区分应由驱动完成。)
- ``DSA_DB_LAG``: the entry belongs to the address database of LAG ``db->lag``.
  Note: ``DSA_DB_LAG`` is currently unused and may be removed in the future.
  ( ``DSA_DB_LAG``：表项属于 LAG ``db->lag`` 的地址数据库。注意： ``DSA_DB_LAG`` 目前未被使用，将来可能被移除。)

The drivers which act upon the ``dsa_db`` argument in ``port_fdb_add``,
``port_mdb_add`` etc should declare ``ds->fdb_isolation`` as true.
(在 ``port_fdb_add`` 、 ``port_mdb_add`` 等方法中处理 ``dsa_db`` 参数的驱动应把 ``ds->fdb_isolation`` 声明为 true。)

DSA associates each offloaded bridge and each offloaded LAG with a one-based ID
(``struct dsa_bridge :: num``, ``struct dsa_lag :: id``) for the purposes of
refcounting addresses on shared ports. Drivers may piggyback on DSA's numbering
scheme (the ID is readable through ``db->bridge.num`` and ``db->lag.id`` or may
implement their own.
(DSA 为每个被卸载的网桥和每个被卸载的 LAG 关联一个从 1 开始的 ID (``struct dsa_bridge :: num`` 、 ``struct dsa_lag :: id``)，用于在共享端口上对地址进行引用计数。驱动可以借助 DSA 的编号方案 (该 ID 可通过 ``db->bridge.num`` 和 ``db->lag.id`` 读取)，也可以实现自己的方案。)

Only the drivers which declare support for FDB isolation are notified of FDB
entries on the CPU port belonging to ``DSA_DB_PORT`` databases.
For compatibility/legacy reasons, ``DSA_DB_BRIDGE`` addresses are notified to
drivers even if they do not support FDB isolation. However, ``db->bridge.num``
and ``db->lag.id`` are always set to 0 in that case (to denote the lack of
isolation, for refcounting purposes).
(只有声明支持 FDB 隔离的驱动才会收到关于 CPU 端口上属于 ``DSA_DB_PORT`` 数据库的 FDB 表项的通知。出于兼容性/历史原因，即使驱动不支持 FDB 隔离， ``DSA_DB_BRIDGE`` 地址也会通知给驱动。不过在这种情况下， ``db->bridge.num`` 和 ``db->lag.id`` 总是被设置为 0 (以表示缺乏隔离，用于引用计数目的)。)

Note that it is not mandatory for a switch driver to implement physically
separate address databases for each standalone user port. Since FDB entries in
the port private databases will always point to the CPU port, there is no risk
for incorrect forwarding decisions. In this case, all standalone ports may
share the same database, but the reference counting of host-filtered addresses
(not deleting the FDB entry for a port's MAC address if it's still in use by
another port) becomes the responsibility of the driver, because DSA is unaware
that the port databases are in fact shared. This can be achieved by calling
``dsa_fdb_present_in_other_db()`` and ``dsa_mdb_present_in_other_db()``.
The down side is that the RX filtering lists of each user port are in fact
shared, which means that user port A may accept a packet with a MAC DA it
shouldn't have, only because that MAC address was in the RX filtering list of
user port B. These packets will still be dropped in software, however.
(注意，交换机驱动并不强制为每个独立用户端口实现物理上相互分离的地址数据库。由于端口私有数据库中的 FDB 表项始终指向 CPU 端口，不存在错误转发决策的风险。在这种情况下，所有独立端口可以共享同一个数据库，但主机过滤地址的引用计数 (如果某端口的 MAC 地址仍被另一个端口使用，则不删除其 FDB 表项) 就成为驱动的责任，因为 DSA 并不知道端口数据库实际上是共享的。这可以通过调用 ``dsa_fdb_present_in_other_db()`` 和 ``dsa_mdb_present_in_other_db()`` 来实现。缺点是每个用户端口的 RX 过滤列表实际上是共享的，这意味着用户端口 A 可能接受一个它本不该接受的 MAC DA 的数据包，仅仅因为该 MAC 地址在用户端口 B 的 RX 过滤列表中。不过，这些数据包仍会在软件中被丢弃。)

Bridge layer
(网桥层)
------------

Offloading the bridge forwarding plane is optional and handled by the methods
below. They may be absent, return -EOPNOTSUPP, or ``ds->max_num_bridges`` may
be non-zero and exceeded, and in this case, joining a bridge port is still
possible, but the packet forwarding will take place in software, and the ports
under a software bridge must remain configured in the same way as for
standalone operation, i.e. have all bridging service functions (address
learning etc) disabled, and send all received packets to the CPU port only.
(卸载网桥转发平面是可选的，由下面的方法处理。这些方法可能不存在、返回 -EOPNOTSUPP，或者 ``ds->max_num_bridges`` 非零但已被超出；在这种情况下，端口仍然可以加入网桥，但数据包转发将在软件中进行，并且软件网桥下的端口必须保持与独立运行时相同的配置，即禁用所有网桥服务功能 (地址学习等)，并只把所有收到的数据包发送到 CPU 端口。)

Concretely, a port starts offloading the forwarding plane of a bridge once it
returns success to the ``port_bridge_join`` method, and stops doing so after
``port_bridge_leave`` has been called. Offloading the bridge means autonomously
learning FDB entries in accordance with the software bridge port's state, and
autonomously forwarding (or flooding) received packets without CPU intervention.
This is optional even when offloading a bridge port. Tagging protocol drivers
are expected to call ``dsa_default_offload_fwd_mark(skb)`` for packets which
have already been autonomously forwarded in the forwarding domain of the
ingress switch port. DSA, through ``dsa_port_devlink_setup()``, considers all
switch ports part of the same tree ID to be part of the same bridge forwarding
domain (capable of autonomous forwarding to each other).
(具体来说，端口一旦对 ``port_bridge_join`` 方法返回成功，就开始卸载网桥的转发平面；在 ``port_bridge_leave`` 被调用之后则停止卸载。卸载网桥意味着：根据软件网桥端口的状态自主学习 FDB 表项，并在无需 CPU 干预的情况下自主转发 (或洪泛) 收到的数据包。即使卸载了网桥端口，这仍是可选的。对于已经在入口交换机端口的转发域中被自主转发过的数据包，标签协议驱动应调用 ``dsa_default_offload_fwd_mark(skb)`` 。DSA 通过 ``dsa_port_devlink_setup()`` 把属于同一树 ID 的所有交换机端口视为同一网桥转发域的一部分 (能够相互自主转发)。)

Offloading the TX forwarding process of a bridge is a distinct concept from
simply offloading its forwarding plane, and refers to the ability of certain
driver and tag protocol combinations to transmit a single skb coming from the
bridge device's transmit function to potentially multiple egress ports (and
thereby avoid its cloning in software).
(卸载网桥的 TX 转发过程是一个与单纯卸载其转发平面不同的概念，它指的是某些驱动与标签协议的组合能够把来自网桥设备发送函数的单个 skb 发送到潜在的多个出端口 (从而避免在软件中克隆 skb)。)

Packets for which the bridge requests this behavior are called data plane
packets and have ``skb->offload_fwd_mark`` set to true in the tag protocol
driver's ``xmit`` function. Data plane packets are subject to FDB lookup,
hardware learning on the CPU port, and do not override the port STP state.
Additionally, replication of data plane packets (multicast, flooding) is
handled in hardware and the bridge driver will transmit a single skb for each
packet that may or may not need replication.
(网桥请求这一行为的数据包被称为数据平面 (data plane) 数据包，它们在标签协议驱动的 ``xmit`` 函数中被设置 ``skb->offload_fwd_mark`` 为 true。数据平面数据包要经过 FDB 查找、CPU 端口上的硬件学习，并且不会覆盖端口的 STP 状态。此外，数据平面数据包的复制 (组播、洪泛) 由硬件处理，网桥驱动会为每个可能需要也可能不需要复制的包发送单个 skb。)

When the TX forwarding offload is enabled, the tag protocol driver is
responsible to inject packets into the data plane of the hardware towards the
correct bridging domain (FID) that the port is a part of. The port may be
VLAN-unaware, and in this case the FID must be equal to the FID used by the
driver for its VLAN-unaware address database associated with that bridge.
Alternatively, the bridge may be VLAN-aware, and in that case, it is guaranteed
that the packet is also VLAN-tagged with the VLAN ID that the bridge processed
this packet in. It is the responsibility of the hardware to untag the VID on
the egress-untagged ports, or keep the tag on the egress-tagged ones.
(当 TX 转发卸载被启用时，标签协议驱动负责把数据包注入硬件的数据平面，导向该端口所属的正确网桥域 (FID)。该端口可能不感知 VLAN，在这种情况下，FID 必须等于驱动为其与该网桥相关联的不感知 VLAN 地址数据库所使用的 FID。或者，网桥可能是感知 VLAN 的，在这种情况下，可以保证该数据包也带有 VLAN 标签，其 VLAN ID 正是网桥处理该数据包所用者。在出口不带标签的端口上移除 VID、或在出口带标签的端口上保留标签，是硬件的职责。)

- ``port_bridge_join``: bridge layer function invoked when a given switch port is
  added to a bridge, this function should do what's necessary at the switch
  level to permit the joining port to be added to the relevant logical
  domain for it to ingress/egress traffic with other members of the bridge.
  By setting the ``tx_fwd_offload`` argument to true, the TX forwarding process
  of this bridge is also offloaded.
  ( ``port_bridge_join``：当给定交换机端口被加入网桥时调用的网桥层函数。此函数应在交换机层面做必要的工作，允许加入的端口被加入相应的逻辑域，以便它与网桥的其他成员进行流量的进出。通过把 ``tx_fwd_offload`` 参数设为 true，该网桥的 TX 转发过程也被卸载。)

- ``port_bridge_leave``: bridge layer function invoked when a given switch port is
  removed from a bridge, this function should do what's necessary at the
  switch level to deny the leaving port from ingress/egress traffic from the
  remaining bridge members.
  ( ``port_bridge_leave``：当给定交换机端口从网桥移除时调用的网桥层函数。此函数应在交换机层面做必要的工作，阻止离开的端口与剩余的网桥成员进行流量的进出。)

- ``port_stp_state_set``: bridge layer function invoked when a given switch port STP
  state is computed by the bridge layer and should be propagated to switch
  hardware to forward/block/learn traffic.
  ( ``port_stp_state_set``：当网桥层计算出给定交换机端口的 STP 状态、并应将其传播到交换机硬件以转发/阻塞/学习流量时调用的网桥层函数。)

- ``port_bridge_flags``: bridge layer function invoked when a port must
  configure its settings for e.g. flooding of unknown traffic or source address
  learning. The switch driver is responsible for initial setup of the
  standalone ports with address learning disabled and egress flooding of all
  types of traffic, then the DSA core notifies of any change to the bridge port
  flags when the port joins and leaves a bridge. DSA does not currently manage
  the bridge port flags for the CPU port. The assumption is that address
  learning should be statically enabled (if supported by the hardware) on the
  CPU port, and flooding towards the CPU port should also be enabled, due to a
  lack of an explicit address filtering mechanism in the DSA core.
  ( ``port_bridge_flags``：当端口必须配置其设置 (例如未知流量的洪泛或源地址学习) 时调用的网桥层函数。交换机驱动负责独立端口的初始设置 (禁用地址学习、所有类型的流量都从出口洪泛)，随后当端口加入和离开网桥时，DSA 核心会通知网桥端口标志的任何更改。DSA 目前不管理 CPU 端口的网桥端口标志。其假设是：CPU 端口上的地址学习应静态启用 (如果硬件支持)，并且朝向 CPU 端口的洪泛也应启用，因为 DSA 核心缺乏显式的地址过滤机制。)

- ``port_fast_age``: bridge layer function invoked when flushing the
  dynamically learned FDB entries on the port is necessary. This is called when
  transitioning from an STP state where learning should take place to an STP
  state where it shouldn't, or when leaving a bridge, or when address learning
  is turned off via ``port_bridge_flags``.
  ( ``port_fast_age``：当需要刷新端口上动态学习的 FDB 表项时调用的网桥层函数。它在以下情况下被调用：从应当进行学习的 STP 状态转换到不应进行学习的 STP 状态、离开网桥、或通过 ``port_bridge_flags`` 关闭地址学习时。)

Bridge VLAN filtering
(网桥 VLAN 过滤)
---------------------

- ``port_vlan_filtering``: bridge layer function invoked when the bridge gets
  configured for turning on or off VLAN filtering. If nothing specific needs to
  be done at the hardware level, this callback does not need to be implemented.
  When VLAN filtering is turned on, the hardware must be programmed with
  rejecting 802.1Q frames which have VLAN IDs outside of the programmed allowed
  VLAN ID map/rules.  If there is no PVID programmed into the switch port,
  untagged frames must be rejected as well. When turned off the switch must
  accept any 802.1Q frames irrespective of their VLAN ID, and untagged frames are
  allowed.
  ( ``port_vlan_filtering``：当网桥被配置为开启或关闭 VLAN 过滤时调用的网桥层函数。如果在硬件层面不需要做任何特定操作，则无需实现此回调。当 VLAN 过滤开启时，硬件必须被编程为拒绝那些 VLAN ID 超出已编程的允许 VLAN ID 映射/规则范围的 802.1Q 帧。如果交换机端口中没有编程 PVID，不带标签的帧也必须被拒绝。当过滤关闭时，交换机必须接受任何 802.1Q 帧而不论其 VLAN ID 如何，并且允许不带标签的帧。)

- ``port_vlan_add``: bridge layer function invoked when a VLAN is configured
  (tagged or untagged) for the given switch port. The CPU port becomes a member
  of a VLAN only if a foreign bridge port is also a member of it (and
  forwarding needs to take place in software), or the VLAN is installed to the
  VLAN group of the bridge device itself, for termination purposes
  (``bridge vlan add dev br0 vid 100 self``). VLANs on shared ports are
  reference counted and removed when there is no user left. Drivers do not need
  to manually install a VLAN on the CPU port.
  ( ``port_vlan_add``：当为给定交换机端口配置 VLAN (带标签或不带标签) 时调用的网桥层函数。只有当某个外来网桥端口也是该 VLAN 的成员时 (且转发需要在软件中进行)，或者该 VLAN 被安装到网桥设备自身的 VLAN 组中以供终结之用 (``bridge vlan add dev br0 vid 100 self``)，CPU 端口才会成为该 VLAN 的成员。共享端口上的 VLAN 是引用计数的，当不再有使用者时会被移除。驱动无需在 CPU 端口上手动安装 VLAN。)

- ``port_vlan_del``: bridge layer function invoked when a VLAN is removed from the
  given switch port
  ( ``port_vlan_del``：当从给定交换机端口移除 VLAN 时调用的网桥层函数)

- ``port_fdb_add``: bridge layer function invoked when the bridge wants to install a
  Forwarding Database entry, the switch hardware should be programmed with the
  specified address in the specified VLAN Id in the forwarding database
  associated with this VLAN ID.
  ( ``port_fdb_add``：当网桥想要安装转发数据库 (Forwarding Database) 表项时调用的网桥层函数。交换机硬件应被编程为在与该 VLAN ID 相关联的转发数据库中的指定 VLAN ID 下记录指定地址。)

- ``port_fdb_del``: bridge layer function invoked when the bridge wants to remove a
  Forwarding Database entry, the switch hardware should be programmed to delete
  the specified MAC address from the specified VLAN ID if it was mapped into
  this port forwarding database
  ( ``port_fdb_del``：当网桥想要移除转发数据库表项时调用的网桥层函数。如果指定的 MAC 地址曾被映射到该端口转发数据库中的指定 VLAN ID，交换机硬件应被编程为将其删除)

- ``port_fdb_dump``: bridge bypass function invoked by ``ndo_fdb_dump`` on the
  physical DSA port interfaces. Since DSA does not attempt to keep in sync its
  hardware FDB entries with the software bridge, this method is implemented as
  a means to view the entries visible on user ports in the hardware database.
  The entries reported by this function have the ``self`` flag in the output of
  the ``bridge fdb show`` command.
  ( ``port_fdb_dump``：由 ``ndo_fdb_dump`` 在物理 DSA 端口接口上调用的网桥旁路函数。由于 DSA 不打算让其硬件 FDB 表项与软件网桥保持同步，因此实现此方法是为了查看硬件数据库中用户端口上可见的表项。此函数报告的表项在 ``bridge fdb show`` 命令的输出中带有 ``self`` 标志。)

- ``port_mdb_add``: bridge layer function invoked when the bridge wants to install
  a multicast database entry. The switch hardware should be programmed with the
  specified address in the specified VLAN ID in the forwarding database
  associated with this VLAN ID.
  ( ``port_mdb_add``：当网桥想要安装组播数据库 (multicast database) 表项时调用的网桥层函数。交换机硬件应被编程为在与该 VLAN ID 相关联的转发数据库中的指定 VLAN ID 下记录指定地址。)

- ``port_mdb_del``: bridge layer function invoked when the bridge wants to remove a
  multicast database entry, the switch hardware should be programmed to delete
  the specified MAC address from the specified VLAN ID if it was mapped into
  this port forwarding database.
  ( ``port_mdb_del``：当网桥想要移除组播数据库表项时调用的网桥层函数。如果指定的 MAC 地址曾被映射到该端口转发数据库中的指定 VLAN ID，交换机硬件应被编程为将其删除。)

Link aggregation
(链路聚合)
----------------

Link aggregation is implemented in the Linux networking stack by the bonding
and team drivers, which are modeled as virtual, stackable network interfaces.
DSA is capable of offloading a link aggregation group (LAG) to hardware that
supports the feature, and supports bridging between physical ports and LAGs,
as well as between LAGs. A bonding/team interface which holds multiple physical
ports constitutes a logical port, although DSA has no explicit concept of a
logical port at the moment. Due to this, events where a LAG joins/leaves a
bridge are treated as if all individual physical ports that are members of that
LAG join/leave the bridge. Switchdev port attributes (VLAN filtering, STP
state, etc) and objects (VLANs, MDB entries) offloaded to a LAG as bridge port
are treated similarly: DSA offloads the same switchdev object / port attribute
on all members of the LAG. Static bridge FDB entries on a LAG are not yet
supported, since the DSA driver API does not have the concept of a logical port
ID.
(链路聚合在 Linux 网络栈中由 bonding 和 team 驱动实现，它们被建模为虚拟的、可堆叠的网络接口。DSA 能够把链路聚合组 (LAG) 卸载到支持该特性的硬件上，并支持物理端口与 LAG 之间以及 LAG 相互之间的桥接。持有多个物理端口的 bonding/team 接口构成一个逻辑端口，尽管 DSA 目前没有显式的逻辑端口概念。正因如此，LAG 加入/离开网桥的事件被当作该 LAG 的所有成员物理端口分别加入/离开网桥来处理。作为网桥端口卸载到 LAG 的 switchdev 端口属性 (VLAN 过滤、STP 状态等) 和对象 (VLAN、MDB 表项) 也按类似方式处理：DSA 在 LAG 的所有成员上卸载相同的 switchdev 对象/端口属性。LAG 上的静态网桥 FDB 表项尚不支持，因为 DSA 驱动 API 没有逻辑端口 ID 的概念。)

- ``port_lag_join``: function invoked when a given switch port is added to a
  LAG. The driver may return ``-EOPNOTSUPP``, and in this case, DSA will fall
  back to a software implementation where all traffic from this port is sent to
  the CPU.
  ( ``port_lag_join``：当给定交换机端口被加入 LAG 时调用的函数。驱动可以返回 ``-EOPNOTSUPP``，在这种情况下，DSA 会回退到软件实现，把来自该端口的所有流量都发送到 CPU。)
- ``port_lag_leave``: function invoked when a given switch port leaves a LAG
  and returns to operation as a standalone port.
  ( ``port_lag_leave``：当给定交换机端口离开 LAG、恢复为独立端口运行时调用的函数。)
- ``port_lag_change``: function invoked when the link state of any member of
  the LAG changes, and the hashing function needs rebalancing to only make use
  of the subset of physical LAG member ports that are up.
  ( ``port_lag_change``：当 LAG 任一成员的链路状态发生变化、哈希函数需要重新平衡以只利用处于 up 状态的物理 LAG 成员端口子集时调用的函数。)

Drivers that benefit from having an ID associated with each offloaded LAG
can optionally populate ``ds->num_lag_ids`` from the ``dsa_switch_ops::setup``
method. The LAG ID associated with a bonding/team interface can then be
retrieved by a DSA switch driver using the ``dsa_lag_id`` function.
(希望为每个被卸载的 LAG 关联一个 ID 的驱动，可以在 ``dsa_switch_ops::setup`` 方法中选择性地填充 ``ds->num_lag_ids`` 。之后，DSA 交换机驱动可以使用 ``dsa_lag_id`` 函数获取与 bonding/team 接口相关联的 LAG ID。)

IEC 62439-2 (MRP)
(IEC 62439-2 (MRP))
-----------------

The Media Redundancy Protocol is a topology management protocol optimized for
fast fault recovery time for ring networks, which has some components
implemented as a function of the bridge driver. MRP uses management PDUs
(Test, Topology, LinkDown/Up, Option) sent at a multicast destination MAC
address range of 01:15:4e:00:00:0x and with an EtherType of 0x88e3.
Depending on the node's role in the ring (MRM: Media Redundancy Manager,
MRC: Media Redundancy Client, MRA: Media Redundancy Automanager), certain MRP
PDUs might need to be terminated locally and others might need to be forwarded.
An MRM might also benefit from offloading to hardware the creation and
transmission of certain MRP PDUs (Test).
(介质冗余协议 (Media Redundancy Protocol) 是一种针对环形网络快速故障恢复时间而优化的拓扑管理协议，它的一部分组件以网桥驱动的功能形式实现。MRP 使用管理 PDU (Test、Topology、LinkDown/Up、Option)，发送到 01:15:4e:00:00:0x 的组播目标 MAC 地址范围，EtherType 为 0x88e3。根据节点在环中的角色 (MRM：介质冗余管理器；MRC：介质冗余客户端；MRA：介质冗余自动管理器)，某些 MRP PDU 可能需要在本地终结，另一些可能需要被转发。MRM 还可以从把某些 MRP PDU (Test) 的创建与发送卸载到硬件中获益。)

Normally an MRP instance can be created on top of any network interface,
however in the case of a device with an offloaded data path such as DSA, it is
necessary for the hardware, even if it is not MRP-aware, to be able to extract
the MRP PDUs from the fabric before the driver can proceed with the software
implementation. DSA today has no driver which is MRP-aware, therefore it only
listens for the bare minimum switchdev objects required for the software assist
to work properly. The operations are detailed below.
(通常，MRP 实例可以创建在任何网络接口之上；但对于具有卸载数据路径的设备 (如 DSA)，即使硬件不感知 MRP，它也必须能够在驱动继续进行软件实现之前，把 MRP PDU 从 fabric 中提取出来。目前 DSA 没有任何 MRP 感知的驱动，因此它只监听软件辅助正常工作所需的最低限度的 switchdev 对象。相关操作详见下文。)

- ``port_mrp_add`` and ``port_mrp_del``: notifies driver when an MRP instance
  with a certain ring ID, priority, primary port and secondary port is
  created/deleted.
  ( ``port_mrp_add`` 与 ``port_mrp_del``：当具有特定环 ID、优先级、主端口和辅端口的 MRP 实例被创建/删除时通知驱动。)
- ``port_mrp_add_ring_role`` and ``port_mrp_del_ring_role``: function invoked
  when an MRP instance changes ring roles between MRM or MRC. This affects
  which MRP PDUs should be trapped to software and which should be autonomously
  forwarded.
  ( ``port_mrp_add_ring_role`` 与 ``port_mrp_del_ring_role``：当 MRP 实例在 MRM 或 MRC 之间改变环角色时调用的函数。这会影响哪些 MRP PDU 应被陷阱到软件、哪些应被自主转发。)

IEC 62439-3 (HSR/PRP)
(IEC 62439-3 (HSR/PRP))
---------------------

The Parallel Redundancy Protocol (PRP) is a network redundancy protocol which
works by duplicating and sequence numbering packets through two independent L2
networks (which are unaware of the PRP tail tags carried in the packets), and
eliminating the duplicates at the receiver. The High-availability Seamless
Redundancy (HSR) protocol is similar in concept, except all nodes that carry
the redundant traffic are aware of the fact that it is HSR-tagged (because HSR
uses a header with an EtherType of 0x892f) and are physically connected in a
ring topology. Both HSR and PRP use supervision frames for monitoring the
health of the network and for discovery of other nodes.
(并行冗余协议 (PRP) 是一种网络冗余协议，其工作方式是：通过两个相互独立的 L2 网络复制数据包并为其编号排序 (这两个网络并不感知数据包中携带的 PRP 尾部标签)，然后在接收端消除重复。高可用无缝冗余 (HSR) 协议在概念上与之类似，区别在于所有承载冗余流量的节点都知晓流量带有 HSR 标签 (因为 HSR 使用 EtherType 为 0x892f 的头)，并且在物理上以环形拓扑连接。HSR 和 PRP 都使用监督帧 (supervision frames) 来监视网络健康状况并发现其他节点。)

In Linux, both HSR and PRP are implemented in the hsr driver, which
instantiates a virtual, stackable network interface with two member ports.
The driver only implements the basic roles of DANH (Doubly Attached Node
implementing HSR), DANP (Doubly Attached Node implementing PRP) and RedBox
(allows non-HSR devices to connect to the ring via Interlink ports).
(在 Linux 中，HSR 和 PRP 都在 hsr 驱动中实现，该驱动实例化一个带有两个成员端口的虚拟、可堆叠网络接口。该驱动只实现 DANH (实现 HSR 的双连接节点)、DANP (实现 PRP 的双连接节点) 和 RedBox (允许非 HSR 设备通过 Interlink 端口连接到环) 这几种基本角色。)

A driver which is able of offloading certain functions should declare the
corresponding netdev features as indicated by the documentation at
``Documentation/networking/netdev-features.rst``. Additionally, the following
methods must be implemented:
(能够卸载某些功能的驱动应按 ``Documentation/networking/netdev-features.rst`` 文档的说明声明相应的 netdev 特性。此外，必须实现以下方法：)

- ``port_hsr_join``: function invoked when a given switch port is added to a
  DANP/DANH. The driver may return ``-EOPNOTSUPP`` and in this case, DSA will
  fall back to a software implementation where all traffic from this port is
  sent to the CPU.
  ( ``port_hsr_join``：当给定交换机端口被加入 DANP/DANH 时调用的函数。驱动可以返回 ``-EOPNOTSUPP``，在这种情况下，DSA 会回退到软件实现，把来自该端口的所有流量都发送到 CPU。)
- ``port_hsr_leave``: function invoked when a given switch port leaves a
  DANP/DANH and returns to normal operation as a standalone port.
  ( ``port_hsr_leave``：当给定交换机端口离开 DANP/DANH、恢复正常独立端口运行时调用的函数。)

Note that the ``NETIF_F_HW_HSR_DUP`` feature relies on transmission towards
multiple ports, which is generally available whenever the tagging protocol uses
the ``dsa_xmit_port_mask()`` helper function. If the helper is used, the HSR
offload feature should also be set. The ``dsa_port_simple_hsr_join()`` and
``dsa_port_simple_hsr_leave()`` methods can be used as generic implementations
of ``port_hsr_join`` and ``port_hsr_leave``, if this is the only supported
offload feature.
(注意， ``NETIF_F_HW_HSR_DUP`` 特性依赖于向多个端口的发送，只要标签协议使用了 ``dsa_xmit_port_mask()`` 辅助函数，该能力通常就具备。如果使用了该辅助函数，也应设置 HSR 卸载特性。 ``dsa_port_simple_hsr_join()`` 和 ``dsa_port_simple_hsr_leave()`` 方法可以用作 ``port_hsr_join`` 和 ``port_hsr_leave`` 的通用实现 (如果这是唯一支持的卸载特性)。)

TODO
(TODO)
====

Making SWITCHDEV and DSA converge towards an unified codebase
(让 SWITCHDEV 与 DSA 收敛到统一的代码库)
-------------------------------------------------------------

SWITCHDEV properly takes care of abstracting the networking stack with offload
capable hardware, but does not enforce a strict switch device driver model. On
the other DSA enforces a fairly strict device driver model, and deals with most
of the switch specific. At some point we should envision a merger between these
two subsystems and get the best of both worlds.
(SWITCHDEV 恰当地负责用具备卸载能力的硬件来抽象网络栈，但不强制执行严格的交换机设备驱动模型。而 DSA 则强制执行相当严格的设备驱动模型，并处理大部分交换机特定的事务。在某个时间点，我们应当考虑让这两个子系统合并，兼取两者之长。)
