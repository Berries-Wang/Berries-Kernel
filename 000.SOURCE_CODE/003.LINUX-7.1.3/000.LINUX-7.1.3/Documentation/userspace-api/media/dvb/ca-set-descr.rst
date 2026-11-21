.. SPDX-License-Identifier: GFDL-1.1-no-invariants-or-later
.. c:namespace:: DTV.ca

.. _CA_SET_DESCR:

============================
CA_SET_DESCR(设置解扰描述符)
============================

Name(名称)
----------

CA_SET_DESCR

Synopsis(概要)
--------------

.. c:macro:: CA_SET_DESCR

``int ioctl(fd, CA_SET_DESCR, struct ca_descr *desc)``

Arguments(参数)
---------------

``fd``
  File descriptor returned by a previous call to :c:func:`open()`.
  (由之前对 :c:func:`open()` 的调用返回的文件描述符。)

``msg``
  Pointer to struct :c:type:`ca_descr`.
  (指向 struct :c:type:`ca_descr` 的指针。)

Description(描述)
-----------------

CA_SET_DESCR is used for feeding descrambler CA slots with descrambling
keys (referred as control words).
(CA_SET_DESCR 用于向解扰器 CA 槽位馈送解扰密钥（即控制字）。)

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
