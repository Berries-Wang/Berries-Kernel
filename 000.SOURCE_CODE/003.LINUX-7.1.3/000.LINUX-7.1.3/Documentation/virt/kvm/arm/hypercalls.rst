.. SPDX-License-Identifier: GPL-2.0

====================================================================================
KVM/arm64-specific hypercalls exposed to guests(暴露给客户机的 KVM/arm64 专有超调用)
====================================================================================

This file documents the KVM/arm64-specific hypercalls which may be
exposed by KVM/arm64 to guest operating systems. These hypercalls are
issued using the HVC instruction according to version 1.1 of the Arm SMC
Calling Convention (DEN0028/C):
(本文档记录 KVM/arm64 可能向客户机操作系统暴露的 KVM/arm64 专有超调用。这些超调用按照 Arm SMC 调用约定(DEN0028/C)1.1 版，使用 HVC 指令发起：)

https://developer.arm.com/docs/den0028/c

All KVM/arm64-specific hypercalls are allocated within the "Vendor
Specific Hypervisor Service Call" range with a UID of
``28b46fb6-2ec5-11e9-a9ca-4b564d003a74``. This UID should be queried by the
guest using the standard "Call UID" function for the service range in
order to determine that the KVM/arm64-specific hypercalls are available.
(所有 KVM/arm64 专有超调用都分配在"厂商特定 Hypervisor 服务调用"范围内，UID 为 ``28b46fb6-2ec5-11e9-a9ca-4b564d003a74`` 。客户机应使用该服务范围标准的 "Call UID" 功能查询此 UID，以确定 KVM/arm64 专有超调用是否可用。)

``ARM_SMCCC_VENDOR_HYP_KVM_FEATURES_FUNC_ID`` (原名)
----------------------------------------------------

Provides a discovery mechanism for other KVM/arm64 hypercalls.
(为其他 KVM/arm64 超调用提供发现机制。)

+---------------------+-------------------------------------------------------------+
| Presence:           | Mandatory for the KVM/arm64 UID                             |
|                     | (对 KVM/arm64 UID 而言是强制性的)                           |
+---------------------+-------------------------------------------------------------+
| Calling convention: | HVC32                                                       |
+---------------------+----------+--------------------------------------------------+
| Function ID:        | (uint32) | 0x86000000                                       |
+---------------------+----------+--------------------------------------------------+
| Arguments:          | None                                                        |
|                     | (无)                                                        |
+---------------------+----------+----+---------------------------------------------+
| Return Values:      | (uint32) | R0 | Bitmap of available function numbers 0-31   |
|                     |          |    | (可用功能编号 0-31 的位图)                  |
|                     +----------+----+---------------------------------------------+
|                     | (uint32) | R1 | Bitmap of available function numbers 32-63  |
|                     |          |    | (可用功能编号 32-63 的位图)                 |
|                     +----------+----+---------------------------------------------+
|                     | (uint32) | R2 | Bitmap of available function numbers 64-95  |
|                     |          |    | (可用功能编号 64-95 的位图)                 |
|                     +----------+----+---------------------------------------------+
|                     | (uint32) | R3 | Bitmap of available function numbers 96-127 |
|                     |          |    | (可用功能编号 96-127 的位图)                |
+---------------------+----------+----+---------------------------------------------+

``ARM_SMCCC_VENDOR_HYP_KVM_PTP_FUNC_ID`` (原名)
-----------------------------------------------

See ptp_kvm.rst
(参见 ptp_kvm.rst)

``ARM_SMCCC_KVM_FUNC_HYP_MEMINFO`` (原名)
-----------------------------------------

Query the memory protection parameters for a pKVM protected virtual machine.
(查询 pKVM 受保护虚拟机的内存保护参数。)

+---------------------+-------------------------------------------------------------+
| Presence:           | Optional; pKVM protected guests only.                       |
|                     | (可选；仅限 pKVM 受保护客户机。)                            |
+---------------------+-------------------------------------------------------------+
| Calling convention: | HVC64                                                       |
+---------------------+----------+--------------------------------------------------+
| Function ID:        | (uint32) | 0xC6000002                                       |
+---------------------+----------+----+---------------------------------------------+
| Arguments:          | (uint64) | R1 | Reserved / Must be zero                     |
|                     |          |    | (保留 / 必须为零)                           |
|                     +----------+----+---------------------------------------------+
|                     | (uint64) | R2 | Reserved / Must be zero                     |
|                     |          |    | (保留 / 必须为零)                           |
|                     +----------+----+---------------------------------------------+
|                     | (uint64) | R3 | Reserved / Must be zero                     |
|                     |          |    | (保留 / 必须为零)                           |
+---------------------+----------+----+---------------------------------------------+
| Return Values:      | (int64)  | R0 | ``INVALID_PARAMETER (-3)`` on error, else   |
|                     |          |    | memory protection granule in bytes          |
|                     |          |    | (出错时为 ``INVALID_PARAMETER (-3)`` ，)    |
|                     |          |    | (否则为以字节为单位的内存保护粒度)          |
+---------------------+----------+----+---------------------------------------------+

``ARM_SMCCC_KVM_FUNC_MEM_SHARE`` (原名)
---------------------------------------

Share a region of memory with the KVM host, granting it read, write and execute
permissions. The size of the region is equal to the memory protection granule
advertised by ``ARM_SMCCC_KVM_FUNC_HYP_MEMINFO``.
(与 KVM 宿主共享一块内存区域，授予其读、写与执行权限。区域大小等于 ``ARM_SMCCC_KVM_FUNC_HYP_MEMINFO`` 所通告的内存保护粒度。)

+---------------------+-------------------------------------------------------------+
| Presence:           | Optional; pKVM protected guests only.                       |
|                     | (可选；仅限 pKVM 受保护客户机。)                            |
+---------------------+-------------------------------------------------------------+
| Calling convention: | HVC64                                                       |
+---------------------+----------+--------------------------------------------------+
| Function ID:        | (uint32) | 0xC6000003                                       |
+---------------------+----------+----+---------------------------------------------+
| Arguments:          | (uint64) | R1 | Base IPA of memory region to share          |
|                     |          |    | (要共享的内存区域的基地址 IPA)              |
|                     +----------+----+---------------------------------------------+
|                     | (uint64) | R2 | Reserved / Must be zero                     |
|                     |          |    | (保留 / 必须为零)                           |
|                     +----------+----+---------------------------------------------+
|                     | (uint64) | R3 | Reserved / Must be zero                     |
|                     |          |    | (保留 / 必须为零)                           |
+---------------------+----------+----+---------------------------------------------+
| Return Values:      | (int64)  | R0 | ``SUCCESS (0)``                             |
|                     |          |    +---------------------------------------------+
|                     |          |    | ``INVALID_PARAMETER (-3)``                  |
+---------------------+----------+----+---------------------------------------------+

``ARM_SMCCC_KVM_FUNC_MEM_UNSHARE`` (原名)
-----------------------------------------

Revoke access permission from the KVM host to a memory region previously shared
with ``ARM_SMCCC_KVM_FUNC_MEM_SHARE``. The size of the region is equal to the
memory protection granule advertised by ``ARM_SMCCC_KVM_FUNC_HYP_MEMINFO``.
(撤销 KVM 宿主对先前通过 ``ARM_SMCCC_KVM_FUNC_MEM_SHARE`` 共享的内存区域的访问权限。区域大小等于 ``ARM_SMCCC_KVM_FUNC_HYP_MEMINFO`` 所通告的内存保护粒度。)

+---------------------+-------------------------------------------------------------+
| Presence:           | Optional; pKVM protected guests only.                       |
|                     | (可选；仅限 pKVM 受保护客户机。)                            |
+---------------------+-------------------------------------------------------------+
| Calling convention: | HVC64                                                       |
+---------------------+----------+--------------------------------------------------+
| Function ID:        | (uint32) | 0xC6000004                                       |
+---------------------+----------+----+---------------------------------------------+
| Arguments:          | (uint64) | R1 | Base IPA of memory region to unshare        |
|                     |          |    | (要取消共享的内存区域的基地址 IPA)          |
|                     +----------+----+---------------------------------------------+
|                     | (uint64) | R2 | Reserved / Must be zero                     |
|                     |          |    | (保留 / 必须为零)                           |
|                     +----------+----+---------------------------------------------+
|                     | (uint64) | R3 | Reserved / Must be zero                     |
|                     |          |    | (保留 / 必须为零)                           |
+---------------------+----------+----+---------------------------------------------+
| Return Values:      | (int64)  | R0 | ``SUCCESS (0)``                             |
|                     |          |    +---------------------------------------------+
|                     |          |    | ``INVALID_PARAMETER (-3)``                  |
+---------------------+----------+----+---------------------------------------------+

``ARM_SMCCC_KVM_FUNC_MMIO_GUARD`` (原名)
----------------------------------------

Request that a given memory region is handled as MMIO by the hypervisor,
allowing accesses to this region to be emulated by the KVM host. The size of the
region is equal to the memory protection granule advertised by
``ARM_SMCCC_KVM_FUNC_HYP_MEMINFO``.
(请求 hypervisor 把给定的内存区域按 MMIO 处理，使 KVM 宿主可以模拟对该区域的访问。区域大小等于 ``ARM_SMCCC_KVM_FUNC_HYP_MEMINFO`` 所通告的内存保护粒度。)

+---------------------+-------------------------------------------------------------+
| Presence:           | Optional; pKVM protected guests only.                       |
|                     | (可选；仅限 pKVM 受保护客户机。)                            |
+---------------------+-------------------------------------------------------------+
| Calling convention: | HVC64                                                       |
+---------------------+----------+--------------------------------------------------+
| Function ID:        | (uint32) | 0xC6000007                                       |
+---------------------+----------+----+---------------------------------------------+
| Arguments:          | (uint64) | R1 | Base IPA of MMIO memory region              |
|                     |          |    | (MMIO 内存区域的基地址 IPA)                 |
|                     +----------+----+---------------------------------------------+
|                     | (uint64) | R2 | Reserved / Must be zero                     |
|                     |          |    | (保留 / 必须为零)                           |
|                     +----------+----+---------------------------------------------+
|                     | (uint64) | R3 | Reserved / Must be zero                     |
|                     |          |    | (保留 / 必须为零)                           |
+---------------------+----------+----+---------------------------------------------+
| Return Values:      | (int64)  | R0 | ``SUCCESS (0)``                             |
|                     |          |    +---------------------------------------------+
|                     |          |    | ``INVALID_PARAMETER (-3)``                  |
+---------------------+----------+----+---------------------------------------------+

``ARM_SMCCC_VENDOR_HYP_KVM_DISCOVER_IMPL_VER_FUNC_ID`` (原名)
-------------------------------------------------------------
Request the target CPU implementation version information and the number of target
implementations for the Guest VM.
(请求客户机 VM 的目标 CPU 实现版本信息以及目标实现的数量。)

+---------------------+-------------------------------------------------------------+
| Presence:           | Optional;  KVM/ARM64 Guests only                            |
|                     | (可选；仅限 KVM/ARM64 客户机)                               |
+---------------------+-------------------------------------------------------------+
| Calling convention: | HVC64                                                       |
+---------------------+----------+--------------------------------------------------+
| Function ID:        | (uint32) | 0xC6000040                                       |
+---------------------+----------+--------------------------------------------------+
| Arguments:          | None                                                        |
|                     | (无)                                                        |
+---------------------+----------+----+---------------------------------------------+
| Return Values:      | (int64)  | R0 | ``SUCCESS (0)``                             |
|                     |          |    +---------------------------------------------+
|                     |          |    | ``NOT_SUPPORTED (-1)``                      |
|                     +----------+----+---------------------------------------------+
|                     | (uint64) | R1 | Bits [63:32] Reserved/Must be zero          |
|                     |          |    | (位 [63:32] 保留/必须为零)                  |
|                     |          |    +---------------------------------------------+
|                     |          |    | Bits [31:16] Major version                  |
|                     |          |    | (位 [31:16] 主版本号)                       |
|                     |          |    +---------------------------------------------+
|                     |          |    | Bits [15:0] Minor version                   |
|                     |          |    | (位 [15:0] 次版本号)                        |
|                     +----------+----+---------------------------------------------+
|                     | (uint64) | R2 | Number of target implementations            |
|                     |          |    | (目标实现的数量)                            |
|                     +----------+----+---------------------------------------------+
|                     | (uint64) | R3 | Reserved / Must be zero                     |
|                     |          |    | (保留 / 必须为零)                           |
+---------------------+----------+----+---------------------------------------------+

``ARM_SMCCC_VENDOR_HYP_KVM_DISCOVER_IMPL_CPUS_FUNC_ID`` (原名)
--------------------------------------------------------------

Request the target CPU implementation information for the Guest VM. The Guest kernel
will use this information to enable the associated errata.
(请求客户机 VM 的目标 CPU 实现信息。客户机内核将使用这些信息来启用相应的勘误(errata)。)

+---------------------+-------------------------------------------------------------+
| Presence:           | Optional;  KVM/ARM64 Guests only                            |
|                     | (可选；仅限 KVM/ARM64 客户机)                               |
+---------------------+-------------------------------------------------------------+
| Calling convention: | HVC64                                                       |
+---------------------+----------+--------------------------------------------------+
| Function ID:        | (uint32) | 0xC6000041                                       |
+---------------------+----------+----+---------------------------------------------+
| Arguments:          | (uint64) | R1 | selected implementation index               |
|                     |          |    | (所选实现的索引)                            |
|                     +----------+----+---------------------------------------------+
|                     | (uint64) | R2 | Reserved / Must be zero                     |
|                     |          |    | (保留 / 必须为零)                           |
|                     +----------+----+---------------------------------------------+
|                     | (uint64) | R3 | Reserved / Must be zero                     |
|                     |          |    | (保留 / 必须为零)                           |
+---------------------+----------+----+---------------------------------------------+
| Return Values:      | (int64)  | R0 | ``SUCCESS (0)``                             |
|                     |          |    +---------------------------------------------+
|                     |          |    | ``INVALID_PARAMETER (-3)``                  |
|                     +----------+----+---------------------------------------------+
|                     | (uint64) | R1 | MIDR_EL1 of the selected implementation     |
|                     |          |    | (所选实现的 MIDR_EL1)                       |
|                     +----------+----+---------------------------------------------+
|                     | (uint64) | R2 | REVIDR_EL1 of the selected implementation   |
|                     |          |    | (所选实现的 REVIDR_EL1)                     |
|                     +----------+----+---------------------------------------------+
|                     | (uint64) | R3 | AIDR_EL1  of the selected implementation    |
|                     |          |    | (所选实现的 AIDR_EL1)                       |
+---------------------+----------+----+---------------------------------------------+
