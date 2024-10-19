## 原理

**信号量（Semaphore）其实是一个整型的计数器，主要用于实现进程间和线程间的互斥与同步。** 但是这也是一种广义上的通信。

信号量表示资源的数量，控制信号量的方式有两种原子操作：

- **P 操作** `wait()`
  1. 信号量 - 1
  2. 相减后信号量 < 0，执行该操作的进程阻塞；相减后信号量 >= 0，则获取一个该资源
- **V 操作** `signal()`
  1. 信号量 + 1
  2. 相减后信号量 <= 0，唤醒被阻塞的进程；相减后信号量 > 0，则没有进程阻塞

使用信号量的特殊场景

1. **信号量初始值为 1**
   可以**保护临界资源**最多只被一个进程访问
1. **信号量初始值为 0**
   **同步信号量**，它可以保证进程 A 应在进程 B 之前执行。
   *因为必须得先 V 后 P，V 后信号量才为 1* 
## 初始化信号量

```c
include <semaphore.h>
sem_t semaphore;
int sem_init(sem_t *sem, int pshared, unsigned int value);
```
**参数**

- sem 传入的 sem_t 类型的信号量变量
- pshared 控制信号量的类型，如果其值为 0，就表示信号量是当前进程的局部信号量，否则信号量就可以在多个进程间共享
- value 信号量的初始值

## V 操作

```c
include <semaphore.h>
int sem_post(sem_t *sem);
```
## P 操作

```c
include <semaphore.h>
int sem_wait(sem_t *sem);
```
## 销毁 Semaphore

```c
int sem_destroy(sem_t *sem);
```

[multithreading - Conditional Variable vs Semaphore - Stack Overflow](https://stackoverflow.com/questions/3513045/conditional-variable-vs-semaphore)

信号量和条件变量的区别