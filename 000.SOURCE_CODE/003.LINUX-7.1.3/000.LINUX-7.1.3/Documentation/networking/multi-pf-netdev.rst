.. SPDX-License-Identifier: GPL-2.0
.. include:: <isonum.txt>

===============
Multi-PF Netdev
===============

(Multi-PF Netdev)

Contents(目录)
==============

- `Background(背景)`_
  (背景)
- `Overview(概述)`_
  (概述)
- `mlx5 implementation(mlx5 实现)`_
  (mlx5 实现)
- `Channels distribution(通道分布)`_
  (通道分布)
- `Observability(可观测性)`_
  (可观测性)
- `Steering(导流)`_
  (导流)
- `Mutually exclusive features(互斥特性)`_
  (互斥特性)

Background(背景)
================

The Multi-PF NIC technology enables several CPUs within a multi-socket server to connect directly to
the network, each through its own dedicated PCIe interface. Through either a connection harness that
splits the PCIe lanes between two cards or by bifurcating a PCIe slot for a single card. This
results in eliminating the network traffic traversing over the internal bus between the sockets,
significantly reducing overhead and latency, in addition to reducing CPU utilization and increasing
network throughput.
(Multi-PF 网卡技术使多插槽服务器中的多个 CPU 能够直接连接到网络，每个 CPU 通过自己专用的 PCIe 接口连接。连接方式可以是：通过一个在两块卡之间分配 PCIe 通道的连接转接板，或者对一个插槽做 PCIe 分叉 (bifurcation) 来插单块卡。这样就消除了跨插槽内部总线传输的网络流量，显著降低了开销和延迟，同时还降低了 CPU 利用率并提高了网络吞吐量。)

Overview(概述)
==============

The feature adds support for combining multiple PFs of the same port in a Multi-PF environment under
one netdev instance. It is implemented in the netdev layer. Lower-layer instances like pci func,
sysfs entry, and devlink are kept separate.
Passing traffic through different devices belonging to different NUMA sockets saves cross-NUMA
traffic and allows apps running on the same netdev from different NUMAs to still feel a sense of
proximity to the device and achieve improved performance.
(该特性支持在 Multi-PF 环境下把同一端口上的多个 PF 组合到一个 netdev 实例之下。它在 netdev 层实现。pci func、sysfs 条目和 devlink 等较低层的实例保持独立。让流量经过属于不同 NUMA 插槽的不同设备，可以节省跨 NUMA 流量，使运行在同一 netdev 上、位于不同 NUMA 的应用程序仍能感觉贴近设备，从而获得更好的性能。)

mlx5 implementation(mlx5 实现)
==============================

Multi-PF or Socket-direct in mlx5 is achieved by grouping PFs together which belong to the same
NIC and has the socket-direct property enabled, once all PFs are probed, we create a single netdev
to represent all of them, symmetrically, we destroy the netdev whenever any of the PFs is removed.
(mlx5 中的 Multi-PF 或 Socket-direct 是这样实现的：把属于同一块网卡且启用了 socket-direct 属性的 PF 分组在一起；一旦所有 PF 都被探测到，我们就创建一个 netdev 来代表它们全部；对称地，只要任何一个 PF 被移除，我们就销毁该 netdev。)

The netdev network channels are distributed between all devices, a proper configuration would utilize
the correct close NUMA node when working on a certain app/CPU.
(netdev 的网络通道分布在所有设备之间，正确的配置会在某个应用/CPU 工作时利用正确的就近 NUMA 节点。)

We pick one PF to be a primary (leader), and it fills a special role. The other devices
(secondaries) are disconnected from the network at the chip level (set to silent mode). In silent
mode, no south <-> north traffic flowing directly through a secondary PF. It needs the assistance of
the leader PF (east <-> west traffic) to function. All Rx/Tx traffic is steered through the primary
to/from the secondaries.
(我们选择一个 PF 作为主 (primary，即 leader)，它承担特殊角色。其他设备（secondary）在芯片层面与网络断开（置为静默模式）。在静默模式下，没有南北向流量直接经过 secondary PF。它需要主 PF 的协助（东西向流量）才能工作。所有 Rx/Tx 流量都经由主 PF 在主与 secondary 之间导流。)

Currently, we limit the support to PFs only, and up to two PFs (sockets).
(目前，我们把支持范围限制为仅 PF，且最多两个 PF（插槽）。)

Channels distribution(通道分布)
===============================

We distribute the channels between the different PFs to achieve local NUMA node performance
on multiple NUMA nodes.
(我们在不同 PF 之间分配通道，以便在多个 NUMA 节点上获得本地 NUMA 节点的性能。)

Each combined channel works against one specific PF, creating all its datapath queues against it. We
distribute channels to PFs in a round-robin policy.
(每个合并后的通道针对某个特定 PF 工作，针对它创建其所有数据路径队列。我们以轮转 (round-robin) 策略把通道分配给各个 PF。)

::

        Example for 2 PFs and 5 channels:
        +--------+--------+
        | ch idx | PF idx |
        +--------+--------+
        |    0   |    0   |
        |    1   |    1   |
        |    2   |    0   |
        |    3   |    1   |
        |    4   |    0   |
        +--------+--------+


The reason we prefer round-robin is, it is less influenced by changes in the number of channels. The
mapping between a channel index and a PF is fixed, no matter how many channels the user configures.
As the channel stats are persistent across channel's closure, changing the mapping every single time
would turn the accumulative stats less representing of the channel's history.
(我们偏好在轮转策略的原因是：它受通道数量变化的影响较小。无论用户配置多少个通道，通道索引与 PF 之间的映射都是固定的。由于通道统计信息在通道关闭后仍然保留，如果每次都改变映射，累积统计信息就不能很好地反映该通道的历史。)

This is achieved by using the correct core device instance (mdev) in each channel, instead of them
all using the same instance under "priv->mdev".
(这是通过在每个通道中使用正确的核心设备实例 (mdev) 实现的，而不是让所有通道都使用 "priv->mdev" 下的同一个实例。)

Observability(可观测性)
=======================

The relation between PF, irq, napi, and queue can be observed via netlink spec::
(PF、irq、napi 和队列之间的关系可以通过 netlink spec 观察：)

  $ ./tools/net/ynl/pyynl/cli.py --spec Documentation/netlink/specs/netdev.yaml --dump queue-get --json='{"ifindex": 13}'
  [{'id': 0, 'ifindex': 13, 'napi-id': 539, 'type': 'rx'},
   {'id': 1, 'ifindex': 13, 'napi-id': 540, 'type': 'rx'},
   {'id': 2, 'ifindex': 13, 'napi-id': 541, 'type': 'rx'},
   {'id': 3, 'ifindex': 13, 'napi-id': 542, 'type': 'rx'},
   {'id': 4, 'ifindex': 13, 'napi-id': 543, 'type': 'rx'},
   {'id': 0, 'ifindex': 13, 'napi-id': 539, 'type': 'tx'},
   {'id': 1, 'ifindex': 13, 'napi-id': 540, 'type': 'tx'},
   {'id': 2, 'ifindex': 13, 'napi-id': 541, 'type': 'tx'},
   {'id': 3, 'ifindex': 13, 'napi-id': 542, 'type': 'tx'},
   {'id': 4, 'ifindex': 13, 'napi-id': 543, 'type': 'tx'}]

  $ ./tools/net/ynl/pyynl/cli.py --spec Documentation/netlink/specs/netdev.yaml --dump napi-get --json='{"ifindex": 13}'
  [{'id': 543, 'ifindex': 13, 'irq': 42},
   {'id': 542, 'ifindex': 13, 'irq': 41},
   {'id': 541, 'ifindex': 13, 'irq': 40},
   {'id': 540, 'ifindex': 13, 'irq': 39},
   {'id': 539, 'ifindex': 13, 'irq': 36}]

Here you can clearly observe our channels distribution policy::
(这里可以清楚地观察到我们的通道分布策略：)

  $ ls /proc/irq/{36,39,40,41,42}/mlx5* -d -1
  /proc/irq/36/mlx5_comp0@pci:0000:08:00.0
  /proc/irq/39/mlx5_comp0@pci:0000:09:00.0
  /proc/irq/40/mlx5_comp1@pci:0000:08:00.0
  /proc/irq/41/mlx5_comp1@pci:0000:09:00.0
  /proc/irq/42/mlx5_comp2@pci:0000:08:00.0

Steering(导流)
==============

Secondary PFs are set to "silent" mode, meaning they are disconnected from the network.
(Secondary PF 被置为"静默"模式，即它们与网络断开。)

In Rx, the steering tables belong to the primary PF only, and it is its role to distribute incoming
traffic to other PFs, via cross-vhca steering capabilities. Still maintain a single default RSS table,
that is capable of pointing to the receive queues of a different PF.
(在 Rx 方向，导流表只属于主 PF，它的职责是通过跨 vhca 导流能力把入站流量分配给其他 PF。同时仍维护一个单一的默认 RSS 表，该表能够指向不同 PF 的接收队列。)

In Tx, the primary PF creates a new Tx flow table, which is aliased by the secondaries, so they can
go out to the network through it.
(在 Tx 方向，主 PF 创建一个新的 Tx 流表，secondary 通过别名 (alias) 引用它，从而经由它发往网络。)

In addition, we set default XPS configuration that, based on the CPU, selects an SQ belonging to the
PF on the same node as the CPU.
(此外，我们设置了默认的 XPS 配置：根据 CPU，选择属于与该 CPU 位于同一节点上的 PF 的 SQ。)

XPS default config example:
(XPS 默认配置示例：)

NUMA node(s):          2
NUMA node0 CPU(s):     0-11
NUMA node1 CPU(s):     12-23
(NUMA 节点数：2；NUMA 节点 0 CPU：0-11；NUMA 节点 1 CPU：12-23。)

PF0 on node0, PF1 on node1.
(PF0 在节点 0 上，PF1 在节点 1 上。)

- /sys/class/net/eth2/queues/tx-0/xps_cpus:000001
- /sys/class/net/eth2/queues/tx-1/xps_cpus:001000
- /sys/class/net/eth2/queues/tx-2/xps_cpus:000002
- /sys/class/net/eth2/queues/tx-3/xps_cpus:002000
- /sys/class/net/eth2/queues/tx-4/xps_cpus:000004
- /sys/class/net/eth2/queues/tx-5/xps_cpus:004000
- /sys/class/net/eth2/queues/tx-6/xps_cpus:000008
- /sys/class/net/eth2/queues/tx-7/xps_cpus:008000
- /sys/class/net/eth2/queues/tx-8/xps_cpus:000010
- /sys/class/net/eth2/queues/tx-9/xps_cpus:010000
- /sys/class/net/eth2/queues/tx-10/xps_cpus:000020
- /sys/class/net/eth2/queues/tx-11/xps_cpus:020000
- /sys/class/net/eth2/queues/tx-12/xps_cpus:000040
- /sys/class/net/eth2/queues/tx-13/xps_cpus:040000
- /sys/class/net/eth2/queues/tx-14/xps_cpus:000080
- /sys/class/net/eth2/queues/tx-15/xps_cpus:080000
- /sys/class/net/eth2/queues/tx-16/xps_cpus:000100
- /sys/class/net/eth2/queues/tx-17/xps_cpus:100000
- /sys/class/net/eth2/queues/tx-18/xps_cpus:000200
- /sys/class/net/eth2/queues/tx-19/xps_cpus:200000
- /sys/class/net/eth2/queues/tx-20/xps_cpus:000400
- /sys/class/net/eth2/queues/tx-21/xps_cpus:400000
- /sys/class/net/eth2/queues/tx-22/xps_cpus:000800
- /sys/class/net/eth2/queues/tx-23/xps_cpus:800000

Mutually exclusive features(互斥特性)
=====================================

The nature of Multi-PF, where different channels work with different PFs, conflicts with
stateful features where the state is maintained in one of the PFs.
For example, in the TLS device-offload feature, special context objects are created per connection
and maintained in the PF.  Transitioning between different RQs/SQs would break the feature. Hence,
we disable this combination for now.
(Multi-PF 的特性是不同通道与不同 PF 协同工作，这与状态保存在某个 PF 中的有状态 (stateful) 特性相冲突。例如，在 TLS 设备卸载特性中，会为每个连接创建特殊的上下文对象并保存在 PF 中。在不同的 RQ/SQ 之间切换会破坏该特性。因此，我们暂时禁用了这种组合。)
