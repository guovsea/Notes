利用 RAII 技术封装`pthread_mutex`和`pthread_cond`，在构造函数中申请锁或条件变量，在析构函数中释放锁或条件变量。


MutexLock
- MutexLock 构造：申请锁
- 手动加锁解锁
- MutexLock 析构：释放锁

MutexLockGuard
- 构造：加锁
- 析构：解锁
- 锁在它的作用域内生效，自动加锁解锁
        