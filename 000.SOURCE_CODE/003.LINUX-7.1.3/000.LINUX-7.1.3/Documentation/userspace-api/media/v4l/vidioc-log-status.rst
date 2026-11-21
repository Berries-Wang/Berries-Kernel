.. SPDX-License-Identifier: GFDL-1.1-no-invariants-or-later
.. c:namespace:: V4L

.. _VIDIOC_LOG_STATUS:

*****************************************
ioctl VIDIOC_LOG_STATUS(记录驱动状态信息)
*****************************************

Name(名称)
==========

VIDIOC_LOG_STATUS - Log driver status information(VIDIOC_LOG_STATUS - 记录驱动状态信息)

Synopsis(概要)
==============

.. c:macro:: VIDIOC_LOG_STATUS

``int ioctl(int fd, VIDIOC_LOG_STATUS)``

Arguments(参数)
===============

``fd``
    File descriptor returned by :c:func:`open()`.
    (由 :c:func:`open()` 返回的文件描述符。)

Description(描述)
=================

As the video/audio devices become more complicated it becomes harder to
debug problems. When this ioctl is called the driver will output the
current device status to the kernel log. This is particular useful when
dealing with problems like no sound, no video and incorrectly tuned
channels. Also many modern devices autodetect video and audio standards
and this ioctl will report what the device thinks what the standard is.
Mismatches may give an indication where the problem is.
(随着视频/音频设备变得越来越复杂，调试问题也变得越来越困难。调用此 ioctl
时，驱动会把当前设备状态输出到内核日志。这在处理无声、无视频以及频道调谐
不正确等问题时特别有用。此外，许多现代设备会自动检测视频和音频制式，此
ioctl 会报告设备认为当前的制式是什么。不匹配之处可能提示问题所在。)

This ioctl is optional and not all drivers support it. It was introduced
in Linux 2.6.15.
(此 ioctl 是可选的，并非所有驱动都支持。它引入于 Linux 2.6.15。)

Return Value(返回值)
====================

On success 0 is returned, on error -1 and the ``errno`` variable is set
appropriately. The generic error codes are described at the
:ref:`Generic Error Codes <gen-errors>` chapter.
(成功时返回 0；出错时返回 -1，并相应设置 ``errno`` 变量。通用错误码在
:ref:`Generic Error Codes <gen-errors>` 一章中描述。)
