.. SPDX-License-Identifier: GFDL-1.1-no-invariants-or-later
.. c:namespace:: V4L

.. _VIDIOC_ENUMINPUT:

************************************
ioctl VIDIOC_ENUMINPUT(枚举视频输入)
************************************

Name(名称)
==========

VIDIOC_ENUMINPUT - Enumerate video inputs(VIDIOC_ENUMINPUT - 枚举视频输入)

Synopsis(概要)
===============

.. c:macro:: VIDIOC_ENUMINPUT

``int ioctl(int fd, VIDIOC_ENUMINPUT, struct v4l2_input *argp)``

Arguments(参数)
===============

``fd``
    File descriptor returned by :c:func:`open()`.
    (:c:func:`open()` 返回的文件描述符。)

``argp``
    Pointer to struct :c:type:`v4l2_input`.
    (指向 struct :c:type:`v4l2_input` 的指针。)

Description(描述)
=================

To query the attributes of a video input applications initialize the
``index`` field of struct :c:type:`v4l2_input` and call the
:ref:`VIDIOC_ENUMINPUT` with a pointer to this structure. Drivers
fill the rest of the structure or return an ``EINVAL`` error code when the
index is out of bounds. To enumerate all inputs applications shall begin
at index zero, incrementing by one until the driver returns ``EINVAL``.
(要查询视频输入的属性，应用程序先初始化 struct :c:type:`v4l2_input` 的 ``index`` 字段，然后用指向该结构的指针调用
:ref:`VIDIOC_ENUMINPUT` 。驱动填充结构的其余部分，或在索引越界时返回 ``EINVAL`` 错误码。要枚举所有输入，应用程序应从索引 0 开始，每次加一，直到驱动返回 ``EINVAL`` 。)

.. tabularcolumns:: |p{3.0cm}|p{3.5cm}|p{10.8cm}|

.. c:type:: v4l2_input

.. flat-table:: struct v4l2_input
    :header-rows:  0
    :stub-columns: 0
    :widths:       1 1 2

    * - __u32
      - ``index``
      - Identifies the input, set by the application.
        (标识输入，由应用程序设置。)
    * - __u8
      - ``name``\ [32]
      - Name of the video input, a NUL-terminated ASCII string, for
	example: "Vin (Composite 2)". This information is intended for the
	user, preferably the connector label on the device itself.
        (视频输入的名称，一个以 NUL 结尾的 ASCII 字符串，例如 "Vin (Composite 2)"。该信息面向用户，最好是设备本身连接器上的标签。)
    * - __u32
      - ``type``
      - Type of the input, see :ref:`input-type`.
        (输入的类型，见 :ref:`input-type`。)
    * - __u32
      - ``audioset``
      - Drivers can enumerate up to 32 video and audio inputs. This field
	shows which audio inputs were selectable as audio source if this
	was the currently selected video input. It is a bit mask. The LSB
	corresponds to audio input 0, the MSB to input 31. Any number of
	bits can be set, or none.

	When the driver does not enumerate audio inputs no bits must be
	set. Applications shall not interpret this as lack of audio
	support. Some drivers automatically select audio sources and do
	not enumerate them since there is no choice anyway.

	For details on audio inputs and how to select the current input
	see :ref:`audio`.
        (驱动最多可枚举 32 个视频与音频输入。如果当前选中的是这个视频输入，该字段显示哪些音频输入可选作音频源。它是一个位掩码。LSB 对应音频输入 0，MSB 对应输入 31。可以置任意多个位，也可以一个都不置。

	当驱动不枚举音频输入时，所有位都不应置位。应用程序不应把这种情况解读为缺乏音频支持。有些驱动自动选择音频源且不枚举它们，因为反正没有别的选择。

	音频输入的细节以及如何选择当前输入见 :ref:`audio` 。)
    * - __u32
      - ``tuner``
      - Capture devices can have zero or more tuners (RF demodulators).
	When the ``type`` is set to ``V4L2_INPUT_TYPE_TUNER`` this is an
	RF connector and this field identifies the tuner. It corresponds
	to struct :c:type:`v4l2_tuner` field ``index``. For
	details on tuners see :ref:`tuner`.
        (采集设备可以没有或有多个调谐器 (RF 解调器)。当 ``type`` 被设为 ``V4L2_INPUT_TYPE_TUNER`` 时，这是一个 RF 连接器，该字段标识调谐器。它对应 struct :c:type:`v4l2_tuner` 的 ``index`` 字段。调谐器的细节见 :ref:`tuner` 。)
    * - :ref:`v4l2_std_id <v4l2-std-id>`
      - ``std``
      - Every video input supports one or more different video standards.
	This field is a set of all supported standards. For details on
	video standards and how to switch see :ref:`standard`.
        (每个视频输入支持一种或多种不同的视频标准。该字段是所有受支持标准的集合。视频标准的细节及切换方法见 :ref:`standard` 。)
    * - __u32
      - ``status``
      - This field provides status information about the input. See
	:ref:`input-status` for flags. With the exception of the sensor
	orientation bits ``status`` is only valid when this is the current
	input.
        (该字段提供关于输入的状态信息。标志见 :ref:`input-status` 。除传感器方向位外，``status`` 仅在当前输入是该输入时才有效。)
    * - __u32
      - ``capabilities``
      - This field provides capabilities for the input. See
	:ref:`input-capabilities` for flags.
        (该字段提供输入的能力信息。标志见 :ref:`input-capabilities` 。)
    * - __u32
      - ``reserved``\ [3]
      - Reserved for future extensions. Drivers must set the array to
	zero.
        (留作将来扩展。驱动必须把该数组置零。)


.. tabularcolumns:: |p{6.6cm}|p{1.0cm}|p{9.7cm}|

.. _input-type:

.. flat-table:: Input Types
    :header-rows:  0
    :stub-columns: 0
    :widths:       3 1 4

    * - ``V4L2_INPUT_TYPE_TUNER``
      - 1
      - This input uses a tuner (RF demodulator).
        (该输入使用调谐器 (RF 解调器)。)
    * - ``V4L2_INPUT_TYPE_CAMERA``
      - 2
      - Any non-tuner video input, for example Composite Video,
	S-Video, HDMI, camera sensor. The naming as ``_TYPE_CAMERA`` is historical,
	today we would have called it ``_TYPE_VIDEO``.
        (任何非调谐器视频输入，例如复合视频、S-Video、HDMI、相机传感器。命名为 ``_TYPE_CAMERA`` 属历史原因，如今我们会把它叫做 ``_TYPE_VIDEO`` 。)
    * - ``V4L2_INPUT_TYPE_TOUCH``
      - 3
      - This input is a touch device for capturing raw touch data.
        (该输入是用于采集原始触摸数据的触摸设备。)


.. tabularcolumns:: |p{5.6cm}|p{2.6cm}|p{9.1cm}|

.. _input-status:

.. flat-table:: Input Status Flags
    :header-rows:  0
    :stub-columns: 0

    * - :cspan:`2` General(一般)
    * - ``V4L2_IN_ST_NO_POWER``
      - 0x00000001
      - Attached device is off.(所连接的设备已关闭。)
    * - ``V4L2_IN_ST_NO_SIGNAL``
      - 0x00000002
      -
    * - ``V4L2_IN_ST_NO_COLOR``
      - 0x00000004
      - The hardware supports color decoding, but does not detect color
	modulation in the signal.
        (硬件支持色彩解码，但未在信号中检测到色彩调制。)
    * - :cspan:`2` Sensor Orientation(传感器方向)
    * - ``V4L2_IN_ST_HFLIP``
      - 0x00000010
      - The input is connected to a device that produces a signal that is
	flipped horizontally and does not correct this before passing the
	signal to userspace.
        (该输入所连接的设备产生的信号被水平翻转，且在把信号传给用户态之前不会纠正。)
    * - ``V4L2_IN_ST_VFLIP``
      - 0x00000020
      - The input is connected to a device that produces a signal that is
	flipped vertically and does not correct this before passing the
	signal to userspace.
	.. note:: A 180 degree rotation is the same as HFLIP | VFLIP
        (该输入所连接的设备产生的信号被垂直翻转，且在把信号传给用户态之前不会纠正。
	.. note:: 180 度旋转等同于 HFLIP | VFLIP)
    * - :cspan:`2` Analog Video(模拟视频)
    * - ``V4L2_IN_ST_NO_H_LOCK``
      - 0x00000100
      - No horizontal sync lock.(没有水平同步锁定。)
    * - ``V4L2_IN_ST_COLOR_KILL``
      - 0x00000200
      - A color killer circuit automatically disables color decoding when
	it detects no color modulation. When this flag is set the color
	killer is enabled *and* has shut off color decoding.
        (消色电路在检测到没有色彩调制时自动禁用色彩解码。当此标志置位时，消色电路已启用 *并且* 已关闭色彩解码。)
    * - ``V4L2_IN_ST_NO_V_LOCK``
      - 0x00000400
      - No vertical sync lock.(没有垂直同步锁定。)
    * - ``V4L2_IN_ST_NO_STD_LOCK``
      - 0x00000800
      - No standard format lock in case of auto-detection format
	by the component.
        (当组件自动检测格式时没有标准格式锁定。)
    * - :cspan:`2` Digital Video(数字视频)
    * - ``V4L2_IN_ST_NO_SYNC``
      - 0x00010000
      - No synchronization lock.(没有同步锁定。)
    * - ``V4L2_IN_ST_NO_EQU``
      - 0x00020000
      - No equalizer lock.(没有均衡器锁定。)
    * - ``V4L2_IN_ST_NO_CARRIER``
      - 0x00040000
      - Carrier recovery failed.(载波恢复失败。)
    * - :cspan:`2` VCR and Set-Top Box(录像机与机顶盒)
    * - ``V4L2_IN_ST_MACROVISION``
      - 0x01000000
      - Macrovision is an analog copy prevention system mangling the video
	signal to confuse video recorders. When this flag is set
	Macrovision has been detected.
        (Macrovision 是一种模拟拷贝防护系统，通过搅乱视频信号来迷惑录像机。当此标志置位时，表示检测到了 Macrovision。)
    * - ``V4L2_IN_ST_NO_ACCESS``
      - 0x02000000
      - Conditional access denied.(条件访问被拒绝。)
    * - ``V4L2_IN_ST_VTR``
      - 0x04000000
      - VTR time constant. [?]
        (VTR 时间常数。[?])


.. tabularcolumns:: |p{6.6cm}|p{2.4cm}|p{8.3cm}|

.. _input-capabilities:

.. flat-table:: Input capabilities
    :header-rows:  0
    :stub-columns: 0
    :widths:       3 1 4

    * - ``V4L2_IN_CAP_DV_TIMINGS``
      - 0x00000002
      - This input supports setting video timings by using
	``VIDIOC_S_DV_TIMINGS``.
        (该输入支持通过使用 ``VIDIOC_S_DV_TIMINGS`` 来设置视频时序。)
    * - ``V4L2_IN_CAP_STD``
      - 0x00000004
      - This input supports setting the TV standard by using
	``VIDIOC_S_STD``.
        (该输入支持通过使用 ``VIDIOC_S_STD`` 来设置电视标准。)
    * - ``V4L2_IN_CAP_NATIVE_SIZE``
      - 0x00000008
      - This input supports setting the native size using the
	``V4L2_SEL_TGT_NATIVE_SIZE`` selection target, see
	:ref:`v4l2-selections-common`.
        (该输入支持使用 ``V4L2_SEL_TGT_NATIVE_SIZE`` 选择目标来设置原生尺寸，见
	:ref:`v4l2-selections-common` 。)

Return Value(返回值)
====================

On success 0 is returned, on error -1 and the ``errno`` variable is set
appropriately. The generic error codes are described at the
:ref:`Generic Error Codes <gen-errors>` chapter.
(成功时返回 0；出错时返回 -1 并适当设置 ``errno`` 变量。通用错误码在
:ref:`Generic Error Codes <gen-errors>` 一章中描述。)

EINVAL
    The struct :c:type:`v4l2_input` ``index`` is out of
    bounds.
    (struct :c:type:`v4l2_input` 的 ``index`` 越界。)
