.. SPDX-License-Identifier: GPL-2.0

==================
Page Cache(页缓存)
==================

The page cache is the primary way that the user and the rest of the kernel
interact with filesystems.  It can be bypassed (e.g. with O_DIRECT),
but normal reads, writes and mmaps go through the page cache.
(页缓存 (page cache) 是用户和内核其他部分与文件系统交互的主要途径。它可以被绕过 (例如使用 O_DIRECT)，但正常的读、写和 mmap 都会经过页缓存。)

Folios(页簇)
============

The folio is the unit of memory management within the page cache.
Operations
(folio 是页缓存内的内存管理单元。)
