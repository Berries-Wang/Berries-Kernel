.. SPDX-License-Identifier: GPL-2.0

===================
The QNX6 Filesystem
===================

(The QNX6 文件系统)

The qnx6fs is used by newer QNX operating system versions. (e.g. Neutrino)
It got introduced in QNX 6.4.0 and is used default since 6.4.1.
(qnx6fs 被较新的 QNX 操作系统版本所使用(例如 Neutrino)。它于 QNX 6.4.0 中引入，并自 6.4.1 起作为默认文件系统。)

Option
(选项)
======

mmi_fs		Mount filesystem as used for example by Audi MMI 3G system
(将文件系统按例如 Audi MMI 3G 系统所使用的方式挂载)

Specification
(规范)
=============

qnx6fs shares many properties with traditional Unix filesystems. It has the
concepts of blocks, inodes and directories.
(qnx6fs 与传统 Unix 文件系统有许多共同的特性。它具有块、inode 和目录的概念。)

On QNX it is possible to create little endian and big endian qnx6 filesystems.
This feature makes it possible to create and use a different endianness fs
for the target (QNX is used on quite a range of embedded systems) platform
running on a different endianness.
(在 QNX 上可以创建小端和大端的 qnx6 文件系统。这一特性使得可以为运行在不同字节序上的目标平台(QNX 被用于相当多种类的嵌入式系统)创建并使用不同字节序的文件系统。)

The Linux driver handles endianness transparently. (LE and BE)
(Linux 驱动程序透明地处理字节序。(LE 与 BE))

Blocks
(块)
------

The space in the device or file is split up into blocks. These are a fixed
size of 512, 1024, 2048 or 4096, which is decided when the filesystem is
created.
(设备或文件中的空间被分割成块。块的大小固定为 512、1024、2048 或 4096，在创建文件系统时决定。)

Blockpointers are 32bit, so the maximum space that can be addressed is
2^32 * 4096 bytes or 16TB
(块指针为 32 位，因此可寻址的最大空间为 2^32 * 4096 字节，即 16TB)

The superblocks
(超级块)
---------------

The superblock contains all global information about the filesystem.
Each qnx6fs got two superblocks, each one having a 64bit serial number.
That serial number is used to identify the "active" superblock.
In write mode with reach new snapshot (after each synchronous write), the
serial of the new master superblock is increased (old superblock serial + 1)
(超级块包含关于文件系统的所有全局信息。每个 qnx6fs 有两个超级块，每个都拥有一个 64 位序列号。该序列号用于标识"活动的"超级块。在写入模式下，每当到达一个新快照(每次同步写入之后)，新的主超级块的序列号就会增加(旧超级块序列号 + 1))

So basically the snapshot functionality is realized by an atomic final
update of the serial number. Before updating that serial, all modifications
are done by copying all modified blocks during that specific write request
(or period) and building up a new (stable) filesystem structure under the
inactive superblock.
(因此，快照功能基本上是通过对序列号的原子性最终更新来实现的。在更新该序列号之前，所有修改都是这样完成的：在该特定写入请求(或周期)期间复制所有被修改的块，并在非活动超级块之下构建一个新的(稳定的)文件系统结构。)

Each superblock holds a set of root inodes for the different filesystem
parts. (Inode, Bitmap and Longfilenames)
Each of these root nodes holds information like total size of the stored
data and the addressing levels in that specific tree.
If the level value is 0, up to 16 direct blocks can be addressed by each
node.
(每个超级块为文件系统的各个不同部分保存一组根 inode。(Inode、Bitmap 和 Longfilenames)这些根节点中的每一个都保存着诸如所存储数据的总大小以及该特定树中的寻址级别之类的信息。如果级别值为 0，则每个节点最多可以寻址 16 个直接块。)

Level 1 adds an additional indirect addressing level where each indirect
addressing block holds up to blocksize / 4 bytes pointers to data blocks.
Level 2 adds an additional indirect addressing block level (so, already up
to 16 * 256 * 256 = 1048576 blocks that can be addressed by such a tree).
(级别 1 增加了一个额外的间接寻址级别，其中每个间接寻址块最多保存 blocksize / 4 字节的数据块指针。级别 2 增加了一个额外的间接寻址块级别(因此，这样一棵树已经可以寻址多达 16 * 256 * 256 = 1048576 个块)。)

Unused block pointers are always set to ~0 - regardless of root node,
indirect addressing blocks or inodes.
(未使用的块指针总是被设置为 ~0——无论是根节点、间接寻址块还是 inode。)

Data leaves are always on the lowest level. So no data is stored on upper
tree levels.
(数据叶子总是位于最低层。因此，上层树级别中不存储任何数据。)

The first Superblock is located at 0x2000. (0x2000 is the bootblock size)
The Audi MMI 3G first superblock directly starts at byte 0.
(第一个超级块位于 0x2000 处。(0x2000 是引导块的大小)Audi MMI 3G 的第一个超级块直接从字节 0 开始。)

Second superblock position can either be calculated from the superblock
information (total number of filesystem blocks) or by taking the highest
device address, zeroing the last 3 bytes and then subtracting 0x1000 from
that address.
(第二个超级块的位置既可以根据超级块信息(文件系统总块数)计算得出，也可以通过取最高的设备地址、将最后 3 个字节清零，然后从该地址减去 0x1000 来确定。)

0x1000 is the size reserved for each superblock - regardless of the
blocksize of the filesystem.
(0x1000 是为每个超级块保留的大小——与文件系统的块大小无关。)

Inodes
(索引节点)
------

Each object in the filesystem is represented by an inode. (index node)
The inode structure contains pointers to the filesystem blocks which contain
the data held in the object and all of the metadata about an object except
its longname. (filenames longer than 27 characters)
The metadata about an object includes the permissions, owner, group, flags,
size, number of blocks used, access time, change time and modification time.
(文件系统中的每个对象都由一个 inode(索引节点)表示。inode 结构包含指向文件系统块的指针，这些块包含对象中保存的数据，以及除 longname 之外关于对象的全部元数据。(文件名长于 27 个字符的情况)关于对象的元数据包括权限、所有者、属组、标志、大小、已使用的块数、访问时间、变更时间和修改时间。)

Object mode field is POSIX format. (which makes things easier)
(对象的 mode 字段采用 POSIX 格式。(这让事情变得更容易))

There are also pointers to the first 16 blocks, if the object data can be
addressed with 16 direct blocks.
(如果对象数据可以用 16 个直接块来寻址，还会有指向前 16 个块的指针。)

For more than 16 blocks an indirect addressing in form of another tree is
used. (scheme is the same as the one used for the superblock root nodes)
(对于超过 16 个块的情况，则使用另一种树形式的间接寻址。(其方案与超级块根节点所使用的方案相同))

The filesize is stored 64bit. Inode counting starts with 1. (while long
filename inodes start with 0)
(文件大小以 64 位存储。inode 计数从 1 开始。(而长文件名 inode 则从 0 开始))

Directories
(目录)
-----------

A directory is a filesystem object and has an inode just like a file.
It is a specially formatted file containing records which associate each
name with an inode number.
(目录是一种文件系统对象，和文件一样拥有一个 inode。它是一个特殊格式的文件，其中包含将每个名称与一个 inode 编号关联起来的记录。)

'.' inode number points to the directory inode
('.' 的 inode 编号指向该目录自身的 inode)

'..' inode number points to the parent directory inode
('..' 的 inode 编号指向父目录的 inode)

Eeach filename record additionally got a filename length field.
(每个文件名记录还额外拥有一个文件名长度字段。)

One special case are long filenames or subdirectory names.
(一种特殊情况是长文件名或子目录名。)

These got set a filename length field of 0xff in the corresponding directory
record plus the longfile inode number also stored in that record.
(这些名称在相应的目录记录中把文件名长度字段设置为 0xff，同时该记录中还存储有长文件 inode 编号。)

With that longfilename inode number, the longfilename tree can be walked
starting with the superblock longfilename root node pointers.
(凭借该长文件名 inode 编号，就可以从超级块的长文件名根节点指针开始遍历长文件名树。)

Special files
(特殊文件)
-------------

Symbolic links are also filesystem objects with inodes. They got a specific
bit in the inode mode field identifying them as symbolic link.
(符号链接也是带有 inode 的文件系统对象。它们在 inode 的 mode 字段中有一个特定的位，用于把自身标识为符号链接。)

The directory entry file inode pointer points to the target file inode.
(目录项文件的 inode 指针指向目标文件的 inode。)

Hard links got an inode, a directory entry, but a specific mode bit set,
no block pointers and the directory file record pointing to the target file
inode.
(硬链接拥有一个 inode 和一个目录项，但设置了特定的 mode 位，没有块指针，并且目录文件记录指向目标文件的 inode。)

Character and block special devices do not exist in QNX as those files
are handled by the QNX kernel/drivers and created in /dev independent of the
underlying filesystem.
(QNX 中不存在字符设备和块设备特殊文件，因为这些文件由 QNX 内核/驱动程序处理，并在 /dev 中创建，与底层文件系统无关。)

Long filenames
(长文件名)
--------------

Long filenames are stored in a separate addressing tree. The staring point
is the longfilename root node in the active superblock.
(长文件名存储在一个单独的寻址树中。其起点是活动超级块中的长文件名根节点。)

Each data block (tree leaves) holds one long filename. That filename is
limited to 510 bytes. The first two starting bytes are used as length field
for the actual filename.
(每个数据块(树叶)保存一个长文件名。该文件名被限制为 510 字节。起始的前两个字节被用作实际文件名的长度字段。)

If that structure shall fit for all allowed blocksizes, it is clear why there
is a limit of 510 bytes for the actual filename stored.
(如果该结构要适用于所有允许的块大小，那么为什么存储的实际文件名有 510 字节的限制就显而易见了。)

Bitmap
(位图)
------

The qnx6fs filesystem allocation bitmap is stored in a tree under bitmap
root node in the superblock and each bit in the bitmap represents one
filesystem block.
(qnx6fs 文件系统的分配位图存储在超级块中位图根节点之下的一棵树中，位图中的每一位代表一个文件系统块。)

The first block is block 0, which starts 0x1000 after superblock start.
So for a normal qnx6fs 0x3000 (bootblock + superblock) is the physical
address at which block 0 is located.
(第一个块是块 0，它从超级块起始处之后 0x1000 的位置开始。因此，对于普通的 qnx6fs，0x3000(引导块 + 超级块)就是块 0 所在的物理地址。)

Bits at the end of the last bitmap block are set to 1, if the device is
smaller than addressing space in the bitmap.
(如果设备小于位图中的寻址空间，则最后一个位图块末尾的位会被设置为 1。)

Bitmap system area
(位图系统区域)
------------------

The bitmap itself is divided into three parts.
(位图本身被划分为三个部分。)

First the system area, that is split into two halves.
(首先是系统区域，它被分成两半。)

Then userspace.
(然后是用户空间。)

The requirement for a static, fixed preallocated system area comes from how
qnx6fs deals with writes.
(之所以需要一个静态、固定、预先分配的系统区域，源于 qnx6fs 处理写入的方式。)

Each superblock got its own half of the system area. So superblock #1
always uses blocks from the lower half while superblock #2 just writes to
blocks represented by the upper half bitmap system area bits.
(每个超级块都拥有自己那一半的系统区域。因此，超级块 #1 总是使用下半部分的块，而超级块 #2 只写入由上半部分位图系统区域位所表示的块。)

Bitmap blocks, Inode blocks and indirect addressing blocks for those two
tree structures are treated as system blocks.
(这两个树结构所用的位图块、inode 块和间接寻址块都被视为系统块。)

The rational behind that is that a write request can work on a new snapshot
(system area of the inactive - resp. lower serial numbered superblock) while
at the same time there is still a complete stable filesystem structure in the
other half of the system area.
(其背后的理由是：一个写入请求可以在新快照(非活动超级块——即序列号较小的那个超级块——的系统区域)上进行，与此同时，系统区域的另一半中仍然保留着一个完整稳定的文件系统结构。)

When finished with writing (a sync write is completed, the maximum sync leap
time or a filesystem sync is requested), serial of the previously inactive
superblock atomically is increased and the fs switches over to that - then
stable declared - superblock.
(当写入完成时(一次同步写已完成、达到最大同步跳跃时间，或者请求进行文件系统同步)，先前非活动超级块的序列号被原子性地增加，文件系统随即切换到那个——此时被声明为稳定的——超级块。)

For all data outside the system area, blocks are just copied while writing.
(对于系统区域之外的所有数据，写入时只是对块进行简单的复制。)
