.. SPDX-License-Identifier: GFDL-1.1-no-invariants-or-later
.. c:namespace:: V4L

.. _VIDIOC_G_MODULATOR:

******************************************************************
ioctl VIDIOC_G_MODULATOR, VIDIOC_S_MODULATOR(获取或设置调制器属性)
******************************************************************

Name(名称)
==========

VIDIOC_G_MODULATOR - VIDIOC_S_MODULATOR - Get or set modulator attributes
(VIDIOC_G_MODULATOR - VIDIOC_S_MODULATOR - 获取或设置调制器属性)

Synopsis(概要)
==============

.. c:macro:: VIDIOC_G_MODULATOR

``int ioctl(int fd, VIDIOC_G_MODULATOR, struct v4l2_modulator *argp)``

.. c:macro:: VIDIOC_S_MODULATOR

``int ioctl(int fd, VIDIOC_S_MODULATOR, const struct v4l2_modulator *argp)``

Arguments(参数)
===============

``fd``
    File descriptor returned by :c:func:`open()`.
    (由 :c:func:`open()` 返回的文件描述符。)

``argp``
    Pointer to struct :c:type:`v4l2_modulator`.
    (指向结构体 :c:type:`v4l2_modulator` 的指针。)

Description(描述)
=================

To query the attributes of a modulator applications initialize the
``index`` field and zero out the ``reserved`` array of a struct
:c:type:`v4l2_modulator` and call the
:ref:`VIDIOC_G_MODULATOR <VIDIOC_G_MODULATOR>` ioctl with a pointer to this structure. Drivers
fill the rest of the structure or return an ``EINVAL`` error code when the
index is out of bounds. To enumerate all modulators applications shall
begin at index zero, incrementing by one until the driver returns
EINVAL.
(要查询调制器的属性，应用需初始化结构体 :c:type:`v4l2_modulator` 的 ``index`` 字段并将 ``reserved`` 数组清零，然后以指向该结构的指针调用 :ref:`VIDIOC_G_MODULATOR <VIDIOC_G_MODULATOR>` ioctl。驱动会填充结构的其余字段；若索引越界则返回 ``EINVAL`` 错误码。要枚举所有调制器，应用应从索引零开始，每次加一，直到驱动返回 EINVAL。)

Modulators have two writable properties, an audio modulation set and the
radio frequency. To change the modulated audio subprograms, applications
initialize the ``index`` and ``txsubchans`` fields and the ``reserved``
array and call the :ref:`VIDIOC_S_MODULATOR <VIDIOC_G_MODULATOR>` ioctl. Drivers may choose a
different audio modulation if the request cannot be satisfied. However
this is a write-only ioctl, it does not return the actual audio
modulation selected.
(调制器有两个可写属性：音频调制集合与无线电频率。要更改被调制的音频子节目，应用需初始化 ``index`` 与 ``txsubchans`` 字段以及 ``reserved`` 数组，然后调用 :ref:`VIDIOC_S_MODULATOR <VIDIOC_G_MODULATOR>` ioctl。如果请求无法满足，驱动可以选择不同的音频调制。不过这是一个只写 ioctl，它不会返回实际选择的音频调制。)

:ref:`SDR <sdr>` specific modulator types are ``V4L2_TUNER_SDR`` and
``V4L2_TUNER_RF``. For SDR devices ``txsubchans`` field must be
initialized to zero. The term 'modulator' means SDR transmitter in this
context.
(:ref:`SDR <sdr>` 特有的调制器类型是 ``V4L2_TUNER_SDR`` 与 ``V4L2_TUNER_RF``。对 SDR 设备，``txsubchans`` 字段必须初始化为零。在此上下文中，术语 'modulator' 指 SDR 发射机。)

To change the radio frequency the
:ref:`VIDIOC_S_FREQUENCY <VIDIOC_G_FREQUENCY>` ioctl is available.
(要更改无线电频率，可使用 :ref:`VIDIOC_S_FREQUENCY <VIDIOC_G_FREQUENCY>` ioctl。)

.. tabularcolumns:: |p{2.9cm}|p{2.9cm}|p{5.8cm}|p{2.9cm}|p{2.4cm}|

.. c:type:: v4l2_modulator

.. flat-table:: struct v4l2_modulator
    :header-rows:  0
    :stub-columns: 0
    :widths:       1 1 2 1 1

    * - __u32
      - ``index``
      - Identifies the modulator, set by the application.
        (标识调制器，由应用设置。)
    * - __u8
      - ``name``\ [32]
      - Name of the modulator, a NUL-terminated ASCII string.

	This information is intended for the user.
        (调制器的名称，以 NUL 结尾的 ASCII 字符串。

        该信息面向用户。)
    * - __u32
      - ``capability``
      - Modulator capability flags. No flags are defined for this field,
	the tuner flags in struct :c:type:`v4l2_tuner` are
	used accordingly. The audio flags indicate the ability to encode
	audio subprograms. They will *not* change for example with the
	current video standard.
        (调制器能力标志。该字段未定义专用标志，相应地使用结构体 :c:type:`v4l2_tuner` 中的调谐器标志。音频标志表示编码音频子节目的能力。例如它们*不会*随当前视频标准而改变。)
    * - __u32
      - ``rangelow``
      - The lowest tunable frequency in units of 62.5 KHz, or if the
	``capability`` flag ``V4L2_TUNER_CAP_LOW`` is set, in units of
	62.5 Hz, or if the ``capability`` flag ``V4L2_TUNER_CAP_1HZ`` is
	set, in units of 1 Hz.
        (最低可调谐频率，单位为 62.5 KHz；若设置了 ``capability`` 标志 ``V4L2_TUNER_CAP_LOW``，单位为 62.5 Hz；若设置了 ``capability`` 标志 ``V4L2_TUNER_CAP_1HZ``，单位为 1 Hz。)
    * - __u32
      - ``rangehigh``
      - The highest tunable frequency in units of 62.5 KHz, or if the
	``capability`` flag ``V4L2_TUNER_CAP_LOW`` is set, in units of
	62.5 Hz, or if the ``capability`` flag ``V4L2_TUNER_CAP_1HZ`` is
	set, in units of 1 Hz.
        (最高可调谐频率，单位为 62.5 KHz；若设置了 ``capability`` 标志 ``V4L2_TUNER_CAP_LOW``，单位为 62.5 Hz；若设置了 ``capability`` 标志 ``V4L2_TUNER_CAP_1HZ``，单位为 1 Hz。)
    * - __u32
      - ``txsubchans``
      - With this field applications can determine how audio sub-carriers
	shall be modulated. It contains a set of flags as defined in
	:ref:`modulator-txsubchans`.

	.. note::

	   The tuner ``rxsubchans`` flags  are reused, but the
	   semantics are different. Video output devices
	   are assumed to have an analog or PCM audio input with 1-3
	   channels. The ``txsubchans`` flags select one or more channels
	   for modulation, together with some audio subprogram indicator,
	   for example, a stereo pilot tone.

        (应用通过该字段决定音频副载波应如何被调制。它包含一组标志，定义见 :ref:`modulator-txsubchans` 。

        .. note::

           调谐器的 ``rxsubchans`` 标志被复用，但语义不同。视频输出设备假定具有 1-3 个声道的模拟或 PCM 音频输入。``txsubchans`` 标志选择一个或多个声道进行调制，并连同某个音频子节目指示(例如立体声导频音)一起使用。)
    * - __u32
      - ``type``
      - :cspan:`2` Type of the modulator, see :c:type:`v4l2_tuner_type`.
        (调制器的类型，见 :c:type:`v4l2_tuner_type`。)
    * - __u32
      - ``reserved``\ [3]
      - Reserved for future extensions.

	Drivers and applications must set the array to zero.
        (保留用于未来扩展。

        驱动与应用必须将该数组置零。)

.. tabularcolumns:: |p{6.0cm}|p{2.0cm}|p{9.3cm}|

.. cssclass:: longtable

.. _modulator-txsubchans:

.. flat-table:: Modulator Audio Transmission Flags
    :header-rows:  0
    :stub-columns: 0
    :widths:       3 1 4

    * - ``V4L2_TUNER_SUB_MONO``
      - 0x0001
      - Modulate channel 1 as mono audio, when the input has more
	channels, a down-mix of channel 1 and 2. This flag does not
	combine with ``V4L2_TUNER_SUB_STEREO`` or
	``V4L2_TUNER_SUB_LANG1``.
        (将声道 1 调制为单声道音频；当输入有更多声道时，为声道 1 和 2 的下混。该标志不与 ``V4L2_TUNER_SUB_STEREO`` 或 ``V4L2_TUNER_SUB_LANG1`` 组合。)
    * - ``V4L2_TUNER_SUB_STEREO``
      - 0x0002
      - Modulate channel 1 and 2 as left and right channel of a stereo
	audio signal. When the input has only one channel or two channels
	and ``V4L2_TUNER_SUB_SAP`` is also set, channel 1 is encoded as
	left and right channel. This flag does not combine with
	``V4L2_TUNER_SUB_MONO`` or ``V4L2_TUNER_SUB_LANG1``. When the
	driver does not support stereo audio it shall fall back to mono.
        (将声道 1 和 2 调制为立体声音频信号的左右声道。当输入只有一个声道，或有两个声道且同时设置了 ``V4L2_TUNER_SUB_SAP`` 时，声道 1 被编码为左右声道。该标志不与 ``V4L2_TUNER_SUB_MONO`` 或 ``V4L2_TUNER_SUB_LANG1`` 组合。当驱动不支持立体声音频时，应回退到单声道。)
    * - ``V4L2_TUNER_SUB_LANG1``
      - 0x0008
      - Modulate channel 1 and 2 as primary and secondary language of a
	bilingual audio signal. When the input has only one channel it is
	used for both languages. It is not possible to encode the primary
	or secondary language only. This flag does not combine with
	``V4L2_TUNER_SUB_MONO``, ``V4L2_TUNER_SUB_STEREO`` or
	``V4L2_TUNER_SUB_SAP``. If the hardware does not support the
	respective audio matrix, or the current video standard does not
	permit bilingual audio the :ref:`VIDIOC_S_MODULATOR <VIDIOC_G_MODULATOR>` ioctl shall
	return an ``EINVAL`` error code and the driver shall fall back to mono
	or stereo mode.
        (将声道 1 和 2 调制为双语音频信号的主语言和副语言。当输入只有一个声道时，两个语言都用它。不能只编码主语言或只编码副语言。该标志不与 ``V4L2_TUNER_SUB_MONO``、``V4L2_TUNER_SUB_STEREO`` 或 ``V4L2_TUNER_SUB_SAP`` 组合。如果硬件不支持相应的音频矩阵，或当前视频标准不允许双语音频，:ref:`VIDIOC_S_MODULATOR <VIDIOC_G_MODULATOR>` ioctl 应返回 ``EINVAL`` 错误码，驱动应回退到单声道或立体声模式。)
    * - ``V4L2_TUNER_SUB_LANG2``
      - 0x0004
      - Same effect as ``V4L2_TUNER_SUB_SAP``.
        (效果与 ``V4L2_TUNER_SUB_SAP`` 相同。)
    * - ``V4L2_TUNER_SUB_SAP``
      - 0x0004
      - When combined with ``V4L2_TUNER_SUB_MONO`` the first channel is
	encoded as mono audio, the last channel as Second Audio Program.
	When the input has only one channel it is used for both audio
	tracks. When the input has three channels the mono track is a
	down-mix of channel 1 and 2. When combined with
	``V4L2_TUNER_SUB_STEREO`` channel 1 and 2 are encoded as left and
	right stereo audio, channel 3 as Second Audio Program. When the
	input has only two channels, the first is encoded as left and
	right channel and the second as SAP. When the input has only one
	channel it is used for all audio tracks. It is not possible to
	encode a Second Audio Program only. This flag must combine with
	``V4L2_TUNER_SUB_MONO`` or ``V4L2_TUNER_SUB_STEREO``. If the
	hardware does not support the respective audio matrix, or the
	current video standard does not permit SAP the
	:ref:`VIDIOC_S_MODULATOR <VIDIOC_G_MODULATOR>` ioctl shall return an ``EINVAL`` error code and
	driver shall fall back to mono or stereo mode.
        (与 ``V4L2_TUNER_SUB_MONO`` 组合时，第一个声道被编码为单声道音频，最后一个声道被编码为第二音频节目 (Second Audio Program)。当输入只有一个声道时，两路音频都用它。当输入有三个声道时，单声道轨是声道 1 和 2 的下混。与 ``V4L2_TUNER_SUB_STEREO`` 组合时，声道 1 和 2 被编码为左右立体声音频，声道 3 作为第二音频节目。当输入只有两个声道时，第一个被编码为左右声道，第二个作为 SAP。当输入只有一个声道时，它用于所有音频轨。不能只编码第二音频节目。该标志必须与 ``V4L2_TUNER_SUB_MONO`` 或 ``V4L2_TUNER_SUB_STEREO`` 组合。如果硬件不支持相应的音频矩阵，或当前视频标准不允许 SAP，:ref:`VIDIOC_S_MODULATOR <VIDIOC_G_MODULATOR>` ioctl 应返回 ``EINVAL`` 错误码，驱动应回退到单声道或立体声模式。)
    * - ``V4L2_TUNER_SUB_RDS``
      - 0x0010
      - Enable the RDS encoder for a radio FM transmitter.
        (为调频 (FM) 发射机启用 RDS 编码器。)

Return Value(返回值)
====================

On success 0 is returned, on error -1 and the ``errno`` variable is set
appropriately. The generic error codes are described at the
:ref:`Generic Error Codes <gen-errors>` chapter.
(成功时返回 0，出错时返回 -1 并将 ``errno`` 变量设置为相应值。通用错误码在 :ref:`Generic Error Codes <gen-errors>` 一章中描述。)

EINVAL
    The struct :c:type:`v4l2_modulator` ``index`` is
    out of bounds.
    (结构体 :c:type:`v4l2_modulator` 的 ``index`` 越界。)
