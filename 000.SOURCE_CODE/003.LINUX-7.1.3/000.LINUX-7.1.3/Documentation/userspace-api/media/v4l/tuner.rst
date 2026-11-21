.. SPDX-License-Identifier: GFDL-1.1-no-invariants-or-later
.. c:namespace:: V4L

.. _tuner:

*************************************
Tuners and Modulators(调谐器与调制器)
*************************************


Tuners(调谐器)
==============

Video input devices can have one or more tuners demodulating a RF
signal. Each tuner is associated with one or more video inputs,
depending on the number of RF connectors on the tuner. The ``type``
field of the respective struct :c:type:`v4l2_input`
returned by the :ref:`VIDIOC_ENUMINPUT` ioctl is
set to ``V4L2_INPUT_TYPE_TUNER`` and its ``tuner`` field contains the
index number of the tuner.
(视频输入设备可以拥有一个或多个调谐器来解调 RF 信号。每个调谐器与一个
或多个视频输入相关联，具体取决于调谐器上 RF 连接器的数量。由
:ref:`VIDIOC_ENUMINPUT` ioctl 返回的相应 struct :c:type:`v4l2_input` 的
``type`` 字段被设置为 ``V4L2_INPUT_TYPE_TUNER``，其 ``tuner`` 字段包含
该调谐器的索引号。)

Radio input devices have exactly one tuner with index zero, no video
inputs.
(收音输入设备只有一个索引为零的调谐器，没有视频输入。)

To query and change tuner properties applications use the
:ref:`VIDIOC_G_TUNER <VIDIOC_G_TUNER>` and
:ref:`VIDIOC_S_TUNER <VIDIOC_G_TUNER>` ioctls, respectively. The
struct :c:type:`v4l2_tuner` returned by :ref:`VIDIOC_G_TUNER <VIDIOC_G_TUNER>`
also contains signal status information applicable when the tuner of the
current video or radio input is queried.
(应用程序分别使用 :ref:`VIDIOC_G_TUNER <VIDIOC_G_TUNER>` 与
:ref:`VIDIOC_S_TUNER <VIDIOC_G_TUNER>` ioctl 来查询和更改调谐器属性。
由 :ref:`VIDIOC_G_TUNER <VIDIOC_G_TUNER>` 返回的 struct
:c:type:`v4l2_tuner` 还包含信号状态信息，适用于查询当前视频或收音输入
的调谐器时使用。)

.. note::

   :ref:`VIDIOC_S_TUNER <VIDIOC_G_TUNER>` does not switch the
   current tuner, when there is more than one. The tuner is solely
   determined by the current video input. Drivers must support both ioctls
   and set the ``V4L2_CAP_TUNER`` flag in the struct :c:type:`v4l2_capability`
   returned by the :ref:`VIDIOC_QUERYCAP` ioctl when the
   device has one or more tuners.
   (:ref:`VIDIOC_S_TUNER <VIDIOC_G_TUNER>` 在有多个调谐器时并不会切换
   当前调谐器。调谐器完全由当前视频输入决定。当设备拥有一个或多个调谐
   器时，驱动程序必须支持这两个 ioctl，并在由 :ref:`VIDIOC_QUERYCAP`
   ioctl 返回的 struct :c:type:`v4l2_capability` 中设置
   ``V4L2_CAP_TUNER`` 标志。)


Modulators(调制器)
==================

Video output devices can have one or more modulators, that modulate a
video signal for radiation or connection to the antenna input of a TV
set or video recorder. Each modulator is associated with one or more
video outputs, depending on the number of RF connectors on the
modulator. The ``type`` field of the respective struct
:c:type:`v4l2_output` returned by the
:ref:`VIDIOC_ENUMOUTPUT` ioctl is set to
``V4L2_OUTPUT_TYPE_MODULATOR`` and its ``modulator`` field contains the
index number of the modulator.
(视频输出设备可以拥有一个或多个调制器，用于调制视频信号以便辐射发射，
或连接到电视机或录像机的天线输入。每个调制器与一个或多个视频输出相关
联，具体取决于调制器上 RF 连接器的数量。由 :ref:`VIDIOC_ENUMOUTPUT`
ioctl 返回的相应 struct :c:type:`v4l2_output` 的 ``type`` 字段被设置为
``V4L2_OUTPUT_TYPE_MODULATOR``，其 ``modulator`` 字段包含该调制器的
索引号。)

Radio output devices have exactly one modulator with index zero, no
video outputs.
(收音输出设备只有一个索引为零的调制器，没有视频输出。)

A video or radio device cannot support both a tuner and a modulator. Two
separate device nodes will have to be used for such hardware, one that
supports the tuner functionality and one that supports the modulator
functionality. The reason is a limitation with the
:ref:`VIDIOC_S_FREQUENCY <VIDIOC_G_FREQUENCY>` ioctl where you
cannot specify whether the frequency is for a tuner or a modulator.
(视频或收音设备不能同时支持调谐器和调制器。这样的硬件必须使用两个独立
的设备节点，一个支持调谐器功能，另一个支持调制器功能。原因在于
:ref:`VIDIOC_S_FREQUENCY <VIDIOC_G_FREQUENCY>` ioctl 的一个局限：无法
指定频率是用于调谐器还是调制器。)

To query and change modulator properties applications use the
:ref:`VIDIOC_G_MODULATOR <VIDIOC_G_MODULATOR>` and
:ref:`VIDIOC_S_MODULATOR <VIDIOC_G_MODULATOR>` ioctl. Note that
:ref:`VIDIOC_S_MODULATOR <VIDIOC_G_MODULATOR>` does not switch the current modulator, when there
is more than one at all. The modulator is solely determined by the
current video output. Drivers must support both ioctls and set the
``V4L2_CAP_MODULATOR`` flag in the struct
:c:type:`v4l2_capability` returned by the
:ref:`VIDIOC_QUERYCAP` ioctl when the device has
one or more modulators.
(应用程序使用 :ref:`VIDIOC_G_MODULATOR <VIDIOC_G_MODULATOR>` 与
:ref:`VIDIOC_S_MODULATOR <VIDIOC_G_MODULATOR>` ioctl 来查询和更改调制
器属性。注意，当有多个调制器时，
:ref:`VIDIOC_S_MODULATOR <VIDIOC_G_MODULATOR>` 完全不会切换当前调制
器。调制器完全由当前视频输出决定。当设备拥有一个或多个调制器时，驱动
程序必须支持这两个 ioctl，并在由 :ref:`VIDIOC_QUERYCAP` ioctl 返回的
struct :c:type:`v4l2_capability` 中设置 ``V4L2_CAP_MODULATOR`` 标志。)


Radio Frequency(射频)
=====================

To get and set the tuner or modulator radio frequency applications use
the :ref:`VIDIOC_G_FREQUENCY <VIDIOC_G_FREQUENCY>` and
:ref:`VIDIOC_S_FREQUENCY <VIDIOC_G_FREQUENCY>` ioctl which both take
a pointer to a struct :c:type:`v4l2_frequency`. These
ioctls are used for TV and radio devices alike. Drivers must support
both ioctls when the tuner or modulator ioctls are supported, or when
the device is a radio device.
(应用程序使用 :ref:`VIDIOC_G_FREQUENCY <VIDIOC_G_FREQUENCY>` 与
:ref:`VIDIOC_S_FREQUENCY <VIDIOC_G_FREQUENCY>` ioctl 来获取和设置调谐
器或调制器的射频，这两个 ioctl 都接受指向 struct
:c:type:`v4l2_frequency` 的指针。这些 ioctl 对电视和收音设备通用。当
支持调谐器或调制器 ioctl 时，或当设备是收音设备时，驱动程序必须支持
这两个 ioctl。)
