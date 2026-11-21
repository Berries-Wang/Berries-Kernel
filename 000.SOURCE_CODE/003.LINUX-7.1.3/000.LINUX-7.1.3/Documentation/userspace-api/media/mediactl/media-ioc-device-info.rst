.. SPDX-License-Identifier: GFDL-1.1-no-invariants-or-later
.. c:namespace:: MC

.. _media_ioc_device_info:

*****************************************
ioctl MEDIA_IOC_DEVICE_INFO(查询设备信息)
*****************************************

Name(名称)
==========

MEDIA_IOC_DEVICE_INFO - Query device information(MEDIA_IOC_DEVICE_INFO - 查询设备信息)

Synopsis(概要)
==============

.. c:macro:: MEDIA_IOC_DEVICE_INFO

``int ioctl(int fd, MEDIA_IOC_DEVICE_INFO, struct media_device_info *argp)``

Arguments(参数)
===============

``fd``
    File descriptor returned by :c:func:`open()`.
    (:c:func:`open()` 返回的文件描述符。)

``argp``
    Pointer to struct :c:type:`media_device_info`.
    (指向 struct :c:type:`media_device_info` 的指针。)

Description(描述)
=================

All media devices must support the ``MEDIA_IOC_DEVICE_INFO`` ioctl. To
query device information, applications call the ioctl with a pointer to
a struct :c:type:`media_device_info`. The driver
fills the structure and returns the information to the application. The
ioctl never fails.
(所有 media 设备都必须支持 ``MEDIA_IOC_DEVICE_INFO`` ioctl。要查询
设备信息，应用程序以指向 struct :c:type:`media_device_info` 的指针
调用该 ioctl。驱动程序填充该结构体并把信息返回给应用程序。该 ioctl
永不失败。)

.. c:type:: media_device_info

.. tabularcolumns:: |p{4.4cm}|p{4.4cm}|p{8.5cm}|

.. flat-table:: struct media_device_info
    :header-rows:  0
    :stub-columns: 0
    :widths:       1 1 2

    *  -  char
       -  ``driver``\ [16]
       -  Name of the driver implementing the media API as a NUL-terminated
	  ASCII string. The driver version is stored in the
	  ``driver_version`` field.
          (实现 media API 的驱动程序名称，为以 NUL 结尾的 ASCII 字符串。
          驱动版本存储在 ``driver_version`` 字段中。)

	  Driver specific applications can use this information to verify
	  the driver identity. It is also useful to work around known bugs,
	  or to identify drivers in error reports.
          (驱动专属的应用程序可以用此信息来验证驱动程序的身份。它对于
          绕过已知缺陷或在错误报告中识别驱动程序也很有用。)

    *  -  char
       -  ``model``\ [32]
       -  Device model name as a NUL-terminated UTF-8 string. The device
	  version is stored in the ``device_version`` field and is not be
	  appended to the model name.
          (设备型号名称，为以 NUL 结尾的 UTF-8 字符串。设备版本存储在
          ``device_version`` 字段中，不会附加到型号名称后面。)

    *  -  char
       -  ``serial``\ [40]
       -  Serial number as a NUL-terminated ASCII string.
          (序列号，为以 NUL 结尾的 ASCII 字符串。)

    *  -  char
       -  ``bus_info``\ [32]
       -  Location of the device in the system as a NUL-terminated ASCII
	  string. This includes the bus type name (PCI, USB, ...) and a
	  bus-specific identifier.
          (设备在系统中的位置，为以 NUL 结尾的 ASCII 字符串。这包括总线
          类型名称(PCI、USB 等)和总线特定的标识符。)

    *  -  __u32
       -  ``media_version``
       -  Media API version, formatted with the ``KERNEL_VERSION()`` macro.
          (Media API 版本，用 ``KERNEL_VERSION()`` 宏格式化。)

    *  -  __u32
       -  ``hw_revision``
       -  Hardware device revision in a driver-specific format.
          (硬件设备修订版本，格式由驱动程序决定。)

    *  -  __u32
       -  ``driver_version``
       -  Media device driver version, formatted with the
	  ``KERNEL_VERSION()`` macro. Together with the ``driver`` field
	  this identifies a particular driver.
          (Media 设备驱动版本，用 ``KERNEL_VERSION()`` 宏格式化。它与
          ``driver`` 字段一起标识一个特定的驱动程序。)

    *  -  __u32
       -  ``reserved``\ [31]
       -  Reserved for future extensions. Drivers and applications must set
	  this array to zero.
          (保留用于未来的扩展。驱动程序和应用程序必须将此数组置零。)

The ``serial`` and ``bus_info`` fields can be used to distinguish
between multiple instances of otherwise identical hardware. The serial
number takes precedence when provided and can be assumed to be unique.
If the serial number is an empty string, the ``bus_info`` field can be
used instead. The ``bus_info`` field is guaranteed to be unique, but can
vary across reboots or device unplug/replug.
(``serial`` 和 ``bus_info`` 字段可用于区分其他方面完全相同的多个硬件
实例。如果提供序列号，它具有最高优先级，并且可以认为是唯一的。如果
序列号是空字符串，则可以改用 ``bus_info`` 字段。``bus_info`` 字段
保证唯一，但在重新启动或设备拔插后可能会改变。)

Return Value(返回值)
====================

On success 0 is returned, on error -1 and the ``errno`` variable is set
appropriately. The generic error codes are described at the
:ref:`Generic Error Codes <gen-errors>` chapter.
(成功时返回 0，出错时返回 -1 并适当设置 ``errno`` 变量。通用错误码在
:ref:`Generic Error Codes <gen-errors>` 一章中描述。)
