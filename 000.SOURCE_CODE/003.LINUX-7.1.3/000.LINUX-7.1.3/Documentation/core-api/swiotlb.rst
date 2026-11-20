.. SPDX-License-Identifier: GPL-2.0

===============================
DMA and swiotlb(DMA 与 swiotlb)
===============================

swiotlb is a memory buffer allocator used by the Linux kernel DMA layer. It is
typically used when a device doing DMA can't directly access the target memory
buffer because of hardware limitations or other requirements. In such a case,
the DMA layer calls swiotlb to allocate a temporary memory buffer that conforms
to the limitations. The DMA is done to/from this temporary memory buffer, and
the CPU copies the data between the temporary buffer and the original target
memory buffer. This approach is generically called "bounce buffering", and the
temporary memory buffer is called a "bounce buffer".
(swiotlb 是 Linux 内核 DMA 层使用的一个内存缓冲区分配器。当执行 DMA 的设备由于硬件限制或其他要求而无法直接访问目标内存缓冲区时，通常就会用到它。这种情况下，DMA 层调用 swiotlb 分配一块符合限制条件的临时内存缓冲区。DMA 在这块临时内存缓冲区上进行，由 CPU 在临时缓冲区与原始目标内存缓冲区之间复制数据。这种做法一般称为"反弹缓冲"（bounce buffering），临时内存缓冲区则称为"反弹缓冲区"（bounce buffer）。)

Device drivers don't interact directly with swiotlb. Instead, drivers inform
the DMA layer of the DMA attributes of the devices they are managing, and use
the normal DMA map, unmap, and sync APIs when programming a device to do DMA.
These APIs use the device DMA attributes and kernel-wide settings to determine
if bounce buffering is necessary. If so, the DMA layer manages the allocation,
freeing, and sync'ing of bounce buffers. Since the DMA attributes are per
device, some devices in a system may use bounce buffering while others do not.
(设备驱动不直接与 swiotlb 交互。相反，驱动把它们所管理设备的 DMA 属性告知 DMA 层，并在对设备编程执行 DMA 时使用常规的 DMA map、unmap 和 sync API。这些 API 依据设备 DMA 属性和内核全局设置来判断是否需要反弹缓冲。如果需要，DMA 层会负责反弹缓冲区的分配、释放与同步。由于 DMA 属性是每设备独立的，系统中可能一部分设备使用反弹缓冲，另一部分不使用。)

Because the CPU copies data between the bounce buffer and the original target
memory buffer, doing bounce buffering is slower than doing DMA directly to the
original memory buffer, and it consumes more CPU resources. So it is used only
when necessary for providing DMA functionality.
(由于 CPU 要在反弹缓冲区与原始目标内存缓冲区之间复制数据，反弹缓冲比直接对原始内存缓冲区做 DMA 更慢，并且消耗更多 CPU 资源。因此只在提供 DMA 功能确有必要时才使用它。)

Usage Scenarios(使用场景)
-------------------------
swiotlb was originally created to handle DMA for devices with addressing
limitations. As physical memory sizes grew beyond 4 GiB, some devices could
only provide 32-bit DMA addresses. By allocating bounce buffer memory below
the 4 GiB line, these devices with addressing limitations could still work and
do DMA.
(swiotlb 最初是为处理有寻址限制的设备的 DMA 而创建的。随着物理内存容量超过 4 GiB，一些设备只能提供 32 位 DMA 地址。通过把反弹缓冲内存分配在 4 GiB 线以下，这些有寻址限制的设备仍然能够工作并执行 DMA。)

More recently, Confidential Computing (CoCo) VMs have the guest VM's memory
encrypted by default, and the memory is not accessible by the host hypervisor
and VMM. For the host to do I/O on behalf of the guest, the I/O must be
directed to guest memory that is unencrypted. CoCo VMs set a kernel-wide option
to force all DMA I/O to use bounce buffers, and the bounce buffer memory is set
up as unencrypted. The host does DMA I/O to/from the bounce buffer memory, and
the Linux kernel DMA layer does "sync" operations to cause the CPU to copy the
data to/from the original target memory buffer. The CPU copying bridges between
the unencrypted and the encrypted memory. This use of bounce buffers allows
device drivers to "just work" in a CoCo VM, with no modifications
needed to handle the memory encryption complexity.
(近来，机密计算（CoCo）VM 默认对客户机 VM 的内存加密，宿主机 hypervisor 与 VMM 无法访问这些内存。宿主机要代表客户机做 I/O，就必须把 I/O 定向到未加密的客户机内存。CoCo VM 设置一个内核全局选项，强制所有 DMA I/O 使用反弹缓冲区，并把反弹缓冲内存设置为不加密。宿主机对反弹缓冲内存做 DMA I/O，Linux 内核 DMA 层执行"sync"操作使 CPU 在原始目标内存缓冲区与反弹缓冲区之间复制数据。CPU 复制在不加密内存与加密内存之间架起桥梁。这种反弹缓冲的用法让设备驱动在 CoCo VM 中"开箱即用"，无须为处理内存加密的复杂性做任何修改。)

Other edge case scenarios arise for bounce buffers. For example, when IOMMU
mappings are set up for a DMA operation to/from a device that is considered
"untrusted", the device should be given access only to the memory containing
the data being transferred. But if that memory occupies only part of an IOMMU
granule, other parts of the granule may contain unrelated kernel data. Since
IOMMU access control is per-granule, the untrusted device can gain access to
the unrelated kernel data. This problem is solved by bounce buffering the DMA
operation and ensuring that unused portions of the bounce buffers do not
contain any unrelated kernel data.
(反弹缓冲还有其他边缘场景。例如，当为与"不受信任"设备之间的 DMA 操作建立 IOMMU 映射时，只应让该设备访问包含所传输数据的内存。但如果这块内存只占用某个 IOMMU 粒度（granule）的一部分，粒度的其他部分可能含有无关的内核数据。由于 IOMMU 访问控制以粒度为单位，不受信任的设备就能访问到无关内核数据。对 DMA 操作做反弹缓冲，并确保反弹缓冲区中未使用的部分不含任何无关内核数据，即可解决此问题。)

Core Functionality(核心功能)
----------------------------
The primary swiotlb APIs are swiotlb_tbl_map_single() and
swiotlb_tbl_unmap_single(). The "map" API allocates a bounce buffer of a
specified size in bytes and returns the physical address of the buffer. The
buffer memory is physically contiguous. The expectation is that the DMA layer
maps the physical memory address to a DMA address, and returns the DMA address
to the driver for programming into the device. If a DMA operation specifies
multiple memory buffer segments, a separate bounce buffer must be allocated for
each segment. swiotlb_tbl_map_single() always does a "sync" operation (i.e., a
CPU copy) to initialize the bounce buffer to match the contents of the original
buffer.
(swiotlb 的主要 API 是 swiotlb_tbl_map_single() 和 swiotlb_tbl_unmap_single()。"map" API 按指定的大小（字节）分配一个反弹缓冲区并返回其物理地址。缓冲区内存是物理连续的。预期流程是：DMA 层把物理内存地址映射为 DMA 地址，并把 DMA 地址返回给驱动以编程到设备中。如果一个 DMA 操作指定了多个内存缓冲区分段，则必须为每个分段单独分配一个反弹缓冲区。swiotlb_tbl_map_single() 总是执行一次"sync"操作（即 CPU 复制），把反弹缓冲区初始化为与原始缓冲区内容一致。)

swiotlb_tbl_unmap_single() does the reverse. If the DMA operation might have
updated the bounce buffer memory and DMA_ATTR_SKIP_CPU_SYNC is not set, the
unmap does a "sync" operation to cause a CPU copy of the data from the bounce
buffer back to the original buffer. Then the bounce buffer memory is freed.
(swiotlb_tbl_unmap_single() 做相反的事。如果 DMA 操作可能已更新反弹缓冲内存、且未设置 DMA_ATTR_SKIP_CPU_SYNC，unmap 会执行一次"sync"操作，使 CPU 把数据从反弹缓冲区复制回原始缓冲区。然后释放反弹缓冲内存。)

swiotlb also provides "sync" APIs that correspond to the dma_sync_*() APIs that
a driver may use when control of a buffer transitions between the CPU and the
device. The swiotlb "sync" APIs cause a CPU copy of the data between the
original buffer and the bounce buffer. Like the dma_sync_*() APIs, the swiotlb
"sync" APIs support doing a partial sync, where only a subset of the bounce
buffer is copied to/from the original buffer.
(swiotlb 还提供与 dma_sync_*() API 对应的"sync" API，驱动在缓冲区的控制权在 CPU 与设备之间切换时会用到 dma_sync_*()。swiotlb 的"sync" API 使 CPU 在原始缓冲区与反弹缓冲区之间复制数据。与 dma_sync_*() API 一样，swiotlb 的"sync" API 支持部分同步，即只把反弹缓冲区的一部分与原始缓冲区互拷。)

Core Functionality Constraints(核心功能约束)
--------------------------------------------
The swiotlb map/unmap/sync APIs must operate without blocking, as they are
called by the corresponding DMA APIs which may run in contexts that cannot
block. Hence the default memory pool for swiotlb allocations must be
pre-allocated at boot time (but see Dynamic swiotlb below). Because swiotlb
allocations must be physically contiguous, the entire default memory pool is
allocated as a single contiguous block.
(swiotlb 的 map/unmap/sync API 必须无阻塞地运行，因为调用它们的相应 DMA API 可能运行在不可阻塞的上下文中。因此，swiotlb 分配所用的默认内存池必须在启动时预先分配（但参见下文"动态 swiotlb"）。由于 swiotlb 分配必须物理连续，整个默认内存池作为单个连续块分配。)

The need to pre-allocate the default swiotlb pool creates a boot-time tradeoff.
The pool should be large enough to ensure that bounce buffer requests can
always be satisfied, as the non-blocking requirement means requests can't wait
for space to become available. But a large pool potentially wastes memory, as
this pre-allocated memory is not available for other uses in the system. The
tradeoff is particularly acute in CoCo VMs that use bounce buffers for all DMA
I/O. These VMs use a heuristic to set the default pool size to ~6% of memory,
with a max of 1 GiB, which has the potential to be very wasteful of memory.
Conversely, the heuristic might produce a size that is insufficient, depending
on the I/O patterns of the workload in the VM. The dynamic swiotlb feature
described below can help, but has limitations. Better management of the swiotlb
default memory pool size remains an open issue.
(需要预先分配默认 swiotlb 池，造成了启动时的权衡。池应当足够大，以保证反弹缓冲请求总能被满足——无阻塞要求意味着请求不能等待空间变得可用。但大池可能浪费内存，因为这部分预分配内存不能被系统挪作他用。这一权衡在所有 DMA I/O 都使用反弹缓冲的 CoCo VM 中尤为突出。这些 VM 用启发式方法把默认池大小设为内存的约 6%、上限 1 GiB，可能非常浪费内存。反过来，启发式得出的大小也可能不足，取决于 VM 中工作负载的 I/O 模式。下文描述的动态 swiotlb 特性可以有所帮助，但也有局限。更好地管理 swiotlb 默认内存池大小仍是一个悬而未决的问题。)

A single allocation from swiotlb is limited to IO_TLB_SIZE * IO_TLB_SEGSIZE
bytes, which is 256 KiB with current definitions. When a device's DMA settings
are such that the device might use swiotlb, the maximum size of a DMA segment
must be limited to that 256 KiB. This value is communicated to higher-level
kernel code via dma_map_mapping_size() and swiotlb_max_mapping_size(). If the
higher-level code fails to account for this limit, it may make requests that
are too large for swiotlb, and get a "swiotlb full" error.
(swiotlb 的单次分配上限为 IO_TLB_SIZE * IO_TLB_SEGSIZE 字节，按当前定义为 256 KiB。当设备的 DMA 设置使其可能使用 swiotlb 时，DMA 分段的最大尺寸必须限制在 256 KiB。该值通过 dma_map_mapping_size() 和 swiotlb_max_mapping_size() 传递给更高层的内核代码。如果高层代码没有考虑这一限制，就可能发出对 swiotlb 来说过大的请求，并得到 "swiotlb full" 错误。)

A key device DMA setting is "min_align_mask", which is a power of 2 minus 1
so that some number of low order bits are set, or it may be zero. swiotlb
allocations ensure these min_align_mask bits of the physical address of the
bounce buffer match the same bits in the address of the original buffer. When
min_align_mask is non-zero, it may produce an "alignment offset" in the address
of the bounce buffer that slightly reduces the maximum size of an allocation.
This potential alignment offset is reflected in the value returned by
swiotlb_max_mapping_size(), which can show up in places like
/sys/block/<device>/queue/max_sectors_kb. For example, if a device does not use
swiotlb, max_sectors_kb might be 512 KiB or larger. If a device might use
swiotlb, max_sectors_kb will be 256 KiB. When min_align_mask is non-zero,
max_sectors_kb might be even smaller, such as 252 KiB.
(一个关键的设备 DMA 设置是 "min_align_mask"，它是 2 的幂减 1（使若干低位被置位），也可以为 0。swiotlb 分配会确保反弹缓冲区物理地址中的这些 min_align_mask 位与原始缓冲区地址中的对应位一致。当 min_align_mask 非零时，它可能在反弹缓冲区地址中产生一个"对齐偏移"（alignment offset），略微降低单次分配的最大尺寸。这一潜在的对齐偏移反映在 swiotlb_max_mapping_size() 的返回值中，可能体现在诸如 /sys/block/<device>/queue/max_sectors_kb 之类的地方。例如，设备不使用 swiotlb 时 max_sectors_kb 可能是 512 KiB 或更大；设备可能使用 swiotlb 时则为 256 KiB；min_align_mask 非零时，max_sectors_kb 还可能更小，比如 252 KiB。)

swiotlb_tbl_map_single() also takes an "alloc_align_mask" parameter. This
parameter specifies the allocation of bounce buffer space must start at a
physical address with the alloc_align_mask bits set to zero. But the actual
bounce buffer might start at a larger address if min_align_mask is non-zero.
Hence there may be pre-padding space that is allocated prior to the start of
the bounce buffer. Similarly, the end of the bounce buffer is rounded up to an
alloc_align_mask boundary, potentially resulting in post-padding space. Any
pre-padding or post-padding space is not initialized by swiotlb code. The
"alloc_align_mask" parameter is used by IOMMU code when mapping for untrusted
devices. It is set to the granule size - 1 so that the bounce buffer is
allocated entirely from granules that are not used for any other purpose.
(swiotlb_tbl_map_single() 还接受一个 "alloc_align_mask" 参数。该参数规定反弹缓冲区空间的分配必须从 alloc_align_mask 位全为 0 的物理地址开始。但如果 min_align_mask 非零，实际反弹缓冲区可能从更大的地址开始。因此可能存在在反弹缓冲区起点之前分配的"前填充"空间。类似地，反弹缓冲区的结尾会向上取整到 alloc_align_mask 边界，可能产生"后填充"空间。任何前填充或后填充空间都不会被 swiotlb 代码初始化。"alloc_align_mask" 参数由 IOMMU 代码在为不受信任设备做映射时使用，设置为粒度大小 - 1，使反弹缓冲区完全从不作其他用途的粒度中分配。)

Data structures concepts(数据结构概念)
--------------------------------------
Memory used for swiotlb bounce buffers is allocated from overall system memory
as one or more "pools". The default pool is allocated during system boot with a
default size of 64 MiB. The default pool size may be modified with the
"swiotlb=" kernel boot line parameter. The default size may also be adjusted
due to other conditions, such as running in a CoCo VM, as described above. If
CONFIG_SWIOTLB_DYNAMIC is enabled, additional pools may be allocated later in
the life of the system. Each pool must be a contiguous range of physical
memory. The default pool is allocated below the 4 GiB physical address line so
it works for devices that can only address 32-bits of physical memory (unless
architecture-specific code provides the SWIOTLB_ANY flag). In a CoCo VM, the
pool memory must be decrypted before swiotlb is used.
(swiotlb 反弹缓冲所用的内存从整个系统内存中分配，形式为一个或多个"内存池"（pool）。默认池在系统引导期间分配，默认大小为 64 MiB。默认池大小可用内核引导参数 "swiotlb=" 修改；也可能因其他条件而调整，比如上文所述运行在 CoCo VM 中。如果启用了 CONFIG_SWIOTLB_DYNAMIC，系统运行期间还可以再分配额外的池。每个池必须是物理内存的连续区间。默认池分配在 4 GiB 物理地址线以下，因此对只能寻址 32 位物理内存的设备可用（除非体系结构相关代码提供了 SWIOTLB_ANY 标志）。在 CoCo VM 中，使用 swiotlb 之前必须先解密池内存。)

Each pool is divided into "slots" of size IO_TLB_SIZE, which is 2 KiB with
current definitions. IO_TLB_SEGSIZE contiguous slots (128 slots) constitute
what might be called a "slot set". When a bounce buffer is allocated, it
occupies one or more contiguous slots. A slot is never shared by multiple
bounce buffers. Furthermore, a bounce buffer must be allocated from a single
slot set, which leads to the maximum bounce buffer size being IO_TLB_SIZE *
IO_TLB_SEGSIZE. Multiple smaller bounce buffers may co-exist in a single slot
set if the alignment and size constraints can be met.
(每个池被划分为大小为 IO_TLB_SIZE 的"槽"（slot），按当前定义为 2 KiB。IO_TLB_SEGSIZE 个连续槽（128 个）构成所谓的"槽组"（slot set）。分配反弹缓冲区时，它占据一个或多个连续槽。一个槽绝不会被多个反弹缓冲区共享。此外，反弹缓冲区必须从单个槽组中分配，因此反弹缓冲区的最大尺寸为 IO_TLB_SIZE * IO_TLB_SEGSIZE。在对齐与大小约束能被满足的前提下，多个较小的反弹缓冲区可以共存于同一个槽组中。)

Slots are also grouped into "areas", with the constraint that a slot set exists
entirely in a single area. Each area has its own spin lock that must be held to
manipulate the slots in that area. The division into areas avoids contending
for a single global spin lock when swiotlb is heavily used, such as in a CoCo
VM. The number of areas defaults to the number of CPUs in the system for
maximum parallelism, but since an area can't be smaller than IO_TLB_SEGSIZE
slots, it might be necessary to assign multiple CPUs to the same area. The
number of areas can also be set via the "swiotlb=" kernel boot parameter.
(槽还被组织成"区域"（area），约束是槽组必须完整地位于单个区域内。每个区域有自己的自旋锁，操作该区域内的槽必须持有它。划分区域是为了避免在 swiotlb 高强度使用（如在 CoCo VM 中）时争用单一的全局自旋锁。区域数量默认为系统中的 CPU 数量以获得最大并行度；但由于区域不能小于 IO_TLB_SEGSIZE 个槽，可能需要把多个 CPU 分到同一区域。区域数量也可以通过内核引导参数 "swiotlb=" 设置。)

When allocating a bounce buffer, if the area associated with the calling CPU
does not have enough free space, areas associated with other CPUs are tried
sequentially. For each area tried, the area's spin lock must be obtained before
trying an allocation, so contention may occur if swiotlb is relatively busy
overall. But an allocation request does not fail unless all areas do not have
enough free space.
(分配反弹缓冲区时，如果与调用 CPU 关联的区域没有足够的空闲空间，会依次尝试与其他 CPU 关联的区域。对每个尝试的区域，必须先取得该区域的自旋锁再尝试分配，因此在 swiotlb 整体较忙时可能发生争用。但只要不是所有区域都没有足够的空闲空间，分配请求就不会失败。)

IO_TLB_SIZE, IO_TLB_SEGSIZE, and the number of areas must all be powers of 2 as
the code uses shifting and bit masking to do many of the calculations. The
number of areas is rounded up to a power of 2 if necessary to meet this
requirement.
(IO_TLB_SIZE、IO_TLB_SEGSIZE 和区域数量都必须是 2 的幂，因为代码用移位和位掩码完成许多计算。必要时，区域数量会向上取整到 2 的幂以满足这一要求。)

The default pool is allocated with PAGE_SIZE alignment. If an alloc_align_mask
argument to swiotlb_tbl_map_single() specifies a larger alignment, one or more
initial slots in each slot set might not meet the alloc_align_mask criterium.
Because a bounce buffer allocation can't cross a slot set boundary, eliminating
those initial slots effectively reduces the max size of a bounce buffer.
Currently, there's no problem because alloc_align_mask is set based on IOMMU
granule size, and granules cannot be larger than PAGE_SIZE. But if that were to
change in the future, the initial pool allocation might need to be done with
alignment larger than PAGE_SIZE.
(默认池以 PAGE_SIZE 对齐分配。如果传给 swiotlb_tbl_map_single() 的 alloc_align_mask 参数指定了更大的对齐，每个槽组开头的一个或多个槽可能不满足 alloc_align_mask 条件。由于反弹缓冲区分配不能跨越槽组边界，剔除这些起始槽实际上会缩小反弹缓冲区的最大尺寸。目前这不成问题，因为 alloc_align_mask 基于 IOMMU 粒度大小设置，而粒度不会大于 PAGE_SIZE。但如果将来这一点改变，初始池分配可能需要以大于 PAGE_SIZE 的对齐进行。)

Dynamic swiotlb(动态 swiotlb)
-----------------------------
When CONFIG_SWIOTLB_DYNAMIC is enabled, swiotlb can do on-demand expansion of
the amount of memory available for allocation as bounce buffers. If a bounce
buffer request fails due to lack of available space, an asynchronous background
task is kicked off to allocate memory from general system memory and turn it
into an swiotlb pool. Creating an additional pool must be done asynchronously
because the memory allocation may block, and as noted above, swiotlb requests
are not allowed to block. Once the background task is kicked off, the bounce
buffer request creates a "transient pool" to avoid returning an "swiotlb full"
error. A transient pool has the size of the bounce buffer request, and is
deleted when the bounce buffer is freed. Memory for this transient pool comes
from the general system memory atomic pool so that creation does not block.
Creating a transient pool has relatively high cost, particularly in a CoCo VM
where the memory must be decrypted, so it is done only as a stopgap until the
background task can add another non-transient pool.
(启用 CONFIG_SWIOTLB_DYNAMIC 后，swiotlb 可以按需扩充可用于反弹缓冲分配的内存量。如果反弹缓冲请求因空间不足而失败，会启动一个异步后台任务，从常规系统内存中分配内存并将其变为一个 swiotlb 池。创建额外的池必须异步进行，因为内存分配可能阻塞，而如前所述 swiotlb 请求不允许阻塞。后台任务启动之后，反弹缓冲请求会创建一个"瞬态池"（transient pool）以避免返回 "swiotlb full" 错误。瞬态池的大小就是该反弹缓冲请求的大小，并在反弹缓冲区释放时删除。瞬态池的内存来自常规系统内存原子池，因此创建过程不会阻塞。创建瞬态池的代价相对较高，尤其是在必须解密内存的 CoCo VM 中，所以它只是权宜之计，直到后台任务能够添加另一个非瞬态池为止。)

Adding a dynamic pool has limitations. Like with the default pool, the memory
must be physically contiguous, so the size is limited to MAX_PAGE_ORDER pages
(e.g., 4 MiB on a typical x86 system). Due to memory fragmentation, a max size
allocation may not be available. The dynamic pool allocator tries smaller sizes
until it succeeds, but with a minimum size of 1 MiB. Given sufficient system
memory fragmentation, dynamically adding a pool might not succeed at all.
(添加动态池有局限。与默认池一样，内存必须物理连续，因此大小受限于 MAX_PAGE_ORDER 页（例如典型 x86 系统上为 4 MiB）。由于内存碎片化，最大尺寸的分配可能不可得。动态池分配器会尝试更小的尺寸直至成功，但最小为 1 MiB。如果系统内存碎片化足够严重，动态添加池可能根本无法成功。)

The number of areas in a dynamic pool may be different from the number of areas
in the default pool. Because the new pool size is typically a few MiB at most,
the number of areas will likely be smaller. For example, with a new pool size
of 4 MiB and the 256 KiB minimum area size, only 16 areas can be created. If
the system has more than 16 CPUs, multiple CPUs must share an area, creating
more lock contention.
(动态池的区域数量可能与默认池不同。由于新池的大小通常最多几 MiB，区域数量可能更少。例如，新池大小为 4 MiB、区域最小 256 KiB 时，只能创建 16 个区域。如果系统 CPU 超过 16 个，就必须多个 CPU 共享一个区域，产生更多锁争用。)

New pools added via dynamic swiotlb are linked together in a linear list.
swiotlb code frequently must search for the pool containing a particular
swiotlb physical address, so that search is linear and not performant with a
large number of dynamic pools. The data structures could be improved for
faster searches.
(通过动态 swiotlb 添加的新池以线性链表链接。swiotlb 代码经常需要搜索包含某个特定 swiotlb 物理地址的池，该搜索是线性的，在动态池很多时性能不佳。可以改进数据结构以加快搜索。)

Overall, dynamic swiotlb works best for small configurations with relatively
few CPUs. It allows the default swiotlb pool to be smaller so that memory is
not wasted, with dynamic pools making more space available if needed (as long
as fragmentation isn't an obstacle). It is less useful for large CoCo VMs.
(总体而言，动态 swiotlb 最适合 CPU 相对较少的小型配置。它允许默认 swiotlb 池更小、不浪费内存，并在需要时由动态池提供更多空间（只要碎片化不构成障碍）。它对大型 CoCo VM 用处较小。)

Data Structure Details(数据结构细节)
------------------------------------
swiotlb is managed with four primary data structures: io_tlb_mem, io_tlb_pool,
io_tlb_area, and io_tlb_slot. io_tlb_mem describes a swiotlb memory allocator,
which includes the default memory pool and any dynamic or transient pools
linked to it. Limited statistics on swiotlb usage are kept per memory allocator
and are stored in this data structure. These statistics are available under
/sys/kernel/debug/swiotlb when CONFIG_DEBUG_FS is set.
(swiotlb 由四个主要数据结构管理：io_tlb_mem、io_tlb_pool、io_tlb_area 和 io_tlb_slot。io_tlb_mem 描述一个 swiotlb 内存分配器，包括默认内存池和链接到它的任何动态或瞬态池。每个内存分配器保存有限的 swiotlb 使用统计信息，存放在该数据结构中。设置了 CONFIG_DEBUG_FS 时，这些统计信息可在 /sys/kernel/debug/swiotlb 下查看。)

io_tlb_pool describes a memory pool, either the default pool, a dynamic pool,
or a transient pool. The description includes the start and end addresses of
the memory in the pool, a pointer to an array of io_tlb_area structures, and a
pointer to an array of io_tlb_slot structures that are associated with the pool.
(io_tlb_pool 描述一个内存池——默认池、动态池或瞬态池。描述内容包括池内内存的起止地址、指向 io_tlb_area 结构数组的指针，以及指向与该池关联的 io_tlb_slot 结构数组的指针。)

io_tlb_area describes an area. The primary field is the spin lock used to
serialize access to slots in the area. The io_tlb_area array for a pool has an
entry for each area, and is accessed using a 0-based area index derived from the
calling processor ID. Areas exist solely to allow parallel access to swiotlb
from multiple CPUs.
(io_tlb_area 描述一个区域。其核心字段是用于对区域内槽的访问做串行化的自旋锁。池的 io_tlb_area 数组为每个区域各有一个表项，通过从调用处理器 ID 派生的 0 起始区域索引访问。区域存在的唯一目的，是让多个 CPU 能够并行访问 swiotlb。)

io_tlb_slot describes an individual memory slot in the pool, with size
IO_TLB_SIZE (2 KiB currently). The io_tlb_slot array is indexed by the slot
index computed from the bounce buffer address relative to the starting memory
address of the pool. The size of struct io_tlb_slot is 24 bytes, so the
overhead is about 1% of the slot size.
(io_tlb_slot 描述池中的一个内存槽，大小为 IO_TLB_SIZE（当前为 2 KiB）。io_tlb_slot 数组以槽索引为下标，该索引由反弹缓冲区地址相对池起始内存地址计算得出。struct io_tlb_slot 的大小为 24 字节，开销约为槽大小的 1%。)

The io_tlb_slot array is designed to meet several requirements. First, the DMA
APIs and the corresponding swiotlb APIs use the bounce buffer address as the
identifier for a bounce buffer. This address is returned by
swiotlb_tbl_map_single(), and then passed as an argument to
swiotlb_tbl_unmap_single() and the swiotlb_sync_*() functions.  The original
memory buffer address obviously must be passed as an argument to
swiotlb_tbl_map_single(), but it is not passed to the other APIs. Consequently,
swiotlb data structures must save the original memory buffer address so that it
can be used when doing sync operations. This original address is saved in the
io_tlb_slot array.
(io_tlb_slot 数组的设计要满足几项要求。第一，DMA API 及对应的 swiotlb API 用反弹缓冲区地址作为反弹缓冲区的标识符。该地址由 swiotlb_tbl_map_single() 返回，随后作为参数传给 swiotlb_tbl_unmap_single() 和 swiotlb_sync_*() 函数。原始内存缓冲区地址显然必须作为参数传给 swiotlb_tbl_map_single()，但不会传给其他 API。因此 swiotlb 数据结构必须保存原始内存缓冲区地址，以便在执行 sync 操作时使用。该原始地址保存在 io_tlb_slot 数组中。)

Second, the io_tlb_slot array must handle partial sync requests. In such cases,
the argument to swiotlb_sync_*() is not the address of the start of the bounce
buffer but an address somewhere in the middle of the bounce buffer, and the
address of the start of the bounce buffer isn't known to swiotlb code. But
swiotlb code must be able to calculate the corresponding original memory buffer
address to do the CPU copy dictated by the "sync". So an adjusted original
memory buffer address is populated into the struct io_tlb_slot for each slot
occupied by the bounce buffer. An adjusted "alloc_size" of the bounce buffer is
also recorded in each struct io_tlb_slot so a sanity check can be performed on
the size of the "sync" operation. The "alloc_size" field is not used except for
the sanity check.
(第二，io_tlb_slot 数组必须处理部分 sync 请求。这类请求传给 swiotlb_sync_*() 的参数不是反弹缓冲区起始地址，而是反弹缓冲区中间某处的地址，swiotlb 代码并不知道反弹缓冲区的起始地址。但 swiotlb 代码必须能算出对应的原始内存缓冲区地址，以执行"sync"所要求的 CPU 复制。因此，对于反弹缓冲区占据的每个槽，都会在相应的 struct io_tlb_slot 中填入一个调整后的原始内存缓冲区地址。每个 struct io_tlb_slot 中还记录调整后的反弹缓冲区 "alloc_size"，以便对"sync"操作的大小做健全性检查。"alloc_size" 字段除健全性检查外不作他用。)

Third, the io_tlb_slot array is used to track available slots. The "list" field
in struct io_tlb_slot records how many contiguous available slots exist starting
at that slot. A "0" indicates that the slot is occupied. A value of "1"
indicates only the current slot is available. A value of "2" indicates the
current slot and the next slot are available, etc. The maximum value is
IO_TLB_SEGSIZE, which can appear in the first slot in a slot set, and indicates
that the entire slot set is available. These values are used when searching for
available slots to use for a new bounce buffer. They are updated when allocating
a new bounce buffer and when freeing a bounce buffer. At pool creation time, the
"list" field is initialized to IO_TLB_SEGSIZE down to 1 for the slots in every
slot set.
(第三，io_tlb_slot 数组用于跟踪可用槽。struct io_tlb_slot 的 "list" 字段记录从该槽开始有多少个连续可用槽。"0" 表示该槽已被占用；"1" 表示仅当前槽可用；"2" 表示当前槽与下一槽都可用，依此类推。最大值为 IO_TLB_SEGSIZE，只能出现在槽组的第一个槽中，表示整个槽组可用。搜索用于新反弹缓冲区的可用槽时使用这些值；它们在分配新反弹缓冲区和释放反弹缓冲区时更新。池创建时，每个槽组内各槽的 "list" 字段被初始化为从 IO_TLB_SEGSIZE 递减到 1。)

Fourth, the io_tlb_slot array keeps track of any "padding slots" allocated to
meet alloc_align_mask requirements described above. When
swiotlb_tbl_map_single() allocates bounce buffer space to meet alloc_align_mask
requirements, it may allocate pre-padding space across zero or more slots. But
when swiotlb_tbl_unmap_single() is called with the bounce buffer address, the
alloc_align_mask value that governed the allocation, and therefore the
allocation of any padding slots, is not known. The "pad_slots" field records
the number of padding slots so that swiotlb_tbl_unmap_single() can free them.
The "pad_slots" value is recorded only in the first non-padding slot allocated
to the bounce buffer.
(第四，io_tlb_slot 数组还跟踪为满足上文所述 alloc_align_mask 要求而分配的"填充槽"（padding slots）。当 swiotlb_tbl_map_single() 为满足 alloc_align_mask 要求而分配反弹缓冲区空间时，可能跨越零个或多个槽分配前填充空间。但当以反弹缓冲区地址调用 swiotlb_tbl_unmap_single() 时，支配此次分配（以及任何填充槽分配）的 alloc_align_mask 值是未知的。"pad_slots" 字段记录填充槽的数量，使 swiotlb_tbl_unmap_single() 能够释放它们。"pad_slots" 值只记录在分配给反弹缓冲区的第一个非填充槽中。)

Restricted pools(受限内存池)
----------------------------
The swiotlb machinery is also used for "restricted pools", which are pools of
memory separate from the default swiotlb pool, and that are dedicated for DMA
use by a particular device. Restricted pools provide a level of DMA memory
protection on systems with limited hardware protection capabilities, such as
those lacking an IOMMU. Such usage is specified by DeviceTree entries and
requires that CONFIG_DMA_RESTRICTED_POOL is set. Each restricted pool is based
on its own io_tlb_mem data structure that is independent of the main swiotlb
io_tlb_mem.
(swiotlb 机制还用于"受限内存池"（restricted pools）——独立于默认 swiotlb 池的内存池，专供特定设备做 DMA 使用。受限内存池为硬件保护能力有限的系统（如缺少 IOMMU 的系统）提供一定程度的 DMA 内存保护。这种用法由 DeviceTree 条目指定，并要求设置 CONFIG_DMA_RESTRICTED_POOL。每个受限池基于自己独立的 io_tlb_mem 数据结构，与主 swiotlb 的 io_tlb_mem 无关。)

Restricted pools add swiotlb_alloc() and swiotlb_free() APIs, which are called
from the dma_alloc_*() and dma_free_*() APIs. The swiotlb_alloc/free() APIs
allocate/free slots from/to the restricted pool directly and do not go through
swiotlb_tbl_map/unmap_single().
(受限池增加了 swiotlb_alloc() 和 swiotlb_free() API，由 dma_alloc_*() 和 dma_free_*() API 调用。swiotlb_alloc/free() API 直接在受限池中分配/释放槽，不经过 swiotlb_tbl_map/unmap_single()。)
