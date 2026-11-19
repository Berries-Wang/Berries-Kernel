=========================================================================
How to get printk format specifiers right(如何正确使用 printk 格式说明符)
=========================================================================

.. _printk-specifiers:

:Author: Randy Dunlap <rdunlap@infradead.org>
(作者: Randy Dunlap <rdunlap@infradead.org>)
:Author: Andrew Murray <amurray@mpc-data.co.uk>
(作者: Andrew Murray <amurray@mpc-data.co.uk>)


Integer types(整数类型)
=======================

::

	If variable is of Type,		use printk format specifier:
	------------------------------------------------------------
		signed char		%d or %hhx
		unsigned char		%u or %x
		char			%u or %x
		short int		%d or %hx
		unsigned short int	%u or %x
		int			%d or %x
		unsigned int		%u or %x
		long			%ld or %lx
		unsigned long		%lu or %lx
		long long		%lld or %llx
		unsigned long long	%llu or %llx
		size_t			%zu or %zx
		ssize_t			%zd or %zx
		s8			%d or %hhx
		u8			%u or %x
		s16			%d or %hx
		u16			%u or %x
		s32			%d or %x
		u32			%u or %x
		s64			%lld or %llx
		u64			%llu or %llx


If <type> is architecture-dependent for its size (e.g., cycles_t, tcflag_t) or
is dependent on a config option for its size (e.g., blk_status_t), use a format
specifier of its largest possible type and explicitly cast to it.
(如果 <type> 的大小与架构相关（例如 cycles_t、tcflag_t），或其大小取决于某个配置选项
（例如 blk_status_t），请使用其最大可能类型的格式说明符，并显式转换 (cast) 为该类型。)

Example::

	printk("test: latency: %llu cycles\n", (unsigned long long)time);

(示例：)

Reminder: sizeof() returns type size_t.
(提醒：sizeof() 返回的类型是 size_t。)

The kernel's printf does not support %n. Floating point formats (%e, %f,
%g, %a) are also not recognized, for obvious reasons. Use of any
unsupported specifier or length qualifier results in a WARN and early
return from vsnprintf().
(内核的 printf 不支持 %n。浮点数格式 (%e、%f、
%g、%a) 出于显而易见的原因同样无法识别。使用任何不受支持的说明符或长度修饰符
都会触发 WARN 并从 vsnprintf() 提前返回。)

Pointer types(指针类型)
=======================

A raw pointer value may be printed with %p which will hash the address
before printing. The kernel also supports extended specifiers for printing
pointers of different types.
(原始指针值可以用 %p 打印，它会在打印之前对地址进行哈希处理。内核还支持各种
扩展说明符，用于打印不同类型的指针。)

Some of the extended specifiers print the data on the given address instead
of printing the address itself. In this case, the following error messages
might be printed instead of the unreachable information::

	(null)	 data on plain NULL address
	(efault) data on invalid address
	(einval) invalid data on a valid address

(部分扩展说明符打印给定地址处的数据，而不是地址本身。在这种情况下，可能打印以下
错误消息来代替无法获取的信息：)

Plain Pointers(普通指针)
------------------------

::

	%p	abcdef12 or 00000000abcdef12

Pointers printed without a specifier extension (i.e unadorned %p) are
hashed to prevent leaking information about the kernel memory layout. This
has the added benefit of providing a unique identifier. On 64-bit machines
the first 32 bits are zeroed. The kernel will print ``(ptrval)`` until it
gathers enough entropy.
(不带说明符扩展的指针（即不加修饰的 %p）会被哈希处理，以防止泄露内核内存布局
信息。这还带来了提供一个唯一标识符的额外好处。在 64 位机器上，前 32 位会被
置零。在收集到足够的熵之前，内核会打印 ``(ptrval)`` 。)

When possible, use specialised modifiers such as %pS or %pB (described below)
to avoid the need of providing an unhashed address that has to be interpreted
post-hoc. If not possible, and the aim of printing the address is to provide
more information for debugging, use %p and boot the kernel with the
``no_hash_pointers`` parameter during debugging, which will print all %p
addresses unmodified. If you *really* always want the unmodified address, see
%px below.
(只要可能，请使用诸如 %pS 或 %pB（下文描述）之类的专用修饰符，以避免提供事后
才能解释的未哈希地址。如果做不到这一点，且打印地址的目的是为调试提供更多信息，
请使用 %p，并在调试期间以 ``no_hash_pointers`` 参数启动内核，这将原样打印所有
%p 地址。如果你 *真的* 始终想要未修改的地址，请参阅下文的 %px。)

If (and only if) you are printing addresses as a content of a virtual file in
e.g. procfs or sysfs (using e.g. seq_printf(), not printk()) read by a
userspace process, use the %pK modifier described below instead of %p or %px.
(当且仅当你是把地址打印到 procfs 或 sysfs 等虚拟文件的内容中（例如使用
seq_printf()，而不是 printk()）供用户空间进程读取时，请使用下文描述的 %pK
修饰符，而不是 %p 或 %px。)

Error Pointers(错误指针)
------------------------

::

	%pe	-ENOSPC

For printing error pointers (i.e. a pointer for which IS_ERR() is true)
as a symbolic error name. Error values for which no symbolic name is
known are printed in decimal, while a non-ERR_PTR passed as the
argument to %pe gets treated as ordinary %p.
(用于打印错误指针（即 IS_ERR() 为真的指针）为符号化的错误名称。没有对应符号名
的错误值会以十进制打印；而作为参数传给 %pe 的非 ERR_PTR 则按普通 %p 处理。)

Symbols/Function Pointers(符号/函数指针)
----------------------------------------

::

	%pS	versatile_init+0x0/0x110
	%ps	versatile_init
	%pSR	versatile_init+0x9/0x110
		(with __builtin_extract_return_addr() translation)
	%pB	prev_fn_of_versatile_init+0x88/0x88


The ``S`` and ``s`` specifiers are used for printing a pointer in symbolic
format. They result in the symbol name with (S) or without (s)
offsets. If KALLSYMS are disabled then the symbol address is printed instead.
( ``S`` 和 ``s`` 说明符用于以符号格式打印指针。它们会输出带 (S) 或不带 (s)
偏移量的符号名。如果 KALLSYMS 被禁用，则改为打印符号地址。)

The ``B`` specifier results in the symbol name with offsets and should be
used when printing stack backtraces. The specifier takes into
consideration the effect of compiler optimisations which may occur
when tail-calls are used and marked with the noreturn GCC attribute.
( ``B`` 说明符输出带偏移量的符号名，应在打印栈回溯时使用。该说明符会考虑
编译器优化的影响，这种优化可能在使用尾调用并标记了 noreturn GCC 属性时发生。)

If the pointer is within a module, the module name and optionally build ID is
printed after the symbol name with an extra ``b`` appended to the end of the
specifier.
(如果指针位于模块内，则会在符号名之后打印模块名以及可选的 build ID，并在说明符
末尾附加一个额外的 ``b`` 标记。)

::

	%pS	versatile_init+0x0/0x110 [module_name]
	%pSb	versatile_init+0x0/0x110 [module_name ed5019fdf5e53be37cb1ba7899292d7e143b259e]
	%pSRb	versatile_init+0x9/0x110 [module_name ed5019fdf5e53be37cb1ba7899292d7e143b259e]
		(with __builtin_extract_return_addr() translation)
	%pBb	prev_fn_of_versatile_init+0x88/0x88 [module_name ed5019fdf5e53be37cb1ba7899292d7e143b259e]

Probed Pointers from BPF / tracing(BPF/跟踪中的探测指针)
--------------------------------------------------------

::

	%pks	kernel string
	%pus	user string

The ``k`` and ``u`` specifiers are used for printing prior probed memory from
either kernel memory (k) or user memory (u). The subsequent ``s`` specifier
results in printing a string. For direct use in regular vsnprintf() the (k)
and (u) annotation is ignored, however, when used out of BPF's bpf_trace_printk(),
for example, it reads the memory it is pointing to without faulting.
( ``k`` 和 ``u`` 说明符用于打印先前探测到的内存，来源可以是内核内存 (k) 或用户
内存 (u)。其后的 ``s`` 说明符表示打印字符串。在普通 vsnprintf() 中直接使用时，
(k) 和 (u) 标注会被忽略；然而当在 BPF 的 bpf_trace_printk() 等场景之外使用时，
它会在不触发缺页的情况下读取其所指向的内存。)

Kernel Pointers(内核指针)
-------------------------

::

	%pK	01234567 or 0123456789abcdef

For printing kernel pointers which should be hidden from unprivileged
users. The behaviour of %pK depends on the kptr_restrict sysctl - see
Documentation/admin-guide/sysctl/kernel.rst for more details.
(用于打印应对非特权用户隐藏的内核指针。%pK 的行为取决于 kptr_restrict
sysctl —— 详见 Documentation/admin-guide/sysctl/kernel.rst。)

This modifier is *only* intended when producing content of a file read by
userspace from e.g. procfs or sysfs, not for dmesg. Please refer to the
section about %p above for discussion about how to manage hashing pointers
in printk().
(此修饰符 *只* 应用于生成供用户空间从 procfs 或 sysfs 等途径读取的文件内容，
而非 dmesg。关于如何在 printk() 中管理指针哈希的讨论，请参阅上文关于 %p 的
章节。)

Unmodified Addresses(未修改的地址)
----------------------------------

::

	%px	01234567 or 0123456789abcdef

For printing pointers when you *really* want to print the address. Please
consider whether or not you are leaking sensitive information about the
kernel memory layout before printing pointers with %px. %px is functionally
equivalent to %lx (or %lu). %px is preferred because it is more uniquely
grep'able. If in the future we need to modify the way the kernel handles
printing pointers we will be better equipped to find the call sites.
(用于在你 *确实* 想打印地址本身时打印指针。在使用 %px 打印指针之前，请考虑
是否正在泄露关于内核内存布局的敏感信息。%px 在功能上等同于 %lx（或 %lu）。
之所以首选 %px，是因为它更容易用 grep 唯一地定位；将来如果我们需要修改内核
处理指针打印的方式，也能更容易地找到调用点。)

Before using %px, consider if using %p is sufficient together with enabling the
``no_hash_pointers`` kernel parameter during debugging sessions (see the %p
description above). One valid scenario for %px might be printing information
immediately before a panic, which prevents any sensitive information to be
exploited anyway, and with %px there would be no need to reproduce the panic
with no_hash_pointers.
(在使用 %px 之前，请考虑在调试会话期间启用 ``no_hash_pointers`` 内核参数并配合
使用 %p 是否已经足够（参见上文对 %p 的描述）。%px 的一个合理使用场景是在 panic
即将发生前立即打印信息——无论如何这都能防止敏感信息被利用，而使用 %px 就无需
借助 no_hash_pointers 重现该 panic。)

Pointer Differences(指针差值)
-----------------------------

::

	%td	2560
	%tx	a00

For printing the pointer differences, use the %t modifier for ptrdiff_t.
(打印指针差值时，对 ptrdiff_t 使用 %t 修饰符。)

Example::

	printk("test: difference between pointers: %td\n", ptr2 - ptr1);

(示例：)

Struct Resources(struct resource 结构体)
----------------------------------------

::

	%pr	[mem 0x60000000-0x6fffffff flags 0x2200] or
		[mem 0x60000000 flags 0x2200] or
		[mem 0x0000000060000000-0x000000006fffffff flags 0x2200]
		[mem 0x0000000060000000 flags 0x2200]
	%pR	[mem 0x60000000-0x6fffffff pref] or
		[mem 0x60000000 pref] or
		[mem 0x0000000060000000-0x000000006fffffff pref]
		[mem 0x0000000060000000 pref]

For printing struct resources. The ``R`` and ``r`` specifiers result in a
printed resource with (R) or without (r) a decoded flags member.  If start is
equal to end only print the start value.
(用于打印 struct resource。``R`` 和 ``r`` 说明符输出的资源分别带 (R) 或不带
(r) 已解码的 flags 成员。如果 start 等于 end，则只打印 start 值。)

Passed by reference.
(按引用传递。)

Physical address types phys_addr_t(物理地址类型 phys_addr_t)
------------------------------------------------------------

::

	%pa[p]	0x01234567 or 0x0123456789abcdef

For printing a phys_addr_t type (and its derivatives, such as
resource_size_t) which can vary based on build options, regardless of the
width of the CPU data path.
(用于打印 phys_addr_t 类型（及其衍生类型，如 resource_size_t），其宽度可能随
构建选项而变化，与 CPU 数据通路宽度无关。)

Passed by reference.
(按引用传递。)

Struct Range(struct range 结构体)
---------------------------------

::

	%pra    [range 0x0000000060000000-0x000000006fffffff] or
		[range 0x0000000060000000]

For printing struct range.  struct range holds an arbitrary range of u64
values.  If start is equal to end only print the start value.
(用于打印 struct range。struct range 保存一段任意的 u64 值范围。如果 start
等于 end，则只打印 start 值。)

Passed by reference.
(按引用传递。)

DMA address types dma_addr_t(DMA 地址类型 dma_addr_t)
-----------------------------------------------------

::

	%pad	0x01234567 or 0x0123456789abcdef

For printing a dma_addr_t type which can vary based on build options,
regardless of the width of the CPU data path.
(用于打印 dma_addr_t 类型，其宽度可能随构建选项而变化，与 CPU 数据通路宽度
无关。)

Passed by reference.
(按引用传递。)

Raw buffer as an escaped string(原始缓冲区作为转义字符串)
---------------------------------------------------------

::

	%*pE[achnops]

For printing raw buffer as an escaped string. For the following buffer::

		1b 62 20 5c 43 07 22 90 0d 5d

(用于把原始缓冲区打印为转义字符串。对于以下缓冲区：)

A few examples show how the conversion would be done (excluding surrounding
quotes)::

	%*pE		"\eb \C\a"\220\r]"
	%*pEhp		"\x1bb \C\x07"\x90\x0d]"
	%*pEa		"\e\142\040\\\103\a\042\220\r\135"

(几个示例展示了转换将如何进行（不含两侧的引号）：)

The conversion rules are applied according to an optional combination
of flags (see :c:func:`string_escape_mem` kernel documentation for the
details):
(转换规则根据可选的标志组合来应用，详见 :c:func:`string_escape_mem` 内核文档。)

	- a - ESCAPE_ANY
	- c - ESCAPE_SPECIAL
	- h - ESCAPE_HEX
	- n - ESCAPE_NULL
	- o - ESCAPE_OCTAL
	- p - ESCAPE_NP
	- s - ESCAPE_SPACE

By default ESCAPE_ANY_NP is used.
(默认使用 ESCAPE_ANY_NP。)

ESCAPE_ANY_NP is the sane choice for many cases, in particularly for
printing SSIDs.
(对于许多场景，ESCAPE_ANY_NP 都是合理的选择，尤其是打印 SSID 时。)

If field width is omitted then 1 byte only will be escaped.
(如果省略字段宽度，则只转义 1 个字节。)

Raw buffer as a hex string(原始缓冲区作为十六进制字符串)
--------------------------------------------------------

::

	%*ph	00 01 02  ...  3f
	%*phC	00:01:02: ... :3f
	%*phD	00-01-02- ... -3f
	%*phN	000102 ... 3f

For printing small buffers (up to 64 bytes long) as a hex string with a
certain separator. For larger buffers consider using
:c:func:`print_hex_dump`.
(用于以特定分隔符将较小的缓冲区（最长 64 字节）打印为十六进制字符串。对于更大的
缓冲区，考虑使用 :c:func:`print_hex_dump`。)

MAC/FDDI addresses(MAC/FDDI 地址)
---------------------------------

::

	%pM	00:01:02:03:04:05
	%pMR	05:04:03:02:01:00
	%pMF	00-01-02-03-04-05
	%pm	000102030405
	%pmR	050403020100

For printing 6-byte MAC/FDDI addresses in hex notation. The ``M`` and ``m``
specifiers result in a printed address with (M) or without (m) byte
separators. The default byte separator is the colon (:).
(用于以十六进制表示法打印 6 字节的 MAC/FDDI 地址。``M`` 和 ``m`` 说明符输出的
地址分别带 (M) 或不带 (m) 字节分隔符。默认的字节分隔符是冒号 (:)。)

Where FDDI addresses are concerned the ``F`` specifier can be used after
the ``M`` specifier to use dash (-) separators instead of the default
separator.
(对于 FDDI 地址，``F`` 说明符可用在 ``M`` 说明符之后，以使用连字符 (-) 分隔符
代替默认分隔符。)

For Bluetooth addresses the ``R`` specifier shall be used after the ``M``
specifier to use reversed byte order suitable for visual interpretation
of Bluetooth addresses which are in the little endian order.
(对于蓝牙地址，``R`` 说明符应用在 ``M`` 说明符之后，以使用反转的字节序——蓝牙
地址采用小端序，反转后便于直观阅读。)

Passed by reference.
(按引用传递。)

IPv4 addresses(IPv4 地址)
-------------------------

::

	%pI4	1.2.3.4
	%pi4	001.002.003.004
	%p[Ii]4[hnbl]

For printing IPv4 dot-separated decimal addresses. The ``I4`` and ``i4``
specifiers result in a printed address with (i4) or without (I4) leading
zeros.
(用于打印 IPv4 点分十进制地址。``I4`` 和 ``i4`` 说明符输出的地址分别不带
(I4) 或带 (i4) 前导零。)

The additional ``h``, ``n``, ``b``, and ``l`` specifiers are used to specify
host, network, big or little endian order addresses respectively. Where
no specifier is provided the default network/big endian order is used.
(附加的 ``h``、``n``、``b`` 和 ``l`` 说明符分别用于指定主机序、网络序、大端序
或小端序地址。若未提供说明符，则使用默认的网络序/大端序。)

Passed by reference.
(按引用传递。)

IPv6 addresses(IPv6 地址)
-------------------------

::

	%pI6	0001:0002:0003:0004:0005:0006:0007:0008
	%pi6	00010002000300040005000600070008
	%pI6c	1:2:3:4:5:6:7:8

For printing IPv6 network-order 16-bit hex addresses. The ``I6`` and ``i6``
specifiers result in a printed address with (I6) or without (i6)
colon-separators. Leading zeros are always used.
(用于打印 IPv6 网络字节序的 16 位十六进制地址。``I6`` 和 ``i6`` 说明符输出的
地址分别带 (I6) 或不带 (i6) 冒号分隔符。始终使用前导零。)

The additional ``c`` specifier can be used with the ``I`` specifier to
print a compressed IPv6 address as described by
https://tools.ietf.org/html/rfc5952
(附加的 ``c`` 说明符可与 ``I`` 说明符配合使用，按照
https://tools.ietf.org/html/rfc5952 的描述打印压缩形式的 IPv6 地址。)

Passed by reference.
(按引用传递。)

IPv4/IPv6 addresses (generic, with port, flowinfo, scope)(IPv4/IPv6 地址（通用，含端口、flowinfo、scope）)
----------------------------------------------------------------------------------------------------------

::

	%pIS	1.2.3.4		or 0001:0002:0003:0004:0005:0006:0007:0008
	%piS	001.002.003.004	or 00010002000300040005000600070008
	%pISc	1.2.3.4		or 1:2:3:4:5:6:7:8
	%pISpc	1.2.3.4:12345	or [1:2:3:4:5:6:7:8]:12345
	%p[Ii]S[pfschnbl]

For printing an IP address without the need to distinguish whether it's of
type AF_INET or AF_INET6. A pointer to a valid struct sockaddr,
specified through ``IS`` or ``iS``, can be passed to this format specifier.
(用于打印 IP 地址而无需区分其类型是 AF_INET 还是 AF_INET6。通过 ``IS`` 或
``iS`` 指定的、指向有效 struct sockaddr 的指针可以传给此格式说明符。)

The additional ``p``, ``f``, and ``s`` specifiers are used to specify port
(IPv4, IPv6), flowinfo (IPv6) and scope (IPv6). Ports have a ``:`` prefix,
flowinfo a ``/`` and scope a ``%``, each followed by the actual value.
(附加的 ``p``、``f`` 和 ``s`` 说明符分别用于指定端口 (IPv4、IPv6)、flowinfo
(IPv6) 和 scope (IPv6)。端口带 ``:`` 前缀，flowinfo 带 ``/``，scope 带 ``%`` ，
后跟实际值。)

In case of an IPv6 address the compressed IPv6 address as described by
https://tools.ietf.org/html/rfc5952 is being used if the additional
specifier ``c`` is given. The IPv6 address is surrounded by ``[``, ``]`` in
case of additional specifiers ``p``, ``f`` or ``s`` as suggested by
https://tools.ietf.org/html/draft-ietf-6man-text-addr-representation-07
(对于 IPv6 地址，若给出附加说明符 ``c`` ，则使用
https://tools.ietf.org/html/rfc5952 所描述的压缩形式。若给出附加说明符 ``p``、
``f`` 或 ``s`` ，则按照
https://tools.ietf.org/html/draft-ietf-6man-text-addr-representation-07 的
建议，IPv6 地址会被 ``[`` 和 ``]`` 包围。)

In case of IPv4 addresses, the additional ``h``, ``n``, ``b``, and ``l``
specifiers can be used as well and are ignored in case of an IPv6
address.
(对于 IPv4 地址，同样可以使用附加的 ``h``、``n``、``b`` 和 ``l`` 说明符；对
IPv6 地址它们会被忽略。)

Passed by reference.
(按引用传递。)

Further examples::

	%pISfc		1.2.3.4		or [1:2:3:4:5:6:7:8]/123456789
	%pISsc		1.2.3.4		or [1:2:3:4:5:6:7:8]%1234567890
	%pISpfc		1.2.3.4:12345	or [1:2:3:4:5:6:7:8]:12345/123456789

(更多示例：)

UUID/GUID addresses(UUID/GUID 地址)
------------------------------------

::

	%pUb	00010203-0405-0607-0809-0a0b0c0d0e0f
	%pUB	00010203-0405-0607-0809-0A0B0C0D0E0F
	%pUl	03020100-0504-0706-0809-0a0b0c0e0e0f
	%pUL	03020100-0504-0706-0809-0A0B0C0E0E0F

For printing 16-byte UUID/GUIDs addresses. The additional ``l``, ``L``,
``b`` and ``B`` specifiers are used to specify a little endian order in
lower (l) or upper case (L) hex notation - and big endian order in lower (b)
or upper case (B) hex notation.
(用于打印 16 字节的 UUID/GUID 地址。附加的 ``l``、``L``、
``b`` 和 ``B`` 说明符分别指定小端序的小写 (l) 或大写 (L) 十六进制表示——以及
大端序的小写 (b) 或大写 (B) 十六进制表示。)

Where no additional specifiers are used the default big endian
order with lower case hex notation will be printed.
(若不使用任何附加说明符，则默认打印大端序的小写十六进制表示。)

Passed by reference.
(按引用传递。)

dentry names(dentry 名称)
-------------------------

::

	%pd{,2,3,4}
	%pD{,2,3,4}

For printing dentry name; if we race with :c:func:`d_move`, the name might
be a mix of old and new ones, but it won't oops.  %pd dentry is a safer
equivalent of %s dentry->d_name.name we used to use, %pd<n> prints ``n``
last components.  %pD does the same thing for struct file.
(用于打印 dentry 名称；如果与 :c:func:`d_move` 发生竞争，名称可能是新旧名称的
混合，但不会 oops。%pd dentry 是我们过去使用的 %s dentry->d_name.name 的更
安全等价物；%pd<n> 打印最后 ``n`` 个分量。%pD 对 struct file 做同样的事情。)

Passed by reference.
(按引用传递。)

block_device names(block_device 名称)
-------------------------------------

::

	%pg	sda, sda1 or loop0p1

For printing name of block_device pointers.
(用于打印 block_device 指针的名称。)

struct va_format(struct va_format 结构体)
-----------------------------------------

::

	%pV

For printing struct va_format structures. These contain a format string
and va_list as follows::

	struct va_format {
		const char *fmt;
		va_list *va;
	};

(用于打印 struct va_format 结构体。这些结构体包含一个格式字符串和一个 va_list，
如下所示：)

Implements a "recursive vsnprintf".
(实现了一个"递归的 vsnprintf"。)

Do not use this feature without some mechanism to verify the
correctness of the format string and va_list arguments.
(在没有某种机制来验证格式字符串和 va_list 参数正确性的情况下，不要使用此特性。)

Passed by reference.
(按引用传递。)

Device tree nodes(设备树节点)
-----------------------------

::

	%pOF[fnpPcCF]


For printing device tree node structures. Default behaviour is
equivalent to %pOFf.
(用于打印设备树节点结构。默认行为等价于 %pOFf。)

	- f - device node full_name(设备节点 full_name)
	- n - device node name(设备节点名称)
	- p - device node phandle(设备节点 phandle)
	- P - device node path spec (name + @unit)(设备节点路径说明 (name + @unit))
	- F - device node flags(设备节点标志)
	- c - major compatible string(主 compatible 字符串)
	- C - full compatible string(完整 compatible 字符串)

The separator when using multiple arguments is ':'
(使用多个参数时的分隔符是 ':')

Examples::

	%pOF	/foo/bar@0			- Node full name
	%pOFf	/foo/bar@0			- Same as above
	%pOFfp	/foo/bar@0:10			- Node full name + phandle
	%pOFfcF	/foo/bar@0:foo,device:--P-	- Node full name +
	                                          major compatible string +
	                                            node flags
							D - dynamic
							d - detached
							P - Populated
							B - Populated bus

(示例：)

Passed by reference.
(按引用传递。)

Fwnode handles(fwnode 句柄)
---------------------------

::

	%pfw[fP]

For printing information on an fwnode_handle. The default is to print the full
node name, including the path. The modifiers are functionally equivalent to
%pOF above.
(用于打印 fwnode_handle 的信息。默认打印完整节点名，包括路径。这些修饰符在功能
上等同于上文的 %pOF。)

	- f - full name of the node, including the path(节点全名，包括路径)
	- P - the name of the node including an address (if there is one)(节点名，含地址（如果存在）)

Examples (ACPI)::

	%pfwf	\_SB.PCI0.CIO2.port@1.endpoint@0	- Full node name
	%pfwP	endpoint@0				- Node name

Examples (OF)::

	%pfwf	/ocp@68000000/i2c@48072000/camera@10/port/endpoint - Full name
	%pfwP	endpoint				- Node name

Time and date(时间与日期)
-------------------------

::

	%pt[RT]			YYYY-mm-ddTHH:MM:SS
	%pt[RT]s		YYYY-mm-dd HH:MM:SS
	%pt[RT]d		YYYY-mm-dd
	%pt[RT]t		HH:MM:SS
	%ptSp			<seconds>.<nanoseconds>
	%pt[RST][dt][r][s]

For printing date and time as represented by::

	R  content of struct rtc_time
	S  content of struct timespec64
	T  time64_t type

(打印由下列内容表示的日期和时间：)

in human readable format.
(并以人类可读的格式输出。)

By default year will be incremented by 1900 and month by 1.
Use %pt[RT]r (raw) to suppress this behaviour.
(默认情况下，年份会加 1900，月份会加 1。使用 %pt[RT]r (raw) 可抑制此行为。)

The %pt[RT]s (space) will override ISO 8601 separator by using ' ' (space)
instead of 'T' (Capital T) between date and time. It won't have any effect
when date or time is omitted.
(%pt[RT]s (space) 会覆盖 ISO 8601 分隔符，在日期与时间之间使用 ' ' (空格) 而不是
'T' (大写字母 T)。当日期或时间被省略时，它不会产生任何效果。)

The %ptSp is equivalent to %lld.%09ld for the content of the struct timespec64.
When the other specifiers are given, it becomes the respective equivalent of
%ptT[dt][r][s].%09ld. In other words, the seconds are being printed in
the human readable format followed by a dot and nanoseconds.
(对于 struct timespec64 的内容，%ptSp 等价于 %lld.%09ld。当给出其他说明符时，
它成为 %ptT[dt][r][s].%09ld 各自的等价形式。换句话说，秒以人类可读的格式打印，
后跟一个点和纳秒数。)

Passed by reference.
(按引用传递。)

struct clk(struct clk 结构体)
-----------------------------

::

	%pC	pll1

For printing struct clk structures. %pC prints the name of the clock
(Common Clock Framework) or a unique 32-bit ID (legacy clock framework).
(用于打印 struct clk 结构体。%pC 打印时钟的名称 (Common Clock Framework) 或
一个唯一的 32 位 ID (旧式时钟框架)。)

Passed by reference.
(按引用传递。)

bitmap and its derivatives such as cpumask and nodemask(位图及其衍生类型，如 cpumask 与 nodemask)
-------------------------------------------------------------------------------------------------

::

	%*pb	0779
	%*pbl	0,3-6,8-10

For printing bitmap and its derivatives such as cpumask and nodemask,
%*pb outputs the bitmap with field width as the number of bits and %*pbl
output the bitmap as range list with field width as the number of bits.
(用于打印位图 (bitmap) 及其衍生类型，如 cpumask 和 nodemask。%*pb 以位数为字段
宽度输出位图；%*pbl 以位数为字段宽度把位图输出为范围列表。)

The field width is passed by value, the bitmap is passed by reference.
Helper macros cpumask_pr_args() and nodemask_pr_args() are available to ease
printing cpumask and nodemask.
(字段宽度按值传递，位图按引用传递。辅助宏 cpumask_pr_args() 和
nodemask_pr_args() 可用于简化 cpumask 和 nodemask 的打印。)

Flags bitfields such as page flags and gfp_flags(标志位域，如页标志与 gfp_flags)
--------------------------------------------------------------------------------

::

	%pGp	0x17ffffc0002036(referenced|uptodate|lru|active|private|node=0|zone=2|lastcpupid=0x1fffff)
	%pGg	GFP_USER|GFP_DMA32|GFP_NOWARN
	%pGv	read|exec|mayread|maywrite|mayexec|denywrite

For printing flags bitfields as a collection of symbolic constants that
would construct the value. The type of flags is given by the third
character. Currently supported are:
(用于把标志位域打印为一组能构造出该值的符号常量。标志的类型由第三个字符给定。
当前支持：

        - p - [p]age flags, expects value of type (``unsigned long *``)(页标志，期望 ``unsigned long *`` 类型的值)
        - v - [v]ma_flags, expects value of type (``unsigned long *``)(vma 标志，期望 ``unsigned long *`` 类型的值)
        - g - [g]fp_flags, expects value of type (``gfp_t *``)(GFP 标志，期望 ``gfp_t *`` 类型的值)

The flag names and print order depends on the particular type.
(标志名称与打印顺序取决于具体的类型。)

Note that this format should not be used directly in the
:c:func:`TP_printk()` part of a tracepoint. Instead, use the show_*_flags()
functions from <trace/events/mmflags.h>.
(注意，此格式不应直接用于跟踪点的 :c:func:`TP_printk()` 部分。应改用
<trace/events/mmflags.h> 中的 show_*_flags() 函数。)

Passed by reference.
(按引用传递。)

Network device features(网络设备特性)
-------------------------------------

::

	%pNF	0x000000000000c000

For printing netdev_features_t.
(用于打印 netdev_features_t。)

Passed by reference.
(按引用传递。)

V4L2 and DRM FourCC code (pixel format)(V4L2 与 DRM FourCC 码（像素格式）)
--------------------------------------------------------------------------

::

	%p4cc

Print a FourCC code used by V4L2 or DRM, including format endianness and
its numerical value as hexadecimal.
(打印 V4L2 或 DRM 使用的 FourCC 码，包括格式的字节序及其十六进制数值。)

Passed by reference.
(按引用传递。)

Examples::

	%p4cc	BG12 little-endian (0x32314742)
	%p4cc	Y10  little-endian (0x20303159)
	%p4cc	NV12 big-endian (0xb231564e)

(示例：)

Generic FourCC code(通用 FourCC 码)
-----------------------------------

::
	%p4c[h[R]lb]	gP00 (0x67503030)

Print a generic FourCC code, as both ASCII characters and its numerical
value as hexadecimal.
(打印一个通用 FourCC 码，同时以 ASCII 字符和十六进制数值两种形式输出。)

The generic FourCC code is always printed in the big-endian format,
the most significant byte first. This is the opposite of V4L/DRM FourCCs.
(通用 FourCC 码总是以大端格式打印，最高有效字节在前。这与 V4L/DRM FourCC 正好
相反。)

The additional ``h``, ``hR``, ``l``, and ``b`` specifiers define what
endianness is used to load the stored bytes. The data might be interpreted
using the host, reversed host byte order, little-endian, or big-endian.
(附加的 ``h``、``hR``、``l`` 和 ``b`` 说明符定义加载已存储字节时所用的字节序。
数据可以按主机序、反转的主机序、小端序或大端序来解释。)

Passed by reference.
(按引用传递。)

Examples for a little-endian machine, given &(u32)0x67503030::

	%p4ch	gP00 (0x67503030)
	%p4chR	00Pg (0x30305067)
	%p4cl	gP00 (0x67503030)
	%p4cb	00Pg (0x30305067)

(小端机器上的示例，给定 &(u32)0x67503030：)

Examples for a big-endian machine, given &(u32)0x67503030::

	%p4ch	gP00 (0x67503030)
	%p4chR	00Pg (0x30305067)
	%p4cl	00Pg (0x30305067)
	%p4cb	gP00 (0x30305067)

(大端机器上的示例，给定 &(u32)0x67503030：)

Rust(Rust 语言)
---------------

::

	%pA

Only intended to be used from Rust code to format ``core::fmt::Arguments``.
Do *not* use it from C.
(仅打算在 Rust 代码中用于格式化 ``core::fmt::Arguments`` 。
不要在 C 代码中使用它。)

Thanks(致谢)
============

If you add other %p extensions, please extend <lib/tests/printf_kunit.c>
with one or more test cases, if at all feasible.
(如果你添加了其他 %p 扩展，请尽量为 <lib/tests/printf_kunit.c> 扩充一个或多个
测试用例。)

Thank you for your cooperation and attention.
(感谢你的配合与关注。)
