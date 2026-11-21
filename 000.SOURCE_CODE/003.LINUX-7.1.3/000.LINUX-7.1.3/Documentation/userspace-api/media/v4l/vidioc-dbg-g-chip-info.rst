.. SPDX-License-Identifier: GFDL-1.1-no-invariants-or-later
.. c:namespace:: V4L

.. _VIDIOC_DBG_G_CHIP_INFO:

************************************************
ioctl VIDIOC_DBG_G_CHIP_INFO(标识电视卡上的芯片)
************************************************

Name(名称)
==========

VIDIOC_DBG_G_CHIP_INFO - Identify the chips on a TV card
(VIDIOC_DBG_G_CHIP_INFO - 识别电视卡上的芯片)

Synopsis(概要)
==============

.. c:macro:: VIDIOC_DBG_G_CHIP_INFO

``int ioctl(int fd, VIDIOC_DBG_G_CHIP_INFO, struct v4l2_dbg_chip_info *argp)``

Arguments(参数)
===============

``fd``
    File descriptor returned by :c:func:`open()`.
    (由 :c:func:`open()` 返回的文件描述符。)

``argp``
    Pointer to struct :c:type:`v4l2_dbg_chip_info`.
    (指向 struct :c:type:`v4l2_dbg_chip_info` 的指针。)

Description(描述)
=================

.. note::

    This is an :ref:`experimental` interface and may
    change in the future.
    (这是一个 :ref:`experimental` 实验性接口，将来可能发生变化。)

For driver debugging purposes this ioctl allows test applications to
query the driver about the chips present on the TV card. Regular
applications must not use it. When you found a chip specific bug, please
contact the linux-media mailing list
(`https://linuxtv.org/lists.php <https://linuxtv.org/lists.php>`__)
so it can be fixed.
(出于驱动调试目的，此 ioctl 允许测试应用程序向驱动查询电视卡上存在的芯片。
常规应用程序不得使用它。当你发现一个芯片特有的 bug 时，请联系 linux-media
邮件列表 (`https://linuxtv.org/lists.php <https://linuxtv.org/lists.php>`__)
以便修复。)

Additionally the Linux kernel must be compiled with the
``CONFIG_VIDEO_ADV_DEBUG`` option to enable this ioctl.
(此外，Linux 内核编译时必须带有 ``CONFIG_VIDEO_ADV_DEBUG`` 选项才能启用此
ioctl。)

To query the driver applications must initialize the ``match.type`` and
``match.addr`` or ``match.name`` fields of a struct
:c:type:`v4l2_dbg_chip_info` and call
:ref:`VIDIOC_DBG_G_CHIP_INFO` with a pointer to this structure. On success
the driver stores information about the selected chip in the ``name``
and ``flags`` fields.
(要查询驱动，应用程序必须初始化 struct :c:type:`v4l2_dbg_chip_info` 的
``match.type`` 和 ``match.addr`` 或 ``match.name`` 字段，并用指向该结构体的
指针调用 :ref:`VIDIOC_DBG_G_CHIP_INFO`。成功时，驱动会把所选芯片的信息存储
在 ``name`` 和 ``flags`` 字段中。)

When ``match.type`` is ``V4L2_CHIP_MATCH_BRIDGE``, ``match.addr``
selects the nth bridge 'chip' on the TV card. You can enumerate all
chips by starting at zero and incrementing ``match.addr`` by one until
:ref:`VIDIOC_DBG_G_CHIP_INFO` fails with an ``EINVAL`` error code. The number
zero always selects the bridge chip itself, e. g. the chip connected to
the PCI or USB bus. Non-zero numbers identify specific parts of the
bridge chip such as an AC97 register block.
(当 ``match.type`` 为 ``V4L2_CHIP_MATCH_BRIDGE`` 时，``match.addr`` 选择电视
卡上第 n 个桥接 "chip"。你可以从零开始、每次将 ``match.addr`` 加 1 来枚举
所有芯片，直到 :ref:`VIDIOC_DBG_G_CHIP_INFO` 返回 ``EINVAL`` 错误码为止。
编号 0 始终选择桥接芯片本身，例如连接到 PCI 或 USB 总线的芯片。非零编号
标识桥接芯片的特定部分，例如 AC97 寄存器块。)

When ``match.type`` is ``V4L2_CHIP_MATCH_SUBDEV``, ``match.addr``
selects the nth sub-device. This allows you to enumerate over all
sub-devices.
(当 ``match.type`` 为 ``V4L2_CHIP_MATCH_SUBDEV`` 时，``match.addr`` 选择第
n 个子设备。这允许你枚举所有子设备。)

On success, the ``name`` field will contain a chip name and the
``flags`` field will contain ``V4L2_CHIP_FL_READABLE`` if the driver
supports reading registers from the device or ``V4L2_CHIP_FL_WRITABLE``
if the driver supports writing registers to the device.
(成功时，``name`` 字段将包含芯片名称；如果驱动支持从设备读取寄存器，
``flags`` 字段将包含 ``V4L2_CHIP_FL_READABLE``；如果驱动支持向设备写入
寄存器，则包含 ``V4L2_CHIP_FL_WRITABLE``。)

We recommended the v4l2-dbg utility over calling this ioctl directly. It
is available from the LinuxTV v4l-dvb repository; see
`https://linuxtv.org/repo/ <https://linuxtv.org/repo/>`__ for access
instructions.
(我们建议使用 v4l2-dbg 实用工具，而不是直接调用此 ioctl。它可从 LinuxTV
v4l-dvb 仓库获取；访问方法参见
`https://linuxtv.org/repo/ <https://linuxtv.org/repo/>`__ 。)

.. tabularcolumns:: |p{3.5cm}|p{3.5cm}|p{3.5cm}|p{6.6cm}|

.. _name-v4l2-dbg-match:

.. flat-table:: struct v4l2_dbg_match
    :header-rows:  0
    :stub-columns: 0
    :widths:       1 1 2

    * - __u32
      - ``type``
      - See :ref:`name-chip-match-types` for a list of possible types.
	(可能类型的列表参见 :ref:`name-chip-match-types`。)
    * - union {
      - (anonymous)
    * - __u32
      - ``addr``
      - Match a chip by this number, interpreted according to the ``type``
	field.
	(按此编号匹配芯片，根据 ``type`` 字段进行解释。)
    * - char
      - ``name[32]``
      - Match a chip by this name, interpreted according to the ``type``
	field. Currently unused.
	(按此名称匹配芯片，根据 ``type`` 字段进行解释。目前未使用。)
    * - }
      -


.. tabularcolumns:: |p{4.4cm}|p{4.4cm}|p{8.5cm}|

.. c:type:: v4l2_dbg_chip_info

.. flat-table:: struct v4l2_dbg_chip_info
    :header-rows:  0
    :stub-columns: 0
    :widths:       1 1 2

    * - struct v4l2_dbg_match
      - ``match``
      - How to match the chip, see :ref:`name-v4l2-dbg-match`.
	(如何匹配芯片，参见 :ref:`name-v4l2-dbg-match`。)
    * - char
      - ``name[32]``
      - The name of the chip.
	(芯片的名称。)
    * - __u32
      - ``flags``
      - Set by the driver. If ``V4L2_CHIP_FL_READABLE`` is set, then the
	driver supports reading registers from the device. If
	``V4L2_CHIP_FL_WRITABLE`` is set, then it supports writing
	registers.
	(由驱动设置。若置位 ``V4L2_CHIP_FL_READABLE``，则驱动支持从设备读取
	寄存器。若置位 ``V4L2_CHIP_FL_WRITABLE``，则支持向设备写入寄存器。)
    * - __u32
      - ``reserved[8]``
      - Reserved fields, both application and driver must set these to 0.
	(保留字段，应用程序和驱动都必须将其置为 0。)


.. tabularcolumns:: |p{6.6cm}|p{2.2cm}|p{8.5cm}|

.. _name-chip-match-types:

.. flat-table:: Chip Match Types
    :header-rows:  0
    :stub-columns: 0
    :widths:       3 1 4

    * - ``V4L2_CHIP_MATCH_BRIDGE``
      - 0
      - Match the nth chip on the card, zero for the bridge chip. Does not
	match sub-devices.
	(匹配卡上的第 n 个芯片，0 表示桥接芯片。不匹配子设备。)
    * - ``V4L2_CHIP_MATCH_SUBDEV``
      - 4
      - Match the nth sub-device.
	(匹配第 n 个子设备。)

Return Value(返回值)
====================

On success 0 is returned, on error -1 and the ``errno`` variable is set
appropriately. The generic error codes are described at the
:ref:`Generic Error Codes <gen-errors>` chapter.
(成功时返回 0，出错时返回 -1 并适当设置 ``errno`` 变量。通用错误码在
:ref:`Generic Error Codes <gen-errors>` 一章中描述。)

EINVAL
    The ``match_type`` is invalid or no device could be matched.
    (``match_type`` 无效，或无法匹配到任何设备。)
