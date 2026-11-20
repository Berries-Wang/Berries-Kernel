.. SPDX-License-Identifier: GFDL-1.1-no-invariants-or-later
.. c:namespace:: V4L

.. _VIDIOC_QBUF:

*************************************************
ioctl VIDIOC_QBUF, VIDIOC_DQBUF(与驱动交换缓冲区)
*************************************************

Name(名称)
==========

VIDIOC_QBUF - VIDIOC_DQBUF - Exchange a buffer with the driver
(VIDIOC_QBUF - VIDIOC_DQBUF - 与驱动交换缓冲区)

Synopsis(概要)
==============

.. c:macro:: VIDIOC_QBUF

``int ioctl(int fd, VIDIOC_QBUF, struct v4l2_buffer *argp)``

.. c:macro:: VIDIOC_DQBUF

``int ioctl(int fd, VIDIOC_DQBUF, struct v4l2_buffer *argp)``

Arguments(参数)
===============

``fd``
    File descriptor returned by :c:func:`open()`.
    (由 :c:func:`open()` 返回的文件描述符。)

``argp``
    Pointer to struct :c:type:`v4l2_buffer`.
    (指向结构体 :c:type:`v4l2_buffer` 的指针。)

Description(描述)
=================

Applications call the ``VIDIOC_QBUF`` ioctl to enqueue an empty
(capturing) or filled (output) buffer in the driver's incoming queue.
The semantics depend on the selected I/O method.
(应用程序调用 ``VIDIOC_QBUF`` ioctl 将空(采集)或满(输出)缓冲区排入驱动的传入队列。其语义取决于所选的 I/O 方式。)

To enqueue a buffer applications set the ``type`` field of a struct
:c:type:`v4l2_buffer` to the same buffer type as was
previously used with struct :c:type:`v4l2_format` ``type``
and struct :c:type:`v4l2_requestbuffers` ``type``.
Applications must also set the ``index`` field. Valid index numbers
range from zero to the number of buffers allocated with
:ref:`VIDIOC_REQBUFS` (struct
:c:type:`v4l2_requestbuffers` ``count``) minus
one. The contents of the struct :c:type:`v4l2_buffer` returned
by a :ref:`VIDIOC_QUERYBUF` ioctl will do as well.
When the buffer is intended for output (``type`` is
``V4L2_BUF_TYPE_VIDEO_OUTPUT``, ``V4L2_BUF_TYPE_VIDEO_OUTPUT_MPLANE``,
or ``V4L2_BUF_TYPE_VBI_OUTPUT``) applications must also initialize the
``bytesused``, ``field`` and ``timestamp`` fields, see :ref:`buffer`
for details. Applications must also set ``flags`` to 0. The
``reserved2`` and ``reserved`` fields must be set to 0. When using the
:ref:`multi-planar API <planar-apis>`, the ``m.planes`` field must
contain a userspace pointer to a filled-in array of struct
:c:type:`v4l2_plane` and the ``length`` field must be set
to the number of elements in that array.
(入队缓冲区时，应用程序需把结构体 :c:type:`v4l2_buffer` 的 ``type`` 字段设置为之前与结构体 :c:type:`v4l2_format` 的 ``type`` 及结构体 :c:type:`v4l2_requestbuffers` 的 ``type`` 使用过的相同缓冲区类型。应用程序还必须设置 ``index`` 字段。有效索引号范围为 0 到用 :ref:`VIDIOC_REQBUFS` 分配的缓冲区数(结构体 :c:type:`v4l2_requestbuffers` 的 ``count``)减一。由 :ref:`VIDIOC_QUERYBUF` ioctl 返回的结构体 :c:type:`v4l2_buffer` 内容也同样可用。当缓冲区用于输出时(``type`` 为 ``V4L2_BUF_TYPE_VIDEO_OUTPUT``、``V4L2_BUF_TYPE_VIDEO_OUTPUT_MPLANE`` 或 ``V4L2_BUF_TYPE_VBI_OUTPUT``)，应用程序还必须初始化 ``bytesused``、``field`` 与 ``timestamp`` 字段，详见 :ref:`buffer`。应用程序还必须把 ``flags`` 设为 0。``reserved2`` 与 ``reserved`` 字段必须置 0。使用 :ref:`multi-planar API <planar-apis>` 时，``m.planes`` 字段必须包含一个指向已填写好的结构体 :c:type:`v4l2_plane` 数组的用户态指针，且 ``length`` 字段必须设为该数组的元素个数。)

To enqueue a :ref:`memory mapped <mmap>` buffer applications set the
``memory`` field to ``V4L2_MEMORY_MMAP``. When ``VIDIOC_QBUF`` is called
with a pointer to this structure the driver sets the
``V4L2_BUF_FLAG_MAPPED`` and ``V4L2_BUF_FLAG_QUEUED`` flags and clears
the ``V4L2_BUF_FLAG_DONE`` flag in the ``flags`` field, or it returns an
``EINVAL`` error code.
(要入队一个 :ref:`memory mapped <mmap>` 缓冲区，应用程序将 ``memory`` 字段设为 ``V4L2_MEMORY_MMAP``。当以指向该结构的指针调用 ``VIDIOC_QBUF`` 时，驱动会在 ``flags`` 字段中设置 ``V4L2_BUF_FLAG_MAPPED`` 与 ``V4L2_BUF_FLAG_QUEUED`` 标志并清除 ``V4L2_BUF_FLAG_DONE`` 标志，否则返回 ``EINVAL`` 错误码。)

To enqueue a :ref:`user pointer <userp>` buffer applications set the
``memory`` field to ``V4L2_MEMORY_USERPTR``, the ``m.userptr`` field to
the address of the buffer and ``length`` to its size. When the
multi-planar API is used, ``m.userptr`` and ``length`` members of the
passed array of struct :c:type:`v4l2_plane` have to be used
instead. When ``VIDIOC_QBUF`` is called with a pointer to this structure
the driver sets the ``V4L2_BUF_FLAG_QUEUED`` flag and clears the
``V4L2_BUF_FLAG_MAPPED`` and ``V4L2_BUF_FLAG_DONE`` flags in the
``flags`` field, or it returns an error code. This ioctl locks the
memory pages of the buffer in physical memory, they cannot be swapped
out to disk. Buffers remain locked until dequeued, until the
:ref:`VIDIOC_STREAMOFF <VIDIOC_STREAMON>` or
:ref:`VIDIOC_REQBUFS` ioctl is called, or until the
device is closed.
(要入队一个 :ref:`user pointer <userp>` 缓冲区，应用程序将 ``memory`` 字段设为 ``V4L2_MEMORY_USERPTR``，将 ``m.userptr`` 字段设为缓冲区地址，``length`` 设为其大小。使用多平面 API 时，必须改用所传入结构体 :c:type:`v4l2_plane` 数组中的 ``m.userptr`` 与 ``length`` 成员。当以指向该结构的指针调用 ``VIDIOC_QBUF`` 时，驱动会在 ``flags`` 字段中设置 ``V4L2_BUF_FLAG_QUEUED`` 标志并清除 ``V4L2_BUF_FLAG_MAPPED`` 与 ``V4L2_BUF_FLAG_DONE`` 标志，否则返回错误码。该 ioctl 会把缓冲区的内存页锁定在物理内存中，它们不能被换出到磁盘。缓冲区保持锁定状态，直到被出队、调用了 :ref:`VIDIOC_STREAMOFF <VIDIOC_STREAMON>` 或 :ref:`VIDIOC_REQBUFS` ioctl、或设备被关闭为止。)

To enqueue a :ref:`DMABUF <dmabuf>` buffer applications set the
``memory`` field to ``V4L2_MEMORY_DMABUF`` and the ``m.fd`` field to a
file descriptor associated with a DMABUF buffer. When the multi-planar
API is used the ``m.fd`` fields of the passed array of struct
:c:type:`v4l2_plane` have to be used instead. When
``VIDIOC_QBUF`` is called with a pointer to this structure the driver
sets the ``V4L2_BUF_FLAG_QUEUED`` flag and clears the
``V4L2_BUF_FLAG_MAPPED`` and ``V4L2_BUF_FLAG_DONE`` flags in the
``flags`` field, or it returns an error code. This ioctl locks the
buffer. Locking a buffer means passing it to a driver for a hardware
access (usually DMA). If an application accesses (reads/writes) a locked
buffer then the result is undefined. Buffers remain locked until
dequeued, until the :ref:`VIDIOC_STREAMOFF <VIDIOC_STREAMON>` or
:ref:`VIDIOC_REQBUFS` ioctl is called, or until the
device is closed.
(要入队一个 :ref:`DMABUF <dmabuf>` 缓冲区，应用程序将 ``memory`` 字段设为 ``V4L2_MEMORY_DMABUF``，并把 ``m.fd`` 字段设为与某个 DMABUF 缓冲区关联的文件描述符。使用多平面 API 时，必须改用所传入结构体 :c:type:`v4l2_plane` 数组中的 ``m.fd`` 字段。当以指向该结构的指针调用 ``VIDIOC_QBUF`` 时，驱动会在 ``flags`` 字段中设置 ``V4L2_BUF_FLAG_QUEUED`` 标志并清除 ``V4L2_BUF_FLAG_MAPPED`` 与 ``V4L2_BUF_FLAG_DONE`` 标志，否则返回错误码。该 ioctl 会锁定缓冲区。锁定缓冲区意味着把它交给驱动进行硬件访问(通常是 DMA)。如果应用访问(读/写)一个已锁定的缓冲区，则结果未定义。缓冲区保持锁定状态，直到被出队、调用了 :ref:`VIDIOC_STREAMOFF <VIDIOC_STREAMON>` 或 :ref:`VIDIOC_REQBUFS` ioctl、或设备被关闭为止。)

The ``request_fd`` field can be used with the ``VIDIOC_QBUF`` ioctl to specify
the file descriptor of a :ref:`request <media-request-api>`, if requests are
in use. Setting it means that the buffer will not be passed to the driver
until the request itself is queued. Also, the driver will apply any
settings associated with the request for this buffer. This field will
be ignored unless the ``V4L2_BUF_FLAG_REQUEST_FD`` flag is set.
If the device does not support requests, then ``EBADR`` will be returned.
If requests are supported but an invalid request file descriptor is given,
then ``EINVAL`` will be returned.
(在使用 request 的情况下，``request_fd`` 字段可与 ``VIDIOC_QBUF`` ioctl 一起使用，用于指定 :ref:`request <media-request-api>` 的文件描述符。设置它意味着在该 request 本身入队之前，缓冲区不会被传给驱动。此外，驱动还会为该缓冲区应用与该 request 关联的所有设置。除非设置了 ``V4L2_BUF_FLAG_REQUEST_FD`` 标志，否则该字段将被忽略。如果设备不支持 request，则返回 ``EBADR``。如果支持 request 但给定的 request 文件描述符无效，则返回 ``EINVAL``。)

.. caution::
   It is not allowed to mix queuing requests with queuing buffers directly.
   ``EBUSY`` will be returned if the first buffer was queued directly and
   then the application tries to queue a request, or vice versa. After
   closing the file descriptor, calling
   :ref:`VIDIOC_STREAMOFF <VIDIOC_STREAMON>` or calling :ref:`VIDIOC_REQBUFS`
   the check for this will be reset.

   For :ref:`memory-to-memory devices <mem2mem>` you can specify the
   ``request_fd`` only for output buffers, not for capture buffers. Attempting
   to specify this for a capture buffer will result in an ``EBADR`` error.
   (注意：不允许把"通过 request 入队"与"直接入队缓冲区"混用。如果第一个缓冲区是直接入队的，而应用随后又尝试入队一个 request(或反之)，将返回 ``EBUSY``。在关闭文件描述符、调用 :ref:`VIDIOC_STREAMOFF <VIDIOC_STREAMON>` 或调用 :ref:`VIDIOC_REQBUFS` 之后，这项检查会被重置。

   对于 :ref:`memory-to-memory devices <mem2mem>`，只能为输出缓冲区指定 ``request_fd``，不能为采集缓冲区指定。尝试为采集缓冲区指定它将导致 ``EBADR`` 错误。)

Applications call the ``VIDIOC_DQBUF`` ioctl to dequeue a filled
(capturing) or displayed (output) buffer from the driver's outgoing
queue. They just set the ``type``, ``memory`` and ``reserved`` fields of
a struct :c:type:`v4l2_buffer` as above, when
``VIDIOC_DQBUF`` is called with a pointer to this structure the driver
fills all remaining fields or returns an error code. The driver may also
set ``V4L2_BUF_FLAG_ERROR`` in the ``flags`` field. It indicates a
non-critical (recoverable) streaming error. In such case the application
may continue as normal, but should be aware that data in the dequeued
buffer might be corrupted. When using the multi-planar API, the planes
array must be passed in as well.
(应用程序调用 ``VIDIOC_DQBUF`` ioctl，从驱动的传出队列中出队一个已填充(采集)或已显示(输出)的缓冲区。它们只需像上文那样设置结构体 :c:type:`v4l2_buffer` 的 ``type``、``memory`` 与 ``reserved`` 字段；当以指向该结构的指针调用 ``VIDIOC_DQBUF`` 时，驱动会填入其余所有字段或返回错误码。驱动还可能在 ``flags`` 字段中设置 ``V4L2_BUF_FLAG_ERROR``。它表示一个非致命(可恢复)的流处理错误。此时应用可以照常继续，但应注意出队缓冲区中的数据可能已损坏。使用多平面 API 时，还必须传入 planes 数组。)

If the application sets the ``memory`` field to ``V4L2_MEMORY_DMABUF`` to
dequeue a :ref:`DMABUF <dmabuf>` buffer, the driver fills the ``m.fd`` field
with a file descriptor numerically the same as the one given to ``VIDIOC_QBUF``
when the buffer was enqueued. No new file descriptor is created at dequeue time
and the value is only for the application convenience. When the multi-planar
API is used the ``m.fd`` fields of the passed array of struct
:c:type:`v4l2_plane` are filled instead.
(如果应用把 ``memory`` 字段设为 ``V4L2_MEMORY_DMABUF`` 来出队一个 :ref:`DMABUF <dmabuf>` 缓冲区，驱动会向 ``m.fd`` 字段填入一个文件描述符，其数值与缓冲区入队时传给 ``VIDIOC_QBUF`` 的那个相同。出队时不会创建新的文件描述符，该值只是为了方便应用。使用多平面 API 时，改为填充所传入结构体 :c:type:`v4l2_plane` 数组中的 ``m.fd`` 字段。)

By default ``VIDIOC_DQBUF`` blocks when no buffer is in the outgoing
queue. When the ``O_NONBLOCK`` flag was given to the
:c:func:`open()` function, ``VIDIOC_DQBUF`` returns
immediately with an ``EAGAIN`` error code when no buffer is available.
(默认情况下，当传出队列中没有缓冲区时 ``VIDIOC_DQBUF`` 会阻塞。如果在调用 :c:func:`open()` 函数时给了 ``O_NONBLOCK`` 标志，则当没有可用缓冲区时 ``VIDIOC_DQBUF`` 会立即以 ``EAGAIN`` 错误码返回。)

The struct :c:type:`v4l2_buffer` structure is specified in
:ref:`buffer`.
(结构体 :c:type:`v4l2_buffer` 在 :ref:`buffer` 一节中定义。)

Return Value(返回值)
====================

On success 0 is returned, on error -1 and the ``errno`` variable is set
appropriately. The generic error codes are described at the
:ref:`Generic Error Codes <gen-errors>` chapter.
(成功时返回 0，出错时返回 -1 并将 ``errno`` 变量设置为相应值。通用错误码在 :ref:`Generic Error Codes <gen-errors>` 一章中描述。)

EAGAIN
    Non-blocking I/O has been selected using ``O_NONBLOCK`` and no
    buffer was in the outgoing queue.
    (已通过 ``O_NONBLOCK`` 选择非阻塞 I/O，且传出队列中没有缓冲区。)

EINVAL
    The buffer ``type`` is not supported, or the ``index`` is out of
    bounds, or no buffers have been allocated yet, or the ``userptr`` or
    ``length`` are invalid, or the ``V4L2_BUF_FLAG_REQUEST_FD`` flag was
    set but the given ``request_fd`` was invalid, or ``m.fd`` was
    an invalid DMABUF file descriptor.
    (缓冲区 ``type`` 不受支持，或 ``index`` 越界，或尚未分配任何缓冲区，或 ``userptr`` 或 ``length`` 无效，或设置了 ``V4L2_BUF_FLAG_REQUEST_FD`` 标志但给定的 ``request_fd`` 无效，或 ``m.fd`` 是无效的 DMABUF 文件描述符。)

EIO
    ``VIDIOC_DQBUF`` failed due to an internal error. Can also indicate
    temporary problems like signal loss.
    (``VIDIOC_DQBUF`` 因内部错误而失败。也可能表示信号丢失等临时性问题。)

    .. note::

       The driver might dequeue an (empty) buffer despite returning
       an error, or even stop capturing. Reusing such buffer may be unsafe
       though and its details (e.g. ``index``) may not be returned either.
       It is recommended that drivers indicate recoverable errors by setting
       the ``V4L2_BUF_FLAG_ERROR`` and returning 0 instead. In that case the
       application should be able to safely reuse the buffer and continue
       streaming.
       (注意：驱动即使返回错误，也可能已经出队了一个(空)缓冲区，甚至可能停止采集。不过复用这样的缓冲区可能不安全，而且其细节(例如 ``index``)也可能不会返回。建议驱动通过设置 ``V4L2_BUF_FLAG_ERROR`` 并返回 0 来指示可恢复的错误。在这种情况下，应用应能安全地复用该缓冲区并继续流处理。)

EPIPE
    ``VIDIOC_DQBUF`` returns this on an empty capture queue for mem2mem
    codecs if a buffer with the ``V4L2_BUF_FLAG_LAST`` was already
    dequeued and no new buffers are expected to become available.
    (对于 mem2mem 编解码器，当带有 ``V4L2_BUF_FLAG_LAST`` 的缓冲区已被出队且预期不会再有新缓冲区变得可用时，``VIDIOC_DQBUF`` 在采集队列为空时返回此错误码。)

EBADR
    The ``V4L2_BUF_FLAG_REQUEST_FD`` flag was set but the device does not
    support requests for the given buffer type, or
    the ``V4L2_BUF_FLAG_REQUEST_FD`` flag was not set but the device requires
    that the buffer is part of a request.
    (设置了 ``V4L2_BUF_FLAG_REQUEST_FD`` 标志，但设备不支持给定缓冲区类型的 request；或未设置 ``V4L2_BUF_FLAG_REQUEST_FD`` 标志，但设备要求缓冲区必须属于某个 request。)

EBUSY
    The first buffer was queued via a request, but the application now tries
    to queue it directly, or vice versa (it is not permitted to mix the two
    APIs).
    (第一个缓冲区是通过 request 入队的，但应用现在却尝试直接入队；或反之(不允许混用这两种 API)。)
