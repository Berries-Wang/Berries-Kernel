.. SPDX-License-Identifier: GPL-2.0

======================
DCTCP (DataCenter TCP)
======================

(DCTCP(数据中心 TCP))

DCTCP is an enhancement to the TCP congestion control algorithm for data
center networks and leverages Explicit Congestion Notification (ECN) in
the data center network to provide multi-bit feedback to the end hosts.
(DCTCP 是针对数据中心网络的 TCP 拥塞控制算法的一种增强，它利用数据中心网络中的显式拥塞通知 (ECN) 向端主机提供多比特反馈。)

To enable it on end hosts::
(要在端主机上启用它，执行以下命令：)

  sysctl -w net.ipv4.tcp_congestion_control=dctcp
  sysctl -w net.ipv4.tcp_ecn_fallback=0 (optional)

All switches in the data center network running DCTCP must support ECN
marking and be configured for marking when reaching defined switch buffer
thresholds. The default ECN marking threshold heuristic for DCTCP on
switches is 20 packets (30KB) at 1Gbps, and 65 packets (~100KB) at 10Gbps,
but might need further careful tweaking.
(数据中心网络中所有运行 DCTCP 的交换机都必须支持 ECN 标记，并被配置为在达到预设的交换机缓冲区阈值时进行标记。DCTCP 在交换机上默认的 ECN 标记阈值启发式规则为：1Gbps 下 20 个数据包 (30KB),10Gbps 下 65 个数据包(约 100KB),但可能还需要进一步仔细调整。)

For more details, see below documents:
(更多细节请参见以下文档：)

Paper:
(论文：)

The algorithm is further described in detail in the following two
SIGCOMM/SIGMETRICS papers:
(该算法在以下两篇 SIGCOMM/SIGMETRICS 论文中有更详细的描述：)

 i) Mohammad Alizadeh, Albert Greenberg, David A. Maltz, Jitendra Padhye,
    Parveen Patel, Balaji Prabhakar, Sudipta Sengupta, and Murari Sridharan:

      "Data Center TCP (DCTCP)", Data Center Networks session"

      Proc. ACM SIGCOMM, New Delhi, 2010.

    (即论文《Data Center TCP (DCTCP)》，发表于 ACM SIGCOMM 2010 年会(新德里)的数据中心网络分会场。)

    http://simula.stanford.edu/~alizade/Site/DCTCP_files/dctcp-final.pdf
    http://www.sigcomm.org/ccr/papers/2010/October/1851275.1851192

ii) Mohammad Alizadeh, Adel Javanmard, and Balaji Prabhakar:

      "Analysis of DCTCP: Stability, Convergence, and Fairness"
      Proc. ACM SIGMETRICS, San Jose, 2011.

    (即论文《Analysis of DCTCP: Stability, Convergence, and Fairness》，发表于 ACM SIGMETRICS 2011 年会(圣何塞)。)

    http://simula.stanford.edu/~alizade/Site/DCTCP_files/dctcp_analysis-full.pdf

IETF informational draft:
(IETF 信息性草案：)

  http://tools.ietf.org/html/draft-bensley-tcpm-dctcp-00

DCTCP site:
(DCTCP 网站：)

  http://simula.stanford.edu/~alizade/Site/DCTCP.html
