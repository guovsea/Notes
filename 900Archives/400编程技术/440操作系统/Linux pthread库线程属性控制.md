使用 pthread_create ()函数创建线程时，可以通过传入参数 attr 来设置线程的属性，该参数是一个 pthread_attr_t 类型的结构体，在调用 pthread_create ()之前应先初始化该结构体。pthread_attr_t 结构体的定义如下：

```c
typedef struct
{
    int                     etachstate;     //线程的分离状态
    int                     schedpolicy;     //线程调度策略
    struct sched_param    schedparam;     //线程的调度参数
    int                     inheritsched;     //线程的继承性
    int                     scope;             //线程的作用域
    size_t                 guardsize;         //线程栈末尾的警戒缓冲区大小
    int                    stackaddr_set;    //线程栈的设置
    void*                 stackaddr;         //线程栈的位置
    size_t                 stacksize;         //线程栈的大小
} pthread_attr_t;
```

该结构体中成员的值不能直接修改，须使用函数进行相关操作。初始化线程属性结构体的函数为 pthread_attr_init ()，这个函数必须在 pthread_create ()之前调用，且线程终止后须通过 pthread_attr_destroy () 函数销毁属性资源。

pthread_attr_init ()函数与 pthread_attr_destroy ()函数都存在于函数库 pthread. h 中，它们的声明分别如下：

```c
int pthread_attr_init(pthread_attr_t *attr);
int pthread_attr_destroy(pthread_attr_t *attr);
```

调用 pthread_attr_init ()后，线程属性 attr 会被设置为默认值，默认情况下线程处于非绑定、非分离状态，并与共享父进程优先级。若要使用默认状态，将 pthread_create ()函数中的参数 attr 设置为 NULL 即可；若要使用自定义属性创建线程，则需使用 Linux 系统中提供的接口函数去修改程序中 pthread_attr_t 结构体变量各成员的值。这些接口函数都存在于函数库 pthread. h 中，下面将对线程属性结构体中常用状态及其相关函数分别进行讲解。

**1、 线程的分离状态**

线程的分离状态决定一个线程终止自身运行的方式，默认情况下线程处于非分离状态，Linux 系统中可以通过 pthread_attr_setdetachstate ()函数修改线程属性中的分离状态，此外，Linux 系统还提供了 pthread_attr_getdetachstate ()函数，用于获取线程的分离状态。这两个函数的声明如下：

```
int pthread_attr_setdetachstate(pthread_attr_t *attr, int detachstate);
int pthread_attr_getdetachstate(pthread_attr_t *attr, int *detachstate);
```

其中参数 attr 表示线程属性，detachstate 表示线程的分离状态属性。调用 set 函数时，若将其参数 detachstate 设置为 PTHREAD_CREATE_DETACHED，线程创建后将以分离状态启动。若函数调用成功则返回 0，否则返回 errno。

**2、 线程的调度策略**

线程的调度策略决定了系统调用该线程的方法，Linux 系统中的调度策略分为三种：SCHED_OTHER、SCHED_FIFO、SCHED_RR，这三种调度策略的含义分别如下：

● SCHED_OTHER，分时调度策略；

● SCHED_FIFO，实时调度策略，先到先服务；

● SCHED_RR，实时调度策略，按时间片轮询。

其中分时调度策略通过 nice 值和 counter 值决定调度权值，nice 值越小、counter 越大，被调用的概率越高；实时调度策略通过实时优先级决定调度权值，若线程已准备就绪，除非有优先级相同或更高的线程正在运行，否则该线程很快便会执行。

而实时调度策略 SCHED_FIFO 与 SCHED_RR 的不同在于：调度策略为 SCHED_FIFO 的进程一旦获取 cpu 便会一直运行，除非有优先级更高的任务就绪或主动放弃 cpu；调度策略为 SCHED_RR 的进程则根据时间片轮询，若线程占用 cpu 的时间超过一个时间片，该线程就会失去 cpu，并被置于就绪队列队尾，确保与该进程优先级相同且调度策略为 SCHED_FIFO 或 SCHED_RR 能被公平调度。

Linux 系统中用于设置和获取线程的调度策略的函数分别为 pthread_attr_setschedpolicy ()和 pthread_attr_getschedpolicy ()，这两个函数的声明分别如下：

```c
int pthread_attr_setschedpolicy(pthread_attr_t *attr, int policy);
int pthread_attr_getschedpolicy(pthread_attr_t *attr, int *policy);
```

其中参数 attr 表示线程属性，policy 表示线程的调度策略，policy 的默认值为 SCHED_OTHER，调度策略 SCHED_FIFO 和 SCHED_RR 只对超级用户有效。若函数调用成功则返回 0，否则返回 errno。

**3、 线程的调度参数**

线程的调度参数是一个 struct sched_param 类型的结构体，该结构体中只有一个成员 sched_priority，该参数是一个整型变量，代表线程的优先级，仅当调度策略为 SCHED_FIFO 或 SCHED_RR 时成员 sched_priority 有效。Linux 系统中用于设置和获取调度参数的函数为 pthread_attr_setschedparam ()和 pthread_attr_getschedparam ()，函数声明分别如下：

```c
int pthread_attr_setschedparam(pthread_attr_t *attr,
​                   const struct sched_param *param);
int pthread_attr_getschedparam(pthread_attr_t *attr,
​                    struct sched_param *param);
```

其中参数 attr 代表线程属性，参数 param 代表线程的调度参数，param 中成员 sched_priority 的默认值为 0。若函数调用成功则返回 0，否则返回 errno。

**4、 线程的继承性**

线程的继承性决定线程调度策略属性和线程调度参数的来源，其来源有两个，一是从创建该线程的线程属性中继承，二是从该线程属性结构体中获取。线程的继承性没有默认值，若要使用该属性，必须对其进行设置。Linux 系统中用来设置和获取线程继承性的函数为 pthread_attr_setinheritsched ()和 pthread_attr_getinheritsched ()，这两个函数的函数声明分别如下：

```c
int pthread_attr_setinheritsched(pthread_attr_t *attr,int inheritsched);
int pthread_attr_getinheritsched(pthread_attr_t *attr,int *inheritsched);
```

其中参数 attr 代表线程属性，参数 inheritsched 代表线程的继承性，该参数的常用取值为 PTHREAD_INHERIT_SCHED 和 PTHREAD_EXPLICIT_SCHED，其中 PTHREAD_INHERIT_SCHED 表示使新线程继承其父线程中的调度策略和调度参数；PTHREAD_EXPLICIT_SCHED 表示使用在 attr 属性中显示设置的调度策略和调度参数。若函数调用成功则返回 0，否则返回 errno。

**5、 线程的作用域**

线程的作用域控制线程获取资源的范围，Linux 系统中使用 pthread_attr_setscope ()函数和 pthread_attr_getscope ()函数设置和获取线程的作用域，这两个函数的声明分别如下：

```c
int pthread_attr_setscope(pthread_attr_t *attr, int scope);
int pthread_attr_getscope(pthread_attr_t *attr, int *scope);
```

其中参数 attr 代表线程属性；参数 scope 代表线程的作用域，该参数常用的取值为 PTHREAD_SCOPE_PROCESS 和 PTHREAD_SCOPE_SYSTEM，分别表示在进程中竞争资源和在系统层级竞争资源。若函数调用成功则返回 0，否则返回-1。

**6、 线程的栈**

线程中有属于该线程的栈，用于存储线程的私有数据，用户可以通过 Linux 系统中的系统调用，对这个栈的地址、栈的大小以及栈末尾警戒区的大小等进行设置，其中栈末尾警戒区用于防止栈溢出时栈中数据覆盖附近内存空间中存储的数据。

一般情况下使用默认设置即可，但是当对效率要求较高，或线程调用的函数中局部变量较多、函数调用层次较深时，可以从实际情况出发，修改栈的容量。Linux 系统中用于修改和获取线程栈空间大小的函数为 pthread_attr_setstacksize ()和 pthread_attr_getstacksize () ，这两个函数的声明分别如下：

```c
int pthread_attr_setstacksize(pthread_attr_t *attr, size_t stacksize);
int pthread_attr_getstacksize(pthread_attr_t *attr, size_t *stacksize);
```

其中参数 attr 代表线程属性，参数 stacksize 代表栈空间大小。若函数调用成功则返回 0，否则返回 errno。

Linux 中也提供了用于设置和获取栈地址、栈末尾警戒区大小的函数，它们的函数声明分别如下：

```c
int pthread_attr_setstackaddr(pthread_attr_t *attr, void *stackaddr);
int pthread_attr_getstackaddr(pthread_attr_t *attr, void **stackaddr);
int pthread_attr_setguardsize(pthread_attr_t *attr, size_t guardsize);
int pthread_attr_getguardsize(pthread_attr_t *attr, size_t *guardsize);
```

当改变栈地址属性时，栈警戒区大小通常会被清零。若函数调用成功则返回 0，否则返回 errno。

此外，Linux 系统中还提供了 pthread_attr_setstach ()函数和 pthread_attr_getstack () 函数，这两个函数可以在一次调用中设置或获取线程属性中的栈地址与栈容量，它们的函数声明分别如下：

```c
int pthread_attr_setstack(pthread_attr_t *attr,
void *stackaddr, size_t stacksize);
int pthread_attr_getstack(pthread_attr_t *attr,
void **stackaddr, size_t *stacksize);
```

其中的参数 attr、stackaddr、stacksize 分别代表线程属性、栈空间地址、栈空间容量。若函数调用成功则返回 0，否则返回 errno。

以上介绍的几组函数都用于在创建线程前对线程属性进行设置，下面通过一个案例来展示使用线程属性控制线程状态的方法。

案例 8：在程序中通过设置线程属性的方式设置线程分离状态和线程内部栈空间容量、栈地址，使程序不断创建线程，耗尽内存空间，并打印线程编号。

```c
 include <stdio.h>
 include <pthread.h>
 include <string.h>
 include <stdlib.h>
 include <unistd.h>
 define SIZE 0x90000000
 oid *th_fun(void *arg)
 
    while (1) 
         sleep(1);
 }
 int main()
 {
     pthread_t tid;                    //线程id
     int err, detachstate;
     int i = 1;
     pthread_attr_t attr;                //线程属性
     size_t stacksize;                //栈容量
     void *stackaddr;                    //栈地址
     pthread_attr_init(&attr);        //初始化线程属性结构体
     //获取程栈地址、栈容量
     pthread_attr_getstack(&attr, &stackaddr, &stacksize);
     //获取线程分离状态
     pthread_attr_getdetachstate(&attr, &detachstate);
     //判断线程分离状态
     if (detachstate == PTHREAD_CREATE_DETACHED)
         printf("thread detached\n");
     else if (detachstate == PTHREAD_CREATE_JOINABLE)
         printf("thread join\n");
     else
         printf("thread un known\n");
     //设置线程分离状态，使线程分离
     pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_DETACHED);
     while (1) {
         //在堆上申请内存,指定线程栈的起始地址和大小
         stackaddr = malloc(SIZE);
         if (stackaddr == NULL) {
             perror("malloc");
             exit(1);
         }
         stacksize = SIZE;
         //设置线程栈地址和栈容量
         pthread_attr_setstack(&attr, stackaddr, stacksize);
         //使用自定义属性创建线程
         err = pthread_create(&tid, &attr, th_fun, NULL);
         if (err != 0) {
             printf("%s\n", strerror(err));
             exit(1);
         }
         i++;
         printf("%d\n", i);                //打印线程编号
     }
     pthread_attr_destroy(&attr);        //销毁attr资源
     return 0;
 }
```

由于系统中栈空间的总容量是有限的，因此系统中可创建的栈的数量与栈的容量成反比，若案例 8 中成功设置栈的容量，案例最终输出的栈的编号将会被改变（当然由于系统中不只有这一个线程，小范围的波动可忽略不计）。

分析案例 8，其中第 6 行代码使用宏定义了自定义栈空间的大小，第 14~19 行代码中定义了程序中需要用到的变量，第 20 行代码将线程属性变量 attr 初始化为系统默认值，第 22 行代码通过传参的方式使用 pthread_attr_getstack ()函数初始化了变量 stacksize 和 stackaddr，第 24 行代码使用 pthread_attr_getdetachstate () 函数获取线程分离状态，第 26~31 行代码对线程属性中的分离状态进行了判断，第 33 行代码将 attr 属性中的成员 etachstate 设置为分离态，第 34~52 行代码在循环中使用自定义的线程属性创建线程，设置线程栈空间地址，不断消耗系统内存空间，并打印线程编号，第 53 行代码销毁线程中的 attr 资源。

编译案例 8，执行程序，执行结果中最后打印的信息如下：

```
……
5456
5457
malloc: Cannot allocate memory
```

修改案例 8 中宏 SIZE 的值为 0x10000000，重新编译并执行程序，终端最后打印的信息如下：

```
……
7365
7366
Resource temporarily unavailable
```

对比两次执行结果可知，案例 8 成功通过设置线程属性修改了线程的状态。

当然用户虽然能修改栈容量，但系统中栈的数量是有限的，因此当栈容量小于某个值时，栈的总数将不会再改变。