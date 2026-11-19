
==========================
eBPF verifier(eBPF 校验器)
==========================

The safety of the eBPF program is determined in two steps.
(eBPF 程序的安全性分两步确定。)

First step does DAG check to disallow loops and other CFG validation.
In particular it will detect programs that have unreachable instructions.
(though classic BPF checker allows them)
(第一步做 DAG 检查以禁止循环，并进行其他 CFG 校验。特别是它会检测含不可
达指令的程序(经典 BPF 检查器允许这类程序)。)

Second step starts from the first insn and descends all possible paths.
It simulates execution of every insn and observes the state change of
registers and stack.
(第二步从第一条指令出发，下行遍历所有可能的路径。它模拟每条指令的执行，
观察寄存器与栈的状态变化。)

At the start of the program the register R1 contains a pointer to context
and has type PTR_TO_CTX.
If verifier sees an insn that does R2=R1, then R2 has now type
PTR_TO_CTX as well and can be used on the right hand side of expression.
If R1=PTR_TO_CTX and insn is R2=R1+R1, then R2=SCALAR_VALUE,
since addition of two valid pointers makes invalid pointer.
(In 'secure' mode verifier will reject any type of pointer arithmetic to make
sure that kernel addresses don't leak to unprivileged users)
(程序开始时，寄存器 R1 包含一个指向上下文的指针，类型为 PTR_TO_CTX。如果
校验器看到一条执行 R2=R1 的指令，那么 R2 现在也具有 PTR_TO_CTX 类型，可以
用在表达式的右侧。如果 R1=PTR_TO_CTX 且指令是 R2=R1+R1，那么 R2=SCALAR_VALUE，
因为两个有效指针相加会得到无效指针。(在 'secure' 模式下，校验器会拒绝任
何类型的指针算术，以确保内核地址不泄露给无特权的用户。))

If register was never written to, it's not readable::

  bpf_mov R0 = R2
  bpf_exit

(如果某个寄存器从未被写入，它就是不可读的：)

will be rejected, since R2 is unreadable at the start of the program.
(这样的程序会被拒绝，因为 R2 在程序开始时不可读。)

After kernel function call, R1-R5 are reset to unreadable and
R0 has a return type of the function.
(内核函数调用之后，R1-R5 被重置为不可读，R0 具有该函数的返回类型。)

Since R6-R9 are callee saved, their state is preserved across the call.
(由于 R6-R9 是被调用者保存 (callee saved) 寄存器，它们的状态在调用期间得
以保留。)

::

  bpf_mov R6 = 1
  bpf_call foo
  bpf_mov R0 = R6
  bpf_exit

is a correct program. If there was R1 instead of R6, it would have
been rejected.
(是一个正确的程序。如果其中用 R1 而不是 R6，它就会被拒绝。)

load/store instructions are allowed only with registers of valid types, which
are PTR_TO_CTX, PTR_TO_MAP, PTR_TO_STACK. They are bounds and alignment checked.
For example::

 bpf_mov R1 = 1
 bpf_mov R2 = 2
 bpf_xadd *(u32 *)(R1 + 3) += R2
 bpf_exit

(load/store 指令只允许用于具有有效类型的寄存器，即 PTR_TO_CTX、PTR_TO_MAP、
PTR_TO_STACK。它们会做边界与对齐检查。例如：)

(load/store 指令只允许用于具有有效类型的寄存器，即 PTR_TO_CTX、PTR_TO_MAP、
PTR_TO_STACK。它们会做边界与对齐检查。例如：)

will be rejected, since R1 doesn't have a valid pointer type at the time of
execution of instruction bpf_xadd.
(会被拒绝，因为执行 bpf_xadd 指令时 R1 不具有有效的指针类型。)

At the start R1 type is PTR_TO_CTX (a pointer to generic ``struct bpf_context``)
A callback is used to customize verifier to restrict eBPF program access to only
certain fields within ctx structure with specified size and alignment.
(开始时 R1 的类型是 PTR_TO_CTX(指向通用 ``struct bpf_context`` 的指针)。
通过一个回调来定制校验器，把 eBPF 程序对 ctx 结构的访问限制为其中某些指
定大小与对齐方式的字段。)

For example, the following insn::

  bpf_ld R0 = *(u32 *)(R6 + 8)

(例如，下面这条指令：)

intends to load a word from address R6 + 8 and store it into R0
If R6=PTR_TO_CTX, via is_valid_access() callback the verifier will know
that offset 8 of size 4 bytes can be accessed for reading, otherwise
the verifier will reject the program.
If R6=PTR_TO_STACK, then access should be aligned and be within
stack bounds, which are [-MAX_BPF_STACK, 0). In this example offset is 8,
so it will fail verification, since it's out of bounds.
(打算从地址 R6 + 8 加载一个字并存入 R0。如果 R6=PTR_TO_CTX，校验器通过
is_valid_access() 回调会知道偏移 8 处、大小 4 字节可读，否则校验器将拒绝
该程序。如果 R6=PTR_TO_STACK，则访问应当对齐并位于栈边界内，栈边界是
[-MAX_BPF_STACK, 0)。本例中偏移是 8，超出了边界，因此无法通过校验。)

The verifier will allow eBPF program to read data from stack only after
it wrote into it.
(校验器只允许 eBPF 程序在写入栈之后才从栈中读取数据。)

Classic BPF verifier does similar check with M[0-15] memory slots.
For example::

  bpf_ld R0 = *(u32 *)(R10 - 4)
  bpf_exit

(经典 BPF 校验器用 M[0-15] 内存槽做类似的检查。例如：)

(经典 BPF 校验器用 M[0-15] 内存槽做类似的检查。例如：)

is invalid program.
Though R10 is correct read-only register and has type PTR_TO_STACK
and R10 - 4 is within stack bounds, there were no stores into that location.
(是无效程序。虽然 R10 是正确的只读寄存器且类型为 PTR_TO_STACK，R10 - 4 也
在栈边界内，但该位置从未有过写入。)

Pointer register spill/fill is tracked as well, since four (R6-R9)
callee saved registers may not be enough for some programs.
(指针寄存器的溢出/重载 (spill/fill) 也会被跟踪，因为四个 (R6-R9) 被调用
者保存寄存器对某些程序来说可能不够。)

Allowed function calls are customized with bpf_verifier_ops->get_func_proto()
The eBPF verifier will check that registers match argument constraints.
After the call register R0 will be set to return type of the function.
(允许的函数调用通过 bpf_verifier_ops->get_func_proto() 定制。eBPF 校验器
会检查寄存器是否满足参数约束。调用之后，寄存器 R0 会被设置为该函数的返回
类型。)

Function calls is a main mechanism to extend functionality of eBPF programs.
Socket filters may let programs to call one set of functions, whereas tracing
filters may allow completely different set.
(函数调用是扩展 eBPF 程序功能的主要机制。套接字过滤器可能允许程序调用某
一组函数，而追踪过滤器可能允许完全不同的一组。)

If a function made accessible to eBPF program, it needs to be thought through
from safety point of view. The verifier will guarantee that the function is
called with valid arguments.
(如果一个函数被开放给 eBPF 程序，就需要从安全角度把它想清楚。校验器会保
证该函数以有效参数被调用。)

seccomp vs socket filters have different security restrictions for classic BPF.
Seccomp solves this by two stage verifier: classic BPF verifier is followed
by seccomp verifier. In case of eBPF one configurable verifier is shared for
all use cases.
(对于经典 BPF，seccomp 与套接字过滤器有不同的安全限制。seccomp 用两级校验
器解决这个问题：经典 BPF 校验器之后跟着 seccomp 校验器。而对 eBPF，所有
用例共享同一个可配置的校验器。)

See details of eBPF verifier in kernel/bpf/verifier.c
(eBPF 校验器的细节见 kernel/bpf/verifier.c。)

Register value tracking(寄存器值跟踪)
=====================================

In order to determine the safety of an eBPF program, the verifier must track
the range of possible values in each register and also in each stack slot.
This is done with ``struct bpf_reg_state``, defined in include/linux/
bpf_verifier.h, which unifies tracking of scalar and pointer values.  Each
register state has a type, which is either NOT_INIT (the register has not been
written to), SCALAR_VALUE (some value which is not usable as a pointer), or a
pointer type.  The types of pointers describe their base, as follows:
(为了确定 eBPF 程序的安全性，校验器必须跟踪每个寄存器以及每个栈槽中可能
值的范围。这是用 ``struct bpf_reg_state`` 完成的，它定义于 include/linux/
bpf_verifier.h，统一了标量值与指针值的跟踪。每个寄存器状态都有一个类型：
NOT_INIT(该寄存器从未被写入)、SCALAR_VALUE(某个不能用作指针的值)或某个
指针类型。指针类型描述它们的基址，如下：)

    PTR_TO_CTX
			Pointer to bpf_context.
			(指向 bpf_context 的指针。)
    CONST_PTR_TO_MAP
			Pointer to struct bpf_map.  "Const" because arithmetic
			on these pointers is forbidden.
			(指向 struct bpf_map 的指针。"Const" 是因为禁止对这类
			指针做算术运算。)
    PTR_TO_MAP_VALUE
			Pointer to the value stored in a map element.
			(指向 map 元素中所存值的指针。)
    PTR_TO_MAP_VALUE_OR_NULL
			Either a pointer to a map value, or NULL; map accesses
			(see maps.rst) return this type, which becomes a
			PTR_TO_MAP_VALUE when checked != NULL. Arithmetic on
			these pointers is forbidden.
			(要么是指向 map 值的指针，要么是 NULL；map 访问(见
			maps.rst)返回此类型，当检查确认 != NULL 后它会变成
			PTR_TO_MAP_VALUE。禁止对这类指针做算术运算。)
    PTR_TO_STACK
			Frame pointer.
			(帧指针。)
    PTR_TO_PACKET
			skb->data.
    PTR_TO_PACKET_END
			skb->data + headlen; arithmetic forbidden.
			(skb->data + headlen；禁止算术运算。)
    PTR_TO_SOCKET
			Pointer to struct bpf_sock_ops, implicitly refcounted.
			(指向 struct bpf_sock_ops 的指针，隐式带引用计数。)
    PTR_TO_SOCKET_OR_NULL
			Either a pointer to a socket, or NULL; socket lookup
			returns this type, which becomes a PTR_TO_SOCKET when
			checked != NULL. PTR_TO_SOCKET is reference-counted,
			so programs must release the reference through the
			socket release function before the end of the program.
			Arithmetic on these pointers is forbidden.
			(要么是指向套接字的指针，要么是 NULL；socket lookup 返
			回此类型，当检查确认 != NULL 后它会变成 PTR_TO_SOCKET。
			PTR_TO_SOCKET 带引用计数，因此程序必须在程序结束前通过
			socket release 函数释放该引用。禁止对这类指针做算术运
			算。)

However, a pointer may be offset from this base (as a result of pointer
arithmetic), and this is tracked in two parts: the 'fixed offset' and 'variable
offset'.  The former is used when an exactly-known value (e.g. an immediate
operand) is added to a pointer, while the latter is used for values which are
not exactly known.  The variable offset is also used in SCALAR_VALUEs, to track
the range of possible values in the register.
(不过，指针可能偏离这个基址(作为指针算术的结果)，这部分被拆成两个部分来
跟踪："固定偏移" (fixed offset) 与 "可变偏移" (variable offset)。前者用于
把一个确切已知的值(例如立即数操作数)加到指针上的情形，后者用于不那么确切
的值。SCALAR_VALUE 也使用可变偏移，用来跟踪寄存器中可能值的范围。)

The verifier's knowledge about the variable offset consists of:
(校验器对可变偏移的了解包括：)

* minimum and maximum values as unsigned
  (以无符号表示的最小值与最大值)
* minimum and maximum values as signed
  (以有符号表示的最小值与最大值)

* knowledge of the values of individual bits, in the form of a 'tnum': a u64
  'mask' and a u64 'value'.  1s in the mask represent bits whose value is unknown;
  1s in the value represent bits known to be 1.  Bits known to be 0 have 0 in both
  mask and value; no bit should ever be 1 in both.  For example, if a byte is read
  into a register from memory, the register's top 56 bits are known zero, while
  the low 8 are unknown - which is represented as the tnum (0x0; 0xff).  If we
  then OR this with 0x40, we get (0x40; 0xbf), then if we add 1 we get (0x0;
  0x1ff), because of potential carries.
  (对各个位取值的了解，以 'tnum' 的形式表示：一个 u64 的 'mask' 与一个 u64
  的 'value'。mask 中的 1 表示取值未知的位；value 中的 1 表示已知为 1 的
  位。已知为 0 的位在 mask 与 value 中都是 0；任何位都不应同时在两者中为
  1。例如，从内存把一个字节读入寄存器后，寄存器高 56 位已知为零，而低 8
  位未知——表示为 tnum (0x0; 0xff)。接着若与 0x40 做或运算，得到 (0x40;
  0xbf)；再加 1 则得到 (0x0; 0x1ff)，因为可能产生进位。)

Besides arithmetic, the register state can also be updated by conditional
branches.  For instance, if a SCALAR_VALUE is compared > 8, in the 'true' branch
it will have a umin_value (unsigned minimum value) of 9, whereas in the 'false'
branch it will have a umax_value of 8.  A signed compare (with BPF_JSGT or
BPF_JSGE) would instead update the signed minimum/maximum values.  Information
from the signed and unsigned bounds can be combined; for instance if a value is
first tested < 8 and then tested s> 4, the verifier will conclude that the value
is also > 4 and s< 8, since the bounds prevent crossing the sign boundary.
(除了算术运算，寄存器状态还会被条件分支更新。例如，如果一个 SCALAR_VALUE
被比较 > 8，在 'true' 分支中它将有 umin_value(无符号最小值)9，而在
'false' 分支中它将有 umax_value 8。有符号比较(用 BPF_JSGT 或 BPF_JSGE)则
会更新有符号的最小/最大值。有符号与无符号边界的信息可以结合；例如，如果
一个值先被测试 < 8，然后又被测试 s> 4，校验器会得出该值也 > 4 且 s< 8 的
结论，因为这些边界阻止越过符号边界。)

PTR_TO_PACKETs with a variable offset part have an 'id', which is common to all
pointers sharing that same variable offset.  This is important for packet range
checks: after adding a variable to a packet pointer register A, if you then copy
it to another register B and then add a constant 4 to A, both registers will
share the same 'id' but the A will have a fixed offset of +4.  Then if A is
bounds-checked and found to be less than a PTR_TO_PACKET_END, the register B is
now known to have a safe range of at least 4 bytes.  See 'Direct packet access',
below, for more on PTR_TO_PACKET ranges.
(带可变偏移部分的 PTR_TO_PACKET 有一个 'id'，所有共享同一可变偏移的指针都
有相同的 id。这对数据包范围检查很重要：在把一个变量加到数据包指针寄存器 A
之后，如果你随后把它拷贝到另一个寄存器 B，再给 A 加上常数 4，两个寄存器将
共享同一个 'id'，但 A 会带有 +4 的固定偏移。于是如果 A 做了边界检查且发现
小于某个 PTR_TO_PACKET_END，那么就知道寄存器 B 现在拥有至少 4 字节的安全
范围。PTR_TO_PACKET 范围的更多信息见下文 "Direct packet access"。)

The 'id' field is also used on PTR_TO_MAP_VALUE_OR_NULL, common to all copies of
the pointer returned from a map lookup.  This means that when one copy is
checked and found to be non-NULL, all copies can become PTR_TO_MAP_VALUEs.
As well as range-checking, the tracked information is also used for enforcing
alignment of pointer accesses.  For instance, on most systems the packet pointer
is 2 bytes after a 4-byte alignment.  If a program adds 14 bytes to that to jump
over the Ethernet header, then reads IHL and adds (IHL * 4), the resulting
pointer will have a variable offset known to be 4n+2 for some n, so adding the 2
bytes (NET_IP_ALIGN) gives a 4-byte alignment and so word-sized accesses through
that pointer are safe.
('id' 字段也用于 PTR_TO_MAP_VALUE_OR_NULL，对 map lookup 返回的指针的所有
副本都相同。这意味着当其中一个副本被检查并确认非 NULL 时，所有副本都可以
变成 PTR_TO_MAP_VALUE。除范围检查外，所跟踪的信息还用于强制指针访问的对
齐。例如，在大多数系统上，数据包指针在 4 字节对齐之后又偏移 2 字节。如果
程序在其上加 14 字节以跳过以太网头，然后读取 IHL 并加上 (IHL * 4)，所得指
针的可变偏移将已知为某个 n 的 4n+2，因此加上 2 字节 (NET_IP_ALIGN) 后就达
到 4 字节对齐，通过该指针做的字长访问是安全的。)

The 'id' field is also used on PTR_TO_SOCKET and PTR_TO_SOCKET_OR_NULL, common
to all copies of the pointer returned from a socket lookup. This has similar
behaviour to the handling for PTR_TO_MAP_VALUE_OR_NULL->PTR_TO_MAP_VALUE, but
it also handles reference tracking for the pointer. PTR_TO_SOCKET implicitly
represents a reference to the corresponding ``struct sock``. To ensure that the
reference is not leaked, it is imperative to NULL-check the reference and in
the non-NULL case, and pass the valid reference to the socket release function.
('id' 字段也用于 PTR_TO_SOCKET 与 PTR_TO_SOCKET_OR_NULL，对 socket lookup 返
回的指针的所有副本都相同。其行为与 PTR_TO_MAP_VALUE_OR_NULL->
PTR_TO_MAP_VALUE 的处理类似，但它还负责该指针的引用跟踪。PTR_TO_SOCKET 隐
式地表示对相应 ``struct sock`` 的一个引用。为确保引用不泄漏，必须对该引用
做 NULL 检查，并在非 NULL 的情况下，把有效引用传给 socket release 函数。)

Direct packet access(直接数据包访问)
====================================

In cls_bpf and act_bpf programs the verifier allows direct access to the packet
data via skb->data and skb->data_end pointers.
Ex::

    1:  r4 = *(u32 *)(r1 +80)  /* load skb->data_end */
    2:  r3 = *(u32 *)(r1 +76)  /* load skb->data */
    3:  r5 = r3
    4:  r5 += 14
    5:  if r5 > r4 goto pc+16
    R1=ctx R3=pkt(id=0,off=0,r=14) R4=pkt_end R5=pkt(id=0,off=14,r=14) R10=fp
    6:  r0 = *(u16 *)(r3 +12) /* access 12 and 13 bytes of the packet */

(在 cls_bpf 与 act_bpf 程序中，校验器允许通过 skb->data 与 skb->data_end 指
针直接访问数据包数据。例如：)

this 2byte load from the packet is safe to do, since the program author
did check ``if (skb->data + 14 > skb->data_end) goto err`` at insn #5 which
means that in the fall-through case the register R3 (which points to skb->data)
has at least 14 directly accessible bytes. The verifier marks it
as R3=pkt(id=0,off=0,r=14).
id=0 means that no additional variables were added to the register.
off=0 means that no additional constants were added.
r=14 is the range of safe access which means that bytes [R3, R3 + 14) are ok.
Note that R5 is marked as R5=pkt(id=0,off=14,r=14). It also points
to the packet data, but constant 14 was added to the register, so
it now points to ``skb->data + 14`` and accessible range is [R5, R5 + 14 - 14)
which is zero bytes.

(上面这个从数据包中加载 2 字节的做法是安全的，因为程序作者在指令 #5 处做
了 ``if (skb->data + 14 > skb->data_end) goto err`` 检查，这意味着在直落
(fall-through)的情况下，寄存器 R3(指向 skb->data)至少有 14 个可直接访问
的字节。校验器把它标记为 R3=pkt(id=0,off=0,r=14)。id=0 表示没有向寄存器
添加额外变量；off=0 表示没有添加额外常数；r=14 是安全访问范围，表示字节
[R3, R3 + 14) 是可以的。注意 R5 被标记为 R5=pkt(id=0,off=14,r=14)。它也指
向数据包数据，但由于向寄存器加了常数 14，它现在指向 ``skb->data + 14``，
可访问范围是 [R5, R5 + 14 - 14)，即 0 字节。)

More complex packet access may look like::

    R0=inv1 R1=ctx R3=pkt(id=0,off=0,r=14) R4=pkt_end R5=pkt(id=0,off=14,r=14) R10=fp
    6:  r0 = *(u8 *)(r3 +7) /* load 7th byte from the packet */
    7:  r4 = *(u8 *)(r3 +12)
    8:  r4 *= 14
    9:  r3 = *(u32 *)(r1 +76) /* load skb->data */
    10:  r3 += r4
    11:  r2 = r1
    12:  r2 <<= 48
    13:  r2 >>= 48
    14:  r3 += r2
    15:  r2 = r3
    16:  r2 += 8
    17:  r1 = *(u32 *)(r1 +80) /* load skb->data_end */
    18:  if r2 > r1 goto pc+2
    R0=inv(id=0,umax_value=255,var_off=(0x0; 0xff)) R1=pkt_end R2=pkt(id=2,off=8,r=8) R3=pkt(id=2,off=0,r=8) R4=inv(id=0,umax_value=3570,var_off=(0x0; 0xfffe)) R5=pkt(id=0,off=14,r=14) R10=fp
    19:  r1 = *(u8 *)(r3 +4)

The state of the register R3 is R3=pkt(id=2,off=0,r=8)
id=2 means that two ``r3 += rX`` instructions were seen, so r3 points to some
offset within a packet and since the program author did
``if (r3 + 8 > r1) goto err`` at insn #18, the safe range is [R3, R3 + 8).
The verifier only allows 'add'/'sub' operations on packet registers. Any other
operation will set the register state to 'SCALAR_VALUE' and it won't be
available for direct packet access.

(更复杂的数据包访问可能形如：)
(寄存器 R3 的状态是 R3=pkt(id=2,off=0,r=8)。id=2 表示看到了两条 ``r3 += rX``
指令，因此 r3 指向数据包内的某个偏移；由于程序作者在指令 #18 处做了
``if (r3 + 8 > r1) goto err`` 检查，安全范围是 [R3, R3 + 8)。校验器只允许
对数据包寄存器做 'add'/'sub' 操作。任何其他操作都会把寄存器状态置为
'SCALAR_VALUE'，它将不能再用于直接数据包访问。)

Operation ``r3 += rX`` may overflow and become less than original skb->data,
therefore the verifier has to prevent that.  So when it sees ``r3 += rX``
instruction and rX is more than 16-bit value, any subsequent bounds-check of r3
against skb->data_end will not give us 'range' information, so attempts to read
through the pointer will give "invalid access to packet" error.
(操作 ``r3 += rX`` 可能溢出而变得小于原始的 skb->data，因此校验器必须防止
这种情况。所以当它看到 ``r3 += rX`` 指令且 rX 大于 16 位值时，后续任何把
r3 与 skb->data_end 做的边界检查都不会给出 'range' 信息，于是通过该指针读
取的尝试会报 "invalid access to packet" 错误。)

Ex. after insn ``r4 = *(u8 *)(r3 +12)`` (insn #7 above) the state of r4 is
R4=inv(id=0,umax_value=255,var_off=(0x0; 0xff)) which means that upper 56 bits
of the register are guaranteed to be zero, and nothing is known about the lower
8 bits. After insn ``r4 *= 14`` the state becomes
R4=inv(id=0,umax_value=3570,var_off=(0x0; 0xfffe)), since multiplying an 8-bit
value by constant 14 will keep upper 52 bits as zero, also the least significant
bit will be zero as 14 is even.  Similarly ``r2 >>= 48`` will make
R2=inv(id=0,umax_value=65535,var_off=(0x0; 0xffff)), since the shift is not sign
extending.  This logic is implemented in adjust_reg_min_max_vals() function,
which calls adjust_ptr_min_max_vals() for adding pointer to scalar (or vice
versa) and adjust_scalar_min_max_vals() for operations on two scalars.
(例如，在指令 ``r4 = *(u8 *)(r3 +12)``(上面的指令 #7)之后，r4 的状态是
R4=inv(id=0,umax_value=255,var_off=(0x0; 0xff))，这意味着寄存器高 56 位保证
为零，而低 8 位一无所知。在指令 ``r4 *= 14`` 之后，状态变为
R4=inv(id=0,umax_value=3570,var_off=(0x0; 0xfffe))，因为 8 位值乘以常数 14
会使高 52 位保持为零，而且由于 14 是偶数，最低位也会是零。类似地，
``r2 >>= 48`` 会使 R2=inv(id=0,umax_value=65535,var_off=(0x0; 0xffff))，因
为该移位不做符号扩展。这一逻辑实现在 adjust_reg_min_max_vals() 函数中，它
对"指针加标量(或反过来)"调用 adjust_ptr_min_max_vals()，对两个标量之间的
操作调用 adjust_scalar_min_max_vals()。)

The end result is that bpf program author can access packet directly
using normal C code as::

  void *data = (void *)(long)skb->data;
  void *data_end = (void *)(long)skb->data_end;
  struct eth_hdr *eth = data;
  struct iphdr *iph = data + sizeof(*eth);
  struct udphdr *udp = data + sizeof(*eth) + sizeof(*iph);

  if (data + sizeof(*eth) + sizeof(*iph) + sizeof(*udp) > data_end)
	  return 0;
  if (eth->h_proto != htons(ETH_P_IP))
	  return 0;
  if (iph->protocol != IPPROTO_UDP || iph->ihl != 5)
	  return 0;
  if (udp->dest == 53 || udp->source == 9)
	  ...;

which makes such programs easier to write comparing to LD_ABS insn
and significantly faster.
(与 LD_ABS 指令相比，这使此类程序更易编写，而且明显更快。)

Pruning(剪枝)
=============

The verifier does not actually walk all possible paths through the program.  For
each new branch to analyse, the verifier looks at all the states it's previously
been in when at this instruction.  If any of them contain the current state as a
subset, the branch is 'pruned' - that is, the fact that the previous state was
accepted implies the current state would be as well.  For instance, if in the
previous state, r1 held a packet-pointer, and in the current state, r1 holds a
packet-pointer with a range as long or longer and at least as strict an
alignment, then r1 is safe.  Similarly, if r2 was NOT_INIT before then it can't
have been used by any path from that point, so any value in r2 (including
another NOT_INIT) is safe.  The implementation is in the function regsafe().
Pruning considers not only the registers but also the stack (and any spilled
registers it may hold).  They must all be safe for the branch to be pruned.
This is implemented in states_equal().
(校验器实际上并不遍历程序中所有可能的路径。对每个要分析的新分支，校验器会
查看它此前处于该指令时的所有状态。如果其中任何一个以当前状态为子集，该分
支就会被"剪枝" (pruned)——也就是说，之前那个状态被接受，就意味着当前状态
也会被接受。例如，如果之前状态中 r1 持有一个数据包指针，而当前状态中 r1
持有一个范围同样长或更长、对齐至少同样严格的数据包指针，那么 r1 是安全
的。类似地，如果 r2 之前是 NOT_INIT，那么从那一点出发的任何路径都不可能用
过它，因此 r2 中的任何值(包括另一个 NOT_INIT)都是安全的。实现位于函数
regsafe() 中。剪枝不仅考虑寄存器，还考虑栈(以及它可能持有的溢出寄存器)。
要剪掉一个分支，它们必须全部安全。这实现在 states_equal() 中。)

Some technical details about state pruning implementation could be found below.
(关于状态剪枝实现的一些技术细节见下文。)

Register liveness tracking(寄存器活跃性跟踪)
--------------------------------------------

In order to make state pruning effective, liveness state is tracked for each
register and stack slot. The basic idea is to track which registers and stack
slots are actually used during subseqeuent execution of the program, until
program exit is reached. Registers and stack slots that were never used could be
removed from the cached state thus making more states equivalent to a cached
state. This could be illustrated by the following program::

  0: call bpf_get_prandom_u32()
  1: r1 = 0
  2: if r0 == 0 goto +1
  3: r0 = 1
  --- checkpoint ---
  4: r0 = r1
  5: exit

(为了让状态剪枝有效，会为每个寄存器和栈槽跟踪活跃性 (liveness) 状态。基本
思想是：跟踪在程序后续执行中、直到到达程序出口为止，哪些寄存器和栈槽被实
际使用。从未被使用的寄存器和栈槽可以从缓存状态中剔除，从而使更多状态与某
个缓存状态等价。下面的程序可以说明这一点：)

(为了让状态剪枝有效，会为每个寄存器和栈槽跟踪活跃性 (liveness) 状态。基本
思想是：跟踪在程序后续执行中、直到到达程序出口为止，哪些寄存器和栈槽被实
际使用。从未被使用的寄存器和栈槽可以从缓存状态中剔除，从而使更多状态与某
个缓存状态等价。下面的程序可以说明这一点：)

Suppose that a state cache entry is created at instruction #4 (such entries are
also called "checkpoints" in the text below). The verifier could reach the
instruction with one of two possible register states:
(假设在指令 #4 处创建了一个状态缓存条目(这类条目在下文中也被称为"检查
点" (checkpoint))。校验器可能以两种可能的寄存器状态到达该指令：)

* r0 = 1, r1 = 0
* r0 = 0, r1 = 0

However, only the value of register ``r1`` is important to successfully finish
verification. The goal of the liveness tracking algorithm is to spot this fact
and figure out that both states are actually equivalent.
(然而，要成功完成校验，只有寄存器 ``r1`` 的值是重要的。活跃性跟踪算法的
目标就是发现这一事实，并断定这两种状态实际上是等价的。)

Understanding eBPF verifier messages(理解 eBPF 校验器消息)
==========================================================

The following are few examples of invalid eBPF programs and verifier error
messages as seen in the log:
(下面是若干无效 eBPF 程序的例子，以及在日志中所见到的校验器错误消息：)

Program with unreachable instructions::

  static struct bpf_insn prog[] = {
  BPF_EXIT_INSN(),
  BPF_EXIT_INSN(),
  };

(含不可达指令的程序：)

Error::

  unreachable insn 1

(报错：)

Program that reads uninitialized register::

  BPF_MOV64_REG(BPF_REG_0, BPF_REG_2),
  BPF_EXIT_INSN(),

(读取未初始化寄存器的程序：)

Error::

  0: (bf) r0 = r2
  R2 !read_ok

(报错：)

Program that doesn't initialize R0 before exiting::

  BPF_MOV64_REG(BPF_REG_2, BPF_REG_1),
  BPF_EXIT_INSN(),

(退出前未初始化 R0 的程序：)

Error::

  0: (bf) r2 = r1
  1: (95) exit
  R0 !read_ok

(报错：)

Program that accesses stack out of bounds::

    BPF_ST_MEM(BPF_DW, BPF_REG_10, 8, 0),
    BPF_EXIT_INSN(),

(越界访问栈的程序：)

Error::

    0: (7a) *(u64 *)(r10 +8) = 0
    invalid stack off=8 size=8

(报错：)

Program that doesn't initialize stack before passing its address into function::

  BPF_MOV64_REG(BPF_REG_2, BPF_REG_10),
  BPF_ALU64_IMM(BPF_ADD, BPF_REG_2, -8),
  BPF_LD_MAP_FD(BPF_REG_1, 0),
  BPF_RAW_INSN(BPF_JMP | BPF_CALL, 0, 0, 0, BPF_FUNC_map_lookup_elem),
  BPF_EXIT_INSN(),

(在把栈地址传入函数之前未初始化栈的程序：)

Error::

  0: (bf) r2 = r10
  1: (07) r2 += -8
  2: (b7) r1 = 0x0
  3: (85) call 1
  invalid indirect read from stack off -8+0 size 8

(报错：)

Program that uses invalid map_fd=0 while calling to map_lookup_elem() function::

  BPF_ST_MEM(BPF_DW, BPF_REG_10, -8, 0),
  BPF_MOV64_REG(BPF_REG_2, BPF_REG_10),
  BPF_ALU64_IMM(BPF_ADD, BPF_REG_2, -8),
  BPF_LD_MAP_FD(BPF_REG_1, 0),
  BPF_RAW_INSN(BPF_JMP | BPF_CALL, 0, 0, 0, BPF_FUNC_map_lookup_elem),
  BPF_EXIT_INSN(),

(调用 map_lookup_elem() 函数时使用无效 map_fd=0 的程序：)

Error::

  0: (7a) *(u64 *)(r10 -8) = 0
  1: (bf) r2 = r10
  2: (07) r2 += -8
  3: (b7) r1 = 0x0
  4: (85) call 1
  fd 0 is not pointing to valid bpf_map

(报错：)

Program that doesn't check return value of map_lookup_elem() before accessing
map element::

  BPF_ST_MEM(BPF_DW, BPF_REG_10, -8, 0),
  BPF_MOV64_REG(BPF_REG_2, BPF_REG_10),
  BPF_ALU64_IMM(BPF_ADD, BPF_REG_2, -8),
  BPF_LD_MAP_FD(BPF_REG_1, 0),
  BPF_RAW_INSN(BPF_JMP | BPF_CALL, 0, 0, 0, BPF_FUNC_map_lookup_elem),
  BPF_ST_MEM(BPF_DW, BPF_REG_0, 0, 0),
  BPF_EXIT_INSN(),

(访问 map 元素前未检查 map_lookup_elem() 返回值的程序：)

Error::

  0: (7a) *(u64 *)(r10 -8) = 0
  1: (bf) r2 = r10
  2: (07) r2 += -8
  3: (b7) r1 = 0x0
  4: (85) call 1
  5: (7a) *(u64 *)(r0 +0) = 0
  R0 invalid mem access 'map_value_or_null'

(报错：)

Program that correctly checks map_lookup_elem() returned value for NULL, but
accesses the memory with incorrect alignment::

  BPF_ST_MEM(BPF_DW, BPF_REG_10, -8, 0),
  BPF_MOV64_REG(BPF_REG_2, BPF_REG_10),
  BPF_ALU64_IMM(BPF_ADD, BPF_REG_2, -8),
  BPF_LD_MAP_FD(BPF_REG_1, 0),
  BPF_RAW_INSN(BPF_JMP | BPF_CALL, 0, 0, 0, BPF_FUNC_map_lookup_elem),
  BPF_JMP_IMM(BPF_JEQ, BPF_REG_0, 0, 1),
  BPF_ST_MEM(BPF_DW, BPF_REG_0, 4, 0),
  BPF_EXIT_INSN(),

(正确检查了 map_lookup_elem() 返回值是否为 NULL，但以错误对齐访问内存的程序：)

Error::

  0: (7a) *(u64 *)(r10 -8) = 0
  1: (bf) r2 = r10
  2: (07) r2 += -8
  3: (b7) r1 = 1
  4: (85) call 1
  5: (15) if r0 == 0x0 goto pc+1
   R0=map_ptr R10=fp
  6: (7a) *(u64 *)(r0 +4) = 0
  misaligned access off 4 size 8

(报错：)

Program that correctly checks map_lookup_elem() returned value for NULL and
accesses memory with correct alignment in one side of 'if' branch, but fails
to do so in the other side of 'if' branch::

  BPF_ST_MEM(BPF_DW, BPF_REG_10, -8, 0),
  BPF_MOV64_REG(BPF_REG_2, BPF_REG_10),
  BPF_ALU64_IMM(BPF_ADD, BPF_REG_2, -8),
  BPF_LD_MAP_FD(BPF_REG_1, 0),
  BPF_RAW_INSN(BPF_JMP | BPF_CALL, 0, 0, 0, BPF_FUNC_map_lookup_elem),
  BPF_JMP_IMM(BPF_JEQ, BPF_REG_0, 0, 2),
  BPF_ST_MEM(BPF_DW, BPF_REG_0, 0, 0),
  BPF_EXIT_INSN(),
  BPF_ST_MEM(BPF_DW, BPF_REG_0, 0, 1),
  BPF_EXIT_INSN(),

(在 'if' 分支的一侧正确检查了 map_lookup_elem() 返回值是否为 NULL 并以正确对齐访问内存，但在另一侧未能这样做的程序：)

Error::

  0: (7a) *(u64 *)(r10 -8) = 0
  1: (bf) r2 = r10
  2: (07) r2 += -8
  3: (b7) r1 = 1
  4: (85) call 1
  5: (15) if r0 == 0x0 goto pc+2
   R0=map_ptr R10=fp
  6: (7a) *(u64 *)(r0 +0) = 0
  7: (95) exit

  from 5 to 8: R0=imm0 R10=fp
  8: (7a) *(u64 *)(r0 +0) = 1
  R0 invalid mem access 'imm'

(报错：)

Program that performs a socket lookup then sets the pointer to NULL without
checking it::

  BPF_MOV64_IMM(BPF_REG_2, 0),
  BPF_STX_MEM(BPF_W, BPF_REG_10, BPF_REG_2, -8),
  BPF_MOV64_REG(BPF_REG_2, BPF_REG_10),
  BPF_ALU64_IMM(BPF_ADD, BPF_REG_2, -8),
  BPF_MOV64_IMM(BPF_REG_3, 4),
  BPF_MOV64_IMM(BPF_REG_4, 0),
  BPF_MOV64_IMM(BPF_REG_5, 0),
  BPF_EMIT_CALL(BPF_FUNC_sk_lookup_tcp),
  BPF_MOV64_IMM(BPF_REG_0, 0),
  BPF_EXIT_INSN(),

(执行 socket lookup 然后未加检查就把指针置为 NULL 的程序：)

Error::

  0: (b7) r2 = 0
  1: (63) *(u32 *)(r10 -8) = r2
  2: (bf) r2 = r10
  3: (07) r2 += -8
  4: (b7) r3 = 4
  5: (b7) r4 = 0
  6: (b7) r5 = 0
  7: (85) call bpf_sk_lookup_tcp#65
  8: (b7) r0 = 0
  9: (95) exit
  Unreleased reference id=1, alloc_insn=7

(报错：)

Program that performs a socket lookup but does not NULL-check the returned
value::

  BPF_MOV64_IMM(BPF_REG_2, 0),
  BPF_STX_MEM(BPF_W, BPF_REG_10, BPF_REG_2, -8),
  BPF_MOV64_REG(BPF_REG_2, BPF_REG_10),
  BPF_ALU64_IMM(BPF_ADD, BPF_REG_2, -8),
  BPF_MOV64_IMM(BPF_REG_3, 4),
  BPF_MOV64_IMM(BPF_REG_4, 0),
  BPF_MOV64_IMM(BPF_REG_5, 0),
  BPF_EMIT_CALL(BPF_FUNC_sk_lookup_tcp),
  BPF_EXIT_INSN(),

(执行 socket lookup 但不对返回值做 NULL 检查的程序：)

Error::

  0: (b7) r2 = 0
  1: (63) *(u32 *)(r10 -8) = r2
  2: (bf) r2 = r10
  3: (07) r2 += -8
  4: (b7) r3 = 4
  5: (b7) r4 = 0
  6: (b7) r5 = 0
  7: (85) call bpf_sk_lookup_tcp#65
  8: (95) exit
  Unreleased reference id=1, alloc_insn=7

(报错：)
