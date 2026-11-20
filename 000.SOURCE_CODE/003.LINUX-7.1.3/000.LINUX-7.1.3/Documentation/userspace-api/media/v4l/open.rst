.. SPDX-License-Identifier: GFDL-1.1-no-invariants-or-later
.. c:namespace:: V4L

.. _open:

*******************************************
Opening and Closing Devices(打开与关闭设备)
*******************************************

.. _v4l2_hardware_control:

Controlling a hardware peripheral via V4L2(通过 V4L2 控制硬件外设)
==================================================================

Hardware that is supported using the V4L2 uAPI often consists of multiple
devices or peripherals, each of which have their own driver.
(使用 V4L2 uAPI 支持的硬件通常由多个设备或外设组成，每个都有自己的驱动。)

The bridge driver exposes one or more V4L2 device nodes
(see :ref:`v4l2_device_naming`).
(桥接驱动 (bridge driver) 暴露一个或多个 V4L2 设备节点
(见 :ref:`v4l2_device_naming`)。)

There are other drivers providing support for other components of
the hardware, which may also expose device nodes, called V4L2 sub-devices.
(还有其他驱动为硬件的其他组件提供支持，它们也可能暴露设备节点，称为 V4L2 子设备 (sub-devices)。)

When such V4L2 sub-devices are exposed, they allow controlling those
other hardware components - usually connected via a serial bus (like
I²C, SMBus or SPI). Depending on the bridge driver, those sub-devices
can be controlled indirectly via the bridge driver or explicitly via
the :ref:`Media Controller <media_controller>` and via the
:ref:`V4L2 sub-devices <subdev>`.
(当这类 V4L2 子设备被暴露时，便可以控制那些其他硬件组件——它们通常通过串行总线 (如 I²C、SMBus 或 SPI) 连接。取决于桥接驱动，这些子设备可以经由桥接驱动间接控制，也可以显式地通过
:ref:`Media Controller <media_controller>` 与
:ref:`V4L2 sub-devices <subdev>` 控制。)

The devices that require the use of the
:ref:`Media Controller <media_controller>` are called **MC-centric**
devices. The devices that are fully controlled via V4L2 device nodes
are called **video-node-centric**.
(需要使用 :ref:`Media Controller <media_controller>` 的设备称为 **MC-centric** 设备。完全通过 V4L2 设备节点控制的设备称为 **video-node-centric** 设备。)

Userspace can check if a V4L2 hardware peripheral is MC-centric by
calling :ref:`VIDIOC_QUERYCAP` and checking the
:ref:`device_caps field <device-capabilities>`.
(用户态可以通过调用 :ref:`VIDIOC_QUERYCAP` 并检查
:ref:`device_caps field <device-capabilities>` 来判断 V4L2 硬件外设是否为 MC-centric。)

If the device returns ``V4L2_CAP_IO_MC`` flag at ``device_caps``,
then it is MC-centric, otherwise, it is video-node-centric.
(如果设备在 ``device_caps`` 中返回 ``V4L2_CAP_IO_MC`` 标志，则它是 MC-centric 的；否则它是 video-node-centric 的。)

It is required for MC-centric drivers to identify the V4L2
sub-devices and to configure the pipelines via the
:ref:`media controller API <media_controller>` before using the peripheral.
Also, the sub-devices' configuration shall be controlled via the
:ref:`sub-device API <subdev>`.
(MC-centric 驱动要求在使用外设之前，先识别 V4L2 子设备并通过
:ref:`media controller API <media_controller>` 配置管线。另外，子设备的配置应通过
:ref:`sub-device API <subdev>` 控制。)

.. note::

   A video-node-centric may still provide media-controller and
   sub-device interfaces as well.
   (video-node-centric 设备仍可能同时提供 media-controller 与 sub-device 接口。)

  However, in that case the media-controller and the sub-device
  interfaces are read-only and just provide information about the
  device. The actual configuration is done via the video nodes.
  (不过在这种情况下，media-controller 与 sub-device 接口是只读的，只提供关于设备的信息。实际配置通过视频节点完成。)

.. _v4l2_device_naming:

V4L2 Device Node Naming(V4L2 设备节点命名)
===========================================

V4L2 drivers are implemented as kernel modules, loaded manually by the
system administrator or automatically when a device is first discovered.
The driver modules plug into the ``videodev`` kernel module. It provides
helper functions and a common application interface specified in this
document.
(V4L2 驱动以内核模块的形式实现，由系统管理员手动加载，或在设备首次被发现时自动加载。驱动模块插入 ``videodev`` 内核模块中。后者提供辅助函数以及本文档规定的通用应用接口。)

Each driver thus loaded registers one or more device nodes with major
number 81. Minor numbers are allocated dynamically unless the kernel
is compiled with the kernel option CONFIG_VIDEO_FIXED_MINOR_RANGES.
In that case minor numbers are allocated in ranges depending on the
device node type.
(这样加载的每个驱动都会注册一个或多个主设备号为 81 的设备节点。次设备号动态分配，除非内核编译时带 CONFIG_VIDEO_FIXED_MINOR_RANGES 内核选项。在这种情况下，次设备号按设备节点类型分区间分配。)

The device nodes supported by the Video4Linux subsystem are:
(Video4Linux 子系统支持的设备节点有：)

======================== ====================================================
Default device node name Usage
======================== ====================================================
``/dev/videoX``		 Video and metadata for capture/output devices(采集/输出设备的视频与元数据)
``/dev/vbiX``		 Vertical blank data (i.e. closed captions, teletext)(垂直消隐数据 (即隐蔽式字幕、图文电视))
``/dev/radioX``		 Radio tuners and modulators(收音调谐器与调制器)
``/dev/swradioX``	 Software Defined Radio tuners and modulators(软件定义收音调谐器与调制器)
``/dev/v4l-touchX``	 Touch sensors(触摸传感器)
``/dev/v4l-subdevX``	 Video sub-devices (used by sensors and other
			 components of the hardware peripheral)\ [#]_
			 (视频子设备，供传感器及硬件外设的其他组件使用)
======================== ====================================================

Where ``X`` is a non-negative integer.
(其中 ``X`` 是非负整数。)

.. note::

   1. The actual device node name is system-dependent, as udev rules may apply.
   2. There is no guarantee that ``X`` will remain the same for the same
      device, as the number depends on the device driver's probe order.
      If you need an unique name, udev default rules produce
      ``/dev/v4l/by-id/`` and ``/dev/v4l/by-path/`` directories containing
      links that can be used uniquely to identify a V4L2 device node::

	$ tree /dev/v4l
	/dev/v4l
	├── by-id
	│   └── usb-OmniVision._USB_Camera-B4.04.27.1-video-index0 -> ../../video0
	└── by-path
	    └── pci-0000:00:14.0-usb-0:2:1.0-video-index0 -> ../../video0

      (实际的设备节点名取决于系统，因为可能应用 udev 规则；无法保证 ``X`` 对同一设备保持不变，因为编号取决于设备驱动的探测顺序。如果你需要唯一的名称，udev 默认规则会生成 ``/dev/v4l/by-id/`` 与 ``/dev/v4l/by-path/`` 目录，其中包含可用于唯一标识 V4L2 设备节点的链接。)

.. [#] **V4L2 sub-device nodes** (e. g. ``/dev/v4l-subdevX``) use a different
       set of system calls, as covered at :ref:`subdev`.
       (**V4L2 子设备节点** (如 ``/dev/v4l-subdevX``) 使用一组不同的系统调用，见 :ref:`subdev`。)

Many drivers support "video_nr", "radio_nr" or "vbi_nr" module
options to select specific video/radio/vbi node numbers. This allows the
user to request that the device node is named e.g. /dev/video5 instead
of leaving it to chance. When the driver supports multiple devices of
the same type more than one device node number can be assigned,
separated by commas:
(许多驱动支持 "video_nr"、"radio_nr" 或 "vbi_nr" 模块选项来选择特定的 video/radio/vbi 节点编号。这允许用户请求把设备节点命名为例如 /dev/video5，而不是听天由命。当驱动支持多个同类型设备时，可以指定多个设备节点编号，用逗号分隔：)

.. code-block:: none

   # modprobe mydriver video_nr=0,1 radio_nr=0,1

In ``/etc/modules.conf`` this may be written as:
(在 ``/etc/modules.conf`` 中可以写成：)

::

    options mydriver video_nr=0,1 radio_nr=0,1

When no device node number is given as module option the driver supplies
a default.
(如果模块选项中未给出设备节点编号，驱动会提供默认值。)

Normally udev will create the device nodes in /dev automatically for
you. If udev is not installed, then you need to enable the
CONFIG_VIDEO_FIXED_MINOR_RANGES kernel option in order to be able to
correctly relate a minor number to a device node number. I.e., you need
to be certain that minor number 5 maps to device node name video5. With
this kernel option different device types have different minor number
ranges. These ranges are listed in :ref:`devices`.
(通常 udev 会自动在 /dev 中为你创建设备节点。如果未安装 udev，则需要启用 CONFIG_VIDEO_FIXED_MINOR_RANGES 内核选项，才能把次设备号正确对应到设备节点编号。也就是说，你需要能确定次设备号 5 对应设备节点名 video5。使用该内核选项时，不同设备类型拥有不同的次设备号区间。这些区间列于 :ref:`devices`。)

The creation of character special files (with mknod) is a privileged
operation and devices cannot be opened by major and minor number. That
means applications cannot *reliably* scan for loaded or installed
drivers. The user must enter a device name, or the application can try
the conventional device names.
(创建字符特殊文件 (用 mknod) 是特权操作，设备无法按主/次设备号打开。这意味着应用程序不能 *可靠地* 扫描已加载或已安装的驱动。用户必须输入设备名，或者应用程序可以尝试约定俗成的设备名。)

.. _related:

Related Devices(相关设备)
=========================

Devices can support several functions. For example video capturing, VBI
capturing and radio support.
(设备可以支持多种功能。例如视频采集、VBI 采集与收音支持。)

The V4L2 API creates different V4L2 device nodes for each of these functions.
(V4L2 API 为其中每种功能创建不同的 V4L2 设备节点。)

The V4L2 API was designed with the idea that one device node could
support all functions. However, in practice this never worked: this
'feature' was never used by applications and many drivers did not
support it and if they did it was certainly never tested. In addition,
switching a device node between different functions only works when
using the streaming I/O API, not with the
:c:func:`read()`/\ :c:func:`write()` API.
(V4L2 API 的设计理念是单个设备节点可以支持所有功能。然而实践中这从未奏效：应用程序从不使用这个"特性"，许多驱动不支持它，即便支持也肯定从未测试过。此外，把设备节点在不同功能之间切换只在使用流 I/O API 时可行，使用
:c:func:`read()`/\ :c:func:`write()` API 时不行。)

Today each V4L2 device node supports just one function.
(如今每个 V4L2 设备节点只支持一种功能。)

Besides video input or output the hardware may also support audio
sampling or playback. If so, these functions are implemented as ALSA PCM
devices with optional ALSA audio mixer devices.
(除视频输入/输出外，硬件还可能支持音频采样或播放。如果是这样，这些功能以 ALSA PCM 设备的形式实现，并可选配 ALSA 音频混音器设备。)

One problem with all these devices is that the V4L2 API makes no
provisions to find these related V4L2 device nodes. Some really complex
hardware use the Media Controller (see :ref:`media_controller`) which can
be used for this purpose. But several drivers do not use it, and while some
code exists that uses sysfs to discover related V4L2 device nodes (see
libmedia_dev in the
`v4l-utils <http://git.linuxtv.org/cgit.cgi/v4l-utils.git/>`__ git
repository), there is no library yet that can provide a single API
towards both Media Controller-based devices and devices that do not use
the Media Controller. If you want to work on this please write to the
linux-media mailing list:
`https://linuxtv.org/lists.php <https://linuxtv.org/lists.php>`__.
(所有这些设备的一个问题是，V4L2 API 没有提供查找这些相关 V4L2 设备节点的途径。一些非常复杂的硬件使用 Media Controller (见 :ref:`media_controller`)，它可用于此目的。但有些驱动不使用它；虽然存在一些用 sysfs 发现相关 V4L2 设备节点的代码 (见
`v4l-utils <http://git.linuxtv.org/cgit.cgi/v4l-utils.git/>`__ git
仓库中的 libmedia_dev)，但目前还没有一个库能为基于 Media Controller 的设备与不使用 Media Controller 的设备提供统一的 API。如果你想从事这方面的工作，请写信给 linux-media 邮件列表：
`https://linuxtv.org/lists.php <https://linuxtv.org/lists.php>`__ 。)

Multiple Opens(多次打开)
========================

V4L2 devices can be opened more than once. [#f1]_ When this is supported
by the driver, users can for example start a "panel" application to
change controls like brightness or audio volume, while another
application captures video and audio. In other words, panel applications
are comparable to an ALSA audio mixer application. Just opening a V4L2
device should not change the state of the device. [#f2]_
(V4L2 设备可以被打开多次。[#f1]_ 在驱动支持的情况下，用户可以例如启动一个 "面板" 应用程序来调整亮度或音量等控制项，同时另一个应用程序采集视频与音频。换言之，面板应用程序类似于 ALSA 音频混音器应用程序。仅仅打开 V4L2 设备不应改变设备的状态。[#f2]_ )

Once an application has allocated the memory buffers needed for
streaming data (by calling the :ref:`VIDIOC_REQBUFS`
or :ref:`VIDIOC_CREATE_BUFS` ioctls, or
implicitly by calling the :c:func:`read()` or
:c:func:`write()` functions) that application (filehandle)
becomes the owner of the device. It is no longer allowed to make changes
that would affect the buffer sizes (e.g. by calling the
:ref:`VIDIOC_S_FMT <VIDIOC_G_FMT>` ioctl) and other applications are
no longer allowed to allocate buffers or start or stop streaming. The
EBUSY error code will be returned instead.
(一旦应用程序分配了流式传输所需的内存缓冲 (通过调用 :ref:`VIDIOC_REQBUFS`
或 :ref:`VIDIOC_CREATE_BUFS` ioctl，或通过调用 :c:func:`read()` 或
:c:func:`write()` 函数隐式完成)，该应用程序 (文件句柄) 就成为设备的所有者。此后不允许再做会影响缓冲大小的更改 (例如调用
:ref:`VIDIOC_S_FMT <VIDIOC_G_FMT>` ioctl)，其他应用程序也不允许再分配缓冲或启动/停止流式传输；此时将返回 EBUSY 错误码。)

Merely opening a V4L2 device does not grant exclusive access. [#f3]_
Initiating data exchange however assigns the right to read or write the
requested type of data, and to change related properties, to this file
descriptor. Applications can request additional access privileges using
the priority mechanism described in :ref:`app-pri`.
(仅仅打开 V4L2 设备不会授予独占访问权。[#f3]_ 但发起数据交换会把读写所请求数据类型以及更改相关属性的权利赋予该文件描述符。应用程序可以使用 :ref:`app-pri` 中描述的优先级机制请求额外的访问权限。)

Shared Data Streams(共享数据流)
===============================

V4L2 drivers should not support multiple applications reading or writing
the same data stream on a device by copying buffers, time multiplexing
or similar means. This is better handled by a proxy application in user
space.
(V4L2 驱动不应通过复制缓冲、时间复用或类似手段来支持多个应用程序读写设备上的同一数据流。这更适合由用户态的代理应用程序处理。)

Functions(函数)
===============

To open and close V4L2 devices applications use the
:c:func:`open()` and :c:func:`close()` function,
respectively. Devices are programmed using the
:ref:`ioctl() <func-ioctl>` function as explained in the following
sections.
(应用程序分别使用 :c:func:`open()` 与 :c:func:`close()` 函数来打开和关闭 V4L2 设备。设备用
:ref:`ioctl() <func-ioctl>` 函数编程，详见以下各节。)

.. [#f1]
   There are still some old and obscure drivers that have not been
   updated to allow for multiple opens. This implies that for such
   drivers :c:func:`open()` can return an ``EBUSY`` error code
   when the device is already in use.
   (仍有一些老旧冷门的驱动未更新以支持多次打开。这意味着对于这类驱动，当设备已被占用时 :c:func:`open()` 可能返回 ``EBUSY`` 错误码。)

.. [#f2]
   Unfortunately, opening a radio device often switches the state of the
   device to radio mode in many drivers. This behavior should be fixed
   eventually as it violates the V4L2 specification.
   (遗憾的是，在许多驱动中打开收音设备常会把设备状态切换到收音模式。该行为最终应被修复，因为它违反 V4L2 规范。)

.. [#f3]
   Drivers could recognize the ``O_EXCL`` open flag. Presently this is
   not required, so applications cannot know if it really works.
   (驱动可以识别 ``O_EXCL`` 打开标志。目前并不要求这一点，因此应用程序无法知道它是否真的有效。)
