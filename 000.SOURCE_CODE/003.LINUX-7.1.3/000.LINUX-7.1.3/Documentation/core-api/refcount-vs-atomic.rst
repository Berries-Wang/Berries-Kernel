======================================================================
refcount_t API compared to atomic_t(refcount_t API 与 atomic_t 的对比)
======================================================================

.. contents:: :local:

Introduction(引言)
==================

The goal of refcount_t API is to provide a minimal API for implementing
an object's reference counters. While a generic architecture-independent
implementation from lib/refcount.c uses atomic operations underneath,
there are a number of differences between some of the ``refcount_*()`` and
``atomic_*()`` functions with regards to the memory ordering guarantees.
This document outlines the differences and provides respective examples
in order to help maintainers validate their code against the change in
these memory ordering guarantees.
(refcount_t API 的目标是提供一个实现对象引用计数的最小 API。虽然 lib/refcount.c 中通用的、与架构无关的实现在底层使用原子操作，但某些 ``refcount_*()`` 与 ``atomic_*()`` 函数在内存序保证方面存在若干差异。本文概述这些差异并给出相应的示例，以帮助维护者针对这些内存序保证的变化校验自己的代码。)

The terms used through this document try to follow the formal LKMM defined in
tools/memory-model/Documentation/explanation.txt.
(本文通篇使用的术语尽量遵循 tools/memory-model/Documentation/explanation.txt 中定义的正式 LKMM。)

memory-barriers.txt and atomic_t.txt provide more background to the
memory ordering in general and for atomic operations specifically.
(memory-barriers.txt 与 atomic_t.txt 提供了关于内存序总体以及原子操作方面的更多背景知识。)

Relevant types of memory ordering(相关的内存序类型)
===================================================

.. note:: The following section only covers some of the memory
   ordering types that are relevant for the atomics and reference
   counters and used through this document. For a much broader picture
   please consult memory-barriers.txt document.
   (注意：下面的小节只涵盖与原子操作和引用计数相关且在本文中用到的部分内存序类型。要了解更全面的内容，请参阅 memory-barriers.txt 文档。)

In the absence of any memory ordering guarantees (i.e. fully unordered)
atomics & refcounters only provide atomicity and
program order (po) relation (on the same CPU). It guarantees that
each ``atomic_*()`` and ``refcount_*()`` operation is atomic and instructions
are executed in program order on a single CPU.
This is implemented using READ_ONCE()/WRITE_ONCE() and
compare-and-swap primitives.
(在没有任何内存序保证的情况下(即完全无序)，原子操作与引用计数器只提供原子性以及(同一 CPU 上的)程序顺序 (po) 关系。它保证每个 ``atomic_*()`` 与 ``refcount_*()`` 操作都是原子的，且指令在单个 CPU 上按程序顺序执行。这是使用 READ_ONCE()/WRITE_ONCE() 与比较并交换(CAS)原语实现的。)

A strong (full) memory ordering guarantees that all prior loads and
stores (all po-earlier instructions) on the same CPU are completed
before any po-later instruction is executed on the same CPU.
It also guarantees that all po-earlier stores on the same CPU
and all propagated stores from other CPUs must propagate to all
other CPUs before any po-later instruction is executed on the original
CPU (A-cumulative property). This is implemented using smp_mb().
(强(完全)内存序保证：在同一 CPU 上，所有先前的加载与存储(所有 po 更早的指令)都在执行任何 po 更晚的指令之前完成。它还保证：在原始 CPU 执行任何 po 更晚的指令之前，该 CPU 上所有 po 更早的存储以及从其他 CPU 传播而来的所有存储都必须传播到所有其他 CPU(A 累积属性)。这是使用 smp_mb() 实现的。)

A RELEASE memory ordering guarantees that all prior loads and
stores (all po-earlier instructions) on the same CPU are completed
before the operation. It also guarantees that all po-earlier
stores on the same CPU and all propagated stores from other CPUs
must propagate to all other CPUs before the release operation
(A-cumulative property). This is implemented using
smp_store_release().
(RELEASE 内存序保证：在同一 CPU 上，所有先前的加载与存储(所有 po 更早的指令)都在该操作之前完成。它还保证：在该释放操作之前，同一 CPU 上所有 po 更早的存储以及从其他 CPU 传播而来的所有存储都必须传播到所有其他 CPU(A 累积属性)。这是使用 smp_store_release() 实现的。)

An ACQUIRE memory ordering guarantees that all post loads and
stores (all po-later instructions) on the same CPU are
completed after the acquire operation. It also guarantees that all
po-later stores on the same CPU must propagate to all other CPUs
after the acquire operation executes. This is implemented using
smp_acquire__after_ctrl_dep().
(ACQUIRE 内存序保证：在同一 CPU 上，所有后续的加载与存储(所有 po 更晚的指令)都在获取(acquire)操作之后完成。它还保证：同一 CPU 上所有 po 更晚的存储都必须在获取操作执行之后传播到所有其他 CPU。这是使用 smp_acquire__after_ctrl_dep() 实现的。)

A control dependency (on success) for refcounters guarantees that
if a reference for an object was successfully obtained (reference
counter increment or addition happened, function returned true),
then further stores are ordered against this operation.
Control dependency on stores are not implemented using any explicit
barriers, but rely on CPU not to speculate on stores. This is only
a single CPU relation and provides no guarantees for other CPUs.
(引用计数器的控制依赖(成功时)保证：如果成功获取了对象的引用(引用计数发生了递增或累加，函数返回 true)，则后续的存储相对于该操作是有序的。对存储的控制依赖不使用任何显式屏障实现，而是依赖 CPU 不对存储进行投机执行。这只是单一 CPU 上的关系，不为其他 CPU 提供任何保证。)


Comparison of functions(函数对比)
=================================

case 1) - non-"Read/Modify/Write" (RMW) ops(情况 1：非"读/改/写" (RMW) 操作)
----------------------------------------------------------------------------

Function changes:
(函数变化：)

 * atomic_set() --> refcount_set()
 * atomic_read() --> refcount_read()

Memory ordering guarantee changes:
(内存序保证变化：)

 * none (both fully unordered)
   (无(两者均为完全无序))


case 2) - non-"Read/Modify/Write" (RMW) ops with release ordering(情况 2：带 release 序的非"读/改/写" (RMW) 操作)
----------------------------------------------------------------------------------------------------------------

Function changes:
(函数变化：)

 * atomic_set_release() --> refcount_set_release()

Memory ordering guarantee changes:
(内存序保证变化：)

 * none (both provide RELEASE ordering)
   (无(两者均提供 RELEASE 序))


case 3) - increment-based ops that return no value(情况 3：不返回值的递增类操作)
--------------------------------------------------------------------------------

Function changes:
(函数变化：)

 * atomic_inc() --> refcount_inc()
 * atomic_add() --> refcount_add()

Memory ordering guarantee changes:
(内存序保证变化：)

 * none (both fully unordered)
   (无(两者均为完全无序))

case 4) - decrement-based RMW ops that return no value(情况 4：不返回值的递减类 RMW 操作)
-----------------------------------------------------------------------------------------

Function changes:
(函数变化：)

 * atomic_dec() --> refcount_dec()

Memory ordering guarantee changes:
(内存序保证变化：)

 * fully unordered --> RELEASE ordering
   (完全无序 --> RELEASE 序)


case 5) - increment-based RMW ops that return a value(情况 5：返回值的递增类 RMW 操作)
--------------------------------------------------------------------------------------

Function changes:
(函数变化：)

 * atomic_inc_not_zero() --> refcount_inc_not_zero()
 * no atomic counterpart --> refcount_add_not_zero()

Memory ordering guarantees changes:
(内存序保证变化：)

 * fully ordered --> control dependency on success for stores
   (完全有序 --> 成功时对存储的控制依赖)

.. note:: We really assume here that necessary ordering is provided as a
   result of obtaining pointer to the object!
   (注意：这里我们确实假定获取指向对象的指针这一结果本身就提供了必要的顺序！)


case 6) - increment-based RMW ops with acquire ordering that return a value(情况 6：带 acquire 序且返回值的递增类 RMW 操作)
---------------------------------------------------------------------------------------------------------------------------

Function changes:
(函数变化：)

 * atomic_inc_not_zero() --> refcount_inc_not_zero_acquire()
 * no atomic counterpart --> refcount_add_not_zero_acquire()

Memory ordering guarantees changes:
(内存序保证变化：)

 * fully ordered --> ACQUIRE ordering on success
   (完全有序 --> 成功时提供 ACQUIRE 序)


case 7) - generic dec/sub decrement-based RMW ops that return a value(情况 7：返回值的通用 dec/sub 递减类 RMW 操作)
-------------------------------------------------------------------------------------------------------------------

Function changes:
(函数变化：)

 * atomic_dec_and_test() --> refcount_dec_and_test()
 * atomic_sub_and_test() --> refcount_sub_and_test()

Memory ordering guarantees changes:
(内存序保证变化：)

 * fully ordered --> RELEASE ordering + ACQUIRE ordering on success
   (完全有序 --> RELEASE 序 + 成功时提供 ACQUIRE 序)


case 8) other decrement-based RMW ops that return a value(情况 8：其他返回值的递减类 RMW 操作)
----------------------------------------------------------------------------------------------

Function changes:
(函数变化：)

 * no atomic counterpart --> refcount_dec_if_one()
 * ``atomic_add_unless(&var, -1, 1)`` --> ``refcount_dec_not_one(&var)``

Memory ordering guarantees changes:
(内存序保证变化：)

 * fully ordered --> RELEASE ordering + control dependency
   (完全有序 --> RELEASE 序 + 控制依赖)

.. note:: atomic_add_unless() only provides full order on success.
   (注意：atomic_add_unless() 只在成功时提供完全序。)


case 9) - lock-based RMW(情况 9：基于锁的 RMW)
----------------------------------------------

Function changes:
(函数变化：)

 * atomic_dec_and_lock() --> refcount_dec_and_lock()
 * atomic_dec_and_mutex_lock() --> refcount_dec_and_mutex_lock()

Memory ordering guarantees changes:
(内存序保证变化：)

 * fully ordered --> RELEASE ordering + control dependency + hold
   spin_lock() on success
   (完全有序 --> RELEASE 序 + 控制依赖 + 成功时持有 spin_lock())
