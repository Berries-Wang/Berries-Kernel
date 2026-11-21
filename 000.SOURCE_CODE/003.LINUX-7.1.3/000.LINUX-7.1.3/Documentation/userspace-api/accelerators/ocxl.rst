====================================================================================
OpenCAPI (Open Coherent Accelerator Processor Interface)(开放一致性加速器处理器接口)
====================================================================================

OpenCAPI is an interface between processors and accelerators. It aims
at being low-latency and high-bandwidth.
(OpenCAPI 是处理器与加速器之间的接口。它的目标是低延迟、高带宽。)

The specification was developed by the OpenCAPI Consortium, and is now
available from the `Compute Express Link Consortium
<https://computeexpresslink.org/resource/opencapi-specification-archive/>`_.
(该规范由 OpenCAPI 联盟制定，现在可从 `Compute Express Link Consortium <https://computeexpresslink.org/resource/opencapi-specification-archive/>`_ 获取。)

It allows an accelerator (which could be an FPGA, ASICs, ...) to access
the host memory coherently, using virtual addresses. An OpenCAPI
device can also host its own memory, that can be accessed from the
host.
(它允许加速器(可以是 FPGA、ASIC 等)使用虚拟地址一致性地访问主机内存。OpenCAPI 设备也可以自带内存，供主机访问。)

OpenCAPI is known in linux as 'ocxl', as the open, processor-agnostic
evolution of 'cxl' (the driver for the IBM CAPI interface for
powerpc), which was named that way to avoid confusion with the ISDN
CAPI subsystem.
(在 Linux 中，OpenCAPI 以 'ocxl' 之名为人所知，它是 'cxl'(powerpc 上 IBM CAPI 接口的驱动)的开放、与处理器无关的演进版本；'cxl' 之所以叫这个名字，是为了避免与 ISDN CAPI 子系统混淆。)


High-level view(高层视图)
=========================

OpenCAPI defines a Data Link Layer (DL) and Transaction Layer (TL), to
be implemented on top of a physical link. Any processor or device
implementing the DL and TL can start sharing memory.
(OpenCAPI 定义了数据链路层 (DL) 与事务层 (TL)，需实现在物理链路之上。任何实现了 DL 与 TL 的处理器或设备都可以开始共享内存。)

::

  +-----------+                         +-------------+
  |           |                         |             |
  |           |                         | Accelerated |
  | Processor |                         |  Function   |
  |           |  +--------+             |    Unit     |  +--------+
  |           |--| Memory |             |    (AFU)    |--| Memory |
  |           |  +--------+             |             |  +--------+
  +-----------+                         +-------------+
       |                                       |
  +-----------+                         +-------------+
  |    TL     |                         |    TLX      |
  +-----------+                         +-------------+
       |                                       |
  +-----------+                         +-------------+
  |    DL     |                         |    DLX      |
  +-----------+                         +-------------+
       |                                       |
       |                   PHY                 |
       +---------------------------------------+



Device discovery(设备发现)
==========================

OpenCAPI relies on a PCI-like configuration space, implemented on the
device. So the host can discover AFUs by querying the config space.
(OpenCAPI 依赖一个类 PCI 的配置空间，它实现在设备上。因此主机可以通过查询配置空间来发现 AFU。)

OpenCAPI devices in Linux are treated like PCI devices (with a few
caveats). The firmware is expected to abstract the hardware as if it
was a PCI link. A lot of the existing PCI infrastructure is reused:
devices are scanned and BARs are assigned during the standard PCI
enumeration. Commands like 'lspci' can therefore be used to see what
devices are available.
(Linux 中的 OpenCAPI 设备被当作 PCI 设备对待(有少许例外)。固件应当把硬件抽象得好像是一条 PCI 链路。大量现有 PCI 基础设施被复用：设备在标准 PCI 枚举期间被扫描并分配 BAR。因此可以使用 'lspci' 之类的命令查看有哪些可用设备。)

The configuration space defines the AFU(s) that can be found on the
physical adapter, such as its name, how many memory contexts it can
work with, the size of its MMIO areas, ...
(配置空间定义了物理适配器上可以找到的 AFU，例如其名称、它可以同时处理多少个内存上下文、其 MMIO 区域的大小等。)



MMIO(MMIO 区域)
===============

OpenCAPI defines two MMIO areas for each AFU:
(OpenCAPI 为每个 AFU 定义两个 MMIO 区域：)

* the global MMIO area, with registers pertinent to the whole AFU.
  (* 全局 MMIO 区域，包含与整个 AFU 相关的寄存器。)
* a per-process MMIO area, which has a fixed size for each context.
  (* 每进程 MMIO 区域，每个上下文的该区域大小固定。)



AFU interrupts(AFU 中断)
========================

OpenCAPI includes the possibility for an AFU to send an interrupt to a
host process. It is done through a 'intrp_req' defined in the
Transaction Layer, specifying a 64-bit object handle which defines the
interrupt.
(OpenCAPI 允许 AFU 向主机进程发送中断。它通过事务层中定义的 'intrp_req' 实现，指定一个 64 位对象句柄来定义该中断。)

The driver allows a process to allocate an interrupt and obtain its
64-bit object handle, that can be passed to the AFU.
(驱动允许进程分配一个中断并获得其 64 位对象句柄，该句柄可以传给 AFU。)



char devices(字符设备)
======================

The driver creates one char device per AFU found on the physical
device. A physical device may have multiple functions and each
function can have multiple AFUs. At the time of this writing though,
it has only been tested with devices exporting only one AFU.
(驱动为物理设备上找到的每个 AFU 创建一个字符设备。一个物理设备可能有多个 function，每个 function 可以有多个 AFU。不过在撰写本文时，只在只导出一个 AFU 的设备上测试过。)

Char devices can be found in /dev/ocxl/ and are named as:
/dev/ocxl/<AFU name>.<location>.<index>
(字符设备位于 /dev/ocxl/ 下，命名格式为 /dev/ocxl/<AFU name>.<location>.<index>。)

where <AFU name> is a max 20-character long name, as found in the
config space of the AFU.
<location> is added by the driver and can help distinguish devices
when a system has more than one instance of the same OpenCAPI device.
<index> is also to help distinguish AFUs in the unlikely case where a
device carries multiple copies of the same AFU.
(其中 <AFU name> 是最长 20 个字符的名称，来自 AFU 的配置空间。<location> 由驱动添加，当系统中有同一 OpenCAPI 设备的多个实例时，有助于区分设备。<index> 也用于区分 AFU，以防出现设备带有同一 AFU 的多个副本这种罕见情况。)



Sysfs class(Sysfs 类)
=====================

An ocxl class is added for the devices representing the AFUs. See
/sys/class/ocxl. The layout is described in
Documentation/ABI/testing/sysfs-class-ocxl
(为表示 AFU 的设备添加了一个 ocxl 类，见 /sys/class/ocxl。其布局在 Documentation/ABI/testing/sysfs-class-ocxl 中描述。)



User API(用户 API)
==================

open(打开)
----------

Based on the AFU definition found in the config space, an AFU may
support working with more than one memory context, in which case the
associated char device may be opened multiple times by different
processes.
(根据配置空间中的 AFU 定义，一个 AFU 可能支持使用多个内存上下文工作；此时，关联的字符设备可以被不同进程多次打开。)


ioctl(ioctl 命令)
-----------------

OCXL_IOCTL_ATTACH:

  Attach the memory context of the calling process to the AFU so that
  the AFU can access its memory.
  (把调用进程的内存上下文附加到 AFU，使 AFU 可以访问它的内存。)

OCXL_IOCTL_IRQ_ALLOC:

  Allocate an AFU interrupt and return an identifier.
  (分配一个 AFU 中断并返回一个标识符。)

OCXL_IOCTL_IRQ_FREE:

  Free a previously allocated AFU interrupt.
  (释放先前分配的 AFU 中断。)

OCXL_IOCTL_IRQ_SET_FD:

  Associate an event fd to an AFU interrupt so that the user process
  can be notified when the AFU sends an interrupt.
  (把一个 event fd 关联到 AFU 中断，使用户进程能在 AFU 发送中断时得到通知。)

OCXL_IOCTL_GET_METADATA:

  Obtains configuration information from the card, such at the size of
  MMIO areas, the AFU version, and the PASID for the current context.
  (从卡上获取配置信息，例如 MMIO 区域的大小、AFU 版本以及当前上下文的 PASID。)

OCXL_IOCTL_ENABLE_P9_WAIT:

  Allows the AFU to wake a userspace thread executing 'wait'. Returns
  information to userspace to allow it to configure the AFU. Note that
  this is only available on POWER9.
  (允许 AFU 唤醒正在执行 'wait' 的用户空间线程。它返回一些信息给用户空间，用于配置 AFU。注意：该功能只在 POWER9 上可用。)

OCXL_IOCTL_GET_FEATURES:

  Reports on which CPU features that affect OpenCAPI are usable from
  userspace.
  (报告哪些影响 OpenCAPI 的 CPU 特性可从用户空间使用。)


mmap(mmap 映射)
---------------

A process can mmap the per-process MMIO area for interactions with the
AFU.
(进程可以 mmap 每进程 MMIO 区域，以便与 AFU 交互。)
