.. SPDX-License-Identifier: GPL-2.0

===================================
Cache on Already Mounted Filesystem
===================================

(在已挂载文件系统上的缓存)

.. Contents:

 (*) Overview.
     (概述。)

 (*) Requirements.
     (要求。)

 (*) Configuration.
     (配置。)

 (*) Starting the cache.
     (启动缓存。)

 (*) Things to avoid.
     (应当避免的事情。)

 (*) Cache culling.
     (缓存剔除。)

 (*) Cache structure.
     (缓存结构。)

 (*) Security model and SELinux.
     (安全模型与 SELinux。)

 (*) A note on security.
     (关于安全的一点说明。)

 (*) Statistical information.
     (统计信息。)

 (*) Debugging.
     (调试。)

 (*) On-demand Read.
     (按需读。)


Overview
(概述)
========

CacheFiles is a caching backend that's meant to use as a cache a directory on
an already mounted filesystem of a local type (such as Ext3).
(CacheFiles 是一个缓存后端，它把本地类型文件系统(例如 Ext3)上某个已挂载文件系统中的目录用作缓存。)

CacheFiles uses a userspace daemon to do some of the cache management - such as
reaping stale nodes and culling.  This is called cachefilesd and lives in
/sbin.
(CacheFiles 使用一个用户空间守护进程来完成部分缓存管理工作——例如回收过期节点和剔除。该守护进程名为 cachefilesd，位于 /sbin 中。)

The filesystem and data integrity of the cache are only as good as those of the
filesystem providing the backing services.  Note that CacheFiles does not
attempt to journal anything since the journalling interfaces of the various
filesystems are very specific in nature.
(缓存的文件系统与数据完整性，仅与提供后备服务的那个文件系统相当。注意，CacheFiles 不尝试对任何东西做日志(journal)记录，因为各种文件系统的日志接口在性质上都非常特殊。)

CacheFiles creates a misc character device - "/dev/cachefiles" - that is used
to communication with the daemon.  Only one thing may have this open at once,
and while it is open, a cache is at least partially in existence.  The daemon
opens this and sends commands down it to control the cache.
(CacheFiles 会创建一个 misc 字符设备——"/dev/cachefiles"——用于与守护进程通信。同一时间只能有一个东西打开它，而且只要它处于打开状态，缓存就至少是部分存在的。守护进程打开它并向其发送命令以控制缓存。)

CacheFiles is currently limited to a single cache.
(CacheFiles 目前仅限于单个缓存。)

CacheFiles attempts to maintain at least a certain percentage of free space on
the filesystem, shrinking the cache by culling the objects it contains to make
space if necessary - see the "Cache Culling" section.  This means it can be
placed on the same medium as a live set of data, and will expand to make use of
spare space and automatically contract when the set of data requires more
space.
(CacheFiles 尝试在文件系统上维持至少一定比例的空闲空间，必要时通过剔除其所含对象来收缩缓存以腾出空间——参见"缓存剔除(Cache Culling)"一节。这意味着它可以与一份活跃数据集放在同一介质上，并会扩展以利用空闲空间，而在该数据集需要更多空间时自动收缩。)



Requirements
(要求)
============

The use of CacheFiles and its daemon requires the following features to be
available in the system and in the cache filesystem:
(使用 CacheFiles 及其守护进程，需要系统和缓存文件系统中具备以下特性：)

	- dnotify.
	  (dnotify。)

	- extended attributes (xattrs).
	  (扩展属性(xattrs)。)

	- openat() and friends.
	  (openat() 及相关函数。)

	- bmap() support on files in the filesystem (FIBMAP ioctl).
	  (文件系统中的文件支持 bmap()(FIBMAP ioctl)。)

	- The use of bmap() to detect a partial page at the end of the file.
	  (使用 bmap() 来检测文件末尾的部分页。)

It is strongly recommended that the "dir_index" option is enabled on Ext3
filesystems being used as a cache.
(强烈建议在用作缓存的 Ext3 文件系统上启用 "dir_index" 选项。)


Configuration
(配置)
=============

The cache is configured by a script in /etc/cachefilesd.conf.  These commands
set up cache ready for use.  The following script commands are available:
(缓存由 /etc/cachefilesd.conf 中的一个脚本进行配置。这些命令把缓存设置好以备使用。可用的脚本命令如下：)

 brun <N>%, bcull <N>%, bstop <N>%, frun <N>%, fcull <N>%, fstop <N>%
	Configure the culling limits.  Optional.  See the section on culling
	The defaults are 7% (run), 5% (cull) and 1% (stop) respectively.
	(配置剔除限制。可选。参见关于剔除的一节。默认值分别为 7%(run)、5%(cull) 和 1%(stop)。)

	The commands beginning with a 'b' are file space (block) limits, those
	beginning with an 'f' are file count limits.
	(以 'b' 开头的命令是文件空间(块)限制，以 'f' 开头的命令是文件数量限制。)

 dir <path>
	Specify the directory containing the root of the cache.  Mandatory.
	(指定包含缓存根的目录。必填。)

 tag <name>
	Specify a tag to FS-Cache to use in distinguishing multiple caches.
	Optional.  The default is "CacheFiles".
	(指定一个标签，供 FS-Cache 用于区分多个缓存。可选。默认值为 "CacheFiles"。)

 debug <mask>
	Specify a numeric bitmask to control debugging in the kernel module.
	Optional.  The default is zero (all off).  The following values can be
	OR'd into the mask to collect various information:
	(指定一个数字位掩码来控制内核模块中的调试。可选。默认值为零(全部关闭)。下列值可以按位或进该掩码，以收集各种信息：)

		==	=================================================
		1	Turn on trace of function entry (_enter() macros) (打开函数入口跟踪(_enter() 宏))
		2	Turn on trace of function exit (_leave() macros) (打开函数退出跟踪(_leave() 宏))
		4	Turn on trace of internal debug points (_debug()) (打开内部调试点跟踪(_debug()))
		==	=================================================

	This mask can also be set through sysfs, eg::

		echo 5 > /sys/module/cachefiles/parameters/debug

	(此掩码也可以通过 sysfs 设置，例如：)


Starting the Cache
(启动缓存)
==================

The cache is started by running the daemon.  The daemon opens the cache device,
configures the cache and tells it to begin caching.  At that point the cache
binds to fscache and the cache becomes live.
(通过运行守护进程来启动缓存。守护进程打开缓存设备，配置缓存并告知它开始缓存。此时缓存绑定到 fscache，缓存即进入活动状态。)

The daemon is run as follows::

	/sbin/cachefilesd [-d]* [-s] [-n] [-f <configfile>]

(守护进程按如下方式运行：)

The flags are:
(标志如下：)

 ``-d``
	Increase the debugging level.  This can be specified multiple times and
	is cumulative with itself.
	(提高调试级别。可以多次指定，并且会与其自身累加。)

 ``-s``
	Send messages to stderr instead of syslog.
	(把消息发送到 stderr 而不是 syslog。)

 ``-n``
	Don't daemonise and go into background.
	(不要守护进程化并转入后台。)

 ``-f <configfile>``
	Use an alternative configuration file rather than the default one.
	(使用替代的配置文件，而不是默认的那个。)


Things to Avoid
(应当避免的事情)
===============

Do not mount other things within the cache as this will cause problems.  The
kernel module contains its own very cut-down path walking facility that ignores
mountpoints, but the daemon can't avoid them.
(不要在缓存内挂载其他东西，因为这会导致问题。内核模块包含它自己的一套非常精简的路径遍历设施，它会忽略挂载点，但守护进程无法避开它们。)

Do not create, rename or unlink files and directories in the cache while the
cache is active, as this may cause the state to become uncertain.
(在缓存处于活动状态时，不要在缓存中创建、重命名或取消链接(unlink)文件和目录，因为这可能导致状态变得不确定。)

Renaming files in the cache might make objects appear to be other objects (the
filename is part of the lookup key).
(在缓存中重命名文件，可能会使对象看起来像是其他对象(文件名是查找键的一部分)。)

Do not change or remove the extended attributes attached to cache files by the
cache as this will cause the cache state management to get confused.
(不要更改或移除缓存附加到缓存文件上的扩展属性，因为这会使缓存状态管理陷入混乱。)

Do not create files or directories in the cache, lest the cache get confused or
serve incorrect data.
(不要在缓存中创建文件或目录，以免缓存陷入混乱或提供不正确的数据。)

Do not chmod files in the cache.  The module creates things with minimal
permissions to prevent random users being able to access them directly.
(不要对缓存中的文件执行 chmod。模块以最小权限创建这些东西，以防止任意用户能够直接访问它们。)


Cache Culling
(缓存剔除)
=============

The cache may need culling occasionally to make space.  This involves
discarding objects from the cache that have been used less recently than
anything else.  Culling is based on the access time of data objects.  Empty
directories are culled if not in use.
(缓存可能偶尔需要剔除(cull)以腾出空间。这涉及从缓存中丢弃那些比其他任何东西都更久未使用的对象。剔除基于数据对象的访问时间。空目录若未被使用，也会被剔除。)

Cache culling is done on the basis of the percentage of blocks and the
percentage of files available in the underlying filesystem.  There are six
"limits":
(缓存剔除依据底层文件系统中可用块的百分比和可用文件的百分比来进行。共有六个"限制"：)

 brun, frun
     If the amount of free space and the number of available files in the cache
     rises above both these limits, then culling is turned off.
     (如果缓存中的空闲空间量和可用文件数量上升到这两个限制之上，则剔除被关闭。)

 bcull, fcull
     If the amount of available space or the number of available files in the
     cache falls below either of these limits, then culling is started.
     (如果缓存中的可用空间量或可用文件数量下降到低于这些限制中的任何一个，则剔除被启动。)

 bstop, fstop
     If the amount of available space or the number of available files in the
     cache falls below either of these limits, then no further allocation of
     disk space or files is permitted until culling has raised things above
     these limits again.
     (如果缓存中的可用空间量或可用文件数量下降到低于这些限制中的任何一个，则不允许再分配磁盘空间或文件，直到剔除把这些数值重新提升到限制之上。)

These must be configured thusly::

	0 <= bstop < bcull < brun < 100
	0 <= fstop < fcull < frun < 100

(必须按如下方式配置这些值：)

Note that these are percentages of available space and available files, and do
_not_ appear as 100 minus the percentage displayed by the "df" program.
(注意，这些是可用空间和可用文件的百分比，_not_(并不)表现为 100 减去 "df" 程序所显示的百分比。)

The userspace daemon scans the cache to build up a table of cullable objects.
These are then culled in least recently used order.  A new scan of the cache is
started as soon as space is made in the table.  Objects will be skipped if
their atimes have changed or if the kernel module says it is still using them.
(用户空间守护进程扫描缓存，以建立一个可剔除对象的表。随后按最近最少使用的顺序剔除这些对象。一旦表中腾出了空间，就会立即开始对缓存的新一轮扫描。如果对象的 atime 已改变，或者内核模块表示它仍在使用该对象，则该对象会被跳过。)


Cache Structure
(缓存结构)
===============

The CacheFiles module will create two directories in the directory it was
given:
(CacheFiles 模块会在它被给予的目录中创建两个目录：)

 * cache/
 * graveyard/

The active cache objects all reside in the first directory.  The CacheFiles
kernel module moves any retired or culled objects that it can't simply unlink
to the graveyard from which the daemon will actually delete them.
(活动的缓存对象全部位于第一个目录中。CacheFiles 内核模块会把任何无法直接 unlink 的退役或被剔除的对象移动到墓地(graveyard)目录，守护进程将从那里真正删除它们。)

The daemon uses dnotify to monitor the graveyard directory, and will delete
anything that appears therein.
(守护进程使用 dnotify 监视墓地目录，并会删除其中出现的任何东西。)


The module represents index objects as directories with the filename "I..." or
"J...".  Note that the "cache/" directory is itself a special index.
(模块将索引对象表示为文件名为 "I..." 或 "J..." 的目录。注意，"cache/" 目录本身就是一个特殊的索引。)

Data objects are represented as files if they have no children, or directories
if they do.  Their filenames all begin "D..." or "E...".  If represented as a
directory, data objects will have a file in the directory called "data" that
actually holds the data.
(数据对象在没有子对象时表示为文件，有子对象时则表示为目录。它们的文件名都以 "D..." 或 "E..." 开头。如果表示为目录，数据对象会在该目录中有一个名为 "data" 的文件，由它实际持有数据。)

Special objects are similar to data objects, except their filenames begin
"S..." or "T...".
(特殊对象与数据对象类似，只是它们的文件名以 "S..." 或 "T..." 开头。)


If an object has children, then it will be represented as a directory.
Immediately in the representative directory are a collection of directories
named for hash values of the child object keys with an '@' prepended.  Into
this directory, if possible, will be placed the representations of the child
objects::

	 /INDEX    /INDEX     /INDEX                            /DATA FILES
	/=========/==========/=================================/================
	cache/@4a/I03nfs/@30/Ji000000000000000--fHg8hi8400
	cache/@4a/I03nfs/@30/Ji000000000000000--fHg8hi8400/@75/Es0g000w...DB1ry
	cache/@4a/I03nfs/@30/Ji000000000000000--fHg8hi8400/@75/Es0g000w...N22ry
	cache/@4a/I03nfs/@30/Ji000000000000000--fHg8hi8400/@75/Es0g000w...FP1ry


(如果一个对象有子对象，那么它将被表示为一个目录。在代表目录中，直接就是一组以子对象键的哈希值命名、并带有 '@' 前缀的目录。如果可能的话，子对象的表示形式会被放入该目录中：)


If the key is so long that it exceeds NAME_MAX with the decorations added on to
it, then it will be cut into pieces, the first few of which will be used to
make a nest of directories, and the last one of which will be the objects
inside the last directory.  The names of the intermediate directories will have
'+' prepended::

	J1223/@23/+xy...z/+kl...m/Epqr


(如果键太长，加上装饰后超过了 NAME_MAX，那么它将被切成若干段：前几段用于构成一层层嵌套的目录，最后一段则是最后一个目录中的对象。中间目录的名称将加上 '+' 前缀：)


Note that keys are raw data, and not only may they exceed NAME_MAX in size,
they may also contain things like '/' and NUL characters, and so they may not
be suitable for turning directly into a filename.
(注意，键是原始数据：它们不仅大小可能超过 NAME_MAX，还可能包含 '/' 和 NUL 字符之类的东西，因此可能不适合直接转换为文件名。)

To handle this, CacheFiles will use a suitably printable filename directly and
"base-64" encode ones that aren't directly suitable.  The two versions of
object filenames indicate the encoding:
(为处理这种情况，CacheFiles 会直接使用适当可打印的文件名，而对不适合直接使用的进行 "base-64" 编码。对象文件名的这两个版本表明了其所用的编码：)

	===============	===============	===============
	OBJECT TYPE	PRINTABLE	ENCODED
	===============	===============	===============
	Index		"I..."		"J..."
	Data		"D..."		"E..."
	Special		"S..."		"T..."
	===============	===============	===============

Intermediate directories are always "@" or "+" as appropriate.
(中间目录总是适当地为 "@" 或 "+"。)


Each object in the cache has an extended attribute label that holds the object
type ID (required to distinguish special objects) and the auxiliary data from
the netfs.  The latter is used to detect stale objects in the cache and update
or retire them.
(缓存中的每个对象都有一个扩展属性标签，其中保存着对象类型 ID(区分特殊对象所必需)以及来自 netfs 的辅助数据。后者用于检测缓存中的过期对象，并对它们进行更新或退役。)


Note that CacheFiles will erase from the cache any file it doesn't recognise or
any file of an incorrect type (such as a FIFO file or a device file).
(注意，CacheFiles 会从缓存中擦除它不认识的任何文件，以及类型不正确的任何文件(例如 FIFO 文件或设备文件)。)


Security Model and SELinux
(安全模型与 SELinux)
==========================

CacheFiles is implemented to deal properly with the LSM security features of
the Linux kernel and the SELinux facility.
(CacheFiles 的实现能够妥善处理 Linux 内核的 LSM 安全特性以及 SELinux 设施。)

One of the problems that CacheFiles faces is that it is generally acting on
behalf of a process, and running in that process's context, and that includes a
security context that is not appropriate for accessing the cache - either
because the files in the cache are inaccessible to that process, or because if
the process creates a file in the cache, that file may be inaccessible to other
processes.
(CacheFiles 面临的问题之一是：它通常是代表某个进程行事的，并在该进程的上下文中运行，而该上下文包含的安全上下文并不适合访问缓存——要么是因为该进程无法访问缓存中的文件，要么是因为该进程若在缓存中创建了文件，该文件可能对其他进程不可访问。)

The way CacheFiles works is to temporarily change the security context (fsuid,
fsgid and actor security label) that the process acts as - without changing the
security context of the process when it the target of an operation performed by
some other process (so signalling and suchlike still work correctly).
(CacheFiles 的工作方式是：临时更改该进程所扮演的安全上下文(fsuid、fsgid 和执行者安全标签)——而不改变当该进程成为其他进程所执行操作的目标时该进程的安全上下文(因此信号发送之类的功能仍然正常工作)。)


When the CacheFiles module is asked to bind to its cache, it:
(当 CacheFiles 模块被要求绑定到它的缓存时，它会：)

 (1) Finds the security label attached to the root cache directory and uses
     that as the security label with which it will create files.  By default,
     this is::

	cachefiles_var_t

     (查找附加在根缓存目录上的安全标签，并将其用作它创建文件时所使用的安全标签。默认情况下，这是：)

 (2) Finds the security label of the process which issued the bind request
     (presumed to be the cachefilesd daemon), which by default will be::

	cachefilesd_t

     (查找发出绑定请求的进程(推定为 cachefilesd 守护进程)的安全标签，默认情况下它将是：)

     and asks LSM to supply a security ID as which it should act given the
     daemon's label.  By default, this will be::

	cachefiles_kernel_t

     (并要求 LSM 提供一个安全 ID，使它应当以该 ID 的身份行事，依据是守护进程的标签。默认情况下，这将是：)

     SELinux transitions the daemon's security ID to the module's security ID
     based on a rule of this form in the policy::

	type_transition <daemon's-ID> kernel_t : process <module's-ID>;

     (SELinux 依据策略中如下形式的规则，把守护进程的安全 ID 转换为模块的安全 ID：)

     For instance::

	type_transition cachefilesd_t kernel_t : process cachefiles_kernel_t;

     (例如：)


The module's security ID gives it permission to create, move and remove files
and directories in the cache, to find and access directories and files in the
cache, to set and access extended attributes on cache objects, and to read and
write files in the cache.
(模块的安全 ID 授予它如下权限：在缓存中创建、移动和移除文件与目录；查找并访问缓存中的目录和文件；设置和访问缓存对象上的扩展属性；以及读写缓存中的文件。)

The daemon's security ID gives it only a very restricted set of permissions: it
may scan directories, stat files and erase files and directories.  It may
not read or write files in the cache, and so it is precluded from accessing the
data cached therein; nor is it permitted to create new files in the cache.
(守护进程的安全 ID 只授予它一个非常受限的权限集：它可以扫描目录、对文件执行 stat、擦除文件和目录。它不可以读写缓存中的文件，因此被禁止访问其中缓存的数据；它也不被允许在缓存中创建新文件。)


There are policy source files available in:
(以下位置提供有策略源文件：)

	https://people.redhat.com/~dhowells/fscache/cachefilesd-0.8.tar.bz2

and later versions.  In that tarball, see the files::

	cachefilesd.te
	cachefilesd.fc
	cachefilesd.if

(以及更新的版本。在该 tarball 中，请参见以下文件：)

They are built and installed directly by the RPM.
(它们由 RPM 直接构建和安装。)

If a non-RPM based system is being used, then copy the above files to their own
directory and run::

	make -f /usr/share/selinux/devel/Makefile
	semodule -i cachefilesd.pp

(如果使用的是非基于 RPM 的系统，请把上述文件复制到它们自己的目录中，然后运行：)

You will need checkpolicy and selinux-policy-devel installed prior to the
build.
(在构建之前，你需要已安装 checkpolicy 和 selinux-policy-devel。)


By default, the cache is located in /var/fscache, but if it is desirable that
it should be elsewhere, than either the above policy files must be altered, or
an auxiliary policy must be installed to label the alternate location of the
cache.
(默认情况下，缓存位于 /var/fscache，但如果希望它位于其他位置，那么必须修改上述策略文件，或者必须安装一条辅助策略来为缓存的其他位置打标签。)

For instructions on how to add an auxiliary policy to enable the cache to be
located elsewhere when SELinux is in enforcing mode, please see::

	/usr/share/doc/cachefilesd-*/move-cache.txt

(关于在 SELinux 处于 enforcing 模式时如何添加辅助策略以使缓存能够位于其他位置的说明，请参见：)

When the cachefilesd rpm is installed; alternatively, the document can be found
in the sources.
(当 cachefilesd rpm 已安装时可用；另外，该文档也可以在源代码中找到。)


A Note on Security
(关于安全的一点说明)
==================

CacheFiles makes use of the split security in the task_struct.  It allocates
its own task_security structure, and redirects current->cred to point to it
when it acts on behalf of another process, in that process's context.
(CacheFiles 利用了 task_struct 中拆分的安全机制。当它代表另一个进程、在该进程的上下文中行事时，它会分配自己的 task_security 结构，并让 current->cred 重定向指向它。)

The reason it does this is that it calls vfs_mkdir() and suchlike rather than
bypassing security and calling inode ops directly.  Therefore the VFS and LSM
may deny the CacheFiles access to the cache data because under some
circumstances the caching code is running in the security context of whatever
process issued the original syscall on the netfs.
(它这样做的原因是：它调用 vfs_mkdir() 之类函数，而不是绕过安全机制直接调用 inode 操作。因此，VFS 和 LSM 可能会拒绝 CacheFiles 访问缓存数据，因为在某些情况下，缓存代码正运行在最初对 netfs 发起系统调用的那个进程的安全上下文中。)

Furthermore, should CacheFiles create a file or directory, the security
parameters with that object is created (UID, GID, security label) would be
derived from that process that issued the system call, thus potentially
preventing other processes from accessing the cache - including CacheFiles's
cache management daemon (cachefilesd).
(此外，如果 CacheFiles 创建了文件或目录，创建该对象所用的安全参数(UID、GID、安全标签)将派生自发出该系统调用的那个进程，从而可能阻止其他进程访问缓存——包括 CacheFiles 自己的缓存管理守护进程(cachefilesd)。)

What is required is to temporarily override the security of the process that
issued the system call.  We can't, however, just do an in-place change of the
security data as that affects the process as an object, not just as a subject.
This means it may lose signals or ptrace events for example, and affects what
the process looks like in /proc.
(所需做的是临时覆盖发出该系统调用的进程的安全属性。然而，我们不能只在原位修改安全数据，因为那会影响作为对象的进程，而不仅仅是作为主体的进程。这意味着它可能会丢失信号或 ptrace 事件之类的东西，并影响该进程在 /proc 中的呈现。)

So CacheFiles makes use of a logical split in the security between the
objective security (task->real_cred) and the subjective security (task->cred).
The objective security holds the intrinsic security properties of a process and
is never overridden.  This is what appears in /proc, and is what is used when a
process is the target of an operation by some other process (SIGKILL for
example).
(因此，CacheFiles 利用安全机制中的一个逻辑拆分：客观安全(objective security, task->real_cred)与主观安全(subjective security, task->cred)。客观安全保存进程固有的安全属性，永远不会被覆盖。这就是出现在 /proc 中的内容，也是当某进程成为其他进程所执行操作(例如 SIGKILL)的目标时所使用的内容。)

The subjective security holds the active security properties of a process, and
may be overridden.  This is not seen externally, and is used when a process
acts upon another object, for example SIGKILLing another process or opening a
file.
(主观安全保存进程当前生效的安全属性，可以被覆盖。它不会对外可见，并在进程对另一个对象采取行动时使用，例如 SIGKILL 另一个进程或打开一个文件。)

LSM hooks exist that allow SELinux (or Smack or whatever) to reject a request
for CacheFiles to run in a context of a specific security label, or to create
files and directories with another security label.
(存在这样的 LSM 钩子：允许 SELinux(或 Smack 或其他任何 LSM)拒绝让 CacheFiles 在特定安全标签的上下文中运行的请求，或拒绝用另一个安全标签创建文件和目录。)


Statistical Information
(统计信息)
=======================

If FS-Cache is compiled with the following option enabled::

	CONFIG_CACHEFILES_HISTOGRAM=y

(如果 FS-Cache 编译时启用了以下选项：)

then it will gather certain statistics and display them through a proc file.
(那么它会收集某些统计信息，并通过一个 proc 文件显示它们。)

 /proc/fs/cachefiles/histogram

     ::

	cat /proc/fs/cachefiles/histogram
	JIFS  SECS  LOOKUPS   MKDIRS    CREATES
	===== ===== ========= ========= =========

     This shows the breakdown of the number of times each amount of time
     between 0 jiffies and HZ-1 jiffies a variety of tasks took to run.  The
     columns are as follows:
     (这里显示了各种任务花费 0 个 jiffy 到 HZ-1 个 jiffy 之间的每种时长的次数细分。各列含义如下：)

	=======		=======================================================
	COLUMN		TIME MEASUREMENT (时间度量)
	=======		=======================================================
	LOOKUPS		Length of time to perform a lookup on the backing fs (在后备文件系统上执行一次查找所花的时间)
	MKDIRS		Length of time to perform a mkdir on the backing fs (在后备文件系统上执行一次 mkdir 所花的时间)
	CREATES		Length of time to perform a create on the backing fs (在后备文件系统上执行一次 create 所花的时间)
	=======		=======================================================

     Each row shows the number of events that took a particular range of times.
     Each step is 1 jiffy in size.  The JIFS column indicates the particular
     jiffy range covered, and the SECS field the equivalent number of seconds.
     (每一行显示了花费特定时间范围的事件数量。每一步的大小为 1 个 jiffy。JIFS 列指示所覆盖的特定 jiffy 范围，SECS 字段则是等效的秒数。)


Debugging
(调试)
=========

If CONFIG_CACHEFILES_DEBUG is enabled, the CacheFiles facility can have runtime
debugging enabled by adjusting the value in::

	/sys/module/cachefiles/parameters/debug

(如果启用了 CONFIG_CACHEFILES_DEBUG，就可以通过调整以下文件中的值来为 CacheFiles 设施启用运行时调试：)

This is a bitmask of debugging streams to enable:
(这是要启用的调试流位掩码：)

	=======	=======	===============================	=======================
	BIT	VALUE	STREAM				POINT
	=======	=======	===============================	=======================
	0	1	General				Function entry trace (函数入口跟踪)
	1	2					Function exit trace (函数退出跟踪)
	2	4					General
	=======	=======	===============================	=======================

The appropriate set of values should be OR'd together and the result written to
the control file.  For example::

	echo $((1|4|8)) >/sys/module/cachefiles/parameters/debug

(应当把适当的一组值按位或起来，并将结果写入控制文件。例如：)

will turn on all function entry debugging.
(将会打开所有函数入口调试。)


On-demand Read
(按需读)
==============

When working in its original mode, CacheFiles serves as a local cache for a
remote networking fs - while in on-demand read mode, CacheFiles can boost the
scenario where on-demand read semantics are needed, e.g. container image
distribution.
(在原始模式下工作时，CacheFiles 充当远程网络文件系统的本地缓存——而在按需读(on-demand read)模式下，CacheFiles 可以增强需要按需读语义的场景，例如容器镜像分发。)

The essential difference between these two modes is seen when a cache miss
occurs: In the original mode, the netfs will fetch the data from the remote
server and then write it to the cache file; in on-demand read mode, fetching
the data and writing it into the cache is delegated to a user daemon.
(这两种模式的本质区别体现在缓存未命中(cache miss)发生时：在原始模式下，netfs 会从远程服务器取回数据，然后把它写入缓存文件；而在按需读模式下，取回数据并写入缓存的工作被委托给一个用户守护进程。)

``CONFIG_CACHEFILES_ONDEMAND`` should be enabled to support on-demand read mode.
(应启用 ``CONFIG_CACHEFILES_ONDEMAND`` 以支持按需读模式。)


Protocol Communication
(协议通信)
----------------------

The on-demand read mode uses a simple protocol for communication between kernel
and user daemon. The protocol can be modeled as::

	kernel --[request]--> user daemon --[reply]--> kernel

(按需读模式使用一个简单的协议在内核与用户守护进程之间通信。该协议可以建模为：)

CacheFiles will send requests to the user daemon when needed.  The user daemon
should poll the devnode ('/dev/cachefiles') to check if there's a pending
request to be processed.  A POLLIN event will be returned when there's a pending
request.
(CacheFiles 会在需要时向用户守护进程发送请求。用户守护进程应轮询设备节点('/dev/cachefiles')以检查是否有待处理的请求。当有待处理的请求时会返回 POLLIN 事件。)

The user daemon then reads the devnode to fetch a request to process.  It should
be noted that each read only gets one request. When it has finished processing
the request, the user daemon should write the reply to the devnode.
(然后，用户守护进程读取设备节点以取回一个要处理的请求。应当注意，每次读取只会得到一个请求。当它处理完该请求后，用户守护进程应把回复写入设备节点。)

Each request starts with a message header of the form::

	struct cachefiles_msg {
		__u32 msg_id;
		__u32 opcode;
		__u32 len;
		__u32 object_id;
		__u8  data[];
	};

(每个请求都以下列形式的消息头开始：)

where:
(其中：)

	* ``msg_id`` is a unique ID identifying this request among all pending
	  requests.
	  (``msg_id`` 是在所有待处理请求中标识本请求的唯一 ID。)

	* ``opcode`` indicates the type of this request.
	  (``opcode`` 指示本请求的类型。)

	* ``object_id`` is a unique ID identifying the cache file operated on.
	  (``object_id`` 是标识所操作缓存文件的唯一 ID。)

	* ``data`` indicates the payload of this request.
	  (``data`` 表示本请求的载荷。)

	* ``len`` indicates the whole length of this request, including the
	  header and following type-specific payload.
	  (``len`` 表示本请求的总长度，包括消息头和其后的类型特定载荷。)


Turning on On-demand Mode
(开启按需模式)
-------------------------

An optional parameter becomes available to the "bind" command::

	bind [ondemand]

("bind" 命令变得可以使用一个可选参数：)

When the "bind" command is given no argument, it defaults to the original mode.
When it is given the "ondemand" argument, i.e. "bind ondemand", on-demand read
mode will be enabled.
(当 "bind" 命令没有给定参数时，默认为原始模式。当它被给定 "ondemand" 参数时，即 "bind ondemand"，将启用按需读模式。)


The OPEN Request
(OPEN 请求)
----------------

When the netfs opens a cache file for the first time, a request with the
CACHEFILES_OP_OPEN opcode, a.k.a an OPEN request will be sent to the user
daemon.  The payload format is of the form::

	struct cachefiles_open {
		__u32 volume_key_size;
		__u32 cookie_key_size;
		__u32 fd;
		__u32 flags;
		__u8  data[];
	};

(当 netfs 第一次打开某个缓存文件时，一个带有 CACHEFILES_OP_OPEN 操作码的请求(即 OPEN 请求)将被发送给用户守护进程。载荷格式如下：)

where:
(其中：)

	* ``data`` contains the volume_key followed directly by the cookie_key.
	  The volume key is a NUL-terminated string; the cookie key is binary
	  data.
	  (``data`` 依次包含 volume_key 以及紧随其后的 cookie_key。volume key 是一个以 NUL 结尾的字符串；cookie key 是二进制数据。)

	* ``volume_key_size`` indicates the size of the volume key in bytes.
	  (``volume_key_size`` 指示 volume key 的大小，以字节为单位。)

	* ``cookie_key_size`` indicates the size of the cookie key in bytes.
	  (``cookie_key_size`` 指示 cookie key 的大小，以字节为单位。)

	* ``fd`` indicates an anonymous fd referring to the cache file, through
	  which the user daemon can perform write/llseek file operations on the
	  cache file.
	  (``fd`` 指示一个引用该缓存文件的匿名文件描述符，用户守护进程可以通过它对该缓存文件执行 write/llseek 文件操作。)


The user daemon can use the given (volume_key, cookie_key) pair to distinguish
the requested cache file.  With the given anonymous fd, the user daemon can
fetch the data and write it to the cache file in the background, even when
kernel has not triggered a cache miss yet.
(用户守护进程可以用给定的 (volume_key, cookie_key) 对来区分所请求的缓存文件。借助给定的匿名文件描述符，用户守护进程可以在后台取回数据并写入缓存文件，即使内核尚未触发缓存未命中也是如此。)

Be noted that each cache file has a unique object_id, while it may have multiple
anonymous fds.  The user daemon may duplicate anonymous fds from the initial
anonymous fd indicated by the @fd field through dup().  Thus each object_id can
be mapped to multiple anonymous fds, while the usr daemon itself needs to
maintain the mapping.
(注意，每个缓存文件都有一个唯一的 object_id，但它可能有多个匿名文件描述符。用户守护进程可以通过 dup() 从 @fd 字段所指示的初始匿名文件描述符复制出更多匿名文件描述符。这样，每个 object_id 都可以映射到多个匿名文件描述符，而用户守护进程自身需要维护这一映射。)

When implementing a user daemon, please be careful of RLIMIT_NOFILE,
``/proc/sys/fs/nr_open`` and ``/proc/sys/fs/file-max``.  Typically these needn't
be huge since they're related to the number of open device blobs rather than
open files of each individual filesystem.
(在实现用户守护进程时，请留意 RLIMIT_NOFILE、``/proc/sys/fs/nr_open`` 和 ``/proc/sys/fs/file-max``。通常这些值不必很大，因为它们与打开的设备描述符的数量相关，而不是与各个文件系统打开的文件数量相关。)

The user daemon should reply the OPEN request by issuing a "copen" (complete
open) command on the devnode::

	copen <msg_id>,<cache_size>

(用户守护进程应通过在设备节点上发出 "copen"(complete open，完成打开)命令来回复 OPEN 请求：)

where:
(其中：)

	* ``msg_id`` must match the msg_id field of the OPEN request.
	  (``msg_id`` 必须与 OPEN 请求的 msg_id 字段相匹配。)

	* When >= 0, ``cache_size`` indicates the size of the cache file;
	  when < 0, ``cache_size`` indicates any error code encountered by the
	  user daemon.
	  (当 >= 0 时，``cache_size`` 指示缓存文件的大小；当 < 0 时，``cache_size`` 指示用户守护进程遇到的任何错误码。)


The CLOSE Request
(CLOSE 请求)
-----------------

When a cookie withdrawn, a CLOSE request (opcode CACHEFILES_OP_CLOSE) will be
sent to the user daemon.  This tells the user daemon to close all anonymous fds
associated with the given object_id.  The CLOSE request has no extra payload,
and shouldn't be replied.
(当某个 cookie 被撤回时，一个 CLOSE 请求(操作码 CACHEFILES_OP_CLOSE)将被发送给用户守护进程。它告诉用户守护进程关闭与给定 object_id 相关联的所有匿名文件描述符。CLOSE 请求没有额外的载荷，也不应回复。)


The READ Request
(READ 请求)
----------------

When a cache miss is encountered in on-demand read mode, CacheFiles will send a
READ request (opcode CACHEFILES_OP_READ) to the user daemon. This tells the user
daemon to fetch the contents of the requested file range.  The payload is of the
form::

	struct cachefiles_read {
		__u64 off;
		__u64 len;
	};

(当在按需读模式下遇到缓存未命中时，CacheFiles 将向用户守护进程发送一个 READ 请求(操作码 CACHEFILES_OP_READ)。它告诉用户守护进程去取回所请求文件范围的内容。载荷形式如下：)

where:
(其中：)

	* ``off`` indicates the starting offset of the requested file range.
	  (``off`` 指示所请求文件范围的起始偏移。)

	* ``len`` indicates the length of the requested file range.
	  (``len`` 指示所请求文件范围的长度。)


When it receives a READ request, the user daemon should fetch the requested data
and write it to the cache file identified by object_id.
(当收到 READ 请求时，用户守护进程应取回所请求的数据，并把它写入由 object_id 标识的缓存文件。)

When it has finished processing the READ request, the user daemon should reply
by using the CACHEFILES_IOC_READ_COMPLETE ioctl on one of the anonymous fds
associated with the object_id given in the READ request.  The ioctl is of the
form::

	ioctl(fd, CACHEFILES_IOC_READ_COMPLETE, msg_id);

(当处理完 READ 请求后，用户守护进程应使用 CACHEFILES_IOC_READ_COMPLETE ioctl，在 READ 请求中给定的 object_id 相关联的某个匿名文件描述符上进行回复。该 ioctl 的形式如下：)

where:
(其中：)

	* ``fd`` is one of the anonymous fds associated with the object_id
	  given.
	  (``fd`` 是与给定 object_id 相关联的匿名文件描述符之一。)

	* ``msg_id`` must match the msg_id field of the READ request.
	  (``msg_id`` 必须与 READ 请求的 msg_id 字段相匹配。)
