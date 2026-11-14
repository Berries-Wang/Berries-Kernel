==========================
Reference counting in pnfs
==========================

(pnfs 中的引用计数)

The are several inter-related caches.  We have layouts which can
reference multiple devices, each of which can reference multiple data servers.
Each data server can be referenced by multiple devices.  Each device
can be referenced by multiple layouts. To keep all of this straight,
we need to reference count.
(这里有几个相互关联的缓存。我们有可以引用多个设备的布局(layout)，其中每个设备又可以引用多个数据服务器(data server)。每个数据服务器可以被多个设备引用。每个设备可以被多个布局引用。为了把这些关系理清楚，我们需要引用计数。)


struct pnfs_layout_hdr
(结构体 pnfs_layout_hdr)
======================

The on-the-wire command LAYOUTGET corresponds to struct
pnfs_layout_segment, usually referred to by the variable name lseg.
Each nfs_inode may hold a pointer to a cache of these layout
segments in nfsi->layout, of type struct pnfs_layout_hdr.
(线上命令 LAYOUTGET 对应 struct pnfs_layout_segment，通常以变量名 lseg 来指代。每个 nfs_inode 都可以在 nfsi->layout 中持有一个指向这些布局段缓存的指针，其类型为 struct pnfs_layout_hdr。)

We reference the header for the inode pointing to it, across each
outstanding RPC call that references it (LAYOUTGET, LAYOUTRETURN,
LAYOUTCOMMIT), and for each lseg held within.
(我们为指向该头部的 inode 持有引用，为每个引用它的未完成 RPC 调用(LAYOUTGET、LAYOUTRETURN、LAYOUTCOMMIT)持有引用，并为其中包含的每个 lseg 持有引用。)

Each header is also (when non-empty) put on a list associated with
struct nfs_client (cl_layouts).  Being put on this list does not bump
the reference count, as the layout is kept around by the lseg that
keeps it in the list.
(每个头部还会(在非空时)被放入一个与 struct nfs_client 关联的列表(cl_layouts)中。放入该列表并不会增加引用计数，因为该布局是由把它留在列表中的 lseg 保全的。)

deviceid_cache
(设备 ID 缓存(deviceid_cache))
==============

lsegs reference device ids, which are resolved per nfs_client and
layout driver type.  The device ids are held in a RCU cache (struct
nfs4_deviceid_cache).  The cache itself is referenced across each
mount.  The entries (struct nfs4_deviceid) themselves are held across
the lifetime of each lseg referencing them.
(lseg 引用设备 ID，设备 ID 按 nfs_client 和布局驱动类型来解析。设备 ID 保存在一个 RCU 缓存(struct nfs4_deviceid_cache)中。该缓存本身在每次挂载(mount)期间持有引用。条目(struct nfs4_deviceid)本身则在引用它们的每个 lseg 的生命周期内被持有。)

RCU is used because the deviceid is basically a write once, read many
data structure.  The hlist size of 32 buckets needs better
justification, but seems reasonable given that we can have multiple
deviceid's per filesystem, and multiple filesystems per nfs_client.
(使用 RCU 是因为 deviceid 基本上是一个一次写入、多次读取的数据结构。hlist 的 32 个桶这一大小需要更好的论证，但考虑到每个文件系统可以有多个 deviceid、每个 nfs_client 可以有多个文件系统，这个大小看起来是合理的。)

The hash code is copied from the nfsd code base.  A discussion of
hashing and variations of this algorithm can be found `here.
<http://groups.google.com/group/comp.lang.c/browse_thread/thread/9522965e2b8d3809>`_
(哈希代码复制自 nfsd 代码库。关于哈希以及该算法各种变体的讨论可以在 `这里 <http://groups.google.com/group/comp.lang.c/browse_thread/thread/9522965e2b8d3809>`_ 找到。)

data server cache
(数据服务器缓存)
=================

file driver devices refer to data servers, which are kept in a module
level cache.  Its reference is held over the lifetime of the deviceid
pointing to it.
(file 驱动的设备引用数据服务器，这些数据服务器保存在一个模块级缓存中。指向某个数据服务器的 deviceid 在其整个生命周期内都会持有对该数据服务器的引用。)

lseg
(lseg(布局段))
====

lseg maintains an extra reference corresponding to the NFS_LSEG_VALID
bit which holds it in the pnfs_layout_hdr's list.  When the final lseg
is removed from the pnfs_layout_hdr's list, the NFS_LAYOUT_DESTROYED
bit is set, preventing any new lsegs from being added.
(lseg 维护一个与 NFS_LSEG_VALID 位相对应的额外引用，该位把它保持在 pnfs_layout_hdr 的列表中。当最后一个 lseg 从 pnfs_layout_hdr 的列表中移除时，NFS_LAYOUT_DESTROYED 位会被设置，从而防止添加任何新的 lseg。)

layout drivers
(布局驱动)
==============

PNFS utilizes what is called layout drivers. The STD defines 4 basic
layout types: "files", "objects", "blocks", and "flexfiles". For each
of these types there is a layout-driver with a common function-vectors
table which are called by the nfs-client pnfs-core to implement the
different layout types.
(PNFS 利用所谓的布局驱动(layout driver)。标准(STD)定义了 4 种基本布局类型："files"、"objects"、"blocks" 和 "flexfiles"。对于这些类型中的每一种，都有一个布局驱动，它带有一张公共的函数向量表，由 nfs 客户端的 pnfs-core 调用，以实现不同的布局类型。)

Files-layout-driver code is in: fs/nfs/filelayout/.. directory
Blocks-layout-driver code is in: fs/nfs/blocklayout/.. directory
Flexfiles-layout-driver code is in: fs/nfs/flexfilelayout/.. directory
(Files 布局驱动的代码位于 fs/nfs/filelayout/.. 目录；Blocks 布局驱动的代码位于 fs/nfs/blocklayout/.. 目录；Flexfiles 布局驱动的代码位于 fs/nfs/flexfilelayout/.. 目录。)

blocks-layout setup
(blocks 布局的设置)
===================

TODO: Document the setup needs of the blocks layout driver
(TODO: 记录 blocks 布局驱动所需的设置工作。)
