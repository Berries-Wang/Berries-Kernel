.. SPDX-License-Identifier: GPL-2.0

Multiple Mount Protection
(多挂载保护)
-------------------------

Multiple mount protection (MMP) is a feature that protects the
filesystem against multiple hosts trying to use the filesystem
simultaneously. When a filesystem is opened (for mounting, or fsck,
etc.), the MMP code running on the node (call it node A) checks a
sequence number. If the sequence number is EXT4_MMP_SEQ_CLEAN, the
open continues. If the sequence number is EXT4_MMP_SEQ_FSCK, then
fsck is (hopefully) running, and open fails immediately. Otherwise, the
open code will wait for twice the specified MMP check interval and check
the sequence number again. If the sequence number has changed, then the
filesystem is active on another machine and the open fails. If the MMP
code passes all of those checks, a new MMP sequence number is generated
and written to the MMP block, and the mount proceeds.
(多挂载保护(MMP)是一项保护文件系统免受多个主机同时尝试使用该文件系统的特性。当一个文件系统被打开(用于挂载或 fsck 等)时，在该节点上运行的 MMP 代码(称其为节点 A)会检查一个序列号。如果序列号是 EXT4_MMP_SEQ_CLEAN，则打开继续。如果序列号是 EXT4_MMP_SEQ_FSCK，那么 fsck 正在(但愿如此)运行，打开立即失败。否则，打开代码将等待两倍于指定的 MMP 检查间隔的时间，并再次检查序列号。如果序列号已改变，则该文件系统正在另一台机器上活动，打开失败。如果 MMP 代码通过了所有这些检查，则会生成一个新的 MMP 序列号并写入 MMP 块，挂载继续进行。)

While the filesystem is live, the kernel sets up a timer to re-check the
MMP block at the specified MMP check interval. To perform the re-check,
the MMP sequence number is re-read; if it does not match the in-memory
MMP sequence number, then another node (node B) has mounted the
filesystem, and node A remounts the filesystem read-only. If the
sequence numbers match, the sequence number is incremented both in
memory and on disk, and the re-check is complete.
(在文件系统活动期间，内核会设置一个定时器，按指定的 MMP 检查间隔重新检查 MMP 块。为了执行重新检查，会重新读取 MMP 序列号；如果它与内存中的 MMP 序列号不匹配，则另一个节点(节点 B)已经挂载了该文件系统，节点 A 会将文件系统重新以只读方式挂载。如果序列号匹配，则序列号在内存和磁盘上都会递增，重新检查完成。)

The hostname and device filename are written into the MMP block whenever
an open operation succeeds. The MMP code does not use these values; they
are provided purely for informational purposes.
(每当打开操作成功时，主机名和设备文件名都会写入 MMP 块。MMP 代码不使用这些值；它们纯粹是为提供信息而设。)

The checksum is calculated against the FS UUID and the MMP structure.
The MMP structure (``struct mmp_struct``) is as follows:
(校验和基于 FS UUID 和 MMP 结构计算。MMP 结构(``struct mmp_struct``)如下：)

.. list-table::
   :widths: 8 12 20 40
   :header-rows: 1

   * - Offset (偏移)
     - Type (类型)
     - Name (名称)
     - Description (描述)
   * - 0x0
     - __le32
     - mmp_magic
     - Magic number for MMP, 0x004D4D50 (“MMP”). (MMP 的幻数，0x004D4D50(“MMP”)。)
   * - 0x4
     - __le32
     - mmp_seq
     - Sequence number, updated periodically. (序列号，定期更新。)
   * - 0x8
     - __le64
     - mmp_time
     - Time that the MMP block was last updated. (MMP 块上次更新的时间。)
   * - 0x10
     - char[64]
     - mmp_nodename
     - Hostname of the node that opened the filesystem. (打开该文件系统的节点的主机名。)
   * - 0x50
     - char[32]
     - mmp_bdevname
     - Block device name of the filesystem. (该文件系统的块设备名。)
   * - 0x70
     - __le16
     - mmp_check_interval
     - The MMP re-check interval, in seconds. (MMP 重新检查间隔，以秒为单位。)
   * - 0x72
     - __le16
     - mmp_pad1
     - Zero. (零。)
   * - 0x74
     - __le32[226]
     - mmp_pad2
     - Zero. (零。)
   * - 0x3FC
     - __le32
     - mmp_checksum
     - Checksum of the MMP block. (MMP 块的校验和。)
