.. SPDX-License-Identifier: GFDL-1.1-no-invariants-or-later
.. c:namespace:: V4L

.. _pixfmt-bayer:

**********************************
Raw Bayer Formats(原始 Bayer 格式)
**********************************

Description(描述)
=================

The raw Bayer formats are used by image sensors before much if any processing is
performed on the image. The formats contain green, red and blue components, with
alternating lines of red and green, and blue and green pixels in different
orders. See also `the Wikipedia article on Bayer filter
<https://en.wikipedia.org/wiki/Bayer_filter>`__.
(原始 Bayer 格式由图像传感器在尚未对图像做多少（或未做）处理时使用。这些格
式包含绿、红、蓝三种分量，红绿像素行与蓝绿像素行交替出现，且排列顺序各不相
同。另见 `the Wikipedia article on Bayer filter
<https://en.wikipedia.org/wiki/Bayer_filter>`__（维基百科关于拜耳滤波器的条
目）。)


.. toctree::
    :maxdepth: 1

    pixfmt-rawnn-cru
    pixfmt-srggb8
    pixfmt-srggb8-pisp-comp
    pixfmt-srggb10
    pixfmt-srggb10p
    pixfmt-srggb10alaw8
    pixfmt-srggb10dpcm8
    pixfmt-srggb10-ipu3
    pixfmt-srggb12
    pixfmt-srggb12p
    pixfmt-srggb14
    pixfmt-srggb14p
    pixfmt-srggb16
