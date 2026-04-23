---
tags:
  - database/sql
  - database/mysql
  - type/howto
mastery: 5
---

## SQL执行频率

```sql
-- session 是查看当前会话 ;  
-- global 是查询全局数据 ;  
SHOW GLOBAL STATUS LIKE 'Com_______';
```

## 慢查询日志

慢查询日志记录了所有执行时间超过指定参数(`long_query_time`，单位:秒，默认10秒)的所有SQL语句的日志。  
- MySQL 的慢查询日志默认没有开启，我们可以查看一下系统变量 `slow_query_log`。
- 如果要开启慢查询日志，需要在 MySQL 的配置文件(`/etc/my.cnf`)中配置如下信息:
```ini
# 开启MySQL慢日志查询开关  
slow_query_log=1  
# 设置慢日志的时间为2秒，SQL语句执行时间超过2秒，就会视为慢查询，记录慢查询日志
long_query_time=2
```
- 配置完毕后重启 MySQL
```shell
systemctl restart mysqld
```
## profile 详情

`show profiles` 能够在做 SQL 优化时帮助我们了解时间都耗费到哪里去了。
```sql
-- 查询是否支持profile
SELECT @@have_profiling ;
-- 查询是否开启profile
SELECT @@profiling

-- 开启profile
SET profiling = 1;
```
使用profile

```sql
-- 查看每一条SQL的耗时基本情况 
show profiles;

-- 查看指定query_id的SQL语句各个阶段的耗时情况 
show profile for query query_id;

-- 查看指定query_id的SQL语句CPU的使用情况 
show profile cpu for query query_id;
```

## EXPLAIN / DESC

EXPLAIN 或者 DESC 命令获取 MySQL 如何执行 SELECT 语句的信息，包括在 SELECT 语句执行过程中表如何连接和连接的顺序。


| 字段名       | 含义                                                                                                                                                                                            |
| ------------ | ----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------- |
| id           | select 查询的序列号，表示查询中执行 select 子句或者是操作表的顺序 (id 相同，从上到下执行。id 不同，值越大，越先执行)。                                                                      |
| select_type  | 表示 SELECT 的类型，常见的取值有 SIMPLE(简单表，即不使用表连接或者子查询)、PRIMARY(主查询，即外层的查询)、UNION(UNION 中的第二个或者后面的查询语句)、 SUBQUERY(SELECT/WHERE 之后包含了子查询)等 |
| type         | 表示连接类型，性能由好到差的连接类型为 NULL、system、const、 eq_ref、ref、range、 index、all                                                                                                    |
| possible_key | 显示可能应用在这张表上的索引，一个或多个。                                                                                                                                                      |
| key          | 实际使用的索引，如果为 NULL，则没有使用索引。                                                                                                                                                   |
| key_len      | 表示索引中使用的字节数，该值为索引字段最大可能长度，并非实际使用长度，在不损失精确性的前提下，长度越短越好 。                                                                                   |
| rows         | MySQL 认为必须要执行查询的行数，在 innodb 引擎的表中，是一个估计值，可能并不总是准确的。                                                                                                        |
| filtered     | 表示返回结果的行数占需读取行数的百分比， filtered 的值越大越好。(读的都是需要的)                                                                                                                                                                                               |

## explain

查看 SQL 执行计划

explain SQL 语句

## See also

- [[SQL 优化]] - SQL 优化实践
- [[索引使用]] - 索引语法与使用原则