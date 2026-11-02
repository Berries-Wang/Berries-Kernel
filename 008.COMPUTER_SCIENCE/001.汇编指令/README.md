# 汇编指令（arm64/A64）

从**编码视角**与**单条指令视角**两个方向吃透 A64：前者给"32 位怎么切成字段"的通法，后者逐条深潜内核里最常打交道的指令。

## 目录

| 子目录/文档 | 视角 | 内容 |
| - | - | - |
| [000.arm64汇编指令](./000.arm64汇编指令/) | 单指令深潜 | [000.微指令-SignExtend](./000.arm64汇编指令/000.微指令-SignExtend.md)（手册伪代码原语：SignExtend/Replicate/Zeros）；[001.ADRP](./000.arm64汇编指令/001.ADRP.md)（PC 相对取 4KB 页基址：编码/伪代码/占位与回填/内核 head.S 实例） |
| [001.机器指令编码&解码](./001.机器指令编码&解码/README.md) | 编码视角 | A64 定长 32 位；解码四步法；常见指令指纹表（实测反推）；ADRP/BL/ADD 三个完整走例；`.inst`+objdump 双向验证法 |

## 阅读路线

```
 想看懂一条反汇编        想弄懂某条指令的全部语义
        │                      │
        ▼                      ▼
 001.机器指令编码&解码      000.arm64汇编指令/001.ADRP
   (指纹表+四步法)            (以点带面:编码+伪代码+重定位+内核实例)
        │                      │
        └──────────┬───────────┘
                   ▼
        每篇都回链 [028.编译原理](../../001.UNIX-DOCS/028.编译原理/README.md)
        ——指令里那些"等着回填的立即数"就是链接器的重定位账本
```

## 工具与实验

| 工具 | 用途 |
| - | - |
| `clang -target aarch64-linux-gnu -c` | 在 macOS 上产出 arm64 ELF 目标文件（含 `.S`） |
| `llvm-objdump -d -r` | 反汇编 + **重定位记录**（看"占位 0 + 记账"最直观） |
| `llvm-nm` | 符号表（U/T/D/B/V 分类） |
| `.inst 0x......` | 汇编器伪指令：按原始编码发射，用于手工编码的反向验证 |
| 实验源码 | [003.TEST-SPACE/018.adrp_encoding.S](../../003.TEST-SPACE/018.adrp_encoding.S) |

## 参考

| 资料 | 链接 |
| - | - |
| Arm ARM for A-profile（DDI 0487） | [007.BOOKs 目录](../../007.BOOKs/Arm%20Architecture%20Reference%20Manual%20for%20A-profile%20architecture/) |
| DDI 0602 在线版 | [developer.arm.com/documentation/ddi0602](https://developer.arm.com/documentation/ddi0602/2025-09/Base-Instructions) |
| 相关内核笔记 | [008.同步原语/001.原子指令](../../001.UNIX-DOCS/008.同步原语/001.原子指令/README.md)（LSE/LLSC 指令族）、[028.编译原理](../../001.UNIX-DOCS/028.编译原理/README.md)（汇编与链接的接口） |
