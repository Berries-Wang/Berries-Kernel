=======================
VGA Arbiter(VGA 仲裁器)
=======================

Graphic devices are accessed through ranges in I/O or memory space. While most
modern devices allow relocation of such ranges, some "Legacy" VGA devices
implemented on PCI will typically have the same "hard-decoded" addresses as
they did on ISA. For more details see "PCI Bus Binding to IEEE Std 1275-1994
Standard for Boot (Initialization Configuration) Firmware Revision 2.1"
Section 7, Legacy Devices.
(图形设备通过 I/O 或内存空间中的地址范围访问。虽然大多数现代设备允许重定位这些范围，但 PCI 上实现的一些"传统" (Legacy) VGA 设备通常仍使用与 ISA 时代相同的"硬编码"地址。更多细节请参阅 "PCI Bus Binding to IEEE Std 1275-1994 Standard for Boot (Initialization Configuration) Firmware Revision 2.1" 第 7 节 Legacy Devices。)

The Resource Access Control (RAC) module inside the X server [0] existed for
the legacy VGA arbitration task (besides other bus management tasks) when more
than one legacy device co-exist on the same machine. But the problem happens
when these devices are trying to be accessed by different userspace clients
(e.g. two servers in parallel). Their address assignments conflict. Moreover,
ideally, being a userspace application, it is not the role of the X server to
control bus resources. Therefore an arbitration scheme outside of the X server
is needed to control the sharing of these resources. This document introduces
the operation of the VGA arbiter implemented for the Linux kernel.
(X server 内部的资源访问控制 (RAC) 模块 [0] 曾用于在多个传统设备共存于同一台机器时执行传统 VGA 仲裁任务(以及总线管理的其他任务)。但当这些设备被不同的用户态客户端访问时(例如两个 server 并行运行)，问题就出现了：它们的地址分配相互冲突。此外，理想情况下，作为用户态应用的 X server 本不应负责控制总线资源。因此，需要一个位于 X server 之外的仲裁方案来控制这些资源的共享。本文档介绍为 Linux 内核实现的 VGA 仲裁器的运作方式。)

vgaarb kernel/userspace ABI(vgaarb 内核/用户态 ABI)
---------------------------------------------------

The vgaarb is a module of the Linux Kernel. When it is initially loaded, it
scans all PCI devices and adds the VGA ones inside the arbitration. The
arbiter then enables/disables the decoding on different devices of the VGA
legacy instructions. Devices which do not want/need to use the arbiter may
explicitly tell it by calling vga_set_legacy_decoding().
(vgaarb 是 Linux 内核的一个模块。初始加载时，它会扫描所有 PCI 设备并把 VGA 设备纳入仲裁。随后，仲裁器在不同设备上启用/禁用 VGA 传统指令的解码。不想或不需要使用仲裁器的设备可以通过调用 vga_set_legacy_decoding() 明确告知它。)

The kernel exports a char device interface (/dev/vga_arbiter) to the clients,
which has the following semantics:
(内核向客户端导出一个字符设备接口 (/dev/vga_arbiter)，其语义如下：)

open
        Opens a user instance of the arbiter. By default, it's attached to the
        default VGA device of the system.
        (打开仲裁器的一个用户实例。默认情况下，它附着在系统的默认 VGA 设备上。)

close
        Close a user instance. Release locks made by the user
        (关闭一个用户实例。释放该用户持有的锁)

read
        Return a string indicating the status of the target like:
        (返回一个指示目标状态的字符串，形如：)

        "<card_ID>,decodes=<io_state>,owns=<io_state>,locks=<io_state> (ic,mc)"

        An IO state string is of the form {io,mem,io+mem,none}, mc and
        ic are respectively mem and io lock counts (for debugging/
        diagnostic only). "decodes" indicate what the card currently
        decodes, "owns" indicates what is currently enabled on it, and
        "locks" indicates what is locked by this card. If the card is
        unplugged, we get "invalid" then for card_ID and an -ENODEV
        error is returned for any command until a new card is targeted.
        (IO 状态字符串的形式为 {io,mem,io+mem,none}；mc 与 ic 分别是 mem 与 io 的锁计数(仅用于调试/诊断)。"decodes" 表示该卡当前解码什么，"owns" 表示其上当前启用了什么，"locks" 表示被该卡锁定的资源。如果该卡被拔出，card_ID 将变为 "invalid"，并且在重新指定新卡之前，任何命令都返回 -ENODEV 错误。)


write
        Write a command to the arbiter. List of commands:
        (向仲裁器写入一条命令。命令列表：)

        target <card_ID>
                switch target to card <card_ID> (see below)
                (把目标切换到卡 <card_ID>(见下文))
        lock <io_state>
                acquires locks on target ("none" is an invalid io_state)
                (在目标上获取锁("none" 是非法的 io_state))
        trylock <io_state>
                non-blocking acquire locks on target (returns EBUSY if
                unsuccessful)
                (以非阻塞方式在目标上获取锁(失败时返回 EBUSY))
        unlock <io_state>
                release locks on target
                (释放在目标上的锁)
        unlock all
                release all locks on target held by this user (not implemented
                yet)
                (释放该用户在目标上持有的全部锁(尚未实现))
        decodes <io_state>
                set the legacy decoding attributes for the card
                (设置该卡的传统解码属性)

        poll
                event if something changes on any card (not just the target)
                (任何卡(而不仅是目标)发生变化时的事件)

        card_ID is of the form "PCI:domain:bus:dev.fn". It can be set to "default"
        to go back to the system default card (TODO: not implemented yet). Currently,
        only PCI is supported as a prefix, but the userland API may support other bus
        types in the future, even if the current kernel implementation doesn't.
        (card_ID 的形式为 "PCI:domain:bus:dev.fn"。它可以设为 "default" 以回到系统默认卡(TODO：尚未实现)。目前仅支持 PCI 作为前缀，但用户态 API 将来可能支持其他总线类型，即使当前内核实现并不支持。)

Note about locks:
(关于锁的说明：)

The driver keeps track of which user has which locks on which card. It
supports stacking, like the kernel one. This complexifies the implementation
a bit, but makes the arbiter more tolerant to user space problems and able
to properly cleanup in all cases when a process dies.
Currently, a max of 16 cards can have locks simultaneously issued from
user space for a given user (file descriptor instance) of the arbiter.
(驱动会记录哪个用户在哪个卡上持有哪些锁。它支持嵌套(栈式)持有，与内核中的做法类似。这使实现稍复杂一些，但让仲裁器对用户空间的问题更加宽容，并能在进程死亡的所有情况下正确清理。目前，对仲裁器的给定用户(文件描述符实例)而言，最多同时有 16 张卡可以从用户空间持有锁。)

In the case of devices hot-{un,}plugged, there is a hook - pci_notify() - to
notify them being added/removed in the system and automatically added/removed
in the arbiter.
(对于设备热插拔/热拔出的情况，有一个钩子 pci_notify()，用于获知设备在系统中被添加/移除，并自动将其加入/移出仲裁器。)

There is also an in-kernel API of the arbiter in case DRM, vgacon, or other
drivers want to use it.
(此外还有仲裁器的内核态 API，供 DRM、vgacon 或其他驱动使用。)

In-kernel interface(内核态接口)
-------------------------------

.. kernel-doc:: include/linux/vgaarb.h
   :internal:

.. kernel-doc:: drivers/pci/vgaarb.c
   :export:

libpciaccess
------------

To use the vga arbiter char device, an API was implemented inside the
libpciaccess library. One field was added to struct pci_device (each device
on the system)::

    /* the type of resource decoded by the device */
    int vgaarb_rsrc;

(为了使用 vga 仲裁器字符设备，在 libpciaccess 库内实现了一套 API。为 struct pci_device(系统上的每个设备)增加了一个字段：)

Besides it, in pci_system were added::

    int vgaarb_fd;
    int vga_count;
    struct pci_device *vga_target;
    struct pci_device *vga_default_dev;

(此外，还在 pci_system 中增加了：)

The vga_count is used to track how many cards are being arbitrated, so for
instance, if there is only one card, then it can completely escape arbitration.
(vga_count 用于跟踪有多少张卡正在被仲裁；例如，如果只有一张卡，那么它完全可以脱离仲裁。)

These functions below acquire VGA resources for the given card and mark those
resources as locked. If the resources requested are "normal" (and not legacy)
resources, the arbiter will first check whether the card is doing legacy
decoding for that type of resource. If yes, the lock is "converted" into a
legacy resource lock. The arbiter will first look for all VGA cards that
might conflict and disable their IOs and/or Memory access, including VGA
forwarding on P2P bridges if necessary, so that the requested resources can
be used. Then, the card is marked as locking these resources and the IO and/or
Memory access is enabled on the card (including VGA forwarding on parent
P2P bridges if any). In the case of vga_arb_lock(), the function will block
if some conflicting card is already locking one of the required resources (or
any resource on a different bus segment, since P2P bridges don't differentiate
VGA memory and IO afaik). If the card already owns the resources, the function
succeeds.  vga_arb_trylock() will return (-EBUSY) instead of blocking. Nested
calls are supported (a per-resource counter is maintained).
(下面这些函数为给定的卡获取 VGA 资源，并将这些资源标记为已锁定。如果请求的资源是"普通" (normal) 资源(而非传统 (legacy) 资源)，仲裁器会先检查该卡是否正在对该类型的资源做传统解码。如果是，该锁会被"转换"为传统资源锁。仲裁器会先找出所有可能冲突的 VGA 卡，禁用它们的 IO 和/或内存访问(必要时包括 P2P 桥上的 VGA 转发)，以便请求的资源可以使用。然后，该卡被标记为锁定这些资源，其 IO 和/或内存访问被启用(如有父级 P2P 桥，还包括其上的 VGA 转发)。对于 vga_arb_lock()，若某个冲突的卡已经锁定了所需资源之一(或不同总线段上的任何资源，因为 P2P 桥据我所知并不区分 VGA 内存与 IO)，该函数会阻塞。如果该卡已拥有这些资源，函数直接成功。vga_arb_trylock() 会返回 (-EBUSY) 而不是阻塞。支持嵌套调用(为每个资源维护一个计数器)。)

Set the target device of this client. ::

    int  pci_device_vgaarb_set_target   (struct pci_device *dev);

(设置此客户端的目标设备：)

For instance, in x86 if two devices on the same bus want to lock different
resources, both will succeed (lock). If devices are in different buses and
trying to lock different resources, only the first who tried succeeds. ::

    int  pci_device_vgaarb_lock         (void);
    int  pci_device_vgaarb_trylock      (void);

(例如，在 x86 上，如果同一总线上的两个设备想锁定不同的资源，两者都会成功 (lock)。如果设备位于不同总线且试图锁定不同的资源，则只有先尝试者成功：)

Unlock resources of device. ::

    int  pci_device_vgaarb_unlock       (void);

(解锁设备的资源：)

Indicates to the arbiter if the card decodes legacy VGA IOs, legacy VGA
Memory, both, or none. All cards default to both, the card driver (fbdev for
example) should tell the arbiter if it has disabled legacy decoding, so the
card can be left out of the arbitration process (and can be safe to take
interrupts at any time. ::

    int  pci_device_vgaarb_decodes      (int new_vgaarb_rsrc);

(告知仲裁器该卡解码传统 VGA IO、传统 VGA 内存、两者皆有，还是皆无。所有卡默认为两者皆有；卡的驱动(例如 fbdev)如果已禁用传统解码，应告知仲裁器，这样该卡就可以不参与仲裁过程(并且可以随时安全地接受中断：)

Connects to the arbiter device, allocates the struct ::

    int  pci_device_vgaarb_init         (void);

(连接到仲裁器设备并分配结构体：)

Close the connection ::

    void pci_device_vgaarb_fini         (void);

(关闭连接：)

xf86VGAArbiter (X server implementation)(xf86VGAArbiter(X server 实现))
-----------------------------------------------------------------------

X server basically wraps all the functions that touch VGA registers somehow.
(X server 基本上把所有以某种方式触及 VGA 寄存器的函数都包装了一遍。)

References(参考资料)
--------------------

Benjamin Herrenschmidt (IBM?) started this work when he discussed such design
with the Xorg community in 2005 [1, 2]. In the end of 2007, Paulo Zanoni and
Tiago Vignatti (both of C3SL/Federal University of Paraná) proceeded his work
enhancing the kernel code to adapt as a kernel module and also did the
implementation of the user space side [3]. Now (2009) Tiago Vignatti and Dave
Airlie finally put this work in shape and queued to Jesse Barnes' PCI tree.
(Benjamin Herrenschmidt (IBM?) 在 2005 年与 Xorg 社区讨论这一设计时开始了这项工作 [1, 2]。2007 年底，Paulo Zanoni 与 Tiago Vignatti(均来自 C3SL/巴拉那联邦大学)继续推进他的工作，对内核代码进行改造使其成为一个内核模块，并完成了用户态侧的实现 [3]。如今 (2009)，Tiago Vignatti 与 Dave Airlie 最终把这项工作整理成形，并排入 Jesse Barnes 的 PCI 树。)

0) https://cgit.freedesktop.org/xorg/xserver/commit/?id=4b42448a2388d40f257774fbffdccaea87bd0347
1) https://lists.freedesktop.org/archives/xorg/2005-March/006663.html
2) https://lists.freedesktop.org/archives/xorg/2005-March/006745.html
3) https://lists.freedesktop.org/archives/xorg/2007-October/029507.html
