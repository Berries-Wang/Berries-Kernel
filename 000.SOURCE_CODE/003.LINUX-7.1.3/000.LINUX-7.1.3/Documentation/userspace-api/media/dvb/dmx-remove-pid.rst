.. SPDX-License-Identifier: GFDL-1.1-no-invariants-or-later
.. c:namespace:: DTV.dmx

.. _DMX_REMOVE_PID:

========================
DMX_REMOVE_PID(移除 PID)
========================

Name(名称)
----------

DMX_REMOVE_PID

Synopsis(概要)
--------------

.. c:macro:: DMX_REMOVE_PID

``int ioctl(fd, DMX_REMOVE_PID, __u16 *pid)``

Arguments(参数)
---------------

``fd``
    File descriptor returned by :c:func:`open()`.
    (由 :c:func:`open()` 返回的文件描述符。)

``pid``
    PID of the PES filter to be removed.
    (要移除的 PES 过滤器的 PID。)

Description(描述)
-----------------

This ioctl call allows to remove a PID when multiple PIDs are set on a
transport stream filter, e. g. a filter previously set up with output
equal to :c:type:`DMX_OUT_TSDEMUX_TAP <dmx_output>`, created via either
:ref:`DMX_SET_PES_FILTER` or :ref:`DMX_ADD_PID`.
(当在传输流过滤器上设置了多个 PID 时，此 ioctl 调用允许移除其中一个 PID，
例如通过 :ref:`DMX_SET_PES_FILTER` 或 :ref:`DMX_ADD_PID` 创建的、之前设置
为输出等于 :c:type:`DMX_OUT_TSDEMUX_TAP <dmx_output>` 的过滤器。)

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
