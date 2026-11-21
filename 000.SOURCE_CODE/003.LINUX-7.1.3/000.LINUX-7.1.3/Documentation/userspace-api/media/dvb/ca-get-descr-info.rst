.. SPDX-License-Identifier: GFDL-1.1-no-invariants-or-later
.. c:namespace:: DTV.ca

.. _CA_GET_DESCR_INFO:

=================================
CA_GET_DESCR_INFO(查询解扰器信息)
=================================

Name(名称)
----------

CA_GET_DESCR_INFO

Synopsis(概要)
--------------

.. c:macro:: CA_GET_DESCR_INFO

``int ioctl(fd, CA_GET_DESCR_INFO, struct ca_descr_info *desc)``

Arguments(参数)
---------------

``fd``
  File descriptor returned by a previous call to :c:func:`open()`.
  (由之前对 :c:func:`open()` 的调用返回的文件描述符。)

``desc``
  Pointer to struct :c:type:`ca_descr_info`.
  (指向 struct :c:type:`ca_descr_info` 的指针。)

Description(描述)
-----------------

Returns information about all descrambler slots.
(返回所有解扰器槽位的信息。)

Return Value(返回值)
--------------------

On success 0 is returned, and :c:type:`ca_descr_info` is filled.
(成功时返回 0，并填充 :c:type:`ca_descr_info`。)

On error -1 is returned, and the ``errno`` variable is set
appropriately. The generic error codes are described at the
:ref:`Generic Error Codes <gen-errors>` chapter.
(出错时返回 -1，并相应设置 ``errno`` 变量。通用错误码在
:ref:`Generic Error Codes <gen-errors>` 一章中描述。)
