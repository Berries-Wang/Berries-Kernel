.. SPDX-License-Identifier: GPL-2.0

=========================================================
Secure Encrypted Virtualization (SEV)(安全加密虚拟化 SEV)
=========================================================

Overview(概述)
==============

Secure Encrypted Virtualization (SEV) is a feature found on AMD processors.
(安全加密虚拟化 (SEV) 是 AMD 处理器上的一项特性。)

SEV is an extension to the AMD-V architecture which supports running
virtual machines (VMs) under the control of a hypervisor. When enabled,
the memory contents of a VM will be transparently encrypted with a key
unique to that VM.
(SEV 是 AMD-V 架构的扩展，支持在 hypervisor 的控制下运行虚拟机 (VM)。启用后，VM 的内存内容会使用该 VM 独有的密钥透明加密。)

The hypervisor can determine the SEV support through the CPUID
instruction. The CPUID function 0x8000001f reports information related
to SEV::

	0x8000001f[eax]:
			Bit[1] 	indicates support for SEV
	    ...
		  [ecx]:
			Bits[31:0]  Number of encrypted guests supported simultaneously

(hypervisor 可以通过 CPUID 指令判断 SEV 支持情况。CPUID 功能号 0x8000001f 会报告与 SEV 相关的信息：)

If support for SEV is present, MSR 0xc001_0010 (MSR_AMD64_SYSCFG) and MSR 0xc001_0015
(MSR_K7_HWCR) can be used to determine if it can be enabled::

	0xc001_0010:
		Bit[23]	   1 = memory encryption can be enabled
			   0 = memory encryption can not be enabled

	0xc001_0015:
		Bit[0]	   1 = memory encryption can be enabled
			   0 = memory encryption can not be enabled

(如果存在 SEV 支持，可以使用 MSR 0xc001_0010 (MSR_AMD64_SYSCFG) 和 MSR 0xc001_0015 (MSR_K7_HWCR) 来判断它能否被启用：)

When SEV support is available, it can be enabled in a specific VM by
setting the SEV bit before executing VMRUN.::

	VMCB[0x90]:
		Bit[1]	    1 = SEV is enabled
			    0 = SEV is disabled

(当 SEV 支持可用时，可以在执行 VMRUN 之前设置 SEV 位，从而在特定的 VM 中启用它。)

SEV hardware uses ASIDs to associate a memory encryption key with a VM.
Hence, the ASID for the SEV-enabled guests must be from 1 to a maximum value
defined in the CPUID 0x8000001f[ecx] field.
(SEV 硬件使用 ASID 将内存加密密钥与 VM 关联。因此，启用 SEV 的客户机的 ASID 必须取 1 到 CPUID 0x8000001f[ecx] 字段定义的最大值之间的值。)

The KVM_MEMORY_ENCRYPT_OP ioctl(KVM_MEMORY_ENCRYPT_OP ioctl 命令)
=================================================================

The main ioctl to access SEV is KVM_MEMORY_ENCRYPT_OP, which operates on
the VM file descriptor.  If the argument to KVM_MEMORY_ENCRYPT_OP is NULL,
the ioctl returns 0 if SEV is enabled and ``ENOTTY`` if it is disabled
(on some older versions of Linux, the ioctl tries to run normally even
with a NULL argument, and therefore will likely return ``EFAULT`` instead
of zero if SEV is enabled).  If non-NULL, the argument to
KVM_MEMORY_ENCRYPT_OP must be a struct kvm_sev_cmd::

       struct kvm_sev_cmd {
               __u32 id;
               __u64 data;
               __u32 error;
               __u32 sev_fd;
       };

(访问 SEV 的主要 ioctl 是 KVM_MEMORY_ENCRYPT_OP，它作用于 VM 的文件描述符。如果 KVM_MEMORY_ENCRYPT_OP 的参数为 NULL，则 SEV 启用时该 ioctl 返回 0，禁用时返回 ``ENOTTY`` (在某些较旧版本的 Linux 中，即使参数为 NULL 该 ioctl 也会尝试正常执行，因此 SEV 启用时它多半会返回 ``EFAULT`` 而不是 0)。如果参数非 NULL，则必须是 struct kvm_sev_cmd 类型：)


The ``id`` field contains the subcommand, and the ``data`` field points to
another struct containing arguments specific to command.  The ``sev_fd``
should point to a file descriptor that is opened on the ``/dev/sev``
device, if needed (see individual commands).
(``id`` 字段包含子命令，``data`` 字段指向另一个结构体，其中包含该命令特有的参数。如有需要，``sev_fd`` 应指向在 ``/dev/sev`` 设备上打开的文件描述符 (见各条命令的说明)。)

On output, ``error`` is zero on success, or an error code.  Error codes
are defined in ``<linux/psp-dev.h>``.
(输出时，``error`` 成功为 0，否则为错误码。错误码定义在 ``<linux/psp-dev.h>`` 中。)

KVM implements the following commands to support common lifecycle events of SEV
guests, such as launching, running, snapshotting, migrating and decommissioning.
(KVM 实现了以下命令，用于支持 SEV 客户机生命周期中的常见事件，例如启动、运行、快照、迁移和退役。)

1. KVM_SEV_INIT2(初始化 SEV 平台上下文)
---------------------------------------

The KVM_SEV_INIT2 command is used by the hypervisor to initialize the SEV platform
context. In a typical workflow, this command should be the first command issued.
(hypervisor 使用 KVM_SEV_INIT2 命令来初始化 SEV 平台上下文。在典型工作流中，该命令应当是第一条下发的命令。)

For this command to be accepted, either KVM_X86_SEV_VM or KVM_X86_SEV_ES_VM
must have been passed to the KVM_CREATE_VM ioctl.  A virtual machine created
with those machine types in turn cannot be run until KVM_SEV_INIT2 is invoked.
(要使该命令被接受，必须在 KVM_CREATE_VM ioctl 中传入 KVM_X86_SEV_VM 或 KVM_X86_SEV_ES_VM。反过来，用这些机型创建的虚拟机在调用 KVM_SEV_INIT2 之前也无法运行。)

Parameters: struct kvm_sev_init (in)
(参数：struct kvm_sev_init(输入)。)

Returns: 0 on success, -negative on error
(返回：成功为 0，出错为负错误码。)

::

        struct kvm_sev_init {
                __u64 vmsa_features;  /* initial value of features field in VMSA */
                __u32 flags;          /* must be 0 */
                __u16 ghcb_version;   /* maximum guest GHCB version allowed */
                __u16 pad1;
                __u32 pad2[8];
        };

It is an error if the hypervisor does not support any of the bits that
are set in ``flags`` or ``vmsa_features``.  ``vmsa_features`` must be
0 for SEV virtual machines, as they do not have a VMSA.
(如果 hypervisor 不支持 ``flags`` 或 ``vmsa_features`` 中置位的任何比特，则为错误。SEV 虚拟机没有 VMSA，因此其 ``vmsa_features`` 必须为 0。)

``ghcb_version`` must be 0 for SEV virtual machines, as they do not issue GHCB
requests. If ``ghcb_version`` is 0 for any other guest type, then the maximum
allowed guest GHCB protocol will default to version 2.
(SEV 虚拟机不会发出 GHCB 请求，因此其 ``ghcb_version`` 必须为 0。如果其他客户机类型的 ``ghcb_version`` 为 0，则允许的客户机 GHCB 协议最高版本默认为 2。)

This command replaces the deprecated KVM_SEV_INIT and KVM_SEV_ES_INIT commands.
The commands did not have any parameters (the ```data``` field was unused) and
only work for the KVM_X86_DEFAULT_VM machine type (0).
(该命令取代了已废弃的 KVM_SEV_INIT 和 KVM_SEV_ES_INIT 命令。后两个命令没有任何参数(``data`` 字段未被使用)，并且只适用于 KVM_X86_DEFAULT_VM 机型 (0)。)

They behave as if:
(它们的行为等价于：)

* the VM type is KVM_X86_SEV_VM for KVM_SEV_INIT, or KVM_X86_SEV_ES_VM for
  KVM_SEV_ES_INIT
  (对于 KVM_SEV_INIT，VM 类型为 KVM_X86_SEV_VM；对于 KVM_SEV_ES_INIT，VM 类型为 KVM_X86_SEV_ES_VM。)

* the ``flags`` and ``vmsa_features`` fields of ``struct kvm_sev_init`` are
  set to zero, and ``ghcb_version`` is set to 0 for KVM_SEV_INIT and 1 for
  KVM_SEV_ES_INIT.
  (``struct kvm_sev_init`` 的 ``flags`` 和 ``vmsa_features`` 字段置为 0；``ghcb_version`` 在 KVM_SEV_INIT 时置为 0、在 KVM_SEV_ES_INIT 时置为 1。)

If the ``KVM_X86_SEV_VMSA_FEATURES`` attribute does not exist, the hypervisor only
supports KVM_SEV_INIT and KVM_SEV_ES_INIT.  In that case, note that KVM_SEV_ES_INIT
might set the debug swap VMSA feature (bit 5) depending on the value of the
``debug_swap`` parameter of ``kvm-amd.ko``.
(如果 ``KVM_X86_SEV_VMSA_FEATURES`` 属性不存在，则 hypervisor 只支持 KVM_SEV_INIT 和 KVM_SEV_ES_INIT。在这种情况下要注意，KVM_SEV_ES_INIT 可能会根据 ``kvm-amd.ko`` 的 ``debug_swap`` 参数值设置 debug swap VMSA 特性 (比特 5)。)

2. KVM_SEV_LAUNCH_START(启动：创建内存加密上下文)
-------------------------------------------------

The KVM_SEV_LAUNCH_START command is used for creating the memory encryption
context. To create the encryption context, user must provide a guest policy,
the owner's public Diffie-Hellman (PDH) key and session information.
(KVM_SEV_LAUNCH_START 命令用于创建内存加密上下文。要创建加密上下文，用户必须提供客户机策略、所有者的公钥 Diffie-Hellman (PDH) 密钥以及会话信息。)

Parameters: struct  kvm_sev_launch_start (in/out)
(参数：struct kvm_sev_launch_start(输入/输出)。)

Returns: 0 on success, -negative on error
(返回：成功为 0，出错为负错误码。)

::

        struct kvm_sev_launch_start {
                __u32 handle;           /* if zero then firmware creates a new handle */
                __u32 policy;           /* guest's policy */

                __u64 dh_uaddr;         /* userspace address pointing to the guest owner's PDH key */
                __u32 dh_len;

                __u64 session_addr;     /* userspace address which points to the guest session information */
                __u32 session_len;
        };

On success, the 'handle' field contains a new handle and on error, a negative value.
(成功时 'handle' 字段包含一个新句柄，出错时为负值。)

KVM_SEV_LAUNCH_START requires the ``sev_fd`` field to be valid.
(KVM_SEV_LAUNCH_START 要求 ``sev_fd`` 字段有效。)

For more details, see SEV spec Section 6.2.
(更多细节见 SEV 规范 6.2 节。)

3. KVM_SEV_LAUNCH_UPDATE_DATA(启动：加密内存区域)
-------------------------------------------------

The KVM_SEV_LAUNCH_UPDATE_DATA is used for encrypting a memory region. It also
calculates a measurement of the memory contents. The measurement is a signature
of the memory contents that can be sent to the guest owner as an attestation
that the memory was encrypted correctly by the firmware.
(KVM_SEV_LAUNCH_UPDATE_DATA 用于加密一段内存区域，同时计算内存内容的度量值。该度量值是内存内容的签名，可以发送给客户机所有者，作为内存已被固件正确加密的证明。)

Parameters (in): struct  kvm_sev_launch_update_data
(参数(输入)：struct kvm_sev_launch_update_data。)

Returns: 0 on success, -negative on error
(返回：成功为 0，出错为负错误码。)

::

        struct kvm_sev_launch_update {
                __u64 uaddr;    /* userspace address to be encrypted (must be 16-byte aligned) */
                __u32 len;      /* length of the data to be encrypted (must be 16-byte aligned) */
        };

For more details, see SEV spec Section 6.3.
(更多细节见 SEV 规范 6.3 节。)

4. KVM_SEV_LAUNCH_MEASURE(启动：获取度量值)
-------------------------------------------

The KVM_SEV_LAUNCH_MEASURE command is used to retrieve the measurement of the
data encrypted by the KVM_SEV_LAUNCH_UPDATE_DATA command. The guest owner may
wait to provide the guest with confidential information until it can verify the
measurement. Since the guest owner knows the initial contents of the guest at
boot, the measurement can be verified by comparing it to what the guest owner
expects.
(KVM_SEV_LAUNCH_MEASURE 命令用于获取 KVM_SEV_LAUNCH_UPDATE_DATA 命令所加密数据的度量值。客户机所有者可以等到能够验证度量值之后，再向客户机提供机密信息。由于客户机所有者知道客户机启动时的初始内容，因此可以通过将度量值与其期望值比较来完成验证。)

If len is zero on entry, the measurement blob length is written to len and
uaddr is unused.
(如果入口时 len 为 0，则度量值数据的长度会写入 len，uaddr 不使用。)

Parameters (in): struct  kvm_sev_launch_measure
(参数(输入)：struct kvm_sev_launch_measure。)

Returns: 0 on success, -negative on error
(返回：成功为 0，出错为负错误码。)

::

        struct kvm_sev_launch_measure {
                __u64 uaddr;    /* where to copy the measurement */
                __u32 len;      /* length of measurement blob */
        };

For more details on the measurement verification flow, see SEV spec Section 6.4.
(度量值验证流程的更多细节见 SEV 规范 6.4 节。)

5. KVM_SEV_LAUNCH_FINISH(启动：完成启动流程)
--------------------------------------------

After completion of the launch flow, the KVM_SEV_LAUNCH_FINISH command can be
issued to make the guest ready for the execution.
(启动流程完成之后，可以下发 KVM_SEV_LAUNCH_FINISH 命令，使客户机准备好执行。)

Returns: 0 on success, -negative on error
(返回：成功为 0，出错为负错误码。)

6. KVM_SEV_GUEST_STATUS(查询客户机状态)
---------------------------------------

The KVM_SEV_GUEST_STATUS command is used to retrieve status information about a
SEV-enabled guest.
(KVM_SEV_GUEST_STATUS 命令用于获取启用 SEV 的客户机的状态信息。)

Parameters (out): struct kvm_sev_guest_status
(参数(输出)：struct kvm_sev_guest_status。)

Returns: 0 on success, -negative on error
(返回：成功为 0，出错为负错误码。)

::

        struct kvm_sev_guest_status {
                __u32 handle;   /* guest handle */
                __u32 policy;   /* guest policy */
                __u8 state;     /* guest state (see enum below) */
        };

SEV guest state:
(SEV 客户机状态：)

::

        enum {
        SEV_STATE_INVALID = 0;
        SEV_STATE_LAUNCHING,    /* guest is currently being launched */
        SEV_STATE_SECRET,       /* guest is being launched and ready to accept the ciphertext data */
        SEV_STATE_RUNNING,      /* guest is fully launched and running */
        SEV_STATE_RECEIVING,    /* guest is being migrated in from another SEV machine */
        SEV_STATE_SENDING       /* guest is getting migrated out to another SEV machine */
        };

7. KVM_SEV_DBG_DECRYPT(调试：解密内存区域)
------------------------------------------

The KVM_SEV_DEBUG_DECRYPT command can be used by the hypervisor to request the
firmware to decrypt the data at the given memory region.
(hypervisor 可以使用 KVM_SEV_DEBUG_DECRYPT 命令请求固件解密给定内存区域中的数据。)

Parameters (in): struct kvm_sev_dbg
(参数(输入)：struct kvm_sev_dbg。)

Returns: 0 on success, -negative on error
(返回：成功为 0，出错为负错误码。)

::

        struct kvm_sev_dbg {
                __u64 src_uaddr;        /* userspace address of data to decrypt */
                __u64 dst_uaddr;        /* userspace address of destination */
                __u32 len;              /* length of memory region to decrypt */
        };

The command returns an error if the guest policy does not allow debugging.
(如果客户机策略不允许调试，该命令会返回错误。)

8. KVM_SEV_DBG_ENCRYPT(调试：加密内存区域)
------------------------------------------

The KVM_SEV_DEBUG_ENCRYPT command can be used by the hypervisor to request the
firmware to encrypt the data at the given memory region.
(hypervisor 可以使用 KVM_SEV_DEBUG_ENCRYPT 命令请求固件加密给定内存区域中的数据。)

Parameters (in): struct kvm_sev_dbg
(参数(输入)：struct kvm_sev_dbg。)

Returns: 0 on success, -negative on error
(返回：成功为 0，出错为负错误码。)

::

        struct kvm_sev_dbg {
                __u64 src_uaddr;        /* userspace address of data to encrypt */
                __u64 dst_uaddr;        /* userspace address of destination */
                __u32 len;              /* length of memory region to encrypt */
        };

The command returns an error if the guest policy does not allow debugging.
(如果客户机策略不允许调试，该命令会返回错误。)

9. KVM_SEV_LAUNCH_SECRET(启动：注入机密数据)
--------------------------------------------

The KVM_SEV_LAUNCH_SECRET command can be used by the hypervisor to inject secret
data after the measurement has been validated by the guest owner.
(在度量值经客户机所有者验证之后，hypervisor 可以使用 KVM_SEV_LAUNCH_SECRET 命令注入机密数据。)

Parameters (in): struct kvm_sev_launch_secret
(参数(输入)：struct kvm_sev_launch_secret。)

Returns: 0 on success, -negative on error
(返回：成功为 0，出错为负错误码。)

::

        struct kvm_sev_launch_secret {
                __u64 hdr_uaddr;        /* userspace address containing the packet header */
                __u32 hdr_len;

                __u64 guest_uaddr;      /* the guest memory region where the secret should be injected */
                __u32 guest_len;

                __u64 trans_uaddr;      /* the hypervisor memory region which contains the secret */
                __u32 trans_len;
        };

10. KVM_SEV_GET_ATTESTATION_REPORT(获取 attestation 报告)
---------------------------------------------------------

The KVM_SEV_GET_ATTESTATION_REPORT command can be used by the hypervisor to query the attestation
report containing the SHA-256 digest of the guest memory and VMSA passed through the KVM_SEV_LAUNCH
commands and signed with the PEK. The digest returned by the command should match the digest
used by the guest owner with the KVM_SEV_LAUNCH_MEASURE.
(hypervisor 可以使用 KVM_SEV_GET_ATTESTATION_REPORT 命令查询 attestation 报告，报告中包含经过 KVM_SEV_LAUNCH 命令处理的客户机内存与 VMSA 的 SHA-256 摘要，并由 PEK 签名。该命令返回的摘要应当与客户机所有者在 KVM_SEV_LAUNCH_MEASURE 中使用的摘要一致。)

If len is zero on entry, the measurement blob length is written to len and
uaddr is unused.
(如果入口时 len 为 0，则度量值数据的长度会写入 len，uaddr 不使用。)

Parameters (in): struct kvm_sev_attestation
(参数(输入)：struct kvm_sev_attestation。)

Returns: 0 on success, -negative on error
(返回：成功为 0，出错为负错误码。)

::

        struct kvm_sev_attestation_report {
                __u8 mnonce[16];        /* A random mnonce that will be placed in the report */

                __u64 uaddr;            /* userspace address where the report should be copied */
                __u32 len;
        };

11. KVM_SEV_SEND_START(迁移：开始发送)
--------------------------------------

The KVM_SEV_SEND_START command can be used by the hypervisor to create an
outgoing guest encryption context.
(hypervisor 可以使用 KVM_SEV_SEND_START 命令创建用于发出的客户机加密上下文。)

If session_len is zero on entry, the length of the guest session information is
written to session_len and all other fields are not used.
(如果入口时 session_len 为 0，则客户机会话信息的长度会写入 session_len，其余字段均不使用。)

Parameters (in): struct kvm_sev_send_start
(参数(输入)：struct kvm_sev_send_start。)

Returns: 0 on success, -negative on error
(返回：成功为 0，出错为负错误码。)

::

        struct kvm_sev_send_start {
                __u32 policy;                 /* guest policy */

                __u64 pdh_cert_uaddr;         /* platform Diffie-Hellman certificate */
                __u32 pdh_cert_len;

                __u64 plat_certs_uaddr;        /* platform certificate chain */
                __u32 plat_certs_len;

                __u64 amd_certs_uaddr;        /* AMD certificate */
                __u32 amd_certs_len;

                __u64 session_uaddr;          /* Guest session information */
                __u32 session_len;
        };

12. KVM_SEV_SEND_UPDATE_DATA(迁移：发送更新数据)
------------------------------------------------

The KVM_SEV_SEND_UPDATE_DATA command can be used by the hypervisor to encrypt the
outgoing guest memory region with the encryption context creating using
KVM_SEV_SEND_START.
(hypervisor 可以使用 KVM_SEV_SEND_UPDATE_DATA 命令，用 KVM_SEV_SEND_START 创建的加密上下文加密待发出的客户机内存区域。)

If hdr_len or trans_len are zero on entry, the length of the packet header and
transport region are written to hdr_len and trans_len respectively, and all
other fields are not used.
(如果入口时 hdr_len 或 trans_len 为 0，则包头部与传输区域的长度会分别写入 hdr_len 和 trans_len，其余字段均不使用。)

Parameters (in): struct kvm_sev_send_update_data
(参数(输入)：struct kvm_sev_send_update_data。)

Returns: 0 on success, -negative on error
(返回：成功为 0，出错为负错误码。)

::

        struct kvm_sev_launch_send_update_data {
                __u64 hdr_uaddr;        /* userspace address containing the packet header */
                __u32 hdr_len;

                __u64 guest_uaddr;      /* the source memory region to be encrypted */
                __u32 guest_len;

                __u64 trans_uaddr;      /* the destination memory region  */
                __u32 trans_len;
        };

13. KVM_SEV_SEND_FINISH(迁移：完成发送)
---------------------------------------

After completion of the migration flow, the KVM_SEV_SEND_FINISH command can be
issued by the hypervisor to delete the encryption context.
(迁移流程完成之后，hypervisor 可以下发 KVM_SEV_SEND_FINISH 命令来删除加密上下文。)

Returns: 0 on success, -negative on error
(返回：成功为 0，出错为负错误码。)

14. KVM_SEV_SEND_CANCEL(迁移：取消发送)
---------------------------------------

After completion of SEND_START, but before SEND_FINISH, the source VMM can issue the
SEND_CANCEL command to stop a migration. This is necessary so that a cancelled
migration can restart with a new target later.
(在 SEND_START 完成之后、SEND_FINISH 之前，源 VMM 可以下发 SEND_CANCEL 命令来停止迁移。这是必要的，这样被取消的迁移之后才能以新的目标重新开始。)

Returns: 0 on success, -negative on error
(返回：成功为 0，出错为负错误码。)

15. KVM_SEV_RECEIVE_START(迁移：开始接收)
-----------------------------------------

The KVM_SEV_RECEIVE_START command is used for creating the memory encryption
context for an incoming SEV guest. To create the encryption context, the user must
provide a guest policy, the platform public Diffie-Hellman (PDH) key and session
information.
(KVM_SEV_RECEIVE_START 命令用于为待接收的 SEV 客户机创建内存加密上下文。要创建加密上下文，用户必须提供客户机策略、平台公钥 Diffie-Hellman (PDH) 密钥以及会话信息。)

Parameters: struct  kvm_sev_receive_start (in/out)
(参数：struct kvm_sev_receive_start(输入/输出)。)

Returns: 0 on success, -negative on error
(返回：成功为 0，出错为负错误码。)

::

        struct kvm_sev_receive_start {
                __u32 handle;           /* if zero then firmware creates a new handle */
                __u32 policy;           /* guest's policy */

                __u64 pdh_uaddr;        /* userspace address pointing to the PDH key */
                __u32 pdh_len;

                __u64 session_uaddr;    /* userspace address which points to the guest session information */
                __u32 session_len;
        };

On success, the 'handle' field contains a new handle and on error, a negative value.
(成功时 'handle' 字段包含一个新句柄，出错时为负值。)

For more details, see SEV spec Section 6.12.
(更多细节见 SEV 规范 6.12 节。)

16. KVM_SEV_RECEIVE_UPDATE_DATA(迁移：接收更新数据)
---------------------------------------------------

The KVM_SEV_RECEIVE_UPDATE_DATA command can be used by the hypervisor to copy
the incoming buffers into the guest memory region with encryption context
created during the KVM_SEV_RECEIVE_START.
(hypervisor 可以使用 KVM_SEV_RECEIVE_UPDATE_DATA 命令，在 KVM_SEV_RECEIVE_START 创建的加密上下文中，把传入的缓冲区拷贝到客户机内存区域。)

Parameters (in): struct kvm_sev_receive_update_data
(参数(输入)：struct kvm_sev_receive_update_data。)

Returns: 0 on success, -negative on error
(返回：成功为 0，出错为负错误码。)

::

        struct kvm_sev_launch_receive_update_data {
                __u64 hdr_uaddr;        /* userspace address containing the packet header */
                __u32 hdr_len;

                __u64 guest_uaddr;      /* the destination guest memory region */
                __u32 guest_len;

                __u64 trans_uaddr;      /* the incoming buffer memory region  */
                __u32 trans_len;
        };

17. KVM_SEV_RECEIVE_FINISH(迁移：完成接收)
------------------------------------------

After completion of the migration flow, the KVM_SEV_RECEIVE_FINISH command can be
issued by the hypervisor to make the guest ready for execution.
(迁移流程完成之后，hypervisor 可以下发 KVM_SEV_RECEIVE_FINISH 命令，使客户机准备好执行。)

Returns: 0 on success, -negative on error
(返回：成功为 0，出错为负错误码。)

18. KVM_SEV_SNP_LAUNCH_START(SNP 启动：创建加密上下文)
------------------------------------------------------

The KVM_SNP_LAUNCH_START command is used for creating the memory encryption
context for the SEV-SNP guest. It must be called prior to issuing
KVM_SEV_SNP_LAUNCH_UPDATE or KVM_SEV_SNP_LAUNCH_FINISH;
(KVM_SNP_LAUNCH_START 命令用于为 SEV-SNP 客户机创建内存加密上下文。必须在下发 KVM_SEV_SNP_LAUNCH_UPDATE 或 KVM_SEV_SNP_LAUNCH_FINISH 之前调用。)

Parameters (in): struct  kvm_sev_snp_launch_start
(参数(输入)：struct kvm_sev_snp_launch_start。)

Returns: 0 on success, -negative on error
(返回：成功为 0，出错为负错误码。)

::

        struct kvm_sev_snp_launch_start {
                __u64 policy;           /* Guest policy to use. */
                __u8 gosvw[16];         /* Guest OS visible workarounds. */
                __u16 flags;            /* Must be zero. */
                __u8 pad0[6];
                __u64 pad1[4];
        };

See SNP_LAUNCH_START in the SEV-SNP specification [snp-fw-abi]_ for further
details on the input parameters in ``struct kvm_sev_snp_launch_start``.
(``struct kvm_sev_snp_launch_start`` 输入参数的更多细节见 SEV-SNP 规范中的 SNP_LAUNCH_START 一节。)

19. KVM_SEV_SNP_LAUNCH_UPDATE(SNP 启动：加载并度量内存)
-------------------------------------------------------

The KVM_SEV_SNP_LAUNCH_UPDATE command is used for loading userspace-provided
data into a guest GPA range, measuring the contents into the SNP guest context
created by KVM_SEV_SNP_LAUNCH_START, and then encrypting/validating that GPA
range so that it will be immediately readable using the encryption key
associated with the guest context once it is booted, after which point it can
attest the measurement associated with its context before unlocking any
secrets.
(KVM_SEV_SNP_LAUNCH_UPDATE 命令用于把用户态提供的数据加载进客户机 GPA 范围，将内容度量进 KVM_SEV_SNP_LAUNCH_START 创建的 SNP 客户机上下文，然后加密/验证该 GPA 范围，使客户机启动后能立即用与该客户机上下文关联的加密密钥读取这些内容；此后客户机可以在解锁任何机密之前，对其上下文关联的度量值进行 attest。)

It is required that the GPA ranges initialized by this command have had the
KVM_MEMORY_ATTRIBUTE_PRIVATE attribute set in advance. See the documentation
for KVM_SET_MEMORY_ATTRIBUTES for more details on this aspect.
(该命令初始化的 GPA 范围必须事先设置 KVM_MEMORY_ATTRIBUTE_PRIVATE 属性。这方面的更多细节见 KVM_SET_MEMORY_ATTRIBUTES 的文档。)

Upon success, this command is not guaranteed to have processed the entire
range requested. Instead, the ``gfn_start``, ``uaddr``, and ``len`` fields of
``struct kvm_sev_snp_launch_update`` will be updated to correspond to the
remaining range that has yet to be processed. The caller should continue
calling this command until those fields indicate the entire range has been
processed, e.g. ``len`` is 0, ``gfn_start`` is equal to the last GFN in the
range plus 1, and ``uaddr`` is the last byte of the userspace-provided source
buffer address plus 1. In the case where ``type`` is KVM_SEV_SNP_PAGE_TYPE_ZERO,
``uaddr`` will be ignored completely.
(即使命令成功返回，也不保证已处理完整个请求范围。相反，``struct kvm_sev_snp_launch_update`` 的 ``gfn_start``、``uaddr`` 和 ``len`` 字段会被更新为尚未处理的剩余范围。调用者应当继续调用该命令，直到这些字段表明整个范围都已处理完毕，例如 ``len`` 为 0、``gfn_start`` 等于范围内最后一个 GFN 加 1、``uaddr`` 等于用户态提供的源缓冲区地址最后一个字节加 1。当 ``type`` 为 KVM_SEV_SNP_PAGE_TYPE_ZERO 时，``uaddr`` 会被完全忽略。)

Parameters (in): struct  kvm_sev_snp_launch_update
(参数(输入)：struct kvm_sev_snp_launch_update。)

Returns: 0 on success, < 0 on error, -EAGAIN if caller should retry
(返回：成功为 0，出错为负值，调用者应当重试时为 -EAGAIN。)

::

        struct kvm_sev_snp_launch_update {
                __u64 gfn_start;        /* Guest page number to load/encrypt data into. */
                __u64 uaddr;            /* 4k-aligned address of data to be loaded/encrypted. */
                __u64 len;              /* 4k-aligned length in bytes to copy into guest memory.*/
                __u8 type;              /* The type of the guest pages being initialized. */
                __u8 pad0;
                __u16 flags;            /* Must be zero. */
                __u32 pad1;
                __u64 pad2[4];

        };

where the allowed values for page_type are #define'd as::

        KVM_SEV_SNP_PAGE_TYPE_NORMAL
        KVM_SEV_SNP_PAGE_TYPE_ZERO
        KVM_SEV_SNP_PAGE_TYPE_UNMEASURED
        KVM_SEV_SNP_PAGE_TYPE_SECRETS
        KVM_SEV_SNP_PAGE_TYPE_CPUID

(page_type 的允许取值通过 #define 定义为：)

See the SEV-SNP spec [snp-fw-abi]_ for further details on how each page type is
used/measured.
(每种页类型如何使用/度量的更多细节见 SEV-SNP 规范。)

20. KVM_SEV_SNP_LAUNCH_FINISH(SNP 启动：完成启动流程)
-----------------------------------------------------

After completion of the SNP guest launch flow, the KVM_SEV_SNP_LAUNCH_FINISH
command can be issued to make the guest ready for execution.
(SNP 客户机启动流程完成之后，可以下发 KVM_SEV_SNP_LAUNCH_FINISH 命令，使客户机准备好执行。)

Parameters (in): struct kvm_sev_snp_launch_finish
(参数(输入)：struct kvm_sev_snp_launch_finish。)

Returns: 0 on success, -negative on error
(返回：成功为 0，出错为负错误码。)

::

        struct kvm_sev_snp_launch_finish {
                __u64 id_block_uaddr;
                __u64 id_auth_uaddr;
                __u8 id_block_en;
                __u8 auth_key_en;
                __u8 vcek_disabled;
                __u8 host_data[32];
                __u8 pad0[3];
                __u16 flags;                    /* Must be zero */
                __u64 pad1[4];
        };


See SNP_LAUNCH_FINISH in the SEV-SNP specification [snp-fw-abi]_ for further
details on the input parameters in ``struct kvm_sev_snp_launch_finish``.
(``struct kvm_sev_snp_launch_finish`` 输入参数的更多细节见 SEV-SNP 规范中的 SNP_LAUNCH_FINISH 一节。)

21. KVM_SEV_SNP_ENABLE_REQ_CERTS(SNP：启用证书请求退出)
-------------------------------------------------------

The KVM_SEV_SNP_ENABLE_REQ_CERTS command will configure KVM to exit to
userspace with a ``KVM_EXIT_SNP_REQ_CERTS`` exit type as part of handling
a guest attestation report, which will to allow userspace to provide a
certificate corresponding to the endorsement key used by firmware to sign
that attestation report.
(KVM_SEV_SNP_ENABLE_REQ_CERTS 命令会配置 KVM，使其在处理客户机 attestation 报告时以 ``KVM_EXIT_SNP_REQ_CERTS`` 退出类型退出到用户态，从而允许用户态提供与固件签名该 attestation 报告所用的 endorsement key 相对应的证书。)

Returns: 0 on success, -negative on error
(返回：成功为 0，出错为负错误码。)

NOTE: The endorsement key used by firmware may change as a result of
management activities like updating SEV-SNP firmware or loading new
endorsement keys, so some care should be taken to keep the returned
certificate data in sync with the actual endorsement key in use by
firmware at the time the attestation request is sent to SNP firmware. The
recommended scheme to do this is to use file locking (e.g. via fcntl()'s
F_OFD_SETLK) in the following manner:
(注意：固件使用的 endorsement key 可能会因更新 SEV-SNP 固件或加载新 endorsement key 等管理操作而改变，因此需要小心确保返回的证书数据与向 SNP 固件发送 attestation 请求时固件实际使用的 endorsement key 保持同步。推荐的方案是按如下方式使用文件锁 (例如通过 fcntl() 的 F_OFD_SETLK)：)

  - Prior to obtaining/providing certificate data as part of servicing an
    exit type of ``KVM_EXIT_SNP_REQ_CERTS``, the VMM should obtain a
    shared/read or exclusive/write lock on the certificate blob file before
    reading it and returning it to KVM, and continue to hold the lock until
    the attestation request is actually sent to firmware. To facilitate
    this, the VMM can set the ``immediate_exit`` flag of kvm_run just after
    supplying the certificate data, and just before resuming the vCPU.
    This will ensure the vCPU will exit again to userspace with ``-EINTR``
    after it finishes fetching the attestation request from firmware, at
    which point the VMM can safely drop the file lock.
    (在为 ``KVM_EXIT_SNP_REQ_CERTS`` 退出类型提供服务、获取/提供证书数据之前，VMM 应当先在证书 blob 文件上取得共享/读锁或独占/写锁，然后再读取该文件并返回给 KVM，并持续持锁直到 attestation 请求真正发送给固件。为便于做到这一点，VMM 可以在提供证书数据之后、恢复 vCPU 之前，设置 kvm_run 的 ``immediate_exit`` 标志。这将确保 vCPU 在从固件取回 attestation 请求之后，再次以 ``-EINTR`` 退出到用户态，此时 VMM 便可安全地释放文件锁。)

  - Tools/libraries that perform updates to SNP firmware TCB values or
    endorsement keys (e.g. via /dev/sev interfaces such as ``SNP_COMMIT``,
    ``SNP_SET_CONFIG``, or ``SNP_VLEK_LOAD``, see
    Documentation/virt/coco/sev-guest.rst for more details) in such a way
    that the certificate blob needs to be updated, should similarly take an
    exclusive lock on the certificate blob for the duration of any updates
    to endorsement keys or the certificate blob contents to ensure that
    VMMs using the above scheme will not return certificate blob data that
    is out of sync with the endorsement key used by firmware at the time
    the attestation request is actually issued.
    (对 SNP 固件 TCB 值或 endorsement key 执行更新(例如通过 ``SNP_COMMIT``、``SNP_SET_CONFIG`` 或 ``SNP_VLEK_LOAD`` 等 /dev/sev 接口，详见 Documentation/virt/coco/sev-guest.rst)、因而需要更新证书 blob 的工具/库，同样应当在更新 endorsement key 或证书 blob 内容的整个期间持有证书 blob 上的独占锁，以确保使用上述方案的 VMM 不会返回与实际发出 attestation 请求时固件所用 endorsement key 不同步的证书 blob 数据。)

This scheme is recommended so that tools can use a fairly generic/natural
approach to synchronizing firmware/certificate updates via file-locking,
which should make it easier to maintain interoperability across
tools/VMMs/vendors.
(推荐采用该方案，是为了让工具可以用相当通用/自然的方式通过文件锁来同步固件/证书更新，从而更容易在工具/VMM/厂商之间保持互操作性。)

Device attribute API(设备属性 API)
==================================

Attributes of the SEV implementation can be retrieved through the
``KVM_HAS_DEVICE_ATTR`` and ``KVM_GET_DEVICE_ATTR`` ioctls on the ``/dev/kvm``
device node, using group ``KVM_X86_GRP_SEV``.
(SEV 实现的各项属性可以通过 ``/dev/kvm`` 设备节点上的 ``KVM_HAS_DEVICE_ATTR`` 和 ``KVM_GET_DEVICE_ATTR`` ioctl、使用 ``KVM_X86_GRP_SEV`` 组来获取。)

The following attributes are currently implemented:
(目前实现了以下属性：)

* ``KVM_X86_SEV_VMSA_FEATURES``: return the set of all bits that
  are accepted in the ``vmsa_features`` of ``KVM_SEV_INIT2``.
  (``KVM_X86_SEV_VMSA_FEATURES``：返回 ``KVM_SEV_INIT2`` 的 ``vmsa_features`` 中所有可接受比特的集合。)

* ``KVM_X86_SEV_SNP_REQ_CERTS``: return a value of 1 if the kernel supports the
  ``KVM_EXIT_SNP_REQ_CERTS`` exit, which allows for fetching endorsement key
  certificates from userspace for each SNP attestation request the guest issues.
  (``KVM_X86_SEV_SNP_REQ_CERTS``：如果内核支持 ``KVM_EXIT_SNP_REQ_CERTS`` 退出，则返回 1；该退出允许在客户机每次发出 SNP attestation 请求时从用户态获取 endorsement key 证书。)

Firmware Management(固件管理)
=============================

The SEV guest key management is handled by a separate processor called the AMD
Secure Processor (AMD-SP). Firmware running inside the AMD-SP provides a secure
key management interface to perform common hypervisor activities such as
encrypting bootstrap code, snapshot, migrating and debugging the guest. For more
information, see the SEV Key Management spec [api-spec]_
(SEV 客户机密钥管理由一个名为 AMD Secure Processor (AMD-SP) 的独立处理器负责。AMD-SP 内运行的固件提供安全的密钥管理接口，用于执行常见的 hypervisor 活动，例如加密引导代码、快照、迁移和调试客户机。更多信息见 SEV Key Management 规范。)

The AMD-SP firmware can be initialized either by using its own non-volatile
storage or the OS can manage the NV storage for the firmware using
parameter ``init_ex_path`` of the ``ccp`` module. If the file specified
by ``init_ex_path`` does not exist or is invalid, the OS will create or
override the file with PSP non-volatile storage.
(AMD-SP 固件既可以使用自己的非易失存储来初始化，也可以由操作系统通过 ``ccp`` 模块的 ``init_ex_path`` 参数为固件管理 NV 存储。如果 ``init_ex_path`` 指定的文件不存在或无效，操作系统会创建该文件或用 PSP 非易失存储覆盖它。)

References(参考资料)
====================


See [white-paper]_, [api-spec]_, [amd-apm]_, [kvm-forum]_, and [snp-fw-abi]_
for more info.
(更多信息请参阅上述 white-paper、api-spec、amd-apm、kvm-forum 与 snp-fw-abi 等资料。)

.. [white-paper] https://docs.amd.com/v/u/en-US/memory-encryption-white-paper
.. [api-spec] https://docs.amd.com/v/u/en-US/55766_PUB_3.24_SEV_API
.. [amd-apm] https://docs.amd.com/v/u/en-US/24593_3.44_APM_Vol2 (section 15.34)
.. [kvm-forum]  https://www.linux-kvm.org/images/7/74/02x08A-Thomas_Lendacky-AMDs_Virtualizatoin_Memory_Encryption_Technology.pdf
.. [snp-fw-abi] https://www.amd.com/content/dam/amd/en/documents/developer/56860.pdf
