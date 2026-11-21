.. SPDX-License-Identifier: GFDL-1.1-no-invariants-or-later
.. c:namespace:: CEC

.. _cec-func-open:

**********
cec open()
**********

Name(名称)
==========

cec-open - Open a cec device
(cec-open - 打开一个 cec 设备。)

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
    Open flags. Access mode must be ``O_RDWR``.
    (打开标志。访问模式必须是 ``O_RDWR`` 。)

    When the ``O_NONBLOCK`` flag is given, the
    :ref:`CEC_RECEIVE <CEC_RECEIVE>` and :ref:`CEC_DQEVENT <CEC_DQEVENT>` ioctls
    will return the ``EAGAIN`` error code when no message or event is available, and
    ioctls :ref:`CEC_TRANSMIT <CEC_TRANSMIT>`,
    :ref:`CEC_ADAP_S_PHYS_ADDR <CEC_ADAP_S_PHYS_ADDR>` and
    :ref:`CEC_ADAP_S_LOG_ADDRS <CEC_ADAP_S_LOG_ADDRS>`
    all return 0.
    (当给出 ``O_NONBLOCK`` 标志时， :ref:`CEC_RECEIVE <CEC_RECEIVE>` 与
    :ref:`CEC_DQEVENT <CEC_DQEVENT>` ioctl 在没有可用消息或事件时返回
    ``EAGAIN`` 错误码，而 ioctl :ref:`CEC_TRANSMIT <CEC_TRANSMIT>` 、
    :ref:`CEC_ADAP_S_PHYS_ADDR <CEC_ADAP_S_PHYS_ADDR>` 与
    :ref:`CEC_ADAP_S_LOG_ADDRS <CEC_ADAP_S_LOG_ADDRS>` 则都返回 0。)

    Other flags have no effect.
    (其他标志没有影响。)

Description(描述)
=================

To open a cec device applications call :c:func:`open()` with the
desired device name. The function has no side effects; the device
configuration remain unchanged.
(应用程序用所需的设备名调用 :c:func:`open()` 来打开 cec 设备。该函数
没有副作用；设备配置保持不变。)

When the device is opened in read-only mode, attempts to modify its
configuration will result in an error, and ``errno`` will be set to
EBADF.
(当设备以只读模式打开时，修改其配置的尝试将导致一个错误，且 ``errno``
会被设置为 EBADF。)

Return Value(返回值)
====================

:c:func:`open()` returns the new file descriptor on success. On error,
-1 is returned, and ``errno`` is set appropriately. Possible error codes
include:
(成功时 :c:func:`open()` 返回新的文件描述符。出错时返回 -1，并适当设
置 ``errno`` 。可能的错误码包括：)

``EACCES``
    The requested access to the file is not allowed.
    (不允许对文件进行所请求的访问。)

``EMFILE``
    The process already has the maximum number of files open.
    (该进程已打开的文件数已达上限。)

``ENFILE``
    The system limit on the total number of open files has been reached.
    (已达到系统对打开文件总数的限制。)

``ENOMEM``
    Insufficient kernel memory was available.
    (内核内存不足。)

``ENODEV``
    Device not found or was removed.
    (未找到设备，或设备已被移除。)
