## 大致原理

redo log 不会记录完整的一页数据，它只会记录谁如何操作了哪页的哪个位置。以减小日志长度。

## redo log 存储结构

![[redo log 的原理_red哦.png]]
[Innodb引擎 · 基础模块篇(三) · 详解redo log存储结构 - 掘金](https://juejin.cn/post/6895265596985114638)

因此，redo log 记录的最小粒度是数据页级别的，一条 redo record 会记录某个数据页的修改。