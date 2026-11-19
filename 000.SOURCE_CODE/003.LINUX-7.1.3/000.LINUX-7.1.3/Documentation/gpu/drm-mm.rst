=========================================
DRM Memory Management(DRM 内存管理)
=========================================

Modern Linux systems require large amount of graphics memory to store
frame buffers, textures, vertices and other graphics-related data. Given
the very dynamic nature of many of that data, managing graphics memory
efficiently is thus crucial for the graphics stack and plays a central
role in the DRM infrastructure.
(现代 Linux 系统需要大量显存来存储帧缓冲、纹理、顶点和其他图形相关数据。鉴于其中许多数据的高度动态特性，高效地管理显存对图形栈至关重要，并在 DRM 基础设施中扮演核心角色。)

The DRM core includes two memory managers, namely Translation Table Manager
(TTM) and Graphics Execution Manager (GEM). TTM was the first DRM memory
manager to be developed and tried to be a one-size-fits-them all
solution. It provides a single userspace API to accommodate the need of
all hardware, supporting both Unified Memory Architecture (UMA) devices
and devices with dedicated video RAM (i.e. most discrete video cards).
This resulted in a large, complex piece of code that turned out to be
hard to use for driver development.
(DRM 核心包含两个内存管理器，即转换表管理器 (Translation Table Manager, TTM) 和图形执行管理器 (Graphics Execution Manager, GEM)。TTM 是第一个被开发的 DRM 内存管理器，试图成为一把万能钥匙式的解决方案。它提供单一的用户态 API 来满足所有硬件的需求，同时支持统一内存架构 (UMA) 设备和带独立显存的设备 (即大多数独立显卡)。这导致了一大块庞大而复杂的代码，结果被证明难以用于驱动开发。)

GEM started as an Intel-sponsored project in reaction to TTM's
complexity. Its design philosophy is completely different: instead of
providing a solution to every graphics memory-related problems, GEM
identified common code between drivers and created a support library to
share it. GEM has simpler initialization and execution requirements than
TTM, but has no video RAM management capabilities and is thus limited to
UMA devices.
(GEM 作为一个由 Intel 赞助的项目而启动，是对 TTM 复杂性的回应。它的设计哲学完全不同：GEM 不是为每一个与图形内存相关的问题提供解决方案，而是识别出驱动之间的公共代码，并创建一个支持库来共享这些代码。GEM 的初始化和执行要求比 TTM 简单，但没有显存管理能力，因此仅限于 UMA 设备。)

The Translation Table Manager (TTM)(转换表管理器)
==================================================

.. kernel-doc:: drivers/gpu/drm/ttm/ttm_module.c
   :doc: TTM

.. kernel-doc:: include/drm/ttm/ttm_caching.h
   :internal:

TTM device object reference(TTM 设备对象参考)
---------------------------------------------

.. kernel-doc:: include/drm/ttm/ttm_device.h
   :internal:

.. kernel-doc:: drivers/gpu/drm/ttm/ttm_device.c
   :export:

TTM resource placement reference(TTM 资源放置参考)
--------------------------------------------------

.. kernel-doc:: include/drm/ttm/ttm_placement.h
   :internal:

TTM resource object reference(TTM 资源对象参考)
-----------------------------------------------

.. kernel-doc:: include/drm/ttm/ttm_resource.h
   :internal:

.. kernel-doc:: drivers/gpu/drm/ttm/ttm_resource.c
   :export:

TTM TT object reference(TTM TT 对象参考)
----------------------------------------

.. kernel-doc:: include/drm/ttm/ttm_tt.h
   :internal:

.. kernel-doc:: drivers/gpu/drm/ttm/ttm_tt.c
   :export:

TTM page pool reference(TTM 页池参考)
-------------------------------------

.. kernel-doc:: include/drm/ttm/ttm_pool.h
   :internal:

.. kernel-doc:: drivers/gpu/drm/ttm/ttm_pool.c
   :export:

The Graphics Execution Manager (GEM)(图形执行管理器)
----------------------------------------------------

The GEM design approach has resulted in a memory manager that doesn't
provide full coverage of all (or even all common) use cases in its
userspace or kernel API. GEM exposes a set of standard memory-related
operations to userspace and a set of helper functions to drivers, and
let drivers implement hardware-specific operations with their own
private API.
(GEM 的设计方法造就了一个在用户态或内核 API 中并不全面覆盖所有 (甚至所有常见) 用例的内存管理器。GEM 向用户态暴露一组标准的内存相关操作，向驱动提供一组辅助函数，并让驱动用自己的私有 API 实现硬件特定的操作。)

The GEM userspace API is described in the `GEM - the Graphics Execution
Manager <http://lwn.net/Articles/283798/>`__ article on LWN. While
slightly outdated, the document provides a good overview of the GEM API
principles. Buffer allocation and read and write operations, described
as part of the common GEM API, are currently implemented using
driver-specific ioctls.
(GEM 的用户态 API 在 LWN 上的 `GEM - the Graphics Execution Manager <http://lwn.net/Articles/283798/>`__ 一文中有描述。该文档虽然略显过时，但对 GEM API 的原理提供了很好的概览。作为公共 GEM API 一部分描述的缓冲区分配和读写操作，目前是使用驱动特定的 ioctl 实现的。)

GEM is data-agnostic. It manages abstract buffer objects without knowing
what individual buffers contain. APIs that require knowledge of buffer
contents or purpose, such as buffer allocation or synchronization
primitives, are thus outside of the scope of GEM and must be implemented
using driver-specific ioctls.
(GEM 与数据内容无关。它管理抽象的缓冲区对象，而不关心各个缓冲区里装的是什么。因此，需要了解缓冲区内容或用途的 API，例如缓冲区分配或同步原语，都在 GEM 的范围之外，必须用驱动特定的 ioctl 实现。)

On a fundamental level, GEM involves several operations:
(从根本上说，GEM 涉及以下几种操作：)

-  Memory allocation and freeing
-  Command execution
-  Aperture management at command execution time
(内存分配与释放、命令执行、命令执行时的孔径 (aperture) 管理)

Buffer object allocation is relatively straightforward and largely
provided by Linux's shmem layer, which provides memory to back each
object.
(缓冲区对象的分配相对简单，主要由 Linux 的 shmem 层提供，由它为每个对象提供支撑内存。)

Device-specific operations, such as command execution, pinning, buffer
read & write, mapping, and domain ownership transfers are left to
driver-specific ioctls.
(设备特定的操作，例如命令执行、固定 (pinning)、缓冲区读写、映射以及域归属转移，都留给驱动特定的 ioctl。)

GEM Initialization(GEM 初始化)
-------------------------------

Drivers that use GEM must set the DRIVER_GEM bit in the struct
:c:type:`struct drm_driver <drm_driver>` driver_features
field. The DRM core will then automatically initialize the GEM core
before calling the load operation. Behind the scene, this will create a
DRM Memory Manager object which provides an address space pool for
object allocation.
(使用 GEM 的驱动必须在 struct :c:type:`struct drm_driver <drm_driver>` 的 driver_features 字段中设置 DRIVER_GEM 位。此后，DRM 核心会在调用 load 操作之前自动初始化 GEM 核心。在这背后，它会创建一个 DRM 内存管理器对象，为对象分配提供一个地址空间池。)

In a KMS configuration, drivers need to allocate and initialize a
command ring buffer following core GEM initialization if required by the
hardware. UMA devices usually have what is called a "stolen" memory
region, which provides space for the initial framebuffer and large,
contiguous memory regions required by the device. This space is
typically not managed by GEM, and must be initialized separately into
its own DRM MM object.
(在 KMS 配置中，如果硬件有要求，驱动需要在核心 GEM 初始化之后分配并初始化一个命令环形缓冲区。UMA 设备通常有一块所谓的 "被窃取" (stolen) 内存区域，它为初始帧缓冲以及设备所需的大块连续内存区域提供空间。这块空间通常不由 GEM 管理，必须单独初始化为它自己的 DRM MM 对象。)

GEM Objects Creation(GEM 对象创建)
----------------------------------

GEM splits creation of GEM objects and allocation of the memory that
backs them in two distinct operations.
(GEM 把 GEM 对象的创建与为其分配支撑内存拆分为两个独立的操作。)

GEM objects are represented by an instance of struct :c:type:`struct
drm_gem_object <drm_gem_object>`. Drivers usually need to
extend GEM objects with private information and thus create a
driver-specific GEM object structure type that embeds an instance of
struct :c:type:`struct drm_gem_object <drm_gem_object>`.
(GEM 对象由 struct :c:type:`struct drm_gem_object <drm_gem_object>` 的实例表示。驱动通常需要在 GEM 对象中扩展私有信息，因此会创建一个驱动特定的 GEM 对象结构体类型，其中内嵌一个 struct :c:type:`struct drm_gem_object <drm_gem_object>` 实例。)

To create a GEM object, a driver allocates memory for an instance of its
specific GEM object type and initializes the embedded struct
:c:type:`struct drm_gem_object <drm_gem_object>` with a call
to drm_gem_object_init(). The function takes a pointer
to the DRM device, a pointer to the GEM object and the buffer object
size in bytes.
(要创建 GEM 对象，驱动为其特定的 GEM 对象类型的一个实例分配内存，并通过调用 drm_gem_object_init() 初始化其中内嵌的 struct :c:type:`struct drm_gem_object <drm_gem_object>`。该函数接受一个指向 DRM 设备的指针、一个指向 GEM 对象的指针以及以字节为单位的缓冲区对象大小。)

GEM uses shmem to allocate anonymous pageable memory.
drm_gem_object_init() will create an shmfs file of the
requested size and store it into the struct :c:type:`struct
drm_gem_object <drm_gem_object>` filp field. The memory is
used as either main storage for the object when the graphics hardware
uses system memory directly or as a backing store otherwise. Drivers
can call drm_gem_huge_mnt_create() to create, mount and use a huge
shmem mountpoint instead of the default one ('shm_mnt'). For builds
with CONFIG_TRANSPARENT_HUGEPAGE enabled, further calls to
drm_gem_object_init() will let shmem allocate huge pages when
possible.
(GEM 使用 shmem 分配匿名可分页内存。drm_gem_object_init() 会创建一个所请求大小的 shmfs 文件，并把它存入 struct :c:type:`struct drm_gem_object <drm_gem_object>` 的 filp 字段。当图形硬件直接使用系统内存时，这块内存用作对象的主存储；否则用作后备存储。驱动可以调用 drm_gem_huge_mnt_create() 来创建、挂载并使用一个 huge shmem 挂载点，代替默认的挂载点 ('shm_mnt')。对于启用了 CONFIG_TRANSPARENT_HUGEPAGE 的构建，后续对 drm_gem_object_init() 的调用会让 shmem 在可能时分配大页。)

Drivers are responsible for the actual physical pages allocation by
calling shmem_read_mapping_page_gfp() for each page.
Note that they can decide to allocate pages when initializing the GEM
object, or to delay allocation until the memory is needed (for instance
when a page fault occurs as a result of a userspace memory access or
when the driver needs to start a DMA transfer involving the memory).
(驱动负责实际的物理页分配，方法是逐页调用 shmem_read_mapping_page_gfp()。注意，驱动可以决定在初始化 GEM 对象时就分配页面，也可以把分配推迟到真正需要内存的时候 (例如用户态内存访问引发缺页时，或驱动需要发起涉及该内存的 DMA 传输时)。)

Anonymous pageable memory allocation is not always desired, for instance
when the hardware requires physically contiguous system memory as is
often the case in embedded devices. Drivers can create GEM objects with
no shmfs backing (called private GEM objects) by initializing them with a call
to drm_gem_private_object_init() instead of drm_gem_object_init(). Storage for
private GEM objects must be managed by drivers.
(匿名可分页内存的分配并不总是想要的，例如硬件要求物理连续的系统内存时——这在嵌入式设备中很常见。驱动可以通过调用 drm_gem_private_object_init() (而不是 drm_gem_object_init()) 来初始化，从而创建没有 shmfs 后备的 GEM 对象 (称为私有 GEM 对象)。私有 GEM 对象的存储必须由驱动自行管理。)

GEM Objects Lifetime(GEM 对象生命周期)
--------------------------------------

All GEM objects are reference-counted by the GEM core. References can be
acquired and release by calling drm_gem_object_get() and drm_gem_object_put()
respectively.
(所有 GEM 对象都由 GEM 核心进行引用计数。引用可以通过分别调用 drm_gem_object_get() 和 drm_gem_object_put() 来获取和释放。)

When the last reference to a GEM object is released the GEM core calls
the :c:type:`struct drm_gem_object_funcs <gem_object_funcs>` free
operation. That operation is mandatory for GEM-enabled drivers and must
free the GEM object and all associated resources.
(当对一个 GEM 对象的最后一个引用被释放时，GEM 核心会调用 :c:type:`struct drm_gem_object_funcs <gem_object_funcs>` 的 free 操作。该操作对启用 GEM 的驱动是必需的，它必须释放 GEM 对象及所有关联资源。)

void (\*free) (struct drm_gem_object \*obj); Drivers are
responsible for freeing all GEM object resources. This includes the
resources created by the GEM core, which need to be released with
drm_gem_object_release().
(void (\*free) (struct drm_gem_object \*obj); 驱动负责释放所有 GEM 对象资源。这包括由 GEM 核心创建的资源，它们需要通过 drm_gem_object_release() 释放。)

GEM Objects Naming(GEM 对象命名)
--------------------------------

Communication between userspace and the kernel refers to GEM objects
using local handles, global names or, more recently, file descriptors.
All of those are 32-bit integer values; the usual Linux kernel limits
apply to the file descriptors.
(用户态与内核之间的通信通过本地句柄、全局名称，以及最近的文件描述符来引用 GEM 对象。这些全都是 32 位整数值；文件描述符受通常的 Linux 内核限制。)


GEM handles are local to a DRM file. Applications get a handle to a GEM
object through a driver-specific ioctl, and can use that handle to refer
to the GEM object in other standard or driver-specific ioctls. Closing a
DRM file handle frees all its GEM handles and dereferences the
associated GEM objects.
(GEM 句柄是 DRM 文件局部的。应用通过驱动特定的 ioctl 获得 GEM 对象的句柄，并可以用该句柄在其他标准或驱动特定的 ioctl 中引用该 GEM 对象。关闭一个 DRM 文件句柄会释放其所有 GEM 句柄，并解除对相关 GEM 对象的引用。)

To create a handle for a GEM object drivers call drm_gem_handle_create(). The
function takes a pointer to the DRM file and the GEM object and returns a
locally unique handle.  When the handle is no longer needed drivers delete it
with a call to drm_gem_handle_delete(). Finally the GEM object associated with a
handle can be retrieved by a call to drm_gem_object_lookup().
(要为 GEM 对象创建句柄，驱动调用 drm_gem_handle_create()。该函数接受指向 DRM 文件和 GEM 对象的指针，并返回一个局部唯一的句柄。当句柄不再需要时，驱动调用 drm_gem_handle_delete() 删除它。最后，可以通过调用 drm_gem_object_lookup() 取回与句柄关联的 GEM 对象。)

Handles don't take ownership of GEM objects, they only take a reference
to the object that will be dropped when the handle is destroyed. To
avoid leaking GEM objects, drivers must make sure they drop the
reference(s) they own (such as the initial reference taken at object
creation time) as appropriate, without any special consideration for the
handle. For example, in the particular case of combined GEM object and
handle creation in the implementation of the dumb_create operation,
drivers must drop the initial reference to the GEM object before
returning the handle.
(句柄不拥有 GEM 对象的所有权，它们只是持有一个引用，该引用会在句柄销毁时被放下。为避免泄漏 GEM 对象，驱动必须确保适时放下自己拥有的引用 (比如对象创建时取得的初始引用)，而无需对句柄做任何特别考虑。例如，在 dumb_create 操作实现中同时创建 GEM 对象与句柄这一特殊情况下，驱动必须在返回句柄之前放下对 GEM 对象的初始引用。)

GEM names are similar in purpose to handles but are not local to DRM
files. They can be passed between processes to reference a GEM object
globally. Names can't be used directly to refer to objects in the DRM
API, applications must convert handles to names and names to handles
using the DRM_IOCTL_GEM_FLINK and DRM_IOCTL_GEM_OPEN ioctls
respectively. The conversion is handled by the DRM core without any
driver-specific support.
(GEM 名称的用途与句柄类似，但不局限于 DRM 文件。它们可以在进程之间传递，以全局地引用一个 GEM 对象。在 DRM API 中名称不能直接用来引用对象，应用必须分别用 DRM_IOCTL_GEM_FLINK 和 DRM_IOCTL_GEM_OPEN ioctl 把句柄转换为名称、把名称转换为句柄。这一转换由 DRM 核心处理，不需要任何驱动特定支持。)

GEM also supports buffer sharing with dma-buf file descriptors through
PRIME. GEM-based drivers must use the provided helpers functions to
implement the exporting and importing correctly. See ?. Since sharing
file descriptors is inherently more secure than the easily guessable and
global GEM names it is the preferred buffer sharing mechanism. Sharing
buffers through GEM names is only supported for legacy userspace.
Furthermore PRIME also allows cross-device buffer sharing since it is
based on dma-bufs.
(GEM 还通过 PRIME 支持使用 dma-buf 文件描述符共享缓冲区。基于 GEM 的驱动必须使用提供的辅助函数来正确实现导出与导入。见 ?。由于共享文件描述符天然比容易被猜到且全局的 GEM 名称更安全，它是首选的缓冲区共享机制。通过 GEM 名称共享缓冲区仅为遗留用户态提供支持。此外，PRIME 基于dma-buf，还允许跨设备共享缓冲区。)

GEM Objects Mapping(GEM 对象映射)
---------------------------------

Because mapping operations are fairly heavyweight GEM favours
read/write-like access to buffers, implemented through driver-specific
ioctls, over mapping buffers to userspace. However, when random access
to the buffer is needed (to perform software rendering for instance),
direct access to the object can be more efficient.
(由于映射操作相当重量级，GEM 更倾向于通过驱动特定 ioctl 实现的类似读写 (read/write) 的缓冲区访问方式，而不是把缓冲区映射到用户态。不过，当需要对缓冲区做随机访问时 (例如执行软件渲染)，直接访问对象可能更高效。)

The mmap system call can't be used directly to map GEM objects, as they
don't have their own file handle. Two alternative methods currently
co-exist to map GEM objects to userspace. The first method uses a
driver-specific ioctl to perform the mapping operation, calling
do_mmap() under the hood. This is often considered
dubious, seems to be discouraged for new GEM-enabled drivers, and will
thus not be described here.
(mmap 系统调用不能直接用来映射 GEM 对象，因为它们没有自己的文件句柄。目前有两种把 GEM 对象映射到用户态的替代方法并存。第一种方法使用驱动特定的 ioctl 执行映射操作，底层调用 do_mmap()。这种做法通常被认为不太靠谱，对新的启用 GEM 的驱动似乎也不受鼓励，因此此处不再描述。)

The second method uses the mmap system call on the DRM file handle. void
\*mmap(void \*addr, size_t length, int prot, int flags, int fd, off_t
offset); DRM identifies the GEM object to be mapped by a fake offset
passed through the mmap offset argument. Prior to being mapped, a GEM
object must thus be associated with a fake offset. To do so, drivers
must call drm_gem_create_mmap_offset() on the object.
(第二种方法在 DRM 文件句柄上使用 mmap 系统调用。void \*mmap(void \*addr, size_t length, int prot, int flags, int fd, off_t offset); DRM 通过 mmap 的 offset 参数传入的假偏移量来识别要映射的 GEM 对象。因此在被映射之前，GEM 对象必须先关联一个假偏移量。为此，驱动必须对该对象调用 drm_gem_create_mmap_offset()。)

Once allocated, the fake offset value must be passed to the application
in a driver-specific way and can then be used as the mmap offset
argument.
(假偏移量分配出来之后，必须以驱动特定的方式传递给应用，之后就可以用作 mmap 的 offset 参数。)

The GEM core provides a helper method drm_gem_mmap() to
handle object mapping. The method can be set directly as the mmap file
operation handler. It will look up the GEM object based on the offset
value and set the VMA operations to the :c:type:`struct drm_driver
<drm_driver>` gem_vm_ops field. Note that drm_gem_mmap() doesn't map memory to
userspace, but relies on the driver-provided fault handler to map pages
individually.
(GEM 核心提供辅助方法 drm_gem_mmap() 来处理对象映射。该方法可以直接设置为 mmap 文件操作处理函数。它会根据 offset 值查找 GEM 对象，并把 VMA 操作设置为 :c:type:`struct drm_driver <drm_driver>` 的 gem_vm_ops 字段。注意，drm_gem_mmap() 并不把内存映射到用户态，而是依赖驱动提供的 fault 处理函数逐页映射。)

To use drm_gem_mmap(), drivers must fill the struct :c:type:`struct drm_driver
<drm_driver>` gem_vm_ops field with a pointer to VM operations.
(要使用 drm_gem_mmap()，驱动必须把 struct :c:type:`struct drm_driver <drm_driver>` 的 gem_vm_ops 字段填成一个指向 VM 操作的指针。)

The VM operations is a :c:type:`struct vm_operations_struct <vm_operations_struct>`
made up of several fields, the more interesting ones being:
(VM 操作是一个 :c:type:`struct vm_operations_struct <vm_operations_struct>`，由若干字段组成，其中比较有意义的是：)

.. code-block:: c

	struct vm_operations_struct {
		void (*open)(struct vm_area_struct * area);
		void (*close)(struct vm_area_struct * area);
		vm_fault_t (*fault)(struct vm_fault *vmf);
	};


The open and close operations must update the GEM object reference
count. Drivers can use the drm_gem_vm_open() and drm_gem_vm_close() helper
functions directly as open and close handlers.
(open 和 close 操作必须更新 GEM 对象的引用计数。驱动可以直接使用 drm_gem_vm_open() 和 drm_gem_vm_close() 辅助函数作为 open 和 close 处理函数。)

The fault operation handler is responsible for mapping pages to
userspace when a page fault occurs. Depending on the memory allocation
scheme, drivers can allocate pages at fault time, or can decide to
allocate memory for the GEM object at the time the object is created.
(fault 操作处理函数负责在发生缺页时把页面映射到用户态。取决于内存分配方案，驱动可以在缺页时分配页面，也可以决定在创建 GEM 对象时就为它分配好内存。)

Drivers that want to map the GEM object upfront instead of handling page
faults can implement their own mmap file operation handler.
(想要预先映射 GEM 对象而不是处理缺页的驱动，可以实现自己的 mmap 文件操作处理函数。)

In order to reduce page table overhead, if the internal shmem mountpoint
"shm_mnt" is configured to use transparent huge pages (for builds with
CONFIG_TRANSPARENT_HUGEPAGE enabled) and if the shmem backing store
managed to allocate a huge page for a faulty address, the fault handler
will first attempt to insert that huge page into the VMA before falling
back to individual page insertion. mmap() user address alignment for GEM
objects is handled by providing a custom get_unmapped_area file
operation which forwards to the shmem backing store. For most drivers,
which don't create a huge mountpoint by default or through a module
parameter, transparent huge pages can be enabled by either setting the
"transparent_hugepage_shmem" kernel parameter or the
"/sys/kernel/mm/transparent_hugepage/shmem_enabled" sysfs knob.
(为了减少页表开销，如果内部 shmem 挂载点 "shm_mnt" 被配置为使用透明大页 (对启用了 CONFIG_TRANSPARENT_HUGEPAGE 的构建)，并且 shmem 后备存储成功为发生缺页的地址分配了大页，fault 处理函数会先尝试把那个大页插入 VMA，然后再退回到逐页插入。GEM 对象的 mmap() 用户地址对齐是通过提供一个自定义的 get_unmapped_area 文件操作来处理的，它转发给 shmem 后备存储。对大多数默认或通过模块参数都不创建 huge 挂载点的驱动来说，可以通过设置 "transparent_hugepage_shmem" 内核参数或 "/sys/kernel/mm/transparent_hugepage/shmem_enabled" sysfs 开关来启用透明大页。)

For platforms without MMU the GEM core provides a helper method
drm_gem_dma_get_unmapped_area(). The mmap() routines will call this to get a
proposed address for the mapping.
(对没有 MMU 的平台，GEM 核心提供辅助方法 drm_gem_dma_get_unmapped_area()。mmap() 例程会调用它来获取映射的建议地址。)

To use drm_gem_dma_get_unmapped_area(), drivers must fill the struct
:c:type:`struct file_operations <file_operations>` get_unmapped_area field with
a pointer on drm_gem_dma_get_unmapped_area().
(要使用 drm_gem_dma_get_unmapped_area()，驱动必须把 struct :c:type:`struct file_operations <file_operations>` 的 get_unmapped_area 字段填成指向 drm_gem_dma_get_unmapped_area() 的指针。)

More detailed information about get_unmapped_area can be found in
Documentation/admin-guide/mm/nommu-mmap.rst
(关于 get_unmapped_area 的更详细信息见 Documentation/admin-guide/mm/nommu-mmap.rst。)

Memory Coherency(内存一致性)
-----------------------------

When mapped to the device or used in a command buffer, backing pages for
an object are flushed to memory and marked write combined so as to be
coherent with the GPU. Likewise, if the CPU accesses an object after the
GPU has finished rendering to the object, then the object must be made
coherent with the CPU's view of memory, usually involving GPU cache
flushing of various kinds. This core CPU<->GPU coherency management is
provided by a device-specific ioctl, which evaluates an object's current
domain and performs any necessary flushing or synchronization to put the
object into the desired coherency domain (note that the object may be
busy, i.e. an active render target; in that case, setting the domain
blocks the client and waits for rendering to complete before performing
any necessary flushing operations).
(当对象被映射到设备或用于命令缓冲区时，它的支撑页会被刷入内存并标记为写合并 (write combined)，以便与 GPU 保持一致。同样，如果在 GPU 完成对对象的渲染之后 CPU 访问该对象，那么必须使对象与 CPU 的内存视图一致，这通常涉及各种 GPU 缓存刷新。这种核心的 CPU<->GPU 一致性管理由一个设备特定的 ioctl 提供，它评估对象当前的域，并执行任何必要的刷新或同步，把对象放入期望的一致性域 (注意对象可能正忙，即是一个活跃的渲染目标；在这种情况下，设置域会阻塞客户端，等待渲染完成之后才执行任何必要的刷新操作)。)

Command Execution(命令执行)
---------------------------

Perhaps the most important GEM function for GPU devices is providing a
command execution interface to clients. Client programs construct
command buffers containing references to previously allocated memory
objects, and then submit them to GEM. At that point, GEM takes care to
bind all the objects into the GTT, execute the buffer, and provide
necessary synchronization between clients accessing the same buffers.
This often involves evicting some objects from the GTT and re-binding
others (a fairly expensive operation), and providing relocation support
which hides fixed GTT offsets from clients. Clients must take care not
to submit command buffers that reference more objects than can fit in
the GTT; otherwise, GEM will reject them and no rendering will occur.
Similarly, if several objects in the buffer require fence registers to
be allocated for correct rendering (e.g. 2D blits on pre-965 chips),
care must be taken not to require more fence registers than are
available to the client. Such resource management should be abstracted
from the client in libdrm.
(对 GPU 设备来说，GEM 或许最重要的功能是向客户端提供命令执行接口。客户端程序构造包含对先前分配的内存对象之引用的命令缓冲区，然后提交给 GEM。此时，GEM 负责把所有对象绑定进 GTT、执行缓冲区，并为访问相同缓冲区的客户端之间提供必要的同步。这通常涉及把一些对象从 GTT 逐出、把另一些重新绑定 (这是相当昂贵的操作)，以及提供向客户端隐藏固定 GTT 偏移的重定位 (relocation) 支持。客户端必须注意不要提交引用的对象数超出 GTT 容量的命令缓冲区；否则 GEM 会拒绝它们，渲染将不会发生。类似地，如果缓冲区中有多个对象需要分配 fence 寄存器才能正确渲染 (例如 pre-965 芯片上的 2D 位块传输)，必须注意不要要求超出客户端可用数量的 fence 寄存器。这类资源管理应当在 libdrm 中对客户端抽象掉。)

GEM Function Reference(GEM 函数参考)
------------------------------------

.. kernel-doc:: include/drm/drm_gem.h
   :internal:

.. kernel-doc:: drivers/gpu/drm/drm_gem.c
   :export:

GEM DMA Helper Functions Reference(GEM DMA 辅助函数参考)
--------------------------------------------------------

.. kernel-doc:: drivers/gpu/drm/drm_gem_dma_helper.c
   :doc: dma helpers

.. kernel-doc:: include/drm/drm_gem_dma_helper.h
   :internal:

.. kernel-doc:: drivers/gpu/drm/drm_gem_dma_helper.c
   :export:

GEM SHMEM Helper Function Reference(GEM SHMEM 辅助函数参考)
-----------------------------------------------------------

.. kernel-doc:: drivers/gpu/drm/drm_gem_shmem_helper.c
   :doc: overview

.. kernel-doc:: include/drm/drm_gem_shmem_helper.h
   :internal:

.. kernel-doc:: drivers/gpu/drm/drm_gem_shmem_helper.c
   :export:

GEM VRAM Helper Functions Reference(GEM VRAM 辅助函数参考)
----------------------------------------------------------

.. kernel-doc:: drivers/gpu/drm/drm_gem_vram_helper.c
   :doc: overview

.. kernel-doc:: include/drm/drm_gem_vram_helper.h
   :internal:

.. kernel-doc:: drivers/gpu/drm/drm_gem_vram_helper.c
   :export:

GEM TTM Helper Functions Reference(GEM TTM 辅助函数参考)
--------------------------------------------------------

.. kernel-doc:: drivers/gpu/drm/drm_gem_ttm_helper.c
   :doc: overview

.. kernel-doc:: drivers/gpu/drm/drm_gem_ttm_helper.c
   :export:

VMA Offset Manager(VMA 偏移管理器)
==================================

.. kernel-doc:: drivers/gpu/drm/drm_vma_manager.c
   :doc: vma offset manager

.. kernel-doc:: include/drm/drm_vma_manager.h
   :internal:

.. kernel-doc:: drivers/gpu/drm/drm_vma_manager.c
   :export:

.. _prime_buffer_sharing:

PRIME Buffer Sharing(PRIME 缓冲区共享)
======================================

PRIME is the cross device buffer sharing framework in drm, originally
created for the OPTIMUS range of multi-gpu platforms. To userspace PRIME
buffers are dma-buf based file descriptors.
(PRIME 是 drm 中的跨设备缓冲区共享框架，最初为 OPTIMUS 系列多 GPU 平台而创建。对用户态而言，PRIME 缓冲区是基于 dma-buf 的文件描述符。)

Overview and Lifetime Rules(概述与生命周期规则)
-----------------------------------------------

.. kernel-doc:: drivers/gpu/drm/drm_prime.c
   :doc: overview and lifetime rules

PRIME Helper Functions(PRIME 辅助函数)
--------------------------------------

.. kernel-doc:: drivers/gpu/drm/drm_prime.c
   :doc: PRIME Helpers

PRIME Function References(PRIME 函数参考)
-----------------------------------------

.. kernel-doc:: include/drm/drm_prime.h
   :internal:

.. kernel-doc:: drivers/gpu/drm/drm_prime.c
   :export:

DRM MM Range Allocator(DRM MM 范围分配器)
=========================================

Overview(概述)
--------------

.. kernel-doc:: drivers/gpu/drm/drm_mm.c
   :doc: Overview

LRU Scan/Eviction Support(LRU 扫描/回收支持)
--------------------------------------------

.. kernel-doc:: drivers/gpu/drm/drm_mm.c
   :doc: lru scan roster

DRM MM Range Allocator Function References(DRM MM 范围分配器函数参考)
---------------------------------------------------------------------

.. kernel-doc:: include/drm/drm_mm.h
   :internal:

.. kernel-doc:: drivers/gpu/drm/drm_mm.c
   :export:

.. _drm_gpuvm:

DRM GPUVM(GPU 虚拟内存管理)
===========================

Overview(概述)
--------------

.. kernel-doc:: drivers/gpu/drm/drm_gpuvm.c
   :doc: Overview

Split and Merge(拆分与合并)
---------------------------

.. kernel-doc:: drivers/gpu/drm/drm_gpuvm.c
   :doc: Split and Merge

.. _drm_gpuvm_locking:

Locking(加锁)
-------------

.. kernel-doc:: drivers/gpu/drm/drm_gpuvm.c
   :doc: Locking

Examples(示例)
--------------

.. kernel-doc:: drivers/gpu/drm/drm_gpuvm.c
   :doc: Examples

DRM GPUVM Function References(DRM GPUVM 函数参考)
-------------------------------------------------

.. kernel-doc:: include/drm/drm_gpuvm.h
   :internal:

.. kernel-doc:: drivers/gpu/drm/drm_gpuvm.c
   :export:

DRM Buddy Allocator(DRM Buddy 分配器)
=====================================

Buddy Allocator Function References (GPU buddy)(Buddy 分配器函数参考)
---------------------------------------------------------------------

.. kernel-doc:: drivers/gpu/buddy.c
   :export:

DRM Buddy Specific Logging Function References(DRM Buddy 专用日志函数参考)
--------------------------------------------------------------------------

.. kernel-doc:: drivers/gpu/drm/drm_buddy.c
   :export:

DRM Cache Handling and Fast WC memcpy()(DRM 缓存处理与快速 WC memcpy)
=====================================================================

.. kernel-doc:: drivers/gpu/drm/drm_cache.c
   :export:

.. _drm_sync_objects:

DRM Sync Objects(DRM 同步对象)
==============================

.. kernel-doc:: drivers/gpu/drm/drm_syncobj.c
   :doc: Overview

.. kernel-doc:: include/drm/drm_syncobj.h
   :internal:

.. kernel-doc:: drivers/gpu/drm/drm_syncobj.c
   :export:

DRM Execution context(DRM 执行上下文)
=====================================

.. kernel-doc:: drivers/gpu/drm/drm_exec.c
   :doc: Overview

.. kernel-doc:: include/drm/drm_exec.h
   :internal:

.. kernel-doc:: drivers/gpu/drm/drm_exec.c
   :export:

GPU Scheduler(GPU 调度器)
=========================

Overview(概述)
--------------

.. kernel-doc:: drivers/gpu/drm/scheduler/sched_main.c
   :doc: Overview

Flow Control(流控)
------------------

.. kernel-doc:: drivers/gpu/drm/scheduler/sched_main.c
   :doc: Flow Control

Scheduler Function References(调度器函数参考)
---------------------------------------------

.. kernel-doc:: include/drm/gpu_scheduler.h
   :internal:

.. kernel-doc:: drivers/gpu/drm/scheduler/sched_main.c
   :export:

.. kernel-doc:: drivers/gpu/drm/scheduler/sched_entity.c
   :export:
