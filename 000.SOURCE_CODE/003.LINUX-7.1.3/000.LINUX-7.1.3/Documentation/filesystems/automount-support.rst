.. SPDX-License-Identifier: GPL-2.0

=================
Automount Support
=================

(自动挂载支持)


Support is available for filesystems that wish to do automounting
support (such as kAFS which can be found in fs/afs/ and NFS in
fs/nfs/). This facility includes allowing in-kernel mounts to be
performed and mountpoint degradation to be requested. The latter can
also be requested by userspace.
(支持希望提供自动挂载(automounting)支持的文件系统(例如可以在 fs/afs/ 中找到的 kAFS 和位于 fs/nfs/ 的 NFS)。该设施包括允许执行内核内挂载以及请求挂载点降级。后者也可以由用户空间请求。)


In-Kernel Automounting
(内核内自动挂载)
======================

See section "Mount Traps" of  Documentation/filesystems/autofs.rst
(参见 Documentation/filesystems/autofs.rst 的 "Mount Traps" 一节)

Then from userspace, you can just do something like::
(然后在用户空间，你可以直接执行类似以下的操作：)

	[root@andromeda root]# mount -t afs \#root.afs. /afs
	[root@andromeda root]# ls /afs
	asd  cambridge  cambridge.redhat.com  grand.central.org
	[root@andromeda root]# ls /afs/cambridge
	afsdoc
	[root@andromeda root]# ls /afs/cambridge/afsdoc/
	ChangeLog  html  LICENSE  pdf  RELNOTES-1.2.2

And then if you look in the mountpoint catalogue, you'll see something like::
(然后如果你查看挂载点目录表，你会看到类似如下的内容：)

	[root@andromeda root]# cat /proc/mounts
	...
	#root.afs. /afs afs rw 0 0
	#root.cell. /afs/cambridge.redhat.com afs rw 0 0
	#afsdoc. /afs/cambridge.redhat.com/afsdoc afs rw 0 0


Automatic Mountpoint Expiry
(挂载点自动过期)
===========================

Automatic expiration of mountpoints is easy, provided you've mounted the
mountpoint to be expired in the automounting procedure outlined separately.
(只要你已经按照另外单独介绍的自动挂载步骤挂载了要过期的挂载点，挂载点的自动过期就很容易实现。)

To do expiration, you need to follow these steps:
(要执行过期，你需要遵循以下步骤：)

 (1) Create at least one list off which the vfsmounts to be expired can be
     hung.
     (创建至少一个列表，将要过期的 vfsmount 挂在其上。)

 (2) When a new mountpoint is created in the ->d_automount method, add
     the mnt to the list using mnt_set_expiry()::
     (当在 ->d_automount 方法中创建新挂载点时，使用 mnt_set_expiry() 将 mnt 添加到该列表：)

             mnt_set_expiry(newmnt, &afs_vfsmounts);

 (3) When you want mountpoints to be expired, call mark_mounts_for_expiry()
     with a pointer to this list. This will process the list, marking every
     vfsmount thereon for potential expiry on the next call.
     (当你想让挂载点过期时，调用 mark_mounts_for_expiry() 并传入指向该列表的指针。这将处理该列表，将其上的每个 vfsmount 标记为在下一次调用时可能过期。)

     If a vfsmount was already flagged for expiry, and if its usage count is 1
     (it's only referenced by its parent vfsmount), then it will be deleted
     from the namespace and thrown away (effectively unmounted).
     (如果一个 vfsmount 已被标记为过期，且其使用计数为 1(它仅被其父 vfsmount 引用)，那么它将从命名空间中删除并被丢弃(实际上被卸载)。)

     It may prove simplest to simply call this at regular intervals, using
     some sort of timed event to drive it.
     (最简单的做法可能是定期调用它，使用某种定时事件来驱动。)

The expiration flag is cleared by calls to mntput. This means that expiration
will only happen on the second expiration request after the last time the
mountpoint was accessed.
(过期标志由对 mntput 的调用清除。这意味着过期只会在挂载点最后一次被访问之后的第二个过期请求时发生。)

If a mountpoint is moved, it gets removed from the expiration list. If a bind
mount is made on an expirable mount, the new vfsmount will not be on the
expiration list and will not expire.
(如果挂载点被移动，它会从过期列表中移除。如果在一个可过期的挂载上进行了绑定挂载(bind mount)，新的 vfsmount 将不在过期列表上，也不会过期。)

If a namespace is copied, all mountpoints contained therein will be copied,
and the copies of those that are on an expiration list will be added to the
same expiration list.
(如果一个命名空间被复制，其中包含的所有挂载点都将被复制，并且在过期列表上的那些挂载点的副本将被添加到同一个过期列表中。)


Userspace Driven Expiry
(用户空间驱动的过期)
=======================

As an alternative, it is possible for userspace to request expiry of any
mountpoint (though some will be rejected - the current process's idea of the
rootfs for example). It does this by passing the MNT_EXPIRE flag to
umount(). This flag is considered incompatible with MNT_FORCE and MNT_DETACH.
(作为替代，用户空间可以请求任何挂载点过期(尽管有些会被拒绝——例如当前进程所认为的 rootfs)。它通过向 umount() 传递 MNT_EXPIRE 标志来实现。此标志被视为与 MNT_FORCE 和 MNT_DETACH 不兼容。)

If the mountpoint in question is in referenced by something other than
umount() or its parent mountpoint, an EBUSY error will be returned and the
mountpoint will not be marked for expiration or unmounted.
(如果相关挂载点被 umount() 或其父挂载点以外的东西引用，将返回 EBUSY 错误，并且该挂载点不会被标记为过期或卸载。)

If the mountpoint was not already marked for expiry at that time, an EAGAIN
error will be given and it won't be unmounted.
(如果挂载点当时尚未被标记为过期，将给出 EAGAIN 错误，并且它不会被卸载。)

Otherwise if it was already marked and it wasn't referenced, unmounting will
take place as usual.
(否则，如果它已被标记且未被引用，卸载将照常进行。)

Again, the expiration flag is cleared every time anything other than umount()
looks at a mountpoint.
(同样，每当有 umount() 以外的任何东西查看挂载点时，过期标志都会被清除。)
