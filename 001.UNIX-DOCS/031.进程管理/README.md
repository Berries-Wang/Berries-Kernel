# 进程管理/调度
> 阅读: <br/> - [处理器调度：单核调度策略](../../001.UNIX-DOCS/000.内存管理/998.REFS/000.中山大学-操作系统/6-0331-schedule-1.pdf) <br/> - [处理器调度：多核调度策略](../../../001.UNIX-DOCS/000.内存管理/998.REFS/000.中山大学-操作系统/7-0407-schedule-2.pdf) <br/> - [Run Linux Kernel (2nd Edition) Volume 1: Infrastructure](../../007.BOOKs/Run%20Linux%20Kernel%20(2nd%20Edition)%20Volume%201:%20Infrastructure.epub)

---

## 进程调度
|类型|核心概念|说明|备注|
|-|-|-|-|
|- 多核调度|- 任务在CPU核心之间频繁切换，对缓存不友好<sup>:因为可能之前的缓存无法使用<sup>|- 每个CPU核心(逻辑核心)都有自己的L1缓存 <br/> - 两个同属一个物理核心的逻辑核心，共享L2缓存 <br/> - 一个物理CPU上的所有物理核心共享L3缓存 |- ![wechat_2026-03-19_073523_109.png](./999.IMGS/wechat_2026-03-19_073523_109.png)|
|-|-|-|-|
|- Linux的多核调度： 标准的两级调度|- 每个CPU有各自的本地调度器和调度队列(rq)|-|- ![wechat_2026-03-19_074551_996.png](./999.IMGS/wechat_2026-03-19_074551_996.png)|
|- 两级调度问题: 负载均衡|-|-|- ![wechat_2026-03-19_074727_708.png](./999.IMGS/wechat_2026-03-19_074727_708.png)|
|-两级调度问题: 负载均衡: 负载追踪(负载计算) -> PELT(Per Entity Load Tracking)|-|-|- [处理器调度：多核调度策略](../../../001.UNIX-DOCS/000.内存管理/998.REFS/000.中山大学-操作系统/7-0407-schedule-2.pdf) > P18|
|-|-|-|-|
|-程序主动选择用哪个CPU来运行|-|-|cpu_set_t mask;<br/>  CPU_ZERO(&mask);<br/> CPU_SET(0,&mask);<br/> CPU_SET(0,&mask);<br/> sched_setaffinity(0,sizeof(mask),&mask);|
|-|-|-|-|

---

