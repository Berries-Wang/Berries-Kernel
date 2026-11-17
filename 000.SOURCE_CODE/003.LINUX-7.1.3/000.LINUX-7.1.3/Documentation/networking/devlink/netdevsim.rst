.. SPDX-License-Identifier: GPL-2.0

=================================================
netdevsim devlink support(netdevsim devlink 支持)
=================================================

This document describes the ``devlink`` features supported by the
``netdevsim`` device driver.
(本文档描述 ``netdevsim`` 设备驱动所支持的 ``devlink`` 特性。)

Parameters(参数)
================

.. list-table:: Generic parameters implemented

   * - Name (名称)
     - Mode (模式)
   * - ``max_macs``
     - driverinit

The ``netdevsim`` driver also implements the following driver-specific
parameters.
(``netdevsim`` 驱动还实现了以下驱动特定参数。)

.. list-table:: Driver-specific parameters implemented
   :widths: 5 5 5 85

   * - Name (名称)
     - Type (类型)
     - Mode (模式)
     - Description (描述)
   * - ``test1``
     - Boolean
     - driverinit
     - Test parameter used to show how a driver-specific devlink parameter
       can be implemented.
       (用于演示如何实现驱动特定 devlink 参数的测试参数。)

The ``netdevsim`` driver supports reloading via ``DEVLINK_CMD_RELOAD``
(``netdevsim`` 驱动支持通过 ``DEVLINK_CMD_RELOAD`` 进行重载)

Regions(区域)
=============

The ``netdevsim`` driver exposes a ``dummy`` region as an example of how the
devlink-region interfaces work. A snapshot is taken whenever the
``take_snapshot`` debugfs file is written to.
(``netdevsim`` 驱动暴露一个 ``dummy`` 区域，作为 devlink-region 接口如何
工作的示例。每当 ``take_snapshot`` debugfs 文件被写入时，就会拍摄一个
快照。)

Resources(资源)
===============

The ``netdevsim`` driver exposes resources to control the number of FIB
entries, FIB rule entries and nexthops that the driver will allow.
(``netdevsim`` 驱动暴露一些资源，用于控制驱动允许的 FIB 表项、FIB 规则
表项和 nexthop 的数量。)

.. code:: shell

    $ devlink resource set netdevsim/netdevsim0 path /IPv4/fib size 96
    $ devlink resource set netdevsim/netdevsim0 path /IPv4/fib-rules size 16
    $ devlink resource set netdevsim/netdevsim0 path /IPv6/fib size 64
    $ devlink resource set netdevsim/netdevsim0 path /IPv6/fib-rules size 16
    $ devlink resource set netdevsim/netdevsim0 path /nexthops size 16
    $ devlink dev reload netdevsim/netdevsim0

Rate objects(速率对象)
======================

The ``netdevsim`` driver supports rate objects management, which includes:
(``netdevsim`` 驱动支持速率对象管理，包括：)

- registering/unregistering leaf rate objects per VF devlink port;
  (为每个 VF devlink 端口注册/注销叶 (leaf) 速率对象；)
- creation/deletion node rate objects;
  (创建/删除节点 (node) 速率对象；)
- setting tx_share and tx_max rate values for any rate object type;
  (为任何速率对象类型设置 tx_share 和 tx_max 速率值；)
- setting parent node for any rate object type.
  (为任何速率对象类型设置父节点。)

Rate nodes and their parameters are exposed in ``netdevsim`` debugfs in RO mode.
For example created rate node with name ``some_group``:
(速率节点及其参数以 RO 模式暴露在 ``netdevsim`` debugfs 中。例如，创建了
名为 ``some_group`` 的速率节点之后：)

.. code:: shell

    $ ls /sys/kernel/debug/netdevsim/netdevsim0/rate_groups/some_group
    rate_parent  tx_max  tx_share

Same parameters are exposed for leaf objects in corresponding ports directories.
For ex.:
(在相应的端口目录中，也为叶对象暴露了相同的参数。例如：)

.. code:: shell

    $ ls /sys/kernel/debug/netdevsim/netdevsim0/ports/1
    dev  ethtool  rate_parent  tx_max  tx_share

Driver-specific Traps(驱动特定陷阱)
===================================

.. list-table:: List of Driver-specific Traps Registered by ``netdevsim``
   :widths: 5 5 90

   * - Name (名称)
     - Type (类型)
     - Description (描述)
   * - ``fid_miss``
     - ``exception``
     - When a packet enters the device it is classified to a filtering
       identifier (FID) based on the ingress port and VLAN. This trap is used
       to trap packets for which a FID could not be found
       (当数据包进入设备时，会根据入端口和 VLAN 把它归类到某个过滤标识符
       (FID)。此陷阱用于捕获那些无法找到 FID 的数据包)
