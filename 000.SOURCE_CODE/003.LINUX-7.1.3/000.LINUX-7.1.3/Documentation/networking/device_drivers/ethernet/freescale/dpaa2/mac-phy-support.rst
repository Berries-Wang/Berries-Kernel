.. SPDX-License-Identifier: GPL-2.0
.. include:: <isonum.txt>

=============================================
DPAA2 MAC / PHY support(DPAA2 MAC / PHY 支持)
=============================================

:Copyright: |copy| 2019 NXP
(版权所有: |copy| 2019 NXP)

Overview(概述)
--------------

The DPAA2 MAC / PHY support consists of a set of APIs that help DPAA2 network
drivers (dpaa2-eth, dpaa2-ethsw) interact with the PHY library.
(DPAA2 MAC / PHY 支持由一组 API 组成，用于帮助 DPAA2 网络驱动（dpaa2-eth、dpaa2-ethsw）与 PHY 库交互。)

DPAA2 Software Architecture(DPAA2 软件架构)
-------------------------------------------

Among other DPAA2 objects, the fsl-mc bus exports DPNI objects (abstracting a
network interface) and DPMAC objects (abstracting a MAC). The dpaa2-eth driver
probes on the DPNI object and connects to and configures a DPMAC object with
the help of phylink.
(除其他 DPAA2 对象外，fsl-mc 总线还导出 DPNI 对象（抽象网络接口）和 DPMAC 对象（抽象 MAC）。dpaa2-eth 驱动
探测 DPNI 对象，并借助 phylink 连接和配置 DPMAC 对象。)

Data connections may be established between a DPNI and a DPMAC, or between two
DPNIs. Depending on the connection type, the netif_carrier_[on/off] is handled
directly by the dpaa2-eth driver or by phylink.
(可以在 DPNI 与 DPMAC 之间、或两个 DPNI 之间建立数据连接。根据连接类型的不同，netif_carrier_[on/off] 由
dpaa2-eth 驱动直接处理，或由 phylink 处理。)

.. code-block:: none

  Sources of abstracted link state information presented by the MC firmware

                                               +--------------------------------------+
  +------------+                  +---------+  |                           xgmac_mdio |
  | net_device |                  | phylink |--|  +-----+  +-----+  +-----+  +-----+  |
  +------------+                  +---------+  |  | PHY |  | PHY |  | PHY |  | PHY |  |
        |                             |        |  +-----+  +-----+  +-----+  +-----+  |
      +------------------------------------+   |                    External MDIO bus |
      |            dpaa2-eth               |   +--------------------------------------+
      +------------------------------------+
        |                             |                                           Linux
  ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
        |                             |                                     MC firmware
        |              /|             V
  +----------+        / |       +----------+
  |          |       /  |       |          |
  |          |       |  |       |          |
  |   DPNI   |<------|  |<------|   DPMAC  |
  |          |       |  |       |          |
  |          |       \  |<---+  |          |
  +----------+        \ |    |  +----------+
                       \|    |
                             |
           +--------------------------------------+
           | MC firmware polling MAC PCS for link |
           |  +-----+  +-----+  +-----+  +-----+  |
           |  | PCS |  | PCS |  | PCS |  | PCS |  |
           |  +-----+  +-----+  +-----+  +-----+  |
           |                    Internal MDIO bus |
           +--------------------------------------+


Depending on an MC firmware configuration setting, each MAC may be in one of two modes:
(根据 MC 固件的配置设定，每个 MAC 可能处于以下两种模式之一：)

- DPMAC_LINK_TYPE_FIXED: the link state management is handled exclusively by
  the MC firmware by polling the MAC PCS. Without the need to register a
  phylink instance, the dpaa2-eth driver will not bind to the connected dpmac
  object at all.
  (DPMAC_LINK_TYPE_FIXED：链路状态管理完全由 MC 固件通过轮询 MAC PCS 处理。由于无需注册 phylink 实例，
  dpaa2-eth 驱动根本不会绑定到所连接的 dpmac 对象。)

- DPMAC_LINK_TYPE_PHY: The MC firmware is left waiting for link state update
  events, but those are in fact passed strictly between the dpaa2-mac (based on
  phylink) and its attached net_device driver (dpaa2-eth, dpaa2-ethsw),
  effectively bypassing the firmware.
  (DPMAC_LINK_TYPE_PHY：MC 固件仍处于等待链路状态更新事件的状态，但这些事件实际上严格地在 dpaa2-mac（基于
  phylink）与其所挂接的 net_device 驱动（dpaa2-eth、dpaa2-ethsw）之间传递，从而有效地绕过了固件。)

Implementation(实现)
--------------------

At probe time or when a DPNI's endpoint is dynamically changed, the dpaa2-eth
is responsible to find out if the peer object is a DPMAC and if this is the
case, to integrate it with PHYLINK using the dpaa2_mac_connect() API, which
will do the following:
(在探测时或 DPNI 的端点被动态更改时，dpaa2-eth 负责判断对端对象是否为 DPMAC；如果是，则使用
dpaa2_mac_connect() API 将其与 PHYLINK 集成，该 API 将执行以下操作：)

 - look up the device tree for PHYLINK-compatible of binding (phy-handle)
   (查找设备树中与 PHYLINK 兼容的绑定 (phy-handle))
 - will create a PHYLINK instance associated with the received net_device
   (将创建一个与所接收 net_device 关联的 PHYLINK 实例)
 - connect to the PHY using phylink_of_phy_connect()
   (使用 phylink_of_phy_connect() 连接到 PHY)

The following phylink_mac_ops callback are implemented:
(实现了以下 phylink_mac_ops 回调：)

 - .validate() will populate the supported linkmodes with the MAC capabilities
   only when the phy_interface_t is RGMII_* (at the moment, this is the only
   link type supported by the driver).
   (.validate() 仅在 phy_interface_t 为 RGMII_* 时，才把 MAC 能力填入支持的链路模式集合中（目前这是该驱动
   支持的唯一链路类型）。)

 - .mac_config() will configure the MAC in the new configuration using the
   dpmac_set_link_state() MC firmware API.
   (.mac_config() 将使用 dpmac_set_link_state() MC 固件 API 把 MAC 配置为新配置。)

 - .mac_link_up() / .mac_link_down() will update the MAC link using the same
   API described above.
   (.mac_link_up() / .mac_link_down() 将使用上述相同的 API 更新 MAC 链路。)

At driver unbind() or when the DPNI object is disconnected from the DPMAC, the
dpaa2-eth driver calls dpaa2_mac_disconnect() which will, in turn, disconnect
from the PHY and destroy the PHYLINK instance.
(在驱动解绑 (unbind()) 时或 DPNI 对象与 DPMAC 断开连接时，dpaa2-eth 驱动会调用 dpaa2_mac_disconnect()，
它随后会断开与 PHY 的连接并销毁 PHYLINK 实例。)

In case of a DPNI-DPMAC connection, an 'ip link set dev eth0 up' would start
the following sequence of operations:
(在 DPNI-DPMAC 连接的情况下，执行 'ip link set dev eth0 up' 将启动如下操作序列：)

(1) phylink_start() called from .dev_open().
    ((1) 由 .dev_open() 调用 phylink_start()。)
(2) The .mac_config() and .mac_link_up() callbacks are called by PHYLINK.
    ((2) PHYLINK 调用 .mac_config() 和 .mac_link_up() 回调。)
(3) In order to configure the HW MAC, the MC Firmware API
    dpmac_set_link_state() is called.
    ((3) 为了配置硬件 MAC，会调用 MC 固件 API dpmac_set_link_state()。)
(4) The firmware will eventually setup the HW MAC in the new configuration.
    ((4) 固件最终会按新配置设置硬件 MAC。)
(5) A netif_carrier_on() call is made directly from PHYLINK on the associated
    net_device.
    ((5) PHYLINK 直接在关联的 net_device 上调用 netif_carrier_on()。)
(6) The dpaa2-eth driver handles the LINK_STATE_CHANGE irq in order to
    enable/disable Rx taildrop based on the pause frame settings.
    ((6) dpaa2-eth 驱动处理 LINK_STATE_CHANGE 中断，以便根据暂停帧 (pause frame) 设置启用/禁用 Rx 尾丢弃
    (taildrop)。)

.. code-block:: none

  +---------+               +---------+
  | PHYLINK |-------------->|  eth0   |
  +---------+           (5) +---------+
  (1) ^  |
      |  |
      |  v (2)
  +-----------------------------------+
  |             dpaa2-eth             |
  +-----------------------------------+
         |                    ^ (6)
         |                    |
         v (3)                |
  +---------+---------------+---------+
  |  DPMAC  |               |  DPNI   |
  +---------+               +---------+
  |            MC Firmware            |
  +-----------------------------------+
         |
         |
         v (4)
  +-----------------------------------+
  |             HW MAC                |
  +-----------------------------------+

In case of a DPNI-DPNI connection, a usual sequence of operations looks like
the following:
(在 DPNI-DPNI 连接的情况下，常见的操作序列如下：)

(1) ip link set dev eth0 up
    ((1) 执行 ip link set dev eth0 up)
(2) The dpni_enable() MC API called on the associated fsl_mc_device.
    ((2) 在关联的 fsl_mc_device 上调用 MC API dpni_enable()。)
(3) ip link set dev eth1 up
    ((3) 执行 ip link set dev eth1 up)
(4) The dpni_enable() MC API called on the associated fsl_mc_device.
    ((4) 在关联的 fsl_mc_device 上调用 MC API dpni_enable()。)
(5) The LINK_STATE_CHANGED irq is received by both instances of the dpaa2-eth
    driver because now the operational link state is up.
    ((5) dpaa2-eth 驱动的两个实例都会收到 LINK_STATE_CHANGED 中断，因为此时操作链路状态已为 up。)
(6) The netif_carrier_on() is called on the exported net_device from
    link_state_update().
    ((6) 在 link_state_update() 中对导出的 net_device 调用 netif_carrier_on()。)

.. code-block:: none

  +---------+               +---------+
  |  eth0   |               |  eth1   |
  +---------+               +---------+
      |  ^                     ^  |
      |  |                     |  |
  (1) v  | (6)             (6) |  v (3)
  +---------+               +---------+
  |dpaa2-eth|               |dpaa2-eth|
  +---------+               +---------+
      |  ^                     ^  |
      |  |                     |  |
  (2) v  | (5)             (5) |  v (4)
  +---------+---------------+---------+
  |  DPNI   |               |  DPNI   |
  +---------+               +---------+
  |            MC Firmware            |
  +-----------------------------------+


Exported API(导出的 API)
------------------------

Any DPAA2 driver that drivers endpoints of DPMAC objects should service its
_EVENT_ENDPOINT_CHANGED irq and connect/disconnect from the associated DPMAC
when necessary using the below listed API::
(任何驱动 DPMAC 对象端点的 DPAA2 驱动，都应处理其 _EVENT_ENDPOINT_CHANGED 中断，并在必要时使用下面列出的
API 连接/断开相应的 DPMAC：)

 - int dpaa2_mac_connect(struct dpaa2_mac *mac);
 - void dpaa2_mac_disconnect(struct dpaa2_mac *mac);

A phylink integration is necessary only when the partner DPMAC is not of
``TYPE_FIXED``. This means it is either of ``TYPE_PHY``, or of
``TYPE_BACKPLANE`` (the difference being the two that in the ``TYPE_BACKPLANE``
mode, the MC firmware does not access the PCS registers). One can check for
this condition using the following helper::
(只有当对端 DPMAC 不是 ``TYPE_FIXED`` 类型时，才需要进行 phylink 集成。也就是说，它是 ``TYPE_PHY`` 或
``TYPE_BACKPLANE`` 类型（两者的区别在于：在 ``TYPE_BACKPLANE`` 模式下，MC 固件不访问 PCS 寄存器）。可以使
用以下辅助函数检查这一条件：)

 - static inline bool dpaa2_mac_is_type_phy(struct dpaa2_mac *mac);

Before connection to a MAC, the caller must allocate and populate the
dpaa2_mac structure with the associated net_device, a pointer to the MC portal
to be used and the actual fsl_mc_device structure of the DPMAC.
(在连接到 MAC 之前，调用者必须分配并填充 dpaa2_mac 结构：填入关联的 net_device、指向将使用的 MC portal 的
指针，以及该 DPMAC 实际的 fsl_mc_device 结构。)
