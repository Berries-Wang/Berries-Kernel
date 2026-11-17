.. SPDX-License-Identifier: GPL-2.0

============================
Devlink Region(devlink 区域)
============================

``devlink`` regions enable access to driver defined address regions using
devlink.
(``devlink`` 区域 (region) 支持通过 devlink 访问驱动定义的地址区域。)

Each device can create and register its own supported address regions. The
region can then be accessed via the devlink region interface.
(每个设备都可以创建并注册自己支持的地址区域。之后即可通过 devlink region 接口访问该区域。)

Region snapshots are collected by the driver, and can be accessed via read
or dump commands. This allows future analysis on the created snapshots.
Regions may optionally support triggering snapshots on demand.
(区域快照 (snapshot) 由驱动收集，可通过 read 或 dump 命令访问。这样可以对已创建的快照做后续分析。区域可以选择支持按需触发快照。)

Snapshot identifiers are scoped to the devlink instance, not a region.
All snapshots with the same snapshot id within a devlink instance
correspond to the same event.
(快照标识符的作用域是 devlink 实例，而不是某个区域。在一个 devlink 实例内，所有具有相同快照 id 的快照都对应同一事件。)

The major benefit to creating a region is to provide access to internal
address regions that are otherwise inaccessible to the user.
(创建区域的主要好处是提供对用户原本无法访问的内部地址区域的访问能力。)

Regions may also be used to provide an additional way to debug complex error
states, but see also Documentation/networking/devlink/devlink-health.rst
(区域还可用于提供调试复杂错误状态的另一种途径，但另见 Documentation/networking/devlink/devlink-health.rst)

Regions may optionally support capturing a snapshot on demand via the
``DEVLINK_CMD_REGION_NEW`` netlink message. A driver wishing to allow
requested snapshots must implement the ``.snapshot`` callback for the region
in its ``devlink_region_ops`` structure. If snapshot id is not set in
the ``DEVLINK_CMD_REGION_NEW`` request kernel will allocate one and send
the snapshot information to user space.
(区域可以选择支持通过 ``DEVLINK_CMD_REGION_NEW`` netlink 消息按需捕获快照。希望允许请求式快照的驱动必须在其 ``devlink_region_ops`` 结构中为该区域实现 ``.snapshot`` 回调。若 ``DEVLINK_CMD_REGION_NEW`` 请求中未设置快照 id，内核会分配一个，并将快照信息发送到用户空间。)

Regions may optionally allow directly reading from their contents without a
snapshot. Direct read requests are not atomic. In particular a read request
of size 256 bytes or larger will be split into multiple chunks. If atomic
access is required, use a snapshot. A driver wishing to enable this for a
region should implement the ``.read`` callback in the ``devlink_region_ops``
structure. User space can request a direct read by using the
``DEVLINK_ATTR_REGION_DIRECT`` attribute instead of specifying a snapshot
id.
(区域可以选择允许在不创建快照的情况下直接读取其内容。直接读取请求不是原子的。特别是，大小为 256 字节或更大的读取请求会被拆分成多个块。如果需要原子访问，请使用快照。希望为区域启用此功能的驱动应在 ``devlink_region_ops`` 结构中实现 ``.read`` 回调。用户空间可以通过使用 ``DEVLINK_ATTR_REGION_DIRECT`` 属性(而不是指定快照 id)来请求直接读取。)

example usage(用法示例)
-----------------------

.. code:: shell

    $ devlink region help
    $ devlink region show [ DEV/REGION ]
    $ devlink region del DEV/REGION snapshot SNAPSHOT_ID
    $ devlink region dump DEV/REGION [ snapshot SNAPSHOT_ID ]
    $ devlink region read DEV/REGION [ snapshot SNAPSHOT_ID ] address ADDRESS length LENGTH

    # Show all of the exposed regions with region sizes:
    $ devlink region show
    pci/0000:00:05.0/cr-space: size 1048576 snapshot [1 2] max 8
    pci/0000:00:05.0/fw-health: size 64 snapshot [1 2] max 8

    # Delete a snapshot using:
    $ devlink region del pci/0000:00:05.0/cr-space snapshot 1

    # Request an immediate snapshot, if supported by the region
    $ devlink region new pci/0000:00:05.0/cr-space
    pci/0000:00:05.0/cr-space: snapshot 5

    # Dump a snapshot:
    $ devlink region dump pci/0000:00:05.0/fw-health snapshot 1
    0000000000000000 0014 95dc 0014 9514 0035 1670 0034 db30
    0000000000000010 0000 0000 ffff ff04 0029 8c00 0028 8cc8
    0000000000000020 0016 0bb8 0016 1720 0000 0000 c00f 3ffc
    0000000000000030 bada cce5 bada cce5 bada cce5 bada cce5

    # Read a specific part of a snapshot:
    $ devlink region read pci/0000:00:05.0/fw-health snapshot 1 address 0 length 16
    0000000000000000 0014 95dc 0014 9514 0035 1670 0034 db30

    # Read from the region without a snapshot
    $ devlink region read pci/0000:00:05.0/fw-health address 16 length 16
    0000000000000010 0000 0000 ffff ff04 0029 8c00 0028 8cc8

As regions are likely very device or driver specific, no generic regions are
defined. See the driver-specific documentation files for information on the
specific regions a driver supports.
(由于区域通常非常特定于设备或驱动，因此没有定义通用区域。驱动支持哪些具体区域，请参阅驱动特定的文档文件。)
