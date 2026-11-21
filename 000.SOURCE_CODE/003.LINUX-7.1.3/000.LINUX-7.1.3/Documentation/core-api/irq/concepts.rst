=============================
What is an IRQ?(什么是 IRQ？)
=============================

An IRQ is an interrupt request from a device. Currently, they can come
in over a pin, or over a packet. Several devices may be connected to
the same pin thus sharing an IRQ. Such as on legacy PCI bus: All devices
typically share 4 lanes/pins. Note that each device can request an
interrupt on each of the lanes.
(IRQ 是来自设备的中断请求。目前，中断可以通过引脚或数据包到来。多个设备可以连接到同一个引脚，从而共享一个 IRQ。例如在传统 PCI 总线上：所有设备通常共享 4 条通道/引脚。注意，每个设备可以在每条通道上请求中断。)

An IRQ number is a kernel identifier used to talk about a hardware
interrupt source. Typically, this is an index into the global irq_desc
array or sparse_irqs tree. But except for what linux/interrupt.h
implements, the details are architecture specific.
(IRQ 编号是用于指代硬件中断源的内核标识符。通常，它是全局 irq_desc 数组或 sparse_irqs 树的索引。但除 linux/interrupt.h 所实现的部分之外，其细节与具体架构相关。)

An IRQ number is an enumeration of the possible interrupt sources on a
machine. Typically, what is enumerated is the number of input pins on
all of the interrupt controllers in the system. In the case of ISA,
what is enumerated are the 8 input pins on each of the two i8259
interrupt controllers.
(IRQ 编号是对机器上可能的中断源的枚举。通常被枚举的是系统中所有中断控制器的输入引脚数量。就 ISA 而言，被枚举的是两个 i8259 中断控制器各自拥有的 8 个输入引脚。)

Architectures can assign additional meaning to the IRQ numbers, and
are encouraged to in the case where there is any manual configuration
of the hardware involved. The ISA IRQs are a classic example of
assigning this kind of additional meaning.
(架构可以为 IRQ 编号赋予额外的含义，并且在涉及硬件手动配置时鼓励这样做。ISA IRQ 就是为中断编号赋予此类额外含义的经典例子。)
