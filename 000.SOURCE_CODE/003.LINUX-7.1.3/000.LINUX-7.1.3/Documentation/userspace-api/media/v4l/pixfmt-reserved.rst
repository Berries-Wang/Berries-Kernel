.. SPDX-License-Identifier: GFDL-1.1-no-invariants-or-later
.. c:namespace:: V4L

.. _pixfmt-reserved:

*********************************************
Reserved Format Identifiers(保留的格式标识符)
*********************************************

These formats are not defined by this specification, they are just
listed for reference and to avoid naming conflicts. If you want to
register your own format, send an e-mail to the linux-media mailing list
`https://linuxtv.org/lists.php <https://linuxtv.org/lists.php>`__
for inclusion in the ``videodev2.h`` file. If you want to share your
format with other developers add a link to your documentation and send a
copy to the linux-media mailing list for inclusion in this section. If
you think your format should be listed in a standard format section
please make a proposal on the linux-media mailing list.
(这些格式不由本规范定义，列出它们只是为了参考并避免命名冲突。如果你想
注册自己的格式，请发送电子邮件到 linux-media 邮件列表
(https://linuxtv.org/lists.php)，以便将其纳入 ``videodev2.h`` 文件。
如果你想与其他开发者共享你的格式，请添加指向你文档的链接，并发送一份
副本到 linux-media 邮件列表以便纳入本节。如果你认为你的格式应当列在某个
标准格式小节中，请在 linux-media 邮件列表上提出建议。)


.. tabularcolumns:: |p{6.6cm}|p{2.2cm}|p{8.5cm}|

.. raw:: latex

    \small

.. _reserved-formats:

.. flat-table:: Reserved Image Formats
    :header-rows:  1
    :stub-columns: 0
    :widths:       3 1 4

    * - Identifier(标识符)
      - Code(代码)
      - Details(详情)
    * .. _V4L2-PIX-FMT-DV:

      - ``V4L2_PIX_FMT_DV``
      - 'dvsd'
      - unknown (未知)
    * .. _V4L2-PIX-FMT-ET61X251:

      - ``V4L2_PIX_FMT_ET61X251``
      - 'E625'
      - Compressed format of the ET61X251 driver.
	(ET61X251 驱动使用的压缩格式。)
    * .. _V4L2-PIX-FMT-HI240:

      - ``V4L2_PIX_FMT_HI240``
      - 'HI24'
      - 8 bit RGB format used by the BTTV driver.
	(BTTV 驱动使用的 8 位 RGB 格式。)
    * .. _V4L2-PIX-FMT-CPIA1:

      - ``V4L2_PIX_FMT_CPIA1``
      - 'CPIA'
      - YUV format used by the gspca cpia1 driver.
	(gspca cpia1 驱动使用的 YUV 格式。)
    * .. _V4L2-PIX-FMT-JPGL:

      - ``V4L2_PIX_FMT_JPGL``
      - 'JPGL'
      - JPEG-Light format (Pegasus Lossless JPEG) used in Divio webcams NW
	80x.
	(Divio NW 80x 网络摄像头使用的 JPEG-Light 格式 (Pegasus 无损
	JPEG)。)
    * .. _V4L2-PIX-FMT-SPCA501:

      - ``V4L2_PIX_FMT_SPCA501``
      - 'S501'
      - YUYV per line used by the gspca driver.
	(gspca 驱动使用的每行 YUYV 格式。)
    * .. _V4L2-PIX-FMT-SPCA505:

      - ``V4L2_PIX_FMT_SPCA505``
      - 'S505'
      - YYUV per line used by the gspca driver.
	(gspca 驱动使用的每行 YYUV 格式。)
    * .. _V4L2-PIX-FMT-SPCA508:

      - ``V4L2_PIX_FMT_SPCA508``
      - 'S508'
      - YUVY per line used by the gspca driver.
	(gspca 驱动使用的每行 YUVY 格式。)
    * .. _V4L2-PIX-FMT-SPCA561:

      - ``V4L2_PIX_FMT_SPCA561``
      - 'S561'
      - Compressed GBRG Bayer format used by the gspca driver.
	(gspca 驱动使用的压缩 GBRG Bayer 格式。)
    * .. _V4L2-PIX-FMT-PAC207:

      - ``V4L2_PIX_FMT_PAC207``
      - 'P207'
      - Compressed BGGR Bayer format used by the gspca driver.
	(gspca 驱动使用的压缩 BGGR Bayer 格式。)
    * .. _V4L2-PIX-FMT-MR97310A:

      - ``V4L2_PIX_FMT_MR97310A``
      - 'M310'
      - Compressed BGGR Bayer format used by the gspca driver.
	(gspca 驱动使用的压缩 BGGR Bayer 格式。)
    * .. _V4L2-PIX-FMT-JL2005BCD:

      - ``V4L2_PIX_FMT_JL2005BCD``
      - 'JL20'
      - JPEG compressed RGGB Bayer format used by the gspca driver.
	(gspca 驱动使用的 JPEG 压缩 RGGB Bayer 格式。)
    * .. _V4L2-PIX-FMT-OV511:

      - ``V4L2_PIX_FMT_OV511``
      - 'O511'
      - OV511 JPEG format used by the gspca driver.
	(gspca 驱动使用的 OV511 JPEG 格式。)
    * .. _V4L2-PIX-FMT-OV518:

      - ``V4L2_PIX_FMT_OV518``
      - 'O518'
      - OV518 JPEG format used by the gspca driver.
	(gspca 驱动使用的 OV518 JPEG 格式。)
    * .. _V4L2-PIX-FMT-PJPG:

      - ``V4L2_PIX_FMT_PJPG``
      - 'PJPG'
      - Pixart 73xx JPEG format used by the gspca driver.
	(gspca 驱动使用的 Pixart 73xx JPEG 格式。)
    * .. _V4L2-PIX-FMT-SE401:

      - ``V4L2_PIX_FMT_SE401``
      - 'S401'
      - Compressed RGB format used by the gspca se401 driver
	(gspca se401 驱动使用的压缩 RGB 格式)
    * .. _V4L2-PIX-FMT-SQ905C:

      - ``V4L2_PIX_FMT_SQ905C``
      - '905C'
      - Compressed RGGB bayer format used by the gspca driver.
	(gspca 驱动使用的压缩 RGGB Bayer 格式。)
    * .. _V4L2-PIX-FMT-MJPEG:

      - ``V4L2_PIX_FMT_MJPEG``
      - 'MJPG'
      - Compressed format used by the Zoran driver
	(Zoran 驱动使用的压缩格式)
    * .. _V4L2-PIX-FMT-PWC1:

      - ``V4L2_PIX_FMT_PWC1``
      - 'PWC1'
      - Compressed format of the PWC driver.
	(PWC 驱动的压缩格式。)
    * .. _V4L2-PIX-FMT-PWC2:

      - ``V4L2_PIX_FMT_PWC2``
      - 'PWC2'
      - Compressed format of the PWC driver.
	(PWC 驱动的压缩格式。)
    * .. _V4L2-PIX-FMT-SN9C10X:

      - ``V4L2_PIX_FMT_SN9C10X``
      - 'S910'
      - Compressed format of the SN9C102 driver.
	(SN9C102 驱动的压缩格式。)
    * .. _V4L2-PIX-FMT-SN9C20X-I420:

      - ``V4L2_PIX_FMT_SN9C20X_I420``
      - 'S920'
      - YUV 4:2:0 format of the gspca sn9c20x driver.
	(gspca sn9c20x 驱动的 YUV 4:2:0 格式。)
    * .. _V4L2-PIX-FMT-SN9C2028:

      - ``V4L2_PIX_FMT_SN9C2028``
      - 'SONX'
      - Compressed GBRG bayer format of the gspca sn9c2028 driver.
	(gspca sn9c2028 驱动的压缩 GBRG Bayer 格式。)
    * .. _V4L2-PIX-FMT-STV0680:

      - ``V4L2_PIX_FMT_STV0680``
      - 'S680'
      - Bayer format of the gspca stv0680 driver.
	(gspca stv0680 驱动的 Bayer 格式。)
    * .. _V4L2-PIX-FMT-WNVA:

      - ``V4L2_PIX_FMT_WNVA``
      - 'WNVA'
      - Used by the Winnov Videum driver,
	`http://www.thedirks.org/winnov/ <http://www.thedirks.org/winnov/>`__
	(Winnov Videum 驱动使用，见 http://www.thedirks.org/winnov/)
    * .. _V4L2-PIX-FMT-TM6000:

      - ``V4L2_PIX_FMT_TM6000``
      - 'TM60'
      - Used by Trident tm6000
	(Trident tm6000 使用)
    * .. _V4L2-PIX-FMT-CIT-YYVYUY:

      - ``V4L2_PIX_FMT_CIT_YYVYUY``
      - 'CITV'
      - Used by xirlink CIT, found at IBM webcams.
	(xirlink CIT 使用，见于 IBM 网络摄像头。)

	Uses one line of Y then 1 line of VYUY
	(先使用一行 Y，然后一行 VYUY。)
    * .. _V4L2-PIX-FMT-KONICA420:

      - ``V4L2_PIX_FMT_KONICA420``
      - 'KONI'
      - Used by Konica webcams.
	(Konica 网络摄像头使用。)

	YUV420 planar in blocks of 256 pixels.
	(以 256 像素为块的平面 YUV420。)
    * .. _V4L2-PIX-FMT-YYUV:

      - ``V4L2_PIX_FMT_YYUV``
      - 'YYUV'
      - unknown (未知)
    * .. _V4L2-PIX-FMT-Y4:

      - ``V4L2_PIX_FMT_Y4``
      - 'Y04 '
      - Old 4-bit greyscale format. Only the most significant 4 bits of
	each byte are used, the other bits are set to 0.
	(旧的 4 位灰度格式。每个字节只有最高的 4 位被使用，其余位设为
	0。)
    * .. _V4L2-PIX-FMT-Y6:

      - ``V4L2_PIX_FMT_Y6``
      - 'Y06 '
      - Old 6-bit greyscale format. Only the most significant 6 bits of
	each byte are used, the other bits are set to 0.
	(旧的 6 位灰度格式。每个字节只有最高的 6 位被使用，其余位设为
	0。)
    * .. _V4L2-PIX-FMT-S5C-UYVY-JPG:

      - ``V4L2_PIX_FMT_S5C_UYVY_JPG``
      - 'S5CI'
      - Two-planar format used by Samsung S5C73MX cameras. The first plane
	contains interleaved JPEG and UYVY image data, followed by meta
	data in form of an array of offsets to the UYVY data blocks. The
	actual pointer array follows immediately the interleaved JPEG/UYVY
	data, the number of entries in this array equals the height of the
	UYVY image. Each entry is a 4-byte unsigned integer in big endian
	order and it's an offset to a single pixel line of the UYVY image.
	The first plane can start either with JPEG or UYVY data chunk. The
	size of a single UYVY block equals the UYVY image's width
	multiplied by 2. The size of a JPEG chunk depends on the image and
	can vary with each line.
	(Samsung S5C73MX 相机使用的双平面格式。第一个平面包含交错的 JPEG
	与 UYVY 图像数据，其后是以偏移数组形式出现的元数据，该数组指向各个
	UYVY 数据块。实际的指针数组紧跟在交错的 JPEG/UYVY 数据之后，数组
	条目数等于 UYVY 图像的高度。每个条目是一个按大端序排列的 4 字节无
	符号整数，是指向 UYVY 图像单个像素行的偏移。第一个平面既可以以
	JPEG 数据块开头，也可以以 UYVY 数据块开头。单个 UYVY 块的大小等于
	UYVY 图像宽度乘以 2。JPEG 块的大小取决于图像，并且每一行都可能
	不同。)

	The second plane, at an offset of 4084 bytes, contains a 4-byte
	offset to the pointer array in the first plane. This offset is
	followed by a 4-byte value indicating size of the pointer array.
	All numbers in the second plane are also in big endian order.
	Remaining data in the second plane is undefined. The information
	in the second plane allows to easily find location of the pointer
	array, which can be different for each frame. The size of the
	pointer array is constant for given UYVY image height.
	(第二个平面位于偏移 4084 字节处，包含一个指向第一个平面中指针数组
	的 4 字节偏移。该偏移之后是一个 4 字节的值，指示指针数组的大小。
	第二个平面中的所有数字同样按大端序排列。第二个平面中剩余的数据
	未定义。第二个平面中的信息可以方便地定位指针数组，指针数组对每一帧
	可能不同。对于给定的 UYVY 图像高度，指针数组的大小是恒定的。)

	In order to extract UYVY and JPEG frames an application can
	initially set a data pointer to the start of first plane and then
	add an offset from the first entry of the pointers table. Such a
	pointer indicates start of an UYVY image pixel line. Whole UYVY
	line can be copied to a separate buffer. These steps should be
	repeated for each line, i.e. the number of entries in the pointer
	array. Anything what's in between the UYVY lines is JPEG data and
	should be concatenated to form the JPEG stream.
	(为了提取 UYVY 与 JPEG 帧，应用程序可以先把数据指针设置为第一个
	平面的起始处，然后加上指针表第一个条目中的偏移。这样的指针指向一个
	UYVY 图像像素行的起始处。整个 UYVY 行可以被复制到单独的缓冲区。
	应对指针数组中的每个条目 (即每一行) 重复这些步骤。UYVY 行之间的
	任何内容都是 JPEG 数据，应当拼接起来形成 JPEG 流。)
    * .. _V4L2-PIX-FMT-MT21C:

      - ``V4L2_PIX_FMT_MT21C``
      - 'MT21'
      - Compressed two-planar YVU420 format used by Mediatek MT8173, MT8192,
        MT8195 and more. The compression is lossless. This format have
	similitude with ``V4L2_PIX_FMT_MM21`` in term of alignment and tiling.
	It remains an opaque intermediate format and the MDP hardware must be
	used to convert ``V4L2_PIX_FMT_MT21C`` to ``V4L2_PIX_FMT_NV12M``,
	``V4L2_PIX_FMT_YUV420M`` or ``V4L2_PIX_FMT_YVU420``.
	(Mediatek MT8173、MT8192、MT8195 等平台使用的压缩双平面 YVU420
	格式。压缩是无损的。在对齐与平铺 (tiling) 方面，该格式与
	``V4L2_PIX_FMT_MM21`` 有相似之处。它仍是一种不透明的中间格式，
	必须使用 MDP 硬件把 ``V4L2_PIX_FMT_MT21C`` 转换为
	``V4L2_PIX_FMT_NV12M``、``V4L2_PIX_FMT_YUV420M`` 或
	``V4L2_PIX_FMT_YVU420``。)
    * .. _V4L2-PIX-FMT-QC08C:

      - ``V4L2_PIX_FMT_QC08C``
      - 'QC08C'
      - Compressed Macro-tile 8-Bit YUV420 format used by Qualcomm platforms.
        It is an opaque intermediate format. The used compression is lossless
        and it is used by various multimedia hardware blocks like GPU, display
        controllers, ISP and video accelerators.
        It contains four planes for progressive video and eight planes for
        interlaced video.
	(Qualcomm 平台使用的压缩宏块 8 位 YUV420 格式。它是一种不透明的
	中间格式。所使用的压缩是无损的，并被 GPU、显示控制器、ISP 与视频
	加速器等各种多媒体硬件块使用。对于逐行视频它包含四个平面，对于隔行
	视频包含八个平面。)
    * .. _V4L2-PIX-FMT-QC10C:

      - ``V4L2_PIX_FMT_QC10C``
      - 'QC10C'
      - Compressed Macro-tile 10-Bit YUV420 format used by Qualcomm platforms.
        It is an opaque intermediate format. The used compression is lossless
        and it is used by various multimedia hardware blocks like GPU, display
        controllers, ISP and video accelerators.
        It contains four planes for progressive video.
	(Qualcomm 平台使用的压缩宏块 10 位 YUV420 格式。它是一种不透明的
	中间格式。所使用的压缩是无损的，并被 GPU、显示控制器、ISP 与视频
	加速器等各种多媒体硬件块使用。对于逐行视频它包含四个平面。)
    * .. _V4L2-PIX-FMT-AJPG:

      - ``V4L2_PIX_FMT_AJPG``
      - 'AJPG'
      - ASPEED JPEG format used by the aspeed-video driver on Aspeed platforms,
        which is generally adapted for remote KVM.
        On each frame compression, I will compare the new frame with previous
        one to decide which macroblock's data is changed, and only the changed
        macroblocks will be compressed.
	(Aspeed 平台上 aspeed-video 驱动使用的 ASPEED JPEG 格式，通常适配
	于远程 KVM。在每一帧压缩时，会把新帧与上一帧进行比较，以决定哪些
	宏块的数据发生了变化，只有发生变化的宏块才会被压缩。)

        The implementation is based on AST2600 A3 datasheet, revision 0.9, which
        is not publicly available. Or you can reference Video stream data format
        – ASPEED mode compression of SDK_User_Guide which available on
        `github <https://github.com/AspeedTech-BMC/openbmc/releases/>`__.
	(该实现基于 AST2600 A3 数据手册 0.9 版，该手册未公开发布。你也可以
	参考 SDK_User_Guide 中的 "Video stream data format - ASPEED mode
	compression"，可在 github 上获取。)

        Decoder's implementation can be found here,
        `aspeed_codec <https://github.com/AspeedTech-BMC/aspeed_codec/>`__
	(解码器的实现可以在这里找到：aspeed_codec)
    * .. _V4L2-PIX-FMT-HEXTILE:

      - ``V4L2_PIX_FMT_HEXTILE``
      - 'HXTL'
      - Compressed format used by Nuvoton NPCM video driver. This format is
        defined in Remote Framebuffer Protocol (RFC 6143, chapter 7.7.4 Hextile
        Encoding).
	(Nuvoton NPCM 视频驱动使用的压缩格式。该格式定义于远程帧缓冲协议
	(RFC 6143，第 7.7.4 节 Hextile Encoding)。)
.. raw:: latex

    \normalsize
