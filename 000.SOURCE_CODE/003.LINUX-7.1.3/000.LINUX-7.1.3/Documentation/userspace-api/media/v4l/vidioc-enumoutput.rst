.. SPDX-License-Identifier: GFDL-1.1-no-invariants-or-later
.. c:namespace:: V4L

.. _VIDIOC_ENUMOUTPUT:

*************************************
ioctl VIDIOC_ENUMOUTPUT(枚举视频输出)
*************************************

Name(名称)
==========

VIDIOC_ENUMOUTPUT - Enumerate video outputs
(VIDIOC_ENUMOUTPUT - 枚举视频输出)

Synopsis(概要)
==============

.. c:macro:: VIDIOC_ENUMOUTPUT

``int ioctl(int fd, VIDIOC_ENUMOUTPUT, struct v4l2_output *argp)``

Arguments(参数)
===============

``fd``
    File descriptor returned by :c:func:`open()`.
    (由 :c:func:`open()` 返回的文件描述符。)

``argp``
    Pointer to struct :c:type:`v4l2_output`.
    (指向 struct :c:type:`v4l2_output` 的指针。)

Description(描述)
=================

To query the attributes of a video outputs applications initialize the
``index`` field of struct :c:type:`v4l2_output` and call
the :ref:`VIDIOC_ENUMOUTPUT` with a pointer to this structure.
Drivers fill the rest of the structure or return an ``EINVAL`` error code
when the index is out of bounds. To enumerate all outputs applications
shall begin at index zero, incrementing by one until the driver returns
``EINVAL``.
(要查询视频输出的属性，应用程序初始化 struct :c:type:`v4l2_output` 的
``index`` 字段，并用指向该结构体的指针调用 :ref:`VIDIOC_ENUMOUTPUT`。
当索引越界时，驱动填充结构体的其余部分或返回 ``EINVAL`` 错误码。要枚举
所有输出，应用程序应从索引 0 开始，每次加 1，直到驱动返回 ``EINVAL``。)

.. tabularcolumns:: |p{4.4cm}|p{4.4cm}|p{8.5cm}|

.. c:type:: v4l2_output

.. flat-table:: struct v4l2_output
    :header-rows:  0
    :stub-columns: 0
    :widths:       1 1 2

    * - __u32
      - ``index``
      - Identifies the output, set by the application.
	(标识输出，由应用程序设置。)
    * - __u8
      - ``name``\ [32]
      - Name of the video output, a NUL-terminated ASCII string, for
	example: "Vout". This information is intended for the user,
	preferably the connector label on the device itself.
	(视频输出的名称，以 NUL 结尾的 ASCII 字符串，例如 "Vout"。此信息
	面向用户，最好是设备本身上连接器的标签。)
    * - __u32
      - ``type``
      - Type of the output, see :ref:`output-type`.
	(输出的类型，参见 :ref:`output-type`。)
    * - __u32
      - ``audioset``
      - Drivers can enumerate up to 32 video and audio outputs. This field
	shows which audio outputs were selectable as the current output if
	this was the currently selected video output. It is a bit mask.
	The LSB corresponds to audio output 0, the MSB to output 31. Any
	number of bits can be set, or none.
	(驱动最多可以枚举 32 个视频和音频输出。如果本输出是当前选中的视频
	输出，此字段显示哪些音频输出可被选择为当前输出。它是一个位掩码。
	LSB 对应音频输出 0，MSB 对应输出 31。可以置位任意数量的位，也可以
	一个都不置位。)

	When the driver does not enumerate audio outputs no bits must be
	set. Applications shall not interpret this as lack of audio
	support. Drivers may automatically select audio outputs without
	enumerating them.
	(当驱动不枚举音频输出时，不得置位任何位。应用程序不应将此解释为
	缺乏音频支持。驱动可以在不枚举音频输出的情况下自动选择音频
	输出。)

	For details on audio outputs and how to select the current output
	see :ref:`audio`.
	(有关音频输出以及如何选择当前输出的详细信息，参见 :ref:`audio`。)
    * - __u32
      - ``modulator``
      - Output devices can have zero or more RF modulators. When the
	``type`` is ``V4L2_OUTPUT_TYPE_MODULATOR`` this is an RF connector
	and this field identifies the modulator. It corresponds to struct
	:c:type:`v4l2_modulator` field ``index``. For
	details on modulators see :ref:`tuner`.
	(输出设备可以有零个或多个 RF 调制器。当 ``type`` 为
	``V4L2_OUTPUT_TYPE_MODULATOR`` 时，这是一个 RF 连接器，此字段标识
	调制器。它对应 struct :c:type:`v4l2_modulator` 的 ``index`` 字段。
	有关调制器的详细信息，参见 :ref:`tuner`。)
    * - :ref:`v4l2_std_id <v4l2-std-id>`
      - ``std``
      - Every video output supports one or more different video standards.
	This field is a set of all supported standards. For details on
	video standards and how to switch see :ref:`standard`.
	(每个视频输出支持一种或多种不同的视频制式。此字段是所有受支持
	制式的集合。有关视频制式以及如何切换的详细信息，参见
	:ref:`standard`。)
    * - __u32
      - ``capabilities``
      - This field provides capabilities for the output. See
	:ref:`output-capabilities` for flags.
	(此字段提供输出的能力。标志参见 :ref:`output-capabilities`。)
    * - __u32
      - ``reserved``\ [3]
      - Reserved for future extensions. Drivers must set the array to
	zero.
	(为未来扩展保留。驱动必须将数组置零。)


.. tabularcolumns:: |p{7.5cm}|p{0.6cm}|p{9.2cm}|

.. _output-type:

.. flat-table:: Output Type
    :header-rows:  0
    :stub-columns: 0
    :widths:       3 1 4

    * - ``V4L2_OUTPUT_TYPE_MODULATOR``
      - 1
      - This output is an analog TV modulator.
	(此输出是模拟电视调制器。)
    * - ``V4L2_OUTPUT_TYPE_ANALOG``
      - 2
      - Any non-modulator video output, for example Composite Video,
	S-Video, HDMI. The naming as ``_TYPE_ANALOG`` is historical,
	today we would have called it ``_TYPE_VIDEO``.
	(任何非调制器视频输出，例如复合视频 (Composite Video)、S-Video、
	HDMI。命名为 ``_TYPE_ANALOG`` 是历史原因，如今我们会称其为
	``_TYPE_VIDEO`` 。)
    * - ``V4L2_OUTPUT_TYPE_ANALOGVGAOVERLAY``
      - 3
      - The video output will be copied to a :ref:`video overlay <overlay>`.
	(该视频输出将被复制到 :ref:`video overlay <overlay>` （视频
	覆盖）。)


.. tabularcolumns:: |p{6.4cm}|p{2.4cm}|p{8.5cm}|

.. _output-capabilities:

.. flat-table:: Output capabilities
    :header-rows:  0
    :stub-columns: 0
    :widths:       3 1 4

    * - ``V4L2_OUT_CAP_DV_TIMINGS``
      - 0x00000002
      - This output supports setting video timings by using
	``VIDIOC_S_DV_TIMINGS``.
	(此输出支持使用 ``VIDIOC_S_DV_TIMINGS`` 设置视频时序。)
    * - ``V4L2_OUT_CAP_STD``
      - 0x00000004
      - This output supports setting the TV standard by using
	``VIDIOC_S_STD``.
	(此输出支持使用 ``VIDIOC_S_STD`` 设置电视制式。)
    * - ``V4L2_OUT_CAP_NATIVE_SIZE``
      - 0x00000008
      - This output supports setting the native size using the
	``V4L2_SEL_TGT_NATIVE_SIZE`` selection target, see
	:ref:`v4l2-selections-common`.
	(此输出支持使用 ``V4L2_SEL_TGT_NATIVE_SIZE`` 选择目标设置原始
	尺寸，参见 :ref:`v4l2-selections-common`。)

Return Value(返回值)
====================

On success 0 is returned, on error -1 and the ``errno`` variable is set
appropriately. The generic error codes are described at the
:ref:`Generic Error Codes <gen-errors>` chapter.
(成功时返回 0，出错时返回 -1 并适当设置 ``errno`` 变量。通用错误码在
:ref:`Generic Error Codes <gen-errors>` 一章中描述。)

EINVAL
    The struct :c:type:`v4l2_output` ``index`` is out of
    bounds.
    (struct :c:type:`v4l2_output` 的 ``index`` 越界。)
