.. SPDX-License-Identifier: GPL-2.0

Hibernating Guest VMs(客户机 VM 休眠)
=====================================

Background(背景)
----------------
Linux supports the ability to hibernate itself in order to save power.
Hibernation is sometimes called suspend-to-disk, as it writes a memory
image to disk and puts the hardware into the lowest possible power
state. Upon resume from hibernation, the hardware is restarted and the
memory image is restored from disk so that it can resume execution
where it left off. See the "Hibernation" section of
Documentation/admin-guide/pm/sleep-states.rst.
(Linux 支持为了省电而将自身休眠的能力。休眠有时被称为挂起到磁盘（suspend-to-disk），因为它把内存映像写入磁盘，并让硬件进入尽可能低的电源状态。从休眠中恢复时，硬件被重新启动，内存映像从磁盘恢复，从而可以从上次中断的地方继续执行。参见 Documentation/admin-guide/pm/sleep-states.rst 的"Hibernation"一节。)

Hibernation is usually done on devices with a single user, such as a
personal laptop. For example, the laptop goes into hibernation when
the cover is closed, and resumes when the cover is opened again.
Hibernation and resume happen on the same hardware, and Linux kernel
code orchestrating the hibernation steps assumes that the hardware
configuration is not changed while in the hibernated state.
(休眠通常在单一用户的设备上进行，比如个人笔记本电脑。例如，合上笔记本盖子时进入休眠，再次打开盖子时恢复。休眠与恢复发生在同一硬件上，编排休眠步骤的 Linux 内核代码假定在休眠状态下硬件配置不会改变。)

Hibernation can be initiated within Linux by writing "disk" to
/sys/power/state or by invoking the reboot system call with the
appropriate arguments. This functionality may be wrapped by user space
commands such "systemctl hibernate" that are run directly from a
command line or in response to events such as the laptop lid closing.
(在 Linux 内部，可以通过向 /sys/power/state 写入 "disk"，或用适当参数调用 reboot 系统调用来发起休眠。这一功能可能被用户空间命令（如 "systemctl hibernate"）封装，这些命令可以直接从命令行运行，也可以在笔记本合盖等事件发生时被触发。)

Considerations for Guest VM Hibernation(客户机 VM 休眠的注意事项)
-----------------------------------------------------------------
Linux guests on Hyper-V can also be hibernated, in which case the
hardware is the virtual hardware provided by Hyper-V to the guest VM.
Only the targeted guest VM is hibernated, while other guest VMs and
the underlying Hyper-V host continue to run normally. While the
underlying Windows Hyper-V and physical hardware on which it is
running might also be hibernated using hibernation functionality in
the Windows host, host hibernation and its impact on guest VMs is not
in scope for this documentation.
(Hyper-V 上的 Linux 客户机也可以被休眠，此时的"硬件"就是 Hyper-V 提供给客户机 VM 的虚拟硬件。只有目标客户机 VM 被休眠，其他客户机 VM 和底层的 Hyper-V 主机继续正常运行。虽然底层的 Windows Hyper-V 及其运行的物理硬件也可以用 Windows 主机的休眠功能进行休眠，但主机休眠及其对客户机 VM 的影响不在本文档的讨论范围内。)

Resuming a hibernated guest VM can be more challenging than with
physical hardware because VMs make it very easy to change the hardware
configuration between the hibernation and resume. Even when the resume
is done on the same VM that hibernated, the memory size might be
changed, or virtual NICs or SCSI controllers might be added or
removed. Virtual PCI devices assigned to the VM might be added or
removed. Most such changes cause the resume steps to fail, though
adding a new virtual NIC, SCSI controller, or vPCI device should work.
(恢复一台已休眠的客户机 VM 可能比物理硬件更有挑战性，因为 VM 使得在休眠与恢复之间修改硬件配置变得非常容易。即使恢复是在原来休眠的同一台 VM 上进行的，内存大小也可能被改变，或者虚拟网卡、SCSI 控制器可能被增删。分配给 VM 的虚拟 PCI 设备也可能被增删。这类改动大多会导致恢复步骤失败，不过新增一个虚拟网卡、SCSI 控制器或 vPCI 设备应该可以工作。)

Additional complexity can ensue because the disks of the hibernated VM
can be moved to another newly created VM that otherwise has the same
virtual hardware configuration. While it is desirable for resume from
hibernation to succeed after such a move, there are challenges. See
details on this scenario and its limitations in the "Resuming on a
Different VM" section below.
(还可能出现额外的复杂情况：已休眠 VM 的磁盘可以被移动到另一台新建的、其他虚拟硬件配置相同的 VM 上。虽然希望这样的迁移之后休眠恢复能够成功，但其中存在挑战。关于这一场景及其限制的细节，参见下文"在另一台 VM 上恢复"一节。)

Hyper-V also provides ways to move a VM from one Hyper-V host to
another. Hyper-V tries to ensure processor model and Hyper-V version
compatibility using VM Configuration Versions, and prevents moves to
a host that isn't compatible. Linux adapts to host and processor
differences by detecting them at boot time, but such detection is not
done when resuming execution in the hibernation image. If a VM is
hibernated on one host, then resumed on a host with a different processor
model or Hyper-V version, settings recorded in the hibernation image
may not match the new host. Because Linux does not detect such
mismatches when resuming the hibernation image, undefined behavior
and failures could result.
(Hyper-V 也提供了把 VM 从一台 Hyper-V 主机迁移到另一台的方法。Hyper-V 尝试通过 VM 配置版本来确保处理器型号与 Hyper-V 版本的兼容性，并阻止迁移到不兼容的主机。Linux 通过在启动时检测主机与处理器的差异来适应它们，但在休眠映像中恢复执行时不会做这种检测。如果一台 VM 在一台主机上休眠，然后在处理器型号或 Hyper-V 版本不同的主机上恢复，休眠映像中记录的设置可能与新主机不匹配。由于 Linux 在恢复休眠映像时不会检测这类不匹配，可能导致未定义行为和故障。)


Enabling Guest VM Hibernation(启用客户机 VM 休眠)
-------------------------------------------------
Hibernation of a Hyper-V guest VM is disabled by default because
hibernation is incompatible with memory hot-add, as provided by the
Hyper-V balloon driver. If hot-add is used and the VM hibernates, it
hibernates with more memory than it started with. But when the VM
resumes from hibernation, Hyper-V gives the VM only the originally
assigned memory, and the memory size mismatch causes resume to fail.
(默认情况下，Hyper-V 客户机 VM 的休眠是被禁用的，因为休眠与 Hyper-V 内存气球驱动提供的内存热添加（hot-add）不兼容。如果使用了热添加且 VM 休眠，它休眠时的内存比初始时多。但当 VM 从休眠中恢复时，Hyper-V 只给 VM 原本分配的内存，内存大小不匹配导致恢复失败。)

To enable a Hyper-V VM for hibernation, the Hyper-V administrator must
enable the ACPI virtual S4 sleep state in the ACPI configuration that
Hyper-V provides to the guest VM. Such enablement is accomplished by
modifying a WMI property of the VM, the steps for which are outside
the scope of this documentation but are available on the web.
Enablement is treated as the indicator that the administrator
prioritizes Linux hibernation in the VM over hot-add, so the Hyper-V
balloon driver in Linux disables hot-add. Enablement is indicated if
the contents of /sys/power/disk contains "platform" as an option. The
enablement is also visible in /sys/bus/vmbus/hibernation. See function
hv_is_hibernation_supported().
(要为一台 Hyper-V VM 启用休眠，Hyper-V 管理员必须在 Hyper-V 提供给客户机 VM 的 ACPI 配置中启用 ACPI 虚拟 S4 睡眠状态。这一启用通过修改该 VM 的一个 WMI 属性完成，其步骤不在本文档范围内，但可在网上查到。启用被视为管理员在 VM 中把 Linux 休眠优先于热添加的标志，因此 Linux 中的 Hyper-V 气球驱动会禁用热添加。如果 /sys/power/disk 的内容中包含 "platform" 选项，即表示已启用。该启用状态也可以在 /sys/bus/vmbus/hibernation 中看到。参见函数 hv_is_hibernation_supported()。)

Linux supports ACPI sleep states on x86, but not on arm64. So Linux
guest VM hibernation is not available on Hyper-V for arm64.
(Linux 在 x86 上支持 ACPI 睡眠状态，但在 arm64 上不支持。因此，arm64 的 Hyper-V 上无法使用 Linux 客户机 VM 休眠。)

Initiating Guest VM Hibernation(发起客户机 VM 休眠)
---------------------------------------------------
Guest VMs can self-initiate hibernation using the standard Linux
methods of writing "disk" to /sys/power/state or the reboot system
call. As an additional layer, Linux guests on Hyper-V support the
"Shutdown" integration service, via which a Hyper-V administrator can
tell a Linux VM to hibernate using a command outside the VM. The
command generates a request to the Hyper-V shutdown driver in Linux,
which sends the uevent "EVENT=hibernate". See kernel functions
shutdown_onchannelcallback() and send_hibernate_uevent(). A udev rule
must be provided in the VM that handles this event and initiates
hibernation.
(客户机 VM 可以用标准的 Linux 方法自行发起休眠：向 /sys/power/state 写入 "disk" 或调用 reboot 系统调用。作为额外的一层，Hyper-V 上的 Linux 客户机支持 "Shutdown" 集成服务，Hyper-V 管理员可以通过它在 VM 之外用一条命令让 Linux VM 休眠。该命令会向 Linux 中的 Hyper-V 关机驱动生成一个请求，后者发送 uevent "EVENT=hibernate"。参见内核函数 shutdown_onchannelcallback() 和 send_hibernate_uevent()。VM 中必须提供处理该事件并发起休眠的 udev 规则。)

Handling VMBus Devices During Hibernation & Resume(休眠与恢复期间对 VMBus 设备的处理)
-------------------------------------------------------------------------------------
The VMBus bus driver, and the individual VMBus device drivers,
implement suspend and resume functions that are called as part of the
Linux orchestration of hibernation and of resuming from hibernation.
The overall approach is to leave in place the data structures for the
primary VMBus channels and their associated Linux devices, such as
SCSI controllers and others, so that they are captured in the
hibernation image. This approach allows any state associated with the
device to be persisted across the hibernation/resume. When the VM
resumes, the devices are re-offered by Hyper-V and are connected to
the data structures that already exist in the resumed hibernation
image.
(VMBus 总线驱动和各个 VMBus 设备驱动都实现了挂起与恢复函数，它们作为 Linux 编排休眠和从休眠恢复的一部分被调用。总体做法是保留主 VMBus 通道及其关联的 Linux 设备（如 SCSI 控制器等）的数据结构，使其被捕获进休眠映像。这一做法让与设备相关联的任何状态都能跨休眠/恢复保存。当 VM 恢复时，Hyper-V 会重新提供这些设备，并把它们连接到恢复后的休眠映像中已存在的数据结构上。)

VMBus devices are identified by class and instance GUID. (See section
"VMBus device creation/deletion" in
Documentation/virt/hyperv/vmbus.rst.) Upon resume from hibernation,
the resume functions expect that the devices offered by Hyper-V have
the same class/instance GUIDs as the devices present at the time of
hibernation. Having the same class/instance GUIDs allows the offered
devices to be matched to the primary VMBus channel data structures in
the memory of the now resumed hibernation image. If any devices are
offered that don't match primary VMBus channel data structures that
already exist, they are processed normally as newly added devices. If
primary VMBus channels that exist in the resumed hibernation image are
not matched with a device offered in the resumed VM, the resume
sequence waits for 10 seconds, then proceeds. But the unmatched device
is likely to cause errors in the resumed VM.
(VMBus 设备由类 GUID 和实例 GUID 标识。（参见 Documentation/virt/hyperv/vmbus.rst 中 "VMBus device creation/deletion" 一节。）从休眠恢复时，恢复函数期望 Hyper-V 提供的设备与休眠时存在的设备具有相同的类/实例 GUID。类/实例 GUID 相同，才能把新提供的设备匹配到恢复后的休眠映像内存中的主 VMBus 通道数据结构。如果提供的设备与已存在的主 VMBus 通道数据结构不匹配，它们会作为新增设备被正常处理。如果恢复后的休眠映像中存在的主 VMBus 通道没有与恢复后 VM 中提供的设备匹配上，恢复流程会等待 10 秒后继续，但未匹配的设备很可能在恢复后的 VM 中引发错误。)

When resuming existing primary VMBus channels, the newly offered
relids might be different because relids can change on each VM boot,
even if the VM configuration hasn't changed. The VMBus bus driver
resume function matches the class/instance GUIDs, and updates the
relids in case they have changed.
(恢复已存在的主 VMBus 通道时，新提供的 relid 可能不同，因为即使 VM 配置没有改变，relid 也可能在每次 VM 启动时变化。VMBus 总线驱动的恢复函数会匹配类/实例 GUID，并在 relid 发生变化时更新它们。)

VMBus sub-channels are not persisted in the hibernation image. Each
VMBus device driver's suspend function must close any sub-channels
prior to hibernation. Closing a sub-channel causes Hyper-V to send a
RESCIND_CHANNELOFFER message, which Linux processes by freeing the
channel data structures so that all vestiges of the sub-channel are
removed. By contrast, primary channels are marked closed and their
ring buffers are freed, but Hyper-V does not send a rescind message,
so the channel data structure continues to exist. Upon resume, the
device driver's resume function re-allocates the ring buffer and
re-opens the existing channel. It then communicates with Hyper-V to
re-open sub-channels from scratch.
(VMBus 子通道不会保存在休眠映像中。每个 VMBus 设备驱动的挂起函数必须在休眠之前关闭所有子通道。关闭子通道会使 Hyper-V 发送 RESCIND_CHANNELOFFER 消息，Linux 处理该消息时释放通道数据结构，从而清除子通道的所有痕迹。相比之下，主通道被标记为关闭并释放其环形缓冲区，但 Hyper-V 不会发送 rescind 消息，因此通道数据结构继续存在。恢复时，设备驱动的恢复函数重新分配环形缓冲区并重新打开已存在的通道，然后与 Hyper-V 通信，从头重建子通道。)

The Linux ends of Hyper-V sockets are forced closed at the time of
hibernation. The guest can't force closing the host end of the socket,
but any host-side actions on the host end will produce an error.
(休眠时，Hyper-V socket 的 Linux 端会被强制关闭。客户机无法强制关闭 socket 的主机端，但主机端上的任何主机侧操作都会产生错误。)

VMBus devices use the same suspend function for the "freeze" and the
"poweroff" phases, and the same resume function for the "thaw" and
"restore" phases. See the "Entering Hibernation" section of
Documentation/driver-api/pm/devices.rst for the sequencing of the
phases.
(VMBus 设备在 "freeze" 和 "poweroff" 阶段使用同一个挂起函数，在 "thaw" 和 "restore" 阶段使用同一个恢复函数。各阶段的先后次序参见 Documentation/driver-api/pm/devices.rst 的 "Entering Hibernation" 一节。)

Detailed Hibernation Sequence(休眠详细流程)
-------------------------------------------
1. The Linux power management (PM) subsystem prepares for
   hibernation by freezing user space processes and allocating
   memory to hold the hibernation image.
   (Linux 电源管理（PM）子系统通过冻结用户空间进程并分配内存来存放休眠映像，为休眠做准备。)
2. As part of the "freeze" phase, Linux PM calls the "suspend"
   function for each VMBus device in turn. As described above, this
   function removes sub-channels, and leaves the primary channel in
   a closed state.
   (作为 "freeze" 阶段的一部分，Linux PM 依次为每个 VMBus 设备调用 "suspend" 函数。如上所述，该函数移除子通道，并让主通道处于关闭状态。)
3. Linux PM calls the "suspend" function for the VMBus bus, which
   closes any Hyper-V socket channels and unloads the top-level
   VMBus connection with the Hyper-V host.
   (Linux PM 为 VMBus 总线调用 "suspend" 函数，它会关闭所有 Hyper-V socket 通道，并卸载与 Hyper-V 主机的顶层 VMBus 连接。)
4. Linux PM disables non-boot CPUs, creates the hibernation image in
   the previously allocated memory, then re-enables non-boot CPUs.
   The hibernation image contains the memory data structures for the
   closed primary channels, but no sub-channels.
   (Linux PM 禁用非引导 CPU，在先前分配的内存中创建休眠映像，然后重新启用非引导 CPU。休眠映像包含已关闭主通道的内存数据结构，但不包含子通道。)
5. As part of the "thaw" phase, Linux PM calls the "resume" function
   for the VMBus bus, which re-establishes the top-level VMBus
   connection and requests that Hyper-V re-offer the VMBus devices.
   As offers are received for the primary channels, the relids are
   updated as previously described.
   (作为 "thaw" 阶段的一部分，Linux PM 为 VMBus 总线调用 "resume" 函数，它重新建立顶层 VMBus 连接，并请求 Hyper-V 重新提供 VMBus 设备。随着主通道的 offer 被接收，relid 按前述方式更新。)
6. Linux PM calls the "resume" function for each VMBus device. Each
   device re-opens its primary channel, and communicates with Hyper-V
   to re-establish sub-channels if appropriate. The sub-channels
   are re-created as new channels since they were previously removed
   entirely in Step 2.
   (Linux PM 为每个 VMBus 设备调用 "resume" 函数。每个设备重新打开其主通道，并在适当时与 Hyper-V 通信以重建子通道。由于子通道在第 2 步中已被完全移除，因此它们作为新通道重新创建。)
7. With VMBus devices now working again, Linux PM writes the
   hibernation image from memory to disk.
   (在 VMBus 设备重新工作之后，Linux PM 把休眠映像从内存写入磁盘。)
8. Linux PM repeats Steps 2 and 3 above as part of the "poweroff"
   phase. VMBus channels are closed and the top-level VMBus
   connection is unloaded.
   (作为 "poweroff" 阶段的一部分，Linux PM 重复上面的第 2 和第 3 步。VMBus 通道被关闭，顶层 VMBus 连接被卸载。)
9. Linux PM disables non-boot CPUs, and then enters ACPI sleep state
   S4. Hibernation is now complete.
   (Linux PM 禁用非引导 CPU，然后进入 ACPI 睡眠状态 S4。至此休眠完成。)

Detailed Resume Sequence(恢复详细流程)
--------------------------------------
1. The guest VM boots into a fresh Linux OS instance. During boot,
   the top-level VMBus connection is established, and synthetic
   devices are enabled. This happens via the normal paths that don't
   involve hibernation.
   (客户机 VM 引导进入一个全新的 Linux OS 实例。引导期间，顶层 VMBus 连接被建立，合成设备被启用。这一过程通过不涉及休眠的常规路径完成。)
2. Linux PM hibernation code reads swap space is to find and read
   the hibernation image into memory. If there is no hibernation
   image, then this boot becomes a normal boot.
   (Linux PM 休眠代码读取 swap 空间，找到休眠映像并读入内存。如果没有休眠映像，则这次引导就是一次普通引导。)
3. If this is a resume from hibernation, the "freeze" phase is used
   to shutdown VMBus devices and unload the top-level VMBus
   connection in the running fresh OS instance, just like Steps 2
   and 3 in the hibernation sequence.
   (如果这是一次从休眠中的恢复，则用 "freeze" 阶段在正在运行的新 OS 实例中关闭 VMBus 设备并卸载顶层 VMBus 连接，就像休眠流程中的第 2 和第 3 步一样。)
4. Linux PM disables non-boot CPUs, and transfers control to the
   read-in hibernation image. In the now-running hibernation image,
   non-boot CPUs are restarted.
   (Linux PM 禁用非引导 CPU，并把控制权移交给读入的休眠映像。在此时运行的休眠映像中，非引导 CPU 被重新启动。)
5. As part of the "resume" phase, Linux PM repeats Steps 5 and 6
   from the hibernation sequence. The top-level VMBus connection is
   re-established, and offers are received and matched to primary
   channels in the image. Relids are updated. VMBus device resume
   functions re-open primary channels and re-create sub-channels.
   (作为 "resume" 阶段的一部分，Linux PM 重复休眠流程中的第 5 和第 6 步。顶层 VMBus 连接被重新建立，offer 被接收并与映像中的主通道匹配。relid 被更新。VMBus 设备恢复函数重新打开主通道并重建子通道。)
6. Linux PM exits the hibernation resume sequence and the VM is now
   running normally from the hibernation image.
   (Linux PM 退出休眠恢复流程，VM 此时已从休眠映像中正常运行。)

Key-Value Pair (KVP) Pseudo-Device Anomalies(KVP 伪设备的异常行为)
------------------------------------------------------------------
The VMBus KVP device behaves differently from other pseudo-devices
offered by Hyper-V.  When the KVP primary channel is closed, Hyper-V
sends a rescind message, which causes all vestiges of the device to be
removed. But Hyper-V then re-offers the device, causing it to be newly
re-created. The removal and re-creation occurs during the "freeze"
phase of hibernation, so the hibernation image contains the re-created
KVP device. Similar behavior occurs during the "freeze" phase of the
resume sequence while still in the fresh OS instance. But in both
cases, the top-level VMBus connection is subsequently unloaded, which
causes the device to be discarded on the Hyper-V side. So no harm is
done and everything still works.
(VMBus KVP 设备的行为与 Hyper-V 提供的其他伪设备不同。当 KVP 主通道关闭时，Hyper-V 会发送 rescind 消息，使该设备的所有痕迹被移除。但随后 Hyper-V 又重新提供该设备，使其被重新创建。移除与重建发生在休眠的 "freeze" 阶段，因此休眠映像中包含的是重建后的 KVP 设备。在恢复流程的 "freeze" 阶段（尚处于全新 OS 实例中时）也会发生类似行为。但在这两种情况下，顶层 VMBus 连接随后都会被卸载，这使 Hyper-V 侧丢弃该设备。因此并无危害，一切照常工作。)

Virtual PCI devices(虚拟 PCI 设备)
----------------------------------
Virtual PCI devices are physical PCI devices that are mapped directly
into the VM's physical address space so the VM can interact directly
with the hardware. vPCI devices include those accessed via what Hyper-V
calls "Discrete Device Assignment" (DDA), as well as SR-IOV NIC
Virtual Functions (VF) devices. See Documentation/virt/hyperv/vpci.rst.
(虚拟 PCI 设备是被直接映射进 VM 物理地址空间的物理 PCI 设备，VM 因此可以直接与硬件交互。vPCI 设备包括通过 Hyper-V 所谓 "Discrete Device Assignment"（DDA）访问的设备，以及 SR-IOV 网卡虚拟功能（VF）设备。参见 Documentation/virt/hyperv/vpci.rst。)

Hyper-V DDA devices are offered to guest VMs after the top-level VMBus
connection is established, just like VMBus synthetic devices. They are
statically assigned to the VM, and their instance GUIDs don't change
unless the Hyper-V administrator makes changes to the configuration.
DDA devices are represented in Linux as virtual PCI devices that have
a VMBus identity as well as a PCI identity. Consequently, Linux guest
hibernation first handles DDA devices as VMBus devices in order to
manage the VMBus channel. But then they are also handled as PCI
devices using the hibernation functions implemented by their native
PCI driver.
(Hyper-V DDA 设备在顶层 VMBus 连接建立之后才提供给客户机 VM，与 VMBus 合成设备一样。它们被静态分配给 VM，除非 Hyper-V 管理员修改配置，其实例 GUID 不会改变。DDA 设备在 Linux 中表示为既有 VMBus 身份又有 PCI 身份的虚拟 PCI 设备。因此，Linux 客户机休眠首先把 DDA 设备当作 VMBus 设备处理以管理 VMBus 通道，然后再当作 PCI 设备、用其原生 PCI 驱动实现的休眠函数来处理。)

SR-IOV NIC VFs also have a VMBus identity as well as a PCI
identity, and overall are processed similarly to DDA devices. A
difference is that VFs are not offered to the VM during initial boot
of the VM. Instead, the VMBus synthetic NIC driver first starts
operating and communicates to Hyper-V that it is prepared to accept a
VF, and then the VF offer is made. However, the VMBus connection
might later be unloaded and then re-established without the VM being
rebooted, as happens in Steps 3 and 5 in the Detailed Hibernation
Sequence above and in the Detailed Resume Sequence. In such a case,
the VFs likely became part of the VM during initial boot, so when the
VMBus connection is re-established, the VFs are offered on the
re-established connection without intervention by the synthetic NIC driver.
(SR-IOV 网卡 VF 同样既有 VMBus 身份又有 PCI 身份，总体上按与 DDA 设备类似的方式处理。区别在于，VM 初次引导时 VF 并不会提供给 VM。而是先由 VMBus 合成网卡驱动开始工作，并向 Hyper-V 表明它已准备好接受 VF，然后才发出 VF 的 offer。然而，VMBus 连接之后可能在不重启 VM 的情况下被卸载再重新建立——就像上文休眠详细流程和恢复详细流程中的第 3 步与第 5 步那样。这种情况下，VF 很可能在 VM 初次引导时就已成为 VM 的一部分，因此当 VMBus 连接重新建立时，VF 会在重建的连接上直接被提供，无需合成网卡驱动干预。)

UIO Devices(UIO 设备)
---------------------
A VMBus device can be exposed to user space using the Hyper-V UIO
driver (uio_hv_generic.c) so that a user space driver can control and
operate the device. However, the VMBus UIO driver does not support the
suspend and resume operations needed for hibernation. If a VMBus
device is configured to use the UIO driver, hibernating the VM fails
and Linux continues to run normally. The most common use of the Hyper-V
UIO driver is for DPDK networking, but there are other uses as well.
(VMBus 设备可以通过 Hyper-V UIO 驱动（uio_hv_generic.c）暴露给用户空间，让用户空间驱动控制并操作该设备。但 VMBus UIO 驱动不支持休眠所需的挂起与恢复操作。如果某个 VMBus 设备被配置为使用 UIO 驱动，VM 的休眠会失败，Linux 继续正常运行。Hyper-V UIO 驱动最常见的用途是 DPDK 网络，但也有其他用途。)

Resuming on a Different VM(在另一台 VM 上恢复)
----------------------------------------------
This scenario occurs in the Azure public cloud in that a hibernated
customer VM only exists as saved configuration and disks -- the VM no
longer exists on any Hyper-V host. When the customer VM is resumed, a
new Hyper-V VM with identical configuration is created, likely on a
different Hyper-V host. That new Hyper-V VM becomes the resumed
customer VM, and the steps the Linux kernel takes to resume from the
hibernation image must work in that new VM.
(这种场景出现在 Azure 公有云中：已休眠的客户 VM 只以保存的配置和磁盘的形式存在——该 VM 不再存在于任何 Hyper-V 主机上。当客户 VM 恢复时，会创建一台配置完全相同的新 Hyper-V VM，很可能位于另一台 Hyper-V 主机上。这台新的 Hyper-V VM 就成为恢复后的客户 VM，Linux 内核从休眠映像恢复所执行的步骤必须能在新 VM 中正常工作。)

While the disks and their contents are preserved from the original VM,
the Hyper-V-provided VMBus instance GUIDs of the disk controllers and
other synthetic devices would typically be different. The difference
would cause the resume from hibernation to fail, so several things are
done to solve this problem:
(虽然磁盘及其内容从原 VM 保留了下来，但磁盘控制器和其他合成设备由 Hyper-V 分配的 VMBus 实例 GUID 通常会不同。这种差异会导致从休眠中恢复失败，因此采取了几项措施来解决这个问题：)

* For VMBus synthetic devices that support only a single instance,
  Hyper-V always assigns the same instance GUIDs. For example, the
  Hyper-V mouse, the shutdown pseudo-device, the time sync pseudo
  device, etc., always have the same instance GUID, both for local
  Hyper-V installs as well as in the Azure cloud.
  (对于只支持单一实例的 VMBus 合成设备，Hyper-V 总是分配相同的实例 GUID。例如 Hyper-V 鼠标、关机伪设备、时间同步伪设备等，无论在本地 Hyper-V 安装还是在 Azure 云中，都始终具有相同的实例 GUID。)

* VMBus synthetic SCSI controllers may have multiple instances in a
  VM, and in the general case instance GUIDs vary from VM to VM.
  However, Azure VMs always have exactly two synthetic SCSI
  controllers, and Azure code overrides the normal Hyper-V behavior
  so these controllers are always assigned the same two instance
  GUIDs. Consequently, when a customer VM is resumed on a newly
  created VM, the instance GUIDs match. But this guarantee does not
  hold for local Hyper-V installs.
  (VMBus 合成 SCSI 控制器在一台 VM 中可能有多个实例，一般情况下实例 GUID 因 VM 而异。不过 Azure VM 总是恰好有两个合成 SCSI 控制器，Azure 代码覆盖了 Hyper-V 的正常行为，使这两个控制器总是被分配相同的两个实例 GUID。因此，当客户 VM 在新建的 VM 上恢复时，实例 GUID 能够匹配。但这一保证不适用于本地 Hyper-V 安装。)

* Similarly, VMBus synthetic NICs may have multiple instances in a
  VM, and the instance GUIDs vary from VM to VM. Again, Azure code
  overrides the normal Hyper-V behavior so that the instance GUID
  of a synthetic NIC in a customer VM does not change, even if the
  customer VM is deallocated or hibernated, and then re-constituted
  on a newly created VM. As with SCSI controllers, this behavior
  does not hold for local Hyper-V installs.
  (类似地，VMBus 合成网卡在一台 VM 中可能有多个实例，实例 GUID 因 VM 而异。同样，Azure 代码覆盖了 Hyper-V 的正常行为，使客户 VM 中合成网卡的实例 GUID 不会改变——即使该客户 VM 被释放或休眠、然后在新建的 VM 上重新组建。与 SCSI 控制器一样，这一行为不适用于本地 Hyper-V 安装。)

* vPCI devices do not have the same instance GUIDs when resuming
  from hibernation on a newly created VM. Consequently, Azure does
  not support hibernation for VMs that have DDA devices such as
  NVMe controllers or GPUs. For SR-IOV NIC VFs, Azure removes the
  VF from the VM before it hibernates so that the hibernation image
  does not contain a VF device. When the VM is resumed it
  instantiates a new VF, rather than trying to match against a VF
  that is present in the hibernation image. Because Azure must
  remove any VFs before initiating hibernation, Azure VM
  hibernation must be initiated externally from the Azure Portal or
  Azure CLI, which in turn uses the Shutdown integration service to
  tell Linux to do the hibernation. If hibernation is self-initiated
  within the Azure VM, VFs remain in the hibernation image, and are
  not resumed properly.
  (在新建的 VM 上从休眠恢复时，vPCI 设备的实例 GUID 并不相同。因此 Azure 不支持含有 DDA 设备（如 NVMe 控制器或 GPU）的 VM 休眠。对于 SR-IOV 网卡 VF，Azure 会在 VM 休眠之前把 VF 从 VM 中移除，使休眠映像中不包含 VF 设备。VM 恢复时会实例化一个新的 VF，而不是尝试与休眠映像中的 VF 匹配。由于 Azure 必须在发起休眠之前移除所有 VF，Azure VM 的休眠必须从 Azure Portal 或 Azure CLI 外部发起，后者再通过 Shutdown 集成服务通知 Linux 执行休眠。如果休眠是在 Azure VM 内部自行发起的，VF 会留在休眠映像中，从而无法正确恢复。)

In summary, Azure takes special actions to remove VFs and to ensure
that VMBus device instance GUIDs match on a new/different VM, allowing
hibernation to work for most general-purpose Azure VMs sizes. While
similar special actions could be taken when resuming on a different VM
on a local Hyper-V install, orchestrating such actions is not provided
out-of-the-box by local Hyper-V and so requires custom scripting.
(总之，Azure 采取特殊措施移除 VF，并确保 VMBus 设备实例 GUID 在新的/不同的 VM 上相互匹配，使休眠对大多数通用规格的 Azure VM 有效。虽然当本地 Hyper-V 安装上的恢复发生在另一台 VM 上时也可以采取类似的特殊措施，但本地 Hyper-V 并不提供开箱即用的此类编排，因此需要自定义脚本。)
