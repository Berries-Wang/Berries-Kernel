.. SPDX-License-Identifier: GFDL-1.1-no-invariants-or-later
.. c:namespace:: DTV.dmx

.. _DMX_SET_FILTER:

==========================
DMX_SET_FILTER(设置过滤器)
==========================

Name(名称)
----------

DMX_SET_FILTER

Synopsis(概要)
--------------

.. c:macro:: DMX_SET_FILTER

``int ioctl(int fd, DMX_SET_FILTER, struct dmx_sct_filter_params *params)``

Arguments(参数)
---------------

``fd``
    File descriptor returned by :c:func:`open()`.
    (由 :c:func:`open()` 返回的文件描述符。)

``params``

    Pointer to structure containing filter parameters.
    (指向包含过滤参数的结构的指针。)

Description(描述)
-----------------

This ioctl call sets up a filter according to the filter and mask
parameters provided. A timeout may be defined stating number of seconds
to wait for a section to be loaded. A value of 0 means that no timeout
should be applied. Finally there is a flag field where it is possible to
state whether a section should be CRC-checked, whether the filter should
be a "one-shot" filter, i.e. if the filtering operation should be
stopped after the first section is received, and whether the filtering
operation should be started immediately (without waiting for a
:ref:`DMX_START` ioctl call). If a filter was previously set-up, this
filter will be canceled, and the receive buffer will be flushed.
(此 ioctl 调用根据提供的过滤器和掩码参数设置一个过滤器。可以定义一个超时，
指出等待一个分段 (section) 装载的秒数。值为 0 表示不应用超时。最后还有一个
标志字段，可用于说明是否要对分段做 CRC 校验、过滤器是否应为"一次性"
(one-shot) 过滤器 (即是否在收到第一个分段后停止过滤操作)，以及过滤操作是否
应立即开始 (而不等待 :ref:`DMX_START` ioctl 调用)。如果之前已设置过过滤器，
该过滤器将被取消，接收缓冲区将被清空。)

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
