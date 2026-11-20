.. SPDX-License-Identifier: GPL-2.0
.. Copyright 2021-2023 Collabora Ltd.

========================================
Exchanging pixel buffers(交换像素缓冲区)
========================================

As originally designed, the Linux graphics subsystem had extremely limited
support for sharing pixel-buffer allocations between processes, devices, and
subsystems. Modern systems require extensive integration between all three
classes; this document details how applications and kernel subsystems should
approach this sharing for two-dimensional image data.
(在最初的设计中，Linux 图形子系统对在进程、设备与子系统之间共享像素缓冲区分配的支持极为有限。现代系统要求这三类对象之间有广泛的整合；本文档详细说明应用程序与内核子系统应如何针对二维图像数据进行这种共享。)

It is written with reference to the DRM subsystem for GPU and display devices,
V4L2 for media devices, and also to Vulkan, EGL and Wayland, for userspace
support, however any other subsystems should also follow this design and advice.
(本文以面向 GPU 与显示设备的 DRM 子系统、面向媒体设备的 V4L2，以及面向用户空间支持的 Vulkan、EGL 与 Wayland 为参照撰写，但任何其他子系统也应遵循这一设计与建议。)


Glossary of terms(术语表)
=========================

.. glossary::

    image:
    (图像)

      Conceptually a two-dimensional array of pixels. The pixels may be stored
      in one or more memory buffers. Has width and height in pixels, pixel
      format and modifier (implicit or explicit).
      (概念上是一个二维的像素数组。这些像素可以存储在一个或多个内存缓冲区中。带有以像素为单位的宽度与高度、像素格式以及修饰符 (modifier)(隐式或显式)。)

    row:
    (行)

      A span along a single y-axis value, e.g. from co-ordinates (0,100) to
      (200,100).
      (沿单个 y 轴值的一段区间，例如从坐标 (0,100) 到 (200,100)。)

    scanline:
    (扫描行)

      Synonym for row.
      (row 的同义词。)

    column:
    (列)

      A span along a single x-axis value, e.g. from co-ordinates (100,0) to
      (100,100).
      (沿单个 x 轴值的一段区间，例如从坐标 (100,0) 到 (100,100)。)

    memory buffer:
    (内存缓冲区)

      A piece of memory for storing (parts of) pixel data. Has stride and size
      in bytes and at least one handle in some API. May contain one or more
      planes.
      (用于存储(部分)像素数据的一块内存。带有以字节为单位的 stride 与大小，以及在某个 API 中至少一个句柄。可能包含一个或多个平面。)

    plane:
    (平面)

      A two-dimensional array of some or all of an image's color and alpha
      channel values.
      (图像的部分或全部颜色与 alpha 通道值构成的二维数组。)

    pixel:
    (像素)

      A picture element. Has a single color value which is defined by one or
      more color channels values, e.g. R, G and B, or Y, Cb and Cr. May also
      have an alpha value as an additional channel.
      (图像元素。具有由一个或多个颜色通道值定义的单个颜色值，例如 R、G 与 B，或 Y、Cb 与 Cr。还可以带有一个作为附加通道的 alpha 值。)

    pixel data:
    (像素数据)

      Bytes or bits that represent some or all of the color/alpha channel values
      of a pixel or an image. The data for one pixel may be spread over several
      planes or memory buffers depending on format and modifier.
      (表示某个像素或图像的部分或全部颜色/alpha 通道值的字节或比特。取决于格式与修饰符，一个像素的数据可能分散在多个平面或内存缓冲区中。)

    color value:
    (颜色值)

      A tuple of numbers, representing a color. Each element in the tuple is a
      color channel value.
      (表示一个颜色的一组数值。该组中的每个元素是一个颜色通道值。)

    color channel:
    (颜色通道)

      One of the dimensions in a color model. For example, RGB model has
      channels R, G, and B. Alpha channel is sometimes counted as a color
      channel as well.
      (颜色模型中的一个维度。例如，RGB 模型有 R、G 与 B 三个通道。alpha 通道有时也被算作颜色通道。)

    pixel format:
    (像素格式)

      A description of how pixel data represents the pixel's color and alpha
      values.
      (对像素数据如何表示像素的颜色与 alpha 值的描述。)

    modifier:
    (修饰符)

      A description of how pixel data is laid out in memory buffers.
      (对像素数据在内存缓冲区中如何布局的描述。)

    alpha:
    (alpha)

      A value that denotes the color coverage in a pixel. Sometimes used for
      translucency instead.
      (表示像素中颜色覆盖率的值。有时也被用于表示半透明度。)

    stride:
    (步长)

      A value that denotes the relationship between pixel-location co-ordinates
      and byte-offset values. Typically used as the byte offset between two
      pixels at the start of vertically-consecutive tiling blocks. For linear
      layouts, the byte offset between two vertically-adjacent pixels. For
      non-linear formats the stride must be computed in a consistent way, which
      usually is done as-if the layout was linear.
      (表示像素位置坐标与字节偏移值之间关系的值。通常用作垂直相邻的两个平铺块起始处两个像素之间的字节偏移。对线性布局而言，是垂直相邻两个像素之间的字节偏移。对非线性格式，stride 必须以一致的方式计算，通常按布局是线性时的方式计算。)

    pitch:
    (行距)

      Synonym for stride.
      (stride 的同义词。)


Formats and modifiers(格式与修饰符)
===================================

Each buffer must have an underlying format. This format describes the color
values provided for each pixel. Although each subsystem has its own format
descriptions (e.g. V4L2 and fbdev), the ``DRM_FORMAT_*`` tokens should be reused
wherever possible, as they are the standard descriptions used for interchange.
These tokens are described in the ``drm_fourcc.h`` file, which is a part of
DRM's uAPI.
(每个缓冲区都必须有一个底层格式。该格式描述为每个像素提供的颜色值。虽然每个子系统都有自己的格式描述(例如 V4L2 与 fbdev)，但应尽可能复用 ``DRM_FORMAT_*`` 令牌，因为它们是用于交换的标准描述。这些令牌在 ``drm_fourcc.h`` 文件中描述，该文件是 DRM uAPI 的一部分。)

Each ``DRM_FORMAT_*`` token describes the translation between a pixel
co-ordinate in an image, and the color values for that pixel contained within
its memory buffers. The number and type of color channels are described:
whether they are RGB or YUV, integer or floating-point, the size of each channel
and their locations within the pixel memory, and the relationship between color
planes.
(每个 ``DRM_FORMAT_*`` 令牌描述图像中像素坐标与该像素在其内存缓冲区中的颜色值之间的换算关系。其中描述了颜色通道的数量与类型：是 RGB 还是 YUV、整数还是浮点、每个通道的大小及其在像素内存中的位置，以及颜色平面之间的关系。)

For example, ``DRM_FORMAT_ARGB8888`` describes a format in which each pixel has
a single 32-bit value in memory. Alpha, red, green, and blue, color channels are
available at 8-bit precision per channel, ordered respectively from most to
least significant bits in little-endian storage. ``DRM_FORMAT_*`` is not
affected by either CPU or device endianness; the byte pattern in memory is
always as described in the format definition, which is usually little-endian.
(例如，``DRM_FORMAT_ARGB8888`` 描述的格式中，每个像素在内存中有一个 32 位值。alpha、红、绿、蓝四个颜色通道各以 8 位精度提供，在小端存储中按从最高有效位到最低有效位的顺序排列。 ``DRM_FORMAT_*`` 不受 CPU 或设备字节序的影响；内存中的字节模式始终与格式定义一致，通常为小端。)

As a more complex example, ``DRM_FORMAT_NV12`` describes a format in which luma
and chroma YUV samples are stored in separate planes, where the chroma plane is
stored at half the resolution in both dimensions (i.e. one U/V chroma
sample is stored for each 2x2 pixel grouping).
(一个更复杂的例子：``DRM_FORMAT_NV12`` 描述的格式中，YUV 的亮度 (luma) 与色度 (chroma) 样本存储在分离的平面中，色度平面在两个维度上均以一半分辨率存储(即每 2x2 像素分组存储一个 U/V 色度样本)。)

Format modifiers describe a translation mechanism between these per-pixel memory
samples, and the actual memory storage for the buffer. The most straightforward
modifier is ``DRM_FORMAT_MOD_LINEAR``, describing a scheme in which each plane
is laid out row-sequentially, from the top-left to the bottom-right corner.
This is considered the baseline interchange format, and most convenient for CPU
access.
(格式修饰符描述这些逐像素内存样本与缓冲区实际内存存储之间的转换机制。最直接的修饰符是 ``DRM_FORMAT_MOD_LINEAR``，它描述的方案中每个平面按行顺序排列，从左上角到右下角。这被视为基线交换格式，也最便于 CPU 访问。)

Modern hardware employs much more sophisticated access mechanisms, typically
making use of tiled access and possibly also compression. For example, the
``DRM_FORMAT_MOD_VIVANTE_TILED`` modifier describes memory storage where pixels
are stored in 4x4 blocks arranged in row-major ordering, i.e. the first tile in
a plane stores pixels (0,0) to (3,3) inclusive, and the second tile in a plane
stores pixels (4,0) to (7,3) inclusive.
(现代硬件采用复杂得多的访问机制，通常利用平铺 (tiled) 访问，还可能加上压缩。例如，``DRM_FORMAT_MOD_VIVANTE_TILED`` 修饰符描述的内存存储中，像素以 4x4 块、按行优先顺序存放，即平面中的第一个块存放像素 (0,0) 到 (3,3)(含)，第二个块存放像素 (4,0) 到 (7,3)(含)。)

Some modifiers may modify the number of planes required for an image; for
example, the ``I915_FORMAT_MOD_Y_TILED_CCS`` modifier adds a second plane to RGB
formats in which it stores data about the status of every tile, notably
including whether the tile is fully populated with pixel data, or can be
expanded from a single solid color.
(某些修饰符会改变图像所需的平面数量；例如 ``I915_FORMAT_MOD_Y_TILED_CCS`` 修饰符为 RGB 格式增加第二个平面，其中存储每个平铺块的状态数据，特别是该块是完全填充了像素数据、还是可以从单一纯色展开。)

These extended layouts are highly vendor-specific, and even specific to
particular generations or configurations of devices per-vendor. For this reason,
support of modifiers must be explicitly enumerated and negotiated by all users
in order to ensure a compatible and optimal pipeline, as discussed below.
(这些扩展布局高度依赖特定厂商，甚至因厂商而异、特定于某些设备代次或配置。因此，如下文所述，所有使用者必须显式枚举并协商对修饰符的支持，以确保管线兼容且最优。)


Dimensions and size(尺寸与大小)
===============================

Each pixel buffer must be accompanied by logical pixel dimensions. This refers
to the number of unique samples which can be extracted from, or stored to, the
underlying memory storage. For example, even though a 1920x1080
``DRM_FORMAT_NV12`` buffer has a luma plane containing 1920x1080 samples for the Y
component, and 960x540 samples for the U and V components, the overall buffer is
still described as having dimensions of 1920x1080.
(每个像素缓冲区都必须带有逻辑像素尺寸。它指的是可以从底层内存存储中提取、或写入其中的唯一样本的数量。例如，一个 1920x1080 的 ``DRM_FORMAT_NV12`` 缓冲区，其亮度平面包含 Y 分量的 1920x1080 个样本、U 与 V 分量的 960x540 个样本，但整个缓冲区仍被描述为尺寸 1920x1080。)

The in-memory storage of a buffer is not guaranteed to begin immediately at the
base address of the underlying memory, nor is it guaranteed that the memory
storage is tightly clipped to either dimension.
(缓冲区的内存存储不保证恰好从底层内存的基地址开始，也不保证内存存储在任一维度上被紧凑地裁剪。)

Each plane must therefore be described with an ``offset`` in bytes, which will be
added to the base address of the memory storage before performing any per-pixel
calculations. This may be used to combine multiple planes into a single memory
buffer; for example, ``DRM_FORMAT_NV12`` may be stored in a single memory buffer
where the luma plane's storage begins immediately at the start of the buffer
with an offset of 0, and the chroma plane's storage follows within the same buffer
beginning from the byte offset for that plane.
(因此每个平面都必须以字节为单位的 ``offset`` 来描述，在进行任何逐像素计算之前，该偏移会加到内存存储的基地址上。这可用于把多个平面合并进单个内存缓冲区；例如 ``DRM_FORMAT_NV12`` 可以存储在单个内存缓冲区中，亮度平面的存储从缓冲区起点、偏移 0 处立即开始，色度平面的存储则在同一缓冲区内从该平面的字节偏移处接着存放。)

Each plane must also have a ``stride`` in bytes, expressing the offset in memory
between two contiguous row. For example, a ``DRM_FORMAT_MOD_LINEAR`` buffer
with dimensions of 1000x1000 may have been allocated as if it were 1024x1000, in
order to allow for aligned access patterns. In this case, the buffer will still
be described with a width of 1000, however the stride will be ``1024 * bpp``,
indicating that there are 24 pixels at the positive extreme of the x axis whose
values are not significant.
(每个平面还必须有以字节为单位的 ``stride``，表示内存中两个相邻行之间的偏移。例如，一个尺寸为 1000x1000 的 ``DRM_FORMAT_MOD_LINEAR`` 缓冲区，可能是按 1024x1000 分配的，以便支持对齐的访问模式。此时缓冲区仍以宽度 1000 描述，但 stride 将是 ``1024 * bpp``，表明 x 轴正方向上有 24 个像素的值并不重要。)

Buffers may also be padded further in the y dimension, simply by allocating a
larger area than would ordinarily be required. For example, many media decoders
are not able to natively output buffers of height 1080, but instead require an
effective height of 1088 pixels. In this case, the buffer continues to be
described as having a height of 1080, with the memory allocation for each buffer
being increased to account for the extra padding.
(缓冲区也可以在 y 维度上进一步填充，只需分配比通常所需更大的区域即可。例如，许多媒体解码器无法原生输出高度 1080 的缓冲区，而是要求 1088 像素的有效高度。这种情况下，缓冲区仍以高度 1080 描述，只是把每个缓冲区的内存分配加大以容纳额外的填充。)


Enumeration(枚举)
=================

Every user of pixel buffers must be able to enumerate a set of supported formats
and modifiers, described together. Within KMS, this is achieved with the
``IN_FORMATS`` property on each DRM plane, listing the supported DRM formats, and
the modifiers supported for each format. In userspace, this is supported through
the `EGL_EXT_image_dma_buf_import_modifiers`_ extension entrypoints for EGL, the
`VK_EXT_image_drm_format_modifier`_ extension for Vulkan, and the
`zwp_linux_dmabuf_v1`_ extension for Wayland.
(像素缓冲区的每个使用者都必须能够把一组受支持的格式与修饰符放在一起进行枚举。在 KMS 中，这通过每个 DRM 平面上的 ``IN_FORMATS`` 属性实现，其中列出受支持的 DRM 格式，以及每种格式受支持的修饰符。在用户空间，这通过 EGL 的 `EGL_EXT_image_dma_buf_import_modifiers`_ 扩展入口点、Vulkan 的 `VK_EXT_image_drm_format_modifier`_ 扩展以及 Wayland 的 `zwp_linux_dmabuf_v1`_ 扩展来支持。)

Each of these interfaces allows users to query a set of supported
format+modifier combinations.
(这些接口都允许用户查询一组受支持的"格式+修饰符"组合。)


Negotiation(协商)
=================

It is the responsibility of userspace to negotiate an acceptable format+modifier
combination for its usage. This is performed through a simple intersection of
lists. For example, if a user wants to use Vulkan to render an image to be
displayed on a KMS plane, it must:
(为它的用途协商出一个可接受的"格式+修饰符"组合是用户空间的责任。这通过简单的列表求交集完成。例如，用户想用 Vulkan 渲染一幅图像并显示在 KMS 平面上，就必须：)

 - query KMS for the ``IN_FORMATS`` property for the given plane
 - query Vulkan for the supported formats for its physical device, making sure
   to pass the ``VkImageUsageFlagBits`` and ``VkImageCreateFlagBits``
   corresponding to the intended rendering use
 - intersect these formats to determine the most appropriate one
 - for this format, intersect the lists of supported modifiers for both KMS and
   Vulkan, to obtain a final list of acceptable modifiers for that format

 (向 KMS 查询给定平面的 ``IN_FORMATS`` 属性)
 (向 Vulkan 查询其物理设备支持的格式，并确保传入与预期渲染用途相对应的 ``VkImageUsageFlagBits`` 与 ``VkImageCreateFlagBits``)
 (对这些格式求交集，确定最合适的一个)
 (针对该格式，对 KMS 与 Vulkan 二者受支持的修饰符列表求交集，得到该格式最终可接受的修饰符列表)

This intersection must be performed for all usages. For example, if the user
also wishes to encode the image to a video stream, it must query the media API
it intends to use for encoding for the set of modifiers it supports, and
additionally intersect against this list.
(这一求交集必须针对所有用途进行。例如，如果用户还想把该图像编码为视频流，就必须向打算用于编码的媒体 API 查询它支持的修饰符集合，并再与该列表求一次交集。)

If the intersection of all lists is an empty list, it is not possible to share
buffers in this way, and an alternate strategy must be considered (e.g. using
CPU access routines to copy data between the different uses, with the
corresponding performance cost).
(如果所有列表的交集为空，就无法以这种方式共享缓冲区，必须考虑替代策略(例如使用 CPU 访问例程在不同用途之间复制数据，并付出相应的性能代价)。)

The resulting modifier list is unsorted; the order is not significant.
(最终得到的修饰符列表是无序的；顺序没有意义。)


Allocation(分配)
================

Once userspace has determined an appropriate format, and corresponding list of
acceptable modifiers, it must allocate the buffer. As there is no universal
buffer-allocation interface available at either kernel or userspace level, the
client makes an arbitrary choice of allocation interface such as Vulkan, GBM, or
a media API.
(一旦用户空间确定了合适的格式以及相应的可接受修饰符列表，它就必须分配缓冲区。由于在内核或用户空间层面都不存在通用的缓冲区分配接口，客户端可以任意选择分配接口，例如 Vulkan、GBM 或某个媒体 API。)

Each allocation request must take, at a minimum: the pixel format, a list of
acceptable modifiers, and the buffer's width and height. Each API may extend
this set of properties in different ways, such as allowing allocation in more
than two dimensions, intended usage patterns, etc.
(每个分配请求至少必须提供：像素格式、可接受修饰符列表，以及缓冲区的宽度与高度。每个 API 可以以不同方式扩展这组属性，例如允许超过两个维度的分配、预期的使用模式等。)

The component which allocates the buffer will make an arbitrary choice of what
it considers the 'best' modifier within the acceptable list for the requested
allocation, any padding required, and further properties of the underlying
memory buffers such as whether they are stored in system or device-specific
memory, whether or not they are physically contiguous, and their cache mode.
These properties of the memory buffer are not visible to userspace, however the
``dma-heaps`` API is an effort to address this.
(分配缓冲区的组件将在可接受列表内，就它认为对所请求分配"最佳"的修饰符、所需的任何填充，以及底层内存缓冲区的其他属性做出任意选择，例如存储在系统内存还是设备专属内存、是否物理连续、缓存模式等。这些内存缓冲区属性对用户空间不可见，不过 ``dma-heaps`` API 正是为解决这一问题所做的努力。)

After allocation, the client must query the allocator to determine the actual
modifier selected for the buffer, as well as the per-plane offset and stride.
Allocators are not permitted to vary the format in use, to select a modifier not
provided within the acceptable list, nor to vary the pixel dimensions other than
the padding expressed through offset, stride, and size.
(分配之后，客户端必须查询分配器，确定为该缓冲区实际选择的修饰符，以及每个平面的 offset 与 stride。分配器不允许改变所用的格式，不允许选择可接受列表之外的修饰符，也不允许改变像素尺寸——通过 offset、stride 与 size 表达的填充除外。)

Communicating additional constraints, such as alignment of stride or offset,
placement within a particular memory area, etc, is out of scope of dma-buf,
and is not solved by format and modifier tokens.
(传达额外的约束——例如 stride 或 offset 的对齐、在特定内存区域中的放置位置等——超出了 dma-buf 的范围，格式与修饰符令牌也不解决这些问题。)


Import(导入)
============

To use a buffer within a different context, device, or subsystem, the user
passes these parameters (format, modifier, width, height, and per-plane offset
and stride) to an importing API.
(要在另一个上下文、设备或子系统中使用缓冲区，用户把这些参数(格式、修饰符、宽度、高度，以及每个平面的 offset 与 stride)传给导入 API。)

Each memory buffer is referred to by a buffer handle, which may be unique or
duplicated within an image. For example, a ``DRM_FORMAT_NV12`` buffer may have
the luma and chroma buffers combined into a single memory buffer by use of the
per-plane offset parameters, or they may be completely separate allocations in
memory. For this reason, each import and allocation API must provide a separate
handle for each plane.
(每个内存缓冲区都由一个缓冲区句柄指代，句柄在一幅图像中可以是唯一的，也可以重复。例如，``DRM_FORMAT_NV12`` 缓冲区可以借助每平面 offset 参数把亮度与色度缓冲区合并到单个内存缓冲区中，也可以是内存中完全独立的分配。因此，每个导入与分配 API 都必须为每个平面提供单独的句柄。)

Each kernel subsystem has its own types and interfaces for buffer management.
DRM uses GEM buffer objects (BOs), V4L2 has its own references, etc. These types
are not portable between contexts, processes, devices, or subsystems.
(每个内核子系统都有自己的缓冲区管理类型与接口。DRM 使用 GEM 缓冲区对象 (BO)，V4L2 有自己的引用，等等。这些类型在上下文、进程、设备或子系统之间不可移植。)

To address this, ``dma-buf`` handles are used as the universal interchange for
buffers. Subsystem-specific operations are used to export native buffer handles
to a ``dma-buf`` file descriptor, and to import those file descriptors into a
native buffer handle. dma-buf file descriptors can be transferred between
contexts, processes, devices, and subsystems.
(为解决这一问题，``dma-buf`` 句柄被用作缓冲区的通用交换媒介。各子系统用专属操作把本机缓冲区句柄导出为 ``dma-buf`` 文件描述符，以及把这些文件描述符导入为本机缓冲区句柄。dma-buf 文件描述符可以在上下文、进程、设备与子系统之间传递。)

For example, a Wayland media player may use V4L2 to decode a video frame into a
``DRM_FORMAT_NV12`` buffer. This will result in two memory planes (luma and
chroma) being dequeued by the user from V4L2. These planes are then exported to
one dma-buf file descriptor per plane, these descriptors are then sent along
with the metadata (format, modifier, width, height, per-plane offset and stride)
to the Wayland server. The Wayland server will then import these file
descriptors as an EGLImage for use through EGL/OpenGL (ES), a VkImage for use
through Vulkan, or a KMS framebuffer object; each of these import operations
will take the same metadata and convert the dma-buf file descriptors into their
native buffer handles.
(例如，一个 Wayland 媒体播放器可以用 V4L2 把视频帧解码进 ``DRM_FORMAT_NV12`` 缓冲区。用户随后从 V4L2 中出队两个内存平面(亮度与色度)。这些平面随后被导出，每个平面一个 dma-buf 文件描述符；这些描述符连同元数据(格式、修饰符、宽度、高度、每平面 offset 与 stride)一起发送给 Wayland 服务器。Wayland 服务器随后把这些文件描述符导入为 EGLImage 以便通过 EGL/OpenGL (ES) 使用、导入为 VkImage 以便通过 Vulkan 使用，或导入为 KMS framebuffer 对象；每种导入操作都会使用同样的元数据，把 dma-buf 文件描述符转换成各自的本机缓冲区句柄。)

Having a non-empty intersection of supported modifiers does not guarantee that
import will succeed into all consumers; they may have constraints beyond those
implied by modifiers which must be satisfied.
(受支持修饰符的交集非空，并不能保证导入到所有消费者都会成功；它们可能有修饰符之外的、必须满足的约束。)


Implicit modifiers(隐式修饰符)
==============================

The concept of modifiers post-dates all of the subsystems mentioned above. As
such, it has been retrofitted into all of these APIs, and in order to ensure
backwards compatibility, support is needed for drivers and userspace which do
not (yet) support modifiers.
(修饰符的概念晚于上文提到的所有子系统。因此它被 retrofit 进所有这些 API；为了保证向后兼容，需要支持那些尚不支持修饰符的驱动与用户空间。)

As an example, GBM is used to allocate buffers to be shared between EGL for
rendering and KMS for display. It has two entrypoints for allocating buffers:
``gbm_bo_create`` which only takes the format, width, height, and a usage token,
and ``gbm_bo_create_with_modifiers`` which extends this with a list of modifiers.
(以 GBM 为例，它用于分配在负责渲染的 EGL 与负责显示的 KMS 之间共享的缓冲区。它有两个缓冲区分配入口点：只接受格式、宽度、高度与用途令牌的 ``gbm_bo_create``，以及在此基础上扩展出修饰符列表的 ``gbm_bo_create_with_modifiers``。)

In the latter case, the allocation is as discussed above, being provided with a
list of acceptable modifiers that the implementation can choose from (or fail if
it is not possible to allocate within those constraints). In the former case
where modifiers are not provided, the GBM implementation must make its own
choice as to what is likely to be the 'best' layout. Such a choice is entirely
implementation-specific: some will internally use tiled layouts which are not
CPU-accessible if the implementation decides that is a good idea through
whatever heuristic. It is the implementation's responsibility to ensure that
this choice is appropriate.
(在后一种情况下，分配如上文所述：实现可以从可接受修饰符列表中选择(若无法在约束内分配则失败)。在前一种不提供修饰符的情况下，GBM 实现必须自行判断哪种布局可能是"最佳"的。这种选择完全取决于实现：某些实现在内部使用 CPU 无法访问的平铺布局，只要实现通过某种启发式规则认为这是个好主意。确保这一选择合适是实现的责任。)

To support this case where the layout is not known because there is no awareness
of modifiers, a special ``DRM_FORMAT_MOD_INVALID`` token has been defined. This
pseudo-modifier declares that the layout is not known, and that the driver
should use its own logic to determine what the underlying layout may be.
(为了支持这种因不了解修饰符而不知道布局的情况，人们定义了一个特殊的 ``DRM_FORMAT_MOD_INVALID`` 令牌。这个伪修饰符声明布局未知，驱动应使用自己的逻辑来确定底层布局可能是什么。)

.. note::

  ``DRM_FORMAT_MOD_INVALID`` is a non-zero value. The modifier value zero is
  ``DRM_FORMAT_MOD_LINEAR``, which is an explicit guarantee that the image
  has the linear layout. Care and attention should be taken to ensure that
  zero as a default value is not mixed up with either no modifier or the linear
  modifier. Also note that in some APIs the invalid modifier value is specified
  with an out-of-band flag, like in ``DRM_IOCTL_MODE_ADDFB2``.
  (注意：``DRM_FORMAT_MOD_INVALID`` 是一个非零值。修饰符值零是 ``DRM_FORMAT_MOD_LINEAR``，它是对图像具有线性布局的显式保证。应当小心，不要把作为默认值的零与"没有修饰符"或"线性修饰符"混淆。另请注意，在某些 API 中，无效修饰符值是用带外旗标指定的，例如 ``DRM_IOCTL_MODE_ADDFB2``。)

There are four cases where this token may be used:
  - during enumeration, an interface may return ``DRM_FORMAT_MOD_INVALID``, either
    as the sole member of a modifier list to declare that explicit modifiers are
    not supported, or as part of a larger list to declare that implicit modifiers
    may be used
  - during allocation, a user may supply ``DRM_FORMAT_MOD_INVALID``, either as the
    sole member of a modifier list (equivalent to not supplying a modifier list
    at all) to declare that explicit modifiers are not supported and must not be
    used, or as part of a larger list to declare that an allocation using implicit
    modifiers is acceptable
  - in a post-allocation query, an implementation may return
    ``DRM_FORMAT_MOD_INVALID`` as the modifier of the allocated buffer to declare
    that the underlying layout is implementation-defined and that an explicit
    modifier description is not available; per the above rules, this may only be
    returned when the user has included ``DRM_FORMAT_MOD_INVALID`` as part of the
    list of acceptable modifiers, or not provided a list
  - when importing a buffer, the user may supply ``DRM_FORMAT_MOD_INVALID`` as the
    buffer modifier (or not supply a modifier) to indicate that the modifier is
    unknown for whatever reason; this is only acceptable when the buffer has
    not been allocated with an explicit modifier

 (有四种情形可以使用该令牌：)
 (在枚举期间，接口可以返回 ``DRM_FORMAT_MOD_INVALID``，或者作为修饰符列表的唯一成员，声明不支持显式修饰符；或者作为较大列表的一部分，声明可以使用隐式修饰符。)
 (在分配期间，用户可以提供 ``DRM_FORMAT_MOD_INVALID``，或者作为修饰符列表的唯一成员(等价于完全不提供修饰符列表)，声明不支持也不得使用显式修饰符；或者作为较大列表的一部分，声明接受使用隐式修饰符的分配。)
 (在分配后的查询中，实现可以返回 ``DRM_FORMAT_MOD_INVALID`` 作为已分配缓冲区的修饰符，声明底层布局由实现定义、没有可用的显式修饰符描述；按上述规则，只有当用户把 ``DRM_FORMAT_MOD_INVALID`` 包含在可接受修饰符列表中、或未提供列表时，才允许这样返回。)
 (在导入缓冲区时，用户可以提供 ``DRM_FORMAT_MOD_INVALID`` 作为缓冲区修饰符(或不提供修饰符)，表示修饰符因故未知；这仅在缓冲区不是以显式修饰符分配时才可接受。)

It follows from this that for any single buffer, the complete chain of operations
formed by the producer and all the consumers must be either fully implicit or fully
explicit. For example, if a user wishes to allocate a buffer for use between
GPU, display, and media, but the media API does not support modifiers, then the
user **must not** allocate the buffer with explicit modifiers and attempt to
import the buffer into the media API with no modifier, but either perform the
allocation using implicit modifiers, or allocate the buffer for media use
separately and copy between the two buffers.
(由此可知：对任何一个缓冲区，由生产者与所有消费者构成的完整操作链，要么完全隐式、要么完全显式。例如，如果用户想分配一个在 GPU、显示与媒体之间使用的缓冲区，但媒体 API 不支持修饰符，那么用户 **不得** 用显式修饰符分配缓冲区、再不带修饰符把它导入媒体 API，而应要么使用隐式修饰符进行分配，要么为媒体用途单独分配缓冲区并在两个缓冲区之间复制。)

As one exception to the above, allocations may be 'upgraded' from implicit
to explicit modifiers. For example, if the buffer is allocated with
``gbm_bo_create`` (taking no modifiers), the user may then query the modifier with
``gbm_bo_get_modifier`` and then use this modifier as an explicit modifier token
if a valid modifier is returned.
(作为上述规则的一个例外，分配可以从隐式"升级"为显式修饰符。例如，如果缓冲区用 ``gbm_bo_create``(不带修饰符)分配，用户随后可以用 ``gbm_bo_get_modifier`` 查询修饰符，如果返回的是有效修饰符，就可以把它用作显式修饰符令牌。)

When allocating buffers for exchange between different users and modifiers are
not available, implementations are strongly encouraged to use
``DRM_FORMAT_MOD_LINEAR`` for their allocation, as this is the universal baseline
for exchange. However, it is not guaranteed that this will result in the correct
interpretation of buffer content, as implicit modifier operation may still be
subject to driver-specific heuristics.
(当在不同使用者之间交换缓冲区而修饰符不可用时，强烈建议实现使用 ``DRM_FORMAT_MOD_LINEAR`` 进行分配，因为它是交换的通用基线。然而，不能保证这会带来对缓冲区内容的正确解读，因为隐式修饰符的行为仍可能受驱动特定启发式规则的影响。)

Any new users - userspace programs and protocols, kernel subsystems, etc -
wishing to exchange buffers must offer interoperability through dma-buf file
descriptors for memory planes, DRM format tokens to describe the format, DRM
format modifiers to describe the layout in memory, at least width and height for
dimensions, and at least offset and stride for each memory plane.
(任何希望交换缓冲区的新使用者——用户空间程序与协议、内核子系统等——都必须提供互操作性：以 dma-buf 文件描述符承载内存平面、以 DRM 格式令牌描述格式、以 DRM 格式修饰符描述内存布局、至少以宽度和高度描述尺寸，以及为每个内存平面至少提供 offset 与 stride。)

.. _zwp_linux_dmabuf_v1: https://gitlab.freedesktop.org/wayland/wayland-protocols/-/blob/main/unstable/linux-dmabuf/linux-dmabuf-unstable-v1.xml
.. _VK_EXT_image_drm_format_modifier: https://registry.khronos.org/vulkan/specs/1.3-extensions/man/html/VK_EXT_image_drm_format_modifier.html
.. _EGL_EXT_image_dma_buf_import_modifiers: https://registry.khronos.org/EGL/extensions/EXT/EGL_EXT_image_dma_buf_import_modifiers.txt
