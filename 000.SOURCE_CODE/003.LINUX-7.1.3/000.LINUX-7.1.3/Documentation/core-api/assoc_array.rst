============================================================
Generic Associative Array Implementation(通用关联数组的实现)
============================================================

Overview(概述)
==============

This associative array implementation is an object container with the following
properties:
(该关联数组实现是一个具有以下属性的对象容器：)

1. Objects are opaque pointers.  The implementation does not care where they
   point (if anywhere) or what they point to (if anything).
   (对象是不透明指针。实现不关心它们指向哪里(如果指向哪里的话)、指向什么
   (如果指向什么的话)。)

   .. note::

      Pointers to objects _must_ be zero in the least significant bit.
      (指向对象的指针在最低有效位上必须为零。)

2. Objects do not need to contain linkage blocks for use by the array.  This
   permits an object to be located in multiple arrays simultaneously.
   Rather, the array is made up of metadata blocks that point to objects.
   (对象无需包含供数组使用的链接块。这允许一个对象同时位于多个数组中。相
   反，数组由指向对象的元数据块构成。)

3. Objects require index keys to locate them within the array.
   (对象需要索引键才能在数组中被定位。)

4. Index keys must be unique.  Inserting an object with the same key as one
   already in the array will replace the old object.
   (索引键必须唯一。插入一个与数组中已有对象同键的对象，将替换旧对象。)

5. Index keys can be of any length and can be of different lengths.
   (索引键可以是任意长度，也可以是不同长度。)

6. Index keys should encode the length early on, before any variation due to
   length is seen.
   (索引键应在早期就编码其长度，先于任何因长度不同而产生的差异出现。)

7. Index keys can include a hash to scatter objects throughout the array.
   (索引键可以包含一个哈希，用来把对象散布到整个数组中。)

8. The array can iterated over.  The objects will not necessarily come out in
   key order.
   (可以对数组进行迭代。对象不一定会按键顺序输出。)

9. The array can be iterated over while it is being modified, provided the
   RCU readlock is being held by the iterator.  Note, however, under these
   circumstances, some objects may be seen more than once.  If this is a
   problem, the iterator should lock against modification.  Objects will not
   be missed, however, unless deleted.
   (数组在被修改的同时可以被迭代，前提是迭代者持有 RCU 读锁。但注意，在这
   种情况下，某些对象可能被看到不止一次。如果这是个问题，迭代者应通过加锁
   来防止修改。不过，除非对象被删除，否则不会漏掉任何对象。)

10. Objects in the array can be looked up by means of their index key.
    (可以通过索引键查找数组中的对象。)

11. Objects can be looked up while the array is being modified, provided the
    RCU readlock is being held by the thread doing the look up.
    (数组在被修改的同时可以查找对象，前提是执行查找的线程持有 RCU 读锁。)

The implementation uses a tree of 16-pointer nodes internally that are indexed
on each level by nibbles from the index key in the same manner as in a radix
tree.  To improve memory efficiency, shortcuts can be emplaced to skip over
what would otherwise be a series of single-occupancy nodes.  Further, nodes
pack leaf object pointers into spare space in the node rather than making an
extra branch until as such time an object needs to be added to a full node.
(该实现在内部使用一棵由 16 指针节点构成的树，每层都像基数树 (radix tree)
那样用索引键的半字节 (nibble) 进行索引。为提高内存效率，可以放置捷径
(shortcut)来跳过原本会是一系列单占用节点的部分。此外，节点会把叶子对象指
针打包进节点内的空余空间，而不是额外分支，直到需要把一个对象加入一个已满
的节点为止。)

The Public API(公共 API)
========================

The public API can be found in ``<linux/assoc_array.h>``.  The associative
array is rooted on the following structure::

    struct assoc_array {
            ...
    };

(公共 API 见 ``<linux/assoc_array.h>``。关联数组以下面的结构为根：)

The code is selected by enabling ``CONFIG_ASSOCIATIVE_ARRAY`` with::

    ./script/config -e ASSOCIATIVE_ARRAY

(通过启用 ``CONFIG_ASSOCIATIVE_ARRAY`` 来选中这些代码，命令如下：)

Edit Script(编辑脚本)
---------------------

The insertion and deletion functions produce an 'edit script' that can later be
applied to effect the changes without risking ``ENOMEM``. This retains the
preallocated metadata blocks that will be installed in the internal tree and
keeps track of the metadata blocks that will be removed from the tree when the
script is applied.
(插入与删除函数会产生一个"编辑脚本" (edit script)，稍后应用它来实施改动，
从而不必冒 ``ENOMEM`` 的风险。它会保留将安装到内部树中的预分配元数据块，
并跟踪脚本应用时将从树中移除的元数据块。)

This is also used to keep track of dead blocks and dead objects after the
script has been applied so that they can be freed later.  The freeing is done
after an RCU grace period has passed - thus allowing access functions to
proceed under the RCU read lock.
(它还用于在脚本应用之后跟踪死块与死对象，以便稍后释放它们。释放在一个 RCU
宽限期过后进行——这样访问函数就能在 RCU 读锁之下继续执行。)

The script appears as outside of the API as a pointer of the type::

    struct assoc_array_edit;

(在 API 之外，脚本表现为一个如下类型的指针：)

There are two functions for dealing with the script:
(有两个处理脚本的函数：)

1. Apply an edit script::

    void assoc_array_apply_edit(struct assoc_array_edit *edit);

   (应用一个编辑脚本：)

   This will perform the edit functions, interpolating various write barriers
   to permit accesses under the RCU read lock to continue.  The edit script
   will then be passed to ``call_rcu()`` to free it and any dead stuff it
   points to.
   (这将执行编辑操作，并插入各种写屏障，以允许在 RCU 读锁下的访问继续进
   行。之后，编辑脚本会被传给 ``call_rcu()``，把它及其指向的所有死对象释放
   掉。)

2. Cancel an edit script::

    void assoc_array_cancel_edit(struct assoc_array_edit *edit);

   (取消一个编辑脚本：)

   This frees the edit script and all preallocated memory immediately. If
   this was for insertion, the new object is *not* released by this function,
   but must rather be released by the caller.
   (这会立即释放编辑脚本与所有预分配内存。如果这是为插入而做的，新对象
   *不会* 被该函数释放，而必须由调用者释放。)

These functions are guaranteed not to fail.
(保证这两个函数不会失败。)

Operations Table(操作表)
------------------------

Various functions take a table of operations::

    struct assoc_array_ops {
            ...
    };

(许多函数接受一个操作表：)

This points to a number of methods, all of which need to be provided:
(它指向一组方法，所有这些方法都必须提供：)

1. Get a chunk of index key from caller data::

    unsigned long (*get_key_chunk)(const void *index_key, int level);

   (从调用者数据中取一段索引键：)

   This should return a chunk of caller-supplied index key starting at the
   *bit* position given by the level argument.  The level argument will be a
   multiple of ``ASSOC_ARRAY_KEY_CHUNK_SIZE`` and the function should return
   ``ASSOC_ARRAY_KEY_CHUNK_SIZE bits``.  No error is possible.
   (它应返回调用者提供的索引键中、从 level 参数给定的 *位* 位置开始的一段
   索引键。level 参数会是 ``ASSOC_ARRAY_KEY_CHUNK_SIZE`` 的倍数，函数应返回
   ``ASSOC_ARRAY_KEY_CHUNK_SIZE`` 位。不可能出错。)

2. Get a chunk of an object's index key::

    unsigned long (*get_object_key_chunk)(const void *object, int level);

   (取某个对象的索引键的一段：)

   As the previous function, but gets its data from an object in the array
   rather than from a caller-supplied index key.
   (与上一个函数相同，但其数据来自数组中的一个对象，而不是调用者提供的索
   引键。)

3. See if this is the object we're looking for::

    bool (*compare_object)(const void *object, const void *index_key);

   (看看这是否是我们要找的对象：)

   Compare the object against an index key and return ``true`` if it matches
   and ``false`` if it doesn't.
   (把对象与一个索引键比较，匹配则返回 ``true``，不匹配则返回 ``false``。)

4. Diff the index keys of two objects::

    int (*diff_objects)(const void *object, const void *index_key);

   (比较两个对象的索引键的差异：)

   Return the bit position at which the index key of the specified object
   differs from the given index key or -1 if they are the same.
   (返回指定对象的索引键与给定索引键出现差异的位位置；若相同则返回 -1。)

5. Free an object::

    void (*free_object)(void *object);

   (释放一个对象：)

   Free the specified object.  Note that this may be called an RCU grace period
   after ``assoc_array_apply_edit()`` was called, so ``synchronize_rcu()`` may
   be necessary on module unloading.
   (释放指定对象。注意，它可能在 ``assoc_array_apply_edit()`` 被调用之后的
   一个 RCU 宽限期才被调用，因此卸载模块时可能需要 ``synchronize_rcu()``。)

Manipulation Functions(操作函数)
--------------------------------

There are a number of functions for manipulating an associative array:
(有若干用于操作关联数组的函数：)

1. Initialise an associative array::

    void assoc_array_init(struct assoc_array *array);

   (初始化一个关联数组：)

   This initialises the base structure for an associative array.  It can't fail.
   (它初始化关联数组的基础结构。不会失败。)

2. Insert/replace an object in an associative array::

    struct assoc_array_edit *
    assoc_array_insert(struct assoc_array *array,
                       const struct assoc_array_ops *ops,
                       const void *index_key,
                       void *object);

   (向关联数组插入/替换一个对象：)

   This inserts the given object into the array.  Note that the least
   significant bit of the pointer must be zero as it's used to type-mark
   pointers internally.
   (它把给定对象插入数组。注意，指针的最低有效位必须为零，因为它在内部被
   用于指针的类型标记。)

   If an object already exists for that key then it will be replaced with the
   new object and the old one will be freed automatically.
   (如果该键已有对象存在，它将被新对象替换，旧对象会被自动释放。)

   The ``index_key`` argument should hold index key information and is
   passed to the methods in the ops table when they are called.
   (``index_key`` 参数应承载索引键信息，并在 ops 表中的方法被调用时传给它
   们。)

   This function makes no alteration to the array itself, but rather returns
   an edit script that must be applied.  ``-ENOMEM`` is returned in the case of
   an out-of-memory error.
   (该函数不会对数组本身做任何改动，而是返回一个必须被应用的编辑脚本。发生
   内存不足错误时返回 ``-ENOMEM``。)

   The caller should lock exclusively against other modifiers of the array.
   (调用者应以独占方式加锁，防止数组的其他修改者。)

3. Delete an object from an associative array::

    struct assoc_array_edit *
    assoc_array_delete(struct assoc_array *array,
                       const struct assoc_array_ops *ops,
                       const void *index_key);

   (从关联数组删除一个对象：)

   This deletes an object that matches the specified data from the array.
   (它从数组中删除与指定数据匹配的对象。)

   The ``index_key`` argument should hold index key information and is
   passed to the methods in the ops table when they are called.
   (``index_key`` 参数应承载索引键信息，并在 ops 表中的方法被调用时传给它
   们。)

   This function makes no alteration to the array itself, but rather returns
   an edit script that must be applied.  ``-ENOMEM`` is returned in the case of
   an out-of-memory error.  ``NULL`` will be returned if the specified object
   is not found within the array.
   (该函数不会对数组本身做任何改动，而是返回一个必须被应用的编辑脚本。发生
   内存不足错误时返回 ``-ENOMEM``。如果指定的对象在数组中找不到，则返回
   ``NULL``。)

The caller should lock exclusively against other modifiers of the array.
(调用者应以独占方式加锁，防止数组的其他修改者。)

4. Delete all objects from an associative array::

    struct assoc_array_edit *
    assoc_array_clear(struct assoc_array *array,
                      const struct assoc_array_ops *ops);

   (删除关联数组中的所有对象：)

   This deletes all the objects from an associative array and leaves it
   completely empty.
   (它删除关联数组中的所有对象，使其完全变空。)

   This function makes no alteration to the array itself, but rather returns
   an edit script that must be applied.  ``-ENOMEM`` is returned in the case of
   an out-of-memory error.
   (该函数不会对数组本身做任何改动，而是返回一个必须被应用的编辑脚本。发生
   内存不足错误时返回 ``-ENOMEM``。)

   The caller should lock exclusively against other modifiers of the array.
   (调用者应以独占方式加锁，防止数组的其他修改者。)

5. Destroy an associative array, deleting all objects::

    void assoc_array_destroy(struct assoc_array *array,
                             const struct assoc_array_ops *ops);

   (销毁一个关联数组，删除所有对象：)

   This destroys the contents of the associative array and leaves it
   completely empty.  It is not permitted for another thread to be traversing
   the array under the RCU read lock at the same time as this function is
   destroying it as no RCU deferral is performed on memory release -
   something that would require memory to be allocated.
   (它销毁关联数组的内容并使其完全变空。不允许其他线程在该函数销毁数组的
   同时在 RCU 读锁下遍历该数组，因为内存释放不做任何 RCU 延迟——而延迟将
   需要分配内存。)

   The caller should lock exclusively against other modifiers and accessors
   of the array.
   (调用者应以独占方式加锁，防止数组的其他修改者与访问者。)

6. Garbage collect an associative array::

    int assoc_array_gc(struct assoc_array *array,
                       const struct assoc_array_ops *ops,
                       bool (*iterator)(void *object, void *iterator_data),
                       void *iterator_data);

   (对一个关联数组做垃圾回收：)

   This iterates over the objects in an associative array and passes each one
   to ``iterator()``.  If ``iterator()`` returns ``true``, the object is kept.
   If it returns ``false``, the object will be freed.  If the ``iterator()``
   function returns ``true``, it must perform any appropriate refcount
   incrementing on the object before returning.
   (它遍历关联数组中的对象，并把每个对象传给 ``iterator()``。如果
   ``iterator()`` 返回 ``true``，对象被保留；返回 ``false`` 则对象将被释放。
   如果 ``iterator()`` 返回 ``true``，它必须在返回之前对该对象执行任何适当
   的引用计数递增。)

   The internal tree will be packed down if possible as part of the iteration
   to reduce the number of nodes in it.
   (作为迭代的一部分，内部树会在可能的情况下被压实，以减少其中的节点数。)

   The ``iterator_data`` is passed directly to ``iterator()`` and is otherwise
   ignored by the function.
   (``iterator_data`` 被直接传给 ``iterator()``，除此之外该函数不使用它。)

   The function will return ``0`` if successful and ``-ENOMEM`` if there wasn't
   enough memory.
   (成功时该函数返回 ``0``，内存不足时返回 ``-ENOMEM``。)

   It is possible for other threads to iterate over or search the array under
   the RCU read lock while this function is in progress.  The caller should
   lock exclusively against other modifiers of the array.
   (在该函数执行期间，其他线程可以在 RCU 读锁下遍历或搜索该数组。调用者应
   以独占方式加锁，防止数组的其他修改者。)

Access Functions(访问函数)
--------------------------

There are two functions for accessing an associative array:
(有两个访问关联数组的函数：)

1. Iterate over all the objects in an associative array::

    int assoc_array_iterate(const struct assoc_array *array,
                            int (*iterator)(const void *object,
                                            void *iterator_data),
                            void *iterator_data);

   (迭代关联数组中的所有对象：)

   This passes each object in the array to the iterator callback function.
   ``iterator_data`` is private data for that function.
   (它把数组中的每个对象传给迭代器回调函数。``iterator_data`` 是该函数的
   私有数据。)

   This may be used on an array at the same time as the array is being
   modified, provided the RCU read lock is held.  Under such circumstances,
   it is possible for the iteration function to see some objects twice.  If
   this is a problem, then modification should be locked against.  The
   iteration algorithm should not, however, miss any objects.
   (只要持有 RCU 读锁，就可以在数组被修改的同时对它使用此函数。在这种情况
   下，迭代函数可能把某些对象看到两次。如果这是个问题，应当通过加锁防止修
   改。不过，迭代算法不应漏掉任何对象。)

   The function will return ``0`` if no objects were in the array or else it
   will return the result of the last iterator function called.  Iteration
   stops immediately if any call to the iteration function results in a
   non-zero return.
   (如果数组中没有对象，该函数返回 ``0``；否则返回最后一次调用的迭代器函数
   的结果。只要对迭代函数的任何一次调用返回非零值，迭代就立即停止。)

2. Find an object in an associative array::

    void *assoc_array_find(const struct assoc_array *array,
                           const struct assoc_array_ops *ops,
                           const void *index_key);

   (在关联数组中查找一个对象：)

   This walks through the array's internal tree directly to the object
   specified by the index key.
   (它沿数组内部树直接走到索引键所指定的对象。)

   This may be used on an array at the same time as the array is being
   modified, provided the RCU read lock is held.
   (只要持有 RCU 读锁，就可以在数组被修改的同时对它使用此函数。)

   The function will return the object if found (and set ``*_type`` to the
   object type) or will return ``NULL`` if the object was not found.
   (找到则返回该对象(并把 ``*_type`` 设置为对象类型)；找不到则返回
   ``NULL``。)

Index Key Form(索引键的形式)
----------------------------

The index key can be of any form, but since the algorithms aren't told how long
the key is, it is strongly recommended that the index key includes its length
very early on before any variation due to the length would have an effect on
comparisons.
(索引键可以是任何形式，但由于算法并不知道键有多长，强烈建议索引键在非常
靠前的位置就包含其长度——先于任何因长度不同而影响比较的差异出现。)

This will cause leaves with different length keys to scatter away from each
other - and those with the same length keys to cluster together.
(这会使键长度不同的叶子彼此散开——而键长度相同的叶子聚在一起。)

It is also recommended that the index key begin with a hash of the rest of the
key to maximise scattering throughout keyspace.
(还建议索引键以其余键内容的哈希开头，以在整个键空间中最大化散布。)

The better the scattering, the wider and lower the internal tree will be.
(散布越好，内部树就越宽、越矮。)

Poor scattering isn't too much of a problem as there are shortcuts and nodes
can contain mixtures of leaves and metadata pointers.
(散布差也不是太大的问题，因为有捷径存在，而且节点可以混合容纳叶子和元数
据指针。)

The index key is read in chunks of machine word.  Each chunk is subdivided into
one nibble (4 bits) per level, so on a 32-bit CPU this is good for 8 levels and
on a 64-bit CPU, 16 levels.  Unless the scattering is really poor, it is
unlikely that more than one word of any particular index key will have to be
used.
(索引键按机器字分块读取。每块在每一层被细分为一个半字节 (4 位)，因此在
32 位 CPU 上可供 8 层使用，在 64 位 CPU 上可供 16 层使用。除非散布真的很
差，否则任何特定索引键都很少需要用到超过一个字。)

Internal Workings(内部工作原理)
===============================

The associative array data structure has an internal tree.  This tree is
constructed of two types of metadata blocks: nodes and shortcuts.
(关联数组数据结构有一棵内部树。这棵树由两种类型的元数据块构成：节点
(node)与捷径 (shortcut)。)

A node is an array of slots.  Each slot can contain one of four things:
(节点是一个槽数组。每个槽可以包含以下四种东西之一：)

* A NULL pointer, indicating that the slot is empty.
  (一个 NULL 指针，表示该槽为空。)
* A pointer to an object (a leaf).
  (一个指向对象的指针(叶子)。)
* A pointer to a node at the next level.
  (一个指向下一层节点的指针。)
* A pointer to a shortcut.
  (一个指向捷径的指针。)

Basic Internal Tree Layout(基本内部树布局)
------------------------------------------

Ignoring shortcuts for the moment, the nodes form a multilevel tree.  The index
key space is strictly subdivided by the nodes in the tree and nodes occur on
fixed levels.  For example::

 Level: 0               1               2               3
        =============== =============== =============== ===============
                                                        NODE D
                        NODE B          NODE C  +------>+---+
                +------>+---+   +------>+---+   |       | 0 |
        NODE A  |       | 0 |   |       | 0 |   |       +---+
        +---+   |       +---+   |       +---+   |       :   :
        | 0 |   |       :   :   |       :   :   |       +---+
        +---+   |       +---+   |       +---+   |       | f |
        | 1 |---+       | 3 |---+       | 7 |---+       +---+
        +---+           +---+           +---+
        :   :           :   :           | 8 |---+
        +---+           +---+           +---+   |       NODE E
        | e |---+       | f |           :   :   +------>+---+
        +---+   |       +---+           +---+           | 0 |
        | f |   |                       | f |           +---+
        +---+   |                       +---+           :   :
                |       NODE F                          +---+
                +------>+---+                           | f |
                        | 0 |           NODE G          +---+
                        +---+   +------>+---+
                        :   :   |       | 0 |
                        +---+   |       +---+
                        | 6 |---+       :   :
                        +---+           +---+
                        :   :           | f |
                        +---+           +---+
                        | f |
                        +---+

(暂时忽略捷径，这些节点构成一棵多层树。索引键空间被树中的节点严格细分，
节点出现在固定的层上。例如上面的示意图。)

In the above example, there are 7 nodes (A-G), each with 16 slots (0-f).
Assuming no other meta data nodes in the tree, the key space is divided
thusly:
(在上面的例子中，有 7 个节点(A-G)，每个节点有 16 个槽(0-f)。假设树中没有
其他元数据节点，键空间划分如下：)

    ===========     ====
    KEY PREFIX      NODE
    (键前缀)        (节点)
    ===========     ====
    137*            D
    138*            E
    13[0-69-f]*     C
    1[0-24-f]*      B
    e6*             G
    e[0-57-f]*      F
    [02-df]*        A
    ===========     ====

So, for instance, keys with the following example index keys will be found in
the appropriate nodes:
(例如，具有下面这些示例索引键的键将可在相应节点中找到：)

    =============== ======= ====
    INDEX KEY       PREFIX  NODE
    (索引键)        (前缀)  (节点)
    =============== ======= ====
    13694892892489  13      C
    13795289025897  137     D
    13889dde88793   138     E
    138bbb89003093  138     E
    1394879524789   12      C
    1458952489      1       B
    9431809de993ba  \-      A
    b4542910809cd   \-      A
    e5284310def98   e       F
    e68428974237    e6      G
    e7fffcbd443     e       F
    f3842239082     \-      A
    =============== ======= ====

To save memory, if a node can hold all the leaves in its portion of keyspace,
then the node will have all those leaves in it and will not have any metadata
pointers - even if some of those leaves would like to be in the same slot.
(为了节省内存，如果一个节点能容纳其键空间份额内的所有叶子，那么该节点将
把所有这些叶子都装在自己里面，而不带任何元数据指针——即使其中某些叶子希
望落在同一个槽中也是如此。)

A node can contain a heterogeneous mix of leaves and metadata pointers.
Metadata pointers must be in the slots that match their subdivisions of key
space.  The leaves can be in any slot not occupied by a metadata pointer.  It
is guaranteed that none of the leaves in a node will match a slot occupied by a
metadata pointer.  If the metadata pointer is there, any leaf whose key matches
the metadata key prefix must be in the subtree that the metadata pointer points
to.
(一个节点可以混合容纳异构的叶子与元数据指针。元数据指针必须位于与其键空
间细分相匹配的槽中。叶子可以位于任何未被元数据指针占据的槽。可以保证节点
中没有任何叶子会落在被元数据指针占用的槽上。如果元数据指针在那里，那么键
匹配该元数据键前缀的任何叶子都必须在元数据指针所指向的子树中。)

In the above example list of index keys, node A will contain:
(在上面那份索引键示例列表中，节点 A 将包含：)

    ====    =============== ==================
    SLOT    CONTENT         INDEX KEY (PREFIX)
    (槽)    (内容)          (索引键(前缀))
    ====    =============== ==================
    1       PTR TO NODE B   1*
    any     LEAF            9431809de993ba
    any     LEAF            b4542910809cd
    e       PTR TO NODE F   e*
    any     LEAF            f3842239082
    ====    =============== ==================

and node B:
(而节点 B 则包含：)

    ====    =============== ==================
    SLOT    CONTENT         INDEX KEY (PREFIX)
    (槽)    (内容)          (索引键(前缀))
    ====    =============== ==================
    3       PTR TO NODE C   13*
    any     LEAF            1458952489
    ====    =============== ==================

Shortcuts(捷径)
---------------

Shortcuts are metadata records that jump over a piece of keyspace.  A shortcut
is a replacement for a series of single-occupancy nodes ascending through the
levels.  Shortcuts exist to save memory and to speed up traversal.
(捷径是跨越一段键空间的元数据记录。捷径是对逐层上升的一系列单占用节点的
替代。捷径的存在是为了节省内存并加速遍历。)

It is possible for the root of the tree to be a shortcut - say, for example,
the tree contains at least 17 nodes all with key prefix ``1111``.  The
insertion algorithm will insert a shortcut to skip over the ``1111`` keyspace
in a single bound and get to the fourth level where these actually become
different.
(树的根也可能是一个捷径——举例来说，树中至少有 17 个节点的键前缀都是
``1111``。插入算法会插入一个捷径，一步跳过 ``1111`` 键空间，直达第四层——
在那里它们才真正开始变得不同。)

Splitting And Collapsing Nodes(节点的分裂与合并)
------------------------------------------------

Each node has a maximum capacity of 16 leaves and metadata pointers.  If the
insertion algorithm finds that it is trying to insert a 17th object into a
node, that node will be split such that at least two leaves that have a common
key segment at that level end up in a separate node rooted on that slot for
that common key segment.
(每个节点的最大容量是 16 个叶子与元数据指针。如果插入算法发现自己要向一
个节点插入第 17 个对象，该节点将被分裂：在该层拥有共同键段的至少两个叶子
最终进入一个单独的节点，该节点扎根于该共同键段对应的那个槽。)

If the leaves in a full node and the leaf that is being inserted are
sufficiently similar, then a shortcut will be inserted into the tree.
(如果已满节点中的叶子与正在插入的叶子足够相似，则会向树中插入一个捷径。)

When the number of objects in the subtree rooted at a node falls to 16 or
fewer, then the subtree will be collapsed down to a single node - and this will
ripple towards the root if possible.
(当以某节点为根的子树中的对象数量降到 16 个或更少时，该子树会被合并成单
个节点——并且如果可能，这一过程会向根的方向持续传播。)

Non-Recursive Iteration(非递归迭代)
-----------------------------------

Each node and shortcut contains a back pointer to its parent and the number of
slot in that parent that points to it.  None-recursive iteration uses these to
proceed rootwards through the tree, going to the parent node, slot N + 1 to
make sure progress is made without the need for a stack.
(每个节点与捷径都包含一个指向其父节点的回指针 (back pointer)，以及在其父
节点中指向自己的槽号。非递归迭代利用这些信息沿树向根前进：走到父节点的
N+1 号槽，从而无需栈也能保证取得进展。)

The backpointers, however, make simultaneous alteration and iteration tricky.
(然而，回指针让同时修改与迭代变得棘手。)

Simultaneous Alteration And Iteration(同时修改与迭代)
-----------------------------------------------------

There are a number of cases to consider:
(需要考虑多种情况：)

1. Simple insert/replace.  This involves simply replacing a NULL or old
   matching leaf pointer with the pointer to the new leaf after a barrier.
   The metadata blocks don't change otherwise.  An old leaf won't be freed
   until after the RCU grace period.
   (简单插入/替换。这只是在屏障之后把一个 NULL 或旧的匹配叶子指针替换为
   指向新叶子的指针。元数据块不做其他改变。旧叶子要等到 RCU 宽限期之后才
   会被释放。)

2. Simple delete.  This involves just clearing an old matching leaf.  The
   metadata blocks don't change otherwise.  The old leaf won't be freed until
   after the RCU grace period.
   (简单删除。这只是清掉一个旧的匹配叶子。元数据块不做其他改变。旧叶子要
   等到 RCU 宽限期之后才会被释放。)

3. Insertion replacing part of a subtree that we haven't yet entered.  This
   may involve replacement of part of that subtree - but that won't affect
   the iteration as we won't have reached the pointer to it yet and the
   ancestry blocks are not replaced (the layout of those does not change).
   (插入替换了我们尚未进入的子树的一部分。这可能涉及该子树一部分的替换
   ——但这不影响迭代，因为我们尚未走到指向它的指针，而且祖先块未被替换
   (它们的布局不变)。)

4. Insertion replacing nodes that we're actively processing.  This isn't a
   problem as we've passed the anchoring pointer and won't switch onto the
   new layout until we follow the back pointers - at which point we've
   already examined the leaves in the replaced node (we iterate over all the
   leaves in a node before following any of its metadata pointers).
   (插入替换了我们正在处理的节点。这不是问题，因为我们已经越过锚定指针，
   在沿回指针回溯之前不会切换到新布局——到那时，我们已检查完被替换节点中
   的所有叶子(我们总是先迭代完一个节点中的全部叶子，再沿其元数据指针前
   进)。)

   We might, however, re-see some leaves that have been split out into a new
   branch that's in a slot further along than we were at.
   (不过，我们可能会再次看到某些被分裂出去、进入一个位置比我们所处槽位更
   靠后的新分支的叶子。)

5. Insertion replacing nodes that we're processing a dependent branch of.
   This won't affect us until we follow the back pointers.  Similar to (4).
   (插入替换了我们正在处理其某个依赖分支的节点。在我们沿回指针回溯之前，
   这不会影响我们。与情况 (4) 类似。)

6. Deletion collapsing a branch under us.  This doesn't affect us because the
   back pointers will get us back to the parent of the new node before we
   could see the new node.  The entire collapsed subtree is thrown away
   unchanged - and will still be rooted on the same slot, so we shouldn't
   process it a second time as we'll go back to slot + 1.
   (删除在我们下方合并了一个分支。这不会影响我们，因为在我们能够看到新节
   点之前，回指针就会把我们带回新节点的父节点。整个被合并的子树被原样抛
   弃——而且仍扎根于同一个槽，所以我们不会把它处理第二次，因为我们会回到
   slot+1。)

.. note::

   Under some circumstances, we need to simultaneously change the parent
   pointer and the parent slot pointer on a node (say, for example, we
   inserted another node before it and moved it up a level).  We cannot do
   this without locking against a read - so we have to replace that node too.
   (在某些情况下，我们需要同时改变一个节点上的父指针与父槽指针(例如，我
   们在它之前插入了另一个节点、使它上升一层)。不与读操作加锁互斥就无法做
   到这一点——因此我们必须同时替换那个节点。)

   However, when we're changing a shortcut into a node this isn't a problem
   as shortcuts only have one slot and so the parent slot number isn't used
   when traversing backwards over one.  This means that it's okay to change
   the slot number first - provided suitable barriers are used to make sure
   the parent slot number is read after the back pointer.
   (不过，当我们把捷径变成节点时，这不是问题，因为捷径只有一个槽，所以在
   反向遍历捷径时不会用到父槽号。这意味着先改槽号是可以的——前提是使用了
   适当的屏障，确保父槽号是在回指针之后被读取的。)

Obsolete blocks and leaves are freed up after an RCU grace period has passed,
so as long as anyone doing walking or iteration holds the RCU read lock, the
old superstructure should not go away on them.
(废弃的块与叶子要在一个 RCU 宽限期过后才会被释放，因此只要做遍历或迭代的
人都持有 RCU 读锁，旧的上层结构就不应从他们脚下消失。)
