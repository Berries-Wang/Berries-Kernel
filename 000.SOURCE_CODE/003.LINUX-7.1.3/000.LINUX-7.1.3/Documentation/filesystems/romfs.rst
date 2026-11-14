.. SPDX-License-Identifier: GPL-2.0

=======================
ROMFS - ROM File System
=======================

(ROMFS - ROM 文件系统)

This is a quite dumb, read only filesystem, mainly for initial RAM
disks of installation disks.  It has grown up by the need of having
modules linked at boot time.  Using this filesystem, you get a very
similar feature, and even the possibility of a small kernel, with a
file system which doesn't take up useful memory from the router
functions in the basement of your office.
(这是一个相当简单、只读的文件系统，主要用于安装盘的初始 RAM 磁盘。它的成长源于在引导时链接模块的需求。使用这个文件系统，你可以获得非常类似的特性，甚至还有小内核的可能，它所用的文件系统不会从你办公室地下室中的路由功能那里占用有用的内存。)

For comparison, both the older minix and xiafs (the latter is now
defunct) filesystems, compiled as module need more than 20000 bytes,
while romfs is less than a page, about 4000 bytes (assuming i586
code).  Under the same conditions, the msdos filesystem would need
about 30K (and does not support device nodes or symlinks), while the
nfs module with nfsroot is about 57K.  Furthermore, as a bit unfair
comparison, an actual rescue disk used up 3202 blocks with ext2, while
with romfs, it needed 3079 blocks.
(作为对比，较老的 minix 和 xiafs(后者现已停用)文件系统以模块形式编译后需要超过 20000 字节，而 romfs 不到一页，大约 4000 字节(假设是 i586 代码)。在相同的条件下，msdos 文件系统需要大约 30K(而且不支持设备节点或符号链接)，而带 nfsroot 的 nfs 模块大约 57K。此外，作为一个有点不公平的比较，一个实际的救援盘使用 ext2 时占用了 3202 个块，而使用 romfs 时只需要 3079 个块。)

To create such a file system, you'll need a user program named
genromfs. It is available on http://romfs.sourceforge.net/
(要创建这样的文件系统，你需要一个名为 genromfs 的用户程序。它可以在 http://romfs.sourceforge.net/ 上获得。)

As the name suggests, romfs could be also used (space-efficiently) on
various read-only media, like (E)EPROM disks if someone will have the
motivation.. :)
(顾名思义，romfs 也可以(高效地利用空间)用于各种只读介质，比如(E)EPROM 盘，只要有人有这个动力的话。:)

However, the main purpose of romfs is to have a very small kernel,
which has only this filesystem linked in, and then can load any module
later, with the current module utilities.  It can also be used to run
some program to decide if you need SCSI devices, and even IDE or
floppy drives can be loaded later if you use the "initrd"--initial
RAM disk--feature of the kernel.  This would not be really news
flash, but with romfs, you can even spare off your ext2 or minix or
maybe even affs filesystem until you really know that you need it.
(然而，romfs 的主要目的是拥有一个非常小的内核，它只链接了这个文件系统，然后可以使用当前的模块工具在之后加载任何模块。它还可以用来运行某个程序以决定你是否需要 SCSI 设备；如果你使用内核的 "initrd"(初始 RAM 磁盘)特性，甚至连 IDE 或软盘驱动程序都可以在之后加载。这算不上什么新闻，但有了 romfs，你甚至可以暂时省去你的 ext2 或 minix 或许还有 affs 文件系统，直到你真正确定需要它。)

For example, a distribution boot disk can contain only the cd disk
drivers (and possibly the SCSI drivers), and the ISO 9660 filesystem
module.  The kernel can be small enough, since it doesn't have other
filesystems, like the quite large ext2fs module, which can then be
loaded off the CD at a later stage of the installation.  Another use
would be for a recovery disk, when you are reinstalling a workstation
from the network, and you will have all the tools/modules available
from a nearby server, so you don't want to carry two disks for this
purpose, just because it won't fit into ext2.
(例如，发行版的引导盘可以只包含 cd 盘驱动程序(可能还有 SCSI 驱动程序)以及 ISO 9660 文件系统模块。内核可以足够小，因为它没有其他文件系统，比如相当庞大的 ext2fs 模块——后者可以在安装的后续阶段从 CD 加载。另一种用途是恢复盘：当你从网络重新安装工作站时，所有工具/模块都可以从附近的服务器获得，所以你不想为此携带两张盘，仅仅因为 ext2 装不下。)

romfs operates on block devices as you can expect, and the underlying
structure is very simple.  Every accessible structure begins on 16
byte boundaries for fast access.  The minimum space a file will take
is 32 bytes (this is an empty file, with a less than 16 character
name).  The maximum overhead for any non-empty file is the header, and
the 16 byte padding for the name and the contents, also 16+14+15 = 45
bytes.  This is quite rare however, since most file names are longer
than 3 bytes, and shorter than 15 bytes.
(正如你所期望的，romfs 运行在块设备上，其底层结构非常简单。为了快速访问，每个可访问的结构都从 16 字节边界开始。一个文件占用的最小空间是 32 字节(这是一个空的、名字少于 16 个字符的文件)。任何非空文件的最大开销是文件头，以及名称和内容各 16 字节的填充，即 16+14+15 = 45 字节。然而这种情况相当少见，因为大多数文件名长于 3 字节且短于 15 字节。)

The layout of the filesystem is the following::

 offset	    content

	+---+---+---+---+
  0	| - | r | o | m |  \
	+---+---+---+---+	The ASCII representation of those bytes
  4	| 1 | f | s | - |  /	(i.e. "-rom1fs-")
	+---+---+---+---+
  8	|   full size	|	The number of accessible bytes in this fs.
	+---+---+---+---+
 12	|    checksum	|	The checksum of the FIRST 512 BYTES.
	+---+---+---+---+
 16	| volume name	|	The zero terminated name of the volume,
	:               :	padded to 16 byte boundary.
	+---+---+---+---+
 xx	|     file	|
	:    headers	:

(文件系统的布局如下：)

Every multi byte value (32 bit words, I'll use the longwords term from
now on) must be in big endian order.
(每个多字节值(32 位字，从现在起我将使用 longwords 这个术语)都必须按大端序存储。)

The first eight bytes identify the filesystem, even for the casual
inspector.  After that, in the 3rd longword, it contains the number of
bytes accessible from the start of this filesystem.  The 4th longword
is the checksum of the first 512 bytes (or the number of bytes
accessible, whichever is smaller).  The applied algorithm is the same
as in the AFFS filesystem, namely a simple sum of the longwords
(assuming bigendian quantities again).  For details, please consult
the source.  This algorithm was chosen because although it's not quite
reliable, it does not require any tables, and it is very simple.
(最前面的八个字节用于标识文件系统，即使是随便看看的人也能识别。在此之后，第 3 个 longword 包含从该文件系统起始处开始可访问的字节数。第 4 个 longword 是前 512 字节(或可访问的字节数，取两者中较小者)的校验和。所采用的算法与 AFFS 文件系统中的相同，即对 longword 做简单求和(同样假设是大端数量)。详情请查阅源代码。之所以选择这个算法，是因为虽然它不太可靠，但它不需要任何表，而且非常简单。)

The following bytes are now part of the file system; each file header
must begin on a 16 byte boundary::

 offset	    content

     	+---+---+---+---+
  0	| next filehdr|X|	The offset of the next file header
	+---+---+---+---+	  (zero if no more files)
  4	|   spec.info	|	Info for directories/hard links/devices
	+---+---+---+---+
  8	|     size      |	The size of this file in bytes
	+---+---+---+---+
 12	|   checksum	|	Covering the meta data, including the file
	+---+---+---+---+	  name, and padding
 16	| file name     |	The zero terminated name of the file,
	:               :	padded to 16 byte boundary
	+---+---+---+---+
 xx	| file data	|
	:		:

(现在接下来的字节就属于文件系统了；每个文件头都必须从 16 字节边界开始：)

Since the file headers begin always at a 16 byte boundary, the lowest
4 bits would be always zero in the next filehdr pointer.  These four
bits are used for the mode information.  Bits 0..2 specify the type of
the file; while bit 4 shows if the file is executable or not.  The
permissions are assumed to be world readable, if this bit is not set,
and world executable if it is; except the character and block devices,
they are never accessible for other than owner.  The owner of every
file is user and group 0, this should never be a problem for the
intended use.  The mapping of the 8 possible values to file types is
the following:
(由于文件头总是从 16 字节边界开始，next filehdr 指针的最低 4 位将始终为零。这四个位被用作模式信息。位 0..2 指定文件的类型；而位 4 显示文件是否可执行。如果该位未设置，则假定权限为所有人可读；如果设置了，则为所有人可执行；但字符设备和块设备除外——它们对除所有者以外的任何人都不可访问。每个文件的所有者都是用户和属组 0，对于预期用途来说这应该不会成为问题。8 个可能的取值到文件类型的映射如下：)

==	=============== ============================================
	  mapping		spec.info means (映射		spec.info 的含义)
==	=============== ============================================
 0	hard link	link destination [file header] (硬链接	链接目标 [文件头])
 1	directory	first file's header (目录	第一个文件的文件头)
 2	regular file	unused, must be zero [MBZ] (常规文件	未使用，必须为零 [MBZ])
 3	symbolic link	unused, MBZ (file data is the link content) (符号链接	未使用，必须为零(文件数据即链接内容))
 4	block device	16/16 bits major/minor number (块设备	16/16 位主设备号/次设备号)
 5	char device		    - " - (字符设备	16/16 位主设备号/次设备号)
 6	socket		unused, MBZ (套接字	未使用，必须为零)
 7	fifo		unused, MBZ (fifo	未使用，必须为零)
==	=============== ============================================

Note that hard links are specifically marked in this filesystem, but
they will behave as you can expect (i.e. share the inode number).
Note also that it is your responsibility to not create hard link
loops, and creating all the . and .. links for directories.  This is
normally done correctly by the genromfs program.  Please refrain from
using the executable bits for special purposes on the socket and fifo
special files, they may have other uses in the future.  Additionally,
please remember that only regular files, and symlinks are supposed to
have a nonzero size field; they contain the number of bytes available
directly after the (padded) file name.
(注意，硬链接在这个文件系统中被专门标记出来，但它们的行为如你所期望的那样(即共享 inode 编号)。还要注意，不创建硬链接循环，以及为目录创建所有 . 和 .. 链接，这是你的责任。这通常由 genromfs 程序正确完成。请勿在套接字和 fifo 特殊文件上出于特殊目的使用可执行位，它们在未来可能有其他用途。另外，请记住只有常规文件和符号链接才应该有非零的 size 字段；它们包含紧跟在(填充后的)文件名之后的可用字节数。)

Another thing to note is that romfs works on file headers and data
aligned to 16 byte boundaries, but most hardware devices and the block
device drivers are unable to cope with smaller than block-sized data.
To overcome this limitation, the whole size of the file system must be
padded to an 1024 byte boundary.
(另一件需要注意的事情是，romfs 处理的是按 16 字节边界对齐的文件头和数据，但大多数硬件设备和块设备驱动程序无法处理小于块大小的数据。为了克服这一限制，文件系统的整体大小必须填充到 1024 字节边界。)

If you have any problems or suggestions concerning this file system,
please contact me.  However, think twice before wanting me to add
features and code, because the primary and most important advantage of
this file system is the small code.  On the other hand, don't be
alarmed, I'm not getting that much romfs related mail.  Now I can
understand why Avery wrote poems in the ARCnet docs to get some more
feedback. :)
(如果你对这个文件系统有任何问题或建议，请联系我。不过，在希望我添加功能和代码之前请三思，因为这个文件系统首要且最重要的优点就是代码量小。另一方面，不必担心，我并没有收到那么多与 romfs 相关的邮件。现在我就能理解为什么 Avery 要在 ARCnet 文档中写诗来获取更多反馈了。:)

romfs has also a mailing list, and to date, it hasn't received any
traffic, so you are welcome to join it to discuss your ideas. :)
(romfs 还有一个邮件列表，到目前为止它还没有收到任何流量，欢迎你加入它来讨论你的想法。:)

It's run by ezmlm, so you can subscribe to it by sending a message
to romfs-subscribe@shadow.banki.hu, the content is irrelevant.
(它由 ezmlm 运行，你可以通过发送一条消息到 romfs-subscribe@shadow.banki.hu 来订阅，内容无关紧要。)

Pending issues:
(悬而未决的问题：)

- Permissions and owner information are pretty essential features of a
  Un*x like system, but romfs does not provide the full possibilities.
  I have never found this limiting, but others might.
  (权限和所有者信息是类 Un*x 系统相当重要的特性，但 romfs 并不提供全部的可能性。我从未觉得这是个限制，但其他人可能会。)

- The file system is read only, so it can be very small, but in case
  one would want to write _anything_ to a file system, he still needs
  a writable file system, thus negating the size advantages.  Possible
  solutions: implement write access as a compile-time option, or a new,
  similarly small writable filesystem for RAM disks.
  (这个文件系统是只读的，因此它可以非常小，但如果有人想向文件系统写入_任何_内容，他仍然需要一个可写的文件系统，这就抵消了体积上的优势。可能的解决方案：把写访问实现为一个编译时选项，或者为 RAM 磁盘提供一个同样小巧的可写文件系统。)

- Since the files are only required to have alignment on a 16 byte
  boundary, it is currently possibly suboptimal to read or execute files
  from the filesystem.  It might be resolved by reordering file data to
  have most of it (i.e. except the start and the end) laying at "natural"
  boundaries, thus it would be possible to directly map a big portion of
  the file contents to the mm subsystem.
  (由于文件只要求按 16 字节边界对齐，目前从文件系统中读取或执行文件可能不是最优的。可以通过重新排列文件数据来解决这个问题，使其大部分(即除开头和结尾之外)位于"自然"边界上，这样就可以把文件内容的大部分直接映射到 mm 子系统。)

- Compression might be an useful feature, but memory is quite a
  limiting factor in my eyes.
  (压缩可能是一个有用的特性，但在我看来内存是一个相当大的限制因素。)

- Where it is used?
  (它在哪里被使用？)

- Does it work on other architectures than intel and motorola?
  (它在 intel 和 motorola 之外的其他架构上能工作吗？)


Have fun,
(祝愉快，)

Janos Farkas <chexum@shadow.banki.hu>
(Janos Farkas <chexum@shadow.banki.hu>)
