.. SPDX-License-Identifier: GFDL-1.1-no-invariants-or-later
.. c:namespace:: V4L

.. _pixfmt-indexed:

************************
Indexed Format(索引格式)
************************

In this format each pixel is represented by an 8 bit index into a 256
entry ARGB palette. It is intended for
:ref:`Video Output Overlays <osd>` only. There are no ioctls to access
the palette, this must be done with ioctls of the Linux framebuffer API.
(此格式中每个像素由一个 8 位索引表示，指向一个 256 项的 ARGB 调色板。它
仅用于 :ref:`Video Output Overlays <osd>` 。没有访问调色板的 ioctl，必须
使用 Linux framebuffer API 的 ioctl 来完成。)



.. flat-table:: Indexed Image Format
    :header-rows:  2
    :stub-columns: 0

    * - Identifier
      - Code
      -
      - :cspan:`7` Byte 0
    * -
      -
      - Bit
      - 7
      - 6
      - 5
      - 4
      - 3
      - 2
      - 1
      - 0
    * .. _V4L2-PIX-FMT-PAL8:

      - ``V4L2_PIX_FMT_PAL8``
      - 'PAL8'
      -
      - i\ :sub:`7`
      - i\ :sub:`6`
      - i\ :sub:`5`
      - i\ :sub:`4`
      - i\ :sub:`3`
      - i\ :sub:`2`
      - i\ :sub:`1`
      - i\ :sub:`0`
