.. SPDX-License-Identifier: GPL-2.0

Journal (jbd2)
(日志 (jbd2))
--------------

Introduced in ext3, the ext4 filesystem employs a journal to protect the
filesystem against metadata inconsistencies in the case of a system crash. Up
to 10,240,000 file system blocks (see man mke2fs(8) for more details on journal
size limits) can be reserved inside the filesystem as a place to land
“important” data writes on-disk as quickly as possible. Once the important
data transaction is fully written to the disk and flushed from the disk write
cache, a record of the data being committed is also written to the journal. At
some later point in time, the journal code writes the transactions to their
final locations on disk (this could involve a lot of seeking or a lot of small
read-write-erases) before erasing the commit record. Should the system
crash during the second slow write, the journal can be replayed all the
way to the latest commit record, guaranteeing the atomicity of whatever
gets written through the journal to the disk. The effect of this is to
guarantee that the filesystem does not become stuck midway through a
metadata update.
(自 ext3 引入以来，ext4 文件系统就采用日志来保护文件系统，防止在系统崩溃的情况下出现元数据不一致。文件系统内部最多可以保留 10,240,000 个文件系统块(关于日志大小限制的更多细节请参见 man mke2fs(8))，作为一个让“重要”数据写入尽快落盘的地方。一旦重要数据事务被完整写入磁盘并从磁盘写缓存中刷出，一条表明该数据已提交的记录也会被写入日志。在之后的某个时间点，日志代码会将事务写到它们在磁盘上的最终位置(这可能涉及大量寻道或大量小的读-写-擦除操作)，然后才擦除提交记录。如果系统在第二次慢速写入期间崩溃，日志可以一直重放到最新的提交记录，从而保证通过日志写入磁盘的任何内容的原子性。这样做的效果是保证文件系统不会在元数据更新的中途卡住。)

For performance reasons, ext4 by default only writes filesystem metadata
through the journal. This means that file data blocks are /not/
guaranteed to be in any consistent state after a crash. If this default
guarantee level (``data=ordered``) is not satisfactory, there is a mount
option to control journal behavior. If ``data=journal``, all data and
metadata are written to disk through the journal. This is slower but
safest. If ``data=writeback``, dirty data blocks are not flushed to the
disk before the metadata are written to disk through the journal.
(出于性能考虑,ext4 默认只将文件系统的元数据写入日志。这意味着在系统崩溃后,文件的数据块无法保证处于一致的状态。)
(如果这种默认的保证级别（即 data=ordered 模式）无法满足需求,可以通过挂载选项来调整日志的行为：)
(如果设置为 data=journal,所有的数据和元数据都会通过日志写入磁盘。这种方式速度较慢,但安全性最高。) 
(如果设置为 data=writeback,在元数据通过日志写入磁盘之前,不会强制将脏数据块刷写到磁盘中。)

In case of ``data=ordered`` mode, Ext4 also supports fast commits which
help reduce commit latency significantly. The default ``data=ordered``
mode works by logging metadata blocks to the journal. In fast commit
mode, Ext4 only stores the minimal delta needed to recreate the
affected metadata in fast commit space that is shared with JBD2.
Once the fast commit area fills in or if fast commit is not possible
or if JBD2 commit timer goes off, Ext4 performs a traditional full commit.
A full commit invalidates all the fast commits that happened before
it and thus it makes the fast commit area empty for further fast
commits. This feature needs to be enabled at mkfs time.
(在 ``data=ordered`` 模式下，Ext4 还支持快速提交，这有助于显著降低提交延迟。默认的 ``data=ordered`` 模式的工作方式是将元数据块记录到日志中。在快速提交模式下，Ext4 只在与 JBD2 共享的快速提交空间中存储重建受影响元数据所需的最小增量。一旦快速提交区域被填满、或者无法进行快速提交、或者 JBD2 提交定时器到期，Ext4 就会执行一次传统的完整提交。完整提交会使在它之前发生的所有快速提交失效，从而使快速提交区域变空，供后续的快速提交使用。此特性需要在 mkfs 时启用。)

The journal inode is typically inode 8. The first 68 bytes of the
journal inode are replicated in the ext4 superblock. The journal itself
is normal (but hidden) file within the filesystem. The file usually
consumes an entire block group, though mke2fs tries to put it in the
middle of the disk.
(日志 inode 通常是 8 号 inode。日志 inode 的前 68 个字节被复制到 ext4 超级块中。日志本身是文件系统内的一个普通(但隐藏)的文件。该文件通常占用整个块组，不过 mke2fs 会尽量把它放在磁盘的中部。)

All fields in jbd2 are written to disk in big-endian order. This is the
opposite of ext4.
(jbd2 中的所有字段都以大端序写入磁盘。这与 ext4 正好相反。)

NOTE: Both ext4 and ocfs2 use jbd2.
(注意：ext4 和 ocfs2 都使用 jbd2。)

The maximum size of a journal embedded in an ext4 filesystem is 2^32
blocks. jbd2 itself does not seem to care.
(内嵌于 ext4 文件系统中的日志的最大大小是 2^32 个块。jbd2 本身似乎并不关心这一点。)

Layout
(布局)
~~~~~~

Generally speaking, the journal has this format:
(一般来说，日志具有如下格式：)

.. list-table::
   :widths: 16 48 16
   :header-rows: 1

   * - Superblock
       (超级块)
     - descriptor_block (data_blocks or revocation_block) [more data or
       revocations] commmit_block
       (描述符块(数据块或撤销块)[更多数据或撤销]提交块)
     - [more transactions...]
       (更多事务……)
   * - 
     - One transaction
       (一个事务)
     -

Notice that a transaction begins with either a descriptor and some data,
or a block revocation list. A finished transaction always ends with a
commit. If there is no commit record (or the checksums don't match), the
transaction will be discarded during replay.
(注意，事务要么以一个描述符和一些数据开始，要么以一个块撤销列表开始。已完成的事务总是以一次提交结束。如果没有提交记录(或者校验和不匹配)，该事务将在重放期间被丢弃。)

External Journal
(外部日志)
~~~~~~~~~~~~~~~~

Optionally, an ext4 filesystem can be created with an external journal
device (as opposed to an internal journal, which uses a reserved inode).
In this case, on the filesystem device, ``s_journal_inum`` should be
zero and ``s_journal_uuid`` should be set. On the journal device there
will be an ext4 super block in the usual place, with a matching UUID.
The journal superblock will be in the next full block after the
superblock.
(可选地，创建 ext4 文件系统时可以带一个外部日志设备(与使用保留 inode 的内部日志相对)。在这种情况下，在文件系统设备上，``s_journal_inum`` 应为零，而 ``s_journal_uuid`` 应被设置。在日志设备上的常用位置会有一个 ext4 超级块，带有匹配的 UUID。日志超级块将位于超级块之后的下一个完整块中。)

.. list-table::
   :widths: 12 12 12 32 12
   :header-rows: 1

   * - 1024 bytes of padding
       (1024 字节的填充)
     - ext4 Superblock
       (ext4 超级块)
     - Journal Superblock
       (日志超级块)
     - descriptor_block (data_blocks or revocation_block) [more data or
       revocations] commmit_block
       (描述符块(数据块或撤销块)[更多数据或撤销]提交块)
     - [more transactions...]
       (更多事务……)
   * - 
     -
     -
     - One transaction
       (一个事务)
     -

Block Header
(块头部)
~~~~~~~~~~~~

Every block in the journal starts with a common 12-byte header
``struct journal_header_s``:
(日志中的每个块都以一个公共的 12 字节头部 ``struct journal_header_s`` 开始：)

.. list-table::
   :widths: 8 8 24 40
   :header-rows: 1

   * - Offset
       (偏移)
     - Type
       (类型)
     - Name
       (名称)
     - Description
       (描述)
   * - 0x0
     - __be32
     - h_magic
     - jbd2 magic number, 0xC03B3998.
       (jbd2 魔数，0xC03B3998。)
   * - 0x4
     - __be32
     - h_blocktype
     - Description of what this block contains. See the jbd2_blocktype_ table
       below.
       (此块所含内容的描述。参见下面的 jbd2_blocktype_ 表。)
   * - 0x8
     - __be32
     - h_sequence
     - The transaction ID that goes with this block.
       (与此块配套的事务 ID。)

.. _jbd2_blocktype:

The journal block type can be any one of:
(日志块类型可以是以下任意一种：)

.. list-table::
   :widths: 16 64
   :header-rows: 1

   * - Value
       (值)
     - Description
       (描述)
   * - 1
     - Descriptor. This block precedes a series of data blocks that were
       written through the journal during a transaction.
       (描述符。此块位于一个事务期间通过日志写入的一系列数据块之前。)
   * - 2
     - Block commit record. This block signifies the completion of a
       transaction.
       (块提交记录。此块标志着一个事务的完成。)
   * - 3
     - Journal superblock, v1.
       (日志超级块，v1。)
   * - 4
     - Journal superblock, v2.
       (日志超级块，v2。)
   * - 5
     - Block revocation records. This speeds up recovery by enabling the
       journal to skip writing blocks that were subsequently rewritten.
       (块撤销记录。通过让日志跳过写入随后已被重写的块，这可以加快恢复速度。)

Super Block
(超级块)
~~~~~~~~~~~

The super block for the journal is much simpler as compared to ext4's.
The key data kept within are size of the journal, and where to find the
start of the log of transactions.
(与 ext4 的超级块相比，日志的超级块要简单得多。其中保存的关键数据是日志的大小，以及到哪里查找事务日志的起始。)

The journal superblock is recorded as ``struct journal_superblock_s``,
which is 1024 bytes long:
(日志超级块被记录为 ``struct journal_superblock_s``，长度为 1024 字节：)

.. list-table::
   :widths: 8 8 24 40
   :header-rows: 1

   * - Offset
       (偏移)
     - Type
       (类型)
     - Name
       (名称)
     - Description
       (描述)
   * -
     -
     -
     - Static information describing the journal.
       (描述日志的静态信息。)
   * - 0x0
     - journal_header_t (12 bytes)
     - s_header
     - Common header identifying this as a superblock.
       (标识这是一个超级块的公共头部。)
   * - 0xC
     - __be32
     - s_blocksize
     - Journal device block size.
       (日志设备的块大小。)
   * - 0x10
     - __be32
     - s_maxlen
     - Total number of blocks in this journal.
       (此日志中的块总数。)
   * - 0x14
     - __be32
     - s_first
     - First block of log information.
       (日志信息的第一个块。)
   * -
     -
     -
     - Dynamic information describing the current state of the log.
       (描述日志当前状态的动态信息。)
   * - 0x18
     - __be32
     - s_sequence
     - First commit ID expected in log.
       (日志中期望出现的第一个提交 ID。)
   * - 0x1C
     - __be32
     - s_start
     - Block number of the start of log. Contrary to the comments, this field
       being zero does not imply that the journal is clean!
       (日志起始处的块号。与注释相反，此字段为零并不表示日志是干净的！)
   * - 0x20
     - __be32
     - s_errno
     - Error value, as set by jbd2_journal_abort().
       (错误值，由 jbd2_journal_abort() 设置。)
   * -
     -
     -
     - The remaining fields are only valid in a v2 superblock.
       (其余字段仅在 v2 超级块中有效。)
   * - 0x24
     - __be32
     - s_feature_compat;
     - Compatible feature set. See the table jbd2_compat_ below.
       (兼容特性集。参见下面的 jbd2_compat_ 表。)
   * - 0x28
     - __be32
     - s_feature_incompat
     - Incompatible feature set. See the table jbd2_incompat_ below.
       (不兼容特性集。参见下面的 jbd2_incompat_ 表。)
   * - 0x2C
     - __be32
     - s_feature_ro_compat
     - Read-only compatible feature set. There aren't any of these currently.
       (只读兼容特性集。目前没有任何此类特性。)
   * - 0x30
     - __u8
     - s_uuid[16]
     - 128-bit uuid for journal. This is compared against the copy in the ext4
       super block at mount time.
       (日志的 128 位 uuid。挂载时会与 ext4 超级块中的副本进行比较。)
   * - 0x40
     - __be32
     - s_nr_users
     - Number of file systems sharing this journal.
       (共享此日志的文件系统数量。)
   * - 0x44
     - __be32
     - s_dynsuper
     - Location of dynamic super block copy. (Not used?)
       (动态超级块副本的位置。(未使用？))
   * - 0x48
     - __be32
     - s_max_transaction
     - Limit of journal blocks per transaction. (Not used?)
       (每个事务的日志块数上限。(未使用？))
   * - 0x4C
     - __be32
     - s_max_trans_data
     - Limit of data blocks per transaction. (Not used?)
       (每个事务的数据块数上限。(未使用？))
   * - 0x50
     - __u8
     - s_checksum_type
     - Checksum algorithm used for the journal.  See jbd2_checksum_type_ for
       more info.
       (日志使用的校验和算法。更多信息参见 jbd2_checksum_type_。)
   * - 0x51
     - __u8[3]
     - s_padding2
     -
   * - 0x54
     - __be32
     - s_num_fc_blocks
     - Number of fast commit blocks in the journal.
       (日志中快速提交块的数量。)
   * - 0x58
     - __be32
     - s_head
     - Block number of the head (first unused block) of the journal, only
       up-to-date when the journal is empty.
       (日志头部(第一个未使用的块)的块号，仅在日志为空时才是最新的。)
   * - 0x5C
     - __u32
     - s_padding[40]
     -
   * - 0xFC
     - __be32
     - s_checksum
     - Checksum of the entire superblock, with this field set to zero.
       (整个超级块的校验和，计算时此字段置零。)
   * - 0x100
     - __u8
     - s_users[16*48]
     - ids of all file systems sharing the log. e2fsprogs/Linux don't allow
       shared external journals, but I imagine Lustre (or ocfs2?), which use
       the jbd2 code, might.
       (共享此日志的所有文件系统的 id。e2fsprogs/Linux 不允许共享外部日志，但我想使用 jbd2 代码的 Lustre(或 ocfs2？)可能会允许。)

.. _jbd2_compat:

The journal compat features are any combination of the following:
(日志兼容特性是以下任意项的组合：)

.. list-table::
   :widths: 16 64
   :header-rows: 1

   * - Value
       (值)
     - Description
       (描述)
   * - 0x1
     - Journal maintains checksums on the data blocks.
       (JBD2_FEATURE_COMPAT_CHECKSUM)
       (日志在数据块上维护校验和。(JBD2_FEATURE_COMPAT_CHECKSUM))

.. _jbd2_incompat:

The journal incompat features are any combination of the following:
(日志不兼容特性是以下任意项的组合：)

.. list-table::
   :widths: 16 64
   :header-rows: 1

   * - Value
       (值)
     - Description
       (描述)
   * - 0x1
     - Journal has block revocation records. (JBD2_FEATURE_INCOMPAT_REVOKE)
       (日志拥有块撤销记录。(JBD2_FEATURE_INCOMPAT_REVOKE))
   * - 0x2
     - Journal can deal with 64-bit block numbers.
       (JBD2_FEATURE_INCOMPAT_64BIT)
       (日志能够处理 64 位块号。(JBD2_FEATURE_INCOMPAT_64BIT))
   * - 0x4
     - Journal commits asynchronously. (JBD2_FEATURE_INCOMPAT_ASYNC_COMMIT)
       (日志异步提交。(JBD2_FEATURE_INCOMPAT_ASYNC_COMMIT))
   * - 0x8
     - This journal uses v2 of the checksum on-disk format. Each journal
       metadata block gets its own checksum, and the block tags in the
       descriptor table contain checksums for each of the data blocks in the
       journal. (JBD2_FEATURE_INCOMPAT_CSUM_V2)
       (此日志使用磁盘上校验和格式的 v2 版本。每个日志元数据块都有自己的校验和，并且描述符表中的块标签包含日志中每个数据块的校验和。(JBD2_FEATURE_INCOMPAT_CSUM_V2))
   * - 0x10
     - This journal uses v3 of the checksum on-disk format. This is the same as
       v2, but the journal block tag size is fixed regardless of the size of
       block numbers. (JBD2_FEATURE_INCOMPAT_CSUM_V3)
       (此日志使用磁盘上校验和格式的 v3 版本。这与 v2 相同，但日志块标签的大小是固定的，与块号的大小无关。(JBD2_FEATURE_INCOMPAT_CSUM_V3))
   * - 0x20
     - Journal has fast commit blocks. (JBD2_FEATURE_INCOMPAT_FAST_COMMIT)
       (日志拥有快速提交块。(JBD2_FEATURE_INCOMPAT_FAST_COMMIT))

.. _jbd2_checksum_type:

Journal checksum type codes are one of the following.  crc32 or crc32c are the
most likely choices.
(日志校验和类型代码是以下之一。最可能的选择是 crc32 或 crc32c。)

.. list-table::
   :widths: 16 64
   :header-rows: 1

   * - Value
       (值)
     - Description
       (描述)
   * - 1
     - CRC32
   * - 2
     - MD5
   * - 3
     - SHA1
   * - 4
     - CRC32C

Descriptor Block
(描述符块)
~~~~~~~~~~~~~~~~

The descriptor block contains an array of journal block tags that
describe the final locations of the data blocks that follow in the
journal. Descriptor blocks are open-coded instead of being completely
described by a data structure, but here is the block structure anyway.
Descriptor blocks consume at least 36 bytes, but use a full block:
(描述符块包含一个日志块标签数组，这些标签描述了日志中其后的数据块的最终位置。描述符块是手工编码(open-coded)的，而不是由某个数据结构完整描述，不过这里还是给出它的块结构。描述符块至少消耗 36 字节，但会占用一个完整的块：)

.. list-table::
   :widths: 8 8 24 40
   :header-rows: 1

   * - Offset
       (偏移)
     - Type
       (类型)
     - Name
       (名称)
     - Descriptor
       (描述)
   * - 0x0
     - journal_header_t
     - (open coded)
       (手工编码)
     - Common block header.
       (公共块头部。)
   * - 0xC
     - struct journal_block_tag_s
     - open coded array[]
       (手工编码的数组[])
     - Enough tags either to fill up the block or to describe all the data
       blocks that follow this descriptor block.
       (足够多的标签，要么填满该块，要么描述此描述符块之后的所有数据块。)

Journal block tags have any of the following formats, depending on which
journal feature and block tag flags are set.
(日志块标签具有以下任意一种格式，具体取决于设置了哪些日志特性和块标签标志。)

If JBD2_FEATURE_INCOMPAT_CSUM_V3 is set, the journal block tag is
defined as ``struct journal_block_tag3_s``, which looks like the
following. The size is 16 or 32 bytes.
(如果设置了 JBD2_FEATURE_INCOMPAT_CSUM_V3，日志块标签被定义为 ``struct journal_block_tag3_s``，如下所示。大小为 16 或 32 字节。)

.. list-table::
   :widths: 8 8 24 40
   :header-rows: 1

   * - Offset
       (偏移)
     - Type
       (类型)
     - Name
       (名称)
     - Descriptor
       (描述)
   * - 0x0
     - __be32
     - t_blocknr
     - Lower 32-bits of the location of where the corresponding data block
       should end up on disk.
       (对应数据块在磁盘上最终位置的低 32 位。)
   * - 0x4
     - __be32
     - t_flags
     - Flags that go with the descriptor. See the table jbd2_tag_flags_ for
       more info.
       (随描述符一起的标志。更多信息参见 jbd2_tag_flags_ 表。)
   * - 0x8
     - __be32
     - t_blocknr_high
     - Upper 32-bits of the location of where the corresponding data block
       should end up on disk. This is zero if JBD2_FEATURE_INCOMPAT_64BIT is
       not enabled.
       (对应数据块在磁盘上最终位置的高 32 位。如果未启用 JBD2_FEATURE_INCOMPAT_64BIT，则为零。)
   * - 0xC
     - __be32
     - t_checksum
     - Checksum of the journal UUID, the sequence number, and the data block.
       (日志 UUID、序列号和数据块的校验和。)
   * -
     -
     -
     - This field appears to be open coded. It always comes at the end of the
       tag, after t_checksum. This field is not present if the "same UUID" flag
       is set.
       (此字段似乎是手工编码的。它总是位于标签的末尾，在 t_checksum 之后。如果设置了“相同 UUID”标志，则此字段不存在。)
   * - 0x8 or 0xC
     - char
     - uuid[16]
     - A UUID to go with this tag. This field appears to be copied from the
       ``j_uuid`` field in ``struct journal_s``, but only tune2fs touches that
       field.
       (与此标签配套的 UUID。此字段似乎是从 ``struct journal_s`` 的 ``j_uuid`` 字段复制而来，但只有 tune2fs 会碰该字段。)

.. _jbd2_tag_flags:

The journal tag flags are any combination of the following:
(日志标签标志是以下任意项的组合：)

.. list-table::
   :widths: 16 64
   :header-rows: 1

   * - Value
       (值)
     - Description
       (描述)
   * - 0x1
     - On-disk block is escaped. The first four bytes of the data block just
       happened to match the jbd2 magic number.
       (磁盘上的块被转义了。数据块的前四个字节恰好与 jbd2 魔数相同。)
   * - 0x2
     - This block has the same UUID as previous, therefore the UUID field is
       omitted.
       (此块与前一个块具有相同的 UUID，因此省略了 UUID 字段。)
   * - 0x4
     - The data block was deleted by the transaction. (Not used?)
       (数据块已被该事务删除。(未使用？))
   * - 0x8
     - This is the last tag in this descriptor block.
       (这是此描述符块中的最后一个标签。)

If JBD2_FEATURE_INCOMPAT_CSUM_V3 is NOT set, the journal block tag
is defined as ``struct journal_block_tag_s``, which looks like the
following. The size is 8, 12, 24, or 28 bytes:
(如果未设置 JBD2_FEATURE_INCOMPAT_CSUM_V3，日志块标签被定义为 ``struct journal_block_tag_s``，如下所示。大小为 8、12、24 或 28 字节：)

.. list-table::
   :widths: 8 8 24 40
   :header-rows: 1

   * - Offset
       (偏移)
     - Type
       (类型)
     - Name
       (名称)
     - Descriptor
       (描述)
   * - 0x0
     - __be32
     - t_blocknr
     - Lower 32-bits of the location of where the corresponding data block
       should end up on disk.
       (对应数据块在磁盘上最终位置的低 32 位。)
   * - 0x4
     - __be16
     - t_checksum
     - Checksum of the journal UUID, the sequence number, and the data block.
       Note that only the lower 16 bits are stored.
       (日志 UUID、序列号和数据块的校验和。注意只存储低 16 位。)
   * - 0x6
     - __be16
     - t_flags
     - Flags that go with the descriptor. See the table jbd2_tag_flags_ for
       more info.
       (随描述符一起的标志。更多信息参见 jbd2_tag_flags_ 表。)
   * -
     -
     -
     - This next field is only present if the super block indicates support for
       64-bit block numbers.
       (只有当超级块表明支持 64 位块号时，下一个字段才存在。)
   * - 0x8
     - __be32
     - t_blocknr_high
     - Upper 32-bits of the location of where the corresponding data block
       should end up on disk.
       (对应数据块在磁盘上最终位置的高 32 位。)
   * -
     -
     -
     - This field appears to be open coded. It always comes at the end of the
       tag, after t_flags or t_blocknr_high. This field is not present if the
       "same UUID" flag is set.
       (此字段似乎是手工编码的。它总是位于标签的末尾，在 t_flags 或 t_blocknr_high 之后。如果设置了“相同 UUID”标志，则此字段不存在。)
   * - 0x8 or 0xC
     - char
     - uuid[16]
     - A UUID to go with this tag. This field appears to be copied from the
       ``j_uuid`` field in ``struct journal_s``, but only tune2fs touches that
       field.
       (与此标签配套的 UUID。此字段似乎是从 ``struct journal_s`` 的 ``j_uuid`` 字段复制而来，但只有 tune2fs 会碰该字段。)

If JBD2_FEATURE_INCOMPAT_CSUM_V2 or
JBD2_FEATURE_INCOMPAT_CSUM_V3 are set, the end of the block is a
``struct jbd2_journal_block_tail``, which looks like this:
(如果设置了 JBD2_FEATURE_INCOMPAT_CSUM_V2 或 JBD2_FEATURE_INCOMPAT_CSUM_V3，块的末尾是一个 ``struct jbd2_journal_block_tail``，如下所示：)

.. list-table::
   :widths: 8 8 24 40
   :header-rows: 1

   * - Offset
       (偏移)
     - Type
       (类型)
     - Name
       (名称)
     - Descriptor
       (描述)
   * - 0x0
     - __be32
     - t_checksum
     - Checksum of the journal UUID + the descriptor block, with this field set
       to zero.
       (日志 UUID + 描述符块的校验和，计算时此字段置零。)

Data Block
(数据块)
~~~~~~~~~~

In general, the data blocks being written to disk through the journal
are written verbatim into the journal file after the descriptor block.
However, if the first four bytes of the block match the jbd2 magic
number then those four bytes are replaced with zeroes and the “escaped”
flag is set in the descriptor block tag.
(一般来说，通过日志写入磁盘的数据块会在描述符块之后被原封不动地写入日志文件。但是，如果该块的前四个字节与 jbd2 魔数相同，那么这四个字节会被替换为零，并在描述符块标签中设置“转义(escaped)”标志。)

Revocation Block
(撤销块)
~~~~~~~~~~~~~~~~

A revocation block is used to prevent replay of a block in an earlier
transaction. This is used to mark blocks that were journalled at one
time but are no longer journalled. Typically this happens if a metadata
block is freed and re-allocated as a file data block; in this case, a
journal replay after the file block was written to disk will cause
corruption.
(撤销块用于阻止早先事务中的某个块被重放。它用于标记那些曾经被日志记录、但不再被日志记录的块。通常，当一个元数据块被释放并重新分配为文件数据块时会发生这种情况；在这种情况下，如果在该文件块写入磁盘之后进行日志重放，将导致损坏。)

**NOTE**: This mechanism is NOT used to express “this journal block is
superseded by this other journal block”, as the author (djwong)
mistakenly thought. Any block being added to a transaction will cause
the removal of all existing revocation records for that block.
(**注意**：正如作者 (djwong) 曾误以为的那样，此机制并不用于表达“此日志块已被另一个日志块取代”。任何被加入事务的块都会导致移除该块的所有现有撤销记录。)

Revocation blocks are described in
``struct jbd2_journal_revoke_header_s``, are at least 16 bytes in
length, but use a full block:
(撤销块由 ``struct jbd2_journal_revoke_header_s`` 描述，长度至少为 16 字节，但会占用一个完整的块：)

.. list-table::
   :widths: 8 8 24 40
   :header-rows: 1

   * - Offset
       (偏移)
     - Type
       (类型)
     - Name
       (名称)
     - Description
       (描述)
   * - 0x0
     - journal_header_t
     - r_header
     - Common block header.
       (公共块头部。)
   * - 0xC
     - __be32
     - r_count
     - Number of bytes used in this block.
       (此块中使用的字节数。)
   * - 0x10
     - __be32 or __be64
     - blocks[0]
     - Blocks to revoke.
       (要撤销的块。)

After r_count is a linear array of block numbers that are effectively
revoked by this transaction. The size of each block number is 8 bytes if
the superblock advertises 64-bit block number support, or 4 bytes
otherwise.
(在 r_count 之后是一个块号的线性数组，这些块号被此事务实际撤销。如果超级块声明支持 64 位块号，则每个块号的大小为 8 字节，否则为 4 字节。)

If JBD2_FEATURE_INCOMPAT_CSUM_V2 or
JBD2_FEATURE_INCOMPAT_CSUM_V3 are set, the end of the revocation
block is a ``struct jbd2_journal_revoke_tail``, which has this format:
(如果设置了 JBD2_FEATURE_INCOMPAT_CSUM_V2 或 JBD2_FEATURE_INCOMPAT_CSUM_V3，撤销块的末尾是一个 ``struct jbd2_journal_revoke_tail``，其格式如下：)

.. list-table::
   :widths: 8 8 24 40
   :header-rows: 1

   * - Offset
       (偏移)
     - Type
       (类型)
     - Name
       (名称)
     - Description
       (描述)
   * - 0x0
     - __be32
     - r_checksum
     - Checksum of the journal UUID + revocation block
       (日志 UUID + 撤销块的校验和)

Commit Block
(提交块)
~~~~~~~~~~~~

The commit block is a sentry that indicates that a transaction has been
completely written to the journal. Once this commit block reaches the
journal, the data stored with this transaction can be written to their
final locations on disk.
(提交块是一个哨兵，表明一个事务已被完整地写入日志。一旦此提交块到达日志，与该事务一起存储的数据就可以被写到它们在磁盘上的最终位置。)

The commit block is described by ``struct commit_header``, which is 32
bytes long (but uses a full block):
(提交块由 ``struct commit_header`` 描述，长度为 32 字节(但会占用一个完整的块)：)

.. list-table::
   :widths: 8 8 24 40
   :header-rows: 1

   * - Offset
       (偏移)
     - Type
       (类型)
     - Name
       (名称)
     - Descriptor
       (描述)
   * - 0x0
     - journal_header_s
     - (open coded)
       (手工编码)
     - Common block header.
       (公共块头部。)
   * - 0xC
     - unsigned char
     - h_chksum_type
     - The type of checksum to use to verify the integrity of the data blocks
       in the transaction. See jbd2_checksum_type_ for more info.
       (用于验证事务中数据块完整性的校验和类型。更多信息参见 jbd2_checksum_type_。)
   * - 0xD
     - unsigned char
     - h_chksum_size
     - The number of bytes used by the checksum. Most likely 4.
       (校验和使用的字节数。很可能是 4。)
   * - 0xE
     - unsigned char
     - h_padding[2]
     -
   * - 0x10
     - __be32
     - h_chksum[JBD2_CHECKSUM_BYTES]
     - 32 bytes of space to store checksums. If
       JBD2_FEATURE_INCOMPAT_CSUM_V2 or JBD2_FEATURE_INCOMPAT_CSUM_V3
       are set, the first ``__be32`` is the checksum of the journal UUID and
       the entire commit block, with this field zeroed. If
       JBD2_FEATURE_COMPAT_CHECKSUM is set, the first ``__be32`` is the
       crc32 of all the blocks already written to the transaction.
       (32 字节的校验和存储空间。如果设置了 JBD2_FEATURE_INCOMPAT_CSUM_V2 或 JBD2_FEATURE_INCOMPAT_CSUM_V3，第一个 ``__be32`` 是日志 UUID 加整个提交块的校验和，计算时此字段置零。如果设置了 JBD2_FEATURE_COMPAT_CHECKSUM，第一个 ``__be32`` 是已写入该事务的所有块的 crc32。)
   * - 0x30
     - __be64
     - h_commit_sec
     - The time that the transaction was committed, in seconds since the epoch.
       (事务被提交的时间，自纪元起的秒数。)
   * - 0x38
     - __be32
     - h_commit_nsec
     - Nanoseconds component of the above timestamp.
       (上述时间戳的纳秒部分。)

Fast commits
(快速提交)
~~~~~~~~~~~~

Fast commit area is organized as a log of tag length values. Each TLV has
a ``struct ext4_fc_tl`` in the beginning which stores the tag and the length
of the entire field. It is followed by variable length tag specific value.
Here is the list of supported tags and their meanings:
(快速提交区域被组织为标签-长度-值(tag length value)的日志形式。每个 TLV 开头都有一个 ``struct ext4_fc_tl``，存储整个字段的标签和长度。其后是可变长度的、与标签相关的值。以下是受支持的标签及其含义的列表：)

.. list-table::
   :widths: 8 20 20 32
   :header-rows: 1

   * - Tag
       (标签)
     - Meaning
       (含义)
     - Value struct
       (值结构)
     - Description
       (描述)
   * - EXT4_FC_TAG_HEAD
     - Fast commit area header
       (快速提交区域头部)
     - ``struct ext4_fc_head``
     - Stores the TID of the transaction after which these fast commits should
       be applied.
       (存储事务的 TID，这些快速提交应在该事务之后应用。)
   * - EXT4_FC_TAG_ADD_RANGE
     - Add extent to inode
       (向 inode 添加 extent)
     - ``struct ext4_fc_add_range``
     - Stores the inode number and extent to be added in this inode
       (存储要在该 inode 中添加的 inode 号和 extent)
   * - EXT4_FC_TAG_DEL_RANGE
     - Remove logical offsets to inode
       (移除 inode 的逻辑偏移)
     - ``struct ext4_fc_del_range``
     - Stores the inode number and the logical offset range that needs to be
       removed
       (存储需要移除的 inode 号和逻辑偏移范围)
   * - EXT4_FC_TAG_CREAT
     - Create directory entry for a newly created file
       (为新创建的文件创建目录项)
     - ``struct ext4_fc_dentry_info``
     - Stores the parent inode number, inode number and directory entry of the
       newly created file
       (存储新创建的文件的父 inode 号、inode 号和目录项)
   * - EXT4_FC_TAG_LINK
     - Link a directory entry to an inode
       (将目录项链接到 inode)
     - ``struct ext4_fc_dentry_info``
     - Stores the parent inode number, inode number and directory entry
       (存储父 inode 号、inode 号和目录项)
   * - EXT4_FC_TAG_UNLINK
     - Unlink a directory entry of an inode
       (取消 inode 的某个目录项的链接)
     - ``struct ext4_fc_dentry_info``
     - Stores the parent inode number, inode number and directory entry
       (存储父 inode 号、inode 号和目录项)

   * - EXT4_FC_TAG_PAD
     - Padding (unused area)
       (填充(未使用的区域))
     - None
       (无)
     - Unused bytes in the fast commit area.
       (快速提交区域中未使用的字节。)

   * - EXT4_FC_TAG_TAIL
     - Mark the end of a fast commit
       (标记一次快速提交的结束)
     - ``struct ext4_fc_tail``
     - Stores the TID of the commit, CRC of the fast commit of which this tag
       represents the end of
       (存储该提交的 TID，以及此标签所标志其结束的那次快速提交的 CRC)

Fast Commit Replay Idempotence
(快速提交重放的幂等性)
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

Fast commits tags are idempotent in nature provided the recovery code follows
certain rules. The guiding principle that the commit path follows while
committing is that it stores the result of a particular operation instead of
storing the procedure.
(只要恢复代码遵循某些规则，快速提交标签在本质上是幂等的。提交路径在提交时遵循的指导原则是：它存储某个特定操作的结果，而不是存储操作过程。)

Let's consider this rename operation: 'mv /a /b'. Let's assume dirent '/a'
was associated with inode 10. During fast commit, instead of storing this
operation as a procedure "rename a to b", we store the resulting file system
state as a "series" of outcomes:
(我们来考虑这个重命名操作：'mv /a /b'。假设目录项 '/a' 与 inode 10 相关联。在快速提交期间，我们不把该操作作为一个过程“把 a 重命名为 b”来存储，而是把产生的文件系统状态作为一个由结果构成的“序列”来存储：)

- Link dirent b to inode 10
  (将目录项 b 链接到 inode 10)
- Unlink dirent a
  (取消目录项 a 的链接)
- Inode 10 with valid refcount
  (inode 10 具有有效的引用计数)

Now when recovery code runs, it needs "enforce" this state on the file
system. This is what guarantees idempotence of fast commit replay.
(当恢复代码运行时，它需要在文件系统上“强制”实施这个状态。这正是保证快速提交重放幂等性的原因。)

Let's take an example of a procedure that is not idempotent and see how fast
commits make it idempotent. Consider following sequence of operations:
(让我们举一个非幂等过程的例子，看看快速提交如何使它变得幂等。考虑以下操作序列：)

1) rm A
   (删除 A)
2) mv B A
   (把 B 移动(重命名)为 A)
3) read A
   (读取 A)

If we store this sequence of operations as is then the replay is not idempotent.
Let's say while in replay, we crash after (2). During the second replay,
file A (which was actually created as a result of "mv B A" operation) would get
deleted. Thus, file named A would be absent when we try to read A. So, this
sequence of operations is not idempotent. However, as mentioned above, instead
of storing the procedure fast commits store the outcome of each procedure. Thus
the fast commit log for above procedure would be as follows:
(如果我们按原样存储这个操作序列，那么重放就不是幂等的。假设在重放期间，我们在 (2) 之后崩溃。在第二次重放期间，文件 A(它实际上是由于“mv B A”操作而创建的)会被删除。于是，当我们尝试读取 A 时，名为 A 的文件将不存在。所以，这个操作序列不是幂等的。然而，如上所述，快速提交不存储操作过程，而是存储每个过程的结果。因此，上述过程的快速提交日志将如下所示：)

(Let's assume dirent A was linked to inode 10 and dirent B was linked to
inode 11 before the replay)
(假设在重放之前，目录项 A 链接到 inode 10，目录项 B 链接到 inode 11)

1) Unlink A
   (取消 A 的链接)
2) Link A to inode 11
   (将 A 链接到 inode 11)
3) Unlink B
   (取消 B 的链接)
4) Inode 11
   (inode 11)

If we crash after (3) we will have file A linked to inode 11. During the second
replay, we will remove file A (inode 11). But we will create it back and make
it point to inode 11. We won't find B, so we'll just skip that step. At this
point, the refcount for inode 11 is not reliable, but that gets fixed by the
replay of last inode 11 tag. Thus, by converting a non-idempotent procedure
into a series of idempotent outcomes, fast commits ensured idempotence during
the replay.
(如果我们在 (3) 之后崩溃，文件 A 将链接到 inode 11。在第二次重放期间，我们会移除文件 A(inode 11)。但我们会重新创建它并使其指向 inode 11。我们找不到 B，所以只需跳过那一步。此时，inode 11 的引用计数是不可靠的，但这会被最后一个 inode 11 标签的重放所修复。因此，通过把一个非幂等的过程转换为一系列幂等的结果，快速提交保证了重放期间的幂等性。)

Journal Checkpoint
(日志检查点)
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

Checkpointing the journal ensures all transactions and their associated buffers
are submitted to the disk. In-progress transactions are waited upon and included
in the checkpoint. Checkpointing is used internally during critical updates to
the filesystem including journal recovery, filesystem resizing, and freeing of
the journal_t structure.
(对日志进行检查点操作可确保所有事务及其关联的缓冲都被提交到磁盘。进行中的事务会被等待并包含在检查点中。检查点操作在文件系统的关键更新期间被内部使用，包括日志恢复、文件系统大小调整以及释放 journal_t 结构。)

A journal checkpoint can be triggered from userspace via the ioctl
EXT4_IOC_CHECKPOINT. This ioctl takes a single, u64 argument for flags.
Currently, three flags are supported. First, EXT4_IOC_CHECKPOINT_FLAG_DRY_RUN
can be used to verify input to the ioctl. It returns error if there is any
invalid input, otherwise it returns success without performing
any checkpointing. This can be used to check whether the ioctl exists on a
system and to verify there are no issues with arguments or flags. The
other two flags are EXT4_IOC_CHECKPOINT_FLAG_DISCARD and
EXT4_IOC_CHECKPOINT_FLAG_ZEROOUT. These flags cause the journal blocks to be
discarded or zero-filled, respectively, after the journal checkpoint is
complete. EXT4_IOC_CHECKPOINT_FLAG_DISCARD and EXT4_IOC_CHECKPOINT_FLAG_ZEROOUT
cannot both be set. The ioctl may be useful when snapshotting a system or for
complying with content deletion SLOs.
(日志检查点可以由用户空间通过 ioctl EXT4_IOC_CHECKPOINT 触发。该 ioctl 接受单个 u64 参数作为标志。目前支持三个标志。首先，EXT4_IOC_CHECKPOINT_FLAG_DRY_RUN 可用于验证 ioctl 的输入。如果有任何无效输入，它会返回错误，否则它不执行任何检查点操作而返回成功。这可用于检查某个系统上是否存在该 ioctl，并验证参数或标志没有问题。另外两个标志是 EXT4_IOC_CHECKPOINT_FLAG_DISCARD 和 EXT4_IOC_CHECKPOINT_FLAG_ZEROOUT。在日志检查点完成之后，这两个标志分别使日志块被丢弃(discard)或被清零填充(zero-filled)。EXT4_IOC_CHECKPOINT_FLAG_DISCARD 和 EXT4_IOC_CHECKPOINT_FLAG_ZEROOUT 不能同时设置。该 ioctl 在对系统进行快照时，或者为了满足内容删除 SLO 时可能有用。)
