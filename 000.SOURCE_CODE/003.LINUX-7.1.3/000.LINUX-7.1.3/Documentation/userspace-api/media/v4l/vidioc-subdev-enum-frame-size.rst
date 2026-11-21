.. SPDX-License-Identifier: GFDL-1.1-no-invariants-or-later
.. c:namespace:: V4L

.. _VIDIOC_SUBDEV_ENUM_FRAME_SIZE:

***********************************
ioctl VIDIOC_SUBDEV_ENUM_FRAME_SIZE
***********************************

Name(名称)
==========

VIDIOC_SUBDEV_ENUM_FRAME_SIZE - Enumerate media bus frame sizes
(VIDIOC_SUBDEV_ENUM_FRAME_SIZE - 枚举媒体总线帧尺寸。)

Synopsis(概要)
==============

.. c:macro:: VIDIOC_SUBDEV_ENUM_FRAME_SIZE

``int ioctl(int fd, VIDIOC_SUBDEV_ENUM_FRAME_SIZE, struct v4l2_subdev_frame_size_enum * argp)``

Arguments(参数)
===============

``fd``
    File descriptor returned by :c:func:`open()`.
    (由 :c:func:`open()` 返回的文件描述符。)

``argp``
    Pointer to struct :c:type:`v4l2_subdev_frame_size_enum`.
    (指向 struct :c:type:`v4l2_subdev_frame_size_enum` 的指针。)

Description(描述)
=================

This ioctl allows applications to access the enumeration of frame sizes
supported by a sub-device on the specified pad
for the specified media bus format.
Supported formats can be retrieved with the
:ref:`VIDIOC_SUBDEV_ENUM_MBUS_CODE`
ioctl.
(此 ioctl 允许应用程序枚举子设备在指定 pad 上针对指定媒体总线格式所支持的帧尺寸。可使用 :ref:`VIDIOC_SUBDEV_ENUM_MBUS_CODE` ioctl 检索支持的格式。)

The enumerations are defined by the driver, and indexed using the ``index`` field
of the struct :c:type:`v4l2_subdev_frame_size_enum`.
Each pair of ``pad`` and ``code`` correspond to a separate enumeration.
Each enumeration starts with the ``index`` of 0, and
the lowest invalid index marks the end of the enumeration.
(枚举由驱动程序定义，并使用 struct :c:type:`v4l2_subdev_frame_size_enum` 的 ``index`` 字段进行索引。每对 ``pad`` 和 ``code`` 对应一个单独的枚举。每个枚举从 ``index`` 为 0 开始，最小的无效索引标记枚举的结束。)

Therefore, to enumerate frame sizes allowed on the specified pad
and using the specified mbus format, initialize the
``pad``, ``which``, and ``code`` fields to desired values,
and set ``index`` to 0.
Then call the :ref:`VIDIOC_SUBDEV_ENUM_FRAME_SIZE` ioctl with a pointer to the
structure.
(因此，要枚举在指定 pad 上使用指定 mbus 格式所允许的帧尺寸，将 ``pad`` 、 ``which`` 和 ``code`` 字段初始化为所需的值，并把 ``index`` 设为 0。然后以指向该结构的指针调用 :ref:`VIDIOC_SUBDEV_ENUM_FRAME_SIZE` ioctl。)

A successful call will return with minimum and maximum frame sizes filled in.
Repeat with increasing ``index`` until ``EINVAL`` is received.
``EINVAL`` means that either no more entries are available in the enumeration,
or that an input parameter was invalid.
(调用成功时将返回填好的最小和最大帧尺寸。以递增的 ``index`` 重复调用，直到收到 ``EINVAL`` 。``EINVAL`` 表示枚举中已没有更多条目，或者某个输入参数无效。)

Sub-devices that only support discrete frame sizes (such as most
sensors) will return one or more frame sizes with identical minimum and
maximum values.
(只支持离散帧尺寸的子设备（例如大多数传感器）将返回一个或多个最小值和最大值相同的帧尺寸。)

Not all possible sizes in given [minimum, maximum] ranges need to be
supported. For instance, a scaler that uses a fixed-point scaling ratio
might not be able to produce every frame size between the minimum and
maximum values. Applications must use the
:ref:`VIDIOC_SUBDEV_S_FMT <VIDIOC_SUBDEV_G_FMT>` ioctl to try the
sub-device for an exact supported frame size.
(给定的 [最小值, 最大值] 范围内并非所有可能的尺寸都必须受支持。例如，使用定点缩放比例的缩放器可能无法产生介于最小值和最大值之间的每一种帧尺寸。应用程序必须使用 :ref:`VIDIOC_SUBDEV_S_FMT <VIDIOC_SUBDEV_G_FMT>` ioctl 来尝试子设备，以获得确切支持的帧尺寸。)

Available frame sizes may depend on the current 'try' formats at other
pads of the sub-device, as well as on the current active links and the
current values of V4L2 controls. See
:ref:`VIDIOC_SUBDEV_G_FMT` for more
information about try formats.
(可用的帧尺寸可能取决于子设备其他 pad 上当前的 'try' 格式，以及当前的活动链路和 V4L2 控件的当前值。有关 try 格式的更多信息，参见 :ref:`VIDIOC_SUBDEV_G_FMT` 。)

.. c:type:: v4l2_subdev_frame_size_enum

.. tabularcolumns:: |p{4.4cm}|p{4.4cm}|p{8.5cm}|

.. flat-table:: struct v4l2_subdev_frame_size_enum
    :header-rows:  0
    :stub-columns: 0
    :widths:       1 1 2

    * - __u32
      - ``index``
      - Index of the frame size in the enumeration belonging to the given pad
	and format. Filled in by the application.
        (给定 pad 和格式所属枚举中帧尺寸的索引。由应用程序填写。)
    * - __u32
      - ``pad``
      - Pad number as reported by the media controller API.
	Filled in by the application.
        (媒体控制器 API 所报告的 pad 编号。由应用程序填写。)
    * - __u32
      - ``code``
      - The media bus format code, as defined in
	:ref:`v4l2-mbus-format`. Filled in by the application.
        (媒体总线格式代码，定义见 :ref:`v4l2-mbus-format` 。由应用程序填写。)
    * - __u32
      - ``min_width``
      - Minimum frame width, in pixels. Filled in by the driver.
        (最小帧宽度，以像素为单位。由驱动程序填写。)
    * - __u32
      - ``max_width``
      - Maximum frame width, in pixels. Filled in by the driver.
        (最大帧宽度，以像素为单位。由驱动程序填写。)
    * - __u32
      - ``min_height``
      - Minimum frame height, in pixels. Filled in by the driver.
        (最小帧高度，以像素为单位。由驱动程序填写。)
    * - __u32
      - ``max_height``
      - Maximum frame height, in pixels. Filled in by the driver.
        (最大帧高度，以像素为单位。由驱动程序填写。)
    * - __u32
      - ``which``
      - Frame sizes to be enumerated, from enum
	:ref:`v4l2_subdev_format_whence <v4l2-subdev-format-whence>`.
        (要枚举的帧尺寸，来自 enum :ref:`v4l2_subdev_format_whence <v4l2-subdev-format-whence>` 。)
    * - __u32
      - ``stream``
      - Stream identifier.
        (流标识符。)
    * - __u32
      - ``reserved``\ [7]
      - Reserved for future extensions. Applications and drivers must set
	the array to zero.
        (保留用于未来的扩展。应用程序和驱动程序必须把该数组置零。)

Return Value(返回值)
====================

On success 0 is returned, on error -1 and the ``errno`` variable is set
appropriately. The generic error codes are described at the
:ref:`Generic Error Codes <gen-errors>` chapter.
(成功时返回 0，出错时返回 -1 并适当设置 ``errno`` 变量。通用错误码在 :ref:`Generic Error Codes <gen-errors>` 一章中描述。)

EINVAL
    The struct :c:type:`v4l2_subdev_frame_size_enum` ``pad`` references a
    non-existing pad, the ``which`` field has an unsupported value, the ``code``
    is invalid for the given pad, or the ``index`` field is out of bounds.
    (struct :c:type:`v4l2_subdev_frame_size_enum` 的 ``pad`` 引用了不存在的 pad、``which`` 字段的值不受支持、``code`` 对给定 pad 无效，或 ``index`` 字段越界。)
