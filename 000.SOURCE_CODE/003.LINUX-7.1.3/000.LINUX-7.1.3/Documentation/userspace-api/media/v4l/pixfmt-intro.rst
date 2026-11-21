.. SPDX-License-Identifier: GFDL-1.1-no-invariants-or-later
.. c:namespace:: V4L

************************************
Standard Image Formats(标准图像格式)
************************************

In order to exchange images between drivers and applications, it is
necessary to have standard image data formats which both sides will
interpret the same way. V4L2 includes several such formats, and this
section is intended to be an unambiguous specification of the standard
image data formats in V4L2.
(为了在驱动与应用之间交换图像，双方必须有一套以相同方式解释的标准图像数据
格式。V4L2 包含若干这样的格式，本节旨在对 V4L2 中的标准图像数据格式给出
一个无歧义的规范。)

V4L2 drivers are not limited to these formats, however. Driver-specific
formats are possible. In that case the application may depend on a codec
to convert images to one of the standard formats when needed. But the
data can still be stored and retrieved in the proprietary format. For
example, a device may support a proprietary compressed format.
Applications can still capture and save the data in the compressed
format, saving much disk space, and later use a codec to convert the
images to the X Windows screen format when the video is to be displayed.
(不过，V4L2 驱动并不限于这些格式。驱动可以定义自己的专有格式。这种情况下，
应用在需要时可以依靠编解码器 (codec) 把图像转换为某种标准格式，而数据仍然
可以用专有格式存储和读取。例如，设备可能支持一种专有的压缩格式。应用仍然
可以以压缩格式采集并保存数据，从而节省大量磁盘空间，稍后在需要显示视频时
再用编解码器把图像转换为 X Windows 屏幕格式。)

Even so, ultimately, some standard formats are needed, so the V4L2
specification would not be complete without well-defined standard
formats.
(即便如此，归根结底还是需要一些标准格式，因此如果没有定义良好的标准格式，
V4L2 规范就不算完整。)

The V4L2 standard formats are mainly uncompressed formats. The pixels
are always arranged in memory from left to right, and from top to
bottom. The first byte of data in the image buffer is always for the
leftmost pixel of the topmost row. Following that is the pixel
immediately to its right, and so on until the end of the top row of
pixels. Following the rightmost pixel of the row there may be zero or
more bytes of padding to guarantee that each row of pixel data has a
certain alignment. Following the pad bytes, if any, is data for the
leftmost pixel of the second row from the top, and so on. The last row
has just as many pad bytes after it as the other rows.
(V4L2 标准格式主要是未压缩格式。像素在内存中总是从左到右、从上到下排列。
图像缓冲区中数据的第一个字节总是最顶行最左侧像素的数据。紧随其后的是它
右侧的像素，依此类推，直到顶行像素结束。在这一行最右侧像素之后，可能有
零个或多个填充字节，以保证每行像素数据具有一定的对齐。填充字节 (如果有)
之后是从顶部起第二行最左侧像素的数据，依此类推。最后一行之后的填充字节
数量与其他行相同。)

In V4L2 each format has an identifier which looks like ``PIX_FMT_XXX``,
defined in the :ref:`videodev2.h <videodev>` header file. These
identifiers represent
:ref:`four character (FourCC) codes <v4l2-fourcc>` which are also
listed below, however they are not the same as those used in the Windows
world.
(在 V4L2 中，每种格式都有一个形如 ``PIX_FMT_XXX`` 的标识符，定义在
:ref:`videodev2.h <videodev>` 头文件中。这些标识符表示
:ref:`four character (FourCC) codes <v4l2-fourcc>` ，它们也在下面列出，
但与 Windows 世界中使用的那些并不相同。)

For some formats, data is stored in separate, discontiguous memory
buffers. Those formats are identified by a separate set of FourCC codes
and are referred to as "multi-planar formats". For example, a
:ref:`YUV422 <V4L2-PIX-FMT-YUV422M>` frame is normally stored in one
memory buffer, but it can also be placed in two or three separate
buffers, with Y component in one buffer and CbCr components in another
in the 2-planar version or with each component in its own buffer in the
3-planar case. Those sub-buffers are referred to as "*planes*".
(对于某些格式，数据存储在分开的、不连续的内存缓冲区中。这些格式由一组独立
的 FourCC 码标识，被称为"多平面格式"。例如，:ref:`YUV422 <V4L2-PIX-FMT-YUV422M>`
帧通常存储在一个内存缓冲区中，但也可以放在两三个单独的缓冲区中：双平面
版本把 Y 分量放在一个缓冲区、CbCr 分量放在另一个缓冲区；三平面情况下每个
分量都有自己的缓冲区。这些子缓冲区被称为"*平面 (planes)*"。)
