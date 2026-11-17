.. SPDX-License-Identifier: GPL-2.0

===========================================================
Linux wireless regulatory documentation(Linux 无线监管文档)
===========================================================

This document gives a brief review over how the Linux wireless
regulatory infrastructure works.
(本文简要回顾 Linux 无线监管基础设施的工作方式。)

More up to date information can be obtained at the project's web page:
(更新的信息可从项目网页获取：)

https://wireless.wiki.kernel.org/en/developers/Regulatory

Keeping regulatory domains in userspace(将监管域保存在用户空间)
---------------------------------------------------------------

Due to the dynamic nature of regulatory domains we keep them
in userspace and provide a framework for userspace to upload
to the kernel one regulatory domain to be used as the central
core regulatory domain all wireless devices should adhere to.
(由于监管域具有动态性，我们将其保存在用户空间，并提供一个框架让用户空间向内核上传一个监管域，作为所有无线设备
都应遵循的中心核心监管域。)

How to get regulatory domains to the kernel(如何将监管域送入内核)
-----------------------------------------------------------------

When the regulatory domain is first set up, the kernel will request a
database file (regulatory.db) containing all the regulatory rules. It
will then use that database when it needs to look up the rules for a
given country.
(监管域首次建立时，内核将请求一个包含全部监管规则的数据库文件 (regulatory.db)。之后当需要查询某个国家的规则
时，内核就会使用该数据库。)

How to get regulatory domains to the kernel (old CRDA solution)(如何将监管域送入内核（旧 CRDA 方案）)
-----------------------------------------------------------------------------------------------------

Userspace gets a regulatory domain in the kernel by having
a userspace agent build it and send it via nl80211. Only
expected regulatory domains will be respected by the kernel.
(用户空间通过让一个用户空间代理构建监管域并通过 nl80211 发送，将监管域送入内核。内核只接受预期的监管域。)

A currently available userspace agent which can accomplish this
is CRDA - central regulatory domain agent. Its documented here:
(目前可用于完成此任务的用户空间代理是 CRDA——中央监管域代理。其文档见：

https://wireless.wiki.kernel.org/en/developers/Regulatory/CRDA

Essentially the kernel will send a udev event when it knows
it needs a new regulatory domain. A udev rule can be put in place
to trigger crda to send the respective regulatory domain for a
specific ISO/IEC 3166 alpha2.
(本质上，当内核知道自己需要新的监管域时，会发送一个 udev 事件。可以部署一条 udev 规则来触发 crda，为特定的
ISO/IEC 3166 alpha2 发送相应的监管域。)

Below is an example udev rule which can be used:
(下面是一条可用的 udev 规则示例：)

# Example file, should be put in /etc/udev/rules.d/regulatory.rules
KERNEL=="regulatory*", ACTION=="change", SUBSYSTEM=="platform", RUN+="/sbin/crda"

The alpha2 is passed as an environment variable under the variable COUNTRY.

Who asks for regulatory domains?
--------------------------------

* Users
  (用户)

Users can use iw:
(用户可以使用 iw：)

https://wireless.wiki.kernel.org/en/users/Documentation/iw

An example::
(一个示例：)

  # set regulatory domain to "Costa Rica"
  iw reg set CR

This will request the kernel to set the regulatory domain to
the specified alpha2. The kernel in turn will then ask userspace
to provide a regulatory domain for the alpha2 specified by the user
by sending a uevent.
(这将请求内核把监管域设置为指定的 alpha2。随后内核会反过来通过发送 uevent，要求用户空间为用户指定的 alpha2
提供一个监管域。)

* Wireless subsystems for Country Information elements
  (携带国家信息元素 (Country Information element) 的无线子系统)

The kernel will send a uevent to inform userspace a new
regulatory domain is required. More on this to be added
as its integration is added.
(内核会发送一个 uevent，通知用户空间需要新的监管域。待其集成工作加入后，本文将补充更多相关内容。)

* Drivers
  (驱动)

If drivers determine they need a specific regulatory domain
set they can inform the wireless core using regulatory_hint().
They have two options -- they either provide an alpha2 so that
crda can provide back a regulatory domain for that country or
they can build their own regulatory domain based on internal
custom knowledge so the wireless core can respect it.
(如果驱动确定需要设置特定的监管域，可以使用 regulatory_hint() 通知无线核心。它们有两种选择——要么提供一
个 alpha2，让 crda 为该国家返回监管域；要么基于内部自定义知识构建自己的监管域，让无线核心予以采纳。)

*Most* drivers will rely on the first mechanism of providing a
regulatory hint with an alpha2. For these drivers there is an additional
check that can be used to ensure compliance based on custom EEPROM
regulatory data. This additional check can be used by drivers by
registering on its struct wiphy a reg_notifier() callback. This notifier
is called when the core's regulatory domain has been changed. The driver
can use this to review the changes made and also review who made them
(driver, user, country IE) and determine what to allow based on its
internal EEPROM data. Devices drivers wishing to be capable of world
roaming should use this callback. More on world roaming will be
added to this document when its support is enabled.
(*大多数* 驱动会依赖第一种机制，即提供带 alpha2 的监管提示。对于这类驱动，还有一种额外的检查可用，以便基于
自定义 EEPROM 监管数据确保合规。驱动可以通过在其 struct wiphy 上注册一个 reg_notifier() 回调来使用这一额
外检查。当核心的监管域发生变更时会调用该通知器。驱动可以利用它审查所做的变更，并查明是谁做的变更（驱动、用户
还是国家 IE），再根据其内部 EEPROM 数据决定允许哪些内容。希望具备全球漫游 (world roaming) 能力的设备驱动应
使用此回调。当全球漫游支持启用后，本文会补充更多相关内容。)

Device drivers who provide their own built regulatory domain
do not need a callback as the channels registered by them are
the only ones that will be allowed and therefore *additional*
channels cannot be enabled.
(提供自己内置监管域的设备驱动无需回调，因为它们注册的信道是唯一会被允许的信道，因此*额外的* 信道无法被启用。)

Example code - drivers hinting an alpha2:(示例代码——驱动提示一个 alpha2：)
--------------------------------------------------------------------------

This example comes from the zd1211rw device driver. You can start
by having a mapping of your device's EEPROM country/regulatory
domain value to a specific alpha2 as follows::
(此示例来自 zd1211rw 设备驱动。你可以先建立一个映射表，把设备的 EEPROM 国家/监管域值映射到特定的 alpha2，
如下所示：)

  static struct zd_reg_alpha2_map reg_alpha2_map[] = {
	{ ZD_REGDOMAIN_FCC, "US" },
	{ ZD_REGDOMAIN_IC, "CA" },
	{ ZD_REGDOMAIN_ETSI, "DE" }, /* Generic ETSI, use most restrictive */
	{ ZD_REGDOMAIN_JAPAN, "JP" },
	{ ZD_REGDOMAIN_JAPAN_ADD, "JP" },
	{ ZD_REGDOMAIN_SPAIN, "ES" },
	{ ZD_REGDOMAIN_FRANCE, "FR" },

Then you can define a routine to map your read EEPROM value to an alpha2,
as follows::
(然后可以定义一个例程，把读到的 EEPROM 值映射为一个 alpha2，如下所示：)

  static int zd_reg2alpha2(u8 regdomain, char *alpha2)
  {
	unsigned int i;
	struct zd_reg_alpha2_map *reg_map;
		for (i = 0; i < ARRAY_SIZE(reg_alpha2_map); i++) {
			reg_map = &reg_alpha2_map[i];
			if (regdomain == reg_map->reg) {
			alpha2[0] = reg_map->alpha2[0];
			alpha2[1] = reg_map->alpha2[1];
			return 0;
		}
	}
	return 1;
  }

Lastly, you can then hint to the core of your discovered alpha2, if a match
was found. You need to do this after you have registered your wiphy. You
are expected to do this during initialization.
(最后，如果找到了匹配项，就可以把发现的 alpha2 提示给核心。你需要在注册 wiphy 之后再这样做，通常应在初始
化期间进行。)

::

	r = zd_reg2alpha2(mac->regdomain, alpha2);
	if (!r)
		regulatory_hint(hw->wiphy, alpha2);

Example code - drivers providing a built in regulatory domain:(示例代码——驱动提供内置监管域：)
----------------------------------------------------------------------------------------------

[NOTE: This API is not currently available, it can be added when required]
([注意：此 API 目前尚不可用，可在需要时添加])

If you have regulatory information you can obtain from your
driver and you *need* to use this we let you build a regulatory domain
structure and pass it to the wireless core. To do this you should
kmalloc() a structure big enough to hold your regulatory domain
structure and you should then fill it with your data. Finally you simply
call regulatory_hint() with the regulatory domain structure in it.
(如果你拥有可从驱动获取的监管信息，并且*确实* 需要使用此方式，我们允许你构建一个监管域结构并传给无线核心。
为此，你应 kmalloc() 一个足以容纳监管域结构的空间，然后用你的数据填充它。最后，只需携带监管域结构调用
regulatory_hint() 即可。)

Below is a simple example, with a regulatory domain cached using the stack.
Your implementation may vary (read EEPROM cache instead, for example).
(下面是一个简单示例，其中监管域使用栈来缓存。你的实现可能有所不同（例如改用 EEPROM 缓存读取）。)

Example cache of some regulatory domain::
(某个监管域的缓存示例：)

  struct ieee80211_regdomain mydriver_jp_regdom = {
	.n_reg_rules = 3,
	.alpha2 =  "JP",
	//.alpha2 =  "99", /* If I have no alpha2 to map it to */
	.reg_rules = {
		/* IEEE 802.11b/g, channels 1..14 */
		REG_RULE(2412-10, 2484+10, 40, 6, 20, 0),
		/* IEEE 802.11a, channels 34..48 */
		REG_RULE(5170-10, 5240+10, 40, 6, 20,
			NL80211_RRF_NO_IR),
		/* IEEE 802.11a, channels 52..64 */
		REG_RULE(5260-10, 5320+10, 40, 6, 20,
			NL80211_RRF_NO_IR|
			NL80211_RRF_DFS),
	}
  };

Then in some part of your code after your wiphy has been registered::
(然后，在你的 wiphy 注册之后的代码某处：)

	struct ieee80211_regdomain *rd;
	int size_of_regd;
	int num_rules = mydriver_jp_regdom.n_reg_rules;
	unsigned int i;

	size_of_regd = sizeof(struct ieee80211_regdomain) +
		(num_rules * sizeof(struct ieee80211_reg_rule));

	rd = kzalloc(size_of_regd, GFP_KERNEL);
	if (!rd)
		return -ENOMEM;

	memcpy(rd, &mydriver_jp_regdom, sizeof(struct ieee80211_regdomain));

	for (i=0; i < num_rules; i++)
		memcpy(&rd->reg_rules[i],
		       &mydriver_jp_regdom.reg_rules[i],
		       sizeof(struct ieee80211_reg_rule));
	regulatory_struct_hint(rd);

Statically compiled regulatory database(静态编译的监管数据库)
-------------------------------------------------------------

When a database should be fixed into the kernel, it can be provided as a
firmware file at build time that is then linked into the kernel.
(当数据库需要固化进内核时，可以在构建时以固件文件的形式提供，随后链接进内核。)
