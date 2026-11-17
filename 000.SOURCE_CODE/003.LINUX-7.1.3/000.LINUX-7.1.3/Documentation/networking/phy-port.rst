.. SPDX-License-Identifier: GPL-2.0
.. _phy_port:

==========================
Ethernet ports(以太网端口)
==========================

This document is a basic description of the phy_port infrastructure,
introduced to represent physical interfaces of Ethernet devices.
(本文档是对 phy_port 基础设施的基本描述，该基础设施用于表示以太网设备的
物理接口。)

Without phy_port, we already have quite a lot of information about what the
media-facing interface of a NIC can do and looks like, through the
:c:type:`struct ethtool_link_ksettings <ethtool_link_ksettings>` attributes,
which includes :
(在没有 phy_port 的情况下，通过
:c:type:`struct ethtool_link_ksettings <ethtool_link_ksettings>` 属性，
我们已经掌握了相当多关于网卡面向介质 (media) 接口能做什么、长什么样的
信息，其中包括：)

 - What the NIC can do through the :c:member:`supported` field
   (网卡能做什么，通过 :c:member:`supported` 字段)
 - What the Link Partner advertises through :c:member:`lp_advertising`
   (链路伙伴通告了什么，通过 :c:member:`lp_advertising`)
 - Which features we're advertising through :c:member:`advertising`
   (我们正在通告哪些特性，通过 :c:member:`advertising`)

We also have info about the number of pairs and the PORT type. These settings
are built by aggregating together information reported by various devices that
are sitting on the link :
(我们还有关于线对数量和 PORT 类型的信息。这些设置是把链路上的各个设备
所报告的信息聚合在一起而得到的：)

  - The NIC itself, through the :c:member:`get_link_ksettings` callback
    (网卡本身，通过 :c:member:`get_link_ksettings` 回调)
  - Precise information from the MAC and PCS by using phylink in the MAC driver
    (来自 MAC 和 PCS 的精确信息，通过在 MAC 驱动中使用 phylink 获得)
  - Information reported by the PHY device
    (PHY 设备报告的信息)
  - Information reported by an SFP module (which can itself include a PHY)
    (SFP 模块报告的信息 (该模块自身可以包含一个 PHY))

This model however starts showing its limitations when we consider devices that
have more than one media interface. In such a case, only information about the
actively used interface is reported, and it's not possible to know what the
other interfaces can do. In fact, we have very little information about whether
or not there are any other media interfaces.
(然而，当我们考虑拥有多个介质接口的设备时，这一模型开始显现其局限。
在这种情况下，只有正在使用的接口的信息会被报告，而无法知道其他接口能
做什么。实际上，关于是否存在其他介质接口，我们所知甚少。)

The goal of the phy_port representation is to provide a way of representing a
physical interface of a NIC, regardless of what is driving the port (NIC through
a firmware, SFP module, Ethernet PHY).
(phy_port 表示的目标是提供一种表示网卡物理接口的方法，而不关心是什么在
驱动该端口 (通过固件驱动的网卡、SFP 模块、以太网 PHY)。)

Multi-port interfaces examples(多端口接口示例)
==============================================

Several cases of multi-interface NICs have been observed so far :
(迄今为止已观察到以下几种多接口网卡的情形：)

Internal MII Mux::

  +------------------+
  | SoC              |
  |          +-----+ |           +-----+
  | +-----+  |     |-------------| PHY |
  | | MAC |--| Mux | |   +-----+ +-----+
  | +-----+  |     |-----| SFP |
  |          +-----+ |   +-----+
  +------------------+

(内部 MII 复用器：)

Internal Mux with internal PHY::

  +------------------------+
  | SoC                    |
  |          +-----+ +-----+
  | +-----+  |     |-| PHY |
  | | MAC |--| Mux | +-----+   +-----+
  | +-----+  |     |-----------| SFP |
  |          +-----+       |   +-----+
  +------------------------+

(带内部 PHY 的内部复用器：)

External Mux::

  +---------+
  | SoC     |  +-----+  +-----+
  |         |  |     |--| PHY |
  | +-----+ |  |     |  +-----+
  | | MAC |----| Mux |  +-----+
  | +-----+ |  |     |--| PHY |
  |         |  +-----+  +-----+
  |         |     |
  |    GPIO-------+
  +---------+

(外部复用器：)

Double-port PHY::

  +---------+
  | SoC     | +-----+
  |         | |     |--- RJ45
  | +-----+ | |     |
  | | MAC |---| PHY |   +-----+
  | +-----+ | |     |---| SFP |
  +---------+ +-----+   +-----+

(双端口 PHY：)

phy_port aims at providing a path to support all the above topologies, by
representing the media interfaces in a way that's agnostic to what's driving
the interface. the struct phy_port object has its own set of callback ops, and
will eventually be able to report its own ksettings::

             _____      +------+
            (     )-----| Port |
 +-----+   (       )    +------+
 | MAC |--(   ???   )
 +-----+   (       )    +------+
            (_____)-----| Port |
                        +------+

(phy_port 旨在提供一条支持上述所有拓扑的路径，以一种与驱动该接口者无关
的方式表示介质接口。struct phy_port 对象有自己的一组回调 ops，并且最终
将能够报告自己的 ksettings：)

Next steps(后续步骤)
====================

As of writing this documentation, only ports controlled by PHY devices are
supported. The next steps will be to add the Netlink API to expose these
to userspace and add support for raw ports (controlled by some firmware, and directly
managed by the NIC driver).
(在撰写本文档时，仅支持由 PHY 设备控制的端口。后续步骤将是添加 Netlink
API 以把这些端口暴露给用户空间，并添加对原始端口 (由某个固件控制、由
网卡驱动直接管理) 的支持。)

Another parallel task is the introduction of a MII muxing framework to allow the
control of non-PHY driver multi-port setups.
(另一项并行任务是引入 MII 复用框架，以允许控制非 PHY 驱动的多端口
设置。)
