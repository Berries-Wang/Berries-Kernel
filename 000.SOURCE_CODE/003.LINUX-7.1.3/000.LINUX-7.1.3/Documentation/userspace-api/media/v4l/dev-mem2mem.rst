.. SPDX-License-Identifier: GFDL-1.1-no-invariants-or-later
.. c:namespace:: V4L

.. _mem2mem:

****************************************************
Video Memory-To-Memory Interface(视频内存到内存接口)
****************************************************

A V4L2 memory-to-memory device can compress, decompress, transform, or
otherwise convert video data from one format into another format, in memory.
Such memory-to-memory devices set the ``V4L2_CAP_VIDEO_M2M`` or
``V4L2_CAP_VIDEO_M2M_MPLANE`` capability. Examples of memory-to-memory
devices are codecs, scalers, deinterlacers or format converters (i.e.
converting from YUV to RGB).
(V4L2 内存到内存设备可以在内存中将视频数据从一种格式压缩、解压、变换或以
其他方式转换为另一种格式。这类内存到内存设备会设置 ``V4L2_CAP_VIDEO_M2M``
或 ``V4L2_CAP_VIDEO_M2M_MPLANE`` 能力。内存到内存设备的例子有编解码器、
缩放器、去隔行扫描器或格式转换器（例如从 YUV 转换到 RGB）。)

A memory-to-memory video node acts just like a normal video node, but it
supports both output (sending frames from memory to the hardware)
and capture (receiving the processed frames from the hardware into
memory) stream I/O. An application will have to setup the stream I/O for
both sides and finally call :ref:`VIDIOC_STREAMON <VIDIOC_STREAMON>`
for both capture and output to start the hardware.
(内存到内存视频节点的行为与普通视频节点一样，但它同时支持输出（把帧从内存
发送到硬件）和采集（把硬件处理后的帧接收到内存）两种流 I/O。应用程序必须为
两侧都设置好流 I/O，最后对采集和输出都调用 :ref:`VIDIOC_STREAMON
<VIDIOC_STREAMON>` 以启动硬件。)

Memory-to-memory devices function as a shared resource: you can
open the video node multiple times, each application setting up their
own properties that are local to the file handle, and each can use
it independently from the others. The driver will arbitrate access to
the hardware and reprogram it whenever another file handler gets access.
This is different from the usual video node behavior where the video
properties are global to the device (i.e. changing something through one
file handle is visible through another file handle).
(内存到内存设备作为共享资源工作：可以多次打开视频节点，每个应用程序设置
各自的、仅对文件句柄局部生效的属性，并各自独立地使用它。每当另一个文件
句柄获得访问权时，驱动会对硬件访问进行仲裁并重新对硬件编程。这与通常视频
节点的行为不同——后者的视频属性对设备是全局的（即通过一个文件句柄所做的
更改在另一个文件句柄中也可见）。)

One of the most common memory-to-memory device is the codec. Codecs
are more complicated than most and require additional setup for
their codec parameters. This is done through codec controls.
See :ref:`codec-controls`. More details on how to use codec memory-to-memory
devices are given in the following sections.
(最常见的内存到内存设备之一是编解码器。编解码器比大多数设备复杂，需要为其
编解码器参数做额外设置。这通过编解码器控制完成，见 :ref:`codec-controls`。
关于如何使用编解码器内存到内存设备的更多细节见以下章节。)

.. toctree::
    :maxdepth: 1

    dev-decoder
    dev-encoder
    dev-stateless-decoder
