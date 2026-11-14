=======================
Direct Access for files
=======================

(文件直接访问)

Motivation
(动机)
----------

The page cache is usually used to buffer reads and writes to files.
It is also used to provide the pages which are mapped into userspace
by a call to mmap.
(页缓存通常用于缓冲对文件的读取和写入。它还用于提供通过调用 mmap 而映射到用户空间的那些页。)

For block devices that are memory-like, the page cache pages would be
unnecessary copies of the original storage.  The `DAX` code removes the
extra copy by performing reads and writes directly to the storage device.
For file mappings, the storage device is mapped directly into userspace.
(对于类内存型的块设备，页缓存页将是原始存储的多余副本。`DAX`(直接访问)代码通过直接对存储设备执行读取和写入来消除这个多余的副本。对于文件映射，存储设备被直接映射到用户空间。)


Usage
(用法)
-----

If you have a block device which supports `DAX`, you can make a filesystem
on it as usual.  The `DAX` code currently only supports files with a block
size equal to your kernel's `PAGE_SIZE`, so you may need to specify a block
size when creating the filesystem.
(如果你有一个支持 `DAX` 的块设备，你可以像往常一样在其上创建文件系统。`DAX` 代码目前只支持块大小等于内核 `PAGE_SIZE` 的文件，因此在创建文件系统时可能需要指定块大小。)

Currently 5 filesystems support `DAX`: ext2, ext4, xfs, virtiofs and erofs.
Enabling `DAX` on them is different.
(目前有 5 个文件系统支持 `DAX`：ext2、ext4、xfs、virtiofs 和 erofs。在它们上启用 `DAX` 的方式各不相同。)

Enabling DAX on ext2 and erofs
(在 ext2 和 erofs 上启用 DAX)
------------------------------

When mounting the filesystem, use the ``-o dax`` option on the command line or
add 'dax' to the options in ``/etc/fstab``.  This works to enable `DAX` on all files
within the filesystem.  It is equivalent to the ``-o dax=always`` behavior below.
(挂载文件系统时，在命令行上使用 ``-o dax`` 选项，或者在 ``/etc/fstab`` 的选项中添加 'dax'。这样可以对文件系统内的所有文件启用 `DAX`。它等同于下述 ``-o dax=always`` 的行为。)


Enabling DAX on xfs and ext4
(在 xfs 和 ext4 上启用 DAX)
----------------------------

Summary
(摘要)
-------

 1. There exists an in-kernel file access mode flag `S_DAX` that corresponds to
    the statx flag `STATX_ATTR_DAX`.  See the manpage for statx(2) for details
    about this access mode.
    (内核中存在一个文件访问模式标志 `S_DAX`，对应于 statx 标志 `STATX_ATTR_DAX`。有关此访问模式的详细信息，请参见 statx(2) 手册页。)

 2. There exists a persistent flag `FS_XFLAG_DAX` that can be applied to regular
    files and directories. This advisory flag can be set or cleared at any
    time, but doing so does not immediately affect the `S_DAX` state.
    (存在一个可以应用于普通文件和目录的持久标志 `FS_XFLAG_DAX`。此建议性标志可以随时设置或清除，但这样做不会立即影响 `S_DAX` 状态。)

 3. If the persistent `FS_XFLAG_DAX` flag is set on a directory, this flag will
    be inherited by all regular files and subdirectories that are subsequently
    created in this directory. Files and subdirectories that exist at the time
    this flag is set or cleared on the parent directory are not modified by
    this modification of the parent directory.
    (如果在某个目录上设置了持久标志 `FS_XFLAG_DAX`，那么随后在该目录中创建的所有普通文件和子目录都将继承该标志。在父目录上设置或清除该标志的那一刻已经存在的文件和子目录，不会因父目录的此次修改而被修改。)

 4. There exist dax mount options which can override `FS_XFLAG_DAX` in the
    setting of the `S_DAX` flag.  Given underlying storage which supports `DAX` the
    following hold:
    (存在一些 dax 挂载选项，它们可以在设置 `S_DAX` 标志时覆盖 `FS_XFLAG_DAX`。在底层存储支持 `DAX` 的前提下，以下内容成立：)

    ``-o dax=inode``  means "follow `FS_XFLAG_DAX`" and is the default.
    (``-o dax=inode`` 表示"遵循 `FS_XFLAG_DAX`"，并且是默认值。)

    ``-o dax=never``  means "never set `S_DAX`, ignore `FS_XFLAG_DAX`."
    (``-o dax=never`` 表示"永不设置 `S_DAX`，忽略 `FS_XFLAG_DAX`"。)

    ``-o dax=always`` means "always set `S_DAX` ignore `FS_XFLAG_DAX`."
    (``-o dax=always`` 表示"总是设置 `S_DAX`，忽略 `FS_XFLAG_DAX`"。)

    ``-o dax``      is a legacy option which is an alias for ``dax=always``.
    (``-o dax`` 是一个遗留选项，是 ``dax=always`` 的别名。)

    .. warning::

      The option ``-o dax`` may be removed in the future so ``-o dax=always`` is
      the preferred method for specifying this behavior.
      (选项 ``-o dax`` 未来可能会被移除，因此 ``-o dax=always`` 是指定此行为的首选方法。)

    .. note::

      Modifications to and the inheritance behavior of `FS_XFLAG_DAX` remain
      the same even when the filesystem is mounted with a dax option.  However,
      in-core inode state (`S_DAX`) will be overridden until the filesystem is
      remounted with dax=inode and the inode is evicted from kernel memory.
      (即使文件系统以 dax 选项挂载，对 `FS_XFLAG_DAX` 的修改及其继承行为仍保持不变。但是，在以 dax=inode 重新挂载文件系统并且该 inode 从内核内存中被逐出之前，内核内的 inode 状态(`S_DAX`)会一直被覆盖。)

 5. The `S_DAX` policy can be changed via:
    (`S_DAX` 策略可以通过以下方式更改：)

    a) Setting the parent directory `FS_XFLAG_DAX` as needed before files are
       created
       (在创建文件之前，根据需要设置父目录的 `FS_XFLAG_DAX`)

    b) Setting the appropriate dax="foo" mount option
       (设置适当的 dax="foo" 挂载选项)

    c) Changing the `FS_XFLAG_DAX` flag on existing regular files and
       directories.  This has runtime constraints and limitations that are
       described in 6) below.
       (更改现有普通文件和目录上的 `FS_XFLAG_DAX` 标志。这存在运行时方面的约束和限制，如下面 6) 中所述。)

 6. When changing the `S_DAX` policy via toggling the persistent `FS_XFLAG_DAX`
    flag, the change to existing regular files won't take effect until the
    files are closed by all processes.
    (在通过切换持久标志 `FS_XFLAG_DAX` 来更改 `S_DAX` 策略时，对现有普通文件的更改要等到所有进程都关闭这些文件之后才会生效。)


Details
(详细信息)
-------

There are 2 per-file dax flags.  One is a persistent inode setting (`FS_XFLAG_DAX`)
and the other is a volatile flag indicating the active state of the feature
(`S_DAX`).
(每个文件有 2 个 dax 标志。一个是持久的 inode 设置(`FS_XFLAG_DAX`)，另一个是指示该功能活动状态的易失性标志(`S_DAX`)。)

`FS_XFLAG_DAX` is preserved within the filesystem.  This persistent config
setting can be set, cleared and/or queried using the `FS_IOC_FS`[`GS`]`ETXATTR` ioctl
(see ioctl_xfs_fsgetxattr(2)) or an utility such as 'xfs_io'.
(`FS_XFLAG_DAX` 在文件系统内部被保存。可以使用 `FS_IOC_FS`[`GS`]`ETXATTR` ioctl(参见 ioctl_xfs_fsgetxattr(2))或诸如 'xfs_io' 之类的工具来设置、清除和/或查询这一持久配置。)

New files and directories automatically inherit `FS_XFLAG_DAX` from
their parent directory **when created**.  Therefore, setting `FS_XFLAG_DAX` at
directory creation time can be used to set a default behavior for an entire
sub-tree.
(新文件和新目录在**创建时**会自动从其父目录继承 `FS_XFLAG_DAX`。因此，在目录创建时设置 `FS_XFLAG_DAX` 可以用来为一整棵子树设定默认行为。)

To clarify inheritance, here are 3 examples:
(为阐明继承关系，下面给出 3 个示例：)

Example A:
(示例 A：)

.. code-block:: shell

  mkdir -p a/b/c
  xfs_io -c 'chattr +x' a
  mkdir a/b/c/d
  mkdir a/e

  ------[outcome]------

  dax: a,e
  no dax: b,c,d

Example B:
(示例 B：)

.. code-block:: shell

  mkdir a
  xfs_io -c 'chattr +x' a
  mkdir -p a/b/c/d

  ------[outcome]------

  dax: a,b,c,d
  no dax:

Example C:
(示例 C：)

.. code-block:: shell

  mkdir -p a/b/c
  xfs_io -c 'chattr +x' c
  mkdir a/b/c/d

  ------[outcome]------

  dax: c,d
  no dax: a,b

The current enabled state (`S_DAX`) is set when a file inode is instantiated in
memory by the kernel.  It is set based on the underlying media support, the
value of `FS_XFLAG_DAX` and the filesystem's dax mount option.
(当前的启用状态(`S_DAX`)是在内核把文件 inode 实例化到内存中时设置的。它依据底层介质支持情况、`FS_XFLAG_DAX` 的值以及文件系统的 dax 挂载选项来设置。)

statx can be used to query `S_DAX`.
(statx 可用于查询 `S_DAX`。)

.. note::

  That only regular files will ever have `S_DAX` set and therefore statx
  will never indicate that `S_DAX` is set on directories.
  (只有普通文件才会被设置 `S_DAX`，因此 statx 永远不会指示目录上设置了 `S_DAX`。)

Setting the `FS_XFLAG_DAX` flag (specifically or through inheritance) occurs even
if the underlying media does not support dax and/or the filesystem is
overridden with a mount option.
(即使底层介质不支持 dax 和/或文件系统被挂载选项覆盖，`FS_XFLAG_DAX` 标志(显式设置或通过继承)也照样会被设置。)


Enabling DAX on virtiofs
(在 virtiofs 上启用 DAX)
----------------------------
The semantic of DAX on virtiofs is basically equal to that on ext4 and xfs,
except that when '-o dax=inode' is specified, virtiofs client derives the hint
whether DAX shall be enabled or not from virtiofs server through FUSE protocol,
rather than the persistent `FS_XFLAG_DAX` flag. That is, whether DAX shall be
enabled or not is completely determined by virtiofs server, while virtiofs
server itself may deploy various algorithm making this decision, e.g. depending
on the persistent `FS_XFLAG_DAX` flag on the host.
(virtiofs 上 DAX 的语义基本上与 ext4 和 xfs 上的相同，区别在于：当指定 '-o dax=inode' 时，virtiofs 客户端是通过 FUSE 协议从 virtiofs 服务器获取是否应启用 DAX 的提示，而不是依据持久的 `FS_XFLAG_DAX` 标志。也就是说，是否启用 DAX 完全由 virtiofs 服务器决定，而 virtiofs 服务器自身可以部署各种算法来做出这一决定，例如取决于主机上持久的 `FS_XFLAG_DAX` 标志。)

It is still supported to set or clear persistent `FS_XFLAG_DAX` flag inside
guest, but it is not guaranteed that DAX will be enabled or disabled for
corresponding file then. Users inside guest still need to call statx(2) and
check the statx flag `STATX_ATTR_DAX` to see if DAX is enabled for this file.
(在客户机内设置或清除持久标志 `FS_XFLAG_DAX` 仍然是受支持的，但此后并不保证相应文件一定会启用或禁用 DAX。客户机内的用户仍需要调用 statx(2) 并检查 statx 标志 `STATX_ATTR_DAX`，以查看该文件是否启用了 DAX。)


Implementation Tips for Block Driver Writers
(块驱动程序编写者的实现提示)
--------------------------------------------

To support `DAX` in your block driver, implement the 'direct_access'
block device operation.  It is used to translate the sector number
(expressed in units of 512-byte sectors) to a page frame number (pfn)
that identifies the physical page for the memory.  It also returns a
kernel virtual address that can be used to access the memory.
(要在你的块驱动程序中支持 `DAX`，需要实现 'direct_access' 块设备操作。它用于把扇区号(以 512 字节扇区为单位表示)转换为标识该内存物理页的页帧号(pfn)。它还会返回一个可用于访问该内存的内核虚拟地址。)

The direct_access method takes a 'size' parameter that indicates the
number of bytes being requested.  The function should return the number
of bytes that can be contiguously accessed at that offset.  It may also
return a negative errno if an error occurs.
(direct_access 方法接受一个 'size' 参数，指示所请求的字节数。该函数应返回在该偏移处可以连续访问的字节数。如果发生错误，它也可以返回一个负的 errno。)

In order to support this method, the storage must be byte-accessible by
the CPU at all times.  If your device uses paging techniques to expose
a large amount of memory through a smaller window, then you cannot
implement direct_access.  Equally, if your device can occasionally
stall the CPU for an extended period, you should also not attempt to
implement direct_access.
(为了支持此方法，存储必须始终可以被 CPU 按字节访问。如果你的设备使用分页技术通过一个较小的窗口来暴露大量内存，那么你将无法实现 direct_access。同样，如果你的设备偶尔会使 CPU 长时间停顿，你也不应尝试实现 direct_access。)

These block devices may be used for inspiration:
(这些块设备可以作为参考：)
- pmem: NVDIMM persistent memory driver
  (pmem：NVDIMM 持久内存驱动程序)


Implementation Tips for Filesystem Writers
(文件系统编写者的实现提示)
------------------------------------------

Filesystem support consists of:
(文件系统支持包括以下内容：)

* Adding support to mark inodes as being `DAX` by setting the `S_DAX` flag in
  i_flags
  (添加支持，通过在 i_flags 中设置 `S_DAX` 标志来把 inode 标记为 `DAX`)
* Implementing ->read_iter and ->write_iter operations which use
  :c:func:`dax_iomap_rw()` when inode has `S_DAX` flag set
  (实现 ->read_iter 和 ->write_iter 操作，当 inode 设置了 `S_DAX` 标志时使用 :c:func:`dax_iomap_rw()`)
* Implementing an mmap file operation for `DAX` files which sets the
  `VM_MIXEDMAP` and `VM_HUGEPAGE` flags on the `VMA`, and setting the vm_ops to
  include handlers for fault, pmd_fault, page_mkwrite, pfn_mkwrite. These
  handlers should probably call :c:func:`dax_iomap_fault()` passing the
  appropriate fault size and iomap operations.
  (为 `DAX` 文件实现 mmap 文件操作，在 `VMA` 上设置 `VM_MIXEDMAP` 和 `VM_HUGEPAGE` 标志，并把 vm_ops 设置为包含 fault、pmd_fault、page_mkwrite、pfn_mkwrite 的处理程序。这些处理程序应当调用 :c:func:`dax_iomap_fault()`，并传入适当的缺页大小和 iomap 操作。)
* Calling :c:func:`iomap_zero_range()` passing appropriate iomap operations
  instead of :c:func:`block_truncate_page()` for `DAX` files
  (对于 `DAX` 文件，调用 :c:func:`iomap_zero_range()` 并传入适当的 iomap 操作，而不是调用 :c:func:`block_truncate_page()`)
* Ensuring that there is sufficient locking between reads, writes,
  truncates and page faults
  (确保在读取、写入、截断和缺页之间有充分的加锁)

The iomap handlers for allocating blocks must make sure that allocated blocks
are zeroed out and converted to written extents before being returned to avoid
exposure of uninitialized data through mmap.
(用于分配块的 iomap 处理程序必须确保已分配的块在返回之前被清零并转换为已写入的 extent，以避免通过 mmap 暴露未初始化的数据。)

These filesystems may be used for inspiration:
(这些文件系统可以作为参考：)

.. seealso::

  ext2: see Documentation/filesystems/ext2.rst
  (ext2：参见 Documentation/filesystems/ext2.rst)

.. seealso::

  xfs:  see Documentation/admin-guide/xfs.rst
  (xfs：参见 Documentation/admin-guide/xfs.rst)

.. seealso::

  ext4: see Documentation/filesystems/ext4/
  (ext4：参见 Documentation/filesystems/ext4/)


Handling Media Errors
(处理介质错误)
---------------------

The libnvdimm subsystem stores a record of known media error locations for
each pmem block device (in gendisk->badblocks). If we fault at such location,
or one with a latent error not yet discovered, the application can expect
to receive a `SIGBUS`. Libnvdimm also allows clearing of these errors by simply
writing the affected sectors (through the pmem driver, and if the underlying
NVDIMM supports the clear_poison DSM defined by ACPI).
(libnvdimm 子系统为每个 pmem 块设备保存了一份已知介质错误位置的记录(在 gendisk->badblocks 中)。如果我们在这样的位置、或某个尚未被发现的存在潜在错误的位置发生缺页，应用程序可以预期收到 `SIGBUS`。Libnvdimm 还允许通过简单地写入受影响的扇区来清除这些错误(通过 pmem 驱动程序进行，且前提是底层 NVDIMM 支持 ACPI 定义的 clear_poison DSM)。)

Since `DAX` IO normally doesn't go through the ``driver/bio`` path, applications or
sysadmins have an option to restore the lost data from a prior ``backup/inbuilt``
redundancy in the following ways:
(由于 `DAX` IO 通常不经过 ``driver/bio`` 路径，应用程序或系统管理员可以选择通过以下方式，从先前的 ``backup/inbuilt`` 冗余中恢复丢失的数据：)

1. Delete the affected file, and restore from a backup (sysadmin route):
   This will free the filesystem blocks that were being used by the file,
   and the next time they're allocated, they will be zeroed first, which
   happens through the driver, and will clear bad sectors.
   (删除受影响的文件，并从备份恢复(系统管理员路线)：这将释放该文件之前占用的文件系统块，下次这些块被分配时，会先把它们清零，这一过程通过驱动程序进行，并将清除坏扇区。)

2. Truncate or hole-punch the part of the file that has a bad-block (at least
   an entire aligned sector has to be hole-punched, but not necessarily an
   entire filesystem block).
   (对文件中包含坏块的部分进行截断或打洞(至少必须打洞掉一整个对齐的扇区，但不一定是一整个文件系统块)。)

These are the two basic paths that allow `DAX` filesystems to continue operating
in the presence of media errors. More robust error recovery mechanisms can be
built on top of this in the future, for example, involving redundancy/mirroring
provided at the block layer through DM, or additionally, at the filesystem
level. These would have to rely on the above two tenets, that error clearing
can happen either by sending an IO through the driver, or zeroing (also through
the driver).
(这是让 `DAX` 文件系统在存在介质错误的情况下仍能继续运行的两种基本途径。将来可以在其之上构建更健壮的错误恢复机制，例如，涉及块层通过 DM 提供的冗余/镜像，或者另外在文件系统层提供。这些机制将不得不依赖上述两条原则，即错误清除既可以由驱动程序通过发送 IO 来完成，也可以通过清零(同样通过驱动程序)来完成。)


Shortcomings
(缺点)
------------

Even if the kernel or its modules are stored on a filesystem that supports
`DAX` on a block device that supports `DAX`, they will still be copied into RAM.
(即使内核或其模块存储在支持 `DAX` 的块设备上支持 `DAX` 的文件系统中，它们仍然会被复制进 RAM。)

The DAX code does not work correctly on architectures which have virtually
mapped caches such as ARM, MIPS and SPARC.
(DAX 代码在具有虚拟映射缓存的架构(如 ARM、MIPS 和 SPARC)上无法正确工作。)

Calling :c:func:`get_user_pages()` on a range of user memory that has been
mmapped from a `DAX` file will fail when there are no 'struct page' to describe
those pages.  This problem has been addressed in some device drivers
by adding optional struct page support for pages under the control of
the driver (see `CONFIG_NVDIMM_PFN` in ``drivers/nvdimm`` for an example of
how to do this). In the non struct page cases `O_DIRECT` reads/writes to
those memory ranges from a non-`DAX` file will fail 
(对从 `DAX` 文件 mmap 而来的一段用户内存调用 :c:func:`get_user_pages()` 时，如果没有 'struct page' 来描述这些页，调用将会失败。这个问题已经在一些设备驱动程序中得到解决，方法是为驱动程序所控制的页添加可选的 struct page 支持(关于如何做到这一点，可参见 ``drivers/nvdimm`` 中的 `CONFIG_NVDIMM_PFN` 示例)。在非 struct page 的情况下，从非 `DAX` 文件对那些内存范围进行 `O_DIRECT` 读/写将会失败)


.. note::

  `O_DIRECT` reads/writes _of a `DAX` file do work, it is the memory that
  is being accessed that is key here).  Other things that will not work in
  the non struct page case include RDMA, :c:func:`sendfile()` and
  :c:func:`splice()`.
  (`O_DIRECT` 对 `DAX` 文件的读/写确实可以工作，这里的关键在于所访问的是内存)。在非 struct page 的情况下，其他无法工作的还包括 RDMA、:c:func:`sendfile()` 和 :c:func:`splice()`。)
