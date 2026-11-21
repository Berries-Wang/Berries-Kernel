.. SPDX-License-Identifier: GPL-2.0

==========================
Page Table Check(页表检查)
==========================

Introduction(简介)
==================

Page table check allows to harden the kernel by ensuring that some types of
the memory corruptions are prevented.
(页表检查 (page table check) 通过确保阻止某些类型的内存损坏来强化内
核。)

Page table check performs extra verifications at the time when new pages become
accessible from the userspace by getting their page table entries (PTEs PMDs
etc.) added into the table.
(页表检查在新的页面通过把其页表项 (PTE、PMD 等) 加入页表而对用户空间
变得可访问的时刻，执行额外的验证。)

In case of most detected corruption, the kernel is crashed. There is a small
performance and memory overhead associated with the page table check. Therefore,
it is disabled by default, but can be optionally enabled on systems where the
extra hardening outweighs the performance costs. Also, because page table check
is synchronous, it can help with debugging double map memory corruption issues,
by crashing kernel at the time wrong mapping occurs instead of later which is
often the case with memory corruptions bugs.
(对于检测到的大多数损坏，内核会崩溃。页表检查会带来一定的性能与内存
开销。因此它默认被禁用，但在额外加固的收益超过性能代价的系统上可以
选择启用。此外，由于页表检查是同步的，它有助于调试重复映射 (double
map) 内存损坏问题——在错误映射发生时立即让内核崩溃，而不是像内存损
坏 bug 中常见的那样延后发生。)

It can also be used to do page table entry checks over various flags, dump
warnings when illegal combinations of entry flags are detected.  Currently,
userfaultfd is the only user of such to sanity check wr-protect bit against
any writable flags.  Illegal flag combinations will not directly cause data
corruption in this case immediately, but that will cause read-only data to
be writable, leading to corrupt when the page content is later modified.
(它还可用于对各种标志执行页表项检查，在检测到非法的项标志组合时输出
警告。目前，userfaultfd 是此功能的唯一使用者，用于对照任何可写标志对
wr-protect 位进行健全性检查。在这种情况下，非法标志组合不会立即直接
导致数据损坏，但会使只读数据变为可写，从而在该页内容稍后被修改时导
致损坏。)

Double mapping detection logic(重复映射检测逻辑)
================================================

+-------------------+-------------------+-------------------+------------------+
| Current Mapping   | New mapping       | Permissions       | Rule             |
| (当前映射)        | (新映射)          | (权限)            | (规则)           |
+===================+===================+===================+==================+
| Anonymous         | Anonymous         | Read              | Allow            |
| (匿名)            | (匿名)            | (读)              | (允许)           |
+-------------------+-------------------+-------------------+------------------+
| Anonymous         | Anonymous         | Read / Write      | Prohibit         |
| (匿名)            | (匿名)            | (读/写)           | (禁止)           |
+-------------------+-------------------+-------------------+------------------+
| Anonymous         | Named             | Any               | Prohibit         |
| (匿名)            | (命名)            | (任意)            | (禁止)           |
+-------------------+-------------------+-------------------+------------------+
| Named             | Anonymous         | Any               | Prohibit         |
| (命名)            | (匿名)            | (任意)            | (禁止)           |
+-------------------+-------------------+-------------------+------------------+
| Named             | Named             | Any               | Allow            |
| (命名)            | (命名)            | (任意)            | (允许)           |
+-------------------+-------------------+-------------------+------------------+

Enabling Page Table Check(启用页表检查)
=======================================

Build kernel with:
(使用如下方式构建内核：)

- PAGE_TABLE_CHECK=y
  Note, it can only be enabled on platforms where ARCH_SUPPORTS_PAGE_TABLE_CHECK
  is available.
  (PAGE_TABLE_CHECK=y。注意，只能在 ARCH_SUPPORTS_PAGE_TABLE_CHECK 可
  用的平台上启用。)

- Boot with 'page_table_check=on' kernel parameter.
  (以 'page_table_check=on' 内核参数启动。)

Optionally, build kernel with PAGE_TABLE_CHECK_ENFORCED in order to have page
table support without extra kernel parameter.
(可选地，使用 PAGE_TABLE_CHECK_ENFORCED 构建内核，以便无需额外的内核
参数即可获得页表支持。)

Implementation notes(实现说明)
==============================

We specifically decided not to use VMA information in order to avoid relying on
MM states (except for limited "struct page" info). The page table check is a
separate from Linux-MM state machine that verifies that the user accessible
pages are not falsely shared.
(我们特意决定不使用 VMA 信息，以避免依赖 MM 状态 (仅使用有限的
"struct page" 信息)。页表检查是一个独立于 Linux-MM 的状态机，它验证
用户可访问的页面没有被错误地共享。)

PAGE_TABLE_CHECK depends on EXCLUSIVE_SYSTEM_RAM. The reason is that without
EXCLUSIVE_SYSTEM_RAM, users are allowed to map arbitrary physical memory
regions into the userspace via /dev/mem. At the same time, pages may change
their properties (e.g., from anonymous pages to named pages) while they are
still being mapped in the userspace, leading to "corruption" detected by the
page table check.
(PAGE_TABLE_CHECK 依赖于 EXCLUSIVE_SYSTEM_RAM。原因在于：如果没有
EXCLUSIVE_SYSTEM_RAM，用户可以通过 /dev/mem 把任意物理内存区域映射到
用户空间。同时，页面在仍被映射到用户空间期间可能改变其属性 (例如从
匿名页变为命名页)，从而导致页表检查检测到"损坏"。)

Even with EXCLUSIVE_SYSTEM_RAM, I/O pages may be still allowed to be mapped via
/dev/mem. However, these pages are always considered as named pages, so they
won't break the logic used in the page table check.
(即使有 EXCLUSIVE_SYSTEM_RAM，I/O 页面可能仍然允许通过 /dev/mem 映射。
不过，这些页面总是被视为命名页，因此不会破坏页表检查所使用的逻辑。)
