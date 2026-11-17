.. SPDX-License-Identifier: (GPL-2.0-only OR BSD-2-Clause)
.. include:: <isonum.txt>

===========================================
Network Flow Processor (NFP) Kernel Drivers
===========================================

(网络流处理器 (NFP) 内核驱动程序)

:Copyright: |copy| 2019, Netronome Systems, Inc.
  (版权：|copy| 2019, Netronome Systems, Inc.)
:Copyright: |copy| 2022, Corigine, Inc.
  (版权：|copy| 2022, Corigine, Inc.)

Contents
(目录)
========

- `Overview`_
  (总览)
- `Acquiring Firmware`_
  (获取固件)
- `Devlink Info`_
  (Devlink 信息)
- `Configure Device`_
  (配置设备)
- `Statistics`_
  (统计)

Overview
(总览)
========

This driver supports Netronome and Corigine's line of Network Flow Processor
devices, including the NFP3800, NFP4000, NFP5000, and NFP6000 models, which
are also incorporated in the companies' family of Agilio SmartNICs. The SR-IOV
physical and virtual functions for these devices are supported by the driver.
(此驱动程序支持 Netronome 和 Corigine 的网络流处理器 (Network Flow Processor) 设备系列，包括 NFP3800、NFP4000、NFP5000 和 NFP6000 型号，这些型号也集成在两家公司的 Agilio SmartNIC 产品家族中。该驱动程序支持这些设备的 SR-IOV 物理功能和虚拟功能。)

Acquiring Firmware
(获取固件)
==================

The NFP3800, NFP4000 and NFP6000 devices require application specific firmware
to function. Application firmware can be located either on the host file system
or in the device flash (if supported by management firmware).
(NFP3800、NFP4000 和 NFP6000 设备需要应用特定的固件才能工作。应用固件既可以位于主机文件系统中，也可以位于设备闪存中 (如果管理固件支持)。)

Firmware files on the host filesystem contain card type (`AMDA-*` string), media
config etc. They should be placed in `/lib/firmware/netronome` directory to
load firmware from the host file system.
(主机文件系统上的固件文件包含卡类型 (`AMDA-*` 字符串)、介质配置等信息。要从主机文件系统加载固件，应把它们放置在 `/lib/firmware/netronome` 目录下。)

Firmware for basic NIC operation is available in the upstream
`linux-firmware.git` repository.
(基本 NIC 操作所需的固件可在上游 `linux-firmware.git` 仓库中获取。)

A more comprehensive list of firmware can be downloaded from the
`Corigine Support site <https://www.corigine.com/DPUDownload.html>`_.
(更全面的固件列表可从 Corigine 支持站点下载。)

Firmware in NVRAM
(NVRAM 中的固件)
-----------------

Recent versions of management firmware supports loading application
firmware from flash when the host driver gets probed. The firmware loading
policy configuration may be used to configure this feature appropriately.
(较新版本的管理固件支持在主机驱动程序被探测时从闪存加载应用固件。可以使用固件加载策略配置来适当地配置此特性。)

Devlink or ethtool can be used to update the application firmware on the device
flash by providing the appropriate `nic_AMDA*.nffw` file to the respective
command. Users need to take care to write the correct firmware image for the
card and media configuration to flash.
(可以使用 devlink 或 ethtool 更新设备闪存上的应用固件，方法是向相应命令提供适当的 `nic_AMDA*.nffw` 文件。用户需注意为闪存写入与卡和介质配置相符的正确固件镜像。)

Available storage space in flash depends on the card being used.
(闪存的可用存储空间取决于所使用的卡。)

Dealing with multiple projects
(处理多个项目)
------------------------------

NFP hardware is fully programmable therefore there can be different
firmware images targeting different applications.
(NFP 硬件是完全可编程的，因此可以存在针对不同应用的不同固件镜像。)

When using application firmware from host, we recommend placing
actual firmware files in application-named subdirectories in
`/lib/firmware/netronome` and linking the desired files, e.g.::
(从主机使用应用固件时，我们建议把实际固件文件放在 `/lib/firmware/netronome` 中以应用命名的子目录里，并链接所需的文件，例如：)

    $ tree /lib/firmware/netronome/
    /lib/firmware/netronome/
    ├── bpf
    │   ├── nic_AMDA0081-0001_1x40.nffw
    │   └── nic_AMDA0081-0001_4x10.nffw
    ├── flower
    │   ├── nic_AMDA0081-0001_1x40.nffw
    │   └── nic_AMDA0081-0001_4x10.nffw
    ├── nic
    │   ├── nic_AMDA0081-0001_1x40.nffw
    │   └── nic_AMDA0081-0001_4x10.nffw
    ├── nic_AMDA0081-0001_1x40.nffw -> bpf/nic_AMDA0081-0001_1x40.nffw
    └── nic_AMDA0081-0001_4x10.nffw -> bpf/nic_AMDA0081-0001_4x10.nffw

    3 directories, 8 files

You may need to use hard instead of symbolic links on distributions
which use old `mkinitrd` command instead of `dracut` (e.g. Ubuntu).
(在使用旧式 `mkinitrd` 命令而非 `dracut` 的发行版 (例如 Ubuntu) 上，可能需要使用硬链接而不是符号链接。)

After changing firmware files you may need to regenerate the initramfs
image. Initramfs contains drivers and firmware files your system may
need to boot. Refer to the documentation of your distribution to find
out how to update initramfs. Good indication of stale initramfs
is system loading wrong driver or firmware on boot, but when driver is
later reloaded manually everything works correctly.
(更改固件文件后，可能需要重新生成 initramfs 镜像。initramfs 包含系统启动所需的驱动程序和固件文件。请参阅所用发行版的文档，了解如何更新 initramfs。initramfs 过期的一个明显迹象是系统在启动时加载了错误的驱动程序或固件，但之后手动重新加载驱动程序时一切工作正常。)

Selecting firmware per device
(按设备选择固件)
-----------------------------

Most commonly all cards on the system use the same type of firmware.
If you want to load a specific firmware image for a specific card, you
can use either the PCI bus address or serial number. The driver will
print which files it's looking for when it recognizes a NFP device::
(通常，系统中的所有卡都使用相同类型的固件。如果想为特定的卡加载特定的固件镜像，可以使用 PCI 总线地址或序列号。当识别到 NFP 设备时，驱动程序会打印它正在按优先级查找哪些文件：)

    nfp: Looking for firmware file in order of priority:
    nfp:  netronome/serial-00-12-34-aa-bb-cc-10-ff.nffw: not found
    nfp:  netronome/pci-0000:02:00.0.nffw: not found
    nfp:  netronome/nic_AMDA0081-0001_1x40.nffw: found, loading...

In this case if file (or link) called *serial-00-12-34-aa-bb-5d-10-ff.nffw*
or *pci-0000:02:00.0.nffw* is present in `/lib/firmware/netronome` this
firmware file will take precedence over `nic_AMDA*` files.
(在这种情况下，如果 `/lib/firmware/netronome` 中存在名为 *serial-00-12-34-aa-bb-5d-10-ff.nffw* 或 *pci-0000:02:00.0.nffw* 的文件 (或链接)，则该固件文件将优先于 `nic_AMDA*` 文件。)

Note that `serial-*` and `pci-*` files are **not** automatically included
in initramfs, you will have to refer to documentation of appropriate tools
to find out how to include them.
(注意，`serial-*` 和 `pci-*` 文件不会被自动包含到 initramfs 中，你需要参阅相应工具的文档，了解如何包含它们。)

Running firmware version
(运行中的固件版本)
------------------------

The version of the loaded firmware for a particular <netdev> interface,
(e.g. enp4s0), or an interface's port <netdev port> (e.g. enp4s0np0) can
be displayed with the ethtool command::
(特定 <netdev> 接口 (例如 enp4s0) 或接口的端口 <netdev port> (例如 enp4s0np0) 所加载固件的版本，可以用 ethtool 命令显示：)

  $ ethtool -i <netdev>

Firmware loading policy
(固件加载策略)
-----------------------

Firmware loading policy is controlled via three HWinfo parameters
stored as key value pairs in the device flash:
(固件加载策略由三个以键值对形式存储在设备闪存中的 HWinfo 参数控制：)

app_fw_from_flash
(来自闪存的应用固件)

    Defines which firmware should take precedence, 'Disk' (0), 'Flash' (1) or
    the 'Preferred' (2) firmware. When 'Preferred' is selected, the management
    firmware makes the decision over which firmware will be loaded by comparing
    versions of the flash firmware and the host supplied firmware.
    This variable is configurable using the 'fw_load_policy'
    devlink parameter.
    (定义哪个固件应具有优先权：'Disk' (0)、'Flash' (1) 或 'Preferred' (2) 固件。选择 'Preferred' 时，管理固件会通过比较闪存固件与主机提供的固件的版本来决定加载哪个固件。此变量可通过 'fw_load_policy' devlink 参数进行配置。)

abi_drv_reset
(驱动探测时的固件复位策略)

    Defines if the driver should reset the firmware when
    the driver is probed, either 'Disk' (0) if firmware was found on disk,
    'Always' (1) reset or 'Never' (2) reset. Note that the device is always
    reset on driver unload if firmware was loaded when the driver was probed.
    This variable is configurable using the 'reset_dev_on_drv_probe'
    devlink parameter.
    (定义驱动程序被探测时是否应复位固件：如果固件在磁盘上找到则为 'Disk' (0)、'Always' (1) 总是复位，或 'Never' (2) 从不复位。注意，如果在驱动程序被探测时加载了固件，则在卸载驱动程序时设备总是会被复位。此变量可通过 'reset_dev_on_drv_probe' devlink 参数进行配置。)

abi_drv_load_ifc
(允许加载固件的接口列表)

    Defines a list of PF devices allowed to load FW on the device.
    This variable is not currently user configurable.
    (定义允许在该设备上加载 FW 的 PF 设备列表。此变量目前不可由用户配置。)

Devlink Info
(Devlink 信息)
============

The devlink info command displays the running and stored firmware versions
on the device, serial number and board information.
(devlink info 命令显示设备上正在运行和已存储的固件版本、序列号和板卡信息。)

Devlink info command example (replace PCI address)::
(devlink info 命令示例 (替换其中的 PCI 地址)：)

  $ devlink dev info pci/0000:03:00.0
    pci/0000:03:00.0:
      driver nfp
      serial_number CSAAMDA2001-1003000111
      versions:
          fixed:
            board.id AMDA2001-1003
            board.rev 01
            board.manufacture CSA
            board.model mozart
          running:
            fw.mgmt 22.10.0-rc3
            fw.cpld 0x1000003
            fw.app nic-22.09.0
            chip.init AMDA-2001-1003  1003000111
          stored:
            fw.bundle_id bspbundle_1003000111
            fw.mgmt 22.10.0-rc3
            fw.cpld 0x0
            chip.init AMDA-2001-1003  1003000111

Configure Device
(配置设备)
================

This section explains how to use Agilio SmartNICs running basic NIC firmware.
(本节介绍如何使用运行基本 NIC 固件的 Agilio SmartNIC。)

Configure interface link-speed
(配置接口链路速度)
------------------------------

The following steps explains how to change between 10G mode and 25G mode on
Agilio CX 2x25GbE cards. The changing of port speed must be done in order,
port 0 (p0) must be set to 10G before port 1 (p1) may be set to 10G.
(以下步骤说明如何在 Agilio CX 2x25GbE 卡上于 10G 模式与 25G 模式之间切换。端口速度的更改必须按顺序进行：必须先把端口 0 (p0) 设为 10G，然后才能把端口 1 (p1) 设为 10G。)

Down the respective interface(s)::
(把相应的接口置于 down 状态：)

  $ ip link set dev <netdev port 0> down
  $ ip link set dev <netdev port 1> down

Set interface link-speed to 10G::
(把接口链路速度设为 10G：)

  $ ethtool -s <netdev port 0> speed 10000
  $ ethtool -s <netdev port 1> speed 10000

Set interface link-speed to 25G::
(把接口链路速度设为 25G：)

  $ ethtool -s <netdev port 0> speed 25000
  $ ethtool -s <netdev port 1> speed 25000

Reload driver for changes to take effect::
(重新加载驱动程序以使更改生效：)

  $ rmmod nfp; modprobe nfp

Configure interface Maximum Transmission Unit (MTU)
(配置接口最大传输单元 (MTU))
---------------------------------------------------

The MTU of interfaces can temporarily be set using the iproute2, ip link or
ifconfig tools. Note that this change will not persist. Setting this via
Network Manager, or another appropriate OS configuration tool, is
recommended as changes to the MTU using Network Manager can be made to
persist.
(可以使用 iproute2 的 ip link 或 ifconfig 工具临时设置接口的 MTU。注意此更改不会持久保存。建议通过 Network Manager 或其他适当的操作系统配置工具来设置，因为通过 Network Manager 所做的 MTU 更改可以持久保存。)

Set interface MTU to 9000 bytes::
(把接口 MTU 设为 9000 字节：)

  $ ip link set dev <netdev port> mtu 9000

It is the responsibility of the user or the orchestration layer to set
appropriate MTU values when handling jumbo frames or utilizing tunnels. For
example, if packets sent from a VM are to be encapsulated on the card and
egress a physical port, then the MTU of the VF should be set to lower than
that of the physical port to account for the extra bytes added by the
additional header. If a setup is expected to see fallback traffic between
the SmartNIC and the kernel then the user should also ensure that the PF MTU
is appropriately set to avoid unexpected drops on this path.
(在处理巨型帧或使用隧道时，由用户或编排层负责设置适当的 MTU 值。例如，如果从虚拟机 (VM) 发送的数据包要在卡上进行封装并从物理端口发出，那么 VF 的 MTU 应设得低于物理端口的 MTU，以顾及额外头部所增加的字节数。如果预期部署中 SmartNIC 与内核之间存在回退 (fallback) 流量，用户还应确保 PF 的 MTU 设置得当，以避免在此路径上出现意外的丢包。)

Configure Forward Error Correction (FEC) modes
(配置前向纠错 (FEC) 模式)
----------------------------------------------

Agilio SmartNICs support FEC mode configuration, e.g. Auto, Firecode Base-R,
ReedSolomon and Off modes. Each physical port's FEC mode can be set
independently using ethtool. The supported FEC modes for an interface can
be viewed using::
(Agilio SmartNIC 支持 FEC 模式配置，例如 Auto (自动)、Firecode Base-R、ReedSolomon 和 Off (关闭) 模式。每个物理端口的 FEC 模式都可以用 ethtool 独立设置。接口支持的 FEC 模式可使用以下命令查看：)

  $ ethtool <netdev>

The currently configured FEC mode can be viewed using::
(当前配置的 FEC 模式可使用以下命令查看：)

  $ ethtool --show-fec <netdev>

To force the FEC mode for a particular port, auto-negotiation must be disabled
(see the `Auto-negotiation`_ section). An example of how to set the FEC mode
to Reed-Solomon is::
(要为特定端口强制指定 FEC 模式，必须先禁用自协商 (参见 `Auto-negotiation`_ 一节)。下面是把 FEC 模式设为 Reed-Solomon 的示例：)

  $ ethtool --set-fec <netdev> encoding rs

Auto-negotiation
(自协商)
----------------

To change auto-negotiation settings, the link must first be put down. After the
link is down, auto-negotiation can be enabled or disabled using::
(要更改自协商设置，必须先把链路置于 down 状态。链路 down 之后，即可使用以下命令启用或禁用自协商：)

  ethtool -s <netdev> autoneg <on|off>

Statistics
(统计)
==========

Following device statistics are available through the ``ethtool -S`` interface:
(通过 ``ethtool -S`` 接口可以查看以下设备统计信息：)

.. flat-table:: NFP device statistics
   :header-rows: 1
   :widths: 3 1 11

   * - Name (名称)
     - ID (标识)
     - Meaning (含义)

   * - dev_rx_discards
     - 1
     - Packet can be discarded on the RX path for one of the following reasons:
       (数据包在 RX 路径上可能因以下原因之一被丢弃：)

        * The NIC is not in promisc mode, and the destination MAC address
          doesn't match the interfaces' MAC address.
          (网卡未处于混杂模式，且目的 MAC 地址与接口的 MAC 地址不匹配。)
        * The received packet is larger than the max buffer size on the host.
          I.e. it exceeds the Layer 3 MRU.
          (接收到的数据包大于主机上的最大缓冲区大小，即超过了第 3 层 MRU。)
        * There is no freelist descriptor available on the host for the packet.
          It is likely that the NIC couldn't cache one in time.
          (主机上没有可用于该数据包的空闲列表描述符，很可能是网卡未能及时缓存一个。)
        * A BPF program discarded the packet.
          (BPF 程序丢弃了该数据包。)
        * The datapath drop action was executed.
          (执行了数据路径的丢弃 (drop) 动作。)
        * The MAC discarded the packet due to lack of ingress buffer space
          on the NIC.
          (由于网卡上入口缓冲空间不足，MAC 丢弃了该数据包。)

   * - dev_rx_errors
     - 2
     - A packet can be counted (and dropped) as RX error for the following
       reasons:
       (数据包可能因以下原因被计为 RX 错误 (并丢弃)：)

       * A problem with the VEB lookup (only when SR-IOV is used).
         (VEB 查找出现问题 (仅在使用 SR-IOV 时)。)
       * A physical layer problem that causes Ethernet errors, like FCS or
         alignment errors. The cause is usually faulty cables or SFPs.
         (导致以太网错误 (如 FCS 错误或对齐错误) 的物理层问题。原因通常是电缆或 SFP 故障。)

   * - dev_rx_bytes
     - 3
     - Total number of bytes received. (接收的总字节数。)

   * - dev_rx_uc_bytes
     - 4
     - Unicast bytes received. (接收的单播字节数。)

   * - dev_rx_mc_bytes
     - 5
     - Multicast bytes received. (接收的组播字节数。)

   * - dev_rx_bc_bytes
     - 6
     - Broadcast bytes received. (接收的广播字节数。)

   * - dev_rx_pkts
     - 7
     - Total number of packets received. (接收的数据包总数。)

   * - dev_rx_mc_pkts
     - 8
     - Multicast packets received. (接收的组播数据包数。)

   * - dev_rx_bc_pkts
     - 9
     - Broadcast packets received. (接收的广播数据包数。)

   * - dev_tx_discards
     - 10
     - A packet can be discarded in the TX direction if the MAC is
       being flow controlled and the NIC runs out of TX queue space.
       (如果 MAC 正处于流控状态且网卡的 TX 队列空间耗尽，数据包可能会在 TX 方向上被丢弃。)

   * - dev_tx_errors
     - 11
     - A packet can be counted as TX error (and dropped) for one for the
       following reasons:
       (数据包可能因以下原因之一被计为 TX 错误 (并丢弃)：)

       * The packet is an LSO segment, but the Layer 3 or Layer 4 offset
         could not be determined. Therefore LSO could not continue.
         (该数据包是一个 LSO 分段，但无法确定第 3 层或第 4 层偏移，因此 LSO 无法继续。)
       * An invalid packet descriptor was received over PCIe.
         (通过 PCIe 收到了无效的数据包描述符。)
       * The packet Layer 3 length exceeds the device MTU.
         (数据包的第 3 层长度超过了设备 MTU。)
       * An error on the MAC/physical layer. Usually due to faulty cables or
         SFPs.
         (MAC/物理层出现错误。通常由电缆或 SFP 故障引起。)
       * A CTM buffer could not be allocated.
         (无法分配 CTM 缓冲区。)
       * The packet offset was incorrect and could not be fixed by the NIC.
         (数据包偏移不正确，且网卡无法修正。)

   * - dev_tx_bytes
     - 12
     - Total number of bytes transmitted. (发送的总字节数。)

   * - dev_tx_uc_bytes
     - 13
     - Unicast bytes transmitted. (发送的单播字节数。)

   * - dev_tx_mc_bytes
     - 14
     - Multicast bytes transmitted. (发送的组播字节数。)

   * - dev_tx_bc_bytes
     - 15
     - Broadcast bytes transmitted. (发送的广播字节数。)

   * - dev_tx_pkts
     - 16
     - Total number of packets transmitted. (发送的数据包总数。)

   * - dev_tx_mc_pkts
     - 17
     - Multicast packets transmitted. (发送的组播数据包数。)

   * - dev_tx_bc_pkts
     - 18
     - Broadcast packets transmitted. (发送的广播数据包数。)

Note that statistics unknown to the driver will be displayed as
``dev_unknown_stat$ID``, where ``$ID`` refers to the second column
above.
(注意，驱动程序未知的统计信息将显示为 ``dev_unknown_stat$ID`` ，其中 ``$ID`` 指上表的第二列。)
