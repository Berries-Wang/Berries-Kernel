.. SPDX-License-Identifier: GFDL-1.1-no-invariants-or-later
.. c:namespace:: DTV.dmx

.. _DMX_STOP:

==================
DMX_STOP(停止过滤)
==================

Name(名称)
----------

DMX_STOP

Synopsis(概要)
--------------

.. c:macro:: DMX_STOP

``int ioctl(int fd, DMX_STOP)``

Arguments(参数)
---------------

``fd``
    File descriptor returned by :c:func:`open()`.
    (由 :c:func:`open()` 返回的文件描述符。)

Description(描述)
-----------------

This ioctl call is used to stop the actual filtering operation defined
via the ioctl calls :ref:`DMX_SET_FILTER` or :ref:`DMX_SET_PES_FILTER` and
started via the :ref:`DMX_START` command.
(此 ioctl 调用用于停止由 :ref:`DMX_SET_FILTER` 或 :ref:`DMX_SET_PES_FILTER`
ioctl 调用定义、且经 :ref:`DMX_START` 命令启动的实际过滤操作。)

Return Value(返回值)
--------------------

On success 0 is returned.
(成功时返回 0。)

On error -1 is returned, and the ``errno`` variable is set
appropriately.
(出错时返回 -1，并相应设置 ``errno`` 变量。)

The generic error codes are described at the
:ref:`Generic Error Codes <gen-errors>` chapter.
(通用错误码在 :ref:`Generic Error Codes <gen-errors>` 一章中描述。)
