
=====================================
Classic BPF vs eBPF(经典 BPF 与 eBPF)
=====================================

eBPF is designed to be JITed with one to one mapping, which can also open up
the possibility for GCC/LLVM compilers to generate optimized eBPF code through
an eBPF backend that performs almost as fast as natively compiled code.
(eBPF 的设计支持一对一映射的 JIT 编译，这也为 GCC/LLVM 编译器通过 eBPF
后端生成优化的 eBPF 代码打开了可能，其执行速度几乎与本地编译的代码一样
快。)

Some core changes of the eBPF format from classic BPF:
(eBPF 格式相对于经典 BPF 的一些核心变化：)

- Number of registers increase from 2 to 10:
  (- 寄存器数量从 2 个增加到 10 个：)

  The old format had two registers A and X, and a hidden frame pointer. The
  new layout extends this to be 10 internal registers and a read-only frame
  pointer. Since 64-bit CPUs are passing arguments to functions via registers
  the number of args from eBPF program to in-kernel function is restricted
  to 5 and one register is used to accept return value from an in-kernel
  function. Natively, x86_64 passes first 6 arguments in registers, aarch64/
  sparcv9/mips64 have 7 - 8 registers for arguments; x86_64 has 6 callee saved
  registers, and aarch64/sparcv9/mips64 have 11 or more callee saved registers.
  (旧格式有两个寄存器 A 和 X，以及一个隐藏的帧指针。新布局将其扩展为
  10 个内部寄存器和一个只读帧指针。由于 64 位 CPU 通过寄存器向函数传递
  参数，eBPF 程序传给内核内函数的参数个数被限制为 5 个，另有一个寄存器
  用于接收内核内函数的返回值。本地代码中，x86_64 在寄存器中传递前 6 个
  参数，aarch64/sparcv9/mips64 有 7 - 8 个参数寄存器；x86_64 有 6 个被
  调用者保存寄存器，aarch64/sparcv9/mips64 有 11 个或更多被调用者保存
  寄存器。)

  Thus, all eBPF registers map one to one to HW registers on x86_64, aarch64,
  etc, and eBPF calling convention maps directly to ABIs used by the kernel on
  64-bit architectures.
  (因此，所有 eBPF 寄存器与 x86_64、aarch64 等架构上的硬件寄存器一一
  对应，eBPF 调用约定直接映射到内核在 64 位架构上使用的 ABI。)

  On 32-bit architectures JIT may map programs that use only 32-bit arithmetic
  and may let more complex programs to be interpreted.
  (在 32 位架构上，JIT 可以映射只使用 32 位运算的程序，并让更复杂的
  程序走解释执行。)

  R0 - R5 are scratch registers and eBPF program needs spill/fill them if
  necessary across calls. Note that there is only one eBPF program (== one
  eBPF main routine) and it cannot call other eBPF functions, it can only
  call predefined in-kernel functions, though.
  (R0 - R5 是临时寄存器，如有必要，eBPF 程序需要在跨调用时将其溢出
  (spill)/回填 (fill)。注意只有一个 eBPF 程序 (== 一个 eBPF 主例程)，
  它不能调用其他 eBPF 函数，但可以调用预定义的内核内函数。)

- Register width increases from 32-bit to 64-bit:
  (- 寄存器宽度从 32 位增加到 64 位：)

  Still, the semantics of the original 32-bit ALU operations are preserved
  via 32-bit subregisters. All eBPF registers are 64-bit with 32-bit lower
  subregisters that zero-extend into 64-bit if they are being written to.
  That behavior maps directly to x86_64 and arm64 subregister definition, but
  makes other JITs more difficult.
  (尽管如此，原始 32 位 ALU 操作的语义仍通过 32 位子寄存器保留。所有
  eBPF 寄存器都是 64 位的，具有 32 位低子寄存器，写入它们时会零扩展到
  64 位。该行为直接映射到 x86_64 和 arm64 的子寄存器定义，但使其他 JIT
  更难实现。)

  32-bit architectures run 64-bit eBPF programs via interpreter.
  Their JITs may convert BPF programs that only use 32-bit subregisters into
  native instruction set and let the rest being interpreted.
  (32 位架构通过解释器运行 64 位 eBPF 程序。它们的 JIT 可以把只使用
  32 位子寄存器的 BPF 程序转换为原生指令集，并让其余部分解释执行。)

  Operation is 64-bit, because on 64-bit architectures, pointers are also
  64-bit wide, and we want to pass 64-bit values in/out of kernel functions,
  so 32-bit eBPF registers would otherwise require to define register-pair
  ABI, thus, there won't be able to use a direct eBPF register to HW register
  mapping and JIT would need to do combine/split/move operations for every
  register in and out of the function, which is complex, bug prone and slow.
  Another reason is the use of atomic 64-bit counters.
  (操作是 64 位的，因为在 64 位架构上指针也是 64 位宽，我们希望在内核
  函数中传入/传出 64 位值，否则 32 位 eBPF 寄存器将需要定义寄存器对
  (register-pair) ABI，因而无法使用 eBPF 寄存器到硬件寄存器的直接映射，
  JIT 需要对进出函数的每个寄存器做合并/拆分/移动操作，这复杂、易错且慢。
  另一个原因是需要使用原子的 64 位计数器。)

- Conditional jt/jf targets replaced with jt/fall-through:
  (- 条件跳转的 jt/jf 目标被替换为 jt/fall-through：)

  While the original design has constructs such as ``if (cond) jump_true;
  else jump_false;``, they are being replaced into alternative constructs like
  ``if (cond) jump_true; /* else fall-through */``.
  (虽然原始设计有 ``if (cond) jump_true;
  else jump_false;`` 这样的构造，但它们正被替换为 ``if (cond)
  jump_true; /* else fall-through */`` 这样的替代构造。)

- Introduces bpf_call insn and register passing convention for zero overhead
  calls from/to other kernel functions:
  (- 引入 bpf_call 指令与寄存器传参约定，使与内核内其他函数之间的调用
  零开销：)

  Before an in-kernel function call, the eBPF program needs to
  place function arguments into R1 to R5 registers to satisfy calling
  convention, then the interpreter will take them from registers and pass
  to in-kernel function. If R1 - R5 registers are mapped to CPU registers
  that are used for argument passing on given architecture, the JIT compiler
  doesn't need to emit extra moves. Function arguments will be in the correct
  registers and BPF_CALL instruction will be JITed as single 'call' HW
  instruction. This calling convention was picked to cover common call
  situations without performance penalty.
  (在内核内函数调用之前，eBPF 程序需要把函数参数放入 R1 到 R5 寄存器
  以满足调用约定，然后解释器会从寄存器中取出它们并传给内核内函数。如果
  R1 - R5 寄存器被映射到给定架构上用于参数传递的 CPU 寄存器，JIT 编译器
  就不需要生成额外的移动指令。函数参数将位于正确的寄存器中，BPF_CALL
  指令会被 JIT 编译为单条 'call' 硬件指令。选择这一调用约定是为了在不
  损失性能的情况下覆盖常见的调用情形。)

  After an in-kernel function call, R1 - R5 are reset to unreadable and R0 has
  a return value of the function. Since R6 - R9 are callee saved, their state
  is preserved across the call.
  (在内核内函数调用之后，R1 - R5 被重置为不可读，R0 保存函数的返回值。
  由于 R6 - R9 是被调用者保存的，它们的状态在调用过程中得以保留。)

  For example, consider three C functions::

    u64 f1() { return (*_f2)(1); }
    u64 f2(u64 a) { return f3(a + 1, a); }
    u64 f3(u64 a, u64 b) { return a - b; }

  (例如，考虑三个 C 函数：)

  GCC can compile f1, f3 into x86_64::

    f1:
	movl $1, %edi
	movq _f2(%rip), %rax
	jmp  *%rax
    f3:
	movq %rdi, %rax
	subq %rsi, %rax
	ret

  (GCC 可以把 f1、f3 编译为 x86_64：)

  Function f2 in eBPF may look like::

    f2:
	bpf_mov R2, R1
	bpf_add R1, 1
	bpf_call f3
	bpf_exit

  (eBPF 中的函数 f2 可能形如：)

  If f2 is JITed and the pointer stored to ``_f2``. The calls f1 -> f2 -> f3 and
  returns will be seamless. Without JIT, __bpf_prog_run() interpreter needs to
  be used to call into f2.
  (如果 f2 被 JIT 编译且指针已存储到 ``_f2``。f1 -> f2 -> f3 的调用与
  返回将无缝进行。没有 JIT 时，需要使用 __bpf_prog_run() 解释器来调用
  f2。)

  For practical reasons all eBPF programs have only one argument 'ctx' which is
  already placed into R1 (e.g. on __bpf_prog_run() startup) and the programs
  can call kernel functions with up to 5 arguments. Calls with 6 or more arguments
  are currently not supported, but these restrictions can be lifted if necessary
  in the future.
  (出于实际考虑，所有 eBPF 程序只有一个参数 'ctx'，它已被放入 R1 (例如
  在 __bpf_prog_run() 启动时)，程序可以调用最多 5 个参数的内核函数。
  目前不支持 6 个或更多参数的调用，但将来如有必要可以解除这些限制。)

  On 64-bit architectures all register map to HW registers one to one. For
  example, x86_64 JIT compiler can map them as ...
  (在 64 位架构上，所有寄存器与硬件寄存器一一对应。例如，x86_64 JIT
  编译器可以把它们映射为 ...)

  ::

    R0 - rax
    R1 - rdi
    R2 - rsi
    R3 - rdx
    R4 - rcx
    R5 - r8
    R6 - rbx
    R7 - r13
    R8 - r14
    R9 - r15
    R10 - rbp

  (具体如下：)

  ... since x86_64 ABI mandates rdi, rsi, rdx, rcx, r8, r9 for argument passing
  and rbx, r12 - r15 are callee saved.
  (... 因为 x86_64 ABI 规定 rdi、rsi、rdx、rcx、r8、r9 用于参数传递，
  而 rbx、r12 - r15 是被调用者保存的。)

  Then the following eBPF pseudo-program::

    bpf_mov R6, R1 /* save ctx */
    bpf_mov R2, 2
    bpf_mov R3, 3
    bpf_mov R4, 4
    bpf_mov R5, 5
    bpf_call foo
    bpf_mov R7, R0 /* save foo() return value */
    bpf_mov R1, R6 /* restore ctx for next call */
    bpf_mov R2, 6
    bpf_mov R3, 7
    bpf_mov R4, 8
    bpf_mov R5, 9
    bpf_call bar
    bpf_add R0, R7
    bpf_exit

  (下面是相应的 eBPF 伪程序：)

  After JIT to x86_64 may look like::

    push %rbp
    mov %rsp,%rbp
    sub $0x228,%rsp
    mov %rbx,-0x228(%rbp)
    mov %r13,-0x220(%rbp)
    mov %rdi,%rbx
    mov $0x2,%esi
    mov $0x3,%edx
    mov $0x4,%ecx
    mov $0x5,%r8d
    callq foo
    mov %rax,%r13
    mov %rbx,%rdi
    mov $0x6,%esi
    mov $0x7,%edx
    mov $0x8,%ecx
    mov $0x9,%r8d
    callq bar
    add %r13,%rax
    mov -0x228(%rbp),%rbx
    mov -0x220(%rbp),%r13
    leaveq
    retq

  (JIT 编译为 x86_64 之后可能形如：)

  Which is in this example equivalent in C to::

    u64 bpf_filter(u64 ctx)
    {
	return foo(ctx, 2, 3, 4, 5) + bar(ctx, 6, 7, 8, 9);
    }

  (在本例中，其等价的 C 代码为：)

  In-kernel functions foo() and bar() with prototype: u64 (*)(u64 arg1, u64
  arg2, u64 arg3, u64 arg4, u64 arg5); will receive arguments in proper
  registers and place their return value into ``%rax`` which is R0 in eBPF.
  Prologue and epilogue are emitted by JIT and are implicit in the
  interpreter. R0-R5 are scratch registers, so eBPF program needs to preserve
  them across the calls as defined by calling convention.
  (内核内函数 foo() 和 bar() 的原型为 u64 (*)(u64 arg1, u64 arg2, u64
  arg3, u64 arg4, u64 arg5); 它们会在正确的寄存器中接收参数，并把返回值
  放入 ``%rax`` (即 eBPF 中的 R0)。序言 (prologue) 和尾声 (epilogue) 由
  JIT 生成，在解释器中则是隐式的。R0-R5 是临时寄存器，因此按照调用约定
  的定义，eBPF 程序需要跨调用保存它们。)

  For example the following program is invalid::

    bpf_mov R1, 1
    bpf_call foo
    bpf_mov R0, R1
    bpf_exit

  (例如，下面的程序是无效的：)

  After the call the registers R1-R5 contain junk values and cannot be read.
  An in-kernel verifier.rst is used to validate eBPF programs.
  (调用之后，寄存器 R1-R5 包含垃圾值，不可读取。内核内的 verifier.rst
  (校验器) 用于验证 eBPF 程序。)

Also in the new design, eBPF is limited to 4096 insns, which means that any
program will terminate quickly and will only call a fixed number of kernel
functions. Original BPF and eBPF are two operand instructions,
which helps to do one-to-one mapping between eBPF insn and x86 insn during JIT.
(在新设计中，eBPF 还被限制为 4096 条指令，这意味着任何程序都会很快
终止，并且只会调用固定数量的内核函数。原始 BPF 与 eBPF 都是双操作数
指令，这有助于在 JIT 期间做 eBPF 指令与 x86 指令之间的一对一映射。)

The input context pointer for invoking the interpreter function is generic,
its content is defined by a specific use case. For seccomp register R1 points
to seccomp_data, for converted BPF filters R1 points to a skb.
(调用解释器函数的输入上下文指针是通用的，其内容由具体用例定义。对
seccomp 而言寄存器 R1 指向 seccomp_data；对转换后的 BPF 过滤器而言
R1 指向一个 skb。)

A program, that is translated internally consists of the following elements::

  op:16, jt:8, jf:8, k:32    ==>    op:8, dst_reg:4, src_reg:4, off:16, imm:32

(在内部转换后，程序由以下元素构成：)

So far 87 eBPF instructions were implemented. 8-bit 'op' opcode field
has room for new instructions. Some of them may use 16/24/32 byte encoding. New
instructions must be multiple of 8 bytes to preserve backward compatibility.
(迄今为止已实现 87 条 eBPF 指令。8 位的 'op' 操作码字段还有容纳新指令
的空间。其中一些可能使用 16/24/32 字节编码。新指令必须是 8 字节的倍数，
以保持向后兼容。)

eBPF is a general purpose RISC instruction set. Not every register and
every instruction are used during translation from original BPF to eBPF.
For example, socket filters are not using ``exclusive add`` instruction, but
tracing filters may do to maintain counters of events, for example. Register R9
is not used by socket filters either, but more complex filters may be running
out of registers and would have to resort to spill/fill to stack.
(eBPF 是一个通用 RISC 指令集。从原始 BPF 转换到 eBPF 时，并非每个
寄存器和每条指令都会用到。例如，socket 过滤器不使用 ``exclusive add``
指令，而跟踪过滤器可能使用它来维护事件计数器。寄存器 R9 也不被 socket
过滤器使用，但更复杂的过滤器可能会用尽寄存器，不得不求助于向栈溢出/
回填。)

eBPF can be used as a generic assembler for last step performance
optimizations, socket filters and seccomp are using it as assembler. Tracing
filters may use it as assembler to generate code from kernel. In kernel usage
may not be bounded by security considerations, since generated eBPF code
may be optimizing internal code path and not being exposed to the user space.
Safety of eBPF can come from the verifier.rst. In such use cases as
described, it may be used as safe instruction set.
(eBPF 可以用作通用汇编器来做最后一步的性能优化，socket 过滤器和
seccomp 就把它用作汇编器。跟踪过滤器也可以把它用作汇编器来从内核生成
代码。内核内的使用可能不受安全考虑的限制，因为生成的 eBPF 代码可能是在
优化内部代码路径，而不暴露给用户空间。eBPF 的安全性可以来自
verifier.rst (校验器)。在上述用例中，它可以被用作安全的指令集。)

Just like the original BPF, eBPF runs within a controlled environment,
is deterministic and the kernel can easily prove that. The safety of the program
can be determined in two steps: first step does depth-first-search to disallow
loops and other CFG validation; second step starts from the first insn and
descends all possible paths. It simulates execution of every insn and observes
the state change of registers and stack.
(与原始 BPF 一样，eBPF 在受控环境中运行，是确定性的，内核可以轻易证明
这一点。程序的安全性分两步确定：第一步做深度优先搜索以禁止循环并进行
其他 CFG 验证；第二步从第一条指令开始，下降所有可能的路径。它模拟每条
指令的执行，并观察寄存器和栈的状态变化。)

opcode encoding(操作码编码)
===========================

eBPF is reusing most of the opcode encoding from classic to simplify conversion
of classic BPF to eBPF.
(eBPF 复用了经典 BPF 的大部分操作码编码，以简化从经典 BPF 到 eBPF 的
转换。)

For arithmetic and jump instructions the 8-bit 'code' field is divided into three
parts::

  +----------------+--------+--------------------+
  |   4 bits       |  1 bit |   3 bits           |
  | operation code | source | instruction class  |
  +----------------+--------+--------------------+
  (MSB)                                      (LSB)

(对于算术和跳转指令，8 位的 'code' 字段分为三个部分：)

Three LSB bits store instruction class which is one of:
(最低 3 个 LSB 位存储指令类别，它是以下之一：)

  ===================     ===============
  Classic BPF classes     eBPF classes
  ===================     ===============
  BPF_LD    0x00          BPF_LD    0x00
  BPF_LDX   0x01          BPF_LDX   0x01
  BPF_ST    0x02          BPF_ST    0x02
  BPF_STX   0x03          BPF_STX   0x03
  BPF_ALU   0x04          BPF_ALU   0x04
  BPF_JMP   0x05          BPF_JMP   0x05
  BPF_RET   0x06          BPF_JMP32 0x06
  BPF_MISC  0x07          BPF_ALU64 0x07
  ===================     ===============

The 4th bit encodes the source operand ...

    ::

	BPF_K     0x00
	BPF_X     0x08

(第 4 位编码源操作数 ...，取值为：)

 * in classic BPF, this means::

	BPF_SRC(code) == BPF_X - use register X as source operand
	BPF_SRC(code) == BPF_K - use 32-bit immediate as source operand

   (在经典 BPF 中，这意味着：)

 * in eBPF, this means::

	BPF_SRC(code) == BPF_X - use 'src_reg' register as source operand
	BPF_SRC(code) == BPF_K - use 32-bit immediate as source operand

   (在 eBPF 中，这意味着：)

... and four MSB bits store operation code.
(... 而 4 个 MSB 位存储操作码。)

If BPF_CLASS(code) == BPF_ALU or BPF_ALU64 [ in eBPF ], BPF_OP(code) is one of::

  BPF_ADD   0x00
  BPF_SUB   0x10
  BPF_MUL   0x20
  BPF_DIV   0x30
  BPF_OR    0x40
  BPF_AND   0x50
  BPF_LSH   0x60
  BPF_RSH   0x70
  BPF_NEG   0x80
  BPF_MOD   0x90
  BPF_XOR   0xa0
  BPF_MOV   0xb0  /* eBPF only: mov reg to reg */
  BPF_ARSH  0xc0  /* eBPF only: sign extending shift right */
  BPF_END   0xd0  /* eBPF only: endianness conversion */

(如果 BPF_CLASS(code) == BPF_ALU 或 BPF_ALU64 [在 eBPF 中]，则
BPF_OP(code) 是以下之一：)

If BPF_CLASS(code) == BPF_JMP or BPF_JMP32 [ in eBPF ], BPF_OP(code) is one of::

  BPF_JA    0x00  /* BPF_JMP only */
  BPF_JEQ   0x10
  BPF_JGT   0x20
  BPF_JGE   0x30
  BPF_JSET  0x40
  BPF_JNE   0x50  /* eBPF only: jump != */
  BPF_JSGT  0x60  /* eBPF only: signed '>' */
  BPF_JSGE  0x70  /* eBPF only: signed '>=' */
  BPF_CALL  0x80  /* eBPF BPF_JMP only: function call */
  BPF_EXIT  0x90  /* eBPF BPF_JMP only: function return */
  BPF_JLT   0xa0  /* eBPF only: unsigned '<' */
  BPF_JLE   0xb0  /* eBPF only: unsigned '<=' */
  BPF_JSLT  0xc0  /* eBPF only: signed '<' */
  BPF_JSLE  0xd0  /* eBPF only: signed '<=' */

(如果 BPF_CLASS(code) == BPF_JMP 或 BPF_JMP32 [在 eBPF 中]，则
BPF_OP(code) 是以下之一：)

So BPF_ADD | BPF_X | BPF_ALU means 32-bit addition in both classic BPF
and eBPF. There are only two registers in classic BPF, so it means A += X.
In eBPF it means dst_reg = (u32) dst_reg + (u32) src_reg; similarly,
BPF_XOR | BPF_K | BPF_ALU means A ^= imm32 in classic BPF and analogous
src_reg = (u32) src_reg ^ (u32) imm32 in eBPF.
(所以 BPF_ADD | BPF_X | BPF_ALU 在经典 BPF 和 eBPF 中都表示 32 位加法。
经典 BPF 只有两个寄存器，所以它表示 A += X。在 eBPF 中它表示
dst_reg = (u32) dst_reg + (u32) src_reg；类似地，BPF_XOR | BPF_K |
BPF_ALU 在经典 BPF 中表示 A ^= imm32，在 eBPF 中对应
src_reg = (u32) src_reg ^ (u32) imm32。)

Classic BPF is using BPF_MISC class to represent A = X and X = A moves.
eBPF is using BPF_MOV | BPF_X | BPF_ALU code instead. Since there are no
BPF_MISC operations in eBPF, the class 7 is used as BPF_ALU64 to mean
exactly the same operations as BPF_ALU, but with 64-bit wide operands
instead. So BPF_ADD | BPF_X | BPF_ALU64 means 64-bit addition, i.e.:
dst_reg = dst_reg + src_reg
(经典 BPF 使用 BPF_MISC 类来表示 A = X 和 X = A 的移动。eBPF 改用
BPF_MOV | BPF_X | BPF_ALU 代码。由于 eBPF 中没有 BPF_MISC 操作，类别
7 被用作 BPF_ALU64，表示与 BPF_ALU 完全相同的操作，只是操作数为 64 位
宽。所以 BPF_ADD | BPF_X | BPF_ALU64 表示 64 位加法，即：
dst_reg = dst_reg + src_reg)

Classic BPF wastes the whole BPF_RET class to represent a single ``ret``
operation. Classic BPF_RET | BPF_K means copy imm32 into return register
and perform function exit. eBPF is modeled to match CPU, so BPF_JMP | BPF_EXIT
in eBPF means function exit only. The eBPF program needs to store return
value into register R0 before doing a BPF_EXIT. Class 6 in eBPF is used as
BPF_JMP32 to mean exactly the same operations as BPF_JMP, but with 32-bit wide
operands for the comparisons instead.
(经典 BPF 把整个 BPF_RET 类浪费在表示单个 ``ret`` 操作上。
Classic BPF_RET | BPF_K 表示把 imm32 复制到返回寄存器并执行函数退出。
eBPF 建模为与 CPU 匹配，所以 eBPF 中的 BPF_JMP | BPF_EXIT 只表示函数
退出。eBPF 程序在执行 BPF_EXIT 之前需要把返回值存入寄存器 R0。eBPF 中
的类别 6 被用作 BPF_JMP32，表示与 BPF_JMP 完全相同的操作，只是比较的
操作数为 32 位宽。)

For load and store instructions the 8-bit 'code' field is divided as::

  +--------+--------+-------------------+
  | 3 bits | 2 bits |   3 bits          |
  |  mode  |  size  | instruction class |
  +--------+--------+-------------------+
  (MSB)                             (LSB)

(对于加载和存储指令，8 位的 'code' 字段划分如下：)

Size modifier is one of ...

::

  BPF_W   0x00    /* word */
  BPF_H   0x08    /* half word */
  BPF_B   0x10    /* byte */
  BPF_DW  0x18    /* eBPF only, double word */

(大小修饰符是以下之一 ...)

... which encodes size of load/store operation::

 B  - 1 byte
 H  - 2 byte
 W  - 4 byte
 DW - 8 byte (eBPF only)

(... 它编码加载/存储操作的大小：)

Mode modifier is one of::

  BPF_IMM     0x00  /* used for 32-bit mov in classic BPF and 64-bit in eBPF */
  BPF_ABS     0x20
  BPF_IND     0x40
  BPF_MEM     0x60
  BPF_LEN     0x80  /* classic BPF only, reserved in eBPF */
  BPF_MSH     0xa0  /* classic BPF only, reserved in eBPF */
  BPF_ATOMIC  0xc0  /* eBPF only, atomic operations */

(模式修饰符是以下之一：)
