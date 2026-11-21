.. SPDX-License-Identifier: GFDL-1.1-no-invariants-or-later
.. c:namespace:: DTV.dmx

.. _DMX_ADD_PID:

=========================
DMX_ADD_PID(添加 PID)
=========================

Name(名称)
----------

DMX_ADD_PID

Synopsis(概要)
--------------

.. c:macro:: DMX_ADD_PID

``int ioctl(fd, DMX_ADD_PID, __u16 *pid)``

Arguments(参数)
---------------

``fd``
    File descriptor returned by :c:func:`open()`.
    (由 :c:func:`open()` 返回的文件描述符。)

``pid``
   PID number to be filtered.
   (要过滤的 PID 编号。)

Description(描述)
-----------------

This ioctl call allows to add multiple PIDs to a transport stream filter
previously set up with :ref:`DMX_SET_PES_FILTER` and output equal to
:c:type:`DMX_OUT_TSDEMUX_TAP <dmx_output>`.
(此 ioctl 调用允许向之前用 :ref:`DMX_SET_PES_FILTER` 设置、输出等于
:c:type:`DMX_OUT_TSDEMUX_TAP <dmx_output>` 的传输流过滤器添加多个 PID。)

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
