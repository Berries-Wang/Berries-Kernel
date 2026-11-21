.. SPDX-License-Identifier: GFDL-1.1-no-invariants-or-later
.. c:namespace:: DTV.ca

.. _ca_fclose:

=======================================
Digital TV CA close()(数字电视 CA 关闭)
=======================================

Name(名称)
----------

Digital TV CA close()
(数字电视 CA 关闭。)

Synopsis(概要)
--------------

.. c:function:: int close(int fd)

Arguments(参数)
---------------

``fd``
  File descriptor returned by a previous call to :c:func:`open()`.
  (由之前对 :c:func:`open()` 的调用返回的文件描述符。)

Description(描述)
-----------------

This system call closes a previously opened CA device.
(此系统调用关闭先前打开的 CA 设备。)

Return Value(返回值)
--------------------

On success 0 is returned.
(成功时返回 0。)

On error -1 is returned, and the ``errno`` variable is set
appropriately.
(出错时返回 -1，并相应设置 ``errno`` 变量。)

Generic error codes are described at the
:ref:`Generic Error Codes <gen-errors>` chapter.
(通用错误码在 :ref:`Generic Error Codes <gen-errors>` 一章中描述。)
