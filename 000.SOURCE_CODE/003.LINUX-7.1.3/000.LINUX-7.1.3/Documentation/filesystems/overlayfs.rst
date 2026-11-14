.. SPDX-License-Identifier: GPL-2.0

Written by: Neil Brown
(作者: Neil Brown)
Please see MAINTAINERS file for where to send questions.
(问题反馈地址请参见 MAINTAINERS 文件。)

Overlay Filesystem
(Overlay 文件系统)
==================

This document describes a prototype for a new approach to providing
overlay-filesystem functionality in Linux (sometimes referred to as
union-filesystems).  An overlay-filesystem tries to present a
filesystem which is the result of overlaying one filesystem on top
of the other.
(本文档描述一种在 Linux 中提供 overlay 文件系统功能(有时称为联合文件系统，union-filesystems)的新方法的原型。overlay 文件系统试图呈现一个文件系统，它是把一个文件系统叠加到另一个文件系统之上得到的结果。)


Overlay objects
(Overlay 对象)
---------------

The overlay filesystem approach is 'hybrid', because the objects that
appear in the filesystem do not always appear to belong to that filesystem.
In many cases, an object accessed in the union will be indistinguishable
from accessing the corresponding object from the original filesystem.
This is most obvious from the 'st_dev' field returned by stat(2).
(overlay 文件系统的方法是"混合式"的，因为出现在该文件系统中的对象并不总是看起来属于该文件系统。在许多情况下，在联合(union)中访问一个对象与从原始文件系统访问对应对象是无法区分的。这一点从 stat(2) 返回的 'st_dev' 字段可以最明显地看出。)

While directories will report an st_dev from the overlay-filesystem,
non-directory objects may report an st_dev from the lower filesystem or
upper filesystem that is providing the object.  Similarly st_ino will
only be unique when combined with st_dev, and both of these can change
over the lifetime of a non-directory object.  Many applications and
tools ignore these values and will not be affected.
(目录会报告来自 overlay 文件系统的 st_dev，而非目录对象可能报告来自提供该对象的下层文件系统或上层文件系统的 st_dev。类似地，st_ino 只有与 st_dev 组合时才是唯一的，并且对于非目录对象，这两者在对象的生命周期内都可能改变。许多应用程序和工具会忽略这些值，因此不会受到影响。)

In the special case of all overlay layers on the same underlying
filesystem, all objects will report an st_dev from the overlay
filesystem and st_ino from the underlying filesystem.  This will
make the overlay mount more compliant with filesystem scanners and
overlay objects will be distinguishable from the corresponding
objects in the original filesystem.
(在所有 overlay 层都位于同一底层文件系统的特殊情况下，所有对象都会报告来自 overlay 文件系统的 st_dev 和来自底层文件系统的 st_ino。这将使 overlay 挂载更符合文件系统扫描器的要求，并且 overlay 对象将可以与原始文件系统中的对应对象区分开来。)

On 64bit systems, even if all overlay layers are not on the same
underlying filesystem, the same compliant behavior could be achieved
with the "xino" feature.  The "xino" feature composes a unique object
identifier from the real object st_ino and an underlying fsid number.
The "xino" feature uses the high inode number bits for fsid, because the
underlying filesystems rarely use the high inode number bits.  In case
the underlying inode number does overflow into the high xino bits, overlay
filesystem will fall back to the non xino behavior for that inode.
(在 64 位系统上，即使所有 overlay 层不在同一底层文件系统上，也可以通过 "xino" 特性实现同样的兼容行为。"xino" 特性由真实对象的 st_ino 和一个底层 fsid 编号组合出一个唯一的对象标识符。"xino" 特性将 inode 编号的高位用作 fsid，因为底层文件系统很少使用 inode 编号的高位。如果底层 inode 编号确实溢出到 xino 高位，overlay 文件系统会针对该 inode 回退到非 xino 行为。)

The "xino" feature can be enabled with the "-o xino=on" overlay mount option.
If all underlying filesystems support NFS file handles, the value of st_ino
for overlay filesystem objects is not only unique, but also persistent over
the lifetime of the filesystem.  The "-o xino=auto" overlay mount option
enables the "xino" feature only if the persistent st_ino requirement is met.
("xino" 特性可以通过 overlay 挂载选项 "-o xino=on" 启用。如果所有底层文件系统都支持 NFS 文件句柄，则 overlay 文件系统对象的 st_ino 值不仅唯一，而且在文件系统的生命周期内是持久的。"-o xino=auto" overlay 挂载选项只在满足 st_ino 持久性要求时才启用 "xino" 特性。)

The following table summarizes what can be expected in different overlay
configurations.
(下表总结了在不同的 overlay 配置下可以预期的情况。)

Inode properties
````````````````
(inode 属性)

+--------------+------------+------------+-----------------+----------------+
|Configuration | Persistent | Uniform    | st_ino == d_ino | d_ino == i_ino |
|  (配置)      | st_ino     | st_dev     |                 | [*]            |
|              | (持久)     | (统一)     |                 |                |
+==============+=====+======+=====+======+========+========+========+=======+
|              | dir | !dir | dir | !dir |  dir   |  !dir  |  dir   | !dir  |
+--------------+-----+------+-----+------+--------+--------+--------+-------+
| All layers   |  Y  |  Y   |  Y  |  Y   |  Y     |   Y    |  Y     |  Y    |
| on same fs   |     |      |     |      |        |        |        |       |
| (所有层都在  |     |      |     |      |        |        |        |       |
| 同一文件系统)|     |      |     |      |        |        |        |       |
+--------------+-----+------+-----+------+--------+--------+--------+-------+
| Layers not   |  N  |  N   |  Y  |  N   |  N     |   Y    |  N     |  Y    |
| on same fs,  |     |      |     |      |        |        |        |       |
| xino=off     |     |      |     |      |        |        |        |       |
| (各层不在    |     |      |     |      |        |        |        |       |
| 同一文件系统,|     |      |     |      |        |        |        |       |
| xino=off)    |     |      |     |      |        |        |        |       |
+--------------+-----+------+-----+------+--------+--------+--------+-------+
| xino=on/auto |  Y  |  Y   |  Y  |  Y   |  Y     |   Y    |  Y     |  Y    |
+--------------+-----+------+-----+------+--------+--------+--------+-------+
| xino=on/auto,|  N  |  N   |  Y  |  N   |  N     |   Y    |  N     |  Y    |
| ino overflow |     |      |     |      |        |        |        |       |
| (ino 溢出)   |     |      |     |      |        |        |        |       |
+--------------+-----+------+-----+------+--------+--------+--------+-------+

[*] nfsd v3 readdirplus verifies d_ino == i_ino. i_ino is exposed via several
/proc files, such as /proc/locks and /proc/self/fdinfo/<fd> of an inotify
file descriptor.
([*] nfsd v3 readdirplus 会验证 d_ino == i_ino。i_ino 通过若干 /proc 文件暴露，例如 /proc/locks 以及 inotify 文件描述符的 /proc/self/fdinfo/<fd>。)

Upper and Lower
(上层与下层)
---------------

An overlay filesystem combines two filesystems - an 'upper' filesystem
and a 'lower' filesystem.  When a name exists in both filesystems, the
object in the 'upper' filesystem is visible while the object in the
'lower' filesystem is either hidden or, in the case of directories,
merged with the 'upper' object.
(overlay 文件系统组合两个文件系统——"上层(upper)"文件系统和"下层(lower)"文件系统。当一个名字在两个文件系统中都存在时，"上层"文件系统中的对象可见，而"下层"文件系统中的对象要么被隐藏，要么(对于目录而言)与"上层"对象合并。)

It would be more correct to refer to an upper and lower 'directory
tree' rather than 'filesystem' as it is quite possible for both
directory trees to be in the same filesystem and there is no
requirement that the root of a filesystem be given for either upper or
lower.
(更准确的说法应是"上层目录树"和"下层目录树"，而不是"文件系统"，因为两个目录树完全可能位于同一个文件系统中，而且对于上层或下层，并不要求给出文件系统的根。)

A wide range of filesystems supported by Linux can be the lower filesystem,
but not all filesystems that are mountable by Linux have the features
needed for OverlayFS to work.  The lower filesystem does not need to be
writable.  The lower filesystem can even be another overlayfs.  The upper
filesystem will normally be writable and if it is it must support the
creation of trusted.* and/or user.* extended attributes, and must provide
valid d_type in readdir responses, so NFS is not suitable.
(Linux 支持的众多文件系统都可以作为下层文件系统，但并非所有可被 Linux 挂载的文件系统都具有 OverlayFS 工作所需的特性。下层文件系统不需要可写。下层文件系统甚至可以是另一个 overlayfs。上层文件系统通常需要可写，如果可写，它必须支持创建 trusted.* 和/或 user.* 扩展属性，并且必须在 readdir 响应中提供有效的 d_type，因此 NFS 不适用。)

A read-only overlay of two read-only filesystems may use any
filesystem type.
(两个只读文件系统的只读 overlay 可以使用任何文件系统类型。)

Directories
(目录)
-----------

Overlaying mainly involves directories.  If a given name appears in both
upper and lower filesystems and refers to a non-directory in either,
then the lower object is hidden - the name refers only to the upper
object.
(叠加(overlay)主要涉及目录。如果给定的名字同时出现在上层和下层文件系统中，并且在其中任一中指向一个非目录对象，那么下层对象将被隐藏——该名字只指向上层对象。)

Where both upper and lower objects are directories, a merged directory
is formed.
(当上层和下层对象都是目录时，会形成一个合并目录(merged directory)。)

At mount time, the two directories given as mount options "lowerdir" and
"upperdir" are combined into a merged directory::
(挂载时，作为挂载选项 "lowerdir" 和 "upperdir" 给出的两个目录被组合成一个合并目录：)

  mount -t overlay overlay -olowerdir=/lower,upperdir=/upper,\
  workdir=/work /merged

The "workdir" needs to be an empty directory on the same filesystem
as upperdir.
("workdir" 必须是与 upperdir 位于同一文件系统上的一个空目录。)

Then whenever a lookup is requested in such a merged directory, the
lookup is performed in each actual directory and the combined result
is cached in the dentry belonging to the overlay filesystem.  If both
actual lookups find directories, both are stored and a merged
directory is created, otherwise only one is stored: the upper if it
exists, else the lower.
(之后，每当在这样的合并目录中请求查找(lookup)时，查找会在每个实际目录中执行，组合后的结果缓存在属于 overlay 文件系统的 dentry 中。如果两个实际查找都找到了目录，则两者都被保存并创建一个合并目录；否则只保存其中一个：如果上层存在则是上层，否则是下层。)

Only the lists of names from directories are merged.  Other content
such as metadata and extended attributes are reported for the upper
directory only.  These attributes of the lower directory are hidden.
(只有目录中的名字列表会被合并。其他内容(如元数据和扩展属性)只针对上层目录报告。下层目录的这些属性被隐藏。)

whiteouts and opaque directories
(whiteouts 与不透明目录)
--------------------------------

In order to support rm and rmdir without changing the lower
filesystem, an overlay filesystem needs to record in the upper filesystem
that files have been removed.  This is done using whiteouts and opaque
directories (non-directories are always opaque).
(为了在不更改下层文件系统的情况下支持 rm 和 rmdir，overlay 文件系统需要在上层文件系统中记录文件已被删除。这是通过 whiteout(遮挡文件)和不透明目录(opaque directory)实现的(非目录对象总是不透明的)。)

A whiteout is created as a character device with 0/0 device number or
as a zero-size regular file with the xattr "trusted.overlay.whiteout".
(whiteout 以设备号为 0/0 的字符设备形式创建，或者以带有 xattr "trusted.overlay.whiteout" 的零大小普通文件形式创建。)

When a whiteout is found in the upper level of a merged directory, any
matching name in the lower level is ignored, and the whiteout itself
is also hidden.
(当在合并目录的上层发现一个 whiteout 时，下层中任何匹配的名字都会被忽略，而且 whiteout 本身也是隐藏的。)

A directory is made opaque by setting the xattr "trusted.overlay.opaque"
to "y".  Where the upper filesystem contains an opaque directory, any
directory in the lower filesystem with the same name is ignored.
(通过将 xattr "trusted.overlay.opaque" 设置为 "y" 可使目录变为不透明。当上层文件系统包含一个不透明目录时，下层文件系统中同名的任何目录都会被忽略。)

An opaque directory should not contain any whiteouts, because they do not
serve any purpose.  A merge directory containing regular files with the xattr
"trusted.overlay.whiteout", should be additionally marked by setting the xattr
"trusted.overlay.opaque" to "x" on the merge directory itself.
This is needed to avoid the overhead of checking the "trusted.overlay.whiteout"
on all entries during readdir in the common case.
(不透明目录不应包含任何 whiteout，因为它们没有任何用处。包含带有 xattr "trusted.overlay.whiteout" 的普通文件的合并(merge)目录，应额外通过在该合并目录自身上把 xattr "trusted.overlay.opaque" 设置为 "x" 来标记。这样做是为了避免在常见情况下 readdir 期间对所有目录项检查 "trusted.overlay.whiteout" 的开销。)

readdir
(readdir 的行为)
-------

When a 'readdir' request is made on a merged directory, the upper and
lower directories are each read and the name lists merged in the
obvious way (upper is read first, then lower - entries that already
exist are not re-added).  This merged name list is cached in the
'struct file' and so remains as long as the file is kept open.  If the
directory is opened and read by two processes at the same time, they
will each have separate caches.  A seekdir to the start of the
directory (offset 0) followed by a readdir will cause the cache to be
discarded and rebuilt.
(当在合并目录上发起 'readdir' 请求时，上层和下层目录各自被读取，名字列表以显而易见的方式合并(先读上层，再读下层——已存在的项不会被重复添加)。合并后的名字列表缓存在 'struct file' 中，因此只要文件保持打开就一直存在。如果目录同时被两个进程打开并读取，它们将各自拥有独立的缓存。seekdir 到目录开头(偏移 0)之后再执行 readdir，会导致缓存被丢弃并重建。)

This means that changes to the merged directory do not appear while a
directory is being read.  This is unlikely to be noticed by many
programs.
(这意味着在读取目录期间，对合并目录的更改不会显现出来。很多程序不太可能注意到这一点。)

seek offsets are assigned sequentially when the directories are read.
Thus if:
(目录被读取时，seek 偏移量是按顺序分配的。因此，如果：)

 - read part of a directory
   (读取目录的一部分)
 - remember an offset, and close the directory
   (记住一个偏移量，然后关闭目录)
 - re-open the directory some time later
   (稍后重新打开该目录)
 - seek to the remembered offset
   (seek 到记住的偏移量)

there may be little correlation between the old and new locations in
the list of filenames, particularly if anything has changed in the
directory.
(文件名列表中的旧位置与新位置之间可能几乎没有关联，尤其是当目录中发生了任何变化时。)

Readdir on directories that are not merged is simply handled by the
underlying directory (upper or lower).
(对于未合并的目录，readdir 直接由底层目录(上层或下层)处理。)

renaming directories
(重命名目录)
--------------------

When renaming a directory that is on the lower layer or merged (i.e. the
directory was not created on the upper layer to start with) overlayfs can
handle it in two different ways:
(当重命名位于下层或已合并的目录时(即该目录一开始并非在上层创建)，overlayfs 可以用两种不同的方式处理：)

1. return EXDEV error: this error is returned by rename(2) when trying to
   move a file or directory across filesystem boundaries.  Hence
   applications are usually prepared to handle this error (mv(1) for example
   recursively copies the directory tree).  This is the default behavior.
   (返回 EXDEV 错误：当尝试跨文件系统边界移动文件或目录时，rename(2) 会返回此错误。因此应用程序通常已准备好处理此错误(例如 mv(1) 会递归地复制目录树)。这是默认行为。)

2. If the "redirect_dir" feature is enabled, then the directory will be
   copied up (but not the contents).  Then the "trusted.overlay.redirect"
   extended attribute is set to the path of the original location from the
   root of the overlay.  Finally the directory is moved to the new
   location.
   (如果启用了 "redirect_dir" 特性，则该目录会被复制上去(copy up)(但内容不会)。然后 "trusted.overlay.redirect" 扩展属性被设置为从 overlay 根算起的原始位置路径。最后该目录被移动到新位置。)

There are several ways to tune the "redirect_dir" feature.
(有几种方式可以调整 "redirect_dir" 特性。)

Kernel config options:
(内核配置选项：)

- OVERLAY_FS_REDIRECT_DIR:
    If this is enabled, then redirect_dir is turned on by  default.
    (如果启用此项，则 redirect_dir 默认开启。)
- OVERLAY_FS_REDIRECT_ALWAYS_FOLLOW:
    If this is enabled, then redirects are always followed by default. Enabling
    this results in a less secure configuration.  Enable this option only when
    worried about backward compatibility with kernels that have the redirect_dir
    feature and follow redirects even if turned off.
    (如果启用此项，则默认总是跟随重定向。启用它会导致安全性较低的配置。仅当担心与具有 redirect_dir 特性、且即使被关闭也会跟随重定向的内核保持向后兼容时，才启用此选项。)

Module options (can also be changed through /sys/module/overlay/parameters/):
(模块选项(也可以通过 /sys/module/overlay/parameters/ 更改)：)

- "redirect_dir=BOOL":
    See OVERLAY_FS_REDIRECT_DIR kernel config option above.
    (参见上面的 OVERLAY_FS_REDIRECT_DIR 内核配置选项。)
- "redirect_always_follow=BOOL":
    See OVERLAY_FS_REDIRECT_ALWAYS_FOLLOW kernel config option above.
    (参见上面的 OVERLAY_FS_REDIRECT_ALWAYS_FOLLOW 内核配置选项。)
- "redirect_max=NUM":
    The maximum number of bytes in an absolute redirect (default is 256).
    (绝对重定向的最大字节数(默认为 256)。)

Mount options:
(挂载选项：)

- "redirect_dir=on":
    Redirects are enabled.
    (重定向被启用。)
- "redirect_dir=follow":
    Redirects are not created, but followed.
    (不创建重定向，但会跟随。)
- "redirect_dir=nofollow":
    Redirects are not created and not followed.
    (不创建重定向，也不跟随。)
- "redirect_dir=off":
    If "redirect_always_follow" is enabled in the kernel/module config,
    this "off" translates to "follow", otherwise it translates to "nofollow".
    (如果在内核/模块配置中启用了 "redirect_always_follow"，则此 "off" 相当于 "follow"，否则相当于 "nofollow"。)

When the NFS export feature is enabled, every copied up directory is
indexed by the file handle of the lower inode and a file handle of the
upper directory is stored in a "trusted.overlay.upper" extended attribute
on the index entry.  On lookup of a merged directory, if the upper
directory does not match the file handle stores in the index, that is an
indication that multiple upper directories may be redirected to the same
lower directory.  In that case, lookup returns an error and warns about
a possible inconsistency.
(当启用 NFS 导出特性时，每个被复制上去(copy up)的目录都会由下层 inode 的文件句柄索引，并且上层目录的文件句柄存储在索引项的 "trusted.overlay.upper" 扩展属性中。在查找合并目录时，如果上层目录与索引中存储的文件句柄不匹配，这表明可能有多个上层目录被重定向到了同一个下层目录。在这种情况下，查找返回错误并警告可能存在不一致。)

Because lower layer redirects cannot be verified with the index, enabling
NFS export support on an overlay filesystem with no upper layer requires
turning off redirect follow (e.g. "redirect_dir=nofollow").
(由于下层重定向无法用索引来验证，在没有上层的 overlay 文件系统上启用 NFS 导出支持需要关闭重定向跟随(例如 "redirect_dir=nofollow")。)


Non-directories
(非目录对象)
---------------

Objects that are not directories (files, symlinks, device-special
files etc.) are presented either from the upper or lower filesystem as
appropriate.  When a file in the lower filesystem is accessed in a way
that requires write-access, such as opening for write access, changing
some metadata etc., the file is first copied from the lower filesystem
to the upper filesystem (copy_up).  Note that creating a hard-link
also requires copy_up, though of course creation of a symlink does
not.
(非目录对象(文件、符号链接、设备特殊文件等)会视情况从上层或下层文件系统呈现。当下层文件系统中的文件以需要写访问权限的方式被访问时(例如以写方式打开、更改某些元数据等)，该文件会先从下层文件系统复制到上层文件系统(copy-up)。注意，创建硬链接也需要 copy-up，当然创建符号链接不需要。)

The copy_up may turn out to be unnecessary, for example if the file is
opened for read-write but the data is not modified.
(copy_up 最终可能是不必要的，例如文件以读写方式打开但数据并未被修改。)

The copy_up process first makes sure that the containing directory
exists in the upper filesystem - creating it and any parents as
necessary.  It then creates the object with the same metadata (owner,
mode, mtime, symlink-target etc.) and then if the object is a file, the
data is copied from the lower to the upper filesystem.  Finally any
extended attributes are copied up.
(copy_up 过程首先确保其所在目录存在于上层文件系统中——如有必要会创建它及各级父目录。然后以相同的元数据(所有者、权限模式、mtime、符号链接目标等)创建该对象；如果对象是文件，则数据会从下层复制到上层文件系统。最后，所有扩展属性都会被复制上去。)

Once the copy_up is complete, the overlay filesystem simply
provides direct access to the newly created file in the upper
filesystem - future operations on the file are barely noticed by the
overlay filesystem (though an operation on the name of the file such as
rename or unlink will of course be noticed and handled).
(copy_up 完成后，overlay 文件系统只是提供对上层文件系统中新建文件的直接访问——之后对该文件的操作几乎不会被 overlay 文件系统察觉(当然，对文件名字本身的操作(如 rename 或 unlink)会被察觉并处理)。)


Permission model
(权限模型)
----------------

An overlay filesystem stashes credentials that will be used when
accessing lower or upper filesystems.
(overlay 文件系统会保存一组凭据(credential)，用于访问下层或上层文件系统。)

In the old mount api the credentials of the task calling mount(2) are
stashed. In the new mount api the credentials of the task creating the
superblock through FSCONFIG_CMD_CREATE command of fsconfig(2) are
stashed.
(在旧挂载 API 中，保存的是调用 mount(2) 的任务的凭据。在新挂载 API 中，保存的是通过 fsconfig(2) 的 FSCONFIG_CMD_CREATE 命令创建超级块的任务的凭据。)

Starting with kernel v6.15 it is possible to use the "override_creds"
mount option which will cause the credentials of the calling task to be
recorded. Note that "override_creds" is only meaningful when used with
the new mount api as the old mount api combines setting options and
superblock creation in a single mount(2) syscall.
(从内核 v6.15 开始，可以使用 "override_creds" 挂载选项，它会使调用任务的凭据被记录。注意，"override_creds" 只在新挂载 API 下才有意义，因为旧挂载 API 在单个 mount(2) 系统调用中同时完成选项设置和超级块创建。)

Permission checking in the overlay filesystem follows these principles:
(overlay 文件系统中的权限检查遵循以下原则：)

 1) permission check SHOULD return the same result before and after copy up
    (权限检查在 copy up 之前和之后应当(SHOULD)返回相同的结果)

 2) task creating the overlay mount MUST NOT gain additional privileges
    (创建 overlay 挂载的任务绝不能(MUST NOT)获得额外特权)

 3) task[*] MAY gain additional privileges through the overlay,
    compared to direct access on underlying lower or upper filesystems
    (与直接访问底层下层或上层文件系统相比，任务[*]可以(MAY)通过 overlay 获得额外特权)

This is achieved by performing two permission checks on each access:
(这是通过对每次访问执行两次权限检查来实现的：)

 a) check if current task is allowed access based on local DAC (owner,
    group, mode and posix acl), as well as MAC checks
    (基于本地 DAC(所有者、组、权限模式和 posix acl)以及 MAC 检查，检查当前任务是否被允许访问)

 b) check if stashed credentials would be allowed real operation on lower or
    upper layer based on underlying filesystem permissions, again including
    MAC checks
    (基于底层文件系统权限，检查保存的凭据是否被允许在下层或上层执行实际操作，同样包括 MAC 检查)

Check (a) ensures consistency (1) since owner, group, mode and posix acls
are copied up.  On the other hand it can result in server enforced
permissions (used by NFS, for example) being ignored (3).
(检查 (a) 保证了原则 (1) 的一致性，因为所有者、组、权限模式和 posix acl 都会被复制上去。另一方面，它可能导致服务器强制实施的权限(例如 NFS 所使用的)被忽略(原则 (3))。)

Check (b) ensures that no task gains permissions to underlying layers that
the stashed credentials do not have (2).  This also means that it is possible
to create setups where the consistency rule (1) does not hold; normally,
however, the stashed credentials will have sufficient privileges to
perform all operations.
(检查 (b) 确保任何任务都不会获得保存的凭据所不具备的底层层访问权限(原则 (2))。这也意味着可以构造出一致性规则 (1) 不成立的配置；不过通常情况下，保存的凭据会拥有执行所有操作所需的足够特权。)

Another way to demonstrate this model is drawing parallels between::
(演示该模型的另一种方式是在以下两者之间做类比：)

  mount -t overlay overlay -olowerdir=/lower,upperdir=/upper,... /merged

and::
(与：)

  cp -a /lower /upper
  mount --bind /upper /merged

The resulting access permissions should be the same.  The difference is in
the time of copy (on-demand vs. up-front).
(两者产生的访问权限应当相同。区别在于复制的时机(按需 vs. 预先)。)


Multiple lower layers
(多个下层)
---------------------

Multiple lower layers can now be given using the colon (":") as a
separator character between the directory names.  For example::
(现在可以使用冒号(":")作为目录名之间的分隔字符来指定多个下层。例如：)

  mount -t overlay overlay -olowerdir=/lower1:/lower2:/lower3 /merged

As the example shows, "upperdir=" and "workdir=" may be omitted.  In
that case the overlay will be read-only.
(如示例所示，"upperdir=" 和 "workdir=" 可以省略。在这种情况下，overlay 将是只读的。)

The specified lower directories will be stacked beginning from the
rightmost one and going left.  In the above example lower1 will be the
top, lower2 the middle and lower3 the bottom layer.
(指定的下层目录将从最右边的一个开始向左依次堆叠。在上面的例子中，lower1 将是最顶层，lower2 是中间层，lower3 是最底层。)

Note: directory names containing colons can be provided as lower layer by
escaping the colons with a single backslash.  For example::
(注意：包含冒号的目录名可以通过用单个反斜杠转义冒号来作为下层提供。例如：)

  mount -t overlay overlay -olowerdir=/a\:lower\:\:dir /merged

Since kernel version v6.8, directory names containing colons can also
be configured as lower layer using the "lowerdir+" mount options and the
fsconfig syscall from new mount api.  For example::
(自内核版本 v6.8 起，包含冒号的目录名还可以使用 "lowerdir+" 挂载选项和新挂载 API 的 fsconfig 系统调用配置为下层。例如：)

  fsconfig(fs_fd, FSCONFIG_SET_STRING, "lowerdir+", "/a:lower::dir", 0);

In the latter case, colons in lower layer directory names will be escaped
as an octal characters (\072) when displayed in /proc/self/mountinfo.
(在后一种情况下，下层目录名中的冒号在 /proc/self/mountinfo 中显示时会被转义为八进制字符(\072)。)

Metadata only copy up
(仅元数据 copy up)
---------------------

When the "metacopy" feature is enabled, overlayfs will only copy
up metadata (as opposed to whole file), when a metadata specific operation
like chown/chmod is performed. An upper file in this state is marked with
"trusted.overlayfs.metacopy" xattr which indicates that the upper file
contains no data.  The data will be copied up later when file is opened for
WRITE operation.  After the lower file's data is copied up,
the "trusted.overlayfs.metacopy" xattr is removed from the upper file.
(当 "metacopy" 特性启用时，当执行 chown/chmod 等元数据特定的操作时，overlayfs 将只复制元数据(而不是整个文件)。处于这种状态的上层文件用 "trusted.overlayfs.metacopy" xattr 标记，表明该上层文件不包含数据。数据会在之后文件以 WRITE 方式打开时才被复制上去。下层文件的数据被复制上去之后，"trusted.overlayfs.metacopy" xattr 会从上层文件中移除。)

In other words, this is delayed data copy up operation and data is copied
up when there is a need to actually modify data.
(换句话说，这是延迟的数据 copy up 操作，只有在需要实际修改数据时数据才被复制上去。)

There are multiple ways to enable/disable this feature. A config option
CONFIG_OVERLAY_FS_METACOPY can be set/unset to enable/disable this feature
by default. Or one can enable/disable it at module load time with module
parameter metacopy=on/off. Lastly, there is also a per mount option
metacopy=on/off to enable/disable this feature per mount.
(有多种方式启用/禁用此特性。配置选项 CONFIG_OVERLAY_FS_METACOPY 可以被设置/取消以默认启用/禁用此特性。或者可以在模块加载时通过模块参数 metacopy=on/off 启用/禁用它。最后，还有每挂载选项 metacopy=on/off 来按挂载启用/禁用此特性。)

Do not use metacopy=on with untrusted upper/lower directories. Otherwise
it is possible that an attacker can create a handcrafted file with
appropriate REDIRECT and METACOPY xattrs, and gain access to file on lower
pointed by REDIRECT. This should not be possible on local system as setting
"trusted." xattrs will require CAP_SYS_ADMIN. But it should be possible
for untrusted layers like from a pen drive.
(不要在不受信任的上层/下层目录上使用 metacopy=on。否则，攻击者有可能创建一个带有适当 REDIRECT 和 METACOPY xattr 的手工构造文件，并获得对 REDIRECT 所指向的下层文件的访问。在本地系统上这应该不可能发生，因为设置 "trusted." xattr 需要 CAP_SYS_ADMIN。但对于来自 U 盘等不受信任的层，这应该是可能的。)

Note: redirect_dir={off|nofollow|follow[*]} and nfs_export=on mount options
conflict with metacopy=on, and will result in an error.
(注意：redirect_dir={off|nofollow|follow[*]} 和 nfs_export=on 挂载选项与 metacopy=on 冲突，会导致错误。)

[*] redirect_dir=follow only conflicts with metacopy=on if upperdir=... is
given.
([*] 只有在给出 upperdir=... 时，redirect_dir=follow 才与 metacopy=on 冲突。)


Data-only lower layers
(仅数据下层)
----------------------

With "metacopy" feature enabled, an overlayfs regular file may be a composition
of information from up to three different layers:
(在 "metacopy" 特性启用的情况下，一个 overlayfs 普通文件可能是由最多三个不同层的信息组合而成的：)

 1) metadata from a file in the upper layer
    (来自上层中某个文件的元数据)

 2) st_ino and st_dev object identifier from a file in a lower layer
    (来自下层中某个文件的 st_ino 和 st_dev 对象标识符)

 3) data from a file in another lower layer (further below)
    (来自另一个(更靠下的)下层中某个文件的数据)

The "lower data" file can be on any lower layer, except from the top most
lower layer.
("下层数据"文件可以位于任何下层上，但最顶层的下层除外。)

Below the topmost lower layer, any number of lowermost layers may be defined
as "data-only" lower layers, using double colon ("::") separators.
A normal lower layer is not allowed to be below a data-only layer, so single
colon separators are not allowed to the right of double colon ("::") separators.
(在最顶层的下层之下，可以使用双冒号("::")分隔符将任意数量的最底层定义为"仅数据(data-only)"下层。普通下层不允许位于仅数据层之下，因此单冒号分隔符不允许出现在双冒号("::")分隔符的右侧。)


For example::
(例如：)

  mount -t overlay overlay -olowerdir=/l1:/l2:/l3::/do1::/do2 /merged

The paths of files in the "data-only" lower layers are not visible in the
merged overlayfs directories and the metadata and st_ino/st_dev of files
in the "data-only" lower layers are not visible in overlayfs inodes.
("仅数据"下层中文件的路径在合并后的 overlayfs 目录中不可见，且"仅数据"下层中文件的元数据和 st_ino/st_dev 在 overlayfs inode 中不可见。)

Only the data of the files in the "data-only" lower layers may be visible
when a "metacopy" file in one of the lower layers above it, has a "redirect"
to the absolute path of the "lower data" file in the "data-only" lower layer.
(只有当其上方某个下层中的 "metacopy" 文件带有指向"仅数据"下层中"下层数据"文件绝对路径的 "redirect" 时，"仅数据"下层中文件的数据才可能可见。)

Instead of explicitly enabling "metacopy=on" it is sufficient to specify at
least one data-only layer to enable redirection of data to a data-only layer.
In this case other forms of metacopy are rejected.  Note: this way, data-only
layers may be used together with "userxattr", in which case careful attention
must be given to privileges needed to change the "user.overlay.redirect" xattr
to prevent misuse.
(无需显式启用 "metacopy=on"，只要指定至少一个仅数据层即可启用将数据重定向到仅数据层。在这种情况下，其他形式的 metacopy 会被拒绝。注意：通过这种方式，仅数据层可以与 "userxattr" 一起使用，此时必须注意更改 "user.overlay.redirect" xattr 所需的权限以防止滥用。)

Since kernel version v6.8, "data-only" lower layers can also be added using
the "datadir+" mount options and the fsconfig syscall from new mount api.
For example::
(自内核版本 v6.8 起，"仅数据"下层还可以使用 "datadir+" 挂载选项和新挂载 API 的 fsconfig 系统调用来添加。例如：)

  fsconfig(fs_fd, FSCONFIG_SET_STRING, "lowerdir+", "/l1", 0);
  fsconfig(fs_fd, FSCONFIG_SET_STRING, "lowerdir+", "/l2", 0);
  fsconfig(fs_fd, FSCONFIG_SET_STRING, "lowerdir+", "/l3", 0);
  fsconfig(fs_fd, FSCONFIG_SET_STRING, "datadir+", "/do1", 0);
  fsconfig(fs_fd, FSCONFIG_SET_STRING, "datadir+", "/do2", 0);


Specifying layers via file descriptors
(通过文件描述符指定层)
--------------------------------------

Since kernel v6.13, overlayfs supports specifying layers via file descriptors in
addition to specifying them as paths. This feature is available for the
"datadir+", "lowerdir+", "upperdir", and "workdir+" mount options with the
fsconfig syscall from the new mount api::
(自内核 v6.13 起，除了以路径方式指定层之外，overlayfs 还支持通过文件描述符指定层。该特性可用于新挂载 API 的 fsconfig 系统调用的 "datadir+"、"lowerdir+"、"upperdir" 和 "workdir+" 挂载选项：)

  fsconfig(fs_fd, FSCONFIG_SET_FD, "lowerdir+", NULL, fd_lower1);
  fsconfig(fs_fd, FSCONFIG_SET_FD, "lowerdir+", NULL, fd_lower2);
  fsconfig(fs_fd, FSCONFIG_SET_FD, "lowerdir+", NULL, fd_lower3);
  fsconfig(fs_fd, FSCONFIG_SET_FD, "datadir+", NULL, fd_data1);
  fsconfig(fs_fd, FSCONFIG_SET_FD, "datadir+", NULL, fd_data2);
  fsconfig(fs_fd, FSCONFIG_SET_FD, "workdir", NULL, fd_work);
  fsconfig(fs_fd, FSCONFIG_SET_FD, "upperdir", NULL, fd_upper);


fs-verity support
(fs-verity 支持)
-----------------

During metadata copy up of a lower file, if the source file has
fs-verity enabled and overlay verity support is enabled, then the
digest of the lower file is added to the "trusted.overlay.metacopy"
xattr. This is then used to verify the content of the lower file
each the time the metacopy file is opened.
(在下层文件的元数据 copy up 期间，如果源文件启用了 fs-verity 且 overlay 的 verity 支持已启用，则下层文件的摘要会被添加到 "trusted.overlay.metacopy" xattr 中。之后每次打开 metacopy 文件时，都会用它来验证下层文件的内容。)

When a layer containing verity xattrs is used, it means that any such
metacopy file in the upper layer is guaranteed to match the content
that was in the lower at the time of the copy-up. If at any time
(during a mount, after a remount, etc) such a file in the lower is
replaced or modified in any way, access to the corresponding file in
overlayfs will result in EIO errors (either on open, due to overlayfs
digest check, or from a later read due to fs-verity) and a detailed
error is printed to the kernel logs. For more details of how fs-verity
file access works, see :ref:`Documentation/filesystems/fsverity.rst
<accessing_verity_files>`.
(当使用了包含 verity xattr 的层时，意味着上层中任何这样的 metacopy 文件都保证与 copy-up 时下层中的内容匹配。如果任何时候(在挂载期间、重新挂载之后等)下层中的这样的文件被以任何方式替换或修改，对 overlayfs 中对应文件的访问将导致 EIO 错误(或者由于 overlayfs 摘要检查而在打开时出错，或者由于 fs-verity 而在之后的读取中出错)，并且会在内核日志中打印详细的错误信息。关于 fs-verity 文件访问工作方式的更多细节，参见 Documentation/filesystems/fsverity.rst 的"访问 verity 文件"一节。)

Verity can be used as a general robustness check to detect accidental
changes in the overlayfs directories in use. But, with additional care
it can also give more powerful guarantees. For example, if the upper
layer is fully trusted (by using dm-verity or something similar), then
an untrusted lower layer can be used to supply validated file content
for all metacopy files.  If additionally the untrusted lower
directories are specified as "Data-only", then they can only supply
such file content, and the entire mount can be trusted to match the
upper layer.
(verity 可以作为一种通用的健壮性检查，用于检测正在使用的 overlayfs 目录中的意外更改。但如果额外小心，它还可以提供更强的保证。例如，如果上层完全可信(通过使用 dm-verity 或类似机制)，则可以使用不受信任的下层为所有 metacopy 文件提供经过验证的文件内容。如果另外将不受信任的下层目录指定为"仅数据"，那么它们只能提供这样的文件内容，并且整个挂载都可以被信任为与上层匹配。)

This feature is controlled by the "verity" mount option, which
supports these values:
(此特性由 "verity" 挂载选项控制，该选项支持以下取值：)

- "off":
    The metacopy digest is never generated or used. This is the
    default if verity option is not specified.
    (metacopy 摘要从不生成也从不使用。如果未指定 verity 选项，这是默认值。)
- "on":
    Whenever a metacopy file specifies an expected digest, the
    corresponding data file must match the specified digest. When
    generating a metacopy file the verity digest will be set in it
    based on the source file (if it has one).
    (只要 metacopy 文件指定了期望的摘要，对应的数据文件就必须与指定的摘要匹配。生成 metacopy 文件时，会基于源文件(如果它有摘要的话)在其中设置 verity 摘要。)
- "require":
    Same as "on", but additionally all metacopy files must specify a
    digest (or EIO is returned on open). This means metadata copy up
    will only be used if the data file has fs-verity enabled,
    otherwise a full copy-up is used.
    (与 "on" 相同，但另外所有 metacopy 文件都必须指定摘要(否则打开时返回 EIO)。这意味着只有当数据文件启用了 fs-verity 时才会使用元数据 copy up，否则使用完整 copy-up。)

Sharing and copying layers
(共享与复制层)
--------------------------

Lower layers may be shared among several overlay mounts and that is indeed
a very common practice.  An overlay mount may use the same lower layer
path as another overlay mount and it may use a lower layer path that is
beneath or above the path of another overlay lower layer path.
(下层可以在多个 overlay 挂载之间共享，这确实是非常常见的做法。一个 overlay 挂载可以使用与另一个 overlay 挂载相同的下层路径，也可以使用位于另一个 overlay 下层路径之下或之上的下层路径。)

Using an upper layer path and/or a workdir path that are already used by
another overlay mount is not allowed and may fail with EBUSY.  Using
partially overlapping paths is not allowed and may fail with EBUSY.
If files are accessed from two overlayfs mounts which share or overlap the
upper layer and/or workdir path, the behavior of the overlay is undefined,
though it will not result in a crash or deadlock.
(使用已被另一个 overlay 挂载使用的上层路径和/或 workdir 路径是不允许的，可能以 EBUSY 失败。使用部分重叠的路径是不允许的，可能以 EBUSY 失败。如果从两个共享或重叠上层和/或 workdir 路径的 overlayfs 挂载访问文件，overlay 的行为是未定义的，但不会导致崩溃或死锁。)

Mounting an overlay using an upper layer path, where the upper layer path
was previously used by another mounted overlay in combination with a
different lower layer path, is allowed, unless the "index" or "metacopy"
features are enabled.
(使用一个上层路径挂载 overlay 是允许的，即使该上层路径之前曾被另一个已挂载的 overlay 与不同的下层路径组合使用过，除非启用了 "index" 或 "metacopy" 特性。)

With the "index" feature, on the first time mount, an NFS file
handle of the lower layer root directory, along with the UUID of the lower
filesystem, are encoded and stored in the "trusted.overlay.origin" extended
attribute on the upper layer root directory.  On subsequent mount attempts,
the lower root directory file handle and lower filesystem UUID are compared
to the stored origin in upper root directory.  On failure to verify the
lower root origin, mount will fail with ESTALE.  An overlayfs mount with
"index" enabled will fail with EOPNOTSUPP if the lower filesystem
does not support NFS export, lower filesystem does not have a valid UUID or
if the upper filesystem does not support extended attributes.
(使用 "index" 特性时，在首次挂载时，下层根目录的 NFS 文件句柄以及下层文件系统的 UUID 会被编码并存储在上层根目录的 "trusted.overlay.origin" 扩展属性中。在后续的挂载尝试中，下层根目录文件句柄和下层文件系统 UUID 会与上层根目录中存储的 origin 进行比较。如果验证下层根 origin 失败，挂载将以 ESTALE 失败。启用了 "index" 的 overlayfs 挂载在下层文件系统不支持 NFS 导出、下层文件系统没有有效 UUID、或上层文件系统不支持扩展属性时，将以 EOPNOTSUPP 失败。)

For the "metacopy" feature, there is no verification mechanism at
mount time. So if same upper is mounted with different set of lower, mount
probably will succeed but expect the unexpected later on. So don't do it.
(对于 "metacopy" 特性，挂载时没有验证机制。因此，如果同一个上层与不同的一组下层挂载，挂载可能会成功，但之后请准备好面对意外情况。所以不要这样做。)

It is quite a common practice to copy overlay layers to a different
directory tree on the same or different underlying filesystem, and even
to a different machine.  With the "index" feature, trying to mount
the copied layers will fail the verification of the lower root file handle.
(把 overlay 层复制到同一或不同底层文件系统上的不同目录树，甚至复制到不同的机器上，是相当常见的做法。使用 "index" 特性时，尝试挂载复制出来的层将无法通过下层根文件句柄的验证。)

Nesting overlayfs mounts
(嵌套 overlayfs 挂载)
------------------------

It is possible to use a lower directory that is stored on an overlayfs
mount. For regular files this does not need any special care. However, files
that have overlayfs attributes, such as whiteouts or "overlay.*" xattrs, will
be interpreted by the underlying overlayfs mount and stripped out. In order to
allow the second overlayfs mount to see the attributes they must be escaped.
(可以使用存储在 overlayfs 挂载上的下层目录。对于普通文件，这不需要任何特殊处理。但是，带有 overlayfs 属性的文件(例如 whiteout 或 "overlay.*" xattr)会被底层 overlayfs 挂载解释并剥离。为了让第二个 overlayfs 挂载能看到这些属性，必须对它们进行转义。)

Overlayfs specific xattrs are escaped by using a special prefix of
"overlay.overlay.". So, a file with a "trusted.overlay.overlay.metacopy" xattr
in the lower dir will be exposed as a regular file with a
"trusted.overlay.metacopy" xattr in the overlayfs mount. This can be nested by
repeating the prefix multiple time, as each instance only removes one prefix.
(overlayfs 特有的 xattr 通过使用特殊前缀 "overlay.overlay." 来转义。因此，下层目录中带有 "trusted.overlay.overlay.metacopy" xattr 的文件，在 overlayfs 挂载中将暴露为一个带有 "trusted.overlay.metacopy" xattr 的普通文件。由于每个实例只去除一个前缀，可以通过多次重复该前缀来进行多层嵌套。)

A lower dir with a regular whiteout will always be handled by the overlayfs
mount, so to support storing an effective whiteout file in an overlayfs mount an
alternative form of whiteout is supported. This form is a regular, zero-size
file with the "overlay.whiteout" xattr set, inside a directory with the
"overlay.opaque" xattr set to "x" (see `whiteouts and opaque directories`_).
These alternative whiteouts are never created by overlayfs, but can be used by
userspace tools (like containers) that generate lower layers.
These alternative whiteouts can be escaped using the standard xattr escape
mechanism in order to properly nest to any depth.
(带有普通 whiteout 的下层目录总是会被 overlayfs 挂载处理，因此为了支持在 overlayfs 挂载中存储有效的 whiteout 文件，支持一种替代形式的 whiteout。这种形式是一个设置了 "overlay.whiteout" xattr 的普通零大小文件，位于一个把 "overlay.opaque" xattr 设置为 "x" 的目录内(参见"whiteouts 与不透明目录"一节)。这些替代形式的 whiteout 从不由 overlayfs 创建，但可以由生成下层的用户空间工具(如容器)使用。这些替代形式的 whiteout 可以使用标准的 xattr 转义机制进行转义，以便正确嵌套到任意深度。)

Non-standard behavior
(非标准行为)
---------------------

Current version of overlayfs can act as a mostly POSIX compliant
filesystem.
(当前版本的 overlayfs 可以作为一个大体符合 POSIX 的文件系统。)

This is the list of cases that overlayfs doesn't currently handle:
(以下是 overlayfs 目前未处理的情形列表：)

 a) POSIX mandates updating st_atime for reads.  This is currently not
    done in the case when the file resides on a lower layer.
    (POSIX 要求读取时更新 st_atime。目前，当文件位于下层时不会这样做。)

 b) If a file residing on a lower layer is opened for read-only and then
    memory mapped with MAP_SHARED, then subsequent changes to the file are not
    reflected in the memory mapping.
    (如果位于下层的文件以只读方式打开，然后用 MAP_SHARED 进行内存映射，那么对文件的后续更改不会反映在内存映射中。)

 c) If a file residing on a lower layer is being executed, then opening that
    file for write or truncating the file will not be denied with ETXTBSY.
    (如果正在执行位于下层的文件，那么以写方式打开该文件或截断该文件不会被 ETXTBSY 拒绝。)

The following options allow overlayfs to act more like a standards
compliant filesystem:
(以下选项使 overlayfs 的行为更像一个符合标准的文件系统：)

redirect_dir
````````````
(redirect_dir 选项)

Enabled with the mount option or module option: "redirect_dir=on" or with
the kernel config option CONFIG_OVERLAY_FS_REDIRECT_DIR=y.
(通过挂载选项或模块选项 "redirect_dir=on" 启用，或通过内核配置选项 CONFIG_OVERLAY_FS_REDIRECT_DIR=y 启用。)

If this feature is disabled, then rename(2) on a lower or merged directory
will fail with EXDEV ("Invalid cross-device link").
(如果此特性被禁用，则对下层或合并目录执行 rename(2) 将以 EXDEV ("Invalid cross-device link") 失败。)

index
`````
(index 特性)

Enabled with the mount option or module option "index=on" or with the
kernel config option CONFIG_OVERLAY_FS_INDEX=y.
(通过挂载选项或模块选项 "index=on" 启用，或通过内核配置选项 CONFIG_OVERLAY_FS_INDEX=y 启用。)

If this feature is disabled and a file with multiple hard links is copied
up, then this will "break" the link.  Changes will not be propagated to
other names referring to the same inode.
(如果此特性被禁用，且一个有多个硬链接的文件被复制上去，那么这会"打断"链接。更改不会传播到指向同一 inode 的其他名字。)

xino
````
(xino 特性)

Enabled with the mount option "xino=auto" or "xino=on", with the module
option "xino_auto=on" or with the kernel config option
CONFIG_OVERLAY_FS_XINO_AUTO=y.  Also implicitly enabled by using the same
underlying filesystem for all layers making up the overlay.
(通过挂载选项 "xino=auto" 或 "xino=on"、模块选项 "xino_auto=on" 或内核配置选项 CONFIG_OVERLAY_FS_XINO_AUTO=y 启用。当组成 overlay 的所有层使用相同的底层文件系统时，它也会被隐式启用。)

If this feature is disabled or the underlying filesystem doesn't have
enough free bits in the inode number, then overlayfs will not be able to
guarantee that the values of st_ino and st_dev returned by stat(2) and the
value of d_ino returned by readdir(3) will act like on a normal filesystem.
E.g. the value of st_dev may be different for two objects in the same
overlay filesystem and the value of st_ino for filesystem objects may not be
persistent and could change even while the overlay filesystem is mounted, as
summarized in the `Inode properties`_ table above.
(如果此特性被禁用，或底层文件系统在 inode 编号中没有足够的空闲位，那么 overlayfs 将无法保证 stat(2) 返回的 st_ino 和 st_dev 值以及 readdir(3) 返回的 d_ino 值表现得像在普通文件系统上一样。例如，同一 overlay 文件系统中两个对象的 st_dev 值可能不同，文件系统对象的 st_ino 值可能不持久，甚至在 overlay 文件系统仍处于挂载状态时也可能改变，如上文"inode 属性"表中所总结的。)


Changes to underlying filesystems
(对底层文件系统的更改)
---------------------------------

Changes to the underlying filesystems while part of a mounted overlay
filesystem are not allowed.  If the underlying filesystem is changed,
the behavior of the overlay is undefined, though it will not result in
a crash or deadlock.
(不允许在作为已挂载 overlay 文件系统的一部分期间对底层文件系统进行更改。如果底层文件系统被更改，overlay 的行为是未定义的，但不会导致崩溃或死锁。)

Offline changes, when the overlay is not mounted, are allowed to the
upper tree.  Offline changes to the lower tree are only allowed if the
"metacopy", "index", "xino" and "redirect_dir" features
have not been used.  If the lower tree is modified and any of these
features has been used, the behavior of the overlay is undefined,
though it will not result in a crash or deadlock.
(在 overlay 未挂载时的离线更改对上层目录树是允许的。对下层目录树的离线更改只有在未使用过 "metacopy"、"index"、"xino" 和 "redirect_dir" 特性时才允许。如果下层目录树被修改且这些特性中有任何一个被使用过，overlay 的行为是未定义的，但不会导致崩溃或死锁。)

When the overlay NFS export feature is enabled, overlay filesystems
behavior on offline changes of the underlying lower layer is different
than the behavior when NFS export is disabled.
(当 overlay NFS 导出特性启用时，overlay 文件系统对底层下层离线更改的行为与禁用 NFS 导出时的行为不同。)

On every copy_up, an NFS file handle of the lower inode, along with the
UUID of the lower filesystem, are encoded and stored in an extended
attribute "trusted.overlay.origin" on the upper inode.
(在每次 copy_up 时，下层 inode 的 NFS 文件句柄以及下层文件系统的 UUID 会被编码并存储在上层 inode 的扩展属性 "trusted.overlay.origin" 中。)

When the NFS export feature is enabled, a lookup of a merged directory,
that found a lower directory at the lookup path or at the path pointed
to by the "trusted.overlay.redirect" extended attribute, will verify
that the found lower directory file handle and lower filesystem UUID
match the origin file handle that was stored at copy_up time.  If a
found lower directory does not match the stored origin, that directory
will not be merged with the upper directory.
(当 NFS 导出特性启用时，对合并目录的查找如果在查找路径上或在 "trusted.overlay.redirect" 扩展属性所指向的路径上找到了下层目录，将验证所找到的下层目录文件句柄和下层文件系统 UUID 是否与 copy_up 时存储的 origin 文件句柄匹配。如果所找到的下层目录与存储的 origin 不匹配，该目录将不会与上层目录合并。)



NFS export
(NFS 导出)
----------

When the underlying filesystems supports NFS export and the "nfs_export"
feature is enabled, an overlay filesystem may be exported to NFS.
(当底层文件系统支持 NFS 导出且 "nfs_export" 特性启用时，overlay 文件系统可以被导出到 NFS。)

With the "nfs_export" feature, on copy_up of any lower object, an index
entry is created under the index directory.  The index entry name is the
hexadecimal representation of the copy up origin file handle.  For a
non-directory object, the index entry is a hard link to the upper inode.
For a directory object, the index entry has an extended attribute
"trusted.overlay.upper" with an encoded file handle of the upper
directory inode.
(使用 "nfs_export" 特性时，在任何一个下层对象 copy up 时，会在索引目录下创建一个索引项。索引项的名字是 copy up origin 文件句柄的十六进制表示。对于非目录对象，索引项是指向上层 inode 的硬链接。对于目录对象，索引项有一个扩展属性 "trusted.overlay.upper"，其中包含编码后的上层目录 inode 文件句柄。)

When encoding a file handle from an overlay filesystem object, the
following rules apply:
(从 overlay 文件系统对象编码文件句柄时，适用以下规则：)

 1. For a non-upper object, encode a lower file handle from lower inode
    (对于非上层对象，从下层 inode 编码一个下层文件句柄)
 2. For an indexed object, encode a lower file handle from copy_up origin
    (对于有索引的对象，从 copy_up origin 编码一个下层文件句柄)
 3. For a pure-upper object and for an existing non-indexed upper object,
    encode an upper file handle from upper inode
    (对于纯上层对象和已存在的无索引上层对象，从上层 inode 编码一个上层文件句柄)

The encoded overlay file handle includes:
(编码后的 overlay 文件句柄包括：)

 - Header including path type information (e.g. lower/upper)
   (包含路径类型信息(例如 lower/upper)的头部)
 - UUID of the underlying filesystem
   (底层文件系统的 UUID)
 - Underlying filesystem encoding of underlying inode
   (底层 inode 的底层文件系统编码)

This encoding format is identical to the encoding format file handles that
are stored in extended attribute "trusted.overlay.origin".
(此编码格式与存储在扩展属性 "trusted.overlay.origin" 中的文件句柄的编码格式相同。)

When decoding an overlay file handle, the following steps are followed:
(解码 overlay 文件句柄时，遵循以下步骤：)

 1. Find underlying layer by UUID and path type information.
    (通过 UUID 和路径类型信息找到底层。)
 2. Decode the underlying filesystem file handle to underlying dentry.
    (把底层文件系统文件句柄解码为底层 dentry。)
 3. For a lower file handle, lookup the handle in index directory by name.
    (对于下层文件句柄，按名字在索引目录中查找该句柄。)
 4. If a whiteout is found in index, return ESTALE. This represents an
    overlay object that was deleted after its file handle was encoded.
    (如果在索引中找到一个 whiteout，则返回 ESTALE。这表示一个在其文件句柄被编码之后被删除的 overlay 对象。)
 5. For a non-directory, instantiate a disconnected overlay dentry from the
    decoded underlying dentry, the path type and index inode, if found.
    (对于非目录，从解码出的底层 dentry、路径类型和索引 inode(如果找到)实例化一个断开连接的 overlay dentry。)
 6. For a directory, use the connected underlying decoded dentry, path type
    and index, to lookup a connected overlay dentry.
    (对于目录，使用已连接的底层解码 dentry、路径类型和索引来查找一个已连接的 overlay dentry。)

Decoding a non-directory file handle may return a disconnected dentry.
copy_up of that disconnected dentry will create an upper index entry with
no upper alias.
(解码非目录文件句柄可能返回一个断开连接的 dentry。对该断开连接 dentry 的 copy_up 会创建一个没有上层别名的上层索引项。)

When overlay filesystem has multiple lower layers, a middle layer
directory may have a "redirect" to lower directory.  Because middle layer
"redirects" are not indexed, a lower file handle that was encoded from the
"redirect" origin directory, cannot be used to find the middle or upper
layer directory.  Similarly, a lower file handle that was encoded from a
descendant of the "redirect" origin directory, cannot be used to
reconstruct a connected overlay path.  To mitigate the cases of
directories that cannot be decoded from a lower file handle, these
directories are copied up on encode and encoded as an upper file handle.
On an overlay filesystem with no upper layer this mitigation cannot be
used NFS export in this setup requires turning off redirect follow (e.g.
"redirect_dir=nofollow").
(当 overlay 文件系统有多个下层时，中间层目录可能有指向下层目录的 "redirect"。由于中间层的 "redirect" 没有被索引，从 "redirect" origin 目录编码的下层文件句柄不能用于查找中间层或上层目录。类似地，从 "redirect" origin 目录的后代编码的下层文件句柄不能用于重构已连接的 overlay 路径。为了缓解无法从下层文件句柄解码的目录的情况，这些目录会在编码时被复制上去并编码为上层文件句柄。在没有上层的 overlay 文件系统上无法使用这种缓解措施；在这种配置下，NFS 导出需要关闭重定向跟随(例如 "redirect_dir=nofollow")。)

The overlay filesystem does not support non-directory connectable file
handles, so exporting with the 'subtree_check' exportfs configuration will
cause failures to lookup files over NFS.
(overlay 文件系统不支持非目录的可连接文件句柄，因此使用 'subtree_check' exportfs 配置进行导出会导致通过 NFS 查找文件失败。)

When the NFS export feature is enabled, all directory index entries are
verified on mount time to check that upper file handles are not stale.
This verification may cause significant overhead in some cases.
(当 NFS 导出特性启用时，所有目录索引项都会在挂载时进行验证，以检查上层文件句柄是否过期。这种验证在某些情况下可能造成显著的开销。)

Note: the mount options index=off,nfs_export=on are conflicting for a
read-write mount and will result in an error.
(注意：对于读写挂载，挂载选项 index=off,nfs_export=on 相互冲突，会导致错误。)

Note: the mount option uuid=off can be used to replace UUID of the underlying
filesystem in file handles with null, in order to relax the UUID checks. This
can be useful in case the underlying disk is copied and the UUID of this copy
is changed. This is only applicable if all lower directories are on
the same filesystem, otherwise it will fallback to normal behaviour.
(注意：挂载选项 uuid=off 可用于把文件句柄中底层文件系统的 UUID 替换为 null，以放宽 UUID 检查。这在底层磁盘被复制且该副本的 UUID 被更改的情况下可能有用。这仅适用于所有下层目录都在同一文件系统上的情况，否则将回退到正常行为。)


UUID and fsid
(UUID 与 fsid)
-------------

The UUID of overlayfs instance itself and the fsid reported by statfs(2) are
controlled by the "uuid" mount option, which supports these values:
(overlayfs 实例自身的 UUID 和 statfs(2) 报告的 fsid 由 "uuid" 挂载选项控制，该选项支持以下取值：)

- "null":
    UUID of overlayfs is null. fsid is taken from upper most filesystem.
    (overlayfs 的 UUID 为 null。fsid 取自最上层的文件系统。)
- "off":
    UUID of overlayfs is null. fsid is taken from upper most filesystem.
    UUID of underlying layers is ignored and null used instead.
    (overlayfs 的 UUID 为 null。fsid 取自最上层的文件系统。底层各层的 UUID 被忽略并使用 null 代替。)
- "on":
    UUID of overlayfs is generated and used to report a unique fsid.
    UUID is stored in xattr "trusted.overlay.uuid", making overlayfs fsid
    unique and persistent.  This option requires an overlayfs with upper
    filesystem that supports xattrs.
    (overlayfs 的 UUID 被生成并用于报告唯一的 fsid。UUID 存储在 xattr "trusted.overlay.uuid" 中，使 overlayfs 的 fsid 唯一且持久。此选项要求 overlayfs 的上层文件系统支持 xattr。)
- "auto": (default)
    UUID is taken from xattr "trusted.overlay.uuid" if it exists.
    Upgrade to "uuid=on" on first time mount of new overlay filesystem that
    meets the prerequisites.
    Downgrade to "uuid=null" for existing overlay filesystems that were never
    mounted with "uuid=on".
    (UUID 取自 xattr "trusted.overlay.uuid"(如果它存在)。对满足前提条件的新 overlay 文件系统在首次挂载时升级为 "uuid=on"。对于从未以 "uuid=on" 挂载过的现有 overlay 文件系统，降级为 "uuid=null"。)


Durability and copy up
(持久性与 copy up)
----------------------

The fsync(2) system call ensures that the data and metadata of a file
are safely written to the backing storage, which is expected to
guarantee the existence of the information post system crash.
(fsync(2) 系统调用确保文件的数据和元数据被安全地写入后备存储，预期可以保证系统崩溃后信息仍然存在。)

Without an fsync(2) call, there is no guarantee that the observed
data after a system crash will be either the old or the new data, but
in practice, the observed data after crash is often the old or new data
or a mix of both.
(在没有 fsync(2) 调用的情况下，无法保证系统崩溃后观察到的数据是旧数据还是新数据，但实践中，崩溃后观察到的数据通常是旧数据、新数据或两者的混合。)

When an overlayfs file is modified for the first time, copy up will
create a copy of the lower file and its parent directories in the upper
layer.  Since the Linux filesystem API does not enforce any particular
ordering on storing changes without explicit fsync(2) calls, in case
of a system crash, the upper file could end up with no data at all
(i.e. zeros), which would be an unusual outcome.  To avoid this
experience, overlayfs calls fsync(2) on the upper file before completing
data copy up with rename(2) or link(2) to make the copy up "atomic".
(当 overlayfs 文件第一次被修改时，copy up 会在上层创建下层文件及其父目录的副本。由于 Linux 文件系统 API 在没有显式 fsync(2) 调用时不会对存储更改强制任何特定顺序，在系统崩溃的情况下，上层文件最终可能完全没有数据(即全为零)，这将是一种不寻常的结果。为了避免这种体验，overlayfs 在通过 rename(2) 或 link(2) 完成数据 copy up 之前对上层文件调用 fsync(2)，以使 copy up 具有"原子性"。)

By default, overlayfs does not explicitly call fsync(2) on copied up
directories or on metadata-only copy up, so it provides no guarantee to
persist the user's modification unless the user calls fsync(2).
The fsync during copy up only guarantees that if a copy up is observed
after a crash, the observed data is not zeroes or intermediate values
from the copy up staging area.
(默认情况下，overlayfs 不会对被复制上去的目录或仅元数据 copy up 显式调用 fsync(2)，因此除非用户调用 fsync(2)，否则它不保证持久化用户的修改。copy up 期间的 fsync 只保证：如果崩溃后观察到一次 copy up，观察到的数据不会是零或来自 copy up 暂存区的中间值。)

On traditional local filesystems with a single journal (e.g. ext4, xfs),
fsync on a file also persists the parent directory changes, because they
are usually modified in the same transaction, so metadata durability during
data copy up effectively comes for free.  Overlayfs further limits risk by
disallowing network filesystems as upper layer.
(在具有单一日志的传统本地文件系统上(例如 ext4、xfs)，对文件的 fsync 也会持久化父目录更改，因为它们通常在同一个事务中被修改，因此数据 copy up 期间的元数据持久性实际上是免费获得的。overlayfs 通过禁止使用网络文件系统作为上层来进一步限制风险。)

Overlayfs can be tuned to prefer performance or durability when storing
to the underlying upper layer.  This is controlled by the "fsync" mount
option, which supports these values:
(在向底层上层存储时，overlayfs 可以被调整以优先考虑性能或持久性。这由 "fsync" 挂载选项控制，该选项支持以下取值：)

- "auto": (default)
    Call fsync(2) on upper file before completion of data copy up.
    No explicit fsync(2) on directory or metadata-only copy up.
    (在数据 copy up 完成之前对上层文件调用 fsync(2)。对目录或仅元数据 copy up 不做显式 fsync(2)。)
- "strict":
    Call fsync(2) on upper file and directories before completion of any
    copy up.
    (在任何 copy up 完成之前对上层文件和目录调用 fsync(2)。)
- "volatile": [*]
    Prefer performance over durability (see `Volatile mount`_)
    (优先考虑性能而非持久性(参见"volatile 挂载"一节))

[*] The mount option "volatile" is an alias to "fsync=volatile".
([*] 挂载选项 "volatile" 是 "fsync=volatile" 的别名。)


Volatile mount
(volatile 挂载)
--------------

This is enabled with the "volatile" mount option.  Volatile mounts are not
guaranteed to survive a crash.  It is strongly recommended that volatile
mounts are only used if data written to the overlay can be recreated
without significant effort.
(通过 "volatile" 挂载选项启用。volatile 挂载不保证在崩溃后存活。强烈建议仅在写入 overlay 的数据可以不费很大力气就重建的情况下才使用 volatile 挂载。)

The advantage of mounting with the "volatile" option is that all forms of
sync calls to the upper filesystem are omitted.
(使用 "volatile" 选项挂载的优势是对上层文件系统的所有形式的同步调用都被省略。)

In order to avoid giving a false sense of safety, the syncfs (and fsync)
semantics of volatile mounts are slightly different than that of the rest of
VFS.  If any writeback error occurs on the upperdir's filesystem after a
volatile mount takes place, all sync functions will return an error.  Once this
condition is reached, the filesystem will not recover, and every subsequent sync
call will return an error, even if the upperdir has not experienced a new error
since the last sync call.
(为了避免给人虚假的安全感，volatile 挂载的 syncfs(和 fsync)语义与其余 VFS 略有不同。如果在 volatile 挂载发生之后，upperdir 的文件系统上发生了任何回写(writeback)错误，所有同步函数都将返回错误。一旦达到这种状态，文件系统将不会恢复，之后每次同步调用都会返回错误，即使 upperdir 自上次同步调用以来没有发生新的错误。)

When overlay is mounted with "volatile" option, the directory
"$workdir/work/incompat/volatile" is created.  During next mount, overlay
checks for this directory and refuses to mount if present. This is a strong
indicator that the user should discard upper and work directories and create
fresh ones. In very limited cases where the user knows that the system has
not crashed and contents of upperdir are intact, the "volatile" directory
can be removed.
(当 overlay 以 "volatile" 选项挂载时，会创建目录 "$workdir/work/incompat/volatile"。在下一次挂载期间，overlay 会检查该目录，如果存在则拒绝挂载。这强烈表明用户应当丢弃上层和 work 目录并创建新的目录。在极少数情况下，如果用户确定系统没有崩溃且 upperdir 内容完好，则可以删除该 "volatile" 目录。)


User xattr
(用户 xattr)
----------

The "-o userxattr" mount option forces overlayfs to use the
"user.overlay." xattr namespace instead of "trusted.overlay.".  This is
useful for unprivileged mounting of overlayfs.
("-o userxattr" 挂载选项强制 overlayfs 使用 "user.overlay." xattr 命名空间而不是 "trusted.overlay."。这对于 overlayfs 的非特权挂载很有用。)


Testsuite
(测试套件)
---------

There's a testsuite originally developed by David Howells and currently
maintained by Amir Goldstein at:
(有一个最初由 David Howells 开发、目前由 Amir Goldstein 维护的测试套件，位于：)

https://github.com/amir73il/unionmount-testsuite.git

Run as root::
(以 root 身份运行：)

  # cd unionmount-testsuite
  # ./run --ov --verify
