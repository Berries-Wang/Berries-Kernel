.. SPDX-License-Identifier: GFDL-1.1-no-invariants-or-later
.. c:namespace:: V4L

.. _func-ioctl:

*****************************
V4L2 ioctl()(V4L2 ioctl 调用)
*****************************

Name(名称)
==========

v4l2-ioctl - Program a V4L2 device(v4l2-ioctl - 对 V4L2 设备进行编程)

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
    V4L2 ioctl request code as defined in the ``videodev2.h`` header
    file, for example VIDIOC_QUERYCAP.
    (``videodev2.h`` 头文件中定义的 V4L2 ioctl 请求码，例如
    VIDIOC_QUERYCAP。)

``argp``
    Pointer to a function parameter, usually a structure.
    (指向功能参数的指针，通常是一个结构。)

Description(描述)
=================

The :ref:`ioctl() <func-ioctl>` function is used to program V4L2 devices. The
argument ``fd`` must be an open file descriptor. An ioctl ``request``
has encoded in it whether the argument is an input, output or read/write
parameter, and the size of the argument ``argp`` in bytes. Macros and
defines specifying V4L2 ioctl requests are located in the
``videodev2.h`` header file. Applications should use their own copy, not
include the version in the kernel sources on the system they compile on.
All V4L2 ioctl requests, their respective function and parameters are
specified in :ref:`user-func`.
(:ref:`ioctl() <func-ioctl>` 函数用于对 V4L2 设备进行编程。参数 ``fd``
必须是一个已打开的文件描述符。ioctl 的 ``request`` 码中编码了参数是输入、
输出还是读/写参数，以及参数 ``argp`` 的字节大小。指定 V4L2 ioctl 请求的宏
和定义位于 ``videodev2.h`` 头文件中。应用应使用自己的副本，而不是包含其
编译系统上内核源码中的那份。所有 V4L2 ioctl 请求及其相应的功能与参数在
:ref:`user-func` 中说明。)

Return Value(返回值)
====================

On success 0 is returned, on error -1 and the ``errno`` variable is set
appropriately. The generic error codes are described at the
:ref:`Generic Error Codes <gen-errors>` chapter.
(成功时返回 0；出错时返回 -1，并相应设置 ``errno`` 变量。通用错误码在
:ref:`Generic Error Codes <gen-errors>` 一章中描述。)

When an ioctl that takes an output or read/write parameter fails, the
parameter remains unmodified.
(当带有输出或读/写参数的 ioctl 失败时，该参数保持不变。)
