# Graduation Project
> 应用 Linux内核学习 成果

## 计划
|项目名称|进度|成果|
|-|-|-|
|内核重启排查（实战：分析云主机 srv1572367 的一次重启）|进行中|[001.内核重启排查](./001.内核重启排查/README.md)|
|高负载 D 状态排查（负载 100 而 CPU 空闲、kill -9 无效）|已完成|[002.高负载D状态排查](./002.高负载D状态排查/README.md)|
|U-Boot + Linux内核源码，启动Orange-PI|未开始||
|编写一个系统调用: 获取当前系统时间|未开始||
|简单的设备驱动: 点亮一个LED|未开始||

## 通用命令手册

- [dmesg命令手册](./dmesg命令手册.md) —— 从 001/002 实战沉淀的 dmesg 用法与场景（原理边界、选项速查、dmesg vs journalctl -k 选型、开机巡检/故障进行时/重启之后三类场景）
- [journalctl命令手册](./journalctl命令手册.md) —— 同上风格的 journalctl 用法与场景（持久化前提、dmesg 四条边界的解法、时间窗/跨启动查询、磁盘治理）
- [journalctl.man.中文版](./journalctl.man.中文版.md) —— systemd 262 官方 journalctl(1) man 页的全量中文翻译（本仓库无 systemd 源码，译自上游）
