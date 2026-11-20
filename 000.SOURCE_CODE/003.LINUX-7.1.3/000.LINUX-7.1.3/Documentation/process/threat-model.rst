The Linux Kernel threat model(Linux 内核威胁模型)
==================================================

There are a lot of assumptions regarding what the kernel does and does not
protect against. These assumptions tend to cause confusion for bug reports
(:doc:`security-related ones <security-bugs>` vs :doc:`non-security ones
<../admin-guide/reporting-issues>`), and can complicate security enforcement
when the responsibilities for some boundaries is not clear between the kernel,
distros, administrators and users.
(关于内核防范什么、不防范什么，存在大量假设。这些假设容易给缺陷报告造成困惑 (:doc:`安全相关的 <security-bugs>` 与 :doc:`非安全相关的
<../admin-guide/reporting-issues>`)，并且当某些边界的职责在内核、发行版、管理员与用户之间不清晰时，会使安全策略的执行变得复杂。)

This document tries to clarify the responsibilities of the kernel in this
domain.
(本文档试图厘清内核在这一领域的职责。)

The kernel's responsibilities(内核的职责)
------------------------------------------

The kernel abstracts access to local hardware resources and to remote systems
in a way that allows multiple local users to get a fair share of the available
resources granted to them, and, when the underlying hardware permits, to assign
a level of confidentiality to their communications and to the data they are
processing or storing.
(内核对本地硬件资源与远程系统的访问进行抽象，使多个本地用户能够公平地分享分配给他们的可用资源，并且在底层硬件允许的情况下，能够为其通信以及正在处理或存储的数据赋予一定的保密性。)

The kernel assumes that the underlying hardware behaves according to its
specifications. This includes the integrity of the CPU's instruction set, the
transparency of the branch prediction unit and the cache units, the consistency
of the Memory Management Unit (MMU), the isolation of DMA-capable peripherals
(e.g., via IOMMU), state transitions in controllers, ranges of values read from
registers, the respect of documented hardware limitations, etc.
(内核假定底层硬件按其规范运作。这包括 CPU 指令集的完整性、分支预测单元与缓存单元的透明性、内存管理单元 (MMU) 的一致性、具备 DMA 能力的外设的隔离 (例如通过 IOMMU)、控制器中的状态转换、从寄存器读到的值的范围、对已文档化硬件限制的遵守等等。)

When hardware fails to maintain its specified isolation (e.g., CPU bugs,
side-channels, hardware response to unexpected inputs), the kernel will usually
attempt to implement reasonable mitigations. These are best-effort measures
intended to reduce the attack surface or elevate the cost of an attack within
the limits of the hardware's facilities; they do not constitute a
kernel-provided safety guarantee.
(当硬件未能维持其规定的隔离性时 (例如 CPU 缺陷、侧信道、硬件对意外输入的响应)，内核通常会尝试实现合理的缓解措施。这些是尽力而为的措施，旨在在硬件设施允许的范围内减小攻击面或提高攻击代价；它们并不构成内核提供的安全保证。)

Users always perform their activities under the authority of an administrator
who is able to grant or deny various types of permissions that may affect how
users benefit from available resources, or the level of confidentiality of
their activities. Administrators may also delegate all or part of their own
permissions to some users, particularly via capabilities but not only. All this
is performed via configuration (sysctl, file-system permissions etc).
(用户始终在某个管理员的授权之下开展其活动；管理员能够授予或拒绝各种类型的权限，这些权限可能影响用户如何使用可用资源，或其活动的保密级别。管理员还可以把自己权限的全部或一部分委托给某些用户，尤其是通过 capability (能力)，但不限于此。所有这些都通过配置完成 (sysctl、文件系统权限等)。)

The Linux Kernel applies a certain collection of default settings that match
its threat model. Distros have their own threat model and will come with their
own configuration presets, that the administrator may have to adjust to better
suit their expectations (relax or restrict).
(Linux 内核应用一组与其威胁模型相匹配的默认设置。发行版有自己的威胁模型，并自带配置预设，管理员可能需要对其加以调整，以更好地符合其期望 (放宽或收紧)。)

By default, the Linux Kernel guarantees the following protections when running
on common processors featuring privilege levels and memory management units:
(默认情况下，在配备特权级别与内存管理单元的常见处理器上运行时，Linux 内核保证以下保护：)

* **User-based isolation**: an unprivileged user may restrict access to their
  own data from other unprivileged users running on the same system. This
  includes:
  (**基于用户的隔离**：非特权用户可以限制同一系统上其他非特权用户对其自身数据的访问。这包括：)

  * stored data, via file system permissions
    (存储的数据，通过文件系统权限)
  * in-memory data (pages are not accessible by default to other users)
    (内存中的数据 (其他用户默认无法访问这些页))
  * process activity (ptrace is not permitted to other users)
    (进程活动 (ptrace 对其他用户是不允许的))
  * inter-process communication (other users may not observe data exchanged via
    UNIX domain sockets or other IPC mechanisms).
    (进程间通信 (其他用户不得观察通过 UNIX 域套接字或其他 IPC 机制交换的数据)。)
  * network communications within the same or with other systems
    (同一系统内或与其他系统之间的网络通信)

* **Capability-based protection**:
  (**基于 capability 的保护**：)

  * users not having elevated capabilities (including but not limited to
    CAP_SYS_ADMIN) may not alter the
    kernel's configuration, memory nor state, change other users' view of the
    file system layout, grant any user capabilities they do not have, nor
    affect the system's availability (shutdown, reboot, panic, hang, or making
    the system unresponsive via unbounded resource exhaustion).
    (不具备提升能力的用户 (包括但不限于 CAP_SYS_ADMIN) 不得更改内核的配置、内存或状态，不得更改其他用户眼中文件系统布局，不得授予任何用户其本不具备的能力，也不得影响系统的可用性 (关机、重启、panic、挂起，或通过无界的资源消耗使系统失去响应)。)
  * users not having the ``CAP_NET_ADMIN`` capability may not alter the network
    configuration, intercept nor spoof network communications from other users
    nor systems.
    (不具备 ``CAP_NET_ADMIN`` 能力的用户不得更改网络配置，不得截获或伪造其他用户或系统的网络通信。)
  * users not having ``CAP_SYS_PTRACE`` may not observe other users' processes
    activities.
    (不具备 ``CAP_SYS_PTRACE`` 的用户不得观察其他用户的进程活动。)

When ``CONFIG_USER_NS`` is set, the kernel also permits unprivileged users to
create their own user namespace in which they have all capabilities, but with a
number of restrictions (they may not perform actions that have impacts on the
initial user namespace, such as changing time, loading modules or mounting
block devices). Please refer to ``user_namespaces(7)`` for more details, the
possibilities of user namespaces are not covered in this document.
(设置了 ``CONFIG_USER_NS`` 时，内核还允许非特权用户创建自己的用户命名空间，在其中他们拥有全部能力，但有若干限制 (他们不得执行对初始用户命名空间有影响的操作，例如更改时间、加载模块或挂载块设备)。更多细节请参见 ``user_namespaces(7)`` ，本文档不涵盖用户命名空间的各种可能性。)

The kernel also offers a lot of troubleshooting and debugging facilities, which
can constitute attack vectors when placed in wrong hands. While some of them
are designed to be accessible to regular local users with a low risk (e.g.
kernel logs via ``/proc/kmsg``), some would expose enough information to
represent a risk in most places and the decision to expose them is under the
administrator's responsibility (perf events, traces), and others are not
designed to be accessed by non-privileged users (e.g. debugfs). Access to these
facilities by a user who has been explicitly granted permission by an
administrator does not constitute a security breach.
(内核还提供大量故障排查与调试设施，一旦落入坏人之手便可能构成攻击向量。其中一些被设计为可让普通本地用户以低风险访问 (例如通过 ``/proc/kmsg`` 的内核日志)；一些会暴露足以在多数环境中构成风险的信息，是否暴露由管理员负责决定 (perf 事件、跟踪)；还有一些并非为非特权用户访问而设计 (例如 debugfs)。被管理员明确授予权限的用户访问这些设施不构成安全破坏。)

Bugs that permit to violate the principles above constitute security breaches.
However, bugs that permit one violation only once another one was already
achieved are only weaknesses. The kernel applies a number of self-protection
measures whose purpose is to avoid crossing a security boundary when certain
classes of bugs are found, but a failure of these extra protections do not
constitute a vulnerability alone.
(允许违反上述原则的缺陷构成安全破坏。而只有在另一项破坏已经达成之后才能实现某项违反的缺陷只是弱点。内核应用了若干自我保护措施，其目的是在发现某些类别的缺陷时避免跨越安全边界，但这些额外保护的失效本身并不构成漏洞。)

What does not constitute a security bug(什么不构成安全缺陷)
------------------------------------------------------------

In the Linux kernel's threat model, the following classes of problems are
**NOT** considered as Linux Kernel security bugs. However, when it is believed
that the kernel could do better, they should be reported, so that they can be
reviewed and fixed where reasonably possible, but they will be handled as any
regular bug:
(在 Linux 内核的威胁模型中，以下几类问题 **不** 被视为 Linux 内核安全缺陷。不过，当认为内核可以做得更好时，仍应报告它们，以便在合理可行之处加以审查与修复，但它们将按普通缺陷处理：)

* **Configuration**:
  (**配置**：)

  * outdated kernels and particularly end-of-life branches are out of the scope
    of the kernel's threat model: administrators are responsible for keeping
    their system up to date. For a bug to qualify as a security bug, it must be
    demonstrated that it affects actively maintained versions.
    (过时的内核，尤其是已终止维护 (end-of-life) 的分支，不在内核威胁模型的范围之内：管理员负责保持其系统更新。要使一个缺陷被认定为安全缺陷，必须证明它影响仍在积极维护的版本。)

  * build-level: changes to the kernel configuration that are explicitly
    documented as lowering the security level (e.g. ``CONFIG_NOMMU``), or
    targeted at developers only.
    (构建层面：文档中明确说明会降低安全级别的内核配置更改 (例如 ``CONFIG_NOMMU``)，或仅面向开发者的配置。)

  * OS-level: changes to command line parameters, sysctls, filesystem
    permissions, user capabilities, exposure of privileged interfaces, that
    explicitly increase exposure by either offering non-default access to
    unprivileged users, or reduce the kernel's ability to enforce some
    protections or mitigations. Example: write access to procfs or debugfs.
    (操作系统层面：对命令行参数、sysctl、文件系统权限、用户能力、特权接口暴露的更改，它们通过向非特权用户提供非默认访问而明显增大暴露面，或削弱内核实施某些保护或缓解措施的能力。例如：对 procfs 或 debugfs 的写访问。)

  * issues triggered only when using features intended for development or
    debugging (e.g., LOCKDEP, KASAN, FAULT_INJECTION): these features are known
    to introduce overhead and potential instability and are not intended for
    production use.
    (仅在使用面向开发或调试的特性时才触发的问题 (例如 LOCKDEP、KASAN、FAULT_INJECTION)：已知这些特性会引入开销与潜在的不稳定性，并非为生产使用而设计。)

  * issues affecting drivers exposed under CONFIG_STAGING, as well as features
    marked EXPERIMENTAL in the configuration.
    (影响 CONFIG_STAGING 下所暴露驱动的问题，以及配置中标记为 EXPERIMENTAL 的特性。)

  * loading of explicitly insecure/broken/staging modules, and generally any
    using any subsystem marked as experimental or not intended for production
    use.
    (加载明确不安全/已损坏/staging 的模块，以及一般而言使用任何标记为实验性或不面向生产使用的子系统。)

  * running out-of-tree modules or unofficial kernel forks; these should be
    reported to the relevant vendor.
    (运行树外 (out-of-tree) 模块或非官方的内核分支；这些应报告给相关厂商。)

* **Excess of initial privileges**:
  (**初始权限过剩**：)

  * actions performed by a user already possessing the privileges required to
    perform that action or modify that state (e.g. ``CAP_SYS_ADMIN``,
    ``CAP_NET_ADMIN``, ``CAP_SYS_RAWIO``, ``CAP_SYS_MODULE`` with no further
    boundary being crossed).
    (用户已具备执行该操作或修改该状态所需权限而执行的操作 (例如 ``CAP_SYS_ADMIN``、
    ``CAP_NET_ADMIN``、``CAP_SYS_RAWIO``、``CAP_SYS_MODULE``，且未跨越更多边界)。)

  * actions performed in user namespace that do not bypass the restrictions
    imposed to the initial user (e.g. ptrace usage, signal delivery, resource
    usage, access to FS/device/sysctl/memory, network binding, system/network
    configuration etc).
    (在用户命名空间中执行、且未绕过对初始用户所施加限制的操作 (例如 ptrace 使用、信号投递、资源使用、对文件系统/设备/sysctl/内存的访问、网络绑定、系统/网络配置等)。)

  * anything performed by the root user in the initial namespace (e.g. kernel
    oops when writing to a privileged device).
    (root 用户在初始命名空间中执行的任何操作 (例如向特权设备写入时内核 oops)。)

* **Out of production use**:
  (**超出生产使用**：)

  This covers theoretical/probabilistic attacks that rely on laboratory
  conditions with zero system noise, or those requiring an unrealistic number
  of attempts (e.g., billions of trials) that would be detected by standard
  system monitoring long before success, such as:
  (这涵盖依赖零系统噪声实验室条件的理论性/概率性攻击，或需要不切实际的尝试次数 (例如数十亿次试验) 以至于早在成功之前就会被标准系统监测发现的攻击，例如：)

  * prediction of random numbers that only works in a totally silent
    environment (such as IP ID, TCP ports or sequence numbers that can only be
    guessed in a lab).
    (只在完全安静环境中才有效的随机数预测 (例如只有在实验室里才能猜中的 IP ID、TCP 端口或序列号)。)

  * activity observation and information leaks based on probabilistic
    approaches that are prone to measurement noise and not realistically
    reproducible on a production system.
    (基于概率方法的活动观察与信息泄露，这些方法易受测量噪声影响，在生产系统上无法真实重现。)

  * issues that can only be triggered by heavy attacks (e.g. brute force) whose
    impact on the system makes it unlikely or impossible to remain undetected
    before they succeed (e.g. consuming all memory before succeeding).
    (只能由重度攻击 (例如暴力破解) 触发的问题，其对系统的影响使得在攻击成功之前几乎不可能保持不被发现 (例如在成功之前耗尽全部内存)。)

  * problems seen only under development simulators, emulators, or combinations
    that do not exist on real systems at the time of reporting (issues
    involving tens of millions of threads, tens of thousands of CPUs,
    unrealistic CPU frequencies, RAM sizes or disk capacities, network speeds.
    (只在开发用模拟器、仿真器或报告时真实系统上并不存在的组合中才出现的问题 (涉及数千万线程、数万 CPU、不切实际的 CPU 频率、内存大小或磁盘容量、网络速度的问题)。)

  * issues whose reproduction requires hardware modification or emulation,
    including fake USB devices that pretend to be another one.
    (复现需要修改或仿真硬件的问题，包括伪装成另一设备的假 USB 设备。)

  * as well as issues that can be triggered at a cost that is orders of
    magnitude higher than the expected benefits (e.g. fully functional keyboard
    emulator only to retrieve 7 uninitialized bytes in a structure, or
    brute-force method involving millions of connection attempts to guess a
    port number).
    (以及触发代价远高于预期收益数个量级的问题 (例如为了取回结构体中 7 个未初始化字节而动用全功能键盘仿真器，或用涉及数百万次连接尝试的暴力方法猜端口号)。)

* **Hardening failures**:
  (**加固失效**：)

  * ability to bypass some of the kernel's hardening measures with no
    demonstrable exploit path (e.g. ASLR bypass, events timing or probing with
    no demonstrable consequence). These are just weaknesses, not
    vulnerabilities.
    (在没有可证实的利用路径的情况下绕过内核某些加固措施的能力 (例如 ASLR 绕过、无可证实后果的事件计时或探测)。这些只是弱点，不是漏洞。)

  * missing argument checks and failure to report certain errors with no
    immediate consequence.
    (缺少参数检查，以及未报告某些没有直接后果的错误。)

* **Random information leaks**:
  (**随机信息泄露**：)

  This concerns information leaks of small data parts that happen to be there
  and that cannot be chosen by the attacker, or face access restrictions:
  (这涉及恰好存在于那里、攻击者无法挑选、或面临访问限制的小数据片段的信息泄露：)

  * structure padding reported by syscalls or other interfaces.
    (系统调用或其他接口报告的结构体填充字节。)

  * identifiers, partial data, non-terminated strings reported in error
    messages.
    (错误消息中报告的标识符、部分数据、未以空字符结尾的字符串。)

  * Leaks of kernel memory addresses/pointers do not constitute an immediately
    exploitable vector and are not security bugs, though they must be reported
    and fixed.
    (内核内存地址/指针的泄露不构成可直接利用的攻击向量，不属于安全缺陷，尽管它们必须被报告并修复。)

* **Crafted file system images**:
  (**精心构造的文件系统镜像**：)

  * bugs triggered by mounting a corrupted or maliciously crafted file system
    image are generally not security bugs, as the kernel assumes the underlying
    storage media is under the administrator's control, unless the filesystem
    driver is specifically documented as being hardened against untrusted media.
    (由挂载已损坏或恶意构造的文件系统镜像触发的缺陷通常不是安全缺陷，因为内核假定底层存储介质处于管理员控制之下，除非文件系统驱动被明确文档化为针对不受信任介质做了加固。)

  * issues that are resolved, mitigated, or detected by running a filesystem
    consistency check (fsck) on the image prior to mounting.
    (在挂载前对镜像运行文件系统一致性检查 (fsck) 即可解决、缓解或检出的问题。)

* **Physical access**:
  (**物理访问**：)

  Issues that require physical access to the machine, hardware modification, or
  the use of specialized hardware (e.g., logic analyzers, DMA-attack tools over
  PCI-E/Thunderbolt) are out of scope unless the system is explicitly
  configured with technologies meant to defend against such attacks
  (e.g. IOMMU).
  (需要物理接触机器、修改硬件或使用专用硬件 (例如逻辑分析仪、基于 PCI-E/Thunderbolt 的 DMA 攻击工具) 的问题不在范围之内，除非系统明确配置了用于防御此类攻击的技术 (例如 IOMMU)。)

* **Functional and performance regressions**:
  (**功能与性能退化**：)

  Any issue that can be mitigated by setting proper permissions and limits
  doesn't qualify as a security bug.
  (任何可以通过设置适当权限与限额来缓解的问题都不算安全缺陷。)
