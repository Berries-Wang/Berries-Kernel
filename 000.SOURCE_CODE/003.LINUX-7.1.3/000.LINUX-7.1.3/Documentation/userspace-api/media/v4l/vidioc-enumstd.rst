.. SPDX-License-Identifier: GFDL-1.1-no-invariants-or-later
.. c:namespace:: V4L

.. _VIDIOC_ENUMSTD:

***********************************************************
ioctl VIDIOC_ENUMSTD, VIDIOC_SUBDEV_ENUMSTD(枚举支持的视频制式)
***********************************************************

Name(名称)
==========

VIDIOC_ENUMSTD - VIDIOC_SUBDEV_ENUMSTD - Enumerate supported video standards
(VIDIOC_ENUMSTD - VIDIOC_SUBDEV_ENUMSTD - 枚举支持的视频制式)

Synopsis(概要)
==============

.. c:macro:: VIDIOC_ENUMSTD

``int ioctl(int fd, VIDIOC_ENUMSTD, struct v4l2_standard *argp)``

.. c:macro:: VIDIOC_SUBDEV_ENUMSTD

``int ioctl(int fd, VIDIOC_SUBDEV_ENUMSTD, struct v4l2_standard *argp)``

Arguments(参数)
===============

``fd``
    File descriptor returned by :c:func:`open()`.
    (由 :c:func:`open()` 返回的文件描述符。)

``argp``
    Pointer to struct :c:type:`v4l2_standard`.
    (指向 struct :c:type:`v4l2_standard` 的指针。)

Description(描述)
=================

To query the attributes of a video standard, especially a custom (driver
defined) one, applications initialize the ``index`` field of struct
:c:type:`v4l2_standard` and call the :ref:`VIDIOC_ENUMSTD`
ioctl with a pointer to this structure. Drivers fill the rest of the
structure or return an ``EINVAL`` error code when the index is out of
bounds. To enumerate all standards applications shall begin at index
zero, incrementing by one until the driver returns ``EINVAL``. Drivers may
enumerate a different set of standards after switching the video input
or output. [#f1]_
(要查询视频制式的属性 (尤其是自定义的、由驱动定义的制式)，应用程序初始
化 struct :c:type:`v4l2_standard` 的 ``index`` 字段，并用指向该结构的指
针调用 :ref:`VIDIOC_ENUMSTD` ioctl。索引越界时，驱动填充结构的其余部分
或返回 ``EINVAL`` 错误码。要枚举所有制式，应用程序应从索引 0 开始，每次
加一，直到驱动返回 ``EINVAL``。切换视频输入或输出之后，驱动可能枚举出
不同的制式集合。)

.. c:type:: v4l2_standard

.. tabularcolumns:: |p{4.4cm}|p{4.4cm}|p{8.5cm}|

.. flat-table:: struct v4l2_standard
    :header-rows:  0
    :stub-columns: 0
    :widths:       1 1 2

    * - __u32
      - ``index``
      - Number of the video standard, set by the application.
	(视频制式的编号，由应用程序设置。)
    * - :ref:`v4l2_std_id <v4l2-std-id>`
      - ``id``
      - The bits in this field identify the standard as one of the common
	standards listed in :ref:`v4l2-std-id`, or if bits 32 to 63 are
	set as custom standards. Multiple bits can be set if the hardware
	does not distinguish between these standards, however separate
	indices do not indicate the opposite. The ``id`` must be unique.
	No other enumerated struct :c:type:`v4l2_standard` structure,
	for this input or output anyway, can contain the same set of bits.
	(此字段中的位将该制式标识为 :ref:`v4l2-std-id` 中列出的常见制式之
	一；如果第 32 至 63 位被置位，则表示自定义制式。如果硬件不区分这些
	制式，可以置多个位，但反之，分开的索引并不表示硬件可以区分。
	``id`` 必须唯一。就本输入或输出而言，其他任何已枚举的 struct
	:c:type:`v4l2_standard` 结构都不能包含相同的位集合。)
    * - __u8
      - ``name``\ [24]
      - Name of the standard, a NUL-terminated ASCII string, for example:
	"PAL-B/G", "NTSC Japan". This information is intended for the
	user.
	(制式的名称，以 NUL 结尾的 ASCII 字符串，例如 "PAL-B/G"、
	"NTSC Japan"。此信息供用户参考。)
    * - struct :c:type:`v4l2_fract`
      - ``frameperiod``
      - The frame period (not field period) is numerator / denominator.
	For example M/NTSC has a frame period of 1001 / 30000 seconds.
	(帧周期 (不是场周期) 为分子 / 分母。例如 M/NTSC 的帧周期为
	1001 / 30000 秒。)
    * - __u32
      - ``framelines``
      - Total lines per frame including blanking, e. g. 625 for B/PAL.
	(每帧总行数，包括消隐，例如 B/PAL 为 625。)
    * - __u32
      - ``reserved``\ [4]
      - Reserved for future extensions. Drivers must set the array to
	zero.
	(保留供未来扩展使用。驱动必须把该数组置零。)


.. c:type:: v4l2_fract

.. tabularcolumns:: |p{4.4cm}|p{4.4cm}|p{8.5cm}|

.. flat-table:: struct v4l2_fract
    :header-rows:  0
    :stub-columns: 0
    :widths:       1 1 2

    * - __u32
      - ``numerator``
      -
    * - __u32
      - ``denominator``
      -

.. tabularcolumns:: |p{4.4cm}|p{4.4cm}|p{8.5cm}|

.. _v4l2-std-id:

.. flat-table:: typedef v4l2_std_id
    :header-rows:  0
    :stub-columns: 0
    :widths:       1 1 2

    * - __u64
      - ``v4l2_std_id``
      - This type is a set, each bit representing another video standard
	as listed below and in :ref:`video-standards`. The 32 most
	significant bits are reserved for custom (driver defined) video
	standards.
	(此类型是一个集合，每个位代表另一种视频制式，如下文及
	:ref:`video-standards` 所列。最高 32 位保留给自定义 (驱动定义) 的视
	频制式。)


.. code-block:: c

    #define V4L2_STD_PAL_B          ((v4l2_std_id)0x00000001)
    #define V4L2_STD_PAL_B1         ((v4l2_std_id)0x00000002)
    #define V4L2_STD_PAL_G          ((v4l2_std_id)0x00000004)
    #define V4L2_STD_PAL_H          ((v4l2_std_id)0x00000008)
    #define V4L2_STD_PAL_I          ((v4l2_std_id)0x00000010)
    #define V4L2_STD_PAL_D          ((v4l2_std_id)0x00000020)
    #define V4L2_STD_PAL_D1         ((v4l2_std_id)0x00000040)
    #define V4L2_STD_PAL_K          ((v4l2_std_id)0x00000080)

    #define V4L2_STD_PAL_M          ((v4l2_std_id)0x00000100)
    #define V4L2_STD_PAL_N          ((v4l2_std_id)0x00000200)
    #define V4L2_STD_PAL_Nc         ((v4l2_std_id)0x00000400)
    #define V4L2_STD_PAL_60         ((v4l2_std_id)0x00000800)

``V4L2_STD_PAL_60`` is a hybrid standard with 525 lines, 60 Hz refresh
rate, and PAL color modulation with a 4.43 MHz color subcarrier. Some
PAL video recorders can play back NTSC tapes in this mode for display on
a 50/60 Hz agnostic PAL TV.
(``V4L2_STD_PAL_60`` 是一种混合制式，有 525 行、60 Hz 刷新率，采用带
4.43 MHz 色度副载波的 PAL 彩色调制。一些 PAL 录像机可以在此模式下播放
NTSC 磁带，在对 50/60 Hz 不敏感的 PAL 电视上显示。)

.. code-block:: c

    #define V4L2_STD_NTSC_M         ((v4l2_std_id)0x00001000)
    #define V4L2_STD_NTSC_M_JP      ((v4l2_std_id)0x00002000)
    #define V4L2_STD_NTSC_443       ((v4l2_std_id)0x00004000)

``V4L2_STD_NTSC_443`` is a hybrid standard with 525 lines, 60 Hz refresh
rate, and NTSC color modulation with a 4.43 MHz color subcarrier.
(``V4L2_STD_NTSC_443`` 是一种混合制式，有 525 行、60 Hz 刷新率，采用带
4.43 MHz 色度副载波的 NTSC 彩色调制。)

.. code-block:: c

    #define V4L2_STD_NTSC_M_KR      ((v4l2_std_id)0x00008000)

    #define V4L2_STD_SECAM_B        ((v4l2_std_id)0x00010000)
    #define V4L2_STD_SECAM_D        ((v4l2_std_id)0x00020000)
    #define V4L2_STD_SECAM_G        ((v4l2_std_id)0x00040000)
    #define V4L2_STD_SECAM_H        ((v4l2_std_id)0x00080000)
    #define V4L2_STD_SECAM_K        ((v4l2_std_id)0x00100000)
    #define V4L2_STD_SECAM_K1       ((v4l2_std_id)0x00200000)
    #define V4L2_STD_SECAM_L        ((v4l2_std_id)0x00400000)
    #define V4L2_STD_SECAM_LC       ((v4l2_std_id)0x00800000)

    /* ATSC/HDTV */
    #define V4L2_STD_ATSC_8_VSB     ((v4l2_std_id)0x01000000)
    #define V4L2_STD_ATSC_16_VSB    ((v4l2_std_id)0x02000000)

``V4L2_STD_ATSC_8_VSB`` and ``V4L2_STD_ATSC_16_VSB`` are U.S.
terrestrial digital TV standards. Presently the V4L2 API does not
support digital TV. See also the Linux DVB API at
`https://linuxtv.org <https://linuxtv.org>`__.
(``V4L2_STD_ATSC_8_VSB`` 和 ``V4L2_STD_ATSC_16_VSB`` 是美国的地面数字电
视制式。目前 V4L2 API 尚不支持数字电视。另请参见 Linux DVB API：
`https://linuxtv.org <https://linuxtv.org>`__ 。)

.. code-block:: c

    #define V4L2_STD_PAL_BG         (V4L2_STD_PAL_B         |
		     V4L2_STD_PAL_B1        |
		     V4L2_STD_PAL_G)
    #define V4L2_STD_B              (V4L2_STD_PAL_B         |
		     V4L2_STD_PAL_B1        |
		     V4L2_STD_SECAM_B)
    #define V4L2_STD_GH             (V4L2_STD_PAL_G         |
		     V4L2_STD_PAL_H         |
		     V4L2_STD_SECAM_G       |
		     V4L2_STD_SECAM_H)
    #define V4L2_STD_PAL_DK         (V4L2_STD_PAL_D         |
		     V4L2_STD_PAL_D1        |
		     V4L2_STD_PAL_K)
    #define V4L2_STD_PAL            (V4L2_STD_PAL_BG        |
		     V4L2_STD_PAL_DK        |
		     V4L2_STD_PAL_H         |
		     V4L2_STD_PAL_I)
    #define V4L2_STD_NTSC           (V4L2_STD_NTSC_M        |
		     V4L2_STD_NTSC_M_JP     |
		     V4L2_STD_NTSC_M_KR)
    #define V4L2_STD_MN             (V4L2_STD_PAL_M         |
		     V4L2_STD_PAL_N         |
		     V4L2_STD_PAL_Nc        |
		     V4L2_STD_NTSC)
    #define V4L2_STD_SECAM_DK       (V4L2_STD_SECAM_D       |
		     V4L2_STD_SECAM_K       |
		     V4L2_STD_SECAM_K1)
    #define V4L2_STD_DK             (V4L2_STD_PAL_DK        |
		     V4L2_STD_SECAM_DK)

    #define V4L2_STD_SECAM          (V4L2_STD_SECAM_B       |
		     V4L2_STD_SECAM_G       |
		     V4L2_STD_SECAM_H       |
		     V4L2_STD_SECAM_DK      |
		     V4L2_STD_SECAM_L       |
		     V4L2_STD_SECAM_LC)

    #define V4L2_STD_525_60         (V4L2_STD_PAL_M         |
		     V4L2_STD_PAL_60        |
		     V4L2_STD_NTSC          |
		     V4L2_STD_NTSC_443)
    #define V4L2_STD_625_50         (V4L2_STD_PAL           |
		     V4L2_STD_PAL_N         |
		     V4L2_STD_PAL_Nc        |
		     V4L2_STD_SECAM)

    #define V4L2_STD_UNKNOWN        0
    #define V4L2_STD_ALL            (V4L2_STD_525_60        |
		     V4L2_STD_625_50)

.. raw:: latex

    \begingroup
    \tiny
    \setlength{\tabcolsep}{2pt}

..                            NTSC/M   PAL/M    /N       /B       /D       /H       /I        SECAM/B    /D       /K1     /L
.. tabularcolumns:: |p{1.43cm}|p{1.38cm}|p{1.59cm}|p{1.7cm}|p{1.7cm}|p{1.17cm}|p{0.64cm}|p{1.71cm}|p{1.6cm}|p{1.07cm}|p{1.07cm}|p{1.07cm}|

.. _video-standards:

.. flat-table:: Video Standards (based on :ref:`itu470`)
    :header-rows:  1
    :stub-columns: 0

    * - Characteristics(特性)
      - M/NTSC [#f2]_
      - M/PAL
      - N/PAL [#f3]_
      - B, B1, G/PAL
      - D, D1, K/PAL
      - H/PAL
      - I/PAL
      - B, G/SECAM
      - D, K/SECAM
      - K1/SECAM
      - L/SECAM
    * - Frame lines(每帧行数)
      - :cspan:`1` 525
      - :cspan:`8` 625
    * - Frame period (s)(帧周期，秒)
      - :cspan:`1` 1001/30000
      - :cspan:`8` 1/25
    * - Chrominance sub-carrier frequency (Hz)(色度副载波频率，Hz)
      - 3579545 ± 10
      - 3579611.49 ± 10
      - 4433618.75 ± 5

	(3582056.25 ± 5)
      - :cspan:`3` 4433618.75 ± 5
      - 4433618.75 ± 1
      - :cspan:`2` f\ :sub:`OR` = 4406250 ± 2000,

	f\ :sub:`OB` = 4250000 ± 2000
    * - Nominal radio-frequency channel bandwidth (MHz)(标称射频信道带宽，MHz)
      - 6
      - 6
      - 6
      - B: 7; B1, G: 8
      - 8
      - 8
      - 8
      - 8
      - 8
      - 8
      - 8
    * - Sound carrier relative to vision carrier (MHz)(伴音载波相对图像载波，MHz)
      - 4.5
      - 4.5
      - 4.5
      - 5.5 ± 0.001  [#f4]_  [#f5]_  [#f6]_  [#f7]_
      - 6.5 ± 0.001
      - 5.5
      - 5.9996 ± 0.0005
      - 5.5 ± 0.001
      - 6.5 ± 0.001
      - 6.5
      - 6.5 [#f8]_

.. raw:: latex

    \endgroup


Return Value(返回值)
====================

On success 0 is returned, on error -1 and the ``errno`` variable is set
appropriately. The generic error codes are described at the
:ref:`Generic Error Codes <gen-errors>` chapter.
(成功时返回 0，出错时返回 -1 并把 ``errno`` 变量设置为相应的值。通用错
误码见 :ref:`Generic Error Codes <gen-errors>` 一章。)

EINVAL
    The struct :c:type:`v4l2_standard` ``index`` is out
    of bounds.
    (struct :c:type:`v4l2_standard` 的 ``index`` 越界。)

ENODATA
    Standard video timings are not supported for this input or output.
    (本输入或输出不支持标准视频时序。)

.. [#f1]
   The supported standards may overlap and we need an unambiguous set to
   find the current standard returned by :ref:`VIDIOC_G_STD <VIDIOC_G_STD>`.
   (支持的制式可能相互重叠，因此我们需要一个无歧义的集合来找出
   :ref:`VIDIOC_G_STD <VIDIOC_G_STD>` 返回的当前制式。)

.. [#f2]
   Japan uses a standard similar to M/NTSC (V4L2_STD_NTSC_M_JP).
   (日本使用的制式类似 M/NTSC (V4L2_STD_NTSC_M_JP)。)

.. [#f3]
   The values in brackets apply to the combination N/PAL a.k.a.
   N\ :sub:`C` used in Argentina (V4L2_STD_PAL_Nc).
   (括号中的值适用于 N/PAL (又称 N\ :sub:`C`)，用于阿根廷
   (V4L2_STD_PAL_Nc)。)

.. [#f4]
   In the Federal Republic of Germany, Austria, Italy, the Netherlands,
   Slovakia and Switzerland a system of two sound carriers is used, the
   frequency of the second carrier being 242.1875 kHz above the
   frequency of the first sound carrier. For stereophonic sound
   transmissions a similar system is used in Australia.
   (在德意志联邦共和国、奥地利、意大利、荷兰、斯洛伐克和瑞士，使用双伴
   音载波系统，第二载波的频率比第一伴音载波的频率高 242.1875 kHz。对于
   立体声传输，澳大利亚使用类似的系统。)

.. [#f5]
   New Zealand uses a sound carrier displaced 5.4996 ± 0.0005 MHz from
   the vision carrier.
   (新西兰使用的伴音载波偏离图像载波 5.4996 ± 0.0005 MHz。)

.. [#f6]
   In Denmark, Finland, New Zealand, Sweden and Spain a system of two
   sound carriers is used. In Iceland, Norway and Poland the same system
   is being introduced. The second carrier is 5.85 MHz above the vision
   carrier and is DQPSK modulated with 728 kbit/s sound and data
   multiplex. (NICAM system)
   (在丹麦、芬兰、新西兰、瑞典和西班牙，使用双伴音载波系统。在冰岛、挪
   威和波兰，同样的系统正在引入。第二载波比图像载波高 5.85 MHz，并以
   728 kbit/s 的声音与数据复用进行 DQPSK 调制。(NICAM 系统))

.. [#f7]
   In the United Kingdom, a system of two sound carriers is used. The
   second sound carrier is 6.552 MHz above the vision carrier and is
   DQPSK modulated with a 728 kbit/s sound and data multiplex able to
   carry two sound channels. (NICAM system)
   (在英国，使用双伴音载波系统。第二伴音载波比图像载波高 6.552 MHz，并
   以能够承载两个声音通道的 728 kbit/s 声音与数据复用进行 DQPSK 调制。
   (NICAM 系统))

.. [#f8]
   In France, a digital carrier 5.85 MHz away from the vision carrier
   may be used in addition to the main sound carrier. It is modulated in
   differentially encoded QPSK with a 728 kbit/s sound and data
   multiplexer capable of carrying two sound channels. (NICAM system)
   (在法国，除主伴音载波之外，还可以使用距图像载波 5.85 MHz 的数字载
   波。它以能够承载两个声音通道的 728 kbit/s 声音与数据复用器进行差分编
   码 QPSK 调制。(NICAM 系统))
