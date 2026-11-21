.. SPDX-License-Identifier: GFDL-1.1-no-invariants-or-later
.. c:namespace:: DTV.net

.. _NET_REMOVE_IF:

*********************************
ioctl NET_REMOVE_IF(移除网络接口)
*********************************

Name(名称)
==========

NET_REMOVE_IF - Removes a network interface.
(NET_REMOVE_IF - 移除一个网络接口。)

Synopsis(概要)
==============

.. c:macro:: NET_REMOVE_IF

``int ioctl(int fd, NET_REMOVE_IF, int ifnum)``

Arguments(参数)
===============

``fd``
    File descriptor returned by :c:func:`open()`.
    (由 :c:func:`open()` 返回的文件描述符。)

``net_if``
    number of the interface to be removed
    (要移除的接口编号)

Description(描述)
=================

The NET_REMOVE_IF ioctl deletes an interface previously created via
:ref:`NET_ADD_IF <net>`.
(NET_REMOVE_IF ioctl 删除之前通过 :ref:`NET_ADD_IF <net>` 创建的接口。)

Return Value(返回值)
====================

On success 0 is returned, and :c:type:`ca_slot_info` is filled.
(成功时返回 0，并填充 :c:type:`ca_slot_info`。)

On error -1 is returned, and the ``errno`` variable is set
appropriately.
(出错时返回 -1，并相应设置 ``errno`` 变量。)

The generic error codes are described at the
:ref:`Generic Error Codes <gen-errors>` chapter.
(通用错误码在 :ref:`Generic Error Codes <gen-errors>` 一章中描述。)
