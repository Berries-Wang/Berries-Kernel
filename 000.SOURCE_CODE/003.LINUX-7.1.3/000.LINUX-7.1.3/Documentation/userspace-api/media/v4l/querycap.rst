.. SPDX-License-Identifier: GFDL-1.1-no-invariants-or-later
.. c:namespace:: V4L

.. _querycap:

*******************************
Querying Capabilities(查询能力)
*******************************

Because V4L2 covers a wide variety of devices not all aspects of the API
are equally applicable to all types of devices. Furthermore devices of
the same type have different capabilities and this specification permits
the omission of a few complicated and less important parts of the API.
(由于 V4L2 涵盖种类繁多的设备，并非 API 的所有方面都同等地适用于所有类型
的设备。此外，同类型设备的能力也各不相同，本规范允许省略 API 中一些复杂且
较不重要的部分。)

The :ref:`VIDIOC_QUERYCAP` ioctl is available to
check if the kernel device is compatible with this specification, and to
query the :ref:`functions <devices>` and :ref:`I/O methods <io>`
supported by the device.
(:ref:`VIDIOC_QUERYCAP` ioctl 可用于检查内核设备是否与本规范兼容，并查询
设备支持的 :ref:`functions <devices>`（功能）与 :ref:`I/O methods <io>`
（I/O 方法）。)

Starting with kernel version 3.1, :ref:`VIDIOC_QUERYCAP`
will return the V4L2 API version used by the driver, with generally
matches the Kernel version. There's no need of using
:ref:`VIDIOC_QUERYCAP` to check if a specific ioctl
is supported, the V4L2 core now returns ``ENOTTY`` if a driver doesn't
provide support for an ioctl.
(从内核版本 3.1 开始，:ref:`VIDIOC_QUERYCAP` 会返回驱动所使用的 V4L2 API
版本，它通常与内核版本一致。无需使用 :ref:`VIDIOC_QUERYCAP` 来检查某个特定
ioctl 是否受支持——如果驱动未提供对某个 ioctl 的支持，V4L2 核心现在会返回
``ENOTTY``。)

Other features can be queried by calling the respective ioctl, for
example :ref:`VIDIOC_ENUMINPUT` to learn about the
number, types and names of video connectors on the device. Although
abstraction is a major objective of this API, the
:ref:`VIDIOC_QUERYCAP` ioctl also allows driver
specific applications to reliably identify the driver.
(其他特性可以通过调用相应的 ioctl 来查询，例如 :ref:`VIDIOC_ENUMINPUT` 可
了解设备上视频连接器的数量、类型与名称。虽然抽象是本 API 的主要目标之一，
:ref:`VIDIOC_QUERYCAP` ioctl 也允许驱动专属的应用程序可靠地识别驱动。)

All V4L2 drivers must support :ref:`VIDIOC_QUERYCAP`.
Applications should always call this ioctl after opening the device.
(所有 V4L2 驱动都必须支持 :ref:`VIDIOC_QUERYCAP`。应用程序在打开设备后应
始终调用此 ioctl。)
