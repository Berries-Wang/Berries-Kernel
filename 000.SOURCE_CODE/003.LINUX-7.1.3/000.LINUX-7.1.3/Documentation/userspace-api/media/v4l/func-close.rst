.. SPDX-License-Identifier: GFDL-1.1-no-invariants-or-later
.. c:namespace:: V4L

.. _func-close:

***********************
V4L2 close()(V4L2 关闭)
***********************

Name(名称)
==========

v4l2-close - Close a V4L2 device
(v4l2-close - 关闭 V4L2 设备)

Synopsis(概要)
==============

.. code-block:: c

    #include <unistd.h>

.. c:function:: int close( int fd )

Arguments(参数)
===============

``fd``
    File descriptor returned by :c:func:`open()`.
    (由 :c:func:`open()` 返回的文件描述符。)

Description(描述)
=================

Closes the device. Any I/O in progress is terminated and resources
associated with the file descriptor are freed. However data format
parameters, current input or output, control values or other properties
remain unchanged.
(关闭设备。任何进行中的 I/O 都会被终止，与该文件描述符关联的资源会被释放。
但数据格式参数、当前输入或输出、控制值以及其他属性保持不变。)

Return Value(返回值)
====================

The function returns 0 on success, -1 on failure and the ``errno`` is
set appropriately. Possible error codes:
(该函数成功时返回 0，失败时返回 -1 并相应设置 ``errno``。可能的错误码：)

EBADF
    ``fd`` is not a valid open file descriptor.
    (``fd`` 不是有效的已打开文件描述符。)
