.. SPDX-License-Identifier: GPL-2.0

===========================
Ramfs, rootfs and initramfs
===========================

(ramfs、rootfs 和 initramfs)

October 17, 2005
(2005年10月17日)

:Author: Rob Landley <rob@landley.net>
(作者: Rob Landley <rob@landley.net>)

What is ramfs?
(什么是 ramfs？)
--------------

Ramfs is a very simple filesystem that exports Linux's disk caching
mechanisms (the page cache and dentry cache) as a dynamically resizable
RAM-based filesystem.
(ramfs 是一个非常简单的文件系统，它把 Linux 的磁盘缓存机制(页缓存和 dentry 缓存)导出为一个可动态调整大小的基于内存(RAM)的文件系统。)

Normally all files are cached in memory by Linux.  Pages of data read from
backing store (usually the block device the filesystem is mounted on) are kept
around in case it's needed again, but marked as clean (freeable) in case the
Virtual Memory system needs the memory for something else.  Similarly, data
written to files is marked clean as soon as it has been written to backing
store, but kept around for caching purposes until the VM reallocates the
memory.  A similar mechanism (the dentry cache) greatly speeds up access to
directories.
(通常，Linux 会把所有文件都缓存在内存中。从后备存储(通常是文件系统所挂载的块设备)读取的数据页会保留在内存中，以备再次需要，但会被标记为干净(可释放)，以防虚拟内存系统出于其他用途需要这些内存。类似地，写入文件的数据一经写入后备存储就会被标记为干净，但仍会保留用于缓存，直到 VM 重新分配该内存。一个类似的机制(dentry 缓存)大大加快了对目录的访问。)

With ramfs, there is no backing store.  Files written into ramfs allocate
dentries and page cache as usual, but there's nowhere to write them to.
This means the pages are never marked clean, so they can't be freed by the
VM when it's looking to recycle memory.
(而对于 ramfs，没有后备存储。写入 ramfs 的文件照常分配 dentry 和页缓存，却没有地方可以把它们写进去。这意味着这些页永远不会被标记为干净，因此当 VM 想要回收内存时也无法释放它们。)

The amount of code required to implement ramfs is tiny, because all the
work is done by the existing Linux caching infrastructure.  Basically,
you're mounting the disk cache as a filesystem.  Because of this, ramfs is not
an optional component removable via menuconfig, since there would be negligible
space savings.
(实现 ramfs 所需的代码量非常小，因为所有工作都由现有的 Linux 缓存基础设施完成。基本上，你就是在把磁盘缓存作为文件系统来挂载。正因如此，ramfs 不是一个可以通过 menuconfig 移除的可选组件，因为这样做节省的空间微乎其微。)

ramfs and ramdisk:
(ramfs 与 ramdisk：)
------------------

The older "ram disk" mechanism created a synthetic block device out of
an area of RAM and used it as backing store for a filesystem.  This block
device was of fixed size, so the filesystem mounted on it was of fixed
size.  Using a ram disk also required unnecessarily copying memory from the
fake block device into the page cache (and copying changes back out), as well
as creating and destroying dentries.  Plus it needed a filesystem driver
(such as ext2) to format and interpret this data.
(较旧的"ram disk"机制用一块内存区域创建出一个合成的块设备，并将它用作文件系统的后备存储。这个块设备大小固定，因此挂载在其上的文件系统大小也固定。使用 ram disk 还需要不必要地把内存从假的块设备拷贝到页缓存中(并把更改拷贝回去)，以及创建和销毁 dentry。此外，它还需要一个文件系统驱动(如 ext2)来格式化和解释这些数据。)

Compared to ramfs, this wastes memory (and memory bus bandwidth), creates
unnecessary work for the CPU, and pollutes the CPU caches.  (There are tricks
to avoid this copying by playing with the page tables, but they're unpleasantly
complicated and turn out to be about as expensive as the copying anyway.)
More to the point, all the work ramfs is doing has to happen _anyway_,
since all file access goes through the page and dentry caches.  The RAM
disk is simply unnecessary; ramfs is internally much simpler.
(与 ramfs 相比，这种方式浪费内存(以及内存总线带宽)，给 CPU 制造不必要的工作，并污染 CPU 缓存。(有一些通过摆弄页表来避免这种拷贝的技巧，但它们复杂得令人不快，而且实际开销与拷贝相比也相差无几。)更重要的是，ramfs 所做的所有工作反正都必须发生，因为所有文件访问都要经过页缓存和 dentry 缓存。RAM disk 完全是多余的；ramfs 在内部要简单得多。)

Another reason ramdisks are semi-obsolete is that the introduction of
loopback devices offered a more flexible and convenient way to create
synthetic block devices, now from files instead of from chunks of memory.
See losetup (8) for details.
(ramdisk 半过时的另一个原因是，回环(loopback)设备的引入提供了一种更灵活、更方便的创建合成块设备的方式，如今是从文件而不是从内存块创建。详情参见 losetup (8)。)

ramfs and tmpfs:
(ramfs 与 tmpfs：)
----------------

One downside of ramfs is you can keep writing data into it until you fill
up all memory, and the VM can't free it because the VM thinks that files
should get written to backing store (rather than swap space), but ramfs hasn't
got any backing store.  Because of this, only root (or a trusted user) should
be allowed write access to a ramfs mount.
(ramfs 的一个缺点是，你可以不断向它写入数据，直到占满全部内存，而 VM 无法释放这些内存，因为 VM 认为文件应当被写入后备存储(而不是交换空间)，但 ramfs 没有任何后备存储。因此，只应允许 root(或受信任的用户)对 ramfs 挂载拥有写权限。)

A ramfs derivative called tmpfs was created to add size limits, and the ability
to write the data to swap space.  Normal users can be allowed write access to
tmpfs mounts.  See Documentation/filesystems/tmpfs.rst for more information.
(为了增加大小限制以及把数据写入交换空间的能力，人们创建了一个名为 tmpfs 的 ramfs 衍生品。可以允许普通用户对 tmpfs 挂载拥有写权限。更多信息参见 Documentation/filesystems/tmpfs.rst。)

What is rootfs?
(什么是 rootfs？)
---------------

Rootfs is a special instance of ramfs (or tmpfs, if that's enabled), which is
always present in Linux systems.  The kernel uses an immutable empty filesystem
called nullfs as the true root of the VFS hierarchy, with the mutable rootfs
(tmpfs/ramfs) mounted on top of it.  This allows pivot_root() and unmounting
of the initramfs to work normally.
(rootfs 是 ramfs(或 tmpfs，如果启用了的话)的一个特殊实例，它总是存在于 Linux 系统中。内核使用一个名为 nullfs 的不可变空文件系统作为 VFS 层级结构的真正根，而可变的 rootfs(tmpfs/ramfs)挂载在它之上。这使得 pivot_root() 和卸载 initramfs 能够正常工作。)

Most systems just mount another filesystem over rootfs and ignore it.  The
amount of space an empty instance of ramfs takes up is tiny.
(大多数系统只是在 rootfs 之上再挂载另一个文件系统，然后忽略它。一个空的 ramfs 实例所占用的空间非常小。)

If CONFIG_TMPFS is enabled, rootfs will use tmpfs instead of ramfs by
default.  To force ramfs, add "rootfstype=ramfs" to the kernel command
line.
(如果启用了 CONFIG_TMPFS，rootfs 默认将使用 tmpfs 而不是 ramfs。要强制使用 ramfs，请向内核命令行添加 "rootfstype=ramfs"。)

What is initramfs?
(什么是 initramfs？)
------------------

All 2.6 Linux kernels contain a gzipped "cpio" format archive, which is
extracted into rootfs when the kernel boots up.  After extracting, the kernel
checks to see if rootfs contains a file "init", and if so it executes it as PID
1.  If found, this init process is responsible for bringing the system the
rest of the way up, including locating and mounting the real root device (if
any).  If rootfs does not contain an init program after the embedded cpio
archive is extracted into it, the kernel will fall through to the older code
to locate and mount a root partition, then exec some variant of /sbin/init
out of that.
(所有 2.6 Linux 内核都包含一个 gzip 压缩的"cpio"格式归档，内核启动时会将其解包到 rootfs 中。解包之后，内核会检查 rootfs 中是否包含名为 "init" 的文件，如果有，就以 PID 1 执行它。如果找到，这个 init 进程负责把系统引导完剩下的部分，包括定位并挂载真正的根设备(如果有的话)。如果嵌入式 cpio 归档解包到 rootfs 之后，rootfs 中不包含 init 程序，内核将转而使用较旧的代码去定位并挂载一个根分区，然后从那里 exec 某种形式的 /sbin/init。)

All this differs from the old initrd in several ways:
(所有这些在好几个方面都与旧的 initrd 不同：)

  - The old initrd was always a separate file, while the initramfs archive is
    linked into the linux kernel image.  (The directory ``linux-*/usr`` is
    devoted to generating this archive during the build.)
    (旧的 initrd 总是一个单独的文件，而 initramfs 归档则链接进 linux 内核映像中。(目录 ``linux-*/usr`` 专门用于在构建过程中生成这个归档。))

  - The old initrd file was a gzipped filesystem image (in some file format,
    such as ext2, that needed a driver built into the kernel), while the new
    initramfs archive is a gzipped cpio archive (like tar only simpler,
    see cpio(1) and Documentation/driver-api/early-userspace/buffer-format.rst).
    The kernel's cpio extraction code is not only extremely small, it's also
    __init text and data that can be discarded during the boot process.
    (旧的 initrd 文件是一个 gzip 压缩的文件系统镜像(采用某种文件格式，如 ext2，需要内核内置驱动)，而新的 initramfs 归档是一个 gzip 压缩的 cpio 归档(类似 tar 只不过更简单，参见 cpio(1) 和 Documentation/driver-api/early-userspace/buffer-format.rst)。内核的 cpio 解包代码不仅极其小巧，它还是可以在引导过程中丢弃的 __init 文本和数据。)

  - The program run by the old initrd (which was called /initrd, not /init) did
    some setup and then returned to the kernel, while the init program from
    initramfs is not expected to return to the kernel.  (If /init needs to hand
    off control it can overmount / with a new root device and exec another init
    program.  See the switch_root utility, below.)
    (由旧 initrd 运行的程序(它叫做 /initrd，而不是 /init)做一些设置然后就返回内核，而来自 initramfs 的 init 程序则不预期会返回内核。(如果 /init 需要交出控制权，它可以在 / 上覆盖挂载一个新的根设备并 exec 另一个 init 程序。参见下文的 switch_root 工具。))

  - When switching another root device, initrd would pivot_root and then
    umount the ramdisk.  With nullfs as the true root, pivot_root() works
    normally from the initramfs.  Userspace can simply do::
    (在切换到另一个根设备时，initrd 会先 pivot_root 然后 umount 掉 ramdisk。有了 nullfs 作为真正的根，pivot_root() 在 initramfs 中可以正常工作。用户空间可以简单地这样做：)

      chdir(new_root);
      pivot_root(".", ".");
      umount2(".", MNT_DETACH);

    This is the preferred method for switching root filesystems.
    (这是切换根文件系统的首选方法。)

Populating initramfs:
(填充 initramfs：)
---------------------

The 2.6 kernel build process always creates a gzipped cpio format initramfs
archive and links it into the resulting kernel binary.  By default, this
archive is empty (consuming 134 bytes on x86).
(2.6 内核的构建过程总是会创建一个 gzip 压缩的 cpio 格式 initramfs 归档，并将其链接进最终的内核二进制中。默认情况下，这个归档是空的(在 x86 上占用 134 字节)。)

The config option CONFIG_INITRAMFS_SOURCE (in General Setup in menuconfig,
and living in usr/Kconfig) can be used to specify a source for the
initramfs archive, which will automatically be incorporated into the
resulting binary.  This option can point to an existing gzipped cpio
archive, a directory containing files to be archived, or a text file
specification such as the following example::
(配置选项 CONFIG_INITRAMFS_SOURCE(在 menuconfig 的 General Setup 中，位于 usr/Kconfig)可用于指定 initramfs 归档的来源，它会自动并入最终的二进制文件中。该选项可以指向一个已有的 gzip 压缩 cpio 归档、一个包含待归档文件的目录，或者一个文本文件规格说明，例如下面这个例子：)

  dir /dev 755 0 0
  nod /dev/console 644 0 0 c 5 1
  nod /dev/loop0 644 0 0 b 7 0
  dir /bin 755 1000 1000
  slink /bin/sh busybox 777 0 0
  file /bin/busybox initramfs/busybox 755 0 0
  dir /proc 755 0 0
  dir /sys 755 0 0
  dir /mnt 755 0 0
  file /init initramfs/init.sh 755 0 0

Run "usr/gen_init_cpio" (after the kernel build) to get a usage message
documenting the above file format.
(运行 "usr/gen_init_cpio"(在内核构建之后)可以得到一个描述上述文件格式的用法消息。)

One advantage of the configuration file is that root access is not required to
set permissions or create device nodes in the new archive.  (Note that those
two example "file" entries expect to find files named "init.sh" and "busybox" in
a directory called "initramfs", under the linux-2.6.* directory.  See
Documentation/driver-api/early-userspace/early_userspace_support.rst for more details.)
(配置文件的一个优点是，在新归档中设置权限或创建设备节点不需要 root 权限。(注意，这两个示例 "file" 表项期望在 linux-2.6.* 目录下一个名为 "initramfs" 的目录中找到名为 "init.sh" 和 "busybox" 的文件。更多细节参见 Documentation/driver-api/early-userspace/early_userspace_support.rst。))

The kernel does not depend on external cpio tools.  If you specify a
directory instead of a configuration file, the kernel's build infrastructure
creates a configuration file from that directory (usr/Makefile calls
usr/gen_initramfs.sh), and proceeds to package up that directory
using the config file (by feeding it to usr/gen_init_cpio, which is created
from usr/gen_init_cpio.c).  The kernel's build-time cpio creation code is
entirely self-contained, and the kernel's boot-time extractor is also
(obviously) self-contained.
(内核不依赖外部的 cpio 工具。如果你指定的是一个目录而不是配置文件，内核的构建基础设施会从该目录生成一个配置文件(usr/Makefile 调用 usr/gen_initramfs.sh)，然后使用该配置文件打包这个目录(通过把它喂给 usr/gen_init_cpio，后者由 usr/gen_init_cpio.c 创建)。内核构建时的 cpio 创建代码是完全自包含的，内核引导时的解包器显然也是自包含的。)

The one thing you might need external cpio utilities installed for is creating
or extracting your own preprepared cpio files to feed to the kernel build
(instead of a config file or directory).
(你可能需要安装外部 cpio 工具的唯一情形是，创建或解包你自己预先准备好的 cpio 文件以供内核构建使用(代替配置文件或目录)。)

The following command line can extract a cpio image (either by the above script
or by the kernel build) back into its component files::
(下面的命令行可以把一个 cpio 镜像(无论是通过上述脚本还是通过内核构建产生的)解包回其组成文件：)

  cpio -i -d -H newc -F initramfs_data.cpio --no-absolute-filenames

The following shell script can create a prebuilt cpio archive you can
use in place of the above config file::
(下面的 shell 脚本可以创建一个预构建的 cpio 归档，你可以用它来代替上面的配置文件：)

  #!/bin/sh

  # Copyright 2006 Rob Landley <rob@landley.net> and TimeSys Corporation.
  # Licensed under GPL version 2

  if [ $# -ne 2 ]
  then
    echo "usage: mkinitramfs directory imagename.cpio.gz"
    exit 1
  fi

  if [ -d "$1" ]
  then
    echo "creating $2 from $1"
    (cd "$1"; find . | cpio -o -H newc | gzip) > "$2"
  else
    echo "First argument must be a directory"
    exit 1
  fi

.. Note::

   The cpio man page contains some bad advice that will break your initramfs
   archive if you follow it.  It says "A typical way to generate the list
   of filenames is with the find command; you should give find the -depth
   option to minimize problems with permissions on directories that are
   unwritable or not searchable."  Don't do this when creating
   initramfs.cpio.gz images, it won't work.  The Linux kernel cpio extractor
   won't create files in a directory that doesn't exist, so the directory
   entries must go before the files that go in those directories.
   The above script gets them in the right order.
   (cpio 的 man 手册页包含一些糟糕的建议，如果你照做，会破坏你的 initramfs 归档。它说："生成文件名列表的典型方法是使用 find 命令；你应当给 find 传递 -depth 选项，以尽量减少目录不可写或不可搜索带来的权限问题。"在创建 initramfs.cpio.gz 镜像时不要这样做，它不会起作用。Linux 内核的 cpio 解包器不会在尚不存在的目录中创建文件，因此目录表项必须放在进入这些目录的文件之前。上面的脚本把它们排成了正确的顺序。)

External initramfs images:
(外部 initramfs 镜像：)
--------------------------

If the kernel has initrd support enabled, an external cpio.gz archive can also
be passed into a 2.6 kernel in place of an initrd.  In this case, the kernel
will autodetect the type (initramfs, not initrd) and extract the external cpio
archive into rootfs before trying to run /init.
(如果内核启用了 initrd 支持，也可以把一个外部的 cpio.gz 归档代替 initrd 传给 2.6 内核。在这种情况下，内核会自动检测类型(initramfs，而不是 initrd)，并在尝试运行 /init 之前把这个外部 cpio 归档解包到 rootfs 中。)

This has the memory efficiency advantages of initramfs (no ramdisk block
device) but the separate packaging of initrd (which is nice if you have
non-GPL code you'd like to run from initramfs, without conflating it with
the GPL licensed Linux kernel binary).
(这种方式兼具 initramfs 的内存效率优点(没有 ramdisk 块设备)和 initrd 的独立打包优点(如果你有一些想从 initramfs 运行、又不想与采用 GPL 许可的 Linux 内核二进制混在一起的非 GPL 代码，这会很有用)。)

It can also be used to supplement the kernel's built-in initramfs image.  The
files in the external archive will overwrite any conflicting files in
the built-in initramfs archive.  Some distributors also prefer to customize
a single kernel image with task-specific initramfs images, without recompiling.
(它也可以用来补充内核内置的 initramfs 镜像。外部归档中的文件会覆盖内置 initramfs 归档中任何与之冲突的文件。一些发行版厂商还喜欢单独用一个内核镜像配合针对特定任务的 initramfs 镜像进行定制，而无需重新编译。)

Contents of initramfs:
(initramfs 的内容：)
----------------------

An initramfs archive is a complete self-contained root filesystem for Linux.
If you don't already understand what shared libraries, devices, and paths
you need to get a minimal root filesystem up and running, here are some
references:
(initramfs 归档是一个为 Linux 准备的完整的、自包含的根文件系统。如果你还不了解要让一个最小根文件系统启动并运行需要哪些共享库、设备和路径，这里有一些参考资料：)

- https://www.tldp.org/HOWTO/Bootdisk-HOWTO/
- https://www.tldp.org/HOWTO/From-PowerUp-To-Bash-Prompt-HOWTO.html
- http://www.linuxfromscratch.org/lfs/view/stable/

The "klibc" package (https://www.kernel.org/pub/linux/libs/klibc) is
designed to be a tiny C library to statically link early userspace
code against, along with some related utilities.  It is BSD licensed.
("klibc" 软件包(https://www.kernel.org/pub/linux/libs/klibc)旨在成为一个微小的 C 库，供早期用户空间代码静态链接使用，另外还附带一些相关工具。它采用 BSD 许可。)

I use uClibc (https://www.uclibc.org) and busybox (https://www.busybox.net)
myself.  These are LGPL and GPL, respectively.  (A self-contained initramfs
package is planned for the busybox 1.3 release.)
(我自己使用的是 uClibc(https://www.uclibc.org)和 busybox(https://www.busybox.net)。它们分别采用 LGPL 和 GPL。(busybox 1.3 版本计划包含一个自包含的 initramfs 软件包。))

In theory you could use glibc, but that's not well suited for small embedded
uses like this.  (A "hello world" program statically linked against glibc is
over 400k.  With uClibc it's 7k.  Also note that glibc dlopens libnss to do
name lookups, even when otherwise statically linked.)
(理论上你可以使用 glibc，但它并不很适合这样的小型嵌入式用途。("hello world" 程序与 glibc 静态链接后超过 400k。而使用 uClibc 只有 7k。还要注意，glibc 会 dlopen libnss 来进行名字查找，即使其他部分是静态链接的。))

A good first step is to get initramfs to run a statically linked "hello world"
program as init, and test it under an emulator like qemu (www.qemu.org) or
User Mode Linux, like so::
(一个好的第一步是让 initramfs 以一个静态链接的 "hello world" 程序作为 init 运行，并在 qemu(www.qemu.org)或 User Mode Linux 之类的模拟器下测试它，如下所示：)

  cat > hello.c << EOF
  #include <stdio.h>
  #include <unistd.h>

  int main(int argc, char *argv[])
  {
    printf("Hello world!\n");
    sleep(999999999);
  }
  EOF
  gcc -static hello.c -o init
  echo init | cpio -o -H newc | gzip > test.cpio.gz
  # Testing external initramfs using the initrd loading mechanism.
  qemu -kernel /boot/vmlinuz -initrd test.cpio.gz /dev/zero

When debugging a normal root filesystem, it's nice to be able to boot with
"init=/bin/sh".  The initramfs equivalent is "rdinit=/bin/sh", and it's
just as useful.
(在调试普通根文件系统时，能够用 "init=/bin/sh" 启动是很有用的。initramfs 中的等价物是 "rdinit=/bin/sh"，它同样有用。)

Why cpio rather than tar?
(为什么用 cpio 而不是 tar？)
-------------------------

This decision was made back in December, 2001.  The discussion started here:
(这个决定是在 2001 年 12 月做出的。讨论从这里开始：)

- https://lore.kernel.org/lkml/a03cke$640$1@cesium.transmeta.com/

And spawned a second thread (specifically on tar vs cpio), starting here:
(并衍生出第二个讨论串(专门讨论 tar 与 cpio 的对比)，从这里开始：)

- https://lore.kernel.org/lkml/3C25A06D.7030408@zytor.com/

The quick and dirty summary version (which is no substitute for reading
the above threads) is:
(一个简陋而快速的摘要版本(它不能替代阅读上面那些讨论串)如下：)

1) cpio is a standard.  It's decades old (from the AT&T days), and already
   widely used on Linux (inside RPM, Red Hat's device driver disks).  Here's
   a Linux Journal article about it from 1996:
   (cpio 是一个标准。它已有几十年的历史(源自 AT&T 时代)，并已在 Linux 上广泛使用(位于 RPM、Red Hat 的设备驱动光盘之中)。下面是 1996 年一篇关于它的 Linux Journal 文章：)

      http://www.linuxjournal.com/article/1213

   It's not as popular as tar because the traditional cpio command line tools
   require _truly_hideous_ command line arguments.  But that says nothing
   either way about the archive format, and there are alternative tools,
   such as:
   (它之所以不如 tar 流行，是因为传统的 cpio 命令行工具需要_极其丑陋_的命令行参数。但这对归档格式本身没有任何影响，而且还有一些替代工具，例如：)

      https://linux.die.net/man/1/afio

2) The cpio archive format chosen by the kernel is simpler and cleaner (and
   thus easier to create and parse) than any of the (literally dozens of)
   various tar archive formats.  The complete initramfs archive format is
   explained in buffer-format.rst, created in usr/gen_init_cpio.c, and
   extracted in init/initramfs.c.  All three together come to less than 26k
   total of human-readable text.
   (内核所选用的 cpio 归档格式比(确实有几十种之多的)各种 tar 归档格式中的任何一种都更简单、更干净(因而也更容易创建和解析)。完整的 initramfs 归档格式在 buffer-format.rst 中有说明，创建于 usr/gen_init_cpio.c，解包于 init/initramfs.c。三者加起来总共不到 26k 的人类可读文本。)

3) The GNU project standardizing on tar is approximately as relevant as
   Windows standardizing on zip.  Linux is not part of either, and is free
   to make its own technical decisions.
   (GNU 项目标准化于 tar，这与 Windows 标准化于 zip 的重要性差不多。Linux 不属于其中任何一方，可以自由地做出自己的技术决定。)

4) Since this is a kernel internal format, it could easily have been
   something brand new.  The kernel provides its own tools to create and
   extract this format anyway.  Using an existing standard was preferable,
   but not essential.
   (由于这是一个内核内部格式，它本来完全可以是某种全新的东西。反正内核都提供了自己的工具来创建和解包这种格式。采用一个现有的标准更可取，但并非必需。)

5) Al Viro made the decision (quote: "tar is ugly as hell and not going to be
   supported on the kernel side"):
   (是 Al Viro 做出的这个决定(原话："tar 丑得要命，内核一侧不会支持它")：)

    - https://lore.kernel.org/lkml/Pine.GSO.4.21.0112222109050.21702-100000@weyl.math.psu.edu/

   explained his reasoning:
   (并解释了他的理由：)

    - https://lore.kernel.org/lkml/Pine.GSO.4.21.0112222240530.21702-100000@weyl.math.psu.edu/
    - https://lore.kernel.org/lkml/Pine.GSO.4.21.0112230849550.23300-100000@weyl.math.psu.edu/

   and, most importantly, designed and implemented the initramfs code.
   (而且，最重要的是，设计并实现了 initramfs 代码。)

Future directions:
(未来的方向：)
------------------

Today (2.6.16), initramfs is always compiled in, but not always used.  The
kernel falls back to legacy boot code that is reached only if initramfs does
not contain an /init program.  The fallback is legacy code, there to ensure a
smooth transition and allowing early boot functionality to gradually move to
"early userspace" (I.E. initramfs).
(如今(2.6.16)，initramfs 总是被编译进内核，但并不总是被使用。只有当 initramfs 中不包含 /init 程序时，内核才会回退到那些旧式引导代码。这种回退是遗留代码，它的存在是为了确保平稳过渡，并让早期引导功能逐步迁移到"早期用户空间"(即 initramfs)。)

The move to early userspace is necessary because finding and mounting the real
root device is complex.  Root partitions can span multiple devices (raid or
separate journal).  They can be out on the network (requiring dhcp, setting a
specific MAC address, logging into a server, etc).  They can live on removable
media, with dynamically allocated major/minor numbers and persistent naming
issues requiring a full udev implementation to sort out.  They can be
compressed, encrypted, copy-on-write, loopback mounted, strangely partitioned,
and so on.
(迁移到早期用户空间是必要的，因为定位并挂载真正的根设备非常复杂。根分区可以横跨多个设备(raid 或独立日志)。它们可以位于网络上(需要 dhcp、设置特定的 MAC 地址、登录服务器等)。它们可以放在可移动介质上，主/次设备号动态分配，持久化命名问题需要完整的 udev 实现才能理清。它们还可以是压缩的、加密的、写时复制的、回环挂载的、采用奇怪分区方式的，等等。)

This kind of complexity (which inevitably includes policy) is rightly handled
in userspace.  Both klibc and busybox/uClibc are working on simple initramfs
packages to drop into a kernel build.
(这类复杂性(其中不可避免地包含策略)理应在用户空间中处理。klibc 和 busybox/uClibc 都在开发可以放进内核构建中的简单 initramfs 软件包。)

The klibc package has now been accepted into Andrew Morton's 2.6.17-mm tree.
The kernel's current early boot code (partition detection, etc) will probably
be migrated into a default initramfs, automatically created and used by the
kernel build.
(klibc 软件包现在已被接受进入 Andrew Morton 的 2.6.17-mm 树中。内核当前的早期引导代码(分区检测等)很可能会被迁移到一个默认的 initramfs 中，由内核构建自动创建并使用。)
