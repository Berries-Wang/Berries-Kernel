# 系统虚拟化

## 虚拟化核心 - 虚拟机监控器（Virtual Machine Monitor VMM,Hypervisor）
- 向上层虚拟机暴露其所需要的ISA（Instruction Set Architecture）
- 可同时运行多台虚拟机(VM)


### 虚拟机监控器分类
<table>
<th>虚拟化类型</th><th>说明</th><th>虚拟化技术</th><th>参考</th>
<tr><td>Type-1</td><td>直接运行在硬件之上</td><td>- VMware ESXi (vSphere): 商业化最成熟，功能最全，企业级私有云的首选。<br/> - KVM (Kernel-based Virtual Machine)：Linux 内核的一部分。目前的公有云（亚马逊 AWS、阿里云、腾讯云）底层几乎全是 KVM 的变体<br/>- Microsoft Hyper-V：Windows Server 自带，与 Windows 生态集成度极高。<br/>- Xen：以安全隔离性著称，虽然在通用云市场被 KVM 赶超，但在安全系统（如 Qubes OS）和某些嵌入式场景仍在使用<br/></td><td rowspan="3"><img src="./998.IMGS/wechat_2026-01-19_073810_234.png"/></td></tr>
<tr><td> </td><td></td><td></td></tr>
<tr><td>Type-2</td><td>虚拟机当做物理机操作系统的一个进程</td><td>- Oracle VirtualBox：开源免费，跨平台支持好 <br/> -  VMware Workstation / Player：Windows/Linux 用户常用的桌面虚拟化</td></tr>
</table>

### 如何实现系统虚拟化
> 关键点(难点)： 如何管理好系统ISA

---

## 参考资料
- [23-虚拟化：CPU虚拟化 [中山大学 操作系统原理]](./000.REF-DOCS/13-0522-virt-1.pdf)
- [24-虚拟化：内存、设备虚拟化 [中山大学 操作系统原理]](./000.REF-DOCS/14-0526-virt-2.pdf)
