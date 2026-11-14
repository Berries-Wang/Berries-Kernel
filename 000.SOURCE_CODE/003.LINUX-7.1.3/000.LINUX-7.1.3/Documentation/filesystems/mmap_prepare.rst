.. SPDX-License-Identifier: GPL-2.0

===========================
mmap_prepare callback HOWTO
===========================

(mmap_prepare 回调 HOWTO)

Introduction
(简介)
============

The ``struct file->f_op->mmap()`` callback has been deprecated as it is both a
stability and security risk, and doesn't always permit the merging of adjacent
mappings resulting in unnecessary memory fragmentation.
(``struct file->f_op->mmap()`` 回调已被弃用，因为它既是稳定性风险也是安全风险，并且并不总是允许合并相邻的映射，从而导致不必要的内存碎片。)

It has been replaced with the ``file->f_op->mmap_prepare()`` callback which
solves these problems.
(它已被 ``file->f_op->mmap_prepare()`` 回调所取代，后者解决了这些问题。)

This hook is called right at the beginning of setting up the mapping, and
importantly it is invoked *before* any merging of adjacent mappings has taken
place.
(这个钩子在开始设置映射时立即被调用，而且重要的是，它在任何相邻映射合并发生*之前*被调用。)

If an error arises upon mapping, it might arise after this callback has been
invoked, therefore it should be treated as effectively stateless.
(如果映射过程中出现错误，该错误可能出现在此回调被调用之后，因此应当把它视为实际上无状态的。)

That is - no resources should be allocated nor state updated to reflect that a
mapping has been established, as the mapping may either be merged, or fail to be
mapped after the callback is complete.
(也就是说——不应分配任何资源，也不应更新状态来表明映射已经建立，因为在回调完成之后，该映射既可能被合并，也可能映射失败。)

Mapped callback
(已映射回调)
---------------

If resources need to be allocated per-mapping, or state such as a reference
count needs to be manipulated, this should be done using the ``vm_ops->mapped``
hook, which itself should be set by the >mmap_prepare hook.
(如果需要按映射分配资源，或者需要操纵引用计数之类的状态，应当使用 ``vm_ops->mapped`` 钩子来完成，而该钩子本身应由 >mmap_prepare 钩子来设置。)

This callback is only invoked if a new mapping has been established and was not
merged with any other, and is invoked at a point where no error may occur before
the mapping is established.
(只有在已经建立了新映射且它没有与任何其他映射合并的情况下，才会调用此回调，并且调用它的时点保证在映射建立之前不会发生任何错误。)

You may return an error to the callback itself, which will cause the mapping to
become unmapped and an error returned to the mmap() caller. This is useful if
resources need to be allocated, and that allocation might fail.
(你可以直接向回调本身返回一个错误，这将导致该映射被解除映射，并向 mmap() 调用者返回一个错误。如果需要分配资源而该分配可能失败，这就很有用。)

How To Use
(如何使用)
==========

In your driver's struct file_operations struct, specify an ``mmap_prepare``
callback rather than an ``mmap`` one, e.g. for ext4:
(在你驱动程序的 struct file_operations 结构体中，指定一个 ``mmap_prepare`` 回调而不是 ``mmap`` 回调，例如对 ext4 而言：)

.. code-block:: C

    const struct file_operations ext4_file_operations = {
        ...
        .mmap_prepare    = ext4_file_mmap_prepare,
    };

This has a signature of ``int (*mmap_prepare)(struct vm_area_desc *)``.
(它的函数签名是 ``int (*mmap_prepare)(struct vm_area_desc *)``。)

Examining the struct vm_area_desc type:
(下面来看一下 struct vm_area_desc 类型：)

.. code-block:: C

    struct vm_area_desc {
        /* Immutable state. */
        const struct mm_struct *const mm;
        struct file *const file; /* May vary from vm_file in stacked callers. */
        unsigned long start;
        unsigned long end;

        /* Mutable fields. Populated with initial state. */
        pgoff_t pgoff;
        struct file *vm_file;
        vma_flags_t vma_flags;
        pgprot_t page_prot;

        /* Write-only fields. */
        const struct vm_operations_struct *vm_ops;
        void *private_data;

        /* Take further action? */
        struct mmap_action action;
    };

This is straightforward - you have all the fields you need to set up the
mapping, and you can update the mutable and writable fields, for instance:
(这很直截了当——你拥有设置映射所需的全部字段，并且可以更新其中的可变和可写字段，例如：)

.. code-block:: C

    static int ext4_file_mmap_prepare(struct vm_area_desc *desc)
    {
        int ret;
        struct file *file = desc->file;
        struct inode *inode = file->f_mapping->host;

        ...

        file_accessed(file);
        if (IS_DAX(file_inode(file))) {
            desc->vm_ops = &ext4_dax_vm_ops;
            vma_desc_set_flags(desc, VMA_HUGEPAGE_BIT);
        } else {
            desc->vm_ops = &ext4_file_vm_ops;
        }
        return 0;
    }

Importantly, you no longer have to dance around with reference counts or locks
when updating these fields - **you can simply go ahead and change them**.
(重要的是，在更新这些字段时，你不再需要围着引用计数或锁打转——**你可以直接修改它们**。)

Everything is taken care of by the mapping code.
(一切都由映射代码处理好了。)

VMA Flags
(VMA 标志)
---------

Along with ``mmap_prepare``, VMA flags have undergone an overhaul. Where before
you would invoke one of vm_flags_init(), vm_flags_reset(), vm_flags_set(),
vm_flags_clear(), and vm_flags_mod() to modify flags (and to have the
locking done correctly for you, this is no longer necessary.
(与 ``mmap_prepare`` 一起，VMA 标志也经历了一次大修。以前你需要调用 vm_flags_init()、vm_flags_reset()、vm_flags_set()、vm_flags_clear() 和 vm_flags_mod() 之一来修改标志(并让加锁操作正确地为你完成)，现在这已不再必要。)

Also, the legacy approach of specifying VMA flags via ``VM_READ``, ``VM_WRITE``,
etc. - i.e. using a ``-VM_xxx``- macro has changed too.
(此外，通过 ``VM_READ``、``VM_WRITE`` 等来指定 VMA 标志的老式做法——即使用 ``-VM_xxx``- 宏——也发生了变化。)

When implementing mmap_prepare(), reference flags by their bit number, defined
as a ``VMA_xxx_BIT`` macro, e.g. ``VMA_READ_BIT``, ``VMA_WRITE_BIT`` etc.,
and use one of (where ``desc`` is a pointer to struct vm_area_desc):
(在实现 mmap_prepare() 时，请通过位号来引用标志，位号定义为 ``VMA_xxx_BIT`` 宏，例如 ``VMA_READ_BIT``、``VMA_WRITE_BIT`` 等，并使用以下函数之一(其中 ``desc`` 是指向 struct vm_area_desc 的指针)：)

* ``vma_desc_test_any(desc, ...)`` - Specify a comma-separated list of flags
  you wish to test for (whether _any_ are set), e.g. - ``vma_desc_test_any(
  desc, VMA_WRITE_BIT, VMA_MAYWRITE_BIT)`` - returns ``true`` if either are set,
  otherwise ``false``.
  (``vma_desc_test_any(desc, ...)``——指定一个以逗号分隔的、你想要测试的标志列表(测试是否设置了_任意一个_)，例如——``vma_desc_test_any(desc, VMA_WRITE_BIT, VMA_MAYWRITE_BIT)``——如果其中任一标志已设置则返回 ``true``，否则返回 ``false``。)
* ``vma_desc_set_flags(desc, ...)`` - Update the VMA descriptor flags to set
  additional flags specified by a comma-separated list,
  e.g. - ``vma_desc_set_flags(desc, VMA_PFNMAP_BIT, VMA_IO_BIT)``.
  (``vma_desc_set_flags(desc, ...)``——更新 VMA 描述符标志，以设置由逗号分隔列表所指定的额外标志，例如——``vma_desc_set_flags(desc, VMA_PFNMAP_BIT, VMA_IO_BIT)``。)
* ``vma_desc_clear_flags(desc, ...)`` - Update the VMA descriptor flags to clear
  flags specified by a comma-separated list, e.g. - ``vma_desc_clear_flags(
  desc, VMA_WRITE_BIT, VMA_MAYWRITE_BIT)``.
  (``vma_desc_clear_flags(desc, ...)``——更新 VMA 描述符标志，以清除由逗号分隔列表所指定的标志，例如——``vma_desc_clear_flags(desc, VMA_WRITE_BIT, VMA_MAYWRITE_BIT)``。)

Actions
(操作)
=======

You can now very easily have actions be performed upon a mapping once set up by
utilising simple helper functions invoked upon the struct vm_area_desc
pointer. These are:
(现在，通过调用作用于 struct vm_area_desc 指针上的简单辅助函数，你可以在映射建立之后非常容易地对其执行操作。这些函数是：)

* mmap_action_remap() - Remaps a range consisting only of PFNs for a specific
  range starting a virtual address and PFN number of a set size.
  (mmap_action_remap()——对一个仅由 PFN 组成的范围进行重映射，该范围从给定的虚拟地址和 PFN 号开始，长度为设定的大小。)

* mmap_action_remap_full() - Same as mmap_action_remap(), only remaps the
  entire mapping from ``start_pfn`` onward.
  (mmap_action_remap_full()——与 mmap_action_remap() 相同，只不过重映射从 ``start_pfn`` 开始的整个映射。)

* mmap_action_ioremap() - Same as mmap_action_remap(), only performs an I/O
  remap.
  (mmap_action_ioremap()——与 mmap_action_remap() 相同，只不过执行的是 I/O 重映射。)

* mmap_action_ioremap_full() - Same as mmap_action_ioremap(), only remaps
  the entire mapping from ``start_pfn`` onward.
  (mmap_action_ioremap_full()——与 mmap_action_ioremap() 相同，只不过重映射从 ``start_pfn`` 开始的整个映射。)

* mmap_action_simple_ioremap() - Sets up an I/O remap from a specified
  physical address and over a specified length.
  (mmap_action_simple_ioremap()——从给定的物理地址开始、跨越给定的长度设置一个 I/O 重映射。)

* mmap_action_map_kernel_pages() - Maps a specified array of `struct page`
  pointers in the VMA from a specific offset.
  (mmap_action_map_kernel_pages()——在 VMA 中从指定偏移量开始映射给定的 `struct page` 指针数组。)

* mmap_action_map_kernel_pages_full() - Maps a specified array of `struct
  page` pointers over the entire VMA. The caller must ensure there are
  sufficient entries in the page array to cover the entire range of the
  described VMA.
  (mmap_action_map_kernel_pages_full()——在整个 VMA 上映射给定的 `struct page` 指针数组。调用者必须确保页数组中有足够的表项来覆盖所描述 VMA 的整个范围。)

**NOTE:** The ``action`` field should never normally be manipulated directly,
rather you ought to use one of these helpers.
(**注意：**通常绝不应直接操纵 ``action`` 字段，你应该使用这些辅助函数之一。)
