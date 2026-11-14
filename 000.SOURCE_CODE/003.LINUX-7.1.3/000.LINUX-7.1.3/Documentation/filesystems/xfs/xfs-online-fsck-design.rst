.. SPDX-License-Identifier: GPL-2.0
.. _xfs_online_fsck_design:

..
        Mapping of heading styles within this document:
        Heading 1 uses "====" above and below
        Heading 2 uses "===="
        Heading 3 uses "----"
        Heading 4 uses "````"
        Heading 5 uses "^^^^"
        Heading 6 uses "~~~~"
        Heading 7 uses "...."

        Sections are manually numbered because apparently that's what everyone
        does in the kernel.

======================
XFS Online Fsck Design
======================

(XFS 在线 fsck 设计)

This document captures the design of the online filesystem check feature for
XFS.
The purpose of this document is threefold:
(本文档记录了 XFS 在线文件系统检查功能的设计。本文档的目的有三：)

- To help kernel distributors understand exactly what the XFS online fsck
  feature is, and issues about which they should be aware.
  (帮助内核发行商确切了解 XFS 在线 fsck 功能是什么，以及他们应当注意哪些问题。)

- To help people reading the code to familiarize themselves with the relevant
  concepts and design points before they start digging into the code.
  (帮助阅读代码的人在深入代码之前熟悉相关概念和设计要点。)

- To help developers maintaining the system by capturing the reasons
  supporting higher level decision making.
  (通过记录支持更高层决策的理由，帮助维护系统的开发者。)

As the online fsck code is merged, the links in this document to topic branches
will be replaced with links to code.
(随着在线 fsck 代码被合并，本文档中指向主题分支的链接将被替换为指向代码的链接。)

This document is licensed under the terms of the GNU Public License, v2.
The primary author is Darrick J. Wong.
(本文档依据 GNU 公共许可证第 2 版的条款授权。主要作者是 Darrick J. Wong。)

This design document is split into seven parts.
Part 1 defines what fsck tools are and the motivations for writing a new one.
Parts 2 and 3 present a high level overview of how online fsck process works
and how it is tested to ensure correct functionality.
Part 4 discusses the user interface and the intended usage modes of the new
program.
Parts 5 and 6 show off the high level components and how they fit together, and
then present case studies of how each repair function actually works.
Part 7 sums up what has been discussed so far and speculates about what else
might be built atop online fsck.
(本设计文档分为七个部分。第 1 部分定义什么是 fsck 工具以及编写一个新工具的动机。第 2 和第 3 部分概要介绍在线 fsck 的工作过程，以及如何对其进行测试以确保功能正确。第 4 部分讨论新程序的用户界面和预期的使用模式。第 5 和第 6 部分展示高层组件及其如何组合在一起，然后给出每个修复函数实际如何工作的案例研究。第 7 部分总结到目前为止讨论的内容，并推测还可以在在线 fsck 之上构建哪些其他功能。)

.. contents:: Table of Contents
   :local:

1. What is a Filesystem Check?
(1. 什么是文件系统检查？)
==============================

A Unix filesystem has four main responsibilities:
(Unix 文件系统有四项主要职责：)

- Provide a hierarchy of names through which application programs can associate
  arbitrary blobs of data for any length of time,
  (提供一个名称层级结构，应用程序可以通过它在任意长的时间内关联任意的数据块，)

- Virtualize physical storage media across those names, and
  (跨这些名称虚拟化物理存储介质，以及)

- Retrieve the named data blobs at any time.
  (随时取回已命名的数据块。)

- Examine resource usage.
  (检查资源使用情况。)

Metadata directly supporting these functions (e.g. files, directories, space
mappings) are sometimes called primary metadata.
Secondary metadata (e.g. reverse mapping and directory parent pointers) support
operations internal to the filesystem, such as internal consistency checking
and reorganization.
Summary metadata, as the name implies, condense information contained in
primary metadata for performance reasons.
(直接支持这些功能的元数据(例如文件、目录、空间映射)有时被称为主元数据。二级元数据(例如反向映射和目录父指针)支持文件系统内部的操作，例如内部一致性检查和重组。摘要元数据，顾名思义，出于性能原因浓缩了主元数据中包含的信息。)

The filesystem check (fsck) tool examines all the metadata in a filesystem
to look for errors.
In addition to looking for obvious metadata corruptions, fsck also
cross-references different types of metadata records with each other to look
for inconsistencies.
People do not like losing data, so most fsck tools also contains some ability
to correct any problems found.
As a word of caution -- the primary goal of most Linux fsck tools is to restore
the filesystem metadata to a consistent state, not to maximize the data
recovered.
That precedent will not be challenged here.
(文件系统检查(fsck)工具检查文件系统中的所有元数据以查找错误。除了查找明显的元数据损坏之外，fsck 还会将不同类型的元数据记录相互交叉引用以查找不一致。人们不喜欢丢失数据，因此大多数 fsck 工具还包含一些纠正所发现问题的能力。需要提醒的是——大多数 Linux fsck 工具的首要目标是使文件系统元数据恢复到一致状态，而不是最大化恢复的数据。这一惯例在此不会被挑战。)

Filesystems of the 20th century generally lacked any redundancy in the ondisk
format, which means that fsck can only respond to errors by erasing files until
errors are no longer detected.
More recent filesystem designs contain enough redundancy in their metadata that
it is now possible to regenerate data structures when non-catastrophic errors
occur; this capability aids both strategies.
(20 世纪的文件系统在磁盘格式中普遍缺乏冗余，这意味着 fsck 只能通过删除文件来应对错误，直到不再检测到错误为止。较新的文件系统设计在元数据中包含足够的冗余，使得在发生非灾难性错误时重新生成数据结构成为可能；这一能力对两种策略都有帮助。)

+--------------------------------------------------------------------------+
| **Note**:                                                                |
| (注意：)                                                                    |
+--------------------------------------------------------------------------+
| System administrators avoid data loss by increasing the number of        |
| separate storage systems through the creation of backups; and they avoid |
| downtime by increasing the redundancy of each storage system through the |
| creation of RAID arrays.                                                 |
| fsck tools address only the first problem.                               |
| (系统管理员通过创建备份来增加独立存储系统的数量，从而避免数据丢失；并通过创建 RAID 阵列来提高每个存储系统的冗余度，从而避免停机。)    |
| (fsck 工具只解决第一个问题。)                                                       |
+--------------------------------------------------------------------------+

TLDR; Show Me the Code!
(TLDR；直接看代码！)
-----------------------

Kernel and userspace code has been fully merged as of October 2025.
(截至 2025 年 10 月，内核和用户空间代码已全部合并。)

Each kernel patchset adding an online repair function will use the same branch
name across the kernel, xfsprogs, and fstests git repos.
(每个添加在线修复功能的内核补丁集都会在内核、xfsprogs 和 fstests 的 git 仓库中使用相同的分支名称。)

Existing Tools
(现有工具)
--------------

The online fsck tool described here will be the third tool in the history of
XFS (on Linux) to check and repair filesystems.
Two programs precede it:
(这里描述的在线 fsck 工具将是 XFS(Linux 上)历史上第三个检查和修复文件系统的工具。在它之前已有两个程序：)

The first program, ``xfs_check``, was created as part of the XFS debugger
(``xfs_db``) and can only be used with unmounted filesystems.
It walks all metadata in the filesystem looking for inconsistencies in the
metadata, though it lacks any ability to repair what it finds.
Due to its high memory requirements and inability to repair things, this
program is now deprecated and will not be discussed further.
(第一个程序 ``xfs_check`` 是作为 XFS 调试器(``xfs_db``)的一部分创建的，只能在未挂载的文件系统上使用。它遍历文件系统中的所有元数据以查找元数据中的不一致，但缺乏修复所发现问题的任何能力。由于其内存需求高且无法修复问题，该程序现已被弃用，不再进一步讨论。)

The second program, ``xfs_repair``, was created to be faster and more robust
than the first program.
Like its predecessor, it can only be used with unmounted filesystems.
It uses extent-based in-memory data structures to reduce memory consumption,
and tries to schedule readahead IO appropriately to reduce I/O waiting time
while it scans the metadata of the entire filesystem.
The most important feature of this tool is its ability to respond to
inconsistencies in file metadata and directory tree by erasing things as needed
to eliminate problems.
Space usage metadata are rebuilt from the observed file metadata.
(第二个程序 ``xfs_repair`` 的创建目的是比第一个程序更快、更健壮。与其前身一样，它只能在未挂载的文件系统上使用。它使用基于区段的内存数据结构来减少内存消耗，并尝试适当地调度预读 IO 以减少扫描整个文件系统元数据时的 I/O 等待时间。此工具最重要的特性是它能够通过按需删除内容来响应文件元数据和目录树中的不一致，从而消除问题。空间使用元数据从观察到的文件元数据重建。)

Problem Statement
(问题陈述)
-----------------

The current XFS tools leave several problems unsolved:
(当前的 XFS 工具留下了若干未解决的问题：)

1. **User programs** suddenly **lose access** to the filesystem when unexpected
   shutdowns occur as a result of silent corruptions in the metadata.
   These occur **unpredictably** and often without warning.
   (当元数据中的静默损坏导致意外停机发生时，**用户程序**突然**失去对文件系统的访问**。这些停机的发生**不可预测**，而且常常没有任何警告。)

2. **Users** experience a **total loss of service** during the recovery period
   after an **unexpected shutdown** occurs.
   (**用户**在**意外停机**发生后的恢复期间会经历**完全的服务中断**。)

3. **Users** experience a **total loss of service** if the filesystem is taken
   offline to **look for problems** proactively.
   (如果文件系统被下线以主动**查找问题**，**用户**会经历**完全的服务中断**。)

4. **Data owners** cannot **check the integrity** of their stored data without
   reading all of it.
   This may expose them to substantial billing costs when a linear media scan
   performed by the storage system administrator might suffice.
   (**数据所有者**无法在不读取全部数据的情况下**检查其存储数据的完整性**。当存储系统管理员执行一次线性介质扫描可能就足够时，这可能使他们承担可观的计费成本。)

5. **System administrators** cannot **schedule** a maintenance window to deal
   with corruptions if they **lack the means** to assess filesystem health
   while the filesystem is online.
   (如果**系统管理员****缺乏手段**在文件系统在线时评估文件系统健康状况，他们就无法**安排**维护窗口来处理损坏。)

6. **Fleet monitoring tools** cannot **automate periodic checks** of filesystem
   health when doing so requires **manual intervention** and downtime.
   (当这样做需要**人工干预**和停机时，**机群监控工具**无法**自动执行周期性的文件系统健康检查**。)

7. **Users** can be tricked into **doing things they do not desire** when
   malicious actors **exploit quirks of Unicode** to place misleading names
   in directories.
   (当恶意行为者**利用 Unicode 的怪癖**在目录中放置误导性名称时，**用户**可能被诱骗**做出他们不希望做的事情**。)

Given this definition of the problems to be solved and the actors who would
benefit, the proposed solution is a third fsck tool that acts on a running
filesystem.
(基于对要解决的问题以及将从中受益的行动者的这一定义，提议的解决方案是第三个 fsck 工具，它作用于正在运行的文件系统。)

This new third program has three components: an in-kernel facility to check
metadata, an in-kernel facility to repair metadata, and a userspace driver
program to drive fsck activity on a live filesystem.
``xfs_scrub`` is the name of the driver program.
The rest of this document presents the goals and use cases of the new fsck
tool, describes its major design points in connection to those goals, and
discusses the similarities and differences with existing tools.
(这个新的第三个程序有三个组件：一个检查元数据的内核内设施，一个修复元数据的内核内设施，以及一个在活动文件系统上驱动 fsck 活动的用户空间驱动程序。``xfs_scrub`` 是驱动程序的名称。本文档的其余部分介绍新 fsck 工具的目标和用例，结合这些目标描述其主要设计点，并讨论与现有工具的异同。)

+--------------------------------------------------------------------------+
| **Note**:                                                                |
| (注意：)                                                                    |
+--------------------------------------------------------------------------+
| Throughout this document, the existing offline fsck tool can also be     |
| referred to by its current name "``xfs_repair``".                        |
| The userspace driver program for the new online fsck tool can be         |
| referred to as "``xfs_scrub``".                                          |
| The kernel portion of online fsck that validates metadata is called      |
| "online scrub", and portion of the kernel that fixes metadata is called  |
| "online repair".                                                         |
| (在本文档中，现有的离线 fsck 工具也可以用其当前的名称 "``xfs_repair``" 来指代。)                    |
| (新在线 fsck 工具的用户空间驱动程序可以称为 "``xfs_scrub``"。)                              |
| (在线 fsck 中校验元数据的内核部分称为 "online scrub"(在线检查)，)                            |
| (而内核中修复元数据的部分称为 "online repair"(在线修复)。)                                  |
+--------------------------------------------------------------------------+

The naming hierarchy is broken up into objects known as directories and files
and the physical space is split into pieces known as allocation groups.
Sharding enables better performance on highly parallel systems and helps to
contain the damage when corruptions occur.
The division of the filesystem into principal objects (allocation groups and
inodes) means that there are ample opportunities to perform targeted checks and
repairs on a subset of the filesystem.
(命名层级结构被分解为称为目录和文件的对象，物理空间被分割成称为分配组的片段。分片在高度并行的系统上能带来更好的性能，并有助于在损坏发生时控制损害范围。将文件系统划分为主体对象(分配组和 inode)意味着有充足的机会对文件系统的一个子集执行有针对性的检查和修复。)

While this is going on, other parts continue processing IO requests.
Even if a piece of filesystem metadata can only be regenerated by scanning the
entire system, the scan can still be done in the background while other file
operations continue.
(在此过程中，其他部分继续处理 IO 请求。即使某块文件系统元数据只能通过扫描整个系统来重新生成，该扫描仍然可以在后台进行，同时其他文件操作继续进行。)

In summary, online fsck takes advantage of resource sharding and redundant
metadata to enable targeted checking and repair operations while the system
is running.
This capability will be coupled to automatic system management so that
autonomous self-healing of XFS maximizes service availability.
(总之，在线 fsck 利用资源分片和冗余元数据，在系统运行时实现有针对性的检查和修复操作。这一能力将与自动系统管理相结合，使 XFS 的自主自愈最大化服务可用性。)

2. Theory of Operation
(2. 工作原理)
======================

Because it is necessary for online fsck to lock and scan live metadata objects,
online fsck consists of three separate code components.
The first is the userspace driver program ``xfs_scrub``, which is responsible
for identifying individual metadata items, scheduling work items for them,
reacting to the outcomes appropriately, and reporting results to the system
administrator.
The second and third are in the kernel, which implements functions to check
and repair each type of online fsck work item.
(由于在线 fsck 必须锁定并扫描活动的元数据对象，在线 fsck 由三个独立的代码组件组成。第一个是用户空间驱动程序 ``xfs_scrub``，它负责识别单个元数据项、为它们调度工作项、适当地对结果作出反应，并向系统管理员报告结果。第二和第三个组件在内核中，内核实现检查和修复每种类型的在线 fsck 工作项的函数。)

+------------------------------------------------------------------+
| **Note**:                                                        |
| (注意：)                                                            |
+------------------------------------------------------------------+
| For brevity, this document shortens the phrase "online fsck work |
| item" to "scrub item".                                           |
| (为简洁起见，本文档将 "online fsck work item" 一词简写为 "scrub item"。)         |
+------------------------------------------------------------------+

Scrub item types are delineated in a manner consistent with the Unix design
philosophy, which is to say that each item should handle one aspect of a
metadata structure, and handle it well.
(scrub 项类型的划分方式与 Unix 设计哲学一致，也就是说，每一项应处理元数据结构的一个方面，并把它处理好。)

Scope
(范围)
-----

In principle, online fsck should be able to check and to repair everything that
the offline fsck program can handle.
However, online fsck cannot be running 100% of the time, which means that
latent errors may creep in after a scrub completes.
If these errors cause the next mount to fail, offline fsck is the only
solution.
This limitation means that maintenance of the offline fsck tool will continue.
A second limitation of online fsck is that it must follow the same resource
sharing and lock acquisition rules as the regular filesystem.
This means that scrub cannot take *any* shortcuts to save time, because doing
so could lead to concurrency problems.
In other words, online fsck is not a complete replacement for offline fsck, and
a complete run of online fsck may take longer than offline fsck.
However, both of these limitations are acceptable tradeoffs to satisfy the
different motivations of online fsck, which are to **minimize system downtime**
and to **increase predictability of operation**.
(原则上，在线 fsck 应该能够检查和修复离线 fsck 程序所能处理的一切。然而，在线 fsck 不可能 100% 的时间都在运行，这意味着潜在的错误可能在一次 scrub 完成后潜入。如果这些错误导致下一次挂载失败，离线 fsck 是唯一的解决方案。这一限制意味着对离线 fsck 工具的维护将继续。在线 fsck 的第二个限制是它必须遵循与常规文件系统相同的资源共享和锁获取规则。这意味着 scrub 不能走*任何*节省时间的捷径，因为这样做可能导致并发问题。换句话说，在线 fsck 不是离线 fsck 的完全替代品，并且在线 fsck 的一次完整运行可能比离线 fsck 花费更长时间。然而，这两个限制都是可接受的折衷，以满足在线 fsck 的不同动机，即**最小化系统停机时间**和**提高运行的可预测性**。)

.. _scrubphases:

Phases of Work
(工作阶段)
--------------

The userspace driver program ``xfs_scrub`` splits the work of checking and
repairing an entire filesystem into seven phases.
Each phase concentrates on checking specific types of scrub items and depends
on the success of all previous phases.
The seven phases are as follows:
(用户空间驱动程序 ``xfs_scrub`` 将检查和修复整个文件系统的工作分为七个阶段。每个阶段专注于检查特定类型的 scrub 项，并依赖于之前所有阶段的成功。七个阶段如下：)

1. Collect geometry information about the mounted filesystem and computer,
   discover the online fsck capabilities of the kernel, and open the
   underlying storage devices.
   (收集已挂载文件系统和计算机的几何信息，发现内核的在线 fsck 能力，并打开底层存储设备。)

2. Check allocation group metadata, all realtime volume metadata, and all quota
   files.
   Each metadata structure is scheduled as a separate scrub item.
   If corruption is found in the inode header or inode btree and ``xfs_scrub``
   is permitted to perform repairs, then those scrub items are repaired to
   prepare for phase 3.
   Repairs are implemented by using the information in the scrub item to
   resubmit the kernel scrub call with the repair flag enabled; this is
   discussed in the next section.
   (修复通过使用 scrub 项中的信息重新提交启用了修复标志的内核 scrub 调用来实现；这将在下一节讨论。)
   Optimizations and all other repairs are deferred to phase 4.
   (检查分配组元数据、所有实时卷元数据和所有配额文件。每个元数据结构都被调度为一个单独的 scrub 项。如果在 inode 头或 inode btree 中发现损坏，且允许 ``xfs_scrub`` 执行修复，则修复这些 scrub 项以准备阶段 3。修复通过使用 scrub 项中的信息重新提交启用了修复标志的内核 scrub 调用来实现；这将在下一节讨论。优化和所有其他修复被推迟到阶段 4。)

3. Check all metadata of every file in the filesystem.
   Each metadata structure is also scheduled as a separate scrub item.
   If repairs are needed and ``xfs_scrub`` is permitted to perform repairs,
   and there were no problems detected during phase 2, then those scrub items
   are repaired immediately.
   Optimizations, deferred repairs, and unsuccessful repairs are deferred to
   phase 4.
   (检查文件系统中每个文件的所有元数据。每个元数据结构也被调度为一个单独的 scrub 项。如果需要修复、允许 ``xfs_scrub`` 执行修复，且在阶段 2 中没有检测到问题，则立即修复这些 scrub 项。优化、延迟修复和不成功的修复被推迟到阶段 4。)

4. All remaining repairs and scheduled optimizations are performed during this
   phase, if the caller permits them.
   Before starting repairs, the summary counters are checked and any necessary
   repairs are performed so that subsequent repairs will not fail the resource
   reservation step due to wildly incorrect summary counters.
   Unsuccessful repairs are requeued as long as forward progress on repairs is
   made somewhere in the filesystem.
   Free space in the filesystem is trimmed at the end of phase 4 if the
   filesystem is clean.
   (如果调用者允许，所有剩余的修复和已调度的优化都在此阶段执行。在开始修复之前，先检查摘要计数器并执行任何必要的修复，以便后续修复不会因严重不正确的摘要计数器而无法通过资源预留步骤。只要在文件系统的某处取得了修复上的进展，不成功的修复就会被重新排队。如果文件系统是干净的，则在阶段 4 结束时修剪文件系统中的空闲空间。)

5. By the start of this phase, all primary and secondary filesystem metadata
   must be correct.
   Summary counters such as the free space counts and quota resource counts
   are checked and corrected.
   Directory entry names and extended attribute names are checked for
   suspicious entries such as control characters or confusing Unicode sequences
   appearing in names.
   (到本阶段开始时，所有主元数据和二级文件系统元数据都必须是正确的。检查并纠正诸如空闲空间计数和配额资源计数之类的摘要计数器。检查目录项名称和扩展属性名称中是否出现可疑条目，例如名称中出现的控制字符或令人困惑的 Unicode 序列。)

6. If the caller asks for a media scan, read all allocated and written data
   file extents in the filesystem.
   The ability to use hardware-assisted data file integrity checking is new
   to online fsck; neither of the previous tools have this capability.
   If media errors occur, they will be mapped to the owning files and reported.
   (如果调用者要求介质扫描，则读取文件系统中所有已分配且已写入的数据文件区段。使用硬件辅助的数据文件完整性检查的能力对在线 fsck 来说是新的；之前的两个工具都没有此能力。如果发生介质错误，它们将被映射到所属文件并报告。)

7. Re-check the summary counters and presents the caller with a summary of
   space usage and file counts.
   (重新检查摘要计数器，并向调用者呈现空间使用情况和文件计数的摘要。)

This allocation of responsibilities will be :ref:`revisited <scrubcheck>`
later in this document.
(这一职责分配将在本文档后面 :ref:`重新讨论 <scrubcheck>`。)

Steps for Each Scrub Item
(每个 scrub 项的步骤)
-------------------------

The kernel scrub code uses a three-step strategy for checking and repairing
the one aspect of a metadata object represented by a scrub item:
(内核 scrub 代码使用一个三步策略来检查和修复 scrub 项所代表的元数据对象的一个方面：)

1. The scrub item of interest is checked for corruptions; opportunities for
   optimization; and for values that are directly controlled by the system
   administrator but look suspicious.
   If the item is not corrupt or does not need optimization, resource are
   released and the positive scan results are returned to userspace.
   If the item is corrupt or could be optimized but the caller does not permit
   this, resources are released and the negative scan results are returned to
   userspace.
   Otherwise, the kernel moves on to the second step.
   (检查所关注的 scrub 项是否存在损坏、优化机会，以及是否存在由系统管理员直接控制但看起来可疑的值。如果该项未损坏或不需要优化，则释放资源并将肯定(正常)的扫描结果返回给用户空间。如果该项已损坏或可以优化但调用者不允许，则释放资源并将否定(异常)的扫描结果返回给用户空间。否则，内核进入第二步。)

2. The repair function is called to rebuild the data structure.
   Repair functions generally choose rebuild a structure from other metadata
   rather than try to salvage the existing structure.
   If the repair fails, the scan results from the first step are returned to
   userspace.
   Otherwise, the kernel moves on to the third step.
   (调用修复函数来重建数据结构。修复函数通常选择从其他元数据重建结构，而不是试图抢救现有结构。如果修复失败，则将第一步的扫描结果返回给用户空间。否则，内核进入第三步。)

3. In the third step, the kernel runs the same checks over the new metadata
   item to assess the efficacy of the repairs.
   The results of the reassessment are returned to userspace.
   (在第三步中，内核对新的元数据项运行相同的检查，以评估修复的效果。重新评估的结果返回给用户空间。)

Classification of Metadata
(元数据分类)
--------------------------

Each type of metadata object (and therefore each type of scrub item) is
classified as follows:
(每种类型的元数据对象(因此每种类型的 scrub 项)按如下方式分类：)

Primary Metadata
(主元数据)
````````````````

Metadata structures in this category should be most familiar to filesystem
users either because they are directly created by the user or they index
objects created by the user
Most filesystem objects fall into this class:
(这一类别中的元数据结构对文件系统用户来说应该最为熟悉，因为它们要么由用户直接创建，要么为由用户创建的对象建立索引。大多数文件系统对象都属于这一类：)

- Free space and reference count information
  (空闲空间和引用计数信息)

- Inode records and indexes
  (inode 记录和索引)

- Storage mapping information for file data
  (文件数据的存储映射信息)

- Directories
  (目录)

- Extended attributes
  (扩展属性)

- Symbolic links
  (符号链接)

- Quota limits
  (配额限制)

Scrub obeys the same rules as regular filesystem accesses for resource and lock
acquisition.
(scrub 在资源和锁获取方面遵循与常规文件系统访问相同的规则。)

Primary metadata objects are the simplest for scrub to process.
The principal filesystem object (either an allocation group or an inode) that
owns the item being scrubbed is locked to guard against concurrent updates.
The check function examines every record associated with the type for obvious
errors and cross-references healthy records against other metadata to look for
inconsistencies.
Repairs for this class of scrub item are simple, since the repair function
starts by holding all the resources acquired in the previous step.
The repair function scans available metadata as needed to record all the
observations needed to complete the structure.
Next, it stages the observations in a new ondisk structure and commits it
atomically to complete the repair.
Finally, the storage from the old data structure are carefully reaped.
(主元数据对象是 scrub 最容易处理的。拥有被 scrub 项的主体文件系统对象(分配组或 inode)会被锁定，以防止并发更新。检查函数检查与该类型关联的每条记录是否存在明显错误，并将健康的记录与其他元数据交叉引用以查找不一致。这类 scrub 项的修复很简单，因为修复函数从持有上一步获取的所有资源开始。修复函数根据需要扫描可用的元数据，记录完成该结构所需的所有观察结果。接下来，它将观察结果暂存在一个新的磁盘结构中并原子地提交，以完成修复。最后，仔细回收旧数据结构占用的存储。)

Because ``xfs_scrub`` locks a primary object for the duration of the repair,
this is effectively an offline repair operation performed on a subset of the
filesystem.
This minimizes the complexity of the repair code because it is not necessary to
handle concurrent updates from other threads, nor is it necessary to access
any other part of the filesystem.
As a result, indexed structures can be rebuilt very quickly, and programs
trying to access the damaged structure will be blocked until repairs complete.
The only infrastructure needed by the repair code are the staging area for
observations and a means to write new structures to disk.
Despite these limitations, the advantage that online repair holds is clear:
targeted work on individual shards of the filesystem avoids total loss of
service.
(因为 ``xfs_scrub`` 在修复期间锁定一个主体对象，这实际上是在文件系统的一个子集上执行的离线修复操作。这使修复代码的复杂度最小化，因为无需处理来自其他线程的并发更新，也无需访问文件系统的任何其他部分。因此，索引结构可以非常快速地重建，而试图访问受损结构的程序将被阻塞，直到修复完成。修复代码唯一需要的基础设施是用于观察结果的暂存区，以及将新结构写入磁盘的手段。尽管有这些限制，在线修复所拥有的优势是明确的：对文件系统各个分片的针对性工作避免了完全的服务中断。)

This mechanism is described in section 2.1 ("Off-Line Algorithm") of
V. Srinivasan and M. J. Carey, `"Performance of On-Line Index Construction
Algorithms" <https://minds.wisconsin.edu/bitstream/handle/1793/59524/TR1047.pdf>`_,
*Extending Database Technology*, pp. 293-309, 1992.
(这一机制在 V. Srinivasan 和 M. J. Carey 的 `"Performance of On-Line Index Construction Algorithms" <https://minds.wisconsin.edu/bitstream/handle/1793/59524/TR1047.pdf>`_(*Extending Database Technology*，1992 年，第 293-309 页)第 2.1 节("Off-Line Algorithm")中有所描述。)

Most primary metadata repair functions stage their intermediate results in an
in-memory array prior to formatting the new ondisk structure, which is very
similar to the list-based algorithm discussed in section 2.3 ("List-Based
Algorithms") of Srinivasan.
However, any data structure builder that maintains a resource lock for the
duration of the repair is *always* an offline algorithm.
(大多数主元数据修复函数在格式化新的磁盘结构之前，将其中间结果暂存在内存数组中，这非常类似于 Srinivasan 论文第 2.3 节("List-Based Algorithms")讨论的基于列表的算法。然而，任何在修复期间持有资源锁的数据结构构建器*总是*离线算法。)

.. _secondary_metadata:

Secondary Metadata
(二级元数据)
``````````````````

Metadata structures in this category reflect records found in primary metadata,
but are only needed for online fsck or for reorganization of the filesystem.
(这一类别中的元数据结构反映在主元数据中找到的记录，但只有在线 fsck 或文件系统重组才需要它们。)

Secondary metadata include:
(二级元数据包括：)

- Reverse mapping information
  (反向映射信息)

- Directory parent pointers
  (目录父指针)

This class of metadata is difficult for scrub to process because scrub attaches
to the secondary object but needs to check primary metadata, which runs counter
to the usual order of resource acquisition.
Frequently, this means that full filesystems scans are necessary to rebuild the
metadata.
Check functions can be limited in scope to reduce runtime.
Repairs, however, require a full scan of primary metadata, which can take a
long time to complete.
Under these conditions, ``xfs_scrub`` cannot lock resources for the entire
duration of the repair.
(这类元数据对 scrub 来说难以处理，因为 scrub 附着在二级对象上却需要检查主元数据，这与通常的资源获取顺序相反。通常，这意味着需要全文件系统扫描来重建元数据。检查函数可以限制范围以减少运行时间。然而，修复需要对主元数据进行全面扫描，这可能需要很长时间才能完成。在这些条件下，``xfs_scrub`` 无法在修复的整个期间锁定资源。)

Instead, repair functions set up an in-memory staging structure to store
observations.
Depending on the requirements of the specific repair function, the staging
index will either have the same format as the ondisk structure or a design
specific to that repair function.
The next step is to release all locks and start the filesystem scan.
When the repair scanner needs to record an observation, the staging data are
locked long enough to apply the update.
While the filesystem scan is in progress, the repair function hooks the
filesystem so that it can apply pending filesystem updates to the staging
information.
Once the scan is done, the owning object is re-locked, the live data is used to
write a new ondisk structure, and the repairs are committed atomically.
The hooks are disabled and the staging area is freed.
Finally, the storage from the old data structure are carefully reaped.
(相反，修复函数建立一个内存中的暂存结构来存储观察结果。根据特定修复函数的要求，暂存索引要么采用与磁盘结构相同的格式，要么采用该修复函数专用的设计。下一步是释放所有锁并开始文件系统扫描。当修复扫描器需要记录观察结果时，暂存数据被锁定足够长的时间以应用更新。在文件系统扫描进行期间，修复函数挂钩文件系统，以便能够将待处理的文件系统更新应用到暂存信息。一旦扫描完成，拥有该对象的对象被重新锁定，使用实时数据写入新的磁盘结构，并以原子方式提交修复。挂钩被禁用，暂存区被释放。最后，仔细回收旧数据结构占用的存储。)

Introducing concurrency helps online repair avoid various locking problems, but
comes at a high cost to code complexity.
Live filesystem code has to be hooked so that the repair function can observe
updates in progress.
The staging area has to become a fully functional parallel structure so that
updates can be merged from the hooks.
Finally, the hook, the filesystem scan, and the inode locking model must be
sufficiently well integrated that a hook event can decide if a given update
should be applied to the staging structure.
(引入并发有助于在线修复避免各种锁问题，但代码复杂度代价很高。必须挂钩活动文件系统代码，以便修复函数能够观察正在进行的更新。暂存区必须成为一个功能齐全的并行结构，以便能够从挂钩合并更新。最后，挂钩、文件系统扫描和 inode 锁定模型必须充分良好地集成，使得挂钩事件能够决定某个给定的更新是否应应用于暂存结构。)

In theory, the scrub implementation could apply these same techniques for
primary metadata, but doing so would make it massively more complex and less
performant.
Programs attempting to access the damaged structures are not blocked from
operation, which may cause application failure or an unplanned filesystem
shutdown.
(理论上，scrub 实现可以对主元数据应用这些相同的技术，但这样做会使其复杂度大幅增加而性能下降。试图访问受损结构的程序不会被阻止运行，这可能导致应用程序失败或计划外的文件系统停机。)

Inspiration for the secondary metadata repair strategy was drawn from section
2.4 of Srinivasan above, and sections 2 ("NSF: Index Build Without Side-File")
and 3.1.1 ("Duplicate Key Insert Problem") in C. Mohan, `"Algorithms for
Creating Indexes for Very Large Tables Without Quiescing Updates"
<https://dl.acm.org/doi/10.1145/130283.130337>`_, 1992.
(二级元数据修复策略的灵感来自上文 Srinivasan 论文的第 2.4 节，以及 C. Mohan 的 `"Algorithms for Creating Indexes for Very Large Tables Without Quiescing Updates" <https://dl.acm.org/doi/10.1145/130283.130337>`_(1992 年)中的第 2 节("NSF: Index Build Without Side-File")和第 3.1.1 节("Duplicate Key Insert Problem")。)

The sidecar index mentioned above bears some resemblance to the side file
method mentioned in Srinivasan and Mohan.
Their method consists of an index builder that extracts relevant record data to
build the new structure as quickly as possible; and an auxiliary structure that
captures all updates that would be committed to the index by other threads were
the new index already online.
After the index building scan finishes, the updates recorded in the side file
are applied to the new index.
To avoid conflicts between the index builder and other writer threads, the
builder maintains a publicly visible cursor that tracks the progress of the
scan through the record space.
To avoid duplication of work between the side file and the index builder, side
file updates are elided when the record ID for the update is greater than the
cursor position within the record ID space.
(上文提到的边车索引与 Srinivasan 和 Mohan 提到的边文件(side file)方法有几分相似。他们的方法包括一个索引构建器，它尽快提取相关记录数据以构建新结构；以及一个辅助结构，它捕获假如新索引已经在线时其他线程会提交给索引的所有更新。在索引构建扫描完成后，记录在边文件中的更新被应用到新索引。为避免索引构建器与其他写线程之间的冲突，构建器维护一个公开可见的游标，跟踪扫描在记录空间中的进度。为避免边文件与索引构建器之间的重复工作，当更新的记录 ID 大于游标在记录 ID 空间中的位置时，边文件更新会被省略。)

To minimize changes to the rest of the codebase, XFS online repair keeps the
replacement index hidden until it's completely ready to go.
In other words, there is no attempt to expose the keyspace of the new index
while repair is running.
The complexity of such an approach would be very high and perhaps more
appropriate to building *new* indices.
(为了尽量减少对代码库其余部分的更改，XFS 在线修复在替换索引完全就绪之前将其隐藏。换句话说，在修复运行期间，不尝试暴露新索引的键空间。这种方法的复杂度会非常高，也许更适合构建*全新*的索引。)

**Future Work Question**: Can the full scan and live update code used to
facilitate a repair also be used to implement a comprehensive check?
(**未来工作问题**：用于辅助修复的全量扫描和实时更新代码是否也可以用来实现全面的检查？)

*Answer*: In theory, yes.  Check would be much stronger if each scrub function
employed these live scans to build a shadow copy of the metadata and then
compared the shadow records to the ondisk records.
However, doing that is a fair amount more work than what the checking functions
do now.
The live scans and hooks were developed much later.
That in turn increases the runtime of those scrub functions.
(*回答*：理论上可以。如果每个 scrub 函数都采用这些实时扫描来构建元数据的影子副本，然后将影子记录与磁盘记录进行比较，检查会强大得多。然而，这样做比现在的检查函数要多做相当多的工作。实时扫描和挂钩是很久以后才开发出来的。这反过来又会增加那些 scrub 函数的运行时间。)

Summary Information
(摘要信息)
```````````````````

Metadata structures in this last category summarize the contents of primary
metadata records.
These are often used to speed up resource usage queries, and are many times
smaller than the primary metadata which they represent.
(最后一个类别中的元数据结构概括主元数据记录的内容。它们通常用于加速资源使用查询，并且比它们所代表的主元数据小很多倍。)

Examples of summary information include:
(摘要信息的示例包括：)

- Summary counts of free space and inodes
  (空闲空间和 inode 的摘要计数)

- File link counts from directories
  (来自目录的文件链接计数)

- Quota resource usage counts
  (配额资源使用计数)

Check and repair require full filesystem scans, but resource and lock
acquisition follow the same paths as regular filesystem accesses.
(检查和修复需要全文件系统扫描，但资源和锁获取遵循与常规文件系统访问相同的路径。)

The superblock summary counters have special requirements due to the underlying
implementation of the incore counters, and will be treated separately.
Check and repair of the other types of summary counters (quota resource counts
and file link counts) employ the same filesystem scanning and hooking
techniques as outlined above, but because the underlying data are sets of
integer counters, the staging data need not be a fully functional mirror of the
ondisk structure.
(由于内存中计数器的底层实现，超级块摘要计数器有特殊要求，将单独处理。其他类型的摘要计数器(配额资源计数和文件链接计数)的检查和修复采用与上文概述相同的文件系统扫描和挂钩技术，但由于底层数据是整数计数器的集合，暂存数据不必是磁盘结构的功能齐全的镜像。)

Inspiration for quota and file link count repair strategies were drawn from
sections 2.12 ("Online Index Operations") through 2.14 ("Incremental View
Maintenance") of G.  Graefe, `"Concurrent Queries and Updates in Summary Views
and Their Indexes"
<http://www.odbms.org/wp-content/uploads/2014/06/Increment-locks.pdf>`_, 2011.
(配额和文件链接计数修复策略的灵感来自 G. Graefe 的 `"Concurrent Queries and Updates in Summary Views and Their Indexes" <http://www.odbms.org/wp-content/uploads/2014/06/Increment-locks.pdf>`_(2011 年)的第 2.12 节("Online Index Operations")至第 2.14 节("Incremental View Maintenance")。)

Since quotas are non-negative integer counts of resource usage, online
quotacheck can use the incremental view deltas described in section 2.14 to
track pending changes to the block and inode usage counts in each transaction,
and commit those changes to a dquot side file when the transaction commits.
Delta tracking is necessary for dquots because the index builder scans inodes,
whereas the data structure being rebuilt is an index of dquots.
Link count checking combines the view deltas and commit step into one because
it sets attributes of the objects being scanned instead of writing them to a
separate data structure.
Each online fsck function will be discussed as case studies later in this
document.
(由于配额是资源使用的非负整数计数，在线配额检查可以使用第 2.14 节描述的增量视图增量来跟踪每个事务中对块和 inode 使用计数的待定更改，并在事务提交时将这些更改提交到 dquot 边文件。dquot 需要增量跟踪，因为索引构建器扫描的是 inode，而正在重建的数据结构是 dquot 的索引。链接计数检查将视图增量和提交步骤合而为一，因为它设置被扫描对象的属性，而不是将它们写入单独的数据结构。本文档稍后将作为案例研究讨论每个在线 fsck 函数。)

Risk Management
(风险管理)
---------------

During the development of online fsck, several risk factors were identified
that may make the feature unsuitable for certain distributors and users.
Steps can be taken to mitigate or eliminate those risks, though at a cost to
functionality.
(在在线 fsck 的开发过程中，识别出若干风险因素，它们可能使该功能不适合某些发行商和用户。可以采取措施减轻或消除这些风险，但要以牺牲功能为代价。)

- **Decreased performance**: Adding metadata indices to the filesystem
  increases the time cost of persisting changes to disk, and the reverse space
  mapping and directory parent pointers are no exception.
  System administrators who require the maximum performance can disable the
  reverse mapping features at format time, though this choice dramatically
  reduces the ability of online fsck to find inconsistencies and repair them.
  (**性能下降**：向文件系统添加元数据索引会增加将更改持久化到磁盘的时间成本，反向空间映射和目录父指针也不例外。需要最高性能的系统管理员可以在格式化时禁用反向映射功能，尽管这一选择会大幅降低在线 fsck 发现不一致并修复它们的能力。)

- **Incorrect repairs**: As with all software, there might be defects in the
  software that result in incorrect repairs being written to the filesystem.
  Systematic fuzz testing (detailed in the next section) is employed by the
  authors to find bugs early, but it might not catch everything.
  The kernel build system provides Kconfig options (``CONFIG_XFS_ONLINE_SCRUB``
  and ``CONFIG_XFS_ONLINE_REPAIR``) to enable distributors to choose not to
  accept this risk.
  The xfsprogs build system has a configure option (``--enable-scrub=no``) that
  disables building of the ``xfs_scrub`` binary, though this is not a risk
  mitigation if the kernel functionality remains enabled.
  (**错误的修复**：与所有软件一样，软件中可能存在导致将错误修复写入文件系统的缺陷。作者采用系统化的模糊测试(下一节详述)来尽早发现 bug，但它可能无法捕获所有问题。内核构建系统提供了 Kconfig 选项(``CONFIG_XFS_ONLINE_SCRUB`` 和 ``CONFIG_XFS_ONLINE_REPAIR``)，使发行商可以选择不接受这一风险。xfsprogs 构建系统有一个配置选项(``--enable-scrub=no``)可以禁用 ``xfs_scrub`` 二进制文件的构建，不过如果内核功能仍然启用，这并不能减轻风险。)

- **Inability to repair**: Sometimes, a filesystem is too badly damaged to be
  repairable.
  If the keyspaces of several metadata indices overlap in some manner but a
  coherent narrative cannot be formed from records collected, then the repair
  fails.
  To reduce the chance that a repair will fail with a dirty transaction and
  render the filesystem unusable, the online repair functions have been
  designed to stage and validate all new records before committing the new
  structure.
  (**无法修复**：有时，文件系统受损严重到无法修复。如果若干元数据索引的键空间以某种方式重叠，但无法从收集的记录中形成连贯的叙述，则修复失败。为了降低修复因脏事务而失败并导致文件系统不可用的可能性，在线修复函数被设计为在提交新结构之前暂存并验证所有新记录。)

- **Misbehavior**: Online fsck requires many privileges -- raw IO to block
  devices, opening files by handle, ignoring Unix discretionary access control,
  and the ability to perform administrative changes.
  Running this automatically in the background scares people, so the systemd
  background service is configured to run with only the privileges required.
  Obviously, this cannot address certain problems like the kernel crashing or
  deadlocking, but it should be sufficient to prevent the scrub process from
  escaping and reconfiguring the system.
  The cron job does not have this protection.
  (**不当行为**：在线 fsck 需要许多特权——对块设备的原始 IO、通过句柄打开文件、忽略 Unix 自主访问控制，以及执行管理性更改的能力。在后台自动运行这些会让人不安，因此 systemd 后台服务被配置为仅以所需的特权运行。显然，这无法解决诸如内核崩溃或死锁之类的某些问题，但应该足以防止 scrub 进程逃逸并重新配置系统。cron 任务没有这种保护。)

- **Fuzz Kiddiez**: There are many people now who seem to think that running
  automated fuzz testing of ondisk artifacts to find mischievous behavior and
  spraying exploit code onto the public mailing list for instant zero-day
  disclosure is somehow of some social benefit.
  In the view of this author, the benefit is realized only when the fuzz
  operators help to **fix** the flaws, but this opinion apparently is not
  widely shared among security "researchers".
  The XFS maintainers' continuing ability to manage these events presents an
  ongoing risk to the stability of the development process.
  Automated testing should front-load some of the risk while the feature is
  considered EXPERIMENTAL.
  (**模糊测试小子**：现在有很多人似乎认为，对磁盘制品运行自动化模糊测试以发现恶意行为，并将利用代码喷洒到公共邮件列表上以实现即时零日披露，不知为何具有某种社会效益。在本作者看来，只有当模糊测试操作者帮助**修复**缺陷时，这种效益才会实现，但这一观点显然没有在安全"研究人员"中得到广泛认同。XFS 维护者持续管理这些事件的能力对开发过程的稳定性构成持续风险。在该功能仍被视为 EXPERIMENTAL(实验性)期间，自动化测试应当前置承担部分风险。)

Many of these risks are inherent to software programming.
Despite this, it is hoped that this new functionality will prove useful in
reducing unexpected downtime.
(这些风险中有许多是软件编程所固有的。尽管如此，希望这一新功能能在减少意外停机方面证明是有用的。)

3. Testing Plan
(3. 测试计划)
===============

As stated before, fsck tools have three main goals:
(如前所述，fsck 工具有三个主要目标：)

1. Detect inconsistencies in the metadata;
   (检测元数据中的不一致；)

2. Eliminate those inconsistencies; and
   (消除这些不一致；以及)

3. Minimize further loss of data.
   (最小化进一步的数据丢失。)

Demonstrations of correct operation are necessary to build users' confidence
that the software behaves within expectations.
Unfortunately, it was not really feasible to perform regular exhaustive testing
of every aspect of a fsck tool until the introduction of low-cost virtual
machines with high-IOPS storage.
With ample hardware availability in mind, the testing strategy for the online
fsck project involves differential analysis against the existing fsck tools and
systematic testing of every attribute of every type of metadata object.
Testing can be split into four major categories, as discussed below.
(对正确操作的演示是建立用户对软件在预期范围内运行的信心所必需的。不幸的是，在引入配备高 IOPS 存储的低成本虚拟机之前，对 fsck 工具的每个方面进行定期穷举测试并不真正可行。考虑到硬件的充足可用性，在线 fsck 项目的测试策略包括与现有 fsck 工具的差分分析，以及对每种类型的元数据对象的每个属性的系统化测试。测试可以分为四个主要类别，如下所述。)

Integrated Testing with fstests
(与 fstests 的集成测试)
-------------------------------

The primary goal of any free software QA effort is to make testing as
inexpensive and widespread as possible to maximize the scaling advantages of
community.
In other words, testing should maximize the breadth of filesystem configuration
scenarios and hardware setups.
This improves code quality by enabling the authors of online fsck to find and
fix bugs early, and helps developers of new features to find integration
issues earlier in their development effort.
(任何自由软件质量保证工作的首要目标是使测试尽可能廉价和普及，以最大化社区的规模优势。换句话说，测试应最大限度地覆盖文件系统配置场景和硬件设置的广度。这通过使在线 fsck 的作者能够尽早发现并修复 bug 来提高代码质量，并帮助新功能的开发者在开发工作的早期发现集成问题。)

The Linux filesystem community shares a common QA testing suite,
`fstests <https://git.kernel.org/pub/scm/fs/xfs/xfstests-dev.git/>`_, for
functional and regression testing.
Even before development work began on online fsck, fstests (when run on XFS)
would run both the ``xfs_check`` and ``xfs_repair -n`` commands on the test and
scratch filesystems between each test.
This provides a level of assurance that the kernel and the fsck tools stay in
alignment about what constitutes consistent metadata.
During development of the online checking code, fstests was modified to run
``xfs_scrub -n`` between each test to ensure that the new checking code
produces the same results as the two existing fsck tools.
(Linux 文件系统社区共享一个通用的质量保证测试套件 `fstests <https://git.kernel.org/pub/scm/fs/xfs/xfstests-dev.git/>`_，用于功能和回归测试。早在在线 fsck 的开发工作开始之前，fstests(在 XFS 上运行时)就会在每个测试之间对测试文件系统和临时文件系统运行 ``xfs_check`` 和 ``xfs_repair -n`` 两个命令。这提供了一定程度的保证，即内核与 fsck 工具对什么构成一致的元数据保持一致。在在线检查代码的开发期间，fstests 被修改为在每个测试之间运行 ``xfs_scrub -n``，以确保新的检查代码产生与两个现有 fsck 工具相同的结果。)

To start development of online repair, fstests was modified to run
``xfs_repair`` to rebuild the filesystem's metadata indices between tests.
This ensures that offline repair does not crash, leave a corrupt filesystem
after it exists, or trigger complaints from the online check.
This also established a baseline for what can and cannot be repaired offline.
To complete the first phase of development of online repair, fstests was
modified to be able to run ``xfs_scrub`` in a "force rebuild" mode.
This enables a comparison of the effectiveness of online repair as compared to
the existing offline repair tools.
(为了开始在线修复的开发，fstests 被修改为在测试之间运行 ``xfs_repair`` 来重建文件系统的元数据索引。这确保离线修复不会崩溃、在退出后留下损坏的文件系统，或引发在线检查的抱怨。这也建立了什么能修、什么不能离线修复的基线。为了完成在线修复开发的第一阶段，fstests 被修改为能够以"强制重建"模式运行 ``xfs_scrub``。这使得可以比较在线修复与现有离线修复工具的有效性。)

General Fuzz Testing of Metadata Blocks
(元数据块的通用模糊测试)
---------------------------------------

XFS benefits greatly from having a very robust debugging tool, ``xfs_db``.
(XFS 得益于拥有一个非常健壮的调试工具 ``xfs_db``。)

Before development of online fsck even began, a set of fstests were created
to test the rather common fault that entire metadata blocks get corrupted.
This required the creation of fstests library code that can create a filesystem
containing every possible type of metadata object.
Next, individual test cases were created to create a test filesystem, identify
a single block of a specific type of metadata object, trash it with the
existing ``blocktrash`` command in ``xfs_db``, and test the reaction of a
particular metadata validation strategy.
(早在在线 fsck 开发开始之前，就创建了一组 fstests 来测试相当常见的故障——整个元数据块被损坏。这要求创建能够创建包含每种可能类型的元数据对象的文件系统的 fstests 库代码。接下来，创建了单独的测试用例来创建测试文件系统、识别特定类型元数据对象的单个块、用 ``xfs_db`` 中现有的 ``blocktrash`` 命令破坏它，并测试特定元数据校验策略的反应。)

This earlier test suite enabled XFS developers to test the ability of the
in-kernel validation functions and the ability of the offline fsck tool to
detect and eliminate the inconsistent metadata.
This part of the test suite was extended to cover online fsck in exactly the
same manner.
(这个较早的测试套件使 XFS 开发者能够测试内核内校验函数的能力以及离线 fsck 工具检测和消除不一致元数据的能力。测试套件的这一部分以完全相同的方式扩展到覆盖在线 fsck。)

In other words, for a given fstests filesystem configuration:
(换句话说，对于一个给定的 fstests 文件系统配置：)

* For each metadata object existing on the filesystem:
  (对于文件系统上存在的每个元数据对象：)

  * Write garbage to it
    (向其写入垃圾数据)

  * Test the reactions of:
    (测试以下各项的反应：)

    1. The kernel verifiers to stop obviously bad metadata
       (内核校验器阻止明显坏的元数据)
    2. Offline repair (``xfs_repair``) to detect and fix
       (离线修复(``xfs_repair``)检测并修复)
    3. Online repair (``xfs_scrub``) to detect and fix
       (在线修复(``xfs_scrub``)检测并修复)

Targeted Fuzz Testing of Metadata Records
(元数据记录的定向模糊测试)
-----------------------------------------

The testing plan for online fsck includes extending the existing fs testing
infrastructure to provide a much more powerful facility: targeted fuzz testing
of every metadata field of every metadata object in the filesystem.
``xfs_db`` can modify every field of every metadata structure in every
block in the filesystem to simulate the effects of memory corruption and
software bugs.
Given that fstests already contains the ability to create a filesystem
containing every metadata format known to the filesystem, ``xfs_db`` can be
used to perform exhaustive fuzz testing!
(在线 fsck 的测试计划包括扩展现有的文件系统测试基础设施，以提供一个强大得多的设施：对文件系统中每个元数据对象的每个元数据字段进行定向模糊测试。``xfs_db`` 可以修改文件系统中每个块的每个元数据结构的每个字段，以模拟内存损坏和软件 bug 的影响。鉴于 fstests 已经具备创建包含文件系统已知的每种元数据格式的文件系统的能力，``xfs_db`` 可以用来执行穷举模糊测试！)

For a given fstests filesystem configuration:
(对于一个给定的 fstests 文件系统配置：)

* For each metadata object existing on the filesystem...
  (对于文件系统上存在的每个元数据对象……)

  * For each record inside that metadata object...
    (对于该元数据对象内的每条记录……)

    * For each field inside that record...
      (对于该记录内的每个字段……)

      * For each conceivable type of transformation that can be applied to a bit field...
        (对于可应用于位字段的每种可想到的变换类型……)

        1. Clear all bits
           (清除所有位)
        2. Set all bits
           (设置所有位)
        3. Toggle the most significant bit
           (翻转最高有效位)
        4. Toggle the middle bit
           (翻转中间位)
        5. Toggle the least significant bit
           (翻转最低有效位)
        6. Add a small quantity
           (增加一个小量)
        7. Subtract a small quantity
           (减去一个小量)
        8. Randomize the contents
           (随机化内容)

        * ...test the reactions of:
          (……测试以下各项的反应：)

          1. The kernel verifiers to stop obviously bad metadata
             (内核校验器阻止明显坏的元数据)
          2. Offline checking (``xfs_repair -n``)
             (离线检查(``xfs_repair -n``))
          3. Offline repair (``xfs_repair``)
             (离线修复(``xfs_repair``))
          4. Online checking (``xfs_scrub -n``)
             (在线检查(``xfs_scrub -n``))
          5. Online repair (``xfs_scrub``)
             (在线修复(``xfs_scrub``))
          6. Both repair tools (``xfs_scrub`` and then ``xfs_repair`` if online repair doesn't succeed)
             (两种修复工具(``xfs_scrub``，如果在线修复不成功则接着 ``xfs_repair``))

This is quite the combinatoric explosion!
(这确实是相当大的组合爆炸！)

Fortunately, having this much test coverage makes it easy for XFS developers to
check the responses of XFS' fsck tools.
Since the introduction of the fuzz testing framework, these tests have been
used to discover incorrect repair code and missing functionality for entire
classes of metadata objects in ``xfs_repair``.
The enhanced testing was used to finalize the deprecation of ``xfs_check`` by
confirming that ``xfs_repair`` could detect at least as many corruptions as
the older tool.
(幸运的是，拥有如此多的测试覆盖使 XFS 开发者能够方便地检查 XFS 的 fsck 工具的反应。自从引入模糊测试框架以来，这些测试已被用于发现 ``xfs_repair`` 中针对整类元数据对象的错误修复代码和缺失功能。增强后的测试通过确认 ``xfs_repair`` 能够检测到至少与旧工具一样多的损坏，从而最终确定了 ``xfs_check`` 的弃用。)

These tests have been very valuable for ``xfs_scrub`` in the same ways -- they
allow the online fsck developers to compare online fsck against offline fsck,
and they enable XFS developers to find deficiencies in the code base.
(这些测试以同样的方式对 ``xfs_scrub`` 非常有价值——它们让在线 fsck 开发者能够将在线 fsck 与离线 fsck 进行比较，并使 XFS 开发者能够发现代码库中的不足。)

Proposed patchsets include
`fuzzing baselines
<https://git.kernel.org/pub/scm/linux/kernel/git/djwong/xfstests-dev.git/log/?h=fuzz-baseline>`_.
(提议的补丁集包括 `fuzzing baselines <https://git.kernel.org/pub/scm/linux/kernel/git/djwong/xfstests-dev.git/log/?h=fuzz-baseline>`_。)

Stress Testing
(压力测试)
--------------

A unique requirement to online fsck is the ability to operate on a filesystem
concurrently with regular workloads.
Although it is of course impossible to run ``xfs_scrub`` with *zero* observable
impact on the running system, the online repair code should never introduce
inconsistencies into the filesystem metadata, and regular workloads should
never notice resource starvation.
To verify that these conditions are being met, fstests has been enhanced in
the following ways:
(在线 fsck 的一个独特要求是能够与常规工作负载并发地在文件系统上操作。虽然当然不可能以*零*可观察影响运行 ``xfs_scrub``，但在线修复代码绝不应向文件系统元数据引入不一致，常规工作负载也绝不应察觉到资源匮乏。为了验证这些条件得到满足，fstests 已通过以下方式得到增强：)

* For each scrub item type, create a test to exercise checking that item type
  while running ``fsstress``.
  (对于每种 scrub 项类型，创建一个测试，在运行 ``fsstress`` 的同时演练检查该项类型。)
* For each scrub item type, create a test to exercise repairing that item type
  while running ``fsstress``.
  (对于每种 scrub 项类型，创建一个测试，在运行 ``fsstress`` 的同时演练修复该项类型。)
* Race ``fsstress`` and ``xfs_scrub -n`` to ensure that checking the whole
  filesystem doesn't cause problems.
  (让 ``fsstress`` 与 ``xfs_scrub -n`` 竞争，以确保检查整个文件系统不会引起问题。)
* Race ``fsstress`` and ``xfs_scrub`` in force-rebuild mode to ensure that
  force-repairing the whole filesystem doesn't cause problems.
  (让 ``fsstress`` 与强制重建模式的 ``xfs_scrub`` 竞争，以确保强制修复整个文件系统不会引起问题。)
* Race ``xfs_scrub`` in check and force-repair mode against ``fsstress`` while
  freezing and thawing the filesystem.
  (在冻结和解冻文件系统期间，让检查和强制修复模式的 ``xfs_scrub`` 与 ``fsstress`` 竞争。)
* Race ``xfs_scrub`` in check and force-repair mode against ``fsstress`` while
  remounting the filesystem read-only and read-write.
  (在以只读和读写重新挂载文件系统期间，让检查和强制修复模式的 ``xfs_scrub`` 与 ``fsstress`` 竞争。)
* The same, but running ``fsx`` instead of ``fsstress``.  (Not done yet?)
  (同样的内容，但运行 ``fsx`` 而不是 ``fsstress``。(尚未完成？))

Success is defined by the ability to run all of these tests without observing
any unexpected filesystem shutdowns due to corrupted metadata, kernel hang
check warnings, or any other sort of mischief.
(成功的定义是能够运行所有这些测试，而观察不到任何因元数据损坏导致的意外文件系统停机、内核挂起检查警告或任何其他类型的恶作剧。)

4. User Interface
(4. 用户界面)
=================

The primary user of online fsck is the system administrator, just like offline
repair.
Online fsck presents two modes of operation to administrators:
A foreground CLI process for online fsck on demand, and a background service
that performs autonomous checking and repair.
(在线 fsck 的主要用户是系统管理员，就像离线修复一样。在线 fsck 向管理员提供两种操作模式：按需执行在线 fsck 的前台 CLI 进程，以及执行自主检查和修复的后台服务。)

Checking on Demand
(按需检查)
------------------

For administrators who want the absolute freshest information about the
metadata in a filesystem, ``xfs_scrub`` can be run as a foreground process on
a command line.
The program checks every piece of metadata in the filesystem while the
administrator waits for the results to be reported, just like the existing
``xfs_repair`` tool.
Both tools share a ``-n`` option to perform a read-only scan, and a ``-v``
option to increase the verbosity of the information reported.
(对于想要关于文件系统中元数据的绝对最新信息的管理员，``xfs_scrub`` 可以作为前台进程在命令行上运行。该程序在管理员等待结果报告的同时检查文件系统中的每一条元数据，就像现有的 ``xfs_repair`` 工具一样。两个工具共享一个执行只读扫描的 ``-n`` 选项，以及一个增加所报告信息详细程度的 ``-v`` 选项。)

A new feature of ``xfs_scrub`` is the ``-x`` option, which employs the error
correction capabilities of the hardware to check data file contents.
The media scan is not enabled by default because it may dramatically increase
program runtime and consume a lot of bandwidth on older storage hardware.
(``xfs_scrub`` 的一个新特性是 ``-x`` 选项，它利用硬件的错误纠正能力来检查数据文件内容。介质扫描默认不启用，因为它可能显著增加程序运行时间，并在较旧的存储硬件上消耗大量带宽。)

The output of a foreground invocation is captured in the system log.
(前台调用的输出被捕获到系统日志中。)

The ``xfs_scrub_all`` program walks the list of mounted filesystems and
initiates ``xfs_scrub`` for each of them in parallel.
It serializes scans for any filesystems that resolve to the same top level
kernel block device to prevent resource overconsumption.
(``xfs_scrub_all`` 程序遍历已挂载文件系统的列表，并并行地对每个文件系统启动 ``xfs_scrub``。它对解析到同一顶层内核块设备的任何文件系统将扫描串行化，以防止资源过度消耗。)

Background Service
(后台服务)
------------------

To reduce the workload of system administrators, the ``xfs_scrub`` package
provides a suite of `systemd <https://systemd.io/>`_ timers and services that
run online fsck automatically on weekends by default.
The background service configures scrub to run with as little privilege as
possible, the lowest CPU and IO priority, and in a CPU-constrained single
threaded mode.
This can be tuned by the systemd administrator at any time to suit the latency
and throughput requirements of customer workloads.
(为了减少系统管理员的工作量，``xfs_scrub`` 软件包提供了一组 `systemd <https://systemd.io/>`_ 定时器和服务，默认在周末自动运行在线 fsck。后台服务将 scrub 配置为以尽可能少的特权、最低的 CPU 和 IO 优先级以及 CPU 受限的单线程模式运行。systemd 管理员可以随时对其进行调整，以适应客户工作负载的延迟和吞吐量要求。)

The output of the background service is also captured in the system log.
If desired, reports of failures (either due to inconsistencies or mere runtime
errors) can be emailed automatically by setting the ``EMAIL_ADDR`` environment
variable in the following service files:
(后台服务的输出也被捕获到系统日志中。如果需要，可以通过在以下服务文件中设置 ``EMAIL_ADDR`` 环境变量，自动通过电子邮件发送失败报告(无论是由于不一致还是仅仅由于运行时错误)：)

* ``xfs_scrub_fail@.service``
* ``xfs_scrub_media_fail@.service``
* ``xfs_scrub_all_fail.service``

The decision to enable the background scan is left to the system administrator.
This can be done by enabling either of the following services:
(是否启用后台扫描的决定留给系统管理员。可以通过启用以下任一服务来完成：)

* ``xfs_scrub_all.timer`` on systemd systems
  (systemd 系统上的 ``xfs_scrub_all.timer``)
* ``xfs_scrub_all.cron`` on non-systemd systems
  (非 systemd 系统上的 ``xfs_scrub_all.cron``)

This automatic weekly scan is configured out of the box to perform an
additional media scan of all file data once per month.
This is less foolproof than, say, storing file data block checksums, but much
more performant if application software provides its own integrity checking,
redundancy can be provided elsewhere above the filesystem, or the storage
device's integrity guarantees are deemed sufficient.
(开箱即用的情况下，这个自动的每周扫描被配置为每月对所有文件数据执行一次额外的介质扫描。这不如存储文件数据块校验和那样万无一失，但如果应用软件提供自己的完整性检查、可以在文件系统之上的其他地方提供冗余，或者存储设备的完整性保证被认为足够，那么它的性能要好得多。)

The systemd unit file definitions have been subjected to a security audit
(as of systemd 249) to ensure that the xfs_scrub processes have as little
access to the rest of the system as possible.
This was performed via ``systemd-analyze security``, after which privileges
were restricted to the minimum required, sandboxing was set up to the maximal
extent possible with sandboxing and system call filtering; and access to the
filesystem tree was restricted to the minimum needed to start the program and
access the filesystem being scanned.
The service definition files restrict CPU usage to 80% of one CPU core, and
apply as nice of a priority to IO and CPU scheduling as possible.
This measure was taken to minimize delays in the rest of the filesystem.
No such hardening has been performed for the cron job.
(systemd 单元文件定义已经过安全审计(截至 systemd 249)，以确保 xfs_scrub 进程对系统其余部分的访问尽可能少。这是通过 ``systemd-analyze security`` 执行的，之后特权被限制到所需的最小值，通过沙箱和系统调用过滤将沙箱设置到最大程度；并且对文件系统树的访问被限制为启动程序和访问被扫描文件系统所需的最小值。服务定义文件将 CPU 使用限制在一个 CPU 核心的 80%，并尽可能对 IO 和 CPU 调度应用尽可能好的优先级。采取这一措施是为了尽量减少文件系统其余部分的延迟。cron 任务没有执行这种加固。)

Health Reporting
(健康报告)
----------------

XFS caches a summary of each filesystem's health status in memory.
The information is updated whenever ``xfs_scrub`` is run, or whenever
inconsistencies are detected in the filesystem metadata during regular
operations.
System administrators should use the ``health`` command of ``xfs_spaceman`` to
download this information into a human-readable format.
If problems have been observed, the administrator can schedule a reduced
service window to run the online repair tool to correct the problem.
Failing that, the administrator can decide to schedule a maintenance window to
run the traditional offline repair tool to correct the problem.
(XFS 在内存中缓存每个文件系统健康状况的摘要。每当运行 ``xfs_scrub`` 时，或在常规操作期间在文件系统元数据中检测到不一致时，该信息都会更新。系统管理员应使用 ``xfs_spaceman`` 的 ``health`` 命令将该信息下载为人类可读的格式。如果观察到问题，管理员可以安排一个缩短的服务窗口来运行在线修复工具以纠正问题。如果做不到这一点，管理员可以决定安排一个维护窗口来运行传统的离线修复工具以纠正问题。)

**Future Work Question**: Should the health reporting integrate with the new
inotify fs error notification system?
Would it be helpful for sysadmins to have a daemon to listen for corruption
notifications and initiate a repair?
(**未来工作问题**：健康报告是否应与新的 inotify 文件系统错误通知系统集成？对系统管理员来说，拥有一个守护进程来侦听损坏通知并启动修复是否有帮助？)

*Answer*: These questions remain unanswered, but should be a part of the
conversation with early adopters and potential downstream users of XFS.
(*回答*：这些问题仍未有答案，但应当成为与 XFS 的早期采用者和潜在下游用户对话的一部分。)

5. Kernel Algorithms and Data Structures
(5. 内核算法和数据结构)
========================================

This section discusses the key algorithms and data structures of the kernel
code that provide the ability to check and repair metadata while the system
is running.
The first chapters in this section reveal the pieces that provide the
foundation for checking metadata.
The remainder of this section presents the mechanisms through which XFS
regenerates itself.
(本节讨论内核代码中提供在系统运行时检查和修复元数据能力的关键算法和数据结构。本节的第一批章节揭示为检查元数据提供基础的部件。本节的其余部分展示 XFS 借以自我再生(重建)的机制。)

Self Describing Metadata
(自描述元数据)
------------------------

Starting with XFS version 5 in 2012, XFS updated the format of nearly every
ondisk block header to record a magic number, a checksum, a universally
"unique" identifier (UUID), an owner code, the ondisk address of the block,
and a log sequence number.
When loading a block buffer from disk, the magic number, UUID, owner, and
ondisk address confirm that the retrieved block matches the specific owner of
the current filesystem, and that the information contained in the block is
supposed to be found at the ondisk address.
The first three components enable checking tools to disregard alleged metadata
that doesn't belong to the filesystem, and the fourth component enables the
filesystem to detect lost writes.
(从 2012 年的 XFS 版本 5 开始，XFS 更新了几乎所有磁盘块头的格式，以记录魔数、校验和、通用"唯一"标识符(UUID)、属主代码、块的磁盘地址以及日志序列号。当从磁盘加载块缓冲时，魔数、UUID、属主和磁盘地址确认所取回的块与当前文件系统的特定属主匹配，并且块中包含的信息应当位于该磁盘地址处。前三个组件使检查工具能够忽略不属于该文件系统的所谓元数据，第四个组件使文件系统能够检测丢失的写入。)

Whenever a file system operation modifies a block, the change is submitted
to the log as part of a transaction.
The log then processes these transactions marking them done once they are
safely persisted to storage.
The logging code maintains the checksum and the log sequence number of the last
transactional update.
Checksums are useful for detecting torn writes and other discrepancies that can
be introduced between the computer and its storage devices.
Sequence number tracking enables log recovery to avoid applying out of date
log updates to the filesystem.
(每当文件系统操作修改一个块时，该更改作为事务的一部分被提交到日志。日志随后处理这些事务，一旦它们被安全地持久化到存储，就将它们标记为完成。日志代码维护最后一次事务性更新的校验和与日志序列号。校验和对于检测撕裂写以及可能出现在计算机与其存储设备之间的其他差异很有用。序列号跟踪使日志恢复能够避免将过期的日志更新应用到文件系统。)

These two features improve overall runtime resiliency by providing a means for
the filesystem to detect obvious corruption when reading metadata blocks from
disk, but these buffer verifiers cannot provide any consistency checking
between metadata structures.
(这两个特性通过提供一种手段使文件系统能够在从磁盘读取元数据块时检测明显的损坏，从而提高了整体运行时弹性，但这些缓冲校验器无法在元数据结构之间提供任何一致性检查。)

For more information, please see the documentation for
Documentation/filesystems/xfs/xfs-self-describing-metadata.rst
(更多信息请参阅 Documentation/filesystems/xfs/xfs-self-describing-metadata.rst 文档。)

Reverse Mapping
(反向映射)
---------------

The original design of XFS (circa 1993) is an improvement upon 1980s Unix
filesystem design.
In those days, storage density was expensive, CPU time was scarce, and
excessive seek time could kill performance.
For performance reasons, filesystem authors were reluctant to add redundancy to
the filesystem, even at the cost of data integrity.
Filesystems designers in the early 21st century choose different strategies to
increase internal redundancy -- either storing nearly identical copies of
metadata, or more space-efficient encoding techniques.
(XFS 的原始设计(约 1993 年)是对 1980 年代 Unix 文件系统设计的改进。在那个年代，存储密度昂贵，CPU 时间稀缺，过度的寻道时间会扼杀性能。出于性能原因，文件系统作者不愿意向文件系统添加冗余，即使以数据完整性为代价。21 世纪初的文件系统设计者选择了不同的策略来增加内部冗余——要么存储几乎相同的元数据副本，要么采用更节省空间的编码技术。)

For XFS, a different redundancy strategy was chosen to modernize the design:
a secondary space usage index that maps allocated disk extents back to their
owners.
By adding a new index, the filesystem retains most of its ability to scale
well to heavily threaded workloads involving large datasets, since the primary
file metadata (the directory tree, the file block map, and the allocation
groups) remain unchanged.
Like any system that improves redundancy, the reverse-mapping feature increases
overhead costs for space mapping activities.
However, it has two critical advantages: first, the reverse index is key to
enabling online fsck and other requested functionality such as free space
defragmentation, better media failure reporting, and filesystem shrinking.
Second, the different ondisk storage format of the reverse mapping btree
defeats device-level deduplication because the filesystem requires real
redundancy.
(对于 XFS，选择了一种不同的冗余策略来现代化其设计：一个将已分配的磁盘区段映射回其属主的二级空间使用索引。通过添加新索引，文件系统保留了对涉及大型数据集的重度多线程工作负载良好扩展的大部分能力，因为主文件元数据(目录树、文件块映射和分配组)保持不变。与任何提高冗余的系统一样，反向映射特性增加了空间映射活动的开销成本。然而，它有两个关键优势：第一，反向索引是启用在线 fsck 以及其他所需功能(如空闲空间碎片整理、更好的介质故障报告和文件系统收缩)的关键。第二，反向映射 btree 不同的磁盘存储格式击败了设备级去重，因为文件系统需要真正的冗余。)

+--------------------------------------------------------------------------+
| **Sidebar**:                                                             |
| (侧栏：)                                                                    |
+--------------------------------------------------------------------------+
| A criticism of adding the secondary index is that it does nothing to     |
| improve the robustness of user data storage itself.                      |
| This is a valid point, but adding a new index for file data block        |
| checksums increases write amplification by turning data overwrites into  |
| copy-writes, which age the filesystem prematurely.                       |
| In keeping with thirty years of precedent, users who want file data      |
| integrity can supply as powerful a solution as they require.             |
| As for metadata, the complexity of adding a new secondary index of space |
| usage is much less than adding volume management and storage device      |
| mirroring to XFS itself.                                                 |
| Perfection of RAID and volume management are best left to existing       |
| layers in the kernel.                                                    |
| (对添加二级索引的一种批评是，它对提高用户数据存储本身的健壮性毫无帮助。)                                    |
| (这是一个有道理的观点，但为文件数据块校验和添加新索引会将数据覆写变成复制写，从而增加写放大，使文件系统过早老化。)               |
| (遵循三十年的惯例，想要文件数据完整性的用户可以自行提供他们所需的尽可能强大的解决方案。)                            |
| (至于元数据，添加一个新的空间使用二级索引的复杂度远低于向 XFS 本身添加卷管理和存储设备镜像。)                       |
| (RAID 和卷管理的完善最好留给内核中现有的层。)                                               |
+--------------------------------------------------------------------------+

The information captured in a reverse space mapping record is as follows:
(反向空间映射记录中捕获的信息如下：)

.. code-block:: c

	struct xfs_rmap_irec {
	    xfs_agblock_t    rm_startblock;   /* extent start block */
	    xfs_extlen_t     rm_blockcount;   /* extent length */
	    uint64_t         rm_owner;        /* extent owner */
	    uint64_t         rm_offset;       /* offset within the owner */
	    unsigned int     rm_flags;        /* state flags */
	};

The first two fields capture the location and size of the physical space,
in units of filesystem blocks.
The owner field tells scrub which metadata structure or file inode have been
assigned this space.
For space allocated to files, the offset field tells scrub where the space was
mapped within the file fork.
Finally, the flags field provides extra information about the space usage --
is this an attribute fork extent?  A file mapping btree extent?  Or an
unwritten data extent?
(前两个字段以文件系统块为单位捕获物理空间的位置和大小。owner 字段告诉 scrub 哪个元数据结构或文件 inode 被分配了这块空间。对于分配给文件的空间，offset 字段告诉 scrub 该空间被映射到文件 fork 中的什么位置。最后，flags 字段提供关于空间使用的额外信息——这是属性 fork 区段？文件映射 btree 区段？还是未写入的数据区段？)

Online filesystem checking judges the consistency of each primary metadata
record by comparing its information against all other space indices.
The reverse mapping index plays a key role in the consistency checking process
because it contains a centralized alternate copy of all space allocation
information.
Program runtime and ease of resource acquisition are the only real limits to
what online checking can consult.
For example, a file data extent mapping can be checked against:
(在线文件系统检查通过将每条主元数据记录的信息与所有其他空间索引进行比较来判断其一致性。反向映射索引在一致性检查过程中扮演关键角色，因为它包含所有空间分配信息的集中式替代副本。程序运行时间和资源获取的难易程度是在线检查所能查询对象的唯一真正限制。例如，一个文件数据区段映射可以对照以下内容进行检查：)

* The absence of an entry in the free space information.
  (空闲空间信息中不存在对应条目。)
* The absence of an entry in the inode index.
  (inode 索引中不存在对应条目。)
* The absence of an entry in the reference count data if the file is not
  marked as having shared extents.
  (如果文件未被标记为拥有共享区段，则引用计数数据中不存在对应条目。)
* The correspondence of an entry in the reverse mapping information.
  (反向映射信息中存在对应条目。)

There are several observations to make about reverse mapping indices:
(关于反向映射索引有几点观察：)

1. Reverse mappings can provide a positive affirmation of correctness if any of
   the above primary metadata are in doubt.
   The checking code for most primary metadata follows a path similar to the
   one outlined above.
   (如果上述任何主元数据存疑，反向映射可以提供对正确性的肯定确认。大多数主元数据的检查代码遵循与上面概述类似的路径。)

2. Proving the consistency of secondary metadata with the primary metadata is
   difficult because that requires a full scan of all primary space metadata,
   which is very time intensive.
   For example, checking a reverse mapping record for a file extent mapping
   btree block requires locking the file and searching the entire btree to
   confirm the block.
   Instead, scrub relies on rigorous cross-referencing during the primary space
   mapping structure checks.
   (证明二级元数据与主元数据的一致性是困难的，因为那需要对所有主空间元数据进行全面扫描，非常耗时。例如，检查文件区段映射 btree 块的反向映射记录需要锁定文件并搜索整个 btree 以确认该块。相反，scrub 依靠在主空间映射结构检查期间的严格交叉引用。)

3. Consistency scans must use non-blocking lock acquisition primitives if the
   required locking order is not the same order used by regular filesystem
   operations.
   For example, if the filesystem normally takes a file ILOCK before taking
   the AGF buffer lock but scrub wants to take a file ILOCK while holding
   an AGF buffer lock, scrub cannot block on that second acquisition.
   This means that forward progress during this part of a scan of the reverse
   mapping data cannot be guaranteed if system load is heavy.
   (如果所需的加锁顺序与常规文件系统操作使用的顺序不同，一致性扫描必须使用非阻塞的锁获取原语。例如，如果文件系统通常在获取 AGF 缓冲锁之前先获取文件 ILOCK，而 scrub 想在持有 AGF 缓冲锁的同时获取文件 ILOCK，则 scrub 不能在第二次获取上阻塞。这意味着如果系统负载很重，反向映射数据扫描的这一部分无法保证向前推进。)

In summary, reverse mappings play a key role in reconstruction of primary
metadata.
The details of how these records are staged, written to disk, and committed
into the filesystem are covered in subsequent sections.
(总之，反向映射在主元数据的重建中扮演关键角色。关于这些记录如何暂存、写入磁盘以及提交到文件系统的细节将在后续章节中介绍。)

Checking and Cross-Referencing
(检查与交叉引用)
------------------------------

The first step of checking a metadata structure is to examine every record
contained within the structure and its relationship with the rest of the
system.
XFS contains multiple layers of checking to try to prevent inconsistent
metadata from wreaking havoc on the system.
Each of these layers contributes information that helps the kernel to make
three decisions about the health of a metadata structure:
(检查元数据结构的第一步是检查结构中包含的每条记录及其与系统其余部分的关系。XFS 包含多层检查，以尽力防止不一致的元数据对系统造成严重破坏。这些层中的每一层都提供信息，帮助内核就元数据结构的健康状况做出三项决策：)

- Is a part of this structure obviously corrupt (``XFS_SCRUB_OFLAG_CORRUPT``) ?
  (该结构的一部分是否明显损坏(``XFS_SCRUB_OFLAG_CORRUPT``)？)
- Is this structure inconsistent with the rest of the system
  (``XFS_SCRUB_OFLAG_XCORRUPT``) ?
  (该结构是否与系统其余部分不一致(``XFS_SCRUB_OFLAG_XCORRUPT``)？)
- Is there so much damage around the filesystem that cross-referencing is not
  possible (``XFS_SCRUB_OFLAG_XFAIL``) ?
  (文件系统周围的损坏是否严重到无法进行交叉引用(``XFS_SCRUB_OFLAG_XFAIL``)？)
- Can the structure be optimized to improve performance or reduce the size of
  metadata (``XFS_SCRUB_OFLAG_PREEN``) ?
  (该结构是否可以优化以提高性能或减小元数据大小(``XFS_SCRUB_OFLAG_PREEN``)？)
- Does the structure contain data that is not inconsistent but deserves review
  by the system administrator (``XFS_SCRUB_OFLAG_WARNING``) ?
  (该结构是否包含并非不一致但值得系统管理员审阅的数据(``XFS_SCRUB_OFLAG_WARNING``)？)

The following sections describe how the metadata scrubbing process works.
(以下各节描述元数据 scrubbing 过程的工作方式。)

Metadata Buffer Verification
(元数据缓冲校验)
````````````````````````````

The lowest layer of metadata protection in XFS are the metadata verifiers built
into the buffer cache.
These functions perform inexpensive internal consistency checking of the block
itself, and answer these questions:
(XFS 中元数据保护的最低层是内置于缓冲缓存中的元数据校验器。这些函数对块本身执行廉价的内部一致性检查，并回答这些问题：)

- Does the block belong to this filesystem?
  (该块是否属于此文件系统？)

- Does the block belong to the structure that asked for the read?
  This assumes that metadata blocks only have one owner, which is always true
  in XFS.
  (该块是否属于请求读取的结构？这假设元数据块只有一个属主，这在 XFS 中总是成立的。)

- Is the type of data stored in the block within a reasonable range of what
  scrub is expecting?
  (块中存储的数据类型是否在 scrub 所期望的合理范围内？)

- Does the physical location of the block match the location it was read from?
  (块的物理位置是否与它被读取的位置匹配？)

- Does the block checksum match the data?
  (块校验和是否与数据匹配？)

The scope of the protections here are very limited -- verifiers can only
establish that the filesystem code is reasonably free of gross corruption bugs
and that the storage system is reasonably competent at retrieval.
Corruption problems observed at runtime cause the generation of health reports,
failed system calls, and in the extreme case, filesystem shutdowns if the
corrupt metadata force the cancellation of a dirty transaction.
(这里保护的范围非常有限——校验器只能确定文件系统代码基本没有严重的损坏 bug，以及存储系统在检索方面基本称职。运行时观察到的损坏问题会导致生成健康报告、系统调用失败，在极端情况下，如果损坏的元数据迫使取消一个脏事务，则会导致文件系统停机。)

Every online fsck scrubbing function is expected to read every ondisk metadata
block of a structure in the course of checking the structure.
Corruption problems observed during a check are immediately reported to
userspace as corruption; during a cross-reference, they are reported as a
failure to cross-reference once the full examination is complete.
Reads satisfied by a buffer already in cache (and hence already verified)
bypass these checks.
(每个在线 fsck scrubbing 函数都应在检查结构的过程中读取该结构的每个磁盘元数据块。检查期间观察到的损坏问题会立即作为损坏报告给用户空间；在交叉引用期间，一旦完整检查完成，它们会被报告为交叉引用失败。由缓存中已有的缓冲(因此已校验过)满足的读取会绕过这些检查。)

Internal Consistency Checks
(内部一致性检查)
```````````````````````````

After the buffer cache, the next level of metadata protection is the internal
record verification code built into the filesystem.
These checks are split between the buffer verifiers, the in-filesystem users of
the buffer cache, and the scrub code itself, depending on the amount of higher
level context required.
The scope of checking is still internal to the block.
These higher level checking functions answer these questions:
(在缓冲缓存之后，下一层元数据保护是内置于文件系统中的内部记录校验代码。这些检查根据所需高层上下文的多少，在缓冲校验器、缓冲缓存的文件系统内使用者以及 scrub 代码本身之间分配。检查的范围仍在块内部。这些更高层的检查函数回答这些问题：)

- Does the type of data stored in the block match what scrub is expecting?
  (块中存储的数据类型是否与 scrub 所期望的匹配？)

- Does the block belong to the owning structure that asked for the read?
  (该块是否属于请求读取的拥有结构？)

- If the block contains records, do the records fit within the block?
  (如果块包含记录，这些记录是否都能容纳在块内？)

- If the block tracks internal free space information, is it consistent with
  the record areas?
  (如果块跟踪内部空闲空间信息，它与记录区域是否一致？)

- Are the records contained inside the block free of obvious corruptions?
  (块内包含的记录是否没有明显的损坏？)

Record checks in this category are more rigorous and more time-intensive.
For example, block pointers and inumbers are checked to ensure that they point
within the dynamically allocated parts of an allocation group and within
the filesystem.
Names are checked for invalid characters, and flags are checked for invalid
combinations.
Other record attributes are checked for sensible values.
Btree records spanning an interval of the btree keyspace are checked for
correct order and lack of mergeability (except for file fork mappings).
For performance reasons, regular code may skip some of these checks unless
debugging is enabled or a write is about to occur.
Scrub functions, of course, must check all possible problems.
(这一类别的记录检查更严格、更耗时。例如，检查块指针和 inumber 以确保它们指向分配组的动态分配部分之内以及文件系统之内。检查名称是否含有无效字符，检查标志是否为无效组合。检查其他记录属性是否有合理的值。跨越 btree 键空间一个区间的 btree 记录被检查顺序是否正确以及是否不可合并(文件 fork 映射除外)。出于性能原因，常规代码可能会跳过其中一些检查，除非启用了调试或即将发生写入。当然，scrub 函数必须检查所有可能的问题。)

Validation of Userspace-Controlled Record Attributes
(用户空间控制的记录属性的校验)
````````````````````````````````````````````````````

Various pieces of filesystem metadata are directly controlled by userspace.
Because of this nature, validation work cannot be more precise than checking
that a value is within the possible range.
These fields include:
(文件系统元数据的各个部分由用户空间直接控制。由于这种性质，校验工作最多只能检查一个值是否在可能的范围内。这些字段包括：)

- Superblock fields controlled by mount options
  (由挂载选项控制的超级块字段)
- Filesystem labels
  (文件系统标签)
- File timestamps
  (文件时间戳)
- File permissions
  (文件权限)
- File size
  (文件大小)
- File flags
  (文件标志)
- Names present in directory entries, extended attribute keys, and filesystem
  labels
  (目录项、扩展属性键和文件系统标签中出现的名称)
- Extended attribute key namespaces
  (扩展属性键命名空间)
- Extended attribute values
  (扩展属性值)
- File data block contents
  (文件数据块内容)
- Quota limits
  (配额限制)
- Quota timer expiration (if resource usage exceeds the soft limit)
  (配额计时器到期(如果资源使用超过软限制))

Cross-Referencing Space Metadata
(交叉引用空间元数据)
````````````````````````````````

After internal block checks, the next higher level of checking is
cross-referencing records between metadata structures.
For regular runtime code, the cost of these checks is considered to be
prohibitively expensive, but as scrub is dedicated to rooting out
inconsistencies, it must pursue all avenues of inquiry.
The exact set of cross-referencing is highly dependent on the context of the
data structure being checked.
(在内部块检查之后，下一个更高层次的检查是在元数据结构之间交叉引用记录。对于常规运行时代码，这些检查的成本被认为是高得难以承受的，但由于 scrub 专心于根除不一致，它必须探索所有查询途径。交叉引用的具体集合高度依赖于被检查数据结构的上下文。)

The XFS btree code has keyspace scanning functions that online fsck uses to
cross reference one structure with another.
Specifically, scrub can scan the key space of an index to determine if that
keyspace is fully, sparsely, or not at all mapped to records.
For the reverse mapping btree, it is possible to mask parts of the key for the
purposes of performing a keyspace scan so that scrub can decide if the rmap
btree contains records mapping a certain extent of physical space without the
sparsenses of the rest of the rmap keyspace getting in the way.
(XFS btree 代码具有键空间扫描函数，在线 fsck 用它来将一个结构与另一个结构交叉引用。具体来说，scrub 可以扫描一个索引的键空间，以确定该键空间是完全、稀疏还是完全没有映射到记录。对于反向映射 btree，可以为了执行键空间扫描而掩蔽键的一部分，使 scrub 能够判断 rmap btree 是否包含映射某段物理空间的记录，而 rmap 键空间其余部分的稀疏性不会造成干扰。)

Btree blocks undergo the following checks before cross-referencing:
(btree 块在交叉引用之前要经过以下检查：)

- Does the type of data stored in the block match what scrub is expecting?
  (块中存储的数据类型是否与 scrub 所期望的匹配？)

- Does the block belong to the owning structure that asked for the read?
  (该块是否属于请求读取的拥有结构？)

- Do the records fit within the block?
  (记录是否都能容纳在块内？)

- Are the records contained inside the block free of obvious corruptions?
  (块内包含的记录是否没有明显的损坏？)

- Are the name hashes in the correct order?
  (名称哈希的顺序是否正确？)

- Do node pointers within the btree point to valid block addresses for the type
  of btree?
  (btree 内的节点指针是否指向对该类型 btree 有效的块地址？)

- Do child pointers point towards the leaves?
  (子指针是否指向叶子方向？)

- Do sibling pointers point across the same level?
  (兄弟指针是否指向同一层级？)

- For each node block record, does the record key accurate reflect the contents
  of the child block?
  (对于每个节点块记录，记录键是否准确反映了子块的内容？)

Space allocation records are cross-referenced as follows:
(空间分配记录按以下方式交叉引用：)

1. Any space mentioned by any metadata structure are cross-referenced as
   follows:
   (任何元数据结构提及的任何空间按以下方式交叉引用：)

   - Does the reverse mapping index list only the appropriate owner as the
     owner of each block?
     (反向映射索引是否只将适当的属主列为每个块的属主？)

   - Are none of the blocks claimed as free space?
     (是否没有任何块被声明为空闲空间？)

   - If these aren't file data blocks, are none of the blocks claimed as space
     shared by different owners?
     (如果这些不是文件数据块，是否没有任何块被声明为由不同属主共享的空间？)

2. Btree blocks are cross-referenced as follows:
   (btree 块按以下方式交叉引用：)

   - Everything in class 1 above.
     (上面第 1 类中的所有内容。)

   - If there's a parent node block, do the keys listed for this block match the
     keyspace of this block?
     (如果存在父节点块，为此块列出的键是否与该块的键空间匹配？)

   - Do the sibling pointers point to valid blocks?  Of the same level?
     (兄弟指针是否指向有效的块？同一层级吗？)

   - Do the child pointers point to valid blocks?  Of the next level down?
     (子指针是否指向有效的块？下一层吗？)

3. Free space btree records are cross-referenced as follows:
   (空闲空间 btree 记录按以下方式交叉引用：)

   - Everything in class 1 and 2 above.
     (上面第 1 类和第 2 类中的所有内容。)

   - Does the reverse mapping index list no owners of this space?
     (反向映射索引是否未列出此空间的任何属主？)

   - Is this space not claimed by the inode index for inodes?
     (此空间是否未被 inode 索引声明为用于 inode？)

   - Is it not mentioned by the reference count index?
     (它是否未被引用计数索引提及？)

   - Is there a matching record in the other free space btree?
     (另一个空闲空间 btree 中是否有匹配的记录？)

4. Inode btree records are cross-referenced as follows:
   (inode btree 记录按以下方式交叉引用：)

   - Everything in class 1 and 2 above.
     (上面第 1 类和第 2 类中的所有内容。)

   - Is there a matching record in free inode btree?
     (空闲 inode btree 中是否有匹配的记录？)

   - Do cleared bits in the holemask correspond with inode clusters?
     (holemask 中清零的位是否与 inode 簇对应？)

   - Do set bits in the freemask correspond with inode records with zero link
     count?
     (freemask 中置位的位是否与链接计数为零的 inode 记录对应？)

5. Inode records are cross-referenced as follows:
   (inode 记录按以下方式交叉引用：)

   - Everything in class 1.
     (第 1 类中的所有内容。)

   - Do all the fields that summarize information about the file forks actually
     match those forks?
     (所有概括文件 fork 信息的字段是否与那些 fork 实际匹配？)

   - Does each inode with zero link count correspond to a record in the free
     inode btree?
     (每个链接计数为零的 inode 是否对应空闲 inode btree 中的一条记录？)

6. File fork space mapping records are cross-referenced as follows:
   (文件 fork 空间映射记录按以下方式交叉引用：)

   - Everything in class 1 and 2 above.
     (上面第 1 类和第 2 类中的所有内容。)

   - Is this space not mentioned by the inode btrees?
     (此空间是否未被 inode btree 提及？)

   - If this is a CoW fork mapping, does it correspond to a CoW entry in the
     reference count btree?
     (如果这是 CoW fork 映射，它是否对应引用计数 btree 中的一个 CoW 条目？)

7. Reference count records are cross-referenced as follows:
   (引用计数记录按以下方式交叉引用：)

   - Everything in class 1 and 2 above.
     (上面第 1 类和第 2 类中的所有内容。)

   - Within the space subkeyspace of the rmap btree (that is to say, all
     records mapped to a particular space extent and ignoring the owner info),
     are there the same number of reverse mapping records for each block as the
     reference count record claims?
     (在 rmap btree 的空间子键空间内(也就是说，映射到特定空间区段的所有记录，忽略属主信息)，每个块的反向映射记录数量是否与引用计数记录所声称的相同？)

Checking Extended Attributes
(检查扩展属性)
````````````````````````````

Extended attributes implement a key-value store that enable fragments of data
to be attached to any file.
Both the kernel and userspace can access the keys and values, subject to
namespace and privilege restrictions.
Most typically these fragments are metadata about the file -- origins, security
contexts, user-supplied labels, indexing information, etc.
(扩展属性实现了一个键值存储，使数据片段能够附着到任何文件上。内核和用户空间都可以在命名空间和特权限制下访问键和值。最典型的是，这些片段是关于文件的元数据——来源、安全上下文、用户提供的标签、索引信息等。)

Names can be as long as 255 bytes and can exist in several different
namespaces.
Values can be as large as 64KB.
A file's extended attributes are stored in blocks mapped by the attr fork.
The mappings point to leaf blocks, remote value blocks, or dabtree blocks.
Block 0 in the attribute fork is always the top of the structure, but otherwise
each of the three types of blocks can be found at any offset in the attr fork.
Leaf blocks contain attribute key records that point to the name and the value.
Names are always stored elsewhere in the same leaf block.
Values that are less than 3/4 the size of a filesystem block are also stored
elsewhere in the same leaf block.
Remote value blocks contain values that are too large to fit inside a leaf.
If the leaf information exceeds a single filesystem block, a dabtree (also
rooted at block 0) is created to map hashes of the attribute names to leaf
blocks in the attr fork.
(名称可以长达 255 字节，并且可以存在于几个不同的命名空间中。值可以大到 64KB。文件的扩展属性存储在由 attr fork 映射的块中。这些映射指向叶子块、远程值块或 dabtree 块。属性 fork 中的块 0 始终是结构的顶部，但除此之外，三种类型的块都可以位于 attr fork 的任意偏移处。叶子块包含指向名称和值的属性键记录。名称总是存储在同一叶子块的其他地方。小于文件系统块大小 3/4 的值也存储在同一叶子块的其他地方。远程值块包含大到无法容纳在叶子内的值。如果叶子信息超过单个文件系统块，则创建一个 dabtree(同样以块 0 为根)来将属性名称的哈希映射到 attr fork 中的叶子块。)

Checking an extended attribute structure is not so straightforward due to the
lack of separation between attr blocks and index blocks.
Scrub must read each block mapped by the attr fork and ignore the non-leaf
blocks:
(由于 attr 块与索引块之间缺乏分离，检查扩展属性结构并不那么直接。scrub 必须读取 attr fork 映射的每个块，并忽略非叶子块：)

1. Walk the dabtree in the attr fork (if present) to ensure that there are no
   irregularities in the blocks or dabtree mappings that do not point to
   attr leaf blocks.
   (遍历 attr fork 中的 dabtree(如果存在)，以确保块或 dabtree 映射中不存在不指向 attr 叶子块的反常情况。)

2. Walk the blocks of the attr fork looking for leaf blocks.
   For each entry inside a leaf:
   (遍历 attr fork 的块以查找叶子块。对于叶子内的每个条目：)

   a. Validate that the name does not contain invalid characters.
      (校验名称不包含无效字符。)

   b. Read the attr value.
      This performs a named lookup of the attr name to ensure the correctness
      of the dabtree.
      If the value is stored in a remote block, this also validates the
      integrity of the remote value block.
      (读取 attr 值。这会对 attr 名称执行一次按名查找，以确保 dabtree 的正确性。如果值存储在远程块中，这也会校验远程值块的完整性。)

Checking and Cross-Referencing Directories
(检查和交叉引用目录)
``````````````````````````````````````````

The filesystem directory tree is a directed acylic graph structure, with files
constituting the nodes, and directory entries (dirents) constituting the edges.
Directories are a special type of file containing a set of mappings from a
255-byte sequence (name) to an inumber.
These are called directory entries, or dirents for short.
Each directory file must have exactly one directory pointing to the file.
A root directory points to itself.
Directory entries point to files of any type.
Each non-directory file may have multiple directories point to it.
(文件系统目录树是一个有向无环图结构，文件构成节点，目录项(dirent)构成边。目录是一种特殊类型的文件，包含一组从 255 字节序列(名称)到 inumber 的映射。这些被称为目录项，简称 dirent。每个目录文件必须恰好有一个目录指向该文件。根目录指向自身。目录项可以指向任何类型的文件。每个非目录文件可以有多个目录指向它。)

In XFS, directories are implemented as a file containing up to three 32GB
partitions.
The first partition contains directory entry data blocks.
Each data block contains variable-sized records associating a user-provided
name with an inumber and, optionally, a file type.
If the directory entry data grows beyond one block, the second partition (which
exists as post-EOF extents) is populated with a block containing free space
information and an index that maps hashes of the dirent names to directory data
blocks in the first partition.
This makes directory name lookups very fast.
If this second partition grows beyond one block, the third partition is
populated with a linear array of free space information for faster
expansions.
If the free space has been separated and the second partition grows again
beyond one block, then a dabtree is used to map hashes of dirent names to
directory data blocks.
(在 XFS 中，目录被实现为包含最多三个 32GB 分区的文件。第一个分区包含目录项数据块。每个数据块包含可变大小的记录，将用户提供的名称与 inumber 以及可选的文件类型关联起来。如果目录项数据增长超过一个块，则第二个分区(以 EOF 后区段的形式存在)被填充一个包含空闲空间信息的块，以及一个将 dirent 名称的哈希映射到第一分区中目录数据块的索引。这使得目录名称查找非常快。如果第二个分区增长超过一个块，则第三个分区被填充一个空闲空间信息的线性数组，以实现更快的扩展。如果空闲空间已被分离出去，且第二个分区再次增长超过一个块，则使用 dabtree 将 dirent 名称的哈希映射到目录数据块。)

Checking a directory is pretty straightforward:
(检查目录相当直接：)

1. Walk the dabtree in the second partition (if present) to ensure that there
   are no irregularities in the blocks or dabtree mappings that do not point to
   dirent blocks.
   (遍历第二个分区中的 dabtree(如果存在)，以确保块或 dabtree 映射中不存在不指向 dirent 块的反常情况。)

2. Walk the blocks of the first partition looking for directory entries.
   Each dirent is checked as follows:
   (遍历第一个分区的块以查找目录项。每个 dirent 按如下方式检查：)

   a. Does the name contain no invalid characters?
      (名称是否不含无效字符？)

   b. Does the inumber correspond to an actual, allocated inode?
      (inumber 是否对应一个实际的、已分配的 inode？)

   c. Does the child inode have a nonzero link count?
      (子 inode 的链接计数是否非零？)

   d. If a file type is included in the dirent, does it match the type of the
      inode?
      (如果 dirent 中包含文件类型，它是否与 inode 的类型匹配？)

   e. If the child is a subdirectory, does the child's dotdot pointer point
      back to the parent?
      (如果子项是子目录，子项的 dotdot 指针是否指回父目录？)

   f. If the directory has a second partition, perform a named lookup of the
      dirent name to ensure the correctness of the dabtree.
      (如果目录有第二个分区，对 dirent 名称执行一次按名查找，以确保 dabtree 的正确性。)

3. Walk the free space list in the third partition (if present) to ensure that
   the free spaces it describes are really unused.
   (遍历第三个分区中的空闲空间列表(如果存在)，以确保它所描述的空闲空间确实未被使用。)

Checking operations involving :ref:`parents <dirparent>` and
:ref:`file link counts <nlinks>` are discussed in more detail in later
sections.
(涉及 :ref:`父目录 <dirparent>` 和 :ref:`文件链接计数 <nlinks>` 的检查操作将在后面的章节中更详细地讨论。)

Checking Directory/Attribute Btrees
(检查目录/属性 btree)
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

As stated in previous sections, the directory/attribute btree (dabtree) index
maps user-provided names to improve lookup times by avoiding linear scans.
Internally, it maps a 32-bit hash of the name to a block offset within the
appropriate file fork.
(如前面章节所述，目录/属性 btree(dabtree)索引映射用户提供的名称，通过避免线性扫描来改善查找时间。在内部，它将名称的 32 位哈希映射到相应文件 fork 内的块偏移。)

The internal structure of a dabtree closely resembles the btrees that record
fixed-size metadata records -- each dabtree block contains a magic number, a
checksum, sibling pointers, a UUID, a tree level, and a log sequence number.
The format of leaf and node records are the same -- each entry points to the
next level down in the hierarchy, with dabtree node records pointing to dabtree
leaf blocks, and dabtree leaf records pointing to non-dabtree blocks elsewhere
in the fork.
(dabtree 的内部结构与记录固定大小元数据记录的 btree 非常相似——每个 dabtree 块包含魔数、校验和、兄弟指针、UUID、树层级和日志序列号。叶子记录和节点记录的格式相同——每个条目指向层级中下一层，dabtree 节点记录指向 dabtree 叶子块，dabtree 叶子记录指向 fork 中其他地方的非 dabtree 块。)

Checking and cross-referencing the dabtree is very similar to what is done for
space btrees:
(检查和交叉引用 dabtree 与对空间 btree 所做的非常相似：)

- Does the type of data stored in the block match what scrub is expecting?
  (块中存储的数据类型是否与 scrub 所期望的匹配？)

- Does the block belong to the owning structure that asked for the read?
  (该块是否属于请求读取的拥有结构？)

- Do the records fit within the block?
  (记录是否都能容纳在块内？)

- Are the records contained inside the block free of obvious corruptions?
  (块内包含的记录是否没有明显的损坏？)

- Are the name hashes in the correct order?
  (名称哈希的顺序是否正确？)

- Do node pointers within the dabtree point to valid fork offsets for dabtree
  blocks?
  (dabtree 内的节点指针是否指向对 dabtree 块有效的 fork 偏移？)

- Do leaf pointers within the dabtree point to valid fork offsets for directory
  or attr leaf blocks?
  (dabtree 内的叶子指针是否指向对目录或 attr 叶子块有效的 fork 偏移？)

- Do child pointers point towards the leaves?
  (子指针是否指向叶子方向？)

- Do sibling pointers point across the same level?
  (兄弟指针是否指向同一层级？)

- For each dabtree node record, does the record key accurate reflect the
  contents of the child dabtree block?
  (对于每个 dabtree 节点记录，记录键是否准确反映了子 dabtree 块的内容？)

- For each dabtree leaf record, does the record key accurate reflect the
  contents of the directory or attr block?
  (对于每个 dabtree 叶子记录，记录键是否准确反映了目录或 attr 块的内容？)

Cross-Referencing Summary Counters
(交叉引用摘要计数器)
``````````````````````````````````

XFS maintains three classes of summary counters: available resources, quota
resource usage, and file link counts.
(XFS 维护三类摘要计数器：可用资源、配额资源使用和文件链接计数。)

In theory, the amount of available resources (data blocks, inodes, realtime
extents) can be found by walking the entire filesystem.
This would make for very slow reporting, so a transactional filesystem can
maintain summaries of this information in the superblock.
Cross-referencing these values against the filesystem metadata should be a
simple matter of walking the free space and inode metadata in each AG and the
realtime bitmap, but there are complications that will be discussed in
:ref:`more detail <fscounters>` later.
(理论上，可用资源(数据块、inode、实时区段)的数量可以通过遍历整个文件系统来得到。这会导致报告非常慢，所以事务性文件系统可以在超级块中维护这些信息的摘要。将这些值与文件系统元数据交叉引用，应当只是遍历每个 AG 的空闲空间和 inode 元数据以及实时位图的简单事情，但存在一些复杂情况，稍后将在 :ref:`更多细节 <fscounters>` 中讨论。)

:ref:`Quota usage <quotacheck>` and :ref:`file link count <nlinks>`
checking are sufficiently complicated to warrant separate sections.
(:ref:`配额使用 <quotacheck>` 和 :ref:`文件链接计数 <nlinks>` 检查相当复杂，值得单独设节。)

Post-Repair Reverification
(修复后重新校验)
``````````````````````````

After performing a repair, the checking code is run a second time to validate
the new structure, and the results of the health assessment are recorded
internally and returned to the calling process.
This step is critical for enabling system administrator to monitor the status
of the filesystem and the progress of any repairs.
For developers, it is a useful means to judge the efficacy of error detection
and correction in the online and offline checking tools.
(执行修复之后，检查代码会再次运行以校验新结构，健康评估的结果被记录在内部并返回给调用进程。这一步骤对于让系统管理员能够监控文件系统状态和任何修复的进度至关重要。对开发者而言，它是评判在线和离线检查工具错误检测与纠正效果的有用手段。)

Eventual Consistency vs. Online Fsck
(最终一致性与在线 fsck)
------------------------------------

Complex operations can make modifications to multiple per-AG data structures
with a chain of transactions.
These chains, once committed to the log, are restarted during log recovery if
the system crashes while processing the chain.
Because the AG header buffers are unlocked between transactions within a chain,
online checking must coordinate with chained operations that are in progress to
avoid incorrectly detecting inconsistencies due to pending chains.
Furthermore, online repair must not run when operations are pending because
the metadata are temporarily inconsistent with each other, and rebuilding is
not possible.
(复杂操作可以通过一串事务对多个每 AG 数据结构进行修改。这些链条一旦提交到日志，如果系统在处理链条期间崩溃，会在日志恢复期间被重启。由于链条内各事务之间 AG 头缓冲是解锁的，在线检查必须与正在进行的链式操作协调，以避免因待处理链条而错误地检测到不一致。此外，当操作待处理时，在线修复绝不能运行，因为元数据彼此暂时不一致，重建是不可能的。)

Only online fsck has this requirement of total consistency of AG metadata, and
should be relatively rare as compared to filesystem change operations.
Online fsck coordinates with transaction chains as follows:
(只有在线 fsck 有这种对 AG 元数据完全一致性的要求，并且与文件系统更改操作相比应该相对少见。在线 fsck 按如下方式与事务链条协调：)

* For each AG, maintain a count of intent items targeting that AG.
  The count should be bumped whenever a new item is added to the chain.
  The count should be dropped when the filesystem has locked the AG header
  buffers and finished the work.
  (对于每个 AG，维护一个针对该 AG 的意图项计数。每当有新项加入链条时，该计数应当递增。当文件系统已锁定 AG 头缓冲并完成工作时，该计数应当递减。)

* When online fsck wants to examine an AG, it should lock the AG header
  buffers to quiesce all transaction chains that want to modify that AG.
  If the count is zero, proceed with the checking operation.
  If it is nonzero, cycle the buffer locks to allow the chain to make forward
  progress.
  (当在线 fsck 想要检查一个 AG 时，它应当锁定 AG 头缓冲，以静默所有想要修改该 AG 的事务链条。如果计数为零，则继续执行检查操作。如果计数非零，则循环缓冲锁，让链条得以向前推进。)

This may lead to online fsck taking a long time to complete, but regular
filesystem updates take precedence over background checking activity.
Details about the discovery of this situation are presented in the
:ref:`next section <chain_coordination>`, and details about the solution
are presented :ref:`after that<intent_drains>`.
(这可能导致在线 fsck 需要很长时间才能完成，但常规文件系统更新优先于后台检查活动。关于发现这一情况的细节在 :ref:`下一节 <chain_coordination>` 中介绍，解决方案的细节在 :ref:`其后 <intent_drains>` 介绍。)

.. _chain_coordination:

Discovery of the Problem
(问题的发现)
````````````````````````

Midway through the development of online scrubbing, the fsstress tests
uncovered a misinteraction between online fsck and compound transaction chains
created by other writer threads that resulted in false reports of metadata
inconsistency.
The root cause of these reports is the eventual consistency model introduced by
the expansion of deferred work items and compound transaction chains when
reverse mapping and reflink were introduced.
(在在线 scrubbing 开发的中期，fsstress 测试发现了在线 fsck 与其他写线程创建的复合事务链条之间的错误交互，导致对元数据不一致的误报。这些误报的根本原因是在引入反向映射和 reflink 时，延迟工作项和复合事务链条的扩展所引入的最终一致性模型。)

Originally, transaction chains were added to XFS to avoid deadlocks when
unmapping space from files.
Deadlock avoidance rules require that AGs only be locked in increasing order,
which makes it impossible (say) to use a single transaction to free a space
extent in AG 7 and then try to free a now superfluous block mapping btree block
in AG 3.
To avoid these kinds of deadlocks, XFS creates Extent Freeing Intent (EFI) log
items to commit to freeing some space in one transaction while deferring the
actual metadata updates to a fresh transaction.
The transaction sequence looks like this:
(最初，事务链条被添加到 XFS 中是为了避免从文件取消映射空间时出现死锁。死锁避免规则要求 AG 只能按递增顺序锁定，这使得(比如说)无法用一个事务释放 AG 7 中的一个空间区段，然后尝试释放 AG 3 中一个现已多余的块映射 btree 块。为了避免这类死锁，XFS 创建区段释放意图(EFI)日志项，在一个事务中承诺释放一些空间，同时将实际的元数据更新推迟到一个新事务。事务序列如下所示：)

1. The first transaction contains a physical update to the file's block mapping
   structures to remove the mapping from the btree blocks.
   It then attaches to the in-memory transaction an action item to schedule
   deferred freeing of space.
   Concretely, each transaction maintains a list of ``struct
   xfs_defer_pending`` objects, each of which maintains a list of ``struct
   xfs_extent_free_item`` objects.
   Returning to the example above, the action item tracks the freeing of both
   the unmapped space from AG 7 and the block mapping btree (BMBT) block from
   AG 3.
   Deferred frees recorded in this manner are committed in the log by creating
   an EFI log item from the ``struct xfs_extent_free_item`` object and
   attaching the log item to the transaction.
   When the log is persisted to disk, the EFI item is written into the ondisk
   transaction record.
   EFIs can list up to 16 extents to free, all sorted in AG order.
   (第一个事务包含对文件块映射结构的物理更新，以从 btree 块中移除该映射。然后它向内存中的事务附加一个动作项，以调度延迟的空间释放。具体而言，每个事务维护一个 ``struct xfs_defer_pending`` 对象列表，每个对象维护一个 ``struct xfs_extent_free_item`` 对象列表。回到上面的例子，动作项跟踪从 AG 7 取消映射的空间和 AG 3 的块映射 btree(BMBT)块两者的释放。以这种方式记录的延迟释放通过从 ``struct xfs_extent_free_item`` 对象创建 EFI 日志项并将该日志项附加到事务来提交到日志。当日志被持久化到磁盘时，EFI 项被写入磁盘事务记录。EFI 最多可以列出 16 个待释放区段，全部按 AG 顺序排序。)

2. The second transaction contains a physical update to the free space btrees
   of AG 3 to release the former BMBT block and a second physical update to the
   free space btrees of AG 7 to release the unmapped file space.
   Observe that the physical updates are resequenced in the correct order
   when possible.
   Attached to the transaction is a an extent free done (EFD) log item.
   The EFD contains a pointer to the EFI logged in transaction #1 so that log
   recovery can tell if the EFI needs to be replayed.
   (第二个事务包含对 AG 3 空闲空间 btree 的物理更新以释放原来的 BMBT 块，以及第二个物理更新到 AG 7 的空闲空间 btree 以释放取消映射的文件空间。注意，物理更新在可能的情况下会按正确顺序重新排序。附加到该事务的是一个区段释放完成(EFD)日志项。EFD 包含一个指向事务 #1 中记录的 EFI 的指针，以便日志恢复可以判断 EFI 是否需要重放。)

If the system goes down after transaction #1 is written back to the filesystem
but before #2 is committed, a scan of the filesystem metadata would show
inconsistent filesystem metadata because there would not appear to be any owner
of the unmapped space.
Happily, log recovery corrects this inconsistency for us -- when recovery finds
an intent log item but does not find a corresponding intent done item, it will
reconstruct the incore state of the intent item and finish it.
In the example above, the log must replay both frees described in the recovered
EFI to complete the recovery phase.
(如果系统在事务 #1 被写回文件系统之后、#2 被提交之前宕机，对文件系统元数据的扫描将显示不一致的文件系统元数据，因为取消映射的空间似乎没有任何属主。幸运的是，日志恢复会为我们纠正这种不一致——当恢复发现一个意图日志项但没有找到对应的意图完成项时，它会重建该意图项的内存状态并完成它。在上面的例子中，日志必须重放恢复的 EFI 中描述的两次释放，以完成恢复阶段。)

There are subtleties to XFS' transaction chaining strategy to consider:
(XFS 的事务链接策略有一些需要考虑的细节：)

* Log items must be added to a transaction in the correct order to prevent
  conflicts with principal objects that are not held by the transaction.
  In other words, all per-AG metadata updates for an unmapped block must be
  completed before the last update to free the extent, and extents should not
  be reallocated until that last update commits to the log.
  (日志项必须以正确的顺序添加到事务中，以防止与事务未持有的主体对象发生冲突。换句话说，取消映射块的每 AG 元数据更新都必须在释放该区段的最后一次更新之前完成，并且在该最后一次更新提交到日志之前，不应重新分配区段。)

* AG header buffers are released between each transaction in a chain.
  This means that other threads can observe an AG in an intermediate state,
  but as long as the first subtlety is handled, this should not affect the
  correctness of filesystem operations.
  (AG 头缓冲在链条的每个事务之间被释放。这意味着其他线程可以观察到处于中间状态的 AG，但只要处理好了第一个细节，这应该不会影响文件系统操作的正确性。)

* Unmounting the filesystem flushes all pending work to disk, which means that
  offline fsck never sees the temporary inconsistencies caused by deferred
  work item processing.
  (卸载文件系统会将所有待处理工作刷新到磁盘，这意味着离线 fsck 永远看不到延迟工作项处理造成的暂时性不一致。)

In this manner, XFS employs a form of eventual consistency to avoid deadlocks
and increase parallelism.
(以这种方式，XFS 采用了一种最终一致性形式来避免死锁并提高并行度。)

During the design phase of the reverse mapping and reflink features, it was
decided that it was impractical to cram all the reverse mapping updates for a
single filesystem change into a single transaction because a single file
mapping operation can explode into many small updates:
(在反向映射和 reflink 特性的设计阶段，人们认为把单个文件系统更改的所有反向映射更新塞进单个事务是不切实际的，因为单个文件映射操作可能爆炸成许多小更新：)

* The block mapping update itself
  (块映射更新本身)
* A reverse mapping update for the block mapping update
  (针对块映射更新的反向映射更新)
* Fixing the freelist
  (修复空闲列表)
* A reverse mapping update for the freelist fix
  (针对空闲列表修复的反向映射更新)

* A shape change to the block mapping btree
  (块映射 btree 的形状改变)
* A reverse mapping update for the btree update
  (针对 btree 更新的反向映射更新)
* Fixing the freelist (again)
  (修复空闲列表(再次))
* A reverse mapping update for the freelist fix
  (针对空闲列表修复的反向映射更新)

* An update to the reference counting information
  (引用计数信息的更新)
* A reverse mapping update for the refcount update
  (针对引用计数更新的反向映射更新)
* Fixing the freelist (a third time)
  (修复空闲列表(第三次))
* A reverse mapping update for the freelist fix
  (针对空闲列表修复的反向映射更新)

* Freeing any space that was unmapped and not owned by any other file
  (释放任何被取消映射且不被任何其他文件拥有的空间)
* Fixing the freelist (a fourth time)
  (修复空闲列表(第四次))
* A reverse mapping update for the freelist fix
  (针对空闲列表修复的反向映射更新)

* Freeing the space used by the block mapping btree
  (释放块映射 btree 使用的空间)
* Fixing the freelist (a fifth time)
  (修复空闲列表(第五次))
* A reverse mapping update for the freelist fix
  (针对空闲列表修复的反向映射更新)

Free list fixups are not usually needed more than once per AG per transaction
chain, but it is theoretically possible if space is very tight.
For copy-on-write updates this is even worse, because this must be done once to
remove the space from a staging area and again to map it into the file!
(空闲列表修补通常在每条事务链中每个 AG 不需要超过一次，但如果空间非常紧张，理论上可能需要更多次。对于写时复制更新，情况甚至更糟，因为这必须做一次以将空间从暂存区移除，再做一次以将其映射进文件！)

To deal with this explosion in a calm manner, XFS expands its use of deferred
work items to cover most reverse mapping updates and all refcount updates.
This reduces the worst case size of transaction reservations by breaking the
work into a long chain of small updates, which increases the degree of eventual
consistency in the system.
Again, this generally isn't a problem because XFS orders its deferred work
items carefully to avoid resource reuse conflicts between unsuspecting threads.
(为了从容应对这种爆炸，XFS 扩大了延迟工作项的使用范围，以覆盖大多数反向映射更新和所有引用计数更新。这通过把工作分解成一长串小更新来减少事务预留的最坏情况大小，这提高了系统中最终一致性的程度。同样，这通常不是问题，因为 XFS 仔细地为延迟工作项排序，以避免毫不知情的线程之间的资源重用冲突。)

However, online fsck changes the rules -- remember that although physical
updates to per-AG structures are coordinated by locking the buffers for AG
headers, buffer locks are dropped between transactions.
Once scrub acquires resources and takes locks for a data structure, it must do
all the validation work without releasing the lock.
If the main lock for a space btree is an AG header buffer lock, scrub may have
interrupted another thread that is midway through finishing a chain.
For example, if a thread performing a copy-on-write has completed a reverse
mapping update but not the corresponding refcount update, the two AG btrees
will appear inconsistent to scrub and an observation of corruption will be
recorded.  This observation will not be correct.
If a repair is attempted in this state, the results will be catastrophic!
(然而，在线 fsck 改变了规则——请记住，尽管每 AG 结构的物理更新通过锁定 AG 头缓冲来协调，但缓冲锁在事务之间会被释放。一旦 scrub 获取资源并为一个数据结构取得锁，它就必须在不释放锁的情况下完成所有校验工作。如果一个空间 btree 的主锁是 AG 头缓冲锁，scrub 可能中断了另一个正在完成链条中途的线程。例如，如果执行写时复制的线程已完成反向映射更新但尚未完成相应的引用计数更新，这两个 AG btree 在 scrub 看来将不一致，并会记录一次损坏观察。这一观察将是不正确的。如果在这种状态下尝试修复，结果将是灾难性的！)

Several other solutions to this problem were evaluated upon discovery of this
flaw and rejected:
(在发现这一缺陷后，评估了几种其他解决方案并被否决：)

1. Add a higher level lock to allocation groups and require writer threads to
   acquire the higher level lock in AG order before making any changes.
   This would be very difficult to implement in practice because it is
   difficult to determine which locks need to be obtained, and in what order,
   without simulating the entire operation.
   Performing a dry run of a file operation to discover necessary locks would
   make the filesystem very slow.
   (为分配组添加更高层的锁，并要求写线程在进行任何更改之前按 AG 顺序获取更高层的锁。这在实践中将非常难以实现，因为如果不模拟整个操作，很难确定需要获取哪些锁以及按什么顺序。通过试运行文件操作来发现必要的锁会使文件系统非常慢。)

2. Make the deferred work coordinator code aware of consecutive intent items
   targeting the same AG and have it hold the AG header buffers locked across
   the transaction roll between updates.
   This would introduce a lot of complexity into the coordinator since it is
   only loosely coupled with the actual deferred work items.
   It would also fail to solve the problem because deferred work items can
   generate new deferred subtasks, but all subtasks must be complete before
   work can start on a new sibling task.
   (让延迟工作协调器代码感知针对同一 AG 的连续意图项，并让它在更新之间的事务滚动期间保持 AG 头缓冲被锁定。这会给协调器引入大量复杂度，因为它与实际的延迟工作项只有松散的耦合。它也无法解决问题，因为延迟工作项可以生成新的延迟子任务，但在新兄弟任务开始工作之前，所有子任务必须完成。)

3. Teach online fsck to walk all transactions waiting for whichever lock(s)
   protect the data structure being scrubbed to look for pending operations.
   The checking and repair operations must factor these pending operations into
   the evaluations being performed.
   This solution is a nonstarter because it is *extremely* invasive to the main
   filesystem.
   (教会在线 fsck 遍历所有正在等待保护被 scrub 数据结构的任意锁的事务，以寻找待处理的操作。检查和修复操作必须把这些待处理操作纳入正在执行的评估中。这一方案不可行，因为它对主文件系统*极其*具有侵入性。)

.. _intent_drains:

Intent Drains
(意图排空)
`````````````

Online fsck uses an atomic intent item counter and lock cycling to coordinate
with transaction chains.
There are two key properties to the drain mechanism.
First, the counter is incremented when a deferred work item is *queued* to a
transaction, and it is decremented after the associated intent done log item is
*committed* to another transaction.
The second property is that deferred work can be added to a transaction without
holding an AG header lock, but per-AG work items cannot be marked done without
locking that AG header buffer to log the physical updates and the intent done
log item.
The first property enables scrub to yield to running transaction chains, which
is an explicit deprioritization of online fsck to benefit file operations.
The second property of the drain is key to the correct coordination of scrub,
since scrub will always be able to decide if a conflict is possible.
(在线 fsck 使用一个原子的意图项计数器和锁循环来与事务链条协调。排空(drain)机制有两个关键属性。第一，当延迟工作项被*排队*到一个事务时，计数器递增；在相关联的意图完成日志项被*提交*到另一个事务之后，计数器递减。第二个属性是，延迟工作可以在不持有 AG 头锁的情况下添加到事务中，但每 AG 工作项如果不锁定该 AG 头缓冲以记录物理更新和意图完成日志项，就不能被标记为完成。第一个属性使 scrub 能够让位于正在运行的事务链条，这是为有利于文件操作而明确降低在线 fsck 的优先级。排空的第二个属性是 scrub 正确协调的关键，因为 scrub 将始终能够判断冲突是否可能发生。)

For regular filesystem code, the drain works as follows:
(对于常规文件系统代码，排空按如下方式工作：)

1. Call the appropriate subsystem function to add a deferred work item to a
   transaction.
   (调用适当的子系统函数将一个延迟工作项添加到事务中。)

2. The function calls ``xfs_defer_drain_bump`` to increase the counter.
   (该函数调用 ``xfs_defer_drain_bump`` 来增加计数器。)

3. When the deferred item manager wants to finish the deferred work item, it
   calls ``->finish_item`` to complete it.
   (当延迟项管理器想要完成该延迟工作项时，它调用 ``->finish_item`` 来完成它。)

4. The ``->finish_item`` implementation logs some changes and calls
   ``xfs_defer_drain_drop`` to decrease the sloppy counter and wake up any threads
   waiting on the drain.
   (``->finish_item`` 实现记录一些更改，并调用 ``xfs_defer_drain_drop`` 来减少这个不精确的计数器，并唤醒任何正在等待排空的线程。)

5. The subtransaction commits, which unlocks the resource associated with the
   intent item.
   (子事务提交，从而解锁与意图项关联的资源。)

For scrub, the drain works as follows:
(对于 scrub，排空按如下方式工作：)

1. Lock the resource(s) associated with the metadata being scrubbed.
   For example, a scan of the refcount btree would lock the AGI and AGF header
   buffers.
   (锁定与被 scrub 元数据关联的资源。例如，对引用计数 btree 的扫描会锁定 AGI 和 AGF 头缓冲。)

2. If the counter is zero (``xfs_defer_drain_busy`` returns false), there are no
   chains in progress and the operation may proceed.
   (如果计数器为零(``xfs_defer_drain_busy`` 返回 false)，则没有正在进行的链条，操作可以进行。)

3. Otherwise, release the resources grabbed in step 1.
   (否则，释放在步骤 1 中获取的资源。)

4. Wait for the intent counter to reach zero (``xfs_defer_drain_intents``), then go
   back to step 1 unless a signal has been caught.
   (等待意图计数器达到零(``xfs_defer_drain_intents``)，然后回到步骤 1，除非捕获到信号。)

To avoid polling in step 4, the drain provides a waitqueue for scrub threads to
be woken up whenever the intent count drops to zero.
(为了避免在步骤 4 中轮询，排空提供了一个等待队列，每当意图计数降为零时，scrub 线程就会被唤醒。)

.. _jump_labels:

Static Keys (aka Jump Label Patching)
(静态键(又名跳转标签修补))
`````````````````````````````````````

Online fsck for XFS separates the regular filesystem from the checking and
repair code as much as possible.
However, there are a few parts of online fsck (such as the intent drains, and
later, live update hooks) where it is useful for the online fsck code to know
what's going on in the rest of the filesystem.
Since it is not expected that online fsck will be constantly running in the
background, it is very important to minimize the runtime overhead imposed by
these hooks when online fsck is compiled into the kernel but not actively
running on behalf of userspace.
Taking locks in the hot path of a writer thread to access a data structure only
to find that no further action is necessary is expensive -- on the author's
computer, this have an overhead of 40-50ns per access.
Fortunately, the kernel supports dynamic code patching, which enables XFS to
replace a static branch to hook code with ``nop`` sleds when online fsck isn't
running.
This sled has an overhead of however long it takes the instruction decoder to
skip past the sled, which seems to be on the order of less than 1ns and
does not access memory outside of instruction fetching.
(XFS 的在线 fsck 尽可能地将常规文件系统与检查和修复代码分离。然而，在线 fsck 的少数几个部分(例如意图排空，以及后来的实时更新挂钩)需要让在线 fsck 代码了解文件系统其余部分正在发生什么。由于并不期望在线 fsck 一直在后台运行，因此当在线 fsck 被编译进内核但没有代表用户空间主动运行时，尽量减少这些挂钩强加的运行时开销非常重要。在写线程的热路径中获取锁以访问一个数据结构，结果却发现无需进一步操作，这是昂贵的——在作者的计算机上，每次访问的开销为 40-50ns。幸运的是，内核支持动态代码修补，这使 XFS 能够在在线 fsck 未运行时用 ``nop`` 指令序列替换指向挂钩代码的静态分支。这个指令序列的开销只是指令解码器跳过它所需的时间，似乎小于 1ns 的量级，并且在取指令之外不访问内存。)

When online fsck enables the static key, the sled is replaced with an
unconditional branch to call the hook code.
The switchover is quite expensive (~22000ns) but is paid entirely by the
program that invoked online fsck, and can be amortized if multiple threads
enter online fsck at the same time, or if multiple filesystems are being
checked at the same time.
Changing the branch direction requires taking the CPU hotplug lock, and since
CPU initialization requires memory allocation, online fsck must be careful not
to change a static key while holding any locks or resources that could be
accessed in the memory reclaim paths.
To minimize contention on the CPU hotplug lock, care should be taken not to
enable or disable static keys unnecessarily.
(当在线 fsck 启用静态键时，该指令序列被替换为调用挂钩代码的无条件分支。切换相当昂贵(约 22000ns)，但完全由调用在线 fsck 的程序承担，并且如果多个线程同时进入在线 fsck，或者同时检查多个文件系统，则可以摊销。改变分支方向需要获取 CPU 热插拔锁，由于 CPU 初始化需要内存分配，在线 fsck 必须小心，不要在持有任何可能被内存回收路径访问的锁或资源的情况下更改静态键。为了尽量减少对 CPU 热插拔锁的争用，应注意不要不必要地启用或禁用静态键。)

Because static keys are intended to minimize hook overhead for regular
filesystem operations when xfs_scrub is not running, the intended usage
patterns are as follows:
(由于静态键的目的是在 xfs_scrub 未运行时尽量减少常规文件系统操作的挂钩开销，预期的使用模式如下：)

- The hooked part of XFS should declare a static-scoped static key that
  defaults to false.
  The ``DEFINE_STATIC_KEY_FALSE`` macro takes care of this.
  The static key itself should be declared as a ``static`` variable.
  (XFS 中被挂钩的部分应声明一个默认为 false 的静态作用域静态键。``DEFINE_STATIC_KEY_FALSE`` 宏负责此事。静态键本身应声明为 ``static`` 变量。)

- When deciding to invoke code that's only used by scrub, the regular
  filesystem should call the ``static_branch_unlikely`` predicate to avoid the
  scrub-only hook code if the static key is not enabled.
  (在决定调用仅由 scrub 使用的代码时，常规文件系统应调用 ``static_branch_unlikely`` 谓词，以便在静态键未启用时避开仅限 scrub 的挂钩代码。)

- The regular filesystem should export helper functions that call
  ``static_branch_inc`` to enable and ``static_branch_dec`` to disable the
  static key.
  Wrapper functions make it easy to compile out the relevant code if the kernel
  distributor turns off online fsck at build time.
  (常规文件系统应导出辅助函数，调用 ``static_branch_inc`` 来启用静态键，调用 ``static_branch_dec`` 来禁用静态键。如果内核发行商在构建时关闭在线 fsck，包装函数使得相关代码很容易被编译排除。)

- Scrub functions wanting to turn on scrub-only XFS functionality should call
  the ``xchk_fsgates_enable`` from the setup function to enable a specific
  hook.
  This must be done before obtaining any resources that are used by memory
  reclaim.
  Callers had better be sure they really need the functionality gated by the
  static key; the ``TRY_HARDER`` flag is useful here.
  (想要开启仅限 scrub 的 XFS 功能的 scrub 函数应在设置函数中调用 ``xchk_fsgates_enable`` 来启用特定的挂钩。这必须在获取任何被内存回收使用的资源之前完成。调用者最好确保自己真的需要由静态键门控的功能；``TRY_HARDER`` 标志在这里很有用。)

Online scrub has resource acquisition helpers (e.g. ``xchk_perag_lock``) to
handle locking AGI and AGF buffers for all scrubber functions.
If it detects a conflict between scrub and the running transactions, it will
try to wait for intents to complete.
If the caller of the helper has not enabled the static key, the helper will
return -EDEADLOCK, which should result in the scrub being restarted with the
``TRY_HARDER`` flag set.
The scrub setup function should detect that flag, enable the static key, and
try the scrub again.
Scrub teardown disables all static keys obtained by ``xchk_fsgates_enable``.
(在线 scrub 有资源获取辅助函数(例如 ``xchk_perag_lock``)来为所有 scrubber 函数处理 AGI 和 AGF 缓冲的锁定。如果它检测到 scrub 与正在运行的事务之间存在冲突，它会尝试等待意图完成。如果辅助函数的调用者尚未启用静态键，辅助函数将返回 -EDEADLOCK，这应导致 scrub 在设置了 ``TRY_HARDER`` 标志的情况下重新启动。scrub 设置函数应检测到该标志，启用静态键，然后再次尝试 scrub。scrub 清理会禁用由 ``xchk_fsgates_enable`` 获取的所有静态键。)

For more information, please see the kernel documentation of
Documentation/staging/static-keys.rst.
(更多信息请参阅 Documentation/staging/static-keys.rst 的内核文档。)

.. _xfile:

Pageable Kernel Memory
(可分页的内核内存)
----------------------

Some online checking functions work by scanning the filesystem to build a
shadow copy of an ondisk metadata structure in memory and comparing the two
copies.
For online repair to rebuild a metadata structure, it must compute the record
set that will be stored in the new structure before it can persist that new
structure to disk.
Ideally, repairs complete with a single atomic commit that introduces
a new data structure.
To meet these goals, the kernel needs to collect a large amount of information
in a place that doesn't require the correct operation of the filesystem.
(一些在线检查函数的工作方式是扫描文件系统，在内存中构建磁盘元数据结构的影子副本并比较两个副本。在线修复要重建元数据结构，必须先计算出将存储在新结构中的记录集，才能将该新结构持久化到磁盘。理想情况下，修复通过引入新数据结构的单次原子提交完成。为实现这些目标，内核需要在一个不依赖文件系统正确运行的地方收集大量信息。)

Kernel memory isn't suitable because:
(内核内存并不合适，因为：)

* Allocating a contiguous region of memory to create a C array is very
  difficult, especially on 32-bit systems.
  (分配一段连续的内存区域来创建 C 数组非常困难，尤其是在 32 位系统上。)

* Linked lists of records introduce double pointer overhead which is very high
  and eliminate the possibility of indexed lookups.
  (记录的链表引入了非常高的双重指针开销，并消除了索引查找的可能性。)

* Kernel memory is pinned, which can drive the system into OOM conditions.
  (内核内存被钉住，这可能导致系统进入 OOM 状态。)

* The system might not have sufficient memory to stage all the information.
  (系统可能没有足够的内存来暂存所有信息。)

At any given time, online fsck does not need to keep the entire record set in
memory, which means that individual records can be paged out if necessary.
Continued development of online fsck demonstrated that the ability to perform
indexed data storage would also be very useful.
Fortunately, the Linux kernel already has a facility for byte-addressable and
pageable storage: tmpfs.
In-kernel graphics drivers (most notably i915) take advantage of tmpfs files
to store intermediate data that doesn't need to be in memory at all times, so
that usage precedent is already established.
Hence, the ``xfile`` was born!
(在任何给定时刻，在线 fsck 都不需要将整个记录集保留在内存中，这意味着如有必要，单个记录可以被换出。在线 fsck 的持续开发表明，执行索引化数据存储的能力也将非常有用。幸运的是，Linux 内核已经有一个可按字节寻址且可分页的存储设施：tmpfs。内核内图形驱动程序(最著名的是 i915)利用 tmpfs 文件来存储不需要一直驻留内存的中间数据，因此这一使用先例早已确立。于是，``xfile`` 诞生了！)

+--------------------------------------------------------------------------+
| **Historical Sidebar**:                                                  |
| (历史侧栏：)                                                                  |
+--------------------------------------------------------------------------+
| The first edition of online repair inserted records into a new btree as  |
| it found them, which failed because filesystem could shut down with a    |
| built data structure, which would be live after recovery finished.       |
|                                                                          |
| (在线修复的第一版在发现记录时将其插入新的 btree，)                                            |
| (但由于文件系统可能在数据结构已构建的情况下关闭，且该结构在恢复完成后仍然存活，因此失败了。)                          |
|                                                                          |
| The second edition solved the half-rebuilt structure problem by storing  |
| everything in memory, but frequently ran the system out of memory.       |
|                                                                          |
| (第二版通过将所有内容存储在内存中解决了半重建结构的问题，但经常把系统内存耗尽。)                                |
|                                                                          |
| The third edition solved the OOM problem by using linked lists, but the  |
| memory overhead of the list pointers was extreme.                        |
| (第三版通过使用链表解决了 OOM 问题，但链表指针的内存开销极大。)                                      |
+--------------------------------------------------------------------------+

xfile Access Models
(xfile 访问模型)
```````````````````

A survey of the intended uses of xfiles suggested these use cases:
(对 xfile 预期用途的调查表明了以下用例：)

1. Arrays of fixed-sized records (space management btrees, directory and
   extended attribute entries)
   (固定大小记录的数组(空间管理 btree、目录和扩展属性条目))

2. Sparse arrays of fixed-sized records (quotas and link counts)
   (固定大小记录的稀疏数组(配额和链接计数))

3. Large binary objects (BLOBs) of variable sizes (directory and extended
   attribute names and values)
   (可变大小的大型二进制对象(BLOB)(目录和扩展属性的名称与值))

4. Staging btrees in memory (reverse mapping btrees)
   (内存中的暂存 btree(反向映射 btree))

5. Arbitrary contents (realtime space management)
   (任意内容(实时空间管理))

To support the first four use cases, high level data structures wrap the xfile
to share functionality between online fsck functions.
The rest of this section discusses the interfaces that the xfile presents to
four of those five higher level data structures.
The fifth use case is discussed in the :ref:`realtime summary <rtsummary>` case
study.
(为支持前四个用例，高层级数据结构包装了 xfile，以便在在线 fsck 函数之间共享功能。本节其余部分讨论 xfile 提供给这五个高层级数据结构中四个的接口。第五个用例在 :ref:`实时摘要 <rtsummary>` 案例研究中讨论。)

XFS is very record-based, which suggests that the ability to load and store
complete records is important.
To support these cases, a pair of ``xfile_load`` and ``xfile_store``
functions are provided to read and persist objects into an xfile that treat any
error as an out of memory error.  For online repair, squashing error conditions
in this manner is an acceptable behavior because the only reaction is to abort
the operation back to userspace.
(XFS 是非常基于记录的，这表明加载和存储完整记录的能力很重要。为支持这些情况，提供了一对 ``xfile_load`` 和 ``xfile_store`` 函数来读取对象并将其持久化到 xfile 中，它们将任何错误都视为内存不足错误。对于在线修复，以这种方式压扁错误条件是可接受的行为，因为唯一的反应是中止操作并返回用户空间。)

However, no discussion of file access idioms is complete without answering the
question, "But what about mmap?"
It is convenient to access storage directly with pointers, just like userspace
code does with regular memory.
Online fsck must not drive the system into OOM conditions, which means that
xfiles must be responsive to memory reclamation.
tmpfs can only push a pagecache folio to the swap cache if the folio is neither
pinned nor locked, which means the xfile must not pin too many folios.
(然而，如果不回答"那 mmap 呢？"这个问题，对文件访问惯用法的讨论就不完整。像用户空间代码使用常规内存那样，用指针直接访问存储是很方便的。在线 fsck 不得把系统推进 OOM 状态，这意味着 xfile 必须对内存回收作出响应。tmpfs 只有在页缓存 folio 既未被钉住也未被锁定的情况下才能将其推入交换缓存，这意味着 xfile 不得钉住太多 folio。)

Short term direct access to xfile contents is done by locking the pagecache
folio and mapping it into kernel address space.  Object load and store uses this
mechanism.  Folio locks are not supposed to be held for long periods of time, so
long term direct access to xfile contents is done by bumping the folio refcount,
mapping it into kernel address space, and dropping the folio lock.
These long term users *must* be responsive to memory reclaim by hooking into
the shrinker infrastructure to know when to release folios.
(对 xfile 内容的短期直接访问通过锁定页缓存 folio 并将其映射到内核地址空间来完成。对象的加载和存储使用这种机制。folio 锁不应被长时间持有，因此对 xfile 内容的长期直接访问通过提升 folio 引用计数、将其映射到内核地址空间，然后释放 folio 锁来完成。这些长期使用者*必须*通过挂钩 shrinker 基础设施来响应内存回收，以知道何时释放 folio。)

The ``xfile_get_folio`` and ``xfile_put_folio`` functions are provided to
retrieve the (locked) folio that backs part of an xfile and to release it.
The only code to use these folio lease functions are the xfarray
:ref:`sorting<xfarray_sort>` algorithms and the :ref:`in-memory
btrees<xfbtree>`.
(提供 ``xfile_get_folio`` 和 ``xfile_put_folio`` 函数来取回支撑 xfile 某部分的(已锁定的)folio 并释放它。唯一使用这些 folio 租借函数的代码是 xfarray 的 :ref:`排序 <xfarray_sort>` 算法和 :ref:`内存中 btree <xfbtree>`。)

xfile Access Coordination
(xfile 访问协调)
`````````````````````````

For security reasons, xfiles must be owned privately by the kernel.
They are marked ``S_PRIVATE`` to prevent interference from the security system,
must never be mapped into process file descriptor tables, and their pages must
never be mapped into userspace processes.
(出于安全原因，xfile 必须由内核私有拥有。它们被标记为 ``S_PRIVATE`` 以防止安全系统的干扰，绝不能被映射到进程文件描述符表中，并且它们的页面绝不能被映射到用户空间进程。)

To avoid locking recursion issues with the VFS, all accesses to the shmfs file
are performed by manipulating the page cache directly.
xfile writers call the ``->write_begin`` and ``->write_end`` functions of the
xfile's address space to grab writable pages, copy the caller's buffer into the
page, and release the pages.
xfile readers call ``shmem_read_mapping_page_gfp`` to grab pages directly
before copying the contents into the caller's buffer.
In other words, xfiles ignore the VFS read and write code paths to avoid
having to create a dummy ``struct kiocb`` and to avoid taking inode and
freeze locks.
tmpfs cannot be frozen, and xfiles must not be exposed to userspace.
(为了避免与 VFS 的锁定递归问题，对 shmfs 文件的所有访问都通过直接操作页缓存来执行。xfile 写入者调用 xfile 地址空间的 ``->write_begin`` 和 ``->write_end`` 函数来获取可写页面、将调用者的缓冲复制到页面中，然后释放页面。xfile 读取者调用 ``shmem_read_mapping_page_gfp`` 直接获取页面，然后再将内容复制到调用者的缓冲中。换句话说，xfile 忽略 VFS 的读写代码路径，以避免必须创建虚拟 ``struct kiocb``，并避免获取 inode 锁和冻结锁。tmpfs 不能被冻结，且 xfile 不得暴露给用户空间。)

If an xfile is shared between threads to stage repairs, the caller must provide
its own locks to coordinate access.
For example, if a scrub function stores scan results in an xfile and needs
other threads to provide updates to the scanned data, the scrub function must
provide a lock for all threads to share.
(如果一个 xfile 在线程之间共享以暂存修复，调用者必须提供自己的锁来协调访问。例如，如果某个 scrub 函数将扫描结果存储在 xfile 中，并需要其他线程为扫描的数据提供更新，则该 scrub 函数必须提供一个供所有线程共享的锁。)

.. _xfarray:

Arrays of Fixed-Sized Records
(固定大小记录的数组)
`````````````````````````````

In XFS, each type of indexed space metadata (free space, inodes, reference
counts, file fork space, and reverse mappings) consists of a set of fixed-size
records indexed with a classic B+ tree.
Directories have a set of fixed-size dirent records that point to the names,
and extended attributes have a set of fixed-size attribute keys that point to
names and values.
Quota counters and file link counters index records with numbers.
During a repair, scrub needs to stage new records during the gathering step and
retrieve them during the btree building step.
(在 XFS 中，每种类型的索引化空间元数据(空闲空间、inode、引用计数、文件 fork 空间和反向映射)都由一组用经典 B+ 树索引的固定大小记录组成。目录有一组指向名称的固定大小 dirent 记录，扩展属性有一组指向名称和值的固定大小属性键。配额计数器和文件链接计数器用数字索引记录。在修复期间，scrub 需要在收集步骤中暂存新记录，并在 btree 构建步骤中取回它们。)

Although this requirement can be satisfied by calling the read and write
methods of the xfile directly, it is simpler for callers for there to be a
higher level abstraction to take care of computing array offsets, to provide
iterator functions, and to deal with sparse records and sorting.
The ``xfarray`` abstraction presents a linear array for fixed-size records atop
the byte-accessible xfile.
(虽然这一要求可以通过直接调用 xfile 的读写方法来满足，但对调用者来说，有一个更高层的抽象来负责计算数组偏移、提供迭代器函数以及处理稀疏记录和排序会更简单。``xfarray`` 抽象在可按字节访问的 xfile 之上呈现一个用于固定大小记录的线性数组。)

.. _xfarray_access_patterns:

Array Access Patterns
(数组访问模式)
^^^^^^^^^^^^^^^^^^^^^

Array access patterns in online fsck tend to fall into three categories.
Iteration of records is assumed to be necessary for all cases and will be
covered in the next section.
(在线 fsck 中的数组访问模式往往分为三类。记录的迭代被假定为所有情况都需要，并将在下一节中介绍。)

The first type of caller handles records that are indexed by position.
Gaps may exist between records, and a record may be updated multiple times
during the collection step.
In other words, these callers want a sparse linearly addressed table file.
The typical use case are quota records or file link count records.
Access to array elements is performed programmatically via ``xfarray_load`` and
``xfarray_store`` functions, which wrap the similarly-named xfile functions to
provide loading and storing of array elements at arbitrary array indices.
Gaps are defined to be null records, and null records are defined to be a
sequence of all zero bytes.
Null records are detected by calling ``xfarray_element_is_null``.
They are created either by calling ``xfarray_unset`` to null out an existing
record or by never storing anything to an array index.
(第一类调用者处理按位置索引的记录。记录之间可能存在间隙，并且在收集步骤中一条记录可能被更新多次。换句话说，这些调用者想要一个稀疏的线性寻址表文件。典型的用例是配额记录或文件链接计数记录。对数组元素的访问通过 ``xfarray_load`` 和 ``xfarray_store`` 函数以编程方式执行，这两个函数包装了同名的 xfile 函数，以提供在任意数组索引处加载数组元素和存储数组元素的能力。间隙被定义为空记录，空记录被定义为全零字节序列。空记录通过调用 ``xfarray_element_is_null`` 来检测。它们或者通过调用 ``xfarray_unset`` 将现有记录置空来创建，或者通过从不向某个数组索引存储任何内容来创建。)

The second type of caller handles records that are not indexed by position
and do not require multiple updates to a record.
The typical use case here is rebuilding space btrees and key/value btrees.
These callers can add records to the array without caring about array indices
via the ``xfarray_append`` function, which stores a record at the end of the
array.
For callers that require records to be presentable in a specific order (e.g.
rebuilding btree data), the ``xfarray_sort`` function can arrange the sorted
records; this function will be covered later.
(第二类调用者处理不按位置索引、也不需要对一条记录多次更新的记录。这里的典型用例是重建空间 btree 和键/值 btree。这些调用者可以通过 ``xfarray_append`` 函数在不关心数组索引的情况下向数组添加记录，该函数将记录存储在数组末尾。对于要求记录以特定顺序呈现的调用者(例如重建 btree 数据)，``xfarray_sort`` 函数可以安排排序后的记录；该函数稍后介绍。)

The third type of caller is a bag, which is useful for counting records.
The typical use case here is constructing space extent reference counts from
reverse mapping information.
Records can be put in the bag in any order, they can be removed from the bag
at any time, and uniqueness of records is left to callers.
The ``xfarray_store_anywhere`` function is used to insert a record in any
null record slot in the bag; and the ``xfarray_unset`` function removes a
record from the bag.
(第三类调用者是一个袋子(bag)，它对记录计数很有用。这里的典型用例是从反向映射信息构造空间区段引用计数。记录可以按任意顺序放入袋子，可以随时从袋子中移除，记录的唯一性留给调用者。``xfarray_store_anywhere`` 函数用于在袋子中任何空记录槽位插入一条记录；``xfarray_unset`` 函数从袋子中移除一条记录。)

Iterating Array Elements
(迭代数组元素)
^^^^^^^^^^^^^^^^^^^^^^^^

Most users of the xfarray require the ability to iterate the records stored in
the array.
Callers can probe every possible array index with the following:
(xfarray 的大多数使用者都需要迭代数组中存储的记录的能力。调用者可以用以下方式探测每个可能的数组索引：)

.. code-block:: c

	xfarray_idx_t i;
	foreach_xfarray_idx(array, i) {
	    xfarray_load(array, i, &rec);

	    /* do something with rec */
	}

All users of this idiom must be prepared to handle null records or must already
know that there aren't any.
(这一惯用法的所有使用者都必须准备好处理空记录，或者必须已经知道不存在空记录。)

For xfarray users that want to iterate a sparse array, the ``xfarray_iter``
function ignores indices in the xfarray that have never been written to by
calling ``xfile_seek_data`` (which internally uses ``SEEK_DATA``) to skip areas
of the array that are not populated with memory pages.
Once it finds a page, it will skip the zeroed areas of the page.
(对于想要迭代稀疏数组的 xfarray 使用者，``xfarray_iter`` 函数通过调用 ``xfile_seek_data``(内部使用 ``SEEK_DATA``)来跳过数组中未填充内存页面的区域，从而忽略 xfarray 中从未被写入过的索引。一旦找到一个页面，它会跳过该页面中清零的区域。)

.. code-block:: c

	xfarray_idx_t i = XFARRAY_CURSOR_INIT;
	while ((ret = xfarray_iter(array, &i, &rec)) == 1) {
	    /* do something with rec */
	}

.. _xfarray_sort:

Sorting Array Elements
(排序数组元素)
^^^^^^^^^^^^^^^^^^^^^^

During the fourth demonstration of online repair, a community reviewer remarked
that for performance reasons, online repair ought to load batches of records
into btree record blocks instead of inserting records into a new btree one at a
time.
The btree insertion code in XFS is responsible for maintaining correct ordering
of the records, so naturally the xfarray must also support sorting the record
set prior to bulk loading.
(在在线修复的第四次演示期间，一位社区评审者指出，出于性能原因，在线修复应当成批地将记录加载到 btree 记录块中，而不是一次一条地将记录插入新的 btree。XFS 中的 btree 插入代码负责维护记录的正确顺序，因此 xfarray 自然也必须支持在批量加载之前对记录集排序。)

Case Study: Sorting xfarrays
(案例研究：排序 xfarray)
~~~~~~~~~~~~~~~~~~~~~~~~~~~~

The sorting algorithm used in the xfarray is actually a combination of adaptive
quicksort and a heapsort subalgorithm in the spirit of
`Sedgewick <https://algs4.cs.princeton.edu/23quicksort/>`_ and
`pdqsort <https://github.com/orlp/pdqsort>`_, with customizations for the Linux
kernel.
To sort records in a reasonably short amount of time, ``xfarray`` takes
advantage of the binary subpartitioning offered by quicksort, but it also uses
heapsort to hedge against performance collapse if the chosen quicksort pivots
are poor.
Both algorithms are (in general) O(n * lg(n)), but there is a wide performance
gulf between the two implementations.
(xfarray 中使用的排序算法实际上是自适应快速排序与堆排序子算法的结合，精神上与 `Sedgewick <https://algs4.cs.princeton.edu/23quicksort/>`_ 和 `pdqsort <https://github.com/orlp/pdqsort>`_ 一致，并针对 Linux 内核做了定制。为了在相当短的时间内对记录排序，``xfarray`` 利用了快速排序提供的二分子分区，但它也使用堆排序来对冲所选快速排序基准不佳时的性能崩溃。两种算法(一般而言)都是 O(n * lg(n))，但两个实现之间存在巨大的性能差距。)

The Linux kernel already contains a reasonably fast implementation of heapsort.
It only operates on regular C arrays, which limits the scope of its usefulness.
There are two key places where the xfarray uses it:
(Linux 内核已经包含一个相当快的堆排序实现。它只能操作常规 C 数组，这限制了其用途范围。xfarray 在两个关键地方使用它：)

* Sorting any record subset backed by a single xfile page.
  (对由单个 xfile 页面支撑的任何记录子集排序。)

* Loading a small number of xfarray records from potentially disparate parts
  of the xfarray into a memory buffer, and sorting the buffer.
  (将少量 xfarray 记录从 xfarray 中可能相距甚远的部分加载到内存缓冲中，并对该缓冲排序。)

In other words, ``xfarray`` uses heapsort to constrain the nested recursion of
quicksort, thereby mitigating quicksort's worst runtime behavior.
(换句话说，``xfarray`` 使用堆排序来约束快速排序的嵌套递归，从而缓解快速排序最坏情况下的运行时行为。)

Choosing a quicksort pivot is a tricky business.
A good pivot splits the set to sort in half, leading to the divide and conquer
behavior that is crucial to  O(n * lg(n)) performance.
A poor pivot barely splits the subset at all, leading to O(n\ :sup:`2`)
runtime.
The xfarray sort routine tries to avoid picking a bad pivot by sampling nine
records into a memory buffer and using the kernel heapsort to identify the
median of the nine.
(选择快速排序的基准是一件棘手的事。好的基准将待排序集合对半分开，带来对 O(n * lg(n)) 性能至关重要的分而治之行为。差的基准几乎不对子集进行分割，导致 O(n\ :sup:`2`) 的运行时间。xfarray 排序例程通过把九条记录采样到内存缓冲并使用内核堆排序找出这九条记录的中位数，来尽量避免选出差的基准。)

Most modern quicksort implementations employ Tukey's "ninther" to select a
pivot from a classic C array.
Typical ninther implementations pick three unique triads of records, sort each
of the triads, and then sort the middle value of each triad to determine the
ninther value.
As stated previously, however, xfile accesses are not entirely cheap.
It turned out to be much more performant to read the nine elements into a
memory buffer, run the kernel's in-memory heapsort on the buffer, and choose
the 4th element of that buffer as the pivot.
Tukey's ninthers are described in J. W. Tukey, `The ninther, a technique for
low-effort robust (resistant) location in large samples`, in *Contributions to
Survey Sampling and Applied Statistics*, edited by H. David, (Academic Press,
1978), pp. 251–257.
(大多数现代快速排序实现采用 Tukey 的"ninther"从经典 C 数组中选择基准。典型的 ninther 实现选取三个不同的记录三元组，分别对每个三元组排序，然后对每个三元组的中间值排序以确定 ninther 值。然而如前所述，xfile 访问并不完全便宜。结果证明，把九个元素读入内存缓冲、在缓冲上运行内核的内存中堆排序、然后选择该缓冲的第 4 个元素作为基准，性能要好得多。Tukey 的 ninther 在 J. W. Tukey 的 `The ninther, a technique for low-effort robust (resistant) location in large samples`(*Contributions to Survey Sampling and Applied Statistics*，H. David 编，Academic Press，1978 年，第 251–257 页)中有所描述。)

The partitioning of quicksort is fairly textbook -- rearrange the record
subset around the pivot, then set up the current and next stack frames to
sort with the larger and the smaller halves of the pivot, respectively.
This keeps the stack space requirements to log2(record count).
(快速排序的分区相当教科书式——围绕基准重新排列记录子集，然后设置当前和下一个栈帧，分别用基准的较大一半和较小一半进行排序。这将栈空间需求保持在 log2(记录数)。)

As a final performance optimization, the hi and lo scanning phase of quicksort
keeps examined xfile pages mapped in the kernel for as long as possible to
reduce map/unmap cycles.
Surprisingly, this reduces overall sort runtime by nearly half again after
accounting for the application of heapsort directly onto xfile pages.
(作为最后一项性能优化，快速排序的 hi 和 lo 扫描阶段会尽可能长时间地将已检查的 xfile 页面保持在内核中的映射状态，以减少映射/解除映射循环。令人惊讶的是，在计入直接应用于 xfile 页面的堆排序之后，这又将整体排序运行时间减少了近一半。)

.. _xfblob:

Blob Storage
(Blob 存储)
````````````

Extended attributes and directories add an additional requirement for staging
records: arbitrary byte sequences of finite length.
Each directory entry record needs to store entry name,
and each extended attribute needs to store both the attribute name and value.
The names, keys, and values can consume a large amount of memory, so the
``xfblob`` abstraction was created to simplify management of these blobs
atop an xfile.
(扩展属性和目录对暂存记录提出了一个额外要求：有限长度的任意字节序列。每条目录项记录都需要存储条目名称，每个扩展属性都需要同时存储属性名称和值。名称、键和值可能消耗大量内存，因此创建了 ``xfblob`` 抽象来简化在 xfile 之上管理这些 blob。)

Blob arrays provide ``xfblob_load`` and ``xfblob_store`` functions to retrieve
and persist objects.
The store function returns a magic cookie for every object that it persists.
Later, callers provide this cookie to the ``xblob_load`` to recall the object.
The ``xfblob_free`` function frees a specific blob, and the ``xfblob_truncate``
function frees them all because compaction is not needed.
(blob 数组提供 ``xfblob_load`` 和 ``xfblob_store`` 函数来取回和持久化对象。store 函数为它持久化的每个对象返回一个魔数 cookie。之后，调用者将此 cookie 提供给 ``xblob_load`` 以取回该对象。``xfblob_free`` 函数释放一个特定的 blob，``xfblob_truncate`` 函数释放全部 blob，因为不需要压缩整理。)

The details of repairing directories and extended attributes will be discussed
in a subsequent section about atomic file content exchanges.
However, it should be noted that these repair functions only use blob storage
to cache a small number of entries before adding them to a temporary ondisk
file, which is why compaction is not required.
(修复目录和扩展属性的细节将在关于原子文件内容交换的后续章节中讨论。不过，应当注意，这些修复函数只使用 blob 存储在将条目添加到临时磁盘文件之前缓存少量条目，这就是不需要压缩整理的原因。)

.. _xfbtree:

In-Memory B+Trees
(内存中的 B+ 树)
`````````````````

The chapter about :ref:`secondary metadata<secondary_metadata>` mentioned that
checking and repairing of secondary metadata commonly requires coordination
between a live metadata scan of the filesystem and writer threads that are
updating that metadata.
Keeping the scan data up to date requires the ability to propagate
metadata updates from the filesystem into the data being collected by the scan.
This *can* be done by appending concurrent updates into a separate log file and
applying them before writing the new metadata to disk, but this leads to
unbounded memory consumption if the rest of the system is very busy.
Another option is to skip the side-log and commit live updates from the
filesystem directly into the scan data, which trades more overhead for a lower
maximum memory requirement.
In both cases, the data structure holding the scan results must support indexed
access to perform well.
(关于 :ref:`二级元数据 <secondary_metadata>` 的章节提到，二级元数据的检查和修复通常需要在文件系统的实时元数据扫描与正在更新该元数据的写线程之间进行协调。保持扫描数据最新要求具备把元数据更新从文件系统传播到扫描正在收集的数据中的能力。这*可以*通过把并发更新追加到一个单独的日志文件中、并在将新元数据写入磁盘之前应用它们来完成，但如果系统其余部分非常繁忙，这会导致无上限的内存消耗。另一种选择是跳过边日志，把来自文件系统的实时更新直接提交到扫描数据中，这以更多开销换取更低的内存需求上限。在这两种情况下，保存扫描结果的数据结构都必须支持索引访问才能有良好表现。)

Given that indexed lookups of scan data is required for both strategies, online
fsck employs the second strategy of committing live updates directly into
scan data.
Because xfarrays are not indexed and do not enforce record ordering, they
are not suitable for this task.
Conveniently, however, XFS has a library to create and maintain ordered reverse
mapping records: the existing rmap btree code!
If only there was a means to create one in memory.
(鉴于两种策略都需要对扫描数据进行索引查找，在线 fsck 采用第二种策略，即把实时更新直接提交到扫描数据中。由于 xfarray 没有索引、也不强制记录顺序，它们不适合这项任务。然而，方便的是，XFS 有一个创建和维护有序反向映射记录的库：现有的 rmap btree 代码！要是能在内存中创建一个就好了。)

Recall that the :ref:`xfile <xfile>` abstraction represents memory pages as a
regular file, which means that the kernel can create byte or block addressable
virtual address spaces at will.
The XFS buffer cache specializes in abstracting IO to block-oriented  address
spaces, which means that adaptation of the buffer cache to interface with
xfiles enables reuse of the entire btree library.
Btrees built atop an xfile are collectively known as ``xfbtrees``.
The next few sections describe how they actually work.
(回想一下，:ref:`xfile <xfile>` 抽象将内存页面表示为常规文件，这意味着内核可以随意创建可按字节或块寻址的虚拟地址空间。XFS 缓冲缓存专门负责对面向块的地址空间的 IO 进行抽象，这意味着改造缓冲缓存以与 xfile 接口，就能重用整个 btree 库。构建在 xfile 之上的 btree 统称为 ``xfbtrees``。接下来几节描述它们实际上如何工作。)

Using xfiles as a Buffer Cache Target
(将 xfile 用作缓冲缓存目标)
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

Two modifications are necessary to support xfiles as a buffer cache target.
The first is to make it possible for the ``struct xfs_buftarg`` structure to
host the ``struct xfs_buf`` rhashtable, because normally those are held by a
per-AG structure.
The second change is to modify the buffer ``ioapply`` function to "read" cached
pages from the xfile and "write" cached pages back to the xfile.
Multiple access to individual buffers is controlled by the ``xfs_buf`` lock,
since the xfile does not provide any locking on its own.
With this adaptation in place, users of the xfile-backed buffer cache use
exactly the same APIs as users of the disk-backed buffer cache.
The separation between xfile and buffer cache implies higher memory usage since
they do not share pages, but this property could some day enable transactional
updates to an in-memory btree.
Today, however, it simply eliminates the need for new code.
(为支持将 xfile 用作缓冲缓存目标，需要两处修改。第一处是使 ``struct xfs_buftarg`` 结构能够承载 ``struct xfs_buf`` rhashtable，因为这些通常由每 AG 结构持有。第二处修改是改造缓冲 ``ioapply`` 函数，使其从 xfile "读取"缓存的页面并将缓存的页面"写回"xfile。对单个缓冲的多次访问由 ``xfs_buf`` 锁控制，因为 xfile 自身不提供任何锁定。有了这一改造，xfile 支撑的缓冲缓存的使用者使用的 API 与磁盘支撑的缓冲缓存的使用者完全相同。xfile 与缓冲缓存的分离意味着更高的内存使用，因为它们不共享页面，但这一特性有朝一日可能启用对内存中 btree 的事务性更新。不过今天，它只是免除了编写新代码的需要。)

Space Management with an xfbtree
(用 xfbtree 进行空间管理)
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

Space management for an xfile is very simple -- each btree block is one memory
page in size.
These blocks use the same header format as an on-disk btree, but the in-memory
block verifiers ignore the checksums, assuming that xfile memory is no more
corruption-prone than regular DRAM.
Reusing existing code here is more important than absolute memory efficiency.
(xfile 的空间管理非常简单——每个 btree 块大小为一个内存页面。这些块使用与磁盘上 btree 相同的头格式，但内存中的块校验器忽略校验和，假定 xfile 内存并不比常规 DRAM 更容易损坏。在这里重用现有代码比绝对的内存效率更重要。)

The very first block of an xfile backing an xfbtree contains a header block.
The header describes the owner, height, and the block number of the root
xfbtree block.
(支撑 xfbtree 的 xfile 的最前面一个块包含一个头块。头描述属主、高度以及根 xfbtree 块的块号。)

To allocate a btree block, use ``xfile_seek_data`` to find a gap in the file.
If there are no gaps, create one by extending the length of the xfile.
Preallocate space for the block with ``xfile_prealloc``, and hand back the
location.
To free an xfbtree block, use ``xfile_discard`` (which internally uses
``FALLOC_FL_PUNCH_HOLE``) to remove the memory page from the xfile.
(要分配一个 btree 块，使用 ``xfile_seek_data`` 在文件中寻找一个间隙。如果没有间隙，则通过延长 xfile 的长度来创建一个。用 ``xfile_prealloc`` 为该块预分配空间，并交回该位置。要释放一个 xfbtree 块，使用 ``xfile_discard``(内部使用 ``FALLOC_FL_PUNCH_HOLE``)从 xfile 中移除该内存页面。)

Populating an xfbtree
(填充一个 xfbtree)
^^^^^^^^^^^^^^^^^^^^^

An online fsck function that wants to create an xfbtree should proceed as
follows:
(想要创建 xfbtree 的在线 fsck 函数应按如下步骤进行：)

1. Call ``xfile_create`` to create an xfile.
   (调用 ``xfile_create`` 创建一个 xfile。)

2. Call ``xfs_alloc_memory_buftarg`` to create a buffer cache target structure
   pointing to the xfile.
   (调用 ``xfs_alloc_memory_buftarg`` 创建一个指向该 xfile 的缓冲缓存目标结构。)

3. Pass the buffer cache target, buffer ops, and other information to
   ``xfbtree_init`` to initialize the passed in ``struct xfbtree`` and write an
   initial root block to the xfile.
   Each btree type should define a wrapper that passes necessary arguments to
   the creation function.
   For example, rmap btrees define ``xfs_rmapbt_mem_create`` to take care of
   all the necessary details for callers.
   (将缓冲缓存目标、缓冲操作和其他信息传递给 ``xfbtree_init``，以初始化传入的 ``struct xfbtree`` 并向 xfile 写入一个初始根块。每种 btree 类型都应定义一个包装函数，向创建函数传递必要的参数。例如，rmap btree 定义了 ``xfs_rmapbt_mem_create`` 来为调用者处理所有必要的细节。)

4. Pass the xfbtree object to the btree cursor creation function for the
   btree type.
   Following the example above, ``xfs_rmapbt_mem_cursor`` takes care of this
   for callers.
   (将 xfbtree 对象传递给该 btree 类型的 btree 游标创建函数。按照上面的例子，``xfs_rmapbt_mem_cursor`` 为调用者处理此事。)

5. Pass the btree cursor to the regular btree functions to make queries against
   and to update the in-memory btree.
   For example, a btree cursor for an rmap xfbtree can be passed to the
   ``xfs_rmap_*`` functions just like any other btree cursor.
   See the :ref:`next section<xfbtree_commit>` for information on dealing with
   xfbtree updates that are logged to a transaction.
   (将 btree 游标传递给常规 btree 函数，以对内存中的 btree 进行查询和更新。例如，rmap xfbtree 的 btree 游标可以像任何其他 btree 游标一样传递给 ``xfs_rmap_*`` 函数。处理被记录到事务中的 xfbtree 更新的信息参见 :ref:`下一节 <xfbtree_commit>`。)

6. When finished, delete the btree cursor, destroy the xfbtree object, free the
   buffer target, and the destroy the xfile to release all resources.
   (完成后，删除 btree 游标、销毁 xfbtree 对象、释放缓冲目标，并销毁 xfile 以释放所有资源。)

.. _xfbtree_commit:

Committing Logged xfbtree Buffers
(提交已记录日志的 xfbtree 缓冲)
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

Although it is a clever hack to reuse the rmap btree code to handle the staging
structure, the ephemeral nature of the in-memory btree block storage presents
some challenges of its own.
The XFS transaction manager must not commit buffer log items for buffers backed
by an xfile because the log format does not understand updates for devices
other than the data device.
An ephemeral xfbtree probably will not exist by the time the AIL checkpoints
log transactions back into the filesystem, and certainly won't exist during
log recovery.
For these reasons, any code updating an xfbtree in transaction context must
remove the buffer log items from the transaction and write the updates into the
backing xfile before committing or cancelling the transaction.
(虽然重用 rmap btree 代码来处理暂存结构是一个聪明的技巧，但内存中 btree 块存储的短暂性质也带来了一些挑战。XFS 事务管理器不得为 xfile 支撑的缓冲提交缓冲日志项，因为日志格式不理解除数据设备以外设备的更新。一个短暂的 xfbtree 到 AIL 将日志事务检查点写回文件系统时可能已不存在，在日志恢复期间更是肯定不存在。出于这些原因，任何在事务上下文中更新 xfbtree 的代码必须在提交或取消事务之前，从事务中移除缓冲日志项并将更新写入支撑的 xfile。)

The ``xfbtree_trans_commit`` and ``xfbtree_trans_cancel`` functions implement
this functionality as follows:
(``xfbtree_trans_commit`` 和 ``xfbtree_trans_cancel`` 函数按如下方式实现这一功能：)

1. Find each buffer log item whose buffer targets the xfile.
   (找出每个缓冲指向 xfile 的缓冲日志项。)

2. Record the dirty/ordered status of the log item.
   (记录该日志项的脏/有序状态。)

3. Detach the log item from the buffer.
   (将该日志项从缓冲上分离。)

4. Queue the buffer to a special delwri list.
   (将该缓冲排入一个特殊的延迟写(delwri)列表。)

5. Clear the transaction dirty flag if the only dirty log items were the ones
   that were detached in step 3.
   (如果唯一的脏日志项就是步骤 3 中分离的那些，则清除事务的脏标志。)

6. Submit the delwri list to commit the changes to the xfile, if the updates
   are being committed.
   (如果更新正在被提交，则提交该延迟写列表，将更改提交到 xfile。)

After removing xfile logged buffers from the transaction in this manner, the
transaction can be committed or cancelled.
(以这种方式从事务中移除 xfile 日志缓冲之后，该事务就可以被提交或取消了。)

Bulk Loading of Ondisk B+Trees
(磁盘上 B+ 树的批量加载)
------------------------------

As mentioned previously, early iterations of online repair built new btree
structures by creating a new btree and adding observations individually.
Loading a btree one record at a time had a slight advantage of not requiring
the incore records to be sorted prior to commit, but was very slow and leaked
blocks if the system went down during a repair.
Loading records one at a time also meant that repair could not control the
loading factor of the blocks in the new btree.
(如前所述，在线修复的早期迭代通过创建新 btree 并逐条添加观察结果来构建新的 btree 结构。一次加载一条记录有一个小优势，即不要求内存中记录在提交前排序，但非常慢，并且如果在修复期间系统宕机会泄漏块。一次加载一条记录还意味着修复无法控制新 btree 中块的装载因子。)

Fortunately, the venerable ``xfs_repair`` tool had a more efficient means for
rebuilding a btree index from a collection of records -- bulk btree loading.
This was implemented rather inefficiently code-wise, since ``xfs_repair``
had separate copy-pasted implementations for each btree type.
(幸运的是，久经沙场的 ``xfs_repair`` 工具有一种从记录集合重建 btree 索引的更高效手段——btree 批量加载。这在代码层面的实现相当低效，因为 ``xfs_repair`` 为每种 btree 类型都有单独的复制粘贴实现。)

To prepare for online fsck, each of the four bulk loaders were studied, notes
were taken, and the four were refactored into a single generic btree bulk
loading mechanism.
Those notes in turn have been refreshed and are presented below.
(为了给在线 fsck 做准备，对四个批量加载器逐一进行了研究、做了笔记，并将它们重构为单一的通用 btree 批量加载机制。这些笔记随后经过更新，呈现如下。)

Geometry Computation
(几何计算)
````````````````````

The zeroth step of bulk loading is to assemble the entire record set that will
be stored in the new btree, and sort the records.
Next, call ``xfs_btree_bload_compute_geometry`` to compute the shape of the
btree from the record set, the type of btree, and any load factor preferences.
This information is required for resource reservation.
(批量加载的第零步是组装将要存储在新 btree 中的完整记录集，并对记录排序。接下来，调用 ``xfs_btree_bload_compute_geometry``，根据记录集、btree 类型以及任何装载因子偏好来计算 btree 的形状。这一信息是资源预留所必需的。)

First, the geometry computation computes the minimum and maximum records that
will fit in a leaf block from the size of a btree block and the size of the
block header.
Roughly speaking, the maximum number of records is::
(首先，几何计算根据 btree 块的大小和块头的大小，计算一个叶子块能容纳的最小和最大记录数。粗略地说，最大记录数是：)

        maxrecs = (block_size - header_size) / record_size

The XFS design specifies that btree blocks should be merged when possible,
which means the minimum number of records is half of maxrecs::
(XFS 设计规定 btree 块应在可能时合并，这意味着最小记录数是 maxrecs 的一半：)

        minrecs = maxrecs / 2

The next variable to determine is the desired loading factor.
This must be at least minrecs and no more than maxrecs.
Choosing minrecs is undesirable because it wastes half the block.
Choosing maxrecs is also undesirable because adding a single record to each
newly rebuilt leaf block will cause a tree split, which causes a noticeable
drop in performance immediately afterwards.
The default loading factor was chosen to be 75% of maxrecs, which provides a
reasonably compact structure without any immediate split penalties::
(下一个要确定的变量是期望的装载因子。它必须至少为 minrecs 且不超过 maxrecs。选择 minrecs 不可取，因为它浪费了块的一半。选择 maxrecs 也不可取，因为向每个新重建的叶子块添加一条记录就会导致树分裂，紧接着会造成明显的性能下降。默认装载因子被选为 maxrecs 的 75%，这提供了一个相当紧凑的结构且没有任何即时的分裂惩罚：)

        default_load_factor = (maxrecs + minrecs) / 2

If space is tight, the loading factor will be set to maxrecs to try to avoid
running out of space::
(如果空间紧张，装载因子将被设为 maxrecs，以尽量避免耗尽空间：)

        leaf_load_factor = enough space ? default_load_factor : maxrecs

Load factor is computed for btree node blocks using the combined size of the
btree key and pointer as the record size::
(btree 节点块的装载因子以 btree 键和指针的组合大小作为记录大小来计算：)

        maxrecs = (block_size - header_size) / (key_size + ptr_size)
        minrecs = maxrecs / 2
        node_load_factor = enough space ? default_load_factor : maxrecs

Once that's done, the number of leaf blocks required to store the record set
can be computed as::
(完成后，存储记录集所需的叶子块数可以按如下方式计算：)

        leaf_blocks = ceil(record_count / leaf_load_factor)

The number of node blocks needed to point to the next level down in the tree
is computed as::
(树中指向下一层所需的节点块数按如下方式计算：)

        n_blocks = (n == 0 ? leaf_blocks : node_blocks[n])
        node_blocks[n + 1] = ceil(n_blocks / node_load_factor)

The entire computation is performed recursively until the current level only
needs one block.
The resulting geometry is as follows:
(整个计算递归执行，直到当前层只需要一个块。最终得到的几何形状如下：)

- For AG-rooted btrees, this level is the root level, so the height of the new
  tree is ``level + 1`` and the space needed is the summation of the number of
  blocks on each level.
  (对于以 AG 为根的 btree，这一层就是根层，因此新树的高度是 ``level + 1``，所需空间是各层块数的总和。)

- For inode-rooted btrees where the records in the top level do not fit in the
  inode fork area, the height is ``level + 2``, the space needed is the
  summation of the number of blocks on each level, and the inode fork points to
  the root block.
  (对于以 inode 为根、且顶层记录容纳不进 inode fork 区域的 btree，高度是 ``level + 2``，所需空间是各层块数的总和，inode fork 指向根块。)

- For inode-rooted btrees where the records in the top level can be stored in
  the inode fork area, then the root block can be stored in the inode, the
  height is ``level + 1``, and the space needed is one less than the summation
  of the number of blocks on each level.
  This only becomes relevant when non-bmap btrees gain the ability to root in
  an inode, which is a future patchset and only included here for completeness.
  (对于以 inode 为根、且顶层记录可以存储在 inode fork 区域中的 btree，根块可以存储在 inode 中，高度是 ``level + 1``，所需空间比各层块数的总和少一。这只有在非 bmap btree 获得以 inode 为根的能力时才有意义，那是未来的一个补丁集，此处仅为完整性而包含。)

.. _newbt:

Reserving New B+Tree Blocks
(预留新 B+ 树块)
```````````````````````````

Once repair knows the number of blocks needed for the new btree, it allocates
those blocks using the free space information.
Each reserved extent is tracked separately by the btree builder state data.
To improve crash resilience, the reservation code also logs an Extent Freeing
Intent (EFI) item in the same transaction as each space allocation and attaches
its in-memory ``struct xfs_extent_free_item`` object to the space reservation.
If the system goes down, log recovery will use the unfinished EFIs to free the
unused space, the free space, leaving the filesystem unchanged.
(一旦修复知道新 btree 需要多少块，它就使用空闲空间信息分配这些块。每个预留的区段都由 btree 构建器状态数据单独跟踪。为了提高崩溃恢复能力，预留代码还在与每次空间分配相同的事务中记录一个区段释放意图(EFI)项，并将其内存中的 ``struct xfs_extent_free_item`` 对象附加到空间预留上。如果系统宕机，日志恢复将使用未完成的 EFI 来释放未使用的空间，使文件系统保持不变。)

Each time the btree builder claims a block for the btree from a reserved
extent, it updates the in-memory reservation to reflect the claimed space.
Block reservation tries to allocate as much contiguous space as possible to
reduce the number of EFIs in play.
(每次 btree 构建器从预留区段中为 btree 认领一个块时，它都会更新内存中的预留以反映所认领的空间。块预留尝试分配尽可能多的连续空间，以减少在场的 EFI 数量。)

While repair is writing these new btree blocks, the EFIs created for the space
reservations pin the tail of the ondisk log.
It's possible that other parts of the system will remain busy and push the head
of the log towards the pinned tail.
To avoid livelocking the filesystem, the EFIs must not pin the tail of the log
for too long.
To alleviate this problem, the dynamic relogging capability of the deferred ops
mechanism is reused here to commit a transaction at the log head containing an
EFD for the old EFI and new EFI at the head.
This enables the log to release the old EFI to keep the log moving forwards.
(当修复正在写这些新 btree 块时，为空间预留创建的 EFI 会钉住磁盘日志的尾部。系统的其他部分可能保持繁忙，把日志头部推向被钉住的尾部。为避免文件系统活锁，EFI 不得把日志尾部钉住太久。为缓解这一问题，这里重用了延迟操作机制的重日志(dynamic relogging)能力，在日志头部提交一个事务，其中包含旧 EFI 的 EFD 和位于头部的新 EFI。这使日志能够释放旧 EFI，让日志继续向前推进。)

EFIs have a role to play during the commit and reaping phases; please see the
next section and the section about :ref:`reaping<reaping>` for more details.
(EFI 在提交和回收阶段都有作用；更多细节请参阅下一节以及关于 :ref:`回收 <reaping>` 的章节。)

Writing the New Tree
(写入新树)
````````````````````

This part is pretty simple -- the btree builder (``xfs_btree_bulkload``) claims
a block from the reserved list, writes the new btree block header, fills the
rest of the block with records, and adds the new leaf block to a list of
written blocks::
(这部分相当简单——btree 构建器(``xfs_btree_bulkload``)从预留列表中认领一个块，写入新的 btree 块头，用记录填充块的其余部分，并将新的叶子块添加到已写入块的列表中：)

  ┌────┐
  │leaf│
  │RRR │
  └────┘

Sibling pointers are set every time a new block is added to the level::
(每当向该层添加新块时都会设置兄弟指针：)

  ┌────┐ ┌────┐ ┌────┐ ┌────┐
  │leaf│→│leaf│→│leaf│→│leaf│
  │RRR │←│RRR │←│RRR │←│RRR │
  └────┘ └────┘ └────┘ └────┘

When it finishes writing the record leaf blocks, it moves on to the node
blocks
To fill a node block, it walks each block in the next level down in the tree
to compute the relevant keys and write them into the parent node::
(写完记录叶子块后，它转向节点块。要填充一个节点块，它会遍历树中下一层的每个块，计算相关的键并写入父节点：)

      ┌────┐       ┌────┐
      │node│──────→│node│
      │PP  │←──────│PP  │
      └────┘       └────┘
      ↙   ↘         ↙   ↘
  ┌────┐ ┌────┐ ┌────┐ ┌────┐
  │leaf│→│leaf│→│leaf│→│leaf│
  │RRR │←│RRR │←│RRR │←│RRR │
  └────┘ └────┘ └────┘ └────┘

When it reaches the root level, it is ready to commit the new btree!::
(当到达根层时，它就准备好提交新的 btree 了！：)

          ┌─────────┐
          │  root   │
          │   PP    │
          └─────────┘
          ↙         ↘
      ┌────┐       ┌────┐
      │node│──────→│node│
      │PP  │←──────│PP  │
      └────┘       └────┘
      ↙   ↘         ↙   ↘
  ┌────┐ ┌────┐ ┌────┐ ┌────┐
  │leaf│→│leaf│→│leaf│→│leaf│
  │RRR │←│RRR │←│RRR │←│RRR │
  └────┘ └────┘ └────┘ └────┘

The first step to commit the new btree is to persist the btree blocks to disk
synchronously.
This is a little complicated because a new btree block could have been freed
in the recent past, so the builder must use ``xfs_buf_delwri_queue_here`` to
remove the (stale) buffer from the AIL list before it can write the new blocks
to disk.
Blocks are queued for IO using a delwri list and written in one large batch
with ``xfs_buf_delwri_submit``.
(提交新 btree 的第一步是将 btree 块同步地持久化到磁盘。这有点复杂，因为一个新 btree 块可能不久前刚被释放过，所以构建器必须先使用 ``xfs_buf_delwri_queue_here`` 从 AIL 列表中移除(过期的)缓冲，然后才能将新块写入磁盘。块使用延迟写列表排队进行 IO，并通过 ``xfs_buf_delwri_submit`` 一大批次写入。)

Once the new blocks have been persisted to disk, control returns to the
individual repair function that called the bulk loader.
The repair function must log the location of the new root in a transaction,
clean up the space reservations that were made for the new btree, and reap the
old metadata blocks:
(一旦新块被持久化到磁盘，控制权返回到调用批量加载器的各个修复函数。修复函数必须在一个事务中记录新根的位置、清理为新 btree 所做的空间预留，并回收旧的元数据块：)

1. Commit the location of the new btree root.
   (提交新 btree 根的位置。)

2. For each incore reservation:
   (对于每个内存中的预留：)

   a. Log Extent Freeing Done (EFD) items for all the space that was consumed
      by the btree builder.  The new EFDs must point to the EFIs attached to
      the reservation to prevent log recovery from freeing the new blocks.
      (为 btree 构建器消耗的所有空间记录区段释放完成(EFD)项。新的 EFD 必须指向附加到预留的 EFI，以防止日志恢复释放新块。)

   b. For unclaimed portions of incore reservations, create a regular deferred
      extent free work item to be free the unused space later in the
      transaction chain.
      (对于内存中预留中未被认领的部分，创建一个常规的延迟区段释放工作项，以便稍后在事务链中释放未使用的空间。)

   c. The EFDs and EFIs logged in steps 2a and 2b must not overrun the
      reservation of the committing transaction.
      If the btree loading code suspects this might be about to happen, it must
      call ``xrep_defer_finish`` to clear out the deferred work and obtain a
      fresh transaction.
      (在步骤 2a 和 2b 中记录的 EFD 和 EFI 不得超出提交事务的预留。如果 btree 加载代码怀疑这可能即将发生，它必须调用 ``xrep_defer_finish`` 来清空延迟工作并获取一个新事务。)

3. Clear out the deferred work a second time to finish the commit and clean
   the repair transaction.
   (第二次清空延迟工作，以完成提交并清理修复事务。)

The transaction rolling in steps 2c and 3 represent a weakness in the repair
algorithm, because a log flush and a crash before the end of the reap step can
result in space leaking.
Online repair functions minimize the chances of this occurring by using very
large transactions, which each can accommodate many thousands of block freeing
instructions.
Repair moves on to reaping the old blocks, which will be presented in a
subsequent :ref:`section<reaping>` after a few case studies of bulk loading.
(步骤 2c 和 3 中的事务滚动代表着修复算法的一个弱点，因为在回收步骤结束之前的日志刷新和崩溃可能导致空间泄漏。在线修复函数通过使用非常大的事务来最小化这种情况发生的几率，每个这样的事务可以容纳数千条块释放指令。修复接着进行旧块的回收，这将在几个批量加载案例研究之后的后续 :ref:`章节 <reaping>` 中呈现。)

Case Study: Rebuilding the Inode Index
(案例研究：重建 inode 索引)
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

The high level process to rebuild the inode index btree is:
(重建 inode 索引 btree 的高层流程是：)

1. Walk the reverse mapping records to generate ``struct xfs_inobt_rec``
   records from the inode chunk information and a bitmap of the old inode btree
   blocks.
   (遍历反向映射记录，根据 inode 块信息以及旧 inode btree 块的位图生成 ``struct xfs_inobt_rec`` 记录。)

2. Append the records to an xfarray in inode order.
   (按 inode 顺序将这些记录追加到 xfarray。)

3. Use the ``xfs_btree_bload_compute_geometry`` function to compute the number
   of blocks needed for the inode btree.
   If the free space inode btree is enabled, call it again to estimate the
   geometry of the finobt.
   (使用 ``xfs_btree_bload_compute_geometry`` 函数计算 inode btree 所需的块数。如果空闲空间 inode btree 已启用，再次调用它以估计 finobt 的几何形状。)

4. Allocate the number of blocks computed in the previous step.
   (分配上一步计算出的块数。)

5. Use ``xfs_btree_bload`` to write the xfarray records to btree blocks and
   generate the internal node blocks.
   If the free space inode btree is enabled, call it again to load the finobt.
   (使用 ``xfs_btree_bload`` 将 xfarray 记录写入 btree 块并生成内部节点块。如果空闲空间 inode btree 已启用，再次调用它来加载 finobt。)

6. Commit the location of the new btree root block(s) to the AGI.
   (将新 btree 根块的位置提交到 AGI。)

7. Reap the old btree blocks using the bitmap created in step 1.
   (使用步骤 1 创建的位图回收旧的 btree 块。)

Details are as follows.
(细节如下。)

The inode btree maps inumbers to the ondisk location of the associated
inode records, which means that the inode btrees can be rebuilt from the
reverse mapping information.
Reverse mapping records with an owner of ``XFS_RMAP_OWN_INOBT`` marks the
location of the old inode btree blocks.
Each reverse mapping record with an owner of ``XFS_RMAP_OWN_INODES`` marks the
location of at least one inode cluster buffer.
A cluster is the smallest number of ondisk inodes that can be allocated or
freed in a single transaction; it is never smaller than 1 fs block or 4 inodes.
(inode btree 将 inumber 映射到相关联 inode 记录的磁盘位置，这意味着可以从反向映射信息重建 inode btree。属主为 ``XFS_RMAP_OWN_INOBT`` 的反向映射记录标记旧 inode btree 块的位置。每条属主为 ``XFS_RMAP_OWN_INODES`` 的反向映射记录标记至少一个 inode 簇缓冲的位置。簇是单个事务中可以分配或释放的最少磁盘 inode 数；它绝不小于 1 个文件系统块或 4 个 inode。)

For the space represented by each inode cluster, ensure that there are no
records in the free space btrees nor any records in the reference count btree.
If there are, the space metadata inconsistencies are reason enough to abort the
operation.
Otherwise, read each cluster buffer to check that its contents appear to be
ondisk inodes and to decide if the file is allocated
(``xfs_dinode.i_mode != 0``) or free (``xfs_dinode.i_mode == 0``).
Accumulate the results of successive inode cluster buffer reads until there is
enough information to fill a single inode chunk record, which is 64 consecutive
numbers in the inumber keyspace.
If the chunk is sparse, the chunk record may include holes.
(对于每个 inode 簇所代表的空间，确保空闲空间 btree 中没有任何记录、引用计数 btree 中也没有任何记录。如果有，空间元数据的不一致就足以中止该操作。否则，读取每个簇缓冲，检查其内容看起来是否为磁盘上的 inode，并判断文件是已分配(``xfs_dinode.i_mode != 0``)还是空闲(``xfs_dinode.i_mode == 0``)。累积连续的 inode 簇缓冲读取结果，直到有足够的信息填满一条 inode 块记录——即 inumber 键空间中 64 个连续的编号。如果块是稀疏的，块记录可能包含空洞。)

Once the repair function accumulates one chunk's worth of data, it calls
``xfarray_append`` to add the inode btree record to the xfarray.
This xfarray is walked twice during the btree creation step -- once to populate
the inode btree with all inode chunk records, and a second time to populate the
free inode btree with records for chunks that have free non-sparse inodes.
The number of records for the inode btree is the number of xfarray records,
but the record count for the free inode btree has to be computed as inode chunk
records are stored in the xfarray.
(一旦修复函数累积了一个块的数据量，它就调用 ``xfarray_append`` 将 inode btree 记录添加到 xfarray。这个 xfarray 在 btree 创建步骤中被遍历两次——第一次用所有 inode 块记录填充 inode btree，第二次用拥有空闲非稀疏 inode 的块的记录填充空闲 inode btree。inode btree 的记录数就是 xfarray 记录数，但空闲 inode btree 的记录数必须在 inode 块记录存入 xfarray 时计算。)

Case Study: Rebuilding the Space Reference Counts
(案例研究：重建空间引用计数)
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

Reverse mapping records are used to rebuild the reference count information.
Reference counts are required for correct operation of copy on write for shared
file data.
Imagine the reverse mapping entries as rectangles representing extents of
physical blocks, and that the rectangles can be laid down to allow them to
overlap each other.
From the diagram below, it is apparent that a reference count record must start
or end wherever the height of the stack changes.
In other words, the record emission stimulus is level-triggered::
(反向映射记录被用来重建引用计数信息。共享文件数据的写时复制要正确运行，引用计数是必需的。把反向映射条目想象成代表物理块区段的矩形，并且这些矩形可以平放以允许彼此重叠。从下面的图可以明显看出，引用计数记录必须在栈高度发生变化之处开始或结束。换句话说，记录发出的触发是电平触发的：)

                        █    ███
              ██      █████ ████   ███        ██████
        ██   ████     ███████████ ████     █████████
        ████████████████████████████████ ███████████
        ^ ^  ^^ ^^    ^ ^^ ^^^  ^^^^  ^ ^^ ^  ^     ^
        2 1  23 21    3 43 234  2123  1 01 2  3     0

The ondisk reference count btree does not store the refcount == 0 cases because
the free space btree already records which blocks are free.
Extents being used to stage copy-on-write operations should be the only records
with refcount == 1.
Single-owner file blocks aren't recorded in either the free space or the
reference count btrees.
(磁盘上的引用计数 btree 不存储 refcount == 0 的情况，因为空闲空间 btree 已经记录了哪些块是空闲的。用于暂存写时复制操作的区段应当是唯一 refcount == 1 的记录。单属主文件块在空闲空间 btree 和引用计数 btree 中都不记录。)

The high level process to rebuild the reference count btree is:
(重建引用计数 btree 的高层流程是：)

1. Walk the reverse mapping records to generate ``struct xfs_refcount_irec``
   records for any space having more than one reverse mapping and add them to
   the xfarray.
   Any records owned by ``XFS_RMAP_OWN_COW`` are also added to the xfarray
   because these are extents allocated to stage a copy on write operation and
   are tracked in the refcount btree.
   (遍历反向映射记录，为拥有多于一条反向映射的任何空间生成 ``struct xfs_refcount_irec`` 记录，并将它们添加到 xfarray。属主为 ``XFS_RMAP_OWN_COW`` 的记录也被添加到 xfarray，因为这些是分配用于暂存写时复制操作的区段，并在引用计数 btree 中跟踪。)

   Use any records owned by ``XFS_RMAP_OWN_REFC`` to create a bitmap of old
   refcount btree blocks.
   (使用属主为 ``XFS_RMAP_OWN_REFC`` 的记录创建旧引用计数 btree 块的位图。)

2. Sort the records in physical extent order, putting the CoW staging extents
   at the end of the xfarray.
   This matches the sorting order of records in the refcount btree.
   (按物理区段顺序对记录排序，将 CoW 暂存区段放在 xfarray 的末尾。这与引用计数 btree 中记录的排序顺序一致。)

3. Use the ``xfs_btree_bload_compute_geometry`` function to compute the number
   of blocks needed for the new tree.
   (使用 ``xfs_btree_bload_compute_geometry`` 函数计算新树所需的块数。)

4. Allocate the number of blocks computed in the previous step.
   (分配上一步计算出的块数。)

5. Use ``xfs_btree_bload`` to write the xfarray records to btree blocks and
   generate the internal node blocks.
   (使用 ``xfs_btree_bload`` 将 xfarray 记录写入 btree 块并生成内部节点块。)

6. Commit the location of new btree root block to the AGF.
   (将新 btree 根块的位置提交到 AGF。)

7. Reap the old btree blocks using the bitmap created in step 1.
   (使用步骤 1 创建的位图回收旧的 btree 块。)

Details are as follows; the same algorithm is used by ``xfs_repair`` to
generate refcount information from reverse mapping records.
(细节如下；``xfs_repair`` 使用相同的算法从反向映射记录生成引用计数信息。)

- Until the reverse mapping btree runs out of records:
  (在反向映射 btree 的记录用完之前：)

  - Retrieve the next record from the btree and put it in a bag.
    (从 btree 中取回下一条记录并放入袋子。)

  - Collect all records with the same starting block from the btree and put
    them in the bag.
    (从 btree 中收集所有具有相同起始块的记录并放入袋子。)

  - While the bag isn't empty:
    (在袋子不空的情况下：)

    - Among the mappings in the bag, compute the lowest block number where the
      reference count changes.
      This position will be either the starting block number of the next
      unprocessed reverse mapping or the next block after the shortest mapping
      in the bag.
      (在袋子中的映射之间，计算引用计数发生变化的最小块号。该位置要么是下一个未处理反向映射的起始块号，要么是袋子中最短映射之后的下一个块。)

    - Remove all mappings from the bag that end at this position.
      (从袋子中移除所有在该位置结束的映射。)

    - Collect all reverse mappings that start at this position from the btree
      and put them in the bag.
      (从 btree 中收集所有在该位置起始的反向映射并放入袋子。)

    - If the size of the bag changed and is greater than one, create a new
      refcount record associating the block number range that we just walked to
      the size of the bag.
      (如果袋子的大小发生了变化且大于一，则创建一条新的引用计数记录，将刚刚遍历的块号范围与袋子的大小关联起来。)

The bag-like structure in this case is a type 2 xfarray as discussed in the
:ref:`xfarray access patterns<xfarray_access_patterns>` section.
Reverse mappings are added to the bag using ``xfarray_store_anywhere`` and
removed via ``xfarray_unset``.
Bag members are examined through ``xfarray_iter`` loops.
(这种情况下的类袋子结构是 :ref:`xfarray 访问模式 <xfarray_access_patterns>` 一节中讨论的类型 2 xfarray。反向映射通过 ``xfarray_store_anywhere`` 添加到袋子，通过 ``xfarray_unset`` 移除。袋子成员通过 ``xfarray_iter`` 循环进行检查。)

Case Study: Rebuilding File Fork Mapping Indices
(案例研究：重建文件 fork 映射索引)
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

The high level process to rebuild a data/attr fork mapping btree is:
(重建数据/属性 fork 映射 btree 的高层流程是：)

1. Walk the reverse mapping records to generate ``struct xfs_bmbt_rec``
   records from the reverse mapping records for that inode and fork.
   Append these records to an xfarray.
   Compute the bitmap of the old bmap btree blocks from the ``BMBT_BLOCK``
   records.
   (遍历该 inode 和 fork 的反向映射记录以生成 ``struct xfs_bmbt_rec`` 记录。将这些记录追加到 xfarray。根据 ``BMBT_BLOCK`` 记录计算旧 bmap btree 块的位图。)

2. Use the ``xfs_btree_bload_compute_geometry`` function to compute the number
   of blocks needed for the new tree.
   (使用 ``xfs_btree_bload_compute_geometry`` 函数计算新树所需的块数。)

3. Sort the records in file offset order.
   (按文件偏移顺序对记录排序。)

4. If the extent records would fit in the inode fork immediate area, commit the
   records to that immediate area and skip to step 8.
   (如果区段记录能放进 inode fork 的内联区域，则将这些记录提交到该内联区域并跳到步骤 8。)

5. Allocate the number of blocks computed in the previous step.
   (分配上一步计算出的块数。)

6. Use ``xfs_btree_bload`` to write the xfarray records to btree blocks and
   generate the internal node blocks.
   (使用 ``xfs_btree_bload`` 将 xfarray 记录写入 btree 块并生成内部节点块。)

7. Commit the new btree root block to the inode fork immediate area.
   (将新 btree 根块提交到 inode fork 内联区域。)

8. Reap the old btree blocks using the bitmap created in step 1.
   (使用步骤 1 创建的位图回收旧的 btree 块。)

There are some complications here:
First, it's possible to move the fork offset to adjust the sizes of the
immediate areas if the data and attr forks are not both in BMBT format.
Second, if there are sufficiently few fork mappings, it may be possible to use
EXTENTS format instead of BMBT, which may require a conversion.
Third, the incore extent map must be reloaded carefully to avoid disturbing
any delayed allocation extents.
(这里有一些复杂情况：第一，如果数据 fork 和属性 fork 不都处于 BMBT 格式，可以移动 fork 偏移来调整内联区域的大小。第二，如果 fork 映射足够少，可能可以使用 EXTENTS 格式代替 BMBT，这可能需要一次转换。第三，必须小心地重新加载内存中区段映射，以避免干扰任何延迟分配的区段。)

.. _reaping:

Reaping Old Metadata Blocks
(回收旧元数据块)
---------------------------

Whenever online fsck builds a new data structure to replace one that is
suspect, there is a question of how to find and dispose of the blocks that
belonged to the old structure.
The laziest method of course is not to deal with them at all, but this slowly
leads to service degradations as space leaks out of the filesystem.
Hopefully, someone will schedule a rebuild of the free space information to
plug all those leaks.
Offline repair rebuilds all space metadata after recording the usage of
the files and directories that it decides not to clear, hence it can build new
structures in the discovered free space and avoid the question of reaping.
(每当在线 fsck 构建一个新数据结构来替换一个可疑的结构时，就存在如何找到并处置属于旧结构的块的问题。最懒惰的方法当然是对它们置之不理，但随着空间从文件系统中泄漏出去，这会慢慢导致服务退化。希望有人会调度一次空闲空间信息的重建来堵住所有这些泄漏。离线修复在记录了它决定不清除的文件和目录的使用情况之后重建所有空间元数据，因此它可以在发现的空闲空间中构建新结构，从而回避回收问题。)

As part of a repair, online fsck relies heavily on the reverse mapping records
to find space that is owned by the corresponding rmap owner yet truly free.
Cross referencing rmap records with other rmap records is necessary because
there may be other data structures that also think they own some of those
blocks (e.g. crosslinked trees).
Permitting the block allocator to hand them out again will not push the system
towards consistency.
(作为修复的一部分，在线 fsck 高度依赖反向映射记录来查找由对应 rmap 属主拥有、但确实空闲的空间。必须将 rmap 记录与其他 rmap 记录交叉引用，因为可能还有其他数据结构也认为它们拥有其中一些块(例如交叉链接的树)。允许块分配器再次把它们发放出去并不能推动系统走向一致。)

For space metadata, the process of finding extents to dispose of generally
follows this format:
(对于空间元数据，寻找待处置区段的过程通常遵循如下格式：)

1. Create a bitmap of space used by data structures that must be preserved.
   The space reservations used to create the new metadata can be used here if
   the same rmap owner code is used to denote all of the objects being rebuilt.
   (创建一个位图，记录必须保留的数据结构所使用的空间。如果用同一个 rmap 属主代码来表示所有正在重建的对象，则可以在这里使用用于创建新元数据的空间预留。)

2. Survey the reverse mapping data to create a bitmap of space owned by the
   same ``XFS_RMAP_OWN_*`` number for the metadata that is being preserved.
   (调查反向映射数据，为被保留元数据创建一个由相同 ``XFS_RMAP_OWN_*`` 编号拥有的空间的位图。)

3. Use the bitmap disunion operator to subtract (1) from (2).
   The remaining set bits represent candidate extents that could be freed.
   The process moves on to step 4 below.
   (使用位图差集运算符从 (2) 中减去 (1)。剩余的置位表示可能被释放的候选区段。过程继续进行到下面的步骤 4。)

Repairs for file-based metadata such as extended attributes, directories,
symbolic links, quota files and realtime bitmaps are performed by building a
new structure attached to a temporary file and exchanging all mappings in the
file forks.
Afterward, the mappings in the old file fork are the candidate blocks for
disposal.
(诸如扩展属性、目录、符号链接、配额文件和实时位图之类的基于文件的元数据的修复，通过构建一个附着到临时文件的新结构并交换文件 fork 中的所有映射来执行。此后，旧文件 fork 中的映射就是待处置的候选块。)

The process for disposing of old extents is as follows:
(处置旧区段的过程如下：)

4. For each candidate extent, count the number of reverse mapping records for
   the first block in that extent that do not have the same rmap owner for the
   data structure being repaired.
   (对于每个候选区段，统计该区段第一个块的不属于被修复数据结构同一 rmap 属主的反向映射记录数量。)

   - If zero, the block has a single owner and can be freed.
     (如果为零，该块只有一个属主，可以释放。)

   - If not, the block is part of a crosslinked structure and must not be
     freed.
     (如果不为零，该块是交叉链接结构的一部分，绝不能释放。)

5. Starting with the next block in the extent, figure out how many more blocks
   have the same zero/nonzero other owner status as that first block.
   (从该区段中的下一个块开始，计算还有多少个块与第一个块具有相同的零/非零其他属主状态。)

6. If the region is crosslinked, delete the reverse mapping entry for the
   structure being repaired and move on to the next region.
   (如果该区域是交叉链接的，则删除被修复结构的反向映射条目，并继续处理下一个区域。)

7. If the region is to be freed, mark any corresponding buffers in the buffer
   cache as stale to prevent log writeback.
   (如果该区域将被释放，则将缓冲缓存中所有对应的缓冲标记为过期(stale)，以防止日志回写。)

8. Free the region and move on.
   (释放该区域并继续。)

However, there is one complication to this procedure.
Transactions are of finite size, so the reaping process must be careful to roll
the transactions to avoid overruns.
Overruns come from two sources:
(然而，这一过程有一个复杂之处。事务大小是有限的，因此回收过程必须小心地滚动事务以避免超限。超限来自两个来源：)

a. EFIs logged on behalf of space that is no longer occupied
   (为不再被占用的空间记录的 EFI)

b. Log items for buffer invalidations
   (用于缓冲失效的日志项)

This is also a window in which a crash during the reaping process can leak
blocks.
As stated earlier, online repair functions use very large transactions to
minimize the chances of this occurring.
(这也是一个窗口，在此期间回收过程中的崩溃可能泄漏块。如前所述，在线修复函数使用非常大的事务来尽量减少这种情况发生的几率。)

Case Study: Reaping After a Regular Btree Repair
(案例研究：常规 btree 修复后的回收)
````````````````````````````````````````````````

Old reference count and inode btrees are the easiest to reap because they have
rmap records with special owner codes: ``XFS_RMAP_OWN_REFC`` for the refcount
btree, and ``XFS_RMAP_OWN_INOBT`` for the inode and free inode btrees.
Creating a list of extents to reap the old btree blocks is quite simple,
conceptually:
(旧的引用计数和 inode btree 最容易回收，因为它们具有带特殊属主代码的 rmap 记录：引用计数 btree 用 ``XFS_RMAP_OWN_REFC``，inode 和空闲 inode btree 用 ``XFS_RMAP_OWN_INOBT``。从概念上说，创建一个用于回收旧 btree 块的区段列表相当简单：)

1. Lock the relevant AGI/AGF header buffers to prevent allocation and frees.
   (锁定相关的 AGI/AGF 头缓冲，以防止分配和释放。)

2. For each reverse mapping record with an rmap owner corresponding to the
   metadata structure being rebuilt, set the corresponding range in a bitmap.
   (对于每条 rmap 属主与正在重建的元数据结构相对应的反向映射记录，在位图中设置相应的范围。)

3. Walk the current data structures that have the same rmap owner.
   For each block visited, clear that range in the above bitmap.
   (遍历具有相同 rmap 属主的当前数据结构。对访问的每个块，清除上述位图中该范围。)

4. Each set bit in the bitmap represents a block that could be a block from the
   old data structures and hence is a candidate for reaping.
   In other words, ``(rmap_records_owned_by & ~blocks_reachable_by_walk)``
   are the blocks that might be freeable.
   (位图中的每个置位表示一个可能是旧数据结构块的块，因此是回收的候选。换句话说，``(rmap_records_owned_by & ~blocks_reachable_by_walk)`` 就是那些可能可释放的块。)

If it is possible to maintain the AGF lock throughout the repair (which is the
common case), then step 2 can be performed at the same time as the reverse
mapping record walk that creates the records for the new btree.
(如果可以在整个修复期间保持 AGF 锁(这是常见情形)，则步骤 2 可以与为新 btree 创建记录的反向映射记录遍历同时执行。)

Case Study: Rebuilding the Free Space Indices
(案例研究：重建空闲空间索引)
`````````````````````````````````````````````

The high level process to rebuild the free space indices is:
(重建空闲空间索引的高层流程是：)

1. Walk the reverse mapping records to generate ``struct xfs_alloc_rec_incore``
   records from the gaps in the reverse mapping btree.
   (遍历反向映射记录，从反向映射 btree 的间隙中生成 ``struct xfs_alloc_rec_incore`` 记录。)

2. Append the records to an xfarray.
   (将这些记录追加到 xfarray。)

3. Use the ``xfs_btree_bload_compute_geometry`` function to compute the number
   of blocks needed for each new tree.
   (使用 ``xfs_btree_bload_compute_geometry`` 函数为每棵新树计算所需的块数。)

4. Allocate the number of blocks computed in the previous step from the free
   space information collected.
   (从收集的空闲空间信息中分配上一步计算出的块数。)

5. Use ``xfs_btree_bload`` to write the xfarray records to btree blocks and
   generate the internal node blocks for the free space by length index.
   Call it again for the free space by block number index.
   (使用 ``xfs_btree_bload`` 将 xfarray 记录写入 btree 块，并为按长度索引的空闲空间生成内部节点块。对按块号索引的空闲空间再次调用它。)

6. Commit the locations of the new btree root blocks to the AGF.
   (将新 btree 根块的位置提交到 AGF。)

7. Reap the old btree blocks by looking for space that is not recorded by the
   reverse mapping btree, the new free space btrees, or the AGFL.
   (通过查找未被反向映射 btree、新空闲空间 btree 或 AGFL 记录的空间，回收旧的 btree 块。)

Repairing the free space btrees has three key complications over a regular
btree repair:
(与常规 btree 修复相比，修复空闲空间 btree 有三个关键复杂之处：)

First, free space is not explicitly tracked in the reverse mapping records.
Hence, the new free space records must be inferred from gaps in the physical
space component of the keyspace of the reverse mapping btree.
(第一，空闲空间没有在反向映射记录中被显式跟踪。因此，必须从反向映射 btree 键空间的物理空间分量中的间隙推断出新的空闲空间记录。)

Second, free space repairs cannot use the common btree reservation code because
new blocks are reserved out of the free space btrees.
This is impossible when repairing the free space btrees themselves.
However, repair holds the AGF buffer lock for the duration of the free space
index reconstruction, so it can use the collected free space information to
supply the blocks for the new free space btrees.
It is not necessary to back each reserved extent with an EFI because the new
free space btrees are constructed in what the ondisk filesystem thinks is
unowned space.
However, if reserving blocks for the new btrees from the collected free space
information changes the number of free space records, repair must re-estimate
the new free space btree geometry with the new record count until the
reservation is sufficient.
As part of committing the new btrees, repair must ensure that reverse mappings
are created for the reserved blocks and that unused reserved blocks are
inserted into the free space btrees.
Deferrred rmap and freeing operations are used to ensure that this transition
is atomic, similar to the other btree repair functions.
(第二，空闲空间修复不能使用通用的 btree 预留代码，因为新块是从空闲空间 btree 中预留的。在修复空闲空间 btree 自身时这是不可能的。不过，修复在空闲空间索引重建期间持有 AGF 缓冲锁，因此它可以使用收集到的空闲空间信息来为新空闲空间 btree 提供块。不必为每个预留区段配备 EFI，因为新空闲空间 btree 是在磁盘文件系统认为是无属主空间的地方构建的。然而，如果从收集的空闲空间信息中为新 btree 预留块改变了空闲空间记录的数量，修复必须用新的记录数重新估计新空闲空间 btree 的几何形状，直到预留足够为止。作为提交新 btree 的一部分，修复必须确保为预留块创建反向映射，并将未使用的预留块插入空闲空间 btree。使用延迟的 rmap 和释放操作来确保这一转换是原子的，与其他 btree 修复函数类似。)

Third, finding the blocks to reap after the repair is not overly
straightforward.
Blocks for the free space btrees and the reverse mapping btrees are supplied by
the AGFL.
Blocks put onto the AGFL have reverse mapping records with the owner
``XFS_RMAP_OWN_AG``.
This ownership is retained when blocks move from the AGFL into the free space
btrees or the reverse mapping btrees.
When repair walks reverse mapping records to synthesize free space records, it
creates a bitmap (``ag_owner_bitmap``) of all the space claimed by
``XFS_RMAP_OWN_AG`` records.
The repair context maintains a second bitmap corresponding to the rmap btree
blocks and the AGFL blocks (``rmap_agfl_bitmap``).
When the walk is complete, the bitmap disunion operation ``(ag_owner_bitmap &
~rmap_agfl_bitmap)`` computes the extents that are used by the old free space
btrees.
These blocks can then be reaped using the methods outlined above.
(第三，找到修复后要回收的块并不那么直截了当。空闲空间 btree 和反向映射 btree 的块由 AGFL 提供。放入 AGFL 的块具有属主为 ``XFS_RMAP_OWN_AG`` 的反向映射记录。当块从 AGFL 移入空闲空间 btree 或反向映射 btree 时，这一属主关系被保留。当修复遍历反向映射记录以合成空闲空间记录时，它会创建一个记录 ``XFS_RMAP_OWN_AG`` 记录所声明全部空间的位图(``ag_owner_bitmap``)。修复上下文维护第二个位图，对应 rmap btree 块和 AGFL 块(``rmap_agfl_bitmap``)。遍历完成后，位图差集运算 ``(ag_owner_bitmap & ~rmap_agfl_bitmap)`` 计算出旧空闲空间 btree 使用的区段。然后可以使用上述方法回收这些块。)

.. _rmap_reap:

Case Study: Reaping After Repairing Reverse Mapping Btrees
(案例研究：修复反向映射 btree 后的回收)
``````````````````````````````````````````````````````````

Old reverse mapping btrees are less difficult to reap after a repair.
As mentioned in the previous section, blocks on the AGFL, the two free space
btree blocks, and the reverse mapping btree blocks all have reverse mapping
records with ``XFS_RMAP_OWN_AG`` as the owner.
The full process of gathering reverse mapping records and building a new btree
are described in the case study of
:ref:`live rebuilds of rmap data <rmap_repair>`, but a crucial point from that
discussion is that the new rmap btree will not contain any records for the old
rmap btree, nor will the old btree blocks be tracked in the free space btrees.
The list of candidate reaping blocks is computed by setting the bits
corresponding to the gaps in the new rmap btree records, and then clearing the
bits corresponding to extents in the free space btrees and the current AGFL
blocks.
The result ``(new_rmapbt_gaps & ~(agfl | bnobt_records))`` are reaped using the
methods outlined above.
(修复之后，旧反向映射 btree 的回收难度较低。如上一节所述，AGFL 上的块、两个空闲空间 btree 块以及反向映射 btree 块都具有属主为 ``XFS_RMAP_OWN_AG`` 的反向映射记录。收集反向映射记录并构建新 btree 的完整过程在 :ref:`rmap 数据的实时重建 <rmap_repair>` 案例研究中描述，但该讨论中的一个关键点是：新 rmap btree 不会包含任何针对旧 rmap btree 的记录，旧 btree 块也不会被空闲空间 btree 跟踪。候选回收块列表的计算方式是：设置与新 rmap btree 记录中的间隙相对应的位，然后清除与空闲空间 btree 中的区段以及当前 AGFL 块相对应的位。结果 ``(new_rmapbt_gaps & ~(agfl | bnobt_records))`` 使用上述方法回收。)

The rest of the process of rebuildng the reverse mapping btree is discussed
in a separate :ref:`case study<rmap_repair>`.
(重建反向映射 btree 过程的其余部分在单独的 :ref:`案例研究 <rmap_repair>` 中讨论。)

Case Study: Rebuilding the AGFL
(案例研究：重建 AGFL)
```````````````````````````````

The allocation group free block list (AGFL) is repaired as follows:
(分配组空闲块列表(AGFL)按如下方式修复：)

1. Create a bitmap for all the space that the reverse mapping data claims is
   owned by ``XFS_RMAP_OWN_AG``.
   (为反向映射数据声称由 ``XFS_RMAP_OWN_AG`` 拥有的所有空间创建一个位图。)

2. Subtract the space used by the two free space btrees and the rmap btree.
   (减去两个空闲空间 btree 和 rmap btree 使用的空间。)

3. Subtract any space that the reverse mapping data claims is owned by any
   other owner, to avoid re-adding crosslinked blocks to the AGFL.
   (减去反向映射数据声称由任何其他属主拥有的空间，以避免把交叉链接的块重新添加到 AGFL。)

4. Once the AGFL is full, reap any blocks leftover.
   (一旦 AGFL 装满，回收所有剩余的块。)

5. The next operation to fix the freelist will right-size the list.
   (下一次修复空闲列表的操作将会把列表调整为合适的大小。)

See `fs/xfs/scrub/agheader_repair.c <https://git.kernel.org/pub/scm/linux/kernel/git/torvalds/linux.git/tree/fs/xfs/scrub/agheader_repair.c>`_ for more details.
(更多细节参见 `fs/xfs/scrub/agheader_repair.c <https://git.kernel.org/pub/scm/linux/kernel/git/torvalds/linux.git/tree/fs/xfs/scrub/agheader_repair.c>`_。)

Inode Record Repairs
(inode 记录修复)
--------------------

Inode records must be handled carefully, because they have both ondisk records
("dinodes") and an in-memory ("cached") representation.
There is a very high potential for cache coherency issues if online fsck is not
careful to access the ondisk metadata *only* when the ondisk metadata is so
badly damaged that the filesystem cannot load the in-memory representation.
When online fsck wants to open a damaged file for scrubbing, it must use
specialized resource acquisition functions that return either the in-memory
representation *or* a lock on whichever object is necessary to prevent any
update to the ondisk location.
(inode 记录必须小心处理，因为它们同时具有磁盘记录("dinodes")和内存中("缓存")表示。如果在线 fsck 不小心做到*只在*磁盘元数据损坏到文件系统无法加载内存表示时才访问磁盘元数据，那么出现缓存一致性问题的可能性非常高。当在线 fsck 想要打开一个受损文件进行 scrubbing 时，它必须使用专门的资源获取函数，该函数返回内存表示*或*为防止磁盘位置被任何更新而必需的对象上的锁。)

The only repairs that should be made to the ondisk inode buffers are whatever
is necessary to get the in-core structure loaded.
This means fixing whatever is caught by the inode cluster buffer and inode fork
verifiers, and retrying the ``iget`` operation.
If the second ``iget`` fails, the repair has failed.
(对磁盘 inode 缓冲唯一应做的修复是让内核结构能够被加载所需的修复。这意味着修复 inode 簇缓冲和 inode fork 校验器捕获到的任何问题，并重试 ``iget`` 操作。如果第二次 ``iget`` 失败，则修复失败。)

Once the in-memory representation is loaded, repair can lock the inode and can
subject it to comprehensive checks, repairs, and optimizations.
Most inode attributes are easy to check and constrain, or are user-controlled
arbitrary bit patterns; these are both easy to fix.
Dealing with the data and attr fork extent counts and the file block counts is
more complicated, because computing the correct value requires traversing the
forks, or if that fails, leaving the fields invalid and waiting for the fork
fsck functions to run.
(一旦内存表示被加载，修复就可以锁定 inode，并对其进行全面的检查、修复和优化。大多数 inode 属性易于检查和约束，或者是用户控制的任意位模式；这两者都容易修复。处理数据和属性 fork 的区段计数以及文件块计数则更复杂，因为计算正确的值需要遍历 fork，如果做不到，就让字段保持无效并等待 fork fsck 函数运行。)

Quota Record Repairs
(配额记录修复)
--------------------

Similar to inodes, quota records ("dquots") also have both ondisk records and
an in-memory representation, and hence are subject to the same cache coherency
issues.
Somewhat confusingly, both are known as dquots in the XFS codebase.
(与 inode 类似，配额记录("dquots")也同时具有磁盘记录和内存表示，因此同样面临缓存一致性问题。有些令人困惑的是，在 XFS 代码库中两者都被称为 dquot。)

The only repairs that should be made to the ondisk quota record buffers are
whatever is necessary to get the in-core structure loaded.
Once the in-memory representation is loaded, the only attributes needing
checking are obviously bad limits and timer values.
(对磁盘配额记录缓冲唯一应做的修复是让内核结构能够被加载所需的修复。一旦内存表示被加载，唯一需要检查的属性是明显错误的限制值和计时器值。)

Quota usage counters are checked, repaired, and discussed separately in the
section about :ref:`live quotacheck <quotacheck>`.
(配额使用计数器的检查、修复和讨论在关于 :ref:`实时配额检查 <quotacheck>` 的章节中单独进行。)

.. _fscounters:

Freezing to Fix Summary Counters
(冻结以修复摘要计数器)
--------------------------------

Filesystem summary counters track availability of filesystem resources such
as free blocks, free inodes, and allocated inodes.
This information could be compiled by walking the free space and inode indexes,
but this is a slow process, so XFS maintains a copy in the ondisk superblock
that should reflect the ondisk metadata, at least when the filesystem has been
unmounted cleanly.
For performance reasons, XFS also maintains incore copies of those counters,
which are key to enabling resource reservations for active transactions.
Writer threads reserve the worst-case quantities of resources from the
incore counter and give back whatever they don't use at commit time.
It is therefore only necessary to serialize on the superblock when the
superblock is being committed to disk.
(文件系统摘要计数器跟踪文件系统资源(如空闲块、空闲 inode 和已分配 inode)的可用性。这些信息本可以通过遍历空闲空间和 inode 索引来汇总，但这是一个缓慢的过程，所以 XFS 在磁盘超级块中维护一份副本，它应当反映磁盘元数据——至少在文件系统被干净卸载时如此。出于性能原因，XFS 还在内存中维护这些计数器的副本，它们是启用活动事务资源预留的关键。写线程从内存计数器中预留最坏情况下的资源量，并在提交时归还它们未使用的部分。因此，只有当超级块正被提交到磁盘时才需要在超级块上串行化。)

The lazy superblock counter feature introduced in XFS v5 took this even further
by training log recovery to recompute the summary counters from the AG headers,
which eliminated the need for most transactions even to touch the superblock.
The only time XFS commits the summary counters is at filesystem unmount.
To reduce contention even further, the incore counter is implemented as a
percpu counter, which means that each CPU is allocated a batch of blocks from a
global incore counter and can satisfy small allocations from the local batch.
(XFS v5 引入的惰性超级块计数器特性更进一步，训练日志恢复从 AG 头重新计算摘要计数器，这免除了大多数事务接触超级块的需要。XFS 只在文件系统卸载时提交摘要计数器。为了进一步减少争用，内存计数器被实现为 percpu 计数器，这意味着每个 CPU 从一个全局内存计数器中分配一批块，并可以用本地批次满足小额分配。)

The high-performance nature of the summary counters makes it difficult for
online fsck to check them, since there is no way to quiesce a percpu counter
while the system is running.
Although online fsck can read the filesystem metadata to compute the correct
values of the summary counters, there's no way to hold the value of a percpu
counter stable, so it's quite possible that the counter will be out of date by
the time the walk is complete.
Earlier versions of online scrub would return to userspace with an incomplete
scan flag, but this is not a satisfying outcome for a system administrator.
For repairs, the in-memory counters must be stabilized while walking the
filesystem metadata to get an accurate reading and install it in the percpu
counter.
(摘要计数器的高性能特性使得在线 fsck 难以检查它们，因为在系统运行时没有办法静默一个 percpu 计数器。虽然在线 fsck 可以读取文件系统元数据来计算摘要计数器的正确值，但没有办法保持 percpu 计数器的值稳定，所以在遍历完成时计数器完全可能已经过时。早期版本的在线 scrub 会带着未完成扫描标志返回用户空间，但这对系统管理员来说不是一个令人满意的结果。对于修复，必须在遍历文件系统元数据的同时稳定内存计数器，以获得准确的读数并将其安装到 percpu 计数器中。)

To satisfy this requirement, online fsck must prevent other programs in the
system from initiating new writes to the filesystem, it must disable background
garbage collection threads, and it must wait for existing writer programs to
exit the kernel.
Once that has been established, scrub can walk the AG free space indexes, the
inode btrees, and the realtime bitmap to compute the correct value of all
four summary counters.
This is very similar to a filesystem freeze, though not all of the pieces are
necessary:
(为满足这一要求，在线 fsck 必须阻止系统中的其他程序向文件系统发起新的写入，必须禁用后台垃圾收集线程，并且必须等待现有的写程序退出内核。一旦做到这些，scrub 就可以遍历 AG 空闲空间索引、inode btree 和实时位图，计算出全部四个摘要计数器的正确值。这与文件系统冻结非常相似，不过并非所有环节都是必需的：)

- The final freeze state is set one higher than ``SB_FREEZE_COMPLETE`` to
  prevent other threads from thawing the filesystem, or other scrub threads
  from initiating another fscounters freeze.
  (最终冻结状态被设置为比 ``SB_FREEZE_COMPLETE`` 高一级，以防止其他线程解冻文件系统，或其他 scrub 线程发起另一次 fscounters 冻结。)

- It does not quiesce the log.
  (它不静默日志。)

With this code in place, it is now possible to pause the filesystem for just
long enough to check and correct the summary counters.
(有了这些代码，现在可以让文件系统暂停刚好足够长的时间，以检查并纠正摘要计数器。)

+--------------------------------------------------------------------------+
| **Historical Sidebar**:                                                  |
| (历史侧栏：)                                                                  |
+--------------------------------------------------------------------------+
| The initial implementation used the actual VFS filesystem freeze         |
| mechanism to quiesce filesystem activity.                                |
| With the filesystem frozen, it is possible to resolve the counter values |
| with exact precision, but there are many problems with calling the VFS   |
| methods directly:                                                        |
|                                                                          |
| (最初的实现使用实际的 VFS 文件系统冻结机制来静默文件系统活动。)                                      |
| (在文件系统被冻结的情况下，可以以精确的精度解析计数器的值，但直接调用 VFS 方法存在许多问题：)                       |
|                                                                          |
| - Other programs can unfreeze the filesystem without our knowledge.      |
|   This leads to incorrect scan results and incorrect repairs.            |
|                                                                          |
| (其他程序可以在我们不知情的情况下解冻文件系统。这会导致错误的扫描结果和错误的修复。)                              |
|                                                                          |
| - Adding an extra lock to prevent others from thawing the filesystem     |
|   required the addition of a ``->freeze_super`` function to wrap         |
|   ``freeze_fs()``.                                                       |
|   This in turn caused other subtle problems because it turns out that    |
|   the VFS ``freeze_super`` and ``thaw_super`` functions can drop the     |
|   last reference to the VFS superblock, and any subsequent access        |
|   becomes a UAF bug!                                                     |
|   This can happen if the filesystem is unmounted while the underlying    |
|   block device has frozen the filesystem.                                |
|   This problem could be solved by grabbing extra references to the       |
|   superblock, but it felt suboptimal given the other inadequacies of     |
|   this approach.                                                         |
|                                                                          |
| (为防止他人解冻文件系统而添加额外的锁，需要添加一个 ``->freeze_super`` 函数来封装 ``freeze_fs()``。)    |
| (这又引发了其他微妙的问题，因为 VFS 的 ``freeze_super`` 和 ``thaw_super``)                |
| (函数可能会丢弃对 VFS 超级块的最后一个引用，之后的任何访问都会变成 UAF 错误！)                            |
| (如果文件系统在底层块设备已冻结文件系统时被卸载，就可能发生这种情况。)                                     |
| (这个问题可以通过获取对超级块的额外引用来解决，但考虑到这种方法的其他不足，这感觉并不理想。)                          |
|                                                                          |
| - The log need not be quiesced to check the summary counters, but a VFS  |
|   freeze initiates one anyway.                                           |
|   This adds unnecessary runtime to live fscounter fsck operations.       |
|                                                                          |
| (检查摘要计数器并不需要静默日志，但 VFS 冻结无论如何都会发起一次静默。这给实时 fscounter fsck 操作增加了不必要的运行时间。)|
|                                                                          |
| - Quiescing the log means that XFS flushes the (possibly incorrect)      |
|   counters to disk as part of cleaning the log.                          |
|                                                                          |
| (静默日志意味着 XFS 会在清理日志的过程中将(可能不正确的)计数器刷新到磁盘。)                               |
|                                                                          |
| - A bug in the VFS meant that freeze could complete even when            |
|   sync_filesystem fails to flush the filesystem and returns an error.    |
|   This bug was fixed in Linux 5.17.                                      |
|                                                                          |
| (VFS 中的一个 bug 意味着即使 sync_filesystem 刷新文件系统失败并返回错误，冻结也可能完成。)              |
| (该 bug 已在 Linux 5.17 中修复。)                                               |
+--------------------------------------------------------------------------+

Full Filesystem Scans
(全文件系统扫描)
---------------------

Certain types of metadata can only be checked by walking every file in the
entire filesystem to record observations and comparing the observations against
what's recorded on disk.
Like every other type of online repair, repairs are made by writing those
observations to disk in a replacement structure and committing it atomically.
However, it is not practical to shut down the entire filesystem to examine
hundreds of billions of files because the downtime would be excessive.
Therefore, online fsck must build the infrastructure to manage a live scan of
all the files in the filesystem.
There are two questions that need to be solved to perform a live walk:
(某些类型的元数据只能通过遍历整个文件系统中的每个文件来记录观察结果，并将观察结果与磁盘上记录的内容进行比较才能检查。与其他每种在线修复一样，修复通过将这些观察结果以替换结构的形式写入磁盘并原子地提交来完成。然而，关闭整个文件系统来检查数千亿个文件并不实际，因为停机时间会过长。因此，在线 fsck 必须构建基础设施来管理对所有文件系统文件的实时扫描。执行一次实时遍历需要解决两个问题：)

- How does scrub manage the scan while it is collecting data?
  (scrub 在收集数据时如何管理扫描？)

- How does the scan keep abreast of changes being made to the system by other
  threads?
  (扫描如何紧跟其他线程对系统所做的更改？)

.. _iscan:

Coordinated Inode Scans
(协调的 inode 扫描)
```````````````````````

In the original Unix filesystems of the 1970s, each directory entry contained
an index number (*inumber*) which was used as an index into on ondisk array
(*itable*) of fixed-size records (*inodes*) describing a file's attributes and
its data block mapping.
This system is described by J. Lions, `"inode (5659)"
<http://www.lemis.com/grog/Documentation/Lions/>`_ in *Lions' Commentary on
UNIX, 6th Edition*, (Dept. of Computer Science, the University of New South
Wales, November 1977), pp. 18-2; and later by D. Ritchie and K. Thompson,
`"Implementation of the File System"
<https://archive.org/details/bstj57-6-1905/page/n8/mode/1up>`_, from *The UNIX
Time-Sharing System*, (The Bell System Technical Journal, July 1978), pp.
1913-4.
(在 1970 年代的原始 Unix 文件系统中，每个目录项包含一个索引号(*inumber*)，它被用作进入磁盘上一个固定大小记录(*inode*)数组(*itable*)的索引，该数组描述文件的属性及其数据块映射。这一系统由 J. Lions 在《*Lions' Commentary on UNIX, 6th Edition*》(新南威尔士大学计算机科学系，1977 年 11 月)的 `"inode (5659)" <http://www.lemis.com/grog/Documentation/Lions/>`_ 第 18-2 页中描述；后来由 D. Ritchie 和 K. Thompson 在《*The UNIX Time-Sharing System*》(The Bell System Technical Journal，1978 年 7 月)的 `"Implementation of the File System" <https://archive.org/details/bstj57-6-1905/page/n8/mode/1up>`_ 第 1913-4 页中描述。)

XFS retains most of this design, except now inumbers are search keys over all
the space in the data section filesystem.
They form a continuous keyspace that can be expressed as a 64-bit integer,
though the inodes themselves are sparsely distributed within the keyspace.
Scans proceed in a linear fashion across the inumber keyspace, starting from
``0x0`` and ending at ``0xFFFFFFFFFFFFFFFF``.
Naturally, a scan through a keyspace requires a scan cursor object to track the
scan progress.
Because this keyspace is sparse, this cursor contains two parts.
The first part of this scan cursor object tracks the inode that will be
examined next; call this the examination cursor.
Somewhat less obviously, the scan cursor object must also track which parts of
the keyspace have already been visited, which is critical for deciding if a
concurrent filesystem update needs to be incorporated into the scan data.
Call this the visited inode cursor.
(XFS 保留了这一设计的大部分，只是现在 inumber 是对数据区文件系统中所有空间的搜索键。它们构成一个可以表示为 64 位整数的连续键空间，尽管 inode 本身在键空间中是稀疏分布的。扫描以线性方式跨越 inumber 键空间进行，从 ``0x0`` 开始，到 ``0xFFFFFFFFFFFFFFFF`` 结束。自然，遍历键空间的扫描需要一个扫描游标对象来跟踪扫描进度。由于这个键空间是稀疏的，该游标包含两个部分。这个扫描游标对象的第一部分跟踪下一个将被检查的 inode；称之为检查游标。不太明显的是，扫描游标对象还必须跟踪键空间的哪些部分已被访问过，这对于决定并发文件系统更新是否需要纳入扫描数据至关重要。称之为已访问 inode 游标。)

Advancing the scan cursor is a multi-step process encapsulated in
``xchk_iscan_iter``:
(推进扫描游标是一个封装在 ``xchk_iscan_iter`` 中的多步骤过程：)

1. Lock the AGI buffer of the AG containing the inode pointed to by the visited
   inode cursor.
   This guarantee that inodes in this AG cannot be allocated or freed while
   advancing the cursor.
   (锁定包含已访问 inode 游标所指向 inode 的 AG 的 AGI 缓冲。这保证在推进游标期间该 AG 中的 inode 不能被分配或释放。)

2. Use the per-AG inode btree to look up the next inumber after the one that
   was just visited, since it may not be keyspace adjacent.
   (使用每 AG 的 inode btree 查找刚访问过的 inumber 之后的下一个 inumber，因为它可能不是键空间相邻的。)

3. If there are no more inodes left in this AG:
   (如果该 AG 中没有更多 inode：)

   a. Move the examination cursor to the point of the inumber keyspace that
      corresponds to the start of the next AG.
      (将检查游标移动到与下一个 AG 起始相对应的 inumber 键空间位置。)

   b. Adjust the visited inode cursor to indicate that it has "visited" the
      last possible inode in the current AG's inode keyspace.
      XFS inumbers are segmented, so the cursor needs to be marked as having
      visited the entire keyspace up to just before the start of the next AG's
      inode keyspace.
      (调整已访问 inode 游标，以表明它已"访问"了当前 AG 的 inode 键空间中最后一个可能的 inode。XFS 的 inumber 是分段的，所以游标需要被标记为已访问直到下一个 AG 的 inode 键空间开始之前为止的整个键空间。)

   c. Unlock the AGI and return to step 1 if there are unexamined AGs in the
      filesystem.
      (如果文件系统中还有未检查的 AG，解锁 AGI 并返回步骤 1。)

   d. If there are no more AGs to examine, set both cursors to the end of the
      inumber keyspace.
      The scan is now complete.
      (如果没有更多要检查的 AG，则将两个游标都设置为 inumber 键空间的末尾。扫描至此完成。)

4. Otherwise, there is at least one more inode to scan in this AG:
   (否则，该 AG 中至少还有一个 inode 待扫描：)

   a. Move the examination cursor ahead to the next inode marked as allocated
      by the inode btree.
      (将检查游标向前移动到 inode btree 标记为已分配的下一个 inode。)

   b. Adjust the visited inode cursor to point to the inode just prior to where
      the examination cursor is now.
      Because the scanner holds the AGI buffer lock, no inodes could have been
      created in the part of the inode keyspace that the visited inode cursor
      just advanced.
      (调整已访问 inode 游标，使其指向检查游标当前位置之前的那个 inode。由于扫描器持有 AGI 缓冲锁，在已访问 inode 游标刚刚推进过的 inode 键空间部分中不可能创建了任何 inode。)

5. Get the incore inode for the inumber of the examination cursor.
   By maintaining the AGI buffer lock until this point, the scanner knows that
   it was safe to advance the examination cursor across the entire keyspace,
   and that it has stabilized this next inode so that it cannot disappear from
   the filesystem until the scan releases the incore inode.
   (获取检查游标 inumber 的内存中 inode。通过一直保持 AGI 缓冲锁到这一步，扫描器知道将检查游标推进过整个键空间是安全的，并且它已稳定了这个下一个 inode，使其在扫描释放内存中 inode 之前不会从文件系统中消失。)

6. Drop the AGI lock and return the incore inode to the caller.
   (释放 AGI 锁并将内存中 inode 返回给调用者。)

Online fsck functions scan all files in the filesystem as follows:
(在线 fsck 函数按如下方式扫描文件系统中的所有文件：)

1. Start a scan by calling ``xchk_iscan_start``.
   (通过调用 ``xchk_iscan_start`` 启动扫描。)

2. Advance the scan cursor (``xchk_iscan_iter``) to get the next inode.
   If one is provided:
   (推进扫描游标(``xchk_iscan_iter``)以获取下一个 inode。如果提供了一个：)

   a. Lock the inode to prevent updates during the scan.
      (锁定 inode 以防止扫描期间被更新。)

   b. Scan the inode.
      (扫描该 inode。)

   c. While still holding the inode lock, adjust the visited inode cursor
      (``xchk_iscan_mark_visited``) to point to this inode.
      (在仍持有 inode 锁的情况下，调整已访问 inode 游标(``xchk_iscan_mark_visited``)以指向这个 inode。)

   d. Unlock and release the inode.
      (解锁并释放该 inode。)

8. Call ``xchk_iscan_teardown`` to complete the scan.
   (调用 ``xchk_iscan_teardown`` 完成扫描。)

There are subtleties with the inode cache that complicate grabbing the incore
inode for the caller.
Obviously, it is an absolute requirement that the inode metadata be consistent
enough to load it into the inode cache.
Second, if the incore inode is stuck in some intermediate state, the scan
coordinator must release the AGI and push the main filesystem to get the inode
back into a loadable state.
(inode 缓存有一些微妙之处，使为调用者抓取内存中 inode 变得复杂。显然，inode 元数据必须一致到足以加载进 inode 缓存，这是绝对要求。其次，如果内存中 inode 卡在某个中间状态，扫描协调器必须释放 AGI 并推动主文件系统使该 inode 恢复到可加载状态。)

Inode Management
(inode 管理)
````````````````

In regular filesystem code, references to allocated XFS incore inodes are
always obtained (``xfs_iget``) outside of transaction context because the
creation of the incore context for an existing file does not require metadata
updates.
However, it is important to note that references to incore inodes obtained as
part of file creation must be performed in transaction context because the
filesystem must ensure the atomicity of the ondisk inode btree index updates
and the initialization of the actual ondisk inode.
(在常规文件系统代码中，对已分配的 XFS 内存中 inode 的引用总是在事务上下文之外获取(``xfs_iget``)，因为为已有文件创建内存上下文不需要元数据更新。然而，需要注意，作为文件创建的一部分获得的内存中 inode 引用必须在事务上下文中执行，因为文件系统必须确保磁盘 inode btree 索引更新与实际磁盘 inode 初始化的原子性。)

References to incore inodes are always released (``xfs_irele``) outside of
transaction context because there are a handful of activities that might
require ondisk updates:
(对内存中 inode 的引用总是在事务上下文之外释放(``xfs_irele``)，因为有少数活动可能需要磁盘更新：)

- The VFS may decide to kick off writeback as part of a ``DONTCACHE`` inode
  release.
  (VFS 可能决定在 ``DONTCACHE`` inode 释放过程中启动回写。)

- Speculative preallocations need to be unreserved.
  (投机性预分配需要取消预留。)

- An unlinked file may have lost its last reference, in which case the entire
  file must be inactivated, which involves releasing all of its resources in
  the ondisk metadata and freeing the inode.
  (一个已解除链接的文件可能失去了最后一个引用，此时整个文件必须被去激活，这涉及释放磁盘元数据中它的所有资源并释放 inode。)

These activities are collectively called inode inactivation.
Inactivation has two parts -- the VFS part, which initiates writeback on all
dirty file pages, and the XFS part, which cleans up XFS-specific information
and frees the inode if it was unlinked.
If the inode is unlinked (or unconnected after a file handle operation), the
kernel drops the inode into the inactivation machinery immediately.
(这些活动统称为 inode 去激活。去激活有两个部分——VFS 部分，对所有脏文件页启动回写；XFS 部分，清理 XFS 特有的信息，并在 inode 已解除链接时释放它。如果 inode 被解除链接(或在文件句柄操作之后未连接)，内核会立即将 inode 投入去激活机制。)

During normal operation, resource acquisition for an update follows this order
to avoid deadlocks:
(在正常运行期间，更新的资源获取遵循以下顺序以避免死锁：)

1. Inode reference (``iget``).
   (inode 引用(``iget``)。)

2. Filesystem freeze protection, if repairing (``mnt_want_write_file``).
   (文件系统冻结保护，如果是修复(``mnt_want_write_file``)。)

3. Inode ``IOLOCK`` (VFS ``i_rwsem``) lock to control file IO.
   (inode ``IOLOCK``(VFS ``i_rwsem``)锁，用于控制文件 IO。)

4. Inode ``MMAPLOCK`` (page cache ``invalidate_lock``) lock for operations that
   can update page cache mappings.
   (inode ``MMAPLOCK``(页缓存 ``invalidate_lock``)锁，用于可能更新页缓存映射的操作。)

5. Log feature enablement.
   (日志特性启用。)

6. Transaction log space grant.
   (事务日志空间授予。)

7. Space on the data and realtime devices for the transaction.
   (事务所需的数据设备和实时设备上的空间。)

8. Incore dquot references, if a file is being repaired.
   Note that they are not locked, merely acquired.
   (内存中 dquot 引用，如果正在修复文件。注意它们不被锁定，只是获取。)

9. Inode ``ILOCK`` for file metadata updates.
   (inode ``ILOCK``，用于文件元数据更新。)

10. AG header buffer locks / Realtime metadata inode ILOCK.
    (AG 头缓冲锁 / 实时元数据 inode ILOCK。)

11. Realtime metadata buffer locks, if applicable.
    (实时元数据缓冲锁，如果适用。)

12. Extent mapping btree blocks, if applicable.
    (区段映射 btree 块，如果适用。)

Resources are often released in the reverse order, though this is not required.
However, online fsck differs from regular XFS operations because it may examine
an object that normally is acquired in a later stage of the locking order, and
then decide to cross-reference the object with an object that is acquired
earlier in the order.
The next few sections detail the specific ways in which online fsck takes care
to avoid deadlocks.
(资源通常以相反的顺序释放，但这并非必需。然而，在线 fsck 与常规 XFS 操作不同，因为它可能检查一个通常在加锁顺序较后阶段才获取的对象，然后决定将该对象与顺序中较早获取的对象交叉引用。接下来几节详细介绍在线 fsck 为避免死锁而采取的具体措施。)

iget and irele During a Scrub
(scrub 期间的 iget 与 irele)
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

An inode scan performed on behalf of a scrub operation runs in transaction
context, and possibly with resources already locked and bound to it.
This isn't much of a problem for ``iget`` since it can operate in the context
of an existing transaction, as long as all of the bound resources are acquired
before the inode reference in the regular filesystem.
(代表 scrub 操作执行的 inode 扫描在事务上下文中运行，并且可能已经有资源被锁定并绑定到它。这对 ``iget`` 来说不是什么问题，因为它可以在现有事务的上下文中运行，只要所有绑定的资源都在常规文件系统的 inode 引用之前获取。)

When the VFS ``iput`` function is given a linked inode with no other
references, it normally puts the inode on an LRU list in the hope that it can
save time if another process re-opens the file before the system runs out
of memory and frees it.
Filesystem callers can short-circuit the LRU process by setting a ``DONTCACHE``
flag on the inode to cause the kernel to try to drop the inode into the
inactivation machinery immediately.
(当 VFS ``iput`` 函数收到一个已链接且没有其他引用的 inode 时，它通常把该 inode 放到 LRU 列表上，希望如果另一个进程在系统内存耗尽并释放它之前重新打开该文件，可以节省时间。文件系统调用者可以通过在 inode 上设置 ``DONTCACHE`` 标志来短路 LRU 过程，使内核尝试立即将 inode 投入去激活机制。)

In the past, inactivation was always done from the process that dropped the
inode, which was a problem for scrub because scrub may already hold a
transaction, and XFS does not support nesting transactions.
On the other hand, if there is no scrub transaction, it is desirable to drop
otherwise unused inodes immediately to avoid polluting caches.
To capture these nuances, the online fsck code has a separate ``xchk_irele``
function to set or clear the ``DONTCACHE`` flag to get the required release
behavior.
(过去，去激活总是由丢弃 inode 的进程执行，这对 scrub 来说是个问题，因为 scrub 可能已经持有一个事务，而 XFS 不支持嵌套事务。另一方面，如果没有 scrub 事务，则希望立即丢弃否则不再使用的 inode，以避免污染缓存。为捕捉这些细微差别，在线 fsck 代码有一个单独的 ``xchk_irele`` 函数来设置或清除 ``DONTCACHE`` 标志，以获得所需的释放行为。)

.. _ilocking:

Locking Inodes
(锁定 inode)
^^^^^^^^^^^^^^

In regular filesystem code, the VFS and XFS will acquire multiple IOLOCK locks
in a well-known order: parent → child when updating the directory tree, and
in numerical order of the addresses of their ``struct inode`` object otherwise.
For regular files, the MMAPLOCK can be acquired after the IOLOCK to stop page
faults.
If two MMAPLOCKs must be acquired, they are acquired in numerical order of
the addresses of their ``struct address_space`` objects.
Due to the structure of existing filesystem code, IOLOCKs and MMAPLOCKs must be
acquired before transactions are allocated.
If two ILOCKs must be acquired, they are acquired in inumber order.
(在常规文件系统代码中，VFS 和 XFS 会以众所周知的顺序获取多个 IOLOCK 锁：更新目录树时按父 → 子的顺序，其他情况按其 ``struct inode`` 对象地址的数字顺序。对于常规文件，可以在 IOLOCK 之后获取 MMAPLOCK 以阻止缺页。如果必须获取两个 MMAPLOCK，则按其 ``struct address_space`` 对象地址的数字顺序获取。由于现有文件系统代码的结构，必须在分配事务之前获取 IOLOCK 和 MMAPLOCK。如果必须获取两个 ILOCK，则按 inumber 顺序获取。)

Inode lock acquisition must be done carefully during a coordinated inode scan.
Online fsck cannot abide these conventions, because for a directory tree
scanner, the scrub process holds the IOLOCK of the file being scanned and it
needs to take the IOLOCK of the file at the other end of the directory link.
If the directory tree is corrupt because it contains a cycle, ``xfs_scrub``
cannot use the regular inode locking functions and avoid becoming trapped in an
ABBA deadlock.
(在协调的 inode 扫描期间必须小心获取 inode 锁。在线 fsck 无法遵守这些惯例，因为对目录树扫描器而言，scrub 进程持有被扫描文件的 IOLOCK，同时需要获取目录链接另一端文件的 IOLOCK。如果目录树因包含环路而损坏，``xfs_scrub`` 无法使用常规的 inode 加锁函数来避免陷入 ABBA 死锁。)

Solving both of these problems is straightforward -- any time online fsck
needs to take a second lock of the same class, it uses trylock to avoid an ABBA
deadlock.
If the trylock fails, scrub drops all inode locks and use trylock loops to
(re)acquire all necessary resources.
Trylock loops enable scrub to check for pending fatal signals, which is how
scrub avoids deadlocking the filesystem or becoming an unresponsive process.
However, trylock loops means that online fsck must be prepared to measure the
resource being scrubbed before and after the lock cycle to detect changes and
react accordingly.
(解决这两个问题都很直接——每当在线 fsck 需要获取同一类的第二个锁时，它都使用 trylock 以避免 ABBA 死锁。如果 trylock 失败，scrub 会放下所有 inode 锁，并使用 trylock 循环来(重新)获取所有必要的资源。trylock 循环使 scrub 能够检查待处理的致命信号，这就是 scrub 避免死锁文件系统或变成无响应进程的方式。然而，trylock 循环意味着在线 fsck 必须准备好在锁循环前后测量被 scrub 的资源，以检测变化并作出相应反应。)

.. _dirparent:

Case Study: Finding a Directory Parent
(案例研究：寻找目录父项)
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

Consider the directory parent pointer repair code as an example.
Online fsck must verify that the dotdot dirent of a directory points up to a
parent directory, and that the parent directory contains exactly one dirent
pointing down to the child directory.
Fully validating this relationship (and repairing it if possible) requires a
walk of every directory on the filesystem while holding the child locked, and
while updates to the directory tree are being made.
The coordinated inode scan provides a way to walk the filesystem without the
possibility of missing an inode.
The child directory is kept locked to prevent updates to the dotdot dirent, but
if the scanner fails to lock a parent, it can drop and relock both the child
and the prospective parent.
If the dotdot entry changes while the directory is unlocked, then a move or
rename operation must have changed the child's parentage, and the scan can
exit early.
(以目录父指针修复代码为例。在线 fsck 必须校验目录的 dotdot dirent 向上指向一个父目录，且父目录恰好包含一个向下指向子目录的 dirent。完整校验这一关系(并在可能时修复它)需要在保持子项锁定的情况下遍历文件系统上的每个目录，同时目录树的更新正在进行。协调的 inode 扫描提供了一种遍历文件系统而不会错过 inode 的方法。子目录保持锁定以防止 dotdot dirent 被更新，但如果扫描器无法锁定父目录，它可以放下并重新锁定子项和候选父目录。如果 dotdot 条目在目录未锁定时发生变化，那么一定是移动或重命名操作改变了子项的父子关系，扫描可以提前退出。)

.. _fshooks:

Filesystem Hooks
(文件系统挂钩)
`````````````````

The second piece of support that online fsck functions need during a full
filesystem scan is the ability to stay informed about updates being made by
other threads in the filesystem, since comparisons against the past are useless
in a dynamic environment.
Two pieces of Linux kernel infrastructure enable online fsck to monitor regular
filesystem operations: filesystem hooks and :ref:`static keys<jump_labels>`.
(在线 fsck 函数在全文件系统扫描期间需要的第二项支持是随时了解文件系统中其他线程正在进行的更新的能力，因为在动态环境中与过去进行比较毫无用处。有两项 Linux 内核基础设施使在线 fsck 能够监控常规文件系统操作：文件系统挂钩和 :ref:`静态键 <jump_labels>`。)

Filesystem hooks convey information about an ongoing filesystem operation to
a downstream consumer.
In this case, the downstream consumer is always an online fsck function.
Because multiple fsck functions can run in parallel, online fsck uses the Linux
notifier call chain facility to dispatch updates to any number of interested
fsck processes.
Call chains are a dynamic list, which means that they can be configured at
run time.
Because these hooks are private to the XFS module, the information passed along
contains exactly what the checking function needs to update its observations.
(文件系统挂钩将正在进行的文件系统操作的信息传递给下游消费者。在这种情况下，下游消费者始终是一个在线 fsck 函数。由于多个 fsck 函数可以并行运行，在线 fsck 使用 Linux 通知调用链设施将更新分发给任意数量的感兴趣的 fsck 进程。调用链是一个动态列表，这意味着它们可以在运行时配置。因为这些挂钩是 XFS 模块私有的，传递的信息恰好包含检查函数更新其观察结果所需的内容。)

The current implementation of XFS hooks uses SRCU notifier chains to reduce the
impact to highly threaded workloads.
Regular blocking notifier chains use a rwsem and seem to have a much lower
overhead for single-threaded applications.
However, it may turn out that the combination of blocking chains and static
keys are a more performant combination; more study is needed here.
(XFS 挂钩的当前实现使用 SRCU 通知链以减少对高度多线程工作负载的影响。常规的阻塞通知链使用 rwsem，对单线程应用似乎开销低得多。然而，阻塞链与静态键的组合可能是性能更好的组合；这里还需要更多研究。)

The following pieces are necessary to hook a certain point in the filesystem:
(挂钩文件系统中的某个特定点需要以下部件：)

- A ``struct xfs_hooks`` object must be embedded in a convenient place such as
  a well-known incore filesystem object.
  (一个 ``struct xfs_hooks`` 对象必须嵌入在一个方便的地方，例如一个众所周知的内存文件系统对象中。)

- Each hook must define an action code and a structure containing more context
  about the action.
  (每个挂钩必须定义一个动作代码，以及一个包含关于该动作更多上下文的结构。)

- Hook providers should provide appropriate wrapper functions and structs
  around the ``xfs_hooks`` and ``xfs_hook`` objects to take advantage of type
  checking to ensure correct usage.
  (挂钩提供者应围绕 ``xfs_hooks`` 和 ``xfs_hook`` 对象提供适当的包装函数和结构，以利用类型检查确保正确使用。)

- A callsite in the regular filesystem code must be chosen to call
  ``xfs_hooks_call`` with the action code and data structure.
  This place should be adjacent to (and not earlier than) the place where
  the filesystem update is committed to the transaction.
  In general, when the filesystem calls a hook chain, it should be able to
  handle sleeping and should not be vulnerable to memory reclaim or locking
  recursion.
  However, the exact requirements are very dependent on the context of the hook
  caller and the callee.
  (必须在常规文件系统代码中选择一个调用点，以动作代码和数据结构调用 ``xfs_hooks_call``。这个位置应当与文件系统更新被提交到事务的位置相邻(且不早于它)。一般来说，当文件系统调用挂钩链时，它应当能够处理睡眠，并且不应容易受到内存回收或锁定递归的影响。不过，确切的要求非常依赖于挂钩调用者和被调用者的上下文。)

- The online fsck function should define a structure to hold scan data, a lock
  to coordinate access to the scan data, and a ``struct xfs_hook`` object.
  The scanner function and the regular filesystem code must acquire resources
  in the same order; see the next section for details.
  (在线 fsck 函数应定义一个保存扫描数据的结构、一个协调对扫描数据访问的锁，以及一个 ``struct xfs_hook`` 对象。扫描器函数和常规文件系统代码必须以相同的顺序获取资源；详情见下一节。)

- The online fsck code must contain a C function to catch the hook action code
  and data structure.
  If the object being updated has already been visited by the scan, then the
  hook information must be applied to the scan data.
  (在线 fsck 代码必须包含一个 C 函数来捕获挂钩的动作代码和数据结构。如果被更新的对象已经被扫描访问过，则挂钩信息必须应用于扫描数据。)

- Prior to unlocking inodes to start the scan, online fsck must call
  ``xfs_hooks_setup`` to initialize the ``struct xfs_hook``, and
  ``xfs_hooks_add`` to enable the hook.
  (在解锁 inode 以启动扫描之前，在线 fsck 必须调用 ``xfs_hooks_setup`` 来初始化 ``struct xfs_hook``，并调用 ``xfs_hooks_add`` 来启用挂钩。)

- Online fsck must call ``xfs_hooks_del`` to disable the hook once the scan is
  complete.
  (扫描完成后，在线 fsck 必须调用 ``xfs_hooks_del`` 来禁用挂钩。)

The number of hooks should be kept to a minimum to reduce complexity.
Static keys are used to reduce the overhead of filesystem hooks to nearly
zero when online fsck is not running.
(挂钩数量应保持在最少，以降低复杂度。静态键用于在在线 fsck 未运行时把文件系统挂钩的开销降低到几乎为零。)

.. _liveupdate:

Live Updates During a Scan
(扫描期间的实时更新)
``````````````````````````

The code paths of the online fsck scanning code and the :ref:`hooked<fshooks>`
filesystem code look like this::
(在线 fsck 扫描代码与 :ref:`被挂钩 <fshooks>` 文件系统代码的代码路径如下所示：)

            other program
                  ↓
            inode lock ←────────────────────┐
                  ↓                         │
            AG header lock                  │
                  ↓                         │
            filesystem function             │
                  ↓                         │
            notifier call chain             │    same
                  ↓                         ├─── inode
            scrub hook function             │    lock
                  ↓                         │
            scan data mutex ←──┐    same    │
                  ↓            ├─── scan    │
            update scan data   │    lock    │
                  ↑            │            │
            scan data mutex ←──┘            │
                  ↑                         │
            inode lock ←────────────────────┘
                  ↑
            scrub function
                  ↑
            inode scanner
                  ↑
            xfs_scrub

These rules must be followed to ensure correct interactions between the
checking code and the code making an update to the filesystem:
(必须遵循以下规则，以确保检查代码与对文件系统进行更新的代码之间的正确交互：)

- Prior to invoking the notifier call chain, the filesystem function being
  hooked must acquire the same lock that the scrub scanning function acquires
  to scan the inode.
  (在调用通知调用链之前，被挂钩的文件系统函数必须获取 scrub 扫描函数为扫描 inode 而获取的同一把锁。)

- The scanning function and the scrub hook function must coordinate access to
  the scan data by acquiring a lock on the scan data.
  (扫描函数和 scrub 挂钩函数必须通过获取扫描数据上的锁来协调对扫描数据的访问。)

- Scrub hook function must not add the live update information to the scan
  observations unless the inode being updated has already been scanned.
  The scan coordinator has a helper predicate (``xchk_iscan_want_live_update``)
  for this.
  (除非被更新的 inode 已经被扫描过，否则 scrub 挂钩函数不得将实时更新信息加入扫描观察结果。扫描协调器为此提供了一个辅助谓词(``xchk_iscan_want_live_update``)。)

- Scrub hook functions must not change the caller's state, including the
  transaction that it is running.
  They must not acquire any resources that might conflict with the filesystem
  function being hooked.
  (scrub 挂钩函数不得改变调用者的状态，包括它正在运行的事务。它们不得获取任何可能与被挂钩文件系统函数冲突的资源。)

- The hook function can abort the inode scan to avoid breaking the other rules.
  (挂钩函数可以中止 inode 扫描，以避免破坏其他规则。)

The inode scan APIs are pretty simple:
(inode 扫描 API 相当简单：)

- ``xchk_iscan_start`` starts a scan
  (``xchk_iscan_start`` 启动一次扫描)

- ``xchk_iscan_iter`` grabs a reference to the next inode in the scan or
  returns zero if there is nothing left to scan
  (``xchk_iscan_iter`` 抓取扫描中下一个 inode 的引用，如果没有剩下可扫描的则返回零)

- ``xchk_iscan_want_live_update`` to decide if an inode has already been
  visited in the scan.
  This is critical for hook functions to decide if they need to update the
  in-memory scan information.
  (``xchk_iscan_want_live_update`` 判断一个 inode 是否已在扫描中被访问过。这对挂钩函数决定是否需要更新内存中的扫描信息至关重要。)

- ``xchk_iscan_mark_visited`` to mark an inode as having been visited in the
  scan
  (``xchk_iscan_mark_visited`` 将一个 inode 标记为已在扫描中被访问过)

- ``xchk_iscan_teardown`` to finish the scan
  (``xchk_iscan_teardown`` 结束扫描)

.. _quotacheck:

Case Study: Quota Counter Checking
(案例研究：配额计数器检查)
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

It is useful to compare the mount time quotacheck code to the online repair
quotacheck code.
Mount time quotacheck does not have to contend with concurrent operations, so
it does the following:
(将挂载时的配额检查代码与在线修复的配额检查代码进行比较是有益的。挂载时的配额检查不必与并发操作竞争，因此它执行以下操作：)

1. Make sure the ondisk dquots are in good enough shape that all the incore
   dquots will actually load, and zero the resource usage counters in the
   ondisk buffer.
   (确保磁盘 dquot 的状态足够好，使所有内存 dquot 都能真正加载，并将磁盘缓冲中的资源使用计数器清零。)

2. Walk every inode in the filesystem.
   Add each file's resource usage to the incore dquot.
   (遍历文件系统中的每个 inode。将每个文件的资源使用加到内存 dquot 上。)

3. Walk each incore dquot.
   If the incore dquot is not being flushed, add the ondisk buffer backing the
   incore dquot to a delayed write (delwri) list.
   (遍历每个内存 dquot。如果内存 dquot 未在刷新中，则将支撑该内存 dquot 的磁盘缓冲加入延迟写(delwri)列表。)

4. Write the buffer list to disk.
   (将缓冲列表写入磁盘。)

Like most online fsck functions, online quotacheck can't write to regular
filesystem objects until the newly collected metadata reflect all filesystem
state.
Therefore, online quotacheck records file resource usage to a shadow dquot
index implemented with a sparse ``xfarray``, and only writes to the real dquots
once the scan is complete.
Handling transactional updates is tricky because quota resource usage updates
are handled in phases to minimize contention on dquots:
(与大多数在线 fsck 函数一样，在线配额检查在新收集的元数据反映全部文件系统状态之前，不能向常规文件系统对象写入。因此，在线配额检查将文件资源使用记录到一个用稀疏 ``xfarray`` 实现的影子 dquot 索引中，只在扫描完成后才写入真正的 dquot。处理事务性更新比较棘手，因为配额资源使用更新分阶段处理以尽量减少对 dquot 的争用：)

1. The inodes involved are joined and locked to a transaction.
   (涉及的 inode 被加入并锁定到一个事务中。)

2. For each dquot attached to the file:
   (对于附着到该文件的每个 dquot：)

   a. The dquot is locked.
      (dquot 被锁定。)

   b. A quota reservation is added to the dquot's resource usage.
      The reservation is recorded in the transaction.
      (一个配额预留被加到 dquot 的资源使用上。该预留被记录在事务中。)

   c. The dquot is unlocked.
      (dquot 被解锁。)

3. Changes in actual quota usage are tracked in the transaction.
   (实际配额使用的变化在事务中被跟踪。)

4. At transaction commit time, each dquot is examined again:
   (在事务提交时，再次检查每个 dquot：)

   a. The dquot is locked again.
      (dquot 再次被锁定。)

   b. Quota usage changes are logged and unused reservation is given back to
      the dquot.
      (记录配额使用的变化，并把未使用的预留归还给 dquot。)

   c. The dquot is unlocked.
      (dquot 被解锁。)

For online quotacheck, hooks are placed in steps 2 and 4.
The step 2 hook creates a shadow version of the transaction dquot context
(``dqtrx``) that operates in a similar manner to the regular code.
The step 4 hook commits the shadow ``dqtrx`` changes to the shadow dquots.
Notice that both hooks are called with the inode locked, which is how the
live update coordinates with the inode scanner.
(对于在线配额检查，挂钩放置在步骤 2 和 4 中。步骤 2 的挂钩创建事务 dquot 上下文(``dqtrx``)的一个影子版本，其运行方式与常规代码类似。步骤 4 的挂钩将影子 ``dqtrx`` 的更改提交给影子 dquot。注意，两个挂钩都是在 inode 被锁定的情况下调用的，这就是实时更新与 inode 扫描器协调的方式。)

The quotacheck scan looks like this:
(配额检查扫描如下所示：)

1. Set up a coordinated inode scan.
   (建立一个协调的 inode 扫描。)

2. For each inode returned by the inode scan iterator:
   (对于 inode 扫描迭代器返回的每个 inode：)

   a. Grab and lock the inode.
      (抓取并锁定该 inode。)

   b. Determine that inode's resource usage (data blocks, inode counts,
      realtime blocks) and add that to the shadow dquots for the user, group,
      and project ids associated with the inode.
      (确定该 inode 的资源使用(数据块、inode 计数、实时块)，并将其加入与该 inode 关联的用户、组和项目 id 的影子 dquot。)

   c. Unlock and release the inode.
      (解锁并释放该 inode。)

3. For each dquot in the system:
   (对于系统中的每个 dquot：)

   a. Grab and lock the dquot.
      (抓取并锁定该 dquot。)

   b. Check the dquot against the shadow dquots created by the scan and updated
      by the live hooks.
      (将该 dquot 与由扫描创建、由实时挂钩更新的影子 dquot 进行核对。)

Live updates are key to being able to walk every quota record without
needing to hold any locks for a long duration.
If repairs are desired, the real and shadow dquots are locked and their
resource counts are set to the values in the shadow dquot.
(实时更新是无需长时间持有任何锁就能遍历每条配额记录的关键。如果需要修复，则锁定真实 dquot 和影子 dquot，并把它们的资源计数设置为影子 dquot 中的值。)

.. _nlinks:

Case Study: File Link Count Checking
(案例研究：文件链接计数检查)
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

File link count checking also uses live update hooks.
The coordinated inode scanner is used to visit all directories on the
filesystem, and per-file link count records are stored in a sparse ``xfarray``
indexed by inumber.
During the scanning phase, each entry in a directory generates observation
data as follows:
(文件链接计数检查也使用实时更新挂钩。协调的 inode 扫描器用于访问文件系统上的所有目录，每文件的链接计数记录存储在一个以 inumber 索引的稀疏 ``xfarray`` 中。在扫描阶段，目录中的每个条目按如下方式生成观察数据：)

1. If the entry is a dotdot (``'..'``) entry of the root directory, the
   directory's parent link count is bumped because the root directory's dotdot
   entry is self referential.
   (如果该条目是根目录的 dotdot(``'..'``)条目，则该目录的父链接计数被递增，因为根目录的 dotdot 条目是自引用的。)

2. If the entry is a dotdot entry of a subdirectory, the parent's backref
   count is bumped.
   (如果该条目是子目录的 dotdot 条目，则父目录的反向引用(backref)计数被递增。)

3. If the entry is neither a dot nor a dotdot entry, the target file's parent
   count is bumped.
   (如果该条目既不是 dot 也不是 dotdot 条目，则目标文件的父计数被递增。)

4. If the target is a subdirectory, the parent's child link count is bumped.
   (如果目标是子目录，则父目录的子链接计数被递增。)

A crucial point to understand about how the link count inode scanner interacts
with the live update hooks is that the scan cursor tracks which *parent*
directories have been scanned.
In other words, the live updates ignore any update about ``A → B`` when A has
not been scanned, even if B has been scanned.
Furthermore, a subdirectory A with a dotdot entry pointing back to B is
accounted as a backref counter in the shadow data for A, since child dotdot
entries affect the parent's link count.
Live update hooks are carefully placed in all parts of the filesystem that
create, change, or remove directory entries, since those operations involve
bumplink and droplink.
(理解链接计数 inode 扫描器如何与实时更新挂钩交互的一个关键点是：扫描游标跟踪哪些*父*目录已被扫描。换句话说，当 A 尚未被扫描时，实时更新会忽略任何关于 ``A → B`` 的更新，即使 B 已被扫描。此外，具有指回 B 的 dotdot 条目的子目录 A，在 A 的影子数据中被计为一个反向引用计数器，因为子 dotdot 条目会影响父目录的链接计数。实时更新挂钩被仔细放置在文件系统中创建、更改或移除目录项的所有部分，因为这些操作涉及 bumplink 和 droplink。)

For any file, the correct link count is the number of parents plus the number
of child subdirectories.
Non-directories never have children of any kind.
The backref information is used to detect inconsistencies in the number of
links pointing to child subdirectories and the number of dotdot entries
pointing back.
(对任何文件来说，正确的链接计数是父目录数量加上子子目录数量。非目录永远不会有任何类型的子项。反向引用信息用于检测指向子子目录的链接数量与指回的 dotdot 条目数量之间的不一致。)

After the scan completes, the link count of each file can be checked by locking
both the inode and the shadow data, and comparing the link counts.
A second coordinated inode scan cursor is used for comparisons.
Live updates are key to being able to walk every inode without needing to hold
any locks between inodes.
If repairs are desired, the inode's link count is set to the value in the
shadow information.
If no parents are found, the file must be :ref:`reparented <orphanage>` to the
orphanage to prevent the file from being lost forever.
(扫描完成后，可以通过同时锁定 inode 和影子数据并比较链接计数来检查每个文件的链接计数。第二个协调的 inode 扫描游标用于比较。实时更新是无需在 inode 之间持有任何锁就能遍历每个 inode 的关键。如果需要修复，inode 的链接计数被设置为影子信息中的值。如果没有找到任何父目录，该文件必须被 :ref:`重新指定父目录 <orphanage>` 到孤儿院(orphanage)，以防文件永远丢失。)

.. _rmap_repair:

Case Study: Rebuilding Reverse Mapping Records
(案例研究：重建反向映射记录)
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

Most repair functions follow the same pattern: lock filesystem resources,
walk the surviving ondisk metadata looking for replacement metadata records,
and use an :ref:`in-memory array <xfarray>` to store the gathered observations.
The primary advantage of this approach is the simplicity and modularity of the
repair code -- code and data are entirely contained within the scrub module,
do not require hooks in the main filesystem, and are usually the most efficient
in memory use.
A secondary advantage of this repair approach is atomicity -- once the kernel
decides a structure is corrupt, no other threads can access the metadata until
the kernel finishes repairing and revalidating the metadata.
(大多数修复函数遵循相同的模式：锁定文件系统资源、遍历幸存的磁盘元数据寻找替换元数据记录，并使用 :ref:`内存数组 <xfarray>` 存储收集到的观察结果。这种方法的主要优势是修复代码的简单性和模块化——代码和数据完全包含在 scrub 模块中，不需要在主文件系统中挂钩，并且通常内存使用效率最高。这种修复方法的次要优势是原子性——一旦内核判定某个结构已损坏，在内核完成修复和重新校验元数据之前，其他线程不能访问该元数据。)

For repairs going on within a shard of the filesystem, these advantages
outweigh the delays inherent in locking the shard while repairing parts of the
shard.
Unfortunately, repairs to the reverse mapping btree cannot use the "standard"
btree repair strategy because it must scan every space mapping of every fork of
every file in the filesystem, and the filesystem cannot stop.
Therefore, rmap repair foregoes atomicity between scrub and repair.
It combines a :ref:`coordinated inode scanner <iscan>`, :ref:`live update hooks
<liveupdate>`, and an :ref:`in-memory rmap btree <xfbtree>` to complete the
scan for reverse mapping records.
(对于在文件系统某个分片内进行的修复，这些优势超过了在修复该分片部分时锁定该分片所固有的延迟。不幸的是，对反向映射 btree 的修复无法使用"标准"btree 修复策略，因为它必须扫描文件系统中每个文件的每个 fork 的每个空间映射，而文件系统不能停止。因此，rmap 修复放弃了 scrub 与修复之间的原子性。它结合 :ref:`协调的 inode 扫描器 <iscan>`、:ref:`实时更新挂钩 <liveupdate>` 和 :ref:`内存中的 rmap btree <xfbtree>` 来完成对反向映射记录的扫描。)

1. Set up an xfbtree to stage rmap records.
   (建立一个 xfbtree 来暂存 rmap 记录。)

2. While holding the locks on the AGI and AGF buffers acquired during the
   scrub, generate reverse mappings for all AG metadata: inodes, btrees, CoW
   staging extents, and the internal log.
   (在持有 scrub 期间获取的 AGI 和 AGF 缓冲锁的同时，为所有 AG 元数据生成反向映射：inode、btree、CoW 暂存区段以及内部日志。)

3. Set up an inode scanner.
   (建立一个 inode 扫描器。)

4. Hook into rmap updates for the AG being repaired so that the live scan data
   can receive updates to the rmap btree from the rest of the filesystem during
   the file scan.
   (挂钩到被修复 AG 的 rmap 更新，使实时扫描数据能够在文件扫描期间接收来自文件系统其余部分的 rmap btree 更新。)

5. For each space mapping found in either fork of each file scanned,
   decide if the mapping matches the AG of interest.
   If so:
   (对于扫描的每个文件在任一 fork 中找到的每个空间映射，判断该映射是否匹配所关注的 AG。如果是：)

   a. Create a btree cursor for the in-memory btree.
      (为内存中的 btree 创建一个 btree 游标。)

   b. Use the rmap code to add the record to the in-memory btree.
      (使用 rmap 代码将该记录添加到内存中的 btree。)

   c. Use the :ref:`special commit function <xfbtree_commit>` to write the
      xfbtree changes to the xfile.
      (使用 :ref:`特殊提交函数 <xfbtree_commit>` 将 xfbtree 更改写入 xfile。)

6. For each live update received via the hook, decide if the owner has already
   been scanned.
   If so, apply the live update into the scan data:
   (对于通过挂钩收到的每条实时更新，判断属主是否已被扫描过。如果是，则将实时更新应用到扫描数据：)

   a. Create a btree cursor for the in-memory btree.
      (为内存中的 btree 创建一个 btree 游标。)

   b. Replay the operation into the in-memory btree.
      (将该操作重放到内存中的 btree。)

   c. Use the :ref:`special commit function <xfbtree_commit>` to write the
      xfbtree changes to the xfile.
      This is performed with an empty transaction to avoid changing the
      caller's state.
      (使用 :ref:`特殊提交函数 <xfbtree_commit>` 将 xfbtree 更改写入 xfile。这用一个空事务执行，以避免改变调用者的状态。)

7. When the inode scan finishes, create a new scrub transaction and relock the
   two AG headers.
   (当 inode 扫描结束后，创建一个新的 scrub 事务并重新锁定两个 AG 头。)

8. Compute the new btree geometry using the number of rmap records in the
   shadow btree, like all other btree rebuilding functions.
   (像所有其他 btree 重建函数一样，使用影子 btree 中 rmap 记录的数量计算新 btree 的几何形状。)

9. Allocate the number of blocks computed in the previous step.
   (分配上一步计算出的块数。)

10. Perform the usual btree bulk loading and commit to install the new rmap
    btree.
    (执行常规的 btree 批量加载和提交，以安装新的 rmap btree。)

11. Reap the old rmap btree blocks as discussed in the case study about how
    to :ref:`reap after rmap btree repair <rmap_reap>`.
    (按关于 :ref:`rmap btree 修复后的回收 <rmap_reap>` 的案例研究所述，回收旧的 rmap btree 块。)

12. Free the xfbtree now that it not needed.
    (既然不再需要，就释放该 xfbtree。)

Staging Repairs with Temporary Files on Disk
(用磁盘上的临时文件暂存修复)
--------------------------------------------

XFS stores a substantial amount of metadata in file forks: directories,
extended attributes, symbolic link targets, free space bitmaps and summary
information for the realtime volume, and quota records.
File forks map 64-bit logical file fork space extents to physical storage space
extents, similar to how a memory management unit maps 64-bit virtual addresses
to physical memory addresses.
Therefore, file-based tree structures (such as directories and extended
attributes) use blocks mapped in the file fork offset address space that point
to other blocks mapped within that same address space, and file-based linear
structures (such as bitmaps and quota records) compute array element offsets in
the file fork offset address space.
(XFS 在文件 fork 中存储了大量元数据：目录、扩展属性、符号链接目标、实时卷的空闲空间位图和摘要信息，以及配额记录。文件 fork 将 64 位逻辑文件 fork 空间区段映射到物理存储空间区段，类似于内存管理单元将 64 位虚拟地址映射到物理内存地址。因此，基于文件的树结构(例如目录和扩展属性)使用映射在文件 fork 偏移地址空间中的块，这些块指向映射在同一地址空间内的其他块；基于文件的线性结构(例如位图和配额记录)则在文件 fork 偏移地址空间中计算数组元素偏移。)

Because file forks can consume as much space as the entire filesystem, repairs
cannot be staged in memory, even when a paging scheme is available.
Therefore, online repair of file-based metadata createas a temporary file in
the XFS filesystem, writes a new structure at the correct offsets into the
temporary file, and atomically exchanges all file fork mappings (and hence the
fork contents) to commit the repair.
Once the repair is complete, the old fork can be reaped as necessary; if the
system goes down during the reap, the iunlink code will delete the blocks
during log recovery.
(由于文件 fork 可能消耗与整个文件系统一样多的空间，即使有分页方案可用，修复也无法在内存中暂存。因此，对基于文件元数据的在线修复会在 XFS 文件系统中创建一个临时文件，将新结构按正确的偏移写入临时文件，并原子地交换所有文件 fork 映射(也就是 fork 内容)以提交修复。修复完成后，可以按需回收旧 fork；如果系统在回收期间宕机，iunlink 代码将在日志恢复期间删除这些块。)

**Note**: All space usage and inode indices in the filesystem *must* be
consistent to use a temporary file safely!
This dependency is the reason why online repair can only use pageable kernel
memory to stage ondisk space usage information.
(**注意**：文件系统中所有空间使用和 inode 索引都*必须*一致，才能安全地使用临时文件！这一依赖关系正是在线修复只能使用可分页内核内存来暂存磁盘空间使用信息的原因。)

Exchanging metadata file mappings with a temporary file requires the owner
field of the block headers to match the file being repaired and not the
temporary file.
The directory, extended attribute, and symbolic link functions were all
modified to allow callers to specify owner numbers explicitly.
(用临时文件交换元数据文件映射要求块头的属主字段与被修复的文件匹配，而不是临时文件。目录、扩展属性和符号链接函数都被修改为允许调用者显式指定属主编号。)

There is a downside to the reaping process -- if the system crashes during the
reap phase and the fork extents are crosslinked, the iunlink processing will
fail because freeing space will find the extra reverse mappings and abort.
(回收过程有一个缺点——如果系统在回收阶段崩溃且 fork 区段是交叉链接的，iunlink 处理将失败，因为释放空间会发现多余的反向映射并中止。)

Temporary files created for repair are similar to ``O_TMPFILE`` files created
by userspace.
They are not linked into a directory and the entire file will be reaped when
the last reference to the file is lost.
The key differences are that these files must have no access permission outside
the kernel at all, they must be specially marked to prevent them from being
opened by handle, and they must never be linked into the directory tree.
(为修复创建的临时文件类似于用户空间创建的 ``O_TMPFILE`` 文件。它们不链接到任何目录，并且当对文件的最后一个引用丢失时整个文件将被回收。关键区别在于：这些文件在内核之外必须完全没有任何访问权限，必须被特殊标记以防止通过句柄打开，并且绝不能被链接到目录树中。)

+--------------------------------------------------------------------------+
| **Historical Sidebar**:                                                  |
| (历史侧栏：)                                                                  |
+--------------------------------------------------------------------------+
| In the initial iteration of file metadata repair, the damaged metadata   |
| blocks would be scanned for salvageable data; the extents in the file    |
| fork would be reaped; and then a new structure would be built in its     |
| place.                                                                   |
| This strategy did not survive the introduction of the atomic repair      |
| requirement expressed earlier in this document.                          |
|                                                                          |
| (在文件元数据修复的最初迭代中，会扫描受损的元数据块以寻找可抢救的数据；回收文件 fork 中的区段；然后在原位置构建新结构。)         |
| (这一策略没有挺过本文前文所述的原子性修复要求的引入。)                                             |
|                                                                          |
| The second iteration explored building a second structure at a high      |
| offset in the fork from the salvage data, reaping the old extents, and   |
| using a ``COLLAPSE_RANGE`` operation to slide the new extents into       |
| place.                                                                   |
|                                                                          |
| (第二次迭代探索了从抢救数据在 fork 中的高偏移处构建第二个结构、回收旧区段，)                               |
| (并使用 ``COLLAPSE_RANGE`` 操作将新区段滑动到位。)                                     |
|                                                                          |
| This had many drawbacks:                                                 |
|                                                                          |
| (这有许多缺点：)                                                                |
|                                                                          |
| - Array structures are linearly addressed, and the regular filesystem    |
|   codebase does not have the concept of a linear offset that could be    |
|   applied to the record offset computation to build an alternate copy.   |
|                                                                          |
| (数组结构是线性寻址的，而常规文件系统代码库没有线性偏移的概念，无法将其应用于记录偏移计算来构建替代副本。)                   |
|                                                                          |
| - Extended attributes are allowed to use the entire attr fork offset     |
|   address space.                                                         |
|                                                                          |
| (扩展属性被允许使用整个 attr fork 偏移地址空间。)                                          |
|                                                                          |
| - Even if repair could build an alternate copy of a data structure in a  |
|   different part of the fork address space, the atomic repair commit     |
|   requirement means that online repair would have to be able to perform  |
|   a log assisted ``COLLAPSE_RANGE`` operation to ensure that the old     |
|   structure was completely replaced.                                     |
|                                                                          |
| (即使修复能够在 fork 地址空间的不同部分构建数据结构的替代副本，)                                     |
| (原子性修复提交的要求意味着在线修复必须能够执行日志辅助的 ``COLLAPSE_RANGE`` 操作，以确保旧结构被完全替换。)        |
|                                                                          |
| - A crash after construction of the secondary tree but before the range  |
|   collapse would leave unreachable blocks in the file fork.              |
|   This would likely confuse things further.                              |
|                                                                          |
| (在构建了二级树之后、范围收缩之前发生崩溃，会在文件 fork 中留下不可达的块。这可能会进一步混淆情况。)                   |
|                                                                          |
| - Reaping blocks after a repair is not a simple operation, and           |
|   initiating a reap operation from a restarted range collapse operation  |
|   during log recovery is daunting.                                       |
|                                                                          |
| (修复后回收块并不是一个简单的操作，而在日志恢复期间从重启的范围收缩操作中发起回收操作令人望而生畏。)                      |
|                                                                          |
| - Directory entry blocks and quota records record the file fork offset   |
|   in the header area of each block.                                      |
|   An atomic range collapse operation would have to rewrite this part of  |
|   each block header.                                                     |
|   Rewriting a single field in block headers is not a huge problem, but   |
|   it's something to be aware of.                                         |
|                                                                          |
| (目录项块和配额记录在每个块的头部区域记录文件 fork 偏移。)                                        |
| (原子范围收缩操作必须重写每个块头的这一部分。重写块头中的单个字段不是什么大问题，但需要注意这一点。)                      |
|                                                                          |
| - Each block in a directory or extended attributes btree index contains  |
|   sibling and child block pointers.                                      |
|   Were the atomic commit to use a range collapse operation, each block   |
|   would have to be rewritten very carefully to preserve the graph        |
|   structure.                                                             |
|   Doing this as part of a range collapse means rewriting a large number  |
|   of blocks repeatedly, which is not conducive to quick repairs.         |
|                                                                          |
| (目录或扩展属性 btree 索引中的每个块都包含兄弟块指针和子块指针。)                                    |
| (如果原子提交使用范围收缩操作，则必须非常小心地重写每个块以保留图结构。)                                    |
| (将此作为范围收缩的一部分意味着反复重写大量块，这不利于快速修复。)                                       |
|                                                                          |
| This lead to the introduction of temporary file staging.                 |
| (这导致了临时文件暂存机制的引入。)                                                       |
+--------------------------------------------------------------------------+

Using a Temporary File
(使用临时文件)
``````````````````````

Online repair code should use the ``xrep_tempfile_create`` function to create a
temporary file inside the filesystem.
This allocates an inode, marks the in-core inode private, and attaches it to
the scrub context.
These files are hidden from userspace, may not be added to the directory tree,
and must be kept private.
(在线修复代码应使用 ``xrep_tempfile_create`` 函数在文件系统内创建一个临时文件。这会分配一个 inode，将内核中 inode 标记为私有，并将其附着到 scrub 上下文。这些文件对用户空间隐藏，不得被加入目录树，并且必须保持私有。)

Temporary files only use two inode locks: the IOLOCK and the ILOCK.
The MMAPLOCK is not needed here, because there must not be page faults from
userspace for data fork blocks.
The usage patterns of these two locks are the same as for any other XFS file --
access to file data are controlled via the IOLOCK, and access to file metadata
are controlled via the ILOCK.
Locking helpers are provided so that the temporary file and its lock state can
be cleaned up by the scrub context.
To comply with the nested locking strategy laid out in the :ref:`inode
locking<ilocking>` section, it is recommended that scrub functions use the
xrep_tempfile_ilock*_nowait lock helpers.
(临时文件只使用两个 inode 锁：IOLOCK 和 ILOCK。这里不需要 MMAPLOCK，因为对数据 fork 块来说绝不能有来自用户空间的缺页。这两个锁的使用模式与任何其他 XFS 文件相同——对文件数据的访问通过 IOLOCK 控制，对文件元数据的访问通过 ILOCK 控制。提供了锁定辅助函数，使临时文件及其锁状态可以由 scrub 上下文清理。为遵守 :ref:`inode 锁定 <ilocking>` 一节中阐述的嵌套锁定策略，建议 scrub 函数使用 xrep_tempfile_ilock*_nowait 锁辅助函数。)

Data can be written to a temporary file by two means:
(可以通过两种方式向临时文件写入数据：)

1. ``xrep_tempfile_copyin`` can be used to set the contents of a regular
   temporary file from an xfile.
   (``xrep_tempfile_copyin`` 可用于从一个 xfile 设置常规临时文件的内容。)

2. The regular directory, symbolic link, and extended attribute functions can
   be used to write to the temporary file.
   (常规的目录、符号链接和扩展属性函数可用于向临时文件写入。)

Once a good copy of a data file has been constructed in a temporary file, it
must be conveyed to the file being repaired, which is the topic of the next
section.
(一旦在临时文件中构建好数据文件的良好副本，就必须把它传递给被修复的文件，这是下一节的主题。)

Logged File Content Exchanges
(日志化的文件内容交换)
-----------------------------

Once repair builds a temporary file with a new data structure written into
it, it must commit the new changes into the existing file.
It is not possible to swap the inumbers of two files, so instead the new
metadata must replace the old.
This suggests the need for the ability to swap extents, but the existing extent
swapping code used by the file defragmenting tool ``xfs_fsr`` is not sufficient
for online repair because:
(一旦修复构建了写入了新数据结构的临时文件，它必须把新更改提交到现有文件中。不可能交换两个文件的 inumber，所以必须让新元数据替换旧元数据。这暗示需要交换区段的能力，但文件碎片整理工具 ``xfs_fsr`` 所用的现有区段交换代码不足以支持在线修复，因为：)

a. When the reverse-mapping btree is enabled, the swap code must keep the
   reverse mapping information up to date with every exchange of mappings.
   Therefore, it can only exchange one mapping per transaction, and each
   transaction is independent.
   (当反向映射 btree 启用时，交换代码必须在每次映射交换时保持反向映射信息最新。因此，它只能在每事务中交换一个映射，并且每个事务是独立的。)

b. Reverse-mapping is critical for the operation of online fsck, so the old
   defragmentation code (which swapped entire extent forks in a single
   operation) is not useful here.
   (反向映射对在线 fsck 的运行至关重要，所以旧的碎片整理代码(它在单个操作中交换整个区段 fork)在这里没有用处。)

c. Defragmentation is assumed to occur between two files with identical
   contents.
   For this use case, an incomplete exchange will not result in a user-visible
   change in file contents, even if the operation is interrupted.
   (碎片整理被假定为发生在两个内容相同的文件之间。对于这一用例，即使操作被中断，不完整的交换也不会导致用户可见的文件内容变化。)

d. Online repair needs to swap the contents of two files that are by definition
   *not* identical.
   For directory and xattr repairs, the user-visible contents might be the
   same, but the contents of individual blocks may be very different.
   (在线修复需要交换定义上*不*相同的两个文件的内容。对于目录和 xattr 修复，用户可见的内容可能相同，但单个块的内容可能非常不同。)

e. Old blocks in the file may be cross-linked with another structure and must
   not reappear if the system goes down mid-repair.
   (文件中的旧块可能与另一个结构交叉链接，如果系统在修复中途宕机，它们绝不能重新出现。)

These problems are overcome by creating a new deferred operation and a new type
of log intent item to track the progress of an operation to exchange two file
ranges.
The new exchange operation type chains together the same transactions used by
the reverse-mapping extent swap code, but records intermedia progress in the
log so that operations can be restarted after a crash.
This new functionality is called the file contents exchange (xfs_exchrange)
code.
The underlying implementation exchanges file fork mappings (xfs_exchmaps).
The new log item records the progress of the exchange to ensure that once an
exchange begins, it will always run to completion, even there are
interruptions.
The new ``XFS_SB_FEAT_INCOMPAT_EXCHRANGE`` incompatible feature flag
in the superblock protects these new log item records from being replayed on
old kernels.
(这些问题通过创建一个新的延迟操作和一种新类型的日志意图项来克服，以跟踪交换两个文件范围的操作的进度。新的交换操作类型把反向映射区段交换代码所用的相同事务链接在一起，但在日志中记录中间进度，使操作可以在崩溃后重启。这一新功能称为文件内容交换(xfs_exchrange)代码。底层实现交换文件 fork 映射(xfs_exchmaps)。新的日志项记录交换的进度，确保一旦交换开始，它将始终运行到完成，即使有中断。超级块中新的 ``XFS_SB_FEAT_INCOMPAT_EXCHRANGE`` 不兼容特性标志保护这些新日志项记录不会被旧内核重放。)

+--------------------------------------------------------------------------+
| **Sidebar: Using Log-Incompatible Feature Flags**                        |
| (侧栏：使用日志不兼容特性标志)                                                         |
+--------------------------------------------------------------------------+
| Starting with XFS v5, the superblock contains a                          |
| ``sb_features_log_incompat`` field to indicate that the log contains     |
| records that might not readable by all kernels that could mount this     |
| filesystem.                                                              |
| In short, log incompat features protect the log contents against kernels |
| that will not understand the contents.                                   |
| Unlike the other superblock feature bits, log incompat bits are          |
| ephemeral because an empty (clean) log does not need protection.         |
| The log cleans itself after its contents have been committed into the    |
| filesystem, either as part of an unmount or because the system is        |
| otherwise idle.                                                          |
| Because upper level code can be working on a transaction at the same     |
| time that the log cleans itself, it is necessary for upper level code to |
| communicate to the log when it is going to use a log incompatible        |
| feature.                                                                 |
|                                                                          |
| (从 XFS v5 开始，超级块包含一个 ``sb_features_log_incompat`` 字段，)                   |
| (用于指示日志包含可能无法被所有能够挂载此文件系统的内核读取的记录。)                                      |
| (简而言之，日志不兼容特性保护日志内容免受无法理解这些内容的内核的影响。)                                    |
| (与其他超级块特性位不同，日志不兼容位是暂态的，因为空(干净)的日志不需要保护。)                                |
| (在其内容被提交到文件系统之后——无论是作为卸载的一部分还是因为系统空闲——日志会自行清理。)                          |
| (由于上层代码可能在日志自行清理的同时正在处理一个事务，因此上层代码在将要使用日志不兼容特性时必须与日志沟通。)                 |
|                                                                          |
| The log coordinates access to incompatible features through the use of   |
| one ``struct rw_semaphore`` for each feature.                            |
| The log cleaning code tries to take this rwsem in exclusive mode to      |
| clear the bit; if the lock attempt fails, the feature bit remains set.   |
| The code supporting a log incompat feature should create wrapper         |
| functions to obtain the log feature and call                             |
| ``xfs_add_incompat_log_feature`` to set the feature bits in the primary  |
| superblock.                                                              |
| The superblock update is performed transactionally, so the wrapper to    |
| obtain log assistance must be called just prior to the creation of the   |
| transaction that uses the functionality.                                 |
| For a file operation, this step must happen after taking the IOLOCK      |
| and the MMAPLOCK, but before allocating the transaction.                 |
| When the transaction is complete, the ``xlog_drop_incompat_feat``        |
| function is called to release the feature.                               |
| The feature bit will not be cleared from the superblock until the log    |
| becomes clean.                                                           |
|                                                                          |
| (日志通过为每个特性使用一个 ``struct rw_semaphore`` 来协调对不兼容特性的访问。)                    |
| (日志清理代码尝试以独占模式获取该 rwsem 以清除该位；如果加锁尝试失败，特性位保持置位。)                         |
| (支持日志不兼容特性的代码应创建包装函数来获取日志特性，)                                            |
| (并调用 ``xfs_add_incompat_log_feature`` 在主超级块中设置特性位。)                      |
| (超级块更新以事务方式执行，因此获取日志辅助的包装函数必须在创建使用该功能的事务之前调用。)                           |
| (对于文件操作，此步骤必须在获取 IOLOCK 和 MMAPLOCK 之后、分配事务之前进行。)                         |
| (当事务完成时，调用 ``xlog_drop_incompat_feat`` 函数来释放该特性。在日志变干净之前，该特性位不会从超级块中清除。) |
|                                                                          |
| Log-assisted extended attribute updates and file content exchanges bothe |
| use log incompat features and provide convenience wrappers around the    |
| functionality.                                                           |
| (日志辅助的扩展属性更新和文件内容交换都使用日志不兼容特性，并为该功能提供了便利的包装函数。)                          |
+--------------------------------------------------------------------------+

Mechanics of a Logged File Content Exchange
(日志化文件内容交换的机制)
```````````````````````````````````````````

Exchanging contents between file forks is a complex task.
The goal is to exchange all file fork mappings between two file fork offset
ranges.
There are likely to be many extent mappings in each fork, and the edges of
the mappings aren't necessarily aligned.
Furthermore, there may be other updates that need to happen after the exchange,
such as exchanging file sizes, inode flags, or conversion of fork data to local
format.
This is roughly the format of the new deferred exchange-mapping work item:
(在文件 fork 之间交换内容是一项复杂的任务。目标是交换两个文件 fork 偏移范围之间的所有文件 fork 映射。每个 fork 中可能有许多区段映射，并且映射的边界不一定对齐。此外，交换之后可能还有其他需要进行的更新，例如交换文件大小、inode 标志，或将 fork 数据转换为本地格式。这大致就是新的延迟交换映射工作项的格式：)

.. code-block:: c

	struct xfs_exchmaps_intent {
	    /* Inodes participating in the operation. */
	    struct xfs_inode    *xmi_ip1;
	    struct xfs_inode    *xmi_ip2;

	    /* File offset range information. */
	    xfs_fileoff_t       xmi_startoff1;
	    xfs_fileoff_t       xmi_startoff2;
	    xfs_filblks_t       xmi_blockcount;

	    /* Set these file sizes after the operation, unless negative. */
	    xfs_fsize_t         xmi_isize1;
	    xfs_fsize_t         xmi_isize2;

	    /* XFS_EXCHMAPS_* log operation flags */
	    uint64_t            xmi_flags;
	};

The new log intent item contains enough information to track two logical fork
offset ranges: ``(inode1, startoff1, blockcount)`` and ``(inode2, startoff2,
blockcount)``.
Each step of an exchange operation exchanges the largest file range mapping
possible from one file to the other.
After each step in the exchange operation, the two startoff fields are
incremented and the blockcount field is decremented to reflect the progress
made.
The flags field captures behavioral parameters such as exchanging attr fork
mappings instead of the data fork and other work to be done after the exchange.
The two isize fields are used to exchange the file sizes at the end of the
operation if the file data fork is the target of the operation.
(新的日志意图项包含足够的信息来跟踪两个逻辑 fork 偏移范围：``(inode1, startoff1, blockcount)`` 和 ``(inode2, startoff2, blockcount)``。交换操作的每一步都会交换从一个文件到另一个文件的最大可能文件范围映射。在交换操作的每一步之后，两个 startoff 字段被递增，blockcount 字段被递减，以反映已完成的进度。flags 字段捕获行为参数，例如交换属性 fork 映射而不是数据 fork，以及交换之后要做的其他工作。如果文件数据 fork 是操作的目标，两个 isize 字段用于在操作结束时交换文件大小。)

When the exchange is initiated, the sequence of operations is as follows:
(当交换被发起时，操作序列如下：)

1. Create a deferred work item for the file mapping exchange.
   At the start, it should contain the entirety of the file block ranges to be
   exchanged.
   (为文件映射交换创建一个延迟工作项。开始时，它应包含要交换的文件块范围的全部。)

2. Call ``xfs_defer_finish`` to process the exchange.
   This is encapsulated in ``xrep_tempexch_contents`` for scrub operations.
   This will log an extent swap intent item to the transaction for the deferred
   mapping exchange work item.
   (调用 ``xfs_defer_finish`` 处理交换。对 scrub 操作而言，它被封装在 ``xrep_tempexch_contents`` 中。这会向事务记录一个区段交换意图项，对应于延迟映射交换工作项。)

3. Until ``xmi_blockcount`` of the deferred mapping exchange work item is zero,
   (直到延迟映射交换工作项的 ``xmi_blockcount`` 为零：)

   a. Read the block maps of both file ranges starting at ``xmi_startoff1`` and
      ``xmi_startoff2``, respectively, and compute the longest extent that can
      be exchanged in a single step.
      This is the minimum of the two ``br_blockcount`` s in the mappings.
      Keep advancing through the file forks until at least one of the mappings
      contains written blocks.
      Mutual holes, unwritten extents, and extent mappings to the same physical
      space are not exchanged.

      For the next few steps, this document will refer to the mapping that came
      from file 1 as "map1", and the mapping that came from file 2 as "map2".
      (分别读取从 ``xmi_startoff1`` 和 ``xmi_startoff2`` 开始的两个文件范围的块映射，并计算可以在单步中交换的最长区段。这是两个映射中 ``br_blockcount`` 的较小值。持续在文件 fork 中推进，直到至少一个映射包含已写入的块。互相为洞、未写入区段以及指向相同物理空间的区段映射不予交换。

      在接下来的几步中，本文档把来自文件 1 的映射称为 "map1"，把来自文件 2 的映射称为 "map2"。)

   b. Create a deferred block mapping update to unmap map1 from file 1.
      (创建一个延迟块映射更新，从文件 1 取消映射 map1。)

   c. Create a deferred block mapping update to unmap map2 from file 2.
      (创建一个延迟块映射更新，从文件 2 取消映射 map2。)

   d. Create a deferred block mapping update to map map1 into file 2.
      (创建一个延迟块映射更新，把 map1 映射进文件 2。)

   e. Create a deferred block mapping update to map map2 into file 1.
      (创建一个延迟块映射更新，把 map2 映射进文件 1。)

   f. Log the block, quota, and extent count updates for both files.
      (记录两个文件的块、配额和区段计数更新。)

   g. Extend the ondisk size of either file if necessary.
      (如有必要，扩展任一文件的磁盘大小。)

   h. Log a mapping exchange done log item for th mapping exchange intent log
      item that was read at the start of step 3.
      (为在步骤 3 开始时读取的映射交换意图日志项记录一个映射交换完成日志项。)

   i. Compute the amount of file range that has just been covered.
      This quantity is ``(map1.br_startoff + map1.br_blockcount -
      xmi_startoff1)``, because step 3a could have skipped holes.
      (计算刚刚覆盖的文件范围量。这个量是 ``(map1.br_startoff + map1.br_blockcount - xmi_startoff1)``，因为步骤 3a 可能跳过了洞。)

   j. Increase the starting offsets of ``xmi_startoff1`` and ``xmi_startoff2``
      by the number of blocks computed in the previous step, and decrease
      ``xmi_blockcount`` by the same quantity.
      This advances the cursor.
      (将 ``xmi_startoff1`` 和 ``xmi_startoff2`` 的起始偏移增加上一步计算出的块数，并将 ``xmi_blockcount`` 减去相同的量。这推进了游标。)

   k. Log a new mapping exchange intent log item reflecting the advanced state
      of the work item.
      (记录一个新的映射交换意图日志项，反映工作项推进后的状态。)

   l. Return the proper error code (EAGAIN) to the deferred operation manager
      to inform it that there is more work to be done.
      The operation manager completes the deferred work in steps 3b-3e before
      moving back to the start of step 3.
      (向延迟操作管理器返回正确的错误码(EAGAIN)，通知它还有更多工作要做。操作管理器在回到步骤 3 的开始之前，先完成步骤 3b-3e 中的延迟工作。)

4. Perform any post-processing.
   This will be discussed in more detail in subsequent sections.
   (执行任何后处理。这将在后续章节中更详细地讨论。)

If the filesystem goes down in the middle of an operation, log recovery will
find the most recent unfinished mapping exchange log intent item and restart
from there.
This is how atomic file mapping exchanges guarantees that an outside observer
will either see the old broken structure or the new one, and never a mismash of
both.
(如果文件系统在操作中途宕机，日志恢复将找到最近的未完成的映射交换日志意图项并从那里重启。原子文件映射交换就是这样保证外部观察者要么看到旧的损坏结构，要么看到新结构，而绝不会看到两者的混搭。)

Preparation for File Content Exchanges
(文件内容交换的准备)
``````````````````````````````````````

There are a few things that need to be taken care of before initiating an
atomic file mapping exchange operation.
First, regular files require the page cache to be flushed to disk before the
operation begins, and directio writes to be quiesced.
Like any filesystem operation, file mapping exchanges must determine the
maximum amount of disk space and quota that can be consumed on behalf of both
files in the operation, and reserve that quantity of resources to avoid an
unrecoverable out of space failure once it starts dirtying metadata.
The preparation step scans the ranges of both files to estimate:
(在发起原子文件映射交换操作之前，有几点需要处理。首先，常规文件要求在操作开始之前将页缓存刷新到磁盘，并静默 directio 写入。与任何文件系统操作一样，文件映射交换必须确定为操作中两个文件可以消耗的最大磁盘空间和配额，并预留该数量的资源，以避免一旦开始弄脏元数据就发生无法挽回的空间耗尽失败。准备步骤扫描两个文件的范围以估计：)

- Data device blocks needed to handle the repeated updates to the fork
  mappings.
  (处理 fork 映射反复更新所需的数据设备块。)
- Change in data and realtime block counts for both files.
  (两个文件的数据和实时块计数的变化。)
- Increase in quota usage for both files, if the two files do not share the
  same set of quota ids.
  (如果两个文件不共享同一组配额 id，两个文件的配额使用增加量。)
- The number of extent mappings that will be added to each file.
  (将添加到每个文件的区段映射数量。)
- Whether or not there are partially written realtime extents.
  User programs must never be able to access a realtime file extent that maps
  to different extents on the realtime volume, which could happen if the
  operation fails to run to completion.
  (是否存在部分写入的实时区段。用户程序绝不能访问映射到实时卷上不同区段的实时文件区段——如果操作未能运行到完成就可能发生这种情况。)

The need for precise estimation increases the run time of the exchange
operation, but it is very important to maintain correct accounting.
The filesystem must not run completely out of free space, nor can the mapping
exchange ever add more extent mappings to a fork than it can support.
Regular users are required to abide the quota limits, though metadata repairs
may exceed quota to resolve inconsistent metadata elsewhere.
(精确估计的需要增加了交换操作的运行时间，但保持正确的记账非常重要。文件系统绝不能完全耗尽空闲空间，映射交换也绝不能向 fork 添加超出其支持能力的区段映射。要求常规用户遵守配额限制，不过元数据修复可以超过配额以解决别处不一致的元数据。)

Special Features for Exchanging Metadata File Contents
(交换元数据文件内容的特殊功能)
``````````````````````````````````````````````````````

Extended attributes, symbolic links, and directories can set the fork format to
"local" and treat the fork as a literal area for data storage.
Metadata repairs must take extra steps to support these cases:
(扩展属性、符号链接和目录可以把 fork 格式设为 "local"，并将 fork 当作存储数据的字面区域。元数据修复必须采取额外步骤来支持这些情况：)

- If both forks are in local format and the fork areas are large enough, the
  exchange is performed by copying the incore fork contents, logging both
  forks, and committing.
  The atomic file mapping exchange mechanism is not necessary, since this can
  be done with a single transaction.
  (如果两个 fork 都是本地格式且 fork 区域足够大，则通过复制内存中的 fork 内容、记录两个 fork 并提交来执行交换。原子文件映射交换机制不是必需的，因为这可以用单个事务完成。)

- If both forks map blocks, then the regular atomic file mapping exchange is
  used.
  (如果两个 fork 都映射块，则使用常规的原子文件映射交换。)

- Otherwise, only one fork is in local format.
  The contents of the local format fork are converted to a block to perform the
  exchange.
  The conversion to block format must be done in the same transaction that
  logs the initial mapping exchange intent log item.
  The regular atomic mapping exchange is used to exchange the metadata file
  mappings.
  Special flags are set on the exchange operation so that the transaction can
  be rolled one more time to convert the second file's fork back to local
  format so that the second file will be ready to go as soon as the ILOCK is
  dropped.
  (否则，只有一个 fork 是本地格式。将本地格式 fork 的内容转换为一个块来执行交换。到块格式的转换必须在记录初始映射交换意图日志项的同一事务中完成。使用常规的原子映射交换来交换元数据文件映射。在交换操作上设置特殊标志，使事务可以再滚动一次，把第二个文件的 fork 转回本地格式，使第二个文件在 ILOCK 一放下就绪可用。)

Extended attributes and directories stamp the owning inode into every block,
but the buffer verifiers do not actually check the inode number!
Although there is no verification, it is still important to maintain
referential integrity, so prior to performing the mapping exchange, online
repair builds every block in the new data structure with the owner field of the
file being repaired.
(扩展属性和目录把拥有它的 inode 盖章进每个块，但缓冲校验器实际上并不检查 inode 编号！尽管没有校验，维护引用完整性仍然重要，所以在执行映射交换之前，在线修复会用被修复文件的属主字段构建新数据结构中的每个块。)

After a successful exchange operation, the repair operation must reap the old
fork blocks by processing each fork mapping through the standard :ref:`file
extent reaping <reaping>` mechanism that is done post-repair.
If the filesystem should go down during the reap part of the repair, the
iunlink processing at the end of recovery will free both the temporary file and
whatever blocks were not reaped.
However, this iunlink processing omits the cross-link detection of online
repair, and is not completely foolproof.
(交换操作成功之后，修复操作必须通过修复后执行的标准 :ref:`文件区段回收 <reaping>` 机制处理每个 fork 映射来回收旧 fork 块。如果文件系统在修复的回收部分宕机，恢复末尾的 iunlink 处理将同时释放临时文件和所有未被回收的块。不过，这一 iunlink 处理省略了在线修复的交叉链接检测，并非完全万无一失。)

Exchanging Temporary File Contents
(交换临时文件内容)
``````````````````````````````````

To repair a metadata file, online repair proceeds as follows:
(要修复一个元数据文件，在线修复按如下步骤进行：)

1. Create a temporary repair file.
   (创建一个临时修复文件。)

2. Use the staging data to write out new contents into the temporary repair
   file.
   The same fork must be written to as is being repaired.
   (使用暂存数据把新内容写入临时修复文件。必须写入与被修复相同的 fork。)

3. Commit the scrub transaction, since the exchange resource estimation step
   must be completed before transaction reservations are made.
   (提交 scrub 事务，因为交换资源估计步骤必须在事务预留进行之前完成。)

4. Call ``xrep_tempexch_trans_alloc`` to allocate a new scrub transaction with
   the appropriate resource reservations, locks, and fill out a ``struct
   xfs_exchmaps_req`` with the details of the exchange operation.
   (调用 ``xrep_tempexch_trans_alloc`` 分配一个带适当资源预留和锁的新 scrub 事务，并用交换操作的细节填写 ``struct xfs_exchmaps_req``。)

5. Call ``xrep_tempexch_contents`` to exchange the contents.
   (调用 ``xrep_tempexch_contents`` 交换内容。)

6. Commit the transaction to complete the repair.
   (提交事务以完成修复。)

.. _rtsummary:

Case Study: Repairing the Realtime Summary File
(案例研究：修复实时摘要文件)
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

In the "realtime" section of an XFS filesystem, free space is tracked via a
bitmap, similar to Unix FFS.
Each bit in the bitmap represents one realtime extent, which is a multiple of
the filesystem block size between 4KiB and 1GiB in size.
The realtime summary file indexes the number of free extents of a given size to
the offset of the block within the realtime free space bitmap where those free
extents begin.
In other words, the summary file helps the allocator find free extents by
length, similar to what the free space by count (cntbt) btree does for the data
section.
(在 XFS 文件系统的"实时"区，空闲空间通过位图跟踪，类似于 Unix FFS。位图中的每一位代表一个实时区段，其大小是文件系统块大小的倍数，介于 4KiB 到 1GiB 之间。实时摘要文件将给定大小的空闲区段数量索引到实时空闲空间位图中这些空闲区段开始处的块偏移。换句话说，摘要文件帮助分配器按长度查找空闲区段，类似于按计数索引的空闲空间(cntbt)btree 为数据区所做的工作。)

The summary file itself is a flat file (with no block headers or checksums!)
partitioned into ``log2(total rt extents)`` sections containing enough 32-bit
counters to match the number of blocks in the rt bitmap.
Each counter records the number of free extents that start in that bitmap block
and can satisfy a power-of-two allocation request.
(摘要文件本身是一个平面文件(没有块头或校验和！)，被划分为 ``log2(total rt extents)`` 个区段，包含足够的 32 位计数器以匹配 rt 位图中的块数。每个计数器记录从那个位图块开始的、能满足二的幂次分配请求的空闲区段数量。)

To check the summary file against the bitmap:
(对照位图检查摘要文件：)

1. Take the ILOCK of both the realtime bitmap and summary files.
   (获取实时位图和摘要文件两者的 ILOCK。)

2. For each free space extent recorded in the bitmap:
   (对于位图中记录的每个空闲空间区段：)

   a. Compute the position in the summary file that contains a counter that
      represents this free extent.
      (计算摘要文件中包含代表此空闲区段计数器的位置。)

   b. Read the counter from the xfile.
      (从 xfile 读取该计数器。)

   c. Increment it, and write it back to the xfile.
      (递增它，并写回 xfile。)

3. Compare the contents of the xfile against the ondisk file.
   (将 xfile 的内容与磁盘文件进行比较。)

To repair the summary file, write the xfile contents into the temporary file
and use atomic mapping exchange to commit the new contents.
The temporary file is then reaped.
(要修复摘要文件，将 xfile 内容写入临时文件，并使用原子映射交换提交新内容。然后回收该临时文件。)

Case Study: Salvaging Extended Attributes
(案例研究：抢救扩展属性)
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

In XFS, extended attributes are implemented as a namespaced name-value store.
Values are limited in size to 64KiB, but there is no limit in the number of
names.
The attribute fork is unpartitioned, which means that the root of the attribute
structure is always in logical block zero, but attribute leaf blocks, dabtree
index blocks, and remote value blocks are intermixed.
Attribute leaf blocks contain variable-sized records that associate
user-provided names with the user-provided values.
Values larger than a block are allocated separate extents and written there.
If the leaf information expands beyond a single block, a directory/attribute
btree (``dabtree``) is created to map hashes of attribute names to entries
for fast lookup.
(在 XFS 中，扩展属性实现为带命名空间的名称-值存储。值的大小限制为 64KiB，但名称数量没有限制。属性 fork 不分区，这意味着属性结构的根总是在逻辑块 0，但属性叶子块、dabtree 索引块和远程值块混在一起。属性叶子块包含可变大小的记录，将用户提供的名称与用户提供的值关联起来。大于一个块的值分配单独的区段并写入那里。如果叶子信息扩展超过单个块，则创建一个目录/属性 btree(``dabtree``)来将属性名称的哈希映射到条目以便快速查找。)

Salvaging extended attributes is done as follows:
(抢救扩展属性按如下方式进行：)

1. Walk the attr fork mappings of the file being repaired to find the attribute
   leaf blocks.
   When one is found,
   (遍历被修复文件的 attr fork 映射以查找属性叶子块。找到一个时，)

   a. Walk the attr leaf block to find candidate keys.
      When one is found,
      (遍历 attr 叶子块以查找候选键。找到一个时，)

      1. Check the name for problems, and ignore the name if there are.
         (检查名称是否有问题，如果有则忽略该名称。)

      2. Retrieve the value.
         If that succeeds, add the name and value to the staging xfarray and
         xfblob.
         (取回值。如果成功，将名称和值添加到暂存的 xfarray 和 xfblob。)

2. If the memory usage of the xfarray and xfblob exceed a certain amount of
   memory or there are no more attr fork blocks to examine, unlock the file and
   add the staged extended attributes to the temporary file.
   (如果 xfarray 和 xfblob 的内存使用超过一定量，或者没有更多 attr fork 块可检查，则解锁文件，并将暂存的扩展属性添加到临时文件。)

3. Use atomic file mapping exchange to exchange the new and old extended
   attribute structures.
   The old attribute blocks are now attached to the temporary file.
   (使用原子文件映射交换来交换新旧扩展属性结构。旧的属性块现在附着到临时文件上。)

4. Reap the temporary file.
   (回收临时文件。)

Fixing Directories
(修复目录)
------------------

Fixing directories is difficult with currently available filesystem features,
since directory entries are not redundant.
The offline repair tool scans all inodes to find files with nonzero link count,
and then it scans all directories to establish parentage of those linked files.
Damaged files and directories are zapped, and files with no parent are
moved to the ``/lost+found`` directory.
It does not try to salvage anything.
(用当前可用的文件系统特性来修复目录很困难，因为目录项没有冗余。离线修复工具扫描所有 inode 以查找链接计数非零的文件，然后扫描所有目录以确定这些已链接文件的父子关系。受损的文件和目录被清除，没有父目录的文件被移动到 ``/lost+found`` 目录。它不尝试抢救任何东西。)

The best that online repair can do at this time is to read directory data
blocks and salvage any dirents that look plausible, correct link counts, and
move orphans back into the directory tree.
The salvage process is discussed in the case study at the end of this section.
The :ref:`file link count fsck <nlinks>` code takes care of fixing link counts
and moving orphans to the ``/lost+found`` directory.
(在线修复目前能做的最好的事情是：读取目录数据块，抢救任何看起来合理的 dirent，纠正链接计数，并把孤儿移回目录树。抢救过程在本节末尾的案例研究中讨论。:ref:`文件链接计数 fsck <nlinks>` 代码负责修复链接计数并把孤儿移到 ``/lost+found`` 目录。)

Case Study: Salvaging Directories
(案例研究：抢救目录)
`````````````````````````````````

Unlike extended attributes, directory blocks are all the same size, so
salvaging directories is straightforward:
(与扩展属性不同，目录块的大小都相同，因此抢救目录很直接：)

1. Find the parent of the directory.
   If the dotdot entry is not unreadable, try to confirm that the alleged
   parent has a child entry pointing back to the directory being repaired.
   Otherwise, walk the filesystem to find it.
   (找到该目录的父目录。如果 dotdot 条目并非不可读，尝试确认所称父目录具有指回被修复目录的子条目。否则，遍历文件系统来找到它。)

2. Walk the first partition of data fork of the directory to find the directory
   entry data blocks.
   When one is found,
   (遍历目录数据 fork 的第一分区以查找目录项数据块。找到一个时，)

   a. Walk the directory data block to find candidate entries.
      When an entry is found:
      (遍历目录数据块以查找候选条目。找到一个条目时：)

      i. Check the name for problems, and ignore the name if there are.
         (检查名称是否有问题，如果有则忽略该名称。)

      ii. Retrieve the inumber and grab the inode.
          If that succeeds, add the name, inode number, and file type to the
          staging xfarray and xblob.
          (取回 inumber 并抓取 inode。如果成功，将名称、inode 编号和文件类型添加到暂存的 xfarray 和 xblob。)

3. If the memory usage of the xfarray and xfblob exceed a certain amount of
   memory or there are no more directory data blocks to examine, unlock the
   directory and add the staged dirents into the temporary directory.
   Truncate the staging files.
   (如果 xfarray 和 xfblob 的内存使用超过一定量，或者没有更多目录数据块可检查，则解锁目录，并将暂存的 dirent 加入临时目录。截断暂存文件。)

4. Use atomic file mapping exchange to exchange the new and old directory
   structures.
   The old directory blocks are now attached to the temporary file.
   (使用原子文件映射交换来交换新旧目录结构。旧的目录块现在附着到临时文件上。)

5. Reap the temporary file.
   (回收临时文件。)

**Future Work Question**: Should repair revalidate the dentry cache when
rebuilding a directory?
(**未来工作问题**：重建目录时修复是否应重新校验 dentry 缓存？)

*Answer*: Yes, it should.
(*回答*：是的，应该。)

In theory it is necessary to scan all dentry cache entries for a directory to
ensure that one of the following apply:
(理论上，有必要扫描一个目录的所有 dentry 缓存条目，以确保以下情况之一成立：)

1. The cached dentry reflects an ondisk dirent in the new directory.
   (缓存的 dentry 反映新目录中的一个磁盘 dirent。)

2. The cached dentry no longer has a corresponding ondisk dirent in the new
   directory and the dentry can be purged from the cache.
   (缓存的 dentry 在新目录中不再有对应的磁盘 dirent，并且该 dentry 可以从缓存中清除。)

3. The cached dentry no longer has an ondisk dirent but the dentry cannot be
   purged.
   This is the problem case.
   (缓存的 dentry 不再有磁盘 dirent，但该 dentry 无法清除。这是问题情形。)

Unfortunately, the current dentry cache design doesn't provide a means to walk
every child dentry of a specific directory, which makes this a hard problem.
There is no known solution.
(不幸的是，当前的 dentry 缓存设计没有提供遍历特定目录的每个子 dentry 的手段，这使它成为一个难题。目前没有已知的解决方案。)

Parent Pointers
(父指针)
```````````````

A parent pointer is a piece of file metadata that enables a user to locate the
file's parent directory without having to traverse the directory tree from the
root.
Without them, reconstruction of directory trees is hindered in much the same
way that the historic lack of reverse space mapping information once hindered
reconstruction of filesystem space metadata.
The parent pointer feature, however, makes total directory reconstruction
possible.
(父指针是一块文件元数据，使用户无需从根开始遍历目录树就能定位文件的父目录。没有它们，目录树的重建会受到阻碍，就像历史上缺乏反向空间映射信息曾一度阻碍文件系统空间元数据的重建一样。不过，父指针特性使完全的目录重建成为可能。)

XFS parent pointers contain the information needed to identify the
corresponding directory entry in the parent directory.
In other words, child files use extended attributes to store pointers to
parents in the form ``(dirent_name) → (parent_inum, parent_gen)``.
The directory checking process can be strengthened to ensure that the target of
each dirent also contains a parent pointer pointing back to the dirent.
Likewise, each parent pointer can be checked by ensuring that the target of
each parent pointer is a directory and that it contains a dirent matching
the parent pointer.
Both online and offline repair can use this strategy.
(XFS 父指针包含标识父目录中对应目录项所需的信息。换句话说，子文件使用扩展属性以 ``(dirent_name) → (parent_inum, parent_gen)`` 的形式存储指向父目录的指针。目录检查过程可以加强，以确保每个 dirent 的目标也包含指回该 dirent 的父指针。同样，每个父指针可以通过确保其目标是一个目录、且该目录包含与该父指针匹配的 dirent 来检查。在线和离线修复都可以使用这一策略。)

+--------------------------------------------------------------------------+
| **Historical Sidebar**:                                                  |
| (历史侧栏：)                                                                  |
+--------------------------------------------------------------------------+
| Directory parent pointers were first proposed as an XFS feature more     |
| than a decade ago by SGI.                                                |
| Each link from a parent directory to a child file is mirrored with an    |
| extended attribute in the child that could be used to identify the       |
| parent directory.                                                        |
| Unfortunately, this early implementation had major shortcomings and was  |
| never merged into Linux XFS:                                             |
|                                                                          |
| (目录父指针早在十多年前就由 SGI 作为 XFS 特性提出。)                                         |
| (从父目录到子文件的每个链接都在子文件中用一个扩展属性镜像，该属性可用于标识父目录。)                              |
| (不幸的是，这个早期实现存在重大缺陷，从未被合并进 Linux XFS：)                                    |
|                                                                          |
| 1. The XFS codebase of the late 2000s did not have the infrastructure to |
|    enforce strong referential integrity in the directory tree.           |
|    It did not guarantee that a change in a forward link would always be  |
|    followed up with the corresponding change to the reverse links.       |
|                                                                          |
| (2000 年代后期的 XFS 代码库没有在目录树中强制执行强引用完整性的基础设施。)                              |
| (它不保证正向链接的更改总是伴随着对反向链接的相应更改。)                                            |
|                                                                          |
| 2. Referential integrity was not integrated into offline repair.         |
|    Checking and repairs were performed on mounted filesystems without    |
|    taking any kernel or inode locks to coordinate access.                |
|    It is not clear how this actually worked properly.                    |
|                                                                          |
| (引用完整性没有集成到离线修复中。)                                                       |
| (检查和修复在已挂载的文件系统上执行，而不获取任何内核锁或 inode 锁来协调访问。)                             |
| (目前尚不清楚这实际上是如何正常工作的。)                                                    |
|                                                                          |
| 3. The extended attribute did not record the name of the directory entry |
|    in the parent, so the SGI parent pointer implementation cannot be     |
|    used to reconnect the directory tree.                                 |
|                                                                          |
| (该扩展属性没有在父目录中记录目录项的名称，因此 SGI 父指针实现无法用于重新连接目录树。)                          |
|                                                                          |
| 4. Extended attribute forks only support 65,536 extents, which means     |
|    that parent pointer attribute creation is likely to fail at some      |
|    point before the maximum file link count is achieved.                 |
|                                                                          |
| (扩展属性 fork 只支持 65,536 个区段，这意味着在达到最大文件链接数之前，父指针属性创建可能会在某些时候失败。)           |
|                                                                          |
| The original parent pointer design was too unstable for something like   |
| a file system repair to depend on.                                       |
| Allison Henderson, Chandan Babu, and Catherine Hoang are working on a    |
| second implementation that solves all shortcomings of the first.         |
| During 2022, Allison introduced log intent items to track physical       |
| manipulations of the extended attribute structures.                      |
| This solves the referential integrity problem by making it possible to   |
| commit a dirent update and a parent pointer update in the same           |
| transaction.                                                             |
| Chandan increased the maximum extent counts of both data and attribute   |
| forks, thereby ensuring that the extended attribute structure can grow   |
| to handle the maximum hardlink count of any file.                        |
|                                                                          |
| (最初的父指针设计太不稳定，文件系统修复之类的功能无法依赖它。)                                         |
| (Allison Henderson、Chandan Babu 和 Catherine Hoang 正在致力于解决第一版所有缺陷的第二个实现。) |
| (在 2022 年期间，Allison 引入了日志意图项来跟踪对扩展属性结构的物理操作。)                            |
| (这使得可以在同一事务中提交 dirent 更新和父指针更新，从而解决了引用完整性问题。)                            |
| (Chandan 提高了数据 fork 和属性 fork 的最大区段数，从而确保扩展属性结构可以增长以处理任何文件的最大硬链接数。)       |
|                                                                          |
| For this second effort, the ondisk parent pointer format as originally   |
| proposed was ``(parent_inum, parent_gen, dirent_pos) → (dirent_name)``.  |
| The format was changed during development to eliminate the requirement   |
| of repair tools needing to ensure that the ``dirent_pos`` field always   |
| matched when reconstructing a directory.                                 |
|                                                                          |
| (最初提出的父指针格式是 ``(parent_inum, parent_gen, dirent_pos) → (dirent_name)``。) |
| (该格式在开发过程中被修改，以消除修复工具在重建目录时需要确保 ``dirent_pos`` 字段始终匹配的要求。)               |
|                                                                          |
| There were a few other ways to have solved that problem:                 |
|                                                                          |
| (还有其他几种方法可以解决这个问题：)                                                      |
|                                                                          |
| 1. The field could be designated advisory, since the other three values  |
|    are sufficient to find the entry in the parent.                       |
|    However, this makes indexed key lookup impossible while repairs are   |
|    ongoing.                                                              |
|                                                                          |
| (该字段可以被指定为建议性的，因为其他三个值足以在父目录中找到该条目。)                                     |
| (但是，这使得修复进行期间无法进行索引键查找。)                                                 |
|                                                                          |
| 2. We could allow creating directory entries at specified offsets, which |
|    solves the referential integrity problem but runs the risk that       |
|    dirent creation will fail due to conflicts with the free space in the |
|    directory.                                                            |
|                                                                          |
| (我们可以允许在指定偏移处创建目录项，这解决了引用完整性问题，)                                         |
| (但存在由于与目录中空闲空间冲突而导致 dirent 创建失败的风险。)                                     |
|                                                                          |
|    These conflicts could be resolved by appending the directory entry    |
|    and amending the xattr code to support updating an xattr key and      |
|    reindexing the dabtree, though this would have to be performed with   |
|    the parent directory still locked.                                    |
|                                                                          |
| (这些冲突可以通过追加目录项并修改 xattr 代码以支持更新 xattr 键和重新索引 dabtree 来解决，)               |
| (不过这必须在父目录仍被锁定的情况下执行。)                                                   |
|                                                                          |
| 3. Same as above, but remove the old parent pointer entry and add a new  |
|    one atomically.                                                       |
|                                                                          |
| (与上面相同，但以原子方式移除旧的父指针条目并添加新条目。)                                           |
|                                                                          |
| 4. Change the ondisk xattr format to                                     |
|    ``(parent_inum, name) → (parent_gen)``, which would provide the attr  |
|    name uniqueness that we require, without forcing repair code to       |
|    update the dirent position.                                           |
|    Unfortunately, this requires changes to the xattr code to support     |
|    attr names as long as 263 bytes.                                      |
|                                                                          |
| (将磁盘上的 xattr 格式改为 ``(parent_inum, name) → (parent_gen)``，)               |
| (这将提供我们所要求的属性名唯一性，而无需强制修复代码更新 dirent 位置。)                                |
| (不幸的是，这需要修改 xattr 代码以支持长达 263 字节的属性名。)                                   |
|                                                                          |
| 5. Change the ondisk xattr format to ``(parent_inum, hash(name)) →       |
|    (name, parent_gen)``.                                                 |
|    If the hash is sufficiently resistant to collisions (e.g. sha256)     |
|    then this should provide the attr name uniqueness that we require.    |
|    Names shorter than 247 bytes could be stored directly.                |
|                                                                          |
| (将磁盘上的 xattr 格式改为 ``(parent_inum, hash(name)) → (name, parent_gen)``。)   |
| (如果哈希对冲突有足够的抵抗力(例如 sha256)，那么这应该能提供我们所要求的属性名唯一性。)                        |
| (短于 247 字节的名称可以直接存储。)                                                    |
|                                                                          |
| 6. Change the ondisk xattr format to ``(dirent_name) → (parent_ino,      |
|    parent_gen)``.  This format doesn't require any of the complicated    |
|    nested name hashing of the previous suggestions.  However, it was     |
|    discovered that multiple hardlinks to the same inode with the same    |
|    filename caused performance problems with hashed xattr lookups, so    |
|    the parent inumber is now xor'd into the hash index.                  |
|                                                                          |
| (将磁盘上的 xattr 格式改为 ``(dirent_name) → (parent_ino, parent_gen)``。)         |
| (此格式不需要前面建议中任何复杂的嵌套名称哈希。)                                                |
| (然而，人们发现指向同一 inode 且文件名相同的多个硬链接会在哈希 xattr 查找中引起性能问题，)                    |
| (因此现在将父 inode 编号异或进哈希索引中。)                                               |
|                                                                          |
| In the end, it was decided that solution #6 was the most compact and the |
| most performant.  A new hash function was designed for parent pointers.  |
| (最终，决定采用方案 #6，因为它最紧凑、性能最好。为父指针设计了一个新的哈希函数。)                              |
+--------------------------------------------------------------------------+


Case Study: Repairing Directories with Parent Pointers
(案例研究：用父指针修复目录)
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

Directory rebuilding uses a :ref:`coordinated inode scan <iscan>` and
a :ref:`directory entry live update hook <liveupdate>` as follows:
(目录重建使用 :ref:`协调的 inode 扫描 <iscan>` 和 :ref:`目录项实时更新挂钩 <liveupdate>`，如下所示：)

1. Set up a temporary directory for generating the new directory structure,
   an xfblob for storing entry names, and an xfarray for stashing the fixed
   size fields involved in a directory update: ``(child inumber, add vs.
   remove, name cookie, ftype)``.
   (建立一个临时目录用于生成新目录结构、一个 xfblob 用于存储条目名称，以及一个 xfarray 用于暂存目录更新中涉及的固定大小字段：``(child inumber, add vs. remove, name cookie, ftype)``。)

2. Set up an inode scanner and hook into the directory entry code to receive
   updates on directory operations.
   (建立一个 inode 扫描器，并挂钩到目录项代码以接收目录操作的更新。)

3. For each parent pointer found in each file scanned, decide if the parent
   pointer references the directory of interest.
   If so:
   (对于扫描的每个文件中发现的每个父指针，判断该父指针是否引用所关注的目录。如果是：)

   a. Stash the parent pointer name and an addname entry for this dirent in the
      xfblob and xfarray, respectively.
      (将父指针名称和此 dirent 的 addname 条目分别暂存到 xfblob 和 xfarray。)

   b. When finished scanning that file or the kernel memory consumption exceeds
      a threshold, flush the stashed updates to the temporary directory.
      (在扫描完该文件或内核内存消耗超过阈值时，将暂存的更新刷新到临时目录。)

4. For each live directory update received via the hook, decide if the child
   has already been scanned.
   If so:
   (对于通过挂钩收到的每条实时目录更新，判断子项是否已被扫描过。如果是：)

   a. Stash the parent pointer name an addname or removename entry for this
      dirent update in the xfblob and xfarray for later.
      We cannot write directly to the temporary directory because hook
      functions are not allowed to modify filesystem metadata.
      Instead, we stash updates in the xfarray and rely on the scanner thread
      to apply the stashed updates to the temporary directory.
      (将此 dirent 更新的父指针名称以及 addname 或 removename 条目暂存到 xfblob 和 xfarray 以备后用。我们不能直接写入临时目录，因为不允许挂钩函数修改文件系统元数据。相反，我们把更新暂存在 xfarray 中，并依靠扫描器线程将暂存的更新应用到临时目录。)

5. When the scan is complete, replay any stashed entries in the xfarray.
   (扫描完成后，重放 xfarray 中所有暂存的条目。)

6. When the scan is complete, atomically exchange the contents of the temporary
   directory and the directory being repaired.
   The temporary directory now contains the damaged directory structure.
   (扫描完成后，原子地交换临时目录与被修复目录的内容。临时目录现在包含受损的目录结构。)

7. Reap the temporary directory.
   (回收临时目录。)

Case Study: Repairing Parent Pointers
(案例研究：修复父指针)
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

Online reconstruction of a file's parent pointer information works similarly to
directory reconstruction:
(文件的父指针信息的在线重建与目录重建的工作方式类似：)

1. Set up a temporary file for generating a new extended attribute structure,
   an xfblob for storing parent pointer names, and an xfarray for stashing the
   fixed size fields involved in a parent pointer update: ``(parent inumber,
   parent generation, add vs. remove, name cookie)``.
   (建立一个临时文件用于生成新的扩展属性结构、一个 xfblob 用于存储父指针名称，以及一个 xfarray 用于暂存父指针更新中涉及的固定大小字段：``(parent inumber, parent generation, add vs. remove, name cookie)``。)

2. Set up an inode scanner and hook into the directory entry code to receive
   updates on directory operations.
   (建立一个 inode 扫描器，并挂钩到目录项代码以接收目录操作的更新。)

3. For each directory entry found in each directory scanned, decide if the
   dirent references the file of interest.
   If so:
   (对于扫描的每个目录中发现的每个目录项，判断该 dirent 是否引用所关注的文件。如果是：)

   a. Stash the dirent name and an addpptr entry for this parent pointer in the
      xfblob and xfarray, respectively.
      (将 dirent 名称和此父指针的 addpptr 条目分别暂存到 xfblob 和 xfarray。)

   b. When finished scanning the directory or the kernel memory consumption
      exceeds a threshold, flush the stashed updates to the temporary file.
      (在扫描完该目录或内核内存消耗超过阈值时，将暂存的更新刷新到临时文件。)

4. For each live directory update received via the hook, decide if the parent
   has already been scanned.
   If so:
   (对于通过挂钩收到的每条实时目录更新，判断父目录是否已被扫描过。如果是：)

   a. Stash the dirent name and an addpptr or removepptr entry for this dirent
      update in the xfblob and xfarray for later.
      We cannot write parent pointers directly to the temporary file because
      hook functions are not allowed to modify filesystem metadata.
      Instead, we stash updates in the xfarray and rely on the scanner thread
      to apply the stashed parent pointer updates to the temporary file.
      (将此 dirent 更新的 dirent 名称以及 addpptr 或 removepptr 条目暂存到 xfblob 和 xfarray 以备后用。我们不能把父指针直接写入临时文件，因为不允许挂钩函数修改文件系统元数据。相反，我们把更新暂存在 xfarray 中，并依靠扫描器线程将暂存的父指针更新应用到临时文件。)

5. When the scan is complete, replay any stashed entries in the xfarray.
   (扫描完成后，重放 xfarray 中所有暂存的条目。)

6. Copy all non-parent pointer extended attributes to the temporary file.
   (把所有非父指针扩展属性复制到临时文件。)

7. When the scan is complete, atomically exchange the mappings of the attribute
   forks of the temporary file and the file being repaired.
   The temporary file now contains the damaged extended attribute structure.
   (扫描完成后，原子地交换临时文件与被修复文件的属性 fork 映射。临时文件现在包含受损的扩展属性结构。)

8. Reap the temporary file.
   (回收临时文件。)

Digression: Offline Checking of Parent Pointers
(题外话：父指针的离线检查)
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

Examining parent pointers in offline repair works differently because corrupt
files are erased long before directory tree connectivity checks are performed.
Parent pointer checks are therefore a second pass to be added to the existing
connectivity checks:
(在离线修复中检查父指针的工作方式不同，因为损坏的文件早在目录树连通性检查执行之前就被擦除了。因此，父指针检查是要添加到现有连通性检查的第二遍：)

1. After the set of surviving files has been established (phase 6),
   walk the surviving directories of each AG in the filesystem.
   This is already performed as part of the connectivity checks.
   (在确定幸存文件集合之后(阶段 6)，遍历文件系统中每个 AG 的幸存目录。这已经作为连通性检查的一部分执行。)

2. For each directory entry found,
   (对于发现的每个目录项：)

   a. If the name has already been stored in the xfblob, then use that cookie
      and skip the next step.
      (如果名称已存储在 xfblob 中，则使用该 cookie 并跳过下一步。)

   b. Otherwise, record the name in an xfblob, and remember the xfblob cookie.
      Unique mappings are critical for
      (否则，在一个 xfblob 中记录该名称，并记住 xfblob cookie。唯一映射对以下方面至关重要：)

      1. Deduplicating names to reduce memory usage, and
         (对名称去重以减少内存使用，以及)

      2. Creating a stable sort key for the parent pointer indexes so that the
         parent pointer validation described below will work.
         (为父指针索引创建稳定的排序键，以便下文描述的父指针校验能够工作。)

   c. Store ``(child_ag_inum, parent_inum, parent_gen, name_hash, name_len,
      name_cookie)`` tuples in a per-AG in-memory slab.  The ``name_hash``
      referenced in this section is the regular directory entry name hash, not
      the specialized one used for parent pointer xattrs.
      (在每 AG 的内存 slab 中存储 ``(child_ag_inum, parent_inum, parent_gen, name_hash, name_len, name_cookie)`` 元组。本节引用的 ``name_hash`` 是常规目录项名称哈希，而不是用于父指针 xattr 的专用哈希。)

3. For each AG in the filesystem,
   (对于文件系统中的每个 AG：)

   a. Sort the per-AG tuple set in order of ``child_ag_inum``, ``parent_inum``,
      ``name_hash``, and ``name_cookie``.
      Having a single ``name_cookie`` for each ``name`` is critical for
      handling the uncommon case of a directory containing multiple hardlinks
      to the same file where all the names hash to the same value.
      (按 ``child_ag_inum``、``parent_inum``、``name_hash`` 和 ``name_cookie`` 的顺序对每 AG 元组集排序。每个 ``name`` 只有一个 ``name_cookie`` 对于处理不常见的情形至关重要——即一个目录包含指向同一文件的多个硬链接且所有名称哈希到相同的值。)

   b. For each inode in the AG,
      (对于该 AG 中的每个 inode：)

      1. Scan the inode for parent pointers.
         For each parent pointer found,
         (扫描该 inode 寻找父指针。对发现的每个父指针：)

         a. Validate the ondisk parent pointer.
            If validation fails, move on to the next parent pointer in the
            file.
            (校验磁盘父指针。如果校验失败，转到该文件中的下一个父指针。)

         b. If the name has already been stored in the xfblob, then use that
            cookie and skip the next step.
            (如果名称已存储在 xfblob 中，则使用该 cookie 并跳过下一步。)

         c. Record the name in a per-file xfblob, and remember the xfblob
            cookie.
            (在一个每文件 xfblob 中记录该名称，并记住 xfblob cookie。)

         d. Store ``(parent_inum, parent_gen, name_hash, name_len,
            name_cookie)`` tuples in a per-file slab.
            (在一个每文件 slab 中存储 ``(parent_inum, parent_gen, name_hash, name_len, name_cookie)`` 元组。)

      2. Sort the per-file tuples in order of ``parent_inum``, ``name_hash``,
         and ``name_cookie``.
         (按 ``parent_inum``、``name_hash`` 和 ``name_cookie`` 的顺序对每文件元组排序。)

      3. Position one slab cursor at the start of the inode's records in the
         per-AG tuple slab.
         This should be trivial since the per-AG tuples are in child inumber
         order.
         (把一个 slab 游标定位到每 AG 元组 slab 中该 inode 记录的起始处。这应该很简单，因为每 AG 元组按子 inumber 顺序排列。)

      4. Position a second slab cursor at the start of the per-file tuple slab.
         (把第二个 slab 游标定位到每文件元组 slab 的起始处。)

      5. Iterate the two cursors in lockstep, comparing the ``parent_ino``,
         ``name_hash``, and ``name_cookie`` fields of the records under each
         cursor:
         (以锁步方式迭代这两个游标，比较每个游标之下记录的 ``parent_ino``、``name_hash`` 和 ``name_cookie`` 字段：)

         a. If the per-AG cursor is at a lower point in the keyspace than the
            per-file cursor, then the per-AG cursor points to a missing parent
            pointer.
            Add the parent pointer to the inode and advance the per-AG
            cursor.
            (如果每 AG 游标在键空间中的位置低于每文件游标，则每 AG 游标指向一个缺失的父指针。把该父指针添加到 inode 并推进每 AG 游标。)

         b. If the per-file cursor is at a lower point in the keyspace than
            the per-AG cursor, then the per-file cursor points to a dangling
            parent pointer.
            Remove the parent pointer from the inode and advance the per-file
            cursor.
            (如果每文件游标在键空间中的位置低于每 AG 游标，则每文件游标指向一个悬空的父指针。从 inode 中移除该父指针并推进每文件游标。)

         c. Otherwise, both cursors point at the same parent pointer.
            Update the parent_gen component if necessary.
            Advance both cursors.
            (否则，两个游标指向同一个父指针。如有必要更新 parent_gen 分量。推进两个游标。)

4. Move on to examining link counts, as we do today.
   (像今天一样，继续进行链接计数检查。)

Rebuilding directories from parent pointers in offline repair would be very
challenging because xfs_repair currently uses two single-pass scans of the
filesystem during phases 3 and 4 to decide which files are corrupt enough to be
zapped.
This scan would have to be converted into a multi-pass scan:
(在离线修复中从父指针重建目录将非常具有挑战性，因为 xfs_repair 目前在阶段 3 和 4 期间使用两次单遍文件系统扫描来决定哪些文件损坏到足以被清除。这一扫描必须转换为多遍扫描：)

1. The first pass of the scan zaps corrupt inodes, forks, and attributes
   much as it does now.
   Corrupt directories are noted but not zapped.
   (扫描的第一遍像现在一样清除损坏的 inode、fork 和属性。损坏的目录被记录但不被清除。)

2. The next pass records parent pointers pointing to the directories noted
   as being corrupt in the first pass.
   This second pass may have to happen after the phase 4 scan for duplicate
   blocks, if phase 4 is also capable of zapping directories.
   (下一遍记录指向第一遍中标记为损坏的目录的父指针。如果阶段 4 也具备清除目录的能力，这第二遍可能必须在阶段 4 的重复块扫描之后进行。)

3. The third pass resets corrupt directories to an empty shortform directory.
   Free space metadata has not been ensured yet, so repair cannot yet use the
   directory building code in libxfs.
   (第三遍把损坏的目录重置为空的 shortform 目录。空闲空间元数据尚未得到保证，所以修复还不能使用 libxfs 中的目录构建代码。)

4. At the start of phase 6, space metadata have been rebuilt.
   Use the parent pointer information recorded during step 2 to reconstruct
   the dirents and add them to the now-empty directories.
   (在阶段 6 开始时，空间元数据已被重建。使用步骤 2 中记录的父指针信息重建 dirent，并把它们添加到现已为空的目录中。)

This code has not yet been constructed.
(这段代码尚未构建。)

.. _dirtree:

Case Study: Directory Tree Structure
(案例研究：目录树结构)
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

As mentioned earlier, the filesystem directory tree is supposed to be a
directed acylic graph structure.
However, each node in this graph is a separate ``xfs_inode`` object with its
own locks, which makes validating the tree qualities difficult.
Fortunately, non-directories are allowed to have multiple parents and cannot
have children, so only directories need to be scanned.
Directories typically constitute 5-10% of the files in a filesystem, which
reduces the amount of work dramatically.
(如前所述，文件系统目录树理应是一个有向无环图结构。然而，这个图中的每个节点都是一个拥有自己锁的独立 ``xfs_inode`` 对象，这使得校验树性质变得困难。幸运的是，允许非目录拥有多个父目录且不能有子项，所以只需扫描目录。目录通常占文件系统中文件的 5-10%，这大幅减少了工作量。)

If the directory tree could be frozen, it would be easy to discover cycles and
disconnected regions by running a depth (or breadth) first search downwards
from the root directory and marking a bitmap for each directory found.
At any point in the walk, trying to set an already set bit means there is a
cycle.
After the scan completes, XORing the marked inode bitmap with the inode
allocation bitmap reveals disconnected inodes.
However, one of online repair's design goals is to avoid locking the entire
filesystem unless it's absolutely necessary.
Directory tree updates can move subtrees across the scanner wavefront on a live
filesystem, so the bitmap algorithm cannot be applied.
(如果目录树可以被冻结，那么通过从根目录向下运行深度(或广度)优先搜索并为找到的每个目录标记一个位图，就很容易发现环路和不连通区域。在遍历的任何时刻，试图设置一个已置位的位就意味着存在环路。扫描完成后，将标记的 inode 位图与 inode 分配位图进行异或即可揭示不连通的 inode。然而，在线修复的设计目标之一是避免锁定整个文件系统，除非绝对必要。在活动文件系统上，目录树更新可以把子树移过扫描器波前，所以位图算法无法应用。)

Directory parent pointers enable an incremental approach to validation of the
tree structure.
Instead of using one thread to scan the entire filesystem, multiple threads can
walk from individual subdirectories upwards towards the root.
For this to work, all directory entries and parent pointers must be internally
consistent, each directory entry must have a parent pointer, and the link
counts of all directories must be correct.
Each scanner thread must be able to take the IOLOCK of an alleged parent
directory while holding the IOLOCK of the child directory to prevent either
directory from being moved within the tree.
This is not possible since the VFS does not take the IOLOCK of a child
subdirectory when moving that subdirectory, so instead the scanner stabilizes
the parent -> child relationship by taking the ILOCKs and installing a dirent
update hook to detect changes.
(目录父指针使树结构校验可以采用增量方法。与其使用一个线程扫描整个文件系统，多个线程可以从各个子目录向上走向根目录。要使其工作，所有目录项和父指针必须在内部一致，每个目录项必须有一个父指针，并且所有目录的链接计数必须正确。每个扫描器线程必须能够在持有子目录 IOLOCK 的同时获取所称父目录的 IOLOCK，以防止任一目录在树中被移动。这不可能做到，因为 VFS 在移动子目录时并不获取该子目录的 IOLOCK，所以扫描器改为通过获取 ILOCK 并安装一个 dirent 更新挂钩来检测变化，从而稳定父 -> 子关系。)

The scanning process uses a dirent hook to detect changes to the directories
mentioned in the scan data.
The scan works as follows:
(扫描过程使用一个 dirent 挂钩来检测扫描数据中提及的目录的变化。扫描按如下方式进行：)

1. For each subdirectory in the filesystem,
   (对于文件系统中的每个子目录：)

   a. For each parent pointer of that subdirectory,
      (对于该子目录的每个父指针：)

      1. Create a path object for that parent pointer, and mark the
         subdirectory inode number in the path object's bitmap.
         (为该父指针创建一个路径对象，并在路径对象的位图中标记该子目录的 inode 编号。)

      2. Record the parent pointer name and inode number in a path structure.
         (在一个路径结构中记录父指针名称和 inode 编号。)

      3. If the alleged parent is the subdirectory being scrubbed, the path is
         a cycle.
         Mark the path for deletion and repeat step 1a with the next
         subdirectory parent pointer.
         (如果所称父目录就是被 scrub 的子目录，则该路径是一个环路。将该路径标记为待删除，并用下一个子目录父指针重复步骤 1a。)

      4. Try to mark the alleged parent inode number in a bitmap in the path
         object.
         If the bit is already set, then there is a cycle in the directory
         tree.
         Mark the path as a cycle and repeat step 1a with the next subdirectory
         parent pointer.
         (尝试在路径对象的位图中标记所称父目录的 inode 编号。如果该位已被置位，则目录树中存在环路。将该路径标记为环路，并用下一个子目录父指针重复步骤 1a。)

      5. Load the alleged parent.
         If the alleged parent is not a linked directory, abort the scan
         because the parent pointer information is inconsistent.
         (加载所称父目录。如果所称父目录不是一个已链接的目录，则中止扫描，因为父指针信息不一致。)

      6. For each parent pointer of this alleged ancestor directory,
         (对于这个所称祖先目录的每个父指针：)

         a. Record the parent pointer name and inode number in the path object
            if no parent has been set for that level.
            (如果该层级尚未设置父项，则在路径对象中记录父指针名称和 inode 编号。)

         b. If an ancestor has more than one parent, mark the path as corrupt.
            Repeat step 1a with the next subdirectory parent pointer.
            (如果一个祖先有多于一个父目录，则将该路径标记为损坏。用下一个子目录父指针重复步骤 1a。)

         c. Repeat steps 1a3-1a6 for the ancestor identified in step 1a6a.
            This repeats until the directory tree root is reached or no parents
            are found.
            (对步骤 1a6a 中识别的祖先重复步骤 1a3-1a6。如此重复，直到到达目录树根或未找到任何父目录。)

      7. If the walk terminates at the root directory, mark the path as ok.
         (如果遍历终止于根目录，则将该路径标记为正常(ok)。)

      8. If the walk terminates without reaching the root, mark the path as
         disconnected.
         (如果遍历未到达根就终止，则将该路径标记为断连(disconnected)。)

2. If the directory entry update hook triggers, check all paths already found
   by the scan.
   If the entry matches part of a path, mark that path and the scan stale.
   When the scanner thread sees that the scan has been marked stale, it deletes
   all scan data and starts over.
   (如果目录项更新挂钩被触发，则检查扫描已发现的所有路径。如果该条目匹配某个路径的一部分，则将该路径和扫描标记为过期(stale)。当扫描器线程看到扫描已被标记为过期时，它会删除所有扫描数据并重新开始。)

Repairing the directory tree works as follows:
(修复目录树按如下方式进行：)

1. Walk each path of the target subdirectory.
   (遍历目标子目录的每条路径。)

   a. Corrupt paths and cycle paths are counted as suspect.
      (损坏路径和环路路径被计为可疑(suspect)。)

   b. Paths already marked for deletion are counted as bad.
      (已标记为待删除的路径被计为坏(bad)。)

   c. Paths that reached the root are counted as good.
      (到达了根的路径被计为好(good)。)

2. If the subdirectory is either the root directory or has zero link count,
   delete all incoming directory entries in the immediate parents.
   Repairs are complete.
   (如果该子目录是根目录或链接计数为零，则删除直接父目录中所有指向它的目录项。修复完成。)

3. If the subdirectory has exactly one path, set the dotdot entry to the
   parent and exit.
   (如果该子目录恰好有一条路径，则将 dotdot 条目设置为该父目录并退出。)

4. If the subdirectory has at least one good path, delete all the other
   incoming directory entries in the immediate parents.
   (如果该子目录至少有一条好路径，则删除直接父目录中所有其他指向它的目录项。)

5. If the subdirectory has no good paths and more than one suspect path, delete
   all the other incoming directory entries in the immediate parents.
   (如果该子目录没有任何好路径且可疑路径多于一条，则删除直接父目录中所有其他指向它的目录项。)

6. If the subdirectory has zero paths, attach it to the lost and found.
   (如果该子目录没有任何路径，则将其挂到 lost and found。)

.. _orphanage:

The Orphanage
(孤儿院)
-------------

Filesystems present files as a directed, and hopefully acyclic, graph.
In other words, a tree.
The root of the filesystem is a directory, and each entry in a directory points
downwards either to more subdirectories or to non-directory files.
Unfortunately, a disruption in the directory graph pointers result in a
disconnected graph, which makes files impossible to access via regular path
resolution.
(文件系统把文件呈现为一个有向的、但愿无环的图。换句话说，是一棵树。文件系统的根是一个目录，目录中的每个条目向下指向更多子目录或非目录文件。不幸的是，目录图指针的破坏会导致图不连通，使文件无法通过常规路径解析访问。)

Without parent pointers, the directory parent pointer online scrub code can
detect a dotdot entry pointing to a parent directory that doesn't have a link
back to the child directory and the file link count checker can detect a file
that isn't pointed to by any directory in the filesystem.
If such a file has a positive link count, the file is an orphan.
(没有父指针时，目录父指针在线 scrub 代码可以检测出 dotdot 条目指向一个没有指回子目录链接的父目录，文件链接计数检查器可以检测出未被文件系统中任何目录指向的文件。如果这样的文件具有正的链接计数，它就是一个孤儿。)

With parent pointers, directories can be rebuilt by scanning parent pointers
and parent pointers can be rebuilt by scanning directories.
This should reduce the incidence of files ending up in ``/lost+found``.
(有了父指针，可以通过扫描父指针重建目录，也可以通过扫描目录重建父指针。这应该会减少文件最终落入 ``/lost+found`` 的情况。)

When orphans are found, they should be reconnected to the directory tree.
Offline fsck solves the problem by creating a directory ``/lost+found`` to
serve as an orphanage, and linking orphan files into the orphanage by using the
inumber as the name.
Reparenting a file to the orphanage does not reset any of its permissions or
ACLs.
(发现孤儿时，应当把它们重新连接到目录树。离线 fsck 通过创建一个 ``/lost+found`` 目录充当孤儿院来解决这一问题，并以 inumber 作为名称把孤儿文件链接进孤儿院。把文件重新指定父目录到孤儿院不会重置其任何权限或 ACL。)

This process is more involved in the kernel than it is in userspace.
The directory and file link count repair setup functions must use the regular
VFS mechanisms to create the orphanage directory with all the necessary
security attributes and dentry cache entries, just like a regular directory
tree modification.
(这一过程在内核中比在用户空间中更复杂。目录和文件链接计数修复的设置函数必须使用常规的 VFS 机制来创建孤儿院目录，包括所有必要的安全属性和 dentry 缓存条目，就像一次常规的目录树修改一样。)

Orphaned files are adopted by the orphanage as follows:
(孤儿文件按如下方式被孤儿院收养：)

1. Call ``xrep_orphanage_try_create`` at the start of the scrub setup function
   to try to ensure that the lost and found directory actually exists.
   This also attaches the orphanage directory to the scrub context.
   (在 scrub 设置函数的开头调用 ``xrep_orphanage_try_create``，尝试确保 lost and found 目录确实存在。这也会把孤儿院目录附着到 scrub 上下文。)

2. If the decision is made to reconnect a file, take the IOLOCK of both the
   orphanage and the file being reattached.
   The ``xrep_orphanage_iolock_two`` function follows the inode locking
   strategy discussed earlier.
   (如果决定重新连接一个文件，则同时获取孤儿院和被重连文件的 IOLOCK。``xrep_orphanage_iolock_two`` 函数遵循前文讨论的 inode 锁定策略。)

3. Use ``xrep_adoption_trans_alloc`` to reserve resources to the repair
   transaction.
   (使用 ``xrep_adoption_trans_alloc`` 为修复事务预留资源。)

4. Call ``xrep_orphanage_compute_name`` to compute the new name in the
   orphanage.
   (调用 ``xrep_orphanage_compute_name`` 计算孤儿院中的新名称。)

5. If the adoption is going to happen, call ``xrep_adoption_reparent`` to
   reparent the orphaned file into the lost and found and invalidate the dentry
   cache.
   (如果将进行收养，则调用 ``xrep_adoption_reparent`` 把孤儿文件重新指定父目录到 lost and found，并使 dentry 缓存失效。)

6. Call ``xrep_adoption_finish`` to commit any filesystem updates, release the
   orphanage ILOCK, and clean the scrub transaction.  Call
   ``xrep_adoption_commit`` to commit the updates and the scrub transaction.
   (调用 ``xrep_adoption_finish`` 提交任何文件系统更新、释放孤儿院 ILOCK 并清理 scrub 事务。调用 ``xrep_adoption_commit`` 提交更新和 scrub 事务。)

7. If a runtime error happens, call ``xrep_adoption_cancel`` to release all
   resources.
   (如果发生运行时错误，则调用 ``xrep_adoption_cancel`` 释放所有资源。)

6. Userspace Algorithms and Data Structures
(6. 用户空间算法和数据结构)
===========================================

This section discusses the key algorithms and data structures of the userspace
program, ``xfs_scrub``, that provide the ability to drive metadata checks and
repairs in the kernel, verify file data, and look for other potential problems.
(本节讨论用户空间程序 ``xfs_scrub`` 的关键算法和数据结构，它们提供在内核中驱动元数据检查和修复、校验文件数据以及查找其他潜在问题的能力。)

.. _scrubcheck:

Checking Metadata
(检查元数据)
-----------------

Recall the :ref:`phases of fsck work<scrubphases>` outlined earlier.
That structure follows naturally from the data dependencies designed into the
filesystem from its beginnings in 1993.
In XFS, there are several groups of metadata dependencies:
(回想一下前面概述的 :ref:`fsck 工作阶段 <scrubphases>`。这一结构自然地源自文件系统自 1993 年诞生以来设计其中的数据依赖关系。在 XFS 中，有若干组元数据依赖：)

a. Filesystem summary counts depend on consistency within the inode indices,
   the allocation group space btrees, and the realtime volume space
   information.
   (文件系统摘要计数依赖于 inode 索引、分配组空间 btree 以及实时卷空间信息内部的一致性。)

b. Quota resource counts depend on consistency within the quota file data
   forks, inode indices, inode records, and the forks of every file on the
   system.
   (配额资源计数依赖于配额文件数据 fork、inode 索引、inode 记录以及系统上每个文件的 fork 内部的一致性。)

c. The naming hierarchy depends on consistency within the directory and
   extended attribute structures.
   This includes file link counts.
   (命名层级依赖于目录和扩展属性结构内部的一致性。这包括文件链接计数。)

d. Directories, extended attributes, and file data depend on consistency within
   the file forks that map directory and extended attribute data to physical
   storage media.
   (目录、扩展属性和文件数据依赖于将目录和扩展属性数据映射到物理存储介质的文件 fork 内部的一致性。)

e. The file forks depends on consistency within inode records and the space
   metadata indices of the allocation groups and the realtime volume.
   This includes quota and realtime metadata files.
   (文件 fork 依赖于 inode 记录以及分配组和实时卷的空间元数据索引内部的一致性。这包括配额和实时元数据文件。)

f. Inode records depends on consistency within the inode metadata indices.
   (inode 记录依赖于 inode 元数据索引内部的一致性。)

g. Realtime space metadata depend on the inode records and data forks of the
   realtime metadata inodes.
   (实时空间元数据依赖于实时元数据 inode 的 inode 记录和数据 fork。)

h. The allocation group metadata indices (free space, inodes, reference count,
   and reverse mapping btrees) depend on consistency within the AG headers and
   between all the AG metadata btrees.
   (分配组元数据索引(空闲空间、inode、引用计数和反向映射 btree)依赖于 AG 头内部以及所有 AG 元数据 btree 之间的一致性。)

i. ``xfs_scrub`` depends on the filesystem being mounted and kernel support
   for online fsck functionality.
   (``xfs_scrub`` 依赖于文件系统已挂载以及内核对在线 fsck 功能的支持。)

Therefore, a metadata dependency graph is a convenient way to schedule checking
operations in the ``xfs_scrub`` program:
(因此，元数据依赖图是在 ``xfs_scrub`` 程序中调度检查操作的一种便捷方式：)

- Phase 1 checks that the provided path maps to an XFS filesystem and detect
  the kernel's scrubbing abilities, which validates group (i).
  (阶段 1 检查所提供的路径映射到 XFS 文件系统，并探测内核的 scrubbing 能力，这验证了组 (i)。)

- Phase 2 scrubs groups (g) and (h) in parallel using a threaded workqueue.
  (阶段 2 使用带线程的工作队列并行 scrub 组 (g) 和 (h)。)

- Phase 3 scans inodes in parallel.
  For each inode, groups (f), (e), and (d) are checked, in that order.
  (阶段 3 并行扫描 inode。对每个 inode，按顺序检查组 (f)、(e) 和 (d)。)

- Phase 4 repairs everything in groups (i) through (d) so that phases 5 and 6
  may run reliably.
  (阶段 4 修复组 (i) 到 (d) 中的一切，使阶段 5 和 6 可以可靠运行。)

- Phase 5 starts by checking groups (b) and (c) in parallel before moving on
  to checking names.
  (阶段 5 先并行检查组 (b) 和 (c)，然后继续检查名称。)

- Phase 6 depends on groups (i) through (b) to find file data blocks to verify,
  to read them, and to report which blocks of which files are affected.
  (阶段 6 依赖组 (i) 到 (b) 来查找要校验的文件数据块、读取它们，并报告哪些文件的哪些块受影响。)

- Phase 7 checks group (a), having validated everything else.
  (在验证了其他一切之后，阶段 7 检查组 (a)。)

Notice that the data dependencies between groups are enforced by the structure
of the program flow.
(注意，组之间的数据依赖由程序流程的结构来强制执行。)

Parallel Inode Scans
(并行 inode 扫描)
--------------------

An XFS filesystem can easily contain hundreds of millions of inodes.
Given that XFS targets installations with large high-performance storage,
it is desirable to scrub inodes in parallel to minimize runtime, particularly
if the program has been invoked manually from a command line.
This requires careful scheduling to keep the threads as evenly loaded as
possible.
(一个 XFS 文件系统可以轻松包含数亿个 inode。鉴于 XFS 面向配备大型高性能存储的部署环境，希望并行 scrub inode 以尽量减少运行时间，特别是当程序从命令行手动调用时。这需要仔细的调度，使线程负载尽可能均匀。)

Early iterations of the ``xfs_scrub`` inode scanner naïvely created a single
workqueue and scheduled a single workqueue item per AG.
Each workqueue item walked the inode btree (with ``XFS_IOC_INUMBERS``) to find
inode chunks and then called bulkstat (``XFS_IOC_BULKSTAT``) to gather enough
information to construct file handles.
The file handle was then passed to a function to generate scrub items for each
metadata object of each inode.
This simple algorithm leads to thread balancing problems in phase 3 if the
filesystem contains one AG with a few large sparse files and the rest of the
AGs contain many smaller files.
The inode scan dispatch function was not sufficiently granular; it should have
been dispatching at the level of individual inodes, or, to constrain memory
consumption, inode btree records.
(``xfs_scrub`` inode 扫描器的早期迭代天真地创建了一个单一的工作队列，并为每个 AG 调度一个工作队列项。每个工作队列项遍历 inode btree(使用 ``XFS_IOC_INUMBERS``)以查找 inode 块，然后调用 bulkstat(``XFS_IOC_BULKSTAT``)收集足够的信息来构造文件句柄。随后文件句柄被传递给一个函数，为每个 inode 的每个元数据对象生成 scrub 项。如果文件系统包含一个只有少数大型稀疏文件的 AG，而其余 AG 包含许多较小的文件，这个简单算法会在阶段 3 导致线程均衡问题。inode 扫描分发函数的粒度不够细；它本应按单个 inode 的级别分发，或者为约束内存消耗而按 inode btree 记录的级别分发。)

Thanks to Dave Chinner, bounded workqueues in userspace enable ``xfs_scrub`` to
avoid this problem with ease by adding a second workqueue.
Just like before, the first workqueue is seeded with one workqueue item per AG,
and it uses INUMBERS to find inode btree chunks.
The second workqueue, however, is configured with an upper bound on the number
of items that can be waiting to be run.
Each inode btree chunk found by the first workqueue's workers are queued to the
second workqueue, and it is this second workqueue that queries BULKSTAT,
creates a file handle, and passes it to a function to generate scrub items for
each metadata object of each inode.
If the second workqueue is too full, the workqueue add function blocks the
first workqueue's workers until the backlog eases.
This doesn't completely solve the balancing problem, but reduces it enough to
move on to more pressing issues.
(多亏了 Dave Chinner，用户空间中有界的工作队列使 ``xfs_scrub`` 能够通过添加第二个工作队列轻松避免这个问题。与之前一样，第一个工作队列以每个 AG 一个工作队列项作为种子，并使用 INUMBERS 查找 inode btree 块。而第二个工作队列配置了等待运行的项数上限。第一个工作队列的工作者发现的每个 inode btree 块都被排队到第二个工作队列，正是这第二个工作队列查询 BULKSTAT、创建文件句柄，并将其传递给一个函数来为每个 inode 的每个元数据对象生成 scrub 项。如果第二个工作队列太满，工作队列添加函数会阻塞第一个工作队列的工作者，直到积压缓解。这并没有完全解决均衡问题，但已足够缓解，可以继续处理更紧迫的问题。)

.. _scrubrepair:

Scheduling Repairs
(调度修复)
------------------

During phase 2, corruptions and inconsistencies reported in any AGI header or
inode btree are repaired immediately, because phase 3 relies on proper
functioning of the inode indices to find inodes to scan.
Failed repairs are rescheduled to phase 4.
Problems reported in any other space metadata are deferred to phase 4.
Optimization opportunities are always deferred to phase 4, no matter their
origin.
(在阶段 2 中，任何 AGI 头或 inode btree 中报告的损坏和不一致都会立即修复，因为阶段 3 依赖 inode 索引的正常运行来查找要扫描的 inode。失败的修复被重新调度到阶段 4。在任何其他空间元数据中报告的问题被推迟到阶段 4。优化机会总是被推迟到阶段 4，无论其来源是什么。)

During phase 3, corruptions and inconsistencies reported in any part of a
file's metadata are repaired immediately if all space metadata were validated
during phase 2.
Repairs that fail or cannot be repaired immediately are scheduled for phase 4.
(在阶段 3 中，如果所有空间元数据都在阶段 2 中得到校验，文件元数据任何部分报告的损坏和不一致都会立即修复。失败或无法立即修复的修复被调度到阶段 4。)

In the original design of ``xfs_scrub``, it was thought that repairs would be
so infrequent that the ``struct xfs_scrub_metadata`` objects used to
communicate with the kernel could also be used as the primary object to
schedule repairs.
With recent increases in the number of optimizations possible for a given
filesystem object, it became much more memory-efficient to track all eligible
repairs for a given filesystem object with a single repair item.
Each repair item represents a single lockable object -- AGs, metadata files,
individual inodes, or a class of summary information.
(在 ``xfs_scrub`` 的原始设计中，人们认为修复会非常罕见，以至于用于与内核通信的 ``struct xfs_scrub_metadata`` 对象也可以用作调度修复的主要对象。随着最近给定文件系统对象可能的优化数量增加，用单个修复项跟踪给定文件系统对象的所有符合条件修复变得在内存上高效得多。每个修复项代表一个单独的可锁定对象——AG、元数据文件、单个 inode，或一类摘要信息。)

Phase 4 is responsible for scheduling a lot of repair work in as quick a
manner as is practical.
The :ref:`data dependencies <scrubcheck>` outlined earlier still apply, which
means that ``xfs_scrub`` must try to complete the repair work scheduled by
phase 2 before trying repair work scheduled by phase 3.
The repair process is as follows:
(阶段 4 负责以实际可行的最快方式调度大量修复工作。前面概述的 :ref:`数据依赖 <scrubcheck>` 仍然适用，这意味着 ``xfs_scrub`` 必须先尝试完成阶段 2 调度的修复工作，再尝试阶段 3 调度的修复工作。修复过程如下：)

1. Start a round of repair with a workqueue and enough workers to keep the CPUs
   as busy as the user desires.
   (用一个工作队列和足够的工作者启动一轮修复，使 CPU 保持用户期望的忙碌程度。)

   a. For each repair item queued by phase 2,
      (对于阶段 2 排队的每个修复项：)

      i.   Ask the kernel to repair everything listed in the repair item for a
           given filesystem object.
           (请内核修复修复项中为给定文件系统对象列出的所有内容。)

      ii.  Make a note if the kernel made any progress in reducing the number
           of repairs needed for this object.
           (记下内核是否在减少此对象所需修复数量方面取得任何进展。)

      iii. If the object no longer requires repairs, revalidate all metadata
           associated with this object.
           If the revalidation succeeds, drop the repair item.
           If not, requeue the item for more repairs.
           (如果对象不再需要修复，则重新校验与此对象关联的所有元数据。如果重新校验成功，则丢弃该修复项。如果没有成功，则将该修复项重新排队以进行更多修复。)

   b. If any repairs were made, jump back to 1a to retry all the phase 2 items.
      (如果进行了任何修复，则跳回 1a 重试所有阶段 2 的修复项。)

   c. For each repair item queued by phase 3,
      (对于阶段 3 排队的每个修复项：)

      i.   Ask the kernel to repair everything listed in the repair item for a
           given filesystem object.
           (请内核修复修复项中为给定文件系统对象列出的所有内容。)

      ii.  Make a note if the kernel made any progress in reducing the number
           of repairs needed for this object.
           (记下内核是否在减少此对象所需修复数量方面取得任何进展。)

      iii. If the object no longer requires repairs, revalidate all metadata
           associated with this object.
           If the revalidation succeeds, drop the repair item.
           If not, requeue the item for more repairs.
           (如果对象不再需要修复，则重新校验与此对象关联的所有元数据。如果重新校验成功，则丢弃该修复项。如果没有成功，则将该修复项重新排队以进行更多修复。)

   d. If any repairs were made, jump back to 1c to retry all the phase 3 items.
      (如果进行了任何修复，则跳回 1c 重试所有阶段 3 的修复项。)

2. If step 1 made any repair progress of any kind, jump back to step 1 to start
   another round of repair.
   (如果步骤 1 取得了任何类型的修复进展，则跳回步骤 1 开始另一轮修复。)

3. If there are items left to repair, run them all serially one more time.
   Complain if the repairs were not successful, since this is the last chance
   to repair anything.
   (如果还有待修复的项，则将它们全部再串行运行一次。如果修复不成功则报错，因为这是修复任何东西的最后机会。)

Corruptions and inconsistencies encountered during phases 5 and 7 are repaired
immediately.
Corrupt file data blocks reported by phase 6 cannot be recovered by the
filesystem.
(在阶段 5 和 7 中遇到的损坏和不一致会立即修复。阶段 6 报告的损坏文件数据块无法由文件系统恢复。)

Checking Names for Confusable Unicode Sequences
(检查名称中易混淆的 Unicode 序列)
-----------------------------------------------

If ``xfs_scrub`` succeeds in validating the filesystem metadata by the end of
phase 4, it moves on to phase 5, which checks for suspicious looking names in
the filesystem.
These names consist of the filesystem label, names in directory entries, and
the names of extended attributes.
Like most Unix filesystems, XFS imposes the sparest of constraints on the
contents of a name:
(如果 ``xfs_scrub`` 在阶段 4 结束时成功校验了文件系统元数据，它就进入阶段 5，检查文件系统中看起来可疑的名称。这些名称包括文件系统标签、目录项中的名称以及扩展属性的名称。与大多数 Unix 文件系统一样，XFS 对名称内容的约束是最宽松的：)

- Slashes and null bytes are not allowed in directory entries.
  (目录项中不允许出现斜杠和空字节。)

- Null bytes are not allowed in userspace-visible extended attributes.
  (用户空间可见的扩展属性中不允许出现空字节。)

- Null bytes are not allowed in the filesystem label.
  (文件系统标签中不允许出现空字节。)

Directory entries and attribute keys store the length of the name explicitly
ondisk, which means that nulls are not name terminators.
For this section, the term "naming domain" refers to any place where names are
presented together -- all the names in a directory, or all the attributes of a
file.
(目录项和属性键在磁盘上显式存储名称的长度，这意味着空字节不是名称终结符。在本节中，术语"命名域(naming domain)"指任何名称集中出现的地方——一个目录中的所有名称，或一个文件的所有属性。)

Although the Unix naming constraints are very permissive, the reality of most
modern-day Linux systems is that programs work with Unicode character code
points to support international languages.
These programs typically encode those code points in UTF-8 when interfacing
with the C library because the kernel expects null-terminated names.
In the common case, therefore, names found in an XFS filesystem are actually
UTF-8 encoded Unicode data.
(尽管 Unix 的命名约束非常宽松，但当今大多数 Linux 系统的现实是，程序使用 Unicode 字符码点来支持国际语言。这些程序在与 C 库交互时通常以 UTF-8 编码这些码点，因为内核期望以空字节结尾的名称。因此在常见情况下，XFS 文件系统中的名称实际上是 UTF-8 编码的 Unicode 数据。)

To maximize its expressiveness, the Unicode standard defines separate control
points for various characters that render similarly or identically in writing
systems around the world.
For example, the character "Cyrillic Small Letter A" U+0430 "а" often renders
identically to "Latin Small Letter A" U+0061 "a".
(为了最大化其表现力，Unicode 标准为在世界各地的书写系统中呈现相似或相同的各种字符定义了单独的控制码点。例如，字符 "Cyrillic Small Letter A"(西里尔小写字母 A)U+0430 "а" 的呈现常常与 "Latin Small Letter A"(拉丁小写字母 A)U+0061 "a" 相同。)

The standard also permits characters to be constructed in multiple ways --
either by using a defined code point, or by combining one code point with
various combining marks.
For example, the character "Angstrom Sign U+212B "Å" can also be expressed
as "Latin Capital Letter A" U+0041 "A" followed by "Combining Ring Above"
U+030A "◌̊".
Both sequences render identically.
(该标准还允许用多种方式构造字符——既可以使用已定义的码点，也可以将一个码点与各种组合附加符号结合。例如，字符 "Angstrom Sign" U+212B "Å" 也可以表示为 "Latin Capital Letter A" U+0041 "A" 后跟 "Combining Ring Above" U+030A "◌̊"。两个序列的呈现完全相同。)

Like the standards that preceded it, Unicode also defines various control
characters to alter the presentation of text.
For example, the character "Right-to-Left Override" U+202E can trick some
programs into rendering "moo\\xe2\\x80\\xaegnp.txt" as "mootxt.png".
A second category of rendering problems involves whitespace characters.
If the character "Zero Width Space" U+200B is encountered in a file name, the
name will render identically to a name that does not have the zero width
space.
(像它之前的各种标准一样，Unicode 还定义了各种控制字符来改变文本的呈现。例如，字符 "Right-to-Left Override"(从右至左覆盖)U+202E 可以诱骗某些程序把 "moo\\xe2\\x80\\xaegnp.txt" 呈现为 "mootxt.png"。第二类呈现问题涉及空白字符。如果文件名中出现字符 "Zero Width Space"(零宽空格)U+200B，该名称的呈现将与没有零宽空格的名称相同。)

If two names within a naming domain have different byte sequences but render
identically, a user may be confused by it.
The kernel, in its indifference to upper level encoding schemes, permits this.
Most filesystem drivers persist the byte sequence names that are given to them
by the VFS.
(如果同一命名域内的两个名称具有不同的字节序列但呈现相同，用户可能会因此感到困惑。内核对上层编码方案漠不关心，允许这种情况。大多数文件系统驱动程序持久存储 VFS 交给它们的字节序列名称。)

Techniques for detecting confusable names are explained in great detail in
sections 4 and 5 of the
`Unicode Security Mechanisms <https://unicode.org/reports/tr39/>`_
document.
When ``xfs_scrub`` detects UTF-8 encoding in use on a system, it uses the
Unicode normalization form NFD in conjunction with the confusable name
detection component of
`libicu <https://github.com/unicode-org/icu>`_
to identify names with a directory or within a file's extended attributes that
could be confused for each other.
Names are also checked for control characters, non-rendering characters, and
mixing of bidirectional characters.
All of these potential issues are reported to the system administrator during
phase 5.
(检测易混淆名称的技术在 `Unicode Security Mechanisms <https://unicode.org/reports/tr39/>`_ 文档的第 4 和第 5 节中有非常详细的说明。当 ``xfs_scrub`` 检测到系统正在使用 UTF-8 编码时，它会将 Unicode 规范化形式 NFD 与 `libicu <https://github.com/unicode-org/icu>`_ 的易混淆名称检测组件结合使用，以识别目录内或文件扩展属性中可能彼此混淆的名称。名称还会被检查控制字符、不呈现字符以及双向字符的混用。所有这些潜在问题都会在阶段 5 中向系统管理员报告。)

Media Verification of File Data Extents
(文件数据区段的介质校验)
---------------------------------------

The system administrator can elect to initiate a media scan of all file data
blocks.
This scan after validation of all filesystem metadata (except for the summary
counters) as phase 6.
The scan starts by calling ``FS_IOC_GETFSMAP`` to scan the filesystem space map
to find areas that are allocated to file data fork extents.
Gaps between data fork extents that are smaller than 64k are treated as if
they were data fork extents to reduce the command setup overhead.
When the space map scan accumulates a region larger than 32MB, a media
verification request is sent to the disk as a directio read of the raw block
device.
(系统管理员可以选择对所有文件数据块发起一次介质扫描。这次扫描作为阶段 6，在所有文件系统元数据(摘要计数器除外)校验之后进行。扫描从调用 ``FS_IOC_GETFSMAP`` 扫描文件系统空间映射开始，以找到分配给文件数据 fork 区段的区域。小于 64k 的数据 fork 区段之间的间隙被视为数据 fork 区段，以减少命令设置开销。当空间映射扫描积累的区域大于 32MB 时，向磁盘发送一个介质校验请求，作为对原始块设备的 directio 读取。)

If the verification read fails, ``xfs_scrub`` retries with single-block reads
to narrow down the failure to the specific region of the media and recorded.
When it has finished issuing verification requests, it again uses the space
mapping ioctl to map the recorded media errors back to metadata structures
and report what has been lost.
For media errors in blocks owned by files, parent pointers can be used to
construct file paths from inode numbers for user-friendly reporting.
(如果校验读取失败，``xfs_scrub`` 会用单块读取重试，以把失败缩小到介质的特定区域并记录下来。在发出完所有校验请求之后，它再次使用空间映射 ioctl 将记录的介质错误映射回元数据结构，并报告丢失的内容。对于文件拥有的块中的介质错误，可以使用父指针从 inode 编号构造文件路径，以便进行对用户友好的报告。)

7. Conclusion and Future Work
(7. 结论与未来工作)
=============================

It is hoped that the reader of this document has followed the designs laid out
in this document and now has some familiarity with how XFS performs online
rebuilding of its metadata indices, and how filesystem users can interact with
that functionality.
Although the scope of this work is daunting, it is hoped that this guide will
make it easier for code readers to understand what has been built, for whom it
has been built, and why.
Please feel free to contact the XFS mailing list with questions.
(希望本文档的读者跟上了本文阐述的设计，现在对 XFS 如何在线重建其元数据索引、文件系统用户如何与该功能交互已有一定的了解。虽然这项工作的范围令人生畏，但希望本指南能让代码阅读者更容易理解构建了什么、为谁构建以及为什么构建。欢迎通过 XFS 邮件列表提出问题。)

XFS_IOC_EXCHANGE_RANGE
(XFS_IOC_EXCHANGE_RANGE)
----------------------

As discussed earlier, a second frontend to the atomic file mapping exchange
mechanism is a new ioctl call that userspace programs can use to commit updates
to files atomically.
This frontend has been out for review for several years now, though the
necessary refinements to online repair and lack of customer demand mean that
the proposal has not been pushed very hard.
(如前所述，原子文件映射交换机制的第二个前端是一个新的 ioctl 调用，用户空间程序可以用它原子地提交对文件的更新。这个前端已经公开评审好几年了，不过对在线修复的必要改进以及客户需求的缺乏意味着该提案没有被大力推动。)

File Content Exchanges with Regular User Files
(与常规用户文件的文件内容交换)
``````````````````````````````````````````````

As mentioned earlier, XFS has long had the ability to swap extents between
files, which is used almost exclusively by ``xfs_fsr`` to defragment files.
The earliest form of this was the fork swap mechanism, where the entire
contents of data forks could be exchanged between two files by exchanging the
raw bytes in each inode fork's immediate area.
When XFS v5 came along with self-describing metadata, this old mechanism grew
some log support to continue rewriting the owner fields of BMBT blocks during
log recovery.
When the reverse mapping btree was later added to XFS, the only way to maintain
the consistency of the fork mappings with the reverse mapping index was to
develop an iterative mechanism that used deferred bmap and rmap operations to
swap mappings one at a time.
This mechanism is identical to steps 2-3 from the procedure above except for
the new tracking items, because the atomic file mapping exchange mechanism is
an iteration of an existing mechanism and not something totally novel.
For the narrow case of file defragmentation, the file contents must be
identical, so the recovery guarantees are not much of a gain.
(如前所述，XFS 很早就有在文件之间交换区段的能力，这几乎只被 ``xfs_fsr`` 用来整理文件碎片。其最早的形式是 fork 交换机制，通过交换每个 inode fork 内联区域中的原始字节，可以在两个文件之间交换数据 fork 的全部内容。当 XFS v5 带来自描述元数据时，这一旧机制获得了一些日志支持，以便在日志恢复期间继续重写 BMBT 块的属主字段。当反向映射 btree 后来加入 XFS 时，维护 fork 映射与反向映射索引一致性的唯一途径是开发一个迭代机制，使用延迟的 bmap 和 rmap 操作一次交换一个映射。除了新的跟踪项之外，这一机制与上述过程的步骤 2-3 相同，因为原子文件映射交换机制是现有机制的一次迭代，并非完全新颖的东西。对于文件碎片整理这一狭窄场景，文件内容必须相同，所以恢复保证并没有多少收益。)

Atomic file content exchanges are much more flexible than the existing swapext
implementations because it can guarantee that the caller never sees a mix of
old and new contents even after a crash, and it can operate on two arbitrary
file fork ranges.
The extra flexibility enables several new use cases:
(原子文件内容交换比现有的 swapext 实现灵活得多，因为它可以保证即使在崩溃之后调用者也绝不会看到新旧内容的混合，并且它可以对两个任意的文件 fork 范围进行操作。这种额外的灵活性带来了几个新的用例：)

- **Atomic commit of file writes**: A userspace process opens a file that it
  wants to update.
  Next, it opens a temporary file and calls the file clone operation to reflink
  the first file's contents into the temporary file.
  Writes to the original file should instead be written to the temporary file.
  Finally, the process calls the atomic file mapping exchange system call
  (``XFS_IOC_EXCHANGE_RANGE``) to exchange the file contents, thereby
  committing all of the updates to the original file, or none of them.
  (**文件写入的原子提交**：一个用户空间进程打开它想要更新的文件。接下来，它打开一个临时文件并调用文件克隆操作，将第一个文件的内容 reflink 到临时文件中。对原始文件的写入应当改为写入临时文件。最后，该进程调用原子文件映射交换系统调用(``XFS_IOC_EXCHANGE_RANGE``)来交换文件内容，从而将对原始文件的全部更新提交，或者一个也不提交。)

.. _exchrange_if_unchanged:

- **Transactional file updates**: The same mechanism as above, but the caller
  only wants the commit to occur if the original file's contents have not
  changed.
  To make this happen, the calling process snapshots the file modification and
  change timestamps of the original file before reflinking its data to the
  temporary file.
  When the program is ready to commit the changes, it passes the timestamps
  into the kernel as arguments to the atomic file mapping exchange system call.
  The kernel only commits the changes if the provided timestamps match the
  original file.
  A new ioctl (``XFS_IOC_COMMIT_RANGE``) is provided to perform this.
  (**事务性文件更新**：与上面相同的机制，但调用者只在原始文件的内容没有改变时才希望提交发生。为实现这一点，调用进程在将其数据 reflink 到临时文件之前，先对原始文件的文件修改时间和变更时间戳做快照。当程序准备好提交更改时，它把时间戳作为原子文件映射交换系统调用的参数传入内核。只有当提供的时间戳与原始文件匹配时，内核才提交更改。为此提供了一个新的 ioctl(``XFS_IOC_COMMIT_RANGE``)。)

- **Emulation of atomic block device writes**: Export a block device with a
  logical sector size matching the filesystem block size to force all writes
  to be aligned to the filesystem block size.
  Stage all writes to a temporary file, and when that is complete, call the
  atomic file mapping exchange system call with a flag to indicate that holes
  in the temporary file should be ignored.
  This emulates an atomic device write in software, and can support arbitrary
  scattered writes.
  (**模拟原子块设备写入**：导出一个逻辑扇区大小与文件系统块大小匹配的块设备，以强制所有写入对齐到文件系统块大小。将所有写入暂存到一个临时文件，完成后，调用原子文件映射交换系统调用并带一个标志，指示临时文件中的洞应被忽略。这用软件模拟了原子的设备写入，并可以支持任意的分散写入。)

(This functionality was merged into mainline as of 2025)
(截至 2025 年，此功能已合并到主线。)

Vectorized Scrub
(向量化 scrub)
----------------

As it turns out, the :ref:`refactoring <scrubrepair>` of repair items mentioned
earlier was a catalyst for enabling a vectorized scrub system call.
Since 2018, the cost of making a kernel call has increased considerably on some
systems to mitigate the effects of speculative execution attacks.
This incentivizes program authors to make as few system calls as possible to
reduce the number of times an execution path crosses a security boundary.
(事实证明，前面提到的修复项的 :ref:`重构 <scrubrepair>` 是启用向量化 scrub 系统调用的催化剂。自 2018 年以来，为了缓解推测执行攻击的影响，某些系统上进行内核调用的成本大幅增加。这促使程序作者尽可能少地进行系统调用，以减少执行路径跨越安全边界的次数。)

With vectorized scrub, userspace pushes to the kernel the identity of a
filesystem object, a list of scrub types to run against that object, and a
simple representation of the data dependencies between the selected scrub
types.
The kernel executes as much of the caller's plan as it can until it hits a
dependency that cannot be satisfied due to a corruption, and tells userspace
how much was accomplished.
It is hoped that ``io_uring`` will pick up enough of this functionality that
online fsck can use that instead of adding a separate vectored scrub system
call to XFS.
(使用向量化 scrub，用户空间向内核推送一个文件系统对象的标识、要对该对象运行的一列 scrub 类型，以及所选 scrub 类型之间数据依赖的简单表示。内核尽可能多地执行调用者的计划，直到遇到因损坏而无法满足的依赖，并告诉用户空间完成了多少。希望 ``io_uring`` 能吸纳足够多的这类功能，使在线 fsck 可以使用它，而不必向 XFS 添加单独的向量化 scrub 系统调用。)

(This functionality was merged into mainline as of 2025)
(截至 2025 年，此功能已合并到主线。)

Quality of Service Targets for Scrub
(scrub 的服务质量目标)
------------------------------------

One serious shortcoming of the online fsck code is that the amount of time that
it can spend in the kernel holding resource locks is basically unbounded.
Userspace is allowed to send a fatal signal to the process which will cause
``xfs_scrub`` to exit when it reaches a good stopping point, but there's no way
for userspace to provide a time budget to the kernel.
Given that the scrub codebase has helpers to detect fatal signals, it shouldn't
be too much work to allow userspace to specify a timeout for a scrub/repair
operation and abort the operation if it exceeds budget.
However, most repair functions have the property that once they begin to touch
ondisk metadata, the operation cannot be cancelled cleanly, after which a QoS
timeout is no longer useful.
(在线 fsck 代码的一个严重缺点是它在内核中持有资源锁的时间基本上没有上限。允许用户空间向进程发送致命信号，使 ``xfs_scrub`` 在到达合适的停止点时退出，但用户空间没有办法向内核提供时间预算。鉴于 scrub 代码库有检测致命信号的辅助函数，允许用户空间为 scrub/repair 操作指定超时并在超出预算时中止操作，工作量应该不会太大。然而，大多数修复函数有一个特性：一旦开始接触磁盘元数据，操作就无法干净地取消，此后 QoS 超时就不再有用。)

Defragmenting Free Space
(空闲空间碎片整理)
------------------------

Over the years, many XFS users have requested the creation of a program to
clear a portion of the physical storage underlying a filesystem so that it
becomes a contiguous chunk of free space.
Call this free space defragmenter ``clearspace`` for short.
(多年来，许多 XFS 用户请求创建一个程序来清理文件系统底层的一部分物理存储，使其成为一大块连续的空闲空间。这个空闲空间碎片整理程序简称为 ``clearspace``。)

The first piece the ``clearspace`` program needs is the ability to read the
reverse mapping index from userspace.
This already exists in the form of the ``FS_IOC_GETFSMAP`` ioctl.
The second piece it needs is a new fallocate mode
(``FALLOC_FL_MAP_FREE_SPACE``) that allocates the free space in a region and
maps it to a file.
Call this file the "space collector" file.
The third piece is the ability to force an online repair.
(``clearspace`` 程序需要的第一件东西是从用户空间读取反向映射索引的能力。这已经以 ``FS_IOC_GETFSMAP`` ioctl 的形式存在。它需要的第二件东西是一个新的 fallocate 模式(``FALLOC_FL_MAP_FREE_SPACE``)，用于分配一个区域中的空闲空间并将其映射到一个文件。把这个文件称为"空间收集器"文件。第三件东西是强制执行在线修复的能力。)

To clear all the metadata out of a portion of physical storage, clearspace
uses the new fallocate map-freespace call to map any free space in that region
to the space collector file.
Next, clearspace finds all metadata blocks in that region by way of
``GETFSMAP`` and issues forced repair requests on the data structure.
This often results in the metadata being rebuilt somewhere that is not being
cleared.
After each relocation, clearspace calls the "map free space" function again to
collect any newly freed space in the region being cleared.
(要把所有元数据从一部分物理存储中清走，clearspace 使用新的 fallocate map-freespace 调用，把该区域中的任何空闲空间映射到空间收集器文件。接着，clearspace 通过 ``GETFSMAP`` 找到该区域中的所有元数据块，并对数据结构发出强制修复请求。这常常导致元数据在未被清理的某个地方被重建。每次重定位之后，clearspace 再次调用"map free space"函数，以收集正在清理的区域中新释放的任何空间。)

To clear all the file data out of a portion of the physical storage, clearspace
uses the FSMAP information to find relevant file data blocks.
Having identified a good target, it uses the ``FICLONERANGE`` call on that part
of the file to try to share the physical space with a dummy file.
Cloning the extent means that the original owners cannot overwrite the
contents; any changes will be written somewhere else via copy-on-write.
Clearspace makes its own copy of the frozen extent in an area that is not being
cleared, and uses ``FIEDEUPRANGE`` (or the :ref:`atomic file content exchanges
<exchrange_if_unchanged>` feature) to change the target file's data extent
mapping away from the area being cleared.
When all other mappings have been moved, clearspace reflinks the space into the
space collector file so that it becomes unavailable.
(要把所有文件数据从一部分物理存储中清走，clearspace 使用 FSMAP 信息找到相关的文件数据块。确定一个好的目标后，它对该文件的那部分使用 ``FICLONERANGE`` 调用，尝试与一个虚拟(dummy)文件共享物理空间。克隆该区段意味着原始属主不能覆写内容；任何更改都会通过写时复制写到别处。clearspace 在未被清理的区域中制作被冻结区段自己的副本，并使用 ``FIEDEUPRANGE``(或 :ref:`原子文件内容交换 <exchrange_if_unchanged>` 功能)把目标文件的数据区段映射从被清理的区域移开。当所有其他映射都被移走后，clearspace 将该空间 reflink 到空间收集器文件，使其变得不可用。)

There are further optimizations that could apply to the above algorithm.
To clear a piece of physical storage that has a high sharing factor, it is
strongly desirable to retain this sharing factor.
In fact, these extents should be moved first to maximize sharing factor after
the operation completes.
To make this work smoothly, clearspace needs a new ioctl
(``FS_IOC_GETREFCOUNTS``) to report reference count information to userspace.
With the refcount information exposed, clearspace can quickly find the longest,
most shared data extents in the filesystem, and target them first.
(上述算法还可以应用进一步的优化。要清理一块具有高共享因子的物理存储，强烈希望保留这一共享因子。事实上，这些区段应当先被移动，以便在操作完成后最大化共享因子。为了顺利实现这一点，clearspace 需要一个新的 ioctl(``FS_IOC_GETREFCOUNTS``)向用户空间报告引用计数信息。有了引用计数信息，clearspace 可以快速找到文件系统中最长、共享最多的数据区段，并优先处理它们。)

**Future Work Question**: How might the filesystem move inode chunks?
(**未来工作问题**：文件系统如何移动 inode 块？)

*Answer*: To move inode chunks, Dave Chinner constructed a prototype program
that creates a new file with the old contents and then locklessly runs around
the filesystem updating directory entries.
The operation cannot complete if the filesystem goes down.
That problem isn't totally insurmountable: create an inode remapping table
hidden behind a jump label, and a log item that tracks the kernel walking the
filesystem to update directory entries.
The trouble is, the kernel can't do anything about open files, since it cannot
revoke them.
(*回答*：为了移动 inode 块，Dave Chinner 构建了一个原型程序，它创建一个具有旧内容的新文件，然后无锁地在文件系统中奔走更新目录项。如果文件系统宕机，该操作无法完成。这个问题并非完全不可逾越：创建一个藏在跳转标签后面的 inode 重映射表，以及一个跟踪内核遍历文件系统更新目录项的日志项。麻烦在于，内核对已打开的文件无能为力，因为它无法吊销(revoke)它们。)

**Future Work Question**: Can static keys be used to minimize the cost of
supporting ``revoke()`` on XFS files?
(**未来工作问题**：能否使用静态键来把在 XFS 文件上支持 ``revoke()`` 的成本降到最低？)

*Answer*: Yes.
Until the first revocation, the bailout code need not be in the call path at
all.
(*回答*：可以。在第一次吊销之前， bailout 代码完全不必存在于调用路径中。)

The relevant patchsets are the
`kernel freespace defrag
<https://git.kernel.org/pub/scm/linux/kernel/git/djwong/xfs-linux.git/log/?h=defrag-freespace>`_
and
`userspace freespace defrag
<https://git.kernel.org/pub/scm/linux/kernel/git/djwong/xfsprogs-dev.git/log/?h=defrag-freespace>`_
series.
(相关的补丁集是 `kernel freespace defrag <https://git.kernel.org/pub/scm/linux/kernel/git/djwong/xfs-linux.git/log/?h=defrag-freespace>`_ 和 `userspace freespace defrag <https://git.kernel.org/pub/scm/linux/kernel/git/djwong/xfsprogs-dev.git/log/?h=defrag-freespace>`_ 系列。)

Shrinking Filesystems
(收缩文件系统)
---------------------

Removing the end of the filesystem ought to be a simple matter of evacuating
the data and metadata at the end of the filesystem, and handing the freed space
to the shrink code.
That requires an evacuation of the space at end of the filesystem, which is a
use of free space defragmentation!
(移除文件系统的末尾应当只是把文件系统末端的数据和元数据迁出，然后把释放的空间交给收缩代码这么简单。这需要对文件系统末尾的空间进行迁移，而这正是空闲空间碎片整理的一种用途！)
