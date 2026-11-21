.. SPDX-License-Identifier: GPL-2.0

=================================
Introduction of mseal(mseal 简介)
=================================

:Author: Jeff Xu <jeffxu@chromium.org>

(作者: Jeff Xu <jeffxu@chromium.org>)

Modern CPUs support memory permissions such as RW and NX bits. The memory
permission feature improves security stance on memory corruption bugs, i.e.
the attacker can’t just write to arbitrary memory and point the code to it,
the memory has to be marked with X bit, or else an exception will happen.
(现代 CPU 支持诸如 RW 与 NX 位之类的内存权限。内存权限特性改善了针对内存破坏类缺陷的安全态势，即攻击者不能随意写入任意内存并让代码指向它，内存必须带有 X 位标记，否则将发生异常。)

Memory sealing additionally protects the mapping itself against
modifications. This is useful to mitigate memory corruption issues where a
corrupted pointer is passed to a memory management system. For example,
such an attacker primitive can break control-flow integrity guarantees
since read-only memory that is supposed to be trusted can become writable
or .text pages can get remapped. Memory sealing can automatically be
applied by the runtime loader to seal .text and .rodata pages and
applications can additionally seal security critical data at runtime.
(内存密封(sealing)还保护映射本身免遭修改。这有助于缓解把被破坏的指针传给内存管理系统的内存破坏问题。例如，这样的攻击原语可以破坏控制流完整性保证，因为本应受信任的只读内存可能变成可写，或者 .text 页可能被重新映射。运行时加载器可以自动应用内存密封来密封 .text 与 .rodata 页，应用程序还可以在运行时额外密封安全关键数据。)

A similar feature already exists in the XNU kernel with the
VM_FLAGS_PERMANENT flag [1] and on OpenBSD with the mimmutable syscall [2].
(类似特性已存在于 XNU 内核的 VM_FLAGS_PERMANENT 标志 [1] 中，以及 OpenBSD 的 mimmutable 系统调用 [2] 中。)

SYSCALL(系统调用)
=================
mseal syscall signature(mseal 系统调用签名)
-------------------------------------------
   ``int mseal(void *addr, size_t len, unsigned long flags)``

   **addr**/**len**: virtual memory address range. (**addr**/**len**：虚拟内存地址范围。)
      The address range set by **addr**/**len** must meet: (由 **addr**/**len** 设置的地址范围必须满足：)
         - The start address must be in an allocated VMA. (起始地址必须位于一个已分配的 VMA 中。)
         - The start address must be page aligned. (起始地址必须按页对齐。)
         - The end address (**addr** + **len**) must be in an allocated VMA. (结束地址(**addr** + **len**)必须位于一个已分配的 VMA 中。)
         - no gap (unallocated memory) between start and end address. (起始地址与结束地址之间不能有空隙(未分配的内存)。)

      The ``len`` will be paged aligned implicitly by the kernel.
      ( ``len`` 会被内核隐式地按页对齐。)

   **flags**: reserved for future use. (**flags** 保留供将来使用。)

   **Return values**: (返回值：)
      - **0**: Success. (0：成功。)
      - **-EINVAL**: (EINVAL：)
         * Invalid input ``flags``. (无效的输入 ``flags`` 。)
         * The start address (``addr``) is not page aligned. (起始地址( ``addr`` )未按页对齐。)
         * Address range (``addr`` + ``len``) overflow. (地址范围( ``addr`` + ``len`` )溢出。)
      - **-ENOMEM**: (ENOMEM：)
         * The start address (``addr``) is not allocated. (起始地址( ``addr`` )未被分配。)
         * The end address (``addr`` + ``len``) is not allocated. (结束地址( ``addr`` + ``len`` )未被分配。)
         * A gap (unallocated memory) between start and end address. (起始地址与结束地址之间存在空隙(未分配的内存)。)
      - **-EPERM**: (EPERM：)
         * sealing is supported only on 64-bit CPUs, 32-bit is not supported. (密封仅支持 64 位 CPU，不支持 32 位。)

   **Note about error return**: (关于错误返回的说明：)
      - For above error cases, users can expect the given memory range is
        unmodified, i.e. no partial update.
        (对于上述错误情形，用户可以预期给定的内存范围未被修改，即不会发生部分更新。)
      - There might be other internal errors/cases not listed here, e.g.
        error during merging/splitting VMAs, or the process reaching the maximum
        number of supported VMAs. In those cases, partial updates to the given
        memory range could happen. However, those cases should be rare.
        (可能还有此处未列出的其他内部错误/情形，例如在合并/拆分 VMA 时发生错误，或进程达到所支持的 VMA 数量上限。在这些情形下，对给定内存范围的更新可能是部分的。不过这些情形应当很少见。)

   **Architecture support**: (架构支持：)
      mseal only works on 64-bit CPUs, not 32-bit CPUs.
      (mseal 只适用于 64 位 CPU，不适用于 32 位 CPU。)

   **Idempotent**: (幂等性：)
      users can call mseal multiple times. mseal on an already sealed memory
      is a no-action (not error).
      (用户可以多次调用 mseal。对已密封的内存再次调用 mseal 不会产生任何动作(也不报错)。)

   **no munseal** (没有 munseal)
      Once mapping is sealed, it can't be unsealed. The kernel should never
      have munseal, this is consistent with other sealing feature, e.g.
      F_SEAL_SEAL for file.
      (映射一旦被密封，就无法解除密封。内核永远不应该提供 munseal，这与其他密封特性保持一致，例如针对文件的 F_SEAL_SEAL。)

Blocked mm syscall for sealed mapping(对已密封映射阻止的 mm 系统调用)
---------------------------------------------------------------------
   It might be important to note: **once the mapping is sealed, it will
   stay in the process's memory until the process terminates**.
   (值得强调的是：一旦映射被密封，它就会一直留在进程的内存中，直到进程终止。)

   Example::

         *ptr = mmap(0, 4096, PROT_READ, MAP_ANONYMOUS | MAP_PRIVATE, 0, 0);
         rc = mseal(ptr, 4096, 0);
         /* munmap will fail */
         rc = munmap(ptr, 4096);
         assert(rc < 0);

   (示例：)

   Blocked mm syscall: (会被阻止的 mm 系统调用：)
      - munmap
      - mmap
      - mremap
      - mprotect and pkey_mprotect
        (mprotect 与 pkey_mprotect)
      - some destructive madvise behaviors: MADV_DONTNEED, MADV_FREE,
        MADV_DONTNEED_LOCKED, MADV_FREE, MADV_DONTFORK, MADV_WIPEONFORK
        (一些破坏性的 madvise 行为：MADV_DONTNEED、MADV_FREE、MADV_DONTNEED_LOCKED、MADV_FREE、MADV_DONTFORK、MADV_WIPEONFORK)

   The first set of syscalls to block is munmap, mremap, mmap. They can
   either leave an empty space in the address space, therefore allowing
   replacement with a new mapping with new set of attributes, or can
   overwrite the existing mapping with another mapping.
   (第一批被阻止的系统调用是 munmap、mremap、mmap。它们要么会在地址空间中留出一块空区域，从而允许用一组具有新属性的新映射来替换，要么会用另一个映射覆盖现有映射。)

   mprotect and pkey_mprotect are blocked because they changes the
   protection bits (RWX) of the mapping.
   (阻止 mprotect 和 pkey_mprotect，是因为它们会改变映射的保护位(RWX)。)

   Certain destructive madvise behaviors, specifically MADV_DONTNEED,
   MADV_FREE, MADV_DONTNEED_LOCKED, and MADV_WIPEONFORK, can introduce
   risks when applied to anonymous memory by threads lacking write
   permissions. Consequently, these operations are prohibited under such
   conditions. The aforementioned behaviors have the potential to modify
   region contents by discarding pages, effectively performing a memset(0)
   operation on the anonymous memory.
   (某些破坏性的 madvise 行为，特别是 MADV_DONTNEED、MADV_FREE、MADV_DONTNEED_LOCKED 与 MADV_WIPEONFORK，在由缺乏写权限的线程应用于匿名内存时会带来风险。因此，在这种条件下禁止这些操作。上述行为可能通过丢弃页来修改区域内容，实际上相当于对匿名内存执行 memset(0) 操作。)

   Kernel will return -EPERM for blocked syscalls.
   (对被阻止的系统调用，内核将返回 -EPERM。)

   When blocked syscall return -EPERM due to sealing, the memory regions may
   or may not be changed, depends on the syscall being blocked:
   (当被阻止的系统调用因密封而返回 -EPERM 时，内存区域可能已改变也可能未改变，取决于被阻止的是哪个系统调用：)

      - munmap: munmap is atomic. If one of VMAs in the given range is
        sealed, none of VMAs are updated.
        (munmap 是原子的。如果给定范围中的某个 VMA 已被密封，则所有 VMA 都不会被更新。)
      - mprotect, pkey_mprotect, madvise: partial update might happen, e.g.
        when mprotect over multiple VMAs, mprotect might update the beginning
        VMAs before reaching the sealed VMA and return -EPERM.
        (mprotect、pkey_mprotect、madvise：可能发生部分更新，例如 mprotect 跨越多个 VMA 时，可能在到达已密封的 VMA 之前先更新了开头的那些 VMA，然后返回 -EPERM。)
      - mmap and mremap: undefined behavior. (mmap 与 mremap：行为未定义。)

Use cases(用例)
===============
- glibc:
  (glibc：)
  The dynamic linker, during loading ELF executables, can apply sealing to
  mapping segments.
  (动态链接器在加载 ELF 可执行文件时，可以对映射段应用密封。)

- Chrome browser: protect some security sensitive data structures.
  (Chrome 浏览器：保护一些安全敏感的数据结构。)

- System mappings:
  (系统映射：)
  The system mappings are created by the kernel and includes vdso, vvar,
  vvar_vclock, vectors (arm compat-mode), sigpage (arm compat-mode), uprobes.
  (系统映射由内核创建，包括 vdso、vvar、vvar_vclock、vectors(arm 兼容模式)、sigpage(arm 兼容模式)、uprobes。)

  Those system mappings are readonly only or execute only, memory sealing can
  protect them from ever changing to writable or unmmap/remapped as different
  attributes. This is useful to mitigate memory corruption issues where a
  corrupted pointer is passed to a memory management system.
  (这些系统映射是只读或只执行的，内存密封可以防止它们被改为可写，或被以不同属性取消映射/重新映射。这有助于缓解把被破坏的指针传给内存管理系统的内存破坏问题。)

  If supported by an architecture (CONFIG_ARCH_SUPPORTS_MSEAL_SYSTEM_MAPPINGS),
  the CONFIG_MSEAL_SYSTEM_MAPPINGS seals all system mappings of this
  architecture.
  (如果架构支持(CONFIG_ARCH_SUPPORTS_MSEAL_SYSTEM_MAPPINGS)，CONFIG_MSEAL_SYSTEM_MAPPINGS 会密封该架构的所有系统映射。)

  The following architectures currently support this feature: x86-64, arm64,
  loongarch and s390.
  (目前支持该特性的架构有：x86-64、arm64、loongarch 和 s390。)

  WARNING: This feature breaks programs which rely on relocating
  or unmapping system mappings. Known broken software at the time
  of writing includes CHECKPOINT_RESTORE, UML, gVisor, rr. Therefore
  this config can't be enabled universally.
  (警告：该特性会破坏依赖重定位或取消映射系统映射的程序。截至撰写时，已知受影响的软件包括 CHECKPOINT_RESTORE、UML、gVisor、rr。因此无法普遍启用该配置。)

When not to use mseal(何时不应使用 mseal)
=========================================
Applications can apply sealing to any virtual memory region from userspace,
but it is *crucial to thoroughly analyze the mapping's lifetime* prior to
apply the sealing. This is because the sealed mapping *won’t be unmapped*
until the process terminates or the exec system call is invoked.
(应用程序可以从用户空间对任何虚拟内存区域应用密封，但在应用密封之前，务必彻底分析该映射的生命周期。这是因为被密封的映射在进程终止或调用 exec 系统调用之前不会被取消映射。)

For example: (例如：)
   - aio/shm
     aio/shm can call mmap and  munmap on behalf of userspace, e.g.
     ksys_shmdt() in shm.c. The lifetimes of those mapping are not tied to
     the lifetime of the process. If those memories are sealed from userspace,
     then munmap will fail, causing leaks in VMA address space during the
     lifetime of the process.
     (aio/shm 可以代表用户空间调用 mmap 和 munmap，例如 shm.c 中的 ksys_shmdt()。这些映射的生命周期并不与进程的生命周期绑定。如果这些内存从用户空间被密封，那么 munmap 将会失败，从而在进程的生命周期内造成 VMA 地址空间的泄漏。)

   - ptr allocated by malloc (heap)
     (由 malloc 分配的指针(堆)：)
     Don't use mseal on the memory ptr return from malloc().
     malloc() is implemented by allocator, e.g. by glibc. Heap manager might
     allocate a ptr from brk or mapping created by mmap.
     If an app calls mseal on a ptr returned from malloc(), this can affect
     the heap manager's ability to manage the mappings; the outcome is
     non-deterministic.
     (不要对从 malloc() 返回的指针 ptr 所指的内存使用 mseal。malloc() 由分配器实现，例如 glibc。堆管理器可能从 brk 或由 mmap 创建的映射中分配指针。如果应用程序对 malloc() 返回的 ptr 调用 mseal，这会影响堆管理器管理映射的能力；结果是不确定的。)

     Example::

        ptr = malloc(size);
        /* don't call mseal on ptr return from malloc. */
        mseal(ptr, size);
        /* free will success, allocator can't shrink heap lower than ptr */
        free(ptr);

     (示例：)

mseal doesn't block(mseal 不能阻止的事项)
=========================================
In a nutshell, mseal blocks certain mm syscall from modifying some of VMA's
attributes, such as protection bits (RWX). Sealed mappings doesn't mean the
memory is immutable.
(简而言之，mseal 阻止某些 mm 系统调用修改 VMA 的某些属性，例如保护位(RWX)。密封映射并不意味着内存不可变。)

As Jann Horn pointed out in [3], there are still a few ways to write
to RO memory, which is, in a way, by design. And those could be blocked
by different security measures.
(正如 Jann Horn 在 [3] 中指出的，仍然有若干种写入只读内存的途径，这在某种意义上是设计使然。而这些途径可以通过其他不同的安全措施来阻止。)

Those cases are:
(这些情形包括：)

   - Write to read-only memory through /proc/self/mem interface (FOLL_FORCE).
     (通过 /proc/self/mem 接口写入只读内存(FOLL_FORCE)。)
   - Write to read-only memory through ptrace (such as PTRACE_POKETEXT).
     (通过 ptrace 写入只读内存(例如 PTRACE_POKETEXT)。)
   - userfaultfd.
     (userfaultfd。)

The idea that inspired this patch comes from Stephen Röttger’s work in V8
CFI [4]. Chrome browser in ChromeOS will be the first user of this API.
(启发本补丁的想法来自 Stephen Röttger 在 V8 CFI 方面的工作 [4]。ChromeOS 中的 Chrome 浏览器将是该 API 的第一个使用者。)

Reference(参考)
===============
- [1] https://github.com/apple-oss-distributions/xnu/blob/1031c584a5e37aff177559b9f69dbd3c8c3fd30a/osfmk/mach/vm_statistics.h#L274
- [2] https://man.openbsd.org/mimmutable.2
- [3] https://lore.kernel.org/lkml/CAG48ez3ShUYey+ZAFsU2i1RpQn0a5eOs2hzQ426FkcgnfUGLvA@mail.gmail.com
- [4] https://docs.google.com/document/d/1O2jwK4dxI3nRcOJuPYkonhTkNQfbmwdvxQMyXgeaRHo/edit#heading=h.bvaojj9fu6hc
