========================
 GPU Debugging(GPU 调试)
========================

General Debugging Options(常规调试选项)
=======================================

The DebugFS section provides documentation on a number files to aid in debugging
issues on the GPU.
(DebugFS 一节提供了多篇有助于调试 GPU 问题的文件文档。)


GPUVM Debugging(GPUVM 调试)
===========================

To aid in debugging GPU virtual memory related problems, the driver supports a
number of options module parameters:
(为帮助调试 GPU 虚拟内存相关的问题，驱动程序支持若干选项模块参数：)

`vm_fault_stop` - If non-0, halt the GPU memory controller on a GPU page fault.
(vm_fault_stop - 如果非 0，在发生 GPU 缺页中断时暂停 GPU 内存控制器。)

`vm_update_mode` - If non-0, use the CPU to update GPU page tables rather than
the GPU.
(vm_update_mode - 如果非 0，使用 CPU 而不是 GPU 来更新 GPU 页表。)


Decoding a GPUVM Page Fault(解码 GPUVM 缺页中断)
================================================

If you see a GPU page fault in the kernel log, you can decode it to figure
out what is going wrong in your application.  A page fault in your kernel
log may look something like this:

::

 [gfxhub0] no-retry page fault (src_id:0 ring:24 vmid:3 pasid:32777, for process glxinfo pid 2424 thread glxinfo:cs0 pid 2425)
   in page starting at address 0x0000800102800000 from IH client 0x1b (UTCL2)
 VM_L2_PROTECTION_FAULT_STATUS:0x00301030
 	Faulty UTCL2 client ID: TCP (0x8)
 	MORE_FAULTS: 0x0
 	WALKER_ERROR: 0x0
 	PERMISSION_FAULTS: 0x3
 	MAPPING_ERROR: 0x0
 	RW: 0x0

(如果你在内核日志中看到 GPU 缺页中断，可以对它进行解码，以弄清你的
应用程序中出了什么问题。内核日志中的缺页中断可能类似这样：)

First you have the memory hub, gfxhub and mmhub.  gfxhub is the memory
hub used for graphics, compute, and sdma on some chips.  mmhub is the
memory hub used for multi-media and sdma on some chips.
(首先是内存 hub：gfxhub 和 mmhub。gfxhub 是某些芯片上用于图形、计算
和 SDMA 的内存 hub。mmhub 是某些芯片上用于多媒体和 SDMA 的内存
hub。)

Next you have the vmid and pasid.  If the vmid is 0, this fault was likely
caused by the kernel driver or firmware.  If the vmid is non-0, it is generally
a fault in a user application.  The pasid is used to link a vmid to a system
process id.  If the process is active when the fault happens, the process
information will be printed.
(接下来是 vmid 和 pasid。如果 vmid 为 0，该缺页中断很可能是由内核
驱动或固件引起的。如果 vmid 非 0，它通常是用户应用中的缺页中断。
pasid 用于把 vmid 关联到系统进程 ID。如果缺页中断发生时该进程处于
活动状态，将会打印进程信息。)

The GPU virtual address that caused the fault comes next.
(接下来是引发缺页中断的 GPU 虚拟地址。)

The client ID indicates the GPU block that caused the fault.
Some common client IDs:
(客户端 ID 指示引发缺页中断的 GPU 模块。一些常见的客户端 ID：)

- CB/DB: The color/depth backend of the graphics pipe
  (CB/DB：图形管线的颜色/深度后端。)
- CPF: Command Processor Frontend
  (CPF：命令处理器前端。)
- CPC: Command Processor Compute
  (CPC：命令处理器计算。)
- CPG: Command Processor Graphics
  (CPG：命令处理器图形。)
- TCP/SQC/SQG: Shaders
  (TCP/SQC/SQG：着色器。)
- SDMA: SDMA engines
  (SDMA：SDMA 引擎。)
- VCN: Video encode/decode engines
  (VCN：视频编码/解码引擎。)
- JPEG: JPEG engines
  (JPEG：JPEG 引擎。)

PERMISSION_FAULTS describe what faults were encountered:
(PERMISSION_FAULTS 描述遇到了哪些错误：)

- bit 0: the PTE was not valid
  (bit 0：PTE 无效。)
- bit 1: the PTE read bit was not set
  (bit 1：PTE 读位未设置。)
- bit 2: the PTE write bit was not set
  (bit 2：PTE 写位未设置。)
- bit 3: the PTE execute bit was not set
  (bit 3：PTE 执行位未设置。)

Finally, RW, indicates whether the access was a read (0) or a write (1).
(最后，RW 指示该访问是读(0)还是写(1)。)

In the example above, a shader (cliend id = TCP) generated a read (RW = 0x0) to
an invalid page (PERMISSION_FAULTS = 0x3) at GPU virtual address
0x0000800102800000.  The user can then inspect their shader code and resource
descriptor state to determine what caused the GPU page fault.
(在上面的例子中，着色器(客户端 ID = TCP)对 GPU 虚拟地址
0x0000800102800000 处的一个无效页面(PERMISSION_FAULTS = 0x3)发起了
一次读访问(RW = 0x0)。用户随后可以检查自己的着色器代码和资源描述符
状态，以确定是什么引发了 GPU 缺页中断。)

UMR(UMR)
========

`umr <https://gitlab.freedesktop.org/tomstdenis/umr>`_ is a general purpose
GPU debugging and diagnostics tool.  Please see the umr
`documentation <https://umr.readthedocs.io/en/main/>`_ for more information
about its capabilities.
(umr 是一个通用的 GPU 调试与诊断工具。有关其功能的更多信息，请参阅
umr 文档。)

Debugging backlight brightness(调试背光亮度)
============================================
Default backlight brightness is intended to be set via the policy advertised
by the firmware.  Firmware will often provide different defaults for AC or DC.
Furthermore, some userspace software will save backlight brightness during
the previous boot and attempt to restore it.
(默认背光亮度应通过固件公布的策略来设置。固件通常会为交流(AC)或
直流(DC)供电提供不同的默认值。此外，一些用户态软件会保存上次启动
时的背光亮度并尝试恢复它。)

Some firmware also has support for a feature called "Custom Backlight Curves"
where an input value for brightness is mapped along a linearly interpolated
curve of brightness values that better match display characteristics.
(一些固件还支持名为 "Custom Backlight Curves"(自定义背光曲线)的
功能，其中亮度输入值会沿一条线性插值的亮度值曲线进行映射，以更好地
匹配显示器的特性。)

In the event of problems happening with backlight, there is a trace event
that can be enabled at bootup to log every brightness change request.
This can help isolate where the problem is. To enable the trace event add
the following to the kernel command line:
(如果背光出现问题，有一个可以在启动时启用的 trace 事件，用来记录
每次亮度变更请求。这有助于隔离问题所在。要启用该 trace 事件，请在
内核命令行中添加以下内容：)

  tp_printk trace_event=amdgpu_dm:amdgpu_dm_brightness:mod:amdgpu trace_buf_size=1M
