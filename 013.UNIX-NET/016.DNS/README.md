# DNS域名解析
> 学习: [DNS域名解析过程-1](../../010.LESSONS/425498344-1-208_part1.mp4) & [DNS域名解析过程-2](../../010.LESSONS/425498344-1-208_part2.mp4)

## nsloopup
nslookup（意为name server lookup）是一个网络管理命令行界面工具，用户可以利用nslookup查询域名的ip地址以及ip地址所对应的域名，例如在命令行界面输入nslookup以及网址后，nslookup会发送命令给电脑所连接的域名服务器，随后便能得出网址所映射的IP地址。

### 命令用法
> 先使用 man 手册吧

### 注意事项
+ set type=ns , ns 是记录类型，即 `Resource records  资源记录 `  的类型