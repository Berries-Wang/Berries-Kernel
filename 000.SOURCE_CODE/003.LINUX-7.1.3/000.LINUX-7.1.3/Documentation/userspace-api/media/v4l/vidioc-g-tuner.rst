.. SPDX-License-Identifier: GFDL-1.1-no-invariants-or-later
.. c:namespace:: V4L

.. _VIDIOC_G_TUNER:

**********************************************************
ioctl VIDIOC_G_TUNER, VIDIOC_S_TUNER(获取或设置调谐器属性)
**********************************************************

Name(名称)
==========

VIDIOC_G_TUNER - VIDIOC_S_TUNER - Get or set tuner attributes
(VIDIOC_G_TUNER - VIDIOC_S_TUNER - 获取或设置调谐器属性)

Synopsis(概要)
==============

.. c:macro:: VIDIOC_G_TUNER

``int ioctl(int fd, VIDIOC_G_TUNER, struct v4l2_tuner *argp)``

.. c:macro:: VIDIOC_S_TUNER

``int ioctl(int fd, VIDIOC_S_TUNER, const struct v4l2_tuner *argp)``

Arguments(参数)
===============

``fd``
    File descriptor returned by :c:func:`open()`.
    (由 :c:func:`open()` 返回的文件描述符。)

``argp``
    Pointer to struct :c:type:`v4l2_tuner`.
    (指向 struct :c:type:`v4l2_tuner` 的指针。)

Description(描述)
=================

To query the attributes of a tuner applications initialize the ``index``
field and zero out the ``reserved`` array of a struct
:c:type:`v4l2_tuner` and call the ``VIDIOC_G_TUNER`` ioctl
with a pointer to this structure. Drivers fill the rest of the structure
or return an ``EINVAL`` error code when the index is out of bounds. To
enumerate all tuners applications shall begin at index zero,
incrementing by one until the driver returns ``EINVAL``.
(要查询调谐器的属性，应用程序先初始化 struct :c:type:`v4l2_tuner` 的 ``index`` 字段并把 ``reserved`` 数组清零，然后以指向该结构的指针调用 ``VIDIOC_G_TUNER`` ioctl。驱动会填充结构的其余部分；当 index 越界时返回 ``EINVAL`` 错误码。要枚举所有调谐器，应用程序应从 index 0 开始，每次加 1，直到驱动返回 ``EINVAL``。)

Tuners have two writable properties, the audio mode and the radio
frequency. To change the audio mode, applications initialize the
``index``, ``audmode`` and ``reserved`` fields and call the
``VIDIOC_S_TUNER`` ioctl. This will *not* change the current tuner,
which is determined by the current video input. Drivers may choose a
different audio mode if the requested mode is invalid or unsupported.
Since this is a write-only ioctl, it does not return the actually
selected audio mode.
(调谐器有两个可写属性：音频模式和无线电频率。要更改音频模式，应用程序初始化 ``index``、``audmode`` 和 ``reserved`` 字段，并调用 ``VIDIOC_S_TUNER`` ioctl。这并不会改变当前调谐器——当前调谐器由当前视频输入决定。如果请求的模式无效或不受支持，驱动可以选择另一种音频模式。由于这是一个只写 ioctl，它不会返回实际选择的音频模式。)

:ref:`SDR <sdr>` specific tuner types are ``V4L2_TUNER_SDR`` and
``V4L2_TUNER_RF``. For SDR devices ``audmode`` field must be initialized
to zero. The term 'tuner' means SDR receiver in this context.
(:ref:`SDR <sdr>` 特有的调谐器类型是 ``V4L2_TUNER_SDR`` 与 ``V4L2_TUNER_RF``。对 SDR 设备而言，``audmode`` 字段必须初始化为 0。在此语境中，术语 'tuner' 指 SDR 接收器。)

To change the radio frequency the
:ref:`VIDIOC_S_FREQUENCY <VIDIOC_G_FREQUENCY>` ioctl is available.
(要更改无线电频率，可使用 :ref:`VIDIOC_S_FREQUENCY <VIDIOC_G_FREQUENCY>` ioctl。)

 .. tabularcolumns:: |p{1.3cm}|p{3.0cm}|p{7.0cm}|p{5.8cm}|

.. c:type:: v4l2_tuner

.. cssclass:: longtable

.. flat-table:: struct v4l2_tuner
    :header-rows:  0
    :stub-columns: 0

    * - __u32
      - ``index``
      - :cspan:`1` Identifies the tuner, set by the application.(标识调谐器，由应用程序设置。)
    * - __u8
      - ``name``\ [32]
      - :cspan:`1`

	Name of the tuner, a NUL-terminated ASCII string.
	(调谐器的名称，一个以 NUL 结尾的 ASCII 字符串。)

	This information is intended for the user.
	(该信息面向最终用户。)
    * - __u32
      - ``type``
      - :cspan:`1` Type of the tuner, see :c:type:`v4l2_tuner_type`.(调谐器类型，见 :c:type:`v4l2_tuner_type`。)
    * - __u32
      - ``capability``
      - :cspan:`1`

	Tuner capability flags, see :ref:`tuner-capability`. Audio flags
	indicate the ability to decode audio subprograms. They will *not*
	change, for example with the current video standard.
	(调谐器能力标志，见 :ref:`tuner-capability`。音频标志指示解码音频子节目的能力。例如，它们并不会随当前视频标准而改变。)

	When the structure refers to a radio tuner the
	``V4L2_TUNER_CAP_LANG1``, ``V4L2_TUNER_CAP_LANG2`` and
	``V4L2_TUNER_CAP_NORM`` flags can't be used.
	(当该结构描述的是无线电调谐器时，不能使用 ``V4L2_TUNER_CAP_LANG1``、``V4L2_TUNER_CAP_LANG2`` 与 ``V4L2_TUNER_CAP_NORM`` 标志。)

	If multiple frequency bands are supported, then ``capability`` is
	the union of all ``capability`` fields of each struct
	:c:type:`v4l2_frequency_band`.
	(如果支持多个频段，``capability`` 就是每个 struct :c:type:`v4l2_frequency_band` 的所有 ``capability`` 字段的并集。)
    * - __u32
      - ``rangelow``
      - :cspan:`1` The lowest tunable frequency in units of 62.5 kHz, or
	if the ``capability`` flag ``V4L2_TUNER_CAP_LOW`` is set, in units
	of 62.5 Hz, or if the ``capability`` flag ``V4L2_TUNER_CAP_1HZ``
	is set, in units of 1 Hz. If multiple frequency bands are
	supported, then ``rangelow`` is the lowest frequency of all the
	frequency bands.
	(以 62.5 kHz 为单位的最低可调频率；若设置了 ``capability`` 标志 ``V4L2_TUNER_CAP_LOW``，则以 62.5 Hz 为单位；若设置了 ``capability`` 标志 ``V4L2_TUNER_CAP_1HZ``，则以 1 Hz 为单位。如果支持多个频段，``rangelow`` 是所有频段中的最低频率。)
    * - __u32
      - ``rangehigh``
      - :cspan:`1` The highest tunable frequency in units of 62.5 kHz,
	or if the ``capability`` flag ``V4L2_TUNER_CAP_LOW`` is set, in
	units of 62.5 Hz, or if the ``capability`` flag
	``V4L2_TUNER_CAP_1HZ`` is set, in units of 1 Hz. If multiple
	frequency bands are supported, then ``rangehigh`` is the highest
	frequency of all the frequency bands.
	(以 62.5 kHz 为单位的最高可调频率；若设置了 ``capability`` 标志 ``V4L2_TUNER_CAP_LOW``，则以 62.5 Hz 为单位；若设置了 ``capability`` 标志 ``V4L2_TUNER_CAP_1HZ``，则以 1 Hz 为单位。如果支持多个频段，``rangehigh`` 是所有频段中的最高频率。)
    * - __u32
      - ``rxsubchans``
      - :cspan:`1`

	Some tuners or audio decoders can determine the received audio
	subprograms by analyzing audio carriers, pilot tones or other
	indicators. To pass this information drivers set flags defined in
	:ref:`tuner-rxsubchans` in this field. For example:
	(某些调谐器或音频解码器可以通过分析音频载波、导频音或其他指示来判断接收到的音频子节目。为传递这一信息，驱动在本字段中设置 :ref:`tuner-rxsubchans` 定义的标志。例如：)
    * -
      -
      - ``V4L2_TUNER_SUB_MONO``
      - receiving mono audio(接收单声道音频)
    * -
      -
      - ``STEREO | SAP``
      - receiving stereo audio and a secondary audio program(接收立体声音频及一个第二音频节目)
    * -
      -
      - ``MONO | STEREO``
      - receiving mono or stereo audio, the hardware cannot distinguish(接收单声道或立体声音频，硬件无法区分)
    * -
      -
      - ``LANG1 | LANG2``
      - receiving bilingual audio(接收双语音频)
    * -
      -
      - ``MONO | STEREO | LANG1 | LANG2``
      - receiving mono, stereo or bilingual audio(接收单声道、立体声或双语音频)
    * -
      -
      - :cspan:`1`

	When the ``V4L2_TUNER_CAP_STEREO``, ``_LANG1``, ``_LANG2`` or
	``_SAP`` flag is cleared in the ``capability`` field, the
	corresponding ``V4L2_TUNER_SUB_`` flag must not be set here.
	(当 ``capability`` 字段中的 ``V4L2_TUNER_CAP_STEREO``、``_LANG1``、``_LANG2`` 或 ``_SAP`` 标志被清除时，此处不得设置对应的 ``V4L2_TUNER_SUB_`` 标志。)

	This field is valid only if this is the tuner of the current video
	input, or when the structure refers to a radio tuner.
	(仅当这是当前视频输入的调谐器，或该结构描述的是无线电调谐器时，本字段才有效。)
    * - __u32
      - ``audmode``
      - :cspan:`1`

	The selected audio mode, see :ref:`tuner-audmode` for valid
	values. The audio mode does not affect audio subprogram detection,
	and like a :ref:`control` it does not automatically
	change unless the requested mode is invalid or unsupported. See
	:ref:`tuner-matrix` for possible results when the selected and
	received audio programs do not match.
	(所选的音频模式，有效取值见 :ref:`tuner-audmode`。音频模式不影响音频子节目的检测，并且与 :ref:`control` 一样，除非请求的模式无效或不受支持，否则它不会自动改变。当选择的音频节目与接收到的音频节目不匹配时，可能出现的结果见 :ref:`tuner-matrix`。)

	Currently this is the only field of struct
	struct :c:type:`v4l2_tuner` applications can change.
	(目前这是 struct struct :c:type:`v4l2_tuner` 中应用程序唯一能更改的字段。)
    * - __u32
      - ``signal``
      - :cspan:`1` The signal strength if known.
	(信号强度(若已知)。)

	Ranging from 0 to 65535. Higher values indicate a better signal.
	(范围从 0 到 65535。数值越高表示信号越好。)
    * - __s32
      - ``afc``
      - :cspan:`1` Automatic frequency control.
	(自动频率控制。)

	When the ``afc`` value is negative, the frequency is too
	low, when positive too high.
	(当 ``afc`` 值为负时，表示频率偏低；为正时表示频率偏高。)
    * - __u32
      - ``reserved``\ [4]
      - :cspan:`1` Reserved for future extensions.
	(为未来的扩展保留。)

	Drivers and applications must set the array to zero.
	(驱动和应用程序必须将该数组置零。)


.. tabularcolumns:: |p{6.6cm}|p{2.2cm}|p{8.5cm}|

.. c:type:: v4l2_tuner_type

.. flat-table:: enum v4l2_tuner_type
    :header-rows:  0
    :stub-columns: 0
    :widths:       3 1 6

    * - ``V4L2_TUNER_RADIO``
      - 1
      - Tuner supports radio(调谐器支持收音机)
    * - ``V4L2_TUNER_ANALOG_TV``
      - 2
      - Tuner supports analog TV(调谐器支持模拟电视)
    * - ``V4L2_TUNER_SDR``
      - 4
      - Tuner controls the A/D and/or D/A block of a
	Software Digital Radio (SDR)
	(调谐器控制软件数字收音机(SDR)的 A/D 和/或 D/A 模块)
    * - ``V4L2_TUNER_RF``
      - 5
      - Tuner controls the RF part of a Software Digital Radio (SDR)(调谐器控制软件数字收音机(SDR)的射频部分)

.. tabularcolumns:: |p{7.0cm}|p{2.2cm}|p{8.1cm}|

.. _tuner-capability:

.. cssclass:: longtable

.. flat-table:: Tuner and Modulator Capability Flags
    :header-rows:  0
    :stub-columns: 0
    :widths:       3 1 4

    * - ``V4L2_TUNER_CAP_LOW``
      - 0x0001
      - When set, tuning frequencies are expressed in units of 62.5 Hz
	instead of 62.5 kHz.
	(设置时，调谐频率以 62.5 Hz 而非 62.5 kHz 为单位表示。)
    * - ``V4L2_TUNER_CAP_NORM``
      - 0x0002
      - This is a multi-standard tuner; the video standard can or must be
	switched. (B/G PAL tuners for example are typically not considered
	multi-standard because the video standard is automatically
	determined from the frequency band.) The set of supported video
	standards is available from the struct
	:c:type:`v4l2_input` pointing to this tuner, see the
	description of ioctl :ref:`VIDIOC_ENUMINPUT`
	for details. Only ``V4L2_TUNER_ANALOG_TV`` tuners can have this
	capability.
	(这是一个多制式调谐器；视频标准可以或必须切换。(例如 B/G 制 PAL 调谐器通常不被视为多制式，因为视频标准会从频段自动确定。)支持的视频标准集合可从指向该调谐器的 struct :c:type:`v4l2_input` 获得，详见 ioctl :ref:`VIDIOC_ENUMINPUT` 的描述。只有 ``V4L2_TUNER_ANALOG_TV`` 调谐器可以具备此能力。)
    * - ``V4L2_TUNER_CAP_HWSEEK_BOUNDED``
      - 0x0004
      - If set, then this tuner supports the hardware seek functionality
	where the seek stops when it reaches the end of the frequency
	range.
	(若设置，该调谐器支持硬件搜索功能：搜索到达频率范围末端时停止。)
    * - ``V4L2_TUNER_CAP_HWSEEK_WRAP``
      - 0x0008
      - If set, then this tuner supports the hardware seek functionality
	where the seek wraps around when it reaches the end of the
	frequency range.
	(若设置，该调谐器支持硬件搜索功能：搜索到达频率范围末端时回绕继续。)
    * - ``V4L2_TUNER_CAP_STEREO``
      - 0x0010
      - Stereo audio reception is supported.(支持立体声音频接收。)
    * - ``V4L2_TUNER_CAP_LANG1``
      - 0x0040
      - Reception of the primary language of a bilingual audio program is
	supported. Bilingual audio is a feature of two-channel systems,
	transmitting the primary language monaural on the main audio
	carrier and a secondary language monaural on a second carrier.
	Only ``V4L2_TUNER_ANALOG_TV`` tuners can have this capability.
	(支持接收双语音频节目的主语言。双语音频是双信道系统的特性：主音频载波上以单声道传输主语言，第二个载波上以单声道传输副语言。只有 ``V4L2_TUNER_ANALOG_TV`` 调谐器可以具备此能力。)
    * - ``V4L2_TUNER_CAP_LANG2``
      - 0x0020
      - Reception of the secondary language of a bilingual audio program
	is supported. Only ``V4L2_TUNER_ANALOG_TV`` tuners can have this
	capability.
	(支持接收双语音频节目的副语言。只有 ``V4L2_TUNER_ANALOG_TV`` 调谐器可以具备此能力。)
    * - ``V4L2_TUNER_CAP_SAP``
      - 0x0020
      - Reception of a secondary audio program is supported. This is a
	feature of the BTSC system which accompanies the NTSC video
	standard. Two audio carriers are available for mono or stereo
	transmissions of a primary language, and an independent third
	carrier for a monaural secondary language. Only
	``V4L2_TUNER_ANALOG_TV`` tuners can have this capability.
	(支持接收第二音频节目。这是伴随 NTSC 视频标准的 BTSC 系统的特性：两个音频载波可用于主语言的单声道或立体声传输，第三个独立载波用于单声道的副语言。只有 ``V4L2_TUNER_ANALOG_TV`` 调谐器可以具备此能力。)

	.. note::

	   The ``V4L2_TUNER_CAP_LANG2`` and ``V4L2_TUNER_CAP_SAP``
	   flags are synonyms. ``V4L2_TUNER_CAP_SAP`` applies when the tuner
	   supports the ``V4L2_STD_NTSC_M`` video standard.
	   (``V4L2_TUNER_CAP_LANG2`` 与 ``V4L2_TUNER_CAP_SAP`` 是同义标志。当调谐器支持 ``V4L2_STD_NTSC_M`` 视频标准时适用 ``V4L2_TUNER_CAP_SAP``。)
    * - ``V4L2_TUNER_CAP_RDS``
      - 0x0080
      - RDS capture is supported. This capability is only valid for radio
	tuners.
	(支持 RDS 接收。该能力仅对收音机调谐器有效。)
    * - ``V4L2_TUNER_CAP_RDS_BLOCK_IO``
      - 0x0100
      - The RDS data is passed as unparsed RDS blocks.(RDS 数据以未解析的 RDS 块形式传递。)
    * - ``V4L2_TUNER_CAP_RDS_CONTROLS``
      - 0x0200
      - The RDS data is parsed by the hardware and set via controls.(RDS 数据由硬件解析，并通过控件设置。)
    * - ``V4L2_TUNER_CAP_FREQ_BANDS``
      - 0x0400
      - The :ref:`VIDIOC_ENUM_FREQ_BANDS`
	ioctl can be used to enumerate the available frequency bands.
	(可以使用 :ref:`VIDIOC_ENUM_FREQ_BANDS` ioctl 枚举可用的频段。)
    * - ``V4L2_TUNER_CAP_HWSEEK_PROG_LIM``
      - 0x0800
      - The range to search when using the hardware seek functionality is
	programmable, see
	:ref:`VIDIOC_S_HW_FREQ_SEEK` for
	details.
	(使用硬件搜索功能时，搜索范围是可编程的，详见 :ref:`VIDIOC_S_HW_FREQ_SEEK`。)
    * - ``V4L2_TUNER_CAP_1HZ``
      - 0x1000
      - When set, tuning frequencies are expressed in units of 1 Hz
	instead of 62.5 kHz.
	(设置时，调谐频率以 1 Hz 而非 62.5 kHz 为单位表示。)


.. tabularcolumns:: |p{6.6cm}|p{2.2cm}|p{8.5cm}|

.. _tuner-rxsubchans:

.. flat-table:: Tuner Audio Reception Flags
    :header-rows:  0
    :stub-columns: 0
    :widths:       3 1 4

    * - ``V4L2_TUNER_SUB_MONO``
      - 0x0001
      - The tuner receives a mono audio signal.(调谐器接收单声道音频信号。)
    * - ``V4L2_TUNER_SUB_STEREO``
      - 0x0002
      - The tuner receives a stereo audio signal.(调谐器接收立体声音频信号。)
    * - ``V4L2_TUNER_SUB_LANG1``
      - 0x0008
      - The tuner receives the primary language of a bilingual audio
	signal. Drivers must clear this flag when the current video
	standard is ``V4L2_STD_NTSC_M``.
	(调谐器接收双语音频信号的主语言。当当前视频标准为 ``V4L2_STD_NTSC_M`` 时，驱动必须清除该标志。)
    * - ``V4L2_TUNER_SUB_LANG2``
      - 0x0004
      - The tuner receives the secondary language of a bilingual audio
	signal (or a second audio program).
	(调谐器接收双语音频信号的副语言(或第二音频节目)。)
    * - ``V4L2_TUNER_SUB_SAP``
      - 0x0004
      - The tuner receives a Second Audio Program.(调谐器接收第二音频节目。)

	.. note::

	   The ``V4L2_TUNER_SUB_LANG2`` and ``V4L2_TUNER_SUB_SAP``
	   flags are synonyms. The ``V4L2_TUNER_SUB_SAP`` flag applies
	   when the current video standard is ``V4L2_STD_NTSC_M``.
	   (``V4L2_TUNER_SUB_LANG2`` 与 ``V4L2_TUNER_SUB_SAP`` 是同义标志。当当前视频标准为 ``V4L2_STD_NTSC_M`` 时适用 ``V4L2_TUNER_SUB_SAP`` 标志。)
    * - ``V4L2_TUNER_SUB_RDS``
      - 0x0010
      - The tuner receives an RDS channel.(调谐器接收 RDS 信道。)


.. tabularcolumns:: |p{6.6cm}|p{2.2cm}|p{8.5cm}|

.. _tuner-audmode:

.. flat-table:: Tuner Audio Modes
    :header-rows:  0
    :stub-columns: 0
    :widths:       3 1 4

    * - ``V4L2_TUNER_MODE_MONO``
      - 0
      - Play mono audio. When the tuner receives a stereo signal this a
	down-mix of the left and right channel. When the tuner receives a
	bilingual or SAP signal this mode selects the primary language.
	(播放单声道音频。当调谐器接收立体声信号时，这是左右声道的混音。当调谐器接收双语或 SAP 信号时，该模式选择主语言。)
    * - ``V4L2_TUNER_MODE_STEREO``
      - 1
      - Play stereo audio. When the tuner receives bilingual audio it may
	play different languages on the left and right channel or the
	primary language is played on both channels.
	(播放立体声音频。当调谐器接收双语音频时，可能左右声道播放不同的语言，也可能两个声道都播放主语言。)

	Playing different languages in this mode is deprecated. New
	drivers should do this only in ``MODE_LANG1_LANG2``.
	(在此模式下播放不同语言的做法已弃用。新驱动只应在 ``MODE_LANG1_LANG2`` 中这样做。)

	When the tuner receives no stereo signal or does not support
	stereo reception the driver shall fall back to ``MODE_MONO``.
	(当调谐器未接收到立体声信号或不支持立体声接收时，驱动应回退到 ``MODE_MONO``。)
    * - ``V4L2_TUNER_MODE_LANG1``
      - 3
      - Play the primary language, mono or stereo. Only
	``V4L2_TUNER_ANALOG_TV`` tuners support this mode.
	(播放主语言，单声道或立体声。只有 ``V4L2_TUNER_ANALOG_TV`` 调谐器支持该模式。)
    * - ``V4L2_TUNER_MODE_LANG2``
      - 2
      - Play the secondary language, mono. When the tuner receives no
	bilingual audio or SAP, or their reception is not supported the
	driver shall fall back to mono or stereo mode. Only
	``V4L2_TUNER_ANALOG_TV`` tuners support this mode.
	(播放副语言，单声道。当调谐器未接收双语音频或 SAP、或其接收不受支持时，驱动应回退到单声道或立体声模式。只有 ``V4L2_TUNER_ANALOG_TV`` 调谐器支持该模式。)
    * - ``V4L2_TUNER_MODE_SAP``
      - 2
      - Play the Second Audio Program. When the tuner receives no
	bilingual audio or SAP, or their reception is not supported the
	driver shall fall back to mono or stereo mode. Only
	``V4L2_TUNER_ANALOG_TV`` tuners support this mode.
	(播放第二音频节目。当调谐器未接收双语音频或 SAP、或其接收不受支持时，驱动应回退到单声道或立体声模式。只有 ``V4L2_TUNER_ANALOG_TV`` 调谐器支持该模式。)

	.. note:: The ``V4L2_TUNER_MODE_LANG2`` and ``V4L2_TUNER_MODE_SAP``
	   are synonyms.
	   (``V4L2_TUNER_MODE_LANG2`` 与 ``V4L2_TUNER_MODE_SAP`` 是同义标志。)
    * - ``V4L2_TUNER_MODE_LANG1_LANG2``
      - 4
      - Play the primary language on the left channel, the secondary
	language on the right channel. When the tuner receives no
	bilingual audio or SAP, it shall fall back to ``MODE_LANG1`` or
	``MODE_MONO``. Only ``V4L2_TUNER_ANALOG_TV`` tuners support this
	mode.
	(左声道播放主语言，右声道播放副语言。当调谐器未接收双语音频或 SAP 时，应回退到 ``MODE_LANG1`` 或 ``MODE_MONO``。只有 ``V4L2_TUNER_ANALOG_TV`` 调谐器支持该模式。)

.. raw:: latex

    \scriptsize

.. tabularcolumns:: |p{1.5cm}|p{1.5cm}|p{2.9cm}|p{2.9cm}|p{2.9cm}|p{2.9cm}|

.. _tuner-matrix:

.. flat-table:: Tuner Audio Matrix
    :header-rows:  2
    :stub-columns: 0
    :widths: 7 7 14 14 14 14

    * -
      - :cspan:`4` Selected ``V4L2_TUNER_MODE_``(所选音频模式)
    * - Received ``V4L2_TUNER_SUB_``(接收到的音频)
      - ``MONO``
      - ``STEREO``
      - ``LANG1``
      - ``LANG2 = SAP``
      - ``LANG1_LANG2``\ [#f1]_
    * - ``MONO``
      - Mono(单声道)
      - Mono/Mono(单声道/单声道)
      - Mono(单声道)
      - Mono(单声道)
      - Mono/Mono(单声道/单声道)
    * - ``MONO | SAP``
      - Mono(单声道)
      - Mono/Mono(单声道/单声道)
      - Mono(单声道)
      - SAP(第二音频节目)
      - Mono/SAP (preferred) or Mono/Mono(单声道/SAP(首选)或单声道/单声道)
    * - ``STEREO``
      - L+R(左+右)
      - L/R(左/右)
      - Stereo L/R (preferred) or Mono L+R(立体声 L/R(首选)或单声道 L+R)
      - Stereo L/R (preferred) or Mono L+R(立体声 L/R(首选)或单声道 L+R)
      - L/R (preferred) or L+R/L+R(L/R(首选)或 L+R/L+R)
    * - ``STEREO | SAP``
      - L+R(左+右)
      - L/R(左/右)
      - Stereo L/R (preferred) or Mono L+R(立体声 L/R(首选)或单声道 L+R)
      - SAP(第二音频节目)
      - L+R/SAP (preferred) or L/R or L+R/L+R(L+R/SAP(首选)或 L/R 或 L+R/L+R)
    * - ``LANG1 | LANG2``
      - Language 1(语言 1)
      - Lang1/Lang2 (deprecated\ [#f2]_) or Lang1/Lang1(Lang1/Lang2(已弃用)或 Lang1/Lang1)
      - Language 1(语言 1)
      - Language 2(语言 2)
      - Lang1/Lang2 (preferred) or Lang1/Lang1(Lang1/Lang2(首选)或 Lang1/Lang1)

.. raw:: latex

    \normalsize

Return Value(返回值)
====================

On success 0 is returned, on error -1 and the ``errno`` variable is set
appropriately. The generic error codes are described at the
:ref:`Generic Error Codes <gen-errors>` chapter.
(成功时返回 0；出错时返回 -1 并适当地设置 ``errno`` 变量。通用错误码在 :ref:`Generic Error Codes <gen-errors>` 一章中描述。)

EINVAL
    The struct :c:type:`v4l2_tuner` ``index`` is out of
    bounds.
    (struct :c:type:`v4l2_tuner` 的 ``index`` 越界。)

.. [#f1]
   This mode has been added in Linux 2.6.17 and may not be supported by
   older drivers.
   (该模式在 Linux 2.6.17 中加入，较旧的驱动可能不支持。)

.. [#f2]
   Playback of both languages in ``MODE_STEREO`` is deprecated. In the
   future drivers should produce only the primary language in this mode.
   Applications should request ``MODE_LANG1_LANG2`` to record both
   languages or a stereo signal.
   (在 ``MODE_STEREO`` 中播放两种语言的做法已弃用。将来驱动在此模式下应只输出主语言。应用程序应请求 ``MODE_LANG1_LANG2`` 来录制两种语言或立体声信号。)
