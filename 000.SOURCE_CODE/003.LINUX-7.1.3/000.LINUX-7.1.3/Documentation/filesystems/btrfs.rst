.. SPDX-License-Identifier: GPL-2.0

=====
BTRFS
=====

(BTRFS)

Btrfs is a copy on write filesystem for Linux aimed at implementing advanced
features while focusing on fault tolerance, repair and easy administration.
Jointly developed by several companies, licensed under the GPL and open for
contribution from anyone.
(Btrfs 是 Linux 的写时复制文件系统，旨在实现高级特性，同时注重容错、修复和易于管理。由多家公司联合开发，以 GPL 授权，并向任何人的贡献开放。)

The main Btrfs features include:
(Btrfs 的主要特性包括：)

    * Extent based file storage (2^64 max file size)
      (基于 extent 的文件存储(最大文件大小 2^64))
    * Space efficient packing of small files
      (节省空间的小文件打包)
    * Space efficient indexed directories
      (节省空间的索引目录)
    * Dynamic inode allocation
      (动态 inode 分配)
    * Writable snapshots
      (可写快照)
    * Subvolumes (separate internal filesystem roots)
      (子卷(独立的内部文件系统根))
    * Object level mirroring and striping
      (对象级的镜像与条带化)
    * Checksums on data and metadata (multiple algorithms available)
      (数据和元数据的校验和(支持多种算法))
    * Compression (multiple algorithms available)
      (压缩(支持多种算法))
    * Reflink, deduplication
      (Reflink、去重)
    * Scrub (on-line checksum verification)
      (Scrub(在线校验和验证))
    * Hierarchical quota groups (subvolume and snapshot support)
      (分层配额组(支持子卷和快照))
    * Integrated multiple device support, with several raid algorithms
      (集成多设备支持，提供多种 raid 算法)
    * Offline filesystem check
      (离线文件系统检查)
    * Efficient incremental backup and FS mirroring (send/receive)
      (高效的增量备份和文件系统镜像(send/receive))
    * Trim/discard
    * Online filesystem defragmentation
      (在线文件系统碎片整理)
    * Swapfile support
      (交换文件支持)
    * Zoned mode
      (zoned 模式)
    * Read/write metadata verification
      (读/写元数据验证)
    * Online resize (shrink, grow)
      (在线调整大小(收缩、增长))

For more information please refer to the documentation site or wiki
(更多信息请参阅文档站点或 wiki)

  https://btrfs.readthedocs.io


that maintains information about administration tasks, frequently asked
questions, use cases, mount options, comprehensible changelogs, features,
manual pages, source code repositories, contacts etc.
(其中维护着关于管理任务、常见问题、使用案例、挂载选项、易懂的更新日志、特性、手册页、源代码仓库、联系方式等信息。)
