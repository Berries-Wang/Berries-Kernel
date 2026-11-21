.. SPDX-License-Identifier: GPL-2.0

ACRN Hypervisor Introduction(ACRN Hypervisor 简介)
==================================================

The ACRN Hypervisor is a Type 1 hypervisor, running directly on bare-metal
hardware. It has a privileged management VM, called Service VM, to manage User
VMs and do I/O emulation.
(ACRN Hypervisor 是 Type 1 型 hypervisor，直接运行在裸机硬件之上。它拥有一个
特权管理 VM，称为 Service VM，用于管理 User VM 并执行 I/O 模拟。)

ACRN userspace is an application running in the Service VM that emulates
devices for a User VM based on command line configurations. ACRN Hypervisor
Service Module (HSM) is a kernel module in the Service VM which provides
hypervisor services to the ACRN userspace.
(ACRN userspace 是运行在 Service VM 中的一个应用程序，它根据命令行配置为
User VM 模拟设备。ACRN Hypervisor Service Module（HSM）是 Service VM 中的
一个内核模块，为 ACRN userspace 提供 hypervisor 服务。)

Below figure shows the architecture.

::

                Service VM                    User VM
      +----------------------------+  |  +------------------+
      |        +--------------+    |  |  |                  |
      |        |ACRN userspace|    |  |  |                  |
      |        +--------------+    |  |  |                  |
      |-----------------ioctl------|  |  |                  |   ...
      |kernel space   +----------+ |  |  |                  |
      |               |   HSM    | |  |  | Drivers          |
      |               +----------+ |  |  |                  |
      +--------------------|-------+  |  +------------------+
  +---------------------hypercall----------------------------------------+
  |                         ACRN Hypervisor                              |
  +----------------------------------------------------------------------+
  |                          Hardware                                    |
  +----------------------------------------------------------------------+

(下图展示了其架构。)

ACRN userspace allocates memory for the User VM, configures and initializes the
devices used by the User VM, loads the virtual bootloader, initializes the
virtual CPU state and handles I/O request accesses from the User VM. It uses
ioctls to communicate with the HSM. HSM implements hypervisor services by
interacting with the ACRN Hypervisor via hypercalls. HSM exports a char device
interface (/dev/acrn_hsm) to userspace.
(ACRN userspace 为 User VM 分配内存，配置并初始化 User VM 所使用的设备，加载
虚拟 bootloader，初始化虚拟 CPU 状态，并处理来自 User VM 的 I/O 请求访问。它
使用 ioctl 与 HSM 通信。HSM 通过 hypercall 与 ACRN Hypervisor 交互来实现
hypervisor 服务。HSM 向用户空间导出一个字符设备接口 (/dev/acrn_hsm)。)

The ACRN hypervisor is open for contribution from anyone. The source repo is
available at https://github.com/projectacrn/acrn-hypervisor.
(ACRN hypervisor 欢迎任何人参与贡献。源码仓库见
https://github.com/projectacrn/acrn-hypervisor。)
