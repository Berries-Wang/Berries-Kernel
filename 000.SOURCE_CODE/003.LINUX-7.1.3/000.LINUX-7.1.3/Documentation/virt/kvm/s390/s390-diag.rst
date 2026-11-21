.. SPDX-License-Identifier: GPL-2.0

==========================================================
The s390 DIAGNOSE call on KVM(KVM 上的 s390 DIAGNOSE 调用)
==========================================================

KVM on s390 supports the DIAGNOSE call for making hypercalls, both for
native hypercalls and for selected hypercalls found on other s390
hypervisors.
(s390 上的 KVM 支持 DIAGNOSE 调用用于发起超调用 (hypercall)，既包括原生
超调用，也包括其他 s390 管理程序上存在的选定超调用。)

Note that bits are numbered as by the usual s390 convention (most significant
bit on the left).
(注意，位按照通常的 s390 约定编号（最高有效位在左）。)


General remarks(总体说明)
-------------------------

DIAGNOSE calls by the guest cause a mandatory intercept. This implies
all supported DIAGNOSE calls need to be handled by either KVM or its
userspace.
(客户机发起的 DIAGNOSE 调用会导致强制截获 (intercept)。这意味着所有受
支持的 DIAGNOSE 调用都需要由 KVM 或其用户空间来处理。)

All DIAGNOSE calls supported by KVM use the RS-a format::

  --------------------------------------
  |  '83'  | R1 | R3 | B2 |     D2     |
  --------------------------------------
  0        8    12   16   20           31

(KVM 支持的所有 DIAGNOSE 调用都使用 RS-a 格式：)

The second-operand address (obtained by the base/displacement calculation)
is not used to address data. Instead, bits 48-63 of this address specify
the function code, and bits 0-47 are ignored.
(第二操作数地址（通过基址/位移计算得到）不用于寻址数据。相反，该地址的
第 48-63 位指定功能码，第 0-47 位被忽略。)

The supported DIAGNOSE function codes vary by the userspace used. For
DIAGNOSE function codes not specific to KVM, please refer to the
documentation for the s390 hypervisors defining them.
(受支持的 DIAGNOSE 功能码因所使用的用户空间而异。对于非 KVM 特定的
DIAGNOSE 功能码，请参阅定义它们的 s390 管理程序的文档。)


DIAGNOSE function code 'X'500' - KVM functions(DIAGNOSE 功能码 'X'500' - KVM 功能)
----------------------------------------------------------------------------------

If the function code specifies 0x500, various KVM-specific functions
are performed, including virtio functions.
(如果功能码指定 0x500，将执行各种 KVM 特定的功能，包括 virtio 功能。)

General register 1 contains the subfunction code. Supported subfunctions
depend on KVM's userspace. Regarding virtio subfunctions, generally
userspace provides either s390-virtio (subcodes 0-2) or virtio-ccw
(subcode 3).
(通用寄存器 1 包含子功能码。受支持的子功能取决于 KVM 的用户空间。对于
virtio 子功能，通常用户空间要么提供 s390-virtio（子码 0-2），要么提供
virtio-ccw（子码 3）。)

Upon completion of the DIAGNOSE instruction, general register 2 contains
the function's return code, which is either a return code or a subcode
specific value.
(DIAGNOSE 指令完成后，通用寄存器 2 包含该功能的返回码，它要么是一个
返回码，要么是子码特定的值。)

If the specified subfunction is not supported, a SPECIFICATION exception
will be triggered.
(如果指定的子功能不受支持，将触发 SPECIFICATION 异常。)

Subcode 0 - s390-virtio notification and early console printk
    Handled by userspace.
    (由用户空间处理。)

Subcode 1 - s390-virtio reset
    Handled by userspace.
    (由用户空间处理。)

Subcode 2 - s390-virtio set status
    Handled by userspace.
    (由用户空间处理。)

Subcode 3 - virtio-ccw notification
    Handled by either userspace or KVM (ioeventfd case).
    (由用户空间或 KVM 处理（ioeventfd 情况）。)

    General register 2 contains a subchannel-identification word denoting
    the subchannel of the virtio-ccw proxy device to be notified.
    (通用寄存器 2 包含一个子通道标识字 (subchannel-identification word)，
    表示要通知的 virtio-ccw 代理设备的子通道。)

    General register 3 contains the number of the virtqueue to be notified.
    (通用寄存器 3 包含要通知的 virtqueue 的编号。)

    General register 4 contains a 64bit identifier for KVM usage (the
    kvm_io_bus cookie). If general register 4 does not contain a valid
    identifier, it is ignored.
    (通用寄存器 4 包含一个供 KVM 使用的 64 位标识符（kvm_io_bus
    cookie）。如果通用寄存器 4 不包含有效的标识符，它将被忽略。)

    After completion of the DIAGNOSE call, general register 2 may contain
    a 64bit identifier (in the kvm_io_bus cookie case), or a negative
    error value, if an internal error occurred.
    (DIAGNOSE 调用完成后，通用寄存器 2 可能包含一个 64 位标识符
    （kvm_io_bus cookie 情况），或者在发生内部错误时包含一个负的错误
    值。)

    See also the virtio standard for a discussion of this hypercall.
    (关于此超调用的讨论，另请参见 virtio 标准。)

Subcode 4 - storage-limit
    Handled by userspace.
    (由用户空间处理。)

    After completion of the DIAGNOSE call, general register 2 will
    contain the storage limit: the maximum physical address that might be
    used for storage throughout the lifetime of the VM.
    (DIAGNOSE 调用完成后，通用寄存器 2 将包含存储限制：在整个 VM 生命
    周期内可用于存储的最大物理地址。)

    The storage limit does not indicate currently usable storage, it may
    include holes, standby storage and areas reserved for other means, such
    as memory hotplug or virtio-mem devices. Other interfaces for detecting
    actually usable storage, such as SCLP, must be used in conjunction with
    this subfunction.
    (存储限制并不表示当前可用的存储，它可能包含空洞、待机存储以及为其他
    目的保留的区域，例如内存热插拔或 virtio-mem 设备。必须将检测实际可用
    存储的其他接口（例如 SCLP）与此子功能结合使用。)

    Note that the storage limit can be larger, but never smaller than the
    maximum storage address indicated by SCLP via the "maximum storage
    increment" and the "increment size".
    (注意，存储限制可以大于、但绝不会小于 SCLP 通过 "maximum storage
    increment"（最大存储增量）和 "increment size"（增量大小）指示的最大
    存储地址。)


DIAGNOSE function code 'X'501 - KVM breakpoint(DIAGNOSE 功能码 'X'501 - KVM 断点)
---------------------------------------------------------------------------------

If the function code specifies 0x501, breakpoint functions may be performed.
This function code is handled by userspace.
(如果功能码指定 0x501，可以执行断点功能。此功能码由用户空间处理。)

This diagnose function code has no subfunctions and uses no parameters.
(此 diagnose 功能码没有子功能，也不使用参数。)


DIAGNOSE function code 'X'9C - Voluntary Time Slice Yield(DIAGNOSE 功能码 'X'9C - 自愿让出时间片)
-------------------------------------------------------------------------------------------------

General register 1 contains the target CPU address.
(通用寄存器 1 包含目标 CPU 地址。)

In a guest of a hypervisor like LPAR, KVM or z/VM using shared host CPUs,
DIAGNOSE with function code 0x9c may improve system performance by
yielding the host CPU on which the guest CPU is running to be assigned
to another guest CPU, preferably the logical CPU containing the specified
target CPU.
(在使用共享宿主 CPU 的 LPAR、KVM 或 z/VM 等管理程序的客户机中，功能码为
0x9c 的 DIAGNOSE 可以通过让出 (yield) 客户机 CPU 正在其上运行的宿主
CPU、使其被分配给另一个客户机 CPU（最好是包含指定目标 CPU 的逻辑
CPU），从而改善系统性能。)


DIAG 'X'9C forwarding(DIAG 'X'9C 转发)
++++++++++++++++++++++++++++++++++++++

The guest may send a DIAGNOSE 0x9c in order to yield to a certain
other vcpu. An example is a Linux guest that tries to yield to the vcpu
that is currently holding a spinlock, but not running.
(客户机可以发送 DIAGNOSE 0x9c 以让位给某个其他 vcpu。例如，Linux 客户机
试图让位给当前持有自旋锁但未在运行的 vcpu。)

However, on the host the real cpu backing the vcpu may itself not be
running.
Forwarding the DIAGNOSE 0x9c initially sent by the guest to yield to
the backing cpu will hopefully cause that cpu, and thus subsequently
the guest's vcpu, to be scheduled.
(然而，在宿主机上，支撑该 vcpu 的真实 CPU 本身可能也未在运行。把客户机
最初发送的、用于让位的 DIAGNOSE 0x9c 转发到支撑 CPU，有望使该 CPU（进而
使客户机的 vcpu）得到调度。)


diag9c_forwarding_hz
    KVM kernel parameter allowing to specify the maximum number of DIAGNOSE
    0x9c forwarding per second in the purpose of avoiding a DIAGNOSE 0x9c
    forwarding storm.
    A value of 0 turns the forwarding off.
    (KVM 内核参数，允许指定每秒 DIAGNOSE 0x9c 转发的最大次数，以避免
    DIAGNOSE 0x9c 转发风暴。值为 0 表示关闭转发。)
