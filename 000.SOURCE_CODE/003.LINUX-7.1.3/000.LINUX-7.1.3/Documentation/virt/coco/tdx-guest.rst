.. SPDX-License-Identifier: GPL-2.0

===================================================================
TDX Guest API Documentation(TDX 客户机 API 文档)
===================================================================

1. General description(1. 总体描述)
===================================

The TDX guest driver exposes IOCTL interfaces via the /dev/tdx-guest misc
device to allow userspace to get certain TDX guest-specific details.
(TDX 客户机驱动通过 /dev/tdx-guest misc 设备暴露 IOCTL 接口，允许用户态
获取某些 TDX 客户机特有的细节信息。)

2. API description(2. API 描述)
===============================

In this section, for each supported IOCTL, the following information is
provided along with a generic description.
(在本节中，对于每个受支持的 IOCTL，除一般性描述外还提供以下信息。)

:Input parameters: Parameters passed to the IOCTL and related details.
(输入参数：传给 IOCTL 的参数及相关细节。)
:Output: Details about output data and return value (with details about
         the non common error values).
(输出：关于输出数据与返回值的细节 (包括非常见错误值的细节)。)

2.1 TDX_CMD_GET_REPORT0(获取 TDREPORT0 报告)
--------------------------------------------

:Input parameters: struct tdx_report_req
(输入参数：struct tdx_report_req)
:Output: Upon successful execution, TDREPORT data is copied to
         tdx_report_req.tdreport and return 0. Return -EINVAL for invalid
         operands, -EIO on TDCALL failure or standard error number on other
         common failures.
(输出：执行成功时，TDREPORT 数据被复制到 tdx_report_req.tdreport 并返回
0。操作数无效时返回 -EINVAL；TDCALL 失败时返回 -EIO；其他常见失败返回标准
错误号。)

The TDX_CMD_GET_REPORT0 IOCTL can be used by the attestation software to get
the TDREPORT0 (a.k.a. TDREPORT subtype 0) from the TDX module using
TDCALL[TDG.MR.REPORT].
(证明 (attestation) 软件可以使用 TDX_CMD_GET_REPORT0 IOCTL，通过
TDCALL[TDG.MR.REPORT] 从 TDX 模块获取 TDREPORT0 (即 TDREPORT 子类型 0)。)

A subtype index is added at the end of this IOCTL CMD to uniquely identify the
subtype-specific TDREPORT request. Although the subtype option is mentioned in
the TDX Module v1.0 specification, section titled "TDG.MR.REPORT", it is not
currently used, and it expects this value to be 0. So to keep the IOCTL
implementation simple, the subtype option was not included as part of the input
ABI. However, in the future, if the TDX Module supports more than one subtype,
a new IOCTL CMD will be created to handle it. To keep the IOCTL naming
consistent, a subtype index is added as part of the IOCTL CMD.
(此 IOCTL CMD 的末尾附加了一个子类型索引，以唯一标识特定子类型的 TDREPORT
请求。虽然 TDX 模块 v1.0 规范中标题为 "TDG.MR.REPORT" 的一节提到了子类型
选项，但目前并未使用，且期望该值为 0。因此，为保持 IOCTL 实现简单，子类型
选项未被纳入输入 ABI。不过，将来如果 TDX 模块支持多于一个子类型，将会创建
新的 IOCTL CMD 来处理它。为保持 IOCTL 命名的一致性，子类型索引作为 IOCTL
CMD 的一部分加入。)

Reference(参考资料)
-------------------

TDX reference material is collected here:
(TDX 参考资料汇集于此：)

https://www.intel.com/content/www/us/en/developer/articles/technical/intel-trust-domain-extensions.html

The driver is based on TDX module specification v1.0 and TDX GHCI specification v1.0.
(该驱动基于 TDX 模块规范 v1.0 和 TDX GHCI 规范 v1.0。)
