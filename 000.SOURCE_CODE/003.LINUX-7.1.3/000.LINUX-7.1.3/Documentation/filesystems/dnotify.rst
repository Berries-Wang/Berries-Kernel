.. SPDX-License-Identifier: GPL-2.0

============================
Linux Directory Notification
============================

(Linux 目录通知)

	   Stephen Rothwell <sfr@canb.auug.org.au>
(作者: Stephen Rothwell <sfr@canb.auug.org.au>)

The intention of directory notification is to allow user applications
to be notified when a directory, or any of the files in it, are changed.
The basic mechanism involves the application registering for notification
on a directory using a fcntl(2) call and the notifications themselves
being delivered using signals.
(目录通知的目的在于，当目录或其中的任何文件发生变化时，让用户应用程序能够收到通知。基本机制包括：应用程序使用 fcntl(2) 调用在目录上注册通知，而通知本身则通过信号传递。)

The application decides which "events" it wants to be notified about.
The currently defined events are:
(由应用程序决定它希望收到哪些"事件"的通知。当前定义的事件有：)

	=========	=====================================================
	DN_ACCESS	A file in the directory was accessed (read) (目录中的一个文件被访问(读))
	DN_MODIFY	A file in the directory was modified (write,truncate) (目录中的一个文件被修改(写、截断))
	DN_CREATE	A file was created in the directory (在目录中创建了一个文件)
	DN_DELETE	A file was unlinked from directory (一个文件从目录中解除链接)
	DN_RENAME	A file in the directory was renamed (目录中的一个文件被重命名)
	DN_ATTRIB	A file in the directory had its attributes
			changed (chmod,chown) (目录中的一个文件的属性被更改(chmod、chown))
	=========	=====================================================

Usually, the application must reregister after each notification, but
if DN_MULTISHOT is or'ed with the event mask, then the registration will
remain until explicitly removed (by registering for no events).
(通常，应用程序必须在每次通知之后重新注册，但如果 DN_MULTISHOT 与事件掩码进行或运算，则注册将一直保留，直到被显式移除(通过注册不接收任何事件)。)

By default, SIGIO will be delivered to the process and no other useful
information.  However, if the F_SETSIG fcntl(2) call is used to let the
kernel know which signal to deliver, a siginfo structure will be passed to
the signal handler and the si_fd member of that structure will contain the
file descriptor associated with the directory in which the event occurred.
(默认情况下，只有 SIGIO 会传递给进程，没有其他有用的信息。不过，如果使用 F_SETSIG fcntl(2) 调用让内核知道要传递哪个信号，则会向信号处理程序传递一个 siginfo 结构，该结构的 si_fd 成员将包含与事件发生时所在目录关联的文件描述符。)

Preferably the application will choose one of the real time signals
(SIGRTMIN + <n>) so that the notifications may be queued.  This is
especially important if DN_MULTISHOT is specified.  Note that SIGRTMIN
is often blocked, so it is better to use (at least) SIGRTMIN + 1.
(最好让应用程序选择一个实时信号(SIGRTMIN + <n>)，这样通知就可以排队。如果指定了 DN_MULTISHOT，这一点尤为重要。注意，SIGRTMIN 通常被阻塞，所以最好使用(至少)SIGRTMIN + 1。)

Implementation expectations (features and bugs :-))
(实现预期(特性与缺陷 :-))
---------------------------------------------------

The notification should work for any local access to files even if the
actual file system is on a remote server.  This implies that remote
access to files served by local user mode servers should be notified.
Also, remote accesses to files served by a local kernel NFS server should
be notified.
(对文件的任何本地访问都应产生通知，即使实际文件系统位于远程服务器上。这意味着对由本地用户态服务器提供的文件的远程访问应收到通知。同样，对由本地内核 NFS 服务器提供的文件的远程访问也应收到通知。)

In order to make the impact on the file system code as small as possible,
the problem of hard links to files has been ignored.  So if a file (x)
exists in two directories (a and b) then a change to the file using the
name "a/x" should be notified to a program expecting notifications on
directory "a", but will not be notified to one expecting notifications on
directory "b".
(为了将对文件系统代码的影响降到最小，文件的硬链接问题被忽略了。因此，如果同一个文件(x)存在于两个目录(a 和 b)中，那么使用名称 "a/x" 对该文件所做的更改应通知给期望在目录 "a" 上收到通知的程序，但不会通知给期望在目录 "b" 上收到通知的程序。)

Also, files that are unlinked, will still cause notifications in the
last directory that they were linked to.
(此外，被解除链接的文件仍会在它们最后所链接的目录中产生通知。)

Configuration
(配置)
-------------

Dnotify is controlled via the CONFIG_DNOTIFY configuration option.  When
disabled, fcntl(fd, F_NOTIFY, ...) will return -EINVAL.
(Dnotify 通过 CONFIG_DNOTIFY 配置选项控制。禁用时，fcntl(fd, F_NOTIFY, ...) 将返回 -EINVAL。)

Example
(示例)
-------
See tools/testing/selftests/filesystems/dnotify_test.c for an example.
(示例见 tools/testing/selftests/filesystems/dnotify_test.c。)

NOTE
(注意)
----
Beginning with Linux 2.6.13, dnotify has been replaced by inotify.
See Documentation/filesystems/inotify.rst for more information on it.
(从 Linux 2.6.13 开始，dnotify 已被 inotify 取代。有关它的更多信息参见 Documentation/filesystems/inotify.rst。)
