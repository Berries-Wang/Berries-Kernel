.. SPDX-License-Identifier: GPL-2.0

================================
ACRN CPUID bits(ACRN CPUID 位域)
================================

A guest VM running on an ACRN hypervisor can check some of its features using
CPUID.
(运行在 ACRN hypervisor 上的客户 VM 可以使用 CPUID 检查其部分特性。)

ACRN cpuid functions are:
(ACRN 的 cpuid 函数如下：)

function: 0x40000000
(函数：0x40000000。)

returns::

   eax = 0x40000010
   ebx = 0x4e524341
   ecx = 0x4e524341
   edx = 0x4e524341

(返回：)

Note that this value in ebx, ecx and edx corresponds to the string
"ACRNACRNACRN". The value in eax corresponds to the maximum cpuid function
present in this leaf, and will be updated if more functions are added in the
future.
(注意，ebx、ecx 和 edx 中的该值对应字符串 "ACRNACRNACRN"。eax 中的值对应此
叶中存在的最大 cpuid 函数，将来若加入更多函数，该值将被更新。)

function: define ACRN_CPUID_FEATURES (0x40000001)
(函数：define ACRN_CPUID_FEATURES (0x40000001)。)

returns::

          ebx, ecx, edx
          eax = an OR'ed group of (1 << flag)

(返回：)

where ``flag`` is defined as below:
(其中 ``flag`` 的定义如下：)

================================= =========== ================================
flag(标志)                        value(值)   meaning(含义)
================================= =========== ================================
ACRN_FEATURE_PRIVILEGED_VM        0           guest VM is a privileged VM(客户 VM 是一个有特权的 VM)
================================= =========== ================================

function: 0x40000010
(函数：0x40000010。)

returns::

          ebx, ecx, edx
          eax = (Virtual) TSC frequency in kHz.

(返回：)
