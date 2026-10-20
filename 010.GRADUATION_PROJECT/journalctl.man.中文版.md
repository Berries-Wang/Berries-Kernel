# journalctl(1) 中文手册

> **译本说明**：译自 systemd 262（当前最新稳定版）的官方 man 页 `journalctl(1)`，原文抓取自 [freedesktop.org 官方渲染版](https://www.freedesktop.org/software/systemd/man/journalctl.html)（2026-10-20）。本仓库源码区无 systemd，故依据上游翻译；选项名、字段名、取值保留英文原样，章节标题附原文锚点便于对照。
> 「自版本 X 引入」对应原文 *Added in version X*。实战速查见 [journalctl命令手册.md](./journalctl命令手册.md)。

## 名称（NAME）

journalctl —— 打印 systemd 日志（journal）中存储的日志条目

## 概要（SYNOPSIS）

`journalctl` [OPTIONS...] [MATCHES...]

## 描述（DESCRIPTION）

`journalctl` 用于打印由 `systemd-journald.service`(8) 与 `systemd-journal-remote.service`(8) 写入 journal 的日志条目。

不带参数调用时，显示调用用户有权访问的 journal 内容，从最早收集的条目开始。

若传入一个或多个匹配参数（match），输出会据此过滤。匹配项格式为 "`FIELD=VALUE`"（字段=值），例如 "`_SYSTEMD_UNIT=httpd.service`"，对应结构化日志条目的各个字段。已知字段清单见 `systemd.journal-fields`(7)。若指定了多个**不同字段**的匹配项，日志条目须同时满足全部条件（逻辑 AND）。若两个匹配项作用于**同一字段**，则自动按备选处理（逻辑 OR）——输出显示满足其中任意一个的条目。最后，字符 "`+`" 可以作为独立单词出现在其他条件之间，使它前后的所有匹配项按析取（逻辑 OR）组合。

也可以把**绝对文件路径**作为参数来过滤条目。该路径可以是文件或符号链接，且查询时必须存在。若路径指向可执行二进制文件，则向查询添加规范路径的 "`_EXE=`" 匹配；若指向可执行脚本，则添加脚本名的 "`_COMM=`" 匹配；若指向设备节点，则添加该设备内核名及其各级祖先设备的 "`_KERNEL_DEVICE=`" 匹配。符号链接会被解引用，内核名在查询时合成，父设备也按查询时的环境识别。一般而言，设备节点是真实设备的最佳代理——日志条目通常不含直接标识真实设备的字段。要使结果对真实设备准确，记录条目时与查询时的相关环境（尤其是设备节点对应的真实设备）必须一致。由于设备节点对应的真实设备通常随重启而变化，指定设备节点路径会把结果限制在**本次启动**的条目内。

还可以用 `--boot`、`--unit=` 等选项追加约束，进一步收窄显示的条目（逻辑 AND）。

输出会从所有可访问的 journal 文件中交错（按时间合并）读取——无论它们是已归档的还是正在写入的，也不论属于系统本身还是其他可访问的用户 journal。`--header` 选项可用于识别正在显示的是哪些文件。

可用 `--user`、`--system`、`--directory=`、`--file=` 选项改变所用的 journal 文件集合，见下文。

所有用户都有权访问自己的**每用户私有 journal**。但默认只有 root 与少数特殊组的成员有权访问系统 journal 与其他用户的 journal。"`systemd-journal`"、"`adm`"、"`wheel`" 组的成员可以读取所有 journal 文件。注意后两个组传统上还持有发行版赋予的额外权限，"`wheel`" 组成员通常可以执行管理任务。

输出默认经 `less` 分页，长行会按屏幕宽度「截断」，被隐藏的部分可用左右方向键查看。分页可以关闭，见 `--no-pager` 选项及下文「环境变量」一节。

输出到终端时按优先级着色：ERROR 级及以上为红色；WARNING 级为黄色；NOTICE 级高亮；INFO 级正常显示；DEBUG 级灰色。

**向 journal 写入**条目有几种方法。一般而言，systemd 单元的输出会自动接入 journal，见 `systemd-journald.service`(8)；此外可用 `systemd-cat`(1) 直接向 journal 发送消息。

## 源选项（SOURCE OPTIONS）

以下选项控制从哪里读取 journal 记录：

- **`--system`, `--user`**

  显示系统服务与内核的消息（`--system`）；显示当前用户服务（user unit）的消息（`--user`）。两者都不指定时，显示该用户能看到的所有消息。

  `--user` 会影响 `--unit=` 参数的处理方式，见 `--unit=`。

  注意 `--user` 仅在通过 `journald.conf`(5) 的 `Storage=` 启用了持久化日志时才有效。

  自版本 205 引入。

- **`-M`, `--machine=`**

  显示一个**运行中的本地容器**里的消息。参数为要连接的容器名。

  自版本 209 引入。

- **`-m`, `--merge`**

  交错显示**所有**可用 journal（含远端 journal）的条目。

  自版本 190 引入。

- **`-D DIR`, `--directory=DIR`**

  取目录路径为参数。指定后 journalctl 将操作指定的 journal 目录 `DIR`，而非默认的运行时与系统 journal 路径。

  自版本 187 引入。

- **`-i GLOB`, `--file=GLOB`**

  取文件通配符为参数。指定后 journalctl 将操作匹配 `GLOB` 的 journal 文件，而非默认路径。可多次指定，各文件会适当交错合并。

  自版本 205 引入。

- **`--root=ROOT`**

  取目录路径为参数。指定后 journalctl 将操作指定目录之下的 journal 目录与消息目录（catalog）层级，而非根目录（例如 `--update-catalog` 会创建 `ROOT/var/lib/systemd/catalog/database`，而 `ROOT/run/journal/` 或 `ROOT/var/log/journal/` 之下的 journal 文件会被显示）。

  自版本 201 引入。

- **`--image=IMAGE`**

  取磁盘镜像文件或块设备节点的路径。指定后 journalctl 将操作该磁盘镜像内的文件系统。类似 `--root=`，但作用于磁盘镜像/块设备中存放的文件系统，为从磁盘镜像提取日志数据提供了便捷途径。磁盘镜像应只含一个文件系统，或遵循 UAPI.2 可发现分区规范（Discoverable Partitions Specification）的 GPT 分区表内的一组文件系统。所支持磁盘镜像的详细信息，见 `systemd-nspawn`(1) 同名开关。

  自版本 247 引入。

- **`--image-policy=policy`**

  按 `systemd.image-policy`(7) 取镜像策略串为参数。该策略在操作 `--image=` 指定的磁盘镜像时生效。未指定时默认为 "`*`" 策略，即使用镜像中所有可识别的文件系统。

- **`--namespace=NAMESPACE`**

  取 journal **命名空间**标识串为参数。未指定时显示默认命名空间收集的数据；指定时改为显示该命名空间的日志数据。命名空间指定为 "`*`" 时交错显示所有命名空间的数据；以 "`+`" 为前缀时，交错显示该命名空间与默认命名空间（不含其他）的数据。journal 命名空间的细节见 `systemd-journald.service`(8)。

  自版本 245 引入。

## 过滤选项（FILTERING OPTIONS）

以下选项控制如何过滤 journal 记录：

- **`-S`, `--since=`, `-U`, `--until=`**

  分别显示**不早于**（--since）／**不晚于**（--until）指定日期的条目。日期格式应为 "`2012-10-30 18:17:16`"：省略时间部分则视为 "`00:00:00`"；只省略秒则视为 ":00"；省略日期部分则视为当天。也接受字符串 "`yesterday`"、"`today`"、"`tomorrow`"，分别指前一天、当天、次日的 00:00:00；"`now`" 指当前时刻。最后还可以指定以 "`-`" 或 "`+`" 为前缀的**相对时间**，分别指当前时刻之前／之后。完整的时间日期规范见 `systemd.time`(7)。注意 `--output=short-full` 打印的时间戳正是这种格式。

  自版本 195 引入。

- **`-c`, `--cursor=`**

  从传入游标（cursor）指定的 journal 位置开始显示条目。

  自版本 193 引入。

- **`--after-cursor=`**

  从传入游标指定位置**之后**开始显示条目。游标由 `--show-cursor` 选项显示。

  自版本 206 引入。

- **`--cursor-file=FILE`**

  若 `FILE` 存在且含游标，则从该位置之后开始显示；否则按其他给定选项显示。结束时把最后一条条目的游标写入 `FILE`。用此选项可通过顺序调用 journalctl 持续读取 journal。

  自版本 242 引入。

- **`-b [ID][±offset]|all`, `--boot[=[ID][±offset]|all]`**

  显示某一次启动（boot）的消息。这会添加 "`_BOOT_ID=`" 匹配。

  参数可以为空，此时显示**当前**启动的日志。

  若省略启动 ID 而只给 `offset`：正偏移从 journal 开头往后数，小于等于零的偏移从 journal 末尾往前数。于是 `1` 指时间顺序上 journal 里第一次启动、`2` 指第二次，依此类推；`-0` 是最后一次启动、`-1` 是倒数第二次，依此类推。空偏移等价于 `-0`，除非当前启动不是最后一次启动（例如用 `--directory=` 查看来自另一台机器的日志时）。

  若指定 32 字符的 `ID`，其后可跟 `offset`，表示相对该 ID 所指启动的偏移：负值是更早的启动，正值是更晚的启动。不指定 `offset` 时按 0 计，显示该 ID 所指启动的日志。

  特殊参数 `all` 可用于抵消先前 `-b` 的效果。

  自版本 186 引入。

- **`-u`, `--unit=UNIT|PATTERN`**

  显示指定 systemd 单元 `UNIT`（如某个 service 单元）的消息，或匹配 `PATTERN` 的所有单元的消息。指定模式时，会把 journal 中找到的单元名清单与模式比对，所有匹配者均被采用。对每个单元名，除了该单元自身的消息匹配（"`_SYSTEMD_UNIT=`UNIT`"）外，还会附加来自 systemd 的消息以及该单元 coredump 消息的匹配。同时也会添加 "`_SYSTEMD_SLICE=`UNIT`" 匹配——因此若所给 `UNIT` 是一个 `systemd.slice`(5) 单元，则该 slice 全部子单元的日志都会显示。

  与 `--user` 连用时，所有 `--unit=` 参数会被转换为按用户单元匹配（等价于 `--user-unit=`）。

  此参数可多次指定。

  自版本 195 引入。

- **`--user-unit=`**

  显示指定**用户会话单元**的消息。会添加该单元消息的匹配（"`_SYSTEMD_USER_UNIT=`" 与 "`_UID=`"），并附加来自会话 systemd 的消息以及该单元 coredump 消息的匹配。同时也会添加 "`_SYSTEMD_USER_SLICE=`UNIT`" 匹配——若所给 `UNIT` 是 `systemd.slice`(5) 单元，则其全部子单元的日志都会显示。

  此参数可多次指定。

  自版本 198 引入。

- **`-I`, `--invocation=ID[±offset]|offset`**

  显示单元**某一次运行实例（invocation）**的消息。这会添加 "`_SYSTEMD_INVOCATION_ID=`"、"`OBJECT_SYSTEMD_INVOCATION_ID=`"、"`INVOCATION_ID=`"、"`USER_INVOCATION_ID=`" 匹配。

  正 `offset` 从 journal 开头往后数某单元的运行实例，零或负 `offset` 从末尾往前数。于是 `1` 指时间顺序上第一次运行、`2` 指第二次，依此类推；`0` 是最近一次运行、`-1` 是再上一次，依此类推。

  若指定 32 字符的 `ID`，其后可跟 `±offset`，表示相对该 ID 所指运行实例的偏移：负值更早、正值更晚。不指定时按 0 计，显示该 ID 所指运行的日志。

  `-I` 等价于 `--invocation=0`，显示最近一次运行的日志。

  指定 offset 时，必须同时用 `-u/--unit=` 或 `--user-unit=` 指定单元名。

  与 `-b/--boot=` 连用时，只在指定启动内检索运行实例。

  自版本 257 引入。

- **`-t`, `--identifier=SYSLOG_IDENTIFIER`**

  显示指定 syslog 标识（identifier）的消息。

  此参数可多次指定。

  自版本 217 引入。

- **`-T`, `--exclude-identifier=SYSLOG_IDENTIFIER`**

  **排除**指定 syslog 标识的消息。

  此参数可多次指定。

  自版本 256 引入。

- **`-p`, `--priority=`**

  按消息优先级或优先级**区间**过滤输出。取单个数字或文本日志级别（即 0/"`emerg`" 到 7/"`debug`" 之间），或 `FROM..TO` 形式的数字/文本级别区间。日志级别即 `syslog`(3) 所述的常规 syslog 级别："`emerg`"(0)、"`alert`"(1)、"`crit`"(2)、"`err`"(3)、"`warning`"(4)、"`notice`"(5)、"`info`"(6)、"`debug`"(7)。指定单个级别时，显示**该级别及更低（即更重要）**级别的全部消息；指定区间时，显示区间内（含两端）的全部消息。这会为指定优先级添加 "`PRIORITY=`" 匹配。

  自版本 188 引入。

- **`--facility=`**

  按 syslog 设施（facility）过滤输出。取逗号分隔的数字或设施名列表，名称为 `syslog`(3) 所述的常规 syslog 设施。`--facility=help` 可列出已知设施名并退出。

  自版本 245 引入。

- **`-g`, `--grep=`**

  把输出过滤为 `MESSAGE=` 字段匹配指定正则表达式的条目。使用 PERL 兼容正则，语法详见 `pcre2pattern`(3)。

  若模式全为小写，匹配不区分大小写；否则区分大小写。可用 `--case-sensitive` 选项覆盖（见下）。

  与 `--lines=`（数字不以 "`+`" 为前缀）连用时，隐含 `--reverse`。

  自版本 237 引入。

- **`--case-sensitive[=BOOLEAN]`**

  使模式匹配区分／不区分大小写。

  自版本 237 引入。

- **`-k`, `--dmesg`**

  **只显示内核消息**。添加 "`_TRANSPORT=kernel`" 匹配。除非显式另行指定，隐含 `--boot=0`。

  自版本 205 引入。

## 输出选项（OUTPUT OPTIONS）

以下选项控制 journal 记录的打印方式：

- **`-o`, `--output=`**

  控制所显示日志条目的格式，取以下值之一：

  - **`short`**：默认值。生成与经典 syslog 文件格式基本相同的输出，每条日志一行。

    自版本 206 引入。

  - **`short-full`**：与 `short` 非常相似，但时间戳采用 `--since=`/`--until=` 所接受的格式。与 `short` 的时间戳不同，此模式包含星期、年份与时区信息，且与 locale 无关。

    自版本 232 引入。

  - **`short-iso`**：与 `short` 非常相似，但时间戳为 ISO 8601 的 RFC 3339 子集。

    自版本 206 引入。

  - **`short-iso-precise`**：同 `short-iso`，但带完整微秒精度。

    自版本 234 引入。

  - **`short-precise`**：与 `short` 非常相似，但经典 syslog 时间戳带完整微秒精度。

    自版本 207 引入。

  - **`short-monotonic`**：与 `short` 非常相似，但显示**单调时钟**时间戳而非墙上时钟时间戳。

    自版本 206 引入。

  - **`short-delta`**：同 `short-monotonic`，但附带与上一条条目的时间差。不可靠的时间差以 "`*`" 标记。

    自版本 252 引入。

  - **`short-unix`**：与 `short` 非常相似，但显示自 1970-01-01 UTC 起的秒数（"UNIX 时间"）而非墙上时钟时间戳，微秒精度。

    自版本 230 引入。

  - **`verbose`**：显示条目的完整结构，含全部字段。

    自版本 206 引入。

  - **`export`**：把 journal 序列化为适合备份与网络传输的（以文本为主的）二进制流（格式详见 Journal Export Format）。要把该二进制流导回原生 journald 格式，使用 `systemd-journal-remote`(8)。

    自版本 206 引入。

  - **`json`**：把条目格式化为 JSON 对象，以换行符分隔（格式详见 Journal JSON Format）。字段值一般编码为 JSON 字符串，有三个例外：

    - 大于 4096 字节的字段编码为 `null` 值（传入 `--all` 可关闭此行为，但注意可能生成过长的 JSON 对象）。
    - journal 条目允许同一条目内出现**重复字段**，而 JSON 不允许对象内有重复键。因此遇到重复字段时，字段值改用 JSON 数组，把所有字段值列为元素。
    - 含不可打印或非 UTF-8 字节的字段，编码为逐字节按无符号数表示的数组。

    注意此编码是可逆的（大小限制除外）。

    自版本 206 引入。

  - **`json-pretty`**：把条目格式化为 JSON 数据结构，但分成多行以便人工阅读。

    自版本 206 引入。

  - **`json-sse`**：把条目格式化为 JSON 数据结构，但包装为适合 Server-Sent Events 的格式。

    自版本 206 引入。

  - **`json-seq`**：把条目格式化为 JSON 数据结构，但按 JSON 文本序列（"application/json-seq"，RFC 7464）加前缀 ASCII 记录分隔符（0x1E）、后缀 ASCII 换行符（0x0A）。

    自版本 240 引入。

  - **`cat`**：生成极简输出，只显示每条日志的消息本身，不含任何元数据（连时间戳也没有）。与 `--output-fields=` 连用时，改为输出每条记录所列的字段而非消息。

    自版本 206 引入。

  - **`with-unit`**：类似 `short-full`，但前缀使用单元名与用户单元名，替代传统 syslog 标识。使用模板实例时很有用，单元名中会包含实例参数。

    自版本 239 引入。

- **`--truncate-newline`**

  输出时把每条日志消息截断到第一个换行符，即只显示每条消息的第一行。

  自版本 254 引入。

- **`--output-fields=`**

  逗号分隔的、应包含在输出中的字段列表。仅对通常显示全部字段的输出模式（`verbose`、`export`、`json`、`json-pretty`、`json-sse`、`json-seq`）以及 `cat` 生效。对前者，"`__CURSOR`"、"`__REALTIME_TIMESTAMP`"、"`__MONOTONIC_TIMESTAMP`"、"`_BOOT_ID`" 字段始终打印。

  自版本 236 引入。

- **`-n`, `--lines=`**

  显示最近的日志事件并限制条数。参数为正整数，或 "`all`" 表示不限制。此外，若数字以 "`+`" 为前缀，则改为显示**最旧**的日志事件。不带参数时默认 10。

  使用 `--follow` 时隐含此选项。不以 "`+`" 为前缀且与 `--grep=` 连用时，隐含 `--reverse`。

- **`-r`, `--reverse`**

  反转输出，使最新条目排在最前。

  自版本 198 引入。

- **`--show-cursor`**

  在最后一条条目之后以两道短横线显示游标：

  ```
  -- cursor: s=0639…
  ```

  游标格式属私有实现，可能变更。

  自版本 209 引入。

- **`--utc`**

  以协调世界时（UTC）表示时间。

  自版本 217 引入。

- **`-x`, `--catalog`**

  用消息目录（catalog）中的解释文本增补日志行：在输出中为日志消息附加可用的说明文字。这些简短说明会解释错误或日志事件的背景、可能的解决方案，以及支持论坛、开发者文档和其他相关手册的指引。注意说明文字并非对所有消息可用，只覆盖精选的部分。消息目录的更多信息见 Journal Message Catalogs。

  注意：把 journalctl 输出附到 bug 报告时，请**不要**使用 `-x`。

  自版本 196 引入。

- **`-W`, `--no-hostname`**

  不显示日志消息中的主机名字段。此开关仅对 `short` 系列输出模式（见上）生效。

  注意：该选项不会移除日志条目**内容本身**中出现的主机名，因此不能防止主机名在日志里可见。

  自版本 230 引入。

- **`--no-full`, `--full`, `-l`**

  字段超出可用列宽时打省略号。默认显示完整字段，允许折行或由分页器截断。

  旧选项 `-l`/`--full` 除用于抵消 `--no-full` 外已无实际作用。

  自版本 196 引入。

- **`-a`, `--all`**

  完整显示所有字段，即使含不可打印字符或非常长。默认情况下，含不可打印字符的字段缩写为 "blob data"（注意分页器可能再次转义不可打印字符）。

- **`-f`, `--follow`**

  只显示最近的日志条目，并持续打印追加到 journal 的新条目，直到按下 Ctrl-C（或工具被以其他方式终止）。

  journalctl 完成初始化并成功建立对 journal 的监视后，会发送 `sd_notify`(3) 的 "`READY=1`" 消息。

- **`--no-tail`**

  显示全部已存储的输出行，即使处于 follow 模式。用于抵消 `--lines=` 的效果。

- **`-q`, `--quiet`**

  抑制所有信息性消息（即 "-- Journal begins at …"、"-- Reboot --"），以及以普通用户运行时有关系统 journal 不可访问的警告消息。

- **`--synchronize-on-exit=`**

  取布尔参数。为 true 且以 `--follow` 模式运行时，收到 `SIGTERM`/`SIGINT` 后会发出一次 journal 同步请求（等价于 `journalctl --sync`），并继续日志输出直至该请求完成。这适用于把 journal 日志输出与服务或外部事件的运行期对齐：确保发出 `SIGTERM`/`SIGINT` 时已排入日志子系统的任何日志数据，在输出结束前一定被处理并显示。默认为 false。

  自版本 258 引入。

## 分页器控制选项（PAGER CONTROL OPTIONS）

以下选项控制分页支持：

- **`--no-pager`**

  不把输出导入分页器。

- **`-e`, `--pager-end`**

  在隐含启用的分页器中直接跳到 journal 末尾。除非显式另行指定，隐含 `--lines=1000` 和 `--boot=0`，以保证分页器不会缓冲无上限的日志。注意此选项仅支持 `less`(1) 分页器。

  自版本 198 引入。

## 前向安全密封（FSS）选项（FORWARD SECURE SEALING (FSS) OPTIONS）

以下选项与下文所述 `--setup-keys` 命令配合使用：

- **`--interval=`**

  指定用 `--setup-keys` 生成 FSS 密钥对时密封密钥的更换间隔。间隔越短 CPU 消耗越大，但 journal 被篡改而不被察觉的时间窗越短。默认 15 分钟。

  注意：`--output=json-sse` 与 `--output=json-seq` 会被静默地按 `--output=json` 处理。

  自版本 189 引入。

- **`--verify-key=`**

  为 `--verify` 操作指定 FSS 验证密钥。

  自版本 189 引入。

- **`--force`**

  已配置前向安全密封（FSS）的情况下，传入 `--setup-keys` 时重建 FSS 密钥。

  自版本 206 引入。

## 命令（COMMANDS）

理解以下命令。若都不指定，默认行为是显示 journal 记录：

- **`-N`, `--fields`**

  打印 journal 全部条目中当前使用的所有字段名。

  自版本 229 引入。

- **`-F`, `--field=`**

  打印指定字段在 journal 全部条目中取过的所有可能取值。

  自版本 195 引入。

- **`--list-boots`**

  以表格列出启动序号（相对当前启动）、启动 ID、以及该次启动首条与末条消息的时间戳。与 `-n/--lines=[+]N` 连用时只显示最前（数字以 "`+`" 为前缀）或最后（无前缀）`N` 行；与 `-r/--reverse` 连用时按相反顺序列出。

  自版本 209 引入。

- **`--list-invocations`**

  列出某单元的运行实例（invocation）ID。须用 `-u/--unit=` 或 `--user-unit=` 指定单元名。以表格列出运行实例序号（相对当前或最近一次运行）、其实例 ID、及该次运行首条与末条消息的时间戳。指定 `-b/--boot=` 时只列出该次启动内的运行实例。与 `-n/--lines=[+]N` 连用时只显示最前（前缀 "`+`"）或最后（无前缀）`N` 行；与 `-r/--reverse` 连用时按相反顺序列出。

  自版本 257 引入。

- **`--disk-usage`**

  显示所有 journal 文件当前的磁盘占用，即全部已归档与活跃 journal 文件占用之和。

  自版本 190 引入。

- **`--vacuum-size=`, `--vacuum-time=`, `--vacuum-files=`**

  `--vacuum-size=`：删除最旧的**已归档** journal 文件，直至其磁盘占用降到指定值以下。接受常用的 "`K`"、"`M`"、"`G`"、"`T`" 后缀（1024 进制）。

  `--vacuum-time=`：删除早于指定时长的已归档 journal 文件。接受常用的 "`s`"（默认）、"`m`"、"`h`"、"`days`"、"`weeks`"、"`months`"、"`years`" 后缀，详见 `systemd.time`(7)。

  `--vacuum-files=`：只保留指定数量的独立 journal 文件。

  注意 `--vacuum-size=` 对 `--disk-usage` 显示值只有间接影响——后者包含活跃 journal 文件，而清理操作只作用于已归档文件。类似地，`--vacuum-files=` 未必真能把文件数降到指定值以下，因为它不会删除活跃 journal 文件。

  `--vacuum-size=`、`--vacuum-time=`、`--vacuum-files=` 可在一次调用中组合使用，对已归档文件同时施加容量、时长与数量限制。三者取零等价于不施加该项限制，属冗余。

  这三个开关还可与 `--rotate` 合并为一条命令。此时先轮转全部活跃文件，随后立即执行清理。轮转使当前全部活跃文件被归档（并可能打开新的空 journal 文件替代），因此清理能覆盖迄今为止写入的全部日志数据，效果最大。

  自版本 218 引入。

- **`--verify`**

  检查 journal 文件的内部一致性。若文件是在启用 FSS 的前提下生成的，且用 `--verify-key=` 指定了 FSS 验证密钥，则同时验证 journal 文件的真实性。

  自版本 189 引入。

- **`--sync`**

  要求 journal 守护进程把所有尚未落盘的日志数据写入底层文件系统并同步所有 journal。该调用直到同步操作完成才返回。此命令保证：其返回时，调用之前写入的全部日志消息已安全落盘。

  自版本 228 引入。

- **`--relinquish-var`**

  要求 journal 守护进程执行 `--flush` 的逆操作：守护进程随后把日志数据进一步写入 `/run/log/journal/`，并停止写入 `/var/log/journal/`。之后再调用 `--flush` 会把日志输出切回 `/var/log/journal/`。

  自版本 243 引入。

- **`--smart-relinquish-var`**

  类似 `--relinquish-var`，但若根文件系统与 `/var/log/journal/` 位于同一挂载点则不执行任何操作。此操作用于系统关机时，让 journal 守护进程在该目录位于需要卸载的挂载点上的情况下，停止向 `/var/log/journal/` 写数据。

  自版本 243 引入。

- **`--flush`**

  若启用了持久化存储，要求 journal 守护进程把 `/run/log/journal/` 中存放的日志数据冲刷到 `/var/log/journal/`。该调用直到操作完成才返回。注意此调用是幂等的：系统运行期间数据只会从 `/run/log/journal/` 冲刷到 `/var/log/journal/` 一次（另见下文 `--relinquish-var`），若已发生过，此命令不执行任何操作而干净退出。此命令有效保证：其返回时，所有数据已冲刷到 `/var/log/journal/`。

  自版本 217 引入。

- **`--rotate`**

  要求 journal 守护进程轮转 journal 文件。该调用直到轮转完成才返回。轮转的效果是当前全部活跃 journal 文件被标记为已归档并重命名，今后不再写入；随后在原位创建新的（空的）journal 文件。此操作可与 `--vacuum-size=`、`--vacuum-time=`、`--vacuum-files=` 合并为一条命令，见上文。

  自版本 227 引入。

- **`--header`**

  不显示 journal 内容，而是显示所访问 journal 文件的内部头部信息。

  在排查**乱序**日志条目时尤其有用，例如机器以错误的系统时间启动的情形。

  自版本 187 引入。

- **`--list-catalog [128-bit-ID…]`**

  以消息 ID 加简短描述串的表格形式列出消息目录内容。

  若指定了任何 `128-bit-ID`，则只显示那些条目。

  自版本 196 引入。

- **`--dump-catalog [128-bit-ID…]`**

  显示消息目录内容，条目以两道短横线加 ID 的行分隔（格式与 `.catalog` 文件相同）。

  若指定了任何 `128-bit-ID`，则只显示那些条目。

  自版本 199 引入。

- **`--update-catalog`**

  更新消息目录索引。每当安装、删除或更新了 catalog 文件后，都需执行此命令重建二进制目录索引。

  自版本 196 引入。

- **`--setup-keys`**

  不显示 journal 内容，而是为前向安全密封（FSS）生成一对新密钥：一个密封密钥（sealing key）与一个验证密钥（verification key）。密封密钥存放在 journal 数据目录中，应留在本机；验证密钥应存放在外部。前向安全密封的说明及介绍其密码学理论的论文链接，见 `journald.conf`(5) 的 `Seal=` 选项。

  自版本 189 引入。

- **`-h`, `--help`**

  打印简短帮助文本并退出。

- **`--version`**

  打印简短版本字符串并退出。

## 退出状态（EXIT STATUS）

成功时返回 0；否则返回非零失败码。

## 环境变量（ENVIRONMENT）

- **`$SYSTEMD_LOG_LEVEL`**

  所发出消息的最高日志级别（级别更高、即更不重要的消息会被抑制）。取逗号分隔的值列表。每个值可以是（按重要性递减）`emerg`、`alert`、`crit`、`err`、`warning`、`notice`、`info`、`debug` 之一，或 0…7 范围内的整数，详见 `syslog`(3)。每个值还可选地带 `console`、`syslog`、`kmsg` 或 `journal` 之一加冒号的前缀，为该特定日志目标单独设置最高级别（例如 `SYSTEMD_LOG_LEVEL=debug,console:info` 表示按 debug 级别记录，但输出到控制台的目标按 info 级别）。注意全局最高级别优先于任何按目标设置的级别。

- **`$SYSTEMD_LOG_COLOR`**

  布尔值。为 true 时，写入 tty 的消息按优先级着色。

  此设置仅在消息直接写到终端时有意义，因为 `journalctl`(1) 等显示日志的工具会自行按日志级别着色。

- **`$SYSTEMD_LOG_TIME`**

  布尔值。为 true 时，控制台日志消息带时间戳前缀。

  此设置仅在消息直接写到终端或文件时有意义，因为 `journalctl`(1) 等工具会自行依据条目元数据附加时间戳。

- **`$SYSTEMD_LOG_LOCATION`**

  布尔值。为 true 时，消息带其源码文件名与行号前缀。

  注意日志位置通常本来就作为元数据附在 journal 条目上；但调试程序时把它直接写进消息文本仍可能更方便。

- **`$SYSTEMD_LOG_TID`**

  布尔值。为 true 时，消息带当前线程 ID（TID）数值前缀。

  注意此信息通常本来就作为元数据附在 journal 条目上；但调试程序时把它直接写进消息文本仍可能更方便。

- **`$SYSTEMD_LOG_TARGET`**

  日志消息的目的地。取值为：`console`（写到所连接的 tty）、`console-prefixed`（写到所连接的 tty，但带编码日志级别与 facility 的前缀，见 `syslog`(3)）、`kmsg`（写入内核环形日志缓冲区）、`journal`（写入 journal）、`journal-or-kmsg`（可用时写 journal，否则写 kmsg）、`auto`（自动判定合适目标，默认）、`null`（禁用日志输出）。

- **`$SYSTEMD_LOG_RATELIMIT_KMSG`**

  是否对 kmsg 限速。取布尔值，默认 "`true`"。禁用时 systemd 不会对写入 kmsg 的消息限速。

- **`$SYSTEMD_PAGER`, `$PAGER`**

  未给 `--no-pager` 时所用的分页器。设置了 `$SYSTEMD_PAGER` 则优先用它；否则用 `$PAGER`。两者都未设置时，依次尝试一组已知分页器实现（含 `less`(1) 与 `more`(1)）直至找到可用的；若都不可用则不启用分页。把这些变量设为空串或 "`cat`" 等价于传 `--no-pager`。

  注意：若 `$SYSTEMD_PAGERSECURE` 未设置，`$SYSTEMD_PAGER` 与 `$PAGER` 只能用于**禁用**分页器（值 "`cat`" 或空串），其余取值会被忽略。

- **`$SYSTEMD_LESS`**

  覆盖传给 `less` 的选项（默认 "`FRSXMK`"）。

  用户可能特别想改两个选项：

  - **`K`**：让分页器在按下 `Ctrl`+`C` 时立即退出。若想让 `less` 自行处理 `Ctrl`+`C`（回到分页器命令提示符），应去掉此选项。

    若 `$SYSTEMD_LESS` 的值不含 "`K`" 且所调用的分页器是 `less`，`Ctrl`+`C` 会被可执行文件忽略，交由分页器处理。

  - **`X`**：让分页器不向终端发送 termcap 初始化/反初始化串。默认设置此项，使分页器退出后命令输出仍留在终端上；但这也会使分页器的部分功能失效，特别是无法用鼠标滚动翻页。

  注意设置普通的 `$LESS` 环境变量对 systemd 工具调用 `less` 无效。

  更多讨论见 `less`(1)。

- **`$SYSTEMD_LESSCHARSET`**

  覆盖传给 `less` 的字符集（若调用方终端判定为 UTF-8 兼容，默认 "`utf-8`"）。

  注意设置普通的 `$LESSCHARSET` 对 systemd 工具调用 `less` 无效。

- **`$SYSTEMD_PAGERSECURE`**

  `less`(1) 等常见分页器命令除「分页」（滚动输出）外，还支持打开/写入其他文件、运行任意 shell 命令。当命令以提升的权限运行时（例如在 `sudo`(8) 或 `pkexec`(1) 之下），分页器就成为一道安全边界：必须只把功能严格受限的程序用作分页器，不允许打开/新建文件或派生子进程这类非预期交互特性。如下所述可以启用分页器的「安全模式」**——前提是分页器支持**（多数分页器并未考虑这一点）。允许不可信用户以提升权限执行命令时，建议显式启用「安全模式」，或用 `--no-pager`、`PAGER=cat` 彻底禁用分页器。

  此选项取布尔参数。设为 true 时启用分页器的「安全模式」：调用分页器时设置 `LESSSECURE=1`，指示分页器禁用打开/新建文件、启动新子进程的命令。目前已知只有 `less`(1) 理解该变量并实现「安全模式」。

  设为 false 时不限制分页器。设置 `SYSTEMD_PAGERSECURE=0` 或不从继承环境中清除它，可能允许用户调用任意命令。

  `$SYSTEMD_PAGERSECURE` 未设置时，systemd 工具会尝试自动判断是否应启用「安全模式」以及分页器是否支持它。若有效 UID 与登录会话属主不一致（见 `geteuid`(2) 与 `sd_pid_get_owner_uid`(3)），或在 `sudo`(8) 等工具下运行（`$SUDO_UID` 已设置 [1]），则启用「安全模式」：设置 `SYSTEMD_PAGERSECURE=1`，且完全不使用未知是否实现「安全模式」的分页器。注意该自动检测只覆盖最常见的提权机制，仅为便利性设计；建议显式设置 `$SYSTEMD_PAGERSECURE` 或禁用分页器。

  注意：若希望 `$SYSTEMD_PAGER` 或 `$PAGER` 生效（而非仅用于禁用分页器），必须同时设置 `$SYSTEMD_PAGERSECURE`。

- **`$SYSTEMD_COLORS`**

  取布尔值或特殊值。默认（未设置）时，`systemd` 及相关工具在可能时于输出中使用颜色。若 `$COLORTERM` 为 "`truecolor`" 或 "`24bit`" 则启用 24 位色，否则 256 色——除非 `$NO_COLOR` 或 `$TERM` 表明颜色被禁用。

  - **`true`**：同未设置，但忽略 `$NO_COLOR`。
  - **`false`**：输出为单色。
  - **`"16"`, `"256"`, `"24bit"`**：分别始终使用 16 色 ANSI、256 色、24 位色。
  - **`"auto-16"`, `"auto-256"`, `"auto-24bit"`**：在 `$TERM` 及所连接控制台允许的前提下使用给定色彩数。

- **`$SYSTEMD_URLIFY`**

  值必须为布尔。控制是否为支持该特性的终端模拟器在输出中生成可点击链接。可用于覆盖 systemd 基于 `$TERM` 等条件作出的判断。

## 示例（EXAMPLES）

不带参数，不过滤显示全部已收集的日志：

```
journalctl
```

指定一个匹配项，显示字段匹配该表达式的全部条目：

```
journalctl _SYSTEMD_UNIT=avahi-daemon.service
journalctl _SYSTEMD_CGROUP=/user.slice/user-42.slice/session-c1.scope
```

匹配两个**不同**字段时，只显示同时满足两个表达式的条目：

```
journalctl _SYSTEMD_UNIT=avahi-daemon.service _PID=28097
```

两个匹配项作用于**同一**字段时，显示满足任一表达式的全部条目：

```
journalctl _SYSTEMD_UNIT=avahi-daemon.service _SYSTEMD_UNIT=dbus.service
```

使用分隔符 "`+`" 可把两个表达式按逻辑 OR 组合。下面显示 Avahi 服务进程中 PID 为 28097 的全部消息，外加 D-Bus 服务（其全部进程）的全部消息：

```
journalctl _SYSTEMD_UNIT=avahi-daemon.service _PID=28097 + _SYSTEMD_UNIT=dbus.service
```

要显示某单元**发出**的以及**关于**该单元的全部字段，应使用 `-u`/`--unit=`。`journalctl -u name` 会展开为类似下面的复合过滤：

```
_SYSTEMD_UNIT=name.service
  + UNIT=name.service _PID=1
  + OBJECT_SYSTEMD_UNIT=name.service _UID=0
  + COREDUMP_UNIT=name.service _UID=0 MESSAGE_ID=fc2e22bc6ee647b6b90729ab34a250b1
```

（这些模式的解释见 `systemd.journal-fields`(7)。）

显示由 D-Bus 可执行文件生成的全部日志：

```
journalctl /usr/bin/dbus-daemon
```

显示**上一次**启动的全部内核日志：

```
journalctl -k -b -1
```

对系统服务 `apache.service` 做实时日志跟踪：

```
journalctl -f -u apache
```

## 另见（SEE ALSO）

`systemd`(1)、`systemd-cat`(1)、`systemd-journald.service`(8)、`systemctl`(1)、`coredumpctl`(1)、`systemd.journal-fields`(7)、`journald.conf`(5)、`systemd.time`(7)、`systemd-journal-remote.service`(8)、`systemd-journal-upload.service`(8)

---

**原注 [1]**：建议其他工具也酌情设置并检查 `$SUDO_UID`，将其视为一个通用接口。
