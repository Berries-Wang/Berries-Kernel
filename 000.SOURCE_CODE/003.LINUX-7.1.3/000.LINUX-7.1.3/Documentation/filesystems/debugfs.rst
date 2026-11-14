.. SPDX-License-Identifier: GPL-2.0
.. include:: <isonum.txt>

=======
DebugFS
=======

(DebugFS)

Copyright |copy| 2009 Jonathan Corbet <corbet@lwn.net>
(版权所有 |copy| 2009 Jonathan Corbet <corbet@lwn.net>)

Debugfs exists as a simple way for kernel developers to make information
available to user space.  Unlike /proc, which is only meant for information
about a process, or sysfs, which has strict one-value-per-file rules,
debugfs has no rules at all.  Developers can put any information they want
there.  The debugfs filesystem is also intended to not serve as a stable
ABI to user space; in theory, there are no stability constraints placed on
files exported there.  The real world is not always so simple, though [1]_;
even debugfs interfaces are best designed with the idea that they will need
to be maintained forever.
(Debugfs 作为内核开发人员向用户空间提供信息的一种简单方式而存在。与只用于进程信息的 /proc 不同，也与具有严格的每文件一个值规则的 sysfs 不同，debugfs 完全没有任何规则。开发人员可以把他们想要的任何信息放在那里。debugfs 文件系统也无意作为面向用户空间的稳定 ABI；理论上，对在那里导出的文件没有任何稳定性约束。不过，现实世界并不总是这么简单 [1]_；即使是 debugfs 接口，最好也本着它们将需要被永久维护的理念来设计。)

Debugfs is typically mounted with a command like::
(Debugfs 通常使用类似下面的命令挂载：)

    mount -t debugfs none /sys/kernel/debug

(Or an equivalent /etc/fstab line).
The debugfs root directory is accessible only to the root user by
default. To change access to the tree the "uid", "gid" and "mode" mount
options can be used.
((或者 /etc/fstab 中的一行等价配置)。默认情况下，debugfs 根目录只有 root 用户才能访问。要更改对该树的访问，可以使用 "uid"、"gid" 和 "mode" 挂载选项。)

Note that the debugfs API is exported GPL-only to modules.
(注意，debugfs API 仅以 GPL 方式导出给模块。)

Code using debugfs should include <linux/debugfs.h>.  Then, the first order
of business will be to create at least one directory to hold a set of
debugfs files::
(使用 debugfs 的代码应包含 <linux/debugfs.h>。然后，第一件事就是创建至少一个目录来容纳一组 debugfs 文件：)

    struct dentry *debugfs_create_dir(const char *name, struct dentry *parent);

This call, if successful, will make a directory called name underneath the
indicated parent directory.  If parent is NULL, the directory will be
created in the debugfs root.  On success, the return value is a struct
dentry pointer which can be used to create files in the directory (and to
clean it up at the end).  An ERR_PTR(-ERROR) return value indicates that
something went wrong.  If ERR_PTR(-ENODEV) is returned, that is an
indication that the kernel has been built without debugfs support and none
of the functions described below will work.
(如果此调用成功，将在指定的父目录下创建一个名为 name 的目录。如果 parent 为 NULL，则该目录将在 debugfs 根目录中创建。成功时，返回值是一个 struct dentry 指针，可用于在该目录中创建文件(并在结束时清理它)。ERR_PTR(-ERROR) 返回值表示出了问题。如果返回 ERR_PTR(-ENODEV)，则表明内核构建时没有包含 debugfs 支持，下面描述的所有函数都将无法工作。)

The most general way to create a file within a debugfs directory is with::
(在 debugfs 目录中创建文件的最通用方式是使用：)

    struct dentry *debugfs_create_file(const char *name, umode_t mode,
				       struct dentry *parent, void *data,
				       const struct file_operations *fops);

Here, name is the name of the file to create, mode describes the access
permissions the file should have, parent indicates the directory which
should hold the file, data will be stored in the i_private field of the
resulting inode structure, and fops is a set of file operations which
implement the file's behavior.  At a minimum, the read() and/or write()
operations should be provided; others can be included as needed.  Again,
the return value will be a dentry pointer to the created file,
ERR_PTR(-ERROR) on error, or ERR_PTR(-ENODEV) if debugfs support is
missing.
(这里，name 是要创建的文件的名称，mode 描述该文件应具有的访问权限，parent 指示应容纳该文件的目录，data 将被存储在生成的 inode 结构的 i_private 字段中，而 fops 是实现该文件行为的一组文件操作。至少应提供 read() 和/或 write() 操作；其他操作可以根据需要包含。同样，返回值将是指向所创建文件的 dentry 指针，出错时为 ERR_PTR(-ERROR)，如果缺少 debugfs 支持则为 ERR_PTR(-ENODEV)。)

Create a file with an initial size, the following function can be used
instead::
(要创建一个具有初始大小的文件，可以改用以下函数：)

    void debugfs_create_file_size(const char *name, umode_t mode,
				  struct dentry *parent, void *data,
				  const struct file_operations *fops,
				  loff_t file_size);

file_size is the initial file size. The other parameters are the same
as the function debugfs_create_file.
(file_size 是初始文件大小。其他参数与函数 debugfs_create_file 相同。)

In a number of cases, the creation of a set of file operations is not
actually necessary; the debugfs code provides a number of helper functions
for simple situations.  Files containing a single integer value can be
created with any of::
(在许多情况下，实际上并不需要创建一组文件操作；debugfs 代码为简单情形提供了许多辅助函数。包含单个整数值的文件可以用以下任意一个函数创建：)

    void debugfs_create_u8(const char *name, umode_t mode,
			   struct dentry *parent, u8 *value);
    void debugfs_create_u16(const char *name, umode_t mode,
			    struct dentry *parent, u16 *value);
    void debugfs_create_u32(const char *name, umode_t mode,
			    struct dentry *parent, u32 *value);
    void debugfs_create_u64(const char *name, umode_t mode,
			    struct dentry *parent, u64 *value);

These files support both reading and writing the given value; if a specific
file should not be written to, simply set the mode bits accordingly.  The
values in these files are in decimal; if hexadecimal is more appropriate,
the following functions can be used instead::
(这些文件同时支持读取和写入给定的值；如果某个特定文件不应被写入，只需相应地设置模式位即可。这些文件中的值是十进制的；如果十六进制更合适，可以改用以下函数：)

    void debugfs_create_x8(const char *name, umode_t mode,
			   struct dentry *parent, u8 *value);
    void debugfs_create_x16(const char *name, umode_t mode,
			    struct dentry *parent, u16 *value);
    void debugfs_create_x32(const char *name, umode_t mode,
			    struct dentry *parent, u32 *value);
    void debugfs_create_x64(const char *name, umode_t mode,
			    struct dentry *parent, u64 *value);

These functions are useful as long as the developer knows the size of the
value to be exported.  Some types can have different widths on different
architectures, though, complicating the situation somewhat.  There are
functions meant to help out in such special cases::
(只要开发人员知道要导出的值的大小，这些函数就很有用。不过，某些类型在不同的体系结构上可能有不同的宽度，这在一定程度上使情况复杂化了。有一些函数旨在帮助处理这类特殊情况：)

    void debugfs_create_size_t(const char *name, umode_t mode,
			       struct dentry *parent, size_t *value);

As might be expected, this function will create a debugfs file to represent
a variable of type size_t.
(正如所料，此函数将创建一个 debugfs 文件来表示一个 size_t 类型的变量。)

Similarly, there are helpers for variables of type unsigned long, in decimal
and hexadecimal::
(类似地，对于 unsigned long 类型的变量，也有十进制和十六进制的辅助函数：)

    struct dentry *debugfs_create_ulong(const char *name, umode_t mode,
						struct dentry *parent,
						unsigned long *value);
    void debugfs_create_xul(const char *name, umode_t mode,
			    struct dentry *parent, unsigned long *value);

Boolean values can be placed in debugfs with::
(布尔值可以用以下方式放入 debugfs：)

    void debugfs_create_bool(const char *name, umode_t mode,
                             struct dentry *parent, bool *value);

A read on the resulting file will yield either Y (for non-zero values) or
N, followed by a newline.  If written to, it will accept either upper- or
lower-case values, or 1 or 0.  Any other input will be silently ignored.
(对生成的文件进行读取将得到 Y(对于非零值)或 N，后跟一个换行符。如果写入，它将接受大写或小写的值，或者 1 或 0。任何其他输入都会被静默忽略。)

Also, atomic_t values can be placed in debugfs with::
(另外，atomic_t 值可以用以下方式放入 debugfs：)

    void debugfs_create_atomic_t(const char *name, umode_t mode,
				 struct dentry *parent, atomic_t *value)

A read of this file will get atomic_t values, and a write of this file
will set atomic_t values.
(对此文件的读取将获得 atomic_t 值，对此文件的写入将设置 atomic_t 值。)

Another option is exporting a block of arbitrary binary data, with
this structure and function::
(另一种选择是导出一块任意的二进制数据，使用以下结构和函数：)

    struct debugfs_blob_wrapper {
	void *data;
	unsigned long size;
    };

    struct dentry *debugfs_create_blob(const char *name, umode_t mode,
				       struct dentry *parent,
				       struct debugfs_blob_wrapper *blob);

A read of this file will return the data pointed to by the
debugfs_blob_wrapper structure.  Some drivers use "blobs" as a simple way
to return several lines of (static) formatted text output.  This function
can be used to export binary information, but there does not appear to be
any code which does so in the mainline.  Note that all files created with
debugfs_create_blob() are read-only.
(对此文件的读取将返回 debugfs_blob_wrapper 结构所指向的数据。一些驱动程序使用 "blob" 作为返回多行(静态)格式化文本输出的简单方式。此函数可用于导出二进制信息，但主线中似乎没有任何代码这样做。注意，用 debugfs_create_blob() 创建的所有文件都是只读的。)

If you want to dump a block of registers (something that happens quite
often during development, even if little such code reaches mainline),
debugfs offers two functions: one to make a registers-only file, and
another to insert a register block in the middle of another sequential
file::
(如果你想转储一块寄存器(这在开发过程中相当常见，即使很少有此类代码进入主线)，debugfs 提供了两个函数：一个用于创建仅包含寄存器的文件，另一个用于在另一个顺序文件的中间插入一个寄存器块：)

    struct debugfs_reg32 {
	char *name;
	unsigned long offset;
    };

    struct debugfs_regset32 {
	const struct debugfs_reg32 *regs;
	int nregs;
	void __iomem *base;
	struct device *dev;     /* Optional device for Runtime PM */
    };

    debugfs_create_regset32(const char *name, umode_t mode,
			    struct dentry *parent,
			    struct debugfs_regset32 *regset);

    void debugfs_print_regs32(struct seq_file *s, const struct debugfs_reg32 *regs,
			 int nregs, void __iomem *base, char *prefix);

The "base" argument may be 0, but you may want to build the reg32 array
using __stringify, and a number of register names (macros) are actually
byte offsets over a base for the register block.
("base" 参数可以是 0，但你可能想使用 __stringify 来构建 reg32 数组，而且许多寄存器名称(宏)实际上是基于寄存器块基址的字节偏移。)

If you want to dump a u32 array in debugfs, you can create a file with::
(如果你想在 debugfs 中转储一个 u32 数组，你可以用以下方式创建一个文件：)

    struct debugfs_u32_array {
	u32 *array;
	u32 n_elements;
    };

    void debugfs_create_u32_array(const char *name, umode_t mode,
			struct dentry *parent,
			struct debugfs_u32_array *array);

The "array" argument wraps a pointer to the array's data and the number
of its elements. Note: Once array is created its size can not be changed.
("array" 参数封装了一个指向数组数据的指针及其元素数量。注意：一旦数组被创建，其大小就不能再改变。)

There is a helper function to create a device-related seq_file::
(有一个辅助函数可以创建与设备相关的 seq_file：)

   void debugfs_create_devm_seqfile(struct device *dev,
				const char *name,
				struct dentry *parent,
				int (*read_fn)(struct seq_file *s,
					void *data));

The "dev" argument is the device related to this debugfs file, and
the "read_fn" is a function pointer which to be called to print the
seq_file content.
("dev" 参数是与这个 debugfs 文件相关的设备，"read_fn" 是一个函数指针，将被调用来打印 seq_file 内容。)

There are a couple of other directory-oriented helper functions::
(还有几个其他面向目录的辅助函数：)

    struct dentry *debugfs_change_name(struct dentry *dentry,
					  const char *fmt, ...);

    struct dentry *debugfs_create_symlink(const char *name,
                                          struct dentry *parent,
				      	  const char *target);

A call to debugfs_change_name() will give a new name to an existing debugfs
file, always in the same directory.  The new_name must not exist prior
to the call; the return value is 0 on success and -E... on failure.
Symbolic links can be created with debugfs_create_symlink().
(调用 debugfs_change_name() 将给一个已存在的 debugfs 文件一个新名称，且总是在同一目录中。该新名称在调用之前必须不存在；返回值成功时为 0，失败时为 -E...。符号链接可以用 debugfs_create_symlink() 创建。)

There is one important thing that all debugfs users must take into account:
there is no automatic cleanup of any directories created in debugfs.  If a
module is unloaded without explicitly removing debugfs entries, the result
will be a lot of stale pointers and no end of highly antisocial behavior.
So all debugfs users - at least those which can be built as modules - must
be prepared to remove all files and directories they create there.  A file
or directory can be removed with::
(有一件重要的事情是所有 debugfs 用户都必须考虑的：在 debugfs 中创建的任何目录都没有自动清理。如果一个模块在没有显式移除 debugfs 条目的情况下被卸载，其结果将是大量失效指针和无休止的极其糟糕的行为。因此，所有 debugfs 用户——至少是那些可以构建为模块的——必须准备好移除他们在那里创建的所有文件和目录。文件或目录可以用以下方式移除：)

    void debugfs_remove(struct dentry *dentry);

The dentry value can be NULL or an error value, in which case nothing will
be removed.  Note that this function will recursively remove all files and
directories underneath it.  Previously, debugfs_remove_recursive() was used
to perform that task, but this function is now just an alias to
debugfs_remove().  debugfs_remove_recursive() should be considered
deprecated.
(dentry 值可以是 NULL 或错误值，在这种情况下将不会移除任何东西。注意，此函数将递归地移除其下方的所有文件和目录。以前，debugfs_remove_recursive() 用于执行该任务，但这个函数现在只是 debugfs_remove() 的别名。debugfs_remove_recursive() 应被视为已弃用。)

.. [1] http://lwn.net/Articles/309298/
