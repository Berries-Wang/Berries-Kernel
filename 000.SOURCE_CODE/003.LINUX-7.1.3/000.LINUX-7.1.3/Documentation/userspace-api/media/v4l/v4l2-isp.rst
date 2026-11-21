.. SPDX-License-Identifier: GFDL-1.1-no-invariants-or-later

.. _v4l2-isp:

********************************************
Generic V4L2 ISP formats(通用 V4L2 ISP 格式)
********************************************

Generic ISP formats are metadata formats that define a mechanism to pass ISP
parameters and statistics between userspace and drivers in V4L2 buffers. They
are designed to allow extending them in a backward-compatible way.
(通用 ISP 格式是一类元数据格式，它们定义了在 V4L2 缓冲区中于用户空间
与驱动程序之间传递 ISP 参数与统计信息的机制。它们被设计为可以以向后
兼容的方式扩展。)

ISP parameters(ISP 参数)
========================

The generic ISP configuration parameters format is realized by a defining a
single C structure that contains a header, followed by a binary buffer where
userspace programs a variable number of ISP configuration data block, one for
each supported ISP feature.
(通用 ISP 配置参数格式通过定义一个单一的 C 结构来实现：该结构包含一
个头，后跟一个二进制缓冲区，用户空间在其中编写数量可变的 ISP 配置数
据块，每个受支持的 ISP 特性对应一个块。)

The :c:type:`v4l2_isp_params_buffer` structure defines the buffer header which
is followed by a binary buffer of ISP configuration data. Userspace shall
correctly populate the buffer header with the generic parameters format version
and with the size (in bytes) of the binary data buffer where it will store the
ISP blocks configuration.
( :c:type:`v4l2_isp_params_buffer` 结构定义了缓冲区头，其后是 ISP 配
置数据的二进制缓冲区。用户空间应正确填充缓冲区头：写入通用参数格式
版本，以及将存储 ISP 块配置的二进制数据缓冲区的大小 (以字节为单位)。)

Each *ISP configuration block* is preceded by an header implemented by the
:c:type:`v4l2_isp_params_block_header` structure, followed by the configuration
parameters for that specific block, defined by the ISP driver specific data
types.
(每个 *ISP 配置块* 的前面都有一个由
:c:type:`v4l2_isp_params_block_header` 结构实现的头，其后是该特定块的
配置参数，由 ISP 驱动程序专属的数据类型定义。)

Userspace applications are responsible for correctly populating each block's
header fields (type, flags and size) and the block-specific parameters.
(用户空间应用程序负责正确填充每个块的头字段 (type、flags 与 size) 以
及块专属的参数。)

ISP block enabling, disabling and configuration(启用、禁用与配置 ISP 块)
------------------------------------------------------------------------

When userspace wants to configure and enable an ISP block it shall fully
populate the block configuration and set the V4L2_ISP_PARAMS_FL_BLOCK_ENABLE
bit in the block header's `flags` field.
(当用户空间想配置并启用一个 ISP 块时，它应完整填写该块的配置，并把
V4L2_ISP_PARAMS_FL_BLOCK_ENABLE 位设置到块头的 ``flags`` 字段中。)

When userspace simply wants to disable an ISP block the
V4L2_ISP_PARAMS_FL_BLOCK_DISABLE bit should be set in block header's `flags`
field. Drivers accept a configuration parameters block with no additional
data after the header in this case.
(当用户空间只想禁用一个 ISP 块时，应在块头的 ``flags`` 字段中设
置 V4L2_ISP_PARAMS_FL_BLOCK_DISABLE 位。在这种情况下，驱动程序接受头
之后没有附加数据的配置参数块。)

If the configuration of an already active ISP block has to be updated,
userspace shall fully populate the ISP block parameters and omit setting the
V4L2_ISP_PARAMS_FL_BLOCK_ENABLE and V4L2_ISP_PARAMS_FL_BLOCK_DISABLE bits in the
header's `flags` field.
(如果需要更新一个已激活的 ISP 块的配置，用户空间应完整填写 ISP 块参
数，并且不在头的 ``flags`` 字段中设置
V4L2_ISP_PARAMS_FL_BLOCK_ENABLE 与 V4L2_ISP_PARAMS_FL_BLOCK_DISABLE 位。)

Setting both the V4L2_ISP_PARAMS_FL_BLOCK_ENABLE and
V4L2_ISP_PARAMS_FL_BLOCK_DISABLE bits in the flags field is not allowed and
returns an error.
(不允许在 flags 字段中同时设置 V4L2_ISP_PARAMS_FL_BLOCK_ENABLE 与
V4L2_ISP_PARAMS_FL_BLOCK_DISABLE 位，否则会返回错误。)

Extension to the parameters format can be implemented by adding new blocks
definition without invalidating the existing ones.
(可以通过添加新的块定义来扩展参数格式，而不会使现有的块定义失效。)

ISP statistics(ISP 统计信息)
============================

Support for generic statistics format is not yet implemented in Video4Linux2.
(Video4Linux2 尚未实现对通用统计信息格式的支持。)

V4L2 ISP uAPI data types(V4L2 ISP uAPI 数据类型)
================================================

.. kernel-doc:: include/uapi/linux/media/v4l2-isp.h
