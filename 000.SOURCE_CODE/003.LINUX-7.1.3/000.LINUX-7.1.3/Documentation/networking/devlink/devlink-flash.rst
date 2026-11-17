.. SPDX-License-Identifier: (GPL-2.0-only OR BSD-2-Clause)

.. _devlink_flash:

=============
Devlink Flash
=============

(devlink 固件刷写)

The ``devlink-flash`` API allows updating device firmware. It replaces the
older ``ethtool-flash`` mechanism, and doesn't require taking any
networking locks in the kernel to perform the flash update. Example use::
(``devlink-flash`` API 允许更新设备固件。它取代了较旧的 ``ethtool-flash``
机制，并且在执行固件刷写时不需要在内核中获取任何网络锁。用法示例：)

  $ devlink dev flash pci/0000:05:00.0 file flash-boot.bin

Note that the file name is a path relative to the firmware loading path
(usually ``/lib/firmware/``). Drivers may send status updates to inform
user space about the progress of the update operation.
(注意，文件名是相对于固件加载路径 (通常为 ``/lib/firmware/``) 的路径。
驱动可以发送状态更新，向用户空间告知更新操作的进度。)

Overwrite Mask
(覆盖掩码)
==============

The ``devlink-flash`` command allows optionally specifying a mask indicating
how the device should handle subsections of flash components when updating.
This mask indicates the set of sections which are allowed to be overwritten.
(``devlink-flash`` 命令可以可选地指定一个掩码，指示设备在更新时应如何处理
固件组件的各个子部分。该掩码指示允许被覆盖的部分的集合。)

.. list-table:: List of overwrite mask bits
   :widths: 5 95

   * - Name (名称)
     - Description (描述)
   * - ``DEVLINK_FLASH_OVERWRITE_SETTINGS``
     - Indicates that the device should overwrite settings in the components
       being updated with the settings found in the provided image.
       (指示设备应当用所提供镜像中的设置来覆盖被更新组件中的设置。)
   * - ``DEVLINK_FLASH_OVERWRITE_IDENTIFIERS``
     - Indicates that the device should overwrite identifiers in the
       components being updated with the identifiers found in the provided
       image. This includes MAC addresses, serial IDs, and similar device
       identifiers.
       (指示设备应当用所提供镜像中的标识符来覆盖被更新组件中的标识符。
       这包括 MAC 地址、序列号 ID 以及类似的设备标识符。)

Multiple overwrite bits may be combined and requested together. If no bits
are provided, it is expected that the device only update firmware binaries
in the components being updated. Settings and identifiers are expected to be
preserved across the update. A device may not support every combination and
the driver for such a device must reject any combination which cannot be
faithfully implemented.
(可以组合并同时请求多个覆盖位。如果未提供任何位，则期望设备只更新被更新
组件中的固件二进制文件。设置和标识符预期在整个更新过程中保持不变。设备
可能不支持所有组合，此类设备的驱动必须拒绝任何无法忠实实现的组合。)

Firmware Loading
(固件加载)
================

Devices which require firmware to operate usually store it in non-volatile
memory on the board, e.g. flash. Some devices store only basic firmware on
the board, and the driver loads the rest from disk during probing.
``devlink-info`` allows users to query firmware information (loaded
components and versions).
(需要固件才能运行的设备通常把它存储在板载非易失性存储器中，例如 flash。
有些设备在板上只存储基本固件，其余部分由驱动在探测期间从磁盘加载。
``devlink-info`` 允许用户查询固件信息 (已加载组件和版本)。)

In other cases the device can both store the image on the board, load from
disk, or automatically flash a new image from disk. The ``fw_load_policy``
devlink parameter can be used to control this behavior
(:ref:`Documentation/networking/devlink/devlink-params.rst <devlink_params_generic>`).
(在其他情况下，设备既可以把镜像存储在板上、从磁盘加载，也可以自动从磁盘
刷写新镜像。``fw_load_policy`` devlink 参数可用于控制此行为
(:ref:`Documentation/networking/devlink/devlink-params.rst <devlink_params_generic>`)。)

On-disk firmware files are usually stored in ``/lib/firmware/``.
(磁盘上的固件文件通常存储在 ``/lib/firmware/`` 中。)

Firmware Version Management
(固件版本管理)
===========================

Drivers are expected to implement ``devlink-flash`` and ``devlink-info``
functionality, which together allow for implementing vendor-independent
automated firmware update facilities.
(驱动应实现 ``devlink-flash`` 和 ``devlink-info`` 功能，二者结合可以实现
独立于厂商的自动化固件更新设施。)

``devlink-info`` exposes the ``driver`` name and three version groups
(``fixed``, ``running``, ``stored``).
(``devlink-info`` 公开 ``driver`` 名称和三个版本组 (``fixed``、
``running``、``stored``)。)

The ``driver`` attribute and ``fixed`` group identify the specific device
design, e.g. for looking up applicable firmware updates. This is why
``serial_number`` is not part of the ``fixed`` versions (even though it
is fixed) - ``fixed`` versions should identify the design, not a single
device.
(``driver`` 属性和 ``fixed`` 组标识具体的设备设计，例如用于查找适用的
固件更新。这就是为什么 ``serial_number`` 不属于 ``fixed`` 版本 (尽管它是
固定的) —— ``fixed`` 版本应当标识设计，而不是单个设备。)

``running`` and ``stored`` firmware versions identify the firmware running
on the device, and firmware which will be activated after reboot or device
reset.
(``running`` 和 ``stored`` 固件版本分别标识设备上正在运行的固件，以及
将在重启或设备复位后激活的固件。)

The firmware update agent is supposed to be able to follow this simple
algorithm to update firmware contents, regardless of the device vendor:
(固件更新代理应当能够遵循以下简单算法来更新固件内容，而无需关心设备厂商：)

.. code-block:: sh

  # Get unique HW design identifier
  $hw_id = devlink-dev-info['fixed']

  # Find out which FW flash we want to use for this NIC
  $want_flash_vers = some-db-backed.lookup($hw_id, 'flash')

  # Update flash if necessary
  if $want_flash_vers != devlink-dev-info['stored']:
      $file = some-db-backed.download($hw_id, 'flash')
      devlink-dev-flash($file)

  # Find out the expected overall firmware versions
  $want_fw_vers = some-db-backed.lookup($hw_id, 'all')

  # Update on-disk file if necessary
  if $want_fw_vers != devlink-dev-info['running']:
      $file = some-db-backed.download($hw_id, 'disk')
      write($file, '/lib/firmware/')

  # Try device reset, if available
  if $want_fw_vers != devlink-dev-info['running']:
     devlink-reset()

  # Reboot, if reset wasn't enough
  if $want_fw_vers != devlink-dev-info['running']:
     reboot()

Note that each reference to ``devlink-dev-info`` in this pseudo-code
is expected to fetch up-to-date information from the kernel.
(注意，此伪代码中对 ``devlink-dev-info`` 的每次引用都应从内核获取最新的
信息。)

For the convenience of identifying firmware files some vendors add
``bundle_id`` information to the firmware versions. This meta-version covers
multiple per-component versions and can be used e.g. in firmware file names
(all component versions could get rather long.)
(为了便于识别固件文件，一些厂商在固件版本中添加了 ``bundle_id`` 信息。
这个元版本涵盖多个组件版本，例如可用于固件文件名 (所有组件版本可能相当
长)。)
