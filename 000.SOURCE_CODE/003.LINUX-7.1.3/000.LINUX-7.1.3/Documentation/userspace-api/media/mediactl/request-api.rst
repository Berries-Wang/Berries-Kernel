.. SPDX-License-Identifier: GPL-2.0 OR GFDL-1.1-no-invariants-or-later
.. c:namespace:: MC

.. _media-request-api:

Request API(请求 API)
=====================

The Request API has been designed to allow V4L2 to deal with requirements of
modern devices (stateless codecs, complex camera pipelines, ...) and APIs
(Android Codec v2). One such requirement is the ability for devices belonging to
the same pipeline to reconfigure and collaborate closely on a per-frame basis.
Another is support of stateless codecs, which require controls to be applied
to specific frames (aka 'per-frame controls') in order to be used efficiently.
(Request API 的设计目的是让 V4L2 能够应对现代设备(无状态编解码器、复杂的相机
管线等)与 API(Android Codec v2)的需求。这类需求之一是：属于同一管线的设备
能够以每帧为单位重新配置并紧密协作。另一个是对无状态编解码器的支持——为了
高效使用这类编解码器，需要把控制应用到特定帧(即所谓"按帧控制" (per-frame
controls))。)

While the initial use-case was V4L2, it can be extended to other subsystems
as well, as long as they use the media controller.
(虽然最初的使用场景是 V4L2，但它同样可以扩展到其他子系统，只要这些子系统使用
media controller。)

Supporting these features without the Request API is not always possible and if
it is, it is terribly inefficient: user-space would have to flush all activity
on the media pipeline, reconfigure it for the next frame, queue the buffers to
be processed with that configuration, and wait until they are all available for
dequeuing before considering the next frame. This defeats the purpose of having
buffer queues since in practice only one buffer would be queued at a time.
(在没有 Request API 的情况下，并不总能支持这些特性；即便能支持，效率也极低：
用户空间必须冲刷 media pipeline 上的所有活动，为下一帧重新配置它，把缓冲区按
该配置排队以待处理，并等待它们全部可供出队之后才考虑下一帧。这违背了设置缓冲
区队列的初衷，因为实际上一次只能有一个缓冲区在队列中。)

The Request API allows a specific configuration of the pipeline (media
controller topology + configuration for each media entity) to be associated with
specific buffers. This allows user-space to schedule several tasks ("requests")
with different configurations in advance, knowing that the configuration will be
applied when needed to get the expected result. Configuration values at the time
of request completion are also available for reading.
(Request API 允许把管线的特定配置(media controller 拓扑 + 每个 media entity
的配置)与特定缓冲区关联起来。这使得用户空间可以提前调度多个具有不同配置的
任务("请求")，并确信配置会在需要时得到应用，从而获得预期结果。请求完成时刻
的配置值也可以读取。)

General Usage(一般用法)
-----------------------

The Request API extends the Media Controller API and cooperates with
subsystem-specific APIs to support request usage. At the Media Controller
level, requests are allocated from the supporting Media Controller device
node. Their life cycle is then managed through the request file descriptors in
an opaque way. Configuration data, buffer handles and processing results
stored in requests are accessed through subsystem-specific APIs extended for
request support, such as V4L2 APIs that take an explicit ``request_fd``
parameter.
(Request API 扩展了 Media Controller API，并与各子系统专有的 API 协作以支持
请求的使用。在 Media Controller 层面，请求从支持该功能的 Media Controller
设备节点分配。其生命周期随后通过请求文件描述符以不透明的方式管理。存储在请求
中的配置数据、缓冲区句柄和处理结果，通过为支持请求而扩展的子系统专有 API
访问，例如接受显式 ``request_fd`` 参数的 V4L2 API。)

Request Allocation(请求分配)
----------------------------

User-space allocates requests using :ref:`MEDIA_IOC_REQUEST_ALLOC`
for the media device node. This returns a file descriptor representing the
request. Typically, several such requests will be allocated.
(用户空间使用 :ref:`MEDIA_IOC_REQUEST_ALLOC` 为 media 设备节点分配请求。该
调用返回一个代表请求的文件描述符。通常会分配若干个这样的请求。)

Request Preparation(请求准备)
-----------------------------

Standard V4L2 ioctls can then receive a request file descriptor to express the
fact that the ioctl is part of said request, and is not to be applied
immediately. See :ref:`MEDIA_IOC_REQUEST_ALLOC` for a list of ioctls that
support this. Configurations set with a ``request_fd`` parameter are stored
instead of being immediately applied, and buffers queued to a request do not
enter the regular buffer queue until the request itself is queued.
(标准 V4L2 ioctl 随后可以接收一个请求文件描述符，以表示该 ioctl 属于上述请求
的一部分，不应立即应用。支持此功能的 ioctl 列表见
:ref:`MEDIA_IOC_REQUEST_ALLOC`。带 ``request_fd`` 参数设置的配置会被存储而
不是立即应用；排入请求的缓冲区在该请求本身入队之前不会进入常规缓冲区队列。)

Request Submission(请求提交)
----------------------------

Once the configuration and buffers of the request are specified, it can be
queued by calling :ref:`MEDIA_REQUEST_IOC_QUEUE` on the request file descriptor.
A request must contain at least one buffer, otherwise ``ENOENT`` is returned.
A queued request cannot be modified anymore.
(一旦请求的配置和缓冲区都已指定，就可以通过在请求文件描述符上调用
:ref:`MEDIA_REQUEST_IOC_QUEUE` 将其入队。请求必须至少包含一个缓冲区，否则返回
``ENOENT``。已入队的请求不能再被修改。)

.. caution::
   For :ref:`memory-to-memory devices <mem2mem>` you can use requests only for
   output buffers, not for capture buffers. Attempting to add a capture buffer
   to a request will result in an ``EBADR`` error.
   (对 :ref:`memory-to-memory devices <mem2mem>` 设备而言，请求只能用于输出
   缓冲区，不能用于捕获缓冲区。试图把捕获缓冲区加入请求会导致 ``EBADR``
   错误。)

If the request contains configurations for multiple entities, individual drivers
may synchronize so the requested pipeline's topology is applied before the
buffers are processed. Media controller drivers do a best effort implementation
since perfect atomicity may not be possible due to hardware limitations.
(如果请求包含针对多个实体的配置，各驱动程序可以进行同步，使所请求的管线拓扑
在缓冲区被处理之前得到应用。Media controller 驱动程序尽力实现这一点，因为受
硬件限制，完美的原子性可能无法做到。)

.. caution::

   It is not allowed to mix queuing requests with directly queuing buffers:
   whichever method is used first locks this in place until
   :ref:`VIDIOC_STREAMOFF <VIDIOC_STREAMON>` is called or the device is
   :ref:`closed <func-close>`. Attempts to directly queue a buffer when earlier
   a buffer was queued via a request or vice versa will result in an ``EBUSY``
   error.
   (不允许把请求入队与直接排队缓冲区混用：无论先使用哪种方法，这一选择都会
   固定下来，直到调用 :ref:`VIDIOC_STREAMOFF <VIDIOC_STREAMON>` 或设备被
   :ref:`closed <func-close>`。如果之前通过请求排队了缓冲区，之后又试图直接
   排队缓冲区(反之亦然)，会导致 ``EBUSY`` 错误。)

Controls can still be set without a request and are applied immediately,
regardless of whether a request is in use or not.
(不通过请求也可以设置控制，并且会立即应用，无论是否正在使用请求。)

.. caution::

   Setting the same control through a request and also directly can lead to
   undefined behavior!
   (通过请求设置同一控制的同时又直接设置，可能导致未定义行为！)

User-space can :c:func:`poll()` a request file descriptor in
order to wait until the request completes. A request is considered complete
once all its associated buffers are available for dequeuing and all the
associated controls have been updated with the values at the time of completion.
Note that user-space does not need to wait for the request to complete to
dequeue its buffers: buffers that are available halfway through a request can
be dequeued independently of the request's state.
(用户空间可以对请求文件描述符调用 :c:func:`poll()`，以等待请求完成。一旦其
所有关联缓冲区都可供出队、且所有关联控制都已更新为完成时刻的值，该请求即被
视为完成。注意，用户空间无需等待请求完成即可出队其缓冲区：在请求进行到一半时
就已可用的缓冲区，可以独立于请求的状态出队。)

A completed request contains the state of the device after the request was
executed. User-space can query that state by calling
:ref:`ioctl VIDIOC_G_EXT_CTRLS <VIDIOC_G_EXT_CTRLS>` with the request file
descriptor. Calling :ref:`ioctl VIDIOC_G_EXT_CTRLS <VIDIOC_G_EXT_CTRLS>` for a
request that has been queued but not yet completed will return ``EBUSY``
since the control values might be changed at any time by the driver while the
request is in flight.
(已完成的请求包含请求执行之后的设备状态。用户空间可以带上请求文件描述符调用
:ref:`ioctl VIDIOC_G_EXT_CTRLS <VIDIOC_G_EXT_CTRLS>` 来查询该状态。对已入队但
尚未完成的请求调用
:ref:`ioctl VIDIOC_G_EXT_CTRLS <VIDIOC_G_EXT_CTRLS>` 会返回 ``EBUSY``，因为
在请求执行期间，驱动程序随时可能改变控制的值。)

.. _media-request-life-time:

Recycling and Destruction(回收与销毁)
-------------------------------------

Finally, a completed request can either be discarded or be reused. Calling
:c:func:`close()` on a request file descriptor will make
that file descriptor unusable and the request will be freed once it is no
longer in use by the kernel. That is, if the request is queued and then the
file descriptor is closed, then it won't be freed until the driver completed
the request.
(最后，已完成的请求可以被丢弃或重用。对请求文件描述符调用 :c:func:`close()`
会使该文件描述符失效；一旦内核不再使用该请求，它就会被释放。也就是说，如果
请求已入队然后关闭了文件描述符，那么在驱动程序完成该请求之前它不会被释放。)

The :ref:`MEDIA_REQUEST_IOC_REINIT` will clear a request's state and make it
available again. No state is retained by this operation: the request is as
if it had just been allocated.
(:ref:`MEDIA_REQUEST_IOC_REINIT` 会清除请求的状态并使其重新可用。此操作不保留
任何状态：请求就像刚分配时一样。)

Example for a Codec Device(编解码器设备示例)
--------------------------------------------

For use-cases such as :ref:`codecs <mem2mem>`, the request API can be used
to associate specific controls to
be applied by the driver for the OUTPUT buffer, allowing user-space
to queue many such buffers in advance. It can also take advantage of requests'
ability to capture the state of controls when the request completes to read back
information that may be subject to change.
(对于 :ref:`codecs <mem2mem>` 之类的使用场景，request API 可用于把特定的控制
关联起来，由驱动程序在处理 OUTPUT 缓冲区时应用，从而让用户空间可以提前排队
许多这样的缓冲区。它还可以利用请求在完成时捕获控制状态的能力，读回可能发生
变化的信息。)

Put into code, after obtaining a request, user-space can assign controls and one
OUTPUT buffer to it:
(用代码表示：获得请求之后，用户空间可以把控制和一个 OUTPUT 缓冲区指派给它：)

.. code-block:: c

	struct v4l2_buffer buf;
	struct v4l2_ext_controls ctrls;
	int req_fd;
	...
	if (ioctl(media_fd, MEDIA_IOC_REQUEST_ALLOC, &req_fd))
		return errno;
	...
	ctrls.which = V4L2_CTRL_WHICH_REQUEST_VAL;
	ctrls.request_fd = req_fd;
	if (ioctl(codec_fd, VIDIOC_S_EXT_CTRLS, &ctrls))
		return errno;
	...
	buf.type = V4L2_BUF_TYPE_VIDEO_OUTPUT;
	buf.flags |= V4L2_BUF_FLAG_REQUEST_FD;
	buf.request_fd = req_fd;
	if (ioctl(codec_fd, VIDIOC_QBUF, &buf))
		return errno;

Note that it is not allowed to use the Request API for CAPTURE buffers
since there are no per-frame settings to report there.
(注意，不允许对 CAPTURE 缓冲区使用 Request API，因为那里没有需要上报的按帧
设置。)

Once the request is fully prepared, it can be queued to the driver:
(请求完全准备好之后，就可以把它入队给驱动程序：)

.. code-block:: c

	if (ioctl(req_fd, MEDIA_REQUEST_IOC_QUEUE))
		return errno;

User-space can then either wait for the request to complete by calling poll() on
its file descriptor, or start dequeuing CAPTURE buffers. Most likely, it will
want to get CAPTURE buffers as soon as possible and this can be done using a
regular :ref:`VIDIOC_DQBUF <VIDIOC_QBUF>`:
(之后，用户空间既可以通过对其文件描述符调用 poll() 等待请求完成，也可以开始
出队 CAPTURE 缓冲区。最可能的情况是，它希望尽快拿到 CAPTURE 缓冲区，这可以用
常规的 :ref:`VIDIOC_DQBUF <VIDIOC_QBUF>` 完成：)

.. code-block:: c

	struct v4l2_buffer buf;

	memset(&buf, 0, sizeof(buf));
	buf.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
	if (ioctl(codec_fd, VIDIOC_DQBUF, &buf))
		return errno;

Note that this example assumes for simplicity that for every OUTPUT buffer
there will be one CAPTURE buffer, but this does not have to be the case.
(注意，为简单起见，本例假定每个 OUTPUT 缓冲区都对应一个 CAPTURE 缓冲区，但
实际情况未必如此。)

We can then, after ensuring that the request is completed via polling the
request file descriptor, query control values at the time of its completion via
a call to :ref:`VIDIOC_G_EXT_CTRLS <VIDIOC_G_EXT_CTRLS>`.
This is particularly useful for volatile controls for which we want to
query values as soon as the capture buffer is produced.
(随后，在通过轮询请求文件描述符确认请求已完成之后，可以通过调用
:ref:`VIDIOC_G_EXT_CTRLS <VIDIOC_G_EXT_CTRLS>` 查询请求完成时刻的控制值。
这对易失 (volatile) 控制特别有用——我们希望在捕获缓冲区产生后尽快查询其
值。)

.. code-block:: c

	struct pollfd pfd = { .events = POLLPRI, .fd = req_fd };
	poll(&pfd, 1, -1);
	...
	ctrls.which = V4L2_CTRL_WHICH_REQUEST_VAL;
	ctrls.request_fd = req_fd;
	if (ioctl(codec_fd, VIDIOC_G_EXT_CTRLS, &ctrls))
		return errno;

Once we don't need the request anymore, we can either recycle it for reuse with
:ref:`MEDIA_REQUEST_IOC_REINIT`...
(当不再需要请求时，可以用 :ref:`MEDIA_REQUEST_IOC_REINIT` 回收它以便重用……)

.. code-block:: c

	if (ioctl(req_fd, MEDIA_REQUEST_IOC_REINIT))
		return errno;

... or close its file descriptor to completely dispose of it.
(……或者关闭其文件描述符以彻底处置它。)

.. code-block:: c

	close(req_fd);

Example for a Simple Capture Device(简单捕获设备示例)
-----------------------------------------------------

With a simple capture device, requests can be used to specify controls to apply
for a given CAPTURE buffer.
(对于简单的捕获设备，可以用请求来指定要应用于某个 CAPTURE 缓冲区的控制。)

.. code-block:: c

	struct v4l2_buffer buf;
	struct v4l2_ext_controls ctrls;
	int req_fd;
	...
	if (ioctl(media_fd, MEDIA_IOC_REQUEST_ALLOC, &req_fd))
		return errno;
	...
	ctrls.which = V4L2_CTRL_WHICH_REQUEST_VAL;
	ctrls.request_fd = req_fd;
	if (ioctl(camera_fd, VIDIOC_S_EXT_CTRLS, &ctrls))
		return errno;
	...
	buf.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
	buf.flags |= V4L2_BUF_FLAG_REQUEST_FD;
	buf.request_fd = req_fd;
	if (ioctl(camera_fd, VIDIOC_QBUF, &buf))
		return errno;

Once the request is fully prepared, it can be queued to the driver:
(请求完全准备好之后，就可以把它入队给驱动程序：)

.. code-block:: c

	if (ioctl(req_fd, MEDIA_REQUEST_IOC_QUEUE))
		return errno;

User-space can then dequeue buffers, wait for the request completion, query
controls and recycle the request as in the M2M example above.
(之后，用户空间可以像上面的 M2M 示例那样出队缓冲区、等待请求完成、查询控制并
回收请求。)
