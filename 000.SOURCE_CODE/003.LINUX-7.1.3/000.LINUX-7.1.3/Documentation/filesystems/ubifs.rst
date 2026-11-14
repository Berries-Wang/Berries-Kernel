.. SPDX-License-Identifier: GPL-2.0

===============
UBI File System
===============

(UBI 文件系统)

Introduction
(简介)
============

UBIFS file-system stands for UBI File System. UBI stands for "Unsorted
Block Images". UBIFS is a flash file system, which means it is designed
to work with flash devices. It is important to understand, that UBIFS
is completely different to any traditional file-system in Linux, like
Ext2, XFS, JFS, etc. UBIFS represents a separate class of file-systems
which work with MTD devices, not block devices. The other Linux
file-system of this class is JFFS2.
(UBIFS 文件系统代表 UBI File System。UBI 代表 "Unsorted Block Images"(无序块映像)。UBIFS 是一种闪存文件系统，这意味着它是为闪存设备设计的。需要理解的重要一点是，UBIFS 与 Linux 中任何传统文件系统(如 Ext2、XFS、JFS 等)完全不同。UBIFS 代表了一类单独的文件系统，它们与 MTD 设备而非块设备协同工作。属于这一类的另一个 Linux 文件系统是 JFFS2。)

To make it more clear, here is a small comparison of MTD devices and
block devices.
(为了更清楚起见，这里对 MTD 设备和块设备做一个简单比较。)

1 MTD devices represent flash devices and they consist of eraseblocks of
  rather large size, typically about 128KiB. Block devices consist of
  small blocks, typically 512 bytes.
  (MTD 设备代表闪存设备，它们由尺寸相当大的擦除块组成，通常约为 128KiB。块设备由小块组成，通常为 512 字节。)
2 MTD devices support 3 main operations - read from some offset within an
  eraseblock, write to some offset within an eraseblock, and erase a whole
  eraseblock. Block  devices support 2 main operations - read a whole
  block and write a whole block.
  (MTD 设备支持 3 种主要操作——从擦除块内某一偏移读取、向擦除块内某一偏移写入、以及擦除整个擦除块。块设备支持 2 种主要操作——读取整个块和写入整个块。)
3 The whole eraseblock has to be erased before it becomes possible to
  re-write its contents. Blocks may be just re-written.
  (整个擦除块必须先被擦除，然后才可能重写其内容。块则可以直接重写。)
4 Eraseblocks become worn out after some number of erase cycles -
  typically 100K-1G for SLC NAND and NOR flashes, and 1K-10K for MLC
  NAND flashes. Blocks do not have the wear-out property.
  (擦除块在经过一定次数的擦除周期后会磨损——对 SLC NAND 和 NOR 闪存通常为 100K-1G 次，对 MLC NAND 闪存通常为 1K-10K 次。块没有磨损特性。)
5 Eraseblocks may become bad (only on NAND flashes) and software should
  deal with this. Blocks on hard drives typically do not become bad,
  because hardware has mechanisms to substitute bad blocks, at least in
  modern LBA disks.
  (擦除块可能变坏(仅限 NAND 闪存)，软件应当处理这种情况。硬盘上的块通常不会变坏，因为硬件具有替换坏块的机制，至少在现代 LBA 磁盘上是如此。)

It should be quite obvious why UBIFS is very different to traditional
file-systems.
(为什么 UBIFS 与传统文件系统大不相同，其原因应当相当明显。)

UBIFS works on top of UBI. UBI is a separate software layer which may be
found in drivers/mtd/ubi. UBI is basically a volume management and
wear-leveling layer. It provides so called UBI volumes which is a higher
level abstraction than a MTD device. The programming model of UBI devices
is very similar to MTD devices - they still consist of large eraseblocks,
they have read/write/erase operations, but UBI devices are devoid of
limitations like wear and bad blocks (items 4 and 5 in the above list).
(UBIFS 工作在 UBI 之上。UBI 是一个独立的软件层，可以在 drivers/mtd/ubi 中找到。UBI 本质上是一个卷管理和磨损均衡层。它提供所谓的 UBI 卷，这是一种比 MTD 设备更高层次的抽象。UBI 设备的编程模型与 MTD 设备非常相似——它们仍由大的擦除块组成，具有读/写/擦除操作，但 UBI 设备没有磨损和坏块之类的限制(即上面列表中的第 4 和第 5 项)。)

In a sense, UBIFS is a next generation of JFFS2 file-system, but it is
very different and incompatible to JFFS2. The following are the main
differences.
(从某种意义上说，UBIFS 是下一代 JFFS2 文件系统，但它与 JFFS2 非常不同且不兼容。以下是主要区别。)

* JFFS2 works on top of MTD devices, UBIFS depends on UBI and works on
  top of UBI volumes.
  (JFFS2 工作在 MTD 设备之上，而 UBIFS 依赖 UBI 并工作在 UBI 卷之上。)
* JFFS2 does not have on-media index and has to build it while mounting,
  which requires full media scan. UBIFS maintains the FS indexing
  information on the flash media and does not require full media scan,
  so it mounts many times faster than JFFS2.
  (JFFS2 没有介质上的索引，必须在挂载时构建索引，这需要完整的介质扫描。UBIFS 将文件系统索引信息维护在闪存介质上，不需要完整的介质扫描，因此其挂载速度比 JFFS2 快许多倍。)
* JFFS2 is a write-through file-system, while UBIFS supports write-back,
  which makes UBIFS much faster on writes.
  (JFFS2 是直写(write-through)文件系统，而 UBIFS 支持回写(write-back)，这使 UBIFS 的写入速度快得多。)

Similarly to JFFS2, UBIFS supports on-the-fly compression which makes
it possible to fit quite a lot of data to the flash.
(与 JFFS2 类似，UBIFS 支持即时压缩，这使得相当多的数据能够被装进闪存。)

Similarly to JFFS2, UBIFS is tolerant of unclean reboots and power-cuts.
It does not need stuff like fsck.ext2. UBIFS automatically replays its
journal and recovers from crashes, ensuring that the on-flash data
structures are consistent.
(与 JFFS2 类似，UBIFS 能够容忍不干净的重启和掉电。它不需要 fsck.ext2 之类的东西。UBIFS 会自动重放其日志并从崩溃中恢复，确保闪存上的数据结构是一致的。)

UBIFS scales logarithmically (most of the data structures it uses are
trees), so the mount time and memory consumption do not linearly depend
on the flash size, like in case of JFFS2. This is because UBIFS
maintains the FS index on the flash media. However, UBIFS depends on
UBI, which scales linearly. So overall UBI/UBIFS stack scales linearly.
Nevertheless, UBI/UBIFS scales considerably better than JFFS2.
(UBIFS 按对数方式扩展(它使用的大多数数据结构都是树)，因此挂载时间和内存消耗并不像 JFFS2 那样与闪存大小成线性关系。这是因为 UBIFS 将文件系统索引维护在闪存介质上。然而，UBIFS 依赖的 UBI 是按线性方式扩展的。所以整体上 UBI/UBIFS 栈是线性扩展的。尽管如此，UBI/UBIFS 的扩展性仍比 JFFS2 好得多。)

The authors of UBIFS believe, that it is possible to develop UBI2 which
would scale logarithmically as well. UBI2 would support the same API as UBI,
but it would be binary incompatible to UBI. So UBIFS would not need to be
changed to use UBI2
(UBIFS 的作者认为，开发同样按对数方式扩展的 UBI2 是可能的。UBI2 将支持与 UBI 相同的 API，但与 UBI 二进制不兼容。因此 UBIFS 无需更改即可使用 UBI2。)


Mount options
(挂载选项)
=============

(*) == default.
((*) 表示默认值。)

====================	=======================================================
bulk_read		read more in one go to take advantage of flash
			media that read faster sequentially(一次性读取更多内容，以充分利用顺序读取速度较快的闪存介质。)
no_bulk_read (*)	do not bulk-read(不进行批量读取)
no_chk_data_crc (*)	skip checking of CRCs on data nodes in order to
			improve read performance. Use this option only
			if the flash media is highly reliable. The effect
			of this option is that corruption of the contents
			of a file can go unnoticed.(跳过对数据节点上 CRC 的检查，以提高读取性能。仅在闪存介质高度可靠时才使用此选项。此选项的效果是文件内容的损坏可能不会被察觉。)
chk_data_crc		do not skip checking CRCs on data nodes(不跳过对数据节点上 CRC 的检查)
compr=none              override default compressor and set it to "none"(覆盖默认压缩器并将其设置为 "none")
compr=lzo               override default compressor and set it to "lzo"(覆盖默认压缩器并将其设置为 "lzo")
compr=zlib              override default compressor and set it to "zlib"(覆盖默认压缩器并将其设置为 "zlib")
auth_key=		specify the key used for authenticating the filesystem.
			Passing this option makes authentication mandatory.
			The passed key must be present in the kernel keyring
			and must be of type 'logon'(指定用于认证文件系统的密钥。传递此选项将使认证成为强制要求。所传递的密钥必须存在于内核密钥环中，且类型必须为 'logon')
auth_hash_name=		The hash algorithm used for authentication. Used for
			both hashing and for creating HMACs. Typical values
			include "sha256" or "sha512"(用于认证的哈希算法，同时用于哈希运算和创建 HMAC。典型值包括 "sha256" 或 "sha512")
====================	=======================================================


Quick usage instructions
(快速使用说明)
========================

The UBI volume to mount is specified using "ubiX_Y" or "ubiX:NAME" syntax,
where "X" is UBI device number, "Y" is UBI volume number, and "NAME" is
UBI volume name.
(要挂载的 UBI 卷使用 "ubiX_Y" 或 "ubiX:NAME" 语法指定，其中 "X" 是 UBI 设备号，"Y" 是 UBI 卷号，"NAME" 是 UBI 卷名。)

Mount volume 0 on UBI device 0 to /mnt/ubifs::
(将 UBI 设备 0 上的卷 0 挂载到 /mnt/ubifs：)

    $ mount -t ubifs ubi0_0 /mnt/ubifs

Mount "rootfs" volume of UBI device 0 to /mnt/ubifs ("rootfs" is volume
name)::
(将 UBI 设备 0 的 "rootfs" 卷挂载到 /mnt/ubifs("rootfs" 是卷名)：)

    $ mount -t ubifs ubi0:rootfs /mnt/ubifs

The following is an example of the kernel boot arguments to attach mtd0
to UBI and mount volume "rootfs":
ubi.mtd=0 root=ubi0:rootfs rootfstype=ubifs
(以下是将 mtd0 关联到 UBI 并挂载 "rootfs" 卷的内核启动参数示例：)

References
(参考)
==========

UBIFS documentation and FAQ/HOWTO at the MTD web site:
(MTD 网站上的 UBIFS 文档与 FAQ/HOWTO：)

- http://www.linux-mtd.infradead.org/doc/ubifs.html
- http://www.linux-mtd.infradead.org/faq/ubifs.html
