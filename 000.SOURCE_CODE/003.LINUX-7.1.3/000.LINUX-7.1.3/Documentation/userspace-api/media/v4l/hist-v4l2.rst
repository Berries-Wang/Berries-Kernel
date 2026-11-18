.. SPDX-License-Identifier: GFDL-1.1-no-invariants-or-later
.. c:namespace:: V4L

.. _hist-v4l2:

****************************************
Changes of the V4L2 API(V4L2 API 的变迁)
****************************************

Soon after the V4L API was added to the kernel it was criticised as too
inflexible. In August 1998 Bill Dirks proposed a number of improvements
and began to work on documentation, example drivers and applications.
With the help of other volunteers this eventually became the V4L2 API,
not just an extension but a replacement for the V4L API. However it took
another four years and two stable kernel releases until the new API was
finally accepted for inclusion into the kernel in its present form.
(V4L API 加入内核后不久，就有人批评它过于僵化。1998 年 8 月，Bill Dirks 提出了一系列改进建议，并开始着手编写文档、示例驱动和应用程序。在其他志愿者的帮助下，这些工作最终演变成 V4L2 API——它不只是对 V4L API 的扩展，而是取而代之。然而，又过了四年、经历了两个稳定内核版本之后，新 API 才最终以现在的形式被接受并入内核。)

Early Versions(早期版本)
========================

1998-08-20: First version.
(1998-08-20：第一个版本。)

1998-08-27: The :c:func:`select()` function was introduced.
(1998-08-27：引入了 :c:func:`select()` 函数。)

1998-09-10: New video standard interface.
(1998-09-10：新的视频标准接口。)

1998-09-18: The ``VIDIOC_NONCAP`` ioctl was replaced by the otherwise
meaningless ``O_TRUNC`` :c:func:`open()` flag, and the
aliases ``O_NONCAP`` and ``O_NOIO`` were defined. Applications can set
this flag if they intend to access controls only, as opposed to capture
applications which need exclusive access. The ``VIDEO_STD_XXX``
identifiers are now ordinals instead of flags, and the
``video_std_construct()`` helper function takes id and
transmission arguments.
(1998-09-18：``VIDIOC_NONCAP`` ioctl 被本身并无其他含义的 ``O_TRUNC`` :c:func:`open()` 标志取代，并定义了别名 ``O_NONCAP`` 和 ``O_NOIO``。如果应用程序只想访问控件而不需要独占访问，可以设置这个标志；需要独占访问的捕获应用程序则不然。``VIDEO_STD_XXX`` 标识符现在是序号而不是标志，``video_std_construct()`` 辅助函数接受 id 和传输参数。)

1998-09-28: Revamped video standard. Made video controls individually
enumerable.
(1998-09-28：改进了视频标准。使视频控件可以单独枚举。)

1998-10-02: The ``id`` field was removed from
struct ``video_standard`` and the color subcarrier fields were
renamed. The :ref:`VIDIOC_QUERYSTD` ioctl was
renamed to :ref:`VIDIOC_ENUMSTD`,
:ref:`VIDIOC_G_INPUT <VIDIOC_G_INPUT>` to
:ref:`VIDIOC_ENUMINPUT`. A first draft of the
Codec API was released.
(1998-10-02：从 struct ``video_standard`` 中删除了 ``id`` 字段，并重命名了彩色副载波字段。:ref:`VIDIOC_QUERYSTD` ioctl 改名为 :ref:`VIDIOC_ENUMSTD`，:ref:`VIDIOC_G_INPUT <VIDIOC_G_INPUT>` 改名为 :ref:`VIDIOC_ENUMINPUT`。发布了 Codec API 的第一份草案。)

1998-11-08: Many minor changes. Most symbols have been renamed. Some
material changes to struct v4l2_capability.
(1998-11-08：许多小改动。大多数符号都改了名。struct v4l2_capability 有一些实质性修改。)

1998-11-12: The read/write direction of some ioctls was misdefined.
(1998-11-12：一些 ioctl 的读/写方向定义有误。)

1998-11-14: ``V4L2_PIX_FMT_RGB24`` changed to ``V4L2_PIX_FMT_BGR24``,
and ``V4L2_PIX_FMT_RGB32`` changed to ``V4L2_PIX_FMT_BGR32``. Audio
controls are now accessible with the
:ref:`VIDIOC_G_CTRL <VIDIOC_G_CTRL>` and
:ref:`VIDIOC_S_CTRL <VIDIOC_G_CTRL>` ioctls under names starting
with ``V4L2_CID_AUDIO``. The ``V4L2_MAJOR`` define was removed from
``videodev.h`` since it was only used once in the ``videodev`` kernel
module. The ``YUV422`` and ``YUV411`` planar image formats were added.
(1998-11-14：``V4L2_PIX_FMT_RGB24`` 改为 ``V4L2_PIX_FMT_BGR24``，``V4L2_PIX_FMT_RGB32`` 改为 ``V4L2_PIX_FMT_BGR32``。音频控件现在可以通过以 ``V4L2_CID_AUDIO`` 开头的名字，用 :ref:`VIDIOC_G_CTRL <VIDIOC_G_CTRL>` 和 :ref:`VIDIOC_S_CTRL <VIDIOC_G_CTRL>` ioctl 访问。``V4L2_MAJOR`` 定义从 ``videodev.h`` 中删除了，因为它在 ``videodev`` 内核模块中只用了一次。新增了 ``YUV422`` 和 ``YUV411`` 平面图像格式。)

1998-11-28: A few ioctl symbols changed. Interfaces for codecs and video
output devices were added.
(1998-11-28：若干 ioctl 符号变更。新增了编解码器和视频输出设备的接口。)

1999-01-14: A raw VBI capture interface was added.
(1999-01-14：新增了原始 VBI 捕获接口。)

1999-01-19: The ``VIDIOC_NEXTBUF`` ioctl was removed.
(1999-01-19：删除了 ``VIDIOC_NEXTBUF`` ioctl。)

V4L2 Version 0.16 1999-01-31(V4L2 版本 0.16，1999-01-31)
========================================================

1999-01-27: There is now one QBUF ioctl, VIDIOC_QWBUF and VIDIOC_QRBUF
are gone. VIDIOC_QBUF takes a v4l2_buffer as a parameter. Added
digital zoom (cropping) controls.
(1999-01-27：现在只剩一个 QBUF ioctl，VIDIOC_QWBUF 和 VIDIOC_QRBUF 已被移除。VIDIOC_QBUF 接受一个 v4l2_buffer 作为参数。新增了数字变焦 (裁剪) 控件。)

V4L2 Version 0.18 1999-03-16(V4L2 版本 0.18，1999-03-16)
========================================================

Added a v4l to V4L2 ioctl compatibility layer to videodev.c. Driver
writers, this changes how you implement your ioctl handler. See the
Driver Writer's Guide. Added some more control id codes.
(在 videodev.c 中加入了从 v4l 到 V4L2 的 ioctl 兼容层。驱动编写者们，这改变了你们实现 ioctl 处理函数的方式，参见《驱动编写者指南》。又增加了一些控件 ID。)

V4L2 Version 0.19 1999-06-05(V4L2 版本 0.19，1999-06-05)
========================================================

1999-03-18: Fill in the category and catname fields of v4l2_queryctrl
objects before passing them to the driver. Required a minor change to
the VIDIOC_QUERYCTRL handlers in the sample drivers.
(1999-03-18：在把 v4l2_queryctrl 对象传给驱动之前，要先填好其中的 category 和 catname 字段。这需要对示例驱动中的 VIDIOC_QUERYCTRL 处理函数做一点小改动。)

1999-03-31: Better compatibility for v4l memory capture ioctls. Requires
changes to drivers to fully support new compatibility features, see
Driver Writer's Guide and v4l2cap.c. Added new control IDs:
V4L2_CID_HFLIP, _VFLIP. Changed V4L2_PIX_FMT_YUV422P to _YUV422P,
and _YUV411P to _YUV411P.
(1999-03-31：改进了 v4l 内存捕获 ioctl 的兼容性。要完整支持新的兼容特性需要修改驱动，参见《驱动编写者指南》和 v4l2cap.c。新增控件 ID：V4L2_CID_HFLIP、_VFLIP。把 V4L2_PIX_FMT_YUV422P 改为 _YUV422P，_YUV411P 改为 _YUV411P。)

1999-04-04: Added a few more control IDs.
(1999-04-04：又增加了几个控件 ID。)

1999-04-07: Added the button control type.
(1999-04-07：新增了按钮控件类型。)

1999-05-02: Fixed a typo in videodev.h, and added the
V4L2_CTRL_FLAG_GRAYED (later V4L2_CTRL_FLAG_GRABBED) flag.
(1999-05-02：修正了 videodev.h 中的一个拼写错误，并新增了 V4L2_CTRL_FLAG_GRAYED (后来的 V4L2_CTRL_FLAG_GRABBED) 标志。)

1999-05-20: Definition of VIDIOC_G_CTRL was wrong causing a
malfunction of this ioctl.
(1999-05-20：VIDIOC_G_CTRL 的定义有误，导致该 ioctl 无法正常工作。)

1999-06-05: Changed the value of V4L2_CID_WHITENESS.
(1999-06-05：更改了 V4L2_CID_WHITENESS 的值。)

V4L2 Version 0.20 (1999-09-10)(V4L2 版本 0.20，1999-09-10)
==========================================================

Version 0.20 introduced a number of changes which were *not backward
compatible* with 0.19 and earlier versions. Purpose of these changes was
to simplify the API, while making it more extensible and following
common Linux driver API conventions.
(0.20 版引入了许多与 0.19 及更早版本*不向后兼容*的变更。这些变更的目的在于简化 API，同时使其更可扩展，并遵循常见的 Linux 驱动 API 惯例。)

1. Some typos in ``V4L2_FMT_FLAG`` symbols were fixed. struct v4l2_clip
   was changed for compatibility with v4l. (1999-08-30)
   (1. 修正了 ``V4L2_FMT_FLAG`` 符号中的一些拼写错误。为与 v4l 兼容，struct v4l2_clip 做了修改。(1999-08-30))

2. ``V4L2_TUNER_SUB_LANG1`` was added. (1999-09-05)
   (2. 新增了 ``V4L2_TUNER_SUB_LANG1``。(1999-09-05))

3. All ioctl() commands that used an integer argument now take a pointer
   to an integer. Where it makes sense, ioctls will return the actual
   new value in the integer pointed to by the argument, a common
   convention in the V4L2 API. The affected ioctls are: VIDIOC_PREVIEW,
   VIDIOC_STREAMON, VIDIOC_STREAMOFF, VIDIOC_S_FREQ,
   VIDIOC_S_INPUT, VIDIOC_S_OUTPUT, VIDIOC_S_EFFECT. For example
   (3. 所有原来以整数为参数的 ioctl() 命令，现在都改为接受指向整数的指针。在有意义的地方，ioctl 会通过参数所指向的整数返回实际的新值，这是 V4L2 API 的一个常见约定。受影响的 ioctl 有：VIDIOC_PREVIEW、VIDIOC_STREAMON、VIDIOC_STREAMOFF、VIDIOC_S_FREQ、VIDIOC_S_INPUT、VIDIOC_S_OUTPUT、VIDIOC_S_EFFECT。例如，)

   .. code-block:: c

       err = ioctl (fd, VIDIOC_XXX, V4L2_XXX);

   becomes
   (变为下面这样：)

   .. code-block:: c

       int a = V4L2_XXX; err = ioctl(fd, VIDIOC_XXX, &a);

4. All the different get- and set-format commands were swept into one
   :ref:`VIDIOC_G_FMT <VIDIOC_G_FMT>` and
   :ref:`VIDIOC_S_FMT <VIDIOC_G_FMT>` ioctl taking a union and a
   type field selecting the union member as parameter. Purpose is to
   simplify the API by eliminating several ioctls and to allow new and
   driver private data streams without adding new ioctls.
   (4. 所有各种不同的获取和设置格式命令，都归并成一个 :ref:`VIDIOC_G_FMT <VIDIOC_G_FMT>` 和 :ref:`VIDIOC_S_FMT <VIDIOC_G_FMT>` ioctl，它们以一个联合体和一个选择联合体成员的 type 字段为参数。目的在于通过去掉若干 ioctl 来简化 API，并且无需新增 ioctl 就能支持新的和驱动私有的数据流。)

   This change obsoletes the following ioctls: ``VIDIOC_S_INFMT``,
   ``VIDIOC_G_INFMT``, ``VIDIOC_S_OUTFMT``, ``VIDIOC_G_OUTFMT``,
   ``VIDIOC_S_VBIFMT`` and ``VIDIOC_G_VBIFMT``. The image format
   struct v4l2_format was renamed to struct v4l2_pix_format, while
   struct v4l2_format is now the enveloping structure
   for all format negotiations.
   (这一变更使得以下 ioctls 作废：``VIDIOC_S_INFMT``、``VIDIOC_G_INFMT``、``VIDIOC_S_OUTFMT``、``VIDIOC_G_OUTFMT``、``VIDIOC_S_VBIFMT`` 和 ``VIDIOC_G_VBIFMT``。图像格式 struct v4l2_format 改名为 struct v4l2_pix_format，而 struct v4l2_format 现在是包罗所有格式协商的外层结构。)

5. Similar to the changes above, the ``VIDIOC_G_PARM`` and
   ``VIDIOC_S_PARM`` ioctls were merged with ``VIDIOC_G_OUTPARM`` and
   ``VIDIOC_S_OUTPARM``. A ``type`` field in the new struct v4l2_streamparm
   selects the respective union member.
   (5. 与上面的变更类似，``VIDIOC_G_PARM`` 和 ``VIDIOC_S_PARM`` ioctl 与 ``VIDIOC_G_OUTPARM`` 和 ``VIDIOC_S_OUTPARM`` 合并了。新的 struct v4l2_streamparm 中用一个 ``type`` 字段选择相应的联合体成员。)

   This change obsoletes the ``VIDIOC_G_OUTPARM`` and
   ``VIDIOC_S_OUTPARM`` ioctls.
   (这一变更使得 ``VIDIOC_G_OUTPARM`` 和 ``VIDIOC_S_OUTPARM`` ioctls 作废。)

6. Control enumeration was simplified, and two new control flags were
   introduced and one dropped. The ``catname`` field was replaced by a
   ``group`` field.
   (6. 简化了控件枚举：引入了两个新的控件标志，并删除了一个。``catname`` 字段被 ``group`` 字段取代。)

   Drivers can now flag unsupported and temporarily unavailable controls
   with ``V4L2_CTRL_FLAG_DISABLED`` and ``V4L2_CTRL_FLAG_GRABBED``
   respectively. The ``group`` name indicates a possibly narrower
   classification than the ``category``. In other words, there may be
   multiple groups within a category. Controls within a group would
   typically be drawn within a group box. Controls in different
   categories might have a greater separation, or may even appear in
   separate windows.
   (现在，驱动可以分别用 ``V4L2_CTRL_FLAG_DISABLED`` 和 ``V4L2_CTRL_FLAG_GRABBED`` 标记不支持和暂时不可用的控件。``group`` 名字表示的分类可能比 ``category`` 更细。换言之，一个类别 (category) 中可以有多个组 (group)。同一组内的控件通常会被画在同一个组框里；不同类别中的控件则可能间隔更大，甚至出现在不同的窗口中。)

7. The struct v4l2_buffer ``timestamp`` was
   changed to a 64 bit integer, containing the sampling or output time
   of the frame in nanoseconds. Additionally timestamps will be in
   absolute system time, not starting from zero at the beginning of a
   stream. The data type name for timestamps is stamp_t, defined as a
   signed 64-bit integer. Output devices should not send a buffer out
   until the time in the timestamp field has arrived. I would like to
   follow SGI's lead, and adopt a multimedia timestamping system like
   their UST (Unadjusted System Time). See
   http://web.archive.org/web/\*/http://reality.sgi.com
   /cpirazzi_engr/lg/time/intro.html. UST uses timestamps that are
   64-bit signed integers (not struct timeval's) and given in nanosecond
   units. The UST clock starts at zero when the system is booted and
   runs continuously and uniformly. It takes a little over 292 years for
   UST to overflow. There is no way to set the UST clock. The regular
   Linux time-of-day clock can be changed periodically, which would
   cause errors if it were being used for timestamping a multimedia
   stream. A real UST style clock will require some support in the
   kernel that is not there yet. But in anticipation, I will change the
   timestamp field to a 64-bit integer, and I will change the
   v4l2_masterclock_gettime() function (used only by drivers) to
   return a 64-bit integer.
   (7. struct v4l2_buffer 的 ``timestamp`` 字段改成了一个 64 位整数，以纳秒为单位存放帧的采样或输出时间。此外，时间戳将采用绝对系统时间，而不是从流的开始从零起算。时间戳的数据类型名为 stamp_t，定义为有符号 64 位整数。输出设备在 timestamp 字段所指的时间到达之前不应把缓冲区送出去。我想效仿 SGI 的做法，采用一种类似其 UST (Unadjusted System Time，未调整系统时间) 的多媒体时间戳系统，参见 http://web.archive.org/web/\*/http://reality.sgi.com /cpirazzi_engr/lg/time/intro.html。UST 使用的时间戳是 64 位有符号整数 (不是 struct timeval)，以纳秒为单位。UST 时钟在系统引导时从零开始，连续而均匀地运行。UST 要经过 292 年多一点才会溢出。没有办法设置 UST 时钟。常规的 Linux 时间-of-日期时钟可以被周期性地修改，如果用它给多媒体流打时间戳就会造成错误。一个真正的 UST 风格时钟需要内核提供一些目前尚不存在的支持。但为了未雨绸缪，我会把 timestamp 字段改成 64 位整数，并把 v4l2_masterclock_gettime() 函数 (仅供驱动使用) 改为返回 64 位整数。)

8. A ``sequence`` field was added to struct v4l2_buffer. The ``sequence``
   field counts captured frames, it is ignored by output devices. When a
   capture driver drops a frame, the sequence number of that frame is skipped.
   (8. struct v4l2_buffer 中新增了 ``sequence`` 字段。``sequence`` 字段对捕获到的帧计数，输出设备会忽略它。当捕获驱动丢弃一帧时，该帧的序号就被跳过。)

V4L2 Version 0.20 incremental changes(V4L2 0.20 版的增量变更)
=============================================================

1999-12-23: In struct v4l2_vbi_format the
``reserved1`` field became ``offset``. Previously drivers were required
to clear the ``reserved1`` field.
(1999-12-23：在 struct v4l2_vbi_format 中，``reserved1`` 字段变成了 ``offset``。以前驱动必须清零 ``reserved1`` 字段。)

2000-01-13: The ``V4L2_FMT_FLAG_NOT_INTERLACED`` flag was added.
(2000-01-13：新增了 ``V4L2_FMT_FLAG_NOT_INTERLACED`` 标志。)

2000-07-31: The ``linux/poll.h`` header is now included by
``videodev.h`` for compatibility with the original ``videodev.h`` file.
(2000-07-31：为与最初的 ``videodev.h`` 文件兼容，``videodev.h`` 现在包含了 ``linux/poll.h`` 头文件。)

2000-11-20: ``V4L2_TYPE_VBI_OUTPUT`` and ``V4L2_PIX_FMT_Y41P`` were
added.
(2000-11-20：新增了 ``V4L2_TYPE_VBI_OUTPUT`` 和 ``V4L2_PIX_FMT_Y41P``。)

2000-11-25: ``V4L2_TYPE_VBI_INPUT`` was added.
(2000-11-25：新增了 ``V4L2_TYPE_VBI_INPUT``。)

2000-12-04: A couple typos in symbol names were fixed.
(2000-12-04：修正了几个符号名中的拼写错误。)

2001-01-18: To avoid namespace conflicts the ``fourcc`` macro defined in
the ``videodev.h`` header file was renamed to ``v4l2_fourcc``.
(2001-01-18：为避免命名空间冲突，``videodev.h`` 头文件中定义的 ``fourcc`` 宏改名为 ``v4l2_fourcc``。)

2001-01-25: A possible driver-level compatibility problem between the
``videodev.h`` file in Linux 2.4.0 and the ``videodev.h`` file included
in the ``videodevX`` patch was fixed. Users of an earlier version of
``videodevX`` on Linux 2.4.0 should recompile their V4L and V4L2
drivers.
(2001-01-25：修复了 Linux 2.4.0 中的 ``videodev.h`` 文件与 ``videodevX`` 补丁所带 ``videodev.h`` 文件之间一个可能的驱动级兼容性问题。在 Linux 2.4.0 上使用较早版本 ``videodevX`` 的用户应当重新编译他们的 V4L 和 V4L2 驱动。)

2001-01-26: A possible kernel-level incompatibility between the
``videodev.h`` file in the ``videodevX`` patch and the ``videodev.h``
file in Linux 2.2.x with devfs patches applied was fixed.
(2001-01-26：修复了 ``videodevX`` 补丁中的 ``videodev.h`` 文件与打了 devfs 补丁的 Linux 2.2.x 中的 ``videodev.h`` 文件之间一个可能的内核级不兼容问题。)

2001-03-02: Certain V4L ioctls which pass data in both direction
although they are defined with read-only parameter, did not work
correctly through the backward compatibility layer. [Solution?]
(2001-03-02：某些 V4L ioctl 虽然按只读参数定义，却在双向传递数据，它们经过向后兼容层时不能正确工作。[解决方案？])

2001-04-13: Big endian 16-bit RGB formats were added.
(2001-04-13：新增了大端 16 位 RGB 格式。)

2001-09-17: New YUV formats and the
:ref:`VIDIOC_G_FREQUENCY <VIDIOC_G_FREQUENCY>` and
:ref:`VIDIOC_S_FREQUENCY <VIDIOC_G_FREQUENCY>` ioctls were added.
(The old ``VIDIOC_G_FREQ`` and ``VIDIOC_S_FREQ`` ioctls did not take
multiple tuners into account.)
(2001-09-17：新增了若干 YUV 格式以及 :ref:`VIDIOC_G_FREQUENCY <VIDIOC_G_FREQUENCY>` 和 :ref:`VIDIOC_S_FREQUENCY <VIDIOC_G_FREQUENCY>` ioctls。(旧的 ``VIDIOC_G_FREQ`` 和 ``VIDIOC_S_FREQ`` ioctls 没有把多个调谐器考虑在内。)

2000-09-18: ``V4L2_BUF_TYPE_VBI`` was added. This may *break
compatibility* as the :ref:`VIDIOC_G_FMT <VIDIOC_G_FMT>` and
:ref:`VIDIOC_S_FMT <VIDIOC_G_FMT>` ioctls may fail now if the
struct ``v4l2_fmt`` ``type`` field does not contain
``V4L2_BUF_TYPE_VBI``. In the documentation of the struct v4l2_vbi_format`,
the ``offset`` field the ambiguous phrase "rising edge" was changed to
"leading edge".
(2000-09-18：新增了 ``V4L2_BUF_TYPE_VBI``。这可能会*破坏兼容性*：如果 struct ``v4l2_fmt`` 的 ``type`` 字段不包含 ``V4L2_BUF_TYPE_VBI``，:ref:`VIDIOC_G_FMT <VIDIOC_G_FMT>` 和 :ref:`VIDIOC_S_FMT <VIDIOC_G_FMT>` ioctls 现在可能会失败。在 struct v4l2_vbi_format 的文档中，``offset`` 字段里含糊的措辞 "rising edge" 改成了 "leading edge"。)

V4L2 Version 0.20 2000-11-23(V4L2 版本 0.20，2000-11-23)
========================================================

A number of changes were made to the raw VBI interface.
(对原始 VBI 接口做了若干修改。)

1. Figures clarifying the line numbering scheme were added to the V4L2
   API specification. The ``start``\ [0] and ``start``\ [1] fields no
   longer count line numbers beginning at zero. Rationale: a) The
   previous definition was unclear. b) The ``start``\ [] values are
   ordinal numbers. c) There is no point in inventing a new line
   numbering scheme. We now use line number as defined by ITU-R, period.
   Compatibility: Add one to the start values. Applications depending on
   the previous semantics may not function correctly.
   (1. V4L2 API 规范中新增了阐明行号编排方案的图示。``start``\ [0] 和 ``start``\ [1] 字段不再从零开始数行号。理由：a) 以前的定义不清晰；b) ``start``\ [] 的值是序数；c) 发明一套新的行号编排方案毫无意义。我们现在就使用 ITU-R 定义的行号，仅此而已。兼容性：给 start 值加一。依赖旧有语义的应用程序可能无法正确运行。)

2. The restriction "count[0] > 0 and count[1] > 0" has been relaxed to
   "(count[0] + count[1]) > 0". Rationale: Drivers may allocate
   resources at scan line granularity and some data services are
   transmitted only on the first field. The comment that both ``count``
   values will usually be equal is misleading and pointless and has been
   removed. This change *breaks compatibility* with earlier versions:
   Drivers may return ``EINVAL``, applications may not function correctly.
   (2. 约束条件 "count[0] > 0 and count[1] > 0" 已放宽为 "(count[0] + count[1]) > 0"。理由：驱动可能按扫描行粒度分配资源，而且有些数据业务只在第一个场传输。关于两个 ``count`` 值通常相等的注释有误导性又毫无意义，已被删除。这一变更*破坏了*与早期版本的兼容性：驱动可能返回 ``EINVAL``，应用程序可能无法正确运行。)

3. Drivers are again permitted to return negative (unknown) start values
   as proposed earlier. Why this feature was dropped is unclear. This
   change may *break compatibility* with applications depending on the
   start values being positive. The use of ``EBUSY`` and ``EINVAL``
   error codes with the :ref:`VIDIOC_S_FMT <VIDIOC_G_FMT>` ioctl was
   clarified. The ``EBUSY`` error code was finally documented, and the
   ``reserved2`` field which was previously mentioned only in the
   ``videodev.h`` header file.
   (3. 如先前的提议，现在再次允许驱动返回负的 (未知) start 值。当初为什么砍掉这个特性已不可考。这一变更可能会*破坏*与那些假定 start 值为正的应用程序的兼容性。明确了 :ref:`VIDIOC_S_FMT <VIDIOC_G_FMT>` ioctl 使用 ``EBUSY`` 和 ``EINVAL`` 错误码的情形。``EBUSY`` 错误码终于有了文档说明；``reserved2`` 字段 (此前只在 ``videodev.h`` 头文件里提过) 也补上了。)

4. New buffer types ``V4L2_TYPE_VBI_INPUT`` and ``V4L2_TYPE_VBI_OUTPUT``
   were added. The former is an alias for the old ``V4L2_TYPE_VBI``, the
   latter was missing in the ``videodev.h`` file.
   (4. 新增了缓冲区类型 ``V4L2_TYPE_VBI_INPUT`` 和 ``V4L2_TYPE_VBI_OUTPUT``。前者是旧的 ``V4L2_TYPE_VBI`` 的别名，后者在 ``videodev.h`` 文件中一直缺失。)

V4L2 Version 0.20 2002-07-25(V4L2 版本 0.20，2002-07-25)
========================================================

Added sliced VBI interface proposal.
(加入了切割 VBI (sliced VBI) 接口提案。)

V4L2 in Linux 2.5.46, 2002-10(并入 Linux 2.5.46 的 V4L2，2002-10)
=================================================================

Around October-November 2002, prior to an announced feature freeze of
Linux 2.5, the API was revised, drawing from experience with V4L2 0.20.
This unnamed version was finally merged into Linux 2.5.46.
(2002 年 10-11 月前后，在宣布 Linux 2.5 特性冻结之前，吸取 V4L2 0.20 的经验教训，对 API 进行了一次修订。这个未命名版本最终并入了 Linux 2.5.46。)

1.  As specified in :ref:`related`, drivers must make related device
    functions available under all minor device numbers.
    (1. 按照 :ref:`related` 的规定，驱动必须在所有次设备号下都提供相关设备功能。)

2.  The :c:func:`open()` function requires access mode
    ``O_RDWR`` regardless of the device type. All V4L2 drivers
    exchanging data with applications must support the ``O_NONBLOCK``
    flag. The ``O_NOIO`` flag, a V4L2 symbol which aliased the
    meaningless ``O_TRUNC`` to indicate accesses without data exchange
    (panel applications) was dropped. Drivers must stay in "panel mode"
    until the application attempts to initiate a data exchange, see
    :ref:`open`.
    (2. :c:func:`open()` 函数要求访问模式为 ``O_RDWR``，与设备类型无关。所有与应用程序交换数据的 V4L2 驱动都必须支持 ``O_NONBLOCK`` 标志。``O_NOIO`` 标志 (一个 V4L2 符号，用本身无意义的 ``O_TRUNC`` 的别名来表示无数据交换的访问，即面板类应用) 被删除。在应用程序尝试发起数据交换之前，驱动必须保持在"面板模式"，参见 :ref:`open`。)

3.  The struct v4l2_capability changed
    dramatically. Note that also the size of the structure changed,
    which is encoded in the ioctl request code, thus older V4L2 devices
    will respond with an ``EINVAL`` error code to the new
    :ref:`VIDIOC_QUERYCAP` ioctl.
    (3. struct v4l2_capability 发生了巨大变化。注意结构体的大小也变了，而它被编码在 ioctl 请求码中，因此较旧的 V4L2 设备对新的 :ref:`VIDIOC_QUERYCAP` ioctl 会以 ``EINVAL`` 错误码应答。)

    There are new fields to identify the driver, a new RDS device
    function ``V4L2_CAP_RDS_CAPTURE``, the ``V4L2_CAP_AUDIO`` flag
    indicates if the device has any audio connectors, another I/O
    capability V4L2_CAP_ASYNCIO can be flagged. In response to these
    changes the ``type`` field became a bit set and was merged into the
    ``flags`` field. ``V4L2_FLAG_TUNER`` was renamed to
    ``V4L2_CAP_TUNER``, ``V4L2_CAP_VIDEO_OVERLAY`` replaced
    ``V4L2_FLAG_PREVIEW`` and ``V4L2_CAP_VBI_CAPTURE`` and
    ``V4L2_CAP_VBI_OUTPUT`` replaced ``V4L2_FLAG_DATA_SERVICE``.
    ``V4L2_FLAG_READ`` and ``V4L2_FLAG_WRITE`` were merged into
    ``V4L2_CAP_READWRITE``.
    (新增了用于识别驱动的字段；新增 RDS 设备功能 ``V4L2_CAP_RDS_CAPTURE``；``V4L2_CAP_AUDIO`` 标志指示设备是否有任何音频连接器；另一个 I/O 能力 V4L2_CAP_ASYNCIO 也可以被置位。为响应这些变化，``type`` 字段变成了一个位集合，并合并进 ``flags`` 字段。``V4L2_FLAG_TUNER`` 改名为 ``V4L2_CAP_TUNER``；``V4L2_CAP_VIDEO_OVERLAY`` 取代了 ``V4L2_FLAG_PREVIEW``；``V4L2_CAP_VBI_CAPTURE`` 和 ``V4L2_CAP_VBI_OUTPUT`` 取代了 ``V4L2_FLAG_DATA_SERVICE``。``V4L2_FLAG_READ`` 和 ``V4L2_FLAG_WRITE`` 合并为 ``V4L2_CAP_READWRITE``。)

    The redundant fields ``inputs``, ``outputs`` and ``audios`` were
    removed. These properties can be determined as described in
    :ref:`video` and :ref:`audio`.
    (冗余字段 ``inputs``、``outputs`` 和 ``audios`` 被删除了。这些属性可以按 :ref:`video` 和 :ref:`audio` 中描述的方法确定。)

    The somewhat volatile and therefore barely useful fields
    ``maxwidth``, ``maxheight``, ``minwidth``, ``minheight``,
    ``maxframerate`` were removed. This information is available as
    described in :ref:`format` and :ref:`standard`.
    (有些易变、因而几乎没有用处的字段 ``maxwidth``、``maxheight``、``minwidth``、``minheight``、``maxframerate`` 被删除了。这些信息可按 :ref:`format` 和 :ref:`standard` 中的描述获得。)

    ``V4L2_FLAG_SELECT`` was removed. We believe the select() function
    is important enough to require support of it in all V4L2 drivers
    exchanging data with applications. The redundant
    ``V4L2_FLAG_MONOCHROME`` flag was removed, this information is
    available as described in :ref:`format`.
    (``V4L2_FLAG_SELECT`` 被删除了。我们认为 select() 函数足够重要，值得要求所有与应用程序交换数据的 V4L2 驱动都支持它。冗余的 ``V4L2_FLAG_MONOCHROME`` 标志被删除了，这一信息可按 :ref:`format` 中的描述获得。)

4.  In struct v4l2_input the ``assoc_audio``
    field and the ``capability`` field and its only flag
    ``V4L2_INPUT_CAP_AUDIO`` was replaced by the new ``audioset`` field.
    Instead of linking one video input to one audio input this field
    reports all audio inputs this video input combines with.
    (4. 在 struct v4l2_input 中，``assoc_audio`` 字段、``capability`` 字段及其唯一标志 ``V4L2_INPUT_CAP_AUDIO`` 被新的 ``audioset`` 字段取代。这个字段不再是把一个视频输入链接到一个音频输入，而是报告该视频输入可与哪些音频输入组合。)

    New fields are ``tuner`` (reversing the former link from tuners to
    video inputs), ``std`` and ``status``.
    (新字段有 ``tuner`` (颠倒了原来从调谐器到视频输入的链接方向)、``std`` 和 ``status``。)

    Accordingly struct v4l2_output lost its
    ``capability`` and ``assoc_audio`` fields. ``audioset``,
    ``modulator`` and ``std`` where added instead.
    (相应地，struct v4l2_output 失去了 ``capability`` 和 ``assoc_audio`` 字段，代之以新增的 ``audioset``、``modulator`` 和 ``std``。)

5.  The struct v4l2_audio field ``audio`` was
    renamed to ``index``, for consistency with other structures. A new
    capability flag ``V4L2_AUDCAP_STEREO`` was added to indicated if the
    audio input in question supports stereo sound.
    ``V4L2_AUDCAP_EFFECTS`` and the corresponding ``V4L2_AUDMODE`` flags
    where removed. This can be easily implemented using controls.
    (However the same applies to AVL which is still there.)
    (5. 为与其他结构体保持一致，struct v4l2_audio 的 ``audio`` 字段改名为 ``index``。新增能力标志 ``V4L2_AUDCAP_STEREO``，用于指示所讨论的音频输入是否支持立体声。``V4L2_AUDCAP_EFFECTS`` 及对应的 ``V4L2_AUDMODE`` 标志被删除了——用控件很容易实现同样的功能。(不过同样适用于仍然存在的 AVL。))

    Again for consistency the struct v4l2_audioout field ``audio`` was renamed
    to ``index``.
    (同样为保持一致，struct v4l2_audioout 的 ``audio`` 字段也改名为 ``index``。)

6.  The struct v4l2_tuner ``input`` field was
    replaced by an ``index`` field, permitting devices with multiple
    tuners. The link between video inputs and tuners is now reversed,
    inputs point to their tuner. The ``std`` substructure became a
    simple set (more about this below) and moved into struct v4l2_input.
    A ``type`` field was added.
    (6. struct v4l2_tuner 的 ``input`` 字段被 ``index`` 字段取代，从而允许设备有多个调谐器。视频输入与调谐器之间的链接现在反了过来：输入指向自己的调谐器。``std`` 子结构变成了一个简单集合 (下文详述)，并移入 struct v4l2_input。新增了一个 ``type`` 字段。)

    Accordingly in struct v4l2_modulator the
    ``output`` was replaced by an ``index`` field.
    (相应地，struct v4l2_modulator 的 ``output`` 被 ``index`` 字段取代。)

    In struct v4l2_frequency the ``port``
    field was replaced by a ``tuner`` field containing the respective
    tuner or modulator index number. A tuner ``type`` field was added
    and the ``reserved`` field became larger for future extensions
    (satellite tuners in particular).
    (在 struct v4l2_frequency 中，``port`` 字段被 ``tuner`` 字段取代，后者存放相应调谐器或调制器的索引号。新增了调谐器 ``type`` 字段；``reserved`` 字段变大了一些，为将来的扩展 (尤其是卫星调谐器) 留出余地。)

7.  The idea of completely transparent video standards was dropped.
    Experience showed that applications must be able to work with video
    standards beyond presenting the user a menu. Instead of enumerating
    supported standards with an ioctl applications can now refer to
    standards by :ref:`v4l2_std_id <v4l2-std-id>` and symbols
    defined in the ``videodev2.h`` header file. For details see
    :ref:`standard`. The :ref:`VIDIOC_G_STD <VIDIOC_G_STD>` and
    :ref:`VIDIOC_S_STD <VIDIOC_G_STD>` now take a pointer to this
    type as argument. :ref:`VIDIOC_QUERYSTD` was
    added to autodetect the received standard, if the hardware has this
    capability. In struct v4l2_standard an
    ``index`` field was added for
    :ref:`VIDIOC_ENUMSTD`. A
    :ref:`v4l2_std_id <v4l2-std-id>` field named ``id`` was added as
    machine readable identifier, also replacing the ``transmission``
    field. The misleading ``framerate`` field was renamed to
    ``frameperiod``. The now obsolete ``colorstandard`` information,
    originally needed to distguish between variations of standards, were
    removed.
    (7. 完全透明的视频标准这一想法被放弃了。经验表明，应用程序必须能够处理视频标准，而不只是给用户摆出一个菜单。现在应用程序不再用 ioctl 枚举受支持的标准，而是可以通过 :ref:`v4l2_std_id <v4l2-std-id>` 和 ``videodev2.h`` 头文件中定义的符号来引用标准。详情见 :ref:`standard`。:ref:`VIDIOC_G_STD <VIDIOC_G_STD>` 和 :ref:`VIDIOC_S_STD <VIDIOC_G_STD>` 现在接受指向该类型的指针作为参数。新增 :ref:`VIDIOC_QUERYSTD`，在硬件具备此能力时自动检测收到的标准。struct v4l2_standard 中为 :ref:`VIDIOC_ENUMSTD` 新增了 ``index`` 字段。新增了名为 ``id`` 的 :ref:`v4l2_std_id <v4l2-std-id>` 字段作为机器可读的标识符，同时取代了 ``transmission`` 字段。有误导性的 ``framerate`` 字段改名为 ``frameperiod``。如今已过时的 ``colorstandard`` 信息 (当初用于区分标准的各种变体) 被删除了。")

    Struct ``v4l2_enumstd`` ceased to be.
    :ref:`VIDIOC_ENUMSTD` now takes a pointer to a
    struct v4l2_standard directly. The
    information which standards are supported by a particular video
    input or output moved into struct v4l2_input
    and struct v4l2_output fields named ``std``,
    respectively.
    (Struct ``v4l2_enumstd`` 不复存在。:ref:`VIDIOC_ENUMSTD` 现在直接接受指向 struct v4l2_standard 的指针。某个特定视频输入或输出支持哪些标准的信息，分别移入 struct v4l2_input 和 struct v4l2_output 的 ``std`` 字段。)

8.  The struct :ref:`v4l2_queryctrl <v4l2-queryctrl>` fields
    ``category`` and ``group`` did not catch on and/or were not
    implemented as expected and therefore removed.
    (8. struct :ref:`v4l2_queryctrl <v4l2-queryctrl>` 的 ``category`` 和 ``group`` 字段没有流行起来，或者没有按预期实现，因此被删除了。)

9.  The :ref:`VIDIOC_TRY_FMT <VIDIOC_G_FMT>` ioctl was added to
    negotiate data formats as with
    :ref:`VIDIOC_S_FMT <VIDIOC_G_FMT>`, but without the overhead of
    programming the hardware and regardless of I/O in progress.
    (9. 新增 :ref:`VIDIOC_TRY_FMT <VIDIOC_G_FMT>` ioctl，用于像 :ref:`VIDIOC_S_FMT <VIDIOC_G_FMT>` 那样协商数据格式，但没有编程硬件的开销，也与进行中的 I/O 无关。)

    In struct v4l2_format the ``fmt`` union was
    extended to contain struct v4l2_window. All
    image format negotiations are now possible with ``VIDIOC_G_FMT``,
    ``VIDIOC_S_FMT`` and ``VIDIOC_TRY_FMT``; ioctl. The ``VIDIOC_G_WIN``
    and ``VIDIOC_S_WIN`` ioctls to prepare for a video overlay were
    removed. The ``type`` field changed to type enum v4l2_buf_type and
    the buffer type names changed as follows.
    (在 struct v4l2_format 中，``fmt`` 联合体被扩展为可包含 struct v4l2_window。现在所有的图像格式协商都可以用 ``VIDIOC_G_FMT``、``VIDIOC_S_FMT`` 和 ``VIDIOC_TRY_FMT``；ioctl 完成。为视频叠加做准备的 ``VIDIOC_G_WIN`` 和 ``VIDIOC_S_WIN`` ioctls 被删除了。``type`` 字段改为 enum v4l2_buf_type 类型，缓冲区类型名称的变化如下表。")


    .. flat-table::
	:header-rows:  1
	:stub-columns: 0

	* - Old defines(旧定义)
	  - enum v4l2_buf_type(枚举类型)
	* - ``V4L2_BUF_TYPE_CAPTURE``
	  - ``V4L2_BUF_TYPE_VIDEO_CAPTURE``
	* - ``V4L2_BUF_TYPE_CODECIN``
	  - Omitted for now
	* - ``V4L2_BUF_TYPE_CODECOUT``
	  - Omitted for now
	* - ``V4L2_BUF_TYPE_EFFECTSIN``
	  - Omitted for now
	* - ``V4L2_BUF_TYPE_EFFECTSIN2``
	  - Omitted for now
	* - ``V4L2_BUF_TYPE_EFFECTSOUT``
	  - Omitted for now
	* - ``V4L2_BUF_TYPE_VIDEOOUT``
	  - ``V4L2_BUF_TYPE_VIDEO_OUTPUT``
	* - ``-``
	  - ``V4L2_BUF_TYPE_VIDEO_OVERLAY``
	* - ``-``
	  - ``V4L2_BUF_TYPE_VBI_CAPTURE``
	* - ``-``
	  - ``V4L2_BUF_TYPE_VBI_OUTPUT``
	* - ``-``
	  - ``V4L2_BUF_TYPE_SLICED_VBI_CAPTURE``
	* - ``-``
	  - ``V4L2_BUF_TYPE_SLICED_VBI_OUTPUT``
	* - ``V4L2_BUF_TYPE_PRIVATE_BASE``
	  - ``V4L2_BUF_TYPE_PRIVATE`` (but this is deprecated)

10. In struct v4l2_fmtdesc a enum v4l2_buf_type field named ``type`` was
    added as in struct v4l2_format. The ``VIDIOC_ENUM_FBUFFMT`` ioctl is no
    longer needed and was removed. These calls can be replaced by
    :ref:`VIDIOC_ENUM_FMT` with type ``V4L2_BUF_TYPE_VIDEO_OVERLAY``.
    (10. 在 struct v4l2_fmtdesc 中，像 struct v4l2_format 一样新增了名为 ``type`` 的 enum v4l2_buf_type 字段。``VIDIOC_ENUM_FBUFFMT`` ioctl 不再需要，已被删除。这些调用可以换成带类型 ``V4L2_BUF_TYPE_VIDEO_OVERLAY`` 的 :ref:`VIDIOC_ENUM_FMT`。)

11. In struct v4l2_pix_format the ``depth``
    field was removed, assuming applications which recognize the format
    by its four-character-code already know the color depth, and others
    do not care about it. The same rationale lead to the removal of the
    ``V4L2_FMT_FLAG_COMPRESSED`` flag. The
    ``V4L2_FMT_FLAG_SWCONVECOMPRESSED`` flag was removed because drivers
    are not supposed to convert images in kernel space. A user library
    of conversion functions should be provided instead. The
    ``V4L2_FMT_FLAG_BYTESPERLINE`` flag was redundant. Applications can
    set the ``bytesperline`` field to zero to get a reasonable default.
    Since the remaining flags were replaced as well, the ``flags`` field
    itself was removed.
    (11. struct v4l2_pix_format 的 ``depth`` 字段被删除了——假定那些通过四字符码识别格式的应用程序已经知道色深，而其余应用程序并不在乎它。同样的理由也促成了 ``V4L2_FMT_FLAG_COMPRESSED`` 标志的删除。``V4L2_FMT_FLAG_SWCONVECOMPRESSED`` 标志被删除，因为驱动不应该在内核空间做图像转换，而应改为提供一套用户空间的转换函数库。``V4L2_FMT_FLAG_BYTESPERLINE`` 标志是冗余的：应用程序把 ``bytesperline`` 字段设为零即可得到合理的默认值。既然其余标志也都被替换掉了，``flags`` 字段本身也就被删除了。)

    The interlace flags were replaced by a enum v4l2_field value in a
    newly added ``field`` field.
    (隔行扫描标志被新加的 ``field`` 字段中的 enum v4l2_field 值取代。)

    .. flat-table::
	:header-rows:  1
	:stub-columns: 0

	* - Old flag(旧标志)
	  - enum v4l2_field(枚举类型)
	* - ``V4L2_FMT_FLAG_NOT_INTERLACED``
	  - ?
	* - ``V4L2_FMT_FLAG_INTERLACED`` = ``V4L2_FMT_FLAG_COMBINED``
	  - ``V4L2_FIELD_INTERLACED``
	* - ``V4L2_FMT_FLAG_TOPFIELD`` = ``V4L2_FMT_FLAG_ODDFIELD``
	  - ``V4L2_FIELD_TOP``
	* - ``V4L2_FMT_FLAG_BOTFIELD`` = ``V4L2_FMT_FLAG_EVENFIELD``
	  - ``V4L2_FIELD_BOTTOM``
	* - ``-``
	  - ``V4L2_FIELD_SEQ_TB``
	* - ``-``
	  - ``V4L2_FIELD_SEQ_BT``
	* - ``-``
	  - ``V4L2_FIELD_ALTERNATE``

    The color space flags were replaced by a enum v4l2_colorspace value in
    a newly added ``colorspace`` field, where one of
    ``V4L2_COLORSPACE_SMPTE170M``, ``V4L2_COLORSPACE_BT878``,
    ``V4L2_COLORSPACE_470_SYSTEM_M`` or
    ``V4L2_COLORSPACE_470_SYSTEM_BG`` replaces ``V4L2_FMT_CS_601YUV``.
    (色彩空间标志被新加的 ``colorspace`` 字段中的 enum v4l2_colorspace 值取代，其中 ``V4L2_COLORSPACE_SMPTE170M``、``V4L2_COLORSPACE_BT878``、``V4L2_COLORSPACE_470_SYSTEM_M`` 或 ``V4L2_COLORSPACE_470_SYSTEM_BG`` 之一取代 ``V4L2_FMT_CS_601YUV``。)

12. In struct v4l2_requestbuffers the
    ``type`` field was properly defined as enum v4l2_buf_type. Buffer types
    changed as mentioned above. A new ``memory`` field of type
    enum v4l2_memory was added to distinguish between
    I/O methods using buffers allocated by the driver or the
    application. See :ref:`io` for details.
    (12. struct v4l2_requestbuffers 的 ``type`` 字段被正确定义为 enum v4l2_buf_type。缓冲区类型如上文所述发生了变化。新增了 enum v4l2_memory 类型的 ``memory`` 字段，用于区分使用驱动分配还是应用程序分配缓冲区的 I/O 方法。详见 :ref:`io`。)

13. In struct v4l2_buffer the ``type`` field was
    properly defined as enum v4l2_buf_type.
    Buffer types changed as mentioned above. A ``field`` field of type
    enum v4l2_field was added to indicate if a
    buffer contains a top or bottom field. The old field flags were
    removed. Since no unadjusted system time clock was added to the
    kernel as planned, the ``timestamp`` field changed back from type
    stamp_t, an unsigned 64 bit integer expressing the sample time in
    nanoseconds, to struct timeval. With the addition
    of a second memory mapping method the ``offset`` field moved into
    union ``m``, and a new ``memory`` field of type enum v4l2_memory
    was added to distinguish between
    I/O methods. See :ref:`io` for details.
    (13. struct v4l2_buffer 的 ``type`` 字段被正确定义为 enum v4l2_buf_type。缓冲区类型如上文所述发生了变化。新增了 enum v4l2_field 类型的 ``field`` 字段，用于指示缓冲区包含的是顶场还是底场。旧的场标志被删除了。由于计划中的未调整系统时间时钟并未加入内核，``timestamp`` 字段又从 stamp_t 类型 (以纳秒表示采样时间的无符号 64 位整数) 改回了 struct timeval。随着第二种内存映射方法的加入，``offset`` 字段移入联合体 ``m``，并新增了 enum v4l2_memory 类型的 ``memory`` 字段来区分各 I/O 方法。详见 :ref:`io`。)

    The ``V4L2_BUF_REQ_CONTIG`` flag was used by the V4L compatibility
    layer, after changes to this code it was no longer needed. The
    ``V4L2_BUF_ATTR_DEVICEMEM`` flag would indicate if the buffer was
    indeed allocated in device memory rather than DMA-able system
    memory. It was barely useful and so was removed.
    (``V4L2_BUF_REQ_CONTIG`` 标志曾被 V4L 兼容层使用，这段代码修改之后它就不需要了。``V4L2_BUF_ATTR_DEVICEMEM`` 标志本应指示缓冲区确实分配在设备内存中而非可 DMA 的系统内存中。它几乎没什么用，于是也被删除了。)

14. In struct v4l2_framebuffer the
    ``base[3]`` array anticipating double- and triple-buffering in
    off-screen video memory, however without defining a synchronization
    mechanism, was replaced by a single pointer. The
    ``V4L2_FBUF_CAP_SCALEUP`` and ``V4L2_FBUF_CAP_SCALEDOWN`` flags were
    removed. Applications can determine this capability more accurately
    using the new cropping and scaling interface. The
    ``V4L2_FBUF_CAP_CLIPPING`` flag was replaced by
    ``V4L2_FBUF_CAP_LIST_CLIPPING`` and
    ``V4L2_FBUF_CAP_BITMAP_CLIPPING``.
    (14. struct v4l2_framebuffer 的 ``base[3]`` 数组，本想为屏外显存中的双缓冲和三缓冲预留，却没有定义同步机制，被单个指针取代。``V4L2_FBUF_CAP_SCALEUP`` 和 ``V4L2_FBUF_CAP_SCALEDOWN`` 标志被删除了，应用程序可以用新的裁剪和缩放接口更准确地判断这一能力。``V4L2_FBUF_CAP_CLIPPING`` 标志被 ``V4L2_FBUF_CAP_LIST_CLIPPING`` 和 ``V4L2_FBUF_CAP_BITMAP_CLIPPING`` 取代。)

15. In struct v4l2_clip the ``x``, ``y``,
    ``width`` and ``height`` field moved into a ``c`` substructure of
    type struct v4l2_rect. The ``x`` and ``y``
    fields were renamed to ``left`` and ``top``, i. e. offsets to a
    context dependent origin.
    (15. 在 struct v4l2_clip 中，``x``、``y``、``width`` 和 ``height`` 字段移入 struct v4l2_rect 类型的 ``c`` 子结构。``x`` 和 ``y`` 字段改名为 ``left`` 和 ``top``，即相对于某个与上下文相关的原点的偏移。)

16. In struct v4l2_window the ``x``, ``y``,
    ``width`` and ``height`` field moved into a ``w`` substructure as
    above. A ``field`` field of type enum v4l2_field was added to
    distinguish between field and frame (interlaced) overlay.
    (16. 在 struct v4l2_window 中，``x``、``y``、``width`` 和 ``height`` 字段同样移入一个 ``w`` 子结构。新增了 enum v4l2_field 类型的 ``field`` 字段，用于区分场叠加与帧 (隔行) 叠加。)

17. The digital zoom interface, including struct ``v4l2_zoomcap``,
    struct ``v4l2_zoom``, ``V4L2_ZOOM_NONCAP`` and
    ``V4L2_ZOOM_WHILESTREAMING`` was replaced by a new cropping and
    scaling interface. The previously unused
    struct v4l2_cropcap and struct v4l2_crop
    where redefined for this purpose. See :ref:`crop` for details.
    (17. 数字变焦接口——包括 struct ``v4l2_zoomcap``、struct ``v4l2_zoom``、``V4L2_ZOOM_NONCAP`` 和 ``V4L2_ZOOM_WHILESTREAMING``——被新的裁剪和缩放接口取代。先前未被使用的 struct v4l2_cropcap 和 struct v4l2_crop 为此被重新定义。详见 :ref:`crop`。)

18. In struct v4l2_vbi_format the
    ``SAMPLE_FORMAT`` field now contains a four-character-code as used
    to identify video image formats and ``V4L2_PIX_FMT_GREY`` replaces
    the ``V4L2_VBI_SF_UBYTE`` define. The ``reserved`` field was
    extended.
    (18. 在 struct v4l2_vbi_format 中，``SAMPLE_FORMAT`` 字段现在存放一个四字符码 (与标识视频图像格式所用的一致)，``V4L2_PIX_FMT_GREY`` 取代了 ``V4L2_VBI_SF_UBYTE`` 定义。``reserved`` 字段被扩展了。)

19. In struct v4l2_captureparm the type of
    the ``timeperframe`` field changed from unsigned long to
    struct v4l2_fract. This allows the accurate
    expression of multiples of the NTSC-M frame rate 30000 / 1001. A new
    field ``readbuffers`` was added to control the driver behaviour in
    read I/O mode.
    (19. struct v4l2_captureparm 的 ``timeperframe`` 字段类型从 unsigned long 改为 struct v4l2_fract。这样可以准确表达 NTSC-M 帧率 30000 / 1001 的倍数。新增了 ``readbuffers`` 字段，用于控制驱动在读 I/O 模式下的行为。)

    Similar changes were made to struct v4l2_outputparm.
    (struct v4l2_outputparm 也做了类似的修改。)

20. The struct ``v4l2_performance`` and
    ``VIDIOC_G_PERF`` ioctl were dropped. Except when using the
    :ref:`read/write I/O method <rw>`, which is limited anyway, this
    information is already available to applications.
    (20. struct ``v4l2_performance`` 和 ``VIDIOC_G_PERF`` ioctl 被砍掉了。除了本来就很受限的 :ref:`read/write I/O 方法 <rw>` 之外，这些信息应用程序本来就能获得。)

21. The example transformation from RGB to YCbCr color space in the old
    V4L2 documentation was inaccurate, this has been corrected in
    :ref:`pixfmt`.
    (21. 旧 V4L2 文档中 RGB 到 YCbCr 色彩空间的示例变换不准确，已在 :ref:`pixfmt` 中更正。)

V4L2 2003-06-19(V4L2，2003-06-19)
=================================

1. A new capability flag ``V4L2_CAP_RADIO`` was added for radio devices.
   Prior to this change radio devices would identify solely by having
   exactly one tuner whose type field reads ``V4L2_TUNER_RADIO``.
   (1. 为收音设备新增了能力标志 ``V4L2_CAP_RADIO``。在此变更之前，收音设备仅靠"恰好有一个 type 字段为 ``V4L2_TUNER_RADIO`` 的调谐器"来识别。)

2. An optional driver access priority mechanism was added, see
   :ref:`app-pri` for details.
   (2. 新增了可选的驱动访问优先级机制，详见 :ref:`app-pri`。)

3. The audio input and output interface was found to be incomplete.
   (3. 人们发现音频输入/输出接口并不完整。

   Previously the :ref:`VIDIOC_G_AUDIO <VIDIOC_G_AUDIO>` ioctl would
   enumerate the available audio inputs. An ioctl to determine the
   current audio input, if more than one combines with the current video
   input, did not exist. So ``VIDIOC_G_AUDIO`` was renamed to
   ``VIDIOC_G_AUDIO_OLD``, this ioctl was removed on Kernel 2.6.39. The
   :ref:`VIDIOC_ENUMAUDIO` ioctl was added to
   enumerate audio inputs, while
   :ref:`VIDIOC_G_AUDIO <VIDIOC_G_AUDIO>` now reports the current
   audio input.
   (以前 :ref:`VIDIOC_G_AUDIO <VIDIOC_G_AUDIO>` ioctl 会枚举可用的音频输入。当有不止一个音频输入与当前视频输入组合时，并没有一个 ioctl 来确定当前的音频输入。于是 ``VIDIOC_G_AUDIO`` 改名为 ``VIDIOC_G_AUDIO_OLD``，该 ioctl 在内核 2.6.39 中被删除。新增 :ref:`VIDIOC_ENUMAUDIO` ioctl 来枚举音频输入，而 :ref:`VIDIOC_G_AUDIO <VIDIOC_G_AUDIO>` 现在报告当前的音频输入。)

   The same changes were made to
   :ref:`VIDIOC_G_AUDOUT <VIDIOC_G_AUDOUT>` and
   :ref:`VIDIOC_ENUMAUDOUT <VIDIOC_ENUMAUDOUT>`.
   (:ref:`VIDIOC_G_AUDOUT <VIDIOC_G_AUDOUT>` 和 :ref:`VIDIOC_ENUMAUDOUT <VIDIOC_ENUMAUDOUT>` 也做了同样的修改。)

   Until further the "videodev" module will automatically translate
   between the old and new ioctls, but drivers and applications must be
   updated to successfully compile again.
   (在新通知下达之前，"videodev" 模块会在新旧 ioctls 之间自动转换，但驱动和应用程序必须更新才能再次成功编译。)

4. The :ref:`VIDIOC_OVERLAY` ioctl was incorrectly
   defined with write-read parameter. It was changed to write-only,
   while the write-read version was renamed to ``VIDIOC_OVERLAY_OLD``.
   The old ioctl was removed on Kernel 2.6.39. Until further the
   "videodev" kernel module will automatically translate to the new
   version, so drivers must be recompiled, but not applications.
   (4. :ref:`VIDIOC_OVERLAY` ioctl 原本错误地定义为读写参数。它被改为只写，而读写版本改名为 ``VIDIOC_OVERLAY_OLD``。旧 ioctl 在内核 2.6.39 中被删除。在新通知下达之前，"videodev" 内核模块会自动转换到新版本，因此驱动必须重新编译，应用程序则不必。)

5. :ref:`overlay` incorrectly stated that clipping rectangles define
   regions where the video can be seen. Correct is that clipping
   rectangles define regions where *no* video shall be displayed and so
   the graphics surface can be seen.
   (5. :ref:`overlay` 曾错误地表述为：裁剪矩形定义的是可以看到视频的区域。正确说法是：裁剪矩形定义的是*不*显示视频的区域，因此那些地方可以看到图形表面。)

6. The :ref:`VIDIOC_S_PARM <VIDIOC_G_PARM>` and
   :ref:`VIDIOC_S_CTRL <VIDIOC_G_CTRL>` ioctls were defined with
   write-only parameter, inconsistent with other ioctls modifying their
   argument. They were changed to write-read, while a ``_OLD`` suffix
   was added to the write-only versions. The old ioctls were removed on
   Kernel 2.6.39. Drivers and applications assuming a constant parameter
   need an update.
   (6. :ref:`VIDIOC_S_PARM <VIDIOC_G_PARM>` 和 :ref:`VIDIOC_S_CTRL <VIDIOC_G_CTRL>` ioctls 原来定义为只写参数，与那些会修改其参数的其他 ioctls 不一致。它们被改为读写，同时只写版本加上了 ``_OLD`` 后缀。旧 ioctls 在内核 2.6.39 中被删除。假定参数不变的那些驱动和应用程序需要更新。)

V4L2 2003-11-05(V4L2，2003-11-05)
=================================

1. In :ref:`pixfmt-rgb` the following pixel formats were incorrectly
   transferred from Bill Dirks' V4L2 specification. Descriptions below
   refer to bytes in memory, in ascending address order.
   (1. 以下像素格式在从 Bill Dirks 的 V4L2 规范誊入 :ref:`pixfmt-rgb` 时出错了。下面的描述按地址递增顺序指内存中的字节。)


   .. flat-table::
       :header-rows:  1
       :stub-columns: 0

       * - Symbol(符号)
	 - In this document prior to revision 0.5(0.5 版之前本文档中的值)
	 - Corrected(更正后)
       * - ``V4L2_PIX_FMT_RGB24``
	 - B, G, R
	 - R, G, B
       * - ``V4L2_PIX_FMT_BGR24``
	 - R, G, B
	 - B, G, R
       * - ``V4L2_PIX_FMT_RGB32``
	 - B, G, R, X
	 - R, G, B, X
       * - ``V4L2_PIX_FMT_BGR32``
	 - R, G, B, X
	 - B, G, R, X

   The ``V4L2_PIX_FMT_BGR24`` example was always correct.
   (``V4L2_PIX_FMT_BGR24`` 的例子一直是对的。)

   In :ref:`v4l-image-properties` the mapping of the V4L
   ``VIDEO_PALETTE_RGB24`` and ``VIDEO_PALETTE_RGB32`` formats to V4L2
   pixel formats was accordingly corrected.
   (在 :ref:`v4l-image-properties` 中，V4L ``VIDEO_PALETTE_RGB24`` 和 ``VIDEO_PALETTE_RGB32`` 格式到 V4L2 像素格式的映射也相应地做了更正。)

2. Unrelated to the fixes above, drivers may still interpret some V4L2
   RGB pixel formats differently. These issues have yet to be addressed,
   for details see :ref:`pixfmt-rgb`.
   (2. 与上述修复无关的是，驱动对某些 V4L2 RGB 像素格式的解释可能仍有不同。这些问题尚待解决，详见 :ref:`pixfmt-rgb`。)

V4L2 in Linux 2.6.6, 2004-05-09(Linux 2.6.6 中的 V4L2，2004-05-09)
==================================================================

1. The :ref:`VIDIOC_CROPCAP` ioctl was incorrectly
   defined with read-only parameter. It is now defined as write-read
   ioctl, while the read-only version was renamed to
   ``VIDIOC_CROPCAP_OLD``. The old ioctl was removed on Kernel 2.6.39.
   (1. :ref:`VIDIOC_CROPCAP` ioctl 原本错误地定义为只读参数。它现在被定义为读写 ioctl，只读版本改名为 ``VIDIOC_CROPCAP_OLD``。旧 ioctl 在内核 2.6.39 中被删除。)

V4L2 in Linux 2.6.8(Linux 2.6.8 中的 V4L2)
==========================================

1. A new field ``input`` (former ``reserved[0]``) was added to the
   struct v4l2_buffer. Purpose of this
   field is to alternate between video inputs (e. g. cameras) in step
   with the video capturing process. This function must be enabled with
   the new ``V4L2_BUF_FLAG_INPUT`` flag. The ``flags`` field is no
   longer read-only.
   (1. struct v4l2_buffer 新增了 ``input`` 字段 (原来的 ``reserved[0]``)。这个字段的用途是：在视频捕获过程中，同步地在多个视频输入 (例如多个摄像头) 之间切换。必须用新的 ``V4L2_BUF_FLAG_INPUT`` 标志启用这一功能。``flags`` 字段不再是只读的。)

V4L2 spec erratum 2004-08-01(V4L2 规范勘误，2004-08-01)
=======================================================

1. The return value of the :ref:`func-open` function was incorrectly
   documented.
   (1. :ref:`func-open` 函数的返回值文档有误。)

2. Audio output ioctls end in -AUDOUT, not -AUDIOOUT.
   (2. 音频输出 ioctl 的后缀是 -AUDOUT，不是 -AUDIOOUT。)

3. In the Current Audio Input example the ``VIDIOC_G_AUDIO`` ioctl took
   the wrong argument.
   (3. 在"当前音频输入"示例中，``VIDIOC_G_AUDIO`` ioctl 用错了参数。)

4. The documentation of the :ref:`VIDIOC_QBUF` and
   :ref:`VIDIOC_DQBUF <VIDIOC_QBUF>` ioctls did not mention the
   struct v4l2_buffer ``memory`` field. It was
   also missing from examples. Also on the ``VIDIOC_DQBUF`` page the ``EIO``
   error code was not documented.
   (4. :ref:`VIDIOC_QBUF` 和 :ref:`VIDIOC_DQBUF <VIDIOC_QBUF>` ioctls 的文档没有提到 struct v4l2_buffer 的 ``memory`` 字段，示例里也漏掉了。此外 ``VIDIOC_DQBUF`` 页面也没有记载 ``EIO`` 错误码。)

V4L2 in Linux 2.6.14(Linux 2.6.14 中的 V4L2)
============================================

1. A new sliced VBI interface was added. It is documented in
   :ref:`sliced` and replaces the interface first proposed in V4L2
   specification 0.8.
   (1. 新增了切割 VBI (sliced VBI) 接口。它记载于 :ref:`sliced`，取代了 V4L2 规范 0.8 中首次提出的接口。)

V4L2 in Linux 2.6.15(Linux 2.6.15 中的 V4L2)
============================================

1. The :ref:`VIDIOC_LOG_STATUS` ioctl was added.
   (1. 新增了 :ref:`VIDIOC_LOG_STATUS` ioctl。)

2. New video standards ``V4L2_STD_NTSC_443``, ``V4L2_STD_SECAM_LC``,
   ``V4L2_STD_SECAM_DK`` (a set of SECAM D, K and K1), and
   ``V4L2_STD_ATSC`` (a set of ``V4L2_STD_ATSC_8_VSB`` and
   ``V4L2_STD_ATSC_16_VSB``) were defined. Note the ``V4L2_STD_525_60``
   set now includes ``V4L2_STD_NTSC_443``. See also
   :ref:`v4l2-std-id`.
   (2. 定义了新的视频标准 ``V4L2_STD_NTSC_443``、``V4L2_STD_SECAM_LC``、``V4L2_STD_SECAM_DK`` (SECAM D、K 和 K1 的集合) 以及 ``V4L2_STD_ATSC`` (``V4L2_STD_ATSC_8_VSB`` 和 ``V4L2_STD_ATSC_16_VSB`` 的集合)。注意 ``V4L2_STD_525_60`` 集合现在包含 ``V4L2_STD_NTSC_443``。另见 :ref:`v4l2-std-id`。)

3. The ``VIDIOC_G_COMP`` and ``VIDIOC_S_COMP`` ioctl were renamed to
   ``VIDIOC_G_MPEGCOMP`` and ``VIDIOC_S_MPEGCOMP`` respectively. Their
   argument was replaced by a struct
   ``v4l2_mpeg_compression`` pointer. (The
   ``VIDIOC_G_MPEGCOMP`` and ``VIDIOC_S_MPEGCOMP`` ioctls where removed
   in Linux 2.6.25.)
   (3. ``VIDIOC_G_COMP`` 和 ``VIDIOC_S_COMP`` ioctl 分别改名为 ``VIDIOC_G_MPEGCOMP`` 和 ``VIDIOC_S_MPEGCOMP``。它们的参数换成了指向 struct ``v4l2_mpeg_compression`` 的指针。(``VIDIOC_G_MPEGCOMP`` 和 ``VIDIOC_S_MPEGCOMP`` ioctls 后来在 Linux 2.6.25 中被删除。)

V4L2 spec erratum 2005-11-27(V4L2 规范勘误，2005-11-27)
=======================================================

The capture example in :ref:`capture-example` called the
:ref:`VIDIOC_S_CROP <VIDIOC_G_CROP>` ioctl without checking if
cropping is supported. In the video standard selection example in
:ref:`standard` the :ref:`VIDIOC_S_STD <VIDIOC_G_STD>` call used
the wrong argument type.
(:ref:`capture-example` 中的捕获示例在调用 :ref:`VIDIOC_S_CROP <VIDIOC_G_CROP>` ioctl 前没有检查是否支持裁剪。:ref:`standard` 中的视频标准选择示例里，:ref:`VIDIOC_S_STD <VIDIOC_G_STD>` 调用用错了参数类型。)

V4L2 spec erratum 2006-01-10(V4L2 规范勘误，2006-01-10)
=======================================================

1. The ``V4L2_IN_ST_COLOR_KILL`` flag in struct v4l2_input not only
   indicates if the color killer is enabled, but also if it is active.
   (The color killer disables color decoding when it detects no color
   in the video signal to improve the image quality.)
   (1. struct v4l2_input 中的 ``V4L2_IN_ST_COLOR_KILL`` 标志不仅指示消色器是否启用，还指示它是否处于活动状态。(消色器在检测到视频信号中没有颜色时关闭彩色解码，以改善图像质量。)

2. :ref:`VIDIOC_S_PARM <VIDIOC_G_PARM>` is a write-read ioctl, not
   write-only as stated on its reference page. The ioctl changed in 2003
   as noted above.
   (2. :ref:`VIDIOC_S_PARM <VIDIOC_G_PARM>` 是读写 ioctl，并非其参考页所述的只写。该 ioctl 在 2003 年已如上文所述发生了变化。)

V4L2 spec erratum 2006-02-03(V4L2 规范勘误，2006-02-03)
=======================================================

1. In struct v4l2_captureparm and struct v4l2_outputparm the ``timeperframe``
   field gives the time in seconds, not microseconds.
   (1. 在 struct v4l2_captureparm 和 struct v4l2_outputparm 中，``timeperframe`` 字段以秒而不是微秒给出时间。)

V4L2 spec erratum 2006-02-04(V4L2 规范勘误，2006-02-04)
=======================================================

1. The ``clips`` field in struct v4l2_window
   must point to an array of struct v4l2_clip, not
   a linked list, because drivers ignore the
   struct v4l2_clip. ``next`` pointer.
   (1. struct v4l2_window 的 ``clips`` 字段必须指向一个 struct v4l2_clip 数组，而不是链表，因为驱动会忽略 struct v4l2_clip 的 ``next`` 指针。)

V4L2 in Linux 2.6.17(Linux 2.6.17 中的 V4L2)
============================================

1. New video standard macros were added: ``V4L2_STD_NTSC_M_KR`` (NTSC M
   South Korea), and the sets ``V4L2_STD_MN``, ``V4L2_STD_B``,
   ``V4L2_STD_GH`` and ``V4L2_STD_DK``. The ``V4L2_STD_NTSC`` and
   ``V4L2_STD_SECAM`` sets now include ``V4L2_STD_NTSC_M_KR`` and
   ``V4L2_STD_SECAM_LC`` respectively.
   (1. 新增了视频标准宏：``V4L2_STD_NTSC_M_KR`` (韩国 NTSC M)，以及集合 ``V4L2_STD_MN``、``V4L2_STD_B``、``V4L2_STD_GH`` 和 ``V4L2_STD_DK``。``V4L2_STD_NTSC`` 和 ``V4L2_STD_SECAM`` 集合现在分别包含 ``V4L2_STD_NTSC_M_KR`` 和 ``V4L2_STD_SECAM_LC``。)

2. A new ``V4L2_TUNER_MODE_LANG1_LANG2`` was defined to record both
   languages of a bilingual program. The use of
   ``V4L2_TUNER_MODE_STEREO`` for this purpose is deprecated now. See
   the :ref:`VIDIOC_G_TUNER <VIDIOC_G_TUNER>` section for details.
   (2. 定义了新的 ``V4L2_TUNER_MODE_LANG1_LANG2``，用于录制双语节目的两种语言。用 ``V4L2_TUNER_MODE_STEREO`` 来做这件事现在已弃用。详情参见 :ref:`VIDIOC_G_TUNER <VIDIOC_G_TUNER>` 一节。)

V4L2 spec erratum 2006-09-23 (Draft 0.15)(V4L2 规范勘误，2006-09-23 (草案 0.15))
================================================================================

1. In various places ``V4L2_BUF_TYPE_SLICED_VBI_CAPTURE`` and
   ``V4L2_BUF_TYPE_SLICED_VBI_OUTPUT`` of the sliced VBI interface were
   not mentioned along with other buffer types.
   (1. 在多处，切割 VBI 接口的 ``V4L2_BUF_TYPE_SLICED_VBI_CAPTURE`` 和 ``V4L2_BUF_TYPE_SLICED_VBI_OUTPUT`` 没有与其他缓冲区类型一并提及。)

2. In :ref:`VIDIOC_G_AUDIO <VIDIOC_G_AUDIO>` it was clarified that the
   struct v4l2_audio ``mode`` field is a flags field.
   (2. 在 :ref:`VIDIOC_G_AUDIO <VIDIOC_G_AUDIO>` 中澄清了 struct v4l2_audio 的 ``mode`` 字段是一个标志 (flags) 字段。)

3. :ref:`VIDIOC_QUERYCAP` did not mention the sliced VBI and radio
   capability flags.
   (3. :ref:`VIDIOC_QUERYCAP` 没有提到切割 VBI 和收音能力标志。)

4. In :ref:`VIDIOC_G_FREQUENCY <VIDIOC_G_FREQUENCY>` it was clarified that
   applications must initialize the tuner ``type`` field of
   struct v4l2_frequency before calling
   :ref:`VIDIOC_S_FREQUENCY <VIDIOC_G_FREQUENCY>`.
   (4. 在 :ref:`VIDIOC_G_FREQUENCY <VIDIOC_G_FREQUENCY>` 中澄清了：应用程序在调用 :ref:`VIDIOC_S_FREQUENCY <VIDIOC_G_FREQUENCY>` 之前，必须先初始化 struct v4l2_frequency 的调谐器 ``type`` 字段。)

5. The ``reserved`` array in struct v4l2_requestbuffers has 2 elements,
   not 32.
   (5. struct v4l2_requestbuffers 中的 ``reserved`` 数组有 2 个元素，不是 32 个。)

6. In :ref:`output` and :ref:`raw-vbi` the device file names
   ``/dev/vout`` which never caught on were replaced by ``/dev/video``.
   (6. 在 :ref:`output` 和 :ref:`raw-vbi` 中，从未流行起来的设备文件名 ``/dev/vout`` 被 ``/dev/video`` 取代。)

7. With Linux 2.6.15 the possible range for VBI device minor numbers was
   extended from 224-239 to 224-255. Accordingly device file names
   ``/dev/vbi0`` to ``/dev/vbi31`` are possible now.
   (7. 从 Linux 2.6.15 起，VBI 设备次设备号的可能范围从 224-239 扩展到 224-255。相应地，现在可以有 ``/dev/vbi0`` 到 ``/dev/vbi31`` 的设备文件名。)

V4L2 in Linux 2.6.18(Linux 2.6.18 中的 V4L2)
============================================

1. New ioctls :ref:`VIDIOC_G_EXT_CTRLS <VIDIOC_G_EXT_CTRLS>`,
   :ref:`VIDIOC_S_EXT_CTRLS <VIDIOC_G_EXT_CTRLS>` and
   :ref:`VIDIOC_TRY_EXT_CTRLS <VIDIOC_G_EXT_CTRLS>` were added, a
   flag to skip unsupported controls with
   :ref:`VIDIOC_QUERYCTRL`, new control types
   ``V4L2_CTRL_TYPE_INTEGER64`` and ``V4L2_CTRL_TYPE_CTRL_CLASS``
   (enum v4l2_ctrl_type), and new control flags
   ``V4L2_CTRL_FLAG_READ_ONLY``, ``V4L2_CTRL_FLAG_UPDATE``,
   ``V4L2_CTRL_FLAG_INACTIVE`` and ``V4L2_CTRL_FLAG_SLIDER``
   (:ref:`control-flags`). See :ref:`extended-controls` for details.
   (1. 新增了 ioctls :ref:`VIDIOC_G_EXT_CTRLS <VIDIOC_G_EXT_CTRLS>`、:ref:`VIDIOC_S_EXT_CTRLS <VIDIOC_G_EXT_CTRLS>` 和 :ref:`VIDIOC_TRY_EXT_CTRLS <VIDIOC_G_EXT_CTRLS>`；新增 :ref:`VIDIOC_QUERYCTRL` 中跳过不支持控件的标志；新增控件类型 ``V4L2_CTRL_TYPE_INTEGER64`` 和 ``V4L2_CTRL_TYPE_CTRL_CLASS`` (enum v4l2_ctrl_type)；新增控件标志 ``V4L2_CTRL_FLAG_READ_ONLY``、``V4L2_CTRL_FLAG_UPDATE``、``V4L2_CTRL_FLAG_INACTIVE`` 和 ``V4L2_CTRL_FLAG_SLIDER`` (:ref:`control-flags`)。详见 :ref:`extended-controls`。)

V4L2 in Linux 2.6.19(Linux 2.6.19 中的 V4L2)
============================================

1. In struct v4l2_sliced_vbi_cap a
   buffer type field was added replacing a reserved field. Note on
   architectures where the size of enum types differs from int types the
   size of the structure changed. The
   :ref:`VIDIOC_G_SLICED_VBI_CAP <VIDIOC_G_SLICED_VBI_CAP>` ioctl
   was redefined from being read-only to write-read. Applications must
   initialize the type field and clear the reserved fields now. These
   changes may *break the compatibility* with older drivers and
   applications.
   (1. struct v4l2_sliced_vbi_cap 中新增了一个缓冲区类型字段，取代了一个保留字段。注意：在枚举类型大小与 int 类型不同的架构上，该结构体的大小变了。:ref:`VIDIOC_G_SLICED_VBI_CAP <VIDIOC_G_SLICED_VBI_CAP>` ioctl 从只读重新定义为读写。应用程序现在必须初始化 type 字段并清零保留字段。这些变更可能*破坏*与旧驱动和旧应用程序的兼容性。)

2. The ioctls :ref:`VIDIOC_ENUM_FRAMESIZES`
   and
   :ref:`VIDIOC_ENUM_FRAMEINTERVALS`
   were added.
   (2. 新增了 ioctls :ref:`VIDIOC_ENUM_FRAMESIZES` 和 :ref:`VIDIOC_ENUM_FRAMEINTERVALS`。)

3. A new pixel format ``V4L2_PIX_FMT_RGB444`` (:ref:`pixfmt-rgb`) was
   added.
   (3. 新增了像素格式 ``V4L2_PIX_FMT_RGB444`` (:ref:`pixfmt-rgb`)。)

V4L2 spec erratum 2006-10-12 (Draft 0.17)(V4L2 规范勘误，2006-10-12 (草案 0.17))
================================================================================

1. ``V4L2_PIX_FMT_HM12`` (:ref:`reserved-formats`) is a YUV 4:2:0, not
   4:2:2 format.
   (1. ``V4L2_PIX_FMT_HM12`` (:ref:`reserved-formats`) 是 YUV 4:2:0 格式，不是 4:2:2。)

V4L2 in Linux 2.6.21(Linux 2.6.21 中的 V4L2)
============================================

1. The ``videodev2.h`` header file is now dual licensed under GNU
   General Public License version two or later, and under a 3-clause
   BSD-style license.
   (1. ``videodev2.h`` 头文件现在采用双重许可：GNU 通用公共许可证第 2 版或更高版本，以及 3 条款 BSD 风格许可。)

V4L2 in Linux 2.6.22(Linux 2.6.22 中的 V4L2)
============================================

1. Two new field orders ``V4L2_FIELD_INTERLACED_TB`` and
   ``V4L2_FIELD_INTERLACED_BT`` were added. See enum v4l2_field for
   details.
   (1. 新增了两种场顺序 ``V4L2_FIELD_INTERLACED_TB`` 和 ``V4L2_FIELD_INTERLACED_BT``。详见 enum v4l2_field。)

2. Three new clipping/blending methods with a global or straight or
   inverted local alpha value were added to the video overlay interface.
   See the description of the :ref:`VIDIOC_G_FBUF <VIDIOC_G_FBUF>`
   and :ref:`VIDIOC_S_FBUF <VIDIOC_G_FBUF>` ioctls for details.
   (2. 视频叠加接口新增了三种带全局 alpha 值、或正向/反向局部 alpha 值的裁剪/混合方法。详见 :ref:`VIDIOC_G_FBUF <VIDIOC_G_FBUF>` 和 :ref:`VIDIOC_S_FBUF <VIDIOC_G_FBUF>` ioctls 的描述。)

   A new ``global_alpha`` field was added to struct v4l2_window,
   extending the structure. This may **break compatibility** with
   applications using a struct v4l2_window directly. However the
   :ref:`VIDIOC_G/S/TRY_FMT <VIDIOC_G_FMT>` ioctls, which take a
   pointer to a struct v4l2_format parent structure
   with padding bytes at the end, are not affected.
   (struct v4l2_window 新增了 ``global_alpha`` 字段，结构体随之扩展。这对直接使用 struct v4l2_window 的应用程序可能破坏兼容性。不过，接受指向末尾带填充字节的 struct v4l2_format 父结构体指针的 :ref:`VIDIOC_G/S/TRY_FMT <VIDIOC_G_FMT>` ioctls 不受影响。)

3. The format of the ``chromakey`` field in struct v4l2_window changed from
   "host order RGB32" to a pixel value in the same format as the framebuffer.
   This may **break compatibility** with existing applications. Drivers
   supporting the "host order RGB32" format are not known.
   (3. struct v4l2_window 中 ``chromakey`` 字段的格式从"主机字节序 RGB32"改成了与帧缓冲相同格式的像素值。这可能对现有应用程序破坏兼容性。目前尚无驱动支持"主机字节序 RGB32"格式。)

V4L2 in Linux 2.6.24(Linux 2.6.24 中的 V4L2)
============================================

1. The pixel formats ``V4L2_PIX_FMT_PAL8``, ``V4L2_PIX_FMT_YUV444``,
   ``V4L2_PIX_FMT_YUV555``, ``V4L2_PIX_FMT_YUV565`` and
   ``V4L2_PIX_FMT_YUV32`` were added.
   (1. 新增了像素格式 ``V4L2_PIX_FMT_PAL8``、``V4L2_PIX_FMT_YUV444``、``V4L2_PIX_FMT_YUV555``、``V4L2_PIX_FMT_YUV565`` 和 ``V4L2_PIX_FMT_YUV32``。)

V4L2 in Linux 2.6.25(Linux 2.6.25 中的 V4L2)
============================================

1. The pixel formats :ref:`V4L2_PIX_FMT_Y16 <V4L2-PIX-FMT-Y16>` and
   :ref:`V4L2_PIX_FMT_SBGGR16 <V4L2-PIX-FMT-SBGGR16>` were added.
   (1. 新增了像素格式 :ref:`V4L2_PIX_FMT_Y16 <V4L2-PIX-FMT-Y16>` 和 :ref:`V4L2_PIX_FMT_SBGGR16 <V4L2-PIX-FMT-SBGGR16>`。)

2. New :ref:`controls <control>` ``V4L2_CID_POWER_LINE_FREQUENCY``,
   ``V4L2_CID_HUE_AUTO``, ``V4L2_CID_WHITE_BALANCE_TEMPERATURE``,
   ``V4L2_CID_SHARPNESS`` and ``V4L2_CID_BACKLIGHT_COMPENSATION`` were
   added. The controls ``V4L2_CID_BLACK_LEVEL``, ``V4L2_CID_WHITENESS``,
   ``V4L2_CID_HCENTER`` and ``V4L2_CID_VCENTER`` were deprecated.
   (2. 新增了 :ref:`用户控件 <control>` ``V4L2_CID_POWER_LINE_FREQUENCY``、``V4L2_CID_HUE_AUTO``、``V4L2_CID_WHITE_BALANCE_TEMPERATURE``、``V4L2_CID_SHARPNESS`` 和 ``V4L2_CID_BACKLIGHT_COMPENSATION``。控件 ``V4L2_CID_BLACK_LEVEL``、``V4L2_CID_WHITENESS``、``V4L2_CID_HCENTER`` 和 ``V4L2_CID_VCENTER`` 被弃用。)

3. A :ref:`Camera controls class <camera-controls>` was added, with
   the new controls ``V4L2_CID_EXPOSURE_AUTO``,
   ``V4L2_CID_EXPOSURE_ABSOLUTE``, ``V4L2_CID_EXPOSURE_AUTO_PRIORITY``,
   ``V4L2_CID_PAN_RELATIVE``, ``V4L2_CID_TILT_RELATIVE``,
   ``V4L2_CID_PAN_RESET``, ``V4L2_CID_TILT_RESET``,
   ``V4L2_CID_PAN_ABSOLUTE``, ``V4L2_CID_TILT_ABSOLUTE``,
   ``V4L2_CID_FOCUS_ABSOLUTE``, ``V4L2_CID_FOCUS_RELATIVE`` and
   ``V4L2_CID_FOCUS_AUTO``.
   (3. 新增了 :ref:`相机控件类 <camera-controls>`，包含新控件 ``V4L2_CID_EXPOSURE_AUTO``、``V4L2_CID_EXPOSURE_ABSOLUTE``、``V4L2_CID_EXPOSURE_AUTO_PRIORITY``、``V4L2_CID_PAN_RELATIVE``、``V4L2_CID_TILT_RELATIVE``、``V4L2_CID_PAN_RESET``、``V4L2_CID_TILT_RESET``、``V4L2_CID_PAN_ABSOLUTE``、``V4L2_CID_TILT_ABSOLUTE``、``V4L2_CID_FOCUS_ABSOLUTE``、``V4L2_CID_FOCUS_RELATIVE`` 和 ``V4L2_CID_FOCUS_AUTO``。)

4. The ``VIDIOC_G_MPEGCOMP`` and ``VIDIOC_S_MPEGCOMP`` ioctls, which
   were superseded by the :ref:`extended controls <extended-controls>`
   interface in Linux 2.6.18, where finally removed from the
   ``videodev2.h`` header file.
   (4. ``VIDIOC_G_MPEGCOMP`` 和 ``VIDIOC_S_MPEGCOMP`` ioctls 在 Linux 2.6.18 中已被 :ref:`扩展控件 <extended-controls>` 接口取代，最终从 ``videodev2.h`` 头文件中删除了。)

V4L2 in Linux 2.6.26(Linux 2.6.26 中的 V4L2)
============================================

1. The pixel formats ``V4L2_PIX_FMT_Y16`` and ``V4L2_PIX_FMT_SBGGR16``
   were added.
   (1. 新增了像素格式 ``V4L2_PIX_FMT_Y16`` 和 ``V4L2_PIX_FMT_SBGGR16``。)

2. Added user controls ``V4L2_CID_CHROMA_AGC`` and
   ``V4L2_CID_COLOR_KILLER``.
   (2. 新增了用户控件 ``V4L2_CID_CHROMA_AGC`` 和 ``V4L2_CID_COLOR_KILLER``。)

V4L2 in Linux 2.6.27(Linux 2.6.27 中的 V4L2)
============================================

1. The :ref:`VIDIOC_S_HW_FREQ_SEEK` ioctl
   and the ``V4L2_CAP_HW_FREQ_SEEK`` capability were added.
   (1. 新增了 :ref:`VIDIOC_S_HW_FREQ_SEEK` ioctl 和 ``V4L2_CAP_HW_FREQ_SEEK`` 能力。)

2. The pixel formats ``V4L2_PIX_FMT_YVYU``, ``V4L2_PIX_FMT_PCA501``,
   ``V4L2_PIX_FMT_PCA505``, ``V4L2_PIX_FMT_PCA508``,
   ``V4L2_PIX_FMT_PCA561``, ``V4L2_PIX_FMT_SGBRG8``,
   ``V4L2_PIX_FMT_PAC207`` and ``V4L2_PIX_FMT_PJPG`` were added.
   (2. 新增了像素格式 ``V4L2_PIX_FMT_YVYU``、``V4L2_PIX_FMT_PCA501``、``V4L2_PIX_FMT_PCA505``、``V4L2_PIX_FMT_PCA508``、``V4L2_PIX_FMT_PCA561``、``V4L2_PIX_FMT_SGBRG8``、``V4L2_PIX_FMT_PAC207`` 和 ``V4L2_PIX_FMT_PJPG``。)

V4L2 in Linux 2.6.28(Linux 2.6.28 中的 V4L2)
============================================

1. Added ``V4L2_MPEG_AUDIO_ENCODING_AAC`` and
   ``V4L2_MPEG_AUDIO_ENCODING_AC3`` MPEG audio encodings.
   (1. 新增了 MPEG 音频编码 ``V4L2_MPEG_AUDIO_ENCODING_AAC`` 和 ``V4L2_MPEG_AUDIO_ENCODING_AC3``。)

2. Added ``V4L2_MPEG_VIDEO_ENCODING_MPEG_4_AVC`` MPEG video encoding.
   (2. 新增了 MPEG 视频编码 ``V4L2_MPEG_VIDEO_ENCODING_MPEG_4_AVC``。)

3. The pixel formats ``V4L2_PIX_FMT_SGRBG10`` and
   ``V4L2_PIX_FMT_SGRBG10DPCM8`` were added.
   (3. 新增了像素格式 ``V4L2_PIX_FMT_SGRBG10`` 和 ``V4L2_PIX_FMT_SGRBG10DPCM8``。)

V4L2 in Linux 2.6.29(Linux 2.6.29 中的 V4L2)
============================================

1. The ``VIDIOC_G_CHIP_IDENT`` ioctl was renamed to
   ``VIDIOC_G_CHIP_IDENT_OLD`` and ``VIDIOC_DBG_G_CHIP_IDENT`` was
   introduced in its place. The old struct ``v4l2_chip_ident`` was renamed to
   struct ``v4l2_chip_ident_old``.
   (1. ``VIDIOC_G_CHIP_IDENT`` ioctl 改名为 ``VIDIOC_G_CHIP_IDENT_OLD``，并由新引入的 ``VIDIOC_DBG_G_CHIP_IDENT`` 取代。旧结构体 ``v4l2_chip_ident`` 改名为 struct ``v4l2_chip_ident_old``。)

2. The pixel formats ``V4L2_PIX_FMT_VYUY``, ``V4L2_PIX_FMT_NV16`` and
   ``V4L2_PIX_FMT_NV61`` were added.
   (2. 新增了像素格式 ``V4L2_PIX_FMT_VYUY``、``V4L2_PIX_FMT_NV16`` 和 ``V4L2_PIX_FMT_NV61``。)

3. Added camera controls ``V4L2_CID_ZOOM_ABSOLUTE``,
   ``V4L2_CID_ZOOM_RELATIVE``, ``V4L2_CID_ZOOM_CONTINUOUS`` and
   ``V4L2_CID_PRIVACY``.
   (3. 新增了相机控件 ``V4L2_CID_ZOOM_ABSOLUTE``、``V4L2_CID_ZOOM_RELATIVE``、``V4L2_CID_ZOOM_CONTINUOUS`` 和 ``V4L2_CID_PRIVACY``。)

V4L2 in Linux 2.6.30(Linux 2.6.30 中的 V4L2)
============================================

1. New control flag ``V4L2_CTRL_FLAG_WRITE_ONLY`` was added.
   (1. 新增了控件标志 ``V4L2_CTRL_FLAG_WRITE_ONLY``。)

2. New control ``V4L2_CID_COLORFX`` was added.
   (2. 新增了控件 ``V4L2_CID_COLORFX``。)

V4L2 in Linux 2.6.32
====================

1. In order to be easier to compare a V4L2 API and a kernel version, now
   V4L2 API is numbered using the Linux Kernel version numeration.
   (1. 为了更容易把 V4L2 API 和内核版本对应起来，现在 V4L2 API 改用 Linux 内核版本的编号方式来编号。)

2. Finalized the RDS capture API. See :ref:`rds` for more information.
   (2. 定稿了 RDS 捕获 API。更多信息见 :ref:`rds`。)

3. Added new capabilities for modulators and RDS encoders.
   (3. 为调制器和 RDS 编码器新增了若干能力。)

4. Add description for libv4l API.
   (4. 新增了 libv4l API 的说明。)

5. Added support for string controls via new type
   ``V4L2_CTRL_TYPE_STRING``.
   (5. 通过新类型 ``V4L2_CTRL_TYPE_STRING`` 增加了对字符串控件的支持。)

6. Added ``V4L2_CID_BAND_STOP_FILTER`` documentation.
   (6. 新增了 ``V4L2_CID_BAND_STOP_FILTER`` 的文档。)

7. Added FM Modulator (FM TX) Extended Control Class:
   ``V4L2_CTRL_CLASS_FM_TX`` and their Control IDs.
   (7. 新增了 FM 调制器 (FM TX) 扩展控件类 ``V4L2_CTRL_CLASS_FM_TX`` 及其控件 ID。)

8. Added FM Receiver (FM RX) Extended Control Class:
   ``V4L2_CTRL_CLASS_FM_RX`` and their Control IDs.
   (8. 新增了 FM 接收器 (FM RX) 扩展控件类 ``V4L2_CTRL_CLASS_FM_RX`` 及其控件 ID。)

9. Added Remote Controller chapter, describing the default Remote
   Controller mapping for media devices.
   (9. 新增了遥控器 (Remote Controller) 一章，描述媒体设备的默认遥控器键位映射。)

V4L2 in Linux 2.6.33(Linux 2.6.33 中的 V4L2)
============================================

1. Added support for Digital Video timings in order to support HDTV
   receivers and transmitters.
   (1. 新增了对数字视频 (Digital Video) 时序的支持，以便支持 HDTV 接收器和发送器。)

V4L2 in Linux 2.6.34(Linux 2.6.34 中的 V4L2)
============================================

1. Added ``V4L2_CID_IRIS_ABSOLUTE`` and ``V4L2_CID_IRIS_RELATIVE``
   controls to the :ref:`Camera controls class <camera-controls>`.
   (1. 在 :ref:`相机控件类 <camera-controls>` 中新增了 ``V4L2_CID_IRIS_ABSOLUTE`` 和 ``V4L2_CID_IRIS_RELATIVE`` 控件。)

V4L2 in Linux 2.6.37(Linux 2.6.37 中的 V4L2)
============================================

1. Remove the vtx (videotext/teletext) API. This API was no longer used
   and no hardware exists to verify the API. Nor were any userspace
   applications found that used it. It was originally scheduled for
   removal in 2.6.35.
   (1. 移除了 vtx (videotext/teletext) API。这个 API 已无人使用，也不存在可以验证它的硬件，也没有发现任何使用它的用户空间应用程序。它原本计划在 2.6.35 中移除。)

V4L2 in Linux 2.6.39(Linux 2.6.39 中的 V4L2)
============================================

1. The old VIDIOC_*_OLD symbols and V4L1 support were removed.
   (1. 删除了旧的 VIDIOC_*_OLD 符号和 V4L1 支持。)

2. Multi-planar API added. Does not affect the compatibility of current
   drivers and applications. See :ref:`multi-planar API <planar-apis>`
   for details.
   (2. 新增了多平面 (Multi-planar) API。它不影响现有驱动和应用程序的兼容性。详见 :ref:`多平面 API <planar-apis>`。)

V4L2 in Linux 3.1(Linux 3.1 中的 V4L2)
======================================

1. VIDIOC_QUERYCAP now returns a per-subsystem version instead of a
   per-driver one.
   (1. VIDIOC_QUERYCAP 现在返回子系统级 (per-subsystem) 版本号，而不是驱动级版本号。)

   Standardize an error code for invalid ioctl.
   (为无效 ioctl 统一了错误码。)

   Added V4L2_CTRL_TYPE_BITMASK.
   (新增了 V4L2_CTRL_TYPE_BITMASK。)

V4L2 in Linux 3.2(Linux 3.2 中的 V4L2)
======================================

1. V4L2_CTRL_FLAG_VOLATILE was added to signal volatile controls to
   userspace.
   (1. 新增了 V4L2_CTRL_FLAG_VOLATILE，用于向用户空间标识易变 (volatile) 控件。)

2. Add selection API for extended control over cropping and composing.
   Does not affect the compatibility of current drivers and
   applications. See :ref:`selection API <selection-api>` for details.
   (2. 新增了 selection API，用于对裁剪和合成进行扩展控制。它不影响现有驱动和应用程序的兼容性。详见 :ref:`selection API <selection-api>`。)

V4L2 in Linux 3.3(Linux 3.3 中的 V4L2)
======================================

1. Added ``V4L2_CID_ALPHA_COMPONENT`` control to the
   :ref:`User controls class <control>`.
   (1. 在 :ref:`用户控件类 <control>` 中新增了 ``V4L2_CID_ALPHA_COMPONENT`` 控件。)

2. Added the device_caps field to struct v4l2_capabilities and added
   the new V4L2_CAP_DEVICE_CAPS capability.
   (2. 在 struct v4l2_capabilities 中新增了 device_caps 字段，并新增了 V4L2_CAP_DEVICE_CAPS 能力。)

V4L2 in Linux 3.4(Linux 3.4 中的 V4L2)
======================================

1. Added :ref:`JPEG compression control class <jpeg-controls>`.
   (1. 新增了 :ref:`JPEG 压缩控件类 <jpeg-controls>`。)

2. Extended the DV Timings API:
   :ref:`VIDIOC_ENUM_DV_TIMINGS`,
   :ref:`VIDIOC_QUERY_DV_TIMINGS` and
   :ref:`VIDIOC_DV_TIMINGS_CAP`.
   (2. 扩展了 DV Timings API：:ref:`VIDIOC_ENUM_DV_TIMINGS`、:ref:`VIDIOC_QUERY_DV_TIMINGS` 和 :ref:`VIDIOC_DV_TIMINGS_CAP`。)

V4L2 in Linux 3.5(Linux 3.5 中的 V4L2)
======================================

1. Added integer menus, the new type will be
   V4L2_CTRL_TYPE_INTEGER_MENU.
   (1. 新增了整数菜单，新类型为 V4L2_CTRL_TYPE_INTEGER_MENU。)

2. Added selection API for V4L2 subdev interface:
   :ref:`VIDIOC_SUBDEV_G_SELECTION` and
   :ref:`VIDIOC_SUBDEV_S_SELECTION <VIDIOC_SUBDEV_G_SELECTION>`.
   (2. 为 V4L2 subdev 接口新增了 selection API：:ref:`VIDIOC_SUBDEV_G_SELECTION` 和 :ref:`VIDIOC_SUBDEV_S_SELECTION <VIDIOC_SUBDEV_G_SELECTION>`。)

3. Added ``V4L2_COLORFX_ANTIQUE``, ``V4L2_COLORFX_ART_FREEZE``,
   ``V4L2_COLORFX_AQUA``, ``V4L2_COLORFX_SILHOUETTE``,
   ``V4L2_COLORFX_SOLARIZATION``, ``V4L2_COLORFX_VIVID`` and
   ``V4L2_COLORFX_ARBITRARY_CBCR`` menu items to the
   ``V4L2_CID_COLORFX`` control.
   (3. 为 ``V4L2_CID_COLORFX`` 控件新增了菜单项 ``V4L2_COLORFX_ANTIQUE``、``V4L2_COLORFX_ART_FREEZE``、``V4L2_COLORFX_AQUA``、``V4L2_COLORFX_SILHOUETTE``、``V4L2_COLORFX_SOLARIZATION``、``V4L2_COLORFX_VIVID`` 和 ``V4L2_COLORFX_ARBITRARY_CBCR``。)

4. Added ``V4L2_CID_COLORFX_CBCR`` control.
   (4. 新增了 ``V4L2_CID_COLORFX_CBCR`` 控件。)

5. Added camera controls ``V4L2_CID_AUTO_EXPOSURE_BIAS``,
   ``V4L2_CID_AUTO_N_PRESET_WHITE_BALANCE``,
   ``V4L2_CID_IMAGE_STABILIZATION``, ``V4L2_CID_ISO_SENSITIVITY``,
   ``V4L2_CID_ISO_SENSITIVITY_AUTO``, ``V4L2_CID_EXPOSURE_METERING``,
   ``V4L2_CID_SCENE_MODE``, ``V4L2_CID_3A_LOCK``,
   ``V4L2_CID_AUTO_FOCUS_START``, ``V4L2_CID_AUTO_FOCUS_STOP``,
   ``V4L2_CID_AUTO_FOCUS_STATUS`` and ``V4L2_CID_AUTO_FOCUS_RANGE``.
   (5. 新增了相机控件 ``V4L2_CID_AUTO_EXPOSURE_BIAS``、``V4L2_CID_AUTO_N_PRESET_WHITE_BALANCE``、``V4L2_CID_IMAGE_STABILIZATION``、``V4L2_CID_ISO_SENSITIVITY``、``V4L2_CID_ISO_SENSITIVITY_AUTO``、``V4L2_CID_EXPOSURE_METERING``、``V4L2_CID_SCENE_MODE``、``V4L2_CID_3A_LOCK``、``V4L2_CID_AUTO_FOCUS_START``、``V4L2_CID_AUTO_FOCUS_STOP``、``V4L2_CID_AUTO_FOCUS_STATUS`` 和 ``V4L2_CID_AUTO_FOCUS_RANGE``。)

V4L2 in Linux 3.6(Linux 3.6 中的 V4L2)
======================================

1. Replaced ``input`` in struct v4l2_buffer by
   ``reserved2`` and removed ``V4L2_BUF_FLAG_INPUT``.
   (1. struct v4l2_buffer 中的 ``input`` 被 ``reserved2`` 取代，并删除了 ``V4L2_BUF_FLAG_INPUT``。)

2. Added V4L2_CAP_VIDEO_M2M and V4L2_CAP_VIDEO_M2M_MPLANE
   capabilities.
   (2. 新增了 V4L2_CAP_VIDEO_M2M 和 V4L2_CAP_VIDEO_M2M_MPLANE 能力。)

3. Added support for frequency band enumerations:
   :ref:`VIDIOC_ENUM_FREQ_BANDS`.
   (3. 新增了对频段枚举的支持：:ref:`VIDIOC_ENUM_FREQ_BANDS`。)

V4L2 in Linux 3.9(Linux 3.9 中的 V4L2)
======================================

1. Added timestamp types to ``flags`` field in
   struct v4l2_buffer. See :ref:`buffer-flags`.
   (1. 在 struct v4l2_buffer 的 ``flags`` 字段中新增了时间戳类型。见 :ref:`buffer-flags`。)

2. Added ``V4L2_EVENT_CTRL_CH_RANGE`` control event changes flag. See
   :ref:`ctrl-changes-flags`.
   (2. 新增了 ``V4L2_EVENT_CTRL_CH_RANGE`` 控件事件变化标志。见 :ref:`ctrl-changes-flags`。)

V4L2 in Linux 3.10(Linux 3.10 中的 V4L2)
========================================

1. Removed obsolete and unused DV_PRESET ioctls VIDIOC_G_DV_PRESET,
   VIDIOC_S_DV_PRESET, VIDIOC_QUERY_DV_PRESET and
   VIDIOC_ENUM_DV_PRESET. Remove the related v4l2_input/output
   capability flags V4L2_IN_CAP_PRESETS and V4L2_OUT_CAP_PRESETS.
   (1. 删除了过时且无人使用的 DV_PRESET ioctls：VIDIOC_G_DV_PRESET、VIDIOC_S_DV_PRESET、VIDIOC_QUERY_DV_PRESET 和 VIDIOC_ENUM_DV_PRESET。同时删除了相关的 v4l2_input/output 能力标志 V4L2_IN_CAP_PRESETS 和 V4L2_OUT_CAP_PRESETS。)

2. Added new debugging ioctl
   :ref:`VIDIOC_DBG_G_CHIP_INFO`.
   (2. 新增了调试 ioctl :ref:`VIDIOC_DBG_G_CHIP_INFO`。)

V4L2 in Linux 3.11(Linux 3.11 中的 V4L2)
========================================

1. Remove obsolete ``VIDIOC_DBG_G_CHIP_IDENT`` ioctl.
   (1. 删除了过时的 ``VIDIOC_DBG_G_CHIP_IDENT`` ioctl。)

V4L2 in Linux 3.14(Linux 3.14 中的 V4L2)
========================================

1. In struct v4l2_rect, the type of ``width`` and
   ``height`` fields changed from _s32 to _u32.
   (1. 在 struct v4l2_rect 中，``width`` 和 ``height`` 字段的类型从 _s32 改为 _u32。)

V4L2 in Linux 3.15(Linux 3.15 中的 V4L2)
========================================

1. Added Software Defined Radio (SDR) Interface.
   (1. 新增了软件定义无线电 (SDR) 接口。)

V4L2 in Linux 3.16(Linux 3.16 中的 V4L2)
========================================

1. Added event V4L2_EVENT_SOURCE_CHANGE.
   (1. 新增了事件 V4L2_EVENT_SOURCE_CHANGE。)

V4L2 in Linux 3.17(Linux 3.17 中的 V4L2)
========================================

1. Extended struct v4l2_pix_format. Added
   format flags.
   (1. 扩展了 struct v4l2_pix_format，新增了格式标志。)

2. Added compound control types and
   :ref:`VIDIOC_QUERY_EXT_CTRL <VIDIOC_QUERYCTRL>`.
   (2. 新增了复合控件类型以及 :ref:`VIDIOC_QUERY_EXT_CTRL <VIDIOC_QUERYCTRL>`。)

V4L2 in Linux 3.18(Linux 3.18 中的 V4L2)
========================================

1. Added ``V4L2_CID_PAN_SPEED`` and ``V4L2_CID_TILT_SPEED`` camera
   controls.
   (1. 新增了相机控件 ``V4L2_CID_PAN_SPEED`` 和 ``V4L2_CID_TILT_SPEED``。)

V4L2 in Linux 3.19(Linux 3.19 中的 V4L2)
========================================

1. Rewrote Colorspace chapter, added new enum v4l2_ycbcr_encoding
   and enum v4l2_quantization fields to struct v4l2_pix_format,
   struct v4l2_pix_format_mplane and struct v4l2_mbus_framefmt.
   (1. 重写了色彩空间 (Colorspace) 一章，为 struct v4l2_pix_format、struct v4l2_pix_format_mplane 和 struct v4l2_mbus_framefmt 新增了 enum v4l2_ycbcr_encoding 和 enum v4l2_quantization 字段。)

V4L2 in Linux 4.4(Linux 4.4 中的 V4L2)
======================================

1. Renamed ``V4L2_TUNER_ADC`` to ``V4L2_TUNER_SDR``. The use of
   ``V4L2_TUNER_ADC`` is deprecated now.
   (1. ``V4L2_TUNER_ADC`` 改名为 ``V4L2_TUNER_SDR``。``V4L2_TUNER_ADC`` 的使用现已弃用。)

2. Added ``V4L2_CID_RF_TUNER_RF_GAIN`` RF Tuner control.
   (2. 新增了 RF 调谐器控件 ``V4L2_CID_RF_TUNER_RF_GAIN``。)

3. Added transmitter support for Software Defined Radio (SDR) Interface.
   (3. 为软件定义无线电 (SDR) 接口新增了发送器支持。)

.. _other:

Relation of V4L2 to other Linux multimedia APIs(V4L2 与其他 Linux 多媒体 API 的关系)
====================================================================================

.. _xvideo:

X Video Extension(X 视频扩展)
-----------------------------

The X Video Extension (abbreviated XVideo or just Xv) is an extension of
the X Window system, implemented for example by the XFree86 project. Its
scope is similar to V4L2, an API to video capture and output devices for
X clients. Xv allows applications to display live video in a window,
send window contents to a TV output, and capture or output still images
in XPixmaps [#f1]_. With their implementation XFree86 makes the extension
available across many operating systems and architectures.
(X 视频扩展 (X Video Extension，简称 XVideo 或 Xv) 是 X Window 系统的一个扩展，例如由 XFree86 项目实现。它的适用范围与 V4L2 类似，是面向 X 客户端的视频捕获和输出设备 API。Xv 允许应用程序在窗口中显示实时视频、把窗口内容发送到电视输出，以及用 XPixmap 捕获或输出静态图像 [#f1]_。借助 XFree86 的实现，这一扩展可在许多操作系统和架构上使用。)

Because the driver is embedded into the X server Xv has a number of
advantages over the V4L2 :ref:`video overlay interface <overlay>`. The
driver can easily determine the overlay target, i. e. visible graphics
memory or off-screen buffers for a destructive overlay. It can program
the RAMDAC for a non-destructive overlay, scaling or color-keying, or
the clipping functions of the video capture hardware, always in sync
with drawing operations or windows moving or changing their stacking
order.
(由于驱动内嵌在 X 服务器中，Xv 比 V4L2 的 :ref:`视频叠加接口 <overlay>` 有若干优势。驱动可以轻松确定叠加目标，即对破坏性叠加而言的可见图形显存或屏外缓冲区。对于非破坏性叠加、缩放或色键 (color-keying)，以及视频捕获硬件的裁剪功能，它可以对 RAMDAC 编程，并且始终与绘图操作或窗口的移动、层叠次序变化保持同步。)

To combine the advantages of Xv and V4L a special Xv driver exists in
XFree86 and XOrg, just programming any overlay capable Video4Linux
device it finds. To enable it ``/etc/X11/XF86Config`` must contain these
lines:

::

    Section "Module"
	Load "v4l"
    EndSection

(为了结合 Xv 和 V4L 的优点，XFree86 和 XOrg 中有一个特殊的 Xv 驱动，它只对找到的任何具备叠加能力的 Video4Linux 设备编程。要启用它，``/etc/X11/XF86Config`` 必须包含上面这几行。)

As of XFree86 4.2 this driver still supports only V4L ioctls, however it
should work just fine with all V4L2 devices through the V4L2
backward-compatibility layer. Since V4L2 permits multiple opens it is
possible (if supported by the V4L2 driver) to capture video while an X
client requested video overlay. Restrictions of simultaneous capturing
and overlay are discussed in :ref:`overlay` apply.
(到 XFree86 4.2 为止，这个驱动仍然只支持 V4L ioctls，不过通过 V4L2 向后兼容层，它对所有 V4L2 设备都应该工作得很好。由于 V4L2 允许多次打开，在某个 X 客户端请求了视频叠加的同时进行视频捕获也是可能的 (前提是 V4L2 驱动支持)。同时捕获与叠加的限制在 :ref:`overlay` 中有讨论。)

Only marginally related to V4L2, XFree86 extended Xv to support hardware
YUV to RGB conversion and scaling for faster video playback, and added
an interface to MPEG-2 decoding hardware. This API is useful to display
images captured with V4L2 devices.
(与 V4L2 只有很小的关联：XFree86 扩展了 Xv 以支持硬件 YUV 到 RGB 转换和缩放，从而加快视频播放，并增加了对接 MPEG-2 解码硬件的接口。这个 API 对显示用 V4L2 设备捕获的图像很有用。)

Digital Video(数字视频)
-----------------------

V4L2 does not support digital terrestrial, cable or satellite broadcast.
A separate project aiming at digital receivers exists. You can find its
homepage at `https://linuxtv.org <https://linuxtv.org>`__. The Linux
DVB API has no connection to the V4L2 API except that drivers for hybrid
hardware may support both.
(V4L2 不支持数字地面、有线或卫星广播。另有一个面向数字接收器的独立项目，其主页在 `https://linuxtv.org <https://linuxtv.org>`__ 。除了混合硬件的驱动可能两者都支持之外，Linux DVB API 与 V4L2 API 没有别的关联。)

Audio Interfaces(音频接口)
--------------------------

[to do - OSS/ALSA]
([待完成 - OSS/ALSA])

.. _experimental:

Experimental API Elements(实验性 API 元素)
==========================================

The following V4L2 API elements are currently experimental and may
change in the future.
(以下 V4L2 API 元素目前仍属实验性质，将来可能会变化。)

-  :ref:`VIDIOC_DBG_G_REGISTER` and
   :ref:`VIDIOC_DBG_S_REGISTER <VIDIOC_DBG_G_REGISTER>` ioctls.
   (:ref:`VIDIOC_DBG_G_REGISTER` 和 :ref:`VIDIOC_DBG_S_REGISTER <VIDIOC_DBG_G_REGISTER>` ioctls。)

-  :ref:`VIDIOC_DBG_G_CHIP_INFO` ioctl.
   (:ref:`VIDIOC_DBG_G_CHIP_INFO` ioctl。)

.. _obsolete:

Obsolete API Elements(已废弃的 API 元素)
========================================

The following V4L2 API elements were superseded by new interfaces and
should not be implemented in new drivers.
(以下 V4L2 API 元素已被新接口取代，不应在新驱动中实现。)

-  ``VIDIOC_G_MPEGCOMP`` and ``VIDIOC_S_MPEGCOMP`` ioctls. Use Extended
   Controls, :ref:`extended-controls`.
   (``VIDIOC_G_MPEGCOMP`` 和 ``VIDIOC_S_MPEGCOMP`` ioctls。请改用扩展控件，见 :ref:`extended-controls`。)

-  VIDIOC_G_DV_PRESET, VIDIOC_S_DV_PRESET,
   VIDIOC_ENUM_DV_PRESETS and VIDIOC_QUERY_DV_PRESET ioctls. Use
   the DV Timings API (:ref:`dv-timings`).
   (VIDIOC_G_DV_PRESET、VIDIOC_S_DV_PRESET、VIDIOC_ENUM_DV_PRESETS 和 VIDIOC_QUERY_DV_PRESET ioctls。请改用 DV Timings API (:ref:`dv-timings`)。)

-  ``VIDIOC_SUBDEV_G_CROP`` and ``VIDIOC_SUBDEV_S_CROP`` ioctls. Use
   ``VIDIOC_SUBDEV_G_SELECTION`` and ``VIDIOC_SUBDEV_S_SELECTION``,
   :ref:`VIDIOC_SUBDEV_G_SELECTION`.
   (``VIDIOC_SUBDEV_G_CROP`` 和 ``VIDIOC_SUBDEV_S_CROP`` ioctls。请改用 ``VIDIOC_SUBDEV_G_SELECTION`` 和 ``VIDIOC_SUBDEV_S_SELECTION``，见 :ref:`VIDIOC_SUBDEV_G_SELECTION`。)

.. [#f1]
   This is not implemented in XFree86.
   (XFree86 中并未实现这一点。)
