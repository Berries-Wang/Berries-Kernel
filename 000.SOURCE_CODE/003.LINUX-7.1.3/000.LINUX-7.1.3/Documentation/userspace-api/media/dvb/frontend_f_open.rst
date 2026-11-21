.. SPDX-License-Identifier: GFDL-1.1-no-invariants-or-later
.. c:namespace:: DTV.fe

.. _frontend_f_open:

***********************************************
Digital TV frontend open()(数字电视前端 open())
***********************************************

Name(名称)
==========

fe-open - Open a frontend device(fe-open - 打开前端设备)

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
    Open flags. Access can either be ``O_RDWR`` or ``O_RDONLY``.
    (打开标志。访问方式可以是 ``O_RDWR`` 或 ``O_RDONLY``。)

    Multiple opens are allowed with ``O_RDONLY``. In this mode, only
    query and read ioctls are allowed.
    (``O_RDONLY`` 方式允许多次打开。在此模式下，只允许查询和读取类
    ioctl。)

    Only one open is allowed in ``O_RDWR``. In this mode, all ioctls are
    allowed.
    (``O_RDWR`` 方式只允许打开一次。在此模式下，允许所有 ioctl。)

    When the ``O_NONBLOCK`` flag is given, the system calls may return
    ``EAGAIN`` error code when no data is available or when the device
    driver is temporarily busy.
    (当给出 ``O_NONBLOCK`` 标志时，在没有数据可用或设备驱动程序暂时
    繁忙时，系统调用可能返回 ``EAGAIN`` 错误码。)

    Other flags have no effect.
    (其他标志没有效果。)

Description(描述)
=================

This system call opens a named frontend device
(``/dev/dvb/adapter?/frontend?``) for subsequent use. Usually the first
thing to do after a successful open is to find out the frontend type
with :ref:`FE_GET_INFO`.
(此系统调用打开一个命名的前端设备(``/dev/dvb/adapter?/frontend?``)
供后续使用。成功打开之后通常要做的第一件事是用 :ref:`FE_GET_INFO`
查明前端类型。)

The device can be opened in read-only mode, which only allows monitoring
of device status and statistics, or read/write mode, which allows any
kind of use (e.g. performing tuning operations.)
(设备可以以只读模式打开，这只允许监控设备状态和统计信息；或以读/写
模式打开，这允许任何类型的使用(例如执行调谐操作)。)

In a system with multiple front-ends, it is usually the case that
multiple devices cannot be open in read/write mode simultaneously. As
long as a front-end device is opened in read/write mode, other open()
calls in read/write mode will either fail or block, depending on whether
non-blocking or blocking mode was specified. A front-end device opened
in blocking mode can later be put into non-blocking mode (and vice
versa) using the F_SETFL command of the fcntl system call. This is a
standard system call, documented in the Linux manual page for fcntl.
When an open() call has succeeded, the device will be ready for use in
the specified mode. This implies that the corresponding hardware is
powered up, and that other front-ends may have been powered down to make
that possible.
(在具有多个前端的系统中，通常不能同时以读/写模式打开多个设备。只要
一个前端设备以读/写模式打开，其他以读/写模式的 open() 调用要么失败
要么阻塞，取决于指定的是非阻塞还是阻塞模式。以阻塞模式打开的前端
设备之后可以用 fcntl 系统调用的 F_SETFL 命令切换到非阻塞模式(反之
亦然)。这是一个标准的系统调用，在 Linux 的 fcntl 手册页中有文档。
当 open() 调用成功后，设备就可按指定模式使用了。这意味着相应的硬件
已上电，而其他前端可能已被断电以做到这一点。)

Return Value(返回值)
====================

On success :c:func:`open()` returns the new file descriptor.
On error, -1 is returned, and the ``errno`` variable is set appropriately.
(成功时 :c:func:`open()` 返回新的文件描述符。出错时返回 -1，并适当
设置 ``errno`` 变量。)

Possible error codes are:
(可能的错误码有：)

On success 0 is returned, and :c:type:`ca_slot_info` is filled.
(成功时返回 0，并填充 :c:type:`ca_slot_info`。)

On error -1 is returned, and the ``errno`` variable is set
appropriately.
(出错时返回 -1，并适当设置 ``errno`` 变量。)

.. tabularcolumns:: |p{2.5cm}|p{15.0cm}|

.. flat-table::
    :header-rows:  0
    :stub-columns: 0
    :widths: 1 16

    -  - ``EPERM``
       -  The caller has no permission to access the device.
          (调用者没有访问该设备的权限。)

    -  - ``EBUSY``
       -  The device driver is already in use.
          (设备驱动程序已被占用。)

    -  - ``EMFILE``
       -  The process already has the maximum number of files open.
          (进程已达到可打开文件数的上限。)

    -  - ``ENFILE``
       -  The limit on the total number of files open on the system has been
	  reached.
          (已达到系统可打开文件总数的上限。)

The generic error codes are described at the
:ref:`Generic Error Codes <gen-errors>` chapter.
(通用错误码在 :ref:`Generic Error Codes <gen-errors>` 一章中描述。)
