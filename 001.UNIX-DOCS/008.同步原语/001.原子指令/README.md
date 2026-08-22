# 原子指令<sup>ARM64</sup>
- arm64： <br/>[000.LINUX-5.9/arch/arm64/include/asm/atomic.h](../../000.SOURCE_CODE/000.LINUX-5.9/000.LINUX-5.9/arch/arm64/include/asm/atomic.h) <br/> [000.LINUX-5.9/arch/arm64/include/asm/atomic_ll_sc.h](../../000.SOURCE_CODE/000.LINUX-5.9/000.LINUX-5.9/arch/arm64/include/asm/atomic_ll_sc.h)<br/> LL/SC 实现原理（架构手册分析）： [003.LLSC实现原理.md](./003.LLSC实现原理.md) <br/> CAS 实现原理（架构手册分析）： [002.CAS实现原理.md](./002.CAS实现原理.md)


原子操作是指保证指令以原子的方式执行，执行过程中不会被打断。

## 原子指令族
|指令类型|代表指令|说明|备注|
|-|-|-|-|
|- LL/SC|LDXR / STXR（含 LDAXR / STLXR 等）|Load-Exclusive 读+标记；Store-Exclusive 条件写，失败返回状态位，软件循环重试|Monitor（独占监视器）是 LDXR/STXR 专用的机制，原子性靠"监视器检测他人写入→STXR 失败"兑现，见 [003.LLSC实现原理.md](./003.LLSC实现原理.md)|
|-|-|-|-|
|- LSE(Large System Extensions) |CAS / CASP / SWP / LDADD 等|单条指令完成无条件原子 read-modify-write|LSE 的 CAS 走的是另一套机制——由缓存一致性系统执行的原子 RMW，见 [002.CAS实现原理.md](./002.CAS实现原理.md)|

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