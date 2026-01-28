# 内核启动
## 内核启动重要文件
|文件名|路径|包含内容|备注|
|-|-|-|-|
|head.S|arch/arm64/kernel/head.S.copy|-|-|
|-|-|-|-|
|vmlinux.lds.S|arch/arm64/kernel/vmlinux.lds.S.copy|-|-|
|-|-|-|-|
|entry.S|arch/arm64/kernel/entry.S.md|-|-|
|-|-|-|-|

----

## 内核启动日志
```shell
wei@Berries:~/OPEN_SOURCE/Berries-Kernel/000.SOURCE_CODE/000.LINUX-5.9/000.LINUX-5.9$ sudo qemu-system-aarch64 -M virt -cpu cortex-a57 -smp 4 -m 1024M -kernel arch/arm64/boot/Image -append "rdinit=/linuxrc console=ttyAMA0 loglevel=8"  -nographic
[sudo] password for wei: 
[    0.000000] Booting Linux on physical CPU 0x0000000000 [0x411fd070]
[    0.000000] Linux version 5.9.0 (wei@Berries) (aarch64-none-linux-gnu-gcc (Arm GNU Toolchain 14.3.Rel1 (Build arm-14.174)) 14.3.1 20250623, GNU ld (Arm GNU Toolchain 14.3.Rel1 (Build arm-14.174)) 2.44.0.20250616) #9 SMP PREEMPT Fri Nov 21 22:52:42 CST 2025
[    0.000000] Machine model: linux,dummy-virt
[    0.000000] efi: UEFI not found.
[    0.000000] cma: Reserved 32 MiB at 0x000000007e000000
[    0.000000] swapper_pg_dir is 0xffffa0f0c073e000 , pa: ox4173e000 
[    0.000000] kimage_voffset is 0xffffa0f07f000000 
[    0.000000] _text is 0xffffa0f0bf200000 
[    0.000000] kimage_vaddr is 0xffffa0f0bf200000 
[    0.000000] KIMAGE_VADDR is 0xffff800010000000 
[    0.000000] FIXADDR_TOP is 0xfffffdfffea00000 
[    0.000000] VMEMMAP_START is 0xfffffdffffe00000 
[    0.000000] VMEMMAP_END is 0xffffffffffe00000 
[    0.000000] PCI_IO_START is 0xfffffdfffec00000 
[    0.000000] PCI_IO_END is 0xfffffdffffc00000 
[    0.000000] KERNEL_START is 0xffffa0f0bf200000 
[    0.000000] KERNEL_END is 0xffffa0f0c10b0000 
[    0.000000] PAGE_OFFSET is 0xffff000000000000 
[    0.000000] MODULES_VADDR is 0xffff800008000000 
[    0.000000] MODULES_END is 0xffff800010000000 
[    0.000000] NUMA: No NUMA configuration found
[    0.000000] NUMA: Faking a node at [mem 0x0000000040000000-0x000000007fffffff]
[    0.000000] NUMA: NODE_DATA [mem 0x7ddf4100-0x7ddf5fff]
[    0.000000] Zone ranges:
[    0.000000]   DMA      [mem 0x0000000040000000-0x000000007fffffff]
[    0.000000]   DMA32    empty
[    0.000000]   Normal   empty
[    0.000000] Movable zone start for each node
[    0.000000] Early memory node ranges
[    0.000000]   node   0: [mem 0x0000000040000000-0x000000007fffffff]
[    0.000000] Initmem setup node 0 [mem 0x0000000040000000-0x000000007fffffff]
[    0.000000] On node 0 totalpages: 262144
[    0.000000]   DMA zone: 4096 pages used for memmap
[    0.000000]   DMA zone: 0 pages reserved
[    0.000000]   DMA zone: 262144 pages, LIFO batch:63
[    0.000000] psci: probing for conduit method from DT.
[    0.000000] psci: PSCIv0.2 detected in firmware.
[    0.000000] psci: Using standard PSCI v0.2 function IDs
[    0.000000] psci: Trusted OS migration not required
[    0.000000] percpu: Embedded 23 pages/cpu s54104 r8192 d31912 u94208
[    0.000000] pcpu-alloc: s54104 r8192 d31912 u94208 alloc=23*4096
[    0.000000] pcpu-alloc: [0] 0 [0] 1 [0] 2 [0] 3 
[    0.000000] Detected PIPT I-cache on CPU0
[    0.000000] CPU features: detected: ARM erratum 832075
[    0.000000] CPU features: detected: ARM erratum 834220
[    0.000000] CPU features: detected: EL2 vector hardening
[    0.000000] CPU features: kernel page table isolation forced ON by KASLR
[    0.000000] CPU features: detected: Kernel page table isolation (KPTI)
[    0.000000] ARM_SMCCC_ARCH_WORKAROUND_1 missing from firmware
[    0.000000] CPU features: detected: ARM errata 1165522, 1319367, or 1530923
[    0.000000] Built 1 zonelists, mobility grouping on.  Total pages: 258048
[    0.000000] Policy zone: DMA
[    0.000000] Kernel command line: rdinit=/linuxrc console=ttyAMA0 loglevel=8
[    0.000000] Dentry cache hash table entries: 131072 (order: 8, 1048576 bytes, linear)
[    0.000000] Inode-cache hash table entries: 65536 (order: 7, 524288 bytes, linear)
[    0.000000] mem auto-init: stack:off, heap alloc:off, heap free:off
[    0.000000] Memory: 962972K/1048576K available (14140K kernel code, 2280K rwdata, 7592K rodata, 6848K init, 484K bss, 52836K reserved, 32768K cma-reserved)
[    0.000000] SLUB: HWalign=64, Order=0-3, MinObjects=0, CPUs=4, Nodes=1
[    0.000000] rcu: Preemptible hierarchical RCU implementation.
[    0.000000] rcu:     RCU event tracing is enabled.
[    0.000000] rcu:     RCU restricting CPUs from NR_CPUS=256 to nr_cpu_ids=4.
[    0.000000]  Trampoline variant of Tasks RCU enabled.
[    0.000000] rcu: RCU calculated value of scheduler-enlistment delay is 25 jiffies.
[    0.000000] rcu: Adjusting geometry for rcu_fanout_leaf=16, nr_cpu_ids=4
[    0.000000] NR_IRQS: 64, nr_irqs: 64, preallocated irqs: 0
[    0.000000] GICv2m: range[mem 0x08020000-0x08020fff], SPI[80:143]
[    0.000000] random: get_random_bytes called from start_kernel+0x32c/0x4fc with crng_init=0
[    0.000000] arch_timer: cp15 timer(s) running at 62.50MHz (virt).
[    0.000000] clocksource: arch_sys_counter: mask: 0xffffffffffffff max_cycles: 0x1cd42e208c, max_idle_ns: 881590405314 ns
[    0.000082] sched_clock: 56 bits at 62MHz, resolution 16ns, wraps every 4398046511096ns
[    0.002659] Console: colour dummy device 80x25
[    0.005736] Calibrating delay loop (skipped), value calculated using timer frequency.. 125.00 BogoMIPS (lpj=250000)
[    0.005820] pid_max: default: 32768 minimum: 301
[    0.006498] LSM: Security Framework initializing
[    0.007196] Mount-cache hash table entries: 2048 (order: 2, 16384 bytes, linear)
[    0.007221] Mountpoint-cache hash table entries: 2048 (order: 2, 16384 bytes, linear)
[    0.026860] /cpus/cpu-map: empty cluster
[    0.031787] rcu: Hierarchical SRCU implementation.
[    0.034963] EFI services will not be available.
[    0.036042] smp: Bringing up secondary CPUs ...
[    0.038705] Detected PIPT I-cache on CPU1
[    0.039163] CPU1: Booted secondary processor 0x0000000001 [0x411fd070]
[    0.041869] Detected PIPT I-cache on CPU2
[    0.042002] CPU2: Booted secondary processor 0x0000000002 [0x411fd070]
[    0.043722] Detected PIPT I-cache on CPU3
[    0.043840] CPU3: Booted secondary processor 0x0000000003 [0x411fd070]
[    0.044086] smp: Brought up 1 node, 4 CPUs
[    0.044131] SMP: Total of 4 processors activated.
[    0.044170] CPU features: detected: 32-bit EL0 Support
[    0.044210] CPU features: detected: CRC32 instructions
[    0.044242] CPU features: detected: 32-bit EL1 Support
[    0.087887] CPU: All CPU(s) started at EL1
[    0.088113] alternatives: patching kernel code
[    0.100884] devtmpfs: initialized
[    0.106884] KASLR enabled
[    0.109390] clocksource: jiffies: mask: 0xffffffff max_cycles: 0xffffffff, max_idle_ns: 7645041785100000 ns
[    0.109482] futex hash table entries: 1024 (order: 4, 65536 bytes, linear)
[    0.111290] pinctrl core: initialized pinctrl subsystem
[    0.118614] DMI not present or invalid.
[    0.124072] NET: Registered protocol family 16
[    0.131365] DMA: preallocated 128 KiB GFP_KERNEL pool for atomic allocations
[    0.131465] DMA: preallocated 128 KiB GFP_KERNEL|GFP_DMA pool for atomic allocations
[    0.131663] DMA: preallocated 128 KiB GFP_KERNEL|GFP_DMA32 pool for atomic allocations
[    0.131850] audit: initializing netlink subsys (disabled)
[    0.133390] audit: type=2000 audit(0.128:1): state=initialized audit_enabled=0 res=1
[    0.136289] thermal_sys: Registered thermal governor 'step_wise'
[    0.136310] thermal_sys: Registered thermal governor 'power_allocator'
[    0.136877] cpuidle: using governor menu
[    0.137516] hw-breakpoint: found 6 breakpoint and 4 watchpoint registers.
[    0.138134] ASID allocator initialised with 32768 entries
[    0.140854] Serial: AMBA PL011 UART driver
[    0.169044] 9000000.pl011: ttyAMA0 at MMIO 0x9000000 (irq = 39, base_baud = 0) is a PL011 rev1
[    0.186525] printk: console [ttyAMA0] enabled
[    0.218340] HugeTLB registered 1.00 GiB page size, pre-allocated 0 pages
[    0.218546] HugeTLB registered 32.0 MiB page size, pre-allocated 0 pages
[    0.218735] HugeTLB registered 2.00 MiB page size, pre-allocated 0 pages
[    0.218915] HugeTLB registered 64.0 KiB page size, pre-allocated 0 pages
[    0.228847] cryptd: max_cpu_qlen set to 1000
[    0.235827] ACPI: Interpreter disabled.
[    0.239278] iommu: Default domain type: Translated 
[    0.240412] vgaarb: loaded
[    0.241513] SCSI subsystem initialized
[    0.242496] libata version 3.00 loaded.
[    0.243497] usbcore: registered new interface driver usbfs
[    0.243787] usbcore: registered new interface driver hub
[    0.244121] usbcore: registered new device driver usb
[    0.245371] pps_core: LinuxPPS API ver. 1 registered
[    0.245517] pps_core: Software ver. 5.3.6 - Copyright 2005-2007 Rodolfo Giometti <giometti@linux.it>
[    0.245769] PTP clock support registered
[    0.246208] EDAC MC: Ver: 3.0.0
[    0.249607] FPGA manager framework
[    0.250432] Advanced Linux Sound Architecture Driver Initialized.
[    0.258212] clocksource: Switched to clocksource arch_sys_counter
[    0.259146] VFS: Disk quotas dquot_6.6.0
[    0.259350] VFS: Dquot-cache hash table entries: 512 (order 0, 4096 bytes)
[    0.261485] pnp: PnP ACPI: disabled
[    0.277450] NET: Registered protocol family 2
[    0.280863] tcp_listen_portaddr_hash hash table entries: 512 (order: 1, 8192 bytes, linear)
[    0.281146] TCP established hash table entries: 8192 (order: 4, 65536 bytes, linear)
[    0.281475] TCP bind hash table entries: 8192 (order: 5, 131072 bytes, linear)
[    0.281796] TCP: Hash tables configured (established 8192 bind 8192)
[    0.282826] UDP hash table entries: 512 (order: 2, 16384 bytes, linear)
[    0.283143] UDP-Lite hash table entries: 512 (order: 2, 16384 bytes, linear)
[    0.284230] NET: Registered protocol family 1
[    0.286684] RPC: Registered named UNIX socket transport module.
[    0.286856] RPC: Registered udp transport module.
[    0.286961] RPC: Registered tcp transport module.
[    0.287070] RPC: Registered tcp NFSv4.1 backchannel transport module.
[    0.287277] PCI: CLS 0 bytes, default 64
[    0.343506] hw perfevents: enabled with armv8_pmuv3 PMU driver, 5 counters available
[    0.344030] kvm [1]: HYP mode not available
[    0.354654] Initialise system trusted keyrings
[    0.356490] workingset: timestamp_bits=44 max_order=18 bucket_order=0
[    0.365185] squashfs: version 4.0 (2009/01/31) Phillip Lougher
[    0.367176] NFS: Registering the id_resolver key type
[    0.367539] Key type id_resolver registered
[    0.367655] Key type id_legacy registered
[    0.367981] nfs4filelayout_init: NFSv4 File Layout Driver Registering...
[    0.368755] 9p: Installing v9fs 9p2000 file system support
[    0.387124] Key type asymmetric registered
[    0.387311] Asymmetric key parser 'x509' registered
[    0.387593] Block layer SCSI generic (bsg) driver version 0.4 loaded (major 245)
[    0.387903] io scheduler mq-deadline registered
[    0.388041] io scheduler kyber registered
[    0.399375] pl061_gpio 9030000.pl061: PL061 GPIO chip registered
[    0.401762] pci-host-generic 4010000000.pcie: host bridge /pcie@10000000 ranges:
[    0.402713] pci-host-generic 4010000000.pcie:       IO 0x003eff0000..0x003effffff -> 0x0000000000
[    0.403223] pci-host-generic 4010000000.pcie:      MEM 0x0010000000..0x003efeffff -> 0x0010000000
[    0.403471] pci-host-generic 4010000000.pcie:      MEM 0x8000000000..0xffffffffff -> 0x8000000000
[    0.404307] pci-host-generic 4010000000.pcie: ECAM at [mem 0x4010000000-0x401fffffff] for [bus 00-ff]
[    0.405302] pci-host-generic 4010000000.pcie: PCI host bridge to bus 0000:00
[    0.405581] pci_bus 0000:00: root bus resource [bus 00-ff]
[    0.405761] pci_bus 0000:00: root bus resource [io  0x0000-0xffff]
[    0.405903] pci_bus 0000:00: root bus resource [mem 0x10000000-0x3efeffff]
[    0.406083] pci_bus 0000:00: root bus resource [mem 0x8000000000-0xffffffffff]
[    0.407282] pci 0000:00:00.0: [1b36:0008] type 00 class 0x060000
[    0.409559] pci 0000:00:01.0: [1af4:1000] type 00 class 0x020000
[    0.409879] pci 0000:00:01.0: reg 0x10: [io  0x0000-0x001f]
[    0.410053] pci 0000:00:01.0: reg 0x14: [mem 0x00000000-0x00000fff]
[    0.410489] pci 0000:00:01.0: reg 0x20: [mem 0x00000000-0x00003fff 64bit pref]
[    0.410660] pci 0000:00:01.0: reg 0x30: [mem 0x00000000-0x0007ffff pref]
[    0.412381] pci 0000:00:01.0: BAR 6: assigned [mem 0x10000000-0x1007ffff pref]
[    0.412721] pci 0000:00:01.0: BAR 4: assigned [mem 0x8000000000-0x8000003fff 64bit pref]
[    0.412956] pci 0000:00:01.0: BAR 1: assigned [mem 0x10080000-0x10080fff]
[    0.413113] pci 0000:00:01.0: BAR 0: assigned [io  0x1000-0x101f]
[    0.417428] EINJ: ACPI disabled.
[    0.430747] virtio-pci 0000:00:01.0: enabling device (0000 -> 0003)
[    0.441167] Serial: 8250/16550 driver, 4 ports, IRQ sharing enabled
[    0.445156] SuperH (H)SCI(F) driver initialized
[    0.445968] msm_serial: driver initialized
[    0.448431] cacheinfo: Unable to detect cache hierarchy for CPU 0
[    0.464292] loop: module loaded
[    0.465714] megasas: 07.714.04.00-rc1
[    0.469462] physmap-flash 0.flash: physmap platform flash device: [mem 0x00000000-0x03ffffff]
[    0.470939] 0.flash: Found 2 x16 devices at 0x0 in 32-bit bank. Manufacturer ID 0x000000 Chip ID 0x000000
[    0.471597] Intel/Sharp Extended Query Table at 0x0031
[    0.472573] Using buffer write method
[    0.472914] erase region 0: offset=0x0,size=0x40000,blocks=256
[    0.473209] physmap-flash 0.flash: physmap platform flash device: [mem 0x04000000-0x07ffffff]
[    0.473942] 0.flash: Found 2 x16 devices at 0x0 in 32-bit bank. Manufacturer ID 0x000000 Chip ID 0x000000
[    0.474448] Intel/Sharp Extended Query Table at 0x0031
[    0.475258] Using buffer write method
[    0.475425] erase region 0: offset=0x0,size=0x40000,blocks=256
[    0.475632] Concatenating MTD devices:
[    0.475750] (0): "0.flash"
[    0.475827] (1): "0.flash"
[    0.475915] into device "0.flash"
[    0.524687] libphy: Fixed MDIO Bus: probed
[    0.525666] tun: Universal TUN/TAP device driver, 1.6
[    0.534969] thunder_xcv, ver 1.0
[    0.535272] thunder_bgx, ver 1.0
[    0.535421] nicpf, ver 1.0
[    0.537088] hclge is initializing
[    0.537393] hns3: Hisilicon Ethernet Network Driver for Hip08 Family - version
[    0.537605] hns3: Copyright (c) 2017 Huawei Corporation.
[    0.537908] e1000: Intel(R) PRO/1000 Network Driver
[    0.538029] e1000: Copyright (c) 1999-2006 Intel Corporation.
[    0.538346] e1000e: Intel(R) PRO/1000 Network Driver
[    0.538459] e1000e: Copyright(c) 1999 - 2015 Intel Corporation.
[    0.538649] igb: Intel(R) Gigabit Ethernet Network Driver
[    0.538766] igb: Copyright (c) 2007-2014 Intel Corporation.
[    0.538935] igbvf: Intel(R) Gigabit Virtual Function Network Driver
[    0.539070] igbvf: Copyright (c) 2009 - 2012 Intel Corporation.
[    0.539573] sky2: driver version 1.30
[    0.541325] VFIO - User Level meta-driver version: 0.3
[    0.544179] ehci_hcd: USB 2.0 'Enhanced' Host Controller (EHCI) Driver
[    0.544376] ehci-pci: EHCI PCI platform driver
[    0.544615] ehci-platform: EHCI generic platform driver
[    0.544862] ehci-orion: EHCI orion driver
[    0.545054] ehci-exynos: EHCI Exynos driver
[    0.545242] ohci_hcd: USB 1.1 'Open' Host Controller (OHCI) Driver
[    0.545456] ohci-pci: OHCI PCI platform driver
[    0.545714] ohci-platform: OHCI generic platform driver
[    0.545963] ohci-exynos: OHCI Exynos driver
[    0.547207] usbcore: registered new interface driver usb-storage
[    0.552876] rtc-pl031 9010000.pl031: registered as rtc0
[    0.553622] rtc-pl031 9010000.pl031: setting system clock to 2025-11-21T15:36:10 UTC (1763739370)
[    0.555082] i2c /dev entries driver
[    0.562990] sdhci: Secure Digital Host Controller Interface driver
[    0.563150] sdhci: Copyright(c) Pierre Ossman
[    0.563786] Synopsys Designware Multimedia Card Interface Driver
[    0.564893] sdhci-pltfm: SDHCI platform and OF driver helper
[    0.567419] ledtrig-cpu: registered to indicate activity on CPUs
[    0.569544] usbcore: registered new interface driver usbhid
[    0.569695] usbhid: USB HID core driver
[    0.577020] NET: Registered protocol family 17
[    0.578409] 9pnet: Installing 9P2000 support
[    0.578753] Key type dns_resolver registered
[    0.579527] registered taskstats version 1
[    0.579721] Loading compiled-in X.509 certificates
[    0.586988] input: gpio-keys as /devices/platform/gpio-keys/input/input0
[    0.590779] ALSA device list:
[    0.590908]   No soundcards found.
[    0.593098] uart-pl011 9000000.pl011: no DMA platform data
[    0.628055] Freeing unused kernel memory: 6848K
[    0.628850] Run /linuxrc as init process
[    0.628962]   with arguments:
[    0.629035]     /linuxrc
[    0.629099]   with environment:
[    0.629174]     HOME=/
[    0.629227]     TERM=linux
/etc/init.d/rcS: line 8: can't create /proc/sys/kernel/hotplug: nonexistent directory

Please press Enter to activate this console. 
```