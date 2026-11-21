.. SPDX-License-Identifier: GFDL-1.1-no-invariants-or-later

***********************
Frontend type(前端类型)
***********************

For historical reasons, frontend types are named by the type of
modulation used in transmission. The fontend types are given by
fe_type_t type, defined as:
(出于历史原因，前端类型按传输中使用的调制方式命名。前端类型由
fe_type_t 类型给出，定义如下：)


.. c:type:: fe_type

.. tabularcolumns:: |p{6.6cm}|p{2.2cm}|p{8.5cm}|

.. flat-table:: Frontend types
    :header-rows:  1
    :stub-columns: 0
    :widths:       3 1 4


    -  .. row 1

       -  fe_type(类型)

       -  Description(说明)

       -  :ref:`DTV_DELIVERY_SYSTEM <DTV-DELIVERY-SYSTEM>` equivalent
	  type

    -  .. row 2

       -  .. _FE-QPSK:

	  ``FE_QPSK``

       -  For DVB-S standard
          (适用于 DVB-S 标准)

       -  ``SYS_DVBS``

    -  .. row 3

       -  .. _FE-QAM:

	  ``FE_QAM``

       -  For DVB-C annex A standard
          (适用于 DVB-C annex A 标准)

       -  ``SYS_DVBC_ANNEX_A``

    -  .. row 4

       -  .. _FE-OFDM:

	  ``FE_OFDM``

       -  For DVB-T standard
          (适用于 DVB-T 标准)

       -  ``SYS_DVBT``

    -  .. row 5

       -  .. _FE-ATSC:

	  ``FE_ATSC``

       -  For ATSC standard (terrestrial) or for DVB-C Annex B (cable) used
	  in US.
          (适用于美国的 ATSC 标准(地面)或 DVB-C Annex B(有线)。)

       -  ``SYS_ATSC`` (terrestrial) or ``SYS_DVBC_ANNEX_B`` (cable)


Newer formats like DVB-S2, ISDB-T, ISDB-S and DVB-T2 are not described
at the above, as they're supported via the new
:ref:`FE_GET_PROPERTY/FE_GET_SET_PROPERTY <FE_GET_PROPERTY>`
ioctl's, using the :ref:`DTV_DELIVERY_SYSTEM <DTV-DELIVERY-SYSTEM>`
parameter.
(像 DVB-S2、ISDB-T、ISDB-S 和 DVB-T2 这样较新的格式不在上表中描述，
因为它们通过新的 :ref:`FE_GET_PROPERTY/FE_GET_SET_PROPERTY <FE_GET_PROPERTY>`
ioctl 并使用 :ref:`DTV_DELIVERY_SYSTEM <DTV-DELIVERY-SYSTEM>` 参数来
支持。)

In the old days, struct :c:type:`dvb_frontend_info`
used to contain ``fe_type_t`` field to indicate the delivery systems,
filled with either ``FE_QPSK, FE_QAM, FE_OFDM`` or ``FE_ATSC``. While this
is still filled to keep backward compatibility, the usage of this field
is deprecated, as it can report just one delivery system, but some
devices support multiple delivery systems. Please use
:ref:`DTV_ENUM_DELSYS <DTV-ENUM-DELSYS>` instead.
(早先，struct :c:type:`dvb_frontend_info` 曾包含 ``fe_type_t`` 字段
来指示传送系统，填充为 ``FE_QPSK, FE_QAM, FE_OFDM`` 或 ``FE_ATSC``。
虽然为了保持向后兼容它仍会被填充，但该字段的用法已被弃用，因为它
只能报告一个传送系统，而有些设备支持多个传送系统。请改用
:ref:`DTV_ENUM_DELSYS <DTV-ENUM-DELSYS>`。)

On devices that support multiple delivery systems, struct
:c:type:`dvb_frontend_info`::``fe_type_t`` is
filled with the currently standard, as selected by the last call to
:ref:`FE_SET_PROPERTY <FE_GET_PROPERTY>` using the
:ref:`DTV_DELIVERY_SYSTEM <DTV-DELIVERY-SYSTEM>` property.
(在支持多个传送系统的设备上，struct :c:type:`dvb_frontend_info`::``fe_type_t``
会填充为当前标准，即由最后一次使用 :ref:`DTV_DELIVERY_SYSTEM <DTV-DELIVERY-SYSTEM>`
属性调用 :ref:`FE_SET_PROPERTY <FE_GET_PROPERTY>` 所选择的标准。)
