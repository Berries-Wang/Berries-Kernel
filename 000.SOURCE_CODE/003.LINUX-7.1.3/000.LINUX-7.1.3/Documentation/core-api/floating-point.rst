.. SPDX-License-Identifier: GPL-2.0+

============================
Floating-point API(浮点 API)
============================

Kernel code is normally prohibited from using floating-point (FP) registers or
instructions, including the C float and double data types. This rule reduces
system call overhead, because the kernel does not need to save and restore the
userspace floating-point register state.
(内核代码通常被禁止使用浮点 (FP) 寄存器或指令，包括 C 语言的 float 与
double 数据类型。这条规则减少了系统调用的开销，因为内核不需要保存和
恢复用户空间的浮点寄存器状态。)

However, occasionally drivers or library functions may need to include FP code.
This is supported by isolating the functions containing FP code to a separate
translation unit (a separate source file), and saving/restoring the FP register
state around calls to those functions. This creates "critical sections" of
floating-point usage.
(不过，偶尔驱动程序或库函数可能需要包含 FP 代码。支持的做法是：把包含
FP 代码的函数隔离到一个单独的编译单元 (单独的源文件) 中，并在调用这些
函数的前后保存/恢复 FP 寄存器状态。这样就创建了浮点使用的"临界区"。)

The reason for this isolation is to prevent the compiler from generating code
touching the FP registers outside these critical sections. Compilers sometimes
use FP registers to optimize inlined ``memcpy`` or variable assignment, as
floating-point registers may be wider than general-purpose registers.
(之所以这样隔离，是为了防止编译器在这些临界区之外生成触碰 FP 寄存器的
代码。编译器有时会使用 FP 寄存器来优化内联的 ``memcpy`` 或变量赋值，
因为浮点寄存器可能比通用寄存器更宽。)

Usability of floating-point code within the kernel is architecture-specific.
Additionally, because a single kernel may be configured to support platforms
both with and without a floating-point unit, FPU availability must be checked
both at build time and at run time.
(浮点代码在内核中的可用性取决于具体架构。此外，由于同一个内核可能被
配置为同时支持带浮点单元和不带浮点单元的平台，FPU 的可用性必须在构
建时和运行时都进行检查。)

Several architectures implement the generic kernel floating-point API from
``linux/fpu.h``, as described below. Some other architectures implement their
own unique APIs, which are documented separately.
(若干架构实现了来自 ``linux/fpu.h`` 的通用内核浮点 API，如下所述。其
他一些架构实现了自己独有的 API，另行单独文档化。)

Build-time API(构建期 API)
--------------------------

Floating-point code may be built if the option ``ARCH_HAS_KERNEL_FPU_SUPPORT``
is enabled. For C code, such code must be placed in a separate file, and that
file must have its compilation flags adjusted using the following pattern::

    CFLAGS_foo.o += $(CC_FLAGS_FPU)
    CFLAGS_REMOVE_foo.o += $(CC_FLAGS_NO_FPU)

(如果启用了 ``ARCH_HAS_KERNEL_FPU_SUPPORT`` 选项，则可以构建浮点代码。
对于 C 代码，这类代码必须放在单独的文件中，并且该文件必须使用以下模
式调整其编译标志：)

Architectures are expected to define one or both of these variables in their
top-level Makefile as needed. For example::

    CC_FLAGS_FPU := -mhard-float

(架构应按需在其顶层 Makefile 中定义这两个变量中的一个或两个。例如：)

or::

    CC_FLAGS_NO_FPU := -msoft-float

(或：)

Normal kernel code is assumed to use the equivalent of ``CC_FLAGS_NO_FPU``.
(假定普通内核代码使用与 ``CC_FLAGS_NO_FPU`` 等价的设置。)

Runtime API(运行时 API)
-----------------------

The runtime API is provided in ``linux/fpu.h``. This header cannot be included
from files implementing FP code (those with their compilation flags adjusted as
above). Instead, it must be included when defining the FP critical sections.
(运行时 API 由 ``linux/fpu.h`` 提供。该头文件不能被实现 FP 代码的文件
(即编译标志如上调整的文件) 包含。相反，它必须在定义 FP 临界区时被包
含。)

.. c:function:: bool kernel_fpu_available( void )

        This function reports if floating-point code can be used on this CPU or
        platform. The value returned by this function is not expected to change
        at runtime, so it only needs to be called once, not before every
        critical section.
        (该函数报告在此 CPU 或平台上是否可以使用浮点代码。该函数的返回
        值预期不会在运行时改变，因此只需调用一次，而不必在每个临界区之
        前调用。)

.. c:function:: void kernel_fpu_begin( void )
                void kernel_fpu_end( void )

        These functions create a floating-point critical section. It is only
        valid to call ``kernel_fpu_begin()`` after a previous call to
        ``kernel_fpu_available()`` returned ``true``. These functions are only
        guaranteed to be callable from (preemptible or non-preemptible) process
        context.
        (这些函数创建一个浮点临界区。只有在前一次调用
        ``kernel_fpu_available()`` 返回 ``true`` 之后，才可以调用
        ``kernel_fpu_begin()``。这些函数只保证可以从 (可抢占或不可抢占
        的) 进程上下文中调用。)

        Preemption may be disabled inside critical sections, so their size
        should be minimized. They are *not* required to be reentrant. If the
        caller expects to nest critical sections, it must implement its own
        reference counting.
        (临界区内可能会禁用抢占，因此临界区的尺寸应尽量小。它们 *不*
        保证可重入。如果调用者期望嵌套临界区，它必须自行实现引用计
        数。)
