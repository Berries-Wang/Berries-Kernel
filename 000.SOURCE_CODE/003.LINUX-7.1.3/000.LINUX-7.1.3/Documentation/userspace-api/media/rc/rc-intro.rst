.. SPDX-License-Identifier: GPL-2.0 OR GFDL-1.1-no-invariants-or-later

.. _Remote_controllers_Intro:

******************
Introduction(引言)
******************

Currently, most analog and digital devices have a Infrared input for
remote controllers. Each manufacturer has their own type of control. It
is not rare for the same manufacturer to ship different types of
controls, depending on the device.
(目前，大多数模拟和数字设备都有用于遥控器的红外输入。每个制造商都有自己的遥控器类型。同一制造商根据设备附带不同类型的遥控器的情况并不罕见。)

A Remote Controller interface is mapped as a normal evdev/input
interface, just like a keyboard or a mouse. So, it uses all ioctls
already defined for any other input devices.
(遥控器接口被映射为一个普通的 evdev/input 接口，就像键盘或鼠标一样。因此，它使用已经为任何其他输入设备定义的所有 ioctl。)

However, remove controllers are more flexible than a normal input
device, as the IR receiver (and/or transmitter) can be used in
conjunction with a wide variety of different IR remotes.
(不过，遥控器比普通输入设备更加灵活，因为红外接收器 (和/或发射器) 可以与各种各样的不同红外遥控器配合使用。)

In order to allow flexibility, the Remote Controller subsystem allows
controlling the RC-specific attributes via
:ref:`the sysfs class nodes <remote_controllers_sysfs_nodes>`.
(为了支持这种灵活性，遥控器子系统允许通过 :ref:`the sysfs class nodes <remote_controllers_sysfs_nodes>` 控制 RC 特有的属性。)
