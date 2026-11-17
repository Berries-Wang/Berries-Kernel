.. SPDX-License-Identifier: GPL-2.0

=============================================
ATM cxacru device driver(ATM cxacru 设备驱动)
=============================================

Firmware is required for this device: http://accessrunner.sourceforge.net/
(此设备需要固件： http://accessrunner.sourceforge.net/)

While it is capable of managing/maintaining the ADSL connection without the
module loaded, the device will sometimes stop responding after unloading the
driver and it is necessary to unplug/remove power to the device to fix this.
(虽然在没有加载模块的情况下它也能管理/维护 ADSL 连接，但在卸载驱动后设备
有时会停止响应，此时必须拔掉/切断设备电源才能解决。)

Note: support for cxacru-cf.bin has been removed. It was not loaded correctly
so it had no effect on the device configuration. Fixing it could have stopped
existing devices working when an invalid configuration is supplied.
(注意： 对 cxacru-cf.bin 的支持已被移除。它此前未能被正确加载，因此对设备
配置没有任何影响。修复它可能会导致在提供了无效配置时现有设备停止工作。)

There is a script cxacru-cf.py to convert an existing file to the sysfs form.
(有一个脚本 cxacru-cf.py 可以把现有文件转换为 sysfs 形式。)

Detected devices will appear as ATM devices named "cxacru". In /sys/class/atm/
these are directories named cxacruN where N is the device number. A symlink
named device points to the USB interface device's directory which contains
several sysfs attribute files for retrieving device statistics:
(被检测到的设备会显示为名为 "cxacru" 的 ATM 设备。在 /sys/class/atm/ 中，
它们是名为 cxacruN 的目录，其中 N 是设备编号。一个名为 device 的符号链接
指向 USB 接口设备的目录，该目录包含多个用于获取设备统计信息的 sysfs
属性文件：)

* adsl_controller_version
  (ADSL 控制器版本)

* adsl_headend
  (ADSL 局端信息)
* adsl_headend_environment
  (ADSL 局端环境信息)

	- Information about the remote headend.
	  (关于远端局端的信息。)

* adsl_config
  (ADSL 配置)

	- Configuration writing interface.
	  (配置写入接口。)
	- Write parameters in hexadecimal format <index>=<value>,
	  separated by whitespace, e.g.:
	  (以十六进制格式 <index>=<value> 写入参数，
	  以空白字符分隔，例如：)

		"1=0 a=5"

	- Up to 7 parameters at a time will be sent and the modem will restart
	  the ADSL connection when any value is set. These are logged for future
	  reference.
	  (一次最多发送 7 个参数，并且每当设置任何值时，调制解调器都会重启
	  ADSL 连接。这些操作会被记录下来以供将来参考。)

* downstream_attenuation (dB)
  (下行衰减 (dB))
* downstream_bits_per_frame
  (每帧下行比特数)
* downstream_rate (kbps)
  (下行速率 (kbps))
* downstream_snr_margin (dB)
  (下行信噪比余量 (dB))

	- Downstream stats.
	  (下行统计。)

* upstream_attenuation (dB)
  (上行衰减 (dB))
* upstream_bits_per_frame
  (每帧上行比特数)
* upstream_rate (kbps)
  (上行速率 (kbps))
* upstream_snr_margin (dB)
  (上行信噪比余量 (dB))
* transmitter_power (dBm/Hz)
  (发送功率 (dBm/Hz))

	- Upstream stats.
	  (上行统计。)

* downstream_crc_errors
  (下行 CRC 错误数)
* downstream_fec_errors
  (下行 FEC 错误数)
* downstream_hec_errors
  (下行 HEC 错误数)
* upstream_crc_errors
  (上行 CRC 错误数)
* upstream_fec_errors
  (上行 FEC 错误数)
* upstream_hec_errors
  (上行 HEC 错误数)

	- Error counts.
	  (错误计数。)

* line_startable
  (线路可启动)

	- Indicates that ADSL support on the device
	  is/can be enabled, see adsl_start.
	  (指示设备上的 ADSL 支持
	  已/可被启用，参见 adsl_start。)

* line_status
  (线路状态)

	 - "initialising"
	   (初始化中)
	 - "down"
	   (断开)
	 - "attempting to activate"
	   (尝试激活)
	 - "training"
	   (训练)
	 - "channel analysis"
	   (信道分析)
	 - "exchange"
	   (交换)
	 - "waiting"
	   (等待)
	 - "up"
	   (接通)

	Changes between "down" and "attempting to activate"
	if there is no signal.
	(如果没有信号，会在 "down" 与 "attempting to activate"
	之间来回变化。)

* link_status
  (链路状态)

	 - "not connected"
	   (未连接)
	 - "connected"
	   (已连接)
	 - "lost"
	   (丢失)

* mac_address
  (MAC 地址)

* modulation
  (调制方式)

	 - "" (when not connected)
	   ("" (未连接时))
	 - "ANSI T1.413"
	 - "ITU-T G.992.1 (G.DMT)"
	 - "ITU-T G.992.2 (G.LITE)"

* startup_attempts
  (启动尝试次数)

	- Count of total attempts to initialise ADSL.
	  (初始化 ADSL 的总尝试次数。)

To enable/disable ADSL, the following can be written to the adsl_state file:
(要启用/禁用 ADSL，可以向 adsl_state 文件写入以下内容：)

	 - "start"
	   (启动)
	 - "stop
	   (停止)
	 - "restart" (stops, waits 1.5s, then starts)
	   ("restart" (停止，等待 1.5 秒，然后启动))
	 - "poll" (used to resume status polling if it was disabled due to failure)
	   ("poll" (用于在状态轮询因失败而被禁用时恢复轮询))

Changes in adsl/line state are reported via kernel log messages::
(adsl/line 状态的变化通过内核日志消息报告：)

	[4942145.150704] ATM dev 0: ADSL state: running
	[4942243.663766] ATM dev 0: ADSL line: down
	[4942249.665075] ATM dev 0: ADSL line: attempting to activate
	[4942253.654954] ATM dev 0: ADSL line: training
	[4942255.666387] ATM dev 0: ADSL line: channel analysis
	[4942259.656262] ATM dev 0: ADSL line: exchange
	[2635357.696901] ATM dev 0: ADSL line: up (8128 kb/s down | 832 kb/s up)
