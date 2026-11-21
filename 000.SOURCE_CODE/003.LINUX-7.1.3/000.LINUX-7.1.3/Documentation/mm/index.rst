=============================================
Memory Management Documentation(内存管理文档)
=============================================

This is a guide to understanding the memory management subsystem
of Linux.  If you are looking for advice on simply allocating memory,
see the :ref:`memory_allocation`.  For controlling and tuning guides,
see the :doc:`admin guide <../admin-guide/mm/index>`.
(本文档帮助读者了解 Linux 的内存管理子系统。如果你只是想查找如何简单地分配
内存的建议，请参阅 :ref:`memory_allocation` 。有关控制与调优方面的指南，
请参阅 :doc:`管理指南 <../admin-guide/mm/index>` 。)

.. toctree::
   :maxdepth: 1

   physical_memory
   page_tables
   process_addrs
   bootmem
   page_allocation
   vmalloc
   slab
   highmem
   page_reclaim
   swap
   swap-table
   page_cache
   shmfs
   oom

Unsorted Documentation(未整理的文档)
====================================

This is a collection of unsorted documents about the Linux memory management
(MM) subsystem internals with different level of details ranging from notes and
mailing list responses for elaborating descriptions of data structures and
algorithms.  It should all be integrated nicely into the above structured
documentation, or deleted if it has served its purpose.
(这里汇集了关于 Linux 内存管理 (MM) 子系统内部实现的未整理文档，详细程度不一，
既有简短笔记和邮件列表回复，也有对数据结构与算法的详尽描述。这些内容最终都应
妥善整合进上述结构化文档中，或者在其使命完成之后删除。)

.. toctree::
   :maxdepth: 1

   active_mm
   allocation-profiling
   arch_pgtable_helpers
   balance
   damon/index
   free_page_reporting
   hmm
   hwpoison
   hugetlbfs_reserv
   ksm
   memory-model
   memfd_preservation
   mmu_notifier
   multigen_lru
   numa
   overcommit-accounting
   page_migration
   page_frags
   page_owner
   page_table_check
   remap_file_pages
   split_page_table_lock
   transhuge
   unevictable-lru
   vmalloced-kernel-stacks
   vmemmap_dedup
   zsmalloc
