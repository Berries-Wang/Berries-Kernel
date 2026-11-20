.. SPDX-License-Identifier: GPL-2.0

=====================================================
Running nested guests with KVM(用 KVM 运行嵌套客户机)
=====================================================

A nested guest is the ability to run a guest inside another guest (it
can be KVM-based or a different hypervisor).  The straightforward
example is a KVM guest that in turn runs on a KVM guest (the rest of
this document is built on this example)::

              .----------------.  .----------------.
              |                |  |                |
              |      L2        |  |      L2        |
              | (Nested Guest) |  | (Nested Guest) |
              |                |  |                |
              |----------------'--'----------------|
              |                                    |
              |       L1 (Guest Hypervisor)        |
              |          KVM (/dev/kvm)            |
              |                                    |
      .------------------------------------------------------.
      |                 L0 (Host Hypervisor)                 |
      |                    KVM (/dev/kvm)                    |
      |------------------------------------------------------|
      |        Hardware (with virtualization extensions)     |
      '------------------------------------------------------'

(嵌套客户机指的是在一个客户机内部再运行客户机的能力(可以是基于 KVM 的，也可以
是其他 hypervisor)。最直接的例子就是运行在 KVM 客户机之上的 KVM 客户机(本文档
其余部分都基于这个例子展开)。)

Terminology:
(术语：)

- L0 – level-0; the bare metal host, running KVM
  (L0 即第 0 层；运行 KVM 的裸金属主机。)

- L1 – level-1 guest; a VM running on L0; also called the "guest
  hypervisor", as it itself is capable of running KVM.
  (L1 即第 1 层客户机；运行在 L0 上的虚拟机，也称为 "guest hypervisor"(客户机
  hypervisor)，因为它自身也能够运行 KVM。)

- L2 – level-2 guest; a VM running on L1, this is the "nested guest"
  (L2 即第 2 层客户机；运行在 L1 上的虚拟机，也就是这里所说的 "nested
  guest"(嵌套客户机)。)

.. note:: The above diagram is modelled after the x86 architecture;
          s390x, ppc64 and other architectures are likely to have
          a different design for nesting.
          (上面的示意图以 x86 体系结构为蓝本；s390x、ppc64 及其他体系结构的嵌套
          设计很可能与此不同。)

          For example, s390x always has an LPAR (LogicalPARtition)
          hypervisor running on bare metal, adding another layer and
          resulting in at least four levels in a nested setup — L0 (bare
          metal, running the LPAR hypervisor), L1 (host hypervisor), L2
          (guest hypervisor), L3 (nested guest).
          (例如，s390x 在裸金属上总是运行着一个 LPAR (LogicalPARtition)
          hypervisor，多出一层，使得嵌套环境中至少有四层——L0 (裸金属，运行
          LPAR hypervisor)、L1 (宿主 hypervisor)、L2 (客户机 hypervisor)、
          L3 (嵌套客户机)。)

          This document will stick with the three-level terminology (L0,
          L1, and L2) for all architectures; and will largely focus on
          x86.
          (本文档对所有体系结构都沿用三层术语 (L0、L1 和 L2)，并且主要关注
          x86。)


Use Cases(使用场景)
-------------------

There are several scenarios where nested KVM can be useful, to name a
few:
(有很多场景都能从嵌套 KVM 中受益，这里举几个例子：)

- As a developer, you want to test your software on different operating
  systems (OSes).  Instead of renting multiple VMs from a Cloud
  Provider, using nested KVM lets you rent a large enough "guest
  hypervisor" (level-1 guest).  This in turn allows you to create
  multiple nested guests (level-2 guests), running different OSes, on
  which you can develop and test your software.
  (作为开发者，你想在不同的操作系统 (OS) 上测试自己的软件。与其从云服务商处
  租用多台虚拟机，不如使用嵌套 KVM 租用一台足够大的 "guest hypervisor"(第 1
  层客户机)，进而在其上创建多台运行不同操作系统的嵌套客户机(第 2 层客户机)，
  并在其中开发和测试你的软件。)

- Live migration of "guest hypervisors" and their nested guests, for
  load balancing, disaster recovery, etc.
  (对 "guest hypervisor" 及其嵌套客户机做实时迁移，用于负载均衡、灾难恢复等。)

- VM image creation tools (e.g. ``virt-install``,  etc) often run
  their own VM, and users expect these to work inside a VM.
  (虚拟机镜像创建工具(如 ``virt-install`` 等)通常会运行自己的虚拟机，用户期望
  它们在虚拟机里也能正常工作。)

- Some OSes use virtualization internally for security (e.g. to let
  applications run safely in isolation).
  (一些操作系统出于安全考虑在内部使用虚拟化(例如让应用程序在隔离环境中安全
  运行)。)


Enabling "nested" (x86)(启用 "nested"(x86))
-------------------------------------------

From Linux kernel v4.20 onwards, the ``nested`` KVM parameter is enabled
by default for Intel and AMD.  (Though your Linux distribution might
override this default.)
(从 Linux 内核 v4.20 起，``nested`` KVM 参数在 Intel 和 AMD 上默认启用(不过你
的 Linux 发行版可能会覆盖这一默认值)。)

In case you are running a Linux kernel older than v4.19, to enable
nesting, set the ``nested`` KVM module parameter to ``Y`` or ``1``.  To
persist this setting across reboots, you can add it in a config file, as
shown below:
(如果你运行的 Linux 内核版本低于 v4.19，要启用嵌套，需将 ``nested`` KVM 模块
参数设置为 ``Y`` 或 ``1``。要让这一设置在重启后依然生效，可以把它加入配置文件，
如下所示：)

1. On the bare metal host (L0), list the kernel modules and ensure that
   the KVM modules::

    $ lsmod | grep -i kvm
    kvm_intel             133627  0
    kvm                   435079  1 kvm_intel

   (在裸金属主机 (L0) 上列出内核模块，并确认 KVM 模块已加载：)

2. Show information for ``kvm_intel`` module::

    $ modinfo kvm_intel | grep -i nested
    parm:           nested:bool

   (查看 ``kvm_intel`` 模块的信息：)

3. For the nested KVM configuration to persist across reboots, place the
   below in ``/etc/modprobed/kvm_intel.conf`` (create the file if it
   doesn't exist)::

    $ cat /etc/modprobe.d/kvm_intel.conf
    options kvm-intel nested=y

   (要让嵌套 KVM 配置在重启后保持生效，把以下内容放入
   ``/etc/modprobed/kvm_intel.conf``(如果该文件不存在则创建)：)

4. Unload and re-load the KVM Intel module::

    $ sudo rmmod kvm-intel
    $ sudo modprobe kvm-intel

   (卸载并重新加载 KVM Intel 模块：)

5. Verify if the ``nested`` parameter for KVM is enabled::

    $ cat /sys/module/kvm_intel/parameters/nested
    Y

   (确认 KVM 的 ``nested`` 参数是否已启用：)

For AMD hosts, the process is the same as above, except that the module
name is ``kvm-amd``.
(对于 AMD 主机，流程与上面相同，只是模块名为 ``kvm-amd``。)


Additional nested-related kernel parameters (x86)(其他嵌套相关的内核参数 (x86))
-------------------------------------------------------------------------------

If your hardware is sufficiently advanced (Intel Haswell processor or
higher, which has newer hardware virt extensions), the following
additional features will also be enabled by default: "Shadow VMCS
(Virtual Machine Control Structure)", APIC Virtualization on your bare
metal host (L0).  Parameters for Intel hosts::

    $ cat /sys/module/kvm_intel/parameters/enable_shadow_vmcs
    Y

    $ cat /sys/module/kvm_intel/parameters/enable_apicv
    Y

    $ cat /sys/module/kvm_intel/parameters/ept
    Y

(如果你的硬件足够先进(Intel Haswell 或更新的处理器，具备较新的硬件虚拟化扩展)，
那么在你的裸金属主机 (L0) 上，以下附加特性也会默认启用："Shadow VMCS (Virtual
Machine Control Structure)"(影子 VMCS)与 APIC Virtualization(APIC 虚拟化)。
上面就是 Intel 主机的相关参数：)

.. note:: If you suspect your L2 (i.e. nested guest) is running slower,
          ensure the above are enabled (particularly
          ``enable_shadow_vmcs`` and ``ept``).
          (如果你怀疑自己的 L2(即嵌套客户机)运行变慢，请确认上述参数都已启用
          (尤其是 ``enable_shadow_vmcs`` 和 ``ept``)。)


Starting a nested guest (x86)(启动嵌套客户机 (x86))
---------------------------------------------------

Once your bare metal host (L0) is configured for nesting, you should be
able to start an L1 guest with::

    $ qemu-kvm -cpu host [...]

(一旦你的裸金属主机 (L0) 完成了嵌套配置，就应该能用以下命令启动 L1 客户机：)

The above will pass through the host CPU's capabilities as-is to the
guest, or for better live migration compatibility, use a named CPU
model supported by QEMU. e.g.::

    $ qemu-kvm -cpu Haswell-noTSX-IBRS,vmx=on

(上面的做法会把宿主 CPU 的能力原样透传给客户机；若想要更好的实时迁移兼容性，
可以使用 QEMU 支持的具名 CPU 型号，例如：)

then the guest hypervisor will subsequently be capable of running a
nested guest with accelerated KVM.
(这样，客户机 hypervisor 随后就能运行带 KVM 加速的嵌套客户机了。)


Enabling "nested" (s390x)(启用 "nested"(s390x))
-----------------------------------------------

1. On the host hypervisor (L0), enable the ``nested`` parameter on
   s390x::

    $ rmmod kvm
    $ modprobe kvm nested=1

   (在宿主 hypervisor (L0) 上为 s390x 启用 ``nested`` 参数：)

.. note:: On s390x, the kernel parameter ``hpage`` is mutually exclusive
          with the ``nested`` parameter — i.e. to be able to enable
          ``nested``, the ``hpage`` parameter *must* be disabled.
          (在 s390x 上，内核参数 ``hpage`` 与 ``nested`` 参数互斥——也就是说，
          要启用 ``nested``，就必须先禁用 ``hpage`` 参数。)

2. The guest hypervisor (L1) must be provided with the ``sie`` CPU
   feature — with QEMU, this can be done by using "host passthrough"
   (via the command-line ``-cpu host``).
   (客户机 hypervisor (L1) 必须具备 ``sie`` CPU 特性——使用 QEMU 时，可以通过
   "host passthrough"(借助命令行 ``-cpu host``)来提供。)

3. Now the KVM module can be loaded in the L1 (guest hypervisor)::

    $ modprobe kvm

   (现在可以在 L1(客户机 hypervisor)中加载 KVM 模块了：)


Live migration with nested KVM(嵌套 KVM 的实时迁移)
---------------------------------------------------

Migrating an L1 guest, with a  *live* nested guest in it, to another
bare metal host, works as of Linux kernel 5.3 and QEMU 4.2.0 for
Intel x86 systems, and even on older versions for s390x.
(把带有*活动* 嵌套客户机的 L1 客户机迁移到另一台裸金属主机，自 Linux 内核 5.3
与 QEMU 4.2.0 起在 Intel x86 系统上可行；对 s390x 来说，更早的版本就已支持。)

On AMD systems, once an L1 guest has started an L2 guest, the L1 guest
should no longer be migrated or saved (refer to QEMU documentation on
"savevm"/"loadvm") until the L2 guest shuts down.  Attempting to migrate
or save-and-load an L1 guest while an L2 guest is running will result in
undefined behavior.  You might see a ``kernel BUG!`` entry in ``dmesg``, a
kernel 'oops', or an outright kernel panic.  Such a migrated or loaded L1
guest can no longer be considered stable or secure, and must be restarted.
Migrating an L1 guest merely configured to support nesting, while not
actually running L2 guests, is expected to function normally even on AMD
systems but may fail once guests are started.
(在 AMD 系统上，一旦 L1 客户机启动了 L2 客户机，在 L2 客户机关闭之前，就不应再
对 L1 客户机进行迁移或保存(参见 QEMU 关于 "savevm"/"loadvm" 的文档)。在 L2
客户机运行期间尝试迁移或"保存后加载"L1 客户机会导致未定义行为。你可能会在
``dmesg`` 中看到 ``kernel BUG!`` 条目、一个内核 'oops'，或者干脆是内核 panic。
经过这样迁移或加载的 L1 客户机不能再被视为稳定或安全，必须重启。仅配置为支持
嵌套、而并未实际运行 L2 客户机的 L1 客户机，即使在 AMD 系统上迁移也预期可以正常
工作，但一旦启动了客户机就可能失败。)

Migrating an L2 guest is always expected to succeed, so all the following
scenarios should work even on AMD systems:
(迁移 L2 客户机预期总是可以成功，因此以下所有场景即使在 AMD 系统上也应当可行：)

- Migrating a nested guest (L2) to another L1 guest on the *same* bare
  metal host.
  (把嵌套客户机 (L2) 迁移到*同一台* 裸金属主机上的另一个 L1 客户机。)

- Migrating a nested guest (L2) to another L1 guest on a *different*
  bare metal host.
  (把嵌套客户机 (L2) 迁移到*另一台* 裸金属主机上的另一个 L1 客户机。)

- Migrating a nested guest (L2) to a bare metal host.
  (把嵌套客户机 (L2) 迁移到一台裸金属主机。)

Reporting bugs from nested setups(报告嵌套环境中的 bug)
-------------------------------------------------------

Debugging "nested" problems can involve sifting through log files across
L0, L1 and L2; this can result in tedious back-n-forth between the bug
reporter and the bug fixer.
(调试 "nested" 相关问题可能需要在 L0、L1 和 L2 的日志文件中反复翻查；这会让
bug 报告者与修复者之间来回拉扯，十分繁琐。)

- Mention that you are in a "nested" setup.  If you are running any kind
  of "nesting" at all, say so.  Unfortunately, this needs to be called
  out because when reporting bugs, people tend to forget to even
  *mention* that they're using nested virtualization.
  (说明你处于 "nested" 环境。只要你运行了任何形式的 "nesting"，都要说出来。
  遗憾的是，这一点必须专门提醒，因为人们在报告 bug 时，往往连自己*正在使用*
  嵌套虚拟化这件事都忘了提。)

- Ensure you are actually running KVM on KVM.  Sometimes people do not
  have KVM enabled for their guest hypervisor (L1), which results in
  them running with pure emulation or what QEMU calls it as "TCG", but
  they think they're running nested KVM.  Thus confusing "nested Virt"
  (which could also mean, QEMU on KVM) with "nested KVM" (KVM on KVM).
  (确认你确实在运行 KVM on KVM。有时人们没有在客户机 hypervisor (L1) 上启用
  KVM，结果实际运行的是纯模拟(QEMU 称之为 "TCG")，却以为自己跑的是嵌套 KVM，
  从而把 "nested Virt"(也可能指 KVM 之上的 QEMU)与 "nested KVM"(KVM 之上的
  KVM)混为一谈。)

Information to collect (generic)(需要收集的信息 (通用))
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

The following is not an exhaustive list, but a very good starting point:
(以下并非详尽清单，但是一个非常好的起点：)

  - Kernel, libvirt, and QEMU version from L0
    (L0 上的内核、libvirt 和 QEMU 版本。)

  - Kernel, libvirt and QEMU version from L1
    (L1 上的内核、libvirt 和 QEMU 版本。)

  - QEMU command-line of L1 -- when using libvirt, you'll find it here:
    ``/var/log/libvirt/qemu/instance.log``
    (L1 的 QEMU 命令行——使用 libvirt 时，可以在
    ``/var/log/libvirt/qemu/instance.log`` 中找到。)

  - QEMU command-line of L2 -- as above, when using libvirt, get the
    complete libvirt-generated QEMU command-line
    (L2 的 QEMU 命令行——同上，使用 libvirt 时获取 libvirt 生成的完整 QEMU
    命令行。)

  - ``cat /sys/cpuinfo`` from L0
    (L0 上的 ``cat /sys/cpuinfo`` 输出。)

  - ``cat /sys/cpuinfo`` from L1
    (L1 上的 ``cat /sys/cpuinfo`` 输出。)

  - ``lscpu`` from L0
    (L0 上的 ``lscpu`` 输出。)

  - ``lscpu`` from L1
    (L1 上的 ``lscpu`` 输出。)

  - Full ``dmesg`` output from L0
    (L0 的完整 ``dmesg`` 输出。)

  - Full ``dmesg`` output from L1
    (L1 的完整 ``dmesg`` 输出。)

x86-specific info to collect(x86 特有的收集信息)
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

Both the below commands, ``x86info`` and ``dmidecode``, should be
available on most Linux distributions with the same name:
(以下两个命令 ``x86info`` 和 ``dmidecode``，在大多数 Linux 发行版上都以相同的
名字提供：)

  - Output of: ``x86info -a`` from L0
    (L0 上 ``x86info -a`` 的输出。)

  - Output of: ``x86info -a`` from L1
    (L1 上 ``x86info -a`` 的输出。)

  - Output of: ``dmidecode`` from L0
    (L0 上 ``dmidecode`` 的输出。)

  - Output of: ``dmidecode`` from L1
    (L1 上 ``dmidecode`` 的输出。)

s390x-specific info to collect(s390x 特有的收集信息)
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

Along with the earlier mentioned generic details, the below is
also recommended:
(除了前面提到的通用信息之外，还建议收集以下内容：)

  - ``/proc/sysinfo`` from L1; this will also include the info from L0
    (L1 上的 ``/proc/sysinfo``；其中也会包含来自 L0 的信息。)
