.. SPDX-License-Identifier: GPL-2.0

=========================================================
vCPU feature selection on arm64(arm64 上的 vCPU 特性选择)
=========================================================

KVM/arm64 provides two mechanisms that allow userspace to configure
the CPU features presented to the guest.
(KVM/arm64 提供两种机制，允许用户态配置呈现给客户机的 CPU 特性。)

KVM_ARM_VCPU_INIT
=================

The ``KVM_ARM_VCPU_INIT`` ioctl accepts a bitmap of feature flags
(``struct kvm_vcpu_init::features``). Features enabled by this interface are
*opt-in* and may change/extend UAPI. See :ref:`KVM_ARM_VCPU_INIT` for complete
documentation of the features controlled by the ioctl.
(``KVM_ARM_VCPU_INIT`` ioctl 接受一个特性标志位图
(``struct kvm_vcpu_init::features``)。通过此接口启用的特性是选择性加入
(opt-in) 的，并可能更改/扩展 UAPI。该 ioctl 所控制特性的完整文档参见
:ref:`KVM_ARM_VCPU_INIT` 。)

Otherwise, all CPU features supported by KVM are described by the architected
ID registers.
(否则，KVM 支持的所有 CPU 特性都由体系架构定义的 ID 寄存器描述。)

The ID Registers(ID 寄存器)
===========================

The Arm architecture specifies a range of *ID Registers* that describe the set
of architectural features supported by the CPU implementation. KVM initializes
the guest's ID registers to the maximum set of CPU features supported by the
system. The ID register values may be VM-scoped in KVM, meaning that the
values could be shared for all vCPUs in a VM.
(Arm 体系架构规定了一系列 *ID 寄存器*，用于描述 CPU 实现所支持的体系架构
特性集合。KVM 把客户机的 ID 寄存器初始化为系统支持的 CPU 特性的最大集合。
在 KVM 中，ID 寄存器值可以是 VM 作用域的，也就是说这些值可以为 VM 中的所有
vCPU 共享。)

KVM allows userspace to *opt-out* of certain CPU features described by the ID
registers by writing values to them via the ``KVM_SET_ONE_REG`` ioctl. The ID
registers are mutable until the VM has started, i.e. userspace has called
``KVM_RUN`` on at least one vCPU in the VM. Userspace can discover what fields
are mutable in the ID registers using the ``KVM_ARM_GET_REG_WRITABLE_MASKS``.
See the :ref:`ioctl documentation <KVM_ARM_GET_REG_WRITABLE_MASKS>` for more
details.
(KVM 允许用户态通过 ``KVM_SET_ONE_REG`` ioctl 向 ID 寄存器写入值，从而选择
不使用 (opt-out) 其中描述的某些 CPU 特性。在 VM 启动之前——即用户态尚未对
VM 中至少一个 vCPU 调用 ``KVM_RUN``——ID 寄存器是可变的。用户态可以使用
``KVM_ARM_GET_REG_WRITABLE_MASKS`` 发现 ID 寄存器中哪些字段是可变的。更多
细节参见 :ref:`ioctl documentation <KVM_ARM_GET_REG_WRITABLE_MASKS>` 。)

Userspace is allowed to *limit* or *mask* CPU features according to the rules
outlined by the architecture in DDI0487J.a D19.1.3 'Principles of the ID
scheme for fields in ID register'. KVM does not allow ID register values that
exceed the capabilities of the system.
(允许用户态按照体系架构在 DDI0487J.a D19.1.3 "ID 寄存器中字段 ID 方案的
原则" 中概述的规则来 *限制* 或 *屏蔽* CPU 特性。KVM 不允许 ID 寄存器值
超出系统的能力。)

.. warning::
   It is **strongly recommended** that userspace modify the ID register values
   before accessing the rest of the vCPU's CPU register state. KVM may use the
   ID register values to control feature emulation. Interleaving ID register
   modification with other system register accesses may lead to unpredictable
   behavior.
   (强烈建议用户态在访问 vCPU 的其余 CPU 寄存器状态之前先修改 ID 寄存器
   值。KVM 可能会使用 ID 寄存器值来控制特性仿真。把修改 ID 寄存器与其他系统
   寄存器访问交错进行可能会导致不可预测的行为。)
