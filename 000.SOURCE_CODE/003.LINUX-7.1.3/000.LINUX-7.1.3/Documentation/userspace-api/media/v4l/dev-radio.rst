.. SPDX-License-Identifier: GFDL-1.1-no-invariants-or-later
.. c:namespace:: V4L

.. _radio:

***************************
Radio Interface(无线电接口)
***************************

This interface is intended for AM and FM (analog) radio receivers and
transmitters.
(此接口面向调幅 (AM) 和调频 (FM) (模拟) 无线电接收器和发射器。)

Conventionally V4L2 radio devices are accessed through character device
special files named ``/dev/radio`` and ``/dev/radio0`` to
``/dev/radio63`` with major number 81 and minor numbers 64 to 127.
(按照惯例，V4L2 无线电设备通过名为 ``/dev/radio`` 和 ``/dev/radio0`` 到
``/dev/radio63`` 的字符设备特殊文件访问，主设备号为 81，次设备号为 64 到
127。)


Querying Capabilities(查询能力)
===============================

Devices supporting the radio interface set the ``V4L2_CAP_RADIO`` and
``V4L2_CAP_TUNER`` or ``V4L2_CAP_MODULATOR`` flag in the
``capabilities`` field of struct
:c:type:`v4l2_capability` returned by the
:ref:`VIDIOC_QUERYCAP` ioctl. Other combinations of
capability flags are reserved for future extensions.
(支持无线电接口的设备会在 :ref:`VIDIOC_QUERYCAP` ioctl 返回的 struct
:c:type:`v4l2_capability` 的 ``capabilities`` 字段中设置 ``V4L2_CAP_RADIO``
标志以及 ``V4L2_CAP_TUNER`` 或 ``V4L2_CAP_MODULATOR`` 标志。其他能力标志
组合保留用于未来扩展。)


Supplemental Functions(补充功能)
================================

Radio devices can support :ref:`controls <control>`, and must support
the :ref:`tuner or modulator <tuner>` ioctls.
(无线电设备可以支持 :ref:`controls <control>`，并且必须支持
:ref:`tuner or modulator <tuner>` ioctl。)

They do not support the video input or output, audio input or output,
video standard, cropping and scaling, compression and streaming
parameter, or overlay ioctls. All other ioctls and I/O methods are
reserved for future extensions.
(它们不支持视频输入/输出、音频输入/输出、视频制式、裁剪与缩放、压缩与
流参数以及叠加 (overlay) 等 ioctl。所有其他 ioctl 和 I/O 方法保留用于
未来扩展。)


Programming(编程)
=================

Radio devices may have a couple audio controls (as discussed in
:ref:`control`) such as a volume control, possibly custom controls.
Further all radio devices have one tuner or modulator (these are
discussed in :ref:`tuner`) with index number zero to select the radio
frequency and to determine if a monaural or FM stereo program is
received/emitted. Drivers switch automatically between AM and FM
depending on the selected frequency. The
:ref:`VIDIOC_G_TUNER <VIDIOC_G_TUNER>` or
:ref:`VIDIOC_G_MODULATOR <VIDIOC_G_MODULATOR>` ioctl reports the
supported frequency range.
(无线电设备可能有几个音频控件 (如 :ref:`control` 中所述)，例如音量控制，
也可能有自定义控件。此外，所有无线电设备都有一个索引号为零的调谐器或
调制器 (在 :ref:`tuner` 中讨论)，用于选择无线电频率，并确定接收到/发射的是
单声道还是 FM 立体声节目。驱动会根据所选频率在 AM 与 FM 之间自动切换。
:ref:`VIDIOC_G_TUNER <VIDIOC_G_TUNER>` 或
:ref:`VIDIOC_G_MODULATOR <VIDIOC_G_MODULATOR>` ioctl 会报告支持的频率
范围。)
