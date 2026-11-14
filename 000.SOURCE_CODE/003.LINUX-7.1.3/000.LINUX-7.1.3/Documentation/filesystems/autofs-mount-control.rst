.. SPDX-License-Identifier: GPL-2.0

====================================================================
Miscellaneous Device control operations for the autofs kernel module
====================================================================

(autofs 内核模块的杂项设备控制操作)

The problem
(问题)
===========

There is a problem with active restarts in autofs (that is to say
restarting autofs when there are busy mounts).
(autofs 在活动重启方面存在问题(也就是说，在有繁忙挂载的情况下重启 autofs)。)

During normal operation autofs uses a file descriptor opened on the
directory that is being managed in order to be able to issue control
operations. Using a file descriptor gives ioctl operations access to
autofs specific information stored in the super block. The operations
are things such as setting an autofs mount catatonic, setting the
expire timeout and requesting expire checks. As is explained below,
certain types of autofs triggered mounts can end up covering an autofs
mount itself which prevents us being able to use open(2) to obtain a
file descriptor for these operations if we don't already have one open.
(在正常操作期间，autofs 使用一个在正被管理的目录上打开的文件描述符，以便能够发出控制操作。使用文件描述符使 ioctl 操作可以访问存储在超级块中的 autofs 特定信息。这些操作包括诸如将 autofs 挂载设置为 catatonic、设置过期超时以及请求过期检查等。如下文所解释的，某些类型的 autofs 触发的挂载最终可能会覆盖 autofs 挂载本身，如果我们尚未打开一个文件描述符，这会使我们无法使用 open(2) 为这些操作获取文件描述符。)

Currently autofs uses "umount -l" (lazy umount) to clear active mounts
at restart. While using lazy umount works for most cases, anything that
needs to walk back up the mount tree to construct a path, such as
getcwd(2) and the proc file system /proc/<pid>/cwd, no longer works
because the point from which the path is constructed has been detached
from the mount tree.
(目前 autofs 使用 "umount -l"(延迟卸载)来在重启时清除活动挂载。虽然使用延迟卸载在大多数情况下有效，但任何需要沿挂载树向上回溯以构造路径的东西，例如 getcwd(2) 和 proc 文件系统的 /proc/<pid>/cwd，都将不再工作，因为构造路径的起点已经从挂载树上分离。)

The actual problem with autofs is that it can't reconnect to existing
mounts. Immediately one thinks of just adding the ability to remount
autofs file systems would solve it, but alas, that can't work. This is
because autofs direct mounts and the implementation of "on demand mount
and expire" of nested mount trees have the file system mounted directly
on top of the mount trigger directory dentry.
(autofs 的实际问题在于它无法重新连接到现有的挂载。人们立刻会想到，只需添加重新挂载 autofs 文件系统的能力就能解决这个问题，但可惜的是，那是行不通的。这是因为 autofs 直接挂载以及嵌套挂载树的"按需挂载和过期"实现将文件系统直接挂载在挂载触发目录 dentry 之上。)

For example, there are two types of automount maps, direct (in the kernel
module source you will see a third type called an offset, which is just
a direct mount in disguise) and indirect.
(例如，有两种类型的自动挂载映射: 直接的(在内核模块源码中你会看到第三种类型，称为 offset，它只是伪装的直接挂载)和间接的。)

Here is a master map with direct and indirect map entries::
(下面是一个带有直接和间接映射项的主映射：)

    /-      /etc/auto.direct
    /test   /etc/auto.indirect

and the corresponding map files::
(以及相应的映射文件：)

    /etc/auto.direct:

    /automount/dparse/g6  budgie:/autofs/export1
    /automount/dparse/g1  shark:/autofs/export1
    and so on.

/etc/auto.indirect::

    g1    shark:/autofs/export1
    g6    budgie:/autofs/export1
    and so on.

For the above indirect map an autofs file system is mounted on /test and
mounts are triggered for each sub-directory key by the inode lookup
operation. So we see a mount of shark:/autofs/export1 on /test/g1, for
example.
(对于上面的间接映射，一个 autofs 文件系统被挂载在 /test 上，并且 inode 查找操作会为每个子目录键触发挂载。例如，我们会看到 shark:/autofs/export1 被挂载到 /test/g1 上。)

The way that direct mounts are handled is by making an autofs mount on
each full path, such as /automount/dparse/g1, and using it as a mount
trigger. So when we walk on the path we mount shark:/autofs/export1 "on
top of this mount point". Since these are always directories we can
use the follow_link inode operation to trigger the mount.
(处理直接挂载的方式是在每个完整路径(例如 /automount/dparse/g1)上创建一个 autofs 挂载，并将其用作挂载触发器。因此，当我们沿着路径行走时，我们会将 shark:/autofs/export1 挂载"到此挂载点之上"。由于这些总是目录，我们可以使用 follow_link inode 操作来触发挂载。)

But, each entry in direct and indirect maps can have offsets (making
them multi-mount map entries).
(但是，直接和间接映射中的每个条目都可以有 offset(使它们成为多挂载映射条目)。)

For example, an indirect mount map entry could also be::
(例如，一个间接挂载映射条目也可以是：)

    g1  \
    /        shark:/autofs/export5/testing/test \
    /s1      shark:/autofs/export/testing/test/s1 \
    /s2      shark:/autofs/export5/testing/test/s2 \
    /s1/ss1  shark:/autofs/export1 \
    /s2/ss2  shark:/autofs/export2

and a similarly a direct mount map entry could also be::
(类似地，一个直接挂载映射条目也可以是：)

    /automount/dparse/g1 \
	/       shark:/autofs/export5/testing/test \
	/s1     shark:/autofs/export/testing/test/s1 \
	/s2     shark:/autofs/export5/testing/test/s2 \
	/s1/ss1 shark:/autofs/export2 \
	/s2/ss2 shark:/autofs/export2

One of the issues with version 4 of autofs was that, when mounting an
entry with a large number of offsets, possibly with nesting, we needed
to mount and umount all of the offsets as a single unit. Not really a
problem, except for people with a large number of offsets in map entries.
This mechanism is used for the well known "hosts" map and we have seen
cases (in 2.4) where the available number of mounts are exhausted or
where the number of privileged ports available is exhausted.
(autofs 版本 4 的一个问题在于，当挂载一个带有大量 offset(可能有嵌套)的条目时，我们需要将所有 offset 作为一个整体来挂载和卸载。这其实算不上什么问题，只是对映射条目中有大量 offset 的人来说是个问题。这种机制用于众所周知的 "hosts" 映射，我们曾见过(在 2.4 中)可用挂载数量被耗尽或可用特权端口数量被耗尽的情况。)

In version 5 we mount only as we go down the tree of offsets and
similarly for expiring them which resolves the above problem. There is
somewhat more detail to the implementation but it isn't needed for the
sake of the problem explanation. The one important detail is that these
offsets are implemented using the same mechanism as the direct mounts
above and so the mount points can be covered by a mount.
(在版本 5 中，我们只在沿 offset 树向下时才进行挂载，对它们的过期处理也是如此，这解决了上述问题。实现上还有一些细节，但就问题说明而言并不需要。其中一个重要细节是，这些 offset 是使用与上面的直接挂载相同的机制实现的，因此挂载点可能被某个挂载所覆盖。)

The current autofs implementation uses an ioctl file descriptor opened
on the mount point for control operations. The references held by the
descriptor are accounted for in checks made to determine if a mount is
in use and is also used to access autofs file system information held
in the mount super block. So the use of a file handle needs to be
retained.
(当前的 autofs 实现使用在挂载点上打开的 ioctl 文件描述符来执行控制操作。该描述符持有的引用会在确定挂载是否正在使用的检查中被计入，并且还用于访问挂载超级块中保存的 autofs 文件系统信息。因此，需要保留文件句柄的使用。)


The Solution
(解决方案)
============

To be able to restart autofs leaving existing direct, indirect and
offset mounts in place we need to be able to obtain a file handle
for these potentially covered autofs mount points. Rather than just
implement an isolated operation it was decided to re-implement the
existing ioctl interface and add new operations to provide this
functionality.
(为了能够在重启 autofs 时让现有的直接、间接和 offset 挂载保持原样，我们需要能够为这些可能被覆盖的 autofs 挂载点获取文件句柄。我们决定不只是实现一个孤立的操作，而是重新实现现有的 ioctl 接口并添加新的操作来提供此功能。)

In addition, to be able to reconstruct a mount tree that has busy mounts,
the uid and gid of the last user that triggered the mount needs to be
available because these can be used as macro substitution variables in
autofs maps. They are recorded at mount request time and an operation
has been added to retrieve them.
(此外，为了能够重建包含繁忙挂载的挂载树，需要能够获得最后触发挂载的用户的 uid 和 gid，因为它们可以用作 autofs 映射中的宏替换变量。它们在挂载请求时被记录，并且已经添加了一个操作来检索它们。)

Since we're re-implementing the control interface, a couple of other
problems with the existing interface have been addressed. First, when
a mount or expire operation completes a status is returned to the
kernel by either a "send ready" or a "send fail" operation. The
"send fail" operation of the ioctl interface could only ever send
ENOENT so the re-implementation allows user space to send an actual
status. Another expensive operation in user space, for those using
very large maps, is discovering if a mount is present. Usually this
involves scanning /proc/mounts and since it needs to be done quite
often it can introduce significant overhead when there are many entries
in the mount table. An operation to lookup the mount status of a mount
point dentry (covered or not) has also been added.
(由于我们正在重新实现控制接口，现有接口的另外几个问题也得到了解决。首先，当挂载或过期操作完成时，会通过"发送就绪"或"发送失败"操作向内核返回一个状态。ioctl 接口的"发送失败"操作只能发送 ENOENT，因此重新实现允许用户空间发送实际的状态。对于使用非常大的映射的用户来说，用户空间中另一个昂贵的操作是查明某个挂载是否存在。这通常涉及扫描 /proc/mounts，而且由于需要相当频繁地执行，当挂载表中有很多条目时它可能带来显著的开销。还添加了一个操作，用于查找挂载点 dentry(无论是否被覆盖)的挂载状态。)

Current kernel development policy recommends avoiding the use of the
ioctl mechanism in favor of systems such as Netlink. An implementation
using this system was attempted to evaluate its suitability and it was
found to be inadequate, in this case. The Generic Netlink system was
used for this as raw Netlink would lead to a significant increase in
complexity. There's no question that the Generic Netlink system is an
elegant solution for common case ioctl functions but it's not a complete
replacement probably because its primary purpose in life is to be a
message bus implementation rather than specifically an ioctl replacement.
While it would be possible to work around this there is one concern
that lead to the decision to not use it. This is that the autofs
expire in the daemon has become far to complex because umount
candidates are enumerated, almost for no other reason than to "count"
the number of times to call the expire ioctl. This involves scanning
the mount table which has proved to be a big overhead for users with
large maps. The best way to improve this is try and get back to the
way the expire was done long ago. That is, when an expire request is
issued for a mount (file handle) we should continually call back to
the daemon until we can't umount any more mounts, then return the
appropriate status to the daemon. At the moment we just expire one
mount at a time. A Generic Netlink implementation would exclude this
possibility for future development due to the requirements of the
message bus architecture.
(当前的内核开发政策建议避免使用 ioctl 机制，而倾向于使用 Netlink 之类的系统。我们尝试了一种使用该系统的实现来评估其适用性，发现在这种情况下它并不合适。这里使用的是 Generic Netlink 系统，因为原始 Netlink 会导致复杂性显著增加。毫无疑问，Generic Netlink 系统对于常见情况的 ioctl 函数是一个优雅的解决方案，但它并不是一个完全的替代品，可能是因为它的首要用途是作为一个消息总线实现，而不是专门的 ioctl 替代品。虽然可以绕过这一点，但有一个顾虑导致了不使用它的决定。这就是 daemon 中的 autofs 过期处理已变得过于复杂，因为枚举卸载候选者几乎只是为了"计数"调用过期 ioctl 的次数。这涉及扫描挂载表，事实证明这对拥有大映射的用户来说是一个很大的开销。改进这一点的最佳方法是尽量回到很久以前进行过期处理的方式。也就是说，当对一个挂载(文件句柄)发出过期请求时，我们应该持续回调 daemon，直到我们不能再卸载任何挂载，然后向 daemon 返回适当的状态。目前我们一次只使一个挂载过期。由于消息总线架构的要求，Generic Netlink 实现会排除这种未来开发的可能性。)


autofs Miscellaneous Device mount control interface
(autofs 杂项设备挂载控制接口)
====================================================

The control interface is opening a device node, typically /dev/autofs.
(控制接口就是打开一个设备节点，通常是 /dev/autofs。)

All the ioctls use a common structure to pass the needed parameter
information and return operation results::
(所有 ioctl 都使用一个公共结构来传递所需的参数信息并返回操作结果：)

    struct autofs_dev_ioctl {
	    __u32 ver_major;
	    __u32 ver_minor;
	    __u32 size;             /* total size of data passed in
				    * including this struct */
	    __s32 ioctlfd;          /* automount command fd */

	    /* Command parameters */
	    union {
		    struct args_protover		protover;
		    struct args_protosubver		protosubver;
		    struct args_openmount		openmount;
		    struct args_ready		ready;
		    struct args_fail		fail;
		    struct args_setpipefd		setpipefd;
		    struct args_timeout		timeout;
		    struct args_requester		requester;
		    struct args_expire		expire;
		    struct args_askumount		askumount;
		    struct args_ismountpoint	ismountpoint;
	    };

	    char path[];
    };

The ioctlfd field is a mount point file descriptor of an autofs mount
point. It is returned by the open call and is used by all calls except
the check for whether a given path is a mount point, where it may
optionally be used to check a specific mount corresponding to a given
mount point file descriptor, and when requesting the uid and gid of the
last successful mount on a directory within the autofs file system.
(ioctlfd 字段是一个 autofs 挂载点的挂载点文件描述符。它由 open 调用返回，并被除检查给定路径是否为挂载点之外的所有调用使用；在检查挂载点时，可以选择使用它来检查与给定挂载点文件描述符对应的特定挂载。此外，在请求 autofs 文件系统中目录上最后一次成功挂载的 uid 和 gid 时也会用到它。)

The union is used to communicate parameters and results of calls made
as described below.
(该联合体用于按如下所述传递所做调用的参数和结果。)

The path field is used to pass a path where it is needed and the size field
is used account for the increased structure length when translating the
structure sent from user space.
(path 字段用于在需要时传递路径，而 size 字段用于在转换从用户空间发送来的结构时计入增加的结构长度。)

This structure can be initialized before setting specific fields by using
the void function call init_autofs_dev_ioctl(``struct autofs_dev_ioctl *``).
(在设置特定字段之前，可以使用无返回值的函数调用 init_autofs_dev_ioctl(``struct autofs_dev_ioctl *``)来初始化此结构。)

All of the ioctls perform a copy of this structure from user space to
kernel space and return -EINVAL if the size parameter is smaller than
the structure size itself, -ENOMEM if the kernel memory allocation fails
or -EFAULT if the copy itself fails. Other checks include a version check
of the compiled in user space version against the module version and a
mismatch results in a -EINVAL return. If the size field is greater than
the structure size then a path is assumed to be present and is checked to
ensure it begins with a "/" and is NULL terminated, otherwise -EINVAL is
returned. Following these checks, for all ioctl commands except
AUTOFS_DEV_IOCTL_VERSION_CMD, AUTOFS_DEV_IOCTL_OPENMOUNT_CMD and
AUTOFS_DEV_IOCTL_CLOSEMOUNT_CMD the ioctlfd is validated and if it is
not a valid descriptor or doesn't correspond to an autofs mount point
an error of -EBADF, -ENOTTY or -EINVAL (not an autofs descriptor) is
returned.
(所有 ioctl 都会执行一次将此结构从用户空间复制到内核空间的操作，如果 size 参数小于结构本身的大小则返回 -EINVAL，如果内核内存分配失败则返回 -ENOMEM，如果复制本身失败则返回 -EFAULT。其他检查包括将编译进来的用户空间版本与模块版本进行版本检查，不匹配会导致返回 -EINVAL。如果 size 字段大于结构大小，则假定存在一个路径，并检查它以确保它以 "/" 开头并且以 NULL 结尾，否则返回 -EINVAL。在这些检查之后，对于除 AUTOFS_DEV_IOCTL_VERSION_CMD、AUTOFS_DEV_IOCTL_OPENMOUNT_CMD 和 AUTOFS_DEV_IOCTL_CLOSEMOUNT_CMD 之外的所有 ioctl 命令，都会验证 ioctlfd，如果它不是有效的描述符或不对应于 autofs 挂载点，则返回错误 -EBADF、-ENOTTY 或 -EINVAL(不是 autofs 描述符)。)


The ioctls
(ioctl 列表)
==========

An example of an implementation which uses this interface can be seen
in autofs version 5.0.4 and later in file lib/dev-ioctl-lib.c of the
distribution tar available for download from kernel.org in directory
/pub/linux/daemons/autofs/v5.
(可以在 autofs 5.0.4 及更高版本中看到使用此接口的实现示例，位于可从 kernel.org 的 /pub/linux/daemons/autofs/v5 目录下载的发行版 tar 包中的文件 lib/dev-ioctl-lib.c 内。)

The device node ioctl operations implemented by this interface are:
(此接口实现的设备节点 ioctl 操作如下：)


AUTOFS_DEV_IOCTL_VERSION
------------------------

Get the major and minor version of the autofs device ioctl kernel module
implementation. It requires an initialized struct autofs_dev_ioctl as an
input parameter and sets the version information in the passed in structure.
It returns 0 on success or the error -EINVAL if a version mismatch is
detected.
(获取 autofs 设备 ioctl 内核模块实现的主版本和次版本。它需要一个已初始化的 struct autofs_dev_ioctl 作为输入参数，并在传入的结构中设置版本信息。成功时返回 0，如果检测到版本不匹配则返回错误 -EINVAL。)


AUTOFS_DEV_IOCTL_PROTOVER_CMD and AUTOFS_DEV_IOCTL_PROTOSUBVER_CMD
------------------------------------------------------------------

Get the major and minor version of the autofs protocol version understood
by loaded module. This call requires an initialized struct autofs_dev_ioctl
with the ioctlfd field set to a valid autofs mount point descriptor
and sets the requested version number in version field of struct args_protover
or sub_version field of struct args_protosubver. These commands return
0 on success or one of the negative error codes if validation fails.
(获取已加载模块所理解的 autofs 协议版本的主版本和次版本。此调用需要一个已初始化的 struct autofs_dev_ioctl，其 ioctlfd 字段设置为有效的 autofs 挂载点描述符，并将请求的版本号设置到 struct args_protover 的 version 字段或 struct args_protosubver 的 sub_version 字段中。这些命令成功时返回 0，验证失败时返回某个负数错误码。)


AUTOFS_DEV_IOCTL_OPENMOUNT and AUTOFS_DEV_IOCTL_CLOSEMOUNT
----------------------------------------------------------

Obtain and release a file descriptor for an autofs managed mount point
path. The open call requires an initialized struct autofs_dev_ioctl with
the path field set and the size field adjusted appropriately as well
as the devid field of struct args_openmount set to the device number of
the autofs mount. The device number can be obtained from the mount options
shown in /proc/mounts. The close call requires an initialized struct
autofs_dev_ioct with the ioctlfd field set to the descriptor obtained
from the open call. The release of the file descriptor can also be done
with close(2) so any open descriptors will also be closed at process exit.
The close call is included in the implemented operations largely for
completeness and to provide for a consistent user space implementation.
(获取并释放 autofs 管理的挂载点路径的文件描述符。open 调用需要一个已初始化的 struct autofs_dev_ioctl，其中 path 字段已设置、size 字段已适当调整，并且 struct args_openmount 的 devid 字段设置为该 autofs 挂载的设备号。设备号可以从 /proc/mounts 中显示的挂载选项获得。close 调用需要一个已初始化的 struct autofs_dev_ioct，其 ioctlfd 字段设置为从 open 调用获得的描述符。文件描述符的释放也可以通过 close(2) 完成，因此任何打开的描述符也会在进程退出时关闭。close 调用被包含在已实现的操作中，主要是为了完整性并提供一致的用户空间实现。)


AUTOFS_DEV_IOCTL_READY_CMD and AUTOFS_DEV_IOCTL_FAIL_CMD
--------------------------------------------------------

Return mount and expire result status from user space to the kernel.
Both of these calls require an initialized struct autofs_dev_ioctl
with the ioctlfd field set to the descriptor obtained from the open
call and the token field of struct args_ready or struct args_fail set
to the wait queue token number, received by user space in the foregoing
mount or expire request. The status field of struct args_fail is set to
the errno of the operation. It is set to 0 on success.
(将挂载和过期的结果状态从用户空间返回给内核。这两个调用都需要一个已初始化的 struct autofs_dev_ioctl，其 ioctlfd 字段设置为从 open 调用获得的描述符，并且 struct args_ready 或 struct args_fail 的 token 字段设置为等待队列令牌号，该令牌号由用户空间在前述挂载或过期请求中接收。struct args_fail 的 status 字段设置为该操作的 errno。成功时它被设置为 0。)


AUTOFS_DEV_IOCTL_SETPIPEFD_CMD
------------------------------

Set the pipe file descriptor used for kernel communication to the daemon.
Normally this is set at mount time using an option but when reconnecting
to a existing mount we need to use this to tell the autofs mount about
the new kernel pipe descriptor. In order to protect mounts against
incorrectly setting the pipe descriptor we also require that the autofs
mount be catatonic (see next call).
(设置用于内核与 daemon 通信的管道文件描述符。通常这在挂载时使用一个选项设置，但当重新连接到现有挂载时，我们需要使用此操作来告知 autofs 挂载新的内核管道描述符。为了保护挂载免受管道描述符的错误设置，我们还要求该 autofs 挂载处于 catatonic 状态(参见下一个调用)。)

The call requires an initialized struct autofs_dev_ioctl with the
ioctlfd field set to the descriptor obtained from the open call and
the pipefd field of struct args_setpipefd set to descriptor of the pipe.
On success the call also sets the process group id used to identify the
controlling process (eg. the owning automount(8) daemon) to the process
group of the caller.
(该调用需要一个已初始化的 struct autofs_dev_ioctl，其 ioctlfd 字段设置为从 open 调用获得的描述符，并且 struct args_setpipefd 的 pipefd 字段设置为该管道的描述符。成功时，该调用还会将用于标识控制进程(例如拥有它的 automount(8) daemon)的进程组 id 设置为调用者的进程组。)


AUTOFS_DEV_IOCTL_CATATONIC_CMD
------------------------------

Make the autofs mount point catatonic. The autofs mount will no longer
issue mount requests, the kernel communication pipe descriptor is released
and any remaining waits in the queue released.
(使 autofs 挂载点进入 catatonic 状态。该 autofs 挂载将不再发出挂载请求，内核通信管道描述符被释放，队列中任何剩余的等待也被释放。)

The call requires an initialized struct autofs_dev_ioctl with the
ioctlfd field set to the descriptor obtained from the open call.
(该调用需要一个已初始化的 struct autofs_dev_ioctl，其 ioctlfd 字段设置为从 open 调用获得的描述符。)


AUTOFS_DEV_IOCTL_TIMEOUT_CMD
----------------------------

Set the expire timeout for mounts within an autofs mount point.
(为 autofs 挂载点内的挂载设置过期超时。)

The call requires an initialized struct autofs_dev_ioctl with the
ioctlfd field set to the descriptor obtained from the open call.
(该调用需要一个已初始化的 struct autofs_dev_ioctl，其 ioctlfd 字段设置为从 open 调用获得的描述符。)


AUTOFS_DEV_IOCTL_REQUESTER_CMD
------------------------------

Return the uid and gid of the last process to successfully trigger a the
mount on the given path dentry.
(返回最后成功触发给定路径 dentry 上挂载的进程的 uid 和 gid。)

The call requires an initialized struct autofs_dev_ioctl with the path
field set to the mount point in question and the size field adjusted
appropriately. Upon return the uid field of struct args_requester contains
the uid and gid field the gid.
(该调用需要一个已初始化的 struct autofs_dev_ioctl，其 path 字段设置为相关挂载点，并且 size 字段已适当调整。返回时，struct args_requester 的 uid 字段包含 uid，gid 字段包含 gid。)

When reconstructing an autofs mount tree with active mounts we need to
re-connect to mounts that may have used the original process uid and
gid (or string variations of them) for mount lookups within the map entry.
This call provides the ability to obtain this uid and gid so they may be
used by user space for the mount map lookups.
(在重建带有活动挂载的 autofs 挂载树时，我们需要重新连接到可能在映射条目内的挂载查找中使用了原始进程 uid 和 gid(或它们的字符串变体)的挂载。此调用提供了获取此 uid 和 gid 的能力，以便用户空间可以将它们用于挂载映射查找。)


AUTOFS_DEV_IOCTL_EXPIRE_CMD
---------------------------

Issue an expire request to the kernel for an autofs mount. Typically
this ioctl is called until no further expire candidates are found.
(为某个 autofs 挂载向内核发出过期请求。通常会一直调用此 ioctl，直到找不到更多的过期候选者为止。)

The call requires an initialized struct autofs_dev_ioctl with the
ioctlfd field set to the descriptor obtained from the open call. In
addition an immediate expire that's independent of the mount timeout,
and a forced expire that's independent of whether the mount is busy,
can be requested by setting the how field of struct args_expire to
AUTOFS_EXP_IMMEDIATE or AUTOFS_EXP_FORCED, respectively . If no
expire candidates can be found the ioctl returns -1 with errno set to
EAGAIN.
(该调用需要一个已初始化的 struct autofs_dev_ioctl，其 ioctlfd 字段设置为从 open 调用获得的描述符。此外，可以通过将 struct args_expire 的 how 字段分别设置为 AUTOFS_EXP_IMMEDIATE 或 AUTOFS_EXP_FORCED，来请求独立于挂载超时的立即过期，以及独立于挂载是否繁忙的强制过期。如果找不到过期候选者，该 ioctl 返回 -1 并将 errno 设置为 EAGAIN。)

This call causes the kernel module to check the mount corresponding
to the given ioctlfd for mounts that can be expired, issues an expire
request back to the daemon and waits for completion.
(此调用使内核模块检查与给定 ioctlfd 对应的挂载中是否存在可以过期的挂载，向 daemon 发回一个过期请求并等待完成。)

AUTOFS_DEV_IOCTL_ASKUMOUNT_CMD
------------------------------

Checks if an autofs mount point is in use.
(检查一个 autofs 挂载点是否正在使用中。)

The call requires an initialized struct autofs_dev_ioctl with the
ioctlfd field set to the descriptor obtained from the open call and
it returns the result in the may_umount field of struct args_askumount,
1 for busy and 0 otherwise.
(该调用需要一个已初始化的 struct autofs_dev_ioctl，其 ioctlfd 字段设置为从 open 调用获得的描述符，并在 struct args_askumount 的 may_umount 字段中返回结果，繁忙时为 1，否则为 0。)


AUTOFS_DEV_IOCTL_ISMOUNTPOINT_CMD
---------------------------------

Check if the given path is a mountpoint.
(检查给定路径是否为挂载点。)

The call requires an initialized struct autofs_dev_ioctl. There are two
possible variations. Both use the path field set to the path of the mount
point to check and the size field adjusted appropriately. One uses the
ioctlfd field to identify a specific mount point to check while the other
variation uses the path and optionally in.type field of struct args_ismountpoint
set to an autofs mount type. The call returns 1 if this is a mount point
and sets out.devid field to the device number of the mount and out.magic
field to the relevant super block magic number (described below) or 0 if
it isn't a mountpoint. In both cases the device number (as returned
by new_encode_dev()) is returned in out.devid field.
(该调用需要一个已初始化的 struct autofs_dev_ioctl。有两种可能的变体。两者都将 path 字段设置为要检查的挂载点路径，并将 size 字段适当调整。一种使用 ioctlfd 字段来标识要检查的特定挂载点，而另一种变体使用 path，并可选地将 struct args_ismountpoint 的 in.type 字段设置为 autofs 挂载类型。如果这是一个挂载点，该调用返回 1，并将 out.devid 字段设置为该挂载的设备号、将 out.magic 字段设置为相关的超级块魔数(如下所述)；如果不是挂载点则返回 0。在这两种情况下，设备号(如 new_encode_dev() 所返回的)都会在 out.devid 字段中返回。)

If supplied with a file descriptor we're looking for a specific mount,
not necessarily at the top of the mounted stack. In this case the path
the descriptor corresponds to is considered a mountpoint if it is itself
a mountpoint or contains a mount, such as a multi-mount without a root
mount. In this case we return 1 if the descriptor corresponds to a mount
point and also returns the super magic of the covering mount if there
is one or 0 if it isn't a mountpoint.
(如果提供了文件描述符，我们要查找的是一个特定的挂载，它不一定位于挂载栈的顶部。在这种情况下，如果描述符对应的路径本身是一个挂载点或者包含一个挂载(例如没有根挂载的多挂载)，则该路径被视为挂载点。在这种情况下，如果描述符对应于一个挂载点，我们返回 1，并且如果存在覆盖它的挂载，还会返回该覆盖挂载的超级块魔数；如果不是挂载点则返回 0。)

If a path is supplied (and the ioctlfd field is set to -1) then the path
is looked up and is checked to see if it is the root of a mount. If a
type is also given we are looking for a particular autofs mount and if
a match isn't found a fail is returned. If the located path is the
root of a mount 1 is returned along with the super magic of the mount
or 0 otherwise.
(如果提供的是路径(且 ioctlfd 字段设置为 -1)，则会查找该路径并检查它是否为某个挂载的根。如果还给定了类型，我们要查找的是一个特定的 autofs 挂载，如果没有找到匹配则返回失败。如果找到的路径是某个挂载的根，则返回 1 以及该挂载的超级块魔数，否则返回 0。)
