.. SPDX-License-Identifier: GPL-2.0

Confidential Computing VMs(机密计算虚拟机)
==========================================
Hyper-V can create and run Linux guests that are Confidential Computing
(CoCo) VMs. Such VMs cooperate with the physical processor to better protect
the confidentiality and integrity of data in the VM's memory, even in the
face of a hypervisor/VMM that has been compromised and may behave maliciously.
CoCo VMs on Hyper-V share the generic CoCo VM threat model and security
objectives described in Documentation/security/snp-tdx-threat-model.rst. Note
that Hyper-V specific code in Linux refers to CoCo VMs as "isolated VMs" or
"isolation VMs".
(Hyper-V 可以创建并运行作为机密计算 (Confidential Computing，CoCo) 虚拟机的 Linux 客户机。这类虚拟机与物理处理器协作，更好地保护 VM 内存中数据的机密性与完整性，即使面对已被攻破、可能实施恶意行为的 hypervisor/VMM 也能如此。Hyper-V 上的 CoCo VM 遵循 Documentation/security/snp-tdx-threat-model.rst 中描述的通用 CoCo VM 威胁模型与安全目标。注意，Linux 中 Hyper-V 特定代码把 CoCo VM 称为 "isolated VMs" 或 "isolation VMs"。)

A Linux CoCo VM on Hyper-V requires the cooperation and interaction of the
following:
(Hyper-V 上的 Linux CoCo VM 需要以下各方的协作与配合：)

* Physical hardware with a processor that supports CoCo VMs

  (带有支持 CoCo VM 的处理器的物理硬件)

* The hardware runs a version of Windows/Hyper-V with support for CoCo VMs

  (硬件上运行支持 CoCo VM 的 Windows/Hyper-V 版本)

* The VM runs a version of Linux that supports being a CoCo VM

  (VM 中运行支持作为 CoCo VM 的 Linux 版本)

The physical hardware requirements are as follows:
(物理硬件要求如下：)

* AMD processor with SEV-SNP. Hyper-V does not run guest VMs with AMD SME,
  SEV, or SEV-ES encryption, and such encryption is not sufficient for a CoCo
  VM on Hyper-V.

  (带 SEV-SNP 的 AMD 处理器。Hyper-V 不会使用 AMD SME、SEV 或 SEV-ES 加密运行客户机 VM，这类加密对 Hyper-V 上的 CoCo VM 而言并不充分。)

* Intel processor with TDX

  (带 TDX 的 Intel 处理器)

To create a CoCo VM, the "Isolated VM" attribute must be specified to Hyper-V
when the VM is created. A VM cannot be changed from a CoCo VM to a normal VM,
or vice versa, after it is created.
(要创建 CoCo VM，必须在创建 VM 时向 Hyper-V 指定 "Isolated VM" 属性。VM 创建后不能从 CoCo VM 改为普通 VM，反之亦然。)

Operational Modes(运行模式)
---------------------------
Hyper-V CoCo VMs can run in two modes. The mode is selected when the VM is
created and cannot be changed during the life of the VM.
(Hyper-V CoCo VM 可以在两种模式下运行。模式在 VM 创建时选定，且在 VM 的生命周期内不能更改。)

* Fully-enlightened mode. In this mode, the guest operating system is
  enlightened to understand and manage all aspects of running as a CoCo VM.

  (完全受启 (fully-enlightened) 模式。在此模式下，客户机操作系统受启以理解并管理作为 CoCo VM 运行的所有方面。)

* Paravisor mode. In this mode, a paravisor layer between the guest and the
  host provides some operations needed to run as a CoCo VM. The guest operating
  system can have fewer CoCo enlightenments than is required in the
  fully-enlightened case.

  (Paravisor 模式。在此模式下，客户机与宿主机之间的一层 paravisor 提供作为 CoCo VM 运行所需的部分操作。与完全受启情形相比，客户机操作系统可以少一些 CoCo 受启组件。)

Conceptually, fully-enlightened mode and paravisor mode may be treated as
points on a spectrum spanning the degree of guest enlightenment needed to run
as a CoCo VM. Fully-enlightened mode is one end of the spectrum. A full
implementation of paravisor mode is the other end of the spectrum, where all
aspects of running as a CoCo VM are handled by the paravisor, and a normal
guest OS with no knowledge of memory encryption or other aspects of CoCo VMs
can run successfully. However, the Hyper-V implementation of paravisor mode
does not go this far, and is somewhere in the middle of the spectrum. Some
aspects of CoCo VMs are handled by the Hyper-V paravisor while the guest OS
must be enlightened for other aspects. Unfortunately, there is no
standardized enumeration of feature/functions that might be provided in the
paravisor, and there is no standardized mechanism for a guest OS to query the
paravisor for the feature/functions it provides. The understanding of what
the paravisor provides is hard-coded in the guest OS.
(从概念上说，完全受启模式与 paravisor 模式可以视为一个连续谱上的点，该谱系度量作为 CoCo VM 运行所需的客户机受启程度。完全受启模式是谱系的一端。paravisor 模式的完整实现是谱系的另一端：作为 CoCo VM 运行的所有方面都由 paravisor 处理，对内存加密或 CoCo VM 其他方面一无所知的普通客户机操作系统也能成功运行。然而，Hyper-V 对 paravisor 模式的实现没有走到这一步，而是处于谱系的中间某处。CoCo VM 的某些方面由 Hyper-V paravisor 处理，而客户机操作系统必须为其他方面提供受启组件。遗憾的是，paravisor 可能提供的特性/功能没有标准化清单，客户机操作系统也没有标准化机制去查询 paravisor 提供的特性/功能。对 paravisor 提供哪些能力的认知是硬编码在客户机操作系统中的。)

Paravisor mode has similarities to the `Coconut project`_, which aims to provide
a limited paravisor to provide services to the guest such as a virtual TPM.
However, the Hyper-V paravisor generally handles more aspects of CoCo VMs
than is currently envisioned for Coconut, and so is further toward the "no
guest enlightenments required" end of the spectrum.
(Paravisor 模式与 Coconut 项目 (Coconut project) 有相似之处，后者旨在提供一个受限的 paravisor，为客户机提供诸如虚拟 TPM 之类的服务。不过，Hyper-V paravisor 通常处理的 CoCo VM 方面比 Coconut 目前的设想更多，因此在谱系上更靠近"无需客户机受启"的那一端。)

.. _Coconut project: https://github.com/coconut-svsm/svsm

In the CoCo VM threat model, the paravisor is in the guest security domain
and must be trusted by the guest OS. By implication, the hypervisor/VMM must
protect itself against a potentially malicious paravisor just like it
protects against a potentially malicious guest.
(在 CoCo VM 威胁模型中，paravisor 属于客户机安全域，必须被客户机操作系统信任。由此推论，hypervisor/VMM 必须像防范潜在恶意客户机一样防范潜在的恶意 paravisor。)

The hardware architectural approach to fully-enlightened vs. paravisor mode
varies depending on the underlying processor.
(完全受启模式与 paravisor 模式的硬件架构方案因底层处理器而异。)

* With AMD SEV-SNP processors, in fully-enlightened mode the guest OS runs in
  VMPL 0 and has full control of the guest context. In paravisor mode, the
  guest OS runs in VMPL 2 and the paravisor runs in VMPL 0. The paravisor
  running in VMPL 0 has privileges that the guest OS in VMPL 2 does not have.
  Certain operations require the guest to invoke the paravisor. Furthermore, in
  paravisor mode the guest OS operates in "virtual Top Of Memory" (vTOM) mode
  as defined by the SEV-SNP architecture. This mode simplifies guest management
  of memory encryption when a paravisor is used.

  (在带 SEV-SNP 的 AMD 处理器上，完全受启模式下客户机操作系统运行在 VMPL 0，对客户机上下文拥有完全控制权。paravisor 模式下，客户机操作系统运行在 VMPL 2，paravisor 运行在 VMPL 0。运行在 VMPL 0 的 paravisor 拥有 VMPL 2 的客户机操作系统所不具备的特权。某些操作需要客户机调用 paravisor。此外，在 paravisor 模式下，客户机操作系统按 SEV-SNP 架构定义的 "虚拟内存顶" (virtual Top Of Memory，vTOM) 模式运行。该模式在使用 paravisor 时简化了客户机对内存加密的管理。)

* With Intel TDX processor, in fully-enlightened mode the guest OS runs in an
  L1 VM. In paravisor mode, TD partitioning is used. The paravisor runs in the
  L1 VM, and the guest OS runs in a nested L2 VM.

  (在带 TDX 的 Intel 处理器上，完全受启模式下客户机操作系统运行在一个 L1 VM 中。paravisor 模式下使用 TD 分区 (TD partitioning)。paravisor 运行在 L1 VM 中，客户机操作系统运行在嵌套的 L2 VM 中。)

Hyper-V exposes a synthetic MSR to guests that describes the CoCo mode. This
MSR indicates if the underlying processor uses AMD SEV-SNP or Intel TDX, and
whether a paravisor is being used. It is straightforward to build a single
kernel image that can boot and run properly on either architecture, and in
either mode.
(Hyper-V 向客户机暴露一个描述 CoCo 模式的合成 MSR。该 MSR 指明底层处理器使用 AMD SEV-SNP 还是 Intel TDX，以及是否正在使用 paravisor。构建一个能在任一体系结构、任一模式下正常引导与运行的单一内核镜像非常直接。)

Paravisor Effects(Paravisor 的影响)
-----------------------------------
Running in paravisor mode affects the following areas of generic Linux kernel
CoCo VM functionality:
(以 paravisor 模式运行会影响通用 Linux 内核 CoCo VM 功能的以下几个方面：)

* Initial guest memory setup. When a new VM is created in paravisor mode, the
  paravisor runs first and sets up the guest physical memory as encrypted. The
  guest Linux does normal memory initialization, except for explicitly marking
  appropriate ranges as decrypted (shared). In paravisor mode, Linux does not
  perform the early boot memory setup steps that are particularly tricky with
  AMD SEV-SNP in fully-enlightened mode.

  (初始客户机内存设置。在 paravisor 模式下创建新 VM 时，paravisor 先行运行，并把客户机物理内存设置为已加密。客户机 Linux 执行正常的内存初始化，只是要把适当的范围显式标记为已解密 (shared)。paravisor 模式下，Linux 不执行那些在完全受启模式下使用 AMD SEV-SNP 时特别棘手的早期引导内存设置步骤。)

* #VC/#VE exception handling. In paravisor mode, Hyper-V configures the guest
  CoCo VM to route #VC and #VE exceptions to VMPL 0 and the L1 VM,
  respectively, and not the guest Linux. Consequently, these exception handlers
  do not run in the guest Linux and are not a required enlightenment for a
  Linux guest in paravisor mode.

  (#VC/#VE 异常处理。paravisor 模式下，Hyper-V 把客户机 CoCo VM 配置为把 #VC 与 #VE 异常分别路由到 VMPL 0 与 L1 VM，而不是客户机 Linux。因此，这些异常处理程序不在客户机 Linux 中运行，也就不是 paravisor 模式下 Linux 客户机所必需的受启组件。)

* CPUID flags. Both AMD SEV-SNP and Intel TDX provide a CPUID flag in the
  guest indicating that the VM is operating with the respective hardware
  support. While these CPUID flags are visible in fully-enlightened CoCo VMs,
  the paravisor filters out these flags and the guest Linux does not see them.
  Throughout the Linux kernel, explicitly testing these flags has mostly been
  eliminated in favor of the cc_platform_has() function, with the goal of
  abstracting the differences between SEV-SNP and TDX. But the
  cc_platform_has() abstraction also allows the Hyper-V paravisor configuration
  to selectively enable aspects of CoCo VM functionality even when the CPUID
  flags are not set. The exception is early boot memory setup on SEV-SNP, which
  tests the CPUID SEV-SNP flag. But not having the flag in Hyper-V paravisor
  mode VM achieves the desired effect or not running SEV-SNP specific early
  boot memory setup.

  (CPUID 标志。AMD SEV-SNP 与 Intel TDX 都会在客户机中提供一个 CPUID 标志，表明 VM 正在相应的硬件支持下运行。虽然这些 CPUID 标志在完全受启的 CoCo VM 中可见，但 paravisor 会把它们过滤掉，客户机 Linux 看不到它们。整个 Linux 内核中，显式测试这些标志的做法基本已被移除，改为使用 cc_platform_has() 函数，目标是抽象掉 SEV-SNP 与 TDX 之间的差异。而 cc_platform_has() 抽象也允许 Hyper-V paravisor 配置在 CPUID 标志未置位的情况下选择性地启用 CoCo VM 功能的各个方面。例外是 SEV-SNP 的早期引导内存设置，它会测试 CPUID 的 SEV-SNP 标志。但在 Hyper-V paravisor 模式 VM 中没有该标志，正好达到了不运行 SEV-SNP 特定早期引导内存设置的预期效果。)

* Device emulation. In paravisor mode, the Hyper-V paravisor provides
  emulation of devices such as the IO-APIC and TPM. Because the emulation
  happens in the paravisor in the guest context (instead of the hypervisor/VMM
  context), MMIO accesses to these devices must be encrypted references instead
  of the decrypted references that would be used in a fully-enlightened CoCo
  VM. The __ioremap_caller() function has been enhanced to make a callback to
  check whether a particular address range should be treated as encrypted
  (private). See the "is_private_mmio" callback.

  (设备仿真。paravisor 模式下，Hyper-V paravisor 提供 IO-APIC 与 TPM 等设备的仿真。由于仿真发生在客户机上下文中的 paravisor 内(而非 hypervisor/VMM 上下文)，对这些设备的 MMIO 访问必须使用加密引用，而不是完全受启 CoCo VM 中会使用的解密引用。__ioremap_caller() 函数已被增强，会发起回调以检查特定地址范围是否应被视为加密 (private)。参见 "is_private_mmio" 回调。)

* Encrypt/decrypt memory transitions. In a CoCo VM, transitioning guest
  memory between encrypted and decrypted requires coordinating with the
  hypervisor/VMM. This is done via callbacks invoked from
  __set_memory_enc_pgtable(). In fully-enlightened mode, the normal SEV-SNP and
  TDX implementations of these callbacks are used. In paravisor mode, a Hyper-V
  specific set of callbacks is used. These callbacks invoke the paravisor so
  that the paravisor can coordinate the transitions and inform the hypervisor
  as necessary. See hv_vtom_init() where these callback are set up.

  (加密/解密内存转换。在 CoCo VM 中，在加密与解密之间转换客户机内存需要与 hypervisor/VMM 协调。这是通过由 __set_memory_enc_pgtable() 调用的回调完成的。完全受启模式下使用 SEV-SNP 与 TDX 的常规回调实现；paravisor 模式下则使用一组 Hyper-V 特定的回调。这些回调会调用 paravisor，使其能够协调转换并在必要时通知 hypervisor。这些回调在 hv_vtom_init() 中建立，参见该函数。)

* Interrupt injection. In fully enlightened mode, a malicious hypervisor
  could inject interrupts into the guest OS at times that violate x86/x64
  architectural rules. For full protection, the guest OS should include
  enlightenments that use the interrupt injection management features provided
  by CoCo-capable processors. In paravisor mode, the paravisor mediates
  interrupt injection into the guest OS, and ensures that the guest OS only
  sees interrupts that are "legal". The paravisor uses the interrupt injection
  management features provided by the CoCo-capable physical processor, thereby
  masking these complexities from the guest OS.

  (中断注入。在完全受启模式下，恶意的 hypervisor 可能在不违反 x86/x64 架构规则的时刻之外向客户机操作系统注入中断。为获得完整保护，客户机操作系统应包含利用支持 CoCo 的处理器所提供的中断注入管理特性的受启组件。paravisor 模式下，paravisor 介入对客户机操作系统的中断注入，并确保客户机操作系统只看到"合法"的中断。paravisor 使用支持 CoCo 的物理处理器提供的中断注入管理特性，从而向客户机操作系统遮蔽了这些复杂性。)

Hyper-V Hypercalls(Hyper-V 超级调用)
------------------------------------
When in fully-enlightened mode, hypercalls made by the Linux guest are routed
directly to the hypervisor, just as in a non-CoCo VM. But in paravisor mode,
normal hypercalls trap to the paravisor first, which may in turn invoke the
hypervisor. But the paravisor is idiosyncratic in this regard, and a few
hypercalls made by the Linux guest must always be routed directly to the
hypervisor. These hypercall sites test for a paravisor being present, and use
a special invocation sequence. See hv_post_message(), for example.
(在完全受启模式下，Linux 客户机发起的超级调用 (hypercall) 会直接路由到 hypervisor，与非 CoCo VM 中一样。而在 paravisor 模式下，常规超级调用首先陷入 (trap) paravisor，后者又可能调用 hypervisor。但 paravisor 在这一点上有其特殊性：Linux 客户机发起的少数几个超级调用必须始终直接路由到 hypervisor。这些调用点会检测 paravisor 是否存在，并使用特殊的调用序列。例如参见 hv_post_message()。)

Guest communication with Hyper-V(与 Hyper-V 的客户机通信)
---------------------------------------------------------
Separate from the generic Linux kernel handling of memory encryption in Linux
CoCo VMs, Hyper-V has VMBus and VMBus devices that communicate using memory
shared between the Linux guest and the host. This shared memory must be
marked decrypted to enable communication. Furthermore, since the threat model
includes a compromised and potentially malicious host, the guest must guard
against leaking any unintended data to the host through this shared memory.
(与 Linux 内核对 Linux CoCo VM 内存加密的通用处理分开，Hyper-V 有 VMBus 与 VMBus 设备，它们使用 Linux 客户机与宿主机之间共享的内存进行通信。这些共享内存必须标记为解密才能通信。此外，由于威胁模型中包含已被攻破且可能恶意的宿主机，客户机必须防止通过这些共享内存把任何非预期数据泄露给宿主机。)

These Hyper-V and VMBus memory pages are marked as decrypted:
(以下 Hyper-V 与 VMBus 内存页被标记为解密：)

* VMBus monitor pages

  (VMBus 监视页)

* Synthetic interrupt controller (SynIC) related pages (unless supplied by
  the paravisor)

  (合成中断控制器 (SynIC) 相关页(除非由 paravisor 提供))

* Per-cpu hypercall input and output pages (unless running with a paravisor)

  (每 CPU 的超级调用输入/输出页(除非正与 paravisor 一起运行))

* VMBus ring buffers. The direct mapping is marked decrypted in
  __vmbus_establish_gpadl(). The secondary mapping created in
  hv_ringbuffer_init() must also include the "decrypted" attribute.

  (VMBus 环形缓冲区。直接映射在 __vmbus_establish_gpadl() 中被标记为解密。在 hv_ringbuffer_init() 中创建的次级映射也必须带上 "decrypted" 属性。)

When the guest writes data to memory that is shared with the host, it must
ensure that only the intended data is written. Padding or unused fields must
be initialized to zeros before copying into the shared memory so that random
kernel data is not inadvertently given to the host.
(当客户机向与宿主机共享的内存写入数据时，必须确保只写入预期的数据。填充字段或未用字段必须在复制进共享内存之前初始化为零，以免把随机的内核数据无意间交给宿主机。)

Similarly, when the guest reads memory that is shared with the host, it must
validate the data before acting on it so that a malicious host cannot induce
the guest to expose unintended data. Doing such validation can be tricky
because the host can modify the shared memory areas even while or after
validation is performed. For messages passed from the host to the guest in a
VMBus ring buffer, the length of the message is validated, and the message is
copied into a temporary (encrypted) buffer for further validation and
processing. The copying adds a small amount of overhead, but is the only way
to protect against a malicious host. See hv_pkt_iter_first().
(同样，当客户机读取与宿主机共享的内存时，必须在据此行动之前校验数据，以免恶意宿主机诱使客户机暴露非预期数据。这类校验可能很棘手，因为宿主机可以在校验执行期间或之后修改共享内存区域。对于经 VMBus 环形缓冲区从宿主机传给客户机的消息，会先校验消息长度，然后把消息复制到一个临时(加密)缓冲区中做进一步校验与处理。复制会带来少量开销，但这是防范恶意宿主机的唯一办法。参见 hv_pkt_iter_first()。)

Many drivers for VMBus devices have been "hardened" by adding code to fully
validate messages received over VMBus, instead of assuming that Hyper-V is
acting cooperatively. Such drivers are marked as "allowed_in_isolated" in the
vmbus_devs[] table. Other drivers for VMBus devices that are not needed in a
CoCo VM have not been hardened, and they are not allowed to load in a CoCo
VM. See vmbus_is_valid_offer() where such devices are excluded.
(许多 VMBus 设备驱动已经过"加固"，添加了代码来完全校验经由 VMBus 收到的消息，而不是假定 Hyper-V 会协作行事。这类驱动在 vmbus_devs[] 表中被标记为 "allowed_in_isolated"。CoCo VM 中不需要的其他 VMBus 设备驱动未经加固，不允许在 CoCo VM 中加载。这类设备在 vmbus_is_valid_offer() 中被排除，参见该函数。)

Two VMBus devices depend on the Hyper-V host to do DMA data transfers:
storvsc for disk I/O and netvsc for network I/O. storvsc uses the normal
Linux kernel DMA APIs, and so bounce buffering through decrypted swiotlb
memory is done implicitly. netvsc has two modes for data transfers. The first
mode goes through send and receive buffer space that is explicitly allocated
by the netvsc driver, and is used for most smaller packets. These send and
receive buffers are marked decrypted by __vmbus_establish_gpadl(). Because
the netvsc driver explicitly copies packets to/from these buffers, the
equivalent of bounce buffering between encrypted and decrypted memory is
already part of the data path. The second mode uses the normal Linux kernel
DMA APIs, and is bounce buffered through swiotlb memory implicitly like in
storvsc.
(两个 VMBus 设备依赖 Hyper-V 宿主机做 DMA 数据传输：磁盘 I/O 的 storvsc 与网络 I/O 的 netvsc。storvsc 使用正常的 Linux 内核 DMA API，因此经由解密的 swiotlb 内存做 bounce buffering 是隐式完成的。netvsc 有两种数据传输模式。第一种模式经由 netvsc 驱动显式分配的发送与接收缓冲区空间，用于大多数较小的数据包。这些发送与接收缓冲区由 __vmbus_establish_gpadl() 标记为解密。由于 netvsc 驱动显式地在这些缓冲区与数据包之间复制，加密内存与解密内存之间的 bounce buffering 等价物已经是数据路径的一部分。第二种模式使用正常的 Linux 内核 DMA API，像 storvsc 一样隐式地经由 swiotlb 内存做 bounce buffering。)

Finally, the VMBus virtual PCI driver needs special handling in a CoCo VM.
Linux PCI device drivers access PCI config space using standard APIs provided
by the Linux PCI subsystem. On Hyper-V, these functions directly access MMIO
space, and the access traps to Hyper-V for emulation. But in CoCo VMs, memory
encryption prevents Hyper-V from reading the guest instruction stream to
emulate the access. So in a CoCo VM, these functions must make a hypercall
with arguments explicitly describing the access. See
_hv_pcifront_read_config() and _hv_pcifront_write_config() and the
"use_calls" flag indicating to use hypercalls.
(最后，VMBus 虚拟 PCI 驱动在 CoCo VM 中需要特殊处理。Linux PCI 设备驱动使用 Linux PCI 子系统提供的标准 API 访问 PCI 配置空间。在 Hyper-V 上，这些函数直接访问 MMIO 空间，访问会陷入 Hyper-V 进行仿真。但在 CoCo VM 中，内存加密使 Hyper-V 无法读取客户机指令流来仿真该访问。因此在 CoCo VM 中，这些函数必须以显式描述访问的参数发起超级调用。参见 _hv_pcifront_read_config() 与 _hv_pcifront_write_config()，以及指示使用超级调用的 "use_calls" 标志。)

Confidential VMBus(机密 VMBus)
------------------------------
The confidential VMBus enables the confidential guest not to interact with
the untrusted host partition and the untrusted hypervisor. Instead, the guest
relies on the trusted paravisor to communicate with the devices processing
sensitive data. The hardware (SNP or TDX) encrypts the guest memory and the
register state while measuring the paravisor image using the platform security
processor to ensure trusted and confidential computing.
(机密 VMBus 使机密客户机无需与不受信任的宿主机分区及不受信任的 hypervisor 交互。取而代之，客户机依靠受信任的 paravisor 与处理敏感数据的设备通信。硬件(SNP 或 TDX)对客户机内存与寄存器状态加密，并使用平台安全处理器度量 paravisor 镜像，以确保可信与机密计算。)

Confidential VMBus provides a secure communication channel between the guest
and the paravisor, ensuring that sensitive data is protected from hypervisor-
level access through memory encryption and register state isolation.
(机密 VMBus 在客户机与 paravisor 之间提供一条安全通信通道，通过内存加密与寄存器状态隔离确保敏感数据免受 hypervisor 层级的访问。)

Confidential VMBus is an extension of Confidential Computing (CoCo) VMs
(a.k.a. "Isolated" VMs in Hyper-V terminology). Without Confidential VMBus,
guest VMBus device drivers (the "VSC"s in VMBus terminology) communicate
with VMBus servers (the VSPs) running on the Hyper-V host. The
communication must be through memory that has been decrypted so the
host can access it. With Confidential VMBus, one or more of the VSPs reside
in the trusted paravisor layer in the guest VM. Since the paravisor layer also
operates in encrypted memory, the memory used for communication with
such VSPs does not need to be decrypted and thereby exposed to the
Hyper-V host. The paravisor is responsible for communicating securely
with the Hyper-V host as necessary.
(机密 VMBus 是机密计算 (CoCo) VM 的扩展(在 Hyper-V 术语中也称 "Isolated" VM)。没有机密 VMBus 时，客户机 VMBus 设备驱动(VMBus 术语中的 "VSC")与运行在 Hyper-V 宿主机上的 VMBus 服务器 (VSP) 通信。通信必须经由已解密的内存进行，宿主机才能访问它。有了机密 VMBus，一个或多个 VSP 驻留在客户机 VM 中受信任的 paravisor 层。由于 paravisor 层同样运行在加密内存中，与这类 VSP 通信所用的内存无需解密，也就不会暴露给 Hyper-V 宿主机。paravisor 负责在必要时与 Hyper-V 宿主机安全通信。)

The data is transferred directly between the VM and a vPCI device (a.k.a.
a PCI pass-thru device, see :doc:`vpci`) that is directly assigned to VTL2
and that supports encrypted memory. In such a case, neither the host partition
nor the hypervisor has any access to the data. The guest needs to establish
a VMBus connection only with the paravisor for the channels that process
sensitive data, and the paravisor abstracts the details of communicating
with the specific devices away providing the guest with the well-established
VSP (Virtual Service Provider) interface that has had support in the Hyper-V
drivers for a decade.
(数据直接在 VM 与一个 vPCI 设备(也称 PCI 直通 (pass-thru) 设备，参见 :doc:`vpci` )之间传输；该设备直接分配给 VTL2 且支持加密内存。这种情况下，宿主机分区与 hypervisor 都无法访问数据。对于处理敏感数据的通道，客户机只需与 paravisor 建立 VMBus 连接，paravisor 把与具体设备通信的细节抽象掉，向客户机提供久经考验的 VSP (Virtual Service Provider) 接口——Hyper-V 驱动对它的支持已有十年之久。)

In the case the device does not support encrypted memory, the paravisor
provides bounce-buffering, and although the data is not encrypted, the backing
pages aren't mapped into the host partition through SLAT. While not impossible,
it becomes much more difficult for the host partition to exfiltrate the data
than it would be with a conventional VMBus connection where the host partition
has direct access to the memory used for communication.
(如果设备不支持加密内存，paravisor 会提供 bounce buffering；虽然数据未加密，但其后备页不会经由 SLAT 映射进宿主机分区。虽然并非不可能，但宿主机分区窃取数据的难度比常规 VMBus 连接(宿主机分区可直接访问通信所用内存)要大得多。)

Here is the data flow for a conventional VMBus connection (`C` stands for the
client or VSC, `S` for the server or VSP, the `DEVICE` is a physical one, might
be with multiple virtual functions)::

  +---- GUEST ----+       +----- DEVICE ----+        +----- HOST -----+
  |               |       |                 |        |                |
  |               |       |                 |        |                |
  |               |       |                 ==========                |
  |               |       |                 |        |                |
  |               |       |                 |        |                |
  |               |       |                 |        |                |
  +----- C -------+       +-----------------+        +------- S ------+
         ||                                                   ||
         ||                                                   ||
  +------||------------------ VMBus --------------------------||------+
  |                     Interrupts, MMIO                              |
  +-------------------------------------------------------------------+

(下面是常规 VMBus 连接的数据流(`C` 代表客户端即 VSC，`S` 代表服务器即 VSP，`DEVICE` 是物理设备，可能带有多个虚拟功能)：)

and the Confidential VMBus connection::

  +---- GUEST --------------- VTL0 ------+               +-- DEVICE --+
  |                                      |               |            |
  | +- PARAVISOR --------- VTL2 -----+   |               |            |
  | |     +-- VMBus Relay ------+    ====+================            |
  | |     |   Interrupts, MMIO  |    |   |               |            |
  | |     +-------- S ----------+    |   |               +------------+
  | |               ||               |   |
  | +---------+     ||               |   |
  | |  Linux  |     ||    OpenHCL    |   |
  | |  kernel |     ||               |   |
  | +---- C --+-----||---------------+   |
  |       ||        ||                   |
  +-------++------- C -------------------+               +------------+
          ||                                             |    HOST    |
          ||                                             +---- S -----+
  +-------||----------------- VMBus ---------------------------||-----+
  |                     Interrupts, MMIO                              |
  +-------------------------------------------------------------------+

(以及机密 VMBus 连接的数据流：)

An implementation of the VMBus relay that offers the Confidential VMBus
channels is available in the OpenVMM project as a part of the OpenHCL
paravisor. Please refer to

  * https://openvmm.dev/, and
  * https://github.com/microsoft/openvmm

for more information about the OpenHCL paravisor.
(提供机密 VMBus 通道的 VMBus relay 的一种实现可在 OpenVMM 项目中找到，它是 OpenHCL paravisor 的一部分。有关 OpenHCL paravisor 的更多信息请参阅：

  * https://openvmm.dev/，以及
  * https://github.com/microsoft/openvmm)

A guest that is running with a paravisor must determine at runtime if
Confidential VMBus is supported by the current paravisor. The x86_64-specific
approach relies on the CPUID Virtualization Stack leaf; the ARM64 implementation
is expected to support the Confidential VMBus unconditionally when running
ARM CCA guests.
(与 paravisor 一起运行的客户机必须在运行时判断当前 paravisor 是否支持机密 VMBus。x86_64 特定的做法依赖 CPUID 的 Virtualization Stack 叶；ARM64 实现预计在运行 ARM CCA 客户机时无条件支持机密 VMBus。)

Confidential VMBus is a characteristic of the VMBus connection as a whole,
and of each VMBus channel that is created. When a Confidential VMBus
connection is established, the paravisor provides the guest the message-passing
path that is used for VMBus device creation and deletion, and it provides a
per-CPU synthetic interrupt controller (SynIC) just like the SynIC that is
offered by the Hyper-V host. Each VMBus device that is offered to the guest
indicates the degree to which it participates in Confidential VMBus. The offer
indicates if the device uses encrypted ring buffers, and if the device uses
encrypted memory for DMA that is done outside the ring buffer. These settings
may be different for different devices using the same Confidential VMBus
connection.
(机密 VMBus 是整个 VMBus 连接以及所创建的每个 VMBus 通道的特性。当机密 VMBus 连接建立时，paravisor 向客户机提供用于 VMBus 设备创建与删除的消息传递路径，并提供每 CPU 的合成中断控制器 (SynIC)，就像 Hyper-V 宿主机提供的 SynIC 一样。每个提供给客户机的 VMBus 设备都会表明它参与机密 VMBus 的程度。offer 会指明设备是否使用加密的环形缓冲区，以及设备在环形缓冲区之外进行 DMA 时是否使用加密内存。使用同一机密 VMBus 连接的不同设备，这些设置可能不同。)

Although these settings are separate, in practice it'll always be encrypted
ring buffer only, or both encrypted ring buffer and external data. If a channel
is offered by the paravisor with confidential VMBus, the ring buffer can always
be encrypted since it's strictly for communication between the VTL2 paravisor
and the VTL0 guest. However, other memory regions are often used for e.g. DMA,
so they need to be accessible by the underlying hardware, and must be
unencrypted (unless the device supports encrypted memory). Currently, there are
not any VSPs in OpenHCL that support encrypted external memory, but future
versions are expected to enable this capability.
(虽然这些设置彼此独立，但实践中要么只加密环形缓冲区，要么环形缓冲区与外部数据都加密。如果 paravisor 通过机密 VMBus 提供某个通道，则环形缓冲区总是可以加密的，因为它严格用于 VTL2 paravisor 与 VTL0 客户机之间的通信。然而，其他内存区域常用于例如 DMA，需要能被底层硬件访问，因此必须不加密(除非设备支持加密内存)。目前 OpenHCL 中还没有支持加密外部内存的 VSP，但未来版本有望启用该能力。)

Because some devices on a Confidential VMBus may require decrypted ring buffers
and DMA transfers, the guest must interact with two SynICs -- the one provided
by the paravisor and the one provided by the Hyper-V host when Confidential
VMBus is not offered. Interrupts are always signaled by the paravisor SynIC,
but the guest must check for messages and for channel interrupts on both SynICs.
(由于机密 VMBus 上的某些设备可能需要解密的环形缓冲区与 DMA 传输，客户机必须与两个 SynIC 交互——一个由 paravisor 提供，另一个由 Hyper-V 宿主机在未提供机密 VMBus 时提供。中断始终由 paravisor SynIC 发出信号，但客户机必须在两个 SynIC 上都检查消息与通道中断。)

In the case of a confidential VMBus, regular SynIC access by the guest is
intercepted by the paravisor (this includes various MSRs such as the SIMP and
SIEFP, as well as hypercalls like HvPostMessage and HvSignalEvent). If the
guest actually wants to communicate with the hypervisor, it has to use special
mechanisms (GHCB page on SNP, or tdcall on TDX). Messages can be of either
kind: with confidential VMBus, messages use the paravisor SynIC, and if the
guest chose to communicate directly to the hypervisor, they use the hypervisor
SynIC. For interrupt signaling, some channels may be running on the host
(non-confidential, using the VMBus relay) and use the hypervisor SynIC, and
some on the paravisor and use its SynIC. The RelIDs are coordinated by the
OpenHCL VMBus server and are guaranteed to be unique regardless of whether
the channel originated on the host or the paravisor.
(在机密 VMBus 的情况下，客户机对 SynIC 的常规访问会被 paravisor 拦截(包括 SIMP 与 SIEFP 等各种 MSR，以及 HvPostMessage、HvSignalEvent 等超级调用)。如果客户机确实想与 hypervisor 通信，就必须使用特殊机制(SNP 上的 GHCB 页，或 TDX 上的 tdcall)。消息可以属于两种类型：使用机密 VMBus 时，消息经由 paravisor SynIC；如果客户机选择直接与 hypervisor 通信，则经由 hypervisor SynIC。就中断发信号而言，某些通道可能运行在宿主机上(非机密，使用 VMBus relay)并使用 hypervisor SynIC，而另一些运行在 paravisor 上并使用其 SynIC。RelID 由 OpenHCL VMBus 服务器统一协调，无论通道源于宿主机还是 paravisor，都保证唯一。)

load_unaligned_zeropad()(load_unaligned_zeropad() 补救机制)
-----------------------------------------------------------
When transitioning memory between encrypted and decrypted, the caller of
set_memory_encrypted() or set_memory_decrypted() is responsible for ensuring
the memory isn't in use and isn't referenced while the transition is in
progress. The transition has multiple steps, and includes interaction with
the Hyper-V host. The memory is in an inconsistent state until all steps are
complete. A reference while the state is inconsistent could result in an
exception that can't be cleanly fixed up.
(在加密与解密之间转换内存时，set_memory_encrypted() 或 set_memory_decrypted() 的调用方负责确保转换进行期间内存不在使用中、也不被引用。转换分多个步骤，并包含与 Hyper-V 宿主机的交互。在所有步骤完成之前，内存处于不一致状态。在状态不一致时引用它可能导致无法干净修复的异常。)

However, the kernel load_unaligned_zeropad() mechanism may make stray
references that can't be prevented by the caller of set_memory_encrypted() or
set_memory_decrypted(), so there's specific code in the #VC or #VE exception
handler to fixup this case. But a CoCo VM running on Hyper-V may be
configured to run with a paravisor, with the #VC or #VE exception routed to
the paravisor. There's no architectural way to forward the exceptions back to
the guest kernel, and in such a case, the load_unaligned_zeropad() fixup code
in the #VC/#VE handlers doesn't run.
(然而，内核的 load_unaligned_zeropad() 机制可能产生调用方无法阻止的杂散引用，因此 #VC 或 #VE 异常处理程序中有专门代码来修复这种情况。但运行在 Hyper-V 上的 CoCo VM 可能被配置为与 paravisor 一起运行，此时 #VC 或 #VE 异常被路由给 paravisor。架构上没有办法把这些异常转发回客户机内核；在这种情形下，#VC/#VE 处理程序中的 load_unaligned_zeropad() 修复代码不会运行。)

To avoid this problem, the Hyper-V specific functions for notifying the
hypervisor of the transition mark pages as "not present" while a transition
is in progress. If load_unaligned_zeropad() causes a stray reference, a
normal page fault is generated instead of #VC or #VE, and the page-fault-
based handlers for load_unaligned_zeropad() fixup the reference. When the
encrypted/decrypted transition is complete, the pages are marked as "present"
again. See hv_vtom_clear_present() and hv_vtom_set_host_visibility().
(为避免这一问题，Hyper-V 特定的、用于向 hypervisor 通知转换的函数会在转换进行期间把页标记为"不存在" (not present)。如果 load_unaligned_zeropad() 产生杂散引用，将生成普通缺页异常而非 #VC 或 #VE，基于缺页异常的 load_unaligned_zeropad() 处理程序会修复该引用。当加密/解密转换完成后，这些页会被重新标记为"存在" (present)。参见 hv_vtom_clear_present() 与 hv_vtom_set_host_visibility()。)
