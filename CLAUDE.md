# 文档写作约定

## CPU 平台举例规范

**分析基座一律基于 arm64 硬件**：写这类内核笔记时，机制分析、流程图、数字举例都要落到 arm64 平台上展开——同构场景用 Arm AGI CPU，异构场景用 Cortex-A725/A520；x86_64（AMD EPYC 9996）仅作架构差异对照与补充，不承担主分析。

平台举例固定包含以下**三部分**，缺一不可——三者覆盖的场景互不可替代：

| # | 平台 | 覆盖场景（不可漏） | 关键规格速查 |
|---|------|------------------|-------------|
| 1 | **AMD EPYC 9996**（x86_64） | 同构 SMT 众核、大规模 NUMA（NPS1/2/4）、CCX/LLC 边界、amd-pstate(EPP/CPPC) 调频域、256 核隔离与部署 | 第 6 代 EPYC 9006 系列旗舰，代号 "Venice"，Zen 6c，256C/512T，8 CCD + 2 IOD，16 CCX × 64MB L3 = 1GB，基频 2.55GHz，TDP 600W |
| 2 | **Arm AGI CPU**（arm64，同构服务器） | arm64 主实例：ACPI/PPTT 拓扑、CPPC capacity、SLC=LLC、CMN 互连、DSU-120 Direct connect、同构无 EAS 的"有 EM 也未必有 EAS"场景 | SP113012（一律用核数最多的旗舰档；家族：SP113012=136C / SP113012S=128C / SP113012A=64C），Neoverse V3 核心，136 核/双 chiplet，Armv9.2-A，每核私有 L2 2MB/3MB，无 SMT |
| 3 | **Cortex-A725（大核）/ Cortex-A520（小核）**（arm64，异构 big.LITTLE） | **异构场景必须补上，不能漏**：capacity-dmips-mhz 归一化（1024/252）、SD_ASYM_CPUCAPACITY(_FULL)、EAS 启用与能量比较、性能域(PD)划分、CLS 域成活/退化（A520 对共享 L2 vs A725 私有 L2）——这些只有异构才有意义 | A725：乱序，每核私有 L2（8 路，128KB~1MB）；A520：顺序，每 complex（一对核）共享 L2（128~512KB）；同经 DSU-120 共享簇级 L3 |

## 组织方式

- **正文主实例**：**Arm AGI CPU（arm64）为分析基座**；EPYC 9996（x86_64）作架构差异对照补充；
- **异构部分**：Cortex-A725/A520 内容一般放在**附录**（标题如"附录：Cortex-A725 + A520 异构教学对照"），但**必须写**——凡主题涉及容量异构、EAS、大小核功耗取舍、cluster 域时，附录即为正文教学数字（如 cap=1024/252、PD 表、能量估算）的出处，不可省略。

## 内核源码版本

- 笔记若需结合 Linux 内核源码，**统一使用 [000.SOURCE_CODE/003.LINUX-7.1.3](000.SOURCE_CODE/003.LINUX-7.1.3)（Linux 7.1.3）** 这套代码；
- 代码引用规范：路径写成 `kernel/sched/xxx.c`，行号必须用 `grep -n` 实测（```c 代码块首行注释如 `// kernel/sched/topology.c:1803`），不得凭记忆编写代码；
- 历史对照：`000.SOURCE_CODE/000.LINUX-5.9` 仅作版本差异对比（如 "5.9 vs 7.1.3" 对照表）使用，主基准始终是 7.1.3。

## 原子指令写作规范

- **正文以 LSE 为主**：后续写原子指令相关笔记（示例代码、执行流程、争用分析）时，主路径一律用 **LSE 原子指令**（`LDADD`/`LDCLR`/`LDSET`/`LDEOR`/`SWP`/`CAS`…，含 `A/L/AL` 内存序后缀）；
- **LL/SC 写到附录**：LL/SC（`LDXR/STXR`）相关内容——独占监视器两级状态机、配对限制、ERG 粒度、cmpwait/WFE 等待（LSE 无等待类指令）、非标准 RMW、128 位原子（FEAT_LSE128 之前）、Armv8.0 基线兼容——统一放到**附录**（标题如"附录：LL/SC（LDXR/STXR）与独占监视器"），与上面"异构部分放附录"同一组织方式；正文只在需要对照时一句话带过并链接附录。附录里**不得写成"LL/SC 已淘汰"**，须注明其保留地；
- **术语**：RMW 首次出现时展开为"RMW（Read-Modify-Write，读-改-写）"，定义见 [001.UNIX-DOCS/008.同步原语/001.原子指令/README.md](001.UNIX-DOCS/008.同步原语/001.原子指令/README.md)；
- **对照表复用**：两路线全区别总表见 [004.LLSC与LSE的区别.md](001.UNIX-DOCS/008.同步原语/001.原子指令/004.LLSC与LSE的区别.md)，LL/SC→LSE 逐条替换对照与四块保留地见 [003.LLSC实现原理.md §8](001.UNIX-DOCS/008.同步原语/001.原子指令/003.LLSC实现原理.md)——新笔记链接这两处，不重复抄表。

## PDF/TRM 引用与摘录规范

整理笔记引用参考资料（ARM ARM、TRM、Learn the Architecture、书籍等 PDF）时：

- **出处必须落到"文档 + 章节 + 页码"**：写明哪份文档、哪一节（章节号和标题，如 "B2.8.2 Alignment of data accesses"、"Table A-233: CTR_EL0 bit descriptions"）、物理页码；分卷 PDF 用 "vol1(-1-3000.pdf) 物理页 309" 这类写法。**不得只给文件链接不给章节页码**；
- **出处必须可点击跳转**：引用写成 Markdown 链接 `[]()`，链接文字保留"卷号(分卷文件名) 物理页 N"，目标指向真实文件并带 `#page=N` 锚点（N 为该分卷内 1 起始的物理页），分卷 PDF 链接到对应分卷文件，如：`[vol1(-1-3000.pdf) 物理页 309](007.BOOKs/Arm%20Architecture%20Reference%20Manual%20for%20A-profile%20architecture/DDI0487_M_a_a_a-profile_architecture_reference_manual-1-3000.pdf#page=309)`；路径含空格用 `%20` 编码；
- **关键论断处摘录原文**：把支撑论断的关键原文句（英文原句 + 中文对照/转述）摘进笔记，放进文首「核心资料」表（列：原文摘录｜说明｜出处）或引用点旁——保证脱离 PDF 也能核对该论断的措辞；
- 摘录必须是**逐字节选**（允许省略号截断），不得凭记忆改写后伪装成原文；中文对照放在原文下方或 `<sup>…</sup>` 中；
- 勘误与复核结论要在笔记中留档（如 README 的「审核记录」表），注明依据的章节页码。

## 参考资料（TRM）

- Neoverse V3（AGI CPU 核心）：[arm_neoverse_v3_core_technical_reference_manual_107734_0002_06_en.pdf](007.BOOKs/arm_neoverse_v3_core_technical_reference_manual_107734_0002_06_en.pdf)；互连/SLC 另见 [arm_neoverse_cmn_700_trm_102308_0307_08_en.pdf](007.BOOKs/arm_neoverse_cmn_700_trm_102308_0307_08_en.pdf)
- Cortex-A725：[arm_cortex_a725_core_trm_107652_0001_04_en.pdf](007.BOOKs/arm_cortex_a725_core_trm_107652_0001_04_en.pdf)；Cortex-A520：[arm_cortex_a520_core_trm_102517_0001_04_en.pdf](007.BOOKs/arm_cortex_a520_core_trm_102517_0001_04_en.pdf)

## 内核文档双语翻译规范（Documentation/ 就地翻译）

对象：`000.SOURCE_CODE/003.LINUX-7.1.3/000.LINUX-7.1.3/Documentation/` 下英文内核文档的就地中英双语化。风格基线 = `arch/arm64/sve.rst`。**translations/ 下的 zh_CN/zh_TW/ja_JP 等原生译档不碰**（它们独立成段落的中文是正文，不是注释）。

### 基本格式

- **段落**：英文段末尾**同段**追加 `(中文译文。)`，不另起段；
- **列表**：译文与条目文本对齐，放条目内；
- **代码块/ASCII 图/`::` 字面块内容/内核源码**：一律不译；
- **表格**：grid 表单元格内补中文、简单表行尾补中文（允许破坏列对齐，**禁止增删边框行**）；
- `.. toctree::` / `.. kernel-doc::` / SPDX 行整块不动；
- **标题（含文档主标题与所有小节，定稿风格）**：中文并入标题行 —— `Title(中文)`，且**下划线（及 overline）必须按新标题的东亚列宽补长**：CJK 字符按 2 列计（`unicodedata.east_asian_width`），标题含 TAB 时先按 `expandtabs(8)` 展开再计宽。

### 高危禁忌（每条都踩过真实坑）

1. **严禁把中文行插在"标题行与下划线之间"**（`Title` / `(中文)` / `====`）。docutils 会**静默**把三行降级成普通段落——零报错零警告，标题节点消失、页面 `<title>` 变 `<no title>`、正文小节标题全部变成正文文字；Sphinx 对无标题 toctree 条目**直接丢弃**（仅一条 WARNING），父级 index 页于是"只剩标题"。旧批次已产生 3357 处此缺陷，2026-11-18 才全量修复；
2. **1-2 字符短下划线也是标题**（`TX` / `--`）。漏改它们会破坏 docutils 的**标题样式注册顺序**，后面某节突然报 SEVERE "Title level inconsistent"（tls-offload.rst 实例）；
3. **`::` 行后禁止直接跟中文注释**——字面块降级为 block quote（代码失去等宽格式），块内含 `----`/`====` 分隔行时直接 **SEVERE 中止构建**（squashfs.rst 实例）。正确做法："译文移块后"——注释放到字面块结束之后（空行隔开）；
4. **改了标题，隐式引用必须同步**：`` `Overview`_ `` / `Overview_` 这类引用的目标名 = 完整标题文本，inline 改名后失配报 `Unknown target name`。改写方案：**引用文本 = 目标节的完整双语标题**（`` `Overview(概述)`_ ``），链接文字与跳转后的标题一字不差；不要用插 `.. _label:` 的方案；
5. **中文注释行里不要复制英文引用标记**：`(使用 \`DIRECT_KEY policies\`_ 时...)` 这种注释内的英文引用会对不上改名后的目标——要么不带引用，要么用完整双语标题；
6. **脚注/尾注引用不要在中文注释里重复**：表格单元格英文含 `[#]_` 时，中文注释直接写"……"即可，再写一个 `[#]_` 就变成 2 引用对 1 定义（proc.rst 实例）；
7. **reST 标记不要照搬直译**：`_any_` 不是斜体语义，会被解析成引用标记（mmap_prepare.rst 报 Unknown target）——斜体用 `*任意一个*`；
8. **中文行紧贴上一节下划线（相邻节无空行）是合法构造**，不要当成坏标题去"修"；zh_CN 原生档的 overline 中文标题（`===` / `标题` / `===`）同样合法。
9. **flat-table 单元格 `-` 后双空格（`-  文字`，文字起始列=9）时，中文续行禁止按惯例缩进 8 列**——差 1 列即触发内核 rstFlatTable 解析器报 "two-level bullet list expected, but row N does not contain a second-level bullet list"，以 `SystemMessagePropagation` **中止整个构建**（sphinx parallel build error，return code 2）。中文续行（含多行英文单元格后的中文块及其折行）必须精确对齐**文字起始列** = 单元格 `-` 行缩进 + 1 + `-` 后空格数；TAB 续行按 tab_width=8 展开后比较。docutils 单机解析对形状静默，唯一 oracle=真实构建。实例：ext-ctrls-codec-stateless.rst 10 处（2026-11-18 修复，含 L1629/4205/4209/4213/4217/4707/4287/4299/5073）。

### 质检门（每批完成必过）

- **oracle = 真实构建，且必须 `gmake O=kernel-docs-output cleandocs` 后全量重建**。增量构建对未变更文件复用缓存 doctree 且**不重新输出警告**——"错误变少"是假象（364→18 的假修复教训）；
- 构建日志 grep 清零项：`SEVERE`、`toc.no_title`、`Unknown target name`、`Title level inconsistent`、`Title underline too short`、`SystemMessagePropagation`（flat-table 续行错列等中止类，见禁忌 9）；`Unexpected indentation`/`Malformed table` 属 `::`-家族遗留（存量约 405 条，round-2 QA 统一"译文移块后"处理，新批次不得新增）；
- 辅助扫描脚本在 [009.IGNORE/net-trans-tools/qa/](009.IGNORE/net-trans-tools/qa/)：`scan-broken-titles.py`（坏标题形状）、`fix-broken-titles.py`（机械修复，幂等）、`scan-colon-shape.py`（`::` 形状清单）；
- docutils 单机诊断会误报 `include::` 路径、未知指令/角色（sphinx 扩展），过滤后再看，最终以 sphinx 构建为准。
