---
tags:
  - database/innodb
  - database/log
  - type/concept
mastery: 5
---

回滚日志，用于记录数据被修改前的信息，作用包含两个：提供回滚（RALLBACK）(保证事务的原子性) 和 [[MVCC]] (多版本并发控制) 。

## 提供 RALLBACK 操作

undo log 和 redo log 记录物理日志不一样，**undo log 是逻辑日志，当某个语句改变了数据，它会记录一条相反的语句**。可以认为当 delete 一条记录时，undo log 中会记录一条对应的 insert 记录，反之亦然，当 update 一条记录时，它记录一条对应相反的 update 记录。

当执行 rollback 时，就可以从 undo log 中的逻辑记录读取到相应的内容并进行回滚.

>Undo log 销毁: undo log 在事务执行时产生，事务提交时，并不会立即删除 undo log，因为这些日志可能还用于 MVCC 。
## 提供 MVCC 机制

MVCC，即多版本控制。在 MySQL 数据库 InnoDB 存储引擎中，用 undo Log 来实现多版本并发控制 (MVCC)。当读取的某一行被其他事务锁定时，它可以从 undo log 中分析出该行记录以前的数据版本是怎样的，从而让用户能够读取到当前事务操作之前的数据——快照读。

[[MVCC|MVCC、快照读、当前读]]
## undo log 版本链原理

### 版本链的形成

![[undo log 版本链-1.png]]

> *注*：实际上 undo log 中存储的应该是相反的逻辑 SQL 语句，但是在迭代版本链的过程中会依次执行经过的 SQL 语句，因此每到一个版本节点时，就等价于该行数据回到了图示中数据行。

- `DB_TRX_ID`：最后对该行进行插入或修改的事务 ID
- `DB_ROLL_PTR`：回滚指针，指向该数据的上一个版本，用于维护历史版本链

[[存储引擎#隐藏字段|隐藏字段]]

事务 A 每次对某一行进行修改时，就会将该行的`DB_TRX_ID`修改事务 A 的事务 ID，事务 ID 是根据时间自增的。同时，undo log 中会产生一条上一个版本的记录，这个记录中包含修改它的事务 ID 和指向该记录上一个版本的 `DB_ROLL_PTR`。第一个版本的`DB_ROLL_PTR`为 nullptr。

**最终，不同事务或相同事务对同一条记录进行修改，会导致该记录的 undo log 生成一条记录版本链表，链表的头部是最新的旧记录，链表尾部是最早的旧记录**。

### ReadView 和版本链的使用

ReadView(读视图)是快照读 SQL 执行时 MVCC 提取数据的依据，ReadView 记录并维护着系统当前活跃的(未提交的)事务 id。

ReadView 中包含了四个核心字段: 

- **m_ids**  
  当前活跃的事务 ID 集合
- **min_trx_id**  
  最小活跃事务  
- **max_trx_id** 
  下一个事务 ID，当前最大事务的 ID + 1 (因为事务 ID 是自增的
- **creator_trx_id**  
  ReadView 创建者的事务 ID

`trx_id` 表示 undo log 版本链上某个节点对应的事务 ID

| `DB_TRX_ID` 条件                                 | 是否可以访问                         | 说明                                                 |
| ------------------------------------ | ------------------------------------ | ---------------------------------------------------- |
| *1* `trx_id == creator_trx_id`           | 可以                                 | 说明数据是创建 ReadView 的事务更改的                 |
| *2* `trx_id < min_trx_id`                | 可以                                 | 比最小的活跃事务还小，说明 `trx_id` 事务不是活动事务，已经提交了 |
| *3* `trx_id > max_trx_id`    | 不可以                               | `trx_id` 事务是在 ReadView 生成后才开启                   |
| *4* `min_trx_id <= trx_id <= max_trx_id` | 如果`trx_id`不在`m_ids`中，可以访问 | 说明这个 `trx_id` 任务已经提交                             |

> *3 注*： `trx_id` 节点是在 `ReadView` 创建后的开启的事务提交的，因此它绝不可能被访问
> *4 注*： `trx_id` 是在 `ReadView` 创建前提交的。
> 比如事务\[2, 3, 4, 5]都在活跃, 然后 3 提交了，**之后**创建 ReadView。此时 `m_ids=[1,2,4,5]`, `3 NOT IN m_ids`，其他事务可以访问 3（情况 *4* ）。当然，也可以访问 1 （情况 *2* ）
> 创建 ReadView 到成功读到数据，中间存在一段时间。但是这段时间 ReadView 并不会更新，这算时间中就算某个事务提交了，也不会在改变 ReadView。因为只根据创建 ReadView 那一个时间点来考虑能读到哪些数据。

*最后 ReadView 的作用就是：* 保证创建者读到的要么是已经提交的，要么是它自己修改的。

**不同的隔离级别，生成 ReadView 的时机不同:**  

- **READ COMMITTED**：在事务中**每次**执行快照读时生成 ReadView。
 事务每次执行快照读都可以读到在此之前提交的数据。
- **REPEATABLE READ**：仅在事务中**第一次**执行快照读时生成 ReadView，后续复用该 ReadView。
  事务每次执行快照读都只能读到第一次快照读之前提交的数据。
- **READ UNCOMMITED**：直接读取当前最新版本的数据即是


拓展阅读

[新特性解读 | 趋近完美的 Undo 空间 - 知乎](https://zhuanlan.zhihu.com/p/133722126)

[[8.0 的 Undo 空间]]

## See also

- [[MVCC]] - undo log 实现 MVCC 的原理
- [[MVCC 和锁]] - 快照读与当前读
- [[redo log]] - 重做日志，用于事务持久性
- [[事务]] - 事务原子性由 undo log 保证
- [[8.0 的 Undo 空间]] - MySQL 8.0 Undo 表空间管理