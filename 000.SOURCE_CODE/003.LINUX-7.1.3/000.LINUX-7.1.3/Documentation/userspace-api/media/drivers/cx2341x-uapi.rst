.. SPDX-License-Identifier: GPL-2.0

The cx2341x driver(cx2341x 驱动)
================================

Non-compressed file format(非压缩文件格式)
------------------------------------------

The cx23416 can produce (and the cx23415 can also read) raw YUV output. The
format of a YUV frame is 16x16 linear tiled NV12 (V4L2_PIX_FMT_NV12_16L16).
(cx23416 可以产生(cx23415 还可以读取)原始 YUV 输出。YUV 帧的格式是 16x16 线性平铺的 NV12 (V4L2_PIX_FMT_NV12_16L16)。)

The format is YUV 4:2:0 which uses 1 Y byte per pixel and 1 U and V byte per
four pixels.
(该格式为 YUV 4:2:0，每个像素使用 1 个 Y 字节，每四个像素使用 1 个 U 字节和 1 个 V 字节。)

The data is encoded as two macroblock planes, the first containing the Y
values, the second containing UV macroblocks.
(数据被编码为两个宏块平面，第一个包含 Y 值，第二个包含 UV 宏块。)

The Y plane is divided into blocks of 16x16 pixels from left to right
and from top to bottom. Each block is transmitted in turn, line-by-line.
(Y 平面从左到右、从上到下被划分为 16x16 像素的块。每个块按顺序逐行传输。)

So the first 16 bytes are the first line of the top-left block, the
second 16 bytes are the second line of the top-left block, etc. After
transmitting this block the first line of the block on the right to the
first block is transmitted, etc.
(因此，前 16 个字节是左上角块的第一行，第二个 16 字节是左上角块的第二行，依此类推。传输完这个块之后，接着传输第一块右侧块的第一行，依此类推。)

The UV plane is divided into blocks of 16x8 UV values going from left
to right, top to bottom. Each block is transmitted in turn, line-by-line.
(UV 平面从左到右、从上到下被划分为 16x8 个 UV 值的块。每个块按顺序逐行传输。)

So the first 16 bytes are the first line of the top-left block and
contain 8 UV value pairs (16 bytes in total). The second 16 bytes are the
second line of 8 UV pairs of the top-left block, etc. After transmitting
this block the first line of the block on the right to the first block is
transmitted, etc.
(因此，前 16 个字节是左上角块的第一行，包含 8 个 UV 值对(总共 16 个字节)。第二个 16 字节是左上角块 8 个 UV 对的第二行，依此类推。传输完这个块之后，接着传输第一块右侧块的第一行，依此类推。)

The code below is given as an example on how to convert V4L2_PIX_FMT_NV12_16L16
to separate Y, U and V planes. This code assumes frames of 720x576 (PAL) pixels.
(下面的代码给出了如何将 V4L2_PIX_FMT_NV12_16L16 转换为独立的 Y、U 和 V 平面的示例。此代码假定帧为 720x576 (PAL) 像素。)

The width of a frame is always 720 pixels, regardless of the actual specified
width.
(无论实际指定的宽度是多少，帧的宽度始终为 720 像素。)

If the height is not a multiple of 32 lines, then the captured video is
missing macroblocks at the end and is unusable. So the height must be a
multiple of 32.
(如果高度不是 32 行的倍数，那么捕获的视频在末尾会缺失宏块，无法使用。因此高度必须是 32 的倍数。)

Raw format c example(原始格式 C 语言示例)
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

.. code-block:: c

	#include <stdio.h>
	#include <stdlib.h>
	#include <string.h>

	static unsigned char frame[576*720*3/2];
	static unsigned char framey[576*720];
	static unsigned char frameu[576*720 / 4];
	static unsigned char framev[576*720 / 4];

	static void de_macro_y(unsigned char* dst, unsigned char *src, int dstride, int w, int h)
	{
	unsigned int y, x, i;

	// descramble Y plane
	// dstride = 720 = w
	// The Y plane is divided into blocks of 16x16 pixels
	// Each block in transmitted in turn, line-by-line.
	for (y = 0; y < h; y += 16) {
		for (x = 0; x < w; x += 16) {
		for (i = 0; i < 16; i++) {
			memcpy(dst + x + (y + i) * dstride, src, 16);
			src += 16;
		}
		}
	}
	}

	static void de_macro_uv(unsigned char *dstu, unsigned char *dstv, unsigned char *src, int dstride, int w, int h)
	{
	unsigned int y, x, i;

	// descramble U/V plane
	// dstride = 720 / 2 = w
	// The U/V values are interlaced (UVUV...).
	// Again, the UV plane is divided into blocks of 16x16 UV values.
	// Each block in transmitted in turn, line-by-line.
	for (y = 0; y < h; y += 16) {
		for (x = 0; x < w; x += 8) {
		for (i = 0; i < 16; i++) {
			int idx = x + (y + i) * dstride;

			dstu[idx+0] = src[0];  dstv[idx+0] = src[1];
			dstu[idx+1] = src[2];  dstv[idx+1] = src[3];
			dstu[idx+2] = src[4];  dstv[idx+2] = src[5];
			dstu[idx+3] = src[6];  dstv[idx+3] = src[7];
			dstu[idx+4] = src[8];  dstv[idx+4] = src[9];
			dstu[idx+5] = src[10]; dstv[idx+5] = src[11];
			dstu[idx+6] = src[12]; dstv[idx+6] = src[13];
			dstu[idx+7] = src[14]; dstv[idx+7] = src[15];
			src += 16;
		}
		}
	}
	}

	/*************************************************************************/
	int main(int argc, char **argv)
	{
	FILE *fin;
	int i;

	if (argc == 1) fin = stdin;
	else fin = fopen(argv[1], "r");

	if (fin == NULL) {
		fprintf(stderr, "cannot open input\n");
		exit(-1);
	}
	while (fread(frame, sizeof(frame), 1, fin) == 1) {
		de_macro_y(framey, frame, 720, 720, 576);
		de_macro_uv(frameu, framev, frame + 720 * 576, 720 / 2, 720 / 2, 576 / 2);
		fwrite(framey, sizeof(framey), 1, stdout);
		fwrite(framev, sizeof(framev), 1, stdout);
		fwrite(frameu, sizeof(frameu), 1, stdout);
	}
	fclose(fin);
	return 0;
	}


Format of embedded V4L2_MPEG_STREAM_VBI_FMT_IVTV VBI data(嵌入式 V4L2_MPEG_STREAM_VBI_FMT_IVTV VBI 数据的格式)
--------------------------------------------------------------------------------------------------------------

Author: Hans Verkuil <hverkuil@kernel.org>
(作者: Hans Verkuil <hverkuil@kernel.org>)


This section describes the V4L2_MPEG_STREAM_VBI_FMT_IVTV format of the VBI data
embedded in an MPEG-2 program stream. This format is in part dictated by some
hardware limitations of the ivtv driver (the driver for the Conexant cx23415/6
chips), in particular a maximum size for the VBI data. Anything longer is cut
off when the MPEG stream is played back through the cx23415.
(本节描述嵌入在 MPEG-2 节目流中的 VBI 数据的 V4L2_MPEG_STREAM_VBI_FMT_IVTV 格式。该格式部分取决于 ivtv 驱动(Conexant cx23415/6 芯片的驱动)的一些硬件限制，特别是 VBI 数据的最大尺寸。当 MPEG 流通过 cx23415 播放时，任何超出的部分都会被截断。)

The advantage of this format is it is very compact and that all VBI data for
all lines can be stored while still fitting within the maximum allowed size.
(这种格式的优点是非常紧凑，并且可以存储所有行的全部 VBI 数据，同时仍能容纳在允许的最大尺寸之内。)

The stream ID of the VBI data is 0xBD. The maximum size of the embedded data is
4 + 43 * 36, which is 4 bytes for a header and 2 * 18 VBI lines with a 1 byte
header and a 42 bytes payload each. Anything beyond this limit is cut off by
the cx23415/6 firmware. Besides the data for the VBI lines we also need 36 bits
for a bitmask determining which lines are captured and 4 bytes for a magic cookie,
signifying that this data package contains V4L2_MPEG_STREAM_VBI_FMT_IVTV VBI data.
If all lines are used, then there is no longer room for the bitmask. To solve this
two different magic numbers were introduced:
(VBI 数据的流 ID 是 0xBD。嵌入数据的最大尺寸为 4 + 43 * 36，即 4 个字节的首部，加上 2 * 18 条 VBI 行，每行有 1 个字节的首部和 42 个字节的负载。超出此限制的任何内容都会被 cx23415/6 固件截断。除了 VBI 行的数据之外，我们还需要 36 位来构成一个位掩码，用于确定捕获了哪些行，以及 4 个字节来构成一个魔数 (magic cookie)，表明此数据包包含 V4L2_MPEG_STREAM_VBI_FMT_IVTV VBI 数据。如果使用了所有行，那么就没有空间容纳位掩码了。为了解决这个问题，引入了两个不同的魔数：)

'itv0': After this magic number two unsigned longs follow. Bits 0-17 of the first
unsigned long denote which lines of the first field are captured. Bits 18-31 of
the first unsigned long and bits 0-3 of the second unsigned long are used for the
second field.
('itv0': 在此魔数之后是两个 unsigned long。第一个 unsigned long 的第 0-17 位表示第一场中捕获了哪些行。第一个 unsigned long 的第 18-31 位和第二个 unsigned long 的第 0-3 位用于第二场。)

'ITV0': This magic number assumes all VBI lines are captured, i.e. it implicitly
implies that the bitmasks are 0xffffffff and 0xf.
('ITV0': 此魔数假定所有 VBI 行都被捕获，也就是说它隐含地表示位掩码为 0xffffffff 和 0xf。)

After these magic cookies (and the 8 byte bitmask in case of cookie 'itv0') the
captured VBI lines start:
(在这些魔数之后(对于魔数 'itv0' 还有 8 字节的位掩码)，被捕获的 VBI 行开始：)

For each line the least significant 4 bits of the first byte contain the data type.
Possible values are shown in the table below. The payload is in the following 42
bytes.
(对于每一行，第一个字节的最低 4 位包含数据类型。可能的取值见下表。负载位于随后的 42 个字节中。)

Here is the list of possible data types:
(以下是可能的数据类型列表：)

.. code-block:: c

	#define IVTV_SLICED_TYPE_TELETEXT       0x1     // Teletext (uses lines 6-22 for PAL)
	#define IVTV_SLICED_TYPE_CC             0x4     // Closed Captions (line 21 NTSC)
	#define IVTV_SLICED_TYPE_WSS            0x5     // Wide Screen Signal (line 23 PAL)
	#define IVTV_SLICED_TYPE_VPS            0x7     // Video Programming System (PAL) (line 16)

