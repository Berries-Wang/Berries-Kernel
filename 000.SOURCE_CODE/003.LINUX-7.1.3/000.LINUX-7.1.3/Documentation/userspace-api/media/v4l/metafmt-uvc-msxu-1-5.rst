.. SPDX-License-Identifier: GFDL-1.1-no-invariants-or-later

.. _v4l2-meta-fmt-uvc-msxu-1-5:

***********************************
V4L2_META_FMT_UVC_MSXU_1_5 ('UVCM')
***********************************

Microsoft(R)'s UVC Payload Metadata.
(Microsoft(R) 的 UVC 负载元数据。)


Description(描述)
=================

V4L2_META_FMT_UVC_MSXU_1_5 buffers follow the metadata buffer layout of
V4L2_META_FMT_UVC with the only difference that it includes all the UVC
metadata in the `buffer[]` field, not just the first 2-12 bytes.
(V4L2_META_FMT_UVC_MSXU_1_5 缓冲区遵循 V4L2_META_FMT_UVC 的元数据缓冲区布局，唯一区别是它在 `buffer[]` 字段中包含全部 UVC 元数据，而不仅仅是最初的 2-12 字节。)

The metadata format follows the specification from Microsoft(R) [1].
(元数据格式遵循 Microsoft(R) 的规范 [1]。)

.. _1:

[1] https://docs.microsoft.com/en-us/windows-hardware/drivers/stream/uvc-extensions-1-5
