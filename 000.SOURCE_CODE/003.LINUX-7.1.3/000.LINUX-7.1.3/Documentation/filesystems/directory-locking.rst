=================
Directory Locking
=================

(目录加锁)


Locking scheme used for directory operations is based on two
kinds of locks - per-inode (->i_rwsem) and per-filesystem
(->s_vfs_rename_mutex).
(目录操作所使用的加锁方案基于两类锁——每 inode 的(->i_rwsem)和每文件系统的(->s_vfs_rename_mutex)。)

When taking the i_rwsem on multiple non-directory objects, we
always acquire the locks in order by increasing address.  We'll call
that "inode pointer" order in the following.
(当对多个非目录对象获取 i_rwsem 时，我们总是按地址递增的顺序获取这些锁。下文中我们将称之为"inode 指针"顺序。)


Primitives
(原语)
==========

For our purposes all operations fall in 6 classes:
(就我们的目的而言，所有操作分为 6 类：)

1. read access.  Locking rules:
   (读访问。加锁规则：)

	* lock the directory we are accessing (shared)
	  (锁定我们正在访问的目录(共享))

2. object creation.  Locking rules:
   (对象创建。加锁规则：)

	* lock the directory we are accessing (exclusive)
	  (锁定我们正在访问的目录(独占))

3. object removal.  Locking rules:
   (对象删除。加锁规则：)

	* lock the parent (exclusive)
	  (锁定父目录(独占))
	* find the victim
	  (找到待删除的对象)
	* lock the victim (exclusive)
	  (锁定该待删除对象(独占))

4. link creation.  Locking rules:
   (链接创建。加锁规则：)

	* lock the parent (exclusive)
	  (锁定父目录(独占))
	* check that the source is not a directory
	  (检查源不是目录)
	* lock the source (exclusive; probably could be weakened to shared)
	  (锁定源(独占；大概可以弱化为共享))

5. rename that is _not_ cross-directory.  Locking rules:
   (非跨目录的重命名。加锁规则：)

	* lock the parent (exclusive)
	  (锁定父目录(独占))
	* find the source and target
	  (找到源和目标)
	* decide which of the source and target need to be locked.
	  The source needs to be locked if it's a non-directory, target - if it's
	  a non-directory or about to be removed.
	  (确定源和目标之中哪一个需要加锁。如果源是非目录，则源需要加锁；目标则在其是非目录或即将被删除时需要加锁。)
	* take the locks that need to be taken (exclusive), in inode pointer order
	  if need to take both (that can happen only when both source and target
	  are non-directories - the source because it wouldn't need to be locked
	  otherwise and the target because mixing directory and non-directory is
	  allowed only with RENAME_EXCHANGE, and that won't be removing the target).
	  (获取那些需要获取的锁(独占)，如果两个锁都需要获取，则按 inode 指针顺序获取(这种情况只可能发生在源和目标都是非目录时——源需要加锁，否则它本不需要加锁；目标需要加锁，是因为目录与非目录混用只有在使用 RENAME_EXCHANGE 时才被允许，而那不会删除目标)。)

6. cross-directory rename.  The trickiest in the whole bunch.  Locking rules:
   (跨目录重命名。这一类中最为棘手。加锁规则：)

	* lock the filesystem
	  (锁定文件系统)
	* if the parents don't have a common ancestor, fail the operation.
	  (如果两个父目录没有共同的祖先，则使操作失败。)
	* lock the parents in "ancestors first" order (exclusive). If neither is an
	  ancestor of the other, lock the parent of source first.
	  (按"祖先优先"的顺序锁定父目录(独占)。如果两者互不为对方的祖先，则先锁定源的父目录。)
	* find the source and target.
	  (找到源和目标。)
	* verify that the source is not a descendent of the target and
	  target is not a descendent of source; fail the operation otherwise.
	  (验证源不是目标的后代，且目标不是源的后代；否则使操作失败。)
	* lock the subdirectories involved (exclusive), source before target.
	  (锁定所涉及的各子目录(独占)，先源后目标。)
	* lock the non-directories involved (exclusive), in inode pointer order.
	  (按 inode 指针顺序锁定所涉及的各非目录(独占)。)

The rules above obviously guarantee that all directories that are going
to be read, modified or removed by method will be locked by the caller.
(上述规则显然保证了所有将被该方法读取、修改或删除的目录都会被调用者锁定。)


Splicing
(拼接)
========

There is one more thing to consider - splicing.  It's not an operation
in its own right; it may happen as part of lookup.  We speak of the
operations on directory trees, but we obviously do not have the full
picture of those - especially for network filesystems.  What we have
is a bunch of subtrees visible in dcache and locking happens on those.
Trees grow as we do operations; memory pressure prunes them.  Normally
that's not a problem, but there is a nasty twist - what should we do
when one growing tree reaches the root of another?  That can happen in
several scenarios, starting from "somebody mounted two nested subtrees
from the same NFS4 server and doing lookups in one of them has reached
the root of another"; there's also open-by-fhandle stuff, and there's a
possibility that directory we see in one place gets moved by the server
to another and we run into it when we do a lookup.
(还有一件事需要考虑——拼接。它本身并不是一个独立的操作；它可能作为查找的一部分发生。我们谈论的是对目录树的操作，但我们显然并不掌握其全貌——对网络文件系统尤其如此。我们实际拥有的只是 dcache 中可见的一堆子树，加锁就发生在这些子树上。树随着我们执行操作而生长；内存压力则修剪它们。通常这不成问题，但有一个棘手的曲折之处——当一棵正在生长的树到达了另一棵树的根时，我们该怎么办？这可能出现在多种场景中，从"有人从同一台 NFS4 服务器挂载了两个嵌套的子树，而在其中一个子树中进行的查找已经到达了另一个子树的根"开始；还有 open-by-fhandle 之类的东西，以及我们在某处看到的目录被服务器移到了另一处、而我们在查找时撞上它的可能性。)

For a lot of reasons we want to have the same directory present in dcache
only once.  Multiple aliases are not allowed.  So when lookup runs into
a subdirectory that already has an alias, something needs to be done with
dcache trees.  Lookup is already holding the parent locked.  If alias is
a root of separate tree, it gets attached to the directory we are doing a
lookup in, under the name we'd been looking for.  If the alias is already
a child of the directory we are looking in, it changes name to the one
we'd been looking for.  No extra locking is involved in these two cases.
However, if it's a child of some other directory, the things get trickier.
First of all, we verify that it is *not* an ancestor of our directory
and fail the lookup if it is.  Then we try to lock the filesystem and the
current parent of the alias.  If either trylock fails, we fail the lookup.
If trylocks succeed, we detach the alias from its current parent and
attach to our directory, under the name we are looking for.
(出于多种原因，我们希望同一个目录在 dcache 中只出现一次。不允许存在多个别名。因此，当查找撞上一个已经有别名的子目录时，就需要对 dcache 树做一些处理。查找此时已经持有父目录的锁。如果该别名是一棵独立树的根，它会被挂接到我们正在其中进行查找的目录之下，采用我们正在查找的那个名字。如果该别名已经是我们要查找的那个目录的孩子，它会把名字改成我们正在查找的那个名字。这两种情况不涉及任何额外的加锁。但是，如果它是某个其他目录的孩子，情况就变得更棘手了。首先，我们验证它*不是*我们目录的祖先，如果是则使查找失败。然后我们尝试锁定文件系统以及该别名当前的父目录。任何一个 trylock 失败，我们就使查找失败。如果 trylock 都成功，我们就把该别名从其当前父目录上摘下，挂接到我们的目录之下，采用我们正在查找的名字。)

Note that splicing does *not* involve any modification of the filesystem;
all we change is the view in dcache.  Moreover, holding a directory locked
exclusive prevents such changes involving its children and holding the
filesystem lock prevents any changes of tree topology, other than having a
root of one tree becoming a child of directory in another.  In particular,
if two dentries have been found to have a common ancestor after taking
the filesystem lock, their relationship will remain unchanged until
the lock is dropped.  So from the directory operations' point of view
splicing is almost irrelevant - the only place where it matters is one
step in cross-directory renames; we need to be careful when checking if
parents have a common ancestor.
(注意，拼接*不*涉及对文件系统的任何修改；我们改变的只是 dcache 中的视图。而且，以独占方式持有一个目录的锁可以防止涉及它的孩子的这类改动，持有文件系统锁则可以防止树拓扑的任何改动——除了一棵树的根变成另一棵树中目录的孩子这种情况之外。特别地，如果在获取文件系统锁之后发现两个 dentry 有共同祖先，那么在锁被释放之前它们的关系将保持不变。所以从目录操作的角度来看，拼接几乎是无关的——它唯一有影响的地方是跨目录重命名中的一个步骤；在检查父目录是否有共同祖先时我们需要小心。)


Multiple-filesystem stuff
(多文件系统相关的问题)
=========================

For some filesystems a method can involve a directory operation on
another filesystem; it may be ecryptfs doing operation in the underlying
filesystem, overlayfs doing something to the layers, network filesystem
using a local one as a cache, etc.  In all such cases the operations
on other filesystems must follow the same locking rules.  Moreover, "a
directory operation on this filesystem might involve directory operations
on that filesystem" should be an asymmetric relation (or, if you will,
it should be possible to rank the filesystems so that directory operation
on a filesystem could trigger directory operations only on higher-ranked
ones - in these terms overlayfs ranks lower than its layers, network
filesystem ranks lower than whatever it caches on, etc.)
(对某些文件系统来说，一个方法可能涉及在另一个文件系统上的目录操作；例如 ecryptfs 在底层文件系统中执行操作，overlayfs 对它的各层做些什么，网络文件系统把本地文件系统用作缓存，等等。在所有这类情况下，对其他文件系统的操作必须遵循相同的加锁规则。此外，"在这个文件系统上的目录操作可能会涉及那个文件系统上的目录操作"应当是一个非对称关系(或者说，应当能够对这些文件系统排出等级，使得在某个文件系统上的目录操作只能触发对更高级别文件系统的目录操作——按这种说法，overlayfs 的级别低于它的各层，网络文件系统的级别低于它以其为缓存的任何文件系统，等等)。)


Deadlock avoidance
(避免死锁)
==================

If no directory is its own ancestor, the scheme above is deadlock-free.
(如果没有目录是它自身的祖先，那么上述方案就是无死锁的。)

Proof:
(证明：)

There is a ranking on the locks, such that all primitives take
them in order of non-decreasing rank.  Namely,
(这些锁之间存在一个排序，使得所有原语都按秩非递减的顺序获取它们。具体来说，)

  * rank ->i_rwsem of non-directories on given filesystem in inode pointer
    order.
    (把给定文件系统上非目录的 ->i_rwsem 按 inode 指针顺序排出秩。)
  * put ->i_rwsem of all directories on a filesystem at the same rank,
    lower than ->i_rwsem of any non-directory on the same filesystem.
    (把一个文件系统上所有目录的 ->i_rwsem 放在同一个秩上，低于同一文件系统上任何非目录的 ->i_rwsem。)
  * put ->s_vfs_rename_mutex at rank lower than that of any ->i_rwsem
    on the same filesystem.
    (把 ->s_vfs_rename_mutex 放在低于同一文件系统上任何 ->i_rwsem 的秩上。)
  * among the locks on different filesystems use the relative
    rank of those filesystems.
    (对于不同文件系统上的锁，使用这些文件系统之间的相对秩。)

For example, if we have NFS filesystem caching on a local one, we have
(例如，如果我们有一个以本地文件系统为缓存的 NFS 文件系统，那么我们有)

  1. ->s_vfs_rename_mutex of NFS filesystem
     (NFS 文件系统的 ->s_vfs_rename_mutex)
  2. ->i_rwsem of directories on that NFS filesystem, same rank for all
     (该 NFS 文件系统上各目录的 ->i_rwsem，所有目录同一秩)
  3. ->i_rwsem of non-directories on that filesystem, in order of
     increasing address of inode
     (该文件系统上各非目录的 ->i_rwsem，按 inode 地址递增排序)
  4. ->s_vfs_rename_mutex of local filesystem
     (本地文件系统的 ->s_vfs_rename_mutex)
  5. ->i_rwsem of directories on the local filesystem, same rank for all
     (本地文件系统上各目录的 ->i_rwsem，所有目录同一秩)
  6. ->i_rwsem of non-directories on local filesystem, in order of
     increasing address of inode.
     (本地文件系统上各非目录的 ->i_rwsem，按 inode 地址递增排序。)

It's easy to verify that operations never take a lock with rank
lower than that of an already held lock.
(很容易验证，操作绝不会获取秩低于某个已持有锁的锁。)

Suppose deadlocks are possible.  Consider the minimal deadlocked
set of threads.  It is a cycle of several threads, each blocked on a lock
held by the next thread in the cycle.
(假设死锁是可能的。考虑最小的死锁线程集合。它是由若干线程构成的一个环，每个线程都阻塞在该环中下一个线程所持有的某个锁上。)

Since the locking order is consistent with the ranking, all
contended locks in the minimal deadlock will be of the same rank,
i.e. they all will be ->i_rwsem of directories on the same filesystem.
Moreover, without loss of generality we can assume that all operations
are done directly to that filesystem and none of them has actually
reached the method call.
(由于加锁顺序与该排序一致，最小死锁中所有被争用的锁都将具有相同的秩，也就是说它们都是同一文件系统上目录的 ->i_rwsem。此外，不失一般性，我们可以假设所有操作都是直接针对该文件系统进行的，并且其中没有任何一个操作真正走到了方法调用。)

In other words, we have a cycle of threads, T1,..., Tn,
and the same number of directories (D1,...,Dn) such that
(换句话说，我们有一个由线程 T1,..., Tn 构成的环，以及同样数目的目录(D1,...,Dn)，使得)

	T1 is blocked on D1 which is held by T2
	(T1 阻塞在 D1 上，而 D1 由 T2 持有)

	T2 is blocked on D2 which is held by T3
	(T2 阻塞在 D2 上，而 D2 由 T3 持有)

	...

	Tn is blocked on Dn which is held by T1.
	(Tn 阻塞在 Dn 上，而 Dn 由 T1 持有。)

Each operation in the minimal cycle must have locked at least
one directory and blocked on attempt to lock another.  That leaves
only 3 possible operations: directory removal (locks parent, then
child), same-directory rename killing a subdirectory (ditto) and
cross-directory rename of some sort.
(最小环中的每个操作都必须至少锁定过一个目录，并且在试图锁定另一个目录时被阻塞过。这就只剩下 3 种可能的操作：目录删除(先锁父目录，再锁孩子)、同一目录内重命名并杀死一个子目录(做法与前者相同)以及某种跨目录重命名。)

There must be a cross-directory rename in the set; indeed,
if all operations had been of the "lock parent, then child" sort
we would have Dn a parent of D1, which is a parent of D2, which is
a parent of D3, ..., which is a parent of Dn.  Relationships couldn't
have changed since the moment directory locks had been acquired,
so they would all hold simultaneously at the deadlock time and
we would have a loop.
(这个集合中必定存在一个跨目录重命名；确实，如果所有操作都属于"先锁父目录，再锁孩子"这一类，那么我们就会得到 Dn 是 D1 的父目录，而 D1 是 D2 的父目录，D2 是 D3 的父目录，……，直到最后一个是 Dn 的父目录。自目录锁被获取的那一刻起，这些关系不可能发生改变，因此在死锁时刻它们会全部同时成立，我们就得到了一个环。)

Since all operations are on the same filesystem, there can't be
more than one cross-directory rename among them.  Without loss of
generality we can assume that T1 is the one doing a cross-directory
rename and everything else is of the "lock parent, then child" sort.
(由于所有操作都在同一个文件系统上，它们之中不可能有多于一个的跨目录重命名。不失一般性，我们可以假设 T1 就是执行跨目录重命名的那个，而其余所有操作都属于"先锁父目录，再锁孩子"这一类。)

In other words, we have a cross-directory rename that locked
Dn and blocked on attempt to lock D1, which is a parent of D2, which is
a parent of D3, ..., which is a parent of Dn.  Relationships between
D1,...,Dn all hold simultaneously at the deadlock time.  Moreover,
cross-directory rename does not get to locking any directories until it
has acquired filesystem lock and verified that directories involved have
a common ancestor, which guarantees that ancestry relationships between
all of them had been stable.
(换句话说，我们有一个跨目录重命名，它锁定了 Dn，然后在试图锁定 D1 时被阻塞，而 D1 是 D2 的父目录，D2 是 D3 的父目录，……，直到最后一个是 Dn 的父目录。D1,...,Dn 之间的关系在死锁时刻全部同时成立。此外，跨目录重命名在获取文件系统锁并验证所涉及的目录拥有共同祖先之前，不会去锁定任何目录，这保证了它们全体之间的祖辈关系此前一直是稳定的。)

Consider the order in which directories are locked by the
cross-directory rename; parents first, then possibly their children.
Dn and D1 would have to be among those, with Dn locked before D1.
Which pair could it be?
(考虑跨目录重命名锁定各目录的顺序；先父目录，然后可能是它们的孩子。Dn 和 D1 必定都在其中，且 Dn 先于 D1 被锁定。那么会是哪一对呢？)

It can't be the parents - indeed, since D1 is an ancestor of Dn,
it would be the first parent to be locked.  Therefore at least one of the
children must be involved and thus neither of them could be a descendent
of another - otherwise the operation would not have progressed past
locking the parents.
(不可能是那两个父目录——确实，由于 D1 是 Dn 的祖先，它会是第一个被锁定的父目录。因此所涉及的目录中至少有一个是孩子，于是它们之中任何一个都不可能是另一个的后代——否则该操作就不可能在锁定父目录之后再往前推进。)

It can't be a parent and its child; otherwise we would've had
a loop, since the parents are locked before the children, so the parent
would have to be a descendent of its child.
(不可能是某个父目录和它自己的孩子；否则我们就会得到一个环，因为父目录先于孩子被锁定，所以那个父目录将不得不成为它自己孩子的后代。)

It can't be a parent and a child of another parent either.
Otherwise the child of the parent in question would've been a descendent
of another child.
(也不可能是某个父目录和另一个父目录的孩子。否则，前面所说的那个父目录的孩子就会是另一个孩子的后代。)

That leaves only one possibility - namely, both Dn and D1 are
among the children, in some order.  But that is also impossible, since
neither of the children is a descendent of another.
(这样就只剩下一种可能——即 Dn 和 D1 都在孩子之中，以某种先后顺序。但这也是不可能的，因为孩子们之中没有任何一个是另一个的后代。)

That concludes the proof, since the set of operations with the
properties required for a minimal deadlock can not exist.
(证明到此结束，因为具备最小死锁所需性质的操作集合不可能存在。)

Note that the check for having a common ancestor in cross-directory
rename is crucial - without it a deadlock would be possible.  Indeed,
suppose the parents are initially in different trees; we would lock the
parent of source, then try to lock the parent of target, only to have
an unrelated lookup splice a distant ancestor of source to some distant
descendent of the parent of target.   At that point we have cross-directory
rename holding the lock on parent of source and trying to lock its
distant ancestor.  Add a bunch of rmdir() attempts on all directories
in between (all of those would fail with -ENOTEMPTY, had they ever gotten
the locks) and voila - we have a deadlock.
(注意，跨目录重命名中对拥有共同祖先的检查是至关重要的——没有它，死锁就会成为可能。确实，假设两个父目录最初位于不同的树中；我们会锁定源的父目录，然后尝试锁定目标的父目录，结果却让一个无关的查找把源的某个远祖拼接成目标父目录的某个远端后代。在那一刻，我们的跨目录重命名持有源父目录的锁，并试图锁定它的远祖。再加上对介于其间的所有目录发起的一堆 rmdir() 尝试(即便它们曾拿到过锁，这些尝试也都会以 -ENOTEMPTY 失败)，瞧——我们就得到了一个死锁。)

Loop avoidance
(避免成环)
==============

These operations are guaranteed to avoid loop creation.  Indeed,
the only operation that could introduce loops is cross-directory rename.
Suppose after the operation there is a loop; since there hadn't been such
loops before the operation, at least on of the nodes in that loop must've
had its parent changed.  In other words, the loop must be passing through
the source or, in case of exchange, possibly the target.
(这些操作保证不会制造环。确实，唯一可能引入环的操作就是跨目录重命名。假设操作之后出现了一个环；由于操作之前并不存在这样的环，该环中至少有一个节点的父节点必定被改变过。换句话说，这个环必定经过源，或者在交换的情况下可能经过目标。)

Since the operation has succeeded, neither source nor target could have
been ancestors of each other.  Therefore the chain of ancestors starting
in the parent of source could not have passed through the target and
vice versa.  On the other hand, the chain of ancestors of any node could
not have passed through the node itself, or we would've had a loop before
the operation.  But everything other than source and target has kept
the parent after the operation, so the operation does not change the
chains of ancestors of (ex-)parents of source and target.  In particular,
those chains must end after a finite number of steps.
(既然操作已经成功，源和目标就不可能互为祖先。因此，从源的父目录出发的祖先链不可能经过目标，反之亦然。另一方面，任何节点的祖先链都不可能经过该节点自身，否则在操作之前我们就会有一个环了。但除源和目标之外的所有东西在操作之后都保持了原来的父节点，所以该操作不会改变源和目标的(前)父目录的祖先链。特别地，这些祖先链必将在有限步之后终止。)

Now consider the loop created by the operation.  It passes through either
source or target; the next node in the loop would be the ex-parent of
target or source resp.  After that the loop would follow the chain of
ancestors of that parent.  But as we have just shown, that chain must
end after a finite number of steps, which means that it can't be a part
of any loop.  Q.E.D.
(现在考虑由该操作制造出来的那个环。它经过源或目标之一；环中的下一个节点将分别是目标或源的前父目录。再往后，这个环将沿着那个父目录的祖先链走。但正如我们刚刚证明的，那条链必将在有限步之后终止，这意味着它不可能成为任何环的一部分。证毕。)

While this locking scheme works for arbitrary DAGs, it relies on
ability to check that directory is a descendent of another object.  Current
implementation assumes that directory graph is a tree.  This assumption is
also preserved by all operations (cross-directory rename on a tree that would
not introduce a cycle will leave it a tree and link() fails for directories).
(虽然这一加锁方案适用于任意的 DAG，但它依赖于检查某个目录是否为另一个对象之后代的能力。当前的实现假设目录图是一棵树。所有操作也都会保持这一假设(在树上的、不会引入环的跨目录重命名将使它仍然是一棵树，而 link() 对目录会失败)。)

Notice that "directory" in the above == "anything that might have
children", so if we are going to introduce hybrid objects we will need
either to make sure that link(2) doesn't work for them or to make changes
in is_subdir() that would make it work even in presence of such beasts.
(注意，上文中的"目录"等同于"任何可能有孩子的东西"，因此如果我们打算引入混合型对象，我们将需要要么确保 link(2) 对它们不起作用，要么对 is_subdir() 做出修改，使之即使在这类东西存在的情况下也能正常工作。)
