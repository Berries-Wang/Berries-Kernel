=========================
Ioctl Numbers(Ioctl 编号)
=========================

19 October 1999(1999 年 10 月 19 日)

Michael Elizabeth Chastain
<mec@shout.net>

If you are adding new ioctl's to the kernel, you should use the _IO
macros defined in <linux/ioctl.h>:
(如果要向内核添加新的 ioctl，应当使用 <linux/ioctl.h> 中定义的 _IO 宏：)

    ====== ===========================
    macro  parameters(参数)
    ====== ===========================
    _IO    none
    _IOW   write (read from userspace)
    _IOR   read (write to userspace)
    _IOWR  write and read
    ====== ===========================

'Write' and 'read' are from the user's point of view, just like the
system calls 'write' and 'read'.  For example, a SET_FOO ioctl would
be _IOW, although the kernel would actually read data from user space;
a GET_FOO ioctl would be _IOR, although the kernel would actually write
data to user space.
(这里的 'Write' 与 'read' 都是从用户的视角来看的，与系统调用 'write' 和 'read' 一样。例如，SET_FOO ioctl 应当用 _IOW，尽管内核实际上是从用户空间读取数据；而 GET_FOO ioctl 应当用 _IOR，尽管内核实际上是向用户空间写入数据。)

The first argument to the macros is an identifying letter or number from
the table below. Because of the large number of drivers, many drivers
share a partial letter with other drivers.
(宏的第一个参数是标识字母或编号，取自下表。由于驱动数量庞大，许多驱动会与其他驱动共享同一个字母的一部分区间。)

If you are writing a driver for a new device and need a letter, pick an
unused block with enough room for expansion: 32 to 256 ioctl commands
should suffice. You can register the block by patching this file and
submitting the patch through :doc:`usual patch submission process
</process/submitting-patches>`.
(如果你要为新设备编写驱动并需要一个字母，请挑选一个留有足够扩展空间的未使用区块：32 到 256 个 ioctl 命令通常就够了。你可以通过修改本文件来注册该区块，并按照常规补丁提交流程提交补丁。)

The second argument is a sequence number to distinguish ioctls from each
other. The third argument (not applicable to _IO) is the type of the data
going into the kernel or coming out of the kernel (e.g.  'int' or
'struct foo').
(第二个参数是用于相互区分各 ioctl 的序号。第三个参数(_IO 不适用)是传入或传出内核的数据类型(例如 'int' 或 'struct foo')。)

.. note::
   Do NOT use sizeof(arg) as the third argument as this results in your
   ioctl thinking it passes an argument of type size_t.
   (切勿把 sizeof(arg) 用作第三个参数，否则 ioctl 会以为自己传递的是一个 size_t 类型的参数。)

Some devices use their major number as the identifier; this is OK, as
long as it is unique.  Some devices are irregular and don't follow any
convention at all.
(有些设备使用自己的主设备号作为标识符；只要唯一就可以。还有些设备不规范，完全不遵循任何约定。)

Following this convention is good because:(遵循这一约定有下列好处：)

(1) Keeping the ioctl's globally unique helps error checking:
    if a program calls an ioctl on the wrong device, it will get an
    error rather than some unexpected behaviour.
    (让 ioctl 全局唯一有助于错误检查：如果程序在错误的设备上调用了 ioctl，它会得到一个错误，而不是某种意外的行为。)

(2) The 'strace' build procedure automatically finds ioctl numbers
    defined with the macros.
    (strace 的构建过程会自动找出用这些宏定义的 ioctl 号码。)

(3) 'strace' can decode numbers back into useful names when the
    numbers are unique.
    (当号码唯一时，strace 可以把号码解码回有用的名字。)

(4) People looking for ioctls can grep for them more easily when
    this convention is used to define the ioctl numbers.
    (当使用这一约定来定义 ioctl 号码时，人们可以更容易地用 grep 找到 ioctl。)

(5) When following the convention, the driver code can use generic
    code to copy the parameters between user and kernel space.
    (遵循约定时，驱动代码可以使用通用代码在用户空间和内核空间之间复制参数。)

This table lists ioctls visible from userland, excluding ones from
drivers/staging/.
(下表列出从用户空间可见的 ioctl，不包括来自 drivers/staging/ 的那些。)

====  =====  ========================================================= ================================================================
Code  Seq#    Include File(包含文件)                                       Comments(备注)
      (hex)
====  =====  ========================================================= ================================================================
0x00  00-1F  linux/fs.h                                                conflict!(冲突！)
0x00  00-1F  scsi/scsi_ioctl.h                                         conflict!(冲突！)
0x00  00-1F  linux/fb.h                                                conflict!(冲突！)
0x00  00-1F  linux/wavefront.h                                         conflict!(冲突！)
0x02  all    linux/fd.h
0x03  all    linux/hdreg.h
0x04  D2-DC  linux/umsdos_fs.h                                         Dead since 2.6.11, but don't reuse these.(自 2.6.11 起已废弃，但请勿复用这些号码。)
0x06  all    linux/lp.h
0x07  9F-D0  linux/vmw_vmci_defs.h, uapi/linux/vm_sockets.h
0x09  all    linux/raid/md_u.h
0x10  00-0F  drivers/char/s390/vmcp.h
0x10  10-1F  arch/s390/include/uapi/sclp_ctl.h
0x10  20-2F  arch/s390/include/uapi/asm/hypfs.h
0x12  all    linux/fs.h                                                BLK* ioctls(BLK* 系列 ioctl)
             linux/blkpg.h
             linux/blkzoned.h
             linux/blk-crypto.h
0x15  all    linux/fs.h                                                FS_IOC_* ioctls(FS_IOC_* 系列 ioctl)
0x1b  all                                                              InfiniBand Subsystem
                                                                       <http://infiniband.sourceforge.net/> (InfiniBand 子系统)
0x20  all    drivers/cdrom/cm206.h
0x22  all    scsi/sg.h
0x3E  00-0F  linux/counter.h                                           <mailto:linux-iio@vger.kernel.org>
'!'   00-1F  uapi/linux/seccomp.h
'#'   00-3F                                                            IEEE 1394 Subsystem
                                                                       Block for the entire subsystem (IEEE 1394 子系统，整个子系统专用区块)
'$'   00-0F  linux/perf_counter.h, linux/perf_event.h
'%'   00-0F  include/uapi/linux/stm.h                                  System Trace Module subsystem
                                                                       <mailto:alexander.shishkin@linux.intel.com> (系统跟踪模块子系统)
'&'   00-07  drivers/firewire/nosy-user.h
'*'   00-1F  uapi/linux/user_events.h                                  User Events Subsystem
                                                                       <mailto:linux-trace-kernel@vger.kernel.org> (用户事件子系统)
'1'   00-1F  linux/timepps.h                                           PPS kit from Ulrich Windl
                                                                       <ftp://ftp.de.kernel.org/pub/linux/daemons/ntp/PPS/> (Ulrich Windl 的 PPS 工具包)
'2'   01-04  linux/i2o.h
'3'   00-0F  drivers/s390/char/raw3270.h                               conflict!(冲突！)
'3'   00-1F  linux/suspend_ioctls.h,                                   conflict!(冲突！)
             kernel/power/user.c
'8'   all                                                              SNP8023 advanced NIC card
                                                                       <mailto:mcr@solidum.com> (SNP8023 高级网卡)
';'   64-7F  linux/vfio.h
';'   80-FF  linux/iommufd.h
'='   00-3f  uapi/linux/ptp_clock.h                                    <mailto:richardcochran@gmail.com>
'@'   00-0F  linux/radeonfb.h                                          conflict!(冲突！)
'@'   00-0F  drivers/video/aty/aty128fb.c                              conflict!(冲突！)
'A'   00-1F  linux/apm_bios.h                                          conflict!(冲突！)
'A'   00-0F  linux/agpgart.h,                                          conflict!(冲突！)
             drivers/char/agp/compat_ioctl.h
'A'   00-7F  sound/asound.h                                            conflict!(冲突！)
'B'   00-1F  linux/cciss_ioctl.h                                       conflict!(冲突！)
'B'   00-0F  include/linux/pmu.h                                       conflict!(冲突！)
'B'   C0-FF  advanced bbus                                             <mailto:maassen@uni-freiburg.de>
'B'   00-0F  xen/xenbus_dev.h                                          conflict!(冲突！)
'C'   all    linux/soundcard.h                                         conflict!(冲突！)
'C'   01-2F  linux/capi.h                                              conflict!(冲突！)
'C'   F0-FF  drivers/net/wan/cosa.h                                    conflict!(冲突！)
'D'   all    arch/s390/include/asm/dasd.h
'D'   40-5F  drivers/scsi/dpt/dtpi_ioctl.h                             Dead since 2022(2022 年起已废弃)
'D'   05     drivers/scsi/pmcraid.h
'E'   all    linux/input.h                                             conflict!(冲突！)
'E'   00-0F  xen/evtchn.h                                              conflict!(冲突！)
'F'   all    linux/fb.h                                                conflict!(冲突！)
'F'   01-02  drivers/scsi/pmcraid.h                                    conflict!(冲突！)
'F'   20     drivers/video/fsl-diu-fb.h                                conflict!(冲突！)
'F'   20     linux/ivtvfb.h                                            conflict!(冲突！)
'F'   20     linux/matroxfb.h                                          conflict!(冲突！)
'F'   20     drivers/video/aty/atyfb_base.c                            conflict!(冲突！)
'F'   00-0F  video/da8xx-fb.h                                          conflict!(冲突！)
'F'   80-8F  linux/arcfb.h                                             conflict!(冲突！)
'F'   DD     video/sstfb.h                                             conflict!(冲突！)
'G'   00-3F  drivers/misc/sgi-gru/grulib.h                             conflict!(冲突！)
'G'   00-0F  xen/gntalloc.h, xen/gntdev.h                              conflict!(冲突！)
'H'   00-7F  linux/hiddev.h                                            conflict!(冲突！)
'H'   00-0F  linux/hidraw.h                                            conflict!(冲突！)
'H'   01     linux/mei.h                                               conflict!(冲突！)
'H'   02     linux/mei.h                                               conflict!(冲突！)
'H'   03     linux/mei.h                                               conflict!(冲突！)
'H'   00-0F  sound/asound.h                                            conflict!(冲突！)
'H'   20-40  sound/asound_fm.h                                         conflict!(冲突！)
'H'   80-8F  sound/sfnt_info.h                                         conflict!(冲突！)
'H'   10-8F  sound/emu10k1.h                                           conflict!(冲突！)
'H'   10-1F  sound/sb16_csp.h                                          conflict!(冲突！)
'H'   10-1F  sound/hda_hwdep.h                                         conflict!(冲突！)
'H'   40-4F  sound/hdspm.h                                             conflict!(冲突！)
'H'   40-4F  sound/hdsp.h                                              conflict!(冲突！)
'H'   90     sound/usb/usx2y/usb_stream.h
'H'   00-0F  uapi/misc/habanalabs.h                                    conflict!(冲突！)
'H'   A0     uapi/linux/usb/cdc-wdm.h
'H'   C0-F0  net/bluetooth/hci.h                                       conflict!(冲突！)
'H'   C0-DF  net/bluetooth/hidp/hidp.h                                 conflict!(冲突！)
'H'   C0-DF  net/bluetooth/cmtp/cmtp.h                                 conflict!(冲突！)
'H'   C0-DF  net/bluetooth/bnep/bnep.h                                 conflict!(冲突！)
'H'   F1     linux/hid-roccat.h                                        <mailto:erazor_de@users.sourceforge.net>
'H'   F8-FA  sound/firewire.h
'I'   all    linux/isdn.h                                              conflict!(冲突！)
'I'   00-0F  drivers/isdn/divert/isdn_divert.h                         conflict!(冲突！)
'I'   40-4F  linux/mISDNif.h                                           conflict!(冲突！)
'K'   all    linux/kd.h
'L'   00-1F  linux/loop.h                                              conflict!(冲突！)
'L'   10-1F  drivers/scsi/mpt3sas/mpt3sas_ctl.h                        conflict!(冲突！)
'L'   E0-FF  linux/ppdd.h                                              encrypted disk device driver
                                                                       <http://linux01.gwdg.de/~alatham/ppdd.html> (加密磁盘设备驱动)
'M'   all    linux/soundcard.h                                         conflict!(冲突！)
'M'   01-16  mtd/mtd-abi.h                                             conflict!(冲突！)
      and    drivers/mtd/mtdchar.c
'M'   01-03  drivers/scsi/megaraid/megaraid_sas.h
'M'   00-0F  drivers/video/fsl-diu-fb.h                                conflict!(冲突！)
'N'   00-1F  drivers/usb/scanner.h
'N'   40-7F  drivers/block/nvme.c
'N'   80-8F  uapi/linux/ntsync.h                                       NT synchronization primitives
                                                                       <mailto:wine-devel@winehq.org> (NT 同步原语)
'O'   00-06  mtd/ubi-user.h                                            UBI
'P'   all    linux/soundcard.h                                         conflict!(冲突！)
'P'   60-6F  sound/sscape_ioctl.h                                      conflict!(冲突！)
'P'   00-0F  drivers/usb/class/usblp.c                                 conflict!(冲突！)
'P'   01-09  drivers/misc/pci_endpoint_test.c                          conflict!(冲突！)
'P'   00-0F  xen/privcmd.h                                             conflict!(冲突！)
'P'   00-05  linux/tps6594_pfsm.h                                      conflict!(冲突！)
'Q'   all    linux/soundcard.h
'R'   00-1F  linux/random.h                                            conflict!(冲突！)
'R'   01     linux/rfkill.h                                            conflict!(冲突！)
'R'   20-2F  linux/trace_mmap.h
'R'   C0-DF  net/bluetooth/rfcomm.h
'R'   E0     uapi/linux/fsl_mc.h
'S'   all    linux/cdrom.h                                             conflict!(冲突！)
'S'   80-81  scsi/scsi_ioctl.h                                         conflict!(冲突！)
'S'   82-FF  scsi/scsi.h                                               conflict!(冲突！)
'S'   00-7F  sound/asequencer.h                                        conflict!(冲突！)
'T'   all    linux/soundcard.h                                         conflict!(冲突！)
'T'   00-AF  sound/asound.h                                            conflict!(冲突！)
'T'   all    arch/x86/include/asm/ioctls.h                             conflict!(冲突！)
'T'   C0-DF  linux/if_tun.h                                            conflict!(冲突！)
'U'   all    sound/asound.h                                            conflict!(冲突！)
'U'   00-CF  linux/uinput.h                                            conflict!(冲突！)
'U'   00-EF  linux/usbdevice_fs.h
'U'   C0-CF  drivers/bluetooth/hci_uart.h
'V'   all    linux/vt.h                                                conflict!(冲突！)
'V'   all    linux/videodev2.h                                         conflict!(冲突！)
'V'   C0     linux/ivtvfb.h                                            conflict!(冲突！)
'V'   C0     linux/ivtv.h                                              conflict!(冲突！)
'V'   C0     media/si4713.h                                            conflict!(冲突！)
'W'   00-1F  linux/watchdog.h                                          conflict!(冲突！)
'W'   00-1F  linux/wanrouter.h                                         conflict! (pre 3.9)(冲突！3.9 之前)
'W'   00-3F  sound/asound.h                                            conflict!(冲突！)
'W'   40-5F  drivers/pci/switch/switchtec.c
'W'   60-61  linux/watch_queue.h
'X'   all    fs/xfs/xfs_fs.h,                                          conflict!(冲突！)
             fs/xfs/linux-2.6/xfs_ioctl32.h,
             include/linux/falloc.h,
             linux/fs.h,
'X'   all    fs/ocfs2/ocfs_fs.h                                        conflict!(冲突！)
'Z'   14-15  drivers/message/fusion/mptctl.h
'['   00-3F  linux/usb/tmc.h                                           USB Test and Measurement Devices
                                                                       <mailto:gregkh@linuxfoundation.org> (USB 测试与测量设备)
'a'   all    linux/atm*.h, linux/sonet.h                               ATM on linux
                                                                       <http://lrcwww.epfl.ch/> (Linux 上的 ATM)
'b'   00-FF                                                            conflict! bit3 vme host bridge
                                                                       <mailto:natalia@nikhefk.nikhef.nl> (冲突！bit3 VME 宿主桥)
'b'   00-0F  linux/dma-buf.h                                           conflict!(冲突！)
'c'   00-7F  linux/comstats.h                                          conflict!(冲突！)
'c'   00-7F  linux/coda.h                                              conflict!(冲突！)
'c'   00-1F  linux/chio.h                                              conflict!(冲突！)
'c'   80-9F  arch/s390/include/asm/chsc.h                              conflict!(冲突！)
'c'   A0-AF  arch/x86/include/asm/msr.h conflict!(冲突！)
'd'   00-FF  linux/char/drm/drm.h                                      conflict!(冲突！)
'd'   02-40  pcmcia/ds.h                                               conflict!(冲突！)
'd'   F0-FF  linux/digi1.h
'e'   all    linux/digi1.h                                             conflict!(冲突！)
'f'   00-1F  linux/ext2_fs.h                                           conflict!(冲突！)
'f'   00-1F  linux/ext3_fs.h                                           conflict!(冲突！)
'f'   00-0F  fs/jfs/jfs_dinode.h                                       conflict!(冲突！)
'f'   00-0F  fs/ext4/ext4.h                                            conflict!(冲突！)
'f'   00-0F  linux/fs.h                                                conflict!(冲突！)
'f'   00-0F  fs/ocfs2/ocfs2_fs.h                                       conflict!(冲突！)
'f'   13-27  linux/fscrypt.h
'f'   81-8F  linux/fsverity.h
'g'   00-0F  linux/usb/gadgetfs.h
'g'   20-2F  linux/usb/g_printer.h
'h'   00-7F                                                            conflict! Charon filesystem
                                                                       <mailto:zapman@interlan.net> (冲突！Charon 文件系统)
'h'   00-1F  linux/hpet.h                                              conflict!(冲突！)
'h'   80-8F  fs/hfsplus/ioctl.c
'i'   00-3F  linux/i2o-dev.h                                           conflict!(冲突！)
'i'   0B-1F  linux/ipmi.h                                              conflict!(冲突！)
'i'   80-8F  linux/i8k.h
'i'   90-9F  `linux/iio/*.h`                                           IIO
'j'   00-3F  linux/joystick.h
'k'   00-0F  linux/spi/spidev.h                                        conflict!(冲突！)
'k'   00-05  video/kyro.h                                              conflict!(冲突！)
'k'   10-17  linux/hsi/hsi_char.h                                      HSI character device(HSI 字符设备)
'l'   00-3F  linux/tcfs_fs.h                                           transparent cryptographic file system
                                                                       <http://web.archive.org/web/%2A/http://mikonos.dia.unisa.it/tcfs> (透明加密文件系统)
'l'   40-7F  linux/udf_fs_i.h                                          in development:
                                                                       <https://github.com/pali/udftools> (开发中：)
'm'   00-09  linux/mmtimer.h                                           conflict!(冲突！)
'm'   all    linux/mtio.h                                              conflict!(冲突！)
'm'   all    linux/soundcard.h                                         conflict!(冲突！)
'm'   all    linux/synclink.h                                          conflict!(冲突！)
'm'   00-19  drivers/message/fusion/mptctl.h                           conflict!(冲突！)
'm'   00     drivers/scsi/megaraid/megaraid_ioctl.h                    conflict!(冲突！)
'n'   00-7F  linux/ncp_fs.h and fs/ncpfs/ioctl.c
'n'   80-8F  uapi/linux/nilfs2_api.h                                   NILFS2
'n'   E0-FF  linux/matroxfb.h                                          matroxfb
'o'   00-1F  fs/ocfs2/ocfs2_fs.h                                       OCFS2
'o'   00-03  mtd/ubi-user.h                                            conflict! (OCFS2 and UBI overlaps)(冲突！OCFS2 与 UBI 重叠)
'o'   40-41  mtd/ubi-user.h                                            UBI
'o'   01-A1  `linux/dvb/*.h`                                           DVB
'p'   00-0F  linux/phantom.h                                           conflict! (OpenHaptics needs this)(冲突！OpenHaptics 需要此区间)
'p'   00-1F  linux/rtc.h                                               conflict!(冲突！)
'p'   40-7F  linux/nvram.h
'p'   80-9F  linux/ppdev.h                                             user-space parport
                                                                       <mailto:tim@cyberelk.net> (用户态并口)
'p'   A1-A5  linux/pps.h                                               LinuxPPS
'p'   B1-B3  linux/pps_gen.h                                           LinuxPPS
                                                                       <mailto:giometti@linux.it>
'q'   00-1F  linux/serio.h
'q'   80-FF  linux/telephony.h                                         Internet PhoneJACK, Internet LineJACK
             linux/ixjuser.h                                           <http://web.archive.org/web/%2A/http://www.quicknet.net> (互联网电话卡)
'r'   00-1F  linux/msdos_fs.h and fs/fat/dir.c
's'   all    linux/cdk.h
't'   00-7F  linux/ppp-ioctl.h
't'   80-8F  linux/isdn_ppp.h
't'   90-91  linux/toshiba.h                                           toshiba and toshiba_acpi SMM(toshiba 与 toshiba_acpi 的 SMM 接口)
'u'   00-1F  linux/smb_fs.h                                            gone(已移除)
'u'   00-2F  linux/ublk_cmd.h                                          conflict!(冲突！)
'u'   20-3F  linux/uvcvideo.h                                          USB video class host driver(USB 视频类主机驱动)
'u'   40-4f  linux/udmabuf.h                                           userspace dma-buf misc device(用户空间 dma-buf 杂项设备)
'v'   00-1F  linux/ext2_fs.h                                           conflict!(冲突！)
'v'   00-1F  linux/fs.h                                                conflict!(冲突！)
'v'   00-0F  linux/sonypi.h                                            conflict!(冲突！)
'v'   00-0F  media/v4l2-subdev.h                                       conflict!(冲突！)
'v'   20-27  arch/powerpc/include/uapi/asm/vas-api.h                   VAS API(虚拟地址空间接口)
'v'   C0-FF  linux/meye.h                                              conflict!(冲突！)
'w'   all                                                              CERN SCI driver(CERN SCI 驱动)
'y'   00-1F                                                            packet based user level communications
                                                                       <mailto:zapman@interlan.net>
'z'   00-3F                                                            CAN bus card conflict!
                                                                       <mailto:hdstich@connectu.ulm.circular.de> (CAN 总线卡冲突！)
'z'   40-7F                                                            CAN bus card conflict!
                                                                       <mailto:oe@port.de> (CAN 总线卡冲突！)
'z'   10-4F  drivers/s390/crypto/zcrypt_api.h                          conflict!(冲突！)
'|'   00-7F  linux/media.h
'|'   80-9F  samples/                                                  Any sample and example drivers(任意样例与示例驱动)
0x80  00-1F  linux/fb.h
0x81  00-1F  linux/vduse.h
0x89  00-06  arch/x86/include/asm/sockios.h
0x89  0B-DF  linux/sockios.h
0x89  E0-EF  linux/sockios.h                                           SIOCPROTOPRIVATE range(SIOCPROTOPRIVATE 区段)
0x89  F0-FF  linux/sockios.h                                           SIOCDEVPRIVATE range(SIOCDEVPRIVATE 区段)
0x8A  00-1F  linux/eventpoll.h
0x8B  all    linux/wireless.h
0x8C  00-3F                                                            WiNRADiO driver
                                                                       <http://www.winradio.com.au/>
0x90  00     drivers/cdrom/sbpcd.h
0x92  00-0F  drivers/usb/mon/mon_bin.c
0x93  60-7F  linux/auto_fs.h
0x94  all    fs/btrfs/ioctl.h                                          Btrfs filesystem(Btrfs 文件系统)
             and linux/fs.h                                            some lifted to vfs/generic(其中一些已提升到 vfs/generic 通用层)
0x97  00-7F  fs/ceph/ioctl.h                                           Ceph file system(Ceph 文件系统)
0x99  00-0F                                                            537-Addinboard driver
                                                                       <mailto:buk@buks.ipn.de> (537 附加板驱动)
0x9A  00-0F  include/uapi/fwctl/fwctl.h
0xA0  all    linux/sdp/sdp.h                                           Industrial Device Project
                                                                       <mailto:kenji@bitgate.com> (工业设备项目)
0xA1  0      linux/vtpm_proxy.h                                        TPM Emulator Proxy Driver(TPM 模拟器代理驱动)
0xA2  all    uapi/linux/acrn.h                                         ACRN hypervisor(ACRN 虚拟机监控器)
0xA3  80-8F                                                            Port ACL  in development:
                                                                       <mailto:tlewis@mindspring.com> (Port ACL，开发中：)
0xA3  90-9F  linux/dtlk.h
0xA4  00-1F  uapi/linux/tee.h                                          Generic TEE subsystem(通用 TEE 子系统)
0xA4  00-1F  uapi/asm/sgx.h                                            <mailto:linux-sgx@vger.kernel.org>
0xA5  01-05  linux/surface_aggregator/cdev.h                           Microsoft Surface Platform System Aggregator
                                                                       <mailto:luzmaximilian@gmail.com> (微软 Surface 平台系统聚合器)
0xA5  20-2F  linux/surface_aggregator/dtx.h                            Microsoft Surface DTX driver
                                                                       <mailto:luzmaximilian@gmail.com> (微软 Surface DTX 驱动)
0xAA  00-3F  linux/uapi/linux/userfaultfd.h
0xAB  00-1F  linux/nbd.h
0xAC  00-1F  linux/raw.h
0xAD  00                                                               Netfilter device in development:
                                                                       <mailto:rusty@rustcorp.com.au> (开发中的 Netfilter 设备：)
0xAE  00-1F  linux/kvm.h                                               Kernel-based Virtual Machine
                                                                       <mailto:kvm@vger.kernel.org> (基于内核的虚拟机)
0xAE  40-FF  linux/kvm.h                                               Kernel-based Virtual Machine
                                                                       <mailto:kvm@vger.kernel.org> (基于内核的虚拟机)
0xAE  20-3F  linux/nitro_enclaves.h                                    Nitro Enclaves(安全飞地)
0xAF  00-1F  linux/fsl_hypervisor.h                                    Freescale hypervisor(Freescale 虚拟机监控器)
0xB0  all                                                              RATIO devices in development:
                                                                       <mailto:vgo@ratio.de> (开发中的 RATIO 设备：)
0xB1  00-1F                                                            PPPoX
                                                                       <mailto:mostrows@styx.uwaterloo.ca>
0xB2  00     arch/powerpc/include/uapi/asm/papr-vpd.h                  powerpc/pseries VPD API
                                                                       <mailto:linuxppc-dev@lists.ozlabs.org> (powerpc/pseries VPD 接口)
0xB2  01-02  arch/powerpc/include/uapi/asm/papr-sysparm.h              powerpc/pseries system parameter API
                                                                       <mailto:linuxppc-dev@lists.ozlabs.org> (powerpc/pseries 系统参数接口)
0xB2  03-05  arch/powerpc/include/uapi/asm/papr-indices.h              powerpc/pseries indices API
                                                                       <mailto:linuxppc-dev@lists.ozlabs.org> (powerpc/pseries 索引接口)
0xB2  06-07  arch/powerpc/include/uapi/asm/papr-platform-dump.h        powerpc/pseries Platform Dump API
                                                                       <mailto:linuxppc-dev@lists.ozlabs.org> (powerpc/pseries 平台转储接口)
0xB2  08     arch/powerpc/include/uapi/asm/papr-physical-attestation.h powerpc/pseries Physical Attestation API
                                                                       <mailto:linuxppc-dev@lists.ozlabs.org> (powerpc/pseries 物理证明接口)
0xB2  09     arch/powerpc/include/uapi/asm/papr-hvpipe.h               powerpc/pseries HVPIPE API
                                                                       <mailto:linuxppc-dev@lists.ozlabs.org> (powerpc/pseries HVPIPE 接口)
0xB3  00     linux/mmc/ioctl.h
0xB4  00-0F  linux/gpio.h                                              <mailto:linux-gpio@vger.kernel.org>
0xB5  00-0F  uapi/linux/rpmsg.h                                        <mailto:linux-remoteproc@vger.kernel.org>
0xB6  all    linux/fpga-dfl.h
0xB7  all    uapi/linux/remoteproc_cdev.h                              <mailto:linux-remoteproc@vger.kernel.org>
0xB7  all    uapi/linux/nsfs.h                                         <mailto:Andrei Vagin <avagin@openvz.org>>
0xB8  01-02  uapi/misc/mrvl_cn10k_dpi.h                                Marvell CN10K DPI driver(Marvell CN10K DPI 驱动)
0xB8  all    uapi/linux/mshv.h                                         Microsoft Hyper-V /dev/mshv driver
                                                                       <mailto:linux-hyperv@vger.kernel.org> (微软 Hyper-V /dev/mshv 驱动)
0xBA  00-0F  uapi/linux/liveupdate.h                                   Pasha Tatashin
                                                                       <mailto:pasha.tatashin@soleen.com>
0xC0  00-0F  linux/usb/iowarrior.h
0xCA  00-0F  uapi/misc/cxl.h                                           Dead since 6.15(自 6.15 起已废弃)
0xCA  10-2F  uapi/misc/ocxl.h
0xCA  80-BF  uapi/scsi/cxlflash_ioctl.h                                Dead since 6.15(自 6.15 起已废弃)
0xCB  00-1F                                                            CBM serial IEC bus in development:
                                                                       <mailto:michael.klein@puffin.lb.shuttle.de> (开发中的 CBM 串行 IEC 总线：)
0xCC  00-0F  drivers/misc/ibmvmc.h                                     pseries VMC driver(pseries VMC 驱动)
0xCD  01     linux/reiserfs_fs.h                                       Dead since 6.13(自 6.13 起已废弃)
0xCE  01-02  uapi/linux/cxl_mem.h                                      Compute Express Link Memory Devices(Compute Express Link 内存设备)
0xCF  02     fs/smb/client/cifs_ioctl.h
0xDD  00-3F                                                            ZFCP device driver see drivers/s390/scsi/
                                                                       <mailto:aherrman@de.ibm.com> (ZFCP 设备驱动，参见 drivers/s390/scsi/)
0xE5  00-3F  linux/fuse.h
0xEC  00-01  drivers/platform/chrome/cros_ec_dev.h                     ChromeOS EC driver(ChromeOS EC 驱动)
0xEE  00-09  uapi/linux/pfrut.h                                        Platform Firmware Runtime Update and Telemetry(平台固件运行时更新与遥测)
0xF3  00-3F  drivers/usb/misc/sisusbvga/sisusb.h                       sisfb (in development)(开发中)
                                                                       <mailto:thomas@winischhofer.net>
0xF6  all                                                              LTTng Linux Trace Toolkit Next Generation
                                                                       <mailto:mathieu.desnoyers@efficios.com> (LTTng，Linux 跟踪工具包下一代)
0xF8  all    arch/x86/include/uapi/asm/amd_hsmp.h                      AMD HSMP EPYC system management interface driver
                                                                       <mailto:nchatrad@amd.com> (AMD HSMP EPYC 系统管理接口驱动)
0xF9  00-0F  uapi/misc/amd-apml.h                                      AMD side band system management interface driver
                                                                       <mailto:naveenkrishna.chatradhi@amd.com> (AMD 边带系统管理接口驱动)
0xFD  all    linux/dm-ioctl.h
0xFE  all    linux/isst_if.h
====  =====  ========================================================= ================================================================
