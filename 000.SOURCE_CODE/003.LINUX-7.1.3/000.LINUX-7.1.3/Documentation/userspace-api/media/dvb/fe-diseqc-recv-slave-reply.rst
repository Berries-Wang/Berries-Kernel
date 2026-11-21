.. SPDX-License-Identifier: GFDL-1.1-no-invariants-or-later
.. c:namespace:: DTV.fe

.. _FE_DISEQC_RECV_SLAVE_REPLY:

**************************************************
ioctl FE_DISEQC_RECV_SLAVE_REPLY(接收 DiSEqC 应答)
**************************************************

Name(名称)
==========

FE_DISEQC_RECV_SLAVE_REPLY - Receives reply from a DiSEqC 2.0 command(FE_DISEQC_RECV_SLAVE_REPLY - 接收对 DiSEqC 2.0 命令的应答)

Synopsis(概要)
==============

.. c:macro:: FE_DISEQC_RECV_SLAVE_REPLY

``int ioctl(int fd, FE_DISEQC_RECV_SLAVE_REPLY, struct dvb_diseqc_slave_reply *argp)``

Arguments(参数)
===============

``fd``
    File descriptor returned by :c:func:`open()`.
    (由 :c:func:`open()` 返回的文件描述符。)

``argp``
    pointer to struct :c:type:`dvb_diseqc_slave_reply`.
    (指向 struct :c:type:`dvb_diseqc_slave_reply` 的指针。)

Description(描述)
=================

Receives reply from a DiSEqC 2.0 command.
(接收对 DiSEqC 2.0 命令的应答。)

The received message is stored at the buffer pointed by ``argp``.
(接收到的消息存储在 ``argp`` 所指向的缓冲区中。)

Return Value(返回值)
====================

On success 0 is returned.
(成功时返回 0。)

On error -1 is returned, and the ``errno`` variable is set
appropriately.
(出错时返回 -1，并相应设置 ``errno`` 变量。)

Generic error codes are described at the
:ref:`Generic Error Codes <gen-errors>` chapter.
(通用错误码在 :ref:`Generic Error Codes <gen-errors>` 一章中描述。)
