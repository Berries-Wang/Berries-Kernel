.. SPDX-License-Identifier: GPL-2.0

==========================
Devlink E-Switch Attribute
==========================

(devlink E-Switch 属性)

Devlink E-Switch supports two modes of operation: legacy and switchdev.
Legacy mode operates based on traditional MAC/VLAN steering rules. Switching
decisions are made based on MAC addresses, VLANs, etc. There is limited ability
to offload switching rules to hardware.
(devlink E-Switch 支持两种工作模式：legacy 和 switchdev。legacy 模式基于传统的 MAC/VLAN 引流规则工作，交换决策依据 MAC 地址、VLAN 等做出，将交换规则卸载到硬件的能力有限。)

On the other hand, switchdev mode allows for more advanced offloading
capabilities of the E-Switch to hardware. In switchdev mode, more switching
rules and logic can be offloaded to the hardware switch ASIC. It enables
representor netdevices that represent the slow path of virtual functions (VFs)
or scalable-functions (SFs) of the device. See more information about
:ref:`Documentation/networking/switchdev.rst <switchdev>` and
:ref:`Documentation/networking/representors.rst <representors>`.
(另一方面，switchdev 模式允许将 E-Switch 更高级的卸载能力交由硬件实现。在 switchdev 模式下，更多的交换规则和逻辑可以卸载到硬件交换 ASIC 上。它会启用代表设备虚拟功能 (VF) 或可扩展功能 (SF) 慢路径的 representor 网络设备。更多信息参见 :ref:`Documentation/networking/switchdev.rst <switchdev>` 和 :ref:`Documentation/networking/representors.rst <representors>`。)

In addition, the devlink E-Switch also comes with other attributes listed
in the following section.
(此外，devlink E-Switch 还带有其他属性，列在下一节中。)

Attributes Description
======================
(属性描述)

The following is a list of E-Switch attributes.
(以下是 E-Switch 属性的列表。)

.. list-table:: E-Switch attributes
   :widths: 8 5 45

   * - Name (名称)
     - Type (类型)
     - Description (描述)
   * - ``mode``
     - enum
     - The mode of the device. The mode can be one of the following:
       (设备的模式。该模式可以是以下之一：)

       * ``legacy`` operates based on traditional MAC/VLAN steering
         rules.
         (基于传统的 MAC/VLAN 引流规则工作。)
       * ``switchdev`` allows for more advanced offloading capabilities of
         the E-Switch to hardware.
         (允许将 E-Switch 更高级的卸载能力交由硬件实现。)
       * ``switchdev_inactive`` switchdev mode but starts inactive, doesn't allow traffic
         until explicitly activated. This mode is useful for orchestrators that
         want to prepare the device in switchdev mode but only activate it when
         all configurations are done.
         (switchdev 模式，但以非活动状态启动，在显式激活之前不允许流量通过。该模式适用于希望以 switchdev 模式准备好设备、但只在所有配置完成后才将其激活的编排器。)
   * - ``inline-mode``
     - enum
     - Some HWs need the VF driver to put part of the packet
       headers on the TX descriptor so the e-switch can do proper
       matching and steering. Support for both switchdev mode and legacy mode.
       (某些硬件需要 VF 驱动将部分数据包头放在 TX 描述符上，以便 e-switch 能进行正确的匹配和引流。switchdev 模式和 legacy 模式均受支持。)

       * ``none`` none.
         (无。)
       * ``link`` L2 mode.
         (L2 模式。)
       * ``network`` L3 mode.
         (L3 模式。)
       * ``transport`` L4 mode.
         (L4 模式。)
   * - ``encap-mode``
     - enum
     - The encapsulation mode of the device. Support for both switchdev mode
       and legacy mode. The mode can be one of the following:
       (设备的封装模式。switchdev 模式和 legacy 模式均受支持。该模式可以是以下之一：)

       * ``none`` Disable encapsulation support.
         (禁用封装支持。)
       * ``basic`` Enable encapsulation support.
         (启用封装支持。)

Example Usage
=============
(用法示例)

.. code:: shell

    # enable switchdev mode
    $ devlink dev eswitch set pci/0000:08:00.0 mode switchdev

    # set inline-mode and encap-mode
    $ devlink dev eswitch set pci/0000:08:00.0 inline-mode none encap-mode basic

    # display devlink device eswitch attributes
    $ devlink dev eswitch show pci/0000:08:00.0
      pci/0000:08:00.0: mode switchdev inline-mode none encap-mode basic

    # enable encap-mode with legacy mode
    $ devlink dev eswitch set pci/0000:08:00.0 mode legacy inline-mode none encap-mode basic

    # start switchdev mode in inactive state
    $ devlink dev eswitch set pci/0000:08:00.0 mode switchdev_inactive

    # setup switchdev configurations, representors, FDB entries, etc..
    ...

    # activate switchdev mode to allow traffic
    $ devlink dev eswitch set pci/0000:08:00.0 mode switchdev
