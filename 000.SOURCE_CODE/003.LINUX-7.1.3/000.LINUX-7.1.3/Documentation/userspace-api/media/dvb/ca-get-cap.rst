.. SPDX-License-Identifier: GFDL-1.1-no-invariants-or-later
.. c:namespace:: DTV.ca

.. _CA_GET_CAP:

============================
CA_GET_CAP(查询条件接收能力)
============================

Name(名称)
----------

CA_GET_CAP

Synopsis(概要)
--------------

.. c:macro:: CA_GET_CAP

``int ioctl(fd, CA_GET_CAP, struct ca_caps *caps)``

Arguments(参数)
---------------

``fd``
  File descriptor returned by a previous call to :c:func:`open()`.
  (由之前对 :c:func:`open()` 的调用返回的文件描述符。)

``caps``
  Pointer to struct :c:type:`ca_caps`.
  (指向 struct :c:type:`ca_caps` 的指针。)

Description(描述)
-----------------

Queries the Kernel for information about the available CA and descrambler
slots, and their types.
(向内核查询可用的 CA 与解扰器槽位及其类型的信息。)

Return Value(返回值)
--------------------

On success 0 is returned and :c:type:`ca_caps` is filled.
(成功时返回 0 并填充 :c:type:`ca_caps`。)

On error, -1 is returned and the ``errno`` variable is set
appropriately.
(出错时返回 -1，并相应设置 ``errno`` 变量。)

The generic error codes are described at the
:ref:`Generic Error Codes <gen-errors>` chapter.
(通用错误码在 :ref:`Generic Error Codes <gen-errors>` 一章中描述。)
