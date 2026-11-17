.. SPDX-License-Identifier: GPL-2.0

================================================
Stream Parser (strparser)(流解析器（strparser）)
================================================

Introduction(简介)
==================

The stream parser (strparser) is a utility that parses messages of an
application layer protocol running over a data stream. The stream
parser works in conjunction with an upper layer in the kernel to provide
kernel support for application layer messages. For instance, Kernel
Connection Multiplexor (KCM) uses the Stream Parser to parse messages
using a BPF program.
(流解析器 (strparser) 是一个实用工具，用于解析运行在数据流之上的应用层协议的消息。流解析器与内核中的上层协同工作，为应用层消息提供内核支持。例如，内核连接多路复用器 (KCM) 使用流解析器通过 BPF 程序来解析消息。)

The strparser works in one of two modes: receive callback or general
mode.
(strparser 以两种模式之一工作：接收回调模式或一般模式。)

In receive callback mode, the strparser is called from the data_ready
callback of a TCP socket. Messages are parsed and delivered as they are
received on the socket.
(在接收回调模式下，strparser 从 TCP 套接字的 data_ready 回调中被调用。消息在套接字上被接收时即得到解析并投递。)

In general mode, a sequence of skbs are fed to strparser from an
outside source. Message are parsed and delivered as the sequence is
processed. This modes allows strparser to be applied to arbitrary
streams of data.
(在一般模式下，一系列 skb 由外部来源馈送给 strparser。消息在该序列被处理的过程中得到解析并投递。此模式使 strparser 能够应用于任意的数据流。)

Interface(接口)
===============

The API includes a context structure, a set of callbacks, utility
functions, and a data_ready function for receive callback mode. The
callbacks include a parse_msg function that is called to perform
parsing (e.g.  BPF parsing in case of KCM), and a rcv_msg function
that is called when a full message has been completed.
(该 API 包括一个上下文结构体、一组回调、若干实用函数，以及用于接收回调模式的 data_ready 函数。回调包括一个 parse_msg 函数（用于执行解析，例如在 KCM 中是 BPF 解析），以及一个 rcv_msg 函数（在一条完整的消息完成时被调用）。)

Functions(函数)
===============

     ::

	strp_init(struct strparser *strp, struct sock *sk,
		const struct strp_callbacks *cb)

     Called to initialize a stream parser. strp is a struct of type
     strparser that is allocated by the upper layer. sk is the TCP
     socket associated with the stream parser for use with receive
     callback mode; in general mode this is set to NULL. Callbacks
     are called by the stream parser (the callbacks are listed below).
     (strp_init 用于初始化一个流解析器。strp 是由上层分配的 strparser
     类型结构体。sk 是与流解析器关联的 TCP 套接字，供接收回调模式使用；
     在一般模式下它被设为 NULL。回调由流解析器调用（回调在下面列出）。)

     ::

	void strp_pause(struct strparser *strp)

     Temporarily pause a stream parser. Message parsing is suspended
     and no new messages are delivered to the upper layer.
     (暂时暂停一个流解析器。消息解析被挂起，不再有新消息投递给上层。)

     ::

	void strp_unpause(struct strparser *strp)

     Unpause a paused stream parser.
     (恢复一个已暂停的流解析器。)

     ::

	void strp_stop(struct strparser *strp);

     strp_stop is called to completely stop stream parser operations.
     This is called internally when the stream parser encounters an
     error, and it is called from the upper layer to stop parsing
     operations.
     (strp_stop 用于完全停止流解析器的操作。当流解析器遇到错误时会在内部
     调用它，上层也会调用它来停止解析操作。)

     ::

	void strp_done(struct strparser *strp);

     strp_done is called to release any resources held by the stream
     parser instance. This must be called after the stream processor
     has been stopped.
     (strp_done 用于释放流解析器实例持有的所有资源。必须在流处理器停止
     之后调用。)

     ::

	int strp_process(struct strparser *strp, struct sk_buff *orig_skb,
			 unsigned int orig_offset, size_t orig_len,
			 size_t max_msg_size, long timeo)

    strp_process is called in general mode for a stream parser to
    parse an sk_buff. The number of bytes processed or a negative
    error number is returned. Note that strp_process does not
    consume the sk_buff. max_msg_size is maximum size the stream
    parser will parse. timeo is timeout for completing a message.
    (在一般模式下调用 strp_process 让流解析器解析一个 sk_buff。返回已处理
    的字节数或一个负的错误号。注意 strp_process 不会消费该 sk_buff。
    max_msg_size 是流解析器将解析的最大尺寸。timeo 是完成一条消息的
    超时时间。)

    ::

	void strp_data_ready(struct strparser *strp);

    The upper layer calls strp_tcp_data_ready when data is ready on
    the lower socket for strparser to process. This should be called
    from a data_ready callback that is set on the socket. Note that
    maximum messages size is the limit of the receive socket
    buffer and message timeout is the receive timeout for the socket.
    (当下层套接字上有数据就绪、可供 strparser 处理时，上层调用
    strp_tcp_data_ready。应当在套接字上设置的 data_ready 回调中调用它。
    注意，最大消息尺寸即为接收套接字缓冲区的上限，而消息超时即该套接字
    的接收超时。)

    ::

	void strp_check_rcv(struct strparser *strp);

    strp_check_rcv is called to check for new messages on the socket.
    This is normally called at initialization of a stream parser
    instance or after strp_unpause.
    (strp_check_rcv 用于检查套接字上的新消息。它通常在流解析器实例初始
    化时或在 strp_unpause 之后调用。)

Callbacks(回调)
===============

There are seven callbacks:
(共有七个回调：)

    ::

	int (*parse_msg)(struct strparser *strp, struct sk_buff *skb);

    parse_msg is called to determine the length of the next message
    in the stream. The upper layer must implement this function. It
    should parse the sk_buff as containing the headers for the
    next application layer message in the stream.
    (parse_msg 用于确定流中下一条消息的长度。上层必须实现此函数。它应当
    把 sk_buff 解析为包含流中下一条应用层消息的报头。)

    The skb->cb in the input skb is a struct strp_msg. Only
    the offset field is relevant in parse_msg and gives the offset
    where the message starts in the skb.
    (输入 skb 中的 skb->cb 是一个 struct strp_msg。在 parse_msg 中只有
    offset 字段有意义，它给出消息在 skb 中开始的偏移量。)

    The return values of this function are:
    (此函数的返回值如下：)

    =========    ===========================================================
    >0           indicates length of successfully parsed message (大于 0 表示成功解析的消息的长度)
    0            indicates more data must be received to parse the message (0 表示需要接收更多数据才能解析该消息)
    -ESTRPIPE    current message should not be processed by the
		 kernel, return control of the socket to userspace which
		 can proceed to read the messages itself (-ESTRPIPE 表示当前消息不应由内核处理：把套接字的控制权交还给用户空间，由用户空间自行读取消息)
    other < 0    Error in parsing, give control back to userspace
		 assuming that synchronization is lost and the stream
		 is unrecoverable (application expected to close TCP socket) (解析出错：把控制权交还给用户空间，认为同步已丢失且流不可恢复（应用程序应关闭 TCP 套接字）)
    =========    ===========================================================

    In the case that an error is returned (return value is less than
    zero) and the parser is in receive callback mode, then it will set
    the error on TCP socket and wake it up. If parse_msg returned
    -ESTRPIPE and the stream parser had previously read some bytes for
    the current message, then the error set on the attached socket is
    ENODATA since the stream is unrecoverable in that case.
    (如果返回了错误（返回值小于零）且解析器处于接收回调模式，则会在 TCP
    套接字上设置该错误并将其唤醒。如果 parse_msg 返回了 -ESTRPIPE，且流
    解析器此前已为当前消息读取了一些字节，那么由于此时流不可恢复，在所
    关联套接字上设置的错误是 ENODATA。)

    ::

	void (*lock)(struct strparser *strp)

    The lock callback is called to lock the strp structure when
    the strparser is performing an asynchronous operation (such as
    processing a timeout). In receive callback mode the default
    function is to lock_sock for the associated socket. In general
    mode the callback must be set appropriately.
    (lock 回调用于在流解析器执行异步操作（例如处理超时）时锁定 strp
    结构。在接收回调模式下，默认函数是对所关联的套接字执行 lock_sock。
    在一般模式下，必须适当地设置该回调。)

    ::

	void (*unlock)(struct strparser *strp)

    The unlock callback is called to release the lock obtained
    by the lock callback. In receive callback mode the default
    function is release_sock for the associated socket. In general
    mode the callback must be set appropriately.
    (unlock 回调用于释放由 lock 回调获取的锁。在接收回调模式下，默认
    函数是对所关联的套接字执行 release_sock。在一般模式下，必须适当地
    设置该回调。)

    ::

	void (*rcv_msg)(struct strparser *strp, struct sk_buff *skb);

    rcv_msg is called when a full message has been received and
    is queued. The callee must consume the sk_buff; it can
    call strp_pause to prevent any further messages from being
    received in rcv_msg (see strp_pause above). This callback
    must be set.
    (当一条完整的消息已被接收并进入队列时，会调用 rcv_msg。被调用方必须
    消费该 sk_buff；它可以调用 strp_pause 来防止后续消息再在 rcv_msg 中
    被接收（见上文 strp_pause）。必须设置此回调。)

    The skb->cb in the input skb is a struct strp_msg. This
    struct contains two fields: offset and full_len. Offset is
    where the message starts in the skb, and full_len is the
    the length of the message. skb->len - offset may be greater
    than full_len since strparser does not trim the skb.
    (输入 skb 中的 skb->cb 是一个 struct strp_msg。该结构体包含两个字段：
    offset 和 full_len。offset 是消息在 skb 中开始的偏移量，full_len 是
    消息的长度。skb->len - offset 可能大于 full_len，因为 strparser 不会
    裁剪 skb。)

    ::

	int (*read_sock)(struct strparser *strp, read_descriptor_t *desc,
                     sk_read_actor_t recv_actor);

    The read_sock callback is used by strparser instead of
    sock->ops->read_sock, if provided.
    (如果提供了 read_sock 回调，strparser 将用它来代替
    sock->ops->read_sock。)
    ::

	int (*read_sock_done)(struct strparser *strp, int err);

     read_sock_done is called when the stream parser is done reading
     the TCP socket in receive callback mode. The stream parser may
     read multiple messages in a loop and this function allows cleanup
     to occur when exiting the loop. If the callback is not set (NULL
     in strp_init) a default function is used.
     (read_sock_done 在接收回调模式下流解析器完成对 TCP 套接字的读取时
     被调用。流解析器可能在循环中读取多条消息，此函数允许在退出循环时
     进行清理。如果未设置该回调（strp_init 中为 NULL），则使用默认函数。)

     ::

	void (*abort_parser)(struct strparser *strp, int err);

     This function is called when stream parser encounters an error
     in parsing. The default function stops the stream parser and
     sets the error in the socket if the parser is in receive callback
     mode. The default function can be changed by setting the callback
     to non-NULL in strp_init.
     (当流解析器在解析中遇到错误时会调用此函数。默认函数会停止流解析器，
     并且如果解析器处于接收回调模式，还会在套接字上设置该错误。通过在
     strp_init 中把该回调设置为非 NULL，可以更改默认函数。)

Statistics(统计)
================

Various counters are kept for each stream parser instance. These are in
the strp_stats structure. strp_aggr_stats is a convenience structure for
accumulating statistics for multiple stream parser instances.
save_strp_stats and aggregate_strp_stats are helper functions to save
and aggregate statistics.
(每个流解析器实例都维护着各种计数器。这些计数器保存在 strp_stats 结构中。strp_aggr_stats 是用于累计多个流解析器实例统计信息的便捷结构。save_strp_stats 和 aggregate_strp_stats 是保存和汇总统计信息的辅助函数。)

Message assembly limits(消息组装限制)
=====================================

The stream parser provide mechanisms to limit the resources consumed by
message assembly.
(流解析器提供了一些机制来限制消息组装所消耗的资源。)

A timer is set when assembly starts for a new message. In receive
callback mode the message timeout is taken from rcvtime for the
associated TCP socket. In general mode, the timeout is passed as an
argument in strp_process. If the timer fires before assembly completes
the stream parser is aborted and the ETIMEDOUT error is set on the TCP
socket if in receive callback mode.
(当开始组装一条新消息时会设置一个定时器。在接收回调模式下，消息超时取自所关联 TCP 套接字的 rcvtime。在一般模式下，超时作为参数传入 strp_process。如果定时器在组装完成之前触发，流解析器将被中止，并且在接收回调模式下会在 TCP 套接字上设置 ETIMEDOUT 错误。)

In receive callback mode, message length is limited to the receive
buffer size of the associated TCP socket. If the length returned by
parse_msg is greater than the socket buffer size then the stream parser
is aborted with EMSGSIZE error set on the TCP socket. Note that this
makes the maximum size of receive skbuffs for a socket with a stream
parser to be 2*sk_rcvbuf of the TCP socket.
(在接收回调模式下，消息长度被限制为所关联 TCP 套接字的接收缓冲区大小。如果 parse_msg 返回的长度大于套接字缓冲区大小，流解析器将被中止，并在 TCP 套接字上设置 EMSGSIZE 错误。注意，这使得带有流解析器的套接字其接收 skbuff 的最大尺寸为 TCP 套接字的 2*sk_rcvbuf。)

In general mode the message length limit is passed in as an argument
to strp_process.
(在一般模式下，消息长度上限作为参数传入 strp_process。)

Author(作者)
============

Tom Herbert (tom@quantonium.net)
(Tom Herbert (tom@quantonium.net))
