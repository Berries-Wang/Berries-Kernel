.. include:: <isonum.txt>

===============================================
Linux generic IRQ handling(Linux 通用 IRQ 处理)
===============================================

:Copyright: |copy| 2005-2010: Thomas Gleixner
            (版权所有 |copy| 2005-2010：Thomas Gleixner)
:Copyright: |copy| 2005-2006:  Ingo Molnar
            (版权所有 |copy| 2005-2006：Ingo Molnar)

Introduction(引言)
==================

The generic interrupt handling layer is designed to provide a complete
abstraction of interrupt handling for device drivers. It is able to
handle all the different types of interrupt controller hardware. Device
drivers use generic API functions to request, enable, disable and free
interrupts. The drivers do not have to know anything about interrupt
hardware details, so they can be used on different platforms without
code changes.
(通用中断处理层旨在为设备驱动提供完整的中断处理抽象。它能够处理所有不同类型的中断控制器硬件。设备驱动使用通用 API 函数来请求、启用、禁用和释放中断。驱动无需了解任何中断硬件细节，因此无需修改代码即可在不同平台上使用。)

This documentation is provided to developers who want to implement an
interrupt subsystem based for their architecture, with the help of the
generic IRQ handling layer.
(本文档面向希望借助通用 IRQ 处理层为其架构实现中断子系统的开发者。)

Rationale(设计缘由)
===================

The original implementation of interrupt handling in Linux uses the
__do_IRQ() super-handler, which is able to deal with every type of
interrupt logic.
(Linux 中断处理的最初实现使用 __do_IRQ() 超级处理器(super-handler)，它能够应付每一种类型的中断逻辑。)

Originally, Russell King identified different types of handlers to build
a quite universal set for the ARM interrupt handler implementation in
Linux 2.5/2.6. He distinguished between:
(最初，Russell King 在 Linux 2.5/2.6 的 ARM 中断处理器实现中识别出不同类型的 handler，构建了一套相当通用的集合。他区分出：)

-  Level type(电平类型)

-  Edge type(边沿类型)

-  Simple type(简单类型)

During the implementation we identified another type:
(在实现过程中我们又识别出另一种类型：)

-  Fast EOI type(快速 EOI 类型)

In the SMP world of the __do_IRQ() super-handler another type was
identified:
(在 __do_IRQ() 超级处理器的 SMP 世界中，又识别出了另一种类型：)

-  Per CPU type(per CPU 类型)

This split implementation of high-level IRQ handlers allows us to
optimize the flow of the interrupt handling for each specific interrupt
type. This reduces complexity in that particular code path and allows
the optimized handling of a given type.
(这种高层 IRQ handler 的拆分实现让我们能够针对每种特定中断类型优化中断处理流程。这降低了该特定代码路径的复杂性，并使对给定类型的优化处理成为可能。)

The original general IRQ implementation used hw_interrupt_type
structures and their ``->ack``, ``->end`` [etc.] callbacks to differentiate
the flow control in the super-handler. This leads to a mix of flow logic
and low-level hardware logic, and it also leads to unnecessary code
duplication: for example in i386, there is an ``ioapic_level_irq`` and an
``ioapic_edge_irq`` IRQ-type which share many of the low-level details but
have different flow handling.
(最初的通用 IRQ 实现使用 hw_interrupt_type 结构及其 ``->ack`` 、 ``->end`` [等]回调来区分超级处理器中的流控制。这导致流逻辑与底层硬件逻辑混杂在一起，还导致不必要的代码重复：例如在 i386 中，有 ``ioapic_level_irq`` 与 ``ioapic_edge_irq`` 两种 IRQ 类型，它们共享许多底层细节，却有着不同的流处理。)

A more natural abstraction is the clean separation of the 'irq flow' and
the 'chip details'.
(一种更自然的抽象是把 "irq flow" 与 "chip details" 干净地分离。)

Analysing a couple of architecture's IRQ subsystem implementations
reveals that most of them can use a generic set of 'irq flow' methods
and only need to add the chip-level specific code. The separation is
also valuable for (sub)architectures which need specific quirks in the
IRQ flow itself but not in the chip details - and thus provides a more
transparent IRQ subsystem design.
(分析若干架构的 IRQ 子系统实现可以发现，它们中的大多数都可以使用一组通用的 "irq flow" 方法，只需添加芯片级特定代码。这种分离对那些需要在 IRQ 流本身(而非芯片细节)中实现特定怪癖的(子)架构也很有价值——因此它带来了一种更透明的 IRQ 子系统设计。)

Each interrupt descriptor is assigned its own high-level flow handler,
which is normally one of the generic implementations. (This high-level
flow handler implementation also makes it simple to provide
demultiplexing handlers which can be found in embedded platforms on
various architectures.)
(每个中断描述符都被分配自己的高层流 handler，它通常是某个通用实现。(这种高层流 handler 实现也使提供多路分解(demultiplexing)handler 变得简单，这类 handler 可在各种架构的嵌入式平台上找到。))

The separation makes the generic interrupt handling layer more flexible
and extensible. For example, an (sub)architecture can use a generic
IRQ-flow implementation for 'level type' interrupts and add a
(sub)architecture specific 'edge type' implementation.
(这种分离使通用中断处理层更灵活、更可扩展。例如，(子)架构可以为 "level type" 中断使用通用 IRQ 流实现，并添加(子)架构特定的 "edge type" 实现。)

To make the transition to the new model easier and prevent the breakage
of existing implementations, the __do_IRQ() super-handler is still
available. This leads to a kind of duality for the time being. Over time
the new model should be used in more and more architectures, as it
enables smaller and cleaner IRQ subsystems. It's deprecated for three
years now and about to be removed.
(为了让向新模型的过渡更容易，并避免破坏现有实现，__do_IRQ() 超级处理器仍然可用。这在目前造成了一种双轨并存的局面。随着时间推移，越来越多的架构应当改用新模型，因为它能带来更小、更干净的 IRQ 子系统。它已被弃用三年，即将被移除。)

Known Bugs And Assumptions(已知缺陷与假设)
==========================================

None (knock on wood).
(无(敲敲木头祈好运)。)

Abstraction layers(抽象层)
==========================

There are three main levels of abstraction in the interrupt code:
(中断代码中有三个主要的抽象层次：)

1. High-level driver API(高层驱动 API)

2. High-level IRQ flow handlers(高层 IRQ 流处理器)

3. Chip-level hardware encapsulation(芯片级硬件封装)

Interrupt control flow(中断控制流)
----------------------------------

Each interrupt is described by an interrupt descriptor structure
irq_desc. The interrupt is referenced by an 'unsigned int' numeric
value which selects the corresponding interrupt description structure in
the descriptor structures array. The descriptor structure contains
status information and pointers to the interrupt flow method and the
interrupt chip structure which are assigned to this interrupt.
(每个中断由中断描述符结构 irq_desc 描述。中断通过一个 'unsigned int' 数值来引用，该数值在描述符结构数组中选择对应的中断描述结构。描述符结构包含状态信息，以及指向分配给该中断的中断流方法与中断芯片结构的指针。)

Whenever an interrupt triggers, the low-level architecture code calls
into the generic interrupt code by calling desc->handle_irq(). This
high-level IRQ handling function only uses desc->irq_data.chip
primitives referenced by the assigned chip descriptor structure.
(每当一个中断触发，底层架构代码就通过调用 desc->handle_irq() 进入通用中断代码。这个高层 IRQ 处理函数只使用由所分配芯片描述符结构引用的 desc->irq_data.chip 原语。)

High-level Driver API(高层驱动 API)
-----------------------------------

The high-level Driver API consists of following functions:
(高层驱动 API 由以下函数组成：)

-  request_irq()

-  request_threaded_irq()

-  free_irq()

-  disable_irq()

-  enable_irq()

-  disable_irq_nosync() (SMP only)

-  synchronize_irq() (SMP only)

-  irq_set_irq_type()

-  irq_set_irq_wake()

-  irq_set_handler_data()

-  irq_set_chip()

-  irq_set_chip_data()

See the autogenerated function documentation for details.
(详情参见自动生成的函数文档。)

High-level IRQ flow handlers(高层 IRQ 流处理器)
-----------------------------------------------

The generic layer provides a set of pre-defined irq-flow methods:
(通用层提供一组预定义的 irq-flow 方法：)

-  handle_level_irq()

-  handle_edge_irq()

-  handle_fasteoi_irq()

-  handle_simple_irq()

-  handle_percpu_irq()

-  handle_edge_eoi_irq()

-  handle_bad_irq()

The interrupt flow handlers (either pre-defined or architecture
specific) are assigned to specific interrupts by the architecture either
during bootup or during device initialization.
(中断流 handler(无论是预定义的还是架构特定的)由架构在启动期间或设备初始化期间分配给特定中断。)

Default flow implementations(默认流实现)
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

Helper functions(辅助函数)
^^^^^^^^^^^^^^^^^^^^^^^^^^

The helper functions call the chip primitives and are used by the
default flow implementations. The following helper functions are
implemented (simplified excerpt)::

    default_enable(struct irq_data *data)
    {
        desc->irq_data.chip->irq_unmask(data);
    }

    default_disable(struct irq_data *data)
    {
        if (!delay_disable(data))
            desc->irq_data.chip->irq_mask(data);
    }

    default_ack(struct irq_data *data)
    {
        chip->irq_ack(data);
    }

    default_mask_ack(struct irq_data *data)
    {
        if (chip->irq_mask_ack) {
            chip->irq_mask_ack(data);
        } else {
            chip->irq_mask(data);
            chip->irq_ack(data);
        }
    }

    noop(struct irq_data *data)
    {
    }

(辅助函数调用芯片原语，供默认流实现使用。已实现以下辅助函数(简化摘录)：)



Default flow handler implementations(默认流处理器实现)
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

Default Level IRQ flow handler(默认电平 IRQ 流处理器)
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

handle_level_irq provides a generic implementation for level-triggered
interrupts.
(handle_level_irq 为电平触发中断提供通用实现。)

The following control flow is implemented (simplified excerpt)::

    desc->irq_data.chip->irq_mask_ack();
    handle_irq_event(desc->action);
    desc->irq_data.chip->irq_unmask();

(实现了以下控制流(简化摘录)：)


Default Fast EOI IRQ flow handler(默认快速 EOI IRQ 流处理器)
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

handle_fasteoi_irq provides a generic implementation for interrupts,
which only need an EOI at the end of the handler.
(handle_fasteoi_irq 为只需在 handler 结束时发出 EOI 的中断提供通用实现。)

The following control flow is implemented (simplified excerpt)::

    handle_irq_event(desc->action);
    desc->irq_data.chip->irq_eoi();

(实现了以下控制流(简化摘录)：)


Default Edge IRQ flow handler(默认边沿 IRQ 流处理器)
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

handle_edge_irq provides a generic implementation for edge-triggered
interrupts.
(handle_edge_irq 为边沿触发中断提供通用实现。)

The following control flow is implemented (simplified excerpt)::

    if (desc->status & running) {
        desc->irq_data.chip->irq_mask_ack();
        desc->status |= pending | masked;
        return;
    }
    desc->irq_data.chip->irq_ack();
    desc->status |= running;
    do {
        if (desc->status & masked)
            desc->irq_data.chip->irq_unmask();
        desc->status &= ~pending;
        handle_irq_event(desc->action);
    } while (desc->status & pending);
    desc->status &= ~running;

(实现了以下控制流(简化摘录)：)


Default simple IRQ flow handler(默认简单 IRQ 流处理器)
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

handle_simple_irq provides a generic implementation for simple
interrupts.
(handle_simple_irq 为简单中断提供通用实现。)

.. note::

   The simple flow handler does not call any handler/chip primitives.
   (简单流处理器不调用任何 handler/chip 原语。)

The following control flow is implemented (simplified excerpt)::

    handle_irq_event(desc->action);

(实现了以下控制流(简化摘录)：)


Default per CPU flow handler(默认 per CPU 流处理器)
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

handle_percpu_irq provides a generic implementation for per CPU
interrupts.
(handle_percpu_irq 为 per CPU 中断提供通用实现。)

Per CPU interrupts are only available on SMP and the handler provides a
simplified version without locking.
(per CPU 中断仅在 SMP 上可用，该 handler 提供了一个不带锁的简化版本。)

The following control flow is implemented (simplified excerpt)::

    if (desc->irq_data.chip->irq_ack)
        desc->irq_data.chip->irq_ack();
    handle_irq_event(desc->action);
    if (desc->irq_data.chip->irq_eoi)
        desc->irq_data.chip->irq_eoi();

(实现了以下控制流(简化摘录)：)


EOI Edge IRQ flow handler(EOI 边沿 IRQ 流处理器)
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

handle_edge_eoi_irq provides an abnomination of the edge handler
which is solely used to tame a badly wreckaged irq controller on
powerpc/cell.
(handle_edge_eoi_irq 提供了一种边沿 handler 的怪异变体，仅用于驯服 powerpc/cell 上严重损坏的 irq 控制器。)

Bad IRQ flow handler(无效 IRQ 流处理器)
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

handle_bad_irq is used for spurious interrupts which have no real
handler assigned..
(handle_bad_irq 用于没有分配真正 handler 的伪中断(spurious interrupts)。)

Quirks and optimizations(怪癖与优化)
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

The generic functions are intended for 'clean' architectures and chips,
which have no platform-specific IRQ handling quirks. If an architecture
needs to implement quirks on the 'flow' level then it can do so by
overriding the high-level irq-flow handler.
(这些通用函数面向 "clean"(干净)的架构与芯片，即没有平台特定 IRQ 处理怪癖的架构与芯片。如果架构需要在 "flow" 层面实现怪癖，它可以通过覆盖高层 irq-flow handler 来做到。)

Delayed interrupt disable(延迟中断禁用)
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

This per interrupt selectable feature, which was introduced by Russell
King in the ARM interrupt implementation, does not mask an interrupt at
the hardware level when disable_irq() is called. The interrupt is kept
enabled and is masked in the flow handler when an interrupt event
happens. This prevents losing edge interrupts on hardware which does not
store an edge interrupt event while the interrupt is disabled at the
hardware level. When an interrupt arrives while the IRQ_DISABLED flag
is set, then the interrupt is masked at the hardware level and the
IRQ_PENDING bit is set. When the interrupt is re-enabled by
enable_irq() the pending bit is checked and if it is set, the interrupt
is resent either via hardware or by a software resend mechanism. (It's
necessary to enable CONFIG_HARDIRQS_SW_RESEND when you want to use
the delayed interrupt disable feature and your hardware is not capable
of retriggering an interrupt.) The delayed interrupt disable is not
configurable.
(这项可按中断选择的特性由 Russell King 在 ARM 中断实现中引入：当调用 disable_irq() 时，它并不在硬件层面屏蔽中断。中断保持启用状态，当中断事件发生时由流 handler 将其屏蔽。这可以防止在硬件层面禁用中断期间丢失边沿中断——这类硬件在中断被禁用期间不保存边沿中断事件。当 IRQ_DISABLED 标志已设置时若有中断到达，则中断会在硬件层面被屏蔽，并设置 IRQ_PENDING 位。当 enable_irq() 重新启用中断时，会检查 pending 位；如果已设置，则通过硬件或软件重发机制重发该中断。(若要使用延迟中断禁用特性而你的硬件又不能重新触发中断，则需要启用 CONFIG_HARDIRQS_SW_RESEND。)延迟中断禁用是不可配置的。)

Chip-level hardware encapsulation(芯片级硬件封装)
-------------------------------------------------

The chip-level hardware descriptor structure :c:type:`irq_chip` contains all
the direct chip relevant functions, which can be utilized by the irq flow
implementations.
(芯片级硬件描述符结构 :c:type:`irq_chip` 包含所有与芯片直接相关的函数，irq 流实现可以使用它们。)

-  ``irq_ack``

-  ``irq_mask_ack`` - Optional, recommended for performance(可选，出于性能考虑推荐使用)

-  ``irq_mask``

-  ``irq_unmask``

-  ``irq_eoi`` - Optional, required for EOI flow handlers(可选，EOI 流处理器必需)

-  ``irq_retrigger`` - Optional(可选)

-  ``irq_set_type`` - Optional(可选)

-  ``irq_set_wake`` - Optional(可选)

These primitives are strictly intended to mean what they say: ack means
ACK, masking means masking of an IRQ line, etc. It is up to the flow
handler(s) to use these basic units of low-level functionality.
(这些原语的含义严格按字面理解：ack 就是指 ACK，masking 就是指屏蔽一条 IRQ 线，等等。如何使用这些底层功能的基本单元，由流 handler 自行决定。)

__do_IRQ entry point(__do_IRQ 入口点)
=====================================

The original implementation __do_IRQ() was an alternative entry point
for all types of interrupts. It no longer exists.
(最初的实现 __do_IRQ() 是所有类型中断的一个替代入口点。它已不复存在。)

This handler turned out to be not suitable for all interrupt hardware
and was therefore reimplemented with split functionality for
edge/level/simple/percpu interrupts. This is not only a functional
optimization. It also shortens code paths for interrupts.
(事实证明该 handler 并不适合所有中断硬件，因此它被重新实现为按边沿/电平/简单/percpu 中断拆分功能的版本。这不仅是功能上的优化，还缩短了中断处理的代码路径。)

Locking on SMP(SMP 上的锁定)
============================

The locking of chip registers is up to the architecture that defines the
chip primitives. The per-irq structure is protected via desc->lock, by
the generic layer.
(芯片寄存器的加锁由定义芯片原语的架构负责。per-irq 结构由通用层通过 desc->lock 保护。)

Generic interrupt chip(通用中断芯片)
====================================

To avoid copies of identical implementations of IRQ chips the core
provides a configurable generic interrupt chip implementation.
Developers should check carefully whether the generic chip fits their
needs before implementing the same functionality slightly differently
themselves.
(为了避免重复实现完全相同的 IRQ 芯片，核心提供了可配置的通用中断芯片实现。开发者在决定自己以略有不同的方式实现相同功能之前，应当仔细检查通用芯片是否满足其需求。)

.. kernel-doc:: kernel/irq/generic-chip.c
   :export:

Structures(结构体)
==================

This chapter contains the autogenerated documentation of the structures
which are used in the generic IRQ layer.
(本章包含通用 IRQ 层所使用结构的自动生成文档。)

.. kernel-doc:: include/linux/irq.h
   :internal:

.. kernel-doc:: include/linux/interrupt.h
   :internal:

Public Functions Provided(提供的公共函数)
=========================================

This chapter contains the autogenerated documentation of the kernel API
functions which are exported.
(本章包含已导出的内核 API 函数的自动生成文档。)

.. kernel-doc:: kernel/irq/manage.c

.. kernel-doc:: kernel/irq/chip.c
   :export:

Internal Functions Provided(提供的内部函数)
===========================================

This chapter contains the autogenerated documentation of the internal
functions.
(本章包含内部函数的自动生成文档。)

.. kernel-doc:: kernel/irq/irqdesc.c

.. kernel-doc:: kernel/irq/handle.c

.. kernel-doc:: kernel/irq/chip.c
   :internal:

Credits(致谢)
=============

The following people have contributed to this document:
(以下人员为本文件做出了贡献：)

1. Thomas Gleixner tglx@kernel.org

2. Ingo Molnar mingo@elte.hu
