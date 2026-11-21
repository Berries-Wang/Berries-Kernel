.. SPDX-License-Identifier: GFDL-1.1-no-invariants-or-later
.. c:namespace:: V4L

.. _VIDIOC_SUBDEV_G_SELECTION:

**********************************************************
ioctl VIDIOC_SUBDEV_G_SELECTION, VIDIOC_SUBDEV_S_SELECTION
**********************************************************

Name(名称)
==========

VIDIOC_SUBDEV_G_SELECTION - VIDIOC_SUBDEV_S_SELECTION - Get or set selection rectangles on a subdev pad
(VIDIOC_SUBDEV_G_SELECTION - VIDIOC_SUBDEV_S_SELECTION - 获取或设置子设备 pad 上的选择矩形。)

Synopsis(概要)
==============

.. c:macro:: VIDIOC_SUBDEV_G_SELECTION

``int ioctl(int fd, VIDIOC_SUBDEV_G_SELECTION, struct v4l2_subdev_selection *argp)``

.. c:macro:: VIDIOC_SUBDEV_S_SELECTION

``int ioctl(int fd, VIDIOC_SUBDEV_S_SELECTION, struct v4l2_subdev_selection *argp)``

Arguments(参数)
===============

``fd``
    File descriptor returned by :c:func:`open()`.
    (由 :c:func:`open()` 返回的文件描述符。)

``argp``
    Pointer to struct :c:type:`v4l2_subdev_selection`.
    (指向 struct :c:type:`v4l2_subdev_selection` 的指针。)

Description(描述)
=================

The selections are used to configure various image processing
functionality performed by the subdevs which affect the image size. This
currently includes cropping, scaling and composition.
(选择用于配置由 subdev 执行的、影响图像尺寸的各种图像处理功能。目前包括裁剪、缩放和合成。)

The selection API replaces
:ref:`the old subdev crop API <VIDIOC_SUBDEV_G_CROP>`. All the
function of the crop API, and more, are supported by the selections API.
(选择 API 取代了 :ref:`the old subdev crop API <VIDIOC_SUBDEV_G_CROP>` （旧的 subdev 裁剪 API）。裁剪 API 的所有功能以及更多功能都由选择 API 支持。)

See :ref:`subdev` for more information on how each selection target
affects the image processing pipeline inside the subdevice.
(有关每个选择目标如何影响子设备内部图像处理管线的更多信息，参见 :ref:`subdev` 。)

If the subdev device node has been registered in read-only mode, calls to
``VIDIOC_SUBDEV_S_SELECTION`` are only valid if the ``which`` field is set to
``V4L2_SUBDEV_FORMAT_TRY``, otherwise an error is returned and the errno
variable is set to ``-EPERM``.
(如果 subdev 设备节点以只读模式注册，则对 ``VIDIOC_SUBDEV_S_SELECTION`` 的调用仅在 ``which`` 字段设置为 ``V4L2_SUBDEV_FORMAT_TRY`` 时有效，否则返回错误并将 errno 变量设置为 ``-EPERM`` 。)

Types of selection targets(选择目标的类型)
------------------------------------------

There are two types of selection targets: actual and bounds. The actual
targets are the targets which configure the hardware. The BOUNDS target
will return a rectangle that contain all possible actual rectangles.
(选择目标有两种类型：实际（actual）和边界（bounds）。实际目标是配置硬件的目标。BOUNDS 目标将返回一个包含所有可能的实际矩形的矩形。)

Discovering supported features(发现支持的特性)
----------------------------------------------

To discover which targets are supported, the user can perform
``VIDIOC_SUBDEV_G_SELECTION`` on them. Any unsupported target will
return ``EINVAL``.
(要发现支持哪些目标，用户可以对它们执行 ``VIDIOC_SUBDEV_G_SELECTION`` 。任何不受支持的目标都会返回 ``EINVAL`` 。)

Selection targets and flags are documented in
:ref:`v4l2-selections-common`.
(选择目标和标志在 :ref:`v4l2-selections-common` 中有文档说明。)

.. c:type:: v4l2_subdev_selection

.. tabularcolumns:: |p{4.4cm}|p{4.4cm}|p{8.5cm}|

.. flat-table:: struct v4l2_subdev_selection
    :header-rows:  0
    :stub-columns: 0
    :widths:       1 1 2

    * - __u32
      - ``which``
      - Active or try selection, from enum
	:ref:`v4l2_subdev_format_whence <v4l2-subdev-format-whence>`.
        (活动或 try 选择，来自 enum :ref:`v4l2_subdev_format_whence <v4l2-subdev-format-whence>` 。)
    * - __u32
      - ``pad``
      - Pad number as reported by the media framework.
        (媒体框架所报告的 pad 编号。)
    * - __u32
      - ``target``
      - Target selection rectangle. See :ref:`v4l2-selections-common`.
        (目标选择矩形。参见 :ref:`v4l2-selections-common` 。)
    * - __u32
      - ``flags``
      - Flags. See :ref:`v4l2-selection-flags`.
        (标志。参见 :ref:`v4l2-selection-flags` 。)
    * - struct :c:type:`v4l2_rect`
      - ``r``
      - Selection rectangle, in pixels.
        (选择矩形，以像素为单位。)
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

EBUSY
    The selection rectangle can't be changed because the pad is
    currently busy. This can be caused, for instance, by an active video
    stream on the pad. The ioctl must not be retried without performing
    another action to fix the problem first. Only returned by
    ``VIDIOC_SUBDEV_S_SELECTION``
    (无法更改选择矩形，因为 pad 当前正忙。例如，这可能是由 pad 上活动的视频流引起的。必须先执行其他操作解决该问题，然后才能重试此 ioctl。仅由 ``VIDIOC_SUBDEV_S_SELECTION`` 返回。)

EINVAL
    The struct :c:type:`v4l2_subdev_selection` ``pad`` references a
    non-existing pad, the ``which`` field has an unsupported value, or the
    selection target is not supported on the given subdev pad.
    (struct :c:type:`v4l2_subdev_selection` 的 ``pad`` 引用了不存在的 pad、``which`` 字段的值不受支持，或给定的 subdev pad 不支持该选择目标。)

EPERM
    The ``VIDIOC_SUBDEV_S_SELECTION`` ioctl has been called on a read-only
    subdevice and the ``which`` field is set to ``V4L2_SUBDEV_FORMAT_ACTIVE``.
    (在只读子设备上调用了 ``VIDIOC_SUBDEV_S_SELECTION`` ioctl，且 ``which`` 字段被设置为 ``V4L2_SUBDEV_FORMAT_ACTIVE`` 。)
