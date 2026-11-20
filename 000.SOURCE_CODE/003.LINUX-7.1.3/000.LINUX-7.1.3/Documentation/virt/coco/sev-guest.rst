.. SPDX-License-Identifier: GPL-2.0

======================================================================
The Definitive SEV Guest API Documentation(权威的 SEV 客户机 API 文档)
======================================================================

1. General description(1. 总体描述)
===================================

The SEV API is a set of ioctls that are used by the guest or hypervisor
to get or set a certain aspect of the SEV virtual machine. The ioctls belong
to the following classes:
(SEV API 是一组 ioctl，供客户机或 hypervisor 获取或设置 SEV 虚拟机的某个方面。这些 ioctl 属于以下几类：)

 - Hypervisor ioctls: These query and set global attributes which affect the
   whole SEV firmware.  These ioctl are used by platform provisioning tools.
   (Hypervisor ioctl：查询并设置影响整个 SEV 固件的全局属性。这类 ioctl 由平台供应工具使用。)

 - Guest ioctls: These query and set attributes of the SEV virtual machine.
   (客户机 ioctl：查询并设置 SEV 虚拟机的属性。)

2. API description(2. API 描述)
===============================

This section describes ioctls that is used for querying the SEV guest report
from the SEV firmware. For each ioctl, the following information is provided
along with a description:
(本节描述用于从 SEV 固件查询 SEV 客户机报告的 ioctl。对每个 ioctl，除描述外还提供以下信息：)

  Technology:
      which SEV technology provides this ioctl. SEV, SEV-ES, SEV-SNP or all.
      (技术：提供该 ioctl 的 SEV 技术。SEV、SEV-ES、SEV-SNP 或全部。)

  Type:
      hypervisor or guest. The ioctl can be used inside the guest or the
      hypervisor.
      (类型：hypervisor 或 guest。该 ioctl 可在客户机或 hypervisor 内部使用。)

  Parameters:
      what parameters are accepted by the ioctl.
      (参数：该 ioctl 接受哪些参数。)

  Returns:
      the return value.  General error numbers (-ENOMEM, -EINVAL)
      are not detailed, but errors with specific meanings are.
      (返回：返回值。通用错误编号 (-ENOMEM、-EINVAL) 不赘述，但具有特定含义的错误会说明。)

The guest ioctl should be issued on a file descriptor of the /dev/sev-guest
device.  The ioctl accepts struct snp_user_guest_request. The input and
output structure is specified through the req_data and resp_data field
respectively. If the ioctl fails to execute due to a firmware error, then
the fw_error code will be set, otherwise fw_error will be set to -1.
(客户机 ioctl 应对 /dev/sev-guest 设备的文件描述符发出。该 ioctl 接受 struct snp_user_guest_request。输入与输出结构分别通过 req_data 与 resp_data 字段指定。若 ioctl 因固件错误而执行失败，则会设置 fw_error 错误码，否则 fw_error 会被置为 -1。)

The firmware checks that the message sequence counter is one greater than
the guests message sequence counter. If guest driver fails to increment message
counter (e.g. counter overflow), then -EIO will be returned.
(固件检查消息序列计数器是否比客户机的消息序列计数器大 1。如果客户机驱动递增消息计数器失败 (例如计数器溢出)，则返回 -EIO。)

::

        struct snp_guest_request_ioctl {
                /* Message version number */
                __u32 msg_version;

                /* Request and response structure address */
                __u64 req_data;
                __u64 resp_data;

                /* bits[63:32]: VMM error code, bits[31:0] firmware error code (see psp-sev.h) */
                union {
                        __u64 exitinfo2;
                        struct {
                                __u32 fw_error;
                                __u32 vmm_error;
                        };
                };
        };

The host ioctls are issued to a file descriptor of the /dev/sev device.
The ioctl accepts the command ID/input structure documented below.
(宿主机 ioctl 对 /dev/sev 设备的文件描述符发出。该 ioctl 接受下文描述的命令 ID/输入结构。)

::

        struct sev_issue_cmd {
                /* Command ID */
                __u32 cmd;

                /* Command request structure */
                __u64 data;

                /* Firmware error code on failure (see psp-sev.h) */
                __u32 error;
        };


2.1 SNP_GET_REPORT
------------------

:Technology: sev-snp(技术：sev-snp)
:Type: guest ioctl(类型：客户机 ioctl)
:Parameters (in): struct snp_report_req(参数 (输入)：struct snp_report_req)
:Returns (out): struct snp_report_resp on success, -negative on error(返回 (输出)：成功时为 struct snp_report_resp，出错时为负值)

The SNP_GET_REPORT ioctl can be used to query the attestation report from the
SEV-SNP firmware. The ioctl uses the SNP_GUEST_REQUEST (MSG_REPORT_REQ) command
provided by the SEV-SNP firmware to query the attestation report.
(SNP_GET_REPORT ioctl 可用于从 SEV-SNP 固件查询 attestation 报告。该 ioctl 使用 SEV-SNP 固件提供的 SNP_GUEST_REQUEST (MSG_REPORT_REQ) 命令来查询 attestation 报告。)

On success, the snp_report_resp.data will contains the report. The report
contain the format described in the SEV-SNP specification. See the SEV-SNP
specification for further details.
(成功时，snp_report_resp.data 将包含报告。报告采用 SEV-SNP 规范中描述的格式。更多细节见 SEV-SNP 规范。)

2.2 SNP_GET_DERIVED_KEY
-----------------------
:Technology: sev-snp(技术：sev-snp)
:Type: guest ioctl(类型：客户机 ioctl)
:Parameters (in): struct snp_derived_key_req(参数 (输入)：struct snp_derived_key_req)
:Returns (out): struct snp_derived_key_resp on success, -negative on error(返回 (输出)：成功时为 struct snp_derived_key_resp，出错时为负值)

The SNP_GET_DERIVED_KEY ioctl can be used to get a key derive from a root key.
The derived key can be used by the guest for any purpose, such as sealing keys
or communicating with external entities.
(SNP_GET_DERIVED_KEY ioctl 可用于获取从一个根密钥派生的密钥。派生密钥可供客户机用于任何目的，例如密封密钥 (sealing keys) 或与外部实体通信。)

The ioctl uses the SNP_GUEST_REQUEST (MSG_KEY_REQ) command provided by the
SEV-SNP firmware to derive the key. See SEV-SNP specification for further details
on the various fields passed in the key derivation request.
(该 ioctl 使用 SEV-SNP 固件提供的 SNP_GUEST_REQUEST (MSG_KEY_REQ) 命令来派生密钥。密钥派生请求中传入的各个字段的更多细节见 SEV-SNP 规范。)

On success, the snp_derived_key_resp.data contains the derived key value. See
the SEV-SNP specification for further details.
(成功时，snp_derived_key_resp.data 包含派生的密钥值。更多细节见 SEV-SNP 规范。)


2.3 SNP_GET_EXT_REPORT
----------------------
:Technology: sev-snp(技术：sev-snp)
:Type: guest ioctl(类型：客户机 ioctl)
:Parameters (in/out): struct snp_ext_report_req(参数 (输入/输出)：struct snp_ext_report_req)
:Returns (out): struct snp_report_resp on success, -negative on error(返回 (输出)：成功时为 struct snp_report_resp，出错时为负值)

The SNP_GET_EXT_REPORT ioctl is similar to the SNP_GET_REPORT. The difference is
related to the additional certificate data that is returned with the report.
The certificate data returned is being provided by the hypervisor through the
SNP_SET_EXT_CONFIG.
(SNP_GET_EXT_REPORT ioctl 与 SNP_GET_REPORT 类似。区别在于随报告一起返回的额外证书数据。返回的证书数据由 hypervisor 通过 SNP_SET_EXT_CONFIG 提供。)

The ioctl uses the SNP_GUEST_REQUEST (MSG_REPORT_REQ) command provided by the SEV-SNP
firmware to get the attestation report.
(该 ioctl 使用 SEV-SNP 固件提供的 SNP_GUEST_REQUEST (MSG_REPORT_REQ) 命令获取 attestation 报告。)

On success, the snp_ext_report_resp.data will contain the attestation report
and snp_ext_report_req.certs_address will contain the certificate blob. If the
length of the blob is smaller than expected then snp_ext_report_req.certs_len will
be updated with the expected value.
(成功时，snp_ext_report_resp.data 将包含 attestation 报告，snp_ext_report_req.certs_address 将包含证书 blob。如果 blob 的长度小于预期，snp_ext_report_req.certs_len 将被更新为期望值。)

See GHCB specification for further detail on how to parse the certificate blob.
(如何解析证书 blob 的更多细节见 GHCB 规范。)

2.4 SNP_PLATFORM_STATUS
-----------------------
:Technology: sev-snp(技术：sev-snp)
:Type: hypervisor ioctl cmd(类型：hypervisor ioctl 命令)
:Parameters (out): struct sev_user_data_snp_status(参数 (输出)：struct sev_user_data_snp_status)
:Returns (out): 0 on success, -negative on error(返回 (输出)：成功时为 0，出错时为负值)

The SNP_PLATFORM_STATUS command is used to query the SNP platform status. The
status includes API major, minor version and more. See the SEV-SNP
specification for further details.
(SNP_PLATFORM_STATUS 命令用于查询 SNP 平台状态。状态包括 API 主/次版本等。更多细节见 SEV-SNP 规范。)

2.5 SNP_COMMIT
--------------
:Technology: sev-snp(技术：sev-snp)
:Type: hypervisor ioctl cmd(类型：hypervisor ioctl 命令)
:Returns (out): 0 on success, -negative on error(返回 (输出)：成功时为 0，出错时为负值)

SNP_COMMIT is used to commit the currently installed firmware using the
SEV-SNP firmware SNP_COMMIT command. This prevents roll-back to a previously
committed firmware version. This will also update the reported TCB to match
that of the currently installed firmware.
(SNP_COMMIT 用于通过 SEV-SNP 固件的 SNP_COMMIT 命令提交当前安装的固件。这可以防止回滚到之前提交的固件版本。这还会更新报告的 TCB 使其与当前安装的固件一致。)

2.6 SNP_SET_CONFIG
------------------
:Technology: sev-snp(技术：sev-snp)
:Type: hypervisor ioctl cmd(类型：hypervisor ioctl 命令)
:Parameters (in): struct sev_user_data_snp_config(参数 (输入)：struct sev_user_data_snp_config)
:Returns (out): 0 on success, -negative on error(返回 (输出)：成功时为 0，出错时为负值)

SNP_SET_CONFIG is used to set the system-wide configuration such as
reported TCB version in the attestation report. The command is similar
to SNP_CONFIG command defined in the SEV-SNP spec. The current values of
the firmware parameters affected by this command can be queried via
SNP_PLATFORM_STATUS.
(SNP_SET_CONFIG 用于设置系统级配置，例如 attestation 报告中报告的 TCB 版本。该命令与 SEV-SNP 规范中定义的 SNP_CONFIG 命令类似。受该命令影响的固件参数的当前值可以通过 SNP_PLATFORM_STATUS 查询。)

2.7 SNP_VLEK_LOAD
-----------------
:Technology: sev-snp(技术：sev-snp)
:Type: hypervisor ioctl cmd(类型：hypervisor ioctl 命令)
:Parameters (in): struct sev_user_data_snp_vlek_load(参数 (输入)：struct sev_user_data_snp_vlek_load)
:Returns (out): 0 on success, -negative on error(返回 (输出)：成功时为 0，出错时为负值)

When requesting an attestation report a guest is able to specify whether
it wants SNP firmware to sign the report using either a Versioned Chip
Endorsement Key (VCEK), which is derived from chip-unique secrets, or a
Versioned Loaded Endorsement Key (VLEK) which is obtained from an AMD
Key Derivation Service (KDS) and derived from seeds allocated to
enrolled cloud service providers.
(请求 attestation 报告时，客户机可以指定让 SNP 固件使用哪种密钥签署报告：版本化芯片背书密钥 (Versioned Chip Endorsement Key, VCEK)，由芯片专属秘密派生；或版本化加载背书密钥 (Versioned Loaded Endorsement Key, VLEK)，从 AMD 密钥派生服务 (Key Derivation Service, KDS) 获取，由分配给已注册云服务提供商的种子派生。)

In the case of VLEK keys, the SNP_VLEK_LOAD SNP command is used to load
them into the system after obtaining them from the KDS, and corresponds
closely to the SNP_VLEK_LOAD firmware command specified in the SEV-SNP
spec.
(对于 VLEK 密钥，从 KDS 获取后使用 SNP_VLEK_LOAD SNP 命令将其加载进系统；它与 SEV-SNP 规范中指定的 SNP_VLEK_LOAD 固件命令紧密对应。)

3. SEV-SNP CPUID Enforcement(3. SEV-SNP CPUID 强制措施)
========================================================

SEV-SNP guests can access a special page that contains a table of CPUID values
that have been validated by the PSP as part of the SNP_LAUNCH_UPDATE firmware
command. It provides the following assurances regarding the validity of CPUID
values:
(SEV-SNP 客户机可以访问一个特殊页面，其中包含一张 CPUID 值表，这些值已由 PSP 作为 SNP_LAUNCH_UPDATE 固件命令的一部分进行过验证。关于 CPUID 值的有效性，它提供以下保证：)

 - Its address is obtained via bootloader/firmware (via CC blob), and those
   binaries will be measured as part of the SEV-SNP attestation report.
   (其地址通过 bootloader/固件获取 (经由 CC blob)，这些二进制文件会作为 SEV-SNP attestation 报告的一部分被度量。)
 - Its initial state will be encrypted/pvalidated, so attempts to modify
   it during run-time will result in garbage being written, or #VC exceptions
   being generated due to changes in validation state if the hypervisor tries
   to swap the backing page.
   (其初始状态会被加密/pvalidated，因此在运行时修改它的尝试会导致写入垃圾数据；如果 hypervisor 试图换掉后备页，则会因验证状态变化而产生 #VC 异常。)
 - Attempts to bypass PSP checks by the hypervisor by using a normal page, or
   a non-CPUID encrypted page will change the measurement provided by the
   SEV-SNP attestation report.
   (hypervisor 若试图用普通页或非 CPUID 加密页绕过 PSP 检查，会改变 SEV-SNP attestation 报告提供的度量值。)
 - The CPUID page contents are *not* measured, but attempts to modify the
   expected contents of a CPUID page as part of guest initialization will be
   gated by the PSP CPUID enforcement policy checks performed on the page
   during SNP_LAUNCH_UPDATE, and noticeable later if the guest owner
   implements their own checks of the CPUID values.
   (CPUID 页的内容 *不会* 被度量，但在客户机初始化过程中修改 CPUID 页预期内容的尝试，会被 PSP 在 SNP_LAUNCH_UPDATE 期间对该页执行的 CPUID 强制策略检查所拦截；如果客户机所有者自己实现了对 CPUID 值的检查，之后也会被发现。)

It is important to note that this last assurance is only useful if the kernel
has taken care to make use of the SEV-SNP CPUID throughout all stages of boot.
Otherwise, guest owner attestation provides no assurance that the kernel wasn't
fed incorrect values at some point during boot.
(需要注意的是，最后这条保证只有在内核做到在引导的所有阶段都使用 SEV-SNP CPUID 时才有意义。否则，客户机所有者的 attestation 无法保证内核在引导期间的某个时刻没有被喂入错误值。)

4. SEV Guest Driver Communication Key(4. SEV 客户机驱动通信密钥)
================================================================

Communication between an SEV guest and the SEV firmware in the AMD Secure
Processor (ASP, aka PSP) is protected by a VM Platform Communication Key
(VMPCK). By default, the sev-guest driver uses the VMPCK associated with the
VM Privilege Level (VMPL) at which the guest is running. Should this key be
wiped by the sev-guest driver (see the driver for reasons why a VMPCK can be
wiped), a different key can be used by reloading the sev-guest driver and
specifying the desired key using the vmpck_id module parameter.
(SEV 客户机与 AMD 安全处理器 (ASP，又称 PSP) 中 SEV 固件之间的通信由 VM 平台通信密钥 (VM Platform Communication Key, VMPCK) 保护。默认情况下，sev-guest 驱动使用与客户机运行所在的 VM 特权级 (VM Privilege Level, VMPL) 相关联的 VMPCK。如果该密钥被 sev-guest 驱动清除 (VMPCK 为何会被清除见驱动代码)，可以通过重新加载 sev-guest 驱动并用 vmpck_id 模块参数指定所需密钥来改用其他密钥。)


Reference(参考资料)
--------------------

SEV-SNP and GHCB specification: developer.amd.com/sev
(SEV-SNP 与 GHCB 规范：developer.amd.com/sev)

The driver is based on SEV-SNP firmware spec 0.9 and GHCB spec version 2.0.
(该驱动基于 SEV-SNP 固件规范 0.9 与 GHCB 规范 2.0 版。)
