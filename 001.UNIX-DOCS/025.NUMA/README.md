# NUMA


## R7-5700G 16G*2
```shell
wei@Berries:~/OPEN_SOURCE/Berries-Chat$ lscpu 
Architecture:             x86_64
  CPU op-mode(s):         32-bit, 64-bit
  Address sizes:          48 bits physical, 48 bits virtual
  Byte Order:             Little Endian
CPU(s):                   16
  On-line CPU(s) list:    0-15
Vendor ID:                AuthenticAMD
  Model name:             AMD Ryzen 7 5700G with Radeon Graphics
    CPU family:           25
    Model:                80
    Thread(s) per core:   2
    Core(s) per socket:   8
    Socket(s):            1
    Stepping:             0
    CPU max MHz:          4673.0000
    CPU min MHz:          400.0000
    BogoMIPS:             7586.09
    Flags:                fpu vme de pse tsc msr pae mce cx8 apic sep mtrr pge mca cmov pat pse36 clflush mmx fxsr sse sse2 ht syscall nx mmxext fxsr_opt pdpe1gb rdtscp lm constant_tsc rep_good nopl nonstop_tsc 
                          cpuid extd_apicid aperfmperf rapl pni pclmulqdq monitor ssse3 fma cx16 sse4_1 sse4_2 x2apic movbe popcnt aes xsave avx f16c rdrand lahf_lm cmp_legacy extapic cr8_legacy abm sse4a misali
                          gnsse 3dnowprefetch osvw ibs skinit wdt tce topoext perfctr_core perfctr_nb bpext perfctr_llc mwaitx cpb cat_l3 cdp_l3 hw_pstate ssbd mba ibrs ibpb stibp vmmcall fsgsbase bmi1 avx2 smep
                           bmi2 erms invpcid cqm rdt_a rdseed adx smap clflushopt clwb sha_ni xsaveopt xsavec xgetbv1 xsaves cqm_llc cqm_occup_llc cqm_mbm_total cqm_mbm_local user_shstk clzero irperf xsaveerptr 
                          rdpru wbnoinvd cppc arat npt lbrv svm_lock nrip_save tsc_scale vmcb_clean flushbyasid decodeassists pausefilter pfthreshold avic v_vmsave_vmload vgif v_spec_ctrl umip pku ospke vaes vpc
                          lmulqdq rdpid overflow_recov succor smca fsrm debug_swap
Caches (sum of all):      
  L1d:                    256 KiB (8 instances)
  L1i:                    256 KiB (8 instances)
  L2:                     4 MiB (8 instances)
  L3:                     16 MiB (1 instance)
NUMA:                     
  NUMA node(s):           1
  NUMA node0 CPU(s):      0-15
Vulnerabilities:          
  Gather data sampling:   Not affected
  Itlb multihit:          Not affected
  L1tf:                   Not affected
  Mds:                    Not affected
  Meltdown:               Not affected
  Mmio stale data:        Not affected
  Reg file data sampling: Not affected
  Retbleed:               Not affected
  Spec rstack overflow:   Vulnerable: Safe RET, no microcode
  Spec store bypass:      Mitigation; Speculative Store Bypass disabled via prctl
  Spectre v1:             Mitigation; usercopy/swapgs barriers and __user pointer sanitization
  Spectre v2:             Mitigation; Retpolines; IBPB conditional; IBRS_FW; STIBP always-on; RSB filling; PBRSB-eIBRS Not affected; BHI Not affected
  Srbds:                  Not affected
  Tsx async abort:        Not affected

```



## NUMA 和物理CPU 内存节点的关系
![NUMA](./999.IMGS/Screenshot%202025-09-12%20at%2008-03-39%20一个%20---%20NUMA.png)


## 参考资料
- [https://learn.arm.com/learning-paths/servers-and-cloud-computing/tune-network-workloads-on-bare-metal/4_local-numa/](https://learn.arm.com/learning-paths/servers-and-cloud-computing/tune-network-workloads-on-bare-metal/4_local-numa/)
- [https://www.boost.org/doc/libs/1_89_0/libs/fiber/doc/html/fiber/numa.html](https://www.boost.org/doc/libs/1_89_0/libs/fiber/doc/html/fiber/numa.html)
- [https://support.huaweicloud.com/usermanual-cce/cce_10_0425.html](https://support.huaweicloud.com/usermanual-cce/cce_10_0425.html)
- [https://docs.redhat.com/zh-cn/documentation/openshift_container_platform/4.11/html/scalability_and_performance/cnf-numa-aware-scheduling](https://docs.redhat.com/zh-cn/documentation/openshift_container_platform/4.11/html/scalability_and_performance/cnf-numa-aware-scheduling)
