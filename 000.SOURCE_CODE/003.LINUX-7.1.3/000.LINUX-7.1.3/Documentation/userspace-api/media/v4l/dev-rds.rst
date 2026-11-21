.. SPDX-License-Identifier: GFDL-1.1-no-invariants-or-later
.. c:namespace:: V4L

.. _rds:

***********************
RDS Interface(RDS 接口)
***********************

The Radio Data System transmits supplementary information in binary
format, for example the station name or travel information, on an
inaudible audio subcarrier of a radio program. This interface is aimed
at devices capable of receiving and/or transmitting RDS information.
(无线电数据系统 (RDS) 以二进制格式在电台节目的不可闻音频副载波上传输辅助信息，例如电台名称或出行信息。此接口面向能够接收和/或发送 RDS 信息的设备。)

For more information see the core RDS standard :ref:`iec62106` and the
RBDS standard :ref:`nrsc4`.
(更多信息请参阅核心 RDS 标准 :ref:`iec62106` 和 RBDS 标准 :ref:`nrsc4` 。)

.. note::

   Note that the RBDS standard as is used in the USA is almost
   identical to the RDS standard. Any RDS decoder/encoder can also handle
   RBDS. Only some of the fields have slightly different meanings. See the
   RBDS standard for more information.
   (注意，美国使用的 RBDS 标准与 RDS 标准几乎完全相同。任何 RDS 解码器/编码器也能处理 RBDS。只有部分字段的含义略有不同。更多信息请参阅 RBDS 标准。)

The RBDS standard also specifies support for MMBS (Modified Mobile
Search). This is a proprietary format which seems to be discontinued.
The RDS interface does not support this format. Should support for MMBS
(or the so-called 'E blocks' in general) be needed, then please contact
the linux-media mailing list:
`https://linuxtv.org/lists.php <https://linuxtv.org/lists.php>`__.
(RBDS 标准还规定了对 MMBS (Modified Mobile Search) 的支持。这是一种似乎已经停止使用的专有格式。RDS 接口不支持这种格式。如果需要支持 MMBS(或一般而言所谓的 'E blocks')，请联系 linux-media 邮件列表：
`https://linuxtv.org/lists.php <https://linuxtv.org/lists.php>`__ 。)

Querying Capabilities(查询能力)
===============================

Devices supporting the RDS capturing API set the
``V4L2_CAP_RDS_CAPTURE`` flag in the ``capabilities`` field of struct
:c:type:`v4l2_capability` returned by the
:ref:`VIDIOC_QUERYCAP` ioctl. Any tuner that
supports RDS will set the ``V4L2_TUNER_CAP_RDS`` flag in the
``capability`` field of struct :c:type:`v4l2_tuner`. If the
driver only passes RDS blocks without interpreting the data the
``V4L2_TUNER_CAP_RDS_BLOCK_IO`` flag has to be set, see
:ref:`Reading RDS data <reading-rds-data>`. For future use the flag
``V4L2_TUNER_CAP_RDS_CONTROLS`` has also been defined. However, a driver
for a radio tuner with this capability does not yet exist, so if you are
planning to write such a driver you should discuss this on the
linux-media mailing list:
`https://linuxtv.org/lists.php <https://linuxtv.org/lists.php>`__.
(支持 RDS 捕获 API 的设备会在 :ref:`VIDIOC_QUERYCAP` ioctl 返回的 struct :c:type:`v4l2_capability` 的 ``capabilities`` 字段中设置 ``V4L2_CAP_RDS_CAPTURE`` 标志。任何支持 RDS 的调谐器都会在 struct :c:type:`v4l2_tuner` 的 ``capability`` 字段中设置 ``V4L2_TUNER_CAP_RDS`` 标志。如果驱动只传递 RDS 块而不解释数据，则必须设置 ``V4L2_TUNER_CAP_RDS_BLOCK_IO`` 标志，参见 :ref:`Reading RDS data <reading-rds-data>` 。为将来使用而定义的标志还有 ``V4L2_TUNER_CAP_RDS_CONTROLS`` 。然而，具备此能力的广播调谐器驱动尚不存在，因此如果你计划编写这样的驱动，应在 linux-media 邮件列表上讨论：
`https://linuxtv.org/lists.php <https://linuxtv.org/lists.php>`__ 。)

Whether an RDS signal is present can be detected by looking at the
``rxsubchans`` field of struct :c:type:`v4l2_tuner`: the
``V4L2_TUNER_SUB_RDS`` will be set if RDS data was detected.
(可以通过查看 struct :c:type:`v4l2_tuner` 的 ``rxsubchans`` 字段来检测是否存在 RDS 信号：如果检测到 RDS 数据，则会设置 ``V4L2_TUNER_SUB_RDS`` 。)

Devices supporting the RDS output API set the ``V4L2_CAP_RDS_OUTPUT``
flag in the ``capabilities`` field of struct
:c:type:`v4l2_capability` returned by the
:ref:`VIDIOC_QUERYCAP` ioctl. Any modulator that
supports RDS will set the ``V4L2_TUNER_CAP_RDS`` flag in the
``capability`` field of struct
:c:type:`v4l2_modulator`. In order to enable the RDS
transmission one must set the ``V4L2_TUNER_SUB_RDS`` bit in the
``txsubchans`` field of struct
:c:type:`v4l2_modulator`. If the driver only passes RDS
blocks without interpreting the data the ``V4L2_TUNER_CAP_RDS_BLOCK_IO``
flag has to be set. If the tuner is capable of handling RDS entities
like program identification codes and radio text, the flag
``V4L2_TUNER_CAP_RDS_CONTROLS`` should be set, see
:ref:`Writing RDS data <writing-rds-data>` and
:ref:`FM Transmitter Control Reference <fm-tx-controls>`.
(支持 RDS 输出 API 的设备会在 :ref:`VIDIOC_QUERYCAP` ioctl 返回的 struct :c:type:`v4l2_capability` 的 ``capabilities`` 字段中设置 ``V4L2_CAP_RDS_OUTPUT`` 标志。任何支持 RDS 的调制器都会在 struct :c:type:`v4l2_modulator` 的 ``capability`` 字段中设置 ``V4L2_TUNER_CAP_RDS`` 标志。为了启用 RDS 发送，必须在 struct :c:type:`v4l2_modulator` 的 ``txsubchans`` 字段中设置 ``V4L2_TUNER_SUB_RDS`` 位。如果驱动只传递 RDS 块而不解释数据，则必须设置 ``V4L2_TUNER_CAP_RDS_BLOCK_IO`` 标志。如果调谐器能够处理节目识别码和电台文本等 RDS 实体，则应设置 ``V4L2_TUNER_CAP_RDS_CONTROLS`` 标志，参见 :ref:`Writing RDS data <writing-rds-data>` 和 :ref:`FM Transmitter Control Reference <fm-tx-controls>` 。)

.. _reading-rds-data:

Reading RDS data(读取 RDS 数据)
===============================

RDS data can be read from the radio device with the
:c:func:`read()` function. The data is packed in groups of
three bytes.
(RDS 数据可以使用 :c:func:`read()` 函数从广播设备中读取。数据以每三个字节为一组进行打包。)

.. _writing-rds-data:

Writing RDS data(写入 RDS 数据)
===============================

RDS data can be written to the radio device with the
:c:func:`write()` function. The data is packed in groups of
three bytes, as follows:
(RDS 数据可以使用 :c:func:`write()` 函数写入广播设备。数据以每三个字节为一组进行打包，具体如下：)

RDS datastructures(RDS 数据结构)
================================

.. c:type:: v4l2_rds_data

.. flat-table:: struct v4l2_rds_data
    :header-rows:  0
    :stub-columns: 0
    :widths:       1 1 5

    * - __u8
      - ``lsb``
      - Least Significant Byte of RDS Block(接收到的 RDS 块的最低有效字节)
    * - __u8
      - ``msb``
      - Most Significant Byte of RDS Block(RDS 块的最高有效字节)
    * - __u8
      - ``block``
      - Block description(块描述)


.. _v4l2-rds-block:

.. tabularcolumns:: |p{2.9cm}|p{14.6cm}|

.. flat-table:: Block description
    :header-rows:  0
    :stub-columns: 0
    :widths:       1 5

    * - Bits 0-2
      - Block (aka offset) of the received data.(接收数据的块(也称为偏移)。)
    * - Bits 3-5
      - Deprecated. Currently identical to bits 0-2. Do not use these
	bits.
	(已弃用。当前与位 0-2 相同。不要使用这些位。)
    * - Bit 6
      - Corrected bit. Indicates that an error was corrected for this data
	block.
	(已纠正位。表示此数据块中的某个错误已被纠正。)
    * - Bit 7
      - Error bit. Indicates that an uncorrectable error occurred during
	reception of this block.
	(错误位。表示在接收此块期间发生了无法纠正的错误。)


.. _v4l2-rds-block-codes:

.. tabularcolumns:: |p{6.4cm}|p{2.0cm}|p{1.2cm}|p{7.0cm}|

.. flat-table:: Block defines
    :header-rows:  0
    :stub-columns: 0
    :widths:       1 1 1 5

    * - V4L2_RDS_BLOCK_MSK
      -
      - 7
      - Mask for bits 0-2 to get the block ID.(用于获取块 ID 的位 0-2 的掩码。)
    * - V4L2_RDS_BLOCK_A
      -
      - 0
      - Block A.(块 A。)
    * - V4L2_RDS_BLOCK_B
      -
      - 1
      - Block B.(块 B。)
    * - V4L2_RDS_BLOCK_C
      -
      - 2
      - Block C.(块 C。)
    * - V4L2_RDS_BLOCK_D
      -
      - 3
      - Block D.(块 D。)
    * - V4L2_RDS_BLOCK_C_ALT
      -
      - 4
      - Block C'.(块 C'。)
    * - V4L2_RDS_BLOCK_INVALID
      - read-only(只读)
      - 7
      - An invalid block.(无效的块。)
    * - V4L2_RDS_BLOCK_CORRECTED
      - read-only(只读)
      - 0x40
      - A bit error was detected but corrected.(检测到位错误但已纠正。)
    * - V4L2_RDS_BLOCK_ERROR
      - read-only(只读)
      - 0x80
      - An uncorrectable error occurred.(发生了无法纠正的错误。)
