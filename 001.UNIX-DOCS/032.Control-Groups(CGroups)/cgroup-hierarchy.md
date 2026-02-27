
wei@Berries:/sys/fs/cgroup$ tree . > /home/wei/Downloads/cgroup-hierarchy.md
.
├── cgroup.controllers
├── cgroup.max.depth
├── cgroup.max.descendants
├── cgroup.pressure
├── cgroup.procs
├── cgroup.stat
├── cgroup.subtree_control
├── cgroup.threads
├── cpu.pressure
├── cpuset.cpus.effective
├── cpuset.cpus.isolated
├── cpuset.mems.effective
├── cpu.stat
├── cpu.stat.local
├── dev-hugepages.mount
│   ├── cgroup.controllers
│   ├── cgroup.events
│   ├── cgroup.freeze
│   ├── cgroup.kill
│   ├── cgroup.max.depth
│   ├── cgroup.max.descendants
│   ├── cgroup.pressure
│   ├── cgroup.procs
│   ├── cgroup.stat
│   ├── cgroup.subtree_control
│   ├── cgroup.threads
│   ├── cgroup.type
│   ├── cpu.pressure
│   ├── cpu.stat
│   ├── cpu.stat.local
│   ├── io.pressure
│   ├── memory.current
│   ├── memory.events
│   ├── memory.events.local
│   ├── memory.high
│   ├── memory.low
│   ├── memory.max
│   ├── memory.min
│   ├── memory.numa_stat
│   ├── memory.oom.group
│   ├── memory.peak
│   ├── memory.pressure
│   ├── memory.reclaim
│   ├── memory.stat
│   ├── memory.swap.current
│   ├── memory.swap.events
│   ├── memory.swap.high
│   ├── memory.swap.max
│   ├── memory.swap.peak
│   ├── memory.zswap.current
│   ├── memory.zswap.max
│   ├── memory.zswap.writeback
│   ├── pids.current
│   ├── pids.events
│   ├── pids.max
│   └── pids.peak
├── dev-mqueue.mount
│   ├── cgroup.controllers
│   ├── cgroup.events
│   ├── cgroup.freeze
│   ├── cgroup.kill
│   ├── cgroup.max.depth
│   ├── cgroup.max.descendants
│   ├── cgroup.pressure
│   ├── cgroup.procs
│   ├── cgroup.stat
│   ├── cgroup.subtree_control
│   ├── cgroup.threads
│   ├── cgroup.type
│   ├── cpu.pressure
│   ├── cpu.stat
│   ├── cpu.stat.local
│   ├── io.pressure
│   ├── memory.current
│   ├── memory.events
│   ├── memory.events.local
│   ├── memory.high
│   ├── memory.low
│   ├── memory.max
│   ├── memory.min
│   ├── memory.numa_stat
│   ├── memory.oom.group
│   ├── memory.peak
│   ├── memory.pressure
│   ├── memory.reclaim
│   ├── memory.stat
│   ├── memory.swap.current
│   ├── memory.swap.events
│   ├── memory.swap.high
│   ├── memory.swap.max
│   ├── memory.swap.peak
│   ├── memory.zswap.current
│   ├── memory.zswap.max
│   ├── memory.zswap.writeback
│   ├── pids.current
│   ├── pids.events
│   ├── pids.max
│   └── pids.peak
├── init.scope
│   ├── cgroup.controllers
│   ├── cgroup.events
│   ├── cgroup.freeze
│   ├── cgroup.kill
│   ├── cgroup.max.depth
│   ├── cgroup.max.descendants
│   ├── cgroup.pressure
│   ├── cgroup.procs
│   ├── cgroup.stat
│   ├── cgroup.subtree_control
│   ├── cgroup.threads
│   ├── cgroup.type
│   ├── cpu.pressure
│   ├── cpu.stat
│   ├── cpu.stat.local
│   ├── io.pressure
│   ├── memory.current
│   ├── memory.events
│   ├── memory.events.local
│   ├── memory.high
│   ├── memory.low
│   ├── memory.max
│   ├── memory.min
│   ├── memory.numa_stat
│   ├── memory.oom.group
│   ├── memory.peak
│   ├── memory.pressure
│   ├── memory.reclaim
│   ├── memory.stat
│   ├── memory.swap.current
│   ├── memory.swap.events
│   ├── memory.swap.high
│   ├── memory.swap.max
│   ├── memory.swap.peak
│   ├── memory.zswap.current
│   ├── memory.zswap.max
│   ├── memory.zswap.writeback
│   ├── pids.current
│   ├── pids.events
│   ├── pids.max
│   └── pids.peak
├── io.cost.model
├── io.cost.qos
├── io.pressure
├── io.prio.class
├── io.stat
├── memory.numa_stat
├── memory.pressure
├── memory.reclaim
├── memory.stat
├── memory.zswap.writeback
├── misc.capacity
├── misc.current
├── proc-sys-fs-binfmt_misc.mount
│   ├── cgroup.controllers
│   ├── cgroup.events
│   ├── cgroup.freeze
│   ├── cgroup.kill
│   ├── cgroup.max.depth
│   ├── cgroup.max.descendants
│   ├── cgroup.pressure
│   ├── cgroup.procs
│   ├── cgroup.stat
│   ├── cgroup.subtree_control
│   ├── cgroup.threads
│   ├── cgroup.type
│   ├── cpu.pressure
│   ├── cpu.stat
│   ├── cpu.stat.local
│   ├── io.pressure
│   ├── memory.current
│   ├── memory.events
│   ├── memory.events.local
│   ├── memory.high
│   ├── memory.low
│   ├── memory.max
│   ├── memory.min
│   ├── memory.numa_stat
│   ├── memory.oom.group
│   ├── memory.peak
│   ├── memory.pressure
│   ├── memory.reclaim
│   ├── memory.stat
│   ├── memory.swap.current
│   ├── memory.swap.events
│   ├── memory.swap.high
│   ├── memory.swap.max
│   ├── memory.swap.peak
│   ├── memory.zswap.current
│   ├── memory.zswap.max
│   ├── memory.zswap.writeback
│   ├── pids.current
│   ├── pids.events
│   ├── pids.max
│   └── pids.peak
├── sys-fs-fuse-connections.mount
│   ├── cgroup.controllers
│   ├── cgroup.events
│   ├── cgroup.freeze
│   ├── cgroup.kill
│   ├── cgroup.max.depth
│   ├── cgroup.max.descendants
│   ├── cgroup.pressure
│   ├── cgroup.procs
│   ├── cgroup.stat
│   ├── cgroup.subtree_control
│   ├── cgroup.threads
│   ├── cgroup.type
│   ├── cpu.pressure
│   ├── cpu.stat
│   ├── cpu.stat.local
│   ├── io.pressure
│   ├── memory.current
│   ├── memory.events
│   ├── memory.events.local
│   ├── memory.high
│   ├── memory.low
│   ├── memory.max
│   ├── memory.min
│   ├── memory.numa_stat
│   ├── memory.oom.group
│   ├── memory.peak
│   ├── memory.pressure
│   ├── memory.reclaim
│   ├── memory.stat
│   ├── memory.swap.current
│   ├── memory.swap.events
│   ├── memory.swap.high
│   ├── memory.swap.max
│   ├── memory.swap.peak
│   ├── memory.zswap.current
│   ├── memory.zswap.max
│   ├── memory.zswap.writeback
│   ├── pids.current
│   ├── pids.events
│   ├── pids.max
│   └── pids.peak
├── sys-kernel-config.mount
│   ├── cgroup.controllers
│   ├── cgroup.events
│   ├── cgroup.freeze
│   ├── cgroup.kill
│   ├── cgroup.max.depth
│   ├── cgroup.max.descendants
│   ├── cgroup.pressure
│   ├── cgroup.procs
│   ├── cgroup.stat
│   ├── cgroup.subtree_control
│   ├── cgroup.threads
│   ├── cgroup.type
│   ├── cpu.pressure
│   ├── cpu.stat
│   ├── cpu.stat.local
│   ├── io.pressure
│   ├── memory.current
│   ├── memory.events
│   ├── memory.events.local
│   ├── memory.high
│   ├── memory.low
│   ├── memory.max
│   ├── memory.min
│   ├── memory.numa_stat
│   ├── memory.oom.group
│   ├── memory.peak
│   ├── memory.pressure
│   ├── memory.reclaim
│   ├── memory.stat
│   ├── memory.swap.current
│   ├── memory.swap.events
│   ├── memory.swap.high
│   ├── memory.swap.max
│   ├── memory.swap.peak
│   ├── memory.zswap.current
│   ├── memory.zswap.max
│   ├── memory.zswap.writeback
│   ├── pids.current
│   ├── pids.events
│   ├── pids.max
│   └── pids.peak
├── sys-kernel-debug.mount
│   ├── cgroup.controllers
│   ├── cgroup.events
│   ├── cgroup.freeze
│   ├── cgroup.kill
│   ├── cgroup.max.depth
│   ├── cgroup.max.descendants
│   ├── cgroup.pressure
│   ├── cgroup.procs
│   ├── cgroup.stat
│   ├── cgroup.subtree_control
│   ├── cgroup.threads
│   ├── cgroup.type
│   ├── cpu.pressure
│   ├── cpu.stat
│   ├── cpu.stat.local
│   ├── io.pressure
│   ├── memory.current
│   ├── memory.events
│   ├── memory.events.local
│   ├── memory.high
│   ├── memory.low
│   ├── memory.max
│   ├── memory.min
│   ├── memory.numa_stat
│   ├── memory.oom.group
│   ├── memory.peak
│   ├── memory.pressure
│   ├── memory.reclaim
│   ├── memory.stat
│   ├── memory.swap.current
│   ├── memory.swap.events
│   ├── memory.swap.high
│   ├── memory.swap.max
│   ├── memory.swap.peak
│   ├── memory.zswap.current
│   ├── memory.zswap.max
│   ├── memory.zswap.writeback
│   ├── pids.current
│   ├── pids.events
│   ├── pids.max
│   └── pids.peak
├── sys-kernel-tracing.mount
│   ├── cgroup.controllers
│   ├── cgroup.events
│   ├── cgroup.freeze
│   ├── cgroup.kill
│   ├── cgroup.max.depth
│   ├── cgroup.max.descendants
│   ├── cgroup.pressure
│   ├── cgroup.procs
│   ├── cgroup.stat
│   ├── cgroup.subtree_control
│   ├── cgroup.threads
│   ├── cgroup.type
│   ├── cpu.pressure
│   ├── cpu.stat
│   ├── cpu.stat.local
│   ├── io.pressure
│   ├── memory.current
│   ├── memory.events
│   ├── memory.events.local
│   ├── memory.high
│   ├── memory.low
│   ├── memory.max
│   ├── memory.min
│   ├── memory.numa_stat
│   ├── memory.oom.group
│   ├── memory.peak
│   ├── memory.pressure
│   ├── memory.reclaim
│   ├── memory.stat
│   ├── memory.swap.current
│   ├── memory.swap.events
│   ├── memory.swap.high
│   ├── memory.swap.max
│   ├── memory.swap.peak
│   ├── memory.zswap.current
│   ├── memory.zswap.max
│   ├── memory.zswap.writeback
│   ├── pids.current
│   ├── pids.events
│   ├── pids.max
│   └── pids.peak
├── system.slice
│   ├── accounts-daemon.service
│   │   ├── cgroup.controllers
│   │   ├── cgroup.events
│   │   ├── cgroup.freeze
│   │   ├── cgroup.kill
│   │   ├── cgroup.max.depth
│   │   ├── cgroup.max.descendants
│   │   ├── cgroup.pressure
│   │   ├── cgroup.procs
│   │   ├── cgroup.stat
│   │   ├── cgroup.subtree_control
│   │   ├── cgroup.threads
│   │   ├── cgroup.type
│   │   ├── cpu.pressure
│   │   ├── cpu.stat
│   │   ├── cpu.stat.local
│   │   ├── io.pressure
│   │   ├── memory.current
│   │   ├── memory.events
│   │   ├── memory.events.local
│   │   ├── memory.high
│   │   ├── memory.low
│   │   ├── memory.max
│   │   ├── memory.min
│   │   ├── memory.numa_stat
│   │   ├── memory.oom.group
│   │   ├── memory.peak
│   │   ├── memory.pressure
│   │   ├── memory.reclaim
│   │   ├── memory.stat
│   │   ├── memory.swap.current
│   │   ├── memory.swap.events
│   │   ├── memory.swap.high
│   │   ├── memory.swap.max
│   │   ├── memory.swap.peak
│   │   ├── memory.zswap.current
│   │   ├── memory.zswap.max
│   │   ├── memory.zswap.writeback
│   │   ├── pids.current
│   │   ├── pids.events
│   │   ├── pids.max
│   │   └── pids.peak
│   ├── acpid.service
│   │   ├── cgroup.controllers
│   │   ├── cgroup.events
│   │   ├── cgroup.freeze
│   │   ├── cgroup.kill
│   │   ├── cgroup.max.depth
│   │   ├── cgroup.max.descendants
│   │   ├── cgroup.pressure
│   │   ├── cgroup.procs
│   │   ├── cgroup.stat
│   │   ├── cgroup.subtree_control
│   │   ├── cgroup.threads
│   │   ├── cgroup.type
│   │   ├── cpu.pressure
│   │   ├── cpu.stat
│   │   ├── cpu.stat.local
│   │   ├── io.pressure
│   │   ├── memory.current
│   │   ├── memory.events
│   │   ├── memory.events.local
│   │   ├── memory.high
│   │   ├── memory.low
│   │   ├── memory.max
│   │   ├── memory.min
│   │   ├── memory.numa_stat
│   │   ├── memory.oom.group
│   │   ├── memory.peak
│   │   ├── memory.pressure
│   │   ├── memory.reclaim
│   │   ├── memory.stat
│   │   ├── memory.swap.current
│   │   ├── memory.swap.events
│   │   ├── memory.swap.high
│   │   ├── memory.swap.max
│   │   ├── memory.swap.peak
│   │   ├── memory.zswap.current
│   │   ├── memory.zswap.max
│   │   ├── memory.zswap.writeback
│   │   ├── pids.current
│   │   ├── pids.events
│   │   ├── pids.max
│   │   └── pids.peak
│   ├── avahi-daemon.service
│   │   ├── cgroup.controllers
│   │   ├── cgroup.events
│   │   ├── cgroup.freeze
│   │   ├── cgroup.kill
│   │   ├── cgroup.max.depth
│   │   ├── cgroup.max.descendants
│   │   ├── cgroup.pressure
│   │   ├── cgroup.procs
│   │   ├── cgroup.stat
│   │   ├── cgroup.subtree_control
│   │   ├── cgroup.threads
│   │   ├── cgroup.type
│   │   ├── cpu.pressure
│   │   ├── cpu.stat
│   │   ├── cpu.stat.local
│   │   ├── io.pressure
│   │   ├── memory.current
│   │   ├── memory.events
│   │   ├── memory.events.local
│   │   ├── memory.high
│   │   ├── memory.low
│   │   ├── memory.max
│   │   ├── memory.min
│   │   ├── memory.numa_stat
│   │   ├── memory.oom.group
│   │   ├── memory.peak
│   │   ├── memory.pressure
│   │   ├── memory.reclaim
│   │   ├── memory.stat
│   │   ├── memory.swap.current
│   │   ├── memory.swap.events
│   │   ├── memory.swap.high
│   │   ├── memory.swap.max
│   │   ├── memory.swap.peak
│   │   ├── memory.zswap.current
│   │   ├── memory.zswap.max
│   │   ├── memory.zswap.writeback
│   │   ├── pids.current
│   │   ├── pids.events
│   │   ├── pids.max
│   │   └── pids.peak
│   ├── boot-efi.mount
│   │   ├── cgroup.controllers
│   │   ├── cgroup.events
│   │   ├── cgroup.freeze
│   │   ├── cgroup.kill
│   │   ├── cgroup.max.depth
│   │   ├── cgroup.max.descendants
│   │   ├── cgroup.pressure
│   │   ├── cgroup.procs
│   │   ├── cgroup.stat
│   │   ├── cgroup.subtree_control
│   │   ├── cgroup.threads
│   │   ├── cgroup.type
│   │   ├── cpu.pressure
│   │   ├── cpu.stat
│   │   ├── cpu.stat.local
│   │   ├── io.pressure
│   │   ├── memory.current
│   │   ├── memory.events
│   │   ├── memory.events.local
│   │   ├── memory.high
│   │   ├── memory.low
│   │   ├── memory.max
│   │   ├── memory.min
│   │   ├── memory.numa_stat
│   │   ├── memory.oom.group
│   │   ├── memory.peak
│   │   ├── memory.pressure
│   │   ├── memory.reclaim
│   │   ├── memory.stat
│   │   ├── memory.swap.current
│   │   ├── memory.swap.events
│   │   ├── memory.swap.high
│   │   ├── memory.swap.max
│   │   ├── memory.swap.peak
│   │   ├── memory.zswap.current
│   │   ├── memory.zswap.max
│   │   ├── memory.zswap.writeback
│   │   ├── pids.current
│   │   ├── pids.events
│   │   ├── pids.max
│   │   └── pids.peak
│   ├── cgroup.controllers
│   ├── cgroup.events
│   ├── cgroup.freeze
│   ├── cgroup.kill
│   ├── cgroup.max.depth
│   ├── cgroup.max.descendants
│   ├── cgroup.pressure
│   ├── cgroup.procs
│   ├── cgroup.stat
│   ├── cgroup.subtree_control
│   ├── cgroup.threads
│   ├── cgroup.type
│   ├── colord.service
│   │   ├── cgroup.controllers
│   │   ├── cgroup.events
│   │   ├── cgroup.freeze
│   │   ├── cgroup.kill
│   │   ├── cgroup.max.depth
│   │   ├── cgroup.max.descendants
│   │   ├── cgroup.pressure
│   │   ├── cgroup.procs
│   │   ├── cgroup.stat
│   │   ├── cgroup.subtree_control
│   │   ├── cgroup.threads
│   │   ├── cgroup.type
│   │   ├── cpu.pressure
│   │   ├── cpu.stat
│   │   ├── cpu.stat.local
│   │   ├── io.pressure
│   │   ├── memory.current
│   │   ├── memory.events
│   │   ├── memory.events.local
│   │   ├── memory.high
│   │   ├── memory.low
│   │   ├── memory.max
│   │   ├── memory.min
│   │   ├── memory.numa_stat
│   │   ├── memory.oom.group
│   │   ├── memory.peak
│   │   ├── memory.pressure
│   │   ├── memory.reclaim
│   │   ├── memory.stat
│   │   ├── memory.swap.current
│   │   ├── memory.swap.events
│   │   ├── memory.swap.high
│   │   ├── memory.swap.max
│   │   ├── memory.swap.peak
│   │   ├── memory.zswap.current
│   │   ├── memory.zswap.max
│   │   ├── memory.zswap.writeback
│   │   ├── pids.current
│   │   ├── pids.events
│   │   ├── pids.max
│   │   └── pids.peak
│   ├── cpu.pressure
│   ├── cpu.stat
│   ├── cpu.stat.local
│   ├── cron.service
│   │   ├── cgroup.controllers
│   │   ├── cgroup.events
│   │   ├── cgroup.freeze
│   │   ├── cgroup.kill
│   │   ├── cgroup.max.depth
│   │   ├── cgroup.max.descendants
│   │   ├── cgroup.pressure
│   │   ├── cgroup.procs
│   │   ├── cgroup.stat
│   │   ├── cgroup.subtree_control
│   │   ├── cgroup.threads
│   │   ├── cgroup.type
│   │   ├── cpu.pressure
│   │   ├── cpu.stat
│   │   ├── cpu.stat.local
│   │   ├── io.pressure
│   │   ├── memory.current
│   │   ├── memory.events
│   │   ├── memory.events.local
│   │   ├── memory.high
│   │   ├── memory.low
│   │   ├── memory.max
│   │   ├── memory.min
│   │   ├── memory.numa_stat
│   │   ├── memory.oom.group
│   │   ├── memory.peak
│   │   ├── memory.pressure
│   │   ├── memory.reclaim
│   │   ├── memory.stat
│   │   ├── memory.swap.current
│   │   ├── memory.swap.events
│   │   ├── memory.swap.high
│   │   ├── memory.swap.max
│   │   ├── memory.swap.peak
│   │   ├── memory.zswap.current
│   │   ├── memory.zswap.max
│   │   ├── memory.zswap.writeback
│   │   ├── pids.current
│   │   ├── pids.events
│   │   ├── pids.max
│   │   └── pids.peak
│   ├── cups-browsed.service
│   │   ├── cgroup.controllers
│   │   ├── cgroup.events
│   │   ├── cgroup.freeze
│   │   ├── cgroup.kill
│   │   ├── cgroup.max.depth
│   │   ├── cgroup.max.descendants
│   │   ├── cgroup.pressure
│   │   ├── cgroup.procs
│   │   ├── cgroup.stat
│   │   ├── cgroup.subtree_control
│   │   ├── cgroup.threads
│   │   ├── cgroup.type
│   │   ├── cpu.pressure
│   │   ├── cpu.stat
│   │   ├── cpu.stat.local
│   │   ├── io.pressure
│   │   ├── memory.current
│   │   ├── memory.events
│   │   ├── memory.events.local
│   │   ├── memory.high
│   │   ├── memory.low
│   │   ├── memory.max
│   │   ├── memory.min
│   │   ├── memory.numa_stat
│   │   ├── memory.oom.group
│   │   ├── memory.peak
│   │   ├── memory.pressure
│   │   ├── memory.reclaim
│   │   ├── memory.stat
│   │   ├── memory.swap.current
│   │   ├── memory.swap.events
│   │   ├── memory.swap.high
│   │   ├── memory.swap.max
│   │   ├── memory.swap.peak
│   │   ├── memory.zswap.current
│   │   ├── memory.zswap.max
│   │   ├── memory.zswap.writeback
│   │   ├── pids.current
│   │   ├── pids.events
│   │   ├── pids.max
│   │   └── pids.peak
│   ├── cups.service
│   │   ├── cgroup.controllers
│   │   ├── cgroup.events
│   │   ├── cgroup.freeze
│   │   ├── cgroup.kill
│   │   ├── cgroup.max.depth
│   │   ├── cgroup.max.descendants
│   │   ├── cgroup.pressure
│   │   ├── cgroup.procs
│   │   ├── cgroup.stat
│   │   ├── cgroup.subtree_control
│   │   ├── cgroup.threads
│   │   ├── cgroup.type
│   │   ├── cpu.pressure
│   │   ├── cpu.stat
│   │   ├── cpu.stat.local
│   │   ├── io.pressure
│   │   ├── memory.current
│   │   ├── memory.events
│   │   ├── memory.events.local
│   │   ├── memory.high
│   │   ├── memory.low
│   │   ├── memory.max
│   │   ├── memory.min
│   │   ├── memory.numa_stat
│   │   ├── memory.oom.group
│   │   ├── memory.peak
│   │   ├── memory.pressure
│   │   ├── memory.reclaim
│   │   ├── memory.stat
│   │   ├── memory.swap.current
│   │   ├── memory.swap.events
│   │   ├── memory.swap.high
│   │   ├── memory.swap.max
│   │   ├── memory.swap.peak
│   │   ├── memory.zswap.current
│   │   ├── memory.zswap.max
│   │   ├── memory.zswap.writeback
│   │   ├── pids.current
│   │   ├── pids.events
│   │   ├── pids.max
│   │   └── pids.peak
│   ├── dbus.service
│   │   ├── cgroup.controllers
│   │   ├── cgroup.events
│   │   ├── cgroup.freeze
│   │   ├── cgroup.kill
│   │   ├── cgroup.max.depth
│   │   ├── cgroup.max.descendants
│   │   ├── cgroup.pressure
│   │   ├── cgroup.procs
│   │   ├── cgroup.stat
│   │   ├── cgroup.subtree_control
│   │   ├── cgroup.threads
│   │   ├── cgroup.type
│   │   ├── cpu.pressure
│   │   ├── cpu.stat
│   │   ├── cpu.stat.local
│   │   ├── io.pressure
│   │   ├── memory.current
│   │   ├── memory.events
│   │   ├── memory.events.local
│   │   ├── memory.high
│   │   ├── memory.low
│   │   ├── memory.max
│   │   ├── memory.min
│   │   ├── memory.numa_stat
│   │   ├── memory.oom.group
│   │   ├── memory.peak
│   │   ├── memory.pressure
│   │   ├── memory.reclaim
│   │   ├── memory.stat
│   │   ├── memory.swap.current
│   │   ├── memory.swap.events
│   │   ├── memory.swap.high
│   │   ├── memory.swap.max
│   │   ├── memory.swap.peak
│   │   ├── memory.zswap.current
│   │   ├── memory.zswap.max
│   │   ├── memory.zswap.writeback
│   │   ├── pids.current
│   │   ├── pids.events
│   │   ├── pids.max
│   │   └── pids.peak
│   ├── fwupd.service
│   │   ├── cgroup.controllers
│   │   ├── cgroup.events
│   │   ├── cgroup.freeze
│   │   ├── cgroup.kill
│   │   ├── cgroup.max.depth
│   │   ├── cgroup.max.descendants
│   │   ├── cgroup.pressure
│   │   ├── cgroup.procs
│   │   ├── cgroup.stat
│   │   ├── cgroup.subtree_control
│   │   ├── cgroup.threads
│   │   ├── cgroup.type
│   │   ├── cpu.pressure
│   │   ├── cpu.stat
│   │   ├── cpu.stat.local
│   │   ├── io.pressure
│   │   ├── memory.current
│   │   ├── memory.events
│   │   ├── memory.events.local
│   │   ├── memory.high
│   │   ├── memory.low
│   │   ├── memory.max
│   │   ├── memory.min
│   │   ├── memory.numa_stat
│   │   ├── memory.oom.group
│   │   ├── memory.peak
│   │   ├── memory.pressure
│   │   ├── memory.reclaim
│   │   ├── memory.stat
│   │   ├── memory.swap.current
│   │   ├── memory.swap.events
│   │   ├── memory.swap.high
│   │   ├── memory.swap.max
│   │   ├── memory.swap.peak
│   │   ├── memory.zswap.current
│   │   ├── memory.zswap.max
│   │   ├── memory.zswap.writeback
│   │   ├── pids.current
│   │   ├── pids.events
│   │   ├── pids.max
│   │   └── pids.peak
│   ├── gdm.service
│   │   ├── cgroup.controllers
│   │   ├── cgroup.events
│   │   ├── cgroup.freeze
│   │   ├── cgroup.kill
│   │   ├── cgroup.max.depth
│   │   ├── cgroup.max.descendants
│   │   ├── cgroup.pressure
│   │   ├── cgroup.procs
│   │   ├── cgroup.stat
│   │   ├── cgroup.subtree_control
│   │   ├── cgroup.threads
│   │   ├── cgroup.type
│   │   ├── cpu.pressure
│   │   ├── cpu.stat
│   │   ├── cpu.stat.local
│   │   ├── io.pressure
│   │   ├── memory.current
│   │   ├── memory.events
│   │   ├── memory.events.local
│   │   ├── memory.high
│   │   ├── memory.low
│   │   ├── memory.max
│   │   ├── memory.min
│   │   ├── memory.numa_stat
│   │   ├── memory.oom.group
│   │   ├── memory.peak
│   │   ├── memory.pressure
│   │   ├── memory.reclaim
│   │   ├── memory.stat
│   │   ├── memory.swap.current
│   │   ├── memory.swap.events
│   │   ├── memory.swap.high
│   │   ├── memory.swap.max
│   │   ├── memory.swap.peak
│   │   ├── memory.zswap.current
│   │   ├── memory.zswap.max
│   │   ├── memory.zswap.writeback
│   │   ├── pids.current
│   │   ├── pids.events
│   │   ├── pids.max
│   │   └── pids.peak
│   ├── io.pressure
│   ├── irqbalance.service
│   │   ├── cgroup.controllers
│   │   ├── cgroup.events
│   │   ├── cgroup.freeze
│   │   ├── cgroup.kill
│   │   ├── cgroup.max.depth
│   │   ├── cgroup.max.descendants
│   │   ├── cgroup.pressure
│   │   ├── cgroup.procs
│   │   ├── cgroup.stat
│   │   ├── cgroup.subtree_control
│   │   ├── cgroup.threads
│   │   ├── cgroup.type
│   │   ├── cpu.pressure
│   │   ├── cpu.stat
│   │   ├── cpu.stat.local
│   │   ├── io.pressure
│   │   ├── memory.current
│   │   ├── memory.events
│   │   ├── memory.events.local
│   │   ├── memory.high
│   │   ├── memory.low
│   │   ├── memory.max
│   │   ├── memory.min
│   │   ├── memory.numa_stat
│   │   ├── memory.oom.group
│   │   ├── memory.peak
│   │   ├── memory.pressure
│   │   ├── memory.reclaim
│   │   ├── memory.stat
│   │   ├── memory.swap.current
│   │   ├── memory.swap.events
│   │   ├── memory.swap.high
│   │   ├── memory.swap.max
│   │   ├── memory.swap.peak
│   │   ├── memory.zswap.current
│   │   ├── memory.zswap.max
│   │   ├── memory.zswap.writeback
│   │   ├── pids.current
│   │   ├── pids.events
│   │   ├── pids.max
│   │   └── pids.peak
│   ├── kerneloops.service
│   │   ├── cgroup.controllers
│   │   ├── cgroup.events
│   │   ├── cgroup.freeze
│   │   ├── cgroup.kill
│   │   ├── cgroup.max.depth
│   │   ├── cgroup.max.descendants
│   │   ├── cgroup.pressure
│   │   ├── cgroup.procs
│   │   ├── cgroup.stat
│   │   ├── cgroup.subtree_control
│   │   ├── cgroup.threads
│   │   ├── cgroup.type
│   │   ├── cpu.pressure
│   │   ├── cpu.stat
│   │   ├── cpu.stat.local
│   │   ├── io.pressure
│   │   ├── memory.current
│   │   ├── memory.events
│   │   ├── memory.events.local
│   │   ├── memory.high
│   │   ├── memory.low
│   │   ├── memory.max
│   │   ├── memory.min
│   │   ├── memory.numa_stat
│   │   ├── memory.oom.group
│   │   ├── memory.peak
│   │   ├── memory.pressure
│   │   ├── memory.reclaim
│   │   ├── memory.stat
│   │   ├── memory.swap.current
│   │   ├── memory.swap.events
│   │   ├── memory.swap.high
│   │   ├── memory.swap.max
│   │   ├── memory.swap.peak
│   │   ├── memory.zswap.current
│   │   ├── memory.zswap.max
│   │   ├── memory.zswap.writeback
│   │   ├── pids.current
│   │   ├── pids.events
│   │   ├── pids.max
│   │   └── pids.peak
│   ├── memory.current
│   ├── memory.events
│   ├── memory.events.local
│   ├── memory.high
│   ├── memory.low
│   ├── memory.max
│   ├── memory.min
│   ├── memory.numa_stat
│   ├── memory.oom.group
│   ├── memory.peak
│   ├── memory.pressure
│   ├── memory.reclaim
│   ├── memory.stat
│   ├── memory.swap.current
│   ├── memory.swap.events
│   ├── memory.swap.high
│   ├── memory.swap.max
│   ├── memory.swap.peak
│   ├── memory.zswap.current
│   ├── memory.zswap.max
│   ├── memory.zswap.writeback
│   ├── ModemManager.service
│   │   ├── cgroup.controllers
│   │   ├── cgroup.events
│   │   ├── cgroup.freeze
│   │   ├── cgroup.kill
│   │   ├── cgroup.max.depth
│   │   ├── cgroup.max.descendants
│   │   ├── cgroup.pressure
│   │   ├── cgroup.procs
│   │   ├── cgroup.stat
│   │   ├── cgroup.subtree_control
│   │   ├── cgroup.threads
│   │   ├── cgroup.type
│   │   ├── cpu.pressure
│   │   ├── cpu.stat
│   │   ├── cpu.stat.local
│   │   ├── io.pressure
│   │   ├── memory.current
│   │   ├── memory.events
│   │   ├── memory.events.local
│   │   ├── memory.high
│   │   ├── memory.low
│   │   ├── memory.max
│   │   ├── memory.min
│   │   ├── memory.numa_stat
│   │   ├── memory.oom.group
│   │   ├── memory.peak
│   │   ├── memory.pressure
│   │   ├── memory.reclaim
│   │   ├── memory.stat
│   │   ├── memory.swap.current
│   │   ├── memory.swap.events
│   │   ├── memory.swap.high
│   │   ├── memory.swap.max
│   │   ├── memory.swap.peak
│   │   ├── memory.zswap.current
│   │   ├── memory.zswap.max
│   │   ├── memory.zswap.writeback
│   │   ├── pids.current
│   │   ├── pids.events
│   │   ├── pids.max
│   │   └── pids.peak
│   ├── networkd-dispatcher.service
│   │   ├── cgroup.controllers
│   │   ├── cgroup.events
│   │   ├── cgroup.freeze
│   │   ├── cgroup.kill
│   │   ├── cgroup.max.depth
│   │   ├── cgroup.max.descendants
│   │   ├── cgroup.pressure
│   │   ├── cgroup.procs
│   │   ├── cgroup.stat
│   │   ├── cgroup.subtree_control
│   │   ├── cgroup.threads
│   │   ├── cgroup.type
│   │   ├── cpu.pressure
│   │   ├── cpu.stat
│   │   ├── cpu.stat.local
│   │   ├── io.pressure
│   │   ├── memory.current
│   │   ├── memory.events
│   │   ├── memory.events.local
│   │   ├── memory.high
│   │   ├── memory.low
│   │   ├── memory.max
│   │   ├── memory.min
│   │   ├── memory.numa_stat
│   │   ├── memory.oom.group
│   │   ├── memory.peak
│   │   ├── memory.pressure
│   │   ├── memory.reclaim
│   │   ├── memory.stat
│   │   ├── memory.swap.current
│   │   ├── memory.swap.events
│   │   ├── memory.swap.high
│   │   ├── memory.swap.max
│   │   ├── memory.swap.peak
│   │   ├── memory.zswap.current
│   │   ├── memory.zswap.max
│   │   ├── memory.zswap.writeback
│   │   ├── pids.current
│   │   ├── pids.events
│   │   ├── pids.max
│   │   └── pids.peak
│   ├── NetworkManager.service
│   │   ├── cgroup.controllers
│   │   ├── cgroup.events
│   │   ├── cgroup.freeze
│   │   ├── cgroup.kill
│   │   ├── cgroup.max.depth
│   │   ├── cgroup.max.descendants
│   │   ├── cgroup.pressure
│   │   ├── cgroup.procs
│   │   ├── cgroup.stat
│   │   ├── cgroup.subtree_control
│   │   ├── cgroup.threads
│   │   ├── cgroup.type
│   │   ├── cpu.pressure
│   │   ├── cpu.stat
│   │   ├── cpu.stat.local
│   │   ├── io.pressure
│   │   ├── memory.current
│   │   ├── memory.events
│   │   ├── memory.events.local
│   │   ├── memory.high
│   │   ├── memory.low
│   │   ├── memory.max
│   │   ├── memory.min
│   │   ├── memory.numa_stat
│   │   ├── memory.oom.group
│   │   ├── memory.peak
│   │   ├── memory.pressure
│   │   ├── memory.reclaim
│   │   ├── memory.stat
│   │   ├── memory.swap.current
│   │   ├── memory.swap.events
│   │   ├── memory.swap.high
│   │   ├── memory.swap.max
│   │   ├── memory.swap.peak
│   │   ├── memory.zswap.current
│   │   ├── memory.zswap.max
│   │   ├── memory.zswap.writeback
│   │   ├── pids.current
│   │   ├── pids.events
│   │   ├── pids.max
│   │   └── pids.peak
│   ├── nvidia-persistenced.service
│   │   ├── cgroup.controllers
│   │   ├── cgroup.events
│   │   ├── cgroup.freeze
│   │   ├── cgroup.kill
│   │   ├── cgroup.max.depth
│   │   ├── cgroup.max.descendants
│   │   ├── cgroup.pressure
│   │   ├── cgroup.procs
│   │   ├── cgroup.stat
│   │   ├── cgroup.subtree_control
│   │   ├── cgroup.threads
│   │   ├── cgroup.type
│   │   ├── cpu.pressure
│   │   ├── cpu.stat
│   │   ├── cpu.stat.local
│   │   ├── io.pressure
│   │   ├── memory.current
│   │   ├── memory.events
│   │   ├── memory.events.local
│   │   ├── memory.high
│   │   ├── memory.low
│   │   ├── memory.max
│   │   ├── memory.min
│   │   ├── memory.numa_stat
│   │   ├── memory.oom.group
│   │   ├── memory.peak
│   │   ├── memory.pressure
│   │   ├── memory.reclaim
│   │   ├── memory.stat
│   │   ├── memory.swap.current
│   │   ├── memory.swap.events
│   │   ├── memory.swap.high
│   │   ├── memory.swap.max
│   │   ├── memory.swap.peak
│   │   ├── memory.zswap.current
│   │   ├── memory.zswap.max
│   │   ├── memory.zswap.writeback
│   │   ├── pids.current
│   │   ├── pids.events
│   │   ├── pids.max
│   │   └── pids.peak
│   ├── packagekit.service
│   │   ├── cgroup.controllers
│   │   ├── cgroup.events
│   │   ├── cgroup.freeze
│   │   ├── cgroup.kill
│   │   ├── cgroup.max.depth
│   │   ├── cgroup.max.descendants
│   │   ├── cgroup.pressure
│   │   ├── cgroup.procs
│   │   ├── cgroup.stat
│   │   ├── cgroup.subtree_control
│   │   ├── cgroup.threads
│   │   ├── cgroup.type
│   │   ├── cpu.pressure
│   │   ├── cpu.stat
│   │   ├── cpu.stat.local
│   │   ├── io.pressure
│   │   ├── memory.current
│   │   ├── memory.events
│   │   ├── memory.events.local
│   │   ├── memory.high
│   │   ├── memory.low
│   │   ├── memory.max
│   │   ├── memory.min
│   │   ├── memory.numa_stat
│   │   ├── memory.oom.group
│   │   ├── memory.peak
│   │   ├── memory.pressure
│   │   ├── memory.reclaim
│   │   ├── memory.stat
│   │   ├── memory.swap.current
│   │   ├── memory.swap.events
│   │   ├── memory.swap.high
│   │   ├── memory.swap.max
│   │   ├── memory.swap.peak
│   │   ├── memory.zswap.current
│   │   ├── memory.zswap.max
│   │   ├── memory.zswap.writeback
│   │   ├── pids.current
│   │   ├── pids.events
│   │   ├── pids.max
│   │   └── pids.peak
│   ├── pids.current
│   ├── pids.events
│   ├── pids.max
│   ├── pids.peak
│   ├── polkit.service
│   │   ├── cgroup.controllers
│   │   ├── cgroup.events
│   │   ├── cgroup.freeze
│   │   ├── cgroup.kill
│   │   ├── cgroup.max.depth
│   │   ├── cgroup.max.descendants
│   │   ├── cgroup.pressure
│   │   ├── cgroup.procs
│   │   ├── cgroup.stat
│   │   ├── cgroup.subtree_control
│   │   ├── cgroup.threads
│   │   ├── cgroup.type
│   │   ├── cpu.pressure
│   │   ├── cpu.stat
│   │   ├── cpu.stat.local
│   │   ├── io.pressure
│   │   ├── memory.current
│   │   ├── memory.events
│   │   ├── memory.events.local
│   │   ├── memory.high
│   │   ├── memory.low
│   │   ├── memory.max
│   │   ├── memory.min
│   │   ├── memory.numa_stat
│   │   ├── memory.oom.group
│   │   ├── memory.peak
│   │   ├── memory.pressure
│   │   ├── memory.reclaim
│   │   ├── memory.stat
│   │   ├── memory.swap.current
│   │   ├── memory.swap.events
│   │   ├── memory.swap.high
│   │   ├── memory.swap.max
│   │   ├── memory.swap.peak
│   │   ├── memory.zswap.current
│   │   ├── memory.zswap.max
│   │   ├── memory.zswap.writeback
│   │   ├── pids.current
│   │   ├── pids.events
│   │   ├── pids.max
│   │   └── pids.peak
│   ├── power-profiles-daemon.service
│   │   ├── cgroup.controllers
│   │   ├── cgroup.events
│   │   ├── cgroup.freeze
│   │   ├── cgroup.kill
│   │   ├── cgroup.max.depth
│   │   ├── cgroup.max.descendants
│   │   ├── cgroup.pressure
│   │   ├── cgroup.procs
│   │   ├── cgroup.stat
│   │   ├── cgroup.subtree_control
│   │   ├── cgroup.threads
│   │   ├── cgroup.type
│   │   ├── cpu.pressure
│   │   ├── cpu.stat
│   │   ├── cpu.stat.local
│   │   ├── io.pressure
│   │   ├── memory.current
│   │   ├── memory.events
│   │   ├── memory.events.local
│   │   ├── memory.high
│   │   ├── memory.low
│   │   ├── memory.max
│   │   ├── memory.min
│   │   ├── memory.numa_stat
│   │   ├── memory.oom.group
│   │   ├── memory.peak
│   │   ├── memory.pressure
│   │   ├── memory.reclaim
│   │   ├── memory.stat
│   │   ├── memory.swap.current
│   │   ├── memory.swap.events
│   │   ├── memory.swap.high
│   │   ├── memory.swap.max
│   │   ├── memory.swap.peak
│   │   ├── memory.zswap.current
│   │   ├── memory.zswap.max
│   │   ├── memory.zswap.writeback
│   │   ├── pids.current
│   │   ├── pids.events
│   │   ├── pids.max
│   │   └── pids.peak
│   ├── rsyslog.service
│   │   ├── cgroup.controllers
│   │   ├── cgroup.events
│   │   ├── cgroup.freeze
│   │   ├── cgroup.kill
│   │   ├── cgroup.max.depth
│   │   ├── cgroup.max.descendants
│   │   ├── cgroup.pressure
│   │   ├── cgroup.procs
│   │   ├── cgroup.stat
│   │   ├── cgroup.subtree_control
│   │   ├── cgroup.threads
│   │   ├── cgroup.type
│   │   ├── cpu.pressure
│   │   ├── cpu.stat
│   │   ├── cpu.stat.local
│   │   ├── io.pressure
│   │   ├── memory.current
│   │   ├── memory.events
│   │   ├── memory.events.local
│   │   ├── memory.high
│   │   ├── memory.low
│   │   ├── memory.max
│   │   ├── memory.min
│   │   ├── memory.numa_stat
│   │   ├── memory.oom.group
│   │   ├── memory.peak
│   │   ├── memory.pressure
│   │   ├── memory.reclaim
│   │   ├── memory.stat
│   │   ├── memory.swap.current
│   │   ├── memory.swap.events
│   │   ├── memory.swap.high
│   │   ├── memory.swap.max
│   │   ├── memory.swap.peak
│   │   ├── memory.zswap.current
│   │   ├── memory.zswap.max
│   │   ├── memory.zswap.writeback
│   │   ├── pids.current
│   │   ├── pids.events
│   │   ├── pids.max
│   │   └── pids.peak
│   ├── rtkit-daemon.service
│   │   ├── cgroup.controllers
│   │   ├── cgroup.events
│   │   ├── cgroup.freeze
│   │   ├── cgroup.kill
│   │   ├── cgroup.max.depth
│   │   ├── cgroup.max.descendants
│   │   ├── cgroup.pressure
│   │   ├── cgroup.procs
│   │   ├── cgroup.stat
│   │   ├── cgroup.subtree_control
│   │   ├── cgroup.threads
│   │   ├── cgroup.type
│   │   ├── cpu.pressure
│   │   ├── cpu.stat
│   │   ├── cpu.stat.local
│   │   ├── io.pressure
│   │   ├── memory.current
│   │   ├── memory.events
│   │   ├── memory.events.local
│   │   ├── memory.high
│   │   ├── memory.low
│   │   ├── memory.max
│   │   ├── memory.min
│   │   ├── memory.numa_stat
│   │   ├── memory.oom.group
│   │   ├── memory.peak
│   │   ├── memory.pressure
│   │   ├── memory.reclaim
│   │   ├── memory.stat
│   │   ├── memory.swap.current
│   │   ├── memory.swap.events
│   │   ├── memory.swap.high
│   │   ├── memory.swap.max
│   │   ├── memory.swap.peak
│   │   ├── memory.zswap.current
│   │   ├── memory.zswap.max
│   │   ├── memory.zswap.writeback
│   │   ├── pids.current
│   │   ├── pids.events
│   │   ├── pids.max
│   │   └── pids.peak
│   ├── run-qemu.mount
│   │   ├── cgroup.controllers
│   │   ├── cgroup.events
│   │   ├── cgroup.freeze
│   │   ├── cgroup.kill
│   │   ├── cgroup.max.depth
│   │   ├── cgroup.max.descendants
│   │   ├── cgroup.pressure
│   │   ├── cgroup.procs
│   │   ├── cgroup.stat
│   │   ├── cgroup.subtree_control
│   │   ├── cgroup.threads
│   │   ├── cgroup.type
│   │   ├── cpu.pressure
│   │   ├── cpu.stat
│   │   ├── cpu.stat.local
│   │   ├── io.pressure
│   │   ├── memory.current
│   │   ├── memory.events
│   │   ├── memory.events.local
│   │   ├── memory.high
│   │   ├── memory.low
│   │   ├── memory.max
│   │   ├── memory.min
│   │   ├── memory.numa_stat
│   │   ├── memory.oom.group
│   │   ├── memory.peak
│   │   ├── memory.pressure
│   │   ├── memory.reclaim
│   │   ├── memory.stat
│   │   ├── memory.swap.current
│   │   ├── memory.swap.events
│   │   ├── memory.swap.high
│   │   ├── memory.swap.max
│   │   ├── memory.swap.peak
│   │   ├── memory.zswap.current
│   │   ├── memory.zswap.max
│   │   ├── memory.zswap.writeback
│   │   ├── pids.current
│   │   ├── pids.events
│   │   ├── pids.max
│   │   └── pids.peak
│   ├── snap-bare-5.mount
│   │   ├── cgroup.controllers
│   │   ├── cgroup.events
│   │   ├── cgroup.freeze
│   │   ├── cgroup.kill
│   │   ├── cgroup.max.depth
│   │   ├── cgroup.max.descendants
│   │   ├── cgroup.pressure
│   │   ├── cgroup.procs
│   │   ├── cgroup.stat
│   │   ├── cgroup.subtree_control
│   │   ├── cgroup.threads
│   │   ├── cgroup.type
│   │   ├── cpu.pressure
│   │   ├── cpu.stat
│   │   ├── cpu.stat.local
│   │   ├── io.pressure
│   │   ├── memory.current
│   │   ├── memory.events
│   │   ├── memory.events.local
│   │   ├── memory.high
│   │   ├── memory.low
│   │   ├── memory.max
│   │   ├── memory.min
│   │   ├── memory.numa_stat
│   │   ├── memory.oom.group
│   │   ├── memory.peak
│   │   ├── memory.pressure
│   │   ├── memory.reclaim
│   │   ├── memory.stat
│   │   ├── memory.swap.current
│   │   ├── memory.swap.events
│   │   ├── memory.swap.high
│   │   ├── memory.swap.max
│   │   ├── memory.swap.peak
│   │   ├── memory.zswap.current
│   │   ├── memory.zswap.max
│   │   ├── memory.zswap.writeback
│   │   ├── pids.current
│   │   ├── pids.events
│   │   ├── pids.max
│   │   └── pids.peak
│   ├── snap-core22-2163.mount
│   │   ├── cgroup.controllers
│   │   ├── cgroup.events
│   │   ├── cgroup.freeze
│   │   ├── cgroup.kill
│   │   ├── cgroup.max.depth
│   │   ├── cgroup.max.descendants
│   │   ├── cgroup.pressure
│   │   ├── cgroup.procs
│   │   ├── cgroup.stat
│   │   ├── cgroup.subtree_control
│   │   ├── cgroup.threads
│   │   ├── cgroup.type
│   │   ├── cpu.pressure
│   │   ├── cpu.stat
│   │   ├── cpu.stat.local
│   │   ├── io.pressure
│   │   ├── memory.current
│   │   ├── memory.events
│   │   ├── memory.events.local
│   │   ├── memory.high
│   │   ├── memory.low
│   │   ├── memory.max
│   │   ├── memory.min
│   │   ├── memory.numa_stat
│   │   ├── memory.oom.group
│   │   ├── memory.peak
│   │   ├── memory.pressure
│   │   ├── memory.reclaim
│   │   ├── memory.stat
│   │   ├── memory.swap.current
│   │   ├── memory.swap.events
│   │   ├── memory.swap.high
│   │   ├── memory.swap.max
│   │   ├── memory.swap.peak
│   │   ├── memory.zswap.current
│   │   ├── memory.zswap.max
│   │   ├── memory.zswap.writeback
│   │   ├── pids.current
│   │   ├── pids.events
│   │   ├── pids.max
│   │   └── pids.peak
│   ├── snap-core22-2193.mount
│   │   ├── cgroup.controllers
│   │   ├── cgroup.events
│   │   ├── cgroup.freeze
│   │   ├── cgroup.kill
│   │   ├── cgroup.max.depth
│   │   ├── cgroup.max.descendants
│   │   ├── cgroup.pressure
│   │   ├── cgroup.procs
│   │   ├── cgroup.stat
│   │   ├── cgroup.subtree_control
│   │   ├── cgroup.threads
│   │   ├── cgroup.type
│   │   ├── cpu.pressure
│   │   ├── cpu.stat
│   │   ├── cpu.stat.local
│   │   ├── io.pressure
│   │   ├── memory.current
│   │   ├── memory.events
│   │   ├── memory.events.local
│   │   ├── memory.high
│   │   ├── memory.low
│   │   ├── memory.max
│   │   ├── memory.min
│   │   ├── memory.numa_stat
│   │   ├── memory.oom.group
│   │   ├── memory.peak
│   │   ├── memory.pressure
│   │   ├── memory.reclaim
│   │   ├── memory.stat
│   │   ├── memory.swap.current
│   │   ├── memory.swap.events
│   │   ├── memory.swap.high
│   │   ├── memory.swap.max
│   │   ├── memory.swap.peak
│   │   ├── memory.zswap.current
│   │   ├── memory.zswap.max
│   │   ├── memory.zswap.writeback
│   │   ├── pids.current
│   │   ├── pids.events
│   │   ├── pids.max
│   │   └── pids.peak
│   ├── snap-core24-1237.mount
│   │   ├── cgroup.controllers
│   │   ├── cgroup.events
│   │   ├── cgroup.freeze
│   │   ├── cgroup.kill
│   │   ├── cgroup.max.depth
│   │   ├── cgroup.max.descendants
│   │   ├── cgroup.pressure
│   │   ├── cgroup.procs
│   │   ├── cgroup.stat
│   │   ├── cgroup.subtree_control
│   │   ├── cgroup.threads
│   │   ├── cgroup.type
│   │   ├── cpu.pressure
│   │   ├── cpu.stat
│   │   ├── cpu.stat.local
│   │   ├── io.pressure
│   │   ├── memory.current
│   │   ├── memory.events
│   │   ├── memory.events.local
│   │   ├── memory.high
│   │   ├── memory.low
│   │   ├── memory.max
│   │   ├── memory.min
│   │   ├── memory.numa_stat
│   │   ├── memory.oom.group
│   │   ├── memory.peak
│   │   ├── memory.pressure
│   │   ├── memory.reclaim
│   │   ├── memory.stat
│   │   ├── memory.swap.current
│   │   ├── memory.swap.events
│   │   ├── memory.swap.high
│   │   ├── memory.swap.max
│   │   ├── memory.swap.peak
│   │   ├── memory.zswap.current
│   │   ├── memory.zswap.max
│   │   ├── memory.zswap.writeback
│   │   ├── pids.current
│   │   ├── pids.events
│   │   ├── pids.max
│   │   └── pids.peak
│   ├── snap-core24-1243.mount
│   │   ├── cgroup.controllers
│   │   ├── cgroup.events
│   │   ├── cgroup.freeze
│   │   ├── cgroup.kill
│   │   ├── cgroup.max.depth
│   │   ├── cgroup.max.descendants
│   │   ├── cgroup.pressure
│   │   ├── cgroup.procs
│   │   ├── cgroup.stat
│   │   ├── cgroup.subtree_control
│   │   ├── cgroup.threads
│   │   ├── cgroup.type
│   │   ├── cpu.pressure
│   │   ├── cpu.stat
│   │   ├── cpu.stat.local
│   │   ├── io.pressure
│   │   ├── memory.current
│   │   ├── memory.events
│   │   ├── memory.events.local
│   │   ├── memory.high
│   │   ├── memory.low
│   │   ├── memory.max
│   │   ├── memory.min
│   │   ├── memory.numa_stat
│   │   ├── memory.oom.group
│   │   ├── memory.peak
│   │   ├── memory.pressure
│   │   ├── memory.reclaim
│   │   ├── memory.stat
│   │   ├── memory.swap.current
│   │   ├── memory.swap.events
│   │   ├── memory.swap.high
│   │   ├── memory.swap.max
│   │   ├── memory.swap.peak
│   │   ├── memory.zswap.current
│   │   ├── memory.zswap.max
│   │   ├── memory.zswap.writeback
│   │   ├── pids.current
│   │   ├── pids.events
│   │   ├── pids.max
│   │   └── pids.peak
│   ├── snapd.service
│   │   ├── cgroup.controllers
│   │   ├── cgroup.events
│   │   ├── cgroup.freeze
│   │   ├── cgroup.kill
│   │   ├── cgroup.max.depth
│   │   ├── cgroup.max.descendants
│   │   ├── cgroup.pressure
│   │   ├── cgroup.procs
│   │   ├── cgroup.stat
│   │   ├── cgroup.subtree_control
│   │   ├── cgroup.threads
│   │   ├── cgroup.type
│   │   ├── cpu.pressure
│   │   ├── cpu.stat
│   │   ├── cpu.stat.local
│   │   ├── io.pressure
│   │   ├── memory.current
│   │   ├── memory.events
│   │   ├── memory.events.local
│   │   ├── memory.high
│   │   ├── memory.low
│   │   ├── memory.max
│   │   ├── memory.min
│   │   ├── memory.numa_stat
│   │   ├── memory.oom.group
│   │   ├── memory.peak
│   │   ├── memory.pressure
│   │   ├── memory.reclaim
│   │   ├── memory.stat
│   │   ├── memory.swap.current
│   │   ├── memory.swap.events
│   │   ├── memory.swap.high
│   │   ├── memory.swap.max
│   │   ├── memory.swap.peak
│   │   ├── memory.zswap.current
│   │   ├── memory.zswap.max
│   │   ├── memory.zswap.writeback
│   │   ├── pids.current
│   │   ├── pids.events
│   │   ├── pids.max
│   │   └── pids.peak
│   ├── snapd.socket
│   │   ├── cgroup.controllers
│   │   ├── cgroup.events
│   │   ├── cgroup.freeze
│   │   ├── cgroup.kill
│   │   ├── cgroup.max.depth
│   │   ├── cgroup.max.descendants
│   │   ├── cgroup.pressure
│   │   ├── cgroup.procs
│   │   ├── cgroup.stat
│   │   ├── cgroup.subtree_control
│   │   ├── cgroup.threads
│   │   ├── cgroup.type
│   │   ├── cpu.pressure
│   │   ├── cpu.stat
│   │   ├── cpu.stat.local
│   │   ├── io.pressure
│   │   ├── memory.current
│   │   ├── memory.events
│   │   ├── memory.events.local
│   │   ├── memory.high
│   │   ├── memory.low
│   │   ├── memory.max
│   │   ├── memory.min
│   │   ├── memory.numa_stat
│   │   ├── memory.oom.group
│   │   ├── memory.peak
│   │   ├── memory.pressure
│   │   ├── memory.reclaim
│   │   ├── memory.stat
│   │   ├── memory.swap.current
│   │   ├── memory.swap.events
│   │   ├── memory.swap.high
│   │   ├── memory.swap.max
│   │   ├── memory.swap.peak
│   │   ├── memory.zswap.current
│   │   ├── memory.zswap.max
│   │   ├── memory.zswap.writeback
│   │   ├── pids.current
│   │   ├── pids.events
│   │   ├── pids.max
│   │   └── pids.peak
│   ├── snap-firefox-7477.mount
│   │   ├── cgroup.controllers
│   │   ├── cgroup.events
│   │   ├── cgroup.freeze
│   │   ├── cgroup.kill
│   │   ├── cgroup.max.depth
│   │   ├── cgroup.max.descendants
│   │   ├── cgroup.pressure
│   │   ├── cgroup.procs
│   │   ├── cgroup.stat
│   │   ├── cgroup.subtree_control
│   │   ├── cgroup.threads
│   │   ├── cgroup.type
│   │   ├── cpu.pressure
│   │   ├── cpu.stat
│   │   ├── cpu.stat.local
│   │   ├── io.pressure
│   │   ├── memory.current
│   │   ├── memory.events
│   │   ├── memory.events.local
│   │   ├── memory.high
│   │   ├── memory.low
│   │   ├── memory.max
│   │   ├── memory.min
│   │   ├── memory.numa_stat
│   │   ├── memory.oom.group
│   │   ├── memory.peak
│   │   ├── memory.pressure
│   │   ├── memory.reclaim
│   │   ├── memory.stat
│   │   ├── memory.swap.current
│   │   ├── memory.swap.events
│   │   ├── memory.swap.high
│   │   ├── memory.swap.max
│   │   ├── memory.swap.peak
│   │   ├── memory.zswap.current
│   │   ├── memory.zswap.max
│   │   ├── memory.zswap.writeback
│   │   ├── pids.current
│   │   ├── pids.events
│   │   ├── pids.max
│   │   └── pids.peak
│   ├── snap-firefox-7559.mount
│   │   ├── cgroup.controllers
│   │   ├── cgroup.events
│   │   ├── cgroup.freeze
│   │   ├── cgroup.kill
│   │   ├── cgroup.max.depth
│   │   ├── cgroup.max.descendants
│   │   ├── cgroup.pressure
│   │   ├── cgroup.procs
│   │   ├── cgroup.stat
│   │   ├── cgroup.subtree_control
│   │   ├── cgroup.threads
│   │   ├── cgroup.type
│   │   ├── cpu.pressure
│   │   ├── cpu.stat
│   │   ├── cpu.stat.local
│   │   ├── io.pressure
│   │   ├── memory.current
│   │   ├── memory.events
│   │   ├── memory.events.local
│   │   ├── memory.high
│   │   ├── memory.low
│   │   ├── memory.max
│   │   ├── memory.min
│   │   ├── memory.numa_stat
│   │   ├── memory.oom.group
│   │   ├── memory.peak
│   │   ├── memory.pressure
│   │   ├── memory.reclaim
│   │   ├── memory.stat
│   │   ├── memory.swap.current
│   │   ├── memory.swap.events
│   │   ├── memory.swap.high
│   │   ├── memory.swap.max
│   │   ├── memory.swap.peak
│   │   ├── memory.zswap.current
│   │   ├── memory.zswap.max
│   │   ├── memory.zswap.writeback
│   │   ├── pids.current
│   │   ├── pids.events
│   │   ├── pids.max
│   │   └── pids.peak
│   ├── snap-foliate-1943.mount
│   │   ├── cgroup.controllers
│   │   ├── cgroup.events
│   │   ├── cgroup.freeze
│   │   ├── cgroup.kill
│   │   ├── cgroup.max.depth
│   │   ├── cgroup.max.descendants
│   │   ├── cgroup.pressure
│   │   ├── cgroup.procs
│   │   ├── cgroup.stat
│   │   ├── cgroup.subtree_control
│   │   ├── cgroup.threads
│   │   ├── cgroup.type
│   │   ├── cpu.pressure
│   │   ├── cpu.stat
│   │   ├── cpu.stat.local
│   │   ├── io.pressure
│   │   ├── memory.current
│   │   ├── memory.events
│   │   ├── memory.events.local
│   │   ├── memory.high
│   │   ├── memory.low
│   │   ├── memory.max
│   │   ├── memory.min
│   │   ├── memory.numa_stat
│   │   ├── memory.oom.group
│   │   ├── memory.peak
│   │   ├── memory.pressure
│   │   ├── memory.reclaim
│   │   ├── memory.stat
│   │   ├── memory.swap.current
│   │   ├── memory.swap.events
│   │   ├── memory.swap.high
│   │   ├── memory.swap.max
│   │   ├── memory.swap.peak
│   │   ├── memory.zswap.current
│   │   ├── memory.zswap.max
│   │   ├── memory.zswap.writeback
│   │   ├── pids.current
│   │   ├── pids.events
│   │   ├── pids.max
│   │   └── pids.peak
│   ├── snap-gaming\x2dgraphics\x2dcore22-184.mount
│   │   ├── cgroup.controllers
│   │   ├── cgroup.events
│   │   ├── cgroup.freeze
│   │   ├── cgroup.kill
│   │   ├── cgroup.max.depth
│   │   ├── cgroup.max.descendants
│   │   ├── cgroup.pressure
│   │   ├── cgroup.procs
│   │   ├── cgroup.stat
│   │   ├── cgroup.subtree_control
│   │   ├── cgroup.threads
│   │   ├── cgroup.type
│   │   ├── cpu.pressure
│   │   ├── cpu.stat
│   │   ├── cpu.stat.local
│   │   ├── io.pressure
│   │   ├── memory.current
│   │   ├── memory.events
│   │   ├── memory.events.local
│   │   ├── memory.high
│   │   ├── memory.low
│   │   ├── memory.max
│   │   ├── memory.min
│   │   ├── memory.numa_stat
│   │   ├── memory.oom.group
│   │   ├── memory.peak
│   │   ├── memory.pressure
│   │   ├── memory.reclaim
│   │   ├── memory.stat
│   │   ├── memory.swap.current
│   │   ├── memory.swap.events
│   │   ├── memory.swap.high
│   │   ├── memory.swap.max
│   │   ├── memory.swap.peak
│   │   ├── memory.zswap.current
│   │   ├── memory.zswap.max
│   │   ├── memory.zswap.writeback
│   │   ├── pids.current
│   │   ├── pids.events
│   │   ├── pids.max
│   │   └── pids.peak
│   ├── snap-gnome\x2d42\x2d2204-202.mount
│   │   ├── cgroup.controllers
│   │   ├── cgroup.events
│   │   ├── cgroup.freeze
│   │   ├── cgroup.kill
│   │   ├── cgroup.max.depth
│   │   ├── cgroup.max.descendants
│   │   ├── cgroup.pressure
│   │   ├── cgroup.procs
│   │   ├── cgroup.stat
│   │   ├── cgroup.subtree_control
│   │   ├── cgroup.threads
│   │   ├── cgroup.type
│   │   ├── cpu.pressure
│   │   ├── cpu.stat
│   │   ├── cpu.stat.local
│   │   ├── io.pressure
│   │   ├── memory.current
│   │   ├── memory.events
│   │   ├── memory.events.local
│   │   ├── memory.high
│   │   ├── memory.low
│   │   ├── memory.max
│   │   ├── memory.min
│   │   ├── memory.numa_stat
│   │   ├── memory.oom.group
│   │   ├── memory.peak
│   │   ├── memory.pressure
│   │   ├── memory.reclaim
│   │   ├── memory.stat
│   │   ├── memory.swap.current
│   │   ├── memory.swap.events
│   │   ├── memory.swap.high
│   │   ├── memory.swap.max
│   │   ├── memory.swap.peak
│   │   ├── memory.zswap.current
│   │   ├── memory.zswap.max
│   │   ├── memory.zswap.writeback
│   │   ├── pids.current
│   │   ├── pids.events
│   │   ├── pids.max
│   │   └── pids.peak
│   ├── snap-gnome\x2d42\x2d2204-226.mount
│   │   ├── cgroup.controllers
│   │   ├── cgroup.events
│   │   ├── cgroup.freeze
│   │   ├── cgroup.kill
│   │   ├── cgroup.max.depth
│   │   ├── cgroup.max.descendants
│   │   ├── cgroup.pressure
│   │   ├── cgroup.procs
│   │   ├── cgroup.stat
│   │   ├── cgroup.subtree_control
│   │   ├── cgroup.threads
│   │   ├── cgroup.type
│   │   ├── cpu.pressure
│   │   ├── cpu.stat
│   │   ├── cpu.stat.local
│   │   ├── io.pressure
│   │   ├── memory.current
│   │   ├── memory.events
│   │   ├── memory.events.local
│   │   ├── memory.high
│   │   ├── memory.low
│   │   ├── memory.max
│   │   ├── memory.min
│   │   ├── memory.numa_stat
│   │   ├── memory.oom.group
│   │   ├── memory.peak
│   │   ├── memory.pressure
│   │   ├── memory.reclaim
│   │   ├── memory.stat
│   │   ├── memory.swap.current
│   │   ├── memory.swap.events
│   │   ├── memory.swap.high
│   │   ├── memory.swap.max
│   │   ├── memory.swap.peak
│   │   ├── memory.zswap.current
│   │   ├── memory.zswap.max
│   │   ├── memory.zswap.writeback
│   │   ├── pids.current
│   │   ├── pids.events
│   │   ├── pids.max
│   │   └── pids.peak
│   ├── snap-gnome\x2d46\x2d2404-125.mount
│   │   ├── cgroup.controllers
│   │   ├── cgroup.events
│   │   ├── cgroup.freeze
│   │   ├── cgroup.kill
│   │   ├── cgroup.max.depth
│   │   ├── cgroup.max.descendants
│   │   ├── cgroup.pressure
│   │   ├── cgroup.procs
│   │   ├── cgroup.stat
│   │   ├── cgroup.subtree_control
│   │   ├── cgroup.threads
│   │   ├── cgroup.type
│   │   ├── cpu.pressure
│   │   ├── cpu.stat
│   │   ├── cpu.stat.local
│   │   ├── io.pressure
│   │   ├── memory.current
│   │   ├── memory.events
│   │   ├── memory.events.local
│   │   ├── memory.high
│   │   ├── memory.low
│   │   ├── memory.max
│   │   ├── memory.min
│   │   ├── memory.numa_stat
│   │   ├── memory.oom.group
│   │   ├── memory.peak
│   │   ├── memory.pressure
│   │   ├── memory.reclaim
│   │   ├── memory.stat
│   │   ├── memory.swap.current
│   │   ├── memory.swap.events
│   │   ├── memory.swap.high
│   │   ├── memory.swap.max
│   │   ├── memory.swap.peak
│   │   ├── memory.zswap.current
│   │   ├── memory.zswap.max
│   │   ├── memory.zswap.writeback
│   │   ├── pids.current
│   │   ├── pids.events
│   │   ├── pids.max
│   │   └── pids.peak
│   ├── snap-gnome\x2d46\x2d2404-145.mount
│   │   ├── cgroup.controllers
│   │   ├── cgroup.events
│   │   ├── cgroup.freeze
│   │   ├── cgroup.kill
│   │   ├── cgroup.max.depth
│   │   ├── cgroup.max.descendants
│   │   ├── cgroup.pressure
│   │   ├── cgroup.procs
│   │   ├── cgroup.stat
│   │   ├── cgroup.subtree_control
│   │   ├── cgroup.threads
│   │   ├── cgroup.type
│   │   ├── cpu.pressure
│   │   ├── cpu.stat
│   │   ├── cpu.stat.local
│   │   ├── io.pressure
│   │   ├── memory.current
│   │   ├── memory.events
│   │   ├── memory.events.local
│   │   ├── memory.high
│   │   ├── memory.low
│   │   ├── memory.max
│   │   ├── memory.min
│   │   ├── memory.numa_stat
│   │   ├── memory.oom.group
│   │   ├── memory.peak
│   │   ├── memory.pressure
│   │   ├── memory.reclaim
│   │   ├── memory.stat
│   │   ├── memory.swap.current
│   │   ├── memory.swap.events
│   │   ├── memory.swap.high
│   │   ├── memory.swap.max
│   │   ├── memory.swap.peak
│   │   ├── memory.zswap.current
│   │   ├── memory.zswap.max
│   │   ├── memory.zswap.writeback
│   │   ├── pids.current
│   │   ├── pids.events
│   │   ├── pids.max
│   │   └── pids.peak
│   ├── snap-gtk\x2dcommon\x2dthemes-1535.mount
│   │   ├── cgroup.controllers
│   │   ├── cgroup.events
│   │   ├── cgroup.freeze
│   │   ├── cgroup.kill
│   │   ├── cgroup.max.depth
│   │   ├── cgroup.max.descendants
│   │   ├── cgroup.pressure
│   │   ├── cgroup.procs
│   │   ├── cgroup.stat
│   │   ├── cgroup.subtree_control
│   │   ├── cgroup.threads
│   │   ├── cgroup.type
│   │   ├── cpu.pressure
│   │   ├── cpu.stat
│   │   ├── cpu.stat.local
│   │   ├── io.pressure
│   │   ├── memory.current
│   │   ├── memory.events
│   │   ├── memory.events.local
│   │   ├── memory.high
│   │   ├── memory.low
│   │   ├── memory.max
│   │   ├── memory.min
│   │   ├── memory.numa_stat
│   │   ├── memory.oom.group
│   │   ├── memory.peak
│   │   ├── memory.pressure
│   │   ├── memory.reclaim
│   │   ├── memory.stat
│   │   ├── memory.swap.current
│   │   ├── memory.swap.events
│   │   ├── memory.swap.high
│   │   ├── memory.swap.max
│   │   ├── memory.swap.peak
│   │   ├── memory.zswap.current
│   │   ├── memory.zswap.max
│   │   ├── memory.zswap.writeback
│   │   ├── pids.current
│   │   ├── pids.events
│   │   ├── pids.max
│   │   └── pids.peak
│   ├── snap-mesa\x2d2404-1165.mount
│   │   ├── cgroup.controllers
│   │   ├── cgroup.events
│   │   ├── cgroup.freeze
│   │   ├── cgroup.kill
│   │   ├── cgroup.max.depth
│   │   ├── cgroup.max.descendants
│   │   ├── cgroup.pressure
│   │   ├── cgroup.procs
│   │   ├── cgroup.stat
│   │   ├── cgroup.subtree_control
│   │   ├── cgroup.threads
│   │   ├── cgroup.type
│   │   ├── cpu.pressure
│   │   ├── cpu.stat
│   │   ├── cpu.stat.local
│   │   ├── io.pressure
│   │   ├── memory.current
│   │   ├── memory.events
│   │   ├── memory.events.local
│   │   ├── memory.high
│   │   ├── memory.low
│   │   ├── memory.max
│   │   ├── memory.min
│   │   ├── memory.numa_stat
│   │   ├── memory.oom.group
│   │   ├── memory.peak
│   │   ├── memory.pressure
│   │   ├── memory.reclaim
│   │   ├── memory.stat
│   │   ├── memory.swap.current
│   │   ├── memory.swap.events
│   │   ├── memory.swap.high
│   │   ├── memory.swap.max
│   │   ├── memory.swap.peak
│   │   ├── memory.zswap.current
│   │   ├── memory.zswap.max
│   │   ├── memory.zswap.writeback
│   │   ├── pids.current
│   │   ├── pids.events
│   │   ├── pids.max
│   │   └── pids.peak
│   ├── snap-mesa\x2d2404-912.mount
│   │   ├── cgroup.controllers
│   │   ├── cgroup.events
│   │   ├── cgroup.freeze
│   │   ├── cgroup.kill
│   │   ├── cgroup.max.depth
│   │   ├── cgroup.max.descendants
│   │   ├── cgroup.pressure
│   │   ├── cgroup.procs
│   │   ├── cgroup.stat
│   │   ├── cgroup.subtree_control
│   │   ├── cgroup.threads
│   │   ├── cgroup.type
│   │   ├── cpu.pressure
│   │   ├── cpu.stat
│   │   ├── cpu.stat.local
│   │   ├── io.pressure
│   │   ├── memory.current
│   │   ├── memory.events
│   │   ├── memory.events.local
│   │   ├── memory.high
│   │   ├── memory.low
│   │   ├── memory.max
│   │   ├── memory.min
│   │   ├── memory.numa_stat
│   │   ├── memory.oom.group
│   │   ├── memory.peak
│   │   ├── memory.pressure
│   │   ├── memory.reclaim
│   │   ├── memory.stat
│   │   ├── memory.swap.current
│   │   ├── memory.swap.events
│   │   ├── memory.swap.high
│   │   ├── memory.swap.max
│   │   ├── memory.swap.peak
│   │   ├── memory.zswap.current
│   │   ├── memory.zswap.max
│   │   ├── memory.zswap.writeback
│   │   ├── pids.current
│   │   ├── pids.events
│   │   ├── pids.max
│   │   └── pids.peak
│   ├── snap-snapd-25577.mount
│   │   ├── cgroup.controllers
│   │   ├── cgroup.events
│   │   ├── cgroup.freeze
│   │   ├── cgroup.kill
│   │   ├── cgroup.max.depth
│   │   ├── cgroup.max.descendants
│   │   ├── cgroup.pressure
│   │   ├── cgroup.procs
│   │   ├── cgroup.stat
│   │   ├── cgroup.subtree_control
│   │   ├── cgroup.threads
│   │   ├── cgroup.type
│   │   ├── cpu.pressure
│   │   ├── cpu.stat
│   │   ├── cpu.stat.local
│   │   ├── io.pressure
│   │   ├── memory.current
│   │   ├── memory.events
│   │   ├── memory.events.local
│   │   ├── memory.high
│   │   ├── memory.low
│   │   ├── memory.max
│   │   ├── memory.min
│   │   ├── memory.numa_stat
│   │   ├── memory.oom.group
│   │   ├── memory.peak
│   │   ├── memory.pressure
│   │   ├── memory.reclaim
│   │   ├── memory.stat
│   │   ├── memory.swap.current
│   │   ├── memory.swap.events
│   │   ├── memory.swap.high
│   │   ├── memory.swap.max
│   │   ├── memory.swap.peak
│   │   ├── memory.zswap.current
│   │   ├── memory.zswap.max
│   │   ├── memory.zswap.writeback
│   │   ├── pids.current
│   │   ├── pids.events
│   │   ├── pids.max
│   │   └── pids.peak
│   ├── snap-snapd-25935.mount
│   │   ├── cgroup.controllers
│   │   ├── cgroup.events
│   │   ├── cgroup.freeze
│   │   ├── cgroup.kill
│   │   ├── cgroup.max.depth
│   │   ├── cgroup.max.descendants
│   │   ├── cgroup.pressure
│   │   ├── cgroup.procs
│   │   ├── cgroup.stat
│   │   ├── cgroup.subtree_control
│   │   ├── cgroup.threads
│   │   ├── cgroup.type
│   │   ├── cpu.pressure
│   │   ├── cpu.stat
│   │   ├── cpu.stat.local
│   │   ├── io.pressure
│   │   ├── memory.current
│   │   ├── memory.events
│   │   ├── memory.events.local
│   │   ├── memory.high
│   │   ├── memory.low
│   │   ├── memory.max
│   │   ├── memory.min
│   │   ├── memory.numa_stat
│   │   ├── memory.oom.group
│   │   ├── memory.peak
│   │   ├── memory.pressure
│   │   ├── memory.reclaim
│   │   ├── memory.stat
│   │   ├── memory.swap.current
│   │   ├── memory.swap.events
│   │   ├── memory.swap.high
│   │   ├── memory.swap.max
│   │   ├── memory.swap.peak
│   │   ├── memory.zswap.current
│   │   ├── memory.zswap.max
│   │   ├── memory.zswap.writeback
│   │   ├── pids.current
│   │   ├── pids.events
│   │   ├── pids.max
│   │   └── pids.peak
│   ├── snap-snapd\x2ddesktop\x2dintegration-178.mount
│   │   ├── cgroup.controllers
│   │   ├── cgroup.events
│   │   ├── cgroup.freeze
│   │   ├── cgroup.kill
│   │   ├── cgroup.max.depth
│   │   ├── cgroup.max.descendants
│   │   ├── cgroup.pressure
│   │   ├── cgroup.procs
│   │   ├── cgroup.stat
│   │   ├── cgroup.subtree_control
│   │   ├── cgroup.threads
│   │   ├── cgroup.type
│   │   ├── cpu.pressure
│   │   ├── cpu.stat
│   │   ├── cpu.stat.local
│   │   ├── io.pressure
│   │   ├── memory.current
│   │   ├── memory.events
│   │   ├── memory.events.local
│   │   ├── memory.high
│   │   ├── memory.low
│   │   ├── memory.max
│   │   ├── memory.min
│   │   ├── memory.numa_stat
│   │   ├── memory.oom.group
│   │   ├── memory.peak
│   │   ├── memory.pressure
│   │   ├── memory.reclaim
│   │   ├── memory.stat
│   │   ├── memory.swap.current
│   │   ├── memory.swap.events
│   │   ├── memory.swap.high
│   │   ├── memory.swap.max
│   │   ├── memory.swap.peak
│   │   ├── memory.zswap.current
│   │   ├── memory.zswap.max
│   │   ├── memory.zswap.writeback
│   │   ├── pids.current
│   │   ├── pids.events
│   │   ├── pids.max
│   │   └── pids.peak
│   ├── snap-snapd\x2ddesktop\x2dintegration-315.mount
│   │   ├── cgroup.controllers
│   │   ├── cgroup.events
│   │   ├── cgroup.freeze
│   │   ├── cgroup.kill
│   │   ├── cgroup.max.depth
│   │   ├── cgroup.max.descendants
│   │   ├── cgroup.pressure
│   │   ├── cgroup.procs
│   │   ├── cgroup.stat
│   │   ├── cgroup.subtree_control
│   │   ├── cgroup.threads
│   │   ├── cgroup.type
│   │   ├── cpu.pressure
│   │   ├── cpu.stat
│   │   ├── cpu.stat.local
│   │   ├── io.pressure
│   │   ├── memory.current
│   │   ├── memory.events
│   │   ├── memory.events.local
│   │   ├── memory.high
│   │   ├── memory.low
│   │   ├── memory.max
│   │   ├── memory.min
│   │   ├── memory.numa_stat
│   │   ├── memory.oom.group
│   │   ├── memory.peak
│   │   ├── memory.pressure
│   │   ├── memory.reclaim
│   │   ├── memory.stat
│   │   ├── memory.swap.current
│   │   ├── memory.swap.events
│   │   ├── memory.swap.high
│   │   ├── memory.swap.max
│   │   ├── memory.swap.peak
│   │   ├── memory.zswap.current
│   │   ├── memory.zswap.max
│   │   ├── memory.zswap.writeback
│   │   ├── pids.current
│   │   ├── pids.events
│   │   ├── pids.max
│   │   └── pids.peak
│   ├── snap-snap\x2dstore-1113.mount
│   │   ├── cgroup.controllers
│   │   ├── cgroup.events
│   │   ├── cgroup.freeze
│   │   ├── cgroup.kill
│   │   ├── cgroup.max.depth
│   │   ├── cgroup.max.descendants
│   │   ├── cgroup.pressure
│   │   ├── cgroup.procs
│   │   ├── cgroup.stat
│   │   ├── cgroup.subtree_control
│   │   ├── cgroup.threads
│   │   ├── cgroup.type
│   │   ├── cpu.pressure
│   │   ├── cpu.stat
│   │   ├── cpu.stat.local
│   │   ├── io.pressure
│   │   ├── memory.current
│   │   ├── memory.events
│   │   ├── memory.events.local
│   │   ├── memory.high
│   │   ├── memory.low
│   │   ├── memory.max
│   │   ├── memory.min
│   │   ├── memory.numa_stat
│   │   ├── memory.oom.group
│   │   ├── memory.peak
│   │   ├── memory.pressure
│   │   ├── memory.reclaim
│   │   ├── memory.stat
│   │   ├── memory.swap.current
│   │   ├── memory.swap.events
│   │   ├── memory.swap.high
│   │   ├── memory.swap.max
│   │   ├── memory.swap.peak
│   │   ├── memory.zswap.current
│   │   ├── memory.zswap.max
│   │   ├── memory.zswap.writeback
│   │   ├── pids.current
│   │   ├── pids.events
│   │   ├── pids.max
│   │   └── pids.peak
│   ├── snap-snap\x2dstore-1216.mount
│   │   ├── cgroup.controllers
│   │   ├── cgroup.events
│   │   ├── cgroup.freeze
│   │   ├── cgroup.kill
│   │   ├── cgroup.max.depth
│   │   ├── cgroup.max.descendants
│   │   ├── cgroup.pressure
│   │   ├── cgroup.procs
│   │   ├── cgroup.stat
│   │   ├── cgroup.subtree_control
│   │   ├── cgroup.threads
│   │   ├── cgroup.type
│   │   ├── cpu.pressure
│   │   ├── cpu.stat
│   │   ├── cpu.stat.local
│   │   ├── io.pressure
│   │   ├── memory.current
│   │   ├── memory.events
│   │   ├── memory.events.local
│   │   ├── memory.high
│   │   ├── memory.low
│   │   ├── memory.max
│   │   ├── memory.min
│   │   ├── memory.numa_stat
│   │   ├── memory.oom.group
│   │   ├── memory.peak
│   │   ├── memory.pressure
│   │   ├── memory.reclaim
│   │   ├── memory.stat
│   │   ├── memory.swap.current
│   │   ├── memory.swap.events
│   │   ├── memory.swap.high
│   │   ├── memory.swap.max
│   │   ├── memory.swap.peak
│   │   ├── memory.zswap.current
│   │   ├── memory.zswap.max
│   │   ├── memory.zswap.writeback
│   │   ├── pids.current
│   │   ├── pids.events
│   │   ├── pids.max
│   │   └── pids.peak
│   ├── snap-webkitgtk\x2d6\x2dgnome\x2d2404-58.mount
│   │   ├── cgroup.controllers
│   │   ├── cgroup.events
│   │   ├── cgroup.freeze
│   │   ├── cgroup.kill
│   │   ├── cgroup.max.depth
│   │   ├── cgroup.max.descendants
│   │   ├── cgroup.pressure
│   │   ├── cgroup.procs
│   │   ├── cgroup.stat
│   │   ├── cgroup.subtree_control
│   │   ├── cgroup.threads
│   │   ├── cgroup.type
│   │   ├── cpu.pressure
│   │   ├── cpu.stat
│   │   ├── cpu.stat.local
│   │   ├── io.pressure
│   │   ├── memory.current
│   │   ├── memory.events
│   │   ├── memory.events.local
│   │   ├── memory.high
│   │   ├── memory.low
│   │   ├── memory.max
│   │   ├── memory.min
│   │   ├── memory.numa_stat
│   │   ├── memory.oom.group
│   │   ├── memory.peak
│   │   ├── memory.pressure
│   │   ├── memory.reclaim
│   │   ├── memory.stat
│   │   ├── memory.swap.current
│   │   ├── memory.swap.events
│   │   ├── memory.swap.high
│   │   ├── memory.swap.max
│   │   ├── memory.swap.peak
│   │   ├── memory.zswap.current
│   │   ├── memory.zswap.max
│   │   ├── memory.zswap.writeback
│   │   ├── pids.current
│   │   ├── pids.events
│   │   ├── pids.max
│   │   └── pids.peak
│   ├── swapfile.swap
│   │   ├── cgroup.controllers
│   │   ├── cgroup.events
│   │   ├── cgroup.freeze
│   │   ├── cgroup.kill
│   │   ├── cgroup.max.depth
│   │   ├── cgroup.max.descendants
│   │   ├── cgroup.pressure
│   │   ├── cgroup.procs
│   │   ├── cgroup.stat
│   │   ├── cgroup.subtree_control
│   │   ├── cgroup.threads
│   │   ├── cgroup.type
│   │   ├── cpu.pressure
│   │   ├── cpu.stat
│   │   ├── cpu.stat.local
│   │   ├── io.pressure
│   │   ├── memory.current
│   │   ├── memory.events
│   │   ├── memory.events.local
│   │   ├── memory.high
│   │   ├── memory.low
│   │   ├── memory.max
│   │   ├── memory.min
│   │   ├── memory.numa_stat
│   │   ├── memory.oom.group
│   │   ├── memory.peak
│   │   ├── memory.pressure
│   │   ├── memory.reclaim
│   │   ├── memory.stat
│   │   ├── memory.swap.current
│   │   ├── memory.swap.events
│   │   ├── memory.swap.high
│   │   ├── memory.swap.max
│   │   ├── memory.swap.peak
│   │   ├── memory.zswap.current
│   │   ├── memory.zswap.max
│   │   ├── memory.zswap.writeback
│   │   ├── pids.current
│   │   ├── pids.events
│   │   ├── pids.max
│   │   └── pids.peak
│   ├── switcheroo-control.service
│   │   ├── cgroup.controllers
│   │   ├── cgroup.events
│   │   ├── cgroup.freeze
│   │   ├── cgroup.kill
│   │   ├── cgroup.max.depth
│   │   ├── cgroup.max.descendants
│   │   ├── cgroup.pressure
│   │   ├── cgroup.procs
│   │   ├── cgroup.stat
│   │   ├── cgroup.subtree_control
│   │   ├── cgroup.threads
│   │   ├── cgroup.type
│   │   ├── cpu.pressure
│   │   ├── cpu.stat
│   │   ├── cpu.stat.local
│   │   ├── io.pressure
│   │   ├── memory.current
│   │   ├── memory.events
│   │   ├── memory.events.local
│   │   ├── memory.high
│   │   ├── memory.low
│   │   ├── memory.max
│   │   ├── memory.min
│   │   ├── memory.numa_stat
│   │   ├── memory.oom.group
│   │   ├── memory.peak
│   │   ├── memory.pressure
│   │   ├── memory.reclaim
│   │   ├── memory.stat
│   │   ├── memory.swap.current
│   │   ├── memory.swap.events
│   │   ├── memory.swap.high
│   │   ├── memory.swap.max
│   │   ├── memory.swap.peak
│   │   ├── memory.zswap.current
│   │   ├── memory.zswap.max
│   │   ├── memory.zswap.writeback
│   │   ├── pids.current
│   │   ├── pids.events
│   │   ├── pids.max
│   │   └── pids.peak
│   ├── systemd-journald.service
│   │   ├── cgroup.controllers
│   │   ├── cgroup.events
│   │   ├── cgroup.freeze
│   │   ├── cgroup.kill
│   │   ├── cgroup.max.depth
│   │   ├── cgroup.max.descendants
│   │   ├── cgroup.pressure
│   │   ├── cgroup.procs
│   │   ├── cgroup.stat
│   │   ├── cgroup.subtree_control
│   │   ├── cgroup.threads
│   │   ├── cgroup.type
│   │   ├── cpu.pressure
│   │   ├── cpu.stat
│   │   ├── cpu.stat.local
│   │   ├── io.pressure
│   │   ├── memory.current
│   │   ├── memory.events
│   │   ├── memory.events.local
│   │   ├── memory.high
│   │   ├── memory.low
│   │   ├── memory.max
│   │   ├── memory.min
│   │   ├── memory.numa_stat
│   │   ├── memory.oom.group
│   │   ├── memory.peak
│   │   ├── memory.pressure
│   │   ├── memory.reclaim
│   │   ├── memory.stat
│   │   ├── memory.swap.current
│   │   ├── memory.swap.events
│   │   ├── memory.swap.high
│   │   ├── memory.swap.max
│   │   ├── memory.swap.peak
│   │   ├── memory.zswap.current
│   │   ├── memory.zswap.max
│   │   ├── memory.zswap.writeback
│   │   ├── pids.current
│   │   ├── pids.events
│   │   ├── pids.max
│   │   └── pids.peak
│   ├── systemd-logind.service
│   │   ├── cgroup.controllers
│   │   ├── cgroup.events
│   │   ├── cgroup.freeze
│   │   ├── cgroup.kill
│   │   ├── cgroup.max.depth
│   │   ├── cgroup.max.descendants
│   │   ├── cgroup.pressure
│   │   ├── cgroup.procs
│   │   ├── cgroup.stat
│   │   ├── cgroup.subtree_control
│   │   ├── cgroup.threads
│   │   ├── cgroup.type
│   │   ├── cpu.pressure
│   │   ├── cpu.stat
│   │   ├── cpu.stat.local
│   │   ├── io.pressure
│   │   ├── memory.current
│   │   ├── memory.events
│   │   ├── memory.events.local
│   │   ├── memory.high
│   │   ├── memory.low
│   │   ├── memory.max
│   │   ├── memory.min
│   │   ├── memory.numa_stat
│   │   ├── memory.oom.group
│   │   ├── memory.peak
│   │   ├── memory.pressure
│   │   ├── memory.reclaim
│   │   ├── memory.stat
│   │   ├── memory.swap.current
│   │   ├── memory.swap.events
│   │   ├── memory.swap.high
│   │   ├── memory.swap.max
│   │   ├── memory.swap.peak
│   │   ├── memory.zswap.current
│   │   ├── memory.zswap.max
│   │   ├── memory.zswap.writeback
│   │   ├── pids.current
│   │   ├── pids.events
│   │   ├── pids.max
│   │   └── pids.peak
│   ├── systemd-oomd.service
│   │   ├── cgroup.controllers
│   │   ├── cgroup.events
│   │   ├── cgroup.freeze
│   │   ├── cgroup.kill
│   │   ├── cgroup.max.depth
│   │   ├── cgroup.max.descendants
│   │   ├── cgroup.pressure
│   │   ├── cgroup.procs
│   │   ├── cgroup.stat
│   │   ├── cgroup.subtree_control
│   │   ├── cgroup.threads
│   │   ├── cgroup.type
│   │   ├── cpu.pressure
│   │   ├── cpu.stat
│   │   ├── cpu.stat.local
│   │   ├── io.pressure
│   │   ├── memory.current
│   │   ├── memory.events
│   │   ├── memory.events.local
│   │   ├── memory.high
│   │   ├── memory.low
│   │   ├── memory.max
│   │   ├── memory.min
│   │   ├── memory.numa_stat
│   │   ├── memory.oom.group
│   │   ├── memory.peak
│   │   ├── memory.pressure
│   │   ├── memory.reclaim
│   │   ├── memory.stat
│   │   ├── memory.swap.current
│   │   ├── memory.swap.events
│   │   ├── memory.swap.high
│   │   ├── memory.swap.max
│   │   ├── memory.swap.peak
│   │   ├── memory.zswap.current
│   │   ├── memory.zswap.max
│   │   ├── memory.zswap.writeback
│   │   ├── pids.current
│   │   ├── pids.events
│   │   ├── pids.max
│   │   └── pids.peak
│   ├── systemd-resolved.service
│   │   ├── cgroup.controllers
│   │   ├── cgroup.events
│   │   ├── cgroup.freeze
│   │   ├── cgroup.kill
│   │   ├── cgroup.max.depth
│   │   ├── cgroup.max.descendants
│   │   ├── cgroup.pressure
│   │   ├── cgroup.procs
│   │   ├── cgroup.stat
│   │   ├── cgroup.subtree_control
│   │   ├── cgroup.threads
│   │   ├── cgroup.type
│   │   ├── cpu.pressure
│   │   ├── cpu.stat
│   │   ├── cpu.stat.local
│   │   ├── io.pressure
│   │   ├── memory.current
│   │   ├── memory.events
│   │   ├── memory.events.local
│   │   ├── memory.high
│   │   ├── memory.low
│   │   ├── memory.max
│   │   ├── memory.min
│   │   ├── memory.numa_stat
│   │   ├── memory.oom.group
│   │   ├── memory.peak
│   │   ├── memory.pressure
│   │   ├── memory.reclaim
│   │   ├── memory.stat
│   │   ├── memory.swap.current
│   │   ├── memory.swap.events
│   │   ├── memory.swap.high
│   │   ├── memory.swap.max
│   │   ├── memory.swap.peak
│   │   ├── memory.zswap.current
│   │   ├── memory.zswap.max
│   │   ├── memory.zswap.writeback
│   │   ├── pids.current
│   │   ├── pids.events
│   │   ├── pids.max
│   │   └── pids.peak
│   ├── systemd-timesyncd.service
│   │   ├── cgroup.controllers
│   │   ├── cgroup.events
│   │   ├── cgroup.freeze
│   │   ├── cgroup.kill
│   │   ├── cgroup.max.depth
│   │   ├── cgroup.max.descendants
│   │   ├── cgroup.pressure
│   │   ├── cgroup.procs
│   │   ├── cgroup.stat
│   │   ├── cgroup.subtree_control
│   │   ├── cgroup.threads
│   │   ├── cgroup.type
│   │   ├── cpu.pressure
│   │   ├── cpu.stat
│   │   ├── cpu.stat.local
│   │   ├── io.pressure
│   │   ├── memory.current
│   │   ├── memory.events
│   │   ├── memory.events.local
│   │   ├── memory.high
│   │   ├── memory.low
│   │   ├── memory.max
│   │   ├── memory.min
│   │   ├── memory.numa_stat
│   │   ├── memory.oom.group
│   │   ├── memory.peak
│   │   ├── memory.pressure
│   │   ├── memory.reclaim
│   │   ├── memory.stat
│   │   ├── memory.swap.current
│   │   ├── memory.swap.events
│   │   ├── memory.swap.high
│   │   ├── memory.swap.max
│   │   ├── memory.swap.peak
│   │   ├── memory.zswap.current
│   │   ├── memory.zswap.max
│   │   ├── memory.zswap.writeback
│   │   ├── pids.current
│   │   ├── pids.events
│   │   ├── pids.max
│   │   └── pids.peak
│   ├── systemd-udevd.service
│   │   ├── cgroup.controllers
│   │   ├── cgroup.events
│   │   ├── cgroup.freeze
│   │   ├── cgroup.kill
│   │   ├── cgroup.max.depth
│   │   ├── cgroup.max.descendants
│   │   ├── cgroup.pressure
│   │   ├── cgroup.procs
│   │   ├── cgroup.stat
│   │   ├── cgroup.subtree_control
│   │   ├── cgroup.threads
│   │   ├── cgroup.type
│   │   ├── cpu.pressure
│   │   ├── cpu.stat
│   │   ├── cpu.stat.local
│   │   ├── io.pressure
│   │   ├── memory.current
│   │   ├── memory.events
│   │   ├── memory.events.local
│   │   ├── memory.high
│   │   ├── memory.low
│   │   ├── memory.max
│   │   ├── memory.min
│   │   ├── memory.numa_stat
│   │   ├── memory.oom.group
│   │   ├── memory.peak
│   │   ├── memory.pressure
│   │   ├── memory.reclaim
│   │   ├── memory.stat
│   │   ├── memory.swap.current
│   │   ├── memory.swap.events
│   │   ├── memory.swap.high
│   │   ├── memory.swap.max
│   │   ├── memory.swap.peak
│   │   ├── memory.zswap.current
│   │   ├── memory.zswap.max
│   │   ├── memory.zswap.writeback
│   │   ├── pids.current
│   │   ├── pids.events
│   │   ├── pids.max
│   │   └── pids.peak
│   ├── system-getty.slice
│   │   ├── cgroup.controllers
│   │   ├── cgroup.events
│   │   ├── cgroup.freeze
│   │   ├── cgroup.kill
│   │   ├── cgroup.max.depth
│   │   ├── cgroup.max.descendants
│   │   ├── cgroup.pressure
│   │   ├── cgroup.procs
│   │   ├── cgroup.stat
│   │   ├── cgroup.subtree_control
│   │   ├── cgroup.threads
│   │   ├── cgroup.type
│   │   ├── cpu.pressure
│   │   ├── cpu.stat
│   │   ├── cpu.stat.local
│   │   ├── io.pressure
│   │   ├── memory.current
│   │   ├── memory.events
│   │   ├── memory.events.local
│   │   ├── memory.high
│   │   ├── memory.low
│   │   ├── memory.max
│   │   ├── memory.min
│   │   ├── memory.numa_stat
│   │   ├── memory.oom.group
│   │   ├── memory.peak
│   │   ├── memory.pressure
│   │   ├── memory.reclaim
│   │   ├── memory.stat
│   │   ├── memory.swap.current
│   │   ├── memory.swap.events
│   │   ├── memory.swap.high
│   │   ├── memory.swap.max
│   │   ├── memory.swap.peak
│   │   ├── memory.zswap.current
│   │   ├── memory.zswap.max
│   │   ├── memory.zswap.writeback
│   │   ├── pids.current
│   │   ├── pids.events
│   │   ├── pids.max
│   │   └── pids.peak
│   ├── system-modprobe.slice
│   │   ├── cgroup.controllers
│   │   ├── cgroup.events
│   │   ├── cgroup.freeze
│   │   ├── cgroup.kill
│   │   ├── cgroup.max.depth
│   │   ├── cgroup.max.descendants
│   │   ├── cgroup.pressure
│   │   ├── cgroup.procs
│   │   ├── cgroup.stat
│   │   ├── cgroup.subtree_control
│   │   ├── cgroup.threads
│   │   ├── cgroup.type
│   │   ├── cpu.pressure
│   │   ├── cpu.stat
│   │   ├── cpu.stat.local
│   │   ├── io.pressure
│   │   ├── memory.current
│   │   ├── memory.events
│   │   ├── memory.events.local
│   │   ├── memory.high
│   │   ├── memory.low
│   │   ├── memory.max
│   │   ├── memory.min
│   │   ├── memory.numa_stat
│   │   ├── memory.oom.group
│   │   ├── memory.peak
│   │   ├── memory.pressure
│   │   ├── memory.reclaim
│   │   ├── memory.stat
│   │   ├── memory.swap.current
│   │   ├── memory.swap.events
│   │   ├── memory.swap.high
│   │   ├── memory.swap.max
│   │   ├── memory.swap.peak
│   │   ├── memory.zswap.current
│   │   ├── memory.zswap.max
│   │   ├── memory.zswap.writeback
│   │   ├── pids.current
│   │   ├── pids.events
│   │   ├── pids.max
│   │   └── pids.peak
│   ├── system-systemd\x2dfsck.slice
│   │   ├── cgroup.controllers
│   │   ├── cgroup.events
│   │   ├── cgroup.freeze
│   │   ├── cgroup.kill
│   │   ├── cgroup.max.depth
│   │   ├── cgroup.max.descendants
│   │   ├── cgroup.pressure
│   │   ├── cgroup.procs
│   │   ├── cgroup.stat
│   │   ├── cgroup.subtree_control
│   │   ├── cgroup.threads
│   │   ├── cgroup.type
│   │   ├── cpu.pressure
│   │   ├── cpu.stat
│   │   ├── cpu.stat.local
│   │   ├── io.pressure
│   │   ├── memory.current
│   │   ├── memory.events
│   │   ├── memory.events.local
│   │   ├── memory.high
│   │   ├── memory.low
│   │   ├── memory.max
│   │   ├── memory.min
│   │   ├── memory.numa_stat
│   │   ├── memory.oom.group
│   │   ├── memory.peak
│   │   ├── memory.pressure
│   │   ├── memory.reclaim
│   │   ├── memory.stat
│   │   ├── memory.swap.current
│   │   ├── memory.swap.events
│   │   ├── memory.swap.high
│   │   ├── memory.swap.max
│   │   ├── memory.swap.peak
│   │   ├── memory.zswap.current
│   │   ├── memory.zswap.max
│   │   ├── memory.zswap.writeback
│   │   ├── pids.current
│   │   ├── pids.events
│   │   ├── pids.max
│   │   └── pids.peak
│   ├── todeskd.service
│   │   ├── cgroup.controllers
│   │   ├── cgroup.events
│   │   ├── cgroup.freeze
│   │   ├── cgroup.kill
│   │   ├── cgroup.max.depth
│   │   ├── cgroup.max.descendants
│   │   ├── cgroup.pressure
│   │   ├── cgroup.procs
│   │   ├── cgroup.stat
│   │   ├── cgroup.subtree_control
│   │   ├── cgroup.threads
│   │   ├── cgroup.type
│   │   ├── cpu.pressure
│   │   ├── cpu.stat
│   │   ├── cpu.stat.local
│   │   ├── io.pressure
│   │   ├── memory.current
│   │   ├── memory.events
│   │   ├── memory.events.local
│   │   ├── memory.high
│   │   ├── memory.low
│   │   ├── memory.max
│   │   ├── memory.min
│   │   ├── memory.numa_stat
│   │   ├── memory.oom.group
│   │   ├── memory.peak
│   │   ├── memory.pressure
│   │   ├── memory.reclaim
│   │   ├── memory.stat
│   │   ├── memory.swap.current
│   │   ├── memory.swap.events
│   │   ├── memory.swap.high
│   │   ├── memory.swap.max
│   │   ├── memory.swap.peak
│   │   ├── memory.zswap.current
│   │   ├── memory.zswap.max
│   │   ├── memory.zswap.writeback
│   │   ├── pids.current
│   │   ├── pids.events
│   │   ├── pids.max
│   │   └── pids.peak
│   ├── udisks2.service
│   │   ├── cgroup.controllers
│   │   ├── cgroup.events
│   │   ├── cgroup.freeze
│   │   ├── cgroup.kill
│   │   ├── cgroup.max.depth
│   │   ├── cgroup.max.descendants
│   │   ├── cgroup.pressure
│   │   ├── cgroup.procs
│   │   ├── cgroup.stat
│   │   ├── cgroup.subtree_control
│   │   ├── cgroup.threads
│   │   ├── cgroup.type
│   │   ├── cpu.pressure
│   │   ├── cpu.stat
│   │   ├── cpu.stat.local
│   │   ├── io.pressure
│   │   ├── memory.current
│   │   ├── memory.events
│   │   ├── memory.events.local
│   │   ├── memory.high
│   │   ├── memory.low
│   │   ├── memory.max
│   │   ├── memory.min
│   │   ├── memory.numa_stat
│   │   ├── memory.oom.group
│   │   ├── memory.peak
│   │   ├── memory.pressure
│   │   ├── memory.reclaim
│   │   ├── memory.stat
│   │   ├── memory.swap.current
│   │   ├── memory.swap.events
│   │   ├── memory.swap.high
│   │   ├── memory.swap.max
│   │   ├── memory.swap.peak
│   │   ├── memory.zswap.current
│   │   ├── memory.zswap.max
│   │   ├── memory.zswap.writeback
│   │   ├── pids.current
│   │   ├── pids.events
│   │   ├── pids.max
│   │   └── pids.peak
│   ├── unattended-upgrades.service
│   │   ├── cgroup.controllers
│   │   ├── cgroup.events
│   │   ├── cgroup.freeze
│   │   ├── cgroup.kill
│   │   ├── cgroup.max.depth
│   │   ├── cgroup.max.descendants
│   │   ├── cgroup.pressure
│   │   ├── cgroup.procs
│   │   ├── cgroup.stat
│   │   ├── cgroup.subtree_control
│   │   ├── cgroup.threads
│   │   ├── cgroup.type
│   │   ├── cpu.pressure
│   │   ├── cpu.stat
│   │   ├── cpu.stat.local
│   │   ├── io.pressure
│   │   ├── memory.current
│   │   ├── memory.events
│   │   ├── memory.events.local
│   │   ├── memory.high
│   │   ├── memory.low
│   │   ├── memory.max
│   │   ├── memory.min
│   │   ├── memory.numa_stat
│   │   ├── memory.oom.group
│   │   ├── memory.peak
│   │   ├── memory.pressure
│   │   ├── memory.reclaim
│   │   ├── memory.stat
│   │   ├── memory.swap.current
│   │   ├── memory.swap.events
│   │   ├── memory.swap.high
│   │   ├── memory.swap.max
│   │   ├── memory.swap.peak
│   │   ├── memory.zswap.current
│   │   ├── memory.zswap.max
│   │   ├── memory.zswap.writeback
│   │   ├── pids.current
│   │   ├── pids.events
│   │   ├── pids.max
│   │   └── pids.peak
│   ├── upower.service
│   │   ├── cgroup.controllers
│   │   ├── cgroup.events
│   │   ├── cgroup.freeze
│   │   ├── cgroup.kill
│   │   ├── cgroup.max.depth
│   │   ├── cgroup.max.descendants
│   │   ├── cgroup.pressure
│   │   ├── cgroup.procs
│   │   ├── cgroup.stat
│   │   ├── cgroup.subtree_control
│   │   ├── cgroup.threads
│   │   ├── cgroup.type
│   │   ├── cpu.pressure
│   │   ├── cpu.stat
│   │   ├── cpu.stat.local
│   │   ├── io.pressure
│   │   ├── memory.current
│   │   ├── memory.events
│   │   ├── memory.events.local
│   │   ├── memory.high
│   │   ├── memory.low
│   │   ├── memory.max
│   │   ├── memory.min
│   │   ├── memory.numa_stat
│   │   ├── memory.oom.group
│   │   ├── memory.peak
│   │   ├── memory.pressure
│   │   ├── memory.reclaim
│   │   ├── memory.stat
│   │   ├── memory.swap.current
│   │   ├── memory.swap.events
│   │   ├── memory.swap.high
│   │   ├── memory.swap.max
│   │   ├── memory.swap.peak
│   │   ├── memory.zswap.current
│   │   ├── memory.zswap.max
│   │   ├── memory.zswap.writeback
│   │   ├── pids.current
│   │   ├── pids.events
│   │   ├── pids.max
│   │   └── pids.peak
│   └── wpa_supplicant.service
│       ├── cgroup.controllers
│       ├── cgroup.events
│       ├── cgroup.freeze
│       ├── cgroup.kill
│       ├── cgroup.max.depth
│       ├── cgroup.max.descendants
│       ├── cgroup.pressure
│       ├── cgroup.procs
│       ├── cgroup.stat
│       ├── cgroup.subtree_control
│       ├── cgroup.threads
│       ├── cgroup.type
│       ├── cpu.pressure
│       ├── cpu.stat
│       ├── cpu.stat.local
│       ├── io.pressure
│       ├── memory.current
│       ├── memory.events
│       ├── memory.events.local
│       ├── memory.high
│       ├── memory.low
│       ├── memory.max
│       ├── memory.min
│       ├── memory.numa_stat
│       ├── memory.oom.group
│       ├── memory.peak
│       ├── memory.pressure
│       ├── memory.reclaim
│       ├── memory.stat
│       ├── memory.swap.current
│       ├── memory.swap.events
│       ├── memory.swap.high
│       ├── memory.swap.max
│       ├── memory.swap.peak
│       ├── memory.zswap.current
│       ├── memory.zswap.max
│       ├── memory.zswap.writeback
│       ├── pids.current
│       ├── pids.events
│       ├── pids.max
│       └── pids.peak
└── user.slice
    ├── cgroup.controllers
    ├── cgroup.events
    ├── cgroup.freeze
    ├── cgroup.kill
    ├── cgroup.max.depth
    ├── cgroup.max.descendants
    ├── cgroup.pressure
    ├── cgroup.procs
    ├── cgroup.stat
    ├── cgroup.subtree_control
    ├── cgroup.threads
    ├── cgroup.type
    ├── cpu.pressure
    ├── cpu.stat
    ├── cpu.stat.local
    ├── io.pressure
    ├── memory.current
    ├── memory.events
    ├── memory.events.local
    ├── memory.high
    ├── memory.low
    ├── memory.max
    ├── memory.min
    ├── memory.numa_stat
    ├── memory.oom.group
    ├── memory.peak
    ├── memory.pressure
    ├── memory.reclaim
    ├── memory.stat
    ├── memory.swap.current
    ├── memory.swap.events
    ├── memory.swap.high
    ├── memory.swap.max
    ├── memory.swap.peak
    ├── memory.zswap.current
    ├── memory.zswap.max
    ├── memory.zswap.writeback
    ├── pids.current
    ├── pids.events
    ├── pids.max
    ├── pids.peak
    └── user-1000.slice
        ├── cgroup.controllers
        ├── cgroup.events
        ├── cgroup.freeze
        ├── cgroup.kill
        ├── cgroup.max.depth
        ├── cgroup.max.descendants
        ├── cgroup.pressure
        ├── cgroup.procs
        ├── cgroup.stat
        ├── cgroup.subtree_control
        ├── cgroup.threads
        ├── cgroup.type
        ├── cpu.pressure
        ├── cpu.stat
        ├── cpu.stat.local
        ├── io.pressure
        ├── memory.current
        ├── memory.events
        ├── memory.events.local
        ├── memory.high
        ├── memory.low
        ├── memory.max
        ├── memory.min
        ├── memory.numa_stat
        ├── memory.oom.group
        ├── memory.peak
        ├── memory.pressure
        ├── memory.reclaim
        ├── memory.stat
        ├── memory.swap.current
        ├── memory.swap.events
        ├── memory.swap.high
        ├── memory.swap.max
        ├── memory.swap.peak
        ├── memory.zswap.current
        ├── memory.zswap.max
        ├── memory.zswap.writeback
        ├── pids.current
        ├── pids.events
        ├── pids.max
        ├── pids.peak
        ├── session-2.scope
        │   ├── cgroup.controllers
        │   ├── cgroup.events
        │   ├── cgroup.freeze
        │   ├── cgroup.kill
        │   ├── cgroup.max.depth
        │   ├── cgroup.max.descendants
        │   ├── cgroup.pressure
        │   ├── cgroup.procs
        │   ├── cgroup.stat
        │   ├── cgroup.subtree_control
        │   ├── cgroup.threads
        │   ├── cgroup.type
        │   ├── cpu.pressure
        │   ├── cpu.stat
        │   ├── cpu.stat.local
        │   ├── io.pressure
        │   ├── memory.current
        │   ├── memory.events
        │   ├── memory.events.local
        │   ├── memory.high
        │   ├── memory.low
        │   ├── memory.max
        │   ├── memory.min
        │   ├── memory.numa_stat
        │   ├── memory.oom.group
        │   ├── memory.peak
        │   ├── memory.pressure
        │   ├── memory.reclaim
        │   ├── memory.stat
        │   ├── memory.swap.current
        │   ├── memory.swap.events
        │   ├── memory.swap.high
        │   ├── memory.swap.max
        │   ├── memory.swap.peak
        │   ├── memory.zswap.current
        │   ├── memory.zswap.max
        │   ├── memory.zswap.writeback
        │   ├── pids.current
        │   ├── pids.events
        │   ├── pids.max
        │   └── pids.peak
        └── user@1000.service
            ├── app.slice
            │   ├── app-gnome-code-7952.scope
            │   │   ├── cgroup.controllers
            │   │   ├── cgroup.events
            │   │   ├── cgroup.freeze
            │   │   ├── cgroup.kill
            │   │   ├── cgroup.max.depth
            │   │   ├── cgroup.max.descendants
            │   │   ├── cgroup.pressure
            │   │   ├── cgroup.procs
            │   │   ├── cgroup.stat
            │   │   ├── cgroup.subtree_control
            │   │   ├── cgroup.threads
            │   │   ├── cgroup.type
            │   │   ├── cpu.pressure
            │   │   ├── cpu.stat
            │   │   ├── cpu.stat.local
            │   │   ├── io.pressure
            │   │   ├── memory.current
            │   │   ├── memory.events
            │   │   ├── memory.events.local
            │   │   ├── memory.high
            │   │   ├── memory.low
            │   │   ├── memory.max
            │   │   ├── memory.min
            │   │   ├── memory.numa_stat
            │   │   ├── memory.oom.group
            │   │   ├── memory.peak
            │   │   ├── memory.pressure
            │   │   ├── memory.reclaim
            │   │   ├── memory.stat
            │   │   ├── memory.swap.current
            │   │   ├── memory.swap.events
            │   │   ├── memory.swap.high
            │   │   ├── memory.swap.max
            │   │   ├── memory.swap.peak
            │   │   ├── memory.zswap.current
            │   │   ├── memory.zswap.max
            │   │   ├── memory.zswap.writeback
            │   │   ├── pids.current
            │   │   ├── pids.events
            │   │   ├── pids.max
            │   │   └── pids.peak
            │   ├── app-gnome-fcitx\x2dui\x2dbaidu\x2dqimpanel\x2dln-2572.scope
            │   │   ├── cgroup.controllers
            │   │   ├── cgroup.events
            │   │   ├── cgroup.freeze
            │   │   ├── cgroup.kill
            │   │   ├── cgroup.max.depth
            │   │   ├── cgroup.max.descendants
            │   │   ├── cgroup.pressure
            │   │   ├── cgroup.procs
            │   │   ├── cgroup.stat
            │   │   ├── cgroup.subtree_control
            │   │   ├── cgroup.threads
            │   │   ├── cgroup.type
            │   │   ├── cpu.pressure
            │   │   ├── cpu.stat
            │   │   ├── cpu.stat.local
            │   │   ├── io.pressure
            │   │   ├── memory.current
            │   │   ├── memory.events
            │   │   ├── memory.events.local
            │   │   ├── memory.high
            │   │   ├── memory.low
            │   │   ├── memory.max
            │   │   ├── memory.min
            │   │   ├── memory.numa_stat
            │   │   ├── memory.oom.group
            │   │   ├── memory.peak
            │   │   ├── memory.pressure
            │   │   ├── memory.reclaim
            │   │   ├── memory.stat
            │   │   ├── memory.swap.current
            │   │   ├── memory.swap.events
            │   │   ├── memory.swap.high
            │   │   ├── memory.swap.max
            │   │   ├── memory.swap.peak
            │   │   ├── memory.zswap.current
            │   │   ├── memory.zswap.max
            │   │   ├── memory.zswap.writeback
            │   │   ├── pids.current
            │   │   ├── pids.events
            │   │   ├── pids.max
            │   │   └── pids.peak
            │   ├── app-gnome-org.gnome.Evolution\x2dalarm\x2dnotify-2044.scope
            │   │   ├── cgroup.controllers
            │   │   ├── cgroup.events
            │   │   ├── cgroup.freeze
            │   │   ├── cgroup.kill
            │   │   ├── cgroup.max.depth
            │   │   ├── cgroup.max.descendants
            │   │   ├── cgroup.pressure
            │   │   ├── cgroup.procs
            │   │   ├── cgroup.stat
            │   │   ├── cgroup.subtree_control
            │   │   ├── cgroup.threads
            │   │   ├── cgroup.type
            │   │   ├── cpu.pressure
            │   │   ├── cpu.stat
            │   │   ├── cpu.stat.local
            │   │   ├── io.pressure
            │   │   ├── memory.current
            │   │   ├── memory.events
            │   │   ├── memory.events.local
            │   │   ├── memory.high
            │   │   ├── memory.low
            │   │   ├── memory.max
            │   │   ├── memory.min
            │   │   ├── memory.numa_stat
            │   │   ├── memory.oom.group
            │   │   ├── memory.peak
            │   │   ├── memory.pressure
            │   │   ├── memory.reclaim
            │   │   ├── memory.stat
            │   │   ├── memory.swap.current
            │   │   ├── memory.swap.events
            │   │   ├── memory.swap.high
            │   │   ├── memory.swap.max
            │   │   ├── memory.swap.peak
            │   │   ├── memory.zswap.current
            │   │   ├── memory.zswap.max
            │   │   ├── memory.zswap.writeback
            │   │   ├── pids.current
            │   │   ├── pids.events
            │   │   ├── pids.max
            │   │   └── pids.peak
            │   ├── app-gnome-org.gnome.SettingsDaemon.DiskUtilityNotify-2069.scope
            │   │   ├── cgroup.controllers
            │   │   ├── cgroup.events
            │   │   ├── cgroup.freeze
            │   │   ├── cgroup.kill
            │   │   ├── cgroup.max.depth
            │   │   ├── cgroup.max.descendants
            │   │   ├── cgroup.pressure
            │   │   ├── cgroup.procs
            │   │   ├── cgroup.stat
            │   │   ├── cgroup.subtree_control
            │   │   ├── cgroup.threads
            │   │   ├── cgroup.type
            │   │   ├── cpu.pressure
            │   │   ├── cpu.stat
            │   │   ├── cpu.stat.local
            │   │   ├── io.pressure
            │   │   ├── memory.current
            │   │   ├── memory.events
            │   │   ├── memory.events.local
            │   │   ├── memory.high
            │   │   ├── memory.low
            │   │   ├── memory.max
            │   │   ├── memory.min
            │   │   ├── memory.numa_stat
            │   │   ├── memory.oom.group
            │   │   ├── memory.peak
            │   │   ├── memory.pressure
            │   │   ├── memory.reclaim
            │   │   ├── memory.stat
            │   │   ├── memory.swap.current
            │   │   ├── memory.swap.events
            │   │   ├── memory.swap.high
            │   │   ├── memory.swap.max
            │   │   ├── memory.swap.peak
            │   │   ├── memory.zswap.current
            │   │   ├── memory.zswap.max
            │   │   ├── memory.zswap.writeback
            │   │   ├── pids.current
            │   │   ├── pids.events
            │   │   ├── pids.max
            │   │   └── pids.peak
            │   ├── app-gnome-update\x2dnotifier-5399.scope
            │   │   ├── cgroup.controllers
            │   │   ├── cgroup.events
            │   │   ├── cgroup.freeze
            │   │   ├── cgroup.kill
            │   │   ├── cgroup.max.depth
            │   │   ├── cgroup.max.descendants
            │   │   ├── cgroup.pressure
            │   │   ├── cgroup.procs
            │   │   ├── cgroup.stat
            │   │   ├── cgroup.subtree_control
            │   │   ├── cgroup.threads
            │   │   ├── cgroup.type
            │   │   ├── cpu.pressure
            │   │   ├── cpu.stat
            │   │   ├── cpu.stat.local
            │   │   ├── io.pressure
            │   │   ├── memory.current
            │   │   ├── memory.events
            │   │   ├── memory.events.local
            │   │   ├── memory.high
            │   │   ├── memory.low
            │   │   ├── memory.max
            │   │   ├── memory.min
            │   │   ├── memory.numa_stat
            │   │   ├── memory.oom.group
            │   │   ├── memory.peak
            │   │   ├── memory.pressure
            │   │   ├── memory.reclaim
            │   │   ├── memory.stat
            │   │   ├── memory.swap.current
            │   │   ├── memory.swap.events
            │   │   ├── memory.swap.high
            │   │   ├── memory.swap.max
            │   │   ├── memory.swap.peak
            │   │   ├── memory.zswap.current
            │   │   ├── memory.zswap.max
            │   │   ├── memory.zswap.writeback
            │   │   ├── pids.current
            │   │   ├── pids.events
            │   │   ├── pids.max
            │   │   └── pids.peak
            │   ├── app-gnome\x2dsession\x2dmanager.slice
            │   │   ├── cgroup.controllers
            │   │   ├── cgroup.events
            │   │   ├── cgroup.freeze
            │   │   ├── cgroup.kill
            │   │   ├── cgroup.max.depth
            │   │   ├── cgroup.max.descendants
            │   │   ├── cgroup.pressure
            │   │   ├── cgroup.procs
            │   │   ├── cgroup.stat
            │   │   ├── cgroup.subtree_control
            │   │   ├── cgroup.threads
            │   │   ├── cgroup.type
            │   │   ├── cpu.pressure
            │   │   ├── cpu.stat
            │   │   ├── cpu.stat.local
            │   │   ├── gnome-session-manager@ubuntu.service
            │   │   │   ├── cgroup.controllers
            │   │   │   ├── cgroup.events
            │   │   │   ├── cgroup.freeze
            │   │   │   ├── cgroup.kill
            │   │   │   ├── cgroup.max.depth
            │   │   │   ├── cgroup.max.descendants
            │   │   │   ├── cgroup.pressure
            │   │   │   ├── cgroup.procs
            │   │   │   ├── cgroup.stat
            │   │   │   ├── cgroup.subtree_control
            │   │   │   ├── cgroup.threads
            │   │   │   ├── cgroup.type
            │   │   │   ├── cpu.pressure
            │   │   │   ├── cpu.stat
            │   │   │   ├── cpu.stat.local
            │   │   │   ├── io.pressure
            │   │   │   ├── memory.current
            │   │   │   ├── memory.events
            │   │   │   ├── memory.events.local
            │   │   │   ├── memory.high
            │   │   │   ├── memory.low
            │   │   │   ├── memory.max
            │   │   │   ├── memory.min
            │   │   │   ├── memory.numa_stat
            │   │   │   ├── memory.oom.group
            │   │   │   ├── memory.peak
            │   │   │   ├── memory.pressure
            │   │   │   ├── memory.reclaim
            │   │   │   ├── memory.stat
            │   │   │   ├── memory.swap.current
            │   │   │   ├── memory.swap.events
            │   │   │   ├── memory.swap.high
            │   │   │   ├── memory.swap.max
            │   │   │   ├── memory.swap.peak
            │   │   │   ├── memory.zswap.current
            │   │   │   ├── memory.zswap.max
            │   │   │   ├── memory.zswap.writeback
            │   │   │   ├── pids.current
            │   │   │   ├── pids.events
            │   │   │   ├── pids.max
            │   │   │   └── pids.peak
            │   │   ├── io.pressure
            │   │   ├── memory.current
            │   │   ├── memory.events
            │   │   ├── memory.events.local
            │   │   ├── memory.high
            │   │   ├── memory.low
            │   │   ├── memory.max
            │   │   ├── memory.min
            │   │   ├── memory.numa_stat
            │   │   ├── memory.oom.group
            │   │   ├── memory.peak
            │   │   ├── memory.pressure
            │   │   ├── memory.reclaim
            │   │   ├── memory.stat
            │   │   ├── memory.swap.current
            │   │   ├── memory.swap.events
            │   │   ├── memory.swap.high
            │   │   ├── memory.swap.max
            │   │   ├── memory.swap.peak
            │   │   ├── memory.zswap.current
            │   │   ├── memory.zswap.max
            │   │   ├── memory.zswap.writeback
            │   │   ├── pids.current
            │   │   ├── pids.events
            │   │   ├── pids.max
            │   │   └── pids.peak
            │   ├── app-org.chromium.Chromium-7952.scope
            │   │   ├── cgroup.controllers
            │   │   ├── cgroup.events
            │   │   ├── cgroup.freeze
            │   │   ├── cgroup.kill
            │   │   ├── cgroup.max.depth
            │   │   ├── cgroup.max.descendants
            │   │   ├── cgroup.pressure
            │   │   ├── cgroup.procs
            │   │   ├── cgroup.stat
            │   │   ├── cgroup.subtree_control
            │   │   ├── cgroup.threads
            │   │   ├── cgroup.type
            │   │   ├── cpu.pressure
            │   │   ├── cpu.stat
            │   │   ├── cpu.stat.local
            │   │   ├── io.pressure
            │   │   ├── memory.current
            │   │   ├── memory.events
            │   │   ├── memory.events.local
            │   │   ├── memory.high
            │   │   ├── memory.low
            │   │   ├── memory.max
            │   │   ├── memory.min
            │   │   ├── memory.numa_stat
            │   │   ├── memory.oom.group
            │   │   ├── memory.peak
            │   │   ├── memory.pressure
            │   │   ├── memory.reclaim
            │   │   ├── memory.stat
            │   │   ├── memory.swap.current
            │   │   ├── memory.swap.events
            │   │   ├── memory.swap.high
            │   │   ├── memory.swap.max
            │   │   ├── memory.swap.peak
            │   │   ├── memory.zswap.current
            │   │   ├── memory.zswap.max
            │   │   ├── memory.zswap.writeback
            │   │   ├── pids.current
            │   │   ├── pids.events
            │   │   ├── pids.max
            │   │   └── pids.peak
            │   ├── app-org.gnome.Terminal.slice
            │   │   ├── cgroup.controllers
            │   │   ├── cgroup.events
            │   │   ├── cgroup.freeze
            │   │   ├── cgroup.kill
            │   │   ├── cgroup.max.depth
            │   │   ├── cgroup.max.descendants
            │   │   ├── cgroup.pressure
            │   │   ├── cgroup.procs
            │   │   ├── cgroup.stat
            │   │   ├── cgroup.subtree_control
            │   │   ├── cgroup.threads
            │   │   ├── cgroup.type
            │   │   ├── cpu.pressure
            │   │   ├── cpu.stat
            │   │   ├── cpu.stat.local
            │   │   ├── gnome-terminal-server.service
            │   │   │   ├── cgroup.controllers
            │   │   │   ├── cgroup.events
            │   │   │   ├── cgroup.freeze
            │   │   │   ├── cgroup.kill
            │   │   │   ├── cgroup.max.depth
            │   │   │   ├── cgroup.max.descendants
            │   │   │   ├── cgroup.pressure
            │   │   │   ├── cgroup.procs
            │   │   │   ├── cgroup.stat
            │   │   │   ├── cgroup.subtree_control
            │   │   │   ├── cgroup.threads
            │   │   │   ├── cgroup.type
            │   │   │   ├── cpu.pressure
            │   │   │   ├── cpu.stat
            │   │   │   ├── cpu.stat.local
            │   │   │   ├── io.pressure
            │   │   │   ├── memory.current
            │   │   │   ├── memory.events
            │   │   │   ├── memory.events.local
            │   │   │   ├── memory.high
            │   │   │   ├── memory.low
            │   │   │   ├── memory.max
            │   │   │   ├── memory.min
            │   │   │   ├── memory.numa_stat
            │   │   │   ├── memory.oom.group
            │   │   │   ├── memory.peak
            │   │   │   ├── memory.pressure
            │   │   │   ├── memory.reclaim
            │   │   │   ├── memory.stat
            │   │   │   ├── memory.swap.current
            │   │   │   ├── memory.swap.events
            │   │   │   ├── memory.swap.high
            │   │   │   ├── memory.swap.max
            │   │   │   ├── memory.swap.peak
            │   │   │   ├── memory.zswap.current
            │   │   │   ├── memory.zswap.max
            │   │   │   ├── memory.zswap.writeback
            │   │   │   ├── pids.current
            │   │   │   ├── pids.events
            │   │   │   ├── pids.max
            │   │   │   └── pids.peak
            │   │   ├── io.pressure
            │   │   ├── memory.current
            │   │   ├── memory.events
            │   │   ├── memory.events.local
            │   │   ├── memory.high
            │   │   ├── memory.low
            │   │   ├── memory.max
            │   │   ├── memory.min
            │   │   ├── memory.numa_stat
            │   │   ├── memory.oom.group
            │   │   ├── memory.peak
            │   │   ├── memory.pressure
            │   │   ├── memory.reclaim
            │   │   ├── memory.stat
            │   │   ├── memory.swap.current
            │   │   ├── memory.swap.events
            │   │   ├── memory.swap.high
            │   │   ├── memory.swap.max
            │   │   ├── memory.swap.peak
            │   │   ├── memory.zswap.current
            │   │   ├── memory.zswap.max
            │   │   ├── memory.zswap.writeback
            │   │   ├── pids.current
            │   │   ├── pids.events
            │   │   ├── pids.max
            │   │   ├── pids.peak
            │   │   └── vte-spawn-a7337949-da3c-4a4b-91b9-91d049b0e291.scope
            │   │       ├── cgroup.controllers
            │   │       ├── cgroup.events
            │   │       ├── cgroup.freeze
            │   │       ├── cgroup.kill
            │   │       ├── cgroup.max.depth
            │   │       ├── cgroup.max.descendants
            │   │       ├── cgroup.pressure
            │   │       ├── cgroup.procs
            │   │       ├── cgroup.stat
            │   │       ├── cgroup.subtree_control
            │   │       ├── cgroup.threads
            │   │       ├── cgroup.type
            │   │       ├── cpu.pressure
            │   │       ├── cpu.stat
            │   │       ├── cpu.stat.local
            │   │       ├── io.pressure
            │   │       ├── memory.current
            │   │       ├── memory.events
            │   │       ├── memory.events.local
            │   │       ├── memory.high
            │   │       ├── memory.low
            │   │       ├── memory.max
            │   │       ├── memory.min
            │   │       ├── memory.numa_stat
            │   │       ├── memory.oom.group
            │   │       ├── memory.peak
            │   │       ├── memory.pressure
            │   │       ├── memory.reclaim
            │   │       ├── memory.stat
            │   │       ├── memory.swap.current
            │   │       ├── memory.swap.events
            │   │       ├── memory.swap.high
            │   │       ├── memory.swap.max
            │   │       ├── memory.swap.peak
            │   │       ├── memory.zswap.current
            │   │       ├── memory.zswap.max
            │   │       ├── memory.zswap.writeback
            │   │       ├── pids.current
            │   │       ├── pids.events
            │   │       ├── pids.max
            │   │       └── pids.peak
            │   ├── at-spi-dbus-bus.service
            │   │   ├── cgroup.controllers
            │   │   ├── cgroup.events
            │   │   ├── cgroup.freeze
            │   │   ├── cgroup.kill
            │   │   ├── cgroup.max.depth
            │   │   ├── cgroup.max.descendants
            │   │   ├── cgroup.pressure
            │   │   ├── cgroup.procs
            │   │   ├── cgroup.stat
            │   │   ├── cgroup.subtree_control
            │   │   ├── cgroup.threads
            │   │   ├── cgroup.type
            │   │   ├── cpu.pressure
            │   │   ├── cpu.stat
            │   │   ├── cpu.stat.local
            │   │   ├── io.pressure
            │   │   ├── memory.current
            │   │   ├── memory.events
            │   │   ├── memory.events.local
            │   │   ├── memory.high
            │   │   ├── memory.low
            │   │   ├── memory.max
            │   │   ├── memory.min
            │   │   ├── memory.numa_stat
            │   │   ├── memory.oom.group
            │   │   ├── memory.peak
            │   │   ├── memory.pressure
            │   │   ├── memory.reclaim
            │   │   ├── memory.stat
            │   │   ├── memory.swap.current
            │   │   ├── memory.swap.events
            │   │   ├── memory.swap.high
            │   │   ├── memory.swap.max
            │   │   ├── memory.swap.peak
            │   │   ├── memory.zswap.current
            │   │   ├── memory.zswap.max
            │   │   ├── memory.zswap.writeback
            │   │   ├── pids.current
            │   │   ├── pids.events
            │   │   ├── pids.max
            │   │   └── pids.peak
            │   ├── cgroup.controllers
            │   ├── cgroup.events
            │   ├── cgroup.freeze
            │   ├── cgroup.kill
            │   ├── cgroup.max.depth
            │   ├── cgroup.max.descendants
            │   ├── cgroup.pressure
            │   ├── cgroup.procs
            │   ├── cgroup.stat
            │   ├── cgroup.subtree_control
            │   ├── cgroup.threads
            │   ├── cgroup.type
            │   ├── cpu.pressure
            │   ├── cpu.stat
            │   ├── cpu.stat.local
            │   ├── dbus.service
            │   │   ├── cgroup.controllers
            │   │   ├── cgroup.events
            │   │   ├── cgroup.freeze
            │   │   ├── cgroup.kill
            │   │   ├── cgroup.max.depth
            │   │   ├── cgroup.max.descendants
            │   │   ├── cgroup.pressure
            │   │   ├── cgroup.procs
            │   │   ├── cgroup.stat
            │   │   ├── cgroup.subtree_control
            │   │   ├── cgroup.threads
            │   │   ├── cgroup.type
            │   │   ├── cpu.pressure
            │   │   ├── cpu.stat
            │   │   ├── cpu.stat.local
            │   │   ├── io.pressure
            │   │   ├── memory.current
            │   │   ├── memory.events
            │   │   ├── memory.events.local
            │   │   ├── memory.high
            │   │   ├── memory.low
            │   │   ├── memory.max
            │   │   ├── memory.min
            │   │   ├── memory.numa_stat
            │   │   ├── memory.oom.group
            │   │   ├── memory.peak
            │   │   ├── memory.pressure
            │   │   ├── memory.reclaim
            │   │   ├── memory.stat
            │   │   ├── memory.swap.current
            │   │   ├── memory.swap.events
            │   │   ├── memory.swap.high
            │   │   ├── memory.swap.max
            │   │   ├── memory.swap.peak
            │   │   ├── memory.zswap.current
            │   │   ├── memory.zswap.max
            │   │   ├── memory.zswap.writeback
            │   │   ├── pids.current
            │   │   ├── pids.events
            │   │   ├── pids.max
            │   │   └── pids.peak
            │   ├── dbus.socket
            │   │   ├── cgroup.controllers
            │   │   ├── cgroup.events
            │   │   ├── cgroup.freeze
            │   │   ├── cgroup.kill
            │   │   ├── cgroup.max.depth
            │   │   ├── cgroup.max.descendants
            │   │   ├── cgroup.pressure
            │   │   ├── cgroup.procs
            │   │   ├── cgroup.stat
            │   │   ├── cgroup.subtree_control
            │   │   ├── cgroup.threads
            │   │   ├── cgroup.type
            │   │   ├── cpu.pressure
            │   │   ├── cpu.stat
            │   │   ├── cpu.stat.local
            │   │   ├── io.pressure
            │   │   ├── memory.current
            │   │   ├── memory.events
            │   │   ├── memory.events.local
            │   │   ├── memory.high
            │   │   ├── memory.low
            │   │   ├── memory.max
            │   │   ├── memory.min
            │   │   ├── memory.numa_stat
            │   │   ├── memory.oom.group
            │   │   ├── memory.peak
            │   │   ├── memory.pressure
            │   │   ├── memory.reclaim
            │   │   ├── memory.stat
            │   │   ├── memory.swap.current
            │   │   ├── memory.swap.events
            │   │   ├── memory.swap.high
            │   │   ├── memory.swap.max
            │   │   ├── memory.swap.peak
            │   │   ├── memory.zswap.current
            │   │   ├── memory.zswap.max
            │   │   ├── memory.zswap.writeback
            │   │   ├── pids.current
            │   │   ├── pids.events
            │   │   ├── pids.max
            │   │   └── pids.peak
            │   ├── dconf.service
            │   │   ├── cgroup.controllers
            │   │   ├── cgroup.events
            │   │   ├── cgroup.freeze
            │   │   ├── cgroup.kill
            │   │   ├── cgroup.max.depth
            │   │   ├── cgroup.max.descendants
            │   │   ├── cgroup.pressure
            │   │   ├── cgroup.procs
            │   │   ├── cgroup.stat
            │   │   ├── cgroup.subtree_control
            │   │   ├── cgroup.threads
            │   │   ├── cgroup.type
            │   │   ├── cpu.pressure
            │   │   ├── cpu.stat
            │   │   ├── cpu.stat.local
            │   │   ├── io.pressure
            │   │   ├── memory.current
            │   │   ├── memory.events
            │   │   ├── memory.events.local
            │   │   ├── memory.high
            │   │   ├── memory.low
            │   │   ├── memory.max
            │   │   ├── memory.min
            │   │   ├── memory.numa_stat
            │   │   ├── memory.oom.group
            │   │   ├── memory.peak
            │   │   ├── memory.pressure
            │   │   ├── memory.reclaim
            │   │   ├── memory.stat
            │   │   ├── memory.swap.current
            │   │   ├── memory.swap.events
            │   │   ├── memory.swap.high
            │   │   ├── memory.swap.max
            │   │   ├── memory.swap.peak
            │   │   ├── memory.zswap.current
            │   │   ├── memory.zswap.max
            │   │   ├── memory.zswap.writeback
            │   │   ├── pids.current
            │   │   ├── pids.events
            │   │   ├── pids.max
            │   │   └── pids.peak
            │   ├── evolution-addressbook-factory.service
            │   │   ├── cgroup.controllers
            │   │   ├── cgroup.events
            │   │   ├── cgroup.freeze
            │   │   ├── cgroup.kill
            │   │   ├── cgroup.max.depth
            │   │   ├── cgroup.max.descendants
            │   │   ├── cgroup.pressure
            │   │   ├── cgroup.procs
            │   │   ├── cgroup.stat
            │   │   ├── cgroup.subtree_control
            │   │   ├── cgroup.threads
            │   │   ├── cgroup.type
            │   │   ├── cpu.pressure
            │   │   ├── cpu.stat
            │   │   ├── cpu.stat.local
            │   │   ├── io.pressure
            │   │   ├── memory.current
            │   │   ├── memory.events
            │   │   ├── memory.events.local
            │   │   ├── memory.high
            │   │   ├── memory.low
            │   │   ├── memory.max
            │   │   ├── memory.min
            │   │   ├── memory.numa_stat
            │   │   ├── memory.oom.group
            │   │   ├── memory.peak
            │   │   ├── memory.pressure
            │   │   ├── memory.reclaim
            │   │   ├── memory.stat
            │   │   ├── memory.swap.current
            │   │   ├── memory.swap.events
            │   │   ├── memory.swap.high
            │   │   ├── memory.swap.max
            │   │   ├── memory.swap.peak
            │   │   ├── memory.zswap.current
            │   │   ├── memory.zswap.max
            │   │   ├── memory.zswap.writeback
            │   │   ├── pids.current
            │   │   ├── pids.events
            │   │   ├── pids.max
            │   │   └── pids.peak
            │   ├── evolution-calendar-factory.service
            │   │   ├── cgroup.controllers
            │   │   ├── cgroup.events
            │   │   ├── cgroup.freeze
            │   │   ├── cgroup.kill
            │   │   ├── cgroup.max.depth
            │   │   ├── cgroup.max.descendants
            │   │   ├── cgroup.pressure
            │   │   ├── cgroup.procs
            │   │   ├── cgroup.stat
            │   │   ├── cgroup.subtree_control
            │   │   ├── cgroup.threads
            │   │   ├── cgroup.type
            │   │   ├── cpu.pressure
            │   │   ├── cpu.stat
            │   │   ├── cpu.stat.local
            │   │   ├── io.pressure
            │   │   ├── memory.current
            │   │   ├── memory.events
            │   │   ├── memory.events.local
            │   │   ├── memory.high
            │   │   ├── memory.low
            │   │   ├── memory.max
            │   │   ├── memory.min
            │   │   ├── memory.numa_stat
            │   │   ├── memory.oom.group
            │   │   ├── memory.peak
            │   │   ├── memory.pressure
            │   │   ├── memory.reclaim
            │   │   ├── memory.stat
            │   │   ├── memory.swap.current
            │   │   ├── memory.swap.events
            │   │   ├── memory.swap.high
            │   │   ├── memory.swap.max
            │   │   ├── memory.swap.peak
            │   │   ├── memory.zswap.current
            │   │   ├── memory.zswap.max
            │   │   ├── memory.zswap.writeback
            │   │   ├── pids.current
            │   │   ├── pids.events
            │   │   ├── pids.max
            │   │   └── pids.peak
            │   ├── evolution-source-registry.service
            │   │   ├── cgroup.controllers
            │   │   ├── cgroup.events
            │   │   ├── cgroup.freeze
            │   │   ├── cgroup.kill
            │   │   ├── cgroup.max.depth
            │   │   ├── cgroup.max.descendants
            │   │   ├── cgroup.pressure
            │   │   ├── cgroup.procs
            │   │   ├── cgroup.stat
            │   │   ├── cgroup.subtree_control
            │   │   ├── cgroup.threads
            │   │   ├── cgroup.type
            │   │   ├── cpu.pressure
            │   │   ├── cpu.stat
            │   │   ├── cpu.stat.local
            │   │   ├── io.pressure
            │   │   ├── memory.current
            │   │   ├── memory.events
            │   │   ├── memory.events.local
            │   │   ├── memory.high
            │   │   ├── memory.low
            │   │   ├── memory.max
            │   │   ├── memory.min
            │   │   ├── memory.numa_stat
            │   │   ├── memory.oom.group
            │   │   ├── memory.peak
            │   │   ├── memory.pressure
            │   │   ├── memory.reclaim
            │   │   ├── memory.stat
            │   │   ├── memory.swap.current
            │   │   ├── memory.swap.events
            │   │   ├── memory.swap.high
            │   │   ├── memory.swap.max
            │   │   ├── memory.swap.peak
            │   │   ├── memory.zswap.current
            │   │   ├── memory.zswap.max
            │   │   ├── memory.zswap.writeback
            │   │   ├── pids.current
            │   │   ├── pids.events
            │   │   ├── pids.max
            │   │   └── pids.peak
            │   ├── gnome-session-monitor.service
            │   │   ├── cgroup.controllers
            │   │   ├── cgroup.events
            │   │   ├── cgroup.freeze
            │   │   ├── cgroup.kill
            │   │   ├── cgroup.max.depth
            │   │   ├── cgroup.max.descendants
            │   │   ├── cgroup.pressure
            │   │   ├── cgroup.procs
            │   │   ├── cgroup.stat
            │   │   ├── cgroup.subtree_control
            │   │   ├── cgroup.threads
            │   │   ├── cgroup.type
            │   │   ├── cpu.pressure
            │   │   ├── cpu.stat
            │   │   ├── cpu.stat.local
            │   │   ├── io.pressure
            │   │   ├── memory.current
            │   │   ├── memory.events
            │   │   ├── memory.events.local
            │   │   ├── memory.high
            │   │   ├── memory.low
            │   │   ├── memory.max
            │   │   ├── memory.min
            │   │   ├── memory.numa_stat
            │   │   ├── memory.oom.group
            │   │   ├── memory.peak
            │   │   ├── memory.pressure
            │   │   ├── memory.reclaim
            │   │   ├── memory.stat
            │   │   ├── memory.swap.current
            │   │   ├── memory.swap.events
            │   │   ├── memory.swap.high
            │   │   ├── memory.swap.max
            │   │   ├── memory.swap.peak
            │   │   ├── memory.zswap.current
            │   │   ├── memory.zswap.max
            │   │   ├── memory.zswap.writeback
            │   │   ├── pids.current
            │   │   ├── pids.events
            │   │   ├── pids.max
            │   │   └── pids.peak
            │   ├── gvfs-afc-volume-monitor.service
            │   │   ├── cgroup.controllers
            │   │   ├── cgroup.events
            │   │   ├── cgroup.freeze
            │   │   ├── cgroup.kill
            │   │   ├── cgroup.max.depth
            │   │   ├── cgroup.max.descendants
            │   │   ├── cgroup.pressure
            │   │   ├── cgroup.procs
            │   │   ├── cgroup.stat
            │   │   ├── cgroup.subtree_control
            │   │   ├── cgroup.threads
            │   │   ├── cgroup.type
            │   │   ├── cpu.pressure
            │   │   ├── cpu.stat
            │   │   ├── cpu.stat.local
            │   │   ├── io.pressure
            │   │   ├── memory.current
            │   │   ├── memory.events
            │   │   ├── memory.events.local
            │   │   ├── memory.high
            │   │   ├── memory.low
            │   │   ├── memory.max
            │   │   ├── memory.min
            │   │   ├── memory.numa_stat
            │   │   ├── memory.oom.group
            │   │   ├── memory.peak
            │   │   ├── memory.pressure
            │   │   ├── memory.reclaim
            │   │   ├── memory.stat
            │   │   ├── memory.swap.current
            │   │   ├── memory.swap.events
            │   │   ├── memory.swap.high
            │   │   ├── memory.swap.max
            │   │   ├── memory.swap.peak
            │   │   ├── memory.zswap.current
            │   │   ├── memory.zswap.max
            │   │   ├── memory.zswap.writeback
            │   │   ├── pids.current
            │   │   ├── pids.events
            │   │   ├── pids.max
            │   │   └── pids.peak
            │   ├── gvfs-daemon.service
            │   │   ├── cgroup.controllers
            │   │   ├── cgroup.events
            │   │   ├── cgroup.freeze
            │   │   ├── cgroup.kill
            │   │   ├── cgroup.max.depth
            │   │   ├── cgroup.max.descendants
            │   │   ├── cgroup.pressure
            │   │   ├── cgroup.procs
            │   │   ├── cgroup.stat
            │   │   ├── cgroup.subtree_control
            │   │   ├── cgroup.threads
            │   │   ├── cgroup.type
            │   │   ├── cpu.pressure
            │   │   ├── cpu.stat
            │   │   ├── cpu.stat.local
            │   │   ├── io.pressure
            │   │   ├── memory.current
            │   │   ├── memory.events
            │   │   ├── memory.events.local
            │   │   ├── memory.high
            │   │   ├── memory.low
            │   │   ├── memory.max
            │   │   ├── memory.min
            │   │   ├── memory.numa_stat
            │   │   ├── memory.oom.group
            │   │   ├── memory.peak
            │   │   ├── memory.pressure
            │   │   ├── memory.reclaim
            │   │   ├── memory.stat
            │   │   ├── memory.swap.current
            │   │   ├── memory.swap.events
            │   │   ├── memory.swap.high
            │   │   ├── memory.swap.max
            │   │   ├── memory.swap.peak
            │   │   ├── memory.zswap.current
            │   │   ├── memory.zswap.max
            │   │   ├── memory.zswap.writeback
            │   │   ├── pids.current
            │   │   ├── pids.events
            │   │   ├── pids.max
            │   │   └── pids.peak
            │   ├── gvfs-goa-volume-monitor.service
            │   │   ├── cgroup.controllers
            │   │   ├── cgroup.events
            │   │   ├── cgroup.freeze
            │   │   ├── cgroup.kill
            │   │   ├── cgroup.max.depth
            │   │   ├── cgroup.max.descendants
            │   │   ├── cgroup.pressure
            │   │   ├── cgroup.procs
            │   │   ├── cgroup.stat
            │   │   ├── cgroup.subtree_control
            │   │   ├── cgroup.threads
            │   │   ├── cgroup.type
            │   │   ├── cpu.pressure
            │   │   ├── cpu.stat
            │   │   ├── cpu.stat.local
            │   │   ├── io.pressure
            │   │   ├── memory.current
            │   │   ├── memory.events
            │   │   ├── memory.events.local
            │   │   ├── memory.high
            │   │   ├── memory.low
            │   │   ├── memory.max
            │   │   ├── memory.min
            │   │   ├── memory.numa_stat
            │   │   ├── memory.oom.group
            │   │   ├── memory.peak
            │   │   ├── memory.pressure
            │   │   ├── memory.reclaim
            │   │   ├── memory.stat
            │   │   ├── memory.swap.current
            │   │   ├── memory.swap.events
            │   │   ├── memory.swap.high
            │   │   ├── memory.swap.max
            │   │   ├── memory.swap.peak
            │   │   ├── memory.zswap.current
            │   │   ├── memory.zswap.max
            │   │   ├── memory.zswap.writeback
            │   │   ├── pids.current
            │   │   ├── pids.events
            │   │   ├── pids.max
            │   │   └── pids.peak
            │   ├── gvfs-gphoto2-volume-monitor.service
            │   │   ├── cgroup.controllers
            │   │   ├── cgroup.events
            │   │   ├── cgroup.freeze
            │   │   ├── cgroup.kill
            │   │   ├── cgroup.max.depth
            │   │   ├── cgroup.max.descendants
            │   │   ├── cgroup.pressure
            │   │   ├── cgroup.procs
            │   │   ├── cgroup.stat
            │   │   ├── cgroup.subtree_control
            │   │   ├── cgroup.threads
            │   │   ├── cgroup.type
            │   │   ├── cpu.pressure
            │   │   ├── cpu.stat
            │   │   ├── cpu.stat.local
            │   │   ├── io.pressure
            │   │   ├── memory.current
            │   │   ├── memory.events
            │   │   ├── memory.events.local
            │   │   ├── memory.high
            │   │   ├── memory.low
            │   │   ├── memory.max
            │   │   ├── memory.min
            │   │   ├── memory.numa_stat
            │   │   ├── memory.oom.group
            │   │   ├── memory.peak
            │   │   ├── memory.pressure
            │   │   ├── memory.reclaim
            │   │   ├── memory.stat
            │   │   ├── memory.swap.current
            │   │   ├── memory.swap.events
            │   │   ├── memory.swap.high
            │   │   ├── memory.swap.max
            │   │   ├── memory.swap.peak
            │   │   ├── memory.zswap.current
            │   │   ├── memory.zswap.max
            │   │   ├── memory.zswap.writeback
            │   │   ├── pids.current
            │   │   ├── pids.events
            │   │   ├── pids.max
            │   │   └── pids.peak
            │   ├── gvfs-metadata.service
            │   │   ├── cgroup.controllers
            │   │   ├── cgroup.events
            │   │   ├── cgroup.freeze
            │   │   ├── cgroup.kill
            │   │   ├── cgroup.max.depth
            │   │   ├── cgroup.max.descendants
            │   │   ├── cgroup.pressure
            │   │   ├── cgroup.procs
            │   │   ├── cgroup.stat
            │   │   ├── cgroup.subtree_control
            │   │   ├── cgroup.threads
            │   │   ├── cgroup.type
            │   │   ├── cpu.pressure
            │   │   ├── cpu.stat
            │   │   ├── cpu.stat.local
            │   │   ├── io.pressure
            │   │   ├── memory.current
            │   │   ├── memory.events
            │   │   ├── memory.events.local
            │   │   ├── memory.high
            │   │   ├── memory.low
            │   │   ├── memory.max
            │   │   ├── memory.min
            │   │   ├── memory.numa_stat
            │   │   ├── memory.oom.group
            │   │   ├── memory.peak
            │   │   ├── memory.pressure
            │   │   ├── memory.reclaim
            │   │   ├── memory.stat
            │   │   ├── memory.swap.current
            │   │   ├── memory.swap.events
            │   │   ├── memory.swap.high
            │   │   ├── memory.swap.max
            │   │   ├── memory.swap.peak
            │   │   ├── memory.zswap.current
            │   │   ├── memory.zswap.max
            │   │   ├── memory.zswap.writeback
            │   │   ├── pids.current
            │   │   ├── pids.events
            │   │   ├── pids.max
            │   │   └── pids.peak
            │   ├── gvfs-mtp-volume-monitor.service
            │   │   ├── cgroup.controllers
            │   │   ├── cgroup.events
            │   │   ├── cgroup.freeze
            │   │   ├── cgroup.kill
            │   │   ├── cgroup.max.depth
            │   │   ├── cgroup.max.descendants
            │   │   ├── cgroup.pressure
            │   │   ├── cgroup.procs
            │   │   ├── cgroup.stat
            │   │   ├── cgroup.subtree_control
            │   │   ├── cgroup.threads
            │   │   ├── cgroup.type
            │   │   ├── cpu.pressure
            │   │   ├── cpu.stat
            │   │   ├── cpu.stat.local
            │   │   ├── io.pressure
            │   │   ├── memory.current
            │   │   ├── memory.events
            │   │   ├── memory.events.local
            │   │   ├── memory.high
            │   │   ├── memory.low
            │   │   ├── memory.max
            │   │   ├── memory.min
            │   │   ├── memory.numa_stat
            │   │   ├── memory.oom.group
            │   │   ├── memory.peak
            │   │   ├── memory.pressure
            │   │   ├── memory.reclaim
            │   │   ├── memory.stat
            │   │   ├── memory.swap.current
            │   │   ├── memory.swap.events
            │   │   ├── memory.swap.high
            │   │   ├── memory.swap.max
            │   │   ├── memory.swap.peak
            │   │   ├── memory.zswap.current
            │   │   ├── memory.zswap.max
            │   │   ├── memory.zswap.writeback
            │   │   ├── pids.current
            │   │   ├── pids.events
            │   │   ├── pids.max
            │   │   └── pids.peak
            │   ├── gvfs-udisks2-volume-monitor.service
            │   │   ├── cgroup.controllers
            │   │   ├── cgroup.events
            │   │   ├── cgroup.freeze
            │   │   ├── cgroup.kill
            │   │   ├── cgroup.max.depth
            │   │   ├── cgroup.max.descendants
            │   │   ├── cgroup.pressure
            │   │   ├── cgroup.procs
            │   │   ├── cgroup.stat
            │   │   ├── cgroup.subtree_control
            │   │   ├── cgroup.threads
            │   │   ├── cgroup.type
            │   │   ├── cpu.pressure
            │   │   ├── cpu.stat
            │   │   ├── cpu.stat.local
            │   │   ├── io.pressure
            │   │   ├── memory.current
            │   │   ├── memory.events
            │   │   ├── memory.events.local
            │   │   ├── memory.high
            │   │   ├── memory.low
            │   │   ├── memory.max
            │   │   ├── memory.min
            │   │   ├── memory.numa_stat
            │   │   ├── memory.oom.group
            │   │   ├── memory.peak
            │   │   ├── memory.pressure
            │   │   ├── memory.reclaim
            │   │   ├── memory.stat
            │   │   ├── memory.swap.current
            │   │   ├── memory.swap.events
            │   │   ├── memory.swap.high
            │   │   ├── memory.swap.max
            │   │   ├── memory.swap.peak
            │   │   ├── memory.zswap.current
            │   │   ├── memory.zswap.max
            │   │   ├── memory.zswap.writeback
            │   │   ├── pids.current
            │   │   ├── pids.events
            │   │   ├── pids.max
            │   │   └── pids.peak
            │   ├── io.pressure
            │   ├── memory.current
            │   ├── memory.events
            │   ├── memory.events.local
            │   ├── memory.high
            │   ├── memory.low
            │   ├── memory.max
            │   ├── memory.min
            │   ├── memory.numa_stat
            │   ├── memory.oom.group
            │   ├── memory.peak
            │   ├── memory.pressure
            │   ├── memory.reclaim
            │   ├── memory.stat
            │   ├── memory.swap.current
            │   ├── memory.swap.events
            │   ├── memory.swap.high
            │   ├── memory.swap.max
            │   ├── memory.swap.peak
            │   ├── memory.zswap.current
            │   ├── memory.zswap.max
            │   ├── memory.zswap.writeback
            │   ├── pids.current
            │   ├── pids.events
            │   ├── pids.max
            │   ├── pids.peak
            │   ├── snap.firefox.firefox-3d544c0f-d72d-4292-a311-2c130e8e58b7.scope
            │   │   ├── cgroup.controllers
            │   │   ├── cgroup.events
            │   │   ├── cgroup.freeze
            │   │   ├── cgroup.kill
            │   │   ├── cgroup.max.depth
            │   │   ├── cgroup.max.descendants
            │   │   ├── cgroup.pressure
            │   │   ├── cgroup.procs
            │   │   ├── cgroup.stat
            │   │   ├── cgroup.subtree_control
            │   │   ├── cgroup.threads
            │   │   ├── cgroup.type
            │   │   ├── cpu.pressure
            │   │   ├── cpu.stat
            │   │   ├── cpu.stat.local
            │   │   ├── io.pressure
            │   │   ├── memory.current
            │   │   ├── memory.events
            │   │   ├── memory.events.local
            │   │   ├── memory.high
            │   │   ├── memory.low
            │   │   ├── memory.max
            │   │   ├── memory.min
            │   │   ├── memory.numa_stat
            │   │   ├── memory.oom.group
            │   │   ├── memory.peak
            │   │   ├── memory.pressure
            │   │   ├── memory.reclaim
            │   │   ├── memory.stat
            │   │   ├── memory.swap.current
            │   │   ├── memory.swap.events
            │   │   ├── memory.swap.high
            │   │   ├── memory.swap.max
            │   │   ├── memory.swap.peak
            │   │   ├── memory.zswap.current
            │   │   ├── memory.zswap.max
            │   │   ├── memory.zswap.writeback
            │   │   ├── pids.current
            │   │   ├── pids.events
            │   │   ├── pids.max
            │   │   └── pids.peak
            │   ├── snap.snapd-desktop-integration.snapd-desktop-integration.service
            │   │   ├── cgroup.controllers
            │   │   ├── cgroup.events
            │   │   ├── cgroup.freeze
            │   │   ├── cgroup.kill
            │   │   ├── cgroup.max.depth
            │   │   ├── cgroup.max.descendants
            │   │   ├── cgroup.pressure
            │   │   ├── cgroup.procs
            │   │   ├── cgroup.stat
            │   │   ├── cgroup.subtree_control
            │   │   ├── cgroup.threads
            │   │   ├── cgroup.type
            │   │   ├── cpu.pressure
            │   │   ├── cpu.stat
            │   │   ├── cpu.stat.local
            │   │   ├── io.pressure
            │   │   ├── memory.current
            │   │   ├── memory.events
            │   │   ├── memory.events.local
            │   │   ├── memory.high
            │   │   ├── memory.low
            │   │   ├── memory.max
            │   │   ├── memory.min
            │   │   ├── memory.numa_stat
            │   │   ├── memory.oom.group
            │   │   ├── memory.peak
            │   │   ├── memory.pressure
            │   │   ├── memory.reclaim
            │   │   ├── memory.stat
            │   │   ├── memory.swap.current
            │   │   ├── memory.swap.events
            │   │   ├── memory.swap.high
            │   │   ├── memory.swap.max
            │   │   ├── memory.swap.peak
            │   │   ├── memory.zswap.current
            │   │   ├── memory.zswap.max
            │   │   ├── memory.zswap.writeback
            │   │   ├── pids.current
            │   │   ├── pids.events
            │   │   ├── pids.max
            │   │   └── pids.peak
            │   ├── snap.snap-store.ubuntu-software-371f0ddc-80d8-42ce-a73d-0a673917b9d4.scope
            │   │   ├── cgroup.controllers
            │   │   ├── cgroup.events
            │   │   ├── cgroup.freeze
            │   │   ├── cgroup.kill
            │   │   ├── cgroup.max.depth
            │   │   ├── cgroup.max.descendants
            │   │   ├── cgroup.pressure
            │   │   ├── cgroup.procs
            │   │   ├── cgroup.stat
            │   │   ├── cgroup.subtree_control
            │   │   ├── cgroup.threads
            │   │   ├── cgroup.type
            │   │   ├── cpu.pressure
            │   │   ├── cpu.stat
            │   │   ├── cpu.stat.local
            │   │   ├── io.pressure
            │   │   ├── memory.current
            │   │   ├── memory.events
            │   │   ├── memory.events.local
            │   │   ├── memory.high
            │   │   ├── memory.low
            │   │   ├── memory.max
            │   │   ├── memory.min
            │   │   ├── memory.numa_stat
            │   │   ├── memory.oom.group
            │   │   ├── memory.peak
            │   │   ├── memory.pressure
            │   │   ├── memory.reclaim
            │   │   ├── memory.stat
            │   │   ├── memory.swap.current
            │   │   ├── memory.swap.events
            │   │   ├── memory.swap.high
            │   │   ├── memory.swap.max
            │   │   ├── memory.swap.peak
            │   │   ├── memory.zswap.current
            │   │   ├── memory.zswap.max
            │   │   ├── memory.zswap.writeback
            │   │   ├── pids.current
            │   │   ├── pids.events
            │   │   ├── pids.max
            │   │   └── pids.peak
            │   ├── speech-dispatcher.service
            │   │   ├── cgroup.controllers
            │   │   ├── cgroup.events
            │   │   ├── cgroup.freeze
            │   │   ├── cgroup.kill
            │   │   ├── cgroup.max.depth
            │   │   ├── cgroup.max.descendants
            │   │   ├── cgroup.pressure
            │   │   ├── cgroup.procs
            │   │   ├── cgroup.stat
            │   │   ├── cgroup.subtree_control
            │   │   ├── cgroup.threads
            │   │   ├── cgroup.type
            │   │   ├── cpu.pressure
            │   │   ├── cpu.stat
            │   │   ├── cpu.stat.local
            │   │   ├── io.pressure
            │   │   ├── memory.current
            │   │   ├── memory.events
            │   │   ├── memory.events.local
            │   │   ├── memory.high
            │   │   ├── memory.low
            │   │   ├── memory.max
            │   │   ├── memory.min
            │   │   ├── memory.numa_stat
            │   │   ├── memory.oom.group
            │   │   ├── memory.peak
            │   │   ├── memory.pressure
            │   │   ├── memory.reclaim
            │   │   ├── memory.stat
            │   │   ├── memory.swap.current
            │   │   ├── memory.swap.events
            │   │   ├── memory.swap.high
            │   │   ├── memory.swap.max
            │   │   ├── memory.swap.peak
            │   │   ├── memory.zswap.current
            │   │   ├── memory.zswap.max
            │   │   ├── memory.zswap.writeback
            │   │   ├── pids.current
            │   │   ├── pids.events
            │   │   ├── pids.max
            │   │   └── pids.peak
            │   ├── xdg-desktop-portal-gnome.service
            │   │   ├── cgroup.controllers
            │   │   ├── cgroup.events
            │   │   ├── cgroup.freeze
            │   │   ├── cgroup.kill
            │   │   ├── cgroup.max.depth
            │   │   ├── cgroup.max.descendants
            │   │   ├── cgroup.pressure
            │   │   ├── cgroup.procs
            │   │   ├── cgroup.stat
            │   │   ├── cgroup.subtree_control
            │   │   ├── cgroup.threads
            │   │   ├── cgroup.type
            │   │   ├── cpu.pressure
            │   │   ├── cpu.stat
            │   │   ├── cpu.stat.local
            │   │   ├── io.pressure
            │   │   ├── memory.current
            │   │   ├── memory.events
            │   │   ├── memory.events.local
            │   │   ├── memory.high
            │   │   ├── memory.low
            │   │   ├── memory.max
            │   │   ├── memory.min
            │   │   ├── memory.numa_stat
            │   │   ├── memory.oom.group
            │   │   ├── memory.peak
            │   │   ├── memory.pressure
            │   │   ├── memory.reclaim
            │   │   ├── memory.stat
            │   │   ├── memory.swap.current
            │   │   ├── memory.swap.events
            │   │   ├── memory.swap.high
            │   │   ├── memory.swap.max
            │   │   ├── memory.swap.peak
            │   │   ├── memory.zswap.current
            │   │   ├── memory.zswap.max
            │   │   ├── memory.zswap.writeback
            │   │   ├── pids.current
            │   │   ├── pids.events
            │   │   ├── pids.max
            │   │   └── pids.peak
            │   ├── xdg-desktop-portal-gtk.service
            │   │   ├── cgroup.controllers
            │   │   ├── cgroup.events
            │   │   ├── cgroup.freeze
            │   │   ├── cgroup.kill
            │   │   ├── cgroup.max.depth
            │   │   ├── cgroup.max.descendants
            │   │   ├── cgroup.pressure
            │   │   ├── cgroup.procs
            │   │   ├── cgroup.stat
            │   │   ├── cgroup.subtree_control
            │   │   ├── cgroup.threads
            │   │   ├── cgroup.type
            │   │   ├── cpu.pressure
            │   │   ├── cpu.stat
            │   │   ├── cpu.stat.local
            │   │   ├── io.pressure
            │   │   ├── memory.current
            │   │   ├── memory.events
            │   │   ├── memory.events.local
            │   │   ├── memory.high
            │   │   ├── memory.low
            │   │   ├── memory.max
            │   │   ├── memory.min
            │   │   ├── memory.numa_stat
            │   │   ├── memory.oom.group
            │   │   ├── memory.peak
            │   │   ├── memory.pressure
            │   │   ├── memory.reclaim
            │   │   ├── memory.stat
            │   │   ├── memory.swap.current
            │   │   ├── memory.swap.events
            │   │   ├── memory.swap.high
            │   │   ├── memory.swap.max
            │   │   ├── memory.swap.peak
            │   │   ├── memory.zswap.current
            │   │   ├── memory.zswap.max
            │   │   ├── memory.zswap.writeback
            │   │   ├── pids.current
            │   │   ├── pids.events
            │   │   ├── pids.max
            │   │   └── pids.peak
            │   └── xdg-permission-store.service
            │       ├── cgroup.controllers
            │       ├── cgroup.events
            │       ├── cgroup.freeze
            │       ├── cgroup.kill
            │       ├── cgroup.max.depth
            │       ├── cgroup.max.descendants
            │       ├── cgroup.pressure
            │       ├── cgroup.procs
            │       ├── cgroup.stat
            │       ├── cgroup.subtree_control
            │       ├── cgroup.threads
            │       ├── cgroup.type
            │       ├── cpu.pressure
            │       ├── cpu.stat
            │       ├── cpu.stat.local
            │       ├── io.pressure
            │       ├── memory.current
            │       ├── memory.events
            │       ├── memory.events.local
            │       ├── memory.high
            │       ├── memory.low
            │       ├── memory.max
            │       ├── memory.min
            │       ├── memory.numa_stat
            │       ├── memory.oom.group
            │       ├── memory.peak
            │       ├── memory.pressure
            │       ├── memory.reclaim
            │       ├── memory.stat
            │       ├── memory.swap.current
            │       ├── memory.swap.events
            │       ├── memory.swap.high
            │       ├── memory.swap.max
            │       ├── memory.swap.peak
            │       ├── memory.zswap.current
            │       ├── memory.zswap.max
            │       ├── memory.zswap.writeback
            │       ├── pids.current
            │       ├── pids.events
            │       ├── pids.max
            │       └── pids.peak
            ├── background.slice
            │   ├── cgroup.controllers
            │   ├── cgroup.events
            │   ├── cgroup.freeze
            │   ├── cgroup.kill
            │   ├── cgroup.max.depth
            │   ├── cgroup.max.descendants
            │   ├── cgroup.pressure
            │   ├── cgroup.procs
            │   ├── cgroup.stat
            │   ├── cgroup.subtree_control
            │   ├── cgroup.threads
            │   ├── cgroup.type
            │   ├── cpu.pressure
            │   ├── cpu.stat
            │   ├── cpu.stat.local
            │   ├── io.pressure
            │   ├── memory.current
            │   ├── memory.events
            │   ├── memory.events.local
            │   ├── memory.high
            │   ├── memory.low
            │   ├── memory.max
            │   ├── memory.min
            │   ├── memory.numa_stat
            │   ├── memory.oom.group
            │   ├── memory.peak
            │   ├── memory.pressure
            │   ├── memory.reclaim
            │   ├── memory.stat
            │   ├── memory.swap.current
            │   ├── memory.swap.events
            │   ├── memory.swap.high
            │   ├── memory.swap.max
            │   ├── memory.swap.peak
            │   ├── memory.zswap.current
            │   ├── memory.zswap.max
            │   ├── memory.zswap.writeback
            │   ├── pids.current
            │   ├── pids.events
            │   ├── pids.max
            │   ├── pids.peak
            │   └── tracker-miner-fs-3.service
            │       ├── cgroup.controllers
            │       ├── cgroup.events
            │       ├── cgroup.freeze
            │       ├── cgroup.kill
            │       ├── cgroup.max.depth
            │       ├── cgroup.max.descendants
            │       ├── cgroup.pressure
            │       ├── cgroup.procs
            │       ├── cgroup.stat
            │       ├── cgroup.subtree_control
            │       ├── cgroup.threads
            │       ├── cgroup.type
            │       ├── cpu.pressure
            │       ├── cpu.stat
            │       ├── cpu.stat.local
            │       ├── io.pressure
            │       ├── memory.current
            │       ├── memory.events
            │       ├── memory.events.local
            │       ├── memory.high
            │       ├── memory.low
            │       ├── memory.max
            │       ├── memory.min
            │       ├── memory.numa_stat
            │       ├── memory.oom.group
            │       ├── memory.peak
            │       ├── memory.pressure
            │       ├── memory.reclaim
            │       ├── memory.stat
            │       ├── memory.swap.current
            │       ├── memory.swap.events
            │       ├── memory.swap.high
            │       ├── memory.swap.max
            │       ├── memory.swap.peak
            │       ├── memory.zswap.current
            │       ├── memory.zswap.max
            │       ├── memory.zswap.writeback
            │       ├── pids.current
            │       ├── pids.events
            │       ├── pids.max
            │       └── pids.peak
            ├── cgroup.controllers
            ├── cgroup.events
            ├── cgroup.freeze
            ├── cgroup.kill
            ├── cgroup.max.depth
            ├── cgroup.max.descendants
            ├── cgroup.pressure
            ├── cgroup.procs
            ├── cgroup.stat
            ├── cgroup.subtree_control
            ├── cgroup.threads
            ├── cgroup.type
            ├── cpu.pressure
            ├── cpu.stat
            ├── cpu.stat.local
            ├── init.scope
            │   ├── cgroup.controllers
            │   ├── cgroup.events
            │   ├── cgroup.freeze
            │   ├── cgroup.kill
            │   ├── cgroup.max.depth
            │   ├── cgroup.max.descendants
            │   ├── cgroup.pressure
            │   ├── cgroup.procs
            │   ├── cgroup.stat
            │   ├── cgroup.subtree_control
            │   ├── cgroup.threads
            │   ├── cgroup.type
            │   ├── cpu.pressure
            │   ├── cpu.stat
            │   ├── cpu.stat.local
            │   ├── io.pressure
            │   ├── memory.current
            │   ├── memory.events
            │   ├── memory.events.local
            │   ├── memory.high
            │   ├── memory.low
            │   ├── memory.max
            │   ├── memory.min
            │   ├── memory.numa_stat
            │   ├── memory.oom.group
            │   ├── memory.peak
            │   ├── memory.pressure
            │   ├── memory.reclaim
            │   ├── memory.stat
            │   ├── memory.swap.current
            │   ├── memory.swap.events
            │   ├── memory.swap.high
            │   ├── memory.swap.max
            │   ├── memory.swap.peak
            │   ├── memory.zswap.current
            │   ├── memory.zswap.max
            │   ├── memory.zswap.writeback
            │   ├── pids.current
            │   ├── pids.events
            │   ├── pids.max
            │   └── pids.peak
            ├── io.pressure
            ├── memory.current
            ├── memory.events
            ├── memory.events.local
            ├── memory.high
            ├── memory.low
            ├── memory.max
            ├── memory.min
            ├── memory.numa_stat
            ├── memory.oom.group
            ├── memory.peak
            ├── memory.pressure
            ├── memory.reclaim
            ├── memory.stat
            ├── memory.swap.current
            ├── memory.swap.events
            ├── memory.swap.high
            ├── memory.swap.max
            ├── memory.swap.peak
            ├── memory.zswap.current
            ├── memory.zswap.max
            ├── memory.zswap.writeback
            ├── pids.current
            ├── pids.events
            ├── pids.max
            ├── pids.peak
            └── session.slice
                ├── cgroup.controllers
                ├── cgroup.events
                ├── cgroup.freeze
                ├── cgroup.kill
                ├── cgroup.max.depth
                ├── cgroup.max.descendants
                ├── cgroup.pressure
                ├── cgroup.procs
                ├── cgroup.stat
                ├── cgroup.subtree_control
                ├── cgroup.threads
                ├── cgroup.type
                ├── cpu.pressure
                ├── cpu.stat
                ├── cpu.stat.local
                ├── io.pressure
                ├── memory.current
                ├── memory.events
                ├── memory.events.local
                ├── memory.high
                ├── memory.low
                ├── memory.max
                ├── memory.min
                ├── memory.numa_stat
                ├── memory.oom.group
                ├── memory.peak
                ├── memory.pressure
                ├── memory.reclaim
                ├── memory.stat
                ├── memory.swap.current
                ├── memory.swap.events
                ├── memory.swap.high
                ├── memory.swap.max
                ├── memory.swap.peak
                ├── memory.zswap.current
                ├── memory.zswap.max
                ├── memory.zswap.writeback
                ├── org.freedesktop.IBus.session.GNOME.service
                │   ├── cgroup.controllers
                │   ├── cgroup.events
                │   ├── cgroup.freeze
                │   ├── cgroup.kill
                │   ├── cgroup.max.depth
                │   ├── cgroup.max.descendants
                │   ├── cgroup.pressure
                │   ├── cgroup.procs
                │   ├── cgroup.stat
                │   ├── cgroup.subtree_control
                │   ├── cgroup.threads
                │   ├── cgroup.type
                │   ├── cpu.pressure
                │   ├── cpu.stat
                │   ├── cpu.stat.local
                │   ├── io.pressure
                │   ├── memory.current
                │   ├── memory.events
                │   ├── memory.events.local
                │   ├── memory.high
                │   ├── memory.low
                │   ├── memory.max
                │   ├── memory.min
                │   ├── memory.numa_stat
                │   ├── memory.oom.group
                │   ├── memory.peak
                │   ├── memory.pressure
                │   ├── memory.reclaim
                │   ├── memory.stat
                │   ├── memory.swap.current
                │   ├── memory.swap.events
                │   ├── memory.swap.high
                │   ├── memory.swap.max
                │   ├── memory.swap.peak
                │   ├── memory.zswap.current
                │   ├── memory.zswap.max
                │   ├── memory.zswap.writeback
                │   ├── pids.current
                │   ├── pids.events
                │   ├── pids.max
                │   └── pids.peak
                ├── org.gnome.SettingsDaemon.A11ySettings.service
                │   ├── cgroup.controllers
                │   ├── cgroup.events
                │   ├── cgroup.freeze
                │   ├── cgroup.kill
                │   ├── cgroup.max.depth
                │   ├── cgroup.max.descendants
                │   ├── cgroup.pressure
                │   ├── cgroup.procs
                │   ├── cgroup.stat
                │   ├── cgroup.subtree_control
                │   ├── cgroup.threads
                │   ├── cgroup.type
                │   ├── cpu.pressure
                │   ├── cpu.stat
                │   ├── cpu.stat.local
                │   ├── io.pressure
                │   ├── memory.current
                │   ├── memory.events
                │   ├── memory.events.local
                │   ├── memory.high
                │   ├── memory.low
                │   ├── memory.max
                │   ├── memory.min
                │   ├── memory.numa_stat
                │   ├── memory.oom.group
                │   ├── memory.peak
                │   ├── memory.pressure
                │   ├── memory.reclaim
                │   ├── memory.stat
                │   ├── memory.swap.current
                │   ├── memory.swap.events
                │   ├── memory.swap.high
                │   ├── memory.swap.max
                │   ├── memory.swap.peak
                │   ├── memory.zswap.current
                │   ├── memory.zswap.max
                │   ├── memory.zswap.writeback
                │   ├── pids.current
                │   ├── pids.events
                │   ├── pids.max
                │   └── pids.peak
                ├── org.gnome.SettingsDaemon.Color.service
                │   ├── cgroup.controllers
                │   ├── cgroup.events
                │   ├── cgroup.freeze
                │   ├── cgroup.kill
                │   ├── cgroup.max.depth
                │   ├── cgroup.max.descendants
                │   ├── cgroup.pressure
                │   ├── cgroup.procs
                │   ├── cgroup.stat
                │   ├── cgroup.subtree_control
                │   ├── cgroup.threads
                │   ├── cgroup.type
                │   ├── cpu.pressure
                │   ├── cpu.stat
                │   ├── cpu.stat.local
                │   ├── io.pressure
                │   ├── memory.current
                │   ├── memory.events
                │   ├── memory.events.local
                │   ├── memory.high
                │   ├── memory.low
                │   ├── memory.max
                │   ├── memory.min
                │   ├── memory.numa_stat
                │   ├── memory.oom.group
                │   ├── memory.peak
                │   ├── memory.pressure
                │   ├── memory.reclaim
                │   ├── memory.stat
                │   ├── memory.swap.current
                │   ├── memory.swap.events
                │   ├── memory.swap.high
                │   ├── memory.swap.max
                │   ├── memory.swap.peak
                │   ├── memory.zswap.current
                │   ├── memory.zswap.max
                │   ├── memory.zswap.writeback
                │   ├── pids.current
                │   ├── pids.events
                │   ├── pids.max
                │   └── pids.peak
                ├── org.gnome.SettingsDaemon.Datetime.service
                │   ├── cgroup.controllers
                │   ├── cgroup.events
                │   ├── cgroup.freeze
                │   ├── cgroup.kill
                │   ├── cgroup.max.depth
                │   ├── cgroup.max.descendants
                │   ├── cgroup.pressure
                │   ├── cgroup.procs
                │   ├── cgroup.stat
                │   ├── cgroup.subtree_control
                │   ├── cgroup.threads
                │   ├── cgroup.type
                │   ├── cpu.pressure
                │   ├── cpu.stat
                │   ├── cpu.stat.local
                │   ├── io.pressure
                │   ├── memory.current
                │   ├── memory.events
                │   ├── memory.events.local
                │   ├── memory.high
                │   ├── memory.low
                │   ├── memory.max
                │   ├── memory.min
                │   ├── memory.numa_stat
                │   ├── memory.oom.group
                │   ├── memory.peak
                │   ├── memory.pressure
                │   ├── memory.reclaim
                │   ├── memory.stat
                │   ├── memory.swap.current
                │   ├── memory.swap.events
                │   ├── memory.swap.high
                │   ├── memory.swap.max
                │   ├── memory.swap.peak
                │   ├── memory.zswap.current
                │   ├── memory.zswap.max
                │   ├── memory.zswap.writeback
                │   ├── pids.current
                │   ├── pids.events
                │   ├── pids.max
                │   └── pids.peak
                ├── org.gnome.SettingsDaemon.Housekeeping.service
                │   ├── cgroup.controllers
                │   ├── cgroup.events
                │   ├── cgroup.freeze
                │   ├── cgroup.kill
                │   ├── cgroup.max.depth
                │   ├── cgroup.max.descendants
                │   ├── cgroup.pressure
                │   ├── cgroup.procs
                │   ├── cgroup.stat
                │   ├── cgroup.subtree_control
                │   ├── cgroup.threads
                │   ├── cgroup.type
                │   ├── cpu.pressure
                │   ├── cpu.stat
                │   ├── cpu.stat.local
                │   ├── io.pressure
                │   ├── memory.current
                │   ├── memory.events
                │   ├── memory.events.local
                │   ├── memory.high
                │   ├── memory.low
                │   ├── memory.max
                │   ├── memory.min
                │   ├── memory.numa_stat
                │   ├── memory.oom.group
                │   ├── memory.peak
                │   ├── memory.pressure
                │   ├── memory.reclaim
                │   ├── memory.stat
                │   ├── memory.swap.current
                │   ├── memory.swap.events
                │   ├── memory.swap.high
                │   ├── memory.swap.max
                │   ├── memory.swap.peak
                │   ├── memory.zswap.current
                │   ├── memory.zswap.max
                │   ├── memory.zswap.writeback
                │   ├── pids.current
                │   ├── pids.events
                │   ├── pids.max
                │   └── pids.peak
                ├── org.gnome.SettingsDaemon.Keyboard.service
                │   ├── cgroup.controllers
                │   ├── cgroup.events
                │   ├── cgroup.freeze
                │   ├── cgroup.kill
                │   ├── cgroup.max.depth
                │   ├── cgroup.max.descendants
                │   ├── cgroup.pressure
                │   ├── cgroup.procs
                │   ├── cgroup.stat
                │   ├── cgroup.subtree_control
                │   ├── cgroup.threads
                │   ├── cgroup.type
                │   ├── cpu.pressure
                │   ├── cpu.stat
                │   ├── cpu.stat.local
                │   ├── io.pressure
                │   ├── memory.current
                │   ├── memory.events
                │   ├── memory.events.local
                │   ├── memory.high
                │   ├── memory.low
                │   ├── memory.max
                │   ├── memory.min
                │   ├── memory.numa_stat
                │   ├── memory.oom.group
                │   ├── memory.peak
                │   ├── memory.pressure
                │   ├── memory.reclaim
                │   ├── memory.stat
                │   ├── memory.swap.current
                │   ├── memory.swap.events
                │   ├── memory.swap.high
                │   ├── memory.swap.max
                │   ├── memory.swap.peak
                │   ├── memory.zswap.current
                │   ├── memory.zswap.max
                │   ├── memory.zswap.writeback
                │   ├── pids.current
                │   ├── pids.events
                │   ├── pids.max
                │   └── pids.peak
                ├── org.gnome.SettingsDaemon.MediaKeys.service
                │   ├── cgroup.controllers
                │   ├── cgroup.events
                │   ├── cgroup.freeze
                │   ├── cgroup.kill
                │   ├── cgroup.max.depth
                │   ├── cgroup.max.descendants
                │   ├── cgroup.pressure
                │   ├── cgroup.procs
                │   ├── cgroup.stat
                │   ├── cgroup.subtree_control
                │   ├── cgroup.threads
                │   ├── cgroup.type
                │   ├── cpu.pressure
                │   ├── cpu.stat
                │   ├── cpu.stat.local
                │   ├── io.pressure
                │   ├── memory.current
                │   ├── memory.events
                │   ├── memory.events.local
                │   ├── memory.high
                │   ├── memory.low
                │   ├── memory.max
                │   ├── memory.min
                │   ├── memory.numa_stat
                │   ├── memory.oom.group
                │   ├── memory.peak
                │   ├── memory.pressure
                │   ├── memory.reclaim
                │   ├── memory.stat
                │   ├── memory.swap.current
                │   ├── memory.swap.events
                │   ├── memory.swap.high
                │   ├── memory.swap.max
                │   ├── memory.swap.peak
                │   ├── memory.zswap.current
                │   ├── memory.zswap.max
                │   ├── memory.zswap.writeback
                │   ├── pids.current
                │   ├── pids.events
                │   ├── pids.max
                │   └── pids.peak
                ├── org.gnome.SettingsDaemon.Power.service
                │   ├── cgroup.controllers
                │   ├── cgroup.events
                │   ├── cgroup.freeze
                │   ├── cgroup.kill
                │   ├── cgroup.max.depth
                │   ├── cgroup.max.descendants
                │   ├── cgroup.pressure
                │   ├── cgroup.procs
                │   ├── cgroup.stat
                │   ├── cgroup.subtree_control
                │   ├── cgroup.threads
                │   ├── cgroup.type
                │   ├── cpu.pressure
                │   ├── cpu.stat
                │   ├── cpu.stat.local
                │   ├── io.pressure
                │   ├── memory.current
                │   ├── memory.events
                │   ├── memory.events.local
                │   ├── memory.high
                │   ├── memory.low
                │   ├── memory.max
                │   ├── memory.min
                │   ├── memory.numa_stat
                │   ├── memory.oom.group
                │   ├── memory.peak
                │   ├── memory.pressure
                │   ├── memory.reclaim
                │   ├── memory.stat
                │   ├── memory.swap.current
                │   ├── memory.swap.events
                │   ├── memory.swap.high
                │   ├── memory.swap.max
                │   ├── memory.swap.peak
                │   ├── memory.zswap.current
                │   ├── memory.zswap.max
                │   ├── memory.zswap.writeback
                │   ├── pids.current
                │   ├── pids.events
                │   ├── pids.max
                │   └── pids.peak
                ├── org.gnome.SettingsDaemon.PrintNotifications.service
                │   ├── cgroup.controllers
                │   ├── cgroup.events
                │   ├── cgroup.freeze
                │   ├── cgroup.kill
                │   ├── cgroup.max.depth
                │   ├── cgroup.max.descendants
                │   ├── cgroup.pressure
                │   ├── cgroup.procs
                │   ├── cgroup.stat
                │   ├── cgroup.subtree_control
                │   ├── cgroup.threads
                │   ├── cgroup.type
                │   ├── cpu.pressure
                │   ├── cpu.stat
                │   ├── cpu.stat.local
                │   ├── io.pressure
                │   ├── memory.current
                │   ├── memory.events
                │   ├── memory.events.local
                │   ├── memory.high
                │   ├── memory.low
                │   ├── memory.max
                │   ├── memory.min
                │   ├── memory.numa_stat
                │   ├── memory.oom.group
                │   ├── memory.peak
                │   ├── memory.pressure
                │   ├── memory.reclaim
                │   ├── memory.stat
                │   ├── memory.swap.current
                │   ├── memory.swap.events
                │   ├── memory.swap.high
                │   ├── memory.swap.max
                │   ├── memory.swap.peak
                │   ├── memory.zswap.current
                │   ├── memory.zswap.max
                │   ├── memory.zswap.writeback
                │   ├── pids.current
                │   ├── pids.events
                │   ├── pids.max
                │   └── pids.peak
                ├── org.gnome.SettingsDaemon.Rfkill.service
                │   ├── cgroup.controllers
                │   ├── cgroup.events
                │   ├── cgroup.freeze
                │   ├── cgroup.kill
                │   ├── cgroup.max.depth
                │   ├── cgroup.max.descendants
                │   ├── cgroup.pressure
                │   ├── cgroup.procs
                │   ├── cgroup.stat
                │   ├── cgroup.subtree_control
                │   ├── cgroup.threads
                │   ├── cgroup.type
                │   ├── cpu.pressure
                │   ├── cpu.stat
                │   ├── cpu.stat.local
                │   ├── io.pressure
                │   ├── memory.current
                │   ├── memory.events
                │   ├── memory.events.local
                │   ├── memory.high
                │   ├── memory.low
                │   ├── memory.max
                │   ├── memory.min
                │   ├── memory.numa_stat
                │   ├── memory.oom.group
                │   ├── memory.peak
                │   ├── memory.pressure
                │   ├── memory.reclaim
                │   ├── memory.stat
                │   ├── memory.swap.current
                │   ├── memory.swap.events
                │   ├── memory.swap.high
                │   ├── memory.swap.max
                │   ├── memory.swap.peak
                │   ├── memory.zswap.current
                │   ├── memory.zswap.max
                │   ├── memory.zswap.writeback
                │   ├── pids.current
                │   ├── pids.events
                │   ├── pids.max
                │   └── pids.peak
                ├── org.gnome.SettingsDaemon.ScreensaverProxy.service
                │   ├── cgroup.controllers
                │   ├── cgroup.events
                │   ├── cgroup.freeze
                │   ├── cgroup.kill
                │   ├── cgroup.max.depth
                │   ├── cgroup.max.descendants
                │   ├── cgroup.pressure
                │   ├── cgroup.procs
                │   ├── cgroup.stat
                │   ├── cgroup.subtree_control
                │   ├── cgroup.threads
                │   ├── cgroup.type
                │   ├── cpu.pressure
                │   ├── cpu.stat
                │   ├── cpu.stat.local
                │   ├── io.pressure
                │   ├── memory.current
                │   ├── memory.events
                │   ├── memory.events.local
                │   ├── memory.high
                │   ├── memory.low
                │   ├── memory.max
                │   ├── memory.min
                │   ├── memory.numa_stat
                │   ├── memory.oom.group
                │   ├── memory.peak
                │   ├── memory.pressure
                │   ├── memory.reclaim
                │   ├── memory.stat
                │   ├── memory.swap.current
                │   ├── memory.swap.events
                │   ├── memory.swap.high
                │   ├── memory.swap.max
                │   ├── memory.swap.peak
                │   ├── memory.zswap.current
                │   ├── memory.zswap.max
                │   ├── memory.zswap.writeback
                │   ├── pids.current
                │   ├── pids.events
                │   ├── pids.max
                │   └── pids.peak
                ├── org.gnome.SettingsDaemon.Sharing.service
                │   ├── cgroup.controllers
                │   ├── cgroup.events
                │   ├── cgroup.freeze
                │   ├── cgroup.kill
                │   ├── cgroup.max.depth
                │   ├── cgroup.max.descendants
                │   ├── cgroup.pressure
                │   ├── cgroup.procs
                │   ├── cgroup.stat
                │   ├── cgroup.subtree_control
                │   ├── cgroup.threads
                │   ├── cgroup.type
                │   ├── cpu.pressure
                │   ├── cpu.stat
                │   ├── cpu.stat.local
                │   ├── io.pressure
                │   ├── memory.current
                │   ├── memory.events
                │   ├── memory.events.local
                │   ├── memory.high
                │   ├── memory.low
                │   ├── memory.max
                │   ├── memory.min
                │   ├── memory.numa_stat
                │   ├── memory.oom.group
                │   ├── memory.peak
                │   ├── memory.pressure
                │   ├── memory.reclaim
                │   ├── memory.stat
                │   ├── memory.swap.current
                │   ├── memory.swap.events
                │   ├── memory.swap.high
                │   ├── memory.swap.max
                │   ├── memory.swap.peak
                │   ├── memory.zswap.current
                │   ├── memory.zswap.max
                │   ├── memory.zswap.writeback
                │   ├── pids.current
                │   ├── pids.events
                │   ├── pids.max
                │   └── pids.peak
                ├── org.gnome.SettingsDaemon.Smartcard.service
                │   ├── cgroup.controllers
                │   ├── cgroup.events
                │   ├── cgroup.freeze
                │   ├── cgroup.kill
                │   ├── cgroup.max.depth
                │   ├── cgroup.max.descendants
                │   ├── cgroup.pressure
                │   ├── cgroup.procs
                │   ├── cgroup.stat
                │   ├── cgroup.subtree_control
                │   ├── cgroup.threads
                │   ├── cgroup.type
                │   ├── cpu.pressure
                │   ├── cpu.stat
                │   ├── cpu.stat.local
                │   ├── io.pressure
                │   ├── memory.current
                │   ├── memory.events
                │   ├── memory.events.local
                │   ├── memory.high
                │   ├── memory.low
                │   ├── memory.max
                │   ├── memory.min
                │   ├── memory.numa_stat
                │   ├── memory.oom.group
                │   ├── memory.peak
                │   ├── memory.pressure
                │   ├── memory.reclaim
                │   ├── memory.stat
                │   ├── memory.swap.current
                │   ├── memory.swap.events
                │   ├── memory.swap.high
                │   ├── memory.swap.max
                │   ├── memory.swap.peak
                │   ├── memory.zswap.current
                │   ├── memory.zswap.max
                │   ├── memory.zswap.writeback
                │   ├── pids.current
                │   ├── pids.events
                │   ├── pids.max
                │   └── pids.peak
                ├── org.gnome.SettingsDaemon.Sound.service
                │   ├── cgroup.controllers
                │   ├── cgroup.events
                │   ├── cgroup.freeze
                │   ├── cgroup.kill
                │   ├── cgroup.max.depth
                │   ├── cgroup.max.descendants
                │   ├── cgroup.pressure
                │   ├── cgroup.procs
                │   ├── cgroup.stat
                │   ├── cgroup.subtree_control
                │   ├── cgroup.threads
                │   ├── cgroup.type
                │   ├── cpu.pressure
                │   ├── cpu.stat
                │   ├── cpu.stat.local
                │   ├── io.pressure
                │   ├── memory.current
                │   ├── memory.events
                │   ├── memory.events.local
                │   ├── memory.high
                │   ├── memory.low
                │   ├── memory.max
                │   ├── memory.min
                │   ├── memory.numa_stat
                │   ├── memory.oom.group
                │   ├── memory.peak
                │   ├── memory.pressure
                │   ├── memory.reclaim
                │   ├── memory.stat
                │   ├── memory.swap.current
                │   ├── memory.swap.events
                │   ├── memory.swap.high
                │   ├── memory.swap.max
                │   ├── memory.swap.peak
                │   ├── memory.zswap.current
                │   ├── memory.zswap.max
                │   ├── memory.zswap.writeback
                │   ├── pids.current
                │   ├── pids.events
                │   ├── pids.max
                │   └── pids.peak
                ├── org.gnome.SettingsDaemon.Wacom.service
                │   ├── cgroup.controllers
                │   ├── cgroup.events
                │   ├── cgroup.freeze
                │   ├── cgroup.kill
                │   ├── cgroup.max.depth
                │   ├── cgroup.max.descendants
                │   ├── cgroup.pressure
                │   ├── cgroup.procs
                │   ├── cgroup.stat
                │   ├── cgroup.subtree_control
                │   ├── cgroup.threads
                │   ├── cgroup.type
                │   ├── cpu.pressure
                │   ├── cpu.stat
                │   ├── cpu.stat.local
                │   ├── io.pressure
                │   ├── memory.current
                │   ├── memory.events
                │   ├── memory.events.local
                │   ├── memory.high
                │   ├── memory.low
                │   ├── memory.max
                │   ├── memory.min
                │   ├── memory.numa_stat
                │   ├── memory.oom.group
                │   ├── memory.peak
                │   ├── memory.pressure
                │   ├── memory.reclaim
                │   ├── memory.stat
                │   ├── memory.swap.current
                │   ├── memory.swap.events
                │   ├── memory.swap.high
                │   ├── memory.swap.max
                │   ├── memory.swap.peak
                │   ├── memory.zswap.current
                │   ├── memory.zswap.max
                │   ├── memory.zswap.writeback
                │   ├── pids.current
                │   ├── pids.events
                │   ├── pids.max
                │   └── pids.peak
                ├── org.gnome.SettingsDaemon.XSettings.service
                │   ├── cgroup.controllers
                │   ├── cgroup.events
                │   ├── cgroup.freeze
                │   ├── cgroup.kill
                │   ├── cgroup.max.depth
                │   ├── cgroup.max.descendants
                │   ├── cgroup.pressure
                │   ├── cgroup.procs
                │   ├── cgroup.stat
                │   ├── cgroup.subtree_control
                │   ├── cgroup.threads
                │   ├── cgroup.type
                │   ├── cpu.pressure
                │   ├── cpu.stat
                │   ├── cpu.stat.local
                │   ├── io.pressure
                │   ├── memory.current
                │   ├── memory.events
                │   ├── memory.events.local
                │   ├── memory.high
                │   ├── memory.low
                │   ├── memory.max
                │   ├── memory.min
                │   ├── memory.numa_stat
                │   ├── memory.oom.group
                │   ├── memory.peak
                │   ├── memory.pressure
                │   ├── memory.reclaim
                │   ├── memory.stat
                │   ├── memory.swap.current
                │   ├── memory.swap.events
                │   ├── memory.swap.high
                │   ├── memory.swap.max
                │   ├── memory.swap.peak
                │   ├── memory.zswap.current
                │   ├── memory.zswap.max
                │   ├── memory.zswap.writeback
                │   ├── pids.current
                │   ├── pids.events
                │   ├── pids.max
                │   └── pids.peak
                ├── org.gnome.Shell@x11.service
                │   ├── cgroup.controllers
                │   ├── cgroup.events
                │   ├── cgroup.freeze
                │   ├── cgroup.kill
                │   ├── cgroup.max.depth
                │   ├── cgroup.max.descendants
                │   ├── cgroup.pressure
                │   ├── cgroup.procs
                │   ├── cgroup.stat
                │   ├── cgroup.subtree_control
                │   ├── cgroup.threads
                │   ├── cgroup.type
                │   ├── cpu.pressure
                │   ├── cpu.stat
                │   ├── cpu.stat.local
                │   ├── io.pressure
                │   ├── memory.current
                │   ├── memory.events
                │   ├── memory.events.local
                │   ├── memory.high
                │   ├── memory.low
                │   ├── memory.max
                │   ├── memory.min
                │   ├── memory.numa_stat
                │   ├── memory.oom.group
                │   ├── memory.peak
                │   ├── memory.pressure
                │   ├── memory.reclaim
                │   ├── memory.stat
                │   ├── memory.swap.current
                │   ├── memory.swap.events
                │   ├── memory.swap.high
                │   ├── memory.swap.max
                │   ├── memory.swap.peak
                │   ├── memory.zswap.current
                │   ├── memory.zswap.max
                │   ├── memory.zswap.writeback
                │   ├── pids.current
                │   ├── pids.events
                │   ├── pids.max
                │   └── pids.peak
                ├── pids.current
                ├── pids.events
                ├── pids.max
                ├── pids.peak
                ├── pipewire-media-session.service
                │   ├── cgroup.controllers
                │   ├── cgroup.events
                │   ├── cgroup.freeze
                │   ├── cgroup.kill
                │   ├── cgroup.max.depth
                │   ├── cgroup.max.descendants
                │   ├── cgroup.pressure
                │   ├── cgroup.procs
                │   ├── cgroup.stat
                │   ├── cgroup.subtree_control
                │   ├── cgroup.threads
                │   ├── cgroup.type
                │   ├── cpu.pressure
                │   ├── cpu.stat
                │   ├── cpu.stat.local
                │   ├── io.pressure
                │   ├── memory.current
                │   ├── memory.events
                │   ├── memory.events.local
                │   ├── memory.high
                │   ├── memory.low
                │   ├── memory.max
                │   ├── memory.min
                │   ├── memory.numa_stat
                │   ├── memory.oom.group
                │   ├── memory.peak
                │   ├── memory.pressure
                │   ├── memory.reclaim
                │   ├── memory.stat
                │   ├── memory.swap.current
                │   ├── memory.swap.events
                │   ├── memory.swap.high
                │   ├── memory.swap.max
                │   ├── memory.swap.peak
                │   ├── memory.zswap.current
                │   ├── memory.zswap.max
                │   ├── memory.zswap.writeback
                │   ├── pids.current
                │   ├── pids.events
                │   ├── pids.max
                │   └── pids.peak
                ├── pipewire.service
                │   ├── cgroup.controllers
                │   ├── cgroup.events
                │   ├── cgroup.freeze
                │   ├── cgroup.kill
                │   ├── cgroup.max.depth
                │   ├── cgroup.max.descendants
                │   ├── cgroup.pressure
                │   ├── cgroup.procs
                │   ├── cgroup.stat
                │   ├── cgroup.subtree_control
                │   ├── cgroup.threads
                │   ├── cgroup.type
                │   ├── cpu.pressure
                │   ├── cpu.stat
                │   ├── cpu.stat.local
                │   ├── io.pressure
                │   ├── memory.current
                │   ├── memory.events
                │   ├── memory.events.local
                │   ├── memory.high
                │   ├── memory.low
                │   ├── memory.max
                │   ├── memory.min
                │   ├── memory.numa_stat
                │   ├── memory.oom.group
                │   ├── memory.peak
                │   ├── memory.pressure
                │   ├── memory.reclaim
                │   ├── memory.stat
                │   ├── memory.swap.current
                │   ├── memory.swap.events
                │   ├── memory.swap.high
                │   ├── memory.swap.max
                │   ├── memory.swap.peak
                │   ├── memory.zswap.current
                │   ├── memory.zswap.max
                │   ├── memory.zswap.writeback
                │   ├── pids.current
                │   ├── pids.events
                │   ├── pids.max
                │   └── pids.peak
                ├── pulseaudio.service
                │   ├── cgroup.controllers
                │   ├── cgroup.events
                │   ├── cgroup.freeze
                │   ├── cgroup.kill
                │   ├── cgroup.max.depth
                │   ├── cgroup.max.descendants
                │   ├── cgroup.pressure
                │   ├── cgroup.procs
                │   ├── cgroup.stat
                │   ├── cgroup.subtree_control
                │   ├── cgroup.threads
                │   ├── cgroup.type
                │   ├── cpu.pressure
                │   ├── cpu.stat
                │   ├── cpu.stat.local
                │   ├── io.pressure
                │   ├── memory.current
                │   ├── memory.events
                │   ├── memory.events.local
                │   ├── memory.high
                │   ├── memory.low
                │   ├── memory.max
                │   ├── memory.min
                │   ├── memory.numa_stat
                │   ├── memory.oom.group
                │   ├── memory.peak
                │   ├── memory.pressure
                │   ├── memory.reclaim
                │   ├── memory.stat
                │   ├── memory.swap.current
                │   ├── memory.swap.events
                │   ├── memory.swap.high
                │   ├── memory.swap.max
                │   ├── memory.swap.peak
                │   ├── memory.zswap.current
                │   ├── memory.zswap.max
                │   ├── memory.zswap.writeback
                │   ├── pids.current
                │   ├── pids.events
                │   ├── pids.max
                │   └── pids.peak
                ├── xdg-desktop-portal.service
                │   ├── cgroup.controllers
                │   ├── cgroup.events
                │   ├── cgroup.freeze
                │   ├── cgroup.kill
                │   ├── cgroup.max.depth
                │   ├── cgroup.max.descendants
                │   ├── cgroup.pressure
                │   ├── cgroup.procs
                │   ├── cgroup.stat
                │   ├── cgroup.subtree_control
                │   ├── cgroup.threads
                │   ├── cgroup.type
                │   ├── cpu.pressure
                │   ├── cpu.stat
                │   ├── cpu.stat.local
                │   ├── io.pressure
                │   ├── memory.current
                │   ├── memory.events
                │   ├── memory.events.local
                │   ├── memory.high
                │   ├── memory.low
                │   ├── memory.max
                │   ├── memory.min
                │   ├── memory.numa_stat
                │   ├── memory.oom.group
                │   ├── memory.peak
                │   ├── memory.pressure
                │   ├── memory.reclaim
                │   ├── memory.stat
                │   ├── memory.swap.current
                │   ├── memory.swap.events
                │   ├── memory.swap.high
                │   ├── memory.swap.max
                │   ├── memory.swap.peak
                │   ├── memory.zswap.current
                │   ├── memory.zswap.max
                │   ├── memory.zswap.writeback
                │   ├── pids.current
                │   ├── pids.events
                │   ├── pids.max
                │   └── pids.peak
                └── xdg-document-portal.service
                    ├── cgroup.controllers
                    ├── cgroup.events
                    ├── cgroup.freeze
                    ├── cgroup.kill
                    ├── cgroup.max.depth
                    ├── cgroup.max.descendants
                    ├── cgroup.pressure
                    ├── cgroup.procs
                    ├── cgroup.stat
                    ├── cgroup.subtree_control
                    ├── cgroup.threads
                    ├── cgroup.type
                    ├── cpu.pressure
                    ├── cpu.stat
                    ├── cpu.stat.local
                    ├── io.pressure
                    ├── memory.current
                    ├── memory.events
                    ├── memory.events.local
                    ├── memory.high
                    ├── memory.low
                    ├── memory.max
                    ├── memory.min
                    ├── memory.numa_stat
                    ├── memory.oom.group
                    ├── memory.peak
                    ├── memory.pressure
                    ├── memory.reclaim
                    ├── memory.stat
                    ├── memory.swap.current
                    ├── memory.swap.events
                    ├── memory.swap.high
                    ├── memory.swap.max
                    ├── memory.swap.peak
                    ├── memory.zswap.current
                    ├── memory.zswap.max
                    ├── memory.zswap.writeback
                    ├── pids.current
                    ├── pids.events
                    ├── pids.max
                    └── pids.peak

137 directories, 5643 files
