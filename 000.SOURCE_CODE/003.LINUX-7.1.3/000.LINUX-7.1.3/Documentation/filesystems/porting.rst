====================
Changes since 2.5.0:
====================

(2.5.0 以来的变更：)

---

**recommended**
(建议)

New helpers: sb_bread(), sb_getblk(), sb_find_get_block(), set_bh(),
sb_set_blocksize() and sb_min_blocksize().
(新增辅助函数：sb_bread()、sb_getblk()、sb_find_get_block()、set_bh()、
sb_set_blocksize() 和 sb_min_blocksize()。)

Use them.
(请使用它们。)

(sb_find_get_block() replaces 2.4's get_hash_table())
((sb_find_get_block() 取代了 2.4 的 get_hash_table()))

---

**recommended**
(建议)

New methods: ->alloc_inode() and ->destroy_inode().
(新增方法：->alloc_inode() 和 ->destroy_inode()。)

Remove inode->u.foo_inode_i
(移除 inode->u.foo_inode_i。)

Declare::
(声明：)

	struct foo_inode_info {
		/* fs-private stuff */
		struct inode vfs_inode;
	};
	static inline struct foo_inode_info *FOO_I(struct inode *inode)
	{
		return list_entry(inode, struct foo_inode_info, vfs_inode);
	}

Use FOO_I(inode) instead of &inode->u.foo_inode_i;
(用 FOO_I(inode) 取代 &inode->u.foo_inode_i；)

Add foo_alloc_inode() and foo_destroy_inode() - the former should allocate
foo_inode_info and return the address of ->vfs_inode, the latter should free
FOO_I(inode) (see in-tree filesystems for examples).
(新增 foo_alloc_inode() 和 foo_destroy_inode() —— 前者应分配 foo_inode_info
并返回 ->vfs_inode 的地址，后者应释放 FOO_I(inode)(参见内核树内的文件系统
作为示例)。)

Make them ->alloc_inode and ->destroy_inode in your super_operations.
(在你的 super_operations 中把它们设为 ->alloc_inode 和 ->destroy_inode。)

Keep in mind that now you need explicit initialization of private data
typically between calling iget_locked() and unlocking the inode.
(请记住，现在你需要显式初始化私有数据，通常是在调用 iget_locked() 与解锁
该 inode 之间进行。)

At some point that will become mandatory.
(在将来的某个时候，这将成为强制要求。)

**mandatory**
(强制)

The foo_inode_info should always be allocated through alloc_inode_sb() rather
than kmem_cache_alloc() or kmalloc() related to set up the inode reclaim context
correctly.
(foo_inode_info 应当始终通过 alloc_inode_sb() 来分配，而不是 kmem_cache_alloc()
或 kmalloc()，以便正确设置 inode 回收上下文。)

---

**mandatory**
(强制)

Change of file_system_type method (->read_super to ->get_sb)
(file_system_type 方法变更(->read_super 改为 ->get_sb))

->read_super() is no more.  Ditto for DECLARE_FSTYPE and DECLARE_FSTYPE_DEV.
(->read_super() 已不复存在。DECLARE_FSTYPE 和 DECLARE_FSTYPE_DEV 也一样。)

Turn your foo_read_super() into a function that would return 0 in case of
success and negative number in case of error (-EINVAL unless you have more
informative error value to report).  Call it foo_fill_super().  Now declare::
(把你的 foo_read_super() 改写成一个在成功时返回 0、出错时返回负数的函数
(除非你有信息量更大的错误值可报告，否则就用 -EINVAL)。把它命名为
foo_fill_super()。现在声明：)

  int foo_get_sb(struct file_system_type *fs_type,
	int flags, const char *dev_name, void *data, struct vfsmount *mnt)
  {
	return get_sb_bdev(fs_type, flags, dev_name, data, foo_fill_super,
			   mnt);
  }

(or similar with s/bdev/nodev/ or s/bdev/single/, depending on the kind of
filesystem).
((或类似地用 s/bdev/nodev/ 或 s/bdev/single/ 替换，取决于文件系统的种类。))

Replace DECLARE_FSTYPE... with explicit initializer and have ->get_sb set as
foo_get_sb.
(用显式初始化器取代 DECLARE_FSTYPE...，并把 ->get_sb 设置为 foo_get_sb。)

---

**mandatory**
(强制)

Locking change: ->s_vfs_rename_sem is taken only by cross-directory renames.
Most likely there is no need to change anything, but if you relied on
global exclusion between renames for some internal purpose - you need to
change your internal locking.  Otherwise exclusion warranties remain the
same (i.e. parents and victim are locked, etc.).
(加锁变更：->s_vfs_rename_sem 现在只由跨目录重命名获取。很可能不需要更改
任何东西，但如果你曾出于某种内部目的而依赖重命名之间的全局互斥 —— 你需要
更改你的内部加锁。否则互斥保证保持不变(即父目录与被改名的对象会被锁定等)。)

---

**informational**
(仅供参考)

Now we have the exclusion between ->lookup() and directory removal (by
->rmdir() and ->rename()).  If you used to need that exclusion and do
it by internal locking (most of filesystems couldn't care less) - you
can relax your locking.
(现在 ->lookup() 与目录删除(通过 ->rmdir() 和 ->rename())之间有了互斥。
如果你过去需要这种互斥并通过内部加锁来实现(大多数文件系统对此毫不在意)
—— 你可以放宽你的加锁了。)

---

**mandatory**
(强制)

->lookup(), ->truncate(), ->create(), ->unlink(), ->mknod(), ->mkdir(),
->rmdir(), ->link(), ->lseek(), ->symlink(), ->rename()
and ->readdir() are called without BKL now.  Grab it on entry, drop upon return
- that will guarantee the same locking you used to have.  If your method or its
parts do not need BKL - better yet, now you can shift lock_kernel() and
unlock_kernel() so that they would protect exactly what needs to be
protected.
(->lookup()、->truncate()、->create()、->unlink()、->mknod()、->mkdir()、
->rmdir()、->link()、->lseek()、->symlink()、->rename() 和 ->readdir() 现在
在调用时不再持有 BKL。进入时获取它，返回时释放 —— 这将保证你原来拥有的
加锁。如果你的方法或其部分不需要 BKL —— 更好的是，现在你可以移动
lock_kernel() 和 unlock_kernel()，使它们恰好保护需要保护的内容。)

---

**mandatory**
(强制)

BKL is also moved from around sb operations. BKL should have been shifted into
individual fs sb_op functions.  If you don't need it, remove it.
(BKL 也从 sb 操作外围移走了。BKL 应当已被移入各个文件系统的 sb_op 函数中。
如果你不需要它，就将其移除。)

---

**informational**
(仅供参考)

check for ->link() target not being a directory is done by callers.  Feel
free to drop it...
(检查 ->link() 目标不是目录这一工作已由调用者完成。尽管放心地删掉它……)

---

**informational**
(仅供参考)

->link() callers hold ->i_mutex on the object we are linking to.  Some of your
problems might be over...
(->link() 的调用者在我们要链接到的对象上持有 ->i_mutex。你的某些问题也许
就此结束了……)

---

**mandatory**
(强制)

new file_system_type method - kill_sb(superblock).  If you are converting
an existing filesystem, set it according to ->fs_flags::
(新增 file_system_type 方法 —— kill_sb(超级块)。如果你正在转换一个现有的
文件系统，请根据 ->fs_flags 设置它：)

	FS_REQUIRES_DEV		-	kill_block_super
	FS_LITTER		-	kill_litter_super
	neither			-	kill_anon_super

FS_LITTER is gone - just remove it from fs_flags.
(FS_LITTER 已被移除 —— 直接把它从 fs_flags 中删掉即可。)

---

**mandatory**
(强制)

FS_SINGLE is gone (actually, that had happened back when ->get_sb()
went in - and hadn't been documented ;-/).  Just remove it from fs_flags
(and see ->get_sb() entry for other actions).
(FS_SINGLE 已被移除(实际上，早在 ->get_sb() 合入时就已发生 —— 只是从未
记录过 ;-/)。直接把它从 fs_flags 中删掉即可(其他操作参见 ->get_sb() 条目)。)

---

**mandatory**
(强制)

->setattr() is called without BKL now.  Caller _always_ holds ->i_mutex, so
watch for ->i_mutex-grabbing code that might be used by your ->setattr().
Callers of notify_change() need ->i_mutex now.
(->setattr() 现在在调用时不再持有 BKL。调用者_总是_持有 ->i_mutex，因此
留意可能被你的 ->setattr() 用到的获取 ->i_mutex 的代码。notify_change() 的
调用者现在需要 ->i_mutex。)

---

**recommended**
(建议)

New super_block field ``struct export_operations *s_export_op`` for
explicit support for exporting, e.g. via NFS.  The structure is fully
documented at its declaration in include/linux/fs.h, and in
Documentation/filesystems/nfs/exporting.rst.
(新增 super_block 字段 ``struct export_operations *s_export_op``，用于显式
支持导出，例如通过 NFS。该结构在其声明处(include/linux/fs.h 中)以及
Documentation/filesystems/nfs/exporting.rst 中有完整文档。)

Briefly it allows for the definition of decode_fh and encode_fh operations
to encode and decode filehandles, and allows the filesystem to use
a standard helper function for decode_fh, and provide file-system specific
support for this helper, particularly get_parent.
(简而言之，它允许定义 decode_fh 和 encode_fh 操作来编码与解码文件句柄，
并允许文件系统对 decode_fh 使用一个标准辅助函数，同时为该辅助函数提供
文件系统特定的支持，尤其是 get_parent。)

It is planned that this will be required for exporting once the code
settles down a bit.
(计划在代码稍微稳定下来之后，导出时将要求使用它。)

**mandatory**
(强制)

s_export_op is now required for exporting a filesystem.
isofs, ext2, ext3, fat
can be used as examples of very different filesystems.
(现在导出一个文件系统需要 s_export_op。isofs、ext2、ext3、fat 可用作截然
不同的文件系统的示例。)

---

**mandatory**
(强制)

iget4() and the read_inode2 callback have been superseded by iget5_locked()
which has the following prototype::
(iget4() 和 read_inode2 回调已被 iget5_locked() 取代，后者具有如下原型：)

    struct inode *iget5_locked(struct super_block *sb, unsigned long ino,
				int (*test)(struct inode *, void *),
				int (*set)(struct inode *, void *),
				void *data);

'test' is an additional function that can be used when the inode
number is not sufficient to identify the actual file object. 'set'
should be a non-blocking function that initializes those parts of a
newly created inode to allow the test function to succeed. 'data' is
passed as an opaque value to both test and set functions.
('test' 是一个附加函数，当 inode 编号不足以标识实际文件对象时可以使用它。
'set' 应当是一个非阻塞函数，它初始化新建 inode 的相关部分，使 test 函数
能够成功。'data' 作为不透明的值传递给 test 和 set 两个函数。)

When the inode has been created by iget5_locked(), it will be returned with the
I_NEW flag set and will still be locked.  The filesystem then needs to finalize
the initialization. Once the inode is initialized it must be unlocked by
calling unlock_new_inode().
(当 inode 由 iget5_locked() 创建时，它返回时带有 I_NEW 标志且仍处于锁定
状态。文件系统随后需要完成初始化。一旦 inode 初始化完毕，必须调用
unlock_new_inode() 将其解锁。)

The filesystem is responsible for setting (and possibly testing) i_ino
when appropriate. There is also a simpler iget_locked function that
just takes the superblock and inode number as arguments and does the
test and set for you.
(文件系统负责在适当的时候设置(以及可能的测试) i_ino。还有一个更简单的
iget_locked 函数，它只接受超级块和 inode 编号作为参数，并替你完成 test
和 set。)

e.g.::
(例如：)

	inode = iget_locked(sb, ino);
	if (inode_state_read_once(inode) & I_NEW) {
		err = read_inode_from_disk(inode);
		if (err < 0) {
			iget_failed(inode);
			return err;
		}
		unlock_new_inode(inode);
	}

Note that if the process of setting up a new inode fails, then iget_failed()
should be called on the inode to render it dead, and an appropriate error
should be passed back to the caller.
(注意，如果设置新 inode 的过程失败，则应在该 inode 上调用 iget_failed()
使其作废，并向调用者传回适当的错误。)

---

**recommended**
(建议)

->getattr() finally getting used.  See instances in nfs, minix, etc.
(->getattr() 终于得到使用了。参见 nfs、minix 等中的实例。)

---

**mandatory**
(强制)

->revalidate() is gone.  If your filesystem had it - provide ->getattr()
and let it call whatever you had as ->revlidate() + (for symlinks that
had ->revalidate()) add calls in ->follow_link()/->readlink().
(->revalidate() 已被移除。如果你的文件系统曾有它 —— 提供 ->getattr()，让它
调用你原来作为 ->revlidate() 的东西 + (对于曾有 ->revalidate() 的符号链接)
在 ->follow_link()/->readlink() 中添加调用。)

---

**mandatory**
(强制)

->d_parent changes are not protected by BKL anymore.  Read access is safe
if at least one of the following is true:
(->d_parent 的变更不再受 BKL 保护。如果以下条件至少有一条成立，读取访问
就是安全的：)

	* filesystem has no cross-directory rename()
	  (文件系统没有跨目录的 rename()。)
	* we know that parent had been locked (e.g. we are looking at
	  ->d_parent of ->lookup() argument).
	  (我们知道父目录已被锁定(例如我们正在查看 ->lookup() 参数的
	  ->d_parent)。)
	* we are called from ->rename().
	  (我们是从 ->rename() 中被调用的。)
	* the child's ->d_lock is held
	  (子项的 ->d_lock 已被持有。)

Audit your code and add locking if needed.  Notice that any place that is
not protected by the conditions above is risky even in the old tree - you
had been relying on BKL and that's prone to screwups.  Old tree had quite
a few holes of that kind - unprotected access to ->d_parent leading to
anything from oops to silent memory corruption.
(审计你的代码并在需要时添加加锁。注意，任何不受上述条件保护的地方即使在
旧内核树中也是有风险的 —— 你一直依赖的是 BKL，而这很容易出岔子。旧内核树
中有不少这类漏洞 —— 对 ->d_parent 的未保护访问导致从 oops 到静默内存损坏
的各种问题。)

---

**mandatory**
(强制)

FS_NOMOUNT is gone.  If you use it - just set SB_NOUSER in flags
(see rootfs for one kind of solution and bdev/socket/pipe for another).
(FS_NOMOUNT 已被移除。如果你在用它 —— 直接在 flags 中设置 SB_NOUSER 即可
(一种解决方案参见 rootfs，另一种参见 bdev/socket/pipe)。)

---

**recommended**
(建议)

Use bdev_read_only(bdev) instead of is_read_only(kdev).  The latter
is still alive, but only because of the mess in drivers/s390/block/dasd.c.
As soon as it gets fixed is_read_only() will die.
(用 bdev_read_only(bdev) 取代 is_read_only(kdev)。后者仍然存在，但只是因为
drivers/s390/block/dasd.c 中的一团糟。一旦它被修复，is_read_only() 就会
消亡。)

---

**mandatory**
(强制)

->permission() is called without BKL now. Grab it on entry, drop upon
return - that will guarantee the same locking you used to have.  If
your method or its parts do not need BKL - better yet, now you can
shift lock_kernel() and unlock_kernel() so that they would protect
exactly what needs to be protected.
(->permission() 现在在调用时不再持有 BKL。进入时获取它，返回时释放 ——
这将保证你原来拥有的加锁。如果你的方法或其部分不需要 BKL —— 更好的是，
现在你可以移动 lock_kernel() 和 unlock_kernel()，使它们恰好保护需要保护的
内容。)

---

**mandatory**
(强制)

->statfs() is now called without BKL held.  BKL should have been
shifted into individual fs sb_op functions where it's not clear that
it's safe to remove it.  If you don't need it, remove it.
(->statfs() 现在在调用时不再持有 BKL。在尚不清楚移除它是否安全的地方，
BKL 应当已被移入各个文件系统的 sb_op 函数中。如果你不需要它，就将其移除。)

---

**mandatory**
(强制)

is_read_only() is gone; use bdev_read_only() instead.
(is_read_only() 已被移除；改用 bdev_read_only()。)

---

**mandatory**
(强制)

destroy_buffers() is gone; use invalidate_bdev().
(destroy_buffers() 已被移除；改用 invalidate_bdev()。)

---

**mandatory**
(强制)

fsync_dev() is gone; use fsync_bdev().  NOTE: lvm breakage is
deliberate; as soon as struct block_device * is propagated in a reasonable
way by that code fixing will become trivial; until then nothing can be
done.
(fsync_dev() 已被移除；改用 fsync_bdev()。注意：lvm 的破坏是故意的；一旦
struct block_device * 以合理的方式被该代码传递，修复将变得轻而易举；在那
之前无能为力。)

**mandatory**
(强制)

block truncation on error exit from ->write_begin, and ->direct_IO
moved from generic methods (block_write_begin, cont_write_begin,
nobh_write_begin, blockdev_direct_IO*) to callers.  Take a look at
ext2_write_failed and callers for an example.
(从 ->write_begin 出错退出时的块截断，以及 ->direct_IO，已从通用方法
(block_write_begin、cont_write_begin、nobh_write_begin、
blockdev_direct_IO*)移至调用者。示例请看 ext2_write_failed 及其调用者。)

**mandatory**
(强制)

->truncate is gone.  The whole truncate sequence needs to be
implemented in ->setattr, which is now mandatory for filesystems
implementing on-disk size changes.  Start with a copy of the old inode_setattr
and vmtruncate, and the reorder the vmtruncate + foofs_vmtruncate sequence to
be in order of zeroing blocks using block_truncate_page or similar helpers,
size update and on finally on-disk truncation which should not fail.
setattr_prepare (which used to be inode_change_ok) now includes the size checks
for ATTR_SIZE and must be called in the beginning of ->setattr unconditionally.
(->truncate 已被移除。整个截断序列需要在 ->setattr 中实现，对于实现磁盘上
大小更改的文件系统来说这现在是强制的。从旧 inode_setattr 和 vmtruncate 的
副本开始，重新排列 vmtruncate + foofs_vmtruncate 序列的顺序：先使用
block_truncate_page 或类似辅助函数把块清零，然后更新大小，最后进行不应
失败的磁盘上截断。setattr_prepare(以前叫 inode_change_ok)现在包含对
ATTR_SIZE 的大小检查，并且必须在 ->setattr 的开头无条件调用。)

**mandatory**
(强制)

->clear_inode() and ->delete_inode() are gone; ->evict_inode() should
be used instead.  It gets called whenever the inode is evicted, whether it has
remaining links or not.  Caller does *not* evict the pagecache or inode-associated
metadata buffers; the method has to use truncate_inode_pages_final() to get rid
of those. Caller makes sure async writeback cannot be running for the inode while
(or after) ->evict_inode() is called.
(->clear_inode() 和 ->delete_inode() 已被移除；应改用 ->evict_inode()。
每当 inode 被逐出时都会调用它，无论它是否还有剩余链接。调用者*不会*逐出
页缓存或与 inode 关联的元数据缓冲；该方法必须使用
truncate_inode_pages_final() 来清除它们。调用者确保在 ->evict_inode() 被
调用期间(或之后)不会有针对该 inode 的异步回写在运行。)

->drop_inode() returns int now; it's called on final iput() with
inode->i_lock held and it returns true if filesystems wants the inode to be
dropped.  As before, inode_generic_drop() is still the default and it's been
updated appropriately.  inode_just_drop() is also alive and it consists
simply of return 1.  Note that all actual eviction work is done by caller after
->drop_inode() returns.
(->drop_inode() 现在返回 int；它在最终的 iput() 时被调用，此时持有
inode->i_lock，如果文件系统希望丢弃该 inode 则返回 true。与以前一样，
inode_generic_drop() 仍是默认值，并已相应更新。inode_just_drop() 也仍然
存在，它只是由 return 1 组成。注意，所有实际的逐出工作都是在
->drop_inode() 返回之后由调用者完成的。)

As before, clear_inode() must be called exactly once on each call of
->evict_inode() (as it used to be for each call of ->delete_inode()).  Unlike
before, if you are using inode-associated metadata buffers (i.e.
mark_buffer_dirty_inode()), it's your responsibility to call
invalidate_inode_buffers() before clear_inode().
(与以前一样，每次调用 ->evict_inode() 时都必须恰好调用一次 clear_inode()
(就像以前每次调用 ->delete_inode() 时那样)。与以前不同的是，如果你使用与
inode 关联的元数据缓冲(即 mark_buffer_dirty_inode())，则有责任在
clear_inode() 之前调用 invalidate_inode_buffers()。)

NOTE: checking i_nlink in the beginning of ->write_inode() and bailing out
if it's zero is not *and* *never* *had* *been* enough.  Final unlink() and iput()
may happen while the inode is in the middle of ->write_inode(); e.g. if you blindly
free the on-disk inode, you may end up doing that while ->write_inode() is writing
to it.
(注意：在 ->write_inode() 的开头检查 i_nlink 并在其为零时提前退出，这并
*不*足够，而且*从*来*不曾*足够过。最终的 unlink() 和 iput() 可能发生在
inode 正处于 ->write_inode() 中途之时；例如，如果你盲目地释放磁盘上的
inode，你可能恰好在 ->write_inode() 正在写入它的时候这样做。)

---

**mandatory**
(强制)

.d_delete() now only advises the dcache as to whether or not to cache
unreferenced dentries, and is now only called when the dentry refcount goes to
0. Even on 0 refcount transition, it must be able to tolerate being called 0,
1, or more times (eg. constant, idempotent).
(.d_delete() 现在只是就是否缓存未被引用的 dentry 向 dcache 提供建议，并且
现在只在 dentry 引用计数变为 0 时被调用。即使是在变为 0 引用计数的转换时，
它也必须能够容忍被调用 0 次、1 次或更多次(例如：常量、幂等)。)

---

**mandatory**
(强制)

.d_compare() calling convention and locking rules are significantly
changed. Read updated documentation in Documentation/filesystems/vfs.rst (and
look at examples of other filesystems) for guidance.
(.d_compare() 的调用约定和加锁规则发生了重大变化。请阅读
Documentation/filesystems/vfs.rst 中更新后的文档(并参考其他文件系统的示例)
以获得指导。)

---

**mandatory**
(强制)

.d_hash() calling convention and locking rules are significantly
changed. Read updated documentation in Documentation/filesystems/vfs.rst (and
look at examples of other filesystems) for guidance.
(.d_hash() 的调用约定和加锁规则发生了重大变化。请阅读
Documentation/filesystems/vfs.rst 中更新后的文档(并参考其他文件系统的示例)
以获得指导。)

---

**mandatory**
(强制)

dcache_lock is gone, replaced by fine grained locks. See fs/dcache.c
for details of what locks to replace dcache_lock with in order to protect
particular things. Most of the time, a filesystem only needs ->d_lock, which
protects *all* the dcache state of a given dentry.
(dcache_lock 已被移除，由细粒度的锁取代。关于用什么锁来取代 dcache_lock
以保护特定事项的细节，请参见 fs/dcache.c。大多数情况下，文件系统只需要
->d_lock，它保护给定 dentry 的*所有* dcache 状态。)

---

**mandatory**
(强制)

Filesystems must RCU-free their inodes, if they can have been accessed
via rcu-walk path walk (basically, if the file can have had a path name in the
vfs namespace).
(如果文件系统中的 inode 可能通过 rcu-walk 路径遍历被访问过(基本上，如果该
文件在 vfs 命名空间中可能有过路径名)，则文件系统必须以 RCU 方式释放其
inode。)

Even though i_dentry and i_rcu share storage in a union, we will
initialize the former in inode_init_always(), so just leave it alone in
the callback.  It used to be necessary to clean it there, but not anymore
(starting at 3.2).
(尽管 i_dentry 和 i_rcu 在一个联合体中共享存储，我们会在
inode_init_always() 中初始化前者，所以在回调中不要动它。过去需要在回调中
清理它，但现在不再需要了(从 3.2 开始)。)

---

**recommended**
(建议)

vfs now tries to do path walking in "rcu-walk mode", which avoids
atomic operations and scalability hazards on dentries and inodes (see
Documentation/filesystems/path-lookup.txt). d_hash and d_compare changes
(above) are examples of the changes required to support this. For more complex
filesystem callbacks, the vfs drops out of rcu-walk mode before the fs call, so
no changes are required to the filesystem. However, this is costly and loses
the benefits of rcu-walk mode. We will begin to add filesystem callbacks that
are rcu-walk aware, shown below. Filesystems should take advantage of this
where possible.
(vfs 现在尝试在"rcu-walk 模式"下进行路径遍历，这避免了 dentry 和 inode 上
的原子操作与可扩展性隐患(参见
Documentation/filesystems/path-lookup.txt)。d_hash 和 d_compare 的变更
(如上)是支持这一点所需的变更示例。对于更复杂的文件系统回调，vfs 会在调用
文件系统之前退出 rcu-walk 模式，因此文件系统无需任何更改。然而，这样代价
高昂并失去了 rcu-walk 模式的好处。我们将开始添加能感知 rcu-walk 的文件系统
回调，如下所示。文件系统应尽可能利用这一点。)

---

**mandatory**
(强制)

d_revalidate is a callback that is made on every path element (if
the filesystem provides it), which requires dropping out of rcu-walk mode. This
may now be called in rcu-walk mode (nd->flags & LOOKUP_RCU). -ECHILD should be
returned if the filesystem cannot handle rcu-walk. See
Documentation/filesystems/vfs.rst for more details.
(d_revalidate 是在每个路径元素上进行的回调(如果文件系统提供了它)，它要求
退出 rcu-walk 模式。现在它可能在 rcu-walk 模式下被调用
(nd->flags & LOOKUP_RCU)。如果文件系统无法处理 rcu-walk，应返回 -ECHILD。
更多细节参见 Documentation/filesystems/vfs.rst。)

permission is an inode permission check that is called on many or all
directory inodes on the way down a path walk (to check for exec permission). It
must now be rcu-walk aware (mask & MAY_NOT_BLOCK).  See
Documentation/filesystems/vfs.rst for more details.
(permission 是一个 inode 权限检查，在路径遍历下行途中会对许多或所有目录
inode 调用它(以检查执行权限)。它现在必须能感知 rcu-walk
(mask & MAY_NOT_BLOCK)。更多细节参见
Documentation/filesystems/vfs.rst。)

---

**mandatory**
(强制)

In ->fallocate() you must check the mode option passed in.  If your
filesystem does not support hole punching (deallocating space in the middle of a
file) you must return -EOPNOTSUPP if FALLOC_FL_PUNCH_HOLE is set in mode.
Currently you can only have FALLOC_FL_PUNCH_HOLE with FALLOC_FL_KEEP_SIZE set,
so the i_size should not change when hole punching, even when puching the end of
a file off.
(在 ->fallocate() 中你必须检查传入的 mode 选项。如果你的文件系统不支持打洞
(释放文件中间的空间)，那么当 mode 中设置了 FALLOC_FL_PUNCH_HOLE 时你必须
返回 -EOPNOTSUPP。当前 FALLOC_FL_PUNCH_HOLE 只能与 FALLOC_FL_KEEP_SIZE 一起
设置，因此打洞时 i_size 不应改变，即使是把文件末尾打掉也是如此。)

---

**mandatory**
(强制)

->get_sb() and ->mount() are gone. Switch to using the new mount API. See
Documentation/filesystems/mount_api.rst for more details.
(->get_sb() 和 ->mount() 已被移除。请改用新的挂载 API。更多细节参见
Documentation/filesystems/mount_api.rst。)

---

**mandatory**
(强制)

->permission() and generic_permission()have lost flags
argument; instead of passing IPERM_FLAG_RCU we add MAY_NOT_BLOCK into mask.
(->permission() 和 generic_permission() 失去了 flags 参数；我们现在不传
IPERM_FLAG_RCU，而是把 MAY_NOT_BLOCK 加进 mask。)

generic_permission() has also lost the check_acl argument; ACL checking
has been taken to VFS and filesystems need to provide a non-NULL
->i_op->get_inode_acl to read an ACL from disk.
(generic_permission() 也失去了 check_acl 参数；ACL 检查已移交给 VFS，文件
系统需要提供非 NULL 的 ->i_op->get_inode_acl 以从磁盘读取 ACL。)

---

**mandatory**
(强制)

If you implement your own ->llseek() you must handle SEEK_HOLE and
SEEK_DATA.  You can handle this by returning -EINVAL, but it would be nicer to
support it in some way.  The generic handler assumes that the entire file is
data and there is a virtual hole at the end of the file.  So if the provided
offset is less than i_size and SEEK_DATA is specified, return the same offset.
If the above is true for the offset and you are given SEEK_HOLE, return the end
of the file.  If the offset is i_size or greater return -ENXIO in either case.
(如果你实现自己的 ->llseek()，你必须处理 SEEK_HOLE 和 SEEK_DATA。你可以
通过返回 -EINVAL 来处理，但以某种方式支持它会更好。通用处理程序假定整个文件
都是数据，并且在文件末尾有一个虚拟的洞。因此，如果给定的偏移小于 i_size 且
指定了 SEEK_DATA，则返回相同的偏移。如果偏移满足上述条件且你收到的是
SEEK_HOLE，则返回文件末尾。如果偏移大于等于 i_size，两种情况下都返回
-ENXIO。)

**mandatory**
(强制)

If you have your own ->fsync() you must make sure to call
filemap_write_and_wait_range() so that all dirty pages are synced out properly.
You must also keep in mind that ->fsync() is not called with i_mutex held
anymore, so if you require i_mutex locking you must make sure to take it and
release it yourself.
(如果你有自己的 ->fsync()，你必须确保调用 filemap_write_and_wait_range()，
以便所有脏页被正确地同步写出。你还必须记住，->fsync() 不再在持有 i_mutex
的情况下被调用，因此如果你需要 i_mutex 加锁，你必须确保自己获取并释放它。)

---

**mandatory**
(强制)

d_alloc_root() is gone, along with a lot of bugs caused by code
misusing it.  Replacement: d_make_root(inode).  On success d_make_root(inode)
allocates and returns a new dentry instantiated with the passed in inode.
On failure NULL is returned and the passed in inode is dropped so the reference
to inode is consumed in all cases and failure handling need not do any cleanup
for the inode.  If d_make_root(inode) is passed a NULL inode it returns NULL
and also requires no further error handling. Typical usage is::
(d_alloc_root() 已被移除，连同许多因代码滥用它而导致的 bug 一起。替代者：
d_make_root(inode)。成功时 d_make_root(inode) 分配并返回一个用传入 inode
实例化的新 dentry。失败时返回 NULL，且传入的 inode 被丢弃，因此对 inode 的
引用在所有情况下都被消耗掉，失败处理无需为该 inode 做任何清理。如果传给
d_make_root(inode) 的是 NULL inode，它返回 NULL，同样无需进一步的错误处理。
典型用法：)

	inode = foofs_new_inode(....);
	s->s_root = d_make_root(inode);
	if (!s->s_root)
		/* Nothing needed for the inode cleanup */
		return -ENOMEM;
	...

---

**mandatory**
(强制)

The witch is dead!  Well, 2/3 of it, anyway.  ->d_revalidate() and
->lookup() do *not* take struct nameidata anymore; just the flags.
(女巫死了！嗯，反正死了 2/3。->d_revalidate() 和 ->lookup() *不再*接受
struct nameidata；只接受 flags。)

---

**mandatory**
(强制)

->create() doesn't take ``struct nameidata *``; unlike the previous
two, it gets "is it an O_EXCL or equivalent?" boolean argument.  Note that
local filesystems can ignore this argument - they are guaranteed that the
object doesn't exist.  It's remote/distributed ones that might care...
(->create() 不再接受 ``struct nameidata *``；与前两者不同，它接受一个
"它是否为 O_EXCL 或等价物？"的布尔参数。注意，本地文件系统可以忽略这个
参数 —— 它们保证对象不存在。可能在意的是远程/分布式文件系统……)

---

**mandatory**
(强制)

FS_REVAL_DOT is gone; if you used to have it, add ->d_weak_revalidate()
in your dentry operations instead.
(FS_REVAL_DOT 已被移除；如果你过去有它，请改为在你的 dentry 操作中添加
->d_weak_revalidate()。)

---

**mandatory**
(强制)

vfs_readdir() is gone; switch to iterate_dir() instead
(vfs_readdir() 已被移除；请改用 iterate_dir())

---

**mandatory**
(强制)

->readdir() is gone now; switch to ->iterate_shared()
(->readdir() 现已被移除；请改用 ->iterate_shared())

**mandatory**
(强制)

vfs_follow_link has been removed.  Filesystems must use nd_set_link
from ->follow_link for normal symlinks, or nd_jump_link for magic
/proc/<pid> style links.
(vfs_follow_link 已被移除。文件系统对于普通符号链接必须在 ->follow_link
中使用 nd_set_link，对于魔术般的 /proc/<pid> 式链接则使用 nd_jump_link。)

---

**mandatory**
(强制)

iget5_locked()/ilookup5()/ilookup5_nowait() test() callback used to be
called with both ->i_lock and inode_hash_lock held; the former is *not*
taken anymore, so verify that your callbacks do not rely on it (none
of the in-tree instances did).  inode_hash_lock is still held,
of course, so they are still serialized wrt removal from inode hash,
as well as wrt set() callback of iget5_locked().
(iget5_locked()/ilookup5()/ilookup5_nowait() 的 test() 回调过去在同时持有
->i_lock 和 inode_hash_lock 的情况下被调用；前者*不再*被获取，因此请验证
你的回调不依赖它(内核树内的实例都没有依赖)。当然，inode_hash_lock 仍然被
持有，因此它们仍然相对于从 inode 哈希中的移除以及 iget5_locked() 的 set()
回调是串行化的。)

---

**mandatory**
(强制)

d_materialise_unique() is gone; d_splice_alias() does everything you
need now.  Remember that they have opposite orders of arguments ;-/
(d_materialise_unique() 已被移除；d_splice_alias() 现在能做你需要的一切。
记住，它们的参数顺序正好相反 ;-/)

---

**mandatory**
(强制)

f_dentry is gone; use f_path.dentry, or, better yet, see if you can avoid
it entirely.
(f_dentry 已被移除；使用 f_path.dentry，或者更好的是，看看能否完全避开它。)

---

**mandatory**
(强制)

never call ->read() and ->write() directly; use __vfs_{read,write} or
wrappers; instead of checking for ->write or ->read being NULL, look for
FMODE_CAN_{WRITE,READ} in file->f_mode.
(绝不要直接调用 ->read() 和 ->write()；使用 __vfs_{read,write} 或其包装
函数；不要检查 ->write 或 ->read 是否为 NULL，而是查看 file->f_mode 中的
FMODE_CAN_{WRITE,READ}。)

---

**mandatory**
(强制)

do _not_ use new_sync_{read,write} for ->read/->write; leave it NULL
instead.
(_不要_在 ->read/->write 中使用 new_sync_{read,write}；改为将其保留为
NULL。)

---

**mandatory**
(强制)
	->aio_read/->aio_write are gone.  Use ->read_iter/->write_iter.
	(->aio_read/->aio_write 已被移除。请使用 ->read_iter/->write_iter。)

---

**recommended**
(建议)

for embedded ("fast") symlinks just set inode->i_link to wherever the
symlink body is and use simple_follow_link() as ->follow_link().
(对于内嵌的("快速")符号链接，只需把 inode->i_link 设置为符号链接体所在的
位置，并使用 simple_follow_link() 作为 ->follow_link()。)

---

**mandatory**
(强制)

calling conventions for ->follow_link() have changed.  Instead of returning
cookie and using nd_set_link() to store the body to traverse, we return
the body to traverse and store the cookie using explicit void ** argument.
nameidata isn't passed at all - nd_jump_link() doesn't need it and
nd_[gs]et_link() is gone.
(->follow_link() 的调用约定已改变。我们不再返回 cookie 并用 nd_set_link()
存储要遍历的链接体，而是返回要遍历的链接体，并用显式的 void ** 参数存储
cookie。完全不再传递 nameidata —— nd_jump_link() 不需要它，而
nd_[gs]et_link() 已被移除。)

---

**mandatory**
(强制)

calling conventions for ->put_link() have changed.  It gets inode instead of
dentry,  it does not get nameidata at all and it gets called only when cookie
is non-NULL.  Note that link body isn't available anymore, so if you need it,
store it as cookie.
(->put_link() 的调用约定已改变。它接受 inode 而不是 dentry，完全不接受
nameidata，并且只在 cookie 非 NULL 时才被调用。注意，链接体不再可用，因此
如果你需要它，把它存为 cookie。)

---

**mandatory**
(强制)

any symlink that might use page_follow_link_light/page_put_link() must
have inode_nohighmem(inode) called before anything might start playing with
its pagecache.  No highmem pages should end up in the pagecache of such
symlinks.  That includes any preseeding that might be done during symlink
creation.  page_symlink() will honour the mapping gfp flags, so once
you've done inode_nohighmem() it's safe to use, but if you allocate and
insert the page manually, make sure to use the right gfp flags.
(任何可能使用 page_follow_link_light/page_put_link() 的符号链接，都必须在
任何东西可能开始摆弄其页缓存之前调用 inode_nohighmem(inode)。这类符号链接
的页缓存中不应出现高端内存页。这包括在符号链接创建期间可能进行的任何预
填充。page_symlink() 会遵循映射的 gfp 标志，因此一旦你做了
inode_nohighmem()，使用它就是安全的，但如果你手动分配并插入页，请确保使用
正确的 gfp 标志。)

---

**mandatory**
(强制)

->follow_link() is replaced with ->get_link(); same API, except that
(->follow_link() 被 ->get_link() 取代；API 相同，除了以下几点：)

	* ->get_link() gets inode as a separate argument
	  (->get_link() 单独接受一个 inode 参数)
	* ->get_link() may be called in RCU mode - in that case NULL
	  dentry is passed
	  (->get_link() 可能在 RCU 模式下被调用 —— 在这种情况下传入
	  NULL dentry)

---

**mandatory**
(强制)

->get_link() gets struct delayed_call ``*done`` now, and should do
set_delayed_call() where it used to set ``*cookie``.
(->get_link() 现在接受 struct delayed_call ``*done``，应在过去设置
``*cookie`` 的地方调用 set_delayed_call()。)

->put_link() is gone - just give the destructor to set_delayed_call()
in ->get_link().
(->put_link() 已被移除 —— 只需在 ->get_link() 中把析构函数交给
set_delayed_call()。)

---

**mandatory**
(强制)

->getxattr() and xattr_handler.get() get dentry and inode passed separately.
dentry might be yet to be attached to inode, so do _not_ use its ->d_inode
in the instances.  Rationale: !@#!@# security_d_instantiate() needs to be
called before we attach dentry to inode.
(->getxattr() 和 xattr_handler.get() 被分别传入 dentry 和 inode。dentry
可能尚未关联到 inode，因此在实例中_不要_使用它的 ->d_inode。理由：
!@#!@# security_d_instantiate() 需要在我们把 dentry 关联到 inode 之前
被调用。)

---

**mandatory**
(强制)

symlinks are no longer the only inodes that do *not* have i_bdev/i_cdev/
i_pipe/i_link union zeroed out at inode eviction.  As the result, you can't
assume that non-NULL value in ->i_nlink at ->destroy_inode() implies that
it's a symlink.  Checking ->i_mode is really needed now.  In-tree we had
to fix shmem_destroy_callback() that used to take that kind of shortcut;
watch out, since that shortcut is no longer valid.
(符号链接不再是*唯一*在 inode 逐出时不把 i_bdev/i_cdev/i_pipe/i_link 联合体
清零的 inode。因此，你不能假定 ->destroy_inode() 时 ->i_nlink 中的非 NULL
值意味着它是一个符号链接。现在确实需要检查 ->i_mode。内核树内我们不得不
修复过去走那种捷径的 shmem_destroy_callback()；当心，因为那种捷径不再
有效。)

---

**mandatory**
(强制)

->i_mutex is replaced with ->i_rwsem now.  inode_lock() et.al. work as
they used to - they just take it exclusive.  However, ->lookup() may be
called with parent locked shared.  Its instances must not
(->i_mutex 现在被 ->i_rwsem 取代。inode_lock() 等的工作方式与从前一样 ——
只是它们以独占方式获取它。然而，->lookup() 可能在父目录以共享方式加锁的
情况下被调用。其实例绝不可以：)

	* use d_instantiate) and d_rehash() separately - use d_add() or
	  d_splice_alias() instead.
	  (分别使用 d_instantiate) 和 d_rehash() —— 请改用 d_add() 或
	  d_splice_alias()。)
	* use d_rehash() alone - call d_add(new_dentry, NULL) instead.
	  (单独使用 d_rehash() —— 请改为调用 d_add(new_dentry, NULL)。)
	* in the unlikely case when (read-only) access to filesystem
	  data structures needs exclusion for some reason, arrange it
	  yourself.  None of the in-tree filesystems needed that.
	  (在极罕见的情况下，(只读)访问文件系统数据结构出于某种原因需要
	  互斥时，自行安排。内核树内的文件系统都不需要那样做。)
	* rely on ->d_parent and ->d_name not changing after dentry has
	  been fed to d_add() or d_splice_alias().  Again, none of the
	  in-tree instances relied upon that.
	  (依赖 dentry 被交给 d_add() 或 d_splice_alias() 之后 ->d_parent
	  和 ->d_name 不再改变。同样，内核树内的实例都没有依赖于此。)

We are guaranteed that lookups of the same name in the same directory
will not happen in parallel ("same" in the sense of your ->d_compare()).
Lookups on different names in the same directory can and do happen in
parallel now.
(我们可以保证在同一目录中对相同名字的查找不会并行发生("相同"是就你的
->d_compare() 而言)。现在，对同一目录中不同名字的查找可以且确实会并行
发生。)

---

**mandatory**
(强制)

->iterate_shared() is added.
Exclusion on struct file level is still provided (as well as that
between it and lseek on the same struct file), but if your directory
has been opened several times, you can get these called in parallel.
Exclusion between that method and all directory-modifying ones is
still provided, of course.
(新增了 ->iterate_shared()。在 struct file 层面上的互斥仍然提供(以及它与
同一 struct file 上 lseek 之间的互斥)，但如果你的目录被打开了多次，这些
调用可能并行发生。当然，该方法与所有修改目录的方法之间的互斥仍然提供。)

If you have any per-inode or per-dentry in-core data structures modified
by ->iterate_shared(), you might need something to serialize the access
to them.  If you do dcache pre-seeding, you'll need to switch to
d_alloc_parallel() for that; look for in-tree examples.
(如果你有任何由 ->iterate_shared() 修改的每 inode 或每 dentry 的内存中数据
结构，你可能需要某种东西来串行化对它们的访问。如果你做 dcache 预填充，你
将需要为此改用 d_alloc_parallel()；请在内核树内寻找示例。)

---

**mandatory**
(强制)

->atomic_open() calls without O_CREAT may happen in parallel.
(不带 O_CREAT 的 ->atomic_open() 调用可能并行发生。)

---

**mandatory**
(强制)

->setxattr() and xattr_handler.set() get dentry and inode passed separately.
The xattr_handler.set() gets passed the user namespace of the mount the inode
is seen from so filesystems can idmap the i_uid and i_gid accordingly.
dentry might be yet to be attached to inode, so do _not_ use its ->d_inode
in the instances.  Rationale: !@#!@# security_d_instantiate() needs to be
called before we attach dentry to inode and !@#!@##!@$!$#!@#$!@$!@$ smack
->d_instantiate() uses not just ->getxattr() but ->setxattr() as well.
(->setxattr() 和 xattr_handler.set() 被分别传入 dentry 和 inode。
xattr_handler.set() 被传入看到该 inode 的那个挂载的用户命名空间，以便文件
系统可以相应地对 i_uid 和 i_gid 做 idmap。dentry 可能尚未关联到 inode，因此
在实例中_不要_使用它的 ->d_inode。理由：!@#!@# security_d_instantiate()
需要在我们把 dentry 关联到 inode 之前被调用，而且 !@#!@##!@$!$#!@#$!@$!@$
smack 的 ->d_instantiate() 不仅使用 ->getxattr()，还使用 ->setxattr()。)

---

**mandatory**
(强制)

->d_compare() doesn't get parent as a separate argument anymore.  If you
used it for finding the struct super_block involved, dentry->d_sb will
work just as well; if it's something more complicated, use dentry->d_parent.
Just be careful not to assume that fetching it more than once will yield
the same value - in RCU mode it could change under you.
(->d_compare() 不再单独接受 parent 参数。如果你用它来找到所涉及的
struct super_block，dentry->d_sb 同样可行；如果是更复杂的东西，使用
dentry->d_parent。只是要小心，不要假定多次获取它会得到相同的值 —— 在 RCU
模式下它可能在你眼皮底下改变。)

---

**mandatory**
(强制)

->rename() has an added flags argument.  Any flags not handled by the
filesystem should result in EINVAL being returned.
(->rename() 新增了一个 flags 参数。文件系统未处理的任何标志都应导致返回
EINVAL。)

---


**recommended**
(建议)

->readlink is optional for symlinks.  Don't set, unless filesystem needs
to fake something for readlink(2).
(对符号链接来说 ->readlink 是可选的。除非文件系统需要为 readlink(2) 伪造
点什么，否则不要设置。)

---

**mandatory**
(强制)

->getattr() is now passed a struct path rather than a vfsmount and
dentry separately, and it now has request_mask and query_flags arguments
to specify the fields and sync type requested by statx.  Filesystems not
supporting any statx-specific features may ignore the new arguments.
(现在传给 ->getattr() 的是 struct path，而不是分开的 vfsmount 和 dentry，
并且它现在带有 request_mask 和 query_flags 参数，用以指定 statx 请求的字段
和同步类型。不支持任何 statx 特定功能的文件系统可以忽略这些新参数。)

---

**mandatory**
(强制)

->atomic_open() calling conventions have changed.  Gone is ``int *opened``,
along with FILE_OPENED/FILE_CREATED.  In place of those we have
FMODE_OPENED/FMODE_CREATED, set in file->f_mode.  Additionally, return
value for 'called finish_no_open(), open it yourself' case has become
0, not 1.  Since finish_no_open() itself is returning 0 now, that part
does not need any changes in ->atomic_open() instances.
(->atomic_open() 的调用约定已改变。``int *opened`` 没有了，
FILE_OPENED/FILE_CREATED 也随之消失。取而代之的是
FMODE_OPENED/FMODE_CREATED，设置在 file->f_mode 中。此外，"已调用
finish_no_open()，自己打开它"这一情况的返回值变成了 0 而不是 1。由于
finish_no_open() 本身现在返回 0，这部分在 ->atomic_open() 实例中不需要
任何更改。)

---

**mandatory**
(强制)

alloc_file() has become static now; two wrappers are to be used instead.
alloc_file_pseudo(inode, vfsmount, name, flags, ops) is for the cases
when dentry needs to be created; that's the majority of old alloc_file()
users.  Calling conventions: on success a reference to new struct file
is returned and callers reference to inode is subsumed by that.  On
failure, ERR_PTR() is returned and no caller's references are affected,
so the caller needs to drop the inode reference it held.
alloc_file_clone(file, flags, ops) does not affect any caller's references.
On success you get a new struct file sharing the mount/dentry with the
original, on failure - ERR_PTR().
(alloc_file() 现已成为静态函数；应改用两个包装函数。
alloc_file_pseudo(inode, vfsmount, name, flags, ops) 用于需要创建 dentry
的情形；这是旧 alloc_file() 使用者中的大多数。调用约定：成功时返回对新
struct file 的引用，调用者对 inode 的引用被其吸收。失败时返回 ERR_PTR()，
调用者的任何引用都不受影响，因此调用者需要释放它持有的 inode 引用。
alloc_file_clone(file, flags, ops) 不影响调用者的任何引用。成功时你会得到
一个与原文件共享挂载/dentry 的新 struct file，失败时得到 ERR_PTR()。)

---

**mandatory**
(强制)

->clone_file_range() and ->dedupe_file_range have been replaced with
->remap_file_range().  See Documentation/filesystems/vfs.rst for more
information.
(->clone_file_range() 和 ->dedupe_file_range 已被 ->remap_file_range()
取代。更多信息参见 Documentation/filesystems/vfs.rst。)

---

**recommended**
(建议)

->lookup() instances doing an equivalent of::
(做着如下等价操作的 ->lookup() 实例：)

	if (IS_ERR(inode))
		return ERR_CAST(inode);
	return d_splice_alias(inode, dentry);

don't need to bother with the check - d_splice_alias() will do the
right thing when given ERR_PTR(...) as inode.  Moreover, passing NULL
inode to d_splice_alias() will also do the right thing (equivalent of
d_add(dentry, NULL); return NULL;), so that kind of special cases
also doesn't need a separate treatment.
(无需费心做这个检查 —— 当以 ERR_PTR(...) 作为 inode 传入时，
d_splice_alias() 会做正确的事。此外，向 d_splice_alias() 传入 NULL inode
也会做正确的事(等价于 d_add(dentry, NULL); return NULL;)，因此这类特殊
情况也不需要单独处理。)

---

**strongly recommended**
(强烈建议)

take the RCU-delayed parts of ->destroy_inode() into a new method -
->free_inode().  If ->destroy_inode() becomes empty - all the better,
just get rid of it.  Synchronous work (e.g. the stuff that can't
be done from an RCU callback, or any WARN_ON() where we want the
stack trace) *might* be movable to ->evict_inode(); however,
that goes only for the things that are not needed to balance something
done by ->alloc_inode().  IOW, if it's cleaning up the stuff that
might have accumulated over the life of in-core inode, ->evict_inode()
might be a fit.
(把 ->destroy_inode() 中需要 RCU 延迟的部分挪进一个新方法 ——
->free_inode()。如果 ->destroy_inode() 由此变空 —— 那就更好，直接把它
删掉。同步的工作(例如无法从 RCU 回调中完成的东西，或任何我们想要栈回溯的
WARN_ON())*或许*可以移到 ->evict_inode() 中；然而，这只适用于那些不需要
用来平衡 ->alloc_inode() 所做之事的东西。换句话说，如果它清理的是内存中
inode 的一生中可能积累的东西，->evict_inode() 可能是个合适的位置。)

Rules for inode destruction:
(inode 销毁的规则：)

	* if ->destroy_inode() is non-NULL, it gets called
	  (如果 ->destroy_inode() 非 NULL，它会被调用)
	* if ->free_inode() is non-NULL, it gets scheduled by call_rcu()
	  (如果 ->free_inode() 非 NULL，它会由 call_rcu() 调度)
	* combination of NULL ->destroy_inode and NULL ->free_inode is
	  treated as NULL/free_inode_nonrcu, to preserve the compatibility.
	  (NULL ->destroy_inode 与 NULL ->free_inode 的组合被当作
	  NULL/free_inode_nonrcu 处理，以保持兼容性。)

Note that the callback (be it via ->free_inode() or explicit call_rcu()
in ->destroy_inode()) is *NOT* ordered wrt superblock destruction;
as the matter of fact, the superblock and all associated structures
might be already gone.  The filesystem driver is guaranteed to be still
there, but that's it.  Freeing memory in the callback is fine; doing
more than that is possible, but requires a lot of care and is best
avoided.
(注意，回调(无论是通过 ->free_inode() 还是 ->destroy_inode() 中的显式
call_rcu())相对于超级块的销毁*不是*有序的；事实上，超级块和所有相关结构
可能已经消失。唯一保证还在的只有文件系统驱动本身。在回调中释放内存没有
问题；做更多的事是可能的，但需要非常小心，最好避免。)

---

**mandatory**
(强制)

DCACHE_RCUACCESS is gone; having an RCU delay on dentry freeing is the
default.  DCACHE_NORCU opts out, and only d_alloc_pseudo() has any
business doing so.
(DCACHE_RCUACCESS 已被移除；对 dentry 释放进行 RCU 延迟现在是默认行为。
DCACHE_NORCU 用于选择退出，而只有 d_alloc_pseudo() 有理由这么做。)

---

**mandatory**
(强制)

d_alloc_pseudo() is internal-only; uses outside of alloc_file_pseudo() are
very suspect (and won't work in modules).  Such uses are very likely to
be misspelled d_alloc_anon().
(d_alloc_pseudo() 仅限内部使用；在 alloc_file_pseudo() 之外的使用非常可疑
(而且在模块中无法工作)。这类使用极有可能是拼错的 d_alloc_anon()。)

---

**mandatory**
(强制)

[should've been added in 2016] stale comment in finish_open() notwithstanding,
failure exits in ->atomic_open() instances should *NOT* fput() the file,
no matter what.  Everything is handled by the caller.
([该项本应于 2016 年添加] 尽管 finish_open() 中的注释已经过时，
->atomic_open() 实例中的失败出口*绝不要*对文件调用 fput()，无论如何都不
要。一切都由调用者处理。)

---

**mandatory**
(强制)

clone_private_mount() returns a longterm mount now, so the proper destructor of
its result is kern_unmount() or kern_unmount_array().
(clone_private_mount() 现在返回一个长期挂载，因此其结果的正确析构函数是
kern_unmount() 或 kern_unmount_array()。)

---

**mandatory**
(强制)

zero-length bvec segments are disallowed, they must be filtered out before
passed on to an iterator.
(不允许长度为零的 bvec 段，它们必须在传给迭代器之前被过滤掉。)

---

**mandatory**
(强制)

For bvec based itererators bio_iov_iter_get_pages() now doesn't copy bvecs but
uses the one provided. Anyone issuing kiocb-I/O should ensure that the bvec and
page references stay until I/O has completed, i.e. until ->ki_complete() has
been called or returned with non -EIOCBQUEUED code.
(对于基于 bvec 的迭代器，bio_iov_iter_get_pages() 现在不再复制 bvec，而是
使用提供的那个。任何发出 kiocb-I/O 的人都应确保 bvec 和页引用一直保留到
I/O 完成，即直到 ->ki_complete() 被调用或以非 -EIOCBQUEUED 的代码返回。)

---

**mandatory**
(强制)

mnt_want_write_file() can now only be paired with mnt_drop_write_file(),
whereas previously it could be paired with mnt_drop_write() as well.
(mnt_want_write_file() 现在只能与 mnt_drop_write_file() 配对，而以前它也
可以与 mnt_drop_write() 配对。)

---

**mandatory**
(强制)

iov_iter_copy_from_user_atomic() is gone; use copy_page_from_iter_atomic().
The difference is copy_page_from_iter_atomic() advances the iterator and
you don't need iov_iter_advance() after it.  However, if you decide to use
only a part of obtained data, you should do iov_iter_revert().
(iov_iter_copy_from_user_atomic() 已被移除；改用
copy_page_from_iter_atomic()。区别在于 copy_page_from_iter_atomic() 会推进
迭代器，你在它之后不再需要 iov_iter_advance()。然而，如果你决定只使用所获
数据的一部分，你应该调用 iov_iter_revert()。)

---

**mandatory**
(强制)

Calling conventions for file_open_root() changed; now it takes struct path *
instead of passing mount and dentry separately.  For callers that used to
pass <mnt, mnt->mnt_root> pair (i.e. the root of given mount), a new helper
is provided - file_open_root_mnt().  In-tree users adjusted.
(file_open_root() 的调用约定已改变；现在它接受 struct path *，而不是分开
传递挂载和 dentry。对于过去传递 <mnt, mnt->mnt_root> 对(即给定挂载的根)的
调用者，提供了一个新的辅助函数 —— file_open_root_mnt()。内核树内的使用者
已调整。)

---

**mandatory**
(强制)

no_llseek is gone; don't set .llseek to that - just leave it NULL instead.
Checks for "does that file have llseek(2), or should it fail with ESPIPE"
should be done by looking at FMODE_LSEEK in file->f_mode.
(no_llseek 已被移除；不要把 .llseek 设为它 —— 直接保留为 NULL 即可。关于
"该文件是否有 llseek(2)，还是应以 ESPIPE 失败"的检查，应通过查看
file->f_mode 中的 FMODE_LSEEK 来完成。)

---

*mandatory*
(强制)

filldir_t (readdir callbacks) calling conventions have changed.  Instead of
returning 0 or -E... it returns bool now.  false means "no more" (as -E... used
to) and true - "keep going" (as 0 in old calling conventions).  Rationale:
callers never looked at specific -E... values anyway. -> iterate_shared()
instances require no changes at all, all filldir_t ones in the tree
converted.
(filldir_t(readdir 回调)的调用约定已改变。它现在返回 bool 而不是返回 0 或
-E...。false 表示"没有更多了"(如 -E... 过去的含义)，true 表示"继续"(如旧
调用约定中的 0)。理由：反正调用者从不查看具体的 -E... 值。
-> iterate_shared() 实例完全无需更改，树内所有 filldir_t 实例都已转换。)

---

**mandatory**
(强制)

Calling conventions for ->tmpfile() have changed.  It now takes a struct
file pointer instead of struct dentry pointer.  d_tmpfile() is similarly
changed to simplify callers.  The passed file is in a non-open state and on
success must be opened before returning (e.g. by calling
finish_open_simple()).
(->tmpfile() 的调用约定已改变。它现在接受 struct file 指针而不是
struct dentry 指针。d_tmpfile() 也做了类似的修改以简化调用者。传入的文件
处于未打开状态，成功时必须在返回之前将其打开(例如通过调用
finish_open_simple())。)

---

**mandatory**
(强制)

Calling convention for ->huge_fault has changed.  It now takes a page
order instead of an enum page_entry_size, and it may be called without the
mmap_lock held.  All in-tree users have been audited and do not seem to
depend on the mmap_lock being held, but out of tree users should verify
for themselves.  If they do need it, they can return VM_FAULT_RETRY to
be called with the mmap_lock held.
(->huge_fault 的调用约定已改变。它现在接受一个页阶(page order)而不是
enum page_entry_size，并且可能在未持有 mmap_lock 的情况下被调用。所有树内
使用者都经过了审计，似乎并不依赖 mmap_lock 被持有，但树外使用者应自行
验证。如果确实需要，他们可以返回 VM_FAULT_RETRY，以便在持有 mmap_lock 的
情况下被再次调用。)

---

**mandatory**
(强制)

The order of opening block devices and matching or creating superblocks has
changed.
(打开块设备与匹配或创建超级块的顺序已改变。)

The old logic opened block devices first and then tried to find a
suitable superblock to reuse based on the block device pointer.
(旧逻辑先打开块设备，然后尝试基于块设备指针找到一个合适的超级块来重用。)

The new logic tries to find a suitable superblock first based on the device
number, and opening the block device afterwards.
(新逻辑先尝试基于设备号找到合适的超级块，然后再打开块设备。)

Since opening block devices cannot happen under s_umount because of lock
ordering requirements s_umount is now dropped while opening block devices and
reacquired before calling fill_super().
(由于锁顺序要求，打开块设备不能在 s_umount 之下进行，因此现在打开块设备时
会放下 s_umount，并在调用 fill_super() 之前重新获取。)

In the old logic concurrent mounters would find the superblock on the list of
superblocks for the filesystem type. Since the first opener of the block device
would hold s_umount they would wait until the superblock became either born or
was discarded due to initialization failure.
(在旧逻辑中，并发的挂载者会在该文件系统类型的超级块列表上找到超级块。由于
块设备的第一个打开者会持有 s_umount，他们会等待，直到超级块诞生或因初始化
失败而被丢弃。)

Since the new logic drops s_umount concurrent mounters could grab s_umount and
would spin. Instead they are now made to wait using an explicit wait-wake
mechanism without having to hold s_umount.
(由于新逻辑会放下 s_umount，并发的挂载者可能抢到 s_umount 并自旋。现在改为
使用显式的等待-唤醒机制让他们等待，而不必持有 s_umount。)

---

**mandatory**
(强制)

The holder of a block device is now the superblock.
(块设备的持有者现在是超级块。)

The holder of a block device used to be the file_system_type which wasn't
particularly useful. It wasn't possible to go from block device to owning
superblock without matching on the device pointer stored in the superblock.
This mechanism would only work for a single device so the block layer couldn't
find the owning superblock of any additional devices.
(块设备的持有者过去是 file_system_type，这并不是特别有用。不通过匹配存储
在超级块中的设备指针，就无法从块设备找到拥有它的超级块。这种机制只对单个
设备有效，因此块层无法找到任何额外设备的拥有超级块。)

In the old mechanism reusing or creating a superblock for a racing mount(2) and
umount(2) relied on the file_system_type as the holder. This was severely
underdocumented however:
(在旧机制中，为相互竞争的 mount(2) 和 umount(2) 重用或创建超级块依赖于
file_system_type 作为持有者。然而这方面的文档严重不足：)

(1) Any concurrent mounter that managed to grab an active reference on an
    existing superblock was made to wait until the superblock either became
    ready or until the superblock was removed from the list of superblocks of
    the filesystem type. If the superblock is ready the caller would simple
    reuse it.
    (任何成功地在现有超级块上获取活动引用的并发挂载者都会被要求等待，直到
    超级块就绪，或者直到超级块从该文件系统类型的超级块列表中被移除。如果
    超级块已就绪，调用者就简单地重用它。)

(2) If the mounter came after deactivate_locked_super() but before
    the superblock had been removed from the list of superblocks of the
    filesystem type the mounter would wait until the superblock was shutdown,
    reuse the block device and allocate a new superblock.
    (如果挂载者出现在 deactivate_locked_super() 之后、超级块从该文件系统
    类型的超级块列表中被移除之前，挂载者会等待超级块关闭，然后重用块设备并
    分配新的超级块。)

(3) If the mounter came after deactivate_locked_super() and after
    the superblock had been removed from the list of superblocks of the
    filesystem type the mounter would reuse the block device and allocate a new
    superblock (the bd_holder point may still be set to the filesystem type).
    (如果挂载者出现在 deactivate_locked_super() 之后、且超级块已从该文件
    系统类型的超级块列表中被移除之后，挂载者会重用块设备并分配新的超级块
    (bd_holder 指针可能仍被设为该文件系统类型)。)

Because the holder of the block device was the file_system_type any concurrent
mounter could open the block devices of any superblock of the same
file_system_type without risking seeing EBUSY because the block device was
still in use by another superblock.
(由于块设备的持有者是 file_system_type，任何并发挂载者都可以打开同一
file_system_type 的任何超级块的块设备，而不会因块设备仍被另一个超级块使用
而冒着看到 EBUSY 的风险。)

Making the superblock the owner of the block device changes this as the holder
is now a unique superblock and thus block devices associated with it cannot be
reused by concurrent mounters. So a concurrent mounter in (2) could suddenly
see EBUSY when trying to open a block device whose holder was a different
superblock.
(让超级块成为块设备的所有者改变了这一点，因为持有者现在是唯一的超级块，
因而与之关联的块设备不能被并发挂载者重用。因此(2)中的并发挂载者在尝试
打开持有者为另一个超级块的块设备时，可能突然看到 EBUSY。)

The new logic thus waits until the superblock and the devices are shutdown in
->kill_sb(). Removal of the superblock from the list of superblocks of the
filesystem type is now moved to a later point when the devices are closed:
(因此新逻辑会等到超级块和设备在 ->kill_sb() 中关闭。超级块从该文件系统
类型的超级块列表中的移除现在被推迟到稍后设备关闭之时：)

(1) Any concurrent mounter managing to grab an active reference on an existing
    superblock is made to wait until the superblock is either ready or until
    the superblock and all devices are shutdown in ->kill_sb(). If the
    superblock is ready the caller will simply reuse it.
    (任何成功地在现有超级块上获取活动引用的并发挂载者都会被要求等待，直到
    超级块就绪，或者直到超级块和所有设备在 ->kill_sb() 中关闭。如果超级块
    已就绪，调用者就直接重用它。)

(2) If the mounter comes after deactivate_locked_super() but before
    the superblock has been removed from the list of superblocks of the
    filesystem type the mounter is made to wait until the superblock and the
    devices are shut down in ->kill_sb() and the superblock is removed from the
    list of superblocks of the filesystem type. The mounter will allocate a new
    superblock and grab ownership of the block device (the bd_holder pointer of
    the block device will be set to the newly allocated superblock).
    (如果挂载者出现在 deactivate_locked_super() 之后、超级块从该文件系统
    类型的超级块列表中被移除之前，挂载者会被要求等待，直到超级块和设备在
    ->kill_sb() 中关闭并且超级块从该文件系统类型的超级块列表中被移除。
    挂载者将分配新的超级块并获取块设备的所有权(该块设备的 bd_holder 指针
    将被设为新分配的超级块)。)

(3) This case is now collapsed into (2) as the superblock is left on the list
    of superblocks of the filesystem type until all devices are shutdown in
    ->kill_sb(). In other words, if the superblock isn't on the list of
    superblock of the filesystem type anymore then it has given up ownership of
    all associated block devices (the bd_holder pointer is NULL).
    (这种情况现在被合并进(2)，因为超级块会留在该文件系统类型的超级块列表
    上，直到所有设备在 ->kill_sb() 中关闭。换句话说，如果超级块不再位于该
    文件系统类型的超级块列表上，那么它已放弃所有关联块设备的所有权
    (bd_holder 指针为 NULL)。)

As this is a VFS level change it has no practical consequences for filesystems
other than that all of them must use one of the provided kill_litter_super(),
kill_anon_super(), or kill_block_super() helpers.
(由于这是 VFS 层面的更改，它对文件系统没有实际影响，只是所有文件系统都
必须使用所提供的 kill_litter_super()、kill_anon_super() 或
kill_block_super() 辅助函数之一。)

---

**mandatory**
(强制)

Lock ordering has been changed so that s_umount ranks above open_mutex again.
All places where s_umount was taken under open_mutex have been fixed up.
(锁顺序已更改，使 s_umount 再次排在 open_mutex 之上。所有曾在 open_mutex
之下获取 s_umount 的地方都已修正。)

---

**mandatory**
(强制)

export_operations ->encode_fh() no longer has a default implementation to
encode FILEID_INO32_GEN* file handles.
Filesystems that used the default implementation may use the generic helper
generic_encode_ino32_fh() explicitly.
(export_operations 的 ->encode_fh() 不再拥有编码 FILEID_INO32_GEN* 文件
句柄的默认实现。过去使用默认实现的文件系统可以显式使用通用辅助函数
generic_encode_ino32_fh()。)

---

**mandatory**
(强制)

If ->rename() update of .. on cross-directory move needs an exclusion with
directory modifications, do *not* lock the subdirectory in question in your
->rename() - it's done by the caller now [that item should've been added in
28eceeda130f "fs: Lock moved directories"].
(如果 ->rename() 在跨目录移动时对 .. 的更新需要与目录修改互斥，*不要*在你
的 ->rename() 中锁定相关子目录 —— 这现在由调用者完成[该条目本应添加于
28eceeda130f "fs: Lock moved directories"]。)

---

**mandatory**
(强制)

On same-directory ->rename() the (tautological) update of .. is not protected
by any locks; just don't do it if the old parent is the same as the new one.
We really can't lock two subdirectories in same-directory rename - not without
deadlocks.
(在同目录 ->rename() 时，对 .. 的(同义反复的)更新不受任何锁保护；如果旧
父目录与新父目录相同，就别做这个更新。在同目录重命名中我们真的无法锁定两
个子目录 —— 除非死锁。)

---

**mandatory**
(强制)

lock_rename() and lock_rename_child() may fail in cross-directory case, if
their arguments do not have a common ancestor.  In that case ERR_PTR(-EXDEV)
is returned, with no locks taken.  In-tree users updated; out-of-tree ones
would need to do so.
(在跨目录情形下，如果它们的参数没有共同祖先，lock_rename() 和
lock_rename_child() 可能失败。此时返回 ERR_PTR(-EXDEV)，不持有任何锁。
内核树内的使用者已更新；树外的使用者则需要自行更新。)

---

**mandatory**
(强制)

The list of children anchored in parent dentry got turned into hlist now.
Field names got changed (->d_children/->d_sib instead of ->d_subdirs/->d_child
for anchor/entries resp.), so any affected places will be immediately caught
by compiler.
(锚定在父 dentry 中的子项列表现已改为 hlist。字段名已更改(锚点/条目分别用
->d_children/->d_sib 取代 ->d_subdirs/->d_child)，因此任何受影响的地方都会
立即被编译器捕获。)

---

**mandatory**
(强制)

->d_delete() instances are now called for dentries with ->d_lock held
and refcount equal to 0.  They are not permitted to drop/regain ->d_lock.
None of in-tree instances did anything of that sort.  Make sure yours do not...
(->d_delete() 实例现在在持有 ->d_lock 且引用计数为 0 的 dentry 上被调用。
它们不允许释放/重新获取 ->d_lock。树内实例没有做过任何那种事。请确保你的
也不……)

---

**mandatory**
(强制)

->d_prune() instances are now called without ->d_lock held on the parent.
->d_lock on dentry itself is still held; if you need per-parent exclusions (none
of the in-tree instances did), use your own spinlock.
(->d_prune() 实例现在在调用时父目录不持有 ->d_lock。dentry 自身的
->d_lock 仍然被持有；如果你需要针对每个父目录的互斥(树内实例都不需要)，
使用你自己的自旋锁。)

->d_iput() and ->d_release() are called with victim dentry still in the
list of parent's children.  It is still unhashed, marked killed, etc., just not
removed from parent's ->d_children yet.
(->d_iput() 和 ->d_release() 被调用时，受害 dentry 仍在父目录的子项列表
中。它仍然是未入哈希、被标记为已杀死等，只是尚未从父目录的 ->d_children
中移除。)

Anyone iterating through the list of children needs to be aware of the
half-killed dentries that might be seen there; taking ->d_lock on those will
see them negative, unhashed and with negative refcount, which means that most
of the in-kernel users would've done the right thing anyway without any adjustment.
(任何遍历子项列表的人都需要留意可能在那里看到的半死 dentry；对它们获取
->d_lock 将看到它们为负、未入哈希且引用计数为负，这意味着大多数内核内
使用者无论如何无需任何调整就会做正确的事。)

---

**recommended**
(建议)

Block device freezing and thawing have been moved to holder operations.
(块设备的冻结与解冻已被移至持有者操作。)

Before this change, get_active_super() would only be able to find the
superblock of the main block device, i.e., the one stored in sb->s_bdev. Block
device freezing now works for any block device owned by a given superblock, not
just the main block device. The get_active_super() helper and bd_fsfreeze_sb
pointer are gone.
(在此更改之前，get_active_super() 只能找到主块设备的超级块，即存储在
sb->s_bdev 中的那一个。块设备冻结现在对给定超级块拥有的任何块设备都有效，
而不仅仅是主块设备。get_active_super() 辅助函数和 bd_fsfreeze_sb 指针已被
移除。)

---

**mandatory**
(强制)

set_blocksize() takes opened struct file instead of struct block_device now
and it *must* be opened exclusive.
(set_blocksize() 现在接受已打开的 struct file 而不是 struct block_device，
并且它*必须*以独占方式打开。)

---

**mandatory**
(强制)

->d_revalidate() gets two extra arguments - inode of parent directory and
name our dentry is expected to have.  Both are stable (dir is pinned in
non-RCU case and will stay around during the call in RCU case, and name
is guaranteed to stay unchanging).  Your instance doesn't have to use
either, but it often helps to avoid a lot of painful boilerplate.
Note that while name->name is stable and NUL-terminated, it may (and
often will) have name->name[name->len] equal to '/' rather than '\0' -
in normal case it points into the pathname being looked up.
NOTE: if you need something like full path from the root of filesystem,
you are still on your own - this assists with simple cases, but it's not
magic.
(->d_revalidate() 得到两个额外的参数 —— 父目录的 inode 以及我们的
dentry 应有的名字。两者都是稳定的(非 RCU 情形下 dir 被钉住，RCU 情形下
dir 在调用期间会一直存在，且名字保证不变)。你的实例不必使用其中任何一
个，但这通常有助于避免大量痛苦的样板代码。注意，虽然 name->name 是稳定
且以 NUL 结尾的，但 name->name[name->len] 可能(而且经常会)等于 '/' 而不
是 '\0' —— 正常情况下它指向正在查找的路径名之中。注意：如果你需要类似
从文件系统根开始的完整路径这样的东西，你仍然只能靠自己 —— 这只对简单
情况有帮助，但并非魔法。)

---

**recommended**
(建议)

kern_path_locked() and user_path_locked() no longer return a negative
dentry so this doesn't need to be checked.  If the name cannot be found,
ERR_PTR(-ENOENT) is returned.
(kern_path_locked() 和 user_path_locked() 不再返回负 dentry，因此无需检查
这一点。如果找不到该名字，则返回 ERR_PTR(-ENOENT)。)

---

**recommended**
(建议)

lookup_one_qstr_excl() is changed to return errors in more cases, so
these conditions don't require explicit checks:
(lookup_one_qstr_excl() 已改为在更多情况下返回错误，因此这些条件不再需要
显式检查：)

 - if LOOKUP_CREATE is NOT given, then the dentry won't be negative,
   ERR_PTR(-ENOENT) is returned instead
   (如果没有给出 LOOKUP_CREATE，则 dentry 不会是负的，
   而是返回 ERR_PTR(-ENOENT))
 - if LOOKUP_EXCL IS given, then the dentry won't be positive,
   ERR_PTR(-EEXIST) is rreturned instread
   (如果给出了 LOOKUP_EXCL，则 dentry 不会是正的，
   而是返回 ERR_PTR(-EEXIST))

LOOKUP_EXCL now means "target must not exist".  It can be combined with
LOOK_CREATE or LOOKUP_RENAME_TARGET.
(LOOKUP_EXCL 现在意味着"目标必须不存在"。它可以与 LOOK_CREATE 或
LOOKUP_RENAME_TARGET 组合。)

---

**mandatory**
(强制)
invalidate_inodes() is gone use evict_inodes() instead.
(invalidate_inodes() 已被移除，请改用 evict_inodes()。)

---

**mandatory**
(强制)

->mkdir() now returns a dentry.  If the created inode is found to
already be in cache and have a dentry (often IS_ROOT()), it will need to
be spliced into the given name in place of the given dentry. That dentry
now needs to be returned.  If the original dentry is used, NULL should
be returned.  Any error should be returned with ERR_PTR().
(->mkdir() 现在返回一个 dentry。如果发现所创建的 inode 已在缓存中且拥有
一个 dentry(通常是 IS_ROOT())，则需要将它拼接进给定的名字以取代给定的
dentry。现在需要返回那个 dentry。如果使用了原来的 dentry，则应返回 NULL。
任何错误都应通过 ERR_PTR() 返回。)

In general, filesystems which use d_instantiate_new() to install the new
inode can safely return NULL.  Filesystems which may not have an I_NEW inode
should use d_drop();d_splice_alias() and return the result of the latter.
(一般而言，使用 d_instantiate_new() 安装新 inode 的文件系统可以安全地返回
NULL。可能没有 I_NEW inode 的文件系统应使用 d_drop();d_splice_alias() 并
返回后者的结果。)

If a positive dentry cannot be returned for some reason, in-kernel
clients such as cachefiles, nfsd, smb/server may not perform ideally but
will fail-safe.
(如果由于某种原因无法返回正 dentry，诸如 cachefiles、nfsd、smb/server 之类
的内核内客户端可能表现不理想，但会安全失败。)

---

** mandatory**
(强制)

lookup_one(), lookup_one_unlocked(), lookup_one_positive_unlocked() now
take a qstr instead of a name and len.  These, not the "one_len"
versions, should be used whenever accessing a filesystem from outside
that filesysmtem, through a mount point - which will have a mnt_idmap.
(lookup_one()、lookup_one_unlocked()、lookup_one_positive_unlocked() 现在
接受一个 qstr 而不是 name 和 len。每当通过挂载点从该文件系统之外访问该
文件系统时，应使用这些函数而不是"one_len"版本 —— 挂载点会有 mnt_idmap。)

---

** mandatory**
(强制)

Functions try_lookup_one_len(), lookup_one_len(),
lookup_one_len_unlocked() and lookup_positive_unlocked() have been
renamed to try_lookup_noperm(), lookup_noperm(),
lookup_noperm_unlocked(), lookup_noperm_positive_unlocked().  They now
take a qstr instead of separate name and length.  QSTR() can be used
when strlen() is needed for the length.
(函数 try_lookup_one_len()、lookup_one_len()、lookup_one_len_unlocked() 和
lookup_positive_unlocked() 已更名为 try_lookup_noperm()、lookup_noperm()、
lookup_noperm_unlocked()、lookup_noperm_positive_unlocked()。它们现在接受
一个 qstr 而不是分开的 name 和 length。当长度需要 strlen() 时可以使用
QSTR()。)

These function no longer do any permission checking - they previously
checked that the caller has 'X' permission on the parent.  They must
ONLY be used internally by a filesystem on itself when it knows that
permissions are irrelevant or in a context where permission checks have
already been performed such as after vfs_path_parent_lookup()
(这些函数不再进行任何权限检查 —— 它们以前会检查调用者对父目录是否拥有
'X' 权限。它们只能由文件系统在内部对其自身使用，且仅限文件系统知道权限
无关紧要的场合，或已经执行过权限检查的上下文，例如在
vfs_path_parent_lookup() 之后)

---

** mandatory**
(强制)

d_hash_and_lookup() is no longer exported or available outside the VFS.
Use try_lookup_noperm() instead.  This adds name validation and takes
arguments in the opposite order but is otherwise identical.
(d_hash_and_lookup() 不再被导出，也不再在 VFS 之外可用。请改用
try_lookup_noperm()。它会添加名字验证并以相反的顺序接受参数，但除此之外
完全相同。)

Using try_lookup_noperm() will require linux/namei.h to be included.
(使用 try_lookup_noperm() 将需要包含 linux/namei.h。)

---

**mandatory**
(强制)

Calling conventions for ->d_automount() have changed; we should *not* grab
an extra reference to new mount - it should be returned with refcount 1.
(->d_automount() 的调用约定已改变；我们*不应*为新挂载获取额外的引用 ——
它应以引用计数 1 返回。)

---

collect_mounts()/drop_collected_mounts()/iterate_mounts() are gone now.
Replacement is collect_paths()/drop_collected_path(), with no special
iterator needed.  Instead of a cloned mount tree, the new interface returns
an array of struct path, one for each mount collect_mounts() would've
created.  These struct path point to locations in the caller's namespace
that would be roots of the cloned mounts.
(collect_mounts()/drop_collected_mounts()/iterate_mounts() 现已被移除。
替代者是 collect_paths()/drop_collected_path()，无需特殊的迭代器。新接口
不再返回克隆的挂载树，而是返回一个 struct path 数组，对应
collect_mounts() 本会创建的每个挂载各一个。这些 struct path 指向调用者
命名空间中将成为克隆挂载之根的位置。)

---

**mandatory**
(强制)

If your filesystem sets the default dentry_operations, use set_default_d_op()
rather than manually setting sb->s_d_op.
(如果你的文件系统设置默认的 dentry_operations，请使用 set_default_d_op()，
而不是手动设置 sb->s_d_op。)

---

**mandatory**
(强制)

d_set_d_op() is no longer exported (or public, for that matter); _if_
your filesystem really needed that, make use of d_splice_alias_ops()
to have them set.  Better yet, think hard whether you need different
->d_op for different dentries - if not, just use set_default_d_op()
at mount time and be done with that.  Currently procfs is the only
thing that really needs ->d_op varying between dentries.
(d_set_d_op() 不再被导出(就此而言，也不再公开)；_如果_你的文件系统真的
需要它，使用 d_splice_alias_ops() 来设置它们。更好的是，认真想想你是否
需要为不同的 dentry 使用不同的 ->d_op —— 如果不需要，只需在挂载时使用
set_default_d_op() 就完事了。目前 procfs 是唯一真正需要 ->d_op 在不同
dentry 之间变化的东西。)

---

**highly recommended**
(高度建议)

The file operations mmap() callback is deprecated in favour of
mmap_prepare(). This passes a pointer to a vm_area_desc to the callback
rather than a VMA, as the VMA at this stage is not yet valid.
(文件操作的 mmap() 回调已被弃用，取而代之的是 mmap_prepare()。它向回调
传递指向 vm_area_desc 的指针而不是 VMA，因为此阶段的 VMA 尚未有效。)

The vm_area_desc provides the minimum required information for a filesystem
to initialise state upon memory mapping of a file-backed region, and output
parameters for the file system to set this state.
(vm_area_desc 提供了文件系统在内存映射一个文件支撑区域时初始化状态所需的
最低限度信息，以及供文件系统设置该状态的输出参数。)

In nearly all cases, this is all that is required for a filesystem. However, if
a filesystem needs to perform an operation such a pre-population of page tables,
then that action can be specified in the vm_area_desc->action field, which can
be configured using the mmap_action_*() helpers.
(在几乎所有情况下，这对文件系统来说就足够了。然而，如果文件系统需要执行
诸如预填充页表之类的操作，则该动作可以在 vm_area_desc->action 字段中指定，
该字段可使用 mmap_action_*() 辅助函数进行配置。)

---

**mandatory**
(强制)

Several functions are renamed:
(若干函数已更名：)

-  kern_path_locked -> start_removing_path
-  kern_path_create -> start_creating_path
-  user_path_create -> start_creating_user_path
-  user_path_locked_at -> start_removing_user_path_at
-  done_path_create -> end_creating_path

---

**mandatory**
(强制)

Calling conventions for vfs_parse_fs_string() have changed; it does *not*
take length anymore (value ? strlen(value) : 0 is used).  If you want
a different length, use
(vfs_parse_fs_string() 的调用约定已改变；它*不再*接受长度(现在使用
value ? strlen(value) : 0)。如果你想要不同的长度，请使用)

	vfs_parse_fs_qstr(fc, key, &QSTR_LEN(value, len))

instead.
(来代替。)

---

**mandatory**
(强制)

vfs_mkdir() now returns a dentry - the one returned by ->mkdir().  If
that dentry is different from the dentry passed in, including if it is
an IS_ERR() dentry pointer, the original dentry is dput().
(vfs_mkdir() 现在返回一个 dentry —— 即 ->mkdir() 返回的那个。如果该
dentry 与传入的 dentry 不同，包括它是 IS_ERR() dentry 指针的情况，原来的
dentry 会被 dput()。)

When vfs_mkdir() returns an error, and so both dputs() the original
dentry and doesn't provide a replacement, it also unlocks the parent.
Consequently the return value from vfs_mkdir() can be passed to
end_creating() and the parent will be unlocked precisely when necessary.
(当 vfs_mkdir() 返回错误时，由于既对原来的 dentry 做了 dput() 又不提供
替代者，它还会解锁父目录。因此 vfs_mkdir() 的返回值可以传给
end_creating()，父目录将恰好在必要时被解锁。)

---

**mandatory**
(强制)

kill_litter_super() is gone; convert to DCACHE_PERSISTENT use (as all
in-tree filesystems have done).
(kill_litter_super() 已被移除；请改用 DCACHE_PERSISTENT(正如所有树内文件
系统已经做的那样)。)

---

**mandatory**
(强制)

The ->setlease() file_operation must now be explicitly set in order to provide
support for leases. When set to NULL, the kernel will now return -EINVAL to
attempts to set a lease. Filesystems that wish to use the kernel-internal lease
implementation should set it to generic_setlease().
(->setlease() 文件操作现在必须显式设置才能提供租约支持。当设置为 NULL 时，
内核现在会对设置租约的尝试返回 -EINVAL。希望使用内核内部租约实现的文件
系统应将其设置为 generic_setlease()。)

---

**mandatory**
(强制)

fs/namei.c primitives that consume filesystem references (do_renameat2(),
do_linkat(), do_symlinkat(), do_mkdirat(), do_mknodat(), do_unlinkat()
and do_rmdir()) are gone; they are replaced with non-consuming analogues
(filename_renameat2(), etc.)
Callers are adjusted - responsibility for dropping the filenames belongs
to them now.
(fs/namei.c 中消耗文件系统引用的原语(do_renameat2()、do_linkat()、
do_symlinkat()、do_mkdirat()、do_mknodat()、do_unlinkat() 和 do_rmdir())
已被移除；它们由不消耗引用的对应函数(filename_renameat2() 等)取代。
调用者已调整 —— 释放文件名的责任现在归于它们。)

---

**mandatory**
(强制)

readlink_copy() now requires link length as the 4th argument. Said length needs
to match what strlen() would return if it was ran on the string.
(readlink_copy() 现在要求把链接长度作为第 4 个参数。该长度需要与对该字符串
运行 strlen() 所得的结果一致。)

However, if the string is freely accessible for the duration of inode's
lifetime, consider using inode_set_cached_link() instead.
(然而，如果该字符串在 inode 的生命周期内可以随意访问，请考虑改用
inode_set_cached_link()。)

---

**mandatory**
(强制)

lookup_one_qstr_excl() is no longer exported - use start_creating() or
similar.
(lookup_one_qstr_excl() 不再被导出 —— 请使用 start_creating() 或类似的
函数。)

---

** mandatory**
(强制)

lock_rename(), lock_rename_child(), unlock_rename() are no
longer available.  Use start_renaming() or similar.
(lock_rename()、lock_rename_child()、unlock_rename() 不再可用。请使用
start_renaming() 或类似的函数。)

---

**recommended**
(建议)

If you really need to iterate through dentries for given inode, use
for_each_alias(dentry, inode) instead of hlist_for_each_entry; better
yet, see if any of the exported primitives could be used instead of
the entire loop.  You still need to hold ->i_lock of the inode over
either form of manual loop.
(如果你真的需要遍历给定 inode 的 dentry，请使用 for_each_alias(dentry,
inode) 而不是 hlist_for_each_entry；更好的是，看看是否有任何已导出的原语
可以用来取代整个循环。无论哪种形式的手工循环，你仍需要在循环期间持有该
inode 的 ->i_lock。)
