.. SPDX-License-Identifier: GPL-2.0
.. _xfs_self_describing_metadata:

============================
XFS Self Describing Metadata
============================

(XFS 自描述元数据)

Introduction
(引言)
============

The largest scalability problem facing XFS is not one of algorithmic
scalability, but of verification of the filesystem structure. Scalabilty of the
structures and indexes on disk and the algorithms for iterating them are
adequate for supporting PB scale filesystems with billions of inodes, however it
is this very scalability that causes the verification problem.
(XFS 面临的最大可扩展性问题不是算法的可扩展性问题，而是文件系统结构的验证问题。磁盘上的结构、索引以及遍历它们的算法足以支持拥有数十亿 inode 的 PB 级文件系统，然而正是这种可扩展性导致了验证问题。)

Almost all metadata on XFS is dynamically allocated. The only fixed location
metadata is the allocation group headers (SB, AGF, AGFL and AGI), while all
other metadata structures need to be discovered by walking the filesystem
structure in different ways. While this is already done by userspace tools for
validating and repairing the structure, there are limits to what they can
verify, and this in turn limits the supportable size of an XFS filesystem.
(XFS 上几乎所有的元数据都是动态分配的。仅有固定位置的元数据是分配组头部(SB、AGF、AGFL 和 AGI)，而所有其他元数据结构都需要通过以不同方式遍历文件系统结构来发现。虽然用户空间工具在验证和修复结构时已经这样做了，但它们能够验证的内容是有限的，这反过来又限制了 XFS 文件系统可支持的容量。)

For example, it is entirely possible to manually use xfs_db and a bit of
scripting to analyse the structure of a 100TB filesystem when trying to
determine the root cause of a corruption problem, but it is still mainly a
manual task of verifying that things like single bit errors or misplaced writes
weren't the ultimate cause of a corruption event. It may take a few hours to a
few days to perform such forensic analysis, so for at this scale root cause
analysis is entirely possible.
(例如，在试图确定损坏问题的根本原因时，完全可以手动使用 xfs_db 加上一点脚本来分析 100TB 文件系统的结构，但这仍然主要是一项人工任务，即验证诸如单比特错误或错位写入之类的因素并非损坏事件的最终原因。执行这样的取证分析可能需要几个小时到几天，所以在这一规模下根本原因分析是完全可能的。)

However, if we scale the filesystem up to 1PB, we now have 10x as much metadata
to analyse and so that analysis blows out towards weeks/months of forensic work.
Most of the analysis work is slow and tedious, so as the amount of analysis goes
up, the more likely that the cause will be lost in the noise.  Hence the primary
concern for supporting PB scale filesystems is minimising the time and effort
required for basic forensic analysis of the filesystem structure.
(然而，如果我们将文件系统扩展到 1PB，现在就有 10 倍的元数据需要分析，于是分析工作会膨胀到数周/数月的取证工作。大部分分析工作缓慢而乏味，因此分析量越大，原因就越有可能淹没在噪声之中。所以，支持 PB 级文件系统的首要关注点是尽量减少对文件系统结构进行基本取证分析所需的时间和精力。)


Self Describing Metadata
(自描述元数据)
========================

One of the problems with the current metadata format is that apart from the
magic number in the metadata block, we have no other way of identifying what it
is supposed to be. We can't even identify if it is the right place. Put simply,
you can't look at a single metadata block in isolation and say "yes, it is
supposed to be there and the contents are valid".
(当前元数据格式的问题之一是，除了元数据块中的魔数之外，我们没有其他办法识别它本应是什么。我们甚至无法识别它是否位于正确的位置。简而言之，我们无法孤立地查看单个元数据块并说"是的，它本应在那里并且内容有效"。)

Hence most of the time spent on forensic analysis is spent doing basic
verification of metadata values, looking for values that are in range (and hence
not detected by automated verification checks) but are not correct. Finding and
understanding how things like cross linked block lists (e.g. sibling
pointers in a btree end up with loops in them) are the key to understanding what
went wrong, but it is impossible to tell what order the blocks were linked into
each other or written to disk after the fact.
(因此，取证分析所花费的大部分时间都用于对元数据值做基本验证，寻找那些在范围之内(因而未被自动验证检查发现)但不正确的值。找出并理解诸如交叉链接的块列表(例如 btree 中的兄弟指针最终形成环)之类的情况是如何发生的，是理解问题出在哪里的关键，但事后已经无法分辨各个块以何种顺序相互链接、以何种顺序写入磁盘。)

Hence we need to record more information into the metadata to allow us to
quickly determine if the metadata is intact and can be ignored for the purpose
of analysis. We can't protect against every possible type of error, but we can
ensure that common types of errors are easily detectable.  Hence the concept of
self describing metadata.
(因此，我们需要在元数据中记录更多信息，使我们能够快速判断元数据是否完好，从而在分析时将其忽略。我们无法防范每一种可能类型的错误，但可以确保常见类型的错误易于检测。这就是自描述元数据概念的由来。)

The first, fundamental requirement of self describing metadata is that the
metadata object contains some form of unique identifier in a well known
location. This allows us to identify the expected contents of the block and
hence parse and verify the metadata object. IF we can't independently identify
the type of metadata in the object, then the metadata doesn't describe itself
very well at all!
(自描述元数据的第一个基本要求是，元数据对象在一个众所周知的位置包含某种形式的唯一标识符。这使我们能够识别块的预期内容，进而解析并验证元数据对象。如果我们无法独立识别对象中元数据的类型，那么这些元数据就根本没有很好地描述它自己！)

Luckily, almost all XFS metadata has magic numbers embedded already - only the
AGFL, remote symlinks and remote attribute blocks do not contain identifying
magic numbers. Hence we can change the on-disk format of all these objects to
add more identifying information and detect this simply by changing the magic
numbers in the metadata objects. That is, if it has the current magic number,
the metadata isn't self identifying. If it contains a new magic number, it is
self identifying and we can do much more expansive automated verification of the
metadata object at runtime, during forensic analysis or repair.
(幸运的是，几乎所有 XFS 元数据都已经嵌入了魔数——只有 AGFL、远程符号链接和远程属性块不包含标识性魔数。因此，我们可以更改所有这些对象的磁盘格式以加入更多标识信息，并且只需通过更改元数据对象中的魔数就能检测到这一点。也就是说，如果它带有当前的魔数，元数据就不是自标识的；如果它包含新的魔数，它就是自标识的，我们就可以在运行时、取证分析或修复期间对元数据对象进行广泛得多的自动验证。)

As a primary concern, self describing metadata needs some form of overall
integrity checking. We cannot trust the metadata if we cannot verify that it has
not been changed as a result of external influences. Hence we need some form of
integrity check, and this is done by adding CRC32c validation to the metadata
block. If we can verify the block contains the metadata it was intended to
contain, a large amount of the manual verification work can be skipped.
(作为首要关注点，自描述元数据需要某种形式的整体完整性检查。如果我们无法验证元数据没有因外部影响而被更改，我们就不能信任它。因此我们需要某种形式的完整性检查，其实现方式是向元数据块添加 CRC32c 校验。如果我们能验证块中包含的正是它本应包含的元数据，就可以跳过大量的人工验证工作。)

CRC32c was selected as metadata cannot be more than 64k in length in XFS and
hence a 32 bit CRC is more than sufficient to detect multi-bit errors in
metadata blocks. CRC32c is also now hardware accelerated on common CPUs so it is
fast. So while CRC32c is not the strongest of possible integrity checks that
could be used, it is more than sufficient for our needs and has relatively
little overhead. Adding support for larger integrity fields and/or algorithms
does really provide any extra value over CRC32c, but it does add a lot of
complexity and so there is no provision for changing the integrity checking
mechanism.
(之所以选择 CRC32c，是因为在 XFS 中元数据的长度不能超过 64k，因此 32 位 CRC 足以检测元数据块中的多比特错误。CRC32c 现在在常见 CPU 上还有硬件加速，因此速度很快。所以，虽然 CRC32c 并不是可用的完整性检查中最强的一种，但对于我们的需求而言它绰绰有余，且开销相对较小。增加对更大完整性字段和/或算法的支持并不会比 CRC32c 带来任何额外价值，却确实会增加很多复杂性，因此没有提供更改完整性检查机制的选项。)

Self describing metadata needs to contain enough information so that the
metadata block can be verified as being in the correct place without needing to
look at any other metadata. This means it needs to contain location information.
Just adding a block number to the metadata is not sufficient to protect against
mis-directed writes - a write might be misdirected to the wrong LUN and so be
written to the "correct block" of the wrong filesystem. Hence location
information must contain a filesystem identifier as well as a block number.
(自描述元数据需要包含足够的信息，使得元数据块无需查看任何其他元数据就能被验证确实位于正确的位置。这意味着它需要包含位置信息。仅仅在元数据中加入块号不足以防范错向写入——一次写入可能被错误地定向到错误的 LUN，从而写到错误文件系统的"正确块"上。因此，位置信息必须同时包含文件系统标识符和块号。)

Another key information point in forensic analysis is knowing who the metadata
block belongs to. We already know the type, the location, that it is valid
and/or corrupted, and how long ago that it was last modified. Knowing the owner
of the block is important as it allows us to find other related metadata to
determine the scope of the corruption. For example, if we have a extent btree
object, we don't know what inode it belongs to and hence have to walk the entire
filesystem to find the owner of the block. Worse, the corruption could mean that
no owner can be found (i.e. it's an orphan block), and so without an owner field
in the metadata we have no idea of the scope of the corruption. If we have an
owner field in the metadata object, we can immediately do top down validation to
determine the scope of the problem.
(取证分析的另一个关键信息点是弄清元数据块属于谁。我们已经知道类型、位置、它是否有效和/或已损坏，以及它最后一次被修改是在多久之前。知道块的所有者很重要，因为它使我们能够找到其他相关元数据，从而确定损坏的范围。例如，如果我们拿到一个 extent btree 对象，却不知道它属于哪个 inode，就必须遍历整个文件系统来找到该块的所有者。更糟的是，损坏可能意味着找不到任何所有者(即它是一个孤儿块)，因此如果元数据中没有所有者字段，我们就完全无法了解损坏的范围。如果元数据对象中有所有者字段，我们就可以立即进行自顶向下的验证来确定问题的范围。)

Different types of metadata have different owner identifiers. For example,
directory, attribute and extent tree blocks are all owned by an inode, while
freespace btree blocks are owned by an allocation group. Hence the size and
contents of the owner field are determined by the type of metadata object we are
looking at.  The owner information can also identify misplaced writes (e.g.
freespace btree block written to the wrong AG).
(不同类型的元数据有不同的所有者标识符。例如，目录、属性和 extent 树块都由一个 inode 拥有，而空闲空间 btree 块由一个分配组拥有。因此，所有者字段的大小和内容由我们正在查看的元数据对象的类型决定。所有者信息还可以识别错位写入(例如空闲空间 btree 块被写到了错误的 AG)。)

Self describing metadata also needs to contain some indication of when it was
written to the filesystem. One of the key information points when doing forensic
analysis is how recently the block was modified. Correlation of set of corrupted
metadata blocks based on modification times is important as it can indicate
whether the corruptions are related, whether there's been multiple corruption
events that lead to the eventual failure, and even whether there are corruptions
present that the run-time verification is not detecting.
(自描述元数据还需要包含某种关于它何时被写入文件系统的指示。做取证分析时的关键信息点之一是块最近一次被修改的时间。基于修改时间对一组损坏的元数据块进行关联分析非常重要，因为它可以表明这些损坏是否相互关联、是否发生了多个导致最终失败的损坏事件，甚至可以表明是否存在运行时验证未能检测到的损坏。)

For example, we can determine whether a metadata object is supposed to be free
space or still allocated if it is still referenced by its owner by looking at
when the free space btree block that contains the block was last written
compared to when the metadata object itself was last written.  If the free space
block is more recent than the object and the object's owner, then there is a
very good chance that the block should have been removed from the owner.
(例如，如果一个元数据对象仍被其所有者引用，我们可以通过比较包含该块的空闲空间 btree 块最后一次被写入的时间与元数据对象本身最后一次被写入的时间，来判断该元数据对象本应是空闲空间还是仍处于已分配状态。如果空闲空间块比该对象及其所有者都要新，那么很有可能该块本应已从其所有者中移除。)

To provide this "written timestamp", each metadata block gets the Log Sequence
Number (LSN) of the most recent transaction it was modified on written into it.
This number will always increase over the life of the filesystem, and the only
thing that resets it is running xfs_repair on the filesystem. Further, by use of
the LSN we can tell if the corrupted metadata all belonged to the same log
checkpoint and hence have some idea of how much modification occurred between
the first and last instance of corrupt metadata on disk and, further, how much
modification occurred between the corruption being written and when it was
detected.
(为了提供这种"写入时间戳"，每个元数据块中都会写入其最近一次被修改所在事务的日志序列号(LSN)。这个数字在文件系统的生命周期内会一直递增，唯一会将其重置的操作是在文件系统上运行 xfs_repair。此外，借助 LSN，我们可以判断损坏的元数据是否都属于同一个日志检查点(log checkpoint)，从而对磁盘上第一个和最后一个损坏元数据实例之间发生了多少修改有所了解，并进一步了解从损坏被写入到被发现之间发生了多少修改。)

Runtime Validation
(运行时验证)
==================

Validation of self-describing metadata takes place at runtime in two places:
(自描述元数据的验证在运行时发生在两个地方：)

	- immediately after a successful read from disk
	  (成功从磁盘读取之后立即进行)
	- immediately prior to write IO submission
	  (在提交写 IO 之前立即进行)

The verification is completely stateless - it is done independently of the
modification process, and seeks only to check that the metadata is what it says
it is and that the metadata fields are within bounds and internally consistent.
As such, we cannot catch all types of corruption that can occur within a block
as there may be certain limitations that operational state enforces of the
metadata, or there may be corruption of interblock relationships (e.g. corrupted
sibling pointer lists). Hence we still need stateful checking in the main code
body, but in general most of the per-field validation is handled by the
verifiers.
(这种验证完全是无状态的——它独立于修改过程进行，只检查元数据是否与其声称的一致，以及元数据字段是否在边界之内且内部一致。因此，我们无法捕获块内可能发生的所有类型的损坏，因为运行状态可能对元数据施加某些限制，或者块间关系可能已损坏(例如损坏的兄弟指针列表)。所以我们仍然需要在主代码体中进行有状态的检查，但总体而言，大部分逐字段的验证都由验证器处理。)

For read verification, the caller needs to specify the expected type of metadata
that it should see, and the IO completion process verifies that the metadata
object matches what was expected. If the verification process fails, then it
marks the object being read as EFSCORRUPTED. The caller needs to catch this
error (same as for IO errors), and if it needs to take special action due to a
verification error it can do so by catching the EFSCORRUPTED error value. If we
need more discrimination of error type at higher levels, we can define new
error numbers for different errors as necessary.
(对于读验证，调用者需要指定它预期看到的元数据类型，而 IO 完成过程会验证元数据对象与预期是否相符。如果验证过程失败，被读取的对象将被标记为 EFSCORRUPTED。调用者需要捕获这个错误(与 IO 错误一样)，如果它需要因验证错误而采取特殊操作，可以通过捕获 EFSCORRUPTED 错误值来实现。如果我们需要在更高层次上进一步区分错误类型，可以根据需要为不同的错误定义新的错误编号。)

The first step in read verification is checking the magic number and determining
whether CRC validating is necessary. If it is, the CRC32c is calculated and
compared against the value stored in the object itself. Once this is validated,
further checks are made against the location information, followed by extensive
object specific metadata validation. If any of these checks fail, then the
buffer is considered corrupt and the EFSCORRUPTED error is set appropriately.
(读验证的第一步是检查魔数并确定是否需要进行 CRC 校验。如果需要，就计算 CRC32c 并与对象自身存储的值进行比较。验证通过后，再对位置信息做进一步检查，随后进行广泛的对象特定元数据验证。如果其中任何一项检查失败，该缓冲就被视为已损坏，并相应地设置 EFSCORRUPTED 错误。)

Write verification is the opposite of the read verification - first the object
is extensively verified and if it is OK we then update the LSN from the last
modification made to the object, After this, we calculate the CRC and insert it
into the object. Once this is done the write IO is allowed to continue. If any
error occurs during this process, the buffer is again marked with a EFSCORRUPTED
error for the higher layers to catch.
(写验证与读验证相反——首先对对象进行广泛验证，如果没有问题，我们随后根据对对象的最后一次修改更新 LSN。之后，我们计算 CRC 并将其插入对象。完成这些之后，写 IO 才被允许继续。如果在此过程中发生任何错误，该缓冲会再次被标记上 EFSCORRUPTED 错误，供更高层捕获。)

Structures
(结构)
==========

A typical on-disk structure needs to contain the following information::
(一个典型的磁盘上(on-disk)结构需要包含以下信息：)

    struct xfs_ondisk_hdr {
	    __be32  magic;		/* magic number */
	    __be32  crc;		/* CRC, not logged */
	    uuid_t  uuid;		/* filesystem identifier */
	    __be64  owner;		/* parent object */
	    __be64  blkno;		/* location on disk */
	    __be64  lsn;		/* last modification in log, not logged */
    };

Depending on the metadata, this information may be part of a header structure
separate to the metadata contents, or may be distributed through an existing
structure. The latter occurs with metadata that already contains some of this
information, such as the superblock and AG headers.
(根据元数据的不同，这些信息可能是与元数据内容分离的头部结构的一部分，也可能分布在某个现有结构之中。后者出现在已经包含其中部分信息的元数据上，例如超级块和 AG 头部。)

Other metadata may have different formats for the information, but the same
level of information is generally provided. For example:
(其他元数据的信息可能有不同的格式，但通常提供的信息层次是相同的。例如：)

	- short btree blocks have a 32 bit owner (ag number) and a 32 bit block
	  number for location. The two of these combined provide the same
	  information as @owner and @blkno in eh above structure, but using 8
	  bytes less space on disk.
	  (短 btree 块拥有一个 32 位所有者(ag 号)和一个 32 位块号用于定位。两者结合起来提供了与上面结构中 @owner 和 @blkno 相同的信息，但在磁盘上少占用 8 个字节的空间。)

	- directory/attribute node blocks have a 16 bit magic number, and the
	  header that contains the magic number has other information in it as
	  well. hence the additional metadata headers change the overall format
	  of the metadata.
	  (目录/属性节点块拥有一个 16 位魔数，而包含魔数的头部中还有其他信息。因此，额外的元数据头部改变了元数据的整体格式。)

A typical buffer read verifier is structured as follows::
(一个典型的缓冲读验证器的结构如下：)

    #define XFS_FOO_CRC_OFF		offsetof(struct xfs_ondisk_hdr, crc)

    static void
    xfs_foo_read_verify(
	    struct xfs_buf	*bp)
    {
	struct xfs_mount *mp = bp->b_mount;

	    if ((xfs_sb_version_hascrc(&mp->m_sb) &&
		!xfs_verify_cksum(bp->b_addr, BBTOB(bp->b_length),
					    XFS_FOO_CRC_OFF)) ||
		!xfs_foo_verify(bp)) {
		    XFS_CORRUPTION_ERROR(__func__, XFS_ERRLEVEL_LOW, mp, bp->b_addr);
		    xfs_buf_ioerror(bp, EFSCORRUPTED);
	    }
    }

The code ensures that the CRC is only checked if the filesystem has CRCs enabled
by checking the superblock of the feature bit, and then if the CRC verifies OK
(or is not needed) it verifies the actual contents of the block.
(这段代码通过检查超级块中的特性位，确保只有在文件系统启用 CRC 的情况下才会检查 CRC；然后，如果 CRC 验证通过(或不需要 CRC)，它就会验证块的实际内容。)

The verifier function will take a couple of different forms, depending on
whether the magic number can be used to determine the format of the block. In
the case it can't, the code is structured as follows::
(验证器函数会有几种不同的形式，取决于能否使用魔数来确定块的格式。在无法使用魔数的情况下，代码结构如下：)

    static bool
    xfs_foo_verify(
	    struct xfs_buf		*bp)
    {
	    struct xfs_mount	*mp = bp->b_mount;
	    struct xfs_ondisk_hdr	*hdr = bp->b_addr;

	    if (hdr->magic != cpu_to_be32(XFS_FOO_MAGIC))
		    return false;

	    if (!xfs_sb_version_hascrc(&mp->m_sb)) {
		    if (!uuid_equal(&hdr->uuid, &mp->m_sb.sb_uuid))
			    return false;
		    if (bp->b_bn != be64_to_cpu(hdr->blkno))
			    return false;
		    if (hdr->owner == 0)
			    return false;
	    }

	    /* object specific verification checks here */

	    return true;
    }

If there are different magic numbers for the different formats, the verifier
will look like::
(如果不同格式有不同的魔数，验证器将如下所示：)

    static bool
    xfs_foo_verify(
	    struct xfs_buf		*bp)
    {
	    struct xfs_mount	*mp = bp->b_mount;
	    struct xfs_ondisk_hdr	*hdr = bp->b_addr;

	    if (hdr->magic == cpu_to_be32(XFS_FOO_CRC_MAGIC)) {
		    if (!uuid_equal(&hdr->uuid, &mp->m_sb.sb_uuid))
			    return false;
		    if (bp->b_bn != be64_to_cpu(hdr->blkno))
			    return false;
		    if (hdr->owner == 0)
			    return false;
	    } else if (hdr->magic != cpu_to_be32(XFS_FOO_MAGIC))
		    return false;

	    /* object specific verification checks here */

	    return true;
    }

Write verifiers are very similar to the read verifiers, they just do things in
the opposite order to the read verifiers. A typical write verifier::
(写验证器与读验证器非常相似，只是执行顺序与读验证器相反。一个典型的写验证器：)

    static void
    xfs_foo_write_verify(
	    struct xfs_buf	*bp)
    {
	    struct xfs_mount	*mp = bp->b_mount;
	    struct xfs_buf_log_item	*bip = bp->b_fspriv;

	    if (!xfs_foo_verify(bp)) {
		    XFS_CORRUPTION_ERROR(__func__, XFS_ERRLEVEL_LOW, mp, bp->b_addr);
		    xfs_buf_ioerror(bp, EFSCORRUPTED);
		    return;
	    }

	    if (!xfs_sb_version_hascrc(&mp->m_sb))
		    return;


	    if (bip) {
		    struct xfs_ondisk_hdr	*hdr = bp->b_addr;
		    hdr->lsn = cpu_to_be64(bip->bli_item.li_lsn);
	    }
	    xfs_update_cksum(bp->b_addr, BBTOB(bp->b_length), XFS_FOO_CRC_OFF);
    }

This will verify the internal structure of the metadata before we go any
further, detecting corruptions that have occurred as the metadata has been
modified in memory. If the metadata verifies OK, and CRCs are enabled, we then
update the LSN field (when it was last modified) and calculate the CRC on the
metadata. Once this is done, we can issue the IO.
(这会在我们进一步操作之前验证元数据的内部结构，检测元数据在内存中被修改期间发生的损坏。如果元数据验证通过且 CRC 已启用，我们随后更新 LSN 字段(即最后一次修改的时间)并计算元数据的 CRC。完成之后，我们就可以发出 IO。)

Inodes and Dquots
(inode 和 dquot)
=================

Inodes and dquots are special snowflakes. They have per-object CRC and
self-identifiers, but they are packed so that there are multiple objects per
buffer. Hence we do not use per-buffer verifiers to do the work of per-object
verification and CRC calculations. The per-buffer verifiers simply perform basic
identification of the buffer - that they contain inodes or dquots, and that
there are magic numbers in all the expected spots. All further CRC and
verification checks are done when each inode is read from or written back to the
buffer.
(inode 和 dquot 是特殊的存在。它们具有每对象的 CRC 和自标识符，但它们被打包在一起，使得每个缓冲中有多个对象。因此，我们不使用每缓冲验证器来做每对象验证和 CRC 计算的工作。每缓冲验证器只对缓冲做基本的识别——确认其中包含 inode 或 dquot，并且在所有预期的位置上都有魔数。所有进一步的 CRC 和验证检查都在每个 inode 从缓冲读出或回写到缓冲时进行。)

The structure of the verifiers and the identifiers checks is very similar to the
buffer code described above. The only difference is where they are called. For
example, inode read verification is done in xfs_inode_from_disk() when the inode
is first read out of the buffer and the struct xfs_inode is instantiated. The
inode is already extensively verified during writeback in xfs_iflush_int, so the
only addition here is to add the LSN and CRC to the inode as it is copied back
into the buffer.
(这些验证器和标识符检查的结构与上面描述的缓冲代码非常相似。唯一的区别在于它们被调用的位置。例如，inode 读验证是在 xfs_inode_from_disk() 中进行的，即在 inode 第一次从缓冲中读出并实例化 struct xfs_inode 的时候。inode 在回写期间已经在 xfs_iflush_int 中得到了广泛验证，因此这里唯一的补充是在 inode 被拷贝回缓冲时为其添加 LSN 和 CRC。)

XXX: inode unlinked list modification doesn't recalculate the inode CRC! None of
the unlinked list modifications check or update CRCs, neither during unlink nor
log recovery. So, it's gone unnoticed until now. This won't matter immediately -
repair will probably complain about it - but it needs to be fixed.
(XXX: inode 未链接链表的修改不会重新计算 inode 的 CRC！所有未链接链表的修改都不检查也不更新 CRC，无论是在执行 unlink 时还是在日志恢复期间。所以，这个问题直到现在都未被注意到。这不会立即造成影响——repair 可能会对此发出抱怨——但它需要被修复。)
