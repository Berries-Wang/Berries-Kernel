.. SPDX-License-Identifier: GPL-2.0

=======================================
LSM/SeLinux secid(LSM/SeLinux 的 secid)
=======================================

flowi structure:
(flowi 结构：)

The secid member in the flow structure is used in LSMs (e.g. SELinux) to indicate
the label of the flow. This label of the flow is currently used in selecting
matching labeled xfrm(s).
(flow 结构中的 secid 成员被 LSM（例如 SELinux）用于指示该流的标签。此流标签目前
用于选择匹配的带标签 xfrm(s)。)

If this is an outbound flow, the label is derived from the socket, if any, or
the incoming packet this flow is being generated as a response to (e.g. tcp
resets, timewait ack, etc.). It is also conceivable that the label could be
derived from other sources such as process context, device, etc., in special
cases, as may be appropriate.
(若这是一个出站流，则标签派生自套接字（如果有的话），或派生自该流作为响应所
针对的入站数据包（例如 TCP 复位、timewait ack 等）。还可以设想，在特殊情况下
标签可酌情派生自进程上下文、设备等其他来源。)

If this is an inbound flow, the label is derived from the IPSec security
associations, if any, used by the packet.
(若这是一个入站流，则标签派生自该数据包所使用的 IPSec 安全关联（如果有的话）。)
