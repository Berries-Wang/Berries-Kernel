.. SPDX-License-Identifier: GFDL-1.1-no-invariants-or-later
.. c:namespace:: DTV.fe

.. _FE_SET_FRONTEND_TUNE_MODE:

*************************************************
ioctl FE_SET_FRONTEND_TUNE_MODE(设置前端调谐模式)
*************************************************

Name(名称)
==========

FE_SET_FRONTEND_TUNE_MODE - Allow setting tuner mode flags to the frontend.(FE_SET_FRONTEND_TUNE_MODE - 允许向前端设置调谐器模式标志。)

Synopsis(概要)
==============

.. c:macro:: FE_SET_FRONTEND_TUNE_MODE

``int ioctl(int fd, FE_SET_FRONTEND_TUNE_MODE, unsigned int flags)``

Arguments(参数)
===============

``fd``
    File descriptor returned by :c:func:`open()`.
    (由 :c:func:`open()` 返回的文件描述符。)

``flags``
    Valid flags:
    (有效标志：)

    -  0 - normal tune mode(0 - 正常调谐模式)

    -  ``FE_TUNE_MODE_ONESHOT`` - When set, this flag will disable any
       zigzagging or other "normal" tuning behaviour. Additionally,
       there will be no automatic monitoring of the lock status, and
       hence no frontend events will be generated. If a frontend device
       is closed, this flag will be automatically turned off when the
       device is reopened read-write.
       (``FE_TUNE_MODE_ONESHOT`` - 设置后，此标志将禁用任何之字形搜索或其他
       "正常"调谐行为。此外，将不再自动监视锁定状态，因此不会产生前端事件。
       如果前端设备被关闭，当设备以读写方式重新打开时，此标志会自动关闭。)

Description(描述)
=================

Allow setting tuner mode flags to the frontend, between 0 (normal) or
``FE_TUNE_MODE_ONESHOT`` mode
(允许向前端设置调谐器模式标志，在 0 (正常) 与 ``FE_TUNE_MODE_ONESHOT`` 模式
之间选择)

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
