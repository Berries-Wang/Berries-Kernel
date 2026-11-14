.. SPDX-License-Identifier: GPL-2.0

=====================
Multigrain Timestamps
=====================

(多粒度时间戳)

Introduction
(简介)
============
Historically, the kernel has always used coarse time values to stamp inodes.
This value is updated every jiffy, so any change that happens within that jiffy
will end up with the same timestamp.
(从历史上看，内核一直使用粗粒度时间值来标记 inode。该值每个 jiffy 更新一次，因此在同一个 jiffy 内发生的任何更改最终都会有相同的时间戳。)

When the kernel goes to stamp an inode (due to a read or write), it first gets
the current time and then compares it to the existing timestamp(s) to see
whether anything will change. If nothing changed, then it can avoid updating
the inode's metadata.
(当内核准备标记一个 inode 时(由于读或写操作)，它会先获取当前时间，然后将其与现有时间戳进行比较，看是否有任何变化。如果没有变化，那么它可以避免更新 inode 的元数据。)

Coarse timestamps are therefore good from a performance standpoint, since they
reduce the need for metadata updates, but bad from the standpoint of
determining whether anything has changed, since a lot of things can happen in a
jiffy.
(因此，从性能的角度来看，粗粒度时间戳是有利的，因为它们减少了元数据更新的需求；但从判断是否有任何变化的角度来看，它是不利的，因为在一个 jiffy 内可能发生很多事情。)

They are particularly troublesome with NFSv3, where unchanging timestamps can
make it difficult to tell whether to invalidate caches. NFSv4 provides a
dedicated change attribute that should always show a visible change, but not
all filesystems implement this properly, causing the NFS server to substitute
the ctime in many cases.
(它们在 NFSv3 上尤其麻烦，因为不变的时间戳会使人难以判断是否应使缓存失效。NFSv4 提供了一个专门的更改属性，它应当总是显示出可见的变化，但并非所有文件系统都正确地实现了它，这导致 NFS 服务器在许多情况下用 ctime 来代替。)

Multigrain timestamps aim to remedy this by selectively using fine-grained
timestamps when a file has had its timestamps queried recently, and the current
coarse-grained time does not cause a change.
(多粒度时间戳旨在弥补这一点：当某个文件最近被查询过时间戳、且当前粗粒度时间不会造成变化时，选择性地使用细粒度时间戳。)

Inode Timestamps
(inode 时间戳)
================
There are currently 3 timestamps in the inode that are updated to the current
wallclock time on different activity:
(当前 inode 中有 3 个时间戳，会在不同的活动发生时被更新为当前的墙上时钟时间：)

ctime:
  The inode change time. This is stamped with the current time whenever
  the inode's metadata is changed. Note that this value is not settable
  from userland.
  (inode 更改时间。只要 inode 的元数据发生更改，就会用当前时间打上这一标记。注意，该值不能从用户态设置。)

mtime:
  The inode modification time. This is stamped with the current time
  any time a file's contents change.
  (inode 修改时间。只要文件的内容发生更改，就会用当前时间打上这一标记。)

atime:
  The inode access time. This is stamped whenever an inode's contents are
  read. Widely considered to be a terrible mistake. Usually avoided with
  options like noatime or relatime.
  (inode 访问时间。每当 inode 的内容被读取时都会打上这一标记。它被普遍认为是一个可怕的设计错误，通常通过 noatime 或 relatime 之类的选项来避免。)

Updating the mtime always implies a change to the ctime, but updating the
atime due to a read request does not.
(更新 mtime 总是隐含着 ctime 的变化，但因读请求而更新 atime 则不然。)

Multigrain timestamps are only tracked for the ctime and the mtime. atimes are
not affected and always use the coarse-grained value (subject to the floor).
(多粒度时间戳只针对 ctime 和 mtime 进行跟踪。atime 不受影响，始终使用粗粒度值(受下限值约束)。)

Inode Timestamp Ordering
(inode 时间戳排序)
========================

In addition to just providing info about changes to individual files, file
timestamps also serve an important purpose in applications like "make". These
programs measure timestamps in order to determine whether source files might be
newer than cached objects.
(除了提供关于单个文件更改的信息之外，文件时间戳还在 "make" 之类的应用中发挥着重要作用。这些程序测量时间戳，以判断源文件是否可能比缓存的目标文件更新。)

Userland applications like make can only determine ordering based on
operational boundaries. For a syscall those are the syscall entry and exit
points. For io_uring or nfsd operations, that's the request submission and
response. In the case of concurrent operations, userland can make no
determination about the order in which things will occur.
(像 make 这样的用户态应用只能基于操作边界来判断先后顺序。对系统调用而言，这些边界是系统调用的进入和退出点。对 io_uring 或 nfsd 操作而言，则是请求的提交与响应。在并发操作的情况下，用户态无法确定事情发生的先后顺序。)

For instance, if a single thread modifies one file, and then another file in
sequence, the second file must show an equal or later mtime than the first. The
same is true if two threads are issuing similar operations that do not overlap
in time.
(例如，如果单个线程依次修改一个文件，然后再修改另一个文件，那么第二个文件显示的 mtime 必须等于或晚于第一个文件。如果两个线程发出的类似操作在时间上不重叠，情况也是如此。)

If however, two threads have racing syscalls that overlap in time, then there
is no such guarantee, and the second file may appear to have been modified
before, after or at the same time as the first, regardless of which one was
submitted first.
(但是，如果两个线程的系统调用发生竞争且在时间上重叠，那么就没有这种保证了：无论哪一个先提交，第二个文件都可能显示为在第一个文件之前、之后或同时被修改。)

Note that the above assumes that the system doesn't experience a backward jump
of the realtime clock. If that occurs at an inopportune time, then timestamps
can appear to go backward, even on a properly functioning system.
(注意，上面的叙述假设系统不会经历实时时钟的向后跳变。如果这种情况在不合适的时刻发生，那么即使是在正常工作的系统上，时间戳看起来也可能在倒退。)

Multigrain Timestamp Implementation
(多粒度时间戳的实现)
===================================
Multigrain timestamps are aimed at ensuring that changes to a single file are
always recognizable, without violating the ordering guarantees when multiple
different files are modified. This affects the mtime and the ctime, but the
atime will always use coarse-grained timestamps.
(多粒度时间戳的目标是：确保对单个文件的更改总是可识别的，同时在修改多个不同文件时不违反排序保证。这会影响 mtime 和 ctime，而 atime 将始终使用粗粒度时间戳。)

It uses an unused bit in the i_ctime_nsec field to indicate whether the mtime
or ctime has been queried. If either or both have, then the kernel takes
special care to ensure the next timestamp update will display a visible change.
This ensures tight cache coherency for use-cases like NFS, without sacrificing
the benefits of reduced metadata updates when files aren't being watched.
(它使用 i_ctime_nsec 字段中一个未使用的位来指示 mtime 或 ctime 是否已被查询。如果其中之一或两者都被查询过，内核会格外小心，以确保下一次时间戳更新将显示出可见的变化。这为 NFS 之类的用例提供了紧密的缓存一致性，同时在文件未被监视时又不牺牲减少元数据更新的好处。)

The Ctime Floor Value
(ctime 下限值)
=====================
It's not sufficient to simply use fine or coarse-grained timestamps based on
whether the mtime or ctime has been queried. A file could get a fine grained
timestamp, and then a second file modified later could get a coarse-grained one
that appears earlier than the first, which would break the kernel's timestamp
ordering guarantees.
(仅仅根据 mtime 或 ctime 是否被查询来选用细粒度或粗粒度时间戳是不够的。一个文件可能获得一个细粒度时间戳，而之后被修改的第二个文件可能获得一个看起来比第一个更早的粗粒度时间戳，这将破坏内核的时间戳排序保证。)

To mitigate this problem, maintain a global floor value that ensures that
this can't happen. The two files in the above example may appear to have been
modified at the same time in such a case, but they will never show the reverse
order. To avoid problems with realtime clock jumps, the floor is managed as a
monotonic ktime_t, and the values are converted to realtime clock values as
needed.
(为缓解这一问题，需要维护一个全局下限值来确保这种情况不会发生。在这种情况下，上述示例中的两个文件可能看起来是在同一时刻被修改的，但它们绝不会显示相反的顺序。为避免实时时钟跳变带来的问题，该下限以单调的 ktime_t 形式管理，并在需要时转换为实时时钟值。)

Implementation Notes
(实现说明)
====================
Multigrain timestamps are intended for use by local filesystems that get
ctime values from the local clock. This is in contrast to network filesystems
and the like that just mirror timestamp values from a server.
(多粒度时间戳旨在供从本地时钟获取 ctime 值的本地文件系统使用。这与网络文件系统等仅镜像来自服务器的时间戳值的情况形成对比。)

For most filesystems, it's sufficient to just set the FS_MGTIME flag in the
fstype->fs_flags in order to opt-in, providing the ctime is only ever set via
inode_set_ctime_current(). If the filesystem has a ->getattr routine that
doesn't call generic_fillattr, then it should call fill_mg_cmtime() to
fill those values. For setattr, it should use setattr_copy() to update the
timestamps, or otherwise mimic its behavior.
(对大多数文件系统而言，只需在 fstype->fs_flags 中设置 FS_MGTIME 标志即可选择加入，前提是 ctime 只通过 inode_set_ctime_current() 来设置。如果文件系统有一个不调用 generic_fillattr 的 ->getattr 例程，那么它应当调用 fill_mg_cmtime() 来填充这些值。对于 setattr，它应当使用 setattr_copy() 来更新时间戳，或者以其他方式模仿其行为。)
