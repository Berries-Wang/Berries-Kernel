.. SPDX-License-Identifier: GPL-2.0 OR GFDL-1.1-no-invariants-or-later

==================
Glossary(术语表)
==================

.. note::

   The goal of this section is to standardize the terms used within the media
   userspace API documentation. This is Work In Progress.
   (本节的目标是统一媒体用户空间 API 文档中使用的术语。此项工作仍在进行中。)

.. Please keep the glossary entries in alphabetical order

.. glossary::

    Bridge Driver
	A :term:`Device Driver` that implements the main logic to talk with
	media hardware.
	(一种 :term:`Device Driver`，实现与媒体硬件通信的主要逻辑。)

    CEC API
	**Consumer Electronics Control API**
	(**Consumer Electronics Control API**，消费电子控制 API)

	An API designed to receive and transmit data via an HDMI
	CEC interface.
	(一种旨在通过 HDMI CEC 接口接收和发送数据的 API。)

	See :ref:`cec`.
	(参见 :ref:`cec` 。)

    Data Unit

	Unit of data transported by a bus. On parallel buses, the data unit
	consists of one or more related samples while on serial buses the data
	unit is logical. If the data unit is image data, it may also be called a
	pixel.
	(由总线传输的数据单位。在并行总线上，数据单位由一个或多个相关的采样组成；而在串行总线上，数据单位是逻辑性的。如果数据单位是图像数据，也可以称为像素。)

    Device Driver
	Part of the Linux Kernel that implements support for a hardware
	component.
	(Linux 内核中为某个硬件组件实现支持的部分。)

    Device Node
	A character device node in the file system used to control and
	transfer data in and out of a Kernel driver.
	(文件系统中的字符设备节点，用于控制内核驱动以及向其传入和传出数据。)

    Digital TV API
	**Previously known as DVB API**
	(**Previously known as DVB API**，此前称为 DVB API)

	An API designed to control a subset of the :term:`Media Hardware`
	that implements	digital TV (e. g. DVB, ATSC, ISDB, etc).
	(一种旨在控制 :term:`Media Hardware` 中实现数字电视(如 DVB、ATSC、ISDB 等)那一子集的 API。)

	See :ref:`dvbapi`.
	(参见 :ref:`dvbapi` 。)

    DSP
        **Digital Signal Processor**
        (**Digital Signal Processor**，数字信号处理器)

	A specialized :term:`Microprocessor`, with its architecture
	optimized for the operational needs of digital signal processing.
	(一种专用 :term:`Microprocessor`，其架构针对数字信号处理的运行需求做了优化。)

    FPGA
	**Field-programmable Gate Array**
	(**Field-programmable Gate Array**，现场可编程门阵列)

	An :term:`IC` circuit designed to be configured by a customer or
	a designer after manufacturing.
	(一种设计为由客户或设计者在制造之后进行配置的 :term:`IC` 电路。)

	See https://en.wikipedia.org/wiki/Field-programmable_gate_array.
	(参见 https://en.wikipedia.org/wiki/Field-programmable_gate_array。)

    Hardware Component
	A subset of the :term:`Media Hardware`. For example an :term:`I²C` or
	:term:`SPI` device, or an :term:`IP Block` inside an
	:term:`SoC` or :term:`FPGA`.
	( :term:`Media Hardware` 的一个子集。例如 :term:`I²C` 或 :term:`SPI` 设备，或者 :term:`SoC` 或 :term:`FPGA` 内部的 :term:`IP Block`。)

    Hardware Peripheral
	A group of :term:`hardware components <Hardware Component>` that
	together make a larger user-facing functional peripheral. For
	instance, the :term:`SoC` :term:`ISP` :term:`IP Block`
	and the external camera sensors together make a camera hardware
	peripheral.
	(共同构成一个更大的面向用户的功能性外设的一组 :term:`hardware components <Hardware Component>`。例如， :term:`SoC` 的 :term:`ISP` :term:`IP Block` 与外部摄像头传感器共同构成一个摄像头硬件外设。)

	Also known as :term:`Peripheral`.
	(也称为 :term:`Peripheral`。)

    I²C
	**Inter-Integrated Circuit**
	(**Inter-Integrated Circuit**，内部集成电路总线)

	A  multi-master, multi-slave, packet switched, single-ended,
	serial computer bus used to control some hardware components
	like sub-device hardware components.
	(一种多主多从、分组交换、单端的串行计算机总线，用于控制某些硬件组件，例如子设备硬件组件。)

	See http://www.nxp.com/docs/en/user-guide/UM10204.pdf.
	(参见 http://www.nxp.com/docs/en/user-guide/UM10204.pdf。)

    IC
	**Integrated circuit**
	(**Integrated circuit**，集成电路)

	A set of electronic circuits on one small flat piece of
	semiconductor material, normally silicon.
	(制作在一小片平坦半导体材料(通常是硅)上的一组电子电路。)

	Also known as chip.
	(也称为芯片(chip)。)

    IP Block
	**Intellectual property core**
	(**Intellectual property core**，知识产权核)

	In electronic design a semiconductor intellectual property core,
	is a reusable unit of logic, cell, or integrated circuit layout
	design that is the intellectual property of one party.
	IP Blocks may be licensed to another party or can be owned
	and used by a single party alone.
	(在电子设计中，半导体知识产权核是可复用的逻辑、单元或集成电路版图设计单元，是某一方的知识产权。IP Block 可以授权给另一方，也可以由单一一方单独拥有和使用。)

	See https://en.wikipedia.org/wiki/Semiconductor_intellectual_property_core).
	(参见 https://en.wikipedia.org/wiki/Semiconductor_intellectual_property_core)。)

    ISP
	**Image Signal Processor**
	(**Image Signal Processor**，图像信号处理器)

	A specialized processor that implements a set of algorithms for
	processing image data. ISPs may implement algorithms for lens
	shading correction, demosaicing, scaling and pixel format conversion
	as well as produce statistics for the use of the control
	algorithms (e.g. automatic exposure, white balance and focus).
	(一种实现一组图像数据处理算法的专用处理器。ISP 可以实现镜头阴影校正、去马赛克(demosaicing)、缩放和像素格式转换等算法，并为控制算法(例如自动曝光、白平衡和自动对焦)生成统计信息。)

    Media API
	A set of userspace APIs used to control the media hardware. It is
	composed by:
	(一组用于控制媒体硬件的用户空间 API。它由以下部分组成：)

	  - :term:`CEC API`;
	  - :term:`Digital TV API`;
	  - :term:`MC API`;
	  - :term:`RC API`; and
	  - :term:`V4L2 API`.

	See Documentation/userspace-api/media/index.rst.
	(参见 Documentation/userspace-api/media/index.rst。)

    MC API
	**Media Controller API**
	(**Media Controller API**，媒体控制器 API)

	An API designed to expose and control the relationships between
	multimedia devices and sub-devices.
	(一种旨在暴露并控制多媒体设备与子设备之间关系的 API。)

	See :ref:`media_controller`.
	(参见 :ref:`media_controller` 。)

    MC-centric
	:term:`V4L2 Hardware` device driver that requires :term:`MC API`.
	(需要 :term:`MC API` 的 :term:`V4L2 Hardware` 设备驱动。)

	Such drivers have ``V4L2_CAP_IO_MC`` device_caps field set
	(see :ref:`VIDIOC_QUERYCAP`).
	(这类驱动设置了 ``V4L2_CAP_IO_MC`` device_caps 字段(参见 :ref:`VIDIOC_QUERYCAP`)。)

	See :ref:`v4l2_hardware_control` for more details.
	(更多细节参见 :ref:`v4l2_hardware_control`。)

    Media Hardware
	Subset of the hardware that is supported by the Linux Media API.
	(Linux Media API 所支持硬件的子集。)

	This includes audio and video capture and playback hardware,
	digital and analog TV, camera sensors, ISPs, remote controllers,
	codecs, HDMI Consumer Electronics Control, HDMI capture, etc.
	(这包括音频和视频的采集与播放硬件、数字和模拟电视、摄像头传感器、ISP、遥控器、编解码器、HDMI 消费电子控制、HDMI 采集等。)

    Microprocessor
	Electronic circuitry that carries out the instructions of a
	computer program by performing the basic arithmetic, logical,
	control and input/output (I/O) operations specified by the
	instructions on a single integrated circuit.
	(通过在单个集成电路上执行指令所规定的基本算术、逻辑、控制与输入/输出(I/O)操作，来执行计算机程序指令的电子电路。)

    Peripheral
	The same as :term:`Hardware Peripheral`.
	(与 :term:`Hardware Peripheral` 相同。)

    RC API
	**Remote Controller API**
	(**Remote Controller API**，遥控器 API)

	An API designed to receive and transmit data from remote
	controllers.
	(一种旨在接收和发送来自遥控器的数据的 API。)

	See :ref:`remote_controllers`.
	(参见 :ref:`remote_controllers` 。)

    SMBus
	A subset of I²C, which defines a stricter usage of the bus.
	(I²C 的一个子集，它对总线的使用定义了更严格的规则。)

    SPI
	**Serial Peripheral Interface Bus**
	(**Serial Peripheral Interface Bus**，串行外设接口总线)

	Synchronous serial communication interface specification used for
	short distance communication, primarily in embedded systems.
	(用于短距离通信(主要在嵌入式系统中)的同步串行通信接口规范。)

    SoC
	**System on a Chip**
	(**System on a Chip**，片上系统)

	An integrated circuit that integrates all components of a computer
	or other electronic systems.
	(将计算机或其他电子系统的所有组件集成在一起的一种集成电路。)

    Stream
	A distinct flow of data (image data or metadata) from an initial source
	to a final sink. The initial source may be e.g. an image sensor and the
	final sink e.g. a memory buffer.
	(从初始源到最终汇的一种独立的数据流(图像数据或元数据)。初始源例如图像传感器，最终汇例如内存缓冲区。)

    V4L2 API
	**V4L2 userspace API**
	(**V4L2 userspace API**，V4L2 用户空间 API)

	The userspace API defined in :ref:`v4l2spec`, which is used to
	control a V4L2 hardware.
	(在 :ref:`v4l2spec` 中定义的用户空间 API，用于控制 V4L2 硬件。)

    V4L2 Device Node
	A :term:`Device Node` that is associated to a V4L driver.
	(与某个 V4L 驱动相关联的 :term:`Device Node`。)

	The V4L2 device node naming is specified at :ref:`v4l2_device_naming`.
	(V4L2 设备节点的命名规定见 :ref:`v4l2_device_naming`。)

    V4L2 Hardware
	Part of the media hardware which is supported by the :term:`V4L2 API`.
	(媒体硬件中受 :term:`V4L2 API` 支持的部分。)

    V4L2 Sub-device
	V4L2 hardware components that aren't controlled by a
	:term:`Bridge Driver`. See :ref:`subdev`.
	(不受 :term:`Bridge Driver` 控制的 V4L2 硬件组件。参见 :ref:`subdev`。)

    Video-node-centric
	V4L2 device driver that doesn't require a media controller to be used.
	(不需要使用媒体控制器的 V4L2 设备驱动。)

	Such drivers have the ``V4L2_CAP_IO_MC`` device_caps field unset
	(see :ref:`VIDIOC_QUERYCAP`).
	(这类驱动未设置 ``V4L2_CAP_IO_MC`` device_caps 字段(参见 :ref:`VIDIOC_QUERYCAP`)。)

    V4L2 Sub-device API
	Part of the :term:`V4L2 API` which control
	:term:`V4L2 sub-devices <V4L2 Sub-device>`, like sensors,
	HDMI receivers, scalers, deinterlacers.
	( :term:`V4L2 API` 中用于控制 :term:`V4L2 sub-devices <V4L2 Sub-device>`(如传感器、HDMI 接收器、缩放器、去隔行扫描器)的部分。)

	See :ref:`v4l2_hardware_control` for more details.
	(更多细节参见 :ref:`v4l2_hardware_control`。)
