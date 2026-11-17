.. SPDX-License-Identifier: GPL-2.0

==============================================
LC-trie implementation notes(LC-trie 实现说明)
==============================================

Node types(节点类型)
--------------------

leaf
(叶子节点)

	An end node with data. This has a copy of the relevant key, along
	with 'hlist' with routing table entries sorted by prefix length.
	See struct leaf and struct leaf_info.
	(一种带有数据的末端节点。它保存相关键的一份副本，以及一个按前缀长度排序的、
	带有路由表表项的 'hlist'。参见 struct leaf 和 struct leaf_info。)

trie node or tnode
(trie 节点或 tnode)

	An internal node, holding an array of child (leaf or tnode) pointers,
	indexed	through a subset of the key. See Level Compression.
	(一种内部节点，持有一个子节点(叶子或 tnode)指针数组，通过键的一个子集进行
	索引。参见层级压缩(Level Compression)。)

A few concepts explained(一些概念的解释)
----------------------------------------

Bits (tnode)
(位数 (tnode))

	The number of bits in the key segment used for indexing into the
	child array - the "child index". See Level Compression.
	(用于在子节点数组中进行索引的键段的位数——即"子节点索引"。参见层级压缩
	(Level Compression)。)

Pos (tnode)
(位置 (tnode))

	The position (in the key) of the key segment used for indexing into
	the child array. See Path Compression.
	(用于在子节点数组中进行索引的键段在键中的位置。参见路径压缩
	(Path Compression)。)

Path Compression / skipped bits
(路径压缩 / 跳过的位)

	Any given tnode is linked to from the child array of its parent, using
	a segment of the key specified by the parent's "pos" and "bits"
	In certain cases, this tnode's own "pos" will not be immediately
	adjacent to the parent (pos+bits), but there will be some bits
	in the key skipped over because they represent a single path with no
	deviations. These "skipped bits" constitute Path Compression.
	Note that the search algorithm will simply skip over these bits when
	searching, making it necessary to save the keys in the leaves to
	verify that they actually do match the key we are searching for.
	(任何给定的 tnode 都由其父节点通过父节点 "pos" 和 "bits" 所指定的键段，从父
	节点的子节点数组链接过来。在某些情况下，这个 tnode 自身的 "pos" 不会紧邻父
	节点(pos+bits)，键中会有一些位被跳过，因为它们表示一条没有分叉的单一路径。
	这些"跳过的位"即构成路径压缩。注意，搜索算法在搜索时会直接跳过这些位，因此
	有必要把键保存在叶子中，以验证它们确实与我们要搜索的键匹配。)

Level Compression / child arrays
(层级压缩 / 子节点数组)

	the trie is kept level balanced moving, under certain conditions, the
	children of a full child (see "full_children") up one level, so that
	instead of a pure binary tree, each internal node ("tnode") may
	contain an arbitrarily large array of links to several children.
	Conversely, a tnode with a mostly empty	child array (see empty_children)
	may be "halved", having some of its children moved downwards one level,
	in order to avoid ever-increasing child arrays.
	(字典树保持层级平衡：在一定条件下，把满子节点(见 "full_children")的子节点上移
	一层，使得每个内部节点("tnode")都可以包含一个指向若干子节点的任意大的链接
	数组，而不是纯粹的二叉树。反过来，子节点数组大部分为空的 tnode(见
	empty_children)可以被"减半"，把它的一些子节点下移一层，以避免子节点数组不断
	增大。)

empty_children
(empty_children)

	the number of positions in the child array of a given tnode that are
	NULL.
	(给定 tnode 的子节点数组中为 NULL 的位置的数目。)

full_children
(full_children)

	the number of children of a given tnode that aren't path compressed.
	(in other words, they aren't NULL or leaves and their "pos" is equal
	to this	tnode's "pos"+"bits").
	(给定 tnode 中未经过路径压缩的子节点的数量(换句话说，它们既不是 NULL 也不
	是叶子，并且其 "pos" 等于本 tnode 的 "pos"+"bits")。)

	(The word "full" here is used more in the sense of "complete" than
	as the opposite of "empty", which might be a tad confusing.)
	(这里的 "full" 一词更多是取"完整"之意，而不是作为 "empty" 的反义词，这可能
	有点令人困惑。)

Comments(说明)
--------------

We have tried to keep the structure of the code as close to fib_hash as
possible to allow verification and help up reviewing.
(我们尽量让代码结构保持与 fib_hash 尽可能接近，以便核对并帮助我们进行评审。)

fib_find_node()
(fib_find_node())

	A good start for understanding this code. This function implements a
	straightforward trie lookup.
	(理解这段代码的一个很好的起点。该函数实现了一次直接的字典树查找。)

fib_insert_node()
(fib_insert_node())

	Inserts a new leaf node in the trie. This is bit more complicated than
	fib_find_node(). Inserting a new node means we might have to run the
	level compression algorithm on part of the trie.
	(在字典树中插入一个新的叶子节点。这比 fib_find_node() 稍微复杂一些。插入新
	节点意味着我们可能必须对字典树的一部分运行层级压缩算法。)

trie_leaf_remove()
(trie_leaf_remove())

	Looks up a key, deletes it and runs the level compression algorithm.
	(查找一个键，将其删除，并运行层级压缩算法。)

trie_rebalance()
(trie_rebalance())

	The key function for the dynamic trie after any change in the trie
	it is run to optimize and reorganize. It will walk the trie upwards
	towards the root from a given tnode, doing a resize() at each step
	to implement level compression.
	(字典树发生任何变化之后用于动态字典树的关键函数，运行它以进行优化和重组。
	它会从给定的 tnode 向上朝根节点遍历字典树，在每一步执行一次 resize() 以实现
	层级压缩。)

resize()
(resize())

	Analyzes a tnode and optimizes the child array size by either inflating
	or shrinking it repeatedly until it fulfills the criteria for optimal
	level compression. This part follows the original paper pretty closely
	and there may be some room for experimentation here.
	(分析一个 tnode，通过反复增大或缩小子节点数组来优化其大小，直到满足最优层级
	压缩的条件为止。这一部分相当紧贴原始论文，这里可能还有一些试验的空间。)

inflate()
(inflate())

	Doubles the size of the child array within a tnode. Used by resize().
	(将一个 tnode 内子节点数组的大小加倍。由 resize() 使用。)

halve()
(halve())

	Halves the size of the child array within a tnode - the inverse of
	inflate(). Used by resize();
	(将一个 tnode 内子节点数组的大小减半——inflate() 的逆操作。由 resize() 使用。)

fn_trie_insert(), fn_trie_delete(), fn_trie_select_default()
(fn_trie_insert(), fn_trie_delete(), fn_trie_select_default())

	The route manipulation functions. Should conform pretty closely to the
	corresponding functions in fib_hash.
	(路由操作函数。应当与 fib_hash 中对应的函数相当接近。)

fn_trie_flush()
(fn_trie_flush())

	This walks the full trie (using nextleaf()) and searches for empty
	leaves which have to be removed.
	(它遍历整棵字典树(使用 nextleaf())，搜索必须移除的空叶子。)

fn_trie_dump()
(fn_trie_dump())

	Dumps the routing table ordered by prefix length. This is somewhat
	slower than the corresponding fib_hash function, as we have to walk the
	entire trie for each prefix length. In comparison, fib_hash is organized
	as one "zone"/hash per prefix length.
	(按前缀长度排序转储路由表。这比 fib_hash 中对应的函数要稍慢一些，因为对每个
	前缀长度我们都必须遍历整棵字典树。相比之下，fib_hash 按每个前缀长度组织为
	一个 "zone"/哈希。)

Locking(加锁)
-------------

fib_lock is used for an RW-lock in the same way that this is done in fib_hash.
However, the functions are somewhat separated for other possible locking
scenarios. It might conceivably be possible to run trie_rebalance via RCU
to avoid read_lock in the fn_trie_lookup() function.
(fib_lock 被用作 RW 锁，方式与 fib_hash 中的做法相同。不过，为了支持其他可能的
加锁场景，这些函数在一定程度上是相互分离的。可以设想通过 RCU 来运行
trie_rebalance，以避免 fn_trie_lookup() 函数中的 read_lock。)

Main lookup mechanism(主要查找机制)
-----------------------------------
fn_trie_lookup() is the main lookup function.
(fn_trie_lookup() 是主要的查找函数。)

The lookup is in its simplest form just like fib_find_node(). We descend the
trie, key segment by key segment, until we find a leaf. check_leaf() does
the fib_semantic_match in the leaf's sorted prefix hlist.
(查找在最简单的形式下就像 fib_find_node() 一样。我们沿键段一段一段地下降遍历
字典树，直到找到一个叶子。check_leaf() 在叶子的已排序前缀 hlist 中执行
fib_semantic_match。)

If we find a match, we are done.
(如果找到匹配，就完成了。)

If we don't find a match, we enter prefix matching mode. The prefix length,
starting out at the same as the key length, is reduced one step at a time,
and we backtrack upwards through the trie trying to find a longest matching
prefix. The goal is always to reach a leaf and get a positive result from the
fib_semantic_match mechanism.
(如果没有找到匹配，我们就进入前缀匹配模式。前缀长度起初与键长度相同，每次减小
一步，我们在字典树中向上回溯，试图找到最长匹配前缀。目标始终是到达一个叶子并从
fib_semantic_match 机制得到肯定的结果。)

Inside each tnode, the search for longest matching prefix consists of searching
through the child array, chopping off (zeroing) the least significant "1" of
the child index until we find a match or the child index consists of nothing but
zeros.
(在每个 tnode 内部，对最长匹配前缀的搜索包括在子节点数组中进行搜索，把子节点索引
的最低有效位 "1" 不断截去(清零)，直到找到匹配或者子节点索引只剩全 0 为止。)

At this point we backtrack (t->stats.backtrack++) up the trie, continuing to
chop off part of the key in order to find the longest matching prefix.
(此时我们沿字典树向上回溯(t->stats.backtrack++)，继续截去键的一部分，以找到最长
匹配前缀。)

At this point we will repeatedly descend subtries to look for a match, and there
are some optimizations available that can provide us with "shortcuts" to avoid
descending into dead ends. Look for "HL_OPTIMIZE" sections in the code.
(此时我们会反复下降进入各子树以寻找匹配，并且有一些可用的优化可以为我们提供
"捷径"，避免下降进入死胡同。在代码中查找 "HL_OPTIMIZE" 部分。)

To alleviate any doubts about the correctness of the route selection process,
a new netlink operation has been added. Look for NETLINK_FIB_LOOKUP, which
gives userland access to fib_lookup().
(为了缓解对路由选择过程正确性的任何疑虑，增加了一个新的 netlink 操作。查找
NETLINK_FIB_LOOKUP，它让用户空间可以访问 fib_lookup()。)
