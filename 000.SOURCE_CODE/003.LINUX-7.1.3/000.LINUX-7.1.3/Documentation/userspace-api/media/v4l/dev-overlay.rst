.. SPDX-License-Identifier: GFDL-1.1-no-invariants-or-later
.. c:namespace:: V4L

.. _overlay:

**************************************
Video Overlay Interface(视频覆盖接口)
**************************************

**Also known as Framebuffer Overlay or Previewing.**
(**又称帧缓冲覆盖（Framebuffer Overlay）或预览（Previewing）。**)

Video overlay devices have the ability to genlock (TV-)video into the
(VGA-)video signal of a graphics card, or to store captured images
directly in video memory of a graphics card, typically with clipping.
This can be considerable more efficient than capturing images and
displaying them by other means. In the old days when only nuclear power
plants needed cooling towers this used to be the only way to put live
video into a window.
(视频覆盖设备能够把（电视）视频与显卡的（VGA）视频信号锁相，或把采集到的图像直接存入显卡的视频内存，通常还带剪裁。这比先采集图像再用其他方式显示要高效得多。在只有核电站才需要冷却塔的年代，这是把活动视频放进窗口的唯一方法。)

Video overlay devices are accessed through the same character special
files as :ref:`video capture <capture>` devices.
(视频覆盖设备通过与 :ref:`video capture <capture>`（视频采集）设备相同的字符特殊文件访问。)

.. note::

   The default function of a ``/dev/video`` device is video
   capturing. The overlay function is only available after calling
   the :ref:`VIDIOC_S_FMT <VIDIOC_G_FMT>` ioctl.
   (注意： ``/dev/video`` 设备的默认功能是视频采集。只有在调用 :ref:`VIDIOC_S_FMT <VIDIOC_G_FMT>` ioctl 之后，覆盖功能才可用。)

The driver may support simultaneous overlay and capturing using the
read/write and streaming I/O methods. If so, operation at the nominal
frame rate of the video standard is not guaranteed. Frames may be
directed away from overlay to capture, or one field may be used for
overlay and the other for capture if the capture parameters permit this.
(驱动可以通过读写和流式 I/O 方法同时支持覆盖与采集。如果是这样，则不保证以视频标准的标称帧率工作。在采集参数允许的情况下，帧可能被从覆盖转往采集，或者一个场用于覆盖、另一个场用于采集。)

Applications should use different file descriptors for capturing and
overlay. This must be supported by all drivers capable of simultaneous
capturing and overlay. Optionally these drivers may also permit
capturing and overlay with a single file descriptor for compatibility
with V4L and earlier versions of V4L2. [#f1]_
(应用程序应使用不同的文件描述符进行采集与覆盖。所有支持同时采集与覆盖的驱动都必须支持这一点。可选地，为兼容 V4L 及早期版本的 V4L2，这些驱动也可以允许用单个文件描述符同时采集与覆盖。)

A common application of two file descriptors is the X11
:ref:`Xv/V4L <xvideo>` interface driver and a V4L2 application.
While the X server controls video overlay, the application can take
advantage of memory mapping and DMA.
(两个文件描述符的一个常见应用是 X11 :ref:`Xv/V4L <xvideo>` 接口驱动与 V4L2 应用程序的组合：X 服务器控制视频覆盖，而应用程序可以利用内存映射和 DMA。)

Querying Capabilities(查询能力)
===============================

Devices supporting the video overlay interface set the
``V4L2_CAP_VIDEO_OVERLAY`` flag in the ``capabilities`` field of struct
:c:type:`v4l2_capability` returned by the
:ref:`VIDIOC_QUERYCAP` ioctl. The overlay I/O
method specified below must be supported. Tuners and audio inputs are
optional.
(支持视频覆盖接口的设备会在 :ref:`VIDIOC_QUERYCAP` ioctl 返回的 struct :c:type:`v4l2_capability` 的 ``capabilities`` 字段中设置 ``V4L2_CAP_VIDEO_OVERLAY`` 标志。下文指定的覆盖 I/O 方法必须受支持。调谐器和音频输入是可选的。)


Supplemental Functions(辅助功能)
================================

Video overlay devices shall support :ref:`audio input <audio>`,
:ref:`tuner`, :ref:`controls <control>`,
:ref:`cropping and scaling <crop>` and
:ref:`streaming parameter <streaming-par>` ioctls as needed. The
:ref:`video input <video>` and :ref:`video standard <standard>`
ioctls must be supported by all video overlay devices.
(视频覆盖设备应按需支持 :ref:`audio input <audio>`（音频输入）、:ref:`tuner`（调谐器）、:ref:`controls <control>`（控制项）、:ref:`cropping and scaling <crop>`（剪裁与缩放）和 :ref:`streaming parameter <streaming-par>`（流参数）等 ioctl。所有视频覆盖设备都必须支持 :ref:`video input <video>`（视频输入）和 :ref:`video standard <standard>`（视频标准）ioctl。)


Setup(设置)
===========

*Note: support for this has been removed.*
Before overlay can commence applications must program the driver with
frame buffer parameters, namely the address and size of the frame buffer
and the image format, for example RGB 5:6:5. The
:ref:`VIDIOC_G_FBUF <VIDIOC_G_FBUF>` and
:ref:`VIDIOC_S_FBUF <VIDIOC_G_FBUF>` ioctls are available to get and
set these parameters, respectively. The :ref:`VIDIOC_S_FBUF <VIDIOC_G_FBUF>` ioctl is
privileged because it allows to set up DMA into physical memory,
bypassing the memory protection mechanisms of the kernel. Only the
superuser can change the frame buffer address and size. Users are not
supposed to run TV applications as root or with SUID bit set. A small
helper application with suitable privileges should query the graphics
system and program the V4L2 driver at the appropriate time.
(*注：对此的支持已被移除。* 在覆盖开始之前，应用程序必须用帧缓冲参数对驱动编程，即帧缓冲的地址、大小和图像格式（例如 RGB 5:6:5）。:ref:`VIDIOC_G_FBUF <VIDIOC_G_FBUF>` 和 :ref:`VIDIOC_S_FBUF <VIDIOC_G_FBUF>` ioctl 分别用于获取和设置这些参数。:ref:`VIDIOC_S_FBUF <VIDIOC_G_FBUF>` ioctl 是特权操作，因为它允许建立向物理内存的 DMA，绕过内核的内存保护机制。只有超级用户才能修改帧缓冲地址和大小。用户不应以 root 或设置 SUID 位的方式运行电视应用程序。应当由一个具有合适权限的小型辅助应用程序查询图形系统，并在合适的时机对 V4L2 驱动编程。)

Some devices add the video overlay to the output signal of the graphics
card. In this case the frame buffer is not modified by the video device,
and the frame buffer address and pixel format are not needed by the
driver. The :ref:`VIDIOC_S_FBUF <VIDIOC_G_FBUF>` ioctl is not privileged. An application
can check for this type of device by calling the :ref:`VIDIOC_G_FBUF <VIDIOC_G_FBUF>`
ioctl.
(一些设备把视频覆盖叠加到显卡的输出信号上。这种情况下视频设备不会修改帧缓冲，驱动也不需要帧缓冲地址和像素格式，:ref:`VIDIOC_S_FBUF <VIDIOC_G_FBUF>` ioctl 不是特权操作。应用程序可以通过调用 :ref:`VIDIOC_G_FBUF <VIDIOC_G_FBUF>` ioctl 来检查设备是否属于这种类型。)

A driver may support any (or none) of five clipping/blending methods:
(驱动可以支持五种剪裁/混合方法中的任意几种（或全都不支持）：)

1. Chroma-keying displays the overlaid image only where pixels in the
   primary graphics surface assume a certain color.
   (色键（Chroma-keying）：仅当主图形表面的像素呈某种特定颜色时才显示被覆盖的图像。)

2. *Note: support for this has been removed.*
   A bitmap can be specified where each bit corresponds to a pixel in
   the overlaid image. When the bit is set, the corresponding video
   pixel is displayed, otherwise a pixel of the graphics surface.
   (*注：对此的支持已被移除。* 可以指定位图，其中每个位对应被覆盖图像的一个像素。位被置位时显示相应的视频像素，否则显示图形表面的像素。)

3. *Note: support for this has been removed.*
   A list of clipping rectangles can be specified. In these regions *no*
   video is displayed, so the graphics surface can be seen here.
   (*注：对此的支持已被移除。* 可以指定剪裁矩形列表。在这些区域内*不*显示视频，因此可以看到图形表面。)

4. The framebuffer has an alpha channel that can be used to clip or
   blend the framebuffer with the video.
   (帧缓冲带有 alpha 通道，可用于对帧缓冲与视频进行剪裁或混合。)

5. A global alpha value can be specified to blend the framebuffer
   contents with video images.
   (可以指定一个全局 alpha 值，把帧缓冲内容与视频图像混合。)

When simultaneous capturing and overlay is supported and the hardware
prohibits different image and frame buffer formats, the format requested
first takes precedence. The attempt to capture
(:ref:`VIDIOC_S_FMT <VIDIOC_G_FMT>`) or overlay
(:ref:`VIDIOC_S_FBUF <VIDIOC_G_FBUF>`) may fail with an ``EBUSY`` error
code or return accordingly modified parameters..
(当支持同时采集与覆盖、且硬件不允许图像与帧缓冲使用不同格式时，先请求的格式优先。采集（:ref:`VIDIOC_S_FMT <VIDIOC_G_FMT>`）或覆盖（:ref:`VIDIOC_S_FBUF <VIDIOC_G_FBUF>`）的尝试可能以 ``EBUSY`` 错误码失败，或返回相应修改后的参数。)


Overlay Window(覆盖窗口)
========================

The overlaid image is determined by cropping and overlay window
parameters. The former select an area of the video picture to capture,
the latter how images are overlaid and clipped. Cropping initialization
at minimum requires to reset the parameters to defaults. An example is
given in :ref:`crop`.
(被覆盖的图像由剪裁参数与覆盖窗口参数共同决定。前者选择要采集的视频画面区域，后者决定图像如何覆盖与剪裁。剪裁的初始化至少需要把参数重置为默认值。示例见 :ref:`crop`。)

The overlay window is described by a struct
:c:type:`v4l2_window`. It defines the size of the image,
its position over the graphics surface and the clipping to be applied.
To get the current parameters applications set the ``type`` field of a
struct :c:type:`v4l2_format` to
``V4L2_BUF_TYPE_VIDEO_OVERLAY`` and call the
:ref:`VIDIOC_G_FMT <VIDIOC_G_FMT>` ioctl. The driver fills the
struct :c:type:`v4l2_window` substructure named ``win``. It is not
possible to retrieve a previously programmed clipping list or bitmap.
(覆盖窗口由 struct :c:type:`v4l2_window` 描述。它定义图像的大小、其在图形表面上的位置以及要施加的剪裁。要获取当前参数，应用程序把 struct :c:type:`v4l2_format` 的 ``type`` 字段设为 ``V4L2_BUF_TYPE_VIDEO_OVERLAY`` 并调用 :ref:`VIDIOC_G_FMT <VIDIOC_G_FMT>` ioctl。驱动填充名为 ``win`` 的 struct :c:type:`v4l2_window` 子结构。无法取回先前设置的剪裁列表或位图。)

To program the overlay window applications set the ``type`` field of a
struct :c:type:`v4l2_format` to
``V4L2_BUF_TYPE_VIDEO_OVERLAY``, initialize the ``win`` substructure and
call the :ref:`VIDIOC_S_FMT <VIDIOC_G_FMT>` ioctl. The driver
adjusts the parameters against hardware limits and returns the actual
parameters as :ref:`VIDIOC_G_FMT <VIDIOC_G_FMT>` does. Like :ref:`VIDIOC_S_FMT <VIDIOC_G_FMT>`, the
:ref:`VIDIOC_TRY_FMT <VIDIOC_G_FMT>` ioctl can be used to learn
about driver capabilities without actually changing driver state. Unlike
:ref:`VIDIOC_S_FMT <VIDIOC_G_FMT>` this also works after the overlay has been enabled.
(要设置覆盖窗口，应用程序把 struct :c:type:`v4l2_format` 的 ``type`` 字段设为 ``V4L2_BUF_TYPE_VIDEO_OVERLAY``，初始化 ``win`` 子结构并调用 :ref:`VIDIOC_S_FMT <VIDIOC_G_FMT>` ioctl。驱动会按硬件限制调整参数并返回实际参数，与 :ref:`VIDIOC_G_FMT <VIDIOC_G_FMT>` 一样。与 :ref:`VIDIOC_S_FMT <VIDIOC_G_FMT>` 一样，:ref:`VIDIOC_TRY_FMT <VIDIOC_G_FMT>` ioctl 可用于了解驱动能力而不实际改变驱动状态。与 :ref:`VIDIOC_S_FMT <VIDIOC_G_FMT>` 不同的是，它在覆盖启用之后仍然有效。)

The scaling factor of the overlaid image is implied by the width and
height given in struct :c:type:`v4l2_window` and the size
of the cropping rectangle. For more information see :ref:`crop`.
(被覆盖图像的缩放因子由 struct :c:type:`v4l2_window` 中给出的宽高与剪裁矩形的大小共同隐含决定。更多信息参见 :ref:`crop`。)

When simultaneous capturing and overlay is supported and the hardware
prohibits different image and window sizes, the size requested first
takes precedence. The attempt to capture or overlay as well
(:ref:`VIDIOC_S_FMT <VIDIOC_G_FMT>`) may fail with an ``EBUSY`` error
code or return accordingly modified parameters.
(当支持同时采集与覆盖、且硬件不允许图像与窗口使用不同大小时，先请求的尺寸优先。采集或覆盖的尝试（:ref:`VIDIOC_S_FMT <VIDIOC_G_FMT>`）同样可能以 ``EBUSY`` 错误码失败，或返回相应修改后的参数。)


.. c:type:: v4l2_window

struct v4l2_window(覆盖窗口结构体)
----------------------------------

``struct v4l2_rect w``
    Size and position of the window relative to the top, left corner of
    the frame buffer defined with
    :ref:`VIDIOC_S_FBUF <VIDIOC_G_FBUF>`. The window can extend the
    frame buffer width and height, the ``x`` and ``y`` coordinates can
    be negative, and it can lie completely outside the frame buffer. The
    driver clips the window accordingly, or if that is not possible,
    modifies its size and/or position.
    (窗口相对于 :ref:`VIDIOC_S_FBUF <VIDIOC_G_FBUF>` 所定义帧缓冲左上角的大小与位置。窗口可以超出帧缓冲的宽度和高度，``x`` 与 ``y`` 坐标可以为负，也可以完全位于帧缓冲之外。驱动会相应地剪裁窗口；如果做不到，则修改其大小和/或位置。)

``enum v4l2_field field``
    Applications set this field to determine which video field shall be
    overlaid, typically one of ``V4L2_FIELD_ANY`` (0),
    ``V4L2_FIELD_TOP``, ``V4L2_FIELD_BOTTOM`` or
    ``V4L2_FIELD_INTERLACED``. Drivers may have to choose a different
    field order and return the actual setting here.
    (应用程序设置该字段以决定覆盖哪个视频场，通常为 ``V4L2_FIELD_ANY``（0）、``V4L2_FIELD_TOP``、``V4L2_FIELD_BOTTOM`` 或 ``V4L2_FIELD_INTERLACED`` 之一。驱动可能不得不选择不同的场序，并在此处返回实际设置。)

``__u32 chromakey``
    When chroma-keying has been negotiated with
    :ref:`VIDIOC_S_FBUF <VIDIOC_G_FBUF>` applications set this field
    to the desired pixel value for the chroma key. The format is the
    same as the pixel format of the framebuffer (struct
    :c:type:`v4l2_framebuffer` ``fmt.pixelformat``
    field), with bytes in host order. E. g. for
    :ref:`V4L2_PIX_FMT_BGR24 <V4L2-PIX-FMT-BGR32>` the value should
    be 0xRRGGBB on a little endian, 0xBBGGRR on a big endian host.
    (当通过 :ref:`VIDIOC_S_FBUF <VIDIOC_G_FBUF>` 协商了色键之后，应用程序把该字段设为色键所需的像素值。其格式与帧缓冲的像素格式相同（即 struct :c:type:`v4l2_framebuffer` 的 ``fmt.pixelformat`` 字段），字节按主机序排列。例如对 :ref:`V4L2_PIX_FMT_BGR24 <V4L2-PIX-FMT-BGR32>`，在小端主机上该值应为 0xRRGGBB，在大端主机上为 0xBBGGRR。)

``struct v4l2_clip * clips``
    *Note: support for this has been removed.*
    When chroma-keying has *not* been negotiated and
    :ref:`VIDIOC_G_FBUF <VIDIOC_G_FBUF>` indicated this capability,
    applications can set this field to point to an array of clipping
    rectangles.
    (*注：对此的支持已被移除。* 当*未*协商色键、且 :ref:`VIDIOC_G_FBUF <VIDIOC_G_FBUF>` 表明具备该能力时，应用程序可把该字段设为指向剪裁矩形数组的指针。)

    Like the window coordinates w, clipping rectangles are defined
    relative to the top, left corner of the frame buffer. However
    clipping rectangles must not extend the frame buffer width and
    height, and they must not overlap. If possible applications
    should merge adjacent rectangles. Whether this must create
    x-y or y-x bands, or the order of rectangles, is not defined. When
    clip lists are not supported the driver ignores this field. Its
    contents after calling :ref:`VIDIOC_S_FMT <VIDIOC_G_FMT>`
    are undefined.
    (与窗口坐标 w 一样，剪裁矩形也是相对帧缓冲左上角定义的。但剪裁矩形不得超出帧缓冲的宽度和高度，也不得相互重叠。应用程序应尽可能合并相邻矩形。至于必须形成 x-y 还是 y-x 条带、以及矩形的顺序，均未定义。不支持剪裁列表时驱动忽略该字段；调用 :ref:`VIDIOC_S_FMT <VIDIOC_G_FMT>` 之后其内容未定义。)

``__u32 clipcount``
    *Note: support for this has been removed.*
    When the application set the ``clips`` field, this field must
    contain the number of clipping rectangles in the list. When clip
    lists are not supported the driver ignores this field, its contents
    after calling :ref:`VIDIOC_S_FMT <VIDIOC_G_FMT>` are undefined. When clip lists are
    supported but no clipping is desired this field must be set to zero.
    (*注：对此的支持已被移除。* 当应用程序设置了 ``clips`` 字段时，该字段必须包含列表中剪裁矩形的数量。不支持剪裁列表时驱动忽略该字段，调用 :ref:`VIDIOC_S_FMT <VIDIOC_G_FMT>` 之后其内容未定义。支持剪裁列表但不需要剪裁时，该字段必须设为 0。)

``void * bitmap``
    *Note: support for this has been removed.*
    When chroma-keying has *not* been negotiated and
    :ref:`VIDIOC_G_FBUF <VIDIOC_G_FBUF>` indicated this capability,
    applications can set this field to point to a clipping bit mask.
    (*注：对此的支持已被移除。* 当*未*协商色键、且 :ref:`VIDIOC_G_FBUF <VIDIOC_G_FBUF>` 表明具备该能力时，应用程序可把该字段设为指向剪裁位掩码的指针。)

It must be of the same size as the window, ``w.width`` and ``w.height``.
Each bit corresponds to a pixel in the overlaid image, which is
displayed only when the bit is *set*. Pixel coordinates translate to
bits like:
(它必须与窗口（``w.width`` 和 ``w.height``）大小相同。每个位对应被覆盖图像中的一个像素，只有该位被*置位*时相应像素才被显示。像素坐标到位的换算如下：)


.. code-block:: c

    ((__u8 *) bitmap)[w.width * y + x / 8] & (1 << (x & 7))

where ``0`` ≤ x < ``w.width`` and ``0`` ≤ y <``w.height``. [#f2]_
(其中 ``0`` ≤ x < ``w.width`` 且 ``0`` ≤ y <``w.height``。)

When a clipping bit mask is not supported the driver ignores this field,
its contents after calling :ref:`VIDIOC_S_FMT <VIDIOC_G_FMT>` are
undefined. When a bit mask is supported but no clipping is desired this
field must be set to ``NULL``.
(不支持剪裁位掩码时驱动忽略该字段，调用 :ref:`VIDIOC_S_FMT <VIDIOC_G_FMT>` 之后其内容未定义。支持位掩码但不需要剪裁时，该字段必须设为 ``NULL``。)

Applications need not create a clip list or bit mask. When they pass
both, or despite negotiating chroma-keying, the results are undefined.
Regardless of the chosen method, the clipping abilities of the hardware
may be limited in quantity or quality. The results when these limits are
exceeded are undefined. [#f3]_
(应用程序无须创建剪裁列表或位掩码。如果两者都传，或者在协商了色键的情况下仍然传入，结果未定义。无论选择哪种方法，硬件的剪裁能力在数量或质量上都可能受限。超出这些限制时的结果未定义。)

``__u8 global_alpha``
    The global alpha value used to blend the framebuffer with video
    images, if global alpha blending has been negotiated
    (``V4L2_FBUF_FLAG_GLOBAL_ALPHA``, see
    :ref:`VIDIOC_S_FBUF <VIDIOC_G_FBUF>`,
    :ref:`framebuffer-flags`).
    (用于把帧缓冲与视频图像混合的全局 alpha 值（在协商了全局 alpha 混合时使用）（``V4L2_FBUF_FLAG_GLOBAL_ALPHA``，参见 :ref:`VIDIOC_S_FBUF <VIDIOC_G_FBUF>` 与 :ref:`framebuffer-flags`）。)

.. note::

   This field was added in Linux 2.6.23, extending the
   structure. However the :ref:`VIDIOC_[G|S|TRY]_FMT <VIDIOC_G_FMT>`
   ioctls, which take a pointer to a :c:type:`v4l2_format`
   parent structure with padding bytes at the end, are not affected.
   (注意： 该字段在 Linux 2.6.23 中加入，扩展了该结构体。但 :ref:`VIDIOC_[G|S|TRY]_FMT <VIDIOC_G_FMT>` 这些 ioctl 接受的是指向 :c:type:`v4l2_format` 父结构的指针（末尾带有填充字节），因此不受影响。)


.. c:type:: v4l2_clip

struct v4l2_clip [#f4]_(剪裁矩形结构体)
---------------------------------------

``struct v4l2_rect c``
    Coordinates of the clipping rectangle, relative to the top, left
    corner of the frame buffer. Only window pixels *outside* all
    clipping rectangles are displayed.
    (剪裁矩形的坐标，相对帧缓冲左上角。只有位于所有剪裁矩形*之外*的窗口像素才会被显示。)

``struct v4l2_clip * next``
    Pointer to the next clipping rectangle, ``NULL`` when this is the last
    rectangle. Drivers ignore this field, it cannot be used to pass a
    linked list of clipping rectangles.
    (指向下一个剪裁矩形的指针，若是最后一个矩形则为 ``NULL``。驱动忽略该字段，不能用它在驱动之间传递剪裁矩形的链表。)


.. c:type:: v4l2_rect

struct v4l2_rect(矩形结构体)
----------------------------

``__s32 left``
    Horizontal offset of the top, left corner of the rectangle, in
    pixels.
    (矩形左上角的水平偏移，以像素为单位。)

``__s32 top``
    Vertical offset of the top, left corner of the rectangle, in pixels.
    Offsets increase to the right and down.
    (矩形左上角的垂直偏移，以像素为单位。偏移向右、向下递增。)

``__u32 width``
    Width of the rectangle, in pixels.
    (矩形的宽度，以像素为单位。)

``__u32 height``
    Height of the rectangle, in pixels.
    (矩形的高度，以像素为单位。)


Enabling Overlay(启用覆盖)
==========================

To start or stop the frame buffer overlay applications call the
:ref:`VIDIOC_OVERLAY` ioctl.
(要启动或停止帧缓冲覆盖，应用程序调用 :ref:`VIDIOC_OVERLAY` ioctl。)

.. [#f1]
   In the opinion of the designers of this API, no driver writer taking
   the efforts to support simultaneous capturing and overlay will
   restrict this ability by requiring a single file descriptor, as in
   V4L and earlier versions of V4L2. Making this optional means
   applications depending on two file descriptors need backup routines
   to be compatible with all drivers, which is considerable more work
   than using two fds in applications which do not. Also two fd's fit
   the general concept of one file descriptor for each logical stream.
   Hence as a complexity trade-off drivers *must* support two file
   descriptors and *may* support single fd operation.
   (在该 API 的设计者看来，肯花功夫支持同时采集与覆盖的驱动作者，不会像 V4L 及早期版本的 V4L2 那样通过强制使用单个文件描述符来限制这一能力。把它做成可选，意味着依赖两个文件描述符的应用程序需要备用例程来兼容所有驱动，这比在不支持的应用里直接使用两个 fd 要麻烦得多。而且两个 fd 也符合"每个逻辑流一个文件描述符"的一般概念。因此作为复杂度上的权衡，驱动*必须*支持两个文件描述符，*可以*支持单 fd 操作。)

.. [#f2]
   Should we require ``w.width`` to be a multiple of eight?
   (我们是否应该要求 ``w.width`` 是 8 的倍数？)

.. [#f3]
   When the image is written into frame buffer memory it will be
   undesirable if the driver clips out less pixels than expected,
   because the application and graphics system are not aware these
   regions need to be refreshed. The driver should clip out more pixels
   or not write the image at all.
   (当图像被写入帧缓冲内存时，如果驱动剪裁掉的像素比预期的少，将带来问题：应用程序和图形系统并不知道这些区域需要刷新。驱动应当多剪裁一些像素，或者干脆不写图像。)

.. [#f4]
   The X Window system defines "regions" which are vectors of ``struct
   BoxRec { short x1, y1, x2, y2; }`` with ``width = x2 - x1`` and
   ``height = y2 - y1``, so one cannot pass X11 clip lists directly.
   (X Window 系统定义的"区域"（region）是 ``struct BoxRec { short x1, y1, x2, y2; }`` 的数组，其中 ``width = x2 - x1``、``height = y2 - y1``，因此无法直接传递 X11 剪裁列表。)
