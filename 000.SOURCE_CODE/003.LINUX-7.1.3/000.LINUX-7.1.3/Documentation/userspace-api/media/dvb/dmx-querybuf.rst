.. SPDX-License-Identifier: GFDL-1.1-no-invariants-or-later
.. c:namespace:: DTV.dmx

.. _DMX_QUERYBUF:

**********************************
ioctl DMX_QUERYBUF(查询缓冲区状态)
**********************************

Name(名称)
==========

DMX_QUERYBUF - Query the status of a buffer(DMX_QUERYBUF - 查询缓冲区的状态)

.. warning:: this API is still experimental
    (注意：此 API 仍处于实验阶段。)

Synopsis(概要)
==============

.. c:macro:: DMX_QUERYBUF

``int ioctl(int fd, DMX_QUERYBUF, struct dvb_buffer *argp)``

Arguments(参数)
===============

``fd``
    File descriptor returned by :c:func:`open()`.
    (由 :c:func:`open()` 返回的文件描述符。)

``argp``
    Pointer to struct :c:type:`dvb_buffer`.
    (指向 struct :c:type:`dvb_buffer` 的指针。)

Description(描述)
=================

This ioctl is part of the mmap streaming I/O method. It can
be used to query the status of a buffer at any time after buffers have
been allocated with the :ref:`DMX_REQBUFS` ioctl.
(此 ioctl 属于 mmap 流式 I/O 方法。在用 :ref:`DMX_REQBUFS` ioctl 分配缓冲区
之后，可以在任何时刻用它查询缓冲区的状态。)

Applications set the ``index`` field. Valid index numbers range from zero
to the number of buffers allocated with :ref:`DMX_REQBUFS`
(struct :c:type:`dvb_requestbuffers` ``count``) minus one.
(应用设置 ``index`` 字段。有效索引号范围从零到用 :ref:`DMX_REQBUFS` 分配的
缓冲区数量 (struct :c:type:`dvb_requestbuffers` 的 ``count``) 减一。)

After calling :ref:`DMX_QUERYBUF` with a pointer to this structure,
drivers return an error code or fill the rest of the structure.
(以指向该结构的指针调用 :ref:`DMX_QUERYBUF` 之后，驱动要么返回一个错误
码，要么填充该结构的其余部分。)

On success, the ``offset`` will contain the offset of the buffer from the
start of the device memory, the ``length`` field its size, and the
``bytesused`` the number of bytes occupied by data in the buffer (payload).
(成功时，``offset`` 将包含缓冲区相对于设备内存起始处的偏移，``length``
字段为其大小，``bytesused`` 为缓冲区中数据占用的字节数 (有效载荷)。)

Return Value(返回值)
====================

On success 0 is returned, the ``offset`` will contain the offset of the
buffer from the start of the device memory, the ``length`` field its size,
and the ``bytesused`` the number of bytes occupied by data in the buffer
(payload).
(成功时返回 0，``offset`` 将包含缓冲区相对于设备内存起始处的偏移，
``length`` 字段为其大小，``bytesused`` 为缓冲区中数据占用的字节数
(有效载荷)。)

On error it returns -1 and the ``errno`` variable is set
appropriately. The generic error codes are described at the
:ref:`Generic Error Codes <gen-errors>` chapter.
(出错时返回 -1，并相应设置 ``errno`` 变量。通用错误码在
:ref:`Generic Error Codes <gen-errors>` 一章中描述。)

EINVAL
    The ``index`` is out of bounds.
    (``index`` 越界。)
