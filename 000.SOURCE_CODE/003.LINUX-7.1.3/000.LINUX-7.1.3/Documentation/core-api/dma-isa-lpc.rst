======================================================
DMA with ISA and LPC devices(与 ISA 和 LPC 设备的 DMA)
======================================================

:Author: Pierre Ossman <drzeus@drzeus.cx>
(作者：Pierre Ossman <drzeus@drzeus.cx>)

This document describes how to do DMA transfers using the old ISA DMA
controller. Even though ISA is more or less dead today the LPC bus
uses the same DMA system so it will be around for quite some time.
(本文描述如何使用旧式 ISA DMA 控制器进行 DMA 传输。尽管 ISA 在今天已
名存实亡，但 LPC 总线使用相同的 DMA 系统，因此它还会存在相当长的一段
时间。)

Headers and dependencies(头文件与依赖)
--------------------------------------

To do ISA style DMA you need to include two headers::

	#include <linux/dma-mapping.h>
	#include <asm/dma.h>

(要进行 ISA 风格的 DMA，你需要包含两个头文件：)

The first is the generic DMA API used to convert virtual addresses to
bus addresses (see Documentation/core-api/dma-api.rst for details).
(第一个是通用 DMA API，用于把虚拟地址转换为总线地址（详见
Documentation/core-api/dma-api.rst）。)

The second contains the routines specific to ISA DMA transfers. Since
this is not present on all platforms make sure you construct your
Kconfig to be dependent on ISA_DMA_API (not ISA) so that nobody tries
to build your driver on unsupported platforms.
(第二个包含 ISA DMA 传输特有的例程。由于并非所有平台都有它，请确保把
你的 Kconfig 构造为依赖 ISA_DMA_API（而不是 ISA），这样就不会有人在
不受支持的平台上尝试构建你的驱动。)

Buffer allocation(缓冲区分配)
-----------------------------

The ISA DMA controller has some very strict requirements on which
memory it can access so extra care must be taken when allocating
buffers.
(ISA DMA 控制器对它可以访问的内存有一些非常严格的要求，因此在分配
缓冲区时必须格外小心。)

(You usually need a special buffer for DMA transfers instead of
transferring directly to and from your normal data structures.)
(通常，你需要为 DMA 传输准备一个特殊的缓冲区，而不是直接在普通数据
结构上进行传输。)

The DMA-able address space is the lowest 16 MB of _physical_ memory.
Also the transfer block may not cross page boundaries (which are 64
or 128 KiB depending on which channel you use).
(可供 DMA 的地址空间是物理内存中最低的 16 MB。此外，传输块不能跨越
页边界（根据所使用的通道，页边界为 64 或 128 KiB）。)

In order to allocate a piece of memory that satisfies all these
requirements you pass the flag GFP_DMA to kmalloc.
(为了分配一块满足所有这些要求的内存，请把 GFP_DMA 标志传给 kmalloc。)

Unfortunately the memory available for ISA DMA is scarce so unless you
allocate the memory during boot-up it's a good idea to also pass
__GFP_RETRY_MAYFAIL and __GFP_NOWARN to make the allocator try a bit harder.
(遗憾的是，可用于 ISA DMA 的内存非常稀缺，因此除非你在启动期间分配
内存，否则最好同时传递 __GFP_RETRY_MAYFAIL 和 __GFP_NOWARN，让分配器
多做一些尝试。)

(This scarcity also means that you should allocate the buffer as
early as possible and not release it until the driver is unloaded.)
(这种稀缺性也意味着你应该尽早分配缓冲区，并且在驱动卸载之前不要
释放它。)

Address translation(地址转换)
-----------------------------

To translate the virtual address to a bus address, use the normal DMA
API. Do _not_ use isa_virt_to_bus() even though it does the same
thing. The reason for this is that the function isa_virt_to_bus()
will require a Kconfig dependency to ISA, not just ISA_DMA_API which
is really all you need. Remember that even though the DMA controller
has its origins in ISA it is used elsewhere.
(要把虚拟地址转换为总线地址，请使用常规的 DMA API。不要使用
isa_virt_to_bus()，即使它做的是同样的事情。原因在于 isa_virt_to_bus()
需要 Kconfig 依赖 ISA，而实际上你真正需要的只是 ISA_DMA_API。请记住，
尽管 DMA 控制器起源于 ISA，但它也用在其他地方。)

Note: x86_64 had a broken DMA API when it came to ISA but has since
been fixed. If your arch has problems then fix the DMA API instead of
reverting to the ISA functions.
(注意：x86_64 曾在 ISA 方面有一个损坏的 DMA API，但后来已经修复。
如果你的架构有问题，请修复 DMA API，而不是退回到 ISA 函数。)

Channels(通道)
--------------

A normal ISA DMA controller has 8 channels. The lower four are for
8-bit transfers and the upper four are for 16-bit transfers.
(普通的 ISA DMA 控制器有 8 个通道。低 4 个用于 8 位传输，高 4 个用于
16 位传输。)

(Actually the DMA controller is really two separate controllers where
channel 4 is used to give DMA access for the second controller (0-3).
This means that of the four 16-bits channels only three are usable.)
(实际上，DMA 控制器其实是两个独立的控制器，通道 4 用于为第二个控制器
(0-3) 提供 DMA 访问。这意味着 4 个 16 位通道中只有 3 个可用。)

You allocate these in a similar fashion as all basic resources:
(这些通道的分配方式与所有基本资源类似：)

extern int request_dma(unsigned int dmanr, const char * device_id);
extern void free_dma(unsigned int dmanr);

The ability to use 16-bit or 8-bit transfers is _not_ up to you as a
driver author but depends on what the hardware supports. Check your
specs or test different channels.
(使用 16 位还是 8 位传输并不由你这位驱动作者决定，而是取决于硬件
支持什么。请查阅你的规格说明或测试不同的通道。)

Transfer data(传输数据)
-----------------------

Now for the good stuff, the actual DMA transfer. :)
(接下来是重头戏——实际的 DMA 传输。:)

Before you use any ISA DMA routines you need to claim the DMA lock
using claim_dma_lock(). The reason is that some DMA operations are
not atomic so only one driver may fiddle with the registers at a
time.
(在使用任何 ISA DMA 例程之前，你需要用 claim_dma_lock() 获取 DMA 锁。
原因是一些 DMA 操作不是原子的，因此一次只能有一个驱动摆弄这些
寄存器。)

The first time you use the DMA controller you should call
clear_dma_ff(). This clears an internal register in the DMA
controller that is used for the non-atomic operations. As long as you
(and everyone else) uses the locking functions then you only need to
reset this once.
(第一次使用 DMA 控制器时，你应该调用 clear_dma_ff()。它会清除 DMA
控制器中一个用于非原子操作的内部寄存器。只要你（以及其他所有人）都
使用加锁函数，那么只需重置这一次。)

Next, you tell the controller in which direction you intend to do the
transfer using set_dma_mode(). Currently you have the options
DMA_MODE_READ and DMA_MODE_WRITE.
(接下来，用 set_dma_mode() 告诉控制器你打算往哪个方向进行传输。目前
你有 DMA_MODE_READ 和 DMA_MODE_WRITE 两个选项。)

Set the address from where the transfer should start (this needs to
be 16-bit aligned for 16-bit transfers) and how many bytes to
transfer. Note that it's _bytes_. The DMA routines will do all the
required translation to values that the DMA controller understands.
(设置传输开始的地址（对于 16 位传输，它需要 16 位对齐）以及要传输的
字节数。注意单位是字节 (bytes)。DMA 例程会完成所有必要的转换，变成
DMA 控制器能理解的值。)

The final step is enabling the DMA channel and releasing the DMA
lock.
(最后一步是启用 DMA 通道并释放 DMA 锁。)

Once the DMA transfer is finished (or timed out) you should disable
the channel again. You should also check get_dma_residue() to make
sure that all data has been transferred.
(一旦 DMA 传输完成（或超时），你应该再次禁用通道。你还应该检查
get_dma_residue()，确保所有数据都已传输完毕。)

Example::

	int flags, residue;

	flags = claim_dma_lock();

	clear_dma_ff();

	set_dma_mode(channel, DMA_MODE_WRITE);
	set_dma_addr(channel, phys_addr);
	set_dma_count(channel, num_bytes);

	dma_enable(channel);

	release_dma_lock(flags);

	while (!device_done());

	flags = claim_dma_lock();

	dma_disable(channel);

	residue = dma_get_residue(channel);
	if (residue != 0)
		printk(KERN_ERR "driver: Incomplete DMA transfer!"
			" %d bytes left!\n", residue);

	release_dma_lock(flags);

(示例：)

Suspend/resume(挂起/恢复)
-------------------------

It is the driver's responsibility to make sure that the machine isn't
suspended while a DMA transfer is in progress. Also, all DMA settings
are lost when the system suspends so if your driver relies on the DMA
controller being in a certain state then you have to restore these
registers upon resume.
(确保机器不会在 DMA 传输进行期间被挂起是驱动的责任。此外，系统挂起时
所有 DMA 设置都会丢失，因此如果你的驱动依赖 DMA 控制器处于某种特定
状态，那么你必须在恢复时还原这些寄存器。)
