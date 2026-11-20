.. _memory_hotplug:

==========================
Memory hotplug(内存热插拔)
==========================

Memory hotplug event notifier(内存热插拔事件通知器)
===================================================

Hotplugging events are sent to a notification queue.
(热插拔事件会被发送到一个通知队列。)

Memory notifier(内存通知器)
---------------------------

There are six types of notification defined in ``include/linux/memory.h``:
(``include/linux/memory.h`` 中定义了六种通知类型：)

MEM_GOING_ONLINE
  Generated before new memory becomes available in order to be able to
  prepare subsystems to handle memory. The page allocator is still unable
  to allocate from the new memory.
  (在新内存可用之前生成，以便让各子系统能够准备好处理内存。此时页分配器仍不能从新内存分配。)

MEM_CANCEL_ONLINE
  Generated if MEM_GOING_ONLINE fails.
  (在 MEM_GOING_ONLINE 失败时生成。)

MEM_ONLINE
  Generated when memory has successfully brought online. The callback may
  allocate pages from the new memory.
  (在内存成功上线时生成。回调可以从新内存分配页。)

MEM_GOING_OFFLINE
  Generated to begin the process of offlining memory. Allocations are no
  longer possible from the memory but some of the memory to be offlined
  is still in use. The callback can be used to free memory known to a
  subsystem from the indicated memory block.
  (开始内存下线流程时生成。此时已不能再从该内存分配，但待下线的一部分内存仍在使用中。回调可用于从指定的内存块中释放子系统已知的内存。)

MEM_CANCEL_OFFLINE
  Generated if MEM_GOING_OFFLINE fails. Memory is available again from
  the memory block that we attempted to offline.
  (在 MEM_GOING_OFFLINE 失败时生成。我们试图下线的内存块重新变为可用。)

MEM_OFFLINE
  Generated after offlining memory is complete.
  (在内存下线完成后生成。)

A callback routine can be registered by calling::

  hotplug_memory_notifier(callback_func, priority)

(可以通过调用以下函数注册回调例程：)

Callback functions with higher values of priority are called before callback
functions with lower values.
(优先级值较大的回调函数会在优先级值较小的回调函数之前被调用。)

A callback function must have the following prototype::

  int callback_func(
    struct notifier_block *self, unsigned long action, void *arg);

(回调函数必须具有如下原型：)

The first argument of the callback function (self) is a pointer to the block
of the notifier chain that points to the callback function itself.
The second argument (action) is one of the event types described above.
The third argument (arg) passes a pointer of struct memory_notify::

	struct memory_notify {
		unsigned long start_pfn;
		unsigned long nr_pages;
	}

(回调函数的第一个参数 (self) 是指向通知链块的指针，该块指向回调函数自身。第二个参数 (action) 是上文描述的事件类型之一。第三个参数 (arg) 传递指向 struct memory_notify 的指针：)

- start_pfn is start_pfn of online/offline memory.
  (start_pfn 是上线/下线内存的起始 pfn。)
- nr_pages is # of pages of online/offline memory.
  (nr_pages 是上线/下线内存的页数。)

It is possible to get notified for MEM_CANCEL_ONLINE without having been notified
for MEM_GOING_ONLINE, and the same applies to MEM_CANCEL_OFFLINE and
MEM_GOING_OFFLINE.
This can happen when a consumer fails, meaning we break the callchain and we
stop calling the remaining consumers of the notifier.
It is then important that users of memory_notify make no assumptions and get
prepared to handle such cases.
(有可能在未收到 MEM_GOING_ONLINE 通知的情况下收到 MEM_CANCEL_ONLINE 通知，MEM_CANCEL_OFFLINE 与 MEM_GOING_OFFLINE 之间同理。当某个消费者失败时就会出现这种情况：我们打断调用链，不再调用通知器的其余消费者。因此，memory_notify 的使用者不要做任何假设，并准备好处理这类情况，这一点很重要。)

The callback routine shall return one of the values
NOTIFY_DONE, NOTIFY_OK, NOTIFY_BAD, NOTIFY_STOP
defined in ``include/linux/notifier.h``
(回调例程应返回 ``include/linux/notifier.h`` 中定义的 NOTIFY_DONE、NOTIFY_OK、NOTIFY_BAD、NOTIFY_STOP 之一。)

NOTIFY_DONE and NOTIFY_OK have no effect on the further processing.
(NOTIFY_DONE 与 NOTIFY_OK 对后续处理没有影响。)

NOTIFY_BAD is used as response to the MEM_GOING_ONLINE, MEM_GOING_OFFLINE,
MEM_ONLINE, or MEM_OFFLINE action to cancel hotplugging. It stops
further processing of the notification queue.
(NOTIFY_BAD 用作对 MEM_GOING_ONLINE、MEM_GOING_OFFLINE、MEM_ONLINE 或 MEM_OFFLINE 动作的响应以取消热插拔。它会停止通知队列的后续处理。)

NOTIFY_STOP stops further processing of the notification queue.
(NOTIFY_STOP 停止通知队列的后续处理。)

Numa node notifier(Numa 节点通知器)
-----------------------------------

There are six types of notification defined in ``include/linux/node.h``:
(``include/linux/node.h`` 中定义了六种通知类型：)

NODE_ADDING_FIRST_MEMORY
 Generated before memory becomes available to this node for the first time.
 (在内存首次对该节点可用之前生成。)

NODE_CANCEL_ADDING_FIRST_MEMORY
 Generated if NODE_ADDING_FIRST_MEMORY fails.
 (在 NODE_ADDING_FIRST_MEMORY 失败时生成。)

NODE_ADDED_FIRST_MEMORY
 Generated when memory has become available for this node for the first time.
 (在内存首次对该节点可用时生成。)

NODE_REMOVING_LAST_MEMORY
 Generated when the last memory available to this node is about to be offlined.
 (在该节点可用的最后一块内存即将下线时生成。)

NODE_CANCEL_REMOVING_LAST_MEMORY
 Generated when NODE_CANCEL_REMOVING_LAST_MEMORY fails.
 (在 NODE_CANCEL_REMOVING_LAST_MEMORY 失败时生成。)

NODE_REMOVED_LAST_MEMORY
 Generated when the last memory available to this node has been offlined.
 (在该节点可用的最后一块内存已下线时生成。)

A callback routine can be registered by calling::

  hotplug_node_notifier(callback_func, priority)

(可以通过调用以下函数注册回调例程：)

Callback functions with higher values of priority are called before callback
functions with lower values.
(优先级值较大的回调函数会在优先级值较小的回调函数之前被调用。)

A callback function must have the following prototype::

  int callback_func(

    struct notifier_block *self, unsigned long action, void *arg);

(回调函数必须具有如下原型：)

The first argument of the callback function (self) is a pointer to the block
of the notifier chain that points to the callback function itself.
The second argument (action) is one of the event types described above.
The third argument (arg) passes a pointer of struct node_notify::

        struct node_notify {
                int nid;
        }

(回调函数的第一个参数 (self) 是指向通知链块的指针，该块指向回调函数自身。第二个参数 (action) 是上文描述的事件类型之一。第三个参数 (arg) 传递指向 struct node_notify 的指针：)

- nid is the node we are adding or removing memory to.
  (nid 是我们向其添加或从中移除内存的节点。)

It is possible to get notified for NODE_CANCEL_ADDING_FIRST_MEMORY without
having been notified for NODE_ADDING_FIRST_MEMORY, and the same applies to
NODE_CANCEL_REMOVING_LAST_MEMORY and NODE_REMOVING_LAST_MEMORY.
This can happen when a consumer fails, meaning we break the callchain and we
stop calling the remaining consumers of the notifier.
It is then important that users of node_notify make no assumptions and get
prepared to handle such cases.
(有可能在未收到 NODE_ADDING_FIRST_MEMORY 通知的情况下收到 NODE_CANCEL_ADDING_FIRST_MEMORY 通知，NODE_CANCEL_REMOVING_LAST_MEMORY 与 NODE_REMOVING_LAST_MEMORY 之间同理。当某个消费者失败时就会出现这种情况：我们打断调用链，不再调用通知器的其余消费者。因此，node_notify 的使用者不要做任何假设，并准备好处理这类情况，这一点很重要。)

The callback routine shall return one of the values
NOTIFY_DONE, NOTIFY_OK, NOTIFY_BAD, NOTIFY_STOP
defined in ``include/linux/notifier.h``
(回调例程应返回 ``include/linux/notifier.h`` 中定义的 NOTIFY_DONE、NOTIFY_OK、NOTIFY_BAD、NOTIFY_STOP 之一。)

NOTIFY_DONE and NOTIFY_OK have no effect on the further processing.
(NOTIFY_DONE 与 NOTIFY_OK 对后续处理没有影响。)

NOTIFY_BAD is used as response to the NODE_ADDING_FIRST_MEMORY,
NODE_REMOVING_LAST_MEMORY, NODE_ADDED_FIRST_MEMORY or
NODE_REMOVED_LAST_MEMORY action to cancel hotplugging.
It stops further processing of the notification queue.
(NOTIFY_BAD 用作对 NODE_ADDING_FIRST_MEMORY、NODE_REMOVING_LAST_MEMORY、NODE_ADDED_FIRST_MEMORY 或 NODE_REMOVED_LAST_MEMORY 动作的响应以取消热插拔。它会停止通知队列的后续处理。)

NOTIFY_STOP stops further processing of the notification queue.
(NOTIFY_STOP 停止通知队列的后续处理。)

Please note that we should not fail for NODE_ADDED_FIRST_MEMORY /
NODE_REMOVED_FIRST_MEMORY, as memory_hotplug code cannot rollback at that
point anymore.
(请注意，对 NODE_ADDED_FIRST_MEMORY / NODE_REMOVED_FIRST_MEMORY 不应返回失败，因为此时 memory_hotplug 代码已无法回滚。)

Locking Internals(加锁内部实现)
===============================

When adding/removing memory that uses memory block devices (i.e. ordinary RAM),
the device_hotplug_lock should be held to:
(添加/移除使用内存块设备(即普通 RAM)的内存时，应持有 device_hotplug_lock，以便：)

- synchronize against online/offline requests (e.g. via sysfs). This way, memory
  block devices can only be accessed (.online/.state attributes) by user
  space once memory has been fully added. And when removing memory, we
  know nobody is in critical sections.
  (与上线/下线请求(例如通过 sysfs)同步。这样，只有当内存被完全添加后，用户空间才能访问内存块设备(.online/.state 属性)。而在移除内存时，我们能确信没有人在临界区内。)
- synchronize against CPU hotplug and similar (e.g. relevant for ACPI and PPC)
  (与 CPU 热插拔及类似机制同步(例如对 ACPI 和 PPC 相关)。)

Especially, there is a possible lock inversion that is avoided using
device_hotplug_lock when adding memory and user space tries to online that
memory faster than expected:
(特别是，当添加内存且用户空间试图以超出预期的速度上线该内存时，存在一种可能的锁反转，可借助 device_hotplug_lock 避免：)

- device_online() will first take the device_lock(), followed by
  mem_hotplug_lock
  (device_online() 会先获取 device_lock()，再获取 mem_hotplug_lock)
- add_memory_resource() will first take the mem_hotplug_lock, followed by
  the device_lock() (while creating the devices, during bus_add_device()).
  (add_memory_resource() 会先获取 mem_hotplug_lock，再获取 device_lock()(在创建设备期间，即 bus_add_device() 中)。)

As the device is visible to user space before taking the device_lock(), this
can result in a lock inversion.
(由于设备在获取 device_lock() 之前就对用户空间可见，这可能导致锁反转。)

onlining/offlining of memory should be done via device_online()/
device_offline() - to make sure it is properly synchronized to actions
via sysfs. Holding device_hotplug_lock is advised (to e.g. protect online_type)
(内存的上线/下线应通过 device_online()/device_offline() 完成——以确保与通过 sysfs 的操作正确同步。建议持有 device_hotplug_lock(例如用于保护 online_type)。)

When adding/removing/onlining/offlining memory or adding/removing
heterogeneous/device memory, we should always hold the mem_hotplug_lock in
write mode to serialise memory hotplug (e.g. access to global/zone
variables).
(在添加/移除/上线/下线内存或添加/移除异构/设备内存时，我们应始终以写模式持有 mem_hotplug_lock，以串行化内存热插拔(例如对全局/zone 变量的访问)。)

In addition, mem_hotplug_lock (in contrast to device_hotplug_lock) in read
mode allows for a quite efficient get_online_mems/put_online_mems
implementation, so code accessing memory can protect from that memory
vanishing.
(此外，mem_hotplug_lock(与 device_hotplug_lock 不同)在读模式下可以实现相当高效的 get_online_mems/put_online_mems 机制，访问内存的代码借此可以防止所访问的内存消失。)
