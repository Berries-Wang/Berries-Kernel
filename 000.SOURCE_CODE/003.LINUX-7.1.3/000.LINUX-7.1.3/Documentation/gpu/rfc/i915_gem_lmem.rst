=========================================================
I915 DG1/LMEM RFC Section(I915 DG1/LMEM RFC 章节)
=========================================================

Upstream plan(上游计划)
=======================
For upstream the overall plan for landing all the DG1 stuff and turning it for
real, with all the uAPI bits is:
(要把所有 DG1 相关内容落地并真正启用 (含全部 uAPI 部分)，上游的总体计划是：)

* Merge basic HW enabling of DG1(still without pciid)
  (合并 DG1 的基本硬件启用代码 (仍无 pciid)。)
* Merge the uAPI bits behind special CONFIG_BROKEN(or so) flag(把 uAPI 部分放在特殊的 CONFIG_BROKEN (或类似) 旗标之后合并。)
        * At this point we can still make changes, but importantly this lets us
          start running IGTs which can utilize local-memory in CI
          (此时我们仍可做出更改，但重要的是这让我们可以在 CI 中开始运行能够使用本地内存的 IGT 测试。)
* Convert over to TTM, make sure it all keeps working. Some of the work items:(转换为 TTM，并确保一切继续正常工作。部分工作项：)
        * TTM shrinker for discrete
          (面向独显的 TTM shrinker。)
        * dma_resv_lockitem for full dma_resv_lock, i.e not just trylock
          (用于完整 dma_resv_lock (而不仅是 trylock) 的 dma_resv_lockitem。)
        * Use TTM CPU pagefault handler
          (使用 TTM CPU 缺页处理程序。)
        * Route shmem backend over to TTM SYSTEM for discrete
          (对独显把 shmem 后端改道到 TTM SYSTEM。)
        * TTM purgeable object support
          (TTM 可清除 (purgeable) 对象支持。)
        * Move i915 buddy allocator over to TTM
          (把 i915 buddy 分配器迁移到 TTM。)
* Send RFC(with mesa-dev on cc) for final sign off on the uAPI
  (发送 RFC (抄送 mesa-dev) 以获得 uAPI 的最终签署认可。)
* Add pciid for DG1 and turn on uAPI for real
  (为 DG1 添加 pciid 并真正启用 uAPI。)
