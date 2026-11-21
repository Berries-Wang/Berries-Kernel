.. SPDX-License-Identifier: GFDL-1.1-no-invariants-or-later
.. c:namespace:: DTV.dmx

.. _DMX_GET_PES_PIDS:

==============================
DMX_GET_PES_PIDS(获取 PES PID)
==============================

Name(名称)
----------

DMX_GET_PES_PIDS

Synopsis(概要)
--------------

.. c:macro:: DMX_GET_PES_PIDS

``int ioctl(fd, DMX_GET_PES_PIDS, __u16 pids[5])``

Arguments(参数)
---------------

``fd``
    File descriptor returned by :c:func:`open()`.
    (由 :c:func:`open()` 返回的文件描述符。)

``pids``
    Array used to store 5 Program IDs.
    (用于存储 5 个节目 ID (Program ID) 的数组。)

Description(描述)
-----------------

This ioctl allows to query a DVB device to return the first PID used
by audio, video, textext, subtitle and PCR programs on a given service.
They're stored as:
(此 ioctl 允许查询 DVB 设备，以返回给定业务上音频、视频、图文电视
(teletext)、字幕和 PCR 节目所使用的第一个 PID。它们按如下方式存储：)

=======================	========	=======================================
PID  element		position	content(内容)
=======================	========	=======================================
pids[DMX_PES_AUDIO]	0		first audio PID(第一个音频 PID)
pids[DMX_PES_VIDEO]	1		first video PID(第一个视频 PID)
pids[DMX_PES_TELETEXT]	2		first teletext PID(第一个图文电视 PID)
pids[DMX_PES_SUBTITLE]	3		first subtitle PID(第一个字幕 PID)
pids[DMX_PES_PCR]	4		first Program Clock Reference PID(第一个节目时钟参考 PID)
=======================	========	=======================================

.. note::

	A value equal to 0xffff means that the PID was not filled by the
	Kernel.
	(值为 0xffff 表示内核未填充该 PID。)

Return Value(返回值)
--------------------

On success 0 is returned.
(成功时返回 0。)

On error -1 is returned, and the ``errno`` variable is set
appropriately.
(出错时返回 -1，并相应设置 ``errno`` 变量。)

The generic error codes are described at the
:ref:`Generic Error Codes <gen-errors>` chapter.
(通用错误码在 :ref:`Generic Error Codes <gen-errors>` 一章中描述。)
