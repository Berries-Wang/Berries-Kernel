.. SPDX-License-Identifier: GPL-2.0

===========================================================
Intel Trust Domain Extensions (TDX)(Intel 信任域扩展 (TDX))
===========================================================

Overview(概述)
==============
Intel's Trust Domain Extensions (TDX) protect confidential guest VMs from the
host and physical attacks.  A CPU-attested software module called 'the TDX
module' runs inside a new CPU isolated range to provide the functionalities to
manage and run protected VMs, a.k.a, TDX guests or TDs.
(Intel 的信任域扩展 (TDX) 保护机密客户机 VM 免受宿主机和物理攻击。一个经过
CPU 证明 (attested) 的软件模块——"TDX module"——运行在一个新的 CPU 隔离区域
内，提供管理和运行受保护 VM(即 TDX guest 或 TD)所需的功能。)

Please refer to [1] for the whitepaper, specifications and other resources.
(白皮书、规范及其他资源请参见 [1]。)

This documentation describes TDX-specific KVM ABIs.  The TDX module needs to be
initialized before it can be used by KVM to run any TDX guests.  The host
core-kernel provides the support of initializing the TDX module, which is
described in the Documentation/arch/x86/tdx.rst.
(本文档描述 TDX 特有的 KVM ABI。TDX module 需要先完成初始化，KVM 才能用它运行
任何 TDX guest。宿主核心内核提供初始化 TDX module 的支持，相关内容见
Documentation/arch/x86/tdx.rst。)

API description(API 描述)
=========================

KVM_MEMORY_ENCRYPT_OP(原名)
---------------------------
:Type: vm ioctl, vcpu ioctl
  (类型：vm ioctl、vcpu ioctl)

For TDX operations, KVM_MEMORY_ENCRYPT_OP is re-purposed to be generic
ioctl with TDX specific sub-ioctl() commands.

::

  /* Trust Domain Extensions sub-ioctl() commands. */
  enum kvm_tdx_cmd_id {
          KVM_TDX_CAPABILITIES = 0,
          KVM_TDX_INIT_VM,
          KVM_TDX_INIT_VCPU,
          KVM_TDX_INIT_MEM_REGION,
          KVM_TDX_FINALIZE_VM,
          KVM_TDX_GET_CPUID,

          KVM_TDX_CMD_NR_MAX,
  };

  struct kvm_tdx_cmd {
        /* enum kvm_tdx_cmd_id */
        __u32 id;
        /* flags for sub-command. If sub-command doesn't use this, set zero. */
        __u32 flags;
        /*
         * data for each sub-command. An immediate or a pointer to the actual
         * data in process virtual address.  If sub-command doesn't use it,
         * set zero.
         */
        __u64 data;
        /*
         * Auxiliary error code.  The sub-command may return TDX SEAMCALL
         * status code in addition to -Exxx.
         */
        __u64 hw_error;
  };

(对 TDX 操作而言，KVM_MEMORY_ENCRYPT_OP 被改作一个通用 ioctl，其下带有 TDX
特有的子 ioctl() 命令。)

KVM_TDX_CAPABILITIES(原名)
--------------------------
:Type: vm ioctl
  (类型：vm ioctl)
:Returns: 0 on success, <0 on error
  (返回值：成功时为 0，出错时为 <0)

Return the TDX capabilities that current KVM supports with the specific TDX
module loaded in the system.  It reports what features/capabilities are allowed
to be configured to the TDX guest.
(在系统中加载了特定 TDX module 的情况下，返回当前 KVM 所支持的 TDX 能力。
它报告允许对 TDX guest 配置哪些特性/能力。)

- id: KVM_TDX_CAPABILITIES
  (id：KVM_TDX_CAPABILITIES)
- flags: must be 0
  (flags：必须为 0)
- data: pointer to struct kvm_tdx_capabilities
  (data：指向 struct kvm_tdx_capabilities 的指针)
- hw_error: must be 0
  (hw_error：必须为 0)

::

  struct kvm_tdx_capabilities {
        __u64 supported_attrs;
        __u64 supported_xfam;

        /* TDG.VP.VMCALL hypercalls executed in kernel and forwarded to
         * userspace, respectively
         */
        __u64 kernel_tdvmcallinfo_1_r11;
        __u64 user_tdvmcallinfo_1_r11;

        /* TDG.VP.VMCALL instruction executions subfunctions executed in kernel
         * and forwarded to userspace, respectively
         */
        __u64 kernel_tdvmcallinfo_1_r12;
        __u64 user_tdvmcallinfo_1_r12;

        __u64 reserved[250];

        /* Configurable CPUID bits for userspace */
        struct kvm_cpuid2 cpuid;
  };


KVM_TDX_INIT_VM(原名)
---------------------
:Type: vm ioctl
  (类型：vm ioctl)
:Returns: 0 on success, <0 on error
  (返回值：成功时为 0，出错时为 <0)

Perform TDX specific VM initialization.  This needs to be called after
KVM_CREATE_VM and before creating any VCPUs.
(执行 TDX 特有的 VM 初始化。需要在 KVM_CREATE_VM 之后、创建任何 VCPU 之前
调用。)

- id: KVM_TDX_INIT_VM
  (id：KVM_TDX_INIT_VM)
- flags: must be 0
  (flags：必须为 0)
- data: pointer to struct kvm_tdx_init_vm
  (data：指向 struct kvm_tdx_init_vm 的指针)
- hw_error: must be 0
  (hw_error：必须为 0)

::

  struct kvm_tdx_init_vm {
          __u64 attributes;
          __u64 xfam;
          __u64 mrconfigid[6];          /* sha384 digest */
          __u64 mrowner[6];             /* sha384 digest */
          __u64 mrownerconfig[6];       /* sha384 digest */

          /* The total space for TD_PARAMS before the CPUIDs is 256 bytes */
          __u64 reserved[12];

        /*
         * Call KVM_TDX_INIT_VM before vcpu creation, thus before
         * KVM_SET_CPUID2.
         * This configuration supersedes KVM_SET_CPUID2s for VCPUs because the
         * TDX module directly virtualizes those CPUIDs without VMM.  The user
         * space VMM, e.g. qemu, should make KVM_SET_CPUID2 consistent with
         * those values.  If it doesn't, KVM may have wrong idea of vCPUIDs of
         * the guest, and KVM may wrongly emulate CPUIDs or MSRs that the TDX
         * module doesn't virtualize.
         */
          struct kvm_cpuid2 cpuid;
  };


KVM_TDX_INIT_VCPU(原名)
-----------------------
:Type: vcpu ioctl
  (类型：vcpu ioctl)
:Returns: 0 on success, <0 on error
  (返回值：成功时为 0，出错时为 <0)

Perform TDX specific VCPU initialization.
(执行 TDX 特有的 VCPU 初始化。)

- id: KVM_TDX_INIT_VCPU
  (id：KVM_TDX_INIT_VCPU)
- flags: must be 0
  (flags：必须为 0)
- data: initial value of the guest TD VCPU RCX
  (data：guest TD VCPU RCX 的初始值)
- hw_error: must be 0
  (hw_error：必须为 0)

KVM_TDX_INIT_MEM_REGION(原名)
-----------------------------
:Type: vcpu ioctl
  (类型：vcpu ioctl)
:Returns: 0 on success, <0 on error
  (返回值：成功时为 0，出错时为 <0)

Initialize @nr_pages TDX guest private memory starting from @gpa with userspace
provided data from @source_addr. @source_addr must be PAGE_SIZE-aligned.
(以 @source_addr 提供的用户空间数据，初始化从 @gpa 开始的 @nr_pages 页 TDX
guest 私有内存。@source_addr 必须按 PAGE_SIZE 对齐。)

Note, before calling this sub command, memory attribute of the range
[gpa, gpa + nr_pages] needs to be private.  Userspace can use
KVM_SET_MEMORY_ATTRIBUTES to set the attribute.
(注意，在调用这个子命令之前，[gpa, gpa + nr_pages] 范围的内存属性需要是
private。用户空间可以用 KVM_SET_MEMORY_ATTRIBUTES 来设置该属性。)

If KVM_TDX_MEASURE_MEMORY_REGION flag is specified, it also extends measurement.
(如果指定了 KVM_TDX_MEASURE_MEMORY_REGION 标志，还会扩展度量值 (measurement)。)

- id: KVM_TDX_INIT_MEM_REGION
  (id：KVM_TDX_INIT_MEM_REGION)
- flags: currently only KVM_TDX_MEASURE_MEMORY_REGION is defined
  (flags：目前仅定义了 KVM_TDX_MEASURE_MEMORY_REGION)
- data: pointer to struct kvm_tdx_init_mem_region
  (data：指向 struct kvm_tdx_init_mem_region 的指针)
- hw_error: must be 0
  (hw_error：必须为 0)

::

  #define KVM_TDX_MEASURE_MEMORY_REGION   (1UL << 0)

  struct kvm_tdx_init_mem_region {
          __u64 source_addr;
          __u64 gpa;
          __u64 nr_pages;
  };


KVM_TDX_FINALIZE_VM(原名)
-------------------------
:Type: vm ioctl
  (类型：vm ioctl)
:Returns: 0 on success, <0 on error
  (返回值：成功时为 0，出错时为 <0)

Complete measurement of the initial TD contents and mark it ready to run.
(完成初始 TD 内容的度量，并将其标记为可以运行。)

- id: KVM_TDX_FINALIZE_VM
  (id：KVM_TDX_FINALIZE_VM)
- flags: must be 0
  (flags：必须为 0)
- data: must be 0
  (data：必须为 0)
- hw_error: must be 0
  (hw_error：必须为 0)


KVM_TDX_GET_CPUID(原名)
-----------------------
:Type: vcpu ioctl
  (类型：vcpu ioctl)
:Returns: 0 on success, <0 on error
  (返回值：成功时为 0，出错时为 <0)

Get the CPUID values that the TDX module virtualizes for the TD guest.
When it returns -E2BIG, the user space should allocate a larger buffer and
retry. The minimum buffer size is updated in the nent field of the
struct kvm_cpuid2.
(获取 TDX module 为 TD guest 虚拟化的 CPUID 值。当其返回 -E2BIG 时，用户空间
应分配更大的缓冲区并重试。最小缓冲区大小会更新到 struct kvm_cpuid2 的 nent
字段中。)

- id: KVM_TDX_GET_CPUID
  (id：KVM_TDX_GET_CPUID)
- flags: must be 0
  (flags：必须为 0)
- data: pointer to struct kvm_cpuid2 (in/out)
  (data：指向 struct kvm_cpuid2 的指针(入/出))
- hw_error: must be 0 (out)
  (hw_error：必须为 0(出参))

::

  struct kvm_cpuid2 {
	  __u32 nent;
	  __u32 padding;
	  struct kvm_cpuid_entry2 entries[0];
  };

  struct kvm_cpuid_entry2 {
	  __u32 function;
	  __u32 index;
	  __u32 flags;
	  __u32 eax;
	  __u32 ebx;
	  __u32 ecx;
	  __u32 edx;
	  __u32 padding[3];
  };

KVM TDX creation flow(KVM TDX 创建流程)
=======================================
In addition to the standard KVM flow, new TDX ioctls need to be called.  The
control flow is as follows:
(除了标准 KVM 流程之外，还需要调用新的 TDX ioctl。控制流程如下：)

#. Check system wide capability
   (检查系统级能力)

   * KVM_CAP_VM_TYPES: Check if VM type is supported and if KVM_X86_TDX_VM
     is supported.
     (KVM_CAP_VM_TYPES：检查 VM 类型是否受支持，以及 KVM_X86_TDX_VM 是否
     受支持。)

#. Create VM
   (创建 VM)

   * KVM_CREATE_VM
   * KVM_TDX_CAPABILITIES: Query TDX capabilities for creating TDX guests.
     (KVM_TDX_CAPABILITIES：查询用于创建 TDX guest 的 TDX 能力。)
   * KVM_CHECK_EXTENSION(KVM_CAP_MAX_VCPUS): Query maximum VCPUs the TD can
     support at VM level (TDX has its own limitation on this).
     (KVM_CHECK_EXTENSION(KVM_CAP_MAX_VCPUS)：查询 TD 在 VM 层面可支持的
     最大 VCPU 数(TDX 对此有自身的限制)。)
   * KVM_SET_TSC_KHZ: Configure TD's TSC frequency if a different TSC frequency
     than host is desired.  This is Optional.
     (KVM_SET_TSC_KHZ：如果希望使用与宿主不同的 TSC 频率，则配置 TD 的 TSC
     频率。这是可选的。)
   * KVM_TDX_INIT_VM: Pass TDX specific VM parameters.
     (KVM_TDX_INIT_VM：传入 TDX 特有的 VM 参数。)

#. Create VCPU
   (创建 VCPU)

   * KVM_CREATE_VCPU
   * KVM_TDX_INIT_VCPU: Pass TDX specific VCPU parameters.
     (KVM_TDX_INIT_VCPU：传入 TDX 特有的 VCPU 参数。)
   * KVM_SET_CPUID2: Configure TD's CPUIDs.
     (KVM_SET_CPUID2：配置 TD 的 CPUID。)
   * KVM_SET_MSRS: Configure TD's MSRs.
     (KVM_SET_MSRS：配置 TD 的 MSR。)

#. Initialize initial guest memory
   (初始化初始客户机内存)

   * Prepare content of initial guest memory.
     (准备初始客户机内存的内容。)
   * KVM_TDX_INIT_MEM_REGION: Add initial guest memory.
     (KVM_TDX_INIT_MEM_REGION：添加初始客户机内存。)
   * KVM_TDX_FINALIZE_VM: Finalize the measurement of the TDX guest.
     (KVM_TDX_FINALIZE_VM：完成 TDX guest 的度量。)

#. Run VCPU
   (运行 VCPU)

References(参考资料)
====================

https://www.intel.com/content/www/us/en/developer/tools/trust-domain-extensions/documentation.html
