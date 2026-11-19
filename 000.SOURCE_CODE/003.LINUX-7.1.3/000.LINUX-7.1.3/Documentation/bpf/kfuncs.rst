.. SPDX-License-Identifier: GPL-2.0

.. _kfuncs-header-label:

===========================================
BPF Kernel Functions (kfuncs)(BPF 内核函数)
===========================================

1. Introduction(1. 简介)
========================

BPF Kernel Functions or more commonly known as kfuncs are functions in the Linux
kernel which are exposed for use by BPF programs. Unlike normal BPF helpers,
kfuncs do not have a stable interface and can change from one kernel release to
another. Hence, BPF programs need to be updated in response to changes in the
kernel. See :ref:`BPF_kfunc_lifecycle_expectations` for more information.
(BPF 内核函数（通常称为 kfunc）是 Linux 内核中暴露给 BPF 程序使用的函数。与普通的
BPF helper 不同，kfunc 没有稳定的接口，可能随内核版本而变化。因此，BPF 程序需要
随内核的变化而更新。更多信息参见 :ref:`BPF_kfunc_lifecycle_expectations`。)

2. Defining a kfunc(2. 定义 kfunc)
==================================

There are two ways to expose a kernel function to BPF programs, either make an
existing function in the kernel visible, or add a new wrapper for BPF. In both
cases, care must be taken that BPF program can only call such function in a
valid context. To enforce this, visibility of a kfunc can be per program type.
(把内核函数暴露给 BPF 程序有两种方式：要么让内核中的现有函数变为可见，要么为 BPF
添加一个新的包装函数。无论哪种情况，都必须注意 BPF 程序只能在有效上下文中调用
这类函数。为此，kfunc 的可见性可以按程序类型区分。)

If you are not creating a BPF wrapper for existing kernel function, skip ahead
to :ref:`BPF_kfunc_nodef`.
(如果你不是在为现有的内核函数创建 BPF 包装器，可直接跳到 :ref:`BPF_kfunc_nodef`。)

2.1 Creating a wrapper kfunc(2.1 创建包装 kfunc)
------------------------------------------------

When defining a wrapper kfunc, the wrapper function should have extern linkage.
This prevents the compiler from optimizing away dead code, as this wrapper kfunc
is not invoked anywhere in the kernel itself. It is not necessary to provide a
prototype in a header for the wrapper kfunc.
(定义包装 kfunc 时，包装函数应当具有 extern 链接属性。这可以防止编译器把死代码
优化掉，因为这个包装 kfunc 在内核自身中不会被任何地方调用。不必在头文件中为包装
kfunc 提供原型。)

An example is given below::

        /* Disables missing prototype warnings */
        __bpf_kfunc_start_defs();

        __bpf_kfunc struct task_struct *bpf_find_get_task_by_vpid(pid_t nr)
        {
                return find_get_task_by_vpid(nr);
        }

        __bpf_kfunc_end_defs();

(下面给出一个示例：)

A wrapper kfunc is often needed when we need to annotate parameters of the
kfunc. Otherwise one may directly make the kfunc visible to the BPF program by
registering it with the BPF subsystem. See :ref:`BPF_kfunc_nodef`.
(当我们需要为 kfunc 的参数添加注解时，通常就需要包装 kfunc。否则可以直接把 kfunc
注册到 BPF 子系统，使其对 BPF 程序可见。参见 :ref:`BPF_kfunc_nodef`。)

2.2 kfunc Parameters(2.2 kfunc 参数)
------------------------------------

All kfuncs now require trusted arguments by default. This means that all
pointer arguments must be valid, and all pointers to BTF objects must be
passed in their unmodified form (at a zero offset, and without having been
obtained from walking another pointer, with exceptions described below).
(现在所有 kfunc 默认都要求受信任的参数。这意味着所有指针参数都必须有效，并且所有
指向 BTF 对象的指针必须以其未经修改的形式传入（偏移为零，且不能是通过走查另一个
指针得到的，下文描述的例外情况除外）。)

There are two types of pointers to kernel objects which are considered "trusted":
(指向内核对象的指针有两类被视为 "受信任"：)

1. Pointers which are passed as tracepoint or struct_ops callback arguments.
   (作为 tracepoint 或 struct_ops 回调参数传入的指针。)
2. Pointers which were returned from a KF_ACQUIRE kfunc.
   (从 KF_ACQUIRE kfunc 返回的指针。)

Pointers to non-BTF objects (e.g. scalar pointers) may also be passed to
kfuncs, and may have a non-zero offset.
(指向非 BTF 对象的指针（例如标量指针）也可以传给 kfunc，并且可以带有非零偏移。)

The definition of "valid" pointers is subject to change at any time, and has
absolutely no ABI stability guarantees.
("有效" 指针的定义随时可能改变，完全没有 ABI 稳定性保证。)

As mentioned above, a nested pointer obtained from walking a trusted pointer is
no longer trusted, with one exception. If a struct type has a field that is
guaranteed to be valid (trusted or rcu, as in KF_RCU description below) as long
as its parent pointer is valid, the following macros can be used to express
that to the verifier:
(如上所述，通过走查受信任指针得到的嵌套指针不再受信任，只有一个例外。如果某个
结构体类型有一个字段，只要其父指针有效就保证有效（受信任或 rcu，如下文 KF_RCU
描述中那样），可以使用以下宏向验证器表达这一点：)

* ``BTF_TYPE_SAFE_TRUSTED`` (受信任)
* ``BTF_TYPE_SAFE_RCU`` (RCU)
* ``BTF_TYPE_SAFE_RCU_OR_NULL`` (RCU 或 NULL)

For example,
(例如：)

.. code-block:: c

	BTF_TYPE_SAFE_TRUSTED(struct socket) {
		struct sock *sk;
	};

or
(或者：)

.. code-block:: c

	BTF_TYPE_SAFE_RCU(struct task_struct) {
		const cpumask_t *cpus_ptr;
		struct css_set __rcu *cgroups;
		struct task_struct __rcu *real_parent;
		struct task_struct *group_leader;
	};

In other words, you must:
(换句话说，你必须：)

1. Wrap the valid pointer type in a ``BTF_TYPE_SAFE_*`` macro.
   (把有效指针类型包裹在 ``BTF_TYPE_SAFE_*`` 宏中。)

2. Specify the type and name of the valid nested field. This field must match
   the field in the original type definition exactly.
   (指定有效嵌套字段的类型与名称。该字段必须与原始类型定义中的字段完全一致。)

A new type declared by a ``BTF_TYPE_SAFE_*`` macro also needs to be emitted so
that it appears in BTF. For example, ``BTF_TYPE_SAFE_TRUSTED(struct socket)``
is emitted in the ``type_is_trusted()`` function as follows:
(由 ``BTF_TYPE_SAFE_*`` 宏声明的新类型还需要被发射（emit），使其出现在 BTF 中。
例如，``BTF_TYPE_SAFE_TRUSTED(struct socket)`` 在 ``type_is_trusted()`` 函数中
按如下方式发射：)

.. code-block:: c

	BTF_TYPE_EMIT(BTF_TYPE_SAFE_TRUSTED(struct socket));

2.3 Annotating kfunc parameters(2.3 为 kfunc 参数添加注解)
----------------------------------------------------------

Similar to BPF helpers, there is sometime need for additional context required
by the verifier to make the usage of kernel functions safer and more useful.
Hence, we can annotate a parameter by suffixing the name of the argument of the
kfunc with a __tag, where tag may be one of the supported annotations.
(与 BPF helper 类似，有时需要向验证器提供额外上下文，使内核函数的使用更安全、更有
用。因此，我们可以通过在 kfunc 参数名后面加上 __tag 后缀来注解参数，tag 可以是
所支持的注解之一。)

2.3.1 __sz Annotation(2.3.1 __sz 注解)
--------------------------------------

This annotation is used to indicate a memory and size pair in the argument list.
An example is given below::

        __bpf_kfunc void bpf_memzero(void *mem, int mem__sz)
        {
        ...
        }

(该注解用于表示参数列表中的内存与大小对。下面给出一个示例：)

Here, the verifier will treat first argument as a PTR_TO_MEM, and second
argument as its size. By default, without __sz annotation, the size of the type
of the pointer is used. Without __sz annotation, a kfunc cannot accept a void
pointer.
(这里，验证器会把第一个参数视为 PTR_TO_MEM，第二个参数视为其大小。默认情况下，
如果没有 __sz 注解，则使用指针类型的大小。如果没有 __sz 注解，kfunc 不能接受
void 指针。)

2.3.2 __k Annotation(2.3.2 __k 注解)
------------------------------------

This annotation is only understood for scalar arguments, where it indicates that
the verifier must check the scalar argument to be a known constant, which does
not indicate a size parameter, and the value of the constant is relevant to the
safety of the program.
(该注解只对标量参数有意义，它表示验证器必须检查该标量参数是已知常量，且不表示
大小参数，同时该常量的值与程序的安全性相关。)

An example is given below::

        __bpf_kfunc void *bpf_obj_new(u32 local_type_id__k, ...)
        {
        ...
        }

(下面给出一个示例：)

Here, bpf_obj_new uses local_type_id argument to find out the size of that type
ID in program's BTF and return a sized pointer to it. Each type ID will have a
distinct size, hence it is crucial to treat each such call as distinct when
values don't match during verifier state pruning checks.
(这里，bpf_obj_new 使用 local_type_id 参数在程序的 BTF 中找出该类型 ID 的大小，
并返回一个带大小的指针。每个类型 ID 都有不同的大小，因此在验证器状态剪枝检查
期间值不匹配时，必须把每次这样的调用视为不同的调用。)

Hence, whenever a constant scalar argument is accepted by a kfunc which is not a
size parameter, and the value of the constant matters for program safety, __k
suffix should be used.
(因此，每当 kfunc 接受一个不是大小参数的常量标量参数、且该常量的值关系到程序
安全性时，就应使用 __k 后缀。)

2.3.3 __uninit Annotation(2.3.3 __uninit 注解)
----------------------------------------------

This annotation is used to indicate that the argument will be treated as
uninitialized.
(该注解用于表示该参数将被视为未初始化。)

An example is given below::

        __bpf_kfunc int bpf_dynptr_from_skb(..., struct bpf_dynptr_kern *ptr__uninit)
        {
        ...
        }

(下面给出一个示例：)

Here, the dynptr will be treated as an uninitialized dynptr. Without this
annotation, the verifier will reject the program if the dynptr passed in is
not initialized.
(这里，dynptr 将被视为未初始化的 dynptr。如果没有该注解，当传入的 dynptr 未
初始化时，验证器会拒绝该程序。)

2.3.4 __nullable Annotation(2.3.4 __nullable 注解)
--------------------------------------------------

This annotation is used to indicate that the pointer argument may be NULL.
The verifier will allow passing NULL for such arguments.
(该注解用于表示指针参数可以为 NULL。验证器将允许为此类参数传入 NULL。)

An example is given below::

        __bpf_kfunc void bpf_task_release(struct task_struct *task__nullable)
        {
        ...
        }

(下面给出一个示例：)

Here, the task pointer may be NULL. The kfunc is responsible for checking if
the pointer is NULL before dereferencing it.
(这里，task 指针可以为 NULL。kfunc 负责在解引用之前检查指针是否为 NULL。)

The __nullable annotation can be combined with other annotations. For example,
when used with __sz or __szk annotations for memory and size pairs, the
verifier will skip size validation when a NULL pointer is passed, but will
still process the size argument to extract constant size information when
needed::

        __bpf_kfunc void *bpf_dynptr_slice(..., void *buffer__nullable,
                                           u32 buffer__szk)

(该注解可以与其他注解组合使用。例如，当与用于内存和大小对的 __sz 或 __szk 注解
一起使用时，验证器在传入 NULL 指针时会跳过大小验证，但在需要时仍会处理大小参数
以提取常量大小信息：)

(这里，buffer 可以为 NULL。如果 buffer 不为 NULL，它的大小必须至少为
buffer__szk 字节。kfunc 负责在使用 buffer 之前检查它是否为 NULL。)

Here, the buffer may be NULL. If the buffer is not NULL, it must be at least
buffer__szk bytes in size. The kfunc is responsible for checking if the buffer
is NULL before using it.

2.3.5 __str Annotation(2.3.5 __str 注解)
----------------------------------------
This annotation is used to indicate that the argument is a constant string.
(该注解用于表示该参数是常量字符串。)

An example is given below::

        __bpf_kfunc bpf_get_file_xattr(..., const char *name__str, ...)
        {
        ...
        }

(下面给出一个示例：)

In this case, ``bpf_get_file_xattr()`` can be called as::

        bpf_get_file_xattr(..., "xattr_name", ...);

(在这种情况下，``bpf_get_file_xattr()`` 可以按如下方式调用：)

Or::

        const char name[] = "xattr_name";  /* This need to be global */
        int BPF_PROG(...)
        {
                ...
                bpf_get_file_xattr(..., name, ...);
                ...
        }

(或者：)

.. _BPF_kfunc_nodef:

2.4 Using an existing kernel function(2.4 使用现有的内核函数)
-------------------------------------------------------------

When an existing function in the kernel is fit for consumption by BPF programs,
it can be directly registered with the BPF subsystem. However, care must still
be taken to review the context in which it will be invoked by the BPF program
and whether it is safe to do so.
(当内核中的现有函数适合供 BPF 程序使用时，可以直接把它注册到 BPF 子系统。不过，
仍然必须仔细审查它将被 BPF 程序调用的上下文，以及这样做是否安全。)

2.5 Annotating kfuncs(2.5 为 kfunc 添加注解)
--------------------------------------------

In addition to kfuncs' arguments, verifier may need more information about the
type of kfunc(s) being registered with the BPF subsystem. To do so, we define
flags on a set of kfuncs as follows::

        BTF_KFUNCS_START(bpf_task_set)
        BTF_ID_FLAGS(func, bpf_get_task_pid, KF_ACQUIRE | KF_RET_NULL)
        BTF_ID_FLAGS(func, bpf_put_pid, KF_RELEASE)
        BTF_KFUNCS_END(bpf_task_set)

(除了 kfunc 的参数之外，验证器可能还需要关于注册到 BPF 子系统的 kfunc 类型的
更多信息。为此，我们为一组 kfunc 定义标志，如下所示：)

(这个集合为上面列出的每个 kfunc 编码其 BTF ID，并把标志一起编码。当然，也允许
不指定任何标志。)

This set encodes the BTF ID of each kfunc listed above, and encodes the flags
along with it. Ofcourse, it is also allowed to specify no flags.

kfunc definitions should also always be annotated with the ``__bpf_kfunc``
macro. This prevents issues such as the compiler inlining the kfunc if it's a
static kernel function, or the function being elided in an LTO build as it's
not used in the rest of the kernel. Developers should not manually add
annotations to their kfunc to prevent these issues. If an annotation is
required to prevent such an issue with your kfunc, it is a bug and should be
added to the definition of the macro so that other kfuncs are similarly
protected. An example is given below::

        __bpf_kfunc struct task_struct *bpf_get_task_pid(s32 pid)
        {
        ...
        }

(kfunc 定义还应当始终用 ``__bpf_kfunc`` 宏注解。这可以防止诸如编译器内联 kfunc
（如果它是 static 内核函数）、或该函数在 LTO 构建中因内核其余部分未使用而被删除
之类的问题。开发者不应手动向自己的 kfunc 添加注解来防止这些问题。如果需要某个
注解来防止你的 kfunc 出现这类问题，那就是一个 bug，应当把该注解加入宏的定义中，
使其他 kfunc 也同样受到保护。下面给出一个示例：)

2.5.1 KF_ACQUIRE flag(2.5.1 KF_ACQUIRE 标志)
--------------------------------------------

The KF_ACQUIRE flag is used to indicate that the kfunc returns a pointer to a
refcounted object. The verifier will then ensure that the pointer to the object
is eventually released using a release kfunc, or transferred to a map using a
referenced kptr (by invoking bpf_kptr_xchg). If not, the verifier fails the
loading of the BPF program until no lingering references remain in all possible
explored states of the program.
(KF_ACQUIRE 标志用于表示 kfunc 返回一个指向引用计数对象的指针。验证器随后会确保
指向该对象的指针最终通过 release kfunc 释放，或通过引用 kptr（调用 bpf_kptr_xchg）
转移到 map 中。否则，在程序所有可能探索到的状态中还有未释放的引用时，验证器会
拒绝加载该 BPF 程序。)

2.5.2 KF_RET_NULL flag(2.5.2 KF_RET_NULL 标志)
----------------------------------------------

The KF_RET_NULL flag is used to indicate that the pointer returned by the kfunc
may be NULL. Hence, it forces the user to do a NULL check on the pointer
returned from the kfunc before making use of it (dereferencing or passing to
another helper). This flag is often used in pairing with KF_ACQUIRE flag, but
both are orthogonal to each other.
(KF_RET_NULL 标志用于表示 kfunc 返回的指针可能为 NULL。因此，它强制用户在利用
（解引用或传给其他 helper）该 kfunc 返回的指针之前进行 NULL 检查。该标志常与
KF_ACQUIRE 标志配对使用，但二者相互正交。)

2.5.3 KF_RELEASE flag(2.5.3 KF_RELEASE 标志)
--------------------------------------------

The KF_RELEASE flag is used to indicate that the kfunc releases the pointer
passed in to it. There can be only one referenced pointer that can be passed
in. All copies of the pointer being released are invalidated as a result of
invoking kfunc with this flag.
(KF_RELEASE 标志用于表示 kfunc 会释放传入的指针。只能传入一个引用指针。调用带
此标志的 kfunc 会使被释放指针的所有副本失效。)

2.5.4 KF_SLEEPABLE flag(2.5.4 KF_SLEEPABLE 标志)
------------------------------------------------

The KF_SLEEPABLE flag is used for kfuncs that may sleep. Such kfuncs can only
be called by sleepable BPF programs (BPF_F_SLEEPABLE).
(KF_SLEEPABLE 标志用于可能睡眠的 kfunc。这类 kfunc 只能由可睡眠的 BPF 程序
（BPF_F_SLEEPABLE）调用。)

2.5.5 KF_DESTRUCTIVE flag(2.5.5 KF_DESTRUCTIVE 标志)
----------------------------------------------------

The KF_DESTRUCTIVE flag is used to indicate functions calling which is
destructive to the system. For example such a call can result in system
rebooting or panicking. Due to this additional restrictions apply to these
calls. At the moment they only require CAP_SYS_BOOT capability, but more can be
added later.
(KF_DESTRUCTIVE 标志用于表示调用该函数会对系统造成破坏。例如这样的调用可能导致
系统重启或 panic。因此这些调用受到额外的限制。目前它们只要求 CAP_SYS_BOOT 能力，
但以后可能会增加更多。)

2.5.6 KF_RCU flag(2.5.6 KF_RCU 标志)
------------------------------------

The KF_RCU flag allows kfuncs to opt out of the default trusted args
requirement and accept RCU pointers with weaker guarantees. The kfuncs marked
with KF_RCU expect either PTR_TRUSTED or MEM_RCU arguments. The verifier
guarantees that the objects are valid and there is no use-after-free. The
pointers are not NULL, but the object's refcount could have reached zero. The
kfuncs need to consider doing refcnt != 0 check, especially when returning a
KF_ACQUIRE pointer. Note as well that a KF_ACQUIRE kfunc that is KF_RCU should
very likely also be KF_RET_NULL.
(KF_RCU 标志允许 kfunc 退出默认的受信任参数要求，接受保证更弱的 RCU 指针。标记
为 KF_RCU 的 kfunc 期望 PTR_TRUSTED 或 MEM_RCU 参数。验证器保证对象有效且没有
use-after-free。指针不为 NULL，但对象的引用计数可能已到零。这些 kfunc 需要考虑
做 refcnt != 0 检查，尤其是在返回 KF_ACQUIRE 指针时。另请注意，KF_RCU 的
KF_ACQUIRE kfunc 很可能也应标记为 KF_RET_NULL。)

2.5.7 KF_RCU_PROTECTED flag(2.5.7 KF_RCU_PROTECTED 标志)
--------------------------------------------------------

The KF_RCU_PROTECTED flag is used to indicate that the kfunc must be invoked in
an RCU critical section. This is assumed by default in non-sleepable programs,
and must be explicitly ensured by calling ``bpf_rcu_read_lock`` for sleepable
ones.
(KF_RCU_PROTECTED 标志用于表示必须在 RCU 临界区内调用该 kfunc。在不可睡眠程序中
这是默认假设；对于可睡眠程序，必须通过调用 ``bpf_rcu_read_lock`` 显式保证。)

If the kfunc returns a pointer value, this flag also enforces that the returned
pointer is RCU protected, and can only be used while the RCU critical section is
active.
(如果 kfunc 返回指针值，该标志还强制要求返回的指针受 RCU 保护，并且只能在 RCU
临界区活跃期间使用。)

The flag is distinct from the ``KF_RCU`` flag, which only ensures that its
arguments are at least RCU protected pointers. This may transitively imply that
RCU protection is ensured, but it does not work in cases of kfuncs which require
RCU protection but do not take RCU protected arguments.
(该标志与 ``KF_RCU`` 标志不同，后者只确保其参数至少是受 RCU 保护的指针。这可能
间接意味着已确保 RCU 保护，但对于那些需要 RCU 保护却不接受受 RCU 保护参数的
kfunc，它并不适用。)

.. _KF_deprecated_flag:

2.5.8 KF_DEPRECATED flag(2.5.8 KF_DEPRECATED 标志)
--------------------------------------------------

The KF_DEPRECATED flag is used for kfuncs which are scheduled to be
changed or removed in a subsequent kernel release. A kfunc that is
marked with KF_DEPRECATED should also have any relevant information
captured in its kernel doc. Such information typically includes the
kfunc's expected remaining lifespan, a recommendation for new
functionality that can replace it if any is available, and possibly a
rationale for why it is being removed.
(KF_DEPRECATED 标志用于计划在后续内核版本中更改或移除的 kfunc。标记为
KF_DEPRECATED 的 kfunc 还应把任何相关信息记录在其 kernel doc 中。这类信息通常
包括该 kfunc 预期的剩余寿命、对可以替换它的新功能的建议（如果有的话），以及可能
说明其被移除理由的内容。)

Note that while on some occasions, a KF_DEPRECATED kfunc may continue to be
supported and have its KF_DEPRECATED flag removed, it is likely to be far more
difficult to remove a KF_DEPRECATED flag after it's been added than it is to
prevent it from being added in the first place. As described in
:ref:`BPF_kfunc_lifecycle_expectations`, users that rely on specific kfuncs are
encouraged to make their use-cases known as early as possible, and participate
in upstream discussions regarding whether to keep, change, deprecate, or remove
those kfuncs if and when such discussions occur.
(请注意，虽然有时 KF_DEPRECATED kfunc 可能继续受支持并且其 KF_DEPRECATED 标志被
移除，但在标志添加之后要移除 KF_DEPRECATED 标志，很可能比一开始就阻止它被添加
困难得多。如 :ref:`BPF_kfunc_lifecycle_expectations` 所述，鼓励依赖特定 kfunc 的
用户尽早让他们的用例为人所知，并在上游就保留、更改、弃用或移除这些 kfunc 的讨论
出现时参与其中。)

2.5.9 KF_IMPLICIT_ARGS flag(2.5.9 KF_IMPLICIT_ARGS 标志)
--------------------------------------------------------

The KF_IMPLICIT_ARGS flag is used to indicate that the BPF signature
of the kfunc is different from it's kernel signature, and the values
for implicit arguments are provided at load time by the verifier.
(KF_IMPLICIT_ARGS 标志用于表示 kfunc 的 BPF 签名与其内核签名不同，隐式参数的值
由验证器在加载时提供。)

Only arguments of specific types are implicit.
Currently only ``struct bpf_prog_aux *`` type is supported.
(只有特定类型的参数才是隐式的。目前只支持 ``struct bpf_prog_aux *`` 类型。)

A kfunc with KF_IMPLICIT_ARGS flag therefore has two types in BTF: one
function matching the kernel declaration (with _impl suffix in the
name by convention), and another matching the intended BPF API.
(因此，带 KF_IMPLICIT_ARGS 标志的 kfunc 在 BTF 中有两个类型：一个匹配内核声明
（按惯例名称带 _impl 后缀），另一个匹配预期的 BPF API。)

Verifier only allows calls to the non-_impl version of a kfunc, that
uses a signature without the implicit arguments.
(验证器只允许调用 kfunc 的非 _impl 版本，即使用不带隐式参数签名的版本。)

Example declaration:
(声明示例：)

.. code-block:: c

	__bpf_kfunc int bpf_task_work_schedule_signal(struct task_struct *task, struct bpf_task_work *tw,
						      void *map__map, bpf_task_work_callback_t callback,
						      struct bpf_prog_aux *aux) { ... }

Example usage in BPF program:
(BPF 程序中的使用示例：)

.. code-block:: c

	/* note that the last argument is omitted */
        bpf_task_work_schedule_signal(task, &work->tw, &arrmap, task_work_callback);

2.6 Registering the kfuncs(2.6 注册 kfunc)
------------------------------------------

Once the kfunc is prepared for use, the final step to making it visible is
registering it with the BPF subsystem. Registration is done per BPF program
type. An example is shown below::

        BTF_KFUNCS_START(bpf_task_set)
        BTF_ID_FLAGS(func, bpf_get_task_pid, KF_ACQUIRE | KF_RET_NULL)
        BTF_ID_FLAGS(func, bpf_put_pid, KF_RELEASE)
        BTF_KFUNCS_END(bpf_task_set)

        static const struct btf_kfunc_id_set bpf_task_kfunc_set = {
                .owner = THIS_MODULE,
                .set   = &bpf_task_set,
        };

        static int init_subsystem(void)
        {
                return register_btf_kfunc_id_set(BPF_PROG_TYPE_TRACING, &bpf_task_kfunc_set);
        }
        late_initcall(init_subsystem);

(kfunc 准备好供使用之后，使其可见的最后一步是把它注册到 BPF 子系统。注册按 BPF
程序类型进行。示例如下：)

2.7  Specifying no-cast aliases with ___init(2.7  用 ___init 指定无强制转换别名)
--------------------------------------------------------------------------------

The verifier will always enforce that the BTF type of a pointer passed to a
kfunc by a BPF program, matches the type of pointer specified in the kfunc
definition. The verifier, does, however, allow types that are equivalent
according to the C standard to be passed to the same kfunc arg, even if their
BTF_IDs differ.
(验证器总是强制要求 BPF 程序传给 kfunc 的指针的 BTF 类型与 kfunc 定义中指定的
指针类型匹配。不过，验证器确实允许把按 C 标准等价的类型传给同一个 kfunc 参数，
即使它们的 BTF_ID 不同。)

For example, for the following type definition:
(例如，对于以下类型定义：)

.. code-block:: c

	struct bpf_cpumask {
		cpumask_t cpumask;
		refcount_t usage;
	};

The verifier would allow a ``struct bpf_cpumask *`` to be passed to a kfunc
taking a ``cpumask_t *`` (which is a typedef of ``struct cpumask *``). For
instance, both ``struct cpumask *`` and ``struct bpf_cpmuask *`` can be passed
to bpf_cpumask_test_cpu().
(验证器会允许把 ``struct bpf_cpumask *`` 传给接受 ``cpumask_t *``（它是
``struct cpumask *`` 的 typedef）的 kfunc。例如，``struct cpumask *`` 与
``struct bpf_cpmuask *`` 都可以传给 bpf_cpumask_test_cpu()。)

In some cases, this type-aliasing behavior is not desired. ``struct
nf_conn___init`` is one such example:
(在某些情况下，这种类型别名行为并不可取。``struct nf_conn___init`` 就是这样一个
例子：)

.. code-block:: c

	struct nf_conn___init {
		struct nf_conn ct;
	};

The C standard would consider these types to be equivalent, but it would not
always be safe to pass either type to a trusted kfunc. ``struct
nf_conn___init`` represents an allocated ``struct nf_conn`` object that has
*not yet been initialized*, so it would therefore be unsafe to pass a ``struct
nf_conn___init *`` to a kfunc that's expecting a fully initialized ``struct
nf_conn *`` (e.g. ``bpf_ct_change_timeout()``).
(C 标准会认为这些类型等价，但把其中任意一种类型传给受信任的 kfunc 并不总是安全的。
``struct nf_conn___init`` 表示一个已分配但 *尚未初始化* 的 ``struct nf_conn``
对象，因此把 ``struct nf_conn___init *`` 传给期望完全初始化的 ``struct nf_conn *``
的 kfunc（例如 ``bpf_ct_change_timeout()``）是不安全的。)

In order to accommodate such requirements, the verifier will enforce strict
PTR_TO_BTF_ID type matching if two types have the exact same name, with one
being suffixed with ``___init``.
(为了满足这类需求，如果两个类型的名称完全相同、只是其中一个带有 ``___init`` 后缀，
验证器将强制执行严格的 PTR_TO_BTF_ID 类型匹配。)

.. _BPF_kfunc_lifecycle_expectations:

3. kfunc lifecycle expectations(3. kfunc 生命周期预期)
======================================================

kfuncs provide a kernel <-> kernel API, and thus are not bound by any of the
strict stability restrictions associated with kernel <-> user UAPIs. This means
they can be thought of as similar to EXPORT_SYMBOL_GPL, and can therefore be
modified or removed by a maintainer of the subsystem they're defined in when
it's deemed necessary.
(kfunc 提供的是内核 <-> 内核 API，因此不受内核 <-> 用户 UAPI 的任何严格稳定性
限制约束。这意味着可以把它们类比为 EXPORT_SYMBOL_GPL，因此当认为必要时，定义
它们的子系统的维护者可以修改或移除它们。)

Like any other change to the kernel, maintainers will not change or remove a
kfunc without having a reasonable justification.  Whether or not they'll choose
to change a kfunc will ultimately depend on a variety of factors, such as how
widely used the kfunc is, how long the kfunc has been in the kernel, whether an
alternative kfunc exists, what the norm is in terms of stability for the
subsystem in question, and of course what the technical cost is of continuing
to support the kfunc.
(与对内核的任何其他更改一样，维护者不会在没有合理解释的情况下更改或移除 kfunc。
他们是否选择更改某个 kfunc，最终取决于多种因素，例如该 kfunc 的使用广泛程度、
它在内核中存在多久、是否存在替代的 kfunc、相关子系统在稳定性方面的惯例，以及
继续支持该 kfunc 的技术成本等。)

There are several implications of this:
(这有如下几点含义：)

a) kfuncs that are widely used or have been in the kernel for a long time will
   be more difficult to justify being changed or removed by a maintainer. In
   other words, kfuncs that are known to have a lot of users and provide
   significant value provide stronger incentives for maintainers to invest the
   time and complexity in supporting them. It is therefore important for
   developers that are using kfuncs in their BPF programs to communicate and
   explain how and why those kfuncs are being used, and to participate in
   discussions regarding those kfuncs when they occur upstream.
   (a) 被广泛使用或在内核中存在已久的 kfunc，维护者更难为更改或移除它们找到正当
   理由。换言之，已知拥有大量用户并提供显著价值的 kfunc，为维护者投入时间与复杂
   度来支持它们提供了更强的激励。因此，在 BPF 程序中使用 kfunc 的开发者很重要
   的一点是：沟通并解释这些 kfunc 是如何以及为何被使用的，并在上游出现关于这些
   kfunc 的讨论时参与其中。)

b) Unlike regular kernel symbols marked with EXPORT_SYMBOL_GPL, BPF programs
   that call kfuncs are generally not part of the kernel tree. This means that
   refactoring cannot typically change callers in-place when a kfunc changes,
   as is done for e.g. an upstreamed driver being updated in place when a
   kernel symbol is changed.
   (b) 与用 EXPORT_SYMBOL_GPL 标记的常规内核符号不同，调用 kfunc 的 BPF 程序
   通常不属于内核树。这意味着当某个 kfunc 发生变化时，重构通常无法就地修改调用
   方，不像例如上游驱动在内核符号变化时被就地更新那样。)

   Unlike with regular kernel symbols, this is expected behavior for BPF
   symbols, and out-of-tree BPF programs that use kfuncs should be considered
   relevant to discussions and decisions around modifying and removing those
   kfuncs. The BPF community will take an active role in participating in
   upstream discussions when necessary to ensure that the perspectives of such
   users are taken into account.
   (与常规内核符号不同，这是 BPF 符号的预期行为，使用 kfunc 的树外 BPF 程序应被
   视为与修改和移除这些 kfunc 的讨论及决策相关。BPF 社区会在必要时积极参与上游
   讨论，以确保这类用户的视角得到考虑。)

c) A kfunc will never have any hard stability guarantees. BPF APIs cannot and
   will not ever hard-block a change in the kernel purely for stability
   reasons. That being said, kfuncs are features that are meant to solve
   problems and provide value to users. The decision of whether to change or
   remove a kfunc is a multivariate technical decision that is made on a
   case-by-case basis, and which is informed by data points such as those
   mentioned above. It is expected that a kfunc being removed or changed with
   no warning will not be a common occurrence or take place without sound
   justification, but it is a possibility that must be accepted if one is to
   use kfuncs.
   (c) kfunc 永远不会有任何硬性稳定性保证。BPF API 不能也不会仅仅为了稳定性而在
   内核中硬性阻止变更。话虽如此，kfunc 是旨在解决问题并为用户提供价值的功能。
   是否更改或移除某个 kfunc 的决定，是在逐个案例的基础上做出的多变量技术决策，
   其依据包括上文提到的那些数据点。预期 "毫无预警地移除或更改 kfunc" 不会是常见
   情况，也不会在没有充分理由的情况下发生；但如果要使用 kfunc，就必须接受这种
   可能性。)

3.1 kfunc deprecation(3.1 kfunc 弃用)
-------------------------------------

As described above, while sometimes a maintainer may find that a kfunc must be
changed or removed immediately to accommodate some changes in their subsystem,
usually kfuncs will be able to accommodate a longer and more measured
deprecation process. For example, if a new kfunc comes along which provides
superior functionality to an existing kfunc, the existing kfunc may be
deprecated for some period of time to allow users to migrate their BPF programs
to use the new one. Or, if a kfunc has no known users, a decision may be made
to remove the kfunc (without providing an alternative API) after some
deprecation period so as to provide users with a window to notify the kfunc
maintainer if it turns out that the kfunc is actually being used.
(如上所述，虽然有时维护者可能发现某个 kfunc 必须立即更改或移除以适应其子系统的
某些变化，但通常 kfunc 可以经历更长、更有条理的弃用过程。例如，如果出现了一个
比现有 kfunc 功能更优的新 kfunc，现有 kfunc 可能会被弃用一段时间，让用户把他们
的 BPF 程序迁移到新的 kfunc 上。又如，如果某个 kfunc 没有已知用户，可能会决定
在一段弃用期后移除该 kfunc（不提供替代 API），以便给用户一个窗口，在发现该 kfunc
实际上在被使用时通知 kfunc 维护者。)

It's expected that the common case will be that kfuncs will go through a
deprecation period rather than being changed or removed without warning. As
described in :ref:`KF_deprecated_flag`, the kfunc framework provides the
KF_DEPRECATED flag to kfunc developers to signal to users that a kfunc has been
deprecated. Once a kfunc has been marked with KF_DEPRECATED, the following
procedure is followed for removal:
(预期常见情况是 kfunc 会经历弃用期，而不是在毫无预警的情况下被更改或移除。如
:ref:`KF_deprecated_flag` 所述，kfunc 框架为 kfunc 开发者提供了 KF_DEPRECATED
标志，用于向用户表明某个 kfunc 已被弃用。一旦某个 kfunc 被标记为 KF_DEPRECATED，
其移除将遵循以下流程：)

1. Any relevant information for deprecated kfuncs is documented in the kfunc's
   kernel docs. This documentation will typically include the kfunc's expected
   remaining lifespan, a recommendation for new functionality that can replace
   the usage of the deprecated function (or an explanation as to why no such
   replacement exists), etc.
   (1. 已弃用 kfunc 的所有相关信息都记录在该 kfunc 的 kernel docs 中。这些文档
   通常包括该 kfunc 预期的剩余寿命、对可替代该已弃用函数用法的新功能的建议
   （或解释为何不存在这样的替代），等等。)

2. The deprecated kfunc is kept in the kernel for some period of time after it
   was first marked as deprecated. This time period will be chosen on a
   case-by-case basis, and will typically depend on how widespread the use of
   the kfunc is, how long it has been in the kernel, and how hard it is to move
   to alternatives. This deprecation time period is "best effort", and as
   described :ref:`above<BPF_kfunc_lifecycle_expectations>`, circumstances may
   sometimes dictate that the kfunc be removed before the full intended
   deprecation period has elapsed.
   (2. 已弃用的 kfunc 在首次标记为弃用之后，会在内核中保留一段时间。该时长将逐个
   案例决定，通常取决于该 kfunc 的使用广泛程度、它在内核中存在多久，以及迁移到
   替代方案的难度。这一弃用期是 "尽力而为" 的，如
   :ref:`上文<BPF_kfunc_lifecycle_expectations>` 所述，有时情况可能要求在该 kfunc
   完整的预期弃用期结束之前就将其移除。)

3. After the deprecation period the kfunc will be removed. At this point, BPF
   programs calling the kfunc will be rejected by the verifier.
   (3. 弃用期过后，该 kfunc 将被移除。届时，调用该 kfunc 的 BPF 程序将被验证器
   拒绝。)

4. Core kfuncs(4. 核心 kfunc)
=============================

The BPF subsystem provides a number of "core" kfuncs that are potentially
applicable to a wide variety of different possible use cases and programs.
Those kfuncs are documented here.
(BPF 子系统提供了若干 "核心" kfunc，它们可能适用于各种不同的用例与程序。这些
kfunc 记录于此。)

4.1 struct task_struct * kfuncs(4.1 struct task_struct * kfunc)
---------------------------------------------------------------

There are a number of kfuncs that allow ``struct task_struct *`` objects to be
used as kptrs:
(有许多 kfunc 允许把 ``struct task_struct *`` 对象用作 kptr：)

.. kernel-doc:: kernel/bpf/helpers.c
   :identifiers: bpf_task_acquire bpf_task_release

These kfuncs are useful when you want to acquire or release a reference to a
``struct task_struct *`` that was passed as e.g. a tracepoint arg, or a
struct_ops callback arg. For example:
(当你想获取或释放一个作为 tracepoint 参数或 struct_ops 回调参数传入的
``struct task_struct *`` 的引用时，这些 kfunc 很有用。例如：)

.. code-block:: c

	/**
	 * A trivial example tracepoint program that shows how to
	 * acquire and release a struct task_struct * pointer.
	 */
	SEC("tp_btf/task_newtask")
	int BPF_PROG(task_acquire_release_example, struct task_struct *task, u64 clone_flags)
	{
		struct task_struct *acquired;

		acquired = bpf_task_acquire(task);
		if (acquired)
			/*
			 * In a typical program you'd do something like store
			 * the task in a map, and the map will automatically
			 * release it later. Here, we release it manually.
			 */
			bpf_task_release(acquired);
		return 0;
	}


References acquired on ``struct task_struct *`` objects are RCU protected.
Therefore, when in an RCU read region, you can obtain a pointer to a task
embedded in a map value without having to acquire a reference:
(在 ``struct task_struct *`` 对象上获取的引用受 RCU 保护。因此，当处于 RCU 读
区域时，你可以获得指向嵌入在 map 值中的任务的指针，而无需获取引用：)

.. code-block:: c

	#define private(name) SEC(".data." #name) __hidden __attribute__((aligned(8)))
	private(TASK) static struct task_struct *global;

	/**
	 * A trivial example showing how to access a task stored
	 * in a map using RCU.
	 */
	SEC("tp_btf/task_newtask")
	int BPF_PROG(task_rcu_read_example, struct task_struct *task, u64 clone_flags)
	{
		struct task_struct *local_copy;

		bpf_rcu_read_lock();
		local_copy = global;
		if (local_copy)
			/*
			 * We could also pass local_copy to kfuncs or helper functions here,
			 * as we're guaranteed that local_copy will be valid until we exit
			 * the RCU read region below.
			 */
			bpf_printk("Global task %s is valid", local_copy->comm);
		else
			bpf_printk("No global task found");
		bpf_rcu_read_unlock();

		/* At this point we can no longer reference local_copy. */

		return 0;
	}

----

A BPF program can also look up a task from a pid. This can be useful if the
caller doesn't have a trusted pointer to a ``struct task_struct *`` object that
it can acquire a reference on with bpf_task_acquire().
(BPF 程序还可以通过 pid 查找任务。如果调用者没有可以对其调用 bpf_task_acquire()
获取引用的受信任 ``struct task_struct *`` 指针，这会很有用。)

.. kernel-doc:: kernel/bpf/helpers.c
   :identifiers: bpf_task_from_pid

Here is an example of it being used:
(下面是它的一个使用示例：)

.. code-block:: c

	SEC("tp_btf/task_newtask")
	int BPF_PROG(task_get_pid_example, struct task_struct *task, u64 clone_flags)
	{
		struct task_struct *lookup;

		lookup = bpf_task_from_pid(task->pid);
		if (!lookup)
			/* A task should always be found, as %task is a tracepoint arg. */
			return -ENOENT;

		if (lookup->pid != task->pid) {
			/* bpf_task_from_pid() looks up the task via its
			 * globally-unique pid from the init_pid_ns. Thus,
			 * the pid of the lookup task should always be the
			 * same as the input task.
			 */
			bpf_task_release(lookup);
			return -EINVAL;
		}

		/* bpf_task_from_pid() returns an acquired reference,
		 * so it must be dropped before returning from the
		 * tracepoint handler.
		 */
		bpf_task_release(lookup);
		return 0;
	}

4.2 struct cgroup * kfuncs(4.2 struct cgroup * kfunc)
-----------------------------------------------------

``struct cgroup *`` objects also have acquire and release functions:
(``struct cgroup *`` 对象同样有获取与释放函数：)

.. kernel-doc:: kernel/bpf/helpers.c
   :identifiers: bpf_cgroup_acquire bpf_cgroup_release

These kfuncs are used in exactly the same manner as bpf_task_acquire() and
bpf_task_release() respectively, so we won't provide examples for them.
(这些 kfunc 的使用方式分别与 bpf_task_acquire() 和 bpf_task_release() 完全相同，
所以我们不为它们提供示例。)

----

Other kfuncs available for interacting with ``struct cgroup *`` objects are
bpf_cgroup_ancestor() and bpf_cgroup_from_id(), allowing callers to access
the ancestor of a cgroup and find a cgroup by its ID, respectively. Both
return a cgroup kptr.
(其他可用于与 ``struct cgroup *`` 对象交互的 kfunc 有 bpf_cgroup_ancestor() 与
bpf_cgroup_from_id()，分别允许调用者访问 cgroup 的祖先和按 ID 查找 cgroup。二者
都返回一个 cgroup kptr。)

.. kernel-doc:: kernel/bpf/helpers.c
   :identifiers: bpf_cgroup_ancestor

.. kernel-doc:: kernel/bpf/helpers.c
   :identifiers: bpf_cgroup_from_id

Eventually, BPF should be updated to allow this to happen with a normal memory
load in the program itself. This is currently not possible without more work in
the verifier. bpf_cgroup_ancestor() can be used as follows:
(最终，BPF 应当被更新为允许在程序本身中通过普通内存加载来完成这件事。目前在不
为验证器做更多工作的情况下还无法做到。bpf_cgroup_ancestor() 的用法如下：)

.. code-block:: c

	/**
	 * Simple tracepoint example that illustrates how a cgroup's
	 * ancestor can be accessed using bpf_cgroup_ancestor().
	 */
	SEC("tp_btf/cgroup_mkdir")
	int BPF_PROG(cgrp_ancestor_example, struct cgroup *cgrp, const char *path)
	{
		struct cgroup *parent;

		/* The parent cgroup resides at the level before the current cgroup's level. */
		parent = bpf_cgroup_ancestor(cgrp, cgrp->level - 1);
		if (!parent)
			return -ENOENT;

		bpf_printk("Parent id is %d", parent->self.id);

		/* Return the parent cgroup that was acquired above. */
		bpf_cgroup_release(parent);
		return 0;
	}

4.3 struct cpumask * kfuncs(4.3 struct cpumask * kfunc)
-------------------------------------------------------

BPF provides a set of kfuncs that can be used to query, allocate, mutate, and
destroy struct cpumask * objects. Please refer to :ref:`cpumasks-header-label`
for more details.
(BPF 提供了一组可用于查询、分配、修改和销毁 struct cpumask * 对象的 kfunc。更多
细节请参阅 :ref:`cpumasks-header-label`。)
