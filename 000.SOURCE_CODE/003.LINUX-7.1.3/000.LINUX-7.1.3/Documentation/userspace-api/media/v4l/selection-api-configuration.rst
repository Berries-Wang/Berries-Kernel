.. SPDX-License-Identifier: GFDL-1.1-no-invariants-or-later
.. c:namespace:: V4L

*******************
Configuration(配置)
*******************

Applications can use the :ref:`selection API <VIDIOC_G_SELECTION>` to
select an area in a video signal or a buffer, and to query for default
settings and hardware limits.
(应用程序可以使用 :ref:`selection API <VIDIOC_G_SELECTION>` 来选择视频信号或缓冲区中的一个区域，并查询默认设置和硬件限制。)

Video hardware can have various cropping, composing and scaling
limitations. It may only scale up or down, support only discrete scaling
factors, or have different scaling abilities in the horizontal and
vertical directions. Also it may not support scaling at all. At the same
time the cropping/composing rectangles may have to be aligned, and both
the source and the sink may have arbitrary upper and lower size limits.
Therefore, as usual, drivers are expected to adjust the requested
parameters and return the actual values selected. An application can
control the rounding behaviour using
:ref:`constraint flags <v4l2-selection-flags>`.
(视频硬件可能存在各种裁剪、合成和缩放限制。它可能只能放大或缩小、仅支持离散的缩放因子，或者在水平和垂直方向上具有不同的缩放能力；也可能完全不支持缩放。同时，裁剪/合成矩形可能必须对齐，而且源端和接收端都可能具有任意的尺寸上下限。因此，与往常一样，驱动程序需要调整请求的参数并返回实际选定的值。应用程序可以使用 :ref:`constraint flags <v4l2-selection-flags>` 来控制舍入行为。)


Configuration of video capture(视频捕获的配置)
==============================================

See figure :ref:`sel-targets-capture` for examples of the selection
targets available for a video capture device. It is recommended to
configure the cropping targets before to the composing targets.
(有关视频捕获设备可用选择目标的示例，请参见图 :ref:`sel-targets-capture` 。建议先配置裁剪目标，再配置合成目标。)

The range of coordinates of the top left corner, width and height of
areas that can be sampled is given by the ``V4L2_SEL_TGT_CROP_BOUNDS``
target. It is recommended for the driver developers to put the top/left
corner at position ``(0,0)``. The rectangle's coordinates are expressed
in pixels.
(可采样区域的左上角坐标、宽度和高度的范围由 ``V4L2_SEL_TGT_CROP_BOUNDS`` 目标给出。建议驱动程序开发者将左上角置于 ``(0,0)`` 位置。矩形的坐标以像素表示。)

The top left corner, width and height of the source rectangle, that is
the area actually sampled, is given by the ``V4L2_SEL_TGT_CROP`` target.
It uses the same coordinate system as ``V4L2_SEL_TGT_CROP_BOUNDS``. The
active cropping area must lie completely inside the capture boundaries.
The driver may further adjust the requested size and/or position
according to hardware limitations.
(源矩形（即实际被采样的区域）的左上角坐标、宽度和高度由 ``V4L2_SEL_TGT_CROP`` 目标给出。它使用与 ``V4L2_SEL_TGT_CROP_BOUNDS`` 相同的坐标系。活动的裁剪区域必须完全位于捕获边界之内。驱动程序可以根据硬件限制进一步调整请求的尺寸和/或位置。)

Each capture device has a default source rectangle, given by the
``V4L2_SEL_TGT_CROP_DEFAULT`` target. This rectangle shall cover what the
driver writer considers the complete picture. Drivers shall set the
active crop rectangle to the default when the driver is first loaded,
but not later.
(每个捕获设备都有一个默认的源矩形，由 ``V4L2_SEL_TGT_CROP_DEFAULT`` 目标给出。该矩形应覆盖驱动程序编写者所认为的完整图像。驱动程序应在首次加载时将活动裁剪矩形设置为默认值，此后则不必再设置。)

The composing targets refer to a memory buffer. The limits of composing
coordinates are obtained using ``V4L2_SEL_TGT_COMPOSE_BOUNDS``. All
coordinates are expressed in pixels. The rectangle's top/left corner
must be located at position ``(0,0)``. The width and height are equal to
the image size set by :ref:`VIDIOC_S_FMT <VIDIOC_G_FMT>`.
(合成目标指向内存缓冲区。合成坐标的界限通过 ``V4L2_SEL_TGT_COMPOSE_BOUNDS`` 获得。所有坐标均以像素表示。矩形的左上角必须位于 ``(0,0)`` 位置。宽度和高度等于由 :ref:`VIDIOC_S_FMT <VIDIOC_G_FMT>` 设置的图像尺寸。)

The part of a buffer into which the image is inserted by the hardware is
controlled by the ``V4L2_SEL_TGT_COMPOSE`` target. The rectangle's
coordinates are also expressed in the same coordinate system as the
bounds rectangle. The composing rectangle must lie completely inside
bounds rectangle. The driver must adjust the composing rectangle to fit
to the bounding limits. Moreover, the driver can perform other
adjustments according to hardware limitations. The application can
control rounding behaviour using
:ref:`constraint flags <v4l2-selection-flags>`.
(缓冲区中由硬件插入图像的部分由 ``V4L2_SEL_TGT_COMPOSE`` 目标控制。矩形的坐标同样以与界限矩形相同的坐标系表示。合成矩形必须完全位于界限矩形之内。驱动程序必须调整合成矩形以符合界限限制。此外，驱动程序还可以根据硬件限制进行其他调整。应用程序可以使用 :ref:`constraint flags <v4l2-selection-flags>` 来控制舍入行为。)

For capture devices the default composing rectangle is queried using
``V4L2_SEL_TGT_COMPOSE_DEFAULT``. It is usually equal to the bounding
rectangle.
(对于捕获设备，默认合成矩形通过 ``V4L2_SEL_TGT_COMPOSE_DEFAULT`` 查询。它通常等于界限矩形。)

The part of a buffer that is modified by the hardware is given by
``V4L2_SEL_TGT_COMPOSE_PADDED``. It contains all pixels defined using
``V4L2_SEL_TGT_COMPOSE`` plus all padding data modified by hardware
during insertion process. All pixels outside this rectangle *must not*
be changed by the hardware. The content of pixels that lie inside the
padded area but outside active area is undefined. The application can
use the padded and active rectangles to detect where the rubbish pixels
are located and remove them if needed.
(缓冲区中被硬件修改的部分由 ``V4L2_SEL_TGT_COMPOSE_PADDED`` 给出。它包含使用 ``V4L2_SEL_TGT_COMPOSE`` 定义的所有像素，以及硬件在插入过程中修改的所有填充数据。位于此矩形之外的像素 *不得* 被硬件更改。位于填充区域内但活动区域之外的像素内容是未定义的。应用程序可以使用填充矩形和活动矩形来检测无效像素的位置，并在需要时将其移除。)


Configuration of video output(视频输出的配置)
=============================================

For output devices targets and ioctls are used similarly to the video
capture case. The *composing* rectangle refers to the insertion of an
image into a video signal. The cropping rectangles refer to a memory
buffer. It is recommended to configure the composing targets before to
the cropping targets.
(对于输出设备，目标和 ioctl 的使用方式与视频捕获情形类似。*合成* 矩形指将图像插入视频信号，而裁剪矩形指向内存缓冲区。建议先配置合成目标，再配置裁剪目标。)

The cropping targets refer to the memory buffer that contains an image
to be inserted into a video signal or graphical screen. The limits of
cropping coordinates are obtained using ``V4L2_SEL_TGT_CROP_BOUNDS``.
All coordinates are expressed in pixels. The top/left corner is always
point ``(0,0)``. The width and height is equal to the image size
specified using :ref:`VIDIOC_S_FMT <VIDIOC_G_FMT>` ioctl.
(裁剪目标指向包含待插入视频信号或图形屏幕的图像的内存缓冲区。裁剪坐标的界限通过 ``V4L2_SEL_TGT_CROP_BOUNDS`` 获得。所有坐标均以像素表示。左上角始终是点 ``(0,0)`` 。宽度和高度等于使用 :ref:`VIDIOC_S_FMT <VIDIOC_G_FMT>` ioctl 指定的图像尺寸。)

The top left corner, width and height of the source rectangle, that is
the area from which image date are processed by the hardware, is given
by the ``V4L2_SEL_TGT_CROP``. Its coordinates are expressed in the
same coordinate system as the bounds rectangle. The active cropping area
must lie completely inside the crop boundaries and the driver may
further adjust the requested size and/or position according to hardware
limitations.
(源矩形（即硬件从中处理图像数据的区域）的左上角坐标、宽度和高度由 ``V4L2_SEL_TGT_CROP`` 给出。其坐标以与界限矩形相同的坐标系表示。活动裁剪区域必须完全位于裁剪边界之内，驱动程序可以根据硬件限制进一步调整请求的尺寸和/或位置。)

For output devices the default cropping rectangle is queried using
``V4L2_SEL_TGT_CROP_DEFAULT``. It is usually equal to the bounding
rectangle.
(对于输出设备，默认裁剪矩形通过 ``V4L2_SEL_TGT_CROP_DEFAULT`` 查询。它通常等于界限矩形。)

The part of a video signal or graphics display where the image is
inserted by the hardware is controlled by ``V4L2_SEL_TGT_COMPOSE``
target. The rectangle's coordinates are expressed in pixels. The
composing rectangle must lie completely inside the bounds rectangle. The
driver must adjust the area to fit to the bounding limits. Moreover, the
driver can perform other adjustments according to hardware limitations.
(视频信号或图形显示中由硬件插入图像的部分由 ``V4L2_SEL_TGT_COMPOSE`` 目标控制。矩形的坐标以像素表示。合成矩形必须完全位于界限矩形之内。驱动程序必须调整该区域以符合界限限制。此外，驱动程序还可以根据硬件限制进行其他调整。)

The device has a default composing rectangle, given by the
``V4L2_SEL_TGT_COMPOSE_DEFAULT`` target. This rectangle shall cover what
the driver writer considers the complete picture. It is recommended for
the driver developers to put the top/left corner at position ``(0,0)``.
Drivers shall set the active composing rectangle to the default one when
the driver is first loaded.
(设备有一个默认的合成矩形，由 ``V4L2_SEL_TGT_COMPOSE_DEFAULT`` 目标给出。该矩形应覆盖驱动程序编写者所认为的完整图像。建议驱动程序开发者将左上角置于 ``(0,0)`` 位置。驱动程序应在首次加载时将活动合成矩形设置为默认值。)

The devices may introduce additional content to video signal other than
an image from memory buffers. It includes borders around an image.
However, such a padded area is driver-dependent feature not covered by
this document. Driver developers are encouraged to keep padded rectangle
equal to active one. The padded target is accessed by the
``V4L2_SEL_TGT_COMPOSE_PADDED`` identifier. It must contain all pixels
from the ``V4L2_SEL_TGT_COMPOSE`` target.
(设备可能向视频信号引入内存缓冲区图像之外的额外内容，包括图像周围的边框。不过，这种填充区域是驱动程序相关的特性，本文档不涉及。鼓励驱动程序开发者保持填充矩形与活动矩形一致。填充目标通过 ``V4L2_SEL_TGT_COMPOSE_PADDED`` 标识符访问。它必须包含 ``V4L2_SEL_TGT_COMPOSE`` 目标的全部像素。)


Scaling control(缩放控制)
=========================

An application can detect if scaling is performed by comparing the width
and the height of rectangles obtained using ``V4L2_SEL_TGT_CROP`` and
``V4L2_SEL_TGT_COMPOSE`` targets. If these are not equal then the
scaling is applied. The application can compute the scaling ratios using
these values.
(应用程序可以通过比较使用 ``V4L2_SEL_TGT_CROP`` 和 ``V4L2_SEL_TGT_COMPOSE`` 目标获得的矩形的宽度和高度，来检测是否进行了缩放。如果二者不相等，则表示应用了缩放。应用程序可以使用这些值计算缩放比例。)
