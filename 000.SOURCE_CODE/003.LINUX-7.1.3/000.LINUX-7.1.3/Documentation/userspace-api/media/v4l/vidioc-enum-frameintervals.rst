.. SPDX-License-Identifier: GFDL-1.1-no-invariants-or-later
.. c:namespace:: V4L

.. _VIDIOC_ENUM_FRAMEINTERVALS:

****************************************
ioctl VIDIOC_ENUM_FRAMEINTERVALS(枚举帧间隔)
********************************************

Name(名称)
==========

VIDIOC_ENUM_FRAMEINTERVALS - Enumerate frame intervals
(VIDIOC_ENUM_FRAMEINTERVALS - 枚举帧间隔)

Synopsis(概要)
==============

.. c:macro:: VIDIOC_ENUM_FRAMEINTERVALS

``int ioctl(int fd, VIDIOC_ENUM_FRAMEINTERVALS, struct v4l2_frmivalenum *argp)``

Arguments(参数)
===============

``fd``
    File descriptor returned by :c:func:`open()`.
    (由 :c:func:`open()` 返回的文件描述符。)

``argp``
    Pointer to struct :c:type:`v4l2_frmivalenum`
    that contains a pixel format and size and receives a frame interval.
    (指向结构体 :c:type:`v4l2_frmivalenum` 的指针，其中包含像素格式与尺寸，并接收一个帧间隔。)

Description(描述)
=================

This ioctl allows applications to enumerate all frame intervals that the
device supports for the given pixel format and frame size.
(该 ioctl 允许应用枚举设备对给定像素格式与帧尺寸支持的所有帧间隔。)

The supported pixel formats and frame sizes can be obtained by using the
:ref:`VIDIOC_ENUM_FMT` and
:ref:`VIDIOC_ENUM_FRAMESIZES` functions.
(受支持的像素格式与帧尺寸可通过 :ref:`VIDIOC_ENUM_FMT` 与 :ref:`VIDIOC_ENUM_FRAMESIZES` 函数获得。)

The return value and the content of the ``v4l2_frmivalenum.type`` field
depend on the type of frame intervals the device supports. Here are the
semantics of the function for the different cases:
(返回值与 ``v4l2_frmivalenum.type`` 字段的内容取决于设备支持的帧间隔类型。下面是该函数在不同情形下的语义：)

-  **Discrete:** The function returns success if the given index value
   (zero-based) is valid. The application should increase the index by
   one for each call until ``EINVAL`` is returned. The
   `v4l2_frmivalenum.type` field is set to
   `V4L2_FRMIVAL_TYPE_DISCRETE` by the driver. Of the union only
   the `discrete` member is valid.
   (**离散(Discrete)：** 若给定的索引值(从零开始)有效，函数返回成功。应用应在每次调用时把索引加一，直到返回 ``EINVAL``。`v4l2_frmivalenum.type` 字段由驱动设置为 `V4L2_FRMIVAL_TYPE_DISCRETE`。联合体中只有 `discrete` 成员有效。)

-  **Step-wise:** The function returns success if the given index value
   is zero and ``EINVAL`` for any other index value. The
   ``v4l2_frmivalenum.type`` field is set to
   ``V4L2_FRMIVAL_TYPE_STEPWISE`` by the driver. Of the union only the
   ``stepwise`` member is valid.
   (**步进(Step-wise)：** 若给定索引值为零，函数返回成功；对其他任何索引值返回 ``EINVAL``。``v4l2_frmivalenum.type`` 字段由驱动设置为 ``V4L2_FRMIVAL_TYPE_STEPWISE``。联合体中只有 ``stepwise`` 成员有效。)

-  **Continuous:** This is a special case of the step-wise type above.
   The function returns success if the given index value is zero and
   ``EINVAL`` for any other index value. The ``v4l2_frmivalenum.type``
   field is set to ``V4L2_FRMIVAL_TYPE_CONTINUOUS`` by the driver. Of
   the union only the ``stepwise`` member is valid and the ``step``
   value is set to 1.
   (**连续(Continuous)：** 这是上述步进类型的特例。若给定索引值为零，函数返回成功；对其他任何索引值返回 ``EINVAL``。``v4l2_frmivalenum.type`` 字段由驱动设置为 ``V4L2_FRMIVAL_TYPE_CONTINUOUS``。联合体中只有 ``stepwise`` 成员有效，且 ``step`` 值被设为 1。)

When the application calls the function with index zero, it must check
the ``type`` field to determine the type of frame interval enumeration
the device supports. Only for the ``V4L2_FRMIVAL_TYPE_DISCRETE`` type
does it make sense to increase the index value to receive more frame
intervals.
(当应用以索引零调用该函数时，必须检查 ``type`` 字段以确定设备支持的帧间隔枚举类型。只有对 ``V4L2_FRMIVAL_TYPE_DISCRETE`` 类型，增加索引值以获取更多帧间隔才有意义。)

.. note::

   The order in which the frame intervals are returned has no
   special meaning. In particular does it not say anything about potential
   default frame intervals.
   (返回帧间隔的顺序没有特殊含义。它尤其不代表任何潜在的默认帧间隔。)

Applications can assume that the enumeration data does not change
without any interaction from the application itself. This means that the
enumeration data is consistent if the application does not perform any
other ioctl calls while it runs the frame interval enumeration.
(应用可以假定：除非应用自身进行交互，枚举数据不会改变。也就是说，如果应用在执行帧间隔枚举期间不进行任何其他 ioctl 调用，枚举数据就是一致的。)

.. note::

   **Frame intervals and frame rates:** The V4L2 API uses frame
   intervals instead of frame rates. Given the frame interval the frame
   rate can be computed as follows:
   (**帧间隔与帧率：** V4L2 API 使用帧间隔而非帧率。给定帧间隔后，帧率可按如下方式计算：)

   ::

       frame_rate = 1 / frame_interval

Structs(结构体)
===============

In the structs below, *IN* denotes a value that has to be filled in by
the application, *OUT* denotes values that the driver fills in. The
application should zero out all members except for the *IN* fields.
(在下面的结构体中，*IN* 表示必须由应用填入的值，*OUT* 表示由驱动填入的值。应用应把除 *IN* 字段外的所有成员清零。)

.. c:type:: v4l2_frmival_stepwise

.. flat-table:: struct v4l2_frmival_stepwise
    :header-rows:  0
    :stub-columns: 0
    :widths:       1 1 2

    * - struct :c:type:`v4l2_fract`
      - ``min``
      - Minimum frame interval [s].
        (最小帧间隔 [秒]。)
    * - struct :c:type:`v4l2_fract`
      - ``max``
      - Maximum frame interval [s].
        (最大帧间隔 [秒]。)
    * - struct :c:type:`v4l2_fract`
      - ``step``
      - Frame interval step size [s].
        (帧间隔步进值 [秒]。)


.. c:type:: v4l2_frmivalenum

.. tabularcolumns:: |p{4.9cm}|p{3.3cm}|p{9.1cm}|

.. flat-table:: struct v4l2_frmivalenum
    :header-rows:  0
    :stub-columns: 0

    * - __u32
      - ``index``
      - IN: Index of the given frame interval in the enumeration.
        (IN：给定帧间隔在枚举中的索引。)
    * - __u32
      - ``pixel_format``
      - IN: Pixel format for which the frame intervals are enumerated.
        (IN：为其枚举帧间隔的像素格式。)
    * - __u32
      - ``width``
      - IN: Frame width for which the frame intervals are enumerated.
        (IN：为其枚举帧间隔的帧宽度。)
    * - __u32
      - ``height``
      - IN: Frame height for which the frame intervals are enumerated.
        (IN：为其枚举帧间隔的帧高度。)
    * - __u32
      - ``type``
      - OUT: Frame interval type the device supports.
        (OUT：设备支持的帧间隔类型。)
    * - union {
      - (anonymous)
      - OUT: Frame interval with the given index.
        (OUT：给定索引处的帧间隔。)
    * - struct :c:type:`v4l2_fract`
      - ``discrete``
      - Frame interval [s].
        (帧间隔 [秒]。)
    * - struct :c:type:`v4l2_frmival_stepwise`
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

.. c:type:: v4l2_frmivaltypes

.. tabularcolumns:: |p{6.6cm}|p{2.2cm}|p{8.5cm}|

.. flat-table:: enum v4l2_frmivaltypes
    :header-rows:  0
    :stub-columns: 0
    :widths:       3 1 4

    * - ``V4L2_FRMIVAL_TYPE_DISCRETE``
      - 1
      - Discrete frame interval.
        (离散帧间隔。)
    * - ``V4L2_FRMIVAL_TYPE_CONTINUOUS``
      - 2
      - Continuous frame interval.
        (连续帧间隔。)
    * - ``V4L2_FRMIVAL_TYPE_STEPWISE``
      - 3
      - Step-wise defined frame interval.
        (按步进定义的帧间隔。)

Return Value(返回值)
====================

On success 0 is returned, on error -1 and the ``errno`` variable is set
appropriately. The generic error codes are described at the
:ref:`Generic Error Codes <gen-errors>` chapter.
(成功时返回 0，出错时返回 -1 并将 ``errno`` 变量设置为相应值。通用错误码在 :ref:`Generic Error Codes <gen-errors>` 一章中描述。)
