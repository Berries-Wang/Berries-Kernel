.. SPDX-License-Identifier: GFDL-1.1-no-invariants-or-later

.. _media-controller-intro:

Introduction(引言)
==================

Media devices increasingly handle multiple related functions. Many USB
cameras include microphones, video capture hardware can also output
video, or SoC camera interfaces also perform memory-to-memory operations
similar to video codecs.
(媒体设备越来越多地需要处理多个相关功能。许多 USB 摄像头包含麦克风；视频
采集硬件也可以输出视频；SoC 相机接口也会执行类似于视频编解码器的内存到内
存操作。)

Independent functions, even when implemented in the same hardware, can
be modelled as separate devices. A USB camera with a microphone will be
presented to userspace applications as V4L2 and ALSA capture devices.
The devices' relationships (when using a webcam, end-users shouldn't
have to manually select the associated USB microphone), while not made
available directly to applications by the drivers, can usually be
retrieved from sysfs.
(独立的功能即使实现在同一硬件上，也可以建模为单独的设备。带麦克风的 USB
摄像头会作为 V4L2 与 ALSA 采集设备呈现给用户空间应用程序。设备之间的关系
（例如使用网络摄像头时，最终用户不应需要手动选择关联的 USB 麦克风）虽然
不由驱动直接提供给应用程序，但通常可以从 sysfs 获取。)

With more and more advanced SoC devices being introduced, the current
approach will not scale. Device topologies are getting increasingly
complex and can't always be represented by a tree structure. Hardware
blocks are shared between different functions, creating dependencies
between seemingly unrelated devices.
(随着越来越先进的 SoC 设备不断推出，当前的做法将难以为继。设备拓扑日益复
杂，并不总能用树形结构表示。硬件块在不同功能之间共享，从而在看似无关的设
备之间制造了依赖。)

Kernel abstraction APIs such as V4L2 and ALSA provide means for
applications to access hardware parameters. As newer hardware expose an
increasingly high number of those parameters, drivers need to guess what
applications really require based on limited information, thereby
implementing policies that belong to userspace.
(V4L2 和 ALSA 等内核抽象 API 为应用程序提供了访问硬件参数的手段。随着新硬
件暴露的此类参数越来越多，驱动不得不基于有限的信息去猜测应用程序真正需要
什么，从而实现了本应属于用户空间的策略。)

The media controller API aims at solving those problems.
(媒体控制器 API 旨在解决这些问题。)
