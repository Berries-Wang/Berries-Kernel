.. SPDX-License-Identifier: GFDL-1.1-no-invariants-or-later
.. c:namespace:: V4L

.. _VIDIOC_DBG_G_REGISTER:

******************************************************************************************************
ioctl VIDIOC_DBG_G_REGISTER, VIDIOC_DBG_S_REGISTER(ioctl VIDIOC_DBG_G_REGISTER、VIDIOC_DBG_S_REGISTER)
******************************************************************************************************

Name(名称)
==========

VIDIOC_DBG_G_REGISTER - VIDIOC_DBG_S_REGISTER - Read or write hardware registers
(VIDIOC_DBG_G_REGISTER - VIDIOC_DBG_S_REGISTER - 读取或写入硬件寄存器)

Synopsis(概要)
==============

.. c:macro:: VIDIOC_DBG_G_REGISTER

``int ioctl(int fd, VIDIOC_DBG_G_REGISTER, struct v4l2_dbg_register *argp)``

.. c:macro:: VIDIOC_DBG_S_REGISTER

``int ioctl(int fd, VIDIOC_DBG_S_REGISTER, const struct v4l2_dbg_register *argp)``

Arguments(参数)
===============

``fd``
    File descriptor returned by :c:func:`open()`.
    (由 :c:func:`open()` 返回的文件描述符。)

``argp``
    Pointer to struct :c:type:`v4l2_dbg_register`.
    (指向 struct :c:type:`v4l2_dbg_register` 的指针。)

Description(描述)
=================

.. note::

    This is an :ref:`experimental` interface and may
    change in the future.
    (这是一个 :ref:`experimental` (实验性) 接口，将来可能会更改。)

For driver debugging purposes these ioctls allow test applications to
access hardware registers directly. Regular applications must not use
them.
(出于驱动调试目的，这些 ioctl 允许测试应用程序直接访问硬件寄存器。常规应用程序不得使用它们。)

Since writing or even reading registers can jeopardize the system
security, its stability and damage the hardware, both ioctls require
superuser privileges. Additionally the Linux kernel must be compiled
with the ``CONFIG_VIDEO_ADV_DEBUG`` option to enable these ioctls.
(由于写入甚至读取寄存器都可能危及系统安全、破坏系统稳定性并损坏硬件，这两个 ioctl 都需要超级用户权限。此外，Linux 内核必须编译了 ``CONFIG_VIDEO_ADV_DEBUG`` 选项才能启用这些 ioctl。)

To write a register applications must initialize all fields of a struct
:c:type:`v4l2_dbg_register` except for ``size`` and
call ``VIDIOC_DBG_S_REGISTER`` with a pointer to this structure. The
``match.type`` and ``match.addr`` or ``match.name`` fields select a chip
on the TV card, the ``reg`` field specifies a register number and the
``val`` field the value to be written into the register.
(要写入寄存器，应用程序必须初始化 struct :c:type:`v4l2_dbg_register` 中除 ``size`` 之外的所有字段，并以指向该结构的指针调用 ``VIDIOC_DBG_S_REGISTER`` 。 ``match.type`` 和 ``match.addr`` 或 ``match.name`` 字段用于选择电视卡上的一个芯片， ``reg`` 字段指定寄存器编号， ``val`` 字段指定要写入寄存器的值。)

To read a register applications must initialize the ``match.type``,
``match.addr`` or ``match.name`` and ``reg`` fields, and call
``VIDIOC_DBG_G_REGISTER`` with a pointer to this structure. On success
the driver stores the register value in the ``val`` field and the size
(in bytes) of the value in ``size``.
(要读取寄存器，应用程序必须初始化 ``match.type`` 、 ``match.addr`` 或 ``match.name`` 以及 ``reg`` 字段，并以指向该结构的指针调用 ``VIDIOC_DBG_G_REGISTER`` 。成功时，驱动会把寄存器值存储在 ``val`` 字段中，并把值的大小(以字节为单位)存储在 ``size`` 中。)

When ``match.type`` is ``V4L2_CHIP_MATCH_BRIDGE``, ``match.addr``
selects the nth non-sub-device chip on the TV card. The number zero
always selects the host chip, e. g. the chip connected to the PCI or USB
bus. You can find out which chips are present with the
:ref:`VIDIOC_DBG_G_CHIP_INFO` ioctl.
(当 ``match.type`` 为 ``V4L2_CHIP_MATCH_BRIDGE`` 时， ``match.addr`` 选择电视卡上的第 n 个非子设备芯片。数字 0 始终选择主芯片，例如连接到 PCI 或 USB 总线的芯片。你可以用 :ref:`VIDIOC_DBG_G_CHIP_INFO` ioctl 查明存在哪些芯片。)

When ``match.type`` is ``V4L2_CHIP_MATCH_SUBDEV``, ``match.addr``
selects the nth sub-device.
(当 ``match.type`` 为 ``V4L2_CHIP_MATCH_SUBDEV`` 时， ``match.addr`` 选择第 n 个子设备。)

These ioctls are optional, not all drivers may support them. However
when a driver supports these ioctls it must also support
:ref:`VIDIOC_DBG_G_CHIP_INFO`. Conversely
it may support ``VIDIOC_DBG_G_CHIP_INFO`` but not these ioctls.
(这些 ioctl 是可选的，并非所有驱动都支持它们。不过，当驱动支持这些 ioctl 时，它还必须支持 :ref:`VIDIOC_DBG_G_CHIP_INFO` 。反之，它可能支持 ``VIDIOC_DBG_G_CHIP_INFO`` 但不支持这些 ioctl。)

``VIDIOC_DBG_G_REGISTER`` and ``VIDIOC_DBG_S_REGISTER`` were introduced
in Linux 2.6.21, but their API was changed to the one described here in
kernel 2.6.29.
( ``VIDIOC_DBG_G_REGISTER`` 和 ``VIDIOC_DBG_S_REGISTER`` 在 Linux 2.6.21 中引入，但其 API 在内核 2.6.29 中更改为本文所述的形式。)

We recommended the v4l2-dbg utility over calling these ioctls directly.
It is available from the LinuxTV v4l-dvb repository; see
`https://linuxtv.org/repo/ <https://linuxtv.org/repo/>`__ for access
instructions.
(我们建议使用 v4l2-dbg 实用工具，而不是直接调用这些 ioctl。它可以从 LinuxTV v4l-dvb 仓库获得；访问说明见
`https://linuxtv.org/repo/ <https://linuxtv.org/repo/>`__ 。)

.. tabularcolumns:: |p{3.5cm}|p{3.5cm}|p{3.5cm}|p{6.6cm}|

.. c:type:: v4l2_dbg_match

.. flat-table:: struct v4l2_dbg_match
    :header-rows:  0
    :stub-columns: 0
    :widths:       1 1 2

    * - __u32
      - ``type``
      - See :ref:`chip-match-types` for a list of possible types.
	(可能类型的列表参见 :ref:`chip-match-types` 。)
    * - union {
      - (anonymous)
    * - __u32
      - ``addr``
      - Match a chip by this number, interpreted according to the ``type``
	field.
	(按此编号匹配芯片，依据 ``type`` 字段进行解释。)
    * - char
      - ``name[32]``
      - Match a chip by this name, interpreted according to the ``type``
	field. Currently unused.
	(按此名称匹配芯片，依据 ``type`` 字段进行解释。当前未使用。)
    * - }
      -


.. c:type:: v4l2_dbg_register

.. flat-table:: struct v4l2_dbg_register
    :header-rows:  0
    :stub-columns: 0

    * - struct v4l2_dbg_match
      - ``match``
      - How to match the chip, see :c:type:`v4l2_dbg_match`.
	(如何匹配芯片，参见 :c:type:`v4l2_dbg_match` 。)
    * - __u32
      - ``size``
      - The register size in bytes.(寄存器大小，以字节为单位。)
    * - __u64
      - ``reg``
      - A register number.(寄存器编号。)
    * - __u64
      - ``val``
      - The value read from, or to be written into the register.
	(从寄存器读取的值，或要写入寄存器的值。)


.. tabularcolumns:: |p{6.6cm}|p{2.2cm}|p{8.5cm}|

.. _chip-match-types:

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
(成功时返回 0，出错时返回 -1 并适当设置 ``errno`` 变量。通用错误码在 :ref:`Generic Error Codes <gen-errors>` 一章中描述。)

EPERM
    Insufficient permissions. Root privileges are required to execute
    these ioctls.
    (权限不足。执行这些 ioctl 需要 root 权限。)
