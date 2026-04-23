---
tags:
  - database/mvcc
  - type/concept
mastery: 5
---

MVCC（Multi-Version Concurrency Control）多版本并发控制

## 作用

- MVCC 机制**主要用于快照读操作**，它允许多个事务同时读取同一数据行，而不会阻塞彼此。
- MVCC通过为每个事务创建数据的快照版本来实现并发读取。这意味着事务在读取数据时不会看到其他事务未提交更改。
- MVCC 机制提供无阻塞的并发读取功能，**快照读操作不会引发锁竞争**，提高了并发性和性能。

## 实现要素

MVCC 的实现依赖三个核心组件：

1. **隐藏字段**：每行记录包含 `DB_TRX_ID`（最后修改的事务 ID）和 `DB_ROLL_PTR`（回滚指针）。详见 [[存储引擎#隐藏字段|隐藏字段]]。

2. **undo log 版本链**：通过 `DB_ROLL_PTR` 将同一行数据的历史版本串联成链表。详见 [[undo log#undo log 版本链原理|undo log 版本链]]。

3. **ReadView**：快照读执行时生成，记录当前活跃事务的 ID 集合，用于判断版本链上哪个版本对当前事务可见。详见 [[undo log#ReadView 和版本链的使用|ReadView 规则]]。

## 原理

利用 [[undo log#提供 MVCC 机制|undo log 提供 MVCC 机制]]

## See also

- [[锁]] - 当前读时的锁机制
- [[事务]] - 事务四大特性
- [[并发问题和隔离级别]] - MVCC 在不同隔离级别下的行为差异
- [[MVCC 和锁]] - 快照读与当前读的对比
