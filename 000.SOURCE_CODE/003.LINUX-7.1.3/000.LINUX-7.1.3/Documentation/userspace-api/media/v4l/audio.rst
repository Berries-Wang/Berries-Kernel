.. SPDX-License-Identifier: GFDL-1.1-no-invariants-or-later
.. c:namespace:: V4L

.. _audio:

****************************************
Audio Inputs and Outputs(音频输入与输出)
****************************************

Audio inputs and outputs are physical connectors of a device. Video
capture devices have inputs, output devices have outputs, zero or more
each. Radio devices have no audio inputs or outputs. They have exactly
one tuner which in fact *is* an audio source, but this API associates
tuners with video inputs or outputs only, and radio devices have none of
these. [#f1]_ A connector on a TV card to loop back the received audio
signal to a sound card is not considered an audio output.
(音频输入和输出是设备的物理连接器。视频捕获设备有输入，输出设备有
输出，各自为零个或多个。无线电设备没有音频输入或输出。它们恰好有一个
调谐器，而调谐器实际上就是一个音频源，但该 API 只把调谐器与视频输入
或输出相关联，而无线电设备没有这两者。电视卡上用于把接收到的音频信号
回送到声卡的连接器不被视为音频输出。)

Audio and video inputs and outputs are associated. Selecting a video
source also selects an audio source. This is most evident when the video
and audio source is a tuner. Further audio connectors can combine with
more than one video input or output. Assumed two composite video inputs
and two audio inputs exist, there may be up to four valid combinations.
The relation of video and audio connectors is defined in the
``audioset`` field of the respective struct
:c:type:`v4l2_input` or struct
:c:type:`v4l2_output`, where each bit represents the index
number, starting at zero, of one audio input or output.
(音频和视频输入输出是相关联的。选择一个视频源也会选择一个音频源。
当视频和音频源是调谐器时，这一点最为明显。此外，音频连接器可以与
不止一个视频输入或输出组合。假设存在两个复合视频输入和两个音频输入，
则最多可能有四个有效组合。视频与音频连接器的关系在相应 struct
:c:type:`v4l2_input` 或 struct :c:type:`v4l2_output` 的 ``audioset``
字段中定义，其中每个位代表一个音频输入或输出的索引号，从零开始。)

To learn about the number and attributes of the available inputs and
outputs applications can enumerate them with the
:ref:`VIDIOC_ENUMAUDIO` and
:ref:`VIDIOC_ENUMAUDOUT <VIDIOC_ENUMAUDOUT>` ioctl, respectively.
The struct :c:type:`v4l2_audio` returned by the
:ref:`VIDIOC_ENUMAUDIO` ioctl also contains signal
status information applicable when the current audio input is queried.
(要了解可用输入和输出的数量与属性，应用程序可以分别用
:ref:`VIDIOC_ENUMAUDIO` 和 :ref:`VIDIOC_ENUMAUDOUT <VIDIOC_ENUMAUDOUT>`
ioctl 枚举它们。:ref:`VIDIOC_ENUMAUDIO` ioctl 返回的 struct
:c:type:`v4l2_audio` 还包含在查询当前音频输入时适用的信号状态信息。)

The :ref:`VIDIOC_G_AUDIO <VIDIOC_G_AUDIO>` and
:ref:`VIDIOC_G_AUDOUT <VIDIOC_G_AUDOUT>` ioctls report the current
audio input and output, respectively.
(:ref:`VIDIOC_G_AUDIO <VIDIOC_G_AUDIO>` 和 :ref:`VIDIOC_G_AUDOUT <VIDIOC_G_AUDOUT>`
ioctl 分别报告当前的音频输入和输出。)

.. note::

   Note that, unlike :ref:`VIDIOC_G_INPUT <VIDIOC_G_INPUT>` and
   :ref:`VIDIOC_G_OUTPUT <VIDIOC_G_OUTPUT>` these ioctls return a
   structure as :ref:`VIDIOC_ENUMAUDIO` and
   :ref:`VIDIOC_ENUMAUDOUT <VIDIOC_ENUMAUDOUT>` do, not just an index.
   (注意，与 :ref:`VIDIOC_G_INPUT <VIDIOC_G_INPUT>` 和 :ref:`VIDIOC_G_OUTPUT <VIDIOC_G_OUTPUT>`
   不同，这些 ioctl 返回一个结构体，正如 :ref:`VIDIOC_ENUMAUDIO` 和
   :ref:`VIDIOC_ENUMAUDOUT <VIDIOC_ENUMAUDOUT>` 那样，而不只是一个
   索引。)

To select an audio input and change its properties applications call the
:ref:`VIDIOC_S_AUDIO <VIDIOC_G_AUDIO>` ioctl. To select an audio
output (which presently has no changeable properties) applications call
the :ref:`VIDIOC_S_AUDOUT <VIDIOC_G_AUDOUT>` ioctl.
(要选择音频输入并更改其属性，应用程序调用 :ref:`VIDIOC_S_AUDIO <VIDIOC_G_AUDIO>`
ioctl。要选择音频输出(目前没有可更改的属性)，应用程序调用
:ref:`VIDIOC_S_AUDOUT <VIDIOC_G_AUDOUT>` ioctl。)

Drivers must implement all audio input ioctls when the device has
multiple selectable audio inputs, all audio output ioctls when the
device has multiple selectable audio outputs. When the device has any
audio inputs or outputs the driver must set the ``V4L2_CAP_AUDIO`` flag
in the struct :c:type:`v4l2_capability` returned by
the :ref:`VIDIOC_QUERYCAP` ioctl.
(当设备有多个可选音频输入时，驱动程序必须实现所有音频输入 ioctl；
当设备有多个可选音频输出时，必须实现所有音频输出 ioctl。当设备有
任何音频输入或输出时，驱动程序必须在 :ref:`VIDIOC_QUERYCAP` ioctl
返回的 struct :c:type:`v4l2_capability` 中设置 ``V4L2_CAP_AUDIO``
标志。)


Example: Information about the current audio input(示例：当前音频输入的信息)
============================================================================

.. code-block:: c

    struct v4l2_audio audio;

    memset(&audio, 0, sizeof(audio));

    if (-1 == ioctl(fd, VIDIOC_G_AUDIO, &audio)) {
	perror("VIDIOC_G_AUDIO");
	exit(EXIT_FAILURE);
    }

    printf("Current input: %s\\n", audio.name);


Example: Switching to the first audio input(示例：切换到第一个音频输入)
=======================================================================

.. code-block:: c

    struct v4l2_audio audio;

    memset(&audio, 0, sizeof(audio)); /* clear audio.mode, audio.reserved */

    audio.index = 0;

    if (-1 == ioctl(fd, VIDIOC_S_AUDIO, &audio)) {
	perror("VIDIOC_S_AUDIO");
	exit(EXIT_FAILURE);
    }

.. [#f1]
   Actually struct :c:type:`v4l2_audio` ought to have a
   ``tuner`` field like struct :c:type:`v4l2_input`, not
   only making the API more consistent but also permitting radio devices
   with multiple tuners.
   (实际上 struct :c:type:`v4l2_audio` 应当像 struct
   :c:type:`v4l2_input` 一样有一个 ``tuner`` 字段，这不仅会让 API 更
   一致，还能支持带有多个调谐器的无线电设备。)
