# DTS 设备树：从源码文本到驱动匹配

> **定位**：DTS 全链路——**语法（dts/dtsi）→ 编译（dtc）→ 二进制（DTB/FDT）→ bootloader 交接（x0）→ 内核加载与解扁平化（device_node 树）→ 驱动匹配（compatible）**。主基准 [003.LINUX-7.1.3](../../000.SOURCE_CODE/003.LINUX-7.1.3/000.LINUX-7.1.3)，`file:line` 实测。启动时间线上的位置（加载→解析→应用五阶段）已有一篇深潜：[029.内核启动/002.Linux内核启动与设备树](../029.内核启动/002.Linux内核启动与设备树.md)（5.9 版），本篇不重复其时序，专注"设备树本身"。

## 0. 一句话总结

**设备树 = 把"板子上有什么硬件、挂在哪、怎么配"从内核代码里剥离成一份数据**：源文件是 DTS 文本，`dtc` 编译成扁平二进制 DTB，bootloader 把 DTB 物理地址放进 `x0` 交给内核，内核 `unflatten_device_tree()` 把它展开成 `struct device_node` 树，驱动用 `compatible` 字符串与节点对上号。

```
 board.dts ──(dtc 编译)──► board.dtb ──(bootloader: x0=dtb 物理地址)──► 内核
    ▲                        │                                          │
    │ include                │ FDT 扁平 blob                            │ fixmap 映射 FIX_FDT
 soc.dtsi ──► vendor.dtsi    │                                          ▼
 (芯片级)     (板级)         └────────────────────────────► unflatten_device_tree()
                                                              │
                                                              ▼
                                                    struct device_node 树
                                                              │
                                                              ▼
                                              驱动 of_match_table("compatible")
```

## 1. 为什么要有设备树

没有设备树的年代，每块板一个 `arch/arm/mach-xxx/board-*.c`（board file），把 GPIO/IRQ/内存等**写死在内核里**——内核镜像和板子一一绑定，不可移植。设备树（源自 **Open Firmware** 的 IEEE 1275，故内核接口全是 `of_` 前缀）把这份描述变成**启动时传入的数据**：

| | board file（旧） | 设备树（新） |
| - | - | - |
| 硬件描述位置 | 内核源码 C 文件 | 独立的 DTS 文本，编译为 DTB |
| 换板子 | 改内核、重编译 | 换 DTB（或同一内核+不同 dtb） |
| 描述形式 | `platform_device_register()` 逐个注册 | 节点树，`of_platform_populate()` 批量产 device |
| 单一内核镜像多板 | 不可能 | 可以（`compatible` 兜底匹配） |

内核侧的运行模型官方文档：[Documentation/devicetree/usage-model.rst](../../000.SOURCE_CODE/003.LINUX-7.1.3/000.LINUX-7.1.3/Documentation/devicetree/usage-model.rst)。

## 2. DTS 语法骨架（真实文件为例）

以内核自带的最小板级文件之一 [arch/arm64/boot/dts/arm/vexpress-v2f-1xv7-ca53x2.dts](../../000.SOURCE_CODE/003.LINUX-7.1.3/000.LINUX-7.1.3/arch/arm64/boot/dts/arm/vexpress-v2f-1xv7-ca53x2.dts) 为例（152 行，可通读）：

```dts
// arch/arm64/boot/dts/arm/vexpress-v2f-1xv7-ca53x2.dts:13（节选；chosen/aliases 等多行节点作紧凑排版）
/dts-v1/;                                    // 版本声明，dtc 识别

#include <dt-bindings/interrupt-controller/arm-gic.h>   // C 预处理!dts 也走 cpp
#include "arm/arm/vexpress-v2m-rs1.dtsi"     // 板载外设(SoC 级 .dtsi)

/ {                                          // 根节点
	model = "V2F-1XV7 Cortex-A53x2 SMM";
	compatible = "arm,vexpress,v2f-1xv7,ca53x2", "arm,vexpress,v2f-1xv7", "arm,vexpress";
	interrupt-parent = <&gic>;           // phandle 引用:全树默认中断控制器
	#address-cells = <2>;                 // 子节点 reg 的"地址"占几个 u32
	#size-cells = <2>;                    //          "长度"占几个 u32

	chosen { stdout-path = "serial0:38400n8"; };   // 启动期伪节点(console 等)

	aliases { serial0 = &v2m_serial0; ... };        // 别名,给 of_alias 用

	cpus {
		#address-cells = <2>;
		#size-cells = <0>;
		cpu@0 {                                   // 节点名: 名字@单元地址
			device_type = "cpu";
			compatible = "arm,cortex-a53";    // ← 驱动匹配的钥匙
			reg = <0 0>;                      // MPIDR,与 #address-cells 对应
			next-level-cache = <&L2_0>;       // 又一个 phandle
		};
		L2_0: l2-cache0 {                        // "L2_0:" 是 label
			compatible = "cache";
			cache-level = <2>;
		};
	};
};
```

语法五要素（都能在上例找到）：

| 要素 | 写法 | 说明 |
| - | - | - |
| 节点 | `name@unitaddr { ... };` | `@` 后须与 `reg` 首单元一致（无 reg 的用 `name` 纯名） |
| 属性 | `key = <u32...>;` / `"字符串"` / `[byte...]` / 三者混合列表 | 无值属性 `key;` 是开关位 |
| label + phandle | `L2_0: l2-cache0 {...}` ⇄ `<&L2_0>` | 编译器为有 label 的节点分配 `phandle` 整数，引用即指向它 |
| cells 约定 | `#address-cells/#size-cells` | 决定父/子如何解读 `reg`，**逐节点作用域** |
| include | `#include "*.dtsi"` + `dt-bindings/...` | 走 C 预处理器；`.dtsi` 是"被 include 的片段" |

`.dts` 与 `.dtsi` 的分层（include 链，自顶向下越来越板级专属）：

```
 board.dts            本板特有:内存大小/外设使能/别名        (vexpress-v2f-1xv7-ca53x2.dts)
   └► vendor.dtsi     厂商评估板公共                        (vexpress-v2m-rs1.dtsi)
        └► soc.dtsi   芯片全集:CPU/总线/控制器,大而全       (各 SoC 目录下)
             └► dt-bindings/*.h   常量定义(GIC_FLAG/时钟号...)
```

> 后写的同名属性**覆盖**先 include 的——板级 `.dts` 就是靠覆盖 `.dtsi` 的默认值来裁剪（`status = "okay"/"disabled"`）。

绑定（bindings）契约：每个可被驱动的节点，其必带属性在 [Documentation/devicetree/bindings/](../../000.SOURCE_CODE/003.LINUX-7.1.3/000.LINUX-7.1.3/Documentation/devicetree/bindings/) 里有 YAML 规范（DT Schema，`make dt_binding_check` 用它校验 DTS 写法）。

## 3. 编译：dtc 与 DTB

规则与工具都在内核树里：

```make
# scripts/Makefile.dtbs:131（7.1.3 实测；5.9 在 scripts/Makefile.lib）
quiet_cmd_dtc = DTC $(quiet_dtb_check_tag) $@
```

```shell
make dtbs                  # 按 arch/arm64/boot/dts/*/ 里的 dtb-y 清单批量编译
# 或单发(手工):
dtc -I dts -O dtb -o board.dtb board.dts     # -@ 选项附带符号,供 overlay 用
dtc -I dtb -O dts -o board.roundtrip.dts board.dtb    # 反编译(学习 DTB 的好工具)
```

dtc 本体是内核自带的编译器：[scripts/dtc/dtc.c](../../000.SOURCE_CODE/003.LINUX-7.1.3/000.LINUX-7.1.3/scripts/dtc/dtc.c)（词法/语法 [dtc-lexer.l](../../000.SOURCE_CODE/003.LINUX-7.1.3/000.LINUX-7.1.3/scripts/dtc/dtc-lexer.l) / [dtc-parser.y](../../000.SOURCE_CODE/003.LINUX-7.1.3/000.LINUX-7.1.3/scripts/dtc/dtc-parser.y)，语义检查 [checks.c](../../000.SOURCE_CODE/003.LINUX-7.1.3/000.LINUX-7.1.3/scripts/dtc/checks.c)）——树里的节点在 `data.c` 里被拉平成 blob。

## 4. DTB（=FDT）二进制格式

DTB 是 **FDT（Flattened Device Tree）**：一棵被"深度优先拉平"的节点树 + 公共字符串池：

```
 ┌────────────────────────── 0 ──────────────────────────────┐
 │ fdt_header (40 字节, 大端!)                                 │
 │   magic=0xd00dfeed, totalsize, off_dt_struct, off_dt_strings│
 │   version, last_comp_version, boot_cpuid_phys, ...          │
 ├────────────────────────────────────────────────────────────┤
 │ structure block: token 流(大端 u32)                         │
 │   FDT_BEGIN_NODE "cpus\0"                                   │
 │     FDT_BEGIN_NODE "cpu@0\0"                                │
 │       FDT_PROP len=17 nameoff→"compatible"                  │
 │            payload="arm,cortex-a53\0"                       │
 │       FDT_PROP ... reg=<0 0>                                │
 │     FDT_END_NODE                                           │
 │   FDT_END_NODE  ...  FDT_END(token=0x9)                    │
 ├────────────────────────────────────────────────────────────┤
 │ strings block: "compatible\0device_type\0reg\0..."          │
 │   (属性名去重池, PROP 只存 4 字节偏移)                        │
 └────────────────────────────────────────────────────────────┘
```

要点：

- **全大端**（历史沿袭 OF）；头 40 字节，内核两道校验 `magic == 0xd00dfeed`：先在 fixmap 窗口读出 blob 时验（`fdt_magic`，[fixmap.c:162](../../000.SOURCE_CODE/003.LINUX-7.1.3/000.LINUX-7.1.3/arch/arm64/mm/fixmap.c)），正式扫描前再走 `fdt_check_header`（[drivers/of/fdt.c](../../000.SOURCE_CODE/003.LINUX-7.1.3/000.LINUX-7.1.3/drivers/of/fdt.c)）；
- "扁平"的含义：**树形仅由 BEGIN/END token 的配对表达**，没有指针——正是这种无指针格式才允许 bootloader 把 blob 原样搬到任意 8 字节对齐地址再交给内核；
- 操作库是 [scripts/libfdt](../../000.SOURCE_CODE/003.LINUX-7.1.3/000.LINUX-7.1.3/scripts/dtc/libfdt/)（`fdt_magic()/fdt_totalsize()/fdt_getprop()` 等，上文 `fixmap_remap_fdt` 与早期扫描全用它）。

## 5. 交接与内核侧加载（7.1.3 锚点）

boot 协议对 DTB 的全部要求就两条（[Documentation/arch/arm64/booting.rst](../../000.SOURCE_CODE/003.LINUX-7.1.3/000.LINUX-7.1.3/Documentation/arch/arm64/booting.rst)）：

- 必须放在 **8 字节对齐**的内存里（:53），且不得被启动过程覆盖；
- `x0 = DTB 的物理地址`（:165），`head.S` 的 `preserve_boot_args` 把它存进 `__fdt_pointer`。

内核侧三步（与 [029/002](../029.内核启动/002.Linux内核启动与设备树.md) 的五阶段图衔接，此处只列 7.1.3 行号）：

```c
// arch/arm64/kernel/setup.c:292（setup_arch 内）
	setup_machine_fdt(__fdt_pointer);     // ① 映射 + 早期扫描
// arch/arm64/kernel/setup.c:342
	unflatten_device_tree();              // ② 展开成 device_node 树
```

① 里干的三件事（[setup.c:170](../../000.SOURCE_CODE/003.LINUX-7.1.3/000.LINUX-7.1.3/arch/arm64/kernel/setup.c)）：

```
setup_machine_fdt(dt_phys)                       setup.c:170
  ├─ fixmap_remap_fdt(dt_phys,...)               arch/arm64/mm/fixmap.c:137
  │    借 FIX_FDT 槽位(SZ_2M 窗口, boot.h:13)把 blob 映射进来,
  │    先读 header 的 magic/totalsize 再补映射 —— 细节见
  │    [pgd_set_fixmap 原理](../029.内核启动/000.pgd_set_fixmap原理.md) §1
  ├─ memblock_reserve(dt_phys, size)             setup.c:177  DTB 内存保护
  ├─ early_init_dt_scan(dt_virt, dt_phys)        setup.c:183 / drivers/of/fdt.c:1248
  │    扁平扫描(不建树):/chosen(bootargs!)/memory/reg/initrd/...
  │    失败则原地死循环并打印 "must be 8-byte aligned / ≤2MB"
  └─ fixmap_remap_fdt(..., PAGE_KERNEL_RO)       setup.c:200  重映射为只读
```

② 展开为运行期树（[drivers/of/fdt.c:1283](../../000.SOURCE_CODE/003.LINUX-7.1.3/000.LINUX-7.1.3/drivers/of/fdt.c)）：

```
unflatten_device_tree()                          fdt.c:1283
  ├─ fdt_scan_reserved_mem_late()                reserved-memory 兜底
  ├─ (bootloader 没给 DTB 时) 用内核内置 __dtb_empty_root 兜底
  └─ __unflatten_device_tree(fdt, NULL, &of_root, ...)   fdt.c:351
        两遍:①扫描累计每个节点需要多少内存 ②分配+逐节点填充 device_node

  DTB 扁平 blob                          运行期 device_node 树(of_root)
  ┌──────────────────┐                  ┌─────────────────────┐
  │ (FDT_BEGIN_NODE) │  unflatten       │ of_root (/)          │
  │ /                │ ───────────────► │  ├─ cpus             │
  │  cpus            │                  │  │   ├─ cpu@0        │
  │   cpu@0          │                  │  │   └─ cpu@1        │
  │   cpu@1          │                  │  ├─ chosen           │
  │  chosen          │                  │  └─ serial@...       │
  └──────────────────┘                  └─────────────────────┘
      顺序字节流                            每节点一个 struct
                                             device_node(of.h:48):
                                             name/type/properties[]/
                                             child/sibling/parent 指针
```

## 6. 驱动匹配：compatible 的接力

节点要变成 `struct device` 再找到驱动，链条是：

```
 of_platform_default_populate_init() (arch_initcall_sync)   ← 029/002 §⑤
   └─ of_platform_populate(of_root,...)        drivers/of/platform.c:443
        遍历 device_node 树:每个带 compatible 的节点
        ──► platform_device 注册 ──► driver core 走总线的 match()
                  │
                  ▼
        platform_match() 依次试 4 种匹配,OF 路径:
        of_driver_match_device(dev, drv)
          └─ of_match_device(drv->of_match_table, dev)   drivers/of/device.c:26
               把节点 compatible 列表与驱动表逐条比对:
                  节点: compatible = "arm,vexpress,v2f-1xv7,ca53x2",  ◄─ 从最具体
                              "arm,vexpress,v2f-1xv7",                ▼   到最通用
                              "arm,vexpress"
                  驱动: { .compatible = "arm,vexpress", ... },       ◄─ 命中即用
               匹配成功 ──► drv->probe(dev)
```

驱动侧的声明（[include/linux/mod_devicetable.h:282](../../000.SOURCE_CODE/003.LINUX-7.1.3/000.LINUX-7.1.3/include/linux/mod_devicetable.h)）：

```c
struct of_device_id {
	char	name[32];
	char	compatible[128];   // ← 与 DTS 属性逐字符比对(前缀匹配)
	void	*data;              // 命中后随 match 指针带回驱动的设备特化数据
};
// 用法: static const struct of_device_id foo_of_match[] = {
//          { .compatible = "arm,pl011", .data = (void*)TYPE_PL011 },
//          {} };
//       MODULE_DEVICE_TABLE(of, foo_of_match);   // 也供 modprobe 按别名加载模块
```

启动日志对账（QEMU virt，5.9 实测样例在 [029/README](../029.内核启动/README.md)）：`Machine model: linux,dummy-virt`（根节点 model 属性）、`9000000.pl011: ttyAMA0 at MMIO`（节点名+compatible 命中 pl011 驱动）。

## 7. 实用工具速查

| 场景 | 工具 |
| - | - |
| 编译/反编译 | `make dtbs`；`dtc -I dtb -O dts` |
| 运行系统上自检 | `/proc/device-tree`（即 `/sys/firmware/devicetree/base`，属性是二进制文件，`xxd` 看） |
| 查某设备命中了哪个驱动 | `/sys/devices/platform/<dev>/driver → 链接`；`ls -l /sys/bus/platform/drivers/` |
| QEMU | `-machine virt,dumpdtb=virt.dtb` 导出固件视角 DTB；`-dtb` 换自编 DTB |
| 内核源码侧 | `drivers/of/`（运行期）、`scripts/dtc/`（编译期）、`Documentation/devicetree/bindings/`（契约） |

## 参考资料

- 内核文档：[booting.rst](../../000.SOURCE_CODE/003.LINUX-7.1.3/000.LINUX-7.1.3/Documentation/arch/arm64/booting.rst)（协议）、[usage-model.rst](../../000.SOURCE_CODE/003.LINUX-7.1.3/000.LINUX-7.1.3/Documentation/devicetree/usage-model.rst)（内核如何消费 DT）、[bindings/](../../000.SOURCE_CODE/003.LINUX-7.1.3/000.LINUX-7.1.3/Documentation/devicetree/bindings/)（节点契约）
- 本仓库姊妹篇：[029.内核启动/002.Linux内核启动与设备树](../029.内核启动/002.Linux内核启动与设备树.md)（五阶段时序深潜，5.9）、[000.pgd_set_fixmap原理](../029.内核启动/000.pgd_set_fixmap原理.md)（FIX_FDT 窗口的实现机制）
- 源码锚点：`arch/arm64/kernel/setup.c:170/292/342`、`arch/arm64/mm/fixmap.c:137`、`drivers/of/fdt.c:351/1248/1283`、`drivers/of/device.c:26`、`drivers/of/platform.c:443`、`include/linux/of.h:48`、`include/linux/mod_devicetable.h:282`
