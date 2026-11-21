.. SPDX-License-Identifier: GPL-2.0

.. include:: <isonum.txt>

NPCM video driver(NPCM 视频驱动)
================================

This driver is used to control the Video Capture/Differentiation (VCD) engine
and Encoding Compression Engine (ECE) present on Nuvoton NPCM SoCs. The VCD can
capture a frame from digital video input and compare two frames in memory, and
the ECE can compress the frame data into HEXTILE format.
(该驱动用于控制 Nuvoton NPCM SoC 上的视频采集/辨差 (Video Capture/
Differentiation, VCD) 引擎和编码压缩引擎 (ECE)。VCD 可以从数字视频输入采集一
帧并比较内存中的两帧，ECE 则可以把帧数据压缩为 HEXTILE 格式。)

Driver-specific Controls(驱动专属控制)
--------------------------------------

V4L2_CID_NPCM_CAPTURE_MODE(VCD 采集模式)
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

The VCD engine supports two modes:
(VCD 引擎支持两种模式：)

- COMPLETE mode:
  (COMPLETE 模式：)

  Capture the next complete frame into memory.
  (将下一个完整帧采集到内存中。)

- DIFF mode:
  (DIFF 模式：)

  Compare the incoming frame with the frame stored in memory, and updates the
  differentiated frame in memory.
  (将传入帧与存储在内存中的帧进行比较，并更新内存中的差分帧。)

Application can use ``V4L2_CID_NPCM_CAPTURE_MODE`` control to set the VCD mode
with different control values (enum v4l2_npcm_capture_mode):
(应用可以使用 ``V4L2_CID_NPCM_CAPTURE_MODE`` 控制并配合不同的控制值
(enum v4l2_npcm_capture_mode) 来设置 VCD 模式：)

- ``V4L2_NPCM_CAPTURE_MODE_COMPLETE``: will set VCD to COMPLETE mode.
  (将把 VCD 设置为 COMPLETE 模式。)
- ``V4L2_NPCM_CAPTURE_MODE_DIFF``: will set VCD to DIFF mode.
  (将把 VCD 设置为 DIFF 模式。)

V4L2_CID_NPCM_RECT_COUNT(HEXTILE 矩形计数)
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

If using V4L2_PIX_FMT_HEXTILE format, VCD will capture frame data and then ECE
will compress the data into HEXTILE rectangles and store them in V4L2 video
buffer with the layout defined in Remote Framebuffer Protocol:
::

           (RFC 6143, https://www.rfc-editor.org/rfc/rfc6143.html#section-7.6.1)

           +--------------+--------------+-------------------+
           | No. of bytes | Type [Value] | Description       |
           +--------------+--------------+-------------------+
           | 2            | U16          | x-position        |
           | 2            | U16          | y-position        |
           | 2            | U16          | width             |
           | 2            | U16          | height            |
           | 4            | S32          | encoding-type (5) |
           +--------------+--------------+-------------------+
           |             HEXTILE rectangle data              |
           +-------------------------------------------------+

(如果使用 V4L2_PIX_FMT_HEXTILE 格式，VCD 会先采集帧数据，随后 ECE 把数据压缩
为 HEXTILE 矩形，并按 Remote Framebuffer Protocol (RFC 6143，
https://www.rfc-editor.org/rfc/rfc6143.html#section-7.6.1) 中定义的布局存储
到 V4L2 视频缓冲区中。)

Application can get the video buffer through VIDIOC_DQBUF, and followed by
calling ``V4L2_CID_NPCM_RECT_COUNT`` control to get the number of HEXTILE
rectangles in this buffer.
(应用可以通过 VIDIOC_DQBUF 获取视频缓冲区，然后调用
``V4L2_CID_NPCM_RECT_COUNT`` 控制来取得该缓冲区中 HEXTILE 矩形的数量。)

References(参考)
-------------------------------
include/uapi/linux/npcm-video.h

**Copyright** |copy| 2022 Nuvoton Technologies
