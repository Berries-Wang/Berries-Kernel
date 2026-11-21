==================================================================================================================================
Using physical DMA provided by OHCI-1394 FireWire controllers for debugging(利用 OHCI-1394 FireWire 控制器提供的物理 DMA 进行调试)
==================================================================================================================================

Introduction(引言)
------------------

Basically all FireWire controllers which are in use today are compliant
to the OHCI-1394 specification which defines the controller to be a PCI
bus master which uses DMA to offload data transfers from the CPU and has
a "Physical Response Unit" which executes specific requests by employing
PCI-Bus master DMA after applying filters defined by the OHCI-1394 driver.
(如今使用的几乎所有 FireWire 控制器都符合 OHCI-1394 规范。该规范把控制器定义为 PCI 总线主设备，使用 DMA 为 CPU 分担数据传输，并带有一个"物理响应单元"(Physical Response Unit),在应用 OHCI-1394 驱动定义的过滤器之后，通过 PCI 总线主设备 DMA 执行特定请求。)

Once properly configured, remote machines can send these requests to
ask the OHCI-1394 controller to perform read and write requests on
physical system memory and, for read requests, send the result of
the physical memory read back to the requester.
(配置妥当后，远程机器可以发送这些请求，让 OHCI-1394 控制器对物理系统内存执行读写请求；对于读请求，还会把物理内存读取的结果回传给请求方。)

With that, it is possible to debug issues by reading interesting memory
locations such as buffers like the printk buffer or the process table.
(这样，就可以通过读取感兴趣的内存位置来调试问题，例如 printk 缓冲区或进程表等。)

Retrieving a full system memory dump is also possible over the FireWire,
using data transfer rates in the order of 10MB/s or more.
(也可以通过 FireWire 获取完整的系统内存转储，数据传输率可达 10MB/s 或更高。)

With most FireWire controllers, memory access is limited to the low 4 GB
of physical address space.  This can be a problem on machines where memory is
located mostly above that limit, but it is rarely a problem on more common
hardware such as x86, x86-64 and PowerPC.
(对大多数 FireWire 控制器而言，内存访问仅限于物理地址空间的低 4 GB。在内存大多位于该界限之上的机器上这会成为问题，但在 x86、x86-64 和 PowerPC 等更常见的硬件上这很少成为问题。)

At least LSI FW643e and FW643e2 controllers are known to support access to
physical addresses above 4 GB, but this feature is currently not enabled by
Linux.
(已知至少 LSI FW643e 与 FW643e2 控制器支持访问 4 GB 以上的物理地址，但 Linux 目前并未启用该特性。)

Together with a early initialization of the OHCI-1394 controller for debugging,
this facility proved most useful for examining long debugs logs in the printk
buffer on to debug early boot problems in areas like ACPI where the system
fails to boot and other means for debugging (serial port) are either not
available (notebooks) or too slow for extensive debug information (like ACPI).
(配合 OHCI-1394 控制器用于调试的早期初始化，这一设施在以下场景中证明最为有用：查看 printk 缓冲区中的长调试日志，或调试 ACPI 等领域的早期启动问题——此时系统无法启动，而其他调试手段(串口)要么不可用(笔记本)，要么对大量调试信息(如 ACPI)而言太慢。)

Drivers(驱动)
-------------

The firewire-ohci driver in drivers/firewire uses filtered physical
DMA by default, which is more secure but not suitable for remote debugging.
Pass the remote_dma=1 parameter to the driver to get unfiltered physical DMA.
(drivers/firewire 中的 firewire-ohci 驱动默认使用经过过滤的物理 DMA,这更安全但不适合远程调试。向驱动传入 remote_dma=1 参数即可获得未过滤的物理 DMA。)

Because the firewire-ohci driver depends on the PCI enumeration to be
completed, an initialization routine which runs pretty early has been
implemented for x86.  This routine runs long before console_init() can be
called, i.e. before the printk buffer appears on the console.
(由于 firewire-ohci 驱动依赖 PCI 枚举完成，x86 上专门实现了一个运行得非常早的初始化例程。该例程在 console_init() 可以被调用之前很久就会运行，即早于 printk 缓冲区出现在控制台之前。)

To activate it, enable CONFIG_PROVIDE_OHCI1394_DMA_INIT (Kernel hacking menu:
Remote debugging over FireWire early on boot) and pass the parameter
"ohci1394_dma=early" to the recompiled kernel on boot.
(要启用它，需打开 CONFIG_PROVIDE_OHCI1394_DMA_INIT(Kernel hacking 菜单：Remote debugging over FireWire early on boot)，并在启动时向重编译的内核传入参数 "ohci1394_dma=early"。)

Tools(工具)
-----------

firescope - Originally developed by Benjamin Herrenschmidt, Andi Kleen ported
it from PowerPC to x86 and x86_64 and added functionality, firescope can now
be used to view the printk buffer of a remote machine, even with live update.
(firescope - 最初由 Benjamin Herrenschmidt 开发，Andi Kleen 将其从 PowerPC 移植到 x86 与 x86_64 并增加了功能；现在可以用 firescope 查看远程机器的 printk 缓冲区，甚至支持实时刷新。)

Bernhard Kaindl enhanced firescope to support accessing 64-bit machines
from 32-bit firescope and vice versa:
(Bernhard Kaindl 增强了 firescope，使 32 位的 firescope 能够访问 64 位机器，反之亦然：)
- http://v3.sk/~lkundrak/firescope/

and he implemented fast system dump (alpha version - read README.txt):
(他还实现了快速系统转储(alpha 版本 - 请阅读 README.txt)：)
- http://halobates.de/firewire/firedump-0.1.tar.bz2

There is also a gdb proxy for firewire which allows to use gdb to access
data which can be referenced from symbols found by gdb in vmlinux:
(还有一个面向 firewire 的 gdb 代理，允许用 gdb 访问那些可由 gdb 在 vmlinux 中找到的符号所引用的数据：)
- http://halobates.de/firewire/fireproxy-0.33.tar.bz2

The latest version of this gdb proxy (fireproxy-0.34) can communicate (not
yet stable) with kgdb over an memory-based communication module (kgdbom).
(此 gdb 代理的最新版本(fireproxy-0.34)可以通过基于内存的通信模块(kgdbom)与 kgdb 通信(尚不稳定)。)

Getting Started(入门)
---------------------

The OHCI-1394 specification regulates that the OHCI-1394 controller must
disable all physical DMA on each bus reset.
(OHCI-1394 规范规定，每次总线复位时 OHCI-1394 控制器必须禁用所有物理 DMA。)

This means that if you want to debug an issue in a system state where
interrupts are disabled and where no polling of the OHCI-1394 controller
for bus resets takes place, you have to establish any FireWire cable
connections and fully initialize all FireWire hardware __before__ the
system enters such state.
(这意味着，如果你想在"中断已禁用且不会轮询 OHCI-1394 控制器总线复位"的系统状态下调试问题，就必须先建立好全部 FireWire 线缆连接、完整初始化好所有 FireWire 硬件，然后系统才能进入这种状态。)

Step-by-step instructions for using firescope with early OHCI initialization:
(配合早期 OHCI 初始化使用 firescope 的分步说明：)

1) Verify that your hardware is supported:
   (1. 确认你的硬件受支持：)

   Load the firewire-ohci module and check your kernel logs.
   You should see a line similar to::

     firewire_ohci 0000:15:00.1: added OHCI v1.0 device as card 2, 4 IR + 4 IT
     ... contexts, quirks 0x11

   (加载 firewire-ohci 模块并检查内核日志。你应该会看到类似如下的一行：)

   when loading the driver. If you have no supported controller, many PCI,
   CardBus and even some Express cards which are fully compliant to OHCI-1394
   specification are available. If it requires no driver for Windows operating
   systems, it most likely is. Only specialized shops have cards which are not
   compliant, they are based on TI PCILynx chips and require drivers for Windows
   operating systems.
   (该行在加载驱动时输出。如果你没有受支持的控制器，市场上有许多完全符合 OHCI-1394 规范的 PCI、CardBus 甚至 Express 卡可用。如果它无需 Windows 操作系统驱动，那它多半符合规范。只有专业商店才有不符合规范的卡，它们基于 TI PCILynx 芯片，需要 Windows 操作系统驱动。)

   The mentioned kernel log message contains the string "physUB" if the
   controller implements a writable Physical Upper Bound register.  This is
   required for physical DMA above 4 GB (but not utilized by Linux yet).
   (如果控制器实现了可写的 Physical Upper Bound 寄存器，上述内核日志消息会包含字符串 "physUB"。超过 4 GB 的物理 DMA 需要该寄存器(但 Linux 尚未利用)。)

2) Establish a working FireWire cable connection:
   (2. 建立可用的 FireWire 线缆连接：)

   Any FireWire cable, as long at it provides electrically and mechanically
   stable connection and has matching connectors (there are small 4-pin and
   large 6-pin FireWire ports) will do.
   (任何 FireWire 线缆都可以，只要它能提供电气与机械上稳定的连接，并且两端接口匹配(有小型 4 针与大型 6 针两种 FireWire 端口)。)

   If an driver is running on both machines you should see a line like::

     firewire_core 0000:15:00.1: created device fw1: GUID 00061b0020105917, S400

   (如果两台机器上都有驱动在运行，你应该会看到类似如下的一行：)

   on both machines in the kernel log when the cable is plugged in
   and connects the two machines.
   (插上线缆把两台机器连接起来之后，两台机器的内核日志中都会出现该行。)

3) Test physical DMA using firescope:
   (3. 用 firescope 测试物理 DMA：)

   On the debug host, make sure that /dev/fw* is accessible,
   then start firescope::

	$ firescope
	Port 0 (/dev/fw1) opened, 2 nodes detected

	FireScope
	---------
	Target : <unspecified>
	Gen    : 1
	[Ctrl-T] choose target
	[Ctrl-H] this menu
	[Ctrl-Q] quit

    ------> Press Ctrl-T now, the output should be similar to:

	2 nodes available, local node is: 0
	 0: ffc0, uuid: 00000000 00000000 [LOCAL]
	 1: ffc1, uuid: 00279000 ba4bb801

   (在调试主机上，确保 /dev/fw* 可访问，然后启动 firescope：)

   Besides the [LOCAL] node, it must show another node without error message.
   (除 [LOCAL] 节点外，它必须无错误地显示另一个节点。)

4) Prepare for debugging with early OHCI-1394 initialization:
   (4. 为使用早期 OHCI-1394 初始化进行调试做准备：)

   4.1) Kernel compilation and installation on debug target
   (4.1. 在调试目标机上编译并安装内核)

   Compile the kernel to be debugged with CONFIG_PROVIDE_OHCI1394_DMA_INIT
   (Kernel hacking: Provide code for enabling DMA over FireWire early on boot)
   enabled and install it on the machine to be debugged (debug target).
   (编译要调试的内核，启用 CONFIG_PROVIDE_OHCI1394_DMA_INIT(Kernel hacking: Provide code for enabling DMA over FireWire early on boot)，并将其安装到被调试的机器(调试目标机)上。)

   4.2) Transfer the System.map of the debugged kernel to the debug host
   (4.2. 把被调试内核的 System.map 传送到调试主机)

   Copy the System.map of the kernel be debugged to the debug host (the host
   which is connected to the debugged machine over the FireWire cable).
   (把被调试内核的 System.map 复制到调试主机(即通过 FireWire 线缆与被调试机器相连的那台主机)。)

5) Retrieving the printk buffer contents:
   (5. 获取 printk 缓冲区内容：)

   With the FireWire cable connected, the OHCI-1394 driver on the debugging
   host loaded, reboot the debugged machine, booting the kernel which has
   CONFIG_PROVIDE_OHCI1394_DMA_INIT enabled, with the option ohci1394_dma=early.
   (接好 FireWire 线缆并在调试主机上加载 OHCI-1394 驱动之后，重启被调试的机器，启动启用了 CONFIG_PROVIDE_OHCI1394_DMA_INIT 的内核，并带上 ohci1394_dma=early 选项。)

   Then, on the debugging host, run firescope, for example by using -A::

	firescope -A System.map-of-debug-target-kernel

   (然后在调试主机上运行 firescope，例如使用 -A：)

   Note: -A automatically attaches to the first non-local node. It only works
   reliably if only connected two machines are connected using FireWire.
   (注意：-A 会自动连接到第一个非本地节点。只有当恰好只有两台机器通过 FireWire 相连时，它才能可靠工作。)

   After having attached to the debug target, press Ctrl-D to view the
   complete printk buffer or Ctrl-U to enter auto update mode and get an
   updated live view of recent kernel messages logged on the debug target.
   (连接到调试目标机之后，按 Ctrl-D 查看完整的 printk 缓冲区，或按 Ctrl-U 进入自动更新模式，实时查看调试目标机上记录的最新内核消息。)

   Call "firescope -h" to get more information on firescope's options.
   (调用 "firescope -h" 可了解 firescope 各选项的更多信息。)

Notes(附注)
-----------

Documentation and specifications: http://halobates.de/firewire/

FireWire is a trademark of Apple Inc. - for more information please refer to:
(FireWire 是 Apple Inc. 的商标 - 更多信息请参阅：)
https://en.wikipedia.org/wiki/FireWire
