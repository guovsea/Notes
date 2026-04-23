---
tags:
  - os
  - os/signal
  - type/concept
mastery: 5
---
信号是异步的，它会在程序的任何地方发生。程序正常的执行路径被打破，去执行信号处理函数。**一般情况下，进程正在执行某个系统调用，那么在该系统调用返回前信号是不会被传递的，但慢速系统调用除外。如果对某个信号设置了捕捉，那么产生该信号时慢速系统调用会被中断，返回 -1，并设置 errno 为 EINTR，然后去执行信号处理程序**。因此，**当慢速系统调用失败时要判断 errno 是否为 EINTER，如果为 EINTER 需要重启系统调用。**

## 慢系统调用

该术语适用于那些可能被永远阻塞的系统调用。永远阻塞的系统调用是指调用永远无法返回，多数网络支持函数都属于这一类，比如：若没有客户连接到服务器上，那么服务器的accept调用就会一直阻塞。

慢系统调用可以被永久阻塞，包括以下几个类别：

- 读写"慢"设备(包括 pipe、终端设备、网络连接等)。读时，数据不存在，需要等待；写时，缓冲区满或者某些原因，需要等待。读写磁盘文件一般不会阻塞
- 当打开某些特殊文件时，需要等待某些条件，才能打开。比如：打开中断设备时，需要等到连接设备的 modem 响应才能被完成
- pause 和 wait 函数。pause 函数使调用进程睡眠，直到捕获一个信号。wait 等待子进程终止。
- 某些 ioctl 操作
- 某些 IPC 操作

## EINTER 错误的产生


慢速系统调用执行时，比如 read 等待输入期间，如果收到一个信号，系统将中断 read，转而执行信号处理函数。当信号处理返回后，操作系统中断系统调用，并让系统调用失败同时设置 errno 为 EINTR（perror 的错误描述为`Interrupted system call`））。**但是中断了的系统调用是没有完成的调用，它的失败是临时性的，如果再次调用则可能成功，这并不是真正的失败，所以要对这种情况进行处理**。 
## 处理方法

处理方法有以下三种：

1. **人为重启被中断的系统调用** *编程时一定要重启慢速系统调用*
2. 信号时设置 SA_RESTART 属性
3. 忽略中断信号（让系统不产生信号中断）

### 重启被中断系统调用

**在编程的时候，一定要重启慢速系统调用**

**goto 语句**

```c
again:
    if ((n = read(fd， buf， BUFFSIZE)) < 0) {
        if (errno == EINTR)
            goto again;     /* just an interrupted system call */
        /* handle other errors */
    }
```
**continue**

```c
connfd = accept(listenfd, (struct sockaddr *)&client_addr, &client_len);
if (connfd < 0) {
    if (errno == EINTR) {
        continue;
    }
    sys_err("accept error");
}
```

### 信号捕捉设置 SA_RESTART

并非所有的系统调用都可以通过指定 SA_RESTART 来达到自动重启的目的，因此并不保险。
```c
struct sigaction newact;
newact.sa_handler = do_sigchild;
sigemptyset(&newact.sa_mask);
newact.sa_flags = 0;
knewact.sa_flags |= SA_RESTART;      // 重启被中断的系统系统调用
sigaction(SIGCHLD, &newact, NULL);
```

### 忽略信号

忽略系统产生的**所有信号**（除了特权信号：SIGSTOP 和 SIGKILL）。但有时候需要用的信号。

```cpp
struct sigaction action;
action.sa_handler = SIG_IGN;
sigemptyset(&action.sa_mask);
sigaction(SIGALRM, &action, NULL);
```

[Unix/Linux编程：系统调用的中断与重启\_siginterrupt-CSDN博客](https://blog.csdn.net/zhizhengguan/article/details/117332391#:~:text=%E6%80%BB%E7%BB%93,%E7%B3%BB%E7%BB%9F%E4%B8%8D%E4%BA%A7%E7%94%9F%E4%BF%A1%E5%8F%B7%E4%B8%AD%E6%96%AD%EF%BC%89%E3%80%82)