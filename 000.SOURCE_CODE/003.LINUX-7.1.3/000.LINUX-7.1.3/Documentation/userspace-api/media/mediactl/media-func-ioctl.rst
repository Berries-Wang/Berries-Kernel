.. SPDX-License-Identifier: GFDL-1.1-no-invariants-or-later
.. c:namespace:: MC

.. _media-func-ioctl:

***************************
media ioctl()(控制媒体设备)
***************************

Name(名称)
==========

media-ioctl - Control a media device(media-ioctl - 控制媒体设备)

Synopsis(概要)
==============

.. code-block:: c

    #include <sys/ioctl.h>

``int ioctl(int fd, int request, void *argp)``

Arguments(参数)
===============

``fd``
    File descriptor returned by :c:func:`open()`.
    (由 :c:func:`open()` 返回的文件描述符。)

``request``
    Media ioctl request code as defined in the media.h header file, for
    example MEDIA_IOC_SETUP_LINK.
    (media.h 头文件中定义的媒体 ioctl 请求码，例如
    MEDIA_IOC_SETUP_LINK。)

``argp``
    Pointer to a request-specific structure.
    (指向随请求而定的结构的指针。)

Description(描述)
=================

The :ref:`ioctl() <media-func-ioctl>` function manipulates media device
parameters. The argument ``fd`` must be an open file descriptor.
(:ref:`ioctl() <media-func-ioctl>` 函数操作媒体设备参数。参数 ``fd`` 必须
是一个已打开的文件描述符。)

The ioctl ``request`` code specifies the media function to be called. It
has encoded in it whether the argument is an input, output or read/write
parameter, and the size of the argument ``argp`` in bytes.
(ioctl 的 ``request`` 码指定要调用的媒体功能。其中编码了参数是输入、输出
还是读/写参数，以及参数 ``argp`` 的字节大小。)

Macros and structures definitions specifying media ioctl requests and
their parameters are located in the media.h header file. All media ioctl
requests, their respective function and parameters are specified in
:ref:`media-user-func`.
(指定媒体 ioctl 请求及其参数的宏和结构定义位于 media.h 头文件中。所有媒体
ioctl 请求及其相应的功能与参数在 :ref:`media-user-func` 中说明。)

Return Value(返回值)
====================

On success 0 is returned, on error -1 and the ``errno`` variable is set
appropriately. The generic error codes are described at the
:ref:`Generic Error Codes <gen-errors>` chapter.
(成功时返回 0；出错时返回 -1，并相应设置 ``errno`` 变量。通用错误码在
:ref:`Generic Error Codes <gen-errors>` 一章中描述。)

Request-specific error codes are listed in the individual requests
descriptions.
(各请求特有的错误码列在各自的请求说明中。)

When an ioctl that takes an output or read/write parameter fails, the
parameter remains unmodified.
(当带有输出或读/写参数的 ioctl 失败时，该参数保持不变。)
