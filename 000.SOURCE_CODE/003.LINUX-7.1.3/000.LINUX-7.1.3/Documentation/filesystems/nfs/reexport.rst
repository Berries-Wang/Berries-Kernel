Reexporting NFS filesystems
(重新导出 NFS 文件系统)
===========================

Overview
(概述)
--------

It is possible to reexport an NFS filesystem over NFS.  However, this
feature comes with a number of limitations.  Before trying it, we
recommend some careful research to determine whether it will work for
your purposes.
(可以通过 NFS 再导出一个 NFS 文件系统。不过，该功能带有许多限制。在尝试之前，我们建议进行仔细的调研，以确定它是否适用于你的目的。)

A discussion of current known limitations follows.
(下文讨论目前已知的限制。)

"fsid=" required, crossmnt broken
(要求使用 "fsid="，crossmnt 已失效)
---------------------------------

We require the "fsid=" export option on any reexport of an NFS
filesystem.  You can use "uuidgen -r" to generate a unique argument.
(在对 NFS 文件系统进行任何再导出时，我们都要求使用 "fsid=" 导出选项。你可以使用 "uuidgen -r" 来生成一个唯一的参数。)

The "crossmnt" export does not propagate "fsid=", so it will not allow
traversing into further nfs filesystems; if you wish to export nfs
filesystems mounted under the exported filesystem, you'll need to export
them explicitly, assigning each its own unique "fsid= option.
("crossmnt" 导出不会传播 "fsid="，因此它不允许继续遍历进入更深层的 nfs 文件系统；如果你想导出挂载在被导出文件系统之下的 nfs 文件系统，就需要显式地导出它们，并为每个文件系统分配各自唯一的 "fsid=" 选项。)

Reboot recovery
(重启恢复)
---------------

The NFS protocol's normal reboot recovery mechanisms don't work for the
case when the reexport server reboots because the source server has not
rebooted, and so it is not in grace.  Since the source server is not in
grace, it cannot offer any guarantees that the file won't have been
changed between the locks getting lost and any attempt to recover them.
The same applies to delegations and any associated locks.  Clients are
not allowed to get file locks or delegations from a reexport server, any
attempts will fail with operation not supported.
(NFS 协议的正常重启恢复机制不适用于再导出服务器重启而源服务器未重启的情况，因为此时源服务器不处于宽限期(grace period)内。由于源服务器不在宽限期内，它无法提供任何保证，确保文件在锁丢失之后到尝试恢复锁之前不会被更改。同样的情况也适用于委托(delegation)及其关联的任何锁。不允许客户端从再导出服务器获取文件锁或委托，任何此类尝试都会以"操作不受支持"而失败。)

Filehandle limits
(文件句柄限制)
-----------------

If the original server uses an X byte filehandle for a given object, the
reexport server's filehandle for the reexported object will be X+22
bytes, rounded up to the nearest multiple of four bytes.
(如果原始服务器对某个对象使用 X 字节的文件句柄，那么再导出服务器针对该再导出对象生成的文件句柄将是 X+22 字节，并向上取整到最接近的 4 字节倍数。)

The result must fit into the RFC-mandated filehandle size limits:
(结果必须符合 RFC 规定的文件句柄大小限制：)

+-------+-----------+
| NFSv2 |  32 bytes (字节) |
+-------+-----------+
| NFSv3 |  64 bytes (字节) |
+-------+-----------+
| NFSv4 | 128 bytes (字节) |
+-------+-----------+

So, for example, you will only be able to reexport a filesystem over
NFSv2 if the original server gives you filehandles that fit in 10
bytes--which is unlikely.
(因此举例来说，只有当原始服务器给你的文件句柄能装进 10 字节时——这不太可能——你才能通过 NFSv2 再导出一个文件系统。)

In general there's no way to know the maximum filehandle size given out
by an NFS server without asking the server vendor.
(一般来说，不询问服务器厂商就无法知道某个 NFS 服务器所给出的最大文件句柄大小。)

But the following table gives a few examples.  The first column is the
typical length of the filehandle from a Linux server exporting the given
filesystem, the second is the length after that nfs export is reexported
by another Linux host:
(下表给出几个例子。第一列是 Linux 服务器导出给定文件系统时文件句柄的典型长度，第二列是该 nfs 导出被另一台 Linux 主机再导出之后的长度：)

+--------+-------------------+----------------+
|        | filehandle length (文件句柄长度) | after reexport (重新导出后) |
+========+===================+================+
| ext4:  | 28 bytes (字节)          | 52 bytes (字节)       |
+--------+-------------------+----------------+
| xfs:   | 32 bytes (字节)          | 56 bytes (字节)       |
+--------+-------------------+----------------+
| btrfs: | 40 bytes (字节)          | 64 bytes (字节)       |
+--------+-------------------+----------------+

All will therefore fit in an NFSv3 or NFSv4 filehandle after reexport,
but none are reexportable over NFSv2.
(因此，所有这些在再导出之后都能装进 NFSv3 或 NFSv4 的文件句柄，但没有一个能够通过 NFSv2 再导出。)

Linux server filehandles are a bit more complicated than this, though;
for example:
(不过，Linux 服务器的文件句柄比这要复杂一些；例如：)

        - The (non-default) "subtreecheck" export option generally
          requires another 4 to 8 bytes in the filehandle.
          ("subtreecheck" 导出选项(非默认)通常会在文件句柄中额外占用 4 到 8 字节。)
        - If you export a subdirectory of a filesystem (instead of
          exporting the filesystem root), that also usually adds 4 to 8
          bytes.
          (如果导出文件系统的某个子目录(而不是导出文件系统的根目录)，通常也会额外增加 4 到 8 字节。)
        - If you export over NFSv2, knfsd usually uses a shorter
          filesystem identifier that saves 8 bytes.
          (如果通过 NFSv2 导出，knfsd 通常会使用更短的文件系统标识符，从而节省 8 字节。)
        - The root directory of an export uses a filehandle that is
          shorter.
          (导出的根目录所使用的文件句柄会更短一些。)

As you can see, the 128-byte NFSv4 filehandle is large enough that
you're unlikely to have trouble using NFSv4 to reexport any filesystem
exported from a Linux server.  In general, if the original server is
something that also supports NFSv3, you're *probably* OK.  Re-exporting
over NFSv3 may be dicier, and reexporting over NFSv2 will probably
never work.
(可以看出，128 字节的 NFSv4 文件句柄足够大，使用 NFSv4 再导出任何从 Linux 服务器导出的文件系统时不太可能遇到问题。一般来说，如果原始服务器同时也支持 NFSv3，你 *大概* 没问题。通过 NFSv3 再导出可能风险更大一些，而通过 NFSv2 再导出则可能永远行不通。)

For more details of Linux filehandle structure, the best reference is
the source code and comments; see in particular:
(有关 Linux 文件句柄结构的更多细节，最好的参考是源代码及其注释；特别参见：)

        - include/linux/exportfs.h:enum fid_type
        - include/uapi/linux/nfsd/nfsfh.h:struct nfs_fhbase_new
        - fs/nfsd/nfsfh.c:set_version_and_fsid_type
        - fs/nfs/export.c:nfs_encode_fh

Open DENY bits ignored
(Open DENY 位被忽略)
----------------------

NFS since NFSv4 supports ALLOW and DENY bits taken from Windows, which
allow you, for example, to open a file in a mode which forbids other
read opens or write opens. The Linux client doesn't use them, and the
server's support has always been incomplete: they are enforced only
against other NFS users, not against processes accessing the exported
filesystem locally. A reexport server will also not pass them along to
the original server, so they will not be enforced between clients of
different reexport servers.
(自 NFSv4 起，NFS 支持取自 Windows 的 ALLOW 和 DENY 位，例如允许你以某种禁止其他读打开或写打开的模式打开文件。Linux 客户端并不使用它们，而服务器对它们的支持也一直不完整：它们只针对其他 NFS 用户实施，不针对在本地访问被导出文件系统的进程。再导出服务器也不会把它们传递给原始服务器，因此在不同再导出服务器的客户端之间，这些位不会被强制执行。)
