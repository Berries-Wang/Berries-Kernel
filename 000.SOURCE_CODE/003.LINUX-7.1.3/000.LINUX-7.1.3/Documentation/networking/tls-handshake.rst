.. SPDX-License-Identifier: GPL-2.0

========================================
In-Kernel TLS Handshake(内核内 TLS 握手)
========================================

Overview(概述)
==============

Transport Layer Security (TLS) is an Upper Layer Protocol (ULP) that runs
over TCP. TLS provides end-to-end data integrity and confidentiality in
addition to peer authentication.
(传输层安全 (TLS) 是一种运行在 TCP 之上的上层协议 (ULP)。除了对端认证之外，TLS 还提供端到端的数据完整性与机密性。)

The kernel's kTLS implementation handles the TLS record subprotocol, but
does not handle the TLS handshake subprotocol which is used to establish
a TLS session. Kernel consumers can use the API described here to
request TLS session establishment.
(内核的 kTLS 实现负责处理 TLS 记录子协议，但不处理用于建立 TLS 会话的 TLS 握手子协议。内核消费者可以使用这里描述的 API 来请求建立 TLS 会话。)

There are several possible ways to provide a handshake service in the
kernel. The API described here is designed to hide the details of those
implementations so that in-kernel TLS consumers do not need to be
aware of how the handshake gets done.
(在内核中提供握手服务有多种可能的方式。这里描述的 API 旨在隐藏这些实现的细节，使内核内的 TLS 消费者无需关心握手是如何完成的。)


User handshake agent(用户态握手代理)
====================================

As of this writing, there is no TLS handshake implementation in the
Linux kernel. To provide a handshake service, a handshake agent
(typically in user space) is started in each network namespace where a
kernel consumer might require a TLS handshake. Handshake agents listen
for events sent from the kernel that indicate a handshake request is
waiting.
(截至撰写本文时，Linux 内核中尚无 TLS 握手实现。为了提供握手服务，在每个可能需要 TLS 握手的内核消费者所在
的网络命名空间中，都会启动一个握手代理（通常位于用户空间）。握手代理监听内核发出的表明有握手请求正在
等待处理的事件。)

An open socket is passed to a handshake agent via a netlink operation,
which creates a socket descriptor in the agent's file descriptor table.
If the handshake completes successfully, the handshake agent promotes
the socket to use the TLS ULP and sets the session information using the
SOL_TLS socket options. The handshake agent returns the socket to the
kernel via a second netlink operation.
(打开的套接字通过一次 netlink 操作传递给握手代理，该操作会在代理的文件描述符表中创建一个套接字描述符。
如果握手成功完成，握手代理会将套接字提升为使用 TLS ULP，并通过 SOL_TLS 套接字选项设置会话信息。随后握
手代理通过第二次 netlink 操作将该套接字交还给内核。)


Kernel Handshake API(内核握手 API)
==================================

A kernel TLS consumer initiates a client-side TLS handshake on an open
socket by invoking one of the tls_client_hello() functions. First, it
fills in a structure that contains the parameters of the request:
(内核 TLS 消费者通过调用 tls_client_hello() 系列函数之一，在一个已打开的套接字上发起客户端侧的 TLS 握手。首先，
它会填写一个包含请求参数的结构体：)

.. code-block:: c

  struct tls_handshake_args {
        struct socket   *ta_sock;
        tls_done_func_t ta_done;
        void            *ta_data;
        const char      *ta_peername;
        unsigned int    ta_timeout_ms;
        key_serial_t    ta_keyring;
        key_serial_t    ta_my_cert;
        key_serial_t    ta_my_privkey;
        unsigned int    ta_num_peerids;
        key_serial_t    ta_my_peerids[5];
  };

The @ta_sock field references an open and connected socket. The consumer
must hold a reference on the socket to prevent it from being destroyed
while the handshake is in progress. The consumer must also have
instantiated a struct file in sock->file.
(@ta_sock 字段引用一个已打开且已连接的套接字。消费者必须持有该套接字的引用，以防止握手进行期间它被销毁。
消费者还必须在 sock->file 中实例化一个 struct file。)


@ta_done contains a callback function that is invoked when the handshake
has completed. Further explanation of this function is in the "Handshake
Completion" sesction below.
(@ta_done 包含一个回调函数，在握手完成时被调用。关于该函数的进一步说明见下文"握手完成"一节。)

The consumer can provide a NUL-terminated hostname in the @ta_peername
field that is sent as part of ClientHello. If no peername is provided,
the DNS hostname associated with the server's IP address is used instead.
(消费者可以在 @ta_peername 字段中提供一个以 NUL 结尾的主机名，它将作为 ClientHello 的一部分发送。如果未提供
对端名称，则使用与服务器 IP 地址关联的 DNS 主机名。)

The consumer can fill in the @ta_timeout_ms field to force the servicing
handshake agent to exit after a number of milliseconds. This enables the
socket to be fully closed once both the kernel and the handshake agent
have closed their endpoints.
(消费者可以填写 @ta_timeout_ms 字段，强制服务的握手代理在若干毫秒后退出。这样，一旦内核和握手代理都关闭了
各自的端点，套接字就能被完全关闭。)

Authentication material such as x.509 certificates, private certificate
keys, and pre-shared keys are provided to the handshake agent in keys
that are instantiated by the consumer before making the handshake
request. The consumer can provide a private keyring that is linked into
the handshake agent's process keyring in the @ta_keyring field to prevent
access of those keys by other subsystems.
(x.509 证书、证书私钥和预共享密钥等认证材料，由消费者在发起握手请求之前实例化为密钥 (key) 并提供给握手代理。
消费者可以在 @ta_keyring 字段中提供一个私有密钥环 (keyring)，它会被链接到握手代理的进程密钥环中，以防止其他
子系统访问这些密钥。)

To request an x.509-authenticated TLS session, the consumer fills in
the @ta_my_cert and @ta_my_privkey fields with the serial numbers of
keys containing an x.509 certificate and the private key for that
certificate. Then, it invokes this function:
(要请求经 x.509 认证的 TLS 会话，消费者在 @ta_my_cert 和 @ta_my_privkey 字段中填入密钥的序列号，这些密钥分别
包含 x.509 证书和该证书的私钥。然后，它调用以下函数：)

.. code-block:: c

  ret = tls_client_hello_x509(args, gfp_flags);

The function returns zero when the handshake request is under way. A
zero return guarantees the callback function @ta_done will be invoked
for this socket. The function returns a negative errno if the handshake
could not be started. A negative errno guarantees the callback function
@ta_done will not be invoked on this socket.
(当握手请求已经开始进行时，该函数返回零。零返回值保证回调函数 @ta_done 一定会为此套接字被调用。如果握手无
法启动，该函数返回负的 errno。负 errno 保证回调函数 @ta_done 不会在此套接字上被调用。)


To initiate a client-side TLS handshake with a pre-shared key, use:
(要使用预共享密钥发起客户端侧 TLS 握手，请使用：)

.. code-block:: c

  ret = tls_client_hello_psk(args, gfp_flags);

However, in this case, the consumer fills in the @ta_my_peerids array
with serial numbers of keys containing the peer identities it wishes
to offer, and the @ta_num_peerids field with the number of array
entries it has filled in. The other fields are filled in as above.
(不过在这种情况下，消费者在 @ta_my_peerids 数组中填入包含其希望提供的对端身份的密钥的序列号，并在
@ta_num_peerids 字段中填入已填写的数组条目数。其余字段的填写方式与上文相同。)


To initiate an anonymous client-side TLS handshake use:
(要发起匿名的客户端侧 TLS 握手，请使用：)

.. code-block:: c

  ret = tls_client_hello_anon(args, gfp_flags);

The handshake agent presents no peer identity information to the remote
during this type of handshake. Only server authentication (ie the client
verifies the server's identity) is performed during the handshake. Thus
the established session uses encryption only.
(在这种类型的握手期间，握手代理不会向远端提供任何对端身份信息。握手过程中只执行服务器认证（即客户端验证服
务器的身份）。因此建立的会话仅使用加密。)


Consumers that are in-kernel servers use:
(作为内核内服务器的消费者使用：)

.. code-block:: c

  ret = tls_server_hello_x509(args, gfp_flags);

or

.. code-block:: c

  ret = tls_server_hello_psk(args, gfp_flags);

The argument structure is filled in as above.
(参数结构的填写方式与上文相同。)


If the consumer needs to cancel the handshake request, say, due to a ^C
or other exigent event, the consumer can invoke:
(如果消费者需要取消握手请求，比如由于收到 ^C 或其他紧急事件，可以调用：)

.. code-block:: c

  bool tls_handshake_cancel(sock);

This function returns true if the handshake request associated with
@sock has been canceled. The consumer's handshake completion callback
will not be invoked. If this function returns false, then the consumer's
completion callback has already been invoked.
(如果与 @sock 关联的握手请求已被取消，该函数返回 true。此时消费者的握手完成回调不会被调用。如果该函数返回
false，则说明消费者的完成回调已经被调用过了。)


Handshake Completion(握手完成)
==============================

When the handshake agent has completed processing, it notifies the
kernel that the socket may be used by the consumer again. At this point,
the consumer's handshake completion callback, provided in the @ta_done
field in the tls_handshake_args structure, is invoked.
(握手代理处理完毕后，会通知内核该套接字可以再次被消费者使用。此时，在 tls_handshake_args 结构体 @ta_done 字
段中提供的消费者握手完成回调被调用。)

The synopsis of this function is:
(该函数的原型为：)

.. code-block:: c

  typedef void	(*tls_done_func_t)(void *data, int status,
                                   key_serial_t peerid);

The consumer provides a cookie in the @ta_data field of the
tls_handshake_args structure that is returned in the @data parameter of
this callback. The consumer uses the cookie to match the callback to the
thread waiting for the handshake to complete.
(消费者在 tls_handshake_args 结构体的 @ta_data 字段中提供一个"曲奇" (cookie)，它会通过该回调的 @data 参数返回。
消费者利用这个 cookie 把回调与等待握手完成的线程关联起来。)

The success status of the handshake is returned via the @status
parameter:
(握手的成功状态通过 @status 参数返回：)

+------------+----------------------------------------------+
|  status    |  meaning                                     |
|  (状态)    |  (含义)                                      |
+============+==============================================+
|  0         |  TLS session established successfully        |
|            |  (TLS 会话成功建立)                          |
+------------+----------------------------------------------+
|  -EACCESS  |  Remote peer rejected the handshake or       |
|            |  authentication failed                       |
|            |  (远端对端拒绝握手，或认证失败)              |
+------------+----------------------------------------------+
|  -ENOMEM   |  Temporary resource allocation failure       |
|            |  (临时性资源分配失败)                        |
+------------+----------------------------------------------+
|  -EINVAL   |  Consumer provided an invalid argument       |
|            |  (消费者提供了无效参数)                      |
+------------+----------------------------------------------+
|  -ENOKEY   |  Missing authentication material             |
|            |  (缺少认证材料)                              |
+------------+----------------------------------------------+
|  -EIO      |  An unexpected fault occurred                |
|            |  (发生了意外错误)                            |
+------------+----------------------------------------------+

The @peerid parameter contains the serial number of a key containing the
remote peer's identity or the value TLS_NO_PEERID if the session is not
authenticated.
(@peerid 参数包含一个密钥的序列号，该密钥保存着远端对端的身份；如果会话未经认证，则为值 TLS_NO_PEERID。)

A best practice is to close and destroy the socket immediately if the
handshake failed.
(最佳实践是：如果握手失败，立即关闭并销毁该套接字。)


Other considerations(其他注意事项)
----------------------------------

While a handshake is under way, the kernel consumer must alter the
socket's sk_data_ready callback function to ignore all incoming data.
Once the handshake completion callback function has been invoked, normal
receive operation can be resumed.
(在握手进行期间，内核消费者必须修改套接字的 sk_data_ready 回调函数，使其忽略所有传入数据。一旦握手完成回调函
数被调用，就可以恢复正常接收操作。)

Once a TLS session is established, the consumer must provide a buffer
for and then examine the control message (CMSG) that is part of every
subsequent sock_recvmsg(). Each control message indicates whether the
received message data is TLS record data or session metadata.
(TLS 会话建立之后，消费者必须为后续每次 sock_recvmsg() 中包含的控制消息 (CMSG) 提供缓冲区，然后对其进行检查。
每条控制消息指明所接收的消息数据是 TLS 记录数据还是会话元数据。)

See tls.rst for details on how a kTLS consumer recognizes incoming
(decrypted) application data, alerts, and handshake packets once the
socket has been promoted to use the TLS ULP.
(关于套接字被提升为使用 TLS ULP 之后，kTLS 消费者如何识别传入的（已解密的）应用数据、告警和握手数据包，详见
tls.rst。)
