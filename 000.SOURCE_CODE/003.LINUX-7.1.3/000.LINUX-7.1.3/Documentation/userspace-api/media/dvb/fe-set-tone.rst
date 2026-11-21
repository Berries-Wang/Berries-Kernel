.. SPDX-License-Identifier: GFDL-1.1-no-invariants-or-later
.. c:namespace:: DTV.fe

.. _FE_SET_TONE:

**********************************
ioctl FE_SET_TONE(设置 22kHz 音调)
**********************************

Name(名称)
==========

FE_SET_TONE - Sets/resets the generation of the continuous 22kHz tone.(FE_SET_TONE - 设置/复位连续 22kHz 音调的产生。)

Synopsis(概要)
==============

.. c:macro:: FE_SET_TONE

``int ioctl(int fd, FE_SET_TONE, enum fe_sec_tone_mode tone)``

Arguments(参数)
===============

``fd``
    File descriptor returned by :c:func:`open()`.
    (由 :c:func:`open()` 返回的文件描述符。)

``tone``
    An integer enumerated value described at :c:type:`fe_sec_tone_mode`
    (在 :c:type:`fe_sec_tone_mode` 处描述的一个整型枚举值)

Description(描述)
=================

This ioctl is used to set the generation of the continuous 22kHz tone.
This call requires read/write permissions.
(此 ioctl 用于设置连续 22kHz 音调的产生。此调用需要读/写权限。)

Usually, satellite antenna subsystems require that the digital TV device
to send a 22kHz tone in order to select between high/low band on some
dual-band LNBf. It is also used to send signals to DiSEqC equipment, but
this is done using the DiSEqC ioctls.
(通常，卫星天线子系统要求数字电视设备发送 22kHz 音调，以便在某些双频段
LNBf 上选择高/低频段。它也用于向 DiSEqC 设备发送信号，但这是通过 DiSEqC
ioctl 完成的。)

.. attention:: If more than one device is connected to the same antenna,
   setting a tone may interfere on other devices, as they may lose the
   capability of selecting the band. So, it is recommended that applications
   would change to SEC_TONE_OFF when the device is not used.
   (注意：如果多个设备连接到同一天线，设置音调可能会干扰其他设备，因为
   它们可能失去选择频段的能力。因此，建议应用在设备未使用时切换到
   SEC_TONE_OFF。)

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
