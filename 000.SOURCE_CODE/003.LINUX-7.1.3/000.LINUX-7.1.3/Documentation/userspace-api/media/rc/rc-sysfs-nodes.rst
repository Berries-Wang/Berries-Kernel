.. SPDX-License-Identifier: GPL-2.0 OR GFDL-1.1-no-invariants-or-later

.. _remote_controllers_sysfs_nodes:

****************************************************
Remote Controller's sysfs nodes(遥控器的 sysfs 节点)
****************************************************

As defined at Documentation/ABI/testing/sysfs-class-rc, those are
the sysfs nodes that control the Remote Controllers:
(如 Documentation/ABI/testing/sysfs-class-rc 中所定义，以下是控制遥控器
的 sysfs 节点：)


.. _sys_class_rc:

/sys/class/rc/(原名)
====================

The ``/sys/class/rc/`` class sub-directory belongs to the Remote
Controller core and provides a sysfs interface for configuring infrared
remote controller receivers.
(``/sys/class/rc/`` 类子目录属于遥控器核心，提供用于配置红外遥控接收器
的 sysfs 接口。)


.. _sys_class_rc_rcN:

/sys/class/rc/rcN/(原名)
========================

A ``/sys/class/rc/rcN`` directory is created for each remote control
receiver device where N is the number of the receiver.
(会为每个遥控接收器设备创建一个 ``/sys/class/rc/rcN`` 目录，其中 N 是
接收器的编号。)


.. _sys_class_rc_rcN_protocols:

/sys/class/rc/rcN/protocols(协议列表)
=====================================

Reading this file returns a list of available protocols, something like::

	rc5 [rc6] nec jvc [sony]

(读取此文件返回可用协议的列表，类似于：)

Enabled protocols are shown in [] brackets.
(启用的协议显示在 [] 括号中。)

Writing "+proto" will add a protocol to the list of enabled protocols.
(写入 "+proto" 会把一个协议添加到已启用协议列表中。)

Writing "-proto" will remove a protocol from the list of enabled
protocols.
(写入 "-proto" 会从已启用协议列表中移除一个协议。)

Writing "proto" will enable only "proto".
(写入 "proto" 将只启用 "proto"。)

Writing "none" will disable all protocols.
(写入 "none" 将禁用所有协议。)

Write fails with ``EINVAL`` if an invalid protocol combination or unknown
protocol name is used.
(如果使用了无效的协议组合或未知的协议名称，写入将失败并返回
``EINVAL``。)


.. _sys_class_rc_rcN_filter:

/sys/class/rc/rcN/filter(扫描码过滤值)
======================================

Sets the scancode filter expected value.
(设置扫描码过滤器的期望值。)

Use in combination with ``/sys/class/rc/rcN/filter_mask`` to set the
expected value of the bits set in the filter mask. If the hardware
supports it then scancodes which do not match the filter will be
ignored. Otherwise the write will fail with an error.
(与 ``/sys/class/rc/rcN/filter_mask`` 结合使用，以设置过滤掩码中所置位
那些位的期望值。如果硬件支持，不匹配过滤器的扫描码将被忽略。否则写入
将失败并报错。)

This value may be reset to 0 if the current protocol is altered.
(如果当前协议被更改，此值可能会被重置为 0。)


.. _sys_class_rc_rcN_filter_mask:

/sys/class/rc/rcN/filter_mask(过滤掩码)
=======================================

Sets the scancode filter mask of bits to compare. Use in combination
with ``/sys/class/rc/rcN/filter`` to set the bits of the scancode which
should be compared against the expected value. A value of 0 disables the
filter to allow all valid scancodes to be processed.
(设置要比较的位的扫描码过滤器掩码。与 ``/sys/class/rc/rcN/filter`` 结合
使用，以设置扫描码中应与期望值比较的位。值为 0 时禁用过滤器，允许处理
所有有效的扫描码。)

If the hardware supports it then scancodes which do not match the filter
will be ignored. Otherwise the write will fail with an error.
(如果硬件支持，不匹配过滤器的扫描码将被忽略。否则写入将失败并报错。)

This value may be reset to 0 if the current protocol is altered.
(如果当前协议被更改，此值可能会被重置为 0。)


.. _sys_class_rc_rcN_wakeup_protocols:

/sys/class/rc/rcN/wakeup_protocols(唤醒协议列表)
================================================

Reading this file returns a list of available protocols to use for the
wakeup filter, something like::

	rc-5 nec nec-x rc-6-0 rc-6-6a-24 [rc-6-6a-32] rc-6-mce

(读取此文件返回可用于唤醒过滤器的协议列表，类似于：)

Note that protocol variants are listed, so ``nec``, ``sony``, ``rc-5``, ``rc-6``
have their different bit length encodings listed if available.
(注意，列出的是协议的变体，因此 ``nec``、``sony``、``rc-5``、``rc-6``
 的不同位长编码（如果有）都会被列出。)

Note that all protocol variants are listed.
(注意，会列出所有协议变体。)

The enabled wakeup protocol is shown in [] brackets.
(启用的唤醒协议显示在 [] 括号中。)

Only one protocol can be selected at a time.
(一次只能选择一个协议。)

Writing "proto" will use "proto" for wakeup events.
(写入 "proto" 将使用 "proto" 处理唤醒事件。)

Writing "none" will disable wakeup.
(写入 "none" 将禁用唤醒。)

Write fails with ``EINVAL`` if an invalid protocol combination or unknown
protocol name is used, or if wakeup is not supported by the hardware.
(如果使用了无效的协议组合或未知的协议名称，或者硬件不支持唤醒，写入将
失败并返回 ``EINVAL``。)


.. _sys_class_rc_rcN_wakeup_filter:

/sys/class/rc/rcN/wakeup_filter(唤醒过滤值)
===========================================

Sets the scancode wakeup filter expected value. Use in combination with
``/sys/class/rc/rcN/wakeup_filter_mask`` to set the expected value of
the bits set in the wakeup filter mask to trigger a system wake event.
(设置扫描码唤醒过滤器的期望值。与 ``/sys/class/rc/rcN/wakeup_filter_mask``
结合使用，以设置唤醒过滤器掩码中所置位那些位的期望值，从而触发系统唤醒
事件。)

If the hardware supports it and wakeup_filter_mask is not 0 then
scancodes which match the filter will wake the system from e.g. suspend
to RAM or power off. Otherwise the write will fail with an error.
(如果硬件支持且 wakeup_filter_mask 不为 0，匹配过滤器的扫描码将把系统从
例如挂起到内存 (suspend to RAM) 或关机状态唤醒。否则写入将失败并报错。)

This value may be reset to 0 if the wakeup protocol is altered.
(如果唤醒协议被更改，此值可能会被重置为 0。)


.. _sys_class_rc_rcN_wakeup_filter_mask:

/sys/class/rc/rcN/wakeup_filter_mask(唤醒过滤掩码)
==================================================

Sets the scancode wakeup filter mask of bits to compare. Use in
combination with ``/sys/class/rc/rcN/wakeup_filter`` to set the bits of
the scancode which should be compared against the expected value to
trigger a system wake event.
(设置要比较的位的扫描码唤醒过滤器掩码。与 ``/sys/class/rc/rcN/wakeup_filter``
结合使用，以设置扫描码中应与期望值比较的位，从而触发系统唤醒事件。)

If the hardware supports it and wakeup_filter_mask is not 0 then
scancodes which match the filter will wake the system from e.g. suspend
to RAM or power off. Otherwise the write will fail with an error.
(如果硬件支持且 wakeup_filter_mask 不为 0，匹配过滤器的扫描码将把系统从
例如挂起到内存或关机状态唤醒。否则写入将失败并报错。)

This value may be reset to 0 if the wakeup protocol is altered.
(如果唤醒协议被更改，此值可能会被重置为 0。)
