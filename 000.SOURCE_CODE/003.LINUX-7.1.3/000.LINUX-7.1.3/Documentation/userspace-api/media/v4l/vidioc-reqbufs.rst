.. SPDX-License-Identifier: GFDL-1.1-no-invariants-or-later
.. c:namespace:: V4L

.. _VIDIOC_REQBUFS:

********************************
ioctl VIDIOC_REQBUFS(申请缓冲区)
********************************

Name(名称)
==========

VIDIOC_REQBUFS - Initiate Memory Mapping, User Pointer I/O or DMA buffer I/O
(VIDIOC_REQBUFS - 发起内存映射、用户指针 I/O 或 DMA 缓冲区 I/O)

Synopsis(概要)
==============

.. c:macro:: VIDIOC_REQBUFS

``int ioctl(int fd, VIDIOC_REQBUFS, struct v4l2_requestbuffers *argp)``

Arguments(参数)
===============

``fd``
    File descriptor returned by :c:func:`open()`.
    (由 :c:func:`open()` 返回的文件描述符。)

``argp``
    Pointer to struct :c:type:`v4l2_requestbuffers`.
    (指向 struct :c:type:`v4l2_requestbuffers` 的指针。)

Description(描述)
=================

This ioctl is used to initiate :ref:`memory mapped <mmap>`,
:ref:`user pointer <userp>` or :ref:`DMABUF <dmabuf>` based I/O.
Memory mapped buffers are located in device memory and must be allocated
with this ioctl before they can be mapped into the application's address
space. User buffers are allocated by applications themselves, and this
ioctl is merely used to switch the driver into user pointer I/O mode and
to setup some internal structures. Similarly, DMABUF buffers are
allocated by applications through a device driver, and this ioctl only
configures the driver into DMABUF I/O mode without performing any direct
allocation.
(该 ioctl 用于发起基于 :ref:`内存映射 <mmap>` 、 :ref:`用户指针 <userp>` 或 :ref:`DMABUF <dmabuf>` 的 I/O。内存映射缓冲区位于设备内存中，必须先用该 ioctl 分配，然后才能映射到应用程序的地址空间。用户缓冲区由应用程序自行分配，该 ioctl 仅用于把驱动切换到用户指针 I/O 模式并设置一些内部结构。类似地，DMABUF 缓冲区由应用程序通过设备驱动分配，该 ioctl 只把驱动配置为 DMABUF I/O 模式，而不执行任何直接分配。)

To allocate device buffers applications initialize all fields of the
struct :c:type:`v4l2_requestbuffers` structure. They set the ``type``
field to the respective stream or buffer type, the ``count`` field to
the desired number of buffers, ``memory`` must be set to the requested
I/O method and the ``reserved`` array must be zeroed. When the ioctl is
called with a pointer to this structure the driver will attempt to
allocate the requested number of buffers and it stores the actual number
allocated in the ``count`` field. It can be smaller than the number
requested, even zero, when the driver runs out of free memory. A larger
number is also possible when the driver requires more buffers to
function correctly. For example video output requires at least two
buffers, one displayed and one filled by the application.
(要分配设备缓冲区，应用程序需要初始化 struct :c:type:`v4l2_requestbuffers` 结构的所有字段。它们把 ``type`` 字段设置为相应的流或缓冲区类型，把 ``count`` 字段设置为期望的缓冲区数量， ``memory`` 必须设置为所请求的 I/O 方式，并且 ``reserved`` 数组必须清零。当以指向该结构的指针调用该 ioctl 时，驱动会尝试分配所请求数量的缓冲区，并把实际分配的数量存入 ``count`` 字段。当驱动耗尽空闲内存时，实际数量可能小于所请求的数量，甚至为零。当驱动需要更多缓冲区才能正常工作时，实际数量也可能更大。例如，视频输出至少需要两个缓冲区：一个用于显示，一个由应用程序填充。)

When the I/O method is not supported the ioctl returns an ``EINVAL`` error
code.
(当 I/O 方式不受支持时，该 ioctl 返回 ``EINVAL`` 错误码。)

Applications can call :ref:`VIDIOC_REQBUFS` again to change the number of
buffers. Note that if any buffers are still mapped or exported via DMABUF,
then :ref:`VIDIOC_REQBUFS` can only succeed if the
``V4L2_BUF_CAP_SUPPORTS_ORPHANED_BUFS`` capability is set. Otherwise
:ref:`VIDIOC_REQBUFS` will return the ``EBUSY`` error code.
If ``V4L2_BUF_CAP_SUPPORTS_ORPHANED_BUFS`` is set, then these buffers are
orphaned and will be freed when they are unmapped or when the exported DMABUF
fds are closed. A ``count`` value of zero frees or orphans all buffers, after
aborting or finishing any DMA in progress, an implicit
:ref:`VIDIOC_STREAMOFF <VIDIOC_STREAMON>`.
(应用程序可以再次调用 :ref:`VIDIOC_REQBUFS` 来改变缓冲区数量。注意，如果仍有缓冲区被映射或通过 DMABUF 导出，那么只有设置了 ``V4L2_BUF_CAP_SUPPORTS_ORPHANED_BUFS`` 能力时 :ref:`VIDIOC_REQBUFS` 才可能成功，否则它将返回 ``EBUSY`` 错误码。如果设置了 ``V4L2_BUF_CAP_SUPPORTS_ORPHANED_BUFS``，这些缓冲区将被遗弃(orphaned)，并在被取消映射或导出的 DMABUF fd 关闭时释放。 ``count`` 值为零会在中止或等待完成所有进行中的 DMA 之后(即一次隐式的 :ref:`VIDIOC_STREAMOFF <VIDIOC_STREAMON>` )，释放或遗弃所有缓冲区。)

.. c:type:: v4l2_requestbuffers

.. tabularcolumns:: |p{4.4cm}|p{4.4cm}|p{8.5cm}|

.. cssclass:: longtable

.. flat-table:: struct v4l2_requestbuffers
    :header-rows:  0
    :stub-columns: 0
    :widths:       1 1 2

    * - __u32
      - ``count``
      - The number of buffers requested or granted. (请求或授予的缓冲区数量。)
    * - __u32
      - ``type``
      - Type of the stream or buffers, this is the same as the struct
	:c:type:`v4l2_format` ``type`` field. See
	:c:type:`v4l2_buf_type` for valid values.
	(流或缓冲区的类型，与 struct :c:type:`v4l2_format` 的 ``type`` 字段相同。有效取值参见 :c:type:`v4l2_buf_type`。)
    * - __u32
      - ``memory``
      - Applications set this field to ``V4L2_MEMORY_MMAP``,
	``V4L2_MEMORY_DMABUF`` or ``V4L2_MEMORY_USERPTR``. See
	:c:type:`v4l2_memory`.
	(应用程序把该字段设置为 ``V4L2_MEMORY_MMAP`` 、 ``V4L2_MEMORY_DMABUF`` 或 ``V4L2_MEMORY_USERPTR``。参见 :c:type:`v4l2_memory`。)
    * - __u32
      - ``capabilities``
      - Set by the driver. If 0, then the driver doesn't support
        capabilities. In that case all you know is that the driver is
	guaranteed to support ``V4L2_MEMORY_MMAP`` and *might* support
	other :c:type:`v4l2_memory` types. It will not support any other
	capabilities.
	(由驱动设置。如果为 0，则该驱动不支持能力报告。在这种情况下，你只能知道该驱动保证支持 ``V4L2_MEMORY_MMAP`` ，并且 *可能* 支持其他 :c:type:`v4l2_memory` 类型。它不会支持任何其他能力。)

	If you want to query the capabilities with a minimum of side-effects,
	then this can be called with ``count`` set to 0, ``memory`` set to
	``V4L2_MEMORY_MMAP`` and ``type`` set to the buffer type. This will
	free any previously allocated buffers, so this is typically something
	that will be done at the start of the application.
	(如果想在副作用最小的情况下查询能力，可以把 ``count`` 设为 0、 ``memory`` 设为 ``V4L2_MEMORY_MMAP`` 、 ``type`` 设为相应的缓冲区类型来调用。这会释放之前分配的所有缓冲区，因此通常在应用程序启动时进行。)
    * - __u8
      - ``flags``
      - Specifies additional buffer management attributes.
	See :ref:`memory-flags`.
	(指定额外的缓冲区管理属性。参见 :ref:`memory-flags`。)
    * - __u8
      - ``reserved``\ [3]
      - Reserved for future extensions. (为未来的扩展保留。)

.. _v4l2-buf-capabilities:
.. _V4L2-BUF-CAP-SUPPORTS-MMAP:
.. _V4L2-BUF-CAP-SUPPORTS-USERPTR:
.. _V4L2-BUF-CAP-SUPPORTS-DMABUF:
.. _V4L2-BUF-CAP-SUPPORTS-REQUESTS:
.. _V4L2-BUF-CAP-SUPPORTS-ORPHANED-BUFS:
.. _V4L2-BUF-CAP-SUPPORTS-M2M-HOLD-CAPTURE-BUF:
.. _V4L2-BUF-CAP-SUPPORTS-MMAP-CACHE-HINTS:
.. _V4L2-BUF-CAP-SUPPORTS-MAX-NUM-BUFFERS:
.. _V4L2-BUF-CAP-SUPPORTS-REMOVE-BUFS:

.. flat-table:: V4L2 Buffer Capabilities Flags
    :header-rows:  0
    :stub-columns: 0
    :widths:       3 1 4

    * - ``V4L2_BUF_CAP_SUPPORTS_MMAP``
      - 0x00000001
      - This buffer type supports the ``V4L2_MEMORY_MMAP`` streaming mode.
        (该缓冲区类型支持 ``V4L2_MEMORY_MMAP`` 流式模式。)
    * - ``V4L2_BUF_CAP_SUPPORTS_USERPTR``
      - 0x00000002
      - This buffer type supports the ``V4L2_MEMORY_USERPTR`` streaming mode.
        (该缓冲区类型支持 ``V4L2_MEMORY_USERPTR`` 流式模式。)
    * - ``V4L2_BUF_CAP_SUPPORTS_DMABUF``
      - 0x00000004
      - This buffer type supports the ``V4L2_MEMORY_DMABUF`` streaming mode.
        (该缓冲区类型支持 ``V4L2_MEMORY_DMABUF`` 流式模式。)
    * - ``V4L2_BUF_CAP_SUPPORTS_REQUESTS``
      - 0x00000008
      - This buffer type supports :ref:`requests <media-request-api>`.
        (该缓冲区类型支持 :ref:`requests <media-request-api>`。)
    * - ``V4L2_BUF_CAP_SUPPORTS_ORPHANED_BUFS``
      - 0x00000010
      - The kernel allows calling :ref:`VIDIOC_REQBUFS` while buffers are still
        mapped or exported via DMABUF. These orphaned buffers will be freed
        when they are unmapped or when the exported DMABUF fds are closed.
        (内核允许在缓冲区仍被映射或通过 DMABUF 导出时调用 :ref:`VIDIOC_REQBUFS`。这些被遗弃(orphaned)的缓冲区将在被取消映射或导出的 DMABUF fd 被关闭时释放。)
    * - ``V4L2_BUF_CAP_SUPPORTS_M2M_HOLD_CAPTURE_BUF``
      - 0x00000020
      - Only valid for stateless decoders. If set, then userspace can set the
        ``V4L2_BUF_FLAG_M2M_HOLD_CAPTURE_BUF`` flag to hold off on returning the
	capture buffer until the OUTPUT timestamp changes.
        (仅对无状态(stateless)解码器有效。如果置位，用户空间可以设置 ``V4L2_BUF_FLAG_M2M_HOLD_CAPTURE_BUF`` 标志，推迟返回捕获缓冲区，直到 OUTPUT 时间戳发生变化。)
    * - ``V4L2_BUF_CAP_SUPPORTS_MMAP_CACHE_HINTS``
      - 0x00000040
      - This capability is set by the driver to indicate that the queue supports
        cache and memory management hints. However, it's only valid when the
        queue is used for :ref:`memory mapping <mmap>` streaming I/O. See
        :ref:`V4L2_BUF_FLAG_NO_CACHE_INVALIDATE <V4L2-BUF-FLAG-NO-CACHE-INVALIDATE>`,
        :ref:`V4L2_BUF_FLAG_NO_CACHE_CLEAN <V4L2-BUF-FLAG-NO-CACHE-CLEAN>` and
        :ref:`V4L2_MEMORY_FLAG_NON_COHERENT <V4L2-MEMORY-FLAG-NON-COHERENT>`.
        (驱动设置此能力，表示该队列支持缓存与内存管理提示。但仅当队列用于 :ref:`内存映射 <mmap>` 流式 I/O 时才有效。参见 :ref:`V4L2_BUF_FLAG_NO_CACHE_INVALIDATE <V4L2-BUF-FLAG-NO-CACHE-INVALIDATE>` 、 :ref:`V4L2_BUF_FLAG_NO_CACHE_CLEAN <V4L2-BUF-FLAG-NO-CACHE-CLEAN>` 与 :ref:`V4L2_MEMORY_FLAG_NON_COHERENT <V4L2-MEMORY-FLAG-NON-COHERENT>`。)
    * - ``V4L2_BUF_CAP_SUPPORTS_MAX_NUM_BUFFERS``
      - 0x00000080
      - If set, then the ``max_num_buffers`` field in ``struct v4l2_create_buffers``
        is valid. If not set, then the maximum is ``VIDEO_MAX_FRAME`` buffers.
        (如果置位， ``struct v4l2_create_buffers`` 中的 ``max_num_buffers`` 字段有效。如果未置位，最大值为 ``VIDEO_MAX_FRAME`` 个缓冲区。)
    * - ``V4L2_BUF_CAP_SUPPORTS_REMOVE_BUFS``
      - 0x00000100
      - If set, then ``VIDIOC_REMOVE_BUFS`` is supported.
        (如果置位，则支持 ``VIDIOC_REMOVE_BUFS``。)

.. _memory-flags:
.. _V4L2-MEMORY-FLAG-NON-COHERENT:

.. flat-table:: Memory Consistency Flags
    :header-rows:  0
    :stub-columns: 0
    :widths:       3 1 4

    * - ``V4L2_MEMORY_FLAG_NON_COHERENT``
      - 0x00000001
      - A buffer is allocated either in coherent (it will be automatically
	coherent between the CPU and the bus) or non-coherent memory. The
	latter can provide performance gains, for instance the CPU cache
	sync/flush operations can be avoided if the buffer is accessed by the
	corresponding device only and the CPU does not read/write to/from that
	buffer. However, this requires extra care from the driver -- it must
	guarantee memory consistency by issuing a cache flush/sync when
	consistency is needed. If this flag is set V4L2 will attempt to
	allocate the buffer in non-coherent memory. The flag takes effect
	only if the buffer is used for :ref:`memory mapping <mmap>` I/O and the
	queue reports the :ref:`V4L2_BUF_CAP_SUPPORTS_MMAP_CACHE_HINTS
	<V4L2-BUF-CAP-SUPPORTS-MMAP-CACHE-HINTS>` capability.
	(缓冲区要么分配在一致(coherent，CPU 与总线之间自动保持一致)内存中，要么分配在非一致(non-coherent)内存中。后者可以带来性能收益，例如当缓冲区仅由相应设备访问、CPU 不对其进行读写时，可以避免 CPU 缓存同步/刷新操作。但这需要驱动格外小心——在需要一致性时，它必须通过执行缓存刷新/同步来保证内存一致性。如果设置了此标志，V4L2 将尝试在非一致内存中分配缓冲区。该标志仅在缓冲区用于 :ref:`内存映射 <mmap>` I/O 且队列报告 :ref:`V4L2_BUF_CAP_SUPPORTS_MMAP_CACHE_HINTS <V4L2-BUF-CAP-SUPPORTS-MMAP-CACHE-HINTS>` 能力时才生效。)

.. raw:: latex

   \normalsize

Return Value(返回值)
====================

On success 0 is returned, on error -1 and the ``errno`` variable is set
appropriately. The generic error codes are described at the
:ref:`Generic Error Codes <gen-errors>` chapter.
(成功时返回 0；出错时返回 -1，并适当设置 ``errno`` 变量。通用错误码在 :ref:`Generic Error Codes <gen-errors>` 一章中描述。)

EINVAL
    The buffer type (``type`` field) or the requested I/O method
    (``memory``) is not supported.
    (缓冲区类型( ``type`` 字段)或所请求的 I/O 方式( ``memory`` )不受支持。)
