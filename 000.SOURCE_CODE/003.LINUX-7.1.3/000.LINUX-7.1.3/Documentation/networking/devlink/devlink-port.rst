.. SPDX-License-Identifier: GPL-2.0

.. _devlink_port:

============
Devlink Port
============

(Devlink 端口)

``devlink-port`` is a port that exists on the device. It has a logically
separate ingress/egress point of the device. A devlink port can be any one
of many flavours. A devlink port flavour along with port attributes
describe what a port represents.
(``devlink-port`` 是设备上存在的一个端口。它拥有设备上逻辑上独立的入口/出口
点。devlink 端口可以是多种类型 (flavour) 中的任意一种。devlink 端口的类型
连同端口属性一起描述了该端口所代表的内容。)

A device driver that intends to publish a devlink port sets the
devlink port attributes and registers the devlink port.
(打算发布 devlink 端口的设备驱动程序会设置 devlink 端口属性并注册该
devlink 端口。)

Devlink port flavours are described below.
(devlink 端口类型在下面描述。)

.. list-table:: List of devlink port flavours
   :widths: 33 90

   * - Flavour (类型)
     - Description (描述)
   * - ``DEVLINK_PORT_FLAVOUR_PHYSICAL``
     - Any kind of physical port. This can be an eswitch physical port or any
       other physical port on the device.
       (任何类型的物理端口。它可以是一个 eswitch 物理端口，也可以是设备上的
       任何其他物理端口。)
   * - ``DEVLINK_PORT_FLAVOUR_DSA``
     - This indicates a DSA interconnect port.
       (这表示一个 DSA 互连端口。)
   * - ``DEVLINK_PORT_FLAVOUR_CPU``
     - This indicates a CPU port applicable only to DSA.
       (这表示一个 CPU 端口，仅适用于 DSA。)
   * - ``DEVLINK_PORT_FLAVOUR_PCI_PF``
     - This indicates an eswitch port representing a port of PCI
       physical function (PF).
       (这表示一个 eswitch 端口，代表 PCI 物理功能 (PF) 的一个端口。)
   * - ``DEVLINK_PORT_FLAVOUR_PCI_VF``
     - This indicates an eswitch port representing a port of PCI
       virtual function (VF).
       (这表示一个 eswitch 端口，代表 PCI 虚拟功能 (VF) 的一个端口。)
   * - ``DEVLINK_PORT_FLAVOUR_PCI_SF``
     - This indicates an eswitch port representing a port of PCI
       subfunction (SF).
       (这表示一个 eswitch 端口，代表 PCI 子功能 (SF) 的一个端口。)
   * - ``DEVLINK_PORT_FLAVOUR_VIRTUAL``
     - This indicates a virtual port for the PCI virtual function.
       (这表示 PCI 虚拟功能的虚拟端口。)

Devlink port can have a different type based on the link layer described below.
(devlink 端口可以根据下文描述的链路层拥有不同的类型。)

.. list-table:: List of devlink port types
   :widths: 23 90

   * - Type (类型)
     - Description (描述)
   * - ``DEVLINK_PORT_TYPE_ETH``
     - Driver should set this port type when a link layer of the port is
       Ethernet.
       (当端口的链路层为以太网时，驱动程序应设置此端口类型。)
   * - ``DEVLINK_PORT_TYPE_IB``
     - Driver should set this port type when a link layer of the port is
       InfiniBand.
       (当端口的链路层为 InfiniBand 时，驱动程序应设置此端口类型。)
   * - ``DEVLINK_PORT_TYPE_AUTO``
     - This type is indicated by the user when driver should detect the port
       type automatically.
       (当驱动程序应自动检测端口类型时，由用户指定此类型。)

PCI controllers
(PCI 控制器)
---------------

In most cases a PCI device has only one controller. A controller consists of
potentially multiple physical, virtual functions and subfunctions. A function
consists of one or more ports. This port is represented by the devlink eswitch
port.
(在大多数情况下，一个 PCI 设备只有一个控制器。一个控制器可能由多个物理功能、
虚拟功能和子功能组成。一个功能由一个或多个端口组成。该端口由 devlink eswitch
端口表示。)

A PCI device connected to multiple CPUs or multiple PCI root complexes or a
SmartNIC, however, may have multiple controllers. For a device with multiple
controllers, each controller is distinguished by a unique controller number.
An eswitch is on the PCI device which supports ports of multiple controllers.
(然而，连接到多个 CPU、多个 PCI 根复合体或 SmartNIC 的 PCI 设备可能有多个
控制器。对于具有多个控制器的设备，每个控制器由唯一的控制器编号区分。
eswitch 位于支持多个控制器端口的 PCI 设备上。)

An example view of a system with two controllers::
(具有两个控制器的系统示例视图：)

                 ---------------------------------------------------------
                 |                                                       |
                 |           --------- ---------         ------- ------- |
    -----------  |           | vf(s) | | sf(s) |         |vf(s)| |sf(s)| |
    | server  |  | -------   ----/---- ---/----- ------- ---/--- ---/--- |
    | pci rc  |=== | pf0 |______/________/       | pf1 |___/_______/     |
    | connect |  | -------                       -------                 |
    -----------  |     | controller_num=1 (no eswitch)                   |
                 ------|--------------------------------------------------
                 (internal wire)
                       |
                 ---------------------------------------------------------
                 | devlink eswitch ports and reps                        |
                 | ----------------------------------------------------- |
                 | |ctrl-0 | ctrl-0 | ctrl-0 | ctrl-0 | ctrl-0 |ctrl-0 | |
                 | |pf0    | pf0vfN | pf0sfN | pf1    | pf1vfN |pf1sfN | |
                 | ----------------------------------------------------- |
                 | |ctrl-1 | ctrl-1 | ctrl-1 | ctrl-1 | ctrl-1 |ctrl-1 | |
                 | |pf0    | pf0vfN | pf0sfN | pf1    | pf1vfN |pf1sfN | |
                 | ----------------------------------------------------- |
                 |                                                       |
                 |                                                       |
    -----------  |           --------- ---------         ------- ------- |
    | smartNIC|  |           | vf(s) | | sf(s) |         |vf(s)| |sf(s)| |
    | pci rc  |==| -------   ----/---- ---/----- ------- ---/--- ---/--- |
    | connect |  | | pf0 |______/________/       | pf1 |___/_______/     |
    -----------  | -------                       -------                 |
                 |                                                       |
                 |  local controller_num=0 (eswitch)                     |
                 ---------------------------------------------------------

In the above example, the external controller (identified by controller number = 1)
doesn't have the eswitch. Local controller (identified by controller number = 0)
has the eswitch. The Devlink instance on the local controller has eswitch
devlink ports for both the controllers.
(在上面的例子中，外部控制器 (controller number = 1) 没有 eswitch。本地控制器
(controller number = 0) 拥有 eswitch。本地控制器上的 Devlink 实例为两个控制器
都持有 eswitch devlink 端口。)

Function configuration
(功能配置)
======================

Users can configure one or more function attributes before enumerating the PCI
function. Usually it means, user should configure function attribute
before a bus specific device for the function is created. However, when
SRIOV is enabled, virtual function devices are created on the PCI bus.
Hence, function attribute should be configured before binding virtual
function device to the driver. For subfunctions, this means user should
configure port function attribute before activating the port function.
(用户可以在枚举 PCI 功能之前配置一个或多个功能属性。通常这意味着，用户应该在
为该功能创建总线特定设备之前配置功能属性。但是，当启用 SRIOV 时，虚拟功能设备
会创建在 PCI 总线上。因此，应在将虚拟功能设备绑定到驱动程序之前配置功能属性。
对于子功能，这意味着用户应在激活端口功能之前配置端口功能属性。)

A user may set the hardware address of the function using
`devlink port function set hw_addr` command. For Ethernet port function
this means a MAC address.
(用户可以使用 `devlink port function set hw_addr` 命令设置功能的硬件地址。
对于以太网端口功能，这意味着一个 MAC 地址。)

Users may also set the RoCE capability of the function using
`devlink port function set roce` command.
(用户还可以使用 `devlink port function set roce` 命令设置功能的 RoCE
能力。)

Users may also set the function as migratable using
`devlink port function set migratable` command.
(用户还可以使用 `devlink port function set migratable` 命令将功能设置为
可迁移。)

Users may also set the IPsec crypto capability of the function using
`devlink port function set ipsec_crypto` command.
(用户还可以使用 `devlink port function set ipsec_crypto` 命令设置功能的
IPsec 加密能力。)

Users may also set the IPsec packet capability of the function using
`devlink port function set ipsec_packet` command.
(用户还可以使用 `devlink port function set ipsec_packet` 命令设置功能的
IPsec 数据包能力。)

Users may also set the maximum IO event queues of the function
using `devlink port function set max_io_eqs` command.
(用户还可以使用 `devlink port function set max_io_eqs` 命令设置功能的最大
IO 事件队列数。)

Function attributes
(功能属性)
===================

MAC address setup
(MAC 地址设置)
-----------------

The configured MAC address of the PCI VF/SF will be used by netdevice and rdma
device created for the PCI VF/SF.
(为 PCI VF/SF 创建的 netdevice 和 rdma 设备将使用已配置的 PCI VF/SF MAC
地址。)

- Get the MAC address of the VF identified by its unique devlink port index::
  (获取由其唯一 devlink 端口索引标识的 VF 的 MAC 地址：)

    $ devlink port show pci/0000:06:00.0/2
    pci/0000:06:00.0/2: type eth netdev enp6s0pf0vf1 flavour pcivf pfnum 0 vfnum 1
      function:
        hw_addr 00:00:00:00:00:00

- Set the MAC address of the VF identified by its unique devlink port index::
  (设置由其唯一 devlink 端口索引标识的 VF 的 MAC 地址：)

    $ devlink port function set pci/0000:06:00.0/2 hw_addr 00:11:22:33:44:55

    $ devlink port show pci/0000:06:00.0/2
    pci/0000:06:00.0/2: type eth netdev enp6s0pf0vf1 flavour pcivf pfnum 0 vfnum 1
      function:
        hw_addr 00:11:22:33:44:55

- Get the MAC address of the SF identified by its unique devlink port index::
  (获取由其唯一 devlink 端口索引标识的 SF 的 MAC 地址：)

    $ devlink port show pci/0000:06:00.0/32768
    pci/0000:06:00.0/32768: type eth netdev enp6s0pf0sf88 flavour pcisf pfnum 0 sfnum 88
      function:
        hw_addr 00:00:00:00:00:00

- Set the MAC address of the SF identified by its unique devlink port index::
  (设置由其唯一 devlink 端口索引标识的 SF 的 MAC 地址：)

    $ devlink port function set pci/0000:06:00.0/32768 hw_addr 00:00:00:00:88:88

    $ devlink port show pci/0000:06:00.0/32768
    pci/0000:06:00.0/32768: type eth netdev enp6s0pf0sf88 flavour pcisf pfnum 0 sfnum 88
      function:
        hw_addr 00:00:00:00:88:88

RoCE capability setup
(RoCE 能力设置)
---------------------

Not all PCI VFs/SFs require RoCE capability.
(并非所有 PCI VF/SF 都需要 RoCE 能力。)

When RoCE capability is disabled, it saves system memory per PCI VF/SF.
(禁用 RoCE 能力时，可为每个 PCI VF/SF 节省系统内存。)

When user disables RoCE capability for a VF/SF, user application cannot send or
receive any RoCE packets through this VF/SF and RoCE GID table for this PCI
will be empty.
(当用户为某个 VF/SF 禁用 RoCE 能力时，用户应用程序将无法通过该 VF/SF 发送或
接收任何 RoCE 数据包，并且该 PCI 的 RoCE GID 表将为空。)

When RoCE capability is disabled in the device using port function attribute,
VF/SF driver cannot override it.
(当使用端口功能属性在设备中禁用 RoCE 能力时，VF/SF 驱动程序无法覆盖它。)

- Get RoCE capability of the VF device::
  (获取 VF 设备的 RoCE 能力：)

    $ devlink port show pci/0000:06:00.0/2
    pci/0000:06:00.0/2: type eth netdev enp6s0pf0vf1 flavour pcivf pfnum 0 vfnum 1
        function:
            hw_addr 00:00:00:00:00:00 roce enable

- Set RoCE capability of the VF device::
  (设置 VF 设备的 RoCE 能力：)

    $ devlink port function set pci/0000:06:00.0/2 roce disable

    $ devlink port show pci/0000:06:00.0/2
    pci/0000:06:00.0/2: type eth netdev enp6s0pf0vf1 flavour pcivf pfnum 0 vfnum 1
        function:
            hw_addr 00:00:00:00:00:00 roce disable

migratable capability setup
(可迁移能力设置)
---------------------------

Live migration is the process of transferring a live virtual machine
from one physical host to another without disrupting its normal
operation.
(实时迁移 (live migration) 是在不中断正常运行的情况下，将一台运行中的虚拟机
从一个物理主机迁移到另一个物理主机的过程。)

User who want PCI VFs to be able to perform live migration need to
explicitly enable the VF migratable capability.
(希望 PCI VF 能够执行实时迁移的用户，需要显式启用 VF 的可迁移
(migratable) 能力。)

When user enables migratable capability for a VF, and the HV binds the VF to VFIO driver
with migration support, the user can migrate the VM with this VF from one HV to a
different one.
(当用户为 VF 启用可迁移能力，且宿主机 (HV) 将该 VF 绑定到支持迁移的 VFIO
驱动时，用户可以把带有该 VF 的虚拟机从一个 HV 迁移到另一个 HV。)

However, when migratable capability is enable, device will disable features which cannot
be migrated. Thus migratable cap can impose limitations on a VF so let the user decide.
(但是，当启用可迁移能力时，设备会禁用无法迁移的特性。因此，可迁移能力可能会
对 VF 施加限制，故由用户自行决定。)

Example of LM with migratable function configuration:
(使用可迁移功能配置进行实时迁移 (LM) 的示例：)

- Get migratable capability of the VF device::
  (获取 VF 设备的可迁移能力：)

    $ devlink port show pci/0000:06:00.0/2
    pci/0000:06:00.0/2: type eth netdev enp6s0pf0vf1 flavour pcivf pfnum 0 vfnum 1
        function:
            hw_addr 00:00:00:00:00:00 migratable disable

- Set migratable capability of the VF device::
  (设置 VF 设备的可迁移能力：)

    $ devlink port function set pci/0000:06:00.0/2 migratable enable

    $ devlink port show pci/0000:06:00.0/2
    pci/0000:06:00.0/2: type eth netdev enp6s0pf0vf1 flavour pcivf pfnum 0 vfnum 1
        function:
            hw_addr 00:00:00:00:00:00 migratable enable

- Bind VF to VFIO driver with migration support::
  (将 VF 绑定到支持迁移的 VFIO 驱动：)

    $ echo <pci_id> > /sys/bus/pci/devices/0000:08:00.0/driver/unbind
    $ echo mlx5_vfio_pci > /sys/bus/pci/devices/0000:08:00.0/driver_override
    $ echo <pci_id> > /sys/bus/pci/devices/0000:08:00.0/driver/bind

Attach VF to the VM.
Start the VM.
Perform live migration.
(将 VF 挂载到虚拟机。
启动虚拟机。
执行实时迁移。)

IPsec crypto capability setup
(IPsec 加密能力设置)
-----------------------------

When user enables IPsec crypto capability for a VF, user application can offload
XFRM state crypto operation (Encrypt/Decrypt) to this VF.
(当用户为 VF 启用 IPsec 加密能力时，用户应用程序可以将 XFRM state 的加密
操作 (加密/解密) 卸载到该 VF。)

When IPsec crypto capability is disabled (default) for a VF, the XFRM state is
processed in software by the kernel.
(当 VF 的 IPsec 加密能力被禁用 (默认值) 时，XFRM state 由内核在软件中
处理。)

- Get IPsec crypto capability of the VF device::
  (获取 VF 设备的 IPsec 加密能力：)

    $ devlink port show pci/0000:06:00.0/2
    pci/0000:06:00.0/2: type eth netdev enp6s0pf0vf1 flavour pcivf pfnum 0 vfnum 1
        function:
            hw_addr 00:00:00:00:00:00 ipsec_crypto disabled

- Set IPsec crypto capability of the VF device::
  (设置 VF 设备的 IPsec 加密能力：)

    $ devlink port function set pci/0000:06:00.0/2 ipsec_crypto enable

    $ devlink port show pci/0000:06:00.0/2
    pci/0000:06:00.0/2: type eth netdev enp6s0pf0vf1 flavour pcivf pfnum 0 vfnum 1
        function:
            hw_addr 00:00:00:00:00:00 ipsec_crypto enabled

IPsec packet capability setup
(IPsec 数据包能力设置)
-----------------------------

When user enables IPsec packet capability for a VF, user application can offload
XFRM state and policy crypto operation (Encrypt/Decrypt) to this VF, as well as
IPsec encapsulation.
(当用户为 VF 启用 IPsec 数据包能力时，用户应用程序可以将 XFRM state 和
policy 的加密操作 (加密/解密) 以及 IPsec 封装载入卸载到该 VF。)

When IPsec packet capability is disabled (default) for a VF, the XFRM state and
policy is processed in software by the kernel.
(当 VF 的 IPsec 数据包能力被禁用 (默认值) 时，XFRM state 和 policy 由内核在
软件中处理。)

- Get IPsec packet capability of the VF device::
  (获取 VF 设备的 IPsec 数据包能力：)

    $ devlink port show pci/0000:06:00.0/2
    pci/0000:06:00.0/2: type eth netdev enp6s0pf0vf1 flavour pcivf pfnum 0 vfnum 1
        function:
            hw_addr 00:00:00:00:00:00 ipsec_packet disabled

- Set IPsec packet capability of the VF device::
  (设置 VF 设备的 IPsec 数据包能力：)

    $ devlink port function set pci/0000:06:00.0/2 ipsec_packet enable

    $ devlink port show pci/0000:06:00.0/2
    pci/0000:06:00.0/2: type eth netdev enp6s0pf0vf1 flavour pcivf pfnum 0 vfnum 1
        function:
            hw_addr 00:00:00:00:00:00 ipsec_packet enabled

Maximum IO events queues setup
(最大 IO 事件队列设置)
------------------------------

When user sets maximum number of IO event queues for a SF or
a VF, such function driver is limited to consume only enforced
number of IO event queues.
(当用户为 SF 或 VF 设置 IO 事件队列的最大数量时，此类功能驱动程序只能消耗
限定数量的 IO 事件队列。)

IO event queues deliver events related to IO queues, including network
device transmit and receive queues (txq and rxq) and RDMA Queue Pairs (QPs).
For example, the number of netdevice channels and RDMA device completion
vectors are derived from the function's IO event queues. Usually, the number
of interrupt vectors consumed by the driver is limited by the number of IO
event queues per device, as each of the IO event queues is connected to an
interrupt vector.
(IO 事件队列传递与 IO 队列相关的事件，包括网络设备的发送和接收队列 (txq 和
rxq) 以及 RDMA 队列对 (QP)。例如，netdevice 通道数和 RDMA 设备完成向量数
都由该功能的 IO 事件队列派生。通常，驱动程序消耗的中断向量数受每设备 IO 事件
队列数量的限制，因为每个 IO 事件队列都连接到一个中断向量。)

- Get maximum IO event queues of the VF device::
  (获取 VF 设备的最大 IO 事件队列数：)

    $ devlink port show pci/0000:06:00.0/2
    pci/0000:06:00.0/2: type eth netdev enp6s0pf0vf1 flavour pcivf pfnum 0 vfnum 1
        function:
            hw_addr 00:00:00:00:00:00 ipsec_packet disabled max_io_eqs 10

- Set maximum IO event queues of the VF device::
  (设置 VF 设备的最大 IO 事件队列数：)

    $ devlink port function set pci/0000:06:00.0/2 max_io_eqs 32

    $ devlink port show pci/0000:06:00.0/2
    pci/0000:06:00.0/2: type eth netdev enp6s0pf0vf1 flavour pcivf pfnum 0 vfnum 1
        function:
            hw_addr 00:00:00:00:00:00 ipsec_packet disabled max_io_eqs 32

Subfunction
(子功能)
============

Subfunction is a lightweight function that has a parent PCI function on which
it is deployed. Subfunction is created and deployed in unit of 1. Unlike
SRIOV VFs, a subfunction doesn't require its own PCI virtual function.
A subfunction communicates with the hardware through the parent PCI function.
(子功能 (subfunction) 是一种轻量级功能，它拥有一个作为其部署载体的父 PCI
功能。子功能以 1 为单位创建和部署。与 SRIOV VF 不同，子功能不需要自己的 PCI
虚拟功能。子功能通过父 PCI 功能与硬件通信。)

To use a subfunction, 3 steps setup sequence is followed:
(要使用子功能，需遵循以下 3 个步骤的设置顺序：)

1) create - create a subfunction;
   (1) create - 创建一个子功能；
2) configure - configure subfunction attributes;
   (2) configure - 配置子功能属性；
3) deploy - deploy the subfunction;
   (3) deploy - 部署该子功能；

Subfunction management is done using devlink port user interface.
User performs setup on the subfunction management device.
(子功能管理通过 devlink 端口用户界面完成。用户在子功能管理设备上执行设置。)

(1) Create
((1) 创建)
----------

A subfunction is created using a devlink port interface. A user adds the
subfunction by adding a devlink port of subfunction flavour. The devlink
kernel code calls down to subfunction management driver (devlink ops) and asks
it to create a subfunction devlink port. Driver then instantiates the
subfunction port and any associated objects such as health reporters and
representor netdevice.
(子功能通过 devlink 端口接口创建。用户通过添加一个子功能类型 (flavour) 的
devlink 端口来添加子功能。devlink 内核代码向下调用子功能管理驱动程序
(devlink ops)，要求它创建一个子功能 devlink 端口。驱动程序随后实例化该
子功能端口以及任何相关联的对象，例如健康报告器和 representor 网络设备。)

(2) Configure
((2) 配置)
-------------

A subfunction devlink port is created but it is not active yet. That means the
entities are created on devlink side, the e-switch port representor is created,
but the subfunction device itself is not created. A user might use e-switch port
representor to do settings, putting it into bridge, adding TC rules, etc. A user
might as well configure the hardware address (such as MAC address) of the
subfunction while subfunction is inactive.
(子功能 devlink 端口已创建但尚未激活。这意味着相关实体已在 devlink 侧创建，
e-switch 端口 representor 已创建，但子功能设备本身尚未创建。用户可以使用
e-switch 端口 representor 进行设置，例如将其加入网桥、添加 TC 规则等。在
子功能处于未激活状态时，用户也可以配置子功能的硬件地址 (如 MAC 地址)。)

(3) Deploy
((3) 部署)
----------

Once a subfunction is configured, user must activate it to use it. Upon
activation, subfunction management driver asks the subfunction management
device to instantiate the subfunction device on particular PCI function.
A subfunction device is created on the :ref:`Documentation/driver-api/auxiliary_bus.rst <auxiliary_bus>`.
At this point a matching subfunction driver binds to the subfunction's auxiliary device.
(子功能配置完成后，用户必须将其激活才能使用。激活时，子功能管理驱动程序会
要求子功能管理设备在特定的 PCI 功能上实例化子功能设备。子功能设备创建在
:ref:`Documentation/driver-api/auxiliary_bus.rst <auxiliary_bus>` 上。
此时，匹配的子功能驱动程序会绑定到该子功能的辅助 (auxiliary) 设备。)

Rate object management
(速率对象管理)
======================

Devlink provides API to manage tx rates of single devlink port or a group.
This is done through rate objects, which can be one of the two types:
(Devlink 提供用于管理单个 devlink 端口或一组端口的发送速率的 API。这是通过
速率对象 (rate objects) 完成的，速率对象可以是以下两种类型之一：)

``leaf``
(叶子节点)

  Represents a single devlink port; created/destroyed by the driver. Since leaf
  have 1to1 mapping to its devlink port, in user space it is referred as
  ``pci/<bus_addr>/<port_index>``;
  (表示单个 devlink 端口；由驱动程序创建/销毁。由于 leaf 与其 devlink 端口
  一一对应，在用户空间中它以 ``pci/<bus_addr>/<port_index>`` 的形式被
  引用；)

``node``
(节点)

  Represents a group of rate objects (leafs and/or nodes); created/deleted by
  request from the userspace; initially empty (no rate objects added). In
  userspace it is referred as ``pci/<bus_addr>/<node_name>``, where
  ``node_name`` can be any identifier, except decimal number, to avoid
  collisions with leafs.
  (表示一组速率对象 (叶子节点和/或节点)；根据用户空间的请求创建/删除；初始
  为空 (未添加任何速率对象)。在用户空间中它以 ``pci/<bus_addr>/<node_name>``
  的形式被引用，其中 ``node_name`` 可以是除十进制数字之外的任何标识符，
  以避免与叶子节点发生冲突。)

API allows to configure following rate object's parameters:
(API 允许配置以下速率对象参数：)

``tx_share``
(共享发送速率)

  Minimum TX rate value shared among all other rate objects, or rate objects
  that parts of the parent group, if it is a part of the same group.
  (与其他所有速率对象共享的最小发送速率值；如果它属于同一个组，则与父组中的
  各部分速率对象共享。)

``tx_max``
(最大发送速率)

  Maximum TX rate value.
  (最大发送速率值。)

``tx_priority``
(发送优先级)

  Allows for usage of strict priority arbiter among siblings. This
  arbitration scheme attempts to schedule nodes based on their priority
  as long as the nodes remain within their bandwidth limit. The higher the
  priority the higher the probability that the node will get selected for
  scheduling.
  (允许在兄弟节点之间使用严格优先级仲裁器。只要节点保持在各自的带宽限制
  之内，这种仲裁方案就会根据节点的优先级进行调度。优先级越高，节点被选中
  参与调度的概率就越高。)

``tx_weight``
(发送权重)

  Allows for usage of Weighted Fair Queuing arbitration scheme among
  siblings. This arbitration scheme can be used simultaneously with the
  strict priority. As a node is configured with a higher rate it gets more
  BW relative to its siblings. Values are relative like a percentage
  points, they basically tell how much BW should node take relative to
  its siblings.
  (允许在兄弟节点之间使用加权公平排队 (WFQ) 仲裁方案。这种仲裁方案可以与
  严格优先级同时使用。节点配置的速率越高，它相对于兄弟节点获得的带宽就越
  多。数值是相对的，类似于百分点，它们大致表明节点相对于兄弟节点应占多少
  带宽。)

``parent``
(父节点)

  Parent node name. Parent node rate limits are considered as additional limits
  to all node children limits. ``tx_max`` is an upper limit for children.
  ``tx_share`` is a total bandwidth distributed among children.
  (父节点名称。父节点的速率限制被视为对节点所有子节点限制的附加限制。
  ``tx_max`` 是子节点的上限。 ``tx_share`` 是在子节点之间分配的总带宽。)

``tc_bw``
(流量类别带宽)

  Allow users to set the bandwidth allocation per traffic class on rate
  objects. This enables fine-grained QoS configurations by assigning a relative
  share value to each traffic class. The bandwidth is distributed in proportion
  to the share value for each class, relative to the sum of all shares.
  When applied to a non-leaf node, tc_bw determines how bandwidth is shared
  among its child elements.
  (允许用户设置速率对象上每个流量类别的带宽分配。通过为每个流量类别分配一个
  相对的份额值，可以实现细粒度的 QoS 配置。带宽按每个类别份额值相对于所有
  份额总和的比例进行分配。应用于非叶子节点时，tc_bw 决定带宽如何在其子元素
  之间共享。)

``tx_priority`` and ``tx_weight`` can be used simultaneously. In that case
nodes with the same priority form a WFQ subgroup in the sibling group
and arbitration among them is based on assigned weights.
(``tx_priority`` 和 ``tx_weight`` 可以同时使用。在这种情况下，具有相同
优先级的节点会在兄弟节点组内构成一个 WFQ 子组，它们之间的仲裁基于所分配的
权重。)

Arbitration flow from the high level:
(高层仲裁流程：)

#. Choose a node, or group of nodes with the highest priority that stays
   within the BW limit and are not blocked. Use ``tx_priority`` as a
   parameter for this arbitration.
   (选择优先级最高、处于带宽限制之内且未被阻塞的节点或节点组。使用
   ``tx_priority`` 作为此仲裁的参数。)

#. If group of nodes have the same priority perform WFQ arbitration on
   that subgroup. Use ``tx_weight`` as a parameter for this arbitration.
   (如果一组节点具有相同的优先级，则在该子组内执行 WFQ 仲裁。使用
   ``tx_weight`` 作为此仲裁的参数。)

#. Select the winner node, and continue arbitration flow among its children,
   until leaf node is reached, and the winner is established.
   (选出获胜节点，并在其子节点之间继续仲裁流程，直到到达叶子节点并确定最终
   获胜者。)

#. If all the nodes from the highest priority sub-group are satisfied, or
   overused their assigned BW, move to the lower priority nodes.
   (如果最高优先级子组中的所有节点都已满足需求，或者超用了所分配的带宽，
   则转到较低优先级的节点。)

Driver implementations are allowed to support both or either rate object types
and setting methods of their parameters. Additionally driver implementation
may export nodes/leafs and their child-parent relationships.
(驱动程序实现可以同时支持或仅支持这两种速率对象类型及其参数的设置方法。
此外，驱动程序实现还可以导出节点/叶子节点及其父子关系。)

Terms and Definitions
(术语和定义)
=====================

.. list-table:: Terms and Definitions
   :widths: 22 90

   * - Term (术语)
     - Definitions (定义)
   * - ``PCI device``
     - A physical PCI device having one or more PCI buses consists of one or
       more PCI controllers.
       (拥有一个或多个 PCI 总线的物理 PCI 设备，由一个或多个 PCI 控制器
       组成。)
   * - ``PCI controller``
     -  A controller consists of potentially multiple physical functions,
        virtual functions and subfunctions.
        (一个控制器可能由多个物理功能、虚拟功能和子功能组成。)
   * - ``Port function``
     -  An object to manage the function of a port.
        (用于管理端口功能的对象。)
   * - ``Subfunction``
     -  A lightweight function that has parent PCI function on which it is
        deployed.
        (一种轻量级功能，它拥有一个作为其部署载体的父 PCI 功能。)
   * - ``Subfunction device``
     -  A bus device of the subfunction, usually on a auxiliary bus.
        (子功能在总线上的设备，通常位于辅助 (auxiliary) 总线上。)
   * - ``Subfunction driver``
     -  A device driver for the subfunction auxiliary device.
        (子功能辅助设备的设备驱动程序。)
   * - ``Subfunction management device``
     -  A PCI physical function that supports subfunction management.
        (支持子功能管理的 PCI 物理功能。)
   * - ``Subfunction management driver``
     -  A device driver for PCI physical function that supports
        subfunction management using devlink port interface.
        (支持通过 devlink 端口接口进行子功能管理的 PCI 物理功能的设备
        驱动程序。)
   * - ``Subfunction host driver``
     -  A device driver for PCI physical function that hosts subfunction
        devices. In most cases it is same as subfunction management driver. When
        subfunction is used on external controller, subfunction management and
        host drivers are different.
        (承载子功能设备的 PCI 物理功能的设备驱动程序。在大多数情况下，它与
        子功能管理驱动程序相同。当子功能在外部控制器上使用时，子功能管理
        驱动程序与宿主驱动程序是不同的。)
