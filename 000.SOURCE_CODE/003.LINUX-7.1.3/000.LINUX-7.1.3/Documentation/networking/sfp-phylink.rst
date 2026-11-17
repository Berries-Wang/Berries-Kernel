.. SPDX-License-Identifier: GPL-2.0

=======
phylink
=======

(phylink)

Overview
(概述)
========

phylink is a mechanism to support hot-pluggable networking modules
directly connected to a MAC without needing to re-initialise the
adapter on hot-plug events.
(phylink 是一种支持可热插拔网络模块直接连接到 MAC 的机制，在热插拔事件发生时
无需重新初始化适配器。)

phylink supports conventional phylib-based setups, fixed link setups
and SFP (Small Formfactor Pluggable) modules at present.
(目前，phylink 支持常规的基于 phylib 的配置、固定链路 (fixed link) 配置以及
SFP (Small Formfactor Pluggable) 模块。)

Modes of operation
(操作模式)
==================

phylink has several modes of operation, which depend on the firmware
settings.
(phylink 有几种操作模式，具体取决于固件设置。)

1. PHY mode
   (PHY 模式)

   In PHY mode, we use phylib to read the current link settings from
   the PHY, and pass them to the MAC driver.  We expect the MAC driver
   to configure exactly the modes that are specified without any
   negotiation being enabled on the link.
   (在 PHY 模式下，我们使用 phylib 从 PHY 读取当前链路设置并传递给 MAC 驱动。
   我们期望 MAC 驱动精确配置所指定的模式，而不在链路上启用任何协商。)

2. Fixed mode
   (Fixed 模式)

   Fixed mode is the same as PHY mode as far as the MAC driver is
   concerned.
   (就 MAC 驱动而言，Fixed 模式与 PHY 模式相同。)

3. In-band mode
   (带内 (in-band) 模式)

   In-band mode is used with 802.3z, SGMII and similar interface modes,
   and we are expecting to use and honor the in-band negotiation or
   control word sent across the serdes channel.
   (带内模式用于 802.3z、SGMII 及类似的接口模式，我们期望使用并遵从通过
   serdes 通道传送的带内协商或控制字。)

By example, what this means is that:
(举例来说，这意味着：)

.. code-block:: none

  &eth {
    phy = <&phy>;
    phy-mode = "sgmii";
  };

does not use in-band SGMII signalling.  The PHY is expected to follow
exactly the settings given to it in its :c:func:`mac_config` function.
The link should be forced up or down appropriately in the
:c:func:`mac_link_up` and :c:func:`mac_link_down` functions.
(不使用带内 SGMII 信令。期望 PHY 严格遵循其 :c:func:`mac_config` 函数中给出的
设置。链路应在 :c:func:`mac_link_up` 与 :c:func:`mac_link_down` 函数中被适当地
强制置为 up 或 down。)

.. code-block:: none

  &eth {
    managed = "in-band-status";
    phy = <&phy>;
    phy-mode = "sgmii";
  };

uses in-band mode, where results from the PHY's negotiation are passed
to the MAC through the SGMII control word, and the MAC is expected to
acknowledge the control word.  The :c:func:`mac_link_up` and
:c:func:`mac_link_down` functions must not force the MAC side link
up and down.
(使用带内模式，PHY 协商的结果通过 SGMII 控制字传递给 MAC，并且期望 MAC 对
控制字进行确认。:c:func:`mac_link_up` 与 :c:func:`mac_link_down` 函数不得
强制将 MAC 侧链路置为 up 与 down。)

Rough guide to converting a network driver to sfp/phylink
(将网络驱动转换为 sfp/phylink 的粗略指南)
=========================================================

This guide briefly describes how to convert a network driver from
phylib to the sfp/phylink support.  Please send patches to improve
this documentation.
(本指南简要描述如何将网络驱动从 phylib 转换为 sfp/phylink 支持。欢迎提交
补丁来改进本文档。)

1. Optionally split the network driver's phylib update function into
   two parts dealing with link-down and link-up. This can be done as
   a separate preparation commit.
   (可选) 把网络驱动的 phylib 更新函数拆分为处理链路 down 与链路 up 的两个
   部分。这可以作为单独的准备性提交来完成。

   An older example of this preparation can be found in git commit
   fc548b991fb0, although this was splitting into three parts; the
   link-up part now includes configuring the MAC for the link settings.
   Please see :c:func:`mac_link_up` for more information on this.
   (这一准备的较早示例见 git 提交 fc548b991fb0，不过那次拆分成了三个部分；
   如今链路 up 部分已包含按链路设置配置 MAC 的内容。更多信息请参见
   :c:func:`mac_link_up`。)

2. Replace::
   (把以下内容：)

	select FIXED_PHY
	select PHYLIB

   with::
   (替换为：)

	select PHYLINK

   in the driver's Kconfig stanza.
   (以上修改位于驱动的 Kconfig 配置段中。)

3. Add::
   (添加：)

	#include <linux/phylink.h>

   to the driver's list of header files.
   (到驱动的头文件列表中。)

4. Add::
   (添加：)

	struct phylink *phylink;
	struct phylink_config phylink_config;

   to the driver's private data structure.  We shall refer to the
   driver's private data pointer as ``priv`` below, and the driver's
   private data structure as ``struct foo_priv``.
   (到驱动的私有数据结构中。下文中，我们把驱动的私有数据指针称为 ``priv``，
   把驱动的私有数据结构称为 ``struct foo_priv``。)

5. Replace the following functions:
   (将以下函数进行替换：)

   .. flat-table::
    :header-rows: 1
    :widths: 1 1
    :stub-columns: 0

    * - Original function (原函数)
      - Replacement function (替代函数)
    * - phy_start(phydev)
      - phylink_start(priv->phylink)
    * - phy_stop(phydev)
      - phylink_stop(priv->phylink)
    * - phy_mii_ioctl(phydev, ifr, cmd)
      - phylink_mii_ioctl(priv->phylink, ifr, cmd)
    * - phy_ethtool_get_wol(phydev, wol)
      - phylink_ethtool_get_wol(priv->phylink, wol)
    * - phy_ethtool_set_wol(phydev, wol)
      - phylink_ethtool_set_wol(priv->phylink, wol)
    * - phy_disconnect(phydev)
      - phylink_disconnect_phy(priv->phylink)

   Please note that some of these functions must be called under the
   rtnl lock, and will warn if not. This will normally be the case,
   except if these are called from the driver suspend/resume paths.
   (请注意，其中一些函数必须在 rtnl 锁保护下调用，否则会发出警告。通常情况
   正是如此，除非这些函数是从驱动的挂起/恢复路径中调用的。)

6. Add/replace ksettings get/set methods with:
   (添加/替换 ksettings get/set 方法，如下：)

   .. code-block:: c

	static int foo_ethtool_set_link_ksettings(struct net_device *dev,
						  const struct ethtool_link_ksettings *cmd)
	{
		struct foo_priv *priv = netdev_priv(dev);
	
		return phylink_ethtool_ksettings_set(priv->phylink, cmd);
	}

	static int foo_ethtool_get_link_ksettings(struct net_device *dev,
						  struct ethtool_link_ksettings *cmd)
	{
		struct foo_priv *priv = netdev_priv(dev);
	
		return phylink_ethtool_ksettings_get(priv->phylink, cmd);
	}

7. Replace the call to::
   (将调用：)

	phy_dev = of_phy_connect(dev, node, link_func, flags, phy_interface);

   and associated code with a call to::
   (及相关代码替换为调用：)

	err = phylink_of_phy_connect(priv->phylink, node, flags);

   For the most part, ``flags`` can be zero; these flags are passed to
   the phy_attach_direct() inside this function call if a PHY is specified
   in the DT node ``node``.
   (在大多数情况下 ``flags`` 可以为零；如果 DT 节点 ``node`` 中指定了 PHY，
   这些标志会传递给该函数调用内部的 phy_attach_direct()。)

   ``node`` should be the DT node which contains the network phy property,
   fixed link properties, and will also contain the sfp property.
   (``node`` 应为包含网络 phy 属性与固定链路属性的 DT 节点，之后也将包含
   sfp 属性。)

   The setup of fixed links should also be removed; these are handled
   internally by phylink.
   (固定链路的设置代码也应删除；这些由 phylink 在内部处理。)

   of_phy_connect() was also passed a function pointer for link updates.
   This function is replaced by a different form of MAC updates
   described below in (8).
   (of_phy_connect() 还会传入一个用于链路更新的函数指针。该函数被下文第
   (8) 步所述的一种不同形式的 MAC 更新方式所取代。)

   Manipulation of the PHY's supported/advertised happens within phylink
   based on the validate callback, see below in (8).
   (对 PHY 的 supported/advertised 的操作由 phylink 基于 validate 回调在内部
   完成，见下文第 (8) 步。)

   Note that the driver no longer needs to store the ``phy_interface``,
   and also note that ``phy_interface`` becomes a dynamic property,
   just like the speed, duplex etc. settings.
   (注意，驱动不再需要保存 ``phy_interface``；还要注意 ``phy_interface`` 已
   成为动态属性，就像速率、双工等设置一样。)

   Finally, note that the MAC driver has no direct access to the PHY
   anymore; that is because in the phylink model, the PHY can be
   dynamic.
   (最后，注意 MAC 驱动不再直接访问 PHY；这是因为在 phylink 模型中，PHY 可以
   是动态的。)

8. Add a :c:type:`struct phylink_mac_ops <phylink_mac_ops>` instance to
   the driver, which is a table of function pointers, and implement
   these functions. The old link update function for
   :c:func:`of_phy_connect` becomes three methods: :c:func:`mac_link_up`,
   :c:func:`mac_link_down`, and :c:func:`mac_config`. If step 1 was
   performed, then the functionality will have been split there.
   (向驱动添加一个 :c:type:`struct phylink_mac_ops <phylink_mac_ops>` 实例，
   它是一张函数指针表，并实现这些函数。:c:func:`of_phy_connect` 原来的链路
   更新函数变为三个方法：:c:func:`mac_link_up`、:c:func:`mac_link_down` 与
   :c:func:`mac_config`。如果执行了第 1 步，相应功能已在那时完成了拆分。)

   It is important that if in-band negotiation is used,
   :c:func:`mac_link_up` and :c:func:`mac_link_down` do not prevent the
   in-band negotiation from completing, since these functions are called
   when the in-band link state changes - otherwise the link will never
   come up.
   (重要的一点是，如果使用带内协商，:c:func:`mac_link_up` 与
   :c:func:`mac_link_down` 不得阻止带内协商完成，因为这些函数会在带内链路
   状态变化时被调用——否则链路将永远无法建立。)

   The :c:func:`mac_get_caps` method is optional, and if provided should
   return the phylink MAC capabilities that are supported for the passed
   ``interface`` mode. In general, there is no need to implement this method.
   Phylink will use these capabilities in combination with permissible
   capabilities for ``interface`` to determine the allowable ethtool link
   modes.
   (:c:func:`mac_get_caps` 方法是可选的；如果提供，它应返回针对所传入
   ``interface`` 模式受支持的 phylink MAC 能力。一般来说，无需实现该方法。
   Phylink 会将这些能力与 ``interface`` 允许的能力相结合，以确定可用的
   ethtool 链路模式。)

   The :c:func:`mac_link_state` method is used to read the link state
   from the MAC, and report back the settings that the MAC is currently
   using. This is particularly important for in-band negotiation
   methods such as 1000base-X and SGMII.
   (:c:func:`mac_link_state` 方法用于从 MAC 读取链路状态，并回报 MAC 当前
   正在使用的设置。这对 1000base-X、SGMII 等带内协商方式尤为重要。)

   The :c:func:`mac_link_up` method is used to inform the MAC that the
   link has come up. The call includes the negotiation mode and interface
   for reference only. The finalised link parameters are also supplied
   (speed, duplex and flow control/pause enablement settings) which
   should be used to configure the MAC when the MAC and PCS are not
   tightly integrated, or when the settings are not coming from in-band
   negotiation.
   (:c:func:`mac_link_up` 方法用于通知 MAC 链路已建立。调用中包含的协商模式
   与接口仅作参考。最终确定的链路参数 (速率、双工与流控/暂停使能设置) 也会
   一并提供；当 MAC 与 PCS 未紧密集成，或设置并非来自带内协商时，应使用这些
   参数来配置 MAC。)

   The :c:func:`mac_config` method is used to update the MAC with the
   requested state, and must avoid unnecessarily taking the link down
   when making changes to the MAC configuration.  This means the
   function should modify the state and only take the link down when
   absolutely necessary to change the MAC configuration.  An example
   of how to do this can be found in :c:func:`mvneta_mac_config` in
   ``drivers/net/ethernet/marvell/mvneta.c``.
   (:c:func:`mac_config` 方法用于按请求的状态更新 MAC，并且在修改 MAC 配置
   时必须避免不必要地把链路置为 down。这意味着该函数应修改状态，只有在更改
   MAC 配置绝对必要时才把链路置为 down。如何做到这一点的一个示例可参见
   ``drivers/net/ethernet/marvell/mvneta.c`` 中的
   :c:func:`mvneta_mac_config`。)

   For further information on these methods, please see the inline
   documentation in :c:type:`struct phylink_mac_ops <phylink_mac_ops>`.
   (有关这些方法的更多信息，请参见
   :c:type:`struct phylink_mac_ops <phylink_mac_ops>` 中的内嵌文档。)

9. Fill-in the :c:type:`struct phylink_config <phylink_config>` fields with
   a reference to the :c:type:`struct device <device>` associated to your
   :c:type:`struct net_device <net_device>`:
   (在 :c:type:`struct phylink_config <phylink_config>` 字段中填入与你的
   :c:type:`struct net_device <net_device>` 相关联的
   :c:type:`struct device <device>` 的引用：)

   .. code-block:: c

	priv->phylink_config.dev = &dev.dev;
	priv->phylink_config.type = PHYLINK_NETDEV;

   Fill-in the various speeds, pause and duplex modes your MAC can handle:
   (填入你的 MAC 可支持的各种速率、暂停 (pause) 与双工模式：)

   .. code-block:: c

        priv->phylink_config.mac_capabilities = MAC_SYM_PAUSE | MAC_10 | MAC_100 | MAC_1000FD;

10. Some Ethernet controllers work in pair with a PCS (Physical Coding Sublayer)
    block, that can handle among other things the encoding/decoding, link
    establishment detection and autonegotiation. While some MACs have internal
    PCS whose operation is transparent, some other require dedicated PCS
    configuration for the link to become functional. In that case, phylink
    provides a PCS abstraction through :c:type:`struct phylink_pcs <phylink_pcs>`.
    (一些以太网控制器与 PCS (物理编码子层，Physical Coding Sublayer) 块配合
    工作，后者可处理编码/解码、链路建立检测与自协商等事务。一些 MAC 内部
    PCS 的运作是透明的，而另一些 MAC 则需要专门的 PCS 配置才能使链路正常
    工作。在这种情况下，phylink 通过
    :c:type:`struct phylink_pcs <phylink_pcs>` 提供 PCS 抽象。)

    Identify if your driver has one or more internal PCS blocks, and/or if
    your controller can use an external PCS block that might be internally
    connected to your controller.
    (确认你的驱动是否有一个或多个内部 PCS 块，以及/或者你的控制器能否使用
    可能与控制器内部相连的外部 PCS 块。)

    If your controller doesn't have any internal PCS, you can go to step 11.
    (如果你的控制器没有任何内部 PCS，可直接跳到第 11 步。)

    If your Ethernet controller contains one or several PCS blocks, create
    one :c:type:`struct phylink_pcs <phylink_pcs>` instance per PCS block within
    your driver's private data structure:
    (如果你的以太网控制器包含一个或多个 PCS 块，则在驱动私有数据结构中为每个
    PCS 块创建一个 :c:type:`struct phylink_pcs <phylink_pcs>` 实例：)

    .. code-block:: c

        struct phylink_pcs pcs;

    Populate the relevant :c:type:`struct phylink_pcs_ops <phylink_pcs_ops>` to
    configure your PCS. Create a :c:func:`pcs_get_state` function that reports
    the inband link state, a :c:func:`pcs_config` function to configure your
    PCS according to phylink-provided parameters, and a :c:func:`pcs_validate`
    function that report to phylink all accepted configuration parameters for
    your PCS:
    (填充相应的 :c:type:`struct phylink_pcs_ops <phylink_pcs_ops>` 来配置你的
    PCS。创建一个报告带内链路状态的 :c:func:`pcs_get_state` 函数、一个根据
    phylink 提供的参数配置 PCS 的 :c:func:`pcs_config` 函数，以及一个向
    phylink 报告你的 PCS 所接受的全部配置参数的 :c:func:`pcs_validate`
    函数：)

    .. code-block:: c

        struct phylink_pcs_ops foo_pcs_ops = {
                .pcs_validate = foo_pcs_validate,
                .pcs_get_state = foo_pcs_get_state,
                .pcs_config = foo_pcs_config,
        };

    Arrange for PCS link state interrupts to be forwarded into
    phylink, via:
    (安排把 PCS 链路状态中断转发进 phylink，方式如下：)

    .. code-block:: c

        phylink_pcs_change(pcs, link_is_up);

    where ``link_is_up`` is true if the link is currently up or false
    otherwise. If a PCS is unable to provide these interrupts, then
    it should set ``pcs->pcs_poll = true;`` when creating the PCS.
    (其中，如果链路当前为 up，则 ``link_is_up`` 为 true，否则为 false。如果
    PCS 无法提供这些中断，则应在创建 PCS 时设置 ``pcs->pcs_poll = true;``。)

11. If your controller relies on, or accepts the presence of an external PCS
    controlled through its own driver, add a pointer to a phylink_pcs instance
    in your driver private data structure:
    (如果你的控制器依赖外部 PCS，或接受由其自身驱动控制的外部 PCS，则在驱动
    私有数据结构中添加一个指向 phylink_pcs 实例的指针：)

    .. code-block:: c

        struct phylink_pcs *pcs;

    The way of getting an instance of the actual PCS depends on the platform,
    some PCS sit on an MDIO bus and are grabbed by passing a pointer to the
    corresponding :c:type:`struct mii_bus <mii_bus>` and the PCS's address on
    that bus. In this example, we assume the controller attaches to a Lynx PCS
    instance:
    (获取实际 PCS 实例的方式取决于平台。一些 PCS 位于 MDIO 总线上，通过传入
    对应 :c:type:`struct mii_bus <mii_bus>` 的指针以及 PCS 在该总线上的地址来
    获取。在本例中，我们假设控制器连接到一个 Lynx PCS 实例：)

    .. code-block:: c

        priv->pcs = lynx_pcs_create_mdiodev(bus, 0);

    Some PCS can be recovered based on firmware information:
    (一些 PCS 可基于固件信息来获取：)

    .. code-block:: c

        priv->pcs = lynx_pcs_create_fwnode(of_fwnode_handle(node));

12. Populate the :c:func:`mac_select_pcs` callback and add it to your
    :c:type:`struct phylink_mac_ops <phylink_mac_ops>` set of ops. This function
    must return a pointer to the relevant :c:type:`struct phylink_pcs <phylink_pcs>`
    that will be used for the requested link configuration:
    (实现 :c:func:`mac_select_pcs` 回调，并将其添加到你的
    :c:type:`struct phylink_mac_ops <phylink_mac_ops>` 操作集中。该函数必须
    返回指向将被用于所请求链路配置的相关
    :c:type:`struct phylink_pcs <phylink_pcs>` 的指针：)

    .. code-block:: c

        static struct phylink_pcs *foo_select_pcs(struct phylink_config *config,
                                                  phy_interface_t interface)
        {
                struct foo_priv *priv = container_of(config, struct foo_priv,
                                                     phylink_config);

                if ( /* 'interface' needs a PCS to function */ )
                        return priv->pcs;

                return NULL;
        }

    See :c:func:`mvpp2_select_pcs` for an example of a driver that has multiple
    internal PCS.
    (拥有多个内部 PCS 的驱动示例可参见 :c:func:`mvpp2_select_pcs`。)

13. Fill-in all the :c:type:`phy_interface_t <phy_interface_t>` (i.e. all MAC to
    PHY link modes) that your MAC can output. The following example shows a
    configuration for a MAC that can handle all RGMII modes, SGMII and 1000BaseX.
    You must adjust these according to what your MAC and all PCS associated
    with this MAC are capable of, and not just the interface you wish to use:
    (填入你的 MAC 可输出的全部 :c:type:`phy_interface_t <phy_interface_t>`
    (即所有 MAC 到 PHY 的链路模式)。下面的示例展示了一个可处理所有 RGMII
    模式、SGMII 与 1000BaseX 的 MAC 的配置。你必须根据 MAC 及与该 MAC 关联的
    所有 PCS 的实际能力进行调整，而不仅仅根据你想使用的接口：)

    .. code-block:: c

       phy_interface_set_rgmii(priv->phylink_config.supported_interfaces);
        __set_bit(PHY_INTERFACE_MODE_SGMII,
                  priv->phylink_config.supported_interfaces);
        __set_bit(PHY_INTERFACE_MODE_1000BASEX,
                  priv->phylink_config.supported_interfaces);

14. Remove calls to of_parse_phandle() for the PHY,
    of_phy_register_fixed_link() for fixed links etc. from the probe
    function, and replace with:
    (从 probe 函数中移除针对 PHY 的 of_parse_phandle() 调用、针对固定链路的
    of_phy_register_fixed_link() 调用等，并替换为：)

    .. code-block:: c

	struct phylink *phylink;

	phylink = phylink_create(&priv->phylink_config, node, phy_mode, &phylink_ops);
	if (IS_ERR(phylink)) {
		err = PTR_ERR(phylink);
		fail probe;
	}

	priv->phylink = phylink;

    and arrange to destroy the phylink in the probe failure path as
    appropriate and the removal path too by calling:
    (并在 probe 失败路径与移除路径中视情况安排销毁 phylink，方法是调用：)

    .. code-block:: c

	phylink_destroy(priv->phylink);

15. Arrange for MAC link state interrupts to be forwarded into
    phylink, via:
    (安排把 MAC 链路状态中断转发进 phylink，方式如下：)

    .. code-block:: c

	phylink_mac_change(priv->phylink, link_is_up);

    where ``link_is_up`` is true if the link is currently up or false
    otherwise.
    (其中，如果链路当前为 up，则 ``link_is_up`` 为 true，否则为 false。)

16. Verify that the driver does not call::
    (确认驱动不会调用：)

	netif_carrier_on()
	netif_carrier_off()

    as these will interfere with phylink's tracking of the link state,
    and cause phylink to omit calls via the :c:func:`mac_link_up` and
    :c:func:`mac_link_down` methods.
    (因为这些调用会干扰 phylink 对链路状态的跟踪，导致 phylink 省略通过
    :c:func:`mac_link_up` 与 :c:func:`mac_link_down` 方法发起的调用。)

Network drivers should call phylink_stop() and phylink_start() via their
suspend/resume paths, which ensures that the appropriate
:c:type:`struct phylink_mac_ops <phylink_mac_ops>` methods are called
as necessary.
(网络驱动应在其挂起/恢复路径中调用 phylink_stop() 与 phylink_start()，这可
确保按需调用相应的 :c:type:`struct phylink_mac_ops <phylink_mac_ops>` 方法。)

For information describing the SFP cage in DT, please see the binding
documentation in the kernel source tree
``Documentation/devicetree/bindings/net/sff,sfp.yaml``.
(有关 DT 中 SFP 笼 (SFP cage) 的描述信息，请参见内核源码树中的绑定文档
``Documentation/devicetree/bindings/net/sff,sfp.yaml``。)
