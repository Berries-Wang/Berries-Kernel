# LINUX-MODULES(可加载模块)

## 精读系列
- [001.MOS5-10.5.5-Modules-in-Linux.md](001.MOS5-10.5.5-Modules-in-Linux.md) —— MOS5 §10.5.5 原文(书页 768-769/PDF 794)五步加载流程与 Linux 7.1.3 `kernel/module/main.c` 逐条对照:`finit_module` → `load_module` → 重定位/资源注册/`do_init_module` → `MODULE_STATE_LIVE`;含 `request_module` 自动加载与模块签名安全机制

## 快速命令
```bash
lsmod                 # 读 /proc/modules:已加载模块与引用计数
modinfo xx.ko         # 查看 MODINFO 元数据(license/author/参数)
modprobe -r xx        # 递归卸载(delete_module 系统调用)
cat /proc/kallsyms | grep '\[xx\]'   # 模块符号(带 [模块名] 后缀)
```
