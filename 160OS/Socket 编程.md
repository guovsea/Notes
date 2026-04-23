---
tags:
  - os
  - os/network
  - type/howto
mastery: 5
---Socket 通信不仅可以跨网络与不同主机的进程间通信，还可以在同主机上进程间通信。

## 网络字节序

TCP/IP 协议规定，网络数据流应采用大端 [[字节序]]，即低地址高字节。

而本地的机器即可能是大端模式，也可能是小端模式。

本地字节序和网络字节序可以相互转换。[[字节序#转换函数]]

## TCP socket 传输模型

### send buffer 满的情况

已经发送到网络的数据依然需要暂存在 send buffer 中，只有收到对方的 ACK 后，kernel 才从 buffer 中清除这一部分数据，为后续发送数据腾出空间。接收端会将收到的数据暂存在 receive buffer 中，由操作系统进行确认并向发送端发送 ACK 。

如果对端打开 socket 的进程没有及时将数据从 receive buffer 中取出，最终导致 receive buffer 填满，**由于 TCP 的流量控制**，接收端会阻止发送端向其发送数据，如果发送端持续调用 write 发送数据， send buffer 最终会被填满，再次调用 write 将会阻塞。这些控制皆发生在 TCP/IP 栈中，对应用程序是透明的。**应用程序就只会看到：write 调用阻塞。**

一般来说，由于**接收端进程从 socket 读数据的速度发送端进程向 socket 写数据的速度**或者**发送端的发送速率小于进程向 socket 写数据的速率**，最终将导致**发送端 write 调用阻塞。**

### read/write 在 socket 中的读写行为

每个 socket 中都对应两个缓冲区：发送缓冲区/接收缓冲区。**对 socketfd 进行 read 就是从缓接收缓冲区中读数据；对 socketfd 进行 write 就是向发送缓冲区中写数据。**

read/write 的行为和 socket 的模式有关。可以使用 [[fcntl函数]] 控制 socket 的模式：blocking 或者 nonblock。

#### blocking 模式

- 当 receive buffer 为空时，read 会阻塞
- 只要 receive buffer 不为空时，read 就立即返回，read 至少返回 1 个数据
- 当 send buffer 放不下待写入数据时，write 会阻塞
- 当 send buffer

#### nonblock 模式
```c
// 设置 connfd 为 
flag = fcntl(connfd, F_GETFL);
flag |= O_NONBLOCK;
fcntl(connfd, F_SETFL, flag);

```
- 当 receive buffer 为空时，立即返回 -1 并设置 errno = EAGAIN （等价于 EWOULDBLOCK）
- 只要 receive buffer 不为空时，read 就立即返回，read 至少返回 1 个数据
- 当 send buffer 放不下待写入数据时，write 返回能够放入的字节数，并放入这些字节
- 当 send buffer 满了是调用则返回-1（errno = EAGAIN 或 EWOULDBLOCK）
- **特例：当 write 正阻塞等待时对端关闭了 socket，则 write 则会立即将剩余缓冲区填满并返回所写的字节数**
#### 区别

|          | recv buf 空                  | send buf 放不下                            | send buf 满                    |
| -------- | ---------------------------- | ------------------------------------------ | ------------------------------ |
| blocking | read 阻塞                    | write 阻塞                                 | write                          |
| nonblock | read 返回 -1，errno = EAGAIN | write 返回能够放入的字节数，errno = EAGAIN | write 返回 -1 ，errno = EAGAIN |

[[文件打开模式]]

- [ ] 文件打开模式

### read/write 对连接异常的反馈行为

对应用程序来说，与另一进程的TCP通信其实是**完全异步**的过程：

1. 不知道对端能否收到、何时收到我的数据
2. 不知道什么时候能够收到对端的数据
3. 不知道什么时候通信结束、退出的原因（主动退出或是异常退出、机器故障、网络故障等等）

对于 1 和 2，采用客户端先`write`再`read`服务器端先`read`再`write`，这样“一读一写”的序列，并且服务器端采用 **blocking read** 或者 **nonblock read + 轮询**的方式，基本可以保证正确的处理流程。

对于 3 ，**kernel 将这些事件的“通知”通过 read/write 的结果返回给应用层。**

假设A机器上的一个进程a正在和B机器上的进程b通信：某一时刻a正阻塞在socket的read调用上（或者在nonblock下轮询socket）

**情况 1** ：主动关闭端正常关闭（进程显式关闭 socket/进程结束，OS 回收 socket）发送 FIN 包，**被动关闭端接收到 FIN 包** 。

此时，有两种情况

- 被动关闭端收到 FIN 包时进程正阻塞在 read 调用上。则进程会被唤醒，read 调用返回 EOF
- 被动关闭端收到 FIN 包之后调用调用 read。则 read 会立即返回 EOF。

socket 在收到 FIN 后，虽然调用 read 会返回 EOF，但**进程依然可以调用 write，因为根据 TCP 协议，收到对方的 FIN 包只意味着对方不会再发送任何消息，但还可以向对方发送数据**。 在一个双方正常关闭的流程中，收到 FIN 包的一端将剩余数据发送给对面（通过一次或多次 write），然后关闭 socket。因此也就引出下一种情况。

*但是事情远远没有想象中简单。优雅地关闭一个 TCP 连接，不仅仅需要双方的应用程序遵守约定，中间还不能出任何差错。*

**情况 2** ：假如主动关闭端的关闭原因是进程异常终止，发送 FIN 包是 OS 代劳的，b 进程已经不复存在，**当机器再次收到该 socket 的消息时，会回应 RST（因为拥有该 socket 的进程已经终止）**

**进程对收到 RST 的 socket 调用 write 时，操作系统会给该进程发送 SIGPIPE，默认处理动作是终止进程**

**情况 3**：并没有主动关闭端，TCP 连接异常终止：**OS 崩溃**（不同于**关机**，关机时所有进程的退出动作依然能够得到保证）/**主机断电**/**网络不可达时**等情况，**被动关闭端进程根本不会收到 FIN 包作为连接终止的提示。**

如果某端的进程正阻塞在 read 上，那么结果只能是永远的等待。

如果某端的进程先 write 然后阻塞在 read，由于收不到对端回应的 ACK，TCP 会持续重传12次（时间跨度大约为9分钟），然后在阻塞的 read 调用上返回错误：ETIMEDOUT/EHOSTUNREACH/ENETUNREACH。如果被动关闭端

假如 TCP 连接突然恢复连接，某端的进程收到了某个重传的 package，因为**不能识别所以会返回一个 RST**，此时被动关闭端进程上阻塞的 read 调用会返回错误 ECONNREST

socket对这些错误有一定的反馈能力的，前提是在**对面不可达时你做了一次write调用，而不是轮询或是阻塞在read上**，那么总是会在重传的周期内检测出错误。如果没有那次write调用，应用层永远不会收到连接错误的通知。

> 理解自[浅谈TCP/IP网络编程中socket的行为 - PromisE\_谢 - 博客园](https://www.cnblogs.com/promise6522/archive/2012/03/03/2377935.html)，还有待进一步消化。
## Socket 编程

创建一个 socket 并返回其句柄 socketfd，类似于 `open` 函数

一个 socket 就是一个抽象的五元组：

`socketfd ｜ 源ip ｜ 源端口 ｜ 目标ip ｜ 目标端口`

每个端口都能同时建立多条连接，每个源端口都能有多个 socket。

在一个多进程服务器中，我使用三个客户端和服务器进行交互。我想的是：每个 accept 都会返回一个新的 socketfd，它们的源端口都是不同的。但实际上，**accept 都会返回一个新的 socketfd，它们的源端口都是相同的**，但客户端的端口不同。

```text
guo@ubuntu:~/lean/IPC/socket$ netstat -t
Active Internet connections (w/o servers)
Proto Recv-Q Send-Q Local Address           Foreign Address         State
tcp        0      0 ubuntu:37488            ubuntu:8090             ESTABLISHED
tcp        0      0 ubuntu:ssh              192.168.10.1:53182      ESTABLISHED
tcp        0      0 ubuntu:44556            ubuntu:8090             ESTABLISHED
tcp        0      0 ubuntu:8090             ubuntu:44556            ESTABLISHED
tcp        0      0 ubuntu:8090             ubuntu:44562            ESTABLISHED
tcp        0      0 ubuntu:44562            ubuntu:8090             ESTABLISHED
tcp        0      0 ubuntu:8090             ubuntu:37488            ESTABLISHED
```
可以看到 8090 端口和其他三个端口同时建立了连接。

因此：**一个端口可以对应多个连接，只要这些连接具有不同的客户端 IP 地址和端口号。**

**不通的客户端和服务器进行交互，服务器的 ip 地址和目标端口都是同一个。**

### socket 函数
```c
int socket(int domain, int type, int protocal)
```
**参数[[Socket 编程]]**

- domain 参数用来指定协议族，比如
  - **AF_INET** 用于 IPV4
  - **AF_INET6** 用于 IPV6
  - **AF_LOCAL**/**AF_UNIX** 用于本机
- type 参数用来指定通信特性，比如 
  - **SOCK_STREAM** 表示的是字节流，对应 TCP。**返回的套接字是 TCP 套接字**
  - **SOCK_DGRAM** 表示的是数据报，对应 UDP。**返回的套接字是 UDP 套接字**
  - **SOCK_RAW** 表示的是原始套接字；
- protocal 参数原本是用来指定通信协议的，但现在基本废弃。因为协议已经通过前面两个参数指定完成，protocol 目前一般写成 0 即可；

**返回值**

- 成功：返回指向新创建的 socket 的文件描述符
- 失败：返回-1，设置 errno

### bind 函数


```c
#include <sys/types.h> /* See NOTES */
#include <sys/socket.h>
int bind(int sockfd, const struct sockaddr *addr, socklen_t addrlen);

```
**参数**

- sockfd
  **TCP/UDP 套接字**
- addr
   [[sockaddr 结构体|sockaddr 结构体]] ockaddr 地址结构体，使用方法：声明一个`sockadddr_in`类型的结构体，去地址，再强转成`struct sockaddr *` 
- addrlen
  sizeof (addr)长度

**返回值**

- 成功返回 0
- 失败返回-1, 设置 errno

**示例**
```c
struct sockaddr_in servaddr;
bzero(&servaddr, sizeof(servaddr));
servaddr.sin_family = AF_INET;                  // 协议族
inet_pton(AF_INET, "127.0.0.1", &servaddr.sin_addr);// ip地址
/*
也可以使用：
    servaddr.sin_addr.s_addr = inet_addr("127.0.0.1");
servaddr.sin_addr.s_addr = htonl(INADDR_ANY);
*/
servaddr.sin_port = htons(6666);                // 端口
bind(listenfd, (struct sockaddr *)&servaddr, sizeof(servaddr));
```
[[Linux ip 地址操作函数]]
### listen 函数

```c
#include <sys/types.h> /* See NOTES */
#include <sys/socket.h>
int listen(int sockfd, int backlog);
```
**参数**

- sockfd：**TCP 套接字**
- backlog：**backlog 为等待队列的最大长度**

**返回值**

- 成功 ： 0
- 失败 ： -1

当客户端尝试连接到服务器时，连接请求会首先进入等待队列，然后服务器可以使用 `accept()` 函数从队列中接受连接请求。如果等待队列已满，新的连接请求将会被忽略，直到队列中有空间为止。

### accept函数

```c
#include <sys/socket.h>
#include <netinet/in.h> 		
#include <sys/types.h>
int accept(int sockfd, struct sockaddr *addr, socklen_t *addrlen);
```
**参数**

- sockdf：TCP 套接字
- addr：传出参数，返回链接客户端地址信息，含 IP 地址和端口号
  *如果给 addr 参数传 NULL，表示不关心客户端的地址。*
- addrlen：传入传出参数（value-result argument）, **传入** sizeof (addr)大小*接收缓冲区的长度，避免缓冲区溢出*，函数返回时**传出**真正接收到地址结构体的大小

**返回值**

- 成功返回一个新的 socket 文件描述符，用于和客户端通信，
- 失败返回-1，设置 errno

三次握手完成后，服务器调用 accept()接受连接，如果服务器调用 accept()时还没有客户端的连接请求，就阻塞等待直到有客户端连接。

### connect函数

```c
#include <sys/types.h> 					/* See NOTES */
#include <netinet/in.h> 		
#include <sys/socket.h>
int connect(int sockfd, const struct sockaddr *addr, socklen_t addrlen);

```
**参数**

- sockdf：TCP 套接字
- addr：传入参数，指定服务器端地址信息，含 IP 地址和端口号
- addrlen：传入参数, 传入 `sizeof (addr)`大小

**返回值**

- 成功返回 0
- 失败返回-1，设置 errno

### sendto 函数
```c
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h> 		
ssize_t sendto(int sockfd, const void *buf, size_t len, int flags,
              const struct sockaddr *dest_addr, socklen_t addrlen);
```
**参数**

- socketfd：**UDP 套接字**
- buf：发送缓冲区
- len：需要发送的消息的长度
- flags：填0即可
- dest_addr：目的地主机的地址结构体
- addrlen：sizeof (addr)

**返回值**

- 成功：返回发送成功的数据长度
- 失败： -1

### recvfrom 函数

```c
#include <sys/types.h>
#include <netinet/in.h> 		
#include <sys/socket.h>
ssize_t recvfrom(int sockfd, void *buf, size_t len, int flags,
                struct sockaddr *src_addr, socklen_t *addrlen);
```
**参数**

- socketfd：**UDP 套接字**
- buf：接收缓冲区
- len：接收缓冲区的长度，防止缓冲区溢出
- flags：填 0 即可
- dest_addr：传出参数，函数返回后为源主机的地址结构体
- addrlen：传出参数，函数返回后为源主机的地址结构体长度
## 查看 Socket 状态

**netstat** 命令

- 查看所有 Socket 状态：`netstat -a`
- 查看监听状态的Socket：`netstat -l`
- 查看TCP Socket状态：`netstat -t`
- 查看 UDP Socket 状态：`netstat -u`

**结果中各字段含义**

Active Internet connections (servers and established)

- Proto                使用的协议
- Recv-Q               表示接收队列的字节数，即等待应用程序读取的数据量。
- Send-Q               表示发送队列的字节数，即等待发送到对端的数据量
- Local Address        本地IP地址和端口号
- Foreign Address      远端IP地址和端口号
- **State**

Active UNIX domain sockets (servers and established)

- RefCnt              表示套接字的引用计数，即有多少个进程正在引用这个套接字。
- Flags               显示套接字的标志。ACC/W/R/U
  - ACC                 表示套接字支持接受连接请求
  - W                   表示套接字支持写入操作
  - R                   表示套接字支持读取操作
  - U                   表示套接字是 Unconnected，通常用于数据报套接字（DGRAM）
- Type                通常为：`STREAM`或 `DGRAM`
- I-Node              套接字的 inode 
- Path                套接字的文件系统路径，用于定位套接字文件

### setsocketopt 函数

设置 socket 属性

最常用的属性: 端口可重用
    
```c
int flag = 1;
// 关闭后立即能够重用端口,不必处于TIME_WAIT 状态
setsockopt(listenfd, SOL_SOCKET, SO_REUSEADDR, &flag, sizeof(flag));
```
[setsockopt函数功能及参数详解 - CTHON - 博客园](https://www.cnblogs.com/cthon/p/9270778.html)
### TCP 状态转换图


![[TCP Socket 编程模型.png]]

- [ ] TCP 状态转换图

