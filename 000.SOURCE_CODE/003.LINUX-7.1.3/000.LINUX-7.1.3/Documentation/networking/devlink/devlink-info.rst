.. SPDX-License-Identifier: (GPL-2.0-only OR BSD-2-Clause)

============
Devlink Info
============

(Devlink Info)

The ``devlink-info`` mechanism enables device drivers to report device
(hardware and firmware) information in a standard, extensible fashion.
(``devlink-info`` 机制使设备驱动能够以标准的、可扩展的方式报告设备（硬件与固件）信息。)

The original motivation for the ``devlink-info`` API was twofold:
(``devlink-info`` API 的最初动机有两个：)

 - making it possible to automate device and firmware management in a fleet
   of machines in a vendor-independent fashion (see also
   :ref:`Documentation/networking/devlink/devlink-flash.rst <devlink_flash>`);
   (以厂商无关的方式实现机群中设备与固件管理的自动化（另见
   :ref:`Documentation/networking/devlink/devlink-flash.rst <devlink_flash>`）；)
 - name the per component FW versions (as opposed to the crowded ethtool
   version string).
   (为每个组件的 FW 版本命名（相对于拥挤的 ethtool 版本字符串而言）。)

``devlink-info`` supports reporting multiple types of objects. Reporting driver
versions is generally discouraged - here, and via any other Linux API.
(``devlink-info`` 支持报告多种类型的对象。一般不鼓励报告驱动版本——无论是在这里还是通过任何其他 Linux API。)

.. list-table:: List of top level info objects
   :widths: 5 95

   * - Name (名称)
     - Description (描述)
   * - ``driver``
     - Name of the currently used device driver, also available through sysfs.
       (当前使用的设备驱动的名称，也可通过 sysfs 获得。)

   * - ``serial_number``
     - Serial number of the device.
       (设备的序列号。)

       This is usually the serial number of the ASIC, also often available
       in PCI config space of the device in the *Device Serial Number*
       capability.
       (这通常是 ASIC 的序列号，通常也可以在设备 PCI 配置空间的 *Device Serial Number* 能力中读到。)

       The serial number should be unique per physical device.
       Sometimes the serial number of the device is only 48 bits long (the
       length of the Ethernet MAC address), and since PCI DSN is 64 bits long
       devices pad or encode additional information into the serial number.
       One example is adding port ID or PCI interface ID in the extra two bytes.
       Drivers should make sure to strip or normalize any such padding
       or interface ID, and report only the part of the serial number
       which uniquely identifies the hardware. In other words serial number
       reported for two ports of the same device or on two hosts of
       a multi-host device should be identical.
       (序列号应当对每个物理设备唯一。有时设备的序列号只有 48 位长（即以太网 MAC 地址的长度），而由于 PCI DSN 有 64 位长，设备会把额外的信息填充或编码进序列号。一个例子是把端口 ID 或 PCI 接口 ID 放进多出来的两个字节中。驱动应确保剥离或规范化任何此类填充或接口 ID，只报告序列号中唯一标识硬件的部分。换言之，同一设备的两个端口、或多宿主设备的两个主机所报告的序列号应当一致。)

   * - ``board.serial_number``
     - Board serial number of the device.
       (设备的板卡序列号。)

       This is usually the serial number of the board, often available in
       PCI *Vital Product Data*.
       (这通常是板卡的序列号，通常可以在 PCI *Vital Product Data* 中读到。)

   * - ``fixed``
     - Group for hardware identifiers, and versions of components
       which are not field-updatable.
       (硬件标识符以及不可现场更新的组件版本的分组。)

       Versions in this section identify the device design. For example,
       component identifiers or the board version reported in the PCI VPD.
       Data in ``devlink-info`` should be broken into the smallest logical
       components, e.g. PCI VPD may concatenate various information
       to form the Part Number string, while in ``devlink-info`` all parts
       should be reported as separate items.
       (本节中的版本标识设备设计。例如，组件标识符或 PCI VPD 中报告的板卡版本。``devlink-info`` 中的数据应拆分为最小的逻辑组件，例如 PCI VPD 可能会把各种信息拼接在一起构成 Part Number 字符串，而在 ``devlink-info`` 中所有部分都应作为单独的条目报告。)

       This group must not contain any frequently changing identifiers,
       such as serial numbers. See
       :ref:`Documentation/networking/devlink/devlink-flash.rst <devlink_flash>`
       to understand why.
       (此分组不得包含任何频繁变化的标识符，例如序列号。原因参见
       :ref:`Documentation/networking/devlink/devlink-flash.rst <devlink_flash>`。)

   * - ``running``
     - Group for information about currently running software/firmware.
       These versions often only update after a reboot, sometimes device reset.
       (关于当前正在运行的软件/固件信息的分组。这些版本通常只在重启后更新，有时需要设备复位。)

   * - ``stored``
     - Group for software/firmware versions in device flash.
       (设备闪存中软件/固件版本的分组。)

       Stored values must update to reflect changes in the flash even
       if reboot has not yet occurred. If device is not capable of updating
       ``stored`` versions when new software is flashed, it must not report
       them.
       (即使尚未重启，存储值也必须更新以反映闪存中的变化。如果设备在刷入新软件后无法更新 ``stored`` 版本，则不得报告它们。)

Each version can be reported at most once in each version group. Firmware
components stored on the flash should feature in both the ``running`` and
``stored`` sections, if device is capable of reporting ``stored`` versions
(see :ref:`Documentation/networking/devlink/devlink-flash.rst <devlink_flash>`).
In case software/firmware components are loaded from the disk (e.g.
``/lib/firmware``) only the running version should be reported via
the kernel API.
(每个版本在每个版本分组中最多只能报告一次。如果设备能够报告 ``stored`` 版本（参见 :ref:`Documentation/networking/devlink/devlink-flash.rst <devlink_flash>`），存储在闪存中的固件组件应同时出现在 ``running`` 和 ``stored`` 两个部分中。如果软件/固件组件从磁盘加载（例如 ``/lib/firmware``），则只应通过内核 API 报告其运行版本。)

Please note that any security versions reported via devlink are purely
informational. Devlink does not use a secure channel to communicate with
the device.
(请注意，通过 devlink 报告的任何安全版本都纯粹是信息性的。Devlink 不会使用安全通道与设备通信。)

Generic Versions(通用版本)
==========================

It is expected that drivers use the following generic names for exporting
version information. If a generic name for a given component doesn't exist yet,
driver authors should consult existing driver-specific versions and attempt
reuse. As last resort, if a component is truly unique, using driver-specific
names is allowed, but these should be documented in the driver-specific file.
(期望驱动使用以下通用名称来导出版本信息。如果给定组件尚无通用名称，驱动作者应参考现有的驱动特定版本并尝试复用。万不得已时，如果某个组件确实独一无二，允许使用驱动特定的名称，但这些名称应在驱动特定的文件中加以说明。)

All versions should try to use the following terminology:
(所有版本都应尽量使用以下术语：)

.. list-table:: List of common version suffixes
   :widths: 10 90

   * - Name (名称)
     - Description (描述)
   * - ``id``, ``revision``
     - Identifiers of designs and revision, mostly used for hardware versions.
       (设计与修订的标识符，主要用于硬件版本。)

   * - ``api``
     - Version of API between components. API items are usually of limited
       value to the user, and can be inferred from other versions by the vendor,
       so adding API versions is generally discouraged as noise.
       (组件之间 API 的版本。API 条目通常对用户价值有限，厂商可以从其他版本推断出来，因此一般不鼓励添加 API 版本，因其属于噪声。)

   * - ``bundle_id``
     - Identifier of a distribution package which was flashed onto the device.
       This is an attribute of a firmware package which covers multiple versions
       for ease of managing firmware images (see
       :ref:`Documentation/networking/devlink/devlink-flash.rst <devlink_flash>`).
       (刷写到设备上的发行包的标识符。这是固件包的一个属性，它涵盖多个版本，便于管理固件镜像（参见
       :ref:`Documentation/networking/devlink/devlink-flash.rst <devlink_flash>`）。)

       ``bundle_id`` can appear in both ``running`` and ``stored`` versions,
       but it must not be reported if any of the components covered by the
       ``bundle_id`` was changed and no longer matches the version from
       the bundle.
       (``bundle_id`` 可以同时出现在 ``running`` 和 ``stored`` 版本中，但如果 ``bundle_id`` 所涵盖的任何组件已被更改、不再与捆绑包中的版本匹配，则不得报告它。)

board.id(board.id（板卡设计唯一标识符）)
----------------------------------------

Unique identifier of the board design.
(板卡设计的唯一标识符。)

board.rev(board.rev（板卡设计修订版本）)
----------------------------------------

Board design revision.
(板卡设计修订版本。)

asic.id(asic.id（ASIC 设计标识符）)
-----------------------------------

ASIC design identifier.
(ASIC 设计标识符。)

asic.rev(asic.rev（ASIC 设计修订版本）)
---------------------------------------

ASIC design revision/stepping.
(ASIC 设计修订版本/步进。)

board.manufacture(board.manufacture（制造商标识）)
--------------------------------------------------

An identifier of the company or the facility which produced the part.
(生产该部件的公司或工厂的标识符。)

board.part_number(board.part_number（板卡部件号）)
--------------------------------------------------

Part number of the board and its components.
(板卡及其组件的部件号。)

fw(fw（固件总体版本）)
----------------------

Overall firmware version, often representing the collection of
fw.mgmt, fw.app, etc.
(固件总体版本，通常代表 fw.mgmt、fw.app 等的集合。)

fw.mgmt(fw.mgmt（管理固件版本）)
--------------------------------

Control unit firmware version. This firmware is responsible for house
keeping tasks, PHY control etc. but not the packet-by-packet data path
operation.
(控制单元固件版本。该固件负责日常管理任务、PHY 控制等，但不负责逐个数据包的数据路径操作。)

fw.mgmt.api(fw.mgmt.api（管理固件 API 版本）)
---------------------------------------------

Firmware interface specification version of the software interfaces between
driver and firmware.
(驱动与固件之间软件接口的固件接口规范版本。)

fw.app(fw.app（应用固件版本）)
------------------------------

Data path microcode controlling high-speed packet processing.
(控制高速数据包处理的数据路径微码。)

fw.undi(fw.undi（UNDI 软件版本）)
---------------------------------

UNDI software, may include the UEFI driver, firmware or both.
(UNDI 软件，可能包含 UEFI 驱动、固件，或两者兼有。)

fw.ncsi(fw.ncsi（NCSI 软件版本）)
---------------------------------

Version of the software responsible for supporting/handling the
Network Controller Sideband Interface.
(负责支持/处理网络控制器边带接口 (Network Controller Sideband Interface) 的软件版本。)

fw.psid(fw.psid（固件参数集标识符）)
------------------------------------

Unique identifier of the firmware parameter set. These are usually
parameters of a particular board, defined at manufacturing time.
(固件参数集的唯一标识符。这些通常是某个特定板卡的参数，在制造时定义。)

fw.roce(fw.roce（RoCE 固件版本）)
---------------------------------

RoCE firmware version which is responsible for handling roce
management.
(负责处理 RoCE 管理的 RoCE 固件版本。)

fw.bundle_id(fw.bundle_id（固件捆绑包标识符）)
----------------------------------------------

Unique identifier of the entire firmware bundle.
(整个固件捆绑包的唯一标识符。)

fw.bootloader(fw.bootloader（引导加载程序版本）)
------------------------------------------------

Version of the bootloader.
(引导加载程序的版本。)

Future work(未来工作)
=====================

The following extensions could be useful:
(以下扩展可能会有用：)

 - on-disk firmware file names - drivers list the file names of firmware they
   may need to load onto devices via the ``MODULE_FIRMWARE()`` macro. These,
   however, are per module, rather than per device. It'd be useful to list
   the names of firmware files the driver will try to load for a given device,
   in order of priority.
   (磁盘上的固件文件名——驱动通过 ``MODULE_FIRMWARE()`` 宏列出它们可能需要加载到设备上的固件文件名。然而，这些是按模块而非按设备列出的。若能按优先级顺序列出驱动将为给定设备尝试加载的固件文件名，会很有用。)
