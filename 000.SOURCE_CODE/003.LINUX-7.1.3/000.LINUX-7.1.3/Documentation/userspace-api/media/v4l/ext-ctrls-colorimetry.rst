.. SPDX-License-Identifier: GFDL-1.1-no-invariants-or-later
.. c:namespace:: V4L

.. _colorimetry-controls:

*********************************************
Colorimetry Control Reference(色度学控制参考)
*********************************************

The Colorimetry class includes controls for High Dynamic Range
imaging for representing colors in digital images and video. The
controls should be used for video and image encoding and decoding
as well as in HDMI receivers and transmitters.
(Colorimetry 类包含用于在数字图像和视频中呈现颜色的高动态范围(HDR)
成像控制项。这些控制应用于视频和图像的编码与解码，也应用于 HDMI
接收器和发送器。)

Colorimetry Control IDs(色度学控制 ID)
--------------------------------------

.. _colorimetry-control-id:

``V4L2_CID_COLORIMETRY_CLASS (class)``
    The Colorimetry class descriptor. Calling
    :ref:`VIDIOC_QUERYCTRL` for this control will
    return a description of this control class.
    (Colorimetry 类描述符。对该控制调用 :ref:`VIDIOC_QUERYCTRL`
    将返回此控制类的描述。)

``V4L2_CID_COLORIMETRY_HDR10_CLL_INFO (struct)``
    The Content Light Level defines upper bounds for the nominal target
    brightness light level of the pictures.
    (Content Light Level(内容光级)定义图片标称目标亮度光级的上限。)

.. c:type:: v4l2_ctrl_hdr10_cll_info

.. cssclass:: longtable

.. flat-table:: struct v4l2_ctrl_hdr10_cll_info
    :header-rows:  0
    :stub-columns: 0
    :widths:       1 1 2

    * - __u16
      - ``max_content_light_level``
      - The upper bound for the maximum light level among all individual
        samples for the pictures of a video sequence, cd/m\ :sup:`2`.
        When equal to 0 no such upper bound is present.
        (视频序列中所有图片的各个样本中最大光级的上限(单位
        cd/m\ :sup:`2`)；等于 0 时表示不存在这样的上限。)
    * - __u16
      - ``max_pic_average_light_level``
      - The upper bound for the maximum average light level among the
        samples for any individual picture of a video sequence,
        cd/m\ :sup:`2`. When equal to 0 no such upper bound is present.
        (视频序列中任何单个图片的样本间最大平均光级的上限(单位
        cd/m\ :sup:`2`)；等于 0 时表示不存在这样的上限。)

``V4L2_CID_COLORIMETRY_HDR10_MASTERING_DISPLAY (struct)``
    The mastering display defines the color volume (the color primaries,
    white point and luminance range) of a display considered to be the
    mastering display for the current video content.
    (mastering display 定义了被视为当前视频内容的母版显示器(display)
    的色彩容量(颜色基色、白点和亮度范围)。)

.. c:type:: v4l2_ctrl_hdr10_mastering_display

.. cssclass:: longtable

.. flat-table:: struct v4l2_ctrl_hdr10_mastering_display
    :header-rows:  0
    :stub-columns: 0
    :widths:       1 1 2

    * - __u16
      - ``display_primaries_x[3]``
      - Specifies the normalized x chromaticity coordinate of the color
        primary component c of the mastering display in increments of 0.00002.
        For describing the mastering display that uses Red, Green and Blue
        color primaries, index value c equal to 0 corresponds to the Green
        primary, c equal to 1 corresponds to Blue primary and c equal to 2
        corresponds to the Red color primary.
        (以 0.00002 为增量指定母版显示器颜色基色分量 c 的归一化 x 色
        度坐标。对于使用红、绿、蓝颜色基色的母版显示器，索引值 c 等于
        0 对应绿色基色，c 等于 1 对应蓝色基色，c 等于 2 对应红色基色。)
    * - __u16
      - ``display_primaries_y[3]``
      - Specifies the normalized y chromaticity coordinate of the color
        primary component c of the mastering display in increments of 0.00002.
        For describing the mastering display that uses Red, Green and Blue
        color primaries, index value c equal to 0 corresponds to the Green
        primary, c equal to 1 corresponds to Blue primary and c equal to 2
        corresponds to Red color primary.
        (以 0.00002 为增量指定母版显示器颜色基色分量 c 的归一化 y 色
        度坐标。对于使用红、绿、蓝颜色基色的母版显示器，索引值 c 等于
        0 对应绿色基色，c 等于 1 对应蓝色基色，c 等于 2 对应红色基色。)
    * - __u16
      - ``white_point_x``
      - Specifies the normalized x chromaticity coordinate of the white
        point of the mastering display in increments of 0.00002.
        (以 0.00002 为增量指定母版显示器白点的归一化 x 色度坐标。)
    * - __u16
      - ``white_point_y``
      - Specifies the normalized y chromaticity coordinate of the white
        point of the mastering display in increments of 0.00002.
        (以 0.00002 为增量指定母版显示器白点的归一化 y 色度坐标。)
    * - __u32
      - ``max_luminance``
      - Specifies the nominal maximum display luminance of the mastering
        display in units of 0.0001 cd/m\ :sup:`2`.
        (以 0.0001 cd/m\ :sup:`2` 为单位指定母版显示器的标称最大显示
        亮度。)
    * - __u32
      - ``min_luminance``
      - specifies the nominal minimum display luminance of the mastering
        display in units of 0.0001 cd/m\ :sup:`2`.
        (以 0.0001 cd/m\ :sup:`2` 为单位指定母版显示器的标称最小显示
        亮度。)
