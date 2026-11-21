.. SPDX-License-Identifier: GFDL-1.1-no-invariants-or-later
.. c:namespace:: V4L

.. _VIDIOC_QUERYSTD:

*********************************************
ioctl VIDIOC_QUERYSTD, VIDIOC_SUBDEV_QUERYSTD
*********************************************

Name(名称)
==========

VIDIOC_QUERYSTD - VIDIOC_SUBDEV_QUERYSTD - Sense the video standard received by the current input
(VIDIOC_QUERYSTD - VIDIOC_SUBDEV_QUERYSTD - 感知当前输入接收到的视频
标准。)

Synopsis(概要)
==============

.. c:macro:: VIDIOC_QUERYSTD

``int ioctl(int fd, VIDIOC_QUERYSTD, v4l2_std_id *argp)``

.. c:macro:: VIDIOC_SUBDEV_QUERYSTD

``int ioctl(int fd, VIDIOC_SUBDEV_QUERYSTD, v4l2_std_id *argp)``

Arguments(参数)
===============

``fd``
    File descriptor returned by :c:func:`open()`.
    (由 :c:func:`open()` 返回的文件描述符。)

``argp``
    Pointer to :c:type:`v4l2_std_id`.
    (指向 :c:type:`v4l2_std_id` 的指针。)

Description(描述)
=================

The hardware may be able to detect the current video standard
automatically. To do so, applications call :ref:`VIDIOC_QUERYSTD` with a
pointer to a :ref:`v4l2_std_id <v4l2-std-id>` type. The driver
stores here a set of candidates, this can be a single flag or a set of
supported standards if for example the hardware can only distinguish
between 50 and 60 Hz systems. If no signal was detected, then the driver
will return V4L2_STD_UNKNOWN. When detection is not possible or fails,
the set must contain all standards supported by the current video input
or output.
(硬件也许能够自动检测当前的视频标准。为此，应用程序以指向
:ref:`v4l2_std_id <v4l2-std-id>` 类型的指针调用
:ref:`VIDIOC_QUERYSTD`。驱动程序在这里存储一组候选标准——可以是单
个标志，也可以是一组受支持的标准，例如当硬件只能区分 50 Hz 与 60 Hz
系统时。如果没有检测到信号，驱动程序将返回 V4L2_STD_UNKNOWN。当无法
检测或检测失败时，该集合必须包含当前视频输入或输出所支持的全部标
准。)

.. note::

   Drivers shall *not* switch the video standard
   automatically if a new video standard is detected. Instead, drivers
   should send the ``V4L2_EVENT_SOURCE_CHANGE`` event (if they support
   this) and expect that userspace will take action by calling
   :ref:`VIDIOC_QUERYSTD`. The reason is that a new video standard can mean
   different buffer sizes as well, and you cannot change buffer sizes on
   the fly. In general, applications that receive the Source Change event
   will have to call :ref:`VIDIOC_QUERYSTD`, and if the detected video
   standard is valid they will have to stop streaming, set the new
   standard, allocate new buffers and start streaming again.
   (驱动程序 *不得* 在检测到新视频标准时自动切换视频标准。相反，驱动
   程序应发送 ``V4L2_EVENT_SOURCE_CHANGE`` 事件 (如果支持的话)，并期
   望用户空间通过调用 :ref:`VIDIOC_QUERYSTD` 来采取行动。原因在于新
   的视频标准也可能意味着不同的缓冲区大小，而缓冲区大小无法在运行中
   更改。一般而言，接收到 Source Change 事件的应用程序必须调用
   :ref:`VIDIOC_QUERYSTD`，并且如果检测到的视频标准有效，它们必须停
   止流传输、设置新标准、分配新缓冲区，然后重新开始流传输。)

Return Value(返回值)
====================

On success 0 is returned, on error -1 and the ``errno`` variable is set
appropriately. The generic error codes are described at the
:ref:`Generic Error Codes <gen-errors>` chapter.
(成功时返回 0，出错时返回 -1 并适当设置 ``errno`` 变量。通用错误码在
:ref:`Generic Error Codes <gen-errors>` 一章中描述。)

ENODATA
    Standard video timings are not supported for this input or output.
    (此输入或输出不支持标准视频时序。)
