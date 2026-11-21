.. SPDX-License-Identifier: GFDL-1.1-no-invariants-or-later
.. c:namespace:: MC

.. _media-func-open:

**************************
media open()(打开媒体设备)
**************************

Name(名称)
==========

media-open - Open a media device(media-open - 打开媒体设备)

Synopsis(概要)
==============

.. code-block:: c

    #include <fcntl.h>

.. c:function:: int open( const char *device_name, int flags )

Arguments(参数)
===============

``device_name``
    Device to be opened.
    (要打开的设备。)

``flags``
    Open flags. Access mode must be either ``O_RDONLY`` or ``O_RDWR``.
    Other flags have no effect.
    (打开标志。访问模式必须是 ``O_RDONLY`` 或 ``O_RDWR``。其他标志没有
    效果。)

Description(描述)
=================

To open a media device applications call :c:func:`open()` with the
desired device name. The function has no side effects; the device
configuration remain unchanged.
(要打开媒体设备，应用以期望的设备名调用 :c:func:`open()`。该函数没有
副作用；设备配置保持不变。)

When the device is opened in read-only mode, attempts to modify its
configuration will result in an error, and ``errno`` will be set to
EBADF.
(当设备以只读模式打开时，修改其配置的尝试会报错，且 ``errno`` 会被设置为
EBADF。)

Return Value(返回值)
====================

:c:func:`open()` returns the new file descriptor on success. On error,
-1 is returned, and ``errno`` is set appropriately. Possible error codes
are:
(:c:func:`open()` 成功时返回新的文件描述符。出错时返回 -1，并相应设置
``errno``。可能的错误码有：)

EACCES
    The requested access to the file is not allowed.
    (不允许对文件进行所请求的访问。)

EMFILE
    The process already has the maximum number of files open.
    (进程已打开的文件数已达上限。)

ENFILE
    The system limit on the total number of open files has been reached.
    (已达到系统对打开文件总数的限制。)

ENOMEM
    Insufficient kernel memory was available.
    (可用内核内存不足。)

ENXIO
    No device corresponding to this device special file exists.
    (不存在与此设备特殊文件对应的设备。)
