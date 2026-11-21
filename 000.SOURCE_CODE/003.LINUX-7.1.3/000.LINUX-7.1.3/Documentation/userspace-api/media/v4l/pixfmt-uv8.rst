.. SPDX-License-Identifier: GFDL-1.1-no-invariants-or-later
.. c:namespace:: V4L

.. _V4L2-PIX-FMT-UV8:

*******************************************
V4L2_PIX_FMT_UV8 ('UV8')(UV 平面交错格式)
*******************************************


UV plane interleaved
(UV 平面交错。)


Description(描述)
=================

In this format there is no Y plane, Only CbCr plane. ie (UV interleaved)
(此格式中没有 Y 平面，只有 CbCr 平面，即 (UV 交错)。)

**Byte Order.**
Each cell is one byte.
(字节序。每个单元格为一个字节。)




.. flat-table::
    :header-rows:  0
    :stub-columns: 0

    * - start + 0:
      - Cb\ :sub:`00`
      - Cr\ :sub:`00`
      - Cb\ :sub:`01`
      - Cr\ :sub:`01`
    * - start + 4:
      - Cb\ :sub:`10`
      - Cr\ :sub:`10`
      - Cb\ :sub:`11`
      - Cr\ :sub:`11`
    * - start + 8:
      - Cb\ :sub:`20`
      - Cr\ :sub:`20`
      - Cb\ :sub:`21`
      - Cr\ :sub:`21`
    * - start + 12:
      - Cb\ :sub:`30`
      - Cr\ :sub:`30`
      - Cb\ :sub:`31`
      - Cr\ :sub:`31`
