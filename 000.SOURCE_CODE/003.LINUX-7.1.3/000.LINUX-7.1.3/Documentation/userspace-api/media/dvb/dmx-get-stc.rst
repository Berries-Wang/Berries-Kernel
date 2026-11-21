.. SPDX-License-Identifier: GFDL-1.1-no-invariants-or-later
.. c:namespace:: DTV.dmx

.. _DMX_GET_STC:

===============================
DMX_GET_STC(获取系统时间计数器)
===============================

Name(名称)
----------

DMX_GET_STC

Synopsis(概要)
--------------

.. c:macro:: DMX_GET_STC

``int ioctl(int fd, DMX_GET_STC, struct dmx_stc *stc)``

Arguments(参数)
---------------

``fd``
    File descriptor returned by :c:func:`open()`.
    (由 :c:func:`open()` 返回的文件描述符。)

``stc``
    Pointer to :c:type:`dmx_stc` where the stc data is to be stored.
    (指向 :c:type:`dmx_stc` 的指针，stc 数据将存储在其中。)

Description(描述)
-----------------

This ioctl call returns the current value of the system time counter
(which is driven by a PES filter of type :c:type:`DMX_PES_PCR <dmx_ts_pes>`).
Some hardware supports more than one STC, so you must specify which one by
setting the :c:type:`num <dmx_stc>` field of stc before the ioctl (range 0...n).
The result is returned in form of a ratio with a 64 bit numerator
and a 32 bit denominator, so the real 90kHz STC value is
``stc->stc / stc->base``.
(此 ioctl 调用返回系统时间计数器的当前值 (它由类型为
:c:type:`DMX_PES_PCR <dmx_ts_pes>` 的 PES 过滤器驱动)。有些硬件支持多个
STC，因此必须在 ioctl 之前通过设置 stc 的 :c:type:`num <dmx_stc>` 字段来
指定使用哪一个 (范围 0...n)。结果以比值形式返回，分子 64 位，分母 32 位，
因此真实的 90kHz STC 值为 ``stc->stc / stc->base``。)

Return Value(返回值)
--------------------

On success 0 is returned.
(成功时返回 0。)

On error -1 is returned, and the ``errno`` variable is set
appropriately.
(出错时返回 -1，并相应设置 ``errno`` 变量。)

.. tabularcolumns:: |p{2.5cm}|p{15.0cm}|

.. flat-table::
    :header-rows:  0
    :stub-columns: 0
    :widths: 1 16

    -  .. row 1

       -  ``EINVAL``

       -  Invalid stc number.(无效的 stc 编号。)

The generic error codes are described at the
:ref:`Generic Error Codes <gen-errors>` chapter.
(通用错误码在 :ref:`Generic Error Codes <gen-errors>` 一章中描述。)
