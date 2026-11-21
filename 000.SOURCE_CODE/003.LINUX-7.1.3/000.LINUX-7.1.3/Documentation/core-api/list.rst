.. SPDX-License-Identifier: GPL-2.0+

=====================================
Linked Lists in Linux(Linux 中的链表)
=====================================

:Author: Nicolas Frattaroli <nicolas.frattaroli@collabora.com>
(作者: Nicolas Frattaroli <nicolas.frattaroli@collabora.com>)

.. contents::

Introduction(引言)
==================

Linked lists are one of the most basic data structures used in many programs.
The Linux kernel implements several different flavours of linked lists. The
purpose of this document is not to explain linked lists in general, but to show
new kernel developers how to use the Linux kernel implementations of linked
lists.
(链表是许多程序中使用的最基本数据结构之一。Linux 内核实现了几种不同风格的链表。本文档的目的不是泛泛地讲解链表，而是向新的内核开发者展示如何使用 Linux 内核的链表实现。)

Please note that while linked lists certainly are ubiquitous, they are rarely
the best data structure to use in cases where a simple array doesn't already
suffice. In particular, due to their poor data locality, linked lists are a bad
choice in situations where performance may be of consideration. Familiarizing
oneself with other in-kernel generic data structures, especially for concurrent
accesses, is highly encouraged.
(请注意，尽管链表确实无处不在，但在简单数组尚且不够用的场合，链表也很少是最优的数据结构。特别是，由于数据局部性差，在可能需要考虑性能的场景中链表是糟糕的选择。强烈建议熟悉内核中其他的通用数据结构，尤其是面向并发访问的数据结构。)

Linux implementation of doubly linked lists(Linux 双向链表的实现)
=================================================================

Linux's linked list implementations can be used by including the header file
``<linux/list.h>``.
(包含头文件 ``<linux/list.h>`` 即可使用 Linux 的链表实现。)

The doubly-linked list will likely be the most familiar to many readers. It's a
list that can efficiently be traversed forwards and backwards.
(双向链表可能是许多读者最熟悉的。它是可以高效地向前和向后遍历的链表。)

The Linux kernel's doubly-linked list is circular in nature. This means that to
get from the head node to the tail, we can just travel one edge backwards.
Similarly, to get from the tail node to the head, we can simply travel forwards
"beyond" the tail and arrive back at the head.
(Linux 内核的双向链表本质上是循环的。这意味着从链表头节点到链表尾，只需向后走一条边；同样，从链表尾节点到链表头，只需向前走、"越过"链表尾即可回到链表头。)

Declaring a node(声明节点)
--------------------------

A node in a doubly-linked list is declared by adding a struct list_head
member to the data structure you wish to be contained in the list:
(双向链表中的节点，通过向希望放入链表的数据结构添加一个 struct list_head 成员来声明：)

.. code-block:: c

  struct clown {
          unsigned long long shoe_size;
          const char *name;
          struct list_head node;  /* the aforementioned member */
  };

This may be an unfamiliar approach to some, as the classical explanation of a
linked list is a list node data structure with pointers to the previous and next
list node, as well the payload data. Linux chooses this approach because it
allows for generic list modification code regardless of what data structure is
contained within the list. Since the struct list_head member is not a pointer
but part of the data structure proper, the container_of() pattern can be used by
the list implementation to access the payload data regardless of its type, while
staying oblivious to what said type actually is.
(对一些人来说这可能是个陌生的做法，因为对链表的经典解释是：链表节点数据结构带有指向前后链表节点的指针以及载荷数据。Linux 选择这种做法是因为它允许编写通用的链表修改代码，而无需关心链表内包含的是什么数据结构。由于 struct list_head 成员不是指针、而是数据结构本体的一部分，链表实现可以利用 container_of() 模式访问载荷数据，而不必关心其类型究竟是什么。)

Declaring and initializing a list(声明并初始化链表)
---------------------------------------------------

A doubly-linked list can then be declared as just another struct list_head,
and initialized with the LIST_HEAD_INIT() macro during initial assignment, or
with the INIT_LIST_HEAD() function later:
(双向链表随后可以被声明为又一个 struct list_head，在初始赋值时用 LIST_HEAD_INIT() 宏初始化，或者稍后用 INIT_LIST_HEAD() 函数初始化：)

.. code-block:: c

  struct clown_car {
          int tyre_pressure[4];
          struct list_head clowns;        /* Looks like a node! */
  };

  /* ... Somewhere later in our driver ... */

  static int circus_init(struct circus_priv *circus)
  {
          struct clown_car other_car = {
                .tyre_pressure = {10, 12, 11, 9},
                .clowns = LIST_HEAD_INIT(other_car.clowns)
          };

          INIT_LIST_HEAD(&circus->car.clowns);

          return 0;
  }

A further point of confusion to some may be that the list itself doesn't really
have its own type. The concept of the entire linked list and a
struct list_head member that points to other entries in the list are one and
the same.
(另一个可能让一些人困惑的地方是：链表本身其实并没有自己的类型。"整条链表"这一概念，与指向链表中其他条目的 struct list_head 成员，是同一回事。)

Adding nodes to the list(向链表添加节点)
----------------------------------------

Adding a node to the linked list is done through the list_add() macro.
(向链表添加节点通过 list_add() 宏完成。)

We'll return to our clown car example to illustrate how nodes get added to the
list:
(我们回到小丑车 (clown car) 的例子来说明节点如何被加入链表：)

.. code-block:: c

  static int circus_fill_car(struct circus_priv *circus)
  {
          struct clown_car *car = &circus->car;
          struct clown *grock;
          struct clown *dimitri;

          /* State 1 */

          grock = kzalloc(sizeof(*grock), GFP_KERNEL);
          if (!grock)
                  return -ENOMEM;
          grock->name = "Grock";
          grock->shoe_size = 1000;

          /* Note that we're adding the "node" member */
          list_add(&grock->node, &car->clowns);

          /* State 2 */

          dimitri = kzalloc(sizeof(*dimitri), GFP_KERNEL);
          if (!dimitri)
                  return -ENOMEM;
          dimitri->name = "Dimitri";
          dimitri->shoe_size = 50;

          list_add(&dimitri->node, &car->clowns);

          /* State 3 */

          return 0;
  }

In State 1, our list of clowns is still empty::

         .------.
         v      |
    .--------.  |
    | clowns |--'
    '--------'

(在状态 1，我们的小丑链表仍然是空的。)

This diagram shows the singular "clowns" node pointing at itself. In this
diagram, and all following diagrams, only the forward edges are shown, to aid in
clarity.
(此图显示单个 "clowns" 节点指向自身。在这张图以及后面所有的图中，为清晰起见只画出前向边。)

In State 2, we've added Grock after the list head::

         .--------------------.
         v                    |
    .--------.     .-------.  |
    | clowns |---->| Grock |--'
    '--------'     '-------'

(在状态 2，我们把 Grock 加到了链表头之后。)

This diagram shows the "clowns" node pointing at a new node labeled "Grock".
The Grock node is pointing back at the "clowns" node.
(此图显示 "clowns" 节点指向一个标记为 "Grock" 的新节点。Grock 节点则回指 "clowns" 节点。)

In State 3, we've added Dimitri after the list head, resulting in the following::

         .------------------------------------.
         v                                    |
    .--------.     .---------.     .-------.  |
    | clowns |---->| Dimitri |---->| Grock |--'
    '--------'     '---------'     '-------'

(在状态 3，我们把 Dimitri 加到了链表头之后，得到如下结果。)

This diagram shows the "clowns" node pointing at a new node labeled "Dimitri",
which then points at the node labeled "Grock". The "Grock" node still points
back at the "clowns" node.
(此图显示 "clowns" 节点指向一个标记为 "Dimitri" 的新节点，后者又指向标记为 "Grock" 的节点。"Grock" 节点仍回指 "clowns" 节点。)

If we wanted to have Dimitri inserted at the end of the list instead, we'd use
list_add_tail(). Our code would then look like this:
(如果我们想让 Dimitri 插到链表末尾，则应使用 list_add_tail()。这样代码看起来像下面这样：)

.. code-block:: c

  static int circus_fill_car(struct circus_priv *circus)
  {
          /* ... */

          list_add_tail(&dimitri->node, &car->clowns);

          /* State 3b */

          return 0;
  }

This results in the following list::

         .------------------------------------.
         v                                    |
    .--------.     .-------.     .---------.  |
    | clowns |---->| Grock |---->| Dimitri |--'
    '--------'     '-------'     '---------'

(这会得到如下链表。)

This diagram shows the "clowns" node pointing at the node labeled "Grock",
which points at the new node labeled "Dimitri". The node labeled "Dimitri"
points back at the "clowns" node.
(此图显示 "clowns" 节点指向标记为 "Grock" 的节点，后者指向标记为 "Dimitri" 的新节点。标记为 "Dimitri" 的节点回指 "clowns" 节点。)

Traversing the list(遍历链表)
-----------------------------

To iterate the list, we can loop through all nodes within the list with
list_for_each().
(要遍历链表，可以用 list_for_each() 循环访问链表中的所有节点。)

In our clown example, this results in the following somewhat awkward code:
(在小丑的例子中，这会得到下面这段略显笨拙的代码：)

.. code-block:: c

  static unsigned long long circus_get_max_shoe_size(struct circus_priv *circus)
  {
          unsigned long long res = 0;
          struct clown *e;
          struct list_head *cur;

          list_for_each(cur, &circus->car.clowns) {
                  e = list_entry(cur, struct clown, node);
                  if (e->shoe_size > res)
                          res = e->shoe_size;
          }

          return res;
  }

The list_entry() macro internally uses the aforementioned container_of() to
retrieve the data structure instance that ``node`` is a member of.
(list_entry() 宏内部使用前面提到的 container_of() 来取回 ``node`` 成员所属的数据结构实例。)

Note how the additional list_entry() call is a little awkward here. It's only
there because we're iterating through the ``node`` members, but we really want
to iterate through the payload, i.e. the ``struct clown`` that contains each
node's struct list_head. For this reason, there is a second macro:
list_for_each_entry()
(注意，这里额外的 list_entry() 调用有点别扭。它之所以存在，是因为我们遍历的是 ``node`` 成员，而真正想遍历的是载荷，即包含每个节点的 struct list_head 的 ``struct clown`` 。为此，内核提供了第二个宏：list_for_each_entry()。)

Using it would change our code to something like this:
(使用它会把我们的代码变成类似这样：)

.. code-block:: c

  static unsigned long long circus_get_max_shoe_size(struct circus_priv *circus)
  {
          unsigned long long res = 0;
          struct clown *e;

          list_for_each_entry(e, &circus->car.clowns, node) {
                  if (e->shoe_size > res)
                          res = e->shoe_size;
          }

          return res;
  }

This eliminates the need for the list_entry() step, and our loop cursor is now
of the type of our payload. The macro is given the member name that corresponds
to the list's struct list_head within the clown data structure so that it can
still walk the list.
(这省去了 list_entry() 这一步，而且我们的循环游标现在就是载荷的类型。该宏被传入了与 clown 数据结构内链表的 struct list_head 对应的成员名，因此它仍能沿链表行走。)

Removing nodes from the list(从链表移除节点)
--------------------------------------------

The list_del() function can be used to remove entries from the list. It not only
removes the given entry from the list, but poisons the entry's ``prev`` and
``next`` pointers, so that unintended use of the entry after removal does not
go unnoticed.
(list_del() 函数可用于从链表中移除条目。它不仅把给定条目从链表中移除，还会毒化 (poison) 该条目的 ``prev`` 和 ``next`` 指针，使移除后的意外使用不会悄无声息地发生。)

We can extend our previous example to remove one of the entries:
(我们可以扩展前面的例子来移除其中一个条目：)

.. code-block:: c

  static int circus_fill_car(struct circus_priv *circus)
  {
          /* ... */

          list_add(&dimitri->node, &car->clowns);

          /* State 3 */

          list_del(&dimitri->node);

          /* State 4 */

          return 0;
  }

The result of this would be this::

         .--------------------.
         v                    |
    .--------.     .-------.  |      .---------.
    | clowns |---->| Grock |--'      | Dimitri |
    '--------'     '-------'         '---------'

(这样做的结果如下。)

This diagram shows the "clowns" node pointing at the node labeled "Grock",
which points back at the "clowns" node. Off to the side is a lone node labeled
"Dimitri", which has no arrows pointing anywhere.
(此图显示 "clowns" 节点指向标记为 "Grock" 的节点，后者回指 "clowns" 节点。旁边是一个孤立的标记为 "Dimitri" 的节点，它没有任何指向别处的箭头。)

Note how the Dimitri node does not point to itself; its pointers are
intentionally set to a "poison" value that the list code refuses to traverse.
(注意，Dimitri 节点不指向自身；它的指针被刻意设置为一个 "poison" (毒) 值，链表代码拒绝遍历该值。)

If we wanted to reinitialize the removed node instead to make it point at itself
again like an empty list head, we can use list_del_init() instead:
(如果我们不想毒化、而是想重新初始化被移除的节点，让它像空链表头一样重新指向自身，可以改用 list_del_init()：)

.. code-block:: c

  static int circus_fill_car(struct circus_priv *circus)
  {
          /* ... */

          list_add(&dimitri->node, &car->clowns);

          /* State 3 */

          list_del_init(&dimitri->node);

          /* State 4b */

          return 0;
  }

This results in the deleted node pointing to itself again::

         .--------------------.           .-------.
         v                    |           v       |
    .--------.     .-------.  |      .---------.  |
    | clowns |---->| Grock |--'      | Dimitri |--'
    '--------'     '-------'         '---------'

(这样被删除的节点又会重新指向自身。)

This diagram shows the "clowns" node pointing at the node labeled "Grock",
which points back at the "clowns" node. Off to the side is a lone node labeled
"Dimitri", which points to itself.
(此图显示 "clowns" 节点指向标记为 "Grock" 的节点，后者回指 "clowns" 节点。旁边是一个孤立的标记为 "Dimitri" 的节点，它指向自身。)

Traversing whilst removing nodes(遍历的同时移除节点)
----------------------------------------------------

Deleting entries while we're traversing the list will cause problems if we use
list_for_each() and list_for_each_entry(), as deleting the current entry would
modify the ``next`` pointer of it, which means the traversal can't properly
advance to the next list entry.
(如果在遍历链表的同时删除条目，使用 list_for_each() 和 list_for_each_entry() 会出问题，因为删除当前条目会修改它的 ``next`` 指针，这意味着遍历无法正确前进到下一个链表条目。)

There is a solution to this however: list_for_each_safe() and
list_for_each_entry_safe(). These take an additional parameter of a pointer to
a struct list_head to use as temporary storage for the next entry during
iteration, solving the issue.
(不过这个问题有解决办法：list_for_each_safe() 和 list_for_each_entry_safe()。它们接受一个额外的参数——指向 struct list_head 的指针，在迭代期间用作下一个条目的临时存储，从而解决该问题。)

An example of how to use it:
(使用示例如下：)

.. code-block:: c

  static void circus_eject_insufficient_clowns(struct circus_priv *circus)
  {
          struct clown *e;
          struct clown *n;      /* temporary storage for safe iteration */

          list_for_each_entry_safe(e, n, &circus->car.clowns, node) {
                if (e->shoe_size < 500)
                        list_del(&e->node);
          }
  }

Proper memory management (i.e. freeing the deleted node while making sure
nothing still references it) in this case is left as an exercise to the reader.
(至于恰当的内存管理 (即释放被删除的节点、同时确保没有任何东西仍引用它)，此处留给读者作为练习。)

Cutting a list(切分链表)
------------------------

There are two helper functions to cut lists with. Both take elements from the
list ``head``, and replace the contents of the list ``list``.
(有两个用于切分链表的辅助函数。两者都从链表 ``head`` 中取走元素，并替换链表 ``list`` 的内容。)

The first such function is list_cut_position(). It removes all list entries from
``head`` up to and including ``entry``, placing them in ``list`` instead.
(第一个这样的函数是 list_cut_position()。它把 ``head`` 中从开头到 ``entry`` (含) 的所有链表条目移出，放到 ``list`` 中。)

In this example, it's assumed we start with the following list::

         .----------------------------------------------------------------.
         v                                                                |
    .--------.     .-------.     .---------.     .-----.     .---------.  |
    | clowns |---->| Grock |---->| Dimitri |---->| Pic |---->| Alfredo |--'
    '--------'     '-------'     '---------'     '-----'     '---------'

(本例假设我们以下面这个链表作为起点。)

With the following code, every clown up to and including "Pic" is moved from
the "clowns" list head to a separate struct list_head initialized at local
stack variable ``retirement``:
(通过下面的代码，从开头直到 "Pic" (含) 的每只小丑都会从 "clowns" 链表头移到初始化于局部栈变量 ``retirement`` 的另一个 struct list_head 中：)

.. code-block:: c

  static void circus_retire_clowns(struct circus_priv *circus)
  {
          struct list_head retirement = LIST_HEAD_INIT(retirement);
          struct clown *grock, *dimitri, *pic, *alfredo;
          struct clown_car *car = &circus->car;

          /* ... clown initialization, list adding ... */

          list_cut_position(&retirement, &car->clowns, &pic->node);

          /* State 1 */
  }

The resulting ``car->clowns`` list would be this::

         .----------------------.
         v                      |
    .--------.     .---------.  |
    | clowns |---->| Alfredo |--'
    '--------'     '---------'

(由此得到的 ``car->clowns`` 链表如下。)

Meanwhile, the ``retirement`` list is transformed to the following::

           .--------------------------------------------------.
           v                                                  |
    .------------.     .-------.     .---------.     .-----.  |
    | retirement |---->| Grock |---->| Dimitri |---->| Pic |--'
    '------------'     '-------'     '---------'     '-----'

(与此同时， ``retirement`` 链表则变成下面这样。)

The second function, list_cut_before(), is much the same, except it cuts before
the ``entry`` node, i.e. it removes all list entries from ``head`` up to but
excluding ``entry``, placing them in ``list`` instead. This example assumes the
same initial starting list as the previous example:
(第二个函数 list_cut_before() 大体相同，区别是它在 ``entry`` 节点之前切分，即移除 ``head`` 中从开头到 ``entry`` (不含) 的所有链表条目，把它们放到 ``list`` 中。本例假设与上一例相同的初始链表：)

.. code-block:: c

  static void circus_retire_clowns(struct circus_priv *circus)
  {
          struct list_head retirement = LIST_HEAD_INIT(retirement);
          struct clown *grock, *dimitri, *pic, *alfredo;
          struct clown_car *car = &circus->car;

          /* ... clown initialization, list adding ... */

          list_cut_before(&retirement, &car->clowns, &pic->node);

          /* State 1b */
  }

The resulting ``car->clowns`` list would be this::

         .----------------------------------.
         v                                  |
    .--------.     .-----.     .---------.  |
    | clowns |---->| Pic |---->| Alfredo |--'
    '--------'     '-----'     '---------'

(由此得到的 ``car->clowns`` 链表如下。)

Meanwhile, the ``retirement`` list is transformed to the following::

           .--------------------------------------.
           v                                      |
    .------------.     .-------.     .---------.  |
    | retirement |---->| Grock |---->| Dimitri |--'
    '------------'     '-------'     '---------'

(与此同时， ``retirement`` 链表则变成下面这样。)

It should be noted that both functions will destroy links to any existing nodes
in the destination ``struct list_head *list``.
(应当注意，这两个函数都会破坏目标 ``struct list_head *list`` 中任何既有节点的链接。)

Moving entries and partial lists(移动条目与部分链表)
----------------------------------------------------

The list_move() and list_move_tail() functions can be used to move an entry
from one list to another, to either the start or end respectively.
(list_move() 与 list_move_tail() 函数可以把条目从一个链表移到另一个链表，分别移到开头或末尾。)

In the following example, we'll assume we start with two lists ("clowns" and
"sidewalk" in the following initial state "State 0"::

         .----------------------------------------------------------------.
         v                                                                |
    .--------.     .-------.     .---------.     .-----.     .---------.  |
    | clowns |---->| Grock |---->| Dimitri |---->| Pic |---->| Alfredo |--'
    '--------'     '-------'     '---------'     '-----'     '---------'

          .-------------------.
          v                   |
    .----------.     .-----.  |
    | sidewalk |---->| Pio |--'
    '----------'     '-----'

(在下面的例子中，我们假设从两个链表 ("clowns" 与 "sidewalk") 开始，初始状态为 "状态 0"。)

We apply the following example code to the two lists:
(我们对这两个链表应用以下示例代码：)

.. code-block:: c

  static void circus_clowns_exit_car(struct circus_priv *circus)
  {
          struct list_head sidewalk = LIST_HEAD_INIT(sidewalk);
          struct clown *grock, *dimitri, *pic, *alfredo, *pio;
          struct clown_car *car = &circus->car;

          /* ... clown initialization, list adding ... */

          /* State 0 */

          list_move(&pic->node, &sidewalk);

          /* State 1 */

          list_move_tail(&dimitri->node, &sidewalk);

          /* State 2 */
  }

In State 1, we arrive at the following situation::

        .-----------------------------------------------------.
        |                                                     |
        v                                                     |
    .--------.     .-------.     .---------.     .---------.  |
    | clowns |---->| Grock |---->| Dimitri |---->| Alfredo |--'
    '--------'     '-------'     '---------'     '---------'

          .-------------------------------.
          v                               |
    .----------.     .-----.     .-----.  |
    | sidewalk |---->| Pic |---->| Pio |--'
    '----------'     '-----'     '-----'

(在状态 1，我们到达如下局面。)

In State 2, after we've moved Dimitri to the tail of sidewalk, the situation
changes as follows::

        .-------------------------------------.
        |                                     |
        v                                     |
    .--------.     .-------.     .---------.  |
    | clowns |---->| Grock |---->| Alfredo |--'
    '--------'     '-------'     '---------'

          .-----------------------------------------------.
          v                                               |
    .----------.     .-----.     .-----.     .---------.  |
    | sidewalk |---->| Pic |---->| Pio |---->| Dimitri |--'
    '----------'     '-----'     '-----'     '---------'

(在状态 2，把 Dimitri 移到 sidewalk 的末尾之后，局面变化如下。)

As long as the source and destination list head are part of the same list, we
can also efficiently bulk move a segment of the list to the tail end of the
list. We continue the previous example by adding a list_bulk_move_tail() after
State 2, moving Pic and Pio to the tail end of the sidewalk list.
(只要源和目标链表头属于同一条链表，我们还可以高效地把链表的一段整体移动到链表尾部。我们延续前面的例子，在状态 2 之后添加一个 list_bulk_move_tail() 调用，把 Pic 和 Pio 移到 sidewalk 链表的末尾。)

.. code-block:: c

  static void circus_clowns_exit_car(struct circus_priv *circus)
  {
          struct list_head sidewalk = LIST_HEAD_INIT(sidewalk);
          struct clown *grock, *dimitri, *pic, *alfredo, *pio;
          struct clown_car *car = &circus->car;

          /* ... clown initialization, list adding ... */

          /* State 0 */

          list_move(&pic->node, &sidewalk);

          /* State 1 */

          list_move_tail(&dimitri->node, &sidewalk);

          /* State 2 */

          list_bulk_move_tail(&sidewalk, &pic->node, &pio->node);

          /* State 3 */
  }

For the sake of brevity, only the altered "sidewalk" list at State 3 is depicted
in the following diagram::

          .-----------------------------------------------.
          v                                               |
    .----------.     .---------.     .-----.     .-----.  |
    | sidewalk |---->| Dimitri |---->| Pic |---->| Pio |--'
    '----------'     '---------'     '-----'     '-----'

(为简洁起见，下图中只画出状态 3 时发生了变化的 "sidewalk" 链表。)

Do note that list_bulk_move_tail() does not do any checking as to whether all
three supplied ``struct list_head *`` parameters really do belong to the same
list. If you use it outside the constraints the documentation gives, then the
result is a matter between you and the implementation.
(请注意，list_bulk_move_tail() 不会检查传入的三个 ``struct list_head *`` 参数是否真的属于同一条链表。如果在使用时超出了文档给出的约束，结果就是你与实现之间的事了。)

Rotating entries(旋转条目)
--------------------------

A common write operation on lists, especially when using them as queues, is
to rotate it. A list rotation means entries at the front are sent to the back.
(链表上一种常见的写操作是旋转，尤其是在把链表当队列使用时。链表旋转意味着把前端的条目送到后端。)

For rotation, Linux provides us with two functions: list_rotate_left() and
list_rotate_to_front(). The former can be pictured like a bicycle chain, taking
the entry after the supplied ``struct list_head *`` and moving it to the tail,
which in essence means the entire list, due to its circular nature, rotates by
one position.
(对于旋转，Linux 提供了两个函数：list_rotate_left() 和 list_rotate_to_front()。前者可以想象成自行车链条：取给定 ``struct list_head *`` 之后的那个条目并移到链表尾，由于链表的循环性质，这实质上意味着整条链表旋转了一个位置。)

The latter, list_rotate_to_front(), takes the same concept one step further:
instead of advancing the list by one entry, it advances it *until* the specified
entry is the new front.
(后者 list_rotate_to_front() 把这一概念再推进一步：它不是把链表前进一个条目，而是一直前进 *直到* 指定条目成为新的前端。)

In the following example, our starting state, State 0, is the following::

         .-----------------------------------------------------------------.
         v                                                                 |
    .--------.   .-------.   .---------.   .-----.   .---------.   .-----. |
    | clowns |-->| Grock |-->| Dimitri |-->| Pic |-->| Alfredo |-->| Pio |-'
    '--------'   '-------'   '---------'   '-----'   '---------'   '-----'

(在下面的例子中，我们的起始状态 (状态 0) 如下。)

The example code being used to demonstrate list rotations is the following:
(用来演示链表旋转的示例代码如下：)

.. code-block:: c

  static void circus_clowns_rotate(struct circus_priv *circus)
  {
          struct clown *grock, *dimitri, *pic, *alfredo, *pio;
          struct clown_car *car = &circus->car;

          /* ... clown initialization, list adding ... */

          /* State 0 */

          list_rotate_left(&car->clowns);

          /* State 1 */

          list_rotate_to_front(&alfredo->node, &car->clowns);

          /* State 2 */

  }

In State 1, we arrive at the following situation::

         .-----------------------------------------------------------------.
         v                                                                 |
    .--------.   .---------.   .-----.   .---------.   .-----.   .-------. |
    | clowns |-->| Dimitri |-->| Pic |-->| Alfredo |-->| Pio |-->| Grock |-'
    '--------'   '---------'   '-----'   '---------'   '-----'   '-------'

(在状态 1，我们到达如下局面。)

Next, after the list_rotate_to_front() call, we arrive in the following
State 2::

         .-----------------------------------------------------------------.
         v                                                                 |
    .--------.   .---------.   .-----.   .-------.   .---------.   .-----. |
    | clowns |-->| Alfredo |-->| Pio |-->| Grock |-->| Dimitri |-->| Pic |-'
    '--------'   '---------'   '-----'   '-------'   '---------'   '-----'

As is hopefully evident from the diagrams, the entries in front of "Alfredo"
were cycled to the tail end of the list.
(希望从图中可以明显看出，"Alfredo" 之前的条目被轮转到了链表末尾。)

Swapping entries(交换条目)
--------------------------

Another common operation is that two entries need to be swapped with each other.
(另一种常见操作是两个条目需要彼此交换。)

For this, Linux provides us with list_swap().
(为此，Linux 提供了 list_swap()。)

In the following example, we have a list with three entries, and swap two of
them. This is our starting state in "State 0"::

         .-----------------------------------------.
         v                                         |
    .--------.   .-------.   .---------.   .-----. |
    | clowns |-->| Grock |-->| Dimitri |-->| Pic |-'
    '--------'   '-------'   '---------'   '-----'


(在下面的例子中，我们有一个包含三个条目的链表，并交换其中两个。以下是我们 "状态 0" 的起始状态。)

.. code-block:: c

  static void circus_clowns_swap(struct circus_priv *circus)
  {
          struct clown *grock, *dimitri, *pic;
          struct clown_car *car = &circus->car;

          /* ... clown initialization, list adding ... */

          /* State 0 */

          list_swap(&dimitri->node, &pic->node);

          /* State 1 */
  }

The resulting list at State 1 is the following::

         .-----------------------------------------.
         v                                         |
    .--------.   .-------.   .-----.   .---------. |
    | clowns |-->| Grock |-->| Pic |-->| Dimitri |-'
    '--------'   '-------'   '-----'   '---------'

(状态 1 时得到的链表如下。)

As is evident by comparing the diagrams, the "Pic" and "Dimitri" nodes have
traded places.
(对比两图可以明显看出，"Pic" 与 "Dimitri" 节点互换了位置。)

Splicing two lists together(拼接两个链表)
-----------------------------------------

Say we have two lists, in the following example one represented by a list head
we call "knie" and one we call "stey". In a hypothetical circus acquisition,
the two list of clowns should be spliced together. The following is our
situation in "State 0"::

        .-----------------------------------------.
        |                                         |
        v                                         |
    .------.   .-------.   .---------.   .-----.  |
    | knie |-->| Grock |-->| Dimitri |-->| Pic |--'
    '------'   '-------'   '---------'   '-----'

        .-----------------------------.
        v                             |
    .------.   .---------.   .-----.  |
    | stey |-->| Alfredo |-->| Pio |--'
    '------'   '---------'   '-----'


(假设我们有两个链表，在下例中一个由称为 "knie" 的链表头表示，另一个称为 "stey"。在一次假想中的马戏团收购案里，这两个小丑链表应当被拼接起来。以下是我们 "状态 0" 的局面。)

The function to splice these two lists together is list_splice(). Our example
code is as follows:
(把这两个链表拼接在一起的函数是 list_splice()。我们的示例代码如下：)

.. code-block:: c

  static void circus_clowns_splice(void)
  {
          struct clown *grock, *dimitri, *pic, *alfredo, *pio;
          struct list_head knie = LIST_HEAD_INIT(knie);
          struct list_head stey = LIST_HEAD_INIT(stey);

          /* ... Clown allocation and initialization here ... */

          list_add_tail(&grock->node, &knie);
          list_add_tail(&dimitri->node, &knie);
          list_add_tail(&pic->node, &knie);
          list_add_tail(&alfredo->node, &stey);
          list_add_tail(&pio->node, &stey);

          /* State 0 */

          list_splice(&stey, &dimitri->node);

          /* State 1 */
  }

The list_splice() call here adds all the entries in ``stey`` to the list
``dimitri``'s ``node`` list_head is in, after the ``node`` of ``dimitri``. A
somewhat surprising diagram of the resulting "State 1" follows::

        .-----------------------------------------------------------------.
        |                                                                 |
        v                                                                 |
    .------.   .-------.   .---------.   .---------.   .-----.   .-----.  |
    | knie |-->| Grock |-->| Dimitri |-->| Alfredo |-->| Pio |-->| Pic |--'
    '------'   '-------'   '---------'   '---------'   '-----'   '-----'
                                              ^
              .-------------------------------'
              |
    .------.  |
    | stey |--'
    '------'


(这里的 list_splice() 调用把 ``stey`` 中的所有条目添加到 ``dimitri`` 的 ``node`` list_head 所在的链表中 ``dimitri`` 的 ``node`` 之后。以下是有些出人意料的 "状态 1" 结果图。)

Traversing the ``stey`` list no longer results in correct behavior. A call of
list_for_each() on ``stey`` results in an infinite loop, as it never returns
back to the ``stey`` list head.
(此时遍历 ``stey`` 链表不再能得到正确的行为。对 ``stey`` 调用 list_for_each() 会导致无限循环，因为它永远回不到 ``stey`` 链表头。)

This is because list_splice() did not reinitialize the list_head it took
entries from, leaving its pointer pointing into what is now a different list.
(这是因为 list_splice() 没有重新初始化它从中取走条目的那个 list_head，使其指针仍然指向如今已是另一条链表的地方。)

If we want to avoid this situation, list_splice_init() can be used. It does the
same thing as list_splice(), except reinitalizes the donor list_head after the
transplant.
(如果想避免这种情况，可以使用 list_splice_init()。它与 list_splice() 做的事情相同，只是在移植之后重新初始化供体 (donor) 的 list_head。)

Concurrency considerations(并发方面的考量)
------------------------------------------

Concurrent access and modification of a list needs to be protected with a lock
in most cases. Alternatively and preferably, one may use the RCU primitives for
lists in read-mostly use-cases, where read accesses to the list are common but
modifications to the list less so. See Documentation/RCU/listRCU.rst for more
details.
(在大多数情况下，对链表的并发访问与修改需要用锁来保护。或者 (而且更可取)，在读多写少的用例中，可以使用面向链表的 RCU 原语——链表读访问频繁而修改较少的情形。更多细节参见 Documentation/RCU/listRCU.rst。)

Further reading(延伸阅读)
-------------------------

* `How does the kernel implements Linked Lists? - KernelNewbies <https://kernelnewbies.org/FAQ/LinkedLists>`_

Full List API(完整 List API)
============================

.. kernel-doc:: include/linux/list.h
   :internal:

Private List API(私有 List API)
===============================

.. kernel-doc:: include/linux/list_private.h
   :doc: Private List Primitives

.. kernel-doc:: include/linux/list_private.h
   :internal:
