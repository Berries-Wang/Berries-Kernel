.. SPDX-License-Identifier: GFDL-1.1-no-invariants-or-later
.. c:namespace:: DTV.fe

.. _FE_DISEQC_SEND_BURST:

*****************************************************
ioctl FE_DISEQC_SEND_BURST(发送 mini DiSEqC 突发音调)
*****************************************************

Name(名称)
==========

FE_DISEQC_SEND_BURST - Sends a 22KHz tone burst for 2x1 mini DiSEqC satellite selection.(FE_DISEQC_SEND_BURST - 为 2x1 mini DiSEqC 卫星选择发送 22kHz 突发音调。)

Synopsis(概要)
==============

.. c:macro:: FE_DISEQC_SEND_BURST

``int ioctl(int fd, FE_DISEQC_SEND_BURST, enum fe_sec_mini_cmd tone)``

Arguments(参数)
===============

``fd``
    File descriptor returned by :c:func:`open()`.
    (由 :c:func:`open()` 返回的文件描述符。)

``tone``
    An integer enumerated value described at :c:type:`fe_sec_mini_cmd`.
    (在 :c:type:`fe_sec_mini_cmd` 处描述的一个整型枚举值。)

Description(描述)
=================

This ioctl is used to set the generation of a 22kHz tone burst for mini
DiSEqC satellite selection for 2x1 switches. This call requires
read/write permissions.
(此 ioctl 用于为 2x1 开关的 mini DiSEqC 卫星选择设置 22kHz 突发音调的
产生。此调用需要读/写权限。)

It provides support for what's specified at
`Digital Satellite Equipment Control (DiSEqC) - Simple "ToneBurst" Detection Circuit specification. <http://www.eutelsat.com/files/contributed/satellites/pdf/Diseqc/associated%20docs/simple_tone_burst_detec.pdf>`__
(它支持 `Digital Satellite Equipment Control (DiSEqC) - Simple "ToneBurst" Detection Circuit specification. <http://www.eutelsat.com/files/contributed/satellites/pdf/Diseqc/associated%20docs/simple_tone_burst_detec.pdf>`__
中规定的内容。)

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
