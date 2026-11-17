.. _kernel_tls:

====================
Kernel TLS(内核 TLS)
====================

Overview(总览)
==============

Transport Layer Security (TLS) is a Upper Layer Protocol (ULP) that runs over
TCP. TLS provides end-to-end data integrity and confidentiality.
(传输层安全 (TLS) 是一种运行在 TCP 之上的上层协议 (ULP)。TLS 提供端到端的数据完整性和机密性。)

User interface(用户接口)
========================

Creating a TLS connection(创建 TLS 连接)
----------------------------------------

First create a new TCP socket and once the connection is established set the
TLS ULP.
(首先创建一个新的 TCP 套接字，一旦连接建立就设置 TLS ULP。)

.. code-block:: c

  sock = socket(AF_INET, SOCK_STREAM, 0);
  connect(sock, addr, addrlen);
  setsockopt(sock, SOL_TCP, TCP_ULP, "tls", sizeof("tls"));

Setting the TLS ULP allows us to set/get TLS socket options. Currently
only the symmetric encryption is handled in the kernel.  After the TLS
handshake is complete, we have all the parameters required to move the
data-path to the kernel. There is a separate socket option for moving
the transmit and the receive into the kernel.
(设置 TLS ULP 之后，就可以设置/获取 TLS 套接字选项了。目前内核中只处理对称加密。TLS 握手完成后，我们就拥有了把数据路径移入内核所需的全部参数。发送和接收分别有单独的套接字选项用于移入内核。)

.. code-block:: c

  /* From linux/tls.h */
  struct tls_crypto_info {
          unsigned short version;
          unsigned short cipher_type;
  };

  struct tls12_crypto_info_aes_gcm_128 {
          struct tls_crypto_info info;
          unsigned char iv[TLS_CIPHER_AES_GCM_128_IV_SIZE];
          unsigned char key[TLS_CIPHER_AES_GCM_128_KEY_SIZE];
          unsigned char salt[TLS_CIPHER_AES_GCM_128_SALT_SIZE];
          unsigned char rec_seq[TLS_CIPHER_AES_GCM_128_REC_SEQ_SIZE];
  };


  struct tls12_crypto_info_aes_gcm_128 crypto_info;

  crypto_info.info.version = TLS_1_2_VERSION;
  crypto_info.info.cipher_type = TLS_CIPHER_AES_GCM_128;
  memcpy(crypto_info.iv, iv_write, TLS_CIPHER_AES_GCM_128_IV_SIZE);
  memcpy(crypto_info.rec_seq, seq_number_write,
					TLS_CIPHER_AES_GCM_128_REC_SEQ_SIZE);
  memcpy(crypto_info.key, cipher_key_write, TLS_CIPHER_AES_GCM_128_KEY_SIZE);
  memcpy(crypto_info.salt, implicit_iv_write, TLS_CIPHER_AES_GCM_128_SALT_SIZE);

  setsockopt(sock, SOL_TLS, TLS_TX, &crypto_info, sizeof(crypto_info));

Transmit and receive are set separately, but the setup is the same, using either
TLS_TX or TLS_RX.
(发送和接收分开设置，但设置方法相同，使用 TLS_TX 或 TLS_RX 均可。)

Sending TLS application data(发送 TLS 应用数据)
-----------------------------------------------

After setting the TLS_TX socket option all application data sent over this
socket is encrypted using TLS and the parameters provided in the socket option.
For example, we can send an encrypted hello world record as follows:
(设置 TLS_TX 套接字选项后，通过此套接字发送的所有应用数据都会使用 TLS 以及该套接字选项中提供的参数进行加密。例如，我们可以按如下方式发送一个加密的 hello world 记录：)

.. code-block:: c

  const char *msg = "hello world\n";
  send(sock, msg, strlen(msg));

send() data is directly encrypted from the userspace buffer provided
to the encrypted kernel send buffer if possible.
(如果可能，send() 数据会直接从所提供的用户态缓冲区加密到已加密的内核发送缓冲区。)

The sendfile system call will send the file's data over TLS records of maximum
length (2^14).
(sendfile 系统调用会以最大长度 (2^14) 的 TLS 记录发送文件数据。)

.. code-block:: c

  file = open(filename, O_RDONLY);
  fstat(file, &stat);
  sendfile(sock, file, &offset, stat.st_size);

TLS records are created and sent after each send() call, unless
MSG_MORE is passed.  MSG_MORE will delay creation of a record until
MSG_MORE is not passed, or the maximum record size is reached.
(每次 send() 调用之后都会创建并发送 TLS 记录，除非传入了 MSG_MORE。MSG_MORE 会推迟记录的创建，直到不再传入 MSG_MORE 或达到最大记录大小。)

The kernel will need to allocate a buffer for the encrypted data.
This buffer is allocated at the time send() is called, such that
either the entire send() call will return -ENOMEM (or block waiting
for memory), or the encryption will always succeed.  If send() returns
-ENOMEM and some data was left on the socket buffer from a previous
call using MSG_MORE, the MSG_MORE data is left on the socket buffer.
(内核需要为加密数据分配一个缓冲区。该缓冲区在调用 send() 时分配，这样要么整个 send() 调用返回 -ENOMEM (或阻塞等待内存)，要么加密总是能够成功。如果 send() 返回 -ENOMEM，并且之前使用 MSG_MORE 的调用在套接字缓冲区中留下了一些数据，那么这些 MSG_MORE 数据会继续留在套接字缓冲区中。)

Receiving TLS application data(接收 TLS 应用数据)
-------------------------------------------------

After setting the TLS_RX socket option, all recv family socket calls
are decrypted using TLS parameters provided.  A full TLS record must
be received before decryption can happen.
(设置 TLS_RX 套接字选项后，所有 recv 族套接字调用都会使用所提供的 TLS 参数进行解密。必须先收到完整的 TLS 记录才能进行解密。)

.. code-block:: c

  char buffer[16384];
  recv(sock, buffer, 16384);

Received data is decrypted directly in to the user buffer if it is
large enough, and no additional allocations occur.  If the userspace
buffer is too small, data is decrypted in the kernel and copied to
userspace.
(如果用户缓冲区足够大，收到的数据会直接解密到用户缓冲区中，不会发生额外的分配。如果用户态缓冲区太小，数据会先在内核中解密，然后复制到用户态。)

``EINVAL`` is returned if the TLS version in the received message does not
match the version passed in setsockopt.
(如果收到的消息中的 TLS 版本与 setsockopt 中传入的版本不匹配，则返回 ``EINVAL`` 。)

``EMSGSIZE`` is returned if the received message is too big.
(如果收到的消息太大，则返回 ``EMSGSIZE`` 。)

``EBADMSG`` is returned if decryption failed for any other reason.
(如果解密因任何其他原因失败，则返回 ``EBADMSG`` 。)

Send TLS control messages(发送 TLS 控制消息)
--------------------------------------------

Other than application data, TLS has control messages such as alert
messages (record type 21) and handshake messages (record type 22), etc.
These messages can be sent over the socket by providing the TLS record type
via a CMSG. For example the following function sends @data of @length bytes
using a record of type @record_type.
(除应用数据之外，TLS 还有控制消息，例如告警消息 (记录类型 21) 和握手消息 (记录类型 22) 等。这些消息可以通过 CMSG 提供 TLS 记录类型，经由套接字发送。例如，下面的函数使用 @record_type 类型的记录发送 @length 字节的 @data。)

.. code-block:: c

  /* send TLS control message using record_type */
  static int klts_send_ctrl_message(int sock, unsigned char record_type,
                                    void *data, size_t length)
  {
        struct msghdr msg = {0};
        int cmsg_len = sizeof(record_type);
        struct cmsghdr *cmsg;
        char buf[CMSG_SPACE(cmsg_len)];
        struct iovec msg_iov;   /* Vector of data to send/receive into.  */

        msg.msg_control = buf;
        msg.msg_controllen = sizeof(buf);
        cmsg = CMSG_FIRSTHDR(&msg);
        cmsg->cmsg_level = SOL_TLS;
        cmsg->cmsg_type = TLS_SET_RECORD_TYPE;
        cmsg->cmsg_len = CMSG_LEN(cmsg_len);
        *CMSG_DATA(cmsg) = record_type;
        msg.msg_controllen = cmsg->cmsg_len;

        msg_iov.iov_base = data;
        msg_iov.iov_len = length;
        msg.msg_iov = &msg_iov;
        msg.msg_iovlen = 1;

        return sendmsg(sock, &msg, 0);
  }

Control message data should be provided unencrypted, and will be
encrypted by the kernel.
(控制消息数据应以未加密的形式提供，由内核负责加密。)

Receiving TLS control messages(接收 TLS 控制消息)
-------------------------------------------------

TLS control messages are passed in the userspace buffer, with message
type passed via cmsg.  If no cmsg buffer is provided, an error is
returned if a control message is received.  Data messages may be
received without a cmsg buffer set.
(TLS 控制消息在用户态缓冲区中传递，消息类型通过 cmsg 传递。如果未提供 cmsg 缓冲区，那么收到控制消息时会返回错误。数据消息则可以在未设置 cmsg 缓冲区的情况下接收。)

.. code-block:: c

  char buffer[16384];
  char cmsg[CMSG_SPACE(sizeof(unsigned char))];
  struct msghdr msg = {0};
  msg.msg_control = cmsg;
  msg.msg_controllen = sizeof(cmsg);

  struct iovec msg_iov;
  msg_iov.iov_base = buffer;
  msg_iov.iov_len = 16384;

  msg.msg_iov = &msg_iov;
  msg.msg_iovlen = 1;

  int ret = recvmsg(sock, &msg, 0 /* flags */);

  struct cmsghdr *cmsg = CMSG_FIRSTHDR(&msg);
  if (cmsg->cmsg_level == SOL_TLS &&
      cmsg->cmsg_type == TLS_GET_RECORD_TYPE) {
      int record_type = *((unsigned char *)CMSG_DATA(cmsg));
      // Do something with record_type, and control message data in
      // buffer.
      //
      // Note that record_type may be == to application data (23).
  } else {
      // Buffer contains application data.
  }

recv will never return data from mixed types of TLS records.
(recv 永远不会把来自混合类型 TLS 记录的数据一起返回。)

TLS 1.3 Key Updates(TLS 1.3 密钥更新)
-------------------------------------

In TLS 1.3, KeyUpdate handshake messages signal that the sender is
updating its TX key. Any message sent after a KeyUpdate will be
encrypted using the new key. The userspace library can pass the new
key to the kernel using the TLS_TX and TLS_RX socket options, as for
the initial keys. TLS version and cipher cannot be changed.
(在 TLS 1.3 中，KeyUpdate 握手消息表示发送方正在更新其 TX 密钥。KeyUpdate 之后发送的任何消息都会使用新密钥加密。用户态库可以像提供初始密钥那样，通过 TLS_TX 和 TLS_RX 套接字选项把新密钥传给内核。TLS 版本和密码套件不能更改。)

To prevent attempting to decrypt incoming records using the wrong key,
decryption will be paused when a KeyUpdate message is received by the
kernel, until the new key has been provided using the TLS_RX socket
option. Any read occurring after the KeyUpdate has been read and
before the new key is provided will fail with EKEYEXPIRED. poll() will
not report any read events from the socket until the new key is
provided. There is no pausing on the transmit side.
(为防止使用错误的密钥解密入站记录，内核收到 KeyUpdate 消息后会暂停解密，直到通过 TLS_RX 套接字选项提供新密钥为止。在 KeyUpdate 已被读取之后、新密钥尚未提供之前的任何读取都会以 EKEYEXPIRED 失败。在新密钥提供之前，poll() 不会报告来自该套接字的任何读事件。发送方向没有暂停行为。)

Userspace should make sure that the crypto_info provided has been set
properly. In particular, the kernel will not check for key/nonce
reuse.
(用户态应确保所提供的 crypto_info 已被正确设置。特别要注意，内核不会检查密钥/随机数 (nonce) 是否重用。)

The number of successful and failed key updates is tracked in the
``TlsTxRekeyOk``, ``TlsRxRekeyOk``, ``TlsTxRekeyError``,
``TlsRxRekeyError`` statistics. The ``TlsRxRekeyReceived`` statistic
counts KeyUpdate handshake messages that have been received.
(成功和失败的密钥更新次数分别记录在 ``TlsTxRekeyOk`` 、 ``TlsRxRekeyOk`` 、 ``TlsTxRekeyError`` 和 ``TlsRxRekeyError`` 统计项中。 ``TlsRxRekeyReceived`` 统计项统计已收到的 KeyUpdate 握手消息数量。)

Integrating in to userspace TLS library(集成到用户态 TLS 库)
------------------------------------------------------------

At a high level, the kernel TLS ULP is a replacement for the record
layer of a userspace TLS library.
(从高层来看，内核 TLS ULP 可以替代用户态 TLS 库的记录层。)

A patchset to OpenSSL to use ktls as the record layer is
`here <https://github.com/Mellanox/openssl/commits/tls_rx2>`_.
(让 OpenSSL 使用 ktls 作为记录层的补丁集见上述链接。)

`An example <https://github.com/ktls/af_ktls-tool/commits/RX>`_
of calling send directly after a handshake using gnutls.
Since it doesn't implement a full record layer, control
messages are not supported.
(上面链接中的示例演示了使用 gnutls 在握手完成后直接调用 send。由于它没有实现完整的记录层，因此不支持控制消息。)

Optional optimizations(可选优化)
--------------------------------

There are certain condition-specific optimizations the TLS ULP can make,
if requested. Those optimizations are either not universally beneficial
or may impact correctness, hence they require an opt-in.
All options are set per-socket using setsockopt(), and their
state can be checked using getsockopt() and via socket diag (``ss``).
(TLS ULP 可以在请求时进行某些针对特定条件的优化。这些优化或者并非在所有情况下都有益，或者可能影响正确性，因此需要显式选择启用。所有选项都通过 setsockopt() 按套接字设置，其状态可以使用 getsockopt() 以及 socket diag ( ``ss`` ) 查看。)

TLS_TX_ZEROCOPY_RO(TLS_TX_ZEROCOPY_RO)
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

For device offload only. Allow sendfile() data to be transmitted directly
to the NIC without making an in-kernel copy. This allows true zero-copy
behavior when device offload is enabled.
(仅用于设备卸载。允许把 sendfile() 数据直接发送到网卡，而不在内核中进行复制。当启用设备卸载时，这可以实现真正的零拷贝行为。)

The application must make sure that the data is not modified between being
submitted and transmission completing. In other words this is mostly
applicable if the data sent on a socket via sendfile() is read-only.
(应用程序必须确保数据在提交之后、传输完成之前不被修改。换句话说，这主要适用于通过 sendfile() 在套接字上发送的数据是只读的情况。)

Modifying the data may result in different versions of the data being used
for the original TCP transmission and TCP retransmissions. To the receiver
this will look like TLS records had been tampered with and will result
in record authentication failures.
(修改数据可能导致原始 TCP 传输与 TCP 重传使用不同版本的数据。对接收方来说，这看起来就像 TLS 记录被篡改过，会导致记录认证失败。)

TLS_RX_EXPECT_NO_PAD(TLS_RX_EXPECT_NO_PAD)
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

TLS 1.3 only. Expect the sender to not pad records. This allows the data
to be decrypted directly into user space buffers with TLS 1.3.
(仅限 TLS 1.3。期望发送方不对记录做填充。这使得在 TLS 1.3 下可以把数据直接解密到用户态缓冲区。)

This optimization is safe to enable only if the remote end is trusted,
otherwise it is an attack vector to doubling the TLS processing cost.
(只有在对端可信时才适合启用此优化，否则它会成为一个使 TLS 处理开销翻倍的攻击向量。)

If the record decrypted turns out to had been padded or is not a data
record it will be decrypted again into a kernel buffer without zero copy.
Such events are counted in the ``TlsDecryptRetry`` statistic.
(如果解密后的记录结果表明它曾被填充，或者它不是数据记录，则会把它重新解密到内核缓冲区中，不使用零拷贝。这类事件会计入 ``TlsDecryptRetry`` 统计项。)

TLS_TX_MAX_PAYLOAD_LEN(TLS_TX_MAX_PAYLOAD_LEN)
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

Specifies the maximum size of the plaintext payload for transmitted TLS records.
(指定所发送 TLS 记录的明文负载最大大小。)

When this option is set, the kernel enforces the specified limit on all outgoing
TLS records. No plaintext fragment will exceed this size. This option can be used
to implement the TLS Record Size Limit extension [1].
(设置此选项后，内核会对所有发出的 TLS 记录强制执行指定的限制。明文片段不会超过此大小。此选项可用于实现 TLS Record Size Limit 扩展 [1]。)

* For TLS 1.2, the value corresponds directly to the record size limit.
  (对 TLS 1.2，该值直接对应记录大小限制。)
* For TLS 1.3, the value should be set to record_size_limit - 1, since
  the record size limit includes one additional byte for the ContentType
  field.
  (对 TLS 1.3，该值应设为 record_size_limit - 1，因为记录大小限制中包含 ContentType 字段占用的额外 1 个字节。)

The valid range for this option is 64 to 16384 bytes for TLS 1.2, and 63 to
16384 bytes for TLS 1.3. The lower minimum for TLS 1.3 accounts for the
extra byte used by the ContentType field.
(此选项的有效范围为：TLS 1.2 是 64 到 16384 字节，TLS 1.3 是 63 到 16384 字节。TLS 1.3 的最小值较低是为了顾及 ContentType 字段占用的那 1 个额外字节。)

[1] https://datatracker.ietf.org/doc/html/rfc8449
([1] 即 RFC 8449。)

Statistics(统计)
================

TLS implementation exposes the following per-namespace statistics
(``/proc/net/tls_stat``):
(TLS 实现暴露以下按命名空间统计的统计项 ( ``/proc/net/tls_stat`` )：)

- ``TlsCurrTxSw``, ``TlsCurrRxSw`` -
  number of TX and RX sessions currently installed where host handles
  cryptography
  (当前已安装、由主机处理加密的 TX 与 RX 会话数量)

- ``TlsCurrTxDevice``, ``TlsCurrRxDevice`` -
  number of TX and RX sessions currently installed where NIC handles
  cryptography
  (当前已安装、由网卡处理加密的 TX 与 RX 会话数量)

- ``TlsTxSw``, ``TlsRxSw`` -
  number of TX and RX sessions opened with host cryptography
  (使用主机加密打开的 TX 与 RX 会话数量)

- ``TlsTxDevice``, ``TlsRxDevice`` -
  number of TX and RX sessions opened with NIC cryptography
  (使用网卡加密打开的 TX 与 RX 会话数量)

- ``TlsDecryptError`` -
  record decryption failed (e.g. due to incorrect authentication tag)
  (记录解密失败 (例如认证标签不正确))

- ``TlsDeviceRxResync`` -
  number of RX resyncs sent to NICs handling cryptography
  (发送给处理加密的网卡的 RX 重同步次数)

- ``TlsDecryptRetry`` -
  number of RX records which had to be re-decrypted due to
  ``TLS_RX_EXPECT_NO_PAD`` mis-prediction. Note that this counter will
  also increment for non-data records.
  (因 ``TLS_RX_EXPECT_NO_PAD`` 预测错误而不得不重新解密的 RX 记录数量。注意，此计数器对非数据记录也会递增。)

- ``TlsRxNoPadViolation`` -
  number of data RX records which had to be re-decrypted due to
  ``TLS_RX_EXPECT_NO_PAD`` mis-prediction.
  (因 ``TLS_RX_EXPECT_NO_PAD`` 预测错误而不得不重新解密的数据 RX 记录数量。)

- ``TlsTxRekeyOk``, ``TlsRxRekeyOk`` -
  number of successful rekeys on existing sessions for TX and RX
  (TX 与 RX 现有会话上成功的密钥更新次数)

- ``TlsTxRekeyError``, ``TlsRxRekeyError`` -
  number of failed rekeys on existing sessions for TX and RX
  (TX 与 RX 现有会话上失败的密钥更新次数)

- ``TlsRxRekeyReceived`` -
  number of received KeyUpdate handshake messages, requiring userspace
  to provide a new RX key
  (已收到的 KeyUpdate 握手消息数量，需要用户态提供新的 RX 密钥)
