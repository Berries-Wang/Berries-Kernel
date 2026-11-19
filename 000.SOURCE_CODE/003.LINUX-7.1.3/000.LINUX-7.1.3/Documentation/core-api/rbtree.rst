===========================================================
Red-black Trees (rbtree) in Linux(Linux 中的红黑树(rbtree))
===========================================================


:Date: January 18, 2007
       (日期: 2007 年 1 月 18 日)
:Author: Rob Landley <rob@landley.net>
        (作者: Rob Landley <rob@landley.net>)

What are red-black trees, and what are they for?(什么是红黑树，它们有什么用？)
------------------------------------------------------------------------------

Red-black trees are a type of self-balancing binary search tree, used for
storing sortable key/value data pairs.  This differs from radix trees (which
are used to efficiently store sparse arrays and thus use long integer indexes
to insert/access/delete nodes) and hash tables (which are not kept sorted to
be easily traversed in order, and must be tuned for a specific size and
hash function where rbtrees scale gracefully storing arbitrary keys).
(红黑树是一种自平衡二叉搜索树，用于存储可排序的键/值数据对。这不同于基数树(radix tree，用于高效存储稀疏数组，因而使用长整数索引来插入/访问/删除节点)和哈希表(哈希表不保持排序、难以按序遍历，且必须针对特定大小与哈希函数调优，而 rbtree 则能优雅地伸缩、存储任意键)。)

Red-black trees are similar to AVL trees, but provide faster real-time bounded
worst case performance for insertion and deletion (at most two rotations and
three rotations, respectively, to balance the tree), with slightly slower
(but still O(log n)) lookup time.
(红黑树与 AVL 树相似，但插入与删除的最坏情况性能有更快且实时的上界(分别最多两次与三次旋转即可重新平衡树)，代价是查找时间稍慢(但仍为 O(log n))。)

To quote Linux Weekly News:
(引用 Linux Weekly News 的话：)

    There are a number of red-black trees in use in the kernel.
    The deadline and CFQ I/O schedulers employ rbtrees to
    track requests; the packet CD/DVD driver does the same.
    The high-resolution timer code uses an rbtree to organize outstanding
    timer requests.  The ext3 filesystem tracks directory entries in a
    red-black tree.  Virtual memory areas (VMAs) are tracked with red-black
    trees, as are epoll file descriptors, cryptographic keys, and network
    packets in the "hierarchical token bucket" scheduler.
    (内核中有若干棵红黑树在使用。deadline 与 CFQ I/O 调度器用 rbtree 跟踪请求；数据包 CD/DVD 驱动也是如此。高分辨率定时器代码使用一棵 rbtree 来组织未决的定时器请求。ext3 文件系统用红黑树跟踪目录项。虚拟内存区域(VMA)用红黑树跟踪，epoll 文件描述符、加密密钥以及 "层级令牌桶" 调度器中的网络数据包也是如此。)

This document covers use of the Linux rbtree implementation.  For more
information on the nature and implementation of Red Black Trees,  see:
(本文档讨论 Linux rbtree 实现的使用。关于红黑树的性质与实现的更多信息，参见：)

  Linux Weekly News article on red-black trees(Linux Weekly News 关于红黑树的文章)
    https://lwn.net/Articles/184495/

  Wikipedia entry on red-black trees(维基百科红黑树条目)
    https://en.wikipedia.org/wiki/Red-black_tree

Linux implementation of red-black trees(Linux 的红黑树实现)
-----------------------------------------------------------

Linux's rbtree implementation lives in the file "lib/rbtree.c".  To use it,
"#include <linux/rbtree.h>".
(Linux 的 rbtree 实现位于文件 "lib/rbtree.c" 中。要使用它， "#include <linux/rbtree.h>" 。)

The Linux rbtree implementation is optimized for speed, and thus has one
less layer of indirection (and better cache locality) than more traditional
tree implementations.  Instead of using pointers to separate rb_node and data
structures, each instance of struct rb_node is embedded in the data structure
it organizes.  And instead of using a comparison callback function pointer,
users are expected to write their own tree search and insert functions
which call the provided rbtree functions.  Locking is also left up to the
user of the rbtree code.

Creating a new rbtree(创建新的 rbtree)
--------------------------------------

Data nodes in an rbtree tree are structures containing a struct rb_node member::

  struct mytype {
  	struct rb_node node;
  	char *keystring;
  };

(rbtree 树中的数据节点是包含 struct rb_node 成员的结构：)

When dealing with a pointer to the embedded struct rb_node, the containing data
structure may be accessed with the standard container_of() macro.  In addition,
individual members may be accessed directly via rb_entry(node, type, member).
(当拿到指向内嵌 struct rb_node 的指针时，可以用标准的 container_of() 宏访问包含它的数据结构。此外，可以通过 rb_entry(node, type, member) 直接访问各个成员。)

At the root of each rbtree is an rb_root structure, which is initialized to be
empty via:
(每棵 rbtree 的根部是一个 rb_root 结构，它通过如下方式初始化为空：)

  struct rb_root mytree = RB_ROOT;

Searching for a value in an rbtree(在 rbtree 中查找值)
------------------------------------------------------

Writing a search function for your tree is fairly straightforward: start at the
root, compare each value, and follow the left or right branch as necessary.
(为你的树编写查找函数相当直观：从根开始，比较每个值，并按需沿左分支或右分支下行。)

Example::

  struct mytype *my_search(struct rb_root *root, char *string)
  {
  	struct rb_node *node = root->rb_node;

  	while (node) {
  		struct mytype *data = container_of(node, struct mytype, node);
		int result;

		result = strcmp(string, data->keystring);

		if (result < 0)
  			node = node->rb_left;
		else if (result > 0)
  			node = node->rb_right;
		else
  			return data;
	}
	return NULL;
  }

(示例：)

Inserting data into an rbtree(向 rbtree 中插入数据)
---------------------------------------------------

Inserting data in the tree involves first searching for the place to insert the
new node, then inserting the node and rebalancing ("recoloring") the tree.
(向树中插入数据首先需要查找插入新节点的位置，然后插入该节点并对树重新平衡("重新着色")。)

The search for insertion differs from the previous search by finding the
location of the pointer on which to graft the new node.  The new node also
needs a link to its parent node for rebalancing purposes.
(查找插入位置与前面的查找的不同之处在于：要找到可以嫁接新节点的那个指针的位置。为了重新平衡，新节点还需要一个指向其父节点的链接。)

Example::

  int my_insert(struct rb_root *root, struct mytype *data)
  {
  	struct rb_node **new = &(root->rb_node), *parent = NULL;

  	/* Figure out where to put new node */
  	while (*new) {
  		struct mytype *this = container_of(*new, struct mytype, node);
  		int result = strcmp(data->keystring, this->keystring);

		parent = *new;
  		if (result < 0)
  			new = &((*new)->rb_left);
  		else if (result > 0)
  			new = &((*new)->rb_right);
  		else
  			return FALSE;
  	}

  	/* Add new node and rebalance tree. */
  	rb_link_node(&data->node, parent, new);
  	rb_insert_color(&data->node, root);

	return TRUE;
  }

(示例：)

Removing or replacing existing data in an rbtree(在 rbtree 中移除或替换现有数据)
--------------------------------------------------------------------------------

To remove an existing node from a tree, call::

  void rb_erase(struct rb_node *victim, struct rb_root *tree);

(要从树中移除一个已有节点，调用：)

Example::

  struct mytype *data = mysearch(&mytree, "walrus");

  if (data) {
  	rb_erase(&data->node, &mytree);
  	myfree(data);
  }

(示例：)

To replace an existing node in a tree with a new one with the same key, call::

  void rb_replace_node(struct rb_node *old, struct rb_node *new,
  			struct rb_root *tree);

(要把树中的一个已有节点替换为具有相同键的新节点，调用：)

Replacing a node this way does not re-sort the tree: If the new node doesn't
have the same key as the old node, the rbtree will probably become corrupted.
(以这种方式替换节点不会对树重新排序：如果新节点的键与旧节点的键不同，rbtree 很可能会损坏。)

Iterating through the elements stored in an rbtree (in sort order)(遍历 rbtree 中存储的元素(按排序顺序))
--------------------------------------------------------------------------------------------------------

Four functions are provided for iterating through an rbtree's contents in
sorted order.  These work on arbitrary trees, and should not need to be
modified or wrapped (except for locking purposes)::

  struct rb_node *rb_first(struct rb_root *tree);
  struct rb_node *rb_last(struct rb_root *tree);
  struct rb_node *rb_next(struct rb_node *node);
  struct rb_node *rb_prev(struct rb_node *node);

(提供了四个用于按排序顺序遍历 rbtree 内容的函数。它们适用于任意树，应当不需要修改或包装(除加锁目的之外)：)

To start iterating, call rb_first() or rb_last() with a pointer to the root
of the tree, which will return a pointer to the node structure contained in
the first or last element in the tree.  To continue, fetch the next or previous
node by calling rb_next() or rb_prev() on the current node.  This will return
NULL when there are no more nodes left.
(要开始遍历，以指向树根的指针调用 rb_first() 或 rb_last()，它们会返回指向树中第一个或最后一个元素所含节点结构的指针。要继续遍历，对当前节点调用 rb_next() 或 rb_prev() 获取下一个或上一个节点。当没有更多节点剩余时返回 NULL。)

The iterator functions return a pointer to the embedded struct rb_node, from
which the containing data structure may be accessed with the container_of()
macro, and individual members may be accessed directly via
rb_entry(node, type, member).
(这些迭代器函数返回指向内嵌 struct rb_node 的指针，可以用 container_of() 宏从中访问包含它的数据结构，并且可以通过 rb_entry(node, type, member) 直接访问各个成员。)

Example::

  struct rb_node *node;
  for (node = rb_first(&mytree); node; node = rb_next(node))
	printk("key=%s\n", rb_entry(node, struct mytype, node)->keystring);

(示例：)

Cached rbtrees(缓存式 rbtree)
-----------------------------

Computing the leftmost (smallest) node is quite a common task for binary
search trees, such as for traversals or users relying on the particular
order for their own logic. To this end, users can use 'struct rb_root_cached'
to optimize O(logN) rb_first() calls to a simple pointer fetch avoiding
potentially expensive tree iterations. This is done at negligible runtime
overhead for maintenance; albeit larger memory footprint.
(计算最左(最小)节点对二叉搜索树来说是相当常见的任务，例如用于遍历，或供依赖特定顺序实现自身逻辑的使用者。为此，用户可以使用 'struct rb_root_cached' 把 O(logN) 的 rb_first() 调用优化为一次简单的指针读取，避免可能代价高昂的树遍历。这是以可忽略不计的运行时维护开销实现的；代价只是稍大的内存占用。)

Similar to the rb_root structure, cached rbtrees are initialized to be
empty via::

  struct rb_root_cached mytree = RB_ROOT_CACHED;

(与 rb_root 结构类似，缓存式 rbtree 通过如下方式初始化为空：)

Cached rbtree is simply a regular rb_root with an extra pointer to cache the
leftmost node. This allows rb_root_cached to exist wherever rb_root does,
which permits augmented trees to be supported as well as only a few extra
interfaces::

  struct rb_node *rb_first_cached(struct rb_root_cached *tree);
  void rb_insert_color_cached(struct rb_node *, struct rb_root_cached *, bool);
  void rb_erase_cached(struct rb_node *node, struct rb_root_cached *);

(缓存式 rbtree 就是一个普通的 rb_root 外加一个用于缓存最左节点的额外指针。这使 rb_root_cached 可以存在于任何 rb_root 存在的地方，从而在支持增强树(augmented tree)的同时只需额外增加少数几个接口：)

Both insert and erase calls have their respective counterpart of augmented
trees::

  void rb_insert_augmented_cached(struct rb_node *node, struct rb_root_cached *,
				  bool, struct rb_augment_callbacks *);
  void rb_erase_augmented_cached(struct rb_node *, struct rb_root_cached *,
				 struct rb_augment_callbacks *);

(插入与擦除调用都有各自对应的增强树版本：)


Support for Augmented rbtrees(对增强 rbtree 的支持)
---------------------------------------------------

Augmented rbtree is an rbtree with "some" additional data stored in
each node, where the additional data for node N must be a function of
the contents of all nodes in the subtree rooted at N. This data can
be used to augment some new functionality to rbtree. Augmented rbtree
is an optional feature built on top of basic rbtree infrastructure.
An rbtree user who wants this feature will have to call the augmentation
functions with the user provided augmentation callback when inserting
and erasing nodes.
(增强 rbtree 是在每棵节点中存储 "一些" 额外数据的 rbtree，其中节点 N 的额外数据必须是以 N 为根的子树中所有节点内容的某个函数。这些数据可以用来为 rbtree 增添新的功能。增强 rbtree 是构建在基础 rbtree 基础设施之上的可选特性。想要该特性的 rbtree 使用者必须在插入和擦除节点时，以自己提供的增强回调调用这些增强函数。)

C files implementing augmented rbtree manipulation must include
<linux/rbtree_augmented.h> instead of <linux/rbtree.h>. Note that
linux/rbtree_augmented.h exposes some rbtree implementations details
you are not expected to rely on; please stick to the documented APIs
there and do not include <linux/rbtree_augmented.h> from header files
either so as to minimize chances of your users accidentally relying on
such implementation details.
(实现增强 rbtree 操作的 C 文件必须包含 <linux/rbtree_augmented.h> 而非 <linux/rbtree.h>。注意，linux/rbtree_augmented.h 暴露了一些你不应依赖的 rbtree 实现细节；请坚持使用其中文档化的 API，并且同样不要从头文件中包含 <linux/rbtree_augmented.h>，以尽量降低你的用户意外依赖这些实现细节的可能性。)

On insertion, the user must update the augmented information on the path
leading to the inserted node, then call rb_link_node() as usual and
rb_augment_inserted() instead of the usual rb_insert_color() call.
If rb_augment_inserted() rebalances the rbtree, it will callback into
a user provided function to update the augmented information on the
affected subtrees.
(插入时，用户必须更新通向被插入节点路径上的增强信息，然后像往常一样调用 rb_link_node()，并以 rb_augment_inserted() 取代通常的 rb_insert_color() 调用。如果 rb_augment_inserted() 对 rbtree 重新平衡，它会回调用户提供的函数来更新受影响子树上的增强信息。)

When erasing a node, the user must call rb_erase_augmented() instead of
rb_erase(). rb_erase_augmented() calls back into user provided functions
to update the augmented information on affected subtrees.
(擦除节点时，用户必须调用 rb_erase_augmented() 而非 rb_erase()。rb_erase_augmented() 会回调用户提供的函数来更新受影响子树上的增强信息。)

In both cases, the callbacks are provided through struct rb_augment_callbacks.
3 callbacks must be defined:
(两种情况下，回调都通过 struct rb_augment_callbacks 提供。必须定义 3 个回调：)

- A propagation callback, which updates the augmented value for a given
  node and its ancestors, up to a given stop point (or NULL to update
  all the way to the root).
  (传播(propagation)回调，更新给定节点及其祖先的增强值，直到给定的停止点(或为 NULL 表示一路更新到根)。)

- A copy callback, which copies the augmented value for a given subtree
  to a newly assigned subtree root.
  (拷贝(copy)回调，把给定子树的增强值拷贝到新指定的子树根。)

- A tree rotation callback, which copies the augmented value for a given
  subtree to a newly assigned subtree root AND recomputes the augmented
  information for the former subtree root.
  (树旋转(rotation)回调，把给定子树的增强值拷贝到新指定的子树根，并且为原子树根重新计算增强信息。)

The compiled code for rb_erase_augmented() may inline the propagation and
copy callbacks, which results in a large function, so each augmented rbtree
user should have a single rb_erase_augmented() call site in order to limit
compiled code size.
(rb_erase_augmented() 的编译代码可能会内联传播与拷贝回调，这会产生一个很大的函数，因此每个增强 rbtree 使用者应当只保留一个 rb_erase_augmented() 调用点，以限制编译后的代码体积。)


Sample usage(用法示例)
^^^^^^^^^^^^^^^^^^^^^^

Interval tree is an example of augmented rb tree. Reference -
"Introduction to Algorithms" by Cormen, Leiserson, Rivest and Stein.
More details about interval trees:
(区间树(interval tree)是增强 rb 树的一个例子。参考文献——Cormen、Leiserson、Rivest 与 Stein 的《算法导论》(Introduction to Algorithms)。关于区间树的更多细节：)

Classical rbtree has a single key and it cannot be directly used to store
interval ranges like [lo:hi] and do a quick lookup for any overlap with a new
lo:hi or to find whether there is an exact match for a new lo:hi.
(经典 rbtree 只有一个键，无法直接用来存储像 [lo:hi] 这样的区间范围，也无法快速查找与新的 lo:hi 是否有重叠，或查找是否有与新的 lo:hi 精确匹配的区间。)

However, rbtree can be augmented to store such interval ranges in a structured
way making it possible to do efficient lookup and exact match.
(不过，rbtree 可以被增强，以结构化的方式存储这类区间范围，从而实现高效查找与精确匹配。)

This "extra information" stored in each node is the maximum hi
(max_hi) value among all the nodes that are its descendants. This
information can be maintained at each node just by looking at the node
and its immediate children. And this will be used in O(log n) lookup
for lowest match (lowest start address among all possible matches)
with something like::

  struct interval_tree_node *
  interval_tree_first_match(struct rb_root *root,
			    unsigned long start, unsigned long last)
  {
	struct interval_tree_node *node;

	if (!root->rb_node)
		return NULL;
	node = rb_entry(root->rb_node, struct interval_tree_node, rb);

	while (true) {
		if (node->rb.rb_left) {
			struct interval_tree_node *left =
				rb_entry(node->rb.rb_left,
					 struct interval_tree_node, rb);
			if (left->__subtree_last >= start) {
				/*
				 * Some nodes in left subtree satisfy Cond2.
				 * Iterate to find the leftmost such node N.
				 * If it also satisfies Cond1, that's the match
				 * we are looking for. Otherwise, there is no
				 * matching interval as nodes to the right of N
				 * can't satisfy Cond1 either.
				 */
				node = left;
				continue;
			}
		}
		if (node->start <= last) {		/* Cond1 */
			if (node->last >= start)	/* Cond2 */
				return node;	/* node is leftmost match */
			if (node->rb.rb_right) {
				node = rb_entry(node->rb.rb_right,
					struct interval_tree_node, rb);
				if (node->__subtree_last >= start)
					continue;
			}
		}
		return NULL;	/* No match */
	}
  }

(存储在每个节点中的 "额外信息" 是其所有后代节点中的最大 hi(max_hi)值。这一信息在每个节点处只需查看该节点及其直接子节点即可维护。在以 O(log n) 查找最低匹配(所有可能匹配中最低的起始地址)时会用到它，形如：)

Insertion/removal are defined using the following augmented callbacks::

  static inline unsigned long
  compute_subtree_last(struct interval_tree_node *node)
  {
	unsigned long max = node->last, subtree_last;
	if (node->rb.rb_left) {
		subtree_last = rb_entry(node->rb.rb_left,
			struct interval_tree_node, rb)->__subtree_last;
		if (max < subtree_last)
			max = subtree_last;
	}
	if (node->rb.rb_right) {
		subtree_last = rb_entry(node->rb.rb_right,
			struct interval_tree_node, rb)->__subtree_last;
		if (max < subtree_last)
			max = subtree_last;
	}
	return max;
  }

  static void augment_propagate(struct rb_node *rb, struct rb_node *stop)
  {
	while (rb != stop) {
		struct interval_tree_node *node =
			rb_entry(rb, struct interval_tree_node, rb);
		unsigned long subtree_last = compute_subtree_last(node);
		if (node->__subtree_last == subtree_last)
			break;
		node->__subtree_last = subtree_last;
		rb = rb_parent(&node->rb);
	}
  }

  static void augment_copy(struct rb_node *rb_old, struct rb_node *rb_new)
  {
	struct interval_tree_node *old =
		rb_entry(rb_old, struct interval_tree_node, rb);
	struct interval_tree_node *new =
		rb_entry(rb_new, struct interval_tree_node, rb);

	new->__subtree_last = old->__subtree_last;
  }

  static void augment_rotate(struct rb_node *rb_old, struct rb_node *rb_new)
  {
	struct interval_tree_node *old =
		rb_entry(rb_old, struct interval_tree_node, rb);
	struct interval_tree_node *new =
		rb_entry(rb_new, struct interval_tree_node, rb);

	new->__subtree_last = old->__subtree_last;
	old->__subtree_last = compute_subtree_last(old);
  }

  static const struct rb_augment_callbacks augment_callbacks = {
	augment_propagate, augment_copy, augment_rotate
  };

  void interval_tree_insert(struct interval_tree_node *node,
			    struct rb_root *root)
  {
	struct rb_node **link = &root->rb_node, *rb_parent = NULL;
	unsigned long start = node->start, last = node->last;
	struct interval_tree_node *parent;

	while (*link) {
		rb_parent = *link;
		parent = rb_entry(rb_parent, struct interval_tree_node, rb);
		if (parent->__subtree_last < last)
			parent->__subtree_last = last;
		if (start < parent->start)
			link = &parent->rb.rb_left;
		else
			link = &parent->rb.rb_right;
	}

	node->__subtree_last = last;
	rb_link_node(&node->rb, rb_parent, link);
	rb_insert_augmented(&node->rb, root, &augment_callbacks);
  }

  void interval_tree_remove(struct interval_tree_node *node,
			    struct rb_root *root)
  {
	rb_erase_augmented(&node->rb, root, &augment_callbacks);
  }

(插入与移除使用以下增强回调来定义：)
