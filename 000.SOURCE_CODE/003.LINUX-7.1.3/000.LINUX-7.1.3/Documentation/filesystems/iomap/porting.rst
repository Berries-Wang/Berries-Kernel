.. SPDX-License-Identifier: GPL-2.0
.. _iomap_porting:

..
        Dumb style notes to maintain the author's sanity:
        Please try to start sentences on separate lines so that
        sentence changes don't bleed colors in diff.
        Heading decorations are documented in sphinx.rst.

=======================
Porting Your Filesystem
=======================

(移植你的文件系统)

.. contents:: Table of Contents
   :local:

Why Convert?
(为什么要转换？)
============

There are several reasons to convert a filesystem to iomap:
(将文件系统转换为 iomap 有以下几个原因：)

 1. The classic Linux I/O path is not terribly efficient.
    Pagecache operations lock a single base page at a time and then call
    into the filesystem to return a mapping for only that page.
    Direct I/O operations build I/O requests a single file block at a
    time.
    This worked well enough for direct/indirect-mapped filesystems such
    as ext2, but is very inefficient for extent-based filesystems such
    as XFS.
    (经典的 Linux I/O 路径效率并不高。页缓存操作一次只锁定一个基础页，然后调用文件系统仅返回该页的映射。直接 I/O 操作一次只构建单个文件块的 I/O 请求。这对于 ext2 等直接/间接映射文件系统来说效果尚可，但对于 XFS 等基于区段(extent)的文件系统来说效率非常低。)

 2. Large folios are only supported via iomap; there are no plans to
    convert the old buffer_head path to use them.
    (大 folio 仅通过 iomap 支持；没有计划将旧的 buffer_head 路径转换为使用它们。)

 3. Direct access to storage on memory-like devices (fsdax) is only
    supported via iomap.
    (对类内存设备(fsdax)上存储的直接访问仅通过 iomap 支持。)

 4. Lower maintenance overhead for individual filesystem maintainers.
    iomap handles common pagecache related operations itself, such as
    allocating, instantiating, locking, and unlocking of folios.
    No ->write_begin(), ->write_end() or direct_IO
    address_space_operations are required to be implemented by
    filesystem using iomap.
    (降低单个文件系统维护者的维护开销。iomap 自行处理常见的页缓存相关操作，例如 folio 的分配、实例化、锁定和解锁。使用 iomap 的文件系统无需实现 ->write_begin()、->write_end() 或 direct_IO address_space_operations。)

How Do I Convert a Filesystem?
(我如何转换文件系统？)
==============================

First, add ``#include <linux/iomap.h>`` from your source code and add
``select FS_IOMAP`` to your filesystem's Kconfig option.
Build the kernel, run fstests with the ``-g all`` option across a wide
variety of your filesystem's supported configurations to build a
baseline of which tests pass and which ones fail.
(首先，在你的源代码中添加 ``#include <linux/iomap.h>``，并在你的文件系统的 Kconfig 选项中添加 ``select FS_IOMAP``。构建内核，在文件系统支持的各种配置上使用 ``-g all`` 选项运行 fstests，建立一个哪些测试通过、哪些测试失败的基线。)

The recommended approach is first to implement ``->iomap_begin`` (and
``->iomap_end`` if necessary) to allow iomap to obtain a read-only
mapping of a file range.
In most cases, this is a relatively trivial conversion of the existing
``get_block()`` function for read-only mappings.
``FS_IOC_FIEMAP`` is a good first target because it is trivial to
implement support for it and then to determine that the extent map
iteration is correct from userspace.
If FIEMAP is returning the correct information, it's a good sign that
other read-only mapping operations will do the right thing.
(推荐的做法是首先实现 ``->iomap_begin``(如有必要还有 ``->iomap_end``)，让 iomap 能够获得文件范围的只读映射。在大多数情况下，这是对现有的只读映射 ``get_block()`` 函数相对简单的转换。``FS_IOC_FIEMAP`` 是一个很好的首选目标，因为实现对它的支持很简单，随后就可以从用户空间确认区段(extent)映射的迭代是正确的。如果 FIEMAP 返回正确的信息，这是其他只读映射操作也会正确工作的好迹象。)

Next, modify the filesystem's ``get_block(create = false)``
implementation to use the new ``->iomap_begin`` implementation to map
file space for selected read operations.
Hide behind a debugging knob the ability to switch on the iomap mapping
functions for selected call paths.
It is necessary to write some code to fill out the bufferhead-based
mapping information from the ``iomap`` structure, but the new functions
can be tested without needing to implement any iomap APIs.
(接下来，修改文件系统的 ``get_block(create = false)`` 实现，使用新的 ``->iomap_begin`` 实现为选定的读操作映射文件空间。将对选定调用路径开启 iomap 映射函数的能力隐藏在一个调试开关之后。需要编写一些代码，从 ``iomap`` 结构填充基于 bufferhead 的映射信息，但无需实现任何 iomap API 即可测试这些新函数。)

Once the read-only functions are working like this, convert each high
level file operation one by one to use iomap native APIs instead of
going through ``get_block()``.
Done one at a time, regressions should be self evident.
You *do* have a regression test baseline for fstests, right?
It is suggested to convert swap file activation, ``SEEK_DATA``, and
``SEEK_HOLE`` before tackling the I/O paths.
A likely complexity at this point will be converting the buffered read
I/O path because of bufferheads.
The buffered read I/O paths doesn't need to be converted yet, though the
direct I/O read path should be converted in this phase.
(当只读函数以这种方式工作后，将每个高层文件操作逐个转换为使用 iomap 原生 API，而不是通过 ``get_block()``。一次转换一个，回归问题应当显而易见。你确实有 fstests 的回归测试基线，对吧？建议在处理 I/O 路径之前，先转换交换文件激活、``SEEK_DATA`` 和 ``SEEK_HOLE``。此时一个可能的复杂点是由于 bufferheads 的存在而需要转换缓冲读 I/O 路径。缓冲读 I/O 路径尚不需要转换，但直接 I/O 读路径应在此阶段完成转换。)

At this point, you should look over your ``->iomap_begin`` function.
If it switches between large blocks of code based on dispatching of the
``flags`` argument, you should consider breaking it up into
per-operation iomap ops with smaller, more cohesive functions.
XFS is a good example of this.
(此时，你应该审视你的 ``->iomap_begin`` 函数。如果它根据 ``flags`` 参数的分发在大的代码块之间切换，你应该考虑将其拆分为每操作(per-operation)的 iomap ops，使用更小、更内聚的函数。XFS 就是这方面的一个好例子。)

The next thing to do is implement ``get_blocks(create == true)``
functionality in the ``->iomap_begin``/``->iomap_end`` methods.
It is strongly recommended to create separate mapping functions and
iomap ops for write operations.
Then convert the direct I/O write path to iomap, and start running fsx
w/ DIO enabled in earnest on filesystem.
This will flush out lots of data integrity corner case bugs that the new
write mapping implementation introduces.
(接下来要做的是在 ``->iomap_begin``/``->iomap_end`` 方法中实现 ``get_blocks(create == true)`` 功能。强烈建议为写操作创建单独的映射函数和 iomap ops。然后将直接 I/O 写路径转换为 iomap，并开始在文件系统上认真运行启用 DIO 的 fsx。这将暴露出新的写映射实现引入的许多数据完整性边角情况错误。)

Now, convert any remaining file operations to call the iomap functions.
This will get the entire filesystem using the new mapping functions, and
they should largely be debugged and working correctly after this step.
(现在，将剩余的文件操作全部转换为调用 iomap 函数。这将使整个文件系统使用新的映射函数，在此步骤之后，它们应该基本已被调试并正常工作。)

Most likely at this point, the buffered read and write paths will still
need to be converted.
The mapping functions should all work correctly, so all that needs to be
done is rewriting all the code that interfaces with bufferheads to
interface with iomap and folios.
It is much easier first to get regular file I/O (without any fancy
features like fscrypt, fsverity, compression, or data=journaling)
converted to use iomap.
Some of those fancy features (fscrypt and compression) aren't
implemented yet in iomap.
For unjournalled filesystems that use the pagecache for symbolic links
and directories, you might also try converting their handling to iomap.
(此时，缓冲读和写路径很可能仍需转换。映射函数应该都能正确工作，所以需要做的只是重写所有与 bufferheads 交互的代码，改为与 iomap 和 folio 交互。首先将普通文件 I/O(不带 fscrypt、fsverity、压缩或 data=journaling 之类的花哨功能)转换为使用 iomap 要容易得多。其中一些花哨的功能(fscrypt 和压缩)尚未在 iomap 中实现。对于将页缓存用于符号链接和目录的非日志文件系统，你也可以尝试将它们的处理转换为 iomap。)

The rest is left as an exercise for the reader, as it will be different
for every filesystem.
If you encounter problems, email the people and lists in
``get_maintainers.pl`` for help.
(其余部分留给读者作为练习，因为它因文件系统而异。如果遇到问题，请向 ``get_maintainers.pl`` 中列出的人员和邮件列表发邮件寻求帮助。)
