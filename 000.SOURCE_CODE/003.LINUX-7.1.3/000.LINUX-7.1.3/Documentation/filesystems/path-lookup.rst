===============
Pathname lookup
===============

(路径名查找)

This write-up is based on three articles published at lwn.net:
(本文基于发表于 lwn.net 的三篇文章：)

- <https://lwn.net/Articles/649115/> Pathname lookup in Linux
  (Linux 中的路径名查找)
- <https://lwn.net/Articles/649729/> RCU-walk: faster pathname lookup in Linux
  (RCU-walk：Linux 中更快的路径名查找)
- <https://lwn.net/Articles/650786/> A walk among the symlinks
  (符号链接之间的漫步)

Written by Neil Brown with help from Al Viro and Jon Corbet.
It has subsequently been updated to reflect changes in the kernel
including:
(本文由 Neil Brown 撰写，Al Viro 和 Jon Corbet 协助。此后已根据内核中的变化进行了更新，包括：)

- per-directory parallel name lookup.
  (按目录并行的名称查找。)
- ``openat2()`` resolution restriction flags.
  (openat2() 的解析限制标志。)

Introduction to pathname lookup
(路径名查找简介)
===============================

The most obvious aspect of pathname lookup, which very little
exploration is needed to discover, is that it is complex.  There are
many rules, special cases, and implementation alternatives that all
combine to confuse the unwary reader.  Computer science has long been
acquainted with such complexity and has tools to help manage it.  One
tool that we will make extensive use of is "divide and conquer".  For
the early parts of the analysis we will divide off symlinks - leaving
them until the final part.  Well before we get to symlinks we have
another major division based on the VFS's approach to locking which
will allow us to review "REF-walk" and "RCU-walk" separately.  But we
are getting ahead of ourselves.  There are some important low level
distinctions we need to clarify first.
(路径名查找最明显的一点——几乎不需要什么探索就能发现——就是它非常复杂。有许多规则、特殊情况以及各种实现上的备选方案，它们组合在一起足以让不设防的读者感到困惑。计算机科学早已见识过这种复杂性，并拥有帮助管理它的工具。我们将广泛使用的一个工具是"分而治之"。在分析的早期部分，我们将把符号链接划分出去——留到最后部分再处理。在讲到符号链接之前很久，我们还有另一个基于 VFS 加锁方法的重大划分，它使我们能够分别考察"REF-walk"和"RCU-walk"。不过我们有点操之过急了。首先需要澄清一些重要的低层区别。)

There are two sorts of ...
(有两类……)
--------------------------

.. _openat: http://man7.org/linux/man-pages/man2/openat.2.html

Pathnames (sometimes "file names"), used to identify objects in the
filesystem, will be familiar to most readers.  They contain two sorts
of elements: "slashes" that are sequences of one or more "``/``"
characters, and "components" that are sequences of one or more
non-"``/``" characters.  These form two kinds of paths.  Those that
start with slashes are "absolute" and start from the filesystem root.
The others are "relative" and start from the current directory, or
from some other location specified by a file descriptor given to
"``*at()``" system calls such as `openat() <openat_>`_.
(用于标识文件系统中对象的路径名(有时称"文件名")对大多数读者来说并不陌生。它们包含两类元素："斜杠"是由一个或多个"/"字符组成的序列，"组件"则是由一个或多个非"/"字符组成的序列。这构成了两种路径。以斜杠开头的是"绝对"路径，从文件系统根目录开始；其余的是"相对"路径，从当前目录开始，或从某个其他位置开始，该位置由传给诸如 openat() 之类的 ``*at()`` 系统调用的文件描述符指定。)

.. _execveat: http://man7.org/linux/man-pages/man2/execveat.2.html

It is tempting to describe the second kind as starting with a
component, but that isn't always accurate: a pathname can lack both
slashes and components, it can be empty, in other words.  This is
generally forbidden in POSIX, but some of those "``*at()``" system calls
in Linux permit it when the ``AT_EMPTY_PATH`` flag is given.  For
example, if you have an open file descriptor on an executable file you
can execute it by calling `execveat() <execveat_>`_ passing
the file descriptor, an empty path, and the ``AT_EMPTY_PATH`` flag.
(人们很容易把第二种描述为以组件开头，但这并不总是准确：路径名可以既没有斜杠也没有组件，换句话说，它可以是空的。这在 POSIX 中通常是被禁止的，但 Linux 中某些 ``*at()`` 系统调用在给出 AT_EMPTY_PATH 标志时允许这样做。例如，如果你在一个可执行文件上有一个已打开的文件描述符，就可以调用 execveat() 并传入该文件描述符、一个空路径以及 AT_EMPTY_PATH 标志来执行它。)

These paths can be divided into two sections: the final component and
everything else.  The "everything else" is the easy bit.  In all cases
it must identify a directory that already exists, otherwise an error
such as ``ENOENT`` or ``ENOTDIR`` will be reported.
(这些路径可以分成两部分：最后一个组件和其余部分。"其余部分"比较简单。在所有情况下，它都必须标识一个已经存在的目录，否则将报告诸如 ENOENT 或 ENOTDIR 之类的错误。)

The final component is not so simple.  Not only do different system
calls interpret it quite differently (e.g. some create it, some do
not), but it might not even exist: neither the empty pathname nor the
pathname that is just slashes have a final component.  If it does
exist, it could be "``.``" or "``..``" which are handled quite differently
from other components.
(最后一个组件则不那么简单。不仅不同的系统调用对它的解释大相径庭(例如有些会创建它，有些不会)，而且它甚至可能不存在：空路径名和仅由斜杠组成的路径名都没有最后一个组件。如果它存在，它可能是"."或".."，这两者的处理方式与其他组件大不相同。)

.. _POSIX: https://pubs.opengroup.org/onlinepubs/9699919799/basedefs/V1_chap04.html#tag_04_12

If a pathname ends with a slash, such as "``/tmp/foo/``" it might be
tempting to consider that to have an empty final component.  In many
ways that would lead to correct results, but not always.  In
particular, ``mkdir()`` and ``rmdir()`` each create or remove a directory named
by the final component, and they are required to work with pathnames
ending in "``/``".  According to POSIX_:
(如果路径名以斜杠结尾，比如"/tmp/foo/"，人们可能倾向于认为它有一个空的最后组件。在很多情况下这会得到正确的结果，但并非总是如此。特别是，mkdir() 和 rmdir() 各自创建或移除以最后组件命名的目录，而它们被要求能够处理以"/"结尾的路径名。根据 POSIX：)

  A pathname that contains at least one non-<slash> character and
  that ends with one or more trailing <slash> characters shall not
  be resolved successfully unless the last pathname component before
  the trailing <slash> characters names an existing directory or a
  directory entry that is to be created for a directory immediately
  after the pathname is resolved.
  (包含至少一个非<slash>字符并以一个或多个结尾<slash>字符结束的路径名不得被成功解析，除非结尾<slash>字符之前的最后一个路径名组件命名了一个已存在的目录，或者一个在路径名解析完成后即将为某目录创建的目录项。)

The Linux pathname walking code (mostly in ``fs/namei.c``) deals with
all of these issues: breaking the path into components, handling the
"everything else" quite separately from the final component, and
checking that the trailing slash is not used where it isn't
permitted.  It also addresses the important issue of concurrent
access.
(Linux 的路径遍历(path walk)代码(主要在 fs/namei.c 中)处理所有这些问题：把路径拆分成组件，将"其余部分"与最后一个组件完全分开处理，并检查结尾斜杠没有用在不允许的地方。它还处理并发访问这一重要问题。)

While one process is looking up a pathname, another might be making
changes that affect that lookup.  One fairly extreme case is that if
"a/b" were renamed to "a/c/b" while another process were looking up
"a/b/..", that process might successfully resolve on "a/c".
Most races are much more subtle, and a big part of the task of
pathname lookup is to prevent them from having damaging effects.  Many
of the possible races are seen most clearly in the context of the
"dcache" and an understanding of that is central to understanding
pathname lookup.
(当一个进程正在查找路径名时，另一个进程可能正在做出影响该查找的更改。一个相当极端的例子是：如果"a/b"在另一个进程正在查找"a/b/.."时被重命名为"a/c/b"，那么该进程可能会成功地解析到"a/c"。大多数竞争要微妙得多，路径名查找任务的一大部分就是防止它们产生破坏性影响。许多可能的竞争在"dcache"的上下文中看得最清楚，理解 dcache 是理解路径名查找的核心。)

More than just a cache
(不仅仅是缓存)
----------------------

The "dcache" caches information about names in each filesystem to
make them quickly available for lookup.  Each entry (known as a
"dentry") contains three significant fields: a component name, a
pointer to a parent dentry, and a pointer to the "inode" which
contains further information about the object in that parent with
the given name.  The inode pointer can be ``NULL`` indicating that the
name doesn't exist in the parent.  While there can be linkage in the
dentry of a directory to the dentries of the children, that linkage is
not used for pathname lookup, and so will not be considered here.
("dcache"缓存每个文件系统中关于名称的信息，使它们能够被快速查找。每个条目(称为"dentry")包含三个重要字段：一个组件名、一个指向父 dentry 的指针，以及一个指向"inode"的指针，该 inode 包含有关父目录中具有该给定名称的对象的进一步信息。inode 指针可以为 NULL，表示该名称在父目录中不存在。虽然目录的 dentry 中可以有指向其子 dentry 的链接，但该链接并不用于路径名查找，因此这里不予考虑。)

The dcache has a number of uses apart from accelerating lookup.  One
that will be particularly relevant is that it is closely integrated
with the mount table that records which filesystem is mounted where.
What the mount table actually stores is which dentry is mounted on top
of which other dentry.
(除了加速查找之外，dcache 还有许多其他用途。其中一个与此特别相关：它与记录哪个文件系统挂载在哪里的挂载表紧密集成。挂载表实际存储的是哪个 dentry 挂载在哪个其他 dentry 之上。)

When considering the dcache, we have another of our "two types"
distinctions: there are two types of filesystems.
(在考虑 dcache 时，我们有另一个"两种类型"式的区分：文件系统有两种类型。)

Some filesystems ensure that the information in the dcache is always
completely accurate (though not necessarily complete).  This can allow
the VFS to determine if a particular file does or doesn't exist
without checking with the filesystem, and means that the VFS can
protect the filesystem against certain races and other problems.
These are typically "local" filesystems such as ext3, XFS, and Btrfs.
(一些文件系统确保 dcache 中的信息始终完全准确(尽管不一定完整)。这可以让 VFS 无需向文件系统核实就能判断某个特定文件是否存在，也意味着 VFS 可以保护文件系统免受某些竞争和其他问题的影响。这类文件系统通常是"本地"文件系统，例如 ext3、XFS 和 Btrfs。)

Other filesystems don't provide that guarantee because they cannot.
These are typically filesystems that are shared across a network,
whether remote filesystems like NFS and 9P, or cluster filesystems
like ocfs2 or cephfs.  These filesystems allow the VFS to revalidate
cached information, and must provide their own protection against
awkward races.  The VFS can detect these filesystems by the
``DCACHE_OP_REVALIDATE`` flag being set in the dentry.
(其他文件系统无法提供这种保证，因为它们做不到。它们通常是通过网络共享的文件系统，无论是像 NFS 和 9P 这样的远程文件系统，还是像 ocfs2 或 cephfs 这样的集群文件系统。这些文件系统允许 VFS 重新验证缓存的信息，并且必须针对棘手的竞争提供自己的保护。VFS 可以通过 dentry 中设置的 DCACHE_OP_REVALIDATE 标志检测出这些文件系统。)

REF-walk: simple concurrency management with refcounts and spinlocks
(REF-walk：基于引用计数和自旋锁的简单并发管理)
--------------------------------------------------------------------

With all of those divisions carefully classified, we can now start
looking at the actual process of walking along a path.  In particular
we will start with the handling of the "everything else" part of a
pathname, and focus on the "REF-walk" approach to concurrency
management.  This code is found in the ``link_path_walk()`` function, if
you ignore all the places that only run when "``LOOKUP_RCU``"
(indicating the use of RCU-walk) is set.
(在把所有这些划分仔细归类之后，我们现在可以开始考察沿路径行走的实际过程了。特别地，我们将从路径名中"其余部分"的处理开始，并聚焦于"REF-walk"这种并发管理方法。如果忽略所有只在设置了"LOOKUP_RCU"(表示使用 RCU-walk)时才运行的地方，这段代码位于 link_path_walk() 函数中。)

.. _Meet the Lockers: https://lwn.net/Articles/453685/

REF-walk is fairly heavy-handed with locks and reference counts.  Not
as heavy-handed as in the old "big kernel lock" days, but certainly not
afraid of taking a lock when one is needed.  It uses a variety of
different concurrency controls.  A background understanding of the
various primitives is assumed, or can be gleaned from elsewhere such
as in `Meet the Lockers`_.
(REF-walk 对锁和引用计数的使用相当"重手"。虽然不像过去"大内核锁"时代那样重手，但它在需要时肯定不害怕获取锁。它使用多种不同的并发控制手段。我们假定读者已对这些原语有背景了解，或者可以从别处(如 Meet the Lockers 一文)获得相关了解。)

The locking mechanisms used by REF-walk include:
(REF-walk 使用的加锁机制包括：)

dentry->d_lockref
~~~~~~~~~~~~~~~~~

This uses the lockref primitive to provide both a spinlock and a
reference count.  The special-sauce of this primitive is that the
conceptual sequence "lock; inc_ref; unlock;" can often be performed
with a single atomic memory operation.
(它使用 lockref 原语同时提供自旋锁和引用计数。这个原语的独门秘技在于：概念上的"lock; inc_ref; unlock;"序列通常可以用单次原子内存操作完成。)

Holding a reference on a dentry ensures that the dentry won't suddenly
be freed and used for something else, so the values in various fields
will behave as expected.  It also protects the ``->d_inode`` reference
to the inode to some extent.
(持有对 dentry 的引用可以确保该 dentry 不会突然被释放并被挪作他用，因此各字段中的值会表现得符合预期。它也在一定程度上保护了指向 inode 的 ->d_inode 引用。)

The association between a dentry and its inode is fairly permanent.
For example, when a file is renamed, the dentry and inode move
together to the new location.  When a file is created the dentry will
initially be negative (i.e. ``d_inode`` is ``NULL``), and will be assigned
to the new inode as part of the act of creation.
(dentry 与其 inode 之间的关联相当持久。例如，当一个文件被重命名时，dentry 和 inode 会一起移动到新位置。创建文件时，dentry 最初是负的(即 d_inode 为 NULL)，并会在创建动作中指派给新的 inode。)

When a file is deleted, this can be reflected in the cache either by
setting ``d_inode`` to ``NULL``, or by removing it from the hash table
(described shortly) used to look up the name in the parent directory.
If the dentry is still in use the second option is used as it is
perfectly legal to keep using an open file after it has been deleted
and having the dentry around helps.  If the dentry is not otherwise in
use (i.e. if the refcount in ``d_lockref`` is one), only then will
``d_inode`` be set to ``NULL``.  Doing it this way is more efficient for a
very common case.
(当文件被删除时，可以通过两种方式在缓存中反映这一点：要么把 d_inode 设为 NULL，要么把它从(稍后描述的)用于在父目录中查找名称的哈希表中移除。如果 dentry 仍在使用中，则采用第二种方式，因为在文件被删除后继续使用已打开的文件是完全合法的，保留 dentry 会有所帮助。如果 dentry 没有其他使用者(即 d_lockref 中的引用计数为一)，那时才会把 d_inode 设为 NULL。对一个很常见的情况而言，这样做效率更高。)

So as long as a counted reference is held to a dentry, a non-``NULL`` ``->d_inode``
value will never be changed.
(因此，只要持有对 dentry 的计数引用，非 NULL 的 ->d_inode 值就永远不会被改变。)

dentry->d_lock
~~~~~~~~~~~~~~

``d_lock`` is a synonym for the spinlock that is part of ``d_lockref`` above.
For our purposes, holding this lock protects against the dentry being
renamed or unlinked.  In particular, its parent (``d_parent``), and its
name (``d_name``) cannot be changed, and it cannot be removed from the
dentry hash table.
(d_lock 是上面 d_lockref 中那个自旋锁的同义词。就我们的目的而言，持有这个锁可以防止 dentry 被重命名或解除链接。特别是，它的父项(d_parent)和名称(d_name)不能被更改，它也不能从 dentry 哈希表中被移除。)

When looking for a name in a directory, REF-walk takes ``d_lock`` on
each candidate dentry that it finds in the hash table and then checks
that the parent and name are correct.  So it doesn't lock the parent
while searching in the cache; it only locks children.
(在目录中查找名称时，REF-walk 会对它在哈希表中找到的每个候选 dentry 获取 d_lock，然后检查父项和名称是否正确。所以在缓存中搜索时它并不锁住父项；它只锁住子项。)

When looking for the parent for a given name (to handle "``..``"),
REF-walk can take ``d_lock`` to get a stable reference to ``d_parent``,
but it first tries a more lightweight approach.  As seen in
``dget_parent()``, if a reference can be claimed on the parent, and if
subsequently ``d_parent`` can be seen to have not changed, then there is
no need to actually take the lock on the child.
(在为给定名称查找父项时(为了处理"..")，REF-walk 可以获取 d_lock 以获得对 d_parent 的稳定引用，但它首先会尝试一种更轻量的方法。如 dget_parent() 所示，如果能够在父项上主张到引用，并且随后能看到 d_parent 没有发生变化，那么就无需真正对子项加锁。)

rename_lock
~~~~~~~~~~~

Looking up a given name in a given directory involves computing a hash
from the two values (the name and the dentry of the directory),
accessing that slot in a hash table, and searching the linked list
that is found there.
(在给定目录中查找给定名称包括：由两个值(名称和目录的 dentry)计算哈希，访问哈希表中的那个槽位，然后搜索在那里找到的链表。)

When a dentry is renamed, the name and the parent dentry can both
change so the hash will almost certainly change too.  This would move the
dentry to a different chain in the hash table.  If a filename search
happened to be looking at a dentry that was moved in this way,
it might end up continuing the search down the wrong chain,
and so miss out on part of the correct chain.
(当 dentry 被重命名时，名称和父 dentry 都可能改变，因此哈希值几乎肯定也会改变。这会把该 dentry 移到哈希表中的另一条链上。如果一次文件名搜索恰好在看的是以这种方式被移动的 dentry，它可能最终沿着错误的链继续搜索，从而错过正确链的一部分。)

The name-lookup process (``d_lookup()``) does *not* try to prevent this
from happening, but only to detect when it happens.
``rename_lock`` is a seqlock that is updated whenever any dentry is
renamed.  If ``d_lookup`` finds that a rename happened while it
unsuccessfully scanned a chain in the hash table, it simply tries
again.
(名称查找过程(d_lookup())并*不*试图阻止这种情况发生，而只是检测它何时发生。rename_lock 是一个顺序锁(seqlock)，只要任何 dentry 被重命名它就会更新。如果 d_lookup 发现自己没能成功扫描哈希表中的某条链是因为发生了重命名，它就简单地重试。)

``rename_lock`` is also used to detect and defend against potential attacks
against ``LOOKUP_BENEATH`` and ``LOOKUP_IN_ROOT`` when resolving ".." (where
the parent directory is moved outside the root, bypassing the ``path_equal()``
check). If ``rename_lock`` is updated during the lookup and the path encounters
a "..", a potential attack occurred and ``handle_dots()`` will bail out with
``-EAGAIN``.
(rename_lock 还用于在解析".."时检测和防范针对 LOOKUP_BENEATH 和 LOOKUP_IN_ROOT 的潜在攻击(即父目录被移到根之外、绕过 path_equal() 检查的情形)。如果 rename_lock 在查找期间被更新而路径又遇到".."，则说明可能发生了攻击，handle_dots() 将以 -EAGAIN 退出。)

inode->i_rwsem
~~~~~~~~~~~~~~

``i_rwsem`` is a read/write semaphore that serializes all changes to a particular
directory.  This ensures that, for example, an ``unlink()`` and a ``rename()``
cannot both happen at the same time.  It also keeps the directory
stable while the filesystem is asked to look up a name that is not
currently in the dcache or, optionally, when the list of entries in a
directory is being retrieved with ``readdir()``.
(i_rwsem 是一个读/写信号量，它把针对特定目录的所有更改串行化。这确保了例如 unlink() 和 rename() 不能同时发生。当要求文件系统查找一个当前不在 dcache 中的名称时，或者(可选地)当正在用 readdir() 检索目录的条目列表时，它还能保持目录稳定。)

This has a complementary role to that of ``d_lock``: ``i_rwsem`` on a
directory protects all of the names in that directory, while ``d_lock``
on a name protects just one name in a directory.  Most changes to the
dcache hold ``i_rwsem`` on the relevant directory inode and briefly take
``d_lock`` on one or more the dentries while the change happens.  One
exception is when idle dentries are removed from the dcache due to
memory pressure.  This uses ``d_lock``, but ``i_rwsem`` plays no role.
(它与 d_lock 的角色互补：目录上的 i_rwsem 保护该目录中的所有名称，而对某个名称的 d_lock 只保护目录中的一个名称。对 dcache 的大多数更改都会持有相关目录 inode 上的 i_rwsem，并在更改发生期间短暂地对一个或多个 dentry 获取 d_lock。一个例外是空闲 dentry 因内存压力而从 dcache 中移除的情况。这种情况使用 d_lock，而 i_rwsem 不起任何作用。)

The semaphore affects pathname lookup in two distinct ways.  Firstly it
prevents changes during lookup of a name in a directory.  ``walk_component()`` uses
``lookup_fast()`` first which, in turn, checks to see if the name is in the cache,
using only ``d_lock`` locking.  If the name isn't found, then ``walk_component()``
falls back to ``lookup_slow()`` which takes a shared lock on ``i_rwsem``, checks again that
the name isn't in the cache, and then calls in to the filesystem to get a
definitive answer.  A new dentry will be added to the cache regardless of
the result.
(这个信号量以两种截然不同的方式影响路径名查找。首先，它防止在目录中查找名称期间发生更改。walk_component() 首先使用 lookup_fast()，后者仅使用 d_lock 加锁来检查名称是否在缓存中。如果找不到该名称，walk_component() 就退回到 lookup_slow()，后者在 i_rwsem 上获取共享锁，再次确认名称不在缓存中，然后调用文件系统以获得确定的答案。无论结果如何，一个新的 dentry 都会被加入缓存。)

Secondly, when pathname lookup reaches the final component, it will
sometimes need to take an exclusive lock on ``i_rwsem`` before performing the last lookup so
that the required exclusion can be achieved.  How path lookup chooses
to take, or not take, ``i_rwsem`` is one of the
issues addressed in a subsequent section.
(其次，当路径名查找到达最后一个组件时，有时需要在执行最后一次查找之前获取 i_rwsem 上的排他锁，以便实现所需的互斥。路径查找如何选择获取还是不获取 i_rwsem，是后续章节要讨论的问题之一。)

If two threads attempt to look up the same name at the same time - a
name that is not yet in the dcache - the shared lock on ``i_rwsem`` will
not prevent them both adding new dentries with the same name.  As this
would result in confusion an extra level of interlocking is used,
based around a secondary hash table (``in_lookup_hashtable``) and a
per-dentry flag bit (``DCACHE_PAR_LOOKUP``).
(如果两个线程同时尝试查找同一个名称——一个尚不在 dcache 中的名称——i_rwsem 上的共享锁并不能阻止它们两者都添加同名的新 dentry。由于这会造成混乱，因此使用了一个额外的互锁层级，它围绕一个二级哈希表(in_lookup_hashtable)和一个每 dentry 的标志位(DCACHE_PAR_LOOKUP)构建。)

To add a new dentry to the cache while only holding a shared lock on
``i_rwsem``, a thread must call ``d_alloc_parallel()``.  This allocates a
dentry, stores the required name and parent in it, checks if there
is already a matching dentry in the primary or secondary hash
tables, and if not, stores the newly allocated dentry in the secondary
hash table, with ``DCACHE_PAR_LOOKUP`` set.
(要在只持有 i_rwsem 共享锁的情况下向缓存添加新 dentry，线程必须调用 d_alloc_parallel()。它会分配一个 dentry，把所需的名称和父项存入其中，检查主哈希表或二级哈希表中是否已有匹配的 dentry，如果没有，就把新分配的 dentry 存入二级哈希表，并设置 DCACHE_PAR_LOOKUP。)

If a matching dentry was found in the primary hash table then that is
returned and the caller can know that it lost a race with some other
thread adding the entry.  If no matching dentry is found in either
cache, the newly allocated dentry is returned and the caller can
detect this from the presence of ``DCACHE_PAR_LOOKUP``.  In this case it
knows that it has won any race and now is responsible for asking the
filesystem to perform the lookup and find the matching inode.  When
the lookup is complete, it must call ``d_lookup_done()`` which clears
the flag and does some other house keeping, including removing the
dentry from the secondary hash table - it will normally have been
added to the primary hash table already.  Note that a ``struct
waitqueue_head`` is passed to ``d_alloc_parallel()``, and
``d_lookup_done()`` must be called while this ``waitqueue_head`` is still
in scope.
(如果在主哈希表中找到了匹配的 dentry，则返回该 dentry，调用者可以知道自己输掉了与其他添加该条目的线程之间的竞争。如果两个缓存中都没有找到匹配的 dentry，则返回新分配的 dentry，调用者可以通过 DCACHE_PAR_LOOKUP 的存在检测到这一点。在这种情况下，它知道自己赢得了任何竞争，现在由它负责请求文件系统执行查找并找到匹配的 inode。查找完成后，它必须调用 d_lookup_done()，该函数清除标志并做一些其他内部整理工作，包括把 dentry 从二级哈希表中移除——它通常已经被加入主哈希表了。注意，一个 struct waitqueue_head 会被传给 d_alloc_parallel()，并且必须在这个 waitqueue_head 仍在作用域内时调用 d_lookup_done()。)

If a matching dentry is found in the secondary hash table,
``d_alloc_parallel()`` has a little more work to do. It first waits for
``DCACHE_PAR_LOOKUP`` to be cleared, using a wait_queue that was passed
to the instance of ``d_alloc_parallel()`` that won the race and that
will be woken by the call to ``d_lookup_done()``.  It then checks to see
if the dentry has now been added to the primary hash table.  If it
has, the dentry is returned and the caller just sees that it lost any
race.  If it hasn't been added to the primary hash table, the most
likely explanation is that some other dentry was added instead using
``d_splice_alias()``.  In any case, ``d_alloc_parallel()`` repeats all the
look ups from the start and will normally return something from the
primary hash table.
(如果在二级哈希表中找到了匹配的 dentry，d_alloc_parallel() 还要多做一些工作。它首先等待 DCACHE_PAR_LOOKUP 被清除，使用的是传给赢得竞争的那个 d_alloc_parallel() 实例的 wait_queue，该队列会被对 d_lookup_done() 的调用唤醒。然后它检查该 dentry 此时是否已被加入主哈希表。如果已加入，则返回该 dentry，调用者只会看到自己输掉了竞争。如果尚未加入主哈希表，最可能的解释是改用 d_splice_alias() 添加了某个其他 dentry。无论哪种情况，d_alloc_parallel() 都会从头重复所有查找，通常将从主哈希表返回结果。)

mnt->mnt_count
~~~~~~~~~~~~~~

``mnt_count`` is a per-CPU reference counter on "``mount``" structures.
Per-CPU here means that incrementing the count is cheap as it only
uses CPU-local memory, but checking if the count is zero is expensive as
it needs to check with every CPU.  Taking a ``mnt_count`` reference
prevents the mount structure from disappearing as the result of regular
unmount operations, but does not prevent a "lazy" unmount.  So holding
``mnt_count`` doesn't ensure that the mount remains in the namespace and,
in particular, doesn't stabilize the link to the mounted-on dentry.  It
does, however, ensure that the ``mount`` data structure remains coherent,
and it provides a reference to the root dentry of the mounted
filesystem.  So a reference through ``->mnt_count`` provides a stable
reference to the mounted dentry, but not the mounted-on dentry.
(mnt_count 是"mount"结构上的每 CPU 引用计数。这里的"每 CPU"意味着递增计数很便宜，因为它只使用 CPU 本地内存，但检查计数是否为零则很昂贵，因为它需要与每个 CPU 核对。获取一个 mnt_count 引用可以防止 mount 结构因常规卸载操作而消失，但不能阻止"惰性"(lazy)卸载。因此，持有 mnt_count 并不能确保该挂载保留在命名空间中，特别是，它不能稳定指向挂载点 dentry(mounted-on dentry)的链接。不过，它确实能确保 mount 数据结构保持一致，并且它提供对被挂载文件系统根 dentry 的引用。所以，通过 ->mnt_count 得到的引用提供了对被挂载 dentry(即被挂载文件系统的根)的稳定引用，但不是对挂载点 dentry 的稳定引用。)

mount_lock
~~~~~~~~~~

``mount_lock`` is a global seqlock, a bit like ``rename_lock``.  It can be used to
check if any change has been made to any mount points.
(mount_lock 是一个全局顺序锁，有点像 rename_lock。它可用于检查是否对任何挂载点做了任何更改。)

While walking down the tree (away from the root) this lock is used when
crossing a mount point to check that the crossing was safe.  That is,
the value in the seqlock is read, then the code finds the mount that
is mounted on the current directory, if there is one, and increments
the ``mnt_count``.  Finally the value in ``mount_lock`` is checked against
the old value.  If there is no change, then the crossing was safe.  If there
was a change, the ``mnt_count`` is decremented and the whole process is
retried.
(沿树向下走(远离根)时，这个锁在跨越挂载点时用于检查跨越是否安全。也就是说，先读取顺序锁中的值，然后代码找到挂载在当前目录上的挂载(如果有的话)并递增 mnt_count。最后，将 mount_lock 中的值与旧值进行比较。如果没有变化，则跨越是安全的。如果有变化，则递减 mnt_count 并重试整个过程。)

When walking up the tree (towards the root) by following a ".." link,
a little more care is needed.  In this case the seqlock (which
contains both a counter and a spinlock) is fully locked to prevent
any changes to any mount points while stepping up.  This locking is
needed to stabilize the link to the mounted-on dentry, which the
refcount on the mount itself doesn't ensure.
(沿树向上走(朝向根)并跟随".."链接时，需要多一点小心。在这种情况下，顺序锁(它既包含计数器也包含自旋锁)被完全锁住，以防止在向上走的过程中对任何挂载点做出任何更改。需要这种加锁来稳定指向挂载点 dentry 的链接，而 mount 本身上的引用计数并不能保证这一点。)

``mount_lock`` is also used to detect and defend against potential attacks
against ``LOOKUP_BENEATH`` and ``LOOKUP_IN_ROOT`` when resolving ".." (where
the parent directory is moved outside the root, bypassing the ``path_equal()``
check). If ``mount_lock`` is updated during the lookup and the path encounters
a "..", a potential attack occurred and ``handle_dots()`` will bail out with
``-EAGAIN``.
(mount_lock 还用于在解析".."时检测和防范针对 LOOKUP_BENEATH 和 LOOKUP_IN_ROOT 的潜在攻击(即父目录被移到根之外、绕过 path_equal() 检查的情形)。如果 mount_lock 在查找期间被更新而路径又遇到".."，则说明可能发生了攻击，handle_dots() 将以 -EAGAIN 退出。)

RCU
~~~

Finally the global (but extremely lightweight) RCU read lock is held
from time to time to ensure certain data structures don't get freed
unexpectedly.
(最后，全局的(但极其轻量级的)RCU 读锁会被不时持有，以确保某些数据结构不会意外地被释放。)

In particular it is held while scanning chains in the dcache hash
table, and the mount point hash table.
(特别是在扫描 dcache 哈希表和挂载点哈希表中的链时会持有它。)

Bringing it together with ``struct nameidata``
(用 struct nameidata 将其整合在一起)
----------------------------------------------

.. _First edition Unix: https://minnie.tuhs.org/cgi-bin/utree.pl?file=V1/u2.s

Throughout the process of walking a path, the current status is stored
in a ``struct nameidata``, "namei" being the traditional name - dating
all the way back to `First Edition Unix`_ - of the function that
converts a "name" to an "inode".  ``struct nameidata`` contains (among
other fields):
(在整个路径遍历过程中，当前状态存储在一个 struct nameidata 中；"namei"是将"名称"转换为"inode"的函数的传统名称——最早可以追溯到第一版 Unix(First Edition Unix)。struct nameidata 包含(除其他字段外)：)

``struct path path``
~~~~~~~~~~~~~~~~~~~~

A ``path`` contains a ``struct vfsmount`` (which is
embedded in a ``struct mount``) and a ``struct dentry``.  Together these
record the current status of the walk.  They start out referring to the
starting point (the current working directory, the root directory, or some other
directory identified by a file descriptor), and are updated on each
step.  A reference through ``d_lockref`` and ``mnt_count`` is always
held.
(一个 path 包含一个 struct vfsmount(它内嵌在 struct mount 中)和一个 struct dentry。它们共同记录遍历的当前状态。它们起初指向起点(当前工作目录、根目录，或由文件描述符标识的某个其他目录)，并在每一步被更新。始终持有通过 d_lockref 和 mnt_count 的引用。)

``struct qstr last``
~~~~~~~~~~~~~~~~~~~~

This is a string together with a length (i.e. *not* ``nul`` terminated)
that is the "next" component in the pathname.
(这是一个连同长度一起保存的字符串(即*不*以 nul 结尾)，它是路径名中的"下一个"组件。)

``int last_type``
~~~~~~~~~~~~~~~~~

This is one of ``LAST_NORM``, ``LAST_ROOT``, ``LAST_DOT`` or ``LAST_DOTDOT``.
The ``last`` field is only valid if the type is ``LAST_NORM``.
(它是 LAST_NORM、LAST_ROOT、LAST_DOT 或 LAST_DOTDOT 之一。只有当类型为 LAST_NORM 时 last 字段才有效。)

``struct path root``
~~~~~~~~~~~~~~~~~~~~

This is used to hold a reference to the effective root of the
filesystem.  Often that reference won't be needed, so this field is
only assigned the first time it is used, or when a non-standard root
is requested.  Keeping a reference in the ``nameidata`` ensures that
only one root is in effect for the entire path walk, even if it races
with a ``chroot()`` system call.
(它用于保存对文件系统有效根的引用。通常并不需要这个引用，所以该字段只在第一次使用时、或在请求非标准根时才被赋值。在 nameidata 中保留引用可确保在整个路径遍历期间只有一个根生效，即使它与 chroot() 系统调用存在竞争。)

It should be noted that in the case of ``LOOKUP_IN_ROOT`` or
``LOOKUP_BENEATH``, the effective root becomes the directory file descriptor
passed to ``openat2()`` (which exposes these ``LOOKUP_`` flags).
(应当注意，在 LOOKUP_IN_ROOT 或 LOOKUP_BENEATH 的情况下，有效根变为传递给 openat2()(它暴露这些 ``LOOKUP_`` 标志)的目录文件描述符。)

The root is needed when either of two conditions holds: (1) either the
pathname or a symbolic link starts with a "'/'", or (2) a "``..``"
component is being handled, since "``..``" from the root must always stay
at the root.  The value used is usually the current root directory of
the calling process.  An alternate root can be provided as when
``sysctl()`` calls ``file_open_root()``, and when NFSv4 or Btrfs call
``mount_subtree()``.  In each case a pathname is being looked up in a very
specific part of the filesystem, and the lookup must not be allowed to
escape that subtree.  It works a bit like a local ``chroot()``.
(在以下两个条件之一成立时需要根：(1)路径名或符号链接以'/'开头，或者(2)正在处理".."组件，因为来自根的".."必须始终停留在根处。使用的值通常是调用进程当前的根目录。可以提供备用根，例如当 sysctl() 调用 file_open_root() 时，以及当 NFSv4 或 Btrfs 调用 mount_subtree() 时。在每种情况下，都是在文件系统的一个非常特定的部分中查找路径名，并且绝不允许查找逃出该子树。它的工作方式有点像一个本地 chroot()。)

Ignoring the handling of symbolic links, we can now describe the
"``link_path_walk()``" function, which handles the lookup of everything
except the final component as:
(忽略符号链接的处理，我们现在可以把"link_path_walk()"函数——它处理除最后组件之外的所有查找——描述如下：)

   Given a path (``name``) and a nameidata structure (``nd``), check that the
   current directory has execute permission and then advance ``name``
   over one component while updating ``last_type`` and ``last``.  If that
   was the final component, then return, otherwise call
   ``walk_component()`` and repeat from the top.
   (给定一个路径(name)和一个 nameidata 结构(nd)，检查当前目录是否具有执行权限，然后让 name 前进一个组件，同时更新 last_type 和 last。如果那就是最后一个组件，则返回；否则调用 walk_component() 并从顶部开始重复。)

``walk_component()`` is even easier.  If the component is ``LAST_DOTS``,
it calls ``handle_dots()`` which does the necessary locking as already
described.  If it finds a ``LAST_NORM`` component it first calls
"``lookup_fast()``" which only looks in the dcache, but will ask the
filesystem to revalidate the result if it is that sort of filesystem.
If that doesn't get a good result, it calls "``lookup_slow()``" which
takes ``i_rwsem``, rechecks the cache, and then asks the filesystem
to find a definitive answer.
(walk_component() 甚至更简单。如果组件是 LAST_DOTS，它会调用 handle_dots()，后者按前文所述进行必要的加锁。如果它发现的是 LAST_NORM 组件，它首先调用"lookup_fast()"，后者只在 dcache 中查找，但如果属于那类文件系统，它会请求文件系统重新验证结果。如果这样没有得到好结果，它会调用"lookup_slow()"，后者获取 i_rwsem、重新检查缓存，然后请求文件系统找到确定的答案。)

As the last step of walk_component(), step_into() will be called either
directly from walk_component() or from handle_dots().  It calls
handle_mounts(), to check and handle mount points, in which a new
``struct path`` is created containing a counted reference to the new dentry and
a reference to the new ``vfsmount`` which is only counted if it is
different from the previous ``vfsmount``. Then if there is
a symbolic link, step_into() calls pick_link() to deal with it,
otherwise it installs the new ``struct path`` in the ``struct nameidata``, and
drops the unneeded references.
(作为 walk_component() 的最后一步，step_into() 会被调用，要么直接来自 walk_component()，要么来自 handle_dots()。它调用 handle_mounts() 来检查并处理挂载点，其中会创建一个新的 struct path，包含对新 dentry 的计数引用以及对新 vfsmount 的引用——仅当它与前一个 vfsmount 不同时才计数。然后，如果存在符号链接，step_into() 调用 pick_link() 来处理它；否则它把新的 struct path 安装到 struct nameidata 中，并放下不需要的引用。)

This "hand-over-hand" sequencing of getting a reference to the new
dentry before dropping the reference to the previous dentry may
seem obvious, but is worth pointing out so that we will recognize its
analogue in the "RCU-walk" version.
(这种"手把手交接"(hand-over-hand)式的顺序——在放下对前一个 dentry 的引用之前先获取对新 dentry 的引用——看似显而易见，但值得指出，以便我们能在"RCU-walk"版本中认出它的对应物。)

Handling the final component
(处理最后一个组件)
----------------------------

``link_path_walk()`` only walks as far as setting ``nd->last`` and
``nd->last_type`` to refer to the final component of the path.  It does
not call ``walk_component()`` that last time.  Handling that final
component remains for the caller to sort out. Those callers are
path_lookupat(), path_parentat() and
path_openat() each of which handles the differing requirements of
different system calls.
(link_path_walk() 只走到把 nd->last 和 nd->last_type 设置为指向路径的最后组件为止。它不会在最后一次调用 walk_component()。处理那个最后组件留给调用者去解决。这些调用者是 path_lookupat()、path_parentat() 和 path_openat()，它们各自处理不同系统调用的不同需求。)

``path_parentat()`` is clearly the simplest - it just wraps a little bit
of housekeeping around ``link_path_walk()`` and returns the parent
directory and final component to the caller.  The caller will be either
aiming to create a name (via ``filename_create()``) or remove or rename
a name (in which case ``user_path_parent()`` is used).  They will use
``i_rwsem`` to exclude other changes while they validate and then
perform their operation.
(path_parentat() 显然最简单——它只是在 link_path_walk() 周围包裹了一点内部整理工作，并把父目录和最后组件返回给调用者。调用者要么打算创建一个名称(通过 filename_create())，要么移除或重命名一个名称(此时使用 user_path_parent())。它们会在验证并随后执行其操作期间使用 i_rwsem 来排除其他更改。)

``path_lookupat()`` is nearly as simple - it is used when an existing
object is wanted such as by ``stat()`` or ``chmod()``.  It essentially just
calls ``walk_component()`` on the final component through a call to
``lookup_last()``.  ``path_lookupat()`` returns just the final dentry.
It is worth noting that when flag ``LOOKUP_MOUNTPOINT`` is set,
path_lookupat() will unset LOOKUP_JUMPED in nameidata so that in the
subsequent path traversal d_weak_revalidate() won't be called.
This is important when unmounting a filesystem that is inaccessible, such as
one provided by a dead NFS server.
(path_lookupat() 几乎同样简单——当需要的是已存在的对象时使用它，例如被 stat() 或 chmod() 使用。它实质上只是通过调用 lookup_last() 对最后组件调用 walk_component()。path_lookupat() 只返回最后的 dentry。值得注意的是，当设置了 LOOKUP_MOUNTPOINT 标志时，path_lookupat() 会清除 nameidata 中的 LOOKUP_JUMPED，以便在后续的路径遍历中不会调用 d_weak_revalidate()。这在卸载一个不可访问的文件系统(例如由已死掉的 NFS 服务器提供的文件系统)时很重要。)

Finally ``path_openat()`` is used for the ``open()`` system call; it
contains, in support functions starting with "open_last_lookups()", all the
complexity needed to handle the different subtleties of O_CREAT (with
or without O_EXCL), final "``/``" characters, and trailing symbolic
links.  We will revisit this in the final part of this series, which
focuses on those symbolic links.  "open_last_lookups()" will sometimes, but
not always, take ``i_rwsem``, depending on what it finds.
(最后，path_openat() 用于 open() 系统调用；它在以"open_last_lookups()"开头的支撑函数中包含了处理 O_CREAT(带或不带 O_EXCL)的各种微妙之处、结尾"/"字符以及尾部符号链接所需的全部复杂性。我们将在本系列的最后一部分重新审视这一点，该部分聚焦于那些符号链接。"open_last_lookups()"有时会、但并不总是获取 i_rwsem，取决于它发现了什么。)

Each of these, or the functions which call them, need to be alert to
the possibility that the final component is not ``LAST_NORM``.  If the
goal of the lookup is to create something, then any value for
``last_type`` other than ``LAST_NORM`` will result in an error.  For
example if ``path_parentat()`` reports ``LAST_DOTDOT``, then the caller
won't try to create that name.  They also check for trailing slashes
by testing ``last.name[last.len]``.  If there is any character beyond
the final component, it must be a trailing slash.
(这些函数，或者调用它们的函数，都需要警惕最后组件不是 LAST_NORM 的可能性。如果查找的目标是创建某个东西，那么 last_type 的任何非 LAST_NORM 值都会导致错误。例如，如果 path_parentat() 报告 LAST_DOTDOT，那么调用者就不会尝试创建那个名称。它们还通过测试 last.name[last.len] 来检查结尾斜杠。如果最后组件之外还有任何字符，那必然是结尾斜杠。)

Revalidation and automounts
(重新验证与自动挂载)
---------------------------

Apart from symbolic links, there are only two parts of the "REF-walk"
process not yet covered.  One is the handling of stale cache entries
and the other is automounts.
(除了符号链接之外，"REF-walk"过程还有两个部分尚未覆盖。一个是对过时缓存条目的处理，另一个是自动挂载。)

On filesystems that require it, the lookup routines will call the
``->d_revalidate()`` dentry method to ensure that the cached information
is current.  This will often confirm validity or update a few details
from a server.  In some cases it may find that there has been change
further up the path and that something that was thought to be valid
previously isn't really.  When this happens the lookup of the whole
path is aborted and retried with the "``LOOKUP_REVAL``" flag set.  This
forces revalidation to be more thorough.  We will see more details of
this retry process in the next article.
(在需要的文件系统上，查找例程会调用 ->d_revalidate() dentry 方法，以确保缓存的信息是最新的。这通常会确认有效性，或从服务器更新一些细节。在某些情况下，它可能发现路径更上方发生了更改，而之前认为有效的东西实际上并非如此。发生这种情况时，整个路径的查找会被中止，并设置"LOOKUP_REVAL"标志重试。这会强制进行更彻底的重新验证。我们将在下一篇文章中看到这一重试过程的更多细节。)

Automount points are locations in the filesystem where an attempt to
lookup a name can trigger changes to how that lookup should be
handled, in particular by mounting a filesystem there.  These are
covered in greater detail in autofs.rst in the Linux documentation
tree, but a few notes specifically related to path lookup are in order
here.
(自动挂载点是文件系统中的一些位置，在这些位置上尝试查找名称可能触发对该查找应如何处理的更改，特别是通过在那里挂载一个文件系统。Linux 文档树中的 autofs.rst 对它们有更详细的介绍，但这里有必要给出一些与路径查找特别相关的说明。)

The Linux VFS has a concept of "managed" dentries.  There are three
potentially interesting things about these dentries corresponding
to three different flags that might be set in ``dentry->d_flags``:
(Linux VFS 有"受管"(managed)dentry 的概念。这些 dentry 有三个潜在值得关注之处，对应于可能在 dentry->d_flags 中设置的三个不同标志：)

``DCACHE_MANAGE_TRANSIT``
~~~~~~~~~~~~~~~~~~~~~~~~~

If this flag has been set, then the filesystem has requested that the
``d_manage()`` dentry operation be called before handling any possible
mount point.  This can perform two particular services:
(如果设置了这个标志，则文件系统已请求在处理任何可能的挂载点之前调用 d_manage() dentry 操作。它可以提供两项特定的服务：)

It can block to avoid races.  If an automount point is being
unmounted, the ``d_manage()`` function will usually wait for that
process to complete before letting the new lookup proceed and possibly
trigger a new automount.
(它可以阻塞以避免竞争。如果一个自动挂载点正在被卸载，d_manage() 函数通常会等待那个卸载过程完成，然后才让新的查找继续进行，否则可能触发新的自动挂载。)

It can selectively allow only some processes to transit through a
mount point.  When a server process is managing automounts, it may
need to access a directory without triggering normal automount
processing.  That server process can identify itself to the ``autofs``
filesystem, which will then give it a special pass through
``d_manage()`` by returning ``-EISDIR``.
(它可以有选择地只允许某些进程穿过挂载点。当服务器进程管理自动挂载时，它可能需要访问一个目录而不触发正常的自动挂载处理。该服务器进程可以向 autofs 文件系统表明自己的身份，然后 autofs 会通过返回 -EISDIR 给它一张穿越 d_manage() 的特别通行证。)

``DCACHE_MOUNTED``
~~~~~~~~~~~~~~~~~~

This flag is set on every dentry that is mounted on.  As Linux
supports multiple filesystem namespaces, it is possible that the
dentry may not be mounted on in *this* namespace, just in some
other.  So this flag is seen as a hint, not a promise.
(每个被挂载的 dentry 上都设置了这个标志。由于 Linux 支持多个文件系统命名空间，该 dentry 可能并未在*这个*命名空间中被挂载，而只是在其他某个命名空间中被挂载。所以这个标志被视为提示，而不是承诺。)

If this flag is set, and ``d_manage()`` didn't return ``-EISDIR``,
``lookup_mnt()`` is called to examine the mount hash table (honoring the
``mount_lock`` described earlier) and possibly return a new ``vfsmount``
and a new ``dentry`` (both with counted references).
(如果设置了这个标志且 d_manage() 没有返回 -EISDIR，则会调用 lookup_mnt() 检查挂载哈希表(遵循前面描述的 mount_lock)，并可能返回新的 vfsmount 和新的 dentry(两者都带计数引用)。)

``DCACHE_NEED_AUTOMOUNT``
~~~~~~~~~~~~~~~~~~~~~~~~~

If ``d_manage()`` allowed us to get this far, and ``lookup_mnt()`` didn't
find a mount point, then this flag causes the ``d_automount()`` dentry
operation to be called.
(如果 d_manage() 允许我们走到这一步，而 lookup_mnt() 没有找到挂载点，那么这个标志会导致调用 d_automount() dentry 操作。)

The ``d_automount()`` operation can be arbitrarily complex and may
communicate with server processes etc. but it should ultimately either
report that there was an error, that there was nothing to mount, or
should provide an updated ``struct path`` with new ``dentry`` and ``vfsmount``.
(d_automount() 操作可以任意复杂，也可以与服务器进程等通信，但它最终要么报告发生了错误、没有什么可挂载，要么提供带有新 dentry 和新 vfsmount 的更新后的 struct path。)

In the latter case, ``finish_automount()`` will be called to safely
install the new mount point into the mount table.
(在后一种情况下，将调用 finish_automount() 把新的挂载点安全地安装到挂载表中。)

There is no new locking of import here and it is important that no
locks (only counted references) are held over this processing due to
the very real possibility of extended delays.
This will become more important next time when we examine RCU-walk
which is particularly sensitive to delays.
(这里没有新的重要加锁；而且很重要的一点是，在这一处理期间不持有任何锁(只有计数引用)，因为出现长时间延时的可能性非常真实。当我们下次考察对延时特别敏感的 RCU-walk 时，这一点将变得更加重要。)

RCU-walk - faster pathname lookup in Linux
(RCU-walk——Linux 中更快的路径名查找)
==========================================

RCU-walk is another algorithm for performing pathname lookup in Linux.
It is in many ways similar to REF-walk and the two share quite a bit
of code.  The significant difference in RCU-walk is how it allows for
the possibility of concurrent access.
(RCU-walk 是在 Linux 中执行路径名查找的另一种算法。它在许多方面与 REF-walk 相似，两者共享相当多的代码。RCU-walk 的重大差异在于它如何应对并发访问的可能性。)

We noted that REF-walk is complex because there are numerous details
and special cases.  RCU-walk reduces this complexity by simply
refusing to handle a number of cases -- it instead falls back to
REF-walk.  The difficulty with RCU-walk comes from a different
direction: unfamiliarity.  The locking rules when depending on RCU are
quite different from traditional locking, so we will spend a little extra
time when we come to those.
(我们注意到 REF-walk 复杂是因为有大量细节和特殊情况。RCU-walk 通过干脆拒绝处理若干情况来降低这种复杂性——它转而退回到 REF-walk。RCU-walk 的困难来自另一个方向：不熟悉。依赖 RCU 时的加锁规则与传统加锁相当不同，所以当我们讲到那些规则时会多花一点时间。)

Clear demarcation of roles
(角色的清晰划分)
--------------------------

The easiest way to manage concurrency is to forcibly stop any other
thread from changing the data structures that a given thread is
looking at.  In cases where no other thread would even think of
changing the data and lots of different threads want to read at the
same time, this can be very costly.  Even when using locks that permit
multiple concurrent readers, the simple act of updating the count of
the number of current readers can impose an unwanted cost.  So the
goal when reading a shared data structure that no other process is
changing is to avoid writing anything to memory at all.  Take no
locks, increment no counts, leave no footprints.
(管理并发最简单的方法是强行阻止任何其他线程更改给定线程正在查看的数据结构。在没有其他线程会想到更改数据、而大量不同线程想同时读取的情况下，这可能非常昂贵。即使使用允许多个并发读取者的锁，更新当前读取者数量这一简单动作也可能带来不想要的成本。所以，读取没有其他进程正在更改的共享数据结构时的目标是：完全避免向内存写入任何东西。不加锁、不增计数、不留足迹。)

The REF-walk mechanism already described certainly doesn't follow this
principle, but then it is really designed to work when there may well
be other threads modifying the data.  RCU-walk, in contrast, is
designed for the common situation where there are lots of frequent
readers and only occasional writers.  This may not be common in all
parts of the filesystem tree, but in many parts it will be.  For the
other parts it is important that RCU-walk can quickly fall back to
using REF-walk.
(前面描述的 REF-walk 机制当然不遵循这一原则，但它本来就是为很可能有其他线程修改数据的情况而设计的。相比之下，RCU-walk 为常见的情形而设计：有大量频繁的读取者而只有偶发的写入者。这在文件系统树的所有部分可能并不常见，但在许多部分会是这样。对于其他部分，RCU-walk 能够快速退回到 REF-walk 是很重要的。)

Pathname lookup always starts in RCU-walk mode but only remains there
as long as what it is looking for is in the cache and is stable.  It
dances lightly down the cached filesystem image, leaving no footprints
and carefully watching where it is, to be sure it doesn't trip.  If it
notices that something has changed or is changing, or if something
isn't in the cache, then it tries to stop gracefully and switch to
REF-walk.
(路径名查找总是以 RCU-walk 模式开始，但只要它要找的东西在缓存中且是稳定的，它就一直停留在那里。它轻快地在缓存的文件系统映像上起舞，不留足迹，并小心地留意自己身在何处，以确保不会绊倒。如果它注意到某物已经或正在发生变化，或者某物不在缓存中，它就尝试优雅地停下并切换到 REF-walk。)

This stopping requires getting a counted reference on the current
``vfsmount`` and ``dentry``, and ensuring that these are still valid -
that a path walk with REF-walk would have found the same entries.
This is an invariant that RCU-walk must guarantee.  It can only make
decisions, such as selecting the next step, that are decisions which
REF-walk could also have made if it were walking down the tree at the
same time.  If the graceful stop succeeds, the rest of the path is
processed with the reliable, if slightly sluggish, REF-walk.  If
RCU-walk finds it cannot stop gracefully, it simply gives up and
restarts from the top with REF-walk.
(这种停下需要获取当前 vfsmount 和 dentry 上的计数引用，并确保它们仍然有效——即用 REF-walk 进行路径遍历也会找到相同的条目。这是 RCU-walk 必须保证的不变式。它只能做出诸如选择下一步之类的决定，而且这些决定必须是 REF-walk 在同一时刻沿树向下走时也能做出的。如果优雅停止成功，路径的其余部分将由可靠(尽管稍显迟缓)的 REF-walk 处理。如果 RCU-walk 发现自己无法优雅地停下，它就干脆放弃并从顶部用 REF-walk 重新开始。)

This pattern of "try RCU-walk, if that fails try REF-walk" can be
clearly seen in functions like filename_lookup(),
filename_parentat(),
do_filp_open(), and do_file_open_root().  These four
correspond roughly to the three ``path_*()`` functions we met earlier,
each of which calls ``link_path_walk()``.  The ``path_*()`` functions are
called using different mode flags until a mode is found which works.
They are first called with ``LOOKUP_RCU`` set to request "RCU-walk".  If
that fails with the error ``ECHILD`` they are called again with no
special flag to request "REF-walk".  If either of those report the
error ``ESTALE`` a final attempt is made with ``LOOKUP_REVAL`` set (and no
``LOOKUP_RCU``) to ensure that entries found in the cache are forcibly
revalidated - normally entries are only revalidated if the filesystem
determines that they are too old to trust.
("先试 RCU-walk，若失败再试 REF-walk"这一模式在 filename_lookup()、filename_parentat()、do_filp_open() 和 do_file_open_root() 等函数中清晰可见。这四个函数大致对应于我们前面遇到的三个 ``path_*()`` 函数，每个都调用 link_path_walk()。``path_*()`` 函数会用不同的模式标志被调用，直到找到可行的模式。它们首先在设置了 LOOKUP_RCU 的情况下被调用，以请求"RCU-walk"。如果以错误 ECHILD 失败，则不带特殊标志再次调用它们，以请求"REF-walk"。如果两者之一报告错误 ESTALE，则进行最后一次尝试，设置 LOOKUP_REVAL(且不设 LOOKUP_RCU)，以确保对缓存中找到的条目进行强制重新验证——通常只有在文件系统判定条目太旧而不可信时才会重新验证。)

The ``LOOKUP_RCU`` attempt may drop that flag internally and switch to
REF-walk, but will never then try to switch back to RCU-walk.  Places
that trip up RCU-walk are much more likely to be near the leaves and
so it is very unlikely that there will be much, if any, benefit from
switching back.
(LOOKUP_RCU 尝试可能在内部放下该标志并切换到 REF-walk，但之后绝不会尝试切换回 RCU-walk。让 RCU-walk 绊倒的地方更可能靠近叶子，因此切换回去即使有收益也极可能微乎其微。)

RCU and seqlocks: fast and light
(RCU 与顺序锁：快速而轻量)
--------------------------------

RCU is, unsurprisingly, critical to RCU-walk mode.  The
``rcu_read_lock()`` is held for the entire time that RCU-walk is walking
down a path.  The particular guarantee it provides is that the key
data structures - dentries, inodes, super_blocks, and mounts - will
not be freed while the lock is held.  They might be unlinked or
invalidated in one way or another, but the memory will not be
repurposed so values in various fields will still be meaningful.  This
is the only guarantee that RCU provides; everything else is done using
seqlocks.
(不出所料，RCU 对 RCU-walk 模式至关重要。rcu_read_lock() 在 RCU-walk 沿路径向下走的整个期间都被持有。它提供的特定保证是：关键数据结构——dentry、inode、super_block 和 mount——在锁被持有期间不会被释放。它们可能以这样或那样的方式被解除链接或失效，但内存不会被挪作他用，所以各字段中的值仍然有意义。这是 RCU 提供的唯一保证；其余一切都使用顺序锁完成。)

As we saw above, REF-walk holds a counted reference to the current
dentry and the current vfsmount, and does not release those references
before taking references to the "next" dentry or vfsmount.  It also
sometimes takes the ``d_lock`` spinlock.  These references and locks are
taken to prevent certain changes from happening.  RCU-walk must not
take those references or locks and so cannot prevent such changes.
Instead, it checks to see if a change has been made, and aborts or
retries if it has.
(如上所述，REF-walk 持有对当前 dentry 和当前 vfsmount 的计数引用，并且在获取对"下一个"dentry 或 vfsmount 的引用之前不释放这些引用。它有时还会获取 d_lock 自旋锁。获取这些引用和锁是为了防止某些更改发生。RCU-walk 不得获取那些引用或锁，因此无法阻止此类更改。相反，它检查是否发生了更改，如果发生了则中止或重试。)

To preserve the invariant mentioned above (that RCU-walk may only make
decisions that REF-walk could have made), it must make the checks at
or near the same places that REF-walk holds the references.  So, when
REF-walk increments a reference count or takes a spinlock, RCU-walk
samples the status of a seqlock using ``read_seqcount_begin()`` or a
similar function.  When REF-walk decrements the count or drops the
lock, RCU-walk checks if the sampled status is still valid using
``read_seqcount_retry()`` or similar.
(为了保持前面提到的不变式(RCU-walk 只能做出 REF-walk 本可以做出的决定)，它必须在 REF-walk 持有引用的相同位置或附近进行检查。所以，当 REF-walk 递增引用计数或获取自旋锁时，RCU-walk 使用 read_seqcount_begin() 或类似函数对顺序锁的状态进行采样。当 REF-walk 递减计数或放下锁时，RCU-walk 使用 read_seqcount_retry() 或类似函数检查采样到的状态是否仍然有效。)

However, there is a little bit more to seqlocks than that.  If
RCU-walk accesses two different fields in a seqlock-protected
structure, or accesses the same field twice, there is no a priori
guarantee of any consistency between those accesses.  When consistency
is needed - which it usually is - RCU-walk must take a copy and then
use ``read_seqcount_retry()`` to validate that copy.
(不过，顺序锁的讲究还不止这些。如果 RCU-walk 访问受顺序锁保护的结构中的两个不同字段，或访问同一字段两次，那么这些访问之间没有任何先验的一致性保证。当需要一致性时——通常都需要——RCU-walk 必须取一份副本，然后使用 read_seqcount_retry() 来验证该副本。)

``read_seqcount_retry()`` not only checks the sequence number, but also
imposes a memory barrier so that no memory-read instruction from
*before* the call can be delayed until *after* the call, either by the
CPU or by the compiler.  A simple example of this can be seen in
``slow_dentry_cmp()`` which, for filesystems which do not use simple
byte-wise name equality, calls into the filesystem to compare a name
against a dentry.  The length and name pointer are copied into local
variables, then ``read_seqcount_retry()`` is called to confirm the two
are consistent, and only then is ``->d_compare()`` called.  When
standard filename comparison is used, ``dentry_cmp()`` is called
instead.  Notably it does *not* use ``read_seqcount_retry()``, but
instead has a large comment explaining why the consistency guarantee
isn't necessary.  A subsequent ``read_seqcount_retry()`` will be
sufficient to catch any problem that could occur at this point.
(read_seqcount_retry() 不仅检查序列号，还施加内存屏障，使得调用*之前*的任何内存读指令都不会被 CPU 或编译器延迟到调用*之后*执行。一个简单的例子见于 slow_dentry_cmp()，对于不使用简单逐字节名称相等比较的文件系统，它会调用文件系统来将名称与 dentry 比较。长度和名称指针被复制到局部变量中，然后调用 read_seqcount_retry() 确认两者一致，只有在这之后才调用 ->d_compare()。当使用标准的文件名比较时，则改为调用 dentry_cmp()。值得注意的是，它*不*使用 read_seqcount_retry()，而是用一大段注释解释为什么一致性保证并不必要。随后的 read_seqcount_retry() 足以捕获此时可能发生的任何问题。)

With that little refresher on seqlocks out of the way we can look at
the bigger picture of how RCU-walk uses seqlocks.
(在简单重温了顺序锁之后，我们可以从更大的图景来看 RCU-walk 如何使用顺序锁。)

``mount_lock`` and ``nd->m_seq``
(mount_lock 与 nd->m_seq)
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

We already met the ``mount_lock`` seqlock when REF-walk used it to
ensure that crossing a mount point is performed safely.  RCU-walk uses
it for that too, but for quite a bit more.
(我们已经见过 mount_lock 顺序锁，当时 REF-walk 用它来确保安全地跨越挂载点。RCU-walk 也将它用于此目的，但用途远不止于此。)

Instead of taking a counted reference to each ``vfsmount`` as it
descends the tree, RCU-walk samples the state of ``mount_lock`` at the
start of the walk and stores this initial sequence number in the
``struct nameidata`` in the ``m_seq`` field.  This one lock and one
sequence number are used to validate all accesses to all ``vfsmounts``,
and all mount point crossings.  As changes to the mount table are
relatively rare, it is reasonable to fall back on REF-walk any time
that any "mount" or "unmount" happens.
(RCU-walk 在沿树下行时并不对每个 vfsmount 获取计数引用，而是在遍历开始时对 mount_lock 的状态进行采样，并把这一初始序列号存储在 struct nameidata 的 m_seq 字段中。这一把锁和一个序列号用于验证对所有 vfsmount 的所有访问以及所有挂载点跨越。由于挂载表的更改相对少见，因此每当发生任何"挂载"或"卸载"时退回到 REF-walk 是合理的。)

``m_seq`` is checked (using ``read_seqretry()``) at the end of an RCU-walk
sequence, whether switching to REF-walk for the rest of the path or
when the end of the path is reached.  It is also checked when stepping
down over a mount point (in ``__follow_mount_rcu()``) or up (in
``follow_dotdot_rcu()``).  If it is ever found to have changed, the
whole RCU-walk sequence is aborted and the path is processed again by
REF-walk.
(m_seq 在 RCU-walk 序列结束时被检查(使用 read_seqretry())，无论是切换到 REF-walk 处理路径的其余部分，还是到达路径末尾。在跨过挂载点向下走(在 __follow_mount_rcu() 中)或向上走(在 follow_dotdot_rcu() 中)时也会检查它。一旦发现它发生了变化，整个 RCU-walk 序列就会被中止，路径改由 REF-walk 再次处理。)

If RCU-walk finds that ``mount_lock`` hasn't changed then it can be sure
that, had REF-walk taken counted references on each vfsmount, the
results would have been the same.  This ensures the invariant holds,
at least for vfsmount structures.
(如果 RCU-walk 发现 mount_lock 没有变化，那么它可以确信：假如 REF-walk 对每个 vfsmount 都获取了计数引用，结果也会相同。这确保了不变式成立，至少对 vfsmount 结构而言。)

``dentry->d_seq`` and ``nd->seq``
(dentry->d_seq 与 nd->seq)
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

In place of taking a count or lock on ``d_reflock``, RCU-walk samples
the per-dentry ``d_seq`` seqlock, and stores the sequence number in the
``seq`` field of the nameidata structure, so ``nd->seq`` should always be
the current sequence number of ``nd->dentry``.  This number needs to be
revalidated after copying, and before using, the name, parent, or
inode of the dentry.
(RCU-walk 不对 d_reflock 获取计数或加锁，而是对每 dentry 的 d_seq 顺序锁进行采样，并把序列号存储在 nameidata 结构的 seq 字段中，因此 nd->seq 应当始终是 nd->dentry 当前的序列号。在复制 dentry 的名称、父项或 inode 之后、使用之前，需要对这个号码重新验证。)

The handling of the name we have already looked at, and the parent is
only accessed in ``follow_dotdot_rcu()`` which fairly trivially follows
the required pattern, though it does so for three different cases.
(对名称的处理我们已经看过了，而父项只在 follow_dotdot_rcu() 中被访问，该函数相当直白地遵循所需的模式，尽管它要针对三种不同情况这样做。)

When not at a mount point, ``d_parent`` is followed and its ``d_seq`` is
collected.  When we are at a mount point, we instead follow the
``mnt->mnt_mountpoint`` link to get a new dentry and collect its
``d_seq``.  Then, after finally finding a ``d_parent`` to follow, we must
check if we have landed on a mount point and, if so, must find that
mount point and follow the ``mnt->mnt_root`` link.  This would imply a
somewhat unusual, but certainly possible, circumstance where the
starting point of the path lookup was in part of the filesystem that
was mounted on, and so not visible from the root.
(不在挂载点上时，跟随 d_parent 并收集它的 d_seq。在挂载点上时，我们改为跟随 mnt->mnt_mountpoint 链接以获得新的 dentry 并收集它的 d_seq。然后，在最终找到要跟随的 d_parent 之后，我们必须检查是否落在了挂载点上，如果是，必须找到那个挂载点并跟随 mnt->mnt_root 链接。这意味着一种不太寻常但确实可能的情况：路径查找的起点位于文件系统中被挂载覆盖的部分，因而从根不可见。)

The inode pointer, stored in ``->d_inode``, is a little more
interesting.  The inode will always need to be accessed at least
twice, once to determine if it is NULL and once to verify access
permissions.  Symlink handling requires a validated inode pointer too.
Rather than revalidating on each access, a copy is made on the first
access and it is stored in the ``inode`` field of ``nameidata`` from where
it can be safely accessed without further validation.
(存储在 ->d_inode 中的 inode 指针则更有意思一些。inode 总是需要至少被访问两次：一次判断它是否为 NULL，一次验证访问权限。符号链接处理也需要经过验证的 inode 指针。与其在每次访问时都重新验证，不如在第一次访问时制作副本，并把它存储在 nameidata 的 inode 字段中，此后无需进一步验证即可安全访问。)

``lookup_fast()`` is the only lookup routine that is used in RCU-mode,
``lookup_slow()`` being too slow and requiring locks.  It is in
``lookup_fast()`` that we find the important "hand over hand" tracking
of the current dentry.
(lookup_fast() 是 RCU 模式下使用的唯一查找例程，lookup_slow() 太慢而且需要加锁。正是在 lookup_fast() 中，我们发现了对当前 dentry 重要的"手把手交接"式跟踪。)

The current ``dentry`` and current ``seq`` number are passed to
``__d_lookup_rcu()`` which, on success, returns a new ``dentry`` and a
new ``seq`` number.  ``lookup_fast()`` then copies the inode pointer and
revalidates the new ``seq`` number.  It then validates the old ``dentry``
with the old ``seq`` number one last time and only then continues.  This
process of getting the ``seq`` number of the new dentry and then
checking the ``seq`` number of the old exactly mirrors the process of
getting a counted reference to the new dentry before dropping that for
the old dentry which we saw in REF-walk.
(当前 dentry 和当前 seq 号被传给 __d_lookup_rcu()，后者在成功时返回新的 dentry 和新的 seq 号。然后 lookup_fast() 复制 inode 指针并重新验证新的 seq 号。接着它用旧的 seq 号对旧 dentry 做最后一次验证，然后才继续。这个先获取新 dentry 的 seq 号、再检查旧 dentry 的 seq 号的过程，恰好对应于我们在 REF-walk 中看到的、在放下旧 dentry 的引用之前先获取新 dentry 计数引用的过程。)

No ``inode->i_rwsem`` or even ``rename_lock``
(不用 inode->i_rwsem，甚至连 rename_lock 也不用)
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

A semaphore is a fairly heavyweight lock that can only be taken when it is
permissible to sleep.  As ``rcu_read_lock()`` forbids sleeping,
``inode->i_rwsem`` plays no role in RCU-walk.  If some other thread does
take ``i_rwsem`` and modifies the directory in a way that RCU-walk needs
to notice, the result will be either that RCU-walk fails to find the
dentry that it is looking for, or it will find a dentry which
``read_seqretry()`` won't validate.  In either case it will drop down to
REF-walk mode which can take whatever locks are needed.
(信号量是一种相当重量级的锁，只在允许睡眠时才能获取。由于 rcu_read_lock() 禁止睡眠，inode->i_rwsem 在 RCU-walk 中不起作用。如果某个其他线程确实获取了 i_rwsem 并以 RCU-walk 需要察觉的方式修改了目录，结果要么是 RCU-walk 找不到它正在查找的 dentry，要么它会找到一个 read_seqretry() 无法验证的 dentry。无论哪种情况，它都会降级到 REF-walk 模式，后者可以获取所需的任何锁。)

Though ``rename_lock`` could be used by RCU-walk as it doesn't require
any sleeping, RCU-walk doesn't bother.  REF-walk uses ``rename_lock`` to
protect against the possibility of hash chains in the dcache changing
while they are being searched.  This can result in failing to find
something that actually is there.  When RCU-walk fails to find
something in the dentry cache, whether it is really there or not, it
already drops down to REF-walk and tries again with appropriate
locking.  This neatly handles all cases, so adding extra checks on
rename_lock would bring no significant value.
(虽然 rename_lock 不需要任何睡眠、本可以被 RCU-walk 使用，但 RCU-walk 没有这样做。REF-walk 使用 rename_lock 来防范 dcache 中的哈希链在被搜索期间发生变化的可能性。这可能导致找不到实际存在的东西。当 RCU-walk 在 dentry 缓存中找不到某个东西时——无论它是否真的在那里——它本来就会降级到 REF-walk 并以适当的加锁重试。这已经巧妙地处理了所有情况，所以对 rename_lock 增加额外检查不会带来什么显著价值。)

``unlazy walk()`` and ``complete_walk()``
(unlazy walk() 与 complete_walk())
-----------------------------------------

That "dropping down to REF-walk" typically involves a call to
``unlazy_walk()``, so named because "RCU-walk" is also sometimes
referred to as "lazy walk".  ``unlazy_walk()`` is called when
following the path down to the current vfsmount/dentry pair seems to
have proceeded successfully, but the next step is problematic.  This
can happen if the next name cannot be found in the dcache, if
permission checking or name revalidation couldn't be achieved while
the ``rcu_read_lock()`` is held (which forbids sleeping), if an
automount point is found, or in a couple of cases involving symlinks.
It is also called from ``complete_walk()`` when the lookup has reached
the final component, or the very end of the path, depending on which
particular flavor of lookup is used.
(这种"降级到 REF-walk"通常涉及对 unlazy_walk() 的调用，之所以这样命名，是因为"RCU-walk"有时也被称为"惰性遍历"(lazy walk)。当沿路径向下走到当前 vfsmount/dentry 对这一步看起来已经成功、但下一步有问题时，会调用 unlazy_walk()。这可能发生在：下一个名称在 dcache 中找不到；在持有 rcu_read_lock()(它禁止睡眠)期间无法完成权限检查或名称重新验证；发现了自动挂载点；或涉及符号链接的少数几种情况。当查找到达最后组件、或路径的最末端时(取决于使用的是哪种特定风格的查找)，complete_walk() 也会调用它。)

Other reasons for dropping out of RCU-walk that do not trigger a call
to ``unlazy_walk()`` are when some inconsistency is found that cannot be
handled immediately, such as ``mount_lock`` or one of the ``d_seq``
seqlocks reporting a change.  In these cases the relevant function
will return ``-ECHILD`` which will percolate up until it triggers a new
attempt from the top using REF-walk.
(其他退出 RCU-walk 而不触发调用 unlazy_walk() 的原因包括：发现了无法立即处理的某种不一致，例如 mount_lock 或某个 d_seq 顺序锁报告发生变化。在这些情况下，相关函数将返回 -ECHILD，它会向上层层传递，直到触发一次从顶部使用 REF-walk 的新尝试。)

For those cases where ``unlazy_walk()`` is an option, it essentially
takes a reference on each of the pointers that it holds (vfsmount,
dentry, and possibly some symbolic links) and then verifies that the
relevant seqlocks have not been changed.  If there have been changes,
it, too, aborts with ``-ECHILD``, otherwise the transition to REF-walk
has been a success and the lookup process continues.
(对于那些 unlazy_walk() 可行的情况，它实质上对它持有的每个指针(vfsmount、dentry，可能还有一些符号链接)获取引用，然后验证相关的顺序锁没有被更改。如果发生过更改，它同样以 -ECHILD 中止；否则到 REF-walk 的转换就成功了，查找过程继续。)

Taking a reference on those pointers is not quite as simple as just
incrementing a counter.  That works to take a second reference if you
already have one (often indirectly through another object), but it
isn't sufficient if you don't actually have a counted reference at
all.  For ``dentry->d_lockref``, it is safe to increment the reference
counter to get a reference unless it has been explicitly marked as
"dead" which involves setting the counter to ``-128``.
``lockref_get_not_dead()`` achieves this.
(对这些指针获取引用并不像仅仅递增计数器那么简单。如果你已经持有一个引用(通常通过另一个对象间接持有)，递增计数器即可获得第二个引用；但如果你根本没有计数引用，这就不够了。对于 dentry->d_lockref，递增引用计数来获取引用是安全的，除非它已被明确标记为"死亡"(这会把计数器设为 -128)。lockref_get_not_dead() 实现了这一点。)

For ``mnt->mnt_count`` it is safe to take a reference as long as
``mount_lock`` is then used to validate the reference.  If that
validation fails, it may *not* be safe to just drop that reference in
the standard way of calling ``mnt_put()`` - an unmount may have
progressed too far.  So the code in ``legitimize_mnt()``, when it
finds that the reference it got might not be safe, checks the
``MNT_SYNC_UMOUNT`` flag to determine if a simple ``mnt_put()`` is
correct, or if it should just decrement the count and pretend none of
this ever happened.
(对于 mnt->mnt_count，只要随后用 mount_lock 验证该引用，获取引用就是安全的。如果验证失败，以调用 mnt_put() 的标准方式放下该引用可能*不*安全——卸载可能已经进行得太远。所以当 legitimize_mnt() 中的代码发现它获得的引用可能不安全时，会检查 MNT_SYNC_UMOUNT 标志，以确定简单的 mnt_put() 是否正确，或者它是否应该只递减计数并假装这一切从未发生过。)

Taking care in filesystems
(在文件系统中要小心)
--------------------------

RCU-walk depends almost entirely on cached information and often will
not call into the filesystem at all.  However there are two places,
besides the already-mentioned component-name comparison, where the
file system might be included in RCU-walk, and it must know to be
careful.
(RCU-walk 几乎完全依赖缓存的信息，往往根本不会调用文件系统。不过，除了前面提到的组件名比较之外，还有两个地方可能会在 RCU-walk 中涉及文件系统，文件系统必须知道要小心行事。)

If the filesystem has non-standard permission-checking requirements -
such as a networked filesystem which may need to check with the server
- the ``i_op->permission`` interface might be called during RCU-walk.
In this case an extra "``MAY_NOT_BLOCK``" flag is passed so that it
knows not to sleep, but to return ``-ECHILD`` if it cannot complete
promptly.  ``i_op->permission`` is given the inode pointer, not the
dentry, so it doesn't need to worry about further consistency checks.
However if it accesses any other filesystem data structures, it must
ensure they are safe to be accessed with only the ``rcu_read_lock()``
held.  This typically means they must be freed using ``kfree_rcu()`` or
similar.
(如果文件系统有非标准的权限检查需求——例如可能需要与服务器核对的网络文件系统——那么 i_op->permission 接口可能在 RCU-walk 期间被调用。在这种情况下会传递一个额外的"MAY_NOT_BLOCK"标志，让它知道不要睡眠，如果不能迅速完成则返回 -ECHILD。i_op->permission 得到的是 inode 指针而不是 dentry，所以它不需要担心进一步的一致性检查。不过，如果它访问任何其他文件系统数据结构，它必须确保在仅持有 rcu_read_lock() 的情况下访问它们是安全的。这通常意味着必须用 kfree_rcu() 或类似机制来释放它们。)

.. _READ_ONCE: https://lwn.net/Articles/624126/

If the filesystem may need to revalidate dcache entries, then
``d_op->d_revalidate`` may be called in RCU-walk too.  This interface
*is* passed the dentry but does not have access to the ``inode`` or the
``seq`` number from the ``nameidata``, so it needs to be extra careful
when accessing fields in the dentry.  This "extra care" typically
involves using  `READ_ONCE() <READ_ONCE_>`_ to access fields, and verifying the
result is not NULL before using it.  This pattern can be seen in
``nfs_lookup_revalidate()``.
(如果文件系统可能需要重新验证 dcache 条目，那么 d_op->d_revalidate 也可能在 RCU-walk 中被调用。这个接口*确实*会被传入 dentry，但无法访问 nameidata 中的 inode 或 seq 号，所以它在访问 dentry 中的字段时需要格外小心。这种"格外小心"通常包括使用 READ_ONCE() 访问字段，并在使用结果之前验证它不是 NULL。这种模式可见于 nfs_lookup_revalidate()。)

A pair of patterns
(一对模式)
------------------

In various places in the details of REF-walk and RCU-walk, and also in
the big picture, there are a couple of related patterns that are worth
being aware of.
(在 REF-walk 和 RCU-walk 的细节以及整体图景中的许多地方，有几个相关的模式值得留意。)

The first is "try quickly and check, if that fails try slowly".  We
can see that in the high-level approach of first trying RCU-walk and
then trying REF-walk, and in places where ``unlazy_walk()`` is used to
switch to REF-walk for the rest of the path.  We also saw it earlier
in ``dget_parent()`` when following a "``..``" link.  It tries a quick way
to get a reference, then falls back to taking locks if needed.
(第一个是"先快速尝试并检查，若失败则慢慢来"。我们可以在先试 RCU-walk 再试 REF-walk 的高层方法中看到它，也可以在使用 unlazy_walk() 切换到 REF-walk 处理路径其余部分的地方看到它。我们还在前面跟随".."链接时的 dget_parent() 中见过它。它先尝试一种快速获取引用的方式，然后在需要时退回到加锁。)

The second pattern is "try quickly and check, if that fails try
again - repeatedly".  This is seen with the use of ``rename_lock`` and
``mount_lock`` in REF-walk.  RCU-walk doesn't make use of this pattern -
if anything goes wrong it is much safer to just abort and try a more
sedate approach.
(第二个模式是"先快速尝试并检查，若失败则再试——反复进行"。这在 REF-walk 对 rename_lock 和 mount_lock 的使用中可以看到。RCU-walk 不使用这个模式——只要出了任何问题，直接中止并尝试更稳妥的方法要安全得多。)

The emphasis here is "try quickly and check".  It should probably be
"try quickly *and carefully*, then check".  The fact that checking is
needed is a reminder that the system is dynamic and only a limited
number of things are safe at all.  The most likely cause of errors in
this whole process is assuming something is safe when in reality it
isn't.  Careful consideration of what exactly guarantees the safety of
each access is sometimes necessary.
(这里的重点是"快速尝试并检查"。或许应该是"快速*且小心地*尝试，然后检查"。需要检查这一事实提醒我们：系统是动态的，只有为数不多的东西才是真正安全的。这整个过程中最可能的错误原因，是想当然地认为某物安全而实际上并不安全。有时需要仔细考虑究竟是什么保证了每次访问的安全性。)

A walk among the symlinks
(符号链接之间的漫步)
=========================

There are several basic issues that we will examine to understand the
handling of symbolic links:  the symlink stack, together with cache
lifetimes, will help us understand the overall recursive handling of
symlinks and lead to the special care needed for the final component.
Then a consideration of access-time updates and summary of the various
flags controlling lookup will finish the story.
(为了理解符号链接的处理，我们将考察几个基本问题：符号链接栈连同缓存生命周期，将帮助我们理解符号链接的整体递归处理，并引出最后一个组件所需的特殊小心。然后，对访问时间更新的考虑以及对控制查找的各种标志的总结将为这个故事收尾。)

The symlink stack
(符号链接栈)
-----------------

There are only two sorts of filesystem objects that can usefully
appear in a path prior to the final component: directories and symlinks.
Handling directories is quite straightforward: the new directory
simply becomes the starting point at which to interpret the next
component on the path.  Handling symbolic links requires a bit more
work.
(在最后组件之前的路径中，只有两类文件系统对象能有意义地出现：目录和符号链接。处理目录相当直接：新目录只是成为解释路径上下一个组件的起点。处理符号链接则需要多一点工作。)

Conceptually, symbolic links could be handled by editing the path.  If
a component name refers to a symbolic link, then that component is
replaced by the body of the link and, if that body starts with a '/',
then all preceding parts of the path are discarded.  This is what the
"``readlink -f``" command does, though it also edits out "``.``" and
"``..``" components.
(从概念上讲，符号链接可以通过编辑路径来处理。如果组件名指向一个符号链接，那么该组件会被链接的主体替换，并且如果该主体以'/'开头，则路径中所有前面的部分都被丢弃。这正是"readlink -f"命令所做的，尽管它还会把"."和".."组件也一并编辑掉。)

Directly editing the path string is not really necessary when looking
up a path, and discarding early components is pointless as they aren't
looked at anyway.  Keeping track of all remaining components is
important, but they can of course be kept separately; there is no need
to concatenate them.  As one symlink may easily refer to another,
which in turn can refer to a third, we may need to keep the remaining
components of several paths, each to be processed when the preceding
ones are completed.  These path remnants are kept on a stack of
limited size.
(查找路径时其实并不需要直接编辑路径字符串，而丢弃前面的组件也没有意义，因为反正不会再查看它们。跟踪所有剩余组件很重要，但它们当然可以分开保存；没有必要把它们拼接起来。由于一个符号链接很容易引用另一个符号链接，而后者又可能引用第三个，我们可能需要保留若干路径的剩余组件，每个都在前一个处理完成后才被处理。这些路径残余保存在一个大小受限的栈上。)

There are two reasons for placing limits on how many symlinks can
occur in a single path lookup.  The most obvious is to avoid loops.
If a symlink referred to itself either directly or through
intermediaries, then following the symlink can never complete
successfully - the error ``ELOOP`` must be returned.  Loops can be
detected without imposing limits, but limits are the simplest solution
and, given the second reason for restriction, quite sufficient.
(对单次路径查找中可出现的符号链接数量施加限制有两个原因。最明显的是避免循环。如果一个符号链接直接或通过中介引用自身，那么跟随该符号链接永远无法成功完成——必须返回错误 ELOOP。循环可以在不施加限制的情况下检测出来，但限制是最简单的解决方案，而且考虑到限制的第二个原因，它已相当足够。)

.. _outlined recently: http://thread.gmane.org/gmane.linux.kernel/1934390/focus=1934550

The second reason was `outlined recently`_ by Linus:
(第二个原因由 Linus 近期概述过：)

   Because it's a latency and DoS issue too. We need to react well to
   true loops, but also to "very deep" non-loops. It's not about memory
   use, it's about users triggering unreasonable CPU resources.
   (因为它也是一个延迟和 DoS 问题。我们需要对真正的循环以及"非常深"的非循环都做出良好反应。这不是内存使用的问题，而是用户触发不合理 CPU 资源的问题。)

Linux imposes a limit on the length of any pathname: ``PATH_MAX``, which
is 4096.  There are a number of reasons for this limit; not letting the
kernel spend too much time on just one path is one of them.  With
symbolic links you can effectively generate much longer paths so some
sort of limit is needed for the same reason.  Linux imposes a limit of
at most 40 (MAXSYMLINKS) symlinks in any one path lookup.  It previously imposed
a further limit of eight on the maximum depth of recursion, but that was
raised to 40 when a separate stack was implemented, so there is now
just the one limit.
(Linux 对任何路径名的长度施加限制：PATH_MAX，即 4096。这个限制有多个原因；不让内核在仅仅一个路径上花费太多时间是其中之一。借助符号链接，你可以有效地生成长得多的路径，所以出于同样的原因需要某种限制。Linux 对任何一次路径查找施加最多 40 个(MAXSYMLINKS)符号链接的限制。它此前还对最大递归深度施加了 8 的额外限制，但在实现了单独的栈之后，这个限制被提高到 40，所以现在只剩下这一个限制。)

The ``nameidata`` structure that we met in an earlier article contains a
small stack that can be used to store the remaining part of up to two
symlinks.  In many cases this will be sufficient.  If it isn't, a
separate stack is allocated with room for 40 symlinks.  Pathname
lookup will never exceed that stack as, once the 40th symlink is
detected, an error is returned.
(我们在前面文章中见过的 nameidata 结构包含一个小栈，可用于存储最多两个符号链接的剩余部分。在许多情况下这就足够了。如果不够，会分配一个可容纳 40 个符号链接的单独栈。路径名查找永远不会超出那个栈，因为一旦检测到第 40 个符号链接，就会返回错误。)

It might seem that the name remnants are all that needs to be stored on
this stack, but we need a bit more.  To see that, we need to move on to
cache lifetimes.
(看起来似乎名称残余就是需要存储在这个栈上的全部，但我们还需要多一点点。要理解这一点，我们需要转向缓存生命周期。)

Storage and lifetime of cached symlinks
(缓存符号链接的存储与生命周期)
---------------------------------------

Like other filesystem resources, such as inodes and directory
entries, symlinks are cached by Linux to avoid repeated costly access
to external storage.  It is particularly important for RCU-walk to be
able to find and temporarily hold onto these cached entries, so that
it doesn't need to drop down into REF-walk.
(与其他文件系统资源(如 inode 和目录项)一样，符号链接也被 Linux 缓存，以避免反复进行代价高昂的外部存储访问。对 RCU-walk 而言，能够找到并暂时持有这些缓存条目尤其重要，这样它就不需要降级到 REF-walk。)

.. _object-oriented design pattern: https://lwn.net/Articles/446317/

While each filesystem is free to make its own choice, symlinks are
typically stored in one of two places.  Short symlinks are often
stored directly in the inode.  When a filesystem allocates a ``struct
inode`` it typically allocates extra space to store private data (a
common `object-oriented design pattern`_ in the kernel).  This will
sometimes include space for a symlink.  The other common location is
in the page cache, which normally stores the content of files.  The
pathname in a symlink can be seen as the content of that symlink and
can easily be stored in the page cache just like file content.
(虽然每个文件系统都可以自由选择，但符号链接通常存储在两个地方之一。短符号链接经常直接存储在 inode 中。当文件系统分配一个 struct inode 时，它通常会分配额外的空间来存储私有数据(内核中一种常见的面向对象设计模式)。这有时会包括存放符号链接的空间。另一个常见位置是页缓存，它通常存储文件的内容。符号链接中的路径名可以看作该符号链接的内容，可以像文件内容一样方便地存储在页缓存中。)

When neither of these is suitable, the next most likely scenario is
that the filesystem will allocate some temporary memory and copy or
construct the symlink content into that memory whenever it is needed.
(当这两者都不合适时，次可能的情形是：文件系统分配一些临时内存，并在需要时把符号链接内容复制或构造到那块内存中。)

When the symlink is stored in the inode, it has the same lifetime as
the inode which, itself, is protected by RCU or by a counted reference
on the dentry.  This means that the mechanisms that pathname lookup
uses to access the dcache and icache (inode cache) safely are quite
sufficient for accessing some cached symlinks safely.  In these cases,
the ``i_link`` pointer in the inode is set to point to wherever the
symlink is stored and it can be accessed directly whenever needed.
(当符号链接存储在 inode 中时，它的生命周期与 inode 相同，而 inode 本身由 RCU 或 dentry 上的计数引用保护。这意味着路径名查找用来安全访问 dcache 和 icache(inode 缓存)的机制，足以安全访问某些缓存的符号链接。在这些情况下，inode 中的 i_link 指针被设置为指向符号链接的存储位置，可以在需要时直接访问。)

When the symlink is stored in the page cache or elsewhere, the
situation is not so straightforward.  A reference on a dentry or even
on an inode does not imply any reference on cached pages of that
inode, and even an ``rcu_read_lock()`` is not sufficient to ensure that
a page will not disappear.  So for these symlinks the pathname lookup
code needs to ask the filesystem to provide a stable reference and,
significantly, needs to release that reference when it is finished
with it.
(当符号链接存储在页缓存或其他地方时，情况就没那么直接了。对 dentry 甚至 inode 的引用并不意味着对该 inode 缓存页的任何引用，甚至 rcu_read_lock() 也不足以确保一个页不会消失。所以对这些符号链接，路径名查找代码需要请求文件系统提供一个稳定的引用，而且重要的是，用完之后需要释放该引用。)

Taking a reference to a cache page is often possible even in RCU-walk
mode.  It does require making changes to memory, which is best avoided,
but that isn't necessarily a big cost and it is better than dropping
out of RCU-walk mode completely.  Even filesystems that allocate
space to copy the symlink into can use ``GFP_ATOMIC`` to often successfully
allocate memory without the need to drop out of RCU-walk.  If a
filesystem cannot successfully get a reference in RCU-walk mode, it
must return ``-ECHILD`` and ``unlazy_walk()`` will be called to return to
REF-walk mode in which the filesystem is allowed to sleep.
(即使在 RCU-walk 模式下，获取对缓存页的引用通常也是可能的。它确实需要改动内存，这最好避免，但这不一定是很大的代价，而且比完全退出 RCU-walk 模式要好。即使是那些分配空间来复制符号链接的文件系统，也可以使用 GFP_ATOMIC 来经常成功地分配内存，而无需退出 RCU-walk。如果文件系统无法在 RCU-walk 模式下成功获取引用，它必须返回 -ECHILD，随后会调用 unlazy_walk() 返回到允许文件系统睡眠的 REF-walk 模式。)

The place for all this to happen is the ``i_op->get_link()`` inode
method. This is called both in RCU-walk and REF-walk. In RCU-walk the
``dentry*`` argument is NULL, ``->get_link()`` can return -ECHILD to drop out of
RCU-walk.  Much like the ``i_op->permission()`` method we
looked at previously, ``->get_link()`` would need to be careful that
all the data structures it references are safe to be accessed while
holding no counted reference, only the RCU lock. A callback
``struct delayed_called`` will be passed to ``->get_link()``:
file systems can set their own put_link function and argument through
set_delayed_call(). Later on, when VFS wants to put link, it will call
do_delayed_call() to invoke that callback function with the argument.
(这一切发生的地方是 i_op->get_link() inode 方法。它在 RCU-walk 和 REF-walk 中都会被调用。在 RCU-walk 中，dentry* 参数为 NULL，->get_link() 可以返回 -ECHILD 以退出 RCU-walk。与我们前面看过的 i_op->permission() 方法非常相似，->get_link() 需要小心确保它引用的所有数据结构在不持有计数引用、只持有 RCU 锁的情况下可以安全访问。一个回调 struct delayed_called 会被传给 ->get_link()：文件系统可以通过 set_delayed_call() 设置自己的 put_link 函数和参数。稍后，当 VFS 想要放下链接时，它会调用 do_delayed_call() 以该参数调用那个回调函数。)

In order for the reference to each symlink to be dropped when the walk completes,
whether in RCU-walk or REF-walk, the symlink stack needs to contain,
along with the path remnants:
(为了在遍历完成时——无论在 RCU-walk 还是 REF-walk 中——能够放下对每个符号链接的引用，符号链接栈除了路径残余之外还需要包含：)

- the ``struct path`` to provide a reference to the previous path
  (struct path，用于提供对前一路径的引用)
- the ``const char *`` to provide a reference to the to previous name
  (const char \*，用于提供对前一名称的引用)
- the ``seq`` to allow the path to be safely switched from RCU-walk to REF-walk
  (seq，用于允许路径从 RCU-walk 安全地切换到 REF-walk)
- the ``struct delayed_call`` for later invocation.
  (struct delayed_call，供以后调用。)

This means that each entry in the symlink stack needs to hold five
pointers and an integer instead of just one pointer (the path
remnant).  On a 64-bit system, this is about 40 bytes per entry;
with 40 entries it adds up to 1600 bytes total, which is less than
half a page.  So it might seem like a lot, but is by no means
excessive.
(这意味着符号链接栈中的每个条目需要保存五个指针和一个整数，而不是只有一个指针(路径残余)。在 64 位系统上，这大约是每条目 40 字节；40 个条目总计约 1600 字节，不到半页。所以这看起来可能不少，但绝不过分。)

Note that, in a given stack frame, the path remnant (``name``) is not
part of the symlink that the other fields refer to.  It is the remnant
to be followed once that symlink has been fully parsed.
(注意，在给定的栈帧中，路径残余(name)并不是其他字段所指的那个符号链接的一部分。它是那个符号链接被完全解析之后要跟随的残余。)

Following the symlink
(跟随符号链接)
---------------------

The main loop in ``link_path_walk()`` iterates seamlessly over all
components in the path and all of the non-final symlinks.  As symlinks
are processed, the ``name`` pointer is adjusted to point to a new
symlink, or is restored from the stack, so that much of the loop
doesn't need to notice.  Getting this ``name`` variable on and off the
stack is very straightforward; pushing and popping the references is
a little more complex.
(link_path_walk() 中的主循环无缝地遍历路径中的所有组件和所有非最后的符号链接。随着符号链接被处理，name 指针会被调整为指向新的符号链接，或从栈中恢复，因此循环的大部分无需察觉。把这个 name 变量压栈和出栈非常直接；引用的压栈和出栈则稍微复杂一些。)

When a symlink is found, walk_component() calls pick_link() via step_into()
which returns the link from the filesystem.
Providing that operation is successful, the old path ``name`` is placed on the
stack, and the new value is used as the ``name`` for a while.  When the end of
the path is found (i.e. ``*name`` is ``'\0'``) the old ``name`` is restored
off the stack and path walking continues.
(当发现符号链接时，walk_component() 通过 step_into() 调用 pick_link()，后者从文件系统返回链接。只要该操作成功，旧的路径 name 就被放到栈上，新值暂时用作 name。当发现路径末尾(即 \*name 为 '\0')时，旧的 name 从栈中恢复，路径遍历继续。)

Pushing and popping the reference pointers (inode, cookie, etc.) is more
complex in part because of the desire to handle tail recursion.  When
the last component of a symlink itself points to a symlink, we
want to pop the symlink-just-completed off the stack before pushing
the symlink-just-found to avoid leaving empty path remnants that would
just get in the way.
(引用指针(inode、cookie 等)的压栈和出栈更复杂，部分原因在于希望处理尾递归。当符号链接的最后组件本身指向另一个符号链接时，我们希望在压入刚发现的符号链接之前，先把刚完成的符号链接弹出栈，以避免留下只会碍事的空路径残余。)

It is most convenient to push the new symlink references onto the
stack in ``walk_component()`` immediately when the symlink is found;
``walk_component()`` is also the last piece of code that needs to look at the
old symlink as it walks that last component.  So it is quite
convenient for ``walk_component()`` to release the old symlink and pop
the references just before pushing the reference information for the
new symlink.  It is guided in this by three flags: ``WALK_NOFOLLOW`` which
forbids it from following a symlink if it finds one, ``WALK_MORE``
which indicates that it is yet too early to release the
current symlink, and ``WALK_TRAILING`` which indicates that it is on the final
component of the lookup, so we will check userspace flag ``LOOKUP_FOLLOW`` to
decide whether follow it when it is a symlink and call ``may_follow_link()`` to
check if we have privilege to follow it.
(最方便的做法是：在发现符号链接时立即在 walk_component() 中把新符号链接的引用压入栈；walk_component() 也是在走过那个最后组件时需要查看旧符号链接的最后一段代码。所以对 walk_component() 来说，在压入新符号链接的引用信息之前释放旧符号链接并弹出其引用是相当方便的。它由三个标志引导：WALK_NOFOLLOW 禁止它在发现符号链接时跟随它；WALK_MORE 表示释放当前符号链接还为时过早；WALK_TRAILING 表示它处于查找的最后组件上，因此我们会检查用户空间标志 LOOKUP_FOLLOW 来决定当它是符号链接时是否跟随，并调用 may_follow_link() 检查我们是否有跟随它的权限。)

Symlinks with no final component
(没有最后组件的符号链接)
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

A pair of special-case symlinks deserve a little further explanation.
Both result in a new ``struct path`` (with mount and dentry) being set
up in the ``nameidata``, and result in pick_link() returning ``NULL``.
(有两个特殊情况的符号链接值得进一步解释。两者都会导致在 nameidata 中建立一个新的 struct path(带 mount 和 dentry)，并导致 pick_link() 返回 NULL。)

The more obvious case is a symlink to "``/``".  All symlinks starting
with "``/``" are detected in pick_link() which resets the ``nameidata``
to point to the effective filesystem root.  If the symlink only
contains "``/``" then there is nothing more to do, no components at all,
so ``NULL`` is returned to indicate that the symlink can be released and
the stack frame discarded.
(更明显的情况是指向"/"的符号链接。所有以"/"开头的符号链接都在 pick_link() 中被检测到，它会把 nameidata 重置为指向有效的文件系统根。如果符号链接只包含"/"，那么就没有更多要做的了——完全没有任何组件——所以返回 NULL 表示该符号链接可以被释放、栈帧可以被丢弃。)

The other case involves things in ``/proc`` that look like symlinks but
aren't really (and are therefore commonly referred to as "magic-links")::
(另一种情况涉及 /proc 中那些看起来像符号链接但实际并不是的东西(因此通常被称为"magic-links"，魔链)：)

     $ ls -l /proc/self/fd/1
     lrwx------ 1 neilb neilb 64 Jun 13 10:19 /proc/self/fd/1 -> /dev/pts/4

Every open file descriptor in any process is represented in ``/proc`` by
something that looks like a symlink.  It is really a reference to the
target file, not just the name of it.  When you ``readlink`` these
objects you get a name that might refer to the same file - unless it
has been unlinked or mounted over.  When ``walk_component()`` follows
one of these, the ``->get_link()`` method in "procfs" doesn't return
a string name, but instead calls nd_jump_link() which updates the
``nameidata`` in place to point to that target.  ``->get_link()`` then
returns ``NULL``.  Again there is no final component and pick_link()
returns ``NULL``.
(任何进程中每个打开的文件描述符都在 /proc 中由看起来像符号链接的东西表示。它实际上是对目标文件的引用，而不仅仅是它的名称。当你对这些对象执行 readlink 时，会得到一个可能指向同一文件的名称——除非它已被解除链接或被挂载覆盖。当 walk_component() 跟随其中之一时，"procfs"中的 ->get_link() 方法不返回字符串名称，而是调用 nd_jump_link()，它就地更新 nameidata 使其指向那个目标。然后 ->get_link() 返回 NULL。同样，这里没有最后组件，pick_link() 返回 NULL。)

Following the symlink in the final component
(在最后组件中跟随符号链接)
--------------------------------------------

All this leads to ``link_path_walk()`` walking down every component, and
following all symbolic links it finds, until it reaches the final
component.  This is just returned in the ``last`` field of ``nameidata``.
For some callers, this is all they need; they want to create that
``last`` name if it doesn't exist or give an error if it does.  Other
callers will want to follow a symlink if one is found, and possibly
apply special handling to the last component of that symlink, rather
than just the last component of the original file name.  These callers
potentially need to call ``link_path_walk()`` again and again on
successive symlinks until one is found that doesn't point to another
symlink.
(所有这些使得 link_path_walk() 走过每一个组件，并跟随它发现的所有符号链接，直到到达最后组件。它只是在 nameidata 的 last 字段中返回。对某些调用者来说，这就是它们需要的全部；如果该 last 名称不存在，它们想创建它；如果存在，则给出错误。其他调用者会想在发现符号链接时跟随它，并且可能对该符号链接的最后组件(而不仅仅是原始文件名的最后组件)应用特殊处理。这些调用者可能需要对相继的符号链接一次又一次地调用 link_path_walk()，直到发现一个不指向另一个符号链接的链接。)

This case is handled by relevant callers of link_path_walk(), such as
path_lookupat(), path_openat() using a loop that calls link_path_walk(),
and then handles the final component by calling open_last_lookups() or
lookup_last(). If it is a symlink that needs to be followed,
open_last_lookups() or lookup_last() will set things up properly and
return the path so that the loop repeats, calling
link_path_walk() again.  This could loop as many as 40 times if the last
component of each symlink is another symlink.
(这种情况由 link_path_walk() 的相关调用者处理，例如 path_lookupat()、path_openat()，它们使用一个循环来调用 link_path_walk()，然后通过调用 open_last_lookups() 或 lookup_last() 处理最后组件。如果它是一个需要跟随的符号链接，open_last_lookups() 或 lookup_last() 会妥善设置一切并返回路径，以便循环重复、再次调用 link_path_walk()。如果每个符号链接的最后组件都是另一个符号链接，这个循环最多可达 40 次。)

Of the various functions that examine the final component, 
open_last_lookups() is the most interesting as it works in tandem
with do_open() for opening a file.  Part of open_last_lookups() runs
with ``i_rwsem`` held and this part is in a separate function: lookup_open().
(在检查最后组件的各种函数中，open_last_lookups() 最有意思，因为它与 do_open() 协同工作来打开文件。open_last_lookups() 的一部分在持有 i_rwsem 的情况下运行，这一部分位于一个单独的函数中：lookup_open()。)

Explaining open_last_lookups() and do_open() completely is beyond the scope
of this article, but a few highlights should help those interested in exploring
the code.
(完整解释 open_last_lookups() 和 do_open() 超出了本文的范围，但几个亮点应该能帮助有兴趣探索代码的人。)

1. Rather than just finding the target file, do_open() is used after
   open_last_lookup() to open
   it.  If the file was found in the dcache, then ``vfs_open()`` is used for
   this.  If not, then ``lookup_open()`` will either call ``atomic_open()`` (if
   the filesystem provides it) to combine the final lookup with the open, or
   will perform the separate ``i_op->lookup()`` and ``i_op->create()`` steps
   directly.  In the later case the actual "open" of this newly found or
   created file will be performed by vfs_open(), just as if the name
   were found in the dcache.
   (do_open() 不仅仅用于找到目标文件：它在 open_last_lookup() 之后被用来打开目标。如果文件是在 dcache 中找到的，则使用 vfs_open() 来打开。如果不是，则 lookup_open() 要么调用 atomic_open()(如果文件系统提供它)把最后的查找与打开合并起来，要么直接执行单独的 i_op->lookup() 和 i_op->create() 步骤。在后一种情况下，这个新找到或新建的文件的实际"打开"将由 vfs_open() 执行，就像名称是在 dcache 中找到的一样。)

2. vfs_open() can fail with ``-EOPENSTALE`` if the cached information
   wasn't quite current enough.  If it's in RCU-walk ``-ECHILD`` will be returned
   otherwise ``-ESTALE`` is returned.  When ``-ESTALE`` is returned, the caller may
   retry with ``LOOKUP_REVAL`` flag set.
   (如果缓存的信息不够新，vfs_open() 可能以 -EOPENSTALE 失败。如果处于 RCU-walk，将返回 -ECHILD，否则返回 -ESTALE。当返回 -ESTALE 时，调用者可以设置 LOOKUP_REVAL 标志重试。)

3. An open with O_CREAT **does** follow a symlink in the final component,
   unlike other creation system calls (like ``mkdir``).  So the sequence::
   (与其他创建类系统调用(如 mkdir)不同，带 O_CREAT 的打开**确实**会跟随最后组件中的符号链接。因此，如下序列：)

          ln -s bar /tmp/foo
          echo hello > /tmp/foo

   will create a file called ``/tmp/bar``.  This is not permitted if
   ``O_EXCL`` is set but otherwise is handled for an O_CREAT open much
   like for a non-creating open: lookup_last() or open_last_lookup()
   returns a non ``NULL`` value, and link_path_walk() gets called and the
   open process continues on the symlink that was found.
   (将创建一个名为 /tmp/bar 的文件。如果设置了 O_EXCL 则不允许这样做，但在其他方面，对 O_CREAT 打开的处理与非创建性打开非常相似：lookup_last() 或 open_last_lookup() 返回非 NULL 值，随后调用 link_path_walk()，打开过程在发现的那个符号链接上继续。)

Updating the access time
(更新访问时间)
------------------------

We previously said of RCU-walk that it would "take no locks, increment
no counts, leave no footprints."  We have since seen that some
"footprints" can be needed when handling symlinks as a counted
reference (or even a memory allocation) may be needed.  But these
footprints are best kept to a minimum.
(我们前面谈到 RCU-walk 时说它"不加锁、不增计数、不留足迹"。此后我们看到，在处理符号链接时可能需要一些"足迹"，因为可能需要计数引用(甚至内存分配)。但这些足迹最好保持在最少。)

One other place where walking down a symlink can involve leaving
footprints in a way that doesn't affect directories is in updating access times.
In Unix (and Linux) every filesystem object has a "last accessed
time", or "``atime``".  Passing through a directory to access a file
within is not considered to be an access for the purposes of
``atime``; only listing the contents of a directory can update its ``atime``.
Symlinks are different it seems.  Both reading a symlink (with ``readlink()``)
and looking up a symlink on the way to some other destination can
update the atime on that symlink.
(沿符号链接下行可能以不影响目录的方式留下足迹的另一个地方，是更新访问时间。在 Unix(和 Linux)中，每个文件系统对象都有一个"最后访问时间"，即"atime"。为访问目录内的文件而经过该目录，就 atime 而言不算访问；只有列出目录的内容才能更新它的 atime。而符号链接似乎不同。读取符号链接(通过 readlink())以及在前往其他目的地的途中查找符号链接，都可能更新该符号链接的 atime。)

.. _clearest statement: https://pubs.opengroup.org/onlinepubs/9699919799/basedefs/V1_chap04.html#tag_04_08

It is not clear why this is the case; POSIX has little to say on the
subject.  The `clearest statement`_ is that, if a particular implementation
updates a timestamp in a place not specified by POSIX, this must be
documented "except that any changes caused by pathname resolution need
not be documented".  This seems to imply that POSIX doesn't really
care about access-time updates during pathname lookup.
(为什么会这样并不清楚；POSIX 对此几乎没有论述。最清晰的表述是：如果某个特定实现在 POSIX 未规定的地方更新时间戳，这必须记录在案"但路径名解析引起的任何更改无需记录"。这似乎意味着 POSIX 并不真正关心路径名查找期间的访问时间更新。)

.. _Linux 1.3.87: https://git.kernel.org/cgit/linux/kernel/git/history/history.git/diff/fs/ext2/symlink.c?id=f806c6db77b8eaa6e00dcfb6b567706feae8dbb8

An examination of history shows that prior to `Linux 1.3.87`_, the ext2
filesystem, at least, didn't update atime when following a link.
Unfortunately we have no record of why that behavior was changed.
(对历史的考察表明，在 Linux 1.3.87 之前，至少 ext2 文件系统在跟随链接时不更新 atime。遗憾的是，我们没有关于该行为为何被更改的记录。)

In any case, access time must now be updated and that operation can be
quite complex.  Trying to stay in RCU-walk while doing it is best
avoided.  Fortunately it is often permitted to skip the ``atime``
update.  Because ``atime`` updates cause performance problems in various
areas, Linux supports the ``relatime`` mount option, which generally
limits the updates of ``atime`` to once per day on files that aren't
being changed (and symlinks never change once created).  Even without
``relatime``, many filesystems record ``atime`` with a one-second
granularity, so only one update per second is required.
(无论如何，现在必须更新访问时间，而这个操作可能相当复杂。在执行它时还想停留在 RCU-walk 中，最好避免。幸运的是，跳过 atime 更新通常是允许的。由于 atime 更新会在多个方面造成性能问题，Linux 支持 relatime 挂载选项，它通常把对未被更改文件的 atime 更新限制为每天一次(而符号链接一旦创建就永不更改)。即使没有 relatime，许多文件系统也以一秒的粒度记录 atime，所以每秒只需一次更新。)

It is easy to test if an ``atime`` update is needed while in RCU-walk
mode and, if it isn't, the update can be skipped and RCU-walk mode
continues.  Only when an ``atime`` update is actually required does the
path walk drop down to REF-walk.  All of this is handled in the
``get_link()`` function.
(在 RCU-walk 模式下很容易测试是否需要 atime 更新；如果不需要，可以跳过更新并继续 RCU-walk 模式。只有当确实需要 atime 更新时，路径遍历才会降级到 REF-walk。所有这些都在 get_link() 函数中处理。)

A few flags
(若干标志)
-----------

A suitable way to wrap up this tour of pathname walking is to list
the various flags that can be stored in the ``nameidata`` to guide the
lookup process.  Many of these are only meaningful on the final
component, others reflect the current state of the pathname lookup, and some
apply restrictions to all path components encountered in the path lookup.
(为这次路径遍历之旅收尾的合适方式，是列出可以存储在 nameidata 中、用以引导查找过程的各种标志。其中许多只对最后组件有意义，另一些反映路径名查找的当前状态，还有一些对路径查找中遇到的所有路径组件施加限制。)

And then there is ``LOOKUP_EMPTY``, which doesn't fit conceptually with
the others.  If this is not set, an empty pathname causes an error
very early on.  If it is set, empty pathnames are not considered to be
an error.
(另外还有 LOOKUP_EMPTY，它在概念上与其他标志不合。如果未设置它，空路径名很早就会导致错误。如果设置了它，空路径名不被视为错误。)

Global state flags
(全局状态标志)
~~~~~~~~~~~~~~~~~~

We have already met two global state flags: ``LOOKUP_RCU`` and
``LOOKUP_REVAL``.  These select between one of three overall approaches
to lookup: RCU-walk, REF-walk, and REF-walk with forced revalidation.
(我们已经见过两个全局状态标志：LOOKUP_RCU 和 LOOKUP_REVAL。它们在三种总体查找方法之间做出选择：RCU-walk、REF-walk，以及带强制重新验证的 REF-walk。)

``LOOKUP_PARENT`` indicates that the final component hasn't been reached
yet.  This is primarily used to tell the audit subsystem the full
context of a particular access being audited.
(LOOKUP_PARENT 表示尚未到达最后组件。它主要用于告诉审计子系统正在审计的特定访问的完整上下文。)

``ND_ROOT_PRESET`` indicates that the ``root`` field in the ``nameidata`` was
provided by the caller, so it shouldn't be released when it is no
longer needed.
(ND_ROOT_PRESET 表示 nameidata 中的 root 字段由调用者提供，因此当不再需要它时不应释放。)

``ND_JUMPED`` means that the current dentry was chosen not because
it had the right name but for some other reason.  This happens when
following "``..``", following a symlink to ``/``, crossing a mount point
or accessing a "``/proc/$PID/fd/$FD``" symlink (also known as a "magic
link"). In this case the filesystem has not been asked to revalidate the
name (with ``d_revalidate()``).  In such cases the inode may still need
to be revalidated, so ``d_op->d_weak_revalidate()`` is called if
``ND_JUMPED`` is set when the look completes - which may be at the
final component or, when creating, unlinking, or renaming, at the penultimate component.
(ND_JUMPED 表示选择当前 dentry 不是因为它具有正确的名称，而是由于其他原因。这发生在跟随".."时、跟随指向"/"的符号链接时、跨越挂载点时，或访问"/proc/$PID/fd/$FD"符号链接(也称为"magic link"，魔链)时。在这种情况下，文件系统没有被要求重新验证名称(通过 d_revalidate())。此时 inode 可能仍需要重新验证，所以当查找完成而设置了 ND_JUMPED 时会调用 d_op->d_weak_revalidate()——查找可能在最后组件完成，或者在创建、解除链接或重命名时在倒数第二个组件完成。)

Resolution-restriction flags
(解析限制标志)
~~~~~~~~~~~~~~~~~~~~~~~~~~~

In order to allow userspace to protect itself against certain race conditions
and attack scenarios involving changing path components, a series of flags are
available which apply restrictions to all path components encountered during
path lookup. These flags are exposed through ``openat2()``'s ``resolve`` field.
(为了允许用户空间保护自己免受某些涉及更改路径组件的竞争条件和攻击场景的影响，提供了一系列标志，它们对路径查找期间遇到的所有路径组件施加限制。这些标志通过 openat2() 的 resolve 字段暴露。)

``LOOKUP_NO_SYMLINKS`` blocks all symlink traversals (including magic-links).
This is distinctly different from ``LOOKUP_FOLLOW``, because the latter only
relates to restricting the following of trailing symlinks.
(LOOKUP_NO_SYMLINKS 阻止所有符号链接遍历(包括 magic-links)。这与 LOOKUP_FOLLOW 明显不同，因为后者只与限制跟随尾部符号链接有关。)

``LOOKUP_NO_MAGICLINKS`` blocks all magic-link traversals. Filesystems must
ensure that they return errors from ``nd_jump_link()``, because that is how
``LOOKUP_NO_MAGICLINKS`` and other magic-link restrictions are implemented.
(LOOKUP_NO_MAGICLINKS 阻止所有 magic-link 遍历。文件系统必须确保从 nd_jump_link() 返回错误，因为 LOOKUP_NO_MAGICLINKS 和其他 magic-link 限制正是这样实现的。)

``LOOKUP_NO_XDEV`` blocks all ``vfsmount`` traversals (this includes both
bind-mounts and ordinary mounts). Note that the ``vfsmount`` which contains the
lookup is determined by the first mountpoint the path lookup reaches --
absolute paths start with the ``vfsmount`` of ``/``, and relative paths start
with the ``dfd``'s ``vfsmount``. Magic-links are only permitted if the
``vfsmount`` of the path is unchanged.
(LOOKUP_NO_XDEV 阻止所有 vfsmount 遍历(这既包括绑定挂载也包括普通挂载)。注意，包含该查找的 vfsmount 由路径查找到达的第一个挂载点决定——绝对路径以 / 的 vfsmount 开始，相对路径以 dfd 的 vfsmount 开始。只有当路径的 vfsmount 未改变时才允许 magic-links。)

``LOOKUP_BENEATH`` blocks any path components which resolve outside the
starting point of the resolution. This is done by blocking ``nd_jump_root()``
as well as blocking ".." if it would jump outside the starting point.
``rename_lock`` and ``mount_lock`` are used to detect attacks against the
resolution of "..". Magic-links are also blocked.
(LOOKUP_BENEATH 阻止任何解析到解析起点之外的路径组件。这通过阻止 nd_jump_root()、以及在".."会跳出起点之外时阻止".."来实现。rename_lock 和 mount_lock 用于检测针对".."解析的攻击。magic-links 也被阻止。)

``LOOKUP_IN_ROOT`` resolves all path components as though the starting point
were the filesystem root. ``nd_jump_root()`` brings the resolution back to
the starting point, and ".." at the starting point will act as a no-op. As with
``LOOKUP_BENEATH``, ``rename_lock`` and ``mount_lock`` are used to detect
attacks against ".." resolution. Magic-links are also blocked.
(LOOKUP_IN_ROOT 解析所有路径组件时如同起点就是文件系统根。nd_jump_root() 把解析带回起点，而起点处的".."将表现为无操作。与 LOOKUP_BENEATH 一样，rename_lock 和 mount_lock 用于检测针对".."解析的攻击。magic-links 也被阻止。)

Final-component flags
(最后组件标志)
~~~~~~~~~~~~~~~~~~~~~

Some of these flags are only set when the final component is being
considered.  Others are only checked for when considering that final
component.
(这些标志中的一些只在考虑最后组件时才被设置。另一些只在考虑那个最后组件时才被检查。)

``LOOKUP_AUTOMOUNT`` ensures that, if the final component is an automount
point, then the mount is triggered.  Some operations would trigger it
anyway, but operations like ``stat()`` deliberately don't.  ``statfs()``
needs to trigger the mount but otherwise behaves a lot like ``stat()``, so
it sets ``LOOKUP_AUTOMOUNT``, as does "``quotactl()``" and the handling of
"``mount --bind``".
(LOOKUP_AUTOMOUNT 确保：如果最后组件是一个自动挂载点，则触发挂载。某些操作反正会触发它，但像 stat() 这样的操作刻意不触发。statfs() 需要触发挂载，但其他方面表现得很像 stat()，所以它设置 LOOKUP_AUTOMOUNT，"quotactl()"和"mount --bind"的处理也是如此。)

``LOOKUP_FOLLOW`` has a similar function to ``LOOKUP_AUTOMOUNT`` but for
symlinks.  Some system calls set or clear it implicitly, while
others have API flags such as ``AT_SYMLINK_FOLLOW`` and
``UMOUNT_NOFOLLOW`` to control it.  Its effect is similar to
``WALK_TRAILING`` that we already met, but it is used in a different way.
(LOOKUP_FOLLOW 的功能与 LOOKUP_AUTOMOUNT 类似，但针对符号链接。一些系统调用隐式地设置或清除它，而另一些系统调用则有诸如 AT_SYMLINK_FOLLOW 和 UMOUNT_NOFOLLOW 之类的 API 标志来控制它。它的效果与我们已见过的 WALK_TRAILING 类似，但以不同的方式使用。)

``LOOKUP_DIRECTORY`` insists that the final component is a directory.
Various callers set this and it is also set when the final component
is found to be followed by a slash.
(LOOKUP_DIRECTORY 坚持要求最后组件是一个目录。各种调用者会设置它，当发现最后组件后面跟着一个斜杠时也会设置它。)

Finally ``LOOKUP_OPEN``, ``LOOKUP_CREATE``, ``LOOKUP_EXCL``, and
``LOOKUP_RENAME_TARGET`` are not used directly by the VFS but are made
available to the filesystem and particularly the ``->d_revalidate()``
method.  A filesystem can choose not to bother revalidating too hard
if it knows that it will be asked to open or create the file soon.
These flags were previously useful for ``->lookup()`` too but with the
introduction of ``->atomic_open()`` they are less relevant there.
(最后，LOOKUP_OPEN、LOOKUP_CREATE、LOOKUP_EXCL 和 LOOKUP_RENAME_TARGET 不被 VFS 直接使用，而是提供给文件系统，特别是 ->d_revalidate() 方法。如果文件系统知道自己很快就会被要求打开或创建该文件，它可以选择不费心做过重的重新验证。这些标志以前对 ->lookup() 也曾有用，但随着 ->atomic_open() 的引入，它们在那里已不太相关。)

End of the road
(路的尽头)
---------------

Despite its complexity, all this pathname lookup code appears to be
in good shape - various parts are certainly easier to understand now
than even a couple of releases ago.  But that doesn't mean it is
"finished".   As already mentioned, RCU-walk currently only follows
symlinks that are stored in the inode so, while it handles many ext4
symlinks, it doesn't help with NFS, XFS, or Btrfs.  That support
is not likely to be long delayed.
(尽管复杂，所有这些路径名查找代码看起来状态良好——某些部分现在肯定比仅仅几个版本之前更容易理解了。但这并不意味着它已经"完成"。如前所述，RCU-walk 目前只跟随存储在 inode 中的符号链接，所以虽然它能处理许多 ext4 符号链接，但对 NFS、XFS 或 Btrfs 没有帮助。相关支持不太可能拖延太久。)
