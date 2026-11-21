.. SPDX-License-Identifier: GFDL-1.1-no-invariants-or-later
.. c:namespace:: DTV.net

.. _NET_GET_IF:

**********************************
ioctl NET_GET_IF(读取网络接口配置)
**********************************

Name(名称)
==========

NET_GET_IF - Read the configuration data of an interface created via - :ref:`NET_ADD_IF <net>`.(NET_GET_IF - 读取通过 :ref:`NET_ADD_IF <net>` 创建的接口的配置数据。)

Synopsis(概要)
==============

.. c:macro:: NET_GET_IF

``int ioctl(int fd, NET_GET_IF, struct dvb_net_if *net_if)``

Arguments(参数)
===============

``fd``
    File descriptor returned by :c:func:`open()`.
    (由 :c:func:`open()` 返回的文件描述符。)

``net_if``
    pointer to struct :c:type:`dvb_net_if`
    (指向 struct :c:type:`dvb_net_if` 的指针)

Description(描述)
=================

The NET_GET_IF ioctl uses the interface number given by the struct
:c:type:`dvb_net_if`::ifnum field and fills the content of
struct :c:type:`dvb_net_if` with the packet ID and
encapsulation type used on such interface. If the interface was not
created yet with :ref:`NET_ADD_IF <net>`, it will return -1 and fill
the ``errno`` with ``EINVAL`` error code.
(NET_GET_IF ioctl 使用 struct :c:type:`dvb_net_if` 的 ::ifnum 字段给出的
接口号，并把该接口上使用的包 ID 与封装类型填充到 struct
:c:type:`dvb_net_if` 的内容中。如果该接口尚未用 :ref:`NET_ADD_IF <net>`
创建，它将返回 -1 并把 ``errno`` 填充为 ``EINVAL`` 错误码。)

Return Value(返回值)
====================

On success 0 is returned, and :c:type:`ca_slot_info` is filled.
(成功时返回 0，并填充 :c:type:`ca_slot_info` 。)

On error -1 is returned, and the ``errno`` variable is set
appropriately.
(出错时返回 -1，并相应设置 ``errno`` 变量。)

The generic error codes are described at the
:ref:`Generic Error Codes <gen-errors>` chapter.
(通用错误码在 :ref:`Generic Error Codes <gen-errors>` 一章中描述。)
