.. SPDX-License-Identifier: GFDL-1.1-no-invariants-or-later
.. c:namespace:: V4L

.. _VIDIOC_SUBDEV_ENUM_MBUS_CODE:

**********************************************************************
ioctl VIDIOC_SUBDEV_ENUM_MBUS_CODE(ioctl VIDIOC_SUBDEV_ENUM_MBUS_CODE)
**********************************************************************

Name(名称)
==========

VIDIOC_SUBDEV_ENUM_MBUS_CODE - Enumerate media bus formats
(VIDIOC_SUBDEV_ENUM_MBUS_CODE - 枚举媒体总线格式)

Synopsis(概要)
==============

.. c:macro:: VIDIOC_SUBDEV_ENUM_MBUS_CODE

``int ioctl(int fd, VIDIOC_SUBDEV_ENUM_MBUS_CODE, struct v4l2_subdev_mbus_code_enum * argp)``

Arguments(参数)
===============

``fd``
    File descriptor returned by :c:func:`open()`.
    (由 :c:func:`open()` 返回的文件描述符。)

``argp``
    Pointer to struct :c:type:`v4l2_subdev_mbus_code_enum`.
    (指向 struct :c:type:`v4l2_subdev_mbus_code_enum` 的指针。)

Description(描述)
=================

This call is used by the application to access the enumeration
of media bus formats for the selected pad.
(此调用由应用程序用来访问所选 pad 的媒体总线格式枚举。)

The enumerations are defined by the driver, and indexed using the ``index`` field
of struct :c:type:`v4l2_subdev_mbus_code_enum`.
Each enumeration starts with the ``index`` of 0, and
the lowest invalid index marks the end of enumeration.
(枚举由驱动定义，并使用 struct :c:type:`v4l2_subdev_mbus_code_enum` 的
``index`` 字段进行索引。每个枚举从 ``index`` 为 0 开始，最小的无效索引
标志着枚举的结束。)

Therefore, to enumerate media bus formats available at a given sub-device pad,
initialize the ``pad``, and ``which`` fields to desired values,
and set ``index`` to 0.
Then call the :ref:`VIDIOC_SUBDEV_ENUM_MBUS_CODE` ioctl
with a pointer to this structure.
(因此，要枚举给定子设备 pad 上可用的媒体总线格式，将 ``pad`` 和
``which`` 字段初始化为期望的值，并把 ``index`` 设置为 0。然后以指向
此结构的指针调用 :ref:`VIDIOC_SUBDEV_ENUM_MBUS_CODE` ioctl。)

A successful call will return with the ``code`` field filled in
with a mbus code value.
Repeat with increasing ``index`` until ``EINVAL`` is received.
``EINVAL`` means that either ``pad`` is invalid,
or that there are no more codes available at this pad.
(成功的调用将返回并填充 ``code`` 字段为一个 mbus 码值。以递增的
``index`` 重复此过程，直到收到 ``EINVAL`` 。 ``EINVAL`` 表示 ``pad``
无效，或者此 pad 上已没有更多可用的码。)

The driver must not return the same value of ``code`` for different indices
at the same pad.
(在同一个 pad 上，驱动不得为不同的索引返回相同的 ``code`` 值。)

Available media bus formats may depend on the current 'try' formats at
other pads of the sub-device, as well as on the current active links.
See :ref:`VIDIOC_SUBDEV_G_FMT` for more
information about the try formats.
(可用的媒体总线格式可能取决于子设备其他 pad 上当前的 'try' 格式，
以及当前的激活链路。关于 try 格式的更多信息，参见
:ref:`VIDIOC_SUBDEV_G_FMT` 。)

.. c:type:: v4l2_subdev_mbus_code_enum

.. tabularcolumns:: |p{4.4cm}|p{4.4cm}|p{8.5cm}|

.. flat-table:: struct v4l2_subdev_mbus_code_enum
    :header-rows:  0
    :stub-columns: 0
    :widths:       1 1 2

    * - __u32
      - ``pad``
      - Pad number as reported by the media controller API. Filled in by the
        application.
	(pad 编号，由媒体控制器 API 报告。由应用程序填写。)
    * - __u32
      - ``index``
      - Index of the mbus code in the enumeration belonging to the given pad.
        Filled in by the application.
	(属于给定 pad 的枚举中 mbus 码的索引。由应用程序填写。)
    * - __u32
      - ``code``
      - The media bus format code, as defined in
	:ref:`v4l2-mbus-format`. Filled in by the driver.
	(媒体总线格式码，定义见 :ref:`v4l2-mbus-format` 。由驱动填写。)
    * - __u32
      - ``which``
      - Media bus format codes to be enumerated, from enum
	:ref:`v4l2_subdev_format_whence <v4l2-subdev-format-whence>`.
	(要枚举的媒体总线格式码，来自枚举
	:ref:`v4l2_subdev_format_whence <v4l2-subdev-format-whence>` 。)
    * - __u32
      - ``flags``
      - See :ref:`v4l2-subdev-mbus-code-flags`
	(参见 :ref:`v4l2-subdev-mbus-code-flags` )
    * - __u32
      - ``stream``
      - Stream identifier.(流标识符。)
    * - __u32
      - ``reserved``\ [6]
      - Reserved for future extensions. Applications and drivers must set
	the array to zero.
	(保留供将来扩展使用。应用程序和驱动必须将该数组置零。)



.. raw:: latex

   \footnotesize

.. tabularcolumns:: |p{8.8cm}|p{2.2cm}|p{6.3cm}|

.. _v4l2-subdev-mbus-code-flags:

.. flat-table:: Subdev Media Bus Code Enumerate Flags
    :header-rows:  0
    :stub-columns: 0
    :widths:       1 1 2

    * - V4L2_SUBDEV_MBUS_CODE_CSC_COLORSPACE
      - 0x00000001
      - The driver allows the application to try to change the default colorspace
	encoding. The application can ask to configure the colorspace of the
	subdevice when calling the :ref:`VIDIOC_SUBDEV_S_FMT <VIDIOC_SUBDEV_G_FMT>`
	ioctl with :ref:`V4L2_MBUS_FRAMEFMT_SET_CSC <mbus-framefmt-set-csc>` set.
	See :ref:`v4l2-mbus-format` on how to do this.
	(驱动允许应用程序尝试更改默认的 colorspace (色彩空间) 编码。
	当调用 :ref:`VIDIOC_SUBDEV_S_FMT <VIDIOC_SUBDEV_G_FMT>` ioctl 并
	设置 :ref:`V4L2_MBUS_FRAMEFMT_SET_CSC <mbus-framefmt-set-csc>` 时，
	应用程序可以请求配置子设备的色彩空间。具体做法参见
	:ref:`v4l2-mbus-format` 。)
    * - V4L2_SUBDEV_MBUS_CODE_CSC_XFER_FUNC
      - 0x00000002
      - The driver allows the application to try to change the default transform function.
	The application can ask to configure the transform function of
	the subdevice when calling the :ref:`VIDIOC_SUBDEV_S_FMT <VIDIOC_SUBDEV_G_FMT>`
	ioctl with :ref:`V4L2_MBUS_FRAMEFMT_SET_CSC <mbus-framefmt-set-csc>` set.
	See :ref:`v4l2-mbus-format` on how to do this.
	(驱动允许应用程序尝试更改默认的传递函数。当调用
	:ref:`VIDIOC_SUBDEV_S_FMT <VIDIOC_SUBDEV_G_FMT>` ioctl 并设置
	:ref:`V4L2_MBUS_FRAMEFMT_SET_CSC <mbus-framefmt-set-csc>` 时，
	应用程序可以请求配置子设备的传递函数。具体做法参见
	:ref:`v4l2-mbus-format` 。)
    * - V4L2_SUBDEV_MBUS_CODE_CSC_YCBCR_ENC
      - 0x00000004
      - The driver allows the application to try to change the default Y'CbCr
	encoding. The application can ask to configure the Y'CbCr encoding of the
	subdevice when calling the :ref:`VIDIOC_SUBDEV_S_FMT <VIDIOC_SUBDEV_G_FMT>`
	ioctl with :ref:`V4L2_MBUS_FRAMEFMT_SET_CSC <mbus-framefmt-set-csc>` set.
	See :ref:`v4l2-mbus-format` on how to do this.
	(驱动允许应用程序尝试更改默认的 Y'CbCr 编码。当调用
	:ref:`VIDIOC_SUBDEV_S_FMT <VIDIOC_SUBDEV_G_FMT>` ioctl 并设置
	:ref:`V4L2_MBUS_FRAMEFMT_SET_CSC <mbus-framefmt-set-csc>` 时，
	应用程序可以请求配置子设备的 Y'CbCr 编码。具体做法参见
	:ref:`v4l2-mbus-format` 。)
    * - V4L2_SUBDEV_MBUS_CODE_CSC_HSV_ENC
      - 0x00000004
      - The driver allows the application to try to change the default HSV
	encoding. The application can ask to configure the HSV encoding of the
	subdevice when calling the :ref:`VIDIOC_SUBDEV_S_FMT <VIDIOC_SUBDEV_G_FMT>`
	ioctl with :ref:`V4L2_MBUS_FRAMEFMT_SET_CSC <mbus-framefmt-set-csc>` set.
	See :ref:`v4l2-mbus-format` on how to do this.
	(驱动允许应用程序尝试更改默认的 HSV 编码。当调用
	:ref:`VIDIOC_SUBDEV_S_FMT <VIDIOC_SUBDEV_G_FMT>` ioctl 并设置
	:ref:`V4L2_MBUS_FRAMEFMT_SET_CSC <mbus-framefmt-set-csc>` 时，
	应用程序可以请求配置子设备的 HSV 编码。具体做法参见
	:ref:`v4l2-mbus-format` 。)
    * - V4L2_SUBDEV_MBUS_CODE_CSC_QUANTIZATION
      - 0x00000008
      - The driver allows the application to try to change the default
	quantization. The application can ask to configure the quantization of
	the subdevice when calling the :ref:`VIDIOC_SUBDEV_S_FMT <VIDIOC_SUBDEV_G_FMT>`
	ioctl with :ref:`V4L2_MBUS_FRAMEFMT_SET_CSC <mbus-framefmt-set-csc>` set.
	See :ref:`v4l2-mbus-format` on how to do this.
	(驱动允许应用程序尝试更改默认的量化方式。当调用
	:ref:`VIDIOC_SUBDEV_S_FMT <VIDIOC_SUBDEV_G_FMT>` ioctl 并设置
	:ref:`V4L2_MBUS_FRAMEFMT_SET_CSC <mbus-framefmt-set-csc>` 时，
	应用程序可以请求配置子设备的量化方式。具体做法参见
	:ref:`v4l2-mbus-format` 。)

.. raw:: latex

   \normalsize

Return Value(返回值)
====================

On success 0 is returned, on error -1 and the ``errno`` variable is set
appropriately. The generic error codes are described at the
:ref:`Generic Error Codes <gen-errors>` chapter.
(成功时返回 0，出错时返回 -1 并适当设置 ``errno`` 变量。通用错误码在 :ref:`Generic Error Codes <gen-errors>` 一章中描述。)

EINVAL
    The struct :c:type:`v4l2_subdev_mbus_code_enum` ``pad`` references a
    non-existing pad, the ``which`` field has an unsupported value, or the
    ``index`` field is out of bounds.
    ( struct :c:type:`v4l2_subdev_mbus_code_enum` 的 ``pad`` 引用了
    不存在的 pad， ``which`` 字段的值不受支持，或者 ``index`` 字段
    越界。)
