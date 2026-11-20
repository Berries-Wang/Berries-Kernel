.. SPDX-License-Identifier: GPL-2.0

=====================================================
Architecture Page Table Helpers(体系结构页表辅助函数)
=====================================================

Generic MM expects architectures (with MMU) to provide helpers to create, access
and modify page table entries at various level for different memory functions.
These page table helpers need to conform to a common semantics across platforms.
Following tables describe the expected semantics which can also be tested during
boot via CONFIG_DEBUG_VM_PGTABLE option. All future changes in here or the debug
test need to be in sync.
(通用 MM 期望(带 MMU 的)体系结构提供辅助函数，以便在不同的内存功能中创建、访问
和修改各层级的页表项。这些页表辅助函数需要在不同平台之间遵循共同的语义。下面的
各表描述了预期的语义，这些语义还可以在启动时通过 CONFIG_DEBUG_VM_PGTABLE 选项
进行测试。此处或调试测试中的所有未来改动都需要保持同步。)


PTE Page Table Helpers(PTE 页表辅助函数)
========================================

+---------------------------+--------------------------------------------------+
| pte_same                  | Tests whether both PTE entries are the same      |
|                           | 测试两个 PTE 条目是否相同                        |
+---------------------------+--------------------------------------------------+
| pte_present               | Tests a valid mapped PTE                         |
|                           | 测试一个有效的已映射 PTE                         |
+---------------------------+--------------------------------------------------+
| pte_young                 | Tests a young PTE                                |
|                           | 测试 young PTE                                   |
+---------------------------+--------------------------------------------------+
| pte_dirty                 | Tests a dirty PTE                                |
|                           | 测试 dirty PTE                                   |
+---------------------------+--------------------------------------------------+
| pte_write                 | Tests a writable PTE                             |
|                           | 测试可写 PTE                                     |
+---------------------------+--------------------------------------------------+
| pte_special               | Tests a special PTE                              |
|                           | 测试 special PTE                                 |
+---------------------------+--------------------------------------------------+
| pte_protnone              | Tests a PROT_NONE PTE                            |
|                           | 测试 PROT_NONE PTE                               |
+---------------------------+--------------------------------------------------+
| pte_soft_dirty            | Tests a soft dirty PTE                           |
|                           | 测试 soft dirty PTE                              |
+---------------------------+--------------------------------------------------+
| pte_swp_soft_dirty        | Tests a soft dirty swapped PTE                   |
|                           | 测试 soft dirty 的交换 PTE                       |
+---------------------------+--------------------------------------------------+
| pte_mkyoung               | Creates a young PTE                              |
|                           | 创建 young PTE                                   |
+---------------------------+--------------------------------------------------+
| pte_mkold                 | Creates an old PTE                               |
|                           | 创建 old PTE                                     |
+---------------------------+--------------------------------------------------+
| pte_mkdirty               | Creates a dirty PTE                              |
|                           | 创建 dirty PTE                                   |
+---------------------------+--------------------------------------------------+
| pte_mkclean               | Creates a clean PTE                              |
|                           | 创建 clean PTE                                   |
+---------------------------+--------------------------------------------------+
| pte_mkwrite               | Creates a writable PTE of the type specified by  |
|                           | the VMA.                                         |
|                           | 创建由 VMA 指定类型的可写 PTE                    |
+---------------------------+--------------------------------------------------+
| pte_mkwrite_novma         | Creates a writable PTE, of the conventional type |
|                           | of writable.                                     |
|                           | 创建常规可写类型的可写 PTE                       |
+---------------------------+--------------------------------------------------+
| pte_wrprotect             | Creates a write protected PTE                    |
|                           | 创建写保护 PTE                                   |
+---------------------------+--------------------------------------------------+
| pte_mkspecial             | Creates a special PTE                            |
|                           | 创建 special PTE                                 |
+---------------------------+--------------------------------------------------+
| pte_mksoft_dirty          | Creates a soft dirty PTE                         |
|                           | 创建 soft dirty PTE                              |
+---------------------------+--------------------------------------------------+
| pte_clear_soft_dirty      | Clears a soft dirty PTE                          |
|                           | 清除 soft dirty PTE                              |
+---------------------------+--------------------------------------------------+
| pte_swp_mksoft_dirty      | Creates a soft dirty swapped PTE                 |
|                           | 创建 soft dirty 的交换 PTE                       |
+---------------------------+--------------------------------------------------+
| pte_swp_clear_soft_dirty  | Clears a soft dirty swapped PTE                  |
|                           | 清除 soft dirty 的交换 PTE                       |
+---------------------------+--------------------------------------------------+
| pte_mknotpresent          | Invalidates a mapped PTE                         |
|                           | 使已映射的 PTE 无效                              |
+---------------------------+--------------------------------------------------+
| ptep_clear                | Clears a PTE                                     |
|                           | 清除一个 PTE                                     |
+---------------------------+--------------------------------------------------+
| ptep_get_and_clear        | Clears and returns PTE                           |
|                           | 清除并返回 PTE                                   |
+---------------------------+--------------------------------------------------+
| ptep_get_and_clear_full   | Clears and returns PTE (batched PTE unmap)       |
|                           | 清除并返回 PTE(成批的 PTE 解除映射)              |
+---------------------------+--------------------------------------------------+
| ptep_test_and_clear_young | Clears young from a PTE                          |
|                           | 清除 PTE 的 young 标志                           |
+---------------------------+--------------------------------------------------+
| ptep_set_wrprotect        | Converts into a write protected PTE              |
|                           | 转换为写保护的 PTE                               |
+---------------------------+--------------------------------------------------+
| ptep_set_access_flags     | Converts into a more permissive PTE              |
|                           | 转换为更宽松的 PTE                               |
+---------------------------+--------------------------------------------------+


PMD Page Table Helpers(PMD 页表辅助函数)
========================================

+---------------------------+--------------------------------------------------+
| pmd_same                  | Tests whether both PMD entries are the same      |
|                           | 测试两个 PMD 条目是否相同                        |
+---------------------------+--------------------------------------------------+
| pmd_bad                   | Tests a non-table mapped PMD                     |
|                           | 测试非表型的已映射 PMD                           |
+---------------------------+--------------------------------------------------+
| pmd_leaf                  | Tests a leaf mapped PMD                          |
|                           | 测试叶型的已映射 PMD                             |
+---------------------------+--------------------------------------------------+
| pmd_trans_huge            | Tests a Transparent Huge Page (THP) at PMD       |
|                           | 测试 PMD 处的透明大页 (THP)                      |
+---------------------------+--------------------------------------------------+
| pmd_present               | Tests whether pmd_page() points to valid memory  |
|                           | 测试 pmd_page() 是否指向有效内存                 |
+---------------------------+--------------------------------------------------+
| pmd_young                 | Tests a young PMD                                |
|                           | 测试 young PMD                                   |
+---------------------------+--------------------------------------------------+
| pmd_dirty                 | Tests a dirty PMD                                |
|                           | 测试 dirty PMD                                   |
+---------------------------+--------------------------------------------------+
| pmd_write                 | Tests a writable PMD                             |
|                           | 测试可写 PMD                                     |
+---------------------------+--------------------------------------------------+
| pmd_special               | Tests a special PMD                              |
|                           | 测试 special PMD                                 |
+---------------------------+--------------------------------------------------+
| pmd_protnone              | Tests a PROT_NONE PMD                            |
|                           | 测试 PROT_NONE PMD                               |
+---------------------------+--------------------------------------------------+
| pmd_soft_dirty            | Tests a soft dirty PMD                           |
|                           | 测试 soft dirty PMD                              |
+---------------------------+--------------------------------------------------+
| pmd_swp_soft_dirty        | Tests a soft dirty swapped PMD                   |
|                           | 测试 soft dirty 的交换 PMD                       |
+---------------------------+--------------------------------------------------+
| pmd_mkyoung               | Creates a young PMD                              |
|                           | 创建 young PMD                                   |
+---------------------------+--------------------------------------------------+
| pmd_mkold                 | Creates an old PMD                               |
|                           | 创建 old PMD                                     |
+---------------------------+--------------------------------------------------+
| pmd_mkdirty               | Creates a dirty PMD                              |
|                           | 创建 dirty PMD                                   |
+---------------------------+--------------------------------------------------+
| pmd_mkclean               | Creates a clean PMD                              |
|                           | 创建 clean PMD                                   |
+---------------------------+--------------------------------------------------+
| pmd_mkwrite               | Creates a writable PMD of the type specified by  |
|                           | the VMA.                                         |
|                           | 创建由 VMA 指定类型的可写 PMD                    |
+---------------------------+--------------------------------------------------+
| pmd_mkwrite_novma         | Creates a writable PMD, of the conventional type |
|                           | of writable.                                     |
|                           | 创建常规可写类型的可写 PMD                       |
+---------------------------+--------------------------------------------------+
| pmd_wrprotect             | Creates a write protected PMD                    |
|                           | 创建写保护 PMD                                   |
+---------------------------+--------------------------------------------------+
| pmd_mkspecial             | Creates a special PMD                            |
|                           | 创建 special PMD                                 |
+---------------------------+--------------------------------------------------+
| pmd_mksoft_dirty          | Creates a soft dirty PMD                         |
|                           | 创建 soft dirty PMD                              |
+---------------------------+--------------------------------------------------+
| pmd_clear_soft_dirty      | Clears a soft dirty PMD                          |
|                           | 清除 soft dirty PMD                              |
+---------------------------+--------------------------------------------------+
| pmd_swp_mksoft_dirty      | Creates a soft dirty swapped PMD                 |
|                           | 创建 soft dirty 的交换 PMD                       |
+---------------------------+--------------------------------------------------+
| pmd_swp_clear_soft_dirty  | Clears a soft dirty swapped PMD                  |
|                           | 清除 soft dirty 的交换 PMD                       |
+---------------------------+--------------------------------------------------+
| pmd_mkinvalid             | Invalidates a present PMD; do not call for       |
|                           | non-present PMD [1]                              |
|                           | 使已存在的 PMD 失效；不要对不存在的 PMD 调用 [1] |
+---------------------------+--------------------------------------------------+
| pmd_set_huge              | Creates a PMD huge mapping                       |
|                           | 创建 PMD 大页映射                                |
+---------------------------+--------------------------------------------------+
| pmd_clear_huge            | Clears a PMD huge mapping                        |
|                           | 清除 PMD 大页映射                                |
+---------------------------+--------------------------------------------------+
| pmdp_get_and_clear        | Clears a PMD                                     |
|                           | 清除一个 PMD                                     |
+---------------------------+--------------------------------------------------+
| pmdp_get_and_clear_full   | Clears a PMD                                     |
|                           | 清除一个 PMD                                     |
+---------------------------+--------------------------------------------------+
| pmdp_test_and_clear_young | Clears young from a PMD                          |
|                           | 清除 PMD 的 young 标志                           |
+---------------------------+--------------------------------------------------+
| pmdp_set_wrprotect        | Converts into a write protected PMD              |
|                           | 转换为写保护的 PMD                               |
+---------------------------+--------------------------------------------------+
| pmdp_set_access_flags     | Converts into a more permissive PMD              |
|                           | 转换为更宽松的 PMD                               |
+---------------------------+--------------------------------------------------+


PUD Page Table Helpers(PUD 页表辅助函数)
========================================

+---------------------------+--------------------------------------------------+
| pud_same                  | Tests whether both PUD entries are the same      |
|                           | 测试两个 PUD 条目是否相同                        |
+---------------------------+--------------------------------------------------+
| pud_bad                   | Tests a non-table mapped PUD                     |
|                           | 测试非表型的已映射 PUD                           |
+---------------------------+--------------------------------------------------+
| pud_leaf                  | Tests a leaf mapped PUD                          |
|                           | 测试叶型的已映射 PUD                             |
+---------------------------+--------------------------------------------------+
| pud_trans_huge            | Tests a Transparent Huge Page (THP) at PUD       |
|                           | 测试 PUD 处的透明大页 (THP)                      |
+---------------------------+--------------------------------------------------+
| pud_present               | Tests a valid mapped PUD                         |
|                           | 测试一个有效的已映射 PUD                         |
+---------------------------+--------------------------------------------------+
| pud_young                 | Tests a young PUD                                |
|                           | 测试 young PUD                                   |
+---------------------------+--------------------------------------------------+
| pud_dirty                 | Tests a dirty PUD                                |
|                           | 测试 dirty PUD                                   |
+---------------------------+--------------------------------------------------+
| pud_write                 | Tests a writable PUD                             |
|                           | 测试可写 PUD                                     |
+---------------------------+--------------------------------------------------+
| pud_mkyoung               | Creates a young PUD                              |
|                           | 创建 young PUD                                   |
+---------------------------+--------------------------------------------------+
| pud_mkold                 | Creates an old PUD                               |
|                           | 创建 old PUD                                     |
+---------------------------+--------------------------------------------------+
| pud_mkdirty               | Creates a dirty PUD                              |
|                           | 创建 dirty PUD                                   |
+---------------------------+--------------------------------------------------+
| pud_mkclean               | Creates a clean PUD                              |
|                           | 创建 clean PUD                                   |
+---------------------------+--------------------------------------------------+
| pud_mkwrite               | Creates a writable PUD                           |
|                           | 创建可写 PUD                                     |
+---------------------------+--------------------------------------------------+
| pud_wrprotect             | Creates a write protected PUD                    |
|                           | 创建写保护 PUD                                   |
+---------------------------+--------------------------------------------------+
| pud_mkinvalid             | Invalidates a present PUD; do not call for       |
|                           | non-present PUD [1]                              |
|                           | 使已存在的 PUD 失效；不要对不存在的 PUD 调用 [1] |
+---------------------------+--------------------------------------------------+
| pud_set_huge              | Creates a PUD huge mapping                       |
|                           | 创建 PUD 大页映射                                |
+---------------------------+--------------------------------------------------+
| pud_clear_huge            | Clears a PUD huge mapping                        |
|                           | 清除 PUD 大页映射                                |
+---------------------------+--------------------------------------------------+
| pudp_get_and_clear        | Clears a PUD                                     |
|                           | 清除一个 PUD                                     |
+---------------------------+--------------------------------------------------+
| pudp_get_and_clear_full   | Clears a PUD                                     |
|                           | 清除一个 PUD                                     |
+---------------------------+--------------------------------------------------+
| pudp_test_and_clear_young | Clears young from a PUD                          |
|                           | 清除 PUD 的 young 标志                           |
+---------------------------+--------------------------------------------------+
| pudp_set_wrprotect        | Converts into a write protected PUD              |
|                           | 转换为写保护的 PUD                               |
+---------------------------+--------------------------------------------------+
| pudp_set_access_flags     | Converts into a more permissive PUD              |
|                           | 转换为更宽松的 PUD                               |
+---------------------------+--------------------------------------------------+


HugeTLB Page Table Helpers(HugeTLB 页表辅助函数)
================================================

+---------------------------+--------------------------------------------------+
| pte_huge                  | Tests a HugeTLB                                  |
|                           | 测试 HugeTLB                                     |
+---------------------------+--------------------------------------------------+
| arch_make_huge_pte        | Creates a HugeTLB                                |
|                           | 创建 HugeTLB                                     |
+---------------------------+--------------------------------------------------+
| huge_pte_dirty            | Tests a dirty HugeTLB                            |
|                           | 测试 dirty HugeTLB                               |
+---------------------------+--------------------------------------------------+
| huge_pte_write            | Tests a writable HugeTLB                         |
|                           | 测试可写 HugeTLB                                 |
+---------------------------+--------------------------------------------------+
| huge_pte_mkdirty          | Creates a dirty HugeTLB                          |
|                           | 创建 dirty HugeTLB                               |
+---------------------------+--------------------------------------------------+
| huge_pte_mkwrite          | Creates a writable HugeTLB                       |
|                           | 创建可写 HugeTLB                                 |
+---------------------------+--------------------------------------------------+
| huge_pte_wrprotect        | Creates a write protected HugeTLB                |
|                           | 创建写保护 HugeTLB                               |
+---------------------------+--------------------------------------------------+
| huge_ptep_get_and_clear   | Clears a HugeTLB                                 |
|                           | 清除一个 HugeTLB                                 |
+---------------------------+--------------------------------------------------+
| huge_ptep_set_wrprotect   | Converts into a write protected HugeTLB          |
|                           | 转换为写保护的 HugeTLB                           |
+---------------------------+--------------------------------------------------+
| huge_ptep_set_access_flags  | Converts into a more permissive HugeTLB        |
|                           | 转换为更宽松的 HugeTLB                           |
+---------------------------+--------------------------------------------------+


SWAP Page Table Helpers(SWAP 页表辅助函数)
==========================================

+---------------------------+--------------------------------------------------+
| __pte_to_swp_entry        | Creates a swp_entry_t (arch) from a swap PTE     |
|                           | 从交换 PTE 创建 swp_entry_t (arch)               |
+---------------------------+--------------------------------------------------+
| __swp_entry_to_pte        | Creates a swap PTE from a swp_entry_t (arch)     |
|                           | 从 swp_entry_t (arch) 创建交换 PTE               |
+---------------------------+--------------------------------------------------+
| __pmd_to_swp_entry        | Creates a swp_entry_t (arch) from a swap PMD     |
|                           | 从交换 PMD 创建 swp_entry_t (arch)               |
+---------------------------+--------------------------------------------------+
| __swp_entry_to_pmd        | Creates a swap PMD from a swp_entry_t (arch)     |
|                           | 从 swp_entry_t (arch) 创建交换 PMD               |
+---------------------------+--------------------------------------------------+
| is_migration_entry        | Tests a migration (read or write) swapped entry  |
|                           | 测试迁移(读或写)交换项                           |
+-------------------------------+----------------------------------------------+
| is_writable_migration_entry   | Tests a write migration swapped entry        |
|                               | 测试写迁移交换项                             |
+-------------------------------+----------------------------------------------+
| make_readable_migration_entry | Creates a read migration swapped entry       |
|                               | 创建读迁移交换项                             |
+-------------------------------+----------------------------------------------+
| make_writable_migration_entry | Creates a write migration swapped entry      |
|                               | 创建写迁移交换项                             |
+-------------------------------+----------------------------------------------+

[1] https://lore.kernel.org/linux-mm/20181017020930.GN30832@redhat.com/
