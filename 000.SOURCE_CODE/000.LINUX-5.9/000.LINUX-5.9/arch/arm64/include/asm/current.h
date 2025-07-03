/* SPDX-License-Identifier: GPL-2.0 */
#ifndef __ASM_CURRENT_H
#define __ASM_CURRENT_H

#include <linux/compiler.h>

#ifndef __ASSEMBLY__

struct task_struct;

/*
 * We don't use read_sysreg() as we want the compiler to cache the value where
 * possible.
 * 
 * mrs 指令全称为 "Move to Register from System register"，用于将系统寄存器（System Register）的值读取到通用寄存器（General-Purpose Register, GPR）中。
 * 
 * 将 sp_el0 读取到 0寄存器中
 * 
 * 在 ARM64（AArch64）架构中，SP_EL0 是一个系统寄存器，用于存储当前异常级别（EL0，即用户态）的栈指针（Stack Pointer）
 * 
 * 在内核态中，ARM64处理器运行在EL1里，SP_EL0寄存器在EL1上下文中是没有使用的。利用SP_EL0寄存器来存放当前进程的task_struct数据结构的地址是一个简洁有效的办法
 */
static __always_inline struct task_struct *get_current(void)
{
	unsigned long sp_el0;

	asm ("mrs %0, sp_el0" : "=r" (sp_el0));

	return (struct task_struct *)sp_el0;
}

#define current get_current()

#endif /* __ASSEMBLY__ */

#endif /* __ASM_CURRENT_H */

