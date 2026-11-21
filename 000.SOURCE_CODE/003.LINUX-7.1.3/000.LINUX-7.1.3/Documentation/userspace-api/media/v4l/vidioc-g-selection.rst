.. SPDX-License-Identifier: GFDL-1.1-no-invariants-or-later
.. c:namespace:: V4L

.. _VIDIOC_G_SELECTION:

****************************************************************
ioctl VIDIOC_G_SELECTION, VIDIOC_S_SELECTION(获取或设置选择矩形)
****************************************************************

Name(名称)
==========

VIDIOC_G_SELECTION - VIDIOC_S_SELECTION - Get or set one of the selection rectangles
(VIDIOC_G_SELECTION - VIDIOC_S_SELECTION - 获取或设置某个选择矩形)

Synopsis(概要)
==============

.. c:macro:: VIDIOC_G_SELECTION

``int ioctl(int fd, VIDIOC_G_SELECTION, struct v4l2_selection *argp)``

.. c:macro:: VIDIOC_S_SELECTION

``int ioctl(int fd, VIDIOC_S_SELECTION, struct v4l2_selection *argp)``

Arguments(参数)
===============

``fd``
    File descriptor returned by :c:func:`open()`.
    (由 :c:func:`open()` 返回的文件描述符。)

``argp``
    Pointer to struct :c:type:`v4l2_selection`.
    (指向结构体 :c:type:`v4l2_selection` 的指针。)

Description(描述)
=================

The ioctls are used to query and configure selection rectangles.
(这些 ioctl 用于查询和配置选择矩形。)

To query the cropping (composing) rectangle set struct
:c:type:`v4l2_selection` ``type`` field to the
respective buffer type. The next step is setting the
value of struct :c:type:`v4l2_selection` ``target``
field to ``V4L2_SEL_TGT_CROP`` (``V4L2_SEL_TGT_COMPOSE``). Please refer
to table :ref:`v4l2-selections-common` or :ref:`selection-api` for
additional targets. The ``flags`` and ``reserved`` fields of struct
:c:type:`v4l2_selection` are ignored and they must be
filled with zeros. The driver fills the rest of the structure or returns
EINVAL error code if incorrect buffer type or target was used. If
cropping (composing) is not supported then the active rectangle is not
mutable and it is always equal to the bounds rectangle. Finally, the
struct :c:type:`v4l2_rect` ``r`` rectangle is filled with
the current cropping (composing) coordinates. The coordinates are
expressed in driver-dependent units. The only exception are rectangles
for images in raw formats, whose coordinates are always expressed in
pixels.
(要查询裁剪 (cropping)/合成 (composing) 矩形，需把结构体 :c:type:`v4l2_selection` 的 ``type`` 字段设为相应的缓冲区类型。下一步是把结构体 :c:type:`v4l2_selection` 的 ``target`` 字段值设为 ``V4L2_SEL_TGT_CROP``(``V4L2_SEL_TGT_COMPOSE``)。其他 target 请参阅表 :ref:`v4l2-selections-common` 或 :ref:`selection-api`。结构体 :c:type:`v4l2_selection` 的 ``flags`` 与 ``reserved`` 字段会被忽略，必须以零填充。驱动会填充结构的其余字段；如果使用了错误的缓冲区类型或 target，则返回 EINVAL 错误码。若不支持裁剪(合成)，则活动矩形不可更改，且始终等于边界矩形。最后，结构体 :c:type:`v4l2_rect` 的 ``r`` 矩形会被填入当前裁剪(合成)坐标。坐标以驱动相关的单位表示。唯一的例外是原始 (raw) 格式图像的矩形，其坐标总是以像素表示。)

To change the cropping (composing) rectangle set the struct
:c:type:`v4l2_selection` ``type`` field to the
respective buffer type. The next step is setting the
value of struct :c:type:`v4l2_selection` ``target`` to
``V4L2_SEL_TGT_CROP`` (``V4L2_SEL_TGT_COMPOSE``). Please refer to table
:ref:`v4l2-selections-common` or :ref:`selection-api` for additional
targets. The struct :c:type:`v4l2_rect` ``r`` rectangle need
to be set to the desired active area. Field struct
:c:type:`v4l2_selection` ``reserved`` is ignored and
must be filled with zeros. The driver may adjust coordinates of the
requested rectangle. An application may introduce constraints to control
rounding behaviour. The struct :c:type:`v4l2_selection`
``flags`` field must be set to one of the following:
(要更改裁剪(合成)矩形，需把结构体 :c:type:`v4l2_selection` 的 ``type`` 字段设为相应的缓冲区类型。下一步是把结构体 :c:type:`v4l2_selection` 的 ``target`` 值设为 ``V4L2_SEL_TGT_CROP``(``V4L2_SEL_TGT_COMPOSE``)。其他 target 请参阅表 :ref:`v4l2-selections-common` 或 :ref:`selection-api`。结构体 :c:type:`v4l2_rect` 的 ``r`` 矩形需设为期望的活动区域。结构体 :c:type:`v4l2_selection` 的 ``reserved`` 字段会被忽略，必须以零填充。驱动可以调整所请求矩形的坐标。应用可以引入约束来控制取整行为。结构体 :c:type:`v4l2_selection` 的 ``flags`` 字段必须设为以下之一：)

-  ``0`` - The driver can adjust the rectangle size freely and shall
   choose a crop/compose rectangle as close as possible to the requested
   one.
   (``0`` - 驱动可以自由调整矩形大小，并应选择尽可能接近所请求矩形的裁剪/合成矩形。)

-  ``V4L2_SEL_FLAG_GE`` - The driver is not allowed to shrink the
   rectangle. The original rectangle must lay inside the adjusted one.
   (``V4L2_SEL_FLAG_GE`` - 不允许驱动缩小矩形。原矩形必须位于调整后矩形之内。)

-  ``V4L2_SEL_FLAG_LE`` - The driver is not allowed to enlarge the
   rectangle. The adjusted rectangle must lay inside the original one.
   (``V4L2_SEL_FLAG_LE`` - 不允许驱动放大矩形。调整后的矩形必须位于原矩形之内。)

-  ``V4L2_SEL_FLAG_GE | V4L2_SEL_FLAG_LE`` - The driver must choose the
   size exactly the same as in the requested rectangle.
   (``V4L2_SEL_FLAG_GE | V4L2_SEL_FLAG_LE`` - 驱动必须选择与所请求矩形完全相同的尺寸。)

Please refer to :ref:`sel-const-adjust`.
(请参阅 :ref:`sel-const-adjust`。)

The driver may have to adjusts the requested dimensions against hardware
limits and other parts as the pipeline, i.e. the bounds given by the
capture/output window or TV display. The closest possible values of
horizontal and vertical offset and sizes are chosen according to
following priority:
(驱动可能需要对照硬件限制以及管线中的其他部分(即采集/输出窗口或电视显示所给出的边界)来调整所请求的尺寸。水平和垂直偏移与尺寸的最接近可行值按以下优先级选取：)

1. Satisfy constraints from struct
   :c:type:`v4l2_selection` ``flags``.
   (满足结构体 :c:type:`v4l2_selection` ``flags`` 带来的约束。)

2. Adjust width, height, left, and top to hardware limits and
   alignments.
   (把 width、height、left 和 top 调整到硬件限制与对齐要求。)

3. Keep center of adjusted rectangle as close as possible to the
   original one.
   (使调整后矩形的中心尽可能接近原矩形中心。)

4. Keep width and height as close as possible to original ones.
   (使 width 和 height 尽可能接近原值。)

5. Keep horizontal and vertical offset as close as possible to original
   ones.
   (使水平和垂直偏移尽可能接近原值。)

On success the struct :c:type:`v4l2_rect` ``r`` field
contains the adjusted rectangle. When the parameters are unsuitable the
application may modify the cropping (composing) or image parameters and
repeat the cycle until satisfactory parameters have been negotiated. If
constraints flags have to be violated at then ``ERANGE`` is returned. The
error indicates that *there exist no rectangle* that satisfies the
constraints.
(成功时，结构体 :c:type:`v4l2_rect` 的 ``r`` 字段包含调整后的矩形。当参数不合适时，应用可以修改裁剪(合成)或图像参数并重复这一循环，直到协商出令人满意的参数。如果不得不违反约束标志，则返回 ``ERANGE``。该错误表示*不存在*满足这些约束的矩形。)

Selection targets and flags are documented in
:ref:`v4l2-selections-common`.
(选择 target 与标志在 :ref:`v4l2-selections-common` 中说明。)

.. _sel-const-adjust:

.. kernel-figure::  constraints.svg
    :alt:    constraints.svg
    :align:  center

    Size adjustments with constraint flags.

    Behaviour of rectangle adjustment for different constraint flags.



.. c:type:: v4l2_selection

.. tabularcolumns:: |p{4.4cm}|p{4.4cm}|p{8.5cm}|

.. flat-table:: struct v4l2_selection
    :header-rows:  0
    :stub-columns: 0
    :widths:       1 1 2

    * - __u32
      - ``type``
      - Type of the buffer (from enum
	:c:type:`v4l2_buf_type`).
        (缓冲区类型(来自枚举 :c:type:`v4l2_buf_type`)。)
    * - __u32
      - ``target``
      - Used to select between
	:ref:`cropping and composing rectangles <v4l2-selections-common>`.
        (用于在 :ref:`cropping and composing rectangles <v4l2-selections-common>` 之间选择。)
    * - __u32
      - ``flags``
      - Flags controlling the selection rectangle adjustments, refer to
	:ref:`selection flags <v4l2-selection-flags>`.
        (控制选择矩形调整的标志，参见 :ref:`selection flags <v4l2-selection-flags>`。)
    * - struct :c:type:`v4l2_rect`
      - ``r``
      - The selection rectangle.
        (选择矩形。)
    * - __u32
      - ``reserved[9]``
      - Reserved fields for future use. Drivers and applications must zero
	this array.
        (保留字段以供将来使用。驱动与应用必须将该数组清零。)

.. note::
   Unfortunately in the case of multiplanar buffer types
   (``V4L2_BUF_TYPE_VIDEO_CAPTURE_MPLANE`` and ``V4L2_BUF_TYPE_VIDEO_OUTPUT_MPLANE``)
   this API was messed up with regards to how the :c:type:`v4l2_selection` ``type`` field
   should be filled in. Some drivers only accepted the ``_MPLANE`` buffer type while
   other drivers only accepted a non-multiplanar buffer type (i.e. without the
   ``_MPLANE`` at the end).

   Starting with kernel 4.13 both variations are allowed.
   (遗憾的是，对于多平面缓冲区类型(``V4L2_BUF_TYPE_VIDEO_CAPTURE_MPLANE`` 与 ``V4L2_BUF_TYPE_VIDEO_OUTPUT_MPLANE``)，这个 API 在 :c:type:`v4l2_selection` 的 ``type`` 字段应如何填写方面有些混乱。一些驱动只接受 ``_MPLANE`` 缓冲区类型，而另一些驱动只接受非多平面缓冲区类型(即末尾不带 ``_MPLANE``)。

   从内核 4.13 开始，两种写法都被允许。)

Return Value(返回值)
====================

On success 0 is returned, on error -1 and the ``errno`` variable is set
appropriately. The generic error codes are described at the
:ref:`Generic Error Codes <gen-errors>` chapter.
(成功时返回 0，出错时返回 -1 并将 ``errno`` 变量设置为相应值。通用错误码在 :ref:`Generic Error Codes <gen-errors>` 一章中描述。)

EINVAL
    Given buffer type ``type`` or the selection target ``target`` is not
    supported, or the ``flags`` argument is not valid.
    (给定的缓冲区类型 ``type`` 或选择 target ``target`` 不受支持，或 ``flags`` 参数无效。)

ERANGE
    It is not possible to adjust struct :c:type:`v4l2_rect`
    ``r`` rectangle to satisfy all constraints given in the ``flags``
    argument.
    (无法调整结构体 :c:type:`v4l2_rect` 的 ``r`` 矩形以满足 ``flags`` 参数给出的全部约束。)

ENODATA
    Selection is not supported for this input or output.
    (此输入或输出不支持选择。)

EBUSY
    It is not possible to apply change of the selection rectangle at the
    moment. Usually because streaming is in progress.
    (目前无法应用选择矩形的更改。通常是因为流处理正在进行中。)
