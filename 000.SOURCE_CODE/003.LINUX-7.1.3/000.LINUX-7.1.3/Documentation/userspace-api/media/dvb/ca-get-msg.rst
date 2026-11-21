.. SPDX-License-Identifier: GFDL-1.1-no-invariants-or-later
.. c:namespace:: DTV.ca

.. _CA_GET_MSG:

==============================
CA_GET_MSG(从 CA 模块接收消息)
==============================

Name(名称)
----------

CA_GET_MSG

Synopsis(概要)
--------------

.. c:macro:: CA_GET_MSG

``int ioctl(fd, CA_GET_MSG, struct ca_msg *msg)``

Arguments(参数)
---------------

``fd``
  File descriptor returned by a previous call to :c:func:`open()`.
  (由之前调用 :c:func:`open()` 返回的文件描述符。)

``msg``
  Pointer to struct :c:type:`ca_msg`.
  (指向 struct :c:type:`ca_msg` 的指针。)

Description(描述)
-----------------

Receives a message via a CI CA module.
(通过 CI CA 模块接收一条消息。)

.. note::

   Please notice that, on most drivers, this is done by reading from
   the /dev/adapter?/ca? device node.
   (请注意，在大多数驱动上，这是通过从 /dev/adapter?/ca? 设备节点读取来
   完成的。)

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
