.. SPDX-License-Identifier: GPL-2.0

=========================
Min Heap API(最小堆 API)
=========================

:Author: Kuan-Wei Chiu <visitorckw@gmail.com>
(作者： Kuan-Wei Chiu <visitorckw@gmail.com>)

Introduction(引言)
==================

The Min Heap API provides a set of functions and macros for managing min-heaps
in the Linux kernel. A min-heap is a binary tree structure where the value of
each node is less than or equal to the values of its children, ensuring that
the smallest element is always at the root.
(最小堆（Min Heap）API 提供一组用于在 Linux 内核中管理最小堆的函数与宏。最小堆是一种二叉树结构，其中每个节点的值都小于或等于其子节点的值，从而保证最小元素总在根上。)

This document provides a guide to the Min Heap API, detailing how to define and
use min-heaps. Users should not directly call functions with **__min_heap_*()**
prefixes, but should instead use the provided macro wrappers.
(本文档是最小堆 API 的指南，详细介绍如何定义与使用最小堆。用户不应直接调用带 ``__min_heap_*()`` 前缀的函数，而应使用提供的宏封装。)

In addition to the standard version of the functions, the API also includes a
set of inline versions for performance-critical scenarios. These inline
functions have the same names as their non-inline counterparts but include an
**_inline** suffix. For example, **__min_heap_init_inline** and its
corresponding macro wrapper **min_heap_init_inline**. The inline versions allow
custom comparison and swap functions to be called directly, rather than through
indirect function calls. This can significantly reduce overhead, especially
when CONFIG_MITIGATION_RETPOLINE is enabled, as indirect function calls become
more expensive. As with the non-inline versions, it is important to use the
macro wrappers for inline functions instead of directly calling the functions
themselves.
(除了函数的标准版本之外，该 API 还为性能敏感场景提供了一组内联版本。这些内联函数与其非内联版本同名，但带有 ``_inline`` 后缀。例如 ``__min_heap_init_inline`` 及其对应的宏封装 ``min_heap_init_inline``。内联版本允许直接调用自定义的比较与交换函数，而不是通过间接函数调用。这可以显著减少开销——尤其是在启用 CONFIG_MITIGATION_RETPOLINE 时，间接函数调用会变得更昂贵。与非内联版本一样，重要的是使用内联函数的宏封装，而不是直接调用函数本身。)

Data Structures(数据结构)
=========================

Min-Heap Definition(最小堆定义)
-------------------------------

The core data structure for representing a min-heap is defined using the
**MIN_HEAP_PREALLOCATED** and **DEFINE_MIN_HEAP** macros. These macros allow
you to define a min-heap with a preallocated buffer or dynamically allocated
memory.
(表示最小堆的核心数据结构使用 ``MIN_HEAP_PREALLOCATED`` 与 ``DEFINE_MIN_HEAP`` 宏定义。这些宏允许你定义带预分配缓冲区或动态分配内存的最小堆。)

Example:
(示例：)

.. code-block:: c

    #define MIN_HEAP_PREALLOCATED(_type, _name, _nr)
    struct _name {
        size_t nr;         /* Number of elements in the heap */
        size_t size;       /* Maximum number of elements that can be held */
        _type *data;    /* Pointer to the heap data */
        _type preallocated[_nr];  /* Static preallocated array */
    }

    #define DEFINE_MIN_HEAP(_type, _name) MIN_HEAP_PREALLOCATED(_type, _name, 0)

A typical heap structure will include a counter for the number of elements
(`nr`), the maximum capacity of the heap (`size`), and a pointer to an array of
elements (`data`). Optionally, you can specify a static array for preallocated
heap storage using **MIN_HEAP_PREALLOCATED**.
(典型的堆结构会包含元素数量的计数器（`nr`）、堆的最大容量（`size`），以及指向元素数组的指针（`data`）。可选地，可以使用 ``MIN_HEAP_PREALLOCATED`` 指定一个静态数组作为预分配的堆存储。)

Min Heap Callbacks(最小堆回调)
------------------------------

The **struct min_heap_callbacks** provides customization options for ordering
elements in the heap and swapping them. It contains two function pointers:
(struct min_heap_callbacks 提供对堆中元素排序与交换行为的定制选项。它包含两个函数指针：)

.. code-block:: c

    struct min_heap_callbacks {
        bool (*less)(const void *lhs, const void *rhs, void *args);
        void (*swp)(void *lhs, void *rhs, void *args);
    };

- **less** is the comparison function used to establish the order of elements.
  (less 是用于确定元素顺序的比较函数。)
- **swp** is a function for swapping elements in the heap. If swp is set to
  NULL, the default swap function will be used, which swaps the elements based on their size
  (swp 是交换堆中元素的函数。如果 swp 设为 NULL，则使用默认交换函数，它按元素大小进行交换。)

Macro Wrappers(宏封装)
======================

The following macro wrappers are provided for interacting with the heap in a
user-friendly manner. Each macro corresponds to a function that operates on the
heap, and they abstract away direct calls to internal functions.
(下面提供了便于使用的宏封装来与堆交互。每个宏对应一个操作堆的函数，它们把对内部函数的直接调用抽象掉。)

Each macro accepts various parameters that are detailed below.
(每个宏接受多个参数，详见下文。)

Heap Initialization(堆初始化)
-----------------------------

.. code-block:: c

    min_heap_init(heap, data, size);

- **heap**: A pointer to the min-heap structure to be initialized.
  (heap：指向要初始化的最小堆结构的指针。)
- **data**: A pointer to the buffer where the heap elements will be stored. If
  `NULL`, the preallocated buffer within the heap structure will be used.
  (data：指向存储堆元素的缓冲区的指针。如果为 `NULL`，则使用堆结构内部的预分配缓冲区。)
- **size**: The maximum number of elements the heap can hold.
  (size：堆可容纳的最大元素数量。)

This macro initializes the heap, setting its initial state. If `data` is
`NULL`, the preallocated memory inside the heap structure will be used for
storage. Otherwise, the user-provided buffer is used. The operation is **O(1)**.
(该宏初始化堆，设置其初始状态。如果 `data` 为 `NULL`，则使用堆结构内部的预分配内存进行存储；否则使用用户提供的缓冲区。该操作的时间复杂度为 **O(1)**。)

**Inline Version:** min_heap_init_inline(heap, data, size)
(内联版本：min_heap_init_inline(heap, data, size))

Accessing the Top Element(访问堆顶元素)
---------------------------------------

.. code-block:: c

    element = min_heap_peek(heap);

- **heap**: A pointer to the min-heap from which to retrieve the smallest
  element.
  (heap：指向要从中取出最小元素的最小堆的指针。)

This macro returns a pointer to the smallest element (the root) of the heap, or
`NULL` if the heap is empty. The operation is **O(1)**.
(该宏返回指向堆中最小元素（根）的指针；如果堆为空则返回 `NULL`。该操作的时间复杂度为 **O(1)**。)

**Inline Version:** min_heap_peek_inline(heap)
(内联版本：min_heap_peek_inline(heap))

Heap Insertion(堆插入)
----------------------

.. code-block:: c

    success = min_heap_push(heap, element, callbacks, args);

- **heap**: A pointer to the min-heap into which the element should be inserted.
  (heap：指向要把元素插入其中的最小堆的指针。)
- **element**: A pointer to the element to be inserted into the heap.
  (element：指向要插入堆的元素的指针。)
- **callbacks**: A pointer to a `struct min_heap_callbacks` providing the
  `less` and `swp` functions.
  (callbacks：指向提供 `less` 与 `swp` 函数的 `struct min_heap_callbacks` 的指针。)
- **args**: Optional arguments passed to the `less` and `swp` functions.
  (args：传递给 `less` 与 `swp` 函数的可选参数。)

This macro inserts an element into the heap. It returns `true` if the insertion
was successful and `false` if the heap is full. The operation is **O(log n)**.
(该宏把一个元素插入堆中。插入成功返回 `true`，堆已满返回 `false`。该操作的时间复杂度为 **O(log n)**。)

**Inline Version:** min_heap_push_inline(heap, element, callbacks, args)
(内联版本：min_heap_push_inline(heap, element, callbacks, args))

Heap Removal(堆删除)
--------------------

.. code-block:: c

    success = min_heap_pop(heap, callbacks, args);

- **heap**: A pointer to the min-heap from which to remove the smallest element.
  (heap：指向要从中移除最小元素的最小堆的指针。)
- **callbacks**: A pointer to a `struct min_heap_callbacks` providing the
  `less` and `swp` functions.
  (callbacks：指向提供 `less` 与 `swp` 函数的 `struct min_heap_callbacks` 的指针。)
- **args**: Optional arguments passed to the `less` and `swp` functions.
  (args：传递给 `less` 与 `swp` 函数的可选参数。)

This macro removes the smallest element (the root) from the heap. It returns
`true` if the element was successfully removed, or `false` if the heap is
empty. The operation is **O(log n)**.
(该宏从堆中移除最小元素（根）。成功移除返回 `true`，堆为空返回 `false`。该操作的时间复杂度为 **O(log n)**。)

**Inline Version:** min_heap_pop_inline(heap, callbacks, args)
(内联版本：min_heap_pop_inline(heap, callbacks, args))

Heap Maintenance(堆维护)
------------------------

You can use the following macros to maintain the heap's structure:
(可以使用以下宏来维护堆的结构：)

.. code-block:: c

    min_heap_sift_down(heap, pos, callbacks, args);

- **heap**: A pointer to the min-heap.
  (heap：指向最小堆的指针。)
- **pos**: The index from which to start sifting down.
  (pos：开始向下筛选的索引。)
- **callbacks**: A pointer to a `struct min_heap_callbacks` providing the
  `less` and `swp` functions.
  (callbacks：指向提供 `less` 与 `swp` 函数的 `struct min_heap_callbacks` 的指针。)
- **args**: Optional arguments passed to the `less` and `swp` functions.
  (args：传递给 `less` 与 `swp` 函数的可选参数。)

This macro restores the heap property by moving the element at the specified
index (`pos`) down the heap until it is in the correct position. The operation
is **O(log n)**.
(该宏通过把指定索引（`pos`）处的元素沿堆向下移动到正确位置来恢复堆性质。该操作的时间复杂度为 **O(log n)**。)

**Inline Version:** min_heap_sift_down_inline(heap, pos, callbacks, args)
(内联版本：min_heap_sift_down_inline(heap, pos, callbacks, args))

.. code-block:: c

    min_heap_sift_up(heap, idx, callbacks, args);

- **heap**: A pointer to the min-heap.
  (heap：指向最小堆的指针。)
- **idx**: The index of the element to sift up.
  (idx：要向上筛选的元素的索引。)
- **callbacks**: A pointer to a `struct min_heap_callbacks` providing the
  `less` and `swp` functions.
  (callbacks：指向提供 `less` 与 `swp` 函数的 `struct min_heap_callbacks` 的指针。)
- **args**: Optional arguments passed to the `less` and `swp` functions.
  (args：传递给 `less` 与 `swp` 函数的可选参数。)

This macro restores the heap property by moving the element at the specified
index (`idx`) up the heap. The operation is **O(log n)**.
(该宏通过把指定索引（`idx`）处的元素沿堆向上移动来恢复堆性质。该操作的时间复杂度为 **O(log n)**。)

**Inline Version:** min_heap_sift_up_inline(heap, idx, callbacks, args)
(内联版本：min_heap_sift_up_inline(heap, idx, callbacks, args))

.. code-block:: c

    min_heapify_all(heap, callbacks, args);

- **heap**: A pointer to the min-heap.
  (heap：指向最小堆的指针。)
- **callbacks**: A pointer to a `struct min_heap_callbacks` providing the
  `less` and `swp` functions.
  (callbacks：指向提供 `less` 与 `swp` 函数的 `struct min_heap_callbacks` 的指针。)
- **args**: Optional arguments passed to the `less` and `swp` functions.
  (args：传递给 `less` 与 `swp` 函数的可选参数。)

This macro ensures that the entire heap satisfies the heap property. It is
called when the heap is built from scratch or after many modifications. The
operation is **O(n)**.
(该宏确保整个堆满足堆性质。在从头构建堆或经过多次修改之后调用。该操作的时间复杂度为 **O(n)**。)

**Inline Version:** min_heapify_all_inline(heap, callbacks, args)
(内联版本：min_heapify_all_inline(heap, callbacks, args))

Removing Specific Elements(删除指定元素)
----------------------------------------

.. code-block:: c

    success = min_heap_del(heap, idx, callbacks, args);

- **heap**: A pointer to the min-heap.
  (heap：指向最小堆的指针。)
- **idx**: The index of the element to delete.
  (idx：要删除的元素的索引。)
- **callbacks**: A pointer to a `struct min_heap_callbacks` providing the
  `less` and `swp` functions.
  (callbacks：指向提供 `less` 与 `swp` 函数的 `struct min_heap_callbacks` 的指针。)
- **args**: Optional arguments passed to the `less` and `swp` functions.
  (args：传递给 `less` 与 `swp` 函数的可选参数。)

This macro removes an element at the specified index (`idx`) from the heap and
restores the heap property. The operation is **O(log n)**.
(该宏从堆中移除指定索引（`idx`）处的元素并恢复堆性质。该操作的时间复杂度为 **O(log n)**。)

**Inline Version:** min_heap_del_inline(heap, idx, callbacks, args)
(内联版本：min_heap_del_inline(heap, idx, callbacks, args))

Other Utilities(其他实用工具)
=============================

- **min_heap_full(heap)**: Checks whether the heap is full.
  Complexity: **O(1)**.
  (检查堆是否已满。复杂度：**O(1)**。)

.. code-block:: c

    bool full = min_heap_full(heap);

- `heap`: A pointer to the min-heap to check.
  (heap：指向要检查的最小堆的指针。)

This macro returns `true` if the heap is full, otherwise `false`.
(如果堆已满，该宏返回 `true`，否则返回 `false`。)

**Inline Version:** min_heap_full_inline(heap)
(内联版本：min_heap_full_inline(heap))

- **min_heap_empty(heap)**: Checks whether the heap is empty.
  Complexity: **O(1)**.
  (检查堆是否为空。复杂度：**O(1)**。)

.. code-block:: c

    bool empty = min_heap_empty(heap);

- `heap`: A pointer to the min-heap to check.
  (heap：指向要检查的最小堆的指针。)

This macro returns `true` if the heap is empty, otherwise `false`.
(如果堆为空，该宏返回 `true`，否则返回 `false`。)

**Inline Version:** min_heap_empty_inline(heap)
(内联版本：min_heap_empty_inline(heap))

Example Usage(用法示例)
=======================

An example usage of the min-heap API would involve defining a heap structure,
initializing it, and inserting and removing elements as needed.
(最小堆 API 的用法示例包括：定义堆结构、初始化它，并按需插入与移除元素。)

.. code-block:: c

    #include <linux/min_heap.h>

    int my_less_function(const void *lhs, const void *rhs, void *args) {
        return (*(int *)lhs < *(int *)rhs);
    }

    struct min_heap_callbacks heap_cb = {
        .less = my_less_function,    /* Comparison function for heap order */
        .swp  = NULL,                /* Use default swap function */
    };

    void example_usage(void) {
        /* Pre-populate the buffer with elements */
        int buffer[5] = {5, 2, 8, 1, 3};
        /* Declare a min-heap */
        DEFINE_MIN_HEAP(int, my_heap);

        /* Initialize the heap with preallocated buffer and size */
        min_heap_init(&my_heap, buffer, 5);

        /* Build the heap using min_heapify_all */
        my_heap.nr = 5;  /* Set the number of elements in the heap */
        min_heapify_all(&my_heap, &heap_cb, NULL);

        /* Peek at the top element (should be 1 in this case) */
        int *top = min_heap_peek(&my_heap);
        pr_info("Top element: %d\n", *top);

        /* Pop the top element (1) and get the new top (2) */
        min_heap_pop(&my_heap, &heap_cb, NULL);
        top = min_heap_peek(&my_heap);
        pr_info("New top element: %d\n", *top);

        /* Insert a new element (0) and recheck the top */
        int new_element = 0;
        min_heap_push(&my_heap, &new_element, &heap_cb, NULL);
        top = min_heap_peek(&my_heap);
        pr_info("Top element after insertion: %d\n", *top);
    }
