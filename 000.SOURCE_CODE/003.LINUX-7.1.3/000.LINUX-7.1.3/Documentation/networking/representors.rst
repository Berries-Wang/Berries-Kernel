.. SPDX-License-Identifier: GPL-2.0
.. _representors:

=============================
Network Function Representors
=============================

(网络功能代表设备 (representor))

This document describes the semantics and usage of representor netdevices, as
used to control internal switching on SmartNICs.  For the closely-related port
representors on physical (multi-port) switches, see
:ref:`Documentation/networking/switchdev.rst <switchdev>`.
(本文档描述代表 (representor) 网络设备的语义与用法，它们用于控制 SmartNIC 的内部交换。关于物理（多端口）交换机上密切相关的端口代表设备，参见 :ref:`Documentation/networking/switchdev.rst <switchdev>` 。)

Motivation
(背景动机)
----------

Since the mid-2010s, network cards have started offering more complex
virtualisation capabilities than the legacy SR-IOV approach (with its simple
MAC/VLAN-based switching model) can support.  This led to a desire to offload
software-defined networks (such as OpenVSwitch) to these NICs to specify the
network connectivity of each function.  The resulting designs are variously
called SmartNICs or DPUs.
(自 2010 年代中期以来，网卡开始提供比传统 SR-IOV 方法（其简单的基于 MAC/VLAN 的交换模型）所能支持的更复杂的虚拟化能力。这使人们希望把软件定义网络（如 OpenVSwitch）卸载到这类网卡上，以指定每个功能的网络连接。由此产生的设计被冠以 SmartNIC 或 DPU 等各种名称。)

Network function representors bring the standard Linux networking stack to
virtual switches and IOV devices.  Just as each physical port of a Linux-
controlled switch has a separate netdev, so does each virtual port of a virtual
switch.
When the system boots, and before any offload is configured, all packets from
the virtual functions appear in the networking stack of the PF via the
representors.  The PF can thus always communicate freely with the virtual
functions.
The PF can configure standard Linux forwarding between representors, the uplink
or any other netdev (routing, bridging, TC classifiers).
(网络功能代表设备把标准 Linux 网络栈带入虚拟交换机和 IOV 设备。正如受 Linux 控制的交换机的每个物理端口都有一个独立的 netdev 一样，虚拟交换机的每个虚拟端口也有一个。
系统启动时、在任何卸载被配置之前，来自各虚拟功能的所有数据包都会经由代表设备出现在 PF 的网络栈中。因此 PF 始终可以与各虚拟功能自由通信。
PF 可以在代表设备、上行口或其他任何 netdev 之间配置标准 Linux 转发（路由、桥接、TC 分类器）。)

Thus, a representor is both a control plane object (representing the function in
administrative commands) and a data plane object (one end of a virtual pipe).
As a virtual link endpoint, the representor can be configured like any other
netdevice; in some cases (e.g. link state) the representee will follow the
representor's configuration, while in others there are separate APIs to
configure the representee.
(因此，代表设备既是控制面对象（在管理命令中表示该功能），又是数据面对象（虚拟管道的一端）。作为虚拟链路端点，代表设备可以像任何其他网络设备一样加以配置；在某些情况下（例如链路状态），被代表者 (representee) 会跟随代表设备的配置，而在其他情况下则有单独的 API 用于配置被代表者。)

Definitions
(定义)
-----------

This document uses the term "switchdev function" to refer to the PCIe function
which has administrative control over the virtual switch on the device.
Typically, this will be a PF, but conceivably a NIC could be configured to grant
these administrative privileges instead to a VF or SF (subfunction).
Depending on NIC design, a multi-port NIC might have a single switchdev function
for the whole device or might have a separate virtual switch, and hence
switchdev function, for each physical network port.
If the NIC supports nested switching, there might be separate switchdev
functions for each nested switch, in which case each switchdev function should
only create representors for the ports on the (sub-)switch it directly
administers.
(本文档使用术语 "switchdev 功能" 指代对设备上的虚拟交换机拥有管理控制权的 PCIe 功能。通常这是一个 PF，但可以设想网卡也可以被配置为把这些管理特权改授予某个 VF 或 SF（子功能）。取决于网卡设计，多端口网卡可能整个设备只有一个 switchdev 功能，也可能每个物理网络端口都有一个单独的虚拟交换机，因而各有一个 switchdev 功能。
如果网卡支持嵌套交换，那么每个嵌套交换机可能有各自的 switchdev 功能，此时每个 switchdev 功能应只为其直接管理的（子）交换机上的端口创建代表设备。)

A "representee" is the object that a representor represents.  So for example in
the case of a VF representor, the representee is the corresponding VF.
("被代表者" (representee) 是代表设备所代表的对象。例如对于 VF 代表设备，被代表者就是对应的 VF。)

What does a representor do?
(代表设备有什么作用？)
---------------------------

A representor has three main roles.
(代表设备有三个主要角色。)

1. It is used to configure the network connection the representee sees, e.g.
   link up/down, MTU, etc.  For instance, bringing the representor
   administratively UP should cause the representee to see a link up / carrier
   on event.
   (1. 它用于配置被代表者所看到的网络连接，例如链路 up/down、MTU 等。例如，把代表设备管理性地置为 UP 应使被代表者看到链路 up / 载波 on 事件。)
2. It provides the slow path for traffic which does not hit any offloaded
   fast-path rules in the virtual switch.  Packets transmitted on the
   representor netdevice should be delivered to the representee; packets
   transmitted by the representee which fail to match any switching rule should
   be received on the representor netdevice.  (That is, there is a virtual pipe
   connecting the representor to the representee, similar in concept to a veth
   pair.)
   This allows software switch implementations (such as OpenVSwitch or a Linux
   bridge) to forward packets between representees and the rest of the network.
   (2. 它为未命中虚拟交换机中任何已卸载快速路径规则的流量提供慢路径。在代表 netdevice 上发送的数据包应被送达被代表者；被代表者发送的、未能匹配任何交换规则的数据包应在代表 netdevice 上被接收。（也就是说，有一条虚拟管道连接代表设备与被代表者，概念上类似 veth 对。）
   这样，软件交换实现（如 OpenVSwitch 或 Linux 网桥）就能在多个被代表者与网络其余部分之间转发数据包。)
3. It acts as a handle by which switching rules (such as TC filters) can refer
   to the representee, allowing these rules to be offloaded.
   (3. 它充当一个句柄，交换规则（如 TC 过滤器）可以通过它来引用被代表者，从而允许这些规则被卸载。)

The combination of 2) and 3) means that the behaviour (apart from performance)
should be the same whether a TC filter is offloaded or not.  E.g. a TC rule
on a VF representor applies in software to packets received on that representor
netdevice, while in hardware offload it would apply to packets transmitted by
the representee VF.  Conversely, a mirred egress redirect to a VF representor
corresponds in hardware to delivery directly to the representee VF.
(角色 2) 与 3) 的组合意味着，除性能之外，TC 过滤器无论是否被卸载，行为都应当相同。例如，VF 代表设备上的 TC 规则在软件中应用于该代表 netdevice 上收到的数据包，而在硬件卸载中则应用于被代表 VF 发送的数据包。反过来，指向 VF 代表设备的 mirred egress redirect 在硬件中对应于直接投递给被代表 VF。)

What functions should have a representor?
(哪些功能应当有代表设备？)
-----------------------------------------

Essentially, for each virtual port on the device's internal switch, there
should be a representor.
Some vendors have chosen to omit representors for the uplink and the physical
network port, which can simplify usage (the uplink netdev becomes in effect the
physical port's representor) but does not generalise to devices with multiple
ports or uplinks.
(本质上，设备内部交换机上的每个虚拟端口都应当有一个代表设备。
一些厂商选择不为上行口和物理网络端口提供代表设备，这可以简化使用（上行 netdev 实际上就成了物理端口的代表设备），但无法推广到具有多个端口或上行口的设备。)

Thus, the following should all have representors:
(因此，以下所有功能都应当有代表设备：)

 - VFs belonging to the switchdev function.
   (属于 switchdev 功能的 VF。)
 - Other PFs on the local PCIe controller, and any VFs belonging to them.
   (本地 PCIe 控制器上的其他 PF，以及属于它们的任何 VF。)
 - PFs and VFs on external PCIe controllers on the device (e.g. for any embedded
   System-on-Chip within the SmartNIC).
   (设备上外部 PCIe 控制器上的 PF 和 VF（例如 SmartNIC 内任何嵌入式片上系统）。)
 - PFs and VFs with other personalities, including network block devices (such
   as a vDPA virtio-blk PF backed by remote/distributed storage), if (and only
   if) their network access is implemented through a virtual switch port. [#]_
   Note that such functions can require a representor despite the representee
   not having a netdev.
   (具有其他个性 (personality) 的 PF 和 VF，包括网络块设备（例如由远程/分布式存储支持的 vDPA virtio-blk PF），当且仅当它们的网络访问是通过虚拟交换机端口实现时。注意，即使被代表者没有 netdev，这类功能也可能需要一个代表设备。)
 - Subfunctions (SFs) belonging to any of the above PFs or VFs, if they have
   their own port on the switch (as opposed to using their parent PF's port).
   (属于上述任何 PF 或 VF 的子功能 (SF)，前提是它们在交换机上有自己的端口（而不是使用其父 PF 的端口）。)
 - Any accelerators or plugins on the device whose interface to the network is
   through a virtual switch port, even if they do not have a corresponding PCIe
   PF or VF.
   (设备上任何其到网络的接口经由虚拟交换机端口的加速器或插件，即使它们没有对应的 PCIe PF 或 VF。)

This allows the entire switching behaviour of the NIC to be controlled through
representor TC rules.
(这使得网卡的整个交换行为都可以通过代表设备 TC 规则来控制。)

It is a common misunderstanding to conflate virtual ports with PCIe virtual
functions or their netdevs.  While in simple cases there will be a 1:1
correspondence between VF netdevices and VF representors, more advanced device
configurations may not follow this.
A PCIe function which does not have network access through the internal switch
(not even indirectly through the hardware implementation of whatever services
the function provides) should *not* have a representor (even if it has a
netdev).
Such a function has no switch virtual port for the representor to configure or
to be the other end of the virtual pipe.
The representor represents the virtual port, not the PCIe function nor the 'end
user' netdevice.
(把虚拟端口与 PCIe 虚拟功能或其 netdev 混为一谈是一种常见误解。虽然在简单情况下 VF netdev 与 VF 代表设备之间会有一一对应关系，但更高级的设备配置可能不遵循这一点。
不通过内部交换机访问网络（甚至不通过该功能所提供服务的硬件实现间接访问）的 PCIe 功能 *不* 应有代表设备（即使它有 netdev）。
这样的功能没有可供代表设备配置、或充当虚拟管道另一端的交换机虚拟端口。
代表设备代表的是虚拟端口，而不是 PCIe 功能，也不是"最终用户" netdev。)

.. [#] The concept here is that a hardware IP stack in the device performs the
   translation between block DMA requests and network packets, so that only
   network packets pass through the virtual port onto the switch.  The network
   access that the IP stack "sees" would then be configurable through tc rules;
   e.g. its traffic might all be wrapped in a specific VLAN or VxLAN.  However,
   any needed configuration of the block device *qua* block device, not being a
   networking entity, would not be appropriate for the representor and would
   thus use some other channel such as devlink.
   Contrast this with the case of a virtio-blk implementation which forwards the
   DMA requests unchanged to another PF whose driver then initiates and
   terminates IP traffic in software; in that case the DMA traffic would *not*
   run over the virtual switch and the virtio-blk PF should thus *not* have a
   representor.
   (这里的思路是：设备中的硬件 IP 栈执行块 DMA 请求与网络数据包之间的转换，从而只有网络数据包经过虚拟端口进入交换机。该 IP 栈"看到"的网络访问随后可以通过 tc 规则加以配置；例如其流量可能全部被封装在特定 VLAN 或 VxLAN 中。然而，块设备 *本身* (qua block device) 所需的任何配置，由于不是网络实体，不适合由代表设备进行，因而会使用 devlink 之类的其他通道。
   与此对比的是 virtio-blk 实现：它把 DMA 请求原样转发给另一个 PF，由其驱动在软件中发起并终结 IP 流量；在这种情况下 DMA 流量 *不会* 经过虚拟交换机，因此 virtio-blk PF 相应地 *不应* 有代表设备。)

How are representors created?
(代表设备是如何创建的？)
-----------------------------

The driver instance attached to the switchdev function should, for each virtual
port on the switch, create a pure-software netdevice which has some form of
in-kernel reference to the switchdev function's own netdevice or driver private
data (``netdev_priv()``).
This may be by enumerating ports at probe time, reacting dynamically to the
creation and destruction of ports at run time, or a combination of the two.
(挂接到 switchdev 功能的驱动实例应当为交换机上的每个虚拟端口创建一个纯软件 netdevice，它以某种内核内的形式引用 switchdev 功能自身的 netdevice 或驱动私有数据 (netdev_priv())。
这可能通过在探测时枚举端口、在运行时动态响应端口的创建与销毁，或两者的结合来实现。)

The operations of the representor netdevice will generally involve acting
through the switchdev function.  For example, ``ndo_start_xmit()`` might send
the packet through a hardware TX queue attached to the switchdev function, with
either packet metadata or queue configuration marking it for delivery to the
representee.
(代表 netdevice 的操作通常需要经由 switchdev 功能进行。例如，ndo_start_xmit() 可能通过挂接到 switchdev 功能的硬件发送队列发送数据包，用数据包元数据或队列配置将其标记为投递给被代表者。)

How are representors identified?
(代表设备是如何标识的？)
--------------------------------

The representor netdevice should *not* directly refer to a PCIe device (e.g.
through ``net_dev->dev.parent`` / ``SET_NETDEV_DEV()``), either of the
representee or of the switchdev function.
Instead, the driver should use the ``SET_NETDEV_DEVLINK_PORT`` macro to
assign a devlink port instance to the netdevice before registering the
netdevice; the kernel uses the devlink port to provide the ``phys_switch_id``
and ``phys_port_name`` sysfs nodes.
(Some legacy drivers implement ``ndo_get_port_parent_id()`` and
``ndo_get_phys_port_name()`` directly, but this is deprecated.)  See
:ref:`Documentation/networking/devlink/devlink-port.rst <devlink_port>` for the
details of this API.
(代表 netdevice *不* 应直接引用 PCIe 设备（例如通过 net_dev->dev.parent / SET_NETDEV_DEV()），无论该设备是被代表者的还是 switchdev 功能的。
相反，驱动应使用 SET_NETDEV_DEVLINK_PORT 宏在注册 netdevice 之前把一个 devlink 端口实例赋给它；内核使用该 devlink 端口来提供 phys_switch_id 和 phys_port_name sysfs 节点。
（一些旧式驱动直接实现 ndo_get_port_parent_id() 和 ndo_get_phys_port_name()，但这已被弃用。）此 API 的细节见 :ref:`Documentation/networking/devlink/devlink-port.rst <devlink_port>` 。)

It is expected that userland will use this information (e.g. through udev rules)
to construct an appropriately informative name or alias for the netdevice.  For
instance if the switchdev function is ``eth4`` then a representor with a
``phys_port_name`` of ``p0pf1vf2`` might be renamed ``eth4pf1vf2rep``.
(预期用户态会利用这些信息（例如通过 udev 规则）为 netdevice 构造一个信息恰当的名称或别名。例如，如果 switchdev 功能是 eth4，那么 phys_port_name 为 p0pf1vf2 的代表设备可能被重命名为 eth4pf1vf2rep。)

There are as yet no established conventions for naming representors which do not
correspond to PCIe functions (e.g. accelerators and plugins).
(对于不对应 PCIe 功能的代表设备（例如加速器和插件），目前还没有既定的命名惯例。)

How do representors interact with TC rules?
(代表设备如何与 TC 规则交互？)
-------------------------------------------

Any TC rule on a representor applies (in software TC) to packets received by
that representor netdevice.  Thus, if the delivery part of the rule corresponds
to another port on the virtual switch, the driver may choose to offload it to
hardware, applying it to packets transmitted by the representee.
(代表设备上的任何 TC 规则（在软件 TC 中）都应用于该代表 netdevice 收到的数据包。因此，如果规则的投递部分对应虚拟交换机上的另一个端口，驱动可以选择将其卸载到硬件，把它应用于被代表者发送的数据包。)

Similarly, since a TC mirred egress action targeting the representor would (in
software) send the packet through the representor (and thus indirectly deliver
it to the representee), hardware offload should interpret this as delivery to
the representee.
(类似地，由于以代表设备为目标的 TC mirred egress 动作会（在软件中）把数据包经代表设备发送（从而间接投递给被代表者），硬件卸载应把它解释为投递给被代表者。)

As a simple example, if ``PORT_DEV`` is the physical port representor and
``REP_DEV`` is a VF representor, the following rules::
(举个简单的例子，如果 PORT_DEV 是物理端口代表设备、REP_DEV 是 VF 代表设备，那么以下规则：)

    tc filter add dev $REP_DEV parent ffff: protocol ipv4 flower \
        action mirred egress redirect dev $PORT_DEV
    tc filter add dev $PORT_DEV parent ffff: protocol ipv4 flower skip_sw \
        action mirred egress mirror dev $REP_DEV

would mean that all IPv4 packets from the VF are sent out the physical port, and
all IPv4 packets received on the physical port are delivered to the VF in
addition to ``PORT_DEV``.  (Note that without ``skip_sw`` on the second rule,
the VF would get two copies, as the packet reception on ``PORT_DEV`` would
trigger the TC rule again and mirror the packet to ``REP_DEV``.)
(这意味着来自该 VF 的所有 IPv4 数据包都从物理端口发出，而在物理端口上收到的所有 IPv4 数据包除了投递给 PORT_DEV 之外还会被投递给该 VF。（注意，如果第二条规则没有 skip_sw，该 VF 会收到两份副本，因为在 PORT_DEV 上接收数据包会再次触发 TC 规则并把数据包镜像给 REP_DEV。）

On devices without separate port and uplink representors, ``PORT_DEV`` would
instead be the switchdev function's own uplink netdevice.
(在没有单独端口和上行代表设备的设备上，PORT_DEV 将改为是 switchdev 功能自己的上行 netdev。)

Of course the rules can (if supported by the NIC) include packet-modifying
actions (e.g. VLAN push/pop), which should be performed by the virtual switch.
(当然，这些规则可以（如果网卡支持）包含修改数据包的动作（例如 VLAN push/pop），这些动作应由虚拟交换机执行。)

Tunnel encapsulation and decapsulation are rather more complicated, as they
involve a third netdevice (a tunnel netdev operating in metadata mode, such as
a VxLAN device created with ``ip link add vxlan0 type vxlan external``) and
require an IP address to be bound to the underlay device (e.g. switchdev
function uplink netdev or port representor).  TC rules such as::
(隧道封装与解封装要复杂得多，因为它们涉及第三个 netdevice（以元数据模式运行的隧道 netdev，例如用 ip link add vxlan0 type vxlan external 创建的 VxLAN 设备），并且要求把一个 IP 地址绑定到底层设备（例如 switchdev 功能上行 netdev 或端口代表设备）。如下形式的 TC 规则：)

    tc filter add dev $REP_DEV parent ffff: flower \
        action tunnel_key set id $VNI src_ip $LOCAL_IP dst_ip $REMOTE_IP \
                              dst_port 4789 \
        action mirred egress redirect dev vxlan0
    tc filter add dev vxlan0 parent ffff: flower enc_src_ip $REMOTE_IP \
        enc_dst_ip $LOCAL_IP enc_key_id $VNI enc_dst_port 4789 \
        action tunnel_key unset action mirred egress redirect dev $REP_DEV

where ``LOCAL_IP`` is an IP address bound to ``PORT_DEV``, and ``REMOTE_IP`` is
another IP address on the same subnet, mean that packets sent by the VF should
be VxLAN encapsulated and sent out the physical port (the driver has to deduce
this by a route lookup of ``LOCAL_IP`` leading to ``PORT_DEV``, and also
perform an ARP/neighbour table lookup to find the MAC addresses to use in the
outer Ethernet frame), while UDP packets received on the physical port with UDP
port 4789 should be parsed as VxLAN and, if their VSID matches ``$VNI``,
decapsulated and forwarded to the VF.
(其中 LOCAL_IP 是绑定到 PORT_DEV 的 IP 地址，REMOTE_IP 是同一子网上的另一个 IP 地址。这意味着 VF 发送的数据包应被 VxLAN 封装并从物理端口发出（驱动必须通过对 LOCAL_IP 做路由查找得出通往 PORT_DEV 的路径，并且还要执行 ARP/邻居表查找以得到外层以太网帧要使用的 MAC 地址），而在物理端口上收到的、UDP 端口为 4789 的 UDP 数据包应被解析为 VxLAN，如果其 VSID 匹配 $VNI，则被解封装并转发给该 VF。)

If this all seems complicated, just remember the 'golden rule' of TC offload:
the hardware should ensure the same final results as if the packets were
processed through the slow path, traversed software TC (except ignoring any
``skip_hw`` rules and applying any ``skip_sw`` rules) and were transmitted or
received through the representor netdevices.
(如果这一切看起来很复杂，只需记住 TC 卸载的"黄金法则"：硬件应确保最终结果与以下情形相同——数据包经由慢路径处理，经过软件 TC（除了忽略任何 skip_hw 规则并应用任何 skip_sw 规则之外），并通过代表 netdevice 发送或接收。)

Configuring the representee's MAC
(配置被代表者的 MAC)
---------------------------------

The representee's link state is controlled through the representor.  Setting the
representor administratively UP or DOWN should cause carrier ON or OFF at the
representee.
(被代表者的链路状态通过代表设备控制。把代表设备管理性地置为 UP 或 DOWN，应使被代表者的载波变为 ON 或 OFF。)

Setting an MTU on the representor should cause that same MTU to be reported to
the representee.
(On hardware that allows configuring separate and distinct MTU and MRU values,
the representor MTU should correspond to the representee's MRU and vice-versa.)
(在代表设备上设置 MTU 应使相同的 MTU 被报告给被代表者。
（在允许配置相互独立、各不相同的 MTU 和 MRU 值的硬件上，代表设备的 MTU 应对应被代表者的 MRU，反之亦然。）)

Currently there is no way to use the representor to set the station permanent
MAC address of the representee; other methods available to do this include:
(目前还没有办法使用代表设备来设置被代表者的永久 MAC 地址；可用于此目的的其他方法包括：)

 - legacy SR-IOV (``ip link set DEVICE vf NUM mac LLADDR``)
   (传统 SR-IOV（ip link set DEVICE vf NUM mac LLADDR）)
 - devlink port function (see **devlink-port(8)** and
   :ref:`Documentation/networking/devlink/devlink-port.rst <devlink_port>`)
   (devlink port function（参见 **devlink-port(8)** 和 :ref:`Documentation/networking/devlink/devlink-port.rst <devlink_port>` ）)
