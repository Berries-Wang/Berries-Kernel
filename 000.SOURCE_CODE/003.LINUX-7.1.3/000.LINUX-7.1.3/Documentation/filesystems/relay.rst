.. SPDX-License-Identifier: GPL-2.0

==================================
relay interface (formerly relayfs)
==================================

(relay 接口(前身为 relayfs))

The relay interface provides a means for kernel applications to
efficiently log and transfer large quantities of data from the kernel
to userspace via user-defined 'relay channels'.
(relay 接口为内核应用程序提供了一种手段，用于通过用户自定义的"relay 通道"高效地记录并将大量数据从内核传输到用户空间。)

A 'relay channel' is a kernel->user data relay mechanism implemented
as a set of per-cpu kernel buffers ('channel buffers'), each
represented as a regular file ('relay file') in user space.  Kernel
clients write into the channel buffers using efficient write
functions; these automatically log into the current cpu's channel
buffer.  User space applications mmap() or read() from the relay files
and retrieve the data as it becomes available.  The relay files
themselves are files created in a host filesystem, e.g. debugfs, and
are associated with the channel buffers using the API described below.
(relay 通道是一种内核->用户的数据中继机制，实现为一组每 CPU 的内核缓冲区("通道缓冲区")，每个缓冲区在用户空间中表示为一个常规文件("relay 文件")。内核客户端使用高效的写函数写入通道缓冲区；这些函数会自动记录到当前 CPU 的通道缓冲区中。用户空间应用程序对 relay 文件执行 mmap() 或 read()，并在数据变得可用时将其取回。relay 文件本身是在宿主文件系统(例如 debugfs)中创建的文件，并通过下文所述的 API 与通道缓冲区相关联。)

The format of the data logged into the channel buffers is completely
up to the kernel client; the relay interface does however provide
hooks which allow kernel clients to impose some structure on the
buffer data.  The relay interface doesn't implement any form of data
filtering - this also is left to the kernel client.  The purpose is to
keep things as simple as possible.
(记录到通道缓冲区中的数据格式完全由内核客户端决定；不过 relay 接口确实提供了一些钩子，允许内核客户端对缓冲区数据施加一定的结构。relay 接口不实现任何形式的数据过滤——这同样留给内核客户端处理。其目的是让一切尽可能简单。)

This document provides an overview of the relay interface API.  The
details of the function parameters are documented along with the
functions in the relay interface code - please see that for details.
(本文档概述 relay 接口 API。函数参数的细节在 relay 接口代码中与函数一起说明——详情请参阅代码本身。)

Semantics
(语义)
=========

Each relay channel has one buffer per CPU; each buffer has one or more
sub-buffers.  Messages are written to the first sub-buffer until it is
too full to contain a new message, in which case it is written to
the next (if available).  Messages are never split across sub-buffers.
At this point, userspace can be notified so it empties the first
sub-buffer, while the kernel continues writing to the next.
(每个 relay 通道在每个 CPU 上各有一个缓冲区；每个缓冲区有一个或多个 sub-buffer(子缓冲区)。消息被写入第一个 sub-buffer，直到它满到装不下新消息为止，此时消息会被写入下一个 sub-buffer(如果可用)。消息绝不会跨 sub-buffer 分割。此时，用户空间可以得到通知，从而清空第一个 sub-buffer，而内核继续写下一个。)

When notified that a sub-buffer is full, the kernel knows how many
bytes of it are padding, i.e., unused space occurring because a complete
message couldn't fit into a sub-buffer.  Userspace can use this
knowledge to copy only valid data.
(当收到某个 sub-buffer 已满的通知时，内核知道它有多少字节是填充，即由于完整消息装不进 sub-buffer 而产生的未使用空间。用户空间可以利用这一信息只复制有效数据。)

After copying it, userspace can notify the kernel that a sub-buffer
has been consumed.
(复制完成后，用户空间可以通知内核某个 sub-buffer 已被消费。)

A relay channel can operate in a mode where it will overwrite data not
yet collected by userspace, and not wait for it to be consumed.
(relay 通道可以在这样一种模式下运行：它会覆盖用户空间尚未收集的数据，而不等待其被消费。)

The relay channel itself does not provide for communication of such
data between userspace and kernel, allowing the kernel side to remain
simple and not impose a single interface on userspace.  It does
provide a set of examples and a separate helper though, described
below.
(relay 通道本身不提供用户空间与内核之间对此类数据的通信机制，从而使内核侧保持简单，也不会向用户空间强加单一接口。不过它确实提供了一组示例和一个独立的辅助工具，见下文描述。)

The read() interface both removes padding and internally consumes the
read sub-buffers; thus in cases where read(2) is being used to drain
the channel buffers, special-purpose communication between kernel and
user isn't necessary for basic operation.
(read() 接口既会去除填充，又会在内部消费已读取的 sub-buffer；因此在使用 read(2) 排空通道缓冲区的场景下，基本操作无需内核与用户之间专门的通信。)

One of the major goals of the relay interface is to provide a low
overhead mechanism for conveying kernel data to userspace.  While the
read() interface is easy to use, it's not as efficient as the mmap()
approach; the example code attempts to make the tradeoff between the
two approaches as small as possible.
(relay 接口的一个主要目标是提供一种向用户空间传递内核数据的低开销机制。read() 接口易于使用，但不如 mmap() 方式高效；示例代码尽力使两种方式之间的取舍代价尽可能小。)

klog and relay-apps example code
(klog 与 relay-apps 示例代码)
================================

The relay interface itself is ready to use, but to make things easier,
a couple of simple utility functions and a set of examples are provided.
(relay 接口本身已可直接使用，但为了让事情更简单，还提供了一些简单的工具函数和一组示例。)

The relay-apps example tarball, available on the relay sourceforge
site, contains a set of self-contained examples, each consisting of a
pair of .c files containing boilerplate code for each of the user and
kernel sides of a relay application.  When combined these two sets of
boilerplate code provide glue to easily stream data to disk, without
having to bother with mundane housekeeping chores.
(relay-apps 示例压缩包可在 relay sourceforge 网站获取，其中包含一组自包含的示例，每个示例由一对 .c 文件组成，分别包含 relay 应用程序用户侧和内核侧的样板代码。将这两组样板代码组合起来，即可轻松地把数据流写入磁盘，而不必操心繁琐的例行杂务。)

The 'klog debugging functions' patch (klog.patch in the relay-apps
tarball) provides a couple of high-level logging functions to the
kernel which allow writing formatted text or raw data to a channel,
regardless of whether a channel to write into exists or not, or even
whether the relay interface is compiled into the kernel or not.  These
functions allow you to put unconditional 'trace' statements anywhere
in the kernel or kernel modules; only when there is a 'klog handler'
registered will data actually be logged (see the klog and kleak
examples for details).
("klog 调试函数"补丁(relay-apps 压缩包中的 klog.patch)为内核提供了一对高级日志函数，允许向通道写入格式化文本或原始数据，而无论要写入的通道是否存在，甚至无论 relay 接口是否编译进内核。这些函数让你可以在内核或内核模块的任何位置放置无条件的"trace"语句；只有当注册了"klog 处理程序"时，数据才会被真正记录(详见 klog 与 kleak 示例)。)

It is of course possible to use the relay interface from scratch,
i.e., without using any of the relay-apps example code or klog, but
you'll have to implement communication between userspace and kernel,
allowing both to convey the state of buffers (full, empty, amount of
padding).  The read() interface both removes padding and internally
consumes the read sub-buffers; thus in cases where read(2) is being
used to drain the channel buffers, special-purpose communication
between kernel and user isn't necessary for basic operation.  Things
such as buffer-full conditions would still need to be communicated via
some channel though.
(当然，也完全可以从零开始使用 relay 接口，即不使用任何 relay-apps 示例代码或 klog，但那样你就得自己实现用户空间与内核之间的通信，让双方能够传递缓冲区的状态(满、空、填充量)。read() 接口既会去除填充，又会在内部消费已读取的 sub-buffer；因此在使用 read(2) 排空通道缓冲区的场景下，基本操作无需内核与用户之间专门的通信。不过，缓冲区已满之类的状况仍然需要通过某种通道来传递。)

klog and the relay-apps examples can be found in the relay-apps
tarball on http://relayfs.sourceforge.net
(klog 与 relay-apps 示例可在 http://relayfs.sourceforge.net 上的 relay-apps 压缩包中找到。)

The relay interface user space API
(relay 接口用户空间 API)
==================================

The relay interface implements basic file operations for user space
access to relay channel buffer data.  Here are the file operations
that are available and some comments regarding their behavior:
(relay 接口为用户空间访问 relay 通道缓冲区数据实现了基本的文件操作。以下是可用的文件操作以及关于其行为的一些说明：)

=========== ============================================================
open()	    enables user to open an _existing_ channel buffer. (允许用户打开一个_已存在_的通道缓冲区。)

mmap()      results in channel buffer being mapped into the caller's
	    memory space. Note that you can't do a partial mmap - you
	    must map the entire file, which is NRBUF * SUBBUFSIZE. (使通道缓冲区被映射到调用者的内存空间。注意不能进行部分 mmap——必须映射整个文件，即 NRBUF * SUBBUFSIZE。)

read()      read the contents of a channel buffer.  The bytes read are
	    'consumed' by the reader, i.e., they won't be available
	    again to subsequent reads.  If the channel is being used
	    in no-overwrite mode (the default), it can be read at any
	    time even if there's an active kernel writer.  If the
	    channel is being used in overwrite mode and there are
	    active channel writers, results may be unpredictable -
	    users should make sure that all logging to the channel has
	    ended before using read() with overwrite mode.  Sub-buffer
	    padding is automatically removed and will not be seen by
	    the reader. (读取通道缓冲区的内容。读到的字节会被读取者"消费"，即不会再提供给后续的读取。如果通道以非覆盖模式(默认)使用，则任何时候都可以读取，即使存在活动的内核写入者。如果通道以覆盖模式使用且存在活动的通道写入者，结果可能不可预测——用户应确保在使用 read() 配合覆盖模式之前，所有对该通道的日志记录都已结束。sub-buffer 填充会被自动去除，读取者不会看到。)

sendfile()  transfer data from a channel buffer to an output file
	    descriptor. Sub-buffer padding is automatically removed
	    and will not be seen by the reader. (将数据从通道缓冲区传输到一个输出文件描述符。sub-buffer 填充会被自动去除，读取者不会看到。)

poll()      POLLIN/POLLRDNORM/POLLERR supported.  User applications are
	    notified when sub-buffer boundaries are crossed. (支持 POLLIN/POLLRDNORM/POLLERR。跨越 sub-buffer 边界时通知用户应用程序。)

close()     decrements the channel buffer's refcount.  When the refcount
	    reaches 0, i.e., when no process or kernel client has the
	    buffer open, the channel buffer is freed. (递减通道缓冲区的引用计数。当引用计数降为 0，即没有任何进程或内核客户端打开该缓冲区时，通道缓冲区被释放。)
=========== ============================================================

In order for a user application to make use of relay files, the
host filesystem must be mounted.  For example::

	mount -t debugfs debugfs /sys/kernel/debug

(用户应用程序要使用 relay 文件，就必须先挂载宿主文件系统。例如:)

.. Note::

	The host filesystem doesn't need to be mounted for kernel
	clients to create or use channels - it only needs to be
	mounted when user space applications need access to the buffer
	data.
	(内核客户端创建或使用通道时不需要挂载宿主文件系统——只有当用户空间应用程序需要访问缓冲区数据时才需要挂载。)


The relay interface kernel API
(relay 接口内核 API)
==============================

Here's a summary of the API the relay interface provides to in-kernel clients:
(以下是 relay 接口提供给内核内客户端的 API 摘要：)

TBD(curr. line MT:/API/)
  channel management functions::

    relay_open(base_filename, parent, subbuf_size, n_subbufs,
               callbacks, private_data)
    relay_close(chan)
    relay_flush(chan)
    relay_reset(chan)

  (通道管理函数(TBD, 当前行 MT:/API/):)

  channel management typically called on instigation of userspace::

    relay_subbufs_consumed(chan, cpu, subbufs_consumed)

  (通常在用户空间的触发下调用的通道管理函数:)

  write functions::

    relay_write(chan, data, length)
    __relay_write(chan, data, length)
    relay_reserve(chan, length)

  (写函数:)

  callbacks::

    subbuf_start(buf, subbuf, prev_subbuf, prev_padding)
    buf_mapped(buf, filp)
    buf_unmapped(buf, filp)
    create_buf_file(filename, parent, mode, buf, is_global)
    remove_buf_file(dentry)

  (回调函数:)

  helper functions::

    relay_buf_full(buf)
    subbuf_start_reserve(buf, length)

  (辅助函数:)


Creating a channel
(创建通道)
------------------

relay_open() is used to create a channel, along with its per-cpu
channel buffers.  Each channel buffer will have an associated file
created for it in the host filesystem, which can be and mmapped or
read from in user space.  The files are named basename0...basenameN-1
where N is the number of online cpus, and by default will be created
in the root of the filesystem (if the parent param is NULL).  If you
want a directory structure to contain your relay files, you should
create it using the host filesystem's directory creation function,
e.g. debugfs_create_dir(), and pass the parent directory to
relay_open().  Users are responsible for cleaning up any directory
structure they create, when the channel is closed - again the host
filesystem's directory removal functions should be used for that,
e.g. debugfs_remove().
(relay_open() 用于创建通道及其每 CPU 的通道缓冲区。每个通道缓冲区都会在宿主文件系统中创建一个与之关联的文件，用户空间可以对其 mmap 或读取。这些文件命名为 basename0...basenameN-1，其中 N 是在线 CPU 的数量，并且默认创建在文件系统的根目录下(如果 parent 参数为 NULL)。如果你希望用一个目录结构来容纳你的 relay 文件，应当使用宿主文件系统的目录创建函数(例如 debugfs_create_dir())来创建它，并把父目录传给 relay_open()。通道关闭时，用户负责清理自己创建的任何目录结构——同样应使用宿主文件系统的目录删除函数，例如 debugfs_remove()。)

In order for a channel to be created and the host filesystem's files
associated with its channel buffers, the user must provide definitions
for two callback functions, create_buf_file() and remove_buf_file().
create_buf_file() is called once for each per-cpu buffer from
relay_open() and allows the user to create the file which will be used
to represent the corresponding channel buffer.  The callback should
return the dentry of the file created to represent the channel buffer.
remove_buf_file() must also be defined; it's responsible for deleting
the file(s) created in create_buf_file() and is called during
relay_close().
(要创建通道并让宿主文件系统的文件与通道缓冲区相关联，用户必须提供两个回调函数的定义：create_buf_file() 和 remove_buf_file()。create_buf_file() 会在 relay_open() 中为每个每 CPU 缓冲区调用一次，允许用户创建用于表示对应通道缓冲区的文件。该回调应返回所创建的表示通道缓冲区的文件的 dentry。remove_buf_file() 也必须定义；它负责删除在 create_buf_file() 中创建的文件，并在 relay_close() 期间被调用。)

Here are some typical definitions for these callbacks, in this case
using debugfs::

    /*
    * create_buf_file() callback.  Creates relay file in debugfs.
    */
    static struct dentry *create_buf_file_handler(const char *filename,
						struct dentry *parent,
						umode_t mode,
						struct rchan_buf *buf,
						int *is_global)
    {
	    return debugfs_create_file(filename, mode, parent, buf,
				    &relay_file_operations);
    }

    /*
    * remove_buf_file() callback.  Removes relay file from debugfs.
    */
    static int remove_buf_file_handler(struct dentry *dentry)
    {
	    debugfs_remove(dentry);

	    return 0;
    }

    /*
    * relay interface callbacks
    */
    static struct rchan_callbacks relay_callbacks =
    {
	    .create_buf_file = create_buf_file_handler,
	    .remove_buf_file = remove_buf_file_handler,
    };

(以下是使用 debugfs 时这两个回调的一些典型定义:)

And an example relay_open() invocation using them::

  chan = relay_open("cpu", NULL, SUBBUF_SIZE, N_SUBBUFS, &relay_callbacks, NULL);

(以及一个使用它们的 relay_open() 调用示例:)

If the create_buf_file() callback fails, or isn't defined, channel
creation and thus relay_open() will fail.
(如果 create_buf_file() 回调失败或未定义，通道创建乃至 relay_open() 都会失败。)

The total size of each per-cpu buffer is calculated by multiplying the
number of sub-buffers by the sub-buffer size passed into relay_open().
The idea behind sub-buffers is that they're basically an extension of
double-buffering to N buffers, and they also allow applications to
easily implement random-access-on-buffer-boundary schemes, which can
be important for some high-volume applications.  The number and size
of sub-buffers is completely dependent on the application and even for
the same application, different conditions will warrant different
values for these parameters at different times.  Typically, the right
values to use are best decided after some experimentation; in general,
though, it's safe to assume that having only 1 sub-buffer is a bad
idea - you're guaranteed to either overwrite data or lose events
depending on the channel mode being used.
(每个每 CPU 缓冲区的总大小由 sub-buffer 数量乘以传入 relay_open() 的 sub-buffer 大小计算得出。sub-buffer 的思路本质上是把双缓冲扩展为 N 个缓冲，并且它们还允许应用程序轻松实现基于缓冲区边界的随机访问方案，这对某些高吞吐应用很重要。sub-buffer 的数量和大小完全取决于应用程序，即使是同一个应用，不同时候的不同条件也需要这些参数取不同的值。通常，合适的取值最好经过一些实验后再决定；不过一般来说，可以放心地认为只使用 1 个 sub-buffer 是个坏主意——根据通道所用的模式，你必然要么覆盖数据，要么丢失事件。)

The create_buf_file() implementation can also be defined in such a way
as to allow the creation of a single 'global' buffer instead of the
default per-cpu set.  This can be useful for applications interested
mainly in seeing the relative ordering of system-wide events without
the need to bother with saving explicit timestamps for the purpose of
merging/sorting per-cpu files in a postprocessing step.
(create_buf_file() 的实现也可以定义成允许创建单个"全局"缓冲区，而不是默认的每 CPU 缓冲区集合。对于主要关心查看系统范围内事件相对顺序、而不想在后处理步骤中为合并/排序每 CPU 文件而费心保存显式时间戳的应用来说，这会很有用。)

To have relay_open() create a global buffer, the create_buf_file()
implementation should set the value of the is_global outparam to a
non-zero value in addition to creating the file that will be used to
represent the single buffer.  In the case of a global buffer,
create_buf_file() and remove_buf_file() will be called only once.  The
normal channel-writing functions, e.g. relay_write(), can still be
used - writes from any cpu will transparently end up in the global
buffer - but since it is a global buffer, callers should make sure
they use the proper locking for such a buffer, either by wrapping
writes in a spinlock, or by copying a write function from relay.h and
creating a local version that internally does the proper locking.
(要让 relay_open() 创建全局缓冲区，create_buf_file() 的实现除了创建用于表示该单一缓冲区的文件之外，还应把 is_global 出参设置为非零值。对于全局缓冲区，create_buf_file() 和 remove_buf_file() 只会被调用一次。常规的通道写函数(例如 relay_write())仍然可以使用——来自任何 CPU 的写入都会透明地落入全局缓冲区——但正因为它是全局缓冲区，调用者应确保对这样的缓冲区使用正确的加锁，要么用自旋锁包裹写入，要么从 relay.h 复制一个写函数并创建一个内部做了正确加锁的本地版本。)

The private_data passed into relay_open() allows clients to associate
user-defined data with a channel, and is immediately available
(including in create_buf_file()) via chan->private_data or
buf->chan->private_data.
(传入 relay_open() 的 private_data 允许客户端将用户自定义数据与通道关联起来，并且可以通过 chan->private_data 或 buf->chan->private_data 立即访问(包括在 create_buf_file() 中)。)

Channel 'modes'
(通道"模式")
---------------

relay channels can be used in either of two modes - 'overwrite' or
'no-overwrite'.  The mode is entirely determined by the implementation
of the subbuf_start() callback, as described below.  The default if no
subbuf_start() callback is defined is 'no-overwrite' mode.  If the
default mode suits your needs, and you plan to use the read()
interface to retrieve channel data, you can ignore the details of this
section, as it pertains mainly to mmap() implementations.
(relay 通道可以在两种模式下使用——"覆盖"或"非覆盖"。模式完全由 subbuf_start() 回调的实现决定，如下所述。未定义 subbuf_start() 回调时的默认模式是"非覆盖"模式。如果默认模式符合你的需求，并且你打算使用 read() 接口获取通道数据，你可以忽略本节的细节，因为它主要与 mmap() 实现有关。)

In 'overwrite' mode, also known as 'flight recorder' mode, writes
continuously cycle around the buffer and will never fail, but will
unconditionally overwrite old data regardless of whether it's actually
been consumed.  In no-overwrite mode, writes will fail, i.e., data will
be lost, if the number of unconsumed sub-buffers equals the total
number of sub-buffers in the channel.  It should be clear that if
there is no consumer or if the consumer can't consume sub-buffers fast
enough, data will be lost in either case; the only difference is
whether data is lost from the beginning or the end of a buffer.
("覆盖"模式也称为"飞行记录仪"模式，写入围绕缓冲区不断循环，永不失败，但会无条件覆盖旧数据，而不管它是否已被消费。在非覆盖模式下，如果未消费的 sub-buffer 数量等于通道中 sub-buffer 的总数，写入会失败，即数据会丢失。显然，如果没有消费者，或者消费者无法足够快地消费 sub-buffer，两种情况下数据都会丢失；唯一的区别是数据是从缓冲区的头部还是尾部丢失。)

As explained above, a relay channel is made of up one or more
per-cpu channel buffers, each implemented as a circular buffer
subdivided into one or more sub-buffers.  Messages are written into
the current sub-buffer of the channel's current per-cpu buffer via the
write functions described below.  Whenever a message can't fit into
the current sub-buffer, because there's no room left for it, the
client is notified via the subbuf_start() callback that a switch to a
new sub-buffer is about to occur.  The client uses this callback to 1)
initialize the next sub-buffer if appropriate 2) finalize the previous
sub-buffer if appropriate and 3) return a boolean value indicating
whether or not to actually move on to the next sub-buffer.
(如上所述，relay 通道由一个或多个每 CPU 通道缓冲区组成，每个缓冲区实现为一个循环缓冲区，再细分为一个或多个 sub-buffer。消息通过下文描述的写函数写入通道当前每 CPU 缓冲区的当前 sub-buffer。每当消息因为当前 sub-buffer 没有剩余空间而装不下时，客户端会通过 subbuf_start() 回调收到通知：即将切换到新的 sub-buffer。客户端利用此回调来 1)在适当的情况下初始化下一个 sub-buffer 2)在适当的情况下定稿前一个 sub-buffer 3)返回一个布尔值，指示是否真的前进到下一个 sub-buffer。)

To implement 'no-overwrite' mode, the userspace client provides
an implementation of the subbuf_start() callback something like the
following::

    static int subbuf_start(struct rchan_buf *buf,
			    void *subbuf,
			    void *prev_subbuf,
			    unsigned int prev_padding)
    {
	    if (prev_subbuf)
		    *((unsigned *)prev_subbuf) = prev_padding;

	    if (relay_buf_full(buf))
		    return 0;

	    subbuf_start_reserve(buf, sizeof(unsigned int));

	    return 1;
    }

(要实现"非覆盖"模式，用户空间客户端提供的 subbuf_start() 回调实现大致如下:)

If the current buffer is full, i.e., all sub-buffers remain unconsumed,
the callback returns 0 to indicate that the buffer switch should not
occur yet, i.e., until the consumer has had a chance to read the
current set of ready sub-buffers.  For the relay_buf_full() function
to make sense, the consumer is responsible for notifying the relay
interface when sub-buffers have been consumed via
relay_subbufs_consumed().  Any subsequent attempts to write into the
buffer will again invoke the subbuf_start() callback with the same
parameters; only when the consumer has consumed one or more of the
ready sub-buffers will relay_buf_full() return 0, in which case the
buffer switch can continue.
(如果当前缓冲区已满，即所有 sub-buffer 都未被消费，回调返回 0 以指示此时还不应进行缓冲区切换，即直到消费者有机会读取当前这批就绪的 sub-buffer 为止。要让 relay_buf_full() 函数有意义，消费者负责在 sub-buffer 被消费时通过 relay_subbufs_consumed() 通知 relay 接口。任何后续向缓冲区写入的尝试都会再次以相同参数调用 subbuf_start() 回调；只有当消费者已消费一个或多个就绪的 sub-buffer 后，relay_buf_full() 才会返回 0，此时缓冲区切换才能继续。)

The implementation of the subbuf_start() callback for 'overwrite' mode
would be very similar::

    static int subbuf_start(struct rchan_buf *buf,
			    void *subbuf,
			    void *prev_subbuf,
			    size_t prev_padding)
    {
	    if (prev_subbuf)
		    *((unsigned *)prev_subbuf) = prev_padding;

	    subbuf_start_reserve(buf, sizeof(unsigned int));

	    return 1;
    }

("覆盖"模式的 subbuf_start() 回调实现会非常相似:)

In this case, the relay_buf_full() check is meaningless and the
callback always returns 1, causing the buffer switch to occur
unconditionally.  It's also meaningless for the client to use the
relay_subbufs_consumed() function in this mode, as it's never
consulted.
(在这种情况下，relay_buf_full() 检查毫无意义，回调总是返回 1，导致缓冲区切换无条件发生。在此模式下客户端使用 relay_subbufs_consumed() 函数也没有意义，因为它永远不会被查询。)

The default subbuf_start() implementation, used if the client doesn't
define any callbacks, or doesn't define the subbuf_start() callback,
implements the simplest possible 'no-overwrite' mode, i.e., it does
nothing but return 0.
(默认的 subbuf_start() 实现在客户端未定义任何回调、或未定义 subbuf_start() 回调时使用，它实现最简单的"非覆盖"模式，即什么都不做，只返回 0。)

Header information can be reserved at the beginning of each sub-buffer
by calling the subbuf_start_reserve() helper function from within the
subbuf_start() callback.  This reserved area can be used to store
whatever information the client wants.  In the example above, room is
reserved in each sub-buffer to store the padding count for that
sub-buffer.  This is filled in for the previous sub-buffer in the
subbuf_start() implementation; the padding value for the previous
sub-buffer is passed into the subbuf_start() callback along with a
pointer to the previous sub-buffer, since the padding value isn't
known until a sub-buffer is filled.  The subbuf_start() callback is
also called for the first sub-buffer when the channel is opened, to
give the client a chance to reserve space in it.  In this case the
previous sub-buffer pointer passed into the callback will be NULL, so
the client should check the value of the prev_subbuf pointer before
writing into the previous sub-buffer.
(可以在每个 sub-buffer 的开头通过在 subbuf_start() 回调内调用 subbuf_start_reserve() 辅助函数来保留头部信息。这块保留区可用于存储客户端想要的任何信息。在上面的示例中，每个 sub-buffer 中都保留了空间来存储该 sub-buffer 的填充计数。填充值是在 subbuf_start() 实现中为前一个 sub-buffer 填写的；前一个 sub-buffer 的填充值连同指向前一个 sub-buffer 的指针一起传入 subbuf_start() 回调，因为填充值直到 sub-buffer 被填满才可知。通道打开时也会对第一个 sub-buffer 调用 subbuf_start() 回调，给客户端一个在其中保留空间的机会。此时传入回调的前一个 sub-buffer 指针将为 NULL，因此客户端在写入前一个 sub-buffer 之前应检查 prev_subbuf 指针的值。)

Writing to a channel
(向通道写入)
--------------------

Kernel clients write data into the current cpu's channel buffer using
relay_write() or __relay_write().  relay_write() is the main logging
function - it uses local_irqsave() to protect the buffer and should be
used if you might be logging from interrupt context.  If you know
you'll never be logging from interrupt context, you can use
__relay_write(), which only disables preemption.  These functions
don't return a value, so you can't determine whether or not they
failed - the assumption is that you wouldn't want to check a return
value in the fast logging path anyway, and that they'll always succeed
unless the buffer is full and no-overwrite mode is being used, in
which case you can detect a failed write in the subbuf_start()
callback by calling the relay_buf_full() helper function.
(内核客户端使用 relay_write() 或 __relay_write() 将数据写入当前 CPU 的通道缓冲区。relay_write() 是主要的日志函数——它使用 local_irqsave() 保护缓冲区，如果你可能在中断上下文中记录日志，应使用它。如果你确定绝不会在中断上下文中记录日志，可以使用只禁用抢占的 __relay_write()。这些函数不返回值，因此你无法判断它们是否失败——其假设是无论如何你都不会想在快速日志路径中检查返回值，并且它们总会成功，除非缓冲区已满且正在使用非覆盖模式；在这种情况下，你可以通过调用 relay_buf_full() 辅助函数在 subbuf_start() 回调中检测写入失败。)

relay_reserve() is used to reserve a slot in a channel buffer which
can be written to later.  This would typically be used in applications
that need to write directly into a channel buffer without having to
stage data in a temporary buffer beforehand.  Because the actual write
may not happen immediately after the slot is reserved, applications
using relay_reserve() can keep a count of the number of bytes actually
written, either in space reserved in the sub-buffers themselves or as
a separate array.  See the 'reserve' example in the relay-apps tarball
at http://relayfs.sourceforge.net for an example of how this can be
done.  Because the write is under control of the client and is
separated from the reserve, relay_reserve() doesn't protect the buffer
at all - it's up to the client to provide the appropriate
synchronization when using relay_reserve().
(relay_reserve() 用于在通道缓冲区中保留一个稍后可写入的槽位。这通常用于需要直接写入通道缓冲区、而不必事先把数据暂存在临时缓冲区中的应用程序。由于实际写入可能不会在保留槽位后立即发生，使用 relay_reserve() 的应用程序可以记录实际写入的字节数，保存在 sub-buffer 本身保留的空间中，或作为一个单独的数组。关于如何做到这一点，参见 relay-apps 压缩包(http://relayfs.sourceforge.net)中的"reserve"示例。由于写入由客户端控制并与保留相分离，relay_reserve() 完全不保护缓冲区——使用 relay_reserve() 时，由客户端提供适当的同步。)

Closing a channel
(关闭通道)
-----------------

The client calls relay_close() when it's finished using the channel.
The channel and its associated buffers are destroyed when there are no
longer any references to any of the channel buffers.  relay_flush()
forces a sub-buffer switch on all the channel buffers, and can be used
to finalize and process the last sub-buffers before the channel is
closed.
(客户端在使用完通道后调用 relay_close()。当不再有任何对通道缓冲区的引用时，通道及其关联的缓冲区被销毁。relay_flush() 强制在所有通道缓冲区上进行一次 sub-buffer 切换，可用于在通道关闭前定稿并处理最后的 sub-buffer。)

Misc
(杂项)
----

Some applications may want to keep a channel around and reuse it
rather than open and close a new channel for each use.  relay_reset()
can be used for this purpose - it resets a channel to its initial
state without reallocating channel buffer memory or destroying
existing mappings.  It should however only be called when it's safe to
do so, i.e., when the channel isn't currently being written to.
(有些应用程序可能希望保留一个通道并重复使用它，而不是为每次使用都打开并关闭一个新通道。relay_reset() 可用于此目的——它将通道重置为其初始状态，而不重新分配通道缓冲区内存或销毁现有映射。但它只应在安全的时候调用，即通道当前没有被写入时。)

Finally, there are a couple of utility callbacks that can be used for
different purposes.  buf_mapped() is called whenever a channel buffer
is mmapped from user space and buf_unmapped() is called when it's
unmapped.  The client can use this notification to trigger actions
within the kernel application, such as enabling/disabling logging to
the channel.
(最后，还有几个可用于不同用途的工具回调。每当通道缓冲区从用户空间被 mmap 时会调用 buf_mapped()，取消映射时调用 buf_unmapped()。客户端可以利用这一通知在内核应用程序内触发动作，例如启用/禁用向通道记录日志。)


Resources
(资源)
=========

For news, example code, mailing list, etc. see the relay interface homepage:
(新闻、示例代码、邮件列表等请参见 relay 接口主页:)

    http://relayfs.sourceforge.net


Credits
(致谢)
=======

The ideas and specs for the relay interface came about as a result of
discussions on tracing involving the following:
(relay 接口的思想和规格源自以下关于跟踪的讨论:)

Michel Dagenais		<michel.dagenais@polymtl.ca>
Richard Moore		<richardj_moore@uk.ibm.com>
Bob Wisniewski		<bob@watson.ibm.com>
Karim Yaghmour		<karim@opersys.com>
Tom Zanussi		<zanussi@us.ibm.com>

Also thanks to Hubertus Franke for a lot of useful suggestions and bug
reports.
(另外感谢 Hubertus Franke 提出的许多有用建议和错误报告。)
