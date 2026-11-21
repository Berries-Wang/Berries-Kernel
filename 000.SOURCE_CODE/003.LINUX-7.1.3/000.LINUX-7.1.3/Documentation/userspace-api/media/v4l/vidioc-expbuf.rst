.. SPDX-License-Identifier: GFDL-1.1-no-invariants-or-later
.. c:namespace:: V4L

.. _VIDIOC_EXPBUF:

****************************************
ioctl VIDIOC_EXPBUF(ioctl VIDIOC_EXPBUF)
****************************************

Name(名称)
==========

VIDIOC_EXPBUF - Export a buffer as a DMABUF file descriptor.
(VIDIOC_EXPBUF - 将缓冲区导出为 DMABUF 文件描述符。)

Synopsis(概要)
==============

.. c:macro:: VIDIOC_EXPBUF

``int ioctl(int fd, VIDIOC_EXPBUF, struct v4l2_exportbuffer *argp)``

Arguments(参数)
===============

``fd``
    File descriptor returned by :c:func:`open()`.
    (由 :c:func:`open()` 返回的文件描述符。)

``argp``
    Pointer to struct :c:type:`v4l2_exportbuffer`.
    (指向 struct :c:type:`v4l2_exportbuffer` 的指针。)

Description(描述)
=================

This ioctl is an extension to the :ref:`memory mapping <mmap>` I/O
method, therefore it is available only for ``V4L2_MEMORY_MMAP`` buffers.
It can be used to export a buffer as a DMABUF file at any time after
buffers have been allocated with the
:ref:`VIDIOC_REQBUFS` ioctl.
(此 ioctl 是 :ref:`memory mapping <mmap>` I/O 方法的扩展，因此只对 ``V4L2_MEMORY_MMAP`` 缓冲区可用。在用 :ref:`VIDIOC_REQBUFS` ioctl 分配缓冲区之后的任何时刻，都可以用它把缓冲区导出为 DMABUF 文件。)

To export a buffer, applications fill struct
:c:type:`v4l2_exportbuffer`. The ``type`` field is
set to the same buffer type as was previously used with struct
:c:type:`v4l2_requestbuffers` ``type``.
Applications must also set the ``index`` field. Valid index numbers
range from zero to the number of buffers allocated with
:ref:`VIDIOC_REQBUFS` (struct
:c:type:`v4l2_requestbuffers` ``count``) minus
one. For the multi-planar API, applications set the ``plane`` field to
the index of the plane to be exported. Valid planes range from zero to
the maximal number of valid planes for the currently active format. For
the single-planar API, applications must set ``plane`` to zero.
Additional flags may be posted in the ``flags`` field. Refer to a manual
for open() for details. Currently only O_CLOEXEC, O_RDONLY, O_WRONLY,
and O_RDWR are supported. All other fields must be set to zero. In the
case of multi-planar API, every plane is exported separately using
multiple :ref:`VIDIOC_EXPBUF` calls.
(要导出缓冲区，应用程序填写 struct :c:type:`v4l2_exportbuffer` 。 ``type`` 字段被设置为与之前用于 struct :c:type:`v4l2_requestbuffers` 的 ``type`` 相同的缓冲区类型。应用程序还必须设置 ``index`` 字段。有效的索引号范围是从 0 到用 :ref:`VIDIOC_REQBUFS` 分配的缓冲区数量(struct :c:type:`v4l2_requestbuffers` 的 ``count``)减一。对于多平面 (multi-planar) API，应用程序将 ``plane`` 字段设置为要导出的平面的索引。有效平面的范围从 0 到当前激活格式的最大有效平面数。对于单平面 (single-planar) API，应用程序必须把 ``plane`` 设置为 0。可以在 ``flags`` 字段中提供附加标志。详情请参阅 open() 的手册页。目前只支持 O_CLOEXEC、O_RDONLY、O_WRONLY 和 O_RDWR。所有其他字段必须设置为 0。对于多平面 API，每个平面都通过多次 :ref:`VIDIOC_EXPBUF` 调用分别导出。)

After calling :ref:`VIDIOC_EXPBUF` the ``fd`` field will be set by a
driver. This is a DMABUF file descriptor. The application may pass it to
other DMABUF-aware devices. Refer to :ref:`DMABUF importing <dmabuf>`
for details about importing DMABUF files into V4L2 nodes. It is
recommended to close a DMABUF file when it is no longer used to allow
the associated memory to be reclaimed.
(调用 :ref:`VIDIOC_EXPBUF` 之后，驱动会设置 ``fd`` 字段。这是一个 DMABUF 文件描述符。应用程序可以把它传递给其他支持 DMABUF 的设备。关于把 DMABUF 文件导入 V4L2 节点的细节，参见 :ref:`DMABUF importing <dmabuf>` 。建议在不再使用 DMABUF 文件时将其关闭，以便回收其关联的内存。)

Examples(示例)
==============

.. code-block:: c

    int buffer_export(int v4lfd, enum v4l2_buf_type bt, int index, int *dmafd)
    {
	struct v4l2_exportbuffer expbuf;

	memset(&expbuf, 0, sizeof(expbuf));
	expbuf.type = bt;
	expbuf.index = index;
	if (ioctl(v4lfd, VIDIOC_EXPBUF, &expbuf) == -1) {
	    perror("VIDIOC_EXPBUF");
	    return -1;
	}

	*dmafd = expbuf.fd;

	return 0;
    }

.. code-block:: c

    int buffer_export_mp(int v4lfd, enum v4l2_buf_type bt, int index,
	int dmafd[], int n_planes)
    {
	int i;

	for (i = 0; i < n_planes; ++i) {
	    struct v4l2_exportbuffer expbuf;

	    memset(&expbuf, 0, sizeof(expbuf));
	    expbuf.type = bt;
	    expbuf.index = index;
	    expbuf.plane = i;
	    if (ioctl(v4lfd, VIDIOC_EXPBUF, &expbuf) == -1) {
		perror("VIDIOC_EXPBUF");
		while (i)
		    close(dmafd[--i]);
		return -1;
	    }
	    dmafd[i] = expbuf.fd;
	}

	return 0;
    }

.. c:type:: v4l2_exportbuffer

.. tabularcolumns:: |p{4.4cm}|p{4.4cm}|p{8.5cm}|

.. flat-table:: struct v4l2_exportbuffer
    :header-rows:  0
    :stub-columns: 0
    :widths:       1 1 2

    * - __u32
      - ``type``
      - Type of the buffer, same as struct
	:c:type:`v4l2_format` ``type`` or struct
	:c:type:`v4l2_requestbuffers` ``type``, set
	by the application. See :c:type:`v4l2_buf_type`
	(缓冲区类型，与 struct :c:type:`v4l2_format` 的 ``type`` 或
	struct :c:type:`v4l2_requestbuffers` 的 ``type`` 相同，由应用
	程序设置。参见 :c:type:`v4l2_buf_type` )
    * - __u32
      - ``index``
      - Number of the buffer, set by the application. This field is only
	used for :ref:`memory mapping <mmap>` I/O and can range from
	zero to the number of buffers allocated with the
	:ref:`VIDIOC_REQBUFS` and/or
	:ref:`VIDIOC_CREATE_BUFS` ioctls.
	(缓冲区编号，由应用程序设置。此字段仅用于
	:ref:`memory mapping <mmap>` I/O，取值范围从 0 到用
	:ref:`VIDIOC_REQBUFS` 和/或 :ref:`VIDIOC_CREATE_BUFS` ioctl
	分配的缓冲区数量。)
    * - __u32
      - ``plane``
      - Index of the plane to be exported when using the multi-planar API.
	Otherwise this value must be set to zero.
	(使用多平面 API 时要导出的平面的索引。否则此值必须设置为 0。)
    * - __u32
      - ``flags``
      - Flags for the newly created file, currently only ``O_CLOEXEC``,
	``O_RDONLY``, ``O_WRONLY``, and ``O_RDWR`` are supported, refer to
	the manual of open() for more details.
	(新建文件的标志，目前只支持 ``O_CLOEXEC`` 、 ``O_RDONLY`` 、
	``O_WRONLY`` 和 ``O_RDWR`` ，更多细节请参阅 open() 的手册页。)
    * - __s32
      - ``fd``
      - The DMABUF file descriptor associated with a buffer. Set by the
	driver.
	(与缓冲区关联的 DMABUF 文件描述符。由驱动设置。)
    * - __u32
      - ``reserved[11]``
      - Reserved field for future use. Drivers and applications must set
	the array to zero.
	(保留字段供将来使用。驱动和应用程序必须将该数组置零。)

Return Value(返回值)
====================

On success 0 is returned, on error -1 and the ``errno`` variable is set
appropriately. The generic error codes are described at the
:ref:`Generic Error Codes <gen-errors>` chapter.
(成功时返回 0，出错时返回 -1 并适当设置 ``errno`` 变量。通用错误码在 :ref:`Generic Error Codes <gen-errors>` 一章中描述。)

EINVAL
    A queue is not in MMAP mode or DMABUF exporting is not supported or
    ``flags`` or ``type`` or ``index`` or ``plane`` fields are invalid.
    (队列不处于 MMAP 模式，或不支持 DMABUF 导出，或 ``flags`` 、
    ``type`` 、 ``index`` 或 ``plane`` 字段无效。)
