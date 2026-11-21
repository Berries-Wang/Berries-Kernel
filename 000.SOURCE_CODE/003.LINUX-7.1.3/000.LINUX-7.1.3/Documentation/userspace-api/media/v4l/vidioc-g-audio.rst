.. SPDX-License-Identifier: GFDL-1.1-no-invariants-or-later
.. c:namespace:: V4L

.. _VIDIOC_G_AUDIO:

************************************
ioctl VIDIOC_G_AUDIO, VIDIOC_S_AUDIO
************************************

Name(名称)
==========

VIDIOC_G_AUDIO - VIDIOC_S_AUDIO - Query or select the current audio input and its attributes
(VIDIOC_G_AUDIO - VIDIOC_S_AUDIO - 查询或选择当前音频输入及其属性。)

Synopsis(概要)
==============

.. c:macro:: VIDIOC_G_AUDIO

``int ioctl(int fd, VIDIOC_G_AUDIO, struct v4l2_audio *argp)``

.. c:macro:: VIDIOC_S_AUDIO

``int ioctl(int fd, VIDIOC_S_AUDIO, const struct v4l2_audio *argp)``

Arguments(参数)
===============

``fd``
    File descriptor returned by :c:func:`open()`.
    (由 :c:func:`open()` 返回的文件描述符。)

``argp``
    Pointer to struct :c:type:`v4l2_audio`.
    (指向 struct :c:type:`v4l2_audio` 的指针。)

Description(描述)
=================

To query the current audio input applications zero out the ``reserved``
array of a struct :c:type:`v4l2_audio` and call the
:ref:`VIDIOC_G_AUDIO <VIDIOC_G_AUDIO>` ioctl with a pointer to this structure. Drivers fill
the rest of the structure or return an ``EINVAL`` error code when the device
has no audio inputs, or none which combine with the current video input.
(要查询当前音频输入，应用程序把 struct :c:type:`v4l2_audio` 的 ``reserved`` 数组清零，并以指向该结构的指针调用 :ref:`VIDIOC_G_AUDIO <VIDIOC_G_AUDIO>` ioctl。当设备没有音频输入，或没有能与当前视频输入组合的音频输入时，驱动程序填充结构的其余部分或返回 ``EINVAL`` 错误码。)

Audio inputs have one writable property, the audio mode. To select the
current audio input *and* change the audio mode, applications initialize
the ``index`` and ``mode`` fields, and the ``reserved`` array of a
struct :c:type:`v4l2_audio` structure and call the :ref:`VIDIOC_S_AUDIO <VIDIOC_G_AUDIO>`
ioctl. Drivers may switch to a different audio mode if the request
cannot be satisfied. However, this is a write-only ioctl, it does not
return the actual new audio mode.
(音频输入只有一个可写属性：音频模式。要选择当前音频输入 *并* 更改音频模式，应用程序初始化 ``index`` 和 ``mode`` 字段以及 struct :c:type:`v4l2_audio` 结构的 ``reserved`` 数组，然后调用 :ref:`VIDIOC_S_AUDIO <VIDIOC_G_AUDIO>` ioctl。如果请求无法满足，驱动程序可以切换到不同的音频模式。不过，这是一个只写的 ioctl，它不会返回实际的新音频模式。)

.. tabularcolumns:: |p{4.4cm}|p{4.4cm}|p{8.5cm}|

.. c:type:: v4l2_audio

.. flat-table:: struct v4l2_audio
    :header-rows:  0
    :stub-columns: 0
    :widths:       1 1 2

    * - __u32
      - ``index``
      - Identifies the audio input, set by the driver or application.
        (标识音频输入，由驱动程序或应用程序设置。)
    * - __u8
      - ``name``\ [32]
      - Name of the audio input, a NUL-terminated ASCII string, for
	example: "Line In". This information is intended for the user,
	preferably the connector label on the device itself.
        (音频输入的名称，一个以 NUL 结尾的 ASCII 字符串，例如 "Line In"。此信息面向用户，最好是设备本身的连接器标签。)
    * - __u32
      - ``capability``
      - Audio capability flags, see :ref:`audio-capability`.
        (音频能力标志，参见 :ref:`audio-capability` 。)
    * - __u32
      - ``mode``
      - Audio mode flags set by drivers and applications (on
	:ref:`VIDIOC_S_AUDIO <VIDIOC_G_AUDIO>` ioctl), see :ref:`audio-mode`.
        (由驱动程序和应用程序设置（在 :ref:`VIDIOC_S_AUDIO <VIDIOC_G_AUDIO>` ioctl 上）的音频模式标志，参见 :ref:`audio-mode` 。)
    * - __u32
      - ``reserved``\ [2]
      - Reserved for future extensions. Drivers and applications must set
	the array to zero.
        (保留用于未来的扩展。驱动程序和应用程序必须把该数组置零。)


.. tabularcolumns:: |p{6.6cm}|p{2.2cm}|p{8.5cm}|

.. _audio-capability:

.. flat-table:: Audio Capability Flags
    :header-rows:  0
    :stub-columns: 0
    :widths:       3 1 4

    * - ``V4L2_AUDCAP_STEREO``
      - 0x00001
      - This is a stereo input. The flag is intended to automatically
	disable stereo recording etc. when the signal is always monaural.
	The API provides no means to detect if stereo is *received*,
	unless the audio input belongs to a tuner.
        (这是一个立体声输入。该标志用于在信号始终为单声道时自动禁用立体声录制等。除非音频输入属于调谐器，否则 API 不提供检测是否 *接收到* 立体声的方法。)
    * - ``V4L2_AUDCAP_AVL``
      - 0x00002
      - Automatic Volume Level mode is supported.
        (支持自动音量电平（Automatic Volume Level）模式。)


.. tabularcolumns:: |p{6.6cm}|p{2.2cm}|p{8.5cm}|

.. _audio-mode:

.. flat-table:: Audio Mode Flags
    :header-rows:  0
    :stub-columns: 0
    :widths:       3 1 4

    * - ``V4L2_AUDMODE_AVL``
      - 0x00001
      - AVL mode is on.
        (AVL 模式开启。)

Return Value(返回值)
====================

On success 0 is returned, on error -1 and the ``errno`` variable is set
appropriately. The generic error codes are described at the
:ref:`Generic Error Codes <gen-errors>` chapter.
(成功时返回 0，出错时返回 -1 并适当设置 ``errno`` 变量。通用错误码在 :ref:`Generic Error Codes <gen-errors>` 一章中描述。)

EINVAL
    No audio inputs combine with the current video input, or the number
    of the selected audio input is out of bounds or it does not combine.
    (没有音频输入能与当前视频输入组合，或所选音频输入的编号越界或无法组合。)
