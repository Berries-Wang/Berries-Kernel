========================================================================
Generic bitfield packing and unpacking functions(通用位域打包与解包函数)
========================================================================

Problem statement(问题陈述)
---------------------------

When working with hardware, one has to choose between several approaches of
interfacing with it.
One can memory-map a pointer to a carefully crafted struct over the hardware
device's memory region, and access its fields as struct members (potentially
declared as bitfields). But writing code this way would make it less portable,
due to potential endianness mismatches between the CPU and the hardware device.
Additionally, one has to pay close attention when translating register
definitions from the hardware documentation into bit field indices for the
structs. Also, some hardware (typically networking equipment) tends to group
its register fields in ways that violate any reasonable word boundaries
(sometimes even 64 bit ones). This creates the inconvenience of having to
define "high" and "low" portions of register fields within the struct.
A more robust alternative to struct field definitions would be to extract the
required fields by shifting the appropriate number of bits. But this would
still not protect from endianness mismatches, except if all memory accesses
were performed byte-by-byte. Also the code can easily get cluttered, and the
high-level idea might get lost among the many bit shifts required.
Many drivers take the bit-shifting approach and then attempt to reduce the
clutter with tailored macros, but more often than not these macros take
shortcuts that still prevent the code from being truly portable.
(与硬件打交道时，必须在几种接口方式之间做出选择。可以把一个精心构造的 struct 的指针内存映射到硬件设备的内存区域上，像访问 struct 成员（可能声明为位域）那样访问其字段。但这种写法可移植性较差，因为 CPU 与硬件设备之间可能存在字节序不匹配。此外，把硬件文档中的寄存器定义转换成 struct 的位域索引时也必须格外小心。另外，某些硬件（通常是网络设备）往往把寄存器字段分组得违反任何合理的字边界（有时甚至是 64 位字边界）。这带来一个不便：必须在 struct 内部为寄存器字段定义"高位"与"低位"两段。相比 struct 字段定义，一种更稳健的替代方案是通过移位相应的位数来提取所需的字段。但只要内存访问不是逐字节进行，这仍然无法避免字节序不匹配的问题。而且代码很容易变得杂乱，高层思路可能淹没在大量所需的移位操作之中。许多驱动程序采用移位方案，再试图用定制宏来减少杂乱，但这些宏大多走捷径，仍然无法让代码真正做到可移植。)

The solution(解决方案)
----------------------

This API deals with 2 basic operations:
(该 API 处理两种基本操作：)

  - Packing a CPU-usable number into a memory buffer (with hardware
    constraints/quirks)
    (把一个 CPU 可用的数字打包进内存缓冲区（带有硬件约束/怪癖）。)
  - Unpacking a memory buffer (which has hardware constraints/quirks)
    into a CPU-usable number.
    (把一个（带有硬件约束/怪癖的）内存缓冲区解包成 CPU 可用的数字。)

The API offers an abstraction over said hardware constraints and quirks,
over CPU endianness and therefore between possible mismatches between
the two.
(该 API 对上述硬件约束与怪癖、对 CPU 字节序，以及两者之间可能的不匹配，提供了抽象。)

The basic unit of these API functions is the u64. From the CPU's
perspective, bit 63 always means bit offset 7 of byte 7, albeit only
logically. The question is: where do we lay this bit out in memory?
(这些 API 函数的基本单元是 u64。从 CPU 的视角看，位 63 始终表示字节 7 的位偏移 7，尽管这只是逻辑上的。问题在于：我们该把这个位布置到内存的哪个位置？)

The following examples cover the memory layout of a packed u64 field.
The byte offsets in the packed buffer are always implicitly 0, 1, ... 7.
What the examples show is where the logical bytes and bits sit.
(下面的示例涵盖打包后的 u64 字段的内存布局。打包缓冲区中的字节偏移总是隐含地为 0、1、……、7。示例展示的是逻辑字节与逻辑位所在的位置。)

1. Normally (no quirks), we would do it like this:

::

  63 62 61 60 59 58 57 56 55 54 53 52 51 50 49 48 47 46 45 44 43 42 41 40 39 38 37 36 35 34 33 32
  7                       6                       5                        4
  31 30 29 28 27 26 25 24 23 22 21 20 19 18 17 16 15 14 13 12 11 10  9  8  7  6  5  4  3  2  1  0
  3                       2                       1                        0

(正常情况下（没有 quirk），我们会像下面这样布置：)

That is, the MSByte (7) of the CPU-usable u64 sits at memory offset 0, and the
LSByte (0) of the u64 sits at memory offset 7.
This corresponds to what most folks would regard to as "big endian", where
bit i corresponds to the number 2^i. This is also referred to in the code
comments as "logical" notation.
(也就是说，CPU 可用 u64 的最高有效字节（MSByte，7）位于内存偏移 0 处，而 u64 的最低有效字节（LSByte，0）位于内存偏移 7 处。这对应于大多数人所说的"大端"（big endian），即位 i 对应数值 2^i。代码注释中这也被称为"逻辑"（logical）记法。)


2. If QUIRK_MSB_ON_THE_RIGHT is set, we do it like this:

::

  56 57 58 59 60 61 62 63 48 49 50 51 52 53 54 55 40 41 42 43 44 45 46 47 32 33 34 35 36 37 38 39
  7                       6                        5                       4
  24 25 26 27 28 29 30 31 16 17 18 19 20 21 22 23  8  9 10 11 12 13 14 15  0  1  2  3  4  5  6  7
  3                       2                        1                       0

(如果设置了 QUIRK_MSB_ON_THE_RIGHT，我们会像下面这样布置：)

That is, QUIRK_MSB_ON_THE_RIGHT does not affect byte positioning, but
inverts bit offsets inside a byte.
(也就是说，QUIRK_MSB_ON_THE_RIGHT 不影响字节的摆放位置，只是把字节内部的位偏移反转。)


3. If QUIRK_LITTLE_ENDIAN is set, we do it like this:

::

  39 38 37 36 35 34 33 32 47 46 45 44 43 42 41 40 55 54 53 52 51 50 49 48 63 62 61 60 59 58 57 56
  4                       5                       6                       7
  7  6  5  4  3  2  1  0  15 14 13 12 11 10  9  8 23 22 21 20 19 18 17 16 31 30 29 28 27 26 25 24
  0                       1                       2                       3

(如果设置了 QUIRK_LITTLE_ENDIAN，我们会像下面这样布置：)

Therefore, QUIRK_LITTLE_ENDIAN means that inside the memory region, every
byte from each 4-byte word is placed at its mirrored position compared to
the boundary of that word.
(因此，QUIRK_LITTLE_ENDIAN 意味着在内存区域内部，每个 4 字节字中的每个字节都被放置在相对于该字边界的镜像位置上。)

4. If QUIRK_MSB_ON_THE_RIGHT and QUIRK_LITTLE_ENDIAN are both set, we do it
   like this:

::

  32 33 34 35 36 37 38 39 40 41 42 43 44 45 46 47 48 49 50 51 52 53 54 55 56 57 58 59 60 61 62 63
  4                       5                       6                       7
  0  1  2  3  4  5  6  7  8   9 10 11 12 13 14 15 16 17 18 19 20 21 22 23 24 25 26 27 28 29 30 31
  0                       1                       2                       3

(如果 QUIRK_MSB_ON_THE_RIGHT 与 QUIRK_LITTLE_ENDIAN 同时设置，我们会像下面这样布置：)


5. If just QUIRK_LSW32_IS_FIRST is set, we do it like this:

::

  31 30 29 28 27 26 25 24 23 22 21 20 19 18 17 16 15 14 13 12 11 10  9  8  7  6  5  4  3  2  1  0
  3                       2                       1                        0
  63 62 61 60 59 58 57 56 55 54 53 52 51 50 49 48 47 46 45 44 43 42 41 40 39 38 37 36 35 34 33 32
  7                       6                       5                        4

(如果只设置了 QUIRK_LSW32_IS_FIRST，我们会像下面这样布置：)

In this case the 8 byte memory region is interpreted as follows: first
4 bytes correspond to the least significant 4-byte word, next 4 bytes to
the more significant 4-byte word.
(此时 8 字节内存区域的解释方式为：前 4 字节对应较低有效的 4 字节字，接下来的 4 字节对应较高有效的 4 字节字。)


6. If QUIRK_LSW32_IS_FIRST and QUIRK_MSB_ON_THE_RIGHT are set, we do it like
   this:

::

  24 25 26 27 28 29 30 31 16 17 18 19 20 21 22 23  8  9 10 11 12 13 14 15  0  1  2  3  4  5  6  7
  3                       2                        1                       0
  56 57 58 59 60 61 62 63 48 49 50 51 52 53 54 55 40 41 42 43 44 45 46 47 32 33 34 35 36 37 38 39
  7                       6                        5                       4

(如果设置了 QUIRK_LSW32_IS_FIRST 与 QUIRK_MSB_ON_THE_RIGHT，我们会像下面这样布置：)


7. If QUIRK_LSW32_IS_FIRST and QUIRK_LITTLE_ENDIAN are set, it looks like
   this:

::

  7  6  5  4  3  2  1  0  15 14 13 12 11 10  9  8 23 22 21 20 19 18 17 16 31 30 29 28 27 26 25 24
  0                       1                       2                       3
  39 38 37 36 35 34 33 32 47 46 45 44 43 42 41 40 55 54 53 52 51 50 49 48 63 62 61 60 59 58 57 56
  4                       5                       6                       7

(如果设置了 QUIRK_LSW32_IS_FIRST 与 QUIRK_LITTLE_ENDIAN，它看起来像这样：)


8. If QUIRK_LSW32_IS_FIRST, QUIRK_LITTLE_ENDIAN and QUIRK_MSB_ON_THE_RIGHT
   are set, it looks like this:

::

  0  1  2  3  4  5  6  7  8   9 10 11 12 13 14 15 16 17 18 19 20 21 22 23 24 25 26 27 28 29 30 31
  0                       1                       2                       3
  32 33 34 35 36 37 38 39 40 41 42 43 44 45 46 47 48 49 50 51 52 53 54 55 56 57 58 59 60 61 62 63
  4                       5                       6                       7

(如果设置了 QUIRK_LSW32_IS_FIRST、QUIRK_LITTLE_ENDIAN 与 QUIRK_MSB_ON_THE_RIGHT，它看起来像这样：)


We always think of our offsets as if there were no quirk, and we translate
them afterwards, before accessing the memory region.
(我们始终把偏移当作没有 quirk 的情况来思考，然后在访问内存区域之前再进行换算。)

Note on buffer lengths not multiple of 4(关于长度不是 4 的倍数的缓冲区的说明)
-----------------------------------------------------------------------------

To deal with memory layout quirks where groups of 4 bytes are laid out "little
endian" relative to each other, but "big endian" within the group itself, the
concept of groups of 4 bytes is intrinsic to the packing API (not to be
confused with the memory access, which is performed byte by byte, though).
(为了处理这样的内存布局怪癖：4 字节一组，组与组之间按"小端"相对摆放，而组内部本身却按"大端"摆放——"4 字节组"这一概念对打包 API 来说是固有的（不过内存访问本身仍是逐字节执行的，不要混淆）。)

With buffer lengths not multiple of 4, this means one group will be incomplete.
Depending on the quirks, this may lead to discontinuities in the bit fields
accessible through the buffer. The packing API assumes discontinuities were not
the intention of the memory layout, so it avoids them by effectively logically
shortening the most significant group of 4 octets to the number of octets
actually available.
(当缓冲区长度不是 4 的倍数时，意味着会有一组是不完整的。根据 quirk 的不同，这可能导致通过缓冲区可访问的位域出现不连续。打包 API 假定这种不连续并非内存布局的本意，因此它会在逻辑上把最高有效的那组 4 字节缩短到实际可用的字节数，从而避免不连续。)

Example with a 31 byte sized buffer given below. Physical buffer offsets are
implicit, and increase from left to right within a group, and from top to
bottom within a column.
(下面给出一个 31 字节大小缓冲区的示例。物理缓冲区偏移是隐含的，在一组内从左到右递增，在一列内自上而下递增。)

No quirks:

::

            31         29         28        |   Group 7 (most significant)
 27         26         25         24        |   Group 6
 23         22         21         20        |   Group 5
 19         18         17         16        |   Group 4
 15         14         13         12        |   Group 3
 11         10          9          8        |   Group 2
  7          6          5          4        |   Group 1
  3          2          1          0        |   Group 0 (least significant)

(无 quirk：)

QUIRK_LSW32_IS_FIRST:

::

  3          2          1          0        |   Group 0 (least significant)
  7          6          5          4        |   Group 1
 11         10          9          8        |   Group 2
 15         14         13         12        |   Group 3
 19         18         17         16        |   Group 4
 23         22         21         20        |   Group 5
 27         26         25         24        |   Group 6
 30         29         28                   |   Group 7 (most significant)

(设置了 QUIRK_LSW32_IS_FIRST：)

QUIRK_LITTLE_ENDIAN:

::

            30         28         29        |   Group 7 (most significant)
 24         25         26         27        |   Group 6
 20         21         22         23        |   Group 5
 16         17         18         19        |   Group 4
 12         13         14         15        |   Group 3
  8          9         10         11        |   Group 2
  4          5          6          7        |   Group 1
  0          1          2          3        |   Group 0 (least significant)

(设置了 QUIRK_LITTLE_ENDIAN：)

QUIRK_LITTLE_ENDIAN | QUIRK_LSW32_IS_FIRST:

::

  0          1          2          3        |   Group 0 (least significant)
  4          5          6          7        |   Group 1
  8          9         10         11        |   Group 2
 12         13         14         15        |   Group 3
 16         17         18         19        |   Group 4
 20         21         22         23        |   Group 5
 24         25         26         27        |   Group 6
 28         29         30                   |   Group 7 (most significant)

(设置了 QUIRK_LITTLE_ENDIAN | QUIRK_LSW32_IS_FIRST：)

Intended use(预期用法)
----------------------

Drivers that opt to use this API first need to identify which of the above 3
quirk combinations (for a total of 8) match what the hardware documentation
describes.
(选择使用该 API 的驱动程序首先需要判断：上述 3 个 quirk 的 8 种组合中，哪一种与硬件文档所描述的行为相符。)

There are 3 supported usage patterns, detailed below.
(下面详述 3 种受支持的使用模式。)

packing()(打包函数)
^^^^^^^^^^^^^^^^^^^

This API function is deprecated.
(该 API 函数已废弃。)

The packing() function returns an int-encoded error code, which protects the
programmer against incorrect API use.  The errors are not expected to occur
during runtime, therefore it is reasonable to wrap packing() into a custom
function which returns void and swallows those errors. Optionally it can
dump stack or print the error description.
(packing() 函数返回以 int 编码的错误码，以保护程序员免于错误使用 API。这些错误预期不会在运行时发生，因此把 packing() 包装成一个返回 void 并吞掉这些错误的自定义函数是合理的。也可以选择转储栈回溯或打印错误描述。)

.. code-block:: c

  void my_packing(void *buf, u64 *val, int startbit, int endbit,
                  size_t len, enum packing_op op)
  {
          int err;

          /* Adjust quirks accordingly */
          err = packing(buf, val, startbit, endbit, len, op, QUIRK_LSW32_IS_FIRST);
          if (likely(!err))
                  return;

          if (err == -EINVAL) {
                  pr_err("Start bit (%d) expected to be larger than end (%d)\n",
                         startbit, endbit);
          } else if (err == -ERANGE) {
                  if ((startbit - endbit + 1) > 64)
                          pr_err("Field %d-%d too large for 64 bits!\n",
                                 startbit, endbit);
                  else
                          pr_err("Cannot store %llx inside bits %d-%d (would truncate)\n",
                                 *val, startbit, endbit);
          }
          dump_stack();
  }

pack() and unpack()(打包与解包函数)
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

These are const-correct variants of packing(), and eliminate the last "enum
packing_op op" argument.
(它们是 packing() 的 const 正确变体，并去掉了最后一个 "enum packing_op op" 参数。)

Calling pack(...) is equivalent, and preferred, to calling packing(..., PACK).
(调用 pack(...) 等价于调用 packing(..., PACK)，而且更受推荐。)

Calling unpack(...) is equivalent, and preferred, to calling packing(..., UNPACK).
(调用 unpack(...) 等价于调用 packing(..., UNPACK)，而且更受推荐。)

pack_fields() and unpack_fields()(字段打包与解包函数)
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

The library exposes optimized functions for the scenario where there are many
fields represented in a buffer, and it encourages consumer drivers to avoid
repetitive calls to pack() and unpack() for each field, but instead use
pack_fields() and unpack_fields(), which reduces the code footprint.
(该库针对一个缓冲区中表示许多字段的场景提供了优化函数，并鼓励使用方驱动程序避免对每个字段反复调用 pack() 和 unpack()，改用 pack_fields() 和 unpack_fields()，以减少代码占用。)

These APIs use field definitions in arrays of ``struct packed_field_u8`` or
``struct packed_field_u16``, allowing consumer drivers to minimize the size
of these arrays according to their custom requirements.
(这些 API 使用 ``struct packed_field_u8`` 或 ``struct packed_field_u16`` 数组中的字段定义，使驱动程序能够按自身需求把数组尺寸降到最小。)

The pack_fields() and unpack_fields() API functions are actually macros which
automatically select the appropriate function at compile time, based on the
type of the fields array passed in.
(pack_fields() 和 unpack_fields() 这两个 API 函数实际上是宏，会根据传入的字段数组类型在编译期自动选择合适的函数。)

An additional benefit over pack() and unpack() is that sanity checks on the
field definitions are handled at compile time with ``BUILD_BUG_ON`` rather
than only when the offending code is executed. These functions return void and
wrapping them to handle unexpected errors is not necessary.
(相比 pack() 和 unpack() 的另一个优点是：对字段定义的健全性检查通过 ``BUILD_BUG_ON`` 在编译期完成，而不是等到出问题的代码执行时才进行。这些函数返回 void，无须再包一层来处理意外错误。)

It is recommended, but not required, that you wrap your packed buffer into a
structured type with a fixed size. This generally makes it easier for the
compiler to enforce that the correct size buffer is used.
(建议（但非强制）把打包缓冲区包装成一个固定大小的结构化类型。这通常更便于编译器强制使用正确尺寸的缓冲区。)

Here is an example of how to use the fields APIs:
(下面是一个使用这些字段 API 的示例：)

.. code-block:: c

   /* Ordering inside the unpacked structure is flexible and can be different
    * from the packed buffer. Here, it is optimized to reduce padding.
    */
   struct data {
        u64 field3;
        u32 field4;
        u16 field1;
        u8 field2;
   };

   #define SIZE 13

   typedef struct __packed { u8 buf[SIZE]; } packed_buf_t;

   static const struct packed_field_u8 fields[] = {
           PACKED_FIELD(100, 90, struct data, field1),
           PACKED_FIELD(90, 87, struct data, field2),
           PACKED_FIELD(86, 30, struct data, field3),
           PACKED_FIELD(29, 0, struct data, field4),
   };

   void unpack_your_data(const packed_buf_t *buf, struct data *unpacked)
   {
           BUILD_BUG_ON(sizeof(*buf) != SIZE;

           unpack_fields(buf, sizeof(*buf), unpacked, fields,
                         QUIRK_LITTLE_ENDIAN);
   }

   void pack_your_data(const struct data *unpacked, packed_buf_t *buf)
   {
           BUILD_BUG_ON(sizeof(*buf) != SIZE;

           pack_fields(buf, sizeof(*buf), unpacked, fields,
                       QUIRK_LITTLE_ENDIAN);
   }
