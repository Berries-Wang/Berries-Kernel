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

/**
 * We need separate acquire parameters for ll/sc and lse, since the full
 * barrier case is generated as release+dmb for the former and
 * acquire+release for the latter.
 * 
 * 
 * chg(new, v)。它的实现机制是把new赋给原子变量v，返回原子变量v的旧值
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
static __always_inline  unsigned long __xchg##sfx(unsigned long x,	\
					volatile void *ptr,		\
					int size)			\
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
	__ret = (__typeof__(*(ptr)))					\
		__xchg##sfx((unsigned long)(x), (ptr), sizeof(*(ptr))); \
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

#define __CMPXCHG_DBL(name)						\
static inline long __cmpxchg_double##name(unsigned long old1,		\
					 unsigned long old2,		\
					 unsigned long new1,		\
					 unsigned long new2,		\
					 volatile void *ptr)		\
{									\
	return __lse_ll_sc_body(_cmpxchg_double##name, 			\
				old1, old2, new1, new2, ptr);		\
}

__CMPXCHG_DBL(   )
__CMPXCHG_DBL(_mb)

#undef __CMPXCHG_DBL

/**
 * tag:'__cmpxchg##sfx'
 * 
 * __cmpxchg_case##sfx##_8 这个根据指令版本，在 atomic_lse.h / atomic_ll_sc.h 文件中
 */
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

/**
 * __cmpxchg##sfx 就在本文件中，tag:'__cmpxchg##sfx'
 */
#define __cmpxchg_wrapper(sfx, ptr, o, n)				\
({									\
	__typeof__(*(ptr)) __ret;					\
	__ret = (__typeof__(*(ptr)))					\
		__cmpxchg##sfx((ptr), (unsigned long)(o),		\
				(unsigned long)(n), sizeof(*(ptr)));	\
	__ret;								\
})

/**
 * 比较并交换指令——cas指令 , 见[Run Linux Kernel (2nd Edition) Volume 2: Debugging and Case Analysis.epub]#1.1.3　比较并交换指令
 * Linux内核中常见的比较并交换函数是cmpxchg(),见[Run Linux Kernel (2nd Edition) Volume 2: Debugging and Case Analysis.epub]#2．cmpxchg()函数
 */
/* cmpxchg */
#define arch_cmpxchg_relaxed(...)	__cmpxchg_wrapper(    , __VA_ARGS__)
#define arch_cmpxchg_acquire(...)	__cmpxchg_wrapper(_acq, __VA_ARGS__)
#define arch_cmpxchg_release(...)	__cmpxchg_wrapper(_rel, __VA_ARGS__)
#define arch_cmpxchg(...)		    __cmpxchg_wrapper( _mb, __VA_ARGS__)
#define arch_cmpxchg_local		    arch_cmpxchg_relaxed

/* cmpxchg64 */
#define arch_cmpxchg64_relaxed		arch_cmpxchg_relaxed
#define arch_cmpxchg64_acquire		arch_cmpxchg_acquire
#define arch_cmpxchg64_release		arch_cmpxchg_release
#define arch_cmpxchg64			    arch_cmpxchg
#define arch_cmpxchg64_local		arch_cmpxchg_local

/* cmpxchg_double */
#define system_has_cmpxchg_double()     1

#define __cmpxchg_double_check(ptr1, ptr2)					\
({										\
	if (sizeof(*(ptr1)) != 8)						\
		BUILD_BUG();							\
	VM_BUG_ON((unsigned long *)(ptr2) - (unsigned long *)(ptr1) != 1);	\
})

#define arch_cmpxchg_double(ptr1, ptr2, o1, o2, n1, n2)				\
({										\
	int __ret;								\
	__cmpxchg_double_check(ptr1, ptr2);					\
	__ret = !__cmpxchg_double_mb((unsigned long)(o1), (unsigned long)(o2),	\
				     (unsigned long)(n1), (unsigned long)(n2),	\
				     ptr1);					\
	__ret;									\
})

#define arch_cmpxchg_double_local(ptr1, ptr2, o1, o2, n1, n2)			\
({										\
	int __ret;								\
	__cmpxchg_double_check(ptr1, ptr2);					\
	__ret = !__cmpxchg_double((unsigned long)(o1), (unsigned long)(o2),	\
				  (unsigned long)(n1), (unsigned long)(n2),	\
				  ptr1);					\
	__ret;									\
})

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
	: [tmp] "=&r" (tmp), [v] "+Q" (*(unsigned long *)ptr)		\
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

__CMPWAIT_GEN()

#undef __CMPWAIT_GEN

#define __cmpwait_relaxed(ptr, val) \
	__cmpwait((ptr), (unsigned long)(val), sizeof(*(ptr)))

#endif	/* __ASM_CMPXCHG_H */
