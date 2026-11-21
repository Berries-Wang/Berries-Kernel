.. SPDX-License-Identifier: GFDL-1.1-no-invariants-or-later
.. c:namespace:: V4L

.. _VIDIOC_SUBDEV_G_CLIENT_CAP:

**********************************************************************************
ioctl VIDIOC_SUBDEV_G_CLIENT_CAP, VIDIOC_SUBDEV_S_CLIENT_CAP(获取或设置客户端能力)
**********************************************************************************

Name(名称)
==========

VIDIOC_SUBDEV_G_CLIENT_CAP - VIDIOC_SUBDEV_S_CLIENT_CAP - Get or set client
capabilities.
(VIDIOC_SUBDEV_G_CLIENT_CAP - VIDIOC_SUBDEV_S_CLIENT_CAP - 获取或设置
客户端能力。)

Synopsis(概要)
==============

.. c:macro:: VIDIOC_SUBDEV_G_CLIENT_CAP

``int ioctl(int fd, VIDIOC_SUBDEV_G_CLIENT_CAP, struct v4l2_subdev_client_capability *argp)``

.. c:macro:: VIDIOC_SUBDEV_S_CLIENT_CAP

``int ioctl(int fd, VIDIOC_SUBDEV_S_CLIENT_CAP, struct v4l2_subdev_client_capability *argp)``

Arguments(参数)
===============

``fd``
    File descriptor returned by :ref:`open() <func-open>`.
    (由 :ref:`open() <func-open>` 返回的文件描述符。)

``argp``
    Pointer to struct :c:type:`v4l2_subdev_client_capability`.
    (指向 struct :c:type:`v4l2_subdev_client_capability` 的指针。)

Description(描述)
=================

These ioctls are used to get and set the client (the application using the
subdevice ioctls) capabilities. The client capabilities are stored in the file
handle of the opened subdev device node, and the client must set the
capabilities for each opened subdev separately.
(这些 ioctl 用于获取和设置客户端(使用子设备 ioctl 的应用程序)的
能力。客户端能力存储在已打开的 subdev 设备节点的文件句柄中，客户端
必须为每个打开的 subdev 单独设置能力。)

By default no client capabilities are set when a subdev device node is opened.
(默认情况下，打开 subdev 设备节点时不设置任何客户端能力。)

The purpose of the client capabilities are to inform the kernel of the behavior
of the client, mainly related to maintaining compatibility with different
kernel and userspace versions.
(客户端能力的目的在于告知内核客户端的行为，主要与保持不同内核和用户
空间版本之间的兼容性有关。)

The ``VIDIOC_SUBDEV_G_CLIENT_CAP`` ioctl returns the current client capabilities
associated with the file handle ``fd``.
(``VIDIOC_SUBDEV_G_CLIENT_CAP`` ioctl 返回与文件句柄 ``fd`` 关联的
当前客户端能力。)

The ``VIDIOC_SUBDEV_S_CLIENT_CAP`` ioctl sets client capabilities for the file
handle ``fd``. The new capabilities fully replace the current capabilities, the
ioctl can therefore also be used to remove capabilities that have previously
been set.
(``VIDIOC_SUBDEV_S_CLIENT_CAP`` ioctl 为文件句柄 ``fd`` 设置客户端
能力。新能力完全替换当前能力，因此该 ioctl 也可以用于移除之前设置的
能力。)

``VIDIOC_SUBDEV_S_CLIENT_CAP`` modifies the struct
:c:type:`v4l2_subdev_client_capability` to reflect the capabilities that have
been accepted. A common case for the kernel not accepting a capability is that
the kernel is older than the headers the userspace uses, and thus the capability
is unknown to the kernel.
(``VIDIOC_SUBDEV_S_CLIENT_CAP`` 会修改 struct
:c:type:`v4l2_subdev_client_capability`，以反映已被接受的能力。内核
不接受某个能力的常见情形是：内核比用户空间所用的头文件更旧，因而内核
不知道该能力。)

.. tabularcolumns:: |p{1.5cm}|p{2.9cm}|p{12.9cm}|

.. c:type:: v4l2_subdev_client_capability

.. flat-table:: struct v4l2_subdev_client_capability
    :header-rows:  0
    :stub-columns: 0
    :widths:       3 4 20

    * - __u64
      - ``capabilities``
      - Sub-device client capabilities of the opened device.
        (所打开设备的子设备客户端能力。)

.. tabularcolumns:: |p{6.8cm}|p{2.4cm}|p{8.1cm}|

.. flat-table:: Client Capabilities
    :header-rows:  1

    * - Capability(能力)
      - Description(说明)
    * - ``V4L2_SUBDEV_CLIENT_CAP_STREAMS``
      - The client is aware of streams. Setting this flag enables the use
        of 'stream' fields (referring to the stream number) with various
        ioctls. If this is not set (which is the default), the 'stream' fields
        will be forced to 0 by the kernel.
        (客户端知晓流(streams)。设置此标志后，可以在各种 ioctl 中使用
        'stream' 字段(指代流编号)。如果未设置(这是默认情况)，内核会
        把 'stream' 字段强制置为 0。)
    * - ``V4L2_SUBDEV_CLIENT_CAP_INTERVAL_USES_WHICH``
      - The client is aware of the :c:type:`v4l2_subdev_frame_interval`
        ``which`` field. If this is not set (which is the default), the
        ``which`` field is forced to ``V4L2_SUBDEV_FORMAT_ACTIVE`` by the
        kernel.
        (客户端知晓 :c:type:`v4l2_subdev_frame_interval` 的 ``which``
        字段。如果未设置(这是默认情况)，内核会把 ``which`` 字段强制
        设为 ``V4L2_SUBDEV_FORMAT_ACTIVE``。)

Return Value(返回值)
====================

On success 0 is returned, on error -1 and the ``errno`` variable is set
appropriately. The generic error codes are described at the
:ref:`Generic Error Codes <gen-errors>` chapter.
(成功时返回 0，出错时返回 -1 并适当设置 ``errno`` 变量。通用错误码在
:ref:`Generic Error Codes <gen-errors>` 一章中描述。)

ENOIOCTLCMD
   The kernel does not support this ioctl.
   (内核不支持该 ioctl。)
