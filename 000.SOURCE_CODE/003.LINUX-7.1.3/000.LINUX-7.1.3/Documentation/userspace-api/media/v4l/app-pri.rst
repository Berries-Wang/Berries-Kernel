.. SPDX-License-Identifier: GFDL-1.1-no-invariants-or-later
.. c:namespace:: V4L

.. _app-pri:

************************************
Application Priority(应用程序优先级)
************************************

When multiple applications share a device it may be desirable to assign
them different priorities. Contrary to the traditional "rm -rf /" school
of thought, a video recording application could for example block other
applications from changing video controls or switching the current TV
channel. Another objective is to permit low priority applications
working in background, which can be preempted by user controlled
applications and automatically regain control of the device at a later
time.
(当多个应用程序共享一个设备时，为它们分配不同的优先级可能是可取的。与传统
"rm -rf /" 学派的做法相反，例如一个视频录制应用可以阻止其他应用程序更改视
频控件或切换当前电视频道。另一个目标是允许低优先级应用在后台工作：它们会
被用户控制的应用抢占，并在稍后自动重新获得对设备的控制权。)

Since these features cannot be implemented entirely in user space V4L2
defines the :ref:`VIDIOC_G_PRIORITY <VIDIOC_G_PRIORITY>` and
:ref:`VIDIOC_S_PRIORITY <VIDIOC_G_PRIORITY>` ioctls to request and
query the access priority associate with a file descriptor. Opening a
device assigns a medium priority, compatible with earlier versions of
V4L2 and drivers not supporting these ioctls. Applications requiring a
different priority will usually call :ref:`VIDIOC_S_PRIORITY
<VIDIOC_G_PRIORITY>` after verifying the device with the
:ref:`VIDIOC_QUERYCAP` ioctl.
(由于这些特性无法完全在用户空间实现，V4L2 定义了 :ref:`VIDIOC_G_PRIORITY
<VIDIOC_G_PRIORITY>` 与 :ref:`VIDIOC_S_PRIORITY <VIDIOC_G_PRIORITY>` ioctl，
用于请求和查询与文件描述符关联的访问优先级。打开设备会赋予中优先级——这与
较早版本的 V4L2 以及不支持这些 ioctl 的驱动兼容。需要不同优先级的应用程序
通常会先用 :ref:`VIDIOC_QUERYCAP` ioctl 验证设备，再调用 :ref:`VIDIOC_S_PRIORITY
<VIDIOC_G_PRIORITY>`。)

Ioctls changing driver properties, such as
:ref:`VIDIOC_S_INPUT <VIDIOC_G_INPUT>`, return an ``EBUSY`` error code
after another application obtained higher priority.
(更改驱动属性的 ioctl（例如 :ref:`VIDIOC_S_INPUT <VIDIOC_G_INPUT>`）在另一
个应用程序获得更高优先级后会返回 ``EBUSY`` 错误码。)
