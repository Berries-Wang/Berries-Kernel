==================================
this_cpu operations(this_cpu 操作)
==================================

:Author: Christoph Lameter, August 4th, 2014
(作者: Christoph Lameter，2014 年 8 月 4 日)
:Author: Pranith Kumar, Aug 2nd, 2014
(作者: Pranith Kumar，2014 年 8 月 2 日)

this_cpu operations are a way of optimizing access to per cpu
variables associated with the *currently* executing processor. This is
done through the use of segment registers (or a dedicated register where
the cpu permanently stored the beginning of the per cpu	area for a
specific processor).
(this_cpu 操作是一种优化对与 *当前* 执行处理器相关联的 per cpu 变量的
访问的方式。这是通过使用段寄存器 (或 cpu 永久保存特定处理器的 per cpu
区域起始地址的专用寄存器) 来实现的。)

this_cpu operations add a per cpu variable offset to the processor
specific per cpu base and encode that operation in the instruction
operating on the per cpu variable.
(this_cpu 操作把 per cpu 变量的偏移加到处理器特定的 per cpu 基址上，并
把该操作编码在操作 per cpu 变量的指令中。)

This means that there are no atomicity issues between the calculation of
the offset and the operation on the data. Therefore it is not
necessary to disable preemption or interrupts to ensure that the
processor is not changed between the calculation of the address and
the operation on the data.
(这意味着在偏移的计算与对数据的操作之间不存在原子性问题。因此，不必为
了确保处理器在地址计算与数据操作之间不发生改变而禁用抢占或中断。)

Read-modify-write operations are of particular interest. Frequently
processors have special lower latency instructions that can operate
without the typical synchronization overhead, but still provide some
sort of relaxed atomicity guarantees. The x86, for example, can execute
RMW (Read Modify Write) instructions like inc/dec/cmpxchg without the
lock prefix and the associated latency penalty.
(读-改-写 (RMW) 操作尤其值得关注。处理器通常有特殊的低延迟指令，可以在
没有典型同步开销的情况下操作，但仍提供某种宽松的原子性保证。例如，x86
可以执行 inc/dec/cmpxchg 之类的 RMW (Read Modify Write) 指令，而无需
lock 前缀及其相关的延迟代价。)

Access to the variable without the lock prefix is not synchronized but
synchronization is not necessary since we are dealing with per cpu
data specific to the currently executing processor. Only the current
processor should be accessing that variable and therefore there are no
concurrency issues with other processors in the system.
(不带 lock 前缀对变量的访问不是同步的，但同步并不必要，因为我们处理的
是特定于当前执行处理器的 per cpu 数据。只有当前处理器应当访问该变量，因
此与系统中其他处理器不存在并发问题。)

Please note that accesses by remote processors to a per cpu area are
exceptional situations and may impact performance and/or correctness
(remote write operations) of local RMW operations via this_cpu_*.
(请注意，远程处理器对 per cpu 区域的访问属于例外情形，并可能影响经由
this_cpu_* 的本地 RMW 操作的性能和/或正确性 (远程写操作)。)

The main use of the this_cpu operations has been to optimize counter
operations.
(this_cpu 操作的主要用途是优化计数器操作。)

The following this_cpu() operations with implied preemption protection
are defined. These operations can be used without worrying about
preemption and interrupts::

	this_cpu_read(pcp)
	this_cpu_write(pcp, val)
	this_cpu_add(pcp, val)
	this_cpu_and(pcp, val)
	this_cpu_or(pcp, val)
	this_cpu_add_return(pcp, val)
	this_cpu_xchg(pcp, nval)
	this_cpu_cmpxchg(pcp, oval, nval)
	this_cpu_sub(pcp, val)
	this_cpu_inc(pcp)
	this_cpu_dec(pcp)
	this_cpu_sub_return(pcp, val)
	this_cpu_inc_return(pcp)
	this_cpu_dec_return(pcp)

(下面定义了以下带隐式抢占保护的 this_cpu() 操作。使用这些操作时无需担
心抢占和中断：)


Inner working of this_cpu operations(this_cpu 操作的内部原理)
-------------------------------------------------------------

On x86 the fs: or the gs: segment registers contain the base of the
per cpu area. It is then possible to simply use the segment override
to relocate a per cpu relative address to the proper per cpu area for
the processor. So the relocation to the per cpu base is encoded in the
instruction via a segment register prefix.
(在 x86 上，fs: 或 gs: 段寄存器包含 per cpu 区域的基址。于是可以简单地
使用段覆盖把 per cpu 相对地址重定位到该处理器正确的 per cpu 区域。因
此，到 per cpu 基址的重定位通过段寄存器前缀编码在指令中。)

For example::

	DEFINE_PER_CPU(int, x);
	int z;

	z = this_cpu_read(x);

(例如：)

results in a single instruction::

	mov ax, gs:[x]

(将产生单条指令：)

instead of a sequence of calculation of the address and then a fetch
from that address which occurs with the per cpu operations. Before
this_cpu_ops such sequence also required preempt disable/enable to
prevent the kernel from moving the thread to a different processor
while the calculation is performed.
(而不是像使用 per cpu 操作时那样，先计算地址再从该地址取值的指令序
列。在 this_cpu_ops 出现之前，这样的序列还需要禁止/恢复抢占，以防止在
执行计算时内核把线程移到另一个处理器。)

Consider the following this_cpu operation::

	this_cpu_inc(x)

(考虑以下 this_cpu 操作：)

The above results in the following single instruction (no lock prefix!)::

	inc gs:[x]

(上述操作产生以下单条指令 (没有 lock 前缀！)：)

instead of the following operations required if there is no segment
register::

	int *y;
	int cpu;

	cpu = get_cpu();
	y = per_cpu_ptr(&x, cpu);
	(*y)++;
	put_cpu();

(而不是在没有段寄存器时所需的以下操作序列：)

Note that these operations can only be used on per cpu data that is
reserved for a specific processor. Without disabling preemption in the
surrounding code this_cpu_inc() will only guarantee that one of the
per cpu counters is correctly incremented. However, there is no
guarantee that the OS will not move the process directly before or
after the this_cpu instruction is executed. In general this means that
the value of the individual counters for each processor are
meaningless. The sum of all the per cpu counters is the only value
that is of interest.
(注意，这些操作只能用于为特定处理器保留的 per cpu 数据。如果不在周围
代码中禁用抢占，this_cpu_inc() 只能保证某个 per cpu 计数器被正确递增。
但是，无法保证 OS 不会恰好在 this_cpu 指令执行之前或之后移动进程。一般
而言，这意味着每个处理器的各个计数器的值都是无意义的。所有 per cpu 计
数器的总和才是唯一有意义的值。)

Per cpu variables are used for performance reasons. Bouncing cache
lines can be avoided if multiple processors concurrently go through
the same code paths.  Since each processor has its own per cpu
variables no concurrent cache line updates take place. The price that
has to be paid for this optimization is the need to add up the per cpu
counters when the value of a counter is needed.
(使用 per cpu 变量是出于性能原因。如果多个处理器并发地执行相同的代码路
径，可以避免缓存行弹跳。由于每个处理器有自己的 per cpu 变量，不会发生
并发的缓存行更新。为这一优化付出的代价是，当需要计数器的值时，必须把各
per cpu 计数器累加起来。)


Special operations(特殊操作)
----------------------------

::

	y = this_cpu_ptr(&x)

Takes the offset of a per cpu variable (&x !) and returns the address
of the per cpu variable that belongs to the currently executing
processor.  this_cpu_ptr avoids multiple steps that the common
get_cpu/put_cpu sequence requires. No processor number is
available. Instead, the offset of the local per cpu area is simply
added to the per cpu offset.
(它取一个 per cpu 变量的偏移 (&x ！) 并返回属于当前执行处理器的 per
cpu 变量的地址。this_cpu_ptr 避免了常见的 get_cpu/put_cpu 序列所需的
多个步骤。这里没有处理器编号可用，而是简单地把本地 per cpu 区域的偏移
加到 per cpu 偏移上。)

Note that this operation can only be used in code segments where
smp_processor_id() may be used, for example, where preemption has been
disabled. The pointer is then used to access local per cpu data in a
critical section. When preemption is re-enabled this pointer is usually
no longer useful since it may no longer point to per cpu data of the
current processor.
(注意，此操作只能用于可以使用 smp_processor_id() 的代码段中，例如已
禁用抢占的地方。随后该指针用于在临界区内访问本地 per cpu 数据。当重新
启用抢占时，这个指针通常不再有用，因为它可能不再指向当前处理器的 per
cpu 数据。)

The special cases where it makes sense to obtain a per-CPU pointer in
preemptible code are addressed by raw_cpu_ptr(), but such use cases need
to handle cases where two different CPUs are accessing the same per cpu
variable, which might well be that of a third CPU.  These use cases are
typically performance optimizations.  For example, SRCU implements a pair
of counters as a pair of per-CPU variables, and rcu_read_lock_nmisafe()
uses raw_cpu_ptr() to get a pointer to some CPU's counter, and uses
atomic_inc_long() to handle migration between the raw_cpu_ptr() and
the atomic_inc_long().
(在可抢占代码中获取 per-CPU 指针确实有意义的特殊情况由 raw_cpu_ptr()
处理，但这类用例需要处理两个不同 CPU 访问同一 per cpu 变量的情况——该
变量很可能是第三个 CPU 的。这类用例通常是性能优化。例如，SRCU 把一对
计数器实现为一对 per-CPU 变量，rcu_read_lock_nmisafe() 使用
raw_cpu_ptr() 获取指向某个 CPU 计数器的指针，并使用 atomic_inc_long()
来处理 raw_cpu_ptr() 与 atomic_inc_long() 之间可能发生的迁移。)

Per cpu variables and offsets(per cpu 变量与偏移)
-------------------------------------------------

Per cpu variables have *offsets* to the beginning of the per cpu
area. They do not have addresses although they look like that in the
code. Offsets cannot be directly dereferenced. The offset must be
added to a base pointer of a per cpu area of a processor in order to
form a valid address.
(per cpu 变量拥有指向 per cpu 区域起始处的 *偏移* 。它们没有地址，尽
管在代码里看起来像是那样。偏移不能被直接解引用。必须把偏移加到某处理器
per cpu 区域的基址指针上，才能形成有效地址。)

Therefore the use of x or &x outside of the context of per cpu
operations is invalid and will generally be treated like a NULL
pointer dereference.
(因此，在 per cpu 操作的上下文之外使用 x 或 &x 是无效的，通常会被当
成 NULL 指针解引用来处理。)

::

	DEFINE_PER_CPU(int, x);

In the context of per cpu operations the above implies that x is a per
cpu variable. Most this_cpu operations take a cpu variable.
(在 per cpu 操作的上下文中，上面的声明意味着 x 是一个 per cpu 变量。
大多数 this_cpu 操作接受一个 cpu 变量。)

::

	int __percpu *p = &x;

&x and hence p is the *offset* of a per cpu variable. this_cpu_ptr()
takes the offset of a per cpu variable which makes this look a bit
strange.
(&x 因而 p 是 per cpu 变量的 *偏移* 。this_cpu_ptr() 接受 per cpu 变
量的偏移，这使它看起来有点奇怪。)


Operations on a field of a per cpu structure(对 per cpu 结构体字段的操作)
-------------------------------------------------------------------------

Let's say we have a percpu structure::

	struct s {
		int n,m;
	};

	DEFINE_PER_CPU(struct s, p);

(假设我们有一个 percpu 结构：)


Operations on these fields are straightforward::

	this_cpu_inc(p.m)

	z = this_cpu_cmpxchg(p.m, 0, 1);

(对这些字段的操作很直接：)


If we have an offset to struct s::

	struct s __percpu *ps = &p;

	this_cpu_dec(ps->m);

	z = this_cpu_inc_return(ps->n);

(如果我们有一个指向 struct s 的偏移：)


The calculation of the pointer may require the use of this_cpu_ptr()
if we do not make use of this_cpu ops later to manipulate fields::

	struct s *pp;

	pp = this_cpu_ptr(&p);

	pp->m--;

	z = pp->n++;

(如果我们之后不使用 this_cpu ops 来操作字段，那么指针的计算可能需要
使用 this_cpu_ptr()：)


Variants of this_cpu ops(this_cpu ops 的变体)
---------------------------------------------

this_cpu ops are interrupt safe. Some architectures do not support
these per cpu local operations. In that case the operation must be
replaced by code that disables interrupts, then does the operations
that are guaranteed to be atomic and then re-enable interrupts. Doing
so is expensive. If there are other reasons why the scheduler cannot
change the processor we are executing on then there is no reason to
disable interrupts. For that purpose the following __this_cpu operations
are provided.
(this_cpu ops 是中断安全的。有些架构不支持这些 per cpu 本地操作。在
那种情况下，必须把该操作替换为以下代码：先禁用中断，然后执行保证原子性
的操作，再重新启用中断。这样做的代价很高。如果有其他原因保证调度器无法
改变我们正在其上执行的处理器，那么就没有理由禁用中断。为此，提供了以下
__this_cpu 操作。)

These operations have no guarantee against concurrent interrupts or
preemption. If a per cpu variable is not used in an interrupt context
and the scheduler cannot preempt, then they are safe. If any interrupts
still occur while an operation is in progress and if the interrupt too
modifies the variable, then RMW actions can not be guaranteed to be
safe::

	__this_cpu_read(pcp)
	__this_cpu_write(pcp, val)
	__this_cpu_add(pcp, val)
	__this_cpu_and(pcp, val)
	__this_cpu_or(pcp, val)
	__this_cpu_add_return(pcp, val)
	__this_cpu_xchg(pcp, nval)
	__this_cpu_cmpxchg(pcp, oval, nval)
	__this_cpu_sub(pcp, val)
	__this_cpu_inc(pcp)
	__this_cpu_dec(pcp)
	__this_cpu_sub_return(pcp, val)
	__this_cpu_inc_return(pcp)
	__this_cpu_dec_return(pcp)

(这些操作对并发中断或抢占没有任何保证。如果某个 per cpu 变量不在中断
上下文中使用、且调度器无法抢占，那么它们是安全的。如果在操作进行期间
仍发生中断、且中断也修改该变量，则无法保证 RMW 动作是安全的。)


Will increment x and will not fall-back to code that disables
interrupts on platforms that cannot accomplish atomicity through
address relocation and a Read-Modify-Write operation in the same
instruction.
(它将递增 x，并且在无法通过地址重定位和同一指令内的读-改-写操作实现原
子性的平台上，不会回退到禁用中断的代码。)


&this_cpu_ptr(pp)->n vs this_cpu_ptr(&pp->n)(两种形式的对比)
------------------------------------------------------------

The first operation takes the offset and forms an address and then
adds the offset of the n field. This may result in two add
instructions emitted by the compiler.
(第一种操作取偏移、形成地址，然后加上 n 字段的偏移。这可能导致编译器
生成两条加法指令。)

The second one first adds the two offsets and then does the
relocation.  IMHO the second form looks cleaner and has an easier time
with (). The second form also is consistent with the way
this_cpu_read() and friends are used.
(第二种先把两个偏移相加，然后再做重定位。依我之见，第二种形式看起来更
整洁，而且处理 () 时更轻松。第二种形式还与 this_cpu_read() 等函数的使
用方式保持一致。)


Remote access to per cpu data(远程访问 per cpu 数据)
----------------------------------------------------

Per cpu data structures are designed to be used by one cpu exclusively.
If you use the variables as intended, this_cpu_ops() are guaranteed to
be "atomic" as no other CPU has access to these data structures.
(per cpu 数据结构被设计为由一个 cpu 独占使用。如果按预期使用这些变量，
this_cpu_ops() 保证是"原子"的，因为没有其他 CPU 能访问这些数据结构。)

There are special cases where you might need to access per cpu data
structures remotely. It is usually safe to do a remote read access
and that is frequently done to summarize counters. Remote write access
something which could be problematic because this_cpu ops do not
have lock semantics. A remote write may interfere with a this_cpu
RMW operation.
(有些特殊情况可能需要远程访问 per cpu 数据结构。远程读访问通常是安全
的，而且经常用于汇总计数器。远程写访问则可能有问题，因为 this_cpu ops
没有锁语义。远程写可能与 this_cpu RMW 操作相互干扰。)

Remote write accesses to percpu data structures are highly discouraged
unless absolutely necessary. Please consider using an IPI to wake up
the remote CPU and perform the update to its per cpu area.
(除非绝对必要，强烈不建议对 percpu 数据结构进行远程写访问。请考虑使
用 IPI 唤醒远程 CPU 并在其 per cpu 区域上执行更新。)

To access per-cpu data structure remotely, typically the per_cpu_ptr()
function is used::


	DEFINE_PER_CPU(struct data, datap);

	struct data *p = per_cpu_ptr(&datap, cpu);

(要远程访问 per-cpu 数据结构，通常使用 per_cpu_ptr() 函数：)

This makes it explicit that we are getting ready to access a percpu
area remotely.
(这明确表明我们准备远程访问一个 percpu 区域。)

You can also do the following to convert the datap offset to an address::

	struct data *p = this_cpu_ptr(&datap);

(你也可以用以下方式把 datap 偏移转换为地址：)

but, passing of pointers calculated via this_cpu_ptr to other cpus is
unusual and should be avoided.
(但是，把通过 this_cpu_ptr 计算得到的指针传给其他 cpu 并不常见，应当
避免。)

Remote access are typically only for reading the status of another cpus
per cpu data. Write accesses can cause unique problems due to the
relaxed synchronization requirements for this_cpu operations.
(远程访问通常只用于读取其他 cpu 的 per cpu 数据的状态。由于 this_cpu
操作采用宽松的同步要求，写访问可能造成独特的问题。)

One example that illustrates some concerns with write operations is
the following scenario that occurs because two per cpu variables
share a cache-line but the relaxed synchronization is applied to
only one process updating the cache-line.
(一个能说明写操作某些隐患的例子是以下场景：两个 per cpu 变量共享一条
缓存行，但宽松同步只应用于更新该缓存行的其中一个进程。)

Consider the following example::


	struct test {
		atomic_t a;
		int b;
	};

	DEFINE_PER_CPU(struct test, onecacheline);

(考虑以下示例：)

There is some concern about what would happen if the field 'a' is updated
remotely from one processor and the local processor would use this_cpu ops
to update field b. Care should be taken that such simultaneous accesses to
data within the same cache line are avoided. Also costly synchronization
may be necessary. IPIs are generally recommended in such scenarios instead
of a remote write to the per cpu area of another processor.
(如果字段 'a' 被一个处理器远程更新，而本地处理器使用 this_cpu ops 更
新字段 b，会发生什么，这是需要关注的问题。应当注意避免对同一缓存行内数
据的这种同时访问。此外可能需要代价高昂的同步。在这种场景下，一般推荐使
用 IPI，而不是对另一个处理器的 per cpu 区域进行远程写。)

Even in cases where the remote writes are rare, please bear in
mind that a remote write will evict the cache line from the processor
that most likely will access it. If the processor wakes up and finds a
missing local cache line of a per cpu area, its performance and hence
the wake up times will be affected.
(即使在远程写很少发生的情况下，也请记住：远程写会把缓存行从最有可能访
问它的处理器中逐出。如果该处理器唤醒后发现其 per cpu 区域的本地缓存行
缺失，其性能乃至唤醒时间都会受影响。)
