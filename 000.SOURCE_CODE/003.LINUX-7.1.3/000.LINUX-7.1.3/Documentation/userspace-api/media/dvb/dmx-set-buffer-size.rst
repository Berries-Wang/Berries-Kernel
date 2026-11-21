.. SPDX-License-Identifier: GFDL-1.1-no-invariants-or-later
.. c:namespace:: DTV.dmx

.. _DMX_SET_BUFFER_SIZE:

===================================
DMX_SET_BUFFER_SIZE(设置缓冲区大小)
===================================

Name(名称)
----------

DMX_SET_BUFFER_SIZE

Synopsis(概要)
--------------

.. c:macro:: DMX_SET_BUFFER_SIZE

``int ioctl(int fd, DMX_SET_BUFFER_SIZE, unsigned long size)``

Arguments(参数)
---------------

``fd``
    File descriptor returned by :c:func:`open()`.
    (由 :c:func:`open()` 返回的文件描述符。)

``size``
    Unsigned long size
    (无符号长整型的缓冲区大小)

Description(描述)
-----------------

This ioctl call is used to set the size of the circular buffer used for
filtered data. The default size is two maximum sized sections, i.e. if
this function is not called a buffer size of ``2 * 4096`` bytes will be
used.
(此 ioctl 调用用于设置用于已过滤数据的环形缓冲区的大小。默认大小是两个
最大尺寸的分段，即如果不调用此函数，将使用 ``2 * 4096`` 字节的缓冲区
大小。)

Return Value(返回值)
--------------------

On success 0 is returned.
(成功时返回 0。)

On error -1 is returned, and the ``errno`` variable is set
appropriately.
(出错时返回 -1，并相应设置 ``errno`` 变量。)

The generic error codes are described at the
:ref:`Generic Error Codes <gen-errors>` chapter.
(通用错误码在 :ref:`Generic Error Codes <gen-errors>` 一章中描述。)
