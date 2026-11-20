.. SPDX-License-Identifier: GPL-2.0

.. _deprecated:

=======================================================================================================================
Deprecated Interfaces, Language Features, Attributes, and Conventions(已废弃的接口、语言特性、属性与约定)
=======================================================================================================================

In a perfect world, it would be possible to convert all instances of
some deprecated API into the new API and entirely remove the old API in
a single development cycle. However, due to the size of the kernel, the
maintainership hierarchy, and timing, it's not always feasible to do these
kinds of conversions at once. This means that new instances may sneak into
the kernel while old ones are being removed, only making the amount of
work to remove the API grow. In order to educate developers about what
has been deprecated and why, this list has been created as a place to
point when uses of deprecated things are proposed for inclusion in the
kernel.
(在理想世界中，人们可以在单个开发周期内把某个已废弃 API 的所有实例都转换为新 API，并彻底移除旧 API。然而，由于内核的规模、维护层级结构以及时间安排，这类一次性转换并非总是可行。这意味着在旧实例被移除的同时，新实例仍可能悄然混入内核，只会让移除该 API 的工作量不断增长。为了让开发者了解哪些东西已被废弃以及废弃的原因，本清单被创建出来，作为在有人提议把已废弃特性的用例纳入内核时进行反驳的依据。)

__deprecated(已弃用属性)
------------------------
While this attribute does visually mark an interface as deprecated,
it `does not produce warnings during builds any more
<https://git.kernel.org/linus/771c035372a036f83353eef46dbb829780330234>`_
because one of the standing goals of the kernel is to build without
warnings and no one was actually doing anything to remove these deprecated
interfaces. While using `__deprecated` is nice to note an old API in
a header file, it isn't the full solution. Such interfaces must either
be fully removed from the kernel, or added to this file to discourage
others from using them in the future.
(虽然这个属性能在视觉上把一个接口标记为已弃用，但它如今已不会在构建过程中产生任何警告，因为内核的长期目标之一就是无警告构建，而实际上也没有人做过任何移除这些已弃用接口的工作。在头文件里用 `__deprecated` 标注一个旧 API 固然很好，但这并不是完整的解决方案。这类接口要么必须从内核中彻底移除，要么被加入本文件，以阻止他人在将来继续使用。)

BUG() and BUG_ON()(BUG 与 BUG_ON)
---------------------------------
Use WARN() and WARN_ON() instead, and handle the "impossible"
error condition as gracefully as possible. While the BUG()-family
of APIs were originally designed to act as an "impossible situation"
assert and to kill a kernel thread "safely", they turn out to just be
too risky. (e.g. "In what order do locks need to be released? Have
various states been restored?") Very commonly, using BUG() will
destabilize a system or entirely break it, which makes it impossible
to debug or even get viable crash reports. Linus has `very strong
<https://lore.kernel.org/lkml/CA+55aFy6jNLsywVYdGp83AMrXBo_P-pkjkphPGrO=82SPKCpLQ@mail.gmail.com/>`_
feelings `about this
<https://lore.kernel.org/lkml/CAHk-=whDHsbK3HTOpTF=ue_o04onRwTEaK_ZoJp_fjbqq4+=Jw@mail.gmail.com/>`_.
(应改用 WARN() 与 WARN_ON()，并以尽可能优雅的方式处理这种"不可能"出现的错误状况。尽管 BUG() 系列 API 最初被设计为一种"不可能出现的情况"断言，并能"安全地"杀死内核线程，但事实证明它们风险太大。(例如："锁需要按什么顺序释放？各种状态是否都已恢复？")非常常见的情况是，使用 BUG() 会让系统失去稳定性甚至完全崩溃，这使得调试乃至获取有效的崩溃报告都变得不可能。Linus 对此的态度 `非常强硬
<https://lore.kernel.org/lkml/CA+55aFy6jNLsywVYdGp83AMrXBo_P-pkjkphPGrO=82SPKCpLQ@mail.gmail.com/>`_。)

Note that the WARN()-family should only be used for "expected to
be unreachable" situations. If you want to warn about "reachable
but undesirable" situations, please use the pr_warn()-family of
functions. System owners may have set the *panic_on_warn* sysctl,
to make sure their systems do not continue running in the face of
"unreachable" conditions. (For example, see commits like `this one
<https://git.kernel.org/linus/d4689846881d160a4d12a514e991a740bcb5d65a>`_.)
(注意，WARN() 系列只应用于"预期不可达"的情形。如果想对"可达但不合意"的情形发出警告，请使用 pr_warn() 系列函数。系统所有者可能已经设置了 panic_on_warn sysctl，以确保其系统在遇到"不可达"状况时不会继续运行。(例如，可参见类似 `this one
<https://git.kernel.org/linus/d4689846881d160a4d12a514e991a740bcb5d65a>`_ 这样的提交。))

open-coded arithmetic in allocator arguments(分配器参数中的手写算术运算)
------------------------------------------------------------------------
Dynamic size calculations (especially multiplication) should not be
performed in memory allocator (or similar) function arguments due to the
risk of them overflowing. This could lead to values wrapping around and a
smaller allocation being made than the caller was expecting. Using those
allocations could lead to linear overflows of heap memory and other
misbehaviors. (One exception to this is literal values where the compiler
can warn if they might overflow. However, the preferred way in these
cases is to refactor the code as suggested below to avoid the open-coded
arithmetic.)
(不应在内存分配器(或类似)函数的参数中执行动态大小计算(尤其是乘法)，因为这些计算有溢出的风险。这可能导致数值回绕，使实际分配的内存比调用方预期的更小。使用这种过小的分配可能造成堆内存的线性溢出及其他异常行为。(一个例外是字面值——编译器可以在其可能溢出时发出警告。不过，在这些情况下首选的做法仍是按下文建议重构代码，以避免手写算术运算。))

For example, do not use ``count * size`` as an argument, as in::

	foo = kmalloc(count * size, GFP_KERNEL);

(例如，不要像下面这样把 ``count * size`` 用作参数：)

Instead, the 2-factor form of the allocator should be used::

	foo = kmalloc_array(count, size, GFP_KERNEL);

(而应当使用分配器的双因子 (2-factor) 形式：)

Specifically, kmalloc() can be replaced with kmalloc_array(), and
kzalloc() can be replaced with kcalloc().
(具体而言，kmalloc() 可以替换为 kmalloc_array()，kzalloc() 可以替换为 kcalloc()。)

If no 2-factor form is available, the saturate-on-overflow helpers should
be used::

	bar = dma_alloc_coherent(dev, array_size(count, size), &dma, GFP_KERNEL);

(如果没有可用的双因子形式，则应使用溢出饱和 (saturate-on-overflow) 辅助函数：)

Another common case to avoid is calculating the size of a structure with
a trailing array of others structures, as in::

	header = kzalloc(sizeof(*header) + count * sizeof(*header->item),
			 GFP_KERNEL);

(另一个应避免的常见情形是计算"以其他结构的数组作尾随成员"的结构大小，如下所示：)

Instead, use the helper::

	header = kzalloc(struct_size(header, item, count), GFP_KERNEL);

(而应使用这个辅助函数：)

.. note:: If you are using struct_size() on a structure containing a zero-length
        or a one-element array as a trailing array member, please refactor such
        array usage and switch to a `flexible array member
        <#zero-length-and-one-element-arrays>`_ instead.
        (注意：如果你正在对含有零长度或单元素数组作为尾随数组成员的结构使用 struct_size()，请重构这种数组用法，改用柔性数组成员 (flexible array member) 代替。)

For other calculations, please compose the use of the size_mul(),
size_add(), and size_sub() helpers. For example, in the case of::

	foo = krealloc(current_size + chunk_size * (count - 3), GFP_KERNEL);

(对于其他计算，请组合使用 size_mul()、size_add() 与 size_sub() 辅助函数。例如下面这种情况：)

Instead, use the helpers::

	foo = krealloc(size_add(current_size,
				size_mul(chunk_size,
					 size_sub(count, 3))), GFP_KERNEL);

(而应组合使用这些辅助函数：)

For more details, also see array3_size() and flex_array_size(),
as well as the related check_mul_overflow(), check_add_overflow(),
check_sub_overflow(), and check_shl_overflow() family of functions.
(更多细节另请参见 array3_size() 与 flex_array_size()，以及相关的 check_mul_overflow()、check_add_overflow()、check_sub_overflow() 和 check_shl_overflow() 系列函数。)

simple_strtol(), simple_strtoll(), simple_strtoul(), simple_strtoull()(简单字符串转数值函数)
--------------------------------------------------------------------------------------------
The simple_strtol(), simple_strtoll(),
simple_strtoul(), and simple_strtoull() functions
explicitly ignore overflows, which may lead to unexpected results
in callers. The respective kstrtol(), kstrtoll(),
kstrtoul(), and kstrtoull() functions tend to be the
correct replacements, though note that those require the string to be
NUL or newline terminated.
(simple_strtol()、simple_strtoll()、simple_strtoul() 与 simple_strtoull() 函数会显式忽略溢出，这可能在调用方中导致出乎意料的结果。对应的 kstrtol()、kstrtoll()、kstrtoul() 与 kstrtoull() 函数通常是正确的替代品，但要注意它们要求字符串以 NUL 或换行符结尾。)

strcpy()(字符串复制)
--------------------
strcpy() performs no bounds checking on the destination buffer. This
could result in linear overflows beyond the end of the buffer, leading to
all kinds of misbehaviors. While `CONFIG_FORTIFY_SOURCE=y` and various
compiler flags help reduce the risk of using this function, there is
no good reason to add new uses of this function. The safe replacement
is strscpy(), though care must be given to any cases where the return
value of strcpy() was used, since strscpy() does not return a pointer to
the destination, but rather a count of non-NUL bytes copied (or negative
errno when it truncates).
(strcpy() 不对目标缓冲区做任何边界检查。这可能导致超出缓冲区末尾的线性溢出，引发各种异常行为。虽然 `CONFIG_FORTIFY_SOURCE=y` 和各种编译器选项有助于降低使用该函数的风险，但没有任何理由新增对该函数的使用。安全的替代品是 strscpy()，不过凡是原来用到了 strcpy() 返回值的地方都要当心，因为 strscpy() 返回的不是指向目标的指针，而是已复制的非 NUL 字节数(发生截断时返回负的 errno)。)

strncpy() on NUL-terminated strings(NUL 结尾字符串上的 strncpy())
-----------------------------------------------------------------
Use of strncpy() does not guarantee that the destination buffer will
be NUL terminated. This can lead to various linear read overflows and
other misbehavior due to the missing termination. It also NUL-pads
the destination buffer if the source contents are shorter than the
destination buffer size, which may be a needless performance penalty
for callers using only NUL-terminated strings.
(使用 strncpy() 并不保证目标缓冲区会以 NUL 结尾。由于缺少结束符，这可能引发各种线性读溢出及其他异常行为。当源内容短于目标缓冲区大小时，它还会用 NUL 填充目标缓冲区的剩余部分，对只使用 NUL 结尾字符串的调用方而言，这可能是一种不必要的性能损耗。)

When the destination is required to be NUL-terminated, the replacement is
strscpy(), though care must be given to any cases where the return value
of strncpy() was used, since strscpy() does not return a pointer to the
destination, but rather a count of non-NUL bytes copied (or negative
errno when it truncates). Any cases still needing NUL-padding should
instead use strscpy_pad().
(当要求目标必须以 NUL 结尾时，替代品是 strscpy()，不过凡是原来用到了 strncpy() 返回值的地方都要当心，因为 strscpy() 返回的不是指向目标的指针，而是已复制的非 NUL 字节数(发生截断时返回负的 errno)。仍需要 NUL 填充的情形应改用 strscpy_pad()。)

If a caller is using non-NUL-terminated strings, strtomem() should be
used, and the destinations should be marked with the `__nonstring
<https://gcc.gnu.org/onlinedocs/gcc/Common-Variable-Attributes.html>`_
attribute to avoid future compiler warnings. For cases still needing
NUL-padding, strtomem_pad() can be used.
(如果调用方使用的是不以 NUL 结尾的字符串，则应使用 strtomem()，并把目标变量标记为 __nonstring 属性，以避免未来的编译器警告。仍需要 NUL 填充的情形可以使用 strtomem_pad()。)

strlcpy()(长度受限的字符串复制)
-------------------------------
strlcpy() reads the entire source buffer first (since the return value
is meant to match that of strlen()). This read may exceed the destination
size limit. This is both inefficient and can lead to linear read overflows
if a source string is not NUL-terminated. The safe replacement is strscpy(),
though care must be given to any cases where the return value of strlcpy()
is used, since strscpy() will return negative errno values when it truncates.
(strlcpy() 会先读取整个源缓冲区(因为其返回值本意是与 strlen() 一致)。这一读取可能超出目标大小限制。这样做既低效，又可能在源字符串不以 NUL 结尾时导致线性读溢出。安全的替代品是 strscpy()，不过凡是原来用到了 strlcpy() 返回值的地方都要当心，因为 strscpy() 在发生截断时会返回负的 errno 值。)

%p format specifier(%p 格式说明符)
----------------------------------
Traditionally, using "%p" in format strings would lead to regular address
exposure flaws in dmesg, proc, sysfs, etc. Instead of leaving these to
be exploitable, all "%p" uses in the kernel are being printed as a hashed
value, rendering them unusable for addressing. New uses of "%p" should not
be added to the kernel. For text addresses, using "%pS" is likely better,
as it produces the more useful symbol name instead. For nearly everything
else, just do not add "%p" at all.
(传统上，在格式字符串中使用 "%p" 会在 dmesg、proc、sysfs 等处造成常规的地址暴露缺陷。为了不让这些缺陷可被利用，内核中所有 "%p" 的使用都会打印为经过哈希处理的值，使其无法再用于寻址。不应在内核中新增 "%p" 的使用。对于文本地址，使用 "%pS" 可能更好，因为它会产生更有用的符号名。除此之外的几乎所有情况，干脆不要添加 "%p"。)

Paraphrasing Linus's current `guidance <https://lore.kernel.org/lkml/CA+55aFwQEd_d40g4mUCSsVRZzrFPUJt74vc6PPpb675hYNXcKw@mail.gmail.com/>`_:
(概述 Linus 目前的指导意见：)

- If the hashed "%p" value is pointless, ask yourself whether the pointer
  itself is important. Maybe it should be removed entirely?
  (如果哈希后的 "%p" 值毫无意义，问问自己这个指针本身是否重要。也许应该把它完全移除？)
- If you really think the true pointer value is important, why is some
  system state or user privilege level considered "special"? If you think
  you can justify it (in comments and commit log) well enough to stand
  up to Linus's scrutiny, maybe you can use "%px", along with making sure
  you have sensible permissions.
  (如果你确实认为真实的指针值很重要，那为什么某个系统状态或用户特权级别会被认为是"特殊的"？如果你认为自己的论证(写在注释和提交日志中)足以经受住 Linus 的审视，那么也许可以使用 "%px"，同时确保你拥有合理的权限设置。)

If you are debugging something where "%p" hashing is causing problems,
you can temporarily boot with the debug flag "`no_hash_pointers
<https://git.kernel.org/linus/5ead723a20e0447bc7db33dc3070b420e5f80aa6>`_".
(如果你正在调试的问题是由 "%p" 哈希引起的，可以临时用调试开关 `no_hash_pointers <https://git.kernel.org/linus/5ead723a20e0447bc7db33dc3070b420e5f80aa6>`_ 启动内核。)

Variable Length Arrays (VLAs)(变长数组 (VLA))
---------------------------------------------
Using stack VLAs produces much worse machine code than statically
sized stack arrays. While these non-trivial `performance issues
<https://git.kernel.org/linus/02361bc77888>`_ are reason enough to
eliminate VLAs, they are also a security risk. Dynamic growth of a stack
array may exceed the remaining memory in the stack segment. This could
lead to a crash, possible overwriting sensitive contents at the end of the
stack (when built without `CONFIG_THREAD_INFO_IN_TASK=y`), or overwriting
memory adjacent to the stack (when built without `CONFIG_VMAP_STACK=y`)
(使用栈上 VLA 生成的机器码比静态定长的栈数组差得多。虽然这些不容忽视的性能问题是消灭 VLA 的充分理由，它们同时也是安全隐患。栈数组动态增长可能超出栈段中的剩余内存。这可能导致崩溃、可能覆盖栈末尾的敏感内容(在未启用 `CONFIG_THREAD_INFO_IN_TASK=y` 构建时)，或覆盖与栈相邻的内存(在未启用 `CONFIG_VMAP_STACK=y` 构建时)。)

Implicit switch case fall-through(隐式 switch case 贯穿)
--------------------------------------------------------
The C language allows switch cases to fall through to the next case
when a "break" statement is missing at the end of a case. This, however,
introduces ambiguity in the code, as it's not always clear if the missing
break is intentional or a bug. For example, it's not obvious just from
looking at the code if `STATE_ONE` is intentionally designed to fall
through into `STATE_TWO`::

	switch (value) {
	case STATE_ONE:
		do_something();
	case STATE_TWO:
		do_other();
		break;
	default:
		WARN("unknown state");
	}

(C 语言允许在 case 末尾缺少 "break" 语句时贯穿 (fall through) 到下一个 case。然而这会给代码引入歧义，因为缺少 break 究竟是有意为之还是一个 bug 并不总是很清楚。例如，仅凭阅读代码无法判断 `STATE_ONE` 是否被有意设计为贯穿到 `STATE_TWO`：)

As there have been a long list of flaws `due to missing "break" statements
<https://cwe.mitre.org/data/definitions/484.html>`_, we no longer allow
implicit fall-through. In order to identify intentional fall-through
cases, we have adopted a pseudo-keyword macro "fallthrough" which
expands to gcc's extension `__attribute__((__fallthrough__))
<https://gcc.gnu.org/onlinedocs/gcc/Statement-Attributes.html>`_.
(When the C17/C18  `[[fallthrough]]` syntax is more commonly supported by
C compilers, static analyzers, and IDEs, we can switch to using that syntax
for the macro pseudo-keyword.)
(由于历史上曾有一长串因缺少 "break" 语句导致的缺陷，我们不再允许隐式贯穿。为了识别有意为之的贯穿情形，我们采用了伪关键字宏 "fallthrough"，它会展开为 gcc 的扩展 `__attribute__((__fallthrough__))
<https://gcc.gnu.org/onlinedocs/gcc/Statement-Attributes.html>`_。(等 C17/C18 的 `[[fallthrough]]` 语法得到 C 编译器、静态分析器和 IDE 更广泛的支持后，我们可以把这个宏伪关键字切换为该语法。))

All switch/case blocks must end in one of:
(所有 switch/case 块必须以下列之一结尾：)

* break;
* fallthrough;
* continue;
* goto <label>;
* return [expression];

Zero-length and one-element arrays(零长度与单元素数组)
------------------------------------------------------
There is a regular need in the kernel to provide a way to declare having
a dynamically sized set of trailing elements in a structure. Kernel code
should always use `"flexible array members" <https://en.wikipedia.org/wiki/Flexible_array_member>`_
for these cases. The older style of one-element or zero-length arrays should
no longer be used.
(内核中经常需要提供一种在结构内声明"动态大小的尾随元素集合"的方式。内核代码在这些情况下应始终使用"柔性数组成员" (flexible array members)。旧式的单元素或零长度数组不应再使用。)

In older C code, dynamically sized trailing elements were done by specifying
a one-element array at the end of a structure::

        struct something {
                size_t count;
                struct foo items[1];
        };

(在较老的 C 代码中，动态大小的尾随元素是通过在结构末尾声明一个单元素数组来实现的：)

This led to fragile size calculations via sizeof() (which would need to
remove the size of the single trailing element to get a correct size of
the "header"). A `GNU C extension <https://gcc.gnu.org/onlinedocs/gcc/Zero-Length.html>`_
was introduced to allow for zero-length arrays, to avoid these kinds of
size problems::

        struct something {
                size_t count;
                struct foo items[0];
        };

(这导致基于 sizeof() 的脆弱大小计算(必须减去单个尾随元素的大小才能得到"头部"的正确大小)。后来引入了一个 GNU C 扩展，允许使用零长度数组来避免这类大小问题：)

But this led to other problems, and didn't solve some problems shared by
both styles, like not being able to detect when such an array is accidentally
being used _not_ at the end of a structure (which could happen directly, or
when such a struct was in unions, structs of structs, etc).
(但这又带来了其他问题，而且没有解决两种风格共有的一些问题，例如无法检测这类数组是否被意外用在并非结构末尾的位置(这可能直接发生，也可能发生在这样的结构被放入联合体、嵌套结构等场合)。)

C99 introduced "flexible array members", which lacks a numeric size for
the array declaration entirely::

        struct something {
                size_t count;
                struct foo items[];
        };

(C99 引入了"柔性数组成员" (flexible array members)，其数组声明完全不带数字大小：)

This is the way the kernel expects dynamically sized trailing elements
to be declared. It allows the compiler to generate errors when the
flexible array does not occur last in the structure, which helps to prevent
some kind of `undefined behavior
<https://git.kernel.org/linus/76497732932f15e7323dc805e8ea8dc11bb587cf>`_
bugs from being inadvertently introduced to the codebase. It also allows
the compiler to correctly analyze array sizes (via sizeof(),
`CONFIG_FORTIFY_SOURCE`, and `CONFIG_UBSAN_BOUNDS`). For instance,
there is no mechanism that warns us that the following application of the
sizeof() operator to a zero-length array always results in zero::

        struct something {
                size_t count;
                struct foo items[0];
        };

        struct something *instance;

        instance = kmalloc(struct_size(instance, items, count), GFP_KERNEL);
        instance->count = count;

        size = sizeof(instance->items) * instance->count;
        memcpy(instance->items, source, size);

(这正是内核期望声明动态大小尾随元素的方式。当柔性数组没有出现在结构末尾时，它允许编译器产生报错，这有助于防止某些"未定义行为"bug 被无意间引入代码库。它还允许编译器正确分析数组大小(通过 sizeof()、`CONFIG_FORTIFY_SOURCE` 与 `CONFIG_UBSAN_BOUNDS`)。例如，没有任何机制会提醒我们：下面这种对零长度数组应用 sizeof() 运算符的做法结果永远是零：)

At the last line of code above, ``size`` turns out to be ``zero``, when one might
have thought it represents the total size in bytes of the dynamic memory recently
allocated for the trailing array ``items``. Here are a couple examples of this
issue: `link 1
<https://git.kernel.org/linus/f2cd32a443da694ac4e28fbf4ac6f9d5cc63a539>`_,
`link 2
<https://git.kernel.org/linus/ab91c2a89f86be2898cee208d492816ec238b2cf>`_.
Instead, `flexible array members have incomplete type, and so the sizeof()
operator may not be applied <https://gcc.gnu.org/onlinedocs/gcc/Zero-Length.html>`_,
so any misuse of such operators will be immediately noticed at build time.
(在上面代码的最后一行，``size`` 的结果是 ``zero``，而人们可能以为它代表最近为尾随数组 ``items`` 分配的动态内存的总字节数。这一问题的几个实例：`link 1
<https://git.kernel.org/linus/f2cd32a443da694ac4e28fbf4ac6f9d5cc63a539>`_、`link 2
<https://git.kernel.org/linus/ab91c2a89f86be2898cee208d492816ec238b2cf>`_。相比之下，柔性数组成员具有不完整类型，因而不能对其应用 sizeof() 运算符，任何对此类运算符的误用都会在构建时被立即发现。)

With respect to one-element arrays, one has to be acutely aware that `such arrays
occupy at least as much space as a single object of the type
<https://gcc.gnu.org/onlinedocs/gcc/Zero-Length.html>`_,
hence they contribute to the size of the enclosing structure. This is prone
to error every time people want to calculate the total size of dynamic memory
to allocate for a structure containing an array of this kind as a member::

        struct something {
                size_t count;
                struct foo items[1];
        };

        struct something *instance;

        instance = kmalloc(struct_size(instance, items, count - 1), GFP_KERNEL);
        instance->count = count;

        size = sizeof(instance->items) * instance->count;
        memcpy(instance->items, source, size);

(对于单元素数组，必须时刻清醒地意识到：这类数组所占空间至少与该类型单个对象一样大，因此它们会计入外层结构的大小。每当人们想为含有此类数组成员的结构计算要分配的动态内存总大小时，都容易出错：)

In the example above, we had to remember to calculate ``count - 1`` when using
the struct_size() helper, otherwise we would have --unintentionally-- allocated
memory for one too many ``items`` objects. The cleanest and least error-prone way
to implement this is through the use of a `flexible array member`, together with
struct_size() and flex_array_size() helpers::

        struct something {
                size_t count;
                struct foo items[];
        };

        struct something *instance;

        instance = kmalloc(struct_size(instance, items, count), GFP_KERNEL);
        instance->count = count;

        memcpy(instance->items, source, flex_array_size(instance, items, instance->count));

(在上面的例子中，使用 struct_size() 辅助函数时必须记得计算 ``count - 1``，否则我们会无意间为多出一个的 ``items`` 对象分配内存。实现这一目标最干净、最不易出错的方式是使用柔性数组成员，并配合 struct_size() 与 flex_array_size() 辅助函数：)

There are two special cases of replacement where the DECLARE_FLEX_ARRAY()
helper needs to be used. (Note that it is named __DECLARE_FLEX_ARRAY() for
use in UAPI headers.) Those cases are when the flexible array is either
alone in a struct or is part of a union. These are disallowed by the C99
specification, but for no technical reason (as can be seen by both the
existing use of such arrays in those places and the work-around that
DECLARE_FLEX_ARRAY() uses). For example, to convert this::

	struct something {
		...
		union {
			struct type1 one[0];
			struct type2 two[0];
		};
	};

(有两种特殊的替换场景需要使用 DECLARE_FLEX_ARRAY() 辅助函数。(注意，用于 UAPI 头文件时它的名字是 __DECLARE_FLEX_ARRAY()。)这两种场景是：柔性数组单独存在于一个 struct 中，或者它是联合体的一部分。C99 规范不允许这两种用法，但并没有技术上的理由(这些场景中此类数组的既有用法以及 DECLARE_FLEX_ARRAY() 所采用的变通方案都证明了这一点)。例如，要把下面的代码：)

The helper must be used::

	struct something {
		...
		union {
			DECLARE_FLEX_ARRAY(struct type1, one);
			DECLARE_FLEX_ARRAY(struct type2, two);
		};
	};

(转换过去，必须使用该辅助函数：)

Open-coded kmalloc assignments for struct objects(对 struct 对象手写 kmalloc 赋值)
----------------------------------------------------------------------------------
Performing open-coded kmalloc()-family allocation assignments prevents
the kernel (and compiler) from being able to examine the type of the
variable being assigned, which limits any related introspection that
may help with alignment, wrap-around, or additional hardening. The
kmalloc_obj()-family of macros provide this introspection, which can be
used for the common code patterns for single, array, and flexible object
allocations. For example, these open coded assignments::

	ptr = kmalloc(sizeof(*ptr), gfp);
	ptr = kzalloc(sizeof(*ptr), gfp);
	ptr = kmalloc_array(count, sizeof(*ptr), gfp);
	ptr = kcalloc(count, sizeof(*ptr), gfp);
	ptr = kmalloc(struct_size(ptr, flex_member, count), gfp);
	ptr = kmalloc(sizeof(struct foo, gfp);

(对手写 (open-coded) 的 kmalloc() 系列分配赋值，内核(和编译器)无法检查被赋值变量的类型，这限制了可能有助于对齐、回绕防护或额外加固的相关内省。kmalloc_obj() 系列宏提供了这种内省能力，可用于单对象、数组与柔性对象分配的常见代码模式。例如下面这些手写赋值：)

become, respectively::

	ptr = kmalloc_obj(*ptr, gfp);
	ptr = kzalloc_obj(*ptr, gfp);
	ptr = kmalloc_objs(*ptr, count, gfp);
	ptr = kzalloc_objs(*ptr, count, gfp);
	ptr = kmalloc_flex(*ptr, flex_member, count, gfp);
	__auto_type ptr = kmalloc_obj(struct foo, gfp);

(分别变成：)

If `ptr->flex_member` is annotated with __counted_by(), the allocation
will automatically fail if `count` is larger than the maximum
representable value that can be stored in the counter member associated
with `flex_member`.
(如果 `ptr->flex_member` 标注了 __counted_by()，那么当 `count` 大于与 `flex_member` 相关联的计数器成员所能存储的最大可表示值时，分配会自动失败。)
