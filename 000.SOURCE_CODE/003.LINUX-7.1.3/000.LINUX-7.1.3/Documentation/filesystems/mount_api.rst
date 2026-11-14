.. SPDX-License-Identifier: GPL-2.0

====================
Filesystem Mount API
====================

(文件系统挂载 API)

.. CONTENTS

 (1) Overview.

 (2) The filesystem context.

 (3) The filesystem context operations.

 (4) Filesystem context security.

 (5) VFS filesystem context API.

 (6) Superblock creation helpers.

 (7) Parameter description.

 (8) Parameter helper functions.


Overview
(概述)
========

The creation of new mounts is now to be done in a multistep process:
(新挂载的创建现在要通过一个多步骤的流程来完成：)

 (1) Create a filesystem context.
     (创建一个文件系统上下文。)

 (2) Parse the parameters and attach them to the context.  Parameters are
     expected to be passed individually from userspace, though legacy binary
     parameters can also be handled.
     (解析参数并把它们附加到上下文上。参数预期由用户空间逐个传递，不过也可以处理旧式的二进制参数。)

 (3) Validate and pre-process the context.
     (验证并预处理上下文。)

 (4) Get or create a superblock and mountable root.
     (获取或创建超级块和可挂载的根。)

 (5) Perform the mount.
     (执行挂载。)

 (6) Return an error message attached to the context.
     (返回附加到上下文的错误消息。)

 (7) Destroy the context.
     (销毁上下文。)

To support this, the file_system_type struct gains two new fields::

	int (*init_fs_context)(struct fs_context *fc);
	const struct fs_parameter_description *parameters;

(为支持这一点，file_system_type 结构体新增了两个字段：)

The first is invoked to set up the filesystem-specific parts of a filesystem
context, including the additional space, and the second points to the
parameter description for validation at registration time and querying by a
future system call.
(第一个用于设置文件系统上下文中文件系统特有的部分，包括额外的空间；第二个指向参数描述，用于在注册时进行验证，并供未来的系统调用查询。)

Note that security initialisation is done *after* the filesystem is called so
that the namespaces may be adjusted first.
(注意，安全初始化是在调用文件系统 *之后* 进行的，以便可以先调整命名空间。)


The Filesystem context
(文件系统上下文)
======================

The creation and reconfiguration of a superblock is governed by a filesystem
context.  This is represented by the fs_context structure::

	struct fs_context {
		const struct fs_context_operations *ops;
		struct file_system_type *fs_type;
		void			*fs_private;
		struct dentry		*root;
		struct user_namespace	*user_ns;
		struct net		*net_ns;
		const struct cred	*cred;
		char			*source;
		char			*subtype;
		void			*security;
		void			*s_fs_info;
		unsigned int		sb_flags;
		unsigned int		sb_flags_mask;
		unsigned int		s_iflags;
		enum fs_context_purpose	purpose:8;
		...
	};

(超级块的创建和重新配置由文件系统上下文管理。这由 fs_context 结构体表示：)

The fs_context fields are as follows:
(fs_context 的字段如下：)

   * ::

       const struct fs_context_operations *ops

     These are operations that can be done on a filesystem context (see
     below).  This must be set by the ->init_fs_context() file_system_type
     operation.
     (这些是可以在文件系统上下文上执行的操作(见下文)。它必须由 ->init_fs_context() file_system_type 操作来设置。)

   * ::

       struct file_system_type *fs_type

     A pointer to the file_system_type of the filesystem that is being
     constructed or reconfigured.  This retains a reference on the type owner.
     (指向正在被构造或重新配置的文件系统的 file_system_type 的指针。它会保留对类型拥有者的引用。)

   * ::

       void *fs_private

     A pointer to the file system's private data.  This is where the filesystem
     will need to store any options it parses.
     (指向文件系统私有数据的指针。文件系统需要在这里存储它解析出的任何选项。)

   * ::

       struct dentry *root

     A pointer to the root of the mountable tree (and indirectly, the
     superblock thereof).  This is filled in by the ->get_tree() op.  If this
     is set, an active reference on root->d_sb must also be held.
     (指向可挂载树根部的指针(并间接指向其超级块)。它由 ->get_tree() 操作填充。如果设置了它，还必须持有对 root->d_sb 的活动引用。)

   * ::

       struct user_namespace *user_ns
       struct net *net_ns

     There are a subset of the namespaces in use by the invoking process.  They
     retain references on each namespace.  The subscribed namespaces may be
     replaced by the filesystem to reflect other sources, such as the parent
     mount superblock on an automount.
     (它们是发起调用的进程正在使用的命名空间的一个子集。它们对每个命名空间保留引用。所订阅的命名空间可以被文件系统替换，以反映其他来源，例如自动挂载时的父挂载超级块。)

   * ::

       const struct cred *cred

     The mounter's credentials.  This retains a reference on the credentials.
     (挂载者的凭据。它会保留对这些凭据的引用。)

   * ::

       char *source

     This specifies the source.  It may be a block device (e.g. /dev/sda1) or
     something more exotic, such as the "host:/path" that NFS desires.
     (这指定了来源。它可以是块设备(例如 /dev/sda1)，也可以是更特别的东西，例如 NFS 所期望的 "host:/path"。)

   * ::

       char *subtype

     This is a string to be added to the type displayed in /proc/mounts to
     qualify it (used by FUSE).  This is available for the filesystem to set if
     desired.
     (这是一个将被添加到 /proc/mounts 所显示的类型之后的字符串，用来限定该类型(FUSE 会用到)。如果需要，文件系统可以设置它。)

   * ::

       void *security

     A place for the LSMs to hang their security data for the superblock.  The
     relevant security operations are described below.
     (供 LSM 为超级块挂靠其安全数据的地方。相关的安全操作在下文中描述。)

   * ::

       void *s_fs_info

     The proposed s_fs_info for a new superblock, set in the superblock by
     sget_fc().  This can be used to distinguish superblocks.
     (为新超级块提议的 s_fs_info，通过 sget_fc() 设置到超级块中。它可以用来区分超级块。)

   * ::

       unsigned int sb_flags
       unsigned int sb_flags_mask

     Which bits SB_* flags are to be set/cleared in super_block::s_flags.
     (指出要在 super_block::s_flags 中设置/清除哪些 SB_* 标志位。)

   * ::

       unsigned int s_iflags

     These will be bitwise-OR'd with s->s_iflags when a superblock is created.
     (创建超级块时，这些位将与 s->s_iflags 按位或。)

   * ::

       enum fs_context_purpose

     This indicates the purpose for which the context is intended.  The
     available values are:
     (这指明上下文的预期用途。可用值为：)

	==========================	======================================
	FS_CONTEXT_FOR_MOUNT,		New superblock for explicit mount (为显式挂载提供新超级块)
	FS_CONTEXT_FOR_SUBMOUNT		New automatic submount of extant mount (为现存挂载创建新的自动子挂载)
	FS_CONTEXT_FOR_RECONFIGURE	Change an existing mount (更改现有的挂载)
	==========================	======================================

The mount context is created by calling vfs_new_fs_context() or
vfs_dup_fs_context() and is destroyed with put_fs_context().  Note that the
structure is not refcounted.
(挂载上下文通过调用 vfs_new_fs_context() 或 vfs_dup_fs_context() 来创建，并通过 put_fs_context() 销毁。注意，该结构体没有引用计数。)

VFS, security and filesystem mount options are set individually with
vfs_parse_mount_option().  Options provided by the old mount(2) system call as
a page of data can be parsed with generic_parse_monolithic().
(VFS、安全和文件系统挂载选项通过 vfs_parse_mount_option() 逐一设置。旧 mount(2) 系统调用以一页数据形式提供的选项可以用 generic_parse_monolithic() 来解析。)

When mounting, the filesystem is allowed to take data from any of the pointers
and attach it to the superblock (or whatever), provided it clears the pointer
in the mount context.
(挂载时，允许文件系统从任意指针获取数据并将其附加到超级块(或其他对象)上，前提是它清除了挂载上下文中的该指针。)

The filesystem is also allowed to allocate resources and pin them with the
mount context.  For instance, NFS might pin the appropriate protocol version
module.
(也允许文件系统分配资源并用挂载上下文将它们固定住。例如，NFS 可能会固定相应的协议版本模块。)


The Filesystem Context Operations
(文件系统上下文操作)
=================================

The filesystem context points to a table of operations::

	struct fs_context_operations {
		void (*free)(struct fs_context *fc);
		int (*dup)(struct fs_context *fc, struct fs_context *src_fc);
		int (*parse_param)(struct fs_context *fc,
				   struct fs_parameter *param);
		int (*parse_monolithic)(struct fs_context *fc, void *data);
		int (*get_tree)(struct fs_context *fc);
		int (*reconfigure)(struct fs_context *fc);
	};

(文件系统上下文指向一个操作表：)

These operations are invoked by the various stages of the mount procedure to
manage the filesystem context.  They are as follows:
(这些操作由挂载过程的各个阶段调用，用来管理文件系统上下文。它们如下：)

   * ::

	void (*free)(struct fs_context *fc);

     Called to clean up the filesystem-specific part of the filesystem context
     when the context is destroyed.  It should be aware that parts of the
     context may have been removed and NULL'd out by ->get_tree().
     (在上下文销毁时调用，用于清理文件系统上下文中文件系统特有的部分。它应当意识到上下文的部分内容可能已被 ->get_tree() 移除并置为 NULL。)

   * ::

	int (*dup)(struct fs_context *fc, struct fs_context *src_fc);

     Called when a filesystem context has been duplicated to duplicate the
     filesystem-private data.  An error may be returned to indicate failure to
     do this.
     (当文件系统上下文被复制时调用，用来复制文件系统私有的数据。可以返回错误以表明未能完成复制。)

     .. Warning::

         Note that even if this fails, put_fs_context() will be called
	 immediately thereafter, so ->dup() *must* make the
	 filesystem-private data safe for ->free().
         (注意，即使此操作失败，随后也会立即调用 put_fs_context()，因此 ->dup() *必须* 让文件系统私有数据对 ->free() 是安全的。)

   * ::

	int (*parse_param)(struct fs_context *fc,
			   struct fs_parameter *param);

     Called when a parameter is being added to the filesystem context.  param
     points to the key name and maybe a value object.  VFS-specific options
     will have been weeded out and fc->sb_flags updated in the context.
     Security options will also have been weeded out and fc->security updated.
     (当向文件系统上下文添加参数时调用。param 指向键名，可能还有一个值对象。VFS 特有的选项已被剔除，上下文中的 fc->sb_flags 已被更新。安全选项也已被剔除，fc->security 已被更新。)

     The parameter can be parsed with fs_parse() and fs_lookup_param().  Note
     that the source(s) are presented as parameters named "source".
     (参数可以用 fs_parse() 和 fs_lookup_param() 来解析。注意，来源被作为名为 "source" 的参数呈现。)

     If successful, 0 should be returned or a negative error code otherwise.
     (成功时应返回 0，否则返回负的错误码。)

   * ::

	int (*parse_monolithic)(struct fs_context *fc, void *data);

     Called when the mount(2) system call is invoked to pass the entire data
     page in one go.  If this is expected to be just a list of "key[=val]"
     items separated by commas, then this may be set to NULL.
     (当调用 mount(2) 系统调用以一次性传入整个数据页时调用。如果预期它只是一个由逗号分隔的 "key[=val]" 项列表，则可以将其设为 NULL。)

     The return value is as for ->parse_param().
     (返回值与 ->parse_param() 相同。)

     If the filesystem (e.g. NFS) needs to examine the data first and then
     finds it's the standard key-val list then it may pass it off to
     generic_parse_monolithic().
     (如果文件系统(例如 NFS)需要先检查数据，然后发现它是标准的键-值列表，则可以将其转交给 generic_parse_monolithic() 处理。)

   * ::

	int (*get_tree)(struct fs_context *fc);

     Called to get or create the mountable root and superblock, using the
     information stored in the filesystem context (reconfiguration goes via a
     different vector).  It may detach any resources it desires from the
     filesystem context and transfer them to the superblock it creates.
     (调用它以使用文件系统上下文中存储的信息来获取或创建可挂载的根和超级块(重新配置走不同的途径)。它可以将自己想要的任何资源从文件系统上下文中分离出来，并转移到它创建的超级块中。)

     On success it should set fc->root to the mountable root and return 0.  In
     the case of an error, it should return a negative error code.
     (成功时，它应将 fc->root 设置为可挂载的根并返回 0。出错时，应返回负的错误码。)

     The phase on a userspace-driven context will be set to only allow this to
     be called once on any particular context.
     (对于用户空间驱动的上下文，其阶段设置将只允许在任何特定上下文上调用它一次。)

   * ::

	int (*reconfigure)(struct fs_context *fc);

     Called to effect reconfiguration of a superblock using information stored
     in the filesystem context.  It may detach any resources it desires from
     the filesystem context and transfer them to the superblock.  The
     superblock can be found from fc->root->d_sb.
     (调用它以使用文件系统上下文中存储的信息来实施超级块的重新配置。它可以将自己想要的任何资源从文件系统上下文中分离出来，并转移到超级块中。超级块可以从 fc->root->d_sb 找到。)

     On success it should return 0.  In the case of an error, it should return
     a negative error code.
     (成功时应返回 0。出错时，应返回负的错误码。)


Filesystem context Security
(文件系统上下文安全)
===========================

The filesystem context contains a security pointer that the LSMs can use for
building up a security context for the superblock to be mounted.  There are a
number of operations used by the new mount code for this purpose:
(文件系统上下文包含一个 security 指针，LSM 可以用它为要挂载的超级块构建安全上下文。新的挂载代码为此使用了许多操作：)

   * ::

	int security_fs_context_alloc(struct fs_context *fc,
				      struct dentry *reference);

     Called to initialise fc->security (which is preset to NULL) and allocate
     any resources needed.  It should return 0 on success or a negative error
     code on failure.
     (调用它来初始化 fc->security(它被预置为 NULL)并分配所需的任何资源。成功时应返回 0，失败时返回负的错误码。)

     reference will be non-NULL if the context is being created for superblock
     reconfiguration (FS_CONTEXT_FOR_RECONFIGURE) in which case it indicates
     the root dentry of the superblock to be reconfigured.  It will also be
     non-NULL in the case of a submount (FS_CONTEXT_FOR_SUBMOUNT) in which case
     it indicates the automount point.
     (如果上下文是为超级块重新配置而创建的(FS_CONTEXT_FOR_RECONFIGURE)，reference 将是非 NULL 的，此时它指示要重新配置的超级块的根 dentry。在子挂载(FS_CONTEXT_FOR_SUBMOUNT)的情况下它也是非 NULL 的，此时它指示自动挂载点。)

   * ::

	int security_fs_context_dup(struct fs_context *fc,
				    struct fs_context *src_fc);

     Called to initialise fc->security (which is preset to NULL) and allocate
     any resources needed.  The original filesystem context is pointed to by
     src_fc and may be used for reference.  It should return 0 on success or a
     negative error code on failure.
     (调用它来初始化 fc->security(它被预置为 NULL)并分配所需的任何资源。原始文件系统上下文由 src_fc 指向，可供参考。成功时应返回 0，失败时返回负的错误码。)

   * ::

	void security_fs_context_free(struct fs_context *fc);

     Called to clean up anything attached to fc->security.  Note that the
     contents may have been transferred to a superblock and the pointer cleared
     during get_tree.
     (调用它来清理附加到 fc->security 上的任何内容。注意，其内容可能已被转移到超级块中，并且该指针在 get_tree 期间已被清除。)

   * ::

	int security_fs_context_parse_param(struct fs_context *fc,
					    struct fs_parameter *param);

     Called for each mount parameter, including the source.  The arguments are
     as for the ->parse_param() method.  It should return 0 to indicate that
     the parameter should be passed on to the filesystem, 1 to indicate that
     the parameter should be discarded or an error to indicate that the
     parameter should be rejected.
     (对每个挂载参数调用，包括来源。参数与 ->parse_param() 方法相同。它应返回 0 表示该参数应继续传递给文件系统，返回 1 表示该参数应被丢弃，或返回错误表示该参数应被拒绝。)

     The value pointed to by param may be modified (if a string) or stolen
     (provided the value pointer is NULL'd out).  If it is stolen, 1 must be
     returned to prevent it being passed to the filesystem.
     (param 所指向的值可以被修改(如果是字符串)或被窃取(只要把该值指针置为 NULL 即可)。如果它被窃取，必须返回 1 以防止它被传递给文件系统。)

   * ::

	int security_fs_context_validate(struct fs_context *fc);

     Called after all the options have been parsed to validate the collection
     as a whole and to do any necessary allocation so that
     security_sb_get_tree() and security_sb_reconfigure() are less likely to
     fail.  It should return 0 or a negative error code.
     (在所有选项都解析完之后调用，用于验证这组选项的整体情况，并进行任何必要的分配，以使 security_sb_get_tree() 和 security_sb_reconfigure() 更不容易失败。它应返回 0 或负的错误码。)

     In the case of reconfiguration, the target superblock will be accessible
     via fc->root.
     (在重新配置的情况下，目标超级块可以通过 fc->root 访问。)

   * ::

	int security_sb_get_tree(struct fs_context *fc);

     Called during the mount procedure to verify that the specified superblock
     is allowed to be mounted and to transfer the security data there.  It
     should return 0 or a negative error code.
     (在挂载过程中调用，用于验证指定的超级块允许被挂载，并把安全数据转移到那里。它应返回 0 或负的错误码。)

   * ::

	void security_sb_reconfigure(struct fs_context *fc);

     Called to apply any reconfiguration to an LSM's context.  It must not
     fail.  Error checking and resource allocation must be done in advance by
     the parameter parsing and validation hooks.
     (调用它来把任何重新配置应用到 LSM 的上下文。它绝不能失败。错误检查和资源分配必须预先由参数解析和验证钩子完成。)

   * ::

	int security_sb_mountpoint(struct fs_context *fc,
			           struct path *mountpoint,
				   unsigned int mnt_flags);

     Called during the mount procedure to verify that the root dentry attached
     to the context is permitted to be attached to the specified mountpoint.
     It should return 0 on success or a negative error code on failure.
     (在挂载过程中调用，用于验证附加到上下文的根 dentry 获准附加到指定的挂载点。成功时应返回 0，失败时返回负的错误码。)


VFS Filesystem context API
(VFS 文件系统上下文 API)
==========================

There are four operations for creating a filesystem context and one for
destroying a context:
(有四个用于创建文件系统上下文的操作和一个用于销毁上下文的操作：)

   * ::

       struct fs_context *fs_context_for_mount(struct file_system_type *fs_type,
					       unsigned int sb_flags);

     Allocate a filesystem context for the purpose of setting up a new mount,
     whether that be with a new superblock or sharing an existing one.  This
     sets the superblock flags, initialises the security and calls
     fs_type->init_fs_context() to initialise the filesystem private data.
     (为建立新挂载的目的分配一个文件系统上下文，无论是使用新的超级块还是共享现有的超级块。这会设置超级块标志，初始化安全部分，并调用 fs_type->init_fs_context() 来初始化文件系统私有数据。)

     fs_type specifies the filesystem type that will manage the context and
     sb_flags presets the superblock flags stored therein.
     (fs_type 指定将要管理该上下文的文件系统类型，sb_flags 预置其中存储的超级块标志。)

   * ::

       struct fs_context *fs_context_for_reconfigure(
		struct dentry *dentry,
		unsigned int sb_flags,
		unsigned int sb_flags_mask);

     Allocate a filesystem context for the purpose of reconfiguring an
     existing superblock.  dentry provides a reference to the superblock to be
     configured.  sb_flags and sb_flags_mask indicate which superblock flags
     need changing and to what.
     (为重新配置现有超级块的目的分配一个文件系统上下文。dentry 提供对要配置的超级块的引用。sb_flags 和 sb_flags_mask 指示哪些超级块标志需要更改以及更改为何种值。)

   * ::

       struct fs_context *fs_context_for_submount(
		struct file_system_type *fs_type,
		struct dentry *reference);

     Allocate a filesystem context for the purpose of creating a new mount for
     an automount point or other derived superblock.  fs_type specifies the
     filesystem type that will manage the context and the reference dentry
     supplies the parameters.  Namespaces are propagated from the reference
     dentry's superblock also.
     (为自动挂载点或其他派生超级块创建新挂载的目的分配一个文件系统上下文。fs_type 指定将要管理该上下文的文件系统类型，引用 dentry 提供参数。命名空间也会从引用 dentry 的超级块传播过来。)

     Note that it's not a requirement that the reference dentry be of the same
     filesystem type as fs_type.
     (注意，并不要求引用 dentry 与 fs_type 属于相同的文件系统类型。)

   * ::

        struct fs_context *vfs_dup_fs_context(struct fs_context *src_fc);

     Duplicate a filesystem context, copying any options noted and duplicating
     or additionally referencing any resources held therein.  This is available
     for use where a filesystem has to get a mount within a mount, such as NFS4
     does by internally mounting the root of the target server and then doing a
     private pathwalk to the target directory.
     (复制一个文件系统上下文，复制其中记录的任何选项，并复制或额外引用其中持有的任何资源。它可用于文件系统必须在挂载之内再获得一个挂载的场合，例如 NFS4 在内部挂载目标服务器的根，然后对目标目录执行私有的路径遍历(pathwalk)。)

     The purpose in the new context is inherited from the old one.
     (新上下文中的 purpose 继承自旧上下文。)

   * ::

       void put_fs_context(struct fs_context *fc);

     Destroy a filesystem context, releasing any resources it holds.  This
     calls the ->free() operation.  This is intended to be called by anyone who
     created a filesystem context.
     (销毁一个文件系统上下文，释放它持有的任何资源。这会调用 ->free() 操作。任何创建了文件系统上下文的人都应调用它。)

     .. Warning::

        filesystem contexts are not refcounted, so this causes unconditional
	destruction.
        (文件系统上下文没有引用计数，因此这会导致无条件的销毁。)

In all the above operations, apart from the put op, the return is a mount
context pointer or a negative error code.
(在上述所有操作中，除 put 操作外，返回的都是挂载上下文指针或负的错误码。)

For the remaining operations, if an error occurs, a negative error code will be
returned.
(对于其余操作，如果发生错误，将返回负的错误码。)

   * ::

        int vfs_parse_fs_param(struct fs_context *fc,
			       struct fs_parameter *param);

     Supply a single mount parameter to the filesystem context.  This includes
     the specification of the source/device which is specified as the "source"
     parameter (which may be specified multiple times if the filesystem
     supports that).
     (向文件系统上下文提供单个挂载参数。这包括对来源/设备的指定，它被指定为 "source" 参数(如果文件系统支持，可以多次指定)。)

     param specifies the parameter key name and the value.  The parameter is
     first checked to see if it corresponds to a standard mount flag (in which
     case it is used to set an SB_xxx flag and consumed) or a security option
     (in which case the LSM consumes it) before it is passed on to the
     filesystem.
     (param 指定参数的键名和值。该参数首先被检查是否对应标准的挂载标志(在这种情况下，它被用来设置某个 SB_xxx 标志并被消费)或安全选项(在这种情况下由 LSM 消费)，然后才会被传递给文件系统。)

     The parameter value is typed and can be one of:
     (参数值是有类型的，可以是以下之一：)

	====================		=============================
	fs_value_is_flag		Parameter not given a value (未给参数提供值)
	fs_value_is_string		Value is a string (值是一个字符串)
	fs_value_is_blob		Value is a binary blob (值是一个二进制块)
	fs_value_is_filename		Value is a filename* + dirfd (值是一个 filename* 加 dirfd)
	fs_value_is_file		Value is an open file (file*) (值是一个已打开的文件(file*))
	====================		=============================

     If there is a value, that value is stored in a union in the struct in one
     of param->{string,blob,name,file}.  Note that the function may steal and
     clear the pointer, but then becomes responsible for disposing of the
     object.
     (如果有值，该值存储在结构体的 union 中，位于 param->{string,blob,name,file} 之一。注意，该函数可能会窃取并清除该指针，但随后它就负责处置该对象。)

   * ::

       int vfs_parse_fs_qstr(struct fs_context *fc, const char *key,
			       const struct qstr *value);

     A wrapper around vfs_parse_fs_param() that copies the value string it is
     passed.
     (vfs_parse_fs_param() 的一个封装，它会复制传给它的值字符串。)

   * ::

       int vfs_parse_fs_string(struct fs_context *fc, const char *key,
			       const char *value);

     A wrapper around vfs_parse_fs_param() that copies the value string it is
     passed.
     (vfs_parse_fs_param() 的一个封装，它会复制传给它的值字符串。)

   * ::

       int generic_parse_monolithic(struct fs_context *fc, void *data);

     Parse a sys_mount() data page, assuming the form to be a text list
     consisting of key[=val] options separated by commas.  Each item in the
     list is passed to vfs_mount_option().  This is the default when the
     ->parse_monolithic() method is NULL.
     (解析 sys_mount() 数据页，假定其形式为以逗号分隔的 key[=val] 选项组成的文本列表。列表中的每一项都会被传递给 vfs_mount_option()。当 ->parse_monolithic() 方法为 NULL 时，这就是默认实现。)

   * ::

       int vfs_get_tree(struct fs_context *fc);

     Get or create the mountable root and superblock, using the parameters in
     the filesystem context to select/configure the superblock.  This invokes
     the ->get_tree() method.
     (使用文件系统上下文中的参数来选择/配置超级块，以获取或创建可挂载的根和超级块。这会调用 ->get_tree() 方法。)

   * ::

       struct vfsmount *vfs_create_mount(struct fs_context *fc);

     Create a mount given the parameters in the specified filesystem context.
     Note that this does not attach the mount to anything.
     (根据指定文件系统上下文中的参数创建一个挂载。注意，这不会把挂载附加到任何东西上。)


Superblock Creation Helpers
(超级块创建辅助函数)
===========================

A number of VFS helpers are available for use by filesystems for the creation
or looking up of superblocks.
(有许多 VFS 辅助函数可供文件系统用来创建或查找超级块。)

   * ::

       struct super_block *
       sget_fc(struct fs_context *fc,
	       int (*test)(struct super_block *sb, struct fs_context *fc),
	       int (*set)(struct super_block *sb, struct fs_context *fc));

     This is the core routine.  If test is non-NULL, it searches for an
     existing superblock matching the criteria held in the fs_context, using
     the test function to match them.  If no match is found, a new superblock
     is created and the set function is called to set it up.
     (这是核心例程。如果 test 非 NULL，它会使用 test 函数进行匹配，搜索符合 fs_context 中所持条件的现有超级块。如果没有找到匹配，则创建一个新的超级块，并调用 set 函数对其进行设置。)

     Prior to the set function being called, fc->s_fs_info will be transferred
     to sb->s_fs_info - and fc->s_fs_info will be cleared if set returns
     success (ie. 0).
     (在 set 函数被调用之前，fc->s_fs_info 会被转移到 sb->s_fs_info——如果 set 返回成功(即 0)，fc->s_fs_info 将被清除。)

The following helpers all wrap sget_fc():
(以下辅助函数都是对 sget_fc() 的封装：)

	(1) vfs_get_single_super

	    Only one such superblock may exist in the system.  Any further
	    attempt to get a new superblock gets this one (and any parameter
	    differences are ignored).
	    (系统中只能存在一个这样的超级块。任何进一步获取新超级块的尝试都会得到这一个(并且任何参数差异都会被忽略)。)

	(2) vfs_get_keyed_super

	    Multiple superblocks of this type may exist and they're keyed on
	    their s_fs_info pointer (for example this may refer to a
	    namespace).
	    (这种类型的多个超级块可以存在，它们以各自的 s_fs_info 指针为键(例如这可能指向一个命名空间)。)

	(3) vfs_get_independent_super

	    Multiple independent superblocks of this type may exist.  This
	    function never matches an existing one and always creates a new
	    one.
	    (这种类型的多个独立超级块可以存在。此函数从不匹配现有的超级块，而总是创建一个新的。)


Parameter Description
(参数描述)
=====================

Parameters are described using structures defined in linux/fs_parser.h.
There's a core description struct that links everything together::

	struct fs_parameter_description {
		const struct fs_parameter_spec *specs;
		const struct fs_parameter_enum *enums;
	};

(参数使用 linux/fs_parser.h 中定义的结构体来描述。有一个把所有内容链接在一起的核心描述结构体：)

For example::

	enum {
		Opt_autocell,
		Opt_bar,
		Opt_dyn,
		Opt_foo,
		Opt_source,
	};

	static const struct fs_parameter_description afs_fs_parameters = {
		.specs		= afs_param_specs,
		.enums		= afs_param_enums,
	};

(例如：)

The members are as follows:
(成员如下：)

 (1) ::

       const struct fs_parameter_specification *specs;

     Table of parameter specifications, terminated with a null entry, where the
     entries are of type::

	struct fs_parameter_spec {
		const char		*name;
		u8			opt;
		enum fs_parameter_type	type:8;
		unsigned short		flags;
	};

     (参数规格表，以一个空条目结尾，其中条目的类型为：)

     The 'name' field is a string to match exactly to the parameter key (no
     wildcards, patterns and no case-independence) and 'opt' is the value that
     will be returned by the fs_parser() function in the case of a successful
     match.
     ('name' 字段是一个要与参数键精确匹配的字符串(无通配符、无模式，也不区分大小写)，而 'opt' 是在成功匹配时将由 fs_parser() 函数返回的值。)

     The 'type' field indicates the desired value type and must be one of:
     ('type' 字段指明所期望的值类型，必须是以下之一：)

	=======================	=======================	=====================
	TYPE NAME		EXPECTED VALUE		RESULT IN (类型名 / 期望的值 / 结果存于)
	=======================	=======================	=====================
	fs_param_is_flag	No value		n/a (无值)
	fs_param_is_bool	Boolean value		result->boolean (布尔值)
	fs_param_is_u32		32-bit unsigned int	result->uint_32 (32 位无符号整数)
	fs_param_is_u32_octal	32-bit octal int	result->uint_32 (32 位八进制整数)
	fs_param_is_u32_hex	32-bit hex int		result->uint_32 (32 位十六进制整数)
	fs_param_is_s32		32-bit signed int	result->int_32 (32 位有符号整数)
	fs_param_is_u64		64-bit unsigned int	result->uint_64 (64 位无符号整数)
	fs_param_is_enum	Enum value name 	result->uint_32 (枚举值名)
	fs_param_is_string	Arbitrary string	param->string (任意字符串)
	fs_param_is_blockdev	Blockdev path		* Needs lookup (块设备路径；需要查找)
	fs_param_is_fd		File descriptor		result->int_32 (文件描述符)
	fs_param_is_uid		User ID (u32)           result->uid (用户 ID (u32))
	fs_param_is_gid		Group ID (u32)          result->gid (组 ID (u32))
	=======================	=======================	=====================

     Note that if the value is of fs_param_is_bool type, fs_parse() will try
     to match any string value against "0", "1", "no", "yes", "false", "true".
     (注意，如果值的类型为 fs_param_is_bool，fs_parse() 将尝试把任何字符串值与 "0"、"1"、"no"、"yes"、"false"、"true" 进行匹配。)

     Each parameter can also be qualified with 'flags':
     (每个参数还可以用 'flags' 来限定：)

	=======================	================================================
	fs_param_v_optional	The value is optional (值是可选的)
	fs_param_neg_with_no	result->negated set if key is prefixed with "no" (如果键以 "no" 为前缀，则设置 result->negated)
	fs_param_neg_with_empty	result->negated set if value is "" (如果值为 ""，则设置 result->negated)
	fs_param_deprecated	The parameter is deprecated. (该参数已弃用。)
	=======================	================================================

     These are wrapped with a number of convenience wrappers:
     (这些由许多便利的封装宏进行了包装：)

	=======================	===============================================
	MACRO (宏)			SPECIFIES (指定)
	=======================	===============================================
	fsparam_flag()		fs_param_is_flag
	fsparam_flag_no()	fs_param_is_flag, fs_param_neg_with_no
	fsparam_bool()		fs_param_is_bool
	fsparam_u32()		fs_param_is_u32
	fsparam_u32oct()	fs_param_is_u32_octal
	fsparam_s32()		fs_param_is_s32
	fsparam_u64()		fs_param_is_u64
	fsparam_enum()		fs_param_is_enum
	fsparam_string()	fs_param_is_string
	fsparam_bdev()		fs_param_is_blockdev
	fsparam_fd()		fs_param_is_fd
	fsparam_uid()		fs_param_is_uid
	fsparam_gid()		fs_param_is_gid
	=======================	===============================================

     all of which take two arguments, name string and option number - for
     example::

	static const struct fs_parameter_spec afs_param_specs[] = {
		fsparam_flag	("autocell",	Opt_autocell),
		fsparam_flag	("dyn",		Opt_dyn),
		fsparam_string	("source",	Opt_source),
		fsparam_flag_no	("foo",		Opt_foo),
		{}
	};

     (所有这些宏都接受两个参数，即名称字符串和选项号——例如：)

     An addition macro, __fsparam() is provided that takes an additional pair
     of arguments to specify the type and the flags for anything that doesn't
     match one of the above macros.
     (另外还提供了一个附加宏 __fsparam()，它接受额外的一对参数，用于为不匹配上述任何宏的东西指定类型和标志。)

 (2) ::

       const struct fs_parameter_enum *enums;

     Table of enum value names to integer mappings, terminated with a null
     entry.  This is of type::

	struct fs_parameter_enum {
		u8		opt;
		char		name[14];
		u8		value;
	};

     (枚举值名到整数映射的表，以一个空条目结尾。其类型为：)

     Where the array is an unsorted list of { parameter ID, name }-keyed
     elements that indicate the value to map to, e.g.::

	static const struct fs_parameter_enum afs_param_enums[] = {
		{ Opt_bar,   "x",      1},
		{ Opt_bar,   "y",      23},
		{ Opt_bar,   "z",      42},
	};

     (其中该数组是以 { 参数 ID, 名称 } 为键的元素的无序列表，用来指明要映射到的值，例如：)

     If a parameter of type fs_param_is_enum is encountered, fs_parse() will
     try to look the value up in the enum table and the result will be stored
     in the parse result.
     (如果遇到类型为 fs_param_is_enum 的参数，fs_parse() 将尝试在枚举表中查找该值，结果将存储在解析结果中。)

The parser should be pointed to by the parser pointer in the file_system_type
struct as this will provide validation on registration (if
CONFIG_VALIDATE_FS_PARSER=y) and will allow the description to be queried from
userspace using the fsinfo() syscall.
(解析器应由 file_system_type 结构体中的 parser 指针来指向，因为这将在注册时提供验证(如果 CONFIG_VALIDATE_FS_PARSER=y)，并允许通过 fsinfo() 系统调用从用户空间查询该描述。)


Parameter Helper Functions
(参数辅助函数)
==========================

A number of helper functions are provided to help a filesystem or an LSM
process the parameters it is given.
(提供了一些辅助函数，帮助文件系统或 LSM 处理交给它们的参数。)

   * ::

       int lookup_constant(const struct constant_table tbl[],
			   const char *name, int not_found);

     Look up a constant by name in a table of name -> integer mappings.  The
     table is an array of elements of the following type::

	struct constant_table {
		const char	*name;
		int		value;
	};

     (在名称 -> 整数映射表中按名称查找一个常量。该表是以下类型元素的数组：)

     If a match is found, the corresponding value is returned.  If a match
     isn't found, the not_found value is returned instead.
     (如果找到匹配，则返回相应的值。如果没有找到匹配，则改为返回 not_found 值。)

   * ::

       bool fs_validate_description(const char *name,
                                    const struct fs_parameter_description *desc);

     This performs some validation checks on a parameter description.  It
     returns true if the description is good and false if it is not.  It will
     log errors to the kernel log buffer if validation fails.
     (这对参数描述执行一些验证检查。如果描述是良好的则返回 true，否则返回 false。如果验证失败，它会将错误记录到内核日志缓冲区中。)

   * ::

        int fs_parse(struct fs_context *fc,
		     const struct fs_parameter_description *desc,
		     struct fs_parameter *param,
		     struct fs_parse_result *result);

     This is the main interpreter of parameters.  It uses the parameter
     description to look up a parameter by key name and to convert that to an
     option number (which it returns).
     (这是参数的主要解释器。它使用参数描述按键名查找参数，并将其转换为选项号(即它的返回值)。)

     If successful, and if the parameter type indicates the result is a
     boolean, integer, enum, uid, or gid type, the value is converted by this
     function and the result stored in
     result->{boolean,int_32,uint_32,uint_64,uid,gid}.
     (如果成功，并且参数类型表明结果是布尔、整数、枚举、uid 或 gid 类型，则值由该函数转换，结果存储在 result->{boolean,int_32,uint_32,uint_64,uid,gid} 中。)

     If a match isn't initially made, the key is prefixed with "no" and no
     value is present then an attempt will be made to look up the key with the
     prefix removed.  If this matches a parameter for which the type has flag
     fs_param_neg_with_no set, then a match will be made and result->negated
     will be set to true.
     (如果最初没有匹配，键带有 "no" 前缀且不存在值，则将尝试查找去掉该前缀后的键。如果这匹配到一个类型设置了 fs_param_neg_with_no 标志的参数，则将进行匹配，并且 result->negated 将被设置为 true。)

     If the parameter isn't matched, -ENOPARAM will be returned; if the
     parameter is matched, but the value is erroneous, -EINVAL will be
     returned; otherwise the parameter's option number will be returned.
     (如果参数未匹配，将返回 -ENOPARAM；如果参数匹配但值有误，将返回 -EINVAL；否则将返回该参数的选项号。)

   * ::

       int fs_lookup_param(struct fs_context *fc,
			   struct fs_parameter *value,
			   bool want_bdev,
			   unsigned int flags,
			   struct path *_path);

     This takes a parameter that carries a string or filename type and attempts
     to do a path lookup on it.  If the parameter expects a blockdev, a check
     is made that the inode actually represents one.
     (它接受一个携带字符串或文件名类型的参数，并尝试对其进行路径查找。如果该参数期望的是块设备，则会检查该 inode 是否确实表示一个块设备。)

     Returns 0 if successful and ``*_path`` will be set; returns a negative
     error code if not.
     (成功时返回 0 并且 ``*_path`` 将被设置；不成功则返回负的错误码。)
