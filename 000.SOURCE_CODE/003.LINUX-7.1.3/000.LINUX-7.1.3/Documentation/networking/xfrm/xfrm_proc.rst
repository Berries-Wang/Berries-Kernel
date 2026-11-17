.. SPDX-License-Identifier: GPL-2.0

=====================================================================
XFRM proc - /proc/net/xfrm_* files(XFRM proc - /proc/net/xfrm_* 文件)
=====================================================================

Masahide NAKAMURA <nakam@linux-ipv6.org>


Transformation Statistics(转换统计)
-----------------------------------

The xfrm_proc code is a set of statistics showing numbers of packets
dropped by the transformation code and why.  These counters are defined
as part of the linux private MIB.  These counters can be viewed in
/proc/net/xfrm_stat.
(xfrm_proc 代码是一组统计信息，显示被转换 (transformation) 代码丢弃的
数据包数量及其原因。这些计数器被定义为 Linux 私有 MIB 的一部分。可以在
/proc/net/xfrm_stat 中查看这些计数器。)


Inbound errors(入站错误)
~~~~~~~~~~~~~~~~~~~~~~~~

XfrmInError:
	All errors which is not matched others
	(所有未被其他项匹配的错误)

XfrmInBufferError:
	No buffer is left
	(没有剩余缓冲区)

XfrmInHdrError:
	Header error
	(头错误)

XfrmInNoStates:
	No state is found
	i.e. Either inbound SPI, address, or IPsec protocol at SA is wrong
	(未找到状态
	即入站 SPI、地址或 SA 的 IPsec 协议有误)

XfrmInStateProtoError:
	Transformation protocol specific error
	e.g. SA key is wrong
	(转换协议特定错误
	例如 SA 密钥错误)

XfrmInStateModeError:
	Transformation mode specific error
	(转换模式特定错误)

XfrmInStateSeqError:
	Sequence error
	i.e. Sequence number is out of window
	(序列号错误
	即序列号超出窗口)

XfrmInStateExpired:
	State is expired
	(状态已过期)

XfrmInStateMismatch:
	State has mismatch option
	e.g. UDP encapsulation type is mismatch
	(状态有不匹配的选项
	例如 UDP 封装类型不匹配)

XfrmInStateInvalid:
	State is invalid
	(状态无效)

XfrmInTmplMismatch:
	No matching template for states
	e.g. Inbound SAs are correct but SP rule is wrong
	(没有与状态匹配的模板
	例如入站 SA 正确但 SP 规则错误)

XfrmInNoPols:
	No policy is found for states
	e.g. Inbound SAs are correct but no SP is found
	(没有为状态找到策略
	例如入站 SA 正确但未找到 SP)

XfrmInPolBlock:
	Policy discards
	(策略丢弃)

XfrmInPolError:
	Policy error
	(策略错误)

XfrmAcquireError:
	State hasn't been fully acquired before use
	(状态在使用前尚未被完全获取)

XfrmFwdHdrError:
	Forward routing of a packet is not allowed
	(不允许对数据包进行转发路由)

XfrmInStateDirError:
        State direction mismatch (lookup found an output state on the input path, expected input or no direction)
        (状态方向不匹配 (查找在输入路径上找到了输出状态，期望输入或无方向))

Outbound errors(出站错误)
~~~~~~~~~~~~~~~~~~~~~~~~~

XfrmOutError:
	All errors which is not matched others
	(所有未被其他项匹配的错误)

XfrmOutBundleGenError:
	Bundle generation error
	(束 (bundle) 生成错误)

XfrmOutBundleCheckError:
	Bundle check error
	(束检查错误)

XfrmOutNoStates:
	No state is found
	(未找到状态)

XfrmOutStateProtoError:
	Transformation protocol specific error
	(转换协议特定错误)

XfrmOutStateModeError:
	Transformation mode specific error
	(转换模式特定错误)

XfrmOutStateSeqError:
	Sequence error
	i.e. Sequence number overflow
	(序列号错误
	即序列号溢出)

XfrmOutStateExpired:
	State is expired
	(状态已过期)

XfrmOutPolBlock:
	Policy discards
	(策略丢弃)

XfrmOutPolDead:
	Policy is dead
	(策略已死亡)

XfrmOutPolError:
	Policy error
	(策略错误)

XfrmOutStateInvalid:
	State is invalid, perhaps expired
	(状态无效，可能已过期)

XfrmOutStateDirError:
        State direction mismatch (lookup found an input state on the output path, expected output or no direction)
        (状态方向不匹配 (查找在输出路径上找到了输入状态，期望输出或无方向))
