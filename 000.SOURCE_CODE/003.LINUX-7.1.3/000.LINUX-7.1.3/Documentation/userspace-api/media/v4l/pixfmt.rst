.. SPDX-License-Identifier: GFDL-1.1-no-invariants-or-later
.. c:namespace:: V4L

.. _pixfmt:

#######################
Image Formats(图像格式)
#######################
The V4L2 API was primarily designed for devices exchanging image data
with applications. The struct :c:type:`v4l2_pix_format` and
struct :c:type:`v4l2_pix_format_mplane` structures define the
format and layout of an image in memory. The former is used with the
single-planar API, while the latter is used with the multi-planar
version (see :ref:`planar-apis`). Image formats are negotiated with
the :ref:`VIDIOC_S_FMT <VIDIOC_G_FMT>` ioctl. (The explanations here
focus on video capturing and output, for overlay frame buffer formats
see also :ref:`VIDIOC_G_FBUF <VIDIOC_G_FBUF>`.)
(V4L2 API 最初是为在设备与应用程序之间交换图像数据的场景而设计的。struct
:c:type:`v4l2_pix_format` 与 struct :c:type:`v4l2_pix_format_mplane` 结构定义
图像在内存中的格式与布局。前者用于单平面 API，后者用于多平面版本（见
:ref:`planar-apis`）。图像格式通过 :ref:`VIDIOC_S_FMT <VIDIOC_G_FMT>` ioctl
协商。（这里的说明聚焦于视频采集与输出；overlay 帧缓冲格式另见
:ref:`VIDIOC_G_FBUF <VIDIOC_G_FBUF>`。）)


.. toctree::
    :maxdepth: 1

    pixfmt-v4l2
    pixfmt-v4l2-mplane
    pixfmt-intro
    pixfmt-indexed
    pixfmt-rgb
    pixfmt-bayer
    yuv-formats
    hsv-formats
    depth-formats
    pixfmt-compressed
    sdr-formats
    tch-formats
    meta-formats
    pixfmt-reserved
    colorspaces
    colorspaces-defs
    colorspaces-details
