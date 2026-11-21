.. SPDX-License-Identifier: GFDL-1.1-no-invariants-or-later
.. c:namespace:: DTV.fe

.. _FE_GET_FRONTEND:

*******************************
FE_GET_FRONTEND(获取前端参数)
*******************************

Name(名称)
==========

FE_GET_FRONTEND

.. attention:: This ioctl is deprecated.
    (注意：此 ioctl 已被弃用。)

Synopsis(概要)
==============

.. c:macro:: FE_GET_FRONTEND

``int ioctl(int fd, FE_GET_FRONTEND, struct dvb_frontend_parameters *p)``

Arguments(参数)
===============

``fd``
    File descriptor returned by :c:func:`open()`.
    (由 :c:func:`open()` 返回的文件描述符。)

``p``
    Points to parameters for tuning operation.
    (指向调谐操作所用参数的指针。)

Description(描述)
=================

This ioctl call queries the currently effective frontend parameters. For
this command, read-only access to the device is sufficient.
(此 ioctl 调用查询当前生效的前端参数。对于此命令，对设备的只读访问就足够
了。)

Return Value(返回值)
====================

On success 0 is returned.
(成功时返回 0。)

On error -1 is returned, and the ``errno`` variable is set
appropriately.
(出错时返回 -1，并相应设置 ``errno`` 变量。)

.. flat-table::
    :header-rows:  0
    :stub-columns: 0

    -  .. row 1

       -  ``EINVAL``

       -  Maximum supported symbol rate reached.(已达到支持的最大符号率。)

Generic error codes are described at the
:ref:`Generic Error Codes <gen-errors>` chapter.
(通用错误码在 :ref:`Generic Error Codes <gen-errors>` 一章中描述。)
