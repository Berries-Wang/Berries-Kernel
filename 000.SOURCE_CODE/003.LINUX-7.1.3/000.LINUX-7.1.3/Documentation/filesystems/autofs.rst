=====================
autofs - how it works
=====================

(autofs - 其工作原理)

Purpose
(目的)
=======

The goal of autofs is to provide on-demand mounting and race free
automatic unmounting of various other filesystems.  This provides two
key advantages:
(autofs 的目标是按需挂载各种其他文件系统，并无竞争地自动卸载它们。这提供了两个关键优势：)

1. There is no need to delay boot until all filesystems that
   might be needed are mounted.  Processes that try to access those
   slow filesystems might be delayed but other processes can
   continue freely.  This is particularly important for
   network filesystems (e.g. NFS) or filesystems stored on
   media with a media-changing robot.
   (无需延迟引导直到所有可能需要的文件系统都被挂载。尝试访问那些慢速文件系统的进程可能会被延迟，但其他进程可以自由继续。这对于网络文件系统(例如 NFS)或存储在带有换盘机器人的介质上的文件系统尤为重要。)

2. The names and locations of filesystems can be stored in
   a remote database and can change at any time.  The content
   in that database at the time of access will be used to provide
   a target for the access.  The interpretation of names in the
   filesystem can even be programmatic rather than database-backed,
   allowing wildcards for example, and can vary based on the user who
   first accessed a name.
   (文件系统的名称和位置可以存储在远程数据库中，并且可以随时更改。访问时该数据库中的内容将被用作访问的目标。对文件系统中名称的解释甚至可以是程序化的而非基于数据库的，例如允许使用通配符，并且可以根据首次访问某个名称的用户而变化。)

Context
(背景)
=======

The "autofs" filesystem module is only one part of an autofs system.
There also needs to be a user-space program which looks up names
and mounts filesystems.  This will often be the "automount" program,
though other tools including "systemd" can make use of "autofs".
This document describes only the kernel module and the interactions
required with any user-space program.  Subsequent text refers to this
as the "automount daemon" or simply "the daemon".
(“autofs” 文件系统模块只是 autofs 系统的一部分。还需要一个用户空间程序来查找名称并挂载文件系统。这通常是 “automount” 程序，不过包括 “systemd” 在内的其他工具也可以使用 “autofs”。本文档仅描述内核模块以及与任何用户空间程序之间所需的交互。后文将其称为 “automount daemon” 或简称 “daemon”。)

"autofs" is a Linux kernel module which provides the "autofs"
filesystem type.  Several "autofs" filesystems can be mounted and they
can each be managed separately, or all managed by the same daemon.
(“autofs” 是一个提供 “autofs” 文件系统类型的 Linux 内核模块。可以挂载多个 “autofs” 文件系统，它们可以各自单独管理，也可以全部由同一个 daemon 管理。)

Content
(内容)
=======

An autofs filesystem can contain 3 sorts of objects: directories,
symbolic links and mount traps.  Mount traps are directories with
extra properties as described in the next section.
(一个 autofs 文件系统可以包含 3 类对象：目录、符号链接和挂载陷阱 (mount traps)。挂载陷阱是具有额外属性的目录，如下一节所述。)

Objects can only be created by the automount daemon: symlinks are
created with a regular `symlink` system call, while directories and
mount traps are created with `mkdir`.  The determination of whether a
directory should be a mount trap is based on a master map. This master
map is consulted by autofs to determine which directories are mount
points. Mount points can be *direct*/*indirect*/*offset*.
On most systems, the default master map is located at */etc/auto.master*.
(对象只能由 automount daemon 创建：符号链接使用常规的 `symlink` 系统调用创建，而目录和挂载陷阱使用 `mkdir` 创建。一个目录是否应为挂载陷阱取决于主映射 (master map)。autofs 会查询此主映射以确定哪些目录是挂载点。挂载点可以是 *直接*/*间接*/*偏移* 的。在大多数系统上，默认的主映射位于 */etc/auto.master*。)

If neither the *direct* or *offset* mount options are given (so the
mount is considered to be *indirect*), then the root directory is
always a regular directory, otherwise it is a mount trap when it is
empty and a regular directory when not empty.  Note that *direct* and
*offset* are treated identically so a concise summary is that the root
directory is a mount trap only if the filesystem is mounted *direct*
and the root is empty.
(如果既没有给出 *direct* 也没有给出 *offset* 挂载选项(因此该挂载被视为 *间接*)，那么根目录始终是常规目录；否则，当根目录为空时它是挂载陷阱，非空时是常规目录。注意 *direct* 和 *offset* 被同等对待，所以简要的总结是：只有当文件系统以 *直接* 方式挂载且根目录为空时，根目录才是挂载陷阱。)

Directories created in the root directory are mount traps only if the
filesystem is mounted *indirect* and they are empty.
(在根目录中创建的目录只有在文件系统以 *间接* 方式挂载且它们为空时才是挂载陷阱。)

Directories further down the tree depend on the *maxproto* mount
option and particularly whether it is less than five or not.
When *maxproto* is five, no directories further down the
tree are ever mount traps, they are always regular directories.  When
the *maxproto* is four (or three), these directories are mount traps
precisely when they are empty.
(树中更深处的目录取决于 *maxproto* 挂载选项，特别是它是否小于五。当 *maxproto* 为五时，树中更深处的目录绝不会是挂载陷阱，它们始终是常规目录。当 *maxproto* 为四(或三)时，这些目录恰好在其为空时是挂载陷阱。)

So: non-empty (i.e. non-leaf) directories are never mount traps. Empty
directories are sometimes mount traps, and sometimes not depending on
where in the tree they are (root, top level, or lower), the *maxproto*,
and whether the mount was *indirect* or not.
(所以：非空(即非叶子)目录绝不是挂载陷阱。空目录有时是挂载陷阱，有时不是，这取决于它们在树中的位置(根、顶层或更低)、*maxproto*，以及该挂载是否为 *间接*。)

Mount Traps
(挂载陷阱)
===========

A core element of the implementation of autofs is the Mount Traps
which are provided by the Linux VFS.  Any directory provided by a
filesystem can be designated as a trap.  This involves two separate
features that work together to allow autofs to do its job.
(autofs 实现的一个核心元素是 Linux VFS 提供的挂载陷阱 (Mount Traps)。文件系统提供的任何目录都可以被指定为陷阱。这涉及两个相互配合的独立特性，它们共同使 autofs 能够完成其工作。)

**DCACHE_NEED_AUTOMOUNT**

If a dentry has the DCACHE_NEED_AUTOMOUNT flag set (which gets set if
the inode has S_AUTOMOUNT set, or can be set directly) then it is
(potentially) a mount trap.  Any access to this directory beyond a
"`stat`" will (normally) cause the `d_op->d_automount()` dentry operation
to be called. The task of this method is to find the filesystem that
should be mounted on the directory and to return it.  The VFS is
responsible for actually mounting the root of this filesystem on the
directory.
(如果一个 dentry 设置了 DCACHE_NEED_AUTOMOUNT 标志(当 inode 设置了 S_AUTOMOUNT 时会设置该标志，也可以直接设置)，那么它(可能)是一个挂载陷阱。对该目录除 “`stat`” 之外的任何访问(通常)都会导致调用 `d_op->d_automount()` dentry 操作。此方法的任务是找到应当挂载到该目录上的文件系统并将其返回。VFS 负责将该文件系统的根实际挂载到该目录上。)

autofs doesn't find the filesystem itself but sends a message to the
automount daemon asking it to find and mount the filesystem.  The
autofs `d_automount` method then waits for the daemon to report that
everything is ready.  It will then return "`NULL`" indicating that the
mount has already happened.  The VFS doesn't try to mount anything but
follows down the mount that is already there.
(autofs 并不自己查找文件系统，而是向 automount daemon 发送消息，请它查找并挂载文件系统。然后 autofs 的 `d_automount` 方法等待 daemon 报告一切就绪。随后它会返回 “`NULL`”，表示挂载已经发生。VFS 不会尝试挂载任何东西，而是沿着已经存在的挂载继续向下。)

This functionality is sufficient for some users of mount traps such
as NFS which creates traps so that mountpoints on the server can be
reflected on the client.  However it is not sufficient for autofs.  As
mounting onto a directory is considered to be "beyond a `stat`", the
automount daemon would not be able to mount a filesystem on the 'trap'
directory without some way to avoid getting caught in the trap.  For
that purpose there is another flag.
(这一功能对挂载陷阱的某些使用者来说已经足够，例如 NFS 创建陷阱是为了让服务器上的挂载点能够反映到客户端上。然而这对 autofs 来说还不够。由于向目录挂载被视为 “超出 `stat` 范围” 的操作，如果没有某种避开陷阱的方法，automount daemon 将无法在 “陷阱” 目录上挂载文件系统。为此目的还有另一个标志。)

**DCACHE_MANAGE_TRANSIT**

If a dentry has DCACHE_MANAGE_TRANSIT set then two very different but
related behaviours are invoked, both using the `d_op->d_manage()`
dentry operation.
(如果一个 dentry 设置了 DCACHE_MANAGE_TRANSIT，则会调用两种非常不同但相关的行为，两者都使用 `d_op->d_manage()` dentry 操作。)

Firstly, before checking to see if any filesystem is mounted on the
directory, d_manage() will be called with the `rcu_walk` parameter set
to `false`.  It may return one of three things:
(首先，在检查是否有文件系统挂载在该目录上之前，会以 `rcu_walk` 参数设置为 `false` 来调用 d_manage()。它可能返回三种结果之一：)

-  A return value of zero indicates that there is nothing special
   about this dentry and normal checks for mounts and automounts
   should proceed.
   (返回值为零表示此 dentry 没有什么特别之处，应继续进行对挂载和自动挂载的常规检查。)

   autofs normally returns zero, but first waits for any
   expiry (automatic unmounting of the mounted filesystem) to
   complete.  This avoids races.
   (autofs 通常返回零，但会先等待任何过期 (expiry，即已挂载文件系统的自动卸载)完成。这可以避免竞争。)

-  A return value of `-EISDIR` tells the VFS to ignore any mounts
   on the directory and to not consider calling `->d_automount()`.
   This effectively disables the **DCACHE_NEED_AUTOMOUNT** flag
   causing the directory not be a mount trap after all.
   (返回值 `-EISDIR` 告诉 VFS 忽略该目录上的任何挂载，并且不考虑调用 `->d_automount()`。这实际上禁用了 **DCACHE_NEED_AUTOMOUNT** 标志，使该目录最终不成为挂载陷阱。)

   autofs returns this if it detects that the process performing the
   lookup is the automount daemon and that the mount has been
   requested but has not yet completed.  How it determines this is
   discussed later.  This allows the automount daemon not to get
   caught in the mount trap.
   (如果 autofs 检测到执行查找的进程是 automount daemon，并且挂载已被请求但尚未完成，它会返回此值。它如何判定这一点将在后面讨论。这使 automount daemon 不会落入挂载陷阱。)

   There is a subtlety here.  It is possible that a second autofs
   filesystem can be mounted below the first and for both of them to
   be managed by the same daemon.  For the daemon to be able to mount
   something on the second it must be able to "walk" down past the
   first.  This means that d_manage cannot *always* return -EISDIR for
   the automount daemon.  It must only return it when a mount has
   been requested, but has not yet completed.
   (这里有一个微妙之处。第二个 autofs 文件系统可能挂载在第一个之下，并且两者由同一个 daemon 管理。为了使 daemon 能够在第二个文件系统上挂载东西，它必须能够 “走过” 第一个。这意味着 d_manage 不能 *总是* 为 automount daemon 返回 -EISDIR。它必须只在挂载已被请求但尚未完成时才返回它。)

   `d_manage` also returns `-EISDIR` if the dentry shouldn't be a
   mount trap, either because it is a symbolic link or because it is
   not empty.
   (如果该 dentry 不应是挂载陷阱(或者因为它是一个符号链接，或者因为它非空)，`d_manage` 也会返回 `-EISDIR`。)

-  Any other negative value is treated as an error and returned
   to the caller.
   (任何其他负值都被视为错误并返回给调用者。)

   autofs can return
   (autofs 可以返回)

   - -ENOENT if the automount daemon failed to mount anything,
     (-ENOENT，如果 automount daemon 未能挂载任何东西，)
   - -ENOMEM if it ran out of memory,
     (-ENOMEM，如果内存耗尽，)
   - -EINTR if a signal arrived while waiting for expiry to
     complete
     (-EINTR，如果在等待过期完成期间有信号到达，)
   - or any other error sent down by the automount daemon.
     (或 automount daemon 发送下来的任何其他错误。)


The second use case only occurs during an "RCU-walk" and so `rcu_walk`
will be set.
(第二种用例只发生在 “RCU-walk” 期间，因此 `rcu_walk` 将被设置。)

An RCU-walk is a fast and lightweight process for walking down a
filename path (i.e. it is like running on tip-toes).  RCU-walk cannot
cope with all situations so when it finds a difficulty it falls back
to "REF-walk", which is slower but more robust.
(RCU-walk 是一种沿文件名路径向下遍历的快速轻量过程(即好比踮着脚尖奔跑)。RCU-walk 无法应付所有情况，因此当它遇到困难时会回退到 “REF-walk”，后者较慢但更稳健。)

RCU-walk will never call `->d_automount`; the filesystems must already
be mounted or RCU-walk cannot handle the path.
To determine if a mount-trap is safe for RCU-walk mode it calls
`->d_manage()` with `rcu_walk` set to `true`.
(RCU-walk 绝不会调用 `->d_automount`；文件系统必须已经挂载，否则 RCU-walk 无法处理该路径。为了确定挂载陷阱对 RCU-walk 模式是否安全，它会以 `rcu_walk` 设置为 `true` 来调用 `->d_manage()`。)

In this case `d_manage()` must avoid blocking and should avoid taking
spinlocks if at all possible.  Its sole purpose is to determine if it
would be safe to follow down into any mounted directory and the only
reason that it might not be is if an expiry of the mount is
underway.
(在这种情况下，`d_manage()` 必须避免阻塞，并且应尽可能避免获取自旋锁。它的唯一目的是判断向下进入任何已挂载的目录是否安全，而可能不安全的唯一原因是该挂载的过期正在进行中。)

In the `rcu_walk` case, `d_manage()` cannot return -EISDIR to tell the
VFS that this is a directory that doesn't require d_automount.  If
`rcu_walk` sees a dentry with DCACHE_NEED_AUTOMOUNT set but nothing
mounted, it *will* fall back to REF-walk.  `d_manage()` cannot make the
VFS remain in RCU-walk mode, but can only tell it to get out of
RCU-walk mode by returning `-ECHILD`.
(在 `rcu_walk` 情况下，`d_manage()` 不能返回 -EISDIR 来告诉 VFS 这是一个不需要 d_automount 的目录。如果 `rcu_walk` 看到一个设置了 DCACHE_NEED_AUTOMOUNT 但没有挂载任何东西的 dentry，它 *将* 回退到 REF-walk。`d_manage()` 不能让 VFS 保持在 RCU-walk 模式，但可以通过返回 `-ECHILD` 告诉它退出 RCU-walk 模式。)

So `d_manage()`, when called with `rcu_walk` set, should either return
-ECHILD if there is any reason to believe it is unsafe to enter the
mounted filesystem, otherwise it should return 0.
(因此，当以设置了 `rcu_walk` 的方式调用 `d_manage()` 时，如果有任何理由认为进入已挂载的文件系统不安全，它应返回 -ECHILD，否则应返回 0。)

autofs will return `-ECHILD` if an expiry of the filesystem has been
initiated or is being considered, otherwise it returns 0.
(如果文件系统的过期已启动或正在被考虑，autofs 将返回 `-ECHILD`，否则返回 0。)


Mountpoint expiry
(挂载点过期)
=================

The VFS has a mechanism for automatically expiring unused mounts,
much as it can expire any unused dentry information from the dcache.
This is guided by the MNT_SHRINKABLE flag.  This only applies to
mounts that were created by `d_automount()` returning a filesystem to be
mounted.  As autofs doesn't return such a filesystem but leaves the
mounting to the automount daemon, it must involve the automount daemon
in unmounting as well.  This also means that autofs has more control
over expiry.
(VFS 提供了一种自动使未使用的挂载过期的机制，就像它可以从 dcache 中使任何未使用的 dentry 信息过期一样。这由 MNT_SHRINKABLE 标志引导。这仅适用于通过 `d_automount()` 返回待挂载文件系统而创建的挂载。由于 autofs 不返回这样的文件系统而是把挂载留给 automount daemon，它必须让 automount daemon 也参与卸载。这也意味着 autofs 对过期有更多控制权。)

The VFS also supports "expiry" of mounts using the MNT_EXPIRE flag to
the `umount` system call.  Unmounting with MNT_EXPIRE will fail unless
a previous attempt had been made, and the filesystem has been inactive
and untouched since that previous attempt.  autofs does not depend on
this but has its own internal tracking of whether filesystems were
recently used.  This allows individual names in the autofs directory
to expire separately.
(VFS 还支持通过对 `umount` 系统调用使用 MNT_EXPIRE 标志来使挂载 “过期”。使用 MNT_EXPIRE 卸载会失败，除非之前已经尝试过一次，并且自那次尝试以来文件系统一直不活跃且未被触及。autofs 并不依赖这一点，而是对文件系统最近是否被使用有自己的内部跟踪。这允许 autofs 目录中的各个名称分别过期。)

With version 4 of the protocol, the automount daemon can try to
unmount any filesystems mounted on the autofs filesystem or remove any
symbolic links or empty directories any time it likes.  If the unmount
or removal is successful the filesystem will be returned to the state
it was before the mount or creation, so that any access of the name
will trigger normal auto-mount processing.  In particular, `rmdir` and
`unlink` do not leave negative entries in the dcache as a normal
filesystem would, so an attempt to access a recently-removed object is
passed to autofs for handling.
(使用协议版本 4 时，automount daemon 可以随时尝试卸载 autofs 文件系统上挂载的任何文件系统，或移除任何符号链接或空目录。如果卸载或移除成功，文件系统将返回到挂载或创建之前的状态，这样对该名称的任何访问都会触发正常的自动挂载处理。特别是，`rmdir` 和 `unlink` 不会像常规文件系统那样在 dcache 中留下负项 (negative entries)，因此对最近被移除对象的访问尝试会被交给 autofs 处理。)

With version 5, this is not safe except for unmounting from top-level
directories.  As lower-level directories are never mount traps, other
processes will see an empty directory as soon as the filesystem is
unmounted.  So it is generally safest to use the autofs expiry
protocol described below.
(使用版本 5 时，除从顶层目录卸载外，这并不安全。由于较低层的目录绝不是挂载陷阱，其他进程在文件系统被卸载后会立即看到一个空目录。所以通常最安全的做法是使用下文描述的 autofs 过期协议。)

Normally the daemon only wants to remove entries which haven't been
used for a while.  For this purpose autofs maintains a "`last_used`"
time stamp on each directory or symlink.  For symlinks it genuinely
does record the last time the symlink was "used" or followed to find
out where it points to.  For directories the field is used slightly
differently.  The field is updated at mount time and during expire
checks if it is found to be in use (ie. open file descriptor or
process working directory) and during path walks. The update done
during path walks prevents frequent expire and immediate mount of
frequently accessed automounts. But in the case where a GUI continually
access or an application frequently scans an autofs directory tree
there can be an accumulation of mounts that aren't actually being
used. To cater for this case the "`strictexpire`" autofs mount option
can be used to avoid the "`last_used`" update on path walk thereby
preventing this apparent inability to expire mounts that aren't
really in use.
(通常 daemon 只想移除一段时间未使用的条目。为此，autofs 在每个目录或符号链接上维护一个 “`last_used`” 时间戳。对于符号链接，它确实记录了该符号链接最后一次被 “使用” 或被跟随以查明其指向位置的时间。对于目录，该字段的用法略有不同。该字段会在挂载时、在过期检查期间发现其正在使用时(即打开的文件描述符或进程工作目录)以及在路径遍历期间被更新。路径遍历期间所做的更新可以防止频繁访问的自动挂载被频繁地过期又立即挂载。但在 GUI 持续访问或应用程序频繁扫描 autofs 目录树的情况下，可能会积累一些实际上并未使用的挂载。为了应对这种情况，可以使用 “`strictexpire`” autofs 挂载选项来避免在路径遍历时更新 “`last_used`”，从而防止这种表面上无法过期实际并未使用的挂载的现象。)

The daemon is able to ask autofs if anything is due to be expired,
using an `ioctl` as discussed later.  For a *direct* mount, autofs
considers if the entire mount-tree can be unmounted or not.  For an
*indirect* mount, autofs considers each of the names in the top level
directory to determine if any of those can be unmounted and cleaned
up.
(daemon 可以使用一个 `ioctl`(稍后讨论)询问 autofs 是否有东西到期需要过期。对于 *direct*(直接)挂载，autofs 会考虑整个挂载树是否可以卸载。对于 *indirect*(间接)挂载，autofs 会逐一考虑顶层目录中的每个名称，以确定其中是否有可以卸载并清理的。)

There is an option with indirect mounts to consider each of the leaves
that has been mounted on instead of considering the top-level names.
This was originally intended for compatibility with version 4 of autofs
and should be considered as deprecated for Sun Format automount maps.
However, it may be used again for amd format mount maps (which are
generally indirect maps) because the amd automounter allows for the
setting of an expire timeout for individual mounts. But there are
some difficulties in making the needed changes for this.
(对于间接挂载，有一个选项可以考虑每个已在其上挂载的叶子，而不是考虑顶层名称。这最初是为了与 autofs 版本 4 兼容而设计的，对于 Sun 格式的自动挂载映射应视为已废弃。不过，对于 amd 格式的挂载映射(通常是间接映射)，它可能会再次被使用，因为 amd 自动挂载器允许为单个挂载设置过期超时。但是，为此进行所需的更改存在一些困难。)

When autofs considers a directory it checks the `last_used` time and
compares it with the "timeout" value set when the filesystem was
mounted, though this check is ignored in some cases. It also checks if
the directory or anything below it is in use.  For symbolic links,
only the `last_used` time is ever considered.
(当 autofs 考虑一个目录时，它会检查 `last_used` 时间并将其与挂载该文件系统时设置的 “timeout” 值比较，尽管在某些情况下会忽略此检查。它还检查该目录或其下是否有东西正在使用。对于符号链接，只会考虑 `last_used` 时间。)

If both appear to support expiring the directory or symlink, an action
is taken.
(如果两者都似乎支持将该目录或符号链接过期，则会采取一个动作。)

There are two ways to ask autofs to consider expiry.  The first is to
use the **AUTOFS_IOC_EXPIRE** ioctl.  This only works for indirect
mounts.  If it finds something in the root directory to expire it will
return the name of that thing.  Once a name has been returned the
automount daemon needs to unmount any filesystems mounted below the
name normally.  As described above, this is unsafe for non-toplevel
mounts in a version-5 autofs.  For this reason the current `automount(8)`
does not use this ioctl.
(有两种方式请求 autofs 考虑过期。第一种是使用 **AUTOFS_IOC_EXPIRE** ioctl。这只适用于间接挂载。如果它在根目录中找到可过期的东西，它会返回该东西的名称。一旦返回了一个名称，automount daemon 需要以常规方式卸载挂载在该名称之下的所有文件系统。如上所述，这对于版本 5 autofs 中的非顶层挂载是不安全的。出于这个原因，当前的 `automount(8)` 不使用此 ioctl。)

The second mechanism uses either the **AUTOFS_DEV_IOCTL_EXPIRE_CMD** or
the **AUTOFS_IOC_EXPIRE_MULTI** ioctl.  This will work for both direct and
indirect mounts.  If it selects an object to expire, it will notify
the daemon using the notification mechanism described below.  This
will block until the daemon acknowledges the expiry notification.
This implies that the "`EXPIRE`" ioctl must be sent from a different
thread than the one which handles notification.
(第二种机制使用 **AUTOFS_DEV_IOCTL_EXPIRE_CMD** 或 **AUTOFS_IOC_EXPIRE_MULTI** ioctl。它同时适用于直接和间接挂载。如果它选中了一个要过期的对象，它将使用下文描述的通知机制通知 daemon。这会阻塞，直到 daemon 确认该过期通知。这意味着 “`EXPIRE`” ioctl 必须由与处理通知的线程不同的另一个线程发送。)

While the ioctl is blocking, the entry is marked as "expiring" and
`d_manage` will block until the daemon affirms that the unmount has
completed (together with removing any directories that might have been
necessary), or has been aborted.
(在该 ioctl 阻塞期间，该条目被标记为 “expiring”，`d_manage` 会阻塞，直到 daemon 确认卸载已完成(连同移除任何可能必需的目录)，或者已被中止。)

Communicating with autofs: detecting the daemon
(与 autofs 通信：检测 daemon)
===============================================

There are several forms of communication between the automount daemon
and the filesystem.  As we have already seen, the daemon can create and
remove directories and symlinks using normal filesystem operations.
autofs knows whether a process requesting some operation is the daemon
or not based on its process-group id number (see getpgid(1)).
(automount daemon 与文件系统之间存在多种通信形式。正如我们已经看到的，daemon 可以使用常规文件系统操作创建和移除目录及符号链接。autofs 根据请求某个操作的进程的进程组 ID 编号(参见 getpgid(1))来判断该进程是否是 daemon。)

When an autofs filesystem is mounted the pgid of the mounting
processes is recorded unless the "pgrp=" option is given, in which
case that number is recorded instead.  Any request arriving from a
process in that process group is considered to come from the daemon.
If the daemon ever has to be stopped and restarted a new pgid can be
provided through an ioctl as will be described below.
(当 autofs 文件系统被挂载时，会记录挂载进程的 pgid，除非给出了 “pgrp=” 选项，此时记录的是该选项给定的编号。来自该进程组中进程的任何请求都被视为来自 daemon。如果 daemon 必须停止并重新启动，可以通过一个 ioctl 提供新的 pgid，如下文所述。)

Communicating with autofs: the event pipe
(与 autofs 通信：事件管道)
=========================================

When an autofs filesystem is mounted, the 'write' end of a pipe must
be passed using the 'fd=' mount option.  autofs will write
notification messages to this pipe for the daemon to respond to.
For version 5, the format of the message is::
(当 autofs 文件系统被挂载时，必须通过 “fd=” 挂载选项传入一个管道的 ‘写’ 端。autofs 会将通知消息写入此管道供 daemon 响应。对于版本 5，消息的格式为：)

	struct autofs_v5_packet {
		struct autofs_packet_hdr hdr;
		autofs_wqt_t wait_queue_token;
		__u32 dev;
		__u64 ino;
		__u32 uid;
		__u32 gid;
		__u32 pid;
		__u32 tgid;
		__u32 len;
		char name[NAME_MAX+1];
        };

And the format of the header is::
(而头部的格式为：)

	struct autofs_packet_hdr {
		int proto_version;		/* Protocol version */
		int type;			/* Type of packet */
	};

where the type is one of ::
(其中类型是以下之一：)

	autofs_ptype_missing_indirect
	autofs_ptype_expire_indirect
	autofs_ptype_missing_direct
	autofs_ptype_expire_direct

so messages can indicate that a name is missing (something tried to
access it but it isn't there) or that it has been selected for expiry.
(因此消息可以指示某个名称缺失(有东西试图访问它但它不存在)，或者它已被选中进行过期。)

The pipe will be set to "packet mode" (equivalent to passing
`O_DIRECT`) to _pipe2(2)_ so that a read from the pipe will return at
most one packet, and any unread portion of a packet will be discarded.
(该管道将被设置为 “数据包模式”(相当于对 _pipe2(2)_ 传入 `O_DIRECT`)，这样从管道的一次读取最多返回一个数据包，而数据包中任何未读的部分都会被丢弃。)

The `wait_queue_token` is a unique number which can identify a
particular request to be acknowledged.  When a message is sent over
the pipe the affected dentry is marked as either "active" or
"expiring" and other accesses to it block until the message is
acknowledged using one of the ioctls below with the relevant
`wait_queue_token`.
(`wait_queue_token` 是一个唯一编号，可以标识一个需要确认的特定请求。当消息通过管道发送时，受影响的 dentry 会被标记为 “active” 或 “expiring”，对它的其他访问会阻塞，直到使用下文相关 ioctl 之一及相应的 `wait_queue_token` 确认该消息。)

Communicating with autofs: root directory ioctls
(与 autofs 通信：根目录 ioctl)
================================================

The root directory of an autofs filesystem will respond to a number of
ioctls.  The process issuing the ioctl must have the CAP_SYS_ADMIN
capability, or must be the automount daemon.
(autofs 文件系统的根目录会响应若干 ioctl。发出 ioctl 的进程必须具有 CAP_SYS_ADMIN 能力，或者必须是 automount daemon。)

The available ioctl commands are:
(可用的 ioctl 命令有：)

- **AUTOFS_IOC_READY**:
	a notification has been handled.  The argument
	to the ioctl command is the "wait_queue_token" number
	corresponding to the notification being acknowledged.
	(一个通知已被处理。ioctl 命令的参数是对应于被确认通知的 "wait_queue_token" 编号。)
- **AUTOFS_IOC_FAIL**:
	similar to above, but indicates failure with
	the error code `ENOENT`.
	(与上面类似，但表示失败，错误码为 `ENOENT`。)
- **AUTOFS_IOC_CATATONIC**:
	Causes the autofs to enter "catatonic"
	mode meaning that it stops sending notifications to the daemon.
	This mode is also entered if a write to the pipe fails.
	(使 autofs 进入 “catatonic” 模式，即停止向 daemon 发送通知。如果对管道的写入失败，也会进入此模式。)
- **AUTOFS_IOC_PROTOVER**:
	This returns the protocol version in use.
	(返回正在使用的协议版本。)
- **AUTOFS_IOC_PROTOSUBVER**:
	Returns the protocol sub-version which
	is really a version number for the implementation.
	(返回协议子版本，它实际上是实现的版本号。)
- **AUTOFS_IOC_SETTIMEOUT**:
	This passes a pointer to an unsigned
	long.  The value is used to set the timeout for expiry, and
	the current timeout value is stored back through the pointer.
	(它传入一个指向 unsigned long 的指针。该值用于设置过期超时，当前的超时值会通过该指针存回。)
- **AUTOFS_IOC_ASKUMOUNT**:
	Returns, in the pointed-to `int`, 1 if
	the filesystem could be unmounted.  This is only a hint as
	the situation could change at any instant.  This call can be
	used to avoid a more expensive full unmount attempt.
	(在被指向的 `int` 中，如果文件系统可以被卸载则返回 1。这只是一个提示，因为情况可能随时改变。此调用可用于避免代价更高的完整卸载尝试。)
- **AUTOFS_IOC_EXPIRE**:
	as described above, this asks if there is
	anything suitable to expire.  A pointer to a packet::
	(如上所述，它询问是否有适合过期的东西。需要一个指向数据包的指针：)

		struct autofs_packet_expire_multi {
			struct autofs_packet_hdr hdr;
			autofs_wqt_t wait_queue_token;
			int len;
			char name[NAME_MAX+1];
		};

	is required.  This is filled in with the name of something
	that can be unmounted or removed.  If nothing can be expired,
	`errno` is set to `EAGAIN`.  Even though a `wait_queue_token`
	is present in the structure, no "wait queue" is established
	and no acknowledgment is needed.
	(该结构会被填入某个可以卸载或移除的东西的名称。如果没有东西可以过期，`errno` 会被设置为 `EAGAIN`。尽管结构中存在 `wait_queue_token`，但不会建立 “等待队列”，也不需要确认。)
- **AUTOFS_IOC_EXPIRE_MULTI**:
	This is similar to
	**AUTOFS_IOC_EXPIRE** except that it causes notification to be
	sent to the daemon, and it blocks until the daemon acknowledges.
	The argument is an integer which can contain two different flags.
	(它与 **AUTOFS_IOC_EXPIRE** 类似，但它会导致向 daemon 发送通知，并阻塞直到 daemon 确认。参数是一个可以包含两个不同标志的整数。)

	**AUTOFS_EXP_IMMEDIATE** causes `last_used` time to be ignored
	and objects are expired if the are not in use.
	(**AUTOFS_EXP_IMMEDIATE** 使 `last_used` 时间被忽略，对象只要不在使用中就会被过期。)

	**AUTOFS_EXP_FORCED** causes the in use status to be ignored
	and objects are expired even if they are in use. This assumes
	that the daemon has requested this because it is capable of
	performing the umount.
	(**AUTOFS_EXP_FORCED** 使使用状态被忽略，即使对象正在使用中也会被过期。这假定 daemon 已请求此操作，因为它有能力执行卸载。)

	**AUTOFS_EXP_LEAVES** will select a leaf rather than a top-level
	name to expire.  This is only safe when *maxproto* is 4.
	(**AUTOFS_EXP_LEAVES** 将选择一个叶子而不是顶层名称进行过期。这仅在 *maxproto* 为 4 时才是安全的。)

Communicating with autofs: char-device ioctls
(与 autofs 通信：字符设备 ioctl)
=============================================

It is not always possible to open the root of an autofs filesystem,
particularly a *direct* mounted filesystem.  If the automount daemon
is restarted there is no way for it to regain control of existing
mounts using any of the above communication channels.  To address this
need there is a "miscellaneous" character device (major 10, minor 235)
which can be used to communicate directly with the autofs filesystem.
It requires CAP_SYS_ADMIN for access.
(并不总是能够打开 autofs 文件系统的根，特别是 *direct*(直接)挂载的文件系统。如果 automount daemon 重新启动，它无法使用上述任何通信通道重新获得对现有挂载的控制。为满足这一需求，有一个 “miscellaneous” 字符设备(主设备号 10，次设备号 235)，可用于直接与 autofs 文件系统通信。访问它需要 CAP_SYS_ADMIN。)

The 'ioctl's that can be used on this device are described in a separate
document `autofs-mount-control.rst`, and are summarised briefly here.
Each ioctl is passed a pointer to an `autofs_dev_ioctl` structure::
(可以在该设备上使用的 “ioctl” 在单独的文档 `autofs-mount-control.rst` 中描述，这里仅作简要总结。每个 ioctl 都会传入一个指向 `autofs_dev_ioctl` 结构的指针：)

        struct autofs_dev_ioctl {
                __u32 ver_major;
                __u32 ver_minor;
                __u32 size;             /* total size of data passed in
                                         * including this struct */
                __s32 ioctlfd;          /* automount command fd */

		/* Command parameters */
		union {
			struct args_protover		protover;
			struct args_protosubver		protosubver;
			struct args_openmount		openmount;
			struct args_ready		ready;
			struct args_fail		fail;
			struct args_setpipefd		setpipefd;
			struct args_timeout		timeout;
			struct args_requester		requester;
			struct args_expire		expire;
			struct args_askumount		askumount;
			struct args_ismountpoint	ismountpoint;
		};

                char path[];
        };

For the **OPEN_MOUNT** and **IS_MOUNTPOINT** commands, the target
filesystem is identified by the `path`.  All other commands identify
the filesystem by the `ioctlfd` which is a file descriptor open on the
root, and which can be returned by **OPEN_MOUNT**.
(对于 **OPEN_MOUNT** 和 **IS_MOUNTPOINT** 命令，目标文件系统由 `path` 标识。所有其他命令通过 `ioctlfd` 标识文件系统，`ioctlfd` 是一个打开在根目录上的文件描述符，它可以由 **OPEN_MOUNT** 返回。)

The `ver_major` and `ver_minor` are in/out parameters which check that
the requested version is supported, and report the maximum version
that the kernel module can support.
(`ver_major` 和 `ver_minor` 是入/出参数，用于检查所请求的版本是否受支持，并报告内核模块所能支持的最大版本。)

Commands are:
(命令有：)

- **AUTOFS_DEV_IOCTL_VERSION_CMD**:
	does nothing, except validate and
	set version numbers.
	(除了验证并设置版本号之外不做任何事情。)
- **AUTOFS_DEV_IOCTL_OPENMOUNT_CMD**:
	return an open file descriptor
	on the root of an autofs filesystem.  The filesystem is identified
	by name and device number, which is stored in `openmount.devid`.
	Device numbers for existing filesystems can be found in
	`/proc/self/mountinfo`.
	(返回 autofs 文件系统根目录上一个打开的文件描述符。文件系统由名称和设备号标识，设备号存储在 `openmount.devid` 中。现有文件系统的设备号可以在 `/proc/self/mountinfo` 中找到。)
- **AUTOFS_DEV_IOCTL_CLOSEMOUNT_CMD**:
	same as `close(ioctlfd)`.
	(等同于 `close(ioctlfd)`。)
- **AUTOFS_DEV_IOCTL_SETPIPEFD_CMD**:
	if the filesystem is in
	catatonic mode, this can provide the write end of a new pipe
	in `setpipefd.pipefd` to re-establish communication with a daemon.
	The process group of the calling process is used to identify the
	daemon.
	(如果文件系统处于 catatonic 模式，此项可以在 `setpipefd.pipefd` 中提供新管道的写端，以重新建立与 daemon 的通信。调用进程的进程组用于标识 daemon。)
- **AUTOFS_DEV_IOCTL_REQUESTER_CMD**:
	`path` should be a
	name within the filesystem that has been auto-mounted on.
	On successful return, `requester.uid` and `requester.gid` will be
	the UID and GID of the process which triggered that mount.
	(`path` 应是文件系统内一个已被自动挂载上的名称。成功返回时，`requester.uid` 和 `requester.gid` 将是触发该挂载的进程的 UID 和 GID。)
- **AUTOFS_DEV_IOCTL_ISMOUNTPOINT_CMD**:
	Check if path is a
	mountpoint of a particular type - see separate documentation for
	details.
	(检查 path 是否是特定类型的挂载点——详情参见单独的文档。)

- **AUTOFS_DEV_IOCTL_PROTOVER_CMD**
- **AUTOFS_DEV_IOCTL_PROTOSUBVER_CMD**
- **AUTOFS_DEV_IOCTL_READY_CMD**
- **AUTOFS_DEV_IOCTL_FAIL_CMD**
- **AUTOFS_DEV_IOCTL_CATATONIC_CMD**
- **AUTOFS_DEV_IOCTL_TIMEOUT_CMD**
- **AUTOFS_DEV_IOCTL_EXPIRE_CMD**
- **AUTOFS_DEV_IOCTL_ASKUMOUNT_CMD**

These all have the same
function as the similarly named **AUTOFS_IOC** ioctls, except
that **FAIL** can be given an explicit error number in `fail.status`
instead of assuming `ENOENT`, and this **EXPIRE** command
corresponds to **AUTOFS_IOC_EXPIRE_MULTI**.
(这些命令与类似命名的 **AUTOFS_IOC** ioctl 具有相同的功能，除了 **FAIL** 可以在 `fail.status` 中给定一个显式的错误号而不是假定 `ENOENT`，并且此 **EXPIRE** 命令对应于 **AUTOFS_IOC_EXPIRE_MULTI**。)

Catatonic mode
(Catatonic 模式)
==============

As mentioned, an autofs mount can enter "catatonic" mode.  This
happens if a write to the notification pipe fails, or if it is
explicitly requested by an `ioctl`.
(如前所述，autofs 挂载可以进入 “catatonic” 模式。如果对通知管道的写入失败，或者通过 `ioctl` 显式请求，就会发生这种情况。)

When entering catatonic mode, the pipe is closed and any pending
notifications are acknowledged with the error `ENOENT`.
(进入 catatonic 模式时，管道会被关闭，任何待处理的通知都会以错误 `ENOENT` 确认。)

Once in catatonic mode attempts to access non-existing names will
result in `ENOENT` while attempts to access existing directories will
be treated in the same way as if they came from the daemon, so mount
traps will not fire.
(进入 catatonic 模式后，访问不存在的名称将得到 `ENOENT`，而访问已存在目录的尝试将被当作来自 daemon 一样处理，因此挂载陷阱不会触发。)

When the filesystem is mounted a _uid_ and _gid_ can be given which
set the ownership of directories and symbolic links.  When the
filesystem is in catatonic mode, any process with a matching UID can
create directories or symlinks in the root directory, but not in other
directories.
(挂载文件系统时可以给定 _uid_ 和 _gid_，用于设置目录和符号链接的所有权。当文件系统处于 catatonic 模式时，任何具有匹配 UID 的进程都可以在根目录中创建目录或符号链接，但在其他目录中则不行。)

Catatonic mode can only be left via the
**AUTOFS_DEV_IOCTL_OPENMOUNT_CMD** ioctl on the `/dev/autofs`.
(只能通过对 `/dev/autofs` 使用 **AUTOFS_DEV_IOCTL_OPENMOUNT_CMD** ioctl 才能离开 catatonic 模式。)

The "ignore" mount option
(“ignore” 挂载选项)
=========================

The "ignore" mount option can be used to provide a generic indicator
to applications that the mount entry should be ignored when displaying
mount information.
(“ignore” 挂载选项可用于向应用程序提供一个通用指示，表明在显示挂载信息时应忽略该挂载条目。)

In other OSes that provide autofs and that provide a mount list to user
space based on the kernel mount list a no-op mount option ("ignore" is
the one use on the most common OSes) is allowed so that autofs file
system users can optionally use it.
(在其他提供 autofs 并且基于内核挂载列表向用户空间提供挂载列表的操作系统中，允许使用一个无操作 (no-op) 挂载选项("ignore" 是最常见操作系统上使用的那个)，以便 autofs 文件系统用户可以选择使用它。)

This is intended to be used by user space programs to exclude autofs
mounts from consideration when reading the mounts list.
(这旨在供用户空间程序在读取挂载列表时将 autofs 挂载排除在考虑之外。)

autofs, name spaces, and shared mounts
(autofs、命名空间与共享挂载)
======================================

With bind mounts and name spaces it is possible for an autofs
filesystem to appear at multiple places in one or more filesystem
name spaces.  For this to work sensibly, the autofs filesystem should
always be mounted "shared". e.g. ::
(有了绑定挂载和命名空间，一个 autofs 文件系统可能出现在一个或多个文件系统命名空间中的多个位置。为使其合理工作，autofs 文件系统应始终以 “shared” 方式挂载。例如：)

	mount --make-shared /autofs/mount/point

The automount daemon is only able to manage a single mount location for
an autofs filesystem and if mounts on that are not 'shared', other
locations will not behave as expected.  In particular access to those
other locations will likely result in the `ELOOP` error ::
(automount daemon 只能管理 autofs 文件系统的一个挂载位置，如果其上的挂载不是 ‘shared’ 的，其他位置将不会按预期表现。特别是，对这些其他位置的访问很可能导致 `ELOOP` 错误：)

	Too many levels of symbolic links
