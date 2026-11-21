.. SPDX-License-Identifier: GFDL-1.1-no-invariants-or-later
.. c:namespace:: DTV.dmx

.. _DMX_EXPBUF:

**************************************************
ioctl DMX_EXPBUF(把缓冲区导出为 DMABUF 文件描述符)
**************************************************

Name(名称)
==========

DMX_EXPBUF - Export a buffer as a DMABUF file descriptor.
(DMX_EXPBUF - 把缓冲区导出为 DMABUF 文件描述符。)

.. warning:: this API is still experimental
             (此 API 仍处于实验阶段)

Synopsis(概要)
==============

.. c:macro:: DMX_EXPBUF

``int ioctl(int fd, DMX_EXPBUF, struct dmx_exportbuffer *argp)``

Arguments(参数)
===============

``fd``
    File descriptor returned by :c:func:`open()`.
    (:c:func:`open()` 返回的文件描述符。)

``argp``
    Pointer to struct :c:type:`dmx_exportbuffer`.
    (指向 struct :c:type:`dmx_exportbuffer` 的指针。)

Description(描述)
=================

This ioctl is an extension to the memory mapping I/O method.
It can be used to export a buffer as a DMABUF file at any time after
buffers have been allocated with the :ref:`DMX_REQBUFS` ioctl.
(此 ioctl 是内存映射 I/O 方法的扩展。在用 :ref:`DMX_REQBUFS` ioctl
分配缓冲区之后的任何时刻，都可以用它把缓冲区导出为 DMABUF 文件。)

To export a buffer, applications fill struct :c:type:`dmx_exportbuffer`.
Applications must set the ``index`` field. Valid index numbers
range from zero to the number of buffers allocated with :ref:`DMX_REQBUFS`
(struct :c:type:`dmx_requestbuffers` ``count``) minus one.
Additional flags may be posted in the ``flags`` field. Refer to a manual
for open() for details. Currently only O_CLOEXEC, O_RDONLY, O_WRONLY,
and O_RDWR are supported.
All other fields must be set to zero. In the
case of multi-planar API, every plane is exported separately using
multiple :ref:`DMX_EXPBUF` calls.
(要导出缓冲区，应用程序填写 struct :c:type:`dmx_exportbuffer`。
应用程序必须设置 ``index`` 字段。有效的索引号范围是从 0 到用
:ref:`DMX_REQBUFS` 分配的缓冲区数量(struct
:c:type:`dmx_requestbuffers` 的 ``count``)减 1。可以在 ``flags``
字段中给出额外的标志。详情参阅 open() 的手册。目前只支持
O_CLOEXEC、O_RDONLY、O_WRONLY 和 O_RDWR。所有其他字段必须置零。
在使用多平面 API 的情况下，每个平面都用单独的 :ref:`DMX_EXPBUF`
调用分别导出。)

After calling :ref:`DMX_EXPBUF` the ``fd`` field will be set by a
driver, on success. This is a DMABUF file descriptor. The application may
pass it to other DMABUF-aware devices. It is recommended to close a DMABUF
file when it is no longer used to allow the associated memory to be reclaimed.
(调用 :ref:`DMX_EXPBUF` 之后，成功时驱动程序会设置 ``fd`` 字段。
这是一个 DMABUF 文件描述符。应用程序可以把它传给其他支持 DMABUF 的
设备。建议在 DMABUF 文件不再使用时将其关闭，以便回收相关联的内存。)

Examples(示例)
==============

.. code-block:: c

    int buffer_export(int v4lfd, enum dmx_buf_type bt, int index, int *dmafd)
    {
	struct dmx_exportbuffer expbuf;

	memset(&expbuf, 0, sizeof(expbuf));
	expbuf.type = bt;
	expbuf.index = index;
	if (ioctl(v4lfd, DMX_EXPBUF, &expbuf) == -1) {
	    perror("DMX_EXPBUF");
	    return -1;
	}

	*dmafd = expbuf.fd;

	return 0;
    }

Return Value(返回值)
====================

On success 0 is returned, on error -1 and the ``errno`` variable is set
appropriately. The generic error codes are described at the
:ref:`Generic Error Codes <gen-errors>` chapter.
(成功时返回 0，出错时返回 -1 并适当设置 ``errno`` 变量。通用错误码在
:ref:`Generic Error Codes <gen-errors>` 一章中描述。)

EINVAL
    A queue is not in MMAP mode or DMABUF exporting is not supported or
    ``flags`` or ``index`` fields are invalid.
    (队列不在 MMAP 模式，或 DMABUF 导出不受支持，或 ``flags`` 或
    ``index`` 字段无效。)
