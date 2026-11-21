.. SPDX-License-Identifier: GPL-2.0

==============
KVM CPUID bits
==============

:Author: Glauber Costa <glommer@gmail.com>

(作者: Glauber Costa <glommer@gmail.com>)

A guest running on a kvm host, can check some of its features using
cpuid. This is not always guaranteed to work, since userspace can
mask-out some, or even all KVM-related cpuid features before launching
a guest.
(运行在 kvm 宿主机上的客户机可以使用 cpuid 检查自己的某些特性。这并不总是保证有效，因为用户空间可以在启动客户机之前屏蔽掉部分甚至全部与 KVM 相关的 cpuid 特性。)

KVM cpuid functions are:
(KVM cpuid 函数有：)

function: KVM_CPUID_SIGNATURE (0x40000000)
(函数：KVM_CPUID_SIGNATURE (0x40000000)。)

returns::

   eax = 0x40000001
   ebx = 0x4b4d564b
   ecx = 0x564b4d56
   edx = 0x4d

(返回：)

Note that this value in ebx, ecx and edx corresponds to the string "KVMKVMKVM".
The value in eax corresponds to the maximum cpuid function present in this leaf,
and will be updated if more functions are added in the future.
Note also that old hosts set eax value to 0x0. This should
be interpreted as if the value was 0x40000001.
This function queries the presence of KVM cpuid leafs.
(注意，ebx、ecx 和 edx 中的这个值对应于字符串 "KVMKVMKVM"。eax 中的值对应于此叶中存在的最大 cpuid 函数，如果将来添加更多函数，它将被更新。另请注意，旧的宿主机把 eax 值设为 0x0。这应被解释为如同该值为 0x40000001。此函数查询 KVM cpuid 叶的存在。)

function: define KVM_CPUID_FEATURES (0x40000001)
(函数：定义 KVM_CPUID_FEATURES (0x40000001)。)

returns::

          ebx, ecx
          eax = an OR'ed group of (1 << flag)

(返回：)

where ``flag`` is defined as below:
(其中 ``flag`` 的定义如下：)

================================== =========== ================================
flag(标志)                         value(值)   meaning(含义)
================================== =========== ================================
KVM_FEATURE_CLOCKSOURCE            0           kvmclock available at msrs
                                               0x11 and 0x12
                                               (kvmclock 可通过 msr 0x11 和 0x12 使用)

KVM_FEATURE_NOP_IO_DELAY           1           not necessary to perform delays
                                               on PIO operations
                                               (无需在 PIO 操作时执行延迟)

KVM_FEATURE_MMU_OP                 2           deprecated
                                               (已弃用)

KVM_FEATURE_CLOCKSOURCE2           3           kvmclock available at msrs
                                               0x4b564d00 and 0x4b564d01
                                               (kvmclock 可通过 msr 0x4b564d00 和 0x4b564d01 使用)

KVM_FEATURE_ASYNC_PF               4           async pf can be enabled by
                                               writing to msr 0x4b564d02
                                               (通过写入 msr 0x4b564d02 可启用 async pf)

KVM_FEATURE_STEAL_TIME             5           steal time can be enabled by
                                               writing to msr 0x4b564d03
                                               (通过写入 msr 0x4b564d03 可启用 steal time)

KVM_FEATURE_PV_EOI                 6           paravirtualized end of interrupt
                                               handler can be enabled by
                                               writing to msr 0x4b564d04
                                               (通过写入 msr 0x4b564d04 可启用半虚拟化的中断结束处理程序)

KVM_FEATURE_PV_UNHALT              7           guest checks this feature bit
                                               before enabling paravirtualized
                                               spinlock support
                                               (客户机在启用半虚拟化自旋锁支持前检查此特性位)

KVM_FEATURE_PV_TLB_FLUSH           9           guest checks this feature bit
                                               before enabling paravirtualized
                                               tlb flush
                                               (客户机在启用半虚拟化 tlb flush 前检查此特性位)

KVM_FEATURE_ASYNC_PF_VMEXIT        10          paravirtualized async PF VM EXIT
                                               can be enabled by setting bit 2
                                               when writing to msr 0x4b564d02
                                               (在写入 msr 0x4b564d02 时设置位 2 可启用半虚拟化的 async PF VM EXIT)

KVM_FEATURE_PV_SEND_IPI            11          guest checks this feature bit
                                               before enabling paravirtualized
                                               send IPIs
                                               (客户机在启用半虚拟化 IPI 发送前检查此特性位)

KVM_FEATURE_POLL_CONTROL           12          host-side polling on HLT can
                                               be disabled by writing
                                               to msr 0x4b564d05.
                                               (通过写入 msr 0x4b564d05 可以禁用主机侧对 HLT 的轮询。)

KVM_FEATURE_PV_SCHED_YIELD         13          guest checks this feature bit
                                               before using paravirtualized
                                               sched yield.
                                               (客户机在使用半虚拟化 sched yield 前检查此特性位。)

KVM_FEATURE_ASYNC_PF_INT           14          guest checks this feature bit
                                               before using the second async
                                               pf control msr 0x4b564d06 and
                                               async pf acknowledgment msr
                                               0x4b564d07.
                                               (客户机在使用第二个 async pf 控制 msr 0x4b564d06 和 async pf 确认 msr 0x4b564d07 前检查此特性位。)

KVM_FEATURE_MSI_EXT_DEST_ID        15          guest checks this feature bit
                                               before using extended destination
                                               ID bits in MSI address bits 11-5.
                                               (客户机在使用 MSI 地址位 11-5 中的扩展目的 ID 位前检查此特性位。)

KVM_FEATURE_HC_MAP_GPA_RANGE       16          guest checks this feature bit before
                                               using the map gpa range hypercall
                                               to notify the page state change
                                               (客户机在使用 map gpa range 超级调用通知页状态更改前检查此特性位)

KVM_FEATURE_MIGRATION_CONTROL      17          guest checks this feature bit before
                                               using MSR_KVM_MIGRATION_CONTROL
                                               (客户机在使用 MSR_KVM_MIGRATION_CONTROL 前检查此特性位)

KVM_FEATURE_CLOCKSOURCE_STABLE_BIT 24          host will warn if no guest-side
                                               per-cpu warps are expected in
                                               kvmclock
                                               (如果 kvmclock 中预期没有客户机侧的 per-cpu 偏差，主机将发出警告)
================================== =========== ================================

::

      edx = an OR'ed group of (1 << flag)

Where ``flag`` here is defined as below:
(其中 ``flag`` 在此定义如下：)

================== ============ =================================
flag(标志)         value(值)    meaning(含义)
================== ============ =================================
KVM_HINTS_REALTIME 0            guest checks this feature bit to
                                determine that vCPUs are never
                                preempted for an unlimited time
                                allowing optimizations
                                (客户机检查此特性位以确定 vCPU 永远不会在无限制的时间内被抢占，从而允许进行优化)
================== ============ =================================
