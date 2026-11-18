.. SPDX-License-Identifier: GFDL-1.1-no-invariants-or-later OR GPL-2.0

.. c:namespace:: dtv.legacy.osd

.. _dvb_osd:

============================
DVB OSD Device(DVB OSD 设备)
============================

.. attention:: Do **not** use in new drivers!
             See: :ref:`legacy_dvb_decoder_notes`
             (不要在新驱动中使用！参见：:ref:`legacy_dvb_decoder_notes`。)

The DVB OSD device controls the OnScreen-Display of the AV7110 based
DVB-cards with hardware MPEG2 decoder. It can be accessed through
``/dev/dvb/adapter?/osd0``.
(DVB OSD 设备控制基于 AV7110、带硬件 MPEG2 解码器的 DVB 卡的屏幕显示（OnScreen-Display）。可以通过 ``/dev/dvb/adapter?/osd0`` 访问它。)
Data types and ioctl definitions can be accessed by including
``linux/dvb/osd.h`` in your application.
(在应用程序中包含 ``linux/dvb/osd.h`` 即可获得数据类型和 ioctl 定义。)

The OSD is not a frame-buffer like on many other cards.
It is a kind of canvas one can draw on.
The color-depth is limited depending on the memory size installed.
An appropriate palette of colors has to be set up.
The installed memory size can be identified with the `OSD_GET_CAPABILITY(获取 OSD 能力)`_
ioctl.
(OSD 不像许多其他卡上那样是帧缓冲（frame-buffer）。它是一种可以在上面作画的画布。色彩深度受已安装内存大小的限制。必须设置一个合适的颜色调色板。已安装的内存大小可以通过 `OSD_GET_CAPABILITY(获取 OSD 能力)`_ ioctl 识别。)

OSD Data Types(OSD 数据类型)
============================

OSD_Command(OSD 命令)
---------------------

Synopsis(概要)
~~~~~~~~~~~~~~

.. code-block:: c

    typedef enum {
	/* All functions return -2 on "not open" */
	OSD_Close = 1,
	OSD_Open,
	OSD_Show,
	OSD_Hide,
	OSD_Clear,
	OSD_Fill,
	OSD_SetColor,
	OSD_SetPalette,
	OSD_SetTrans,
	OSD_SetPixel,
	OSD_GetPixel,
	OSD_SetRow,
	OSD_SetBlock,
	OSD_FillRow,
	OSD_FillBlock,
	OSD_Line,
	OSD_Query,
	OSD_Test,
	OSD_Text,
	OSD_SetWindow,
	OSD_MoveWindow,
	OSD_OpenRaw,
    } OSD_Command;

Commands(命令)
~~~~~~~~~~~~~~

.. note::  All functions return -2 on "not open"
           (所有函数在"未打开"时都返回 -2。)

.. flat-table::
    :header-rows:  1
    :stub-columns: 0

    -  ..

       -  Command(命令)

       -  | Used variables of ``struct`` `osd_cmd_t(OSD 命令结构体)`_.
          | Usage{variable} if alternative use.
          | (所用 ``struct`` `osd_cmd_t(OSD 命令结构体)`_ 的变量。)
          | (Usage{variable} 表示可替代用法。)

       -  :cspan:`2` Description(描述)



    -  ..

       -  ``OSD_Close``

       -  -

       -  | Disables OSD and releases the buffers.
          | Returns 0 on success.
          | (禁用 OSD 并释放缓冲区。成功时返回 0。)

    -  ..

       -  ``OSD_Open``

       -  | x0,y0,x1,y1,
          | BitPerPixel[2/4/8]{color&0x0F},
          | mix[0..15]{color&0xF0}

       -  | Opens OSD with this size and bit depth
          | Returns 0 on success,
          | -1 on DRAM allocation error,
          | -2 on "already open".
          | (以该尺寸和位深打开 OSD。成功时返回 0，DRAM 分配错误时返回 -1，"已打开"时返回 -2。)

    -  ..

       -  ``OSD_Show``

       - -

       -  | Enables OSD mode.
          | Returns 0 on success.
          | (启用 OSD 模式。成功时返回 0。)

    -  ..

       -  ``OSD_Hide``

       - -

       -  | Disables OSD mode.
          | Returns 0 on success.
          | (禁用 OSD 模式。成功时返回 0。)

    -  ..

       -  ``OSD_Clear``

       - -

       -  | Sets all pixel to color 0.
          | Returns 0 on success.
          | (把所有像素设为颜色 0。成功时返回 0。)

    -  ..

       -  ``OSD_Fill``

       -  color

       -  | Sets all pixel to color <color>.
          | Returns 0 on success.
          | (把所有像素设为颜色 <color>。成功时返回 0。)

    -  ..

       -  ``OSD_SetColor``

       -  | color,
          | R{x0},G{y0},B{x1},
          | opacity{y1}

       -  | Set palette entry <num> to <r,g,b>, <mix> and <trans> apply
          | R,G,B: 0..255
          | R=Red, G=Green, B=Blue
          | opacity=0:      pixel opacity 0% (only video pixel shows)
          | opacity=1..254: pixel opacity as specified in header
          | opacity=255:    pixel opacity 100% (only OSD pixel shows)
          | Returns 0 on success, -1 on error.
          | (把调色板条目 <num> 设置为 <r,g,b>，并应用 <mix> 和 <trans>。R,G,B 取值 0..255，R=红、G=绿、B=蓝。)
          | (opacity=0：像素不透明度 0%（只显示视频像素）；opacity=1..254：像素不透明度按头部指定值；opacity=255：像素不透明度 100%（只显示 OSD 像素）。成功返回 0，出错返回 -1。)

    -  ..

       -  ``OSD_SetPalette``

       -  | firstcolor{color},
          | lastcolor{x0},data

       -  | Set a number of entries in the palette.
          | Sets the entries "firstcolor" through "lastcolor" from the
            array "data".
          | Data has 4 byte for each color:
          | R,G,B, and a opacity value: 0->transparent, 1..254->mix,
            255->pixel
          | (在调色板中设置若干条目：用数组 "data" 设置从 "firstcolor" 到 "lastcolor" 的条目。)
          | (每个颜色占 4 字节：R,G,B 和一个不透明度值：0->透明，1..254->混合，255->像素。)

    -  ..

       -  ``OSD_SetTrans``

       -  transparency{color}

       -  | Sets transparency of mixed pixel (0..15).
          | Returns 0 on success.
          | (设置混合像素的透明度（0..15）。成功时返回 0。)

    -  ..

       -  ``OSD_SetPixel``

       -  x0,y0,color

       -  | Sets pixel <x>,<y> to color number <color>.
          | Returns 0 on success, -1 on error.
          | (把像素 <x>,<y> 设为颜色编号 <color>。成功返回 0，出错返回 -1。)

    -  ..

       -  ``OSD_GetPixel``

       -  x0,y0

       -  | Returns color number of pixel <x>,<y>,  or -1.
          | Command currently not supported by the AV7110!
          | (返回像素 <x>,<y> 的颜色编号，或返回 -1。AV7110 目前不支持该命令！)

    -  ..

       -  ``OSD_SetRow``

       -  x0,y0,x1,data

       -  | Fills pixels x0,y through  x1,y with the content of data[].
          | Returns 0 on success, -1 on clipping all pixel (no pixel
            drawn).
          | (用 data[] 的内容填充从 x0,y 到 x1,y 的像素。成功返回 0，所有像素都被裁剪（未绘制任何像素）时返回 -1。)

    -  ..

       -  ``OSD_SetBlock``

       -  | x0,y0,x1,y1,
          | increment{color},
          | data

       -  | Fills pixels x0,y0 through  x1,y1 with the content of data[].
          | Inc contains the width of one line in the data block,
          | inc<=0 uses block width as line width.
          | Returns 0 on success, -1 on clipping all pixel.
          | (用 data[] 的内容填充从 x0,y0 到 x1,y1 的像素。Inc 是数据块中一行的宽度，inc<=0 时使用块宽作为行宽。成功返回 0，所有像素都被裁剪时返回 -1。)

    -  ..

       -  ``OSD_FillRow``

       -  x0,y0,x1,color

       -  | Fills pixels x0,y through  x1,y with the color <color>.
          | Returns 0 on success, -1 on clipping all pixel.
          | (用颜色 <color> 填充从 x0,y 到 x1,y 的像素。成功返回 0，所有像素都被裁剪时返回 -1。)

    -  ..

       -  ``OSD_FillBlock``

       -  x0,y0,x1,y1,color

       -  | Fills pixels x0,y0 through  x1,y1 with the color <color>.
          | Returns 0 on success, -1 on clipping all pixel.
          | (用颜色 <color> 填充从 x0,y0 到 x1,y1 的像素。成功返回 0，所有像素都被裁剪时返回 -1。)

    -  ..

       -  ``OSD_Line``

       -  x0,y0,x1,y1,color

       -  | Draw a line from x0,y0 to x1,y1 with the color <color>.
          | Returns 0 on success.
          | (用颜色 <color> 从 x0,y0 到 x1,y1 画一条线。成功时返回 0。)

    -  ..

       -  ``OSD_Query``

       -  | x0,y0,x1,y1,
          | xasp{color}; yasp=11

       -  | Fills parameters with the picture dimensions and the pixel
            aspect ratio.
          | Returns 0 on success.
          | Command currently not supported by the AV7110!
          | (用图像尺寸和像素纵横比填充各参数。成功时返回 0。AV7110 目前不支持该命令！)

    -  ..

       -  ``OSD_Test``

       -  -

       -  | Draws a test picture.
          | For debugging purposes only.
          | Returns 0 on success.
          | (绘制一幅测试图。仅用于调试目的。成功时返回 0。)
    -  ..

       -  ``OSD_Text``

       -  x0,y0,size,color,text

       -  Draws a text at position x0,y0 with the color <color>.
          (在位置 x0,y0 处用颜色 <color> 绘制文本。)

    -  ..

       -  ``OSD_SetWindow``

       -  x0

       -  Set window with number 0<x0<8 as current.
          (把编号为 0<x0<8 的窗口设为当前窗口。)

    -  ..

       -  ``OSD_MoveWindow``

       -  x0,y0

       -  Move current window to (x0, y0).
          (把当前窗口移动到 (x0, y0)。)

    -  ..

       -  ``OSD_OpenRaw``

       -  | x0,y0,x1,y1,
          | `osd_raw_window_t(OSD 原始窗口类型)`_ {color}

       -  Open other types of OSD windows.
          (打开其他类型的 OSD 窗口。)

Description(描述)
~~~~~~~~~~~~~~~~~

The ``OSD_Command`` data type is used with the `OSD_SEND_CMD(发送 OSD 命令)`_ ioctl to
tell the driver which OSD_Command to execute.
(``OSD_Command`` 数据类型与 `OSD_SEND_CMD(发送 OSD 命令)`_ ioctl 一起使用，告诉驱动程序要执行哪个 OSD 命令。)


-----

osd_cmd_t(OSD 命令结构体)
-------------------------

Synopsis(概要)
~~~~~~~~~~~~~~

.. code-block:: c

    typedef struct osd_cmd_s {
	OSD_Command cmd;
	int x0;
	int y0;
	int x1;
	int y1;
	int color;
	void __user *data;
    } osd_cmd_t;

Variables(变量)
~~~~~~~~~~~~~~~

.. flat-table::
    :header-rows:  0
    :stub-columns: 0

    -  ..

       -  ``OSD_Command cmd``

       -  `OSD_Command(OSD 命令)`_ to be executed. (要执行的命令。)

    -  ..

       -  ``int x0``

       -  First horizontal position. (第一个水平位置。)

    -  ..

       -  ``int y0``

       -  First vertical position. (第一个垂直位置。)

    -  ..

       -  ``int x1``

       -  Second horizontal position. (第二个水平位置。)

    -  ..

       -  ``int y1``

       -  Second vertical position. (第二个垂直位置。)

    -  ..

       -  ``int color``

       -  Number of the color in the palette. (调色板中的颜色编号。)

    -  ..

       -  ``void __user *data``

       -  Command specific Data. (命令特定的数据。)

Description(描述)
~~~~~~~~~~~~~~~~~

The ``osd_cmd_t`` data type is used with the `OSD_SEND_CMD(发送 OSD 命令)`_ ioctl.
It contains the data for the OSD_Command and the `OSD_Command(OSD 命令)`_ itself.
The structure has to be passed to the driver and the components may be
modified by it.
(``osd_cmd_t`` 数据类型与 `OSD_SEND_CMD(发送 OSD 命令)`_ ioctl 一起使用。它包含 OSD_Command 的数据以及 `OSD_Command(OSD 命令)`_ 本身。该结构体必须传递给驱动程序，其各成员可能会被驱动程序修改。)


-----


osd_raw_window_t(OSD 原始窗口类型)
----------------------------------

Synopsis(概要)
~~~~~~~~~~~~~~

.. code-block:: c

    typedef enum {
	OSD_BITMAP1,
	OSD_BITMAP2,
	OSD_BITMAP4,
	OSD_BITMAP8,
	OSD_BITMAP1HR,
	OSD_BITMAP2HR,
	OSD_BITMAP4HR,
	OSD_BITMAP8HR,
	OSD_YCRCB422,
	OSD_YCRCB444,
	OSD_YCRCB444HR,
	OSD_VIDEOTSIZE,
	OSD_VIDEOHSIZE,
	OSD_VIDEOQSIZE,
	OSD_VIDEODSIZE,
	OSD_VIDEOTHSIZE,
	OSD_VIDEOTQSIZE,
	OSD_VIDEOTDSIZE,
	OSD_VIDEONSIZE,
	OSD_CURSOR
    } osd_raw_window_t;

Constants(常量)
~~~~~~~~~~~~~~~

.. flat-table::
    :header-rows:  0
    :stub-columns: 0

    -  ..

       -  ``OSD_BITMAP1``

       -  :cspan:`1` 1 bit bitmap (1 位位图)

    -  ..

       -  ``OSD_BITMAP2``

       -  2 bit bitmap (2 位位图)

    -  ..

       -  ``OSD_BITMAP4``

       -  4 bit bitmap (4 位位图)

    -  ..

       -  ``OSD_BITMAP8``

       -  8 bit bitmap (8 位位图)

    -  ..

       -  ``OSD_BITMAP1HR``

       -  1 Bit bitmap half resolution (1 位位图，半分辨率)

    -  ..

       -  ``OSD_BITMAP2HR``

       -  2 Bit bitmap half resolution (2 位位图，半分辨率)

    -  ..

       -  ``OSD_BITMAP4HR``

       -  4 Bit bitmap half resolution (4 位位图，半分辨率)

    -  ..

       -  ``OSD_BITMAP8HR``

       -  8 Bit bitmap half resolution (8 位位图，半分辨率)

    -  ..

       -  ``OSD_YCRCB422``

       -  4:2:2 YCRCB Graphic Display (4:2:2 YCRCB 图形显示)

    -  ..

       -  ``OSD_YCRCB444``

       -  4:4:4 YCRCB Graphic Display (4:4:4 YCRCB 图形显示)

    -  ..

       -  ``OSD_YCRCB444HR``

       -  4:4:4 YCRCB graphic half resolution (4:4:4 YCRCB 图形，半分辨率)

    -  ..

       -  ``OSD_VIDEOTSIZE``

       -  True Size Normal MPEG Video Display (真实尺寸正常 MPEG 视频显示)

    -  ..

       -  ``OSD_VIDEOHSIZE``

       -  MPEG Video Display Half Resolution (MPEG 视频显示，半分辨率)

    -  ..

       -  ``OSD_VIDEOQSIZE``

       -  MPEG Video Display Quarter Resolution (MPEG 视频显示，四分之一分辨率)

    -  ..

       -  ``OSD_VIDEODSIZE``

       -  MPEG Video Display Double Resolution (MPEG 视频显示，双倍分辨率)

    -  ..

       -  ``OSD_VIDEOTHSIZE``

       -  True Size MPEG Video Display Half Resolution (真实尺寸 MPEG 视频显示，半分辨率)

    -  ..

       -  ``OSD_VIDEOTQSIZE``

       -  True Size MPEG Video Display Quarter Resolution (真实尺寸 MPEG 视频显示，四分之一分辨率)

    -  ..

       -  ``OSD_VIDEOTDSIZE``

       -  True Size MPEG Video Display Double Resolution (真实尺寸 MPEG 视频显示，双倍分辨率)

    -  ..

       -  ``OSD_VIDEONSIZE``

       -  Full Size MPEG Video Display (全尺寸 MPEG 视频显示)

    -  ..

       -  ``OSD_CURSOR``

       -  Cursor (光标)

Description(描述)
~~~~~~~~~~~~~~~~~

The ``osd_raw_window_t`` data type is used with the `OSD_Command(OSD 命令)`_
OSD_OpenRaw to tell the driver which type of OSD to open.
(``osd_raw_window_t`` 数据类型与 `OSD_Command(OSD 命令)`_ 中的 OSD_OpenRaw 一起使用，告诉驱动程序要打开哪种类型的 OSD。)


-----


osd_cap_t(OSD 能力结构体)
-------------------------

Synopsis(概要)
~~~~~~~~~~~~~~

.. code-block:: c

    typedef struct osd_cap_s {
	int  cmd;
    #define OSD_CAP_MEMSIZE         1
	long val;
    } osd_cap_t;

Variables(变量)
~~~~~~~~~~~~~~~

.. flat-table::
    :header-rows:  0
    :stub-columns: 0

    -  ..

       -  ``int  cmd``

       -  Capability to query. (要查询的能力。)

    -  ..

       -  ``long val``

       -  Used to store the Data. (用于存储数据。)

Supported capabilities(支持的能力)
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

.. flat-table::
    :header-rows:  0
    :stub-columns: 0

    -  ..

       -  ``OSD_CAP_MEMSIZE``

       -  Memory size installed on the card. (卡上安装的内存大小。)

Description(描述)
~~~~~~~~~~~~~~~~~

This structure of data used with the `OSD_GET_CAPABILITY(获取 OSD 能力)`_ call.
(该数据结构与 `OSD_GET_CAPABILITY(获取 OSD 能力)`_ 调用一起使用。)


-----


OSD Function Calls(OSD 函数调用)
================================

OSD_SEND_CMD(发送 OSD 命令)
---------------------------

Synopsis(概要)
~~~~~~~~~~~~~~

.. c:macro:: OSD_SEND_CMD

.. code-block:: c

    int ioctl(int fd, int request = OSD_SEND_CMD, enum osd_cmd_t *cmd)


Arguments(参数)
~~~~~~~~~~~~~~~

.. flat-table::
    :header-rows:  0
    :stub-columns: 0

    -  ..

       -  ``int fd``

       -  :cspan:`1` File descriptor returned by a previous call
          to `open()(打开设备)`_. (由之前调用 `open()(打开设备)`_ 返回的文件描述符。)

    -  ..

       -  ``int request``

       -  Pointer to the location of the structure `osd_cmd_t(OSD 命令结构体)`_ for this
          command. (指向该命令所用 `osd_cmd_t(OSD 命令结构体)`_ 结构体所在位置的指针。)

Description(描述)
~~~~~~~~~~~~~~~~~

.. attention:: Do **not** use in new drivers!
             See: :ref:`legacy_dvb_decoder_notes`
             (不要在新驱动中使用！参见：:ref:`legacy_dvb_decoder_notes`。)

This ioctl sends the `OSD_Command(OSD 命令)`_ to the card.
(该 ioctl 把 `OSD_Command(OSD 命令)`_ 发送到卡上。)

Return Value(返回值)
~~~~~~~~~~~~~~~~~~~~

On success 0 is returned, on error -1 and the ``errno`` variable is set
appropriately. The generic error codes are described at the
:ref:`Generic Error Codes <gen-errors>` chapter.
(成功时返回 0，出错时返回 -1 并将 ``errno`` 变量设置为相应的值。通用错误码在 :ref:`Generic Error Codes <gen-errors>` 一章中描述。)

.. flat-table::
    :header-rows:  0
    :stub-columns: 0

    -  ..

       -  ``EINVAL``

       -  Command is out of range. (命令超出范围。)


-----


OSD_GET_CAPABILITY(获取 OSD 能力)
---------------------------------

Synopsis(概要)
~~~~~~~~~~~~~~

.. c:macro:: OSD_GET_CAPABILITY

.. code-block:: c

    int ioctl(int fd, int request = OSD_GET_CAPABILITY,
    struct osd_cap_t *cap)

Arguments(参数)
~~~~~~~~~~~~~~~

.. flat-table::
    :header-rows:  0
    :stub-columns: 0

    -  ..

       -  ``int fd``

       -  :cspan:`1` File descriptor returned by a previous call
          to `open()(打开设备)`_. (由之前调用 `open()(打开设备)`_ 返回的文件描述符。)

    -  ..

       -  ``int request``

       -  Equals ``OSD_GET_CAPABILITY`` for this command. (对该命令而言等于 ``OSD_GET_CAPABILITY``。)

    -  ..

       -  ``unsigned int *cap``

       -  Pointer to the location of the structure `osd_cap_t(OSD 能力结构体)`_ for this
          command. (指向该命令所用 `osd_cap_t(OSD 能力结构体)`_ 结构体所在位置的指针。)

Description(描述)
~~~~~~~~~~~~~~~~~

.. attention:: Do **not** use in new drivers!
             See: :ref:`legacy_dvb_decoder_notes`
             (不要在新驱动中使用！参见：:ref:`legacy_dvb_decoder_notes`。)

This ioctl is used to get the capabilities of the OSD of the AV7110 based
DVB-decoder-card in use.
(该 ioctl 用于获取正在使用的基于 AV7110 的 DVB 解码卡的 OSD 能力。)

.. note::
    The structure osd_cap_t has to be setup by the user and passed to the
    driver.
    (结构体 osd_cap_t 必须由用户设置好并传递给驱动程序。)

Return Value(返回值)
~~~~~~~~~~~~~~~~~~~~

On success 0 is returned, on error -1 and the ``errno`` variable is set
appropriately. The generic error codes are described at the
:ref:`Generic Error Codes <gen-errors>` chapter.
(成功时返回 0，出错时返回 -1 并将 ``errno`` 变量设置为相应的值。通用错误码在 :ref:`Generic Error Codes <gen-errors>` 一章中描述。)

.. flat-table::
    :header-rows:  0
    :stub-columns: 0


    -  ..

       -  ``EINVAL``

       -  Unsupported capability. (不支持的能力。)


-----


open()(打开设备)
----------------

Synopsis(概要)
~~~~~~~~~~~~~~

.. code-block:: c

    #include <fcntl.h>

.. c:function:: int open(const char *deviceName, int flags)

Arguments(参数)
~~~~~~~~~~~~~~~

.. flat-table::
    :header-rows:  0
    :stub-columns: 0

    -  ..

       -  ``const char *deviceName``

       -  Name of specific OSD device. (特定 OSD 设备的名称。)

    -  ..

       -  :rspan:`3` ``int flags``

       -  :cspan:`1` A bit-wise OR of the following flags: (以下标志的按位或：)

    -  ..

       -  ``O_RDONLY``

       -  read-only access (只读访问)

    -  ..

       -  ``O_RDWR``

       -  read/write access (读写访问)

    -  ..

       -  ``O_NONBLOCK``
       -  | Open in non-blocking mode
          | (blocking mode is the default)
          | (以非阻塞模式打开（默认为阻塞模式）)

Description(描述)
~~~~~~~~~~~~~~~~~

This system call opens a named OSD device (e.g.
``/dev/dvb/adapter?/osd0``) for subsequent use.
(该系统调用打开一个命名的 OSD 设备（例如 ``/dev/dvb/adapter?/osd0``）以供后续使用。)

Return Value(返回值)
~~~~~~~~~~~~~~~~~~~~

.. flat-table::
    :header-rows:  0
    :stub-columns: 0

    -  ..

       -  ``ENODEV``

       -  Device driver not loaded/available. (设备驱动未加载/不可用。)

    -  ..

       -  ``EINTERNAL``

       -  Internal error. (内部错误。)

    -  ..

       -  ``EBUSY``

       -  Device or resource busy. (设备或资源忙。)

    -  ..

       -  ``EINVAL``

       -  Invalid argument. (无效的参数。)


-----


close()(关闭设备)
-----------------

Synopsis(概要)
~~~~~~~~~~~~~~

.. c:function:: int close(int fd)

Arguments(参数)
~~~~~~~~~~~~~~~

.. flat-table::
    :header-rows:  0
    :stub-columns: 0

    -  ..

       -  ``int fd``

       -  :cspan:`1` File descriptor returned by a previous call
          to `open()(打开设备)`_ . (由之前调用 `open()(打开设备)`_ 返回的文件描述符。)

Description(描述)
~~~~~~~~~~~~~~~~~

This system call closes a previously opened OSD device.
(该系统调用关闭之前打开的 OSD 设备。)

Return Value(返回值)
~~~~~~~~~~~~~~~~~~~~

.. flat-table::
    :header-rows:  0
    :stub-columns: 0

    -  ..

       -  ``EBADF``

       -  fd is not a valid open file descriptor. (fd 不是有效的已打开文件描述符。)
