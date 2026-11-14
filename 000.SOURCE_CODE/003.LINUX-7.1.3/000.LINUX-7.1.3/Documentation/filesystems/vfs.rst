.. SPDX-License-Identifier: GPL-2.0

=========================================
Overview of the Linux Virtual File System
=========================================

(Linux 虚拟文件系统概述)

Original author: Richard Gooch <rgooch@atnf.csiro.au>
(原作者: Richard Gooch <rgooch@atnf.csiro.au>)

- Copyright (C) 1999 Richard Gooch
  (版权所有 (C) 1999 Richard Gooch)
- Copyright (C) 2005 Pekka Enberg
  (版权所有 (C) 2005 Pekka Enberg)


Introduction
(引言)
============

The Virtual File System (also known as the Virtual Filesystem Switch) is
the software layer in the kernel that provides the filesystem interface
to userspace programs.  It also provides an abstraction within the
kernel which allows different filesystem implementations to coexist.
(虚拟文件系统(VFS，也称为虚拟文件系统交换器，Virtual Filesystem Switch)是内核中向用户空间程序提供文件系统接口的软件层。它还在内核内部提供了一个抽象，使不同的文件系统实现得以共存。)

VFS system calls open(2), stat(2), read(2), write(2), chmod(2) and so on
are called from a process context.  Filesystem locking is described in
the document Documentation/filesystems/locking.rst.
(VFS 系统调用 open(2)、stat(2)、read(2)、write(2)、chmod(2) 等都在进程上下文中被调用。文件系统的锁机制在文档 Documentation/filesystems/locking.rst 中描述。)


Directory Entry Cache (dcache)
(目录项缓存(dcache))
------------------------------

The VFS implements the open(2), stat(2), chmod(2), and similar system
calls.  The pathname argument that is passed to them is used by the VFS
to search through the directory entry cache (also known as the dentry
cache or dcache).  This provides a very fast look-up mechanism to
translate a pathname (filename) into a specific dentry.  Dentries live
in RAM and are never saved to disc: they exist only for performance.
(VFS 实现 open(2)、stat(2)、chmod(2) 以及类似的系统调用。传给这些调用的路径名参数被 VFS 用来在目录项缓存(也称为 dentry 缓存或 dcache)中查找。这提供了一种非常快速的查找机制，把路径名(文件名)转换为特定的 dentry。dentry 驻留在内存(RAM)中，从不保存到磁盘：它们的存在只是为了性能。)

The dentry cache is meant to be a view into your entire filespace.  As
most computers cannot fit all dentries in the RAM at the same time, some
bits of the cache are missing.  In order to resolve your pathname into a
dentry, the VFS may have to resort to creating dentries along the way,
and then loading the inode.  This is done by looking up the inode.
(dentry 缓存本应是整个文件空间的视图。由于大多数计算机无法同时把所有 dentry 都装进内存(RAM)，缓存中总会缺少一些部分。为了把你的路径名解析成 dentry，VFS 可能不得不沿途创建 dentry，然后再加载 inode。这是通过查找 inode 来完成的。)


The Inode Object
(inode 对象)
----------------

An individual dentry usually has a pointer to an inode.  Inodes are
filesystem objects such as regular files, directories, FIFOs and other
beasts.  They live either on the disc (for block device filesystems) or
in the memory (for pseudo filesystems).  Inodes that live on the disc
are copied into the memory when required and changes to the inode are
written back to disc.  A single inode can be pointed to by multiple
dentries (hard links, for example, do this).
(每个 dentry 通常有一个指向 inode 的指针。inode 是文件系统对象，例如普通文件、目录、FIFO 以及其他类型。它们或者驻留在磁盘上(对于块设备文件系统)，或者驻留在内存中(对于伪文件系统)。驻留在磁盘上的 inode 在需要时被复制进内存，对 inode 的修改会被写回磁盘。一个 inode 可以被多个 dentry 指向(例如硬链接就是这样)。)

To look up an inode requires that the VFS calls the lookup() method of
the parent directory inode.  This method is installed by the specific
filesystem implementation that the inode lives in.  Once the VFS has the
required dentry (and hence the inode), we can do all those boring things
like open(2) the file, or stat(2) it to peek at the inode data.  The
stat(2) operation is fairly simple: once the VFS has the dentry, it
peeks at the inode data and passes some of it back to userspace.
(查找一个 inode 需要 VFS 调用父目录 inode 的 lookup() 方法。该方法由 inode 所在的具体文件系统实现安装。一旦 VFS 拿到了所需的 dentry(进而也拿到了 inode)，我们就可以做那些乏味的事情了，比如 open(2) 这个文件，或者对它执行 stat(2) 来查看 inode 数据。stat(2) 操作相当简单：一旦 VFS 拿到 dentry，它就查看 inode 数据并把其中一部分传回用户空间。)


The File Object
(文件对象)
---------------

Opening a file requires another operation: allocation of a file
structure (this is the kernel-side implementation of file descriptors).
The freshly allocated file structure is initialized with a pointer to
the dentry and a set of file operation member functions.  These are
taken from the inode data.  The open() file method is then called so the
specific filesystem implementation can do its work.  You can see that
this is another switch performed by the VFS.  The file structure is
placed into the file descriptor table for the process.
(打开一个文件还需要另一个操作：分配一个 file 结构(这是文件描述符在内核侧的实现)。新分配的 file 结构用一个指向 dentry 的指针和一组文件操作成员函数来初始化。这些内容取自 inode 数据。随后调用 open() 文件方法，让具体文件系统实现完成其工作。可以看到，这是 VFS 执行的又一次分派。file 结构被放入该进程的文件描述符表中。)

Reading, writing and closing files (and other assorted VFS operations)
is done by using the userspace file descriptor to grab the appropriate
file structure, and then calling the required file structure method to
do whatever is required.  For as long as the file is open, it keeps the
dentry in use, which in turn means that the VFS inode is still in use.
(读、写和关闭文件(以及其他各种 VFS 操作)的做法是：用用户空间的文件描述符获取相应的 file 结构，然后调用所需的 file 结构方法来完成要求的工作。只要文件保持打开状态，它就使其 dentry 一直处于使用中，这又意味着 VFS inode 也仍处于使用中。)


Registering and Mounting a Filesystem
(注册与挂载文件系统)
=====================================

To register and unregister a filesystem, use the following API
functions:
(要注册和注销一个文件系统，请使用以下 API 函数：)

.. code-block:: c

	#include <linux/fs.h>

	extern int register_filesystem(struct file_system_type *);
	extern int unregister_filesystem(struct file_system_type *);

The passed struct file_system_type describes your filesystem.  When a
request is made to mount a filesystem onto a directory in your
namespace, the VFS will call the appropriate get_tree() method for the
specific filesystem.  See Documentation/filesystems/mount_api.rst
for more details.
(传入的 struct file_system_type 描述你的文件系统。当请求把一个文件系统挂载到你的命名空间中的某个目录上时，VFS 会为该具体文件系统调用相应的 get_tree() 方法。更多细节请参见 Documentation/filesystems/mount_api.rst。)

You can see all filesystems that are registered to the kernel in the
file /proc/filesystems.
(你可以在文件 /proc/filesystems 中看到所有已注册到内核的文件系统。)


struct file_system_type
-----------------------

This describes the filesystem.  The following
members are defined:
(这里描述文件系统。其中定义了以下成员：)

.. code-block:: c

	struct file_system_type {
		const char *name;
		int fs_flags;
		int (*init_fs_context)(struct fs_context *);
		const struct fs_parameter_spec *parameters;
		void (*kill_sb) (struct super_block *);
		struct module *owner;
		struct file_system_type * next;
		struct hlist_head fs_supers;

		struct lock_class_key s_lock_key;
		struct lock_class_key s_umount_key;
		struct lock_class_key s_vfs_rename_key;
		struct lock_class_key s_writers_key[SB_FREEZE_LEVELS];

		struct lock_class_key i_lock_key;
		struct lock_class_key i_mutex_key;
		struct lock_class_key invalidate_lock_key;
		struct lock_class_key i_mutex_dir_key;
	};

``name``
	the name of the filesystem type, such as "ext2", "iso9660",
	"msdos" and so on
	(文件系统类型的名称，例如 "ext2"、"iso9660"、"msdos" 等等)

``fs_flags``
	various flags (i.e. FS_REQUIRES_DEV, FS_NO_DCACHE, etc.)
	(各种标志(即 FS_REQUIRES_DEV、FS_NO_DCACHE 等))

``init_fs_context``
	Initializes 'struct fs_context' ->ops and ->fs_private fields with
	filesystem-specific data.
	(用文件系统特定的数据初始化 'struct fs_context' 的 ->ops 和 ->fs_private 字段。)

``parameters``
	Pointer to the array of filesystem parameters descriptors
	'struct fs_parameter_spec'.
	More info in Documentation/filesystems/mount_api.rst.
	(指向文件系统参数描述符数组 'struct fs_parameter_spec' 的指针。更多信息见 Documentation/filesystems/mount_api.rst。)

``kill_sb``
	the method to call when an instance of this filesystem should be
	shut down
	(当该文件系统的一个实例需要被关闭时调用的方法)


``owner``
	for internal VFS use: you should initialize this to THIS_MODULE
	in most cases.
	(供 VFS 内部使用：大多数情况下你应将其初始化为 THIS_MODULE。)

``next``
	for internal VFS use: you should initialize this to NULL
	(供 VFS 内部使用：你应将其初始化为 NULL)

``fs_supers``
	for internal VFS use: hlist of filesystem instances (superblocks)
	(供 VFS 内部使用：文件系统实例(超级块)的 hlist)

  s_lock_key, s_umount_key, s_vfs_rename_key, s_writers_key,
  i_lock_key, i_mutex_key, invalidate_lock_key, i_mutex_dir_key: lockdep-specific
  (以上各锁类键为 lockdep 专用)

The Superblock Object
(超级块对象)
=====================

A superblock object represents a mounted filesystem.
(超级块对象表示一个已挂载的文件系统。)


struct super_operations
-----------------------

This describes how the VFS can manipulate the superblock of your
filesystem.  The following members are defined:
(这里描述 VFS 如何操作你的文件系统的超级块。其中定义了以下成员：)

.. code-block:: c

	struct super_operations {
		struct inode *(*alloc_inode)(struct super_block *sb);
		void (*destroy_inode)(struct inode *);
		void (*free_inode)(struct inode *);

		void (*dirty_inode) (struct inode *, int flags);
		int (*write_inode) (struct inode *, struct writeback_control *wbc);
		int (*drop_inode) (struct inode *);
		void (*evict_inode) (struct inode *);
		void (*put_super) (struct super_block *);
		int (*sync_fs)(struct super_block *sb, int wait);
		int (*freeze_super) (struct super_block *sb,
					enum freeze_holder who);
		int (*freeze_fs) (struct super_block *);
		int (*thaw_super) (struct super_block *sb,
					enum freeze_wholder who);
		int (*unfreeze_fs) (struct super_block *);
		int (*statfs) (struct dentry *, struct kstatfs *);
		void (*umount_begin) (struct super_block *);

		int (*show_options)(struct seq_file *, struct dentry *);
		int (*show_devname)(struct seq_file *, struct dentry *);
		int (*show_path)(struct seq_file *, struct dentry *);
		int (*show_stats)(struct seq_file *, struct dentry *);

		ssize_t (*quota_read)(struct super_block *, int, char *, size_t, loff_t);
		ssize_t (*quota_write)(struct super_block *, int, const char *, size_t, loff_t);
		struct dquot **(*get_dquots)(struct inode *);

		long (*nr_cached_objects)(struct super_block *,
					struct shrink_control *);
		long (*free_cached_objects)(struct super_block *,
					struct shrink_control *);
	};

All methods are called without any locks being held, unless otherwise
noted.  This means that most methods can block safely.  All methods are
only called from a process context (i.e. not from an interrupt handler
or bottom half).
(除非另有说明，所有方法都在不持有任何锁的情况下被调用。这意味着大多数方法可以安全地阻塞。所有方法只在进程上下文中被调用(即不会从中断处理程序或下半部中调用)。)

``alloc_inode``
	this method is called by alloc_inode() to allocate memory for
	struct inode and initialize it.  If this function is not
	defined, a simple 'struct inode' is allocated.  Normally
	alloc_inode will be used to allocate a larger structure which
	contains a 'struct inode' embedded within it.
	(该方法由 alloc_inode() 调用，用于为 struct inode 分配内存并初始化它。如果未定义此函数，则分配一个简单的 'struct inode'。通常会用 alloc_inode 来分配一个更大的结构，其中内嵌一个 'struct inode'。)

``destroy_inode``
	this method is called by destroy_inode() to release resources
	allocated for struct inode.  It is only required if
	->alloc_inode was defined and simply undoes anything done by
	->alloc_inode.
	(该方法由 destroy_inode() 调用，用于释放为 struct inode 分配的资源。只有在定义了 ->alloc_inode 时才需要它，它只是撤销 ->alloc_inode 所做的一切。)

``free_inode``
	this method is called from RCU callback. If you use call_rcu()
	in ->destroy_inode to free 'struct inode' memory, then it's
	better to release memory in this method.
	(该方法从 RCU 回调中调用。如果你在 ->destroy_inode 中使用 call_rcu() 来释放 'struct inode' 内存，那么最好在这个方法中释放内存。)

``dirty_inode``
	this method is called by the VFS when an inode is marked dirty.
	This is specifically for the inode itself being marked dirty,
	not its data.  If the update needs to be persisted by fdatasync(),
	then I_DIRTY_DATASYNC will be set in the flags argument.
	I_DIRTY_TIME will be set in the flags in case lazytime is enabled
	and struct inode has times updated since the last ->dirty_inode
	call.
	(当 inode 被标记为脏时 VFS 会调用此方法。这专门针对 inode 本身被标记为脏，而不是它的数据。如果该更新需要通过 fdatasync() 持久化，则 flags 参数中会设置 I_DIRTY_DATASYNC。如果启用了 lazytime 并且 struct inode 自上次 ->dirty_inode 调用以来更新了时间，则 flags 中会设置 I_DIRTY_TIME。)

``write_inode``
	this method is called when the VFS needs to write an inode to
	disc.  The second parameter indicates whether the write should
	be synchronous or not, not all filesystems check this flag.
	(当 VFS 需要把一个 inode 写入磁盘时调用此方法。第二个参数指示写入是否应为同步；并非所有文件系统都检查此标志。)

``drop_inode``
	called when the last access to the inode is dropped, with the
	inode->i_lock spinlock held.
	(在对该 inode 的最后一次访问被放下时调用，调用时持有 inode->i_lock 自旋锁。)

	This method should be either NULL (normal UNIX filesystem
	semantics) or "inode_just_drop" (for filesystems that do
	not want to cache inodes - causing "delete_inode" to always be
	called regardless of the value of i_nlink)
	(此方法应为 NULL(普通 UNIX 文件系统语义)或 "inode_just_drop"(用于不想缓存 inode 的文件系统——这会导致无论 i_nlink 的值如何都总是调用 "delete_inode"))

	The "inode_just_drop()" behavior is equivalent to the old
	practice of using "force_delete" in the put_inode() case, but
	does not have the races that the "force_delete()" approach had.
	("inode_just_drop()" 的行为等同于过去在 put_inode() 中使用 "force_delete" 的做法，但没有 "force_delete()" 方式所存在的竞态问题。)

``evict_inode``
	called when the VFS wants to evict an inode. Caller does
	*not* evict the pagecache or inode-associated metadata buffers;
	the method has to use truncate_inode_pages_final() to get rid
	of those. Caller makes sure async writeback cannot be running for
	the inode while (or after) ->evict_inode() is called. Optional.
	(当 VFS 想要逐出一个 inode 时调用。调用者 *不会* 逐出页缓存或与 inode 关联的元数据缓冲；该方法必须使用 truncate_inode_pages_final() 来清除它们。调用者确保在 ->evict_inode() 被调用期间(或之后)不会有针对该 inode 的异步回写正在运行。可选。)

``put_super``
	called when the VFS wishes to free the superblock
	(i.e. unmount).  This is called with the superblock lock held
	(当 VFS 希望释放超级块时调用(即卸载)。调用时持有超级块锁)

``sync_fs``
	called when VFS is writing out all dirty data associated with a
	superblock.  The second parameter indicates whether the method
	should wait until the write out has been completed.  Optional.
	(当 VFS 要写出与某个超级块关联的所有脏数据时调用。第二个参数指示该方法是否应等待写出完成。可选。)

``freeze_super``
	Called instead of ->freeze_fs callback if provided.
	Main difference is that ->freeze_super is called without taking
	down_write(&sb->s_umount). If filesystem implements it and wants
	->freeze_fs to be called too, then it has to call ->freeze_fs
	explicitly from this callback. Optional.
	(如果提供了此回调，则调用它而不是 ->freeze_fs。主要区别在于 ->freeze_super 被调用时不获取 down_write(&sb->s_umount)。如果文件系统实现了它并希望 ->freeze_fs 也被调用，那么它必须在这个回调中显式调用 ->freeze_fs。可选。)

``freeze_fs``
	called when VFS is locking a filesystem and forcing it into a
	consistent state.  This method is currently used by the Logical
	Volume Manager (LVM) and ioctl(FIFREEZE). Optional.
	(当 VFS 锁定一个文件系统并强制其进入一致状态时调用。目前逻辑卷管理器(LVM)和 ioctl(FIFREEZE) 使用此方法。可选。)

``thaw_super``
	called when VFS is unlocking a filesystem and making it writable
	again after ->freeze_super. Optional.
	(在 ->freeze_super 之后，当 VFS 解锁一个文件系统并使其恢复可写时调用。可选。)

``unfreeze_fs``
	called when VFS is unlocking a filesystem and making it writable
	again after ->freeze_fs. Optional.
	(在 ->freeze_fs 之后，当 VFS 解锁一个文件系统并使其恢复可写时调用。可选。)

``statfs``
	called when the VFS needs to get filesystem statistics.
	(当 VFS 需要获取文件系统统计信息时调用。)

``umount_begin``
	called when the VFS is unmounting a filesystem.
	(当 VFS 正在卸载一个文件系统时调用。)

``show_options``
	called by the VFS to show mount options for /proc/<pid>/mounts
	and /proc/<pid>/mountinfo.
	(see "Mount Options" section)
	(由 VFS 调用，为 /proc/<pid>/mounts 和 /proc/<pid>/mountinfo 显示挂载选项。(参见 "Mount Options" 一节))

``show_devname``
	Optional. Called by the VFS to show device name for
	/proc/<pid>/{mounts,mountinfo,mountstats}. If not provided then
	'(struct mount).mnt_devname' will be used.
	(可选。由 VFS 调用，为 /proc/<pid>/{mounts,mountinfo,mountstats} 显示设备名。如果未提供，则将使用 '(struct mount).mnt_devname'。)

``show_path``
	Optional. Called by the VFS (for /proc/<pid>/mountinfo) to show
	the mount root dentry path relative to the filesystem root.
	(可选。由 VFS 调用(用于 /proc/<pid>/mountinfo)，显示相对于文件系统根的挂载根 dentry 路径。)

``show_stats``
	Optional. Called by the VFS (for /proc/<pid>/mountstats) to show
	filesystem-specific mount statistics.
	(可选。由 VFS 调用(用于 /proc/<pid>/mountstats)，显示文件系统特定的挂载统计信息。)

``quota_read``
	called by the VFS to read from filesystem quota file.
	(由 VFS 调用，从文件系统配额文件中读取。)

``quota_write``
	called by the VFS to write to filesystem quota file.
	(由 VFS 调用，写入文件系统配额文件。)

``get_dquots``
	called by quota to get 'struct dquot' array for a particular inode.
	Optional.
	(由配额子系统调用，获取特定 inode 的 'struct dquot' 数组。可选。)

``nr_cached_objects``
	called by the sb cache shrinking function for the filesystem to
	return the number of freeable cached objects it contains.
	Optional.
	(由 sb 缓存收缩函数调用，让文件系统返回其包含的可释放缓存对象的数量。可选。)

``free_cache_objects``
	called by the sb cache shrinking function for the filesystem to
	scan the number of objects indicated to try to free them.
	Optional, but any filesystem implementing this method needs to
	also implement ->nr_cached_objects for it to be called
	correctly.
	(由 sb 缓存收缩函数调用，让文件系统扫描指定数量的对象并尝试释放它们。可选，但任何实现此方法的文件系统还必须实现 ->nr_cached_objects，此方法才能被正确调用。)

	We can't do anything with any errors that the filesystem might
	encountered, hence the void return type.  This will never be
	called if the VM is trying to reclaim under GFP_NOFS conditions,
	hence this method does not need to handle that situation itself.
	(对于文件系统可能遇到的任何错误我们都无能为力，因此返回类型为 void。当 VM 在 GFP_NOFS 条件下尝试回收时，此方法绝不会被调用，因此该方法本身不需要处理那种情况。)

	Implementations must include conditional reschedule calls inside
	any scanning loop that is done.  This allows the VFS to
	determine appropriate scan batch sizes without having to worry
	about whether implementations will cause holdoff problems due to
	large scan batch sizes.
	(实现必须在所执行的任何扫描循环内包含条件化的重新调度调用。这使 VFS 能够确定合适的扫描批大小，而不必担心实现是否会因扫描批过大而导致停顿问题。)

Whoever sets up the inode is responsible for filling in the "i_op"
field.  This is a pointer to a "struct inode_operations" which describes
the methods that can be performed on individual inodes.
(无论谁建立起 inode，都由它负责填写 "i_op" 字段。这是一个指向 "struct inode_operations" 的指针，后者描述可以对单个 inode 执行的方法。)


struct xattr_handler
---------------------

On filesystems that support extended attributes (xattrs), the s_xattr
superblock field points to a NULL-terminated array of xattr handlers.
Extended attributes are name:value pairs.
(在支持扩展属性(xattr)的文件系统上，超级块的 s_xattr 字段指向一个以 NULL 结尾的 xattr 处理程序数组。扩展属性是名字:值对。)

``name``
	Indicates that the handler matches attributes with the specified
	name (such as "system.posix_acl_access"); the prefix field must
	be NULL.
	(表示该处理程序匹配具有指定名字的属性(例如 "system.posix_acl_access")；prefix 字段必须为 NULL。)

``prefix``
	Indicates that the handler matches all attributes with the
	specified name prefix (such as "user."); the name field must be
	NULL.
	(表示该处理程序匹配所有具有指定名字前缀的属性(例如 "user.")；name 字段必须为 NULL。)

``list``
	Determine if attributes matching this xattr handler should be
	listed for a particular dentry.  Used by some listxattr
	implementations like generic_listxattr.
	(判断与该 xattr 处理程序匹配的属性是否应针对特定 dentry 列出。被某些 listxattr 实现使用，例如 generic_listxattr。)

``get``
	Called by the VFS to get the value of a particular extended
	attribute.  This method is called by the getxattr(2) system
	call.
	(由 VFS 调用，获取某个特定扩展属性的值。此方法由 getxattr(2) 系统调用调用。)

``set``
	Called by the VFS to set the value of a particular extended
	attribute.  When the new value is NULL, called to remove a
	particular extended attribute.  This method is called by the
	setxattr(2) and removexattr(2) system calls.
	(由 VFS 调用，设置某个特定扩展属性的值。当新值为 NULL 时，调用它来移除某个特定扩展属性。此方法由 setxattr(2) 和 removexattr(2) 系统调用调用。)

When none of the xattr handlers of a filesystem match the specified
attribute name or when a filesystem doesn't support extended attributes,
the various ``*xattr(2)`` system calls return -EOPNOTSUPP.
(当文件系统的所有 xattr 处理程序都不匹配指定的属性名，或文件系统不支持扩展属性时，各种 ``*xattr(2)`` 系统调用返回 -EOPNOTSUPP。)


The Inode Object
(inode 对象)
================

An inode object represents an object within the filesystem.
(inode 对象表示文件系统内的一个对象。)


struct inode_operations
-----------------------

This describes how the VFS can manipulate an inode in your filesystem.
As of kernel 2.6.22, the following members are defined:
(这里描述 VFS 如何操作你的文件系统中的一个 inode。自内核 2.6.22 起，定义了以下成员：)

.. code-block:: c

	struct inode_operations {
		int (*create) (struct mnt_idmap *, struct inode *,struct dentry *, umode_t, bool);
		struct dentry * (*lookup) (struct inode *,struct dentry *, unsigned int);
		int (*link) (struct dentry *,struct inode *,struct dentry *);
		int (*unlink) (struct inode *,struct dentry *);
		int (*symlink) (struct mnt_idmap *, struct inode *,struct dentry *,const char *);
		struct dentry *(*mkdir) (struct mnt_idmap *, struct inode *,struct dentry *,umode_t);
		int (*rmdir) (struct inode *,struct dentry *);
		int (*mknod) (struct mnt_idmap *, struct inode *,struct dentry *,umode_t,dev_t);
		int (*rename) (struct mnt_idmap *, struct inode *, struct dentry *,
			       struct inode *, struct dentry *, unsigned int);
		int (*readlink) (struct dentry *, char __user *,int);
		const char *(*get_link) (struct dentry *, struct inode *,
					 struct delayed_call *);
		int (*permission) (struct mnt_idmap *, struct inode *, int);
		struct posix_acl * (*get_inode_acl)(struct inode *, int, bool);
		int (*setattr) (struct mnt_idmap *, struct dentry *, struct iattr *);
		int (*getattr) (struct mnt_idmap *, const struct path *, struct kstat *, u32, unsigned int);
		ssize_t (*listxattr) (struct dentry *, char *, size_t);
		void (*update_time)(struct inode *inode, enum fs_update_time type,
				    int flags);
		void (*sync_lazytime)(struct inode *inode);
		int (*atomic_open)(struct inode *, struct dentry *, struct file *,
				   unsigned open_flag, umode_t create_mode);
		int (*tmpfile) (struct mnt_idmap *, struct inode *, struct file *, umode_t);
		struct posix_acl * (*get_acl)(struct mnt_idmap *, struct dentry *, int);
	        int (*set_acl)(struct mnt_idmap *, struct dentry *, struct posix_acl *, int);
		int (*fileattr_set)(struct mnt_idmap *idmap,
				    struct dentry *dentry, struct file_kattr *fa);
		int (*fileattr_get)(struct dentry *dentry, struct file_kattr *fa);
	        struct offset_ctx *(*get_offset_ctx)(struct inode *inode);
	};

Again, all methods are called without any locks being held, unless
otherwise noted.
(同样，除非另有说明，所有方法都在不持有任何锁的情况下被调用。)

``create``
	called by the open(2) and creat(2) system calls.  Only required
	if you want to support regular files.  The dentry you get should
	not have an inode (i.e. it should be a negative dentry).  Here
	you will probably call d_instantiate() with the dentry and the
	newly created inode
	(由 open(2) 和 creat(2) 系统调用调用。只有当你想支持普通文件时才需要。你拿到的 dentry 不应有 inode(即它应是一个负 dentry)。在这里你可能会用该 dentry 和新创建的 inode 调用 d_instantiate())

``lookup``
	called when the VFS needs to look up an inode in a parent
	directory.  The name to look for is found in the dentry.  This
	method must call d_add() to insert the found inode into the
	dentry.  The "i_count" field in the inode structure should be
	incremented.  If the named inode does not exist a NULL inode
	should be inserted into the dentry (this is called a negative
	dentry).  Returning an error code from this routine must only be
	done on a real error, otherwise creating inodes with system
	calls like create(2), mknod(2), mkdir(2) and so on will fail.
	If you wish to overload the dentry methods then you should
	initialise the "d_dop" field in the dentry; this is a pointer to
	a struct "dentry_operations".  This method is called with the
	directory inode semaphore held
	(当 VFS 需要在父目录中查找一个 inode 时调用。要查找的名字可在 dentry 中找到。此方法必须调用 d_add() 把找到的 inode 插入 dentry。inode 结构中的 "i_count" 字段应当递增。如果同名 inode 不存在，则应把一个 NULL inode 插入 dentry(这称为负 dentry)。只有真正出错时才能从这个例程返回错误码，否则用 create(2)、mknod(2)、mkdir(2) 等系统调用创建 inode 将会失败。如果你想重载 dentry 方法，那么你应初始化 dentry 中的 "d_dop" 字段；它是一个指向 struct "dentry_operations" 的指针。调用此方法时持有目录 inode 信号量)

``link``
	called by the link(2) system call.  Only required if you want to
	support hard links.  You will probably need to call
	d_instantiate() just as you would in the create() method
	(由 link(2) 系统调用调用。只有当你想支持硬链接时才需要。你可能需要调用 d_instantiate()，就像在 create() 方法中那样)

``unlink``
	called by the unlink(2) system call.  Only required if you want
	to support deleting inodes
	(由 unlink(2) 系统调用调用。只有当你想支持删除 inode 时才需要)

``symlink``
	called by the symlink(2) system call.  Only required if you want
	to support symlinks.  You will probably need to call
	d_instantiate() just as you would in the create() method
	(由 symlink(2) 系统调用调用。只有当你想支持符号链接时才需要。你可能需要调用 d_instantiate()，就像在 create() 方法中那样)

``mkdir``
	called by the mkdir(2) system call.  Only required if you want
	to support creating subdirectories.  You will probably need to
	call d_instantiate_new() just as you would in the create() method.
	(由 mkdir(2) 系统调用调用。只有当你想支持创建子目录时才需要。你可能需要调用 d_instantiate_new()，就像在 create() 方法中那样。)

	If d_instantiate_new() is not used and if the fh_to_dentry()
	export operation is provided, or if the storage might be
	accessible by another path (e.g. with a network filesystem)
	then more care may be needed.  Importantly d_instantate()
	should not be used with an inode that is no longer I_NEW if there
	any chance that the inode could already be attached to a dentry.
	This is because of a hard rule in the VFS that a directory must
	only ever have one dentry.
	(如果不使用 d_instantiate_new()，且提供了 fh_to_dentry() 导出操作，或者存储可能通过另一条路径访问(例如网络文件系统)，那么可能需要更加小心。重要的是，如果该 inode 有任何可能已经连接到某个 dentry，就不应对不再是 I_NEW 状态的 inode 使用 d_instantate()。这是因为 VFS 有一条硬性规则：一个目录只能有一个 dentry。)

	For example, if an NFS filesystem is mounted twice the new directory
	could be visible on the other mount before it is on the original
	mount, and a pair of name_to_handle_at(), open_by_handle_at()
	calls could instantiate the directory inode with an IS_ROOT()
	dentry before the first mkdir returns.
	(例如，如果同一个 NFS 文件系统被挂载了两次，新目录可能先在另一个挂载上可见，然后才在原始挂载上可见，并且在第一个 mkdir 返回之前，一对 name_to_handle_at()、open_by_handle_at() 调用就可能用一个 IS_ROOT() dentry 实例化该目录 inode。)

	If there is any chance this could happen, then the new inode
	should be d_drop()ed and attached with d_splice_alias().  The
	returned dentry (if any) should be returned by ->mkdir().
	(如果有任何可能发生这种情况，那么应对新 inode 执行 d_drop() 并用 d_splice_alias() 连接。->mkdir() 应返回所得到的 dentry(如果有的话)。)

``rmdir``
	called by the rmdir(2) system call.  Only required if you want
	to support deleting subdirectories
	(由 rmdir(2) 系统调用调用。只有当你想支持删除子目录时才需要)

``mknod``
	called by the mknod(2) system call to create a device (char,
	block) inode or a named pipe (FIFO) or socket.  Only required if
	you want to support creating these types of inodes.  You will
	probably need to call d_instantiate() just as you would in the
	create() method
	(由 mknod(2) 系统调用调用，用于创建设备(字符、块)inode、命名管道(FIFO)或套接字。只有当你想支持创建这些类型的 inode 时才需要。你可能需要调用 d_instantiate()，就像在 create() 方法中那样)

``rename``
	called by the rename(2) system call to rename the object to have
	the parent and name given by the second inode and dentry.
	(由 rename(2) 系统调用调用，把对象重命名为由第二个 inode 和 dentry 给出的父目录和名字。)

	The filesystem must return -EINVAL for any unsupported or
	unknown flags.  Currently the following flags are implemented:
	(1) RENAME_NOREPLACE: this flag indicates that if the target of
	the rename exists the rename should fail with -EEXIST instead of
	replacing the target.  The VFS already checks for existence, so
	for local filesystems the RENAME_NOREPLACE implementation is
	equivalent to plain rename.
	(2) RENAME_EXCHANGE: exchange source and target.  Both must
	exist; this is checked by the VFS.  Unlike plain rename, source
	and target may be of different type.
	(文件系统对任何不支持或未知的标志都必须返回 -EINVAL。目前实现了以下标志：(1) RENAME_NOREPLACE：此标志表示如果重命名的目标已存在，则重命名应以 -EEXIST 失败，而不是替换目标。VFS 已经检查过存在性，因此对本地文件系统而言，RENAME_NOREPLACE 的实现等同于普通重命名。(2) RENAME_EXCHANGE：交换源和目标。两者都必须存在；这由 VFS 检查。与普通重命名不同，源和目标的类型可以不同。)

``get_link``
	called by the VFS to follow a symbolic link to the inode it
	points to.  Only required if you want to support symbolic links.
	This method returns the symlink body to traverse (and possibly
	resets the current position with nd_jump_link()).  If the body
	won't go away until the inode is gone, nothing else is needed;
	if it needs to be otherwise pinned, arrange for its release by
	having get_link(..., ..., done) do set_delayed_call(done,
	destructor, argument).  In that case destructor(argument) will
	be called once VFS is done with the body you've returned.  May
	be called in RCU mode; that is indicated by NULL dentry
	argument.  If request can't be handled without leaving RCU mode,
	have it return ERR_PTR(-ECHILD).
	(由 VFS 调用，跟随符号链接到达它所指向的 inode。只有当你想支持符号链接时才需要。此方法返回要遍历的符号链接体(并可能通过 nd_jump_link() 重置当前位置)。如果该链接体在 inode 消失之前不会消失，则无需其他操作；如果它需要以其他方式被固定，应安排其释放，方法是让 get_link(..., ..., done) 执行 set_delayed_call(done, destructor, argument)。在这种情况下，一旦 VFS 用完你返回的链接体，就会调用 destructor(argument)。可能在 RCU 模式下被调用；这由 NULL 的 dentry 参数指示。如果请求无法在不离开 RCU 模式的情况下处理，让它返回 ERR_PTR(-ECHILD)。)

	If the filesystem stores the symlink target in ->i_link, the
	VFS may use it directly without calling ->get_link(); however,
	->get_link() must still be provided.  ->i_link must not be
	freed until after an RCU grace period.  Writing to ->i_link
	post-iget() time requires a 'release' memory barrier.
	(如果文件系统把符号链接目标存储在 ->i_link 中，VFS 可以直接使用它而无需调用 ->get_link()；但仍然必须提供 ->get_link()。->i_link 必须在经过一个 RCU 宽限期之后才能被释放。在 iget() 之后写入 ->i_link 需要 'release' 内存屏障。)

``readlink``
	this is now just an override for use by readlink(2) for the
	cases when ->get_link uses nd_jump_link() or object is not in
	fact a symlink.  Normally filesystems should only implement
	->get_link for symlinks and readlink(2) will automatically use
	that.
	(现在它只是供 readlink(2) 使用的一个覆盖项，用于 ->get_link 使用 nd_jump_link() 或对象实际上不是符号链接的情形。通常文件系统应只为符号链接实现 ->get_link，readlink(2) 会自动使用它。)

``permission``
	called by the VFS to check for access rights on a POSIX-like
	filesystem.
	(由 VFS 调用，在类 POSIX 文件系统上检查访问权限。)

	May be called in rcu-walk mode (mask & MAY_NOT_BLOCK).  If in
	rcu-walk mode, the filesystem must check the permission without
	blocking or storing to the inode.
	(可能在 rcu-walk 模式下被调用(mask & MAY_NOT_BLOCK)。在 rcu-walk 模式下，文件系统必须在不阻塞、不向 inode 存储的情况下检查权限。)

	If a situation is encountered that rcu-walk cannot handle,
	return
	-ECHILD and it will be called again in ref-walk mode.
	(如果遇到 rcu-walk 无法处理的情况，返回 -ECHILD，它将以 ref-walk 模式被再次调用。)

``setattr``
	called by the VFS to set attributes for a file.  This method is
	called by chmod(2) and related system calls.
	(由 VFS 调用，设置文件的属性。此方法由 chmod(2) 及相关系统调用调用。)

``getattr``
	called by the VFS to get attributes of a file.  This method is
	called by stat(2) and related system calls.
	(由 VFS 调用，获取文件的属性。此方法由 stat(2) 及相关系统调用调用。)

``listxattr``
	called by the VFS to list all extended attributes for a given
	file.  This method is called by the listxattr(2) system call.
	(由 VFS 调用，列出一个给定文件的所有扩展属性。此方法由 listxattr(2) 系统调用调用。)

``update_time``
	called by the VFS to update a specific time or the i_version of
	an inode.  If this is not defined the VFS will update the inode
	itself and call mark_inode_dirty_sync.
	(由 VFS 调用，更新 inode 的某个特定时间或 i_version。如果未定义此方法，VFS 将自行更新 inode 并调用 mark_inode_dirty_sync。)

``sync_lazytime``:
	called by the writeback code to update the lazy time stamps to
	regular time stamp updates that get syncing into the on-disk
	inode.
	(由回写代码调用，把惰性时间戳更新转换为常规时间戳更新，进而同步到磁盘上的 inode。)

``atomic_open``
	called on the last component of an open.  Using this optional
	method the filesystem can look up, possibly create and open the
	file in one atomic operation.  If it wants to leave actual
	opening to the caller (e.g. if the file turned out to be a
	symlink, device, or just something filesystem won't do atomic
	open for), it may signal this by returning finish_no_open(file,
	dentry).  This method is only called if the last component is
	negative or needs lookup.  Cached positive dentries are still
	handled by f_op->open().  If the file was created, FMODE_CREATED
	flag should be set in file->f_mode.  In case of O_EXCL the
	method must only succeed if the file didn't exist and hence
	FMODE_CREATED shall always be set on success.
	(在打开路径的最后一个分量上调用。使用这个可选方法，文件系统可以在一次原子操作中查找、可能创建并打开文件。如果它想把实际打开留给调用者(例如文件结果是符号链接、设备，或文件系统不愿对其做原子打开的东西)，它可以通过返回 finish_no_open(file, dentry) 来表明这一点。只有在最后一个分量为负或需要查找时才会调用此方法。缓存的正 dentry 仍由 f_op->open() 处理。如果文件被创建，应在 file->f_mode 中设置 FMODE_CREATED 标志。在 O_EXCL 的情况下，只有当文件原本不存在时该方法才允许成功，因此成功时必须总是设置 FMODE_CREATED。)

``tmpfile``
	called in the end of O_TMPFILE open().  Optional, equivalent to
	atomically creating, opening and unlinking a file in given
	directory.  On success needs to return with the file already
	open; this can be done by calling finish_open_simple() right at
	the end.
	(在 O_TMPFILE open() 的最后调用。可选，等同于在给定目录中原子的创建、打开并解除链接一个文件。成功时需要在返回时文件已经打开；这可以通过在最后调用 finish_open_simple() 来完成。)

``fileattr_get``
	called on ioctl(FS_IOC_GETFLAGS) and ioctl(FS_IOC_FSGETXATTR) to
	retrieve miscellaneous file flags and attributes.  Also called
	before the relevant SET operation to check what is being changed
	(in this case with i_rwsem locked exclusive).  If unset, then
	fall back to f_op->ioctl().
	(在 ioctl(FS_IOC_GETFLAGS) 和 ioctl(FS_IOC_FSGETXATTR) 时调用，用于获取各种杂项文件标志和属性。在相应的 SET 操作之前也会被调用，以检查要更改的内容(此时以独占方式锁定 i_rwsem)。如果未设置，则回退到 f_op->ioctl()。)

``fileattr_set``
	called on ioctl(FS_IOC_SETFLAGS) and ioctl(FS_IOC_FSSETXATTR) to
	change miscellaneous file flags and attributes.  Callers hold
	i_rwsem exclusive.  If unset, then fall back to f_op->ioctl().
	(在 ioctl(FS_IOC_SETFLAGS) 和 ioctl(FS_IOC_FSSETXATTR) 时调用，用于更改各种杂项文件标志和属性。调用者以独占方式持有 i_rwsem。如果未设置，则回退到 f_op->ioctl()。)
``get_offset_ctx``
	called to get the offset context for a directory inode. A
        filesystem must define this operation to use
        simple_offset_dir_operations.
        (调用它以获取目录 inode 的偏移上下文(offset context)。文件系统必须定义此操作才能使用 simple_offset_dir_operations。)

The Address Space Object
(地址空间对象)
========================

The address space object is used to group and manage pages in the page
cache.  It can be used to keep track of the pages in a file (or anything
else) and also track the mapping of sections of the file into process
address spaces.
(地址空间对象用于对页缓存中的页进行分组和管理。它可以用来跟踪一个文件(或任何其他东西)中的页，也可以跟踪文件各部分到进程地址空间的映射。)

There are a number of distinct yet related services that an
address-space can provide.  These include communicating memory pressure,
page lookup by address, and keeping track of pages tagged as Dirty or
Writeback.
(地址空间可以提供若干不同却又相关的服务。这些服务包括传递内存压力、按地址查找页，以及跟踪被标记为 Dirty 或 Writeback 的页。)

The first can be used independently to the others.  The VM can try to
release clean pages in order to reuse them.  To do this it can call
->release_folio on clean folios with the private
flag set.  Clean pages without PagePrivate and with no external references
will be released without notice being given to the address_space.
(第一项服务可以独立于其他服务使用。VM 可以尝试释放干净页以便重用它们。为此，它可以对设置了 private 标志的干净 folio 调用 ->release_folio。没有 PagePrivate 且没有外部引用的干净页将被直接释放，而不会通知 address_space。)

To achieve this functionality, pages need to be placed on an LRU with
lru_cache_add and mark_page_active needs to be called whenever the page
is used.
(为实现这一功能，页需要通过 lru_cache_add 放到 LRU 上，并且每当页被使用时都需要调用 mark_page_active。)

Pages are normally kept in a radix tree index by ->index.  This tree
maintains information about the PG_Dirty and PG_Writeback status of each
page, so that pages with either of these flags can be found quickly.
(页通常保存在一个以 ->index 为索引的 radix tree 中。该树维护每一页的 PG_Dirty 和 PG_Writeback 状态信息，从而可以快速找到带有其中任一标志的页。)

The Dirty tag is primarily used by mpage_writepages - the default
->writepages method.  It uses the tag to find dirty pages to
write back.  If mpage_writepages is not used (i.e. the address
provides its own ->writepages) , the PAGECACHE_TAG_DIRTY tag is almost
unused.  write_inode_now and sync_inode do use it (through
__sync_single_inode) to check if ->writepages has been successful in
writing out the whole address_space.
(Dirty 标签主要由 mpage_writepages——默认的 ->writepages 方法——使用。它利用该标签找到要回写的脏页。如果不使用 mpage_writepages(即地址空间提供自己的 ->writepages)，PAGECACHE_TAG_DIRTY 标签几乎不被使用。write_inode_now 和 sync_inode 确实会使用它(通过 __sync_single_inode)来检查 ->writepages 是否已成功写出整个 address_space。)

The Writeback tag is used by filemap*wait* and sync_page* functions, via
filemap_fdatawait_range, to wait for all writeback to complete.
(Writeback 标签由 filemap*wait* 和 sync_page* 函数通过 filemap_fdatawait_range 使用，用于等待所有回写完成。)

An address_space handler may attach extra information to a page,
typically using the 'private' field in the 'struct page'.  If such
information is attached, the PG_Private flag should be set.  This will
cause various VM routines to make extra calls into the address_space
handler to deal with that data.
(address_space 处理程序可以在页上附加额外信息，通常使用 'struct page' 中的 'private' 字段。如果附加了这样的信息，应设置 PG_Private 标志。这会导致各种 VM 例程向 address_space 处理程序发出额外的调用，以处理那些数据。)

An address space acts as an intermediate between storage and
application.  Data is read into the address space a whole page at a
time, and provided to the application either by copying of the page, or
by memory-mapping the page.  Data is written into the address space by
the application, and then written-back to storage typically in whole
pages, however the address_space has finer control of write sizes.
(地址空间充当存储与应用程序之间的中介。数据一次一整页地读入地址空间，并通过复制页或将页内存映射的方式提供给应用程序。数据由应用程序写入地址空间，然后通常以整页为单位写回存储，不过 address_space 对写入大小有更细的控制。)

The read process essentially only requires 'read_folio'.  The write
process is more complicated and uses write_begin/write_end or
dirty_folio to write data into the address_space, and
writepages to writeback data to storage.
(读过程基本上只需要 'read_folio'。写过程更复杂，它使用 write_begin/write_end 或 dirty_folio 将数据写入 address_space，并使用 writepages 将数据回写到存储。)

Removing pages from an address_space requires holding the inode's i_rwsem
exclusively, while adding pages to the address_space requires holding the
inode's i_mapping->invalidate_lock exclusively.
(从 address_space 中移除页需要以独占方式持有该 inode 的 i_rwsem，而向 address_space 添加页则需要以独占方式持有该 inode 的 i_mapping->invalidate_lock。)

When data is written to a page, the PG_Dirty flag should be set.  It
typically remains set until writepages asks for it to be written.  This
should clear PG_Dirty and set PG_Writeback.  It can be actually written
at any point after PG_Dirty is clear.  Once it is known to be safe,
PG_Writeback is cleared.
(当数据写入某一页时，应设置 PG_Dirty 标志。它通常会保持置位，直到 writepages 要求写该页为止。此时应清除 PG_Dirty 并设置 PG_Writeback。在 PG_Dirty 清除之后的任何时刻都可以实际执行写入。一旦确认安全，PG_Writeback 就会被清除。)

Writeback makes use of a writeback_control structure to direct the
operations.  This gives the writepages operation some
information about the nature of and reason for the writeback request,
and the constraints under which it is being done.  It is also used to
return information back to the caller about the result of a
writepages request.
(回写使用 writeback_control 结构来指导操作。这为 writepages 操作提供了关于回写请求的性质、原因以及执行时所受约束的一些信息。它还用于向调用者返回关于 writepages 请求结果的信息。)


Handling errors during writeback
(处理回写期间的错误)
--------------------------------

Most applications that do buffered I/O will periodically call a file
synchronization call (fsync, fdatasync, msync or sync_file_range) to
ensure that data written has made it to the backing store.  When there
is an error during writeback, they expect that error to be reported when
a file sync request is made.  After an error has been reported on one
request, subsequent requests on the same file descriptor should return
0, unless further writeback errors have occurred since the previous file
synchronization.
(大多数做缓冲 I/O 的应用程序都会周期性地调用文件同步调用(fsync、fdatasync、msync 或 sync_file_range)，以确保写入的数据已经到达后备存储。当回写期间发生错误时，它们期望在发起文件同步请求时报告该错误。在一个请求上报告过错误之后，对同一文件描述符的后续请求应返回 0，除非自上次文件同步以来又发生了新的回写错误。)

Ideally, the kernel would report errors only on file descriptions on
which writes were done that subsequently failed to be written back.  The
generic pagecache infrastructure does not track the file descriptions
that have dirtied each individual page however, so determining which
file descriptors should get back an error is not possible.
(理想情况下，内核只应在这样的文件描述(file description)上报告错误：在其上执行过的写入随后回写失败。然而，通用的页缓存基础设施并不跟踪弄脏了每个具体页的文件描述，因此无法确定哪些文件描述符应收到错误。)

Instead, the generic writeback error tracking infrastructure in the
kernel settles for reporting errors to fsync on all file descriptions
that were open at the time that the error occurred.  In a situation with
multiple writers, all of them will get back an error on a subsequent
fsync, even if all of the writes done through that particular file
descriptor succeeded (or even if there were no writes on that file
descriptor at all).
(作为替代，内核中通用的回写错误跟踪基础设施退而求其次：向错误发生时所有已打开的文件描述上的 fsync 报告错误。在存在多个写入者的情况下，它们都会在随后的 fsync 中收到错误，即使通过该特定文件描述符完成的所有写入都成功了(或者即使该文件描述符上根本没有发生过写入)。)

Filesystems that wish to use this infrastructure should call
mapping_set_error to record the error in the address_space when it
occurs.  Then, after writing back data from the pagecache in their
file->fsync operation, they should call file_check_and_advance_wb_err to
ensure that the struct file's error cursor has advanced to the correct
point in the stream of errors emitted by the backing device(s).
(希望使用该基础设施的文件系统应在错误发生时调用 mapping_set_error，把错误记录在 address_space 中。然后，在 file->fsync 操作中把数据从页缓存写回之后，它们应调用 file_check_and_advance_wb_err，以确保 struct file 的错误游标已前进到后备设备所发出错误流中的正确位置。)


struct address_space_operations
-------------------------------

This describes how the VFS can manipulate mapping of a file to page
cache in your filesystem.  The following members are defined:
(这里描述 VFS 如何操作你的文件系统中文件到页缓存的映射。其中定义了以下成员：)

.. code-block:: c

	struct address_space_operations {
		int (*read_folio)(struct file *, struct folio *);
		int (*writepages)(struct address_space *, struct writeback_control *);
		bool (*dirty_folio)(struct address_space *, struct folio *);
		void (*readahead)(struct readahead_control *);
		int (*write_begin)(const struct kiocb *, struct address_space *mapping,
				   loff_t pos, unsigned len,
				   struct page **pagep, void **fsdata);
		int (*write_end)(const struct kiocb *, struct address_space *mapping,
				 loff_t pos, unsigned len, unsigned copied,
				 struct folio *folio, void *fsdata);
		sector_t (*bmap)(struct address_space *, sector_t);
		void (*invalidate_folio) (struct folio *, size_t start, size_t len);
		bool (*release_folio)(struct folio *, gfp_t);
		void (*free_folio)(struct folio *);
		ssize_t (*direct_IO)(struct kiocb *, struct iov_iter *iter);
		int (*migrate_folio)(struct mapping *, struct folio *dst,
				struct folio *src, enum migrate_mode);
		int (*launder_folio) (struct folio *);

		bool (*is_partially_uptodate) (struct folio *, size_t from,
					       size_t count);
		void (*is_dirty_writeback)(struct folio *, bool *, bool *);
		int (*error_remove_folio)(struct mapping *mapping, struct folio *);
		int (*swap_activate)(struct swap_info_struct *sis, struct file *f, sector_t *span)
		int (*swap_deactivate)(struct file *);
		int (*swap_rw)(struct kiocb *iocb, struct iov_iter *iter);
	};

``read_folio``
	Called by the page cache to read a folio from the backing store.
	The 'file' argument supplies authentication information to network
	filesystems, and is generally not used by block based filesystems.
	It may be NULL if the caller does not have an open file (eg if
	the kernel is performing a read for itself rather than on behalf
	of a userspace process with an open file).
	(由页缓存调用，从后备存储读取一个 folio。'file' 参数为网络文件系统提供认证信息，基于块的文件系统通常不使用它。如果调用者没有打开的文件，它可以为 NULL(例如内核为自己执行读取，而不是代表一个打开了文件的用户空间进程执行读取)。)

	If the mapping does not support large folios, the folio will
	contain a single page.	The folio will be locked when read_folio
	is called.  If the read completes successfully, the folio should
	be marked uptodate.  The filesystem should unlock the folio
	once the read has completed, whether it was successful or not.
	The filesystem does not need to modify the refcount on the folio;
	the page cache holds a reference count and that will not be
	released until the folio is unlocked.
	(如果映射不支持大 folio，则 folio 将只包含一个页。调用 read_folio 时 folio 处于锁定状态。如果读取成功完成，folio 应被标记为 uptodate。无论读取成功与否，文件系统都应在读取完成后解锁 folio。文件系统不需要修改 folio 的引用计数；页缓存持有一个引用计数，在 folio 被解锁之前不会释放它。)

	Filesystems may implement ->read_folio() synchronously.
	In normal operation, folios are read through the ->readahead()
	method.  Only if this fails, or if the caller needs to wait for
	the read to complete will the page cache call ->read_folio().
	Filesystems should not attempt to perform their own readahead
	in the ->read_folio() operation.
	(文件系统可以同步实现 ->read_folio()。正常操作中，folio 通过 ->readahead() 方法读取。只有当该方法失败，或者调用者需要等待读取完成时，页缓存才会调用 ->read_folio()。文件系统不应在 ->read_folio() 操作中尝试执行自己的预读。)

	If the filesystem cannot perform the read at this time, it can
	unlock the folio, do whatever action it needs to ensure that the
	read will succeed in the future and return AOP_TRUNCATED_PAGE.
	In this case, the caller should look up the folio, lock it,
	and call ->read_folio again.
	(如果文件系统此时无法执行读取，它可以解锁 folio，执行任何需要的动作以确保读取将来会成功，并返回 AOP_TRUNCATED_PAGE。在这种情况下，调用者应查找该 folio、锁定它，并再次调用 ->read_folio。)

	Callers may invoke the ->read_folio() method directly, but using
	read_mapping_folio() will take care of locking, waiting for the
	read to complete and handle cases such as AOP_TRUNCATED_PAGE.
	(调用者可以直接调用 ->read_folio() 方法，但使用 read_mapping_folio() 将处理加锁、等待读取完成以及 AOP_TRUNCATED_PAGE 之类的情形。)

``writepages``
	called by the VM to write out pages associated with the
	address_space object.  If wbc->sync_mode is WB_SYNC_ALL, then
	the writeback_control will specify a range of pages that must be
	written out.  If it is WB_SYNC_NONE, then a nr_to_write is
	given and that many pages should be written if possible.  If no
	->writepages is given, then mpage_writepages is used instead.
	This will choose pages from the address space that are tagged as
	DIRTY and will write them back.
	(由 VM 调用，写出与 address_space 对象关联的页。如果 wbc->sync_mode 为 WB_SYNC_ALL，则 writeback_control 将指定一个必须写出的页范围。如果为 WB_SYNC_NONE，则给定一个 nr_to_write，应尽可能写出这么多页。如果没有提供 ->writepages，则使用 mpage_writepages 代替。它会从地址空间中选择被标记为 DIRTY 的页并将它们回写。)

``dirty_folio``
	called by the VM to mark a folio as dirty.  This is particularly
	needed if an address space attaches private data to a folio, and
	that data needs to be updated when a folio is dirtied.  This is
	called, for example, when a memory mapped page gets modified.
	If defined, it should set the folio dirty flag, and the
	PAGECACHE_TAG_DIRTY search mark in i_pages.
	(由 VM 调用，把一个 folio 标记为脏。如果地址空间在 folio 上附加了私有数据，并且 folio 变脏时这些数据需要更新，则尤其需要此方法。例如，当一个内存映射页被修改时会调用它。如果定义了此方法，它应设置 folio 脏标志以及 i_pages 中的 PAGECACHE_TAG_DIRTY 搜索标记。)

``readahead``
	Called by the VM to read pages associated with the address_space
	object.  The pages are consecutive in the page cache and are
	locked.  The implementation should decrement the page refcount
	after starting I/O on each page.  Usually the page will be
	unlocked by the I/O completion handler.  The set of pages are
	divided into some sync pages followed by some async pages,
	rac->ra->async_size gives the number of async pages.  The
	filesystem should attempt to read all sync pages but may decide
	to stop once it reaches the async pages.  If it does decide to
	stop attempting I/O, it can simply return.  The caller will
	remove the remaining pages from the address space, unlock them
	and decrement the page refcount.  Set PageUptodate if the I/O
	completes successfully.
	(由 VM 调用，读取与 address_space 对象关联的页。这些页在页缓存中是连续的且处于锁定状态。实现在对每一页发起 I/O 之后应递减该页的引用计数。通常页会由 I/O 完成处理程序解锁。这组页被分成一些同步页和其后的一些异步页，rac->ra->async_size 给出异步页的数量。文件系统应尝试读取所有同步页，但可以决定在到达异步页时停止。如果它确实决定停止发起 I/O，可以简单地返回。调用者将从地址空间中移除剩余的页、解锁它们并递减页引用计数。如果 I/O 成功完成，设置 PageUptodate。)

``write_begin``
	Called by the generic buffered write code to ask the filesystem
	to prepare to write len bytes at the given offset in the file.
	The address_space should check that the write will be able to
	complete, by allocating space if necessary and doing any other
	internal housekeeping.  If the write will update parts of any
	basic-blocks on storage, then those blocks should be pre-read
	(if they haven't been read already) so that the updated blocks
	can be written out properly.
	(由通用缓冲写代码调用，要求文件系统准备在文件中给定偏移处写入 len 字节。address_space 应通过在必要时分配空间以及做其他内部整理工作，来确认写入能够完成。如果写入将更新存储上某些基本块的部分内容，那么这些块应被预读(如果尚未读过)，以便更新后的块能够被正确写出。)

	The filesystem must return the locked pagecache folio for the
	specified offset, in ``*foliop``, for the caller to write into.
	(文件系统必须在 ``*foliop`` 中返回指定偏移处已锁定的页缓存 folio，供调用者写入。)

	It must be able to cope with short writes (where the length
	passed to write_begin is greater than the number of bytes copied
	into the folio).
	(它必须能够应对短写(即传给 write_begin 的长度大于复制进 folio 的字节数的情况)。)

	A void * may be returned in fsdata, which then gets passed into
	write_end.
	(可以在 fsdata 中返回一个 void *，它随后会被传入 write_end。)

	Returns 0 on success; < 0 on failure (which is the error code),
	in which case write_end is not called.
	(成功时返回 0；失败时返回 < 0(即错误码)，此时不会调用 write_end。)

``write_end``
	After a successful write_begin, and data copy, write_end must be
	called.  len is the original len passed to write_begin, and
	copied is the amount that was able to be copied.
	(在 write_begin 成功并且数据复制完成之后，必须调用 write_end。len 是传给 write_begin 的原始长度，copied 是实际能够复制的数量。)

	The filesystem must take care of unlocking the folio,
	decrementing its refcount, and updating i_size.
	(文件系统必须负责解锁 folio、递减其引用计数并更新 i_size。)

	Returns < 0 on failure, otherwise the number of bytes (<=
	'copied') that were able to be copied into pagecache.
	(失败时返回 < 0，否则返回能够复制进页缓存的字节数(<= 'copied')。)

``bmap``
	called by the VFS to map a logical block offset within object to
	physical block number.  This method is used by the FIBMAP ioctl
	and for working with swap-files.  To be able to swap to a file,
	the file must have a stable mapping to a block device.  The swap
	system does not go through the filesystem but instead uses bmap
	to find out where the blocks in the file are and uses those
	addresses directly.
	(由 VFS 调用，把对象内的逻辑块偏移映射为物理块号。FIBMAP ioctl 使用此方法，处理交换文件(swap-file)时也会用到。为了能够换出到文件，该文件必须具有到块设备的稳定映射。交换系统不经过文件系统，而是使用 bmap 找出文件中的块所在位置并直接使用那些地址。)

``invalidate_folio``
	If a folio has private data, then invalidate_folio will be
	called when part or all of the folio is to be removed from the
	address space.  This generally corresponds to either a
	truncation, punch hole or a complete invalidation of the address
	space (in the latter case 'offset' will always be 0 and 'length'
	will be folio_size()).  Any private data associated with the folio
	should be updated to reflect this truncation.  If offset is 0
	and length is folio_size(), then the private data should be
	released, because the folio must be able to be completely
	discarded.  This may be done by calling the ->release_folio
	function, but in this case the release MUST succeed.
	(如果 folio 带有私有数据，那么当该 folio 的部分或全部要从地址空间中移除时，会调用 invalidate_folio。这通常对应于截断、挖洞(punch hole)或对地址空间的完全失效(在后一种情况下 'offset' 总为 0，'length' 为 folio_size())。与该 folio 关联的任何私有数据都应更新以反映这次截断。如果 offset 为 0 且 length 为 folio_size()，则应释放私有数据，因为该 folio 必须能够被完全丢弃。这可以通过调用 ->release_folio 函数来完成，但在这种情况下释放必须成功。)

``release_folio``
	release_folio is called on folios with private data to tell the
	filesystem that the folio is about to be freed.  ->release_folio
	should remove any private data from the folio and clear the
	private flag.  If release_folio() fails, it should return false.
	release_folio() is used in two distinct though related cases.
	The first is when the VM wants to free a clean folio with no
	active users.  If ->release_folio succeeds, the folio will be
	removed from the address_space and be freed.
	(release_folio 在带有私有数据的 folio 上被调用，告诉文件系统该 folio 即将被释放。->release_folio 应从 folio 中移除任何私有数据并清除 private 标志。如果 release_folio() 失败，它应返回 false。release_folio() 用于两个不同却相关的场景。第一个是 VM 想释放一个没有活动用户的干净 folio。如果 ->release_folio 成功，该 folio 将从 address_space 中移除并被释放。)

	The second case is when a request has been made to invalidate
	some or all folios in an address_space.  This can happen
	through the fadvise(POSIX_FADV_DONTNEED) system call or by the
	filesystem explicitly requesting it as nfs and 9p do (when they
	believe the cache may be out of date with storage) by calling
	invalidate_inode_pages2().  If the filesystem makes such a call,
	and needs to be certain that all folios are invalidated, then
	its release_folio will need to ensure this.  Possibly it can
	clear the uptodate flag if it cannot free private data yet.
	(第二种情况是有人请求使 address_space 中的部分或全部 folio 失效。这可以通过 fadvise(POSIX_FADV_DONTNEED) 系统调用发生，也可以由文件系统显式请求(如 nfs 和 9p 那样，当它们认为缓存可能已与存储不一致时)通过调用 invalidate_inode_pages2() 完成。如果文件系统发起了这样的调用，并且需要确保所有 folio 都被失效，那么它的 release_folio 就需要保证这一点。如果它还不能释放私有数据，或许可以清除 uptodate 标志。)

``free_folio``
	free_folio is called once the folio is no longer visible in the
	page cache in order to allow the cleanup of any private data.
	Since it may be called by the memory reclaimer, it should not
	assume that the original address_space mapping still exists, and
	it should not block.
	(free_folio 在 folio 不再出现在页缓存中之后被调用，以便清理任何私有数据。由于它可能被内存回收器调用，它不应假设原来的 address_space 映射仍然存在，也不应阻塞。)

``direct_IO``
	called by the generic read/write routines to perform direct_IO -
	that is IO requests which bypass the page cache and transfer
	data directly between the storage and the application's address
	space.
	(由通用读写例程调用以执行 direct_IO——即绕过页缓存、在存储与应用程序地址空间之间直接传输数据的 I/O 请求。)

``migrate_folio``
	This is used to compact the physical memory usage.  If the VM
	wants to relocate a folio (maybe from a memory device that is
	signalling imminent failure) it will pass a new folio and an old
	folio to this function.  migrate_folio should transfer any private
	data across and update any references that it has to the folio.
	(它用于压缩物理内存的使用。如果 VM 想迁移一个 folio(可能来自一个正在报告即将发生故障的内存设备)，它会向此函数传入一个新 folio 和一个旧 folio。migrate_folio 应把所有私有数据迁移过去，并更新它持有的对该 folio 的所有引用。)

``launder_folio``
	Called before freeing a folio - it writes back the dirty folio.
	To prevent redirtying the folio, it is kept locked during the
	whole operation.
	(在释放 folio 之前调用——它回写脏 folio。为防止 folio 再次变脏，在整个操作期间它保持锁定。)

``is_partially_uptodate``
	Called by the VM when reading a file through the pagecache when
	the underlying blocksize is smaller than the size of the folio.
	If the required block is up to date then the read can complete
	without needing I/O to bring the whole page up to date.
	(当底层块大小小于 folio 大小时，VM 通过页缓存读取文件时会调用它。如果所需的块已是最新，则读取无需 I/O 把整页更新到最新即可完成。)

``is_dirty_writeback``
	Called by the VM when attempting to reclaim a folio.  The VM uses
	dirty and writeback information to determine if it needs to
	stall to allow flushers a chance to complete some IO.
	Ordinarily it can use folio_test_dirty and folio_test_writeback but
	some filesystems have more complex state (unstable folios in NFS
	prevent reclaim) or do not set those flags due to locking
	problems.  This callback allows a filesystem to indicate to the
	VM if a folio should be treated as dirty or writeback for the
	purposes of stalling.
	(VM 尝试回收一个 folio 时调用它。VM 利用脏和回写信息来判断是否需要停顿，以让刷写器有机会完成一些 I/O。通常它可以使用 folio_test_dirty 和 folio_test_writeback，但有些文件系统具有更复杂的状态(NFS 中的不稳定 folio 会阻止回收)，或者由于加锁问题而不设置那些标志。此回调允许文件系统向 VM 表明，就停顿而言一个 folio 是否应被视为脏或正在回写。)

``error_remove_folio``
	normally set to generic_error_remove_folio if truncation is ok
	for this address space.  Used for memory failure handling.
	Setting this implies you deal with pages going away under you,
	unless you have them locked or reference counts increased.
	(如果允许对此地址空间进行截断，通常将其设置为 generic_error_remove_folio。用于内存故障处理。设置它意味着你能够处理页在你脚下消失的情况，除非你锁住了它们或增加了它们的引用计数。)

``swap_activate``

	Called to prepare the given file for swap.  It should perform
	any validation and preparation necessary to ensure that writes
	can be performed with minimal memory allocation.  It should call
	add_swap_extent(), or the helper iomap_swapfile_activate(), and
	return the number of extents added.  If IO should be submitted
	through ->swap_rw(), it should set SWP_FS_OPS, otherwise IO will
	be submitted directly to the block device ``sis->bdev``.
	(调用它为给定文件准备交换空间。它应执行任何必要的验证和准备工作，以确保能够在最小内存分配的情况下执行写入。它应调用 add_swap_extent() 或辅助函数 iomap_swapfile_activate()，并返回添加的区段(extent)数量。如果 I/O 应通过 ->swap_rw() 提交，它应设置 SWP_FS_OPS，否则 I/O 将直接提交到块设备 ``sis->bdev``。)

``swap_deactivate``
	Called during swapoff on files where swap_activate was
	successful.
	(在 swapoff 期间对 swap_activate 成功的文件调用。)

``swap_rw``
	Called to read or write swap pages when SWP_FS_OPS is set.
	(当设置了 SWP_FS_OPS 时，调用它来读或写交换页。)

The File Object
(文件对象)
===============

A file object represents a file opened by a process.  This is also known
as an "open file description" in POSIX parlance.
(文件对象表示一个由进程打开的文件。在 POSIX 术语中，它也被称为 "open file description"(打开文件描述)。)


struct file_operations
----------------------

This describes how the VFS can manipulate an open file.  As of kernel
4.18, the following members are defined:
(这里描述 VFS 如何操作一个打开的文件。自内核 4.18 起，定义了以下成员：)

.. code-block:: c

	struct file_operations {
		struct module *owner;
		fop_flags_t fop_flags;
		loff_t (*llseek) (struct file *, loff_t, int);
		ssize_t (*read) (struct file *, char __user *, size_t, loff_t *);
		ssize_t (*write) (struct file *, const char __user *, size_t, loff_t *);
		ssize_t (*read_iter) (struct kiocb *, struct iov_iter *);
		ssize_t (*write_iter) (struct kiocb *, struct iov_iter *);
		int (*iopoll)(struct kiocb *kiocb, struct io_comp_batch *,
				unsigned int flags);
		int (*iterate_shared) (struct file *, struct dir_context *);
		__poll_t (*poll) (struct file *, struct poll_table_struct *);
		long (*unlocked_ioctl) (struct file *, unsigned int, unsigned long);
		long (*compat_ioctl) (struct file *, unsigned int, unsigned long);
		int (*mmap) (struct file *, struct vm_area_struct *);
		int (*open) (struct inode *, struct file *);
		int (*flush) (struct file *, fl_owner_t id);
		int (*release) (struct inode *, struct file *);
		int (*fsync) (struct file *, loff_t, loff_t, int datasync);
		int (*fasync) (int, struct file *, int);
		int (*lock) (struct file *, int, struct file_lock *);
		unsigned long (*get_unmapped_area)(struct file *, unsigned long, unsigned long, unsigned long, unsigned long);
		int (*check_flags)(int);
		int (*flock) (struct file *, int, struct file_lock *);
		ssize_t (*splice_write)(struct pipe_inode_info *, struct file *, loff_t *, size_t, unsigned int);
		ssize_t (*splice_read)(struct file *, loff_t *, struct pipe_inode_info *, size_t, unsigned int);
		void (*splice_eof)(struct file *file);
		int (*setlease)(struct file *, int, struct file_lease **, void **);
		long (*fallocate)(struct file *file, int mode, loff_t offset,
				  loff_t len);
		void (*show_fdinfo)(struct seq_file *m, struct file *f);
	#ifndef CONFIG_MMU
		unsigned (*mmap_capabilities)(struct file *);
	#endif
		ssize_t (*copy_file_range)(struct file *, loff_t, struct file *,
				loff_t, size_t, unsigned int);
		loff_t (*remap_file_range)(struct file *file_in, loff_t pos_in,
					   struct file *file_out, loff_t pos_out,
					   loff_t len, unsigned int remap_flags);
		int (*fadvise)(struct file *, loff_t, loff_t, int);
		int (*uring_cmd)(struct io_uring_cmd *ioucmd, unsigned int issue_flags);
		int (*uring_cmd_iopoll)(struct io_uring_cmd *, struct io_comp_batch *,
					unsigned int poll_flags);
		int (*mmap_prepare)(struct vm_area_desc *);
	};

Again, all methods are called without any locks being held, unless
otherwise noted.
(同样，除非另有说明，所有方法都在不持有任何锁的情况下被调用。)

``llseek``
	called when the VFS needs to move the file position index
	(当 VFS 需要移动文件位置索引时调用)

``read``
	called by read(2) and related system calls
	(由 read(2) 及相关系统调用调用)

``read_iter``
	possibly asynchronous read with iov_iter as destination
	(以 iov_iter 为目的地的可能异步的读取)

``write``
	called by write(2) and related system calls
	(由 write(2) 及相关系统调用调用)

``write_iter``
	possibly asynchronous write with iov_iter as source
	(以 iov_iter 为源头的可能异步的写入)

``iopoll``
	called when aio wants to poll for completions on HIPRI iocbs
	(当 aio 想要轮询 HIPRI iocb 的完成情况时调用)

``iterate_shared``
	called when the VFS needs to read the directory contents
	(当 VFS 需要读取目录内容时调用)

``poll``
	called by the VFS when a process wants to check if there is
	activity on this file and (optionally) go to sleep until there
	is activity.  Called by the select(2) and poll(2) system calls
	(当进程想检查此文件上是否有活动并(可选)休眠直到有活动时，由 VFS 调用。由 select(2) 和 poll(2) 系统调用调用)

``unlocked_ioctl``
	called by the ioctl(2) system call.
	(由 ioctl(2) 系统调用调用。)

``compat_ioctl``
	called by the ioctl(2) system call when 32 bit system calls are
	 used on 64 bit kernels.
	(当在 64 位内核上使用 32 位系统调用时，由 ioctl(2) 系统调用调用。)

``mmap``
	called by the mmap(2) system call. Deprecated in favour of
	``mmap_prepare``.
	(由 mmap(2) 系统调用调用。已被 ``mmap_prepare`` 取代，不建议再使用。)

``open``
	called by the VFS when an inode should be opened.  When the VFS
	opens a file, it creates a new "struct file".  It then calls the
	open method for the newly allocated file structure.  You might
	think that the open method really belongs in "struct
	inode_operations", and you may be right.  I think it's done the
	way it is because it makes filesystems simpler to implement.
	The open() method is a good place to initialize the
	"private_data" member in the file structure if you want to point
	to a device structure
	(当应打开一个 inode 时由 VFS 调用。当 VFS 打开一个文件时，它会创建一个新的 "struct file"。然后它为新分配的 file 结构调用 open 方法。你可能认为 open 方法其实属于 "struct inode_operations"，你也许是对的。我认为它之所以采用这种方式，是因为这让文件系统更容易实现。如果你想让 "private_data" 成员指向一个设备结构，open() 方法是初始化 file 结构中该成员的好地方)

``flush``
	called by the close(2) system call to flush a file
	(由 close(2) 系统调用调用以刷新一个文件)

``release``
	called when the last reference to an open file is closed
	(当对打开文件的最后一个引用被关闭时调用)

``fsync``
	called by the fsync(2) system call.  Also see the section above
	entitled "Handling errors during writeback".
	(由 fsync(2) 系统调用调用。另请参见上文标题为 "Handling errors during writeback"(处理回写期间的错误)的一节。)

``fasync``
	called by the fcntl(2) system call when asynchronous
	(non-blocking) mode is enabled for a file
	(当为文件启用异步(非阻塞)模式时，由 fcntl(2) 系统调用调用)

``lock``
	called by the fcntl(2) system call for F_GETLK, F_SETLK, and
	F_SETLKW commands
	(由 fcntl(2) 系统调用在 F_GETLK、F_SETLK 和 F_SETLKW 命令时调用)

``get_unmapped_area``
	called by the mmap(2) system call
	(由 mmap(2) 系统调用调用)

``check_flags``
	called by the fcntl(2) system call for F_SETFL command
	(由 fcntl(2) 系统调用在 F_SETFL 命令时调用)

``flock``
	called by the flock(2) system call
	(由 flock(2) 系统调用调用)

``splice_write``
	called by the VFS to splice data from a pipe to a file.  This
	method is used by the splice(2) system call
	(由 VFS 调用，把数据从管道拼接(splice)到文件。splice(2) 系统调用使用此方法)

``splice_read``
	called by the VFS to splice data from file to a pipe.  This
	method is used by the splice(2) system call
	(由 VFS 调用，把数据从文件拼接(splice)到管道。splice(2) 系统调用使用此方法)

``setlease``
	called by the VFS to set or release a file lock lease.  Local
	filesystems that wish to use the kernel-internal lease implementation
	should set this to generic_setlease(). Other setlease implementations
	should call generic_setlease() to record or remove the lease in the inode
	after setting it. When set to NULL, attempts to set or remove a lease will
	return -EINVAL.
	(由 VFS 调用，设置或释放文件锁租约(lease)。希望使用内核内部租约实现的本地文件系统应将其设置为 generic_setlease()。其他 setlease 实现应在设置租约之后调用 generic_setlease() 在 inode 中记录或移除租约。当设置为 NULL 时，设置或移除租约的尝试将返回 -EINVAL。)

``fallocate``
	called by the VFS to preallocate blocks or punch a hole.
	(由 VFS 调用，预分配块或挖洞。)

``copy_file_range``
	called by the copy_file_range(2) system call.
	(由 copy_file_range(2) 系统调用调用。)

``remap_file_range``
	called by the ioctl(2) system call for FICLONERANGE and FICLONE
	and FIDEDUPERANGE commands to remap file ranges.  An
	implementation should remap len bytes at pos_in of the source
	file into the dest file at pos_out.  Implementations must handle
	callers passing in len == 0; this means "remap to the end of the
	source file".  The return value should the number of bytes
	remapped, or the usual negative error code if errors occurred
	before any bytes were remapped.  The remap_flags parameter
	accepts REMAP_FILE_* flags.  If REMAP_FILE_DEDUP is set then the
	implementation must only remap if the requested file ranges have
	identical contents.  If REMAP_FILE_CAN_SHORTEN is set, the caller is
	ok with the implementation shortening the request length to
	satisfy alignment or EOF requirements (or any other reason).
	(由 ioctl(2) 系统调用在 FICLONERANGE、FICLONE 和 FIDEDUPERANGE 命令时调用，用于重映射文件范围。实现应把源文件中 pos_in 处的 len 字节重映射到目标文件的 pos_out 处。实现必须处理调用者传入 len == 0 的情况；这表示"重映射到源文件末尾"。返回值应是重映射的字节数，如果在任何字节被重映射之前发生错误，则返回通常的负错误码。remap_flags 参数接受 REMAP_FILE_* 标志。如果设置了 REMAP_FILE_DEDUP，则只有当请求的文件范围内容完全相同时实现才可执行重映射。如果设置了 REMAP_FILE_CAN_SHORTEN，调用者可以接受实现缩短请求长度以满足对齐或 EOF 要求(或任何其他原因)。)

``fadvise``
	possibly called by the fadvise64() system call.
	(可能由 fadvise64() 系统调用调用。)

``mmap_prepare``
	Called by the mmap(2) system call. Allows a VFS to set up a
	file-backed memory mapping, most notably establishing relevant
	private state and VMA callbacks.
	(由 mmap(2) 系统调用调用。允许 VFS 建立基于文件的内存映射，最重要的是建立相关的私有状态和 VMA 回调。)

	If further action such as pre-population of page tables is required,
	this can be specified by the vm_area_desc->action field and related
	parameters.
	(如果还需要进一步的操作，例如预先填充页表，可以通过 vm_area_desc->action 字段及相关参数来指定。)

Note that the file operations are implemented by the specific
filesystem in which the inode resides.  When opening a device node
(character or block special) most filesystems will call special
support routines in the VFS which will locate the required device
driver information.  These support routines replace the filesystem file
operations with those for the device driver, and then proceed to call
the new open() method for the file.  This is how opening a device file
in the filesystem eventually ends up calling the device driver open()
method.
(注意，文件操作由 inode 所在的具体文件系统实现。当打开设备节点(字符或块特殊设备)时，大多数文件系统会调用 VFS 中的特殊支持例程，这些例程会定位所需的设备驱动信息。这些支持例程把文件系统的文件操作替换为设备驱动的文件操作，然后接着为该文件调用新的 open() 方法。这就是在文件系统中打开设备文件最终会调用设备驱动 open() 方法的原理。)


Directory Entry Cache (dcache)
(目录项缓存(dcache))
==============================


struct dentry_operations
------------------------

This describes how a filesystem can overload the standard dentry
operations.  Dentries and the dcache are the domain of the VFS and the
individual filesystem implementations.  Device drivers have no business
here.  These methods may be set to NULL, as they are either optional or
the VFS uses a default.  As of kernel 2.6.22, the following members are
defined:
(这里描述文件系统如何重载标准的 dentry 操作。dentry 和 dcache 是 VFS 与各个文件系统实现的领域。设备驱动与此无关。这些方法可以设置为 NULL，因为它们要么是可选的，要么 VFS 会使用默认值。自内核 2.6.22 起，定义了以下成员：)

.. code-block:: c

	struct dentry_operations {
		int (*d_revalidate)(struct inode *, const struct qstr *,
				    struct dentry *, unsigned int);
		int (*d_weak_revalidate)(struct dentry *, unsigned int);
		int (*d_hash)(const struct dentry *, struct qstr *);
		int (*d_compare)(const struct dentry *,
				 unsigned int, const char *, const struct qstr *);
		int (*d_delete)(const struct dentry *);
		int (*d_init)(struct dentry *);
		void (*d_release)(struct dentry *);
		void (*d_iput)(struct dentry *, struct inode *);
		char *(*d_dname)(struct dentry *, char *, int);
		struct vfsmount *(*d_automount)(struct path *);
		int (*d_manage)(const struct path *, bool);
		struct dentry *(*d_real)(struct dentry *, enum d_real_type type);
		bool (*d_unalias_trylock)(const struct dentry *);
		void (*d_unalias_unlock)(const struct dentry *);
	};

``d_revalidate``
	called when the VFS needs to revalidate a dentry.  This is
	called whenever a name look-up finds a dentry in the dcache.
	Most local filesystems leave this as NULL, because all their
	dentries in the dcache are valid.  Network filesystems are
	different since things can change on the server without the
	client necessarily being aware of it.
	(当 VFS 需要重新验证一个 dentry 时调用。只要名字查找在 dcache 中找到了某个 dentry，就会调用它。大多数本地文件系统将其保留为 NULL，因为它们在 dcache 中的所有 dentry 都是有效的。网络文件系统则不同，因为服务器上的内容可能在客户端不知情的情况下发生变化。)

	This function should return a positive value if the dentry is
	still valid, and zero or a negative error code if it isn't.
	(如果 dentry 仍然有效，此函数应返回正值；否则返回零或负错误码。)

	d_revalidate may be called in rcu-walk mode (flags &
	LOOKUP_RCU).  If in rcu-walk mode, the filesystem must
	revalidate the dentry without blocking or storing to the dentry,
	d_parent and d_inode should not be used without care (because
	they can change and, in d_inode case, even become NULL under
	us).
	(d_revalidate 可能在 rcu-walk 模式下被调用(flags & LOOKUP_RCU)。在 rcu-walk 模式下，文件系统必须在不阻塞、不向 dentry 存储的情况下重新验证 dentry；使用 d_parent 和 d_inode 时应当小心(因为它们可能改变，就 d_inode 而言甚至会在我们脚下变成 NULL)。)

	If a situation is encountered that rcu-walk cannot handle,
	return
	-ECHILD and it will be called again in ref-walk mode.
	(如果遇到 rcu-walk 无法处理的情况，返回 -ECHILD，它将以 ref-walk 模式被再次调用。)

``d_weak_revalidate``
	called when the VFS needs to revalidate a "jumped" dentry.  This
	is called when a path-walk ends at dentry that was not acquired
	by doing a lookup in the parent directory.  This includes "/",
	"." and "..", as well as procfs-style symlinks and mountpoint
	traversal.
	(当 VFS 需要重新验证一个"跳入"的 dentry 时调用。当路径遍历结束于一个不是通过在父目录中查找而获得的 dentry 时调用它。这包括 "/"、" ." 和 ".."，以及 procfs 风格的符号链接和挂载点穿越。)

	In this case, we are less concerned with whether the dentry is
	still fully correct, but rather that the inode is still valid.
	As with d_revalidate, most local filesystems will set this to
	NULL since their dcache entries are always valid.
	(在这种情况下，我们不太关心 dentry 是否仍然完全正确，而更关心 inode 是否仍然有效。与 d_revalidate 一样，大多数本地文件系统会将其设置为 NULL，因为它们的 dcache 条目总是有效。)

	This function has the same return code semantics as
	d_revalidate.
	(此函数具有与 d_revalidate 相同的返回码语义。)

	d_weak_revalidate is only called after leaving rcu-walk mode.
	(d_weak_revalidate 只在离开 rcu-walk 模式之后才被调用。)

``d_hash``
	called when the VFS adds a dentry to the hash table.  The first
	dentry passed to d_hash is the parent directory that the name is
	to be hashed into.
	(当 VFS 把一个 dentry 加入哈希表时调用。传给 d_hash 的第一个 dentry 是该名字将要被哈希到的父目录。)

	Same locking and synchronisation rules as d_compare regarding
	what is safe to dereference etc.
	(关于什么可以安全解引用等方面，加锁与同步规则与 d_compare 相同。)

``d_compare``
	called to compare a dentry name with a given name.  The first
	dentry is the parent of the dentry to be compared, the second is
	the child dentry.  len and name string are properties of the
	dentry to be compared.  qstr is the name to compare it with.
	(调用它把一个 dentry 名字与给定名字进行比较。第一个 dentry 是待比较 dentry 的父目录，第二个是子 dentry。len 和名字字符串是待比较 dentry 的属性。qstr 是用来与之比较的名字。)

	Must be constant and idempotent, and should not take locks if
	possible, and should not or store into the dentry.  Should not
	dereference pointers outside the dentry without lots of care
	(eg.  d_parent, d_inode, d_name should not be used).
	(必须是常量性的且幂等的，尽可能不加锁，并且不应向 dentry 存储。在没有充分小心的情况下，不应解引用 dentry 之外的指针(例如不应使用 d_parent、d_inode、d_name)。)

	However, our vfsmount is pinned, and RCU held, so the dentries
	and inodes won't disappear, neither will our sb or filesystem
	module.  ->d_sb may be used.
	(不过，我们的 vfsmount 被固定住且持有 RCU，因此 dentry 和 inode 不会消失，我们的 sb 或文件系统模块也不会消失。可以使用 ->d_sb。)

	It is a tricky calling convention because it needs to be called
	under "rcu-walk", ie. without any locks or references on things.
	(这是一个棘手的调用约定，因为它需要在 "rcu-walk" 之下调用，即不持有对任何东西的锁或引用。)

``d_delete``
	called when the last reference to a dentry is dropped and the
	dcache is deciding whether or not to cache it.  Return 1 to
	delete immediately, or 0 to cache the dentry.  Default is NULL
	which means to always cache a reachable dentry.  d_delete must
	be constant and idempotent.
	(当对一个 dentry 的最后一个引用被放下、dcache 正在决定是否缓存它时调用。返回 1 表示立即删除，返回 0 表示缓存该 dentry。默认为 NULL，意味着总是缓存可达的 dentry。d_delete 必须是常量性的且幂等的。)

``d_init``
	called when a dentry is allocated
	(当一个 dentry 被分配时调用)

``d_release``
	called when a dentry is really deallocated
	(当一个 dentry 真正被去分配时调用)

``d_iput``
	called when a dentry loses its inode (just prior to its being
	deallocated).  The default when this is NULL is that the VFS
	calls iput().  If you define this method, you must call iput()
	yourself
	(当一个 dentry 失去它的 inode 时调用(就在它被去分配之前)。此方法为 NULL 时的默认行为是 VFS 调用 iput()。如果你定义了此方法，你必须自己调用 iput())

``d_dname``
	called when the pathname of a dentry should be generated.
	Useful for some pseudo filesystems (sockfs, pipefs, ...) to
	delay pathname generation.  (Instead of doing it when dentry is
	created, it's done only when the path is needed.).  Real
	filesystems probably dont want to use it, because their dentries
	are present in global dcache hash, so their hash should be an
	invariant.  As no lock is held, d_dname() should not try to
	modify the dentry itself, unless appropriate SMP safety is used.
	CAUTION : d_path() logic is quite tricky.  The correct way to
	return for example "Hello" is to put it at the end of the
	buffer, and returns a pointer to the first char.
	dynamic_dname() helper function is provided to take care of
	this.
	(当需要生成一个 dentry 的路径名时调用。对某些伪文件系统(sockfs、pipefs 等)很有用，可以延迟路径名的生成。(不是在 dentry 创建时生成，而是只在需要路径时才生成)。真实文件系统可能不想使用它，因为它们的 dentry 存在于全局 dcache 哈希中，所以其哈希应当是不变的。由于不持有任何锁，d_dname() 不应尝试修改 dentry 本身，除非使用了适当的 SMP 安全措施。注意：d_path() 的逻辑相当棘手。例如要返回 "Hello"，正确的方法是把它放到缓冲区末尾，并返回指向第一个字符的指针。提供了 dynamic_dname() 辅助函数来处理这件事。)

	Example :
	(示例：)

.. code-block:: c

	static char *pipefs_dname(struct dentry *dent, char *buffer, int buflen)
	{
		return dynamic_dname(dentry, buffer, buflen, "pipe:[%lu]",
				dentry->d_inode->i_ino);
	}

``d_automount``
	called when an automount dentry is to be traversed (optional).
	This should create a new VFS mount record and return the record
	to the caller.  The caller is supplied with a path parameter
	giving the automount directory to describe the automount target
	and the parent VFS mount record to provide inheritable mount
	parameters.  NULL should be returned if someone else managed to
	make the automount first.  If the vfsmount creation failed, then
	an error code should be returned.  If -EISDIR is returned, then
	the directory will be treated as an ordinary directory and
	returned to pathwalk to continue walking.
	(当要穿越一个自动挂载(automount) dentry 时调用(可选)。它应创建一个新的 VFS 挂载记录并把该记录返回给调用者。调用者会获得一个 path 参数，给出描述自动挂载目标的自动挂载目录，以及提供可继承挂载参数的父 VFS 挂载记录。如果别人抢先完成了自动挂载，应返回 NULL。如果 vfsmount 创建失败，则应返回错误码。如果返回 -EISDIR，则该目录将被当作普通目录，并交回路径遍历以继续行走。)

	If a vfsmount is returned, the caller will attempt to mount it
	on the mountpoint and will remove the vfsmount from its
	expiration list in the case of failure.
	(如果返回了一个 vfsmount，调用者将尝试把它挂载到挂载点上，失败时会将该 vfsmount 从其到期列表中移除。)

	This function is only used if DCACHE_NEED_AUTOMOUNT is set on
	the dentry.  This is set by __d_instantiate() if S_AUTOMOUNT is
	set on the inode being added.
	(只有当 dentry 上设置了 DCACHE_NEED_AUTOMOUNT 时才会使用此函数。如果要添加的 inode 上设置了 S_AUTOMOUNT，__d_instantiate() 会设置该标志。)

``d_manage``
	called to allow the filesystem to manage the transition from a
	dentry (optional).  This allows autofs, for example, to hold up
	clients waiting to explore behind a 'mountpoint' while letting
	the daemon go past and construct the subtree there.  0 should be
	returned to let the calling process continue.  -EISDIR can be
	returned to tell pathwalk to use this directory as an ordinary
	directory and to ignore anything mounted on it and not to check
	the automount flag.  Any other error code will abort pathwalk
	completely.
	(调用它让文件系统管理从一个 dentry 穿越的过程(可选)。这使得例如 autofs 可以拦住等待探察 '挂载点' 之后内容的客户端，同时让守护进程通过并在那里构造子树。应返回 0 以让调用进程继续。可以返回 -EISDIR 来告诉路径遍历把这个目录当作普通目录使用，忽略其上挂载的任何东西，并且不检查 automount 标志。任何其他错误码都会完全中止路径遍历。)

	If the 'rcu_walk' parameter is true, then the caller is doing a
	pathwalk in RCU-walk mode.  Sleeping is not permitted in this
	mode, and the caller can be asked to leave it and call again by
	returning -ECHILD.  -EISDIR may also be returned to tell
	pathwalk to ignore d_automount or any mounts.
	(如果 'rcu_walk' 参数为真，则调用者正在以 RCU-walk 模式进行路径遍历。此模式下不允许休眠，调用者可以通过返回 -ECHILD 被要求离开该模式并再次调用。也可以返回 -EISDIR 来告诉路径遍历忽略 d_automount 或任何挂载。)

	This function is only used if DCACHE_MANAGE_TRANSIT is set on
	the dentry being transited from.
	(只有当正在被穿越的 dentry 上设置了 DCACHE_MANAGE_TRANSIT 时才会使用此函数。)

``d_real``
	overlay/union type filesystems implement this method to return one
	of the underlying dentries of a regular file hidden by the overlay.
	(overlay/union 类型的文件系统实现此方法，返回被 overlay 隐藏的某个普通文件的底层 dentry。)

	The 'type' argument takes the values D_REAL_DATA or D_REAL_METADATA
	for returning the real underlying dentry that refers to the inode
	hosting the file's data or metadata respectively.
	('type' 参数取 D_REAL_DATA 或 D_REAL_METADATA 值，分别用于返回引用承载文件数据或元数据的那个 inode 的真实底层 dentry。)

	For non-regular files, the 'dentry' argument is returned.
	(对于非普通文件，返回 'dentry' 参数。)

``d_unalias_trylock``
	if present, will be called by d_splice_alias() before moving a
	preexisting attached alias.  Returning false prevents __d_move(),
	making d_splice_alias() fail with -ESTALE.
	(如果存在，d_splice_alias() 会在移动一个预先存在的已连接别名之前调用它。返回 false 会阻止 __d_move()，使 d_splice_alias() 以 -ESTALE 失败。)

	Rationale: setting FS_RENAME_DOES_D_MOVE will prevent d_move()
	and d_exchange() calls from the outside of filesystem methods;
	however, it does not guarantee that attached dentries won't
	be renamed or moved by d_splice_alias() finding a preexisting
	alias for a directory inode.  Normally we would not care;
	however, something that wants to stabilize the entire path to
	root over a blocking operation might need that.  See 9p for one
	(and hopefully only) example.
	(理由：设置 FS_RENAME_DOES_D_MOVE 将阻止来自文件系统方法之外的 d_move() 和 d_exchange() 调用；但它并不保证已连接的 dentry 不会因 d_splice_alias() 为某个目录 inode 找到预先存在的别名而被重命名或移动。通常我们不会在意；不过，想要在一个阻塞操作之上把到根的整条路径稳定下来的东西可能需要它。参见 9p 的一个(希望是唯一的)例子。)

``d_unalias_unlock``
	should be paired with ``d_unalias_trylock``; that one is called after
	__d_move() call in __d_unalias().
	(应与 ``d_unalias_trylock`` 配对；后者在 __d_unalias() 中的 __d_move() 调用之后被调用。)


Each dentry has a pointer to its parent dentry, as well as a hash list
of child dentries.  Child dentries are basically like files in a
directory.
(每个 dentry 都有一个指向其父 dentry 的指针，以及一个子 dentry 的哈希链表。子 dentry 基本上就像目录中的文件。)


Directory Entry Cache API
(目录项缓存 API)
--------------------------

There are a number of functions defined which permit a filesystem to
manipulate dentries:
(定义了许多允许文件系统操作 dentry 的函数：)

``dget``
	open a new handle for an existing dentry (this just increments
	the usage count)
	(为已有的 dentry 打开一个新句柄(这只是递增使用计数))

``dput``
	close a handle for a dentry (decrements the usage count).  If
	the usage count drops to 0, and the dentry is still in its
	parent's hash, the "d_delete" method is called to check whether
	it should be cached.  If it should not be cached, or if the
	dentry is not hashed, it is deleted.  Otherwise cached dentries
	are put into an LRU list to be reclaimed on memory shortage.
	(关闭一个 dentry 的句柄(递减使用计数)。如果使用计数降到 0，且该 dentry 仍在其父的哈希中，则会调用 "d_delete" 方法检查是否应缓存它。如果不应缓存，或者该 dentry 未被哈希，它会被删除。否则被缓存的 dentry 会放入一个 LRU 链表，在内存短缺时回收。)

``d_drop``
	this unhashes a dentry from its parents hash list.  A subsequent
	call to dput() will deallocate the dentry if its usage count
	drops to 0
	(把一个 dentry 从其父的哈希链表中摘除(unhash)。如果使用计数降到 0，随后调用 dput() 将去分配该 dentry)

``d_delete``
	delete a dentry.  If there are no other open references to the
	dentry then the dentry is turned into a negative dentry (the
	d_iput() method is called).  If there are other references, then
	d_drop() is called instead
	(删除一个 dentry。如果没有其他打开的引用指向该 dentry，则它被变成一个负 dentry(调用 d_iput() 方法)。如果还有其他引用，则改为调用 d_drop())

``d_add``
	add a dentry to its parents hash list and then calls
	d_instantiate()
	(把一个 dentry 加入其父的哈希链表，然后调用 d_instantiate())

``d_instantiate``
	add a dentry to the alias hash list for the inode and updates
	the "d_inode" member.  The "i_count" member in the inode
	structure should be set/incremented.  If the inode pointer is
	NULL, the dentry is called a "negative dentry".  This function
	is commonly called when an inode is created for an existing
	negative dentry
	(把一个 dentry 加入该 inode 的别名哈希链表并更新 "d_inode" 成员。inode 结构中的 "i_count" 成员应被设置/递增。如果 inode 指针为 NULL，该 dentry 被称为 "负 dentry"。当为一个已有的负 dentry 创建 inode 时通常会调用此函数)

``d_lookup``
	look up a dentry given its parent and path name component It
	looks up the child of that given name from the dcache hash
	table.  If it is found, the reference count is incremented and
	the dentry is returned.  The caller must use dput() to free the
	dentry when it finishes using it.
	(根据给定的父 dentry 和路径名分量查找一个 dentry。它在 dcache 哈希表中查找该给定名字的孩子。如果找到，递增其引用计数并返回该 dentry。调用者用完后必须使用 dput() 释放该 dentry。)


Mount Options
(挂载选项)
=============


Parsing options
(解析选项)
---------------

On mount and remount the filesystem is passed a string containing a
comma separated list of mount options.  The options can have either of
these forms:
(在挂载和重新挂载时，会传给文件系统一个包含以逗号分隔的挂载选项列表的字符串。选项可以具有以下两种形式之一：)

  option
  option=value

The <linux/parser.h> header defines an API that helps parse these
options.  There are plenty of examples on how to use it in existing
filesystems.
(<linux/parser.h> 头文件定义了一个帮助解析这些选项的 API。在现有文件系统中有大量使用它的示例。)


Showing options
(显示选项)
---------------

If a filesystem accepts mount options, it must define show_options() to
show all the currently active options.  The rules are:
(如果文件系统接受挂载选项，它必须定义 show_options() 来显示所有当前生效的选项。规则如下：)

  - options MUST be shown which are not default or their values differ
    from the default
    (必须显示那些非默认的选项，或其值与默认值不同的选项)

  - options MAY be shown which are enabled by default or have their
    default value
    (可以显示那些默认启用或取默认值的选项)

Options used only internally between a mount helper and the kernel (such
as file descriptors), or which only have an effect during the mounting
(such as ones controlling the creation of a journal) are exempt from the
above rules.
(只在挂载助手与内核之间内部使用的选项(例如文件描述符)，或只在挂载期间起作用的选项(例如控制日志(journal)创建的选项)不受上述规则约束。)

The underlying reason for the above rules is to make sure, that a mount
can be accurately replicated (e.g. umounting and mounting again) based
on the information found in /proc/mounts.
(上述规则的深层原因是要确保：能够基于 /proc/mounts 中找到的信息精确地重现一次挂载(例如卸载后再次挂载)。)


Resources
(参考资料)
=========

(Note some of these resources are not up-to-date with the latest kernel
 version.)
(注意，其中一些资源并未跟上最新内核的版本。)

Creating Linux virtual filesystems. 2002
    <https://lwn.net/Articles/13325/>
(创建 Linux 虚拟文件系统。2002，<https://lwn.net/Articles/13325/>)

The Linux Virtual File-system Layer by Neil Brown. 1999
    <http://www.cse.unsw.edu.au/~neilb/oss/linux-commentary/vfs.html>
(Linux 虚拟文件系统层，Neil Brown 著。1999，<http://www.cse.unsw.edu.au/~neilb/oss/linux-commentary/vfs.html>)

A tour of the Linux VFS by Michael K. Johnson. 1996
    <https://www.tldp.org/LDP/khg/HyperNews/get/fs/vfstour.html>
(Linux VFS 导览，Michael K. Johnson 著。1996，<https://www.tldp.org/LDP/khg/HyperNews/get/fs/vfstour.html>)

A small trail through the Linux kernel by Andries Brouwer. 2001
    <https://www.win.tue.nl/~aeb/linux/vfs/trail.html>
(穿越 Linux 内核的一条小径，Andries Brouwer 著。2001，<https://www.win.tue.nl/~aeb/linux/vfs/trail.html>)
