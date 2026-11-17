.. SPDX-License-Identifier: GPL-2.0 OR Linux-OpenIB
.. include:: <isonum.txt>

================================================================
Enabling the driver and kconfig options(启用驱动与 kconfig 选项)
================================================================

:Copyright: |copy| 2023, NVIDIA CORPORATION & AFFILIATES. All rights reserved.

(版权所有： |copy| 2023, NVIDIA CORPORATION & AFFILIATES。保留所有权利。)

| mlx5 core is modular and most of the major mlx5 core driver features can be selected (compiled in/out)
| at build time via kernel Kconfig flags.
| Basic features, ethernet net device rx/tx offloads and XDP, are available with the most basic flags
| CONFIG_MLX5_CORE=y/m and CONFIG_MLX5_CORE_EN=y.
| For the list of advanced features, please see below.
| (mlx5 core 是模块化的，大部分主要的 mlx5 core 驱动特性都可以在构建时通过内核 Kconfig 标志选择（编入/不编入）。
| 最基本的标志 CONFIG_MLX5_CORE=y/m 和 CONFIG_MLX5_CORE_EN=y 即可提供基本特性、以太网网络设备的 rx/tx 卸载以及 XDP。
| 高级特性列表请参见下文。)

**CONFIG_MLX5_BRIDGE=(y/n)** (以太网桥接卸载)

|    Enable :ref:`Ethernet Bridging (BRIDGE) offloading support <mlx5_bridge_offload>`.
|    This will provide the ability to add representors of mlx5 uplink and VF
|    ports to Bridge and offloading rules for traffic between such ports.
|    Supports VLANs (trunk and access modes).
|    (启用以太网桥接 (BRIDGE) 卸载支持。
|    这将提供把 mlx5 上联口 (uplink) 和 VF 端口的代表 (representor) 添加到网桥、并为这些端口之间的流量卸载规则的能力。
|    支持 VLAN（trunk 和 access 模式）。)


**CONFIG_MLX5_CORE=(y/m/n)** (module mlx5_core.ko) (mlx5 核心驱动)

|    The driver can be enabled by choosing CONFIG_MLX5_CORE=y/m in kernel config.
|    This will provide mlx5 core driver for mlx5 ulps to interface with (mlx5e, mlx5_ib).
|    (在内核配置中选择 CONFIG_MLX5_CORE=y/m 即可启用该驱动。
|    这将为各个 mlx5 ulp（mlx5e、mlx5_ib）提供可对接的 mlx5 core 驱动。)


**CONFIG_MLX5_CORE_EN=(y/n)** (基本以太网支持)

|    Choosing this option will allow basic ethernet netdevice support with all of the standard rx/tx offloads.
|    mlx5e is the mlx5 ulp driver which provides netdevice kernel interface, when chosen, mlx5e will be
|    built-in into mlx5_core.ko.
|    (选择此选项将允许带有所有标准 rx/tx 卸载的基本以太网 netdevice 支持。
|    mlx5e 是提供 netdevice 内核接口的 mlx5 ulp 驱动；选择此项后，mlx5e 将内建于 mlx5_core.ko 中。)


**CONFIG_MLX5_CORE_EN_DCB=(y/n)**: (数据中心桥接支持)

|    Enables `Data Center Bridging (DCB) Support <https://enterprise-support.nvidia.com/s/article/howto-auto-config-pfc-and-ets-on-connectx-4-via-lldp-dcbx>`_.
|    (启用数据中心桥接 (DCB) 支持。)


**CONFIG_MLX5_CORE_IPOIB=(y/n)** (IPoIB 卸载与加速)

|    IPoIB offloads & acceleration support.
|    Requires CONFIG_MLX5_CORE_EN to provide an accelerated interface for the rdma
|    IPoIB ulp netdevice.
|    (IPoIB 卸载与加速支持。
|    需要 CONFIG_MLX5_CORE_EN 来为 RDMA IPoIB ulp netdevice 提供加速接口。)


**CONFIG_MLX5_CLS_ACT=(y/n)** (TC 分类器动作卸载)

|    Enables offload support for TC classifier action (NET_CLS_ACT).
|    Works in both native NIC mode and Switchdev SRIOV mode.
|    Flow-based classifiers, such as those registered through
|    `tc-flower(8)`, are processed by the device, rather than the
|    host. Actions that would then overwrite matching classification
|    results would then be instant due to the offload.
|    (启用 TC 分类器动作 (NET_CLS_ACT) 的卸载支持。
|    在原生 NIC 模式和 Switchdev SRIOV 模式下均可工作。
|    基于流的分类器（例如通过 tc-flower(8) 注册的分类器）由设备而不是主机处理。由于卸载的存在，随后覆盖匹配分类结果的动作将即时生效。)


**CONFIG_MLX5_EN_ARFS=(y/n)** (硬件加速接收流导向)

|    Enables Hardware-accelerated receive flow steering (arfs) support, and ntuple filtering.
|    https://enterprise-support.nvidia.com/s/article/howto-configure-arfs-on-connectx-4
|    (启用硬件加速接收流导向 (arfs) 支持以及 ntuple 过滤。)


**CONFIG_MLX5_EN_IPSEC=(y/n)** (IPSec 卸载)

|    Enables :ref:`IPSec XFRM cryptography-offload acceleration <xfrm_device>`.
|    (启用 IPSec XFRM 加密卸载加速。)


**CONFIG_MLX5_MACSEC=(y/n)** (MACsec 卸载)

|    Build support for MACsec cryptography-offload acceleration in the NIC.
|    (在网卡中构建 MACsec 加密卸载加速支持。)


**CONFIG_MLX5_EN_RXNFC=(y/n)** (ethtool 接收流分类)

|    Enables ethtool receive network flow classification, which allows user defined
|    flow rules to direct traffic into arbitrary rx queue via ethtool set/get_rxnfc API.
|    (启用 ethtool 接收网络流分类，允许用户定义的流规则通过 ethtool set/get_rxnfc API 把流量引导到任意 rx 队列。)


**CONFIG_MLX5_EN_TLS=(y/n)** (TLS 卸载)

|    TLS cryptography-offload acceleration.
|    (TLS 加密卸载加速。)


**CONFIG_MLX5_ESWITCH=(y/n)** (以太网 SRIOV E-Switch)

|    Ethernet SRIOV E-Switch support in ConnectX NIC. E-Switch provides internal SRIOV packet steering
|    and switching for the enabled VFs and PF in two available modes:
|           1) `Legacy SRIOV mode (L2 mac vlan steering based) <https://enterprise-support.nvidia.com/s/article/HowTo-Configure-SR-IOV-for-ConnectX-4-ConnectX-5-ConnectX-6-with-KVM-Ethernet>`_.
|           2) :ref:`Switchdev mode (eswitch offloads) <switchdev>`.
|    (ConnectX 网卡中的以太网 SRIOV E-Switch 支持。E-Switch 以两种可用模式为已启用的 VF 和 PF 提供内部 SRIOV 数据包导向与交换：
|    1) 传统 SRIOV 模式（基于 L2 mac vlan 导向）；
|    2) Switchdev 模式（eswitch 卸载）。)


**CONFIG_MLX5_FPGA=(y/n)** (Innova FPGA 支持)

|    Build support for the Innova family of network cards by Mellanox Technologies.
|    Innova network cards are comprised of a ConnectX chip and an FPGA chip on one board.
|    If you select this option, the mlx5_core driver will include the Innova FPGA core and allow
|    building sandbox-specific client drivers.
|    (为 Mellanox Technologies 的 Innova 系列网卡提供构建支持。
|    Innova 网卡在同一块板上由一颗 ConnectX 芯片和一颗 FPGA 芯片组成。
|    如果选择此选项，mlx5_core 驱动将包含 Innova FPGA 核心，并允许构建沙箱特定的客户端驱动。)


**CONFIG_MLX5_INFINIBAND=(y/n/m)** (module mlx5_ib.ko) (InfiniBand/RDMA 支持)

|    Provides low-level InfiniBand/RDMA and `RoCE <https://enterprise-support.nvidia.com/s/article/recommended-network-configuration-examples-for-roce-deployment>`_ support.
|    (提供底层 InfiniBand/RDMA 和 RoCE 支持。)


**CONFIG_MLX5_MPFS=(y/n)** (多物理功能交换)

|    Ethernet Multi-Physical Function Switch (MPFS) support in ConnectX NIC.
|    MPFs is required for when `Multi-Host <https://www.nvidia.com/en-us/networking/multi-host/>`_ configuration is enabled to allow passing
|    user configured unicast MAC addresses to the requesting PF.
|    (ConnectX 网卡中的以太网多物理功能交换 (MPFS) 支持。
|    启用 Multi-Host 配置时需要 MPFS，以便把用户配置的单播 MAC 地址传递给发起请求的 PF。)


**CONFIG_MLX5_SF=(y/n)** (子功能)

|    Build support for subfunction.
|    Subfunctions are more light weight than PCI SRIOV VFs. Choosing this option
|    will enable support for creating subfunction devices.
|    (为子功能 (subfunction) 提供构建支持。
|    子功能比 PCI SRIOV VF 更轻量。选择此选项将启用创建子功能设备的支持。)


**CONFIG_MLX5_SF_MANAGER=(y/n)** (子功能端口管理)

|    Build support for subfunction port in the NIC. A Mellanox subfunction
|    port is managed through devlink.  A subfunction supports RDMA, netdevice
|    and vdpa device. It is similar to a SRIOV VF but it doesn't require
|    SRIOV support.
|    (为网卡中的子功能端口提供构建支持。Mellanox 子功能端口通过 devlink 管理。子功能支持 RDMA、netdevice 和 vdpa 设备。它类似于 SRIOV VF，但不要求 SRIOV 支持。)


**CONFIG_MLX5_SW_STEERING=(y/n)** (软件管理的导流)

|    Build support for software-managed steering in the NIC.
|    (为网卡中软件管理的导流 (steering) 提供构建支持。)

**CONFIG_MLX5_HW_STEERING=(y/n)** (硬件管理的导流)

|    Build support for hardware-managed steering in the NIC.
|    (为网卡中硬件管理的导流提供构建支持。)

**CONFIG_MLX5_TC_CT=(y/n)** (TC 连接跟踪卸载)

|    Support offloading connection tracking rules via tc ct action.
|    (支持通过 tc ct 动作卸载连接跟踪规则。)


**CONFIG_MLX5_TC_SAMPLE=(y/n)** (TC 采样卸载)

|    Support offloading sample rules via tc sample action.
|    (支持通过 tc sample 动作卸载采样规则。)


**CONFIG_MLX5_VDPA=(y/n)** (VDPA 支持库)

|    Support library for Mellanox VDPA drivers. Provides code that is
|    common for all types of VDPA drivers. The following drivers are planned:
|    net, block.
|    (Mellanox VDPA 驱动的支持库。提供对所有类型 VDPA 驱动通用的代码。计划中的驱动有：net、block。)


**CONFIG_MLX5_VDPA_NET=(y/n)** (VDPA 网络驱动)

|    VDPA network driver for ConnectX6 and newer. Provides offloading
|    of virtio net datapath such that descriptors put on the ring will
|    be executed by the hardware. It also supports a variety of stateless
|    offloads depending on the actual device used and firmware version.
|    (面向 ConnectX6 及更新设备的 VDPA 网络驱动。提供 virtio net 数据路径的卸载，使放到环上的描述符由硬件执行。它还根据实际使用的设备和固件版本，支持多种无状态卸载。)


**CONFIG_MLX5_VFIO_PCI=(y/n)** (VFIO 迁移支持)

|    This provides migration support for MLX5 devices using the VFIO framework.
|    (这通过 VFIO 框架为 MLX5 设备提供迁移支持。)


**External options** ( Choose if the corresponding mlx5 feature is required )
(外部选项——如果需要相应的 mlx5 特性则选择)

- CONFIG_MLXFW: When chosen, mlx5 firmware flashing support will be enabled (via devlink and ethtool).
  (CONFIG_MLXFW：选择后，将启用 mlx5 固件烧写支持（通过 devlink 和 ethtool）。)
- CONFIG_PTP_1588_CLOCK: When chosen, mlx5 ptp support will be enabled
  (CONFIG_PTP_1588_CLOCK：选择后，将启用 mlx5 的 ptp 支持)
- CONFIG_VXLAN: When chosen, mlx5 vxlan support will be enabled.
  (CONFIG_VXLAN：选择后，将启用 mlx5 的 vxlan 支持。)
