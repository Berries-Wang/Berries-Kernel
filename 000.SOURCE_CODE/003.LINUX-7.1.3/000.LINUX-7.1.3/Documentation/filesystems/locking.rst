=======
Locking
=======

(锁定)

The text below describes the locking rules for VFS-related methods.
It is (believed to be) up-to-date. *Please*, if you change anything in
prototypes or locking protocols - update this file. And update the relevant
instances in the tree, don't leave that to maintainers of filesystems/devices/
etc. At the very least, put the list of dubious cases in the end of this file.
Don't turn it into log - maintainers of out-of-the-tree code are supposed to
be able to use diff(1).
(下面的文本描述了 VFS 相关方法的锁定规则。它(被认为)是最新的。*请*，如果您更改了原型或锁定协议中的任何内容——请更新此文件。并请更新树中的相关实例，不要把这些留给文件系统/设备等的维护者。至少，请把可疑案例的清单放在本文件末尾。不要把它变成日志——树外代码的维护者应该能够使用 diff(1)。)

Thing currently missing here: socket operations. Alexey?
(本文目前缺失的内容：套接字操作。Alexey？)

dentry_operations
(dentry 操作)
=================

prototypes::

	int (*d_revalidate)(struct inode *, const struct qstr *,
			    struct dentry *, unsigned int);
	int (*d_weak_revalidate)(struct dentry *, unsigned int);
	int (*d_hash)(const struct dentry *, struct qstr *);
	int (*d_compare)(const struct dentry *,
			unsigned int, const char *, const struct qstr *);
	int (*d_delete)(struct dentry *);
	int (*d_init)(struct dentry *);
	void (*d_release)(struct dentry *);
	void (*d_iput)(struct dentry *, struct inode *);
	char *(*d_dname)((struct dentry *dentry, char *buffer, int buflen);
	struct vfsmount *(*d_automount)(struct path *path);
	int (*d_manage)(const struct path *, bool);
	struct dentry *(*d_real)(struct dentry *, enum d_real_type type);
	bool (*d_unalias_trylock)(const struct dentry *);
	void (*d_unalias_unlock)(const struct dentry *);

(原型：)

locking rules:
(锁定规则：)

================== ===========	========	==============	========
ops		   rename_lock	->d_lock	may block	rcu-walk
================== ===========	========	==============	========
d_revalidate:	   no		no		yes (ref-walk)	maybe
d_weak_revalidate: no		no		yes	 	no
d_hash		   no		no		no		maybe
d_compare:	   yes		no		no		maybe
d_delete:	   no		yes		no		no
d_init:		   no		no		yes		no
d_release:	   no		no		yes		no
d_prune:           no		yes		no		no
d_iput:		   no		no		yes		no
d_dname:	   no		no		no		no
d_automount:	   no		no		yes		no
d_manage:	   no		no		yes (ref-walk)	maybe
d_real		   no		no		yes 		no
d_unalias_trylock  yes		no		no 		no
d_unalias_unlock   yes		no		no 		no
================== ===========	========	==============	========

inode_operations
(inode 操作)
================

prototypes::

	int (*create) (struct mnt_idmap *, struct inode *,struct dentry *,umode_t, bool);
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
	const char *(*get_link) (struct dentry *, struct inode *, struct delayed_call *);
	void (*truncate) (struct inode *);
	int (*permission) (struct mnt_idmap *, struct inode *, int, unsigned int);
	struct posix_acl * (*get_inode_acl)(struct inode *, int, bool);
	int (*setattr) (struct mnt_idmap *, struct dentry *, struct iattr *);
	int (*getattr) (struct mnt_idmap *, const struct path *, struct kstat *, u32, unsigned int);
	ssize_t (*listxattr) (struct dentry *, char *, size_t);
	int (*fiemap)(struct inode *, struct fiemap_extent_info *, u64 start, u64 len);
	void (*update_time)(struct inode *inode, enum fs_update_time type,
			    int flags);
	void (*sync_lazytime)(struct inode *inode);
	int (*atomic_open)(struct inode *, struct dentry *,
				struct file *, unsigned open_flag,
				umode_t create_mode);
	int (*tmpfile) (struct mnt_idmap *, struct inode *,
			struct file *, umode_t);
	int (*fileattr_set)(struct mnt_idmap *idmap,
			    struct dentry *dentry, struct file_kattr *fa);
	int (*fileattr_get)(struct dentry *dentry, struct file_kattr *fa);
	struct posix_acl * (*get_acl)(struct mnt_idmap *, struct dentry *, int);
	struct offset_ctx *(*get_offset_ctx)(struct inode *inode);

(原型：)

locking rules:
(锁定规则：)

	(所有都可能阻塞)

==============	==================================================
ops		i_rwsem(inode)
==============	==================================================
lookup:		shared
create:		exclusive
link:		exclusive (both)
mknod:		exclusive
symlink:	exclusive
mkdir:		exclusive
unlink:		exclusive (both)
rmdir:		exclusive (both)(see below) (见下文)
rename:		exclusive (both parents, some children)	(see below) (见下文)
readlink:	no
get_link:	no
setattr:	exclusive
permission:	no (may not block if called in rcu-walk mode) (在 rcu-walk 模式下调用时可能不会阻塞)
get_inode_acl:	no
get_acl:	no
getattr:	no
listxattr:	no
fiemap:		no
update_time:	no
sync_lazytime:	no
atomic_open:	shared (exclusive if O_CREAT is set in open flags) (若 open 标志中设置了 O_CREAT 则为独占)
tmpfile:	no
fileattr_get:	no or exclusive
fileattr_set:	exclusive
get_offset_ctx  no
==============	==================================================


	Additionally, ->rmdir(), ->unlink() and ->rename() have ->i_rwsem
	exclusive on victim.
	cross-directory ->rename() has (per-superblock) ->s_vfs_rename_sem.
	->unlink() and ->rename() have ->i_rwsem exclusive on all non-directories
	involved.
	->rename() has ->i_rwsem exclusive on any subdirectory that changes parent.
	(此外，->rmdir()、->unlink() 和 ->rename() 对受害者持有 ->i_rwsem 独占。跨目录的 ->rename() 持有(每个超级块的) ->s_vfs_rename_sem。->unlink() 和 ->rename() 对所涉及的所有非目录持有 ->i_rwsem 独占。->rename() 对任何改变父目录的子目录持有 ->i_rwsem 独占。)

See Documentation/filesystems/directory-locking.rst for more detailed discussion
of the locking scheme for directory operations.
(有关目录操作锁定方案的更详细讨论，请参见 Documentation/filesystems/directory-locking.rst。)

xattr_handler operations
(xattr_handler 操作)
========================

prototypes::

	bool (*list)(struct dentry *dentry);
	int (*get)(const struct xattr_handler *handler, struct dentry *dentry,
		   struct inode *inode, const char *name, void *buffer,
		   size_t size);
	int (*set)(const struct xattr_handler *handler,
                   struct mnt_idmap *idmap,
                   struct dentry *dentry, struct inode *inode, const char *name,
                   const void *buffer, size_t size, int flags);

(原型：)

locking rules:
(锁定规则：)

	(所有都可能阻塞)

=====		==============
ops		i_rwsem(inode)
=====		==============
list:		no
get:		no
set:		exclusive
=====		==============

super_operations
(超级块操作)
================

prototypes::

	struct inode *(*alloc_inode)(struct super_block *sb);
	void (*free_inode)(struct inode *);
	void (*destroy_inode)(struct inode *);
	void (*dirty_inode) (struct inode *, int flags);
	int (*write_inode) (struct inode *, struct writeback_control *wbc);
	int (*drop_inode) (struct inode *);
	void (*evict_inode) (struct inode *);
	void (*put_super) (struct super_block *);
	int (*sync_fs)(struct super_block *sb, int wait);
	int (*freeze_fs) (struct super_block *);
	int (*unfreeze_fs) (struct super_block *);
	int (*statfs) (struct dentry *, struct kstatfs *);
	void (*umount_begin) (struct super_block *);
	int (*show_options)(struct seq_file *, struct dentry *);
	ssize_t (*quota_read)(struct super_block *, int, char *, size_t, loff_t);
	ssize_t (*quota_write)(struct super_block *, int, const char *, size_t, loff_t);

(原型：)

locking rules:
(锁定规则：)

	(全部都可能阻塞[并不属实，见下文])

======================	============	========================
ops			s_umount	note
======================	============	========================
alloc_inode:
free_inode:				called from RCU callback (从 RCU 回调中调用)
destroy_inode:
dirty_inode:
write_inode:
drop_inode:				!!!inode->i_lock!!!
evict_inode:
put_super:		write
sync_fs:		read
freeze_fs:		write
unfreeze_fs:		write
statfs:			maybe(read)	(see below) (见下文)
umount_begin:		no
show_options:		no		(namespace_sem)
quota_read:		no		(see below) (见下文)
quota_write:		no		(see below) (见下文)
======================	============	========================

->statfs() has s_umount (shared) when called by ustat(2) (native or
compat), but that's an accident of bad API; s_umount is used to pin
the superblock down when we only have dev_t given us by userland to
identify the superblock.  Everything else (statfs(), fstatfs(), etc.)
doesn't hold it when calling ->statfs() - superblock is pinned down
by resolving the pathname passed to syscall.
(当被 ustat(2)(原生或兼容版本)调用时，->statfs() 持有 s_umount(共享)，但这是一个糟糕 API 造成的意外；当我们只有用户空间给出的 dev_t 来标识超级块时，s_umount 用于把超级块固定住。其他所有情况(statfs()、fstatfs() 等)在调用 ->statfs() 时都不持有它——超级块通过解析传递给系统调用的路径名来固定。)

->quota_read() and ->quota_write() functions are both guaranteed to
be the only ones operating on the quota file by the quota code (via
dqio_sem) (unless an admin really wants to screw up something and
writes to quota files with quotas on). For other details about locking
see also dquot_operations section.
(配额代码保证 ->quota_read() 和 ->quota_write() 函数(通过 dqio_sem)是唯一对配额文件进行操作的两个函数(除非管理员真的想搞砸某些事情，在配额开启时写入配额文件)。有关锁定的其他细节，另请参见 dquot_operations 一节。)

file_system_type
(文件系统类型)
================

prototypes::

	void (*kill_sb) (struct super_block *);

(原型：)

locking rules:
(锁定规则：)

=======		=========
ops		may block (可阻塞)
=======		=========
kill_sb		yes
=======		=========

->kill_sb() takes a write-locked superblock, does all shutdown work on it,
unlocks and drops the reference.
(->kill_sb() 获取一个处于写锁定状态的超级块，在其上执行全部关闭工作，然后解锁并释放引用。)

address_space_operations
(address_space 操作)
========================
prototypes::

	int (*read_folio)(struct file *, struct folio *);
	int (*writepages)(struct address_space *, struct writeback_control *);
	bool (*dirty_folio)(struct address_space *, struct folio *folio);
	void (*readahead)(struct readahead_control *);
	int (*write_begin)(const struct kiocb *, struct address_space *mapping,
				loff_t pos, unsigned len,
				struct folio **foliop, void **fsdata);
	int (*write_end)(const struct kiocb *, struct address_space *mapping,
				loff_t pos, unsigned len, unsigned copied,
				struct folio *folio, void *fsdata);
	sector_t (*bmap)(struct address_space *, sector_t);
	void (*invalidate_folio) (struct folio *, size_t start, size_t len);
	bool (*release_folio)(struct folio *, gfp_t);
	void (*free_folio)(struct folio *);
	int (*direct_IO)(struct kiocb *, struct iov_iter *iter);
	int (*migrate_folio)(struct address_space *, struct folio *dst,
			struct folio *src, enum migrate_mode);
	int (*launder_folio)(struct folio *);
	bool (*is_partially_uptodate)(struct folio *, size_t from, size_t count);
	int (*error_remove_folio)(struct address_space *, struct folio *);
	int (*swap_activate)(struct swap_info_struct *sis, struct file *f, sector_t *span)
	int (*swap_deactivate)(struct file *);
	int (*swap_rw)(struct kiocb *iocb, struct iov_iter *iter);

(原型：)

locking rules:
(锁定规则：)

	(除 dirty_folio 和 free_folio 外，全部都可能阻塞)

======================	======================== =========	===============
ops			folio locked		 i_rwsem	invalidate_lock
======================	======================== =========	===============
read_folio:		yes, unlocks				shared
writepages:
dirty_folio:		maybe
readahead:		yes, unlocks				shared
write_begin:		locks the folio		 exclusive
write_end:		yes, unlocks		 exclusive
bmap:
invalidate_folio:	yes					exclusive
release_folio:		yes
free_folio:		yes
direct_IO:
migrate_folio:		yes (both)
launder_folio:		yes
is_partially_uptodate:	yes
error_remove_folio:	yes
swap_activate:		no
swap_deactivate:	no
swap_rw:		yes, unlocks
======================	======================== =========	===============

->write_begin(), ->write_end() and ->read_folio() may be called from
the request handler (/dev/loop).
(->write_begin()、->write_end() 和 ->read_folio() 可能会从请求处理程序(/dev/loop)中被调用。)

->read_folio() unlocks the folio, either synchronously or via I/O
completion.
(->read_folio() 会解锁 folio，要么同步进行，要么通过 I/O 完成回调进行。)

->readahead() unlocks the folios that I/O is attempted on like ->read_folio().
(->readahead() 像 ->read_folio() 一样，解锁那些对其发起了 I/O 的 folio。)

->writepages() is used for periodic writeback and for syscall-initiated
sync operations.  The address_space should start I/O against at least
``*nr_to_write`` pages.  ``*nr_to_write`` must be decremented for each page
which is written.  The address_space implementation may write more (or less)
pages than ``*nr_to_write`` asks for, but it should try to be reasonably close.
If nr_to_write is NULL, all dirty pages must be written.
(->writepages() 用于周期性回写以及由系统调用发起的同步操作。address_space 应至少针对 ``*nr_to_write`` 页启动 I/O。每写入一页都必须递减 ``*nr_to_write``。address_space 实现写入的页数可以比 ``*nr_to_write`` 要求的多(或少)，但应尽量接近。如果 nr_to_write 为 NULL，则必须写出所有脏页。)

writepages should _only_ write pages which are present in
mapping->i_pages.
(writepages 应该_只_写 mapping->i_pages 中存在的那些页。)

->dirty_folio() is called from various places in the kernel when
the target folio is marked as needing writeback.  The folio cannot be
truncated because either the caller holds the folio lock, or the caller
has found the folio while holding the page table lock which will block
truncation.
(当目标 folio 被标记为需要回写时，内核中的多个位置都会调用 ->dirty_folio()。此时 folio 不可能被截断，因为要么调用者持有 folio 锁，要么调用者在持有页表锁的情况下找到了该 folio——而页表锁会阻塞截断。)

->bmap() is currently used by legacy ioctl() (FIBMAP) provided by some
filesystems and by the swapper. The latter will eventually go away.  Please,
keep it that way and don't breed new callers.
(->bmap() 目前被某些文件系统提供的传统 ioctl()(FIBMAP)以及交换器(swapper)使用。后者最终会消失。请保持现状，不要滋生新的调用者。)

->invalidate_folio() is called when the filesystem must attempt to drop
some or all of the buffers from the page when it is being truncated. It
returns zero on success.  The filesystem must exclusively acquire
invalidate_lock before invalidating page cache in truncate / hole punch
path (and thus calling into ->invalidate_folio) to block races between page
cache invalidation and page cache filling functions (fault, read, ...).
(当页正在被截断、文件系统必须尝试丢弃该页中的部分或全部缓冲时，会调用 ->invalidate_folio()。成功时返回零。在 truncate / hole punch 路径中，文件系统必须在使页缓存失效之前(从而调用 ->invalidate_folio)独占获取 invalidate_lock，以阻塞页缓存失效与页缓存填充函数(fault、read 等)之间的竞态。)

->release_folio() is called when the MM wants to make a change to the
folio that would invalidate the filesystem's private data.  For example,
it may be about to be removed from the address_space or split.  The folio
is locked and not under writeback.  It may be dirty.  The gfp parameter
is not usually used for allocation, but rather to indicate what the
filesystem may do to attempt to free the private data.  The filesystem may
return false to indicate that the folio's private data cannot be freed.
If it returns true, it should have already removed the private data from
the folio.  If a filesystem does not provide a ->release_folio method,
the pagecache will assume that private data is buffer_heads and call
try_to_free_buffers().
(当 MM 想要对 folio 做出会使文件系统私有数据失效的更改时，会调用 ->release_folio()。例如，该 folio 可能即将从 address_space 中移除或被拆分。folio 处于锁定状态且不在回写中。它可能是脏的。gfp 参数通常不用于分配，而是用于指示文件系统可以采取什么做法来尝试释放私有数据。文件系统可以返回 false 以表示该 folio 的私有数据无法释放。如果返回 true，它应当已经把私有数据从 folio 中移除了。如果文件系统未提供 ->release_folio 方法，页缓存会假定私有数据是 buffer_heads 并调用 try_to_free_buffers()。)

->free_folio() is called when the kernel has dropped the folio
from the page cache.
(当内核已把 folio 从页缓存中丢弃时，会调用 ->free_folio()。)

->launder_folio() may be called prior to releasing a folio if
it is still found to be dirty. It returns zero if the folio was successfully
cleaned, or an error value if not. Note that in order to prevent the folio
getting mapped back in and redirtied, it needs to be kept locked
across the entire operation.
(如果在释放 folio 之前仍发现它是脏的，则可能会先调用 ->launder_folio()。如果该 folio 被成功清理则返回零，否则返回错误值。注意，为防止该 folio 被重新映射回来并再次变脏，需要在整个操作期间一直保持其锁定。)

->swap_activate() will be called to prepare the given file for swap.  It
should perform any validation and preparation necessary to ensure that
writes can be performed with minimal memory allocation.  It should call
add_swap_extent(), or the helper iomap_swapfile_activate(), and return
the number of extents added.  If IO should be submitted through
->swap_rw(), it should set SWP_FS_OPS, otherwise IO will be submitted
directly to the block device ``sis->bdev``.
(->swap_activate() 将被调用以为给定文件准备交换。它应执行一切必要的验证与准备，以确保能在最少的内存分配下执行写入。它应调用 add_swap_extent() 或辅助函数 iomap_swapfile_activate()，并返回添加的区段数。如果 IO 应通过 ->swap_rw() 提交，它应设置 SWP_FS_OPS，否则 IO 将直接提交到块设备 ``sis->bdev``。)

->swap_deactivate() will be called in the sys_swapoff()
path after ->swap_activate() returned success.
(在 ->swap_activate() 返回成功之后，->swap_deactivate() 将在 sys_swapoff() 路径中被调用。)

->swap_rw will be called for swap IO if SWP_FS_OPS was set by ->swap_activate().
(如果 ->swap_activate() 设置了 SWP_FS_OPS，交换 IO 将会调用 ->swap_rw。)

file_lock_operations
(file_lock 操作)
====================

prototypes::

	void (*fl_copy_lock)(struct file_lock *, struct file_lock *);
	void (*fl_release_private)(struct file_lock *);


(原型：)

locking rules:
(锁定规则：)

===================	=============	=========
ops			inode->i_lock	may block (可阻塞)
===================	=============	=========
fl_copy_lock:		yes		no
fl_release_private:	maybe		maybe[1]_
===================	=============	=========

.. [1]:
   ->fl_release_private for flock or POSIX locks is currently allowed
   to block. Leases however can still be freed while the i_lock is held and
   so fl_release_private called on a lease should not block.
   (->fl_release_private 对于 flock 锁或 POSIX 锁目前允许阻塞。然而，租约(lease)仍然可以在持有 i_lock 的情况下被释放，因此对租约调用的 fl_release_private 不应阻塞。)

lock_manager_operations
(锁管理器操作)
=======================

prototypes::

	void (*lm_notify)(struct file_lock *);  /* unblock callback */
	int (*lm_grant)(struct file_lock *, struct file_lock *, int);
	void (*lm_break)(struct file_lock *); /* break_lease callback */
	int (*lm_change)(struct file_lock **, int);
	bool (*lm_breaker_owns_lease)(struct file_lock *);
        bool (*lm_lock_expirable)(struct file_lock *);
        void (*lm_expire_lock)(void);
        bool (*lm_breaker_timedout)(struct file_lease *);

(原型：)

locking rules:
(锁定规则：)

======================	=============	=================	=========
ops			   flc_lock  	blocked_lock_lock	may block (可阻塞)
======================	=============	=================	=========
lm_notify:		no      	yes			no
lm_grant:		no		no			no
lm_break:		yes		no			no
lm_change		yes		no			no
lm_breaker_owns_lease:	yes     	no			no
lm_lock_expirable	yes		no			no
lm_expire_lock		no		no			yes
lm_open_conflict	yes		no			no
lm_breaker_timedout     yes             no                      no
======================	=============	=================	=========

buffer_head
(缓冲区头)
===========

prototypes::

	void (*b_end_io)(struct buffer_head *bh, int uptodate);

(原型：)

locking rules:
(锁定规则：)

called from interrupts. In other words, extreme care is needed here.
bh is locked, but that's all warranties we have here. Currently only RAID1,
highmem, fs/buffer.c, and fs/ntfs/aops.c are providing these. Block devices
call this method upon the IO completion.
(从中断中调用。换句话说，这里需要极其小心。bh 已被锁定，但这就是我们在这里拥有的全部保证。目前只有 RAID1、highmem、fs/buffer.c 和 fs/ntfs/aops.c 提供这些方法。块设备在 IO 完成时调用此方法。)

block_device_operations
(块设备操作)
=======================
prototypes::

	int (*open) (struct block_device *, fmode_t);
	int (*release) (struct gendisk *, fmode_t);
	int (*ioctl) (struct block_device *, fmode_t, unsigned, unsigned long);
	int (*compat_ioctl) (struct block_device *, fmode_t, unsigned, unsigned long);
	int (*direct_access) (struct block_device *, sector_t, void **,
				unsigned long *);
	void (*unlock_native_capacity) (struct gendisk *);
	int (*getgeo)(struct gendisk *, struct hd_geometry *);
	void (*swap_slot_free_notify) (struct block_device *, unsigned long);

(原型：)

locking rules:
(锁定规则：)

======================= ===================
ops			open_mutex
======================= ===================
open:			yes
release:		yes
ioctl:			no
compat_ioctl:		no
direct_access:		no
unlock_native_capacity:	no
getgeo:			no
swap_slot_free_notify:	no	(see below) (见下文)
======================= ===================

swap_slot_free_notify is called with swap_lock and sometimes the page lock
held.
(swap_slot_free_notify 在持有 swap_lock、有时还持有页锁的情况下被调用。)


file_operations
(文件操作)
===============

prototypes::

	loff_t (*llseek) (struct file *, loff_t, int);
	ssize_t (*read) (struct file *, char __user *, size_t, loff_t *);
	ssize_t (*write) (struct file *, const char __user *, size_t, loff_t *);
	ssize_t (*read_iter) (struct kiocb *, struct iov_iter *);
	ssize_t (*write_iter) (struct kiocb *, struct iov_iter *);
	int (*iopoll) (struct kiocb *kiocb, bool spin);
	int (*iterate_shared) (struct file *, struct dir_context *);
	__poll_t (*poll) (struct file *, struct poll_table_struct *);
	long (*unlocked_ioctl) (struct file *, unsigned int, unsigned long);
	long (*compat_ioctl) (struct file *, unsigned int, unsigned long);
	int (*mmap) (struct file *, struct vm_area_struct *);
	int (*open) (struct inode *, struct file *);
	int (*flush) (struct file *);
	int (*release) (struct inode *, struct file *);
	int (*fsync) (struct file *, loff_t start, loff_t end, int datasync);
	int (*fasync) (int, struct file *, int);
	int (*lock) (struct file *, int, struct file_lock *);
	unsigned long (*get_unmapped_area)(struct file *, unsigned long,
			unsigned long, unsigned long, unsigned long);
	int (*check_flags)(int);
	int (*flock) (struct file *, int, struct file_lock *);
	ssize_t (*splice_write)(struct pipe_inode_info *, struct file *, loff_t *,
			size_t, unsigned int);
	ssize_t (*splice_read)(struct file *, loff_t *, struct pipe_inode_info *,
			size_t, unsigned int);
	int (*setlease)(struct file *, long, struct file_lock **, void **);
	long (*fallocate)(struct file *, int, loff_t, loff_t);
	void (*show_fdinfo)(struct seq_file *m, struct file *f);
	unsigned (*mmap_capabilities)(struct file *);
	ssize_t (*copy_file_range)(struct file *, loff_t, struct file *,
			loff_t, size_t, unsigned int);
	loff_t (*remap_file_range)(struct file *file_in, loff_t pos_in,
			struct file *file_out, loff_t pos_out,
			loff_t len, unsigned int remap_flags);
	int (*fadvise)(struct file *, loff_t, loff_t, int);

(原型：)

locking rules:
(锁定规则：)

	(全部都可能阻塞。)

->llseek() locking has moved from llseek to the individual llseek
implementations.  If your fs is not using generic_file_llseek, you
need to acquire and release the appropriate locks in your ->llseek().
For many filesystems, it is probably safe to acquire the inode
mutex or just to use i_size_read() instead.
Note: this does not protect the file->f_pos against concurrent modifications
since this is something the userspace has to take care about.
(->llseek() 的锁定已从 llseek 本身转移到各个 llseek 实现中。如果你的文件系统没有使用 generic_file_llseek，你需要在你的 ->llseek() 中获取并释放适当的锁。对许多文件系统而言，获取 inode 互斥体，或者只是改用 i_size_read()，可能是安全的。注意：这并不能保护 file->f_pos 免受并发修改，因为这是用户空间必须自行留意的事情。)

->iterate_shared() is called with i_rwsem held for reading, and with the
file f_pos_lock held exclusively
(->iterate_shared() 在以读方式持有 i_rwsem、并以独占方式持有文件 f_pos_lock 的情况下被调用)

->fasync() is responsible for maintaining the FASYNC bit in filp->f_flags.
Most instances call fasync_helper(), which does that maintenance, so it's
not normally something one needs to worry about.  Return values > 0 will be
mapped to zero in the VFS layer.
(->fasync() 负责维护 filp->f_flags 中的 FASYNC 位。大多数实例都会调用 fasync_helper()，由它完成该维护工作，因此这通常不是需要担心的事情。返回值 > 0 会在 VFS 层被映射为零。)

->readdir() and ->ioctl() on directories must be changed. Ideally we would
move ->readdir() to inode_operations and use a separate method for directory
->ioctl() or kill the latter completely. One of the problems is that for
anything that resembles union-mount we won't have a struct file for all
components. And there are other reasons why the current interface is a mess...
(目录上的 ->readdir() 和 ->ioctl() 必须加以修改。理想情况下，我们会把 ->readdir() 移到 inode_operations 中，并为目录 ->ioctl() 使用单独的方法，或者干脆彻底删除后者。问题之一是，对于任何类似联合挂载(union-mount)的东西，我们无法为所有组件各提供一个 struct file。而且当前接口之所以混乱还有其他原因……)

->read on directories probably must go away - we should just enforce -EISDIR
in sys_read() and friends.
(目录上的 ->read 可能也必须去掉——我们应当只在 sys_read() 及类似函数中强制返回 -EISDIR。)

->setlease operations should call generic_setlease() before or after setting
the lease within the individual filesystem to record the result of the
operation
(->setlease 操作应在各个文件系统内部设置租约之前或之后调用 generic_setlease()，以记录该操作的结果)

->fallocate implementation must be really careful to maintain page cache
consistency when punching holes or performing other operations that invalidate
page cache contents. Usually the filesystem needs to call
truncate_inode_pages_range() to invalidate relevant range of the page cache.
However the filesystem usually also needs to update its internal (and on disk)
view of file offset -> disk block mapping. Until this update is finished, the
filesystem needs to block page faults and reads from reloading now-stale page
cache contents from the disk. Since VFS acquires mapping->invalidate_lock in
shared mode when loading pages from disk (filemap_fault(), filemap_read(),
readahead paths), the fallocate implementation must take the invalidate_lock to
prevent reloading.
(->fallocate 实现在打洞或执行其他会使页缓存内容失效的操作时，必须格外小心地维护页缓存一致性。通常文件系统需要调用 truncate_inode_pages_range() 来使页缓存的相关范围失效。然而，文件系统通常还需要更新其内部的(以及磁盘上的)文件偏移 -> 磁盘块映射视图。在该更新完成之前，文件系统需要阻塞缺页和读取，以免从磁盘重新加载现已过期的页缓存内容。由于 VFS 在从磁盘加载页时(filemap_fault()、filemap_read()、readahead 路径)会以共享模式获取 mapping->invalidate_lock，因此 fallocate 实现必须持有 invalidate_lock 以防止重新加载。)

->copy_file_range and ->remap_file_range implementations need to serialize
against modifications of file data while the operation is running. For
blocking changes through write(2) and similar operations inode->i_rwsem can be
used. To block changes to file contents via a memory mapping during the
operation, the filesystem must take mapping->invalidate_lock to coordinate
with ->page_mkwrite.
(->copy_file_range 和 ->remap_file_range 实现需要在操作运行期间针对文件数据的修改进行串行化。对于通过 write(2) 及类似操作进行的修改阻塞，可以使用 inode->i_rwsem。为了在操作期间通过内存映射阻塞对文件内容的更改，文件系统必须获取 mapping->invalidate_lock 以与 ->page_mkwrite 协调。)

dquot_operations
(dquot 操作)
================

prototypes::

	int (*write_dquot) (struct dquot *);
	int (*acquire_dquot) (struct dquot *);
	int (*release_dquot) (struct dquot *);
	int (*mark_dirty) (struct dquot *);
	int (*write_info) (struct super_block *, int);

(原型：)

These operations are intended to be more or less wrapping functions that ensure
a proper locking wrt the filesystem and call the generic quota operations.
(这些操作旨在或多或少作为包装函数：确保针对文件系统的正确锁定，然后调用通用配额操作。)

What filesystem should expect from the generic quota functions:
(文件系统可以对通用配额函数有什么期待：)

==============	============	=========================
ops		FS recursion	Held locks when called (调用时持有的锁)
==============	============	=========================
write_dquot:	yes		dqonoff_sem or dqptr_sem (dqonoff_sem 或 dqptr_sem)
acquire_dquot:	yes		dqonoff_sem or dqptr_sem (dqonoff_sem 或 dqptr_sem)
release_dquot:	yes		dqonoff_sem or dqptr_sem (dqonoff_sem 或 dqptr_sem)
mark_dirty:	no		-
write_info:	yes		dqonoff_sem
==============	============	=========================

FS recursion means calling ->quota_read() and ->quota_write() from superblock
operations.
(FS recursion(文件系统递归)是指从超级块操作中调用 ->quota_read() 和 ->quota_write()。)

More details about quota locking can be found in fs/dquot.c.
(有关配额锁定的更多细节可在 fs/dquot.c 中找到。)

vm_operations_struct
(vm_operations_struct 操作)
====================

prototypes::

	void (*open)(struct vm_area_struct *);
	void (*close)(struct vm_area_struct *);
	vm_fault_t (*fault)(struct vm_fault *);
	vm_fault_t (*huge_fault)(struct vm_fault *, unsigned int order);
	vm_fault_t (*map_pages)(struct vm_fault *, pgoff_t start, pgoff_t end);
	vm_fault_t (*page_mkwrite)(struct vm_area_struct *, struct vm_fault *);
	vm_fault_t (*pfn_mkwrite)(struct vm_area_struct *, struct vm_fault *);
	int (*access)(struct vm_area_struct *, unsigned long, void*, int, int);

(原型：)

locking rules:
(锁定规则：)

=============	==========	===========================
ops		mmap_lock	PageLocked(page)
=============	==========	===========================
open:		write
close:		read/write
fault:		read		can return with page locked (可以带着已锁定的页返回)
huge_fault:	maybe-read
map_pages:	maybe-read
page_mkwrite:	read		can return with page locked (可以带着已锁定的页返回)
pfn_mkwrite:	read
access:		read
=============	==========	===========================

->fault() is called when a previously not present pte is about to be faulted
in. The filesystem must find and return the page associated with the passed in
"pgoff" in the vm_fault structure. If it is possible that the page may be
truncated and/or invalidated, then the filesystem must lock invalidate_lock,
then ensure the page is not already truncated (invalidate_lock will block
subsequent truncate), and then return with VM_FAULT_LOCKED, and the page
locked. The VM will unlock the page.
(当一个先前不存在的 pte 即将被调入(fault in)时，会调用 ->fault()。文件系统必须找到并返回与 vm_fault 结构中传入的 "pgoff" 相关联的页。如果该页有可能被截断和/或失效，那么文件系统必须先锁定 invalidate_lock，然后确保该页尚未被截断(invalidate_lock 会阻塞后续的截断)，然后以 VM_FAULT_LOCKED 返回且该页处于锁定状态。VM 会负责解锁该页。)

->huge_fault() is called when there is no PUD or PMD entry present.  This
gives the filesystem the opportunity to install a PUD or PMD sized page.
Filesystems can also use the ->fault method to return a PMD sized page,
so implementing this function may not be necessary.  In particular,
filesystems should not call filemap_fault() from ->huge_fault().
The mmap_lock may not be held when this method is called.
(当不存在 PUD 或 PMD 表项时会调用 ->huge_fault()。这给了文件系统安装一个 PUD 或 PMD 大小页面的机会。文件系统也可以使用 ->fault 方法返回 PMD 大小的页面，因此实现此函数可能并非必要。特别是，文件系统不应从 ->huge_fault() 中调用 filemap_fault()。调用此方法时可能并未持有 mmap_lock。)

->map_pages() is called when VM asks to map easy accessible pages.
Filesystem should find and map pages associated with offsets from "start_pgoff"
till "end_pgoff". ->map_pages() is called with the RCU lock held and must
not block.  If it's not possible to reach a page without blocking,
filesystem should skip it. Filesystem should use set_pte_range() to setup
page table entry. Pointer to entry associated with the page is passed in
"pte" field in vm_fault structure. Pointers to entries for other offsets
should be calculated relative to "pte".
(当 VM 要求映射易于访问的页时会调用 ->map_pages()。文件系统应找到并映射与 "start_pgoff" 到 "end_pgoff" 之间的偏移相关联的页。->map_pages() 在持有 RCU 锁的情况下被调用，且不得阻塞。如果无法在不阻塞的情况下到达某个页，文件系统应跳过它。文件系统应使用 set_pte_range() 来建立页表项。与该页相关联的表项指针通过 vm_fault 结构中的 "pte" 字段传入。其他偏移的表项指针应相对于 "pte" 计算。)

->page_mkwrite() is called when a previously read-only pte is about to become
writeable. The filesystem again must ensure that there are no
truncate/invalidate races or races with operations such as ->remap_file_range
or ->copy_file_range, and then return with the page locked. Usually
mapping->invalidate_lock is suitable for proper serialization. If the page has
been truncated, the filesystem should not look up a new page like the ->fault()
handler, but simply return with VM_FAULT_NOPAGE, which will cause the VM to
retry the fault.
(当一个先前只读的 pte 即将变为可写时会调用 ->page_mkwrite()。文件系统同样必须确保不存在截断/失效竞态，以及与 ->remap_file_range 或 ->copy_file_range 之类操作的竞态，然后带着已锁定的页返回。通常 mapping->invalidate_lock 适合用于正确的串行化。如果该页已被截断，文件系统不应像 ->fault() 处理程序那样查找一个新页，而应简单地以 VM_FAULT_NOPAGE 返回，这会使 VM 重试缺页处理。)

->pfn_mkwrite() is the same as page_mkwrite but when the pte is
VM_PFNMAP or VM_MIXEDMAP with a page-less entry. Expected return is
VM_FAULT_NOPAGE. Or one of the VM_FAULT_ERROR types. The default behavior
after this call is to make the pte read-write, unless pfn_mkwrite returns
an error.
(->pfn_mkwrite() 与 page_mkwrite 相同，但针对的是 pte 为 VM_PFNMAP 或 VM_MIXEDMAP 且表项没有对应页的情况。预期的返回值是 VM_FAULT_NOPAGE，或某个 VM_FAULT_ERROR 类型。此调用之后的默认行为是把 pte 改为可读写，除非 pfn_mkwrite 返回错误。)

->access() is called when get_user_pages() fails in
access_process_vm(), typically used to debug a process through
/proc/pid/mem or ptrace.  This function is needed only for
VM_IO | VM_PFNMAP VMAs.
(当 access_process_vm() 中的 get_user_pages() 失败时会调用 ->access()，它通常用于通过 /proc/pid/mem 或 ptrace 调试一个进程。只有 VM_IO | VM_PFNMAP 类型的 VMA 才需要此函数。)

--------------------------------------------------------------------------------

			Dubious stuff
			(可疑之处)

(if you break something or notice that it is broken and do not fix it yourself
- at least put it here)
(如果你破坏了某些东西，或者注意到它已经损坏而你又不自行修复——至少请把它列在这里)
