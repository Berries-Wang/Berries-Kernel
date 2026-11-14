.. SPDX-License-Identifier: GPL-2.0

===========================================
Cramfs - cram a filesystem onto a small ROM
===========================================

(Cramfs——把一个文件系统塞进一个小 ROM)

cramfs is designed to be simple and small, and to compress things well.
(cramfs 的设计目标是简单、小巧，并具有良好的压缩效果。)

It uses the zlib routines to compress a file one page at a time, and
allows random page access.  The meta-data is not compressed, but is
expressed in a very terse representation to make it use much less
diskspace than traditional filesystems.
(它使用 zlib 例程一次对一个文件的一页进行压缩，并允许随机页访问。元数据不做压缩，但以非常紧凑的形式表示，使其比传统文件系统少占用很多磁盘空间。)

You can't write to a cramfs filesystem (making it compressible and
compact also makes it _very_ hard to update on-the-fly), so you have to
create the disk image with the "mkcramfs" utility.
(你无法写入 cramfs 文件系统(使它可压缩且紧凑的同时，也让它_非常_难以即时更新)，因此你必须使用 "mkcramfs" 工具来创建磁盘映像。)


Usage Notes
(使用注意事项)
-----------

File sizes are limited to less than 16MB.
(文件大小被限制在 16MB 以内。)

Maximum filesystem size is a little over 256MB.  (The last file on the
filesystem is allowed to extend past 256MB.)
(文件系统的最大大小略高于 256MB。(文件系统上的最后一个文件允许延伸到 256MB 之外。))

Only the low 8 bits of gid are stored.  The current version of
mkcramfs simply truncates to 8 bits, which is a potential security
issue.
(只存储 gid 的低 8 位。当前版本的 mkcramfs 只是简单地截断为 8 位，这是一个潜在的安全问题。)

Hard links are supported, but hard linked files
will still have a link count of 1 in the cramfs image.
(支持硬链接，但硬链接的文件在 cramfs 映像中的链接计数仍为 1。)

Cramfs directories have no ``.`` or ``..`` entries.  Directories (like
every other file on cramfs) always have a link count of 1.  (There's
no need to use -noleaf in ``find``, btw.)
(cramfs 目录没有 ``.`` 或 ``..`` 条目。目录(与 cramfs 上的其他所有文件一样)的链接计数始终为 1。(顺便说一句，在 ``find`` 中无需使用 -noleaf。))

No timestamps are stored in a cramfs, so these default to the epoch
(1970 GMT).  Recently-accessed files may have updated timestamps, but
the update lasts only as long as the inode is cached in memory, after
which the timestamp reverts to 1970, i.e. moves backwards in time.
(cramfs 中不存储时间戳，因此这些值默认为纪元(1970 GMT)。最近访问过的文件可能带有更新过的时间戳，但该更新只在 inode 还缓存在内存中期间有效，此后时间戳会恢复为 1970 年，即在时间上向后倒退。)

Currently, cramfs must be written and read with architectures of the
same endianness, and can be read only by kernels with PAGE_SIZE
== 4096.  At least the latter of these is a bug, but it hasn't been
decided what the best fix is.  For the moment if you have larger pages
you can just change the #define in mkcramfs.c, so long as you don't
mind the filesystem becoming unreadable to future kernels.
(目前，cramfs 必须在相同字节序的体系结构上写入和读取，并且只能由 PAGE_SIZE == 4096 的内核读取。至少后者是一个缺陷，但尚未决定什么是最佳的修复方案。眼下，如果你使用更大的页，你可以直接修改 mkcramfs.c 中的 #define，只要你不在乎该文件系统在未来内核中变得不可读。)


Memory Mapped cramfs image
(内存映射的 cramfs 映像)
--------------------------

The CRAMFS_MTD Kconfig option adds support for loading data directly from
a physical linear memory range (usually non volatile memory like Flash)
instead of going through the block device layer. This saves some memory
since no intermediate buffering is necessary to hold the data before
decompressing.
(CRAMFS_MTD Kconfig 选项增加了对直接从物理线性内存范围(通常是非易失性内存，如 Flash)加载数据的支持，而不经过块设备层。这可以节省一些内存，因为在解压之前无需中间缓冲来保存数据。)

And when data blocks are kept uncompressed and properly aligned, they will
automatically be mapped directly into user space whenever possible providing
eXecute-In-Place (XIP) from ROM of read-only segments. Data segments mapped
read-write (hence they have to be copied to RAM) may still be compressed in
the cramfs image in the same file along with non compressed read-only
segments. Both MMU and no-MMU systems are supported. This is particularly
handy for tiny embedded systems with very tight memory constraints.
(而当数据块保持不压缩且适当对齐时，只要有可能，它们就会被自动直接映射到用户空间，从而从 ROM 中对只读段提供就地执行(eXecute-In-Place, XIP)。以读写方式映射的数据段(因此它们必须被复制到 RAM)仍可能与未压缩的只读段一起，在同一文件中被压缩保存在 cramfs 映像里。MMU 和无 MMU 系统都受支持。这对内存约束非常紧张的微型嵌入式系统特别有用。)

The location of the cramfs image in memory is system dependent. You must
know the proper physical address where the cramfs image is located and
configure an MTD device for it. Also, that MTD device must be supported
by a map driver that implements the "point" method. Examples of such
MTD drivers are cfi_cmdset_0001 (Intel/Sharp CFI flash) or physmap
(Flash device in physical memory map). MTD partitions based on such devices
are fine too. Then that device should be specified with the "mtd:" prefix
as the mount device argument. For example, to mount the MTD device named
"fs_partition" on the /mnt directory::
(cramfs 映像在内存中的位置与系统相关。你必须知道 cramfs 映像所在的正确物理地址，并为其配置一个 MTD 设备。此外，该 MTD 设备必须得到实现了 "point" 方法的映射驱动程序的支持。这类 MTD 驱动程序的例子有 cfi_cmdset_0001 (Intel/Sharp CFI flash)或 physmap(物理内存映射中的 Flash 设备)。基于此类设备的 MTD 分区也可以。然后，应以 "mtd:" 前缀将该设备指定为挂载设备参数。例如，要把名为 "fs_partition" 的 MTD 设备挂载到 /mnt 目录：)

    $ mount -t cramfs mtd:fs_partition /mnt

To boot a kernel with this as root filesystem, suffice to specify
something like "root=mtd:fs_partition" on the kernel command line.
(要以此作为根文件系统来引导内核，只需在内核命令行上指定类似 "root=mtd:fs_partition" 的内容即可。)


Tools
(工具)
-----

A version of mkcramfs that can take advantage of the latest capabilities
described above can be found here:
(能够利用上述最新功能的 mkcramfs 版本可在此处找到：)

https://github.com/npitre/cramfs-tools


For /usr/share/magic
(用于 /usr/share/magic)
--------------------

=====	=======================	=======================
0	ulelong	0x28cd3d45	Linux cramfs offset 0(偏移 0 处的 Linux cramfs)
>4	ulelong	x		size %d
>8	ulelong	x		flags 0x%x
>12	ulelong	x		future 0x%x
>16	string	>\0		signature "%.16s"
>32	ulelong	x		fsid.crc 0x%x
>36	ulelong	x		fsid.edition %d
>40	ulelong	x		fsid.blocks %d
>44	ulelong	x		fsid.files %d
>48	string	>\0		name "%.16s"
512	ulelong	0x28cd3d45	Linux cramfs offset 512(偏移 512 处的 Linux cramfs)
>516	ulelong	x		size %d
>520	ulelong	x		flags 0x%x
>524	ulelong	x		future 0x%x
>528	string	>\0		signature "%.16s"
>544	ulelong	x		fsid.crc 0x%x
>548	ulelong	x		fsid.edition %d
>552	ulelong	x		fsid.blocks %d
>556	ulelong	x		fsid.files %d
>560	string	>\0		name "%.16s"
=====	=======================	=======================


Hacker Notes
(Hacker 注记)
------------

See fs/cramfs/README for filesystem layout and implementation notes.
(文件系统布局和实现说明请参见 fs/cramfs/README。)
