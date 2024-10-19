## 原理

poll 不再用 BitMap 来存储所关注的文件描述符，取而代之用动态数组，以链表形式来组织，突破了 select 的文件描述符个数限制，当然还会受到系统文件描述符限制。
 
但是 poll 和 select 并没有太大的本质区别，

1. **都是使用线性结构 (BitMap/动态数组)存储进程关注的 Socket 集合，因此都需要遍历文件描述符集合来找到可读或可写的 Socket，时间复杂度为 O (n)
2. 而且也**需要在用户态与内核态之间拷贝文件描述符集合**，这种方式随着并发数上来，性能的损耗会呈指数级增长。
 
poll 没有最大连接数的限制，原因是它是基于链表来存储的
  
poll 是水平触发的。

# 使用
```c
#include <poll.h>
int poll(struct pollfd *fds, nfds_t nfds, int timeout);
struct pollfd {
    int fd; /* 文件描述符 */
	short events; /* 监控的事件 */
	short revents; /* 监控事件中满足条件返回的事件 */
};
```
**参数**

`pollfd.events` 相关宏

|**事件**|**事件说明**|
|---|---|
|**POLLIN**|文件描述符中有**数据可读**（包括普通数据或优先数据）|
|POLLRDNORM|文件描述符中有普通数据可读|
|POLLRDBAND|文件描述符中有优先数据可读|
|POLLPRI|文件描述符中高优先级数据可读|
|**POLLOUT**|文件描述符中**有数据可写**（包括普通数据或优先数据）|
|POLLWRNORM|文件描述符中可写入普通数据|
|POLLWRBAND|文件描述符中可写入优先数据|
|POLLERR|发生错误事件|
|POLLHUP|发生挂起事件|
|POLLNVAL|非法请求|

**nfds**

**poll 函数中的参数 nfds 等同于 select 函数中的参数 nfds**，用来设置 pollt 监控的文件描述符的范围，**需设置为文件描述符最大值加 1**

**timeout**
参数 timeout 与 select ()函数中的参数 timeout，都用于设置组设时长，但其取值略有差异，poll ()函数中参数 timeout 的取值及其对应含义如下：

- timeout = -1，poll 函数阻塞等待；

- timeout = 0，poll 函数将立即返回，以轮询的方式监测文件描述符表；

- timeout > 0，等待指定时长 (单位为毫秒)

**返回值**

- 成功，返回就绪文件描述符数量
- 等待超时，将返回 0，表示没有已就绪的文件描述符
- 调用出错，将返回 -1，并设置 errno。

