========================================================================
The irq_domain Interrupt Number Mapping Library(irq_domain 中断号映射库)
========================================================================

The current design of the Linux kernel uses a single large number
space where each separate IRQ source is assigned a unique number.
This is simple when there is only one interrupt controller. But in
systems with multiple interrupt controllers, the kernel must ensure
that each one gets assigned non-overlapping allocations of Linux
IRQ numbers.
(Linux 内核目前的设计使用单一的大编号空间，其中每个独立的 IRQ 源都被分配一个唯一的编号。只有一个中断控制器时这很简单。但在有多个中断控制器的系统中，内核必须确保每个控制器分到的 Linux IRQ 编号互不重叠。)

The number of interrupt controllers registered as unique irqchips
shows a rising tendency. For example, subdrivers of different kinds
such as GPIO controllers avoid reimplementing identical callback
mechanisms as the IRQ core system by modelling their interrupt
handlers as irqchips. I.e. in effect cascading interrupt controllers.
(注册为独立 irqchip 的中断控制器数量呈上升趋势。例如，GPIO 控制器等不同类型的子驱动把自己的中断处理程序建模为 irqchip，从而避免重复实现与 IRQ 核心系统相同的回调机制——实际上就是级联中断控制器。)

So in the past, IRQ numbers could be chosen so that they match the
hardware IRQ line into the root interrupt controller (i.e. the
component actually firing the interrupt line to the CPU). Nowadays,
this number is just a number and the number has no
relationship to hardware interrupt numbers.
(在过去，IRQ 编号可以选成与根中断控制器的硬件 IRQ 线一致（即真正向 CPU 触发中断线的部件）。如今，这个编号只是一个数字，与硬件中断号没有任何关系。)

For this reason, we need a mechanism to separate controller-local
interrupt numbers, called hardware IRQs, from Linux IRQ numbers.
(因此，我们需要一种机制，把控制器本地的中断号（称为硬件 IRQ，hwirq）与 Linux IRQ 编号区分开。)

The irq_alloc_desc*() and irq_free_desc*() APIs provide allocation of
IRQ numbers, but they don't provide any support for reverse mapping of
the controller-local IRQ (hwirq) number into the Linux IRQ number
space.
(irq_alloc_desc*() 和 irq_free_desc*() API 提供 IRQ 编号的分配，但不提供把控制器本地 IRQ（hwirq）号反向映射到 Linux IRQ 编号空间的任何支持。)

The irq_domain library adds a mapping between hwirq and IRQ numbers on
top of the irq_alloc_desc*() API. An irq_domain to manage the mapping
is preferred over interrupt controller drivers open coding their own
reverse mapping scheme.
(irq_domain 库在 irq_alloc_desc*() API 之上增加了 hwirq 与 IRQ 编号之间的映射。比起由中断控制器驱动各自手写反向映射方案，更推荐用一个 irq_domain 来管理映射。)

irq_domain also implements a translation from an abstract struct
irq_fwspec to hwirq numbers (Device Tree, non-DT firmware node, ACPI
GSI, and software node so far), and can be easily extended to support
other IRQ topology data sources. The implementation is performed
without any extra platform support code.
(irq_domain 还实现了从抽象的 struct irq_fwspec 到 hwirq 号的转换（目前支持 Device Tree、非 DT 固件节点、ACPI GSI 和软件节点），并且可以方便地扩展以支持其他 IRQ 拓扑数据源。其实现不需要任何额外的平台支持代码。)

irq_domain Usage(irq_domain 的使用)
===================================
struct irq_domain could be defined as an irq domain controller. That
is, it handles the mapping between hardware and virtual interrupt
numbers for a given interrupt domain. The domain structure is
generally created by the PIC code for a given PIC instance (though a
domain can cover more than one PIC if they have a flat number model).
It is the domain callbacks that are responsible for setting the
irq_chip on a given irq_desc after it has been mapped.
(struct irq_domain 可以定义为一个 irq 域控制器。也就是说，它处理给定中断域中硬件中断号与虚拟中断号之间的映射。域结构通常由给定 PIC 实例的 PIC 代码创建（不过如果一个域覆盖多个采用扁平编号模型的 PIC 也是允许的）。映射完成后，由域回调负责在给定的 irq_desc 上设置 irq_chip。)

The host code and data structures use a fwnode_handle pointer to
identify the domain. In some cases, and in order to preserve source
code compatibility, this fwnode pointer is "upgraded" to a DT
device_node. For those firmware infrastructures that do not provide a
unique identifier for an interrupt controller, the irq_domain code
offers a fwnode allocator.
(宿主代码与数据结构用 fwnode_handle 指针来标识域。在某些情况下，为了保持源码兼容性，该 fwnode 指针会被"升级"为 DT device_node。对于不为中断控制器提供唯一标识符的固件基础设施，irq_domain 代码提供了 fwnode 分配器。)

An interrupt controller driver creates and registers a struct irq_domain
by calling one of the irq_domain_create_*() functions (each mapping
method has a different allocator function, more on that later). The
function will return a pointer to the struct irq_domain on success. The
caller must provide the allocator function with a struct irq_domain_ops
pointer.
(中断控制器驱动通过调用 irq_domain_create_*() 函数之一来创建并注册 struct irq_domain（每种映射方法有不同的分配器函数，稍后详述）。成功时该函数返回指向 struct irq_domain 的指针。调用者必须向分配器函数提供一个 struct irq_domain_ops 指针。)

In most cases, the irq_domain will begin empty without any mappings
between hwirq and IRQ numbers.  Mappings are added to the irq_domain
by calling irq_create_mapping() which accepts the irq_domain and a
hwirq number as arguments. If a mapping for the hwirq doesn't already
exist, irq_create_mapping() allocates a new Linux irq_desc, associates
it with the hwirq, and calls the :c:member:`irq_domain_ops.map()`
callback. In there, the driver can perform any required hardware
setup.
(大多数情况下，irq_domain 开始时是空的，没有任何 hwirq 与 IRQ 编号之间的映射。通过调用 irq_create_mapping() 向 irq_domain 添加映射，它接受 irq_domain 和一个 hwirq 号作为参数。如果该 hwirq 的映射尚不存在，irq_create_mapping() 会分配一个新的 Linux irq_desc，把它与 hwirq 关联，并调用 :c:member:`irq_domain_ops.map()` 回调。驱动可以在该回调中完成任何所需的硬件设置。)

Once a mapping has been established, it can be retrieved or used via a
variety of methods:
(映射建立之后，可以通过多种方法检索或使用它：)

- irq_resolve_mapping() returns a pointer to the irq_desc structure
  for a given domain and hwirq number, or NULL if there was no
  mapping.
  (irq_resolve_mapping() 返回给定域与 hwirq 号所对应的 irq_desc 结构指针；若无映射则返回 NULL。)
- irq_find_mapping() returns a Linux IRQ number for a given domain and
  hwirq number, or 0 if there was no mapping
  (irq_find_mapping() 返回给定域与 hwirq 号对应的 Linux IRQ 编号；若无映射则返回 0。)
- generic_handle_domain_irq() handles an interrupt described by a
  domain and a hwirq number
  (generic_handle_domain_irq() 处理由域与 hwirq 号描述的中断。)

Note that irq_domain lookups must happen in contexts that are
compatible with an RCU read-side critical section.
(注意，irq_domain 查找必须发生在与 RCU 读侧临界区兼容的上下文中。)

The irq_create_mapping() function must be called *at least once*
before any call to irq_find_mapping(), lest the descriptor will not
be allocated.
(在任何 irq_find_mapping() 调用之前，必须*至少调用一次* irq_create_mapping()，否则描述符将不会被分配。)

If the driver has the Linux IRQ number or the irq_data pointer, and
needs to know the associated hwirq number (such as in the irq_chip
callbacks) then it can be directly obtained from
:c:member:`irq_data.hwirq`.
(如果驱动已有 Linux IRQ 编号或 irq_data 指针，而需要知道关联的 hwirq 号（例如在 irq_chip 回调中），可以直接从 :c:member:`irq_data.hwirq` 获得。)

Types of irq_domain Mappings(irq_domain 映射的类型)
===================================================

There are several mechanisms available for reverse mapping from hwirq
to Linux IRQ, and each mechanism uses a different allocation function.
Which reverse map type should be used depends on the use case.  Each
of the reverse map types are described below:
(从 hwirq 反向映射到 Linux IRQ 有多种可用机制，每种机制使用不同的分配函数。应使用哪种反向映射类型取决于使用场景。下面逐一描述各反向映射类型：)

Linear(线性映射)
----------------

::

	irq_domain_create_linear()

The linear reverse map maintains a fixed-size table indexed by the
hwirq number.  When a hwirq is mapped, an irq_desc is allocated for
the hwirq, and the IRQ number is stored in the table.
(线性反向映射维护一张以 hwirq 号为索引的固定大小表。当某个 hwirq 被映射时，会为它分配一个 irq_desc，并把 IRQ 编号存入表中。)

The Linear map is a good choice when the maximum number of hwirqs is
fixed and a relatively small number (~ < 256).  The advantages of this
map are fixed-time lookup for IRQ numbers, and irq_descs are only
allocated for in-use IRQs.  The disadvantage is that the table must be
as large as the largest possible hwirq number.
(当 hwirq 的最大数量固定且相对较小（约 < 256）时，线性映射是很好的选择。它的优点是 IRQ 编号的查找时间为常数，且只为使用中的 IRQ 分配 irq_desc。缺点是表必须与最大可能的 hwirq 号一样大。)

The majority of drivers should use the Linear map.
(大多数驱动应使用线性映射。)

Tree(树形映射)
--------------

::

	irq_domain_create_tree()

The irq_domain maintains a radix tree map from hwirq numbers to Linux
IRQs.  When an hwirq is mapped, an irq_desc is allocated and the
hwirq is used as the lookup key for the radix tree.
(irq_domain 维护一棵从 hwirq 号到 Linux IRQ 的基数树映射。当某个 hwirq 被映射时，会分配一个 irq_desc，并以该 hwirq 作为基数树的查找键。)

The Tree map is a good choice if the hwirq number can be very large
since it doesn't need to allocate a table as large as the largest
hwirq number.  The disadvantage is that hwirq to IRQ number lookup is
dependent on how many entries are in the table.
(如果 hwirq 号可能非常大，树形映射是很好的选择，因为它不必分配一张与最大 hwirq 号一样大的表。缺点是 hwirq 到 IRQ 编号的查找依赖于表中条目的数量。)

Very few drivers should need this mapping.
(只有极少数驱动才需要这种映射。)

No Map(无映射)
--------------

::

	irq_domain_create_nomap()

The No Map mapping is to be used when the hwirq number is
programmable in the hardware.  In this case it is best to program the
Linux IRQ number into the hardware itself so that no mapping is
required.  Calling irq_create_direct_mapping() will allocate a Linux
IRQ number and call the .map() callback so that driver can program the
Linux IRQ number into the hardware.
(当 hwirq 号在硬件中可编程时，应使用"无映射"。这种情况下，最好直接把 Linux IRQ 编号编程到硬件中，从而无需任何映射。调用 irq_create_direct_mapping() 会分配一个 Linux IRQ 编号并调用 .map() 回调，让驱动把该 Linux IRQ 编号编程到硬件中。)

Most drivers cannot use this mapping, and it is now gated on the
CONFIG_IRQ_DOMAIN_NOMAP option. Please refrain from introducing new
users of this API.
(大多数驱动无法使用这种映射，且它现在受 CONFIG_IRQ_DOMAIN_NOMAP 选项控制。请不要为该 API 引入新的使用者。)

Legacy(传统映射)
----------------

::

	irq_domain_create_simple()
	irq_domain_create_legacy()

The Legacy mapping is a special case for drivers that already have a
range of irq_descs allocated for the hwirqs.  It is used when the
driver cannot be immediately converted to use the Linear mapping.  For
example, many embedded system board support files use a set of #defines
for IRQ numbers that are passed to struct device registrations.  In that
case the Linux IRQ numbers cannot be dynamically assigned and the Legacy
mapping should be used.
(传统映射是针对已为 hwirq 分配好一段 irq_desc 的驱动的一种特殊情况。当驱动无法立刻改为使用线性映射时，就会用到它。例如，许多嵌入式系统的板级支持文件用一组 #define 定义 IRQ 编号并传给 struct device 注册。这种情况下 Linux IRQ 编号无法动态分配，应使用传统映射。)

As the name implies, the \*_legacy() functions are deprecated and only
exist to ease the support of ancient platforms. No new users should be
added. Same goes for the \*_simple() functions when their use results
in the legacy behaviour.
(顾名思义，\*_legacy() 函数已被废弃，只是为便于支持古老平台而存在，不应添加新的使用者。当 \*_simple() 函数的使用导致传统行为时，情况也一样。)

The Legacy map assumes a contiguous range of IRQ numbers has already
been allocated for the controller and that the IRQ number can be
calculated by adding a fixed offset to the hwirq number, and
visa-versa.  The disadvantage is that it requires the interrupt
controller to manage IRQ allocations and it requires an irq_desc to be
allocated for every hwirq, even if it is unused.
(传统映射假定已为控制器分配好一段连续的 IRQ 编号区间，且 IRQ 编号可以通过给 hwirq 号加上固定偏移计算出来（反之亦然）。缺点是它要求中断控制器自己管理 IRQ 分配，并要求为每个 hwirq 都分配一个 irq_desc，即使它未被使用。)

The Legacy map should only be used if fixed IRQ mappings must be
supported.  For example, ISA controllers would use the Legacy map for
mapping Linux IRQs 0-15 so that existing ISA drivers get the correct IRQ
numbers.
(只有在必须支持固定 IRQ 映射时才应使用传统映射。例如，ISA 控制器会用传统映射来映射 Linux IRQ 0-15，使现有的 ISA 驱动得到正确的 IRQ 编号。)

Most users of legacy mappings should use irq_domain_create_simple()
which will use a legacy domain only if an IRQ range is supplied by the
system and will otherwise use a linear domain mapping. The semantics of
this call are such that if an IRQ range is specified then descriptors
will be allocated on-the-fly for it, and if no range is specified it
will fall through to irq_domain_create_linear() which means *no* IRQ
descriptors will be allocated.
(传统映射的大多数使用者应使用 irq_domain_create_simple()：只有当系统提供了 IRQ 区间时它才使用传统域，否则使用线性域映射。该调用的语义是：如果指定了 IRQ 区间，则描述符会即时按需分配；如果未指定区间，则落到 irq_domain_create_linear()，这意味着*不会*分配任何 IRQ 描述符。)

A typical use case for simple domains is where an irqchip provider
is supporting both dynamic and static IRQ assignments.
(简单域的一个典型用例是 irqchip 提供者同时支持动态与静态 IRQ 分配。)

In order to avoid ending up in a situation where a linear domain is
used and no descriptor gets allocated it is very important to make sure
that the driver using the simple domain call irq_create_mapping()
before any irq_find_mapping() since the latter will actually work
for the static IRQ assignment case.
(为避免出现"使用了线性域却没有分配描述符"的局面，务必确保使用简单域的驱动在任何 irq_find_mapping() 之前先调用 irq_create_mapping()，因为后者实际上适用于静态 IRQ 分配的情形。)

Hierarchy IRQ Domain(层级式 IRQ Domain)
---------------------------------------

On some architectures, there may be multiple interrupt controllers
involved in delivering an interrupt from the device to the target CPU.
Let's look at a typical interrupt delivery path on x86 platforms::

  Device --> IOAPIC -> Interrupt remapping Controller -> Local APIC -> CPU

(在某些体系结构上，把设备的中断送达目标 CPU 可能涉及多个中断控制器。以 x86 平台上典型的中断传递路径为例：)

There are three interrupt controllers involved:
(涉及三个中断控制器：)

1) IOAPIC controller
2) Interrupt remapping controller
3) Local APIC controller

To support such a hardware topology and make software architecture match
hardware architecture, an irq_domain data structure is built for each
interrupt controller and those irq_domains are organized into hierarchy.
When building irq_domain hierarchy, the irq_domain nearest the device is
child and the irq_domain nearest the CPU is parent. So a hierarchy structure
as below will be built for the example above::

	CPU Vector irq_domain (root irq_domain to manage CPU vectors)
		^
		|
	Interrupt Remapping irq_domain (manage irq_remapping entries)
		^
		|
	IOAPIC irq_domain (manage IOAPIC delivery entries/pins)

(为了支持这样的硬件拓扑、使软件架构与硬件架构匹配，会为每个中断控制器构建一个 irq_domain 数据结构，并把这些 irq_domain 组织成层级。构建 irq_domain 层级时，离设备最近的 irq_domain 是子节点，离 CPU 最近的 irq_domain 是父节点。因此对上面的例子会构建出如上图所示的层级结构。)

There are four major interfaces to use hierarchy irq_domain:
(使用层级式 irq_domain 有四个主要接口：)

1) irq_domain_alloc_irqs(): allocate IRQ descriptors and interrupt
   controller related resources to deliver these interrupts.
   (irq_domain_alloc_irqs()：分配 IRQ 描述符与中断控制器相关资源，以传递这些中断。)
2) irq_domain_free_irqs(): free IRQ descriptors and interrupt controller
   related resources associated with these interrupts.
   (irq_domain_free_irqs()：释放 IRQ 描述符以及与这些中断关联的中断控制器相关资源。)
3) irq_domain_activate_irq(): activate interrupt controller hardware to
   deliver the interrupt.
   (irq_domain_activate_irq()：激活中断控制器硬件以传递中断。)
4) irq_domain_deactivate_irq(): deactivate interrupt controller hardware
   to stop delivering the interrupt.
   (irq_domain_deactivate_irq()：停用中断控制器硬件以停止传递中断。)

The following is needed to support hierarchy irq_domain:
(支持层级式 irq_domain 需要以下内容：)

1) The :c:member:`parent` field in struct irq_domain is used to
   maintain irq_domain hierarchy information.
   (struct irq_domain 中的 :c:member:`parent` 字段用于维护 irq_domain 层级信息。)
2) The :c:member:`parent_data` field in struct irq_data is used to
   build hierarchy irq_data to match hierarchy irq_domains. The
   irq_data is used to store irq_domain pointer and hardware irq
   number.
   (struct irq_data 中的 :c:member:`parent_data` 字段用于构建与层级式 irq_domain 匹配的层级式 irq_data。irq_data 用于存储 irq_domain 指针与硬件 irq 号。)
3) The :c:member:`alloc()`, :c:member:`free()`, and other callbacks in
   struct irq_domain_ops to support hierarchy irq_domain operations.
   (struct irq_domain_ops 中的 :c:member:`alloc()`、:c:member:`free()` 等回调，用于支持层级式 irq_domain 操作。)

With the support of hierarchy irq_domain and hierarchy irq_data ready,
an irq_domain structure is built for each interrupt controller, and an
irq_data structure is allocated for each irq_domain associated with an
IRQ.
(在层级式 irq_domain 与层级式 irq_data 的支持就绪后，会为每个中断控制器构建一个 irq_domain 结构，并为与某个 IRQ 关联的每个 irq_domain 分配一个 irq_data 结构。)

For an interrupt controller driver to support hierarchy irq_domain, it
needs to:
(中断控制器驱动要支持层级式 irq_domain，需要：)

1) Implement irq_domain_ops.alloc() and irq_domain_ops.free()
   (实现 irq_domain_ops.alloc() 与 irq_domain_ops.free()。)
2) Optionally, implement irq_domain_ops.activate() and
   irq_domain_ops.deactivate().
   (可选地实现 irq_domain_ops.activate() 与 irq_domain_ops.deactivate()。)
3) Optionally, implement an irq_chip to manage the interrupt controller
   hardware.
   (可选地实现一个 irq_chip 来管理中断控制器硬件。)
4) There is no need to implement irq_domain_ops.map() and
   irq_domain_ops.unmap(). They are unused with hierarchy irq_domain.
   (无需实现 irq_domain_ops.map() 与 irq_domain_ops.unmap()——层级式 irq_domain 不使用它们。)

Note the hierarchy irq_domain is in no way x86-specific, and is
heavily used to support other architectures, such as ARM, ARM64 etc.
(注意，层级式 irq_domain 绝非 x86 专属，它被大量用于支持其他体系结构，如 ARM、ARM64 等。)

Stacked irq_chip(堆叠式 irq_chip)
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

Now, we could go one step further to support stacked (hierarchy)
irq_chip. That is, an irq_chip is associated with each irq_data along
the hierarchy. A child irq_chip may implement a required action by
itself or by cooperating with its parent irq_chip.
(现在，我们可以更进一步支持堆叠式（层级式）irq_chip：层级中的每个 irq_data 都关联一个 irq_chip。子 irq_chip 可以自行实现所需的动作，也可以与其父 irq_chip 协作完成。)

With stacked irq_chip, interrupt controller driver only needs to deal
with the hardware managed by itself and may ask for services from its
parent irq_chip when needed. So we could achieve a much cleaner
software architecture.
(有了堆叠式 irq_chip，中断控制器驱动只需处理自己管理的硬件，并在需要时向其父 irq_chip 请求服务。因此我们可以得到更加干净的软件架构。)

Debugging(调试)
===============

Most of the internals of the IRQ subsystem are exposed in debugfs by
turning CONFIG_GENERIC_IRQ_DEBUGFS on.
(打开 CONFIG_GENERIC_IRQ_DEBUGFS 即可在 debugfs 中查看 IRQ 子系统的大部分内部状态。)

Structures and Public Functions Provided(提供的结构与公共函数)
==============================================================

This chapter contains the autogenerated documentation of the structures
and exported kernel API functions which are used for IRQ domains.
(本章包含 IRQ 域所用的结构与内核导出 API 函数的自动生成文档。)

.. kernel-doc:: include/linux/irqdomain.h

.. kernel-doc:: kernel/irq/irqdomain.c
   :export:

Internal Functions Provided(提供的内部函数)
===========================================

This chapter contains the autogenerated documentation of the internal
functions.
(本章包含内部函数的自动生成文档。)

.. kernel-doc:: kernel/irq/irqdomain.c
   :internal:
