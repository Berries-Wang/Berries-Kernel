============================
BPF Design Q&A(BPF 设计问答)
============================

BPF extensibility and applicability to networking, tracing, security
in the linux kernel and several user space implementations of BPF
virtual machine led to a number of misunderstanding on what BPF actually is.
This short QA is an attempt to address that and outline a direction
of where BPF is heading long term.
(BPF 的可扩展性及其在网络、跟踪、安全方面对 Linux 内核的适用性，加上若
干用户空间的 BPF 虚拟机实现，导致了对 BPF 究竟是什么的诸多误解。这份简
短的问答试图澄清这一点，并勾勒出 BPF 长期的发展方向。)

.. contents::
    :local:
    :depth: 3

Questions and Answers(问题与解答)
=================================

Q: Is BPF a generic instruction set similar to x64 and arm64?(问：BPF 是类似 x64 和 arm64 的通用指令集吗？)
-----------------------------------------------------------------------------------------------------------
A: NO.
(否。)

Q: Is BPF a generic virtual machine ?(问：BPF 是通用虚拟机吗？)
---------------------------------------------------------------
A: NO.
(否。)

BPF is generic instruction set *with* C calling convention.(BPF 是带 C 调用约定的通用指令集)
--------------------------------------------------------------------------------------------

Q: Why C calling convention was chosen?(问：为什么选择 C 调用约定？)
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

A: Because BPF programs are designed to run in the linux kernel
which is written in C, hence BPF defines instruction set compatible
with two most used architectures x64 and arm64 (and takes into
consideration important quirks of other architectures) and
defines calling convention that is compatible with C calling
convention of the linux kernel on those architectures.
(A：因为 BPF 程序被设计为运行在用 C 编写的 Linux 内核中，所以 BPF 定义
了与最常用的两种架构 x64 和 arm64 兼容的指令集 (并考虑了其他架构的重要
怪癖)，并定义了与 Linux 内核在这些架构上的 C 调用约定相兼容的调用约定。)

Q: Can multiple return values be supported in the future?(问：将来能支持多个返回值吗？)
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
A: NO. BPF allows only register R0 to be used as return value.
(否。BPF 只允许寄存器 R0 用作返回值。)

Q: Can more than 5 function arguments be supported in the future?(问：将来能支持多于 5 个函数参数吗？)
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
A: NO. BPF calling convention only allows registers R1-R5 to be used
as arguments. BPF is not a standalone instruction set.
(unlike x64 ISA that allows msft, cdecl and other conventions)
(否。BPF 调用约定只允许寄存器 R1-R5 用作参数。BPF 不是独立的指令集
(不像 x64 ISA 那样允许 msft、cdecl 等约定)。)

Q: Can BPF programs access instruction pointer or return address?(问：BPF 程序能访问指令指针或返回地址吗？)
-----------------------------------------------------------------------------------------------------------
A: NO.
(否。)

Q: Can BPF programs access stack pointer ?(问：BPF 程序能访问栈指针吗？)
------------------------------------------------------------------------
A: NO.
(否。)

Only frame pointer (register R10) is accessible.
From compiler point of view it's necessary to have stack pointer.
For example, LLVM defines register R11 as stack pointer in its
BPF backend, but it makes sure that generated code never uses it.
(只有帧指针 (寄存器 R10) 可以访问。从编译器的角度看，有必要有一个栈指
针。例如，LLVM 在其 BPF 后端中把寄存器 R11 定义为栈指针，但它确保生成
的代码从不使用它。)

Q: Does C-calling convention diminishes possible use cases?(问：C 调用约定是否减少了可能的用例？)
-------------------------------------------------------------------------------------------------
A: YES.
(是。)

BPF design forces addition of major functionality in the form
of kernel helper functions and kernel objects like BPF maps with
seamless interoperability between them. It lets kernel call into
BPF programs and programs call kernel helpers with zero overhead,
as all of them were native C code. That is particularly the case
for JITed BPF programs that are indistinguishable from
native kernel C code.
(BPF 设计强制以内核辅助函数和内核对象 (如 BPF map) 的形式添加主要功
能，并在它们之间实现无缝互操作。它让内核可以调用 BPF 程序、程序可以调
用内核辅助函数，且零开销，就好像它们都是原生 C 代码一样。对于经 JIT 的
BPF 程序尤其如此——它们与原生内核 C 代码难以区分。)

Q: Does it mean that 'innovative' extensions to BPF code are disallowed?(问：这是否意味着不允许对 BPF 代码做"创新性"扩展？)
---------------------------------------------------------------------------------------------------------------------------
A: Soft yes.
(算是肯定的。)

At least for now, until BPF core has support for
bpf-to-bpf calls, indirect calls, loops, global variables,
jump tables, read-only sections, and all other normal constructs
that C code can produce.
(至少在 BPF 核心支持 bpf-to-bpf 调用、间接调用、循环、全局变量、跳转
表、只读段以及 C 代码能产生的所有其他常规构造之前是这样。)

Q: Can loops be supported in a safe way?(问：能以安全的方式支持循环吗？)
------------------------------------------------------------------------
A: It's not clear yet.
(目前还不清楚。)

BPF developers are trying to find a way to
support bounded loops.
(BPF 开发者正在设法支持有界循环。)

Q: What are the verifier limits?(问：验证器的限制是什么？)
----------------------------------------------------------
A: The only limit known to the user space is BPF_MAXINSNS (4096).
It's the maximum number of instructions that the unprivileged bpf
program can have. The verifier has various internal limits.
Like the maximum number of instructions that can be explored during
program analysis. Currently, that limit is set to 1 million.
Which essentially means that the largest program can consist
of 1 million NOP instructions. There is a limit to the maximum number
of subsequent branches, a limit to the number of nested bpf-to-bpf
calls, a limit to the number of the verifier states per instruction,
a limit to the number of maps used by the program.
All these limits can be hit with a sufficiently complex program.
There are also non-numerical limits that can cause the program
to be rejected. The verifier used to recognize only pointer + constant
expressions. Now it can recognize pointer + bounded_register.
bpf_lookup_map_elem(key) had a requirement that 'key' must be
a pointer to the stack. Now, 'key' can be a pointer to map value.
The verifier is steadily getting 'smarter'. The limits are
being removed. The only way to know that the program is going to
be accepted by the verifier is to try to load it.
The bpf development process guarantees that the future kernel
versions will accept all bpf programs that were accepted by
the earlier versions.
(A：用户空间唯一已知的限制是 BPF_MAXINSNS (4096)。它是无特权的 bpf 程
序所能拥有的最大指令数。验证器有各种内部限制，例如程序分析期间可以探索
的最大指令数——目前该限制设为 100 万。这实质上意味着最大的程序可以由
100 万条 NOP 指令组成。还有对后续分支最大数量的限制、对嵌套 bpf-to-bpf
调用数量的限制、对每条指令验证器状态数量的限制、对程序所用 map 数量的
限制。足够复杂的程序都可能触达所有这些限制。还有一些非数值性的限制会导
致程序被拒绝。验证器过去只识别指针 + 常量表达式，现在它能识别指针 +
bounded_register。bpf_lookup_map_elem(key) 过去要求 'key' 必须是指向栈
的指针，现在 'key' 可以是指向 map 值的指针。验证器正在稳步变得"更聪
明"，限制正在被移除。想知道程序是否会被验证器接受，唯一的办法就是尝试
加载它。bpf 开发流程保证，未来的内核版本会接受所有被更早版本接受过的
bpf 程序。)


Instruction level questions(指令级问题)
---------------------------------------

Q: LD_ABS and LD_IND instructions vs C code(问：LD_ABS 与 LD_IND 指令与 C 代码的对比)
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

Q: How come LD_ABS and LD_IND instruction are present in BPF whereas
C code cannot express them and has to use builtin intrinsics?
(问：BPF 中为何存在 LD_ABS 和 LD_IND 指令，而 C 代码无法表达它们、必须
使用内建 (builtin intrinsics)？)

A: This is artifact of compatibility with classic BPF. Modern
networking code in BPF performs better without them.
See 'direct packet access'.
(A：这是与经典 BPF 保持兼容的产物。BPF 中的现代网络代码在没有它们的情
况下表现更好。参见 "direct packet access"。)

Q: BPF instructions mapping not one-to-one to native CPU(问：BPF 指令并非与本地 CPU 一一映射)
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
Q: It seems not all BPF instructions are one-to-one to native CPU.
For example why BPF_JNE and other compare and jumps are not cpu-like?
(问：似乎并非所有 BPF 指令都与本地 CPU 一一对应。例如，为什么 BPF_JNE
和其他比较跳转指令不像 CPU 的那样？)

A: This was necessary to avoid introducing flags into ISA which are
impossible to make generic and efficient across CPU architectures.
(A：这是必要的，以避免把标志位 (flags) 引入 ISA——要让它们在所有 CPU
架构上都通用且高效是不可能的。)

Q: Why BPF_DIV instruction doesn't map to x64 div?(问：为什么 BPF_DIV 指令不映射到 x64 div？)
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
A: Because if we picked one-to-one relationship to x64 it would have made
it more complicated to support on arm64 and other archs. Also it
needs div-by-zero runtime check.
(A：因为如果选择与 x64 一一对应，那么在 arm64 和其他架构上的支持就会
更加复杂。此外它还需要除零运行时检查。)

Q: Why BPF has implicit prologue and epilogue?(问：为什么 BPF 有隐式序言和尾声？)
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
A: Because architectures like sparc have register windows and in general
there are enough subtle differences between architectures, so naive
store return address into stack won't work. Another reason is BPF has
to be safe from division by zero (and legacy exception path
of LD_ABS insn). Those instructions need to invoke epilogue and
return implicitly.
(A：因为像 sparc 这样的架构有寄存器窗口，而且总的来说架构之间存在足够
多的细微差异，所以把返回地址直接存入栈的朴素做法行不通。另一个原因是
BPF 必须对除零安全 (以及 LD_ABS 指令的旧式异常路径)。这些指令需要隐式
地调用尾声并返回。)

Q: Why BPF_JLT and BPF_JLE instructions were not introduced in the beginning?(问：为什么 BPF_JLT 和 BPF_JLE 指令起初没有被引入？)
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
A: Because classic BPF didn't have them and BPF authors felt that compiler
workaround would be acceptable. Turned out that programs lose performance
due to lack of these compare instructions and they were added.
These two instructions is a perfect example what kind of new BPF
instructions are acceptable and can be added in the future.
These two already had equivalent instructions in native CPUs.
New instructions that don't have one-to-one mapping to HW instructions
will not be accepted.
(A：因为经典 BPF 没有它们，而 BPF 作者认为编译器变通方案是可以接受的。
结果表明，由于缺少这些比较指令，程序损失了性能，于是它们被加入了。这两
条指令是一个完美的例子，说明了什么样的新 BPF 指令是可接受、可以在将来
加入的。这两条指令在本地 CPU 中已有等价指令。与硬件指令没有一一映射的新
指令将不会被接受。)

Q: BPF 32-bit subregister requirements(问：BPF 32 位子寄存器的要求)
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
Q: BPF 32-bit subregisters have a requirement to zero upper 32-bits of BPF
registers which makes BPF inefficient virtual machine for 32-bit
CPU architectures and 32-bit HW accelerators. Can true 32-bit registers
be added to BPF in the future?
(问：BPF 的 32 位子寄存器要求把 BPF 寄存器的高 32 位清零，这使得 BPF
在 32 位 CPU 架构和 32 位硬件加速器上是低效的虚拟机。将来能否向 BPF 加
入真正的 32 位寄存器？)

A: NO.
(否。)

But some optimizations on zero-ing the upper 32 bits for BPF registers are
available, and can be leveraged to improve the performance of JITed BPF
programs for 32-bit architectures.
(不过，针对 BPF 寄存器高 32 位清零已有一些优化可用，可以用来提升 32 位
架构上经 JIT 的 BPF 程序的性能。)

Starting with version 7, LLVM is able to generate instructions that operate
on 32-bit subregisters, provided the option -mattr=+alu32 is passed for
compiling a program. Furthermore, the verifier can now mark the
instructions for which zero-ing the upper bits of the destination register
is required, and insert an explicit zero-extension (zext) instruction
(a mov32 variant). This means that for architectures without zext hardware
support, the JIT back-ends do not need to clear the upper bits for
subregisters written by alu32 instructions or narrow loads. Instead, the
back-ends simply need to support code generation for that mov32 variant,
and to overwrite bpf_jit_needs_zext() to make it return "true" (in order to
enable zext insertion in the verifier).
(从版本 7 开始，只要在编译程序时传入 -mattr=+alu32 选项，LLVM 就能生成
对 32 位子寄存器进行操作的指令。此外，验证器现在可以标记那些需要对目的
寄存器高位清零的指令，并插入显式的零扩展 (zext) 指令 (mov32 的一种变
体)。这意味着，对于没有 zext 硬件支持的架构，JIT 后端不需要为 alu32 指
令或窄加载写入的子寄存器清除高位；后端只需要支持为该 mov32 变体生成代
码，并重写 bpf_jit_needs_zext() 使其返回 "true" (以便在验证器中启用
zext 插入)。)

Note that it is possible for a JIT back-end to have partial hardware
support for zext. In that case, if verifier zext insertion is enabled,
it could lead to the insertion of unnecessary zext instructions. Such
instructions could be removed by creating a simple peephole inside the JIT
back-end: if one instruction has hardware support for zext and if the next
instruction is an explicit zext, then the latter can be skipped when doing
the code generation.
(注意，JIT 后端也可能拥有对 zext 的部分硬件支持。在这种情况下，如果启
用了验证器的 zext 插入，就可能插入不必要的 zext 指令。这类指令可以通过
在 JIT 后端内创建一个简单的窥孔优化来移除：如果某条指令有 zext 的硬件支
持，且下一条指令是显式 zext，那么在生成代码时可以跳过后者。)

Q: Does BPF have a stable ABI?(问：BPF 有稳定的 ABI 吗？)
---------------------------------------------------------
A: YES. BPF instructions, arguments to BPF programs, set of helper
functions and their arguments, recognized return codes are all part
of ABI. However there is one specific exception to tracing programs
which are using helpers like bpf_probe_read() to walk kernel internal
data structures and compile with kernel internal headers. Both of these
kernel internals are subject to change and can break with newer kernels
such that the program needs to be adapted accordingly.
(A：是。BPF 指令、BPF 程序的参数、辅助函数集合及其参数、可识别的返回
码都是 ABI 的一部分。不过，跟踪 (tracing) 程序有一个具体的例外：它们使
用 bpf_probe_read() 之类的辅助函数遍历内核内部数据结构，并用内核内部头
文件编译。这两类内核内部细节都可能变化，并可能在新内核中失效，程序需要
相应地进行调整。)

New BPF functionality is generally added through the use of kfuncs instead of
new helpers. Kfuncs are not considered part of the stable API, and have their own
lifecycle expectations as described in :ref:`BPF_kfunc_lifecycle_expectations`.
(新的 BPF 功能一般通过 kfuncs 而不是新辅助函数来添加。kfuncs 不被视为
稳定 API 的一部分，并且有各自的生命周期预期，如
:ref:`BPF_kfunc_lifecycle_expectations` 所述。)

Q: Are tracepoints part of the stable ABI?(问：跟踪点属于稳定 ABI 的一部分吗？)
-------------------------------------------------------------------------------
A: NO. Tracepoints are tied to internal implementation details hence they are
subject to change and can break with newer kernels. BPF programs need to change
accordingly when this happens.
(否。跟踪点 (tracepoints) 与内部实现细节绑定，因此可能变化，并可能在新
内核中失效。发生这种情况时，BPF 程序需要相应地修改。)

Q: Are places where kprobes can attach part of the stable ABI?(问：kprobes 可以挂接的位置属于稳定 ABI 吗？)
-----------------------------------------------------------------------------------------------------------
A: NO. The places to which kprobes can attach are internal implementation
details, which means that they are subject to change and can break with
newer kernels. BPF programs need to change accordingly when this happens.
(否。kprobes 可以挂接的位置属于内部实现细节，这意味着它们可能变化，并
可能在新内核中失效。发生这种情况时，BPF 程序需要相应地修改。)

Q: How much stack space a BPF program uses?(问：BPF 程序使用多少栈空间？)
-------------------------------------------------------------------------
A: Currently all program types are limited to 512 bytes of stack
space, but the verifier computes the actual amount of stack used
and both interpreter and most JITed code consume necessary amount.
(A：目前所有程序类型都限于 512 字节的栈空间，但验证器会计算实际使用的
栈量，解释器和大多数经 JIT 的代码都只消耗必要的量。)

Q: Can BPF be offloaded to HW?(问：BPF 能卸载到硬件吗？)
--------------------------------------------------------
A: YES. BPF HW offload is supported by NFP driver.
(是。BPF 硬件卸载由 NFP 驱动支持。)

Q: Does classic BPF interpreter still exist?(问：经典 BPF 解释器还存在吗？)
---------------------------------------------------------------------------
A: NO. Classic BPF programs are converted into extend BPF instructions.
(否。经典 BPF 程序会被转换成扩展 BPF 指令。)

Q: Can BPF call arbitrary kernel functions?(问：BPF 能调用任意内核函数吗？)
---------------------------------------------------------------------------
A: NO. BPF programs can only call specific functions exposed as BPF helpers or
kfuncs. The set of available functions is defined for every program type.
(否。BPF 程序只能调用以 BPF 辅助函数或 kfuncs 形式暴露的特定函数。可用
函数集合对每种程序类型都有定义。)

Q: Can BPF overwrite arbitrary kernel memory?(问：BPF 能覆写任意内核内存吗？)
-----------------------------------------------------------------------------
A: NO.
(否。)

Tracing bpf programs can *read* arbitrary memory with bpf_probe_read()
and bpf_probe_read_str() helpers. Networking programs cannot read
arbitrary memory, since they don't have access to these helpers.
Programs can never read or write arbitrary memory directly.
(跟踪类 bpf 程序可以用 bpf_probe_read() 和 bpf_probe_read_str() 辅助函
数 *读取* 任意内存。网络程序不能读取任意内存，因为它们无法访问这些辅助
函数。程序永远不能直接读写任意内存。)

Q: Can BPF overwrite arbitrary user memory?(问：BPF 能覆写任意用户内存吗？)
---------------------------------------------------------------------------
A: Sort-of.
(某种意义上可以。)

Tracing BPF programs can overwrite the user memory
of the current task with bpf_probe_write_user(). Every time such
program is loaded the kernel will print warning message, so
this helper is only useful for experiments and prototypes.
Tracing BPF programs are root only.
(跟踪类 BPF 程序可以用 bpf_probe_write_user() 覆写当前任务的用户内存。
每当这样的程序被加载时，内核都会打印警告消息，因此该辅助函数只对实验和
原型有用。跟踪类 BPF 程序仅限 root 使用。)

Q: New functionality via kernel modules?(问：通过内核模块提供新功能？)
----------------------------------------------------------------------
Q: Can BPF functionality such as new program or map types, new
helpers, etc be added out of kernel module code?
(问：诸如新程序或 map 类型、新辅助函数等 BPF 功能，能否通过内核模块代
码来添加？)

A: Yes, through kfuncs and kptrs
(可以，通过 kfuncs 和 kptrs。)

The core BPF functionality such as program types, maps and helpers cannot be
added to by modules. However, modules can expose functionality to BPF programs
by exporting kfuncs (which may return pointers to module-internal data
structures as kptrs).
(程序类型、map、辅助函数等核心 BPF 功能不能由模块添加。不过，模块可以
通过导出 kfuncs 向 BPF 程序暴露功能 (kfuncs 可以以 kptrs 的形式返回指向
模块内部数据结构的指针)。)

Q: Directly calling kernel function is an ABI?(问：直接调用内核函数是 ABI 吗？)
-------------------------------------------------------------------------------
Q: Some kernel functions (e.g. tcp_slow_start) can be called
by BPF programs.  Do these kernel functions become an ABI?
(问：一些内核函数 (例如 tcp_slow_start) 可以被 BPF 程序调用。这些内核
函数会成为 ABI 吗？)

A: NO.
(否。)

The kernel function protos will change and the bpf programs will be
rejected by the verifier.  Also, for example, some of the bpf-callable
kernel functions have already been used by other kernel tcp
cc (congestion-control) implementations.  If any of these kernel
functions has changed, both the in-tree and out-of-tree kernel tcp cc
implementations have to be changed.  The same goes for the bpf
programs and they have to be adjusted accordingly. See
:ref:`BPF_kfunc_lifecycle_expectations` for details.
(内核函数原型会变化，bpf 程序会被验证器拒绝。而且例如，一些可被 bpf 调
用的内核函数已经被其他内核 tcp cc (拥塞控制) 实现所使用。如果这些内核函
数中的任何一个发生变化，树内和树外的内核 tcp cc 实现都必须修改。bpf 程
序也一样，必须相应地调整。详情见
:ref:`BPF_kfunc_lifecycle_expectations`。)

Q: Attaching to arbitrary kernel functions is an ABI?(问：挂接到任意内核函数是 ABI 吗？)
----------------------------------------------------------------------------------------
Q: BPF programs can be attached to many kernel functions.  Do these
kernel functions become part of the ABI?
(问：BPF 程序可以挂接到许多内核函数。这些内核函数会成为 ABI 的一部
分吗？)

A: NO.
(否。)

The kernel function prototypes will change, and BPF programs attaching to
them will need to change.  The BPF compile-once-run-everywhere (CO-RE)
should be used in order to make it easier to adapt your BPF programs to
different versions of the kernel.
(内核函数原型会变化，挂接到它们的 BPF 程序也需要修改。应当使用 BPF 的
compile-once-run-everywhere (CO-RE)，以便更容易地把你的 BPF 程序适配到
不同版本的内核。)

Q: Marking a function with BTF_ID makes that function an ABI?(问：用 BTF_ID 标记函数就使其成为 ABI 吗？)
--------------------------------------------------------------------------------------------------------
A: NO.
(否。)

The BTF_ID macro does not cause a function to become part of the ABI
any more than does the EXPORT_SYMBOL_GPL macro.
(BTF_ID 宏并不会让一个函数成为 ABI 的一部分，正如 EXPORT_SYMBOL_GPL
宏也不会那样。)

Q: What is the compatibility story for special BPF types in map values?(问：map 值中特殊 BPF 类型的兼容性策略是什么？)
----------------------------------------------------------------------------------------------------------------------
Q: Users are allowed to embed bpf_spin_lock, bpf_timer fields in their BPF map
values (when using BTF support for BPF maps). This allows to use helpers for
such objects on these fields inside map values. Users are also allowed to embed
pointers to some kernel types (with __kptr_untrusted and __kptr BTF tags). Will the
kernel preserve backwards compatibility for these features?
(问：用户可以在自己的 BPF map 值中嵌入 bpf_spin_lock、bpf_timer 字段
(在使用 BPF map 的 BTF 支持时)。这使得可以在 map 值内的这些字段上使用
此类对象的辅助函数。用户还可以嵌入指向某些内核类型的指针 (带
__kptr_untrusted 和 __kptr BTF 标签)。内核会为这些特性保持向后兼容吗？)

A: It depends. For bpf_spin_lock, bpf_timer: YES, for kptr and everything else:
NO, but see below.
(视情况而定。对 bpf_spin_lock、bpf_timer：是；对 kptr 及其他一切：否，
但见下文。)

For struct types that have been added already, like bpf_spin_lock and bpf_timer,
the kernel will preserve backwards compatibility, as they are part of UAPI.
(对于已经加入的结构类型，如 bpf_spin_lock 和 bpf_timer，内核会保持向后
兼容，因为它们是 UAPI 的一部分。)

For kptrs, they are also part of UAPI, but only with respect to the kptr
mechanism. The types that you can use with a __kptr_untrusted and __kptr tagged
pointer in your struct are NOT part of the UAPI contract. The supported types can
and will change across kernel releases. However, operations like accessing kptr
fields and bpf_kptr_xchg() helper will continue to be supported across kernel
releases for the supported types.
(对于 kptrs，它们也是 UAPI 的一部分，但仅就 kptr 机制而言。你可以在结
构体中配合 __kptr_untrusted 和 __kptr 标签指针使用的那些类型不是 UAPI
契约的一部分。支持的类型可能且将会跨内核版本变化。不过，访问 kptr 字段
以及 bpf_kptr_xchg() 辅助函数等操作，在各个内核版本中都会继续支持所支持
的类型。)

For any other supported struct type, unless explicitly stated in this document
and added to bpf.h UAPI header, such types can and will arbitrarily change their
size, type, and alignment, or any other user visible API or ABI detail across
kernel releases. The users must adapt their BPF programs to the new changes and
update them to make sure their programs continue to work correctly.
(对于任何其他受支持的结构类型，除非在本文件中明确说明并加入 bpf.h UAPI
头文件，否则这些类型可能且将会在内核版本之间任意改变其大小、类型和对
齐，或任何其他用户可见的 API 或 ABI 细节。用户必须让他们的 BPF 程序适
应新变化并加以更新，以确保其程序继续正常工作。)

NOTE: BPF subsystem specially reserves the 'bpf\_' prefix for type names, in
order to introduce more special fields in the future. Hence, user programs must
avoid defining types with 'bpf\_' prefix to not be broken in future releases.
In other words, no backwards compatibility is guaranteed if one using a type
in BTF with 'bpf\_' prefix.
(注意：BPF 子系统专门为类型名保留了 'bpf\_' 前缀，以便将来引入更多特
殊字段。因此，用户程序必须避免定义带 'bpf\_' 前缀的类型，以免在未来版
本中被破坏。换句话说，如果使用 BTF 中带 'bpf\_' 前缀的类型，不提供任何
向后兼容保证。)

Q: What is the compatibility story for special BPF types in allocated objects?(问：已分配对象中特殊 BPF 类型的兼容性策略是什么？)
---------------------------------------------------------------------------------------------------------------------------------
Q: Same as above, but for allocated objects (i.e. objects allocated using
bpf_obj_new for user defined types). Will the kernel preserve backwards
compatibility for these features?
(问：与上一问相同，但针对已分配对象 (即用 bpf_obj_new 为用户自定义类型
分配的对象)。内核会为这些特性保持向后兼容吗？)

A: NO.
(否。)

Unlike map value types, the API to work with allocated objects and any support
for special fields inside them is exposed through kfuncs, and thus has the same
lifecycle expectations as the kfuncs themselves. See
:ref:`BPF_kfunc_lifecycle_expectations` for details.
(与 map 值类型不同，处理已分配对象的 API 及其内部特殊字段的任何支持都
通过 kfuncs 暴露，因此与 kfuncs 本身有相同的生命周期预期。详情见
:ref:`BPF_kfunc_lifecycle_expectations`。)
