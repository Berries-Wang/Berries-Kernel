.. SPDX-License-Identifier: GPL-2.0

==========================================
MPIC interrupt controller(MPIC 中断控制器)
==========================================

Device types supported:
(支持的设备类型：)

  - KVM_DEV_TYPE_FSL_MPIC_20     Freescale MPIC v2.0(Freescale MPIC v2.0)
  - KVM_DEV_TYPE_FSL_MPIC_42     Freescale MPIC v4.2(Freescale MPIC v4.2)

Only one MPIC instance, of any type, may be instantiated.  The created
MPIC will act as the system interrupt controller, connecting to each
vcpu's interrupt inputs.
(只能实例化一个任意类型的 MPIC 实例。所创建的 MPIC 将充当系统中断控制器，
连接到每个 vcpu 的中断输入。)

Groups:
(组：)
  KVM_DEV_MPIC_GRP_MISC
   Attributes:
   (属性：)

    KVM_DEV_MPIC_BASE_ADDR (rw, 64-bit)
      Base address of the 256 KiB MPIC register space.  Must be
      naturally aligned.  A value of zero disables the mapping.
      Reset value is zero.
      (MPIC 寄存器空间 (256 KiB) 的基地址。必须自然对齐。值为零时禁用该
      映射。复位值为零。)

  KVM_DEV_MPIC_GRP_REGISTER (rw, 32-bit)
    Access an MPIC register, as if the access were made from the guest.
    "attr" is the byte offset into the MPIC register space.  Accesses
    must be 4-byte aligned.
    (访问一个 MPIC 寄存器，如同访问来自客户机。"attr" 是 MPIC 寄存器空间
    内的字节偏移。访问必须按 4 字节对齐。)

    MSIs may be signaled by using this attribute group to write
    to the relevant MSIIR.
    (可以使用此属性组向相关的 MSIIR 写入来发出 MSI 信号。)

  KVM_DEV_MPIC_GRP_IRQ_ACTIVE (rw, 32-bit)
    IRQ input line for each standard openpic source.  0 is inactive and 1
    is active, regardless of interrupt sense.
    (每个标准 openpic 源的 IRQ 输入线。无论中断极性如何，0 表示非活动，
    1 表示活动。)

    For edge-triggered interrupts:  Writing 1 is considered an activating
    edge, and writing 0 is ignored.  Reading returns 1 if a previously
    signaled edge has not been acknowledged, and 0 otherwise.
    (对于边沿触发中断：写入 1 被视为激活边沿，写入 0 会被忽略。如果先前
    发出的边沿尚未被确认，读取返回 1，否则返回 0。)

    "attr" is the IRQ number.  IRQ numbers for standard sources are the
    byte offset of the relevant IVPR from EIVPR0, divided by 32.
    ("attr" 是 IRQ 编号。标准源的 IRQ 编号是相应 IVPR 相对 EIVPR0 的字节
    偏移除以 32。)

IRQ Routing:
(IRQ 路由：)

  The MPIC emulation supports IRQ routing. Only a single MPIC device can
  be instantiated. Once that device has been created, it's available as
  irqchip id 0.
  (MPIC 仿真支持 IRQ 路由。只能实例化一个 MPIC 设备。该设备一旦创建，即
  可作为 irqchip id 0 使用。)

  This irqchip 0 has 256 interrupt pins, which expose the interrupts in
  the main array of interrupt sources (a.k.a. "SRC" interrupts).
  (此 irqchip 0 有 256 个中断引脚，暴露主中断源数组中的中断 (即 "SRC"
  中断)。)

  The numbering is the same as the MPIC device tree binding -- based on
  the register offset from the beginning of the sources array, without
  regard to any subdivisions in chip documentation such as "internal"
  or "external" interrupts.
  (编号方式与 MPIC 设备树绑定相同——基于从 sources 数组起始处算起的寄存器
  偏移，而不考虑芯片文档中诸如 "internal" 或 "external" 中断之类的细分。)

  Access to non-SRC interrupts is not implemented through IRQ routing mechanisms.
  (对非 SRC 中断的访问并未通过 IRQ 路由机制实现。)
