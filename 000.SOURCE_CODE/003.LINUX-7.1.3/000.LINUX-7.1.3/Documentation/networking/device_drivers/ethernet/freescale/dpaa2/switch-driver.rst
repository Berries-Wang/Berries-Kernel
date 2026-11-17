.. SPDX-License-Identifier: GPL-2.0
.. include:: <isonum.txt>

=====================================
DPAA2 Switch driver(DPAA2 交换机驱动)
=====================================

:Copyright: |copy| 2021 NXP
(版权所有: |copy| 2021 NXP)

The DPAA2 Switch driver probes on the Datapath Switch (DPSW) object which can
be instantiated on the following DPAA2 SoCs and their variants: LS2088A and
LX2160A.
(DPAA2 交换机驱动探测数据路径交换机 (DPSW) 对象，该对象可在以下 DPAA2 SoC 及其衍生型号上实例化：LS2088A
和 LX2160A。)

The driver uses the switch device driver model and exposes each switch port as
a network interface, which can be included in a bridge or used as a standalone
interface. Traffic switched between ports is offloaded into the hardware.
(该驱动使用交换机设备驱动模型，将每个交换机端口暴露为一个网络接口，该接口可加入网桥，也可作为独立接口使用。
端口之间交换的流量被卸载 (offload) 到硬件中。)

The DPSW can have ports connected to DPNIs or to DPMACs for external access.
(DPSW 的端口可以连接到 DPNI，也可以连接到 DPMAC 以访问外部。)
::

         [ethA]     [ethB]      [ethC]     [ethD]     [ethE]     [ethF]
            :          :          :          :          :          :
            :          :          :          :          :          :
       [dpaa2-eth]  [dpaa2-eth]  [              dpaa2-switch              ]
            :          :          :          :          :          :        kernel
       =============================================================================
            :          :          :          :          :          :        hardware
         [DPNI]      [DPNI]     [============= DPSW =================]
            |          |          |          |          |          |
            |           ----------           |       [DPMAC]    [DPMAC]
             -------------------------------            |          |
                                                        |          |
                                                      [PHY]      [PHY]

Creating an Ethernet Switch(创建以太网交换机)
=============================================

The dpaa2-switch driver probes on DPSW devices found on the fsl-mc bus. These
devices can be either created statically through the boot time configuration
file - DataPath Layout (DPL) - or at runtime using the DPAA2 object APIs
(incorporated already into the restool userspace tool).
(dpaa2-switch 驱动探测 fsl-mc 总线上的 DPSW 设备。这些设备既可以静态地通过启动时配置文件——数据路径布局
(DataPath Layout, DPL)——创建，也可以在运行时使用 DPAA2 对象 API 创建（这些 API 已并入 restool 用户空间工
具）。)

At the moment, the dpaa2-switch driver imposes the following restrictions on
the DPSW object that it will probe:
(目前，dpaa2-switch 驱动对将要探测的 DPSW 对象施加以下限制：)

 * The minimum number of FDBs should be at least equal to the number of switch
   interfaces. This is necessary so that separation of switch ports can be
   done, ie when not under a bridge, each switch port will have its own FDB.
   (FDB 的最小数量应至少等于交换机接口的数量。这是实现交换机端口隔离所必需的，即当端口不在网桥之下时，每个交
   换机端口都拥有自己的 FDB。)
   ::

        fsl_dpaa2_switch dpsw.0: The number of FDBs is lower than the number of ports, cannot probe

 * Both the broadcast and flooding configuration should be per FDB. This
   enables the driver to restrict the broadcast and flooding domains of each
   FDB depending on the switch ports that are sharing it (aka are under the
   same bridge).
   (广播和泛洪配置都应当按 FDB 各自独立。这使得驱动能够根据共享同一 FDB 的交换机端口（即处于同一网桥之下）
   来限定每个 FDB 的广播域和泛洪域。)
   ::

        fsl_dpaa2_switch dpsw.0: Flooding domain is not per FDB, cannot probe
        fsl_dpaa2_switch dpsw.0: Broadcast domain is not per FDB, cannot probe

 * The control interface of the switch should not be disabled
   (DPSW_OPT_CTRL_IF_DIS not passed as a create time option). Without the
   control interface, the driver is not capable to provide proper Rx/Tx traffic
   support on the switch port netdevices.
   (交换机的控制接口不应被禁用（即创建时不传入 DPSW_OPT_CTRL_IF_DIS 选项）。没有控制接口，驱动就无法在交换
   机端口网络设备上提供正确的 Rx/Tx 流量支持。)
   ::

        fsl_dpaa2_switch dpsw.0: Control Interface is disabled, cannot probe

Besides the configuration of the actual DPSW object, the dpaa2-switch driver
will need the following DPAA2 objects:
(除了实际 DPSW 对象的配置之外，dpaa2-switch 驱动还需要以下 DPAA2 对象：)

 * 1 DPMCP - A Management Command Portal object is needed for any interaction
   with the MC firmware.
   (1 个 DPMCP - 与 MC 固件的任何交互都需要一个管理命令门户 (Management Command Portal) 对象。)

 * 1 DPBP - A Buffer Pool is used for seeding buffers intended for the Rx path
   on the control interface.
   (1 个 DPBP - 缓冲池 (Buffer Pool) 用于为控制接口 Rx 路径预备缓冲区。)

 * Access to at least one DPIO object (Software Portal) is needed for any
   enqueue/dequeue operation to be performed on the control interface queues.
   The DPIO object will be shared, no need for a private one.
   (要在控制接口队列上执行任何入队/出队操作，都需要访问至少一个 DPIO 对象（软件门户）。该 DPIO 对象是共享
   的，无需私有的。)

Switching features(交换功能)
============================

The driver supports the configuration of L2 forwarding rules in hardware for
port bridging as well as standalone usage of the independent switch interfaces.
(该驱动支持在硬件中配置 L2 转发规则，既用于端口桥接，也用于独立交换机接口的单独使用。)

The hardware is not configurable with respect to VLAN awareness, thus any DPAA2
switch port should be used only in usecases with a VLAN aware bridge::
(硬件在 VLAN 感知方面不可配置，因此任何 DPAA2 交换机端口都只应在带有 VLAN 感知网桥的使用场景中使用：)

        $ ip link add dev br0 type bridge vlan_filtering 1

        $ ip link add dev br1 type bridge
        $ ip link set dev ethX master br1
        Error: fsl_dpaa2_switch: Cannot join a VLAN-unaware bridge

Topology and loop detection through STP is supported when ``stp_state 1`` is
used at bridge create ::
(在创建网桥时使用 ``stp_state 1`` 的情况下，支持通过 STP 进行拓扑与环路检测：)

        $ ip link add dev br0 type bridge vlan_filtering 1 stp_state 1

L2 FDB manipulation (add/delete/dump) is supported.
(支持 L2 FDB 操作（添加/删除/转储）。)

HW FDB learning can be configured on each switch port independently through
bridge commands. When the HW learning is disabled, a fast age procedure will be
run and any previously learnt addresses will be removed.
(可以通过网桥命令在每个交换机端口上独立配置硬件 FDB 学习。当硬件学习被禁用时，会执行一次快速老化 (fast age)
过程，之前学到的所有地址都将被移除。)
::

        $ bridge link set dev ethX learning off
        $ bridge link set dev ethX learning on

Restricting the unknown unicast and multicast flooding domain is supported, but
not independently of each other::
(支持限制未知单播和多播泛洪域，但二者不能彼此独立地进行限制：)

        $ ip link set dev ethX type bridge_slave flood off mcast_flood off
        $ ip link set dev ethX type bridge_slave flood off mcast_flood on
        Error: fsl_dpaa2_switch: Cannot configure multicast flooding independently of unicast.

Broadcast flooding on a switch port can be disabled/enabled through the brport sysfs::
(交换机端口上的广播泛洪可以通过 brport sysfs 禁用/启用：)

        $ echo 0 > /sys/bus/fsl-mc/devices/dpsw.Y/net/ethX/brport/broadcast_flood

Offloads(卸载)
==============

Routing actions (redirect, trap, drop)(路由动作（重定向、陷阱、丢弃）)
----------------------------------------------------------------------

The DPAA2 switch is able to offload flow-based redirection of packets making
use of ACL tables. Shared filter blocks are supported by sharing a single ACL
table between multiple ports.
(DPAA2 交换机能够利用 ACL 表将基于流的报文重定向卸载到硬件。通过在多个端口之间共享单个 ACL 表，可以支持共
享过滤器块。)

The following flow keys are supported:
(支持以下流键值 (flow key)：)

 * Ethernet: dst_mac/src_mac
   (以太网: dst_mac/src_mac)
 * IPv4: dst_ip/src_ip/ip_proto/tos
   (IPv4: dst_ip/src_ip/ip_proto/tos)
 * VLAN: vlan_id/vlan_prio/vlan_tpid/vlan_dei
   (VLAN: vlan_id/vlan_prio/vlan_tpid/vlan_dei)
 * L4: dst_port/src_port
   (L4: dst_port/src_port)

Also, the matchall filter can be used to redirect the entire traffic received
on a port.
(此外，可以使用 matchall 过滤器重定向某个端口上收到的全部流量。)

As per flow actions, the following are supported:
(就流动作而言，支持以下几种：)

 * drop
   (丢弃)
 * mirred egress redirect
   (mirred 出方向重定向)
 * trap
   (陷阱上送 CPU)

Each ACL entry (filter) can be setup with only one of the listed
actions.
(每个 ACL 表项（过滤器）只能设置为所列动作中的一种。)

Example 1: send frames received on eth4 with a SA of 00:01:02:03:04:05 to the
CPU::
(示例 1：把 eth4 上收到的源地址为 00:01:02:03:04:05 的帧发送到 CPU：)

        $ tc qdisc add dev eth4 clsact
        $ tc filter add dev eth4 ingress flower src_mac 00:01:02:03:04:05 skip_sw action trap

Example 2: drop frames received on eth4 with VID 100 and PCP of 3::
(示例 2：丢弃 eth4 上收到的 VID 为 100 且 PCP 为 3 的帧：)

        $ tc filter add dev eth4 ingress protocol 802.1q flower skip_sw vlan_id 100 vlan_prio 3 action drop

Example 3: redirect all frames received on eth4 to eth1::
(示例 3：把 eth4 上收到的所有帧重定向到 eth1：)

        $ tc filter add dev eth4 ingress matchall action mirred egress redirect dev eth1

Example 4: Use a single shared filter block on both eth5 and eth6::
(示例 4：在 eth5 和 eth6 上使用同一个共享过滤器块：)

        $ tc qdisc add dev eth5 ingress_block 1 clsact
        $ tc qdisc add dev eth6 ingress_block 1 clsact
        $ tc filter add block 1 ingress flower dst_mac 00:01:02:03:04:04 skip_sw \
                action trap
        $ tc filter add block 1 ingress protocol ipv4 flower src_ip 192.168.1.1 skip_sw \
                action mirred egress redirect dev eth3

Mirroring(镜像)
~~~~~~~~~~~~~~~

The DPAA2 switch supports only per port mirroring and per VLAN mirroring.
Adding mirroring filters in shared blocks is also supported.
(DPAA2 交换机只支持按端口镜像和按 VLAN 镜像。在共享块中添加镜像过滤器也是支持的。)

When using the tc-flower classifier with the 802.1q protocol, only the
''vlan_id'' key will be accepted. Mirroring based on any other fields from the
802.1q protocol will be rejected::
(当 tc-flower 分类器使用 802.1q 协议时，只接受 ''vlan_id'' 这个键。基于 802.1q 协议其他字段的镜像都会被拒
绝：)

        $ tc qdisc add dev eth8 ingress_block 1 clsact
        $ tc filter add block 1 ingress protocol 802.1q flower skip_sw vlan_prio 3 action mirred egress mirror dev eth6
        Error: fsl_dpaa2_switch: Only matching on VLAN ID supported.
        We have an error talking to the kernel

If a mirroring VLAN filter is requested on a port, the VLAN must to be
installed on the switch port in question either using ''bridge'' or by creating
a VLAN upper device if the switch port is used as a standalone interface::
(如果在某个端口上请求基于 VLAN 的镜像过滤器，那么该 VLAN 必须安装在相应的交换机端口上：使用 ''bridge'' 命
令，或者当交换机端口作为独立接口使用时，创建一个 VLAN 上层设备：)

        $ tc qdisc add dev eth8 ingress_block 1 clsact
        $ tc filter add block 1 ingress protocol 802.1q flower skip_sw vlan_id 200 action mirred egress mirror dev eth6
        Error: VLAN must be installed on the switch port.
        We have an error talking to the kernel

        $ bridge vlan add vid 200 dev eth8
        $ tc filter add block 1 ingress protocol 802.1q flower skip_sw vlan_id 200 action mirred egress mirror dev eth6

        $ ip link add link eth8 name eth8.200 type vlan id 200
        $ tc filter add block 1 ingress protocol 802.1q flower skip_sw vlan_id 200 action mirred egress mirror dev eth6

Also, it should be noted that the mirrored traffic will be subject to the same
egress restrictions as any other traffic. This means that when a mirrored
packet will reach the mirror port, if the VLAN found in the packet is not
installed on the port it will get dropped.
(另外需要注意的是，镜像流量将与其他任何流量一样受相同的出方向 (egress) 限制。这意味着当镜像报文到达镜像端
口时，如果报文中的 VLAN 未安装在该端口上，报文将被丢弃。)

The DPAA2 switch supports only a single mirroring destination, thus multiple
mirror rules can be installed but their ''to'' port has to be the same::
(DPAA2 交换机只支持单个镜像目的地，因此可以安装多条镜像规则，但它们的 ''to'' 端口必须是同一个：)

        $ tc filter add block 1 ingress protocol 802.1q flower skip_sw vlan_id 200 action mirred egress mirror dev eth6
        $ tc filter add block 1 ingress protocol 802.1q flower skip_sw vlan_id 100 action mirred egress mirror dev eth7
        Error: fsl_dpaa2_switch: Multiple mirror ports not supported.
        We have an error talking to the kernel
