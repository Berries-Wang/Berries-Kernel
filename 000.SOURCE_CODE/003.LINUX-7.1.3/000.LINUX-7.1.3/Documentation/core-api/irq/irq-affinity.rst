================================
SMP IRQ affinity(SMP IRQ 亲和性)
================================

ChangeLog:
(变更日志：)
	- Started by Ingo Molnar <mingo@redhat.com>
	  (由 Ingo Molnar <mingo@redhat.com> 发起。)
	- Update by Max Krasnyansky <maxk@qualcomm.com>
	  (由 Max Krasnyansky <maxk@qualcomm.com> 更新。)


/proc/irq/IRQ#/smp_affinity and /proc/irq/IRQ#/smp_affinity_list specify
which target CPUs are permitted for a given IRQ source.  It's a bitmask
(smp_affinity) or CPU list (smp_affinity_list) of allowed CPUs.  It's not
allowed to turn off all CPUs, and if an IRQ controller does not support
IRQ affinity then the value will not change from the default of all CPUs.
( ``/proc/irq/IRQ#/smp_affinity`` 与 ``/proc/irq/IRQ#/smp_affinity_list``
指定给定 IRQ 源所允许的目标 CPU。它是允许 CPU 的位掩码 (smp_affinity)
或 CPU 列表 (smp_affinity_list)。不允许关闭所有 CPU；如果 IRQ 控制器
不支持 IRQ 亲和性，则该值不会偏离"全部 CPU"的默认值。)

/proc/irq/default_smp_affinity specifies default affinity mask that applies
to all non-active IRQs. Once IRQ is allocated/activated its affinity bitmask
will be set to the default mask. It can then be changed as described above.
Default mask is 0xffffffff.
( ``/proc/irq/default_smp_affinity`` 指定应用于所有非活动 IRQ 的默认亲
和性掩码。IRQ 一旦被分配/激活，其亲和性位掩码就会被设置为默认掩码。
之后可以按上文所述更改它。默认掩码为 0xffffffff。)

Here is an example of restricting IRQ44 (eth1) to CPU0-3 then restricting
it to CPU4-7 (this is an 8-CPU SMP box)::

	[root@moon 44]# cd /proc/irq/44
	[root@moon 44]# cat smp_affinity
	ffffffff

	[root@moon 44]# echo 0f > smp_affinity
	[root@moon 44]# cat smp_affinity
	0000000f
	[root@moon 44]# ping -f h
	PING hell (195.4.7.3): 56 data bytes
	...
	--- hell ping statistics ---
	6029 packets transmitted, 6027 packets received, 0% packet loss
	round-trip min/avg/max = 0.1/0.1/0.4 ms
	[root@moon 44]# cat /proc/interrupts | grep 'CPU\|44:'
		CPU0       CPU1       CPU2       CPU3      CPU4       CPU5        CPU6       CPU7
	44:       1068       1785       1785       1783         0          0           0         0    IO-APIC-level  eth1

(下面是一个把 IRQ44 (eth1) 先限制到 CPU0-3、再限制到 CPU4-7 (这是一
台 8 CPU 的 SMP 机器) 的示例：)

As can be seen from the line above IRQ44 was delivered only to the first four
processors (0-3).
Now lets restrict that IRQ to CPU(4-7).
(从上面的输出行可以看出，IRQ44 只被投递到前四个处理器 (0-3)。
现在让我们把该 IRQ 限制到 CPU(4-7)。)

::
	[root@moon 44]# echo f0 > smp_affinity
	[root@moon 44]# cat smp_affinity
	000000f0
	[root@moon 44]# ping -f h
	PING hell (195.4.7.3): 56 data bytes
	..
	--- hell ping statistics ---
	2779 packets transmitted, 2777 packets received, 0% packet loss
	round-trip min/avg/max = 0.1/0.5/585.4 ms
	[root@moon 44]# cat /proc/interrupts |  'CPU\|44:'
		CPU0       CPU1       CPU2       CPU3      CPU4       CPU5        CPU6       CPU7
	44:       1068       1785       1785       1783      1784       1069        1070       1069   IO-APIC-level  eth1

This time around IRQ44 was delivered only to the last four processors.
i.e counters for the CPU0-3 did not change.
(这一次，IRQ44 只被投递到最后四个处理器。
也就是说，CPU0-3 的计数器没有变化。)

Here is an example of limiting that same IRQ (44) to CPUs 1024 to 1031::

	[root@moon 44]# echo 1024-1031 > smp_affinity_list
	[root@moon 44]# cat smp_affinity_list
	1024-1031

(下面是一个把同一个 IRQ (44) 限制到 CPU 1024 至 1031 的示例：)

Note that to do this with a bitmask would require 32 bitmasks of zero
to follow the pertinent one.
(注意，若用位掩码来做这件事，需要在相关的那个掩码之后再写 32 个全零
的位掩码。)
