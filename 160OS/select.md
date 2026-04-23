---
tags:
  - os/io
  - type/concept
mastery: 5
---
## 原理

select 实现多路复用的方式是，将已连接的 Socket 都放到一个**文件描述符集合**，然后调用 select 函数将文件描述符集合**拷贝**到内核里，让内核来检查是否有网络事件产生，检查的方式很粗暴，就是通过**遍历**文件描述符集合的方式，当检查到有事件产生后，将此 Socket 标记为可读或可写，接着再把整个文件描述符集合**拷贝**回用户态里，然后用户态还需要再通过**遍历**的方法找到可读或可写的 Socket，然后再对其处理。

所以，对于 select 这种方式，需要进行 **2 次「遍历」文件描述符集合**，一次是在内核态里，一个次是在用户态里 ，而且还会发生 **2 次「拷贝」文件描述符集合**，先从用户空间传入内核空间，由内核修改后，再传出到用户空间中。

select 使用固定长度的 BitsMap，表示文件描述符集合，而且所支持的文件描述符的个数是有限制的，在 Linux 系统中，由内核中的 FD_SETSIZE 限制， 默认最大值为 `1024`，只能监听 0~1023 的文件描述符。

## 使用

```c
#include <sys/select.h>
/* According to earlier standards */
#include <sys/time.h>
#include <sys/types.h>
#include <unistd.h>

// 操作 fd_set 集合的函数
void FD_CLR(int fd, fd_set *set); 	// 集合fd 位清 0
int FD_ISSET(int fd, fd_set *set); 	// 测试 fd 位是否置 1
void FD_SET(int fd, fd_set *set); 	// 集合 fd 位置 1
void FD_ZERO(fd_set *set); 			// 集合里所有位清 0

int select(int nfds, fd_set *readfds, fd_set *writefds, 
               fd_set *exceptfds, struct timeval *timeout);

struct timeval {
    long    tv_sec;         /* seconds */
    long    tv_usec;        /* microseconds */
};      
```
fd_set 简单地理解为一个长度是 1024 的比特位，每个比特位表示一个需要处理的文件描述符。

**参数**

- `nfds`：监控的文件描述符集里**最大文件描述符加 1**，因为此参数会告诉内核检测前多少个文件描述符的状态
- `readfds`：监控有读数据到达文件描述符集合，**传入传出**参数
- `writefds`：监控写数据到达文件描述符集合，**传入传出**参数
- `exceptfds`：监控异常发生达文件描述符集合, 如带外数据到达异常，**传入传出**参数
- `timeout`：表示阻塞的时间，如果是 0 表示非阻塞模型，NULL 表示永远阻塞，直到有数据来
  1. NUll，永远等下去
  2. 设置 timeval，等待固定时间
  3. 设置 timeval 都为 0，检查描述字后立即返回，轮询

**返回值**

- 正数： 表示`readfds`和`writefds`就绪事件的总数
- 0：超时返回 0
- -1：出现错误

**select 使用流程**

1. 使用`FD_`系列位图操作函数设置 readfds、writefds 集合
2. 调用`select`，阻塞直到有文件描述符就绪，返回
3. 遍历传出的`fd_set`，使用`FD_ISSET`来找到就绪的描述符
4. 对就绪的文件描述符进程处理
  

![[select.png]]

## 特点

**优点**

- select 目前几乎在所有的平台上支持，`其良好跨平台支持也是它的一个优点`。

**缺点**


1. **select 最大的缺陷就是单个进程所打开的 FD 是有一定限制的，它由 FD_SETSIZE 设置，默认值是 1024。** 要想调整只能重新编译内核，因为`fd_set的定义为`
```c
typedef long int __fd_mask;

/* fd_set for select and pselect.  */
typedef struct
  {
#ifdef __USE_XOPEN
    __fd_mask fds_bits[__FD_SETSIZE / __NFDBITS];
# define __FDS_BITS(set) ((set)->fds_bits)
#else
    __fd_mask __fds_bits[__FD_SETSIZE / __NFDBITS];
# define __FDS_BITS(set) ((set)->__fds_bits)
#endif
  } fd_set;
```

> 具体数目可以`cat /proc/sys/fs/file-max`查看

2. **对 socket 进行扫描时是线性扫描，即采用轮询的方法，效率较低。**

> 当套接字比较多的时候，每次 select ()都要通过遍历 FD_SETSIZE 个 Socket 来完成调度。

3. **需要维护一个用来存放大量 fd 的`fd_set`，这样会使得用户空间和内核空间在传递该结构时复制开销大。**