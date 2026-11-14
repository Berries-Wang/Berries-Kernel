.. SPDX-License-Identifier: GPL-2.0

.. UBIFS Authentication
.. sigma star gmbh
.. 2018

============================
UBIFS Authentication Support
============================

(UBIFS 认证支持)

Introduction
(简介)
============

UBIFS utilizes the fscrypt framework to provide confidentiality for file
contents and file names. This prevents attacks where an attacker is able to
read contents of the filesystem on a single point in time. A classic example
is a lost smartphone where the attacker is unable to read personal data stored
on the device without the filesystem decryption key.
(UBIFS 利用 fscrypt 框架为文件内容和文件名提供保密性。这可以防止攻击者读取文件系统在单个时间点上的内容的攻击。一个典型的例子是丢失的智能手机：没有文件系统解密密钥，攻击者无法读取设备上存储的个人数据。)

At the current state, UBIFS encryption however does not prevent attacks where
the attacker is able to modify the filesystem contents and the user uses the
device afterwards. In such a scenario an attacker can modify filesystem
contents arbitrarily without the user noticing. One example is to modify a
binary to perform a malicious action when executed [DMC-CBC-ATTACK]. Since
most of the filesystem metadata of UBIFS is stored in plain, this makes it
fairly easy to swap files and replace their contents.
(然而，就目前而言，UBIFS 加密并不能防止攻击者修改文件系统内容、而用户事后继续使用设备的攻击。在这种情况下，攻击者可以在用户毫无察觉的情况下任意修改文件系统内容。一个例子是修改某个二进制文件，使其在执行时执行恶意操作 [DMC-CBC-ATTACK]。由于 UBIFS 的大部分文件系统元数据以明文存储，这使得调换文件并替换其内容相当容易。)

Other full disk encryption systems like dm-crypt cover all filesystem metadata,
which makes such kinds of attacks more complicated, but not impossible.
Especially, if the attacker is given access to the device multiple points in
time. For dm-crypt and other filesystems that build upon the Linux block IO
layer, the dm-integrity or dm-verity subsystems [DM-INTEGRITY, DM-VERITY]
can be used to get full data authentication at the block layer.
These can also be combined with dm-crypt [CRYPTSETUP2].
(诸如 dm-crypt 之类的其他全盘加密系统覆盖了所有文件系统元数据，这使此类攻击变得更加复杂，但并非不可能。尤其是当攻击者可以在多个时间点访问设备时。对于 dm-crypt 以及构建在 Linux 块 I/O 层之上的其他文件系统，可以使用 dm-integrity 或 dm-verity 子系统 [DM-INTEGRITY, DM-VERITY] 在块层获得完整的数据认证。这些也可以与 dm-crypt 结合使用 [CRYPTSETUP2]。)

This document describes an approach to get file contents _and_ full metadata
authentication for UBIFS. Since UBIFS uses fscrypt for file contents and file
name encryption, the authentication system could be tied into fscrypt such that
existing features like key derivation can be utilized. It should however also
be possible to use UBIFS authentication without using encryption.
(本文档描述了一种为 UBIFS 获得文件内容_和_完整元数据认证的方法。由于 UBIFS 使用 fscrypt 进行文件内容和文件名加密，认证系统可以绑定到 fscrypt 中，从而可以利用密钥派生等现有特性。不过，也应该可以在不使用加密的情况下使用 UBIFS 认证。)


MTD, UBI & UBIFS
(MTD、UBI 与 UBIFS)
----------------

On Linux, the MTD (Memory Technology Devices) subsystem provides a uniform
interface to access raw flash devices. One of the more prominent subsystems that
work on top of MTD is UBI (Unsorted Block Images). It provides volume management
for flash devices and is thus somewhat similar to LVM for block devices. In
addition, it deals with flash-specific wear-leveling and transparent I/O error
handling. UBI offers logical erase blocks (LEBs) to the layers on top of it
and maps them transparently to physical erase blocks (PEBs) on the flash.
(在 Linux 上，MTD(Memory Technology Devices，内存技术设备)子系统提供了访问裸闪存设备的统一接口。在 MTD 之上工作的较著名的子系统之一是 UBI(Unsorted Block Images)。它为闪存设备提供卷管理，因此有点类似于块设备的 LVM。此外，它还处理闪存特有的磨损均衡和透明的 I/O 错误处理。UBI 向其上的层提供逻辑擦除块(LEB)，并将它们透明地映射到闪存上的物理擦除块(PEB)。)

UBIFS is a filesystem for raw flash which operates on top of UBI. Thus, wear
leveling and some flash specifics are left to UBI, while UBIFS focuses on
scalability, performance and recoverability.
(UBIFS 是一个在 UBI 之上运行、面向裸闪存的文件系统。因此，磨损均衡和一些闪存特有的细节留给 UBI 处理，而 UBIFS 则专注于可扩展性、性能和可恢复性。)

::

	+------------+ +*******+ +-----------+ +-----+
	|            | * UBIFS * | UBI-BLOCK | | ... |
	| JFFS/JFFS2 | +*******+ +-----------+ +-----+
	|            | +-----------------------------+ +-----------+ +-----+
	|            | |              UBI            | | MTD-BLOCK | | ... |
	+------------+ +-----------------------------+ +-----------+ +-----+
	+------------------------------------------------------------------+
	|                  MEMORY TECHNOLOGY DEVICES (MTD)                 |
	+------------------------------------------------------------------+
	+-----------------------------+ +--------------------------+ +-----+
	|         NAND DRIVERS        | |        NOR DRIVERS       | | ... |
	+-----------------------------+ +--------------------------+ +-----+

            Figure 1: Linux kernel subsystems for dealing with raw flash



Internally, UBIFS maintains multiple data structures which are persisted on
the flash:
(在内部，UBIFS 维护着多个持久化在闪存上的数据结构：)

- *Index*: an on-flash B+ tree where the leaf nodes contain filesystem data
  (*Index*: 一个片上 B+ 树，其叶节点包含文件系统数据)
- *Journal*: an additional data structure to collect FS changes before updating
  the on-flash index and reduce flash wear.
  (*Journal* (日志)：一种附加的数据结构，用于在更新片上索引之前收集文件系统变更，从而减少闪存磨损。)
- *Tree Node Cache (TNC)*: an in-memory B+ tree that reflects the current FS
  state to avoid frequent flash reads. It is basically the in-memory
  representation of the index, but contains additional attributes.
  (*Tree Node Cache (TNC)* (树节点缓存)：一种内存中的 B+ 树，反映当前的文件系统状态，以避免频繁的闪存读取。它基本上是索引的内存表示，但包含额外的属性。)
- *LEB property tree (LPT)*: an on-flash B+ tree for free space accounting per
  UBI LEB.
  (*LEB property tree (LPT)* (LEB 属性树)：一个片上 B+ 树，用于按 UBI LEB 进行空闲空间记账。)

In the remainder of this section we will cover the on-flash UBIFS data
structures in more detail. The TNC is of less importance here since it is never
persisted onto the flash directly. More details on UBIFS can also be found in
[UBIFS-WP].
(在本节的剩余部分，我们将更详细地介绍片上的 UBIFS 数据结构。TNC 在此不太重要，因为它从不直接持久化到闪存上。有关 UBIFS 的更多细节还可参见 [UBIFS-WP]。)


UBIFS Index & Tree Node Cache
(UBIFS 索引与树节点缓存)
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

Basic on-flash UBIFS entities are called *nodes*. UBIFS knows different types
of nodes. Eg. data nodes (``struct ubifs_data_node``) which store chunks of file
contents or inode nodes (``struct ubifs_ino_node``) which represent VFS inodes.
Almost all types of nodes share a common header (``ubifs_ch``) containing basic
information like node type, node length, a sequence number, etc. (see
``fs/ubifs/ubifs-media.h`` in kernel source). Exceptions are entries of the LPT
and some less important node types like padding nodes which are used to pad
unusable content at the end of LEBs.
(片上 UBIFS 的基本实体称为*节点*。UBIFS 有多种不同类型的节点。例如，存储文件内容块的数据节点(``struct ubifs_data_node``)，或代表 VFS inode 的 inode 节点(``struct ubifs_ino_node``)。几乎所有类型的节点都共享一个公共头部(``ubifs_ch``)，其中包含节点类型、节点长度、序列号等基本信息(参见内核源码中的 ``fs/ubifs/ubifs-media.h``)。例外情况是 LPT 的条目以及一些较不重要的节点类型，例如用于填充 LEB 末尾不可用内容的填充节点。)

To avoid re-writing the whole B+ tree on every single change, it is implemented
as *wandering tree*, where only the changed nodes are re-written and previous
versions of them are obsoleted without erasing them right away. As a result,
the index is not stored in a single place on the flash, but *wanders* around
and there are obsolete parts on the flash as long as the LEB containing them is
not reused by UBIFS. To find the most recent version of the index, UBIFS stores
a special node called *master node* into UBI LEB 1 which always points to the
most recent root node of the UBIFS index. For recoverability, the master node
is additionally duplicated to LEB 2. Mounting UBIFS is thus a simple read of
LEB 1 and 2 to get the current master node and from there get the location of
the most recent on-flash index.
(为了避免每次更改都重写整个 B+ 树，它被实现为*游走树*(wandering tree)：只有被更改的节点会被重写，它们的先前版本会被废弃而不会立即擦除。结果是，索引并非存储在闪存上的单一位置，而是四处*游走*，并且只要包含过时部分的 LEB 尚未被 UBIFS 重用，这些过时部分就一直留在闪存上。为了找到索引的最新版本，UBIFS 将一个称为*主节点*(master node)的特殊节点存储到 UBI LEB 1 中，它始终指向 UBIFS 索引最新的根节点。为了可恢复性，主节点还被额外复制到 LEB 2。因此，挂载 UBIFS 只需简单地读取 LEB 1 和 2 以获得当前的主节点，并从那里获得最新片上索引的位置。)

The TNC is the in-memory representation of the on-flash index. It contains some
additional runtime attributes per node which are not persisted. One of these is
a dirty-flag which marks nodes that have to be persisted the next time the
index is written onto the flash. The TNC acts as a write-back cache and all
modifications of the on-flash index are done through the TNC. Like other caches,
the TNC does not have to mirror the full index into memory, but reads parts of
it from flash whenever needed. A *commit* is the UBIFS operation of updating the
on-flash filesystem structures like the index. On every commit, the TNC nodes
marked as dirty are written to the flash to update the persisted index.
(TNC 是片上索引在内存中的表示。它为每个节点包含一些额外的、不会被持久化的运行时属性。其中之一是脏标志，它标记下次将索引写入闪存时需要持久化的节点。TNC 充当回写缓存，片上索引的所有修改都通过 TNC 完成。与其他缓存一样，TNC 不必将整个索引镜像到内存中，而是在需要时从闪存读取其中的一部分。*提交* (commit)是 UBIFS 更新片上文件系统结构(如索引)的操作。每次提交时，被标记为脏的 TNC 节点会被写入闪存，以更新持久化的索引。)


Journal
(日志)
~~~~~~~

To avoid wearing out the flash, the index is only persisted (*committed*) when
certain conditions are met (eg. ``fsync(2)``). The journal is used to record
any changes (in form of inode nodes, data nodes etc.) between commits
of the index. During mount, the journal is read from the flash and replayed
onto the TNC (which will be created on-demand from the on-flash index).
(为了避免磨损闪存，只有当满足特定条件(例如 ``fsync(2)``)时，索引才会被持久化(*提交*)。日志用于记录索引两次提交之间的所有更改(以 inode 节点、数据节点等形式)。在挂载期间，日志会从闪存读取并重放到 TNC 上(TNC 将按需从片上索引创建)。)

UBIFS reserves a bunch of LEBs just for the journal called *log area*. The
amount of log area LEBs is configured on filesystem creation (using
``mkfs.ubifs``) and stored in the superblock node. The log area contains only
two types of nodes: *reference nodes* and *commit start nodes*. A commit start
node is written whenever an index commit is performed. Reference nodes are
written on every journal update. Each reference node points to the position of
other nodes (inode nodes, data nodes etc.) on the flash that are part of this
journal entry. These nodes are called *buds* and describe the actual filesystem
changes including their data.
(UBIFS 专门为日志预留了一组 LEB，称为*日志区*(log area)。日志区 LEB 的数量在创建文件系统时(使用 ``mkfs.ubifs``)配置，并存储在超级块节点中。日志区只包含两种类型的节点：*引用节点* (reference node)和*提交开始节点* (commit start node)。每当执行一次索引提交，就会写入一个提交开始节点。引用节点在每次日志更新时写入。每个引用节点指向闪存上作为该日志条目一部分的其他节点(inode 节点、数据节点等)的位置。这些节点称为 *bud*，它们描述实际的文件系统更改及其数据。)

The log area is maintained as a ring. Whenever the journal is almost full,
a commit is initiated. This also writes a commit start node so that during
mount, UBIFS will seek for the most recent commit start node and just replay
every reference node after that. Every reference node before the commit start
node will be ignored as they are already part of the on-flash index.
(日志区按环形维护。每当日志快要满时，就会发起一次提交。这也会写入一个提交开始节点，这样在挂载期间，UBIFS 会寻找最近的提交开始节点，并只重放其后的每个引用节点。提交开始节点之前的每个引用节点都会被忽略，因为它们已经是片上索引的一部分。)

When writing a journal entry, UBIFS first ensures that enough space is
available to write the reference node and buds part of this entry. Then, the
reference node is written and afterwards the buds describing the file changes.
On replay, UBIFS will record every reference node and inspect the location of
the referenced LEBs to discover the buds. If these are corrupt or missing,
UBIFS will attempt to recover them by re-reading the LEB. This is however only
done for the last referenced LEB of the journal. Only this can become corrupt
because of a power cut. If the recovery fails, UBIFS will not mount. An error
for every other LEB will directly cause UBIFS to fail the mount operation.
(写入日志条目时，UBIFS 首先确保有足够的空间来写入该条目的引用节点和 bud 部分。然后写入引用节点，之后写入描述文件更改的 bud。在重放时，UBIFS 会记录每个引用节点并检查被引用 LEB 的位置以发现 bud。如果这些 LEB 损坏或缺失，UBIFS 将尝试通过重新读取 LEB 来恢复它们。但这只针对日志中最后被引用的 LEB 进行，只有它才可能因断电而损坏。如果恢复失败，UBIFS 将不会挂载。对于任何其他 LEB 的错误将直接导致 UBIFS 挂载操作失败。)

::

       | ----    LOG AREA     ---- | ----------    MAIN AREA    ------------ |

        -----+------+-----+--------+----   ------+-----+-----+---------------
        \    |      |     |        |   /  /      |     |     |               \
        / CS |  REF | REF |        |   \  \ DENT | INO | INO |               /
        \    |      |     |        |   /  /      |     |     |               \
         ----+------+-----+--------+---   -------+-----+-----+----------------
                 |     |                  ^            ^
                 |     |                  |            |
                 +------------------------+            |
                       |                               |
                       +-------------------------------+


                Figure 2: UBIFS flash layout of log area with commit start nodes
                          (CS) and reference nodes (REF) pointing to main area
                          containing their buds


LEB Property Tree/Table
(LEB 属性树/表)
~~~~~~~~~~~~~~~~~~~~~~~

The LEB property tree is used to store per-LEB information. This includes the
LEB type and amount of free and *dirty* (old, obsolete content) space [1]_ on
the LEB. The type is important, because UBIFS never mixes index nodes with data
nodes on a single LEB and thus each LEB has a specific purpose. This again is
useful for free space calculations. See [UBIFS-WP] for more details.
(LEB 属性树用于存储每个 LEB 的信息。这包括 LEB 类型以及 LEB 上空闲和*脏*(旧的、过时的内容)空间的数量 [1]_。类型很重要，因为 UBIFS 从不在单个 LEB 上混合索引节点和数据节点，因此每个 LEB 都有特定用途。这同样有助于空闲空间计算。更多细节参见 [UBIFS-WP]。)

The LEB property tree again is a B+ tree, but it is much smaller than the
index. Due to its smaller size it is always written as one chunk on every
commit. Thus, saving the LPT is an atomic operation.
(LEB 属性树同样是一棵 B+ 树，但它比索引小得多。由于体积较小，它在每次提交时都作为一个整体块写入。因此，保存 LPT 是一个原子操作。)


.. [1] Since LEBs can only be appended and never overwritten, there is a
   difference between free space ie. the remaining space left on the LEB to be
   written to without erasing it and previously written content that is obsolete
   but can't be overwritten without erasing the full LEB.


UBIFS Authentication
(UBIFS 认证)
====================

This chapter introduces UBIFS authentication which enables UBIFS to verify
the authenticity and integrity of metadata and file contents stored on flash.
(本章介绍 UBIFS 认证，它使 UBIFS 能够验证存储在闪存上的元数据和文件内容的真实性与完整性。)


Threat Model
(威胁模型)
------------

UBIFS authentication enables detection of offline data modification. While it
does not prevent it, it enables (trusted) code to check the integrity and
authenticity of on-flash file contents and filesystem metadata. This covers
attacks where file contents are swapped.
(UBIFS 认证能够检测离线数据修改。虽然它不能阻止修改，但它使(受信任的)代码能够检查片上文件内容和文件系统元数据的完整性与真实性。这涵盖了文件内容被调换的攻击。)

UBIFS authentication will not protect against rollback of full flash contents.
Ie. an attacker can still dump the flash and restore it at a later time without
detection. It will also not protect against partial rollback of individual
index commits. That means that an attacker is able to partially undo changes.
This is possible because UBIFS does not immediately overwrites obsolete
versions of the index tree or the journal, but instead marks them as obsolete
and garbage collection erases them at a later time. An attacker can use this by
erasing parts of the current tree and restoring old versions that are still on
the flash and have not yet been erased. This is possible, because every commit
will always write a new version of the index root node and the master node
without overwriting the previous version. This is further helped by the
wear-leveling operations of UBI which copies contents from one physical
eraseblock to another and does not atomically erase the first eraseblock.
(UBIFS 认证无法防范对整个闪存内容的回滚。也就是说，攻击者仍然可以转储闪存内容并在稍后某个时间恢复它而不被检测到。它也无法防范对单个索引提交的部分回滚。这意味着攻击者能够部分撤销更改。这是可能的，因为 UBIFS 不会立即覆盖索引树或日志的过时版本，而是将它们标记为过时，由垃圾回收在稍后擦除。攻击者可以利用这一点，擦除当前树的一部分，并恢复仍然在闪存上、尚未被擦除的旧版本。这是可能的，因为每次提交总是写入索引根节点和主节点的新版本，而不覆盖先前版本。UBI 的磨损均衡操作进一步助长了这一点，它将内容从一个物理擦除块复制到另一个物理擦除块，并且不会原子性地擦除第一个擦除块。)

UBIFS authentication does not cover attacks where an attacker is able to
execute code on the device after the authentication key was provided.
Additional measures like secure boot and trusted boot have to be taken to
ensure that only trusted code is executed on a device.
(UBIFS 认证不覆盖攻击者在提供认证密钥之后能够在设备上执行代码的情况。必须采取诸如安全启动和受信任启动之类的额外措施，以确保设备上只运行受信任的代码。)


Authentication
(认证)
--------------

To be able to fully trust data read from flash, all UBIFS data structures
stored on flash are authenticated. That is:
(为了能够完全信任从闪存读取的数据，存储在闪存上的所有 UBIFS 数据结构都要进行认证。即：)

- The index which includes file contents, file metadata like extended
  attributes, file length etc.
  (索引，其中包括文件内容、文件元数据(如扩展属性、文件长度等)。)
- The journal which also contains file contents and metadata by recording changes
  to the filesystem
  (日志，它通过记录对文件系统的更改，同样包含文件内容和元数据。)
- The LPT which stores UBI LEB metadata which UBIFS uses for free space accounting
  (LPT，它存储 UBIFS 用于空闲空间记账的 UBI LEB 元数据。)


Index Authentication
(索引认证)
~~~~~~~~~~~~~~~~~~~~

Through UBIFS' concept of a wandering tree, it already takes care of only
updating and persisting changed parts from leaf node up to the root node
of the full B+ tree. This enables us to augment the index nodes of the tree
with a hash over each node's child nodes. As a result, the index basically also
a Merkle tree. Since the leaf nodes of the index contain the actual filesystem
data, the hashes of their parent index nodes thus cover all the file contents
and file metadata. When a file changes, the UBIFS index is updated accordingly
from the leaf nodes up to the root node including the master node. This process
can be hooked to recompute the hash only for each changed node at the same time.
Whenever a file is read, UBIFS can verify the hashes from each leaf node up to
the root node to ensure the node's integrity.
(借助 UBIFS 的游走树概念，它本身就已经做到只更新并持久化从叶节点直到完整 B+ 树根节点之间被更改的部分。这使我们能够在树的索引节点中附加一个对其子节点的哈希。这样，索引基本上也就成为一棵 Merkle 树。由于索引的叶节点包含实际的文件系统数据，其父索引节点的哈希因此覆盖了所有文件内容和文件元数据。当文件更改时，UBIFS 索引会相应地从叶节点向上更新到根节点(包括主节点)。这个过程可以被挂钩，以便同时仅为每个被更改的节点重新计算哈希。每当读取文件时，UBIFS 可以从每个叶节点向上到根节点验证哈希，以确保节点的完整性。)

To ensure the authenticity of the whole index, the UBIFS master node stores a
keyed hash (HMAC) over its own contents and a hash of the root node of the index
tree. As mentioned above, the master node is always written to the flash whenever
the index is persisted (ie. on index commit).
(为了确保整个索引的真实性，UBIFS 主节点存储了一个基于其自身内容和索引树根节点哈希的键控哈希(HMAC)。如上所述，每当索引被持久化(即索引提交)时，主节点总是会被写入闪存。)

Using this approach only UBIFS index nodes and the master node are changed to
include a hash. All other types of nodes will remain unchanged. This reduces
the storage overhead which is precious for users of UBIFS (ie. embedded
devices).
(采用这种方法，只有 UBIFS 索引节点和主节点被更改为包含哈希。所有其他类型的节点保持不变。这减少了存储开销，而存储空间对 UBIFS 的用户(即嵌入式设备)来说是宝贵的。)

::

                             +---------------+
                             |  Master Node  |
                             |    (hash)     |
                             +---------------+
                                     |
                                     v
                            +-------------------+
                            |  Index Node #1    |
                            |                   |
                            | branch0   branchn |
                            | (hash)    (hash)  |
                            +-------------------+
                               |    ...   |  (fanout: 8)
                               |          |
                       +-------+          +------+
                       |                         |
                       v                         v
            +-------------------+       +-------------------+
            |  Index Node #2    |       |  Index Node #3    |
            |                   |       |                   |
            | branch0   branchn |       | branch0   branchn |
            | (hash)    (hash)  |       | (hash)    (hash)  |
            +-------------------+       +-------------------+
                 |   ...                     |   ...   |
                 v                           v         v
               +-----------+         +----------+  +-----------+
               | Data Node |         | INO Node |  | DENT Node |
               +-----------+         +----------+  +-----------+


           Figure 3: Coverage areas of index node hash and master node HMAC



The most important part for robustness and power-cut safety is to atomically
persist the hash and file contents. Here the existing UBIFS logic for how
changed nodes are persisted is already designed for this purpose such that
UBIFS can safely recover if a power-cut occurs while persisting. Adding
hashes to index nodes does not change this since each hash will be persisted
atomically together with its respective node.
(鲁棒性和断电安全性的最重要部分是原子性地持久化哈希和文件内容。在这里，UBIFS 现有的关于如何持久化被更改节点的逻辑已经为此目的而设计，使得 UBIFS 可以在持久化期间发生断电时安全恢复。向索引节点添加哈希不会改变这一点，因为每个哈希都会与其各自的节点一起原子性地持久化。)


Journal Authentication
(日志认证)
~~~~~~~~~~~~~~~~~~~~~~

The journal is authenticated too. Since the journal is continuously written
it is necessary to also add authentication information frequently to the
journal so that in case of a powercut not too much data can't be authenticated.
This is done by creating a continuous hash beginning from the commit start node
over the previous reference nodes, the current reference node, and the bud
nodes. From time to time whenever it is suitable authentication nodes are added
between the bud nodes. This new node type contains a HMAC over the current state
of the hash chain. That way a journal can be authenticated up to the last
authentication node. The tail of the journal which may not have a authentication
node cannot be authenticated and is skipped during journal replay.
(日志也要进行认证。由于日志被持续写入，因此有必要频繁地向日志添加认证信息，以便在断电时不会有太多数据无法认证。这是通过创建一个从提交开始节点开始、跨越先前引用节点、当前引用节点和 bud 节点的连续哈希来实现的。时不时地，在合适的时候，认证节点会被添加到 bud 节点之间。这种新节点类型包含一个基于哈希链当前状态的 HMAC。这样，日志可以认证到最后一个认证节点为止。日志末尾可能没有认证节点的尾部无法认证，会在日志重放期间被跳过。)

We get this picture for journal authentication::

    ,,,,,,,,
    ,......,...........................................
    ,. CS  ,               hash1.----.           hash2.----.
    ,.  |  ,                    .    |hmac            .    |hmac
    ,.  v  ,                    .    v                .    v
    ,.REF#0,-> bud -> bud -> bud.-> auth -> bud -> bud.-> auth ...
    ,..|...,...........................................
    ,  |   ,
    ,  |   ,,,,,,,,,,,,,,,
    .  |            hash3,----.
    ,  |                 ,    |hmac
    ,  v                 ,    v
    , REF#1 -> bud -> bud,-> auth ...
    ,,,|,,,,,,,,,,,,,,,,,,
       v
      REF#2 -> ...
       |
       V
      ...

(我们得到如下日志认证示意图：)

Since the hash also includes the reference nodes an attacker cannot reorder or
skip any journal heads for replay. An attacker can only remove bud nodes or
reference nodes from the end of the journal, effectively rewinding the
filesystem at maximum back to the last commit.
(由于哈希也包括引用节点，攻击者无法重新排序或跳过任何日志头进行重放。攻击者只能从日志末尾移除 bud 节点或引用节点，实际上最多将文件系统回退到最后一次提交。)

The location of the log area is stored in the master node. Since the master
node is authenticated with a HMAC as described above, it is not possible to
tamper with that without detection. The size of the log area is specified when
the filesystem is created using `mkfs.ubifs` and stored in the superblock node.
To avoid tampering with this and other values stored there, a HMAC is added to
the superblock struct. The superblock node is stored in LEB 0 and is only
modified on feature flag or similar changes, but never on file changes.
(日志区的位置存储在主节点中。由于主节点如上所述使用 HMAC 进行认证，因此不可能在不被检测到的情况下篡改它。日志区的大小在创建文件系统时使用 `mkfs.ubifs` 指定，并存储在超级块节点中。为了防止篡改存储在其中的这个值和其他值，超级块结构中添加了一个 HMAC。超级块节点存储在 LEB 0 中，只在特性标志或类似更改时被修改，从不因文件更改而修改。)


LPT Authentication
(LPT 认证)
~~~~~~~~~~~~~~~~~~

The location of the LPT root node on the flash is stored in the UBIFS master
node. Since the LPT is written and read atomically on every commit, there is
no need to authenticate individual nodes of the tree. It suffices to
protect the integrity of the full LPT by a simple hash stored in the master
node. Since the master node itself is authenticated, the LPTs authenticity can
be verified by verifying the authenticity of the master node and comparing the
LTP hash stored there with the hash computed from the read on-flash LPT.
(LPT 根节点在闪存上的位置存储在 UBIFS 主节点中。由于 LPT 在每次提交时都以原子方式写入和读取，因此无需对树的各个节点进行认证。只需用存储在主节点中的一个简单哈希来保护整个 LPT 的完整性即可。由于主节点本身经过认证，可以通过验证主节点的真实性，并将其中存储的 LPT 哈希与从读取的片上 LPT 计算出的哈希进行比较，来验证 LPT 的真实性。)


Key Management
(密钥管理)
--------------

For simplicity, UBIFS authentication uses a single key to compute the HMACs
of superblock, master, commit start and reference nodes. This key has to be
available on creation of the filesystem (`mkfs.ubifs`) to authenticate the
superblock node. Further, it has to be available on mount of the filesystem
to verify authenticated nodes and generate new HMACs for changes.
(为简单起见，UBIFS 认证使用单个密钥来计算超级块、主节点、提交开始节点和引用节点的 HMAC。在创建文件系统(`mkfs.ubifs`)时必须提供此密钥，以认证超级块节点。此外，在挂载文件系统时也必须提供它，以验证已认证的节点并为更改生成新的 HMAC。)

UBIFS authentication is intended to operate side-by-side with UBIFS encryption
(fscrypt) to provide confidentiality and authenticity. Since UBIFS encryption
has a different approach of encryption policies per directory, there can be
multiple fscrypt master keys and there might be folders without encryption.
UBIFS authentication on the other hand has an all-or-nothing approach in the
sense that it either authenticates everything of the filesystem or nothing.
Because of this and because UBIFS authentication should also be usable without
encryption, it does not share the same master key with fscrypt, but manages
a dedicated authentication key.
(UBIFS 认证旨在与 UBIFS 加密(fscrypt)并行工作，以同时提供保密性和真实性。由于 UBIFS 加密采用按目录设置加密策略这一不同的方式，因此可能存在多个 fscrypt 主密钥，也可能存在未加密的目录。而 UBIFS 认证则采取全有或全无的方式，即要么认证文件系统的一切，要么什么都不认证。因此，并且由于 UBIFS 认证也应能在不加密的情况下使用，它与 fscrypt 不共享同一个主密钥，而是管理一个专用的认证密钥。)

The API for providing the authentication key has yet to be defined, but the
key can eg. be provided by userspace through a keyring similar to the way it
is currently done in fscrypt. It should however be noted that the current
fscrypt approach has shown its flaws and the userspace API will eventually
change [FSCRYPT-POLICY2].
(提供认证密钥的 API 尚待定义，但该密钥可以例如由用户空间通过与当前 fscrypt 的做法类似的密钥环方式提供。不过应当注意，当前的 fscrypt 方式已经暴露出其缺陷，用户空间 API 最终会发生变化 [FSCRYPT-POLICY2]。)

Nevertheless, it will be possible for a user to provide a single passphrase
or key in userspace that covers UBIFS authentication and encryption. This can
be solved by the corresponding userspace tools which derive a second key for
authentication in addition to the derived fscrypt master key used for
encryption.
(尽管如此，用户将可以在用户空间提供单个口令或密钥，同时涵盖 UBIFS 认证和加密。这可以由相应的用户空间工具来解决：除了用于加密的已派生 fscrypt 主密钥之外，再为认证派生第二个密钥。)

To be able to check if the proper key is available on mount, the UBIFS
superblock node will additionally store a hash of the authentication key. This
approach is similar to the approach proposed for fscrypt encryption policy v2
[FSCRYPT-POLICY2].
(为了能够检查挂载时正确的密钥是否可用，UBIFS 超级块节点还将额外存储认证密钥的哈希。该方法类似于针对 fscrypt 加密策略 v2 提出的方法 [FSCRYPT-POLICY2]。)


Future Extensions
(未来扩展)
=================

In certain cases where a vendor wants to provide an authenticated filesystem
image to customers, it should be possible to do so without sharing the secret
UBIFS authentication key. Instead, in addition the each HMAC a digital
signature could be stored where the vendor shares the public key alongside the
filesystem image. In case this filesystem has to be modified afterwards,
UBIFS can exchange all digital signatures with HMACs on first mount similar
to the way the IMA/EVM subsystem deals with such situations. The HMAC key
will then have to be provided beforehand in the normal way.
(在某些情况下，供应商希望向客户提供已认证的文件系统映像，应当可以在不共享机密的 UBIFS 认证密钥的情况下做到这一点。相反，除了每个 HMAC 之外，还可以存储一个数字签名，供应商将公钥与文件系统映像一起提供。如果此后必须修改该文件系统，UBIFS 可以在首次挂载时将所有数字签名替换为 HMAC，类似于 IMA/EVM 子系统处理此类情况的方式。之后，HMAC 密钥将必须以通常方式预先提供。)


References
(参考文献)
==========

[CRYPTSETUP2]        https://www.saout.de/pipermail/dm-crypt/2017-November/005745.html

[DMC-CBC-ATTACK]     https://www.jakoblell.com/blog/2013/12/22/practical-malleability-attack-against-cbc-encrypted-luks-partitions/

[DM-INTEGRITY]       https://www.kernel.org/doc/Documentation/device-mapper/dm-integrity.rst

[DM-VERITY]          https://www.kernel.org/doc/Documentation/device-mapper/verity.rst

[FSCRYPT-POLICY2]    https://lore.kernel.org/r/20171023214058.128121-1-ebiggers3@gmail.com/

[UBIFS-WP]           http://www.linux-mtd.infradead.org/doc/ubifs_whitepaper.pdf
