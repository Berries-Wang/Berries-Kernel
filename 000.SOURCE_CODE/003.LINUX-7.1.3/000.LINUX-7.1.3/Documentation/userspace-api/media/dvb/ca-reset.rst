.. SPDX-License-Identifier: GFDL-1.1-no-invariants-or-later
.. c:namespace:: DTV.ca

.. _CA_RESET:

======================
CA_RESET(复位条件接收)
======================

Name(名称)
----------

CA_RESET

Synopsis(概要)
--------------

.. c:macro:: CA_RESET

``int ioctl(fd, CA_RESET)``

Arguments(参数)
---------------

``fd``
  File descriptor returned by a previous call to :c:func:`open()`.
  (由之前对 :c:func:`open()` 的调用返回的文件描述符。)

Description(描述)
-----------------

Puts the Conditional Access hardware on its initial state. It should
be called before start using the CA hardware.
(将条件接收硬件置于初始状态。在开始使用 CA 硬件之前应调用它。)

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
