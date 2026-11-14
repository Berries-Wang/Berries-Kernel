.. SPDX-License-Identifier: GPL-2.0

===========================
Coda Kernel-Venus Interface
===========================

(Coda 内核-Venus 接口)

.. Note::

   This is one of the technical documents describing a component of
   Coda -- this document describes the client kernel-Venus interface.
   (这是描述 Coda 组件的技术文档之一——本文档描述客户端的内核-Venus 接口。)

For more information:
(更多信息：)

  http://www.coda.cs.cmu.edu

For user level software needed to run Coda:
(运行 Coda 所需的用户级软件：)

  ftp://ftp.coda.cs.cmu.edu

To run Coda you need to get a user level cache manager for the client,
named Venus, as well as tools to manipulate ACLs, to log in, etc.  The
client needs to have the Coda filesystem selected in the kernel
configuration.
(要运行 Coda，你需要为客户端获取一个用户级缓存管理器，即 Venus，以及用于操作 ACL、登录等的工具。客户端需要在内核配置中选中 Coda 文件系统。)

The server needs a user level server and at present does not depend on
kernel support.
(服务器需要一个用户级的服务器程序，目前不依赖内核支持。)

  The Venus kernel interface
  (Venus 内核接口)

  Peter J. Braam
  (Peter J. Braam)

  v1.0, Nov 9, 1997
  (v1.0，1997年11月9日)

  This document describes the communication between Venus and kernel
  level filesystem code needed for the operation of the Coda file sys-
  tem.  This document version is meant to describe the current interface
  (version 1.0) as well as improvements we envisage.
  (本文档描述 Venus 与内核级文件系统代码之间为支持 Coda 文件系统运行所需的通信。本文档的这一版本旨在描述当前接口(1.0 版)以及我们设想的改进。)

.. Table of Contents

  1. Introduction
  (1. 引言)

  2. Servicing Coda filesystem calls
  (2. 处理 Coda 文件系统调用)

  3. The message layer
  (3. 消息层)

     3.1 Implementation details
     (3.1 实现细节)

  4. The interface at the call level
  (4. 调用层面的接口)

     4.1 Data structures shared by the kernel and Venus
     (4.1 内核与 Venus 共享的数据结构)
     4.2 The pioctl interface
     (4.2 pioctl 接口)
     4.3 root
     (4.3 root(根目录))
     4.4 lookup
     (4.4 lookup(查找))
     4.5 getattr
     (4.5 getattr(获取属性))
     4.6 setattr
     (4.6 setattr(设置属性))
     4.7 access
     (4.7 access(访问权限))
     4.8 create
     (4.8 create(创建文件))
     4.9 mkdir
     (4.9 mkdir(创建目录))
     4.10 link
     (4.10 link(创建硬链接))
     4.11 symlink
     (4.11 symlink(创建符号链接))
     4.12 remove
     (4.12 remove(删除文件))
     4.13 rmdir
     (4.13 rmdir(删除目录))
     4.14 readlink
     (4.14 readlink(读取符号链接))
     4.15 open
     (4.15 open(打开))
     4.16 close
     (4.16 close(关闭))
     4.17 ioctl
     (4.17 ioctl(设备控制))
     4.18 rename
     (4.18 rename(重命名))
     4.19 readdir
     (4.19 readdir(读取目录))
     4.20 vget
     (4.20 vget(获取 vnode))
     4.21 fsync
     (4.21 fsync(同步文件))
     4.22 inactive
     (4.22 inactive(释放))
     4.23 rdwr
     (4.23 rdwr(读写))
     4.24 odymount
     (4.24 odymount(动态挂载))
     4.25 ody_lookup
     (4.25 ody_lookup(动态集查找))
     4.26 ody_expand
     (4.26 ody_expand(动态集展开))
     4.27 prefetch
     (4.27 prefetch(预取))
     4.28 signal
     (4.28 signal(信号))

  5. The minicache and downcalls
  (5. minicache 与下行调用)

     5.1 INVALIDATE
     (5.1 INVALIDATE(使无效))
     5.2 FLUSH
     (5.2 FLUSH(刷新))
     5.3 PURGEUSER
     (5.3 PURGEUSER(清除用户))
     5.4 ZAPFILE
     (5.4 ZAPFILE(清除文件缓存项))
     5.5 ZAPDIR
     (5.5 ZAPDIR(清除目录缓存项))
     5.6 ZAPVNODE
     (5.6 ZAPVNODE(清除 vnode 缓存项))
     5.7 PURGEFID
     (5.7 PURGEFID(清除 fid))
     5.8 REPLACE
     (5.8 REPLACE(替换))

  6. Initialization and cleanup
  (6. 初始化与清理)

     6.1 Requirements
     (6.1 需求)

1. Introduction
(1. 引言)
===============

  A key component in the Coda Distributed File System is the cache
  manager, Venus.
  (Coda 分布式文件系统中的一个关键组件是缓存管理器 Venus。)

  When processes on a Coda enabled system access files in the Coda
  filesystem, requests are directed at the filesystem layer in the
  operating system. The operating system will communicate with Venus to
  service the request for the process.  Venus manages a persistent
  client cache and makes remote procedure calls to Coda file servers and
  related servers (such as authentication servers) to service these
  requests it receives from the operating system.  When Venus has
  serviced a request it replies to the operating system with appropriate
  return codes, and other data related to the request.  Optionally the
  kernel support for Coda may maintain a minicache of recently processed
  requests to limit the number of interactions with Venus.  Venus
  possesses the facility to inform the kernel when elements from its
  minicache are no longer valid.
  (当启用了 Coda 的系统上的进程访问 Coda 文件系统中的文件时，请求会被导向操作系统中的文件系统层。操作系统将与 Venus 通信，为该进程服务此请求。Venus 管理一个持久的客户端缓存，并向 Coda 文件服务器及相关服务器(如认证服务器)发起远程过程调用，以处理它从操作系统收到的这些请求。当 Venus 处理完一个请求后，它会以适当的返回码以及与该请求相关的其他数据答复操作系统。可选地，Coda 的内核支持可以维护一个由最近处理过的请求组成的 minicache，以限制与 Venus 交互的次数。Venus 具备在其 minicache 中的元素不再有效时通知内核的能力。)

  This document describes precisely this communication between the
  kernel and Venus.  The definitions of so called upcalls and downcalls
  will be given with the format of the data they handle. We shall also
  describe the semantic invariants resulting from the calls.
  (本文档精确描述内核与 Venus 之间的这种通信。我们将给出所谓上行调用(upcall)与下行调用(downcall)的定义以及它们所处理数据的格式。我们还将描述由这些调用产生的语义不变量。)

  Historically Coda was implemented in a BSD file system in Mach 2.6.
  The interface between the kernel and Venus is very similar to the BSD
  VFS interface.  Similar functionality is provided, and the format of
  the parameters and returned data is very similar to the BSD VFS.  This
  leads to an almost natural environment for implementing a kernel-level
  filesystem driver for Coda in a BSD system.  However, other operating
  systems such as Linux and Windows 95 and NT have virtual filesystem
  with different interfaces.
  (历史上，Coda 最初是在 Mach 2.6 的一个 BSD 文件系统中实现的。内核与 Venus 之间的接口与 BSD VFS 接口非常相似。两者提供了类似的功能，参数和返回数据的格式也与 BSD VFS 十分接近。这使得在 BSD 系统中为 Coda 实现内核级文件系统驱动几乎是一件顺理成章的事。然而，其他操作系统(如 Linux、Windows 95 和 NT)拥有接口不同的虚拟文件系统。)

  To implement Coda on these systems some reverse engineering of the
  Venus/Kernel protocol is necessary.  Also it came to light that other
  systems could profit significantly from certain small optimizations
  and modifications to the protocol. To facilitate this work as well as
  to make future ports easier, communication between Venus and the
  kernel should be documented in great detail.  This is the aim of this
  document.
  (要在这些系统上实现 Coda，需要对 Venus/内核协议做一些逆向工程。此外人们还发现，对协议进行某些小的优化和修改可以使其他系统显著获益。为了便利这项工作并使未来的移植更容易，Venus 与内核之间的通信应当被非常详尽地记录下来。这正是本文档的目标。)

2.  Servicing Coda filesystem calls
(2. 处理 Coda 文件系统调用)
===================================

  The service of a request for a Coda file system service originates in
  a process P which accessing a Coda file. It makes a system call which
  traps to the OS kernel. Examples of such calls trapping to the kernel
  are ``read``, ``write``, ``open``, ``close``, ``create``, ``mkdir``,
  ``rmdir``, ``chmod`` in a Unix context.  Similar calls exist in the Win32
  environment, and are named ``CreateFile``.
  (对 Coda 文件系统服务的一次请求起源于一个正在访问 Coda 文件的进程 P。它发起一个陷入操作系统内核的系统调用。在 Unix 环境中，这类陷入内核的调用示例有 ``read``、``write``、``open``、``close``、``create``、``mkdir``、``rmdir``、``chmod``。Win32 环境中存在类似的调用，名为 ``CreateFile``。)

  Generally the operating system handles the request in a virtual
  filesystem (VFS) layer, which is named I/O Manager in NT and IFS
  manager in Windows 95.  The VFS is responsible for partial processing
  of the request and for locating the specific filesystem(s) which will
  service parts of the request.  Usually the information in the path
  assists in locating the correct FS drivers.  Sometimes after extensive
  pre-processing, the VFS starts invoking exported routines in the FS
  driver.  This is the point where the FS specific processing of the
  request starts, and here the Coda specific kernel code comes into
  play.
  (通常，操作系统在虚拟文件系统(VFS)层处理该请求，这一层在 NT 中称为 I/O Manager，在 Windows 95 中称为 IFS manager。VFS 负责对请求进行部分处理，并定位将服务该请求各部分的具体文件系统。通常路径中的信息有助于定位正确的文件系统驱动。有时在经过大量预处理之后，VFS 开始调用文件系统驱动导出的例程。这里正是针对该请求的文件系统特定处理开始之处，也是 Coda 特定的内核代码发挥作用之处。)

  The FS layer for Coda must expose and implement several interfaces.
  First and foremost the VFS must be able to make all necessary calls to
  the Coda FS layer, so the Coda FS driver must expose the VFS interface
  as applicable in the operating system. These differ very significantly
  among operating systems, but share features such as facilities to
  read/write and create and remove objects.  The Coda FS layer services
  such VFS requests by invoking one or more well defined services
  offered by the cache manager Venus.  When the replies from Venus have
  come back to the FS driver, servicing of the VFS call continues and
  finishes with a reply to the kernel's VFS. Finally the VFS layer
  returns to the process.
  (Coda 的文件系统层必须暴露并实现若干接口。首先也是最重要的，VFS 必须能够对 Coda 文件系统层进行所有必要的调用，因此 Coda 文件系统驱动必须按照所在操作系统的适用情况暴露 VFS 接口。这些接口在不同操作系统之间差异极大，但都共享诸如读写以及创建和删除对象之类的功能。Coda 文件系统层通过调用缓存管理器 Venus 提供的一个或多个定义良好的服务来处理这类 VFS 请求。当来自 Venus 的应答回到文件系统驱动后，对 VFS 调用的服务继续进行，并以对内核 VFS 的应答结束。最后 VFS 层返回到进程。)

  As a result of this design a basic interface exposed by the FS driver
  must allow Venus to manage message traffic.  In particular Venus must
  be able to retrieve and place messages and to be notified of the
  arrival of a new message. The notification must be through a mechanism
  which does not block Venus since Venus must attend to other tasks even
  when no messages are waiting or being processed.
  (作为这一设计的结果，文件系统驱动暴露的一个基本接口必须允许 Venus 管理消息流量。特别地，Venus 必须能够取回和存放消息，并能被通知有新消息到达。该通知必须通过一种不阻塞 Venus 的机制进行，因为即使没有消息在等待或正在处理，Venus 也必须顾及其他任务。)

  **Interfaces of the Coda FS Driver**
  (**Coda 文件系统驱动的接口**)

  Furthermore the FS layer provides for a special path of communication
  between a user process and Venus, called the pioctl interface. The
  pioctl interface is used for Coda specific services, such as
  requesting detailed information about the persistent cache managed by
  Venus. Here the involvement of the kernel is minimal.  It identifies
  the calling process and passes the information on to Venus.  When
  Venus replies the response is passed back to the caller in unmodified
  form.
  (此外，文件系统层还提供了用户进程与 Venus 之间的一条特殊通信途径，称为 pioctl 接口。pioctl 接口用于 Coda 特定的服务，例如请求关于 Venus 管理的持久缓存的详细信息。这里内核的参与是最小化的：它识别调用进程并将信息传递给 Venus。当 Venus 应答时，响应以未修改的形式传回给调用者。)

  Finally Venus allows the kernel FS driver to cache the results from
  certain services.  This is done to avoid excessive context switches
  and results in an efficient system.  However, Venus may acquire
  information, for example from the network which implies that cached
  information must be flushed or replaced. Venus then makes a downcall
  to the Coda FS layer to request flushes or updates in the cache.  The
  kernel FS driver handles such requests synchronously.
  (最后，Venus 允许内核文件系统驱动缓存某些服务的结果。这样做是为了避免过多的上下文切换，从而造就一个高效的系统。然而，Venus 可能获得某些信息(例如来自网络)，这意味着被缓存的信息必须被刷新或替换。于是 Venus 向 Coda 文件系统层发起一次下行调用(downcall)，请求刷新或更新缓存。内核文件系统驱动同步地处理这类请求。)

  Among these interfaces the VFS interface and the facility to place,
  receive and be notified of messages are platform specific.  We will
  not go into the calls exported to the VFS layer but we will state the
  requirements of the message exchange mechanism.
  (在这些接口中，VFS 接口以及存放、接收消息并获知消息到达的机制是平台相关的。我们不会深入讨论导出给 VFS 层的调用，但会说明消息交换机制的需求。)


3.  The message layer
(3. 消息层)
=====================

  At the lowest level the communication between Venus and the FS driver
  proceeds through messages.  The synchronization between processes
  requesting Coda file service and Venus relies on blocking and waking
  up processes.  The Coda FS driver processes VFS- and pioctl-requests
  on behalf of a process P, creates messages for Venus, awaits replies
  and finally returns to the caller.  The implementation of the exchange
  of messages is platform specific, but the semantics have (so far)
  appeared to be generally applicable.  Data buffers are created by the
  FS Driver in kernel memory on behalf of P and copied to user memory in
  Venus.
  (在最底层，Venus 与文件系统驱动之间的通信通过消息进行。请求 Coda 文件服务的进程与 Venus 之间的同步依赖于阻塞和唤醒进程。Coda 文件系统驱动代表进程 P 处理 VFS 和 pioctl 请求，为 Venus 创建消息，等待应答，最后返回给调用者。消息交换的实现是平台相关的，但其语义(迄今为止)看起来是普遍适用的。数据缓冲区由文件系统驱动代表 P 在内核内存中创建，并复制到 Venus 的用户内存中。)

  The FS Driver while servicing P makes upcalls to Venus.  Such an
  upcall is dispatched to Venus by creating a message structure.  The
  structure contains the identification of P, the message sequence
  number, the size of the request and a pointer to the data in kernel
  memory for the request.  Since the data buffer is re-used to hold the
  reply from Venus, there is a field for the size of the reply.  A flags
  field is used in the message to precisely record the status of the
  message.  Additional platform dependent structures involve pointers to
  determine the position of the message on queues and pointers to
  synchronization objects.  In the upcall routine the message structure
  is filled in, flags are set to 0, and it is placed on the *pending*
  queue.  The routine calling upcall is responsible for allocating the
  data buffer; its structure will be described in the next section.
  (文件系统驱动在为 P 服务时向 Venus 发起上行调用(upcall)。这样的上行调用通过创建一个消息结构分派给 Venus。该结构包含 P 的标识、消息序号、请求的大小以及指向该请求数据所在内核内存的指针。由于数据缓冲区会被复用来存放来自 Venus 的应答，因此有一个字段用于记录应答的大小。消息中使用一个 flags 字段来精确记录消息的状态。其他平台相关的结构包含用于确定消息在队列中位置的指针以及指向同步对象的指针。在上行调用例程中，消息结构被填充，flags 被置为 0，然后它被放到 *pending* 队列上。调用上行调用的例程负责分配数据缓冲区；其结构将在下一节描述。)

  A facility must exist to notify Venus that the message has been
  created, and implemented using available synchronization objects in
  the OS. This notification is done in the upcall context of the process
  P. When the message is on the pending queue, process P cannot proceed
  in upcall.  The (kernel mode) processing of P in the filesystem
  request routine must be suspended until Venus has replied.  Therefore
  the calling thread in P is blocked in upcall.  A pointer in the
  message structure will locate the synchronization object on which P is
  sleeping.
  (必须存在一种机制来通知 Venus 消息已创建，并使用操作系统中可用的同步对象来实现。这一通知在进程 P 的上行调用上下文中完成。当消息位于 pending 队列上时，进程 P 无法在上行调用中继续前进。P 在文件系统请求例程中的(内核态)处理必须被挂起，直到 Venus 应答。因此 P 中的调用线程在上行调用中被阻塞。消息结构中的一个指针将定位 P 正在其上睡眠的同步对象。)

  Venus detects the notification that a message has arrived, and the FS
  driver allow Venus to retrieve the message with a getmsg_from_kernel
  call. This action finishes in the kernel by putting the message on the
  queue of processing messages and setting flags to READ.  Venus is
  passed the contents of the data buffer. The getmsg_from_kernel call
  now returns and Venus processes the request.
  (Venus 检测到消息到达的通知，文件系统驱动允许 Venus 通过 getmsg_from_kernel 调用取回该消息。这一操作在内核中完成的方式是：将消息放到正在处理的消息队列上，并将 flags 设置为 READ。数据缓冲区的内容被传递给 Venus。getmsg_from_kernel 调用随即返回，Venus 开始处理该请求。)

  At some later point the FS driver receives a message from Venus,
  namely when Venus calls sendmsg_to_kernel.  At this moment the Coda FS
  driver looks at the contents of the message and decides if:
  (在稍后的某个时刻，文件系统驱动会收到来自 Venus 的消息，即当 Venus 调用 sendmsg_to_kernel 时。此时 Coda 文件系统驱动查看消息的内容并判断属于下列哪种情况：)


  *  the message is a reply for a suspended thread P.  If so it removes
     the message from the processing queue and marks the message as
     WRITTEN.  Finally, the FS driver unblocks P (still in the kernel
     mode context of Venus) and the sendmsg_to_kernel call returns to
     Venus.  The process P will be scheduled at some point and continues
     processing its upcall with the data buffer replaced with the reply
     from Venus.
     (该消息是对某个被挂起的线程 P 的应答。如果是，文件系统驱动会将该消息从处理队列中移除，并将消息标记为 WRITTEN。最后，文件系统驱动解除 P 的阻塞(此时仍处于 Venus 的内核态上下文中)，sendmsg_to_kernel 调用返回到 Venus。进程 P 将在某个时刻被调度，并在数据缓冲区被替换为来自 Venus 的应答后继续处理其上行调用。)

  *  The message is a ``downcall``.  A downcall is a request from Venus to
     the FS Driver. The FS driver processes the request immediately
     (usually a cache eviction or replacement) and when it finishes
     sendmsg_to_kernel returns.
     (该消息是一个 ``downcall``。下行调用是 Venus 向文件系统驱动发出的请求。文件系统驱动立即处理该请求(通常是缓存的逐出或替换)，完成后 sendmsg_to_kernel 返回。)

  Now P awakes and continues processing upcall.  There are some
  subtleties to take account of. First P will determine if it was woken
  up in upcall by a signal from some other source (for example an
  attempt to terminate P) or as is normally the case by Venus in its
  sendmsg_to_kernel call.  In the normal case, the upcall routine will
  deallocate the message structure and return.  The FS routine can proceed
  with its processing.
  (现在 P 被唤醒并继续处理上行调用。有一些细节需要考虑。首先 P 会判断自己在上行调用中是被其他来源的信号唤醒的(例如有人试图终止 P)，还是像通常情况那样被 Venus 在其 sendmsg_to_kernel 调用中唤醒。在正常情况下，上行调用例程将释放消息结构并返回。文件系统例程随后可以继续其处理。)


  **Sleeping and IPC arrangements**
  (**睡眠与 IPC 安排**)

  In case P is woken up by a signal and not by Venus, it will first look
  at the flags field.  If the message is not yet READ, the process P can
  handle its signal without notifying Venus.  If Venus has READ, and
  the request should not be processed, P can send Venus a signal message
  to indicate that it should disregard the previous message.  Such
  signals are put in the queue at the head, and read first by Venus.  If
  the message is already marked as WRITTEN it is too late to stop the
  processing.  The VFS routine will now continue.  (-- If a VFS request
  involves more than one upcall, this can lead to complicated state, an
  extra field "handle_signals" could be added in the message structure
  to indicate points of no return have been passed.--)
  (如果 P 是被信号而不是被 Venus 唤醒的，它会首先查看 flags 字段。如果消息尚未被 READ，进程 P 可以在不通知 Venus 的情况下处理其信号。如果 Venus 已经 READ，而该请求不应被处理，P 可以向 Venus 发送一条信号消息，表明它应当忽略之前的那条消息。这样的信号消息被放在队列头部，并被 Venus 最先读取。如果消息已被标记为 WRITTEN，那么要停止处理已经为时已晚。VFS 例程此时将继续执行。(-- 如果一个 VFS 请求涉及不止一次上行调用，这可能导致复杂的状态；可以在消息结构中增加一个额外的 "handle_signals" 字段，用以指示已经越过了不可返回点。--))



3.1.  Implementation details
(3.1. 实现细节)
----------------------------

  The Unix implementation of this mechanism has been through the
  implementation of a character device associated with Coda.  Venus
  retrieves messages by doing a read on the device, replies are sent
  with a write and notification is through the select system call on the
  file descriptor for the device.  The process P is kept waiting on an
  interruptible wait queue object.
  (这一机制在 Unix 中的实现是通过一个与 Coda 关联的字符设备完成的。Venus 通过对该设备执行 read 来取回消息，通过 write 发送应答，而通知则通过对该设备的文件描述符调用 select 系统调用进行。进程 P 被保持在一个可中断等待队列对象上等待。)

  In Windows NT and the DPMI Windows 95 implementation a DeviceIoControl
  call is used.  The DeviceIoControl call is designed to copy buffers
  from user memory to kernel memory with OPCODES. The sendmsg_to_kernel
  is issued as a synchronous call, while the getmsg_from_kernel call is
  asynchronous.  Windows EventObjects are used for notification of
  message arrival.  The process P is kept waiting on a KernelEvent
  object in NT and a semaphore in Windows 95.
  (在 Windows NT 和 DPMI Windows 95 实现中使用 DeviceIoControl 调用。DeviceIoControl 调用被设计为用操作码(OPCODE)复制用户内存与内核内存之间的缓冲区。sendmsg_to_kernel 以同步调用的方式发起，而 getmsg_from_kernel 调用是异步的。消息到达的通知使用 Windows 事件对象(EventObject)。进程 P 在 NT 中被保持在一个 KernelEvent 对象上等待，在 Windows 95 中则是一个信号量。)


4.  The interface at the call level
(4. 调用层面的接口)
===================================


  This section describes the upcalls a Coda FS driver can make to Venus.
  Each of these upcalls make use of two structures: inputArgs and
  outputArgs.   In pseudo BNF form the structures take the following
  form::
  (本节描述 Coda 文件系统驱动可以向 Venus 发起的上行调用。每个这样的上行调用都使用两个结构：inputArgs 和 outputArgs。以伪 BNF 形式表示，这些结构具有如下形式：)


	struct inputArgs {
	    u_long opcode;
	    u_long unique;     /* Keep multiple outstanding msgs distinct */
	    u_short pid;                 /* Common to all */
	    u_short pgid;                /* Common to all */
	    struct CodaCred cred;        /* Common to all */

	    <union "in" of call dependent parts of inputArgs>
	};

	struct outputArgs {
	    u_long opcode;
	    u_long unique;       /* Keep multiple outstanding msgs distinct */
	    u_long result;

	    <union "out" of call dependent parts of inputArgs>
	};



  Before going on let us elucidate the role of the various fields. The
  inputArgs start with the opcode which defines the type of service
  requested from Venus. There are approximately 30 upcalls at present
  which we will discuss.   The unique field labels the inputArg with a
  unique number which will identify the message uniquely.  A process and
  process group id are passed.  Finally the credentials of the caller
  are included.
  (在继续之前，让我们阐明各个字段的作用。inputArgs 以 opcode 开头，它定义了向 Venus 请求的服务类型。目前大约有 30 个上行调用，我们将逐一讨论。unique 字段用一个唯一的编号来标记 inputArg，该编号将唯一地标识这条消息。进程 ID 和进程组 ID 被传入。最后还包含了调用者的凭据。)

  Before delving into the specific calls we need to discuss a variety of
  data structures shared by the kernel and Venus.
  (在深入讨论具体调用之前，我们需要讨论内核与 Venus 共享的多种数据结构。)




4.1.  Data structures shared by the kernel and Venus
(4.1. 内核与 Venus 共享的数据结构)
----------------------------------------------------


  The CodaCred structure defines a variety of user and group ids as
  they are set for the calling process. The vuid_t and vgid_t are 32 bit
  unsigned integers.  It also defines group membership in an array.  On
  Unix the CodaCred has proven sufficient to implement good security
  semantics for Coda but the structure may have to undergo modification
  for the Windows environment when these mature::
  (CodaCred 结构定义了为调用进程设置的各类用户和组 ID。vuid_t 和 vgid_t 是 32 位无符号整数。它还以数组形式定义组成员关系。在 Unix 上，CodaCred 已被证明足以实现良好的 Coda 安全语义，但当 Windows 环境成熟后，该结构可能不得不进行修改：)

	struct CodaCred {
	    vuid_t cr_uid, cr_euid, cr_suid, cr_fsuid; /* Real, effective, set, fs uid */
	    vgid_t cr_gid, cr_egid, cr_sgid, cr_fsgid; /* same for groups */
	    vgid_t cr_groups[NGROUPS];        /* Group membership for caller */
	};


  .. Note::

     It is questionable if we need CodaCreds in Venus. Finally Venus
     doesn't know about groups, although it does create files with the
     default uid/gid.  Perhaps the list of group membership is superfluous.
     (我们是否需要在 Venus 中使用 CodaCred 是值得怀疑的。归根结底 Venus 并不了解组，尽管它确实会用默认的 uid/gid 创建文件。也许组成员列表是多余的。)


  The next item is the fundamental identifier used to identify Coda
  files, the ViceFid.  A fid of a file uniquely defines a file or
  directory in the Coda filesystem within a cell [1]_::
  (下一项是用于标识 Coda 文件的基本标识符，即 ViceFid。一个文件的 fid 在一个 cell 内唯一地定义 Coda 文件系统中的一个文件或目录 [1]_：)

	typedef struct ViceFid {
	    VolumeId Volume;
	    VnodeId Vnode;
	    Unique_t Unique;
	} ViceFid;

  .. [1] A cell is agroup of Coda servers acting under the aegis of a single
	 system control machine or SCM. See the Coda Administration manual
	 for a detailed description of the role of the SCM.
	 (cell 是在单一系统控制机(SCM)的主导下协同工作的一组 Coda 服务器。关于 SCM 角色的详细描述请参见 Coda 管理手册。)

  Each of the constituent fields: VolumeId, VnodeId and Unique_t are
  unsigned 32 bit integers.  We envisage that a further field will need
  to be prefixed to identify the Coda cell; this will probably take the
  form of a Ipv6 size IP address naming the Coda cell through DNS.
  (各个组成部分：VolumeId、VnodeId 和 Unique_t 都是无符号 32 位整数。我们预计还需要在前面附加一个字段来标识 Coda cell；它可能会采取通过 DNS 命名 Coda cell 的 IPv6 大小 IP 地址的形式。)

  The next important structure shared between Venus and the kernel is
  the attributes of the file.  The following structure is used to
  exchange information.  It has room for future extensions such as
  support for device files (currently not present in Coda)::
  (Venus 与内核之间共享的下一个重要结构是文件的属性。下面的结构用于交换信息。它为未来的扩展留有空间，例如对设备文件的支持(目前 Coda 中尚不存在)：)


	struct coda_timespec {
		int64_t         tv_sec;         /* seconds */
		long            tv_nsec;        /* nanoseconds */
	};

	struct coda_vattr {
		enum coda_vtype va_type;        /* vnode type (for create) */
		u_short         va_mode;        /* files access mode and type */
		short           va_nlink;       /* number of references to file */
		vuid_t          va_uid;         /* owner user id */
		vgid_t          va_gid;         /* owner group id */
		long            va_fsid;        /* file system id (dev for now) */
		long            va_fileid;      /* file id */
		u_quad_t        va_size;        /* file size in bytes */
		long            va_blocksize;   /* blocksize preferred for i/o */
		struct coda_timespec va_atime;  /* time of last access */
		struct coda_timespec va_mtime;  /* time of last modification */
		struct coda_timespec va_ctime;  /* time file changed */
		u_long          va_gen;         /* generation number of file */
		u_long          va_flags;       /* flags defined for file */
		dev_t           va_rdev;        /* device special file represents */
		u_quad_t        va_bytes;       /* bytes of disk space held by file */
		u_quad_t        va_filerev;     /* file modification number */
		u_int           va_vaflags;     /* operations flags, see below */
		long            va_spare;       /* remain quad aligned */
	};


4.2.  The pioctl interface
(4.2. pioctl 接口)
--------------------------


  Coda specific requests can be made by application through the pioctl
  interface. The pioctl is implemented as an ordinary ioctl on a
  fictitious file /coda/.CONTROL.  The pioctl call opens this file, gets
  a file handle and makes the ioctl call. Finally it closes the file.
  (应用程序可以通过 pioctl 接口发起 Coda 特定的请求。pioctl 实现为对一个虚拟文件 /coda/.CONTROL 的普通 ioctl。pioctl 调用打开该文件，获得一个文件句柄并执行 ioctl 调用，最后关闭该文件。)

  The kernel involvement in this is limited to providing the facility to
  open and close and pass the ioctl message and to verify that a path in
  the pioctl data buffers is a file in a Coda filesystem.
  (内核在这其中的参与仅限于提供打开和关闭以及传递 ioctl 消息的机制，并验证 pioctl 数据缓冲区中的路径是 Coda 文件系统中的一个文件。)

  The kernel is handed a data packet of the form::
  (内核会收到如下形式的数据包：)

	struct {
	    const char *path;
	    struct ViceIoctl vidata;
	    int follow;
	} data;



  where::
  (其中：)


	struct ViceIoctl {
		caddr_t in, out;        /* Data to be transferred in, or out */
		short in_size;          /* Size of input buffer <= 2K */
		short out_size;         /* Maximum size of output buffer, <= 2K */
	};



  The path must be a Coda file, otherwise the ioctl upcall will not be
  made.
  (该路径必须是 Coda 文件，否则将不会发起 ioctl 上行调用。)

  .. Note:: The data structures and code are a mess.  We need to clean this up.
     (这些数据结构和代码一团糟。我们需要把它们清理一下。)


**We now proceed to document the individual calls**:
(**下面我们开始逐个记录这些调用**)


4.3.  root
(4.3. root(根目录))
----------


  Arguments
  (参数)
     in
     (输入)

	empty
	(空)

     out::
     (输出：)

		struct cfs_root_out {
		    ViceFid VFid;
		} cfs_root;



  Description
  (描述)
    This call is made to Venus during the initialization of
    the Coda filesystem. If the result is zero, the cfs_root structure
    contains the ViceFid of the root of the Coda filesystem. If a non-zero
    result is generated, its value is a platform dependent error code
    indicating the difficulty Venus encountered in locating the root of
    the Coda filesystem.
    (此调用在 Coda 文件系统初始化期间向 Venus 发起。如果结果为零，cfs_root 结构包含 Coda 文件系统根的 ViceFid。如果产生了非零结果，其值是一个平台相关的错误码，指示 Venus 在定位 Coda 文件系统根时遇到的困难。)

4.4.  lookup
(4.4. lookup(查找))
------------


  Summary
  (概要)
    Find the ViceFid and type of an object in a directory if it exists.
    (在目录中查找某个对象(如果它存在)的 ViceFid 和类型。)

  Arguments
  (参数)
     in::
     (输入：)

		struct  cfs_lookup_in {
		    ViceFid     VFid;
		    char        *name;          /* Place holder for data. */
		} cfs_lookup;



     out::
     (输出：)

		struct cfs_lookup_out {
		    ViceFid VFid;
		    int vtype;
		} cfs_lookup;



  Description
  (描述)
    This call is made to determine the ViceFid and filetype of
    a directory entry.  The directory entry requested carries name 'name'
    and Venus will search the directory identified by cfs_lookup_in.VFid.
    The result may indicate that the name does not exist, or that
    difficulty was encountered in finding it (e.g. due to disconnection).
    If the result is zero, the field cfs_lookup_out.VFid contains the
    targets ViceFid and cfs_lookup_out.vtype the coda_vtype giving the
    type of object the name designates.
    (此调用用于确定目录项的 ViceFid 和文件类型。所请求的目录项带有名称 'name'，Venus 将搜索由 cfs_lookup_in.VFid 标识的目录。结果可能表明该名称不存在，或者在查找它时遇到了困难(例如由于断连)。如果结果为零，则字段 cfs_lookup_out.VFid 包含目标的 ViceFid，而 cfs_lookup_out.vtype 是给出该名称所指对象类型的 coda_vtype。)

  The name of the object is an 8 bit character string of maximum length
  CFS_MAXNAMLEN, currently set to 256 (including a 0 terminator.)
  (对象的名称是最大长度为 CFS_MAXNAMLEN 的 8 位字符字符串，目前设为 256(包括 0 结束符)。)

  It is extremely important to realize that Venus bitwise ors the field
  cfs_lookup.vtype with CFS_NOCACHE to indicate that the object should
  not be put in the kernel name cache.
  (必须极其清楚地认识到：Venus 会将字段 cfs_lookup.vtype 与 CFS_NOCACHE 按位或，以表明该对象不应放入内核名称缓存。)

  .. Note::

     The type of the vtype is currently wrong.  It should be
     coda_vtype. Linux does not take note of CFS_NOCACHE.  It should.
     (vtype 的类型目前是错误的。它应当是 coda_vtype。Linux 没有注意到 CFS_NOCACHE。它应该注意。)


4.5.  getattr
(4.5. getattr(获取属性))
-------------


  Summary Get the attributes of a file.
  (概要：获取文件的属性。)

  Arguments
  (参数)
     in::
     (输入：)

		struct cfs_getattr_in {
		    ViceFid VFid;
		    struct coda_vattr attr; /* XXXXX */
		} cfs_getattr;



     out::
     (输出：)

		struct cfs_getattr_out {
		    struct coda_vattr attr;
		} cfs_getattr;



  Description
  (描述)
    This call returns the attributes of the file identified by fid.
    (此调用返回由 fid 标识的文件的属性。)

  Errors
  (错误)
    Errors can occur if the object with fid does not exist, is
    unaccessible or if the caller does not have permission to fetch
    attributes.
    (如果具有 fid 的对象不存在、不可访问，或者调用者没有获取属性的权限，就会发生错误。)

  .. Note::

     Many kernel FS drivers (Linux, NT and Windows 95) need to acquire
     the attributes as well as the Fid for the instantiation of an internal
     "inode" or "FileHandle".  A significant improvement in performance on
     such systems could be made by combining the lookup and getattr calls
     both at the Venus/kernel interaction level and at the RPC level.
     (许多内核文件系统驱动(Linux、NT 和 Windows 95)需要同时获取属性和 Fid，以便实例化内部的 "inode" 或 "FileHandle"。在 Venus/内核交互层面和 RPC 层面合并 lookup 与 getattr 调用，可以显著提高此类系统的性能。)

  The vattr structure included in the input arguments is superfluous and
  should be removed.
  (输入参数中包含的 vattr 结构是多余的，应当移除。)


4.6.  setattr
(4.6. setattr(设置属性))
-------------


  Summary
  (概要)
    Set the attributes of a file.
    (设置文件的属性。)

  Arguments
  (参数)
     in::
     (输入：)

		struct cfs_setattr_in {
		    ViceFid VFid;
		    struct coda_vattr attr;
		} cfs_setattr;




     out
     (输出)

	empty
	(空)

  Description
  (描述)
    The structure attr is filled with attributes to be changed
    in BSD style.  Attributes not to be changed are set to -1, apart from
    vtype which is set to VNON. Other are set to the value to be assigned.
    The only attributes which the FS driver may request to change are the
    mode, owner, groupid, atime, mtime and ctime.  The return value
    indicates success or failure.
    (结构 attr 以 BSD 风格填入要更改的属性。不更改的属性被设为 -1，但 vtype 除外，它被设为 VNON。其余属性被设为要赋予的值。文件系统驱动可以请求更改的属性只有 mode、owner、groupid、atime、mtime 和 ctime。返回值指示成功或失败。)

  Errors
  (错误)
    A variety of errors can occur.  The object may not exist, may
    be inaccessible, or permission may not be granted by Venus.
    (可能发生多种错误。对象可能不存在、可能不可访问，或者 Venus 可能不授予权限。)


4.7.  access
(4.7. access(访问权限))
------------


  Arguments
  (参数)
     in::
     (输入：)

		struct cfs_access_in {
		    ViceFid     VFid;
		    int flags;
		} cfs_access;



     out
     (输出)

	empty
	(空)

  Description
  (描述)
    Verify if access to the object identified by VFid for
    operations described by flags is permitted.  The result indicates if
    access will be granted.  It is important to remember that Coda uses
    ACLs to enforce protection and that ultimately the servers, not the
    clients enforce the security of the system.  The result of this call
    will depend on whether a token is held by the user.
    (验证是否允许对由 VFid 标识的对象执行 flags 所描述的操作。结果表明是否将授予访问权限。必须记住：Coda 使用 ACL 来实施保护，最终是由服务器而非客户端来保障系统的安全。此调用的结果将取决于用户是否持有令牌。)

  Errors
  (错误)
    The object may not exist, or the ACL describing the protection
    may not be accessible.
    (对象可能不存在，或者描述保护的 ACL 可能不可访问。)


4.8.  create
(4.8. create(创建文件))
------------


  Summary
  (概要)
    Invoked to create a file
    (被调用以创建一个文件)

  Arguments
  (参数)
     in::
     (输入：)

		struct cfs_create_in {
		    ViceFid VFid;
		    struct coda_vattr attr;
		    int excl;
		    int mode;
		    char        *name;          /* Place holder for data. */
		} cfs_create;




     out::
     (输出：)

		struct cfs_create_out {
		    ViceFid VFid;
		    struct coda_vattr attr;
		} cfs_create;



  Description
  (描述)
    This upcall is invoked to request creation of a file.
    The file will be created in the directory identified by VFid, its name
    will be name, and the mode will be mode.  If excl is set an error will
    be returned if the file already exists.  If the size field in attr is
    set to zero the file will be truncated.  The uid and gid of the file
    are set by converting the CodaCred to a uid using a macro CRTOUID
    (this macro is platform dependent).  Upon success the VFid and
    attributes of the file are returned.  The Coda FS Driver will normally
    instantiate a vnode, inode or file handle at kernel level for the new
    object.
    (此上行调用被用来请求创建一个文件。文件将在由 VFid 标识的目录中创建，其名称为 name，模式为 mode。如果设置了 excl 而文件已存在，则返回错误。如果 attr 中的 size 字段被设为零，文件将被截断。文件的 uid 和 gid 通过使用宏 CRTOUID 将 CodaCred 转换为 uid 来设置(该宏是平台相关的)。成功时返回文件的 VFid 和属性。Coda 文件系统驱动通常会在内核层为新对象实例化一个 vnode、inode 或文件句柄。)


  Errors
  (错误)
    A variety of errors can occur. Permissions may be insufficient.
    If the object exists and is not a file the error EISDIR is returned
    under Unix.
    (可能发生多种错误。权限可能不足。如果对象存在且不是文件，则在 Unix 下返回错误 EISDIR。)

  .. Note::

     The packing of parameters is very inefficient and appears to
     indicate confusion between the system call creat and the VFS operation
     create. The VFS operation create is only called to create new objects.
     This create call differs from the Unix one in that it is not invoked
     to return a file descriptor. The truncate and exclusive options,
     together with the mode, could simply be part of the mode as it is
     under Unix.  There should be no flags argument; this is used in open
     (2) to return a file descriptor for READ or WRITE mode.
     (参数的打包方式非常低效，似乎表明系统调用 creat 与 VFS 操作 create 之间存在混淆。VFS 操作 create 只为创建新对象而调用。此 create 调用与 Unix 的不同之处在于，它不是为了返回文件描述符而调用。截断和独占选项连同模式，本可以像 Unix 中那样简单地成为模式的一部分。不应有 flags 参数；它是在 open(2) 中用来返回 READ 或 WRITE 模式的文件描述符的。)

  The attributes of the directory should be returned too, since the size
  and mtime changed.
  (目录的属性也应当返回，因为其大小和 mtime 已改变。)


4.9.  mkdir
(4.9. mkdir(创建目录))
-----------


  Summary
  (概要)
    Create a new directory.
    (创建一个新目录。)

  Arguments
  (参数)
     in::
     (输入：)

		struct cfs_mkdir_in {
		    ViceFid     VFid;
		    struct coda_vattr attr;
		    char        *name;          /* Place holder for data. */
		} cfs_mkdir;



     out::
     (输出：)

		struct cfs_mkdir_out {
		    ViceFid VFid;
		    struct coda_vattr attr;
		} cfs_mkdir;




  Description
  (描述)
    This call is similar to create but creates a directory.
    Only the mode field in the input parameters is used for creation.
    Upon successful creation, the attr returned contains the attributes of
    the new directory.
    (此调用与 create 类似，但创建的是一个目录。创建时只使用输入参数中的 mode 字段。创建成功后，返回的 attr 包含新目录的属性。)

  Errors
  (错误)
    As for create.
    (与 create 相同。)

  .. Note::

     The input parameter should be changed to mode instead of
     attributes.
     (输入参数应当改为 mode 而不是属性。)

  The attributes of the parent should be returned since the size and
  mtime changes.
  (应当返回父目录的属性，因为其大小和 mtime 会改变。)


4.10.  link
(4.10. link(创建硬链接))
-----------


  Summary
  (概要)
    Create a link to an existing file.
    (创建到一个已有文件的链接。)

  Arguments
  (参数)
     in::
     (输入：)

		struct cfs_link_in {
		    ViceFid sourceFid;          /* cnode to link *to* */
		    ViceFid destFid;            /* Directory in which to place link */
		    char        *tname;         /* Place holder for data. */
		} cfs_link;



     out
     (输出)

	empty
	(空)

  Description
  (描述)
    This call creates a link to the sourceFid in the directory
    identified by destFid with name tname.  The source must reside in the
    target's parent, i.e. the source must be have parent destFid, i.e. Coda
    does not support cross directory hard links.  Only the return value is
    relevant.  It indicates success or the type of failure.
    (此调用在由 destFid 标识的目录中创建一个指向 sourceFid 的链接，名称为 tname。源必须位于目标的父目录中，即源的父目录必须是 destFid，也就是说 Coda 不支持跨目录硬链接。只有返回值是相关的，它指示成功或失败的类型。)

  Errors
  (错误)
    The usual errors can occur.
    (可能发生通常的错误。)


4.11.  symlink
(4.11. symlink(创建符号链接))
--------------


  Summary
  (概要)
    create a symbolic link
    (创建一个符号链接)

  Arguments
  (参数)
     in::
     (输入：)

		struct cfs_symlink_in {
		    ViceFid     VFid;          /* Directory to put symlink in */
		    char        *srcname;
		    struct coda_vattr attr;
		    char        *tname;
		} cfs_symlink;



     out
     (输出)

	none
	(无)

  Description
  (描述)
    Create a symbolic link. The link is to be placed in the
    directory identified by VFid and named tname.  It should point to the
    pathname srcname.  The attributes of the newly created object are to
    be set to attr.
    (创建一个符号链接。该链接将被放置在由 VFid 标识的目录中，并命名为 tname。它应指向路径名 srcname。新建对象的属性将被设为 attr。)

  .. Note::

     The attributes of the target directory should be returned since
     its size changed.
     (应当返回目标目录的属性，因为其大小改变了。)


4.12.  remove
(4.12. remove(删除文件))
-------------


  Summary
  (概要)
    Remove a file
    (删除一个文件)

  Arguments
  (参数)
     in::
     (输入：)

		struct cfs_remove_in {
		    ViceFid     VFid;
		    char        *name;          /* Place holder for data. */
		} cfs_remove;



     out
     (输出)

	none
	(无)

  Description
  (描述)
    Remove file named cfs_remove_in.name in directory
    identified by   VFid.
    (在由 VFid 标识的目录中删除名为 cfs_remove_in.name 的文件。)


  .. Note::

     The attributes of the directory should be returned since its
     mtime and size may change.
     (应当返回该目录的属性，因为其 mtime 和大小可能改变。)


4.13.  rmdir
(4.13. rmdir(删除目录))
------------


  Summary
  (概要)
    Remove a directory
    (删除一个目录)

  Arguments
  (参数)
     in::
     (输入：)

		struct cfs_rmdir_in {
		    ViceFid     VFid;
		    char        *name;          /* Place holder for data. */
		} cfs_rmdir;



     out
     (输出)

	none
	(无)

  Description
  (描述)
    Remove the directory with name 'name' from the directory
    identified by VFid.
    (从由 VFid 标识的目录中删除名为 'name' 的目录。)

  .. Note:: The attributes of the parent directory should be returned since
	    its mtime and size may change.
	    (应当返回父目录的属性，因为其 mtime 和大小可能改变。)


4.14.  readlink
(4.14. readlink(读取符号链接))
---------------


  Summary
  (概要)
    Read the value of a symbolic link.
    (读取一个符号链接的值。)

  Arguments
  (参数)
     in::
     (输入：)

		struct cfs_readlink_in {
		    ViceFid VFid;
		} cfs_readlink;



     out::
     (输出：)

		struct cfs_readlink_out {
		    int count;
		    caddr_t     data;           /* Place holder for data. */
		} cfs_readlink;



  Description
  (描述)
    This routine reads the contents of symbolic link
    identified by VFid into the buffer data.  The buffer data must be able
    to hold any name up to CFS_MAXNAMLEN (PATH or NAM??).
    (此例程将 VFid 标识的符号链接的内容读入缓冲区 data。缓冲区 data 必须能够容纳任何最长为 CFS_MAXNAMLEN 的名称(PATH 还是 NAM??)。)

  Errors
  (错误)
    No unusual errors.
    (没有不寻常的错误。)


4.15.  open
(4.15. open(打开))
-----------


  Summary
  (概要)
    Open a file.
    (打开一个文件。)

  Arguments
  (参数)
     in::
     (输入：)

		struct cfs_open_in {
		    ViceFid     VFid;
		    int flags;
		} cfs_open;



     out::
     (输出：)

		struct cfs_open_out {
		    dev_t       dev;
		    ino_t       inode;
		} cfs_open;



  Description
  (描述)
    This request asks Venus to place the file identified by
    VFid in its cache and to note that the calling process wishes to open
    it with flags as in open(2).  The return value to the kernel differs
    for Unix and Windows systems.  For Unix systems the Coda FS Driver is
    informed of the device and inode number of the container file in the
    fields dev and inode.  For Windows the path of the container file is
    returned to the kernel.
    (此请求要求 Venus 将由 VFid 标识的文件放入其缓存，并记录调用进程希望像 open(2) 中那样以 flags 打开它。返回给内核的值在 Unix 和 Windows 系统上有所不同。对于 Unix 系统，容器文件的设备和 inode 号通过 dev 和 inode 字段告知 Coda 文件系统驱动。对于 Windows，容器文件的路径被返回给内核。)


  .. Note::

     Currently the cfs_open_out structure is not properly adapted to
     deal with the Windows case.  It might be best to implement two
     upcalls, one to open aiming at a container file name, the other at a
     container file inode.
     (目前的 cfs_open_out 结构尚未恰当地适配以处理 Windows 的情况。最好实现两个上行调用：一个打开时以容器文件名为目标，另一个以容器文件 inode 为目标。)


4.16.  close
(4.16. close(关闭))
------------


  Summary
  (概要)
    Close a file, update it on the servers.
    (关闭一个文件，并在服务器上对其进行更新。)

  Arguments
  (参数)
     in::
     (输入：)

		struct cfs_close_in {
		    ViceFid     VFid;
		    int flags;
		} cfs_close;



     out
     (输出)

	none
	(无)

  Description
  (描述)
    Close the file identified by VFid.
    (关闭由 VFid 标识的文件。)

  .. Note::

     The flags argument is bogus and not used.  However, Venus' code
     has room to deal with an execp input field, probably this field should
     be used to inform Venus that the file was closed but is still memory
     mapped for execution.  There are comments about fetching versus not
     fetching the data in Venus vproc_vfscalls.  This seems silly.  If a
     file is being closed, the data in the container file is to be the new
     data.  Here again the execp flag might be in play to create confusion:
     currently Venus might think a file can be flushed from the cache when
     it is still memory mapped.  This needs to be understood.
     (flags 参数是虚假的且未被使用。不过，Venus 的代码留有处理 execp 输入字段的余地，或许应当用这个字段告知 Venus：文件已关闭但仍为执行而保持内存映射。在 Venus 的 vproc_vfscalls 中有一些关于取回还是不取回数据的注释。这看起来很愚蠢。如果要关闭一个文件，容器文件中的数据就应当是新数据。这里 execp 标志可能又会造成混淆：目前当文件仍处于内存映射状态时，Venus 可能会认为它可以被从缓存中清除。这一点需要弄清楚。)


4.17.  ioctl
(4.17. ioctl(设备控制))
------------


  Summary
  (概要)
    Do an ioctl on a file. This includes the pioctl interface.
    (对一个文件执行 ioctl。这包括 pioctl 接口。)

  Arguments
  (参数)
     in::
     (输入：)

		struct cfs_ioctl_in {
		    ViceFid VFid;
		    int cmd;
		    int len;
		    int rwflag;
		    char *data;                 /* Place holder for data. */
		} cfs_ioctl;



     out::
     (输出：)


		struct cfs_ioctl_out {
		    int len;
		    caddr_t     data;           /* Place holder for data. */
		} cfs_ioctl;



  Description
  (描述)
    Do an ioctl operation on a file.  The command, len and
    data arguments are filled as usual.  flags is not used by Venus.
    (对一个文件执行 ioctl 操作。command、len 和 data 参数照常填写。Venus 不使用 flags。)

  .. Note::

     Another bogus parameter.  flags is not used.  What is the
     business about PREFETCHING in the Venus code?
     (又一个虚假参数。flags 未被使用。Venus 代码里关于 PREFETCHING 的那套东西是怎么回事？)



4.18.  rename
(4.18. rename(重命名))
-------------


  Summary
  (概要)
    Rename a fid.
    (重命名一个 fid。)

  Arguments
  (参数)
     in::
     (输入：)

		struct cfs_rename_in {
		    ViceFid     sourceFid;
		    char        *srcname;
		    ViceFid destFid;
		    char        *destname;
		} cfs_rename;



     out
     (输出)

	none
	(无)

  Description
  (描述)
    Rename the object with name srcname in directory
    sourceFid to destname in destFid.   It is important that the names
    srcname and destname are 0 terminated strings.  Strings in Unix
    kernels are not always null terminated.
    (将 sourceFid 目录中名为 srcname 的对象重命名为 destFid 中的 destname。重要的是 srcname 和 destname 必须是以 0 结尾的字符串。Unix 内核中的字符串并不总是以空字符结尾。)


4.19.  readdir
(4.19. readdir(读取目录))
--------------


  Summary
  (概要)
    Read directory entries.
    (读取目录项。)

  Arguments
  (参数)
     in::
     (输入：)

		struct cfs_readdir_in {
		    ViceFid     VFid;
		    int count;
		    int offset;
		} cfs_readdir;




     out::
     (输出：)

		struct cfs_readdir_out {
		    int size;
		    caddr_t     data;           /* Place holder for data. */
		} cfs_readdir;



  Description
  (描述)
    Read directory entries from VFid starting at offset and
    read at most count bytes.  Returns the data in data and returns
    the size in size.
    (从 VFid 的 offset 处开始读取目录项，最多读取 count 字节。在 data 中返回数据，并在 size 中返回大小。)


  .. Note::

     This call is not used.  Readdir operations exploit container
     files.  We will re-evaluate this during the directory revamp which is
     about to take place.
     (此调用未被使用。readdir 操作利用容器文件完成。在即将进行的目录改造期间，我们将重新评估这一点。)


4.20.  vget
(4.20. vget(获取 vnode))
-----------


  Summary
  (概要)
    instructs Venus to do an FSDB->Get.
    (指示 Venus 执行一次 FSDB->Get。)

  Arguments
  (参数)
     in::
     (输入：)

		struct cfs_vget_in {
		    ViceFid VFid;
		} cfs_vget;



     out::
     (输出：)

		struct cfs_vget_out {
		    ViceFid VFid;
		    int vtype;
		} cfs_vget;



  Description
  (描述)
    This upcall asks Venus to do a get operation on an fsobj
    labelled by VFid.
    (此上行调用要求 Venus 对由 VFid 标记的 fsobj 执行一次 get 操作。)

  .. Note::

     This operation is not used.  However, it is extremely useful
     since it can be used to deal with read/write memory mapped files.
     These can be "pinned" in the Venus cache using vget and released with
     inactive.
     (此操作未被使用。然而它极其有用，因为它可用于处理读/写内存映射文件。这些文件可以用 vget 在 Venus 缓存中"钉住"，并用 inactive 释放。)


4.21.  fsync
(4.21. fsync(同步文件))
------------


  Summary
  (概要)
    Tell Venus to update the RVM attributes of a file.
    (告知 Venus 更新一个文件的 RVM 属性。)

  Arguments
  (参数)
     in::
     (输入：)

		struct cfs_fsync_in {
		    ViceFid VFid;
		} cfs_fsync;



     out
     (输出)

	none
	(无)

  Description
  (描述)
    Ask Venus to update RVM attributes of object VFid. This
    should be called as part of kernel level fsync type calls.  The
    result indicates if the syncing was successful.
    (要求 Venus 更新对象 VFid 的 RVM 属性。这应当作为内核层 fsync 类调用的一部分被调用。结果表明同步是否成功。)

  .. Note:: Linux does not implement this call. It should.
     (Linux 没有实现此调用。它应该实现。)


4.22.  inactive
(4.22. inactive(释放))
---------------


  Summary
  (概要)
    Tell Venus a vnode is no longer in use.
    (告知 Venus 一个 vnode 不再被使用。)

  Arguments
  (参数)
     in::
     (输入：)

		struct cfs_inactive_in {
		    ViceFid VFid;
		} cfs_inactive;



     out
     (输出)

	none
	(无)

  Description
  (描述)
    This operation returns EOPNOTSUPP.
    (此操作返回 EOPNOTSUPP。)

  .. Note:: This should perhaps be removed.
     (或许应该将此调用移除。)


4.23.  rdwr
(4.23. rdwr(读写))
-----------


  Summary
  (概要)
    Read or write from a file
    (从一个文件读取或写入)

  Arguments
  (参数)
     in::
     (输入：)

		struct cfs_rdwr_in {
		    ViceFid     VFid;
		    int rwflag;
		    int count;
		    int offset;
		    int ioflag;
		    caddr_t     data;           /* Place holder for data. */
		} cfs_rdwr;




     out::
     (输出：)

		struct cfs_rdwr_out {
		    int rwflag;
		    int count;
		    caddr_t     data;   /* Place holder for data. */
		} cfs_rdwr;



  Description
  (描述)
    This upcall asks Venus to read or write from a file.
    (此上行调用要求 Venus 从一个文件读取或写入。)


  .. Note::

    It should be removed since it is against the Coda philosophy that
    read/write operations never reach Venus.  I have been told the
    operation does not work.  It is not currently used.
    (它应当被移除，因为读写操作永远不应到达 Venus 是 Coda 的理念，而此调用与之相悖。有人告诉我这个操作无法工作。它目前未被使用。)



4.24.  odymount
(4.24. odymount(动态挂载))
---------------


  Summary
  (概要)
    Allows mounting multiple Coda "filesystems" on one Unix mount point.
    (允许在一个 Unix 挂载点上挂载多个 Coda "文件系统"。)

  Arguments
  (参数)
     in::
     (输入：)

		struct ody_mount_in {
		    char        *name;          /* Place holder for data. */
		} ody_mount;



     out::
     (输出：)

		struct ody_mount_out {
		    ViceFid VFid;
		} ody_mount;



  Description
  (描述)
    Asks Venus to return the rootfid of a Coda system named
    name.  The fid is returned in VFid.
    (要求 Venus 返回名为 name 的 Coda 系统的 rootfid。fid 在 VFid 中返回。)

  .. Note::

     This call was used by David for dynamic sets.  It should be
     removed since it causes a jungle of pointers in the VFS mounting area.
     It is not used by Coda proper.  Call is not implemented by Venus.
     (此调用曾被 David 用于动态集。它应当被移除，因为它会在 VFS 挂载区域造成一堆指针的纠缠。正式的 Coda 并不使用它。Venus 未实现该调用。)


4.25.  ody_lookup
(4.25. ody_lookup(动态集查找))
-----------------


  Summary
  (概要)
    Looks up something.
    (查找某个东西。)

  Arguments
  (参数)
     in
     (输入)

	irrelevant
	(无关)


     out
     (输出)

	irrelevant
	(无关)


  .. Note:: Gut it. Call is not implemented by Venus.
     (将其掏空。Venus 未实现该调用。)


4.26.  ody_expand
(4.26. ody_expand(动态集展开))
-----------------


  Summary
  (概要)
    expands something in a dynamic set.
    (在动态集中展开某个东西。)

  Arguments
  (参数)
     in
     (输入)

	irrelevant
	(无关)

     out
     (输出)

	irrelevant
	(无关)

  .. Note:: Gut it. Call is not implemented by Venus.
     (将其掏空。Venus 未实现该调用。)


4.27.  prefetch
(4.27. prefetch(预取))
---------------


  Summary
  (概要)
    Prefetch a dynamic set.
    (预取一个动态集。)

  Arguments
  (参数)

     in
     (输入)

	Not documented.
	(未记录。)

     out
     (输出)

	Not documented.
	(未记录。)

  Description
  (描述)
    Venus worker.cc has support for this call, although it is
    noted that it doesn't work.  Not surprising, since the kernel does not
    have support for it. (ODY_PREFETCH is not a defined operation).
    (Venus 的 worker.cc 对此调用有支持，尽管有人指出它不起作用。这并不奇怪，因为内核并不支持它。(ODY_PREFETCH 不是一个已定义的操作)。)


  .. Note:: Gut it. It isn't working and isn't used by Coda.
     (将其掏空。它不起作用，Coda 也不使用它。)



4.28.  signal
(4.28. signal(信号))
-------------


  Summary
  (概要)
    Send Venus a signal about an upcall.
    (就一次上行调用向 Venus 发送一个信号。)

  Arguments
  (参数)
     in
     (输入)

	none
	(无)

     out
     (输出)

	not applicable.
	(不适用。)

  Description
  (描述)
    This is an out-of-band upcall to Venus to inform Venus
    that the calling process received a signal after Venus read the
    message from the input queue.  Venus is supposed to clean up the
    operation.
    (这是一个带外上行调用，用于告知 Venus：在 Venus 从输入队列读取消息之后，调用进程收到了一个信号。Venus 应当清理该操作。)

  Errors
  (错误)
    No reply is given.
    (不给出应答。)

  .. Note::

     We need to better understand what Venus needs to clean up and if
     it is doing this correctly.  Also we need to handle multiple upcall
     per system call situations correctly.  It would be important to know
     what state changes in Venus take place after an upcall for which the
     kernel is responsible for notifying Venus to clean up (e.g. open
     definitely is such a state change, but many others are maybe not).
     (我们需要更好地理解 Venus 需要清理什么，以及它是否在正确地做这件事。我们还需要正确处理每次系统调用中有多个上行调用的情况。了解在一次上行调用之后 Venus 中发生了哪些状态变化也很重要，对于这些变化，内核有责任通知 Venus 进行清理(例如 open 肯定是这样的状态变化，但许多其他的也许不是)。)


5.  The minicache and downcalls
(5. minicache 与下行调用)
===============================


  The Coda FS Driver can cache results of lookup and access upcalls, to
  limit the frequency of upcalls.  Upcalls carry a price since a process
  context switch needs to take place.  The counterpart of caching the
  information is that Venus will notify the FS Driver that cached
  entries must be flushed or renamed.
  (Coda 文件系统驱动可以缓存 lookup 和 access 上行调用的结果，以限制上行调用的频率。上行调用是有代价的，因为需要进行一次进程上下文切换。缓存信息的另一面是，Venus 会通知文件系统驱动：被缓存的条目必须被刷新或重命名。)

  The kernel code generally has to maintain a structure which links the
  internal file handles (called vnodes in BSD, inodes in Linux and
  FileHandles in Windows) with the ViceFid's which Venus maintains.  The
  reason is that frequent translations back and forth are needed in
  order to make upcalls and use the results of upcalls.  Such linking
  objects are called cnodes.
  (内核代码通常必须维护一个结构，将内部文件句柄(在 BSD 中称为 vnode，在 Linux 中称为 inode，在 Windows 中称为 FileHandle)与 Venus 维护的 ViceFid 关联起来。原因是为了发起上行调用并使用其结果，需要频繁地在两者之间来回转换。这类关联对象称为 cnode。)

  The current minicache implementations have cache entries which record
  the following:
  (目前的 minicache 实现中的缓存条目记录以下内容：)

  1. the name of the file
     (文件的名称)

  2. the cnode of the directory containing the object
     (包含该对象的目录的 cnode)

  3. a list of CodaCred's for which the lookup is permitted.
     (允许对其进行查找的 CodaCred 列表。)

  4. the cnode of the object
     (该对象的 cnode)

  The lookup call in the Coda FS Driver may request the cnode of the
  desired object from the cache, by passing its name, directory and the
  CodaCred's of the caller.  The cache will return the cnode or indicate
  that it cannot be found.  The Coda FS Driver must be careful to
  invalidate cache entries when it modifies or removes objects.
  (Coda 文件系统驱动中的 lookup 调用可以通过传入名称、目录和调用者的 CodaCred，向缓存请求目标对象的 cnode。缓存将返回该 cnode 或表明无法找到它。Coda 文件系统驱动在修改或移除对象时必须小心地使缓存条目失效。)

  When Venus obtains information that indicates that cache entries are
  no longer valid, it will make a downcall to the kernel.  Downcalls are
  intercepted by the Coda FS Driver and lead to cache invalidations of
  the kind described below.  The Coda FS Driver does not return an error
  unless the downcall data could not be read into kernel memory.
  (当 Venus 获得表明缓存条目不再有效的信息时，它会向内核发起一次下行调用(downcall)。下行调用由 Coda 文件系统驱动截获，并导致如下所述的缓存失效。除非下行调用数据无法读入内核内存，否则 Coda 文件系统驱动不会返回错误。)


5.1.  INVALIDATE
(5.1. INVALIDATE(使无效))
----------------


  No information is available on this call.
  (关于此调用没有可用的信息。)


5.2.  FLUSH
(5.2. FLUSH(刷新))
-----------



  Arguments
  (参数)
    None
    (无)

  Summary
  (概要)
    Flush the name cache entirely.
    (完全刷新名称缓存。)

  Description
  (描述)
    Venus issues this call upon startup and when it dies. This
    is to prevent stale cache information being held.  Some operating
    systems allow the kernel name cache to be switched off dynamically.
    When this is done, this downcall is made.
    (Venus 在启动时和消亡时发出此调用。这是为了防止持有过期的缓存信息。某些操作系统允许动态关闭内核名称缓存。当这样做时，就会进行此下行调用。)


5.3.  PURGEUSER
(5.3. PURGEUSER(清除用户))
---------------


  Arguments
  (参数)
    ::

	  struct cfs_purgeuser_out {/* CFS_PURGEUSER is a venus->kernel call */
	      struct CodaCred cred;
	  } cfs_purgeuser;



  Description
  (描述)
    Remove all entries in the cache carrying the Cred.  This
    call is issued when tokens for a user expire or are flushed.
    (移除缓存中所有携带该 Cred 的条目。当用户的令牌过期或被清除时发出此调用。)


5.4.  ZAPFILE
(5.4. ZAPFILE(清除文件缓存项))
-------------


  Arguments
  (参数)
    ::

	  struct cfs_zapfile_out {  /* CFS_ZAPFILE is a venus->kernel call */
	      ViceFid CodaFid;
	  } cfs_zapfile;



  Description
  (描述)
    Remove all entries which have the (dir vnode, name) pair.
    This is issued as a result of an invalidation of cached attributes of
    a vnode.
    (移除所有具有(目录 vnode，名称)对的条目。这是由于某个 vnode 的缓存属性失效而发出的。)

  .. Note::

     Call is not named correctly in NetBSD and Mach.  The minicache
     zapfile routine takes different arguments. Linux does not implement
     the invalidation of attributes correctly.
     (该调用在 NetBSD 和 Mach 中的命名不正确。minicache 的 zapfile 例程采用不同的参数。Linux 没有正确实现属性的失效处理。)



5.5.  ZAPDIR
(5.5. ZAPDIR(清除目录缓存项))
------------


  Arguments
  (参数)
    ::

	  struct cfs_zapdir_out {   /* CFS_ZAPDIR is a venus->kernel call */
	      ViceFid CodaFid;
	  } cfs_zapdir;



  Description
  (描述)
    Remove all entries in the cache lying in a directory
    CodaFid, and all children of this directory. This call is issued when
    Venus receives a callback on the directory.
    (移除缓存中位于目录 CodaFid 内的所有条目，以及该目录的所有子项。当 Venus 收到关于该目录的回调时发出此调用。)


5.6.  ZAPVNODE
(5.6. ZAPVNODE(清除 vnode 缓存项))
--------------



  Arguments
  (参数)
    ::

	  struct cfs_zapvnode_out { /* CFS_ZAPVNODE is a venus->kernel call */
	      struct CodaCred cred;
	      ViceFid VFid;
	  } cfs_zapvnode;



  Description
  (描述)
    Remove all entries in the cache carrying the cred and VFid
    as in the arguments. This downcall is probably never issued.
    (移除缓存中所有携带与参数相同的 cred 和 VFid 的条目。此下行调用可能从未被发出。)


5.7.  PURGEFID
(5.7. PURGEFID(清除 fid))
--------------


  Arguments
  (参数)
    ::

	  struct cfs_purgefid_out { /* CFS_PURGEFID is a venus->kernel call */
	      ViceFid CodaFid;
	  } cfs_purgefid;



  Description
  (描述)
    Flush the attribute for the file. If it is a dir (odd
    vnode), purge its children from the namecache and remove the file from the
    namecache.
    (刷新该文件的属性。如果它是一个目录(奇数 vnode)，则将其子项从名称缓存中清除，并将该文件从名称缓存中移除。)



5.8.  REPLACE
(5.8. REPLACE(替换))
-------------


  Summary
  (概要)
    Replace the Fid's for a collection of names.
    (替换一组名称的 Fid。)

  Arguments
  (参数)
    ::

	  struct cfs_replace_out { /* cfs_replace is a venus->kernel call */
	      ViceFid NewFid;
	      ViceFid OldFid;
	  } cfs_replace;



  Description
  (描述)
    This routine replaces a ViceFid in the name cache with
    another.  It is added to allow Venus during reintegration to replace
    locally allocated temp fids while disconnected with global fids even
    when the reference counts on those fids are not zero.
    (此例程将名称缓存中的一个 ViceFid 替换为另一个。增加它是为了允许 Venus 在重新集成(reintegration)期间，将断连时本地分配的临时 fid 替换为全局 fid，即使这些 fid 上的引用计数不为零。)


6.  Initialization and cleanup
(6. 初始化与清理)
==============================


  This section gives brief hints as to desirable features for the Coda
  FS Driver at startup and upon shutdown or Venus failures.  Before
  entering the discussion it is useful to repeat that the Coda FS Driver
  maintains the following data:
  (本节就 Coda 文件系统驱动在启动时以及关闭或 Venus 失败时应具备的特性给出简要提示。在进入讨论之前，有必要重申：Coda 文件系统驱动维护着以下数据：)


  1. message queues
     (消息队列)

  2. cnodes
     (cnode)

  3. name cache entries
     (名称缓存条目)

     The name cache entries are entirely private to the driver, so they
     can easily be manipulated.   The message queues will generally have
     clear points of initialization and destruction.  The cnodes are
     much more delicate.  User processes hold reference counts in Coda
     filesystems and it can be difficult to clean up the cnodes.
     (名称缓存条目完全为驱动私有，因此很容易操作。消息队列通常有明确的初始化和销毁时机。cnode 则要微妙得多。用户进程在 Coda 文件系统中持有引用计数，清理 cnode 可能会很困难。)

  It can expect requests through:
  (它可以预期来自以下途径的请求：)

  1. the message subsystem
     (消息子系统)

  2. the VFS layer
     (VFS 层)

  3. pioctl interface
     (pioctl 接口)

     Currently the pioctl passes through the VFS for Coda so we can
     treat these similarly.
     (目前 pioctl 要经过 Coda 的 VFS，因此我们可以对它们做类似的处理。)


6.1.  Requirements
(6.1. 需求)
------------------


  The following requirements should be accommodated:
  (应当满足以下需求：)

  1. The message queues should have open and close routines.  On Unix
     the opening of the character devices are such routines.
     (消息队列应当有打开和关闭例程。在 Unix 上，字符设备的打开操作就是这样的例程。)

    -  Before opening, no messages can be placed.
       (在打开之前，不能放置任何消息。)

    -  Opening will remove any old messages still pending.
       (打开操作将移除任何仍处于未决状态的旧消息。)

    -  Close will notify any sleeping processes that their upcall cannot
       be completed.
       (关闭操作将通知任何正在睡眠的进程：它们的上行调用无法完成。)

    -  Close will free all memory allocated by the message queues.
       (关闭操作将释放由消息队列分配的所有内存。)


  2. At open the namecache shall be initialized to empty state.
     (在打开时，名称缓存应被初始化为空状态。)

  3. Before the message queues are open, all VFS operations will fail.
     Fortunately this can be achieved by making sure than mounting the
     Coda filesystem cannot succeed before opening.
     (在消息队列打开之前，所有 VFS 操作都将失败。幸运的是，可以通过确保在打开之前挂载 Coda 文件系统不能成功来实现这一点。)

  4. After closing of the queues, no VFS operations can succeed.  Here
     one needs to be careful, since a few operations (lookup,
     read/write, readdir) can proceed without upcalls.  These must be
     explicitly blocked.
     (在队列关闭之后，任何 VFS 操作都不能成功。这里需要小心，因为少数操作(lookup、read/write、readdir)可以在没有上行调用的情况下进行。这些必须被显式地阻塞。)

  5. Upon closing the namecache shall be flushed and disabled.
     (在关闭时，名称缓存应被刷新并禁用。)

  6. All memory held by cnodes can be freed without relying on upcalls.
     (cnode 持有的所有内存都可以在不依赖上行调用的情况下释放。)

  7. Unmounting the file system can be done without relying on upcalls.
     (卸载文件系统可以在不依赖上行调用的情况下完成。)

  8. Mounting the Coda filesystem should fail gracefully if Venus cannot
     get the rootfid or the attributes of the rootfid.  The latter is
     best implemented by Venus fetching these objects before attempting
     to mount.
     (如果 Venus 无法获得 rootfid 或 rootfid 的属性，挂载 Coda 文件系统应优雅地失败。后者最好通过 Venus 在尝试挂载之前预先获取这些对象来实现。)

  .. Note::

     NetBSD in particular but also Linux have not implemented the
     above requirements fully.  For smooth operation this needs to be
     corrected.
     (尤其是 NetBSD，还有 Linux，都没有完整实现上述需求。为了平稳运行，这需要加以纠正。)



