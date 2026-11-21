.. SPDX-License-Identifier: GFDL-1.1-no-invariants-or-later
.. c:namespace:: DTV.fe

.. _FE_READ_STATUS:

**********************************
ioctl FE_READ_STATUS(读取前端状态)
**********************************

Name(名称)
==========

FE_READ_STATUS - Returns status information about the front-end. This call only requires - read-only access to the device(FE_READ_STATUS - 返回前端的状态信息。此调用只需要对设备的只读访问)

Synopsis(概要)
==============

.. c:macro:: FE_READ_STATUS

``int ioctl(int fd, FE_READ_STATUS, unsigned int *status)``

Arguments(参数)
===============

``fd``
    File descriptor returned by :c:func:`open()`.
    (由 :c:func:`open()` 返回的文件描述符。)

``status``
    pointer to a bitmask integer filled with the values defined by enum
    :c:type:`fe_status`.
    (指向一个位掩码整数的指针，将由 enum :c:type:`fe_status` 定义的值
    填充。)

Description(描述)
=================

All Digital TV frontend devices support the ``FE_READ_STATUS`` ioctl. It is
used to check about the locking status of the frontend after being
tuned. The ioctl takes a pointer to an integer where the status will be
written.
(所有数字电视前端设备都支持 ``FE_READ_STATUS`` ioctl。它用于在调谐之后
检查前端的锁定状态。该 ioctl 接受一个指针，指向将要写入状态的整数。)

.. note::

   The size of status is actually sizeof(enum fe_status), with
   varies according with the architecture. This needs to be fixed in the
   future.
   (status 的大小实际上是 sizeof(enum fe_status)，随体系结构而变化。这个
   问题将来需要修正。)

int fe_status(fe_status 类型)
=============================

The fe_status parameter is used to indicate the current state and/or
state changes of the frontend hardware. It is produced using the enum
:c:type:`fe_status` values on a bitmask
(fe_status 参数用于指示前端硬件的当前状态和/或状态变化。它使用 enum
:c:type:`fe_status` 的值在一个位掩码上生成)

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
