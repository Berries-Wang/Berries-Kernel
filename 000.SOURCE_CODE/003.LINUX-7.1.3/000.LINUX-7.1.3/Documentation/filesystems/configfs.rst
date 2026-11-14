=======================================================
Configfs - Userspace-driven Kernel Object Configuration
=======================================================

(Configfs - 用户空间驱动的内核对象配置)

Joel Becker <joel.becker@oracle.com>
(作者: Joel Becker <joel.becker@oracle.com>)

Updated: 31 March 2005
(更新: 2005年3月31日)

Copyright (c) 2005 Oracle Corporation,
	Joel Becker <joel.becker@oracle.com>
	(版权所有 (c) 2005 Oracle Corporation，Joel Becker <joel.becker@oracle.com>)


What is configfs?
(什么是 configfs？)
=================

configfs is a ram-based filesystem that provides the converse of
sysfs's functionality.  Where sysfs is a filesystem-based view of
kernel objects, configfs is a filesystem-based manager of kernel
objects, or config_items.
(configfs 是一个基于内存的文件系统，提供与 sysfs 功能相反的功能。sysfs 是内核对象的基于文件系统的视图，而 configfs 是内核对象(即 config_item)的基于文件系统的管理器。)

With sysfs, an object is created in kernel (for example, when a device
is discovered) and it is registered with sysfs.  Its attributes then
appear in sysfs, allowing userspace to read the attributes via
readdir(3)/read(2).  It may allow some attributes to be modified via
write(2).  The important point is that the object is created and
destroyed in kernel, the kernel controls the lifecycle of the sysfs
representation, and sysfs is merely a window on all this.
(在 sysfs 中，对象在内核中创建(例如当发现一个设备时)，然后注册到 sysfs。它的属性随后出现在 sysfs 中，允许用户空间通过 readdir(3)/read(2) 读取属性，也可能允许通过 write(2) 修改某些属性。重点在于：对象在内核中创建和销毁，内核控制 sysfs 表示的生命周期，而 sysfs 只是这一切的一个窗口。)

A configfs config_item is created via an explicit userspace operation:
mkdir(2).  It is destroyed via rmdir(2).  The attributes appear at
mkdir(2) time, and can be read or modified via read(2) and write(2).
As with sysfs, readdir(3) queries the list of items and/or attributes.
symlink(2) can be used to group items together.  Unlike sysfs, the
lifetime of the representation is completely driven by userspace.  The
kernel modules backing the items must respond to this.
(configfs 的 config_item 通过显式的用户空间操作创建：mkdir(2)；并通过 rmdir(2) 销毁。属性在 mkdir(2) 时出现，可以通过 read(2) 和 write(2) 读取或修改。与 sysfs 一样，readdir(3) 查询项和/或属性的列表。symlink(2) 可用于将项分组在一起。与 sysfs 不同的是，表示形式的生命周期完全由用户空间驱动。支撑这些项的内核模块必须对此作出响应。)

Both sysfs and configfs can and should exist together on the same
system.  One is not a replacement for the other.
(sysfs 与 configfs 可以而且应该在同一系统上共存。两者互不替代。)

Using configfs
(使用 configfs)
==============

configfs can be compiled as a module or into the kernel.  You can access
it by doing::

	mount -t configfs none /config

(configfs 可以编译为模块或编入内核。可通过如下方式访问它:)

The configfs tree will be empty unless client modules are also loaded.
These are modules that register their item types with configfs as
subsystems.  Once a client subsystem is loaded, it will appear as a
subdirectory (or more than one) under /config.  Like sysfs, the
configfs tree is always there, whether mounted on /config or not.
(除非同时加载了客户模块，否则 configfs 树将是空的。客户模块是把自己的项类型作为子系统注册到 configfs 的模块。一旦客户子系统被加载，它就会在 /config 下显示为一个(或多个)子目录。与 sysfs 一样，configfs 树始终存在，无论是否挂载到 /config。)

An item is created via mkdir(2).  The item's attributes will also
appear at this time.  readdir(3) can determine what the attributes are,
read(2) can query their default values, and write(2) can store new
values.  Don't mix more than one attribute in one attribute file.
(项通过 mkdir(2) 创建。项的属性也会在此时出现。readdir(3) 可以确定有哪些属性，read(2) 可以查询其默认值，write(2) 可以存入新值。不要把多个属性混在同一个属性文件里。)

There are two types of configfs attributes:
(configfs 属性有两种类型：)

* Normal attributes, which similar to sysfs attributes, are small ASCII text
  files, with a maximum size of one page (PAGE_SIZE, 4096 on i386).  Preferably
  only one value per file should be used, and the same caveats from sysfs apply.
  Configfs expects write(2) to store the entire buffer at once.  When writing to
  normal configfs attributes, userspace processes should first read the entire
  file, modify the portions they wish to change, and then write the entire
  buffer back.
  (普通属性：与 sysfs 属性类似，是小的 ASCII 文本文件，最大大小为一页(PAGE_SIZE，在 i386 上为 4096)。最好每个文件只使用一个值，并沿用 sysfs 的相同注意事项。configfs 期望 write(2) 一次性存入整个缓冲区。写入普通 configfs 属性时，用户空间进程应先读取整个文件，修改想要更改的部分，然后把整个缓冲区写回。)

* Binary attributes, which are somewhat similar to sysfs binary attributes,
  but with a few slight changes to semantics.  The PAGE_SIZE limitation does not
  apply, but the whole binary item must fit in single kernel vmalloc'ed buffer.
  The write(2) calls from user space are buffered, and the attributes'
  write_bin_attribute method will be invoked on the final close, therefore it is
  imperative for user-space to check the return code of close(2) in order to
  verify that the operation finished successfully.
  To avoid a malicious user OOMing the kernel, there's a per-binary attribute
  maximum buffer value.
  (二进制属性：与 sysfs 二进制属性有些类似，但语义上有一些细微变化。不适用 PAGE_SIZE 限制，但整个二进制项必须能放进单个内核 vmalloc 分配的缓冲区。来自用户空间的 write(2) 调用会被缓冲，属性的 write_bin_attribute 方法会在最终 close 时被调用，因此用户空间必须检查 close(2) 的返回码，以确认操作成功完成。为防止恶意用户把内核搞到 OOM，每个二进制属性有最大缓冲区上限。)

When an item needs to be destroyed, remove it with rmdir(2).  An
item cannot be destroyed if any other item has a link to it (via
symlink(2)).  Links can be removed via unlink(2).
(当需要销毁一个项时，用 rmdir(2) 移除它。如果任何其他项通过 symlink(2) 链接到它，则该项无法被销毁。链接可以通过 unlink(2) 移除。)

Configuring FakeNBD: an Example
(配置 FakeNBD: 一个示例)
===============================

Imagine there's a Network Block Device (NBD) driver that allows you to
access remote block devices.  Call it FakeNBD.  FakeNBD uses configfs
for its configuration.  Obviously, there will be a nice program that
sysadmins use to configure FakeNBD, but somehow that program has to tell
the driver about it.  Here's where configfs comes in.
(假设有一个网络块设备(NBD)驱动程序，允许你访问远程块设备。称之为 FakeNBD。FakeNBD 使用 configfs 进行配置。显然，会有一个供系统管理员用来配置 FakeNBD 的漂亮程序，但该程序必须以某种方式把配置告诉驱动程序。这就是 configfs 的用武之地。)

When the FakeNBD driver is loaded, it registers itself with configfs.
readdir(3) sees this just fine::

	# ls /config
	fakenbd

(FakeNBD 驱动程序加载后，会向 configfs 注册自己。readdir(3) 能很好地看到这一点:)

A fakenbd connection can be created with mkdir(2).  The name is
arbitrary, but likely the tool will make some use of the name.  Perhaps
it is a uuid or a disk name::

	# mkdir /config/fakenbd/disk1
	# ls /config/fakenbd/disk1
	target device rw

(可以用 mkdir(2) 创建一个 fakenbd 连接。名字是任意的，但工具很可能会以某种方式使用该名字，比如一个 uuid 或磁盘名:)

The target attribute contains the IP address of the server FakeNBD will
connect to.  The device attribute is the device on the server.
Predictably, the rw attribute determines whether the connection is
read-only or read-write::

	# echo 10.0.0.1 > /config/fakenbd/disk1/target
	# echo /dev/sda1 > /config/fakenbd/disk1/device
	# echo 1 > /config/fakenbd/disk1/rw

(target 属性包含 FakeNBD 将要连接的服务器的 IP 地址。device 属性是服务器上的设备。可以预见，rw 属性决定该连接是只读还是读写:)

That's it.  That's all there is.  Now the device is configured, via the
shell no less.
(就是这样。仅此而已。现在设备就配置好了，而且是通过 shell 完成的。)

Coding With configfs
(使用 configfs 编程)
====================

Every object in configfs is a config_item.  A config_item reflects an
object in the subsystem.  It has attributes that match values on that
object.  configfs handles the filesystem representation of that object
and its attributes, allowing the subsystem to ignore all but the
basic show/store interaction.
(configfs 中的每个对象都是一个 config_item。config_item 反映子系统中的一个对象。它拥有与该对象上的值相匹配的属性。configfs 负责该对象及其属性的文件系统表示，使子系统只需关心基本的 show/store 交互。)

Items are created and destroyed inside a config_group.  A group is a
collection of items that share the same attributes and operations.
Items are created by mkdir(2) and removed by rmdir(2), but configfs
handles that.  The group has a set of operations to perform these tasks
(项在 config_group 内创建和销毁。组是共享相同属性和操作的项的集合。项由 mkdir(2) 创建、由 rmdir(2) 移除，但这些由 configfs 处理。组有一组操作来执行这些任务)

A subsystem is the top level of a client module.  During initialization,
the client module registers the subsystem with configfs, the subsystem
appears as a directory at the top of the configfs filesystem.  A
subsystem is also a config_group, and can do everything a config_group
can.
(子系统是客户模块的顶层。初始化期间，客户模块向 configfs 注册子系统，子系统就会作为目录出现在 configfs 文件系统的顶层。子系统也是一个 config_group，能做 config_group 能做的一切。)

struct config_item
==================

::

	struct config_item {
		char                    *ci_name;
		char                    ci_namebuf[UOBJ_NAME_LEN];
		struct kref             ci_kref;
		struct list_head        ci_entry;
		struct config_item      *ci_parent;
		struct config_group     *ci_group;
		struct config_item_type *ci_type;
		struct dentry           *ci_dentry;
	};

	void config_item_init(struct config_item *);
	void config_item_init_type_name(struct config_item *,
					const char *name,
					struct config_item_type *type);
	struct config_item *config_item_get(struct config_item *);
	void config_item_put(struct config_item *);

Generally, struct config_item is embedded in a container structure, a
structure that actually represents what the subsystem is doing.  The
config_item portion of that structure is how the object interacts with
configfs.
(通常，struct config_item 被嵌入在一个容器结构中，即真正代表子系统所做工作的结构。该结构中的 config_item 部分就是对象与 configfs 交互的方式。)

Whether statically defined in a source file or created by a parent
config_group, a config_item must have one of the _init() functions
called on it.  This initializes the reference count and sets up the
appropriate fields.
(无论是在源文件中静态定义，还是由父 config_group 创建，config_item 都必须在其上调用某一个 _init() 函数。这会初始化引用计数并设置相应的字段。)

All users of a config_item should have a reference on it via
config_item_get(), and drop the reference when they are done via
config_item_put().
(config_item 的所有使用者都应通过 config_item_get() 持有对它的引用，并在用完后通过 config_item_put() 放弃该引用。)

By itself, a config_item cannot do much more than appear in configfs.
Usually a subsystem wants the item to display and/or store attributes,
among other things.  For that, it needs a type.
(config_item 本身除了出现在 configfs 中之外，做不了更多事情。通常子系统希望项能够显示和/或存储属性等。为此，它需要一个类型。)

struct config_item_type
=======================

::

	struct configfs_item_operations {
		void (*release)(struct config_item *);
		int (*allow_link)(struct config_item *src,
				  struct config_item *target);
		void (*drop_link)(struct config_item *src,
				 struct config_item *target);
	};

	struct config_item_type {
		struct module                           *ct_owner;
		struct configfs_item_operations         *ct_item_ops;
		struct configfs_group_operations        *ct_group_ops;
		struct configfs_attribute               **ct_attrs;
		struct configfs_bin_attribute		**ct_bin_attrs;
	};

The most basic function of a config_item_type is to define what
operations can be performed on a config_item.  All items that have been
allocated dynamically will need to provide the ct_item_ops->release()
method.  This method is called when the config_item's reference count
reaches zero.
(config_item_type 最基本的功能是定义可以对 config_item 执行哪些操作。所有动态分配的项都需要提供 ct_item_ops->release() 方法。当 config_item 的引用计数降到零时会调用该方法。)

struct configfs_attribute
=========================

::

	struct configfs_attribute {
		char                    *ca_name;
		struct module           *ca_owner;
		umode_t                  ca_mode;
		ssize_t (*show)(struct config_item *, char *);
		ssize_t (*store)(struct config_item *, const char *, size_t);
	};

When a config_item wants an attribute to appear as a file in the item's
configfs directory, it must define a configfs_attribute describing it.
It then adds the attribute to the NULL-terminated array
config_item_type->ct_attrs.  When the item appears in configfs, the
attribute file will appear with the configfs_attribute->ca_name
filename.  configfs_attribute->ca_mode specifies the file permissions.
(当 config_item 希望某个属性以文件的形式出现在该项的 configfs 目录中时，它必须定义一个描述该属性的 configfs_attribute。然后将该属性加入以 NULL 结尾的数组 config_item_type->ct_attrs。当项出现在 configfs 中时，属性文件会以 configfs_attribute->ca_name 作为文件名出现。configfs_attribute->ca_mode 指定文件权限。)

If an attribute is readable and provides a ->show method, that method will
be called whenever userspace asks for a read(2) on the attribute.  If an
attribute is writable and provides a ->store  method, that method will be
called whenever userspace asks for a write(2) on the attribute.
(如果属性可读并提供 ->show 方法，那么每当用户空间请求对该属性进行 read(2) 时都会调用该方法。如果属性可写并提供 ->store 方法，那么每当用户空间请求对该属性进行 write(2) 时都会调用该方法。)

struct configfs_bin_attribute
=============================

::

	struct configfs_bin_attribute {
		struct configfs_attribute	cb_attr;
		void				*cb_private;
		size_t				cb_max_size;
	};

The binary attribute is used when the one needs to use binary blob to
appear as the contents of a file in the item's configfs directory.
To do so add the binary attribute to the NULL-terminated array
config_item_type->ct_bin_attrs, and the item appears in configfs, the
attribute file will appear with the configfs_bin_attribute->cb_attr.ca_name
filename.  configfs_bin_attribute->cb_attr.ca_mode specifies the file
permissions.
The cb_private member is provided for use by the driver, while the
cb_max_size member specifies the maximum amount of vmalloc buffer
to be used.
(二进制属性用于需要以二进制块作为项的 configfs 目录中文件内容的情况。为此，把二进制属性加入以 NULL 结尾的数组 config_item_type->ct_bin_attrs，当项出现在 configfs 中时，属性文件会以 configfs_bin_attribute->cb_attr.ca_name 作为文件名出现。configfs_bin_attribute->cb_attr.ca_mode 指定文件权限。cb_private 成员供驱动程序使用，而 cb_max_size 成员指定要使用的 vmalloc 缓冲区的最大数量。)

If binary attribute is readable and the config_item provides a
ct_item_ops->read_bin_attribute() method, that method will be called
whenever userspace asks for a read(2) on the attribute.  The converse
will happen for write(2). The reads/writes are buffered so only a
single read/write will occur; the attributes' need not concern itself
with it.
(如果二进制属性可读且 config_item 提供了 ct_item_ops->read_bin_attribute() 方法，那么每当用户空间请求对该属性进行 read(2) 时都会调用该方法。write(2) 则相反。读/写是经过缓冲的，因此只会发生单次读/写；属性本身无需为此操心。)

struct config_group
===================

A config_item cannot live in a vacuum.  The only way one can be created
is via mkdir(2) on a config_group.  This will trigger creation of a
child item::

	struct config_group {
		struct config_item		cg_item;
		struct list_head		cg_children;
		struct configfs_subsystem 	*cg_subsys;
		struct list_head		default_groups;
		struct list_head		group_entry;
	};

	void config_group_init(struct config_group *group);
	void config_group_init_type_name(struct config_group *group,
					 const char *name,
					 struct config_item_type *type);

(config_item 不能孤立存在。创建它的唯一途径是在 config_group 上执行 mkdir(2)。这会触发创建一个子项:)


The config_group structure contains a config_item.  Properly configuring
that item means that a group can behave as an item in its own right.
However, it can do more: it can create child items or groups.  This is
accomplished via the group operations specified on the group's
config_item_type::

	struct configfs_group_operations {
		struct config_item *(*make_item)(struct config_group *group,
						 const char *name);
		struct config_group *(*make_group)(struct config_group *group,
						   const char *name);
		void (*disconnect_notify)(struct config_group *group,
					  struct config_item *item);
		void (*drop_item)(struct config_group *group,
				  struct config_item *item);
	};

(config_group 结构包含一个 config_item。正确配置该条目意味着组本身就能作为一个项来运作。此外它还能做得更多：可以创建子项或子组。这是通过在该组的 config_item_type 上指定的组操作完成的:)

A group creates child items by providing the
ct_group_ops->make_item() method.  If provided, this method is called from
mkdir(2) in the group's directory.  The subsystem allocates a new
config_item (or more likely, its container structure), initializes it,
and returns it to configfs.  Configfs will then populate the filesystem
tree to reflect the new item.
(组通过提供 ct_group_ops->make_item() 方法来创建子项。如果提供了该方法，当在组的目录中执行 mkdir(2) 时就会调用它。子系统分配一个新的 config_item(更可能是其容器结构)，初始化它，并把它返回给 configfs。随后 configfs 会填充文件系统树以反映新项。)

If the subsystem wants the child to be a group itself, the subsystem
provides ct_group_ops->make_group().  Everything else behaves the same,
using the group _init() functions on the group.
(如果子系统希望子项本身是一个组，子系统就提供 ct_group_ops->make_group()。其余行为完全相同，对组使用组的 _init() 函数。)

Finally, when userspace calls rmdir(2) on the item or group,
ct_group_ops->drop_item() is called.  As a config_group is also a
config_item, it is not necessary for a separate drop_group() method.
The subsystem must config_item_put() the reference that was initialized
upon item allocation.  If a subsystem has no work to do, it may omit
the ct_group_ops->drop_item() method, and configfs will call
config_item_put() on the item on behalf of the subsystem.
(最后，当用户空间对项或组调用 rmdir(2) 时，会调用 ct_group_ops->drop_item()。由于 config_group 也是 config_item，因此不需要单独的 drop_group() 方法。子系统必须对项分配时初始化的引用调用 config_item_put()。如果子系统没有需要做的工作，可以省略 ct_group_ops->drop_item() 方法，configfs 会代表子系统对该项调用 config_item_put()。)

Important:
   (重要:)
   drop_item() is void, and as such cannot fail.  When rmdir(2)
   is called, configfs WILL remove the item from the filesystem tree
   (assuming that it has no children to keep it busy).  The subsystem is
   responsible for responding to this.  If the subsystem has references to
   the item in other threads, the memory is safe.  It may take some time
   for the item to actually disappear from the subsystem's usage.  But it
   is gone from configfs.
   (drop_item() 返回 void，因此不可能失败。当 rmdir(2) 被调用时，configfs 将把该项从文件系统树中移除(假设它没有子项让它忙个不停)。子系统负责对此作出响应。如果子系统在其他线程中持有对该项的引用，内存是安全的。该项从子系统的使用中真正消失可能需要一些时间，但它已从 configfs 中消失。)

When drop_item() is called, the item's linkage has already been torn
down.  It no longer has a reference on its parent and has no place in
the item hierarchy.  If a client needs to do some cleanup before this
teardown happens, the subsystem can implement the
ct_group_ops->disconnect_notify() method.  The method is called after
configfs has removed the item from the filesystem view but before the
item is removed from its parent group.  Like drop_item(),
disconnect_notify() is void and cannot fail.  Client subsystems should
not drop any references here, as they still must do it in drop_item().
(当 drop_item() 被调用时，项的链接已经被拆除。它不再持有对父项的引用，在项层次结构中也没有位置。如果客户需要在这种拆除发生之前做一些清理，子系统可以实现 ct_group_ops->disconnect_notify() 方法。该方法在 configfs 已将项从文件系统视图中移除之后、但在项从其父组中移除之前被调用。与 drop_item() 一样，disconnect_notify() 返回 void 且不可能失败。客户子系统不应在此处放弃任何引用，因为它们仍必须在 drop_item() 中完成。)

A config_group cannot be removed while it still has child items.  This
is implemented in the configfs rmdir(2) code.  ->drop_item() will not be
called, as the item has not been dropped.  rmdir(2) will fail, as the
directory is not empty.
(当组仍有子项时，config_group 不能被移除。这在 configfs 的 rmdir(2) 代码中实现。->drop_item() 不会被调用，因为项尚未被丢弃。由于目录非空，rmdir(2) 会失败。)

struct configfs_subsystem
=========================

A subsystem must register itself, usually at module_init time.  This
tells configfs to make the subsystem appear in the file tree::

	struct configfs_subsystem {
		struct config_group	su_group;
		struct mutex		su_mutex;
	};

	int configfs_register_subsystem(struct configfs_subsystem *subsys);
	void configfs_unregister_subsystem(struct configfs_subsystem *subsys);

(子系统必须注册自身，通常在 module_init 时期。这告诉 configfs 让该子系统出现在文件树中:)

A subsystem consists of a toplevel config_group and a mutex.
The group is where child config_items are created.  For a subsystem,
this group is usually defined statically.  Before calling
configfs_register_subsystem(), the subsystem must have initialized the
group via the usual group _init() functions, and it must also have
initialized the mutex.
(子系统由一个顶层 config_group 和一个互斥锁组成。组是创建子 config_items 的地方。对于子系统而言，这个组通常静态定义。在调用 configfs_register_subsystem() 之前，子系统必须已通过通常的组 _init() 函数初始化了该组，并且还必须初始化了互斥锁。)

When the register call returns, the subsystem is live, and it
will be visible via configfs.  At that point, mkdir(2) can be called and
the subsystem must be ready for it.
(当注册调用返回时，子系统即已生效，并可通过 configfs 看到。此时就可以调用 mkdir(2)，子系统必须为此做好准备。)

An Example
(一个示例)
==========

The best example of these basic concepts is the simple_children
subsystem/group and the simple_child item in
samples/configfs/configfs_sample.c. It shows a trivial object displaying
and storing an attribute, and a simple group creating and destroying
these children.
(这些基本概念的最佳示例是 samples/configfs/configfs_sample.c 中的 simple_children 子系统/组以及 simple_child 项。它展示了一个显示并存储属性的简单对象，以及一个创建和销毁这些子对象的简单组。)

Hierarchy Navigation and the Subsystem Mutex
(层次导航与子系统互斥锁)
============================================

There is an extra bonus that configfs provides.  The config_groups and
config_items are arranged in a hierarchy due to the fact that they
appear in a filesystem.  A subsystem is NEVER to touch the filesystem
parts, but the subsystem might be interested in this hierarchy.  For
this reason, the hierarchy is mirrored via the config_group->cg_children
and config_item->ci_parent structure members.
(configfs 还提供了一个额外的好处。由于出现在文件系统中，config_groups 和 config_items 被排列成一个层次结构。子系统绝不能触碰文件系统部分，但子系统可能对这个层次结构感兴趣。出于这个原因，该层次结构通过 config_group->cg_children 和 config_item->ci_parent 结构成员镜像出来。)

A subsystem can navigate the cg_children list and the ci_parent pointer
to see the tree created by the subsystem.  This can race with configfs'
management of the hierarchy, so configfs uses the subsystem mutex to
protect modifications.  Whenever a subsystem wants to navigate the
hierarchy, it must do so under the protection of the subsystem
mutex.
(子系统可以遍历 cg_children 列表和 ci_parent 指针来查看由自己创建的树。这可能与 configfs 对层次结构的管理产生竞争，因此 configfs 使用子系统互斥锁来保护修改。每当子系统想要遍历层次结构时，都必须在子系统互斥锁的保护下进行。)

A subsystem will be prevented from acquiring the mutex while a newly
allocated item has not been linked into this hierarchy.   Similarly, it
will not be able to acquire the mutex while a dropping item has not
yet been unlinked.  This means that an item's ci_parent pointer will
never be NULL while the item is in configfs, and that an item will only
be in its parent's cg_children list for the same duration.  This allows
a subsystem to trust ci_parent and cg_children while they hold the
mutex.
(当一个新分配的项尚未链接进此层次结构时，子系统将无法获取互斥锁。类似地，当一个被丢弃的项尚未解除链接时，也无法获取互斥锁。这意味着项在 configfs 中期间其 ci_parent 指针永远不会为 NULL，且项在其父组的 cg_children 列表中停留的时长与此相同。这使得子系统在持有互斥锁期间可以信任 ci_parent 和 cg_children。)

Item Aggregation Via symlink(2)
(通过 symlink(2) 进行项聚合)
===============================

configfs provides a simple group via the group->item parent/child
relationship.  Often, however, a larger environment requires aggregation
outside of the parent/child connection.  This is implemented via
symlink(2).
(configfs 通过组->项的父/子关系提供了一种简单的分组。然而，更大的环境常常需要在父/子连接之外的聚合。这是通过 symlink(2) 实现的。)

A config_item may provide the ct_item_ops->allow_link() and
ct_item_ops->drop_link() methods.  If the ->allow_link() method exists,
symlink(2) may be called with the config_item as the source of the link.
These links are only allowed between configfs config_items.  Any
symlink(2) attempt outside the configfs filesystem will be denied.
(config_item 可以提供 ct_item_ops->allow_link() 和 ct_item_ops->drop_link() 方法。如果存在 ->allow_link() 方法，则可以以该 config_item 作为链接的源来调用 symlink(2)。这些链接只允许在 configfs 的 config_items 之间建立。任何在 configfs 文件系统之外的 symlink(2) 尝试都会被拒绝。)

When symlink(2) is called, the source config_item's ->allow_link()
method is called with itself and a target item.  If the source item
allows linking to target item, it returns 0.  A source item may wish to
reject a link if it only wants links to a certain type of object (say,
in its own subsystem).
(调用 symlink(2) 时，会以源 config_item 自身和一个目标项为参数调用源项的 ->allow_link() 方法。如果源项允许链接到目标项，它返回 0。如果源项只想链接到某种特定类型的对象(比如它自己子系统内的对象)，它可能希望拒绝链接。)

When unlink(2) is called on the symbolic link, the source item is
notified via the ->drop_link() method.  Like the ->drop_item() method,
this is a void function and cannot return failure.  The subsystem is
responsible for responding to the change.
(当对符号链接调用 unlink(2) 时，源项会通过 ->drop_link() 方法得到通知。与 ->drop_item() 方法一样，这是一个返回 void 的函数，不可能返回失败。子系统负责对该变化作出响应。)

A config_item cannot be removed while it links to any other item, nor
can it be removed while an item links to it.  Dangling symlinks are not
allowed in configfs.
(当 config_item 还链接着任何其他项时不能被移除；当有项链接到它时同样不能被移除。configfs 不允许悬空符号链接。)

Automatically Created Subgroups
(自动创建的子组)
===============================

A new config_group may want to have two types of child config_items.
While this could be codified by magic names in ->make_item(), it is much
more explicit to have a method whereby userspace sees this divergence.
(新的 config_group 可能希望拥有两种类型的子 config_items。虽然可以通过 ->make_item() 中的魔法名字来把这一点固化下来，但提供一个让用户空间能看到这种差异的方法要明确得多。)

Rather than have a group where some items behave differently than
others, configfs provides a method whereby one or many subgroups are
automatically created inside the parent at its creation.  Thus,
mkdir("parent") results in "parent", "parent/subgroup1", up through
"parent/subgroupN".  Items of type 1 can now be created in
"parent/subgroup1", and items of type N can be created in
"parent/subgroupN".
(与其让一个组中某些项的行为不同于其他项，configfs 提供了一种方法：在父组创建时自动在其中创建一个或多个子组。这样，mkdir("parent") 会得到 "parent"、"parent/subgroup1"，一直到 "parent/subgroupN"。类型 1 的项现在可以在 "parent/subgroup1" 中创建，而类型 N 的项可以在 "parent/subgroupN" 中创建。)

These automatic subgroups, or default groups, do not preclude other
children of the parent group.  If ct_group_ops->make_group() exists,
other child groups can be created on the parent group directly.
(这些自动子组(即默认组)并不妨碍父组的其他子项。如果存在 ct_group_ops->make_group()，还可以直接在父组上创建其他子组。)

A configfs subsystem specifies default groups by adding them using the
configfs_add_default_group() function to the parent config_group
structure.  Each added group is populated in the configfs tree at the same
time as the parent group.  Similarly, they are removed at the same time
as the parent.  No extra notification is provided.  When a ->drop_item()
method call notifies the subsystem the parent group is going away, it
also means every default group child associated with that parent group.
(configfs 子系统通过使用 configfs_add_default_group() 函数把默认组添加到父 config_group 结构中来指定它们。每个被添加的组与父组在同一时间被填充到 configfs 树中。类似地，它们与父组同时被移除。不提供额外的通知。当 ->drop_item() 方法调用通知子系统父组即将消失时，也意味着与该父组关联的每一个默认组子项。)

As a consequence of this, default groups cannot be removed directly via
rmdir(2).  They also are not considered when rmdir(2) on the parent
group is checking for children.
(因此，默认组不能直接通过 rmdir(2) 移除。在对父组执行 rmdir(2) 检查子项时，也不会把它们考虑在内。)

Dependent Subsystems
(依赖子系统)
====================

Sometimes other drivers depend on particular configfs items.  For
example, ocfs2 mounts depend on a heartbeat region item.  If that
region item is removed with rmdir(2), the ocfs2 mount must BUG or go
readonly.  Not happy.
(有时其他驱动程序依赖特定的 configfs 项。例如，ocfs2 挂载依赖一个心跳区域项。如果该区域项被 rmdir(2) 移除，ocfs2 挂载就必须 BUG 或者转为只读。这可不令人愉快。)

configfs provides two additional API calls: configfs_depend_item() and
configfs_undepend_item().  A client driver can call
configfs_depend_item() on an existing item to tell configfs that it is
depended on.  configfs will then return -EBUSY from rmdir(2) for that
item.  When the item is no longer depended on, the client driver calls
configfs_undepend_item() on it.
(configfs 提供两个额外的 API 调用：configfs_depend_item() 和 configfs_undepend_item()。客户驱动程序可以对一个已存在的项调用 configfs_depend_item()，告诉 configfs 该项正被依赖。此后 configfs 会对该项的 rmdir(2) 返回 -EBUSY。当该项不再被依赖时，客户驱动程序对它调用 configfs_undepend_item()。)

These API cannot be called underneath any configfs callbacks, as
they will conflict.  They can block and allocate.  A client driver
probably shouldn't calling them of its own gumption.  Rather it should
be providing an API that external subsystems call.
(这些 API 不能在任何 configfs 回调内部调用，因为会产生冲突。它们可以阻塞和分配内存。客户驱动程序大概不应该自作主张地调用它们，而应提供由外部子系统调用的 API。)

How does this work?  Imagine the ocfs2 mount process.  When it mounts,
it asks for a heartbeat region item.  This is done via a call into the
heartbeat code.  Inside the heartbeat code, the region item is looked
up.  Here, the heartbeat code calls configfs_depend_item().  If it
succeeds, then heartbeat knows the region is safe to give to ocfs2.
If it fails, it was being torn down anyway, and heartbeat can gracefully
pass up an error.
(这是如何工作的？想象 ocfs2 的挂载过程。挂载时，它请求一个心跳区域项。这是通过一次进入心跳代码的调用完成的。在心跳代码内部，查找该区域项。在这里，心跳代码调用 configfs_depend_item()。如果成功，心跳就知道该区域可以安全地交给 ocfs2。如果失败，它本来就在被拆除，心跳可以优雅地向上传递一个错误。)
