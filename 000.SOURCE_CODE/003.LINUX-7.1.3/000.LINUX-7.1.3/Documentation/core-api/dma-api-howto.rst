============================================
Dynamic DMA mapping Guide(动态 DMA 映射指南)
============================================

:Author: David S. Miller <davem@redhat.com>
  (作者: David S. Miller <davem@redhat.com>)
:Author: Richard Henderson <rth@cygnus.com>
  (作者: Richard Henderson <rth@cygnus.com>)
:Author: Jakub Jelinek <jakub@redhat.com>
  (作者: Jakub Jelinek <jakub@redhat.com>)

This is a guide to device driver writers on how to use the DMA API
with example pseudo-code.  For a concise description of the API, see
Documentation/core-api/dma-api.rst.
(本文是面向设备驱动编写者的 DMA API 使用指南，附有示例伪代码。关于该 API 的简明描述，见 Documentation/core-api/dma-api.rst。)

CPU and DMA addresses(CPU 与 DMA 地址)
======================================

There are several kinds of addresses involved in the DMA API, and it's
important to understand the differences.
(DMA API 中涉及好几种地址，理解它们之间的区别非常重要。)

The kernel normally uses virtual addresses.  Any address returned by
kmalloc(), vmalloc(), and similar interfaces is a virtual address and can
be stored in a ``void *``.
(内核通常使用虚拟地址。kmalloc()、vmalloc() 及类似接口返回的任何地址都是虚拟地址，可以保存在 ``void *`` 中。)

The virtual memory system (TLB, page tables, etc.) translates virtual
addresses to CPU physical addresses, which are stored as "phys_addr_t" or
"resource_size_t".  The kernel manages device resources like registers as
physical addresses.  These are the addresses in /proc/iomem.  The physical
address is not directly useful to a driver; it must use ioremap() to map
the space and produce a virtual address.
(虚拟内存系统(TLB、页表等)将虚拟地址翻译为 CPU 物理地址，后者以 "phys_addr_t" 或 "resource_size_t" 保存。内核以物理地址的形式管理寄存器等设备资源。这些就是 /proc/iomem 中的地址。物理地址对驱动没有直接用处；驱动必须用 ioremap() 将该空间映射出来，得到一个虚拟地址。)

I/O devices use a third kind of address: a "bus address".  If a device has
registers at an MMIO address, or if it performs DMA to read or write system
memory, the addresses used by the device are bus addresses.  In some
systems, bus addresses are identical to CPU physical addresses, but in
general they are not.  IOMMUs and host bridges can produce arbitrary
mappings between physical and bus addresses.
(I/O 设备使用第三种地址："总线地址"。如果设备的寄存器位于某个 MMIO 地址，或者它通过 DMA 读写系统内存，设备所使用的地址就是总线地址。在某些系统中，总线地址与 CPU 物理地址相同，但一般并不同。IOMMU 和主机桥可以在物理地址与总线地址之间产生任意的映射。)

From a device's point of view, DMA uses the bus address space, but it may
be restricted to a subset of that space.  For example, even if a system
supports 64-bit addresses for main memory and PCI BARs, it may use an IOMMU
so devices only need to use 32-bit DMA addresses.
(从设备的角度看，DMA 使用的是总线地址空间，但可能被限制为该空间的一个子集。例如，即使系统对主存和 PCI BAR 支持 64 位地址，它也可能使用 IOMMU，使设备只需使用 32 位 DMA 地址。)

Here's a picture and some examples::

               CPU                  CPU                  Bus
             Virtual              Physical             Address
             Address              Address               Space
              Space                Space

            +-------+             +------+             +------+
            |       |             |MMIO  |   Offset    |      |
            |       |  Virtual    |Space |   applied   |      |
          C +-------+ --------> B +------+ ----------> +------+ A
            |       |  mapping    |      |   by host   |      |
  +-----+   |       |             |      |   bridge    |      |   +--------+
  |     |   |       |             +------+             |      |   |        |
  | CPU |   |       |             | RAM  |             |      |   | Device |
  |     |   |       |             |      |             |      |   |        |
  +-----+   +-------+             +------+             +------+   +--------+
            |       |  Virtual    |Buffer|   Mapping   |      |
          X +-------+ --------> Y +------+ <---------- +------+ Z
            |       |  mapping    | RAM  |   by IOMMU
            |       |             |      |
            |       |             |      |
            +-------+             +------+

(下面是一张示意图和几个例子：)

During the enumeration process, the kernel learns about I/O devices and
their MMIO space and the host bridges that connect them to the system.  For
example, if a PCI device has a BAR, the kernel reads the bus address (A)
from the BAR and converts it to a CPU physical address (B).  The address B
is stored in a struct resource and usually exposed via /proc/iomem.  When a
driver claims a device, it typically uses ioremap() to map physical address
B at a virtual address (C).  It can then use, e.g., ioread32(C), to access
the device registers at bus address A.
(在枚举过程中，内核会了解到 I/O 设备、它们的 MMIO 空间，以及把它们连接到系统的主机桥。例如，如果 PCI 设备有一个 BAR，内核会从 BAR 中读出总线地址 (A)，并将其转换为 CPU 物理地址 (B)。地址 B 保存在 struct resource 中，通常通过 /proc/iomem 暴露。当驱动认领一个设备时，它通常使用 ioremap() 把物理地址 B 映射到某个虚拟地址 (C)。之后它就可以用例如 ioread32(C) 来访问总线地址 A 处的设备寄存器。)

If the device supports DMA, the driver sets up a buffer using kmalloc() or
a similar interface, which returns a virtual address (X).  The virtual
memory system maps X to a physical address (Y) in system RAM.  The driver
can use virtual address X to access the buffer, but the device itself
cannot because DMA doesn't go through the CPU virtual memory system.
(如果设备支持 DMA，驱动会用 kmalloc() 或类似接口建立一个缓冲区，它返回一个虚拟地址 (X)。虚拟内存系统把 X 映射到系统 RAM 中的物理地址 (Y)。驱动可以用虚拟地址 X 访问该缓冲区，但设备本身不行，因为 DMA 不经过 CPU 的虚拟内存系统。)

In some simple systems, the device can do DMA directly to physical address
Y.  But in many others, there is IOMMU hardware that translates DMA
addresses to physical addresses, e.g., it translates Z to Y.  This is part
of the reason for the DMA API: the driver can give a virtual address X to
an interface like dma_map_single(), which sets up any required IOMMU
mapping and returns the DMA address Z.  The driver then tells the device to
do DMA to Z, and the IOMMU maps it to the buffer at address Y in system
RAM.
(在一些简单的系统中，设备可以直接对物理地址 Y 做 DMA。但在许多其他系统中，存在把 DMA 地址翻译为物理地址的 IOMMU 硬件，例如把 Z 翻译为 Y。这正是 DMA API 存在的部分原因：驱动可以把虚拟地址 X 交给 dma_map_single() 之类的接口，由它建立所需的 IOMMU 映射并返回 DMA 地址 Z。随后驱动让设备对 Z 做 DMA，IOMMU 会把它映射到系统 RAM 中位于地址 Y 的缓冲区。)

So that Linux can use the dynamic DMA mapping, it needs some help from the
drivers, namely it has to take into account that DMA addresses should be
mapped only for the time they are actually used and unmapped after the DMA
transfer.
(为了让 Linux 能够使用动态 DMA 映射，需要驱动的一些配合，即必须考虑到：DMA 地址只应在实际使用期间保持映射，并在 DMA 传输结束后解除映射。)

The following API will work of course even on platforms where no such
hardware exists.
(当然，即使在没有这类硬件的平台上，下面描述的 API 也照常可用。)

Note that the DMA API works with any bus independent of the underlying
microprocessor architecture. You should use the DMA API rather than the
bus-specific DMA API, i.e., use the dma_map_*() interfaces rather than the
pci_map_*() interfaces.
(注意，DMA API 与底层微处理器架构无关，适用于任何总线。你应当使用 DMA API 而不是总线特定的 DMA API，即使用 dma_map_*() 接口而不是 pci_map_*() 接口。)

First of all, you should make sure::

	#include <linux/dma-mapping.h>

(首先，你应当确保驱动中包含以下头文件：)

is in your driver, which provides the definition of dma_addr_t.  This type
can hold any valid DMA address for the platform and should be used
everywhere you hold a DMA address returned from the DMA mapping functions.
(它为你的驱动提供 dma_addr_t 的定义。该类型可以保存平台上任何有效的 DMA 地址，凡是保存 DMA 映射函数返回的 DMA 地址的地方都应使用它。)

What memory is DMA'able?(什么样的内存可用于 DMA？)
==================================================

The first piece of information you must know is what kernel memory can
be used with the DMA mapping facilities.  There has been an unwritten
set of rules regarding this, and this text is an attempt to finally
write them down.
(你必须知道的第一件事是，哪些内核内存可以用于 DMA 映射机制。这方面一直存在一套不成文的规则，本文试图最终把它们写下来。)

If you acquired your memory via the page allocator
(i.e. __get_free_page*()) or the generic memory allocators
(i.e. kmalloc() or kmem_cache_alloc()) then you may DMA to/from
that memory using the addresses returned from those routines.
(如果你的内存来自页分配器(即 __get_free_page*())或通用内存分配器(即 kmalloc() 或 kmem_cache_alloc())，那么你可以使用这些例程返回的地址对该内存进行 DMA。)

This means specifically that you may _not_ use the memory/addresses
returned from vmalloc() for DMA.  It is possible to DMA to the
_underlying_ memory mapped into a vmalloc() area, but this requires
walking page tables to get the physical addresses, and then
translating each of those pages back to a kernel address using
something like __va().  [ EDIT: Update this when we integrate
Gerd Knorr's generic code which does this. ]
(具体来说，你不得将 vmalloc() 返回的内存/地址用于 DMA。虽然可以对映射进 vmalloc() 区域的底层内存做 DMA，但这需要遍历页表来获取物理地址，然后再用 __va() 之类的办法把每个页转换回内核地址。[ EDIT: 待集成 Gerd Knorr 实现此功能的通用代码后更新本段。 ])

This rule also means that you may use neither kernel image addresses
(items in data/text/bss segments), nor module image addresses, nor
stack addresses for DMA.  These could all be mapped somewhere entirely
different than the rest of physical memory.  Even if those classes of
memory could physically work with DMA, you'd need to ensure the I/O
buffers were cacheline-aligned.  Without that, you'd see cacheline
sharing problems (data corruption) on CPUs with DMA-incoherent caches.
(The CPU could write to one word, DMA would write to a different one
in the same cache line, and one of them could be overwritten.)
(这条规则还意味着，内核映像地址(data/text/bss 段中的内容)、模块映像地址以及栈地址都不能用于 DMA。它们可能被映射到与物理内存其余部分完全不同的地方。即使这些类别的内存物理上能与 DMA 协同工作，你也需要确保 I/O 缓冲区按缓存行对齐。否则，在缓存与 DMA 不一致的 CPU 上会遇到缓存行共享问题(数据损坏)。(CPU 可能写入一个字，而 DMA 写入同一缓存行中的另一个字，其中一个的写入可能被覆盖。))

Also, this means that you cannot take the return of a kmap()
call and DMA to/from that.  This is similar to vmalloc().
(同样，这意味着你不能拿 kmap() 调用的返回值来对其做 DMA。这一点与 vmalloc() 类似。)

What about block I/O and networking buffers?  The block I/O and
networking subsystems make sure that the buffers they use are valid
for you to DMA from/to.
(那块 I/O 和网络缓冲区呢？块 I/O 子系统和网络子系统会确保它们使用的缓冲区可供你安全地进行 DMA 读写。)

__dma_from_device_group_begin/end annotations(__dma_from_device_group_begin/end 注解)
=====================================================================================

As explained previously, when a structure contains a DMA_FROM_DEVICE /
DMA_BIDIRECTIONAL buffer (device writes to memory) alongside fields that the
CPU writes to, cache line sharing between the DMA buffer and CPU-written fields
can cause data corruption on CPUs with DMA-incoherent caches.
(如前所述，当结构体中同时存在 DMA_FROM_DEVICE / DMA_BIDIRECTIONAL 缓冲区(由设备写入内存)和 CPU 写入的字段时，在缓存与 DMA 不一致的 CPU 上，DMA 缓冲区与 CPU 写入字段之间的缓存行共享可能导致数据损坏。)

The ``__dma_from_device_group_begin(GROUP)/__dma_from_device_group_end(GROUP)``
macros ensure proper alignment to prevent this::

	struct my_device {
		spinlock_t lock1;
		__dma_from_device_group_begin();
		char dma_buffer1[16];
		char dma_buffer2[16];
		__dma_from_device_group_end();
		spinlock_t lock2;
	};

(``__dma_from_device_group_begin(GROUP)/__dma_from_device_group_end(GROUP)`` 宏通过确保适当的对齐来防止这一问题：)

To isolate a DMA buffer from adjacent fields, use
``__dma_from_device_group_begin(GROUP)`` before the first DMA buffer
field and ``__dma_from_device_group_end(GROUP)`` after the last DMA
buffer field (with the same GROUP name). This protects both the head
and tail of the buffer from cache line sharing.
(要把 DMA 缓冲区与相邻字段隔离开，可在第一个 DMA 缓冲区字段之前使用 ``__dma_from_device_group_begin(GROUP)``，并在最后一个 DMA 缓冲区字段之后使用 ``__dma_from_device_group_end(GROUP)`` (GROUP 名称须相同)。这样可以保护缓冲区的头部和尾部都不受缓存行共享的影响。)

The GROUP parameter is an optional identifier that names the DMA buffer group
(in case you have several in the same structure)::

	struct my_device {
		spinlock_t lock1;
		__dma_from_device_group_begin(buffer1);
		char dma_buffer1[16];
		__dma_from_device_group_end(buffer1);
		spinlock_t lock2;
		__dma_from_device_group_begin(buffer2);
		char dma_buffer2[16];
		__dma_from_device_group_end(buffer2);
	};

(GROUP 参数是一个可选的标识符，用于给 DMA 缓冲区组命名(以便在同一个结构体中存在多组时加以区分)：)

On cache-coherent platforms these macros expand to zero-length array markers.
On non-coherent platforms, they also ensure the minimal DMA alignment, which
can be as large as 128 bytes.
(在缓存一致的平台上，这些宏展开为零长度数组标记。在非一致的平台上，它们还会保证最小的 DMA 对齐，该对齐可能高达 128 字节。)

.. note::

        It is allowed (though somewhat fragile) to include extra fields, not
        intended for DMA from the device, within the group (in order to pack the
        structure tightly) - but only as long as the CPU does not write these
        fields while any fields in the group are mapped for DMA_FROM_DEVICE or
        DMA_BIDIRECTIONAL.
        (允许(尽管有些脆弱)在组内包含并非用于设备 DMA 的额外字段(以便紧凑地排列结构体)——但前提是，只要组内有任何字段被映射为 DMA_FROM_DEVICE 或 DMA_BIDIRECTIONAL，CPU 就不得写入这些额外字段。)

DMA addressing capabilities(DMA 寻址能力)
=========================================

By default, the kernel assumes that your device can address 32-bits of DMA
addressing.  For a 64-bit capable device, this needs to be increased, and for
a device with limitations, it needs to be decreased.
(默认情况下，内核假定你的设备只能进行 32 位 DMA 寻址。对于支持 64 位的设备，需要调大该值；而有限制的设备，则需要调小。)

Special note about PCI: PCI-X specification requires PCI-X devices to support
64-bit addressing (DAC) for all transactions.  And at least one platform (SGI
SN2) requires 64-bit coherent allocations to operate correctly when the IO
bus is in PCI-X mode.
(关于 PCI 的特别说明：PCI-X 规范要求 PCI-X 设备在所有事务中都支持 64 位寻址(DAC)。而且至少有一个平台(SGI SN2)在 IO 总线处于 PCI-X 模式时，要求 64 位一致性分配才能正确工作。)

For correct operation, you must set the DMA mask to inform the kernel about
your devices DMA addressing capabilities.
(为了正确工作，你必须设置 DMA 掩码，把设备的 DMA 寻址能力告知内核。)

This is performed via a call to dma_set_mask_and_coherent()::

	int dma_set_mask_and_coherent(struct device *dev, u64 mask);

(这通过调用 dma_set_mask_and_coherent() 来完成：)

which will set the mask for both streaming and coherent APIs together.  If you
have some special requirements, then the following two separate calls can be
used instead:
(它会把流式与一致性两种 API 的掩码一并设置。如果你有特殊需求，也可以改用下面两个单独的调用：)

	The setup for streaming mappings is performed via a call to
	dma_set_mask()::

		int dma_set_mask(struct device *dev, u64 mask);

	(流式映射的设置通过调用 dma_set_mask() 完成：)

	The setup for coherent allocations is performed via a call
	to dma_set_coherent_mask()::

		int dma_set_coherent_mask(struct device *dev, u64 mask);

	(一致性分配的设置通过调用 dma_set_coherent_mask() 完成：)

Here, dev is a pointer to the device struct of your device, and mask is a bit
mask describing which bits of an address your device supports.  Often the
device struct of your device is embedded in the bus-specific device struct of
your device.  For example, &pdev->dev is a pointer to the device struct of a
PCI device (pdev is a pointer to the PCI device struct of your device).
(这里，dev 是指向你设备的 device 结构体的指针，mask 是描述设备支持地址哪些位的位掩码。通常，设备的 device 结构体嵌在特定总线设备结构体中。例如，&pdev->dev 是指向 PCI 设备 device 结构体的指针(pdev 是指向你设备的 PCI 设备结构体的指针)。)

These calls usually return zero to indicate your device can perform DMA
properly on the machine given the address mask you provided, but they might
return an error if the mask is too small to be supportable on the given
system.  If it returns non-zero, your device cannot perform DMA properly on
this platform, and attempting to do so will result in undefined behavior.
You must not use DMA on this device unless the dma_set_mask family of
functions has returned success.
(在给定你所提供的地址掩码的前提下，这些调用通常返回零，表示你的设备在该机器上可以正常执行 DMA；但如果掩码太小、给定系统无法支持，它们也可能返回错误。如果返回非零值，你的设备在该平台上无法正常执行 DMA，强行尝试将导致未定义行为。除非 dma_set_mask 系列函数已返回成功，否则你不得在该设备上使用 DMA。)

This means that in the failure case, you have two options:
(这意味着在失败的情况下，你有两个选择：)

1) Use some non-DMA mode for data transfer, if possible.
   (如果可能，改用某种非 DMA 方式进行数据传输。)
2) Ignore this device and do not initialize it.
   (忽略该设备，不对其进行初始化。)

It is recommended that your driver print a kernel KERN_WARNING message when
setting the DMA mask fails.  In this manner, if a user of your driver reports
that performance is bad or that the device is not even detected, you can ask
them for the kernel messages to find out exactly why.
(建议驱动在设置 DMA 掩码失败时打印一条内核 KERN_WARNING 消息。这样，当用户报告驱动性能不佳或设备根本检测不到时，你可以向他们索要内核消息来查明确切原因。)

The 24-bit addressing device would do something like this::

	if (dma_set_mask_and_coherent(dev, DMA_BIT_MASK(24))) {
		dev_warn(dev, "mydev: No suitable DMA available\n");
		goto ignore_this_device;
	}

(24 位寻址的设备大概会这样写：)

The standard 64-bit addressing device would do something like this::

	dma_set_mask_and_coherent(dev, DMA_BIT_MASK(64))

(标准的 64 位寻址设备大概会这样写：)

dma_set_mask_and_coherent() never return fail when DMA_BIT_MASK(64). Typical
error code like::

	/* Wrong code */
	if (dma_set_mask_and_coherent(dev, DMA_BIT_MASK(64)))
		dma_set_mask_and_coherent(dev, DMA_BIT_MASK(32))

(dma_set_mask_and_coherent() 在 DMA_BIT_MASK(64) 时从不返回失败。典型的错误代码如：)

dma_set_mask_and_coherent() will never return failure when bigger than 32.
So typical code like::

	/* Recommended code */
	if (support_64bit)
		dma_set_mask_and_coherent(dev, DMA_BIT_MASK(64));
	else
		dma_set_mask_and_coherent(dev, DMA_BIT_MASK(32));

(dma_set_mask_and_coherent() 在大于 32 位时从不返回失败。因此典型代码如：)

If the device only supports 32-bit addressing for descriptors in the
coherent allocations, but supports full 64-bits for streaming mappings
it would look like this::

	if (dma_set_mask(dev, DMA_BIT_MASK(64))) {
		dev_warn(dev, "mydev: No suitable DMA available\n");
		goto ignore_this_device;
	}

(如果设备在一致性分配中只支持描述符的 32 位寻址，但流式映射支持完整的 64 位，则代码会是这样：)

The coherent mask will always be able to set the same or a smaller mask as
the streaming mask. However for the rare case that a device driver only
uses coherent allocations, one would have to check the return value from
dma_set_coherent_mask().
(一致性掩码总是可以设置为与流式掩码相同或更小的掩码。不过，在驱动只使用一致性分配的罕见情况下，就必须检查 dma_set_coherent_mask() 的返回值。)

Finally, if your device can only drive the low 24-bits of
address you might do something like::

	if (dma_set_mask(dev, DMA_BIT_MASK(24))) {
		dev_warn(dev, "mydev: 24-bit DMA addressing not available\n");
		goto ignore_this_device;
	}

(最后，如果你的设备只能驱动地址的低 24 位，你可能会这样写：)

When dma_set_mask() or dma_set_mask_and_coherent() is successful, and
returns zero, the kernel saves away this mask you have provided.  The
kernel will use this information later when you make DMA mappings.
(当 dma_set_mask() 或 dma_set_mask_and_coherent() 成功并返回零时，内核会保存你提供的这个掩码。之后进行 DMA 映射时，内核会使用这一信息。)

There is a case which we are aware of at this time, which is worth
mentioning in this documentation.  If your device supports multiple
functions (for example a sound card provides playback and record
functions) and the various different functions have _different_
DMA addressing limitations, you may wish to probe each mask and
only provide the functionality which the machine can handle.  It
is important that the last call to dma_set_mask() be for the
most specific mask.
(目前我们知道有一种情况值得一提。如果你的设备支持多种功能(例如声卡提供播放和录音功能)，而不同功能有 _不同的_ DMA 寻址限制，你可能希望逐一探测每个掩码，只提供机器能够支持的功能。重要的是，对 dma_set_mask() 的最后一次调用必须是针对最具体的掩码。)

Here is pseudo-code showing how this might be done::

	#define PLAYBACK_ADDRESS_BITS	DMA_BIT_MASK(32)
	#define RECORD_ADDRESS_BITS	DMA_BIT_MASK(24)

	struct my_sound_card *card;
	struct device *dev;

	...
	if (!dma_set_mask(dev, PLAYBACK_ADDRESS_BITS)) {
		card->playback_enabled = 1;
	} else {
		card->playback_enabled = 0;
		dev_warn(dev, "%s: Playback disabled due to DMA limitations\n",
		       card->name);
	}
	if (!dma_set_mask(dev, RECORD_ADDRESS_BITS)) {
		card->record_enabled = 1;
	} else {
		card->record_enabled = 0;
		dev_warn(dev, "%s: Record disabled due to DMA limitations\n",
		       card->name);
	}

(下面是展示这一做法的伪代码：)

A sound card was used as an example here because this genre of PCI
devices seems to be littered with ISA chips given a PCI front end,
and thus retaining the 16MB DMA addressing limitations of ISA.
(这里以声卡为例，是因为这一类 PCI 设备中似乎充斥着套着 PCI 外壳的 ISA 芯片，因而保留了 ISA 的 16MB DMA 寻址限制。)

Types of DMA mappings(DMA 映射的类型)
=====================================

There are two types of DMA mappings:
(DMA 映射有两种类型：)

- Coherent DMA mappings which are usually mapped at driver
  initialization, unmapped at the end and for which the hardware should
  guarantee that the device and the CPU can access the data
  in parallel and will see updates made by each other without any
  explicit software flushing.
  (一致性 DMA 映射：通常在驱动初始化时建立映射、在结束时解除映射，硬件应保证设备与 CPU 可以并行访问数据，并且无需任何显式的软件刷新就能看到彼此的更新。)

  Think of "coherent" as "synchronous".
  (可以把 "coherent(一致性)" 理解为 "synchronous(同步)"。)

  The current default is to return coherent memory in the low 32
  bits of the DMA space.  However, for future compatibility you should
  set the coherent mask even if this default is fine for your
  driver.
  (当前的默认做法是在 DMA 空间的低 32 位中返回一致性内存。不过，为了面向未来的兼容性，即使这个默认值对你的驱动没有问题，也应当设置一致性掩码。)

  Good examples of what to use coherent mappings for are:
  (适合使用一致性映射的典型例子有：)

	- Network card DMA ring descriptors.
	- SCSI adapter mailbox command data structures.
	- Device firmware microcode executed out of
	  main memory.
	  (网卡 DMA 环形描述符。)
	  (SCSI 适配器 mailbox 命令数据结构。)
	  (从主存中执行的设备固件微码。)

  The invariant these examples all require is that any CPU store
  to memory is immediately visible to the device, and vice
  versa.  Coherent mappings guarantee this.
  (这些例子共同要求的不变式是：CPU 对内存的任何存储都立即对设备可见，反之亦然。一致性映射保证了这一点。)

  .. important::

	     Coherent DMA memory does not preclude the usage of
	     proper memory barriers.  The CPU may reorder stores to
	     coherent memory just as it may normal memory.  Example:
	     if it is important for the device to see the first word
	     of a descriptor updated before the second, you must do
	     something like::

		desc->word0 = address;
		wmb();
		desc->word1 = DESC_VALID;

             (一致性 DMA 内存并不意味着可以省去适当的内存屏障。CPU 对一致性内存的存储可能与对普通内存一样进行重排。例如：如果必须让设备先看到描述符第一个字的更新、再看到第二个字的更新，你就必须这样做：)

             in order to get correct behavior on all platforms.
             (这样才能在所有平台上获得正确的行为。)

	     Also, on some platforms your driver may need to flush CPU write
	     buffers in much the same way as it needs to flush write buffers
	     found in PCI bridges (such as by reading a register's value
	     after writing it).
	     (此外，在某些平台上，驱动可能需要刷新 CPU 写缓冲，其做法与刷新 PCI 桥中的写缓冲非常类似(例如在写入寄存器之后再读回它的值)。)

- Streaming DMA mappings which are usually mapped for one DMA
  transfer, unmapped right after it (unless you use dma_sync_* below)
  and for which hardware can optimize for sequential accesses.
  (流式 DMA 映射：通常为一次 DMA 传输建立映射、传输完成后立即解除映射(除非使用下文的 dma_sync_*)，硬件可以针对顺序访问进行优化。)

  Think of "streaming" as "asynchronous" or "outside the coherency
  domain".
  (可以把 "streaming(流式)" 理解为 "asynchronous(异步)" 或 "一致性域之外"。)

  Good examples of what to use streaming mappings for are:
  (适合使用流式映射的典型例子有：)

	- Networking buffers transmitted/received by a device.
	- Filesystem buffers written/read by a SCSI device.
	  (设备发送/接收的网络缓冲区。)
	  (SCSI 设备写入/读取的文件系统缓冲区。)

  The interfaces for using this type of mapping were designed in
  such a way that an implementation can make whatever performance
  optimizations the hardware allows.  To this end, when using
  such mappings you must be explicit about what you want to happen.
  (使用这类映射的接口在设计上允许实现充分利用硬件所允许的各种性能优化。因此，使用这类映射时，你必须明确表达自己想要发生什么。)

Neither type of DMA mapping has alignment restrictions that come from
the underlying bus, although some devices may have such restrictions.
Also, systems with caches that aren't DMA-coherent will work better
when the underlying buffers don't share cache lines with other data.
(这两种 DMA 映射都没有来自底层总线的对齐限制，尽管某些设备自身可能有此类限制。此外，在缓存与 DMA 不一致的系统中，如果底层缓冲区不与其他数据共享缓存行，工作效果会更好。)


Using Coherent DMA mappings(使用一致性 DMA 映射)
================================================

To allocate and map large (PAGE_SIZE or so) coherent DMA regions,
you should do::

	dma_addr_t dma_handle;

	cpu_addr = dma_alloc_coherent(dev, size, &dma_handle, gfp);

(要分配并映射较大(约为一页 PAGE_SIZE)的一致性 DMA 区域，应当这样写：)

where device is a ``struct device *``. This may be called in interrupt
context with the GFP_ATOMIC flag.
(其中 device 是一个 ``struct device *``。在携带 GFP_ATOMIC 标志时，该调用可在中断上下文中使用。)

Size is the length of the region you want to allocate, in bytes.
(size 是要分配的区域长度，以字节为单位。)

This routine will allocate RAM for that region, so it acts similarly to
__get_free_pages() (but takes size instead of a page order).  If your
driver needs regions sized smaller than a page, you may prefer using
the dma_pool interface, described below.
(该例程会为该区域分配 RAM，因此其行为与 __get_free_pages() 类似(但接受的是 size 而不是页阶)。如果你的驱动需要小于一页的区域，可能更愿意使用下文介绍的 dma_pool 接口。)

The coherent DMA mapping interfaces, will by default return a DMA address
which is 32-bit addressable.  Even if the device indicates (via the DMA mask)
that it may address the upper 32-bits, coherent allocation will only
return > 32-bit addresses for DMA if the coherent DMA mask has been
explicitly changed via dma_set_coherent_mask().  This is true of the
dma_pool interface as well.
(一致性 DMA 映射接口默认返回 32 位可寻址的 DMA 地址。即使设备通过 DMA 掩码表明它可以寻址高 32 位，一致性分配也只有在一致性 DMA 掩码已通过 dma_set_coherent_mask() 显式修改的情况下，才会返回大于 32 位的 DMA 地址。dma_pool 接口也是如此。)

dma_alloc_coherent() returns two values: the virtual address which you
can use to access it from the CPU and dma_handle which you pass to the
card.
(dma_alloc_coherent() 返回两个值：你可以从 CPU 侧访问内存所用的虚拟地址，以及传递给卡的 dma_handle。)

The CPU virtual address and the DMA address are both
guaranteed to be aligned to the smallest PAGE_SIZE order which
is greater than or equal to the requested size.  This invariant
exists (for example) to guarantee that if you allocate a chunk
which is smaller than or equal to 64 kilobytes, the extent of the
buffer you receive will not cross a 64K boundary.
(CPU 虚拟地址与 DMA 地址都保证按大于等于所请求大小的最小 PAGE_SIZE 阶对齐。这个不变式(例如)是为了保证：如果你分配一块小于等于 64 千字节的内存，所得到的缓冲区范围不会跨越 64K 边界。)

To unmap and free such a DMA region, you call::

	dma_free_coherent(dev, size, cpu_addr, dma_handle);

(要解除映射并释放这样的 DMA 区域，调用：)

where dev, size are the same as in the above call and cpu_addr and
dma_handle are the values dma_alloc_coherent() returned to you.
This function may not be called in interrupt context.
(其中 dev、size 与上面的调用相同，cpu_addr 和 dma_handle 是 dma_alloc_coherent() 返回给你的值。该函数不得在中断上下文中调用。)

If your driver needs lots of smaller memory regions, you can write
custom code to subdivide pages returned by dma_alloc_coherent(),
or you can use the dma_pool API to do that.  A dma_pool is like
a kmem_cache, but it uses dma_alloc_coherent(), not __get_free_pages().
Also, it understands common hardware constraints for alignment,
like queue heads needing to be aligned on N byte boundaries.
(如果你的驱动需要许多更小的内存区域，可以自行编写代码来细分 dma_alloc_coherent() 返回的页，也可以使用 dma_pool API 来完成。dma_pool 类似于 kmem_cache，但它使用的是 dma_alloc_coherent() 而不是 __get_free_pages()。此外，它还理解常见的硬件对齐约束，例如队列头需要按 N 字节边界对齐。)

Create a dma_pool like this::

	struct dma_pool *pool;

	pool = dma_pool_create(name, dev, size, align, boundary);

(像这样创建一个 dma_pool：)

The "name" is for diagnostics (like a kmem_cache name); dev and size
are as above.  The device's hardware alignment requirement for this
type of data is "align" (which is expressed in bytes, and must be a
power of two).  If your device has no boundary crossing restrictions,
pass 0 for boundary; passing 4096 says memory allocated from this pool
must not cross 4KByte boundaries (but at that time it may be better to
use dma_alloc_coherent() directly instead).
("name" 用于诊断(类似 kmem_cache 的名字)；dev 和 size 与上文相同。设备的硬件对齐要求为 "align"(以字节表示，且必须是 2 的幂)。如果你的设备没有跨边界限制，boundary 传 0；传 4096 表示从这个池分配的内存不得跨越 4K 字节边界(但那种情况下，或许直接改用 dma_alloc_coherent() 更好)。)

Allocate memory from a DMA pool like this::

	cpu_addr = dma_pool_alloc(pool, flags, &dma_handle);

(像这样从 DMA 池中分配内存：)

flags are GFP_KERNEL if blocking is permitted (not in_interrupt nor
holding SMP locks), GFP_ATOMIC otherwise.  Like dma_alloc_coherent(),
this returns two values, cpu_addr and dma_handle.
(如果允许阻塞(不在中断中且未持有 SMP 锁)，flags 为 GFP_KERNEL，否则为 GFP_ATOMIC。与 dma_alloc_coherent() 一样，它返回两个值：cpu_addr 和 dma_handle。)

Free memory that was allocated from a dma_pool like this::

	dma_pool_free(pool, cpu_addr, dma_handle);

(像这样释放先前从 dma_pool 分配的内存：)

where pool is what you passed to dma_pool_alloc(), and cpu_addr and
dma_handle are the values dma_pool_alloc() returned. This function
may be called in interrupt context.
(其中 pool 是你传给 dma_pool_alloc() 的值，cpu_addr 和 dma_handle 是 dma_pool_alloc() 返回的值。该函数可以在中断上下文中调用。)

Destroy a dma_pool by calling::

	dma_pool_destroy(pool);

(调用以下函数销毁一个 dma_pool：)

Make sure you've called dma_pool_free() for all memory allocated
from a pool before you destroy the pool. This function may not
be called in interrupt context.
(销毁池之前，请确保对从该池分配的所有内存都调用过 dma_pool_free()。该函数不得在中断上下文中调用。)

DMA Direction(DMA 方向)
=======================

The interfaces described in subsequent portions of this document
take a DMA direction argument, which is an integer and takes on
one of the following values::

 DMA_BIDIRECTIONAL
 DMA_TO_DEVICE
 DMA_FROM_DEVICE
 DMA_NONE

(本文后续部分描述的接口都接受一个 DMA 方向参数，它是一个整数，取以下值之一：)

You should provide the exact DMA direction if you know it.
(如果你知道确切的 DMA 方向，就应提供准确的方向。)

DMA_TO_DEVICE means "from main memory to the device"
DMA_FROM_DEVICE means "from the device to main memory"
It is the direction in which the data moves during the DMA
transfer.
(DMA_TO_DEVICE 表示"从主存到设备"；DMA_FROM_DEVICE 表示"从设备到主存"。它就是 DMA 传输过程中数据移动的方向。)

You are _strongly_ encouraged to specify this as precisely
as you possibly can.
(强烈建议你尽可能准确地指定该方向。)

If you absolutely cannot know the direction of the DMA transfer,
specify DMA_BIDIRECTIONAL.  It means that the DMA can go in
either direction.  The platform guarantees that you may legally
specify this, and that it will work, but this may be at the
cost of performance for example.
(如果你完全无法知道 DMA 传输的方向，就指定 DMA_BIDIRECTIONAL。它表示 DMA 可以朝任一方向进行。平台保证你可以合法地这样指定并且能够工作，但代价可能是性能等方面的损失。)

The value DMA_NONE is to be used for debugging.  One can
hold this in a data structure before you come to know the
precise direction, and this will help catch cases where your
direction tracking logic has failed to set things up properly.
(DMA_NONE 的用途是调试。在你得知准确方向之前，可以先在数据结构中保存它，这有助于发现方向跟踪逻辑未能正确设置的情况。)

Another advantage of specifying this value precisely (outside of
potential platform-specific optimizations of such) is for debugging.
Some platforms actually have a write permission boolean which DMA
mappings can be marked with, much like page protections in the user
program address space.  Such platforms can and do report errors in the
kernel logs when the DMA controller hardware detects violation of the
permission setting.
(准确指定该值的另一个好处(除此之外还可能有平台特定的优化)是便于调试。某些平台实际上带有一个写权限布尔标记，可以标注在 DMA 映射上，非常类似于用户程序地址空间中的页保护。当 DMA 控制器硬件检测到违反权限设置时，这类平台能够并且确实会在内核日志中报告错误。)

Only streaming mappings specify a direction, coherent mappings
implicitly have a direction attribute setting of
DMA_BIDIRECTIONAL.
(只有流式映射才指定方向；一致性映射隐含的方向属性设置是 DMA_BIDIRECTIONAL。)

The SCSI subsystem tells you the direction to use in the
'sc_data_direction' member of the SCSI command your driver is
working on.
(SCSI 子系统会通过你的驱动正在处理的 SCSI 命令的 'sc_data_direction' 成员告诉你应使用的方向。)

For Networking drivers, it's a rather simple affair.  For transmit
packets, map/unmap them with the DMA_TO_DEVICE direction
specifier.  For receive packets, just the opposite, map/unmap them
with the DMA_FROM_DEVICE direction specifier.
(对网络驱动来说这相当简单：发送的数据包用 DMA_TO_DEVICE 方向说明符进行映射/解除映射；接收的数据包正相反，用 DMA_FROM_DEVICE 方向说明符进行映射/解除映射。)

Using Streaming DMA mappings(使用流式 DMA 映射)
===============================================

The streaming DMA mapping routines can be called from interrupt
context.  There are two versions of each map/unmap, one which will
map/unmap a single memory region, and one which will map/unmap a
scatterlist.
(流式 DMA 映射例程可以在中断上下文中调用。每种 map/unmap 都有两个版本：一个用于映射/解除映射单个内存区域，另一个用于映射/解除映射 scatterlist。)

To map a single region, you do::

	struct device *dev = &my_dev->dev;
	dma_addr_t dma_handle;
	void *addr = buffer->ptr;
	size_t size = buffer->len;

	dma_handle = dma_map_single(dev, addr, size, direction);
	if (dma_mapping_error(dev, dma_handle)) {
		/*
		 * reduce current DMA mapping usage,
		 * delay and try again later or
		 * reset driver.
		 */
		goto map_error_handling;
	}

(要映射单个区域，这样写：)

and to unmap it::

	dma_unmap_single(dev, dma_handle, size, direction);

(解除映射则用：)

You should call dma_mapping_error() as dma_map_single() could fail and return
error.  Doing so will ensure that the mapping code will work correctly on all
DMA implementations without any dependency on the specifics of the underlying
implementation. Using the returned address without checking for errors could
result in failures ranging from panics to silent data corruption.  The same
applies to dma_map_page() as well.
(你应当调用 dma_mapping_error()，因为 dma_map_single() 可能失败并返回错误。这样做可以确保映射代码在所有 DMA 实现上都正确工作，而不依赖底层实现的具体细节。不检查错误就直接使用返回的地址，可能导致从 panic 到静默数据损坏等各种故障。dma_map_page() 同样如此。)

You should call dma_unmap_single() when the DMA activity is finished, e.g.,
from the interrupt which told you that the DMA transfer is done.
(DMA 活动结束后应调用 dma_unmap_single()，例如在通知你 DMA 传输已完成的中断中调用。)

Using CPU pointers like this for single mappings has a disadvantage:
you cannot reference HIGHMEM memory in this way.  Thus, there is a
map/unmap interface pair akin to dma_{map,unmap}_single().  These
interfaces deal with page/offset pairs instead of CPU pointers.
Specifically::

	struct device *dev = &my_dev->dev;
	dma_addr_t dma_handle;
	struct page *page = buffer->page;
	unsigned long offset = buffer->offset;
	size_t size = buffer->len;

	dma_handle = dma_map_page(dev, page, offset, size, direction);
	if (dma_mapping_error(dev, dma_handle)) {
		/*
		 * reduce current DMA mapping usage,
		 * delay and try again later or
		 * reset driver.
		 */
		goto map_error_handling;
	}

	...

	dma_unmap_page(dev, dma_handle, size, direction);

(对单个映射这样使用 CPU 指针有一个缺点：这种方式无法引用 HIGHMEM 内存。因此还有一对与 dma_{map,unmap}_single() 类似的 map/unmap 接口，它们处理的是 page/offset 对而不是 CPU 指针。具体如上所示。)

Here, "offset" means byte offset within the given page.
(这里 "offset" 指给定页内的字节偏移。)

You should call dma_mapping_error() as dma_map_page() could fail and return
error as outlined under the dma_map_single() discussion.
(你应当调用 dma_mapping_error()，因为 dma_map_page() 可能失败并返回错误，如前文关于 dma_map_single() 的讨论所述。)

You should call dma_unmap_page() when the DMA activity is finished, e.g.,
from the interrupt which told you that the DMA transfer is done.
(DMA 活动结束后应调用 dma_unmap_page()，例如在通知你 DMA 传输已完成的中断中调用。)

With scatterlists, you map a region gathered from several regions by::

	int i, count = dma_map_sg(dev, sglist, nents, direction);
	struct scatterlist *sg;

	for_each_sg(sglist, sg, count, i) {
		hw_address[i] = sg_dma_address(sg);
		hw_len[i] = sg_dma_len(sg);
	}

(使用 scatterlist 时，用如下方式映射由多个区域汇集而成的区域：)

where nents is the number of entries in the sglist.
(其中 nents 是 sglist 中的条目数。)

The implementation is free to merge several consecutive sglist entries
into one (e.g. if DMA mapping is done with PAGE_SIZE granularity, any
consecutive sglist entries can be merged into one provided the first one
ends and the second one starts on a page boundary - in fact this is a huge
advantage for cards which either cannot do scatter-gather or have very
limited number of scatter-gather entries) and returns the actual number
of sg entries it mapped them to. On failure 0 is returned.
(实现可以自由地把多个连续的 sglist 条目合并为一个(例如，如果 DMA 映射以 PAGE_SIZE 粒度进行，只要前一个条目结束、后一个条目开始的位置都落在页边界上，任意连续的 sglist 条目都可以合并成一个——事实上这对那些不能做 scatter-gather 或 scatter-gather 条目数非常有限的卡来说是一个巨大优势)，并返回它实际映射到的 sg 条目数。失败时返回 0。)

Then you should loop count times (note: this can be less than nents times)
and use sg_dma_address() and sg_dma_len() macros where you previously
accessed sg->address and sg->length as shown above.
(然后你应当循环 count 次(注意：这可能小于 nents 次)，并在之前访问 sg->address 和 sg->length 的地方，如上所示改用 sg_dma_address() 和 sg_dma_len() 宏。)

To unmap a scatterlist, just call::

	dma_unmap_sg(dev, sglist, nents, direction);

(要解除 scatterlist 的映射，只需调用：)

Again, make sure DMA activity has already finished.
(同样，请确保 DMA 活动已经结束。)

.. note::

	The 'nents' argument to the dma_unmap_sg call must be
	the _same_ one you passed into the dma_map_sg call,
	it should _NOT_ be the 'count' value _returned_ from the
	dma_map_sg call.
	(dma_unmap_sg 调用中的 'nents' 参数必须与你传入 dma_map_sg 调用的那个 _完全相同_，而 _不是_ dma_map_sg 调用 _返回_ 的 'count' 值。)

Every dma_map_{single,sg}() call should have its dma_unmap_{single,sg}()
counterpart, because the DMA address space is a shared resource and
you could render the machine unusable by consuming all DMA addresses.
(每个 dma_map_{single,sg}() 调用都应有与之对应的 dma_unmap_{single,sg}()，因为 DMA 地址空间是共享资源，把所有 DMA 地址耗尽会让机器无法使用。)

If you need to use the same streaming DMA region multiple times and touch
the data in between the DMA transfers, the buffer needs to be synced
properly in order for the CPU and device to see the most up-to-date and
correct copy of the DMA buffer.
(如果需要多次使用同一个流式 DMA 区域，并在各次 DMA 传输之间触碰数据，就必须正确地同步缓冲区，使 CPU 和设备都能看到最新且正确的 DMA 缓冲区副本。)

So, firstly, just map it with dma_map_{single,sg}(), and after each DMA
transfer call either::

	dma_sync_single_for_cpu(dev, dma_handle, size, direction);

(因此，首先先用 dma_map_{single,sg}() 建立映射，然后在每次 DMA 传输之后视情况调用以下二者之一：)

or::

	dma_sync_sg_for_cpu(dev, sglist, nents, direction);

(或：)

as appropriate.
(视情况选用。)

Then, if you wish to let the device get at the DMA area again,
finish accessing the data with the CPU, and then before actually
giving the buffer to the hardware call either::

	dma_sync_single_for_device(dev, dma_handle, size, direction);

(然后，如果你想让设备再次访问该 DMA 区域，先结束 CPU 侧的数据访问，再在真正把缓冲区交给硬件之前调用以下二者之一：)

or::

	dma_sync_sg_for_device(dev, sglist, nents, direction);

(或：)

as appropriate.
(视情况选用。)

.. note::

	      The 'nents' argument to dma_sync_sg_for_cpu() and
	      dma_sync_sg_for_device() must be the same passed to
	      dma_map_sg(). It is _NOT_ the count returned by
	      dma_map_sg().
	      (dma_sync_sg_for_cpu() 和 dma_sync_sg_for_device() 的 'nents' 参数必须是传给 dma_map_sg() 的同一个值，而 _不是_ dma_map_sg() 返回的 count。)

After the last DMA transfer call one of the DMA unmap routines
dma_unmap_{single,sg}(). If you don't touch the data from the first
dma_map_*() call till dma_unmap_*(), then you don't have to call the
dma_sync_*() routines at all.
(在最后一次 DMA 传输之后，调用 dma_unmap_{single,sg}() 这类 DMA 解除映射例程之一。如果从第一次 dma_map_*() 调用到 dma_unmap_*() 之间你不触碰数据，那么完全不必调用 dma_sync_*() 例程。)

Here is pseudo code which shows a situation in which you would need
to use the dma_sync_*() interfaces::

	my_card_setup_receive_buffer(struct my_card *cp, char *buffer, int len)
	{
		dma_addr_t mapping;

		mapping = dma_map_single(cp->dev, buffer, len, DMA_FROM_DEVICE);
		if (dma_mapping_error(cp->dev, mapping)) {
			/*
			 * reduce current DMA mapping usage,
			 * delay and try again later or
			 * reset driver.
			 */
			goto map_error_handling;
		}

		cp->rx_buf = buffer;
		cp->rx_len = len;
		cp->rx_dma = mapping;

		give_rx_buf_to_card(cp);
	}

	...

	my_card_interrupt_handler(int irq, void *devid, struct pt_regs *regs)
	{
		struct my_card *cp = devid;

		...
		if (read_card_status(cp) == RX_BUF_TRANSFERRED) {
			struct my_card_header *hp;

			/* Examine the header to see if we wish
			 * to accept the data.  But synchronize
			 * the DMA transfer with the CPU first
			 * so that we see updated contents.
			 */
			dma_sync_single_for_cpu(&cp->dev, cp->rx_dma,
						cp->rx_len,
						DMA_FROM_DEVICE);

			/* Now it is safe to examine the buffer. */
			hp = (struct my_card_header *) cp->rx_buf;
			if (header_is_ok(hp)) {
				dma_unmap_single(&cp->dev, cp->rx_dma, cp->rx_len,
						 DMA_FROM_DEVICE);
				pass_to_upper_layers(cp->rx_buf);
				make_and_setup_new_rx_buf(cp);
			} else {
				/* CPU should not write to
				 * DMA_FROM_DEVICE-mapped area,
				 * so dma_sync_single_for_device() is
				 * not needed here. It would be required
				 * for DMA_BIDIRECTIONAL mapping if
				 * the memory was modified.
				 */
				give_rx_buf_to_card(cp);
			}
		}
	}

(上面的伪代码展示了需要使用 dma_sync_*() 接口的情形。)

Handling Errors(错误处理)
=========================

DMA address space is limited on some architectures and an allocation
failure can be determined by:
(在某些体系结构上 DMA 地址空间是有限的，可以通过以下方式判断分配是否失败：)

- checking if dma_alloc_coherent() returns NULL or dma_map_sg returns 0
  (检查 dma_alloc_coherent() 是否返回 NULL，或 dma_map_sg 是否返回 0)

- checking the dma_addr_t returned from dma_map_single() and dma_map_page()
  by using dma_mapping_error()::

	dma_addr_t dma_handle;

	dma_handle = dma_map_single(dev, addr, size, direction);
	if (dma_mapping_error(dev, dma_handle)) {
		/*
		 * reduce current DMA mapping usage,
		 * delay and try again later or
		 * reset driver.
		 */
		goto map_error_handling;
	}

  (用 dma_mapping_error() 检查 dma_map_single() 和 dma_map_page() 返回的 dma_addr_t：)

- unmap pages that are already mapped, when mapping error occurs in the middle
  of a multiple page mapping attempt. These example are applicable to
  dma_map_page() as well.
  (当多页映射尝试中途出现映射错误时，解除已映射页的映射。这些示例同样适用于 dma_map_page()。)

Example 1::

	dma_addr_t dma_handle1;
	dma_addr_t dma_handle2;

	dma_handle1 = dma_map_single(dev, addr, size, direction);
	if (dma_mapping_error(dev, dma_handle1)) {
		/*
		 * reduce current DMA mapping usage,
		 * delay and try again later or
		 * reset driver.
		 */
		goto map_error_handling1;
	}
	dma_handle2 = dma_map_single(dev, addr, size, direction);
	if (dma_mapping_error(dev, dma_handle2)) {
		/*
		 * reduce current DMA mapping usage,
		 * delay and try again later or
		 * reset driver.
		 */
		goto map_error_handling2;
	}

	...

	map_error_handling2:
		dma_unmap_single(dma_handle1);
	map_error_handling1:

(示例 1：)

Example 2::

	/*
	 * if buffers are allocated in a loop, unmap all mapped buffers when
	 * mapping error is detected in the middle
	 */

	dma_addr_t dma_addr;
	dma_addr_t array[DMA_BUFFERS];
	int save_index = 0;

	for (i = 0; i < DMA_BUFFERS; i++) {

		...

		dma_addr = dma_map_single(dev, addr, size, direction);
		if (dma_mapping_error(dev, dma_addr)) {
			/*
			 * reduce current DMA mapping usage,
			 * delay and try again later or
			 * reset driver.
			 */
			goto map_error_handling;
		}
		array[i].dma_addr = dma_addr;
		save_index++;
	}

	...

	map_error_handling:

	for (i = 0; i < save_index; i++) {

		...

		dma_unmap_single(array[i].dma_addr);
	}

(示例 2：)

Networking drivers must call dev_kfree_skb() to free the socket buffer
and return NETDEV_TX_OK if the DMA mapping fails on the transmit hook
(ndo_start_xmit). This means that the socket buffer is just dropped in
the failure case.
(网络驱动在发送钩子 (ndo_start_xmit) 中遇到 DMA 映射失败时，必须调用 dev_kfree_skb() 释放套接字缓冲区并返回 NETDEV_TX_OK。这意味着失败情况下套接字缓冲区会被直接丢弃。)

SCSI drivers must return SCSI_MLQUEUE_HOST_BUSY if the DMA mapping
fails in the queuecommand hook. This means that the SCSI subsystem
passes the command to the driver again later.
(SCSI 驱动在 queuecommand 钩子中遇到 DMA 映射失败时，必须返回 SCSI_MLQUEUE_HOST_BUSY。这意味着 SCSI 子系统稍后会再次把命令传给驱动。)

Optimizing Unmap State Space Consumption(优化 unmap 状态的空间占用)
===================================================================

On many platforms, dma_unmap_{single,page}() is simply a nop.
Therefore, keeping track of the mapping address and length is a waste
of space.  Instead of filling your drivers up with ifdefs and the like
to "work around" this (which would defeat the whole purpose of a
portable API) the following facilities are provided.
(在许多平台上，dma_unmap_{single,page}() 只是个空操作。因此，记录映射地址和长度是浪费空间。为了避免在驱动中塞满 ifdef 之类的东西来"绕过"这一点(那会破坏可移植 API 的全部意义)，内核提供了以下设施。)

Actually, instead of describing the macros one by one, we'll
transform some example code.
(实际上，与其逐个描述这些宏，我们直接改造一些示例代码。)

1) Use DEFINE_DMA_UNMAP_{ADDR,LEN} in state saving structures.
   Example, before::

	struct ring_state {
		struct sk_buff *skb;
		dma_addr_t mapping;
		__u32 len;
	};

   after::

	struct ring_state {
		struct sk_buff *skb;
		DEFINE_DMA_UNMAP_ADDR(mapping);
		DEFINE_DMA_UNMAP_LEN(len);
	};

   (1) 在保存状态的结构体中使用 DEFINE_DMA_UNMAP_{ADDR,LEN}。示例，修改前如上，修改后如下：)

2) Use dma_unmap_{addr,len}_set() to set these values.
   Example, before::

	ringp->mapping = FOO;
	ringp->len = BAR;

   after::

	dma_unmap_addr_set(ringp, mapping, FOO);
	dma_unmap_len_set(ringp, len, BAR);

   (2) 使用 dma_unmap_{addr,len}_set() 设置这些值。示例，修改前如上，修改后如下：)

3) Use dma_unmap_{addr,len}() to access these values.
   Example, before::

	dma_unmap_single(dev, ringp->mapping, ringp->len,
			 DMA_FROM_DEVICE);

   after::

	dma_unmap_single(dev,
			 dma_unmap_addr(ringp, mapping),
			 dma_unmap_len(ringp, len),
			 DMA_FROM_DEVICE);

   (3) 使用 dma_unmap_{addr,len}() 访问这些值。示例，修改前如上，修改后如下：)

It really should be self-explanatory.  We treat the ADDR and LEN
separately, because it is possible for an implementation to only
need the address in order to perform the unmap operation.
(这应当不言自明。我们把 ADDR 和 LEN 分开处理，因为某些实现可能只需要地址就能执行解除映射操作。)

Platform Issues(平台相关问题)
=============================

If you are just writing drivers for Linux and do not maintain
an architecture port for the kernel, you can safely skip down
to "Closing".
(如果你只是为 Linux 编写驱动、而不维护内核的某个架构移植，可以放心直接跳到 "Closing"。)

1) Struct scatterlist requirements.
   (1) struct scatterlist 的要求：)

   You need to enable CONFIG_NEED_SG_DMA_LENGTH if the architecture
   supports IOMMUs (including software IOMMU).
   (如果架构支持 IOMMU(包括软件 IOMMU)，需要启用 CONFIG_NEED_SG_DMA_LENGTH。)

2) ARCH_DMA_MINALIGN
   (2) ARCH_DMA_MINALIGN：)

   Architectures must ensure that kmalloc'ed buffer is
   DMA-safe. Drivers and subsystems depend on it. If an architecture
   isn't fully DMA-coherent (i.e. hardware doesn't ensure that data in
   the CPU cache is identical to data in main memory),
   ARCH_DMA_MINALIGN must be set so that the memory allocator
   makes sure that kmalloc'ed buffer doesn't share a cache line with
   the others. See arch/arm/include/asm/cache.h as an example.
   (架构必须确保 kmalloc 分配的缓冲区是 DMA 安全的，驱动和子系统都依赖这一点。如果架构并非完全 DMA 一致(即硬件不保证 CPU 缓存中的数据与主存中的数据一致)，就必须设置 ARCH_DMA_MINALIGN，让内存分配器确保 kmalloc 的缓冲区不与其他数据共享缓存行。参见 arch/arm/include/asm/cache.h 这个例子。)

   Note that ARCH_DMA_MINALIGN is about DMA memory alignment
   constraints. You don't need to worry about the architecture data
   alignment constraints (e.g. the alignment constraints about 64-bit
   objects).
   (注意，ARCH_DMA_MINALIGN 只关乎 DMA 内存对齐约束。你不需要操心架构的数据对齐约束(例如关于 64 位对象的对齐约束)。)

Closing(结语)
=============

This document, and the API itself, would not be in its current
form without the feedback and suggestions from numerous individuals.
We would like to specifically mention, in no particular order, the
following people::

	Russell King <rmk@arm.linux.org.uk>
	Leo Dagum <dagum@barrel.engr.sgi.com>
	Ralf Baechle <ralf@oss.sgi.com>
	Grant Grundler <grundler@cup.hp.com>
	Jay Estabrook <Jay.Estabrook@compaq.com>
	Thomas Sailer <sailer@ife.ee.ethz.ch>
	Andrea Arcangeli <andrea@suse.de>
	Jens Axboe <jens.axboe@oracle.com>
	David Mosberger-Tang <davidm@hpl.hp.com>

(如果没有众多个人的反馈和建议，本文档以及这个 API 都不会是今天的模样。我们想特别提到以下诸位(排名不分先后)：)
