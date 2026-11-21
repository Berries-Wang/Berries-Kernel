.. SPDX-License-Identifier: GFDL-1.1-no-invariants-or-later
.. c:namespace:: V4L

.. _VIDIOC_G_EDID:

==================================================================================================================
ioctl VIDIOC_G_EDID, VIDIOC_S_EDID, VIDIOC_SUBDEV_G_EDID, VIDIOC_SUBDEV_S_EDID(获取或设置视频接收器/发送器的 EDID)
==================================================================================================================

Name(名称)
==========

VIDIOC_G_EDID - VIDIOC_S_EDID - VIDIOC_SUBDEV_G_EDID - VIDIOC_SUBDEV_S_EDID - Get or set the EDID of a video receiver/transmitter
(VIDIOC_G_EDID - VIDIOC_S_EDID - VIDIOC_SUBDEV_G_EDID - VIDIOC_SUBDEV_S_EDID - 获取或设置视频接收器/发送器的 EDID)

Synopsis(概要)
==============

.. c:macro:: VIDIOC_G_EDID

``int ioctl(int fd, VIDIOC_G_EDID, struct v4l2_edid *argp)``

.. c:macro:: VIDIOC_S_EDID

``int ioctl(int fd, VIDIOC_S_EDID, struct v4l2_edid *argp)``

.. c:macro:: VIDIOC_SUBDEV_G_EDID

``int ioctl(int fd, VIDIOC_SUBDEV_G_EDID, struct v4l2_edid *argp)``

.. c:macro:: VIDIOC_SUBDEV_S_EDID

``int ioctl(int fd, VIDIOC_SUBDEV_S_EDID, struct v4l2_edid *argp)``

Arguments(参数)
===============

``fd``
    File descriptor returned by :c:func:`open()`.
    (由 :c:func:`open()` 返回的文件描述符。)

``argp``
   Pointer to struct :c:type:`v4l2_edid`.
   (指向 struct :c:type:`v4l2_edid` 的指针。)

Description(描述)
=================

These ioctls can be used to get or set an EDID associated with an input
from a receiver or an output of a transmitter device. They can be used
with subdevice nodes (/dev/v4l-subdevX) or with video nodes
(/dev/videoX).
(这些 ioctl 可用于获取或设置与接收器的输入或发送器设备的输出相关联的
EDID。它们既可用于子设备节点 (/dev/v4l-subdevX)，也可用于视频节点
(/dev/videoX)。)

When used with video nodes the ``pad`` field represents the input (for
video capture devices) or output (for video output devices) index as is
returned by :ref:`VIDIOC_ENUMINPUT` and
:ref:`VIDIOC_ENUMOUTPUT` respectively. When used
with subdevice nodes the ``pad`` field represents the input or output
pad of the subdevice. If there is no EDID support for the given ``pad``
value, then the ``EINVAL`` error code will be returned.
(与视频节点一起使用时，``pad`` 字段表示输入（对视频采集设备而言）或输出
（对视频输出设备而言）索引，分别由 :ref:`VIDIOC_ENUMINPUT` 和
:ref:`VIDIOC_ENUMOUTPUT` 返回。与子设备节点一起使用时，``pad`` 字段表示
子设备的输入或输出 pad。如果给定的 ``pad`` 值不支持 EDID，则将返回
``EINVAL`` 错误码。)

To get the EDID data the application has to fill in the ``pad``,
``start_block``, ``blocks`` and ``edid`` fields, zero the ``reserved``
array and call :ref:`VIDIOC_G_EDID <VIDIOC_G_EDID>`. The current EDID from block
``start_block`` and of size ``blocks`` will be placed in the memory
``edid`` points to. The ``edid`` pointer must point to memory at least
``blocks`` * 128 bytes large (the size of one block is 128 bytes).
(要获取 EDID 数据，应用程序必须填写 ``pad``、``start_block``、``blocks``
和 ``edid`` 字段，将 ``reserved`` 数组清零，并调用 :ref:`VIDIOC_G_EDID
<VIDIOC_G_EDID>`。从块 ``start_block`` 开始、大小为 ``blocks`` 的当前 EDID
将被放入 ``edid`` 指向的内存中。``edid`` 指针必须指向至少 ``blocks`` * 128
字节大的内存（一个块的大小为 128 字节）。)

If there are fewer blocks than specified, then the driver will set
``blocks`` to the actual number of blocks. If there are no EDID blocks
available at all, then the error code ``ENODATA`` is set.
(如果块数比指定的少，驱动会把 ``blocks`` 设置为实际的块数。如果完全没有
可用的 EDID 块，则设置错误码 ``ENODATA``。)

If blocks have to be retrieved from the sink, then this call will block
until they have been read.
(如果必须从接收端检索块，则此调用将阻塞，直到这些块被读取完毕。)

If ``start_block`` and ``blocks`` are both set to 0 when
:ref:`VIDIOC_G_EDID <VIDIOC_G_EDID>` is called, then the driver will set ``blocks`` to the
total number of available EDID blocks and it will return 0 without
copying any data. This is an easy way to discover how many EDID blocks
there are.
(如果在调用 :ref:`VIDIOC_G_EDID <VIDIOC_G_EDID>` 时 ``start_block`` 和
``blocks`` 都设置为 0，则驱动会把 ``blocks`` 设置为可用 EDID 块的总数，并
在不复制任何数据的情况下返回 0。这是获知有多少个 EDID 块的简单方法。)

.. note::

   If there are no EDID blocks available at all, then
   the driver will set ``blocks`` to 0 and it returns 0.
   (如果完全没有可用的 EDID 块，则驱动会把 ``blocks`` 设置为 0 并返回
   0。)

To set the EDID blocks of a receiver the application has to fill in the
``pad``, ``blocks`` and ``edid`` fields, set ``start_block`` to 0 and
zero the ``reserved`` array. It is not possible to set part of an EDID,
it is always all or nothing. Setting the EDID data is only valid for
receivers as it makes no sense for a transmitter.
(要设置接收器的 EDID 块，应用程序必须填写 ``pad``、``blocks`` 和
``edid`` 字段，把 ``start_block`` 设置为 0，并将 ``reserved`` 数组清零。
无法只设置 EDID 的一部分，要么全部设置，要么什么都不设置。设置 EDID 数据
只对接收器有效，对发送器来说没有意义。)

The driver assumes that the full EDID is passed in. If there are more
EDID blocks than the hardware can handle then the EDID is not written,
but instead the error code ``E2BIG`` is set and ``blocks`` is set to the
maximum that the hardware supports. If ``start_block`` is any value
other than 0 then the error code ``EINVAL`` is set.
(驱动假定传入的是完整 EDID。如果 EDID 块数超过硬件能处理的数量，则不会
写入 EDID，而是设置错误码 ``E2BIG``，并把 ``blocks`` 设置为硬件支持的
最大值。如果 ``start_block`` 是 0 以外的任何值，则设置错误码 ``EINVAL``。)

To disable an EDID you set ``blocks`` to 0. Depending on the hardware
this will drive the hotplug pin low and/or block the source from reading
the EDID data in some way. In any case, the end result is the same: the
EDID is no longer available.
(要禁用 EDID，请把 ``blocks`` 设置为 0。根据硬件的不同，这会把热插拔引脚
拉低，和/或以某种方式阻止源端读取 EDID 数据。无论如何，最终结果都是
相同的：EDID 不再可用。)

.. c:type:: v4l2_edid

.. tabularcolumns:: |p{4.4cm}|p{4.4cm}|p{8.5cm}|

.. flat-table:: struct v4l2_edid
    :header-rows:  0
    :stub-columns: 0
    :widths:       1 1 2

    * - __u32
      - ``pad``
      - Pad for which to get/set the EDID blocks. When used with a video
	device node the pad represents the input or output index as
	returned by :ref:`VIDIOC_ENUMINPUT` and
	:ref:`VIDIOC_ENUMOUTPUT` respectively.
	(要为其获取/设置 EDID 块的 pad。与视频设备节点一起使用时，pad 表示
	输入或输出索引，分别由 :ref:`VIDIOC_ENUMINPUT` 和
	:ref:`VIDIOC_ENUMOUTPUT` 返回。)
    * - __u32
      - ``start_block``
      - Read the EDID from starting with this block. Must be 0 when
	setting the EDID.
	(从此块开始读取 EDID。设置 EDID 时必须为 0。)
    * - __u32
      - ``blocks``
      - The number of blocks to get or set. Must be less or equal to 256
	(the maximum number of blocks as defined by the standard). When
	you set the EDID and ``blocks`` is 0, then the EDID is disabled or
	erased.
	(要获取或设置的块数。必须小于或等于 256（标准定义的最大块数）。
	当你设置 EDID 且 ``blocks`` 为 0 时，EDID 被禁用或擦除。)
    * - __u32
      - ``reserved``\ [5]
      - Reserved for future extensions. Applications and drivers must set
	the array to zero.
	(为未来扩展保留。应用程序和驱动必须将数组置零。)
    * - __u8 *
      - ``edid``
      - Pointer to memory that contains the EDID. The minimum size is
	``blocks`` * 128.
	(指向包含 EDID 的内存的指针。最小大小为 ``blocks`` * 128。)

Return Value(返回值)
====================

On success 0 is returned, on error -1 and the ``errno`` variable is set
appropriately. The generic error codes are described at the
:ref:`Generic Error Codes <gen-errors>` chapter.
(成功时返回 0，出错时返回 -1 并适当设置 ``errno`` 变量。通用错误码在
:ref:`Generic Error Codes <gen-errors>` 一章中描述。)

``ENODATA``
    The EDID data is not available.
    (EDID 数据不可用。)

``E2BIG``
    The EDID data you provided is more than the hardware can handle.
    (你提供的 EDID 数据超过了硬件能处理的数量。)
