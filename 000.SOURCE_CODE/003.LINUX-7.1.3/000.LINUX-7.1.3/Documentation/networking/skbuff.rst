.. SPDX-License-Identifier: GPL-2.0

struct sk_buff(struct sk_buff，即套接字缓冲区结构)
==================================================

:c:type:`sk_buff` is the main networking structure representing
a packet.
(:c:type:`sk_buff` 是表示数据包的主要网络结构。)

Basic sk_buff geometry(sk_buff 的基本结构)
------------------------------------------

.. kernel-doc:: include/linux/skbuff.h
   :doc: Basic sk_buff geometry

Shared skbs and skb clones(共享的 skb 与 skb 克隆)
--------------------------------------------------

:c:member:`sk_buff.users` is a simple refcount allowing multiple entities
to keep a struct sk_buff alive. skbs with a ``sk_buff.users != 1`` are referred
to as shared skbs (see skb_shared()).
(:c:member:`sk_buff.users` 是一个简单的引用计数，允许多个实体维持一个 struct sk_buff 存活。 ``sk_buff.users != 1`` 的 skb 被称为共享 skb (shared skbs)(参见 skb_shared())。)

skb_clone() allows for fast duplication of skbs. None of the data buffers
get copied, but caller gets a new metadata struct (struct sk_buff).
&skb_shared_info.refcount indicates the number of skbs pointing at the same
packet data (i.e. clones).
(skb_clone() 可以快速复制 skb。数据缓冲区不会被复制，但调用者会得到一个新的元数据结构 (struct sk_buff)。 &skb_shared_info.refcount 表示指向同一数据包数据的 skb 数量(即克隆)。)

dataref and headerless skbs(dataref 与无头 skb)
-----------------------------------------------

.. kernel-doc:: include/linux/skbuff.h
   :doc: dataref and headerless skbs

Checksum information(校验和信息)
--------------------------------

.. kernel-doc:: include/linux/skbuff.h
   :doc: skb checksums
