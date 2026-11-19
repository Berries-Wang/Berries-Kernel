.. _changes:

++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
Minimal requirements to compile the Kernel(编译内核的最低要求)
++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

Intro(引言)
===========

This document is designed to provide a list of the minimum levels of
software necessary to run the current kernel version.
(本文档旨在列出运行当前内核版本所需的软件的最低版本级别。)

This document is originally based on my "Changes" file for 2.0.x kernels
and therefore owes credit to the same people as that file (Jared Mauch,
Axel Boldt, Alessandro Sigala, and countless other users all over the
'net).
(本文档最初基于我针对 2.0.x 内核的 "Changes" 文件，因此要感谢与该文件相同
的一批人(Jared Mauch、Axel Boldt、Alessandro Sigala，以及"网络"上数不清的
其他用户)。)

Current Minimal Requirements(当前最低要求)
******************************************

Upgrade to at **least** these software revisions before thinking you've
encountered a bug!  If you're unsure what version you're currently
running, the suggested command should tell you. For a list of the programs
on your system including their version execute ./scripts/ver_linux
(在你认为自己遇到了缺陷之前，请先把软件升级到**至少**这些版本！如果你不确
定自己当前运行的版本，建议的命令会告诉你。要列出系统中所有程序及其版本，
可执行 ./scripts/ver_linux。)

Again, keep in mind that this list assumes you are already functionally
running a Linux kernel.  Also, not all tools are necessary on all
systems; obviously, if you don't have any PC Card hardware, for example,
you probably do not need to concern yourself with pcmciautils.
(再次提醒，这份清单假定你已经能正常运行 Linux 内核。另外，并非所有系统都
需要所有工具；显然，例如如果你没有任何 PC Card 硬件，多半无需关心
pcmciautils。)

====================== ===============  ========================================
        Program        Minimal version       Command to check the version
(程序)                   (最低版本)             (检查版本的命令)
====================== ===============  ========================================
bash                   4.2              bash --version
bc                     1.06.95          bc --version
bindgen (optional)     0.71.1           bindgen --version
binutils               2.30             ld -v
bison                  2.0              bison --version
btrfs-progs            0.18             btrfs --version
Clang/LLVM (optional)  15.0.0           clang --version
e2fsprogs              1.41.4           e2fsck -V
flex                   2.5.35           flex --version
gdb                    7.2              gdb --version
GNU awk (optional)     5.1.0            gawk --version
GNU C                  8.1              gcc --version
GNU make               4.0              make --version
GNU tar                1.28             tar --version
GRUB                   0.93             grub --version || grub-install --version
gtags (optional)       6.6.5            gtags --version
iptables               1.4.2            iptables -V
jfsutils               1.1.3            fsck.jfs -V
kmod                   13               kmod -V
mcelog                 0.6              mcelog --version
mkimage (optional)     2017.01          mkimage --version
nfs-utils              1.0.5            showmount --version
openssl & libcrypto    1.0.0            openssl version
pahole                 1.22             pahole --version
pcmciautils            004              pccardctl -V
PPP                    2.4.0            pppd --version
procps                 3.2.0            ps --version
Python                 3.9.x            python3 --version
quota-tools            3.09             quota -V
Rust (optional)        1.85.0           rustc --version
Sphinx\ [#f1]_         3.4.3            sphinx-build --version
squashfs-tools         4.0              mksquashfs -version
udev                   081              udevadm --version
util-linux             2.10o            mount --version
xfsprogs               2.6.0            xfs_db -V
====================== ===============  ========================================

.. [#f1] Sphinx is needed only to build the Kernel documentation
         (Sphinx 只在构建内核文档时才需要。)

Kernel compilation(内核编译)
*****************************

GCC(GNU C 编译器)
-----------------

The gcc version requirements may vary depending on the type of CPU in your
computer.
(gcc 版本要求可能因你计算机中 CPU 的类型而异。)

Clang/LLVM (optional)(Clang/LLVM，可选)
---------------------------------------

The latest formal release of clang and LLVM utils (according to
`releases.llvm.org <https://releases.llvm.org>`_) are supported for building
kernels. Older releases aren't guaranteed to work, and we may drop workarounds
from the kernel that were used to support older versions. Please see additional
docs on :ref:`Building Linux with Clang/LLVM <kbuild_llvm>`.
(支持用最新正式发布的 clang 与 LLVM 工具(依据
`releases.llvm.org <https://releases.llvm.org>`_)构建内核。更老的版本不保证
可用，而且内核中用于支持旧版本的变通代码可能会被移除。更多文档请参见
:ref:`Building Linux with Clang/LLVM <kbuild_llvm>`。)

Rust (optional)(Rust，可选)
---------------------------

A recent version of the Rust compiler is required.
(需要较新版本的 Rust 编译器。)

Please see Documentation/rust/quick-start.rst for instructions on how to
satisfy the build requirements of Rust support. In particular, the ``Makefile``
target ``rustavailable`` is useful to check why the Rust toolchain may not
be detected.
(如何满足 Rust 支持的构建要求，请参见
Documentation/rust/quick-start.rst。特别是，``Makefile`` 目标
``rustavailable`` 有助于排查 Rust 工具链为何未被检测到。)

bindgen (optional)(bindgen，可选)
---------------------------------

``bindgen`` is used to generate the Rust bindings to the C side of the kernel.
It depends on ``libclang``.
(``bindgen`` 用于生成内核 C 侧的 Rust 绑定。它依赖 ``libclang``。)

Make(make 工具)
---------------

You will need GNU make 4.0 or later to build the kernel.
(构建内核需要 GNU make 4.0 或更高版本。)

Bash(bash shell 解释器)
-----------------------

Some bash scripts are used for the kernel build.
Bash 4.2 or newer is needed.
(内核构建会用到一些 bash 脚本。需要 Bash 4.2 或更新版本。)

Binutils(binutils 工具集)
-------------------------

Binutils 2.30 or newer is needed to build the kernel.
(构建内核需要 Binutils 2.30 或更新版本。)

pkg-config(pkg-config 工具)
---------------------------

The build system, as of 4.18, requires pkg-config to check for installed
kconfig tools and to determine flags settings for use in
'make {g,x}config'.  Previously pkg-config was being used but not
verified or documented.
(自 4.18 起，构建系统要求有 pkg-config，用于检查已安装的 kconfig 工具，并
确定 'make {g,x}config' 所用的标志设置。以前也在用 pkg-config，但既未校验
也未记载。)

Flex(flex 词法分析器)
---------------------

Since Linux 4.16, the build system generates lexical analyzers
during build.  This requires flex 2.5.35 or later.
(自 Linux 4.16 起，构建系统会在构建期间生成词法分析器。这要求 flex 2.5.35
或更新版本。)


Bison(bison 语法分析器)
-----------------------

Since Linux 4.16, the build system generates parsers
during build.  This requires bison 2.0 or later.
(自 Linux 4.16 起，构建系统会在构建期间生成语法分析器。这要求 bison 2.0
或更新版本。)

pahole(pahole 工具)
-------------------

Since Linux 5.2, if CONFIG_DEBUG_INFO_BTF is selected, the build system
generates BTF (BPF Type Format) from DWARF in vmlinux, a bit later from kernel
modules as well.  This requires pahole v1.22 or later.
(自 Linux 5.2 起，如果选择了 CONFIG_DEBUG_INFO_BTF，构建系统会从 vmlinux
的 DWARF 信息生成 BTF (BPF Type Format)，稍后也会从内核模块生成。这要求
pahole v1.22 或更新版本。)

It is found in the 'dwarves' or 'pahole' distro packages or from
https://fedorapeople.org/~acme/dwarves/.
(它位于发行版的 'dwarves' 或 'pahole' 软件包中，或可从
https://fedorapeople.org/~acme/dwarves/ 获取。)

Perl(Perl 语言)
---------------

You will need perl 5 and the following modules: ``Getopt::Long``,
``Getopt::Std``, ``File::Basename``, and ``File::Find`` to build the kernel.
(构建内核需要 perl 5 以及以下模块：``Getopt::Long`` 、 ``Getopt::Std`` 、
``File::Basename`` 和 ``File::Find`` 。)

Python(Python 语言)
-------------------

Several config options require it: it is required for arm/arm64
default configs, CONFIG_LTO_CLANG, some DRM optional configs,
the kernel-doc tool, and docs build (Sphinx), among others.
(若干配置选项需要它：其中包括 arm/arm64 默认配置、CONFIG_LTO_CLANG、一
些 DRM 可选配置、kernel-doc 工具以及文档构建(Sphinx)等。)

BC(bc 计算器)
-------------

You will need bc to build kernels 3.10 and higher
(构建 3.10 及更高版本的内核需要 bc。)


OpenSSL(OpenSSL 库)
-------------------

Module signing and external certificate handling use the OpenSSL program and
crypto library to do key creation and signature generation.
(模块签名与外部证书处理使用 OpenSSL 程序和加密库来创建密钥和生成签名。)

You will need openssl to build kernels 3.7 and higher if module signing is
enabled.  You will also need openssl development packages to build kernels 4.3
and higher.
(如果启用了模块签名，构建 3.7 及更高版本的内核需要 openssl。构建 4.3 及更
高版本的内核还需要 openssl 开发包。)

Tar(tar 工具)
-------------

GNU tar is needed if you want to enable access to the kernel headers via sysfs
(CONFIG_IKHEADERS).
(如果你想启用通过 sysfs 访问内核头文件(CONFIG_IKHEADERS)，需要 GNU tar。)

gtags / GNU GLOBAL (optional)(gtags / GNU GLOBAL，可选)
-------------------------------------------------------

The kernel build requires GNU GLOBAL version 6.6.5 or later to generate
tag files through ``make gtags``.  This is due to its use of the gtags
``-C (--directory)`` flag.
(内核构建需要 GNU GLOBAL 6.6.5 或更高版本，以便通过 ``make gtags`` 生成
tag 文件。原因是它用到了 gtags 的 ``-C (--directory)`` 标志。)

mkimage(mkimage 工具)
---------------------

This tool is used when building a Flat Image Tree (FIT), commonly used on ARM
platforms. The tool is available via the ``u-boot-tools`` package or can be
built from the U-Boot source code. See the instructions at
https://docs.u-boot.org/en/latest/build/tools.html#building-tools-for-linux
(构建 Flat Image Tree (FIT，ARM 平台常用)时会用到该工具。该工具可通过
``u-boot-tools`` 软件包获得，或从 U-Boot 源码构建。操作说明见
https://docs.u-boot.org/en/latest/build/tools.html#building-tools-for-linux。)

GNU AWK(GNU AWK 工具)
---------------------

GNU AWK is needed if you want kernel builds to generate address range data for
builtin modules (CONFIG_BUILTIN_MODULE_RANGES).
(如果你想让内核构建为内建模块生成地址范围数据
(CONFIG_BUILTIN_MODULE_RANGES)，需要 GNU AWK。)

System utilities(系统实用工具)
******************************

Architectural changes(架构变化)
-------------------------------

DevFS has been obsoleted in favour of udev
(https://www.kernel.org/pub/linux/utils/kernel/hotplug/)
(DevFS 已被淘汰，由 udev 取代
(https://www.kernel.org/pub/linux/utils/kernel/hotplug/)。)

32-bit UID support is now in place.  Have fun!
(32 位 UID 支持已经就位。祝玩得开心！)

Linux documentation for functions is transitioning to inline
documentation via specially-formatted comments near their
definitions in the source.  These comments can be combined with ReST
files in the Documentation/ directory to make enriched documentation, which can
then be converted to PostScript, HTML, LaTex, ePUB and PDF files.
In order to convert from ReST format to a format of your choice, you'll need
Sphinx.
(Linux 的函数文档正在转向内联文档形式：在源代码中函数定义附近使用特殊格
式的注释。这些注释可与 Documentation/ 目录中的 ReST 文件结合，构成内容更
丰富的文档，进而可转换为 PostScript、HTML、LaTex、ePUB 和 PDF 文件。要把
ReST 格式转换成你选择的格式，需要 Sphinx。)

Util-linux(util-linux 工具)
---------------------------

New versions of util-linux provide ``fdisk`` support for larger disks,
support new options to mount, recognize more supported partition
types, and similar goodies.
You'll probably want to upgrade.
(新版 util-linux 为更大的磁盘提供 ``fdisk`` 支持，为 mount 增加新选项，能
识别更多受支持的分区类型，诸如此类的改进。你多半会想升级。)

Ksymoops(ksymoops 工具)
-----------------------

If the unthinkable happens and your kernel oopses, you may need the
ksymoops tool to decode it, but in most cases you don't.
It is generally preferred to build the kernel with ``CONFIG_KALLSYMS`` so
that it produces readable dumps that can be used as-is (this also
produces better output than ksymoops).  If for some reason your kernel
is not build with ``CONFIG_KALLSYMS`` and you have no way to rebuild and
reproduce the Oops with that option, then you can still decode that Oops
with ksymoops.
(如果发生不可思议的事、你的内核 oops 了，你可能需要 ksymoops 工具来解码
它，但多数情况下不需要。通常更倾向于以 ``CONFIG_KALLSYMS`` 构建内核，使
其产生可直接阅读的转储(其输出也比 ksymoops 更好)。如果由于某种原因你的内
核构建时没有开 ``CONFIG_KALLSYMS``，而你又无法以该选项重新构建并复现这个
Oops，那么你仍然可以用 ksymoops 来解码该 Oops。)

Mkinitrd(mkinitrd 工具)
-----------------------

These changes to the ``/lib/modules`` file tree layout also require that
mkinitrd be upgraded.
(``/lib/modules`` 文件树布局的这些变化也要求升级 mkinitrd。)

E2fsprogs(e2fsprogs 工具)
-------------------------

The latest version of ``e2fsprogs`` fixes several bugs in fsck and
debugfs.  Obviously, it's a good idea to upgrade.
(最新版 ``e2fsprogs`` 修复了 fsck 与 debugfs 中的若干缺陷。显然，升级是个
好主意。)

JFSutils(jfsutils 工具)
-----------------------

The ``jfsutils`` package contains the utilities for the file system.
The following utilities are available:
(``jfsutils`` 软件包包含该文件系统的实用工具。可用的工具有：)

- ``fsck.jfs`` - initiate replay of the transaction log, and check
  and repair a JFS formatted partition.
  (``fsck.jfs`` - 启动事务日志的重放，检查并修复 JFS 格式化的分区。)

- ``mkfs.jfs`` - create a JFS formatted partition.
  (``mkfs.jfs`` - 创建 JFS 格式化的分区。)

- other file system utilities are also available in this package.
  (该软件包中还提供其他文件系统实用工具。)

Xfsprogs(xfsprogs 工具)
-----------------------

The latest version of ``xfsprogs`` contains ``mkfs.xfs``, ``xfs_db``, and the
``xfs_repair`` utilities, among others, for the XFS filesystem.  It is
architecture independent and any version from 2.0.0 onward should
work correctly with this version of the XFS kernel code (2.6.0 or
later is recommended, due to some significant improvements).
(最新版 ``xfsprogs`` 包含 XFS 文件系统的 ``mkfs.xfs``、``xfs_db`` 和
``xfs_repair`` 等实用工具。它与架构无关，2.0.0 及以后的任何版本都应当能与
这一版的 XFS 内核代码正确配合(由于有一些显著改进，推荐 2.6.0 或更新版本)。)

PCMCIAutils(pcmciautils 工具)
-----------------------------

PCMCIAutils replaces ``pcmcia-cs``. It properly sets up
PCMCIA sockets at system startup and loads the appropriate modules
for 16-bit PCMCIA devices if the kernel is modularized and the hotplug
subsystem is used.
(PCMCIAutils 取代 ``pcmcia-cs``。如果内核被模块化且使用热插拔子系统，它会
在系统启动时正确设置 PCMCIA 套接字，并为 16 位 PCMCIA 设备加载相应的模
块。)

Quota-tools(quota-tools 工具)
-----------------------------

Support for 32 bit uid's and gid's is required if you want to use
the newer version 2 quota format.  Quota-tools version 3.07 and
newer has this support.  Use the recommended version or newer
from the table above.
(如果想使用较新的 version 2 配额格式，需要支持 32 位 uid 与 gid。
quota-tools 3.07 及更新版本具有该支持。请使用上表推荐的版本或更新版本。)

Intel IA32 microcode(Intel IA32 微码)
-------------------------------------

A driver has been added to allow updating of Intel IA32 microcode,
accessible as a normal (misc) character device.  If you are not using
udev you may need to::

  mkdir /dev/cpu
  mknod /dev/cpu/microcode c 10 184
  chmod 0644 /dev/cpu/microcode

(新增了一个驱动，允许更新 Intel IA32 微码，它以普通 (misc) 字符设备的形
式提供。如果你没有使用 udev，可能需要先以 root 执行上面的命令。)

as root before you can use this.  You'll probably also want to
get the user-space microcode_ctl utility to use with this.
(之后才能使用它。你可能还会想获取用户空间的 microcode_ctl 工具来配合使
用。)

udev(udev 设备管理器)
---------------------

``udev`` is a userspace application for populating ``/dev`` dynamically with
only entries for devices actually present. ``udev`` replaces the basic
functionality of devfs, while allowing persistent device naming for
devices.
(``udev`` 是一个用户空间应用，用于动态填充 ``/dev``，只保留实际存在的设
备条目。``udev`` 取代了 devfs 的基本功能，同时允许对设备进行持久化命名。)

FUSE(FUSE 文件系统)
-------------------

Needs libfuse 2.4.0 or later.  Absolute minimum is 2.3.0 but mount
options ``direct_io`` and ``kernel_cache`` won't work.
(需要 libfuse 2.4.0 或更高版本。绝对最低是 2.3.0，但挂载选项
``direct_io`` 与 ``kernel_cache`` 将无法使用。)

Networking(网络)
****************

General changes(一般变化)
-------------------------

If you have advanced network configuration needs, you should probably
consider using the network tools from ip-route2.
(如果你有高级网络配置需求，大概应当考虑使用来自 ip-route2 的网络工具。)

Packet Filter / NAT(包过滤 / NAT)
---------------------------------
The packet filtering and NAT code uses the same tools like the previous 2.4.x
kernel series (iptables).  It still includes backwards-compatibility modules
for 2.2.x-style ipchains and 2.0.x-style ipfwadm.
(包过滤与 NAT 代码使用与之前 2.4.x 内核系列相同的工具(iptables)。它仍然
包含面向 2.2.x 风格 ipchains 与 2.0.x 风格 ipfwadm 的向后兼容模块。)

PPP(PPP 协议)
-------------

The PPP driver has been restructured to support multilink and to
enable it to operate over diverse media layers.  If you use PPP,
upgrade pppd to at least 2.4.0.
(PPP 驱动已重构以支持多链路 (multilink)，并使其能在多样的介质层上工作。
如果你使用 PPP，请把 pppd 升级到至少 2.4.0。)

If you are not using udev, you must have the device file /dev/ppp
which can be made by::

  mknod /dev/ppp c 108 0

(如果你没有使用 udev，则必须有设备文件 /dev/ppp，可以用上面的命令创建。)

as root.
(以 root 身份执行即可。)

NFS-utils(nfs-utils 工具)
-------------------------

In ancient (2.4 and earlier) kernels, the nfs server needed to know
about any client that expected to be able to access files via NFS.  This
information would be given to the kernel by ``mountd`` when the client
mounted the filesystem, or by ``exportfs`` at system startup.  exportfs
would take information about active clients from ``/var/lib/nfs/rmtab``.
(在古老的(2.4 及更早)内核中，NFS 服务器需要知道任何希望通过 NFS 访问文
件的客户端。这些信息在客户端挂载文件系统时由 ``mountd`` 提供给内核，或在
系统启动时由 ``exportfs`` 提供。exportfs 会从 ``/var/lib/nfs/rmtab`` 获取
活动客户端的信息。)

This approach is quite fragile as it depends on rmtab being correct
which is not always easy, particularly when trying to implement
fail-over.  Even when the system is working well, ``rmtab`` suffers from
getting lots of old entries that never get removed.
(这种做法相当脆弱，因为它依赖 rmtab 的正确性，而这并不总是容易做到，尤其
在试图实现故障切换 (fail-over) 时。即使系统工作良好，``rmtab`` 也会积累
大量永远不会被移除的陈旧条目。)

With modern kernels we have the option of having the kernel tell mountd
when it gets a request from an unknown host, and mountd can give
appropriate export information to the kernel.  This removes the
dependency on ``rmtab`` and means that the kernel only needs to know about
currently active clients.
(在现代内核中，我们可以选择让内核在收到来自未知主机的请求时告知 mountd，
由 mountd 向内核提供相应的导出 (export) 信息。这消除了对 ``rmtab`` 的依
赖，意味着内核只需了解当前活动的客户端。)

To enable this new functionality, you need to::

  mount -t nfsd nfsd /proc/fs/nfsd

(要启用这一新功能，需要先执行上面的命令。)

before running exportfs or mountd.  It is recommended that all NFS
services be protected from the internet-at-large by a firewall where
that is possible.
(之后才能运行 exportfs 或 mountd。建议在可能的情况下，用防火墙把所有 NFS
服务与整个互联网隔离开。)

mcelog(mcelog 工具)
-------------------

On x86 kernels the mcelog utility is needed to process and log machine check
events when ``CONFIG_X86_MCE`` is enabled. Machine check events are errors
reported by the CPU. Processing them is strongly encouraged.
(在 x86 内核上，启用 ``CONFIG_X86_MCE`` 时需要 mcelog 工具来处理并记录机
器检查 (machine check) 事件。机器检查事件是 CPU 报告的错误。强烈建议对它
们进行处理。)

Kernel documentation(内核文档)
******************************

Sphinx(Sphinx 文档生成器)
-------------------------

Please see :ref:`sphinx_install` in Documentation/doc-guide/sphinx.rst
for details about Sphinx requirements.
(关于 Sphinx 要求的细节，请参见 Documentation/doc-guide/sphinx.rst 中的
:ref:`sphinx_install`。)

rustdoc(rustdoc 工具)
---------------------

``rustdoc`` is used to generate the documentation for Rust code. Please see
Documentation/rust/general-information.rst for more information.
(``rustdoc`` 用于为 Rust 代码生成文档。更多信息请参见
Documentation/rust/general-information.rst。)

Getting updated software(获取更新的软件)
========================================

Kernel compilation(内核编译)
*****************************

gcc(gcc 编译器)
---------------

- <ftp://ftp.gnu.org/gnu/gcc/>

Clang/LLVM(Clang/LLVM)
----------------------

- :ref:`Getting LLVM <getting_llvm>`.

Rust(Rust)
----------

- Documentation/rust/quick-start.rst.

bindgen(bindgen)
----------------

- Documentation/rust/quick-start.rst.

Make(make 工具)
---------------

- <ftp://ftp.gnu.org/gnu/make/>

Bash(bash shell 解释器)
-----------------------

- <ftp://ftp.gnu.org/gnu/bash/>

Binutils(binutils 工具集)
-------------------------

- <https://www.kernel.org/pub/linux/devel/binutils/>

Flex(flex 词法分析器)
---------------------

- <https://github.com/westes/flex/releases>

Bison(bison 语法分析器)
-----------------------

- <ftp://ftp.gnu.org/gnu/bison/>

OpenSSL(OpenSSL 库)
-------------------

- <https://www.openssl.org/>

System utilities(系统实用工具)
******************************

Util-linux(util-linux 工具)
---------------------------

- <https://www.kernel.org/pub/linux/utils/util-linux/>

Kmod(kmod 工具)
---------------

- <https://www.kernel.org/pub/linux/utils/kernel/kmod/>
- <https://git.kernel.org/pub/scm/utils/kernel/kmod/kmod.git>

Ksymoops(ksymoops 工具)
-----------------------

- <https://www.kernel.org/pub/linux/utils/kernel/ksymoops/v2.4/>

Mkinitrd(mkinitrd 工具)
-----------------------

- <https://code.launchpad.net/initrd-tools/main>

E2fsprogs(e2fsprogs 工具)
-------------------------

- <https://www.kernel.org/pub/linux/kernel/people/tytso/e2fsprogs/>
- <https://git.kernel.org/pub/scm/fs/ext2/e2fsprogs.git/>

JFSutils(jfsutils 工具)
-----------------------

- <https://jfs.sourceforge.net/>

Xfsprogs(xfsprogs 工具)
-----------------------

- <https://git.kernel.org/pub/scm/fs/xfs/xfsprogs-dev.git>
- <https://www.kernel.org/pub/linux/utils/fs/xfs/xfsprogs/>

Pcmciautils(pcmciautils 工具)
-----------------------------

- <https://www.kernel.org/pub/linux/utils/kernel/pcmcia/>

Quota-tools(quota-tools 工具)
-----------------------------

- <https://sourceforge.net/projects/linuxquota/>


Intel P6 microcode(Intel P6 微码)
---------------------------------

- <https://downloadcenter.intel.com/>

udev(udev 设备管理器)
---------------------

- <https://www.freedesktop.org/software/systemd/man/udev.html>

FUSE(FUSE 文件系统)
-------------------

- <https://github.com/libfuse/libfuse/releases>

mcelog(mcelog 工具)
-------------------

- <https://www.mcelog.org/>

Networking(网络)
****************

PPP(PPP 协议)
-------------

- <https://download.samba.org/pub/ppp/>
- <https://git.ozlabs.org/?p=ppp.git>
- <https://github.com/paulusmack/ppp/>

NFS-utils(nfs-utils 工具)
-------------------------

- <https://sourceforge.net/project/showfiles.php?group_id=14>
- <https://nfs.sourceforge.net/>

Iptables(iptables 工具)
-----------------------

- <https://netfilter.org/projects/iptables/index.html>

Ip-route2(iproute2 工具)
------------------------

- <https://www.kernel.org/pub/linux/utils/net/iproute2/>

OProfile(oprofile 工具)
-----------------------

- <https://oprofile.sf.net/download/>

Kernel documentation(内核文档)
******************************

Sphinx(Sphinx 文档生成器)
-------------------------

- <https://www.sphinx-doc.org/>
