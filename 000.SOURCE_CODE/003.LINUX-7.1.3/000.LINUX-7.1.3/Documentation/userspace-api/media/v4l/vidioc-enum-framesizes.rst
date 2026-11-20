.. SPDX-License-Identifier: GFDL-1.1-no-invariants-or-later
.. c:namespace:: V4L

.. _VIDIOC_ENUM_FRAMESIZES:

****************************************
ioctl VIDIOC_ENUM_FRAMESIZES(枚举帧尺寸)
****************************************

Name(名称)
==========

VIDIOC_ENUM_FRAMESIZES - Enumerate frame sizes
(VIDIOC_ENUM_FRAMESIZES - 枚举帧尺寸)

Synopsis(概要)
==============

.. c:macro:: VIDIOC_ENUM_FRAMESIZES

``int ioctl(int fd, VIDIOC_ENUM_FRAMESIZES, struct v4l2_frmsizeenum *argp)``

Arguments(参数)
===============

``fd``
    File descriptor returned by :c:func:`open()`.
    (由 :c:func:`open()` 返回的文件描述符。)

``argp``
    Pointer to struct :c:type:`v4l2_frmsizeenum`
    that contains an index and pixel format and receives a frame width
    and height.
    (指向结构体 :c:type:`v4l2_frmsizeenum` 的指针，其中包含索引与像素格式，并接收帧的宽和高。)

Description(描述)
=================

This ioctl allows applications to enumerate all frame sizes (i. e. width
and height in pixels) that the device supports for the given pixel
format.
(该 ioctl 允许应用枚举设备对给定像素格式支持的所有帧尺寸(即以像素为单位的宽和高)。)

The supported pixel formats can be obtained by using the
:ref:`VIDIOC_ENUM_FMT` function.
(受支持的像素格式可通过 :ref:`VIDIOC_ENUM_FMT` 函数获得。)

The return value and the content of the ``v4l2_frmsizeenum.type`` field
depend on the type of frame sizes the device supports. Here are the
semantics of the function for the different cases:
(返回值与 ``v4l2_frmsizeenum.type`` 字段的内容取决于设备支持的帧尺寸类型。下面是该函数在不同情形下的语义：)

-  **Discrete:** The function returns success if the given index value
   (zero-based) is valid. The application should increase the index by
   one for each call until ``EINVAL`` is returned. The
   ``v4l2_frmsizeenum.type`` field is set to
   ``V4L2_FRMSIZE_TYPE_DISCRETE`` by the driver. Of the union only the
   ``discrete`` member is valid.
   (**离散(Discrete)：** 若给定的索引值(从零开始)有效，函数返回成功。应用应在每次调用时把索引加一，直到返回 ``EINVAL``。``v4l2_frmsizeenum.type`` 字段由驱动设置为 ``V4L2_FRMSIZE_TYPE_DISCRETE``。联合体中只有 ``discrete`` 成员有效。)

-  **Step-wise:** The function returns success if the given index value
   is zero and ``EINVAL`` for any other index value. The
   ``v4l2_frmsizeenum.type`` field is set to
   ``V4L2_FRMSIZE_TYPE_STEPWISE`` by the driver. Of the union only the
   ``stepwise`` member is valid.
   (**步进(Step-wise)：** 若给定索引值为零，函数返回成功；对其他任何索引值返回 ``EINVAL``。``v4l2_frmsizeenum.type`` 字段由驱动设置为 ``V4L2_FRMSIZE_TYPE_STEPWISE``。联合体中只有 ``stepwise`` 成员有效。)

-  **Continuous:** This is a special case of the step-wise type above.
   The function returns success if the given index value is zero and
   ``EINVAL`` for any other index value. The ``v4l2_frmsizeenum.type``
   field is set to ``V4L2_FRMSIZE_TYPE_CONTINUOUS`` by the driver. Of
   the union only the ``stepwise`` member is valid and the
   ``step_width`` and ``step_height`` values are set to 1.
   (**连续(Continuous)：** 这是上述步进类型的特例。若给定索引值为零，函数返回成功；对其他任何索引值返回 ``EINVAL``。``v4l2_frmsizeenum.type`` 字段由驱动设置为 ``V4L2_FRMSIZE_TYPE_CONTINUOUS``。联合体中只有 ``stepwise`` 成员有效，且 ``step_width`` 与 ``step_height`` 值被设为 1。)

When the application calls the function with index zero, it must check
the ``type`` field to determine the type of frame size enumeration the
device supports. Only for the ``V4L2_FRMSIZE_TYPE_DISCRETE`` type does
it make sense to increase the index value to receive more frame sizes.
(当应用以索引零调用该函数时，必须检查 ``type`` 字段以确定设备支持的帧尺寸枚举类型。只有对 ``V4L2_FRMSIZE_TYPE_DISCRETE`` 类型，增加索引值以获取更多帧尺寸才有意义。)

.. note::

   The order in which the frame sizes are returned has no special
   meaning. In particular does it not say anything about potential default
   format sizes.
   (返回帧尺寸的顺序没有特殊含义。它尤其不代表任何潜在的默认格式尺寸。)

Applications can assume that the enumeration data does not change
without any interaction from the application itself. This means that the
enumeration data is consistent if the application does not perform any
other ioctl calls while it runs the frame size enumeration.
(应用可以假定：除非应用自身进行交互，枚举数据不会改变。也就是说，如果应用在执行帧尺寸枚举期间不进行任何其他 ioctl 调用，枚举数据就是一致的。)

Structs(结构体)
===============

In the structs below, *IN* denotes a value that has to be filled in by
the application, *OUT* denotes values that the driver fills in. The
application should zero out all members except for the *IN* fields.
(在下面的结构体中，*IN* 表示必须由应用填入的值，*OUT* 表示由驱动填入的值。应用应把除 *IN* 字段外的所有成员清零。)

.. c:type:: v4l2_frmsize_discrete

.. flat-table:: struct v4l2_frmsize_discrete
    :header-rows:  0
    :stub-columns: 0
    :widths:       1 1 2

    * - __u32
      - ``width``
      - Width of the frame [pixel].
        (帧的宽度 [像素]。)
    * - __u32
      - ``height``
      - Height of the frame [pixel].
        (帧的高度 [像素]。)


.. c:type:: v4l2_frmsize_stepwise

.. flat-table:: struct v4l2_frmsize_stepwise
    :header-rows:  0
    :stub-columns: 0
    :widths:       1 1 2

    * - __u32
      - ``min_width``
      - Minimum frame width [pixel].
        (最小帧宽度 [像素]。)
    * - __u32
      - ``max_width``
      - Maximum frame width [pixel].
        (最大帧宽度 [像素]。)
    * - __u32
      - ``step_width``
      - Frame width step size [pixel].
        (帧宽度步进值 [像素]。)
    * - __u32
      - ``min_height``
      - Minimum frame height [pixel].
        (最小帧高度 [像素]。)
    * - __u32
      - ``max_height``
      - Maximum frame height [pixel].
        (最大帧高度 [像素]。)
    * - __u32
      - ``step_height``
      - Frame height step size [pixel].
        (帧高度步进值 [像素]。)


.. c:type:: v4l2_frmsizeenum

.. tabularcolumns:: |p{6.4cm}|p{2.8cm}|p{8.1cm}|

.. flat-table:: struct v4l2_frmsizeenum
    :header-rows:  0
    :stub-columns: 0

    * - __u32
      - ``index``
      - IN: Index of the given frame size in the enumeration.
        (IN：给定帧尺寸在枚举中的索引。)
    * - __u32
      - ``pixel_format``
      - IN: Pixel format for which the frame sizes are enumerated.
        (IN：为其枚举帧尺寸的像素格式。)
    * - __u32
      - ``type``
      - OUT: Frame size type the device supports.
        (OUT：设备支持的帧尺寸类型。)
    * - union {
      - (anonymous)
      - OUT: Frame size with the given index.
        (OUT：给定索引处的帧尺寸。)
    * - struct :c:type:`v4l2_frmsize_discrete`
      - ``discrete``
      -
    * - struct :c:type:`v4l2_frmsize_stepwise`
      - ``stepwise``
      -
    * - }
      -
      -
    * - __u32
      - ``reserved[2]``
      - Reserved space for future use. Must be zeroed by drivers and
	applications.
        (保留以供将来使用。驱动与应用必须将其清零。)


Enums
=====

.. c:type:: v4l2_frmsizetypes

.. tabularcolumns:: |p{6.6cm}|p{2.2cm}|p{8.5cm}|

.. flat-table:: enum v4l2_frmsizetypes
    :header-rows:  0
    :stub-columns: 0
    :widths:       3 1 4

    * - ``V4L2_FRMSIZE_TYPE_DISCRETE``
      - 1
      - Discrete frame size.
        (离散帧尺寸。)
    * - ``V4L2_FRMSIZE_TYPE_CONTINUOUS``
      - 2
      - Continuous frame size.
        (连续帧尺寸。)
    * - ``V4L2_FRMSIZE_TYPE_STEPWISE``
      - 3
      - Step-wise defined frame size.
        (按步进定义的帧尺寸。)

Return Value(返回值)
====================

On success 0 is returned, on error -1 and the ``errno`` variable is set
appropriately. The generic error codes are described at the
:ref:`Generic Error Codes <gen-errors>` chapter.
(成功时返回 0，出错时返回 -1 并将 ``errno`` 变量设置为相应值。通用错误码在 :ref:`Generic Error Codes <gen-errors>` 一章中描述。)
