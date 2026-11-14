.. SPDX-License-Identifier: GPL-2.0

===================================
File management in the Linux kernel
===================================

(Linux 内核中的文件管理)

This document describes how locking for files (struct file)
and file descriptor table (struct files) works.
(本文档描述针对文件(struct file)和文件描述符表(struct files)的锁是如何工作的。)

Up until 2.6.12, the file descriptor table has been protected
with a lock (files->file_lock) and reference count (files->count).
->file_lock protected accesses to all the file related fields
of the table. ->count was used for sharing the file descriptor
table between tasks cloned with CLONE_FILES flag. Typically
this would be the case for posix threads. As with the common
refcounting model in the kernel, the last task doing
a put_files_struct() frees the file descriptor (fd) table.
The files (struct file) themselves are protected using
reference count (->f_count).
(在 2.6.12 之前，文件描述符表由一把锁(files->file_lock)和引用计数(files->count)保护。->file_lock 保护对该表所有文件相关字段的访问。->count 用于在使用 CLONE_FILES 标志克隆的任务之间共享文件描述符表。posix 线程通常就是这种情况。与内核中常见的引用计数模型一样，最后一个执行 put_files_struct() 的任务会释放文件描述符(fd)表。文件(struct file)本身则使用引用计数(->f_count)来保护。)

In the new lock-free model of file descriptor management,
the reference counting is similar, but the locking is
based on RCU. The file descriptor table contains multiple
elements - the fd sets (open_fds and close_on_exec, the
array of file pointers, the sizes of the sets and the array
etc.). In order for the updates to appear atomic to
a lock-free reader, all the elements of the file descriptor
table are in a separate structure - struct fdtable.
files_struct contains a pointer to struct fdtable through
which the actual fd table is accessed. Initially the
fdtable is embedded in files_struct itself. On a subsequent
expansion of fdtable, a new fdtable structure is allocated
and files->fdtab points to the new structure. The fdtable
structure is freed with RCU and lock-free readers either
see the old fdtable or the new fdtable making the update
appear atomic. Here are the locking rules for
the fdtable structure -
(在新的无锁文件描述符管理模型中，引用计数是类似的，但加锁基于 RCU。文件描述符表包含多个元素——fd 集合(open_fds 和 close_on_exec)、文件指针数组、集合的大小以及数组等等。为了让更新在无锁读取者看来是原子的，文件描述符表的所有元素都放在一个单独的结构中——struct fdtable。files_struct 包含一个指向 struct fdtable 的指针，实际的 fd 表就是通过它访问的。起初 fdtable 内嵌在 files_struct 自身之中。随后当 fdtable 扩展时，会分配一个新的 fdtable 结构，并且 files->fdtab 指向新结构。fdtable 结构通过 RCU 释放，无锁读取者要么看到旧的 fdtable，要么看到新的 fdtable，从而使更新看起来是原子的。以下是 fdtable 结构的加锁规则——)

1. All references to the fdtable must be done through
   the files_fdtable() macro::

	struct fdtable *fdt;

	rcu_read_lock();

	fdt = files_fdtable(files);
	....
	if (n <= fdt->max_fds)
		....
	...
	rcu_read_unlock();

   (对 fdtable 的所有引用都必须通过 files_fdtable() 宏进行：)

   files_fdtable() uses rcu_dereference() macro which takes care of
   the memory barrier requirements for lock-free dereference.
   The fdtable pointer must be read within the read-side
   critical section.
   (files_fdtable() 使用 rcu_dereference() 宏，它负责处理无锁解引用所需的内存屏障。必须在读侧临界区内读取 fdtable 指针。)

2. Reading of the fdtable as described above must be protected
   by rcu_read_lock()/rcu_read_unlock().
   (如上所述对 fdtable 的读取必须由 rcu_read_lock()/rcu_read_unlock() 保护。)

3. For any update to the fd table, files->file_lock must
   be held.
   (对 fd 表的任何更新都必须持有 files->file_lock。)

4. To look up the file structure given an fd, a reader
   must use either lookup_fdget_rcu() or files_lookup_fdget_rcu() APIs. These
   take care of barrier requirements due to lock-free lookup.
   (要根据给定的 fd 查找文件结构，读取者必须使用 lookup_fdget_rcu() 或 files_lookup_fdget_rcu() API。它们负责处理无锁查找带来的屏障需求。)

   An example::

	struct file *file;

	rcu_read_lock();
	file = lookup_fdget_rcu(fd);
	rcu_read_unlock();
	if (file) {
		...
                fput(file);
	}
	....

   (一个示例：)

5. Since both fdtable and file structures can be looked up
   lock-free, they must be installed using rcu_assign_pointer()
   API. If they are looked up lock-free, rcu_dereference()
   must be used. However it is advisable to use files_fdtable()
   and lookup_fdget_rcu()/files_lookup_fdget_rcu() which take care of these
   issues.
   (由于 fdtable 和 file 结构都可以无锁查找，因此必须使用 rcu_assign_pointer() API 来安装它们。如果要无锁查找它们，则必须使用 rcu_dereference()。不过，建议使用会处理这些问题的 files_fdtable() 和 lookup_fdget_rcu()/files_lookup_fdget_rcu()。)

6. While updating, the fdtable pointer must be looked up while
   holding files->file_lock. If ->file_lock is dropped, then
   another thread expand the files thereby creating a new
   fdtable and making the earlier fdtable pointer stale.
   (更新期间，必须在持有 files->file_lock 的情况下查找 fdtable 指针。如果 ->file_lock 被释放，那么另一个线程可能扩展这些文件，从而创建一个新的 fdtable，并使先前的 fdtable 指针失效。)

   For example::

	spin_lock(&files->file_lock);
	fd = locate_fd(files, file, start);
	if (fd >= 0) {
		/* locate_fd() may have expanded fdtable, load the ptr */
		fdt = files_fdtable(files);
		__set_open_fd(fd, fdt);
		__clear_close_on_exec(fd, fdt);
		spin_unlock(&files->file_lock);
	.....

   (例如：)

   Since locate_fd() can drop ->file_lock (and reacquire ->file_lock),
   the fdtable pointer (fdt) must be loaded after locate_fd().
   (由于 locate_fd() 可能释放 ->file_lock(并重新获取 ->file_lock)，因此必须在 locate_fd() 之后加载 fdtable 指针(fdt)。)

On newer kernels rcu based file lookup has been switched to rely on
SLAB_TYPESAFE_BY_RCU instead of call_rcu(). It isn't sufficient anymore
to just acquire a reference to the file in question under rcu using
atomic_long_inc_not_zero() since the file might have already been
recycled and someone else might have bumped the reference. In other
words, callers might see reference count bumps from newer users. For
this is reason it is necessary to verify that the pointer is the same
before and after the reference count increment. This pattern can be seen
in get_file_rcu() and __files_get_rcu().
(在较新的内核中，基于 RCU 的文件查找已改为依赖 SLAB_TYPESAFE_BY_RCU 而不是 call_rcu()。仅在 rcu 下使用 atomic_long_inc_not_zero() 获取对相关文件的引用已经不再足够，因为该文件可能已经被回收，而其他人可能已经增加了引用。换句话说，调用者可能会看到来自更新使用者的引用计数增加。因此，有必要验证指针在引用计数递增之前和之后是否相同。这种模式可以在 get_file_rcu() 和 __files_get_rcu() 中看到。)

In addition, it isn't possible to access or check fields in struct file
without first acquiring a reference on it under rcu lookup. Not doing
that was always very dodgy and it was only usable for non-pointer data
in struct file. With SLAB_TYPESAFE_BY_RCU it is necessary that callers
either first acquire a reference or they must hold the files_lock of the
fdtable.
(此外，在 rcu 查找下未先获取引用之前，不可能访问或检查 struct file 中的字段。不这样做一直以来都是非常不可靠的，而且它只适用于 struct file 中的非指针数据。使用 SLAB_TYPESAFE_BY_RCU 时，调用者必须先获取一个引用，或者必须持有该 fdtable 的 files_lock。)
