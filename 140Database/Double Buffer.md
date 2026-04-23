---
tags:
  - database/innodb
  - type/concept
mastery: 5
---

#### 部分写（partial page write）问题

存储引擎缓冲池内的数据页大小默认为 16KB，而文件系统一页大小为 4KB。所以在进行刷盘操作时，如果此时宕机，有可能会导致只写入了数据页的一部分（即 16KB 中的部分 4KB 页），数据页的完整性遭到破坏。

>虽然 MySQL 可以根据 redolog 进行恢复，但无法恢复页面中的部分损坏问题。因为 redo log 的粒度是数据页级别的。[[redo log 的原理]]

**原理**

![[InnoDB架构_double write buffer.png]]
1. 在对缓冲池的脏页进行刷新时，并不直接写磁盘，而是会通过 memcpy 函数将脏页先复制到内存中的 Double write buffer    
2. 再分两次，每次 1MB 顺序地写入共享表空间的物理磁盘上，然后马上调用 fsync 函数，同步磁盘，避免缓冲写带来的问题。
![[InnoDB架构_利用DWBuffer恢复.png]]

如图，如果操作系统在将页写入磁盘的过程中发生了崩溃，在恢复过程中，InnoDB 存储引擎从共享表空间中的 Double write 中找到该页的一个副本，将其复制到表空间文件，再应用重做日志。

**开销**

因为写入 double buffer files 是顺序写，因此开销并不大

## See also

- [[redo log 的原理]] - redo log 粒度为数据页级别
- [[InnoDB架构]] - InnoDB 磁盘架构中的 Doublewrite Buffer Files
- [[redo log]] - WAL 机制
