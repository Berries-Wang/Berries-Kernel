.. SPDX-License-Identifier: GFDL-1.1-no-invariants-or-later
.. c:namespace:: V4L

.. _VIDIOC_SUBDEV_QUERYCAP:

********************************************
ioctl VIDIOC_SUBDEV_QUERYCAP(查询子设备能力)
********************************************

Name(名称)
==========

VIDIOC_SUBDEV_QUERYCAP - Query sub-device capabilities(VIDIOC_SUBDEV_QUERYCAP - 查询子设备能力)

Synopsis(概要)
==============

.. c:macro:: VIDIOC_SUBDEV_QUERYCAP

``int ioctl(int fd, VIDIOC_SUBDEV_QUERYCAP, struct v4l2_subdev_capability *argp)``

Arguments(参数)
===============

``fd``
    File descriptor returned by :c:func:`open()`.
    (:c:func:`open()` 返回的文件描述符。)

``argp``
    Pointer to struct :c:type:`v4l2_subdev_capability`.
    (指向 struct :c:type:`v4l2_subdev_capability` 的指针。)

Description(描述)
=================

All V4L2 sub-devices support the ``VIDIOC_SUBDEV_QUERYCAP`` ioctl. It is used to
identify kernel devices compatible with this specification and to obtain
information about driver and hardware capabilities. The ioctl takes a pointer to
a struct :c:type:`v4l2_subdev_capability` which is filled by the driver. When
the driver is not compatible with this specification the ioctl returns
``ENOTTY`` error code.
(所有 V4L2 子设备都支持 ``VIDIOC_SUBDEV_QUERYCAP`` ioctl。它用于识别
与此规范兼容的内核设备，并获取驱动程序和硬件能力的信息。该 ioctl 接受
一个指向 struct :c:type:`v4l2_subdev_capability` 的指针，由驱动程序
填充。当驱动程序与此规范不兼容时，该 ioctl 返回 ``ENOTTY`` 错误码。)

.. tabularcolumns:: |p{1.5cm}|p{2.9cm}|p{12.9cm}|

.. c:type:: v4l2_subdev_capability

.. flat-table:: struct v4l2_subdev_capability
    :header-rows:  0
    :stub-columns: 0
    :widths:       3 4 20

    * - __u32
      - ``version``
      - Version number of the driver.
        (驱动程序的版本号。)

	The version reported is provided by the V4L2 subsystem following the
	kernel numbering scheme. However, it may not always return the same
	version as the kernel if, for example, a stable or
	distribution-modified kernel uses the V4L2 stack from a newer kernel.
        (报告的版本由 V4L2 子系统按照内核编号方案提供。但是，它不一定
        总是返回与内核相同的版本，例如，当稳定版或发行版修改过的内核
        使用来自更新内核的 V4L2 栈时。)

	The version number is formatted using the ``KERNEL_VERSION()``
	macro:
        (版本号使用 ``KERNEL_VERSION()`` 宏格式化：)
    * - :cspan:`2`

	``#define KERNEL_VERSION(a,b,c) (((a) << 16) + ((b) << 8) + (c))``

	``__u32 version = KERNEL_VERSION(0, 8, 1);``

	``printf ("Version: %u.%u.%u\\n",``

	``(version >> 16) & 0xFF, (version >> 8) & 0xFF, version & 0xFF);``
    * - __u32
      - ``capabilities``
      - Sub-device capabilities of the opened device, see
	:ref:`subdevice-capabilities`.
        (所打开设备的子设备能力，参见 :ref:`subdevice-capabilities`。)
    * - __u32
      - ``reserved``\ [14]
      - Reserved for future extensions. Set to 0 by the V4L2 core.
        (保留用于未来的扩展。由 V4L2 核心置为 0。)

.. tabularcolumns:: |p{6.8cm}|p{2.4cm}|p{8.1cm}|

.. _subdevice-capabilities:

.. cssclass:: longtable

.. flat-table:: Sub-Device Capabilities Flags
    :header-rows:  0
    :stub-columns: 0
    :widths:       3 1 4

    * - V4L2_SUBDEV_CAP_RO_SUBDEV
      - 0x00000001
      - The sub-device device node is registered in read-only mode.
	Access to the sub-device ioctls that modify the device state is
	restricted. Refer to each individual subdevice ioctl documentation
	for a description of which restrictions apply to a read-only sub-device.
        (子设备设备节点以只读模式注册。对修改设备状态的子设备 ioctl 的
        访问受到限制。有关哪些限制适用于只读子设备的说明，请参阅每个
        具体的子设备 ioctl 文档。)

Return Value(返回值)
====================

On success 0 is returned, on error -1 and the ``errno`` variable is set
appropriately. The generic error codes are described at the
:ref:`Generic Error Codes <gen-errors>` chapter.
(成功时返回 0，出错时返回 -1 并适当设置 ``errno`` 变量。通用错误码在
:ref:`Generic Error Codes <gen-errors>` 一章中描述。)

ENOTTY
    The device node is not a V4L2 sub-device.
    (该设备节点不是 V4L2 子设备。)
