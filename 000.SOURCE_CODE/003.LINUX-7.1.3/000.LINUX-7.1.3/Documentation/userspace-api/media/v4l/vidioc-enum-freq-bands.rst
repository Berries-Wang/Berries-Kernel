.. SPDX-License-Identifier: GFDL-1.1-no-invariants-or-later
.. c:namespace:: V4L

.. _VIDIOC_ENUM_FREQ_BANDS:

********************************************
ioctl VIDIOC_ENUM_FREQ_BANDS(枚举支持的频段)
********************************************

Name(名称)
==========

VIDIOC_ENUM_FREQ_BANDS - Enumerate supported frequency bands
(VIDIOC_ENUM_FREQ_BANDS - 枚举支持的频段)

Synopsis(概要)
==============

.. c:macro:: VIDIOC_ENUM_FREQ_BANDS

``int ioctl(int fd, VIDIOC_ENUM_FREQ_BANDS, struct v4l2_frequency_band *argp)``

Arguments(参数)
===============

``fd``
    File descriptor returned by :c:func:`open()`.
    (由 :c:func:`open()` 返回的文件描述符。)

``argp``
    Pointer to struct :c:type:`v4l2_frequency_band`.
    (指向 struct :c:type:`v4l2_frequency_band` 的指针。)

Description(描述)
=================

Enumerates the frequency bands that a tuner or modulator supports. To do
this applications initialize the ``tuner``, ``type`` and ``index``
fields, and zero out the ``reserved`` array of a struct
:c:type:`v4l2_frequency_band` and call the
:ref:`VIDIOC_ENUM_FREQ_BANDS` ioctl with a pointer to this structure.
(枚举调谐器或调制器支持的频段。为此，应用程序初始化 struct
:c:type:`v4l2_frequency_band` 的 ``tuner``、``type`` 和 ``index`` 字段，
把 ``reserved`` 数组清零，并用指向该结构的指针调用
:ref:`VIDIOC_ENUM_FREQ_BANDS` ioctl。)

This ioctl is supported if the ``V4L2_TUNER_CAP_FREQ_BANDS`` capability
of the corresponding tuner/modulator is set.
(如果相应调谐器/调制器的 ``V4L2_TUNER_CAP_FREQ_BANDS`` 能力被设置，则
支持此 ioctl。)

.. tabularcolumns:: |p{2.9cm}|p{2.9cm}|p{5.8cm}|p{2.9cm}|p{2.4cm}|

.. c:type:: v4l2_frequency_band

.. flat-table:: struct v4l2_frequency_band
    :header-rows:  0
    :stub-columns: 0
    :widths:       1 1 2 1 1

    * - __u32
      - ``tuner``
      - The tuner or modulator index number. This is the same value as in
	the struct :c:type:`v4l2_input` ``tuner`` field and
	the struct :c:type:`v4l2_tuner` ``index`` field, or
	the struct :c:type:`v4l2_output` ``modulator`` field
	and the struct :c:type:`v4l2_modulator` ``index``
	field.
	(调谐器或调制器的索引号。它与 struct :c:type:`v4l2_input` 的
	``tuner`` 字段和 struct :c:type:`v4l2_tuner` 的 ``index`` 字段中的
	值相同，或者与 struct :c:type:`v4l2_output` 的 ``modulator`` 字段和
	struct :c:type:`v4l2_modulator` 的 ``index`` 字段中的值相同。)
    * - __u32
      - ``type``
      - The tuner type. This is the same value as in the struct
	:c:type:`v4l2_tuner` ``type`` field. The type must be
	set to ``V4L2_TUNER_RADIO`` for ``/dev/radioX`` device nodes, and
	to ``V4L2_TUNER_ANALOG_TV`` for all others. Set this field to
	``V4L2_TUNER_RADIO`` for modulators (currently only radio
	modulators are supported). See :c:type:`v4l2_tuner_type`
	(调谐器类型。它与 struct :c:type:`v4l2_tuner` 的 ``type`` 字段中的
	值相同。对 ``/dev/radioX`` 设备节点，类型必须设置为
	``V4L2_TUNER_RADIO``；对所有其他设备则设置为
	``V4L2_TUNER_ANALOG_TV``。对调制器，此字段设置为
	``V4L2_TUNER_RADIO``（目前只支持无线电调制器）。参见
	:c:type:`v4l2_tuner_type`)
    * - __u32
      - ``index``
      - Identifies the frequency band, set by the application.
	(标识频段，由应用程序设置。)
    * - __u32
      - ``capability``
      - :cspan:`2` The tuner/modulator capability flags for this
	frequency band, see :ref:`tuner-capability`. The
	``V4L2_TUNER_CAP_LOW`` or ``V4L2_TUNER_CAP_1HZ`` capability must
	be the same for all frequency bands of the selected
	tuner/modulator. So either all bands have that capability set, or
	none of them have that capability.
	(:cspan:`2` 此频段的调谐器/调制器能力标志，参见
	:ref:`tuner-capability`。所选调谐器/调制器的所有频段的
	``V4L2_TUNER_CAP_LOW`` 或 ``V4L2_TUNER_CAP_1HZ`` 能力必须相同。
	也就是说，要么所有频段都设置了该能力，要么都没有设置。)
    * - __u32
      - ``rangelow``
      - :cspan:`2` The lowest tunable frequency in units of 62.5 kHz, or
	if the ``capability`` flag ``V4L2_TUNER_CAP_LOW`` is set, in units
	of 62.5 Hz, for this frequency band. A 1 Hz unit is used when the
	``capability`` flag ``V4L2_TUNER_CAP_1HZ`` is set.
	(:cspan:`2` 此频段最低可调谐的频率，单位为 62.5 kHz；如果设置了
	``capability`` 标志 ``V4L2_TUNER_CAP_LOW``，则单位为 62.5 Hz。当
	设置了 ``capability`` 标志 ``V4L2_TUNER_CAP_1HZ`` 时，使用 1 Hz
	单位。)
    * - __u32
      - ``rangehigh``
      - :cspan:`2` The highest tunable frequency in units of 62.5 kHz,
	or if the ``capability`` flag ``V4L2_TUNER_CAP_LOW`` is set, in
	units of 62.5 Hz, for this frequency band. A 1 Hz unit is used
	when the ``capability`` flag ``V4L2_TUNER_CAP_1HZ`` is set.
	(:cspan:`2` 此频段最高可调谐的频率，单位为 62.5 kHz；如果设置了
	``capability`` 标志 ``V4L2_TUNER_CAP_LOW``，则单位为 62.5 Hz。当
	设置了 ``capability`` 标志 ``V4L2_TUNER_CAP_1HZ`` 时，使用 1 Hz
	单位。)
    * - __u32
      - ``modulation``
      - :cspan:`2` The supported modulation systems of this frequency
	band. See :ref:`band-modulation`.
	(:cspan:`2` 此频段支持的调制系统。参见 :ref:`band-modulation`。)

	.. note::

	   Currently only one modulation system per frequency band
	   is supported. More work will need to be done if multiple
	   modulation systems are possible. Contact the linux-media
	   mailing list
	   (`https://linuxtv.org/lists.php <https://linuxtv.org/lists.php>`__)
	   if you need such functionality.
	   (注意：目前每个频段只支持一种调制系统。如果可能支持多种调制
	   系统，还需要做更多工作。如果需要这样的功能，请联系 linux-media
	   邮件列表 (`https://linuxtv.org/lists.php
	   <https://linuxtv.org/lists.php>`__) 。)
    * - __u32
      - ``reserved``\ [9]
      - Reserved for future extensions.

	Applications and drivers must set the array to zero.
	(为未来扩展保留。

	应用程序和驱动必须将数组置零。)


.. tabularcolumns:: |p{6.6cm}|p{2.2cm}|p{8.5cm}|

.. _band-modulation:

.. flat-table:: Band Modulation Systems
    :header-rows:  0
    :stub-columns: 0
    :widths:       3 1 4

    * - ``V4L2_BAND_MODULATION_VSB``
      - 0x02
      - Vestigial Sideband modulation, used for analog TV.
	(残留边带 (Vestigial Sideband) 调制，用于模拟电视。)
    * - ``V4L2_BAND_MODULATION_FM``
      - 0x04
      - Frequency Modulation, commonly used for analog radio.
	(频率调制 (Frequency Modulation)，常用于模拟收音机。)
    * - ``V4L2_BAND_MODULATION_AM``
      - 0x08
      - Amplitude Modulation, commonly used for analog radio.
	(幅度调制 (Amplitude Modulation)，常用于模拟收音机。)

Return Value(返回值)
====================

On success 0 is returned, on error -1 and the ``errno`` variable is set
appropriately. The generic error codes are described at the
:ref:`Generic Error Codes <gen-errors>` chapter.
(成功时返回 0，出错时返回 -1 并适当设置 ``errno`` 变量。通用错误码在
:ref:`Generic Error Codes <gen-errors>` 一章中描述。)

EINVAL
    The ``tuner`` or ``index`` is out of bounds or the ``type`` field is
    wrong.
    (``tuner`` 或 ``index`` 越界，或 ``type`` 字段错误。)
