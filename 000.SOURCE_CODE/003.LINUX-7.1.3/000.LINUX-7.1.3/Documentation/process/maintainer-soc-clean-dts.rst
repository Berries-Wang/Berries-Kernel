.. SPDX-License-Identifier: GPL-2.0

==============================================================================
SoC Platforms with DTS Compliance Requirements(有 DTS 合规要求的 SoC 平台)
==============================================================================

Overview(概述)
---------------

SoC platforms or subarchitectures should follow all the rules from
Documentation/process/maintainer-soc.rst.  This document referenced in
MAINTAINERS impose additional requirements listed below.
(SoC 平台或子架构应遵循 Documentation/process/maintainer-soc.rst 中的所有规则。在 MAINTAINERS 中被引用的本文档额外施加以下要求。)

Strict DTS DT Schema and dtc Compliance(严格 DTS DT Schema 与 dtc 合规)
------------------------------------------------------------------------

No changes to the SoC platform Devicetree sources (DTS files) should introduce
new ``make dtbs_check W=1`` warnings.  Warnings in a new board DTS, which are
results of issues in an included DTSI file, are considered existing, not new
warnings.  For series split between different trees (DT bindings go via driver
subsystem tree), warnings on linux-next are decisive.  The platform maintainers
have automation in place which should point out any new warnings.
(对 SoC 平台设备树源文件 (DTS 文件) 的任何改动都不应引入新的 ``make dtbs_check W=1`` 警告。新板卡 DTS 中由所含 DTSI 文件的问题导致的警告被视为既有警告而非新警告。对于拆分到不同子系统的补丁系列 (DT 绑定走驱动子系统树)，以 linux-next 上的警告为准。平台维护者已有自动化机制来指出任何新警告。)

If a commit introducing new warnings gets accepted somehow, the resulting
issues shall be fixed in reasonable time (e.g. within one release) or the
commit reverted.
(如果引入新警告的提交以某种方式被接受，则必须在合理时间内 (例如一个发布周期内) 修复所产生的问题，否则回退该提交。)
