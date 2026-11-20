.. SPDX-License-Identifier: (LGPL-2.1 OR BSD-2-Clause)

API naming convention(API 命名约定)
===================================

libbpf API provides access to a few logically separated groups of
functions and types. Every group has its own naming convention
described here. It's recommended to follow these conventions whenever a
new function or type is added to keep libbpf API clean and consistent.
(libbpf API 提供对若干逻辑上相互独立的函数与类型组的访问。每个组都有自己的命名约定，在此描述。建议在向 libbpf API 添加新函数或类型时遵循这些约定，以保持 libbpf API 干净且一致。)

All types and functions provided by libbpf API should have one of the
following prefixes: ``bpf_``, ``btf_``, ``libbpf_``, ``btf_dump_``,
``ring_buffer_``, ``perf_buffer_``.
(libbpf API 提供的所有类型与函数都应带有以下前缀之一：``bpf_``、``btf_``、``libbpf_``、``btf_dump_``、``ring_buffer_``、``perf_buffer_`` 。)

System call wrappers(系统调用包装器)
------------------------------------

System call wrappers are simple wrappers for commands supported by
sys_bpf system call. These wrappers should go to ``bpf.h`` header file
and map one to one to corresponding commands.
(系统调用包装器是对 sys_bpf 系统调用所支持命令的简单包装。这些包装器应放入 ``bpf.h`` 头文件，并与对应命令一一对应。)

For example ``bpf_map_lookup_elem`` wraps ``BPF_MAP_LOOKUP_ELEM``
command of sys_bpf, ``bpf_prog_attach`` wraps ``BPF_PROG_ATTACH``, etc.
(例如 ``bpf_map_lookup_elem`` 包装 sys_bpf 的 ``BPF_MAP_LOOKUP_ELEM`` 命令，``bpf_prog_attach`` 包装 ``BPF_PROG_ATTACH``，等等。)

Objects(对象)
-------------

Another class of types and functions provided by libbpf API is "objects"
and functions to work with them. Objects are high-level abstractions
such as BPF program or BPF map. They're represented by corresponding
structures such as ``struct bpf_object``, ``struct bpf_program``,
``struct bpf_map``, etc.
(libbpf API 提供的另一类类型与函数是"对象"以及操作它们的函数。对象是 BPF 程序或 BPF 映射等高层抽象。它们由相应的结构体表示，如 ``struct bpf_object``、``struct bpf_program``、``struct bpf_map`` 等。)

Structures are forward declared and access to their fields should be
provided via corresponding getters and setters rather than directly.
(这些结构体采用前置声明，对其字段的访问应通过相应的 getter 与 setter 提供，而不是直接访问。)

These objects are associated with corresponding parts of ELF object that
contains compiled BPF programs.
(这些对象与包含已编译 BPF 程序的 ELF 对象的相应部分相关联。)

For example ``struct bpf_object`` represents ELF object itself created
from an ELF file or from a buffer, ``struct bpf_program`` represents a
program in ELF object and ``struct bpf_map`` is a map.
(例如，``struct bpf_object`` 表示从 ELF 文件或缓冲区创建的 ELF 对象本身，``struct bpf_program`` 表示 ELF 对象中的一个程序，``struct bpf_map`` 则是一个映射。)

Functions that work with an object have names built from object name,
double underscore and part that describes function purpose.
(操作对象的函数，其名称由对象名、双下划线以及描述函数用途的部分构成。)

For example ``bpf_object__open`` consists of the name of corresponding
object, ``bpf_object``, double underscore and ``open`` that defines the
purpose of the function to open ELF file and create ``bpf_object`` from
it.
(例如 ``bpf_object__open`` 由对应对象名 ``bpf_object``、双下划线和 ``open`` 组成，``open`` 表明该函数的用途是打开 ELF 文件并从中创建 ``bpf_object``。)

All objects and corresponding functions other than BTF related should go
to ``libbpf.h``. BTF types and functions should go to ``btf.h``.
(除 BTF 相关内容外，所有对象及对应函数都应放入 ``libbpf.h``。BTF 类型与函数应放入 ``btf.h``。)

Auxiliary functions(辅助函数)
-----------------------------

Auxiliary functions and types that don't fit well in any of categories
described above should have ``libbpf_`` prefix, e.g.
``libbpf_get_error`` or ``libbpf_prog_type_by_name``.
(不适合归入上述任何类别的辅助函数与类型应带有 ``libbpf_`` 前缀，例如 ``libbpf_get_error`` 或 ``libbpf_prog_type_by_name``。)

ABI(应用二进制接口)
-------------------

libbpf can be both linked statically or used as DSO. To avoid possible
conflicts with other libraries an application is linked with, all
non-static libbpf symbols should have one of the prefixes mentioned in
API documentation above. See API naming convention to choose the right
name for a new symbol.
(libbpf 既可被静态链接，也可用作 DSO。为避免与应用链接的其他库发生潜在冲突，所有非静态的 libbpf 符号都应带有上文 API 文档中提到的前缀之一。新符号的合适命名参见 API 命名约定。)

Symbol visibility(符号可见性)
-----------------------------

libbpf follow the model when all global symbols have visibility "hidden"
by default and to make a symbol visible it has to be explicitly
attributed with ``LIBBPF_API`` macro. For example:
(libbpf 遵循这样的模型：所有全局符号默认可见性为 "hidden"，要让某个符号可见，必须显式地用 ``LIBBPF_API`` 宏标注。例如：)

.. code-block:: c

        LIBBPF_API int bpf_prog_get_fd_by_id(__u32 id);

This prevents from accidentally exporting a symbol, that is not supposed
to be a part of ABI what, in turn, improves both libbpf developer- and
user-experiences.
(这可以防止意外导出本不应属于 ABI 的符号，从而同时改善 libbpf 开发者与用户的体验。)

ABI versioning(ABI 版本化)
--------------------------

To make future ABI extensions possible libbpf ABI is versioned.
Versioning is implemented by ``libbpf.map`` version script that is
passed to linker.
(为了使未来的 ABI 扩展成为可能，libbpf ABI 是带版本号的。版本化由传给链接器的 ``libbpf.map`` 版本脚本实现。)

Version name is ``LIBBPF_`` prefix + three-component numeric version,
starting from ``0.0.1``.
(版本名称为 ``LIBBPF_`` 前缀加三段式数字版本号，从 ``0.0.1`` 开始。)

Every time ABI is being changed, e.g. because a new symbol is added or
semantic of existing symbol is changed, ABI version should be bumped.
This bump in ABI version is at most once per kernel development cycle.
(每当 ABI 发生变化(例如新增了符号，或已有符号的语义发生改变)，都应递增 ABI 版本。这种 ABI 版本递增在每个内核开发周期内至多进行一次。)

For example, if current state of ``libbpf.map`` is:
(例如，若 ``libbpf.map`` 的当前状态为：)

.. code-block:: none

        LIBBPF_0.0.1 {
        	global:
                        bpf_func_a;
                        bpf_func_b;
        	local:
        		\*;
        };

, and a new symbol ``bpf_func_c`` is being introduced, then
``libbpf.map`` should be changed like this:
(，而现在要引入新符号 ``bpf_func_c``，则应把 ``libbpf.map`` 改成如下形式：)

.. code-block:: none

        LIBBPF_0.0.1 {
        	global:
                        bpf_func_a;
                        bpf_func_b;
        	local:
        		\*;
        };
        LIBBPF_0.0.2 {
                global:
                        bpf_func_c;
        } LIBBPF_0.0.1;

, where new version ``LIBBPF_0.0.2`` depends on the previous
``LIBBPF_0.0.1``.
(，其中新版本 ``LIBBPF_0.0.2`` 依赖于先前的 ``LIBBPF_0.0.1``。)

Format of version script and ways to handle ABI changes, including
incompatible ones, described in details in [1].
(版本脚本的格式以及处理 ABI 变更(包括不兼容变更)的方法在 [1] 中有详细描述。)

Stand-alone build(独立构建)
---------------------------

Under https://github.com/libbpf/libbpf there is a (semi-)automated
mirror of the mainline's version of libbpf for a stand-alone build.
(https://github.com/libbpf/libbpf 下有主线版本 libbpf 的(半)自动镜像，可用于独立构建。)

However, all changes to libbpf's code base must be upstreamed through
the mainline kernel tree.
(不过，对 libbpf 代码库的所有更改都必须通过主线内核树向上游提交。)


API documentation convention(API 文档约定)
==========================================

The libbpf API is documented via comments above definitions in
header files. These comments can be rendered by doxygen and sphinx
for well organized html output. This section describes the
convention in which these comments should be formatted.
(libbpf API 通过头文件中定义上方的注释来撰写文档。这些注释可由 doxygen 与 sphinx 渲染成组织良好的 html 输出。本节描述这些注释应采用的格式约定。)

Here is an example from btf.h:
(下面是来自 btf.h 的示例：)

.. code-block:: c

        /**
         * @brief **btf__new()** creates a new instance of a BTF object from the raw
         * bytes of an ELF's BTF section
         * @param data raw bytes
         * @param size number of bytes passed in `data`
         * @return new BTF object instance which has to be eventually freed with
         * **btf__free()**
         *
         * On error, error-code-encoded-as-pointer is returned, not a NULL. To extract
         * error code from such a pointer `libbpf_get_error()` should be used. If
         * `libbpf_set_strict_mode(LIBBPF_STRICT_CLEAN_PTRS)` is enabled, NULL is
         * returned on error instead. In both cases thread-local `errno` variable is
         * always set to error code as well.
         */

The comment must start with a block comment of the form '/\*\*'.
(注释必须以 '/\*\*' 形式的块注释开头。)

The documentation always starts with a @brief directive. This line is a short
description about this API. It starts with the name of the API, denoted in bold
like so: **api_name**. Please include an open and close parenthesis if this is a
function. Follow with the short description of the API. A longer form description
can be added below the last directive, at the bottom of the comment.
(文档总是以 @brief 指令开头。这一行是对该 API 的简短描述。它以 API 名称开头，名称用加粗表示，如 **api_name** 。如果是函数，请带上前后圆括号。随后是该 API 的简短描述。更长的描述可以加在最后一个指令之下、注释的底部。)

Parameters are denoted with the @param directive, there should be one for each
parameter. If this is a function with a non-void return, use the @return directive
to document it.
(参数用 @param 指令标注，每个参数对应一条。如果函数的返回类型不是 void，则使用 @return 指令来说明返回值。)

License(许可证)
---------------

libbpf is dual-licensed under LGPL 2.1 and BSD 2-Clause.
(libbpf 同时以 LGPL 2.1 与 BSD 2-Clause 双许可发布。)

Links(链接)
-----------

[1] https://www.akkadia.org/drepper/dsohowto.pdf
    (Chapter 3. Maintaining APIs and ABIs).
