.. SPDX-License-Identifier: GFDL-1.1-no-invariants-or-later
.. c:namespace:: V4L

.. _v4l2-meta-fmt-uvc:

**************************************************
V4L2_META_FMT_UVC ('UVCH')(UVC 载荷头部元数据格式)
**************************************************

UVC Payload Header Data
(UVC 载荷头部数据。)


Description(描述)
=================

This format describes standard UVC metadata, extracted from UVC packet headers
and provided by the UVC driver through metadata video nodes. That data includes
exact copies of the standard part of UVC Payload Header contents and auxiliary
timing information, required for precise interpretation of timestamps, contained
in those headers. See section "2.4.3.3 Video and Still Image Payload Headers" of
the "UVC 1.5 Class specification" for details.
(此格式描述标准的 UVC 元数据，它从 UVC 包头部中提取，并由 UVC 驱动通过元
数据视频节点提供。该数据包括 UVC 载荷头部内容标准部分的精确副本，以及包含
在这些头部中的辅助计时信息——精确解释时间戳所必需。详情参见 "UVC 1.5 Class
specification" 的 "2.4.3.3 Video and Still Image Payload Headers" 一节。)

Each UVC payload header can be between 2 and 12 bytes large. Buffers can
contain multiple headers, if multiple such headers have been transmitted by the
camera for the respective frame. However, the driver may drop headers when the
buffer is full, when they contain no useful information (e.g. those without the
SCR field or with that field identical to the previous header), or generally to
perform rate limiting when the device sends a large number of headers.
(每个 UVC 载荷头部的大小可以在 2 到 12 字节之间。如果相机为相应帧传输了多个
此类头部，缓冲区可以包含多个头部。不过，在缓冲区已满、头部不含有用信息
(例如没有 SCR 字段，或该字段与前一个头部相同) 时，或者一般在设备发送大量
头部需要限流时，驱动可能会丢弃头部。)

Each individual block contains the following fields:
(每个单独的块包含以下字段：)

.. flat-table:: UVC Metadata Block
    :widths: 1 4
    :header-rows:  1
    :stub-columns: 0

    * - Field(字段)
      - Description(描述)
    * - __u64 ts;
      - system timestamp in host byte order, measured by the driver upon
        reception of the payload(以主机字节序表示的系统时间戳，由驱动在收到
        载荷时测量)
    * - __u16 sof;
      - USB Frame Number in host byte order, also obtained by the driver as
        close as possible to the above timestamp to enable correlation between
        them(以主机字节序表示的 USB 帧号，同样由驱动尽可能在接近上述时间戳的
        时刻获取，以便在两者之间建立关联)
    * - :cspan:`1` *The rest is an exact copy of the UVC payload header:* (其余部分是 UVC 载荷头部的精确副本：)
    * - __u8 length;
      - length of the rest of the block, including this field. Please note that
        regardless of this value, for V4L2_META_FMT_UVC the kernel will never
        copy more than 2-12 bytes.(块的其余部分的长度，包括此字段本身。请注意，无论此值为何，对于
        V4L2_META_FMT_UVC，内核复制的字节永远不会超过 2-12 字节。)
    * - __u8 flags;
      - Flags, indicating presence of other standard UVC fields(标志，指示存在其他标准 UVC 字段)
    * - __u8 buf[];
      - The rest of the header, possibly including UVC PTS and SCR fields(头部的其余部分，可能包含 UVC PTS 和 SCR 字段)
