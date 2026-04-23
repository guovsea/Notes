---
tags:
  - cpp/concurrency
  - type/concept
---
## 定义

A _reentrant_ function can be called simultaneously from multiple threads, but only if each invocation uses its own data.

可重入函数可以在多个线程中同时调用，但每次调用必须使用各自独立的数据，不依赖任何共享状态。

## 可重入 vs 线程安全

- **可重入**：不依赖共享数据，不需要同步机制
- [[线程安全 Thread-Safety]]：可以使用共享数据，但通过同步机制保证正确性

可重入的函数一定是线程安全的；线程安全的函数不一定是可重入的（可能依赖锁等同步机制）。
