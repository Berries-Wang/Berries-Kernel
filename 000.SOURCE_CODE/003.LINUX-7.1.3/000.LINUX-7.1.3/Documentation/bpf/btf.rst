===================================
BPF Type Format (BTF)(BPF 类型格式)
===================================

1. Introduction(1. 引言)
========================

BTF (BPF Type Format) is the metadata format which encodes the debug info
related to BPF program/map. The name BTF was used initially to describe data
types. The BTF was later extended to include function info for defined
subroutines, and line info for source/line information.
(BTF（BPF Type Format）是编码与 BPF 程序/映射相关的调试信息的元数据格式。BTF 这个名字最初用于描述数据类型。后来 BTF 被扩展为还包含已定义子例程的函数信息（function info），以及承载源码/行号信息的行信息（line info）。)

The debug info is used for map pretty print, function signature, etc. The
function signature enables better bpf program/function kernel symbol. The line
info helps generate source annotated translated byte code, jited code and
verifier log.
(这些调试信息用于映射美化打印（map pretty print）、函数签名等。函数签名带来了更好的 bpf 程序/函数内核符号。行信息则帮助生成带源码注解的翻译字节码、JIT 编译后的代码以及验证器日志。)

The BTF specification contains two parts,
(BTF 规范包含两个部分：)
  * BTF kernel API
    (BTF 内核 API)
  * BTF ELF file format
    (BTF ELF 文件格式)

The kernel API is the contract between user space and kernel. The kernel
verifies the BTF info before using it. The ELF file format is a user space
contract between ELF file and libbpf loader.
(内核 API 是用户空间与内核之间的契约。内核在使用 BTF 信息之前会先对其进行校验。ELF 文件格式则是 ELF 文件与 libbpf 加载器之间的用户空间契约。)

The type and string sections are part of the BTF kernel API, describing the
debug info (mostly types related) referenced by the bpf program. These two
sections are discussed in details in :ref:`BTF_Type_String`.
(类型节与字符串节是 BTF 内核 API 的一部分，描述 bpf 程序引用的调试信息（大多与类型相关）。这两节在 :ref:`BTF_Type_String` 中有详细讨论。)

.. _BTF_Type_String:

2. BTF Type and String Encoding(2. BTF 类型与字符串编码)
========================================================

The file ``include/uapi/linux/btf.h`` provides high-level definition of how
types/strings are encoded.
(文件 include/uapi/linux/btf.h 提供了类型/字符串如何编码的高层定义。)

The beginning of data blob must be::

    struct btf_header {
        __u16   magic;
        __u8    version;
        __u8    flags;
        __u32   hdr_len;

        /* All offsets are in bytes relative to the end of this header */
        __u32   type_off;       /* offset of type section       */
        __u32   type_len;       /* length of type section       */
        __u32   str_off;        /* offset of string section     */
        __u32   str_len;        /* length of string section     */
    };

(数据 blob 的开头必须是如下结构。)

The magic is ``0xeB9F``, which has different encoding for big and little
endian systems, and can be used to test whether BTF is generated for big- or
little-endian target. The ``btf_header`` is designed to be extensible with
``hdr_len`` equal to ``sizeof(struct btf_header)`` when a data blob is
generated.
(magic 是 0xeB9F，它在大端和小端系统上的编码不同，可用于测试 BTF 是为大端还是小端目标生成。btf_header 被设计为可扩展的，生成数据 blob 时 hdr_len 等于 sizeof(struct btf_header)。)

2.1 String Encoding(2.1 字符串编码)
-----------------------------------

The first string in the string section must be a null string. The rest of
string table is a concatenation of other null-terminated strings.
(字符串节中的第一个字符串必须是空字符串。字符串表的其余部分是其他以 null 结尾的字符串的串联。)

2.2 Type Encoding(2.2 类型编码)
-------------------------------

The type id ``0`` is reserved for ``void`` type. The type section is parsed
sequentially and type id is assigned to each recognized type starting from id
``1``. Currently, the following types are supported::

    #define BTF_KIND_INT            1       /* Integer      */
    #define BTF_KIND_PTR            2       /* Pointer      */
    #define BTF_KIND_ARRAY          3       /* Array        */
    #define BTF_KIND_STRUCT         4       /* Struct       */
    #define BTF_KIND_UNION          5       /* Union        */
    #define BTF_KIND_ENUM           6       /* Enumeration up to 32-bit values */
    #define BTF_KIND_FWD            7       /* Forward      */
    #define BTF_KIND_TYPEDEF        8       /* Typedef      */
    #define BTF_KIND_VOLATILE       9       /* Volatile     */
    #define BTF_KIND_CONST          10      /* Const        */
    #define BTF_KIND_RESTRICT       11      /* Restrict     */
    #define BTF_KIND_FUNC           12      /* Function     */
    #define BTF_KIND_FUNC_PROTO     13      /* Function Proto       */
    #define BTF_KIND_VAR            14      /* Variable     */
    #define BTF_KIND_DATASEC        15      /* Section      */
    #define BTF_KIND_FLOAT          16      /* Floating point       */
    #define BTF_KIND_DECL_TAG       17      /* Decl Tag     */
    #define BTF_KIND_TYPE_TAG       18      /* Type Tag     */
    #define BTF_KIND_ENUM64         19      /* Enumeration up to 64-bit values */

(类型 id 0 保留给 void 类型。类型节按顺序解析，从 id 1 开始为每个识别出的类型分配类型 id。当前支持以下类型。)

Note that the type section encodes debug info, not just pure types.
``BTF_KIND_FUNC`` is not a type, and it represents a defined subprogram.
(注意，类型节编码的是调试信息，而不仅仅是纯类型。BTF_KIND_FUNC 不是类型，它表示一个已定义的子程序。)

Each type contains the following common data::

    struct btf_type {
        __u32 name_off;
        /* "info" bits arrangement
         * bits  0-15: vlen (e.g. # of struct's members)
         * bits 16-23: unused
         * bits 24-28: kind (e.g. int, ptr, array...etc)
         * bits 29-30: unused
         * bit     31: kind_flag, currently used by
         *             struct, union, enum, fwd, enum64,
         *             decl_tag and type_tag
         */
        __u32 info;
        /* "size" is used by INT, ENUM, STRUCT, UNION and ENUM64.
         * "size" tells the size of the type it is describing.
         *
         * "type" is used by PTR, TYPEDEF, VOLATILE, CONST, RESTRICT,
         * FUNC, FUNC_PROTO, DECL_TAG and TYPE_TAG.
         * "type" is a type_id referring to another type.
         */
        union {
                __u32 size;
                __u32 type;
        };
    };

(每个类型都包含以下公共数据。)

For certain kinds, the common data are followed by kind-specific data. The
``name_off`` in ``struct btf_type`` specifies the offset in the string table.
The following sections detail encoding of each kind.
(对于某些 kind，公共数据之后还会跟随该 kind 特有的数据。struct btf_type 中的 name_off 指定其在字符串表中的偏移。以下各节详细说明每个 kind 的编码。)

2.2.1 BTF_KIND_INT(2.2.1 整数)
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

``struct btf_type`` encoding requirement:
 * ``name_off``: any valid offset
 * ``info.kind_flag``: 0
 * ``info.kind``: BTF_KIND_INT
 * ``info.vlen``: 0
 * ``size``: the size of the int type in bytes.
   (struct btf_type 编码要求：name_off 为任意有效偏移；info.kind_flag 为 0；info.kind 为 BTF_KIND_INT；info.vlen 为 0；size 为 int 类型以字节计的大小。)

``btf_type`` is followed by a ``u32`` with the following bits arrangement::

  #define BTF_INT_ENCODING(VAL)   (((VAL) & 0x0f000000) >> 24)
  #define BTF_INT_OFFSET(VAL)     (((VAL) & 0x00ff0000) >> 16)
  #define BTF_INT_BITS(VAL)       ((VAL)  & 0x000000ff)

(btf_type 之后跟随一个 u32，其位域布局如下。)

The ``BTF_INT_ENCODING`` has the following attributes::

  #define BTF_INT_SIGNED  (1 << 0)
  #define BTF_INT_CHAR    (1 << 1)
  #define BTF_INT_BOOL    (1 << 2)

(BTF_INT_ENCODING 具有如下属性。)

The ``BTF_INT_ENCODING()`` provides extra information: signedness, char, or
bool, for the int type. The char and bool encoding are mostly useful for
pretty print. At most one encoding can be specified for the int type.
(BTF_INT_ENCODING() 为 int 类型提供额外信息：符号性（signedness）、char 或 bool。char 与 bool 编码主要用于美化打印。对 int 类型而言，最多只能指定一种编码。)

The ``BTF_INT_BITS()`` specifies the number of actual bits held by this int
type. For example, a 4-bit bitfield encodes ``BTF_INT_BITS()`` equals to 4.
The ``btf_type.size * 8`` must be equal to or greater than ``BTF_INT_BITS()``
for the type. The maximum value of ``BTF_INT_BITS()`` is 128.
(BTF_INT_BITS() 指定该 int 类型实际持有的位数。例如，一个 4 位位域会把 BTF_INT_BITS() 编码为 4。对该类型而言，btf_type.size * 8 必须大于或等于 BTF_INT_BITS()。BTF_INT_BITS() 的最大值是 128。)

The ``BTF_INT_OFFSET()`` specifies the starting bit offset to calculate values
for this int. For example, a bitfield struct member has:
(BTF_INT_OFFSET() 指定为该 int 计算取值时的起始位偏移。例如，一个位域结构体成员具有：)

 * btf member bit offset 100 from the start of the structure,
   (btf 成员的位偏移为 100（自结构体起始处计），)
 * btf member pointing to an int type,
   (btf 成员指向一个 int 类型，)
 * the int type has ``BTF_INT_OFFSET() = 2`` and ``BTF_INT_BITS() = 4``
   (该 int 类型具有 BTF_INT_OFFSET() = 2 与 BTF_INT_BITS() = 4)

Then in the struct memory layout, this member will occupy ``4`` bits starting
from bits ``100 + 2 = 102``.
(于是在结构体的内存布局中，该成员将从位 100 + 2 = 102 起占据 4 位。)

Alternatively, the bitfield struct member can be the following to access the
same bits as the above:
(或者，位域结构体成员也可以如下设置，以访问与上面相同的位：)

 * btf member bit offset 102,
   (btf 成员位偏移为 102，)
 * btf member pointing to an int type,
   (btf 成员指向一个 int 类型，)
 * the int type has ``BTF_INT_OFFSET() = 0`` and ``BTF_INT_BITS() = 4``
   (该 int 类型具有 BTF_INT_OFFSET() = 0 与 BTF_INT_BITS() = 4)

The original intention of ``BTF_INT_OFFSET()`` is to provide flexibility of
bitfield encoding. Currently, both llvm and pahole generate
``BTF_INT_OFFSET() = 0`` for all int types.
(BTF_INT_OFFSET() 的最初意图是提供位域编码的灵活性。目前，llvm 和 pahole 对所有 int 类型都生成 BTF_INT_OFFSET() = 0。)

2.2.2 BTF_KIND_PTR(2.2.2 指针)
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

``struct btf_type`` encoding requirement:
  * ``name_off``: 0
  * ``info.kind_flag``: 0
  * ``info.kind``: BTF_KIND_PTR
  * ``info.vlen``: 0
  * ``type``: the pointee type of the pointer
    (struct btf_type 编码要求：name_off 为 0；info.kind_flag 为 0；info.kind 为 BTF_KIND_PTR；info.vlen 为 0；type 为指针所指向的类型。)

No additional type data follow ``btf_type``.
(btf_type 之后没有额外的类型数据。)

2.2.3 BTF_KIND_ARRAY(2.2.3 数组)
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

``struct btf_type`` encoding requirement:
  * ``name_off``: 0
  * ``info.kind_flag``: 0
  * ``info.kind``: BTF_KIND_ARRAY
  * ``info.vlen``: 0
  * ``size/type``: 0, not used
    (struct btf_type 编码要求：name_off 为 0；info.kind_flag 为 0；info.kind 为 BTF_KIND_ARRAY；info.vlen 为 0；size/type 为 0，不使用。)

``btf_type`` is followed by one ``struct btf_array``::

    struct btf_array {
        __u32   type;
        __u32   index_type;
        __u32   nelems;
    };

(btf_type 之后跟随一个 struct btf_array。)

The ``struct btf_array`` encoding:
(struct btf_array 的编码：)
  * ``type``: the element type
    (type：元素类型。)
  * ``index_type``: the index type
    (index_type：索引类型。)
  * ``nelems``: the number of elements for this array (``0`` is also allowed).
    (nelems：该数组的元素数量（也允许为 0）。)

The ``index_type`` can be any regular int type (``u8``, ``u16``, ``u32``,
``u64``, ``unsigned __int128``). The original design of including
``index_type`` follows DWARF, which has an ``index_type`` for its array type.
Currently in BTF, beyond type verification, the ``index_type`` is not used.
(index_type 可以是任何常规 int 类型（u8、u16、u32、u64、unsigned __int128）。包含 index_type 的最初设计沿袭自 DWARF，后者的数组类型也有一个 index_type。目前在 BTF 中，除类型校验外，index_type 并未被使用。)

The ``struct btf_array`` allows chaining through element type to represent
multidimensional arrays. For example, for ``int a[5][6]``, the following type
information illustrates the chaining:
(struct btf_array 允许通过元素类型链接来表示多维数组。例如，对于 int a[5][6]，下面的类型信息展示了这种链接：)

  * [1]: int
    ([1]：int)
  * [2]: array, ``btf_array.type = [1]``, ``btf_array.nelems = 6``
    ([2]：array，btf_array.type = [1]，btf_array.nelems = 6)
  * [3]: array, ``btf_array.type = [2]``, ``btf_array.nelems = 5``
    ([3]：array，btf_array.type = [2]，btf_array.nelems = 5)

Currently, both pahole and llvm collapse multidimensional array into
one-dimensional array, e.g., for ``a[5][6]``, the ``btf_array.nelems`` is
equal to ``30``. This is because the original use case is map pretty print
where the whole array is dumped out so one-dimensional array is enough. As
more BTF usage is explored, pahole and llvm can be changed to generate proper
chained representation for multidimensional arrays.
(目前，pahole 和 llvm 都会把多维数组折叠成一维数组，例如对于 a[5][6]，btf_array.nelems 等于 30。这是因为最初的用例是映射美化打印——整个数组都会被转储出来，一维数组就够用了。随着更多 BTF 用法被发掘，pahole 和 llvm 可以改为为多维数组生成正确的链式表示。)

2.2.4 BTF_KIND_STRUCT(2.2.4 结构体)
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
2.2.5 BTF_KIND_UNION(2.2.5 联合体)
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

``struct btf_type`` encoding requirement:
  * ``name_off``: 0 or offset to a valid C identifier
  * ``info.kind_flag``: 0 or 1
  * ``info.kind``: BTF_KIND_STRUCT or BTF_KIND_UNION
  * ``info.vlen``: the number of struct/union members
  * ``info.size``: the size of the struct/union in bytes
    (struct btf_type 编码要求：name_off 为 0 或指向有效 C 标识符的偏移；info.kind_flag 为 0 或 1；info.kind 为 BTF_KIND_STRUCT 或 BTF_KIND_UNION；info.vlen 为 struct/union 成员数量；info.size 为 struct/union 以字节计的大小。)

``btf_type`` is followed by ``info.vlen`` number of ``struct btf_member``.::

    struct btf_member {
        __u32   name_off;
        __u32   type;
        __u32   offset;
    };

(btf_type 之后跟随 info.vlen 个 struct btf_member。)

``struct btf_member`` encoding:
(struct btf_member 的编码：)
  * ``name_off``: offset to a valid C identifier
    (name_off：指向有效 C 标识符的偏移。)
  * ``type``: the member type
    (type：成员类型。)
  * ``offset``: <see below>
    (offset：<见下文>。)

If the type info ``kind_flag`` is not set, the offset contains only bit offset
of the member. Note that the base type of the bitfield can only be int or enum
type. If the bitfield size is 32, the base type can be either int or enum
type. If the bitfield size is not 32, the base type must be int, and int type
``BTF_INT_BITS()`` encodes the bitfield size.
(如果类型信息中的 kind_flag 未设置，offset 只包含成员的位偏移。注意，位域的基类型只能是 int 或 enum 类型。如果位域大小为 32，基类型可以是 int 或 enum；如果位域大小不是 32，基类型必须是 int，并且 int 类型的 BTF_INT_BITS() 编码位域大小。)

If the ``kind_flag`` is set, the ``btf_member.offset`` contains both member
bitfield size and bit offset. The bitfield size and bit offset are calculated
as below.::

  #define BTF_MEMBER_BITFIELD_SIZE(val)   ((val) >> 24)
  #define BTF_MEMBER_BIT_OFFSET(val)      ((val) & 0xffffff)

(如果设置了 kind_flag，btf_member.offset 同时包含成员位域大小与位偏移。位域大小与位偏移按如下方式计算。)

In this case, if the base type is an int type, it must be a regular int type:
(在这种情况下，如果基类型是 int 类型，它必须是常规 int 类型：)

  * ``BTF_INT_OFFSET()`` must be 0.
    (BTF_INT_OFFSET() 必须为 0。)
  * ``BTF_INT_BITS()`` must be equal to ``{1,2,4,8,16} * 8``.
    (BTF_INT_BITS() 必须等于 {1,2,4,8,16} * 8。)

Commit 9d5f9f701b18 introduced ``kind_flag`` and explains why both modes
exist.
(Commit 9d5f9f701b18 引入了 kind_flag，并解释了为什么存在这两种模式。)

2.2.6 BTF_KIND_ENUM(2.2.6 枚举)
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

``struct btf_type`` encoding requirement:
  * ``name_off``: 0 or offset to a valid C identifier
  * ``info.kind_flag``: 0 for unsigned, 1 for signed
  * ``info.kind``: BTF_KIND_ENUM
  * ``info.vlen``: number of enum values
  * ``size``: 1/2/4/8
    (struct btf_type 编码要求：name_off 为 0 或指向有效 C 标识符的偏移；info.kind_flag 为 0 表示无符号、1 表示有符号；info.kind 为 BTF_KIND_ENUM；info.vlen 为枚举值数量；size 为 1/2/4/8。)

``btf_type`` is followed by ``info.vlen`` number of ``struct btf_enum``.::

    struct btf_enum {
        __u32   name_off;
        __s32   val;
    };

(btf_type 之后跟随 info.vlen 个 struct btf_enum。)

The ``btf_enum`` encoding:
(btf_enum 的编码：)
  * ``name_off``: offset to a valid C identifier
    (name_off：指向有效 C 标识符的偏移。)
  * ``val``: any value
    (val：任意值。)

If the original enum value is signed and the size is less than 4,
that value will be sign extended into 4 bytes. If the size is 8,
the value will be truncated into 4 bytes.
(如果原始枚举值是有符号的且大小小于 4，该值会被符号扩展到 4 字节。如果大小为 8，该值会被截断成 4 字节。)

2.2.7 BTF_KIND_FWD(2.2.7 前置声明)
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

``struct btf_type`` encoding requirement:
  * ``name_off``: offset to a valid C identifier
  * ``info.kind_flag``: 0 for struct, 1 for union
  * ``info.kind``: BTF_KIND_FWD
  * ``info.vlen``: 0
  * ``type``: 0
    (struct btf_type 编码要求：name_off 为指向有效 C 标识符的偏移；info.kind_flag 为 0 表示 struct、1 表示 union；info.kind 为 BTF_KIND_FWD；info.vlen 为 0；type 为 0。)

No additional type data follow ``btf_type``.
(btf_type 之后没有额外的类型数据。)

2.2.8 BTF_KIND_TYPEDEF(2.2.8 类型定义)
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

``struct btf_type`` encoding requirement:
  * ``name_off``: offset to a valid C identifier
  * ``info.kind_flag``: 0
  * ``info.kind``: BTF_KIND_TYPEDEF
  * ``info.vlen``: 0
  * ``type``: the type which can be referred by name at ``name_off``
    (struct btf_type 编码要求：name_off 为指向有效 C 标识符的偏移；info.kind_flag 为 0；info.kind 为 BTF_KIND_TYPEDEF；info.vlen 为 0；type 为可在 name_off 处按名字引用的类型。)

No additional type data follow ``btf_type``.
(btf_type 之后没有额外的类型数据。)

2.2.9 BTF_KIND_VOLATILE(2.2.9 volatile 限定)
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

``struct btf_type`` encoding requirement:
  * ``name_off``: 0
  * ``info.kind_flag``: 0
  * ``info.kind``: BTF_KIND_VOLATILE
  * ``info.vlen``: 0
  * ``type``: the type with ``volatile`` qualifier
    (struct btf_type 编码要求：name_off 为 0；info.kind_flag 为 0；info.kind 为 BTF_KIND_VOLATILE；info.vlen 为 0；type 为带 volatile 限定符的类型。)

No additional type data follow ``btf_type``.
(btf_type 之后没有额外的类型数据。)

2.2.10 BTF_KIND_CONST(2.2.10 const 限定)
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

``struct btf_type`` encoding requirement:
  * ``name_off``: 0
  * ``info.kind_flag``: 0
  * ``info.kind``: BTF_KIND_CONST
  * ``info.vlen``: 0
  * ``type``: the type with ``const`` qualifier
    (struct btf_type 编码要求：name_off 为 0；info.kind_flag 为 0；info.kind 为 BTF_KIND_CONST；info.vlen 为 0；type 为带 const 限定符的类型。)

No additional type data follow ``btf_type``.
(btf_type 之后没有额外的类型数据。)

2.2.11 BTF_KIND_RESTRICT(2.2.11 restrict 限定)
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

``struct btf_type`` encoding requirement:
  * ``name_off``: 0
  * ``info.kind_flag``: 0
  * ``info.kind``: BTF_KIND_RESTRICT
  * ``info.vlen``: 0
  * ``type``: the type with ``restrict`` qualifier
    (struct btf_type 编码要求：name_off 为 0；info.kind_flag 为 0；info.kind 为 BTF_KIND_RESTRICT；info.vlen 为 0；type 为带 restrict 限定符的类型。)

No additional type data follow ``btf_type``.
(btf_type 之后没有额外的类型数据。)

2.2.12 BTF_KIND_FUNC(2.2.12 函数)
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

``struct btf_type`` encoding requirement:
  * ``name_off``: offset to a valid C identifier
  * ``info.kind_flag``: 0
  * ``info.kind``: BTF_KIND_FUNC
  * ``info.vlen``: linkage information (BTF_FUNC_STATIC, BTF_FUNC_GLOBAL
                   or BTF_FUNC_EXTERN - see :ref:`BTF_Function_Linkage_Constants`)
  * ``type``: a BTF_KIND_FUNC_PROTO type
    (struct btf_type 编码要求：name_off 为指向有效 C 标识符的偏移；info.kind_flag 为 0；info.kind 为 BTF_KIND_FUNC；info.vlen 为链接信息（BTF_FUNC_STATIC、BTF_FUNC_GLOBAL 或 BTF_FUNC_EXTERN——参见 :ref:`BTF_Function_Linkage_Constants`）；type 为一个 BTF_KIND_FUNC_PROTO 类型。)

No additional type data follow ``btf_type``.
(btf_type 之后没有额外的类型数据。)

A BTF_KIND_FUNC defines not a type, but a subprogram (function) whose
signature is defined by ``type``. The subprogram is thus an instance of that
type. The BTF_KIND_FUNC may in turn be referenced by a func_info in the
:ref:`BTF_Ext_Section` (ELF) or in the arguments to :ref:`BPF_Prog_Load`
(ABI).
(BTF_KIND_FUNC 定义的不是类型，而是一个子程序（函数），其签名由 type 定义。因此，该子程序就是该类型的一个实例。BTF_KIND_FUNC 反过来可以被 :ref:`BTF_Ext_Section`（ELF）中的 func_info 或 :ref:`BPF_Prog_Load`（ABI）的参数引用。)

Currently, only linkage values of BTF_FUNC_STATIC and BTF_FUNC_GLOBAL are
supported in the kernel.
(目前，内核只支持 BTF_FUNC_STATIC 与 BTF_FUNC_GLOBAL 两种链接值。)

2.2.13 BTF_KIND_FUNC_PROTO(2.2.13 函数原型)
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

``struct btf_type`` encoding requirement:
  * ``name_off``: 0
  * ``info.kind_flag``: 0
  * ``info.kind``: BTF_KIND_FUNC_PROTO
  * ``info.vlen``: # of parameters
  * ``type``: the return type
    (struct btf_type 编码要求：name_off 为 0；info.kind_flag 为 0；info.kind 为 BTF_KIND_FUNC_PROTO；info.vlen 为参数个数；type 为返回类型。)

``btf_type`` is followed by ``info.vlen`` number of ``struct btf_param``.::

    struct btf_param {
        __u32   name_off;
        __u32   type;
    };

(btf_type 之后跟随 info.vlen 个 struct btf_param。)

If a BTF_KIND_FUNC_PROTO type is referred by a BTF_KIND_FUNC type, then
``btf_param.name_off`` must point to a valid C identifier except for the
possible last argument representing the variable argument. The btf_param.type
refers to parameter type.
(如果 BTF_KIND_FUNC_PROTO 类型被某个 BTF_KIND_FUNC 类型引用，那么 btf_param.name_off 必须指向有效的 C 标识符，唯一的例外是可能存在的、表示可变参数的最后一个参数。btf_param.type 引用的是参数类型。)

If the function has variable arguments, the last parameter is encoded with
``name_off = 0`` and ``type = 0``.
(如果函数带有可变参数，最后一个参数以 name_off = 0 和 type = 0 编码。)

2.2.14 BTF_KIND_VAR(2.2.14 变量)
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

``struct btf_type`` encoding requirement:
  * ``name_off``: offset to a valid C identifier
  * ``info.kind_flag``: 0
  * ``info.kind``: BTF_KIND_VAR
  * ``info.vlen``: 0
  * ``type``: the type of the variable
    (struct btf_type 编码要求：name_off 为指向有效 C 标识符的偏移；info.kind_flag 为 0；info.kind 为 BTF_KIND_VAR；info.vlen 为 0；type 为变量的类型。)

``btf_type`` is followed by a single ``struct btf_variable`` with the
following data::

    struct btf_var {
        __u32   linkage;
    };

(btf_type 之后跟随单个 struct btf_variable，其数据如下。)

``btf_var.linkage`` may take the values: BTF_VAR_STATIC, BTF_VAR_GLOBAL_ALLOCATED or BTF_VAR_GLOBAL_EXTERN -
see :ref:`BTF_Var_Linkage_Constants`.
(btf_var.linkage 可以取以下值：BTF_VAR_STATIC、BTF_VAR_GLOBAL_ALLOCATED 或 BTF_VAR_GLOBAL_EXTERN——参见 :ref:`BTF_Var_Linkage_Constants`。)

Not all type of global variables are supported by LLVM at this point.
The following is currently available:
(目前 LLVM 并不支持所有类型的全局变量。当前可用的有：)

  * static variables with or without section attributes
    (带或不带 section 属性的静态变量。)
  * global variables with section attributes
    (带 section 属性的全局变量。)

The latter is for future extraction of map key/value type id's from a
map definition.
(后者用于将来从映射定义中提取映射 key/value 的 type id。)

2.2.15 BTF_KIND_DATASEC(2.2.15 数据段)
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

``struct btf_type`` encoding requirement:
  * ``name_off``: offset to a valid name associated with a variable or
                  one of .data/.bss/.rodata
  * ``info.kind_flag``: 0
  * ``info.kind``: BTF_KIND_DATASEC
  * ``info.vlen``: # of variables
  * ``size``: total section size in bytes (0 at compilation time, patched
              to actual size by BPF loaders such as libbpf)
    (struct btf_type 编码要求：name_off 为指向某个有效名称的偏移，该名称与一个变量或 .data/.bss/.rodata 之一相关联；info.kind_flag 为 0；info.kind 为 BTF_KIND_DATASEC；info.vlen 为变量个数；size 为节的总大小（字节）（编译时为 0，由 libbpf 等 BPF 加载器修补为实际大小）。)

``btf_type`` is followed by ``info.vlen`` number of ``struct btf_var_secinfo``.::

    struct btf_var_secinfo {
        __u32   type;
        __u32   offset;
        __u32   size;
    };

(btf_type 之后跟随 info.vlen 个 struct btf_var_secinfo。)

``struct btf_var_secinfo`` encoding:
(struct btf_var_secinfo 的编码：)
  * ``type``: the type of the BTF_KIND_VAR variable
    (type：BTF_KIND_VAR 变量的类型。)
  * ``offset``: the in-section offset of the variable
    (offset：变量在节内的偏移。)
  * ``size``: the size of the variable in bytes
    (size：变量以字节计的大小。)

2.2.16 BTF_KIND_FLOAT(2.2.16 浮点数)
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

``struct btf_type`` encoding requirement:
 * ``name_off``: any valid offset
 * ``info.kind_flag``: 0
 * ``info.kind``: BTF_KIND_FLOAT
 * ``info.vlen``: 0
 * ``size``: the size of the float type in bytes: 2, 4, 8, 12 or 16.
   (struct btf_type 编码要求：name_off 为任意有效偏移；info.kind_flag 为 0；info.kind 为 BTF_KIND_FLOAT；info.vlen 为 0；size 为 float 类型以字节计的大小：2、4、8、12 或 16。)

No additional type data follow ``btf_type``.
(btf_type 之后没有额外的类型数据。)

2.2.17 BTF_KIND_DECL_TAG(2.2.17 声明标签)
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

``struct btf_type`` encoding requirement:
 * ``name_off``: offset to a non-empty string
 * ``info.kind_flag``: 0 or 1
 * ``info.kind``: BTF_KIND_DECL_TAG
 * ``info.vlen``: 0
 * ``type``: ``struct``, ``union``, ``func``, ``var`` or ``typedef``
   (struct btf_type 编码要求：name_off 为指向非空字符串的偏移；info.kind_flag 为 0 或 1；info.kind 为 BTF_KIND_DECL_TAG；info.vlen 为 0；type 为 struct、union、func、var 或 typedef。)

``btf_type`` is followed by ``struct btf_decl_tag``.::

    struct btf_decl_tag {
        __u32   component_idx;
    };

(btf_type 之后跟随 struct btf_decl_tag。)

The ``type`` should be ``struct``, ``union``, ``func``, ``var`` or ``typedef``.
For ``var`` or ``typedef`` type, ``btf_decl_tag.component_idx`` must be ``-1``.
For the other three types, if the btf_decl_tag attribute is
applied to the ``struct``, ``union`` or ``func`` itself,
``btf_decl_tag.component_idx`` must be ``-1``. Otherwise,
the attribute is applied to a ``struct``/``union`` member or
a ``func`` argument, and ``btf_decl_tag.component_idx`` should be a
valid index (starting from 0) pointing to a member or an argument.
(type 应为 struct、union、func、var 或 typedef。对于 var 或 typedef 类型，btf_decl_tag.component_idx 必须为 -1。对于其余三种类型，如果 btf_decl_tag 属性施加于 struct、union 或 func 本身，btf_decl_tag.component_idx 必须为 -1；否则，属性施加于某个 struct/union 成员或 func 参数，btf_decl_tag.component_idx 应是一个从 0 开始的有效索引，指向某个成员或参数。)

If ``info.kind_flag`` is 0, then this is a normal decl tag, and the
``name_off`` encodes btf_decl_tag attribute string.
(如果 info.kind_flag 为 0，则这是一个普通的 decl tag，name_off 编码 btf_decl_tag 属性字符串。)

If ``info.kind_flag`` is 1, then the decl tag represents an arbitrary
__attribute__. In this case, ``name_off`` encodes a string
representing the attribute-list of the attribute specifier. For
example, for an ``__attribute__((aligned(4)))`` the string's contents
is ``aligned(4)``.
(如果 info.kind_flag 为 1，则该 decl tag 表示任意的 __attribute__。此时，name_off 编码一个字符串，表示属性说明符的属性列表。例如，对于 __attribute__((aligned(4)))，字符串内容就是 aligned(4)。)

2.2.18 BTF_KIND_TYPE_TAG(2.2.18 类型标签)
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

``struct btf_type`` encoding requirement:
 * ``name_off``: offset to a non-empty string
 * ``info.kind_flag``: 0 or 1
 * ``info.kind``: BTF_KIND_TYPE_TAG
 * ``info.vlen``: 0
 * ``type``: the type with ``btf_type_tag`` attribute
   (struct btf_type 编码要求：name_off 为指向非空字符串的偏移；info.kind_flag 为 0 或 1；info.kind 为 BTF_KIND_TYPE_TAG；info.vlen 为 0；type 为带 btf_type_tag 属性的类型。)

Currently, ``BTF_KIND_TYPE_TAG`` is only emitted for pointer types.
It has the following btf type chain:
::

  ptr -> [type_tag]*
      -> [const | volatile | restrict | typedef]*
      -> base_type

(目前，BTF_KIND_TYPE_TAG 只为指针类型生成。它具有如下 btf 类型链。)

Basically, a pointer type points to zero or more
type_tag, then zero or more const/volatile/restrict/typedef
and finally the base type. The base type is one of
int, ptr, array, struct, union, enum, func_proto and float types.
(基本上，指针类型指向零个或多个 type_tag，然后是零个或多个 const/volatile/restrict/typedef，最后是基类型。基类型是 int、ptr、array、struct、union、enum、func_proto 和 float 类型之一。)

Similarly to decl tags, if the ``info.kind_flag`` is 0, then this is a
normal type tag, and the ``name_off`` encodes btf_type_tag attribute
string.
(与 decl tag 类似，如果 info.kind_flag 为 0，则这是一个普通的 type tag，name_off 编码 btf_type_tag 属性字符串。)

If ``info.kind_flag`` is 1, then the type tag represents an arbitrary
__attribute__, and the ``name_off`` encodes a string representing the
attribute-list of the attribute specifier.
(如果 info.kind_flag 为 1，则该 type tag 表示任意的 __attribute__，name_off 编码一个表示属性说明符属性列表的字符串。)

2.2.19 BTF_KIND_ENUM64(2.2.19 64 位枚举)
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

``struct btf_type`` encoding requirement:
  * ``name_off``: 0 or offset to a valid C identifier
  * ``info.kind_flag``: 0 for unsigned, 1 for signed
  * ``info.kind``: BTF_KIND_ENUM64
  * ``info.vlen``: number of enum values
  * ``size``: 1/2/4/8
    (struct btf_type 编码要求：name_off 为 0 或指向有效 C 标识符的偏移；info.kind_flag 为 0 表示无符号、1 表示有符号；info.kind 为 BTF_KIND_ENUM64；info.vlen 为枚举值数量；size 为 1/2/4/8。)

``btf_type`` is followed by ``info.vlen`` number of ``struct btf_enum64``.::

    struct btf_enum64 {
        __u32   name_off;
        __u32   val_lo32;
        __u32   val_hi32;
    };

(btf_type 之后跟随 info.vlen 个 struct btf_enum64。)

The ``btf_enum64`` encoding:
(btf_enum64 的编码：)
  * ``name_off``: offset to a valid C identifier
    (name_off：指向有效 C 标识符的偏移。)
  * ``val_lo32``: lower 32-bit value for a 64-bit value
    (val_lo32：64 位值的低 32 位。)
  * ``val_hi32``: high 32-bit value for a 64-bit value
    (val_hi32：64 位值的高 32 位。)

If the original enum value is signed and the size is less than 8,
that value will be sign extended into 8 bytes.
(如果原始枚举值是有符号的且大小小于 8，该值会被符号扩展到 8 字节。)

2.3 Constant Values(2.3 常量值)
-------------------------------

.. _BTF_Function_Linkage_Constants:

2.3.1 Function Linkage Constant Values(2.3.1 函数链接常量值)
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
.. table:: Function Linkage Values and Meanings

  ===================  =====  ===========
  kind                 value  description
  (类别 / 值 / 说明)
  ===================  =====  ===========
  ``BTF_FUNC_STATIC``  0x0    definition of subprogram not visible outside containing compilation unit (子程序定义，在包含它的编译单元之外不可见)
  ``BTF_FUNC_GLOBAL``  0x1    definition of subprogram visible outside containing compilation unit (子程序定义，在包含它的编译单元之外可见)
  ``BTF_FUNC_EXTERN``  0x2    declaration of a subprogram whose definition is outside the containing compilation unit (子程序声明，其定义位于包含它的编译单元之外)
  ===================  =====  ===========


.. _BTF_Var_Linkage_Constants:

2.3.2 Variable Linkage Constant Values(2.3.2 变量链接常量值)
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
.. table:: Variable Linkage Values and Meanings

  ============================  =====  ===========
  kind                          value  description
  (类别 / 值 / 说明)
  ============================  =====  ===========
  ``BTF_VAR_STATIC``            0x0    definition of global variable not visible outside containing compilation unit (全局变量定义，在包含它的编译单元之外不可见)
  ``BTF_VAR_GLOBAL_ALLOCATED``  0x1    definition of global variable visible outside containing compilation unit (全局变量定义，在包含它的编译单元之外可见)
  ``BTF_VAR_GLOBAL_EXTERN``     0x2    declaration of global variable whose definition is outside the containing compilation unit (全局变量声明，其定义位于包含它的编译单元之外)
  ============================  =====  ===========

3. BTF Kernel API(3. BTF 内核 API)
==================================

The following bpf syscall command involves BTF:
(以下 bpf 系统调用命令会涉及 BTF：)
   * BPF_BTF_LOAD: load a blob of BTF data into kernel
     (BPF_BTF_LOAD：把一块 BTF 数据加载进内核)
   * BPF_MAP_CREATE: map creation with btf key and value type info.
     (BPF_MAP_CREATE：创建映射时附带 btf key 和 value 类型信息。)
   * BPF_PROG_LOAD: prog load with btf function and line info.
     (BPF_PROG_LOAD：加载程序时附带 btf 函数与行信息。)
   * BPF_BTF_GET_FD_BY_ID: get a btf fd
     (BPF_BTF_GET_FD_BY_ID：获取一个 btf fd)
   * BPF_OBJ_GET_INFO_BY_FD: btf, func_info, line_info
     and other btf related info are returned.
     (BPF_OBJ_GET_INFO_BY_FD：返回 btf、func_info、line_info 以及其他与 btf 相关的信息。)

The workflow typically looks like:
::

  Application:
      BPF_BTF_LOAD
          |
          v
      BPF_MAP_CREATE and BPF_PROG_LOAD
          |
          V
      ......

  Introspection tool:
      ......
      BPF_{PROG,MAP}_GET_NEXT_ID (get prog/map id's)
          |
          V
      BPF_{PROG,MAP}_GET_FD_BY_ID (get a prog/map fd)
          |
          V
      BPF_OBJ_GET_INFO_BY_FD (get bpf_prog_info/bpf_map_info with btf_id)
          |                                     |
          V                                     |
      BPF_BTF_GET_FD_BY_ID (get btf_fd)         |
          |                                     |
          V                                     |
      BPF_OBJ_GET_INFO_BY_FD (get btf)          |
          |                                     |
          V                                     V
      pretty print types, dump func signatures and line info, etc.

(典型的工作流大致如下。)


3.1 BPF_BTF_LOAD(3.1 加载 BTF 数据)
-----------------------------------

Load a blob of BTF data into kernel. A blob of data, described in
:ref:`BTF_Type_String`, can be directly loaded into the kernel. A ``btf_fd``
is returned to a userspace.
(把一块 BTF 数据加载进内核。:ref:`BTF_Type_String` 中描述的数据 blob 可以直接加载进内核。会向用户空间返回一个 btf_fd。)

3.2 BPF_MAP_CREATE(3.2 创建映射)
--------------------------------

A map can be created with ``btf_fd`` and specified key/value type id.::

    __u32   btf_fd;         /* fd pointing to a BTF type data */
    __u32   btf_key_type_id;        /* BTF type_id of the key */
    __u32   btf_value_type_id;      /* BTF type_id of the value */

(可以带着 btf_fd 以及指定的 key/value type id 来创建映射。)

In libbpf, the map can be defined with extra annotation like below:
::

    struct {
        __uint(type, BPF_MAP_TYPE_ARRAY);
        __type(key, int);
        __type(value, struct ipv_counts);
        __uint(max_entries, 4);
    } btf_map SEC(".maps");

(在 libbpf 中，映射可以用如下所示的额外注解来定义。)

During ELF parsing, libbpf is able to extract key/value type_id's and assign
them to BPF_MAP_CREATE attributes automatically.
(在 ELF 解析过程中，libbpf 能够提取 key/value 的 type id，并自动把它们赋给 BPF_MAP_CREATE 属性。)

.. _BPF_Prog_Load:

3.3 BPF_PROG_LOAD(3.3 加载程序)
-------------------------------

During prog_load, func_info and line_info can be passed to kernel with proper
values for the following attributes:
::

    __u32           insn_cnt;
    __aligned_u64   insns;
    ......
    __u32           prog_btf_fd;    /* fd pointing to BTF type data */
    __u32           func_info_rec_size;     /* userspace bpf_func_info size */
    __aligned_u64   func_info;      /* func info */
    __u32           func_info_cnt;  /* number of bpf_func_info records */
    __u32           line_info_rec_size;     /* userspace bpf_line_info size */
    __aligned_u64   line_info;      /* line info */
    __u32           line_info_cnt;  /* number of bpf_line_info records */

(在 prog_load 期间，可以把 func_info 和 line_info 连同以下属性的适当取值一起传给内核。)

The func_info and line_info are an array of below, respectively.::

    struct bpf_func_info {
        __u32   insn_off; /* [0, insn_cnt - 1] */
        __u32   type_id;  /* pointing to a BTF_KIND_FUNC type */
    };
    struct bpf_line_info {
        __u32   insn_off; /* [0, insn_cnt - 1] */
        __u32   file_name_off; /* offset to string table for the filename */
        __u32   line_off; /* offset to string table for the source line */
        __u32   line_col; /* line number and column number */
    };

(func_info 和 line_info 分别是如下结构的数组。)

func_info_rec_size is the size of each func_info record, and
line_info_rec_size is the size of each line_info record. Passing the record
size to kernel make it possible to extend the record itself in the future.
(func_info_rec_size 是每条 func_info 记录的大小，line_info_rec_size 是每条 line_info 记录的大小。把记录大小传给内核，使得将来扩展记录本身成为可能。)

Below are requirements for func_info:
(以下是 func_info 的要求：)
  * func_info[0].insn_off must be 0.
    (func_info[0].insn_off 必须为 0。)
  * the func_info insn_off is in strictly increasing order and matches
    bpf func boundaries.
    (func_info 的 insn_off 必须严格递增，并与 bpf 函数边界吻合。)

Below are requirements for line_info:
(以下是 line_info 的要求：)
  * the first insn in each func must have a line_info record pointing to it.
    (每个函数的第一条 insn 必须有一条指向它的 line_info 记录。)
  * the line_info insn_off is in strictly increasing order.
    (line_info 的 insn_off 必须严格递增。)

For line_info, the line number and column number are defined as below:
::

    #define BPF_LINE_INFO_LINE_NUM(line_col)        ((line_col) >> 10)
    #define BPF_LINE_INFO_LINE_COL(line_col)        ((line_col) & 0x3ff)

(对于 line_info，行号和列号的定义如下。)

3.4 BPF_{PROG,MAP}_GET_NEXT_ID(3.4 获取下一个 ID)
-------------------------------------------------

In kernel, every loaded program, map or btf has a unique id. The id won't
change during the lifetime of a program, map, or btf.
(在内核中，每个已加载的程序、映射或 btf 都有一个唯一的 id。在程序、映射或 btf 的生命周期内，该 id 不会改变。)

The bpf syscall command BPF_{PROG,MAP}_GET_NEXT_ID returns all id's, one for
each command, to user space, for bpf program or maps, respectively, so an
inspection tool can inspect all programs and maps.
(bpf 系统调用命令 BPF_{PROG,MAP}_GET_NEXT_ID 分别针对 bpf 程序和映射，把所有 id（每个命令一组）返回给用户空间，这样检查工具就能检查所有程序和映射。)

3.5 BPF_{PROG,MAP}_GET_FD_BY_ID(3.5 通过 ID 获取 fd)
----------------------------------------------------

An introspection tool cannot use id to get details about program or maps.
A file descriptor needs to be obtained first for reference-counting purpose.
(检查工具不能用 id 直接获取程序或映射的详细信息。出于引用计数的目的，需要先获得一个文件描述符。)

3.6 BPF_OBJ_GET_INFO_BY_FD(3.6 通过 fd 获取信息)
------------------------------------------------

Once a program/map fd is acquired, an introspection tool can get the detailed
information from kernel about this fd, some of which are BTF-related. For
example, ``bpf_map_info`` returns ``btf_id`` and key/value type ids.
``bpf_prog_info`` returns ``btf_id``, func_info, and line info for translated
bpf byte codes, and jited_line_info.
(一旦获得了程序/映射的 fd，检查工具就可以从内核获取关于该 fd 的详细信息，其中一些与 BTF 相关。例如，bpf_map_info 会返回 btf_id 以及 key/value 的类型 id。bpf_prog_info 会返回 btf_id、func_info、翻译后的 bpf 字节码的行信息，以及 jited_line_info。)

3.7 BPF_BTF_GET_FD_BY_ID(3.7 通过 ID 获取 BTF fd)
-------------------------------------------------

With ``btf_id`` obtained in ``bpf_map_info`` and ``bpf_prog_info``, bpf
syscall command BPF_BTF_GET_FD_BY_ID can retrieve a btf fd. Then, with
command BPF_OBJ_GET_INFO_BY_FD, the btf blob, originally loaded into the
kernel with BPF_BTF_LOAD, can be retrieved.
(利用在 bpf_map_info 和 bpf_prog_info 中获得的 btf_id，bpf 系统调用命令 BPF_BTF_GET_FD_BY_ID 可以取回一个 btf fd。然后，再通过 BPF_OBJ_GET_INFO_BY_FD 命令，就可以取回当初用 BPF_BTF_LOAD 加载进内核的那块 btf 数据。)

With the btf blob, ``bpf_map_info``, and ``bpf_prog_info``, an introspection
tool has full btf knowledge and is able to pretty print map key/values, dump
func signatures and line info, along with byte/jit codes.
(有了 btf 数据、bpf_map_info 和 bpf_prog_info，检查工具就掌握了完整的 btf 信息，能够美化打印映射的 key/value、转储函数签名和行信息，以及字节码/JIT 代码。)

4. ELF File Format Interface(4. ELF 文件格式接口)
=================================================

4.1 .BTF section(4.1 .BTF 节)
-----------------------------

The .BTF section contains type and string data. The format of this section is
same as the one describe in :ref:`BTF_Type_String`.
(.BTF 节包含类型和字符串数据。该节的格式与 :ref:`BTF_Type_String` 中描述的格式相同。)

.. _BTF_Ext_Section:

4.2 .BTF.ext section(4.2 .BTF.ext 节)
-------------------------------------

The .BTF.ext section encodes func_info, line_info and CO-RE relocations
which needs loader manipulation before loading into the kernel.
(.BTF.ext 节编码 func_info、line_info 和 CO-RE 重定位，这些内容在加载进内核之前需要加载器进行处理。)

The specification for .BTF.ext section is defined at ``tools/lib/bpf/btf.h``
and ``tools/lib/bpf/btf.c``.
(.BTF.ext 节的规范定义在 tools/lib/bpf/btf.h 与 tools/lib/bpf/btf.c 中。)

The current header of .BTF.ext section::

    struct btf_ext_header {
        __u16   magic;
        __u8    version;
        __u8    flags;
        __u32   hdr_len;

        /* All offsets are in bytes relative to the end of this header */
        __u32   func_info_off;
        __u32   func_info_len;
        __u32   line_info_off;
        __u32   line_info_len;

        /* optional part of .BTF.ext header */
        __u32   core_relo_off;
        __u32   core_relo_len;
    };

(.BTF.ext 节目前的头结构如下。)

It is very similar to .BTF section. Instead of type/string section, it
contains func_info, line_info and core_relo sub-sections.
See :ref:`BPF_Prog_Load` for details about func_info and line_info
record format.
(它与 .BTF 节非常相似。区别在于它不是类型/字符串节，而是包含 func_info、line_info 和 core_relo 子节。func_info 与 line_info 记录格式的细节参见 :ref:`BPF_Prog_Load`。)

The func_info is organized as below.::

     func_info_rec_size              /* __u32 value */
     btf_ext_info_sec for section #1 /* func_info for section #1 */
     btf_ext_info_sec for section #2 /* func_info for section #2 */
     ...

(func_info 的组织方式如下。)

``func_info_rec_size`` specifies the size of ``bpf_func_info`` structure when
.BTF.ext is generated. ``btf_ext_info_sec``, defined below, is a collection of
func_info for each specific ELF section.::

     struct btf_ext_info_sec {
        __u32   sec_name_off; /* offset to section name */
        __u32   num_info;
        /* Followed by num_info * record_size number of bytes */
        __u8    data[0];
     };

(func_info_rec_size 指定生成 .BTF.ext 时 bpf_func_info 结构的大小。下文定义的 btf_ext_info_sec 是针对每个特定 ELF 节的 func_info 集合。)

Here, num_info must be greater than 0.
(这里 num_info 必须大于 0。)

The line_info is organized as below.::

     line_info_rec_size              /* __u32 value */
     btf_ext_info_sec for section #1 /* line_info for section #1 */
     btf_ext_info_sec for section #2 /* line_info for section #2 */
     ...

(line_info 的组织方式如下。)

``line_info_rec_size`` specifies the size of ``bpf_line_info`` structure when
.BTF.ext is generated.
(line_info_rec_size 指定生成 .BTF.ext 时 bpf_line_info 结构的大小。)

The interpretation of ``bpf_func_info->insn_off`` and
``bpf_line_info->insn_off`` is different between kernel API and ELF API. For
kernel API, the ``insn_off`` is the instruction offset in the unit of ``struct
bpf_insn``. For ELF API, the ``insn_off`` is the byte offset from the
beginning of section (``btf_ext_info_sec->sec_name_off``).
(bpf_func_info->insn_off 与 bpf_line_info->insn_off 的含义在内核 API 和 ELF API 之间有所不同。对于内核 API，insn_off 是以 struct bpf_insn 为单位的指令偏移；对于 ELF API，insn_off 是自节开头（btf_ext_info_sec->sec_name_off）起的字节偏移。)

The core_relo is organized as below.::

     core_relo_rec_size              /* __u32 value */
     btf_ext_info_sec for section #1 /* core_relo for section #1 */
     btf_ext_info_sec for section #2 /* core_relo for section #2 */

(core_relo 的组织方式如下。)

``core_relo_rec_size`` specifies the size of ``bpf_core_relo``
structure when .BTF.ext is generated. All ``bpf_core_relo`` structures
within a single ``btf_ext_info_sec`` describe relocations applied to
section named by ``btf_ext_info_sec->sec_name_off``.
(core_relo_rec_size 指定生成 .BTF.ext 时 bpf_core_relo 结构的大小。单个 btf_ext_info_sec 内的所有 bpf_core_relo 结构，描述的都是施加在以 btf_ext_info_sec->sec_name_off 命名的节上的重定位。)

See :ref:`Documentation/bpf/llvm_reloc.rst <btf-co-re-relocations>`
for more information on CO-RE relocations.
(关于 CO-RE 重定位的更多信息，参见 :ref:`Documentation/bpf/llvm_reloc.rst <btf-co-re-relocations>`。)

4.3 .BTF_ids section(4.3 .BTF_ids 节)
-------------------------------------

The .BTF_ids section encodes BTF ID values that are used within the kernel.
(.BTF_ids 节编码内核内部使用的 BTF ID 值。)

This section is created during the kernel compilation with the help of
macros defined in ``include/linux/btf_ids.h`` header file. Kernel code can
use them to create lists and sets (sorted lists) of BTF ID values.
(该节在内核编译期间，借助 include/linux/btf_ids.h 头文件中定义的宏来创建。内核代码可以用它们创建 BTF ID 值的列表和集合（有序列表）。)

The ``BTF_ID_LIST`` and ``BTF_ID`` macros define unsorted list of BTF ID values,
with following syntax::

  BTF_ID_LIST(list)
  BTF_ID(type1, name1)
  BTF_ID(type2, name2)

(BTF_ID_LIST 和 BTF_ID 宏定义无序的 BTF ID 值列表，语法如下。)

resulting in following layout in .BTF_ids section::

  __BTF_ID__type1__name1__1:
  .zero 4
  __BTF_ID__type2__name2__2:
  .zero 4

(它在 .BTF_ids 节中产生如下布局。)

The ``u32 list[];`` variable is defined to access the list.
(会定义 ``u32 list[];`` 变量来访问该列表。)

The ``BTF_ID_UNUSED`` macro defines 4 zero bytes. It's used when we
want to define unused entry in BTF_ID_LIST, like::

      BTF_ID_LIST(bpf_skb_output_btf_ids)
      BTF_ID(struct, sk_buff)
      BTF_ID_UNUSED
      BTF_ID(struct, task_struct)

(BTF_ID_UNUSED 宏定义 4 个零字节。当我们想在 BTF_ID_LIST 中定义一个未用条目时使用它，例如：)

The ``BTF_SET_START/END`` macros pair defines sorted list of BTF ID values
and their count, with following syntax::

  BTF_SET_START(set)
  BTF_ID(type1, name1)
  BTF_ID(type2, name2)
  BTF_SET_END(set)

(BTF_SET_START/END 宏对定义 BTF ID 值的有序列表及其数量，语法如下。)

resulting in following layout in .BTF_ids section::

  __BTF_ID__set__set:
  .zero 4
  __BTF_ID__type1__name1__3:
  .zero 4
  __BTF_ID__type2__name2__4:
  .zero 4

(它在 .BTF_ids 节中产生如下布局。)

The ``struct btf_id_set set;`` variable is defined to access the list.
(会定义 ``struct btf_id_set set;`` 变量来访问该列表。)

The ``typeX`` name can be one of following::

   struct, union, typedef, func

(typeX 名称可以是以下之一。)

and is used as a filter when resolving the BTF ID value.
(它在解析 BTF ID 值时被用作过滤条件。)

All the BTF ID lists and sets are compiled in the .BTF_ids section and
resolved during the linking phase of kernel build by ``resolve_btfids`` tool.
(所有 BTF ID 列表和集合都编译在 .BTF_ids 节中，并在内核构建的链接阶段由 resolve_btfids 工具解析。)

4.4 .BTF.base section(4.4 .BTF.base 节)
---------------------------------------
Split BTF - where the .BTF section only contains types not in the associated
base .BTF section - is an extremely efficient way to encode type information
for kernel modules, since they generally consist of a few module-specific
types along with a large set of shared kernel types. The former are encoded
in split BTF, while the latter are encoded in base BTF, resulting in more
compact representations. A type in split BTF that refers to a type in
base BTF refers to it using its base BTF ID, and split BTF IDs start
at last_base_BTF_ID + 1.
(拆分 BTF（split BTF）——即 .BTF 节只包含关联的基础 .BTF 节中没有的类型——是为内核模块编码类型信息的一种极其高效的方式，因为模块通常只由少量模块特有的类型加上一大批共享的内核类型组成。前者编码在拆分 BTF 中，后者编码在基础 BTF 中，从而得到更紧凑的表示。拆分 BTF 中引用基础 BTF 类型的类型，使用其基础 BTF ID 来引用它，而拆分 BTF 的 ID 从 last_base_BTF_ID + 1 开始。)

The downside of this approach however is that this makes the split BTF
somewhat brittle - when the base BTF changes, base BTF ID references are
no longer valid and the split BTF itself becomes useless. The role of the
.BTF.base section is to make split BTF more resilient for cases where
the base BTF may change, as is the case for kernel modules not built every
time the kernel is for example. .BTF.base contains named base types; INTs,
FLOATs, STRUCTs, UNIONs, ENUM[64]s and FWDs. INTs and FLOATs are fully
described in .BTF.base sections, while composite types like structs
and unions are not fully defined - the .BTF.base type simply serves as
a description of the type the split BTF referred to, so structs/unions
have 0 members in the .BTF.base section. ENUM[64]s are similarly recorded
with 0 members. Any other types are added to the split BTF. This
distillation process then leaves us with a .BTF.base section with
such minimal descriptions of base types and .BTF split section which refers
to those base types. Later, we can relocate the split BTF using both the
information stored in the .BTF.base section and the new .BTF base; the type
information in the .BTF.base section allows us to update the split BTF
references to point at the corresponding new base BTF IDs.
(然而，这种方法的缺点是它使拆分 BTF 变得有些脆弱——一旦基础 BTF 发生变化，基础 BTF ID 引用就不再有效，拆分 BTF 本身也就没用了。.BTF.base 节的作用，就是在基础 BTF 可能发生变化的情况下让拆分 BTF 更有韧性，例如那些并非每次构建内核都会重新构建的内核模块。.BTF.base 包含有名字的基础类型：INT、FLOAT、STRUCT、UNION、ENUM[64] 和 FWD。INT 与 FLOAT 在 .BTF.base 节中被完整描述，而 struct、union 之类的复合类型则不被完整定义——.BTF.base 类型只是充当对拆分 BTF 所引用类型的描述，因此 struct/union 在 .BTF.base 节中的成员数为 0。ENUM[64] 同样以 0 成员记录。任何其他类型都会被加入拆分 BTF。经过这样的提纯，最终得到一个只含基础类型最小描述的 .BTF.base 节，以及引用这些基础类型的 .BTF 拆分节。之后，我们可以同时利用保存在 .BTF.base 节中的信息和新的基础 .BTF 来重定位拆分 BTF；.BTF.base 节中的类型信息使我们能够把拆分 BTF 的引用更新为指向对应的新基础 BTF ID。)

BTF relocation happens on kernel module load when a kernel module has a
.BTF.base section, and libbpf also provides a btf__relocate() API to
accomplish this.
(当内核模块带有 .BTF.base 节时，BTF 重定位会在内核模块加载时进行；libbpf 也提供了 btf__relocate() API 来完成这件事。)

As an example consider the following base BTF::

      [1] INT 'int' size=4 bits_offset=0 nr_bits=32 encoding=SIGNED
      [2] STRUCT 'foo' size=8 vlen=2
              'f1' type_id=1 bits_offset=0
              'f2' type_id=1 bits_offset=32

(举个例子，考虑如下基础 BTF。)

...and associated split BTF::

      [3] PTR '(anon)' type_id=2

(……以及与之关联的拆分 BTF。)

i.e. split BTF describes a pointer to struct foo { int f1; int f2 };
(也就是说，拆分 BTF 描述了一个指向 struct foo { int f1; int f2 } 的指针。)

.BTF.base will consist of::

      [1] INT 'int' size=4 bits_offset=0 nr_bits=32 encoding=SIGNED
      [2] STRUCT 'foo' size=8 vlen=0

(.BTF.base 将由以下内容组成。)

If we relocate the split BTF later using the following new base BTF::

      [1] INT 'long unsigned int' size=8 bits_offset=0 nr_bits=64 encoding=(none)
      [2] INT 'int' size=4 bits_offset=0 nr_bits=32 encoding=SIGNED
      [3] STRUCT 'foo' size=8 vlen=2
              'f1' type_id=2 bits_offset=0
              'f2' type_id=2 bits_offset=32

(如果稍后用如下新的基础 BTF 来重定位拆分 BTF。)

...we can use our .BTF.base description to know that the split BTF reference
is to struct foo, and relocation results in new split BTF::

      [4] PTR '(anon)' type_id=3

(……我们就可以利用 .BTF.base 的描述得知拆分 BTF 的引用指向 struct foo，重定位后得到新的拆分 BTF 如下。)

Note that we had to update BTF ID and start BTF ID for the split BTF.
(注意，我们必须为拆分 BTF 更新 BTF ID 和起始 BTF ID。)

So we see how .BTF.base plays the role of facilitating later relocation,
leading to more resilient split BTF.
(由此可见，.BTF.base 所扮演的角色是为后续重定位提供便利，从而得到更有韧性的拆分 BTF。)

.BTF.base sections will be generated automatically for out-of-tree kernel module
builds - i.e. where KBUILD_EXTMOD is set (as it would be for "make M=path/2/mod"
cases). .BTF.base generation requires pahole support for the "distilled_base"
BTF feature; this is available in pahole v1.28 and later.
(树外（out-of-tree）内核模块构建会自动生成 .BTF.base 节——即设置了 KBUILD_EXTMOD 的场合（例如 "make M=path/2/mod" 这类情况）。生成 .BTF.base 需要 pahole 支持 "distilled_base" BTF 特性；pahole v1.28 及更高版本提供该支持。)

5. Using BTF(5. 使用 BTF)
=========================

5.1 bpftool map pretty print(5.1 bpftool 映射美化打印)
------------------------------------------------------

With BTF, the map key/value can be printed based on fields rather than simply
raw bytes. This is especially valuable for large structure or if your data
structure has bitfields. For example, for the following map,::

      enum A { A1, A2, A3, A4, A5 };
      typedef enum A ___A;
      struct tmp_t {
           char a1:4;
           int  a2:4;
           int  :4;
           __u32 a3:4;
           int b;
           ___A b1:4;
           enum A b2:4;
      };
      struct {
           __uint(type, BPF_MAP_TYPE_ARRAY);
           __type(key, int);
           __type(value, struct tmp_t);
           __uint(max_entries, 1);
      } tmpmap SEC(".maps");

(有了 BTF，映射的 key/value 就可以按字段打印，而不只是原始字节。这对大型结构体或含位域的数据结构尤其有价值。例如，对于下面这个映射。)

bpftool is able to pretty print like below:
::

      [{
            "key": 0,
            "value": {
                "a1": 0x2,
                "a2": 0x4,
                "a3": 0x6,
                "b": 7,
                "b1": 0x8,
                "b2": 0xa
            }
        }
      ]

(bpftool 能够像下面这样进行美化打印。)

5.2 bpftool prog dump(5.2 bpftool 程序转储)
-------------------------------------------

The following is an example showing how func_info and line_info can help prog
dump with better kernel symbol names, function prototypes and line
information.::

    $ bpftool prog dump jited pinned /sys/fs/bpf/test_btf_haskv
    [...]
    int test_long_fname_2(struct dummy_tracepoint_args * arg):
    bpf_prog_44a040bf25481309_test_long_fname_2:
    ; static int test_long_fname_2(struct dummy_tracepoint_args *arg)
       0:   push   %rbp
       1:   mov    %rsp,%rbp
       4:   sub    $0x30,%rsp
       b:   sub    $0x28,%rbp
       f:   mov    %rbx,0x0(%rbp)
      13:   mov    %r13,0x8(%rbp)
      17:   mov    %r14,0x10(%rbp)
      1b:   mov    %r15,0x18(%rbp)
      1f:   xor    %eax,%eax
      21:   mov    %rax,0x20(%rbp)
      25:   xor    %esi,%esi
    ; int key = 0;
      27:   mov    %esi,-0x4(%rbp)
    ; if (!arg->sock)
      2a:   mov    0x8(%rdi),%rdi
    ; if (!arg->sock)
      2e:   cmp    $0x0,%rdi
      32:   je     0x0000000000000070
      34:   mov    %rbp,%rsi
    ; counts = bpf_map_lookup_elem(&btf_map, &key);
    [...]

(下面的例子展示了 func_info 和 line_info 如何借助更好的内核符号名、函数原型和行信息来帮助 prog dump。)

5.3 Verifier Log(5.3 验证器日志)
--------------------------------

The following is an example of how line_info can help debugging verification
failure.::

       /* The code at tools/testing/selftests/bpf/test_xdp_noinline.c
        * is modified as below.
        */
       data = (void *)(long)xdp->data;
       data_end = (void *)(long)xdp->data_end;
       /*
       if (data + 4 > data_end)
               return XDP_DROP;
       */
       *(u32 *)data = dst->dst;

    $ bpftool prog load ./test_xdp_noinline.o /sys/fs/bpf/test_xdp_noinline type xdp
        ; data = (void *)(long)xdp->data;
        224: (79) r2 = *(u64 *)(r10 -112)
        225: (61) r2 = *(u32 *)(r2 +0)
        ; *(u32 *)data = dst->dst;
        226: (63) *(u32 *)(r2 +0) = r1
        invalid access to packet, off=0 size=4, R2(id=0,off=0,r=0)
        R2 offset is outside of the packet

(下面的例子展示了 line_info 如何帮助调试验证失败。)

6. BTF Generation(6. BTF 生成)
==============================

You need latest pahole
(你需要最新的 pahole。)

  https://git.kernel.org/pub/scm/devel/pahole/pahole.git/

or llvm (8.0 or later). The pahole acts as a dwarf2btf converter. It doesn't
support .BTF.ext and btf BTF_KIND_FUNC type yet. For example,::

      -bash-4.4$ cat t.c
      struct t {
        int a:2;
        int b:3;
        int c:2;
      } g;
      -bash-4.4$ gcc -c -O2 -g t.c
      -bash-4.4$ pahole -JV t.o
      File t.o:
      [1] STRUCT t kind_flag=1 size=4 vlen=3
              a type_id=2 bitfield_size=2 bits_offset=0
              b type_id=2 bitfield_size=3 bits_offset=2
              c type_id=2 bitfield_size=2 bits_offset=5
      [2] INT int size=4 bit_offset=0 nr_bits=32 encoding=SIGNED

(或者 llvm（8.0 或更高版本）。pahole 充当 dwarf2btf 转换器。它尚不支持 .BTF.ext 和 btf 的 BTF_KIND_FUNC 类型。例如如下输出。)

The llvm is able to generate .BTF and .BTF.ext directly with -g for bpf target
only. The assembly code (-S) is able to show the BTF encoding in assembly
format.::

    -bash-4.4$ cat t2.c
    typedef int __int32;
    struct t2 {
      int a2;
      int (*f2)(char q1, __int32 q2, ...);
      int (*f3)();
    } g2;
    int main() { return 0; }
    int test() { return 0; }
    -bash-4.4$ clang -c -g -O2 --target=bpf t2.c
    -bash-4.4$ readelf -S t2.o
      ......
      [ 8] .BTF              PROGBITS         0000000000000000  00000247
           000000000000016e  0000000000000000           0     0     1
      [ 9] .BTF.ext          PROGBITS         0000000000000000  000003b5
           0000000000000060  0000000000000000           0     0     1
      [10] .rel.BTF.ext      REL              0000000000000000  000007e0
           0000000000000040  0000000000000010          16     9     8
      ......
    -bash-4.4$ clang -S -g -O2 --target=bpf t2.c
    -bash-4.4$ cat t2.s
      ......
            .section        .BTF,"",@progbits
            .short  60319                   # 0xeb9f
            .byte   1
            .byte   0
            .long   24
            .long   0
            .long   220
            .long   220
            .long   122
            .long   0                       # BTF_KIND_FUNC_PROTO(id = 1)
            .long   218103808               # 0xd000000
            .long   2
            .long   83                      # BTF_KIND_INT(id = 2)
            .long   16777216                # 0x1000000
            .long   4
            .long   16777248                # 0x1000020
      ......
            .byte   0                       # string offset=0
            .ascii  ".text"                 # string offset=1
            .byte   0
            .ascii  "/home/yhs/tmp-pahole/t2.c" # string offset=7
            .byte   0
            .ascii  "int main() { return 0; }" # string offset=33
            .byte   0
            .ascii  "int test() { return 0; }" # string offset=58
            .byte   0
            .ascii  "int"                   # string offset=83
      ......
            .section        .BTF.ext,"",@progbits
            .short  60319                   # 0xeb9f
            .byte   1
            .byte   0
            .long   24
            .long   0
            .long   28
            .long   28
            .long   44
            .long   8                       # FuncInfo
            .long   1                       # FuncInfo section string offset=1
            .long   2
            .long   .Lfunc_begin0
            .long   3
            .long   .Lfunc_begin1
            .long   5
            .long   16                      # LineInfo
            .long   1                       # LineInfo section string offset=1
            .long   2
            .long   .Ltmp0
            .long   7
            .long   33
            .long   7182                    # Line 7 Col 14
            .long   .Ltmp3
            .long   7
            .long   58
            .long   8206                    # Line 8 Col 14

(llvm 能够只为 bpf 目标用 -g 直接生成 .BTF 和 .BTF.ext。汇编代码（-S）能够以汇编格式展示 BTF 编码。)

7. Testing(7. 测试)
===================

The kernel BPF selftest `tools/testing/selftests/bpf/prog_tests/btf.c`_
provides an extensive set of BTF-related tests.
(内核 BPF 自测工具 `tools/testing/selftests/bpf/prog_tests/btf.c`_ 提供了一整套与 BTF 相关的测试。)

.. Links
.. _tools/testing/selftests/bpf/prog_tests/btf.c:
   https://git.kernel.org/pub/scm/linux/kernel/git/stable/linux.git/tree/tools/testing/selftests/bpf/prog_tests/btf.c
