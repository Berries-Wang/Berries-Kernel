.. SPDX-License-Identifier: GPL-2.0

.. _fsverity:

=======================================================
fs-verity: read-only file-based authenticity protection
=======================================================

(fs-verity：面向只读文件的真实性保护)

Introduction
(引言)
============

fs-verity (``fs/verity/``) is a support layer that filesystems can
hook into to support transparent integrity and authenticity protection
of read-only files.  Currently, it is supported by the ext4, f2fs, and
btrfs filesystems.  Like fscrypt, not too much filesystem-specific
code is needed to support fs-verity.
(fs-verity(fs/verity/)是一个支持层，文件系统可以挂接它来为只读文件提供透明的完整性与真实性保护。目前，ext4、f2fs 和 btrfs 文件系统支持它。与 fscrypt 一样，支持 fs-verity 所需的文件系统特定代码并不多。)

fs-verity is similar to `dm-verity
<https://www.kernel.org/doc/Documentation/admin-guide/device-mapper/verity.rst>`_
but works on files rather than block devices.  On regular files on
filesystems supporting fs-verity, userspace can execute an ioctl that
causes the filesystem to build a Merkle tree for the file and persist
it to a filesystem-specific location associated with the file.
(fs-verity 类似于 dm-verity，但作用于文件而不是块设备。在支持 fs-verity 的文件系统上的普通文件上，用户空间可以执行一个 ioctl，使文件系统为该文件构建 Merkle 树，并将其持久化到与该文件关联的文件系统特定位置。)

After this, the file is made readonly, and all reads from the file are
automatically verified against the file's Merkle tree.  Reads of any
corrupted data, including mmap reads, will fail.
(此后，该文件被设为只读，来自该文件的所有读取都会自动对照该文件的 Merkle 树进行验证。对任何已损坏数据的读取(包括 mmap 读取)都会失败。)

Userspace can use another ioctl to retrieve the root hash (actually
the "fs-verity file digest", which is a hash that includes the Merkle
tree root hash) that fs-verity is enforcing for the file.  This ioctl
executes in constant time, regardless of the file size.
(用户空间可以使用另一个 ioctl 来获取 fs-verity 对该文件强制实施的根哈希(实际上是"fs-verity 文件摘要"，它是一个包含了 Merkle 树根哈希的哈希)。无论文件大小如何，该 ioctl 都以恒定时间执行。)

fs-verity is essentially a way to hash a file in constant time,
subject to the caveat that reads which would violate the hash will
fail at runtime.
(fs-verity 本质上是一种以恒定时间对文件进行哈希的方法，但附带一个限制：违反哈希的读取在运行时会失败。)

Use cases
(使用场景)
=========

By itself, fs-verity only provides integrity protection, i.e.
detection of accidental (non-malicious) corruption.
(就其本身而言，fs-verity 只提供完整性保护，即检测意外(非恶意)的损坏。)

However, because fs-verity makes retrieving the file hash extremely
efficient, it's primarily meant to be used as a tool to support
authentication (detection of malicious modifications) or auditing
(logging file hashes before use).
(然而，由于 fs-verity 使得获取文件哈希极其高效，它主要旨在用作支持认证(检测恶意修改)或审计(在使用前记录文件哈希)的工具。)

A standard file hash could be used instead of fs-verity.  However,
this is inefficient if the file is large and only a small portion may
be accessed.  This is often the case for Android application package
(APK) files, for example.  These typically contain many translations,
classes, and other resources that are infrequently or even never
accessed on a particular device.  It would be slow and wasteful to
read and hash the entire file before starting the application.
(可以用标准文件哈希来代替 fs-verity。然而，如果文件很大而可能只访问其中一小部分，这样做的效率很低。例如，Android 应用程序包(APK)文件通常就是这种情况。它们通常包含许多翻译、类和其他资源，这些资源在特定设备上很少甚至从不被访问。在启动应用程序之前读取并哈希整个文件会很慢而且浪费。)

Unlike an ahead-of-time hash, fs-verity also re-verifies data each
time it's paged in.  This ensures that malicious disk firmware can't
undetectably change the contents of the file at runtime.
(与预先计算的哈希不同，fs-verity 还会在每次数据被换入(page in)时重新验证数据。这确保了恶意磁盘固件无法在运行时不可检测地更改文件内容。)

fs-verity does not replace or obsolete dm-verity.  dm-verity should
still be used on read-only filesystems.  fs-verity is for files that
must live on a read-write filesystem because they are independently
updated and potentially user-installed, so dm-verity cannot be used.
(fs-verity 并不取代或淘汰 dm-verity。只读文件系统仍应使用 dm-verity。fs-verity 面向的是必须驻留在读写文件系统上的文件，因为这些文件是独立更新的、且可能由用户自行安装，因此无法使用 dm-verity。)

fs-verity does not mandate a particular scheme for authenticating its
file hashes.  (Similarly, dm-verity does not mandate a particular
scheme for authenticating its block device root hashes.)  Options for
authenticating fs-verity file hashes include:
(fs-verity 并不强制规定对其文件哈希进行认证的特定方案。(类似地，dm-verity 也不强制规定对其块设备根哈希进行认证的特定方案。)认证 fs-verity 文件哈希的可选方案包括：)

- Trusted userspace code.  Often, the userspace code that accesses
  files can be trusted to authenticate them.  Consider e.g. an
  application that wants to authenticate data files before using them,
  or an application loader that is part of the operating system (which
  is already authenticated in a different way, such as by being loaded
  from a read-only partition that uses dm-verity) and that wants to
  authenticate applications before loading them.  In these cases, this
  trusted userspace code can authenticate a file's contents by
  retrieving its fs-verity digest using `FS_IOC_MEASURE_VERITY`_, then
  verifying a signature of it using any userspace cryptographic
  library that supports digital signatures.
  (受信任的用户空间代码。通常，访问文件的用户空间代码可以被信任来对文件进行认证。例如，考虑一个想在使用数据文件之前对其进行认证的应用程序，或者一个作为操作系统一部分的应用程序加载器(它本身已通过其他方式认证，例如从使用 dm-verity 的只读分区加载)，并且想在加载应用程序之前对其进行认证。在这些情况下，受信任的用户空间代码可以通过使用 FS_IOC_MEASURE_VERITY 获取文件的 fs-verity 摘要，然后使用任何支持数字签名的用户空间加密库验证其签名，来认证文件的内容。)

- Integrity Measurement Architecture (IMA).  IMA supports fs-verity
  file digests as an alternative to its traditional full file digests.
  "IMA appraisal" enforces that files contain a valid, matching
  signature in their "security.ima" extended attribute, as controlled
  by the IMA policy.  For more information, see the IMA documentation.
  (完整性度量架构(Integrity Measurement Architecture，IMA)。IMA 支持 fs-verity 文件摘要作为其传统完整文件摘要的替代方案。"IMA 评估(IMA appraisal)"强制要求文件在其 "security.ima" 扩展属性中包含一个有效且匹配的签名，这由 IMA 策略控制。更多信息请参阅 IMA 文档。)

- Integrity Policy Enforcement (IPE).  IPE supports enforcing access
  control decisions based on immutable security properties of files,
  including those protected by fs-verity's built-in signatures.
  "IPE policy" specifically allows for the authorization of fs-verity
  files using properties ``fsverity_digest`` for identifying
  files by their verity digest, and ``fsverity_signature`` to authorize
  files with a verified fs-verity's built-in signature. For
  details on configuring IPE policies and understanding its operational
  modes, please refer to :doc:`IPE admin guide </admin-guide/LSM/ipe>`.
  (完整性策略执行(Integrity Policy Enforcement，IPE)。IPE 支持基于文件的不可变安全属性来强制执行访问控制决策，包括受 fs-verity 内置签名保护的文件。"IPE 策略"特别允许使用属性 fsverity_digest(通过 verity 摘要标识文件)和 fsverity_signature(授权带有已验证 fs-verity 内置签名的文件)来对 fs-verity 文件进行授权。有关配置 IPE 策略及了解其运行模式的详细信息，请参阅 IPE 管理指南。)

- Trusted userspace code in combination with `Built-in signature
  verification`_.  This approach should be used only with great care.
  (受信任的用户空间代码与"内置签名验证"相结合。这种方法只应在非常谨慎的情况下使用。)

User API
(用户 API)
========

FS_IOC_ENABLE_VERITY
(启用 fs-verity)
--------------------

The FS_IOC_ENABLE_VERITY ioctl enables fs-verity on a file.  It takes
in a pointer to a struct fsverity_enable_arg, defined as
follows::
(FS_IOC_ENABLE_VERITY ioctl 在一个文件上启用 fs-verity。它接受一个指向 struct fsverity_enable_arg 的指针，该结构体定义如下：)

    struct fsverity_enable_arg {
            __u32 version;
            __u32 hash_algorithm;
            __u32 block_size;
            __u32 salt_size;
            __u64 salt_ptr;
            __u32 sig_size;
            __u32 __reserved1;
            __u64 sig_ptr;
            __u64 __reserved2[11];
    };

This structure contains the parameters of the Merkle tree to build for
the file.  It must be initialized as follows:
(该结构体包含要为该文件构建的 Merkle 树的参数。它必须按如下方式初始化：)

- ``version`` must be 1.
  (version 必须为 1。)
- ``hash_algorithm`` must be the identifier for the hash algorithm to
  use for the Merkle tree, such as FS_VERITY_HASH_ALG_SHA256.  See
  ``include/uapi/linux/fsverity.h`` for the list of possible values.
  (hash_algorithm 必须是用于 Merkle 树的哈希算法的标识符，例如 FS_VERITY_HASH_ALG_SHA256。可能的取值列表见 include/uapi/linux/fsverity.h。)
- ``block_size`` is the Merkle tree block size, in bytes.  In Linux
  v6.3 and later, this can be any power of 2 between (inclusively)
  1024 and the minimum of the system page size and the filesystem
  block size.  In earlier versions, the page size was the only allowed
  value.
  (block_size 是 Merkle 树块大小，以字节为单位。在 Linux v6.3 及更高版本中，它可以是 1024 到系统页大小与文件系统块大小两者中较小值之间(含边界)的任意 2 的幂。在更早的版本中，页大小是唯一允许的值。)
- ``salt_size`` is the size of the salt in bytes, or 0 if no salt is
  provided.  The salt is a value that is prepended to every hashed
  block; it can be used to personalize the hashing for a particular
  file or device.  Currently the maximum salt size is 32 bytes.
  (salt_size 是 salt 的字节大小；如果未提供 salt 则为 0。salt 是一个被前置到每个被哈希块之前的值；它可用于为特定文件或设备定制哈希计算。目前 salt 大小的上限为 32 字节。)
- ``salt_ptr`` is the pointer to the salt, or NULL if no salt is
  provided.
  (salt_ptr 是指向 salt 的指针；如果未提供 salt 则为 NULL。)
- ``sig_size`` is the size of the builtin signature in bytes, or 0 if no
  builtin signature is provided.  Currently the builtin signature is
  (somewhat arbitrarily) limited to 16128 bytes.
  (sig_size 是内置签名(builtin signature)的字节大小；如果未提供内置签名则为 0。目前内置签名(多少有些随意地)被限制为 16128 字节。)
- ``sig_ptr``  is the pointer to the builtin signature, or NULL if no
  builtin signature is provided.  A builtin signature is only needed
  if the `Built-in signature verification`_ feature is being used.  It
  is not needed for IMA appraisal, and it is not needed if the file
  signature is being handled entirely in userspace.
  (sig_ptr 是指向内置签名的指针；如果未提供内置签名则为 NULL。只有在使用"内置签名验证"特性时才需要内置签名。IMA 评估不需要它；如果文件签名完全在用户空间处理，也不需要它。)
- All reserved fields must be zeroed.
  (所有保留字段必须清零。)

FS_IOC_ENABLE_VERITY causes the filesystem to build a Merkle tree for
the file and persist it to a filesystem-specific location associated
with the file, then mark the file as a verity file.  This ioctl may
take a long time to execute on large files, and it is interruptible by
fatal signals.
(FS_IOC_ENABLE_VERITY 会使文件系统为该文件构建 Merkle 树，并将其持久化到与该文件关联的文件系统特定位置，然后把该文件标记为 verity 文件。对于大文件，该 ioctl 可能需要很长时间执行，并且它可以被致命信号中断。)

FS_IOC_ENABLE_VERITY checks for write access to the inode.  However,
it must be executed on an O_RDONLY file descriptor and no processes
can have the file open for writing.  Attempts to open the file for
writing while this ioctl is executing will fail with ETXTBSY.  (This
is necessary to guarantee that no writable file descriptors will exist
after verity is enabled, and to guarantee that the file's contents are
stable while the Merkle tree is being built over it.)
(FS_IOC_ENABLE_VERITY 会检查对 inode 的写访问权限。但是，它必须在以 O_RDONLY 打开的文件描述符上执行，并且不能有任何进程以写方式打开该文件。在该 ioctl 执行期间，尝试以写方式打开该文件会以 ETXTBSY 失败。(这样做是为了保证在启用 verity 之后不存在任何可写的文件描述符，并保证在为文件构建 Merkle 树期间文件内容保持稳定。))

On success, FS_IOC_ENABLE_VERITY returns 0, and the file becomes a
verity file.  On failure (including the case of interruption by a
fatal signal), no changes are made to the file.
(成功时，FS_IOC_ENABLE_VERITY 返回 0，该文件成为 verity 文件。失败时(包括被致命信号中断的情况)，不会对文件做任何更改。)

FS_IOC_ENABLE_VERITY can fail with the following errors:
(FS_IOC_ENABLE_VERITY 可能因以下错误而失败：)

- ``EACCES``: the process does not have write access to the file
  (进程对文件没有写访问权限。)
- ``EBADMSG``: the builtin signature is malformed
  (内置签名格式错误。)
- ``EBUSY``: this ioctl is already running on the file
  (该 ioctl 已在该文件上运行。)
- ``EEXIST``: the file already has verity enabled
  (该文件已启用 verity。)
- ``EFAULT``: the caller provided inaccessible memory
  (调用者提供了不可访问的内存。)
- ``EFBIG``: the file is too large to enable verity on
  (文件太大，无法对其启用 verity。)
- ``EINTR``: the operation was interrupted by a fatal signal
  (操作被致命信号中断。)
- ``EINVAL``: unsupported version, hash algorithm, or block size; or
  reserved bits are set; or the file descriptor refers to neither a
  regular file nor a directory.
  (不支持的版本、哈希算法或块大小；或者设置了保留位；或者文件描述符既不是普通文件也不是目录。)
- ``EISDIR``: the file descriptor refers to a directory
  (文件描述符指向一个目录。)
- ``EKEYREJECTED``: the builtin signature doesn't match the file
  (内置签名与文件不匹配。)
- ``EMSGSIZE``: the salt or builtin signature is too long
  (salt 或内置签名过长。)
- ``ENOKEY``: the ".fs-verity" keyring doesn't contain the certificate
  needed to verify the builtin signature
  (".fs-verity" 密钥环不包含验证内置签名所需的证书。)
- ``ENOPKG``: fs-verity recognizes the hash algorithm, but it's not
  available in the kernel as currently configured
  (fs-verity 能识别该哈希算法，但按当前配置它在内核中不可用。)
- ``ENOTTY``: this type of filesystem does not implement fs-verity
  (该类型的文件系统未实现 fs-verity。)
- ``EOPNOTSUPP``: the kernel was not configured with fs-verity
  support; or the filesystem superblock has not had the 'verity'
  feature enabled on it; or the filesystem does not support fs-verity
  on this file.  (See `Filesystem support`_.)
  (内核未配置 fs-verity 支持；或者文件系统超级块未启用 'verity' 特性；或者该文件系统不支持在此文件上使用 fs-verity。(参见"文件系统支持"一节。))
- ``EPERM``: the file is append-only; or, a builtin signature is
  required and one was not provided.
  (该文件是仅追加(append-only)的；或者要求提供内置签名但未提供。)
- ``EROFS``: the filesystem is read-only
  (该文件系统是只读的。)
- ``ETXTBSY``: someone has the file open for writing.  This can be the
  caller's file descriptor, another open file descriptor, or the file
  reference held by a writable memory map.
  (有人以写方式打开了该文件。这可能是调用者自己的文件描述符、另一个打开的文件描述符，或者可写内存映射持有的文件引用。)

FS_IOC_MEASURE_VERITY
(获取度量值)
---------------------

The FS_IOC_MEASURE_VERITY ioctl retrieves the digest of a verity file.
The fs-verity file digest is a cryptographic digest that identifies
the file contents that are being enforced on reads; it is computed via
a Merkle tree and is different from a traditional full-file digest.
(FS_IOC_MEASURE_VERITY ioctl 获取 verity 文件的摘要。fs-verity 文件摘要是一个标识在读取时被强制校验的文件内容的密码学摘要；它通过 Merkle 树计算得到，与传统的完整文件摘要不同。)

This ioctl takes in a pointer to a variable-length structure::
(该 ioctl 接受一个指向以下变长结构的指针：)

    struct fsverity_digest {
            __u16 digest_algorithm;
            __u16 digest_size; /* input/output */
            __u8 digest[];
    };

``digest_size`` is an input/output field.  On input, it must be
initialized to the number of bytes allocated for the variable-length
``digest`` field.
(digest_size 是一个输入/输出字段。作为输入时，它必须初始化为为变长的 digest 字段分配的字节数。)

On success, 0 is returned and the kernel fills in the structure as
follows:
(成功时返回 0，内核按如下方式填充该结构：)

- ``digest_algorithm`` will be the hash algorithm used for the file
  digest.  It will match ``fsverity_enable_arg::hash_algorithm``.
  (digest_algorithm 将是用于文件摘要的哈希算法。它将与 fsverity_enable_arg::hash_algorithm 匹配。)
- ``digest_size`` will be the size of the digest in bytes, e.g. 32
  for SHA-256.  (This can be redundant with ``digest_algorithm``.)
  (digest_size 将是摘要的字节大小，例如 SHA-256 为 32。(这可能与 digest_algorithm 冗余。))
- ``digest`` will be the actual bytes of the digest.
  (digest 将是摘要的实际字节。)

FS_IOC_MEASURE_VERITY is guaranteed to execute in constant time,
regardless of the size of the file.
(无论文件大小如何，FS_IOC_MEASURE_VERITY 都保证以恒定时间执行。)

FS_IOC_MEASURE_VERITY can fail with the following errors:
(FS_IOC_MEASURE_VERITY 可能因以下错误而失败：)

- ``EFAULT``: the caller provided inaccessible memory
  (调用者提供了不可访问的内存。)
- ``ENODATA``: the file is not a verity file
  (该文件不是 verity 文件。)
- ``ENOTTY``: this type of filesystem does not implement fs-verity
  (该类型的文件系统未实现 fs-verity。)
- ``EOPNOTSUPP``: the kernel was not configured with fs-verity
  support, or the filesystem superblock has not had the 'verity'
  feature enabled on it.  (See `Filesystem support`_.)
  (内核未配置 fs-verity 支持，或者文件系统超级块未启用 'verity' 特性。(参见"文件系统支持"一节。))
- ``EOVERFLOW``: the digest is longer than the specified
  ``digest_size`` bytes.  Try providing a larger buffer.
  (摘要长于指定的 digest_size 字节。请尝试提供更大的缓冲。)

FS_IOC_READ_VERITY_METADATA
(读取 verity 元数据)
---------------------------

The FS_IOC_READ_VERITY_METADATA ioctl reads verity metadata from a
verity file.  This ioctl is available since Linux v5.12.
(FS_IOC_READ_VERITY_METADATA ioctl 从 verity 文件读取 verity 元数据。该 ioctl 自 Linux v5.12 起可用。)

This ioctl is useful for cases where the verity verification should be
performed somewhere other than the currently running kernel.
(该 ioctl 适用于需要在当前运行的内核之外的其他地方执行 verity 验证的情况。)

One example is a server program that takes a verity file and serves it
to a client program, such that the client can do its own fs-verity
compatible verification of the file.  This only makes sense if the
client doesn't trust the server and if the server needs to provide the
storage for the client.
(一个例子是服务器程序，它获取一个 verity 文件并将其提供给客户端程序，使客户端能够自己对文件执行兼容 fs-verity 的验证。只有当客户端不信任服务器、且服务器需要为客户端提供存储时，这才有意义。)

Another example is copying verity metadata when creating filesystem
images in userspace (such as with ``mkfs.ext4 -d``).
(另一个例子是在用户空间创建文件系统镜像时复制 verity 元数据(例如使用 mkfs.ext4 -d)。)

This is a fairly specialized use case, and most fs-verity users won't
need this ioctl.
(这是一个相当特殊的使用场景，大多数 fs-verity 用户不需要这个 ioctl。)

This ioctl takes in a pointer to the following structure::
(该 ioctl 接受一个指向以下结构的指针：)

   #define FS_VERITY_METADATA_TYPE_MERKLE_TREE     1
   #define FS_VERITY_METADATA_TYPE_DESCRIPTOR      2
   #define FS_VERITY_METADATA_TYPE_SIGNATURE       3

   struct fsverity_read_metadata_arg {
           __u64 metadata_type;
           __u64 offset;
           __u64 length;
           __u64 buf_ptr;
           __u64 __reserved;
   };

``metadata_type`` specifies the type of metadata to read:
(metadata_type 指定要读取的元数据的类型：)

- ``FS_VERITY_METADATA_TYPE_MERKLE_TREE`` reads the blocks of the
  Merkle tree.  The blocks are returned in order from the root level
  to the leaf level.  Within each level, the blocks are returned in
  the same order that their hashes are themselves hashed.
  See `Merkle tree`_ for more information.
  (FS_VERITY_METADATA_TYPE_MERKLE_TREE 读取 Merkle 树的块。这些块按从根层级到叶层级的顺序返回。在每个层级内，块按其哈希自身被哈希时的相同顺序返回。更多信息参见"Merkle 树"一节。)

- ``FS_VERITY_METADATA_TYPE_DESCRIPTOR`` reads the fs-verity
  descriptor.  See `fs-verity descriptor`_.
  (FS_VERITY_METADATA_TYPE_DESCRIPTOR 读取 fs-verity 描述符。参见"fs-verity 描述符"一节。)

- ``FS_VERITY_METADATA_TYPE_SIGNATURE`` reads the builtin signature
  which was passed to FS_IOC_ENABLE_VERITY, if any.  See `Built-in
  signature verification`_.
  (FS_VERITY_METADATA_TYPE_SIGNATURE 读取传递给 FS_IOC_ENABLE_VERITY 的内置签名(如果有的话)。参见"内置签名验证"一节。)

The semantics are similar to those of ``pread()``.  ``offset``
specifies the offset in bytes into the metadata item to read from, and
``length`` specifies the maximum number of bytes to read from the
metadata item.  ``buf_ptr`` is the pointer to the buffer to read into,
cast to a 64-bit integer.  ``__reserved`` must be 0.  On success, the
number of bytes read is returned.  0 is returned at the end of the
metadata item.  The returned length may be less than ``length``, for
example if the ioctl is interrupted.
(其语义与 pread() 类似。offset 指定要从元数据项中开始读取的字节偏移量，length 指定从元数据项中最多读取的字节数。buf_ptr 是指向读入缓冲的指针，被转换为 64 位整数。__reserved 必须为 0。成功时返回读取的字节数。到达元数据项末尾时返回 0。返回的长度可能小于 length，例如 ioctl 被中断时。)

The metadata returned by FS_IOC_READ_VERITY_METADATA isn't guaranteed
to be authenticated against the file digest that would be returned by
`FS_IOC_MEASURE_VERITY`_, as the metadata is expected to be used to
implement fs-verity compatible verification anyway (though absent a
malicious disk, the metadata will indeed match).  E.g. to implement
this ioctl, the filesystem is allowed to just read the Merkle tree
blocks from disk without actually verifying the path to the root node.
(FS_IOC_READ_VERITY_METADATA 返回的元数据不保证已对照 FS_IOC_MEASURE_VERITY 将返回的文件摘要进行了认证，因为无论如何这些元数据都预期被用于实现兼容 fs-verity 的验证(不过在磁盘未被恶意篡改的情况下，元数据确实会匹配)。例如，为了实现该 ioctl，允许文件系统直接从磁盘读取 Merkle 树块，而不实际验证到根节点的路径。)

FS_IOC_READ_VERITY_METADATA can fail with the following errors:
(FS_IOC_READ_VERITY_METADATA 可能因以下错误而失败：)

- ``EFAULT``: the caller provided inaccessible memory
  (调用者提供了不可访问的内存。)
- ``EINTR``: the ioctl was interrupted before any data was read
  (ioctl 在读取到任何数据之前被中断。)
- ``EINVAL``: reserved fields were set, or ``offset + length``
  overflowed
  (设置了保留字段，或者 offset + length 溢出。)
- ``ENODATA``: the file is not a verity file, or
  FS_VERITY_METADATA_TYPE_SIGNATURE was requested but the file doesn't
  have a builtin signature
  (该文件不是 verity 文件；或者请求了 FS_VERITY_METADATA_TYPE_SIGNATURE 但该文件没有内置签名。)
- ``ENOTTY``: this type of filesystem does not implement fs-verity, or
  this ioctl is not yet implemented on it
  (该类型的文件系统未实现 fs-verity，或者该文件系统尚未实现此 ioctl。)
- ``EOPNOTSUPP``: the kernel was not configured with fs-verity
  support, or the filesystem superblock has not had the 'verity'
  feature enabled on it.  (See `Filesystem support`_.)
  (内核未配置 fs-verity 支持，或者文件系统超级块未启用 'verity' 特性。(参见"文件系统支持"一节。))

FS_IOC_GETFLAGS
(获取 inode 标志)
---------------

The existing ioctl FS_IOC_GETFLAGS (which isn't specific to fs-verity)
can also be used to check whether a file has fs-verity enabled or not.
To do so, check for FS_VERITY_FL (0x00100000) in the returned flags.
(现有的 ioctl FS_IOC_GETFLAGS(并非 fs-verity 特有)也可用于检查文件是否启用了 fs-verity。为此，检查返回的标志中是否设置了 FS_VERITY_FL (0x00100000)。)

The verity flag is not settable via FS_IOC_SETFLAGS.  You must use
FS_IOC_ENABLE_VERITY instead, since parameters must be provided.
(verity 标志不能通过 FS_IOC_SETFLAGS 设置。必须改用 FS_IOC_ENABLE_VERITY，因为必须提供参数。)

statx
(statx 系统调用)
-----

Since Linux v5.5, the statx() system call sets STATX_ATTR_VERITY if
the file has fs-verity enabled.  This can perform better than
FS_IOC_GETFLAGS and FS_IOC_MEASURE_VERITY because it doesn't require
opening the file, and opening verity files can be expensive.
(自 Linux v5.5 起，如果文件启用了 fs-verity，statx() 系统调用会设置 STATX_ATTR_VERITY。这可能比 FS_IOC_GETFLAGS 和 FS_IOC_MEASURE_VERITY 性能更好，因为它不需要打开文件，而打开 verity 文件的开销可能很大。)

FS_IOC_FSGETXATTR
(获取文件系统扩展属性)
-----------------

Since Linux v7.0, the FS_IOC_FSGETXATTR ioctl sets FS_XFLAG_VERITY (0x00020000)
in the returned flags when the file has verity enabled. Note that this attribute
cannot be set with FS_IOC_FSSETXATTR as enabling verity requires input
parameters. See FS_IOC_ENABLE_VERITY.
(自 Linux v7.0 起，当文件启用了 verity 时，FS_IOC_FSGETXATTR ioctl 会在返回的标志中设置 FS_XFLAG_VERITY (0x00020000)。注意，该属性不能通过 FS_IOC_FSSETXATTR 设置，因为启用 verity 需要输入参数。参见 FS_IOC_ENABLE_VERITY。)

file_getattr
(file_getattr 系统调用)
------------

Since Linux v7.0, the file_getattr() syscall sets FS_XFLAG_VERITY (0x00020000)
in the returned flags when the file has verity enabled. Note that this attribute
cannot be set with file_setattr() as enabling verity requires input parameters.
See FS_IOC_ENABLE_VERITY.
(自 Linux v7.0 起，当文件启用了 verity 时，file_getattr() 系统调用会在返回的标志中设置 FS_XFLAG_VERITY (0x00020000)。注意，该属性不能通过 file_setattr() 设置，因为启用 verity 需要输入参数。参见 FS_IOC_ENABLE_VERITY。)

.. _accessing_verity_files:

Accessing verity files
(访问 verity 文件)
======================

Applications can transparently access a verity file just like a
non-verity one, with the following exceptions:
(应用程序可以像访问非 verity 文件一样透明地访问 verity 文件，但有以下例外：)

- Verity files are readonly.  They cannot be opened for writing or
  truncate()d, even if the file mode bits allow it.  Attempts to do
  one of these things will fail with EPERM.  However, changes to
  metadata such as owner, mode, timestamps, and xattrs are still
  allowed, since these are not measured by fs-verity.  Verity files
  can also still be renamed, deleted, and linked to.
  (verity 文件是只读的。即使文件权限位允许，也不能以写方式打开它们或对其执行 truncate()。尝试做这些操作之一会以 EPERM 失败。不过，对所有者、权限模式、时间戳和 xattr 等元数据的更改仍然允许，因为这些不在 fs-verity 的度量范围内。verity 文件仍然可以被重命名、删除和建立链接。)

- Direct I/O is not supported on verity files.  Attempts to use direct
  I/O on such files will fall back to buffered I/O.
  (verity 文件不支持直接 I/O。尝试对这些文件使用直接 I/O 会退回到缓冲 I/O。)

- DAX (Direct Access) is not supported on verity files, because this
  would circumvent the data verification.
  (verity 文件不支持 DAX(直接访问)，因为这会绕过数据验证。)

- Reads of data that doesn't match the verity Merkle tree will fail
  with EIO (for read()) or SIGBUS (for mmap() reads).
  (读取与 verity Merkle 树不匹配的数据会以 EIO 失败(对于 read())或产生 SIGBUS(对于 mmap() 读取)。)

- If the sysctl "fs.verity.require_signatures" is set to 1 and the
  file is not signed by a key in the ".fs-verity" keyring, then
  opening the file will fail.  See `Built-in signature verification`_.
  (如果 sysctl "fs.verity.require_signatures" 被设为 1，且文件不是由 ".fs-verity" 密钥环中的密钥签名的，那么打开该文件将失败。参见"内置签名验证"一节。)

Direct access to the Merkle tree is not supported.  Therefore, if a
verity file is copied, or is backed up and restored, then it will lose
its "verity"-ness.  fs-verity is primarily meant for files like
executables that are managed by a package manager.
(不支持直接访问 Merkle 树。因此，如果 verity 文件被复制，或者被备份后再恢复，它将失去其"verity"属性。fs-verity 主要面向像可执行文件那样由包管理器管理的文件。)

File digest computation
(文件摘要的计算)
=======================

This section describes how fs-verity hashes the file contents using a
Merkle tree to produce the digest which cryptographically identifies
the file contents.  This algorithm is the same for all filesystems
that support fs-verity.
(本节描述 fs-verity 如何使用 Merkle 树对文件内容进行哈希，以产生在密码学上标识文件内容的摘要。对于所有支持 fs-verity 的文件系统，该算法都是相同的。)

Userspace only needs to be aware of this algorithm if it needs to
compute fs-verity file digests itself, e.g. in order to sign files.
(只有当用户空间需要自行计算 fs-verity 文件摘要时(例如为了给文件签名)，它才需要了解该算法。)

.. _fsverity_merkle_tree:

Merkle tree
(Merkle 树)
-----------

The file contents is divided into blocks, where the block size is
configurable but is usually 4096 bytes.  The end of the last block is
zero-padded if needed.  Each block is then hashed, producing the first
level of hashes.  Then, the hashes in this first level are grouped
into 'blocksize'-byte blocks (zero-padding the ends as needed) and
these blocks are hashed, producing the second level of hashes.  This
proceeds up the tree until only a single block remains.  The hash of
this block is the "Merkle tree root hash".
(文件内容被划分为多个块，块大小可配置，但通常为 4096 字节。如有需要，最后一个块的末尾用零填充。然后对每个块进行哈希，产生第一层哈希。接着，第一层中的哈希被分组为 'blocksize' 字节的块(根据需要在末尾补零)，再对这些块进行哈希，产生第二层哈希。这一过程沿树向上进行，直到只剩下一个块。该块的哈希就是"Merkle 树根哈希"。)

If the file fits in one block and is nonempty, then the "Merkle tree
root hash" is simply the hash of the single data block.  If the file
is empty, then the "Merkle tree root hash" is all zeroes.
(如果文件能放进一个块且非空，那么"Merkle 树根哈希"就是这个单一数据块的哈希。如果文件为空，那么"Merkle 树根哈希"全为零。)

The "blocks" here are not necessarily the same as "filesystem blocks".
(这里的"块"不一定与"文件系统块"相同。)

If a salt was specified, then it's zero-padded to the closest multiple
of the input size of the hash algorithm's compression function, e.g.
64 bytes for SHA-256 or 128 bytes for SHA-512.  The padded salt is
prepended to every data or Merkle tree block that is hashed.
(如果指定了 salt，则将其补零到哈希算法压缩函数输入大小的最接近倍数，例如 SHA-256 为 64 字节、SHA-512 为 128 字节。填充后的 salt 会被前置到每个参与哈希的数据块或 Merkle 树块之前。)

The purpose of the block padding is to cause every hash to be taken
over the same amount of data, which simplifies the implementation and
keeps open more possibilities for hardware acceleration.  The purpose
of the salt padding is to make the salting "free" when the salted hash
state is precomputed, then imported for each hash.
(块填充的目的是使每次哈希都针对相同数量的数据进行，这简化了实现并为硬件加速保留了更多可能性。salt 填充的目的是在预计算加盐哈希状态然后为每次哈希导入时，使加盐"免费"。)

Example: in the recommended configuration of SHA-256 and 4K blocks,
128 hash values fit in each block.  Thus, each level of the Merkle
tree is approximately 128 times smaller than the previous, and for
large files the Merkle tree's size converges to approximately 1/127 of
the original file size.  However, for small files, the padding is
significant, making the space overhead proportionally more.
(例如：在推荐的 SHA-256 与 4K 块的配置下，每个块可容纳 128 个哈希值。因此，Merkle 树的每一层大约比上一层小 128 倍，对于大文件，Merkle 树的大小收敛到约为原始文件大小的 1/127。然而对于小文件，填充所占比例显著，空间开销相应更大。)

.. _fsverity_descriptor:

fs-verity descriptor
(fs-verity 描述符)
--------------------

By itself, the Merkle tree root hash is ambiguous.  For example, it
can't a distinguish a large file from a small second file whose data
is exactly the top-level hash block of the first file.  Ambiguities
also arise from the convention of padding to the next block boundary.
(Merkle 树根哈希本身是有歧义的。例如，它无法区分一个大文件和一个数据恰好是第一个文件顶层哈希块的小文件。填充到下一个块边界的约定也会带来歧义。)

To solve this problem, the fs-verity file digest is actually computed
as a hash of the following structure, which contains the Merkle tree
root hash as well as other fields such as the file size::
(为解决这个问题，fs-verity 文件摘要实际上是作为以下结构的哈希来计算的，该结构包含 Merkle 树根哈希以及文件大小等其他字段：)

    struct fsverity_descriptor {
            __u8 version;           /* must be 1 */
            __u8 hash_algorithm;    /* Merkle tree hash algorithm */
            __u8 log_blocksize;     /* log2 of size of data and tree blocks */
            __u8 salt_size;         /* size of salt in bytes; 0 if none */
            __le32 __reserved_0x04; /* must be 0 */
            __le64 data_size;       /* size of file the Merkle tree is built over */
            __u8 root_hash[64];     /* Merkle tree root hash */
            __u8 salt[32];          /* salt prepended to each hashed block */
            __u8 __reserved[144];   /* must be 0's */
    };

Built-in signature verification
(内置签名验证)
===============================

CONFIG_FS_VERITY_BUILTIN_SIGNATURES=y adds supports for in-kernel
verification of fs-verity builtin signatures.
(CONFIG_FS_VERITY_BUILTIN_SIGNATURES=y 增加了对 fs-verity 内置签名的内核内验证支持。)

**IMPORTANT**!  Please take great care before using this feature.
It is not the only way to do signatures with fs-verity, and the
alternatives (such as userspace signature verification, and IMA
appraisal) can be much better.  It's also easy to fall into a trap
of thinking this feature solves more problems than it actually does.
(**重要**！在使用此特性之前请务必非常谨慎。它不是使用 fs-verity 进行签名的唯一方式，而替代方案(例如用户空间签名验证和 IMA 评估)可能要好得多。人们也很容易陷入一个误区，以为此特性解决的问题比它实际解决的要多。)

Enabling this option adds the following:
(启用该选项会增加以下内容：)

1. At boot time, the kernel creates a keyring named ".fs-verity".  The
   root user can add trusted X.509 certificates to this keyring using
   the add_key() system call.
   (在启动时，内核创建一个名为 ".fs-verity" 的密钥环。root 用户可以使用 add_key() 系统调用向该密钥环添加受信任的 X.509 证书。)

2. `FS_IOC_ENABLE_VERITY`_ accepts a pointer to a PKCS#7 formatted
   detached signature in DER format of the file's fs-verity digest.
   On success, the ioctl persists the signature alongside the Merkle
   tree.  Then, any time the file is opened, the kernel verifies the
   file's actual digest against this signature, using the certificates
   in the ".fs-verity" keyring. This verification happens as long as the
   file's signature exists, regardless of the state of the sysctl variable
   "fs.verity.require_signatures" described in the next item. The IPE LSM
   relies on this behavior to recognize and label fsverity files
   that contain a verified built-in fsverity signature.
   (FS_IOC_ENABLE_VERITY 接受一个指针，指向该文件 fs-verity 摘要的 PKCS#7 格式、DER 编码的分离式(detached)签名。成功时，该 ioctl 会将签名与 Merkle 树一起持久化保存。此后，每次打开该文件时，内核都会使用 ".fs-verity" 密钥环中的证书，对照此签名验证文件的实际摘要。只要文件的签名存在，该验证就会进行，而与下一项所述的 sysctl 变量 "fs.verity.require_signatures" 的状态无关。IPE LSM 依赖这一行为来识别并标记带有已验证内置 fsverity 签名的 fsverity 文件。)

3. A new sysctl "fs.verity.require_signatures" is made available.
   When set to 1, the kernel requires that all verity files have a
   correctly signed digest as described in (2).
   (提供一个新的 sysctl "fs.verity.require_signatures"。当设为 1 时，内核要求所有 verity 文件都带有如 (2) 所述的、经正确签名的摘要。)

The data that the signature as described in (2) must be a signature of
is the fs-verity file digest in the following format::
(如 (2) 所述，签名所必须签署的数据是以下格式的 fs-verity 文件摘要：)

    struct fsverity_formatted_digest {
            char magic[8];                  /* must be "FSVerity" */
            __le16 digest_algorithm;
            __le16 digest_size;
            __u8 digest[];
    };

That's it.  It should be emphasized again that fs-verity builtin
signatures are not the only way to do signatures with fs-verity.  See
`Use cases`_ for an overview of ways in which fs-verity can be used.
fs-verity builtin signatures have some major limitations that should
be carefully considered before using them:
(就这些。需要再次强调的是，fs-verity 内置签名并不是使用 fs-verity 进行签名的唯一方式。fs-verity 可使用方式的概述参见"使用场景"一节。fs-verity 内置签名有一些重大局限性，在使用之前应仔细考虑：)

- Builtin signature verification does *not* make the kernel enforce
  that any files actually have fs-verity enabled.  Thus, it is not a
  complete authentication policy.  Currently, if it is used, one
  way to complete the authentication policy is for trusted userspace
  code to explicitly check whether files have fs-verity enabled with a
  signature before they are accessed.  (With
  fs.verity.require_signatures=1, just checking whether fs-verity is
  enabled suffices.)  But, in this case the trusted userspace code
  could just store the signature alongside the file and verify it
  itself using a cryptographic library, instead of using this feature.
  (内置签名验证并不会让内核强制要求任何文件确实启用了 fs-verity。因此，它不是一个完整的认证策略。目前，如果使用它，完善认证策略的一种方式是由受信任的用户空间代码在文件被访问之前显式检查文件是否启用了带签名的 fs-verity。(在 fs.verity.require_signatures=1 时，只需检查 fs-verity 是否启用即可。)但是，在这种情况下，受信任的用户空间代码完全可以直接把签名与文件存放在一起，并使用加密库自行验证，而不必使用此特性。)

- Another approach is to utilize fs-verity builtin signature
  verification in conjunction with the IPE LSM, which supports defining
  a kernel-enforced, system-wide authentication policy that allows only
  files with a verified fs-verity builtin signature to perform certain
  operations, such as execution. Note that IPE doesn't require
  fs.verity.require_signatures=1.
  Please refer to :doc:`IPE admin guide </admin-guide/LSM/ipe>` for
  more details.
  (另一种方式是将 fs-verity 内置签名验证与 IPE LSM 结合使用，IPE 支持定义由内核强制执行的、系统级的认证策略，只允许带有已验证 fs-verity 内置签名的文件执行某些操作，例如执行。注意，IPE 不要求 fs.verity.require_signatures=1。更多细节请参阅 IPE 管理指南。)

- A file's builtin signature can only be set at the same time that
  fs-verity is being enabled on the file.  Changing or deleting the
  builtin signature later requires re-creating the file.
  (文件的内置签名只能在为该文件启用 fs-verity 的同时设置。之后更改或删除内置签名需要重新创建该文件。)

- Builtin signature verification uses the same set of public keys for
  all fs-verity enabled files on the system.  Different keys cannot be
  trusted for different files; each key is all or nothing.
  (内置签名验证对系统上所有启用了 fs-verity 的文件使用同一组公钥。不同的文件不能信任不同的密钥；每个密钥要么全部信任，要么完全不信任。)

- The sysctl fs.verity.require_signatures applies system-wide.
  Setting it to 1 only works when all users of fs-verity on the system
  agree that it should be set to 1.  This limitation can prevent
  fs-verity from being used in cases where it would be helpful.
  (sysctl fs.verity.require_signatures 在整个系统范围内生效。只有当系统上 fs-verity 的所有使用者都同意应设为 1 时，将其设为 1 才有效。这一限制可能阻碍 fs-verity 在本可发挥作用的场景中被使用。)

- Builtin signature verification can only use signature algorithms
  that are supported by the kernel.  For example, the kernel does not
  yet support Ed25519, even though this is often the signature
  algorithm that is recommended for new cryptographic designs.
  (内置签名验证只能使用内核支持的签名算法。例如，内核尚不支持 Ed25519，尽管对于新的密码学设计，这通常是所推荐的签名算法。)

- fs-verity builtin signatures are in PKCS#7 format, and the public
  keys are in X.509 format.  These formats are commonly used,
  including by some other kernel features (which is why the fs-verity
  builtin signatures use them), and are very feature rich.
  Unfortunately, history has shown that code that parses and handles
  these formats (which are from the 1990s and are based on ASN.1)
  often has vulnerabilities as a result of their complexity.  This
  complexity is not inherent to the cryptography itself.
  (fs-verity 内置签名采用 PKCS#7 格式，公钥采用 X.509 格式。这些格式被广泛使用，包括被其他一些内核特性使用(这正是 fs-verity 内置签名采用它们的原因)，而且功能非常丰富。遗憾的是，历史经验表明，解析和处理这些格式(它们源自 20 世纪 90 年代并基于 ASN.1)的代码由于其复杂性常常存在漏洞。这种复杂性并非密码学本身所固有。)

  fs-verity users who do not need advanced features of X.509 and
  PKCS#7 should strongly consider using simpler formats, such as plain
  Ed25519 keys and signatures, and verifying signatures in userspace.
  (不需要 X.509 和 PKCS#7 高级特性的 fs-verity 用户应认真考虑使用更简单的格式，例如纯 Ed25519 密钥和签名，并在用户空间验证签名。)

  fs-verity users who choose to use X.509 and PKCS#7 anyway should
  still consider that verifying those signatures in userspace is more
  flexible (for other reasons mentioned earlier in this document) and
  eliminates the need to enable CONFIG_FS_VERITY_BUILTIN_SIGNATURES
  and its associated increase in kernel attack surface.  In some cases
  it can even be necessary, since advanced X.509 and PKCS#7 features
  do not always work as intended with the kernel.  For example, the
  kernel does not check X.509 certificate validity times.
  (无论如何仍选择使用 X.509 和 PKCS#7 的 fs-verity 用户，也应考虑在用户空间验证这些签名更为灵活(原因见本文档前文所述)，并且可以免去启用 CONFIG_FS_VERITY_BUILTIN_SIGNATURES 及其带来的内核攻击面增加。在某些情况下这甚至是必需的，因为高级的 X.509 和 PKCS#7 特性与内核配合使用时并不总是符合预期。例如，内核不检查 X.509 证书的有效期。)

  Note: IMA appraisal, which supports fs-verity, does not use PKCS#7
  for its signatures, so it partially avoids the issues discussed
  here.  IMA appraisal does use X.509.
  (注意：支持 fs-verity 的 IMA 评估并不使用 PKCS#7 作为其签名，因此部分避免了这里讨论的问题。不过 IMA 评估确实使用 X.509。)

Filesystem support
(文件系统支持)
==================

fs-verity is supported by several filesystems, described below.  The
CONFIG_FS_VERITY kconfig option must be enabled to use fs-verity on
any of these filesystems.
(有多个文件系统支持 fs-verity，如下所述。要在其中任何文件系统上使用 fs-verity，必须启用 CONFIG_FS_VERITY kconfig 选项。)

``include/linux/fsverity.h`` declares the interface between the
``fs/verity/`` support layer and filesystems.  Briefly, filesystems
must provide an ``fsverity_operations`` structure that provides
methods to read and write the verity metadata to a filesystem-specific
location, including the Merkle tree blocks and
``fsverity_descriptor``.  Filesystems must also call functions in
``fs/verity/`` at certain times, such as when a file is opened or when
pages have been read into the pagecache.  (See `Verifying data`_.)
(include/linux/fsverity.h 声明了 fs/verity/ 支持层与文件系统之间的接口。简言之，文件系统必须提供一个 fsverity_operations 结构，其中包含将 verity 元数据(包括 Merkle 树块和 fsverity_descriptor)读写到文件系统特定位置的方法。文件系统还必须在特定时机调用 fs/verity/ 中的函数，例如在文件被打开时或页被读入页缓存时。(参见"验证数据"一节。))

ext4
(ext4 文件系统)
----

ext4 supports fs-verity since Linux v5.4 and e2fsprogs v1.45.2.
(ext4 自 Linux v5.4 和 e2fsprogs v1.45.2 起支持 fs-verity。)

To create verity files on an ext4 filesystem, the filesystem must have
been formatted with ``-O verity`` or had ``tune2fs -O verity`` run on
it.  "verity" is an RO_COMPAT filesystem feature, so once set, old
kernels will only be able to mount the filesystem readonly, and old
versions of e2fsck will be unable to check the filesystem.
(要在 ext4 文件系统上创建 verity 文件，该文件系统必须在格式化时带有 -O verity，或者对其运行过 tune2fs -O verity。"verity" 是一个 RO_COMPAT 文件系统特性，因此一旦设置，旧内核将只能以只读方式挂载该文件系统，旧版本的 e2fsck 将无法检查该文件系统。)

Originally, an ext4 filesystem with the "verity" feature could only be
mounted when its block size was equal to the system page size
(typically 4096 bytes).  In Linux v6.3, this limitation was removed.
(最初，带有 "verity" 特性的 ext4 文件系统只有在其块大小等于系统页大小(通常为 4096 字节)时才能被挂载。在 Linux v6.3 中，这一限制被移除。)

ext4 sets the EXT4_VERITY_FL on-disk inode flag on verity files.  It
can only be set by `FS_IOC_ENABLE_VERITY`_, and it cannot be cleared.
(ext4 在 verity 文件上设置 EXT4_VERITY_FL 磁盘上 inode 标志。它只能由 FS_IOC_ENABLE_VERITY 设置，并且不能被清除。)

ext4 also supports encryption, which can be used simultaneously with
fs-verity.  In this case, the plaintext data is verified rather than
the ciphertext.  This is necessary in order to make the fs-verity file
digest meaningful, since every file is encrypted differently.
(ext4 还支持加密，可以与 fs-verity 同时使用。在这种情况下，被验证的是明文数据而不是密文。为了使 fs-verity 文件摘要有意义，这是必要的，因为每个文件的加密方式都不同。)

ext4 stores the verity metadata (Merkle tree and fsverity_descriptor)
past the end of the file, starting at the first 64K boundary beyond
i_size.  This approach works because (a) verity files are readonly,
and (b) pages fully beyond i_size aren't visible to userspace but can
be read/written internally by ext4 with only some relatively small
changes to ext4.  This approach avoids having to depend on the
EA_INODE feature and on rearchitecting ext4's xattr support to
support paging multi-gigabyte xattrs into memory, and to support
encrypting xattrs.  Note that the verity metadata *must* be encrypted
when the file is, since it contains hashes of the plaintext data.
(ext4 将 verity 元数据(Merkle 树和 fsverity_descriptor)存储在文件末尾之后、从 i_size 之后的第一个 64K 边界开始的位置。这种方式可行是因为：(a)verity 文件是只读的；(b)完全超出 i_size 的页对用户空间不可见，但 ext4 只需对自身做相对较小的改动就能在内部读写这些页。这种方式避免了对 EA_INODE 特性的依赖，避免重新架构 ext4 的 xattr 支持以支持将数 GB 大小的 xattr 换入内存，以及支持加密 xattr。注意，当文件被加密时，verity 元数据*必须*被加密，因为它包含明文数据的哈希。)

ext4 only allows verity on extent-based files.
(ext4 只允许在基于 extent 的文件上使用 verity。)

f2fs
(f2fs 文件系统)
----

f2fs supports fs-verity since Linux v5.4 and f2fs-tools v1.11.0.
(f2fs 自 Linux v5.4 和 f2fs-tools v1.11.0 起支持 fs-verity。)

To create verity files on an f2fs filesystem, the filesystem must have
been formatted with ``-O verity``.
(要在 f2fs 文件系统上创建 verity 文件，该文件系统必须在格式化时带有 -O verity。)

f2fs sets the FADVISE_VERITY_BIT on-disk inode flag on verity files.
It can only be set by `FS_IOC_ENABLE_VERITY`_, and it cannot be
cleared.
(f2fs 在 verity 文件上设置 FADVISE_VERITY_BIT 磁盘上 inode 标志。它只能由 FS_IOC_ENABLE_VERITY 设置，并且不能被清除。)

Like ext4, f2fs stores the verity metadata (Merkle tree and
fsverity_descriptor) past the end of the file, starting at the first
64K boundary beyond i_size.  See explanation for ext4 above.
Moreover, f2fs supports at most 4096 bytes of xattr entries per inode
which usually wouldn't be enough for even a single Merkle tree block.
(与 ext4 一样，f2fs 将 verity 元数据(Merkle 树和 fsverity_descriptor)存储在文件末尾之后、从 i_size 之后的第一个 64K 边界开始的位置。参见上面对 ext4 的说明。此外，f2fs 每个 inode 最多支持 4096 字节的 xattr 项，这通常连一个 Merkle 树块都不够。)

f2fs doesn't support enabling verity on files that currently have
atomic or volatile writes pending.
(f2fs 不支持在当前有原子写(atomic write)或易失写(volatile write)未决的文件上启用 verity。)

btrfs
(btrfs 文件系统)
-----

btrfs supports fs-verity since Linux v5.15.  Verity-enabled inodes are
marked with a RO_COMPAT inode flag, and the verity metadata is stored
in separate btree items.
(btrfs 自 Linux v5.15 起支持 fs-verity。启用了 verity 的 inode 用一个 RO_COMPAT inode 标志标记，verity 元数据存储在单独的 btree 项中。)

Implementation details
(实现细节)
======================

Verifying data
(验证数据)
--------------

fs-verity ensures that all reads of a verity file's data are verified,
regardless of which syscall is used to do the read (e.g. mmap(),
read(), pread()) and regardless of whether it's the first read or a
later read (unless the later read can return cached data that was
already verified).  Below, we describe how filesystems implement this.
(fs-verity 确保 verity 文件数据的所有读取都被验证，无论使用哪个系统调用来读取(例如 mmap()、read()、pread())，也无论是第一次读取还是之后的读取(除非之后的读取可以返回已经验证过的缓存数据)。下面我们描述文件系统如何实现这一点。)

Pagecache
(页缓存)
~~~~~~~~~

For filesystems using Linux's pagecache, the ``->read_folio()`` and
``->readahead()`` methods must be modified to verify folios before
they are marked Uptodate.  Merely hooking ``->read_iter()`` would be
insufficient, since ``->read_iter()`` is not used for memory maps.
(对于使用 Linux 页缓存的文件系统，必须修改 ->read_folio() 和 ->readahead() 方法，在 folio 被标记为 Uptodate 之前对其进行验证。仅挂接 ->read_iter() 是不够的，因为内存映射不使用 ->read_iter()。)

Therefore, fs/verity/ provides the function fsverity_verify_blocks()
which verifies data that has been read into the pagecache of a verity
inode.  The containing folio must still be locked and not Uptodate, so
it's not yet readable by userspace.  As needed to do the verification,
fsverity_verify_blocks() will call back into the filesystem to read
hash blocks via fsverity_operations::read_merkle_tree_page().
(因此，fs/verity/ 提供了函数 fsverity_verify_blocks()，用于验证已读入 verity inode 页缓存的数据。包含这些数据的 folio 必须仍处于锁定状态且未标记 Uptodate，即还不能被用户空间读取。根据验证的需要，fsverity_verify_blocks() 会通过 fsverity_operations::read_merkle_tree_page() 回调文件系统来读取哈希块。)

fsverity_verify_blocks() returns false if verification failed; in this
case, the filesystem must not set the folio Uptodate.  Following this,
as per the usual Linux pagecache behavior, attempts by userspace to
read() from the part of the file containing the folio will fail with
EIO, and accesses to the folio within a memory map will raise SIGBUS.
(如果验证失败，fsverity_verify_blocks() 返回 false；在这种情况下，文件系统不得将 folio 标记为 Uptodate。此后，按照通常的 Linux 页缓存行为，用户空间尝试从包含该 folio 的文件部分进行 read() 将以 EIO 失败，而在内存映射中访问该 folio 将引发 SIGBUS。)

In principle, verifying a data block requires verifying the entire
path in the Merkle tree from the data block to the root hash.
However, for efficiency the filesystem may cache the hash blocks.
Therefore, fsverity_verify_blocks() only ascends the tree reading hash
blocks until an already-verified hash block is seen.  It then verifies
the path to that block.
(原则上，验证一个数据块需要验证 Merkle 树中从该数据块到根哈希的整条路径。但为了效率，文件系统可以缓存哈希块。因此，fsverity_verify_blocks() 只沿树向上读取哈希块，直到遇到一个已经验证过的哈希块，然后验证到该块的路径。)

This optimization, which is also used by dm-verity, results in
excellent sequential read performance.  This is because usually (e.g.
127 in 128 times for 4K blocks and SHA-256) the hash block from the
bottom level of the tree will already be cached and checked from
reading a previous data block.  However, random reads perform worse.
(这项优化(也被 dm-verity 使用)带来了出色的顺序读取性能。这是因为通常情况下(例如对于 4K 块和 SHA-256，128 次中有 127 次)，树最底层的哈希块在读取前一个数据块时就已经被缓存并检查过了。不过，随机读取的性能较差。)

Block device based filesystems
(基于块设备的文件系统)
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

Block device based filesystems (e.g. ext4 and f2fs) in Linux also use
the pagecache, so the above subsection applies too.  However, they
also usually read many data blocks from a file at once, grouped into a
structure called a "bio".  To make it easier for these types of
filesystems to support fs-verity, fs/verity/ also provides a function
fsverity_verify_bio() which verifies all data blocks in a bio.
(Linux 中基于块设备的文件系统(例如 ext4 和 f2fs)也使用页缓存，因此上述小节同样适用。不过，它们通常还会一次从文件中读取许多数据块，并分组到一个称为 "bio" 的结构中。为了使这类文件系统更容易支持 fs-verity，fs/verity/ 还提供了函数 fsverity_verify_bio()，用于验证一个 bio 中的所有数据块。)

ext4 and f2fs also support encryption.  If a verity file is also
encrypted, the data must be decrypted before being verified.  To
support this, these filesystems allocate a "post-read context" for
each bio and store it in ``->bi_private``::
(ext4 和 f2fs 还支持加密。如果 verity 文件也被加密，则数据必须先解密再验证。为支持这一点，这些文件系统为每个 bio 分配一个"读后上下文"(post-read context)，并将其存储在 ->bi_private 中：)

    struct bio_post_read_ctx {
           struct bio *bio;
           struct work_struct work;
           unsigned int cur_step;
           unsigned int enabled_steps;
    };

``enabled_steps`` is a bitmask that specifies whether decryption,
verity, or both is enabled.  After the bio completes, for each needed
postprocessing step the filesystem enqueues the bio_post_read_ctx on a
workqueue, and then the workqueue work does the decryption or
verification.  Finally, folios where no decryption or verity error
occurred are marked Uptodate, and the folios are unlocked.
(enabled_steps 是一个位掩码，指定启用了解密、verity 还是两者都启用。bio 完成后，对于每个所需的后处理步骤，文件系统将 bio_post_read_ctx 排入工作队列(workqueue)，然后工作队列中的工作执行解密或验证。最后，未发生解密或 verity 错误的 folio 被标记为 Uptodate，并且 folio 被解锁。)

On many filesystems, files can contain holes.  Normally,
``->readahead()`` simply zeroes hole blocks and considers the
corresponding data to be up-to-date; no bios are issued.  To prevent
this case from bypassing fs-verity, filesystems use
fsverity_verify_blocks() to verify hole blocks.
(在许多文件系统上，文件可以包含空洞(hole)。通常，->readahead() 只是将空洞块清零并认为相应数据是最新的；不会发出 bio。为防止这种情况绕过 fs-verity，文件系统使用 fsverity_verify_blocks() 来验证空洞块。)

Filesystems also disable direct I/O on verity files, since otherwise
direct I/O would bypass fs-verity.
(文件系统还会在 verity 文件上禁用直接 I/O，否则直接 I/O 会绕过 fs-verity。)

Userspace utility
(用户空间工具)
=================

This document focuses on the kernel, but a userspace utility for
fs-verity can be found at:
(本文档聚焦于内核，但 fs-verity 的用户空间工具可以在以下位置找到：)

	https://git.kernel.org/pub/scm/fs/fsverity/fsverity-utils.git

See the README.md file in the fsverity-utils source tree for details,
including examples of setting up fs-verity protected files.
(详情(包括设置 fs-verity 保护文件的示例)请参见 fsverity-utils 源码树中的 README.md 文件。)

Tests
(测试)
=====

To test fs-verity, use xfstests.  For example, using `kvm-xfstests
<https://github.com/tytso/xfstests-bld/blob/master/Documentation/kvm-quickstart.md>`_::
(要测试 fs-verity，请使用 xfstests。例如，使用 kvm-xfstests：)

    kvm-xfstests -c ext4,f2fs,btrfs -g verity

FAQ
(常见问题)
===

This section answers frequently asked questions about fs-verity that
weren't already directly answered in other parts of this document.
(本节回答关于 fs-verity 的常见问题，这些问题未在本文档的其他部分直接回答过。)

:Q: Why isn't fs-verity part of IMA?
    (为什么 fs-verity 不是 IMA 的一部分？)
:A: fs-verity and IMA (Integrity Measurement Architecture) have
    different focuses.  fs-verity is a filesystem-level mechanism for
    hashing individual files using a Merkle tree.  In contrast, IMA
    specifies a system-wide policy that specifies which files are
    hashed and what to do with those hashes, such as log them,
    authenticate them, or add them to a measurement list.
    (fs-verity 和 IMA(完整性度量架构)的侧重点不同。fs-verity 是一个文件系统级机制，用于使用 Merkle 树对单个文件进行哈希。相比之下，IMA 指定一个系统级策略，规定哪些文件被哈希以及如何处理这些哈希，例如记录日志、认证它们，或将它们加入度量列表。)

    IMA supports the fs-verity hashing mechanism as an alternative
    to full file hashes, for those who want the performance and
    security benefits of the Merkle tree based hash.  However, it
    doesn't make sense to force all uses of fs-verity to be through
    IMA.  fs-verity already meets many users' needs even as a
    standalone filesystem feature, and it's testable like other
    filesystem features e.g. with xfstests.
    (IMA 支持 fs-verity 哈希机制作为完整文件哈希的替代方案，适用于想要获得基于 Merkle 树的哈希所带来的性能和安全优势的人。然而，强制 fs-verity 的所有使用都通过 IMA 进行是没有意义的。即使作为一个独立的文件系统特性，fs-verity 也已经满足了许多用户的需求，而且它可以像其他文件系统特性一样进行测试，例如使用 xfstests。)

:Q: Isn't fs-verity useless because the attacker can just modify the
    hashes in the Merkle tree, which is stored on-disk?
    (鉴于攻击者可以直接修改存储在磁盘上的 Merkle 树中的哈希，fs-verity 难道不是没用吗？)
:A: To verify the authenticity of an fs-verity file you must verify
    the authenticity of the "fs-verity file digest", which
    incorporates the root hash of the Merkle tree.  See `Use cases`_.
    (要验证 fs-verity 文件的真实性，你必须验证"fs-verity 文件摘要"的真实性，它包含了 Merkle 树的根哈希。参见"使用场景"一节。)

:Q: Isn't fs-verity useless because the attacker can just replace a
    verity file with a non-verity one?
    (鉴于攻击者可以直接用一个非 verity 文件替换 verity 文件，fs-verity 难道不是没用吗？)
:A: See `Use cases`_.  In the initial use case, it's really trusted
    userspace code that authenticates the files; fs-verity is just a
    tool to do this job efficiently and securely.  The trusted
    userspace code will consider non-verity files to be inauthentic.
    (参见"使用场景"一节。在最初的使用场景中，真正对文件进行认证的是受信任的用户空间代码；fs-verity 只是高效且安全地完成这项工作的一个工具。受信任的用户空间代码会把非 verity 文件视为不可信的。)

:Q: Why does the Merkle tree need to be stored on-disk?  Couldn't you
    store just the root hash?
    (为什么 Merkle 树需要存储在磁盘上？难道不能只存储根哈希吗？)
:A: If the Merkle tree wasn't stored on-disk, then you'd have to
    compute the entire tree when the file is first accessed, even if
    just one byte is being read.  This is a fundamental consequence of
    how Merkle tree hashing works.  To verify a leaf node, you need to
    verify the whole path to the root hash, including the root node
    (the thing which the root hash is a hash of).  But if the root
    node isn't stored on-disk, you have to compute it by hashing its
    children, and so on until you've actually hashed the entire file.
    (如果 Merkle 树不存储在磁盘上，那么在文件首次被访问时，即使只读取一个字节，你也必须计算整棵树。这是 Merkle 树哈希工作方式的必然结果。要验证一个叶节点，你需要验证到根哈希的整条路径，包括根节点(根哈希所哈希的那个东西)。但如果根节点不存储在磁盘上，你就必须通过哈希其子节点来计算它，依此类推，直到你实际上对整个文件进行了哈希。)

    That defeats most of the point of doing a Merkle tree-based hash,
    since if you have to hash the whole file ahead of time anyway,
    then you could simply do sha256(file) instead.  That would be much
    simpler, and a bit faster too.
    (这违背了基于 Merkle 树哈希的大部分意义，因为如果反正必须预先对整个文件进行哈希，那你不如直接计算 sha256(file)。那样会简单得多，也稍微快一些。)

    It's true that an in-memory Merkle tree could still provide the
    advantage of verification on every read rather than just on the
    first read.  However, it would be inefficient because every time a
    hash page gets evicted (you can't pin the entire Merkle tree into
    memory, since it may be very large), in order to restore it you
    again need to hash everything below it in the tree.  This again
    defeats most of the point of doing a Merkle tree-based hash, since
    a single block read could trigger re-hashing gigabytes of data.
    (诚然，内存中的 Merkle 树仍然可以提供每次读取都进行验证(而不仅仅是首次读取)的优势。然而，这会很低效，因为每当一个哈希页被逐出时(你无法把整棵 Merkle 树固定在内存中，因为它可能非常大)，为了恢复它，你又需要重新哈希树中位于其之下的所有内容。这同样违背了基于 Merkle 树哈希的大部分意义，因为读取单个块就可能触发对数 GB 数据的重新哈希。)

:Q: But couldn't you store just the leaf nodes and compute the rest?
    (但难道不能只存储叶节点并计算其余部分吗？)
:A: See previous answer; this really just moves up one level, since
    one could alternatively interpret the data blocks as being the
    leaf nodes of the Merkle tree.  It's true that the tree can be
    computed much faster if the leaf level is stored rather than just
    the data, but that's only because each level is less than 1% the
    size of the level below (assuming the recommended settings of
    SHA-256 and 4K blocks).  For the exact same reason, by storing
    "just the leaf nodes" you'd already be storing over 99% of the
    tree, so you might as well simply store the whole tree.
    (参见上一个答案；这实际上只是把问题上移了一层，因为也可以把数据块解释为 Merkle 树的叶节点。诚然，如果存储叶层而不是只存储数据，树可以计算得快得多，但这只是因为每一层都小于其下一层大小的 1%(假设采用推荐的 SHA-256 和 4K 块设置)。出于完全相同的原因，存储"仅叶节点"时你实际上已经存储了树 99% 以上的内容，所以不如干脆存储整棵树。)

:Q: Can the Merkle tree be built ahead of time, e.g. distributed as
    part of a package that is installed to many computers?
    (Merkle 树可以提前构建吗？例如作为安装到多台计算机的软件包的一部分来分发？)
:A: This isn't currently supported.  It was part of the original
    design, but was removed to simplify the kernel UAPI and because it
    wasn't a critical use case.  Files are usually installed once and
    used many times, and cryptographic hashing is somewhat fast on
    most modern processors.
    (目前不支持。这曾是原始设计的一部分，但为了简化内核 UAPI，且因为它不是一个关键使用场景而被移除。文件通常只安装一次而使用多次，而且在大多数现代处理器上，加密哈希相对较快。)

:Q: Why doesn't fs-verity support writes?
    (为什么 fs-verity 不支持写入？)
:A: Write support would be very difficult and would require a
    completely different design, so it's well outside the scope of
    fs-verity.  Write support would require:
    (写支持会非常困难，并且需要完全不同的设计，因此远超 fs-verity 的范围。写支持将需要：)

    - A way to maintain consistency between the data and hashes,
      including all levels of hashes, since corruption after a crash
      (especially of potentially the entire file!) is unacceptable.
      The main options for solving this are data journalling,
      copy-on-write, and log-structured volume.  But it's very hard to
      retrofit existing filesystems with new consistency mechanisms.
      Data journalling is available on ext4, but is very slow.
      (一种在数据和哈希之间保持一致性的方法，包括所有层级的哈希，因为崩溃后出现损坏(尤其是可能整个文件损坏！)是不可接受的。解决这一问题的主要选项包括数据日志(data journalling)、写时复制(copy-on-write)和日志结构卷(log-structured volume)。但是，为现有文件系统加装新的一致性机制非常困难。数据日志在 ext4 上可用，但非常慢。)

    - Rebuilding the Merkle tree after every write, which would be
      extremely inefficient.  Alternatively, a different authenticated
      dictionary structure such as an "authenticated skiplist" could
      be used.  However, this would be far more complex.
      (每次写入后重建 Merkle 树，这会极其低效。或者，可以使用另一种经认证的字典结构，例如"经认证的跳表(authenticated skiplist)"。然而，这会复杂得多。)

    Compare it to dm-verity vs. dm-integrity.  dm-verity is very
    simple: the kernel just verifies read-only data against a
    read-only Merkle tree.  In contrast, dm-integrity supports writes
    but is slow, is much more complex, and doesn't actually support
    full-device authentication since it authenticates each sector
    independently, i.e. there is no "root hash".  It doesn't really
    make sense for the same device-mapper target to support these two
    very different cases; the same applies to fs-verity.
    (可以将其与 dm-verity 与 dm-integrity 的对比进行类比。dm-verity 非常简单：内核只是对照只读的 Merkle 树验证只读数据。相比之下，dm-integrity 支持写入但很慢、复杂得多，而且实际上不支持整设备认证，因为它独立地认证每个扇区，即没有"根哈希"。让同一个 device-mapper 目标支持这两种非常不同的场景并没有太大意义；fs-verity 也是如此。)

:Q: Since verity files are immutable, why isn't the immutable bit set?
    (既然 verity 文件是不可变的，为什么不设置 immutable 位？)
:A: The existing "immutable" bit (FS_IMMUTABLE_FL) already has a
    specific set of semantics which not only make the file contents
    read-only, but also prevent the file from being deleted, renamed,
    linked to, or having its owner or mode changed.  These extra
    properties are unwanted for fs-verity, so reusing the immutable
    bit isn't appropriate.
    (现有的 "immutable" 位(FS_IMMUTABLE_FL)已经有一组特定的语义，它不仅使文件内容只读，还阻止文件被删除、重命名、建立链接或更改其所有者或权限模式。这些额外属性对 fs-verity 来说是不需要的，因此重用 immutable 位并不合适。)

:Q: Why does the API use ioctls instead of setxattr() and getxattr()?
    (为什么 API 使用 ioctl 而不是 setxattr() 和 getxattr()？)
:A: Abusing the xattr interface for basically arbitrary syscalls is
    heavily frowned upon by most of the Linux filesystem developers.
    An xattr should really just be an xattr on-disk, not an API to
    e.g. magically trigger construction of a Merkle tree.
    (滥用 xattr 接口来执行基本上任意的系统调用会遭到大多数 Linux 文件系统开发者的强烈反对。xattr 在磁盘上就应该真的只是一个 xattr，而不是一个例如用来神奇地触发构建 Merkle 树的 API。)

:Q: Does fs-verity support remote filesystems?
    (fs-verity 支持远程文件系统吗？)
:A: So far all filesystems that have implemented fs-verity support are
    local filesystems, but in principle any filesystem that can store
    per-file verity metadata can support fs-verity, regardless of
    whether it's local or remote.  Some filesystems may have fewer
    options of where to store the verity metadata; one possibility is
    to store it past the end of the file and "hide" it from userspace
    by manipulating i_size.  The data verification functions provided
    by ``fs/verity/`` also assume that the filesystem uses the Linux
    pagecache, but both local and remote filesystems normally do so.
    (迄今为止，所有实现了 fs-verity 支持的文件系统都是本地文件系统，但原则上任何能够存储每文件 verity 元数据的文件系统都可以支持 fs-verity，无论它是本地还是远程的。某些文件系统在存储 verity 元数据的位置上可选余地较少；一种可能的做法是把它存储在文件末尾之后，并通过操纵 i_size 来对用户空间"隐藏"它。fs/verity/ 提供的数据验证函数也假设文件系统使用 Linux 页缓存，但本地和远程文件系统通常都使用页缓存。)

:Q: Why is anything filesystem-specific at all?  Shouldn't fs-verity
    be implemented entirely at the VFS level?
    (为什么需要任何文件系统特定的东西？fs-verity 难道不应该完全在 VFS 层实现吗？)
:A: There are many reasons why this is not possible or would be very
    difficult, including the following:
    (有很多原因使得这不可能或者会非常困难，包括以下几点：)

    - To prevent bypassing verification, folios must not be marked
      Uptodate until they've been verified.  Currently, each
      filesystem is responsible for marking folios Uptodate via
      ``->readahead()``.  Therefore, currently it's not possible for
      the VFS to do the verification on its own.  Changing this would
      require significant changes to the VFS and all filesystems.
      (为防止绕过验证，folio 在被验证之前不得被标记为 Uptodate。目前，每个文件系统负责通过 ->readahead() 将 folio 标记为 Uptodate。因此，目前 VFS 不可能自行完成验证。改变这一点需要对 VFS 和所有文件系统进行重大修改。)

    - It would require defining a filesystem-independent way to store
      the verity metadata.  Extended attributes don't work for this
      because (a) the Merkle tree may be gigabytes, but many
      filesystems assume that all xattrs fit into a single 4K
      filesystem block, and (b) ext4 and f2fs encryption doesn't
      encrypt xattrs, yet the Merkle tree *must* be encrypted when the
      file contents are, because it stores hashes of the plaintext
      file contents.
      (这将需要定义一种与文件系统无关的 verity 元数据存储方式。扩展属性不适合这样做，因为(a)Merkle 树可能有数 GB，而许多文件系统假设所有 xattr 都能放进单个 4K 文件系统块；(b)ext4 和 f2fs 的加密不会加密 xattr，而当文件内容被加密时，Merkle 树*必须*被加密，因为它存储着明文文件内容的哈希。)

      So the verity metadata would have to be stored in an actual
      file.  Using a separate file would be very ugly, since the
      metadata is fundamentally part of the file to be protected, and
      it could cause problems where users could delete the real file
      but not the metadata file or vice versa.  On the other hand,
      having it be in the same file would break applications unless
      filesystems' notion of i_size were divorced from the VFS's,
      which would be complex and require changes to all filesystems.
      (因此，verity 元数据将不得不存储在一个实际文件中。使用单独的文件会非常难看，因为元数据本质上是受保护文件的一部分，而且可能引发这样的问题：用户可能删除了真实文件却没有删除元数据文件，或者相反。另一方面，把它放在同一个文件中又会破坏应用程序，除非把文件系统的 i_size 概念与 VFS 的分离开来，这会很复杂且需要修改所有文件系统。)

    - It's desirable that FS_IOC_ENABLE_VERITY uses the filesystem's
      transaction mechanism so that either the file ends up with
      verity enabled, or no changes were made.  Allowing intermediate
      states to occur after a crash may cause problems.
      (希望 FS_IOC_ENABLE_VERITY 使用文件系统的事务机制，以便要么文件最终启用了 verity，要么未做任何更改。允许崩溃后出现中间状态可能引发问题。)
