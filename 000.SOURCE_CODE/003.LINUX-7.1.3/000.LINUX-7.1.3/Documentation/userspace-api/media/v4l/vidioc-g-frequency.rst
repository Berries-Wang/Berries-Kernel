.. SPDX-License-Identifier: GFDL-1.1-no-invariants-or-later
.. c:namespace:: V4L

.. _VIDIOC_G_FREQUENCY:

********************************************************************************
ioctl VIDIOC_G_FREQUENCY, VIDIOC_S_FREQUENCY(获取或设置调谐器或调制器的射频频率)
********************************************************************************

Name(名称)
==========

VIDIOC_G_FREQUENCY - VIDIOC_S_FREQUENCY - Get or set tuner or modulator radio frequency(VIDIOC_G_FREQUENCY - VIDIOC_S_FREQUENCY - 获取或设置调谐器或调制器的射频频率)

Synopsis(概要)
==============

.. c:macro:: VIDIOC_G_FREQUENCY

``int ioctl(int fd, VIDIOC_G_FREQUENCY, struct v4l2_frequency *argp)``

.. c:macro:: VIDIOC_S_FREQUENCY

``int ioctl(int fd, VIDIOC_S_FREQUENCY, const struct v4l2_frequency *argp)``

Arguments(参数)
===============

``fd``
    File descriptor returned by :c:func:`open()`.
    (:c:func:`open()` 返回的文件描述符。)

``argp``
    Pointer to struct :c:type:`v4l2_frequency`.
    (指向 struct :c:type:`v4l2_frequency` 的指针。)

Description(描述)
=================

To get the current tuner or modulator radio frequency applications set
the ``tuner`` field of a struct
:c:type:`v4l2_frequency` to the respective tuner or
modulator number (only input devices have tuners, only output devices
have modulators), zero out the ``reserved`` array and call the
:ref:`VIDIOC_G_FREQUENCY <VIDIOC_G_FREQUENCY>` ioctl with a pointer to this structure. The
driver stores the current frequency in the ``frequency`` field.
(要获取当前调谐器或调制器的射频频率，应用程序把 struct
:c:type:`v4l2_frequency` 的 ``tuner`` 字段设置为相应的调谐器或调制器
编号(只有输入设备有调谐器，只有输出设备有调制器)，把 ``reserved``
数组清零，然后以指向该结构体的指针调用 :ref:`VIDIOC_G_FREQUENCY
<VIDIOC_G_FREQUENCY>` ioctl。驱动程序把当前频率存储在 ``frequency``
字段中。)

To change the current tuner or modulator radio frequency applications
initialize the ``tuner``, ``type`` and ``frequency`` fields, and the
``reserved`` array of a struct :c:type:`v4l2_frequency`
and call the :ref:`VIDIOC_S_FREQUENCY <VIDIOC_G_FREQUENCY>` ioctl with a pointer to this
structure. When the requested frequency is not possible the driver
assumes the closest possible value. However :ref:`VIDIOC_S_FREQUENCY <VIDIOC_G_FREQUENCY>` is a
write-only ioctl, it does not return the actual new frequency.
(要更改当前调谐器或调制器的射频频率，应用程序初始化 struct
:c:type:`v4l2_frequency` 的 ``tuner``、``type`` 和 ``frequency`` 字段
以及 ``reserved`` 数组，然后以指向该结构体的指针调用 :ref:`VIDIOC_S_FREQUENCY
<VIDIOC_G_FREQUENCY>` ioctl。当请求的频率不可行时，驱动程序会采用
最接近的可能值。但 :ref:`VIDIOC_S_FREQUENCY <VIDIOC_G_FREQUENCY>` 是只写
ioctl，不会返回实际的新频率。)

.. tabularcolumns:: |p{4.4cm}|p{4.4cm}|p{8.5cm}|

.. c:type:: v4l2_frequency

.. flat-table:: struct v4l2_frequency
    :header-rows:  0
    :stub-columns: 0
    :widths:       1 1 2

    * - __u32
      - ``tuner``
      - The tuner or modulator index number. This is the same value as in
	the struct :c:type:`v4l2_input` ``tuner`` field and
	the struct :c:type:`v4l2_tuner` ``index`` field, or
	the struct :c:type:`v4l2_output` ``modulator`` field
	and the struct :c:type:`v4l2_modulator` ``index``
	field.
        (调谐器或调制器的索引号。它与 struct :c:type:`v4l2_input` 的
        ``tuner`` 字段和 struct :c:type:`v4l2_tuner` 的 ``index`` 字段，
        或 struct :c:type:`v4l2_output` 的 ``modulator`` 字段和 struct
        :c:type:`v4l2_modulator` 的 ``index`` 字段中的值相同。)
    * - __u32
      - ``type``
      - The tuner type. This is the same value as in the struct
	:c:type:`v4l2_tuner` ``type`` field. The type must be
	set to ``V4L2_TUNER_RADIO`` for ``/dev/radioX`` device nodes, and
	to ``V4L2_TUNER_ANALOG_TV`` for all others. Set this field to
	``V4L2_TUNER_RADIO`` for modulators (currently only radio
	modulators are supported). See :c:type:`v4l2_tuner_type`
        (调谐器类型。它与 struct :c:type:`v4l2_tuner` 的 ``type`` 字段
        中的值相同。对 ``/dev/radioX`` 设备节点，类型必须设为
        ``V4L2_TUNER_RADIO``；对所有其他设备，设为
        ``V4L2_TUNER_ANALOG_TV``。对调制器，此字段设为
        ``V4L2_TUNER_RADIO`` (目前仅支持 radio 调制器)。参见
        :c:type:`v4l2_tuner_type`。)
    * - __u32
      - ``frequency``
      - Tuning frequency in units of 62.5 kHz, or if the struct
	:c:type:`v4l2_tuner` or struct
	:c:type:`v4l2_modulator` ``capability`` flag
	``V4L2_TUNER_CAP_LOW`` is set, in units of 62.5 Hz. A 1 Hz unit is
	used when the ``capability`` flag ``V4L2_TUNER_CAP_1HZ`` is set.
        (调谐频率，单位为 62.5 kHz；如果设置了 struct
        :c:type:`v4l2_tuner` 或 struct :c:type:`v4l2_modulator` 的
        ``capability`` 标志 ``V4L2_TUNER_CAP_LOW``，则单位为 62.5 Hz。
        当设置了 ``capability`` 标志 ``V4L2_TUNER_CAP_1HZ`` 时，使用
        1 Hz 单位。)
    * - __u32
      - ``reserved``\ [8]
      - Reserved for future extensions. Drivers and applications must set
	the array to zero.
        (保留用于未来的扩展。驱动程序和应用程序必须将此数组置零。)

Return Value(返回值)
====================

On success 0 is returned, on error -1 and the ``errno`` variable is set
appropriately. The generic error codes are described at the
:ref:`Generic Error Codes <gen-errors>` chapter.
(成功时返回 0，出错时返回 -1 并适当设置 ``errno`` 变量。通用错误码在
:ref:`Generic Error Codes <gen-errors>` 一章中描述。)

EINVAL
    The ``tuner`` index is out of bounds or the value in the ``type``
    field is wrong.
    (``tuner`` 索引越界，或 ``type`` 字段中的值不对。)

EBUSY
    A hardware seek is in progress.
    (硬件搜索正在进行中。)
