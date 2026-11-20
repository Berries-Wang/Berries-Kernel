.. SPDX-License-Identifier: GFDL-1.1-no-invariants-or-later
.. c:namespace:: V4L

.. _crop:

*************************************************************************************
Image Cropping, Insertion and Scaling -- the CROP API(图像裁剪、插入与缩放——CROP API)
*************************************************************************************

.. note::

   The CROP API is mostly superseded by the newer :ref:`SELECTION API
   <selection-api>`. The new API should be preferred in most cases,
   with the exception of pixel aspect ratio detection, which is
   implemented by :ref:`VIDIOC_CROPCAP <VIDIOC_CROPCAP>` and has no
   equivalent in the SELECTION API. See :ref:`selection-vs-crop` for a
   comparison of the two APIs.
   (注意： CROP API 在很大程度上已被较新的 :ref:`SELECTION API <selection-api>` 取代。大多数情况下应优先使用新 API，例外是像素宽高比检测——它由 :ref:`VIDIOC_CROPCAP <VIDIOC_CROPCAP>` 实现，在 SELECTION API 中没有等价物。两种 API 的对比参见 :ref:`selection-vs-crop`。)

Some video capture devices can sample a subsection of the picture and
shrink or enlarge it to an image of arbitrary size. We call these
abilities cropping and scaling. Some video output devices can scale an
image up or down and insert it at an arbitrary scan line and horizontal
offset into a video signal.
(一些视频采集设备可以对画面的一小块区域采样，并把它缩小或放大为任意尺寸的图像。我们把这些能力称为裁剪（cropping）与缩放（scaling）。一些视频输出设备可以把图像放大或缩小，并以任意的扫描行和水平偏移插入到视频信号中。)

Applications can use the following API to select an area in the video
signal, query the default area and the hardware limits.
(应用程序可以使用下面的 API 在视频信号中选择一个区域，并查询默认区域与硬件限制。)

.. note::

   Despite their name, the :ref:`VIDIOC_CROPCAP <VIDIOC_CROPCAP>`,
   :ref:`VIDIOC_G_CROP <VIDIOC_G_CROP>` and :ref:`VIDIOC_S_CROP
   <VIDIOC_G_CROP>` ioctls apply to input as well as output devices.
   (注意： 尽管名字如此，:ref:`VIDIOC_CROPCAP <VIDIOC_CROPCAP>`、:ref:`VIDIOC_G_CROP <VIDIOC_G_CROP>` 与 :ref:`VIDIOC_S_CROP <VIDIOC_G_CROP>` 这些 ioctl 既适用于输入设备，也适用于输出设备。)

Scaling requires a source and a target. On a video capture or overlay
device the source is the video signal, and the cropping ioctls determine
the area actually sampled. The target are images read by the application
or overlaid onto the graphics screen. Their size (and position for an
overlay) is negotiated with the :ref:`VIDIOC_G_FMT <VIDIOC_G_FMT>`
and :ref:`VIDIOC_S_FMT <VIDIOC_G_FMT>` ioctls.
(缩放需要源与目标。在视频采集或覆盖设备上，源是视频信号，裁剪 ioctl 决定实际采样的区域。目标是应用程序读取的图像或覆盖到图形屏幕上的图像。它们的大小（覆盖时还包括位置）通过与 :ref:`VIDIOC_G_FMT <VIDIOC_G_FMT>` 和 :ref:`VIDIOC_S_FMT <VIDIOC_G_FMT>` ioctl 协商确定。)

On a video output device the source are the images passed in by the
application, and their size is again negotiated with the
:ref:`VIDIOC_G_FMT <VIDIOC_G_FMT>` and :ref:`VIDIOC_S_FMT <VIDIOC_G_FMT>`
ioctls, or may be encoded in a compressed video stream. The target is
the video signal, and the cropping ioctls determine the area where the
images are inserted.
(在视频输出设备上，源是应用程序传入的图像，其大小同样通过与 :ref:`VIDIOC_G_FMT <VIDIOC_G_FMT>` 和 :ref:`VIDIOC_S_FMT <VIDIOC_G_FMT>` ioctl 协商确定，也可能编码在压缩视频流中。目标是视频信号，裁剪 ioctl 决定图像插入的区域。)

Source and target rectangles are defined even if the device does not
support scaling or the :ref:`VIDIOC_G_CROP <VIDIOC_G_CROP>` and
:ref:`VIDIOC_S_CROP <VIDIOC_G_CROP>` ioctls. Their size (and position
where applicable) will be fixed in this case.
(即使设备不支持缩放或 :ref:`VIDIOC_G_CROP <VIDIOC_G_CROP>` 与 :ref:`VIDIOC_S_CROP <VIDIOC_G_CROP>` ioctl，源矩形与目标矩形也有定义。这种情况下它们的大小（以及适用时的位置）是固定的。)

.. note::

   All capture and output devices that support the CROP or SELECTION
   API will also support the :ref:`VIDIOC_CROPCAP <VIDIOC_CROPCAP>`
   ioctl.
   (注意： 所有支持 CROP 或 SELECTION API 的采集与输出设备也都会支持 :ref:`VIDIOC_CROPCAP <VIDIOC_CROPCAP>` ioctl。)

Cropping Structures(裁剪结构体)
===============================


.. _crop-scale:

.. kernel-figure:: crop.svg
    :alt:    crop.svg
    :align:  center

    Image Cropping, Insertion and Scaling

    The cropping, insertion and scaling process



For capture devices the coordinates of the top left corner, width and
height of the area which can be sampled is given by the ``bounds``
substructure of the struct :c:type:`v4l2_cropcap` returned
by the :ref:`VIDIOC_CROPCAP <VIDIOC_CROPCAP>` ioctl. To support a wide
range of hardware this specification does not define an origin or units.
However by convention drivers should horizontally count unscaled samples
relative to 0H (the leading edge of the horizontal sync pulse, see
:ref:`vbi-hsync`). Vertically ITU-R line numbers of the first field
(see ITU R-525 line numbering for :ref:`525 lines <vbi-525>` and for
:ref:`625 lines <vbi-625>`), multiplied by two if the driver
can capture both fields.
(对采集设备而言，可采样区域左上角坐标、宽度与高度由 :ref:`VIDIOC_CROPCAP <VIDIOC_CROPCAP>` ioctl 返回的 struct :c:type:`v4l2_cropcap` 的 ``bounds`` 子结构给出。为了支持广泛的硬件，本规范不定义原点或单位。但按照惯例，驱动应以 0H（水平同步脉冲的前沿，参见 :ref:`vbi-hsync`）为基准水平地统计未缩放的采样点。垂直方向上采用第一场的 ITU-R 行号（ITU R-525 行号定义见 :ref:`525 lines <vbi-525>` 与 :ref:`625 lines <vbi-625>`），如果驱动能同时采集两场则乘以 2。)

The top left corner, width and height of the source rectangle, that is
the area actually sampled, is given by struct
:c:type:`v4l2_crop` using the same coordinate system as
struct :c:type:`v4l2_cropcap`. Applications can use the
:ref:`VIDIOC_G_CROP <VIDIOC_G_CROP>` and :ref:`VIDIOC_S_CROP <VIDIOC_G_CROP>`
ioctls to get and set this rectangle. It must lie completely within the
capture boundaries and the driver may further adjust the requested size
and/or position according to hardware limitations.
(源矩形（即实际采样区域）的左上角坐标、宽度与高度由 struct :c:type:`v4l2_crop` 给出，使用与 struct :c:type:`v4l2_cropcap` 相同的坐标系。应用程序可以用 :ref:`VIDIOC_G_CROP <VIDIOC_G_CROP>` 与 :ref:`VIDIOC_S_CROP <VIDIOC_G_CROP>` ioctl 获取和设置该矩形。它必须完全位于采集边界之内，驱动还可以根据硬件限制进一步调整请求的大小和/或位置。)

Each capture device has a default source rectangle, given by the
``defrect`` substructure of struct
:c:type:`v4l2_cropcap`. The center of this rectangle
shall align with the center of the active picture area of the video
signal, and cover what the driver writer considers the complete picture.
Drivers shall reset the source rectangle to the default when the driver
is first loaded, but not later.
(每个采集设备都有一个默认源矩形，由 struct :c:type:`v4l2_cropcap` 的 ``defrect`` 子结构给出。该矩形的中心应与视频信号活动画面区域的中心对齐，并覆盖驱动作者所认为的完整画面。驱动在首次加载时应把源矩形重置为默认值，此后则不再重置。)

For output devices these structures and ioctls are used accordingly,
defining the *target* rectangle where the images will be inserted into
the video signal.
(对输出设备，这些结构与 ioctl 用法相应变化，用于定义图像将被插入视频信号的*目标*矩形。)


Scaling Adjustments(缩放调整)
=============================

Video hardware can have various cropping, insertion and scaling
limitations. It may only scale up or down, support only discrete scaling
factors, or have different scaling abilities in horizontal and vertical
direction. Also it may not support scaling at all. At the same time the
struct :c:type:`v4l2_crop` rectangle may have to be aligned,
and both the source and target rectangles may have arbitrary upper and
lower size limits. In particular the maximum ``width`` and ``height`` in
struct :c:type:`v4l2_crop` may be smaller than the struct
:c:type:`v4l2_cropcap`. ``bounds`` area. Therefore, as
usual, drivers are expected to adjust the requested parameters and
return the actual values selected.
(视频硬件可能有各种裁剪、插入与缩放限制。它可能只能放大或只能缩小、只支持离散的缩放因子，或横竖方向的缩放能力不同；也可能完全不支持缩放。同时 struct :c:type:`v4l2_crop` 矩形可能必须对齐，源矩形与目标矩形还可能有任意的大小上下限。特别是 struct :c:type:`v4l2_crop` 中的最大 ``width`` 与 ``height`` 可能小于 struct :c:type:`v4l2_cropcap` 的 ``bounds`` 区域。因此，照例驱动应调整请求的参数并返回实际选定的值。)

Applications can change the source or the target rectangle first, as
they may prefer a particular image size or a certain area in the video
signal. If the driver has to adjust both to satisfy hardware
limitations, the last requested rectangle shall take priority, and the
driver should preferably adjust the opposite one. The
:ref:`VIDIOC_TRY_FMT <VIDIOC_G_FMT>` ioctl however shall not change
the driver state and therefore only adjust the requested rectangle.
(应用程序可以先修改源矩形或目标矩形，因为它们可能更偏好特定的图像尺寸或视频信号中的特定区域。如果驱动为满足硬件限制而必须同时调整两者，则最后请求的矩形优先，驱动应尽量调整另一个。而 :ref:`VIDIOC_TRY_FMT <VIDIOC_G_FMT>` ioctl 不应改变驱动状态，因此只调整所请求的矩形。)

Suppose scaling on a video capture device is restricted to a factor 1:1
or 2:1 in either direction and the target image size must be a multiple
of 16 × 16 pixels. The source cropping rectangle is set to defaults,
which are also the upper limit in this example, of 640 × 400 pixels at
offset 0, 0. An application requests an image size of 300 × 225 pixels,
assuming video will be scaled down from the "full picture" accordingly.
The driver sets the image size to the closest possible values 304 × 224,
then chooses the cropping rectangle closest to the requested size, that
is 608 × 224 (224 × 2:1 would exceed the limit 400). The offset 0, 0 is
still valid, thus unmodified. Given the default cropping rectangle
reported by :ref:`VIDIOC_CROPCAP <VIDIOC_CROPCAP>` the application can
easily propose another offset to center the cropping rectangle.
(假设某视频采集设备的缩放被限制为任一方向 1:1 或 2:1，且目标图像尺寸必须是 16 × 16 像素的倍数。源裁剪矩形被设为默认值——本例中同时也是上限——即偏移 0, 0 处的 640 × 400 像素。应用程序请求 300 × 225 像素的图像尺寸，假设视频会相应地从"完整画面"缩小。驱动把图像尺寸设为最接近的可取值 304 × 224，然后选择最接近请求尺寸的裁剪矩形，即 608 × 224（224 × 2:1 会超过 400 的上限）。偏移 0, 0 仍然有效，因此未修改。根据 :ref:`VIDIOC_CROPCAP <VIDIOC_CROPCAP>` 报告的默认裁剪矩形，应用程序可以很容易地提出另一个偏移，使裁剪矩形居中。)

Now the application may insist on covering an area using a picture
aspect ratio closer to the original request, so it asks for a cropping
rectangle of 608 × 456 pixels. The present scaling factors limit
cropping to 640 × 384, so the driver returns the cropping size 608 × 384
and adjusts the image size to closest possible 304 × 192.
(现在应用程序可能坚持要用更接近最初请求的画面宽高比来覆盖一个区域，于是请求 608 × 456 像素的裁剪矩形。当前的缩放因子把裁剪限制在 640 × 384，因此驱动返回裁剪尺寸 608 × 384，并把图像尺寸调整为最接近的 304 × 192。)


Examples(示例)
==============

Source and target rectangles shall remain unchanged across closing and
reopening a device, such that piping data into or out of a device will
work without special preparations. More advanced applications should
ensure the parameters are suitable before starting I/O.
(关闭并重新打开设备后，源矩形与目标矩形应保持不变，这样向设备灌入或从设备取出数据无需特殊准备即可工作。更完善的应用程序应在开始 I/O 之前确保参数合适。)

.. note::

   On the next two examples, a video capture device is assumed;
   change ``V4L2_BUF_TYPE_VIDEO_CAPTURE`` for other types of device.
   (注意： 下面两个示例假定是视频采集设备；对其他类型的设备，请相应修改 ``V4L2_BUF_TYPE_VIDEO_CAPTURE``。)

Example: Resetting the cropping parameters(示例：重置裁剪参数)
==============================================================

.. code-block:: c

    struct v4l2_cropcap cropcap;
    struct v4l2_crop crop;

    memset (&cropcap, 0, sizeof (cropcap));
    cropcap.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;

    if (-1 == ioctl (fd, VIDIOC_CROPCAP, &cropcap)) {
	perror ("VIDIOC_CROPCAP");
	exit (EXIT_FAILURE);
    }

    memset (&crop, 0, sizeof (crop));
    crop.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
    crop.c = cropcap.defrect;

    /* Ignore if cropping is not supported (EINVAL). */

    if (-1 == ioctl (fd, VIDIOC_S_CROP, &crop)
	&& errno != EINVAL) {
	perror ("VIDIOC_S_CROP");
	exit (EXIT_FAILURE);
    }


Example: Simple downscaling(示例：简单缩小)
===========================================

.. code-block:: c

    struct v4l2_cropcap cropcap;
    struct v4l2_format format;

    reset_cropping_parameters ();

    /* Scale down to 1/4 size of full picture. */

    memset (&format, 0, sizeof (format)); /* defaults */

    format.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;

    format.fmt.pix.width = cropcap.defrect.width >> 1;
    format.fmt.pix.height = cropcap.defrect.height >> 1;
    format.fmt.pix.pixelformat = V4L2_PIX_FMT_YUYV;

    if (-1 == ioctl (fd, VIDIOC_S_FMT, &format)) {
	perror ("VIDIOC_S_FORMAT");
	exit (EXIT_FAILURE);
    }

    /* We could check the actual image size now, the actual scaling factor
       or if the driver can scale at all. */

Example: Selecting an output area(示例：选择输出区域)
=====================================================

.. note:: This example assumes an output device.
         (注意： 本例假定是一个输出设备。)

.. code-block:: c

    struct v4l2_cropcap cropcap;
    struct v4l2_crop crop;

    memset (&cropcap, 0, sizeof (cropcap));
    cropcap.type = V4L2_BUF_TYPE_VIDEO_OUTPUT;

    if (-1 == ioctl (fd, VIDIOC_CROPCAP;, &cropcap)) {
	perror ("VIDIOC_CROPCAP");
	exit (EXIT_FAILURE);
    }

    memset (&crop, 0, sizeof (crop));

    crop.type = V4L2_BUF_TYPE_VIDEO_OUTPUT;
    crop.c = cropcap.defrect;

    /* Scale the width and height to 50 % of their original size
       and center the output. */

    crop.c.width /= 2;
    crop.c.height /= 2;
    crop.c.left += crop.c.width / 2;
    crop.c.top += crop.c.height / 2;

    /* Ignore if cropping is not supported (EINVAL). */

    if (-1 == ioctl (fd, VIDIOC_S_CROP, &crop)
	&& errno != EINVAL) {
	perror ("VIDIOC_S_CROP");
	exit (EXIT_FAILURE);
    }

Example: Current scaling factor and pixel aspect(示例：当前缩放因子与像素宽高比)
================================================================================

.. note:: This example assumes a video capture device.
         (注意： 本例假定是一个视频采集设备。)

.. code-block:: c

    struct v4l2_cropcap cropcap;
    struct v4l2_crop crop;
    struct v4l2_format format;
    double hscale, vscale;
    double aspect;
    int dwidth, dheight;

    memset (&cropcap, 0, sizeof (cropcap));
    cropcap.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;

    if (-1 == ioctl (fd, VIDIOC_CROPCAP, &cropcap)) {
	perror ("VIDIOC_CROPCAP");
	exit (EXIT_FAILURE);
    }

    memset (&crop, 0, sizeof (crop));
    crop.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;

    if (-1 == ioctl (fd, VIDIOC_G_CROP, &crop)) {
	if (errno != EINVAL) {
	    perror ("VIDIOC_G_CROP");
	    exit (EXIT_FAILURE);
	}

	/* Cropping not supported. */
	crop.c = cropcap.defrect;
    }

    memset (&format, 0, sizeof (format));
    format.fmt.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;

    if (-1 == ioctl (fd, VIDIOC_G_FMT, &format)) {
	perror ("VIDIOC_G_FMT");
	exit (EXIT_FAILURE);
    }

    /* The scaling applied by the driver. */

    hscale = format.fmt.pix.width / (double) crop.c.width;
    vscale = format.fmt.pix.height / (double) crop.c.height;

    aspect = cropcap.pixelaspect.numerator /
	 (double) cropcap.pixelaspect.denominator;
    aspect = aspect * hscale / vscale;

    /* Devices following ITU-R BT.601 do not capture
       square pixels. For playback on a computer monitor
       we should scale the images to this size. */

    dwidth = format.fmt.pix.width / aspect;
    dheight = format.fmt.pix.height;
