# 原子指令<sup>ARM64</sup>
- arm64： <br/>[000.LINUX-5.9/arch/arm64/include/asm/atomic.h](../../../000.SOURCE_CODE/000.LINUX-5.9/000.LINUX-5.9/arch/arm64/include/asm/atomic.h) <br/> [000.LINUX-5.9/arch/arm64/include/asm/atomic_ll_sc.h](../../../000.SOURCE_CODE/000.LINUX-5.9/000.LINUX-5.9/arch/arm64/include/asm/atomic_ll_sc.h)<br/> [000.LINUX-5.9/arch/arm64/include/asm/atomic_lse.h](../../../000.SOURCE_CODE/000.LINUX-5.9/000.LINUX-5.9/arch/arm64/include/asm/atomic_lse.h)（LSE 路径）· [lse.h](../../../000.SOURCE_CODE/000.LINUX-5.9/000.LINUX-5.9/arch/arm64/include/asm/lse.h)（启动期选路）<br/> LL/SC 实现原理（架构手册分析）： [003.LLSC实现原理.md](./003.LLSC实现原理.md) <br/> CAS 实现原理（架构手册分析）： [002.CAS实现原理.md](./002.CAS实现原理.md) <br/> LL/SC 与 LSE 的区别（对照枢纽）： [004.LLSC与LSE的区别.md](./004.LLSC与LSE的区别.md)


原子操作是指保证指令以原子的方式执行，执行过程中不会被打断。

## RMW（Read-Modify-Write，读-改-写）：原子操作的标准形状

**RMW = 一次内存更新的三段式：读旧值 → 计算/修改 → 写回新值**。`counter++`（读→加→写）、`xchg`（读→写新值、旧值返回）、CAS（读→比较→条件写）、test-and-set（读→置 1）全是 RMW——原子指令族的目标，就是把这三段**焊成不可分割的一笔**。

为什么要"焊"：用普通 `ldr`/`str` 做三段式是三笔独立访存，其他核可以插队。经典**丢更新（lost update）**：A、B 两核同时 `counter++`，都读到 5，各自加 1 写回 6——两次加法只生效一次。

arm64 的两种"焊法"（正对应下表两族指令）：

| | 形态 | 原子性来源 |
|-|-|-|
| LL/SC | 把 RMW **拆开**：`LDXR` 读+标记 → 计算 → `STXR` 条件写，失败重试 | 独占监视器事后检测"有人写过" → STXR 失败（[003.LLSC实现原理.md](./003.LLSC实现原理.md)）|
| LSE（x86 对应 `LOCK` 前缀）| 把 RMW **焊死**：单条指令在缓存行独占态上一笔完成 | 缓存一致性协议充当"锁"（[002.CAS实现原理.md](./002.CAS实现原理.md)）|

> 各文档常说的"**单笔 RMW**"即后者：读、改、写三个效应对外表现为一次不可分割的访存，其他观察者要么看到旧值、要么看到新值，绝不会看到中间态。LL/SC 重试环 → LSE 指令的逐条替换对照见 [003.LLSC实现原理.md §8](./003.LLSC实现原理.md)，全景区别对照见 [004.LLSC与LSE的区别.md](./004.LLSC与LSE的区别.md)。

## 原子指令族
|指令类型|代表指令|说明|备注|
|-|-|-|-|
|- LL/SC|LDXR / STXR（含 LDAXR / STLXR 等）|Load-Exclusive 读+标记；Store-Exclusive 条件写，失败返回状态位，软件循环重试|Monitor（独占监视器）是 LDXR/STXR 专用的机制，原子性靠"监视器检测他人写入→STXR 失败"兑现，见 [003.LLSC实现原理.md](./003.LLSC实现原理.md)|
|-|-|-|-|
|- LSE(Large System Extensions) |CAS / CASP / SWP / LDADD 等|单条指令完成无条件原子 read-modify-write|LSE 的 CAS 走的是另一套机制——由缓存一致性系统执行的原子 RMW，见 [002.CAS实现原理.md](./002.CAS实现原理.md)；LL/SC 重试环 → LSE 指令的替换对照与 LL/SC 的保留地见 [003.LLSC实现原理.md §8](./003.LLSC实现原理.md)|

## 1. 指令本质：LSE 的"读-比较-写"原子操作
CAS 家族属于 FEAT_LSE（Large System Extensions）（ARMv8.1 引入），页面上标注 (FEAT_LSE)。它是对传统 LL/SC 对（LDXR/STXR）的替代：
  - "The atomic instructions perform atomic read and write operations on a memory location such that the architecture guarantees that no modification of that memory location by another observer can occur between the read and the write."<sup>原子指令会对内存位置执行原子的读和写操作。架构保证，在读操作和写操作之间，其他观察者（other observer）无法对该内存位置进行任何修改。</sup>（C3-393）
  - LL/SC 需要"加载-尝试存储-失败重试"的循环，竞争激烈时反复回退；而 LSE CAS 是单条指令完成整个 read-modify-write，不需要重试。




---

## 附录
### 内核编译打开LSE
```txt
Linux内核启用 CONFIG_ARM64_LSE_ATOMICS 

Kernel Features  --->
    [*] Enable support for Large System Extensions (LSE) atomic instructions
```