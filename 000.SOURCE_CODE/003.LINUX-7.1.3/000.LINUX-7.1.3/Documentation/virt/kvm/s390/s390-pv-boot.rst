.. SPDX-License-Identifier: GPL-2.0

==============================================================
s390 (IBM Z) Boot/IPL of Protected VMs(受保护虚拟机的引导/IPL)
==============================================================

Summary(概要)
-------------
The memory of Protected Virtual Machines (PVMs) is not accessible to
I/O or the hypervisor. In those cases where the hypervisor needs to
access the memory of a PVM, that memory must be made accessible.
Memory made accessible to the hypervisor will be encrypted. See
Documentation/virt/kvm/s390/s390-pv.rst for details."
(受保护虚拟机 (PVM) 的内存对 I/O 或管理程序 (hypervisor) 是不可访问
的。在管理程序需要访问 PVM 内存的情况下，必须使该内存变为可访问。对
管理程序可访问的内存将会被加密。详情参见
Documentation/virt/kvm/s390/s390-pv.rst。)

On IPL (boot) a small plaintext bootloader is started, which provides
information about the encrypted components and necessary metadata to
KVM to decrypt the protected virtual machine.
(在 IPL (引导) 时会启动一个小型明文引导加载程序，它向 KVM 提供加密组
件的信息以及必要的元数据，以解密受保护的虚拟机。)

Based on this data, KVM will make the protected virtual machine known
to the Ultravisor (UV) and instruct it to secure the memory of the
PVM, decrypt the components and verify the data and address list
hashes, to ensure integrity. Afterwards KVM can run the PVM via the
SIE instruction which the UV will intercept and execute on KVM's
behalf.
(基于这些数据，KVM 会把受保护的虚拟机告知 Ultravisor (UV)，并指示它
保护 PVM 的内存、解密各组件并验证数据与地址列表的哈希，以确保完整性。
之后，KVM 可以通过 SIE 指令运行 PVM，UV 会拦截该指令并代表 KVM 执行。)

As the guest image is just like an opaque kernel image that does the
switch into PV mode itself, the user can load encrypted guest
executables and data via every available method (network, dasd, scsi,
direct kernel, ...) without the need to change the boot process.
(由于客户机镜像就像一个自行完成向 PV 模式切换的不透明内核镜像，用户
可以通过所有可用的方法 (网络、dasd、scsi、direct kernel 等) 加载加密
的客户机可执行文件和数据，而无需更改引导过程。)


Diag308(Diag308 诊断指令)
-------------------------
This diagnose instruction is the basic mechanism to handle IPL and
related operations for virtual machines. The VM can set and retrieve
IPL information blocks, that specify the IPL method/devices and
request VM memory and subsystem resets, as well as IPLs.
(该诊断指令是处理虚拟机 IPL 及相关操作的基本机制。虚拟机可以设置和
获取 IPL 信息块，这些信息块指定 IPL 方法/设备，并请求虚拟机内存与子
系统复位以及 IPL。)

For PVMs this concept has been extended with new subcodes:
(对于 PVM，此概念已通过新的子码 (subcode) 得到扩展：)

Subcode 8: Set an IPL Information Block of type 5 (information block
for PVMs)
Subcode 9: Store the saved block in guest memory
Subcode 10: Move into Protected Virtualization mode
(子码 8：设置一个类型为 5 的 IPL 信息块 (PVM 的信息块)。
子码 9：把保存的块存储到客户机内存中。
子码 10：进入受保护虚拟化模式。)

The new PV load-device-specific-parameters field specifies all data
that is necessary to move into PV mode.
(新的 PV load-device-specific-parameters 字段指定进入 PV 模式所需的
全部数据。)

* PV Header origin
  (PV 头起始位置。)
* PV Header length
  (PV 头长度。)
* List of Components composed of
  (组件列表，由以下部分组成。)
   * AES-XTS Tweak prefix
     (AES-XTS Tweak 前缀。)
   * Origin
     (起始位置。)
   * Size
     (大小。)

The PV header contains the keys and hashes, which the UV will use to
decrypt and verify the PV, as well as control flags and a start PSW.
(PV 头包含 UV 将用于解密和验证 PV 的密钥与哈希，以及控制标志和一个起
始 PSW。)

The components are for instance an encrypted kernel, kernel parameters
and initrd. The components are decrypted by the UV.
(组件例如是加密的内核、内核参数和 initrd。组件由 UV 解密。)

After the initial import of the encrypted data, all defined pages will
contain the guest content. All non-specified pages will start out as
zero pages on first access.
(在初始导入加密数据之后，所有已定义的页面都将包含客户机内容。所有未
指定的页面在首次访问时将作为零页开始。)


When running in protected virtualization mode, some subcodes will result in
exceptions or return error codes.
(在受保护虚拟化模式下运行时，某些子码会导致异常或返回错误码。)

Subcodes 4 and 7, which specify operations that do not clear the guest
memory, will result in specification exceptions. This is because the
UV will clear all memory when a secure VM is removed, and therefore
non-clearing IPL subcodes are not allowed.
(子码 4 和 7 所指定的操作不会清除客户机内存，它们会导致规范异常
(specification exception)。这是因为当移除安全虚拟机时 UV 会清除所有
内存，因此不允许不清除内存的 IPL 子码。)

Subcodes 8, 9, 10 will result in specification exceptions.
Re-IPL into a protected mode is only possible via a detour into non
protected mode.
(子码 8、9、10 会导致规范异常。重新引导 (Re-IPL) 进入受保护模式只能
通过绕道非保护模式来实现。)

Keys(密钥)
----------
Every CEC will have a unique public key to enable tooling to build
encrypted images.
See  `s390-tools <https://github.com/ibm-s390-linux/s390-tools/>`_
for the tooling.
(每个 CEC 都将拥有一个唯一的公钥，使工具链能够构建加密镜像。工具参见
`s390-tools <https://github.com/ibm-s390-linux/s390-tools/>`_ 。)
