redo log 日志，记录的是事务提交时数据页的物理修改，是用来实现事务的持久性的。

redo log 日志文件由两部分组成
- 重做日志缓冲 (`redo log buffer`) **缓冲区**
- 日志文件 (`ib_logfile`)  **磁盘**

前者是在内存中，后者在磁盘中。当事务提交之后会把所有修改信息都存到该日志文件中，用于在刷新脏页到磁盘,发生错误时, 进行数据恢复使用。

## 问题情景

在 InnoDB 引擎中的内存结构中，主要的内存区域就是缓冲池，在缓冲池中缓存了很多的数据页。当我们在一个事务中，执行多个增删改的操作时，InnoDB 引擎会先操作缓冲池中的数据，如果缓冲区没有对应的数据，会通过后台线程将磁盘中的数据加载出来，存放在缓冲区中，然后将缓冲池中的数据修改。**修改后还未刷新到磁盘上的的数据页称为脏页。**

脏页则会在一定的时机，通过后台线程刷新到磁盘中，从而保证缓冲区与磁盘的数据一致。而缓冲区的**脏页数据并不是实时刷新的**，而是一段时间之后 将缓冲区的数据刷新到磁盘中，假如刷新到磁盘的过程出错了，而提示给用户事务提交成功，而数据却没有持久化下来，这就出现问题了，没有保证事务的持久性。

## 大概流程

- 有了 redolog 之后，当对缓冲区的数据进行增删改之后，会首先将操作的数据页的变化，记录在 redo log buffer 中。[[#redo log 刷入磁盘时机]]
- 在事务提交时，**会将 redo log buffer 中的数据刷新到 redo log 磁盘文件中。**
- 过一段时间之后，如果刷新缓冲区的脏页到磁盘时，发生错误，此时就可以借助于 redo log 进行数据恢复，这样就保证了事务的持久性。[[#利用 redo log 进行恢复的过程]]
- 而如果脏页成功刷新到磁盘或或者涉及到的数据已经落盘，此时 redolog 就没有作用了，就可以删除了，所以存在的两个 redolog 文件是循环写的。

## WAL
那每一次提交事务，**为什么要先刷新 redo log 到磁盘中**呢，而不是直接将 buffer pool 中的脏页刷新到磁盘呢 ?

因为在业务操作中，我们操作数据一般都是随机读写磁盘的，而不是顺序读写磁盘。而 redo log 在往磁盘文件中写入数据，由于是**日志文件，所以都是顺序写的。顺序写的效率，要远大于随机写。** 这种先写日志的方式，称之为 **WAL (Write-Ahead Logging)**。

## redo log 刷入磁盘时机

在计算机操作系统中，用户空间(user space)下的缓冲区数据一般情况下是无法直接写入磁盘的，中间必须经过操作系统内核空间(kernel space)缓冲区(OS Buffer)。因此，redo log buffer 写入 redo log file 实际上是先写入 OS Buffer，然后再通过系统调用 fsync()将其刷到 redo log file 中，过程如下：

redo log buffer **->** OS buffer **->** redo log file (ib_logfile)

[[缓存 IO]]


mysql 支持三种将`redo l]og buffer`写入`redo log file`的时机，也就是将**缓冲区中的内容刷入磁盘的时机**，可以通过`innodb_flush_log_at_trx_commit`进行控制。

| 参数值                             | 含义                                                                                     |
| ---------------------------------- | ---------------------------------------------------------------------------------------- |
| **取值为 1** 默认设置，最常用设置。 | 每次事务的结束都会触发 Log Thread 将 log buffer  中的数据写入文件中，并使用 fsync()**刷入磁盘** |
| **取值为 0**                       | 每次事务结束不会触发 Log Thread，Log Thread **每隔1 秒钟**会将 log buffer 中的数据写入到文件并刷入磁盘                |
| **取值为 2**                                   |   每次事务的结束都会触发 Log Thread 将 log buffer  中的数据写入文件，每隔 1 秒刷入磁盘                                                                                     |
[MySQL 官方文档解释](https://mysql.net.cn/doc/refman/8.0/en/innodb-parameters.html#sysvar_innodb_flush_log_at_trx_commit)
*注*： 
1. **写入文件操作 -> 仅仅只是写入系统缓冲区**  *这个观念很重要*
2. fsync () -> 刷入磁盘 (文件)
3. 执行事务的过程中，每次进行某种操作都会将该操作记录在`redo log bufer`中，无论 `innodb_flush_log_at_trx_commit` 的取值为多少。

**效果分析**

- 参数为 1：不论是 MySQL Crash 还是 OS Crash，最多丢失一条事务
- 参数为 0：MySQL Crash 和 OS Crash 都会丢失 1 秒钟的事务
- 参数为 2：MySQL Crash 最多丢失一条事务，OS Crash 最多会丢失 1 秒的事务。

## [[redo log 的原理]]

## 利用 redo log 进行恢复的过程

在 MySQL 发生崩溃时，下次启动便可以运用 redo log 、 binlog 和 undo log 进行恢复。（过程较为复杂）

[MySQL InnoDB 恢复(recovery)详细流程 - VicLW - 博客园](https://www.cnblogs.com/VicLiu/p/12606597.html)
[[MySQL InnoDB 恢复(recovery)详细流程]]
[MySQL中 LSN 和 Checkpoint 关系 - 知乎](https://zhuanlan.zhihu.com/p/469865071)