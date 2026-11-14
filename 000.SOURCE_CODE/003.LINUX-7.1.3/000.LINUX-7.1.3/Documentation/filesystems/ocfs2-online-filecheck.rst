.. SPDX-License-Identifier: GPL-2.0

=====================================
OCFS2 file system - online file check
=====================================

(OCFS2 文件系统 - 在线文件检查)

This document will describe OCFS2 online file check feature.
(本文档将描述 OCFS2 在线文件检查特性。)

Introduction
(简介)
============
OCFS2 is often used in high-availability systems. However, OCFS2 usually
converts the filesystem to read-only when encounters an error. This may not be
necessary, since turning the filesystem read-only would affect other running
processes as well, decreasing availability.
Then, a mount option (errors=continue) is introduced, which would return the
-EIO errno to the calling process and terminate further processing so that the
filesystem is not corrupted further. The filesystem is not converted to
read-only, and the problematic file's inode number is reported in the kernel
log. The user can try to check/fix this file via online filecheck feature.
(OCFS2 常用于高可用系统。然而，OCFS2 在遇到错误时通常会将文件系统转换为只读。这可能是没有必要的，因为将文件系统转为只读也会影响其他正在运行的进程，降低可用性。于是引入了一个挂载选项(errors=continue)，它会向调用进程返回 -EIO errno 并终止后续处理，使文件系统不会进一步损坏。文件系统不会被转换为只读，出问题的文件的 inode 号会被记录到内核日志中。用户可以尝试通过在线文件检查(online filecheck)特性来检查/修复此文件。)

Scope
(范围)
=====
This effort is to check/fix small issues which may hinder day-to-day operations
of a cluster filesystem by turning the filesystem read-only. The scope of
checking/fixing is at the file level, initially for regular files and eventually
to all files (including system files) of the filesystem.
(这项工作的目的是检查/修复那些可能通过将文件系统转为只读而妨碍集群文件系统日常运作的小问题。检查/修复的范围在文件级别，最初针对普通文件，最终扩展到文件系统的所有文件(包括系统文件)。)

In case of directory to file links is incorrect, the directory inode is
reported as erroneous.
(如果目录到文件的链接不正确，则该目录 inode 会被报告为有误。)

This feature is not suited for extravagant checks which involve dependency of
other components of the filesystem, such as but not limited to, checking if the
bits for file blocks in the allocation has been set. In case of such an error,
the offline fsck should/would be recommended.
(此特性不适合进行涉及文件系统其他组件依赖的复杂检查，例如但不限于检查分配中文件块的位是否已被设置。如果发生此类错误，则会建议使用离线 fsck。)

Finally, such an operation/feature should not be automated lest the filesystem
may end up with more damage than before the repair attempt. So, this has to
be performed using user interaction and consent.
(最后，这样的操作/特性不应被自动化，以免文件系统最终遭受比修复尝试之前更多的损坏。因此，这必须在用户交互和同意的情况下进行。)

User interface
(用户接口)
==============
When there are errors in the OCFS2 filesystem, they are usually accompanied
by the inode number which caused the error. This inode number would be the
input to check/fix the file.
(当 OCFS2 文件系统出现错误时，通常伴随着导致错误的 inode 号。这个 inode 号将是检查/修复该文件的输入。)

There is a sysfs directory for each OCFS2 file system mounting::
(每个已挂载的 OCFS2 文件系统都有一个 sysfs 目录：)

  /sys/fs/ocfs2/<devname>/filecheck

Here, <devname> indicates the name of OCFS2 volume device which has been already
mounted. The file above would accept inode numbers. This could be used to
communicate with kernel space, tell which file(inode number) will be checked or
fixed. Currently, three operations are supported, which includes checking
inode, fixing inode and setting the size of result record history.
(这里，<devname> 表示已挂载的 OCFS2 卷设备的名称。上述文件接受 inode 号。它可用于与内核空间通信，告知哪个文件(inode 号)将被检查或修复。目前支持三种操作，包括检查 inode、修复 inode 以及设置结果记录历史的大小。)

1. If you want to know what error exactly happened to <inode> before fixing, do::
   (如果你想在修复之前确切了解 <inode> 到底发生了什么错误，请执行：)

    # echo "<inode>" > /sys/fs/ocfs2/<devname>/filecheck/check
    # cat /sys/fs/ocfs2/<devname>/filecheck/check

   The output is like this::
   (输出类似于这样：)

    INO		DONE	ERROR
    39502		1	GENERATION

   <INO> lists the inode numbers.
   <DONE> indicates whether the operation has been finished.
   <ERROR> says what kind of errors was found. For the detailed error numbers,
   please refer to the file linux/fs/ocfs2/filecheck.h.
   (<INO> 列出 inode 号。<DONE> 表示操作是否已完成。<ERROR> 说明发现了何种错误。有关详细的错误号，请参考文件 linux/fs/ocfs2/filecheck.h。)

2. If you determine to fix this inode, do::
   (如果你决定修复此 inode，请执行：)

    # echo "<inode>" > /sys/fs/ocfs2/<devname>/filecheck/fix
    # cat /sys/fs/ocfs2/<devname>/filecheck/fix

   The output is like this::
   (输出类似于这样：)

    INO		DONE	ERROR
    39502		1	SUCCESS

   This time, the <ERROR> column indicates whether this fix is successful or not.
   (这一次，<ERROR> 列指示此修复是否成功。)

3. The record cache is used to store the history of check/fix results. It's
   default size is 10, and can be adjust between the range of 10 ~ 100. You can
   adjust the size like this::
   (记录缓存用于存储检查/修复结果的历史。它的默认大小为 10，可以在 10 ~ 100 的范围内调整。你可以像这样调整大小：)

    # echo "<size>" > /sys/fs/ocfs2/<devname>/filecheck/set

Fixing stuff
(修复相关内容)
============
On receiving the inode, the filesystem would read the inode and the
file metadata. In case of errors, the filesystem would fix the errors
and report the problems it fixed in the kernel log. As a precautionary measure,
the inode must first be checked for errors before performing a final fix.
(在接收到 inode 后，文件系统会读取该 inode 和文件元数据。如果出现错误，文件系统会修复这些错误并在内核日志中报告它所修复的问题。作为预防措施，在执行最终修复之前，必须先检查该 inode 是否有错误。)

The inode and the result history will be maintained temporarily in a
small linked list buffer which would contain the last (N) inodes
fixed/checked, the detailed errors which were fixed/checked are printed in the
kernel log.
(inode 和结果历史会被临时保存在一个小的链表缓冲区中，该缓冲区包含最近(N)个被修复/检查的 inode，被修复/检查的详细错误会打印在内核日志中。)
