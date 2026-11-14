.. SPDX-License-Identifier: GPL-2.0

=============
FUSE Overview
=============

(FUSE 概述)

Definitions
(定义)
===========

Userspace filesystem:
  A filesystem in which data and metadata are provided by an ordinary
  userspace process.  The filesystem can be accessed normally through
  the kernel interface.
  (一种数据和元数据由普通用户空间进程提供的文件系统。该文件系统可以通过内核接口正常访问。)

Filesystem daemon:
  The process(es) providing the data and metadata of the filesystem.
  (提供文件系统数据和元数据的进程。)

Non-privileged mount (or user mount):
  A userspace filesystem mounted by a non-privileged (non-root) user.
  The filesystem daemon is running with the privileges of the mounting
  user.  NOTE: this is not the same as mounts allowed with the "user"
  option in /etc/fstab, which is not discussed here.
  (由非特权(非 root)用户挂载的用户空间文件系统。文件系统 daemon 以挂载用户的权限运行。注意: 这与 /etc/fstab 中 "user" 选项所允许的挂载不同，后者不在此讨论之列。)

Filesystem connection:
  A connection between the filesystem daemon and the kernel.  The
  connection exists until either the daemon dies, or the filesystem is
  umounted.  Note that detaching (or lazy umounting) the filesystem
  does *not* break the connection, in this case it will exist until
  the last reference to the filesystem is released.
  (文件系统 daemon 与内核之间的连接。该连接一直存在，直到 daemon 死亡或文件系统被卸载。注意，分离(或延迟卸载)文件系统*不会*断开该连接，在这种情况下，连接将一直存在，直到对文件系统的最后一个引用被释放。)

Mount owner:
  The user who does the mounting.
  (执行挂载的用户。)

User:
  The user who is performing filesystem operations.
  (正在执行文件系统操作的用户。)

What is FUSE?
(什么是 FUSE?)
=============

FUSE is a userspace filesystem framework.  It consists of a kernel
module (fuse.ko), a userspace library (libfuse.*) and a mount utility
(fusermount).
(FUSE 是一个用户空间文件系统框架。它由一个内核模块(fuse.ko)、一个用户空间库(libfuse.*)和一个挂载工具(fusermount)组成。)

One of the most important features of FUSE is allowing secure,
non-privileged mounts.  This opens up new possibilities for the use of
filesystems.  A good example is sshfs: a secure network filesystem
using the sftp protocol.
(FUSE 最重要的特性之一是允许安全的非特权挂载。这为文件系统的使用开辟了新的可能性。一个很好的例子是 sshfs: 一种使用 sftp 协议的安全网络文件系统。)

The userspace library and utilities are available from the
`FUSE homepage: <https://github.com/libfuse/>`_
(用户空间库和实用工具可从 `FUSE homepage: <https://github.com/libfuse/>`_ 获取。)

Filesystem type
(文件系统类型)
===============

The filesystem type given to mount(2) can be one of the following:
(传递给 mount(2) 的文件系统类型可以是以下之一:)

    fuse
      This is the usual way to mount a FUSE filesystem.  The first
      argument of the mount system call may contain an arbitrary string,
      which is not interpreted by the kernel.
      (这是挂载 FUSE 文件系统的常用方式。mount 系统调用的第一个参数可以包含任意字符串，内核不会解释它。)

    fuseblk
      The filesystem is block device based.  The first argument of the
      mount system call is interpreted as the name of the device.
      (该文件系统基于块设备。mount 系统调用的第一个参数被解释为设备名。)

Mount options
(挂载选项)
=============

fd=N
  The file descriptor to use for communication between the userspace
  filesystem and the kernel.  The file descriptor must have been
  obtained by opening the FUSE device ('/dev/fuse').
  (用于用户空间文件系统与内核之间通信的文件描述符。该文件描述符必须是通过打开 FUSE 设备('/dev/fuse')获得的。)

rootmode=M
  The file mode of the filesystem's root in octal representation.
  (文件系统根目录的文件模式，以八进制表示。)

user_id=N
  The numeric user id of the mount owner.
  (挂载者的数字用户 id。)

group_id=N
  The numeric group id of the mount owner.
  (挂载者的数字组 id。)

default_permissions
  By default FUSE doesn't check file access permissions, the
  filesystem is free to implement its access policy or leave it to
  the underlying file access mechanism (e.g. in case of network
  filesystems).  This option enables permission checking, restricting
  access based on file mode.  It is usually useful together with the
  'allow_other' mount option.
  (默认情况下 FUSE 不检查文件访问权限，文件系统可以自行实现其访问策略，或者将其交给底层文件访问机制(例如网络文件系统的情况)。此选项启用权限检查，基于文件模式限制访问。它通常与 'allow_other' 挂载选项配合使用。)

allow_other
  This option overrides the security measure restricting file access
  to the user mounting the filesystem.  This option is by default only
  allowed to root, but this restriction can be removed with a
  (userspace) configuration option.
  (此选项覆盖了将文件访问限制为挂载文件系统的用户的安全措施。此选项默认仅允许 root 使用，但可以通过(用户空间)配置选项移除此限制。)

max_read=N
  With this option the maximum size of read operations can be set.
  The default is infinite.  Note that the size of read requests is
  limited anyway to 32 pages (which is 128kbyte on i386).
  (使用此选项可以设置读操作的最大大小。默认值为无限。请注意，读请求的大小无论如何都限制为 32 页(在 i386 上为 128k 字节)。)

blksize=N
  Set the block size for the filesystem.  The default is 512.  This
  option is only valid for 'fuseblk' type mounts.
  (设置文件系统的块大小。默认值为 512。此选项仅对 'fuseblk' 类型的挂载有效。)

Control filesystem
(控制文件系统)
==================

There's a control filesystem for FUSE, which can be mounted by::
(FUSE 有一个控制文件系统，可以通过如下方式挂载:)

  mount -t fusectl none /sys/fs/fuse/connections

Mounting it under the '/sys/fs/fuse/connections' directory makes it
backwards compatible with earlier versions.
(将其挂载在 '/sys/fs/fuse/connections' 目录下可以使其与早期版本保持向后兼容。)

Under the fuse control filesystem each connection has a directory
named by a unique number.
(在 fuse 控制文件系统中，每个连接都有一个以唯一数字命名的目录。)

For each connection the following files exist within this directory:
(对于每个连接，该目录中存在以下文件:)

	waiting
	  The number of requests which are waiting to be transferred to
	  userspace or being processed by the filesystem daemon.  If there is
	  no filesystem activity and 'waiting' is non-zero, then the
	  filesystem is hung or deadlocked.
	  (等待被传输到用户空间或正由文件系统 daemon 处理的请求数。如果没有文件系统活动且 'waiting' 非零，则文件系统已挂起或陷入死锁。)

	abort
	  Writing anything into this file will abort the filesystem
	  connection.  This means that all waiting requests will be aborted an
	  error returned for all aborted and new requests.
	  (向此文件写入任何内容都会中止该文件系统连接。这意味着所有等待的请求都会被中止，并且对所有被中止的请求和新的请求都会返回错误。)

        max_background
          The maximum number of background requests that can be outstanding
          at a time. When the number of background requests reaches this limit,
          further requests will be blocked until some are completed, potentially
          causing I/O operations to stall.
          (同一时间可以未完成的后台请求的最大数量。当后台请求数达到此限制时，后续请求将被阻塞，直到其中一些完成，这可能导致 I/O 操作停滞。)

        congestion_threshold
          The threshold of background requests at which the kernel considers
          the filesystem to be congested. When the number of background requests
          exceeds this value, the kernel will skip asynchronous readahead
          operations, reducing read-ahead optimizations but preserving essential
          I/O, as well as suspending non-synchronous writeback operations
          (WB_SYNC_NONE), delaying page cache flushing to the filesystem.
          (内核认为文件系统拥塞的后台请求阈值。当后台请求数超过此值时，内核将跳过异步预读操作，减少预读优化但保留必要的 I/O，同时暂停非同步回写操作(WB_SYNC_NONE)，延迟页缓存向文件系统的刷新。)

Only the owner of the mount may read or write these files.
(只有挂载的所有者才可以读取或写入这些文件。)

Interrupting filesystem operations
(中断文件系统操作)
##################################

If a process issuing a FUSE filesystem request is interrupted, the
following will happen:
(如果发出 FUSE 文件系统请求的进程被中断，将发生以下情况:)

  -  If the request is not yet sent to userspace AND the signal is
     fatal (SIGKILL or unhandled fatal signal), then the request is
     dequeued and returns immediately.
     (如果请求尚未发送到用户空间且信号是致命的(SIGKILL 或未处理的致命信号)，则该请求被出队并立即返回。)

  -  If the request is not yet sent to userspace AND the signal is not
     fatal, then an interrupted flag is set for the request.  When
     the request has been successfully transferred to userspace and
     this flag is set, an INTERRUPT request is queued.
     (如果请求尚未发送到用户空间且信号不是致命的，则为该请求设置一个中断标志。当请求已成功传输到用户空间且设置了此标志时，会有一个 INTERRUPT 请求被加入队列。)

  -  If the request is already sent to userspace, then an INTERRUPT
     request is queued.
     (如果请求已经发送到用户空间，则会有一个 INTERRUPT 请求被加入队列。)

INTERRUPT requests take precedence over other requests, so the
userspace filesystem will receive queued INTERRUPTs before any others.
(INTERRUPT 请求优先于其他请求，因此用户空间文件系统会在任何其他请求之前收到已排队的 INTERRUPT。)

The userspace filesystem may ignore the INTERRUPT requests entirely,
or may honor them by sending a reply to the *original* request, with
the error set to EINTR.
(用户空间文件系统可以完全忽略 INTERRUPT 请求，或者通过向*原始*请求发送错误设置为 EINTR 的回复来响应它们。)

It is also possible that there's a race between processing the
original request and its INTERRUPT request.  There are two possibilities:
(在处理原始请求和其 INTERRUPT 请求之间也可能存在竞争。有两种可能性:)

  1. The INTERRUPT request is processed before the original request is
     processed
     (INTERRUPT 请求在原始请求被处理之前得到处理)

  2. The INTERRUPT request is processed after the original request has
     been answered
     (INTERRUPT 请求在原始请求已被应答之后得到处理)

If the filesystem cannot find the original request, it should wait for
some timeout and/or a number of new requests to arrive, after which it
should reply to the INTERRUPT request with an EAGAIN error.  In case
1) the INTERRUPT request will be requeued.  In case 2) the INTERRUPT
reply will be ignored.
(如果文件系统找不到原始请求，它应该等待一段超时时间和/或一定数量的新请求到达，然后应以 EAGAIN 错误回复 INTERRUPT 请求。在情况 1) 中，INTERRUPT 请求将被重新排队。在情况 2) 中，INTERRUPT 回复将被忽略。)

Aborting a filesystem connection
(中止文件系统连接)
================================

It is possible to get into certain situations where the filesystem is
not responding.  Reasons for this may be:
(可能会遇到文件系统无响应的某些情况。原因可能包括:)

  a) Broken userspace filesystem implementation
     (用户空间文件系统实现损坏)

  b) Network connection down
     (网络连接断开)

  c) Accidental deadlock
     (意外死锁)

  d) Malicious deadlock
     (恶意死锁)

(For more on c) and d) see later sections)
((关于 c) 和 d) 的更多信息，请参见后面的章节。))

In either of these cases it may be useful to abort the connection to
the filesystem.  There are several ways to do this:
(在上述任何一种情况下，中止与文件系统的连接都可能是有用的。有几种方法可以做到这一点:)

  - Kill the filesystem daemon.  Works in case of a) and b)
    (杀死文件系统 daemon。适用于情况 a) 和 b)。)

  - Kill the filesystem daemon and all users of the filesystem.  Works
    in all cases except some malicious deadlocks
    (杀死文件系统 daemon 以及文件系统的所有使用者。除某些恶意死锁外，适用于所有情况)

  - Use forced umount (umount -f).  Works in all cases but only if
    filesystem is still attached (it hasn't been lazy unmounted)
    (使用强制卸载(umount -f)。适用于所有情况，但仅当文件系统仍然处于挂接状态时有效(它尚未被延迟卸载))

  - Abort filesystem through the FUSE control filesystem.  Most
    powerful method, always works.
    (通过 FUSE 控制文件系统中止文件系统。最强大的方法，始终有效。)

How do non-privileged mounts work?
(非特权挂载是如何工作的?)
==================================

Since the mount() system call is a privileged operation, a helper
program (fusermount) is needed, which is installed setuid root.
(由于 mount() 系统调用是特权操作，因此需要一个辅助程序(fusermount)，它以 setuid root 方式安装。)

The implication of providing non-privileged mounts is that the mount
owner must not be able to use this capability to compromise the
system.  Obvious requirements arising from this are:
(提供非特权挂载意味着挂载者不得利用此能力来危害系统。由此产生的一些明显要求是:)

 A) mount owner should not be able to get elevated privileges with the
    help of the mounted filesystem
    (挂载者不应能够借助被挂载的文件系统获得提升的特权)

 B) mount owner should not get illegitimate access to information from
    other users' and the super user's processes
    (挂载者不应非法获取来自其他用户和超级用户进程的信息)

 C) mount owner should not be able to induce undesired behavior in
    other users' or the super user's processes
    (挂载者不应能够在其他用户或超级用户的进程中引发不期望的行为)

How are requirements fulfilled?
(这些要求是如何满足的?)
===============================

 A) The mount owner could gain elevated privileges by either:
    (挂载者可以通过以下两种方式之一获得提升的特权:)

    1. creating a filesystem containing a device file, then opening this device
       (创建一个包含设备文件的文件系统，然后打开此设备)

    2. creating a filesystem containing a suid or sgid application, then executing this application
       (创建一个包含 suid 或 sgid 应用程序的文件系统，然后执行此应用程序)

    The solution is not to allow opening device files and ignore
    setuid and setgid bits when executing programs.  To ensure this
    fusermount always adds "nosuid" and "nodev" to the mount options
    for non-privileged mounts.
    (解决方案是不允许打开设备文件，并在执行程序时忽略 setuid 和 setgid 位。为确保这一点，fusermount 总是在非特权挂载的挂载选项中添加 "nosuid" 和 "nodev"。)

 B) If another user is accessing files or directories in the
    filesystem, the filesystem daemon serving requests can record the
    exact sequence and timing of operations performed.  This
    information is otherwise inaccessible to the mount owner, so this
    counts as an information leak.
    (如果另一个用户正在访问文件系统中的文件或目录，服务于请求的文件系统 daemon 可以记录所执行操作的确切顺序和时间。这些信息对挂载者而言通常是不可见的，因此这算作一次信息泄露。)

    The solution to this problem will be presented in point 2) of C).
    (此问题的解决方案将在 C) 的第 2) 点中给出。)

 C) There are several ways in which the mount owner can induce
    undesired behavior in other users' processes, such as:
    (挂载者可以通过多种方式在其他用户的进程中引发不期望的行为，例如:)

     1) mounting a filesystem over a file or directory which the mount
        owner could otherwise not be able to modify (or could only
        make limited modifications).
        (在挂载者原本无法修改(或只能进行有限修改)的文件或目录之上挂载一个文件系统。)

        This is solved in fusermount, by checking the access
        permissions on the mountpoint and only allowing the mount if
        the mount owner can do unlimited modification (has write
        access to the mountpoint, and mountpoint is not a "sticky"
        directory)
        (这在 fusermount 中通过检查挂载点上的访问权限来解决，只有当挂载者可以进行无限制的修改(对挂载点拥有写访问权限，且挂载点不是 "sticky" 目录)时才允许挂载)

     2) Even if 1) is solved the mount owner can change the behavior
        of other users' processes.
        (即使 1) 得到了解决，挂载者仍然可以改变其他用户进程的行为。)

         i) It can slow down or indefinitely delay the execution of a
            filesystem operation creating a DoS against the user or the
            whole system.  For example a suid application locking a
            system file, and then accessing a file on the mount owner's
            filesystem could be stopped, and thus causing the system
            file to be locked forever.
            (它可以减慢或无限期地延迟文件系统操作的执行，从而针对用户或整个系统制造 DoS。例如，一个 suid 应用程序锁定一个系统文件，然后访问挂载者文件系统上的一个文件时可能被停止，从而导致该系统文件被永久锁定。)

         ii) It can present files or directories of unlimited length, or
             directory structures of unlimited depth, possibly causing a
             system process to eat up diskspace, memory or other
             resources, again causing *DoS*.
             (它可以呈现无限长度的文件或目录，或无限深度的目录结构，可能导致系统进程耗尽磁盘空间、内存或其他资源，再次造成 *DoS*。)

	The solution to this as well as B) is not to allow processes
	to access the filesystem, which could otherwise not be
	monitored or manipulated by the mount owner.  Since if the
	mount owner can ptrace a process, it can do all of the above
	without using a FUSE mount, the same criteria as used in
	ptrace can be used to check if a process is allowed to access
	the filesystem or not.
	(针对此问题以及 B) 的解决方案是不允许进程访问文件系统，否则挂载者将无法监视或操纵这些进程。由于如果挂载者能够 ptrace 一个进程，它无需使用 FUSE 挂载就能完成上述所有操作，因此可以使用与 ptrace 中相同的标准来检查一个进程是否被允许访问文件系统。)

	Note that the *ptrace* check is not strictly necessary to
	prevent C/2/i, it is enough to check if mount owner has enough
	privilege to send signal to the process accessing the
	filesystem, since *SIGSTOP* can be used to get a similar effect.
	(请注意，*ptrace* 检查对于防止 C/2/i 并非严格必需，只需检查挂载者是否拥有足够的特权向访问文件系统的进程发送信号即可，因为 *SIGSTOP* 可以用来达到类似的效果。)

I think these limitations are unacceptable?
(我认为这些限制是不可接受的?)
===========================================

If a sysadmin trusts the users enough, or can ensure through other
measures, that system processes will never enter non-privileged
mounts, it can relax the last limitation in several ways:
(如果系统管理员足够信任用户，或者能够通过其他措施确保系统进程永远不会进入非特权挂载，那么可以通过以下几种方式放宽最后一项限制:)

  - With the 'user_allow_other' config option. If this config option is
    set, the mounting user can add the 'allow_other' mount option which
    disables the check for other users' processes.
    (使用 'user_allow_other' 配置选项。如果设置了此配置选项，挂载用户可以添加 'allow_other' 挂载选项，从而禁用对其他用户进程的检查。)

    User namespaces have an unintuitive interaction with 'allow_other':
    an unprivileged user - normally restricted from mounting with
    'allow_other' - could do so in a user namespace where they're
    privileged. If any process could access such an 'allow_other' mount
    this would give the mounting user the ability to manipulate
    processes in user namespaces where they're unprivileged. For this
    reason 'allow_other' restricts access to users in the same userns
    or a descendant.
    (用户命名空间与 'allow_other' 之间有一种不直观的交互: 一个无特权的用户——通常被限制使用 'allow_other' 进行挂载——可以在其拥有特权的用户命名空间中这样做。如果任何进程都可以访问这样的 'allow_other' 挂载，这将使挂载用户能够操纵其没有特权的用户命名空间中的进程。因此，'allow_other' 将访问限制为同一 userns 或其后代中的用户。)

  - With the 'allow_sys_admin_access' module option. If this option is
    set, super user's processes have unrestricted access to mounts
    irrespective of allow_other setting or user namespace of the
    mounting user.
    (使用 'allow_sys_admin_access' 模块选项。如果设置了此选项，无论 allow_other 设置或挂载用户的用户命名空间如何，超级用户的进程都对挂载拥有不受限制的访问权限。)

Note that both of these relaxations expose the system to potential
information leak or *DoS* as described in points B and C/2/i-ii in the
preceding section.
(请注意，这两种放宽方式都会使系统面临上一节中 B 点和 C/2/i-ii 点所描述的潜在信息泄露或 *DoS* 风险。)

Kernel - userspace interface
(内核 - 用户空间接口)
============================

The following diagram shows how a filesystem operation (in this
example unlink) is performed in FUSE. ::
(下图显示了文件系统操作(在本例中为 unlink)在 FUSE 中是如何执行的：)


 |  "rm /mnt/fuse/file"               |  FUSE filesystem daemon
 |                                    |
 |                                    |  >sys_read()
 |                                    |    >fuse_dev_read()
 |                                    |      >request_wait()
 |                                    |        [sleep on fc->waitq]
 |                                    |
 |  >sys_unlink()                     |
 |    >fuse_unlink()                  |
 |      [get request from             |
 |       fc->unused_list]             |
 |      >request_send()               |
 |        [queue req on fc->pending]  |
 |        [wake up fc->waitq]         |        [woken up]
 |        >request_wait_answer()      |
 |          [sleep on req->waitq]     |
 |                                    |      <request_wait()
 |                                    |      [remove req from fc->pending]
 |                                    |      [copy req to read buffer]
 |                                    |      [add req to fc->processing]
 |                                    |    <fuse_dev_read()
 |                                    |  <sys_read()
 |                                    |
 |                                    |  [perform unlink]
 |                                    |
 |                                    |  >sys_write()
 |                                    |    >fuse_dev_write()
 |                                    |      [look up req in fc->processing]
 |                                    |      [remove from fc->processing]
 |                                    |      [copy write buffer to req]
 |          [woken up]                |      [wake up req->waitq]
 |                                    |    <fuse_dev_write()
 |                                    |  <sys_write()
 |        <request_wait_answer()      |
 |      <request_send()               |
 |      [add request to               |
 |       fc->unused_list]             |
 |    <fuse_unlink()                  |
 |  <sys_unlink()                     |

.. note:: Everything in the description above is greatly simplified
          (注意: 上述描述中的所有内容都经过了大幅简化。)

There are a couple of ways in which to deadlock a FUSE filesystem.
Since we are talking about unprivileged userspace programs,
something must be done about these.
(有几种方法会使 FUSE 文件系统陷入死锁。由于我们讨论的是无特权的用户空间程序，因此必须针对这些问题采取一些措施。)

**Scenario 1 -  Simple deadlock**::
(场景 1 - 简单死锁：)

 |  "rm /mnt/fuse/file"               |  FUSE filesystem daemon
 |                                    |
 |  >sys_unlink("/mnt/fuse/file")     |
 |    [acquire inode semaphore        |
 |     for "file"]                    |
 |    >fuse_unlink()                  |
 |      [sleep on req->waitq]         |
 |                                    |  <sys_read()
 |                                    |  >sys_unlink("/mnt/fuse/file")
 |                                    |    [acquire inode semaphore
 |                                    |     for "file"]
 |                                    |    *DEADLOCK*

The solution for this is to allow the filesystem to be aborted.
(对此的解决方案是允许文件系统被中止。)

**Scenario 2 - Tricky deadlock**
(场景 2 - 棘手的死锁)


This one needs a carefully crafted filesystem.  It's a variation on
the above, only the call back to the filesystem is not explicit,
but is caused by a pagefault. ::
(这种情况需要一个精心构造的文件系统。它是上面情况的一个变体，只是对文件系统的回调不是显式的，而是由缺页异常引起的：)

 |  Kamikaze filesystem thread 1      |  Kamikaze filesystem thread 2
 |                                    |
 |  [fd = open("/mnt/fuse/file")]     |  [request served normally]
 |  [mmap fd to 'addr']               |
 |  [close fd]                        |  [FLUSH triggers 'magic' flag]
 |  [read a byte from addr]           |
 |    >do_page_fault()                |
 |      [find or create page]         |
 |      [lock page]                   |
 |      >fuse_readpage()              |
 |         [queue READ request]       |
 |         [sleep on req->waitq]      |
 |                                    |  [read request to buffer]
 |                                    |  [create reply header before addr]
 |                                    |  >sys_write(addr - headerlength)
 |                                    |    >fuse_dev_write()
 |                                    |      [look up req in fc->processing]
 |                                    |      [remove from fc->processing]
 |                                    |      [copy write buffer to req]
 |                                    |        >do_page_fault()
 |                                    |           [find or create page]
 |                                    |           [lock page]
 |                                    |           * DEADLOCK *

The solution is basically the same as above.
(解决方案与上面基本相同。)

An additional problem is that while the write buffer is being copied
to the request, the request must not be interrupted/aborted.  This is
because the destination address of the copy may not be valid after the
request has returned.
(另一个问题是，在将写缓冲区复制到请求期间，该请求不得被中断/中止。这是因为在请求返回之后，复制的目标地址可能不再有效。)

This is solved with doing the copy atomically, and allowing abort
while the page(s) belonging to the write buffer are faulted with
get_user_pages().  The 'req->locked' flag indicates when the copy is
taking place, and abort is delayed until this flag is unset.
(这通过原子地执行复制来解决，并且当属于写缓冲区的页通过 get_user_pages() 发生缺页时允许中止。'req->locked' 标志指示复制何时正在进行，中止操作会被延迟到该标志被清除。)
