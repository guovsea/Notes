---
tags:
  - database/mvcc
  - database/innodb
  - type/concept
mastery: 5
---

[[MVCC]] 作用于**快照读**，使得快照读无需上锁

[[锁]]，在进行**当前读**的时候，必读得借助锁机制。


## 快照读和当前读

- **快照读：**
快照读，顾名思义，就是读取快照数据，也就是说当某个数据正在被修改的时候，也可以进行读取该数据，保证读写不冲突。

单纯的 SELECT 语句，但不包括 `UPDATE `和 `SHARE MODE`模式：
```sql
SELECT ...  FOR UPDATE;
SELECT ... LOCK IN SHARE MODE
```

- **当前读：**
当前读，读取的是最新版本，并且需要先获取对应记录的锁，如以下这些 SQL 类型：

```sql
SELECT ...  FOR UPDATE ;
SELECT ... LOCK IN SHARE MODE / FOR SHARE;
UPDATE;DELETE;INSERT;
```

## See also

- [[MVCC]] - 多版本并发控制详解
- [[锁]] - InnoDB 锁机制详解
- [[并发问题和隔离级别]] - 事务并发问题与隔离级别
