.. SPDX-License-Identifier: GFDL-1.1-no-invariants-or-later
.. c:namespace:: DTV.net

.. _NET_ADD_IF:

********************************
ioctl NET_ADD_IF(创建网络接口)
********************************

Name(名称)
==========

NET_ADD_IF - Creates a new network interface for a given Packet ID.(NET_ADD_IF - 为给定的包 ID 创建一个新的网络接口。)

Synopsis(概要)
==============

.. c:macro:: NET_ADD_IF

``int ioctl(int fd, NET_ADD_IF, struct dvb_net_if *net_if)``

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

The NET_ADD_IF ioctl system call selects the Packet ID (PID) that
contains a TCP/IP traffic, the type of encapsulation to be used (MPE or
ULE) and the interface number for the new interface to be created. When
the system call successfully returns, a new virtual network interface is
created.
(NET_ADD_IF ioctl 系统调用选择包含 TCP/IP 流量的包 ID (PID)、要使用的封装
类型 (MPE 或 ULE)，以及要创建的新接口的接口号。当系统调用成功返回时，一个
新的虚拟网络接口即被创建。)

The struct :c:type:`dvb_net_if`::ifnum field will be
filled with the number of the created interface.
(struct :c:type:`dvb_net_if` 的 ::ifnum 字段将被填充为所创建接口的
编号。)

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
