.. SPDX-License-Identifier: GFDL-1.1-no-invariants-or-later
.. c:namespace:: DTV.ca

.. _CA_GET_SLOT_INFO:

==================================
CA_GET_SLOT_INFO(查询 CA 插槽信息)
==================================

Name(名称)
----------

CA_GET_SLOT_INFO

Synopsis(概要)
--------------

.. c:macro:: CA_GET_SLOT_INFO

``int ioctl(fd, CA_GET_SLOT_INFO, struct ca_slot_info *info)``

Arguments(参数)
---------------

``fd``
  File descriptor returned by a previous call to :c:func:`open()`.
  (由之前调用 :c:func:`open()` 返回的文件描述符。)

``info``
  Pointer to struct :c:type:`ca_slot_info`.
  (指向 struct :c:type:`ca_slot_info` 的指针。)

Description(描述)
-----------------

Returns information about a CA slot identified by
:c:type:`ca_slot_info`.slot_num.
(返回由 :c:type:`ca_slot_info`.slot_num 标识的 CA 插槽的信息。)

Return Value(返回值)
--------------------

On success 0 is returned, and :c:type:`ca_slot_info` is filled.
(成功时返回 0，并填充 :c:type:`ca_slot_info` 。)

On error -1 is returned, and the ``errno`` variable is set
appropriately.
(出错时返回 -1，并相应设置 ``errno`` 变量。)

.. tabularcolumns:: |p{2.5cm}|p{15.0cm}|

.. flat-table::
    :header-rows:  0
    :stub-columns: 0
    :widths: 1 16

    -  -  ``ENODEV``
       -  the slot is not available.(该插槽不可用。)

The generic error codes are described at the
:ref:`Generic Error Codes <gen-errors>` chapter.
(通用错误码在 :ref:`Generic Error Codes <gen-errors>` 一章中描述。)
