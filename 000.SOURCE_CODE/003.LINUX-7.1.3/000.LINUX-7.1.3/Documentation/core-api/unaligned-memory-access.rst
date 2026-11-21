=========================================
Unaligned Memory Accesses(非对齐内存访问)
=========================================

:Author: Daniel Drake <dsd@gentoo.org>,
  (作者：Daniel Drake <dsd@gentoo.org>，)
:Author: Johannes Berg <johannes@sipsolutions.net>
  (作者：Johannes Berg <johannes@sipsolutions.net>)

:With help from: Alan Cox, Avuton Olrich, Heikki Orsila, Jan Engelhardt,
  Kyle McMartin, Kyle Moffett, Randy Dunlap, Robert Hancock, Uli Kunitz,
  Vadim Lobanov
  (特别感谢：Alan Cox、Avuton Olrich、Heikki Orsila、Jan Engelhardt、
  Kyle McMartin、Kyle Moffett、Randy Dunlap、Robert Hancock、Uli Kunitz、
  Vadim Lobanov)


Linux runs on a wide variety of architectures which have varying behaviour
when it comes to memory access. This document presents some details about
unaligned accesses, why you need to write code that doesn't cause them,
and how to write such code!
(Linux 运行在各种各样的体系结构上，这些体系结构在内存访问方面的行为各不相同。
本文档介绍关于非对齐访问的一些细节、为什么需要编写不会引起非对齐访问的代码，
以及如何编写这样的代码！)


The definition of an unaligned access(非对齐访问的定义)
=======================================================

Unaligned memory accesses occur when you try to read N bytes of data starting
from an address that is not evenly divisible by N (i.e. addr % N != 0).
For example, reading 4 bytes of data from address 0x10004 is fine, but
reading 4 bytes of data from address 0x10005 would be an unaligned memory
access.
(当你试图从一个不能被 N 整除的地址(即 addr % N != 0)开始读取 N 字节数据时，
就会发生非对齐内存访问。例如，从地址 0x10004 读取 4 字节数据没有问题，但从
地址 0x10005 读取 4 字节数据就是一次非对齐内存访问。)

The above may seem a little vague, as memory access can happen in different
ways. The context here is at the machine code level: certain instructions read
or write a number of bytes to or from memory (e.g. movb, movw, movl in x86
assembly). As will become clear, it is relatively easy to spot C statements
which will compile to multiple-byte memory access instructions, namely when
dealing with types such as u16, u32 and u64.
(上面的说法可能显得有点模糊，因为内存访问可以以不同的方式发生。这里的语境是
机器码层面：某些指令从内存读取或向内存写入若干字节(例如 x86 汇编中的 movb、
movw、movl)。后面会清楚，识别会编译成多字节内存访问指令的 C 语句相对容易，
即在处理 u16、u32 和 u64 这类类型的时候。)


Natural alignment(自然对齐)
===========================

The rule mentioned above forms what we refer to as natural alignment:
When accessing N bytes of memory, the base memory address must be evenly
divisible by N, i.e. addr % N == 0.
(前面提到的规则构成了我们所说的自然对齐 (natural alignment)：访问 N 字节内存
时，基地址必须能被 N 整除，即 addr % N == 0。)

When writing code, assume the target architecture has natural alignment
requirements.
(编写代码时，假定目标体系结构有自然对齐要求。)

In reality, only a few architectures require natural alignment on all sizes
of memory access. However, we must consider ALL supported architectures;
writing code that satisfies natural alignment requirements is the easiest way
to achieve full portability.
(实际上，只有少数体系结构对所有大小的内存访问都要求自然对齐。但我们必须考虑
所有受支持的体系结构；编写满足自然对齐要求的代码是实现完全可移植性的最简单
途径。)


Why unaligned access is bad(为什么非对齐访问有害)
=================================================

The effects of performing an unaligned memory access vary from architecture
to architecture. It would be easy to write a whole document on the differences
here; a summary of the common scenarios is presented below:
(执行非对齐内存访问的后果因体系结构而异。就此差异写一篇完整文档都很容易；
下面给出常见情形的概要：)

 - Some architectures are able to perform unaligned memory accesses
   transparently, but there is usually a significant performance cost.
   (一些体系结构能够透明地执行非对齐内存访问，但通常有显著的性能代价。)
 - Some architectures raise processor exceptions when unaligned accesses
   happen. The exception handler is able to correct the unaligned access,
   at significant cost to performance.
   (一些体系结构在发生非对齐访问时会触发处理器异常。异常处理程序能够纠正该
   非对齐访问，但要付出显著的性能代价。)
 - Some architectures raise processor exceptions when unaligned accesses
   happen, but the exceptions do not contain enough information for the
   unaligned access to be corrected.
   (一些体系结构在发生非对齐访问时会触发处理器异常，但异常中不包含足够的
   信息来纠正该非对齐访问。)
 - Some architectures are not capable of unaligned memory access, but will
   silently perform a different memory access to the one that was requested,
   resulting in a subtle code bug that is hard to detect!
   (一些体系结构不具备非对齐内存访问能力，但会悄无声息地执行一个与所请求
   不同的内存访问，从而造成难以察觉的隐蔽代码 bug！)

It should be obvious from the above that if your code causes unaligned
memory accesses to happen, your code will not work correctly on certain
platforms and will cause performance problems on others.
(从上面的内容应当可以明显看出：如果你的代码引起非对齐内存访问，它将无法在某些
平台上正确运行，并在另一些平台上造成性能问题。)


Code that does not cause unaligned access(不会引起非对齐访问的代码)
===================================================================

At first, the concepts above may seem a little hard to relate to actual
coding practice. After all, you don't have a great deal of control over
memory addresses of certain variables, etc.
(乍看之下，上述概念似乎难以与实际编码实践联系起来。毕竟，你对某些变量的内存
地址并没有多少控制权。)

Fortunately things are not too complex, as in most cases, the compiler
ensures that things will work for you. For example, take the following
structure::

	struct foo {
		u16 field1;
		u32 field2;
		u8 field3;
	};

(幸运的是，事情并不太复杂，因为在大多数情况下，编译器会确保一切正常。例如，
看看下面这个结构体：)

Let us assume that an instance of the above structure resides in memory
starting at address 0x10000. With a basic level of understanding, it would
not be unreasonable to expect that accessing field2 would cause an unaligned
access. You'd be expecting field2 to be located at offset 2 bytes into the
structure, i.e. address 0x10002, but that address is not evenly divisible
by 4 (remember, we're reading a 4 byte value here).
(让我们假定上述结构体的一个实例从地址 0x10000 开始驻留在内存中。以初级的理解
水平，预期访问 field2 会引起非对齐访问并不过分。你会以为 field2 位于结构体内
偏移 2 字节处，即地址 0x10002，但该地址不能被 4 整除(记住，我们这里读取的是
4 字节的值)。)

Fortunately, the compiler understands the alignment constraints, so in the
above case it would insert 2 bytes of padding in between field1 and field2.
Therefore, for standard structure types you can always rely on the compiler
to pad structures so that accesses to fields are suitably aligned (assuming
you do not cast the field to a type of different length).
(幸运的是，编译器理解对齐约束，所以在上述情况下，它会在 field1 与 field2 之间
插入 2 字节的填充。因此，对于标准的结构体类型，你总是可以依赖编译器对结构体
进行填充，使对字段的访问恰当地对齐(前提是你没有把字段强制转换为不同长度的
类型)。)

Similarly, you can also rely on the compiler to align variables and function
parameters to a naturally aligned scheme, based on the size of the type of
the variable.
(类似地，你也可以依赖编译器根据变量类型的大小，把变量和函数参数按自然对齐方案
对齐。)

At this point, it should be clear that accessing a single byte (u8 or char)
will never cause an unaligned access, because all memory addresses are evenly
divisible by one.
(到这里应当清楚，访问单个字节 (u8 或 char) 永远不会引起非对齐访问，因为所有
内存地址都能被 1 整除。)

On a related topic, with the above considerations in mind you may observe
that you could reorder the fields in the structure in order to place fields
where padding would otherwise be inserted, and hence reduce the overall
resident memory size of structure instances. The optimal layout of the
above example is::

	struct foo {
		u32 field2;
		u16 field1;
		u8 field3;
	};

(在相关的话题上，带着上述考虑，你可能会注意到可以重新排列结构体中的字段，把
字段放到本应插入填充的位置，从而减小结构体实例的常驻内存总大小。上面例子的
最优布局是：)

For a natural alignment scheme, the compiler would only have to add a single
byte of padding at the end of the structure. This padding is added in order
to satisfy alignment constraints for arrays of these structures.
(对自然对齐方案而言，编译器只需在结构体末尾添加一个字节的填充。添加这些填充
是为了满足由这些结构体组成的数组的对齐约束。)

Another point worth mentioning is the use of __attribute__((packed)) on a
structure type. This GCC-specific attribute tells the compiler never to
insert any padding within structures, useful when you want to use a C struct
to represent some data that comes in a fixed arrangement 'off the wire'.
(另一个值得一提的点是结构体类型上 __attribute__((packed)) 的使用。这个 GCC
特有的属性告诉编译器永远不要在结构体内插入任何填充，当你想用 C struct 表示以
固定布局"从线路上"传来的数据时，这很有用。)

You might be inclined to believe that usage of this attribute can easily
lead to unaligned accesses when accessing fields that do not satisfy
architectural alignment requirements. However, again, the compiler is aware
of the alignment constraints and will generate extra instructions to perform
the memory access in a way that does not cause unaligned access. Of course,
the extra instructions obviously cause a loss in performance compared to the
non-packed case, so the packed attribute should only be used when avoiding
structure padding is of importance.
(你可能倾向于认为，使用该属性会在访问不满足体系结构对齐要求的字段时轻易导致
非对齐访问。然而，同样地，编译器知道对齐约束，会生成额外的指令，以不会引起
非对齐访问的方式执行内存访问。当然，与非打包情形相比，额外的指令显然会造成
性能损失，所以只有当避免结构体填充确实重要时才应使用 packed 属性。)


Code that causes unaligned access(会引起非对齐访问的代码)
=========================================================

With the above in mind, let's move onto a real life example of a function
that can cause an unaligned memory access. The following function taken
from include/linux/etherdevice.h is an optimized routine to compare two
ethernet MAC addresses for equality::

  bool ether_addr_equal(const u8 *addr1, const u8 *addr2)
  {
  #ifdef CONFIG_HAVE_EFFICIENT_UNALIGNED_ACCESS
	u32 fold = ((*(const u32 *)addr1) ^ (*(const u32 *)addr2)) |
		   ((*(const u16 *)(addr1 + 4)) ^ (*(const u16 *)(addr2 + 4)));

	return fold == 0;
  #else
	const u16 *a = (const u16 *)addr1;
	const u16 *b = (const u16 *)addr2;
	return ((a[0] ^ b[0]) | (a[1] ^ b[1]) | (a[2] ^ b[2])) == 0;
  #endif
  }

(带着上述认识，我们来看一个现实中可能引起非对齐内存访问的函数例子。取自
include/linux/etherdevice.h 的下面这个函数，是一个比较两个以太网 MAC 地址
是否相等的优化例程：)

In the above function, when the hardware has efficient unaligned access
capability, there is no issue with this code.  But when the hardware isn't
able to access memory on arbitrary boundaries, the reference to a[0] causes
2 bytes (16 bits) to be read from memory starting at address addr1.
(在上面的函数中，当硬件具备高效的非对齐访问能力时，这段代码没有问题。但当
硬件无法在任意边界上访问内存时，对 a[0] 的引用会导致从地址 addr1 开始的
2 字节 (16 位) 内存读取。)

Think about what would happen if addr1 was an odd address such as 0x10003.
(Hint: it'd be an unaligned access.)
(想一想，如果 addr1 是诸如 0x10003 这样的奇数地址会发生什么。
(提示：那将是一次非对齐访问。))

Despite the potential unaligned access problems with the above function, it
is included in the kernel anyway but is understood to only work normally on
16-bit-aligned addresses. It is up to the caller to ensure this alignment or
not use this function at all. This alignment-unsafe function is still useful
as it is a decent optimization for the cases when you can ensure alignment,
which is true almost all of the time in ethernet networking context.
(尽管上述函数存在潜在的非对齐访问问题，它仍然被包含在内核中，但被理解为只在
16 位对齐的地址上才能正常工作。调用者要负责确保这种对齐，或者干脆不使用这个
函数。这个对齐不安全的函数依然有用，因为在你能确保对齐的情形下，它是一个不错
的优化——在以太网网络环境中，几乎总是如此。)


Here is another example of some code that could cause unaligned accesses::

	void myfunc(u8 *data, u32 value)
	{
		[...]
		*((u32 *) data) = cpu_to_le32(value);
		[...]
	}

(下面是另一个可能引起非对齐访问的代码示例：)

This code will cause unaligned accesses every time the data parameter points
to an address that is not evenly divisible by 4.
(每当 data 参数指向不能被 4 整除的地址时，这段代码就会引起非对齐访问。)

In summary, the 2 main scenarios where you may run into unaligned access
problems involve:
(总而言之，你可能遇到非对齐访问问题的两大场景是：)

 1. Casting variables to types of different lengths
    (把变量强制转换为不同长度的类型)
 2. Pointer arithmetic followed by access to at least 2 bytes of data
    (先做指针运算，然后访问至少 2 字节的数据)


Avoiding unaligned accesses(避免非对齐访问)
===========================================

The easiest way to avoid unaligned access is to use the get_unaligned() and
put_unaligned() macros provided by the <linux/unaligned.h> header file.
(避免非对齐访问最简单的方法，是使用 <linux/unaligned.h> 头文件提供的
get_unaligned() 与 put_unaligned() 宏。)

Going back to an earlier example of code that potentially causes unaligned
access::

	void myfunc(u8 *data, u32 value)
	{
		[...]
		*((u32 *) data) = cpu_to_le32(value);
		[...]
	}

(回到前面那个可能引起非对齐访问的代码示例：)

To avoid the unaligned memory access, you would rewrite it as follows::

	void myfunc(u8 *data, u32 value)
	{
		[...]
		value = cpu_to_le32(value);
		put_unaligned(value, (u32 *) data);
		[...]
	}

(为避免非对齐内存访问，可以这样改写：)

The get_unaligned() macro works similarly. Assuming 'data' is a pointer to
memory and you wish to avoid unaligned access, its usage is as follows::

	u32 value = get_unaligned((u32 *) data);

(get_unaligned() 宏的工作方式类似。假定 'data' 是指向内存的指针，而你想避免
非对齐访问，其用法如下：)

These macros work for memory accesses of any length (not just 32 bits as
in the examples above). Be aware that when compared to standard access of
aligned memory, using these macros to access unaligned memory can be costly in
terms of performance.
(这些宏适用于任意长度的内存访问(不只是上面例子中的 32 位)。注意，与标准的
对齐内存访问相比，使用这些宏访问非对齐内存在性能上可能代价高昂。)

If use of such macros is not convenient, another option is to use memcpy(),
where the source or destination (or both) are of type u8* or unsigned char*.
Due to the byte-wise nature of this operation, unaligned accesses are avoided.
(如果使用这类宏不太方便，另一种选择是使用 memcpy()，让源或目的(或两者)的类型
为 u8* 或 unsigned char*。由于该操作按字节逐个进行，非对齐访问得以避免。)


Alignment vs. Networking(对齐与网络)
====================================

On architectures that require aligned loads, networking requires that the IP
header is aligned on a four-byte boundary to optimise the IP stack. For
regular ethernet hardware, the constant NET_IP_ALIGN is used. On most
architectures this constant has the value 2 because the normal ethernet
header is 14 bytes long, so in order to get proper alignment one needs to
DMA to an address which can be expressed as 4*n + 2. One notable exception
here is powerpc which defines NET_IP_ALIGN to 0 because DMA to unaligned
addresses can be very expensive and dwarf the cost of unaligned loads.
(在要求对齐加载的体系结构上，网络子系统要求 IP 头按四字节边界对齐，以优化 IP
协议栈。对于常规以太网硬件，使用常量 NET_IP_ALIGN。在大多数体系结构上该常量
取值为 2，因为常规以太网头长 14 字节，所以要获得恰当的对齐，需要 DMA 到可以
表示为 4*n + 2 的地址。一个值得注意的例外是 powerpc，它把 NET_IP_ALIGN 定义为
0，因为 DMA 到非对齐地址可能非常昂贵，其代价会远远盖过非对齐加载的代价。)

For some ethernet hardware that cannot DMA to unaligned addresses like
4*n+2 or non-ethernet hardware, this can be a problem, and it is then
required to copy the incoming frame into an aligned buffer. Because this is
unnecessary on architectures that can do unaligned accesses, the code can be
made dependent on CONFIG_HAVE_EFFICIENT_UNALIGNED_ACCESS like so::

(对于某些不能 DMA 到 4*n+2 之类非对齐地址的以太网硬件，或者非以太网硬件，这就
可能成为问题，此时需要把收到的帧复制到一个对齐的缓冲区中。由于在能够进行非对齐
访问的体系结构上这是不必要的，可以像下面这样让代码依赖于

CONFIG_HAVE_EFFICIENT_UNALIGNED_ACCESS：)

	#ifdef CONFIG_HAVE_EFFICIENT_UNALIGNED_ACCESS
		skb = original skb
	#else
		skb = copy skb
	#endif
