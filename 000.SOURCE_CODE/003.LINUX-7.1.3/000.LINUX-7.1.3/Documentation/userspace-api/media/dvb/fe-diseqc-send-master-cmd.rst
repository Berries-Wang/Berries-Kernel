.. SPDX-License-Identifier: GFDL-1.1-no-invariants-or-later
.. c:namespace:: DTV.fe

.. _FE_DISEQC_SEND_MASTER_CMD:

*************************************************
ioctl FE_DISEQC_SEND_MASTER_CMD(发送 DiSEqC 命令)
*************************************************

Name(名称)
==========

FE_DISEQC_SEND_MASTER_CMD - Sends a DiSEqC command(FE_DISEQC_SEND_MASTER_CMD - 发送一条 DiSEqC 命令)

Synopsis(概要)
==============

.. c:macro:: FE_DISEQC_SEND_MASTER_CMD

``int ioctl(int fd, FE_DISEQC_SEND_MASTER_CMD, struct dvb_diseqc_master_cmd *argp)``

Arguments(参数)
===============

``fd``
    File descriptor returned by :c:func:`open()`.
    (由 :c:func:`open()` 返回的文件描述符。)

``argp``
    pointer to struct
    :c:type:`dvb_diseqc_master_cmd`
    (指向 struct :c:type:`dvb_diseqc_master_cmd` 的指针)

Description(描述)
=================

Sends the DiSEqC command pointed by :c:type:`dvb_diseqc_master_cmd`
to the antenna subsystem.
(把 :c:type:`dvb_diseqc_master_cmd` 所指向的 DiSEqC 命令发送到天线子系统。)

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

