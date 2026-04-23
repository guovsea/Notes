---
tags:
  - os/thread
  - type/howto
mastery: 5
---
**在编译源代码的时候需要链接pthread库，编译选项需要加上 -lpthread**

## 线程的运行模型

**线程创建**

创建一个新线程时，操作系统**将在进程的地址空间中创建该线程的栈**，操作系统调度线程的同时也会管理这个栈。

**线程运行**

线程之间并发地执行，共享存储资源。应该使用某种方式进行同步。

**线程结束**

主线程和进程的生命周期相同。进程存在时，主线程的栈就存在。

主线程`return`和 `exit` 等效，都将导致进程结束。

其他线程中`return`和`pthread_exit()`等效，都只会导致所在的线程结束。

**线程结束后，操作系统会释放其包括栈空间在内的大部分资源，只会保留其终止状态资源**。可以在主线程（main 函数所在的线程）中使用`pthread_join`获取并释放其终止状态资源。*终止状态包括 Thread Return Value*

**函数不能返回指向其局部变量的指针**。局部变量都创建在栈上，当函数结束时，栈将会被销毁。

线程同样的，**线程返回指针时只能返回指向动态分配空间的指针、指向全局变量的指针、main 函数中局部变量的指针**.


## pthread 线程库API

### 有返回值函数的返回值

为什么是**成功返回 0，失败返回错误号**而不是返回错误号而不是直接设置 `errno` 。

原因：它们是 POSIX 线程库（pthread）的函数，而 `errno` 是标准 C 库的错误码变量。POSIX 线程库的设计与标准 C 库是分离的，因此它们使用了自己的错误处理机制，包括返回错误号而不是设置全局 `errno` 变量。

由于 pthread_create 的错误码不保存在 errno 中，因此不能直接用 `perror()`打印错误信息，可以先用`strerror(返回值)`把错误码转换成错误信息再打印。

```c
#include <string.h>
char *strerror(int errnum)
```

### pthread_t 类型

pthread_t 线程 ID 类型。在 Linux 中是`unsigned long`，在其他 Unix-like 系统可能是结构体，地址等等，所以不能简单地当成整数用 printf 打印。

它只在当前进程中保证是唯一的，两个进程间，线程 ID 允许相同。

### pthread_self 函数

获取线程 ID。其作用对应进程中 getpid () 函数。

```c
#include <pthread.h>
pthread_t pthread_self(void);
```

**返回值**

- 成功：0
- 失败：无

### pthread_create 函数
```c
#include <pthread.h>
int pthread_create(pthread_t *thread, pthread_attr_t *attr,void *(*start_routine)(void *),void *arg);
```
**参数**

- thread，传出参数，函数返回后将为创建的线程 ID 
- attr，通常传 NULL，表示使用线程默认属性。若想使用具体属性也可以修改该参数，操作系统会根据 attr 对线程进行管理。
- start_routine，函数指针，指向线程的执行函数。
- arg，线程主函数执行期间所使用的参数的地址。参数可以是任何类型：`int、char、struct、数组`，因为本质上就只是在传递地址，在使用前要强转成实际的类型，传入参数时要使用`(void*)`强转。

**返回值**

- 成功：0
- 失败：**直接返回错误号**


`start_routine` 函数接收一个参数，是通过 `pthread_create` 的 `arg` 参数传递给它的，该参数的类型为`void *`，这个指针按什么类型解释由调用者自己定义。`start_routine` 的返回值类型也是`void *`，这个指针按什么类型解释由调用者自己定义


### pthread_join 函数

调用该函数的线程将挂起等待，直到 id 为 thread 的线程终止。(不一定是主线的 join 子线程，也可能是子线程间互相 join)
```c
#include <pthread.h>
int pthread_join(pthread_t threadid, void **retval);
```

**参数**

- 线程ID （**注意：不是指针**）
- `retval`：线程返回值 (Thread Return Value)。**指针的指针**，存储线程结束状态，**二级指针**

**返回值**

- 成功：0
- **失败：错误号**

线程终止的原因不同，通 `pthread_join` 得到的线程返回值就不同：

| 线程终止原因                 | `retval`**指向**         |
| ---------------------------- | ------------------------ |
| 通过 return 返回             | 线程函数的返回值         |
| 其他线程调用`pthread_cancel` | 常数 PTHREAD_CANCELED = -1    |
| 自己调用 `pthread_exit` 终止 | 传给`pthread_exit`的参数 |

*对 thread 线程的终止原因不感兴趣，可以传 NULL 给 retval 参数。*

为什么`retval`是双重指针？

因为`retval`最后是一个**指向返回值的指针**，并且它又是一个**传出参数**。

被取消的线程，退出值定义在 Linux 的 pthread 库中。常数 PTHREAD_CANCELED 的值是-1。可在头文件 pthread. h 中找到它的定义：**`#define PTHREAD_CANCELED ((void *) -1)`** 。因此当我们对一个已经被取消的线程使用 pthread_join 回收时，得到的返回值为 -1。
### pthread_detach 函数

detach(分离)子线程：子线程的回收不再由主线程调用`pthread_join()`调用，子线程一旦运行结束，操作系统便回收子线程的所有资源。（而不是保留终止状态资源）

```c
#include <pthread.h>
int pthread_detach(pthread_t thread);
```
**参数**

- 线程 ID

**返回值**

- 成功 0
- 失败返回错误号

如果主线程不需要`pthread_join`获取子线程返回值，最好使用`pthread_detach`分离子线程。

如果不使用`pthread_detach`或者`pthread_join`，那么在创建很多子线程后会出现：

```text
32727
32636
32728
Resource temporarily unavailable
```
### pthread_exit 函数

```c
#include <pthread.h>
void pthread_exit (void *retval);
```
**参数**

- `retval` 表示线程返回值，通常传 NULL

### pthread_cancel 函数

结束某个线程

```c
#include <pthread.h>
int pthread_cancel (pthread_t thread);
```

- 线程 ID 

**返回值**

- 成功：0
- 失败：错误号

**注**：**线程的取消并不是实时的，而有一定的延时。需要等待线程到达某个取消点(检查点)。**

**取消点**：是线程检查是否被取消，并按请求进行动作的一个位置。通常是一些系统调用 creat，open，pause，close，read，write..... 执行命令 man 7 pthreads 可以查看具备这些取消点的系统调用列表。*也可参阅 APUE.12.7 取消选项小节*。

可粗略认为一个系统调用(进入内核)即为一个取消点。如线程中没有取消点，可以通过调用 `pthread_testcancel` 函数自行设置一个取消点。

### 线程属性控制

一般用不上

[[Linux pthread库线程属性控制]]

## 线程同步

[[Linux pthread库线程同步]]

## 相关

- [[线程]]
- [[Linux pthread库线程同步]]
- [[Linux pthread库线程属性控制]]
- [[线程安全]]