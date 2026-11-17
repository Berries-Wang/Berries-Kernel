.. SPDX-License-Identifier: GPL-2.0

======================================
IPVLAN Driver HOWTO(IPVLAN 驱动 HOWTO)
======================================

Initial Release:
	Mahesh Bandewar <maheshb AT google.com>
	(初始发布： Mahesh Bandewar <maheshb AT google.com>)

1. Introduction:(1. 简介：)
===========================

This is conceptually very similar to the macvlan driver with one major
exception of using L3 for mux-ing /demux-ing among slaves. This property makes
the master device share the L2 with its slave devices. I have developed this
driver in conjunction with network namespaces and not sure if there is use case
outside of it.
(这在概念上与 macvlan 驱动非常相似，一个主要区别是在各个 slave 之间使用 L3 进行复用/解复用。这一特性使主设备与其 slave 设备共享 L2。本驱动是结合网络命名空间开发的，不确定在命名空间之外是否还有使用场景。)


2. Building and Installation:(2. 构建与安装：)
==============================================

In order to build the driver, please select the config item CONFIG_IPVLAN.
The driver can be built into the kernel (CONFIG_IPVLAN=y) or as a module
(CONFIG_IPVLAN=m).
(要构建本驱动，请选择配置项 CONFIG_IPVLAN。驱动可以编入内核 (CONFIG_IPVLAN=y)，也可以作为模块构建 (CONFIG_IPVLAN=m)。)


3. Configuration:(3. 配置：)
============================

There are no module parameters for this driver and it can be configured
using IProute2/ip utility.
(本驱动没有模块参数，可以使用 IProute2/ip 工具进行配置。)
::

    ip link add link <master> name <slave> type ipvlan [ mode MODE ] [ FLAGS ]
       where
	 MODE: l3 (default) | l3s | l2
	 FLAGS: bridge (default) | private | vepa

e.g.
(例如：)

    (a) Following will create IPvlan link with eth0 as master in
	L3 bridge mode::
	(以下命令将以 eth0 为主设备、在 L3 bridge 模式下创建 IPvlan 链路：)

	  bash# ip link add link eth0 name ipvl0 type ipvlan
    (b) This command will create IPvlan link in L2 bridge mode::
	(以下命令将在 L2 bridge 模式下创建 IPvlan 链路：)

	  bash# ip link add link eth0 name ipvl0 type ipvlan mode l2 bridge

    (c) This command will create an IPvlan device in L2 private mode::
	(以下命令将创建 L2 private 模式的 IPvlan 设备：)

	  bash# ip link add link eth0 name ipvlan type ipvlan mode l2 private

    (d) This command will create an IPvlan device in L2 vepa mode::
	(以下命令将创建 L2 vepa 模式的 IPvlan 设备：)

	  bash# ip link add link eth0 name ipvlan type ipvlan mode l2 vepa


4. Operating modes:(4. 工作模式：)
==================================

IPvlan has two modes of operation - L2 and L3. For a given master device,
you can select one of these two modes and all slaves on that master will
operate in the same (selected) mode. The RX mode is almost identical except
that in L3 mode the slaves won't receive any multicast / broadcast traffic.
L3 mode is more restrictive since routing is controlled from the other (mostly)
default namespace.
(IPvlan 有两种工作模式 - L2 和 L3。对于给定的主设备，可以选择这两种模式之一，该主设备上的所有 slave 都以同一（所选）模式工作。RX 方向的行为几乎完全相同，区别在于 L3 模式下 slave 不会接收任何组播/广播流量。L3 模式限制更多，因为路由由另一个（通常是默认）命名空间控制。)

4.1 L2 mode:(4.1 L2模式)
------------------------

In this mode TX processing happens on the stack instance attached to the
slave device and packets are switched and queued to the master device to send
out. In this mode the slaves will RX/TX multicast and broadcast (if applicable)
as well.
(在此模式下，TX 处理发生在挂接到 slave 设备的协议栈实例上，数据包被交换并排入主设备的队列后发送出去。在此模式下，slave 也会收发组播和广播（如适用）。)

4.2 L3 mode:(4.2 L3模式)
------------------------

In this mode TX processing up to L3 happens on the stack instance attached
to the slave device and packets are switched to the stack instance of the
master device for the L2 processing and routing from that instance will be
used before packets are queued on the outbound device. In this mode the slaves
will not receive nor can send multicast / broadcast traffic.
(在此模式下，slave 设备上协议栈实例的处理最多到 L3，数据包被交换到主设备的协议栈实例做 L2 处理，并在数据包排入出站设备队列之前使用该实例的路由。在此模式下，slave 既不能接收也不能发送组播/广播流量。)

4.3 L3S mode:(4.3 L3S模式)
--------------------------

This is very similar to the L3 mode except that iptables (conn-tracking)
works in this mode and hence it is L3-symmetric (L3s). This will have slightly less
performance but that shouldn't matter since you are choosing this mode over plain-L3
mode to make conn-tracking work.
(这与 L3 模式非常相似，区别在于 iptables（连接跟踪）在此模式下可以工作，因此它是 L3 对称的 (L3s)。这会带来轻微的性能损失，但这无关紧要，因为你选择此模式而不是普通 L3 模式，正是为了让连接跟踪工作。)

5. Mode flags:(5. 模式标志：)
=============================

At this time following mode flags are available
(目前提供以下模式标志)

5.1 bridge:
-----------

This is the default option. To configure the IPvlan port in this mode,
user can choose to either add this option on the command-line or don't specify
anything. This is the traditional mode where slaves can cross-talk among
themselves apart from talking through the master device.
(这是默认选项。要以该模式配置 IPvlan 端口，用户可以在命令行加上此选项，也可以什么都不指定。这是传统模式，slave 除了通过主设备通信外，彼此之间也可以直接互通。)

5.2 private:
------------

If this option is added to the command-line, the port is set in private
mode. i.e. port won't allow cross communication between slaves.
(如果在命令行加上此选项，端口将被设置为 private 模式，即端口不允许 slave 之间互相通信。)

5.3 vepa:
---------

If this is added to the command-line, the port is set in VEPA mode.
i.e. port will offload switching functionality to the external entity as
described in 802.1Qbg
Note: VEPA mode in IPvlan has limitations. IPvlan uses the mac-address of the
master-device, so the packets which are emitted in this mode for the adjacent
neighbor will have source and destination mac same. This will make the switch /
router send the redirect message.
(如果加上此选项，端口将被设置为 VEPA 模式，即端口把交换功能卸载给 802.1Qbg 中描述的外部实体。
注意：IPvlan 的 VEPA 模式有局限性。IPvlan 使用主设备的 MAC 地址，因此在此模式下发给相邻邻居的数据包源 MAC 和目的 MAC 相同。这会导致交换机/路由器发送重定向消息。)

6. What to choose (macvlan vs. ipvlan)?(6. 该选哪个（macvlan 还是 ipvlan）？)
=============================================================================

These two devices are very similar in many regards and the specific use
case could very well define which device to choose. if one of the following
situations defines your use case then you can choose to use ipvlan:
(这两种设备在许多方面非常相似，具体使用场景很可能决定了该如何选择。如果以下情况之一符合你的使用场景，那么可以选择使用 ipvlan：)


(a) The Linux host that is connected to the external switch / router has
    policy configured that allows only one mac per port.
    (连接到外部交换机/路由器的 Linux 主机上配置了只允许每端口一个 MAC 的策略。)
(b) No of virtual devices created on a master exceed the mac capacity and
    puts the NIC in promiscuous mode and degraded performance is a concern.
    (在一个主设备上创建的虚拟设备数量超过了 MAC 容量，导致网卡进入混杂模式，且性能下降是需要关注的问题。)
(c) If the slave device is to be put into the hostile / untrusted network
    namespace where L2 on the slave could be changed / misused.
    (如果要把 slave 设备放入敌对/不受信任的网络命名空间，其中 slave 上的 L2 可能被更改/滥用。)


6. Example configuration:(6. 配置示例)
======================================

::

  +=============================================================+
  |  Host: host1                                                |
  |                                                             |
  |   +----------------------+      +----------------------+    |
  |   |   NS:ns0             |      |  NS:ns1              |    |
  |   |                      |      |                      |    |
  |   |                      |      |                      |    |
  |   |        ipvl0         |      |         ipvl1        |    |
  |   +----------#-----------+      +-----------#----------+    |
  |              #                              #               |
  |              ################################               |
  |                              # eth0                         |
  +==============================#==============================+


(a) Create two network namespaces - ns0, ns1::
    (创建两个网络命名空间 - ns0、ns1：)

	ip netns add ns0
	ip netns add ns1

(b) Create two ipvlan slaves on eth0 (master device)::
    (在 eth0（主设备）上创建两个 ipvlan slave：)

	ip link add link eth0 ipvl0 type ipvlan mode l2
	ip link add link eth0 ipvl1 type ipvlan mode l2

(c) Assign slaves to the respective network namespaces::
    (把各个 slave 分配到相应的网络命名空间：)

	ip link set dev ipvl0 netns ns0
	ip link set dev ipvl1 netns ns1

(d) Now switch to the namespace (ns0 or ns1) to configure the slave devices
    (现在切换到相应命名空间（ns0 或 ns1）以配置 slave 设备)

	- For ns0::
	  (对于 ns0：)

		(1) ip netns exec ns0 bash
		(2) ip link set dev ipvl0 up
		(3) ip link set dev lo up
		(4) ip -4 addr add 127.0.0.1 dev lo
		(5) ip -4 addr add $IPADDR dev ipvl0
		(6) ip -4 route add default via $ROUTER dev ipvl0

	- For ns1::
	  (对于 ns1：)

		(1) ip netns exec ns1 bash
		(2) ip link set dev ipvl1 up
		(3) ip link set dev lo up
		(4) ip -4 addr add 127.0.0.1 dev lo
		(5) ip -4 addr add $IPADDR dev ipvl1
		(6) ip -4 route add default via $ROUTER dev ipvl1
