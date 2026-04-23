---
tags:
  - act/project
  - cpp/basics
  - type/howto
mastery: 5
---

利用 RAII 技术封装`pthread_mutex`和`pthread_cond`，在构造函数中申请锁或条件变量，在析构函数中释放锁或条件变量。

### MutexLock

- MutexLock 构造：调用 `pthread_mutex_init` 初始化锁
- 手动加锁解锁：提供 `lock()` / `unlock()` 方法
- MutexLock 析构：调用 `pthread_mutex_destroy` 释放锁

### MutexLockGuard

- 构造：调用 `MutexLock::lock()` 加锁
- 析构：调用 `MutexLock::unlock()` 解锁
- 锁在它的作用域内生效，自动加锁解锁

> `MutexLockGuard` 的典型用法是在函数内部创建局部对象，利用 RAII 确保即使发生异常也能正确释放锁。

### Condition

封装 `pthread_cond_t`，配合 `MutexLock` 使用：
- `wait()`：阻塞等待条件变量
- `notify()` / `notifyAll()`：唤醒等待线程

## 相关

- [[Log]]
- [[数据库连接]]
- [[项目架构]]
        