.. SPDX-License-Identifier: GFDL-1.1-no-invariants-or-later
.. c:namespace:: V4L

.. _dmabuf:

**************************************************************
Streaming I/O (DMA buffer importing)(流式 I/O(DMA 缓冲区导入))
**************************************************************

The DMABUF framework provides a generic method for sharing buffers
between multiple devices. Device drivers that support DMABUF can export
a DMA buffer to userspace as a file descriptor (known as the exporter
role), import a DMA buffer from userspace using a file descriptor
previously exported for a different or the same device (known as the
importer role), or both. This section describes the DMABUF importer role
API in V4L2.
(DMABUF 框架提供了一种在多个设备之间共享缓冲区的通用方法。支持 DMABUF 的设备驱动可以把 DMA 缓冲区作为文件描述符导出到用户空间(即导出者 (exporter) 角色)，或者通过使用先前为不同或相同设备导出的文件描述符从用户空间导入 DMA 缓冲区(即导入者 (importer) 角色)，或两者兼而有之。本节描述 V4L2 中的 DMABUF 导入者角色 API。)

Refer to :ref:`DMABUF exporting <VIDIOC_EXPBUF>` for details about
exporting V4L2 buffers as DMABUF file descriptors.
(关于把 V4L2 缓冲区导出为 DMABUF 文件描述符的细节，请参阅 :ref:`DMABUF exporting <VIDIOC_EXPBUF>` 。)

Input and output devices support the streaming I/O method when the
``V4L2_CAP_STREAMING`` flag in the ``capabilities`` field of struct
:c:type:`v4l2_capability` returned by the
:ref:`VIDIOC_QUERYCAP <VIDIOC_QUERYCAP>` ioctl is set. Whether
importing DMA buffers through DMABUF file descriptors is supported is
determined by calling the :ref:`VIDIOC_REQBUFS <VIDIOC_REQBUFS>`
ioctl with the memory type set to ``V4L2_MEMORY_DMABUF``.
(当 :ref:`VIDIOC_QUERYCAP <VIDIOC_QUERYCAP>` ioctl 返回的 struct :c:type:`v4l2_capability` 的 ``capabilities`` 字段中设置了 ``V4L2_CAP_STREAMING`` 标志时，输入和输出设备支持流式 I/O 方法。是否支持通过 DMABUF 文件描述符导入 DMA 缓冲区，可以通过以内存类型 ``V4L2_MEMORY_DMABUF`` 调用 :ref:`VIDIOC_REQBUFS <VIDIOC_REQBUFS>` ioctl 来确定。)

This I/O method is dedicated to sharing DMA buffers between different
devices, which may be V4L devices or other video-related devices (e.g.
DRM). Buffers (planes) are allocated by a driver on behalf of an
application. Next, these buffers are exported to the application as file
descriptors using an API which is specific for an allocator driver. Only
such file descriptor are exchanged. The descriptors and meta-information
are passed in struct :c:type:`v4l2_buffer` (or in struct
:c:type:`v4l2_plane` in the multi-planar API case). The
driver must be switched into DMABUF I/O mode by calling the
:ref:`VIDIOC_REQBUFS <VIDIOC_REQBUFS>` with the desired buffer type.
(这种 I/O 方法专用于在不同设备之间共享 DMA 缓冲区，这些设备可以是 V4L 设备或其他与视频相关的设备(例如 DRM)。缓冲区(平面)由驱动代表应用程序分配。然后，使用分配器驱动特定的 API 把这些缓冲区作为文件描述符导出给应用程序。只交换这样的文件描述符。描述符和元信息通过 struct :c:type:`v4l2_buffer` 传递(在多平面 API 的情况下为 struct :c:type:`v4l2_plane`)。必须通过以所需的缓冲区类型调用 :ref:`VIDIOC_REQBUFS <VIDIOC_REQBUFS>` 把驱动切换到 DMABUF I/O 模式。)

Example: Initiating streaming I/O with DMABUF file descriptors(示例: 使用 DMABUF 文件描述符发起流式 I/O)
========================================================================================================

.. code-block:: c

    struct v4l2_requestbuffers reqbuf;

    memset(&reqbuf, 0, sizeof (reqbuf));
    reqbuf.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
    reqbuf.memory = V4L2_MEMORY_DMABUF;
    reqbuf.count = 1;

    if (ioctl(fd, VIDIOC_REQBUFS, &reqbuf) == -1) {
	if (errno == EINVAL)
	    printf("Video capturing or DMABUF streaming is not supported\\n");
	else
	    perror("VIDIOC_REQBUFS");

	exit(EXIT_FAILURE);
    }

The buffer (plane) file descriptor is passed on the fly with the
:ref:`VIDIOC_QBUF <VIDIOC_QBUF>` ioctl. In case of multiplanar
buffers, every plane can be associated with a different DMABUF
descriptor. Although buffers are commonly cycled, applications can pass
a different DMABUF descriptor at each :ref:`VIDIOC_QBUF <VIDIOC_QBUF>` call.
(缓冲区(平面)文件描述符在调用 :ref:`VIDIOC_QBUF <VIDIOC_QBUF>` ioctl 时即时传递。对于多平面 (multiplanar) 缓冲区，每个平面可以关联不同的 DMABUF 描述符。虽然缓冲区通常是循环使用的，应用程序也可以在每次 :ref:`VIDIOC_QBUF <VIDIOC_QBUF>` 调用时传递不同的 DMABUF 描述符。)

Example: Queueing DMABUF using single plane API(示例: 使用单平面 API 排队 DMABUF)
=================================================================================

.. code-block:: c

    int buffer_queue(int v4lfd, int index, int dmafd)
    {
	struct v4l2_buffer buf;

	memset(&buf, 0, sizeof buf);
	buf.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
	buf.memory = V4L2_MEMORY_DMABUF;
	buf.index = index;
	buf.m.fd = dmafd;

	if (ioctl(v4lfd, VIDIOC_QBUF, &buf) == -1) {
	    perror("VIDIOC_QBUF");
	    return -1;
	}

	return 0;
    }

Example 3.6. Queueing DMABUF using multi plane API(示例 3.6. 使用多平面 API 排队 DMABUF)
========================================================================================

.. code-block:: c

    int buffer_queue_mp(int v4lfd, int index, int dmafd[], int n_planes)
    {
	struct v4l2_buffer buf;
	struct v4l2_plane planes[VIDEO_MAX_PLANES];
	int i;

	memset(&buf, 0, sizeof buf);
	buf.type = V4L2_BUF_TYPE_VIDEO_CAPTURE_MPLANE;
	buf.memory = V4L2_MEMORY_DMABUF;
	buf.index = index;
	buf.m.planes = planes;
	buf.length = n_planes;

	memset(&planes, 0, sizeof planes);

	for (i = 0; i < n_planes; ++i)
	    buf.m.planes[i].m.fd = dmafd[i];

	if (ioctl(v4lfd, VIDIOC_QBUF, &buf) == -1) {
	    perror("VIDIOC_QBUF");
	    return -1;
	}

	return 0;
    }

Captured or displayed buffers are dequeued with the
:ref:`VIDIOC_DQBUF <VIDIOC_QBUF>` ioctl. The driver can unlock the
buffer at any time between the completion of the DMA and this ioctl. The
memory is also unlocked when
:ref:`VIDIOC_STREAMOFF <VIDIOC_STREAMON>` is called,
:ref:`VIDIOC_REQBUFS <VIDIOC_REQBUFS>`, or when the device is closed.
(已捕获或已显示的缓冲区用 :ref:`VIDIOC_DQBUF <VIDIOC_QBUF>` ioctl 出队。驱动可以在 DMA 完成与此 ioctl 之间的任何时刻解锁缓冲区。当调用 :ref:`VIDIOC_STREAMOFF <VIDIOC_STREAMON>` 、 :ref:`VIDIOC_REQBUFS <VIDIOC_REQBUFS>` 或设备被关闭时，内存也会被解锁。)

For capturing applications it is customary to enqueue a number of empty
buffers, to start capturing and enter the read loop. Here the
application waits until a filled buffer can be dequeued, and re-enqueues
the buffer when the data is no longer needed. Output applications fill
and enqueue buffers, when enough buffers are stacked up output is
started. In the write loop, when the application runs out of free
buffers it must wait until an empty buffer can be dequeued and reused.
Two methods exist to suspend execution of the application until one or
more buffers can be dequeued. By default :ref:`VIDIOC_DQBUF
<VIDIOC_QBUF>` blocks when no buffer is in the outgoing queue. When the
``O_NONBLOCK`` flag was given to the :c:func:`open()` function,
:ref:`VIDIOC_DQBUF <VIDIOC_QBUF>` returns immediately with an ``EAGAIN``
error code when no buffer is available. The
:c:func:`select()` and :c:func:`poll()`
functions are always available.
(对于捕获应用程序，习惯做法是先把若干空缓冲区入队，然后开始捕获并进入读取循环。在这里，应用程序等待直到可以出队一个已填充的缓冲区，并在不再需要数据时将该缓冲区重新入队。输出应用程序填充并入队缓冲区，当缓冲了足够多的缓冲区后开始输出。在写入循环中，当应用程序用完了空闲缓冲区时，它必须等待直到可以出队并复用一个空缓冲区。有两种方法可以挂起应用程序的执行，直到可以出队一个或多个缓冲区。默认情况下，当输出队列中没有缓冲区时， :ref:`VIDIOC_DQBUF <VIDIOC_QBUF>` 会阻塞。当向 :c:func:`open()` 函数传入 ``O_NONBLOCK`` 标志时，如果没有可用的缓冲区， :ref:`VIDIOC_DQBUF <VIDIOC_QBUF>` 会立即返回并给出 ``EAGAIN`` 错误码。 :c:func:`select()` 和 :c:func:`poll()` 函数始终可用。)

To start and stop capturing or displaying applications call the
:ref:`VIDIOC_STREAMON <VIDIOC_STREAMON>` and
:ref:`VIDIOC_STREAMOFF <VIDIOC_STREAMON>` ioctls.
(要开始和停止捕获或显示，应用程序调用 :ref:`VIDIOC_STREAMON <VIDIOC_STREAMON>` 和 :ref:`VIDIOC_STREAMOFF <VIDIOC_STREAMON>` ioctl。)

.. note::

   :ref:`VIDIOC_STREAMOFF <VIDIOC_STREAMON>` removes all buffers from
   both queues and unlocks all buffers as a side effect. Since there is no
   notion of doing anything "now" on a multitasking system, if an
   application needs to synchronize with another event it should examine
   the struct :c:type:`v4l2_buffer` ``timestamp`` of captured or
   outputted buffers.
   ( :ref:`VIDIOC_STREAMOFF <VIDIOC_STREAMON>` 作为副作用会从两个队列中
   移除所有缓冲区并解锁所有缓冲区。由于在多任务系统中不存在'立即'做某
   事的概念，如果应用程序需要与另一个事件同步，它应该检查已捕获或已输
   出缓冲区的 struct :c:type:`v4l2_buffer` 的 ``timestamp`` 。)

Drivers implementing DMABUF importing I/O must support the
:ref:`VIDIOC_REQBUFS <VIDIOC_REQBUFS>`, :ref:`VIDIOC_QBUF <VIDIOC_QBUF>`,
:ref:`VIDIOC_DQBUF <VIDIOC_QBUF>`, :ref:`VIDIOC_STREAMON
<VIDIOC_STREAMON>` and :ref:`VIDIOC_STREAMOFF <VIDIOC_STREAMON>` ioctls,
and the :c:func:`select()` and :c:func:`poll()`
functions.
(实现 DMABUF 导入 I/O 的驱动必须支持 :ref:`VIDIOC_REQBUFS <VIDIOC_REQBUFS>` 、 :ref:`VIDIOC_QBUF <VIDIOC_QBUF>` 、 :ref:`VIDIOC_DQBUF <VIDIOC_QBUF>` 、 :ref:`VIDIOC_STREAMON <VIDIOC_STREAMON>` 和 :ref:`VIDIOC_STREAMOFF <VIDIOC_STREAMON>` ioctl，以及 :c:func:`select()` 和 :c:func:`poll()` 函数。)
