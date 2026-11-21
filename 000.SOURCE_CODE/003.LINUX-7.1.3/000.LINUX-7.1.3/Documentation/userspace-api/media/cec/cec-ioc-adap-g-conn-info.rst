.. SPDX-License-Identifier: GPL-2.0
..
.. Copyright 2019 Google LLC
..
.. c:namespace:: CEC

.. _CEC_ADAP_G_CONNECTOR_INFO:

*****************************************************
ioctl CEC_ADAP_G_CONNECTOR_INFO(查询 HDMI 连接器信息)
*****************************************************

Name(名称)
==========

CEC_ADAP_G_CONNECTOR_INFO - Query HDMI connector information(CEC_ADAP_G_CONNECTOR_INFO - 查询 HDMI 连接器信息)

Synopsis(概要)
==============

.. c:macro:: CEC_ADAP_G_CONNECTOR_INFO

``int ioctl(int fd, CEC_ADAP_G_CONNECTOR_INFO, struct cec_connector_info *argp)``

Arguments(参数)
===============

``fd``
    File descriptor returned by :c:func:`open()`.
    (:c:func:`open()` 返回的文件描述符。)

``argp``

Description(描述)
=================

Using this ioctl an application can learn which HDMI connector this CEC
device corresponds to. While calling this ioctl the application should
provide a pointer to a cec_connector_info struct which will be populated
by the kernel with the info provided by the adapter's driver. This ioctl
is only available if the ``CEC_CAP_CONNECTOR_INFO`` capability is set.
(通过这个 ioctl，应用程序可以了解该 CEC 设备对应哪个 HDMI 连接器。
调用这个 ioctl 时，应用程序应提供一个指向 cec_connector_info 结构体的
指针，内核会用适配器驱动程序提供的信息填充它。仅当设置了
``CEC_CAP_CONNECTOR_INFO`` 能力时，该 ioctl 才可用。)

.. tabularcolumns:: |p{1.0cm}|p{4.4cm}|p{2.5cm}|p{9.2cm}|

.. c:type:: cec_connector_info

.. flat-table:: struct cec_connector_info
    :header-rows:  0
    :stub-columns: 0
    :widths:       1 1 8

    * - __u32
      - ``type``
      - The type of connector this adapter is associated with.
        (此适配器关联的连接器类型。)
    * - union {
      - ``(anonymous)``
    * - ``struct cec_drm_connector_info``
      - drm
      - :ref:`cec-drm-connector-info`
    * - }
      -

.. tabularcolumns:: |p{4.4cm}|p{2.5cm}|p{10.4cm}|

.. _connector-type:

.. flat-table:: Connector types
    :header-rows:  0
    :stub-columns: 0
    :widths:       3 1 8

    * .. _`CEC-CONNECTOR-TYPE-NO-CONNECTOR`:

      - ``CEC_CONNECTOR_TYPE_NO_CONNECTOR``
      - 0
      - No connector is associated with the adapter/the information is not
        provided by the driver.
        (没有连接器与适配器关联/驱动程序未提供该信息。)
    * .. _`CEC-CONNECTOR-TYPE-DRM`:

      - ``CEC_CONNECTOR_TYPE_DRM``
      - 1
      - Indicates that a DRM connector is associated with this adapter.
        Information about the connector can be found in
	:ref:`cec-drm-connector-info`.
        (表示有 DRM 连接器与该适配器关联。连接器的信息可以在
        :ref:`cec-drm-connector-info` 中找到。)

.. tabularcolumns:: |p{4.4cm}|p{2.5cm}|p{10.4cm}|

.. c:type:: cec_drm_connector_info

.. _cec-drm-connector-info:

.. flat-table:: struct cec_drm_connector_info
    :header-rows:  0
    :stub-columns: 0
    :widths:       3 1 8

    * .. _`CEC-DRM-CONNECTOR-TYPE-CARD-NO`:

      - __u32
      - ``card_no``
      - DRM card number: the number from a card's path, e.g. 0 in case of
        /dev/card0.
        (DRM 卡号：卡的路径中的编号，例如 /dev/card0 中的 0。)
    * .. _`CEC-DRM-CONNECTOR-TYPE-CONNECTOR_ID`:

      - __u32
      - ``connector_id``
      - DRM connector ID.
        (DRM 连接器 ID。)
