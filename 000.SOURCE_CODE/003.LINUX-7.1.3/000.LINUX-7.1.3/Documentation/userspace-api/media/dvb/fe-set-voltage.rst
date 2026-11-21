.. SPDX-License-Identifier: GFDL-1.1-no-invariants-or-later
.. c:namespace:: DTV.fe

.. _FE_SET_VOLTAGE:

******************************
ioctl FE_SET_VOLTAGE(设置电压)
******************************

Name(名称)
==========

FE_SET_VOLTAGE - Allow setting the DC level sent to the antenna subsystem.(FE_SET_VOLTAGE - 允许设置发送到天线子系统的直流电平。)

Synopsis(概要)
==============

.. c:macro:: FE_SET_VOLTAGE

``int ioctl(int fd, FE_SET_VOLTAGE, enum fe_sec_voltage voltage)``

Arguments(参数)
===============

``fd``
    File descriptor returned by :c:func:`open()`.
    (由 :c:func:`open()` 返回的文件描述符。)

``voltage``
    An integer enumerated value described at :c:type:`fe_sec_voltage`
    (在 :c:type:`fe_sec_voltage` 处描述的一个整型枚举值)

Description(描述)
=================

This ioctl allows to set the DC voltage level sent through the antenna
cable to 13V, 18V or off.
(此 ioctl 允许将通过天线电缆发送的直流电压电平设置为 13V、18V 或关闭。)

Usually, a satellite antenna subsystems require that the digital TV
device to send a DC voltage to feed power to the LNBf. Depending on the
LNBf type, the polarization or the intermediate frequency (IF) of the
LNBf can controlled by the voltage level. Other devices (for example,
the ones that implement DISEqC and multipoint LNBf's don't need to
control the voltage level, provided that either 13V or 18V is sent to
power up the LNBf.
(通常，卫星天线子系统要求数字电视设备发送直流电压为 LNBf 供电。根据
LNBf 的类型，LNBf 的极化方式或中频 (IF) 可以由电压电平控制。其他设备
(例如实现了 DISEqC 和多点 LNBf 的设备) 不需要控制电压电平，只要发送 13V
或 18V 为 LNBf 供电即可。)

.. attention:: if more than one device is connected to the same antenna,
   setting a voltage level may interfere on other devices, as they may lose
   the capability of setting polarization or IF. So, on those cases, setting
   the voltage to SEC_VOLTAGE_OFF while the device is not is used is
   recommended.
   (注意：如果多个设备连接到同一天线，设置电压电平可能会干扰其他设备，
   因为它们可能失去设置极化或中频的能力。因此，在这些情况下，建议在设备
   未使用时将电压设置为 SEC_VOLTAGE_OFF。)

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
