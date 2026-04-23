---
tags:
  - cpp/concurrency
  - type/concept
---
## 定义

A _thread-safe_ function can be called simultaneously from multiple threads, even when the invocations use shared data, because all references to the shared data are serialized.

线程安全函数可以被多个线程同时调用，即使使用了共享数据，也能通过同步机制（如互斥锁、原子操作）保证共享数据的访问是串行化的。

## 与可重入的关系

- [[可重入 Reentrancy]]：不依赖共享数据，无需同步
- 线程安全：可以使用共享数据，但通过同步保证正确性

可重入函数一定是线程安全的；线程安全的函数不一定是可重入的。
