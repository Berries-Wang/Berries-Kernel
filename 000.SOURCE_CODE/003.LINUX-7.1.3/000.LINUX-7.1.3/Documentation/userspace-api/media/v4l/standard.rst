.. SPDX-License-Identifier: GFDL-1.1-no-invariants-or-later
.. c:namespace:: V4L

.. _standard:

*************************
Video Standards(视频标准)
*************************

Video devices typically support one or more different video standards or
variations of standards. Each video input and output may support another
set of standards. This set is reported by the ``std`` field of struct
:c:type:`v4l2_input` and struct
:c:type:`v4l2_output` returned by the
:ref:`VIDIOC_ENUMINPUT` and
:ref:`VIDIOC_ENUMOUTPUT` ioctls, respectively.
(视频设备通常支持一种或多种不同的视频标准或标准的变体。每个视频输入和输出可能支持另一组标准。该组标准由 :ref:`VIDIOC_ENUMINPUT` 与 :ref:`VIDIOC_ENUMOUTPUT` ioctl 分别返回的结构体 :c:type:`v4l2_input` 与 :c:type:`v4l2_output` 的 ``std`` 字段报告。)

V4L2 defines one bit for each analog video standard currently in use
worldwide, and sets aside bits for driver defined standards, e. g.
hybrid standards to watch NTSC video tapes on PAL TVs and vice versa.
Applications can use the predefined bits to select a particular
standard, although presenting the user a menu of supported standards is
preferred. To enumerate and query the attributes of the supported
standards applications use the :ref:`VIDIOC_ENUMSTD`
ioctl.
(V4L2 为目前全球在用的每种模拟视频标准定义一个位，并为驱动自定义的标准保留位，例如在 PAL 电视上观看 NTSC 录像带(或反之)的混合标准。应用可以使用预定义的位来选择特定标准，不过更推荐的做法是向用户展示一个受支持标准的菜单。要枚举并查询受支持标准的属性，应用使用 :ref:`VIDIOC_ENUMSTD` ioctl。)

Many of the defined standards are actually just variations of a few
major standards. The hardware may in fact not distinguish between them,
or do so internal and switch automatically. Therefore enumerated
standards also contain sets of one or more standard bits.
(许多已定义的标准实际上只是少数几个主要标准的变体。硬件实际上可能并不区分它们，或在内部区分并自动切换。因此，枚举出的标准也包含一个或多个标准位的集合。)

Assume a hypothetic tuner capable of demodulating B/PAL, G/PAL and I/PAL
signals. The first enumerated standard is a set of B and G/PAL, switched
automatically depending on the selected radio frequency in UHF or VHF
band. Enumeration gives a "PAL-B/G" or "PAL-I" choice. Similar a
Composite input may collapse standards, enumerating "PAL-B/G/H/I",
"NTSC-M" and "SECAM-D/K". [#f1]_
(假设有一个能够解调 B/PAL、G/PAL 和 I/PAL 信号的假想调谐器。第一个枚举出的标准是 B 和 G/PAL 的集合，根据所选 UHF 或 VHF 频段中的无线电频率自动切换。枚举会给出 "PAL-B/G" 或 "PAL-I" 的选择。类似地，Composite 输入可能合并标准，枚举出 "PAL-B/G/H/I"、"NTSC-M" 和 "SECAM-D/K"。)

To query and select the standard used by the current video input or
output applications call the :ref:`VIDIOC_G_STD <VIDIOC_G_STD>` and
:ref:`VIDIOC_S_STD <VIDIOC_G_STD>` ioctl, respectively. The
*received* standard can be sensed with the
:ref:`VIDIOC_QUERYSTD` ioctl.
(要查询并选择当前视频输入或输出使用的标准，应用分别调用 :ref:`VIDIOC_G_STD <VIDIOC_G_STD>` 与 :ref:`VIDIOC_S_STD <VIDIOC_G_STD>` ioctl。*接收到的*标准可以用 :ref:`VIDIOC_QUERYSTD` ioctl 感知。)

.. note::

   The parameter of all these ioctls is a pointer to a
   :ref:`v4l2_std_id <v4l2-std-id>` type (a standard set), *not* an
   index into the standard enumeration. Drivers must implement all video
   standard ioctls when the device has one or more video inputs or outputs.
   (注意：所有这些 ioctl 的参数都是指向 :ref:`v4l2_std_id <v4l2-std-id>` 类型(一个标准集合)的指针，*而不是*标准枚举中的索引。当设备有一个或多个视频输入或输出时，驱动必须实现所有视频标准 ioctl。)

Special rules apply to devices such as USB cameras where the notion of
video standards makes little sense. More generally for any capture or
output device which is:
(对于 USB 摄像头这类视频标准概念几乎无意义的设备，适用特殊规则。更一般地，对任何满足以下条件的采集或输出设备：)

-  incapable of capturing fields or frames at the nominal rate of the
   video standard, or
   (无法按视频标准的标称速率采集场或帧，或)

-  that does not support the video standard formats at all.
   (完全不支持视频标准格式。)

Here the driver shall set the ``std`` field of struct
:c:type:`v4l2_input` and struct
:c:type:`v4l2_output` to zero and the :ref:`VIDIOC_G_STD <VIDIOC_G_STD>`,
:ref:`VIDIOC_S_STD <VIDIOC_G_STD>`, :ref:`VIDIOC_QUERYSTD` and :ref:`VIDIOC_ENUMSTD` ioctls
shall return the ``ENOTTY`` error code or the ``EINVAL`` error code.
(此时驱动应把结构体 :c:type:`v4l2_input` 与 :c:type:`v4l2_output` 的 ``std`` 字段设为零，且 :ref:`VIDIOC_G_STD <VIDIOC_G_STD>`、:ref:`VIDIOC_S_STD <VIDIOC_G_STD>`、:ref:`VIDIOC_QUERYSTD` 与 :ref:`VIDIOC_ENUMSTD` ioctl 应返回 ``ENOTTY`` 或 ``EINVAL`` 错误码。)

Applications can make use of the :ref:`input-capabilities` and
:ref:`output-capabilities` flags to determine whether the video
standard ioctls can be used with the given input or output.
(应用可以利用 :ref:`input-capabilities` 与 :ref:`output-capabilities` 标志来判断视频标准 ioctl 能否用于给定的输入或输出。)

Example: Information about the current video standard(示例：当前视频标准的信息)
===============================================================================

.. code-block:: c

    v4l2_std_id std_id;
    struct v4l2_standard standard;

    if (-1 == ioctl(fd, VIDIOC_G_STD, &std_id)) {
	/* Note when VIDIOC_ENUMSTD always returns ENOTTY this
	   is no video device or it falls under the USB exception,
	   and VIDIOC_G_STD returning ENOTTY is no error. */

	perror("VIDIOC_G_STD");
	exit(EXIT_FAILURE);
    }

    memset(&standard, 0, sizeof(standard));
    standard.index = 0;

    while (0 == ioctl(fd, VIDIOC_ENUMSTD, &standard)) {
	if (standard.id & std_id) {
	       printf("Current video standard: %s\\n", standard.name);
	       exit(EXIT_SUCCESS);
	}

	standard.index++;
    }

    /* EINVAL indicates the end of the enumeration, which cannot be
       empty unless this device falls under the USB exception. */

    if (errno == EINVAL || standard.index == 0) {
	perror("VIDIOC_ENUMSTD");
	exit(EXIT_FAILURE);
    }

Example: Listing the video standards supported by the current input(示例：列出当前输入支持的视频标准)
=====================================================================================================

.. code-block:: c

    struct v4l2_input input;
    struct v4l2_standard standard;

    memset(&input, 0, sizeof(input));

    if (-1 == ioctl(fd, VIDIOC_G_INPUT, &input.index)) {
	perror("VIDIOC_G_INPUT");
	exit(EXIT_FAILURE);
    }

    if (-1 == ioctl(fd, VIDIOC_ENUMINPUT, &input)) {
	perror("VIDIOC_ENUM_INPUT");
	exit(EXIT_FAILURE);
    }

    printf("Current input %s supports:\\n", input.name);

    memset(&standard, 0, sizeof(standard));
    standard.index = 0;

    while (0 == ioctl(fd, VIDIOC_ENUMSTD, &standard)) {
	if (standard.id & input.std)
	    printf("%s\\n", standard.name);

	standard.index++;
    }

    /* EINVAL indicates the end of the enumeration, which cannot be
       empty unless this device falls under the USB exception. */

    if (errno != EINVAL || standard.index == 0) {
	perror("VIDIOC_ENUMSTD");
	exit(EXIT_FAILURE);
    }

Example: Selecting a new video standard(示例：选择新的视频标准)
===============================================================

.. code-block:: c

    struct v4l2_input input;
    v4l2_std_id std_id;

    memset(&input, 0, sizeof(input));

    if (-1 == ioctl(fd, VIDIOC_G_INPUT, &input.index)) {
	perror("VIDIOC_G_INPUT");
	exit(EXIT_FAILURE);
    }

    if (-1 == ioctl(fd, VIDIOC_ENUMINPUT, &input)) {
	perror("VIDIOC_ENUM_INPUT");
	exit(EXIT_FAILURE);
    }

    if (0 == (input.std & V4L2_STD_PAL_BG)) {
	fprintf(stderr, "Oops. B/G PAL is not supported.\\n");
	exit(EXIT_FAILURE);
    }

    /* Note this is also supposed to work when only B
       or G/PAL is supported. */

    std_id = V4L2_STD_PAL_BG;

    if (-1 == ioctl(fd, VIDIOC_S_STD, &std_id)) {
	perror("VIDIOC_S_STD");
	exit(EXIT_FAILURE);
    }

.. [#f1]
   Some users are already confused by technical terms PAL, NTSC and
   SECAM. There is no point asking them to distinguish between B, G, D,
   or K when the software or hardware can do that automatically.
   (有些用户已经被 PAL、NTSC 和 SECAM 这些技术术语搞糊涂了。既然软件或硬件能够自动区分，再让他们去分辨 B、G、D 或 K 就毫无意义。)
