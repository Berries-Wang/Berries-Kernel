.. SPDX-License-Identifier: GPL-2.0 OR GFDL-1.1-no-invariants-or-later
.. c:namespace:: V4L

********************************************************************************************************************************************************************************************************************************************************************************************************
V4L2_META_FMT_GENERIC_8 ('MET8'), V4L2_META_FMT_GENERIC_CSI2_10 ('MC1A'), V4L2_META_FMT_GENERIC_CSI2_12 ('MC1C'), V4L2_META_FMT_GENERIC_CSI2_14 ('MC1E'), V4L2_META_FMT_GENERIC_CSI2_16 ('MC1G'), V4L2_META_FMT_GENERIC_CSI2_20 ('MC1K'), V4L2_META_FMT_GENERIC_CSI2_24 ('MC1O')
********************************************************************************************************************************************************************************************************************************************************************************************************


Generic line-based metadata formats
(通用基于行的元数据格式)


Description(描述)
=================

These generic line-based metadata formats define the memory layout of the data
without defining the format or meaning of the metadata itself.
(这些通用基于行的元数据格式只定义数据的内存布局，而不定义元数据本身的格式或含义。)

.. _v4l2-meta-fmt-generic-8:

V4L2_META_FMT_GENERIC_8(8 位通用元数据格式)
-------------------------------------------

The V4L2_META_FMT_GENERIC_8 format is a plain 8-bit metadata format. This format
is used on CSI-2 for 8 bits per :term:`Data Unit`.
(V4L2_META_FMT_GENERIC_8 格式是一种普通的 8 位元数据格式。在 CSI-2 上，该格式用于每 :term:`Data Unit` 8 位的场合。)

Additionally it is used for 16 bits per Data Unit when two bytes of metadata are
packed into one 16-bit Data Unit. Otherwise the 16 bits per pixel dataformat is
:ref:`V4L2_META_FMT_GENERIC_CSI2_16 <v4l2-meta-fmt-generic-csi2-16>`.
(此外，当两个元数据字节打包进一个 16 位 Data Unit 时，它也用于每 Data Unit 16 位的场合。否则，每像素 16 位的数据格式应使用 :ref:`V4L2_META_FMT_GENERIC_CSI2_16 <v4l2-meta-fmt-generic-csi2-16>` 格式。)

**Byte Order Of V4L2_META_FMT_GENERIC_8.**
Each cell is one byte. "M" denotes a byte of metadata.
(**字节序。** 每个单元格为一个字节。"M" 表示一个元数据字节。)

.. tabularcolumns:: |p{2.4cm}|p{1.2cm}|p{1.2cm}|p{1.2cm}|p{1.2cm}|

.. flat-table:: Sample 4x2 Metadata Frame
    :header-rows:  0
    :stub-columns: 0
    :widths: 12 8 8 8 8

    * - start + 0:
      - M\ :sub:`00`
      - M\ :sub:`10`
      - M\ :sub:`20`
      - M\ :sub:`30`
    * - start + 4:
      - M\ :sub:`01`
      - M\ :sub:`11`
      - M\ :sub:`21`
      - M\ :sub:`31`

.. _v4l2-meta-fmt-generic-csi2-10:

V4L2_META_FMT_GENERIC_CSI2_10(10 位 CSI-2 通用元数据格式)
---------------------------------------------------------

V4L2_META_FMT_GENERIC_CSI2_10 contains 8-bit generic metadata packed in 10-bit
Data Units, with one padding byte after every four bytes of metadata. This
format is typically used by CSI-2 receivers with a source that transmits
MEDIA_BUS_FMT_META_10 and the CSI-2 receiver writes the received data to memory
as-is.
(V4L2_META_FMT_GENERIC_CSI2_10 包含打包进 10 位 Data Unit 的 8 位通用元数据，每四个元数据字节之后有一个填充字节。该格式通常由 CSI-2 接收器配合发送 MEDIA_BUS_FMT_META_10 的源使用，CSI-2 接收器把收到的数据原样写入内存。)

The packing of the data follows the MIPI CSI-2 specification and the padding of
the data is defined in the MIPI CCS specification.
(数据的打包遵循 MIPI CSI-2 规范，数据的填充则定义于 MIPI CCS 规范。)

This format is also used in conjunction with 20 bits per :term:`Data Unit`
formats that pack two bytes of metadata into one Data Unit. Otherwise the
20 bits per pixel dataformat is :ref:`V4L2_META_FMT_GENERIC_CSI2_20
<v4l2-meta-fmt-generic-csi2-20>`.
(该格式也用于把两个元数据字节打包进一个 20 位 :term:`Data Unit` 的格式。否则，每像素 20 位的数据格式应使用 :ref:`V4L2_META_FMT_GENERIC_CSI2_20 <v4l2-meta-fmt-generic-csi2-20>` 格式。)

This format is little endian.
(该格式为小端序。)

**Byte Order Of V4L2_META_FMT_GENERIC_CSI2_10.**
Each cell is one byte. "M" denotes a byte of metadata and "x" a byte of padding.
(**字节序。** 每个单元格为一个字节。"M" 表示一个元数据字节，"x" 表示一个填充字节。)

.. tabularcolumns:: |p{2.4cm}|p{1.2cm}|p{1.2cm}|p{1.2cm}|p{1.2cm}|p{1.8cm}|

.. flat-table:: Sample 4x2 Metadata Frame
    :header-rows:  0
    :stub-columns: 0
    :widths: 12 8 8 8 8 8

    * - start + 0:
      - M\ :sub:`00`
      - M\ :sub:`10`
      - M\ :sub:`20`
      - M\ :sub:`30`
      - x
    * - start + 5:
      - M\ :sub:`01`
      - M\ :sub:`11`
      - M\ :sub:`21`
      - M\ :sub:`31`
      - x

.. _v4l2-meta-fmt-generic-csi2-12:

V4L2_META_FMT_GENERIC_CSI2_12(12 位 CSI-2 通用元数据格式)
---------------------------------------------------------

V4L2_META_FMT_GENERIC_CSI2_12 contains 8-bit generic metadata packed in 12-bit
Data Units, with one padding byte after every two bytes of metadata. This format
is typically used by CSI-2 receivers with a source that transmits
MEDIA_BUS_FMT_META_12 and the CSI-2 receiver writes the received data to memory
as-is.
(V4L2_META_FMT_GENERIC_CSI2_12 包含打包进 12 位 Data Unit 的 8 位通用元数据，每两个元数据字节之后有一个填充字节。该格式通常由 CSI-2 接收器配合发送 MEDIA_BUS_FMT_META_12 的源使用，CSI-2 接收器把收到的数据原样写入内存。)

The packing of the data follows the MIPI CSI-2 specification and the padding of
the data is defined in the MIPI CCS specification.
(数据的打包遵循 MIPI CSI-2 规范，数据的填充则定义于 MIPI CCS 规范。)

This format is also used in conjunction with 24 bits per :term:`Data Unit`
formats that pack two bytes of metadata into one Data Unit. Otherwise the
24 bits per pixel dataformat is :ref:`V4L2_META_FMT_GENERIC_CSI2_24
<v4l2-meta-fmt-generic-csi2-24>`.
(该格式也用于把两个元数据字节打包进一个 24 位 :term:`Data Unit` 的格式。否则，每像素 24 位的数据格式应使用 :ref:`V4L2_META_FMT_GENERIC_CSI2_24 <v4l2-meta-fmt-generic-csi2-24>` 格式。)

This format is little endian.
(该格式为小端序。)

**Byte Order Of V4L2_META_FMT_GENERIC_CSI2_12.**
Each cell is one byte. "M" denotes a byte of metadata and "x" a byte of padding.
(**字节序。** 每个单元格为一个字节。"M" 表示一个元数据字节，"x" 表示一个填充字节。)

.. tabularcolumns:: |p{2.4cm}|p{1.2cm}|p{1.2cm}|p{1.8cm}|p{1.2cm}|p{1.2cm}|p{1.8cm}|

.. flat-table:: Sample 4x2 Metadata Frame
    :header-rows:  0
    :stub-columns: 0
    :widths: 12 8 8 8 8 8 8

    * - start + 0:
      - M\ :sub:`00`
      - M\ :sub:`10`
      - x
      - M\ :sub:`20`
      - M\ :sub:`30`
      - x
    * - start + 6:
      - M\ :sub:`01`
      - M\ :sub:`11`
      - x
      - M\ :sub:`21`
      - M\ :sub:`31`
      - x

.. _v4l2-meta-fmt-generic-csi2-14:

V4L2_META_FMT_GENERIC_CSI2_14(14 位 CSI-2 通用元数据格式)
---------------------------------------------------------

V4L2_META_FMT_GENERIC_CSI2_14 contains 8-bit generic metadata packed in 14-bit
Data Units, with three padding bytes after every four bytes of metadata. This
format is typically used by CSI-2 receivers with a source that transmits
MEDIA_BUS_FMT_META_14 and the CSI-2 receiver writes the received data to memory
as-is.
(V4L2_META_FMT_GENERIC_CSI2_14 包含打包进 14 位 Data Unit 的 8 位通用元数据，每四个元数据字节之后有三个填充字节。该格式通常由 CSI-2 接收器配合发送 MEDIA_BUS_FMT_META_14 的源使用，CSI-2 接收器把收到的数据原样写入内存。)

The packing of the data follows the MIPI CSI-2 specification and the padding of
the data is defined in the MIPI CCS specification.
(数据的打包遵循 MIPI CSI-2 规范，数据的填充则定义于 MIPI CCS 规范。)

This format is little endian.
(该格式为小端序。)

**Byte Order Of V4L2_META_FMT_GENERIC_CSI2_14.**
Each cell is one byte. "M" denotes a byte of metadata and "x" a byte of padding.
(**字节序。** 每个单元格为一个字节。"M" 表示一个元数据字节，"x" 表示一个填充字节。)

.. tabularcolumns:: |p{2.4cm}|p{1.2cm}|p{1.2cm}|p{1.2cm}|p{1.2cm}|p{1.8cm}|p{1.8cm}|p{1.8cm}|

.. flat-table:: Sample 4x2 Metadata Frame
    :header-rows:  0
    :stub-columns: 0
    :widths: 12 8 8 8 8 8 8 8

    * - start + 0:
      - M\ :sub:`00`
      - M\ :sub:`10`
      - M\ :sub:`20`
      - M\ :sub:`30`
      - x
      - x
      - x
    * - start + 7:
      - M\ :sub:`01`
      - M\ :sub:`11`
      - M\ :sub:`21`
      - M\ :sub:`31`
      - x
      - x
      - x

.. _v4l2-meta-fmt-generic-csi2-16:

V4L2_META_FMT_GENERIC_CSI2_16(16 位 CSI-2 通用元数据格式)
---------------------------------------------------------

V4L2_META_FMT_GENERIC_CSI2_16 contains 8-bit generic metadata packed in 16-bit
Data Units, with one padding byte after every byte of metadata. This format is
typically used by CSI-2 receivers with a source that transmits
MEDIA_BUS_FMT_META_16 and the CSI-2 receiver writes the received data to memory
as-is.
(V4L2_META_FMT_GENERIC_CSI2_16 包含打包进 16 位 Data Unit 的 8 位通用元数据，每个元数据字节之后有一个填充字节。该格式通常由 CSI-2 接收器配合发送 MEDIA_BUS_FMT_META_16 的源使用，CSI-2 接收器把收到的数据原样写入内存。)

The packing of the data follows the MIPI CSI-2 specification and the padding of
the data is defined in the MIPI CCS specification.
(数据的打包遵循 MIPI CSI-2 规范，数据的填充则定义于 MIPI CCS 规范。)

Some devices support more efficient packing of metadata in conjunction with
16-bit image data. In that case the dataformat is
:ref:`V4L2_META_FMT_GENERIC_8 <v4l2-meta-fmt-generic-8>`.
(一些设备在配合 16 位图像数据时支持对元数据更高效的打包。这种情况下应使用 :ref:`V4L2_META_FMT_GENERIC_8 <v4l2-meta-fmt-generic-8>` 格式。)

This format is little endian.
(该格式为小端序。)

**Byte Order Of V4L2_META_FMT_GENERIC_CSI2_16.**
Each cell is one byte. "M" denotes a byte of metadata and "x" a byte of padding.
(**字节序。** 每个单元格为一个字节。"M" 表示一个元数据字节，"x" 表示一个填充字节。)

.. tabularcolumns:: |p{2.4cm}|p{1.2cm}|p{.8cm}|p{1.2cm}|p{.8cm}|p{1.2cm}|p{.8cm}|p{1.2cm}|p{.8cm}|

.. flat-table:: Sample 4x2 Metadata Frame
    :header-rows:  0
    :stub-columns: 0
    :widths: 12 8 8 8 8 8 8 8 8

    * - start + 0:
      - M\ :sub:`00`
      - x
      - M\ :sub:`10`
      - x
      - M\ :sub:`20`
      - x
      - M\ :sub:`30`
      - x
    * - start + 8:
      - M\ :sub:`01`
      - x
      - M\ :sub:`11`
      - x
      - M\ :sub:`21`
      - x
      - M\ :sub:`31`
      - x

.. _v4l2-meta-fmt-generic-csi2-20:

V4L2_META_FMT_GENERIC_CSI2_20(20 位 CSI-2 通用元数据格式)
---------------------------------------------------------

V4L2_META_FMT_GENERIC_CSI2_20 contains 8-bit generic metadata packed in 20-bit
Data Units, with alternating one or two padding bytes after every byte of
metadata. This format is typically used by CSI-2 receivers with a source that
transmits MEDIA_BUS_FMT_META_20 and the CSI-2 receiver writes the received data
to memory as-is.
(V4L2_META_FMT_GENERIC_CSI2_20 包含打包进 20 位 Data Unit 的 8 位通用元数据，每个元数据字节之后交替跟有一或两个填充字节。该格式通常由 CSI-2 接收器配合发送 MEDIA_BUS_FMT_META_20 的源使用，CSI-2 接收器把收到的数据原样写入内存。)

The packing of the data follows the MIPI CSI-2 specification and the padding of
the data is defined in the MIPI CCS specification.
(数据的打包遵循 MIPI CSI-2 规范，数据的填充则定义于 MIPI CCS 规范。)

Some devices support more efficient packing of metadata in conjunction with
16-bit image data. In that case the dataformat is
:ref:`V4L2_META_FMT_GENERIC_CSI2_10 <v4l2-meta-fmt-generic-csi2-10>`.
(一些设备在配合 16 位图像数据时支持对元数据更高效的打包。这种情况下应使用 :ref:`V4L2_META_FMT_GENERIC_CSI2_10 <v4l2-meta-fmt-generic-csi2-10>` 格式。)

This format is little endian.
(该格式为小端序。)

**Byte Order Of V4L2_META_FMT_GENERIC_CSI2_20.**
Each cell is one byte. "M" denotes a byte of metadata and "x" a byte of padding.
(**字节序。** 每个单元格为一个字节。"M" 表示一个元数据字节，"x" 表示一个填充字节。)

.. tabularcolumns:: |p{2.4cm}|p{1.2cm}|p{1.2cm}|p{1.2cm}|p{1.2cm}|p{1.8cm}|p{1.2cm}|p{1.2cm}|p{1.2cm}|p{1.2cm}|p{1.8cm}

.. flat-table:: Sample 4x2 Metadata Frame
    :header-rows:  0
    :stub-columns: 0
    :widths: 12 8 8 8 8 8 8 8 8 8 8

    * - start + 0:
      - M\ :sub:`00`
      - x
      - M\ :sub:`10`
      - x
      - x
      - M\ :sub:`20`
      - x
      - M\ :sub:`30`
      - x
      - x
    * - start + 10:
      - M\ :sub:`01`
      - x
      - M\ :sub:`11`
      - x
      - x
      - M\ :sub:`21`
      - x
      - M\ :sub:`31`
      - x
      - x

.. _v4l2-meta-fmt-generic-csi2-24:

V4L2_META_FMT_GENERIC_CSI2_24(24 位 CSI-2 通用元数据格式)
---------------------------------------------------------

V4L2_META_FMT_GENERIC_CSI2_24 contains 8-bit generic metadata packed in 24-bit
Data Units, with two padding bytes after every byte of metadata. This format is
typically used by CSI-2 receivers with a source that transmits
MEDIA_BUS_FMT_META_24 and the CSI-2 receiver writes the received data to memory
as-is.
(V4L2_META_FMT_GENERIC_CSI2_24 包含打包进 24 位 Data Unit 的 8 位通用元数据，每个元数据字节之后有两个填充字节。该格式通常由 CSI-2 接收器配合发送 MEDIA_BUS_FMT_META_24 的源使用，CSI-2 接收器把收到的数据原样写入内存。)

The packing of the data follows the MIPI CSI-2 specification and the padding of
the data is defined in the MIPI CCS specification.
(数据的打包遵循 MIPI CSI-2 规范，数据的填充则定义于 MIPI CCS 规范。)

Some devices support more efficient packing of metadata in conjunction with
16-bit image data. In that case the dataformat is
:ref:`V4L2_META_FMT_GENERIC_CSI2_12 <v4l2-meta-fmt-generic-csi2-12>`.
(一些设备在配合 16 位图像数据时支持对元数据更高效的打包。这种情况下应使用 :ref:`V4L2_META_FMT_GENERIC_CSI2_12 <v4l2-meta-fmt-generic-csi2-12>` 格式。)

This format is little endian.
(该格式为小端序。)

**Byte Order Of V4L2_META_FMT_GENERIC_CSI2_24.**
Each cell is one byte. "M" denotes a byte of metadata and "x" a byte of padding.
(**字节序。** 每个单元格为一个字节。"M" 表示一个元数据字节，"x" 表示一个填充字节。)

.. tabularcolumns:: |p{2.4cm}|p{1.2cm}|p{.8cm}|p{.8cm}|p{1.2cm}|p{.8cm}|p{.8cm}|p{1.2cm}|p{.8cm}|p{.8cm}|p{1.2cm}|p{.8cm}|p{.8cm}|

.. flat-table:: Sample 4x2 Metadata Frame
    :header-rows:  0
    :stub-columns: 0
    :widths: 12 8 8 8 8 8 8 8 8 8 8 8 8

    * - start + 0:
      - M\ :sub:`00`
      - x
      - x
      - M\ :sub:`10`
      - x
      - x
      - M\ :sub:`20`
      - x
      - x
      - M\ :sub:`30`
      - x
      - x
    * - start + 12:
      - M\ :sub:`01`
      - x
      - x
      - M\ :sub:`11`
      - x
      - x
      - M\ :sub:`21`
      - x
      - x
      - M\ :sub:`31`
      - x
      - x
