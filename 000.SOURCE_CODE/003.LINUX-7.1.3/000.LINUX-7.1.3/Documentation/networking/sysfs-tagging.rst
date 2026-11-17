.. SPDX-License-Identifier: GPL-2.0

=========================
Sysfs tagging(Sysfs 标签)
=========================

(Taken almost verbatim from Eric Biederman's netns tagging patch
commit msg)
(本文几乎逐字摘自 Eric Biederman 的 netns 标签补丁提交说明)

The problem.  Network devices show up in sysfs and with the network
namespace active multiple devices with the same name can show up in
the same directory, ouch!
(问题所在：网络设备会出现在 sysfs 中，而在网络命名空间处于活动状态时，同一个目录下可能出现多个同名设备——糟糕！)

To avoid that problem and allow existing applications in network
namespaces to see the same interface that is currently presented in
sysfs, sysfs now has tagging directory support.
(为了避免这一问题，并让网络命名空间中的现有应用能够看到 sysfs 中当前呈现的同一接口，sysfs 现在提供了标签 (tagging) 目录支持。)

By using the network namespace pointers as tags to separate out
the sysfs directory entries we ensure that we don't have conflicts
in the directories and applications only see a limited set of
the network devices.
(通过使用网络命名空间指针作为标签来区分 sysfs 目录条目，我们确保目录中不会出现冲突，并且应用只能看到网络设备的一个有限子集。)

Each sysfs directory entry may be tagged with a namespace via the
``void *ns member`` of its ``kernfs_node``.  If a directory entry is tagged,
then ``kernfs_node->flags`` will have a flag between KOBJ_NS_TYPE_NONE
and KOBJ_NS_TYPES, and ns will point to the namespace to which it
belongs.
(每个 sysfs 目录条目都可以通过其 ``kernfs_node`` 的 ``void *ns member`` 打上命名空间标签。如果一个目录条目被打上标签，那么 ``kernfs_node->flags`` 会带有一个介于 KOBJ_NS_TYPE_NONE 和 KOBJ_NS_TYPES 之间的标志，并且 ns 将指向它所属的命名空间。)

Each sysfs superblock's kernfs_super_info contains an array
``void *ns[KOBJ_NS_TYPES]``.  When a task in a tagging namespace
kobj_nstype first mounts sysfs, a new superblock is created.  It
will be differentiated from other sysfs mounts by having its
``s_fs_info->ns[kobj_nstype]`` set to the new namespace.  Note that
through bind mounting and mounts propagation, a task can easily view
the contents of other namespaces' sysfs mounts.  Therefore, when a
namespace exits, it will call kobj_ns_exit() to invalidate any
kernfs_node->ns pointers pointing to it.
(每个 sysfs 超级块的 kernfs_super_info 都包含一个数组 ``void *ns[KOBJ_NS_TYPES]`` 。当标签命名空间 kobj_nstype 中的任务首次挂载 sysfs 时，会创建一个新的超级块。它与其他 sysfs 挂载的区别在于其 ``s_fs_info->ns[kobj_nstype]`` 被设置为新的命名空间。注意，通过绑定挂载和挂载传播，任务可以轻松查看其他命名空间的 sysfs 挂载内容。因此，当命名空间退出时，它会调用 kobj_ns_exit() 使所有指向它的 kernfs_node->ns 指针失效。)

Users of this interface:
(该接口的使用者应当：)

- define a type in the ``kobj_ns_type`` enumeration.
  (在 ``kobj_ns_type`` 枚举中定义一个类型。)
- call kobj_ns_type_register() with its ``kobj_ns_type_operations`` which has
  (使用包含以下成员的 ``kobj_ns_type_operations`` 调用 kobj_ns_type_register()：)

  - current_ns() which returns current's namespace
    (current_ns()：返回当前任务的命名空间)
  - netlink_ns() which returns a socket's namespace
    (netlink_ns()：返回套接字的命名空间)
  - initial_ns() which returns the initial namespace
    (initial_ns()：返回初始命名空间)

- call kobj_ns_exit() when an individual tag is no longer valid
  (当某个标签不再有效时调用 kobj_ns_exit())
