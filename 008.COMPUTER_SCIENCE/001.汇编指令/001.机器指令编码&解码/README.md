# 机器指令编码&解码
> 学习: [C4.1 A64 instruction set encoding](../../../007.BOOKs/Arm%20Architecture%20Reference%20Manual%20for%20A-profile%20architecture/)

## 0. 一句话总结

**A64 每条指令固定 32 位、小端存放**（内存中低字节在前）。解码 = 在 32 位里找"固定指纹"（某些位段恒定）→ 按该指令的位域定义切字段 → 查出寄存器号/立即数。本篇给"指纹表 + 三个完整走例"，全部编码经过 `.inst` 反向验证（见 §4 方法）。

## 1. 解码四步法

```
 拿到 32 位(如反汇编/内存dump)
   │
   ▼ ① 找指纹:比对常见指令的固定位段(§2 指纹表)
   │     90000008 = 1001 0000 ... → bits28:24=10000, bit31=1 → ADRP 家族!
   ▼ ② 切字段:按该指令的位域图把剩余位分给 imm/寄存器号
   │     immlo=bits30:29, immhi=bits23:5, Rd=bits4:0
   ▼ ③ 算语义:套该指令的操作公式(ADRP: Xd=(PC&~0xFFF)+(immhi:immlo<<12))
   ▼ ④ 交叉验证:.inst 原样发射 → objdump 解回 → 比对
```

## 2. 常见指令指纹表（由实测编码反推，字节序为反汇编显示的大端字）

| 指令 | 固定位段指纹 | 其余字段 | 实测样例 |
| - | - | - | - |
| `ADRP` | bit31=1, bits28:24=`10000` | immlo=30:29, immhi=23:5, Rd=4:0 | `90000008`→`adrp x8,0x0`；`F0000000`→`adrp x0,0x3000` |
| `ADR` | bit31=0, bits28:24=`10000` | 同上（字节粒度） | —— |
| `ADD/SUB (imm)` | bits28:24=`10001` | sf=31, op=30(SUB=1), S=29, shift=23:22, imm12=21:10, Rn=9:5, Rd=4:0 | `910043fd`→`add x29,sp,#0x10` |
| `MOVZ/MOVK (move wide)` | bits28:24=`10010` | sf=31, opc=30:29, hw=22:21, imm16=20:5, Rd=4:0 | `d2800000`→`mov x0,#0`(即 MOVZ) |
| `B` | bits30:26=`00101`, bit31=0 | imm26=25:0 | `140000xx` 类 |
| `BL` | bits30:26=`00101`, bit31=1 | imm26=25:0 | `94000000`→`bl +0` |
| `LDR/STR (imm, unsigned)` | bits29:27=`111`, bits25:24=`01`, V=26=0 | size=31:30, opc=23:22, imm12=21:10, Rn=9:5, Rt=4:0 | `b9400100`→`ldr w0,[x8]`；`f9400108`→`ldr x8,[x8]` |

> 这张表是**实用速查**（指纹均从下文实测编码核对），不是 ARM ARM 的完整分区——官方完整树状索引按 bits[28:25] 逐级展开，见 [C4.1](../../../007.BOOKs/Arm%20Architecture%20Reference%20Manual%20for%20A-profile%20architecture/)：`DP-Immediate → DP-Register → Loads/Stores → Branches → SIMD/FP ...`，遇到表外指令按 §1 四步法回到 C4.1 查树即可。

## 3. 三个完整走例（全部实测）

### 例 1：`90000008` → `adrp x8, 0x0`

```
 二进制: 1001 0000 0000 0000 0000 0000 0000 1000
          ││└─┬─┘└────────────┬───────────┘└─┬─┘
          ││  │               │              └─ Rd = 01000 = x8
          ││  │               └─ immhi = 0
          ││  └─ bits28:24 = 10000 (ADRP/ADR 家族指纹)
          │└─ immlo = 00
          └─ op = 1 → ADRP(页粒度)
 imm21 = immhi:immlo = 0 → 页差 0
 结果: X8 = (PC & ~0xFFF) + 0 → "adrp x8, 0x0"
 (这正是 [链接器 §4](../../../001.UNIX-DOCS/028.编译原理/000.链接器/README.md) 实验里那条等待链接器回填的占位指令,
  配套重定位 R_AARCH64_ADR_PREL_PG_HI21; ADRP 全解见 [001.ADRP](../000.arm64汇编指令/001.ADRP.md))
```

### 例 2：`94000000` → `bl +0`

```
 二进制: 1001 0100 0000 0000 0000 0000 0000 0000
          │└──┬──┘└──────────────┬──────────────┘
          │   └─ bits30:26 = 00101 (无条件跳转族指纹)
          │       └─ imm26 = 0 → 目标 = PC + (0 符号扩展<<2) = PC 自己
          └─ bit31 = 1 → BL(带链接, X30 = PC+4); =0 则是 B
 结果: 目标=PC+0, objdump 写作 "bl <本指令地址>"(imm26 单位是 4 字节)
```

### 例 3：`910043fd` → `add x29, sp, #0x10`

```
 二进制: 1001 0001 0000 0000 0100 0011 1111 1101
          │││  │    │       │     │      └─ Rd = 11101 = x29
          │││  │    │       │     └─ Rn = 11111 = sp(x31 此处作 SP)
          │││  │    │       └─ imm12 = bits[21:10] (视觉上最容易切错, 见下)
          │││  │    └─ shift = bits[23:22] = 00(不移位)
          │││  └─ bits28:24 = 10001 (ADD/SUB imm 指纹)
          ││└─ S = 0 (不置标志位)
          │└─ op = 0 (ADD; 1 则 SUB)
          └─ sf = 1 (64 位: x 寄存器)
 imm12 取 bits21:10 = 000000010000 = 0x10
 结果: X29 = SP + 0x10 → "add x29, sp, #0x10"   ✓帧指针标准开场
```

> 例 3 的 imm12 切位最容易错：它不是 12 个连续"看起来对齐"的位，而是严格 bits[21:10]。三个走例合起来正好覆盖"PC 相对取页 / 立即数跳转 / 寄存器+立即数运算"三类最常见的内核代码骨架。

## 4. 方法：`.inst` 与 objdump 双向验证

汇编器的 `.inst` 伪指令按**原始编码**发射指令——手工算出 32 位后可立刻闭环：

```shell
$ printf '\t.inst 0x90000008\n\t.inst 0x94000000\n\t.inst 0x910043fd\n\t.inst 0xb9400100\n' \
    | clang -target aarch64-linux-gnu -c -x assembler - -o v.o
$ llvm-objdump -d v.o                       # macOS 自带工具即可
       0: 90000008      adrp x8, 0x0 <.text>
       4: 94000000      bl   0x4 <.text+0x4>
       8: 910043fd      add  x29, sp, #0x10
       c: b9400100      ldr  w0, [x8]
```

反方向（汇编→看编码）：

```shell
$ clang -target aarch64-linux-gnu -c xx.S -o xx.o && llvm-objdump -d xx.o
```

本仓库实验源码：[003.TEST-SPACE/018.adrp_encoding.S](../../../003.TEST-SPACE/018.adrp_encoding.S)（ADRP 占位+重定位的原始观察）。

## 5. FAQ

**Q：为什么反汇编显示 `90000008`，内存里却是 `08 00 00 90`？**
A：A64 指令按**小端**存放——32 位字的低字节在低地址。objdump 显示的是整字（大端读法），dump 内存看到的是字节序。

**Q：C 代码里的函数到这层怎么对上？**
A：`objdump -d vmlinux`（或任意 `.o`）按地址对齐指令流，符号名来自 ELF 符号表——地址↔符号的翻译正是 [链接器](../../../001.UNIX-DOCS/028.编译原理/000.链接器/README.md) 的产物（kallsyms 是内核自带的那套）。

**Q：遇到指纹表外的指令怎么办？**
A：四步法第 ① 步换成"查 C4.1 树"：手册按 bits[28:25] 分大组、组内再按 opcode 位段细分，每个叶子页顶都有完整位域图。

## 参考

| 资料 | 链接 | 用途 |
| - | - | - |
| A-profile ARM ARM | [PDF](../../../007.BOOKs/Arm%20Architecture%20Reference%20Manual%20for%20A-profile%20architecture/) | C4.1 编码总索引 + 各指令位域图 |
| DDI 0602 在线版 | [Base Instructions](https://developer.arm.com/documentation/ddi0602/2025-09/Base-Instructions) | 逐条指令（含每条的操作伪代码） |
| 姊妹篇 | [000.arm64汇编指令](../000.arm64汇编指令/README.md) | 单指令深潜（ADRP 等） |
