.. SPDX-License-Identifier: GPL-2.0

===============================================
membarrier() System Call(membarrier() 系统调用)
===============================================

MEMBARRIER_CMD_{PRIVATE,GLOBAL}_EXPEDITED - Architecture requirements(MEMBARRIER_CMD_{PRIVATE,GLOBAL}_EXPEDITED - 架构要求)
===========================================================================================================================

Memory barriers before updating rq->curr(更新 rq->curr 之前的内存屏障)
----------------------------------------------------------------------

The commands MEMBARRIER_CMD_PRIVATE_EXPEDITED and MEMBARRIER_CMD_GLOBAL_EXPEDITED
require each architecture to have a full memory barrier after coming from
user-space, before updating rq->curr.  This barrier is implied by the sequence
rq_lock(); smp_mb__after_spinlock() in __schedule().  The barrier matches a full
barrier in the proximity of the membarrier system call exit, cf.
membarrier_{private,global}_expedited().
(MEMBARRIER_CMD_PRIVATE_EXPEDITED 和 MEMBARRIER_CMD_GLOBAL_EXPEDITED 命令要求
每种体系结构在从用户空间返回之后、更新 rq->curr 之前有一个完整的内存屏障。
该屏障由 __schedule() 中的 rq_lock(); smp_mb__after_spinlock() 序列隐式提供。
它与 membarrier 系统调用出口附近的完整屏障相匹配，参见
membarrier_{private,global}_expedited()。)

Memory barriers after updating rq->curr(更新 rq->curr 之后的内存屏障)
---------------------------------------------------------------------

The commands MEMBARRIER_CMD_PRIVATE_EXPEDITED and MEMBARRIER_CMD_GLOBAL_EXPEDITED
require each architecture to have a full memory barrier after updating rq->curr,
before returning to user-space.  The schemes providing this barrier on the various
architectures are as follows.
(MEMBARRIER_CMD_PRIVATE_EXPEDITED 和 MEMBARRIER_CMD_GLOBAL_EXPEDITED 命令要求
每种体系结构在更新 rq->curr 之后、返回用户空间之前有一个完整的内存屏障。各种
体系结构上提供该屏障的方案如下。)

 - alpha, arc, arm, hexagon, mips rely on the full barrier implied by
   spin_unlock() in finish_lock_switch().
   (alpha、arc、arm、hexagon、mips 依赖 finish_lock_switch() 中 spin_unlock()
   所隐含的完整屏障。)

 - arm64 relies on the full barrier implied by switch_to().
   (arm64 依赖 switch_to() 所隐含的完整屏障。)

 - powerpc, riscv, s390, sparc, x86 rely on the full barrier implied
   by switch_mm(), if mm is not NULL; they rely on the full barrier implied
   by mmdrop(), otherwise.  On powerpc and riscv, switch_mm() relies on
   membarrier_arch_switch_mm().
   (powerpc、riscv、s390、sparc、x86 在 mm 非 NULL 时依赖 switch_mm() 所隐含
   的完整屏障；否则依赖 mmdrop() 所隐含的完整屏障。在 powerpc 和 riscv 上，
   switch_mm() 依赖 membarrier_arch_switch_mm()。)

The barrier matches a full barrier in the proximity of the membarrier system call
entry, cf. membarrier_{private,global}_expedited().
(该屏障与 membarrier 系统调用入口附近的完整屏障相匹配，参见
membarrier_{private,global}_expedited()。)
