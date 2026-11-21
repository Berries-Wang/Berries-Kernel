.. highlight:: none

Debugging kernel and modules via gdb(通过 gdb 调试内核与模块)
=============================================================

The kernel debugger kgdb, hypervisors like QEMU or JTAG-based hardware
interfaces allow to debug the Linux kernel and its modules during runtime
using gdb. Gdb comes with a powerful scripting interface for python. The
kernel provides a collection of helper scripts that can simplify typical
kernel debugging steps. This is a short tutorial about how to enable and use
them. It focuses on QEMU/KVM virtual machines as target, but the examples can
be transferred to the other gdb stubs as well.
(内核调试器 kgdb、QEMU 等管理程序以及基于 JTAG 的硬件接口，都可以在运行期使用 gdb 调试 Linux 内核及其模块。gdb 自带强大的 python 脚本接口。内核提供了一组辅助脚本，可以简化典型的内核调试步骤。本文是关于如何启用与使用这些脚本的简短教程。它以 QEMU/KVM 虚拟机为目标，但这些示例同样适用于其他 gdb stub。)


Requirements(要求)
------------------

- gdb 7.2+ (recommended: 7.4+) with python support enabled (typically true
  for distributions)
  (- gdb 7.2+(建议 7.4+),需启用 python 支持(发行版通常已启用)。)


Setup(设置)
-----------

- Create a virtual Linux machine for QEMU/KVM (see www.linux-kvm.org and
  www.qemu.org for more details). For cross-development,
  https://landley.net/aboriginal/bin keeps a pool of machine images and
  toolchains that can be helpful to start from.
  (- 为 QEMU/KVM 创建一台虚拟 Linux 机器(更多细节见 www.linux-kvm.org 与 www.qemu.org)。做交叉开发时，https://landley.net/aboriginal/bin 维护了一批机器镜像与工具链，有助于起步。)

- Build the kernel with CONFIG_GDB_SCRIPTS enabled, but leave
  CONFIG_DEBUG_INFO_REDUCED off. If your architecture supports
  CONFIG_FRAME_POINTER, keep it enabled.
  (- 编译内核时启用 CONFIG_GDB_SCRIPTS，但保持 CONFIG_DEBUG_INFO_REDUCED 关闭。如果你的架构支持 CONFIG_FRAME_POINTER，则保持其启用。)

- Install that kernel on the guest, turn off KASLR if necessary by adding
  "nokaslr" to the kernel command line.
  Alternatively, QEMU allows to boot the kernel directly using -kernel,
  -append, -initrd command line switches. This is generally only useful if
  you do not depend on modules. See QEMU documentation for more details on
  this mode. In this case, you should build the kernel with
  CONFIG_RANDOMIZE_BASE disabled if the architecture supports KASLR.
  (- 把该内核安装到客户机上，必要时在内核命令行中添加 "nokaslr" 以关闭 KASLR。
  另外，QEMU 也支持用 -kernel、-append、-initrd 命令行开关直接引导内核。这通常只在不依赖模块时才有用。此模式的更多细节见 QEMU 文档。这种情况下，如果架构支持 KASLR，你应在编译内核时禁用 CONFIG_RANDOMIZE_BASE。)

- Build the gdb scripts (required on kernels v5.1 and above)::

    make scripts_gdb

  (- 构建 gdb 脚本(内核 v5.1 及以上版本必需)：)

- Enable the gdb stub of QEMU/KVM, either
  (- 启用 QEMU/KVM 的 gdb stub，方式二选一：)

    - at VM startup time by appending "-s" to the QEMU command line
      (- 在虚拟机启动时，向 QEMU 命令行追加 "-s"；)

  or
  (或：)

    - during runtime by issuing "gdbserver" from the QEMU monitor
      console
      (- 在运行期，从 QEMU 监视器控制台发出 "gdbserver" 命令。)

- cd /path/to/linux-build

- Start gdb: gdb vmlinux

  Note: Some distros may restrict auto-loading of gdb scripts to known safe
  directories. In case gdb reports to refuse loading vmlinux-gdb.py, add::

    add-auto-load-safe-path /path/to/linux-build

  (注意：部分发行版可能把 gdb 脚本的自动加载限制在已知的安全目录内。如果 gdb 提示拒绝加载 vmlinux-gdb.py，则添加：)

  to ~/.gdbinit. See gdb help for more details.
  (把上面这行加入 ~/.gdbinit。更多细节请参阅 gdb 帮助。)

- Attach to the booted guest::

    (gdb) target remote :1234

  (- 连接到已启动的客户机：)


Examples of using the Linux-provided gdb helpers(使用 Linux 提供的 gdb 辅助工具的示例)
--------------------------------------------------------------------------------------

- Load module (and main kernel) symbols::

    (gdb) lx-symbols
    loading vmlinux
    scanning for modules in /home/user/linux/build
    loading @0xffffffffa0020000: /home/user/linux/build/net/netfilter/xt_tcpudp.ko
    loading @0xffffffffa0016000: /home/user/linux/build/net/netfilter/xt_pkttype.ko
    loading @0xffffffffa0002000: /home/user/linux/build/net/netfilter/xt_limit.ko
    loading @0xffffffffa00ca000: /home/user/linux/build/net/packet/af_packet.ko
    loading @0xffffffffa003c000: /home/user/linux/build/fs/fuse/fuse.ko
    ...
    loading @0xffffffffa0000000: /home/user/linux/build/drivers/ata/ata_generic.ko

  (- 加载模块(以及主内核)的符号：)

- Set a breakpoint on some not yet loaded module function, e.g.::

    (gdb) b btrfs_init_sysfs
    Function "btrfs_init_sysfs" not defined.
    Make breakpoint pending on future shared library load? (y or [n]) y
    Breakpoint 1 (btrfs_init_sysfs) pending.

  (- 在某个尚未加载的模块函数上设置断点，例如：)

- Continue the target::

    (gdb) c

  (- 让目标机继续运行：)

- Load the module on the target and watch the symbols being loaded as well as
  the breakpoint hit::

    loading @0xffffffffa0034000: /home/user/linux/build/lib/libcrc32c.ko
    loading @0xffffffffa0050000: /home/user/linux/build/lib/lzo/lzo_compress.ko
    loading @0xffffffffa006e000: /home/user/linux/build/lib/zlib_deflate/zlib_deflate.ko
    loading @0xffffffffa01b1000: /home/user/linux/build/fs/btrfs/btrfs.ko

    Breakpoint 1, btrfs_init_sysfs () at /home/user/linux/fs/btrfs/sysfs.c:36
    36              btrfs_kset = kset_create_and_add("btrfs", NULL, fs_kobj);

  (- 在目标机上加载该模块，同时观察符号的加载与断点的命中：)

- Dump the log buffer of the target kernel::

    (gdb) lx-dmesg
    [     0.000000] Initializing cgroup subsys cpuset
    [     0.000000] Initializing cgroup subsys cpu
    [     0.000000] Linux version 3.8.0-rc4-dbg+ (...
    [     0.000000] Command line: root=/dev/sda2 resume=/dev/sda1 vga=0x314
    [     0.000000] e820: BIOS-provided physical RAM map:
    [     0.000000] BIOS-e820: [mem 0x0000000000000000-0x000000000009fbff] usable
    [     0.000000] BIOS-e820: [mem 0x000000000009fc00-0x000000000009ffff] reserved
    ....

  (- 转储目标内核的日志缓冲区：)

- Examine fields of the current task struct(supported by x86 and arm64 only)::

    (gdb) p $lx_current().pid
    $1 = 4998
    (gdb) p $lx_current().comm
    $2 = "modprobe\000\000\000\000\000\000\000"

  (- 检查当前 task 结构体的字段(仅 x86 与 arm64 支持)：)

- Make use of the per-cpu function for the current or a specified CPU::

    (gdb) p $lx_per_cpu(runqueues).nr_running
    $3 = 1
    (gdb) p $lx_per_cpu(runqueues, 2).nr_running
    $4 = 0

  (- 使用 per-cpu 函数，可针对当前 CPU 或指定 CPU：)

- Dig into hrtimers using the container_of helper::

    (gdb) set $leftmost = $lx_per_cpu(hrtimer_bases).clock_base[0].active.rb_root.rb_leftmost
    (gdb) p *$container_of($leftmost, "struct hrtimer", "node")
    $5 = {
      node = {
        node = {
          __rb_parent_color = 18446612686384860673,
          rb_right = 0xffff888231da8b00,
          rb_left = 0x0
        },
        expires = 1228461000000
      },
      _softexpires = 1228461000000,
      function = 0xffffffff8137ab20 <tick_nohz_handler>,
      base = 0xffff888231d9b4c0,
      state = 1 '\001',
      is_rel = 0 '\000',
      is_soft = 0 '\000',
      is_hard = 1 '\001'
    }

  (- 借助 container_of 辅助函数深入探查 hrtimer：)


List of commands and functions(命令与函数列表)
----------------------------------------------

The number of commands and convenience functions may evolve over the time,
this is just a snapshot of the initial version::

 (gdb) apropos lx
 function lx_current -- Return current task
 function lx_module -- Find module by name and return the module variable
 function lx_per_cpu -- Return per-cpu variable
 function lx_task_by_pid -- Find Linux task by PID and return the task_struct variable
 function lx_thread_info -- Calculate Linux thread_info from task variable
 lx-dmesg -- Print Linux kernel log buffer
 lx-lsmod -- List currently loaded modules
 lx-symbols -- (Re-)load symbols of Linux kernel and currently loaded modules

(命令与便捷函数的数量可能随时间演变，这里只是初始版本的一个快照：)

Detailed help can be obtained via "help <command-name>" for commands and "help
function <function-name>" for convenience functions.
(对命令可用 "help <command-name>"，对便捷函数可用 "help function <function-name>" 获取详细帮助。)

Debugging GDB scripts(调试 GDB 脚本)
------------------------------------

GDB does not enable a full Python backtrace which can make debugging GDB
scripts more difficult than necessary. The following will allow for printing a
full backtrace of the python environment::

 (gdb) set python print-stack full

(GDB 默认不启用完整的 Python 回溯，这会让调试 GDB 脚本变得比必要的更困难。以下设置可以打印 python 环境的完整回溯：)
