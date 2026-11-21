.. SPDX-License-Identifier: GFDL-1.1-no-invariants-or-later
.. c:namespace:: V4L

.. _VIDIOC_S_HW_FREQ_SEEK:

***************************
ioctl VIDIOC_S_HW_FREQ_SEEK
***************************

Name(名称)
==========

VIDIOC_S_HW_FREQ_SEEK - Perform a hardware frequency seek
(VIDIOC_S_HW_FREQ_SEEK - 执行硬件频率搜索。)

Synopsis(概要)
==============

.. c:macro:: VIDIOC_S_HW_FREQ_SEEK

``int ioctl(int fd, VIDIOC_S_HW_FREQ_SEEK, struct v4l2_hw_freq_seek *argp)``

Arguments(参数)
===============

``fd``
    File descriptor returned by :c:func:`open()`.
    (由 :c:func:`open()` 返回的文件描述符。)

``argp``
    Pointer to struct :c:type:`v4l2_hw_freq_seek`.
    (指向 struct :c:type:`v4l2_hw_freq_seek` 的指针。)

Description(描述)
=================

Start a hardware frequency seek from the current frequency. To do this
applications initialize the ``tuner``, ``type``, ``seek_upward``,
``wrap_around``, ``spacing``, ``rangelow`` and ``rangehigh`` fields, and
zero out the ``reserved`` array of a struct
:c:type:`v4l2_hw_freq_seek` and call the
``VIDIOC_S_HW_FREQ_SEEK`` ioctl with a pointer to this structure.
(从当前频率开始硬件频率搜索。为此，应用程序初始化 struct :c:type:`v4l2_hw_freq_seek` 的 ``tuner`` 、 ``type`` 、 ``seek_upward`` 、 ``wrap_around`` 、 ``spacing`` 、 ``rangelow`` 和 ``rangehigh`` 字段，并把 ``reserved`` 数组清零，然后调用 ``VIDIOC_S_HW_FREQ_SEEK`` ioctl，并传入指向该结构的指针。)

The ``rangelow`` and ``rangehigh`` fields can be set to a non-zero value
to tell the driver to search a specific band. If the struct
:c:type:`v4l2_tuner` ``capability`` field has the
``V4L2_TUNER_CAP_HWSEEK_PROG_LIM`` flag set, these values must fall
within one of the bands returned by
:ref:`VIDIOC_ENUM_FREQ_BANDS`. If the
``V4L2_TUNER_CAP_HWSEEK_PROG_LIM`` flag is not set, then these values
must exactly match those of one of the bands returned by
:ref:`VIDIOC_ENUM_FREQ_BANDS`. If the
current frequency of the tuner does not fall within the selected band it
will be clamped to fit in the band before the seek is started.
(可以把 ``rangelow`` 和 ``rangehigh`` 字段设置为非零值，以告知驱动程序搜索特定频段。如果 struct :c:type:`v4l2_tuner` 的 ``capability`` 字段设置了 ``V4L2_TUNER_CAP_HWSEEK_PROG_LIM`` 标志，则这些值必须落在 :ref:`VIDIOC_ENUM_FREQ_BANDS` 返回的某个频段之内。如果未设置 ``V4L2_TUNER_CAP_HWSEEK_PROG_LIM`` 标志，则这些值必须与 :ref:`VIDIOC_ENUM_FREQ_BANDS` 返回的某个频段的值完全匹配。如果调谐器的当前频率不在所选频段之内，则在搜索开始前会将其钳位到该频段内。)

If an error is returned, then the original frequency will be restored.
(如果返回错误，则恢复原来的频率。)

This ioctl is supported if the ``V4L2_CAP_HW_FREQ_SEEK`` capability is
set.
(如果设置了 ``V4L2_CAP_HW_FREQ_SEEK`` 能力，则支持此 ioctl。)

If this ioctl is called from a non-blocking filehandle, then ``EAGAIN``
error code is returned and no seek takes place.
(如果从非阻塞文件句柄调用此 ioctl，则返回 ``EAGAIN`` 错误码，且不会执行搜索。)

.. tabularcolumns:: |p{4.4cm}|p{4.4cm}|p{8.5cm}|

.. c:type:: v4l2_hw_freq_seek

.. flat-table:: struct v4l2_hw_freq_seek
    :header-rows:  0
    :stub-columns: 0
    :widths:       1 1 2

    * - __u32
      - ``tuner``
      - The tuner index number. This is the same value as in the struct
	:c:type:`v4l2_input` ``tuner`` field and the struct
	:c:type:`v4l2_tuner` ``index`` field.
        (调谐器索引号。与 struct :c:type:`v4l2_input` 的 ``tuner`` 字段以及 struct :c:type:`v4l2_tuner` 的 ``index`` 字段中的值相同。)
    * - __u32
      - ``type``
      - The tuner type. This is the same value as in the struct
	:c:type:`v4l2_tuner` ``type`` field. See
	:c:type:`v4l2_tuner_type`
        (调谐器类型。与 struct :c:type:`v4l2_tuner` 的 ``type`` 字段中的值相同。参见 :c:type:`v4l2_tuner_type` 。)
    * - __u32
      - ``seek_upward``
      - If non-zero, seek upward from the current frequency, else seek
	downward.
        (若非零，则从当前频率向上搜索；否则向下搜索。)
    * - __u32
      - ``wrap_around``
      - If non-zero, wrap around when at the end of the frequency range,
	else stop seeking. The struct :c:type:`v4l2_tuner`
	``capability`` field will tell you what the hardware supports.
        (若非零，到达频率范围末端时回绕，否则停止搜索。struct :c:type:`v4l2_tuner` 的 ``capability`` 字段会告诉你硬件支持哪些功能。)
    * - __u32
      - ``spacing``
      - If non-zero, defines the hardware seek resolution in Hz. The
	driver selects the nearest value that is supported by the device.
	If spacing is zero a reasonable default value is used.
        (若非零，以 Hz 为单位定义硬件搜索的步进。驱动程序会选择设备支持的最接近值。若 spacing 为零，则使用合理的默认值。)
    * - __u32
      - ``rangelow``
      - If non-zero, the lowest tunable frequency of the band to search in
	units of 62.5 kHz, or if the struct
	:c:type:`v4l2_tuner` ``capability`` field has the
	``V4L2_TUNER_CAP_LOW`` flag set, in units of 62.5 Hz or if the
	struct :c:type:`v4l2_tuner` ``capability`` field has
	the ``V4L2_TUNER_CAP_1HZ`` flag set, in units of 1 Hz. If
	``rangelow`` is zero a reasonable default value is used.
        (若非零，为要搜索的频段中最低的可调谐频率，单位为 62.5 kHz；若 struct :c:type:`v4l2_tuner` 的 ``capability`` 字段设置了 ``V4L2_TUNER_CAP_LOW`` 标志，则单位为 62.5 Hz；若该字段设置了 ``V4L2_TUNER_CAP_1HZ`` 标志，则单位为 1 Hz。若 ``rangelow`` 为零，则使用合理的默认值。)
    * - __u32
      - ``rangehigh``
      - If non-zero, the highest tunable frequency of the band to search
	in units of 62.5 kHz, or if the struct
	:c:type:`v4l2_tuner` ``capability`` field has the
	``V4L2_TUNER_CAP_LOW`` flag set, in units of 62.5 Hz or if the
	struct :c:type:`v4l2_tuner` ``capability`` field has
	the ``V4L2_TUNER_CAP_1HZ`` flag set, in units of 1 Hz. If
	``rangehigh`` is zero a reasonable default value is used.
        (若非零，为要搜索的频段中最高的可调谐频率，单位为 62.5 kHz；若 struct :c:type:`v4l2_tuner` 的 ``capability`` 字段设置了 ``V4L2_TUNER_CAP_LOW`` 标志，则单位为 62.5 Hz；若该字段设置了 ``V4L2_TUNER_CAP_1HZ`` 标志，则单位为 1 Hz。若 ``rangehigh`` 为零，则使用合理的默认值。)
    * - __u32
      - ``reserved``\ [5]
      - Reserved for future extensions. Applications must set the array to
	zero.
        (保留用于未来的扩展。应用程序必须把该数组置零。)

Return Value(返回值)
====================

On success 0 is returned, on error -1 and the ``errno`` variable is set
appropriately. The generic error codes are described at the
:ref:`Generic Error Codes <gen-errors>` chapter.
(成功时返回 0，出错时返回 -1 并适当设置 ``errno`` 变量。通用错误码在 :ref:`Generic Error Codes <gen-errors>` 一章中描述。)

EINVAL
    The ``tuner`` index is out of bounds, the ``wrap_around`` value is
    not supported or one of the values in the ``type``, ``rangelow`` or
    ``rangehigh`` fields is wrong.
    (``tuner`` 索引越界、``wrap_around`` 值不受支持，或 ``type`` 、 ``rangelow`` 或 ``rangehigh`` 字段中的某个值有误。)

EAGAIN
    Attempted to call ``VIDIOC_S_HW_FREQ_SEEK`` with the filehandle in
    non-blocking mode.
    (在非阻塞模式下对 ``VIDIOC_S_HW_FREQ_SEEK`` 的调用尝试。)

ENODATA
    The hardware seek found no channels.
    (硬件搜索未找到任何频道。)

EBUSY
    Another hardware seek is already in progress.
    (另一个硬件搜索已在进行中。)
