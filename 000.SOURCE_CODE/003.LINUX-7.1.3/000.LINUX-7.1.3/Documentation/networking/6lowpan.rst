.. SPDX-License-Identifier: GPL-2.0

==============================================
Netdev private dataroom for 6lowpan interfaces
==============================================

(6lowpan 接口的 netdev 私有数据区)

All 6lowpan able net devices, means all interfaces with ARPHRD_6LOWPAN,
must have "struct lowpan_priv" placed at beginning of netdev_priv.
(所有支持 6lowpan 的网络设备(即所有具有 ARPHRD_6LOWPAN 的接口)，都必须把 "struct lowpan_priv" 放在 netdev_priv 的开头。)

The priv_size of each interface should be calculate by::
(每个接口的 priv_size 应按如下方式计算：)

 dev->priv_size = LOWPAN_PRIV_SIZE(LL_6LOWPAN_PRIV_DATA);

Where LL_PRIV_6LOWPAN_DATA is sizeof linklayer 6lowpan private data struct.
To access the LL_PRIV_6LOWPAN_DATA structure you can cast::
(其中 LL_PRIV_6LOWPAN_DATA 是链路层 6lowpan 私有数据结构的大小。要访问 LL_PRIV_6LOWPAN_DATA 结构，可以把结果转换为：)

 lowpan_priv(dev)-priv;

to your LL_6LOWPAN_PRIV_DATA structure.
(也就是转换到你的 LL_6LOWPAN_PRIV_DATA 结构。)

Before registering the lowpan netdev interface you must run::
(在注册 lowpan netdev 接口之前，你必须运行：)

 lowpan_netdev_setup(dev, LOWPAN_LLTYPE_FOOBAR);

wheres LOWPAN_LLTYPE_FOOBAR is a define for your 6LoWPAN linklayer type of
enum lowpan_lltypes.
(其中 LOWPAN_LLTYPE_FOOBAR 是一个宏定义，对应你在 enum lowpan_lltypes 中的 6LoWPAN 链路层类型。)

Example to evaluate the private usually you can do::
(通常，可以用下面的示例来获取私有数据：)

 static inline struct lowpan_priv_foobar *
 lowpan_foobar_priv(struct net_device *dev)
 {
	return (struct lowpan_priv_foobar *)lowpan_priv(dev)->priv;
 }

 switch (dev->type) {
 case ARPHRD_6LOWPAN:
	lowpan_priv = lowpan_priv(dev);
	/* do great stuff which is ARPHRD_6LOWPAN related */
	switch (lowpan_priv->lltype) {
	case LOWPAN_LLTYPE_FOOBAR:
		/* do 802.15.4 6LoWPAN handling here */
		lowpan_foobar_priv(dev)->bar = foo;
		break;
	...
	}
	break;
 ...
 }

In case of generic 6lowpan branch ("net/6lowpan") you can remove the check
on ARPHRD_6LOWPAN, because you can be sure that these function are called
by ARPHRD_6LOWPAN interfaces.
(对于通用 6lowpan 分支 ("net/6lowpan")，可以去掉对 ARPHRD_6LOWPAN 的检查，因为可以确信这些函数只会被 ARPHRD_6LOWPAN 接口调用。)
