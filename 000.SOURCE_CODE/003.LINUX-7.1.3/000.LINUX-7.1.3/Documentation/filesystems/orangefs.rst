.. SPDX-License-Identifier: GPL-2.0

========
ORANGEFS
========

(ORANGEFS)

OrangeFS is an LGPL userspace scale-out parallel storage system. It is ideal
for large storage problems faced by HPC, BigData, Streaming Video,
Genomics, Bioinformatics.
(OrangeFS 是一个 LGPL 许可的用户空间横向扩展 (scale-out) 并行存储系统。它非常适合 HPC、大数据、流视频、基因组学、生物信息学所面临的大型存储问题。)

Orangefs, originally called PVFS, was first developed in 1993 by
Walt Ligon and Eric Blumer as a parallel file system for Parallel
Virtual Machine (PVM) as part of a NASA grant to study the I/O patterns
of parallel programs.
(Orangefs 最初称为 PVFS，由 Walt Ligon 和 Eric Blumer 于 1993 年首次开发，作为并行虚拟机 (PVM) 的并行文件系统，是 NASA 资助的并行程序 I/O 模式研究项目的一部分。)

Orangefs features include:
(Orangefs 的特性包括：)

  * Distributes file data among multiple file servers
    (在多个文件服务器之间分发文件数据)
  * Supports simultaneous access by multiple clients
    (支持多个客户端同时访问)
  * Stores file data and metadata on servers using local file system
    and access methods
    (使用本地文件系统和访问方法在服务器上存储文件数据和元数据)
  * Userspace implementation is easy to install and maintain
    (用户空间实现易于安装和维护)
  * Direct MPI support
    (直接支持 MPI)
  * Stateless
    (无状态)


Mailing List Archives
(邮件列表归档)
=====================

http://lists.orangefs.org/pipermail/devel_lists.orangefs.org/


Mailing List Submissions
(邮件列表提交)
========================

devel@lists.orangefs.org


Documentation
(文档)
=============

http://www.orangefs.org/documentation/

Running ORANGEFS On a Single Server
(在单一服务器上运行 ORANGEFS)
===================================

OrangeFS is usually run in large installations with multiple servers and
clients, but a complete filesystem can be run on a single machine for
development and testing.
(OrangeFS 通常在具有多台服务器和客户端的大型安装中运行，但为了开发和测试，完整的文件系统也可以在单台机器上运行。)

On Fedora, install orangefs and orangefs-server::
(在 Fedora 上，安装 orangefs 和 orangefs-server：)

    dnf -y install orangefs orangefs-server

There is an example server configuration file in
/etc/orangefs/orangefs.conf.  Change localhost to your hostname if
necessary.
(/etc/orangefs/orangefs.conf 中有一个示例服务器配置文件。如有必要，将 localhost 改为你的主机名。)

To generate a filesystem to run xfstests against, see below.
(要生成一个供运行 xfstests 的文件系统，参见下文。)

There is an example client configuration file in /etc/pvfs2tab.  It is a
single line.  Uncomment it and change the hostname if necessary.  This
controls clients which use libpvfs2.  This does not control the
pvfs2-client-core.
(/etc/pvfs2tab 中有一个示例客户端配置文件。它是单行内容。取消其注释并在必要时更改主机名。它控制使用 libpvfs2 的客户端。它不控制 pvfs2-client-core。)

Create the filesystem::
(创建文件系统：)

    pvfs2-server -f /etc/orangefs/orangefs.conf

Start the server::
(启动服务器：)

    systemctl start orangefs-server

Test the server::
(测试服务器：)

    pvfs2-ping -m /pvfsmnt

Start the client.  The module must be compiled in or loaded before this
point::
(启动客户端。模块必须在此之前已编译进内核或已加载：)

    systemctl start orangefs-client

Mount the filesystem::
(挂载文件系统：)

    mount -t pvfs2 tcp://localhost:3334/orangefs /pvfsmnt

Userspace Filesystem Source
(用户空间文件系统源代码)
===========================

http://www.orangefs.org/download

Orangefs versions prior to 2.9.3 would not be compatible with the
upstream version of the kernel client.
(2.9.3 之前的 Orangefs 版本与上游版本的内核客户端不兼容。)


Building ORANGEFS on a Single Server
(在单一服务器上构建 ORANGEFS)
====================================

Where OrangeFS cannot be installed from distribution packages, it may be
built from source.
(在无法从发行版软件包安装 OrangeFS 的地方，可以从源代码构建。)

You can omit --prefix if you don't care that things are sprinkled around
in /usr/local.  As of version 2.9.6, OrangeFS uses Berkeley DB by
default, we will probably be changing the default to LMDB soon.
(如果你不介意文件散落在 /usr/local 中，可以省略 --prefix。从 2.9.6 版本起，OrangeFS 默认使用 Berkeley DB，我们可能很快会把默认改为 LMDB。)

::

    ./configure --prefix=/opt/ofs --with-db-backend=lmdb --disable-usrint

    make

    make install

Create an orangefs config file by running pvfs2-genconfig and
specifying a target config file. Pvfs2-genconfig will prompt you
through. Generally it works fine to take the defaults, but you
should use your server's hostname, rather than "localhost" when
it comes to that question::
(通过运行 pvfs2-genconfig 并指定目标配置文件来创建 orangefs 配置文件。Pvfs2-genconfig 会引导你完成整个过程。通常接受默认值就可以，但当问到该问题时，应使用你服务器的主机名而不是 “localhost”：)

    /opt/ofs/bin/pvfs2-genconfig /etc/pvfs2.conf

Create an /etc/pvfs2tab file (localhost is fine)::
(创建 /etc/pvfs2tab 文件(localhost 就可以)：)

    echo tcp://localhost:3334/orangefs /pvfsmnt pvfs2 defaults,noauto 0 0 > \
	/etc/pvfs2tab

Create the mount point you specified in the tab file if needed::
(如有需要，创建你在 tab 文件中指定的挂载点：)

    mkdir /pvfsmnt

Bootstrap the server::
(引导启动 (bootstrap) 服务器：)

    /opt/ofs/sbin/pvfs2-server -f /etc/pvfs2.conf

Start the server::
(启动服务器：)

    /opt/ofs/sbin/pvfs2-server /etc/pvfs2.conf

Now the server should be running. Pvfs2-ls is a simple
test to verify that the server is running::
(现在服务器应该已在运行。Pvfs2-ls 是验证服务器是否在运行的一个简单测试：)

    /opt/ofs/bin/pvfs2-ls /pvfsmnt

If stuff seems to be working, load the kernel module and
turn on the client core::
(如果一切看起来正常，加载内核模块并开启客户端核心 (client core)：)

    /opt/ofs/sbin/pvfs2-client -p /opt/ofs/sbin/pvfs2-client-core

Mount your filesystem::
(挂载你的文件系统：)

    mount -t pvfs2 tcp://`hostname`:3334/orangefs /pvfsmnt


Running xfstests
(运行 xfstests)
================

It is useful to use a scratch filesystem with xfstests.  This can be
done with only one server.
(在 xfstests 中使用一个临时 (scratch) 文件系统会很有用。这只用一个服务器就可以做到。)

Make a second copy of the FileSystem section in the server configuration
file, which is /etc/orangefs/orangefs.conf.  Change the Name to scratch.
Change the ID to something other than the ID of the first FileSystem
section (2 is usually a good choice).
(在服务器配置文件(/etc/orangefs/orangefs.conf)中，把 FileSystem 小节再复制一份。将 Name 改为 scratch。将 ID 改为与第一个 FileSystem 小节的 ID 不同的值(通常选 2 就不错)。)

Then there are two FileSystem sections: orangefs and scratch.
(这样就有两个 FileSystem 小节：orangefs 和 scratch。)

This change should be made before creating the filesystem.
(此更改应在创建文件系统之前完成。)

::

    pvfs2-server -f /etc/orangefs/orangefs.conf

To run xfstests, create /etc/xfsqa.config::
(要运行 xfstests，创建 /etc/xfsqa.config：)

    TEST_DIR=/orangefs
    TEST_DEV=tcp://localhost:3334/orangefs
    SCRATCH_MNT=/scratch
    SCRATCH_DEV=tcp://localhost:3334/scratch

Then xfstests can be run::
(然后就可以运行 xfstests：)

    ./check -pvfs2


Options
(选项)
=======

The following mount options are accepted:
(接受以下挂载选项：)

  acl
    Allow the use of Access Control Lists on files and directories.
    (允许在文件和目录上使用访问控制列表 (ACL)。)

  intr
    Some operations between the kernel client and the user space
    filesystem can be interruptible, such as changes in debug levels
    and the setting of tunable parameters.
    (内核客户端与用户空间文件系统之间的某些操作可以是可中断的，例如调试级别的更改和可调参数的设置。)

  local_lock
    Enable posix locking from the perspective of "this" kernel. The
    default file_operations lock action is to return ENOSYS. Posix
    locking kicks in if the filesystem is mounted with -o local_lock.
    Distributed locking is being worked on for the future.
    (从 “本” 内核的角度启用 posix 锁。默认的 file_operations 锁定动作是返回 ENOSYS。如果文件系统以 -o local_lock 挂载，则启用 posix 锁定。分布式锁定正在为未来开发中。)


Debugging
(调试)
=========

If you want the debug (GOSSIP) statements in a particular
source file (inode.c for example) go to syslog::
(如果你想让某个特定源文件(例如 inode.c)中的调试 (GOSSIP) 语句输出到 syslog：)

  echo inode > /sys/kernel/debug/orangefs/kernel-debug

No debugging (the default)::
(无调试(默认)：)

  echo none > /sys/kernel/debug/orangefs/kernel-debug

Debugging from several source files::
(从多个源文件进行调试：)

  echo inode,dir > /sys/kernel/debug/orangefs/kernel-debug

All debugging::
(全部调试：)

  echo all > /sys/kernel/debug/orangefs/kernel-debug

Get a list of all debugging keywords::
(获取所有调试关键字的列表：)

  cat /sys/kernel/debug/orangefs/debug-help


Protocol between Kernel Module and Userspace
(内核模块与用户空间之间的协议)
============================================

Orangefs is a user space filesystem and an associated kernel module.
We'll just refer to the user space part of Orangefs as "userspace"
from here on out. Orangefs descends from PVFS, and userspace code
still uses PVFS for function and variable names. Userspace typedefs
many of the important structures. Function and variable names in
the kernel module have been transitioned to "orangefs", and The Linux
Coding Style avoids typedefs, so kernel module structures that
correspond to userspace structures are not typedefed.
(Orangefs 是一个用户空间文件系统和与之关联的内核模块。从现在起，我们仅将 Orangefs 的用户空间部分称为 “userspace”。Orangefs 源自 PVFS，用户空间代码仍然使用 PVFS 作为函数和变量名。用户空间对许多重要结构使用了 typedef。内核模块中的函数和变量名已过渡为 “orangefs”，而 Linux 内核编码风格避免使用 typedef，因此内核模块中与用户空间结构相对应的结构没有使用 typedef。)

The kernel module implements a pseudo device that userspace
can read from and write to. Userspace can also manipulate the
kernel module through the pseudo device with ioctl.
(内核模块实现了一个伪设备，用户空间可以从中读取和向其写入。用户空间还可以通过该伪设备使用 ioctl 来操作内核模块。)

The Bufmap
(Bufmap)
----------

At startup userspace allocates two page-size-aligned (posix_memalign)
mlocked memory buffers, one is used for IO and one is used for readdir
operations. The IO buffer is 41943040 bytes and the readdir buffer is
4194304 bytes. Each buffer contains logical chunks, or partitions, and
a pointer to each buffer is added to its own PVFS_dev_map_desc structure
which also describes its total size, as well as the size and number of
the partitions.
(启动时，用户空间分配两个按页大小对齐 (posix_memalign) 且被 mlock 的内存缓冲，一个用于 IO，一个用于 readdir 操作。IO 缓冲为 41943040 字节，readdir 缓冲为 4194304 字节。每个缓冲包含逻辑块 (logical chunks)，或称分区 (partitions)，并且指向每个缓冲的指针被添加到其自己的 PVFS_dev_map_desc 结构中，该结构还描述其总大小以及分区的大小和数量。)

A pointer to the IO buffer's PVFS_dev_map_desc structure is sent to a
mapping routine in the kernel module with an ioctl. The structure is
copied from user space to kernel space with copy_from_user and is used
to initialize the kernel module's "bufmap" (struct orangefs_bufmap), which
then contains:
(指向 IO 缓冲的 PVFS_dev_map_desc 结构的指针通过一个 ioctl 发送到内核模块中的一个映射例程。该结构用 copy_from_user 从用户空间复制到内核空间，用于初始化内核模块的 “bufmap” (struct orangefs_bufmap)，其中包含：)

  * refcnt
    - a reference counter
      (一个引用计数器)
  * desc_size - PVFS2_BUFMAP_DEFAULT_DESC_SIZE (4194304) - the IO buffer's
    partition size, which represents the filesystem's block size and
    is used for s_blocksize in super blocks.
    (desc_size - PVFS2_BUFMAP_DEFAULT_DESC_SIZE (4194304) - IO 缓冲的分区大小，它代表文件系统的块大小，并用作超级块中的 s_blocksize。)
  * desc_count - PVFS2_BUFMAP_DEFAULT_DESC_COUNT (10) - the number of
    partitions in the IO buffer.
    (desc_count - PVFS2_BUFMAP_DEFAULT_DESC_COUNT (10) - IO 缓冲中的分区数量。)
  * desc_shift - log2(desc_size), used for s_blocksize_bits in super blocks.
    (desc_shift - log2(desc_size)，用作超级块中的 s_blocksize_bits。)
  * total_size - the total size of the IO buffer.
    (total_size - IO 缓冲的总大小。)
  * page_count - the number of 4096 byte pages in the IO buffer.
    (page_count - IO 缓冲中 4096 字节页的数量。)
  * page_array - a pointer to ``page_count * (sizeof(struct page*))`` bytes
    of kcalloced memory. This memory is used as an array of pointers
    to each of the pages in the IO buffer through a call to get_user_pages.
    (page_array - 指向 ``page_count * (sizeof(struct page*))`` 字节 kcalloc 内存的指针。通过一次 get_user_pages 调用，此内存被用作指向 IO 缓冲中每一页的指针数组。)
  * desc_array - a pointer to ``desc_count * (sizeof(struct orangefs_bufmap_desc))``
    bytes of kcalloced memory. This memory is further initialized:
    (desc_array - 指向 ``desc_count * (sizeof(struct orangefs_bufmap_desc))`` 字节 kcalloc 内存的指针。此内存会被进一步初始化：)

      user_desc is the kernel's copy of the IO buffer's ORANGEFS_dev_map_desc
      structure. user_desc->ptr points to the IO buffer.
      (user_desc 是内核持有的 IO 缓冲 ORANGEFS_dev_map_desc 结构的副本。user_desc->ptr 指向 IO 缓冲。)

      ::

	pages_per_desc = bufmap->desc_size / PAGE_SIZE
	offset = 0

        bufmap->desc_array[0].page_array = &bufmap->page_array[offset]
        bufmap->desc_array[0].array_count = pages_per_desc = 1024
        bufmap->desc_array[0].uaddr = (user_desc->ptr) + (0 * 1024 * 4096)
        offset += 1024
                           .
                           .
                           .
        bufmap->desc_array[9].page_array = &bufmap->page_array[offset]
        bufmap->desc_array[9].array_count = pages_per_desc = 1024
        bufmap->desc_array[9].uaddr = (user_desc->ptr) +
                                               (9 * 1024 * 4096)
        offset += 1024

  * buffer_index_array - a desc_count sized array of ints, used to
    indicate which of the IO buffer's partitions are available to use.
    (buffer_index_array - 一个 desc_count 大小的 int 数组，用于指示 IO 缓冲的哪些分区可供使用。)
  * buffer_index_lock - a spinlock to protect buffer_index_array during update.
    (buffer_index_lock - 一个自旋锁，用于在更新期间保护 buffer_index_array。)
  * readdir_index_array - a five (ORANGEFS_READDIR_DEFAULT_DESC_COUNT) element
    int array used to indicate which of the readdir buffer's partitions are
    available to use.
    (readdir_index_array - 一个五元素 (ORANGEFS_READDIR_DEFAULT_DESC_COUNT) int 数组，用于指示 readdir 缓冲的哪些分区可供使用。)
  * readdir_index_lock - a spinlock to protect readdir_index_array during
    update.
    (readdir_index_lock - 一个自旋锁，用于在更新期间保护 readdir_index_array。)

Operations
(操作)
----------

The kernel module builds an "op" (struct orangefs_kernel_op_s) when it
needs to communicate with userspace. Part of the op contains the "upcall"
which expresses the request to userspace. Part of the op eventually
contains the "downcall" which expresses the results of the request.
(当内核模块需要与用户空间通信时，它会构建一个 “op” (struct orangefs_kernel_op_s)。op 的一部分包含 “upcall”，它表达对用户空间的请求。op 的一部分最终包含 “downcall”，它表达请求的结果。)

The slab allocator is used to keep a cache of op structures handy.
(slab 分配器用于保持一个随时可用的 op 结构缓存。)

At init time the kernel module defines and initializes a request list
and an in_progress hash table to keep track of all the ops that are
in flight at any given time.
(在初始化时，内核模块定义并初始化一个请求列表 (request list) 和一个 in_progress 哈希表，用于跟踪任何给定时刻所有在途 (in flight) 的 op。)

Ops are stateful:
(op 是有状态的：)

 * unknown
	    - op was just initialized
	  (op 刚刚被初始化)
 * waiting
	    - op is on request_list (upward bound)
	  (op 位于 request_list 上(即将上行))
 * inprogr
	    - op is in progress (waiting for downcall)
	  (op 正在进行中(等待 downcall))
 * serviced
	    - op has matching downcall; ok
	  (op 有匹配的 downcall；正常)
 * purged
	    - op has to start a timer since client-core
              exited uncleanly before servicing op
	  (op 必须启动一个计时器，因为 client-core 在处理 op 之前不干净地退出了)
 * given up
	    - submitter has given up waiting for it
	  (提交者已放弃等待它)

When some arbitrary userspace program needs to perform a
filesystem operation on Orangefs (readdir, I/O, create, whatever)
an op structure is initialized and tagged with a distinguishing ID
number. The upcall part of the op is filled out, and the op is
passed to the "service_operation" function.
(当某个任意的用户空间程序需要在 Orangefs 上执行文件系统操作(readdir、I/O、create 等等)时，一个 op 结构会被初始化并标记上一个用于区分的 ID 编号。op 的 upcall 部分被填写完毕，然后该 op 被传递给 “service_operation” 函数。)

Service_operation changes the op's state to "waiting", puts
it on the request list, and signals the Orangefs file_operations.poll
function through a wait queue. Userspace is polling the pseudo-device
and thus becomes aware of the upcall request that needs to be read.
(Service_operation 将 op 的状态改为 “waiting”，将其放到请求列表上，并通过等待队列向 Orangefs 的 file_operations.poll 函数发出信号。用户空间正在轮询该伪设备，从而感知到需要读取的 upcall 请求。)

When the Orangefs file_operations.read function is triggered, the
request list is searched for an op that seems ready-to-process.
The op is removed from the request list. The tag from the op and
the filled-out upcall struct are copy_to_user'ed back to userspace.
(当 Orangefs 的 file_operations.read 函数被触发时，会在请求列表中搜索一个看起来已准备好处理的 op。该 op 从请求列表中移除。op 的 tag 和填写好的 upcall 结构通过 copy_to_user 被复制回用户空间。)

If any of these (and some additional protocol) copy_to_users fail,
the op's state is set to "waiting" and the op is added back to
the request list. Otherwise, the op's state is changed to "in progress",
and the op is hashed on its tag and put onto the end of a list in the
in_progress hash table at the index the tag hashed to.
(如果这些 copy_to_user(以及一些额外的协议)中的任何一个失败，op 的状态会被设为 “waiting”，并且该 op 被重新加回请求列表。否则，op 的状态被改为 “in progress”，op 按其 tag 被哈希，并放到 in_progress 哈希表中该 tag 所哈希到的索引处的列表末尾。)

When userspace has assembled the response to the upcall, it
writes the response, which includes the distinguishing tag, back to
the pseudo device in a series of io_vecs. This triggers the Orangefs
file_operations.write_iter function to find the op with the associated
tag and remove it from the in_progress hash table. As long as the op's
state is not "canceled" or "given up", its state is set to "serviced".
The file_operations.write_iter function returns to the waiting vfs,
and back to service_operation through wait_for_matching_downcall.
(当用户空间组装好对 upcall 的响应后，它将响应(包含用于区分的 tag)以一系列 io_vec 的形式写回伪设备。这会触发 Orangefs 的 file_operations.write_iter 函数找到具有相关 tag 的 op 并将其从 in_progress 哈希表中移除。只要 op 的状态不是 “canceled” 或 “given up”，其状态就会被设为 “serviced”。file_operations.write_iter 函数返回到正在等待的 vfs，并经由 wait_for_matching_downcall 返回到 service_operation。)

Service operation returns to its caller with the op's downcall
part (the response to the upcall) filled out.
(Service operation 返回其调用者时，op 的 downcall 部分(对 upcall 的响应)已填写完毕。)

The "client-core" is the bridge between the kernel module and
userspace. The client-core is a daemon. The client-core has an
associated watchdog daemon. If the client-core is ever signaled
to die, the watchdog daemon restarts the client-core. Even though
the client-core is restarted "right away", there is a period of
time during such an event that the client-core is dead. A dead client-core
can't be triggered by the Orangefs file_operations.poll function.
Ops that pass through service_operation during a "dead spell" can timeout
on the wait queue and one attempt is made to recycle them. Obviously,
if the client-core stays dead too long, the arbitrary userspace processes
trying to use Orangefs will be negatively affected. Waiting ops
that can't be serviced will be removed from the request list and
have their states set to "given up". In-progress ops that can't
be serviced will be removed from the in_progress hash table and
have their states set to "given up".
(“client-core” 是内核模块与用户空间之间的桥梁。client-core 是一个 daemon。client-core 有一个关联的看门狗 (watchdog) daemon。如果 client-core 收到终止信号，看门狗 daemon 会重启 client-core。尽管 client-core 会 “立即” 重启，但在此类事件期间有一段时间 client-core 是死掉的。死掉的 client-core 无法被 Orangefs 的 file_operations.poll 函数触发。在 “死亡期” 间经过 service_operation 的 op 可能会在等待队列上超时，并且会尝试一次回收。显然，如果 client-core 死得太久，试图使用 Orangefs 的任意用户空间进程将受到负面影响。无法得到服务的等待中的 op 将从请求列表中移除，其状态被设为 “given up”。无法得到服务的进行中的 op 将从 in_progress 哈希表中移除，其状态被设为 “given up”。)

Readdir and I/O ops are atypical with respect to their payloads.
(readdir 和 I/O op 在其载荷 (payload) 方面是非典型的。)

  - readdir ops use the smaller of the two pre-allocated pre-partitioned
    memory buffers. The readdir buffer is only available to userspace.
    The kernel module obtains an index to a free partition before launching
    a readdir op. Userspace deposits the results into the indexed partition
    and then writes them to back to the pvfs device.
    (readdir op 使用两个预分配、预分区内存缓冲中较小的一个。readdir 缓冲仅对用户空间可用。内核模块在启动 readdir op 之前获取一个空闲分区的索引。用户空间将结果放入被索引的分区，然后将它们写回 pvfs 设备。)

  - io (read and write) ops use the larger of the two pre-allocated
    pre-partitioned memory buffers. The IO buffer is accessible from
    both userspace and the kernel module. The kernel module obtains an
    index to a free partition before launching an io op. The kernel module
    deposits write data into the indexed partition, to be consumed
    directly by userspace. Userspace deposits the results of read
    requests into the indexed partition, to be consumed directly
    by the kernel module.
    (io(读和写)op 使用两个预分配、预分区内存缓冲中较大的一个。IO 缓冲可由用户空间和内核模块双方访问。内核模块在启动 io op 之前获取一个空闲分区的索引。内核模块将写数据放入被索引的分区，供用户空间直接取用。用户空间将读请求的结果放入被索引的分区，供内核模块直接取用。)

Responses to kernel requests are all packaged in pvfs2_downcall_t
structs. Besides a few other members, pvfs2_downcall_t contains a
union of structs, each of which is associated with a particular
response type.
(对内核请求的响应都封装在 pvfs2_downcall_t 结构中。除了少数其他成员外，pvfs2_downcall_t 包含一个结构体的 union，其中每个结构体都与一种特定的响应类型相关联。)

The several members outside of the union are:
(union 之外的几个成员是：)

 ``int32_t type``
    - type of operation.
      (操作的类型。)
 ``int32_t status``
    - return code for the operation.
      (操作的返回码。)
 ``int64_t trailer_size``
    - 0 unless readdir operation.
      (除 readdir 操作外为 0。)
 ``char *trailer_buf``
    - initialized to NULL, used during readdir operations.
      (初始化为 NULL，在 readdir 操作期间使用。)

The appropriate member inside the union is filled out for any
particular response.
(对于任何特定响应，union 中相应的成员会被填写。)

  PVFS2_VFS_OP_FILE_IO
    fill a pvfs2_io_response_t
    (填写一个 pvfs2_io_response_t)

  PVFS2_VFS_OP_LOOKUP
    fill a PVFS_object_kref
    (填写一个 PVFS_object_kref)

  PVFS2_VFS_OP_CREATE
    fill a PVFS_object_kref
    (填写一个 PVFS_object_kref)

  PVFS2_VFS_OP_SYMLINK
    fill a PVFS_object_kref
    (填写一个 PVFS_object_kref)

  PVFS2_VFS_OP_GETATTR
    fill in a PVFS_sys_attr_s (tons of stuff the kernel doesn't need)
    fill in a string with the link target when the object is a symlink.
    (填写一个 PVFS_sys_attr_s (一大堆内核不需要的东西)。当对象是符号链接时，填写一个包含链接目标的字符串。)

  PVFS2_VFS_OP_MKDIR
    fill a PVFS_object_kref
    (填写一个 PVFS_object_kref)

  PVFS2_VFS_OP_STATFS
    fill a pvfs2_statfs_response_t with useless info <g>. It is hard for
    us to know, in a timely fashion, these statistics about our
    distributed network filesystem.
    (用没用的信息填写一个 pvfs2_statfs_response_t <g>。我们很难及时获知关于我们这个分布式网络文件系统的这些统计信息。)

  PVFS2_VFS_OP_FS_MOUNT
    fill a pvfs2_fs_mount_response_t which is just like a PVFS_object_kref
    except its members are in a different order and "__pad1" is replaced
    with "id".
    (填写一个 pvfs2_fs_mount_response_t，它就像一个 PVFS_object_kref，只是其成员顺序不同，并且 "__pad1" 被替换为 "id"。)

  PVFS2_VFS_OP_GETXATTR
    fill a pvfs2_getxattr_response_t
    (填写一个 pvfs2_getxattr_response_t)

  PVFS2_VFS_OP_LISTXATTR
    fill a pvfs2_listxattr_response_t
    (填写一个 pvfs2_listxattr_response_t)

  PVFS2_VFS_OP_PARAM
    fill a pvfs2_param_response_t
    (填写一个 pvfs2_param_response_t)

  PVFS2_VFS_OP_PERF_COUNT
    fill a pvfs2_perf_count_response_t
    (填写一个 pvfs2_perf_count_response_t)

  PVFS2_VFS_OP_FSKEY
    file a pvfs2_fs_key_response_t
    (填写一个 pvfs2_fs_key_response_t)

  PVFS2_VFS_OP_READDIR
    jamb everything needed to represent a pvfs2_readdir_response_t into
    the readdir buffer descriptor specified in the upcall.
    (将表示一个 pvfs2_readdir_response_t 所需的一切都塞进 upcall 中指定的 readdir 缓冲描述符。)

Userspace uses writev() on /dev/pvfs2-req to pass responses to the requests
made by the kernel side.
(用户空间通过对 /dev/pvfs2-req 使用 writev() 将响应传递给内核侧发出的请求。)

A buffer_list containing:
(一个 buffer_list，包含：)

  - a pointer to the prepared response to the request from the
    kernel (struct pvfs2_downcall_t).
    (一个指针，指向为来自内核的请求准备好的响应 (struct pvfs2_downcall_t)。)
  - and also, in the case of a readdir request, a pointer to a
    buffer containing descriptors for the objects in the target
    directory.
    (以及在 readdir 请求的情况下，一个指针，指向包含目标目录中对象描述符的缓冲。)

... is sent to the function (PINT_dev_write_list) which performs
the writev.
(...会被发送到执行 writev 的函数 (PINT_dev_write_list)。)

PINT_dev_write_list has a local iovec array: struct iovec io_array[10];
(PINT_dev_write_list 有一个局部 iovec 数组：struct iovec io_array[10]；)

The first four elements of io_array are initialized like this for all
responses::
(对于所有响应，io_array 的前四个元素都像下面这样初始化：)

  io_array[0].iov_base = address of local variable "proto_ver" (int32_t)
  io_array[0].iov_len = sizeof(int32_t)

  io_array[1].iov_base = address of global variable "pdev_magic" (int32_t)
  io_array[1].iov_len = sizeof(int32_t)

  io_array[2].iov_base = address of parameter "tag" (PVFS_id_gen_t)
  io_array[2].iov_len = sizeof(int64_t)

  io_array[3].iov_base = address of out_downcall member (pvfs2_downcall_t)
                         of global variable vfs_request (vfs_request_t)
  io_array[3].iov_len = sizeof(pvfs2_downcall_t)

Readdir responses initialize the fifth element io_array like this::
(readdir 响应像下面这样初始化 io_array 的第五个元素：)

  io_array[4].iov_base = contents of member trailer_buf (char *)
                         from out_downcall member of global variable
                         vfs_request
  io_array[4].iov_len = contents of member trailer_size (PVFS_size)
                        from out_downcall member of global variable
                        vfs_request

Orangefs exploits the dcache in order to avoid sending redundant
requests to userspace. We keep object inode attributes up-to-date with
orangefs_inode_getattr. Orangefs_inode_getattr uses two arguments to
help it decide whether or not to update an inode: "new" and "bypass".
Orangefs keeps private data in an object's inode that includes a short
timeout value, getattr_time, which allows any iteration of
orangefs_inode_getattr to know how long it has been since the inode was
updated. When the object is not new (new == 0) and the bypass flag is not
set (bypass == 0) orangefs_inode_getattr returns without updating the inode
if getattr_time has not timed out. Getattr_time is updated each time the
inode is updated.
(Orangefs 利用 dcache 来避免向用户空间发送冗余请求。我们通过 orangefs_inode_getattr 使对象 inode 属性保持最新。Orangefs_inode_getattr 使用两个参数来帮助它决定是否更新一个 inode："new" 和 "bypass"。Orangefs 在对象的 inode 中保存私有数据，其中包括一个短超时值 getattr_time，它使 orangefs_inode_getattr 的任何一次执行都能知道自该 inode 上次更新以来已过去多长时间。当对象不是新的 (new == 0) 且 bypass 标志未设置 (bypass == 0) 时，如果 getattr_time 尚未超时，orangefs_inode_getattr 将不更新 inode 而直接返回。每次更新 inode 时 getattr_time 都会被更新。)

Creation of a new object (file, dir, sym-link) includes the evaluation of
its pathname, resulting in a negative directory entry for the object.
A new inode is allocated and associated with the dentry, turning it from
a negative dentry into a "productive full member of society". Orangefs
obtains the new inode from Linux with new_inode() and associates
the inode with the dentry by sending the pair back to Linux with
d_instantiate().
(创建一个新对象(文件、目录、符号链接)包括对其路径名 (pathname) 的求值，这会为该对象产生一个负项目录项 (negative directory entry)。一个新 inode 被分配并与该 dentry 关联，使它从一个负 dentry 变成 “对社会有产出贡献的正式成员”。Orangefs 通过 new_inode() 从 Linux 获取新 inode，并通过用 d_instantiate() 将这一对发回 Linux 来把 inode 与 dentry 关联起来。)

The evaluation of a pathname for an object resolves to its corresponding
dentry. If there is no corresponding dentry, one is created for it in
the dcache. Whenever a dentry is modified or verified Orangefs stores a
short timeout value in the dentry's d_time, and the dentry will be trusted
for that amount of time. Orangefs is a network filesystem, and objects
can potentially change out-of-band with any particular Orangefs kernel module
instance, so trusting a dentry is risky. The alternative to trusting
dentries is to always obtain the needed information from userspace - at
least a trip to the client-core, maybe to the servers. Obtaining information
from a dentry is cheap, obtaining it from userspace is relatively expensive,
hence the motivation to use the dentry when possible.
(对一个对象的路径名的求值会解析到其对应的 dentry。如果没有对应的 dentry，则会在 dcache 中为它创建一个。每当一个 dentry 被修改或验证时，Orangefs 都会在该 dentry 的 d_time 中存储一个短超时值，并且该 dentry 在这段时间内会被信任。Orangefs 是一个网络文件系统，对象可能会独立于任何特定的 Orangefs 内核模块实例而发生带外 (out-of-band) 变化，所以信任一个 dentry 是有风险的。不信任 dentry 的替代做法是始终从用户空间获取所需信息——至少要跑一趟 client-core，也许还要到服务器。从 dentry 获取信息很便宜，而从用户空间获取则相对昂贵，这就是尽可能使用 dentry 的动机所在。)

The timeout values d_time and getattr_time are jiffy based, and the
code is designed to avoid the jiffy-wrap problem::
(超时值 d_time 和 getattr_time 基于 jiffy，并且代码的设计避免了 jiffy 回绕 (jiffy-wrap) 问题：)

    "In general, if the clock may have wrapped around more than once, there
    is no way to tell how much time has elapsed. However, if the times t1
    and t2 are known to be fairly close, we can reliably compute the
    difference in a way that takes into account the possibility that the
    clock may have wrapped between times."

from course notes by instructor Andy Wang
(摘自讲师 Andy Wang 的课程笔记。)

