---
tags:
  - os/thread
  - type/howto
mastery: 5
---

[[线程同步模型]]
## 死锁

发生死锁的四个条件

- **互斥**
  在一段时间内某资源只能被一个进程所拥有，不能被其他进程再次拥有
- **不可剥夺**
  进程所获得的资源在未使用完毕之前，不能被其他进程强行夺走
- **请求与保持**
  进程已经保持了至少一个资源，但又提出了新的资源请求，而该资源已被其他进程占有，此时请求进程被阻塞，但对自己已获得的资源保持不放。
- **循环等待**

以上这四个条件是死锁的必要条件，只要系统发生死锁，这些条件必然成立，而只要上述条件之一不满足，就不会发生死锁。因此，**预防死锁就是避免出现上面四个条件**

[[死锁]]

## 互斥锁 mutex

**在访问共享资源前加锁，访问结束后立即解锁。锁的“粒度”应越小越好。**

```c
#include <pthread.h>
pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t mutex;
int pthread_mutex_init(pthread_mutex_t *restrict mutex, 
                       const pthread_mutexattr_t *restrict attr);
int pthread_mutex_destroy(pthread_mutex_t *mutex);
int pthread_mutex_lock(pthread_mutex_t *mutex);
int pthread_mutex_unlock(pthread_mutex_t *mutex);
int pthread_mutex_trylock(pthread_mutex_t *mutex);

```
**返回值**

- 成功，返回 0
- 失败，返回 -1
### 初始化 init

**动态初始化**

局部变量应采用动态初始化。 `pthread_mutex_init(&mutex, NULL)`
```c
#include <pthread.h>
int pthread_mutex_init(pthread_mutex_t *restrict mutex, 
                       const pthread_mutexattr_t *restrict attr);
```

**参数**

- mutex，传出参数，调用时应传 &mutex

>restrict 关键字：只用于限制指针，告诉编译器，所有修改该指针指向内存中内容的操作，只能通过本指针完成。不能通过除本指针以外的其他变量或指针修改

- 属性。是一个传入参数，通常传 NULL，选用默认属性 (线程间共享)。

**静态初始化**

PTHREAD_MUTEX_INITIALIZER

如果互斥锁 mutex 是静态分配的（全局变量，或静态变量），可以直接使用宏进行初始化。`pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;`

### 解锁 unlock


unlock 主动解锁函数，**同时将阻塞在该锁上的所有线程全部唤醒**。所有线程会去争抢锁，至于哪个线程能够先拿到锁，取决于优先级，调度关系。
### 尝试加锁 trylock


trylock 加锁失败直接**返回**错误号（如：EBUSY），不阻塞。*pthread 库中都是返回错误号*


## 读写锁

**写独占，读共享**

**读锁、写锁并行阻塞，写锁优先级高**

特别强调：读写锁**只有一把**，但其具备两种状态：

1. 读模式下加锁状态 (读锁)
2. 写模式下加锁状态 (写锁)

```c
#include <pthread.h>
pthread_rwlock_t rwlock;
int pthread_rwlock_init(pthread_rwlock_t *restrict rwlock, 
                            const pthread_rwlockattr_t *restrict attr);
int pthread_rwlock_destroy(pthread_rwlock_t *rwlock);
// 请求读锁
int pthread_rwlock_rdlock(pthread_rwlock_t *rwlock);
// 请求写锁
int pthread_rwlock_wrlock(pthread_rwlock_t *rwlock);
// 解锁
int pthread_rwlock_unlock(pthread_rwlock_t *rwlock);
// 尝试读锁
int pthread_rwlock_tryrdlock(pthread_rwlock_t *rwlock);
// 尝试写锁
int pthread_rwlock_trywrlock(pthread_rwlock_t *rwlock);
```

## 条件变量

```c
#include <pthread.h>
pthread_cond_t cond;
pthread_cond_t cond = PTHREAD_COND_INITIALIZER;
int pthread_cond_init(pthread_cond_t *restrict cond, 
                          const pthread_condattr_t *restrict attr);
int pthread_cond_destroy(pthread_cond_t *cond);
int pthread_cond_wait(pthread_cond_t *restrict cond, 
                          pthread_mutex_t *restrict mutex);


```

### 初始化 init

```c
int pthread_cond_init(pthread_cond_t *restrict cond, 
                          const pthread_condattr_t *restrict attr);
// 静态初始化
pthread_cond_t cond = PTHREAD_COND_INITIALIZER;
```
**参数**

- cond，条件变量
- atrr，属性。通常传 NULL

### 销毁 destory

```c
int pthread_cond_destroy(pthread_cond_t *cond);
```
**只有在没有线程在该条件变量上等待的时候才能注销这个条件变量，否则返回 EBUSY**。
### 将线程加入到等待队列

```c
int pthread_cond_wait(pthread_cond_t *restrict cond, 
                          pthread_mutex_t *restrict mutex);
```
**参数**

- cond，条件变量
- mutex，互斥锁用于保护条件变量,以防止多个线程同时请求 `pthread_cond_wait()`


**用法**

使用`pthread_cond`时要配合使用`pthread_mutex`,先对`mutex`进行加锁，然后再使用`pthread_cond_wait`。

```c
pthread_mutex_t mutex;
pthread_cond_t cond;
pthread_mutex_lock(mutex);
pthread_cond_wait(&cond, &mutex);
// ... 操作共享资源
pthread_cond_signal(&cond);
pthread_mtuex_unlock(&mutex);
```
mutex 的作用

要直到`mutex`的具体作用，首先要了解`pthread_cond_wait`的原理。`ptherad_cond_wait`的原理大致如下：
```text
pthread_mutex_lock(&mutex)
pthread_cond_wait(&cond, &mutex)
    将调用者添加到条件变量的等待队列
    pthread_mutex_unlock(mutex); // 释放锁，让别的线程能操作条件变量
    挂起，被满足后唤醒
    pthread_mutex_lock(mutex);   // 再次加锁，准备操作共享资源
操作共享资源

```
可以看到，`pthread_cond_wait`函数最后会再次使用`mutex`加锁，因此，在操作完共享资源并使用`pthread_cond_signal`之后，仍要对 mutex 解锁。

**pthread_cond 的作用就是比 phtread_mutex 多了一个等待队列**


### 唤醒等待队列上的线程

唤醒至少一个
```c
int pthread_cond_signal(pthread_cond_t *cond);
```
唤醒等待队列上的所有线程
```c
int pthread_cond_broadcast(pthread_cond_t *cond);
```
### pthread_cond 源码


```c
typedef struct
{
  int c_spinlock;                  /* Spin lock to protect the queue.  */
  struct _pthread_queue c_waiting; /* Threads waiting on this condition.  */
} pthread_cond_t;

// 调用 pthread_cond_wait 之前，mutex 已经被加锁
int pthread_cond_wait(pthread_cond_t *cond, pthread_mutex_t *mutex)
{
  volatile pthread_t self = thread_self();
  // 加锁操作队列
  acquire(&cond->c_spinlock);
  // 插入条件的等待队列
  enqueue(&cond->c_waiting, self);
  // 操作完释放锁
  release(&cond->c_spinlock);
  // 释放互斥变量，否则别人无法操作资源，导致条件一直无法满足
  pthread_mutex_unlock(mutex);
  // 挂起等待条件满足后被唤醒
  suspend_with_cancellation(self);
  // 被唤醒后重新获取互斥锁
  pthread_mutex_lock(mutex);
  /* This is a cancellation point */
  // 取消点，等待期间被取消了
  if (self->p_canceled && self->p_cancelstate == PTHREAD_CANCEL_ENABLE) {
    /* Remove ourselves from the waiting queue if we're still on it */
    acquire(&cond->c_spinlock);
    // 线程准备退出，从条件阻塞队列中移除
    remove_from_queue(&cond->c_waiting, self);
    release(&cond->c_spinlock);
    pthread_exit(PTHREAD_CANCELED);
  }
  return 0;
}
// 条件满足，唤醒线程
int pthread_cond_signal(pthread_cond_t *cond)
{
  pthread_t th;

  acquire(&cond->c_spinlock);
  // 取出一个被被阻塞的线程
  th = dequeue(&cond->c_waiting);
  release(&cond->c_spinlock);
  // 发送信号唤醒他
  if (th != NULL) restart(th);
  return 0;
}

// 给pid进程发送唤醒信号
static inline void restart(pthread_t th)
{
  kill(th->p_pid, PTHREAD_SIG_RESTART);
}
// 条件满足，唤醒所有线程
int pthread_cond_broadcast(pthread_cond_t *cond)
{
  pthread_queue tosignal;
  pthread_t th;

  acquire(&cond->c_spinlock);
  /* Copy the current state of the waiting queue and empty it */
  tosignal = cond->c_waiting;
  // 重置阻塞队列
  queue_init(&cond->c_waiting);
  release(&cond->c_spinlock);
  /* Now signal each process in the queue */
  // 发送信号唤醒所有线程
  while ((th = dequeue(&tosignal)) != NULL) restart(th);
  return 0;
}
