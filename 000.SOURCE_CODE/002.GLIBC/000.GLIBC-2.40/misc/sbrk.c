/* Copyright (C) 1991-2024 Free Software Foundation, Inc.
   This file is part of the GNU C Library.

   The GNU C Library is free software; you can redistribute it and/or
   modify it under the terms of the GNU Lesser General Public
   License as published by the Free Software Foundation; either
   version 2.1 of the License, or (at your option) any later version.

   The GNU C Library is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
   Lesser General Public License for more details.

   You should have received a copy of the GNU Lesser General Public
   License along with the GNU C Library; if not, see
   <https://www.gnu.org/licenses/>.  */

/* Mark symbols hidden in static PIE for early self relocation to work.  */
#if BUILD_PIE_DEFAULT
# pragma GCC visibility push(hidden)
#endif
#include <errno.h>
#include <libc-internal.h>
#include <stdbool.h>
#include <stdint.h>
#include <unistd.h>

/* Defined in brk.c.  */
extern void *__curbrk;
extern int __brk (void *addr);

/**
 *  Extend the process's data space by INCREMENT. 
 *  If INCREMENT is negative, shrink data space by - INCREMENT.
 *  Return start of new space allocated, or -1 for errors. 
 * 
 * 扩展进程的数据段空间（堆内存），调整幅度为 INCREMENT：
 *   若 INCREMENT 为正数：扩展数据段，增加 INCREMENT 字节。
 *   若 INCREMENT 为负数：收缩数据段，减少 -INCREMENT 字节。
 * 
 * 返回值：
 *   成功：返回新分配空间的起始地址。
 *   失败：返回 -1（表示错误）。
 * 
 *  */
void * __sbrk (intptr_t increment)
{
  /** 
   * Controls whether __brk (0) is called to read the brk value from the kernel. 
   * 
   * _curbrk:  保存当前进程的 brk 指针（即堆顶地址）
   *  */
  bool update_brk = __curbrk == NULL;

#if defined(SHARED) && !IS_IN(rtld)
  if (!__libc_initial)
    {
      if (increment != 0)
	{
	  /* Do not allow changing the brk from an inner libc because
	     it cannot be synchronized with the outer libc's brk.  */
	  __set_errno (ENOMEM);
	  return (void *) -1;
	}
      /* Querying the kernel's brk value from an inner namespace is
	 fine.  */
      update_brk = true;
    }
#endif

  if (update_brk)
    {
      /**
       *  Initialize the break.
       * 
       * 在 glibc（GNU C Library）的内存管理中，break 指针（也称为 program break 或 brk 指针）是一个关键概念，
       * 用于动态管理进程的堆内存（heap）。它标记了堆空间的当前最高地址（堆顶），决定了进程可以通过
       * brk/sbrk 系统调用分配或释放的内存范围。
       * 
       * break 指针的作用
       * 定义堆的边界：
       *   break 指针指向堆内存的当前末尾地址（即堆顶），所有通过 malloc 分配的动态内存均位于 break 指针之下。
       * 动态调整堆大小：
       *  通过系统调用 brk() 或 sbrk() 移动 break 指针，可以扩展或收缩堆空间：
       *    扩展堆：将 break 指针上移（增加堆大小）。
       *    收缩堆：将 break 指针下移（释放内存回操作系统）。
       */
      if (__brk (0) < 0)
	{
	  return (void *) -1;
	}
    }

  if (increment == 0)
    {
      return __curbrk;
    }

  void *oldbrk = __curbrk; // 当前没有brk

  if (increment > 0
	  ? ((uintptr_t) oldbrk + (uintptr_t) increment < (uintptr_t) oldbrk)
	  : ((uintptr_t) oldbrk < (uintptr_t) -increment))
    {
      __set_errno (ENOMEM);
      return (void *) -1;
    }

  // 发起brk系统调用,扩展内存
  if (__brk (oldbrk + increment) < 0)
    {
      return (void *) -1;
    }

  return oldbrk;
}
libc_hidden_def (__sbrk)
weak_alias (__sbrk, sbrk)
