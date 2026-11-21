.. SPDX-License-Identifier: GFDL-1.1-no-invariants-or-later
.. c:namespace:: DTV.fe

.. _FE_SET_FRONTEND:

***************
FE_SET_FRONTEND
***************

.. attention:: This ioctl is deprecated.
               (注意：此 ioctl 已被弃用。)

Name(名称)
==========

FE_SET_FRONTEND
(FE_SET_FRONTEND 调用。)

Synopsis(概要)
==============

.. c:macro:: FE_SET_FRONTEND

``int ioctl(int fd, FE_SET_FRONTEND, struct dvb_frontend_parameters *p)``

Arguments(参数)
===============

``fd``
    File descriptor returned by :c:func:`open()`.
    (由 :c:func:`open()` 返回的文件描述符。)

``p``
    Points to parameters for tuning operation.
    (指向调谐操作的参数。)

Description(描述)
=================

This ioctl call starts a tuning operation using specified parameters.
The result of this call will be successful if the parameters were valid
and the tuning could be initiated. The result of the tuning operation in
itself, however, will arrive asynchronously as an event (see
documentation for :ref:`FE_GET_EVENT` and
FrontendEvent.) If a new :ref:`FE_SET_FRONTEND`
operation is initiated before the previous one was completed, the
previous operation will be aborted in favor of the new one. This command
requires read/write access to the device.
(此 ioctl 调用使用指定的参数启动一次调谐操作。如果参数有效且调谐能
够被发起，此调用的结果将是成功的。然而，调谐操作本身的结果将作为事
件异步到达 (参见 :ref:`FE_GET_EVENT` 与 FrontendEvent 的文档)。如果在
上一次 :ref:`FE_SET_FRONTEND` 操作完成之前发起新的
:ref:`FE_SET_FRONTEND` 操作，则前一个操作会被中止，让位于新的操作。
此命令需要对设备的读写访问权限。)

Return Value(返回值)
====================

On success 0 is returned.
(成功时返回 0。)

On error -1 is returned, and the ``errno`` variable is set
appropriately.
(出错时返回 -1，并适当设置 ``errno`` 变量。)

.. tabularcolumns:: |p{2.5cm}|p{15.0cm}|

.. flat-table::
    :header-rows:  0
    :stub-columns: 0
    :widths: 1 16

    -  .. row 1

       -  ``EINVAL``

       -  Maximum supported symbol rate reached.
          (已达到支持的最大符号率。)

Generic error codes are described at the
:ref:`Generic Error Codes <gen-errors>` chapter.
(通用错误码在 :ref:`Generic Error Codes <gen-errors>` 一章中描述。)
