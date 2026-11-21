.. SPDX-License-Identifier: GFDL-1.1-no-invariants-or-later

.. _query-dvb-frontend-info:

*******************************************
Querying frontend information(查询前端信息)
*******************************************

Usually, the first thing to do when the frontend is opened is to check
the frontend capabilities. This is done using
:ref:`FE_GET_INFO`. This ioctl will enumerate the
Digital TV API version and other characteristics about the frontend, and can
be opened either in read only or read/write mode.
(通常，打开前端后首先要做的是检查前端能力。这通过 :ref:`FE_GET_INFO` 完成。该 ioctl 会枚举数字电视 API 版本以及前端的其他特性，并且可以以只读或读写模式打开。)
