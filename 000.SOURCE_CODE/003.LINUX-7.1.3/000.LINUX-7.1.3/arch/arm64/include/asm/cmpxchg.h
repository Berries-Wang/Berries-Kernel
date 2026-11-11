/* SPDX-License-Identifier: GPL-2.0-only */
/*
 * Based on arch/arm/include/asm/cmpxchg.h
 *
 * Copyright (C) 2012 ARM Ltd.
 */
#ifndef __ASM_CMPXCHG_H
#define __ASM_CMPXCHG_H

#include <linux/build_bug.h>
#include <linux/compiler.h>

#include <asm/barrier.h>
#include <asm/lse.h>

/*
 * We need separate acquire parameters for ll/sc and lse, since the full
 * barrier case is generated as release+dmb for the former and
 * acquire+release for the latter.
 */
#define __XCHG_CASE(w, sfx, name, sz, mb, nop_lse, acq, acq_lse, rel, cl)	\
static inline u##sz __xchg_case_##name##sz(u##sz x, volatile void *ptr)		\
{										\
	u##sz ret;								\
	unsigned long tmp;							\
										\
	asm volatile(ARM64_LSE_ATOMIC_INSN(					\
	/* LL/SC */								\
	"	prfm	pstl1strm, %2\n"					\
	"1:	ld" #acq "xr" #sfx "\t%" #w "0, %2\n"				\
	"	st" #rel "xr" #sfx "\t%w1, %" #w "3, %2\n"			\
	"	cbnz	%w1, 1b\n"						\
	"	" #mb,								\
	/* LSE atomics */							\
	"	swp" #acq_lse #rel #sfx "\t%" #w "3, %" #w "0, %2\n"		\
		__nops(3)							\
	"	" #nop_lse)							\
	: "=&r" (ret), "=&r" (tmp), "+Q" (*(u##sz *)ptr)			\
	: "r" (x)								\
	: cl);									\
										\
	return ret;								\
}

__XCHG_CASE(w, b,     ,  8,        ,    ,  ,  ,  ,         )
__XCHG_CASE(w, h,     , 16,        ,    ,  ,  ,  ,         )
__XCHG_CASE(w,  ,     , 32,        ,    ,  ,  ,  ,         )
__XCHG_CASE( ,  ,     , 64,        ,    ,  ,  ,  ,         )
__XCHG_CASE(w, b, acq_,  8,        ,    , a, a,  , "memory")
__XCHG_CASE(w, h, acq_, 16,        ,    , a, a,  , "memory")
__XCHG_CASE(w,  , acq_, 32,        ,    , a, a,  , "memory")
__XCHG_CASE( ,  , acq_, 64,        ,    , a, a,  , "memory")
__XCHG_CASE(w, b, rel_,  8,        ,    ,  ,  , l, "memory")
__XCHG_CASE(w, h, rel_, 16,        ,    ,  ,  , l, "memory")
__XCHG_CASE(w,  , rel_, 32,        ,    ,  ,  , l, "memory")
__XCHG_CASE( ,  , rel_, 64,        ,    ,  ,  , l, "memory")
__XCHG_CASE(w, b,  mb_,  8, dmb ish, nop,  , a, l, "memory")
__XCHG_CASE(w, h,  mb_, 16, dmb ish, nop,  , a, l, "memory")
__XCHG_CASE(w,  ,  mb_, 32, dmb ish, nop,  , a, l, "memory")
__XCHG_CASE( ,  ,  mb_, 64, dmb ish, nop,  , a, l, "memory")

#undef __XCHG_CASE

#define __XCHG_GEN(sfx)							\
static __always_inline unsigned long					\
__arch_xchg##sfx(unsigned long x, volatile void *ptr, int size)		\
{									\
	switch (size) {							\
	case 1:								\
		return __xchg_case##sfx##_8(x, ptr);			\
	case 2:								\
		return __xchg_case##sfx##_16(x, ptr);			\
	case 4:								\
		return __xchg_case##sfx##_32(x, ptr);			\
	case 8:								\
		return __xchg_case##sfx##_64(x, ptr);			\
	default:							\
		BUILD_BUG();						\
	}								\
									\
	unreachable();							\
}

__XCHG_GEN()
__XCHG_GEN(_acq)
__XCHG_GEN(_rel)
__XCHG_GEN(_mb)

#undef __XCHG_GEN

#define __xchg_wrapper(sfx, ptr, x)					\
({									\
	__typeof__(*(ptr)) __ret;					\
	__ret = (__force __typeof__(*(ptr)))				\
		__arch_xchg##sfx((__force unsigned long)(x), (ptr),	\
				  sizeof(*(ptr)));			\
	__ret;								\
})

/* xchg */
#define arch_xchg_relaxed(...)	__xchg_wrapper(    , __VA_ARGS__)
#define arch_xchg_acquire(...)	__xchg_wrapper(_acq, __VA_ARGS__)
#define arch_xchg_release(...)	__xchg_wrapper(_rel, __VA_ARGS__)
#define arch_xchg(...)		__xchg_wrapper( _mb, __VA_ARGS__)

#define __CMPXCHG_CASE(name, sz)			\
static inline u##sz __cmpxchg_case_##name##sz(volatile void *ptr,	\
					      u##sz old,		\
					      u##sz new)		\
{									\
	return __lse_ll_sc_body(_cmpxchg_case_##name##sz,		\
				ptr, old, new);				\
}

__CMPXCHG_CASE(    ,  8)
__CMPXCHG_CASE(    , 16)
__CMPXCHG_CASE(    , 32)
__CMPXCHG_CASE(    , 64)
__CMPXCHG_CASE(acq_,  8)
__CMPXCHG_CASE(acq_, 16)
__CMPXCHG_CASE(acq_, 32)
__CMPXCHG_CASE(acq_, 64)
__CMPXCHG_CASE(rel_,  8)
__CMPXCHG_CASE(rel_, 16)
__CMPXCHG_CASE(rel_, 32)
__CMPXCHG_CASE(rel_, 64)
__CMPXCHG_CASE(mb_,  8)
__CMPXCHG_CASE(mb_, 16)
__CMPXCHG_CASE(mb_, 32)
__CMPXCHG_CASE(mb_, 64)

#undef __CMPXCHG_CASE

#define __CMPXCHG128(name)						\
static inline u128 __cmpxchg128##name(volatile u128 *ptr,		\
				      u128 old, u128 new)		\
{									\
	return __lse_ll_sc_body(_cmpxchg128##name,			\
				ptr, old, new);				\
}

__CMPXCHG128(   )
__CMPXCHG128(_mb)

#undef __CMPXCHG128

#define __CMPXCHG_GEN(sfx)						\
static __always_inline unsigned long __cmpxchg##sfx(volatile void *ptr,	\
					   unsigned long old,		\
					   unsigned long new,		\
					   int size)			\
{									\
	switch (size) {							\
	case 1:								\
		return __cmpxchg_case##sfx##_8(ptr, old, new);		\
	case 2:								\
		return __cmpxchg_case##sfx##_16(ptr, old, new);		\
	case 4:								\
		return __cmpxchg_case##sfx##_32(ptr, old, new);		\
	case 8:								\
		return __cmpxchg_case##sfx##_64(ptr, old, new);		\
	default:							\
		BUILD_BUG();						\
	}								\
									\
	unreachable();							\
}

__CMPXCHG_GEN()
__CMPXCHG_GEN(_acq)
__CMPXCHG_GEN(_rel)
__CMPXCHG_GEN(_mb)

#undef __CMPXCHG_GEN

#define __cmpxchg_wrapper(sfx, ptr, o, n)				\
({									\
	__typeof__(*(ptr)) __ret;					\
	__ret = (__force __typeof__(*(ptr)))				\
		__cmpxchg##sfx((ptr), (__force unsigned long)(o),	\
				(__force unsigned long)(n),		\
				sizeof(*(ptr)));			\
	__ret;								\
})

/* cmpxchg */
#define arch_cmpxchg_relaxed(...)	__cmpxchg_wrapper(    , __VA_ARGS__)
#define arch_cmpxchg_acquire(...)	__cmpxchg_wrapper(_acq, __VA_ARGS__)
#define arch_cmpxchg_release(...)	__cmpxchg_wrapper(_rel, __VA_ARGS__)
#define arch_cmpxchg(...)		__cmpxchg_wrapper( _mb, __VA_ARGS__)
#define arch_cmpxchg_local		arch_cmpxchg_relaxed

/* cmpxchg64 */
#define arch_cmpxchg64_relaxed		arch_cmpxchg_relaxed
#define arch_cmpxchg64_acquire		arch_cmpxchg_acquire
#define arch_cmpxchg64_release		arch_cmpxchg_release
#define arch_cmpxchg64			arch_cmpxchg
#define arch_cmpxchg64_local		arch_cmpxchg_local

/* cmpxchg128 */
#define system_has_cmpxchg128()		1

#define arch_cmpxchg128(ptr, o, n)						\
({										\
	__cmpxchg128_mb((ptr), (o), (n));					\
})

#define arch_cmpxchg128_local(ptr, o, n)					\
({										\
	__cmpxchg128((ptr), (o), (n));						\
})

/**
 * 条件轮询中的“等待辅助函数”：如果 *ptr 仍等于旧值 val，就尝试用 WFE 等待事件；如果已经不同，就直接返回，让外层重新检查条件。
 * 
 * wfe 指令: [007.BOOKs/Arm Architecture Reference Manual for A-profile architecture/DDI0487_M_a_a_a-profile_architecture_reference_manual-1-3000.pdf]#C6.2.501 WFE
 * wfe: Wait for event :
 *   This instruction is a hint instruction that indicates that the PE can enter a low-power state and remain there until a wakeup event occurs. 
 *   Wakeup events include the event signaled as a result of executing the SEV instruction on any PE in the multiprocessor system.
 *  (这是一条提示指令，表示PE可以进入低功耗状态并保持在该状态，直到发生唤醒事件。唤醒事件包括由于在多处理器系统中的任何PE上执行SEV指令而发出信号的事件)
 * 
 * -> SEVL, Send Event Local: Causes an event to be signaled locally without requiring the event to be signaled to other PEs in a multiprocessor system.
 * -> ldxr: [C6.2.265 LDXR] This instruction derives an address from a base register value, loads a 32-bit word or a 64-bit doubleword from memory, and writes it to a register. The memory access is atomic. The PE marks the physical address being accessed as an exclusive access. This exclusive access mark is checked by Store Exclusive instructions.
 * -> eor: [F5.1.49 EOR, EORS (immediate)]  按位异或
 * -> cbnz: [C6.2.77 CBNZ] Compare and branch on nonzero
 * 
 * 
 * [B2.12 Synchronization and semaphores#独占访问标记如何取消标记]: 
 * - [由 A Store-Exclusive instruction 指令(STXR)取消] A Load-Exclusive instruction marks a small block of memory for exclusive access. The size of the marked block is IMPLEMENTATION DEFINED, see Marking and the size of the marked memory block. A Store-Exclusive instruction to any address in the marked block clears the marking.
 * - 参考: [001.UNIX-DOCS/008.同步原语/001.原子指令/003.LLSC实现原理.md]
 */
#define __CMPWAIT_CASE(w, sfx, sz)					\
static inline void __cmpwait_case_##sz(volatile void *ptr,		\
				       unsigned long val)		\
{									\
	unsigned long tmp;						\
									\
	asm volatile(							\
	"	sevl\n"							\
	"	wfe\n"							\
	"	ldxr" #sfx "\t%" #w "[tmp], %[v]\n"			\
	"	eor	%" #w "[tmp], %" #w "[tmp], %" #w "[val]\n"	\
	"	cbnz	%" #w "[tmp], 1f\n"				\
	"	wfe\n"							\
	"1:"								\
	: [tmp] "=&r" (tmp), [v] "+Q" (*(u##sz *)ptr)			\
	: [val] "r" (val));						\
}

__CMPWAIT_CASE(w, b, 8);
__CMPWAIT_CASE(w, h, 16);
__CMPWAIT_CASE(w,  , 32);
__CMPWAIT_CASE( ,  , 64);

#undef __CMPWAIT_CASE

#define __CMPWAIT_GEN(sfx)						\
static __always_inline void __cmpwait##sfx(volatile void *ptr,		\
				  unsigned long val,			\
				  int size)				\
{									\
	switch (size) {							\
	case 1:								\
		return __cmpwait_case##sfx##_8(ptr, (u8)val);		\
	case 2:								\
		return __cmpwait_case##sfx##_16(ptr, (u16)val);		\
	case 4:								\
		return __cmpwait_case##sfx##_32(ptr, val);		\
	case 8:								\
		return __cmpwait_case##sfx##_64(ptr, val);		\
	default:							\
		BUILD_BUG();						\
	}								\
									\
	unreachable();							\
}

/**
 * 
 * 因为 sfx 为空，会生成 static __always_inline void __cmpwait(volatile void *ptr, unsigned long val, int size) 函数
 */
__CMPWAIT_GEN()

#undef __CMPWAIT_GEN

#define __cmpwait_relaxed(ptr, val) \
	__cmpwait((ptr), (unsigned long)(val), sizeof(*(ptr)))

#endif	/* __ASM_CMPXCHG_H */
