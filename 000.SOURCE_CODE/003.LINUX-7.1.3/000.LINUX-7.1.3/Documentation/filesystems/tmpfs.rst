.. SPDX-License-Identifier: GPL-2.0

=====
Tmpfs
=====

(Tmpfs)

Tmpfs is a file system which keeps all of its files in virtual memory.
(Tmpfs 是一种把它所有的文件都保存在虚拟内存中的文件系统。)


Everything in tmpfs is temporary in the sense that no files will be
created on your hard drive. If you unmount a tmpfs instance,
everything stored therein is lost.
(tmpfs 中的一切都是临时的——不会有任何文件被创建到你的硬盘上。如果你卸载一个 tmpfs 实例，存储在其中的所有内容都会丢失。)

tmpfs puts everything into the kernel internal caches and grows and
shrinks to accommodate the files it contains and is able to swap
unneeded pages out to swap space, if swap was enabled for the tmpfs
mount. tmpfs also supports THP.
(tmpfs 把所有东西都放入内核内部缓存，并随着它所容纳的文件而伸缩；如果该 tmpfs 挂载启用了 swap，它还能把不需要的页换出到交换空间。tmpfs 也支持 THP。)

tmpfs extends ramfs with a few userspace configurable options listed and
explained further below, some of which can be reconfigured dynamically on the
fly using a remount ('mount -o remount ...') of the filesystem. A tmpfs
filesystem can be resized but it cannot be resized to a size below its current
usage. tmpfs also supports POSIX ACLs, and extended attributes for the
trusted.*, security.* and user.* namespaces. ramfs does not use swap and you
cannot modify any parameter for a ramfs filesystem. The size limit of a ramfs
filesystem is how much memory you have available, and so care must be taken if
used so to not run out of memory.
(tmpfs 在 ramfs 的基础上扩展了一些用户空间可配置的选项，下文会列出并进一步说明，其中一些可以在运行中通过重新挂载文件系统('mount -o remount ...')来动态地重新配置。tmpfs 文件系统可以调整大小，但不能缩小到低于其当前使用量的尺寸。tmpfs 还支持 POSIX ACL，以及 trusted.*、security.* 和 user.* 命名空间的扩展属性。ramfs 不使用 swap，而且你不能修改 ramfs 文件系统的任何参数。ramfs 文件系统的大小限制就是你可用的内存量，因此使用时必须小心，以免耗尽内存。)

An alternative to tmpfs and ramfs is to use brd to create RAM disks
(/dev/ram*), which allows you to simulate a block device disk in physical RAM.
To write data you would just then need to create an regular filesystem on top
this ramdisk. As with ramfs, brd ramdisks cannot swap. brd ramdisks are also
configured in size at initialization and you cannot dynamically resize them.
Contrary to brd ramdisks, tmpfs has its own filesystem, it does not rely on the
block layer at all.
(tmpfs 和 ramfs 的另一种替代方案是使用 brd 来创建 RAM 磁盘(/dev/ram*)，它允许你在物理 RAM 中模拟一个块设备磁盘。要写入数据，你只需在这个 ramdisk 之上创建一个常规文件系统。与 ramfs 一样，brd ramdisk 不能换出(swap)。brd ramdisk 的大小也是在初始化时配置好的，你无法动态地调整它们的大小。与 brd ramdisk 不同，tmpfs 拥有自己的文件系统，它完全不依赖块层。)

Since tmpfs lives completely in the page cache and optionally on swap,
all tmpfs pages will be shown as "Shmem" in /proc/meminfo and "Shared" in
free(1). Notice that these counters also include shared memory
(shmem, see ipcs(1)). The most reliable way to get the count is
using df(1) and du(1).
(由于 tmpfs 完全存在于页缓存之中，并可选地存在于 swap 上，所有 tmpfs 页在 /proc/meminfo 中都会显示为 "Shmem"，在 free(1) 中显示为 "Shared"。注意，这些计数器还包括共享内存(shmem，参见 ipcs(1))。获取这一计数的最可靠方式是使用 df(1) 和 du(1)。)

tmpfs has the following uses:
(tmpfs 有以下用途：)

1) There is always a kernel internal mount which you will not see at
   all. This is used for shared anonymous mappings and SYSV shared
   memory.
   (总有一个你在任何地方都看不到的内核内部挂载。它用于共享匿名映射和 SYSV 共享内存。)

   This mount does not depend on CONFIG_TMPFS. If CONFIG_TMPFS is not
   set, the user visible part of tmpfs is not built. But the internal
   mechanisms are always present.
   (这个挂载不依赖于 CONFIG_TMPFS。如果没有设置 CONFIG_TMPFS，tmpfs 中用户可见的部分就不会被构建。但内部机制始终存在。)

2) glibc 2.2 and above expects tmpfs to be mounted at /dev/shm for
   POSIX shared memory (shm_open, shm_unlink). Adding the following
   line to /etc/fstab should take care of this::
   (glibc 2.2 及以上版本期望 tmpfs 被挂载在 /dev/shm 上，以支持 POSIX 共享内存(shm_open、shm_unlink)。把下面这一行加入 /etc/fstab 即可解决这个问题：)

	tmpfs	/dev/shm	tmpfs	defaults	0 0

   Remember to create the directory that you intend to mount tmpfs on
   if necessary.
   (如有必要，记得先创建你打算用来挂载 tmpfs 的那个目录。)

   This mount is _not_ needed for SYSV shared memory. The internal
   mount is used for that. (In the 2.3 kernel versions it was
   necessary to mount the predecessor of tmpfs (shm fs) to use SYSV
   shared memory.)
   (SYSV 共享内存_不_需要这个挂载，用于此目的的是那个内部挂载。(在 2.3 内核版本中，必须挂载 tmpfs 的前身(shm fs)才能使用 SYSV 共享内存。))

3) Some people (including me) find it very convenient to mount it
   e.g. on /tmp and /var/tmp and have a big swap partition. And now
   loop mounts of tmpfs files do work, so mkinitrd shipped by most
   distributions should succeed with a tmpfs /tmp.
   (有些人(包括我)发现，把它挂载到例如 /tmp 和 /var/tmp 上并配备一个很大的 swap 分区非常方便。而且现在 tmpfs 文件的 loop 挂载也能工作了，所以在 /tmp 为 tmpfs 的情况下，大多数发行版自带的 mkinitrd 应该都能成功。)

4) And probably a lot more I do not know about :-)
   (很可能还有很多我不知道的用途 :-) )


tmpfs has three mount options for sizing:
(tmpfs 有三个用于设定大小的挂载选项：)

=========  ============================================================
size       The limit of allocated bytes for this tmpfs instance. The
           default is half of your physical RAM without swap. If you
           oversize your tmpfs instances the machine will deadlock
           since the OOM handler will not be able to free that memory. (此 tmpfs 实例可分配字节数的上限。默认值是无 swap 时你物理内存的一半。如果你把 tmpfs 实例设置得过大，机器将会死锁，因为 OOM 处理程序将无法释放那部分内存。)
nr_blocks  The same as size, but in blocks of PAGE_SIZE. (与 size 相同，但以 PAGE_SIZE 大小的块为单位。)
nr_inodes  The maximum number of inodes for this instance. The default
           is half of the number of your physical RAM pages, or (on a
           machine with highmem) the number of lowmem RAM pages,
           whichever is the lower. (此实例的 inode 最大数量。默认值是你物理内存页数的一半，或者(在有 highmem 的机器上)lowmem RAM 页数，取两者中较小者。)
=========  ============================================================

These parameters accept a suffix k, m or g for kilo, mega and giga and
can be changed on remount.  The size parameter also accepts a suffix %
to limit this tmpfs instance to that percentage of your physical RAM:
the default, when neither size nor nr_blocks is specified, is size=50%
(这些参数接受后缀 k、m 或 g，分别表示 kilo、mega 和 giga，并且可以在重新挂载时更改。size 参数还接受后缀 %，用于把这个 tmpfs 实例限制为你物理内存的相应百分比：当既未指定 size 也未指定 nr_blocks 时，默认值为 size=50%)

If nr_blocks=0 (or size=0), blocks will not be limited in that instance;
if nr_inodes=0, inodes will not be limited.  It is generally unwise to
mount with such options, since it allows any user with write access to
use up all the memory on the machine; but enhances the scalability of
that instance in a system with many CPUs making intensive use of it.
(如果 nr_blocks=0(或 size=0)，该实例中的块将不受限制；如果 nr_inodes=0，inode 将不受限制。使用这类选项进行挂载通常是不明智的，因为它允许任何拥有写权限的用户用光机器上的全部内存；但它能增强该实例在有许多 CPU 密集使用它的系统中的可伸缩性。)

If nr_inodes is not 0, that limited space for inodes is also used up by
extended attributes: "df -i"'s IUsed and IUse% increase, IFree decreases.
(如果 nr_inodes 不为 0，inode 的那块限额空间也会被扩展属性占用："df -i" 的 IUsed 和 IUse% 会增加，IFree 会减少。)

tmpfs blocks may be swapped out, when there is a shortage of memory.
tmpfs has a mount option to disable its use of swap:
(当内存不足时，tmpfs 块可能被换出。tmpfs 提供了一个挂载选项，用来禁用它对 swap 的使用：)

======  ===========================================================
noswap  Disables swap. Remounts must respect the original settings.
        By default swap is enabled. (禁用 swap。重新挂载必须遵守原有设置。默认启用 swap。)
======  ===========================================================

tmpfs also supports Transparent Huge Pages which requires a kernel
configured with CONFIG_TRANSPARENT_HUGEPAGE and with huge supported for
your system (has_transparent_hugepage(), which is architecture specific).
The mount options for this are:
(tmpfs 还支持透明大页(Transparent Huge Pages)，这要求内核配置了 CONFIG_TRANSPARENT_HUGEPAGE，并且你的系统支持大页(has_transparent_hugepage()，它是与架构相关的)。相关的挂载选项如下：)

================ ==============================================================
huge=never       Do not allocate huge pages.  This is the default. (不分配大页。这是默认值。)
huge=always      Attempt to allocate huge page every time a new page is needed. (每当需要新页时都尝试分配大页。)
huge=within_size Only allocate huge page if it will be fully within i_size.
                 Also respect madvise(2) hints. (只有当大页完全落在 i_size 之内时才分配。同时遵守 madvise(2) 提示。)
huge=advise      Only allocate huge page if requested with madvise(2). (只有当通过 madvise(2) 请求时才分配大页。)
================ ==============================================================

See also Documentation/admin-guide/mm/transhuge.rst, which describes the
sysfs file /sys/kernel/mm/transparent_hugepage/shmem_enabled: which can
be used to deny huge pages on all tmpfs mounts in an emergency, or to
force huge pages on all tmpfs mounts for testing.
(另请参见 Documentation/admin-guide/mm/transhuge.rst，其中描述了 sysfs 文件 /sys/kernel/mm/transparent_hugepage/shmem_enabled：它可用于在紧急情况下拒绝所有 tmpfs 挂载使用大页，或者为了测试而强制所有 tmpfs 挂载使用大页。)

tmpfs also supports quota with the following mount options
(tmpfs 还支持配额，相关的挂载选项如下)

======================== =================================================
quota                    User and group quota accounting and enforcement
                         is enabled on the mount. Tmpfs is using hidden
                         system quota files that are initialized on mount. (在挂载时启用用户和组配额的统计与强制执行。Tmpfs 使用在挂载时初始化的隐藏系统配额文件。)
usrquota                 User quota accounting and enforcement is enabled
                         on the mount. (在挂载时启用用户配额的统计与强制执行。)
grpquota                 Group quota accounting and enforcement is enabled
                         on the mount. (在挂载时启用组配额的统计与强制执行。)
usrquota_block_hardlimit Set global user quota block hard limit. (设置全局用户配额块硬限制。)
usrquota_inode_hardlimit Set global user quota inode hard limit. (设置全局用户配额 inode 硬限制。)
grpquota_block_hardlimit Set global group quota block hard limit. (设置全局组配额块硬限制。)
grpquota_inode_hardlimit Set global group quota inode hard limit. (设置全局组配额 inode 硬限制。)
======================== =================================================

None of the quota related mount options can be set or changed on remount.
(任何与配额相关的挂载选项都不能在重新挂载时设置或更改。)

Quota limit parameters accept a suffix k, m or g for kilo, mega and giga
and can't be changed on remount. Default global quota limits are taking
effect for any and all user/group/project except root the first time the
quota entry for user/group/project id is being accessed - typically the
first time an inode with a particular id ownership is being created after
the mount. In other words, instead of the limits being initialized to zero,
they are initialized with the particular value provided with these mount
options. The limits can be changed for any user/group id at any time as they
normally can be.
(配额限制参数接受后缀 k、m 或 g，分别表示 kilo、mega 和 giga，并且不能在重新挂载时更改。默认的全局配额限制对除 root 之外的任何及所有用户/组/项目生效，生效时机是该用户/组/项目 id 的配额项第一次被访问时——通常是挂载之后第一次创建具有特定 id 属主的 inode 时。换句话说，这些限制不是被初始化为零，而是用这些挂载选项所提供的特定值来初始化。这些限制可以在任何时间对任何用户/组 id 进行更改，就像通常那样。)

Note that tmpfs quotas do not support user namespaces so no uid/gid
translation is done if quotas are enabled inside user namespaces.
(注意，tmpfs 配额不支持用户命名空间，因此在用户命名空间内启用配额时不会进行任何 uid/gid 转换。)

tmpfs has a mount option to set the NUMA memory allocation policy for
all files in that instance (if CONFIG_NUMA is enabled) - which can be
adjusted on the fly via 'mount -o remount ...'
(tmpfs 提供一个挂载选项，用于为该实例中的所有文件设置 NUMA 内存分配策略(如果启用了 CONFIG_NUMA)——它可以通过 'mount -o remount ...' 随时调整)

======================== ==============================================
mpol=default             use the process allocation policy
                         (see set_mempolicy(2)) (使用进程的分配策略(参见 set_mempolicy(2)))
mpol=prefer:Node         prefers to allocate memory from the given Node (优先从给定的 Node 分配内存)
mpol=bind:NodeList       allocates memory only from nodes in NodeList (只从 NodeList 中的节点分配内存)
mpol=interleave          prefers to allocate from each node in turn (优先轮流从各节点分配)
mpol=interleave:NodeList allocates from each node of NodeList in turn (轮流从 NodeList 中的每个节点分配)
mpol=local		 prefers to allocate memory from the local node (优先从本地节点分配内存)
======================== ==============================================

NodeList format is a comma-separated list of decimal numbers and ranges,
a range being two hyphen-separated decimal numbers, the smallest and
largest node numbers in the range.  For example, mpol=bind:0-3,5,7,9-15
(NodeList 格式是一个由逗号分隔的十进制数字和范围组成的列表，一个范围是两个由连字符分隔的十进制数字，即该范围内最小和最大的节点号。例如，mpol=bind:0-3,5,7,9-15)

A memory policy with a valid NodeList will be saved, as specified, for
use at file creation time.  When a task allocates a file in the file
system, the mount option memory policy will be applied with a NodeList,
if any, modified by the calling task's cpuset constraints
[See Documentation/admin-guide/cgroup-v1/cpusets.rst] and any optional flags,
listed below.  If the resulting NodeLists is the empty set, the effective
memory policy for the file will revert to "default" policy.
(带有有效 NodeList 的内存策略将按指定方式保存下来，供文件创建时使用。当某个任务在文件系统中分配文件时，将应用挂载选项的内存策略，其 NodeList(如果有的话)会被调用任务的 cpuset 约束[参见 Documentation/admin-guide/cgroup-v1/cpusets.rst]以及下面列出的任何可选标志所修改。如果得到的 NodeList 是空集，则该文件的有效内存策略将回退为 "default" 策略。)

NUMA memory allocation policies have optional flags that can be used in
conjunction with their modes.  These optional flags can be specified
when tmpfs is mounted by appending them to the mode before the NodeList.
See Documentation/admin-guide/mm/numa_memory_policy.rst for a list of
all available memory allocation policy mode flags and their effect on
memory policy.
(NUMA 内存分配策略有一些可选标志，可以与其模式配合使用。这些可选标志可以在挂载 tmpfs 时指定，方法是把它们附加到 NodeList 之前的模式上。所有可用的内存分配策略模式标志及其对内存策略的影响，请参见 Documentation/admin-guide/mm/numa_memory_policy.rst。)

::

	=static		is equivalent to	MPOL_F_STATIC_NODES
	=relative	is equivalent to	MPOL_F_RELATIVE_NODES

For example, mpol=bind=static:NodeList, is the equivalent of an
allocation policy of MPOL_BIND | MPOL_F_STATIC_NODES.
(例如，mpol=bind=static:NodeList 就等价于 MPOL_BIND | MPOL_F_STATIC_NODES 这一分配策略。)

Note that trying to mount a tmpfs with an mpol option will fail if the
running kernel does not support NUMA; and will fail if its nodelist
specifies a node which is not online.  If your system relies on that
tmpfs being mounted, but from time to time runs a kernel built without
NUMA capability (perhaps a safe recovery kernel), or with fewer nodes
online, then it is advisable to omit the mpol option from automatic
mount options.  It can be added later, when the tmpfs is already mounted
on MountPoint, by 'mount -o remount,mpol=Policy:NodeList MountPoint'.
(注意，如果正在运行的内核不支持 NUMA，尝试用 mpol 选项挂载 tmpfs 将会失败；如果其 nodelist 指定了一个不在线的节点，也会失败。如果你的系统依赖该 tmpfs 被挂载，但不时会运行一个构建时没有 NUMA 能力的内核(也许是一个安全的恢复内核)，或者在线节点较少，那么建议在自动挂载选项中省略 mpol 选项。它可以稍后再添加：当 tmpfs 已经挂载到 MountPoint 上之后，通过 'mount -o remount,mpol=Policy:NodeList MountPoint' 来添加。)


To specify the initial root directory you can use the following mount
options:
(要指定初始根目录，可以使用以下挂载选项：)

====	==================================
mode	The permissions as an octal number (以八进制数表示的权限)
uid	The user id (用户 id)
gid	The group id (组 id)
====	==================================

These options do not have any effect on remount. You can change these
parameters with chmod(1), chown(1) and chgrp(1) on a mounted filesystem.
(这些选项在重新挂载时没有任何效果。你可以在已挂载的文件系统上用 chmod(1)、chown(1) 和 chgrp(1) 来更改这些参数。)


tmpfs has a mount option to select whether it will wrap at 32- or 64-bit inode
numbers:
(tmpfs 提供一个挂载选项，用于选择它在 32 位还是 64 位 inode 号处回绕：)

=======   ========================
inode64   Use 64-bit inode numbers (使用 64 位 inode 号)
inode32   Use 32-bit inode numbers (使用 32 位 inode 号)
=======   ========================

On a 32-bit kernel, inode32 is implicit, and inode64 is refused at mount time.
On a 64-bit kernel, CONFIG_TMPFS_INODE64 sets the default.  inode64 avoids the
possibility of multiple files with the same inode number on a single device;
but risks glibc failing with EOVERFLOW once 33-bit inode numbers are reached -
if a long-lived tmpfs is accessed by 32-bit applications so ancient that
opening a file larger than 2GiB fails with EINVAL.
(在 32 位内核上，inode32 是隐含的，inode64 在挂载时会被拒绝。在 64 位内核上，默认值由 CONFIG_TMPFS_INODE64 设定。inode64 避免了在同一设备上出现多个具有相同 inode 号的文件的可能性；但一旦 inode 号达到 33 位，就有 glibc 以 EOVERFLOW 失败的风险——前提是这个长期存在的 tmpfs 被那些非常古老的 32 位应用程序访问，古老到打开大于 2GiB 的文件会以 EINVAL 失败。)


So 'mount -t tmpfs -o size=10G,nr_inodes=10k,mode=700 tmpfs /mytmpfs'
will give you tmpfs instance on /mytmpfs which can allocate 10GB
RAM/SWAP in 10240 inodes and it is only accessible by root.
(因此，'mount -t tmpfs -o size=10G,nr_inodes=10k,mode=700 tmpfs /mytmpfs' 将在 /mytmpfs 上给你一个 tmpfs 实例，它可以以 10240 个 inode 分配 10GB 的 RAM/SWAP，并且只有 root 可以访问它。)

tmpfs has the following mounting options for case-insensitive lookup support:
(tmpfs 提供以下挂载选项来支持大小写不敏感的查找：)

================= ==============================================================
casefold          Enable casefold support at this mount point using the given
                  argument as the encoding standard. Currently only UTF-8
                  encodings are supported. If no argument is used, it will load
                  the latest UTF-8 encoding available. (在此挂载点启用 casefold 支持，使用给定的参数作为编码标准。目前只支持 UTF-8 编码。如果不带参数，将加载可用的最新 UTF-8 编码。)
strict_encoding   Enable strict encoding at this mount point (disabled by
                  default). In this mode, the filesystem refuses to create file
                  and directory with names containing invalid UTF-8 characters. (在此挂载点启用严格编码(默认禁用)。在这种模式下，文件系统拒绝创建名称中含有无效 UTF-8 字符的文件和目录。)
================= ==============================================================

This option doesn't render the entire filesystem case-insensitive. One needs to
still set the casefold flag per directory, by flipping the +F attribute in an
empty directory. Nevertheless, new directories will inherit the attribute. The
mountpoint itself cannot be made case-insensitive.
(此选项不会使整个文件系统大小写不敏感。仍需要按目录设置 casefold 标志，方法是在一个空目录上翻转 +F 属性。尽管如此，新目录会继承该属性。挂载点本身无法变成大小写不敏感。)

Example::
(示例：)

    $ mount -t tmpfs -o casefold=utf8-12.1.0,strict_encoding fs_name /mytmpfs
    $ mount -t tmpfs -o casefold fs_name /mytmpfs


:Author:
   Christoph Rohland <cr@sap.com>, 1.12.01
   (Christoph Rohland <cr@sap.com>，2001 年 12 月 1 日)
:Updated:
   Hugh Dickins, 4 June 2007
   (Hugh Dickins，2007 年 6 月 4 日)
:Updated:
   KOSAKI Motohiro, 16 Mar 2010
   (KOSAKI Motohiro，2010 年 3 月 16 日)
:Updated:
   Chris Down, 13 July 2020
   (Chris Down，2020 年 7 月 13 日)
:Updated:
   André Almeida, 23 Aug 2024
   (André Almeida，2024 年 8 月 23 日)
