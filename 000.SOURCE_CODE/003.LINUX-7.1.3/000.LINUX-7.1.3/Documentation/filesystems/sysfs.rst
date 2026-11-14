.. SPDX-License-Identifier: GPL-2.0

=====================================================
sysfs - _The_ filesystem for exporting kernel objects
=====================================================

(sysfs - 用于导出内核对象的_首要_文件系统)

Patrick Mochel	<mochel@osdl.org>
(作者: Patrick Mochel <mochel@osdl.org>)

Mike Murphy <mamurph@cs.clemson.edu>
(作者: Mike Murphy <mamurph@cs.clemson.edu>)

:Revised:    16 August 2011
(修订: 2011年8月16日)
:Original:   10 January 2003
(原始: 2003年1月10日)


What it is
(它是什么)
~~~~~~~~~~

sysfs is a RAM-based filesystem initially based on ramfs. It provides
a means to export kernel data structures, their attributes, and the
linkages between them to userspace.
(sysfs 是一个基于内存的文件系统，最初基于 ramfs。它提供了一种将内核数据结构、其属性以及它们之间的关联导出到用户空间的方法。)

sysfs is tied inherently to the kobject infrastructure. Please read
Documentation/core-api/kobject.rst for more information concerning the kobject
interface.
(sysfs 与 kobject 基础设施内在地绑定在一起。有关 kobject 接口的更多信息，请阅读 Documentation/core-api/kobject.rst。)


Using sysfs
(使用 sysfs)
~~~~~~~~~~~

sysfs is always compiled in if CONFIG_SYSFS is defined. You can access
it by doing::
(如果定义了 CONFIG_SYSFS，sysfs 总是会被编译进来。你可以通过如下方式访问它：)

    mount -t sysfs sysfs /sys


Directory Creation
(目录创建)
~~~~~~~~~~~~~~~~~~

For every kobject that is registered with the system, a directory is
created for it in sysfs. That directory is created as a subdirectory
of the kobject's parent, expressing internal object hierarchies to
userspace. Top-level directories in sysfs represent the common
ancestors of object hierarchies; i.e. the subsystems the objects
belong to.
(对于向系统注册的每个 kobject，都会在 sysfs 中为它创建一个目录。该目录被创建为 kobject 父对象的子目录，向用户空间表达内部的对象层级关系。sysfs 中的顶层目录代表对象层级的共同祖先，即对象所属的子系统。)

sysfs internally stores a pointer to the kobject that implements a
directory in the kernfs_node object associated with the directory. In
the past this kobject pointer has been used by sysfs to do reference
counting directly on the kobject whenever the file is opened or closed.
With the current sysfs implementation the kobject reference count is
only modified directly by the function sysfs_schedule_callback().
(sysfs 在与目录相关联的 kernfs_node 对象中内部存储一个指向实现该目录的 kobject 的指针。过去，每当文件被打开或关闭时，sysfs 都会使用这个 kobject 指针直接对 kobject 进行引用计数。在当前的 sysfs 实现中，kobject 引用计数仅由函数 sysfs_schedule_callback() 直接修改。)


Attributes
(属性)
~~~~~~~~~~

Attributes can be exported for kobjects in the form of regular files in
the filesystem. sysfs forwards file I/O operations to methods defined
for the attributes, providing a means to read and write kernel
attributes.
(可以以文件系统中普通文件的形式为 kobject 导出属性。sysfs 将文件 I/O 操作转发给为属性定义的方法，从而提供读写内核属性的手段。)

Attributes should be ASCII text files, preferably with only one value
per file. It is noted that it may not be efficient to contain only one
value per file, so it is socially acceptable to express an array of
values of the same type.
(属性应当是 ASCII 文本文件，最好每个文件只包含一个值。需要指出的是，每个文件只包含一个值可能效率不高，因此在惯例上可以接受用一个文件表达同一类型的多个值组成的数组。)

Mixing types, expressing multiple lines of data, and doing fancy
formatting of data is heavily frowned upon. Doing these things may get
you publicly humiliated and your code rewritten without notice.
(混合类型、表达多行数据以及对数据进行花哨的格式化都是极其不受欢迎的。做这些事情可能会让你受到公开的羞辱，并且你的代码会在未经通知的情况下被重写。)


An attribute definition is simply::
(一个属性的定义非常简单：)

    struct attribute {
	    char                    *name;
	    struct module           *owner;
	    umode_t                 mode;
    };


    int sysfs_create_file(struct kobject * kobj, const struct attribute * attr);
    void sysfs_remove_file(struct kobject * kobj, const struct attribute * attr);


A bare attribute contains no means to read or write the value of the
attribute. Subsystems are encouraged to define their own attribute
structure and wrapper functions for adding and removing attributes for
a specific object type.
(一个裸属性本身不包含读取或写入属性值的手段。鼓励子系统为特定对象类型定义自己的属性结构以及用于添加和删除属性的包装函数。)

For example, the driver model defines struct device_attribute like::
(例如，驱动模型定义了 struct device_attribute，如下所示：)

    struct device_attribute {
	    struct attribute	attr;
	    ssize_t (*show)(struct device *dev, struct device_attribute *attr,
			    char *buf);
	    ssize_t (*store)(struct device *dev, struct device_attribute *attr,
			    const char *buf, size_t count);
    };

    int device_create_file(struct device *, const struct device_attribute *);
    void device_remove_file(struct device *, const struct device_attribute *);

It also defines this helper for defining device attributes::
(它还为定义设备属性定义了这个辅助宏：)

    #define DEVICE_ATTR(_name, _mode, _show, _store) \
    struct device_attribute dev_attr_##_name = __ATTR(_name, _mode, _show, _store)

For example, declaring::
(例如，声明：)

    static DEVICE_ATTR(foo, S_IWUSR | S_IRUGO, show_foo, store_foo);

is equivalent to doing::
(等价于：)

    static struct device_attribute dev_attr_foo = {
	    .attr = {
		    .name = "foo",
		    .mode = S_IWUSR | S_IRUGO,
	    },
	    .show = show_foo,
	    .store = store_foo,
    };

Note as stated in include/linux/sysfs.h "OTHER_WRITABLE?  Generally
considered a bad idea." so trying to set a sysfs file writable for
everyone will fail reverting to RO mode for "Others".
(如 include/linux/sysfs.h 中所述 "OTHER_WRITABLE?  Generally considered a bad idea."，因此尝试将 sysfs 文件设置为对所有人可写将会失败，"Others" 会回退到 RO 模式。)

For the common cases sysfs.h provides convenience macros to make
defining attributes easier as well as making code more concise and
readable. The above case could be shortened to:
(对于常见情况，sysfs.h 提供了便利宏，使定义属性更容易，也使代码更简洁易读。上面的例子可以缩短为：)

static struct device_attribute dev_attr_foo = __ATTR_RW(foo);

the list of helpers available to define your wrapper function is:
(可用于定义你的包装函数的辅助宏列表如下：)

__ATTR_RO(name):
		 assumes default name_show and mode 0444
		 (假定默认的 name_show 和模式 0444)
__ATTR_WO(name):
			 assumes a name_store only and is restricted to mode
                 0200 that is root write access only.
			 (仅假定 name_store，并限制为模式 0200，即仅允许 root 写访问。)
__ATTR_RO_MODE(name, mode):
	         for more restrictive RO access; currently
                 only use case is the EFI System Resource Table
                 (see drivers/firmware/efi/esrt.c)
	         (用于更严格的 RO 访问；目前唯一的使用场景是 EFI 系统资源表(参见 drivers/firmware/efi/esrt.c))
__ATTR_RW(name):
	         assumes default name_show, name_store and setting
                 mode to 0644.
	         (假定默认的 name_show、name_store，并将模式设置为 0644。)
__ATTR_NULL:
	         which sets the name to NULL and is used as end of list
                 indicator (see: kernel/workqueue.c)
	         (将 name 设置为 NULL，并用作列表结束指示符(参见: kernel/workqueue.c))

Subsystem-Specific Callbacks
(子系统特定的回调)
~~~~~~~~~~~~~~~~~~~~~~~~~~~~

When a subsystem defines a new attribute type, it must implement a
set of sysfs operations for forwarding read and write calls to the
show and store methods of the attribute owners::
(当子系统定义一个新的属性类型时，它必须实现一组 sysfs 操作，用于将读写调用转发给属性所有者的 show 和 store 方法：)

    struct sysfs_ops {
	    ssize_t (*show)(struct kobject *, struct attribute *, char *);
	    ssize_t (*store)(struct kobject *, struct attribute *, const char *, size_t);
    };

[ Subsystems should have already defined a struct kobj_type as a
descriptor for this type, which is where the sysfs_ops pointer is
stored. See the kobject documentation for more information. ]
([子系统应当已经定义了 struct kobj_type 作为该类型的描述符，sysfs_ops 指针就存储在其中。更多信息请参见 kobject 文档。])

When a file is read or written, sysfs calls the appropriate method
for the type. The method then translates the generic struct kobject
and struct attribute pointers to the appropriate pointer types, and
calls the associated methods.
(当文件被读或写时，sysfs 会为该类型调用适当的方法。然后该方法将通用的 struct kobject 和 struct attribute 指针转换为适当的指针类型，并调用关联的方法。)


To illustrate::
(举例说明：)

    #define to_dev_attr(_attr) container_of(_attr, struct device_attribute, attr)

    static ssize_t dev_attr_show(struct kobject *kobj, struct attribute *attr,
				char *buf)
    {
	    struct device_attribute *dev_attr = to_dev_attr(attr);
	    struct device *dev = kobj_to_dev(kobj);
	    ssize_t ret = -EIO;

	    if (dev_attr->show)
		    ret = dev_attr->show(dev, dev_attr, buf);
	    if (ret >= (ssize_t)PAGE_SIZE) {
		    printk("dev_attr_show: %pS returned bad count\n",
				    dev_attr->show);
	    }
	    return ret;
    }



Reading/Writing Attribute Data
(读/写属性数据)
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

To read or write attributes, show() or store() methods must be
specified when declaring the attribute. The method types should be as
simple as those defined for device attributes::
(要读取或写入属性，必须在声明属性时指定 show() 或 store() 方法。方法类型应当与为设备属性定义的那些一样简单：)

    ssize_t (*show)(struct device *dev, struct device_attribute *attr, char *buf);
    ssize_t (*store)(struct device *dev, struct device_attribute *attr,
		    const char *buf, size_t count);

IOW, they should take only an object, an attribute, and a buffer as parameters.
(换言之，它们应当只接受一个对象、一个属性和一个缓冲区作为参数。)


sysfs allocates a buffer of size (PAGE_SIZE) and passes it to the
method. sysfs will call the method exactly once for each read or
write. This forces the following behavior on the method
implementations:
(sysfs 分配一个大小为 (PAGE_SIZE) 的缓冲区并将其传递给方法。sysfs 对每次读或写恰好调用该方法一次。这迫使方法实现遵循以下行为：)

- On read(2), the show() method should fill the entire buffer.
  Recall that an attribute should only be exporting one value, or an
  array of similar values, so this shouldn't be that expensive.
  (在 read(2) 时，show() 方法应当填满整个缓冲区。回想一下，一个属性只应导出一个值或一组同类值，所以这不应太昂贵。)

  This allows userspace to do partial reads and forward seeks
  arbitrarily over the entire file at will. If userspace seeks back to
  zero or does a pread(2) with an offset of '0' the show() method will
  be called again, rearmed, to fill the buffer.
  (这使得用户空间可以进行部分读取，并可以在整个文件上随意进行向前查找。如果用户空间查找回到零位置，或者以偏移量 '0' 执行 pread(2)，show() 方法将被再次调用(重新就绪)以填充缓冲区。)

- On write(2), sysfs expects the entire buffer to be passed during the
  first write. sysfs then passes the entire buffer to the store() method.
  A terminating null is added after the data on stores. This makes
  functions like sysfs_streq() safe to use.
  (在 write(2) 时，sysfs 期望在第一次写入时传递整个缓冲区。然后 sysfs 将整个缓冲区传递给 store() 方法。写入时会在数据之后添加一个结尾空字符。这使得像 sysfs_streq() 这样的函数可以安全使用。)

  When writing sysfs files, userspace processes should first read the
  entire file, modify the values it wishes to change, then write the
  entire buffer back.
  (在写 sysfs 文件时，用户空间进程应当先读取整个文件，修改它希望更改的值，然后将整个缓冲区写回。)

  Attribute method implementations should operate on an identical
  buffer when reading and writing values.
  (属性方法实现在读取和写入值时应当在相同的缓冲区上进行操作。)

Other notes:
(其他说明：)

- Writing causes the show() method to be rearmed regardless of current
  file position.
  (写入会导致 show() 方法被重新就绪，而与当前文件位置无关。)

- The buffer will always be PAGE_SIZE bytes in length. On x86, this
  is 4096.
  (缓冲区的长度将始终为 PAGE_SIZE 字节。在 x86 上是 4096。)

- show() methods should return the number of bytes printed into the
  buffer.
  (show() 方法应当返回打印到缓冲区中的字节数。)

- New implementations of show() methods should only use sysfs_emit() or
  sysfs_emit_at() when formatting the value to be returned to user space.
  (show() 方法的新实现应当在格式化要返回给用户空间的值时只使用 sysfs_emit() 或 sysfs_emit_at()。)

- store() should return the number of bytes used from the buffer. If the
  entire buffer has been used, just return the count argument.
  (store() 应当返回从缓冲区中使用的字节数。如果整个缓冲区已被使用，只需返回 count 参数。)

- show() or store() can always return errors. If a bad value comes
  through, be sure to return an error.
  (show() 或 store() 总是可以返回错误。如果传入了一个错误的值，务必返回一个错误。)

- The object passed to the methods will be pinned in memory via sysfs
  reference counting its embedded object. However, the physical
  entity (e.g. device) the object represents may not be present. Be
  sure to have a way to check this, if necessary.
  (传递给方法的对象将通过 sysfs 对其内嵌对象的引用计数而固定在内存中。然而，该对象所代表的物理实体(例如设备)可能并不存在。如有必要，务必有办法对此进行检查。)


A very simple (and naive) implementation of a device attribute is::
(一个非常简单(而且天真)的设备属性实现如下：)

    static ssize_t show_name(struct device *dev, struct device_attribute *attr,
			    char *buf)
    {
	    return sysfs_emit(buf, "%s\n", dev->name);
    }

    static ssize_t store_name(struct device *dev, struct device_attribute *attr,
			    const char *buf, size_t count)
    {
	    snprintf(dev->name, sizeof(dev->name), "%.*s",
		    (int)min(count, sizeof(dev->name) - 1), buf);
	    return count;
    }

    static DEVICE_ATTR(name, S_IRUGO, show_name, store_name);


(Note that the real implementation doesn't allow userspace to set the
name for a device.)
((请注意，实际的实现不允许用户空间为设备设置名称。))


Top Level Directory Layout
(顶层目录布局)
~~~~~~~~~~~~~~~~~~~~~~~~~~

The sysfs directory arrangement exposes the relationship of kernel
data structures.
(sysfs 的目录安排展示了内核数据结构之间的关系。)

The top level sysfs directory looks like::
(sysfs 顶层目录如下所示：)

    block/
    bus/
    class/
    dev/
    devices/
    firmware/
    fs/
    hypervisor/
    kernel/
    module/
    power/

devices/ contains a filesystem representation of the device tree. It maps
directly to the internal kernel device tree, which is a hierarchy of
struct device.
(devices/ 包含设备树的文件系统表示。它直接映射到内核内部的设备树，即 struct device 的层级结构。)

bus/ contains flat directory layout of the various bus types in the
kernel. Each bus's directory contains two subdirectories::
(bus/ 包含内核中各种总线类型的扁平目录布局。每个总线的目录包含两个子目录：)

	devices/
	drivers/

devices/ contains symlinks for each device discovered in the system
that point to the device's directory under /sys/devices.
(devices/ 包含系统中发现的每个设备的符号链接，这些链接指向 /sys/devices 下该设备的目录。)

drivers/ contains a directory for each device driver that is loaded
for devices on that particular bus (this assumes that drivers do not
span multiple bus types).
(drivers/ 为在该特定总线上为设备加载的每个设备驱动包含一个目录(这假设驱动程序不会跨越多种总线类型)。)

fs/ contains a directory for some filesystems.  Currently each
filesystem wanting to export attributes must create its own hierarchy
below fs/ (see fuse/fuse.rst for an example).
(fs/ 包含一些文件系统的目录。目前，每个想要导出属性的文件系统都必须在 fs/ 下创建自己的层级结构(示例参见 fuse/fuse.rst)。)

module/ contains parameter values and state information for all
loaded system modules, for both builtin and loadable modules.
(module/ 包含所有已加载系统模块的参数值和状态信息，包括内建模块和可加载模块。)

dev/ contains two directories: char/ and block/. Inside these two
directories there are symlinks named <major>:<minor>.  These symlinks
point to the directories under /sys/devices for each device.  /sys/dev provides a
quick way to lookup the sysfs interface for a device from the result of
a stat(2) operation.
(dev/ 包含两个目录: char/ 和 block/。在这两个目录中有名为 <major>:<minor> 的符号链接。这些符号链接指向 /sys/devices 下每个设备的目录。/sys/dev 提供了一种从 stat(2) 操作的结果快速查找设备 sysfs 接口的方法。)

More information on driver-model specific features can be found in
Documentation/driver-api/driver-model/.
(有关驱动模型特定特性的更多信息，请参见 Documentation/driver-api/driver-model/。)

block/ contains symlinks to all the block devices discovered on the system.
These symlinks point to directories under /sys/devices.
(block/ 包含指向系统上发现的所有块设备的符号链接。这些符号链接指向 /sys/devices 下的目录。)

class/ contains a directory for each device class, grouped by functional type.
Each directory in class/ contains symlinks to devices in the /sys/devices directory.
(class/ 包含每个设备类别的目录，按功能类型分组。class/ 中的每个目录包含指向 /sys/devices 目录中设备的符号链接。)

firmware/ contains system firmware data and configuration such as firmware tables,
ACPI information, and device tree data.
(firmware/ 包含系统固件数据和配置，例如固件表、ACPI 信息和设备树数据。)

hypervisor/ contains virtualization platform information and provides an interface to
the underlying hypervisor.  It is only present when running on a virtual machine.
(hypervisor/ 包含虚拟化平台信息，并提供到底层虚拟机管理程序的接口。它只在虚拟机上运行时才存在。)

kernel/ contains runtime kernel parameters, configuration settings, and status.
(kernel/ 包含运行时内核参数、配置设置和状态。)

power/ contains power management subsystem information including
sleep states, suspend/resume capabilities, and policies.
(power/ 包含电源管理子系统信息，包括睡眠状态、挂起/恢复能力和策略。)


Current Interfaces
(当前接口)
~~~~~~~~~~~~~~~~~~

The following interface layers currently exist in sysfs.
(以下接口层目前存在于 sysfs 中。)


devices (include/linux/device.h)
(devices (include/linux/device.h))
--------------------------------
Structure::
(结构：)

    struct device_attribute {
	    struct attribute	attr;
	    ssize_t (*show)(struct device *dev, struct device_attribute *attr,
			    char *buf);
	    ssize_t (*store)(struct device *dev, struct device_attribute *attr,
			    const char *buf, size_t count);
    };

Declaring::
(声明：)

    DEVICE_ATTR(_name, _mode, _show, _store);

Creation/Removal::
(创建/删除：)

    int device_create_file(struct device *dev, const struct device_attribute * attr);
    void device_remove_file(struct device *dev, const struct device_attribute * attr);


bus drivers (include/linux/device.h)
(总线驱动 (include/linux/device.h))
------------------------------------
Structure::
(结构：)

    struct bus_attribute {
	    struct attribute        attr;
	    ssize_t (*show)(const struct bus_type *, char * buf);
	    ssize_t (*store)(const struct bus_type *, const char * buf, size_t count);
    };

Declaring::
(声明：)

    static BUS_ATTR_RW(name);
    static BUS_ATTR_RO(name);
    static BUS_ATTR_WO(name);

Creation/Removal::
(创建/删除：)

    int bus_create_file(struct bus_type *, struct bus_attribute *);
    void bus_remove_file(struct bus_type *, struct bus_attribute *);


device drivers (include/linux/device.h)
(设备驱动 (include/linux/device.h))
---------------------------------------

Structure::
(结构：)

    struct driver_attribute {
	    struct attribute        attr;
	    ssize_t (*show)(struct device_driver *, char * buf);
	    ssize_t (*store)(struct device_driver *, const char * buf,
			    size_t count);
    };

Declaring::
(声明：)

    DRIVER_ATTR_RO(_name)
    DRIVER_ATTR_RW(_name)

Creation/Removal::
(创建/删除：)

    int driver_create_file(struct device_driver *, const struct driver_attribute *);
    void driver_remove_file(struct device_driver *, const struct driver_attribute *);


Documentation
(文档)
~~~~~~~~~~~~~

The sysfs directory structure and the attributes in each directory define an
ABI between the kernel and user space. As for any ABI, it is important that
this ABI is stable and properly documented. All new sysfs attributes must be
documented in Documentation/ABI. See also Documentation/ABI/README for more
information.
(sysfs 目录结构以及每个目录中的属性定义了内核与用户空间之间的一个 ABI。与任何 ABI 一样，重要的是这个 ABI 要稳定并有适当的文档。所有新的 sysfs 属性必须在 Documentation/ABI 中提供文档。更多信息另请参见 Documentation/ABI/README。)
