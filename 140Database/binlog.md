---
tags:
  - database/log
  - database/mysql
  - type/reference
mastery: 5
---

## 1.概述

binlog 是 Mysql sever 层维护的一种二进制日志，与 innodb 引擎中的 redo/undo log 是完全不同的日志；其主要是用来记录对 mysql 数据更新或潜在发生更新的 SQL 语句，并以"事务"为最小记录单位，保存在磁盘中；

主要**作用**：

- 数据恢复：通过 mysqlbinlog 工具恢复数据
- 主从复制：MySQL Replication 在 Master 端开启 binlog，Master 把它的binlog传递给 slaves 并回放来达到 master-slave 数据一致的目的
- 增量备份

## 2.binlog管理

### 显示binlog信息

```shell
show variables like '%log_bin%';
+---------------------------------+-------------------------------------+
| Variable_name                   | Value                               |
+---------------------------------+-------------------------------------+
| log_bin                         | ON                                  |
| log_bin_basename                | /var/lib/mysql/3306/mysql-bin       |
| log_bin_index                   | /var/lib/mysql/3306/mysql-bin.index |
| log_bin_trust_function_creators | OFF                                 |
| log_bin_use_v1_row_events       | OFF                                 |
| sql_log_bin                     | ON                                  |
+---------------------------------+-------------------------------------+

```
**查看 binlog 文件信息**

当开启了 MySQL 的二进制日志（binlog）功能后，MySQL 会将日志信息记录到 binlog 文件中，通常这些文件位于数据目录下，默认的文件名格式是`host-bin.n`，其中`host`是主机名，`n`是一个序号。
- 当启用 binlog 后，MySQL 会开始写入 binlog 文件。它会在 binlog 文件大小达到一定限制（默认为 1GB，可以通过配置参数`max_binlog_size`进行修改）时，或者在数据库重启时，生成新的 binlog 文件。
- 也可以通过手动执行`FLUSH LOGS`命令来切换到新的 binlog 文件，这样也会生成一个新的 binlog 文件。
- binlog 是以事务为单位记录的。一个 binlog 文件中必须包含完整的事务，一个事务不会跨越两个不同的 binlog 文件。
- MySQL 会维护一个名为`binlog.index`的索引文件，用于记录所有已生成的 binlog 文件的列表和顺序。
```shell
mysql> SHOW BINARY LOGS;
+---------------+-----------+-----------+
| Log_name      | File_size | Encrypted |
+---------------+-----------+-----------+
| binlog.000001 |       667 | No        |
| binlog.000002 |       402 | No        |
| binlog.000003 |      1987 | No        |
| binlog.000004 |    109987 | No        |
+---------------+-----------+-----------+
4 rows in set (0.00 sec)

mysql> FLUSH LOGS;
Query OK, 0 rows affected (0.01 sec)

mysql> SHOW BINARY LOGS;
+---------------+-----------+-----------+
| Log_name      | File_size | Encrypted |
+---------------+-----------+-----------+
| binlog.000001 |       667 | No        |
| binlog.000002 |       402 | No        |
| binlog.000003 |      1987 | No        |
| binlog.000004 |    110031 | No        |
| binlog.000005 |       156 | No        |   # 产生了新的binlog
+---------------+-----------+-----------+
5 rows in set (0.00 sec)
➜  data sudo cat binlog.index
./binlog.000001
./binlog.000002
./binlog.000003
./binlog.000004
./binlog.000005
```
**查看当前的状态**
```shell
mysql> SHOW MASTER STATUS;
+---------------+----------+--------------+------------------+-------------------+
| File          | Position | Binlog_Do_DB | Binlog_Ignore_DB | Executed_Gtid_Set |
+---------------+----------+--------------+------------------+-------------------+
| binlog.000005 |      156 |              |                  |                   |
+---------------+----------+--------------+------------------+-------------------+
```
表示当前记录的 master 的位置为` binlog. 000005`中的 `156`
  
**清空 binlog 日志文件**
```shell
mysql> RESET MASTER;
```

## 3.binlog内容

默认情况下binlog日志是二进制格式，无法直接查看。可使用两种方式进行查看：

1. mysqlbinlog 工具
`mysqlbinlog` 工具在`/path/to/MySQL安装路径/bin`目录下

```shell
mysqlbinlog binlog_file_name [ --start-position= --stop-position= ] | [ --start-time= --stop-time= ]
# position可以通过MySQL命令 SHOW MASTER STATUS;查看
```
2. MySQL 命令
```myqsl
SHOW BINLOG EVENTS
            [IN 'binlog_file_name']
            [FROM pos]  
            [LIMIT [offset,] row_count]  
```

## 4.binlog 格式

Mysql binlog 日志有**ROW, STATEMENT, MIXED**三种格式。可通过 my.cnf 配置文件或者`set global binlog_format='ROW/STATEMENT/MIXED' `进行修改。
命令行 show variables like 'binlog_format' 命令查看 binglog 格式；。

**ROW**
仅保存记录被修改细节，不记录 sql 语句上下文相关信息。
- *优点*：**能非常清晰的记录下每行数据的修改细节，不需要记录上下文相关信息，任何情况都可以被复制**。不会发生某些特定情况下的 procedure、function、及 trigger 的调用触发无法被正确复制的问题，任何情况都可以被复制，且能加快从库重放日志的效率，保证从库数据的一致性  
- *缺点*：**可能会产生大量的日志内容**。比如一条 update 语句，如修改多条记录，则 binlog 中每一条修改都会有记录，这样造成 binlog 日志量会很大，特别是当执行 alter table 之类的语句的时候，由于表结构修改，每条记录都发生改变，那么该表每一条记录都会记录到日志中，实际等于重建了表。  
- tip: - row 模式生成的 sql 编码需要解码，不能用常规的办法去生成，需要加上相应的参数 (--base64-output=decode-rows -v)才能显示出 sql 语句; 
  ```shell
  mysqlbinlog --base64-output=decode-rows -v binlog.000001
  ```
- 新版本 binlog 默认为 ROW 模式，且 5.6 新增了一个参数：binlog_row_image；把 binlog_row_image 设置为 minimal 以后，binlog 记录的就只是影响的列，大大减少了日志内容。（在 my.cnf 中）

**STATEMENT**
每一条会修改数据的sql都会记录在binlog中

- *优点* 
  1. 只需要记录执行语句的细节和上下文环境，避免了记录每一行的变化，在一些修改记录较多的情况下相比 ROW 模式能大**大减少 binlog 日志量**，节约 IO，提高性能；
  2. 可以在从服务器上实时还原主服务器上的内容
  3. 同时主从版本可以不一样，从服务器版本可以比主服务器版本高  
- *缺点*  
  1. **为了保证 sql 语句能在 slave 上正确执行，必须记录上下文信息**，以保证所有语句能在 slave 得到和在 master 端执行时候相同的结果；
  2. **主从复制时，存在部分函数，如 NOW () 及存储过程在 slave 上会出现与 master 结果不一致的情况**，而相比 ROW 模式记录每一行的变化细节，绝不会发生这种不一致的情况。
     > 因为不同时候运行 NOW() 函数得到的结果当然不同
     > 存储过程中如果包含了 NOW ()函数来记录时间戳，得到的结果也不同

**MIXED**

以上两种 level 的混合使用经过前面的对比，可以发现 ROW level 和 statement level 各有优势，如能根据 sql 语句取舍可能会有更好地性能和效果；Mixed level 便是以上两种 leve 的结合。


不过，新版本的 MySQL 对 row level 模式也被做了优化，并不是所有的修改都会以 row level 来记录，像遇到表结构变更的时候就会以 statement 模式来记录，如果 sql 语句确实就是 update 或者 delete 等修改数据的语句，那么还是会记录所有行的变更；因此，现在一般使用 row level 即可。

**记录规则**
MySQL 中的记录规则如下：
- INSERT，UPDATE，DELETE 等**会修改表中数据的语句**，则根据`@@binlog_format`变量所指定的格式记录。 
- GRANT，REVOKE，SET PASSWORD 等**不会修改表中的数据的管理语句**，那么无论如何都采用 statement 模式记录。
  > 因为 ROW 模式只能记录每行数据发生的变化

## 5. 恢复

1. 记录起始 Position 
2. 对数据进行修改操作
3. 记录结束 Position 
4. 将这段区间的 binlog 输出为 SQL 文件
5. 运行这个 SQL 文件
```shell
mysqlbinlog   --start-position=  --stop-position=  binlog_file_name  > ~/test. sql
mysql> source ~/test. sql
```
**原理**

从新执行一遍`start-position`到`stop-position`之间的 SQL 语句。

**过程演示**

1. 记录起始 Position 和结束 Position
```shell
mysql> show master status;
+---------------+----------+--------------+------------------+-------------------+
| File          | Position | Binlog_Do_DB | Binlog_Ignore_DB | Executed_Gtid_Set |
+---------------+----------+--------------+------------------+-------------------+
| binlog.000001 |      156 |              |                  |                   |
+---------------+----------+--------------+------------------+-------------------+
1 row in set (0.00 sec)

mysql> use sample;
Reading table information for completion of table and column names
You can turn off this feature to get a quicker startup with -A

Database changed
mysql> insert into account values(null, 'jack', 10);
Query OK, 1 row affected (0.01 sec)

mysql> show master status;
+---------------+----------+--------------+------------------+-------------------+
| File          | Position | Binlog_Do_DB | Binlog_Ignore_DB | Executed_Gtid_Set |
+---------------+----------+--------------+------------------+-------------------+
| binlog.000001 |      461 |              |                  |                   |
+---------------+----------+--------------+------------------+-------------------+
1 row in set (0.00 sec)
```
- **删除掉这一行 `(null, 'jack', 10)`**
- 将指定区间的 binlog 输出为 SQL 文件
```shell
mysqlbinlog --start-position=156 --stop-position=461 binlog. 000001 > ~/testbinlog.sql
```
- 执行这个 SQL 文件
```shell
mysql> source ~/testbinlog.txt
Query OK, 0 rows affected (0.00 sec)

Query OK, 0 rows affected (0.00 sec)

......

```
- **这一行`(null, 'jack', 10)`又被重新添加回去了
## 5. 主从同步

- [ ] MySQL 主从同步，待学习
[[主从同步]]

[腾讯工程师带你深入解析 MySQL binlog - 知乎](https://zhuanlan.zhihu.com/p/33504555)


## 8.参考

- _[https://dev.mysql.com/doc/internals/en/binary-log-versions.html](https://link.zhihu.com/?target=https%3A//dev.mysql.com/doc/internals/en/binary-log-versions.html)_
- _[http://www.php.cn/mysql-tutorials-361643.html](https://link.zhihu.com/?target=http%3A//www.php.cn/mysql-tutorials-361643.html)_
- _[https://www.jianshu.com/p/c16686b35807](https://link.zhihu.com/?target=https%3A//www.jianshu.com/p/c16686b35807)_
- _[https://www.cnblogs.com/jackluo/p/3336585.html](https://link.zhihu.com/?target=https%3A//www.cnblogs.com/jackluo/p/3336585.html)_
- _[http://www.cnblogs.com/hustcat/archive/2009/12/19/1627525.html](https://link.zhihu.com/?target=http%3A//www.cnblogs.com/hustcat/archive/2009/12/19/1627525.html)_

## See also

- [[redo log]] - InnoDB 重做日志
- [[undo log]] - InnoDB 回滚日志
- [[MySQL InnoDB 恢复(recovery)详细流程]] - 恢复过程中 binlog、redo log、undo log 的协作