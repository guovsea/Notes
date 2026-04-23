---
tags:
  - type/pitfall
mastery: 5
---

# VMWare 安装虚拟机报错：断裂管道

> 错误信息：”打不开 /dev/vmmon：断裂管道”

## 解决方法

1. 给 VMware 赋予完全磁盘访问权限

![[VMWare安装虚拟机报错：断裂管道.png]]

2. 会弹出一个是否允许访问，点击允许

![[VMWare安装虚拟机报错：断裂管道-1.png]]

3. 重启 VMware