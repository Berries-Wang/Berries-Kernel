.. SPDX-License-Identifier: GFDL-1.1-no-invariants-or-later
.. c:namespace:: V4L

.. _VIDIOC_QUERY_DV_TIMINGS:

*****************************
ioctl VIDIOC_QUERY_DV_TIMINGS
*****************************

Name(名称)
==========

VIDIOC_QUERY_DV_TIMINGS - VIDIOC_SUBDEV_QUERY_DV_TIMINGS - Sense the DV preset received by the current input
(VIDIOC_QUERY_DV_TIMINGS - VIDIOC_SUBDEV_QUERY_DV_TIMINGS - 感知当前输入接收到的 DV 预设。)

Synopsis(概要)
==============

.. c:macro:: VIDIOC_QUERY_DV_TIMINGS

``int ioctl(int fd, VIDIOC_QUERY_DV_TIMINGS, struct v4l2_dv_timings *argp)``

.. c:macro:: VIDIOC_SUBDEV_QUERY_DV_TIMINGS

``int ioctl(int fd, VIDIOC_SUBDEV_QUERY_DV_TIMINGS, struct v4l2_dv_timings *argp)``

Arguments(参数)
===============

``fd``
    File descriptor returned by :c:func:`open()`.
    (由 :c:func:`open()` 返回的文件描述符。)

``argp``
    Pointer to struct :c:type:`v4l2_dv_timings`.
    (指向 struct :c:type:`v4l2_dv_timings` 的指针。)

Description(描述)
=================

The hardware may be able to detect the current DV timings automatically,
similar to sensing the video standard. To do so, applications call
:ref:`VIDIOC_QUERY_DV_TIMINGS` with a pointer to a struct
:c:type:`v4l2_dv_timings`. Once the hardware detects
the timings, it will fill in the timings structure.
(硬件也许能够自动检测当前的 DV 时序，类似于感知视频标准。为此，应用
程序以指向 struct :c:type:`v4l2_dv_timings` 的指针调用
:ref:`VIDIOC_QUERY_DV_TIMINGS`。一旦硬件检测到时序，就会填充该时序
结构。)

.. note::

   Drivers shall *not* switch timings automatically if new
   timings are detected. Instead, drivers should send the
   ``V4L2_EVENT_SOURCE_CHANGE`` event (if they support this) and expect
   that userspace will take action by calling :ref:`VIDIOC_QUERY_DV_TIMINGS`.
   The reason is that new timings usually mean different buffer sizes as
   well, and you cannot change buffer sizes on the fly. In general,
   applications that receive the Source Change event will have to call
   :ref:`VIDIOC_QUERY_DV_TIMINGS`, and if the detected timings are valid they
   will have to stop streaming, set the new timings, allocate new buffers
   and start streaming again.
   (驱动程序 *不得* 在检测到新时序时自动切换时序。相反，驱动程序应发送
   ``V4L2_EVENT_SOURCE_CHANGE`` 事件 (如果支持的话)，并期望用户空间通过
   调用 :ref:`VIDIOC_QUERY_DV_TIMINGS` 来采取行动。原因在于新时序通常
   也意味着不同的缓冲区大小，而缓冲区大小无法在运行中更改。一般而言，
   接收到 Source Change 事件的应用程序必须调用
   :ref:`VIDIOC_QUERY_DV_TIMINGS`，并且如果检测到的时序有效，它们必须
   停止流传输、设置新时序、分配新缓冲区，然后重新开始流传输。)

If the timings could not be detected because there was no signal, then
ENOLINK is returned. If a signal was detected, but it was unstable and
the receiver could not lock to the signal, then ``ENOLCK`` is returned. If
the receiver could lock to the signal, but the format is unsupported
(e.g. because the pixelclock is out of range of the hardware
capabilities), then the driver fills in whatever timings it could find
and returns ``ERANGE``. In that case the application can call
:ref:`VIDIOC_DV_TIMINGS_CAP` to compare the
found timings with the hardware's capabilities in order to give more
precise feedback to the user.
(如果因为没有信号而无法检测时序，则返回 ENOLINK。如果检测到了信号，但
信号不稳定、接收器无法锁定，则返回 ``ENOLCK``。如果接收器能够锁定信号，
但格式不受支持 (例如因为像素时钟超出了硬件能力范围)，则驱动程序会填入
它能找到的任何时序并返回 ``ERANGE``。在这种情况下，应用程序可以调用
:ref:`VIDIOC_DV_TIMINGS_CAP`，把找到的时序与硬件的能力进行比较，以便向
用户给出更精确的反馈。)

Return Value(返回值)
====================

On success 0 is returned, on error -1 and the ``errno`` variable is set
appropriately. The generic error codes are described at the
:ref:`Generic Error Codes <gen-errors>` chapter.
(成功时返回 0，出错时返回 -1 并适当设置 ``errno`` 变量。通用错误码在
:ref:`Generic Error Codes <gen-errors>` 一章中描述。)

ENODATA
    Digital video timings are not supported for this input or output.
    (此输入或输出不支持数字视频时序。)

ENOLINK
    No timings could be detected because no signal was found.
    (由于没有找到信号，无法检测到任何时序。)

ENOLCK
    The signal was unstable and the hardware could not lock on to it.
    (信号不稳定，硬件无法锁定到该信号。)

ERANGE
    Timings were found, but they are out of range of the hardware
    capabilities.
    (找到了时序，但超出了硬件能力的范围。)
