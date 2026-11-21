.. SPDX-License-Identifier: GFDL-1.1-no-invariants-or-later
.. c:namespace:: V4L

.. _VIDIOC_CREATE_BUFS:

************************************
ioctl VIDIOC_CREATE_BUFS(创建缓冲区)
************************************

Name(名称)
==========

VIDIOC_CREATE_BUFS - Create buffers for Memory Mapped or User Pointer or DMA Buffer I/O
(VIDIOC_CREATE_BUFS - 为内存映射、用户指针或 DMA 缓冲区 I/O 创建缓冲区)

Synopsis(概要)
==============

.. c:macro:: VIDIOC_CREATE_BUFS

``int ioctl(int fd, VIDIOC_CREATE_BUFS, struct v4l2_create_buffers *argp)``

Arguments(参数)
===============

``fd``
    File descriptor returned by :c:func:`open()`.
    (由 :c:func:`open()` 返回的文件描述符。)

``argp``
    Pointer to struct :c:type:`v4l2_create_buffers`.
    (指向 struct :c:type:`v4l2_create_buffers` 的指针。)

Description(描述)
=================

This ioctl is used to create buffers for :ref:`memory mapped <mmap>`
or :ref:`user pointer <userp>` or :ref:`DMA buffer <dmabuf>` I/O. It
can be used as an alternative or in addition to the
:ref:`VIDIOC_REQBUFS` ioctl, when a tighter control
over buffers is required. This ioctl can be called multiple times to
create buffers of different sizes.
(此 ioctl 用于为 :ref:`memory mapped <mmap>`（内存映射）、
:ref:`user pointer <userp>`（用户指针）或 :ref:`DMA buffer <dmabuf>`
（DMA 缓冲区）I/O 创建缓冲区。当需要对缓冲区进行更精细的控制时，它可以
作为 :ref:`VIDIOC_REQBUFS` ioctl 的替代或补充使用。此 ioctl 可以多次
调用，以创建不同大小的缓冲区。)

To allocate the device buffers applications must initialize the relevant
fields of the struct :c:type:`v4l2_create_buffers` structure. The
``count`` field must be set to the number of requested buffers, the
``memory`` field specifies the requested I/O method and the ``reserved``
array must be zeroed.
(要分配设备缓冲区，应用程序必须初始化 struct
:c:type:`v4l2_create_buffers` 结构的相关字段。``count`` 字段必须设置为
所请求的缓冲区数量，``memory`` 字段指定所请求的 I/O 方法，``reserved``
数组必须清零。)

The ``format`` field specifies the image format that the buffers must be
able to handle. The application has to fill in this struct
:c:type:`v4l2_format`. Usually this will be done using the
:ref:`VIDIOC_TRY_FMT <VIDIOC_G_FMT>` or
:ref:`VIDIOC_G_FMT <VIDIOC_G_FMT>` ioctls to ensure that the
requested format is supported by the driver. Based on the format's
``type`` field the requested buffer size (for single-planar) or plane
sizes (for multi-planar formats) will be used for the allocated buffers.
The driver may return an error if the size(s) are not supported by the
hardware (usually because they are too small).
(``format`` 字段指定缓冲区必须能够处理的图像格式。应用程序必须填写这个
struct :c:type:`v4l2_format`。通常会先用 :ref:`VIDIOC_TRY_FMT
<VIDIOC_G_FMT>` 或 :ref:`VIDIOC_G_FMT <VIDIOC_G_FMT>` ioctl 来确保所
请求的格式受驱动支持。根据格式的 ``type`` 字段，所请求的缓冲区大小（对
单平面格式）或各平面大小（对多平面格式）将用于分配的缓冲区。如果硬件不
支持这些大小（通常是因为太小），驱动可能返回错误。)

The buffers created by this ioctl will have as minimum size the size
defined by the ``format.pix.sizeimage`` field (or the corresponding
fields for other format types). Usually if the ``format.pix.sizeimage``
field is less than the minimum required for the given format, then an
error will be returned since drivers will typically not allow this. If
it is larger, then the value will be used as-is. In other words, the
driver may reject the requested size, but if it is accepted the driver
will use it unchanged.
(此 ioctl 创建的缓冲区的最小尺寸为 ``format.pix.sizeimage`` 字段定义的
大小（或其他格式类型对应的字段）。通常，如果 ``format.pix.sizeimage``
字段小于给定格式所需的最小值，则会返回错误，因为驱动一般不允许这样
做。如果更大，则该值将按原样使用。换句话说，驱动可以拒绝所请求的大小，
但如果接受了它，驱动将不加改变地使用它。)

When the ioctl is called with a pointer to this structure the driver
will attempt to allocate up to the requested number of buffers and store
the actual number allocated and the starting index in the ``count`` and
the ``index`` fields respectively. On return ``count`` can be smaller
than the number requested.
(当用指向该结构的指针调用此 ioctl 时，驱动会尝试分配最多所请求数量的
缓冲区，并把实际分配的数量和起始索引分别存储在 ``count`` 和 ``index``
字段中。返回时 ``count`` 可能小于所请求的数量。)

.. c:type:: v4l2_create_buffers

.. tabularcolumns:: |p{4.4cm}|p{4.4cm}|p{8.5cm}|

.. flat-table:: struct v4l2_create_buffers
    :header-rows:  0
    :stub-columns: 0
    :widths:       1 1 2

    * - __u32
      - ``index``
      - The starting buffer index, returned by the driver.
	(起始缓冲区索引，由驱动返回。)
    * - __u32
      - ``count``
      - The number of buffers requested or granted. If count == 0, then
	:ref:`VIDIOC_CREATE_BUFS` will set ``index`` to the current number of
	created buffers, and it will check the validity of ``memory`` and
	``format.type``. If those are invalid -1 is returned and errno is
	set to ``EINVAL`` error code, otherwise :ref:`VIDIOC_CREATE_BUFS` returns
	0. It will never set errno to ``EBUSY`` error code in this particular
	case.
	(请求或授予的缓冲区数量。如果 count == 0，则 :ref:`VIDIOC_CREATE_BUFS`
	会把 ``index`` 设置为当前已创建缓冲区的数量，并检查 ``memory`` 和
	``format.type`` 的有效性。如果它们无效，则返回 -1 并把 errno 设置为
	``EINVAL`` 错误码；否则 :ref:`VIDIOC_CREATE_BUFS` 返回 0。在这种特殊
	情况下，它绝不会把 errno 设置为 ``EBUSY`` 错误码。)
    * - __u32
      - ``memory``
      - Applications set this field to ``V4L2_MEMORY_MMAP``,
	``V4L2_MEMORY_DMABUF`` or ``V4L2_MEMORY_USERPTR``. See
	:c:type:`v4l2_memory`
	(应用程序把此字段设置为 ``V4L2_MEMORY_MMAP``、
	``V4L2_MEMORY_DMABUF`` 或 ``V4L2_MEMORY_USERPTR``。参见
	:c:type:`v4l2_memory`)
    * - struct :c:type:`v4l2_format`
      - ``format``
      - Filled in by the application, preserved by the driver.
	(由应用程序填写，由驱动保留。)
    * - __u32
      - ``capabilities``
      - Set by the driver. If 0, then the driver doesn't support
        capabilities. In that case all you know is that the driver is
	guaranteed to support ``V4L2_MEMORY_MMAP`` and *might* support
	other :c:type:`v4l2_memory` types. It will not support any other
	capabilities. See :ref:`here <v4l2-buf-capabilities>` for a list of the
	capabilities.
	(由驱动设置。如果为 0，则驱动不支持 capabilities。在这种情况下，
	你只能知道驱动保证支持 ``V4L2_MEMORY_MMAP``，并且 *可能* 支持其他
	:c:type:`v4l2_memory` 类型。它不支持任何其他能力。能力列表参见
	:ref:`here <v4l2-buf-capabilities>`。)

	If you want to just query the capabilities without making any
	other changes, then set ``count`` to 0, ``memory`` to
	``V4L2_MEMORY_MMAP`` and ``format.type`` to the buffer type.
	(如果只想查询 capabilities 而不做任何其他更改，则把 ``count`` 设置
	为 0，``memory`` 设置为 ``V4L2_MEMORY_MMAP``，``format.type`` 设置为
	缓冲区类型。)

    * - __u32
      - ``flags``
      - Specifies additional buffer management attributes.
	See :ref:`memory-flags`.
	(指定额外的缓冲区管理属性。参见 :ref:`memory-flags`。)
    * - __u32
      - ``max_num_buffers``
      - If the V4L2_BUF_CAP_SUPPORTS_MAX_NUM_BUFFERS capability flag is set
        this field indicates the maximum possible number of buffers
        for this queue.
	(如果设置了 V4L2_BUF_CAP_SUPPORTS_MAX_NUM_BUFFERS 能力标志，此字段
	指示此队列可能的最大缓冲区数量。)
    * - __u32
      - ``reserved``\ [5]
      - A place holder for future extensions. Drivers and applications
	must set the array to zero.
	(为未来扩展保留的占位符。驱动和应用程序必须将数组置零。)

Return Value(返回值)
====================

On success 0 is returned, on error -1 and the ``errno`` variable is set
appropriately. The generic error codes are described at the
:ref:`Generic Error Codes <gen-errors>` chapter.
(成功时返回 0，出错时返回 -1 并适当设置 ``errno`` 变量。通用错误码在
:ref:`Generic Error Codes <gen-errors>` 一章中描述。)

ENOMEM
    No memory to allocate buffers for :ref:`memory mapped <mmap>` I/O.
    (没有内存为 :ref:`memory mapped <mmap>` I/O 分配缓冲区。)

EINVAL
    The buffer type (``format.type`` field), requested I/O method
    (``memory``) or format (``format`` field) is not valid.
    (缓冲区类型（``format.type`` 字段）、所请求的 I/O 方法
    （``memory``）或格式（``format`` 字段）无效。)
