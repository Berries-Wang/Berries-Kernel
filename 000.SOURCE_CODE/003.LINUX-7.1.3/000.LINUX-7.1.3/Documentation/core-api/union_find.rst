.. SPDX-License-Identifier: GPL-2.0

=====================================
Union-Find in Linux(Linux 中的并查集)
=====================================


:Date: June 21, 2024
(日期：2024 年 6 月 21 日)
:Author: Xavier <xavier_qy@163.com>
(作者：Xavier <xavier_qy@163.com>)

What is union-find, and what is it used for?(什么是并查集，它有什么用途？)
--------------------------------------------------------------------------

Union-find is a data structure used to handle the merging and querying
of disjoint sets. The primary operations supported by union-find are:
(并查集是一种用于处理不相交集合的合并与查询的数据结构。并查集支持的
主要操作有：)

	Initialization: Resetting each element as an individual set, with
		each set's initial parent node pointing to itself.
                (初始化：把每个元素重置为独立集合，每个集合的初始父节点
                指向自身。)

	Find: Determine which set a particular element belongs to, usually by
		returning a “representative element” of that set. This operation
		is used to check if two elements are in the same set.
                (查找：确定某个特定元素属于哪个集合，通常通过返回该集合的
                一个"代表元素"来实现。该操作用于检查两个元素是否在同一
                集合中。)

	Union: Merge two sets into one.
        (合并：把两个集合合并为一个。)

As a data structure used to maintain sets (groups), union-find is commonly
utilized to solve problems related to offline queries, dynamic connectivity,
and graph theory. It is also a key component in Kruskal's algorithm for
computing the minimum spanning tree, which is crucial in scenarios like
network routing. Consequently, union-find is widely referenced. Additionally,
union-find has applications in symbolic computation, register allocation,
and more.
(作为维护集合(分组)的数据结构，并查集常用于解决离线查询、动态连通性
和图论相关的问题。它也是计算最小生成树的 Kruskal 算法的关键组成部分，
而最小生成树在网络路由等场景中至关重要。因此，并查集被广泛引用。
此外，并查集还应用于符号计算、寄存器分配等领域。)

Space Complexity: O(n), where n is the number of nodes.
(空间复杂度：O(n)，其中 n 是节点数。)

Time Complexity: Using path compression can reduce the time complexity of
the find operation, and using union by rank can reduce the time complexity
of the union operation. These optimizations reduce the average time
complexity of each find and union operation to O(α(n)), where α(n) is the
inverse Ackermann function. This can be roughly considered a constant time
complexity for practical purposes.
(时间复杂度：路径压缩可以降低查找操作的时间复杂度，按秩合并可以降低
合并操作的时间复杂度。这些优化把每次查找和合并操作的平均时间复杂度
降到 O(α(n))，其中 α(n) 是反阿克曼函数。就实际应用而言，这可以粗略
地视为常数时间复杂度。)

This document covers use of the Linux union-find implementation.  For more
information on the nature and implementation of union-find,  see:
(本文介绍 Linux 并查集实现的用法。关于并查集的本质与实现的更多信息，
参见：)

  Wikipedia entry on union-find
    https://en.wikipedia.org/wiki/Disjoint-set_data_structure
    (关于并查集的 Wikipedia 条目)

Linux implementation of union-find(Linux 的并查集实现)
------------------------------------------------------

Linux's union-find implementation resides in the file "lib/union_find.c".
To use it, "#include <linux/union_find.h>".
(Linux 的并查集实现位于文件 "lib/union_find.c" 中。要使用它，需
"#include <linux/union_find.h>"。)

The union-find data structure is defined as follows::

	struct uf_node {
		struct uf_node *parent;
		unsigned int rank;
	};

(并查集数据结构定义如下：)

In this structure, parent points to the parent node of the current node.
The rank field represents the height of the current tree. During a union
operation, the tree with the smaller rank is attached under the tree with the
larger rank to maintain balance.
(在这个结构中，parent 指向当前节点的父节点。rank 字段表示当前树的
高度。合并操作时，秩较小的树会挂在秩较大的树之下，以保持平衡。)

Initializing union-find(初始化并查集)
-------------------------------------

You can complete the initialization using either static or initialization
interface. Initialize the parent pointer to point to itself and set the rank
to 0.
Example::

	struct uf_node my_node = UF_INIT_NODE(my_node);

(你可以使用静态初始化方式或初始化接口来完成初始化。把 parent 指针
初始化为指向自身，并将 rank 设为 0。示例：)

or(或者)

	uf_node_init(&my_node);

Find the Root Node of union-find(查找并查集的根节点)
----------------------------------------------------

This operation is mainly used to determine whether two nodes belong to the same
set in the union-find. If they have the same root, they are in the same set.
During the find operation, path compression is performed to improve the
efficiency of subsequent find operations.
Example::

	int connected;
	struct uf_node *root1 = uf_find(&node_1);
	struct uf_node *root2 = uf_find(&node_2);
	if (root1 == root2)
		connected = 1;
	else
		connected = 0;

(该操作主要用于判断两个节点在并查集中是否属于同一个集合。如果它们的
根相同，则它们在同一集合中。查找操作会执行路径压缩，以提高后续查找
操作的效率。示例：)

Union Two Sets in union-find(在并查集中合并两个集合)
----------------------------------------------------

To union two sets in the union-find, you first find their respective root nodes
and then link the smaller node to the larger node based on the rank of the root
nodes.
Example::

	uf_union(&node_1, &node_2);

(要在并查集中合并两个集合，先找到它们各自的根节点，然后根据根节点的
秩，把较小的节点链接到较大的节点上。示例：)
