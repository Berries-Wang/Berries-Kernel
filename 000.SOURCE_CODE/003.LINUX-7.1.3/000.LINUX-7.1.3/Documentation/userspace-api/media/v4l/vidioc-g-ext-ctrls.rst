.. SPDX-License-Identifier: GFDL-1.1-no-invariants-or-later
.. c:namespace:: V4L

.. _VIDIOC_G_EXT_CTRLS:

******************************************************************************************
ioctl VIDIOC_G_EXT_CTRLS, VIDIOC_S_EXT_CTRLS, VIDIOC_TRY_EXT_CTRLS(获取或设置多个控制的值)
******************************************************************************************

Name(名称)
==========

VIDIOC_G_EXT_CTRLS - VIDIOC_S_EXT_CTRLS - VIDIOC_TRY_EXT_CTRLS - Get or set the value of several controls, try control values
(VIDIOC_G_EXT_CTRLS - VIDIOC_S_EXT_CTRLS - VIDIOC_TRY_EXT_CTRLS - 获取或设置若干控制的值，试设控制值。)

Synopsis(概要)
==============

.. c:macro:: VIDIOC_G_EXT_CTRLS

``int ioctl(int fd, VIDIOC_G_EXT_CTRLS, struct v4l2_ext_controls *argp)``

.. c:macro:: VIDIOC_S_EXT_CTRLS

``int ioctl(int fd, VIDIOC_S_EXT_CTRLS, struct v4l2_ext_controls *argp)``

.. c:macro:: VIDIOC_TRY_EXT_CTRLS

``int ioctl(int fd, VIDIOC_TRY_EXT_CTRLS, struct v4l2_ext_controls *argp)``

Arguments(参数)
===============

``fd``
    File descriptor returned by :c:func:`open()`.
    (由 :c:func:`open()` 返回的文件描述符。)

``argp``
    Pointer to struct :c:type:`v4l2_ext_controls`.
    (指向 struct :c:type:`v4l2_ext_controls` 的指针。)

Description(描述)
=================

These ioctls allow the caller to get or set multiple controls
atomically. Control IDs are grouped into control classes (see
:ref:`ctrl-class`) and all controls in the control array must belong
to the same control class.
(这些 ioctl 允许调用者原子地获取或设置多个控制。控制 ID 被分组成控制类
(见 :ref:`ctrl-class`)，控制数组中的所有控制必须属于同一个控制类。)

Applications must always fill in the ``count``, ``which``, ``controls``
and ``reserved`` fields of struct
:c:type:`v4l2_ext_controls`, and initialize the
struct :c:type:`v4l2_ext_control` array pointed to
by the ``controls`` fields.
(应用必须始终填写 struct :c:type:`v4l2_ext_controls` 的 ``count``、
``which``、``controls`` 与 ``reserved`` 字段，并初始化 ``controls`` 字段所
指向的 struct :c:type:`v4l2_ext_control` 数组。)

To get the current value of a set of controls applications initialize
the ``id``, ``size`` and ``reserved2`` fields of each struct
:c:type:`v4l2_ext_control` and call the
:ref:`VIDIOC_G_EXT_CTRLS <VIDIOC_G_EXT_CTRLS>` ioctl. String controls must also set the
``string`` field. Controls of compound types
(``V4L2_CTRL_FLAG_HAS_PAYLOAD`` is set) must set the ``ptr`` field.
(要获取一组控制的当前值，应用初始化每个 struct
:c:type:`v4l2_ext_control` 的 ``id``、``size`` 与 ``reserved2`` 字段，然后调
用 :ref:`VIDIOC_G_EXT_CTRLS <VIDIOC_G_EXT_CTRLS>` ioctl。字符串控制还必须
设置 ``string`` 字段。复合类型的控制(设置了 ``V4L2_CTRL_FLAG_HAS_PAYLOAD``)
必须设置 ``ptr`` 字段。)

If the ``size`` is too small to receive the control result (only
relevant for pointer-type controls like strings), then the driver will
set ``size`` to a valid value and return an ``ENOSPC`` error code. You
should re-allocate the memory to this new size and try again. For the
string type it is possible that the same issue occurs again if the
string has grown in the meantime. It is recommended to call
:ref:`VIDIOC_QUERYCTRL` first and use
``maximum``\ +1 as the new ``size`` value. It is guaranteed that that is
sufficient memory.
(如果 ``size`` 太小、装不下控制结果(只与字符串这类指针型控制相关)，驱动会
把 ``size`` 设置为一个有效值并返回 ``ENOSPC`` 错误码。你应当把内存重新分配
到这个新大小再试一次。对字符串类型，如果字符串在此期间变长了，同样的问题
可能再次出现。建议先调用 :ref:`VIDIOC_QUERYCTRL`，并使用
``maximum``\ +1 作为新的 ``size`` 值。可以保证该值所需的内存是充足的。)

N-dimensional arrays are set and retrieved row-by-row. You cannot set a
partial array, all elements have to be set or retrieved. The total size
is calculated as ``elems`` * ``elem_size``. These values can be obtained
by calling :ref:`VIDIOC_QUERY_EXT_CTRL <VIDIOC_QUERYCTRL>`.
(N 维数组按行设置和获取。不能设置部分数组，所有元素都必须被设置或获取。总
大小按 ``elems`` * ``elem_size`` 计算。这些值可通过调用
:ref:`VIDIOC_QUERY_EXT_CTRL <VIDIOC_QUERYCTRL>` 获得。)

To change the value of a set of controls applications initialize the
``id``, ``size``, ``reserved2`` and ``value/value64/string/ptr`` fields
of each struct :c:type:`v4l2_ext_control` and call
the :ref:`VIDIOC_S_EXT_CTRLS <VIDIOC_G_EXT_CTRLS>` ioctl. The controls will only be set if *all*
control values are valid.
(要修改一组控制的值，应用初始化每个 struct :c:type:`v4l2_ext_control` 的
``id``、``size``、``reserved2`` 与 ``value/value64/string/ptr`` 字段，然后调
用 :ref:`VIDIOC_S_EXT_CTRLS <VIDIOC_G_EXT_CTRLS>` ioctl。只有当 *全部* 控
制值都有效时，控制才会被设置。)

To check if a set of controls have correct values applications
initialize the ``id``, ``size``, ``reserved2`` and
``value/value64/string/ptr`` fields of each struct
:c:type:`v4l2_ext_control` and call the
:ref:`VIDIOC_TRY_EXT_CTRLS <VIDIOC_G_EXT_CTRLS>` ioctl. It is up to the driver whether wrong
values are automatically adjusted to a valid value or if an error is
returned.
(要检查一组控制的值是否正确，应用初始化每个 struct
:c:type:`v4l2_ext_control` 的 ``id``、``size``、``reserved2`` 与
``value/value64/string/ptr`` 字段，然后调用
:ref:`VIDIOC_TRY_EXT_CTRLS <VIDIOC_G_EXT_CTRLS>` ioctl。对错误的值是自动调
整为有效值还是返回错误，由驱动自行决定。)

When the ``id`` or ``which`` is invalid drivers return an ``EINVAL`` error
code. When the value is out of bounds drivers can choose to take the
closest valid value or return an ``ERANGE`` error code, whatever seems more
appropriate. In the first case the new value is set in struct
:c:type:`v4l2_ext_control`. If the new control value
is inappropriate (e.g. the given menu index is not supported by the menu
control), then this will also result in an ``EINVAL`` error code error.
(当 ``id`` 或 ``which`` 无效时，驱动返回 ``EINVAL`` 错误码。当值越界时，驱
动可以选择取最接近的有效值或返回 ``ERANGE`` 错误码，看哪种更合适。在第一种
情况下，新值会写入 struct :c:type:`v4l2_ext_control`。如果新的控制值不合
适(例如给定的菜单索引不受该菜单控制支持)，也会导致 ``EINVAL`` 错误码错
误。)

If ``request_fd`` is set to a not-yet-queued :ref:`request <media-request-api>`
file descriptor and ``which`` is set to ``V4L2_CTRL_WHICH_REQUEST_VAL``,
then the controls are not applied immediately when calling
:ref:`VIDIOC_S_EXT_CTRLS <VIDIOC_G_EXT_CTRLS>`, but instead are applied by
the driver for the buffer associated with the same request.
If the device does not support requests, then ``EACCES`` will be returned.
If requests are supported but an invalid request file descriptor is given,
then ``EINVAL`` will be returned.
(如果 ``request_fd`` 被设置为一个尚未入队的 :ref:`request <media-request-api>`
文件描述符，且 ``which`` 被设置为 ``V4L2_CTRL_WHICH_REQUEST_VAL``，那么调
用 :ref:`VIDIOC_S_EXT_CTRLS <VIDIOC_G_EXT_CTRLS>` 时控制不会立即被应用，而
是由驱动在与同一 request 关联的缓冲区上应用。如果设备不支持 request，则返
回 ``EACCES``。如果支持 request 但给定的 request 文件描述符无效，则返回
``EINVAL``。)

An attempt to call :ref:`VIDIOC_S_EXT_CTRLS <VIDIOC_G_EXT_CTRLS>` for a
request that has already been queued will result in an ``EBUSY`` error.
(对一个已经入队的 request 调用
:ref:`VIDIOC_S_EXT_CTRLS <VIDIOC_G_EXT_CTRLS>` 会导致 ``EBUSY`` 错误。)

If ``request_fd`` is specified and ``which`` is set to
``V4L2_CTRL_WHICH_REQUEST_VAL`` during a call to
:ref:`VIDIOC_G_EXT_CTRLS <VIDIOC_G_EXT_CTRLS>`, then it will return the
values of the controls at the time of request completion.
If the request is not yet completed, then this will result in an
``EACCES`` error.
(如果在调用 :ref:`VIDIOC_G_EXT_CTRLS <VIDIOC_G_EXT_CTRLS>` 时指定了
``request_fd`` 且 ``which`` 被设置为 ``V4L2_CTRL_WHICH_REQUEST_VAL``，那么
它将返回 request 完成时刻的控制值。如果 request 尚未完成，将导致
``EACCES`` 错误。)

The driver will only set/get these controls if all control values are
correct. This prevents the situation where only some of the controls
were set/get. Only low-level errors (e. g. a failed i2c command) can
still cause this situation.
(只有在所有控制值都正确时，驱动才会设置/获取这些控制。这避免了只设置/获
取了部分控制的情形。只有低层错误(例如 i2c 命令失败)仍可能造成这种情况。)

.. tabularcolumns:: |p{6.8cm}|p{4.0cm}|p{6.5cm}|

.. c:type:: v4l2_ext_control

.. raw:: latex

   \footnotesize

.. cssclass:: longtable

.. flat-table:: struct v4l2_ext_control
    :header-rows:  0
    :stub-columns: 0
    :widths:       1 1 2

    * - __u32
      - ``id``
      - Identifies the control, set by the application.
	(标识该控制，由应用设置。)
    * - __u32
      - ``size``
      - The total size in bytes of the payload of this control.
	(该控制负载的总大小，以字节为单位。)
    * - :cspan:`2` The ``size`` field is normally 0, but for pointer
	controls this should be set to the size of the memory that contains
	the payload or that will receive the payload.
	If :ref:`VIDIOC_G_EXT_CTRLS <VIDIOC_G_EXT_CTRLS>` finds that this value
	is less than is required to store the payload result, then it is set
	to a value large enough to store the payload result and ``ENOSPC`` is
	returned.
	(``size`` 字段通常为 0，但对于指针型控制，应将其设置为包含负载、或将
	接收负载的内存的大小。如果 :ref:`VIDIOC_G_EXT_CTRLS <VIDIOC_G_EXT_CTRLS>`
	发现该值小于存放负载结果所需的大小，则会把它设置为足以存放负载结果的
	值，并返回 ``ENOSPC``。)

	.. note::

	   For string controls, this ``size`` field should
	   not be confused with the length of the string. This field refers
	   to the size of the memory that contains the string. The actual
	   *length* of the string may well be much smaller.
	   (对于字符串控制，不要把此 ``size`` 字段与字符串的长度相混淆。该字段
	   指的是存放字符串的内存大小。字符串的实际 *长度* 可能小得多。)
    * - __u32
      - ``reserved2``\ [1]
      - Reserved for future extensions. Drivers and applications must set
	the array to zero.
	(为将来的扩展保留。驱动与应用必须把该数组置零。)
    * - union {
      - (anonymous)
    * - __s32
      - ``value``
      - New value or current value. Valid if this control is not of type
	``V4L2_CTRL_TYPE_INTEGER64`` and ``V4L2_CTRL_FLAG_HAS_PAYLOAD`` is
	not set.
	(新值或当前值。当该控制不是 ``V4L2_CTRL_TYPE_INTEGER64`` 类型且未设置
	``V4L2_CTRL_FLAG_HAS_PAYLOAD`` 时有效。)
    * - __s64
      - ``value64``
      - New value or current value. Valid if this control is of type
	``V4L2_CTRL_TYPE_INTEGER64`` and ``V4L2_CTRL_FLAG_HAS_PAYLOAD`` is
	not set.
	(新值或当前值。当该控制是 ``V4L2_CTRL_TYPE_INTEGER64`` 类型且未设置
	``V4L2_CTRL_FLAG_HAS_PAYLOAD`` 时有效。)
    * - char *
      - ``string``
      - A pointer to a string. Valid if this control is of type
	``V4L2_CTRL_TYPE_STRING``.
	(指向字符串的指针。当该控制是 ``V4L2_CTRL_TYPE_STRING`` 类型时有效。)
    * - __u8 *
      - ``p_u8``
      - A pointer to a matrix control of unsigned 8-bit values. Valid if
	this control is of type ``V4L2_CTRL_TYPE_U8``.
	(指向无符号 8 位值矩阵控制的指针。当该控制是 ``V4L2_CTRL_TYPE_U8``
	类型时有效。)
    * - __u16 *
      - ``p_u16``
      - A pointer to a matrix control of unsigned 16-bit values. Valid if
	this control is of type ``V4L2_CTRL_TYPE_U16``.
	(指向无符号 16 位值矩阵控制的指针。当该控制是 ``V4L2_CTRL_TYPE_U16``
	类型时有效。)
    * - __u32 *
      - ``p_u32``
      - A pointer to a matrix control of unsigned 32-bit values. Valid if
	this control is of type ``V4L2_CTRL_TYPE_U32``.
	(指向无符号 32 位值矩阵控制的指针。当该控制是 ``V4L2_CTRL_TYPE_U32``
	类型时有效。)
    * - __s32 *
      - ``p_s32``
      - A pointer to a matrix control of signed 32-bit values. Valid if
        this control is of type ``V4L2_CTRL_TYPE_INTEGER`` and
        ``V4L2_CTRL_FLAG_HAS_PAYLOAD`` is set.
	(指向有符号 32 位值矩阵控制的指针。当该控制是
	``V4L2_CTRL_TYPE_INTEGER`` 类型且设置了 ``V4L2_CTRL_FLAG_HAS_PAYLOAD``
	时有效。)
    * - __s64 *
      - ``p_s64``
      - A pointer to a matrix control of signed 64-bit values. Valid if
        this control is of type ``V4L2_CTRL_TYPE_INTEGER64`` and
        ``V4L2_CTRL_FLAG_HAS_PAYLOAD`` is set.
	(指向有符号 64 位值矩阵控制的指针。当该控制是
	``V4L2_CTRL_TYPE_INTEGER64`` 类型且设置了 ``V4L2_CTRL_FLAG_HAS_PAYLOAD``
	时有效。)
    * - struct :c:type:`v4l2_area` *
      - ``p_area``
      - A pointer to a struct :c:type:`v4l2_area`. Valid if this control is
        of type ``V4L2_CTRL_TYPE_AREA``.
	(指向 struct :c:type:`v4l2_area` 的指针。当该控制是 ``V4L2_CTRL_TYPE_AREA`` 类型时有效。)
    * - struct :c:type:`v4l2_rect` *
      - ``p_rect``
      - A pointer to a struct :c:type:`v4l2_rect`. Valid if this control is
        of type ``V4L2_CTRL_TYPE_RECT``.
	(指向 struct :c:type:`v4l2_rect` 的指针。当该控制是 ``V4L2_CTRL_TYPE_RECT`` 类型时有效。)
    * - struct :c:type:`v4l2_ctrl_h264_sps` *
      - ``p_h264_sps``
      - A pointer to a struct :c:type:`v4l2_ctrl_h264_sps`. Valid if this control is
        of type ``V4L2_CTRL_TYPE_H264_SPS``.
	(指向 struct :c:type:`v4l2_ctrl_h264_sps` 的指针。当该控制是 ``V4L2_CTRL_TYPE_H264_SPS`` 类型时有效。)
    * - struct :c:type:`v4l2_ctrl_h264_pps` *
      - ``p_h264_pps``
      - A pointer to a struct :c:type:`v4l2_ctrl_h264_pps`. Valid if this control is
        of type ``V4L2_CTRL_TYPE_H264_PPS``.
	(指向 struct :c:type:`v4l2_ctrl_h264_pps` 的指针。当该控制是 ``V4L2_CTRL_TYPE_H264_PPS`` 类型时有效。)
    * - struct :c:type:`v4l2_ctrl_h264_scaling_matrix` *
      - ``p_h264_scaling_matrix``
      - A pointer to a struct :c:type:`v4l2_ctrl_h264_scaling_matrix`. Valid if this control is
        of type ``V4L2_CTRL_TYPE_H264_SCALING_MATRIX``.
	(指向 struct :c:type:`v4l2_ctrl_h264_scaling_matrix` 的指针。当该控制是 ``V4L2_CTRL_TYPE_H264_SCALING_MATRIX`` 类型时有效。)
    * - struct :c:type:`v4l2_ctrl_h264_pred_weights` *
      - ``p_h264_pred_weights``
      - A pointer to a struct :c:type:`v4l2_ctrl_h264_pred_weights`. Valid if this control is
        of type ``V4L2_CTRL_TYPE_H264_PRED_WEIGHTS``.
	(指向 struct :c:type:`v4l2_ctrl_h264_pred_weights` 的指针。当该控制是 ``V4L2_CTRL_TYPE_H264_PRED_WEIGHTS`` 类型时有效。)
    * - struct :c:type:`v4l2_ctrl_h264_slice_params` *
      - ``p_h264_slice_params``
      - A pointer to a struct :c:type:`v4l2_ctrl_h264_slice_params`. Valid if this control is
        of type ``V4L2_CTRL_TYPE_H264_SLICE_PARAMS``.
	(指向 struct :c:type:`v4l2_ctrl_h264_slice_params` 的指针。当该控制是 ``V4L2_CTRL_TYPE_H264_SLICE_PARAMS`` 类型时有效。)
    * - struct :c:type:`v4l2_ctrl_h264_decode_params` *
      - ``p_h264_decode_params``
      - A pointer to a struct :c:type:`v4l2_ctrl_h264_decode_params`. Valid if this control is
        of type ``V4L2_CTRL_TYPE_H264_DECODE_PARAMS``.
	(指向 struct :c:type:`v4l2_ctrl_h264_decode_params` 的指针。当该控制是 ``V4L2_CTRL_TYPE_H264_DECODE_PARAMS`` 类型时有效。)
    * - struct :c:type:`v4l2_ctrl_fwht_params` *
      - ``p_fwht_params``
      - A pointer to a struct :c:type:`v4l2_ctrl_fwht_params`. Valid if this control is
        of type ``V4L2_CTRL_TYPE_FWHT_PARAMS``.
	(指向 struct :c:type:`v4l2_ctrl_fwht_params` 的指针。当该控制是 ``V4L2_CTRL_TYPE_FWHT_PARAMS`` 类型时有效。)
    * - struct :c:type:`v4l2_ctrl_vp8_frame` *
      - ``p_vp8_frame``
      - A pointer to a struct :c:type:`v4l2_ctrl_vp8_frame`. Valid if this control is
        of type ``V4L2_CTRL_TYPE_VP8_FRAME``.
	(指向 struct :c:type:`v4l2_ctrl_vp8_frame` 的指针。当该控制是 ``V4L2_CTRL_TYPE_VP8_FRAME`` 类型时有效。)
    * - struct :c:type:`v4l2_ctrl_mpeg2_sequence` *
      - ``p_mpeg2_sequence``
      - A pointer to a struct :c:type:`v4l2_ctrl_mpeg2_sequence`. Valid if this control is
        of type ``V4L2_CTRL_TYPE_MPEG2_SEQUENCE``.
	(指向 struct :c:type:`v4l2_ctrl_mpeg2_sequence` 的指针。当该控制是 ``V4L2_CTRL_TYPE_MPEG2_SEQUENCE`` 类型时有效。)
    * - struct :c:type:`v4l2_ctrl_mpeg2_picture` *
      - ``p_mpeg2_picture``
      - A pointer to a struct :c:type:`v4l2_ctrl_mpeg2_picture`. Valid if this control is
        of type ``V4L2_CTRL_TYPE_MPEG2_PICTURE``.
	(指向 struct :c:type:`v4l2_ctrl_mpeg2_picture` 的指针。当该控制是 ``V4L2_CTRL_TYPE_MPEG2_PICTURE`` 类型时有效。)
    * - struct :c:type:`v4l2_ctrl_mpeg2_quantisation` *
      - ``p_mpeg2_quantisation``
      - A pointer to a struct :c:type:`v4l2_ctrl_mpeg2_quantisation`. Valid if this control is
        of type ``V4L2_CTRL_TYPE_MPEG2_QUANTISATION``.
	(指向 struct :c:type:`v4l2_ctrl_mpeg2_quantisation` 的指针。当该控制是 ``V4L2_CTRL_TYPE_MPEG2_QUANTISATION`` 类型时有效。)
    * - struct :c:type:`v4l2_ctrl_vp9_compressed_hdr` *
      - ``p_vp9_compressed_hdr_probs``
      - A pointer to a struct :c:type:`v4l2_ctrl_vp9_compressed_hdr`. Valid if this
        control is of type ``V4L2_CTRL_TYPE_VP9_COMPRESSED_HDR``.
	(指向 struct :c:type:`v4l2_ctrl_vp9_compressed_hdr` 的指针。当该控制是 ``V4L2_CTRL_TYPE_VP9_COMPRESSED_HDR`` 类型时有效。)
    * - struct :c:type:`v4l2_ctrl_vp9_frame` *
      - ``p_vp9_frame``
      - A pointer to a struct :c:type:`v4l2_ctrl_vp9_frame`. Valid if this
        control is of type ``V4L2_CTRL_TYPE_VP9_FRAME``.
	(指向 struct :c:type:`v4l2_ctrl_vp9_frame` 的指针。当该控制是 ``V4L2_CTRL_TYPE_VP9_FRAME`` 类型时有效。)
    * - struct :c:type:`v4l2_ctrl_hdr10_cll_info` *
      - ``p_hdr10_cll``
      - A pointer to a struct :c:type:`v4l2_ctrl_hdr10_cll_info`. Valid if this control is
        of type ``V4L2_CTRL_TYPE_HDR10_CLL_INFO``.
	(指向 struct :c:type:`v4l2_ctrl_hdr10_cll_info` 的指针。当该控制是 ``V4L2_CTRL_TYPE_HDR10_CLL_INFO`` 类型时有效。)
    * - struct :c:type:`v4l2_ctrl_hdr10_mastering_display` *
      - ``p_hdr10_mastering``
      - A pointer to a struct :c:type:`v4l2_ctrl_hdr10_mastering_display`. Valid if this control is
        of type ``V4L2_CTRL_TYPE_HDR10_MASTERING_DISPLAY``.
	(指向 struct :c:type:`v4l2_ctrl_hdr10_mastering_display` 的指针。当该控制是 ``V4L2_CTRL_TYPE_HDR10_MASTERING_DISPLAY`` 类型时有效。)
    * - struct :c:type:`v4l2_ctrl_hevc_sps` *
      - ``p_hevc_sps``
      - A pointer to a struct :c:type:`v4l2_ctrl_hevc_sps`. Valid if this
        control is of type ``V4L2_CTRL_TYPE_HEVC_SPS``.
	(指向 struct :c:type:`v4l2_ctrl_hevc_sps` 的指针。当该控制是 ``V4L2_CTRL_TYPE_HEVC_SPS`` 类型时有效。)
    * - struct :c:type:`v4l2_ctrl_hevc_pps` *
      - ``p_hevc_pps``
      - A pointer to a struct :c:type:`v4l2_ctrl_hevc_pps`. Valid if this
        control is of type ``V4L2_CTRL_TYPE_HEVC_PPS``.
	(指向 struct :c:type:`v4l2_ctrl_hevc_pps` 的指针。当该控制是 ``V4L2_CTRL_TYPE_HEVC_PPS`` 类型时有效。)
    * - struct :c:type:`v4l2_ctrl_hevc_slice_params` *
      - ``p_hevc_slice_params``
      - A pointer to a struct :c:type:`v4l2_ctrl_hevc_slice_params`. Valid if this
        control is of type ``V4L2_CTRL_TYPE_HEVC_SLICE_PARAMS``.
	(指向 struct :c:type:`v4l2_ctrl_hevc_slice_params` 的指针。当该控制是 ``V4L2_CTRL_TYPE_HEVC_SLICE_PARAMS`` 类型时有效。)
    * - struct :c:type:`v4l2_ctrl_hevc_scaling_matrix` *
      - ``p_hevc_scaling_matrix``
      - A pointer to a struct :c:type:`v4l2_ctrl_hevc_scaling_matrix`. Valid if this
        control is of type ``V4L2_CTRL_TYPE_HEVC_SCALING_MATRIX``.
	(指向 struct :c:type:`v4l2_ctrl_hevc_scaling_matrix` 的指针。当该控制是 ``V4L2_CTRL_TYPE_HEVC_SCALING_MATRIX`` 类型时有效。)
    * - struct :c:type:`v4l2_ctrl_hevc_decode_params` *
      - ``p_hevc_decode_params``
      - A pointer to a struct :c:type:`v4l2_ctrl_hevc_decode_params`. Valid if this
        control is of type ``V4L2_CTRL_TYPE_HEVC_DECODE_PARAMS``.
	(指向 struct :c:type:`v4l2_ctrl_hevc_decode_params` 的指针。当该控制是 ``V4L2_CTRL_TYPE_HEVC_DECODE_PARAMS`` 类型时有效。)
    * - struct :c:type:`v4l2_ctrl_av1_sequence` *
      - ``p_av1_sequence``
      - A pointer to a struct :c:type:`v4l2_ctrl_av1_sequence`. Valid if this control is
        of type ``V4L2_CTRL_TYPE_AV1_SEQUENCE``.
	(指向 struct :c:type:`v4l2_ctrl_av1_sequence` 的指针。当该控制是 ``V4L2_CTRL_TYPE_AV1_SEQUENCE`` 类型时有效。)
    * - struct :c:type:`v4l2_ctrl_av1_tile_group_entry` *
      - ``p_av1_tile_group_entry``
      - A pointer to a struct :c:type:`v4l2_ctrl_av1_tile_group_entry`. Valid if this control is
        of type ``V4L2_CTRL_TYPE_AV1_TILE_GROUP_ENTRY``.
	(指向 struct :c:type:`v4l2_ctrl_av1_tile_group_entry` 的指针。当该控制是 ``V4L2_CTRL_TYPE_AV1_TILE_GROUP_ENTRY`` 类型时有效。)
    * - struct :c:type:`v4l2_ctrl_av1_frame` *
      - ``p_av1_frame``
      - A pointer to a struct :c:type:`v4l2_ctrl_av1_frame`. Valid if this control is
        of type ``V4L2_CTRL_TYPE_AV1_FRAME``.
	(指向 struct :c:type:`v4l2_ctrl_av1_frame` 的指针。当该控制是 ``V4L2_CTRL_TYPE_AV1_FRAME`` 类型时有效。)
    * - struct :c:type:`v4l2_ctrl_av1_film_grain` *
      - ``p_av1_film_grain``
      - A pointer to a struct :c:type:`v4l2_ctrl_av1_film_grain`. Valid if this control is
        of type ``V4L2_CTRL_TYPE_AV1_FILM_GRAIN``.
	(指向 struct :c:type:`v4l2_ctrl_av1_film_grain` 的指针。当该控制是 ``V4L2_CTRL_TYPE_AV1_FILM_GRAIN`` 类型时有效。)
    * - struct :c:type:`v4l2_ctrl_hdr10_cll_info` *
      - ``p_hdr10_cll_info``
      - A pointer to a struct :c:type:`v4l2_ctrl_hdr10_cll_info`. Valid if this control is
        of type ``V4L2_CTRL_TYPE_HDR10_CLL_INFO``.
	(指向 struct :c:type:`v4l2_ctrl_hdr10_cll_info` 的指针。当该控制是 ``V4L2_CTRL_TYPE_HDR10_CLL_INFO`` 类型时有效。)
    * - struct :c:type:`v4l2_ctrl_hdr10_mastering_display` *
      - ``p_hdr10_mastering_display``
      - A pointer to a struct :c:type:`v4l2_ctrl_hdr10_mastering_display`. Valid if this control is
        of type ``V4L2_CTRL_TYPE_HDR10_MASTERING_DISPLAY``.
	(指向 struct :c:type:`v4l2_ctrl_hdr10_mastering_display` 的指针。当该控制是 ``V4L2_CTRL_TYPE_HDR10_MASTERING_DISPLAY`` 类型时有效。)
    * - void *
      - ``ptr``
      - A pointer to a compound type which can be an N-dimensional array
	and/or a compound type (the control's type is >=
	``V4L2_CTRL_COMPOUND_TYPES``). Valid if
	``V4L2_CTRL_FLAG_HAS_PAYLOAD`` is set for this control.
	(指向一个复合类型的指针，可以是 N 维数组和/或复合类型——该控制的类型
	>= ``V4L2_CTRL_COMPOUND_TYPES``。当该控制设置了 ``V4L2_CTRL_FLAG_HAS_PAYLOAD``
	时有效。)
    * - }
      -

.. raw:: latex

   \normalsize

.. tabularcolumns:: |p{4.0cm}|p{2.5cm}|p{10.8cm}|

.. c:type:: v4l2_ext_controls

.. cssclass:: longtable

.. flat-table:: struct v4l2_ext_controls
    :header-rows:  0
    :stub-columns: 0
    :widths:       1 1 2

    * - union {
      - (anonymous)
    * - __u32
      - ``which``
      - Which value of the control to get/set/try.
	(要获取/设置/试用其值的控制。)
    * - :cspan:`2` ``V4L2_CTRL_WHICH_CUR_VAL`` will return the current value of
	the control, ``V4L2_CTRL_WHICH_DEF_VAL`` will return the default
	value of the control, ``V4L2_CTRL_WHICH_MIN_VAL`` will return the minimum
	value of the control, and ``V4L2_CTRL_WHICH_MAX_VAL`` will return the maximum
	value of the control. ``V4L2_CTRL_WHICH_REQUEST_VAL`` indicates that
	the control value has to be retrieved from a request or tried/set for
	a request. In that case the ``request_fd`` field contains the
	file descriptor of the request that should be used. If the device
	does not support requests, then ``EACCES`` will be returned.

	When using ``V4L2_CTRL_WHICH_DEF_VAL``, ``V4L2_CTRL_WHICH_MIN_VAL``
	or ``V4L2_CTRL_WHICH_MAX_VAL`` be aware that you can only get the
	default/minimum/maximum value of the control, you cannot set or try it.

	Whether a control supports querying the minimum and maximum values using
	``V4L2_CTRL_WHICH_MIN_VAL`` and ``V4L2_CTRL_WHICH_MAX_VAL`` is indicated
	by the ``V4L2_CTRL_FLAG_HAS_WHICH_MIN_MAX`` flag. Most non-compound
	control types support this. For controls with compound types, the
	definition of minimum/maximum values are provided by
	the control documentation. If a compound control does not document the
	meaning of minimum/maximum value, then querying the minimum or maximum
	value will result in the error code -EINVAL.

	For backwards compatibility you can also use a control class here
	(see :ref:`ctrl-class`). In that case all controls have to
	belong to that control class. This usage is deprecated, instead
	just use ``V4L2_CTRL_WHICH_CUR_VAL``. There are some very old
	drivers that do not yet support ``V4L2_CTRL_WHICH_CUR_VAL`` and
	that require a control class here. You can test for such drivers
	by setting ``which`` to ``V4L2_CTRL_WHICH_CUR_VAL`` and calling
	:ref:`VIDIOC_TRY_EXT_CTRLS <VIDIOC_G_EXT_CTRLS>` with a count of 0.
	If that fails, then the driver does not support ``V4L2_CTRL_WHICH_CUR_VAL``.
	(``V4L2_CTRL_WHICH_CUR_VAL`` 将返回控制的当前值，``V4L2_CTRL_WHICH_DEF_VAL``
	将返回控制的默认值，``V4L2_CTRL_WHICH_MIN_VAL`` 将返回控制的最小值，
	``V4L2_CTRL_WHICH_MAX_VAL`` 将返回控制的最大值。
	``V4L2_CTRL_WHICH_REQUEST_VAL`` 表示控制值要从某个 request 中获取，或
	为某个 request 试设/设置。此时 ``request_fd`` 字段包含应使用的 request
	的文件描述符。如果设备不支持 request，则返回 ``EACCES``。
	
	使用 ``V4L2_CTRL_WHICH_DEF_VAL``、``V4L2_CTRL_WHICH_MIN_VAL`` 或
	``V4L2_CTRL_WHICH_MAX_VAL`` 时要注意：你只能获取控制的默认值/最小值/最
	大值，不能设置或试用它。
	
	某个控制是否支持用 ``V4L2_CTRL_WHICH_MIN_VAL`` 与 ``V4L2_CTRL_WHICH_MAX_VAL``
	查询最小值和最大值，由 ``V4L2_CTRL_FLAG_HAS_WHICH_MIN_MAX`` 标志指示。大
	多数非复合控制类型都支持。对复合类型的控制，最小值/最大值的含义由该控
	制的文档给出。如果复合控制没有说明最小值/最大值的含义，查询最小值或最
	大值会得到错误码 -EINVAL。
	
	为了向后兼容，你也可以在这里使用控制类(见 :ref:`ctrl-class`)。此时所有
	控制都必须属于该控制类。这种用法已废弃，请直接使用
	``V4L2_CTRL_WHICH_CUR_VAL``。有一些非常老的驱动尚不支持
	``V4L2_CTRL_WHICH_CUR_VAL``，而要求在这里使用控制类。你可以把 ``which``
	设为 ``V4L2_CTRL_WHICH_CUR_VAL`` 并以 count 为 0 调用
	:ref:`VIDIOC_TRY_EXT_CTRLS <VIDIOC_G_EXT_CTRLS>` 来测试这类驱动。如果
	失败，则说明驱动不支持 ``V4L2_CTRL_WHICH_CUR_VAL``。)
    * - __u32
      - ``ctrl_class``
      - Deprecated name kept for backwards compatibility. Use ``which`` instead.
	(为向后兼容而保留的废弃名称。请改用 ``which``。)
    * - }
      -
    * - __u32
      - ``count``
      - The number of controls in the controls array. May also be zero.
	(controls 数组中控制的数量。也可以为 0。)
    * - __u32
      - ``error_idx``
      - Index of the failing control. Set by the driver in case of an error.
	(失败控制的索引。出错时由驱动设置。)
    * - :cspan:`2` If the error is associated
	with a particular control, then ``error_idx`` is set to the index
	of that control. If the error is not related to a specific
	control, or the validation step failed (see below), then
	``error_idx`` is set to ``count``. The value is undefined if the
	ioctl returned 0 (success).

	Before controls are read from/written to hardware a validation
	step takes place: this checks if all controls in the list are
	valid controls, if no attempt is made to write to a read-only
	control or read from a write-only control, and any other up-front
	checks that can be done without accessing the hardware. The exact
	validations done during this step are driver dependent since some
	checks might require hardware access for some devices, thus making
	it impossible to do those checks up-front. However, drivers should
	make a best-effort to do as many up-front checks as possible.

	This check is done to avoid leaving the hardware in an
	inconsistent state due to easy-to-avoid problems. But it leads to
	another problem: the application needs to know whether an error
	came from the validation step (meaning that the hardware was not
	touched) or from an error during the actual reading from/writing
	to hardware.

	The, in hindsight quite poor, solution for that is to set
	``error_idx`` to ``count`` if the validation failed. This has the
	unfortunate side-effect that it is not possible to see which
	control failed the validation. If the validation was successful
	and the error happened while accessing the hardware, then
	``error_idx`` is less than ``count`` and only the controls up to
	``error_idx-1`` were read or written correctly, and the state of
	the remaining controls is undefined.

	Since :ref:`VIDIOC_TRY_EXT_CTRLS <VIDIOC_G_EXT_CTRLS>` does not access hardware there is
	also no need to handle the validation step in this special way, so
	``error_idx`` will just be set to the control that failed the
	validation step instead of to ``count``. This means that if
	:ref:`VIDIOC_S_EXT_CTRLS <VIDIOC_G_EXT_CTRLS>` fails with ``error_idx`` set to ``count``,
	then you can call :ref:`VIDIOC_TRY_EXT_CTRLS <VIDIOC_G_EXT_CTRLS>` to try to discover the
	actual control that failed the validation step. Unfortunately,
	there is no ``TRY`` equivalent for :ref:`VIDIOC_G_EXT_CTRLS <VIDIOC_G_EXT_CTRLS>`.
	(如果错误与某个特定控制相关，则 ``error_idx`` 会被设置为该控制的索引。
	如果错误与特定控制无关，或验证步骤失败(见下文)，则 ``error_idx`` 被设
	置为 ``count``。如果 ioctl 返回 0(成功)，该值是未定义的。
	
	在从硬件读取/写入控制之前会有一个验证步骤：它检查列表中的所有控制是否
	都是有效控制、是否没有试图写只读控制或读写控制，以及任何其他无需访问
	硬件即可完成的前置检查。这一步所做的确切验证因驱动而异，因为某些检查
	对某些设备可能需要访问硬件，因而无法前置完成。不过，驱动应尽力完成尽
	可能多的前置检查。
	
	做这个检查是为了避免因容易避免的问题把硬件留在不一致的状态。但这也带
	来另一个问题：应用需要知道错误是来自验证步骤(意味着硬件未被触碰)，还
	是来自实际读写硬件时的错误。
	
	事后看来相当糟糕的解决办法是：验证失败时把 ``error_idx`` 设为
	``count``。这带来一个不幸的副作用：无法看出是哪个控制未通过验证。如果
	验证成功而错误发生在访问硬件时，则 ``error_idx`` 小于 ``count``，且只有
	到 ``error_idx-1`` 为止的控制被正确读写，其余控制的状态是未定义的。
	
	由于 :ref:`VIDIOC_TRY_EXT_CTRLS <VIDIOC_G_EXT_CTRLS>` 不访问硬件，因此
	无需以这种特殊方式处理验证步骤，``error_idx`` 会直接被设为未通过验证步
	骤的那个控制，而不是 ``count``。这意味着如果
	:ref:`VIDIOC_S_EXT_CTRLS <VIDIOC_G_EXT_CTRLS>` 失败且 ``error_idx`` 等
	于 ``count``，你可以调用 :ref:`VIDIOC_TRY_EXT_CTRLS <VIDIOC_G_EXT_CTRLS>`
	来尝试找出未通过验证步骤的实际控制。遗憾的是，
	:ref:`VIDIOC_G_EXT_CTRLS <VIDIOC_G_EXT_CTRLS>` 没有 ``TRY`` 等价物。)
    * - __s32
      - ``request_fd``
      - File descriptor of the request to be used by this operation. Only
	valid if ``which`` is set to ``V4L2_CTRL_WHICH_REQUEST_VAL``.
	If the device does not support requests, then ``EACCES`` will be returned.
	If requests are supported but an invalid request file descriptor is
	given, then ``EINVAL`` will be returned.
	(本次操作要使用的 request 的文件描述符。仅当 ``which`` 被设置为
	``V4L2_CTRL_WHICH_REQUEST_VAL`` 时有效。如果设备不支持 request，则返回
	``EACCES``。如果支持 request 但给定的 request 文件描述符无效，则返回
	``EINVAL``。)
    * - __u32
      - ``reserved``\ [1]
      - Reserved for future extensions.

	Drivers and applications must set the array to zero.
	(为将来的扩展保留。驱动与应用必须把该数组置零。)
    * - struct :c:type:`v4l2_ext_control` *
      - ``controls``
      - Pointer to an array of ``count`` v4l2_ext_control structures.

	Ignored if ``count`` equals zero.
	(指向由 ``count`` 个 v4l2_ext_control 结构组成的数组的指针。当
	``count`` 等于 0 时被忽略。)

.. tabularcolumns:: |p{7.3cm}|p{2.0cm}|p{8.0cm}|

.. cssclass:: longtable

.. _ctrl-class:

.. flat-table:: Control classes
    :header-rows:  0
    :stub-columns: 0
    :widths:       3 1 4

    * - ``V4L2_CTRL_CLASS_USER``
      - 0x980000
      - The class containing user controls. These controls are described
	in :ref:`control`. All controls that can be set using the
	:ref:`VIDIOC_S_CTRL <VIDIOC_G_CTRL>` and
	:ref:`VIDIOC_G_CTRL <VIDIOC_G_CTRL>` ioctl belong to this
	class.
	(包含用户控制的类。这些控制在 :ref:`control` 中描述。所有可以用
	:ref:`VIDIOC_S_CTRL <VIDIOC_G_CTRL>` 与 :ref:`VIDIOC_G_CTRL <VIDIOC_G_CTRL>`
	ioctl 设置的控制都属于这个类。)
    * - ``V4L2_CTRL_CLASS_CODEC``
      - 0x990000
      - The class containing stateful codec controls. These controls are
	described in :ref:`codec-controls`.
	(包含有状态 codec 控制的类。这些控制在 :ref:`codec-controls` 中描述。)
    * - ``V4L2_CTRL_CLASS_CAMERA``
      - 0x9a0000
      - The class containing camera controls. These controls are described
	in :ref:`camera-controls`.
	(包含摄像头控制的类。这些控制在 :ref:`camera-controls` 中描述。)
    * - ``V4L2_CTRL_CLASS_FM_TX``
      - 0x9b0000
      - The class containing FM Transmitter (FM TX) controls. These
	controls are described in :ref:`fm-tx-controls`.
	(包含 FM 发射 (FM TX) 控制的类。这些控制在 :ref:`fm-tx-controls` 中描述。)
    * - ``V4L2_CTRL_CLASS_FLASH``
      - 0x9c0000
      - The class containing flash device controls. These controls are
	described in :ref:`flash-controls`.
	(包含闪光灯设备控制的类。这些控制在 :ref:`flash-controls` 中描述。)
    * - ``V4L2_CTRL_CLASS_JPEG``
      - 0x9d0000
      - The class containing JPEG compression controls. These controls are
	described in :ref:`jpeg-controls`.
	(包含 JPEG 压缩控制的类。这些控制在 :ref:`jpeg-controls` 中描述。)
    * - ``V4L2_CTRL_CLASS_IMAGE_SOURCE``
      - 0x9e0000
      - The class containing image source controls. These controls are
	described in :ref:`image-source-controls`.
	(包含图像源控制的类。这些控制在 :ref:`image-source-controls` 中描述。)
    * - ``V4L2_CTRL_CLASS_IMAGE_PROC``
      - 0x9f0000
      - The class containing image processing controls. These controls are
	described in :ref:`image-process-controls`.
	(包含图像处理控制的类。这些控制在 :ref:`image-process-controls` 中描述。)
    * - ``V4L2_CTRL_CLASS_FM_RX``
      - 0xa10000
      - The class containing FM Receiver (FM RX) controls. These controls
	are described in :ref:`fm-rx-controls`.
	(包含 FM 接收 (FM RX) 控制的类。这些控制在 :ref:`fm-rx-controls` 中描述。)
    * - ``V4L2_CTRL_CLASS_RF_TUNER``
      - 0xa20000
      - The class containing RF tuner controls. These controls are
	described in :ref:`rf-tuner-controls`.
	(包含 RF 调谐器控制的类。这些控制在 :ref:`rf-tuner-controls` 中描述。)
    * - ``V4L2_CTRL_CLASS_DETECT``
      - 0xa30000
      - The class containing motion or object detection controls. These controls
        are described in :ref:`detect-controls`.
	(包含运动或目标检测控制的类。这些控制在 :ref:`detect-controls` 中描述。)
    * - ``V4L2_CTRL_CLASS_CODEC_STATELESS``
      - 0xa40000
      - The class containing stateless codec controls. These controls are
	described in :ref:`codec-stateless-controls`.
	(包含无状态 codec 控制的类。这些控制在 :ref:`codec-stateless-controls` 中描述。)
    * - ``V4L2_CTRL_CLASS_COLORIMETRY``
      - 0xa50000
      - The class containing colorimetry controls. These controls are
	described in :ref:`colorimetry-controls`.
	(包含色度学控制的类。这些控制在 :ref:`colorimetry-controls` 中描述。)

Return Value
============

On success 0 is returned, on error -1 and the ``errno`` variable is set
appropriately. The generic error codes are described at the
:ref:`Generic Error Codes <gen-errors>` chapter.
(成功时返回 0；出错时返回 -1，并适当设置 ``errno`` 变量。通用错误码在
:ref:`Generic Error Codes <gen-errors>` 一章中描述。)

EINVAL
    The struct :c:type:`v4l2_ext_control` ``id`` is
    invalid, or the struct :c:type:`v4l2_ext_controls`
    ``which`` is invalid, or the struct
    :c:type:`v4l2_ext_control` ``value`` was
    inappropriate (e.g. the given menu index is not supported by the
    driver), or the ``which`` field was set to ``V4L2_CTRL_WHICH_REQUEST_VAL``
    but the given ``request_fd`` was invalid or ``V4L2_CTRL_WHICH_REQUEST_VAL``
    is not supported by the kernel.
    This error code is also returned by the
    :ref:`VIDIOC_S_EXT_CTRLS <VIDIOC_G_EXT_CTRLS>` and :ref:`VIDIOC_TRY_EXT_CTRLS <VIDIOC_G_EXT_CTRLS>` ioctls if two or
    more control values are in conflict.
    (struct :c:type:`v4l2_ext_control` 的 ``id`` 无效；或 struct
    :c:type:`v4l2_ext_controls` 的 ``which`` 无效；或 struct
    :c:type:`v4l2_ext_control` 的 ``value`` 不合适(例如给定的菜单索引不受驱
    动支持)；或 ``which`` 字段被设置为 ``V4L2_CTRL_WHICH_REQUEST_VAL`` 但给
    定的 ``request_fd`` 无效，或内核不支持 ``V4L2_CTRL_WHICH_REQUEST_VAL``。
    如果两个或更多控制值相互冲突，:ref:`VIDIOC_S_EXT_CTRLS <VIDIOC_G_EXT_CTRLS>`
    与 :ref:`VIDIOC_TRY_EXT_CTRLS <VIDIOC_G_EXT_CTRLS>` ioctl 也会返回此错
    误码。)

ERANGE
    The struct :c:type:`v4l2_ext_control` ``value``
    is out of bounds.
    (struct :c:type:`v4l2_ext_control` 的 ``value`` 越界。)

EBUSY
    The control is temporarily not changeable, possibly because another
    applications took over control of the device function this control
    belongs to, or (if the ``which`` field was set to
    ``V4L2_CTRL_WHICH_REQUEST_VAL``) the request was queued but not yet
    completed.
    (该控制暂时不可更改，可能因为另一个应用接管了该控制所属的设备功能；或
    (当 ``which`` 字段被设置为 ``V4L2_CTRL_WHICH_REQUEST_VAL`` 时)request 已
    入队但尚未完成。)

ENOSPC
    The space reserved for the control's payload is insufficient. The
    field ``size`` is set to a value that is enough to store the payload
    and this error code is returned.
    (为控制负载保留的空间不足。字段 ``size`` 会被设置为足以存放负载的值，并
    返回此错误码。)

EACCES
    Attempt to try or set a read-only control, or to get a write-only
    control, or to get a control from a request that has not yet been
    completed.

    Or the ``which`` field was set to ``V4L2_CTRL_WHICH_REQUEST_VAL`` but the
    device does not support requests.

    Or if there is an attempt to set an inactive control and the driver is
    not capable of caching the new value until the control is active again.
    (试图试用或设置一个只读控制、获取一个只写控制、或从一个尚未完成的
    request 获取控制。

    或者 ``which`` 字段被设置为 ``V4L2_CTRL_WHICH_REQUEST_VAL`` 但设备不支
    持 request。

    或者试图设置一个非活动的控制，而驱动无法把新值缓存到该控制重新活动为
    止。)
