.. SPDX-License-Identifier: GPL-2.0

==================================
VFIO virtual device(VFIO 虚拟设备)
==================================

Device types supported:
(支持的设备类型：)

  - KVM_DEV_TYPE_VFIO

Only one VFIO instance may be created per VM.  The created device
tracks VFIO files (group or device) in use by the VM and features
of those groups/devices important to the correctness and acceleration
of the VM.  As groups/devices are enabled and disabled for use by the
VM, KVM should be updated about their presence.  When registered with
KVM, a reference to the VFIO file is held by KVM.
(每个虚拟机只能创建一个 VFIO 实例。所创建的设备跟踪虚拟机正在使用的 VFIO
文件 (组或设备)，以及这些组/设备对虚拟机的正确性和加速非常重要的特性。
当组/设备被启用或停用供虚拟机使用时，应当把它们的存在情况告知 KVM。注册
到 KVM 之后，KVM 会持有对该 VFIO 文件的引用。)

Groups:
(组：)
  KVM_DEV_VFIO_FILE
	alias: KVM_DEV_VFIO_GROUP

KVM_DEV_VFIO_FILE attributes:
(KVM_DEV_VFIO_FILE 属性：)
  KVM_DEV_VFIO_FILE_ADD: Add a VFIO file (group/device) to VFIO-KVM device
	tracking(把一个 VFIO 文件 (组/设备) 加入 VFIO-KVM 设备跟踪)

	kvm_device_attr.addr points to an int32_t file descriptor for the
	VFIO file.
	(kvm_device_attr.addr 指向该 VFIO 文件的一个 int32_t 文件描述符。)

  KVM_DEV_VFIO_FILE_DEL: Remove a VFIO file (group/device) from VFIO-KVM
	device tracking(把一个 VFIO 文件 (组/设备) 从 VFIO-KVM 设备跟踪中移除)

	kvm_device_attr.addr points to an int32_t file descriptor for the
	VFIO file.
	(kvm_device_attr.addr 指向该 VFIO 文件的一个 int32_t 文件描述符。)

KVM_DEV_VFIO_GROUP (legacy kvm device group restricted to the handling of VFIO group fd):(KVM_DEV_VFIO_GROUP (仅限处理 VFIO 组 fd 的旧式 kvm 设备组)：)
  KVM_DEV_VFIO_GROUP_ADD: same as KVM_DEV_VFIO_FILE_ADD for group fd only(与 KVM_DEV_VFIO_FILE_ADD 相同，但仅适用于组 fd)

  KVM_DEV_VFIO_GROUP_DEL: same as KVM_DEV_VFIO_FILE_DEL for group fd only(与 KVM_DEV_VFIO_FILE_DEL 相同，但仅适用于组 fd)

  KVM_DEV_VFIO_GROUP_SET_SPAPR_TCE: attaches a guest visible TCE table
	allocated by sPAPR KVM.(挂接一个由 sPAPR KVM 分配的、客户机可见的 TCE 表。)
	kvm_device_attr.addr points to a struct::

		struct kvm_vfio_spapr_tce {
			__s32	groupfd;
			__s32	tablefd;
		};

	where:

	- @groupfd is a file descriptor for a VFIO group;
	- @tablefd is a file descriptor for a TCE table allocated via
	  KVM_CREATE_SPAPR_TCE.

(其中：@groupfd 是一个 VFIO 组的文件描述符；@tablefd 是通过
KVM_CREATE_SPAPR_TCE 分配的 TCE 表的文件描述符。)

The FILE/GROUP_ADD operation above should be invoked prior to accessing the
device file descriptor via VFIO_GROUP_GET_DEVICE_FD in order to support
drivers which require a kvm pointer to be set in their .open_device()
callback.  It is the same for device file descriptor via character device
open which gets device access via VFIO_DEVICE_BIND_IOMMUFD.  For such file
descriptors, FILE_ADD should be invoked before VFIO_DEVICE_BIND_IOMMUFD
to support the drivers mentioned in prior sentence as well.
(为了支持那些要求在 .open_device() 回调中设置 kvm 指针的驱动，上面的
FILE/GROUP_ADD 操作应当在通过 VFIO_GROUP_GET_DEVICE_FD 访问设备文件描述符
之前调用。对于通过字符设备打开、经由 VFIO_DEVICE_BIND_IOMMUFD 获得设备访问
的设备文件描述符，情况相同。对于此类文件描述符，也应在
VFIO_DEVICE_BIND_IOMMUFD 之前调用 FILE_ADD，以同样支持前文提到的驱动。)
