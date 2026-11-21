.. SPDX-License-Identifier: GPL-2.0

====================================
Memory Protection Keys(内存保护密钥)
====================================

Memory Protection Keys provide a mechanism for enforcing page-based
protections, but without requiring modification of the page tables when an
application changes protection domains.
(内存保护密钥提供了一种执行基于页的保护的机制，但在应用程序更改保护域时无需修改页表。)

Pkeys Userspace (PKU) is a feature which can be found on:
(Pkeys Userspace（PKU）特性可以在以下平台上找到：)

        * Intel server CPUs, Skylake and later
          (Intel 服务器 CPU，Skylake 及之后版本)
        * Intel client CPUs, Tiger Lake (11th Gen Core) and later
          (Intel 客户端 CPU，Tiger Lake（第 11 代 Core）及之后版本)
        * Future AMD CPUs
          (未来的 AMD CPU)
        * arm64 CPUs implementing the Permission Overlay Extension (FEAT_S1POE)
          (实现权限覆盖扩展（FEAT_S1POE）的 arm64 CPU)

x86_64
======
Pkeys work by dedicating 4 previously Reserved bits in each page table entry to
a "protection key", giving 16 possible keys.
(Pkeys 的工作原理是把每个页表项中先前保留（Reserved）的 4 个位专用于一个"保护密钥"（protection key），从而提供 16 个可能的密钥。)

Protections for each key are defined with a per-CPU user-accessible register
(PKRU).  Each of these is a 32-bit register storing two bits (Access Disable
and Write Disable) for each of 16 keys.
(每个密钥的保护权限由一个每 CPU 用户可访问寄存器（PKRU）定义。它是一个 32 位寄存器，为 16 个密钥中的每一个存储两个位（访问禁用和写入禁用）。)

Being a CPU register, PKRU is inherently thread-local, potentially giving each
thread a different set of protections from every other thread.
(作为 CPU 寄存器，PKRU 天然是线程本地的，这可能使每个线程拥有与其他每个线程都不同的保护集合。)

There are two instructions (RDPKRU/WRPKRU) for reading and writing to the
register.  The feature is only available in 64-bit mode, even though there is
theoretically space in the PAE PTEs.  These permissions are enforced on data
access only and have no effect on instruction fetches.
(有两条指令（RDPKRU/WRPKRU）用于读写该寄存器。该特性仅在 64 位模式下可用，尽管 PAE PTE 中理论上存在空间。这些权限仅对数据访问强制执行，对指令获取没有影响。)

arm64
=====

Pkeys use 3 bits in each page table entry, to encode a "protection key index",
giving 8 possible keys.
(Pkeys 使用每个页表项中的 3 个位来编码一个"保护密钥索引"，从而提供 8 个可能的密钥。)

Protections for each key are defined with a per-CPU user-writable system
register (POR_EL0).  This is a 64-bit register encoding read, write and execute
overlay permissions for each protection key index.
(每个密钥的保护权限由一个每 CPU 用户可写的系统寄存器（POR_EL0）定义。这是一个 64 位寄存器，为每个保护密钥索引编码读、写和执行覆盖权限。)

Being a CPU register, POR_EL0 is inherently thread-local, potentially giving
each thread a different set of protections from every other thread.
(作为 CPU 寄存器，POR_EL0 天然是线程本地的，这可能使每个线程拥有与其他每个线程都不同的保护集合。)

Unlike x86_64, the protection key permissions also apply to instruction
fetches.
(与 x86_64 不同，保护密钥权限也适用于指令获取。)

Syscalls(系统调用)
==================

There are 3 system calls which directly interact with pkeys::

	int pkey_alloc(unsigned long flags, unsigned long init_access_rights)
	int pkey_free(int pkey);
	int pkey_mprotect(unsigned long start, size_t len,
			  unsigned long prot, int pkey);

(有 3 个系统调用直接与 pkeys 交互：)

Before a pkey can be used, it must first be allocated with pkey_alloc().  An
application writes to the architecture specific CPU register directly in order
to change access permissions to memory covered with a key.  In this example
this is wrapped by a C function called pkey_set().
(在可以使用 pkey 之前，必须先用 pkey_alloc() 分配它。应用程序直接写入架构特定的 CPU 寄存器，以更改密钥所覆盖内存的访问权限。在此示例中，这由一个名为 pkey_set() 的 C 函数封装。)
::

	int real_prot = PROT_READ|PROT_WRITE;
	pkey = pkey_alloc(0, PKEY_DISABLE_WRITE);
	ptr = mmap(NULL, PAGE_SIZE, PROT_NONE, MAP_ANONYMOUS|MAP_PRIVATE, -1, 0);
	ret = pkey_mprotect(ptr, PAGE_SIZE, real_prot, pkey);
	... application runs here

Now, if the application needs to update the data at 'ptr', it can
gain access, do the update, then remove its write access::

	pkey_set(pkey, 0); // clear PKEY_DISABLE_WRITE
	*ptr = foo; // assign something
	pkey_set(pkey, PKEY_DISABLE_WRITE); // set PKEY_DISABLE_WRITE again

(现在，如果应用程序需要更新 'ptr' 处的数据，它可以先获得访问权，执行更新，然后再移除自己的写访问权限：)

Now when it frees the memory, it will also free the pkey since it
is no longer in use::

	munmap(ptr, PAGE_SIZE);
	pkey_free(pkey);

(现在当它释放内存时，它也会释放 pkey，因为它不再被使用。)

.. note:: pkey_set() is a wrapper around writing to the CPU register.
          Example implementations can be found in
          tools/testing/selftests/mm/pkey-{arm64,powerpc,x86}.h
          (pkey_set() 是对写入 CPU 寄存器的封装。示例实现可以在 tools/testing/selftests/mm/pkey-{arm64,powerpc,x86}.h 中找到。)

Behavior(行为)
==============

The kernel attempts to make protection keys consistent with the
behavior of a plain mprotect().  For instance if you do this::

	mprotect(ptr, size, PROT_NONE);
	something(ptr);

(内核试图使保护密钥的行为与普通 mprotect() 保持一致。例如，如果你这样做：)

you can expect the same effects with protection keys when doing this::

	pkey = pkey_alloc(0, PKEY_DISABLE_WRITE | PKEY_DISABLE_READ);
	pkey_mprotect(ptr, size, PROT_READ|PROT_WRITE, pkey);
	something(ptr);

(那么当你这样做时，可以预期保护密钥会产生相同的效果：)

That should be true whether something() is a direct access to 'ptr'
like::

	*ptr = foo;

(无论 something() 是对 'ptr' 的直接访问，例如：)

or when the kernel does the access on the application's behalf like
with a read()::

	read(fd, ptr, 1);

(还是当内核代表应用程序执行访问时，例如使用 read() 时：)

The kernel will send a SIGSEGV in both cases, but si_code will be set
to SEGV_PKERR when violating protection keys versus SEGV_ACCERR when
the plain mprotect() permissions are violated.
(在这两种情况下，内核都会发送 SIGSEGV，但违反保护密钥时 si_code 将被设置为 SEGV_PKERR，而违反普通 mprotect() 权限时则是 SEGV_ACCERR。)

Note that kernel accesses from a kthread (such as io_uring) will use a default
value for the protection key register and so will not be consistent with
userspace's value of the register or mprotect().
(请注意，来自 kthread（例如 io_uring）的内核访问将使用保护密钥寄存器的默认值，因此不会与用户空间中该寄存器的值或 mprotect() 保持一致。)
