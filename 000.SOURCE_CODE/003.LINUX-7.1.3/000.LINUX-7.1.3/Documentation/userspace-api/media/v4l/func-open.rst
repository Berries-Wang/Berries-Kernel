.. SPDX-License-Identifier: GFDL-1.1-no-invariants-or-later
.. c:namespace:: V4L

.. _func-open:

***********
V4L2 open()
***********

Name(名称)
==========

v4l2-open - Open a V4L2 device
(v4l2-open - 打开一个 V4L2 设备。)

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
    Open flags. Access mode must be ``O_RDWR``. This is just a
    technicality, input devices still support only reading and output
    devices only writing.
    (打开标志。访问模式必须是 ``O_RDWR`` 。这只是一个技术细节：输入设
    备仍然只支持读取，而输出设备只支持写入。)

    When the ``O_NONBLOCK`` flag is given, the :c:func:`read()`
    function and the :ref:`VIDIOC_DQBUF <VIDIOC_QBUF>` ioctl will
    return the ``EAGAIN`` error code when no data is available or no
    buffer is in the driver outgoing queue, otherwise these functions
    block until data becomes available. All V4L2 drivers exchanging data
    with applications must support the ``O_NONBLOCK`` flag.
    (当给出 ``O_NONBLOCK`` 标志时， :c:func:`read()` 函数与
    :ref:`VIDIOC_DQBUF <VIDIOC_QBUF>` ioctl 会在没有可用数据、或驱动传
    出队列中没有缓冲区时返回 ``EAGAIN`` 错误码；否则这些函数会阻塞，直
    到数据可用为止。所有与应用程序交换数据的 V4L2 驱动程序都必须支持
    ``O_NONBLOCK`` 标志。)

    Other flags have no effect.
    (其他标志没有影响。)

Description(描述)
=================

To open a V4L2 device applications call :c:func:`open()` with the
desired device name. This function has no side effects; all data format
parameters, current input or output, control values or other properties
remain unchanged. At the first :c:func:`open()` call after loading the
driver they will be reset to default values, drivers are never in an
undefined state.
(应用程序用所需的设备名调用 :c:func:`open()` 来打开 V4L2 设备。该函
数没有副作用；所有数据格式参数、当前输入或输出、控件值或其他属性都
保持不变。在加载驱动程序之后的第一次 :c:func:`open()` 调用时，它们会
被重置为默认值；驱动程序永远不会处于未定义状态。)

Return Value(返回值)
====================

On success :c:func:`open()` returns the new file descriptor. On error
-1 is returned, and the ``errno`` variable is set appropriately.
Possible error codes are:
(成功时 :c:func:`open()` 返回新的文件描述符。出错时返回 -1，并适当设
置 ``errno`` 变量。可能的错误码：)

EACCES
    The caller has no permission to access the device.
    (调用者没有访问该设备的权限。)

EBUSY
    The driver does not support multiple opens and the device is already
    in use.
    (驱动程序不支持多重打开，且设备已在使用中。)

ENODEV
    Device not found or was removed.
    (未找到设备，或设备已被移除。)

ENOMEM
    Not enough kernel memory was available to complete the request.
    (没有足够的内核内存来完成该请求。)

EMFILE
    The process already has the maximum number of files open.
    (该进程已打开的文件数已达上限。)

ENFILE
    The limit on the total number of files open on the system has been
    reached.
    (系统上打开文件总数的上限已经达到。)
