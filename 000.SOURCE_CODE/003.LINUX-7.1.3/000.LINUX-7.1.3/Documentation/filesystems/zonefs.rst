.. SPDX-License-Identifier: GPL-2.0

================================================
ZoneFS - Zone filesystem for Zoned block devices
================================================

(ZoneFS - 面向分区块设备(zoned block device)的 Zone 文件系统)

Introduction
(简介)
============

zonefs is a very simple file system exposing each zone of a zoned block device
as a file. Unlike a regular POSIX-compliant file system with native zoned block
device support (e.g. f2fs), zonefs does not hide the sequential write
constraint of zoned block devices to the user. Files representing sequential
write zones of the device must be written sequentially starting from the end
of the file (append only writes).
(zonefs 是一个非常简单的文件系统，它把分区块设备的每个区域(zone)暴露为一个文件。与原生支持分区块设备的常规 POSIX 兼容文件系统(例如 f2fs)不同，zonefs 不向用户隐藏分区块设备的顺序写约束。表示设备顺序写区域的文件必须从文件末尾开始顺序写入(只允许追加写入)。)

As such, zonefs is in essence closer to a raw block device access interface
than to a full-featured POSIX file system. The goal of zonefs is to simplify
the implementation of zoned block device support in applications by replacing
raw block device file accesses with a richer file API, avoiding relying on
direct block device file ioctls which may be more obscure to developers. One
example of this approach is the implementation of LSM (log-structured merge)
tree structures (such as used in RocksDB and LevelDB) on zoned block devices
by allowing SSTables to be stored in a zone file similarly to a regular file
system rather than as a range of sectors of the entire disk. The introduction
of the higher level construct "one file is one zone" can help reducing the
amount of changes needed in the application as well as introducing support for
different application programming languages.
(因此，zonefs 本质上更接近原始块设备访问接口，而不是功能完备的 POSIX 文件系统。zonefs 的目标是：通过用更丰富的文件 API 取代原始块设备文件访问，简化应用程序中对分区块设备支持的实现，避免依赖对开发者来说可能较为晦涩的直接块设备文件 ioctl。这一方式的一个例子是在分区块设备上实现 LSM(日志结构合并)树结构(如 RocksDB 和 LevelDB 中所用的那样)：允许把 SSTable 像在常规文件系统中一样存储在一个区域文件里，而不是作为整个磁盘的一段扇区范围。引入"一个文件即一个区域"这一更高层的抽象，有助于减少应用程序所需的改动量，也有助于引入对不同应用编程语言的支持。)

Zoned block devices
(分区块设备)
-------------------

Zoned storage devices belong to a class of storage devices with an address
space that is divided into zones. A zone is a group of consecutive LBAs and all
zones are contiguous (there are no LBA gaps). Zones may have different types.
(分区存储设备属于这样一类存储设备：其地址空间被划分为多个区域。区域是一组连续的 LBA，并且所有区域彼此连续(没有 LBA 间隙)。区域可以有不同的类型。)

* Conventional zones: there are no access constraints to LBAs belonging to
  conventional zones. Any read or write access can be executed, similarly to a
  regular block device.
  (常规区域：对属于常规区域的 LBA 没有访问约束。可以像常规块设备那样执行任何读或写访问。)
* Sequential zones: these zones accept random reads but must be written
  sequentially. Each sequential zone has a write pointer maintained by the
  device that keeps track of the mandatory start LBA position of the next write
  to the device. As a result of this write constraint, LBAs in a sequential zone
  cannot be overwritten. Sequential zones must first be erased using a special
  command (zone reset) before rewriting.
  (顺序区域：这些区域接受随机读，但必须顺序写入。每个顺序区域都有一个由设备维护的写指针，用于跟踪对设备下一次写入的强制起始 LBA 位置。由于这一写约束，顺序区域中的 LBA 不能被覆盖。顺序区域在重写之前必须先用特殊命令(区域重置)擦除。)

Zoned storage devices can be implemented using various recording and media
technologies. The most common form of zoned storage today uses the SCSI Zoned
Block Commands (ZBC) and Zoned ATA Commands (ZAC) interfaces on Shingled
Magnetic Recording (SMR) HDDs.
(分区存储设备可以用各种记录和介质技术实现。当今最常见的分区存储形式是在叠瓦式磁记录(SMR)硬盘上使用 SCSI Zoned Block Commands(ZBC)和 Zoned ATA Commands(ZAC)接口。)

Solid State Disks (SSD) storage devices can also implement a zoned interface
to, for instance, reduce internal write amplification due to garbage collection.
The NVMe Zoned NameSpace (ZNS) is a technical proposal of the NVMe standard
committee aiming at adding a zoned storage interface to the NVMe protocol.
(固态硬盘(SSD)存储设备也可以实现分区接口，例如为了减少垃圾收集导致的内部写放大。NVMe Zoned NameSpace(ZNS)是 NVMe 标准委员会的一项技术提案，旨在为 NVMe 协议增加分区存储接口。)

Zonefs Overview
(Zonefs 概述)
===============

Zonefs exposes the zones of a zoned block device as files. The files
representing zones are grouped by zone type, which are themselves represented
by sub-directories. This file structure is built entirely using zone information
provided by the device and so does not require any complex on-disk metadata
structure.
(Zonefs 把分区块设备的区域暴露为文件。表示区域的文件按区域类型分组，区域类型本身又由子目录表示。这一文件结构完全使用设备提供的区域信息构建，因此不需要任何复杂的磁盘元数据结构。)

On-disk metadata
(磁盘元数据)
----------------

zonefs on-disk metadata is reduced to an immutable super block which
persistently stores a magic number and optional feature flags and values. On
mount, zonefs uses blkdev_report_zones() to obtain the device zone configuration
and populates the mount point with a static file tree solely based on this
information. File sizes come from the device zone type and write pointer
position managed by the device itself.
(zonefs 的磁盘元数据精简为一个不可变的超级块，它持久地存储一个魔数以及可选的特性标志和值。挂载时，zonefs 使用 blkdev_report_zones() 获取设备的区域配置，并仅依据这一信息在挂载点上填充一棵静态文件树。文件大小来自设备的区域类型和由设备自身管理的写指针位置。)

The super block is always written on disk at sector 0. The first zone of the
device storing the super block is never exposed as a zone file by zonefs. If
the zone containing the super block is a sequential zone, the mkzonefs format
tool always "finishes" the zone, that is, it transitions the zone to a full
state to make it read-only, preventing any data write.
(超级块总是写在磁盘的 0 号扇区。存储超级块的设备第一个区域永远不会被 zonefs 暴露为区域文件。如果包含超级块的区域是顺序区域，mkzonefs 格式化工具总是会"完成"(finish)该区域，也就是把区域转换为满状态使其只读，阻止任何数据写入。)

Zone type sub-directories
(区域类型子目录)
-------------------------

Files representing zones of the same type are grouped together under the same
sub-directory automatically created on mount.
(表示同一类型区域的文件被分组放在挂载时自动创建的同一子目录下。)

For conventional zones, the sub-directory "cnv" is used. This directory is
however created if and only if the device has usable conventional zones. If
the device only has a single conventional zone at sector 0, the zone will not
be exposed as a file as it will be used to store the zonefs super block. For
such devices, the "cnv" sub-directory will not be created.
(对于常规区域，使用子目录 "cnv"。不过，当且仅当设备拥有可用的常规区域时才会创建该目录。如果设备在 0 号扇区只有一个常规区域，该区域将不会暴露为文件，因为它将被用于存储 zonefs 超级块。对于此类设备，不会创建 "cnv" 子目录。)

For sequential write zones, the sub-directory "seq" is used.
(对于顺序写区域，使用子目录 "seq"。)

These two directories are the only directories that exist in zonefs. Users
cannot create other directories and cannot rename nor delete the "cnv" and
"seq" sub-directories.
(这两个目录是 zonefs 中仅有的目录。用户不能创建其他目录，也不能重命名或删除 "cnv" 和 "seq" 子目录。)

The size of the directories indicated by the st_size field of struct stat,
obtained with the stat() or fstat() system calls, indicates the number of files
existing under the directory.
(通过 stat() 或 fstat() 系统调用获得的 struct stat 的 st_size 字段所指示的目录大小，表示该目录下存在的文件数量。)

Zone files
(区域文件)
----------

Zone files are named using the number of the zone they represent within the set
of zones of a particular type. That is, both the "cnv" and "seq" directories
contain files named "0", "1", "2", ... The file numbers also represent
increasing zone start sector on the device.
(区域文件以它们所表示的区域在某一类型区域集合中的编号来命名。也就是说，"cnv" 和 "seq" 目录都包含名为 "0"、"1"、"2"…… 的文件。文件编号也表示设备上递增的区域起始扇区。)

All read and write operations to zone files are not allowed beyond the file
maximum size, that is, beyond the zone capacity. Any access exceeding the zone
capacity is failed with the -EFBIG error.
(对区域文件的所有读写操作都不允许超过文件最大大小，即不允许超过区域容量。任何超出区域容量的访问都会以 -EFBIG 错误失败。)

Creating, deleting, renaming or modifying any attribute of files and
sub-directories is not allowed.
(不允许创建、删除、重命名文件和子目录，也不允许修改其任何属性。)

The number of blocks of a file as reported by stat() and fstat() indicates the
capacity of the zone file, or in other words, the maximum file size.
(stat() 和 fstat() 报告的文件块数表示区域文件的容量，或者换句话说，即最大文件大小。)

Conventional zone files
(常规区域文件)
-----------------------

The size of conventional zone files is fixed to the size of the zone they
represent. Conventional zone files cannot be truncated.
(常规区域文件的大小固定为它们所表示的区域的大小。常规区域文件不能截断。)

These files can be randomly read and written using any type of I/O operation:
buffered I/Os, direct I/Os, memory mapped I/Os (mmap), etc. There are no I/O
constraint for these files beyond the file size limit mentioned above.
(这些文件可以使用任何类型的 I/O 操作进行随机读写：缓冲 I/O、直接 I/O、内存映射 I/O(mmap)等。除上述文件大小限制外，这些文件没有任何 I/O 约束。)

Sequential zone files
(顺序写区域文件)
---------------------

The size of sequential zone files grouped in the "seq" sub-directory represents
the file's zone write pointer position relative to the zone start sector.
("seq" 子目录中分组的顺序区域文件的大小，表示文件的区域写指针相对于区域起始扇区的位置。)

Sequential zone files can only be written sequentially, starting from the file
end, that is, write operations can only be append writes. Zonefs makes no
attempt at accepting random writes and will fail any write request that has a
start offset not corresponding to the end of the file, or to the end of the last
write issued and still in-flight (for asynchronous I/O operations).
(顺序区域文件只能顺序写入，从文件末尾开始，也就是说写操作只能是追加写。Zonefs 不尝试接受随机写入，任何起始偏移不对应文件末尾、或不对应最后一次已发出且仍在途中的写入之末尾(对于异步 I/O 操作)的写请求都会失败。)

Since dirty page writeback by the page cache does not guarantee a sequential
write pattern, zonefs prevents buffered writes and writeable shared mappings
on sequential files. Only direct I/O writes are accepted for these files.
zonefs relies on the sequential delivery of write I/O requests to the device
implemented by the block layer elevator. An elevator implementing the sequential
write feature for zoned block device (ELEVATOR_F_ZBD_SEQ_WRITE elevator feature)
must be used. This type of elevator (e.g. mq-deadline) is set by default
for zoned block devices on device initialization.
(由于页缓存的脏页回写不保证顺序写模式，zonefs 禁止对顺序文件进行缓冲写和可写共享映射。这些文件只接受直接 I/O 写入。zonefs 依赖于由块层 IO 调度器(elevator)实现的向设备顺序提交写 I/O 请求的机制。必须使用为分区块设备实现顺序写特性的 IO 调度器(ELEVATOR_F_ZBD_SEQ_WRITE 调度器特性)。这类 IO 调度器(例如 mq-deadline)在设备初始化时被默认设置为分区块设备的调度器。)

There are no restrictions on the type of I/O used for read operations in
sequential zone files. Buffered I/Os, direct I/Os and shared read mappings are
all accepted.
(对顺序区域文件的读操作所用的 I/O 类型没有任何限制。缓冲 I/O、直接 I/O 和共享读映射都被接受。)

Truncating sequential zone files is allowed only down to 0, in which case, the
zone is reset to rewind the file zone write pointer position to the start of
the zone, or up to the zone capacity, in which case the file's zone is
transitioned to the FULL state (finish zone operation).
(顺序区域文件只允许截断到 0(此时区域被重置，把文件区域的写指针位置回卷到区域起点)，或截断到区域容量(此时文件区域转换为 FULL 状态，即 finish zone 操作)。)

Format options
(格式化选项)
--------------

Several optional features of zonefs can be enabled at format time.
(zonefs 的若干可选特性可以在格式化时启用。)

* Conventional zone aggregation: ranges of contiguous conventional zones can be
  aggregated into a single larger file instead of the default one file per zone.
  (常规区域聚合：可以把连续的常规区域范围聚合为一个较大的单一文件，而不是默认的每区域一个文件。)
* File ownership: The owner UID and GID of zone files is by default 0 (root)
  but can be changed to any valid UID/GID.
  (文件属主：区域文件的属主 UID 和 GID 默认为 0(root)，但可以改为任何有效的 UID/GID。)
* File access permissions: the default 640 access permissions can be changed.
  (文件访问权限：默认的 640 访问权限可以更改。)

IO error handling
(IO 错误处理)
-----------------

Zoned block devices may fail I/O requests for reasons similar to regular block
devices, e.g. due to bad sectors. However, in addition to such known I/O
failure pattern, the standards governing zoned block devices behavior define
additional conditions that result in I/O errors.
(分区块设备可能因与常规块设备类似的原因(例如坏扇区)导致 I/O 请求失败。然而，除了这类已知的 I/O 失败模式之外，规范分区块设备行为的标准还定义了会导致 I/O 错误的其他条件。)

* A zone may transition to the read-only condition (BLK_ZONE_COND_READONLY):
  While the data already written in the zone is still readable, the zone can
  no longer be written. No user action on the zone (zone management command or
  read/write access) can change the zone condition back to a normal read/write
  state. While the reasons for the device to transition a zone to read-only
  state are not defined by the standards, a typical cause for such transition
  would be a defective write head on an HDD (all zones under this head are
  changed to read-only).
  (区域可能转换为只读状态(BLK_ZONE_COND_READONLY)：虽然区域中已写入的数据仍然可读，但区域不能再被写入。对区域的任何用户操作(区域管理命令或读/写访问)都无法把区域状态改回正常的读/写状态。虽然设备把区域转换为只读状态的原因未由标准定义，但这种转换的典型原因是硬盘上的写磁头损坏(该磁头下的所有区域都变为只读)。)

* A zone may transition to the offline condition (BLK_ZONE_COND_OFFLINE):
  An offline zone cannot be read nor written. No user action can transition an
  offline zone back to an operational good state. Similarly to zone read-only
  transitions, the reasons for a drive to transition a zone to the offline
  condition are undefined. A typical cause would be a defective read-write head
  on an HDD causing all zones on the platter under the broken head to be
  inaccessible.
  (区域可能转换为离线状态(BLK_ZONE_COND_OFFLINE)：离线区域既不能读也不能写。任何用户操作都无法把离线区域转换回可用的良好状态。与区域只读转换类似，驱动器把区域转换为离线状态的原因也未定义。典型原因是硬盘上的读写磁头损坏，导致损坏磁头下方盘片上的所有区域都不可访问。)

* Unaligned write errors: These errors result from the host issuing write
  requests with a start sector that does not correspond to a zone write pointer
  position when the write request is executed by the device. Even though zonefs
  enforces sequential file write for sequential zones, unaligned write errors
  may still happen in the case of a partial failure of a very large direct I/O
  operation split into multiple BIOs/requests or asynchronous I/O operations.
  If one of the write request within the set of sequential write requests
  issued to the device fails, all write requests queued after it will
  become unaligned and fail.
  (非对齐写错误：这些错误源于主机发出的写请求的起始扇区在设备执行该写请求时不对应区域写指针位置。尽管 zonefs 对顺序区域强制顺序文件写入，但在一个被拆分为多个 BIO/请求的超大直接 I/O 操作或异步 I/O 操作发生部分失败的情况下，仍可能出现非对齐写错误。如果发给设备的这组顺序写请求中的某一个写请求失败，排在它之后的所有写请求都将变为非对齐并失败。)

* Delayed write errors: similarly to regular block devices, if the device side
  write cache is enabled, write errors may occur in ranges of previously
  completed writes when the device write cache is flushed, e.g. on fsync().
  Similarly to the previous immediate unaligned write error case, delayed write
  errors can propagate through a stream of cached sequential data for a zone
  causing all data to be dropped after the sector that caused the error.
  (延迟写错误：与常规块设备类似，如果启用了设备侧写缓存，那么在设备写缓存被刷出时(例如 fsync() 时)，可能在先前已完成写入的范围内出现写错误。与前面立即非对齐写错误的情况类似，延迟写错误可能沿某个区域的缓存顺序数据流传播，导致错误扇区之后的所有数据被丢弃。)

All I/O errors detected by zonefs are notified to the user with an error code
return for the system call that triggered or detected the error. The recovery
actions taken by zonefs in response to I/O errors depend on the I/O type (read
vs write) and on the reason for the error (bad sector, unaligned writes or zone
condition change).
(zonefs 检测到的所有 I/O 错误都会通过触发或检测到该错误的系统调用返回的错误码通知用户。zonefs 针对 I/O 错误采取的恢复动作取决于 I/O 类型(读还是写)以及错误原因(坏扇区、非对齐写或区域状态变化)。)

* For read I/O errors, zonefs does not execute any particular recovery action,
  but only if the file zone is still in a good condition and there is no
  inconsistency between the file inode size and its zone write pointer position.
  If a problem is detected, I/O error recovery is executed (see below table).
  (对于读 I/O 错误，zonefs 不执行任何特定的恢复动作，但前提是文件区域仍处于良好状态，且文件 inode 大小与其区域写指针位置之间无不一致。如果检测到问题，则执行 I/O 错误恢复(见下表)。)

* For write I/O errors, zonefs I/O error recovery is always executed.
  (对于写 I/O 错误，总是执行 zonefs 的 I/O 错误恢复。)

* A zone condition change to read-only or offline also always triggers zonefs
  I/O error recovery.
  (区域状态变为只读或离线也总是触发 zonefs 的 I/O 错误恢复。)

Zonefs minimal I/O error recovery may change a file size and file access
permissions.
(zonefs 的最小 I/O 错误恢复可能会改变文件大小和文件访问权限。)

* File size changes:
  Immediate or delayed write errors in a sequential zone file may cause the file
  inode size to be inconsistent with the amount of data successfully written in
  the file zone. For instance, the partial failure of a multi-BIO large write
  operation will cause the zone write pointer to advance partially, even though
  the entire write operation will be reported as failed to the user. In such
  case, the file inode size must be advanced to reflect the zone write pointer
  change and eventually allow the user to restart writing at the end of the
  file.
  A file size may also be reduced to reflect a delayed write error detected on
  fsync(): in this case, the amount of data effectively written in the zone may
  be less than originally indicated by the file inode size. After such I/O
  error, zonefs always fixes the file inode size to reflect the amount of data
  persistently stored in the file zone.
  (文件大小变化：顺序区域文件中的立即或延迟写错误可能导致文件 inode 大小与文件区域中成功写入的数据量不一致。例如，一个多 BIO 大写操作的部分失败会导致区域写指针部分前进，尽管整个写操作会被报告为对用户失败。在这种情况下，必须把文件 inode 大小推进以反映区域写指针的变化，并最终允许用户从文件末尾重新开始写入。文件大小也可能被缩小以反映在 fsync() 时检测到的延迟写错误：在这种情况下，区域中实际写入的数据量可能小于文件 inode 大小最初指示的量。在此类 I/O 错误之后，zonefs 总是会修正文件 inode 大小，使其反映文件区域中持久存储的数据量。)

* Access permission changes:
  A zone condition change to read-only is indicated with a change in the file
  access permissions to render the file read-only. This disables changes to the
  file attributes and data modification. For offline zones, all permissions
  (read and write) to the file are disabled.
  (访问权限变化：区域状态变为只读通过文件访问权限的改变来指示，使文件变为只读。这会禁用对文件属性的更改和数据修改。对于离线区域，文件的所有权限(读和写)都被禁用。)

Further action taken by zonefs I/O error recovery can be controlled by the user
with the "errors=xxx" mount option. The table below summarizes the result of
zonefs I/O error processing depending on the mount option and on the zone
conditions::

    +--------------+-----------+-----------------------------------------+
    |              |           |            Post error state             |
    | "errors=xxx" |  device   |                 access permissions      |
    |    mount     |   zone    | file         file          device zone  |
    |    option    | condition | size     read    write    read    write |
    +--------------+-----------+-----------------------------------------+
    |              | good      | fixed    yes     no       yes     yes   |
    | remount-ro   | read-only | as is    yes     no       yes     no    |
    | (default)    | offline   |   0      no      no       no      no    |
    +--------------+-----------+-----------------------------------------+
    |              | good      | fixed    yes     no       yes     yes   |
    | zone-ro      | read-only | as is    yes     no       yes     no    |
    |              | offline   |   0      no      no       no      no    |
    +--------------+-----------+-----------------------------------------+
    |              | good      |   0      no      no       yes     yes   |
    | zone-offline | read-only |   0      no      no       yes     no    |
    |              | offline   |   0      no      no       no      no    |
    +--------------+-----------+-----------------------------------------+
    |              | good      | fixed    yes     yes      yes     yes   |
    | repair       | read-only | as is    yes     no       yes     no    |
    |              | offline   |   0      no      no       no      no    |
    +--------------+-----------+-----------------------------------------+

(zonefs I/O 错误恢复采取的进一步操作可由用户通过 "errors=xxx" 挂载选项控制。下表总结了 zonefs I/O 错误处理的结果与挂载选项及区域状态的关系:)

Further notes:
(补充说明:)

* The "errors=remount-ro" mount option is the default behavior of zonefs I/O
  error processing if no errors mount option is specified.
  (如果没有指定 errors 挂载选项，"errors=remount-ro" 挂载选项是 zonefs I/O 错误处理的默认行为。)
* With the "errors=remount-ro" mount option, the change of the file access
  permissions to read-only applies to all files. The file system is remounted
  read-only.
  (使用 "errors=remount-ro" 挂载选项时，文件访问权限改为只读的变更适用于所有文件。文件系统被重新以只读挂载。)
* Access permission and file size changes due to the device transitioning zones
  to the offline condition are permanent. Remounting or reformatting the device
  with mkfs.zonefs (mkzonefs) will not change back offline zone files to a good
  state.
  (因设备把区域转为离线状态而导致的访问权限和文件大小变化是永久性的。重新挂载或用 mkfs.zonefs(mkzonefs) 重新格式化设备都不会把离线区域文件改回良好状态。)
* File access permission changes to read-only due to the device transitioning
  zones to the read-only condition are permanent. Remounting or reformatting
  the device will not re-enable file write access.
  (因设备把区域转为只读状态而导致的文件访问权限改为只读的变化是永久性的。重新挂载或重新格式化设备都不会重新启用文件写访问。)
* File access permission changes implied by the remount-ro, zone-ro and
  zone-offline mount options are temporary for zones in a good condition.
  Unmounting and remounting the file system will restore the previous default
  (format time values) access rights to the files affected.
  (remount-ro、zone-ro 和 zone-offline 挂载选项所隐含的文件访问权限变化，对于处于良好状态的区域是临时的。卸载并重新挂载文件系统会把受影响文件先前的默认(格式化时)访问权限恢复回来。)
* The repair mount option triggers only the minimal set of I/O error recovery
  actions, that is, file size fixes for zones in a good condition. Zones
  indicated as being read-only or offline by the device still imply changes to
  the zone file access permissions as noted in the table above.
  (repair 挂载选项只触发最小的 I/O 错误恢复动作集合，即对处于良好状态的区域进行文件大小修正。设备指示为只读或离线的区域仍然会像上表所述那样导致区域文件访问权限的变化。)

Mount options
(挂载选项)
-------------

zonefs defines several mount options:
(zonefs 定义了若干挂载选项:)
* errors=<behavior>
* explicit-open

"errors=<behavior>" option
("errors=<behavior>" 选项)
~~~~~~~~~~~~~~~~~~~~~~~~~~

The "errors=<behavior>" option mount option allows the user to specify zonefs
behavior in response to I/O errors, inode size inconsistencies or zone
condition changes. The defined behaviors are as follow:
("errors=<behavior>" 挂载选项允许用户指定 zonefs 针对 I/O 错误、inode 大小不一致或区域状态变化时的行为。定义的行为如下:)

* remount-ro (default)
  (默认)
* zone-ro
* zone-offline
* repair

The run-time I/O error actions defined for each behavior are detailed in the
previous section. Mount time I/O errors will cause the mount operation to fail.
The handling of read-only zones also differs between mount-time and run-time.
If a read-only zone is found at mount time, the zone is always treated in the
same manner as offline zones, that is, all accesses are disabled and the zone
file size set to 0. This is necessary as the write pointer of read-only zones
is defined as invalib by the ZBC and ZAC standards, making it impossible to
discover the amount of data that has been written to the zone. In the case of a
read-only zone discovered at run-time, as indicated in the previous section.
The size of the zone file is left unchanged from its last updated value.
(每种行为定义的运行时 I/O 错误动作已在上一节详述。挂载时发生的 I/O 错误会导致挂载操作失败。只读区域的处理在挂载时和运行时也有所不同。如果在挂载时发现只读区域，该区域总是按与离线区域相同的方式处理，即禁用所有访问并把区域文件大小设为 0。这是必需的，因为 ZBC 和 ZAC 标准把只读区域的写指针定义为无效，使得无法得知已写入该区域的数据量。而对于运行时发现的只读区域，如上一节所述，区域文件的大小保持其最近更新的值不变。)

"explicit-open" option
("explicit-open" 选项)
~~~~~~~~~~~~~~~~~~~~~~

A zoned block device (e.g. an NVMe Zoned Namespace device) may have limits on
the number of zones that can be active, that is, zones that are in the
implicit open, explicit open or closed conditions.  This potential limitation
translates into a risk for applications to see write IO errors due to this
limit being exceeded if the zone of a file is not already active when a write
request is issued by the user.
(分区块设备(例如 NVMe Zoned Namespace 设备)可能对可处于活动状态的区域数量有限制，即处于隐式打开、显式打开或关闭状态的区域数量。这一潜在限制意味着：如果用户发出写请求时文件的区域尚未处于活动状态，应用程序可能会因超出该限制而看到写 IO 错误。)

To avoid these potential errors, the "explicit-open" mount option forces zones
to be made active using an open zone command when a file is opened for writing
for the first time. If the zone open command succeeds, the application is then
guaranteed that write requests can be processed. Conversely, the
"explicit-open" mount option will result in a zone close command being issued
to the device on the last close() of a zone file if the zone is not full nor
empty.
(为避免这些潜在错误，"explicit-open" 挂载选项强制在文件首次以写方式打开时使用打开区域命令把区域激活。如果打开区域命令成功，应用程序随后就能得到保证：写请求可以被处理。反之，"explicit-open" 挂载选项也会导致：当区域既不满也不空时，在区域文件最后一次 close() 时向设备发出关闭区域命令。)

Runtime sysfs attributes
(运行时 sysfs 属性)
------------------------

zonefs defines several sysfs attributes for mounted devices.  All attributes
are user readable and can be found in the directory /sys/fs/zonefs/<dev>/,
where <dev> is the name of the mounted zoned block device.
(zonefs 为已挂载的设备定义了若干 sysfs 属性。所有属性用户可读，可在目录 /sys/fs/zonefs/<dev>/ 下找到，其中 <dev> 是已挂载分区块设备的名称。)

The attributes defined are as follows.
(定义的属性如下。)

* **max_wro_seq_files**:  This attribute reports the maximum number of
  sequential zone files that can be open for writing.  This number corresponds
  to the maximum number of explicitly or implicitly open zones that the device
  supports.  A value of 0 means that the device has no limit and that any zone
  (any file) can be open for writing and written at any time, regardless of the
  state of other zones.  When the *explicit-open* mount option is used, zonefs
  will fail any open() system call requesting to open a sequential zone file for
  writing when the number of sequential zone files already open for writing has
  reached the *max_wro_seq_files* limit.
  (**max_wro_seq_files**: 该属性报告可为写入而打开的顺序区域文件的最大数量。该数字对应设备支持的显式或隐式打开区域的最大数量。值为 0 表示设备没有限制，任何区域(任何文件)都可以随时为写入而打开并写入，而不受其他区域状态的影响。使用 *explicit-open* 挂载选项时，当已为写入打开的顺序区域文件数量达到 *max_wro_seq_files* 上限时，zonefs 会使任何请求为写入而打开顺序区域文件的 open() 系统调用失败。)
* **nr_wro_seq_files**:  This attribute reports the current number of sequential
  zone files open for writing.  When the "explicit-open" mount option is used,
  this number can never exceed *max_wro_seq_files*.  If the *explicit-open*
  mount option is not used, the reported number can be greater than
  *max_wro_seq_files*.  In such case, it is the responsibility of the
  application to not write simultaneously more than *max_wro_seq_files*
  sequential zone files.  Failure to do so can result in write errors.
  (**nr_wro_seq_files**: 该属性报告当前为写入而打开的顺序区域文件数量。使用 "explicit-open" 挂载选项时，该数字永远不会超过 *max_wro_seq_files*。如果未使用 *explicit-open* 挂载选项，报告的数字可能大于 *max_wro_seq_files*。在这种情况下，应用程序有责任不要同时写入超过 *max_wro_seq_files* 个顺序区域文件，否则可能导致写错误。)
* **max_active_seq_files**:  This attribute reports the maximum number of
  sequential zone files that are in an active state, that is, sequential zone
  files that are partially written (not empty nor full) or that have a zone that
  is explicitly open (which happens only if the *explicit-open* mount option is
  used).  This number is always equal to the maximum number of active zones that
  the device supports.  A value of 0 means that the mounted device has no limit
  on the number of sequential zone files that can be active.
  (**max_active_seq_files**: 该属性报告处于活动状态的顺序区域文件的最大数量，即部分写入(非空也非满)或其区域被显式打开(仅在使用 *explicit-open* 挂载选项时才会发生)的顺序区域文件。该数字总是等于设备支持的活动区域最大数量。值为 0 表示所挂载设备对可处于活动状态的顺序区域文件数量没有限制。)
* **nr_active_seq_files**:  This attributes reports the current number of
  sequential zone files that are active. If *max_active_seq_files* is not 0,
  then the value of *nr_active_seq_files* can never exceed the value of
  *nr_active_seq_files*, regardless of the use of the *explicit-open* mount
  option.
  (**nr_active_seq_files**: 该属性报告当前处于活动状态的顺序区域文件数量。如果 *max_active_seq_files* 不为 0，那么无论是否使用 *explicit-open* 挂载选项，*nr_active_seq_files* 的值都永远不会超过 *nr_active_seq_files* 的值。)

Zonefs User Space Tools
(Zonefs 用户空间工具)
=======================

The mkzonefs tool is used to format zoned block devices for use with zonefs.
This tool is available on Github at:
(mkzonefs 工具用于为配合 zonefs 使用而格式化分区块设备。该工具可在 Github 上获取:)

https://github.com/damien-lemoal/zonefs-tools

zonefs-tools also includes a test suite which can be run against any zoned
block device, including null_blk block device created with zoned mode.
(zonefs-tools 还包含一个测试套件，可以针对任何分区块设备运行，包括以 zoned 模式创建的 null_blk 块设备。)

Examples
(示例)
--------

The following formats a 15TB host-managed SMR HDD with 256 MB zones
with the conventional zones aggregation feature enabled::

    # mkzonefs -o aggr_cnv /dev/sdX
    # mount -t zonefs /dev/sdX /mnt
    # ls -l /mnt/
    total 0
    dr-xr-xr-x 2 root root     1 Nov 25 13:23 cnv
    dr-xr-xr-x 2 root root 55356 Nov 25 13:23 seq

(以下命令把一块 15TB 的主机管理式 SMR 硬盘格式化为 256 MB 的区域，并启用常规区域聚合特性:)

The size of the zone files sub-directories indicate the number of files
existing for each type of zones. In this example, there is only one
conventional zone file (all conventional zones are aggregated under a single
file)::

    # ls -l /mnt/cnv
    total 137101312
    -rw-r----- 1 root root 140391743488 Nov 25 13:23 0

(区域文件子目录的大小指示每种类型区域存在的文件数量。在此示例中只有一个常规区域文件(所有常规区域都聚合在同一个文件之下):)

This aggregated conventional zone file can be used as a regular file::

    # mkfs.ext4 /mnt/cnv/0
    # mount -o loop /mnt/cnv/0 /data

(这个聚合的常规区域文件可以当作常规文件使用:)

The "seq" sub-directory grouping files for sequential write zones has in this
example 55356 zones::

    # ls -lv /mnt/seq
    total 14511243264
    -rw-r----- 1 root root 0 Nov 25 13:23 0
    -rw-r----- 1 root root 0 Nov 25 13:23 1
    -rw-r----- 1 root root 0 Nov 25 13:23 2
    ...
    -rw-r----- 1 root root 0 Nov 25 13:23 55354
    -rw-r----- 1 root root 0 Nov 25 13:23 55355

("seq" 子目录分组存放顺序写区域的文件，在此示例中有 55356 个区域:)

For sequential write zone files, the file size changes as data is appended at
the end of the file, similarly to any regular file system::

    # dd if=/dev/zero of=/mnt/seq/0 bs=4096 count=1 conv=notrunc oflag=direct
    1+0 records in
    1+0 records out
    4096 bytes (4.1 kB, 4.0 KiB) copied, 0.00044121 s, 9.3 MB/s

    # ls -l /mnt/seq/0
    -rw-r----- 1 root root 4096 Nov 25 13:23 /mnt/seq/0

(对于顺序写区域文件，随着数据在文件末尾追加，文件大小会像任何常规文件系统一样变化:)

The written file can be truncated to the zone size, preventing any further
write operation::

    # truncate -s 268435456 /mnt/seq/0
    # ls -l /mnt/seq/0
    -rw-r----- 1 root root 268435456 Nov 25 13:49 /mnt/seq/0

(写入后的文件可以截断到区域大小，从而阻止任何进一步的写操作:)

Truncation to 0 size allows freeing the file zone storage space and restart
append-writes to the file::

    # truncate -s 0 /mnt/seq/0
    # ls -l /mnt/seq/0
    -rw-r----- 1 root root 0 Nov 25 13:49 /mnt/seq/0

(截断到 0 大小可以释放文件区域的存储空间，并重新开始对该文件的追加写:)

Since files are statically mapped to zones on the disk, the number of blocks
of a file as reported by stat() and fstat() indicates the capacity of the file
zone::

    # stat /mnt/seq/0
    File: /mnt/seq/0
    Size: 0         	Blocks: 524288     IO Block: 4096   regular empty file
    Device: 870h/2160d	Inode: 50431       Links: 1
    Access: (0640/-rw-r-----)  Uid: (    0/    root)   Gid: (    0/    root)
    Access: 2019-11-25 13:23:57.048971997 +0900
    Modify: 2019-11-25 13:52:25.553805765 +0900
    Change: 2019-11-25 13:52:25.553805765 +0900
    Birth: -

(由于文件静态映射到磁盘上的区域，stat() 和 fstat() 报告的文件块数表示文件区域的容量:)

The number of blocks of the file ("Blocks") in units of 512B blocks gives the
maximum file size of 524288 * 512 B = 256 MB, corresponding to the device zone
capacity in this example. Of note is that the "IO block" field always
indicates the minimum I/O size for writes and corresponds to the device
physical sector size.
(文件的块数("Blocks")以 512B 块为单位，给出最大文件大小 524288 * 512 B = 256 MB，对应此示例中的设备区域容量。值得注意的是，"IO block" 字段始终指示写入的最小 I/O 大小，对应设备的物理扇区大小。)
