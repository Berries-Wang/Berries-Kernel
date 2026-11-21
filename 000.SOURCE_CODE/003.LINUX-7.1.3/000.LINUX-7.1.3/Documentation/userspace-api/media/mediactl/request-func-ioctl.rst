.. SPDX-License-Identifier: GPL-2.0 OR GFDL-1.1-no-invariants-or-later
.. c:namespace:: MC

.. _request-func-ioctl:

***********************************
request ioctl()(控制请求文件描述符)
***********************************

Name(名称)
==========

request-ioctl - Control a request file descriptor(request-ioctl - 控制请求文件描述符)

Synopsis(概要)
==============

.. code-block:: c

    #include <sys/ioctl.h>

``int ioctl(int fd, int cmd, void *argp)``

Arguments(参数)
===============

``fd``
    File descriptor returned by :ref:`MEDIA_IOC_REQUEST_ALLOC`.
    (由 :ref:`MEDIA_IOC_REQUEST_ALLOC` 返回的文件描述符。)

``cmd``
    The request ioctl command code as defined in the media.h header file, for
    example :ref:`MEDIA_REQUEST_IOC_QUEUE`.
    (media.h 头文件中定义的请求 ioctl 命令码，例如
    :ref:`MEDIA_REQUEST_IOC_QUEUE`。)

``argp``
    Pointer to a request-specific structure.
    (指向随命令而定的结构的指针。)

Description(描述)
=================

The :ref:`ioctl() <request-func-ioctl>` function manipulates request
parameters. The argument ``fd`` must be an open file descriptor.
(:ref:`ioctl() <request-func-ioctl>` 函数操作请求参数。参数 ``fd`` 必须是
一个已打开的文件描述符。)

The ioctl ``cmd`` code specifies the request function to be called. It
has encoded in it whether the argument is an input, output or read/write
parameter, and the size of the argument ``argp`` in bytes.
(ioctl 的 ``cmd`` 码指定要调用的请求功能。其中编码了参数是输入、输出还是
读/写参数，以及参数 ``argp`` 的字节大小。)

Macros and structures definitions specifying request ioctl commands and
their parameters are located in the media.h header file. All request ioctl
commands, their respective function and parameters are specified in
:ref:`media-user-func`.
(指定请求 ioctl 命令及其参数的宏和结构定义位于 media.h 头文件中。所有请求
ioctl 命令及其相应的功能与参数在 :ref:`media-user-func` 中说明。)

Return Value(返回值)
====================

On success 0 is returned, on error -1 and the ``errno`` variable is set
appropriately. The generic error codes are described at the
:ref:`Generic Error Codes <gen-errors>` chapter.
(成功时返回 0；出错时返回 -1，并相应设置 ``errno`` 变量。通用错误码在
:ref:`Generic Error Codes <gen-errors>` 一章中描述。)

Command-specific error codes are listed in the individual command
descriptions.
(各命令特有的错误码列在各自的命令说明中。)

When an ioctl that takes an output or read/write parameter fails, the
parameter remains unmodified.
(当带有输出或读/写参数的 ioctl 失败时，该参数保持不变。)
