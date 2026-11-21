==========================================================
Dell Systems Management Base Driver(Dell 系统管理基础驱动)
==========================================================

Overview(概述)
==============

The Dell Systems Management Base Driver provides a sysfs interface for
systems management software such as Dell OpenManage to perform system
management interrupts and host control actions (system power cycle or
power off after OS shutdown) on certain Dell systems.
(Dell 系统管理基础驱动为 Dell OpenManage 等系统管理软件提供一个 sysfs
接口，用于在某些 Dell 系统上执行系统管理中断和主机控制操作(操作系统
关机后的系统重新上电或断电)。)

Dell OpenManage requires this driver on the following Dell PowerEdge systems:
300, 1300, 1400, 400SC, 500SC, 1500SC, 1550, 600SC, 1600SC, 650, 1655MC,
700, and 750.  Other Dell software such as the open source libsmbios project
is expected to make use of this driver, and it may include the use of this
driver on other Dell systems.
(Dell OpenManage 在以下 Dell PowerEdge 系统上需要此驱动：300、1300、
1400、400SC、500SC、1500SC、1550、600SC、1600SC、650、1655MC、700 和
750。其他 Dell 软件(例如开源的 libsmbios 项目)预期也会使用此驱动，
并且可能把对此驱动的使用扩展到其他 Dell 系统上。)

The Dell libsmbios project aims towards providing access to as much BIOS
information as possible.  See http://linux.dell.com/libsmbios/main/ for
more information about the libsmbios project.
(Dell libsmbios 项目旨在提供对尽可能多的 BIOS 信息的访问。有关
libsmbios 项目的更多信息，参见 http://linux.dell.com/libsmbios/main/。)


System Management Interrupt(系统管理中断)
=========================================

On some Dell systems, systems management software must access certain
management information via a system management interrupt (SMI).  The SMI data
buffer must reside in 32-bit address space, and the physical address of the
buffer is required for the SMI.  The driver maintains the memory required for
the SMI and provides a way for the application to generate the SMI.
The driver creates the following sysfs entries for systems management
software to perform these system management interrupts::

	/sys/devices/platform/dcdbas/smi_data
	/sys/devices/platform/dcdbas/smi_data_buf_phys_addr
	/sys/devices/platform/dcdbas/smi_data_buf_size
	/sys/devices/platform/dcdbas/smi_request

(在某些 Dell 系统上，系统管理软件必须通过系统管理中断(SMI)访问某些
管理信息。SMI 数据缓冲区必须位于 32 位地址空间中，并且 SMI 需要该
缓冲区的物理地址。驱动程序维护 SMI 所需的内存，并为应用程序提供产生
SMI 的途径。驱动程序创建以下 sysfs 条目供系统管理软件执行这些系统
管理中断：)

Systems management software must perform the following steps to execute
a SMI using this driver:
(系统管理软件必须执行以下步骤，以使用此驱动程序执行 SMI：)

1) Lock smi_data.
   (1) 锁定 smi_data。)
2) Write system management command to smi_data.
   (2) 把系统管理命令写入 smi_data。)
3) Write "1" to smi_request to generate a calling interface SMI or
   "2" to generate a raw SMI.
   (3) 向 smi_request 写入 "1" 以产生调用接口 SMI，或写入 "2" 以产生
   原始 SMI。)
4) Read system management command response from smi_data.
   (4) 从 smi_data 读取系统管理命令的响应。)
5) Unlock smi_data.
   (5) 解锁 smi_data。)


Host Control Action(主机控制操作)
=================================

Dell OpenManage supports a host control feature that allows the administrator
to perform a power cycle or power off of the system after the OS has finished
shutting down.  On some Dell systems, this host control feature requires that
a driver perform a SMI after the OS has finished shutting down.
(Dell OpenManage 支持主机控制功能，允许管理员在操作系统完成关机后对
系统执行重新上电(power cycle)或断电(power off)。在某些 Dell 系统上，
此主机控制功能要求驱动程序在操作系统完成关机后执行一次 SMI。)

The driver creates the following sysfs entries for systems management software
to schedule the driver to perform a power cycle or power off host control
action after the system has finished shutting down:
(驱动程序创建以下 sysfs 条目，供系统管理软件调度驱动程序在系统完成
关机后执行重新上电或断电主机控制操作：)

/sys/devices/platform/dcdbas/host_control_action
/sys/devices/platform/dcdbas/host_control_smi_type
/sys/devices/platform/dcdbas/host_control_on_shutdown

Dell OpenManage performs the following steps to execute a power cycle or
power off host control action using this driver:
(Dell OpenManage 使用此驱动程序执行重新上电或断电主机控制操作的步骤
如下：)

1) Write host control action to be performed to host_control_action.
   (1) 把要执行的主机控制操作写入 host_control_action。)
2) Write type of SMI that driver needs to perform to host_control_smi_type.
   (2) 把驱动程序需要执行的 SMI 类型写入 host_control_smi_type。)
3) Write "1" to host_control_on_shutdown to enable host control action.
   (3) 向 host_control_on_shutdown 写入 "1" 以启用主机控制操作。)
4) Initiate OS shutdown.
   (4) 启动操作系统关机。
   (Driver will perform host control SMI when it is notified that the OS
   has finished shutting down.)
   (当驱动程序收到操作系统已完成关机的通知时，它会执行主机控制
   SMI。)


Host Control SMI Type(主机控制 SMI 类型)
========================================

The following table shows the value to write to host_control_smi_type to
perform a power cycle or power off host control action:
(下表给出了为执行重新上电或断电主机控制操作而应写入
host_control_smi_type 的值：)

=================== =====================
PowerEdge System    Host Control SMI Type(主机控制 SMI 类型)
=================== =====================
      300             HC_SMITYPE_TYPE1
     1300             HC_SMITYPE_TYPE1
     1400             HC_SMITYPE_TYPE2
      500SC           HC_SMITYPE_TYPE2
     1500SC           HC_SMITYPE_TYPE2
     1550             HC_SMITYPE_TYPE2
      600SC           HC_SMITYPE_TYPE2
     1600SC           HC_SMITYPE_TYPE2
      650             HC_SMITYPE_TYPE2
     1655MC           HC_SMITYPE_TYPE2
      700             HC_SMITYPE_TYPE3
      750             HC_SMITYPE_TYPE3
=================== =====================
