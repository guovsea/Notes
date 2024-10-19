![[InnoDB 架构图.png]]

## 内存架构

###  Buffer Pool

InnoDB 存储引擎基于磁盘文件存储，访问物理硬盘和在内存中进行访问，速度相差很大，为了尽可能弥补这两者之间的 I/O 效率的差值，就需要把经常使用的数据加载到缓冲池中，避免每次访问都进行磁盘 I/O。(**预加载**技术)

在 InnoDB 的缓冲池中不仅缓存了索引页和数据页，还包含了 undo 页、插入缓存、自适应哈希索引以及 InnoDB 的锁信息等等。

缓冲池 Buffer Pool，是主内存中的一个区域，里面可以缓存磁盘上经常操作的真实数据，在执行增删改查操作时，先操作缓冲池中的数据(若缓冲池没有数据，则从磁盘加载并缓存)，然后再以一定频率刷新到磁盘，从而减少磁盘 IO，加快处理速度。

缓冲池以 Page 页为单位，底层采用链表数据结构管理 Page。根据状态，将 Page 分为三种类型: 

- **free page**  
  空闲 page，未被使用。  
- **clean page**  
  被使用 page，数据没有被修改过。  
- **dirty page**  
  脏页，被使用 page，数据被修改过，也中数据与磁盘的数据产生了不一致。

在专用服务器上，通常将多达80%的物理内存分配给缓冲池 。

参数设置: show variables like 'innodb_buffer_pool_size';

### Change Buffer

Change Buffer，更改缓冲区(**针对于非唯一二级索引页**)，在执行 DML 语句时，如果这些数据 Page 没有在 Buffer Pool 中，不会直接操作磁盘，而会将数据变更存在更改缓冲区 Change Buffer 中，在未来数据被读取时，再将数据合并恢复到 Buffer Pool 中，再将合并后的数据刷新到磁盘中。

**Change Buffer 的意义**

如果利用二级索引对表进行修改，那么要利用二级索引将修要修改的数据页加载到`Buffer Pool` 中(这个查找过程是随机 `I/O`)，然后进行数据记录的插入或者更新、删除。这些数据记录会随机分布在许多不同的页面，需要加载很多页面并且可能会导致大量页面的分裂和合并。因此，InnoDB 选择在 `Change Buffer` 进行插入或者更新、删除操作。然后将磁盘中的未修改的页面和`Change buffer`中已经修改的页面进行合并，**减少磁盘随机 I/O**。

为什么利用二级索引加载数据页是随机 I/O ？

- 因为数据在磁盘上，是根据聚集索引进行组织的。在二级索引中相邻的数据可能在聚集索引中的位置可能会非常随机。
  如下图，二级索引中相邻的叶子节点的聚集索引值非常随机。
  ![[InnoDB架构_二级索引.png]]

[Innodb引擎 · 基础模块篇(二) · 浅谈Change Buffer - 掘金](https://juejin.cn/post/6892659139794157575)

### Adaptive Hash Index

自适应 hash 索引，用于优化对 Buffer Pool 数据的查询。MySQL 的 innoDB 引擎中虽然没有直接支持 hash 索引，但是给我们提供了一个功能就是这个自适应 hash 索引。因为前面我们讲到过，hash 索引在进行等值匹配时，一般性能是要高于 B+ 树的，因为 hash 索引一般只需要一次 IO 即可，而 B+ 树，可能需要几次匹配，所以 hash 索引的效率要高，但是 hash 索引又不适合做范围查询、模糊匹配等。

InnoDB 存储引擎会监控对表上各索引页的查询，如果观察到在特定的条件下 hash 索引可以提升速度，则建立 hash 索引，称之为自适应 hash 索引。

**自适应哈希索引，无需人工干预，是系统根据情况自动完成。**

参数: `adaptive_hash_index`

### Log Buffer

Log Buffer:日志缓冲区，用来保存要写入到磁盘中的 log 日志数据(redo log 、undo log)，默认大小为 16MB，日志缓冲区的日志会定期刷新到磁盘中。如果需要更新、插入或删除许多行的事务，增加日志缓冲区的大小可以节省磁盘 I/O。

参数:   
- innodb_log_buffer_size
  缓冲区大小 
- innodb_flush_log_at_trx_commit
  日志刷新到磁盘时机，取值主要包含以下三个：[[redo log#redo log 刷入磁盘时机|redo log 刷入磁盘时机]]
  - **1** 志在每次事务提交时写入并刷新到磁盘，默认值。 
  - **0** 每秒将日志写入并刷新到磁盘一次。
  - **2** 日志在每次事务提交后写入，并每秒刷新到磁盘一次。

## 磁盘架构

![[InnoDB 架构图_磁盘结构.png]]

### System Tablespace

系统表空间是更改缓冲区的存储区域。如果表是在系统表空间而不是每个表文件或通用表空间中创建的，它也可能包含表和索引数据。(在 MySQL5.x 版本中还包含 InnoDB 数据字典、undolog 等)

参数:`innodb_data_file_path`

### File-Per-Table Tablespaces

如果开启了 `innodb_file_per_table` 开关，则会将每张表的数据和索引存储在的单个数据文件中，该参数默认开启。

也就是说，每创建一个表，都会产生一个表空间文件
```shell
guo@Mac data % sudo ls -l sample # sample数据库名，存在与MySQL安装路径的data/下
total 3776
-rw-r-----  1 _mysql  _mysql  114688 Sep 27 15:11 account.ibd
-rw-r-----  1 _mysql  _mysql  131072 Sep 23 14:58 countries.ibd
-rw-r-----  1 _mysql  _mysql  131072 Sep 23 14:58 departments.ibd
-rw-r-----  1 _mysql  _mysql  131072 Sep 23 14:58 dependents.ibd
-rw-r-----  1 _mysql  _mysql  114688 Sep 24 23:19 dept.ibd
-rw-r-----  1 _mysql  _mysql  131072 Sep 24 23:19 emp.ibd
-rw-r-----  1 _mysql  _mysql  163840 Sep 23 14:58 employees.ibd
-rw-r-----  1 _mysql  _mysql  180224 Sep 24 17:22 jobs.ibd
-rw-r-----  1 _mysql  _mysql  131072 Sep 23 14:58 locations.ibd
-rw-r-----  1 _mysql  _mysql  114688 Sep 23 14:58 regions.ibd
-rw-r-----  1 _mysql  _mysql  131072 Sep 30 18:39 stu.ibd
-rw-r-----  1 _mysql  _mysql  131072 Sep 24 17:24 test.ibd
-rw-r-----  1 _mysql  _mysql  114688 Sep 24 17:27 test1.ibd
-rw-r-----  1 _mysql  _mysql  114688 Sep 24 17:50 test3.ibd
```
### General Tablespaces
 
通用表空间，需要通过 CREATE TABLESPACE 语法创建通用表空间，在创建表时，可以指定该表空间。 

- 创建表空间
```sql
CREATE TABLESPACE ts_name ADD DATAFILE 'file_name' ENGINE = engine_name;
```
- 创建表时指定表空间
```sql
CREATE TABLE (
...
) 表名 TABLESPACE ts_name;
```
### Undo Tablespaces

MySQL 实例在初始化时会自动创建两个默认的 Undo Tablespaces(初始大小16M)，用于存储 undo log 日志。

### Temporary Tablespaces  
InnoDB 使用会话临时表空间和全局临时表空间。存储用户创建的临时表等数据。

###  Doublewrite Buffer Files

双写缓冲区，InnoDB 引擎将数据页从 Buffer Pool 刷新到磁盘前，先将数据页写入双写缓冲区文件中，**便于 MySQL 宕机后恢复数据**。

[[Double Buffer]]
### Redo Log

重做日志，是用来实现事务的持久性。该日志文件由两部分组成:重做日志缓冲(redo log buffer)以及重做日志文件(redo log),前者是在内存中，后者在磁盘中。当事务提交之后会把所有修改信息都会存到该日志中, 用于在刷新脏页到磁盘时,发生错误时, 进行数据恢复使用。 

## 后台线程

在InnoDB的后台线程中，分为4类，分别是:

- Master Thread
- IO Thread
- Purge Thread
- Page Cleaner Thread

### Master Thread 

核心后台线程，负责调度其他线程，还负责将缓冲池中的数据异步刷新到磁盘中, 保持数据的一致性，还包括脏页的刷新、合并插入缓存、undo页的回收 。

### IO Thread  
在InnoDB存储引擎中大量使用了AIO来处理IO请求, 这样可以极大地提高数据库的性能，而IOThread主要负责这些IO请求的回调。

| 线程类型             | 默认个数 | 职责                         |
| -------------------- | -------- | ---------------------------- |
| Read thread          | 4        | 负责读操作                   |
| Write thread         | 4        | 负责写操作                   |
| Log thread           | 1        | 负责将日志缓冲区刷新到磁盘   |
| Insert buffer thread | 1        | 负责将 Change buffer 缓冲区内容刷新到磁盘 | 

通过以下的指令，查到InnoDB的状态信息，其中就包含IO Thread信息。
```sql
show engine innodb status \G;
```
### Purge Thread  
主要用于回收事务已经提交了的undo log，在事务提交之后，undo log可能不用了，就用它来回

收。

### Page Cleaner Thread  
协助 Master Thread 刷新脏页到磁盘的线程，它可以减轻 Master Thread 的工作压力，减少阻塞。