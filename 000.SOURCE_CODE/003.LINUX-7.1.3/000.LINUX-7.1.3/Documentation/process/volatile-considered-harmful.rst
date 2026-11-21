
.. _volatile_considered_harmful:

Why the "volatile" type class should not be used(为什么不应使用 "volatile" 类型)
--------------------------------------------------------------------------------

C programmers have often taken volatile to mean that the variable could be
changed outside of the current thread of execution; as a result, they are
sometimes tempted to use it in kernel code when shared data structures are
being used.  In other words, they have been known to treat volatile types
as a sort of easy atomic variable, which they are not.  The use of volatile in
kernel code is almost never correct; this document describes why.
(C 程序员常常把 volatile 理解为变量可以在当前执行线程之外被改变；因此，当使用共享数据结构时，他们有时想在内核代码中使用它。换句话说，众所周知，有人会把 volatile 类型当作某种简单的原子变量来对待，而它并非如此。在内核代码中使用 volatile 几乎总是不正确的；本文档描述了原因。)

The key point to understand with regard to volatile is that its purpose is
to suppress optimization, which is almost never what one really wants to
do.  In the kernel, one must protect shared data structures against
unwanted concurrent access, which is very much a different task.  The
process of protecting against unwanted concurrency will also avoid almost
all optimization-related problems in a more efficient way.
(关于 volatile 需要理解的关键点是，它的目的是抑制优化，而这几乎从来不是人们真正想要做的。在内核中，必须保护共享数据结构免受不需要的并发访问，这是截然不同的任务。防止不需要的并发访问的过程还会以更高效的方式避免几乎所有与优化相关的问题。)

Like volatile, the kernel primitives which make concurrent access to data
safe (spinlocks, mutexes, memory barriers, etc.) are designed to prevent
unwanted optimization.  If they are being used properly, there will be no
need to use volatile as well.  If volatile is still necessary, there is
almost certainly a bug in the code somewhere.  In properly-written kernel
code, volatile can only serve to slow things down.
(与 volatile 一样，使对数据的并发访问变得安全的内核原语（自旋锁、互斥锁、内存屏障等）被设计用来阻止不需要的优化。如果它们使用得当，就不再需要同时使用 volatile。如果仍然需要 volatile，那么代码中几乎可以肯定在某处存在缺陷。在编写得当的内核代码中，volatile 只能起到拖慢速度的作用。)

Consider a typical block of kernel code::

    spin_lock(&the_lock);
    do_something_on(&shared_data);
    do_something_else_with(&shared_data);
    spin_unlock(&the_lock);

(考虑一个典型的内核代码块：)

If all the code follows the locking rules, the value of shared_data cannot
change unexpectedly while the_lock is held.  Any other code which might
want to play with that data will be waiting on the lock.  The spinlock
primitives act as memory barriers - they are explicitly written to do so -
meaning that data accesses will not be optimized across them.  So the
compiler might think it knows what will be in shared_data, but the
spin_lock() call, since it acts as a memory barrier, will force it to
forget anything it knows.  There will be no optimization problems with
accesses to that data.
(如果所有代码都遵循加锁规则，那么在持有 the_lock 期间 shared_data 的值不会意外改变。任何其他可能想操作该数据的代码都会在锁上等待。自旋锁原语充当内存屏障——它们被明确编写成这样——这意味着数据访问不会被优化而跨越它们。因此，编译器可能认为自己知道 shared_data 中会有什么，但 spin_lock() 调用由于充当内存屏障，会迫使它忘掉它所知道的一切。对该数据的访问不会有优化问题。)

If shared_data were declared volatile, the locking would still be
necessary.  But the compiler would also be prevented from optimizing access
to shared_data _within_ the critical section, when we know that nobody else
can be working with it.  While the lock is held, shared_data is not
volatile.  When dealing with shared data, proper locking makes volatile
unnecessary - and potentially harmful.
(如果 shared_data 被声明为 volatile，加锁仍然是必要的。但编译器还会被禁止优化临界区内对 shared_data 的访问，而我们知道在临界区内没有其他人可以操作它。在持有锁期间，shared_data 并不是 volatile 的。在处理共享数据时，正确的加锁使 volatile 变得没有必要——而且可能有害。)

The volatile storage class was originally meant for memory-mapped I/O
registers.  Within the kernel, register accesses, too, should be protected
by locks, but one also does not want the compiler "optimizing" register
accesses within a critical section.  But, within the kernel, I/O memory
accesses are always done through accessor functions; accessing I/O memory
directly through pointers is frowned upon and does not work on all
architectures.  Those accessors are written to prevent unwanted
optimization, so, once again, volatile is unnecessary.
(volatile 存储类最初是为内存映射 I/O 寄存器设计的。在内核中，寄存器访问也应该由锁保护，但人们同样不希望编译器"优化"临界区内的寄存器访问。不过，在内核中，I/O 内存访问总是通过访问器函数完成的；直接通过指针访问 I/O 内存是不被认可的，并且在所有架构上都行不通。这些访问器被编写为阻止不需要的优化，所以同样，volatile 是不必要的。)

Another situation where one might be tempted to use volatile is
when the processor is busy-waiting on the value of a variable.  The right
way to perform a busy wait is::

    while (my_variable != what_i_want)
        cpu_relax();

(另一种可能让人想使用 volatile 的情况是处理器在忙等某个变量的值。执行忙等的正确方式是：)

The cpu_relax() call can lower CPU power consumption or yield to a
hyperthreaded twin processor; it also happens to serve as a compiler
barrier, so, once again, volatile is unnecessary.  Of course, busy-
waiting is generally an anti-social act to begin with.
(cpu_relax() 调用可以降低 CPU 功耗或让位给超线程的孪生处理器；它恰好也充当编译器屏障，所以同样，volatile 是不必要的。当然，忙等本身就是一种通常不受欢迎的行为。)

There are still a few rare situations where volatile makes sense in the
kernel:
(在内核中仍有少数几种罕见情况适合使用 volatile：)

  - The above-mentioned accessor functions might use volatile on
    architectures where direct I/O memory access does work.  Essentially,
    each accessor call becomes a little critical section on its own and
    ensures that the access happens as expected by the programmer.
    (上述访问器函数在直接 I/O 内存访问可行的架构上可能会使用 volatile。本质上，每次访问器调用都成为其自身的一个小临界区，并确保访问按程序员的预期发生。)

  - Inline assembly code which changes memory, but which has no other
    visible side effects, risks being deleted by GCC.  Adding the volatile
    keyword to asm statements will prevent this removal.
    (改变内存但没有其他可见副作用的内联汇编代码有被 GCC 删除的风险。在 asm 语句中添加 volatile 关键字可以防止这种删除。)

  - The jiffies variable is special in that it can have a different value
    every time it is referenced, but it can be read without any special
    locking.  So jiffies can be volatile, but the addition of other
    variables of this type is strongly frowned upon.  Jiffies is considered
    to be a "stupid legacy" issue (Linus's words) in this regard; fixing it
    would be more trouble than it is worth.
    (jiffies 变量的特殊之处在于它每次被引用时都可能有不同的值，但无需任何特殊加锁即可读取。因此 jiffies 可以是 volatile 的，但强烈不建议添加其他这种类型的变量。在这方面，Jiffies 被认为是一个"愚蠢的遗留"问题（Linus 的原话）；修复它带来的麻烦超过其价值。)

  - Pointers to data structures in coherent memory which might be modified
    by I/O devices can, sometimes, legitimately be volatile.  A ring buffer
    used by a network adapter, where that adapter changes pointers to
    indicate which descriptors have been processed, is an example of this
    type of situation.
    (指向一致性内存中的数据结构、且可能被 I/O 设备修改的指针，有时可以合法地是 volatile 的。网络适配器使用的环形缓冲区就是这种情形的一个例子，该适配器通过更改指针来指示哪些描述符已被处理。)

For most code, none of the above justifications for volatile apply.  As a
result, the use of volatile is likely to be seen as a bug and will bring
additional scrutiny to the code.  Developers who are tempted to use
volatile should take a step back and think about what they are truly trying
to accomplish.
(对于大多数代码，上述为 volatile 辩护的理由都不适用。因此，volatile 的使用很可能被视为缺陷，并会给代码带来额外的审查。想使用 volatile 的开发者应该退后一步，思考他们真正想完成的是什么。)

Patches to remove volatile variables are generally welcome - as long as
they come with a justification which shows that the concurrency issues have
been properly thought through.
(移除 volatile 变量的补丁通常是受欢迎的——只要它们附带说明并发问题已被仔细考虑的理由。)


References(参考文献)
====================

[1] https://lwn.net/Articles/233481/

[2] https://lwn.net/Articles/233482/

Credits(致谢)
=============

Original impetus and research by Randy Dunlap
(最初的推动和研究由 Randy Dunlap 完成)

Written by Jonathan Corbet
(由 Jonathan Corbet 撰写)

Improvements via comments from Satyam Sharma, Johannes Stezenbach, Jesper
Juhl, Heikki Orsila, H. Peter Anvin, Philipp Hahn, and Stefan
Richter.
(Satyam Sharma、Johannes Stezenbach、Jesper Juhl、Heikki Orsila、H. Peter Anvin、Philipp Hahn 和 Stefan Richter 通过评论提供了改进。)
