=====================================================================================
 drm/tegra NVIDIA Tegra GPU and display driver(drm/tegra NVIDIA Tegra GPU 与显示驱动)
=====================================================================================

NVIDIA Tegra SoCs support a set of display, graphics and video functions via
the host1x controller. host1x supplies command streams, gathered from a push
buffer provided directly by the CPU, to its clients via channels. Software,
or blocks amongst themselves, can use syncpoints for synchronization.
(NVIDIA Tegra SoC 通过 host1x 控制器支持一组显示、图形与视频功能。host1x 把从 CPU 直接提供的 push buffer 中收集到的命令流，经通道 (channel) 传送给它的客户端。软件内部或块与块之间可以使用同步点 (syncpoint) 进行同步。)

Up until, but not including, Tegra124 (aka Tegra K1) the drm/tegra driver
supports the built-in GPU, comprised of the gr2d and gr3d engines. Starting
with Tegra124 the GPU is based on the NVIDIA desktop GPU architecture and
supported by the drm/nouveau driver.
(直到(但不包括)Tegra124(又称 Tegra K1)为止，drm/tegra 驱动支持内置 GPU，它由 gr2d 与 gr3d 引擎组成。从 Tegra124 开始，GPU 基于 NVIDIA 桌面 GPU 架构，由 drm/nouveau 驱动支持。)

The drm/tegra driver supports NVIDIA Tegra SoC generations since Tegra20. It
has three parts:
(drm/tegra 驱动自 Tegra20 起支持各代 NVIDIA Tegra SoC。它由三部分组成：)

  - A host1x driver that provides infrastructure and access to the host1x
    services.
    (- 一个 host1x 驱动，提供基础设施以及对 host1x 服务的访问。)

  - A KMS driver that supports the display controllers as well as a number of
    outputs, such as RGB, HDMI, DSI, and DisplayPort.
    (- 一个 KMS 驱动，支持显示控制器以及若干输出，如 RGB、HDMI、DSI 与 DisplayPort。)

  - A set of custom userspace IOCTLs that can be used to submit jobs to the
    GPU and video engines via host1x.
    (- 一组自定义用户空间 IOCTL，可用于经 host1x 向 GPU 与视频引擎提交作业。)

Driver Infrastructure(驱动基础设施)
===================================

The various host1x clients need to be bound together into a logical device in
order to expose their functionality to users. The infrastructure that supports
this is implemented in the host1x driver. When a driver is registered with the
infrastructure it provides a list of compatible strings specifying the devices
that it needs. The infrastructure creates a logical device and scan the device
tree for matching device nodes, adding the required clients to a list. Drivers
for individual clients register with the infrastructure as well and are added
to the logical host1x device.
(各种 host1x 客户端需要绑定成一个逻辑设备，以便向用户暴露其功能。支持这一机制的基础设施实现在 host1x 驱动中。驱动向该基础设施注册时，会提供一份兼容字符串列表，指明它需要哪些设备。基础设施创建一个逻辑设备并扫描设备树寻找匹配的设备节点，把所需的客户端加入一个列表。各个客户端的驱动也向基础设施注册，并被加入该逻辑 host1x 设备。)

Once all clients are available, the infrastructure will initialize the logical
device using a driver-provided function which will set up the bits specific to
the subsystem and in turn initialize each of its clients.
(一旦所有客户端就绪，基础设施会用驱动提供的函数初始化逻辑设备；该函数设置子系统特有的部分，并依次初始化每个客户端。)

Similarly, when one of the clients is unregistered, the infrastructure will
destroy the logical device by calling back into the driver, which ensures that
the subsystem specific bits are torn down and the clients destroyed in turn.
(类似地，当某个客户端注销时，基础设施会回调驱动以销毁逻辑设备，从而确保子系统特有的部分被拆除、各客户端被依次销毁。)

Host1x Infrastructure Reference(Host1x 基础设施参考)
----------------------------------------------------

.. kernel-doc:: include/linux/host1x.h

.. kernel-doc:: drivers/gpu/host1x/bus.c
   :export:

Host1x Syncpoint Reference(Host1x 同步点参考)
---------------------------------------------

.. kernel-doc:: drivers/gpu/host1x/syncpt.c
   :export:

KMS driver(KMS 驱动)
====================

The display hardware has remained mostly backwards compatible over the various
Tegra SoC generations, up until Tegra186 which introduces several changes that
make it difficult to support with a parameterized driver.
(显示硬件在 Tegra 各代 SoC 之间大体保持向后兼容，直到 Tegra186——它引入了多项变化，使得用参数化驱动来支持变得困难。)

Display Controllers(显示控制器)
-------------------------------

Tegra SoCs have two display controllers, each of which can be associated with
zero or more outputs. Outputs can also share a single display controller, but
only if they run with compatible display timings. Two display controllers can
also share a single framebuffer, allowing cloned configurations even if modes
on two outputs don't match. A display controller is modelled as a CRTC in KMS
terms.
(Tegra SoC 有两个显示控制器，每个可关联零个或多个输出。多个输出也可以共享同一个显示控制器，但前提是它们以兼容的显示时序运行。两个显示控制器也可以共享同一个 framebuffer，即使两个输出上的模式不匹配也能实现克隆 (clone) 配置。用 KMS 术语来说，显示控制器被建模为 CRTC。)

On Tegra186, the number of display controllers has been increased to three. A
display controller can no longer drive all of the outputs. While two of these
controllers can drive both DSI outputs and both SOR outputs, the third cannot
drive any DSI.
(在 Tegra186 上，显示控制器的数量增加到三个。单个显示控制器不再能驱动所有输出。虽然其中两个控制器可以同时驱动两个 DSI 输出和两个 SOR 输出，但第三个无法驱动任何 DSI。)

Windows(窗口)
~~~~~~~~~~~~~

A display controller controls a set of windows that can be used to composite
multiple buffers onto the screen. While it is possible to assign arbitrary Z
ordering to individual windows (by programming the corresponding blending
registers), this is currently not supported by the driver. Instead, it will
assume a fixed Z ordering of the windows (window A is the root window, that
is, the lowest, while windows B and C are overlaid on top of window A). The
overlay windows support multiple pixel formats and can automatically convert
from YUV to RGB at scanout time. This makes them useful for displaying video
content. In KMS, each window is modelled as a plane. Each display controller
has a hardware cursor that is exposed as a cursor plane.
(显示控制器控制一组窗口 (window)，可用于把多个缓冲区合成到屏幕上。虽然可以为各个窗口指定任意 Z 顺序(通过编程相应的混合寄存器)，但驱动目前不支持这一点。相反，驱动假定窗口具有固定的 Z 顺序(窗口 A 是根窗口，即最底层；窗口 B 与 C 叠放在窗口 A 之上)。overlay 窗口支持多种像素格式，并能在扫描输出 (scanout) 时自动把 YUV 转换为 RGB，这使它们很适合显示视频内容。在 KMS 中，每个窗口被建模为一个 plane。每个显示控制器还有一个硬件光标，以光标 plane 的形式暴露。)

Outputs(输出)
-------------

The type and number of supported outputs varies between Tegra SoC generations.
All generations support at least HDMI. While earlier generations supported the
very simple RGB interfaces (one per display controller), recent generations no
longer do and instead provide standard interfaces such as DSI and eDP/DP.
(支持的输出类型与数量因 Tegra SoC 世代而异。所有世代至少支持 HDMI。早期世代支持非常简单的 RGB 接口(每个显示控制器一个)，近期世代不再支持，而是提供 DSI 与 eDP/DP 等标准接口。)

Outputs are modelled as a composite encoder/connector pair.
(输出被建模为一个 encoder/connector 复合对。)

RGB/LVDS(RGB/LVDS 接口)
~~~~~~~~~~~~~~~~~~~~~~~

This interface is no longer available since Tegra124. It has been replaced by
the more standard DSI and eDP interfaces.
(该接口自 Tegra124 起不再可用。它已被更标准的 DSI 与 eDP 接口取代。)

HDMI(HDMI 接口)
~~~~~~~~~~~~~~~

HDMI is supported on all Tegra SoCs. Starting with Tegra210, HDMI is provided
by the versatile SOR output, which supports eDP, DP and HDMI. The SOR is able
to support HDMI 2.0, though support for this is currently not merged.
(所有 Tegra SoC 都支持 HDMI。从 Tegra210 开始，HDMI 由多功能的 SOR 输出提供，它支持 eDP、DP 与 HDMI。SOR 能够支持 HDMI 2.0，不过相关支持目前尚未合并。)

DSI(DSI 接口)
~~~~~~~~~~~~~

Although Tegra has supported DSI since Tegra30, the controller has changed in
several ways in Tegra114. Since none of the publicly available development
boards prior to Dalmore (Tegra114) have made use of DSI, only Tegra114 and
later are supported by the drm/tegra driver.
(虽然 Tegra 自 Tegra30 起就支持 DSI，但该控制器在 Tegra114 上发生了几处变化。由于 Dalmore(Tegra114)之前公开发售的开发板都没有使用 DSI，drm/tegra 驱动只支持 Tegra114 及之后的产品。)

eDP/DP(eDP/DP 接口)
~~~~~~~~~~~~~~~~~~~

eDP was first introduced in Tegra124 where it was used to drive the display
panel for notebook form factors. Tegra210 added support for full DisplayPort
support, though this is currently not implemented in the drm/tegra driver.
(eDP 最早在 Tegra124 上引入，用于驱动笔记本形态设备的显示面板。Tegra210 增加了完整的 DisplayPort 支持，不过 drm/tegra 驱动目前尚未实现。)

Userspace Interface(用户空间接口)
=================================

The userspace interface provided by drm/tegra allows applications to create
GEM buffers, access and control syncpoints as well as submit command streams
to host1x.
(drm/tegra 提供的用户空间接口允许应用创建 GEM 缓冲区、访问与控制同步点，以及向 host1x 提交命令流。)

GEM Buffers(GEM 缓冲区)
-----------------------

The ``DRM_IOCTL_TEGRA_GEM_CREATE`` IOCTL is used to create a GEM buffer object
with Tegra-specific flags. This is useful for buffers that should be tiled, or
that are to be scanned out upside down (useful for 3D content).
(``DRM_IOCTL_TEGRA_GEM_CREATE`` IOCTL 用于创建带有 Tegra 专属标志的 GEM 缓冲区对象。这对需要平铺 (tiled) 的缓冲区，或需要上下颠倒扫描输出的缓冲区(对 3D 内容很有用)非常有用。)

After a GEM buffer object has been created, its memory can be mapped by an
application using the mmap offset returned by the ``DRM_IOCTL_TEGRA_GEM_MMAP``
IOCTL.
(GEM 缓冲区对象创建之后，应用可以使用 ``DRM_IOCTL_TEGRA_GEM_MMAP`` IOCTL 返回的 mmap 偏移来映射它的内存。)

Syncpoints(同步点)
------------------

The current value of a syncpoint can be obtained by executing the
``DRM_IOCTL_TEGRA_SYNCPT_READ`` IOCTL. Incrementing the syncpoint is achieved
using the ``DRM_IOCTL_TEGRA_SYNCPT_INCR`` IOCTL.
(执行 ``DRM_IOCTL_TEGRA_SYNCPT_READ`` IOCTL 可以获取同步点的当前值。使用 ``DRM_IOCTL_TEGRA_SYNCPT_INCR`` IOCTL 可以递增同步点。)

Userspace can also request blocking on a syncpoint. To do so, it needs to
execute the ``DRM_IOCTL_TEGRA_SYNCPT_WAIT`` IOCTL, specifying the value of
the syncpoint to wait for. The kernel will release the application when the
syncpoint reaches that value or after a specified timeout.
(用户空间也可以请求在同步点上阻塞。为此，它需要执行 ``DRM_IOCTL_TEGRA_SYNCPT_WAIT`` IOCTL，并指定要等待的同步点值。当同步点达到该值或经过指定超时后，内核会释放应用。)

Command Stream Submission(命令流提交)
-------------------------------------

Before an application can submit command streams to host1x it needs to open a
channel to an engine using the ``DRM_IOCTL_TEGRA_OPEN_CHANNEL`` IOCTL. Client
IDs are used to identify the target of the channel. When a channel is no
longer needed, it can be closed using the ``DRM_IOCTL_TEGRA_CLOSE_CHANNEL``
IOCTL. To retrieve the syncpoint associated with a channel, an application
can use the ``DRM_IOCTL_TEGRA_GET_SYNCPT``.
(在应用可以向 host1x 提交命令流之前，它需要使用 ``DRM_IOCTL_TEGRA_OPEN_CHANNEL`` IOCTL 打开一条到某个引擎的通道。客户端 ID 用于标识通道的目标。当通道不再需要时，可以用 ``DRM_IOCTL_TEGRA_CLOSE_CHANNEL`` IOCTL 关闭它。要获取与通道关联的同步点，应用可以使用 ``DRM_IOCTL_TEGRA_GET_SYNCPT``。)

After opening a channel, submitting command streams is easy. The application
writes commands into the memory backing a GEM buffer object and passes these
to the ``DRM_IOCTL_TEGRA_SUBMIT`` IOCTL along with various other parameters,
such as the syncpoints or relocations used in the job submission.
(打开通道之后，提交命令流就很简单了。应用把命令写入 GEM 缓冲区对象背后的内存，并把这些内容连同其他各种参数(例如作业提交中使用的同步点或重定位信息)一起传给 ``DRM_IOCTL_TEGRA_SUBMIT`` IOCTL。)
