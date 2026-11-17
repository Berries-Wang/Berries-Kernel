.. SPDX-License-Identifier: GPL-2.0

=================
Devlink Line card
=================

(devlink 线卡)

Background
(背景)
==========

The ``devlink-linecard`` mechanism is targeted for manipulation of
line cards that serve as a detachable PHY modules for modular switch
system. Following operations are provided:
(``devlink-linecard`` 机制用于操作线卡，线卡是模块化交换机系统中可插拔
的 PHY 模块。提供以下操作：)

  * Get a list of supported line card types.
    (获取受支持的线卡类型列表。)
  * Provision of a slot with specific line card type.
    (用特定线卡类型对插槽进行配置 (provision)。)
  * Get and monitor of line card state and its change.
    (获取并监控线卡状态及其变化。)

Line card according to the type may contain one or more gearboxes
to mux the lanes with certain speed to multiple ports with lanes
of different speed. Line card ensures N:M mapping between
the switch ASIC modules and physical front panel ports.
(线卡根据其类型可能包含一个或多个 gearbox，用于把一定速率的通道复用
到具有不同速率通道的多个端口上。线卡确保交换 ASIC 模块与物理前面板
端口之间的 N:M 映射。)

Overview
(概述)
========

Each line card devlink object is created by device driver,
according to the physical line card slots available on the device.
(每个线卡 devlink 对象都由设备驱动根据设备上可用的物理线卡插槽创建。)

Similar to splitter cable, where the device might have no way
of detection of the splitter cable geometry, the device
might not have a way to detect line card type. For that devices,
concept of provisioning is introduced. It allows the user to:
(类似于分路器线缆——设备可能无法检测分路器线缆的几何结构——设备也可能
没有办法检测线卡类型。对于这类设备，引入了配置 (provisioning) 概念。
它允许用户：)

  * Provision a line card slot with certain line card type
    (用某个线卡类型配置线卡插槽)

    - Device driver would instruct the ASIC to prepare all
      resources accordingly. The device driver would
      create all instances, namely devlink port and netdevices
      that reside on the line card, according to the line card type
      (设备驱动会指示 ASIC 相应地准备好所有资源。设备驱动会根据线卡
      类型创建位于线卡上的所有实例，即 devlink port 和 netdevice)
  * Manipulate of line card entities even without line card
    being physically connected or powered-up
    (即使线卡未物理连接或未上电，也能操作线卡实体)
  * Setup splitter cable on line card ports
    (在线卡端口上设置分路器线缆)

    - As on the ordinary ports, user may provision a splitter
      cable of a certain type, without the need to
      be physically connected to the port
      (与普通端口一样，用户可以配置某种类型的分路器线缆，而无需实际
      连接到端口)
  * Configure devlink ports and netdevices
    (配置 devlink port 和 netdevice)

Netdevice carrier is decided as follows:
(Netdevice 载波状态的判定方式如下：)

  * Line card is not inserted or powered-down
    (线卡未插入或已断电)

    - The carrier is always down
      (载波始终为 down)
  * Line card is inserted and powered up
    (线卡已插入且已上电)

    - The carrier is decided as for ordinary port netdevice
      (载波状态按普通端口 netdevice 的方式判定)

Line card state
(线卡状态)
===============

The ``devlink-linecard`` mechanism supports the following line card states:
(``devlink-linecard`` 机制支持以下线卡状态：)

  * ``unprovisioned``: Line card is not provisioned on the slot.
    (``unprovisioned``： 插槽上的线卡未进行配置。)
  * ``unprovisioning``: Line card slot is currently being unprovisioned.
    (``unprovisioning``： 线卡插槽当前正在被取消配置。)
  * ``provisioning``: Line card slot is currently in a process of being provisioned
    with a line card type.
    (``provisioning``： 线卡插槽当前正处于以某个线卡类型进行配置的过程之中。)
  * ``provisioning_failed``: Provisioning was not successful.
    (``provisioning_failed``： 配置未成功。)
  * ``provisioned``: Line card slot is provisioned with a type.
    (``provisioned``： 线卡插槽已用某个类型完成配置。)
  * ``active``: Line card is powered-up and active.
    (``active``： 线卡已上电且处于活动状态。)

The following diagram provides a general overview of ``devlink-linecard``
state transitions::
(下图给出了 ``devlink-linecard`` 状态转换的总体概览：)

                                          +-------------------------+
                                          |                         |
       +---------------------------------->      unprovisioned      |
       |                                  |                         |
       |                                  +--------|-------^--------+
       |                                           |       |
       |                                           |       |
       |                                  +--------v-------|--------+
       |                                  |                         |
       |                                  |       provisioning      |
       |                                  |                         |
       |                                  +------------|------------+
       |                                               |
       |                 +-----------------------------+
       |                 |                             |
       |    +------------v------------+   +------------v------------+   +-------------------------+
       |    |                         |   |                         ---->                         |
       +-----   provisioning_failed   |   |       provisioned       |   |         active          |
       |    |                         |   |                         <----                         |
       |    +------------^------------+   +------------|------------+   +-------------------------+
       |                 |                             |
       |                 |                             |
       |                 |                +------------v------------+
       |                 |                |                         |
       |                 |                |     unprovisioning      |
       |                 |                |                         |
       |                 |                +------------|------------+
       |                 |                             |
       |                 +-----------------------------+
       |                                               |
       +-----------------------------------------------+


Example usage
(用法示例)
=============

.. code:: shell

    $ devlink lc show [ DEV [ lc LC_INDEX ] ]
    $ devlink lc set DEV lc LC_INDEX [ { type LC_TYPE | notype } ]

    # Show current line card configuration and status for all slots:
    $ devlink lc

    # Set slot 8 to be provisioned with type "16x100G":
    $ devlink lc set pci/0000:01:00.0 lc 8 type 16x100G

    # Set slot 8 to be unprovisioned:
    $ devlink lc set pci/0000:01:00.0 lc 8 notype
