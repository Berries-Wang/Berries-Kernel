.. SPDX-License-Identifier: GPL-2.0 OR GFDL-1.1-no-invariants-or-later
.. c:namespace:: MC.request

.. _request-func-close:

*************************
request close()(请求关闭)
*************************

Name(名称)
==========

request-close - Close a request file descriptor
(request-close - 关闭请求文件描述符)

Synopsis(概要)
==============

.. code-block:: c

    #include <unistd.h>

.. c:function:: int close( int fd )

Arguments(参数)
===============

``fd``
    File descriptor returned by :ref:`MEDIA_IOC_REQUEST_ALLOC`.
    (由 :ref:`MEDIA_IOC_REQUEST_ALLOC` 返回的文件描述符。)

Description(描述)
=================

Closes the request file descriptor. Resources associated with the request
are freed once all file descriptors associated with the request are closed
and the driver has completed the request.
See :ref:`here <media-request-life-time>` for more information.
(关闭请求文件描述符。当与请求关联的所有文件描述符均已关闭且驱动已完成该
请求后，与请求关联的资源即被释放。更多信息参见 :ref:`here
<media-request-life-time>`。)

Return Value(返回值)
====================

:c:func:`close()` returns 0 on success. On error, -1 is
returned, and ``errno`` is set appropriately. Possible error codes are:
(:c:func:`close()` 成功时返回 0。出错时返回 -1，并相应设置 ``errno``。
可能的错误码：)

EBADF
    ``fd`` is not a valid open file descriptor.
    (``fd`` 不是有效的已打开文件描述符。)
