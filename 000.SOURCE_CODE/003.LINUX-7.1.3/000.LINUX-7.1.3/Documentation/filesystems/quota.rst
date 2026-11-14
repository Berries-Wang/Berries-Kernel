.. SPDX-License-Identifier: GPL-2.0

===============
Quota subsystem
===============

(配额子系统)

Quota subsystem allows system administrator to set limits on used space and
number of used inodes (inode is a filesystem structure which is associated with
each file or directory) for users and/or groups. For both used space and number
of used inodes there are actually two limits. The first one is called softlimit
and the second one hardlimit.  A user can never exceed a hardlimit for any
resource (unless he has CAP_SYS_RESOURCE capability). User is allowed to exceed
softlimit but only for limited period of time. This period is called "grace
period" or "grace time". When grace time is over, user is not able to allocate
more space/inodes until he frees enough of them to get below softlimit.
(配额子系统允许系统管理员为用户和/或组设置已用空间和已用 inode 数量的限制(inode 是与每个文件或目录相关联的文件系统结构)。对于已用空间和已用 inode 数量，实际上存在两个限制。第一个称为软限制(softlimit)，第二个称为硬限制(hardlimit)。用户永远不能超过任何资源的硬限制(除非他拥有 CAP_SYS_RESOURCE 能力)。允许用户超过软限制，但只能在有限的时间段内。这段时间称为 "grace period"(宽限期)或 "grace time"(宽限时间)。当宽限时间结束后，用户将无法分配更多空间/inode，直到他释放足够多的空间/inode 使其降到软限制以下。)

Quota limits (and amount of grace time) are set independently for each
filesystem.
(配额限制(以及宽限时间量)为每个文件系统独立设置。)

For more details about quota design, see the documentation in quota-tools package
(https://sourceforge.net/projects/linuxquota).
(有关配额设计的更多细节，请参见 quota-tools 软件包中的文档(https://sourceforge.net/projects/linuxquota)。)

Quota netlink interface
(配额 netlink 接口)
=======================
When user exceeds a softlimit, runs out of grace time or reaches hardlimit,
quota subsystem traditionally printed a message to the controlling terminal of
the process which caused the excess. This method has the disadvantage that
when user is using a graphical desktop he usually cannot see the message.
Thus quota netlink interface has been designed to pass information about
the above events to userspace. There they can be captured by an application
and processed accordingly.
(当用户超过软限制、用完宽限时间或达到硬限制时，配额子系统传统上会向导致超额的进程的控制终端打印一条消息。这种方法的缺点是，当用户使用图形桌面时，他通常看不到该消息。因此，配额 netlink 接口被设计用来将上述事件的信息传递给用户空间。在那里，它们可以被应用程序捕获并相应地进行处理。)

The interface uses generic netlink framework (see
https://lwn.net/Articles/208755/ and http://www.infradead.org/~tgr/libnl/ for
more details about this layer). The name of the quota generic netlink interface
is "VFS_DQUOT". Definitions of constants below are in <linux/quota.h>.  Since
the quota netlink protocol is not namespace aware, quota netlink messages are
sent only in initial network namespace.
(该接口使用通用 netlink 框架(关于这一层的更多细节，请参见 https://lwn.net/Articles/208755/ 和 http://www.infradead.org/~tgr/libnl/)。配额通用 netlink 接口的名称是 "VFS_DQUOT"。下面各常量的定义位于 <linux/quota.h> 中。由于配额 netlink 协议不感知命名空间，配额 netlink 消息只在初始网络命名空间中发送。)

Currently, the interface supports only one message type QUOTA_NL_C_WARNING.
This command is used to send a notification about any of the above mentioned
events. Each message has six attributes. These are (type of the argument is
in parentheses):
(目前，该接口只支持一种消息类型 QUOTA_NL_C_WARNING。此命令用于发送对上述任一事件的通知。每条消息有六个属性。它们是(参数类型在括号中)：)

        QUOTA_NL_A_QTYPE (u32)
	  - type of quota being exceeded (one of USRQUOTA, GRPQUOTA)
	    (被超过的配额类型(USRQUOTA、GRPQUOTA 之一))
        QUOTA_NL_A_EXCESS_ID (u64)
	  - UID/GID (depends on quota type) of user / group whose limit
	    is being exceeded.
	    (其限制正被超过的用户/组的 UID/GID(取决于配额类型)。)
        QUOTA_NL_A_CAUSED_ID (u64)
	  - UID of a user who caused the event
	    (导致该事件的用户的 UID)
        QUOTA_NL_A_WARNING (u32)
	  - what kind of limit is exceeded:
	    (超过了哪种类型的限制：)

		QUOTA_NL_IHARDWARN
		    inode hardlimit
		    (inode 硬限制)
		QUOTA_NL_ISOFTLONGWARN
		    inode softlimit is exceeded longer
		    than given grace period
		    (inode 软限制被超过的时间长于给定的宽限期)
		QUOTA_NL_ISOFTWARN
		    inode softlimit
		    (inode 软限制)
		QUOTA_NL_BHARDWARN
		    space (block) hardlimit
		    (空间(块)硬限制)
		QUOTA_NL_BSOFTLONGWARN
		    space (block) softlimit is exceeded
		    longer than given grace period.
		    (空间(块)软限制被超过的时间长于给定的宽限期。)
		QUOTA_NL_BSOFTWARN
		    space (block) softlimit
		    (空间(块)软限制)

	  - four warnings are also defined for the event when user stops
	    exceeding some limit:
	    (还为用户停止超过某个限制的事件定义了四个警告：)

		QUOTA_NL_IHARDBELOW
		    inode hardlimit
		    (inode 硬限制)
		QUOTA_NL_ISOFTBELOW
		    inode softlimit
		    (inode 软限制)
		QUOTA_NL_BHARDBELOW
		    space (block) hardlimit
		    (空间(块)硬限制)
		QUOTA_NL_BSOFTBELOW
		    space (block) softlimit
		    (空间(块)软限制)

        QUOTA_NL_A_DEV_MAJOR (u32)
	  - major number of a device with the affected filesystem
	    (受影响文件系统所在设备的主设备号)
        QUOTA_NL_A_DEV_MINOR (u32)
	  - minor number of a device with the affected filesystem
	    (受影响文件系统所在设备的次设备号)
