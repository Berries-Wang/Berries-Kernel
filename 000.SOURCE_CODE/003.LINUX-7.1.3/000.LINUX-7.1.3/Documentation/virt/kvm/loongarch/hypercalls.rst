.. SPDX-License-Identifier: GPL-2.0

===========================================================
The LoongArch paravirtual interface(LoongArch 半虚拟化接口)
===========================================================

KVM hypercalls use the HVCL instruction with code 0x100 and the hypercall
number is put in a0. Up to five arguments may be placed in registers a1 - a5.
The return value is placed in v0 (an alias of a0).
(KVM 超级调用(hypercall)使用码为 0x100 的 HVCL 指令，超级调用编号
放在 a0 中。最多可以把五个参数放在寄存器 a1 - a5 中。返回值放在
v0(a0 的别名)中。)

Source code for this interface can be found in arch/loongarch/kvm*.
(该接口的源代码可以在 arch/loongarch/kvm* 中找到。)

Querying for existence(查询是否存在)
====================================

To determine if the host is running on KVM, we can utilize the cpucfg()
function at index CPUCFG_KVM_BASE (0x40000000).
(要确定主机是否运行在 KVM 上，我们可以使用索引为 CPUCFG_KVM_BASE
(0x40000000) 的 cpucfg() 函数。)

The CPUCFG_KVM_BASE range, spanning from 0x40000000 to 0x400000FF, The
CPUCFG_KVM_BASE range between 0x40000000 - 0x400000FF is marked as reserved.
Consequently, all current and future processors will not implement any
feature within this range.
(CPUCFG_KVM_BASE 范围从 0x40000000 跨越到 0x400000FF。0x40000000 -
0x400000FF 之间的 CPUCFG_KVM_BASE 范围被标记为保留。因此，所有现在
和未来的处理器都不会实现该范围内的任何特性。)

On a KVM-virtualized Linux system, a read operation on cpucfg() at index
CPUCFG_KVM_BASE (0x40000000) returns the magic string 'KVM\0'.
(在 KVM 虚拟化的 Linux 系统上，读取索引为 CPUCFG_KVM_BASE
(0x40000000) 的 cpucfg() 会返回魔数(magic string)'KVM\0'。)

Once you have determined that your host is running on a paravirtualization-
capable KVM, you may now use hypercalls as described below.
(一旦确定你的主机运行在支持半虚拟化的 KVM 上，就可以按下文所述使用
超级调用了。)

KVM hypercall ABI(KVM 超级调用 ABI)
===================================

The KVM hypercall ABI is simple, with one scratch register a0 (v0) and at most
five generic registers (a1 - a5) used as input parameters. The FP (Floating-
point) and vector registers are not utilized as input registers and must
remain unmodified during a hypercall.
(KVM 超级调用 ABI 很简单，使用一个临时寄存器 a0 (v0) 和最多五个
通用寄存器(a1 - a5)作为输入参数。浮点(FP)和向量寄存器不用作输入
寄存器，并且在超级调用期间必须保持不变。)

Hypercall functions can be inlined as it only uses one scratch register.
(超级调用函数可以被内联，因为它只使用一个临时寄存器。)

The parameters are as follows:
(参数如下：)

	========	=================	================
	Register	IN			OUT
	========	=================	================
	a0		function number		Return	code
	a1		1st	parameter	-
	a2		2nd	parameter	-
	a3		3rd	parameter	-
	a4		4th	parameter	-
	a5		5th	parameter	-
	========	=================	================

The return codes may be one of the following:
(返回码可能是以下之一：)

	====		=========================
	Code		Meaning
	====		=========================
	0		Success
	-1		Hypercall not implemented
	-2		Bad Hypercall parameter
	====		=========================

KVM Hypercalls Documentation(KVM 超级调用文档)
==============================================

The template for each hypercall is as follows:
(每个超级调用的模板如下：)

1. Hypercall name
   (1. 超级调用名称)
2. Purpose
   (2. 用途)

1. KVM_HCALL_FUNC_IPI(1. KVM_HCALL_FUNC_IPI)
--------------------------------------------

:Purpose: Send IPIs to multiple vCPUs.
(用途：向多个 vCPU 发送 IPI。)

- a0: KVM_HCALL_FUNC_IPI
- a1: Lower part of the bitmap for destination physical CPUIDs
- a2: Higher part of the bitmap for destination physical CPUIDs
- a3: The lowest physical CPUID in the bitmap

The hypercall lets a guest send multiple IPIs (Inter-Process Interrupts) with
at most 128 destinations per hypercall. The destinations are represented in a
bitmap contained in the first two input registers (a1 and a2).
(该超级调用允许 guest 发送多个 IPI(处理器间中断)，每次超级调用最多
128 个目的地。目的地由包含在前两个输入寄存器(a1 和 a2)中的位图
表示。)

Bit 0 of a1 corresponds to the physical CPUID in the third input register (a3)
and bit 1 corresponds to the physical CPUID in a3+1, and so on.
(a1 的位 0 对应第三个输入寄存器(a3)中的物理 CPUID，位 1 对应
a3+1 中的物理 CPUID，依此类推。)

PV IPI on LoongArch includes both PV IPI multicast sending and PV IPI receiving,
and SWI is used for PV IPI inject since there is no VM-exits accessing SWI registers.
(LoongArch 上的 PV IPI 包括 PV IPI 组播发送和 PV IPI 接收；由于访问
SWI 寄存器不会引发 VM-exit，因此使用 SWI 来注入 PV IPI。)
