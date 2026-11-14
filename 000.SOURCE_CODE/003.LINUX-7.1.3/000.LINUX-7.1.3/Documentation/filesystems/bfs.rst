.. SPDX-License-Identifier: GPL-2.0

========================
BFS Filesystem for Linux
========================

(Linux 的 BFS 文件系统)

The BFS filesystem is used by SCO UnixWare OS for the /stand slice, which
usually contains the kernel image and a few other files required for the
boot process.
(BFS 文件系统被 SCO UnixWare 操作系统用作 /stand 切片，该切片通常包含内核映像以及引导过程所需的其他一些文件。)

In order to access /stand partition under Linux you obviously need to
know the partition number and the kernel must support UnixWare disk slices
(CONFIG_UNIXWARE_DISKLABEL config option). However BFS support does not
depend on having UnixWare disklabel support because one can also mount
BFS filesystem via loopback::
(要在 Linux 下访问 /stand 分区，显然需要知道分区号，并且内核必须支持 UnixWare 磁盘切片(CONFIG_UNIXWARE_DISKLABEL 配置选项)。不过 BFS 支持并不依赖于 UnixWare 磁盘标签支持，因为还可以通过回环方式挂载 BFS 文件系统：)

    # losetup /dev/loop0 stand.img
    # mount -t bfs /dev/loop0 /mnt/stand

where stand.img is a file containing the image of BFS filesystem.
When you have finished using it and umounted you need to also deallocate
/dev/loop0 device by::
(其中 stand.img 是包含 BFS 文件系统映像的文件。使用完毕并卸载后，还需要通过以下方式释放 /dev/loop0 设备：)

    # losetup -d /dev/loop0

You can simplify mounting by just typing::
(可以通过直接输入以下命令来简化挂载：)

    # mount -t bfs -o loop stand.img /mnt/stand

this will allocate the first available loopback device (and load loop.o
kernel module if necessary) automatically. If the loopback driver is not
loaded automatically, make sure that you have compiled the module and
that modprobe is functioning. Beware that umount will not deallocate
/dev/loopN device if /etc/mtab file on your system is a symbolic link to
/proc/mounts. You will need to do it manually using "-d" switch of
losetup(8). Read losetup(8) manpage for more info.
(这将自动分配第一个可用的回环设备(并在必要时加载 loop.o 内核模块)。如果回环驱动程序没有自动加载，请确保已编译该模块并且 modprobe 工作正常。注意，如果系统上的 /etc/mtab 文件是指向 /proc/mounts 的符号链接，umount 将不会释放 /dev/loopN 设备。你需要使用 losetup(8) 的 "-d" 开关手动完成。更多信息请阅读 losetup(8) 手册页。)

To create the BFS image under UnixWare you need to find out first which
slice contains it. The command prtvtoc(1M) is your friend::
(要在 UnixWare 下创建 BFS 映像，首先需要找出哪个切片包含它。命令 prtvtoc(1M) 会帮到你：)

    # prtvtoc /dev/rdsk/c0b0t0d0s0

(assuming your root disk is on target=0, lun=0, bus=0, controller=0). Then you
look for the slice with tag "STAND", which is usually slice 10. With this
information you can use dd(1) to create the BFS image::
((假设你的根磁盘位于 target=0、lun=0、bus=0、controller=0)。然后查找标签为 "STAND" 的切片，它通常是切片 10。有了这些信息，就可以使用 dd(1) 创建 BFS 映像：)

    # umount /stand
    # dd if=/dev/rdsk/c0b0t0d0sa of=stand.img bs=512

Just in case, you can verify that you have done the right thing by checking
the magic number::
(为保险起见，可以通过检查幻数来验证操作是否正确：)

    # od -Ad -tx4 stand.img | more

The first 4 bytes should be 0x1badface.
(前 4 个字节应该是 0x1badface。)

If you have any patches, questions or suggestions regarding this BFS
implementation please contact the author:
(如果你对这个 BFS 实现有任何补丁、问题或建议，请联系作者：)

Tigran Aivazian <aivazian.tigran@gmail.com>
