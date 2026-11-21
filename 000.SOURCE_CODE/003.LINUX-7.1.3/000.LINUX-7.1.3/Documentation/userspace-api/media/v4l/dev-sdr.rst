.. SPDX-License-Identifier: GFDL-1.1-no-invariants-or-later
.. c:namespace:: V4L

.. _sdr:

****************************************************************
Software Defined Radio Interface (SDR)(软件定义无线电接口 (SDR))
****************************************************************

SDR is an abbreviation of Software Defined Radio, the radio device which
uses application software for modulation or demodulation. This interface
is intended for controlling and data streaming of such devices.
(SDR 是 Software Defined Radio(软件定义无线电)的缩写，指用应用软件
进行调制或解调的无线电设备。该接口用于此类设备的控制与数据流传输。)

SDR devices are accessed through character device special files named
``/dev/swradio0`` to ``/dev/swradio255`` with major number 81 and
dynamically allocated minor numbers 0 to 255.
(SDR 设备通过名为 ``/dev/swradio0`` 到 ``/dev/swradio255`` 的字符设备
特殊文件访问，主设备号为 81，次设备号动态分配，范围为 0 到 255。)


Querying Capabilities(查询能力)
===============================

Devices supporting the SDR receiver interface set the
``V4L2_CAP_SDR_CAPTURE`` and ``V4L2_CAP_TUNER`` flag in the
``capabilities`` field of struct
:c:type:`v4l2_capability` returned by the
:ref:`VIDIOC_QUERYCAP` ioctl. That flag means the
device has an Analog to Digital Converter (ADC), which is a mandatory
element for the SDR receiver.
(支持 SDR 接收器接口的设备会在 :ref:`VIDIOC_QUERYCAP` ioctl 返回的
struct :c:type:`v4l2_capability` 的 ``capabilities`` 字段中设置
``V4L2_CAP_SDR_CAPTURE`` 和 ``V4L2_CAP_TUNER`` 标志。该标志表示设备
带有模数转换器(ADC)，它是 SDR 接收器的必备部件。)

Devices supporting the SDR transmitter interface set the
``V4L2_CAP_SDR_OUTPUT`` and ``V4L2_CAP_MODULATOR`` flag in the
``capabilities`` field of struct
:c:type:`v4l2_capability` returned by the
:ref:`VIDIOC_QUERYCAP` ioctl. That flag means the
device has an Digital to Analog Converter (DAC), which is a mandatory
element for the SDR transmitter.
(支持 SDR 发送器接口的设备会在 :ref:`VIDIOC_QUERYCAP` ioctl 返回的
struct :c:type:`v4l2_capability` 的 ``capabilities`` 字段中设置
``V4L2_CAP_SDR_OUTPUT`` 和 ``V4L2_CAP_MODULATOR`` 标志。该标志表示
设备带有数模转换器(DAC)，它是 SDR 发送器的必备部件。)

At least one of the read/write or streaming I/O methods
must be supported.
(至少必须支持读/写或流式 I/O 方法中的一种。)


Supplemental Functions(辅助功能)
================================

SDR devices can support :ref:`controls <control>`, and must support
the :ref:`tuner` ioctls. Tuner ioctls are used for setting the
ADC/DAC sampling rate (sampling frequency) and the possible radio
frequency (RF).
(SDR 设备可以支持 :ref:`controls <control>`，并且必须支持
:ref:`tuner` ioctl。调谐器 ioctl 用于设置 ADC/DAC 采样率(采样频率)
和可能的射频(RF)。)

The ``V4L2_TUNER_SDR`` tuner type is used for setting SDR device ADC/DAC
frequency, and the ``V4L2_TUNER_RF`` tuner type is used for setting
radio frequency. The tuner index of the RF tuner (if any) must always
follow the SDR tuner index. Normally the SDR tuner is #0 and the RF
tuner is #1.
(``V4L2_TUNER_SDR`` 调谐器类型用于设置 SDR 设备的 ADC/DAC 频率，
``V4L2_TUNER_RF`` 调谐器类型用于设置射频。RF 调谐器(如果存在)的
调谐器索引必须始终紧跟 SDR 调谐器索引。通常 SDR 调谐器是 #0，RF
调谐器是 #1。)

The :ref:`VIDIOC_S_HW_FREQ_SEEK` ioctl is
not supported.
(不支持 :ref:`VIDIOC_S_HW_FREQ_SEEK` ioctl。)


Data Format Negotiation(数据格式协商)
=====================================

The SDR device uses the :ref:`format` ioctls to select the
capture and output format. Both the sampling resolution and the data
streaming format are bound to that selectable format. In addition to the
basic :ref:`format` ioctls, the
:ref:`VIDIOC_ENUM_FMT` ioctl must be supported as
well.
(SDR 设备使用 :ref:`format` ioctl 来选择捕获和输出格式。采样分辨率
和数据流格式都绑定到该可选格式。除了基本的 :ref:`format` ioctl 之外，
还必须支持 :ref:`VIDIOC_ENUM_FMT` ioctl。)

To use the :ref:`format` ioctls applications set the ``type``
field of a struct :c:type:`v4l2_format` to
``V4L2_BUF_TYPE_SDR_CAPTURE`` or ``V4L2_BUF_TYPE_SDR_OUTPUT`` and use
the struct :c:type:`v4l2_sdr_format` ``sdr`` member
of the ``fmt`` union as needed per the desired operation. Currently
there are two fields, ``pixelformat`` and ``buffersize``, of
struct :c:type:`v4l2_sdr_format` which are used.
Content of the ``pixelformat`` is V4L2 FourCC code of the data format.
The ``buffersize`` field is maximum buffer size in bytes required for
data transfer, set by the driver in order to inform application.
(要使用 :ref:`format` ioctl，应用程序把 struct :c:type:`v4l2_format`
的 ``type`` 字段设置为 ``V4L2_BUF_TYPE_SDR_CAPTURE`` 或
``V4L2_BUF_TYPE_SDR_OUTPUT``，并根据所需操作按需使用 ``fmt`` 联合体中
的 struct :c:type:`v4l2_sdr_format` ``sdr`` 成员。目前 struct
:c:type:`v4l2_sdr_format` 中使用的是两个字段：``pixelformat`` 和
``buffersize``。``pixelformat`` 的内容是数据格式的 V4L2 FourCC 码。
``buffersize`` 字段是数据传输所需的最大缓冲区大小(以字节为单位)，
由驱动程序设置以告知应用程序。)


.. c:type:: v4l2_sdr_format

.. tabularcolumns:: |p{4.4cm}|p{4.4cm}|p{8.5cm}|

.. flat-table:: struct v4l2_sdr_format
    :header-rows:  0
    :stub-columns: 0
    :widths:       1 1 2

    * - __u32
      - ``pixelformat``
      - The data format or type of compression, set by the application.
	This is a little endian
	:ref:`four character code <v4l2-fourcc>`. V4L2 defines SDR
	formats in :ref:`sdr-formats`.
        (数据格式或压缩类型，由应用程序设置。这是一个小端序的
        :ref:`four character code <v4l2-fourcc>`。V4L2 在
        :ref:`sdr-formats` 中定义 SDR 格式。)
    * - __u32
      - ``buffersize``
      - Maximum size in bytes required for data. Value is set by the
	driver.
        (数据所需的最大字节大小。该值由驱动程序设置。)
    * - __u8
      - ``reserved[24]``
      - This array is reserved for future extensions. Drivers and
	applications must set it to zero.
        (此数组保留用于未来的扩展。驱动程序和应用程序必须将其置零。)


An SDR device may support :ref:`read/write <rw>` and/or streaming
(:ref:`memory mapping <mmap>` or :ref:`user pointer <userp>`) I/O.
(SDR 设备可以支持 :ref:`read/write <rw>` 和/或流式
(:ref:`memory mapping <mmap>` 或 :ref:`user pointer <userp>`)I/O。)
