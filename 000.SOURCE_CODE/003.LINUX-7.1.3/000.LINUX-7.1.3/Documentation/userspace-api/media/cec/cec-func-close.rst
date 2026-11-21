.. SPDX-License-Identifier: GFDL-1.1-no-invariants-or-later
.. c:namespace:: CEC

.. _cec-func-close:

*************************
cec close()(CEC 设备关闭)
*************************

Name(名称)
==========

cec-close - Close a cec device
(cec-close - 关闭 CEC 设备)

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

Closes the cec device. Resources associated with the file descriptor are
freed. The device configuration remain unchanged.
(关闭 CEC 设备。与文件描述符关联的资源会被释放。设备配置保持不变。)

Return Value(返回值)
====================

:c:func:`close()` returns 0 on success. On error, -1 is returned, and
``errno`` is set appropriately. Possible error codes are:
(:c:func:`close()` 成功时返回 0。出错时返回 -1，并相应设置 ``errno``。
可能的错误码：)

``EBADF``
    ``fd`` is not a valid open file descriptor.
    (``fd`` 不是有效的已打开文件描述符。)
