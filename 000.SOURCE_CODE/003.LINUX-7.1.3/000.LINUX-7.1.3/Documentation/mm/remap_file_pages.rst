===========================================================
remap_file_pages() system call(remap_file_pages() 系统调用)
===========================================================

The remap_file_pages() system call is used to create a nonlinear mapping,
that is, a mapping in which the pages of the file are mapped into a
nonsequential order in memory. The advantage of using remap_file_pages()
over using repeated calls to mmap(2) is that the former approach does not
require the kernel to create additional VMA (Virtual Memory Area) data
structures.
(remap_file_pages() 系统调用用于创建非线性映射，也就是把文件的页以非顺序
方式映射到内存中。与反复调用 mmap(2) 相比，使用 remap_file_pages() 的优势
在于前者不需要内核创建额外的 VMA（虚拟内存区域）数据结构。)

Supporting of nonlinear mapping requires significant amount of non-trivial
code in kernel virtual memory subsystem including hot paths. Also to get
nonlinear mapping work kernel need a way to distinguish normal page table
entries from entries with file offset (pte_file). Kernel reserves flag in
PTE for this purpose. PTE flags are scarce resource especially on some CPU
architectures. It would be nice to free up the flag for other usage.
(支持非线性映射需要在内核虚拟内存子系统（包括热点路径）中编写大量不平凡的
代码。此外，要让非线性映射工作，内核还需要一种方法来区分普通页表项与带文
件偏移的页表项（pte_file）。内核为此在 PTE 中保留了一个标志位。PTE 标志
是稀缺资源，在某些 CPU 体系结构上尤其如此。最好能把该标志位释放出来供其他
用途。)

Fortunately, there are not many users of remap_file_pages() in the wild.
It's only known that one enterprise RDBMS implementation uses the syscall
on 32-bit systems to map files bigger than can linearly fit into 32-bit
virtual address space. This use-case is not critical anymore since 64-bit
systems are widely available.
(幸运的是，现实中使用 remap_file_pages() 的用户并不多。据所知，只有一家企
业级 RDBMS 实现在 32 位系统上使用该系统调用来映射无法线性装入 32 位虚拟地
址空间的大文件。由于 64 位系统已经普及，这一用例已不再关键。)

The syscall is deprecated and replaced it with an emulation now. The
emulation creates new VMAs instead of nonlinear mappings. It's going to
work slower for rare users of remap_file_pages() but ABI is preserved.
(该系统调用已被弃用，现在由一个模拟实现来替代。模拟会创建新的 VMA 而非线
性映射。对 remap_file_pages() 的少数用户来说它会更慢，但 ABI 得以保留。)

One side effect of emulation (apart from performance) is that user can hit
vm.max_map_count limit more easily due to additional VMAs. See comment for
DEFAULT_MAX_MAP_COUNT for more details on the limit.
(模拟（除性能之外）的一个副作用是：由于额外的 VMA，用户更容易触及
vm.max_map_count 上限。关于该上限的更多细节见 DEFAULT_MAX_MAP_COUNT 的
注释。)
