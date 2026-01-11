# Control Groups(cgroups)

## What is cgroup? / What are cgroups
|What is cgroup? / What are cgroups|解释说明|备注|
|-|-|-|
|cgroup-v2|cgroup is a mechanism to organize processes hierarchically and distribute system resources along the hierarchy in a controlled and configurable manner|cgroup 是一种以层级化方式组织进程，并按该层级结构，以受控且可配置的方式分配系统资源的机制。|
|-|-|-|
|cgroup-v1|Control Groups provide a mechanism for aggregating/partitioning sets of tasks, and all their future children, into hierarchical groups with specialized behaviour. |控制组（Control Groups）提供了一种机制，能够将一组任务及其所有未来生成的子任务进行聚合或划分，并将其放入具有特定行为的层级组中|
|-|-|-|


##  cgroup 组成部分
- 核心（core）：主要负责层级化地组织进程；
- 控制器（controllers）：大部分控制器负责 cgroup 层级中 特定类型的系统资源的分配，少部分 utility 控制器用于其他目的。

---

## 相关概念
|名称|说明|备注|
|-|-|-|
|任务(Task)|系统中的一个进程，在内核中使用 struct task_struct表示|-|
|-|-|-|
|层级(hierarchy)|cgroup以树的形式组织，每一个树称之为一个层级|Hierarchy 怎么理解? 参考:[Linux资源管理之cgroups简介](../999.IMGS/Screenshot%202026-01-08%20at%2023-03-23%20Linux资源管理之cgroups简介%20-%20美团技术团队.png) <sub>应该是cgroup v1</sub> <br/> - [001.UNIX-DOCS/032.Control-Groups(cgroups)/cgroup-hierarchy.md](./cgroup-hierarchy.md) <br/> - [000.LINUX-5.9/Documentation/admin-guide/man7-cgroups.7.md](../../000.SOURCE_CODE/000.LINUX-5.9/000.LINUX-5.9/Documentation/admin-guide/man7-cgroups.7.md)#'Terminology(术语)' <br/>  |
|-|-|-|
|子系统(subsystem/controller)|一个资源控制器|- memory:设定内存使用限制，统计内存使用情况 </br> - HugeTLB: 限制透明大页的使用量 </br> - cpu: 限制进程CPU的使用率;</br> - cpuacct 子系统，可以统计 cgroups 中的进程的 cpu 使用报告。</br>- cpuset: 为进程分配单独的cpu/mem节点 </br>- blkio: 为块设备设定输入/输出限制 </br> - devices: 允许或拒绝进程访问设备 </br> - net_cls: 标记进程的网络数据包，并进行控制 </br> - net_prio: 动态配置进程每隔网络接口的流量优先级 </br> - freezer: 挂起或恢复进程 </br>- ns 子系统，可以使不同 cgroups 下面的进程使用不同的 namespace。|


---

## 在内核中的实现
```c
   struct task_struct {
        // ... 

        /**
         * A css_set contains a set of reference-counted pointers to cgroup_subsys_state objects, one for each cgroup subsystem registered in the system. There is no direct link from a task to the cgroup of which it’s a member in each hierarchy, but this can be determined by following pointers through the cgroup_subsys_state objects. 
         * (一个 css_set 包含了一组带有引用计数的指针，这些指针指向 cgroup_subsys_state（cgroup 子系统状态）对象，系统中注册的每个 cgroup 子系统都对应其中一个指针。在任务（task）与其在各个层级结构中所对应的 cgroup 成员身份之间，并没有直接的链接，但可以通过追踪 cgroup_subsys_state 对象的指针来确定这种关系。)
         * 
         * This is because accessing the subsystem state is something that’s expected to happen frequently and in performance-critical code, whereas operations that require a task’s actual cgroup assignments (in particular, moving between cgroups) are less common. A linked list runs through the cg_list field of each task_struct using the css_set, anchored at css_set->tasks.
         * (之所以这样设计，是因为访问子系统状态的操作通常发生得非常频繁，且处于对性能要求极高的关键代码路径中；相比之下，需要获取任务实际 cgroup 分配情况的操作（尤其是进程在不同 cgroup 间迁移）则较少发生。每个 task_struct 的 cg_list 字段都会通过 css_set 连接成一个链表，该链表锚定在 css_set->tasks 处)
         * 
         * IN (https://www.kernel.org/doc/html/v5.9/admin-guide/cgroup-v1/cgroups.html # '1.3 How are cgroups implemented ?')
         * 
         * 为什么要引入 css_set？（性能优化） 在 Linux 中，一个进程可能同时属于多个 cgroup 层级（例如 CPU 限流在 A 组，内存限流在 B 组）。如果每次调度进程都要去遍历所有的层级来查找资源限制，性能会很差。 css_set 就像是一个缓存代理，它把这个进程在所有子系统中的状态（CSS）打包在一起。进程只需要持有一个指向 css_set 的指针，就能以 O(1) 的复杂度直接访问到 CPU、内存等所有限制数据。
         * “间接链接”的含义 任务（task_struct）不直接指向 cgroup 对象，而是指向 css_set。
         *     - 路径： task_struct → css_set → cgroup_subsys_state → cgroup
         * 
         * 多对一的映射 通常，系统中成百上千个进程（比如同一个容器内的所有进程）拥有完全相同的资源限制。在这种情况下，它们会共享同一个 css_set 对象。内核通过 css_set->tasks 这个链表头，将所有共享该配置的进程（task_struct）串联起来。
         */ 
        struct css_set __rcu		*cgroups;
        // ... 
   }
```

The implementation of cgroups requires a few, simple hooks into the rest of the kernel, none in performance-critical paths: <sup>IN (https://www.kernel.org/doc/html/v5.9/admin-guide/cgroup-v1/cgroups.html # '1.3 How are cgroups implemented ?')</sup>（cgroups 的实现仅需在内核其余部分插入少量简单的钩子（hooks），且这些钩子都不位于性能关键路径（performance-critical paths）中。）
- in init/main.c, to initialize the root cgroups and initial css_set at system boot.
- in fork and exit, to attach and detach a task from its css_set.(在 fork（创建进程）和 exit（退出进程）过程中，用于将任务关联至其 css_set 或从中分离。)

### 在系统运行时，cgroup时如何生效的呢?
| 阶段 | 行为 | cgroup 是否干预 | 理由 |
| --- | --- | --- | --- |
| **虚拟内存申请** (`malloc`) | 仅修改 VMA 结构体 | **否**<sub>阅读内存页分配流程代码，确实没有cgroup相关代码</sub> | 尚未消耗物理资源。 |
| **初次读写内存** | 触发缺页异常，申请物理页 | **是(待验证)** | 这是物理内存分配的入口点，必须在此处计费（Charge）。 |
| **内存交换/换入** (Swap In) | 从磁盘读取数据回内存 | **是(待验证)** | 数据回到物理内存，占用重新增加。 |
| **写入已存在页** | 仅仅修改页表权限 | **否(待验证)** | 物理页早已被计费过，不涉及新资源分配。 |


---

## 实操
> 参考: 

---
## 参考资料
- [https://www.kernel.org/doc/html/v5.9/admin-guide/cgroup-v1/cgroups.html](https://www.kernel.org/doc/html/v5.9/admin-guide/cgroup-v1/cgroups.html)
- [https://www.kernel.org/doc/html/v6.1/admin-guide/cgroup-v2.html](https://www.kernel.org/doc/html/v6.1/admin-guide/cgroup-v2.html)
- [Alibaba Cloud Linux: cgroup v1与cgroup v2的区别](https://www.alibabacloud.com/help/zh/alinux/differences-between-cgroup-v1-and-cgroup-v2?spm=a2c63.p38356.help-menu-2632541.d_4_0_2.7c0555d0sdvMjQ)
- [openEuler kernel 技术分享-第17期-cgroup介绍](https://www.bilibili.com/video/BV1e34y1q7bR?spm_id_from=333.788.videopod.sections&vd_source=9eef164b234175c1ae3ca71733d5a727)
- [000.SOURCE_CODE/000.LINUX-5.9/000.LINUX-5.9/Documentation/admin-guide/cgroup-v2.rst](../../000.SOURCE_CODE/000.LINUX-5.9/000.LINUX-5.9/Documentation/admin-guide/cgroup-v2.rst)