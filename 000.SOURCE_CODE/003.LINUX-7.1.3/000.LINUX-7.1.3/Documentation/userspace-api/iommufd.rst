.. SPDX-License-Identifier: GPL-2.0+

========================================
IOMMUFD(基于文件描述符的 IOMMU 用户接口)
========================================

:Author: Jason Gunthorpe
:Author: Kevin Tian

(作者: Jason Gunthorpe)
(作者: Kevin Tian)

Overview(概述)
==============

IOMMUFD is the user API to control the IOMMU subsystem as it relates to managing
IO page tables from userspace using file descriptors. It intends to be general
and consumable by any driver that wants to expose DMA to userspace. These
drivers are eventually expected to deprecate any internal IOMMU logic
they may already/historically implement (e.g. vfio_iommu_type1.c).
(IOMMUFD 是控制 IOMMU 子系统的用户 API，涉及通过文件描述符从用户空间管理 I/O 页表。它力求通用，可被任何想把 DMA 暴露给用户空间的驱动使用。这些驱动最终应弃用它们已经/历史上实现的内部 IOMMU 逻辑(例如 vfio_iommu_type1.c)。)

At minimum iommufd provides universal support of managing I/O address spaces and
I/O page tables for all IOMMUs, with room in the design to add non-generic
features to cater to specific hardware functionality.
(至少，iommufd 为所有 IOMMU 提供管理 I/O 地址空间与 I/O 页表的通用支持，并在设计中留出了添加非通用特性的空间，以满足特定硬件功能。)

In this context the capital letter (IOMMUFD) refers to the subsystem while the
small letter (iommufd) refers to the file descriptors created via /dev/iommu for
use by userspace.
(在此上下文中，大写的 (IOMMUFD) 指子系统，小写的 (iommufd) 指通过 /dev/iommu 创建、供用户空间使用的文件描述符。)

Key Concepts(核心概念)
======================

User Visible Objects(用户可见对象)
----------------------------------

Following IOMMUFD objects are exposed to userspace:
(以下 IOMMUFD 对象暴露给用户空间：)

- IOMMUFD_OBJ_IOAS, representing an I/O address space (IOAS), allowing map/unmap
  of user space memory into ranges of I/O Virtual Address (IOVA).
  (IOMMUFD_OBJ_IOAS，表示一个 I/O 地址空间 (IOAS)，允许把用户空间内存 map/unmap 到 I/O 虚拟地址 (IOVA) 的区间。)

  The IOAS is a functional replacement for the VFIO container, and like the VFIO
  container it copies an IOVA map to a list of iommu_domains held within it.
  (IOAS 是 VFIO container 的功能替代品，与 VFIO container 一样，它把一份 IOVA 映射复制到其内部持有的 iommu_domain 列表。)

- IOMMUFD_OBJ_DEVICE, representing a device that is bound to iommufd by an
  external driver.
  (IOMMUFD_OBJ_DEVICE，表示由外部驱动绑定到 iommufd 的设备。)

- IOMMUFD_OBJ_HWPT_PAGING, representing an actual hardware I/O page table
  (i.e. a single struct iommu_domain) managed by the iommu driver. "PAGING"
  primarily indicates this type of HWPT should be linked to an IOAS. It also
  indicates that it is backed by an iommu_domain with __IOMMU_DOMAIN_PAGING
  feature flag. This can be either an UNMANAGED stage-1 domain for a device
  running in the user space, or a nesting parent stage-2 domain for mappings
  from guest-level physical addresses to host-level physical addresses.
  (IOMMUFD_OBJ_HWPT_PAGING，表示由 iommu 驱动管理的一个实际硬件 I/O 页表(即单个 struct iommu_domain)。"PAGING" 主要表明这类 HWPT 应链接到一个 IOAS。它还表明其底层是一个带有 __IOMMU_DOMAIN_PAGING 特性标志的 iommu_domain。它可以是为运行在用户空间的设备准备的 UNMANAGED stage-1 域，也可以是用于从客户机级物理地址到宿主机级物理地址映射的嵌套父 (nesting parent) stage-2 域。)

  The IOAS has a list of HWPT_PAGINGs that share the same IOVA mapping and
  it will synchronize its mapping with each member HWPT_PAGING.
  (IOAS 持有一个共享相同 IOVA 映射的 HWPT_PAGING 列表，并会把自身映射与每个成员 HWPT_PAGING 同步。)

- IOMMUFD_OBJ_HWPT_NESTED, representing an actual hardware I/O page table
  (i.e. a single struct iommu_domain) managed by user space (e.g. guest OS).
  "NESTED" indicates that this type of HWPT should be linked to an HWPT_PAGING.
  It also indicates that it is backed by an iommu_domain that has a type of
  IOMMU_DOMAIN_NESTED. This must be a stage-1 domain for a device running in
  the user space (e.g. in a guest VM enabling the IOMMU nested translation
  feature.) As such, it must be created with a given nesting parent stage-2
  domain to associate to. This nested stage-1 page table managed by the user
  space usually has mappings from guest-level I/O virtual addresses to guest-
  level physical addresses.
  (IOMMUFD_OBJ_HWPT_NESTED，表示由用户空间(例如客户机操作系统)管理的一个实际硬件 I/O 页表(即单个 struct iommu_domain)。"NESTED" 表明这类 HWPT 应链接到一个 HWPT_PAGING。它还表明其底层是一个类型为 IOMMU_DOMAIN_NESTED 的 iommu_domain。它必须是为运行在用户空间的设备(例如在启用了 IOMMU 嵌套转换特性的客户机 VM 中)准备的 stage-1 域。因此，创建它时必须给定一个要关联的嵌套父 stage-2 域。这个由用户空间管理的嵌套 stage-1 页表通常包含从客户机级 I/O 虚拟地址到客户机级物理地址的映射。)

- IOMMUFD_FAULT, representing a software queue for an HWPT reporting IO page
  faults using the IOMMU HW's PRI (Page Request Interface). This queue object
  provides user space an FD to poll the page fault events and also to respond
  to those events. A FAULT object must be created first to get a fault_id that
  could be then used to allocate a fault-enabled HWPT via the IOMMU_HWPT_ALLOC
  command by setting the IOMMU_HWPT_FAULT_ID_VALID bit in its flags field.
  (IOMMUFD_FAULT，表示一个软件队列，供 HWPT 使用 IOMMU 硬件的 PRI (Page Request Interface) 报告 I/O 缺页。该队列对象为用户空间提供一个 FD，用于轮询缺页事件并对这些事件做出响应。必须先创建 FAULT 对象得到 fault_id，随后才能通过 IOMMU_HWPT_ALLOC 命令、在其 flags 字段中设置 IOMMU_HWPT_FAULT_ID_VALID 位来分配启用了 fault 的 HWPT。)

- IOMMUFD_OBJ_VIOMMU, representing a slice of the physical IOMMU instance,
  passed to or shared with a VM. It may be some HW-accelerated virtualization
  features and some SW resources used by the VM. For examples:
  (IOMMUFD_OBJ_VIOMMU，表示物理 IOMMU 实例的一个切片，被传给或共享给一个 VM。它可以是某些硬件加速的虚拟化特性，以及 VM 使用的一些软件资源。例如：)

  * Security namespace for guest owned ID, e.g. guest-controlled cache tags
  * Non-device-affiliated event reporting, e.g. invalidation queue errors
  * Access to a shareable nesting parent pagetable across physical IOMMUs
  * Virtualization of various platforms IDs, e.g. RIDs and others
  * Delivery of paravirtualized invalidation
  * Direct assigned invalidation queues
  * Direct assigned interrupts

  (客户机自有 ID 的安全命名空间，例如客户机控制的缓存标签)
  (不与设备关联的事件上报，例如失效队列错误)
  (跨物理 IOMMU 访问可共享的嵌套父页表)
  (各种平台 ID 的虚拟化，例如 RID 等)
  (半虚拟化失效的递交)
  (直通的失效队列)
  (直通的中断)

  Such a vIOMMU object generally has the access to a nesting parent pagetable
  to support some HW-accelerated virtualization features. So, a vIOMMU object
  must be created given a nesting parent HWPT_PAGING object, and then it would
  encapsulate that HWPT_PAGING object. Therefore, a vIOMMU object can be used
  to allocate an HWPT_NESTED object in place of the encapsulated HWPT_PAGING.
  (这样的 vIOMMU 对象通常可以访问一个嵌套父页表，以支持某些硬件加速的虚拟化特性。因此，创建 vIOMMU 对象时必须给定一个嵌套父 HWPT_PAGING 对象，随后它会封装该 HWPT_PAGING 对象。于是，vIOMMU 对象可以用来分配 HWPT_NESTED 对象，取代被封装的 HWPT_PAGING。)

  .. note::

     The name "vIOMMU" isn't necessarily identical to a virtualized IOMMU in a
     VM. A VM can have one giant virtualized IOMMU running on a machine having
     multiple physical IOMMUs, in which case the VMM will dispatch the requests
     or configurations from this single virtualized IOMMU instance to multiple
     vIOMMU objects created for individual slices of different physical IOMMUs.
     In other words, a vIOMMU object is always a representation of one physical
     IOMMU, not necessarily of a virtualized IOMMU. For VMMs that want the full
     virtualization features from physical IOMMUs, it is suggested to build the
     same number of virtualized IOMMUs as the number of physical IOMMUs, so the
     passed-through devices would be connected to their own virtualized IOMMUs
     backed by corresponding vIOMMU objects, in which case a guest OS would do
     the "dispatch" naturally instead of VMM trappings.
     (注意："vIOMMU" 这个名字不一定等同于 VM 中的虚拟化 IOMMU。一台 VM 可以在拥有多个物理 IOMMU 的机器上运行一个巨大的虚拟化 IOMMU，此时 VMM 会把这个单一虚拟化 IOMMU 实例的请求或配置分发给为不同物理 IOMMU 的各个切片创建的多个 vIOMMU 对象。换言之，vIOMMU 对象始终是一个物理 IOMMU 的代表，而不一定是虚拟化 IOMMU 的代表。对于想要获得物理 IOMMU 全部虚拟化特性的 VMM，建议创建与物理 IOMMU 数量相同的虚拟化 IOMMU，使直通设备连接到各自由对应 vIOMMU 对象支撑的虚拟化 IOMMU；这种情况下，客户机操作系统会自然地完成"分发"，而不需要 VMM 从中周旋。)

- IOMMUFD_OBJ_VDEVICE, representing a virtual device for an IOMMUFD_OBJ_DEVICE
  against an IOMMUFD_OBJ_VIOMMU. This virtual device holds the device's virtual
  information or attributes (related to the vIOMMU) in a VM. An immediate vDATA
  example can be the virtual ID of the device on a vIOMMU, which is a unique ID
  that VMM assigns to the device for a translation channel/port of the vIOMMU,
  e.g. vSID of ARM SMMUv3, vDeviceID of AMD IOMMU, and vRID of Intel VT-d to a
  Context Table. Potential use cases of some advanced security information can
  be forwarded via this object too, such as security level or realm information
  in a Confidential Compute Architecture. A VMM should create a vDEVICE object
  to forward all the device information in a VM, when it connects a device to a
  vIOMMU, which is a separate ioctl call from attaching the same device to an
  HWPT_PAGING that the vIOMMU holds.
  (IOMMUFD_OBJ_VDEVICE，表示 IOMMUFD_OBJ_DEVICE 相对于某个 IOMMUFD_OBJ_VIOMMU 的虚拟设备。该虚拟设备保存设备在 VM 中与该 vIOMMU 相关的虚拟信息或属性。一个直观的 vDATA 例子是设备在 vIOMMU 上的虚拟 ID，即 VMM 分配给设备、用于 vIOMMU 某个转换通道/端口的唯一 ID，例如 ARM SMMUv3 的 vSID、AMD IOMMU 的 vDeviceID，以及 Intel VT-d 中对应 Context Table 的 vRID。一些高级安全信息的潜在用例也可以经由该对象转发，例如机密计算架构 (Confidential Compute Architecture) 中的安全级别或 realm 信息。当 VMM 把设备连接到 vIOMMU 时，应创建 vDEVICE 对象以转发 VM 中的全部设备信息；这是与把同一设备附加到该 vIOMMU 所持有的 HWPT_PAGING 不同的另一次 ioctl 调用。)

- IOMMUFD_OBJ_VEVENTQ, representing a software queue for a vIOMMU to report its
  events such as translation faults occurred to a nested stage-1 (excluding I/O
  page faults that should go through IOMMUFD_OBJ_FAULT) and HW-specific events.
  This queue object provides user space an FD to poll/read the vIOMMU events. A
  vIOMMU object must be created first to get its viommu_id, which could be then
  used to allocate a vEVENTQ. Each vIOMMU can support multiple types of vEVENTS,
  but is confined to one vEVENTQ per vEVENTQ type.
  (IOMMUFD_OBJ_VEVENTQ，表示一个软件队列，供 vIOMMU 上报其事件，例如发生在嵌套 stage-1 上的转换故障(不包括应经由 IOMMUFD_OBJ_FAULT 处理的 I/O 缺页)以及硬件特定事件。该队列对象为用户空间提供一个 FD 来轮询/读取 vIOMMU 事件。必须先创建 vIOMMU 对象取得其 viommu_id，随后才能用它分配 vEVENTQ。每个 vIOMMU 可以支持多种类型的 vEVENT，但每种 vEVENTQ 类型仅限一个 vEVENTQ。)

- IOMMUFD_OBJ_HW_QUEUE, representing a hardware accelerated queue, as a subset
  of IOMMU's virtualization features, for the IOMMU HW to directly read or write
  the virtual queue memory owned by a guest OS. This HW-acceleration feature can
  allow VM to work with the IOMMU HW directly without a VM Exit, so as to reduce
  overhead from the hypercalls. Along with the HW QUEUE object, iommufd provides
  user space an mmap interface for VMM to mmap a physical MMIO region from the
  host physical address space to the guest physical address space, allowing the
  guest OS to directly control the allocated HW QUEUE. Thus, when allocating a
  HW QUEUE, the VMM must request a pair of mmap info (offset/length) and pass in
  exactly to an mmap syscall via its offset and length arguments.
  (IOMMUFD_OBJ_HW_QUEUE，表示一个硬件加速队列，作为 IOMMU 虚拟化特性的一个子集，供 IOMMU 硬件直接读写客户机操作系统拥有的虚拟队列内存。这一硬件加速特性可以让 VM 无需 VM Exit 即可直接与 IOMMU 硬件协同工作，从而减少超级调用的开销。与 HW QUEUE 对象一起，iommufd 为用户空间提供一个 mmap 接口，让 VMM 把宿主机物理地址空间中的一个物理 MMIO 区域 mmap 到客户机物理地址空间，使客户机操作系统能够直接控制所分配的 HW QUEUE。因此，分配 HW QUEUE 时，VMM 必须请求一对 mmap 信息 (offset/length)，并原封不动地通过 mmap 系统调用的 offset 与 length 参数传入。)

All user-visible objects are destroyed via the IOMMU_DESTROY uAPI.
(所有用户可见对象都通过 IOMMU_DESTROY uAPI 销毁。)

The diagrams below show relationships between user-visible objects and kernel
datastructures (external to iommufd), with numbers referred to operations
creating the objects and links::

  _______________________________________________________________________
 |                      iommufd (HWPT_PAGING only)                       |
 |                                                                       |
 |        [1]                  [3]                                [2]    |
 |  ________________      _____________                        ________  |
 | |                |    |             |                      |        | |
 | |      IOAS      |<---| HWPT_PAGING |<---------------------| DEVICE | |
 | |________________|    |_____________|                      |________| |
 |         |                    |                                  |     |
 |_________|____________________|__________________________________|_____|
           |                    |                                  |
           |              ______v_____                          ___v__
           | PFN storage |  (paging)  |                        |struct|
           |------------>|iommu_domain|<-----------------------|device|
                         |____________|                        |______|

  _______________________________________________________________________
 |                      iommufd (with HWPT_NESTED)                       |
 |                                                                       |
 |        [1]                  [3]                [4]             [2]    |
 |  ________________      _____________      _____________     ________  |
 | |                |    |             |    |             |   |        | |
 | |      IOAS      |<---| HWPT_PAGING |<---| HWPT_NESTED |<--| DEVICE | |
 | |________________|    |_____________|    |_____________|   |________| |
 |         |                    |                  |               |     |
 |_________|____________________|__________________|_______________|_____|
           |                    |                  |               |
           |              ______v_____       ______v_____       ___v__
           | PFN storage |  (paging)  |     |  (nested)  |     |struct|
           |------------>|iommu_domain|<----|iommu_domain|<----|device|
                         |____________|     |____________|     |______|

  _______________________________________________________________________
 |                      iommufd (with vIOMMU/vDEVICE)                    |
 |                                                                       |
 |                             [5]                [6]                    |
 |                        _____________      _____________               |
 |                       |             |    |             |              |
 |      |----------------|    vIOMMU   |<---|   vDEVICE   |<----|        |
 |      |                |             |    |_____________|     |        |
 |      |                |             |                        |        |
 |      |      [1]       |             |          [4]           | [2]    |
 |      |     ______     |             |     _____________     _|______  |
 |      |    |      |    |     [3]     |    |             |   |        | |
 |      |    | IOAS |<---|(HWPT_PAGING)|<---| HWPT_NESTED |<--| DEVICE | |
 |      |    |______|    |_____________|    |_____________|   |________| |
 |      |        |              |                  |               |     |
 |______|________|______________|__________________|_______________|_____|
        |        |              |                  |               |
  ______v_____   |        ______v_____       ______v_____       ___v__
 |   struct   |  |  PFN  |  (paging)  |     |  (nested)  |     |struct|
 |iommu_device|  |------>|iommu_domain|<----|iommu_domain|<----|device|
 |____________|   storage|____________|     |____________|     |______|

(下图展示用户可见对象与内核数据结构(iommufd 之外)之间的关系，其中的编号对应创建对象与链接的操作：)

1. IOMMUFD_OBJ_IOAS is created via the IOMMU_IOAS_ALLOC uAPI. An iommufd can
   hold multiple IOAS objects. IOAS is the most generic object and does not
   expose interfaces that are specific to single IOMMU drivers. All operations
   on the IOAS must operate equally on each of the iommu_domains inside of it.
   (IOMMUFD_OBJ_IOAS 通过 IOMMU_IOAS_ALLOC uAPI 创建。一个 iommufd 可以持有多个 IOAS 对象。IOAS 是最通用的对象，不暴露任何针对单一 IOMMU 驱动的接口。对 IOAS 的所有操作必须对其内部的每个 iommu_domain 平等生效。)

2. IOMMUFD_OBJ_DEVICE is created when an external driver calls the IOMMUFD kAPI
   to bind a device to an iommufd. The driver is expected to implement a set of
   ioctls to allow userspace to initiate the binding operation. Successful
   completion of this operation establishes the desired DMA ownership over the
   device. The driver must also set the driver_managed_dma flag and must not
   touch the device until this operation succeeds.
   (当外部驱动调用 IOMMUFD kAPI 把设备绑定到 iommufd 时，会创建 IOMMUFD_OBJ_DEVICE。驱动应实现一组 ioctl，让用户空间能够发起绑定操作。这一操作的成功完成会建立对设备所期望的 DMA 所有权。驱动还必须设置 driver_managed_dma 标志，并且在该操作成功之前不得触碰设备。)

3. IOMMUFD_OBJ_HWPT_PAGING can be created in two ways:
   (IOMMUFD_OBJ_HWPT_PAGING 可以通过两种方式创建：)

   * IOMMUFD_OBJ_HWPT_PAGING is automatically created when an external driver
     calls the IOMMUFD kAPI to attach a bound device to an IOAS. Similarly the
     external driver uAPI allows userspace to initiate the attaching operation.
     If a compatible member HWPT_PAGING object exists in the IOAS's HWPT_PAGING
     list, then it will be reused. Otherwise a new HWPT_PAGING that represents
     an iommu_domain to userspace will be created, and then added to the list.
     Successful completion of this operation sets up the linkages among IOAS,
     device and iommu_domain. Once this completes the device could do DMA.
     (当外部驱动调用 IOMMUFD kAPI 把已绑定的设备附加到某个 IOAS 时，会自动创建 IOMMUFD_OBJ_HWPT_PAGING。类似地，外部驱动的 uAPI 允许用户空间发起附加操作。如果 IOAS 的 HWPT_PAGING 列表中已存在兼容的成员 HWPT_PAGING 对象，则会复用它；否则会创建一个向用户空间代表某个 iommu_domain 的新 HWPT_PAGING，并加入该列表。这一操作的成功完成会建立 IOAS、设备与 iommu_domain 之间的链接。完成后设备即可执行 DMA。)

   * IOMMUFD_OBJ_HWPT_PAGING can be manually created via the IOMMU_HWPT_ALLOC
     uAPI, provided an ioas_id via @pt_id to associate the new HWPT_PAGING to
     the corresponding IOAS object. The benefit of this manual allocation is to
     allow allocation flags (defined in enum iommufd_hwpt_alloc_flags), e.g. it
     allocates a nesting parent HWPT_PAGING if the IOMMU_HWPT_ALLOC_NEST_PARENT
     flag is set.
     (IOMMUFD_OBJ_HWPT_PAGING 也可以通过 IOMMU_HWPT_ALLOC uAPI 手动创建，经由 @pt_id 提供一个 ioas_id，把新 HWPT_PAGING 关联到对应的 IOAS 对象。手动分配的好处是可以使用分配标志(定义于 enum iommufd_hwpt_alloc_flags)，例如设置了 IOMMU_HWPT_ALLOC_NEST_PARENT 标志时，会分配一个嵌套父 HWPT_PAGING。)

4. IOMMUFD_OBJ_HWPT_NESTED can be only manually created via the IOMMU_HWPT_ALLOC
   uAPI, provided an hwpt_id or a viommu_id of a vIOMMU object encapsulating a
   nesting parent HWPT_PAGING via @pt_id to associate the new HWPT_NESTED object
   to the corresponding HWPT_PAGING object. The associating HWPT_PAGING object
   must be a nesting parent manually allocated via the same uAPI previously with
   an IOMMU_HWPT_ALLOC_NEST_PARENT flag, otherwise the allocation will fail. The
   allocation will be further validated by the IOMMU driver to ensure that the
   nesting parent domain and the nested domain being allocated are compatible.
   Successful completion of this operation sets up linkages among IOAS, device,
   and iommu_domains. Once this completes the device could do DMA via a 2-stage
   translation, a.k.a nested translation. Note that multiple HWPT_NESTED objects
   can be allocated by (and then associated to) the same nesting parent.
   (IOMMUFD_OBJ_HWPT_NESTED 只能通过 IOMMU_HWPT_ALLOC uAPI 手动创建，经由 @pt_id 提供一个 hwpt_id，或一个封装了嵌套父 HWPT_PAGING 的 vIOMMU 对象的 viommu_id，把新 HWPT_NESTED 对象关联到对应的 HWPT_PAGING 对象。所关联的 HWPT_PAGING 对象必须是先前通过同一 uAPI、带 IOMMU_HWPT_ALLOC_NEST_PARENT 标志手动分配的嵌套父，否则分配将失败。分配还会由 IOMMU 驱动进一步校验，确保嵌套父域与所分配的嵌套域兼容。这一操作的成功完成会建立 IOAS、设备与 iommu_domain 之间的链接。完成后设备即可经由两阶段转换(即嵌套转换)执行 DMA。注意，同一个嵌套父可以分配(并随后关联)多个 HWPT_NESTED 对象。)

   .. note::

      Either a manual IOMMUFD_OBJ_HWPT_PAGING or an IOMMUFD_OBJ_HWPT_NESTED is
      created via the same IOMMU_HWPT_ALLOC uAPI. The difference is at the type
      of the object passed in via the @pt_id field of struct iommufd_hwpt_alloc.
      (注意：手动创建 IOMMUFD_OBJ_HWPT_PAGING 或 IOMMUFD_OBJ_HWPT_NESTED 都是通过同一个 IOMMU_HWPT_ALLOC uAPI 完成的，区别仅在于经 struct iommufd_hwpt_alloc 的 @pt_id 字段传入的对象类型。)

5. IOMMUFD_OBJ_VIOMMU can be only manually created via the IOMMU_VIOMMU_ALLOC
   uAPI, provided a dev_id (for the device's physical IOMMU to back the vIOMMU)
   and an hwpt_id (to associate the vIOMMU to a nesting parent HWPT_PAGING). The
   iommufd core will link the vIOMMU object to the struct iommu_device that the
   struct device is behind. And an IOMMU driver can implement a viommu_alloc op
   to allocate its own vIOMMU data structure embedding the core-level structure
   iommufd_viommu and some driver-specific data. If necessary, the driver can
   also configure its HW virtualization feature for that vIOMMU (and thus for
   the VM). Successful completion of this operation sets up the linkages between
   the vIOMMU object and the HWPT_PAGING, then this vIOMMU object can be used
   as a nesting parent object to allocate an HWPT_NESTED object described above.
   (IOMMUFD_OBJ_VIOMMU 只能通过 IOMMU_VIOMMU_ALLOC uAPI 手动创建，需提供一个 dev_id(用设备的物理 IOMMU 来支撑 vIOMMU)与一个 hwpt_id(把 vIOMMU 关联到嵌套父 HWPT_PAGING)。iommufd 核心会把 vIOMMU 对象链接到该 struct device 背后的 struct iommu_device。IOMMU 驱动可以实现 viommu_alloc op，分配自己的 vIOMMU 数据结构，其中内嵌核心级结构 iommufd_viommu 以及一些驱动特定数据。如有必要，驱动还可以为该 vIOMMU(进而为该 VM)配置其硬件虚拟化特性。这一操作的成功完成会建立 vIOMMU 对象与 HWPT_PAGING 之间的链接，随后该 vIOMMU 对象就可以作为嵌套父对象，用来分配上文所述的 HWPT_NESTED 对象。)

6. IOMMUFD_OBJ_VDEVICE can be only manually created via the IOMMU_VDEVICE_ALLOC
   uAPI, provided a viommu_id for an iommufd_viommu object and a dev_id for an
   iommufd_device object. The vDEVICE object will be the binding between these
   two parent objects. Another @virt_id will be also set via the uAPI providing
   the iommufd core an index to store the vDEVICE object to a vDEVICE array per
   vIOMMU. If necessary, the IOMMU driver may choose to implement a vdevce_alloc
   op to init its HW for virtualization feature related to a vDEVICE. Successful
   completion of this operation sets up the linkages between vIOMMU and device.
   (IOMMUFD_OBJ_VDEVICE 只能通过 IOMMU_VDEVICE_ALLOC uAPI 手动创建，需提供一个 iommufd_viommu 对象的 viommu_id 与一个 iommufd_device 对象的 dev_id。vDEVICE 对象将成为这两个父对象之间的绑定。另一个 @virt_id 也会经 uAPI 设置，为 iommufd 核心提供一个索引，把 vDEVICE 对象存入每个 vIOMMU 的 vDEVICE 数组。如有必要，IOMMU 驱动可以选择实现 vdevce_alloc op，为与 vDEVICE 相关的虚拟化特性初始化其硬件。这一操作的成功完成会建立 vIOMMU 与设备之间的链接。)

A device can only bind to an iommufd due to DMA ownership claim and attach to at
most one IOAS object (no support of PASID yet).
(由于 DMA 所有权声明，一个设备只能绑定到一个 iommufd，并且至多附加到一个 IOAS 对象(尚不支持 PASID)。)

Kernel Datastructure(内核数据结构)
----------------------------------

User visible objects are backed by following datastructures:
(用户可见对象由以下数据结构支撑：)

- iommufd_ioas for IOMMUFD_OBJ_IOAS.
- iommufd_device for IOMMUFD_OBJ_DEVICE.
- iommufd_hwpt_paging for IOMMUFD_OBJ_HWPT_PAGING.
- iommufd_hwpt_nested for IOMMUFD_OBJ_HWPT_NESTED.
- iommufd_fault for IOMMUFD_OBJ_FAULT.
- iommufd_viommu for IOMMUFD_OBJ_VIOMMU.
- iommufd_vdevice for IOMMUFD_OBJ_VDEVICE.
- iommufd_veventq for IOMMUFD_OBJ_VEVENTQ.
- iommufd_hw_queue for IOMMUFD_OBJ_HW_QUEUE.

(对应关系为：IOMMUFD_OBJ_IOAS 用 iommufd_ioas；IOMMUFD_OBJ_DEVICE 用 iommufd_device；IOMMUFD_OBJ_HWPT_PAGING 用 iommufd_hwpt_paging；IOMMUFD_OBJ_HWPT_NESTED 用 iommufd_hwpt_nested；IOMMUFD_OBJ_FAULT 用 iommufd_fault；IOMMUFD_OBJ_VIOMMU 用 iommufd_viommu；IOMMUFD_OBJ_VDEVICE 用 iommufd_vdevice；IOMMUFD_OBJ_VEVENTQ 用 iommufd_veventq；IOMMUFD_OBJ_HW_QUEUE 用 iommufd_hw_queue。)

Several terminologies when looking at these datastructures:
(查看这些数据结构时的几个术语：)

- Automatic domain - refers to an iommu domain created automatically when
  attaching a device to an IOAS object. This is compatible to the semantics of
  VFIO type1.
  (自动域 (Automatic domain) —— 指把设备附加到 IOAS 对象时自动创建的 iommu 域。这与 VFIO type1 的语义兼容。)

- Manual domain - refers to an iommu domain designated by the user as the
  target pagetable to be attached to by a device. Though currently there are
  no uAPIs to directly create such domain, the datastructure and algorithms
  are ready for handling that use case.
  (手动域 (Manual domain) —— 指由用户指定、作为设备要附加到的目标页表的 iommu 域。虽然目前还没有直接创建这类域的 uAPI，但数据结构与算法已经为处理该用例准备就绪。)

- In-kernel user - refers to something like a VFIO mdev that is using the
  IOMMUFD access interface to access the IOAS. This starts by creating an
  iommufd_access object that is similar to the domain binding a physical device
  would do. The access object will then allow converting IOVA ranges into struct
  page * lists, or doing direct read/write to an IOVA.
  (内核内使用者 (In-kernel user) —— 指像 VFIO mdev 那样使用 IOMMUFD access 接口访问 IOAS 的对象。首先创建一个 iommufd_access 对象，类似于物理设备进行域绑定。access 对象随后可以把 IOVA 范围转换成 struct page * 列表，或对 IOVA 直接读写。)

iommufd_ioas serves as the metadata datastructure to manage how IOVA ranges are
mapped to memory pages, composed of:
(iommufd_ioas 作为元数据数据结构，管理 IOVA 范围如何映射到内存页，由以下部分组成：)

- struct io_pagetable holding the IOVA map
- struct iopt_area's representing populated portions of IOVA
- struct iopt_pages representing the storage of PFNs
- struct iommu_domain representing the IO page table in the IOMMU
- struct iopt_pages_access representing in-kernel users of PFNs
- struct xarray pinned_pfns holding a list of pages pinned by in-kernel users

(即：持有 IOVA 映射的 struct io_pagetable；代表 IOVA 已填充部分的 struct iopt_area；代表 PFN 存储的 struct iopt_pages；代表 IOMMU 中 I/O 页表的 struct iommu_domain；代表 PFN 内核内使用者的 struct iopt_pages_access；以及持有内核内使用者所钉住页面列表的 struct xarray pinned_pfns。)

Each iopt_pages represents a logical linear array of full PFNs. The PFNs are
ultimately derived from userspace VAs via an mm_struct. Once they have been
pinned the PFNs are stored in IOPTEs of an iommu_domain or inside the pinned_pfns
xarray if they have been pinned through an iommufd_access.
(每个 iopt_pages 代表一个由完整 PFN 构成的逻辑线性数组。这些 PFN 最终经由 mm_struct 从用户空间 VA 得到。被钉住 (pinned) 后，PFN 保存在某个 iommu_domain 的 IOPTE 中；如果是通过 iommufd_access 钉住的，则保存在 pinned_pfns xarray 中。)

PFN have to be copied between all combinations of storage locations, depending
on what domains are present and what kinds of in-kernel "software access" users
exist. The mechanism ensures that a page is pinned only once.
(依据存在哪些域、以及存在哪些种类的内核内"软件访问"使用者，PFN 需要在所有存储位置组合之间复制。该机制确保一个页只被钉住一次。)

An io_pagetable is composed of iopt_areas pointing at iopt_pages, along with a
list of iommu_domains that mirror the IOVA to PFN map.
(一个 io_pagetable 由指向 iopt_pages 的 iopt_area，以及一份镜像 IOVA 到 PFN 映射的 iommu_domain 列表组成。)

Multiple io_pagetable-s, through their iopt_area-s, can share a single
iopt_pages which avoids multi-pinning and double accounting of page
consumption.
(多个 io_pagetable 可以经由各自的 iopt_area 共享同一个 iopt_pages，从而避免重复钉页以及对页消耗的重复记账。)

iommufd_ioas is shareable between subsystems, e.g. VFIO and VDPA, as long as
devices managed by different subsystems are bound to a same iommufd.
(iommufd_ioas 可在不同子系统之间共享，例如 VFIO 与 VDPA，只要不同子系统管理的设备绑定到同一个 iommufd 即可。)

IOMMUFD User API(IOMMUFD 用户 API)
==================================

.. kernel-doc:: include/uapi/linux/iommufd.h

IOMMUFD Kernel API(IOMMUFD 内核 API)
====================================

The IOMMUFD kAPI is device-centric with group-related tricks managed behind the
scene. This allows the external drivers calling such kAPI to implement a simple
device-centric uAPI for connecting its device to an iommufd, instead of
explicitly imposing the group semantics in its uAPI as VFIO does.
(IOMMUFD kAPI 以设备为中心，与组相关的技巧都在幕后处理。这使得调用该 kAPI 的外部驱动可以实现一个简单的、以设备为中心的 uAPI 来把设备连接到 iommufd，而不必像 VFIO 那样在 uAPI 中显式强加组语义。)

.. kernel-doc:: drivers/iommu/iommufd/device.c
   :export:

.. kernel-doc:: drivers/iommu/iommufd/main.c
   :export:

VFIO and IOMMUFD(VFIO 与 IOMMUFD)
---------------------------------

Connecting a VFIO device to iommufd can be done in two ways.
(把 VFIO 设备连接到 iommufd 可以通过两种方式完成。)

First is a VFIO compatible way by directly implementing the /dev/vfio/vfio
container IOCTLs by mapping them into io_pagetable operations. Doing so allows
the use of iommufd in legacy VFIO applications by symlinking /dev/vfio/vfio to
/dev/iommufd or extending VFIO to SET_CONTAINER using an iommufd instead of a
container fd.
(第一种是 VFIO 兼容方式：把 /dev/vfio/vfio 的 container IOCTL 直接实现为对 io_pagetable 操作的映射。这样，通过把 /dev/vfio/vfio 符号链接到 /dev/iommufd，或扩展 VFIO 使 SET_CONTAINER 使用 iommufd 而非 container fd，旧式 VFIO 应用即可使用 iommufd。)

The second approach directly extends VFIO to support a new set of device-centric
user API based on aforementioned IOMMUFD kernel API. It requires userspace
change but better matches the IOMMUFD API semantics and easier to support new
iommufd features when comparing it to the first approach.
(第二种方式直接扩展 VFIO，基于前述 IOMMUFD 内核 API 支持一套新的以设备为中心的用户 API。它需要用户空间改动，但与 IOMMUFD API 语义更匹配，且相比第一种方式更容易支持新的 iommufd 特性。)

Currently both approaches are still work-in-progress.
(目前两种方式都仍在开发中。)

There are still a few gaps to be resolved to catch up with VFIO type1, as
documented in iommufd_vfio_check_extension().
(要追平 VFIO type1 还有几处差距待解决，见 iommufd_vfio_check_extension() 中的记录。)

Future TODOs(未来 TODO)
=======================

Currently IOMMUFD supports only kernel-managed I/O page table, similar to VFIO
type1. New features on the radar include:
(目前 IOMMUFD 只支持内核管理的 I/O 页表，与 VFIO type1 类似。计划中的新特性包括：)

 - Binding iommu_domain's to PASID/SSID
 - Userspace page tables, for ARM, x86 and S390
 - Kernel bypass'd invalidation of user page tables
 - Re-use of the KVM page table in the IOMMU
 - Dirty page tracking in the IOMMU
 - Runtime Increase/Decrease of IOPTE size
 - PRI support with faults resolved in userspace

 (把 iommu_domain 绑定到 PASID/SSID)
 (面向 ARM、x86 与 S390 的用户空间页表)
 (绕过内核的用户页表失效)
 (在 IOMMU 中复用 KVM 页表)
 (IOMMU 中的脏页跟踪)
 (运行时增大/减小 IOPTE 大小)
 (在用户空间解决 fault 的 PRI 支持)
