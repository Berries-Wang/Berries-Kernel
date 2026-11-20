=========================================
BPF Graph Data Structures(BPF 图数据结构)
=========================================

This document describes implementation details of new-style "graph" data
structures (linked_list, rbtree), with particular focus on the verifier's
implementation of semantics specific to those data structures.
(本文档描述新式"图"数据结构 (linked_list、rbtree) 的实现细节，并特别关注验证器
针对这些数据结构特定语义的实现。)

Although no specific verifier code is referred to in this document, the document
assumes that the reader has general knowledge of BPF verifier internals, BPF
maps, and BPF program writing.
(尽管本文档没有引用任何具体的验证器代码，但它假定读者对 BPF 验证器内部机制、
BPF map 和 BPF 程序编写有一般性的了解。)

Note that the intent of this document is to describe the current state of
these graph data structures. **No guarantees** of stability for either
semantics or APIs are made or implied here.
(注意，本文档旨在描述这些图数据结构的现状。此处既未作出、也未暗示任何关于语义
或 API 稳定性的**保证**。)

.. contents::
    :local:
    :depth: 2

Introduction(简介)
------------------

The BPF map API has historically been the main way to expose data structures
of various types for use within BPF programs. Some data structures fit naturally
with the map API (HASH, ARRAY), others less so. Consequently, programs
interacting with the latter group of data structures can be hard to parse
for kernel programmers without previous BPF experience.
(BPF map API 历来是向 BPF 程序暴露各类数据结构的主要方式。有些数据结构天然契合
map API (HASH、ARRAY)，另一些则不然。因此，对于没有 BPF 经验的内核程序员来说，
与后一类数据结构交互的程序可能难以理解。)

Luckily, some restrictions which necessitated the use of BPF map semantics are
no longer relevant. With the introduction of kfuncs, kptrs, and the any-context
BPF allocator, it is now possible to implement BPF data structures whose API
and semantics more closely match those exposed to the rest of the kernel.
(幸运的是，一些曾迫使人们使用 BPF map 语义的限制如今已不再适用。随着 kfuncs、
kptrs 以及任意上下文可用的 BPF 分配器的引入，现在已经可以实现那些 API 和语义与
内核其余部分所暴露的更为接近的 BPF 数据结构了。)

Two such data structures - linked_list and rbtree - have many verification
details in common. Because both have "root"s ("head" for linked_list) and
"node"s, the verifier code and this document refer to common functionality
as "graph_api", "graph_root", "graph_node", etc.
(其中两种数据结构——linked_list 和 rbtree——在验证细节上有许多共同之处。
因为二者都有 "root"(对 linked_list 而言是 "head")和 "node"，验证器代码和
本文档把它们的共同功能称为 "graph_api"、"graph_root"、"graph_node" 等。)

Unless otherwise stated, examples and semantics below apply to both graph data
structures.
(除非另有说明，下文的示例和语义同时适用于这两种图数据结构。)

Unstable API(不稳定的 API)
--------------------------

Data structures implemented using the BPF map API have historically used BPF
helper functions - either standard map API helpers like ``bpf_map_update_elem``
or map-specific helpers. The new-style graph data structures instead use kfuncs
to define their manipulation helpers. Because there are no stability guarantees
for kfuncs, the API and semantics for these data structures can be evolved in
a way that breaks backwards compatibility if necessary.
(使用 BPF map API 实现的数据结构历来使用 BPF helper 函数——要么是标准的 map
API helper(如 ``bpf_map_update_elem``)，要么是特定于 map 的 helper。新式的图
数据结构则改用 kfuncs 来定义其操作 helper。由于 kfuncs 没有稳定性保证，这些
数据结构的 API 和语义可以在必要时以破坏向后兼容的方式演进。)

Root and node types for the new data structures are opaquely defined in the
``uapi/linux/bpf.h`` header.
(新数据结构的 root 与 node 类型在 ``uapi/linux/bpf.h`` 头文件中以不透明的
方式定义。)

Locking(加锁)
-------------

The new-style data structures are intrusive and are defined similarly to their
vanilla kernel counterparts:
(新式数据结构是侵入式的 (intrusive)，其定义方式与原生的内核对应物类似：)

.. code-block:: c

        struct node_data {
          long key;
          long data;
          struct bpf_rb_node node;
        };

        struct bpf_spin_lock glock;
        struct bpf_rb_root groot __contains(node_data, node);

The "root" type for both linked_list and rbtree expects to be in a map_value
which also contains a ``bpf_spin_lock`` - in the above example both global
variables are placed in a single-value arraymap. The verifier considers this
spin_lock to be associated with the ``bpf_rb_root`` by virtue of both being in
the same map_value and will enforce that the correct lock is held when
verifying BPF programs that manipulate the tree. Since this lock checking
happens at verification time, there is no runtime penalty.
(linked_list 和 rbtree 两者的 "root" 类型都要求位于一个同时包含
``bpf_spin_lock`` 的 map_value 中——在上面的例子里，两个全局变量被放在一个
单值 arraymap 中。验证器认为这个 spin_lock 与 ``bpf_rb_root`` 相关联，理由是
二者位于同一个 map_value 中，并在验证操作该树的 BPF 程序时强制要求持有正确的
锁。由于这种锁检查发生在验证阶段，运行时没有任何代价。)

Non-owning references(非拥有引用)
---------------------------------

**Motivation** (动机)

Consider the following BPF code:
(考虑以下 BPF 代码：)

.. code-block:: c

        struct node_data *n = bpf_obj_new(typeof(*n)); /* ACQUIRED */

        bpf_spin_lock(&lock);

        bpf_rbtree_add(&tree, n); /* PASSED */

        bpf_spin_unlock(&lock);

From the verifier's perspective, the pointer ``n`` returned from ``bpf_obj_new``
has type ``PTR_TO_BTF_ID | MEM_ALLOC``, with a ``btf_id`` of
``struct node_data`` and a nonzero ``ref_obj_id``. Because it holds ``n``, the
program has ownership of the pointee's (object pointed to by ``n``) lifetime.
The BPF program must pass off ownership before exiting - either via
``bpf_obj_drop``, which ``free``'s the object, or by adding it to ``tree`` with
``bpf_rbtree_add``.
(从验证器的角度看，``bpf_obj_new`` 返回的指针 ``n`` 具有
``PTR_TO_BTF_ID | MEM_ALLOC`` 类型，其 ``btf_id`` 为 ``struct node_data``，且
``ref_obj_id`` 非零。因为程序持有 ``n``，它就拥有被指者(即 ``n`` 所指向的对象)
生命周期的所有权。BPF 程序必须在退出之前交出所有权——要么通过 ``bpf_obj_drop``
把对象 ``free`` 掉，要么用 ``bpf_rbtree_add`` 把它加入 ``tree``。)

(``ACQUIRED`` and ``PASSED`` comments in the example denote statements where
"ownership is acquired" and "ownership is passed", respectively)
(示例中的 ``ACQUIRED`` 和 ``PASSED`` 注释分别标出"获得所有权"和"交出所有权"的
语句)

What should the verifier do with ``n`` after ownership is passed off? If the
object was ``free``'d with ``bpf_obj_drop`` the answer is obvious: the verifier
should reject programs which attempt to access ``n`` after ``bpf_obj_drop`` as
the object is no longer valid. The underlying memory may have been reused for
some other allocation, unmapped, etc.
(所有权交出之后，验证器应该如何看待 ``n``？如果对象是用 ``bpf_obj_drop``
``free`` 掉的，答案显而易见：验证器应拒绝在 ``bpf_obj_drop`` 之后仍试图访问
``n`` 的程序，因为对象已不再有效。底层内存可能已被重新用于其他分配、解除映射
等等。)

When ownership is passed to ``tree`` via ``bpf_rbtree_add`` the answer is less
obvious. The verifier could enforce the same semantics as for ``bpf_obj_drop``,
but that would result in programs with useful, common coding patterns being
rejected, e.g.:
(而当所有权通过 ``bpf_rbtree_add`` 交给 ``tree`` 时，答案就不那么明显了。
验证器可以执行与 ``bpf_obj_drop`` 相同的语义，但这会导致含有有用且常见编码模式
的程序被拒绝，例如：)

.. code-block:: c

        int x;
        struct node_data *n = bpf_obj_new(typeof(*n)); /* ACQUIRED */

        bpf_spin_lock(&lock);

        bpf_rbtree_add(&tree, n); /* PASSED */
        x = n->data;
        n->data = 42;

        bpf_spin_unlock(&lock);

Both the read from and write to ``n->data`` would be rejected. The verifier
can do better, though, by taking advantage of two details:
(对 ``n->data`` 的读和写都会被拒绝。不过，验证器可以利用以下两个细节做得
更好：)

  * Graph data structure APIs can only be used when the ``bpf_spin_lock``
    associated with the graph root is held
    (图数据结构 API 只能在持有与图根相关联的 ``bpf_spin_lock`` 时使用)

  * Both graph data structures have pointer stability
    (两种图数据结构都具有指针稳定性)

     * Because graph nodes are allocated with ``bpf_obj_new`` and
       adding / removing from the root involves fiddling with the
       ``bpf_{list,rb}_node`` field of the node struct, a graph node will
       remain at the same address after either operation.
       (由于图节点用 ``bpf_obj_new`` 分配，而向根添加/移除只涉及调整节点结构体
       的 ``bpf_{list,rb}_node`` 字段，图节点在任一操作之后仍会保持在同一
       地址。)

Because the associated ``bpf_spin_lock`` must be held by any program adding
or removing, if we're in the critical section bounded by that lock, we know
that no other program can add or remove until the end of the critical section.
This combined with pointer stability means that, until the critical section
ends, we can safely access the graph node through ``n`` even after it was used
to pass ownership.
(由于任何执行添加或移除的程序都必须持有相关联的 ``bpf_spin_lock``，如果我们
正处于该锁界定的临界区内，就可以断定在临界区结束之前没有其他程序能添加或移除
节点。结合指针稳定性，这意味着在临界区结束之前，即使 ``n`` 已被用于交出所有权，
我们仍可以通过它安全地访问该图节点。)

The verifier considers such a reference a *non-owning reference*. The ref
returned by ``bpf_obj_new`` is accordingly considered an *owning reference*.
Both terms currently only have meaning in the context of graph nodes and API.
(验证器把这样的引用视为*非拥有引用* (non-owning reference)。相应地，
``bpf_obj_new`` 返回的引用被视为*拥有引用* (owning reference)。这两个术语目前
只在图节点与图 API 的语境中才有意义。)

**Details** (细节)

Let's enumerate the properties of both types of references.
(让我们列举这两种引用的属性。)

*owning reference*
(拥有引用)

  * This reference controls the lifetime of the pointee
    (该引用控制被指者的生命周期)

  * Ownership of pointee must be 'released' by passing it to some graph API
    kfunc, or via ``bpf_obj_drop``, which ``free``'s the pointee
    (被指者的所有权必须通过把它传给某个图 API kfunc 来"释放"，或者通过
    ``bpf_obj_drop`` 把被指者 ``free`` 掉)

    * If not released before program ends, verifier considers program invalid
      (如果程序结束前未释放，验证器将认定程序无效)

  * Access to the pointee's memory will not page fault
    (访问被指者的内存不会发生缺页)

*non-owning reference*
(非拥有引用)

  * This reference does not own the pointee
    (该引用不拥有被指者)

     * It cannot be used to add the graph node to a graph root, nor ``free``'d via
       ``bpf_obj_drop``
       (它不能用于把图节点加入图根，也不能通过 ``bpf_obj_drop`` 被 ``free``
       掉)

  * No explicit control of lifetime, but can infer valid lifetime based on
    non-owning ref existence (see explanation below)
    (没有对生命周期的显式控制，但可以基于非拥有引用是否存在来推断有效的生命
    周期(见下面的解释))

  * Access to the pointee's memory will not page fault
    (访问被指者的内存不会发生缺页)

From verifier's perspective non-owning references can only exist
between spin_lock and spin_unlock. Why? After spin_unlock another program
can do arbitrary operations on the data structure like removing and ``free``-ing
via bpf_obj_drop. A non-owning ref to some chunk of memory that was remove'd,
``free``'d, and reused via bpf_obj_new would point to an entirely different thing.
Or the memory could go away.
(从验证器的角度看，非拥有引用只能存在于 spin_lock 与 spin_unlock 之间。
为什么？spin_unlock 之后，另一个程序可以对数据结构执行任意操作，比如移除节点
并通过 bpf_obj_drop ``free`` 掉。指向某段已被 remove、``free`` 并经
bpf_obj_new 重用的内存的非拥有引用，会指向完全不同的东西；或者那段内存可能
已经不复存在。)

To prevent this logic violation all non-owning references are invalidated by the
verifier after a critical section ends. This is necessary to ensure the "will
not page fault" property of non-owning references. So if the verifier hasn't
invalidated a non-owning ref, accessing it will not page fault.
(为了防止这种逻辑上的破坏，所有非拥有引用在临界区结束之后都会被验证器作废。
这是保证非拥有引用"不会缺页"特性所必需的。因此，如果验证器没有作废某个非拥有
引用，访问它就不会缺页。)

Currently ``bpf_obj_drop`` is not allowed in the critical section, so
if there's a valid non-owning ref, we must be in a critical section, and can
conclude that the ref's memory hasn't been dropped-and- ``free``'d or
dropped-and-reused.
(目前 ``bpf_obj_drop`` 不允许在临界区内使用，因此如果存在有效的非拥有引用，
我们必定处于某个临界区内，进而可以断定该引用指向的内存尚未被 drop 并 ``free``，
也未被 drop 后重用。)

Any reference to a node that is in an rbtree _must_ be non-owning, since
the tree has control of the pointee's lifetime. Similarly, any ref to a node
that isn't in rbtree _must_ be owning. This results in a nice property:
graph API add / remove implementations don't need to check if a node
has already been added (or already removed), as the ownership model
allows the verifier to prevent such a state from being valid by simply checking
types.
(任何指向位于 rbtree 中节点的引用都必须是非拥有引用，因为树控制着被指者的
生命周期。类似地，任何指向不在 rbtree 中节点的引用都必须是拥有引用。这带来
一个很好的性质：图 API 的添加/移除实现不需要检查节点是否已被添加(或已被
移除)，因为所有权模型允许验证器仅通过类型检查就阻止这种状态成为有效状态。)

However, pointer aliasing poses an issue for the above "nice property".
Consider the following example:
(不过，指针别名给上述"良好性质"带来了一个问题。考虑以下示例：)

.. code-block:: c

        struct node_data *n, *m, *o, *p;
        n = bpf_obj_new(typeof(*n));     /* 1 */

        bpf_spin_lock(&lock);

        bpf_rbtree_add(&tree, n);        /* 2 */
        m = bpf_rbtree_first(&tree);     /* 3 */

        o = bpf_rbtree_remove(&tree, n); /* 4 */
        p = bpf_rbtree_remove(&tree, m); /* 5 */

        bpf_spin_unlock(&lock);

        bpf_obj_drop(o);
        bpf_obj_drop(p); /* 6 */

Assume the tree is empty before this program runs. If we track verifier state
changes here using numbers in above comments:
(假设程序运行前树为空。如果我们用上面注释中的数字来跟踪验证器状态在此处的
变化：)

  1) n is an owning reference
     (n 是拥有引用)

  2) n is a non-owning reference, it's been added to the tree
     (n 是非拥有引用，它已被加入树中)

  3) n and m are non-owning references, they both point to the same node
     (n 和 m 都是非拥有引用，二者指向同一个节点)

  4) o is an owning reference, n and m non-owning, all point to same node
     (o 是拥有引用，n 和 m 是非拥有引用，三者指向同一个节点)

  5) o and p are owning, n and m non-owning, all point to the same node
     (o 和 p 是拥有引用，n 和 m 是非拥有引用，四者指向同一个节点)

  6) a double-free has occurred, since o and p point to same node and o was
     ``free``'d in previous statement
     (发生了双重释放，因为 o 和 p 指向同一个节点，而 o 已在上一条语句中被
     ``free`` 掉)

States 4 and 5 violate our "nice property", as there are non-owning refs to
a node which is not in an rbtree. Statement 5 will try to remove a node which
has already been removed as a result of this violation. State 6 is a dangerous
double-free.
(状态 4 和 5 违反了我们的"良好性质"，因为存在指向不在 rbtree 中的节点的非拥有
引用。这一违反导致语句 5 会试图移除一个已被移除的节点。状态 6 则是危险的双重
释放。)

At a minimum we should prevent state 6 from being possible. If we can't also
prevent state 5 then we must abandon our "nice property" and check whether a
node has already been removed at runtime.
(至少，我们应当防止状态 6 成为可能。如果我们无法同时阻止状态 5，就必须放弃
"良好性质"，在运行时检查节点是否已被移除。)

We prevent both by generalizing the "invalidate non-owning references" behavior
of ``bpf_spin_unlock`` and doing similar invalidation after
``bpf_rbtree_remove``. The logic here being that any graph API kfunc which:
(我们把 ``bpf_spin_unlock`` 的"作废非拥有引用"行为加以泛化，并在
``bpf_rbtree_remove`` 之后做类似的作废，从而同时防止这两种状态。这里的逻辑
是：任何满足以下条件的图 API kfunc：)

  * takes an arbitrary node argument
    (接受一个任意的节点参数)

  * removes it from the data structure
    (把它从数据结构中移除)

  * returns an owning reference to the removed node
    (返回指向被移除节点的拥有引用)

May result in a state where some other non-owning reference points to the same
node. So ``remove``-type kfuncs must be considered a non-owning reference
invalidation point as well.
(都可能造成某个其他非拥有引用指向同一节点的状态。因此，``remove`` 类的 kfunc
也必须被视为非拥有引用的作废点。)
