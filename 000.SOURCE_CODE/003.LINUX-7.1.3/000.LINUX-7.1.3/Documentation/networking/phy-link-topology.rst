.. SPDX-License-Identifier: GPL-2.0
.. _phy_link_topology:

===============================
PHY link topology(PHY 链路拓扑)
===============================

Overview(概述)
==============

The PHY link topology representation in the networking stack aims at representing
the hardware layout for any given Ethernet link.
(网络协议栈中的 PHY 链路拓扑表示，旨在表示任意给定以太网链路的硬件布局。)

An Ethernet interface from userspace's point of view is nothing but a
:c:type:`struct net_device <net_device>`, which exposes configuration options
through the legacy ioctls and the ethtool netlink commands. The base assumption
when designing these configuration APIs were that the link looks something like ::
(从用户空间的角度看，以太网接口不过是一个
:c:type:`struct net_device <net_device>`，它通过传统 ioctl 和 ethtool
netlink 命令暴露配置选项。设计这些配置 API 时的基本假设是，链路的形态
大致如下：)

  +-----------------------+        +----------+      +--------------+
  | Ethernet Controller / |        | Ethernet |      | Connector /  |
  |       MAC             | ------ |   PHY    | ---- |    Port      | ---... to LP
  +-----------------------+        +----------+      +--------------+
  struct net_device               struct phy_device

Commands that needs to configure the PHY will go through the net_device.phydev
field to reach the PHY and perform the relevant configuration.
(需要配置 PHY 的命令将通过 net_device.phydev 字段到达 PHY，并执行相关的
配置。)

This assumption falls apart in more complex topologies that can arise when,
for example, using SFP transceivers (although that's not the only specific case).
(在更复杂的拓扑下，这一假设不再成立。例如使用 SFP 收发器时 (尽管这并非
唯一的特定场景)。)

Here, we have 2 basic scenarios. Either the MAC is able to output a serialized
interface, that can directly be fed to an SFP cage, such as SGMII, 1000BaseX,
10GBaseR, etc.
(这里有两种基本场景。其一，MAC 能够输出串行化接口，可以直接馈入 SFP 笼，
例如 SGMII、1000BaseX、10GBaseR 等。)

The link topology then looks like this (when an SFP module is inserted) ::
(此时的链路拓扑如下所示 (在插入 SFP 模块时)：)

  +-----+  SGMII  +------------+
  | MAC | ------- | SFP Module |
  +-----+         +------------+

Knowing that some modules embed a PHY, the actual link is more like ::
(考虑到有些模块内嵌 PHY，实际链路更像是：)

  +-----+  SGMII   +--------------+
  | MAC | -------- | PHY (on SFP) |
  +-----+          +--------------+

In this case, the SFP PHY is handled by phylib, and registered by phylink through
its SFP upstream ops.
(这种情况下，SFP PHY 由 phylib 处理，并由 phylink 通过其 SFP 上游 ops
注册。)

Now some Ethernet controllers aren't able to output a serialized interface, so
we can't directly connect them to an SFP cage. However, some PHYs can be used
as media-converters, to translate the non-serialized MAC MII interface to a
serialized MII interface fed to the SFP ::
(如今有些以太网控制器无法输出串行化接口，因此无法把它们直接连接到
SFP 笼。不过，有些 PHY 可以用作介质转换器，把非串行化的 MAC MII 接口
转换为馈入 SFP 的串行化 MII 接口：)

  +-----+  RGMII  +-----------------------+  SGMII  +--------------+
  | MAC | ------- | PHY (media converter) | ------- | PHY (on SFP) |
  +-----+         +-----------------------+         +--------------+

This is where the model of having a single net_device.phydev pointer shows its
limitations, as we now have 2 PHYs on the link.
(这就是只有一个 net_device.phydev 指针的模型显现其局限之处，因为现在
链路上有 2 个 PHY。)

The phy_link topology framework aims at providing a way to keep track of every
PHY on the link, for use by both kernel drivers and subsystems, but also to
report the topology to userspace, allowing to target individual PHYs in configuration
commands.
(phy_link 拓扑框架旨在提供一种跟踪链路上每个 PHY 的方法，供内核驱动和
子系统使用，同时也把拓扑报告给用户空间，从而允许在配置命令中定位单个
PHY。)

API
===

The :c:type:`struct phy_link_topology <phy_link_topology>` is a per-netdevice
resource, that gets initialized at netdevice creation. Once it's initialized,
it is then possible to register PHYs to the topology through :

:c:func:`phy_link_topo_add_phy`

(:c:type:`struct phy_link_topology <phy_link_topology>` 是一个 per-netdevice
资源，在 netdevice 创建时被初始化。一旦初始化完成，就可以通过以下调用把
PHY 注册到拓扑：)

Besides registering the PHY to the topology, this call will also assign a unique
index to the PHY, which can then be reported to userspace to refer to this PHY
(akin to the ifindex). This index is a u32, ranging from 1 to U32_MAX. The value
0 is reserved to indicate the PHY doesn't belong to any topology yet.
(除了把 PHY 注册到拓扑之外，该调用还会为 PHY 分配一个唯一索引，随后可以
把该索引报告给用户空间以引用这个 PHY (类似于 ifindex)。该索引是 u32
类型，范围从 1 到 U32_MAX。值 0 被保留，用于表示该 PHY 尚不属于任何
拓扑。)

The PHY can then be removed from the topology through

:c:func:`phy_link_topo_del_phy`

(之后可以通过以下调用把 PHY 从拓扑中移除)

These function are already hooked into the phylib subsystem, so all PHYs that
are linked to a net_device through :c:func:`phy_attach_direct` will automatically
join the netdev's topology.
(这些函数已经挂接进 phylib 子系统，因此所有通过
:c:func:`phy_attach_direct` 链接到 net_device 的 PHY 都会自动加入该
netdev 的拓扑。)

PHYs that are on a SFP module will also be automatically registered IF the SFP
upstream is phylink (so, no media-converter).
(位于 SFP 模块上的 PHY 也会被自动注册，前提是 SFP 上游是 phylink (即
没有介质转换器)。)

PHY drivers that can be used as SFP upstream need to call :c:func:`phy_sfp_attach_phy`
and :c:func:`phy_sfp_detach_phy`, which can be used as a
.attach_phy / .detach_phy implementation for the
:c:type:`struct sfp_upstream_ops <sfp_upstream_ops>`.
(可用作 SFP 上游的 PHY 驱动需要调用 :c:func:`phy_sfp_attach_phy` 和
:c:func:`phy_sfp_detach_phy`，它们可以用作
:c:type:`struct sfp_upstream_ops <sfp_upstream_ops>` 的
.attach_phy / .detach_phy 实现。)

UAPI
====

There exist a set of netlink commands to query the link topology from userspace,
see ``Documentation/networking/ethtool-netlink.rst``.
(存在一组用于从用户空间查询链路拓扑的 netlink 命令，参见
``Documentation/networking/ethtool-netlink.rst``。)

The whole point of having a topology representation is to assign the phyindex
field in :c:type:`struct phy_device <phy_device>`. This index is reported to
userspace using the ``ETHTOOL_MSG_PHY_GET`` ethtnl command. Performing a DUMP operation
will result in all PHYs from all net_device being listed. The DUMP command
accepts either a ``ETHTOOL_A_HEADER_DEV_INDEX`` or ``ETHTOOL_A_HEADER_DEV_NAME``
to be passed in the request to filter the DUMP to a single net_device.
(拥有拓扑表示的全部意义就在于给 :c:type:`struct phy_device <phy_device>`
中的 phyindex 字段赋值。该索引通过 ``ETHTOOL_MSG_PHY_GET`` ethtnl 命令
报告给用户空间。执行 DUMP 操作会列出所有 net_device 的所有 PHY。DUMP
命令接受在请求中传入 ``ETHTOOL_A_HEADER_DEV_INDEX`` 或
``ETHTOOL_A_HEADER_DEV_NAME``，以把 DUMP 过滤到单个 net_device。)

The retrieved index can then be passed as a request parameter using the
``ETHTOOL_A_HEADER_PHY_INDEX`` field in the following ethnl commands :
(获取到的索引随后可以作为请求参数，通过以下 ethnl 命令中的
``ETHTOOL_A_HEADER_PHY_INDEX`` 字段传入：)

* ``ETHTOOL_MSG_STRSET_GET`` to get the stats string set from a given PHY
  (``ETHTOOL_MSG_STRSET_GET`` 用于从给定的 PHY 获取统计字符串集)
* ``ETHTOOL_MSG_CABLE_TEST_ACT`` and ``ETHTOOL_MSG_CABLE_TEST_ACT``, to perform
  cable testing on a given PHY on the link (most likely the outermost PHY)
  (``ETHTOOL_MSG_CABLE_TEST_ACT`` 和 ``ETHTOOL_MSG_CABLE_TEST_ACT``，
  对链路上给定的 PHY (很可能是最外侧的 PHY) 执行线缆测试)
* ``ETHTOOL_MSG_PSE_SET`` and ``ETHTOOL_MSG_PSE_GET`` for PHY-controlled PoE and PSE settings
  (``ETHTOOL_MSG_PSE_SET`` 和 ``ETHTOOL_MSG_PSE_GET`` 用于由 PHY 控制的
  PoE 和 PSE 设置)
* ``ETHTOOL_MSG_PLCA_GET_CFG``, ``ETHTOOL_MSG_PLCA_SET_CFG`` and ``ETHTOOL_MSG_PLCA_GET_STATUS``
  to set the PLCA (Physical Layer Collision Avoidance) parameters
  (``ETHTOOL_MSG_PLCA_GET_CFG``、``ETHTOOL_MSG_PLCA_SET_CFG`` 和
  ``ETHTOOL_MSG_PLCA_GET_STATUS`` 用于设置 PLCA (物理层冲突避免) 参数)

Note that the PHY index can be passed to other requests, which will silently
ignore it if present and irrelevant.
(注意，PHY 索引也可以传给其他请求；如果它存在但不相关，这些请求会静默
忽略它。)
