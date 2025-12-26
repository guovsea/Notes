## epoll 的使用场景

用户在访问某个网页时，并非每时每刻都和服务器进行交互。从网页服务器的角度看，虽然建立的连接数量可能非常多，但某一时刻和需要进行 IO 操作的连接数量可能并不多。**换言之，对某个网页的服务器来说，多路连接中活跃用户的数量可能远远小于连接的总数**。

假如使用 select 或 poll ，服务器大部分的时间都浪费在了遍历文件描述符集合查找就绪的文件描述符上面，真正处理请求的时间反而少之又少。而**epoll 模型非常适合用于这种活跃连接较少的服务器**。


## 使用

### epoll_create

1. 创建 epoll 句柄

```c
#include <sys/poll.h>
int epoll_create(int size);
```
**参数**

- size，最大监听数量

**返回值**

- 失败， -1
- 成功， 0

### epoll_ctl

2. 维护 epoll 的监听文件描述符集合，添加/删除集合中的文件描述符
```c
#include <sys/poll.h>
int epoll_ctl(int epfd, int op, int fd, struct epoll_event *event);
struct epoll_event {
  __uint32_t  events;   //epoll事件
  epoll_data_t data;    //用户数据变量
};
typedef union epoll_data {
  void    *ptr;
  int     fd;
  __uint32_t  u32;
  __uint64_t  u64;
} epoll_data_t;
```

**参数**

- epfd， `epoll_create`返回的 epoll 句柄
- op，控制 epoll_ctl 的功能
  - EPOLL_CTL_ADD
  - EPOLL_CTL_MOD
  - EPOLL_CTL_DEL
- event，**定义和 fd 关联的事件。当 fd 就绪时，内核就会将 event 拷贝到用户空间**。
  `event.events`：设置 fd 关联的事件类型，取值可以有

  | 取值             | 含义                               |
  | ---------------- | ---------------------------------- |
  | **EPOLLIN**      | **读事件**（包括 socket 正常关闭） |
  | **EPOLLOUT**     | **写事件**                         |
  | EPOLLPRI         | 优先可读事件                       |
  | EPOLLERR         | 错误事件                           |
  | EPOLLHUP         | 挂断事件                           |
  | **EPOLLET**      | **设置为边缘触发模式**             |
  | EPOLLONESHOT     | 只监听一次                         |
  | **`EPOLLRDHUP`** | 表示文件描述符的对端关闭连接（半关闭）      |

  **标准输入 STDIN 是可读的，EPOLLIN 就是可读事件**
  
  `event.data`：设置 fd 关联的内容
  **`event.data`的类型是`union`类型**。
  - `void *ptr`，关联一个通用指针，比如关联函数指针可以用来实现反应堆模型
  - `int fd;`，通常就是关联到 fd，然后对`event.data.fd`进行 write/read
  - `__uint32_t u32;`
  - `__uint64_t u64;`
  

### epoll_wait

3. 监听并返回就绪的`struct epoll_event`数组

```c
#include <sys/epoll.h>
	int epoll_wait(int epfd, struct epoll_event *events, int maxevents, int timeout)

```
**参数**

- epfd， epoll_create 的返回值
- events，**`struct epoll_event`类型的数组，保存就绪的 fd 所关联的`epoll_event`
- maxevents，告知内核 events 数组的大小，防止越界，不能超过`epoll_create`传入的 size
- timeout，超时时间
  - -1，阻塞
  - 0，	立即返回，非阻塞
  - > 0：	指定毫秒

**返回值**

- 成功，返回有多少文件描述符就绪
- 到达超时时间，返回 0
- 出错，返回-1

## LT 模式和 ET 模式

- 水平触发 (level-trggered)  
  - 读操作：**只要缓冲区有数据，epoll_wait 就会一直被触发，直到缓冲区为空**
  - 写操作：**只要缓冲区还不满**

- 边沿触发 (edge-triggered)
  - 读操作：
    1. 缓冲区中的数据从没有到有
    2. 缓冲区中的数据增加
    3. 缓冲区中有数据可读且对 socket 监听的事件类型被`EPOLL_CTL_MOD`为`EPOLLIN`
  - 写操作
    1. 缓冲区中的数据从满到不满（满则不可写，不满则可写）
    2. 缓冲区中的数据减少
    3. 缓冲区中有空间可写且对 socket 监听的事件类型被`EPOLL_CTL_MOD`为`EPOLLOUT`

测试代码

```c
#include <stdio.h>
#include <sys/epoll.h>
#include <unistd.h>

#define MAXPOLL 5

int main() {
    int efd, nfds;
    int fd;
    char buf[256];
    efd = epoll_create(MAXPOLL);
    fd = STDIN_FILENO;
    struct epoll_event tmp, events[MAXPOLL];
    tmp.events = EPOLLIN; // 默认水平触发，任意输入字符，无限循环打印
    // tmp.events = EPOLLIN | EPOLLET; // 边沿出发，任意输入字符，只打印一次  
    tmp.data.fd = fd;
    epoll_ctl(efd, EPOLL_CTL_ADD, fd, &tmp);
    while (1) {
        nfds = epoll_wait(efd, events, MAXPOLL, 0);
        for (int i = 0; i < nfds; i++) {
            if (events[i].data.fd == STDIN_FILENO) {
                printf("epoll again\n");
            }
        }
        // epoll_ctl(efd, EPOLL_CTL_ADD, fd, &tmp);
        // 任意输入字符，无限循环打印
    }
}
```

**边沿触发 + 非阻塞 IO**

边沿触发通常与非阻塞 IO 一起使用，其工作模式为：epoll_wait 触发一次，在 `while（1）`循环内非阻塞 IO 读取数据，直到缓冲区数据为空（保证了数据的完整性），内核才会继续调用 epoll_wait 等待事件发生。
## epoll 原理

相比 select/poll 的主动查询，**epoll 模型采用基于事件的通知方式**，事先为建立连接的文件描述符注册事件，一旦该文件描述符就绪，内核会采用类似 callback 的回调机制，将文件描述符加入到 epoll 的指定的文件描述符集中，之后进程再根据该集合中文件描述符的数量，对客户端请求逐一进行处理。

虽然 epoll 机制中返回的同样是就绪文件描述符的数量，但 **epoll 中的文件描述符集只存储了就绪的文件描述符**，服务器进程无需再扫描所有已连接的文件描述符；且 epoll 机制使用内存映射机制（类似共享内存），不必再将内核中的文件描述符集复制到内存空间；此外，epoll 机制不受进程可打开最大文件描述符数量的限制（只与系统内存有关），可连接远超过默认 FD_SETSIZE 的进程。

在使用 epoll 模型时，需要用`epoll_create`创建一个`struct eventpoll`的结构，用于管理。`eventpoll`跟`struct file`很类似，都是操作系统层面维护的一个数据结构，同样的，会有一个文件描述符引用它——`epoll_create`返回的`epfd`句柄。

### eventpoll 结构体

eventpoll 结构体中保存着几个重要的数据结构：

- **等待队列**，和唤醒进程有关
- **rdllist**，就绪的文件描述符表
- **rbr**，用于管理所有 fd 的红黑树（树根）
- **ovflist**，将事件到达的 fd 进行链接起来发送至用户空间
```c
struct eventpoll
{
    spin_lock_t       lock;         // 对本数据结构的访问
    struct mutex      mtx;          // 防止使用时被删除
    /*
     * 等待队列可以看作保存进程的容器，在阻塞进程时，将进程放入等待队列；
     * 当唤醒进程时，从等待队列中取出进程
     */ 
    wait_queue_head_t   wq;         // sys_epoll_wait() 使用的等待队列
    wait_queue_head_t   poll_wait;  // file->poll()使用的等待队列
    struct list_head    rdllist;    // 就绪链表
    struct rb_root      rbr;        // 用于管理所有fd的红黑树（树根）
    struct epitem      *ovflist;    // 将事件到达的fd进行链接起来发送至用户空间
}
```

### reactive 和 proactive

理解 epoll 原理之前，需要对 epoll 的设计模式有个顶层理解。**epoll 采用的设计模式是 reactive 模式**。

某个对象如何应对外部刺激、情境或需求有两种模式

- **reactive 模式**
  指对外部刺激或事件做出反应，而不是主动采取预防措施或计划，这类似于生病了才去医院。
- **proactive 模式**
  指在事件发生之前采取行动，以预防问题的出现或降低问题的影响，这类似于每天锻炼预防生病。

select/poll 采用的都是 proactive 模式，不断轮训主动查看每个 socket 是否就绪。epoll 采用的是 reactive 模式，**epoll 并不会轮训每个 socket，而是当某个 socket 就绪时，会调用中断程序将这个 socket 的引用加入到 rdllist**。


### epoll 中的红黑树的作用

**epoll 中使用红黑树来管理监视的 socket 和 `struct eopll_event`**。每个`eventpoll`对象管理着多个`struct epitem`，每个`epitem`都包括 fd 和`struct epoll_event`等信息。它需要进行包括一下的操作：

- EPOLL_CTL_ADD，添加
- EPOLL_CTL_DEL，删除
- EPOLL_CTL_MOD，修改

添加操作的大致流程为：利用`epoll_ctl`传入的 socketfd 作为键，创建一个`epitem`并插入到红黑树中。同时将`eventpoll`挂到 socket 的等待队列中。这样当 socket 就绪时，便能将 socket 的引用添加到**rdllist**中。

删除事件时，利用传入的 socketfd 作为键，找到`epitem`，索引到 socket，在删除`epitem`所指向的`eventpoll`对象，将 eventpoll 对象从 socket 的等待队列中删除。

因此必须使用能够高效**增删改查的数据结构**，Linux 使用的是红黑树。**红黑树的增删改查的时间复杂度都是`O(n)`的**。增删改查所使用的键就是 `epoll_ctl`传入的 fd。

*epoll 并不会遍历整个红黑树检查每个 socket 是否就绪，它仅仅只是用来管理{socket 引用 + events}这个数据结构的*

```c
struct epitem 
{
    struct rb_node  rbn;        // 用于主结构管理的红黑树
    struct list_head  rdllink;  // 事件就绪链表
    struct epitem  *next;       // 用于主结构体中的链表
 	struct epoll_filefd  ffd;   // 这个结构体对应的被监听的文件描述符信息
 	int  nwait;                 // poll操作中事件的个数
    struct list_head  pwqlist;  // 双向链表，保存着被监视文件的等待队列
    struct eventpoll  *ep;      // 该项属于哪个主结构体（多个epitm从属于一个eventpoll）
    struct list_head  fllink;   // 双向链表，用来链接被监视的文件描述符对应的struct
    struct epoll_event  event;  // 注册的感兴趣的事件，也就是用户空间的epoll_event
}
```
### epoll_wait

当程序执行`epoll_wait`系统调用时，如果 `rdlist` 已经引用了 `socket`，那么 `epoll_wait` 直接返回，如果 `rdlist` 为空，则会阻塞进程。 

`eventpoll` 对象也是文件系统中的数据结构，和 socket 一样，它有等待队列。当`eventpoll`的就绪队列`rdlist`不为空时，便会唤醒阻塞在`eventpoll`上的进程。

**epoll_wait 也会阻塞，但阻塞时并不是在轮询 socket，而是因为 rdllist 中并没有就绪的 socket**。


在理解上，讲得挺好：[如果这篇文章说不清epoll的本质，那就过来掐死我吧！ （3） - 知乎](https://zhuanlan.zhihu.com/p/64746509)
源码注释比较详细： [epoll源码重要部分详解\_struct epitem-CSDN博客](https://blog.csdn.net/daaikuaichuan/article/details/88770427)
有对源码的逻辑图，但不够详细，没太大看明白：[从linux源码看epoll - 无毁的湖光-Al的个人空间 - OSCHINA - 中文开源技术交流社区](https://my.oschina.net/alchemystar/blog/3008840)
