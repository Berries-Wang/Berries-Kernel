.. SPDX-License-Identifier: GPL-2.0

=================================
The Linux NTFS filesystem driver
=================================

(Linux NTFS 文件系统驱动程序)


.. Table of contents

   - Overview
   - Utilities support
   - Supported mount options


Overview
(概述)
========

NTFS is a Linux kernel filesystem driver that provides full read and write
support for NTFS volumes. It is designed for high performance, modern
kernel infrastructure (iomap, folio), and stable long-term maintenance.
(NTFS 是一个 Linux 内核文件系统驱动程序，为 NTFS 卷提供完整的读写支持。它为高性能、现代内核基础设施(iomap、folio)以及稳定的长期维护而设计。)


Utilities support
(工具支持)
=================

The NTFS utilities project, called ntfsprogs-plus, provides mkfs.ntfs,
fsck.ntfs, and other related tools (e.g., ntfsinfo, ntfsclone, etc.) for
creating, checking, and managing NTFS volumes. These utilities can be used
for filesystem testing with xfstests as well as for recovering corrupted
NTFS devices.
(名为 ntfsprogs-plus 的 NTFS 工具项目提供 mkfs.ntfs、fsck.ntfs 以及其他相关工具(例如 ntfsinfo、ntfsclone 等)，用于创建、检查和管理 NTFS 卷。这些工具既可用于配合 xfstests 进行文件系统测试，也可用于恢复损坏的 NTFS 设备。)

The project is available at:
(该项目可在以下位置获得：)

  https://github.com/ntfsprogs-plus/ntfsprogs-plus


Supported mount options
(支持的挂载选项)
=======================

The NTFS driver supports the following mount options:
(NTFS 驱动程序支持以下挂载选项：)

======================= ====================================================
iocharset=name          Character set to use for converting between
                        the encoding is used for user visible filename and
                        16 bit Unicode characters. (用于在用户可见文件名所使用的编码与 16 位 Unicode 字符之间进行转换的字符集。)

nls=name                Deprecated option.  Still supported but please use
                        iocharset=name in the future. (已弃用的选项。仍然支持，但今后请使用 iocharset=name。)

uid=
gid=
umask=                  Provide default owner, group, and access mode mask.
                        These options work as documented in mount(8).  By
                        default, the files/directories are owned by root
                        and he/she has read and write permissions, as well
                        as browse permission for directories.  No one else
                        has any access permissions.  I.e. the mode on all
                        files is by default rw------- and
                        for directories rwx------, a consequence of
                        the default fmask=0177 and dmask=0077.
                        Using a umask of zero will grant all permissions to
                        everyone, i.e. all files and directories will have
                        mode rwxrwxrwx. (提供默认的所有者、属组和访问模式掩码。这些选项的作用如 mount(8) 中所记载。默认情况下，文件/目录归 root 所有，他/她拥有读和写权限，对目录还拥有浏览权限。其他任何人都没有任何访问权限。也就是说，所有文件的模式默认为 rw-------，而目录为 rwx------，这是默认 fmask=0177 和 dmask=0077 的结果。使用为零的 umask 会向所有人授予全部权限，即所有文件和目录的模式都将是 rwxrwxrwx。)

fmask=
dmask=                  Instead of specifying umask which applies both to
                        files and directories, fmask applies only to files
                        and dmask only to directories. (与其指定一个同时作用于文件和目录的 umask，fmask 只作用于文件，而 dmask 只作用于目录。)

showmeta=<BOOL>
show_sys_files=<BOOL>   If show_sys_files is specified, show the system
                        files in directory listings.  Otherwise the default
                        behaviour is to hide the system files.
                        Note that even when show_sys_files is specified,
                        "$MFT" will not be visible due to bugs/mis-features
                        in glibc. Further, note that irrespective of
                        show_sys_files, all files are accessible by name,
                        i.e. you can always do "ls -l \$UpCase" for example
                        to specifically show the system file containing
                        the Unicode upcase table. (如果指定了 show_sys_files，则在目录列表中显示系统文件。否则，默认行为是隐藏系统文件。注意，即使指定了 show_sys_files，由于 glibc 中的缺陷/特性缺失，"$MFT" 也不会可见。此外要注意，无论 show_sys_files 与否，所有文件都可以按名称访问，例如你总是可以执行 "ls -l \$UpCase" 来专门显示包含 Unicode 大写转换表的系统文件。)

case_sensitive=<BOOL>   If case_sensitive is specified, treat all filenames
                        as case sensitive and create file names in
                        the POSIX namespace (default behavior). Note,
                        the Linux NTFS driver will never create short
                        filenames and will remove them on rename/delete of
                        the corresponding long file name. Note that files
                        remain accessible via their short file name, if it
                        exists. (如果指定了 case_sensitive，则将所有文件名视为区分大小写，并在 POSIX 命名空间中创建文件名(默认行为)。注意，Linux NTFS 驱动程序永远不会创建短文件名，并且在重命名/删除对应的长文件名时会将它们移除。注意，文件仍然可以通过其短文件名访问(如果它存在的话)。)

nocase=<BOOL>           If nocase is specified, treat filenames
                        case-insensitively. (如果指定了 nocase，则以不区分大小写的方式对待文件名。)

disable_sparse=<BOOL>   If disable_sparse is specified, creation of sparse
                        regions, i.e. holes, inside files is disabled for
                        the volume (for the duration of this mount only).
                        By default, creation of sparse regions is enabled,
                        which is consistent with the behaviour of
                        traditional Unix filesystems. (如果指定了 disable_sparse，则在该卷上禁止在文件内部创建稀疏区域(即空洞)(仅限本次挂载期间)。默认情况下，稀疏区域的创建是启用的，这与传统 Unix 文件系统的行为一致。)

errors=opt              Specify NTFS behavior on critical errors: panic,
                        remount the partition in read-only mode or
                        continue without doing anything (default behavior). (指定 NTFS 在严重错误时的行为：panic、以只读模式重新挂载分区，或者不做任何事而继续(默认行为)。)

mft_zone_multiplier=    Set the MFT zone multiplier for the volume (this
                        setting is not persistent across mounts and can be
                        changed from mount to mount but cannot be changed
                        on remount).  Values of 1 to 4 are allowed, 1 being
                        the default.  The MFT zone multiplier determines
                        how much space is reserved for the MFT on the
                        volume.  If all other space is used up, then the
                        MFT zone will be shrunk dynamically, so this has no
                        impact on the amount of free space.  However, it
                        can have an impact on performance by affecting
                        fragmentation of the MFT. In general use the
                        default.  If you have a lot of small files then use
                        a higher value.  The values have the following
                        meaning: (为该卷设置 MFT 区倍数(此设置不会跨挂载持久保留，可以在不同挂载之间更改，但不能在重新挂载时更改)。允许的值为 1 到 4，默认值为 1。MFT 区倍数决定为该卷上的 MFT 预留多少空间。如果所有其他空间都被用完，那么 MFT 区会被动态收缩，因此这对可用空间的大小没有影响。不过，它会通过影响 MFT 的碎片化而影响性能。一般来说请使用默认值。如果你有大量小文件，则使用更大的值。各值的含义如下：)

                        =====   =================================
                        Value   MFT zone size (% of volume size) (值   MFT 区大小(占卷大小的百分比))
                        =====   =================================
                          1             12.5%
                          2             25%
                          3             37.5%
                          4             50%
                        =====   =================================

                        Note this option is irrelevant for read-only mount. (注意，此选项对只读挂载无关紧要。)

preallocated_size=      Set preallocated size to optimize runlist merge
                        overhead with small chunck size.(64KB size by
                        default) (设置预分配大小，以优化小块尺寸下 runlist 合并的开销。(默认 64KB 大小))

acl=<BOOL>              Enable POSIX ACL support. When specified, POSIX
                        ACLs stored in extended attributes are enforced.
                        Default is off. Requires kernel config
                        NTFS_FS_POSIX_ACL enabled. (启用 POSIX ACL 支持。指定此选项时，将强制执行存储在扩展属性中的 POSIX ACL。默认关闭。需要启用内核配置 NTFS_FS_POSIX_ACL。)

sys_immutable=<BOOL>    Make NTFS system files (e.g. $MFT, $LogFile,
                        $Bitmap, $UpCase, etc.) immutable to user initiated
                        modifications for extra safety. Default is off. (使 NTFS 系统文件(例如 $MFT、$LogFile、$Bitmap、$UpCase 等)对用户发起的修改不可变，以获得额外的安全性。默认关闭。)

nohidden=<BOOL>         Hide files and directories marked with the Windows
                        "hidden" attribute. By default hidden items are
                        shown. (隐藏带有 Windows "hidden" 属性的文件和目录。默认情况下显示隐藏项。)

hide_dot_files=<BOOL>   Hide names beginning with a dot ("."). By default
                        dot files are shown. When enabled, files and
                        directories created with a leading '.' will be
                        hidden from directory listings. (隐藏以点(".")开头的名称。默认情况下显示点文件。启用后，以 '.' 开头创建的文件和目录将从目录列表中隐藏。)

windows_names=<BOOL>    Refuse creation/rename of files with characters or
                        reserved device names disallowed on Windows (e.g.
                        CON, NUL, AUX, COM1, LPT1, etc.). Default is off. (拒绝创建/重命名包含 Windows 不允许的字符或保留设备名(例如 CON、NUL、AUX、COM1、LPT1 等)的文件。默认关闭。)
discard=<BOOL>          Issue block device discard for clusters freed on
                        file deletion/truncation to inform underlying
                        storage. (对因文件删除/截断而释放的簇发出块设备 discard 命令，以通知底层存储设备。)
======================= ====================================================
