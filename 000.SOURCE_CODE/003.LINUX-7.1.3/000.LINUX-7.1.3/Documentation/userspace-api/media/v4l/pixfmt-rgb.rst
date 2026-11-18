.. SPDX-License-Identifier: GFDL-1.1-no-invariants-or-later
.. c:namespace:: V4L

.. _pixfmt-rgb:

*********************
RGB Formats(RGB 格式)
*********************

These formats encode each pixel as a triplet of RGB values. They are packed
formats, meaning that the RGB values for one pixel are stored consecutively in
memory and each pixel consumes an integer number of bytes. When the number of
bits required to store a pixel is not aligned to a byte boundary, the data is
padded with additional bits to fill the remaining byte.
(这些格式把每个像素编码为一个 RGB 值三元组。它们是紧缩格式 (packed formats)，即一个像素的 RGB 值在内存中连续存放，并且每个像素占用整数个字节。当存储一个像素所需的位数没有对齐到字节边界时，会填充额外的位来补满剩余的字节。)

The formats differ by the number of bits per RGB component (typically but not
always the same for all components), the order of components in memory, and the
presence of an alpha component or additional padding bits.
(这些格式的差别在于：每个 RGB 分量的位数 (通常对所有分量相同，但并非总是如此)、各分量在内存中的排列顺序，以及是否存在 alpha 分量或额外的填充位。)

The usage and value of the alpha bits in formats that support them (named ARGB
or a permutation thereof, collectively referred to as alpha formats) depend on
the device type and hardware operation. :ref:`Capture <capture>` devices
(including capture queues of mem-to-mem devices) fill the alpha component in
memory. When the device captures an alpha channel the alpha component will have
a meaningful value. Otherwise, when the device doesn't capture an alpha channel
but can set the alpha bit to a user-configurable value, the
:ref:`V4L2_CID_ALPHA_COMPONENT <v4l2-alpha-component>` control is used to
specify that alpha value, and the alpha component of all pixels will be set to
the value specified by that control. Otherwise a corresponding format without
an alpha component (XRGB or XBGR) must be used instead of an alpha format.
(在支持 alpha 位的格式 (名为 ARGB 或其排列变体，统称为 alpha 格式) 中，alpha 位的用法和取值取决于设备类型和硬件操作。:ref:`捕获 <capture>` 设备 (包括 mem-to-mem 设备的捕获队列) 在内存中填充 alpha 分量。当设备捕获 alpha 通道时，alpha 分量将具有有意义的值。否则，当设备不捕获 alpha 通道但可以把 alpha 位设置为用户可配置的值时，就使用 :ref:`V4L2_CID_ALPHA_COMPONENT <v4l2-alpha-component>` 控件来指定该 alpha 值，并且所有像素的 alpha 分量都会被设置为该控件指定的值。否则，必须改用对应的、不含 alpha 分量的格式 (XRGB 或 XBGR) 来代替 alpha 格式。)

:ref:`Output <output>` devices (including output queues of mem-to-mem devices
and :ref:`video output overlay <osd>` devices) read the alpha component from
memory. When the device processes the alpha channel the alpha component must be
filled with meaningful values by applications. Otherwise a corresponding format
without an alpha component (XRGB or XBGR) must be used instead of an alpha
format.
(:ref:`输出 <output>` 设备 (包括 mem-to-mem 设备的输出队列和 :ref:`视频输出叠加 <osd>` 设备) 从内存中读取 alpha 分量。当设备处理 alpha 通道时，alpha 分量必须由应用程序填入有意义的值。否则，必须改用对应的、不含 alpha 分量的格式 (XRGB 或 XBGR) 来代替 alpha 格式。)

Formats that contain padding bits are named XRGB (or a permutation thereof).
The padding bits contain undefined values and must be ignored by applications,
devices and drivers, for both :ref:`capture` and :ref:`output` devices.
(含有填充位的格式被命名为 XRGB (或其排列变体)。填充位包含未定义的值，应用程序、设备和驱动都必须忽略它们，对 :ref:`capture` 和 :ref:`output` 设备皆然。)

.. note::

   - In all the tables that follow, bit 7 is the most significant bit in a byte.
     (在下文的所有表格中，位 7 是一个字节中的最高有效位。)
   - 'r', 'g' and 'b' denote bits of the red, green and blue components
     respectively. 'a' denotes bits of the alpha component (if supported by the
     format), and 'x' denotes padding bits.
     ('r'、'g' 和 'b' 分别表示红、绿、蓝分量的位。'a' 表示 alpha 分量的位 (如果该格式支持)，'x' 表示填充位。)


Less Than 8 Bits Per Component(每分量不足 8 位)
===============================================

These formats store an RGB triplet in one, two or four bytes. They are named
based on the order of the RGB components as seen in a 8-, 16- or 32-bit word,
which is then stored in memory in little endian byte order (unless otherwise
noted by the presence of bit 31 in the 4CC value), and on the number of bits
for each component. For instance, the RGB565 format stores a pixel in a 16-bit
word [15:0] laid out at as [R\ :sub:`4` R\ :sub:`3` R\ :sub:`2` R\ :sub:`1`
R\ :sub:`0` G\ :sub:`5` G\ :sub:`4` G\ :sub:`3` G\ :sub:`2` G\ :sub:`1`
G\ :sub:`0` B\ :sub:`4` B\ :sub:`3` B\ :sub:`2` B\ :sub:`1` B\ :sub:`0`], and
stored in memory in two bytes, [R\ :sub:`4` R\ :sub:`3` R\ :sub:`2` R\ :sub:`1`
R\ :sub:`0` G\ :sub:`5` G\ :sub:`4` G\ :sub:`3`] followed by [G\ :sub:`2`
G\ :sub:`1` G\ :sub:`0` B\ :sub:`4` B\ :sub:`3` B\ :sub:`2` B\ :sub:`1`
B\ :sub:`0`].
(这些格式把一个 RGB 三元组存放在一、二或四个字节中。它们依据两件事命名：一是把 8、16 或 32 位字按小端字节序存入内存时 (除非 4CC 值中位 31 的出现另有说明) 所看到的 RGB 分量顺序，二是每个分量的位数。例如，RGB565 格式把一个像素存放在 16 位字 [15:0] 中，位布局即上文方括号里列出的 [R4 R3 R2 R1 R0 G5 G4 G3 G2 G1 G0 B4 B3 B2 B1 B0]，并在内存中以两个字节存放：先存放 [R4 R3 R2 R1 R0 G5 G4 G3]，再存放 [G2 G1 G0 B4 B3 B2 B1 B0]。)

.. raw:: latex

    \begingroup
    \tiny
    \setlength{\tabcolsep}{2pt}

.. tabularcolumns:: |p{2.8cm}|p{2.0cm}|p{0.22cm}|p{0.22cm}|p{0.22cm}|p{0.22cm}|p{0.22cm}|p{0.22cm}|p{0.22cm}|p{0.22cm}|p{0.22cm}|p{0.22cm}|p{0.22cm}|p{0.22cm}|p{0.22cm}|p{0.22cm}|p{0.22cm}|p{0.22cm}|p{0.22cm}|p{0.22cm}|p{0.22cm}|p{0.22cm}|p{0.22cm}|p{0.22cm}|p{0.22cm}|p{0.22cm}|p{0.22cm}|p{0.22cm}|p{0.22cm}|p{0.22cm}|p{0.22cm}|p{0.22cm}|p{0.22cm}|p{0.22cm}|


.. flat-table:: RGB Formats With Less Than 8 Bits Per Component
    :header-rows:  2
    :stub-columns: 0

    * - Identifier(标识符)
      - Code(代码)
      - :cspan:`7` Byte 0 in memory(内存中的字节 0)
      - :cspan:`7` Byte 1(字节 1)
      - :cspan:`7` Byte 2(字节 2)
      - :cspan:`7` Byte 3(字节 3)
    * -
      -
      - 7
      - 6
      - 5
      - 4
      - 3
      - 2
      - 1
      - 0

      - 7
      - 6
      - 5
      - 4
      - 3
      - 2
      - 1
      - 0

      - 7
      - 6
      - 5
      - 4
      - 3
      - 2
      - 1
      - 0

      - 7
      - 6
      - 5
      - 4
      - 3
      - 2
      - 1
      - 0
    * .. _V4L2-PIX-FMT-RGB332:

      - ``V4L2_PIX_FMT_RGB332``
      - 'RGB1'

      - r\ :sub:`2`
      - r\ :sub:`1`
      - r\ :sub:`0`
      - g\ :sub:`2`
      - g\ :sub:`1`
      - g\ :sub:`0`
      - b\ :sub:`1`
      - b\ :sub:`0`
      -
    * .. _V4L2-PIX-FMT-ARGB444:

      - ``V4L2_PIX_FMT_ARGB444``
      - 'AR12'

      - g\ :sub:`3`
      - g\ :sub:`2`
      - g\ :sub:`1`
      - g\ :sub:`0`
      - b\ :sub:`3`
      - b\ :sub:`2`
      - b\ :sub:`1`
      - b\ :sub:`0`

      - a\ :sub:`3`
      - a\ :sub:`2`
      - a\ :sub:`1`
      - a\ :sub:`0`
      - r\ :sub:`3`
      - r\ :sub:`2`
      - r\ :sub:`1`
      - r\ :sub:`0`
      -
    * .. _V4L2-PIX-FMT-XRGB444:

      - ``V4L2_PIX_FMT_XRGB444``
      - 'XR12'

      - g\ :sub:`3`
      - g\ :sub:`2`
      - g\ :sub:`1`
      - g\ :sub:`0`
      - b\ :sub:`3`
      - b\ :sub:`2`
      - b\ :sub:`1`
      - b\ :sub:`0`

      - x
      - x
      - x
      - x
      - r\ :sub:`3`
      - r\ :sub:`2`
      - r\ :sub:`1`
      - r\ :sub:`0`
      -
    * .. _V4L2-PIX-FMT-RGBA444:

      - ``V4L2_PIX_FMT_RGBA444``
      - 'RA12'

      - b\ :sub:`3`
      - b\ :sub:`2`
      - b\ :sub:`1`
      - b\ :sub:`0`
      - a\ :sub:`3`
      - a\ :sub:`2`
      - a\ :sub:`1`
      - a\ :sub:`0`

      - r\ :sub:`3`
      - r\ :sub:`2`
      - r\ :sub:`1`
      - r\ :sub:`0`
      - g\ :sub:`3`
      - g\ :sub:`2`
      - g\ :sub:`1`
      - g\ :sub:`0`
      -
    * .. _V4L2-PIX-FMT-RGBX444:

      - ``V4L2_PIX_FMT_RGBX444``
      - 'RX12'

      - b\ :sub:`3`
      - b\ :sub:`2`
      - b\ :sub:`1`
      - b\ :sub:`0`
      - x
      - x
      - x
      - x

      - r\ :sub:`3`
      - r\ :sub:`2`
      - r\ :sub:`1`
      - r\ :sub:`0`
      - g\ :sub:`3`
      - g\ :sub:`2`
      - g\ :sub:`1`
      - g\ :sub:`0`
      -
    * .. _V4L2-PIX-FMT-ABGR444:

      - ``V4L2_PIX_FMT_ABGR444``
      - 'AB12'

      - g\ :sub:`3`
      - g\ :sub:`2`
      - g\ :sub:`1`
      - g\ :sub:`0`
      - r\ :sub:`3`
      - r\ :sub:`2`
      - r\ :sub:`1`
      - r\ :sub:`0`

      - a\ :sub:`3`
      - a\ :sub:`2`
      - a\ :sub:`1`
      - a\ :sub:`0`
      - b\ :sub:`3`
      - b\ :sub:`2`
      - b\ :sub:`1`
      - b\ :sub:`0`
      -
    * .. _V4L2-PIX-FMT-XBGR444:

      - ``V4L2_PIX_FMT_XBGR444``
      - 'XB12'

      - g\ :sub:`3`
      - g\ :sub:`2`
      - g\ :sub:`1`
      - g\ :sub:`0`
      - r\ :sub:`3`
      - r\ :sub:`2`
      - r\ :sub:`1`
      - r\ :sub:`0`

      - x
      - x
      - x
      - x
      - b\ :sub:`3`
      - b\ :sub:`2`
      - b\ :sub:`1`
      - b\ :sub:`0`
      -
    * .. _V4L2-PIX-FMT-BGRA444:

      - ``V4L2_PIX_FMT_BGRA444``
      - 'BA12'

      - r\ :sub:`3`
      - r\ :sub:`2`
      - r\ :sub:`1`
      - r\ :sub:`0`
      - a\ :sub:`3`
      - a\ :sub:`2`
      - a\ :sub:`1`
      - a\ :sub:`0`

      - b\ :sub:`3`
      - b\ :sub:`2`
      - b\ :sub:`1`
      - b\ :sub:`0`
      - g\ :sub:`3`
      - g\ :sub:`2`
      - g\ :sub:`1`
      - g\ :sub:`0`
      -
    * .. _V4L2-PIX-FMT-BGRX444:

      - ``V4L2_PIX_FMT_BGRX444``
      - 'BX12'

      - r\ :sub:`3`
      - r\ :sub:`2`
      - r\ :sub:`1`
      - r\ :sub:`0`
      - x
      - x
      - x
      - x

      - b\ :sub:`3`
      - b\ :sub:`2`
      - b\ :sub:`1`
      - b\ :sub:`0`
      - g\ :sub:`3`
      - g\ :sub:`2`
      - g\ :sub:`1`
      - g\ :sub:`0`
      -
    * .. _V4L2-PIX-FMT-ARGB555:

      - ``V4L2_PIX_FMT_ARGB555``
      - 'AR15'

      - g\ :sub:`2`
      - g\ :sub:`1`
      - g\ :sub:`0`
      - b\ :sub:`4`
      - b\ :sub:`3`
      - b\ :sub:`2`
      - b\ :sub:`1`
      - b\ :sub:`0`

      - a
      - r\ :sub:`4`
      - r\ :sub:`3`
      - r\ :sub:`2`
      - r\ :sub:`1`
      - r\ :sub:`0`
      - g\ :sub:`4`
      - g\ :sub:`3`
      -
    * .. _V4L2-PIX-FMT-XRGB555:

      - ``V4L2_PIX_FMT_XRGB555``
      - 'XR15'

      - g\ :sub:`2`
      - g\ :sub:`1`
      - g\ :sub:`0`
      - b\ :sub:`4`
      - b\ :sub:`3`
      - b\ :sub:`2`
      - b\ :sub:`1`
      - b\ :sub:`0`

      - x
      - r\ :sub:`4`
      - r\ :sub:`3`
      - r\ :sub:`2`
      - r\ :sub:`1`
      - r\ :sub:`0`
      - g\ :sub:`4`
      - g\ :sub:`3`
      -
    * .. _V4L2-PIX-FMT-RGBA555:

      - ``V4L2_PIX_FMT_RGBA555``
      - 'RA15'

      - g\ :sub:`1`
      - g\ :sub:`0`
      - b\ :sub:`4`
      - b\ :sub:`3`
      - b\ :sub:`2`
      - b\ :sub:`1`
      - b\ :sub:`0`
      - a

      - r\ :sub:`4`
      - r\ :sub:`3`
      - r\ :sub:`2`
      - r\ :sub:`1`
      - r\ :sub:`0`
      - g\ :sub:`4`
      - g\ :sub:`3`
      - g\ :sub:`2`
      -
    * .. _V4L2-PIX-FMT-RGBX555:

      - ``V4L2_PIX_FMT_RGBX555``
      - 'RX15'

      - g\ :sub:`1`
      - g\ :sub:`0`
      - b\ :sub:`4`
      - b\ :sub:`3`
      - b\ :sub:`2`
      - b\ :sub:`1`
      - b\ :sub:`0`
      - x

      - r\ :sub:`4`
      - r\ :sub:`3`
      - r\ :sub:`2`
      - r\ :sub:`1`
      - r\ :sub:`0`
      - g\ :sub:`4`
      - g\ :sub:`3`
      - g\ :sub:`2`
      -
    * .. _V4L2-PIX-FMT-ABGR555:

      - ``V4L2_PIX_FMT_ABGR555``
      - 'AB15'

      - g\ :sub:`2`
      - g\ :sub:`1`
      - g\ :sub:`0`
      - r\ :sub:`4`
      - r\ :sub:`3`
      - r\ :sub:`2`
      - r\ :sub:`1`
      - r\ :sub:`0`

      - a
      - b\ :sub:`4`
      - b\ :sub:`3`
      - b\ :sub:`2`
      - b\ :sub:`1`
      - b\ :sub:`0`
      - g\ :sub:`4`
      - g\ :sub:`3`
      -
    * .. _V4L2-PIX-FMT-XBGR555:

      - ``V4L2_PIX_FMT_XBGR555``
      - 'XB15'

      - g\ :sub:`2`
      - g\ :sub:`1`
      - g\ :sub:`0`
      - r\ :sub:`4`
      - r\ :sub:`3`
      - r\ :sub:`2`
      - r\ :sub:`1`
      - r\ :sub:`0`

      - x
      - b\ :sub:`4`
      - b\ :sub:`3`
      - b\ :sub:`2`
      - b\ :sub:`1`
      - b\ :sub:`0`
      - g\ :sub:`4`
      - g\ :sub:`3`
      -
    * .. _V4L2-PIX-FMT-BGRA555:

      - ``V4L2_PIX_FMT_BGRA555``
      - 'BA15'

      - g\ :sub:`1`
      - g\ :sub:`0`
      - r\ :sub:`4`
      - r\ :sub:`3`
      - r\ :sub:`2`
      - r\ :sub:`1`
      - r\ :sub:`0`
      - a

      - b\ :sub:`4`
      - b\ :sub:`3`
      - b\ :sub:`2`
      - b\ :sub:`1`
      - b\ :sub:`0`
      - g\ :sub:`4`
      - g\ :sub:`3`
      - g\ :sub:`2`
      -
    * .. _V4L2-PIX-FMT-BGRX555:

      - ``V4L2_PIX_FMT_BGRX555``
      - 'BX15'

      - g\ :sub:`1`
      - g\ :sub:`0`
      - r\ :sub:`4`
      - r\ :sub:`3`
      - r\ :sub:`2`
      - r\ :sub:`1`
      - r\ :sub:`0`
      - x

      - b\ :sub:`4`
      - b\ :sub:`3`
      - b\ :sub:`2`
      - b\ :sub:`1`
      - b\ :sub:`0`
      - g\ :sub:`4`
      - g\ :sub:`3`
      - g\ :sub:`2`
      -
    * .. _V4L2-PIX-FMT-RGB565:

      - ``V4L2_PIX_FMT_RGB565``
      - 'RGBP'

      - g\ :sub:`2`
      - g\ :sub:`1`
      - g\ :sub:`0`
      - b\ :sub:`4`
      - b\ :sub:`3`
      - b\ :sub:`2`
      - b\ :sub:`1`
      - b\ :sub:`0`

      - r\ :sub:`4`
      - r\ :sub:`3`
      - r\ :sub:`2`
      - r\ :sub:`1`
      - r\ :sub:`0`
      - g\ :sub:`5`
      - g\ :sub:`4`
      - g\ :sub:`3`
      -
    * .. _V4L2-PIX-FMT-ARGB555X:

      - ``V4L2_PIX_FMT_ARGB555X``
      - 'AR15' | (1 << 31)

      - a
      - r\ :sub:`4`
      - r\ :sub:`3`
      - r\ :sub:`2`
      - r\ :sub:`1`
      - r\ :sub:`0`
      - g\ :sub:`4`
      - g\ :sub:`3`

      - g\ :sub:`2`
      - g\ :sub:`1`
      - g\ :sub:`0`
      - b\ :sub:`4`
      - b\ :sub:`3`
      - b\ :sub:`2`
      - b\ :sub:`1`
      - b\ :sub:`0`
      -
    * .. _V4L2-PIX-FMT-XRGB555X:

      - ``V4L2_PIX_FMT_XRGB555X``
      - 'XR15' | (1 << 31)

      - x
      - r\ :sub:`4`
      - r\ :sub:`3`
      - r\ :sub:`2`
      - r\ :sub:`1`
      - r\ :sub:`0`
      - g\ :sub:`4`
      - g\ :sub:`3`

      - g\ :sub:`2`
      - g\ :sub:`1`
      - g\ :sub:`0`
      - b\ :sub:`4`
      - b\ :sub:`3`
      - b\ :sub:`2`
      - b\ :sub:`1`
      - b\ :sub:`0`
      -
    * .. _V4L2-PIX-FMT-RGB565X:

      - ``V4L2_PIX_FMT_RGB565X``
      - 'RGBR'

      - r\ :sub:`4`
      - r\ :sub:`3`
      - r\ :sub:`2`
      - r\ :sub:`1`
      - r\ :sub:`0`
      - g\ :sub:`5`
      - g\ :sub:`4`
      - g\ :sub:`3`

      - g\ :sub:`2`
      - g\ :sub:`1`
      - g\ :sub:`0`
      - b\ :sub:`4`
      - b\ :sub:`3`
      - b\ :sub:`2`
      - b\ :sub:`1`
      - b\ :sub:`0`
      -
    * .. _V4L2-PIX-FMT-BGR666:

      - ``V4L2_PIX_FMT_BGR666``
      - 'BGRH'

      - b\ :sub:`5`
      - b\ :sub:`4`
      - b\ :sub:`3`
      - b\ :sub:`2`
      - b\ :sub:`1`
      - b\ :sub:`0`
      - g\ :sub:`5`
      - g\ :sub:`4`

      - g\ :sub:`3`
      - g\ :sub:`2`
      - g\ :sub:`1`
      - g\ :sub:`0`
      - r\ :sub:`5`
      - r\ :sub:`4`
      - r\ :sub:`3`
      - r\ :sub:`2`

      - r\ :sub:`1`
      - r\ :sub:`0`
      - x
      - x
      - x
      - x
      - x
      - x

      - x
      - x
      - x
      - x
      - x
      - x
      - x
      - x

.. raw:: latex

    \endgroup


8 Bits Per Component(每分量 8 位)
=================================

These formats store an RGB triplet in three or four bytes. They are named based
on the order of the RGB components as stored in memory, and on the total number
of bits per pixel. For instance, RGB24 format stores a pixel with [R\ :sub:`7`
R\ :sub:`6` R\ :sub:`5` R\ :sub:`4` R\ :sub:`3` R\ :sub:`2` R\ :sub:`1`
R\ :sub:`0`] in the first byte, [G\ :sub:`7` G\ :sub:`6` G\ :sub:`5` G\ :sub:`4`
G\ :sub:`3` G\ :sub:`2` G\ :sub:`1` G\ :sub:`0`] in the second byte and
[B\ :sub:`7` B\ :sub:`6` B\ :sub:`5` B\ :sub:`4` B\ :sub:`3` B\ :sub:`2`
B\ :sub:`1` B\ :sub:`0`] in the third byte. This differs from the DRM format
nomenclature that instead use the order of components as seen in a 24- or
32-bit little endian word.
(这些格式把一个 RGB 三元组存放在三或四个字节中。它们依据内存中存储的 RGB 分量顺序以及每像素总位数命名。例如，RGB24 格式在第一个字节存放 [R7 R6 R5 R4 R3 R2 R1 R0]，在第二个字节存放 [G7 G6 G5 G4 G3 G2 G1 G0]，在第三个字节存放 [B7 B6 B5 B4 B3 B2 B1 B0] (位序即上文方括号所列)。这一点与 DRM 格式命名法不同，后者用的是 24 位或 32 位小端字中所看到的分量顺序。)

.. raw:: latex

    \small

.. flat-table:: RGB Formats With 8 Bits Per Component
    :header-rows:  1
    :stub-columns: 0

    * - Identifier(标识符)
      - Code(代码)
      - Byte 0 in memory(内存中的字节 0)
      - Byte 1(字节 1)
      - Byte 2(字节 2)
      - Byte 3(字节 3)
    * .. _V4L2-PIX-FMT-BGR24:

      - ``V4L2_PIX_FMT_BGR24``
      - 'BGR3'

      - B\ :sub:`7-0`
      - G\ :sub:`7-0`
      - R\ :sub:`7-0`
      -
    * .. _V4L2-PIX-FMT-RGB24:

      - ``V4L2_PIX_FMT_RGB24``
      - 'RGB3'

      - R\ :sub:`7-0`
      - G\ :sub:`7-0`
      - B\ :sub:`7-0`
      -
    * .. _V4L2-PIX-FMT-ABGR32:

      - ``V4L2_PIX_FMT_ABGR32``
      - 'AR24'

      - B\ :sub:`7-0`
      - G\ :sub:`7-0`
      - R\ :sub:`7-0`
      - A\ :sub:`7-0`
    * .. _V4L2-PIX-FMT-XBGR32:

      - ``V4L2_PIX_FMT_XBGR32``
      - 'XR24'

      - B\ :sub:`7-0`
      - G\ :sub:`7-0`
      - R\ :sub:`7-0`
      - X\ :sub:`7-0`
    * .. _V4L2-PIX-FMT-BGRA32:

      - ``V4L2_PIX_FMT_BGRA32``
      - 'RA24'

      - A\ :sub:`7-0`
      - B\ :sub:`7-0`
      - G\ :sub:`7-0`
      - R\ :sub:`7-0`
    * .. _V4L2-PIX-FMT-BGRX32:

      - ``V4L2_PIX_FMT_BGRX32``
      - 'RX24'

      - X\ :sub:`7-0`
      - B\ :sub:`7-0`
      - G\ :sub:`7-0`
      - R\ :sub:`7-0`
    * .. _V4L2-PIX-FMT-RGBA32:

      - ``V4L2_PIX_FMT_RGBA32``
      - 'AB24'

      - R\ :sub:`7-0`
      - G\ :sub:`7-0`
      - B\ :sub:`7-0`
      - A\ :sub:`7-0`
    * .. _V4L2-PIX-FMT-RGBX32:

      - ``V4L2_PIX_FMT_RGBX32``
      - 'XB24'

      - R\ :sub:`7-0`
      - G\ :sub:`7-0`
      - B\ :sub:`7-0`
      - X\ :sub:`7-0`
    * .. _V4L2-PIX-FMT-ARGB32:

      - ``V4L2_PIX_FMT_ARGB32``
      - 'BA24'

      - A\ :sub:`7-0`
      - R\ :sub:`7-0`
      - G\ :sub:`7-0`
      - B\ :sub:`7-0`
    * .. _V4L2-PIX-FMT-XRGB32:

      - ``V4L2_PIX_FMT_XRGB32``
      - 'BX24'

      - X\ :sub:`7-0`
      - R\ :sub:`7-0`
      - G\ :sub:`7-0`
      - B\ :sub:`7-0`

.. raw:: latex

    \normalsize


10 Bits Per Component(每分量 10 位)
===================================

These formats store a 30-bit RGB triplet with an optional 2 bit alpha in four
bytes. They are named based on the order of the RGB components as seen in a
32-bit word, which is then stored in memory in little endian byte order
(unless otherwise noted by the presence of bit 31 in the 4CC value), and on the
number of bits for each component.
(这些格式把一个 30 位 RGB 三元组以及可选的 2 位 alpha 存放在四个字节中。它们依据两件事命名：一是 32 位字中所看到的 RGB 分量顺序，该字随后按小端字节序存入内存 (除非 4CC 值中位 31 的出现另有说明)；二是每个分量的位数。)

.. raw:: latex

    \begingroup
    \tiny
    \setlength{\tabcolsep}{2pt}

.. tabularcolumns:: |p{3.2cm}|p{0.8cm}|p{0.22cm}|p{0.22cm}|p{0.22cm}|p{0.22cm}|p{0.22cm}|p{0.22cm}|p{0.22cm}|p{0.22cm}|p{0.22cm}|p{0.22cm}|p{0.22cm}|p{0.22cm}|p{0.22cm}|p{0.22cm}|p{0.22cm}|p{0.22cm}|p{0.22cm}|p{0.22cm}|p{0.22cm}|p{0.22cm}|p{0.22cm}|p{0.22cm}|p{0.22cm}|p{0.22cm}|p{0.22cm}|p{0.22cm}|p{0.22cm}|p{0.22cm}|p{0.22cm}|p{0.22cm}|p{0.22cm}|p{0.22cm}|


.. flat-table:: RGB Formats 10 Bits Per Color Component
    :header-rows:  2
    :stub-columns: 0

    * - Identifier(标识符)
      - Code(代码)
      - :cspan:`7` Byte 0 in memory(内存中的字节 0)
      - :cspan:`7` Byte 1(字节 1)
      - :cspan:`7` Byte 2(字节 2)
      - :cspan:`7` Byte 3(字节 3)
    * -
      -
      - 7
      - 6
      - 5
      - 4
      - 3
      - 2
      - 1
      - 0

      - 7
      - 6
      - 5
      - 4
      - 3
      - 2
      - 1
      - 0

      - 7
      - 6
      - 5
      - 4
      - 3
      - 2
      - 1
      - 0

      - 7
      - 6
      - 5
      - 4
      - 3
      - 2
      - 1
      - 0
    * .. _V4L2-PIX-FMT-RGBX1010102:

      - ``V4L2_PIX_FMT_RGBX1010102``
      - 'RX30'

      - b\ :sub:`5`
      - b\ :sub:`4`
      - b\ :sub:`3`
      - b\ :sub:`2`
      - b\ :sub:`1`
      - b\ :sub:`0`
      - x
      - x

      - g\ :sub:`3`
      - g\ :sub:`2`
      - g\ :sub:`1`
      - g\ :sub:`0`
      - b\ :sub:`9`
      - b\ :sub:`8`
      - b\ :sub:`7`
      - b\ :sub:`6`

      - r\ :sub:`1`
      - r\ :sub:`0`
      - g\ :sub:`9`
      - g\ :sub:`8`
      - g\ :sub:`7`
      - g\ :sub:`6`
      - g\ :sub:`5`
      - g\ :sub:`4`

      - r\ :sub:`9`
      - r\ :sub:`8`
      - r\ :sub:`7`
      - r\ :sub:`6`
      - r\ :sub:`5`
      - r\ :sub:`4`
      - r\ :sub:`3`
      - r\ :sub:`2`
    * .. _V4L2-PIX-FMT-RGBA1010102:

      - ``V4L2_PIX_FMT_RGBA1010102``
      - 'RA30'

      - b\ :sub:`5`
      - b\ :sub:`4`
      - b\ :sub:`3`
      - b\ :sub:`2`
      - b\ :sub:`1`
      - b\ :sub:`0`
      - a\ :sub:`1`
      - a\ :sub:`0`

      - g\ :sub:`3`
      - g\ :sub:`2`
      - g\ :sub:`1`
      - g\ :sub:`0`
      - b\ :sub:`9`
      - b\ :sub:`8`
      - b\ :sub:`7`
      - b\ :sub:`6`

      - r\ :sub:`1`
      - r\ :sub:`0`
      - g\ :sub:`9`
      - g\ :sub:`8`
      - g\ :sub:`7`
      - g\ :sub:`6`
      - g\ :sub:`5`
      - g\ :sub:`4`

      - r\ :sub:`9`
      - r\ :sub:`8`
      - r\ :sub:`7`
      - r\ :sub:`6`
      - r\ :sub:`5`
      - r\ :sub:`4`
      - r\ :sub:`3`
      - r\ :sub:`2`
    * .. _V4L2-PIX-FMT-ARGB2101010:

      - ``V4L2_PIX_FMT_ARGB2101010``
      - 'AR30'

      - b\ :sub:`7`
      - b\ :sub:`6`
      - b\ :sub:`5`
      - b\ :sub:`4`
      - b\ :sub:`3`
      - b\ :sub:`2`
      - b\ :sub:`1`
      - b\ :sub:`0`

      - g\ :sub:`5`
      - g\ :sub:`4`
      - g\ :sub:`3`
      - g\ :sub:`2`
      - g\ :sub:`1`
      - g\ :sub:`0`
      - b\ :sub:`9`
      - b\ :sub:`8`

      - r\ :sub:`3`
      - r\ :sub:`2`
      - r\ :sub:`1`
      - r\ :sub:`0`
      - g\ :sub:`9`
      - g\ :sub:`8`
      - g\ :sub:`7`
      - g\ :sub:`6`

      - a\ :sub:`1`
      - a\ :sub:`0`
      - r\ :sub:`9`
      - r\ :sub:`8`
      - r\ :sub:`7`
      - r\ :sub:`6`
      - r\ :sub:`5`
      - r\ :sub:`4`

.. raw:: latex

    \endgroup

12 Bits Per Component(每分量 12 位)
===================================

These formats store an RGB triplet in six or eight bytes, with 12 bits per component.
Expand the bits per component to 16 bits, data in the high bits, zeros in the low bits,
arranged in little endian order.
(这些格式把一个 RGB 三元组存放在六或八个字节中，每分量 12 位。每个分量的位扩展到 16 位：数据放在高位，低位补零，按小端序排列。)

.. raw:: latex

    \small

.. flat-table:: RGB Formats With 12 Bits Per Component
    :header-rows:  1

    * - Identifier(标识符)
      - Code(代码)
      - Byte 1-0(字节 1-0)
      - Byte 3-2(字节 3-2)
      - Byte 5-4(字节 5-4)
      - Byte 7-6(字节 7-6)
    * .. _V4L2-PIX-FMT-BGR48-12:

      - ``V4L2_PIX_FMT_BGR48_12``
      - 'B312'

      - B\ :sub:`15-4`
      - G\ :sub:`15-4`
      - R\ :sub:`15-4`
      -
    * .. _V4L2-PIX-FMT-ABGR64-12:

      - ``V4L2_PIX_FMT_ABGR64_12``
      - 'B412'

      - B\ :sub:`15-4`
      - G\ :sub:`15-4`
      - R\ :sub:`15-4`
      - A\ :sub:`15-4`

.. raw:: latex

    \normalsize

16 Bits Per Component(每分量 16 位)
===================================

These formats store an RGB triplet in six bytes, with 16 bits per component
stored in memory in little endian byte order. They are named based on the order
of the RGB components as stored in memory. For instance, RGB48 stores R\
:sub:`7:0` and R\ :sub:`15:8` in bytes 0 and 1 respectively. This differs from
the DRM format nomenclature that instead uses the order of components as seen in
the 48-bits little endian word.
(这些格式把一个 RGB 三元组存放在六个字节中，每分量 16 位，按小端字节序存入内存。它们依据内存中存储的 RGB 分量顺序命名。例如，RGB48 把 R\ :sub:`7:0` 和 R\ :sub:`15:8` 分别存放在字节 0 和字节 1 中。这一点与 DRM 格式命名法不同，后者用的是 48 位小端字中所看到的分量顺序。)

.. raw:: latex

    \small

.. flat-table:: RGB Formats With 16 Bits Per Component
    :header-rows:  1

    * - Identifier(标识符)
      - Code(代码)
      - Byte 0(字节 0)
      - Byte 1(字节 1)
      - Byte 2(字节 2)
      - Byte 3(字节 3)
      - Byte 4(字节 4)
      - Byte 5(字节 5)

    * .. _V4L2-PIX-FMT-BGR48:

      - ``V4L2_PIX_FMT_BGR48``
      - 'BGR6'

      - B\ :sub:`7-0`
      - B\ :sub:`15-8`
      - G\ :sub:`7-0`
      - G\ :sub:`15-8`
      - R\ :sub:`7-0`
      - R\ :sub:`15-8`

    * .. _V4L2-PIX-FMT-RGB48:

      - ``V4L2_PIX_FMT_RGB48``
      - 'RGB6'

      - R\ :sub:`7-0`
      - R\ :sub:`15-8`
      - G\ :sub:`7-0`
      - G\ :sub:`15-8`
      - B\ :sub:`7-0`
      - B\ :sub:`15-8`

.. raw:: latex

    \normalsize

Deprecated RGB Formats(已弃用的 RGB 格式)
=========================================

Formats defined in :ref:`pixfmt-rgb-deprecated` are deprecated and must not be
used by new drivers. They are documented here for reference. The meaning of
their alpha bits ``(a)`` is ill-defined and they are interpreted as in either
the corresponding ARGB or XRGB format, depending on the driver.
(:ref:`pixfmt-rgb-deprecated` 中定义的格式已被弃用，新驱动不得使用。这里记录它们仅供查阅。它们 alpha 位 ``(a)`` 的含义没有明确界定，具体按对应的 ARGB 还是 XRGB 格式来解释，取决于驱动。)

.. raw:: latex

    \begingroup
    \tiny
    \setlength{\tabcolsep}{2pt}

.. tabularcolumns:: |p{2.6cm}|p{0.70cm}|p{0.22cm}|p{0.22cm}|p{0.22cm}|p{0.22cm}|p{0.22cm}|p{0.22cm}|p{0.22cm}|p{0.22cm}|p{0.22cm}|p{0.22cm}|p{0.22cm}|p{0.22cm}|p{0.22cm}|p{0.22cm}|p{0.22cm}|p{0.22cm}|p{0.22cm}|p{0.22cm}|p{0.22cm}|p{0.22cm}|p{0.22cm}|p{0.22cm}|p{0.22cm}|p{0.22cm}|p{0.22cm}|p{0.22cm}|p{0.22cm}|p{0.22cm}|p{0.22cm}|p{0.22cm}|p{0.22cm}|p{0.22cm}|

.. _pixfmt-rgb-deprecated:

.. flat-table:: Deprecated Packed RGB Image Formats
    :header-rows:  2
    :stub-columns: 0

    * - Identifier(标识符)
      - Code(代码)
      - :cspan:`7` Byte 0 in memory(内存中的字节 0)

      - :cspan:`7` Byte 1(字节 1)

      - :cspan:`7` Byte 2(字节 2)

      - :cspan:`7` Byte 3(字节 3)
    * -
      -
      - 7
      - 6
      - 5
      - 4
      - 3
      - 2
      - 1
      - 0

      - 7
      - 6
      - 5
      - 4
      - 3
      - 2
      - 1
      - 0

      - 7
      - 6
      - 5
      - 4
      - 3
      - 2
      - 1
      - 0

      - 7
      - 6
      - 5
      - 4
      - 3
      - 2
      - 1
      - 0
    * .. _V4L2-PIX-FMT-RGB444:

      - ``V4L2_PIX_FMT_RGB444``
      - 'R444'

      - g\ :sub:`3`
      - g\ :sub:`2`
      - g\ :sub:`1`
      - g\ :sub:`0`
      - b\ :sub:`3`
      - b\ :sub:`2`
      - b\ :sub:`1`
      - b\ :sub:`0`

      - a\ :sub:`3`
      - a\ :sub:`2`
      - a\ :sub:`1`
      - a\ :sub:`0`
      - r\ :sub:`3`
      - r\ :sub:`2`
      - r\ :sub:`1`
      - r\ :sub:`0`
      -
    * .. _V4L2-PIX-FMT-RGB555:

      - ``V4L2_PIX_FMT_RGB555``
      - 'RGBO'

      - g\ :sub:`2`
      - g\ :sub:`1`
      - g\ :sub:`0`
      - b\ :sub:`4`
      - b\ :sub:`3`
      - b\ :sub:`2`
      - b\ :sub:`1`
      - b\ :sub:`0`

      - a
      - r\ :sub:`4`
      - r\ :sub:`3`
      - r\ :sub:`2`
      - r\ :sub:`1`
      - r\ :sub:`0`
      - g\ :sub:`4`
      - g\ :sub:`3`
      -
    * .. _V4L2-PIX-FMT-RGB555X:

      - ``V4L2_PIX_FMT_RGB555X``
      - 'RGBQ'

      - a
      - r\ :sub:`4`
      - r\ :sub:`3`
      - r\ :sub:`2`
      - r\ :sub:`1`
      - r\ :sub:`0`
      - g\ :sub:`4`
      - g\ :sub:`3`

      - g\ :sub:`2`
      - g\ :sub:`1`
      - g\ :sub:`0`
      - b\ :sub:`4`
      - b\ :sub:`3`
      - b\ :sub:`2`
      - b\ :sub:`1`
      - b\ :sub:`0`
      -
    * .. _V4L2-PIX-FMT-BGR32:

      - ``V4L2_PIX_FMT_BGR32``
      - 'BGR4'

      - b\ :sub:`7`
      - b\ :sub:`6`
      - b\ :sub:`5`
      - b\ :sub:`4`
      - b\ :sub:`3`
      - b\ :sub:`2`
      - b\ :sub:`1`
      - b\ :sub:`0`

      - g\ :sub:`7`
      - g\ :sub:`6`
      - g\ :sub:`5`
      - g\ :sub:`4`
      - g\ :sub:`3`
      - g\ :sub:`2`
      - g\ :sub:`1`
      - g\ :sub:`0`

      - r\ :sub:`7`
      - r\ :sub:`6`
      - r\ :sub:`5`
      - r\ :sub:`4`
      - r\ :sub:`3`
      - r\ :sub:`2`
      - r\ :sub:`1`
      - r\ :sub:`0`

      - a\ :sub:`7`
      - a\ :sub:`6`
      - a\ :sub:`5`
      - a\ :sub:`4`
      - a\ :sub:`3`
      - a\ :sub:`2`
      - a\ :sub:`1`
      - a\ :sub:`0`
    * .. _V4L2-PIX-FMT-RGB32:

      - ``V4L2_PIX_FMT_RGB32``
      - 'RGB4'

      - a\ :sub:`7`
      - a\ :sub:`6`
      - a\ :sub:`5`
      - a\ :sub:`4`
      - a\ :sub:`3`
      - a\ :sub:`2`
      - a\ :sub:`1`
      - a\ :sub:`0`

      - r\ :sub:`7`
      - r\ :sub:`6`
      - r\ :sub:`5`
      - r\ :sub:`4`
      - r\ :sub:`3`
      - r\ :sub:`2`
      - r\ :sub:`1`
      - r\ :sub:`0`

      - g\ :sub:`7`
      - g\ :sub:`6`
      - g\ :sub:`5`
      - g\ :sub:`4`
      - g\ :sub:`3`
      - g\ :sub:`2`
      - g\ :sub:`1`
      - g\ :sub:`0`

      - b\ :sub:`7`
      - b\ :sub:`6`
      - b\ :sub:`5`
      - b\ :sub:`4`
      - b\ :sub:`3`
      - b\ :sub:`2`
      - b\ :sub:`1`
      - b\ :sub:`0`

.. raw:: latex

    \endgroup

A test utility to determine which RGB formats a driver actually supports
is available from the LinuxTV v4l-dvb repository. See
`https://linuxtv.org/repo/ <https://linuxtv.org/repo/>`__ for access
instructions.
(有一个测试工具可以确定驱动实际支持哪些 RGB 格式，可从 LinuxTV v4l-dvb 仓库获取。访问方法参见 `https://linuxtv.org/repo/ <https://linuxtv.org/repo/>`__ 。)
