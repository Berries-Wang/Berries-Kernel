.. SPDX-License-Identifier: GFDL-1.1-no-invariants-or-later
.. c:namespace:: V4L

.. _io:

#############
Input/Output(输入/输出)
#############
The V4L2 API defines several different methods to read from or write to
a device. All drivers exchanging data with applications must support at
least one of them.
(V4L2 API 定义了若干种从设备读取或向设备写入的不同方法。所有与应用交换
数据的驱动必须至少支持其中一种。)

The classic I/O method using the :c:func:`read()` and
:c:func:`write()` function is automatically selected after opening a
V4L2 device. When the driver does not support this method attempts to
read or write will fail at any time.
(使用 :c:func:`read()` 和 :c:func:`write()` 函数的经典 I/O 方法会在打开
V4L2 设备后自动选定。当驱动不支持此方法时，读取或写入的尝试在任何时候都
会失败。)

Other methods must be negotiated. To select the streaming I/O method
with memory mapped or user buffers applications call the
:ref:`VIDIOC_REQBUFS` ioctl.
(其他方法必须经过协商。要选择使用内存映射缓冲区或用户缓冲区的流式 I/O
方法，应用调用 :ref:`VIDIOC_REQBUFS` ioctl。)

Video overlay can be considered another I/O method, although the
application does not directly receive the image data. It is selected by
initiating video overlay with the :ref:`VIDIOC_S_FMT <VIDIOC_G_FMT>`
ioctl. For more information see :ref:`overlay`.
(视频叠加可以视为另一种 I/O 方法，尽管应用并不直接接收图像数据。通过用
:ref:`VIDIOC_S_FMT <VIDIOC_G_FMT>` ioctl 发起视频叠加来选定它。更多信息
参见 :ref:`overlay` 。)

Generally exactly one I/O method, including overlay, is associated with
each file descriptor. The only exceptions are applications not
exchanging data with a driver ("panel applications", see :ref:`open`)
and drivers permitting simultaneous video capturing and overlay using
the same file descriptor, for compatibility with V4L and earlier
versions of V4L2.
(通常，包括叠加在内，每个文件描述符恰好关联一种 I/O 方法。仅有的例外是
不与驱动交换数据的应用 ("面板应用"，参见 :ref:`open`)，以及出于与 V4L 和
V4L2 早期版本兼容的考虑、允许在同一文件描述符上同时进行视频采集与叠加的
驱动。)

:ref:`VIDIOC_S_FMT <VIDIOC_G_FMT>` and :ref:`VIDIOC_REQBUFS` would permit this to some
degree, but for simplicity drivers need not support switching the I/O
method (after first switching away from read/write) other than by
closing and reopening the device.
(:ref:`VIDIOC_S_FMT <VIDIOC_G_FMT>` 和 :ref:`VIDIOC_REQBUFS` 在一定程度上
允许这样做，但为简单起见，除关闭并重新打开设备之外，驱动不必支持切换
I/O 方法 (在第一次从读/写切换走之后)。)

The following sections describe the various I/O methods in more detail.
(以下各节更详细地描述各种 I/O 方法。)

.. toctree::
    :maxdepth: 1

    rw
    mmap
    userp
    dmabuf
    buffer
    field-order
