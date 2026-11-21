.. SPDX-License-Identifier: GFDL-1.1-no-invariants-or-later
.. c:namespace:: V4L

.. _hsv-formats:

*********************
HSV Formats(HSV 格式)
*********************

These formats store the color information of the image
in a geometrical representation. The colors are mapped into a
cylinder, where the angle is the HUE, the height is the VALUE
and the distance to the center is the SATURATION. This is a very
useful format for image segmentation algorithms.
(这些格式以几何表示方式存储图像的颜色信息。颜色被映射到一个圆柱体中：角度是色调 (HUE)，高度是明度 (VALUE)，到中心的距离是饱和度 (SATURATION)。这是一种对图像分割算法非常有用的格式。)


.. toctree::
    :maxdepth: 1

    pixfmt-packed-hsv
