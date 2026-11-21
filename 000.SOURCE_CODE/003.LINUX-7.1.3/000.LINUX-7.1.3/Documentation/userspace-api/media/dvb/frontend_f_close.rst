.. SPDX-License-Identifier: GFDL-1.1-no-invariants-or-later
.. c:namespace:: DTV.fe

.. _frontend_f_close:

*********************************************
Digital TV frontend close()(数字电视前端关闭)
*********************************************

Name(名称)
==========

fe-close - Close a frontend device
(fe-close - 关闭前端设备)

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

This system call closes a previously opened front-end device. After
closing a front-end device, its corresponding hardware might be powered
down automatically.
(此系统调用关闭先前打开的前端设备。关闭前端设备后，其对应的硬件可能会自动
断电。)

Return Value(返回值)
====================

On success 0 is returned.
(成功时返回 0。)

On error -1 is returned, and the ``errno`` variable is set
appropriately.
(出错时返回 -1，并相应设置 ``errno`` 变量。)

Generic error codes are described at the
:ref:`Generic Error Codes <gen-errors>` chapter.
(通用错误码在 :ref:`Generic Error Codes <gen-errors>` 一章中描述。)
